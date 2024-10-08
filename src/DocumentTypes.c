/*
 *
 * DocumentTypes.c
 *
 * Project: PKS Edit for MS-WINDOWS
 *
 * purpose: maintain "Major Mode Configuration Files", in PKS Edit
 *		  also referred to as RULER Files,
 *		  link configuration files to file name extensions
 *		  tapstop calculating functions
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <CommCtrl.h>
#include <stddef.h>
#include <stdio.h>

#include "alloc.h"
#include "tos.h"
#include "arraylist.h"
#include "funcdef.h"
#include "jsonparser.h"
#include "linkedlist.h"
#include "documentmodel.h"
#include "grammar.h"
#include "fileselector.h"
#include "edierror.h"
#include "stringutil.h"
#include "pksedit.h"
#include "errordialogs.h"
#include "documenttypes.h"
#include "editorfont.h"
#include "fileutil.h"
#include "editorconfiguration.h"
#include "winfo.h"
#include "regexp.h"

 /*
 * Description of one document type in PKS edit.
 */
typedef struct tagDOCUMENT_TYPE {
	/*
	 * Next Document Type - they are maintained in a linked list.
	 */
	struct tagDOCUMENT_TYPE* ll_next;
	char				ll_name[32];						// name of the document type.
	char				ll_grammarScope[32];				// name of the grammar associated with this file type - is also used as primary language name.
	ARRAY_LIST* ll_languages;						// The list of alternative language names - if any.
	char				ll_description[80];					// Description for file selector.
	char				ll_editorConfigurationName[32];		// Editor configuration name
	char   				ll_match[64];						// file name pattern to match
	BOOL				ll_hideInFileSelector;				// Do not display file name pattern in list of file types in files selector.
	char				ll_firstlineMatch[64];				// Can be used to match a file by parsing the 1st line contained in that file.
	EDIT_CONFIGURATION* ll_documentDescriptor;
	HICON				ll_icon;							// The icon to use to represent this document.
} DOCUMENT_TYPE;

DOCUMENT_TYPE* doctypes_createDocumentType(DOCUMENT_TYPE* llp);

/*
 * Returns a default icon to display for editors not having an own icon defined by the Windows file type.
 */
extern HICON mainframe_getDefaultEditorIcon();

static FSELINFO _editConfigurationFileInfo = { "", "pkseditconfig.json", "*.json" };

#define	LINSPACE	offsetof(EDIT_CONFIGURATION, ts)

static JSON_MAPPING_RULE _documentTypeRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(DOCUMENT_TYPE, ll_name), sizeof(((DOCUMENT_TYPE*)NULL)->ll_name)},
	{	RT_STRING_ARRAY, "languages", offsetof(DOCUMENT_TYPE, ll_languages), sizeof(((DOCUMENT_TYPE*)NULL)->ll_languages)},
	{	RT_CHAR_ARRAY, "description", offsetof(DOCUMENT_TYPE, ll_description), sizeof(((DOCUMENT_TYPE*)NULL)->ll_description)},
	{	RT_CHAR_ARRAY, "grammar", offsetof(DOCUMENT_TYPE, ll_grammarScope), sizeof(((DOCUMENT_TYPE*)NULL)->ll_grammarScope)},
	{	RT_FLAG, "hideInFileSelector", offsetof(DOCUMENT_TYPE, ll_hideInFileSelector), 1},
	{	RT_CHAR_ARRAY, "editorConfiguration", offsetof(DOCUMENT_TYPE, ll_editorConfigurationName), sizeof(((DOCUMENT_TYPE*)NULL)->ll_editorConfigurationName)},
	{	RT_CHAR_ARRAY, "filenamePatterns", offsetof(DOCUMENT_TYPE, ll_match), sizeof(((DOCUMENT_TYPE*)NULL)->ll_match)},
	{	RT_CHAR_ARRAY, "firstlineMatch", offsetof(DOCUMENT_TYPE, ll_firstlineMatch), sizeof(((DOCUMENT_TYPE*)NULL)->ll_firstlineMatch)},
	{	RT_END}
};

static JSON_MAPPING_RULE _statuslineSegmentRules[] = {
	{	RT_CHAR_ARRAY,	"language", offsetof(STATUS_LINE_SEGMENT, sls_lang), sizeof(((STATUS_LINE_SEGMENT*)NULL)->sls_lang)},
	{	RT_INTEGER, "width", offsetof(STATUS_LINE_SEGMENT, sls_width), 0},
	{	RT_ALLOC_STRING,"text", offsetof(STATUS_LINE_SEGMENT, sls_text), sizeof(((STATUS_LINE_SEGMENT*)NULL)->sls_text)},
	{	RT_ALLOC_STRING,"icon", offsetof(STATUS_LINE_SEGMENT, sls_icon), sizeof(((STATUS_LINE_SEGMENT*)NULL)->sls_icon)},
	{	RT_COLOR,"iconColor", offsetof(STATUS_LINE_SEGMENT, sls_iconColor)},
	{	RT_END}
};

static JSON_MAPPING_RULE _editConfigurationRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(EDIT_CONFIGURATION, name), sizeof(((EDIT_CONFIGURATION*)NULL)->name)},
	{	RT_INTEGER, "id", offsetof(EDIT_CONFIGURATION, id)},
	{	RT_INTEGER, "leftMargin", offsetof(EDIT_CONFIGURATION, lmargin) },
	{	RT_INTEGER, "rightMargin", offsetof(EDIT_CONFIGURATION, rmargin)},
	{	RT_INTEGER, "tabSize", offsetof(EDIT_CONFIGURATION, tabsize)},
	{	RT_INTEGER, "codepage", offsetof(EDIT_CONFIGURATION, codepage)},
	{	RT_INTEGER_ARRAY, "tabulatorSizes", offsetof(EDIT_CONFIGURATION, tabulatorSizes), DIM(((EDIT_CONFIGURATION*)NULL)->tabulatorSizes)},
	{	RT_INTEGER, "shiftWidth", offsetof(EDIT_CONFIGURATION, shiftwidth)},
	{	RT_FLAG, "showWhitespace", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_CONTROL_CHARS},
	{	RT_FLAG, "hexMode", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_HEX_DISPLAY},
	{	RT_FLAG, "showRuler", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_RULER},
	{	RT_FLAG, "showLineNumbers", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_LINENUMBERS},
	{	RT_FLAG, "hideVerticalSlider", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_HIDE_VSLIDER},
	{	RT_FLAG, "hideHorizontalSlider", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_HIDE_HSLIDER},
	{	RT_FLAG, "showSyntaxHighlight", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_SYNTAX_HIGHLIGHT},
	{	RT_FLAG, "highlightCaretLine", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_CARET_LINE_HIGHLIGHT},
	{	RT_FLAG, "showWysiwyg", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_WYSIWYG_DISPLAY},
	{	RT_FLAG, "insertMode", offsetof(EDIT_CONFIGURATION, workmode), WM_INSERT},
	{	RT_FLAG, "autoIndent", offsetof(EDIT_CONFIGURATION, workmode), WM_AUTOINDENT},
	{	RT_FLAG, "fixCapitals", offsetof(EDIT_CONFIGURATION, workmode), WM_FIX_CAPITALS},
	{	RT_FLAG, "watchLogfile", offsetof(EDIT_CONFIGURATION, workmode), WM_WATCH_LOGFILE},
	{	RT_FLAG, "autoWrap", offsetof(EDIT_CONFIGURATION, workmode), WM_AUTOWRAP},
	{	RT_FLAG, "autoFormat", offsetof(EDIT_CONFIGURATION, workmode), WM_AUTOFORMAT},
	{	RT_FLAG, "expandAbbreviations", offsetof(EDIT_CONFIGURATION, workmode), WM_ABBREV},
	{	RT_FLAG, "oemEncoding", offsetof(EDIT_CONFIGURATION, workmode), WM_OEMMODE},
	{	RT_FLAG, "smartSpaceDelete", offsetof(EDIT_CONFIGURATION, workmode), WM_DELETE_MULTIPLE_SPACES},
	{	RT_OBJECT_LIST, "statuslineSegments", offsetof(EDIT_CONFIGURATION, statuslineSegments),
			{.r_t_arrayDescriptor = {0, _statuslineSegmentRules, sizeof(STATUS_LINE_SEGMENT)}}},
	{	RT_CHAR, "tabCharacter", offsetof(EDIT_CONFIGURATION, tabDisplayFillCharacter)},
	{	RT_CHAR, "fillCharacter", offsetof(EDIT_CONFIGURATION, expandTabsWith)},
	{	RT_CHAR_ZERO, "newlineCharacter", offsetof(EDIT_CONFIGURATION, nl)},
	{	RT_CHAR, "alternateNewlineCharacter", offsetof(EDIT_CONFIGURATION, nl2)},
	{	RT_CHAR_ZERO, "crCharacter", offsetof(EDIT_CONFIGURATION, cr)},
	{	RT_INTEGER, "scrollVerticallyBy", offsetof(EDIT_CONFIGURATION, vscroll)},
	{	RT_INTEGER, "scrollHorizontallyBy", offsetof(EDIT_CONFIGURATION, hscroll)},
	{	RT_INTEGER, "scrollVerticalBorder", offsetof(EDIT_CONFIGURATION, scroll_dy)},
	{	RT_INTEGER, "caretSearchPlacement", offsetof(EDIT_CONFIGURATION, cursaftersearch)},
	{	RT_FLAG, "scrollSupportTrackThumb", offsetof(EDIT_CONFIGURATION, scrollflags), SC_THUMBTRACK},
	{	RT_FLAG, "caretPreserveColumn", offsetof(EDIT_CONFIGURATION, dispmode), SHOW_CARET_PRESERVE_COLUMN},
	{	RT_FLAG, "caretFollowsScrollbar", offsetof(EDIT_CONFIGURATION, scrollflags), SC_CURSORCATCH},
	{	RT_CHAR_ARRAY, "backupExtension", offsetof(EDIT_CONFIGURATION, backupExtension), sizeof(((EDIT_CONFIGURATION*)NULL)->backupExtension)},
	{	RT_CHAR_ARRAY, "executeOnLoad", offsetof(EDIT_CONFIGURATION, createActionName), sizeof(((EDIT_CONFIGURATION*)NULL)->createActionName)},
	{	RT_CHAR_ARRAY, "executeOnSave", offsetof(EDIT_CONFIGURATION, saveActionName), sizeof(((EDIT_CONFIGURATION*)NULL)->saveActionName)},
	{	RT_CHAR_ARRAY, "actionContext", offsetof(EDIT_CONFIGURATION, actionContext), sizeof(((EDIT_CONFIGURATION*)NULL)->actionContext)},
	{	RT_END}
};

typedef struct tagDOCTYPE_CONFIGURATION {
	EDIT_CONFIGURATION* dc_editorConfigurations;
	DOCUMENT_TYPE* dc_types;
	EDIT_CONFIGURATION* dc_defaultEditorConfiguration;
} DOCTYPE_CONFIGURATION;

static const char* DEFAULT = "default";

static JSON_MAPPING_RULE _doctypeConfigurationRules[] = {
	{	RT_OBJECT_LIST, "documentTypes", offsetof(DOCTYPE_CONFIGURATION, dc_types),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _documentTypeRules, .ro_nestedSize = sizeof(DOCUMENT_TYPE)}}},
	{	RT_OBJECT_LIST, "editorConfigurations", offsetof(DOCTYPE_CONFIGURATION, dc_editorConfigurations),
			{.r_t_arrayDescriptor = {doctypes_createDefaultDocumentTypeDescriptor, _editConfigurationRules}}},
	{	RT_END}
};

static DOCTYPE_CONFIGURATION config;

static BOOL doctypes_destroySegment(STATUS_LINE_SEGMENT* pSegment) {
	free(pSegment->sls_icon);
	free(pSegment->sls_text);
	return TRUE;
}

/*
 * Return the mapping rules for the document types and editor configurations.
 */
JSON_MAPPING_RULE* doctypes_getJsonMapping() {
	return _doctypeConfigurationRules;
}

/**
 * Return the file icon to use to display this document type.
 */
HICON doctypes_getFileIcon(DOCUMENT_TYPE* pType) {
	if (pType->ll_icon == NULL) {
		SHFILEINFO sfi = { 0 };
		SHGetFileInfo(pType->ll_match, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof sfi, SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES);
		if (sfi.hIcon != NULL) {
			pType->ll_icon = sfi.hIcon;
		}
		else {
			pType->ll_icon = mainframe_getDefaultEditorIcon();
		}
	}
	return pType->ll_icon;
}

/*
 * Copy a document descriptor from a source to a target.
 */
static void doctypes_copyDescriptor(EDIT_CONFIGURATION* pDest, EDIT_CONFIGURATION* pSource) {
	if (pDest == pSource) {
		return;
	}
	if (pDest->statuslineSegments != NULL) {
		ll_destroy(&pDest->statuslineSegments, doctypes_destroySegment);
	}
	memmove(pDest, pSource, sizeof * pSource);
	pDest->statuslineSegments = NULL;
	STATUS_LINE_SEGMENT* pSegment = pSource->statuslineSegments;
	while (pSegment) {
		STATUS_LINE_SEGMENT* pNew = calloc(sizeof * pNew, 1);
		if (pNew == NULL) {
			break;
		}
		memcpy(pNew, pSegment, sizeof * pSegment);
		if (pSegment->sls_icon != NULL) {
			pNew->sls_icon = _strdup(pSegment->sls_icon);
		}
		if (pSegment->sls_text != NULL) {
			pNew->sls_text = _strdup(pSegment->sls_text);
		}
		ll_add(&pDest->statuslineSegments, (LINKED_LIST*)pNew);
		pSegment = pSegment->sls_next;
	}
}

/*--------------------------------------------------------------------------
 * Creates the default attributes for editing a document. The returned structure
 * must be freed, when done using it.
 */
EDIT_CONFIGURATION* doctypes_createDefaultDocumentTypeDescriptor() {
	EDIT_CONFIGURATION* pDescriptor = calloc(1, sizeof * pDescriptor);

	if (pDescriptor == NULL) {
		return NULL;
	}
	pDescriptor->rmargin = 80;
	pDescriptor->tabsize = 8;
	pDescriptor->shiftwidth = 4;
	pDescriptor->workmode = WM_INSERT;
	pDescriptor->codepage = CP_ACP;
	pDescriptor->cr = '\r';
	pDescriptor->nl = '\n';
	pDescriptor->tabDisplayFillCharacter = ' ';
	pDescriptor->actionContext[0] = 0;
	return pDescriptor;
}

/*--------------------------------------
 * Returns the default document type descriptor for situations,
 * where no document type descriptor was assigned to a file.
 */
EDIT_CONFIGURATION* doctypes_getDefaultDocumentTypeDescriptor() {
	return config.dc_defaultEditorConfiguration;
}

/*--------------------------------------------------------------------------
 * doctypes_getSelectableDocumentFileTypes()
 * Returns the list of document file types defined in PKS Edit in the format
 * that it can be passed on to the open file dialog for filtering of file types
 * (e.g. *.*\0All Files\0*.java\0Java Files\0)
 */
void doctypes_getSelectableDocumentFileTypes(char* pszDest, int nMax) {
	LPSTR		pszEnd;
	DOCUMENT_TYPE* llp;
	int			nCopied;

	pszEnd = pszDest + nMax - 2;
	for (nCopied = 0, llp = config.dc_types; llp != 0; llp = llp->ll_next) {
		if (llp->ll_hideInFileSelector) {
			continue;
		}
		if (llp->ll_match[0]) {
			if (nCopied > 0) {
				*pszDest++ = 0;
			}
			pszDest = strmaxcpy(pszDest,
				llp->ll_description[0] ?
				llp->ll_description : llp->ll_match,
				(int)(pszEnd - pszDest));
			if (pszDest >= pszEnd) {
				break;
			}
			*pszDest++ = 0;
			pszDest = strmaxcpy(pszDest, llp->ll_match,
				(int)(pszEnd - pszDest));
			if (pszDest >= pszEnd) {
				break;
			}
			nCopied++;
		}
	}
	*pszDest = 0;
}

/*--------------------------------------------------------------------------
 * doctypes_addDocumentTypesToListView()
 * Adds all document types to a list view.
 */
int doctypes_addDocumentTypesToListView(HWND hwndList, const void* pSelected) {
	ListView_DeleteAllItems(hwndList);
	DOCUMENT_TYPE* llp;
	HIMAGELIST hSmall;
	int			nCnt;

	LVITEM lvI = {
		.stateMask = LVIS_SELECTED | LVIS_FOCUSED,
		.state = 0
	};
	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		ILC_MASK, 1, 1);
	int nSelected = -1;
	const DOCUMENT_TYPE* pSelectedDocumentType = pSelected;
	for (llp = config.dc_types, nCnt = 0; llp != 0; llp = llp->ll_next) {
		lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM | LVIF_IMAGE;
		lvI.iItem = nCnt;
		lvI.pszText = LPSTR_TEXTCALLBACK;
		lvI.lParam = (LPARAM)llp;
		lvI.iSubItem = 0;
		lvI.iImage = nCnt;
		if (pSelectedDocumentType != NULL && strcmp(pSelectedDocumentType->ll_name, llp->ll_name) == 0) {
			nSelected = nCnt;
		}
		HICON hIcon = doctypes_getFileIcon(llp);
		ImageList_AddIcon(hSmall, hIcon);
		if (ListView_InsertItem(hwndList, &lvI) == -1) {
			return FALSE;
		}
		nCnt++;
	}
	ListView_SetImageList(hwndList, hSmall, LVSIL_SMALL);
	if (nSelected >= 0) {
		ListView_SetItemState(hwndList, nSelected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		ListView_EnsureVisible(hwndList, nSelected, TRUE);
	}
	return nCnt;
}

/*--------------------------------------------------------------------------
 * doctypes_getDocumentTypeDescription()
 * Returns the properties for a document type (primarily for the purpose of displaying
 * it in a UI).
 */
BOOL doctypes_getDocumentTypeDescription(DOCUMENT_TYPE* llp,
	char** ppszId, char** ppszDescription, char** ppszMatch, char** ppszEditorConfigName,
	char** ppszGrammar, BOOL** bHideInFileSelector) {

	if (!llp) {
		llp = config.dc_types;
	}
	if (!llp) {
		doctypes_createDocumentType((DOCUMENT_TYPE*)0);
		llp = config.dc_types;
	}
	if (llp == NULL) {
		return FALSE;
	}
	if (ppszId) {
		*ppszId = llp->ll_name;
	}
	if (ppszDescription) {
		*ppszDescription = llp->ll_description;
	}
	if (ppszMatch) {
		*ppszMatch = llp->ll_match;
	}
	if (ppszEditorConfigName) {
		*ppszEditorConfigName = llp->ll_editorConfigurationName;
	}
	if (ppszGrammar) {
		*ppszGrammar = llp->ll_grammarScope;
	}
	if (bHideInFileSelector) {
		*bHideInFileSelector = &llp->ll_hideInFileSelector;
	}
	return TRUE;
}

/*
 * Try to match the 1st line in a document against a RE. If it matches, assume a special document type.
 */
static BOOL doctypes_matchLine(const char* pszFirstLine, size_t nFirstLineLen, const char* pszFirstlineMatch) {
	if (!*pszFirstlineMatch) {
		return FALSE;
	}
	RE_PATTERN pattern;
	UCHAR 	ebuf[ESIZE];
	RE_MATCH reMatch;
	RE_OPTIONS options;
	memset(&options, 0, sizeof options);
	memset(ebuf, 0, ESIZE);
	options.patternBuf = ebuf;
	options.flags = RE_DOREX;
	options.endOfPatternBuf = &ebuf[ESIZE];
	options.expression = (char*)pszFirstlineMatch;
	if (regex_compile(&options, &pattern) && regex_match(&pattern, pszFirstLine, &pszFirstLine[nFirstLineLen], &reMatch)) {
		return TRUE;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * doctypes_getFileDocumentType()
 * find the correct document descriptor for a given file.
 * 	1. if own document descriptor, try to read  own document descriptor from disc
 * 	2. if common document descriptor try to find a match via name pattern or a match of the first line (if defined - maybe NULL).
 *	3. if neither, use standard document descriptor
 */
static BOOL doctypes_getFileDocumentType(EDIT_CONFIGURATION* pEditConfiguration, char*pDocumentTypeName, 
			char * filename, const char* pszFirstLine, size_t nFirstLineLen) {
	char 	fname[EDMAXPATHLEN];
	DOCUMENT_TYPE* pDocumentType;
	EDIT_CONFIGURATION* lp;

	int nRanking = 100;
	int nMatchRanking;
	DOCUMENT_TYPE* pFound = NULL;
	if (pDocumentTypeName != NULL) {
		for (pDocumentType = config.dc_types; pDocumentType != 0; pDocumentType = pDocumentType->ll_next) {
			if (strcmp(pDocumentTypeName, pDocumentType->ll_name) == 0) {
				pFound = pDocumentType;
				break;
			}
		}
	}
	if (pFound == NULL) {
		string_splitFilename(filename, (char*)0, fname, sizeof fname);
		for (pDocumentType = config.dc_types, lp = 0; pDocumentType != 0 && lp == 0; pDocumentType = pDocumentType->ll_next) {
			if (pszFirstLine && doctypes_matchLine(pszFirstLine, nFirstLineLen, pDocumentType->ll_firstlineMatch)) {
				// prefer over file name matching
				pFound = pDocumentType;
				break;
			}
			if (string_matchFilename(fname, pDocumentType->ll_match)) {
				// Select most explicit file name pattern - e.g. prefer *.c over *.* and prefer *.cpp over *.cpp;*.h
				if (strstr(pDocumentType->ll_match, "*.*")) {
					nMatchRanking = 3;
				}
				else if (strstr(pDocumentType->ll_match, "*")) {
					nMatchRanking = 2;
				}
				else if (strstr(pDocumentType->ll_match, "?")) {
					nMatchRanking = 1;
				}
				else {
					nMatchRanking = 0;
				}
				if (nMatchRanking < nRanking) {
					nRanking = nMatchRanking;
					pFound = pDocumentType;
				}
			}
		}
	}
	if (pFound) {
		lp = pFound->ll_documentDescriptor;
		pDocumentType = pFound;
	}
	if (lp == NULL && config.dc_defaultEditorConfiguration != NULL) {
		lp = config.dc_defaultEditorConfiguration;
	}
	if (!lp) {
		EDIT_CONFIGURATION* pDefaultConfiguration = doctypes_createDefaultDocumentTypeDescriptor();
		doctypes_copyDescriptor(pEditConfiguration, pDefaultConfiguration);
		free(pDefaultConfiguration);
	} else {
		doctypes_copyDescriptor(pEditConfiguration, lp);
	}
	if (pDocumentType && pDocumentType->ll_grammarScope[0]) {
		pEditConfiguration->grammar = grammar_findNamed(pDocumentType->ll_grammarScope);
	}
	if (pFound && strcmp(pFound->ll_name, "default") == 0) {
		pEditConfiguration->documentType = NULL;
	}
	else {
		pEditConfiguration->documentType = pFound;
	}
	return TRUE;
}

/*
 * Find a grammar given the name of a programming language for which a grammar is found.
 * Several different type of syntax to specify the language name are supported:
 * - if the name starts with "lang-" the prefix is skipped.
 * - upper / lower case of language name is ignored.
 * - for every langugage a list of alternate names can be defined (PASCAL, DELPHI, PAS)
 */
GRAMMAR* doctypes_findGrammarForLanguage(const char* pszGrammarName) {
	DOCUMENT_TYPE* llp;
	if (strncmp(pszGrammarName, "lang-", 5) == 0) {
		pszGrammarName += 5;
	}
	for (llp = config.dc_types; llp != 0; llp = llp->ll_next) {
		if (_stricmp(pszGrammarName, llp->ll_grammarScope) == 0) {
			return grammar_findNamed(llp->ll_grammarScope);
		}
		if (llp->ll_languages != NULL) {
			for (int i = 0; i < arraylist_size(llp->ll_languages); i++) {
				if (_strcmpi(arraylist_get(llp->ll_languages, i), pszGrammarName) == 0) {
					return grammar_findNamed(llp->ll_grammarScope);
				}
			}
		}
	}
	return NULL;
}

/*--------------------------------------------------------------------------
 * doctypes_assignDocumentTypeDescriptor()
 * assign document type properties / descriptor to file
 * if pDocumentDescriptor == 0, read document descriptor from disc according to filename pattern
 * match
 */
int doctypes_assignDocumentTypeDescriptor(FTABLE* fp, EDIT_CONFIGURATION* pEditConfiguration, 
		char* pszDocumentTypeName) {
	EDIT_CONFIGURATION* pConfig = fp->documentDescriptor;
	int wMode;
	int dMode;

	if (pConfig) {
		wMode = pConfig->workmode;
		dMode = pConfig->dispmode;
	}

	if (fp->documentDescriptor) {
		doctypes_destroyEditConfiguration(fp->documentDescriptor);
		free(fp->documentDescriptor);
	}

	if ((fp->documentDescriptor = calloc(1, sizeof * fp->documentDescriptor)) == 0) {
		return 0;
	}

	if (pEditConfiguration) {
		doctypes_copyDescriptor(fp->documentDescriptor, pEditConfiguration);
		return 1;
	}
	const char* pszFirstLine = NULL;
	size_t nFirstLineLen = 0;
	if (fp->firstl) {
		pszFirstLine = fp->firstl->lbuf;
		nFirstLineLen = fp->firstl->len;
	}
	doctypes_getFileDocumentType(fp->documentDescriptor, pszDocumentTypeName, fp->fname, pszFirstLine, nFirstLineLen);
	if (pConfig) {
		fp->documentDescriptor->workmode = wMode;
		fp->documentDescriptor->dispmode = dMode;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * doctypes_assignDocumentTypeDescriptor()
 * Try to reassign document type properties if document type properties were assigned
 * before. At this point in time we are able to determine a document type from the first line
 * of the document.
 */
int doctypes_reassignDocumentTypeDescriptor(FTABLE* fp) {
	EDIT_CONFIGURATION* pConfig = fp->documentDescriptor;
	if (pConfig == NULL || pConfig->documentType == NULL) {
		doctypes_assignDocumentTypeDescriptor(fp, NULL, NULL);
		return TRUE;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * doctypes_saveAllDocumentTypes()
 * Save all document types - pass the pointer to the "HEAD" of the doctype list.
 * If a pointer of a changed configuration is passed, the configuration will be
 * copied back to our prototype configuration.
 */
int doctypes_saveAllDocumentTypes(EDIT_CONFIGURATION* pChangedConfiguration, char* pszFilename) {

	if (pChangedConfiguration) {
		for (EDIT_CONFIGURATION* pConfiguration = config.dc_editorConfigurations; pConfiguration; pConfiguration = pConfiguration->next) {
			if (strcmp(pChangedConfiguration->name, pConfiguration->name) == 0) {
				doctypes_copyDescriptor(pConfiguration, pChangedConfiguration);
				break;
			}
		}
	}
	if (pszFilename == NULL) {
		pszFilename = _editConfigurationFileInfo.fname;
	}
 	if (!json_marshal(pszFilename, &config, _doctypeConfigurationRules)) {
		// TODO: I18N
		error_displayAlertDialog("Error saving the file with the editor configuration.");
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * doctypes_createDocumentType()
 */
DOCUMENT_TYPE* doctypes_createDocumentType(DOCUMENT_TYPE* llp)
{
	DOCUMENT_TYPE* llpNew;
	int		nLen;

	if ((llpNew = ll_insert(&config.dc_types, sizeof * llpNew)) == 0) {
		return 0;
	}
	if (llp) {
		/* do not copy link pointer and do not share other malloced references as they are freed later for every doc type */
		memmove(&llpNew->ll_name, &llp->ll_name, sizeof * llpNew - sizeof llpNew->ll_next);
		llpNew->ll_languages = arraylist_create(3);
	}
	llpNew->ll_documentDescriptor = 0;
	if ((nLen = (int)strlen(llpNew->ll_name)) < sizeof(llpNew->ll_name) - 2) {
		llpNew->ll_name[nLen++] = '*';
		llpNew->ll_name[nLen] = 0;
	}
	return llpNew;
}

BOOL doctypes_destroyEditConfiguration(EDIT_CONFIGURATION* pConfiguration) {
	json_destroy(pConfiguration, _editConfigurationRules);
	return TRUE;
}

static void doctypes_preDestroyDocumentType(DOCUMENT_TYPE* pType) {
	if (pType->ll_icon) {
		DeleteObject(pType->ll_icon);
		pType->ll_icon = NULL;
	}
}

/**
 * Deletes and de-allocates all known document types and editor configurations.
 */
void doctypes_destroyAllDocumentTypes() {
	DOCUMENT_TYPE* pTypes = config.dc_types;
	while (pTypes) {
		doctypes_preDestroyDocumentType(pTypes);
		pTypes = pTypes->ll_next;
	}
	json_destroy(&config, _doctypeConfigurationRules);
}

/*--------------------------------------------------------------------------
 * doctypes_deleteDocumentType()
 * Deletes a given document type.
 */
void doctypes_deleteDocumentType(DOCUMENT_TYPE* llp) {
	doctypes_preDestroyDocumentType(llp);
	ll_delete(&config.dc_types, llp);
}

/*--------------------------------------------------------------------------
 * doctypes_getPrivateDocumentType()
 *
 * Return the private document type given the name of the document type.
 */
DOCUMENT_TYPE* doctypes_getPrivateDocumentType(char* name) {
	DOCUMENT_TYPE* llp;

	for (llp = config.dc_types; llp; llp = llp->ll_next) {
		if (lstrcmp(llp->ll_name, name) == 0) {
			return llp;
		}
	}

	return (DOCUMENT_TYPE*)config.dc_types;
}

/*--------------------------------------------------------------------------
 * doctypes_getDocumentTypeDescriptor()
 */
EDIT_CONFIGURATION* doctypes_getDocumentTypeDescriptor(DOCUMENT_TYPE* p, char **pDocumentTypeName)
{
	if (p == 0) {
		*pDocumentTypeName = NULL;
		return 0;
	}
	*pDocumentTypeName = p->ll_name;
	return p->ll_documentDescriptor;
}

/*--------------------------------------------------------------------------
 * doctypes_initAllDocumentTypes()
 * init all document types and editor configurations by reading our JSON
 * config file.
 */
int doctypes_initAllDocumentTypes(void) {
	EDIT_CONFIGURATION* lp;
	DOCUMENT_TYPE* dp;

	memset(&config, 0, sizeof config);
	int ret = 0;
	if (json_parse(_editConfigurationFileInfo.fname, TRUE, &config, _doctypeConfigurationRules)) {
		for (dp = config.dc_types; dp != NULL; dp = dp->ll_next) {
			lp = config.dc_editorConfigurations;
			while (lp != NULL) {
				if (strcmp(DEFAULT, lp->name) == 0) {
					config.dc_defaultEditorConfiguration = lp;
				}
				if (strcmp(dp->ll_editorConfigurationName, lp->name) == 0) {
					dp->ll_documentDescriptor = lp;
					break;
				}
				lp = lp->next;
			}
		}
		ret = 1;
	}
	if (!config.dc_defaultEditorConfiguration) {
		config.dc_defaultEditorConfiguration = doctypes_createDefaultDocumentTypeDescriptor();
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * doctypes_saveToFile()
 */
int doctypes_saveToFile(void) {
	FILE_SELECT_PARAMS params = {
		.fsp_saveAs = TRUE,
		.fsp_optionsAvailable = FALSE
	};

	if (!ft_getCurrentDocument() || fsel_selectFileWithOptions(&_editConfigurationFileInfo, CMD_ADD_DOC_MACROS, &params) == 0) {
		return 0;
	}

	if (!json_marshal(params.fsp_resultFile, &config, _doctypeConfigurationRules)) {
		// TODO: I18N
		error_displayAlertDialog("Error saving the file with the editor configiration.");
		return 0;
	}
	return 1;
}


/*
 * Return a list of all defined editor configuration names sorted. Note, that the returned list must be freed,
 * when it is not used any more.
 */
ARRAY_LIST* doctypes_getEditorConfigurationNamesSorted() {
	ARRAY_LIST* pResult = arraylist_create(20);
	EDIT_CONFIGURATION* pConfig = config.dc_editorConfigurations;
	while (pConfig) {
		arraylist_add(pResult, pConfig->name);
		pConfig = pConfig->next;
	}
	arraylist_sort(pResult, NULL);
	return pResult;
}
