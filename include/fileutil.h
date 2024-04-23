/*
 * FILEUTIL.H
 *
 * Project: PKS Edit for Windows
 *
 * purpose: file utilitities.
 *
 * 										created: 28.10.91
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

# ifndef 	FILEUTIL_H

typedef time_t EDTIME;

/*--------------------------------------------------------------------------
 * file_exists()
 * Test the existance of a file. If it exists, return 0 otherwise -1.
 */
extern int file_exists(const char* s);

/*--------------------------------------------------------------------------
 * file_getFileMode()
 * Return the "modes" (read write, A_NORMAL, A_READONLY, A_HIDDEN, A_SYSTEM, ...) of a file.
 */
extern int file_getFileMode(const char* s);

/*
 * Can be used to check, whether a given filename describes an absolute path.
 */
extern int file_isAbsolutePathName(const char* pszFilename);

/*--------------------------------------------------------------------------
 * file_openFile()
 * Open a file and return the file handle. If it cannot be opened, display an error.
 */
extern int file_openFile(char* fn);

/*
 * Make a file readable and writeable.
 */
extern int file_makeFileReadWrite(char* fn);

/*--------------------------------------------------------------------------
 * file_createFile()
 * Create a file with a given name with a default read-write mode.
 */
extern int file_createFile(char* fn);

/*--------------------------------------------------------------------------
 * file_createFileWithMode()
 * Create a file with a given mode.
 */
extern int file_createFileWithMode(char* fn, int mode);

/*--------------------------------------------------------------------------
 * file_searchFileInPKSEditLocation()
 * Searches a file in a "wellknown" PKS Edit location (PKS_SYS, home etc...) and
 * returns the result - this is not re-entrant. Before calling again, one must
 * save the result.
 */
extern char* file_searchFileInPKSEditLocation(const char* s);

/*--------------------------------------------------------------------------
 * file_searchFileInDir()
 * Searches a file in a directory. If found, return the full path name.
 */
extern char* file_searchFileInDirectory(char* s, char* pDir);

/*--------------------------------------------------------------------------
 * file_searchFileInPath()
 * Search a file in a path list (pathes separated by ; or ,).
 */
extern char* file_searchFileInPath(char* fn, char* path);

/*------------------------------------------------------------
 * file_getTempFilename()
 * Returns the name of a temp file.
 */
extern char* file_getTempFilename(char* dst, char c);

/*------------------------------------------------------------
 * file_getTempDirectory()
 * Returns the path to the temp directory.
 */
extern char* file_getTempDirectory(void);

/*---------------------------------
 * file_closeFile()
 * close a file handle and report an error if unsuccessful.
/*---------------------------------*/
extern int file_closeFile(int* fd);

/*---------------------------------------------------------------
 * file_clearTempFiles()
 * Remove all files from the PKS Edit temp directory.
 */
extern void file_clearTempFiles(void);

/**
 * Returns the last time, a file was accessed and created.
 * If unsuccessful 0 is returned.
 */
extern int file_getAccessTime(char* fname, EDTIME* pCreated, EDTIME* pModified);

/*
 * Read the file identified by the file descriptor into a single 0-terminated string and return it.
 * Returns NULL, if the file is too large and malloc fails. It is the responsibility of the caller
 * to free the memory returned.
 */
extern char* file_readFileAsString(int fd);

extern char* _pksSysFolder;

#define FILEUTIL_H
#endif