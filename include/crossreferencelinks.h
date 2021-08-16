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

extern int xref_openFile(char* name, long line, WINDOWPLACEMENT* wsp);

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
/* EdErrorNext()				*/
/*---------------------------------*/
extern int EdErrorNext(int dir);

/*--------------------------------------------------------------------------
 * xref_addSearchListEntry()
 * Add an entry to the "current search list" file in the standard PKS Edit search list
 * navigation format ("filename", line lineNumer: remarks).
 */
extern int xref_addSearchListEntry(FTABLE* fp, char* fn, long line, char* remark);

/*
 * Determine the "identifier" close to the input caret in the current editor window.
 * If text is selected, use that as the identifier, otherwise try to identify the close
 * by identifier.
 */
extern void xref_getSelectedIdentifier(char* pszText, size_t nMaxChars);

/**
 * xref_findIdentifierCloseToCaret
 * Find an identifier close to the caret in the current editor window.
 * Copy the found identifier into pszTargetBuffer and return the pointer to the start of
 * the expression in pszExpressionBegin and return the end of the found expression in pszExpressionEnd if not NULL.
 **/
extern char* xref_findIdentifierCloseToCaret(unsigned char* pszTargetBuffer, unsigned char* pszTargetBufferEnd,
	unsigned char** pszExpressionBegin, unsigned char** pszExpressionEnd, int bFindStartOfWord);

extern int EdFindTagCursor(void);

extern int EdFindFileCursor(void);

extern int EdFindWordCursor(dir);

extern void xref_openSearchList(char* fn, int cmpflg);

extern int EdSearchListRead(void);

extern int EdTagfileRead(void);

extern int EdErrorListRead(long dummy1, long dummy2);

#define CROSSREFERENCELINKS_H
#endif // !CROSSREFERENCELINKS_H
