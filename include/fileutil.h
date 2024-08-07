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
 * If used during batch mode, error handling is different.
 */
extern int file_openFile(char* fn, BOOL bBatchMode);

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

typedef enum enumCONFIG_FILE_SEARCH_FLAGS {
	CFSF_SEARCH_LOCAL_PKS_SYS = 0x8,
	CFSF_SEARCH_APP_PKS_SYS = 0x1,
	CFSF_SEARCH_PKS_SYS_EXTENSION_DIR = 0x2,
	CFSF_SEARCH_PKS_SYS_OVERRIDE_DIR = 0x20,
	CFSF_SEARCH_ABSOLUTE = 0x10,
} CONFIG_FILE_SEARCH_FLAGS;

/*--------------------------------------------------------------------------
 * file_searchFileInPKSEditLocation()
 * Searches a file in a "wellknown" PKS Edit location (PKS_SYS, home etc...) and
 * returns the result - this is not re-entrant. Before calling again, one must
 * save the result.
 */
extern char* file_searchFileInPKSEditLocation(const char* s);

/*--------------------------------------------------------------------------
 * file_searchFileInPKSEditLocationFlags()
 * Searches a file in the "wellknown" PKS Edit locations defined by the passed search flags and
 * returns the result - this is not re-entrant. Before calling again, one must
 * save the result.
 */
extern char* file_searchFileInPKSEditLocationFlags(const char* s, CONFIG_FILE_SEARCH_FLAGS flags);

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
 * Returns the last time, a file was accessed and created and determines, whether the file is a directory.
 * If unsuccessful 0 is returned.
 */
extern int file_getFileAttributes(char* fname, EDTIME* pCreated, EDTIME* pModified, BOOL* pIsDirectory);

/*
 * Read the file identified by the file descriptor into a single 0-terminated string and return it.
 * Returns NULL, if the file is too large and malloc fails. It is the responsibility of the caller
 * to free the memory returned.
 */
extern char* file_readFileAsString(int fd);

/*
 * The PKS_SYS install directory, where PKS-Edit config files are located.
 */
extern char* _pksSysFolder;

/*
 * A folder in the Users home directory, where PKS-Edit config files may be placed to extend the default configuration of PKS Edit.
 */
extern char _pksSysExtensionFolder[];

/*
 * A folder in the Users home directory, where PKS-Edit config files may be placed to override the installed files.
 */
extern char _pksSysOverrideFolder[];


/*
 * Checks whether two file names describe two different files. Returns
 * TRUE, if different.
 */
extern BOOL file_fileNamesDiffer(const char* fileName1, const char* fileName2);

#define FILEUTIL_H
#endif