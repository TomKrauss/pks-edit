/*
 * crossreferencelinks.h
 *
 * maintain tags and do "under cursor actions"
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 28. Dec. 1990
 *
 */

#ifndef CROSSREFERENCELINKS_H

#ifndef PUBLICAPI_H
#include "publicapi.h"
#endif

/*--------------------------------------------------------------------------
 * xref_restoreFromConfigFile()
 * init all compiler types
 */
extern int xref_restoreFromConfigFile(void);

extern int xref_openFile(const char* name, long line, FT_OPEN_OPTIONS* pOptions);

/*
 * Free all memory occupied by the cross reference lists.
 */
extern void xref_destroyAllCrossReferenceLists();

/*---------------------------------*
 * xref_navigateCrossReference()
 * Navigate to the cross reference word given as an argument - no
 * selection of multiple matches yet.
 *---------------------------------*/
extern int xref_navigateCrossReference(char* s);

/*---------------------------------
 * xref_openWindowHistory()
 * Parse the search list result in the current line and try to navigate to
 * the file and line number which are obtained by parsing the line contents.
 *---------------------------------*/
extern void xref_openWindowHistory(LINE* lp);

/*---------------------------------*/
/* xref_navigateSearchErrorList()				*/
/*---------------------------------*/
extern int xref_navigateSearchErrorList(LIST_OPERATION_FLAGS nNavigationType);

/*--------------------------------------------------------------------------
 * xref_addSearchListEntry()
 * Add an entry to the "current search list" file in the standard PKS Edit search list
 * navigation format ("filename", line lineNumer: remarks).
 */
extern int xref_addSearchListEntry(WINFO* wp, char* pszBuf, char* fn, long line, char* remark);

/*
 * Determine the "identifier" close to the input caret in the current editor window.
 * If text is selected, use that as the identifier, otherwise try to identify the close
 * by identifier.
 * If pByteOffset != NULL, return the offset in bytes in the line, where the identifier was found.
 */
extern int xref_getSelectedIdentifier(WINFO* wp, char* pszText, size_t nMaxChars, int* pByteOffset);

extern int EdFindTagCursor(WINFO* wp);

extern int EdFindFileCursor(WINFO* wp);

/**
 * xref_findIdentifierCloseToCaret
 * Find an identifier close to the caret in the current editor window.
 * Copy the found identifier into pszTargetBuffer and return the pointer to the start of
 * the expression in pszExpressionBegin and return the end of the found expression in pszExpressionEnd if not NULL.
 **/
typedef enum { FI_BEGIN_WORD_TO_CURSOR, FI_CURSOR_TO_END_WORD, FI_COMPLETE_WORD} FIND_IDENTIFIER_OPTIONS;
extern char* xref_findIdentifierCloseToCaret(WINFO* wp, CARET* pCaret, unsigned char* pszTargetBuffer, unsigned char* pszTargetBufferEnd,
	unsigned char** pszExpressionBegin, unsigned char** pszExpressionEnd, FIND_IDENTIFIER_OPTIONS fiOptions);

/**
 * xref_findMatchingCloseToCaret
 * Find an word matching matchFunction close to the caret in the current editor window.
 **/
char* xref_findMatchingCloseToCaret(WINFO* wp, CARET* pCaret, unsigned char* pszTargetBuffer, unsigned char* pszTargetBufferEnd,
	unsigned char** pszExpressionBegin, unsigned char** pszExpressionEnd, int (*matchFunction)(unsigned char c), FIND_IDENTIFIER_OPTIONS fiOptions);

/*
 * Iterate over all cross references defined for the grammar of the given editor window and
 * process all tags defined matching the text 'pszMatching'. Return 1 if successful.
 */
#ifdef CODEANALYZER_H
extern int xref_forAllTagsDo(WINFO* wp, int (*matchfunc)(const char* pszMatching), ANALYZER_CALLBACK cbAnalyzer);
#endif

/*
 * Open a build output file with the given  file name and [codePage]. If codePage should be
 * automatically detected, pass -1. If the output file is the result of a PKSMacroC build, 
 * pass TRUE for [bMacroCError].
 */
extern int xref_openBuildOutputFile(const char* pszFilename, int bMacroCError, long codePage);

extern int EdSearchListRead(void);

extern int EdTagfileRead(void);

extern int EdErrorListRead(long dummy1, long dummy2);

#define CROSSREFERENCELINKS_H
#endif // !CROSSREFERENCELINKS_H
