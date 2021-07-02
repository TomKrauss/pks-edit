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
#include "lineoperations.h"
#include "pksedit.h"
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
EXPORT int file_getFileMode(char *s)
{
	if (dostat(s) < 0) {
		return -1;
	}
	return _dta.attrib;
}


static EDTIME ConvertFileTimeToLTime(FILETIME *pTime) {
	WORD		wDate;
	WORD		wTime;
	EDTIME		tempResult;

	FileTimeToDosDateTime(pTime, &wDate, &wTime);
	tempResult = (EDTIME)wDate;
	tempResult <<= 16;
	tempResult += wTime;
	return tempResult;
}

/**
 * Returns the last time, a file was accessed.
 */
EDTIME file_getAccessTime(char *fname) {
	HANDLE				lHandle;
	EDTIME				lTime;
	WIN32_FIND_DATA		findData;

	if ((lHandle = FindFirstFile(fname, &findData)) == INVALID_HANDLE_VALUE) {
		return 0;
	}
	lTime = ConvertFileTimeToLTime(&findData.ftLastWriteTime);
	FindClose(lHandle);
	return lTime;
}

/*--------------------------------------------------------------------------
 * file_isDirectory()
 * Returns true if the passed filename points to a directory.
 */
EXPORT int file_isDirectory(char *filename)
{
	int ret;

	return ((ret = file_getFileMode(filename)) >= 0 && (ret & _A_SUBDIR) != 0);
}

/*--------------------------------------------------------------------------
 * file_openFile()
 * Open a file and return the file handle. If it cannot be opened, display an error.
 */
EXPORT int file_openFile(char *fn) {	
	int fd;

	if ((fd = Fopen(fn, OF_READ)) < 0) {
		error_openingFileFailed(fn,fd);
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
				if (errorDisplayYesNoConfirmation(IDS_MSGWPROTECT,(LPSTR)string_abbreviateFileName(fn)) == IDYES) {
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
 * file_searchFileInPKSEditLocation()
 * Searches a file in a "wellknown" PKS Edit location (PKS_SYS, home etc...) and
 * returns the result - this is not re-entrant. Before calling again, one must
 * save the result.
 */
EXPORT char *file_searchFileInPKSEditLocation(char *s)
{
	if (dostat(s)) {
		return s;
	}
	if (pathstat(_pksSysFolder, s)) {
		return _found;
	}
	if (pathstat("PKS_SYS", s)) {
		return _found;
	}
	return 0;
}


/*--------------------------------------------------------------------------
 * file_searchFileInPath()
 * Search a file in a path list (pathes separated by ; or ,).
 */
EXPORT char *file_searchFileInPath(char *fn,char *path) {	
	char p[1024],p2[1024];

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

	strcpy(temp, "PKS");
	temp[1] = c;
	GetTempPath(sizeof tempPath, tempPath);
	GetTempFileName(tempPath, temp, LOWORD((intptr_t)hInst), dst);
	return dst;
}

/*------------------------------------------------------------
 * file_getTempDirectory()
 * Returns the path to the temp directory.
 */
EXPORT char *file_getTempDirectory(void)
{	static char tmpdir[1024];

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
{	char tmpname[1024];
	char pathname[1024];
	char fn[1024];
	char *szBang;

	file_getTempFilename(tmpname,'!');
	fn[0] = 0;
	string_splitFilename(tmpname,pathname,fn);
	if ((szBang = strchr(fn, '!')) != 0) {
		*szBang = '?';
		_ftw(pathname, file_removeCb,1,fn,0xFF);
	}
}
