/*
 * 
 * DocumentTypes.c
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS
 *
 * purpose: maintain "Major Mode Configuration Files", the in PKSEDIT
 *		  called RULER Files,
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
#include <stddef.h>
#include <stdio.h>

#include "alloc.h"
#include "tos.h"
#include "jsonparser.h"
#include "linkedlist.h"
#include "lineoperations.h"
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


 /*
 * Description of one document type in PKS edit.
 */
typedef struct tagDOCUMENT_TYPE {
	/*
	 * Next Document Type - they are maintained in a linked list.
	 */
	struct tagDOCUMENT_TYPE *	ll_next;
	char				ll_name[32];						// name of the document type.
	char				ll_grammarScope[32];					// name of the grammar associated with this file type.
	char				ll_description[80];					// Description for file selector.
	int					ll_privateEditorConfiguration;		// Whether this is a private configuration. If true, this is used for single files explicitly
	char				ll_editorConfigurationName[32];		// Editor configuration name
	char   				ll_match[64];						// file name pattern to match
	EDIT_CONFIGURATION * ll_documentDescriptor;
} DOCUMENT_TYPE;

DOCUMENT_TYPE *doctypes_createDocumentType(DOCUMENT_TYPE *llp);

FSELINFO _linfsel = {	"", "pkseditconfig.json", "*.json" };

#define	LINSPACE	offsetof(EDIT_CONFIGURATION, ts)

static JSON_MAPPING_RULE _documentTypeRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(DOCUMENT_TYPE, ll_name), sizeof(((DOCUMENT_TYPE*)NULL)->ll_name)},
	{	RT_CHAR_ARRAY, "description", offsetof(DOCUMENT_TYPE, ll_description), sizeof(((DOCUMENT_TYPE*)NULL)->ll_description)},
	{	RT_CHAR_ARRAY, "grammar", offsetof(DOCUMENT_TYPE, ll_grammarScope), sizeof(((DOCUMENT_TYPE*)NULL)->ll_grammarScope)},
	{	RT_CHAR_ARRAY, "editorConfiguration", offsetof(DOCUMENT_TYPE, ll_editorConfigurationName), sizeof(((DOCUMENT_TYPE*)NULL)->ll_editorConfigurationName)},
	{	RT_CHAR_ARRAY, "filenamePatterns", offsetof(DOCUMENT_TYPE, ll_match), sizeof(((DOCUMENT_TYPE*)NULL)->ll_match)},
	{	RT_FLAG, "isPrivate", offsetof(DOCUMENT_TYPE, ll_privateEditorConfiguration), 1},
	{	RT_END}
};

static JSON_MAPPING_RULE _editorConfigurationRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(EDIT_CONFIGURATION, name), sizeof(((EDIT_CONFIGURATION*)NULL)->name)},
	{	RT_INTEGER, "id", offsetof(EDIT_CONFIGURATION, id)},
	{	RT_INTEGER, "leftMargin", offsetof(EDIT_CONFIGURATION, lmargin) },
	{	RT_INTEGER, "rightMargin", offsetof(EDIT_CONFIGURATION, rmargin)},
	{	RT_INTEGER, "tabSize", offsetof(EDIT_CONFIGURATION, tabsize)},
	{	RT_INTEGER_ARRAY, "tabulatorSizes", offsetof(EDIT_CONFIGURATION, tabulatorSizes), DIM(((EDIT_CONFIGURATION*)NULL)->tabulatorSizes)},
	{	RT_INTEGER, "shiftWidth", offsetof(EDIT_CONFIGURATION, shiftwidth)},
	{	RT_FLAG, "showWhitespace", offsetof(EDIT_CONFIGURATION, dispmode), SHOWCONTROL},
	{	RT_FLAG, "showHexadecimalChars", offsetof(EDIT_CONFIGURATION, dispmode), SHOWHEX},
	{	RT_FLAG, "showRuler", offsetof(EDIT_CONFIGURATION, dispmode), SHOWRULER},
	{	RT_FLAG, "showLineNumbers", offsetof(EDIT_CONFIGURATION, dispmode), SHOWLINENUMBERS},
	{	RT_FLAG, "showStatus", offsetof(EDIT_CONFIGURATION, dispmode), SHOWSTATUS},
	{	RT_FLAG, "hideVerticalSlider", offsetof(EDIT_CONFIGURATION, dispmode), SHOWHIDEVSLIDER},
	{	RT_FLAG, "hideHorizontalSlider", offsetof(EDIT_CONFIGURATION, dispmode), SHOWHIDEHSLIDER},
	{	RT_FLAG, "highlightCaretLine", offsetof(EDIT_CONFIGURATION, dispmode), SHOWCARET_LINE_HIGHLIGHT},
	{	RT_FLAG, "insertMode", offsetof(EDIT_CONFIGURATION, workmode), WM_INSERT},
	{	RT_FLAG, "autoIndent", offsetof(EDIT_CONFIGURATION, workmode), WM_AUTOINDENT},
	{	RT_FLAG, "autoWrap", offsetof(EDIT_CONFIGURATION, workmode), WM_AUTOWRAP},
	{	RT_FLAG, "autoFormat", offsetof(EDIT_CONFIGURATION, workmode), WM_AUTOFORMAT},
	{	RT_FLAG, "codeFormat", offsetof(EDIT_CONFIGURATION, workmode), WM_BRINDENT},
	{	RT_FLAG, "expandAbbreviations", offsetof(EDIT_CONFIGURATION, workmode), WM_ABBREV},
	{	RT_FLAG, "encrypted", offsetof(EDIT_CONFIGURATION, workmode), O_CRYPTED},
	{	RT_FLAG, "oemEncoding", offsetof(EDIT_CONFIGURATION, workmode), WM_OEMMODE},
	{	RT_FLAG, "smartSpaceDelete", offsetof(EDIT_CONFIGURATION, workmode), WM_DELETE_MULTIPLE_SPACES},
	{	RT_CHAR_ARRAY, "statuslineFormat", offsetof(EDIT_CONFIGURATION, statusline), sizeof(((EDIT_CONFIGURATION*)NULL)->statusline)},
	{	RT_CHAR, "tabCharacter", offsetof(EDIT_CONFIGURATION, tabDisplayFillCharacter)},
	{	RT_CHAR, "fillCharacter", offsetof(EDIT_CONFIGURATION, fillc)},
	{	RT_CHAR, "newlineCharacter", offsetof(EDIT_CONFIGURATION, nl)},
	{	RT_CHAR, "alternateNewlineCharacter", offsetof(EDIT_CONFIGURATION, nl2)},
	{	RT_CHAR, "crCharacter", offsetof(EDIT_CONFIGURATION, cr)},
	{	RT_CHAR_ARRAY, "editFontStyleName", offsetof(EDIT_CONFIGURATION, editFontStyleName), sizeof(((EDIT_CONFIGURATION*)NULL)->editFontStyleName)},
	{	RT_INTEGER, "scrollVerticallyBy", offsetof(EDIT_CONFIGURATION, vscroll)},
	{	RT_INTEGER, "scrollHorizontallyBy", offsetof(EDIT_CONFIGURATION, hscroll)},
	{	RT_INTEGER, "scrollVerticalBorder", offsetof(EDIT_CONFIGURATION, scroll_dy)},
	{	RT_INTEGER, "cursorSearchPlacement", offsetof(EDIT_CONFIGURATION, cursaftersearch)},
	{	RT_FLAG, "scrollSupportTrackThumb", offsetof(EDIT_CONFIGURATION, scrollflags), SC_THUMBTRACK},
	{	RT_FLAG, "caretFollowsScrollbar", offsetof(EDIT_CONFIGURATION, scrollflags), SC_CURSORCATCH},
	{	RT_CHAR_ARRAY, "backupExtension", offsetof(EDIT_CONFIGURATION, backupExtension), sizeof(((EDIT_CONFIGURATION*)NULL)->backupExtension)},
	{	RT_CHAR_ARRAY, "executeOnLoad", offsetof(EDIT_CONFIGURATION, creationMacroName), sizeof(((EDIT_CONFIGURATION*)NULL)->creationMacroName)},
	{	RT_CHAR_ARRAY, "executeOnSave", offsetof(EDIT_CONFIGURATION, closingMacroName), sizeof(((EDIT_CONFIGURATION*)NULL)->closingMacroName)},
	{	RT_END}
};

typedef struct tagDOCTYPE_CONFIGURATION {
	EDIT_CONFIGURATION* dc_editorConfigurations;
	DOCUMENT_TYPE* dc_types;
	EDIT_CONFIGURATION* dc_defaultEditorConfiguration;
} DOCTYPE_CONFIGURATION;

static void* doctypes_create() {
	return calloc(sizeof(DOCUMENT_TYPE), 1);
}

static const char* DEFAULT = "default";

static JSON_MAPPING_RULE _doctypeConfigurationRules[] = {
	{	RT_OBJECT_LIST, "documentTypes", offsetof(DOCTYPE_CONFIGURATION, dc_types), 
			{.r_t_arrayDescriptor = {doctypes_create, _documentTypeRules}}},
	{	RT_OBJECT_LIST, "editorConfigurations", offsetof(DOCTYPE_CONFIGURATION, dc_editorConfigurations), 
			{.r_t_arrayDescriptor = {doctypes_createDefaultDocumentTypeDescriptor, _editorConfigurationRules}}},
	{	RT_END}
};

static DOCTYPE_CONFIGURATION config;

/*--------------------------------------------------------------------------
 * Creates the default attributes for editing a document. The returned structure
 * must be freed, when done using it.
 */
EDIT_CONFIGURATION* doctypes_createDefaultDocumentTypeDescriptor() {
	EDIT_CONFIGURATION* pDescriptor = calloc(1, sizeof *pDescriptor);

	pDescriptor->rmargin = 80;
	pDescriptor->tabsize = 8;
	pDescriptor->shiftwidth = 4;
	pDescriptor->workmode = WM_INSERT;
	pDescriptor->cr = '\r';
	pDescriptor->nl = '\n';
	pDescriptor->tabDisplayFillCharacter = ' ';
	strcpy(pDescriptor->statusline, "0x%6p$O: 0x%2p$C 0%h$C");
	strcpy(pDescriptor->editFontStyleName, DEFAULT);
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
 * doctypes_getRelatedFilename()
 */
static void doctypes_getRelatedFilename(char *related_name, char *fname, char *newext)
{	char *ext;

	strcpy(related_name,fname);
	ext = string_getFileExtension(related_name);
	if (ext[-1] != '.')
		*ext++ = '.';
	strcpy(ext,newext);
}

/*--------------------------------------------------------------------------
 * doctypes_getSelectableDocumentFileTypes()
 * Returns the list of document file types defined in PKS Edit in the format
 * that it can be passed on to the open file dialog for filtering of file types
 * (e.g. *.*|All Files|*.java|Java Files)
 */
void doctypes_getSelectableDocumentFileTypes(char* pszDest, int nMax) {
	LPSTR		pszEnd;
	DOCUMENT_TYPE *	llp;
	int			nCopied;

	pszEnd = pszDest + nMax - 2;
	for (nCopied = 0, llp = config.dc_types; llp != 0; llp = llp->ll_next) {
		if (nCopied > 0) {
			*pszDest++ = '|';
		}
		if (llp->ll_match[0]) {
			pszDest = strmaxcpy(pszDest, 
					llp->ll_description[0] ? 
					llp->ll_description : llp->ll_match, 
					(int)(pszEnd - pszDest));
			*pszDest++ = '|';
			pszDest = strmaxcpy(pszDest, llp->ll_match, 
					(int)(pszEnd - pszDest));
			nCopied++;
		}
	}
}

/*--------------------------------------------------------------------------
 * doctypes_countDocumentTypes()
 */
int doctypes_countDocumentTypes(void)
{
	return ll_size((LINKED_LIST*)config.dc_types);
}

/*--------------------------------------------------------------------------
 * doctypes_addDocumentTypesToListBox()
 */
int doctypes_addDocumentTypesToListBox(HWND hwnd, int nItem)
{
	DOCUMENT_TYPE *		llp;
	int			nCnt;

	for (llp = config.dc_types, nCnt = 0; llp != 0; llp = llp->ll_next) {
		SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, (LPARAM)llp);
		nCnt++;
	}
	return nCnt;
}

/*--------------------------------------------------------------------------
 * doctypes_getDocumentTypeDescription()
 */
BOOL doctypes_getDocumentTypeDescription(DOCUMENT_TYPE *llp, 
	char **ppszId,	char **ppszDescription, char **ppszMatch, char **ppszFname, 
	char **ppszGrammar,
	int **pOwn)
{

	if (!llp) {
		llp = config.dc_types;
	}
	if (!llp) {
		doctypes_createDocumentType((DOCUMENT_TYPE *)0);
		llp = config.dc_types;
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
	if (ppszFname) {
		*ppszFname = llp->ll_editorConfigurationName;
	}
	if (ppszGrammar) {
		*ppszGrammar = llp->ll_grammarScope;
	}
	if (pOwn) {
		*pOwn = &llp->ll_privateEditorConfiguration;
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * doctypes_getFileDocumentType()
 * find the correct document descriptor for a given file
 * 	1. if own document descriptor, try to read  own document descriptor from disc
 * 	2. if common document descriptor, ...
 *	3. if neither, use standard document descriptor
 */
BOOL doctypes_getFileDocumentType(EDIT_CONFIGURATION *linp, char *filename) {
	char 			fname[1024];
	DOCUMENT_TYPE *		llp;
	EDIT_CONFIGURATION*	lp;

	int nRanking = 100;
	int nMatchRanking;
	DOCUMENT_TYPE* pFound = NULL;
	string_splitFilename(filename,(char *)0, fname);
	for (llp = config.dc_types, lp = 0; llp != 0 && lp == 0; llp = llp->ll_next) {
		if (string_matchFilename(fname,llp->ll_match)) {
			// Select most explicit file name pattern - e.g. prefer *.c over *.* and prefer *.cpp over *.cpp;*.h
			nMatchRanking = string_countCharacters(llp->ll_match, '*');
			if (nMatchRanking < nRanking) {
				nRanking = nMatchRanking;
				pFound = llp;
			}
		}
	}
	if (pFound) {
		lp = pFound->ll_documentDescriptor;
		llp = pFound;
	}
	if (lp == NULL && config.dc_defaultEditorConfiguration != NULL) {
		lp = config.dc_defaultEditorConfiguration;
	}
	if (!lp) {
		EDIT_CONFIGURATION* defaultLin = doctypes_createDefaultDocumentTypeDescriptor();
		memmove(linp, defaultLin, sizeof *defaultLin);
		free(defaultLin);
	} else {
		memmove(linp, lp, sizeof * lp);
	}
	if (llp && llp->ll_grammarScope[0]) {
		linp->grammar = grammar_findNamed(llp->ll_grammarScope);
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * doctypes_assignDocumentTypeDescriptor()
 * assign document type properties / descriptor to file
 * if pDocumentDescriptor == 0, read document descriptor from disc according to filename pattern
 * match
 */
int  doctypes_assignDocumentTypeDescriptor(FTABLE *fp, EDIT_CONFIGURATION *pDocumentDescriptor) {
	free(fp->documentDescriptor);
	if ((fp->documentDescriptor = malloc(sizeof *fp->documentDescriptor)) == 0)
		return 0;

	if (pDocumentDescriptor) {
		memmove(fp->documentDescriptor,pDocumentDescriptor,sizeof *pDocumentDescriptor);
		return 1;
	}

	doctypes_getFileDocumentType(fp->documentDescriptor,fp->fname);
	return 1;
}

/*--------------------------------------------------------------------------
 * doctypes_saveAllDocumentTypes()
 * Save all document types - pass the pointer to the "HEAD" of the doctype list.
 * If a pointer of a changed configuration is passed, the configuration will be
 * copied back to our prototype configuration.
 */
int doctypes_saveAllDocumentTypes(EDIT_CONFIGURATION* pChangedConfiguration, char *pszFilename) {

	if (pChangedConfiguration) {
		for (EDIT_CONFIGURATION* pConfiguration = config.dc_editorConfigurations; pConfiguration; pConfiguration = pConfiguration->next) {
			if (strcmp(pChangedConfiguration->name, pConfiguration->name) == 0) {
				size_t nOffset = sizeof pConfiguration->next;
				memcpy(((unsigned char*)pConfiguration) + nOffset, ((unsigned char*)pChangedConfiguration) + nOffset, sizeof * pConfiguration - nOffset);
				break;
			}
		}
	}
	if (pszFilename == NULL) {
		pszFilename = _linfsel.fname;
	}
	if (!json_marshal(pszFilename, &config, _doctypeConfigurationRules)) {
		error_displayAlertDialog("Fehler beim Schreiben der Dokumentkonfigurationsdatei");
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * doctypes_createDocumentType()
 */
DOCUMENT_TYPE *doctypes_createDocumentType(DOCUMENT_TYPE *llp)
{
	DOCUMENT_TYPE * llpNew;
	int		nLen;

	if ((llpNew = (DOCUMENT_TYPE*)ll_insert((LINKED_LIST**)&config.dc_types, sizeof *llpNew)) == 0) {
		return 0;
	}
	if (llp) {
	/* do not copy link pointer ! */
		memmove(&llpNew->ll_name, &llp->ll_name, 
			sizeof *llpNew - sizeof llpNew->ll_next);
	}
	llpNew->ll_documentDescriptor = 0;
	if ((nLen = lstrlen(llpNew->ll_name)) < sizeof(llpNew->ll_name) - 2) {
		llpNew->ll_name[nLen++] = '*';
		llpNew->ll_name[nLen] = 0;
	}
	return llpNew;
}

/**
 * Cleanup: delete a document descriptor.
 */
static BOOL doctypes_freeDocumentType(DOCUMENT_TYPE* dt) {
	if (dt->ll_privateEditorConfiguration && dt->ll_documentDescriptor) {
		free(dt->ll_documentDescriptor);
	}
	return TRUE;
}

/**
 * Deletes and de-allocates all known document types and editor configurations. 
 */
void doctypes_destroyAllDocumentTypes() {
	ll_destroy((LINKED_LIST**)&config.dc_types, doctypes_freeDocumentType);
	ll_destroy((LINKED_LIST**)&config.dc_editorConfigurations, NULL);
}

/*--------------------------------------------------------------------------
 * doctypes_deleteDocumentType()
 * Deletes a given document type.
 */
void doctypes_deleteDocumentType(DOCUMENT_TYPE *llp) {
	ll_delete((LINKED_LIST**)&config.dc_types, llp);
}

/*--------------------------------------------------------------------------
 * doctypes_getPrivateDocumentType()
 * 
 * Return the private document type given the name of the document type.
 */
DOCUMENT_TYPE* doctypes_getPrivateDocumentType(char *name) {
	DOCUMENT_TYPE *	llp;

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
EDIT_CONFIGURATION *doctypes_getDocumentTypeDescriptor(DOCUMENT_TYPE *p)
{
	if (p == 0) {
		return 0;
	}
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
	if (json_parse(_linfsel.fname, &config, _doctypeConfigurationRules)) {
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
	return 1;
}

