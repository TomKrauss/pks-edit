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
 * xref_initFileFormats()
 * init all compiler types
 */
extern int xref_initFileFormats(void);

/*--------------------------------------------------------------------------
 * xref_selectFileFormat()
 * activate a new source list for tag searches (e.g. when
 * changing document type of top level window)
 */
extern void xref_selectFileFormat(char* tags);

/*--------------------------------------------------------------------------
 * xref_restoreFromConfigFile()
 * init all compiler types
 */
extern int xref_restoreFromConfigFile(void);

extern int xref_openFile(char* name, long line, WINDOWPLACEMENT* wsp);

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
 * xref_selectSearchListFormat()
 * Select a format for the search lists, that can be navigated. PKS Edit supports
 * some built in compiler and other tool output formats which can be used to
 * navigate along (file name + line number + ....).
 */
extern void xref_selectSearchListFormat(char* pszName);

/*--------------------------------------------------------------------------
 * xref_addSearchListEntry()
 * Add an entry to the "current search list" file in the standard PKS Edit search list
 * navigation format ("filename", line lineNumer: remarks).
 */
extern int xref_addSearchListEntry(FTABLE* fp, char* fn, long line, char* remark);

extern int EdFindTagCursor(void);

extern int EdFindFileCursor(void);

extern int EdFindWordCursor(dir);

extern void xref_openSearchList(char* fn, int cmpflg);

extern int EdSearchListRead(void);

extern int EdTagfileRead(void);

extern int EdErrorListRead(long dummy1, long dummy2, char* pszCompiler);






#define CROSSREFERENCELINKS_H
#endif // !CROSSREFERENCELINKS_H
