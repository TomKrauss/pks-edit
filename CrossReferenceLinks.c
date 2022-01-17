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
#include "edfuncs.h"
#include "winterf.h"
#include "edctype.h"
#include "fileselector.h"
#include "caretmovement.h"
#include "pksedit.h"
#include "customcontrols.h"
#include "editorconfiguration.h"
#include "edierror.h"
#include "errordialogs.h"
#include "xdialog.h"
#include "hashmap.h"
#include "dial2.h"
#include "grammar.h"
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
	EDTIME tt_updated;
	char* tt_directory;
} TAG_TABLE;

static TAG_TABLE _allTags;

#define	TAGMAXTRY			5

static FTABLE	_stepfile;
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
}

/*
 * Free all memory occupied by the cross reference lists. 
 */
void xref_destroyAllCrossReferenceLists() {
	ll_destroy((LINKED_LIST**)&_compilerOutputNavigationPatterns, xref_destroyCmpTag);
	xref_destroyTagTable();
}

/*------------------------------------------------------------------------
 * get_quoted()
 */
static char *get_quoted(char **src) {
	char *d,*s,*start;

	s = d = start = *src;
	while (*s) {
		if (*s == ',') {
			s++;
			break;
		}
		if (*s == '\\' && s[1])
			s++;
		*d++ = *s++;
	}
	*d = 0;
	*src = s;
	return start;
}

/*--------------------------------------------------------------------------
 * compiler_mk()
 */
static char *szCompiler = "compiler";
static intptr_t compiler_mk(char *compiler, LONG unused) {
	NAVIGATION_PATTERN* ct;
	char* s;
	char* pszLine;
	char* szNext;

	if ((ct = prof_llinsert(&_compilerOutputNavigationPatterns,sizeof *ct,szCompiler,compiler,&pszLine)) == 0) {
		return 0;
	}
	s = pszLine;
	ct->pattern = _strdup(get_quoted(&s));
	szNext = get_quoted(&s);
	ct->filenameCapture = szNext[0]-'0';
	szNext = get_quoted(&s);
	ct->lineNumberCapture = szNext[0] -'0';
	szNext = get_quoted(&s);
	ct->commentCapture= szNext[0] - '0';
	free(pszLine);
	return 1;
}

/*--------------------------------------------------------------------------
 * xref_restoreFromConfigFile()
 * init all compiler types
 */
int xref_restoreFromConfigFile(void)
{
	return prof_enum(szCompiler,compiler_mk,0L);
}

/*---------------------------------*/
/* xref_initializeNavigationPattern()				*/
/*---------------------------------*/
static RE_PATTERN *xref_initializeNavigationPattern(NAVIGATION_PATTERN *s) {
	static char ebuf[ESIZE];
	_exprerror = s;
	return find_regexCompile(ebuf,_exprerror->pattern,RE_DOREX);
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
	if (!file_exists(fn)) {
		return 0;
	}
	return xref_readTagFile(fn,fp);
}

/*------------------------------------------------------------
 * taglist_measureitem()
 */
#define		TAGLISTITEMHEIGHT			28
static void taglist_measureitem(MEASUREITEMSTRUCT *mp)
{
	mp->itemHeight = TAGLISTITEMHEIGHT;
	mp->itemWidth = 800;
}

#define TAGLIST_COL_WIDTH_ICON			60
#define TAGLIST_COL_WIDTH_TAGNAME		160
#define TAGLIST_COL_WIDTH_FILE			280
#define TAGLIST_COL_WIDTH_DESCRIPTION	150

/*------------------------------------------------------------
 * taglist_drawitem()
 */
static void taglist_drawitem(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl) {
	LONG			lExtent;
	WORD			hDelta;
	int				nIconDelta = TAGLIST_COL_WIDTH_ICON;
	int				nIconWidth = 16;
	TAG_REFERENCE *	tp = par;
	TAG* pTag;
	HICON			hIcon;

	if (par == 0) {
		return;
	}
	pTag = tp->pTag;
	lExtent = win_getTextExtent(hdc, pTag->tagname, (int)strlen(pTag->tagname));
	hDelta = HIWORD(lExtent);
	hDelta = (TAGLISTITEMHEIGHT - hDelta) / 2;
	char* pszFile = string_abbreviateFileName(tp->filename);
	TextOut(hdc, rcp->left + nIconDelta, rcp->top + hDelta, pTag->tagname, (int)strlen(pTag->tagname));
	TextOut(hdc, rcp->left + nIconDelta + TAGLIST_COL_WIDTH_TAGNAME, rcp->top + hDelta, pszFile, (int)strlen(pszFile));
	
	hIcon = LoadIcon (hInst, tp->isDefinition ? "NEXT" : "PREVIOUS");
	DrawIconEx(hdc,
		rcp->left + nIconWidth / 2,
		rcp->top + (rcp->bottom - rcp->top - nIconWidth) / 2,
		hIcon, nIconWidth, nIconWidth, 0, NULL, DI_NORMAL);
	char* pszComment = tp->referenceDescription;
	if (!pszComment) {
		pszComment = tp->searchCommand;
	}
	TextOut(hdc, rcp->left + nIconDelta + TAGLIST_COL_WIDTH_TAGNAME + TAGLIST_COL_WIDTH_FILE, 
		rcp->top + hDelta, pszComment, (int)strlen(pszComment));
	DestroyIcon(hIcon);
}

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
		TAG* pTag = (TAG*) hashmap_get(_allTags.tt_map, (intptr_t)extCommand);
		if (pTag == NULL) {
			pTag = calloc(1, sizeof *pTag);
			pTag->tagname = _strdup(extCommand);
			hashmap_put(_allTags.tt_map, (intptr_t)pTag->tagname, (intptr_t)pTag);
		}
		pReference = ll_insert(&pTag->tagReferences, sizeof * pReference);
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
			pReference->ln = -1;
			pReference->searchCommand = _strdup(extCommandCopy);
		} else {
			// lines are 0-based
			pReference->ln = string_convertToLong(extCommand)-1l;
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

static void xref_addMessageItems(intptr_t k, intptr_t v) {
	TAG* tp = (TAG*)v;
	TAG_REFERENCE* pRef = tp ? tp->tagReferences : NULL;
	if (!_pSelectReference) {
		_pSelectReference = pRef;
	}
	while (pRef) {
		SendDlgItemMessage(_hwndDialog, IDD_ICONLIST, LB_ADDSTRING, 0, (LPARAM)pRef);
		pRef = pRef->next;
	}
}
/*------------------------------------------------------------
 * xref_fillTagList()
 */
static void xref_fillTagList(HWND hwnd, void* crossReferenceWord) {
	_pszFilterString = crossReferenceWord;
	_hwndDialog = hwnd;
	TAG* tp = (TAG*)hashmap_get(_allTags.tt_map, (intptr_t)crossReferenceWord);
	_pSelectReference = NULL;
	SendDlgItemMessage(hwnd, IDD_ICONLIST, WM_SETREDRAW, FALSE, 0L);
	SendDlgItemMessage(hwnd, IDD_ICONLIST, LB_RESETCONTENT, 0, 0L);
	if (tp != NULL) {
		xref_addMessageItems((intptr_t)crossReferenceWord, (intptr_t)tp);
	} else {
		hashmap_forKeysMatching(_allTags.tt_map, xref_addMessageItems, xref_filter);
	}
	SendDlgItemMessage(hwnd, IDD_ICONLIST, WM_SETREDRAW, TRUE, 0L);
	SendDlgItemMessage(hwnd, IDD_ICONLIST, LB_SELECTSTRING, -1, (LPARAM)_pSelectReference);
}

/*
 * Iterate over all cross references defined for the grammar of the given editor window and
 * process all tags defined matching the text 'pszMatching'. Return 1 if successful.
 */
int xref_forAllTagsDo(WINFO* wp, char* pszMatching, void(*processTag)(intptr_t tagName, intptr_t tag)) {
	if (_allTags.tt_map == NULL) {
		TAGSOURCE* ttl;
		FTABLE* fp = wp->fp;
		if ((ttl = grammar_getTagSources(fp->documentDescriptor->grammar)) == NULL) {
			return 0;
		}
		while (ttl) {
			if (strcmp(TST_TAGFILE, ttl->type) == 0) {
				if (xref_buildTagTable(fp->fname, ttl->fn)) {
					break;
				}
			}
			ttl = ttl->next;
		}
	}
	if (_allTags.tt_map != NULL) {
		_pszFilterString = pszMatching;
		hashmap_forKeysMatching(_allTags.tt_map, processTag, xref_filter);
		_pszFilterString = NULL;
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

/*--------------------------------------------------------------------------
 * xref_lookupTagReferenceProc()
 */
static TAG_REFERENCE* _selectedReference;
static INT_PTR CALLBACK xref_lookupTagReferenceProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	MEASUREITEMSTRUCT* mp;
	COMPAREITEMSTRUCT* cp;
	int					nNotify;
	int					idCtrl;
	DRAWITEMSTRUCT* drp;

	switch (message) {
	case WM_INITDIALOG:
		win_moveWindowToDefaultPosition(GetParent(hDlg));
		SetDlgItemText(hDlg, IDD_STRING1, (char*)lParam);
		xref_fillTagList(hDlg, (char*)lParam);
		LRESULT idx = SendDlgItemMessage(hDlg, IDD_ICONLIST, LB_GETCURSEL, 0, 0);
		_selectedReference = idx < 0 ? NULL : (TAG_REFERENCE*)SendDlgItemMessage(hDlg, IDD_ICONLIST, LB_GETITEMDATA, idx, 0);
		PostMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hDlg, IDD_STRING1), TRUE);
		break;

	case WM_DESTROY:
		return FALSE;

	case WM_MEASUREITEM:
		mp = (MEASUREITEMSTRUCT*)lParam;
		taglist_measureitem(mp);
		return TRUE;

	case WM_COMPAREITEM:
		cp = (COMPAREITEMSTRUCT*)lParam;
		return taglist_compareItem(cp);

	case WM_DRAWITEM:
		drp = (DRAWITEMSTRUCT*)lParam;
		return cust_drawComboBoxOwnerDraw(drp, taglist_drawitem, NULL, FALSE);

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
		if (idCtrl == IDD_ICONLIST) {
			if (nNotify == LBN_SELCHANGE) {
				LRESULT idx = SendDlgItemMessage(hDlg, idCtrl, LB_GETCURSEL, 0, 0);
				if (idx >= 0) {
					_selectedReference = (TAG_REFERENCE*) SendDlgItemMessage(hDlg, idCtrl, LB_GETITEMDATA, idx, 0);
				}
			}
			if (nNotify == LBN_DBLCLK) {
				EndDialog(hDlg, IDOK);
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
	TAG* tp = _allTags.tt_map ? (TAG*)hashmap_get(_allTags.tt_map, (intptr_t) tagName) : NULL;
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
static char* xref_findExpressionCloseToCaret(unsigned char* pszTargetBuffer, unsigned char* pszTargetBufferEnd,
	unsigned char** pszExpressionBegin, unsigned char** pszExpressionEnd, int (*matchesCharacter)(unsigned char c), int bFindStartOfWord) {
	char* s, * pszStart;
	char* s1 = pszTargetBuffer;
	WINFO* wp;

	if ((wp = ww_getCurrentEditorWindow()) == 0L)
		return (char*)0;
	pszStart = wp->caret.linePointer->lbuf;
	s = &pszStart[wp->caret.offset];

	if (bFindStartOfWord) {
		while (s > pszStart && (*matchesCharacter)(s[-1])) {
			s--;
		}
	}
	if (pszExpressionBegin) {
		*pszExpressionBegin = s;
	}

	while ((*matchesCharacter)(*s) && pszTargetBuffer < pszTargetBufferEnd) {
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
char* xref_findIdentifierCloseToCaret(unsigned char* pszTargetBuffer, unsigned char* pszTargetBufferEnd,
	unsigned char** pszExpressionBegin, unsigned char** pszExpressionEnd, int bFindStartOfWord) {

	return xref_findExpressionCloseToCaret(pszTargetBuffer, pszTargetBufferEnd, pszExpressionBegin, pszExpressionEnd, 
		xref_matchIdentifier, bFindStartOfWord);
}

/*---------------------------------*/
/* xref_saveCrossReferenceWord()					*/
/*---------------------------------*/
static char *_tagword;
static char *xref_saveCrossReferenceWord(unsigned char *d,unsigned char *dend) {
	if (_tagword) {
		strcpy(d,_tagword);
		return _tagword;
	}

	return xref_findIdentifierCloseToCaret(d,dend,NULL, NULL, 1);
}

/*------------------*/
/* xref_openFile()	*/
/*------------------*/
int xref_openFile(char *name, long line, const char* pszHint) {
	int ret = 0;

	if (ft_activateWindowOfFileNamed(name)) {
		if (line >= 0)
			ret = caret_placeCursorMakeVisibleAndSaveLocation(ww_getCurrentEditorWindow(), line,0L);
		else ret = 1;
	} else {
		ret = ft_openFileWithoutFileselector(name, line, &(FT_OPEN_OPTIONS) { pszHint, -1 }) != NULL;
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
static int xref_navigateCrossReferenceForceDialog(char *s, BOOL bForceDialog) {
	TAG_REFERENCE * tp;
	char     	buffer[256];
	int			ret = 0;
	TAGSOURCE* ttl;
	FTABLE* fp;
	WINFO* wp = ww_getCurrentEditorWindow();

	if (!s || wp == NULL)
		return 0;
	fp = wp->fp;
	find_setCurrentSearchExpression(s);
	_tagCancelled = FALSE;

	if ((ttl = grammar_getTagSources(fp->documentDescriptor->grammar)) == NULL) {
		return 0;
	}
	_tagword = _strdup(s);
	s = _tagword;
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
				xref_openFile(buffer, tp->ln, NULL);
				if (tp->searchCommand && ft_getCurrentDocument()) {
					RE_PATTERN* pPattern;
					if (pPattern = find_regexCompile(buffer, tp->searchCommand, (int)RE_DOREX)) {
						find_expressionInCurrentFile(1, pPattern, O_WRAPSCAN);
					}
				}
				ret = 1;
				break;
			}
		} else if (strcmp(TST_HYPERLINK, ttl->type) == 0) {
			ret = xref_navigateToHyperlink(ttl->fn, s);
		}
		ttl = ttl->next;
	}

	if (!ret && !_tagCancelled) {
		error_showErrorById(IDS_MSGUNKNWONTAG, (LPSTR)s);
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
	return xref_navigateCrossReferenceForceDialog(s, TRUE);
}

/*
 * Determine the "identifier" close to the input caret in the current editor window.
 * If text is selected, use that as the identifier, otherwise try to identify the close
 * by identifier.
 */
void xref_getSelectedIdentifier(char* pszText, size_t nMaxChars) {
	bl_getSelectedText(pszText, nMaxChars);
	if (!pszText[0]) {
		xref_findIdentifierCloseToCaret(pszText, pszText + nMaxChars, NULL, NULL, 1);
	}

}

/*--------------------------------------------------------------------------
 * EdFindTag()
 */
int EdFindTag(void) {
	char selected[80];
	xref_getSelectedIdentifier(selected, sizeof selected);
	return xref_navigateCrossReferenceForceDialog(selected, TRUE);
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
 * xref_openSearchListResultFromLine()
 * Parse the search list result in the current line and try to navigate to
 * the file and line number which are obtained by parsing the line contents.
 *---------------------------------*/
void xref_openSearchListResultFromLine(FTABLE* fp, LINE *lp) { 	
	NAVIGATION_SPEC spec;
	const char* pszName = NULL;
	RE_PATTERN *pPattern = xref_initializeNavigationPattern(fp->navigationPattern ? fp->navigationPattern : xref_getSearchListFormat());
	WINFO* pActivate = NULL;
	BOOL bActive;
	BOOL bClone;

	while((lp = lp->next) != 0L) {		/* we may skip 1st line ! */
		if (lp->len && xref_parseNavigationSpec(&spec, pPattern, lp)) {
			bActive = FALSE;
			int nDisplayMode = -1;
			if (spec.comment[0]) {
				OPEN_HINT hHint = mainframe_parseOpenHint(spec.comment);
				bActive = hHint.oh_activate;
				pszName = spec.comment;
				bClone = hHint.oh_clone;
				nDisplayMode = hHint.oh_displayMode;
			}
			if (bClone) {
				ft_cloneWindowNamed(spec.filename, pszName);
			} else {
				xref_openFile(spec.filename, spec.line - 1L, pszName);
			}
			if (bActive || nDisplayMode != -1) {
				WINFO* wpThis = ww_getCurrentEditorWindow();
				if (wpThis) {
					if (bActive) {
						pActivate = wpThis;
						pActivate->workmode |= WM_STICKY;
					}
					if (nDisplayMode != -1) {
						ww_changeDisplayMode(wpThis, nDisplayMode);
					}
				}
			}
		}
	}
	if (pActivate) {
		ft_selectWindowWithId(pActivate->win_id, TRUE);
		pActivate->workmode &= ~WM_STICKY;
	}
}

/*
 * Highlight a match in a file opened as a result of a navigation. 
 */
static xref_highlightMatch(long ln, int col, int len) {
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
	if (dir & LIST_USETOPWINDOW) {
	/* treat current window as error list */
		dir &= ~LIST_USETOPWINDOW;
		WINFO* wp = ww_getCurrentEditorWindow();
		if (!wp) {
			fp = NULL;
		} else {
			fp = wp->fp;
			fp->navigationPattern = _compilerOutputNavigationPatterns;
		}
	}

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
	case LIST_NEXT:
		lp = fp->lpReadPointer;
		if (lp) {
			lp = lp->next;
		}
		bGoForward = TRUE;
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
			/* EdSelectWindow(wp->win_id); */
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
	error_showErrorById(compilerError ? IDS_MSGNOMOREERRS : 
					 IDS_MSGSTRINGNOTFOUND);
	return 0;
}

/*
 * Initialize a searchlist file. 
 */
void xref_initSearchList(FTABLE* fp) {
	fp->flags |= F_WATCH_LOGFILE | F_TRANSIENT;
	WINFO* wp = WIPOI(fp);
	if (wp) {
		wp->workmode |= WM_STICKY|WM_LINE_SELECTION;
		wp->controller = macro_getSearchListController();
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
static int xref_openTagFileOrSearchResults(int title, int st_type, FSELINFO *fsp) {
	FILE_SELECT_PARAMS params;
	FTABLE* fp;
	memset(&params, 0, sizeof params);
	params.fsp_saveAs = FALSE;
	if (title && fsel_selectFileWithOptions(fsp, title, &params) == 0) {
		return 0;
	}

	switch(st_type) {
		case ST_ERRORS:
			if (xref_openFile(_fseltarget, 0L, DOCK_NAME_BOTTOM) && ft_getCurrentDocument()) {
				EdFileAbandon();
			}
			return xref_navigateSearchErrorList(LIST_START|LIST_USETOPWINDOW);
		case ST_STEP:
			fp = ft_openFileWithoutFileselector(_fseltarget, 0, &(FT_OPEN_OPTIONS) { DOCK_NAME_BOTTOM, CP_ACP });
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
	unsigned char patternBuf[256];

	if (wp->renderer->r_findLink) {
		if (wp->renderer->r_findLink(wp, szFileBuffer, nFileBufferSize, pResult)) {
			return 1;
		}
	}
	NAVIGATION_PATTERN* pPattern = grammar_getNavigationPatterns(fp->documentDescriptor->grammar);
	while (pPattern) {
		memset(&options, 0, sizeof options);
		options.flags = RE_DOREX;
		options.patternBuf = patternBuf;
		options.endOfPatternBuf = &patternBuf[sizeof patternBuf];
		options.expression = pPattern->pattern;
		if (regex_compile(&options, &pattern)) {
			char* pszStart = lp->lbuf;
			char* pszCursor = &lp->lbuf[wp->caret.col];
			char* pszEnd = &lp->lbuf[lp->len];
			while (regex_match(&pattern, pszStart, pszEnd, &match)) {
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

/*--------------------------------*/
/* EdFindFileCursor() */
/*---------------------------------*/
int EdFindFileCursor(void)
{	char	*found;
	char	fselpath[128];
	char	currentFilePath[512];
	char	filename[128];
	NAVIGATION_INFO_PARSE_RESULT result;
	WINFO* wp = ww_getCurrentEditorWindow();
	extern char *file_searchFileInPath();

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
			if (xref_openFile(found, result.ni_lineNumber, result.ni_wp)) {
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
		HINSTANCE hInst = ShellExecute(hwndMain, "open", _fseltarget, "", ".", SW_SHOWNORMAL);
		if ((intptr_t)hInst < 0 || (intptr_t)hInst > 32) {
			return 1;
		}
		error_displayErrorInToastWindow("Cannot open %s", _fseltarget);
	}
	return 0;
}

/*---------------------------------*/
/* EdFindTagCursor()			*/
/*---------------------------------*/
int EdFindTagCursor(void)
{
	if (EdFindFileCursor()) {
		return 1;
	}
	return xref_navigateCrossReferenceForceDialog(xref_saveCrossReferenceWord(_linebuf, &_linebuf[LINEBUFSIZE]), FALSE);
}

/*---------------------------------*/
/* EdFindWordCursor() 			*/
/*---------------------------------*/
int EdFindWordCursor(dir)
{	char *s,buf[256];

	if ((s = xref_findIdentifierCloseToCaret(buf,&buf[sizeof buf],NULL, NULL, 0)) != 0L) {
		RE_PATTERN *pPattern = regex_compileWithDefault(s);
		return pPattern && find_expressionInCurrentFile(dir, pPattern, _currentSearchAndReplaceParams.options);
	}
	return 0;
}

/*---------------------------------*/
/* xref_openSearchList()				*/
/*---------------------------------*/
int xref_openSearchList(char *fn, int cmpflg)
{
	strcpy(_fseltarget,fn);
	return xref_openTagFileOrSearchResults(0, cmpflg ? ST_ERRORS: ST_STEP, &_cmpfselinfo);
}

/*---------------------------------*/
/* EdSearchListRead() 			*/
/*---------------------------------*/
int EdSearchListRead(void)
{
	FSELINFO searchResultFileSpec = { "", "pksedit.grep", "*.grep" };
	strcpy(searchResultFileSpec.path, config_getPKSEditTempPath());
	return xref_openTagFileOrSearchResults(MREAD_SEARCH_LIST,ST_STEP,&searchResultFileSpec);
}

/*---------------------------------*/
/* EdTagfileRead()				*/
/*---------------------------------*/
int EdTagfileRead(void)
{
	return xref_openTagFileOrSearchResults(MREADTAG,ST_TAGS,&_tagfselinfo);
}

/*---------------------------------*/
/* EdErrorListRead()			*/
/*---------------------------------*/
int EdErrorListRead(long dummy1, long dummy2) {
	return xref_openTagFileOrSearchResults(MREADCMP,ST_ERRORS,&_cmpfselinfo);
}

