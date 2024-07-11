/*
 * CrossReferenceLinks.c
 *
 * maintain tags and do "under cursor actions"
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 28. Dec. 1990
 */

#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <string.h>
#include <stdio.h>
#include "alloc.h"
#include "pksmacro.h"
#include "funcdef.h"
#include "winterf.h"
#include "edctype.h"
#include "fileselector.h"
#include "caretmovement.h"
#include "pksedit.h"
#include "customcontrols.h"
#include "editorconfiguration.h"
#include "publicapi.h"
#include "edierror.h"
#include "errordialogs.h"
#include "xdialog.h"
#include "hashmap.h"
#include "dial2.h"
#include "grammar.h"
#include "winfo.h"
#include "findandreplace.h"
#include "linkedlist.h"
#include "stringutil.h"
#include "winfo.h"
#include "fileutil.h"
#include "winutil.h"
#include "crossreferencelinks.h"
#include "markpositions.h"
#include "textblocks.h"
#include "mainframe.h"
#include "codeanalyzer.h"
#include "dpisupport.h"
#include "pksrc.h"

typedef enum TAG_KIND {
	TK_FUNCTION = 'f', TK_MEMBER = 'm', TK_VARIABLE = 'v', TK_STRUCT = 's', TK_NUMBER_VALUE = 'n', TK_BOOLEAN_VALUE = 'b', TK_STRING_VALUE = 's', TK_OTHER
} TAG_KIND;

/*
 * Describes a xross-reference navigation request.
 */
typedef struct tagTAG_REFERENCE {
	struct tagTAG_REFERENCE* next;		// Tags with the same tag name are organized in linked lists.
	struct tagTAG* pTag;
	char* filename;						// The file to open
	BOOL isDefinition;					// Whether the reference describes the definition rather than the reference to a language artifact.
	BOOL isPrivate;						// Whether the described element is private or static (C)
	long ln;							// The line number to jump to - may be -1
	char* searchCommand;				// A regular expression to search - if the tag is found by searching
	TAG_KIND kind;						// The type of the element described.
	char *referenceDescription;			// A description where the described element can be found (e.g. member of struct xxx)
} TAG_REFERENCE;

/*
 * Describes a xross-reference navigation request.
 */
typedef struct tagTAG {
	char *tagname;						// The name of the tag (method, type, ...) itself.
	TAG_REFERENCE* tagReferences;		// The actual references describing the occurrences of the tag.
} TAG;

typedef struct tagNAVIGATION_SPEC {
	char filename[EDMAXPATHLEN];
	long line;
	char comment[128];
} NAVIGATION_SPEC;

typedef struct tagTAG_TABLE {
	HASHMAP* tt_map;
	GRAMMAR* tt_grammar;
	EDTIME tt_updated;
	char* tt_directory;
} TAG_TABLE;

static TAG_TABLE _allTags;

#define	TAGMAXTRY			5

extern FTABLE 	*ft_getCurrentErrorDocument();

static NAVIGATION_PATTERN _pksEditSearchlistFormat = {
0, "","\"([^\"]+)\", line ([0-9]+): *(.*)",	1,	2,	3 };
static NAVIGATION_PATTERN _universalCTagsFileFormat =  {
0, "", "^([^\t]+)\t([^\t]+)\t(.*);\"\t(.*)", 2, 3, 3, 1, 4 };
static NAVIGATION_PATTERN *_exprerror = &_pksEditSearchlistFormat;
static NAVIGATION_PATTERN *_tagfileFormatPattern  = &_universalCTagsFileFormat;
static NAVIGATION_PATTERN	*_compilerOutputNavigationPatterns;

static int xref_destroyCmpTag(NAVIGATION_PATTERN* ct) {
	free(ct->pattern);
	return 1;
}

static int xref_destroyTagReference(TAG_REFERENCE* pRef) {
	free(pRef->filename);
	free(pRef->searchCommand);
	free(pRef->referenceDescription);
	return 1;
}

static int xref_destroyTag(intptr_t key, intptr_t value) {
	TAG* tp = (TAG*)value;
	ll_destroy((LINKED_LIST**)&tp->tagReferences, xref_destroyTagReference);
	free(tp->tagname);
	free(tp);
	return 1;
}

static void xref_destroyTagTable() {
	hashmap_destroy(_allTags.tt_map, xref_destroyTag);
	_allTags.tt_map = NULL;
	free(_allTags.tt_directory);
	_allTags.tt_directory = NULL;
	_allTags.tt_grammar = NULL;
}

/*
 * Return the navigation pattern for a given compiler.
 */
static NAVIGATION_PATTERN* xref_getNavigationPatternFor(const char* pszCompiler) {
	NAVIGATION_PATTERN* pResult = ll_find((LINKED_LIST*)_compilerOutputNavigationPatterns, (char*)pszCompiler);
	return pResult ? pResult : _compilerOutputNavigationPatterns;
}

/*
 * Free all memory occupied by the cross reference lists.
 */
void xref_destroyAllCrossReferenceLists() {
	ll_destroy((LINKED_LIST**)&_compilerOutputNavigationPatterns, xref_destroyCmpTag);
	xref_destroyTagTable();
}

/*--------------------------------------------------------------------------
 * compiler_llinsert()
 */
static NAVIGATION_PATTERN* compiler_llinsert(void* pHead, int size, char* pszCompiler) {
	NAVIGATION_PATTERN* pNewPattern;


	if ((pNewPattern = (NAVIGATION_PATTERN * )ll_find(*(void**)pHead, pszCompiler)) == 0) {
		if ((pNewPattern = (NAVIGATION_PATTERN * )ll_insert(pHead, size)) == 0) {
			return 0;
		}
	}
	strncpy(pNewPattern->compiler, pszCompiler, sizeof(pNewPattern->compiler) - 1);
	return pNewPattern;
}

/*--------------------------------------------------------------------------
 * compiler_mk()
 */
static int compiler_mk(COMPILER_OUTPUT_PATTERN *pPattern) {
	NAVIGATION_PATTERN* ct;

	if ((ct = compiler_llinsert(&_compilerOutputNavigationPatterns,sizeof *ct, pPattern->cop_name)) == 0) {
		return 0;
	}
	ct->pattern = _strdup(pPattern->cop_pattern);
	ct->filenameCapture = pPattern->cop_filenameCapture;
	ct->lineNumberCapture = pPattern->cop_lineNumberCapture;
	ct->commentCapture= pPattern->cop_commentCapture;
	return 1;
}

/*--------------------------------------------------------------------------
 * xref_restoreFromConfigFile()
 * init all compiler types
 */
int xref_restoreFromConfigFile(void)
{
	COMPILER_OUTPUT_PATTERN* pPattern = GetConfiguration()->outputPatterns;
	while (pPattern) {
		if (!compiler_mk(pPattern)) {
			return 0;
		}
		pPattern = pPattern->cop_next;
	}
	return 1;
}

/*---------------------------------*/
/* xref_initializeNavigationPattern()				*/
/*---------------------------------*/
static RE_PATTERN *xref_initializeNavigationPattern(NAVIGATION_PATTERN *s) {
	static char ebuf[ESIZE];
	static RE_PATTERN pattern;
	_exprerror = s;
	return find_regexCompile(&pattern, ebuf,_exprerror->pattern,RE_DOREX);
}

/*---------------------------------*/
/* xref_readTagFile()					*/
/*---------------------------------*/
static int xref_readTagFile(char* fn, FTABLE* fp) {
	ln_listfree(fp->firstl);
	FILE_READ_OPTIONS fro;
	memset(&fro, 0, sizeof fro);
	fro.fro_fileName = fn;
	fro.fro_useDefaultDocDescriptor = 1;
	return ft_readfileWithOptions(fp, &fro);
}


/*---------------------------------*/
/* xref_loadTagFile()				*/
/*---------------------------------*/
static int xref_loadTagFile(FTABLE *fp, char* sourceFile, char *tagFilename) {
	char   *fn = NULL;
	char   dirname[512];

	if (sourceFile) {
		// tries to locate a tag file relative to a source file name (parent folder).
		string_splitFilename(sourceFile, dirname, NULL);
		while(dirname[0]) {
			size_t len = strlen(dirname);
			if (dirname[len - 1] == '\\' || dirname[len - 1] == '/') {
				dirname[len - 1] = 0;
			}
			fn = file_searchFileInDirectory(tagFilename, dirname);
			if (fn) {
				break;
			}
			string_splitFilename(dirname, dirname, NULL);
		}
	}
	if (fn == NULL && (fn = file_searchFileInPKSEditLocation(tagFilename)) == 0L) {
		return 0;
	}
	string_splitFilename(fn, dirname, NULL);
	if (_allTags.tt_directory != NULL && strcmp(dirname, _allTags.tt_directory) == 0) {
		return 1;
	}
	if (_allTags.tt_map) {
		// Tag index is recreated.
		xref_destroyTagTable();
	}
	_allTags.tt_directory = _strdup(dirname);
	if (file_exists(fn) != 0) {
		return 0;
	}
	return xref_readTagFile(fn,fp);
}

#define TAGLIST_COL_WIDTH_ICON			60
#define TAGLIST_COL_WIDTH_TAGNAME		160
#define TAGLIST_COL_WIDTH_FILE			280
#define TAGLIST_COL_WIDTH_DESCRIPTION	150

/*
 * Parse a tag definition from the tag file. If successful, return the tag definition, if not, return 0.
 */
static TAG* xref_parseTagDefinition(LINE* lp, RE_PATTERN* pattern) {
	RE_MATCH match;
	TAG_REFERENCE* pReference;
	static char extCommand[256];
	static char extCommandCopy[256];

	if (regex_match(pattern, lp->lbuf, &lp->lbuf[lp->len], &match)) {
		regex_getCapturingGroup(&match, _exprerror->tagCapture - 1, extCommand, sizeof extCommand);
		TAG* pTag = (TAG*) hashmap_get(_allTags.tt_map, extCommand);
		if (pTag == NULL) {
			pTag = calloc(1, sizeof *pTag);
			pTag->tagname = _strdup(extCommand);
			hashmap_put(_allTags.tt_map, pTag->tagname, (intptr_t)pTag);
		}
		pReference = ll_insert((void**) & pTag->tagReferences, sizeof * pReference);
		pReference->pTag = pTag;
		char* filename = calloc(1, EDMAXPATHLEN);
		regex_getCapturingGroup(&match, _exprerror->filenameCapture - 1, filename, EDMAXPATHLEN);
		pReference->filename = realloc(filename, strlen(filename)+1);
		if (pReference->filename == NULL) {
			free(filename);
			return 0;
		}
		memset(extCommand, 0, sizeof extCommand);
		regex_getCapturingGroup(&match, _exprerror->commentCapture - 1, extCommand, sizeof extCommand);
		if (extCommand[0] == '?' || extCommand[0] == '/') {
			char* pszSource = extCommand + 1;
			char* pszDest = extCommandCopy;
			while (*pszSource) {
				char c = *pszSource++;
				if (c == '/') {
					*pszDest = 0;
					break;
				}
				if (c == '\\') {
					c = *pszSource++;
				}
				else if (c == '*' || c == '+' || c == '?' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']') {
					*pszDest++ = '\\';
				}
				*pszDest++ = c;
			}
			*pszDest = 0;
			pReference->ln = -1;
			pReference->searchCommand = _strdup(extCommandCopy);
		} else {
			// lines are 0-based
			pReference->ln = (long)string_convertToLong(extCommand)-1l;
		}
		regex_getCapturingGroup(&match, _exprerror->tagExtensionFields - 1, extCommand, sizeof extCommand);
		pReference->kind = extCommand[0];
		char* pszRef = strtok(extCommand + 2, "\t");
		pReference->isDefinition = TRUE;
		while (pszRef) {
			if (strstr(pszRef, "roles:") == pszRef) {
				pReference->isDefinition = strcmp(pszRef+5,"def") == 0;
			}
			if (strstr(pszRef, "struct:") == pszRef || strstr(pszRef, "object:") == pszRef) {
				pReference->referenceDescription = _strdup(pszRef);
				break;
			}
			pszRef = strtok(NULL, "\t");
		}
		return pTag;
	}
	return 0;
}

/*
 * Read the current CTAGS file into our internal lookup index for fast access.
 * We try to locate the tags file relative to the source file passed with sourceFilename,
 * if that is non NULL.
 *
 * TODO: try to rebuild tag file index if required.
 */
static BOOL xref_buildTagTable(char* sourceFilename, char* baseTagFilename) {
	FTABLE ftable;
	LINE* lp;

	memset(&ftable, 0, sizeof ftable);
	if (xref_loadTagFile(&ftable, sourceFilename, baseTagFilename) == 0) {
		return FALSE;
	}
	if (!ftable.firstl) {
		// Everything up to date.
		return TRUE;
	}
	RE_PATTERN* pPattern = xref_initializeNavigationPattern(_tagfileFormatPattern);
	if (pPattern == NULL) {
		ft_bufdestroy(&ftable);
		return FALSE;
	}
	_allTags.tt_map = hashmap_create(997, NULL, NULL);
	find_initializeReplaceByExpression(_exprerror->pattern);	/* assure a few initialiations */
	for (lp = ftable.firstl; lp; lp = lp->next) {
		if (lp->len > 0 && lp->lbuf[0] == '!') {
			// Metatag lines in tag file format.
			continue;
		}
		xref_parseTagDefinition(lp, pPattern);
	}
	ft_bufdestroy(&ftable);
	return 1;
}

/*
 * Test whether an xref expression is matched.
 */
static char* _pszFilterString;
static HWND  _hwndDialog;
static TAG_REFERENCE* _pSelectReference;
static int xref_filter(intptr_t key) {
	char* pszTest = (char*)key;
	return string_strcasestr(pszTest, _pszFilterString) != NULL;
}

static HWND hwndList;
static void xref_addMessageItems(intptr_t k, intptr_t v) {
	LVITEM lvI = { 0 };
	lvI.stateMask = 0;
	TAG* tp = (TAG*)v;
	TAG_REFERENCE* pRef = tp ? tp->tagReferences : NULL;
	if (!_pSelectReference) {
		_pSelectReference = pRef;
	}
	int nCnt = 1;
	int nSelected = -1;
	while (pRef) {
		lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM | LVIF_IMAGE;
		lvI.iItem = nCnt;
		lvI.pszText = LPSTR_TEXTCALLBACK;
		lvI.lParam = (LPARAM)pRef;
		lvI.iSubItem = 0;
		lvI.iImage = pRef->isDefinition ? 1 : 0;
		lvI.state = 0;
		if (_pSelectReference == pRef) {
			lvI.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
			nSelected = nCnt;
			lvI.state = LVIS_SELECTED | LVIS_FOCUSED;
		}
		nCnt++;
		if (ListView_InsertItem(hwndList, &lvI) == -1) {
			return;
		}
		pRef = pRef->next;
	}
	if (nSelected >= 0) {
		ListView_EnsureVisible(
			hwndList,
			nSelected,
			0
		);
	}
}

/*------------------------------------------------------------
 * xref_fillTagList()
 */
static void xref_fillTagList(HWND hwnd, void* crossReferenceWord) {
	hwndList = GetDlgItem(hwnd, IDD_ICONLIST);
	ListView_DeleteAllItems(hwndList);

	_pszFilterString = crossReferenceWord;
	_hwndDialog = hwnd;
	TAG* tp = _allTags.tt_map ? (TAG*)hashmap_get(_allTags.tt_map, crossReferenceWord) : NULL;
	_pSelectReference = NULL;

	if (tp != NULL) {
		xref_addMessageItems((intptr_t)crossReferenceWord, (intptr_t)tp);
	} else if (_allTags.tt_map) {
		hashmap_forKeysMatching(_allTags.tt_map, xref_addMessageItems, xref_filter);
	}
}

static ANALYZER_CALLBACK _addCallback;
static void xref_processTag(intptr_t pszText, intptr_t pszVal) {
	_addCallback(&(ANALYZER_CALLBACK_PARAM) { .acp_recommendation = (const char*)pszText, .acp_object = (void*)pszVal });
}

/*
 * Iterate over all cross references defined for the grammar of the given editor window and
 * process all tags defined matching the text 'pszMatching'. Return 1 if successful.
 */
int xref_forAllTagsDo(WINFO* wp, int (*matchfunc)(const char* pszMatching), ANALYZER_CALLBACK cbCallback) {
	FTABLE* fp = wp->fp;
	GRAMMAR* pszGrammar = fp->documentDescriptor->grammar;
	if (_allTags.tt_map != NULL && pszGrammar != _allTags.tt_grammar) {
		xref_destroyTagTable();
	}
	if (_allTags.tt_map == NULL) {
		TAGSOURCE* ttl;
		if ((ttl = grammar_getTagSources(pszGrammar)) == NULL) {
			return 0;
		}
		while (ttl) {
			if (strcmp(TST_TAGFILE, ttl->type) == 0) {
				if (xref_buildTagTable(fp->fname, ttl->fn)) {
					_allTags.tt_grammar = pszGrammar;
					break;
				}
			}
			ttl = ttl->next;
		}
	}
	if (_allTags.tt_map != NULL) {
		_addCallback = cbCallback;
		hashmap_forKeysMatching(_allTags.tt_map, xref_processTag, (int(*)(intptr_t p))matchfunc);
		_addCallback = NULL;
		return 1;
	}
	return 0;
}

/*
 * Compare the tag list entries for sorting.
 */
static int taglist_compareItem(COMPAREITEMSTRUCT* cip) {
	TAG_REFERENCE* pRef1 = (TAG_REFERENCE * )cip->itemData1;
	TAG_REFERENCE* pRef2 = (TAG_REFERENCE*)cip->itemData2;

	int ret = strcmp(pRef1->pTag->tagname, pRef2->pTag->tagname);
	return ret;
}

static void xref_initTagListView(HWND hwndListView) {

	ListView_SetExtendedListViewStyleEx(hwndListView, LVS_EX_FULLROWSELECT| LVS_SHOWSELALWAYS, LVS_EX_FULLROWSELECT| LVS_SHOWSELALWAYS);
	HIMAGELIST hSmall;
	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		ILC_MASK, 1, 1);
	ImageList_AddIcon(hSmall, LoadIcon(hInst, "NEXT"));
	ImageList_AddIcon(hSmall, LoadIcon(hInst, "PREVIOUS"));
	ListView_SetImageList(hwndListView, hSmall, LVSIL_SMALL);
	LVCOLUMN lvc = { 
		.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
		.iSubItem = 0,
		.pszText = "Type",
		.fmt = LVCFMT_CENTER 
	};
	lvc.cx = dpisupport_getSize(50);
	ListView_InsertColumn(hwndListView, 0, &lvc);

	lvc.pszText = "Term";
	lvc.cx = dpisupport_getSize(200);
	lvc.iSubItem = 0;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(hwndListView, 1, &lvc);

	lvc.pszText = "File";
	lvc.cx = dpisupport_getSize(150);
	lvc.iSubItem = 0;
	ListView_InsertColumn(hwndListView, 2, &lvc);

	lvc.pszText = "Description";
	lvc.cx = dpisupport_getSize(350);
	lvc.iSubItem = 0;
	ListView_InsertColumn(hwndListView, 3, &lvc);
}

static void xref_getColumnParameters(NMLVDISPINFO* plvdi) {
	TAG_REFERENCE* pRef = (TAG_REFERENCE * )plvdi->item.lParam;
	char* pszComment;

	switch (plvdi->item.iSubItem) {
	case 1:
		plvdi->item.pszText = pRef->pTag->tagname;
		break;
	case 2:
		plvdi->item.pszText = string_abbreviateFileName(pRef->filename);
		break;
	case 3:
		pszComment = pRef->referenceDescription;
		if (!pszComment) {
			pszComment = pRef->searchCommand;
		}
		plvdi->item.pszText = pszComment;
		break;
	}
}

/*
 * Returns the selected element of a list view.
 */
static TAG_REFERENCE* xreflistview_getSelection(HWND hwnd) {
	int idx = ListView_GetNextItem(hwnd, -1, LVNI_SELECTED);
	if (idx >= 0) {
		LVITEM item;
		memset(&item, 0, sizeof item);
		item.iItem = idx;
		item.mask = LVIF_PARAM;
		if (ListView_GetItem(hwnd, &item)) {
			return (TAG_REFERENCE*)item.lParam;
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * xref_lookupTagReferenceProc()
 */
static TAG_REFERENCE* _selectedReference;
static INT_PTR CALLBACK xref_lookupTagReferenceProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	int					nNotify;
	int					idCtrl;
	HWND				hwndListView;

	switch (message) {
	case WM_INITDIALOG:
		hwndListView = GetDlgItem(hDlg, IDD_ICONLIST);
		xref_initTagListView(hwndListView);
		// This will trigger an EN_CHANGE filling the tag list.
		SetDlgItemText(hDlg, IDD_STRING1, (char*)lParam);
		_selectedReference = xreflistview_getSelection(hwndListView);
		PostMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hDlg, IDD_STRING1), TRUE);
		break;

	case WM_DESTROY: 
		hwndListView = GetDlgItem(hDlg, IDD_ICONLIST);
		HIMAGELIST hSmall = ListView_GetImageList(hwndListView, LVSIL_SMALL);
		if (hSmall) {
			ImageList_Destroy(hSmall);
		}
		return FALSE;

	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO) {
			NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
			xref_getColumnParameters(plvdi);
		}
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) {
			_selectedReference = xreflistview_getSelection(((LPNMHDR)lParam)->hwndFrom);
		}
		if (((LPNMHDR)lParam)->code == NM_DBLCLK) {
			EndDialog(hDlg, IDOK);
		}
		break;

	case WM_COMMAND:
		nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
		idCtrl = GET_WM_COMMAND_ID(wParam, lParam);
		if (idCtrl == IDOK || idCtrl == IDCANCEL) {
			EndDialog(hDlg, idCtrl);
			return TRUE;
		}
		if (idCtrl == IDD_STRING1) {
			if (nNotify == EN_CHANGE) {
				char szNewText[50];
				GetDlgItemText(hDlg, idCtrl, szNewText, sizeof szNewText);
				xref_fillTagList(hDlg, szNewText);
				return TRUE;
			}
		}
		break;
	}
	return dlg_defaultWndProc(hDlg, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * xref_selectTagsByDialog()
 */
static TAG_REFERENCE* xref_selectTagsByDialog(char* pTagName) {

	intptr_t ret = DialogBoxParam(ui_getResourceModule(), MAKEINTRESOURCE(DLGTAGLIST), hwndMain, xref_lookupTagReferenceProc, (LPARAM)pTagName);
	if (ret == IDCANCEL) {
		return 0;
	}
	return _selectedReference;
}

/*
 * xref_lookupTagReference()
 * Lookup the reference to a tag named 'tagName' from our currently loaded tag index.
 * If bExactMatch is false, all names from the index closely matching the passed
 * tag name are presented to the user for selection.
 */
static BOOL _tagCancelled;
static TAG_REFERENCE *xref_lookupTagReference(char *tagName, BOOL bForceDialog) {
	TAG* tp = _allTags.tt_map ? (TAG*)hashmap_get(_allTags.tt_map, tagName) : NULL;
	TAG_REFERENCE* pRef;

	if (!bForceDialog && tp && ll_size((LINKED_LIST*)tp->tagReferences) <= 1) {
		return tp->tagReferences;
	}
	pRef = xref_selectTagsByDialog(tagName);

	if (!pRef) {
		_tagCancelled = TRUE;
		return 0;
	}
	return pRef;
}

/**
 * xref_findExpressionCloseToCaret
 * Find an 'expression' (identifier) close to the caret in the current editor window.
 * Copy the found identifier into pszTargetBuffer and return the pointer to the start of
 * the expression in pszExpressionBegin.
 **/
static char* xref_findExpressionCloseToCaret(WINFO* wp, CARET* pCaret, unsigned char* pszTargetBuffer, unsigned char* pszTargetBufferEnd,
	unsigned char** pszExpressionBegin, unsigned char** pszExpressionEnd, int (*matchesCharacter)(unsigned char c), FIND_IDENTIFIER_OPTIONS fiOptions) {
	char* s, *pszStart, *pszEnd;
	char* pszCursor;
	char* s1 = pszTargetBuffer;

	LINE* lp = pCaret->linePointer;
	pszStart = lp->lbuf;
	s = &pszStart[pCaret->offset];
	pszEnd = &lp->lbuf[lp->len];
	pszCursor = s;

	if (fiOptions != FI_CURSOR_TO_END_WORD) {
		while (s > pszStart && (*matchesCharacter)(s[-1])) {
			s--;
		}
	}
	if (pszExpressionBegin) {
		*pszExpressionBegin = s;
	}

	while (s < pszEnd && (fiOptions != FI_BEGIN_WORD_TO_CURSOR || s < pszCursor) && (*matchesCharacter)(*s) && pszTargetBuffer < pszTargetBufferEnd) {
		*pszTargetBuffer++ = *s++;
	}
	if (pszExpressionEnd) {
		*pszExpressionEnd = s;
	}
	*pszTargetBuffer = 0;
	return(s1);
}

/*
 * Match an identifier as defined by the current language (grammar).
 */
static int xref_matchIdentifier(unsigned char c) {
	return isident(c);
}

/**
 * xref_findIdentifierCloseToCaret
 * Find an identifier close to the caret in the current editor window.
 * Copy the found identifier into pszTargetBuffer and return the pointer to the start of
 * the expression in pszExpressionBegin and return the end of the found expression in pszExpressionEnd.
 **/
char* xref_findIdentifierCloseToCaret(WINFO* wp, CARET* pCaret, unsigned char* pszTargetBuffer, unsigned char* pszTargetBufferEnd,
	unsigned char** pszExpressionBegin, unsigned char** pszExpressionEnd, FIND_IDENTIFIER_OPTIONS fiOptions) {

	return xref_findExpressionCloseToCaret(wp, pCaret, pszTargetBuffer, pszTargetBufferEnd, pszExpressionBegin, pszExpressionEnd,
		xref_matchIdentifier, fiOptions);
}

/*---------------------------------*/
/* xref_saveCrossReferenceWord()					*/
/*---------------------------------*/
static char *_tagword;
static char *xref_saveCrossReferenceWord(WINFO* wp, unsigned char *d,unsigned char *dend) {
	if (_tagword) {
		strcpy(d,_tagword);
		return _tagword;
	}
	return xref_findIdentifierCloseToCaret(wp, &wp->caret, d,dend,NULL, NULL, FI_COMPLETE_WORD);
}

/*------------------*/
/* xref_openFile()	*/
/*------------------*/
int xref_openFile(const char *name, long line, FT_OPEN_OPTIONS* pOptions) {
	int ret = 0;
	FT_OPEN_OPTIONS options = {
		.fo_codePage = -1
	};
	if (pOptions == NULL) {
		pOptions = &options;
	}
	if (ft_activateWindowOfFileNamed(name)) {
		if (line >= 0)
			ret = caret_placeCursorMakeVisibleAndSaveLocation(ww_getCurrentEditorWindow(), line,0L);
		else ret = 1;
	} else {
		pOptions->fo_isNewFile = -1;
		ret = ft_openFileWithoutFileselector(name, line, pOptions) != NULL;
		if (ret && line > 0) {
			ret = caret_placeCursorMakeVisibleAndSaveLocation(ww_getCurrentEditorWindow(), line, 0L);
		}
	}
	return ret;
}

/*
 * Navigate to a hyperlink.
 */
static int xref_navigateToHyperlink(char* urlSpec, char* pTag) {
	char buf[512];
	char* pszDollar = strstr(urlSpec, "$1");

	if (pszDollar) {
		size_t len = pszDollar - urlSpec;
		strncpy(buf, urlSpec, len);
		buf[len] = 0;
		strcat(buf, pTag);
		strcat(buf, pszDollar + 2);
		return ShellExecute(hwndMain, "open", buf, "", ".", SW_SHOWNORMAL) != NULL;
	}
	return 0;
}

/*---------------------------------*
 * xref_navigateCrossReference()
 * Navigate to the cross reference word given as an argument - no
 * selection of multiple matches yet.
 * If bNavigateWithoutDialogSelection is TRUE, potential single matches
 * are navigated right away. Otherwise a picker for the matches is
 * always presented to the user.
 *---------------------------------*/
static int xref_navigateCrossReferenceForceDialog(WINFO* wp, char *s, BOOL bForceDialog) {
	TAG_REFERENCE * tp;
	char     	buffer[EDMAXPATHLEN];
	RE_PATTERN  pattern;
	int			ret = 0;
	TAGSOURCE* ttl;
	FTABLE* fp;

	if (!s || wp == NULL)
		return 0;
	fp = wp->fp;
	find_setCurrentSearchExpression(s);
	_tagCancelled = FALSE;

	_tagword = _strdup(s);
	s = _tagword;
	ttl = grammar_getTagSources(fp->documentDescriptor->grammar);
	while (ttl && ret == 0 && _tagCancelled == FALSE) {
		if (strcmp(TST_TAGFILE, ttl->type) == 0) {
			if (xref_buildTagTable(fp->fname, ttl->fn) && (tp = xref_lookupTagReference(s, bForceDialog)) != 0L) {
				char* pszCompleteFile = file_searchFileInDirectory(tp->filename, _allTags.tt_directory);
				if (pszCompleteFile) {
					strcpy(buffer, pszCompleteFile);
				} else {
					strcpy(buffer, tp->filename);
				}
				fm_savepos(MTE_AUTO_LAST_SEARCH);
				if (!xref_openFile(buffer, tp->ln, NULL)) {
					error_showErrorById(IDS_FILE_NOT_FOUND, buffer);
					break;
				}
				if (tp->searchCommand && ft_getCurrentDocument()) {
					RE_PATTERN* pPattern;
					if ((pPattern = find_regexCompile(&pattern, buffer, tp->searchCommand, (int)RE_DOREX)) != NULL) {
						find_expressionInCurrentFile(wp, 1, tp->searchCommand, pPattern, RE_WRAPSCAN);
					}
				}
				ret = 1;
				break;
			}
		} else if (!bForceDialog && strcmp(TST_HYPERLINK, ttl->type) == 0) {
			ret = xref_navigateToHyperlink(ttl->fn, s);
		}
		ttl = ttl->next;
	}

	if (!ret && !_tagCancelled) {
		if (ttl == NULL) {
			error_showErrorById(IDS_NO_CROSS_REFERENCES_DEFINED);
		} else {
			error_showErrorById(IDS_MSGUNKNWONTAG, (LPSTR)s);
		}
	}
	free(_tagword);
	_tagword = 0;
	return ret;
}

/*---------------------------------*
 * xref_navigateCrossReference()
 * Navigate to the cross reference word given as an argument - no
 * selection of multiple matches yet.
 *---------------------------------*/
int xref_navigateCrossReference(char* s) {
	return xref_navigateCrossReferenceForceDialog(ww_getCurrentEditorWindow(), s, TRUE);
}

/*
 * Determine the "identifier" close to the input caret in the current editor window.
 * If text is selected, use that as the identifier, otherwise try to identify the close
 * by identifier.
 */
int xref_getSelectedIdentifier(WINFO* wp, char* pszText, size_t nMaxChars) {
	*pszText = 0;
	bl_getSelectedText(wp, pszText, 1, nMaxChars);
	if (!pszText[0]) {
		return xref_findIdentifierCloseToCaret(wp, &wp->caret, pszText, pszText + nMaxChars, NULL, NULL, FI_COMPLETE_WORD) != NULL;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * xref_openCrossReferenceList()
 */
int xref_openCrossReferenceList(WINFO* wp) {
	char selected[80];
	xref_getSelectedIdentifier(wp, selected, sizeof selected);
	return xref_navigateCrossReferenceForceDialog(wp, selected, TRUE);
}

/*
 * Parse a "navigation spec" (open file in line, with options...) from the output created either by a compiler
 * or from a PKS Edit grep result list.
 */
static BOOL xref_parseNavigationSpec(NAVIGATION_SPEC* pSpec, RE_PATTERN* pPattern, LINE* lp) {
	RE_MATCH match;
	char lineNumber[20];

	if (regex_match(pPattern, lp->lbuf, &lp->lbuf[lp->len], &match)) {
		regex_getCapturingGroup(&match, _exprerror->filenameCapture - 1, pSpec->filename, sizeof pSpec->filename);
		if (regex_getCapturingGroup(&match, _exprerror->commentCapture - 1, pSpec->comment, sizeof pSpec->comment) != SUCCESS) {
			pSpec->comment[0] = 0;
		}
		regex_getCapturingGroup(&match, _exprerror->lineNumberCapture - 1, lineNumber, sizeof lineNumber);
		if (lineNumber[0]) {
			pSpec->line = atol(lineNumber);
		}
		return TRUE;
	}
	return FALSE;
}

/*
 * Returns the search list format which can be used to parse PKS Edit search results.
 */
static NAVIGATION_PATTERN* xref_getSearchListFormat() {
	return &_pksEditSearchlistFormat;
}

/*
 * Returns a compiled RE_PATTERN to scan the lines in a search result list.
 * Note, that this method returns the pointer to a shared RE_PATTERN data structure
 * and should therefore only be used, if not in conflicts.
 */
RE_PATTERN* xref_compileSearchListPattern() {
	return xref_initializeNavigationPattern(xref_getSearchListFormat());
}

/*---------------------------------
 * xref_openWindowHistory()
 * Parse the search list result in the current line and try to navigate to
 * the file and line number which are obtained by parsing the line contents.
 *---------------------------------*/
void xref_openWindowHistory(LINE *lp) {
	NAVIGATION_SPEC spec;
	const char* pszName = NULL;
	RE_PATTERN *pPattern = xref_initializeNavigationPattern(xref_getSearchListFormat());
	WINFO* pActivate = NULL;
	BOOL bActive;

	while(lp != 0L) {		/* we may skip 1st line ! */
		if (lp->len && xref_parseNavigationSpec(&spec, pPattern, lp)) {
			bActive = FALSE;
			int nDisplayMode = -1;
			BOOL bClone = FALSE;
			BOOL bLink = FALSE;
			if (spec.comment[0]) {
				OPEN_HINT hHint = mainframe_parseOpenHint(spec.comment, FALSE);
				bActive = hHint.oh_activate;
				pszName = spec.comment;
				bClone = hHint.oh_clone;
				bLink = hHint.oh_link;
				nDisplayMode = hHint.oh_displayMode;
			}
			if (bClone) {
				OPEN_WINDOW_OPTIONS options = {
					.owo_dockName = pszName,
					.owo_linkWithExisting = bLink,
					.owo_preferredRendererMode = nDisplayMode == -1 ? 0 : nDisplayMode
				};
				ft_cloneWindowNamed(spec.filename, &options);
			} else {
				FT_OPEN_OPTIONS options = {
					.fo_dockName = pszName,
					.fo_codePage = -1
				};
				xref_openFile(spec.filename, spec.line - 1L, &options);
			}
			if (bActive || nDisplayMode != -1) {
				WINFO* wpThis = ww_getCurrentEditorWindow();
				if (wpThis) {
					if (bActive) {
						pActivate = wpThis;
						pActivate->workmode |= WM_PINNED;
					}
					if (nDisplayMode != -1 && !bClone) {
						ww_changeDisplayMode(wpThis, nDisplayMode);
					}
				}
			}
		}
		lp = lp->next;
	}
	if (pActivate) {
		ww_selectWindow(pActivate);
		op_updateall();
		pActivate->workmode &= ~WM_PINNED;
	}
}

/*
 * Highlight a match in a file opened as a result of a navigation.
 */
static void xref_highlightMatch(long ln, int col, int len) {
	WINFO* wpFound = ww_getCurrentEditorWindow();
	caret_placeCursorInCurrentFile(wpFound, ln, col);
	bl_hideSelection(wpFound, 1);
	CARET caret = wpFound->caret;
	bl_setSelection(wpFound, caret.linePointer, caret.offset, caret.linePointer, len + caret.offset);
	render_repaintCurrentLine(wpFound);
}

/*---------------------------------*/
/* xref_navigateSearchErrorList()				*/
/*---------------------------------*/
int xref_navigateSearchErrorList(int dir) {
	LINE*			lp;
 	NAVIGATION_SPEC navigationSpec;
	FTABLE	    	*fp = ft_getCurrentErrorDocument();
	int				compilerError = 0;
	NAVIGATION_PATTERN *pNavigationPattern = 0;
	char			fullname[EDMAXPATHLEN];
	WINFO			*wp;
	BOOL			bGoForward = FALSE;

	memset(&navigationSpec, 0, sizeof navigationSpec);
	if (fp) {
		pNavigationPattern = fp->navigationPattern;
		wp = WIPOI(fp);
	}

	if (fp == NULL || pNavigationPattern == 0) {
		error_showErrorById(IDS_MSGNOTAGFILE);
		return 0;
	}
	RE_PATTERN *pPattern = xref_initializeNavigationPattern(pNavigationPattern);

	switch (dir) {
	case LIST_PREV:
		lp = fp->lpReadPointer;
		if (lp) {
			lp = lp->prev;
		}
		break;
	case LIST_CURR:
		lp = wp->caret.linePointer;
		bGoForward = TRUE;
		break;
	case LIST_START:
		lp = fp->firstl;
		bGoForward = TRUE;
		break;
	case LIST_END:
		lp = fp->lastl;
		break;
	default:
		lp = fp->lpReadPointer;
		if (lp) {
			lp = lp->next;
		}
		bGoForward = TRUE;
		break;
	}
	if (bGoForward) {
		while (lp && (xref_parseNavigationSpec(&navigationSpec, pPattern, lp)) == FALSE) {
			lp = lp->next;
		}
	} else {
		while (lp && (xref_parseNavigationSpec(&navigationSpec, pPattern, lp)) == FALSE) {
			lp = lp->prev;
		}
	}
	if (lp && navigationSpec.filename[0]) {
		if ((wp = WIPOI(fp)) != 0) {
			long lineno = ln_indexOf(fp, lp);
			caret_placeCursorForFile(wp, lineno, 0, 0, 0);
			fp->lpReadPointer->lflg &= ~LNXMARKED;
			render_repaintLine(fp, fp->lpReadPointer);
			lp->lflg |= LNXMARKED;
			render_repaintLine(fp, lp);
		}
		fp->lpReadPointer = lp;
		/* make file name relativ to list file */
		if (navigationSpec.filename[0] == '/' || navigationSpec.filename[1] == ':') {
			lstrcpy(fullname, navigationSpec.filename);
		}
		else {
			string_splitFilename(fp->fname, fullname, (char*)0);
			string_concatPathAndFilename(fullname, fullname, navigationSpec.filename);
		}
		if (xref_openFile(fullname, navigationSpec.line - 1L, NULL)) {
			if (navigationSpec.comment[0]) {
				int col;
				int len = 0;
				if (sscanf(navigationSpec.comment, "%d/%d", &col, &len) == 2 && len > 0) {
					xref_highlightMatch(navigationSpec.line - 1L, col, len);
				}
			}
			return 1;
		}
		return 0;
	}
	error_showErrorById(compilerError ? IDS_MSGNOMOREERRS : IDS_NO_MATCHES_FOUND);
	return 0;
}

/*
 * Initialize a searchlist file.
 */
void xref_initSearchList(FTABLE* fp) {
	fp->flags |= F_WATCH_LOGFILE | F_TRANSIENT;
	WINFO* wp = WIPOI(fp);
	if (wp) {
		wp->workmode |= WM_PINNED|WM_LINE_SELECTION;
		strcpy(wp->actionContext, "search-list");
	}
	fp->navigationPattern = xref_getSearchListFormat();
	if (fp->firstl && fp->firstl->len > 5) {
		ft_setTitle(fp, fp->firstl->lbuf);
	}
}

/*---------------------------------*/
/* xref_openTagFileOrSearchResults()					*/
/*---------------------------------*/
#define	ST_TAGS		1
#define	ST_ERRORS	2
#define	ST_STEP		3
static FSELINFO _tagfselinfo = { ".", "tags", "*.tag" };
static FSELINFO _cmpfselinfo = { ".", "build.out", "*.out" };
static int xref_openTagFileOrSearchResults(int nCommand, int st_type, FSELINFO *fsp, FT_OPEN_OPTIONS* pOptions) {
	FILE_SELECT_PARAMS params;
	FTABLE* fp;
	memset(&params, 0, sizeof params);
	params.fsp_saveAs = FALSE;
	if (nCommand && fsel_selectFileWithOptions(fsp, nCommand, &params) == 0) {
		return 0;
	}

	switch(st_type) {
		case ST_ERRORS:
			if (!xref_openFile(_fseltarget, 0L, pOptions)) {
				break;
			}
			fp = ft_fpbyname(_fseltarget);
			if (fp) {
				EdRereadFileFromDisk(WIPOI(fp));
				// TODO: make this configurable - currently only used by PKSMacroC compiler.
				fp->navigationPattern = xref_getNavigationPatternFor("PKSMAKROC");
			}
			return xref_navigateSearchErrorList(LIST_START);
		case ST_STEP:
			fp = ft_openFileWithoutFileselector(_fseltarget, 0, pOptions);
			if (fp) {
				xref_initSearchList(fp);
				return xref_navigateSearchErrorList(LIST_START);
			}
			return 0;
		case ST_TAGS:
			return xref_buildTagTable(NULL, _fseltarget);
	}
	return 0;
}

static int xref_determineNavigationInfo(WINFO* wp, NAVIGATION_INFO_PARSE_RESULT* pResult, char* szFileBuffer, size_t nFileBufferSize) {
	LINE* lp = wp->caret.linePointer;
	FTABLE* fp = wp->fp;
	RE_PATTERN pattern;
	RE_OPTIONS options;
	RE_MATCH match;
	unsigned char patternBuf[256] = { 0 };

	if (wp->renderer->r_findLink) {
		return wp->renderer->r_findLink(wp, szFileBuffer, nFileBufferSize, pResult);
	}
	NAVIGATION_PATTERN* pPattern = grammar_getNavigationPatterns(fp->documentDescriptor->grammar);
	while (pPattern) {
		memset(&options, 0, sizeof options);
		options.flags = RE_DOREX;
		options.patternBuf = (char*) patternBuf;
		options.endOfPatternBuf = (char*) & patternBuf[sizeof patternBuf];
		options.expression = pPattern->pattern;
		if (regex_compile(&options, &pattern)) {
			char* pszStart = lp->lbuf;
			char* pszCursor = &lp->lbuf[wp->caret.col];
			char* pszEnd = &lp->lbuf[lp->len];
			while (regex_match(&pattern, (unsigned char*)pszStart, (unsigned char*)pszEnd, &match)) {
				if (match.loc1 <= pszCursor && match.loc2 >= pszCursor &&
						regex_getCapturingGroup(&match, pPattern->filenameCapture - 1, szFileBuffer, (int)nFileBufferSize) == SUCCESS) {
					pResult->ni_displayMode = -1;
					pResult->ni_reference = szFileBuffer;
					pResult->ni_lineNumber = 0;			// for now - to be initialized from capture group
					return 1;
				}
				pszStart = match.loc2;
			}
		}
		pPattern = pPattern->next;
	}
	return 0;
}

/*
 * Execute the given command line with shell execute. Display an error of failure and
 * return 0 in that case.
 */
static int xref_shellExecute(char* pszCommand) {
	HINSTANCE hInstance = ShellExecute(hwndMain, "open", pszCommand, "", ".", SW_SHOWNORMAL);
	if ((intptr_t)hInstance < 0 || (intptr_t)hInstance > 32) {
		return 1;
	}
	error_displayErrorInToastWindow("Cannot open %.512s", pszCommand);
	return 0;
}

/*--------------------------------*/
/* EdFindFileCursor() */
/*---------------------------------*/
int EdFindFileCursor(WINFO* wp)
{	char	*found;
	char	fselpath[128];
	char	currentFilePath[512];
	char	filename[128];
	NAVIGATION_INFO_PARSE_RESULT result;

	if (wp == NULL) {
		return 0;
	}
	memset(&result, 0, sizeof result);
	if (!xref_determineNavigationInfo(wp, &result, _fseltarget, EDMAXPATHLEN)) {
		return 0;
	}
	if (_fseltarget[0]) {
		FTABLE* fp = wp->fp;
		wp = NULL;
		string_splitFilename(_fseltarget, fselpath, filename);
		string_splitFilename(fp->fname, currentFilePath, NULL);
		if ((found = file_searchFileInPath(filename, GetConfiguration()->includePath)) != 0 ||
			(found = file_searchFileInPath(_fseltarget, currentFilePath)) != 0 ||
			(fselpath[0] && (found = file_searchFileInPath(_fseltarget, fselpath)) != 0)) {
			fm_savepos(MTE_AUTO_LAST_SEARCH);
			FT_OPEN_OPTIONS options = {
				.fo_dockName = result.ni_wp,
				.fo_codePage = -1
			};
			if (xref_openFile(found, result.ni_lineNumber, &options)) {
				// avoid to do a shell execute on the result - we have opened the file "internally".
				_fseltarget[0] = 0;
				wp = ww_getCurrentEditorWindow();
			}
		}
	}
	if (wp) {
		if (result.ni_displayMode != -1) {
			ww_changeDisplayMode(wp, result.ni_displayMode);
		}
		if (result.ni_anchor && wp->renderer->r_navigateAnchor) {
			wp->renderer->r_navigateAnchor(wp, result.ni_anchor);
		}
		return 1;
	}
	if (_fseltarget[0]) {
		xref_shellExecute(_fseltarget);
	}
	return 0;
}

/*
 * Simple URL encoding mechanism to encode searched text as URL parameter.
 */
static void xref_urlEncode(char* pszDest, char* pszSource) {
	unsigned char c;

	while ((c = *pszSource++) != 0) {
		if (c == ' ' || c == '\t') {
			*pszDest++ = '+';
		} else if (c == '+') {
			*pszDest++ = '%';
			*pszDest++ = '2';
			*pszDest++ = 'B';
		} else if (c == '%') {
			*pszDest++ = '%';
			*pszDest++ = '2';
			*pszDest++ = '5';
		} else if (c > 128) {
			*pszDest++ = '%';
			sprintf(pszDest, "%02x", (int)c);
			pszDest += 2;
		} else {
			*pszDest++ = c;
		}
	}
	*pszDest = 0;
}

/*
 * Perform a google / bing / duckduck go / ... search for the current word or selection.
 */
int EdFindOnInternet(WINFO* wp) {
	char buf[128];
	char command[512];

	xref_getSelectedIdentifier(wp, buf, sizeof buf);
	if (!*buf) {
		return 0;
	}
	char* pszCommand = config_getInternetSearchCommand();
	char* pszParam = strstr(pszCommand, "$1");
	if (pszParam == NULL) {
		// configuration error
		return 0;
	}
	int nOffs = (int)(pszParam - pszCommand);
	strncpy(command, pszCommand, nOffs);
	command[nOffs] = 0;
	xref_urlEncode(command + nOffs, buf);
	strcat(command, pszParam + 2);
	return xref_shellExecute(command);
}

/*---------------------------------*/
/* EdFindTagCursor()			*/
/*---------------------------------*/
int EdFindTagCursor(WINFO* wp)
{
	if (EdFindFileCursor(wp)) {
		return 1;
	}
	if (wp->renderer->r_findLink) {
		return 0;
	}
	return xref_navigateCrossReferenceForceDialog(wp, xref_saveCrossReferenceWord(wp, _linebuf, &_linebuf[LINEBUFSIZE]), FALSE);
}

/*---------------------------------*/
/* EdFindWordCursor() 			*/
/*---------------------------------*/
int EdFindWordCursor(WINFO* wp, int dir)
{	char buf[256];

	xref_getSelectedIdentifier(wp, buf, sizeof buf);
	RE_PATTERN *pPattern = regex_compileWithDefault(buf);
	return pPattern && find_expressionInCurrentFile(wp, dir, buf, pPattern, _currentSearchAndReplaceParams.options);
}

/*---------------------------------*/
/* xref_openSearchList()				*/
/*---------------------------------*/
int xref_openSearchList(char *fn, int cmpflg, long codePage) {
	strcpy(_fseltarget,fn);
	FT_OPEN_OPTIONS options = { DOCK_NAME_BOTTOM, codePage };
	return xref_openTagFileOrSearchResults(0, cmpflg ? ST_ERRORS: ST_STEP, &_cmpfselinfo, &options);
}

/*---------------------------------*/
/* EdSearchListRead() 			*/
/*---------------------------------*/
int EdSearchListRead(void)
{
	FSELINFO searchResultFileSpec = { "", "pksedit.grep", "*.grep" };
	strcpy(searchResultFileSpec.path, config_getPKSEditTempPath());
	FT_OPEN_OPTIONS options = { DOCK_NAME_BOTTOM, CP_ACP };
	return xref_openTagFileOrSearchResults(CMD_READ_SEARCH_LIST,ST_STEP,&searchResultFileSpec, &options);
}

/*---------------------------------*/
/* EdTagfileRead()				*/
/*---------------------------------*/
int EdTagfileRead(void)
{
	FT_OPEN_OPTIONS options = { DOCK_NAME_BOTTOM, CP_ACP };
	return xref_openTagFileOrSearchResults(CMD_READ_TAGFILE,ST_TAGS,&_tagfselinfo, &options);
}

/*---------------------------------*/
/* EdErrorListRead()			*/
/*---------------------------------*/
int EdErrorListRead(long dummy1, long dummy2) {
	FT_OPEN_OPTIONS options = { DOCK_NAME_BOTTOM, CP_ACP };
	return xref_openTagFileOrSearchResults(CMD_READ_COMPILER_ERRORS,ST_ERRORS,&_cmpfselinfo, &options);
}

