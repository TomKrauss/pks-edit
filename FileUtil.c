/*
 * FileUtil.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: find, open, create Files
 *
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

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

extern char *_pksSysFolder;
static char _found[1024];

#if !defined(S_IREAD)
#define		S_IREAD		_S_IREAD
#define		S_IWRITE	_S_IWRITE
#endif

#define	dostat(s)	(file_exists(s) == 0)
static struct _finddata_t _dta;

/*--------------------------------------------------------------------------
 * file_exists()
 * Test the existance of a file. If it exists, return 0 otherwise -1.
 */
EXPORT int file_exists(char *s) 
{
	intptr_t handle;

	if ((handle = _findfirst(s,&_dta)) < 0) {
		return -1;
	}
	_findclose(handle);
	return 0;
}

/*--------------------------------------------------------------------------
 * file_getFileMode()
 * Return the "modes" (read write, A_NORMAL, A_READONLY, A_HIDDEN, A_SYSTEM, ...) of a file.
 */
EXPORT int file_getFileMode(char *s) {
	if (!dostat(s)) {
		return -1;
	}
	return _dta.attrib;
}


static EDTIME ConvertFileTimeToLTime(FILETIME *pTime) {
	ULARGE_INTEGER ui;
	ui.LowPart = pTime->dwLowDateTime;
	ui.HighPart = pTime->dwHighDateTime;
	return ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
}

/**
 * Returns the last time, a file was accessed and created.
 * If unsuccessful 0 is returned.
 */
int file_getAccessTime(char *fname, EDTIME* pCreated, EDTIME* pModified) {
	HANDLE				lHandle;
	WIN32_FIND_DATA		findData;

	if ((lHandle = FindFirstFile(fname, &findData)) == INVALID_HANDLE_VALUE) {
		return 0;
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
EXPORT int file_openFile(char *fn) {	
	int fd;

	if ((fd = Fopen(fn, OF_READ)) < 0) {
		if (strlen(fn) >= MAX_PATH) {
			char* pszTemp = malloc(EDMAXPATHLEN);
			strcpy(pszTemp, "\\\\?\\");
			strcat(pszTemp, fn);
			wchar_t* pszWPath = malloc(EDMAXPATHLEN * 2);
			MultiByteToWideChar(CP_ACP, 0, pszTemp, (int) strlen(pszTemp), pszWPath, EDMAXPATHLEN);
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
		if (fd < 0) {
			error_openingFileFailed(fn, fd);
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
				error_openingFileFailed(fn,fd);
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
		string_concatPathAndFilename(_found,path,fn);
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
 * file_searchFileInPKSEditLocation()
 * Searches a file in a "wellknown" PKS Edit location (PKS_SYS, home etc...) and
 * returns the result - this is not re-entrant. Before calling again, one must
 * save the result.
 */
EXPORT char *file_searchFileInPKSEditLocation(const char *s)
{
	if (dostat((char*)s)) {
		return (char*)s;
	}
	if (pathstat(_pksSysFolder, (char*)s)) {
		return _found;
	}
	if (pathstat("PKS_SYS", (char*)s)) {
		return _found;
	}
	return 0;
}


/*--------------------------------------------------------------------------
 * file_searchFileInPath()
 * Search a file in a path list (pathes separated by ; or ,).
 */
EXPORT char *file_searchFileInPath(char *fn,char *path) {	
	char p[EDMAXPATHLEN],p2[EDMAXPATHLEN];

	if (dostat(fn)) {
		return fn;
	}

	if (!path || path[0] == 0)
		return 0;

	strcpy(p,path);

	if ((path = strtok(p,",;")) != 0)
	for (;;) {	
		if (lstrchr(path,'*')) {
			string_splitFilename(path,p2,(char *)0);
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
static int file_removeCb(char* filename, DTA* dta) {
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
	string_splitFilename(tmpname,pathname,fn);
	if ((szBang = strchr(fn, '!')) != 0) {
		*szBang = '?';
		_ftw(pathname, file_removeCb,1,fn,0xFF);
	}
}
