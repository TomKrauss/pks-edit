/*
 * FILEUTIL.H
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: file utilitities.
 *
 * 										created: 28.10.91
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

# ifndef 	FILEUTIL_H

/*--------------------------------------------------------------------------
 * EdStat()
 * Test the existance of a file. If it exists, return 0 otherwise -1.
 */
extern int EdStat(char* s);

/*--------------------------------------------------------------------------
 * EdFileMode()
 * Return the "modes" (read write, A_NORMAL, A_READONLY, A_HIDDEN, A_SYSTEM, ...) of a file.
 */
extern int EdFileMode(char* s);

/*--------------------------------------------------------------------------
 * EdIsDir()
 * Returns true if the passed filename points to a directory.
 */
extern int EdIsDir(char* filename);

/*--------------------------------------------------------------------------
 * searchfile()
 * Searches a file in a "wellknown" PKS Edit location (PKS_SYS, home etc...) and
 * returns the result - this is not re-entrant. Before calling again, one must
 * save the result.
 */
extern char* searchfile(char* s);

/*------------------------------------------------------------
 * TmpName()
 * Returns the name of a temp file.
 */
extern char* TmpName(char* dst, char c);

/*------------------------------------------------------------
 * TmpDir()
 * Returns the path to the temp directory.
 */
extern char* TmpDir(void);

/*---------------------------------
 * closeF()
 * close a file handle and report an error if unsuccessful.
/*---------------------------------*/
extern int closeF(int* fd);

#define FILEUTIL_H
#endif