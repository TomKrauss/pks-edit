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

/*--------------------------------------------------------------------------
 * xref_restoreFromConfigFile()
 * init all compiler types
 */
extern int xref_restoreFromConfigFile(void);

extern int xref_openFile(const char* name, long line, const char* pszDockName);

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
 * xref_openSearchListResultFromLine()
 * Parse the search list result in the current line and try to navigate to
 * the file and line number which are obtained by parsing the line contents.
 *---------------------------------*/
extern void xref_openSearchListResultFromLine(LINE* lp);

/*---------------------------------*/
/* xref_navigateSearchErrorList()				*/
/*---------------------------------*/
extern int xref_navigateSearchErrorList(int dir);

/*--------------------------------------------------------------------------
 * xref_addSearchListEntry()
 * Add an entry to the "current search list" file in the standard PKS Edit search list
 * navigation format ("filename", line lineNumer: remarks).
 */
extern int xref_addSearchListEntry(char* pszBuf, char* fn, long line, char* remark);

/*
 * Determine the "identifier" close to the input caret in the current editor window.
 * If text is selected, use that as the identifier, otherwise try to identify the close
 * by identifier.
 */
extern int xref_getSelectedIdentifier(WINFO* wp, char* pszText, size_t nMaxChars);

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

/*
 * Iterate over all cross references defined for the grammar of the given editor window and
 * process all tags defined matching the text 'pszMatching'. Return 1 if successful.
 */
#ifdef CODEANALYZER_H
extern int xref_forAllTagsDo(WINFO* wp, int (*matchfunc)(const char* pszMatching), ANALYZER_CALLBACK cbAnalyzer);
#endif

extern int xref_openSearchList(char* fn, int cmpflg);

extern int EdSearchListRead(void);

extern int EdTagfileRead(void);

extern int EdErrorListRead(long dummy1, long dummy2);

#define CROSSREFERENCELINKS_H
#endif // !CROSSREFERENCELINKS_H
