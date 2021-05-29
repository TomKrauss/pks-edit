/*
 * FileUtil.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: find, open, create Files
 *
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include <tos.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include "winterf.h"
#include "lineoperations.h"
#include "pksedit.h"
#include "errordialogs.h"
#include "stringutil.h"

extern char *_datadir;
static char _found[1024];

#if !defined(S_IREAD)
#define		S_IREAD		_S_IREAD
#define		S_IWRITE	_S_IWRITE
#endif

#if defined(WIN32)

#define	dostat(s)	(EdStat(s) == 0)
static struct _finddata_t _dta;

/*--------------------------------------------------------------------------
 * EdStat()
 */
EXPORT int EdStat(char *s) 
{
	intptr_t handle;

	if ((handle = _findfirst(s,&_dta)) < 0) {
		return -1;
	}
	_findclose(handle);
	return 0;
}

/*--------------------------------------------------------------------------
 * EdFileMode()
 */
EXPORT int EdFileMode(char *s)
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


EDTIME EdGetFileTime(char *fname) {
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
 * EdIsDir()
 */
EXPORT int EdIsDir(char *s)
{
	int ret;

	return ((ret = EdFileMode(s)) >= 0 && (ret & _A_SUBDIR) != 0);
}

#else
#define	dostat(s)	(EdStat(s, 0xFF) == 0)
static struct  ffblk _dta;

/*--------------------------------------------------------------------------
 * EdStat()
 */
EXPORT int EdStat(char *s, int mode) 
{
	return findfirst(s,&_dta,mode);
}

/*--------------------------------------------------------------------------
 * EdFileMode()
 */
EXPORT int EdFileMode(char *s)
{
	if (dostat(s) < 0)
		return -1;
	return _dta.ff_attrib;
}

/*--------------------------------------------------------------------------
 * EdIsDir()
 */
EXPORT int EdIsDir(char *s)
{
	int ret;

	return ((ret = EdFileMode(s)) >= 0 && (ret & FA_DIREC) != 0);
}

#endif

/*--------------------------------------------------------------------------
 * EdOpen()
 */
EXPORT int EdOpen(char *fn) {	
	int fd;

	if ((fd = Fopen(fn, OF_READ)) < 0) {
		tosfnerror(fn,fd);
	}
	return fd;
}

EXPORT int EdMakeReadable(char *fn) {
	return _chmod(fn,S_IREAD|S_IWRITE);
}

/*--------------------------------------------------------------------------
 * EdCreateWithMode()
 */
EXPORT int EdCreateWithMode(char *fn,int mode)
{
	int fd;

	while(1) {
		if ((fd = Fcreate(fn,mode)) < 0) {
			if ( EdFileMode(fn) < 0) {
				tosfnerror(fn,fd);
				break;
			} else {
				if (ed_yn(IDS_MSGWPROTECT,(LPSTR)AbbrevName(fn)) == IDYES) {
					if (EdMakeReadable(fn) < 0) {
						ed_error(IDS_MSGERRCHMOD);
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
 * EdCreate()
 */
EXPORT int EdCreate(char *fn)
{
	return EdCreateWithMode(fn,0);
}

/*--------------------------------------------------------------------------
 * pathstat()
 */
static int pathstat(char *path,char *fn)
{
	if (path) {
		strdcpy(_found,path,fn);
		return (dostat(_found));
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * searchfile()
 */
EXPORT char *searchfile(char *s)
{
	if (dostat(s)) {
		return s;
	}
	if (pathstat(_datadir, s)) {
		return _found;
	}
	if (pathstat("PKS_SYS", s)) {
		return _found;
	}
	return 0;
}


/*--------------------------------------------------------------------------
 * pathsearch()
 */
EXPORT char *pathsearch(char *fn,char *path) {	
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
			sfsplit(path,p2,(char *)0);
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
 * TmpName()
 */
EXPORT char *TmpName(char *dst, char c)
{	char		temp[50];
	char		tempPath[1024];

	strcpy(temp, "PKS");
	temp[1] = c;
#if defined(WIN32)
	GetTempPath(sizeof tempPath, tempPath);
	GetTempFileName(tempPath, temp, (UINT)hInst, dst);
#else
	GetTempFileName(0, temp, (UINT)hInst, dst);
#endif
	return dst;
}

/*------------------------------------------------------------
 * TmpDir()
 */
EXPORT char *TmpDir(void)
{	static char tmpdir[1024];

	if (!tmpdir[0]) {
#if defined(WIN32)
	GetTempPath(sizeof tmpdir, tmpdir);
#else
	char 		dst[1024];
	GetTempFileName(0,"X",(UINT)hInst,dst);
	sfsplit(dst,tmpdir,(char *)0);
#endif
	}
	return tmpdir;
}

/*---------------------------------------------------------------
 * TmpJunk()
 */
EXPORT void TmpJunk(void)
{	char tmpname[1024];
	char pathname[1024];
	char fn[1024];
	char *szBang;

	TmpName(tmpname,'!');
	fn[0] = 0;
	sfsplit(tmpname,pathname,fn);
	if ((szBang = strchr(fn, '!')) != 0) {
		*szBang = '?';
		_ftw(pathname,_unlink,1,fn,0xFF);
	}
}
