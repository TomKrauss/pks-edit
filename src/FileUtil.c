/*
 * FileUtil.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: find, open, create Files
 *
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include <windows.h>
#include <tos.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <ftw.h>
#include "winterf.h"
#include "documentmodel.h"
#include "pksedit.h"
#include "pathname.h"
#include "errordialogs.h"
#include "stringutil.h"
#include "fileutil.h"

static char _found[1024];

#if !defined(S_IREAD)
#define		S_IREAD		_S_IREAD
#define		S_IWRITE	_S_IWRITE
#endif

#define	dostat(s)	(file_exists(s) == 0)
static struct _finddata_t _dta;

extern char* PKS_SYS;

/*--------------------------------------------------------------------------
 * file_exists()
 * Test the existance of a file. If it exists, return 0 otherwise -1.
 */
EXPORT int file_exists(const char *pszFilename)  {
	struct stat   buffer;
	return stat(pszFilename, &buffer);
}

/*--------------------------------------------------------------------------
 * file_getFileMode()
 * Return the "modes" (read write, A_NORMAL, A_READONLY, A_HIDDEN, A_SYSTEM, ...) of a file.
 */
EXPORT int file_getFileMode(const char *s) {
	if (!dostat(s)) {
		return -1;
	}
	return _dta.attrib;
}


static EDTIME ConvertFileTimeToLTime(FILETIME *pTime) {
	// do not remove - wrong CPP check warning - does not recognize, that QuadPart is build from low- and highpart
	ULARGE_INTEGER ui = {
		.LowPart = pTime->dwLowDateTime,
		.HighPart = pTime->dwHighDateTime
	};
	return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

/**
 * Returns the last time, a file was accessed and created and determines, whether the file is a directory.
 * If unsuccessful 0 is returned.
 */
int file_getFileAttributes(char *fname, EDTIME* pCreated, EDTIME* pModified, BOOL* pIsDirectory) {
	HANDLE				lHandle;
	WIN32_FIND_DATA		findData;

	if ((lHandle = FindFirstFile(fname, &findData)) == INVALID_HANDLE_VALUE) {
		return 0;
	}
	if (pIsDirectory) {
		*pIsDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
	if (pCreated) {
		*pCreated = ConvertFileTimeToLTime(&findData.ftCreationTime);
	}
	if (pModified) {
		*pModified = ConvertFileTimeToLTime(&findData.ftLastWriteTime);
	}
	FindClose(lHandle);
	return 1;
}

/*--------------------------------------------------------------------------
 * file_openFile()
 * Open a file and return the file handle. If it cannot be opened, display an error.
 */
EXPORT int file_openFile(char *fn, BOOL bBatchMode) {
	int fd;

	if ((fd = Fopen(fn, OF_READ)) < 0) {
		if (strlen(fn) >= MAX_PATH) {
			char* pszTemp = malloc(EDMAXPATHLEN);
			if (pszTemp != NULL) {
				strcpy(pszTemp, "\\\\?\\");
				strcat(pszTemp, fn);
				wchar_t* pszWPath = malloc(EDMAXPATHLEN * 2);
				MultiByteToWideChar(CP_ACP, 0, pszTemp, (int)strlen(pszTemp), pszWPath, EDMAXPATHLEN);
				HANDLE hHandle = CreateFile(pszTemp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (hHandle == INVALID_HANDLE_VALUE) {
					fd = -1;
				}
				else {
					fd = (int)(long long)hHandle;
				}
				free(pszTemp);
				free(pszWPath);
			}
		}
		if (fd < 0) {
			error_openingFileFailed(fn, bBatchMode, fd);
		}
	}
	return fd;
}

/*
 * Make a file readable and writeable. 
 */
EXPORT int file_makeFileReadWrite(char *fn) {
	return _chmod(fn,S_IREAD|S_IWRITE);
}

/*--------------------------------------------------------------------------
 * file_createFileWithMode()
 * Create a file with a given mode.
 */
EXPORT int file_createFileWithMode(char *fn,int mode)
{
	int fd;

	while(1) {
		if ((fd = Fcreate(fn,mode)) < 0) {
			if ( file_getFileMode(fn) < 0) {
				error_openingFileFailed(fn, FALSE, fd);
				break;
			} else {
				if (error_displayYesNoConfirmation(IDS_MSGWPROTECT,(LPSTR)string_abbreviateFileName(fn)) == IDYES) {
					if (file_makeFileReadWrite(fn) < 0) {
						error_showErrorById(IDS_MSGERRCHMOD);
						break;
					} else {
						continue;
					}
				}
			}
		} else {
			break;
		}
	}
	return fd;
}

/*--------------------------------------------------------------------------
 * file_createFile()
 * Create a file with a given name with a default read-write mode.
 */
EXPORT int file_createFile(char *fn)
{
	return file_createFileWithMode(fn,0);
}

/*--------------------------------------------------------------------------
 * pathstat()
 */
static int pathstat(char *path,char *fn)
{
	if (path) {
		if (fn != _found) {
			string_concatPathAndFilename(_found, path, fn);
		}
		return (dostat(_found));
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * file_searchFileInDir()
 * Searches a file in a directory. If found, return the full path name.
 */
EXPORT char* file_searchFileInDirectory(char* s, char* pDir) {
	if (pathstat(pDir, s)) {
		return _found;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * file_searchFileInPKSEditLocationFlags()
 * Searches a file in the "wellknown" PKS Edit locations defined by the passed search flags and
 * returns the result - this is not re-entrant. Before calling again, one must
 * save the result.
 */
EXPORT char *file_searchFileInPKSEditLocationFlags(const char *s, CONFIG_FILE_SEARCH_FLAGS flags) {
	BOOL bAbsolute = file_isAbsolutePathName(s);
	if ((flags & CFSF_SEARCH_ABSOLUTE) && bAbsolute && dostat((char*)s)) {
		return (char*)s;
	}
	if (bAbsolute) {
		return 0;
	}
	if ((flags & CFSF_SEARCH_LOCAL_PKS_SYS) && pathstat(PKS_SYS, (char*)s)) {
		return _found;
	}
	if ((flags & CFSF_SEARCH_PKS_SYS_OVERRIDE_DIR) && _pksSysOverrideFolder[0] && pathstat(_pksSysOverrideFolder, (char*)s)) {
		return _found;
	}
	if ((flags & CFSF_SEARCH_PKS_SYS_EXTENSION_DIR) && _pksSysExtensionFolder[0] && pathstat(_pksSysExtensionFolder, (char*)s)) {
		return _found;
	}
	if ((flags & CFSF_SEARCH_APP_PKS_SYS) && pathstat(_pksSysFolder, (char*)s)) {
		return _found;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * file_searchFileInPKSEditLocation()
 * Searches a file in a "wellknown" PKS Edit location (PKS_SYS, home etc...) and
 * returns the result - this is not re-entrant. Before calling again, one must
 * save the result.
 */
EXPORT char* file_searchFileInPKSEditLocation(const char* s) {
	return file_searchFileInPKSEditLocationFlags(s, 0xFF);
}


/*--------------------------------------------------------------------------
 * file_searchFileInPath()
 * Search a file in a path list (pathes separated by ; or ,).
 */
EXPORT char *file_searchFileInPath(char *fn,char *path) {	
	char p[EDMAXPATHLEN],p2[EDMAXPATHLEN];

	if (!*fn) {
		return 0;
	}
	if (dostat(fn)) {
		return fn;
	}

	if (!path || path[0] == 0)
		return 0;

	strcpy(p,path);

	if ((path = strtok(p,",;")) != 0)
	for (;;) {	
		if (strchr(path,'*')) {
			string_splitFilename(path,p2,(char *)0, 0);
			path = p2;
		}
		if (pathstat(path,fn)) {
			return _found;
		}
		if ((path = strtok((char *)0,",;")) == 0)
			break;
	}
	return 0;
}

/*------------------------------------------------------------
 * file_getTempFilename()
 * Returns the name of a temp file.
 */
EXPORT char *file_getTempFilename(char *dst, char c)
{	char		temp[50];
	char		tempPath[1024];

	strcpy(temp, "pks");
	temp[1] = c;
	GetTempPath(sizeof tempPath, tempPath);
	GetTempFileName(tempPath, temp, LOWORD((intptr_t)hInst), dst);
	return dst;
}

/*------------------------------------------------------------
 * file_getTempDirectory()
 * Returns the path to the temp directory.
 */
EXPORT char *file_getTempDirectory(void) {	
	static char tmpdir[1024];

	if (!tmpdir[0]) {
		GetTempPath(sizeof tmpdir, tmpdir);
	}
	return tmpdir;
}

/*
 * Callback used to delete temp files recursively. 
 */
static int file_removeCb(const char* filename, DTA* dta) {
	return _unlink(filename);
}

/*---------------------------------------------------------------
 * file_clearTempFiles()
 * Remove all files from the PKS Edit temp directory.
 */
EXPORT void file_clearTempFiles(void)
{	char tmpname[EDMAXPATHLEN];
	char pathname[EDMAXPATHLEN];
	char fn[EDMAXFNLEN];
	char *szBang;

	file_getTempFilename(tmpname,'!');
	fn[0] = 0;
	string_splitFilename(tmpname,pathname,fn,sizeof fn);
	if ((szBang = strchr(fn, '!')) != 0) {
		*szBang = '?';
		_ftw(pathname, file_removeCb,1,fn,0xFF);
	}
}

/*
 * Can be used to check, whether a given filename describes an absolute path.
 */
EXPORT int file_isAbsolutePathName(const char* pszFilename) {
	return strlen(pszFilename) > 2 && (pszFilename[1] == ':' || (pszFilename[0] == '\\' && pszFilename[1] == '\\'));
}

/*
 * Checks whether two file names describe two different files. Returns
 * value != 0, if different
 */
EXPORT BOOL file_fileNamesDiffer(const char* fileName1, const char* fileName2) {
	char tempFn1[EDMAXPATHLEN];
	char tempFn2[EDMAXPATHLEN];

	if (GetLongPathName(fileName1, tempFn1, sizeof tempFn1) == FALSE) {
		strcpy(tempFn1, fileName1);
	}
	if (GetLongPathName(fileName2, tempFn2, sizeof tempFn2) == FALSE) {
		strcpy(tempFn2, fileName2);
	}
	return lstrcmpi(tempFn1, tempFn2);
}

