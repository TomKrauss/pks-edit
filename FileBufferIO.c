/*
 * FileBufferIO.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: reading and writing file buffers
 *
 * 										created: 10.02.87
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. 
 */

#include <stdio.h>
#include "alloc.h"
#include <tos.h>
#include "trace.h"
#include "documentmodel.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "stringutil.h"
#include "linkedlist.h"
#include "winfo.h"

#include "pksedit.h"
#include "editorconfiguration.h"
#include "edexec.h"
#include "encryption.h"
#include "errordialogs.h"
#include "fileutil.h"
#include "documenttypes.h"
#include "mouseutil.h"
#include "resource.h"

/* #define DEMO 1 /* D E M O V E R S I O N */

/*----- EXTERNALS -----------*/

extern void ft_settime(EDTIME* tp);
extern int  dlg_enterPasswordForEncryption(LPSTR password, LPSTR pszFilename, int twice);

int _flushing;

/*----- LOCALS --------------*/

static char _eof[] = "- eof -\n";
static char _cryptMagic[8] = "\0enc\03\07\01\0";
static int  _verbose = 1;
static int 	_scratchlen = 0;
static char _crypting;
static unsigned char* _scratchstart;

/*------------------------------*/
/* ft_initializeReadWriteBuffers()			  */
/*------------------------------*/
BOOL ft_initializeReadWriteBuffers(void)
{
	if ((_linebuf = malloc(LINEBUFSIZE)) == 0) {
		return FALSE;
	}
	_scratchstart = _linebuf+FBUFSIZE;
	return TRUE;
}

/*
 * Release the scratch buffer as soon as we are done. 
 */
void ft_destroyCaches() {
	free(_linebuf);
	_linebuf = NULL;
}

/*---------------------------------*/
/* list_mk()					*/
/*---------------------------------*/
typedef struct p2list {
	struct p2list *next;
	char *p1,*p2;
} P2LIST;

static P2LIST *_p2list;

/*---------------------------------*/
/* list2_destroy()				*/
/*---------------------------------*/
static int list2_destroy(void *elem)
{
	free(((P2LIST*)elem)->p1);
	free(((P2LIST*)elem)->p2);
	return 1;
}

/*---------------------------------*/
/* list2_insert()				*/
/*---------------------------------*/
static void list2_insert(P2LIST **head, char *p1, char *p2)
{	P2LIST *pl;

	if ((p1 = _strdup(p1)) != 0 &&
	    (p2 = _strdup(p2)) != 0 &&
	    (pl = ll_insert(head,sizeof *pl)) != 0) {
		pl->p1 = p1;
		pl->p2 = p2;
	}
}

/*---------------------------------*/
/* list2_mk()					*/
/*---------------------------------*/
static void list2_mk(P2LIST **head, char *p1, char *p2)
{
	if (p1 == 0) {
		ll_destroy((LINKED_LIST**)head,list2_destroy);
	} else {
		list2_insert(head,p1,p2);
	}
}

/*---------------------------------*/
/* listPathes_mk()				*/
/*---------------------------------*/
static char *szAltpath = "altpath";
static intptr_t listPathes_mk(char *szPath, long unused)
{
	char	 szBuf[256];

	prof_getPksProfileString(szAltpath, szPath, szBuf, sizeof szBuf);
	list2_mk(&_p2list,szPath,szBuf);
	return 0;
}

/*--------------------------------------------------------------------------
 * ft_initializeAlternatePath()
 */
static int ft_initializeAlternatePath(void)
{
	return prof_enum(szAltpath,listPathes_mk,0L);
}

/*---------------------------------
 * file_closeFile()
 * close a file handle and report an error if unsuccessful. 
/*---------------------------------*/
EXPORT int file_closeFile(int *fd) {
	int ret;

	if (*fd > 0 && (ret = Fclose(*fd)) < 0) {
		error_displayGenericErrorNumber(-ret);
	}
	*fd = -1;
	return ret;
}

/*--------------------------------------------------------------------------
 * err_noDiskSpace()
 */
void err_noDiskSpace(void) {
	error_showErrorById(IDS_MSGNODISKSPACE);
}

/*--------------------------------------------------------------------------
 * err_writeErrorOcurred()
 */
void err_writeErrorOcurred(void)
{
	error_showErrorById(IDS_MSGWRITEERR);
}

/*--------------------------------------------------------------------------
 * err_openFailed()
 */
void err_openFailed(char *fname)
{
	error_openingFileFailed(fname,0);
}

/*
 * Read the file identified by the file descriptor into a single 0-terminated string and return it.
 * Returns NULL, if the file is too large and malloc fails. It is the responsibility of the caller
 * to free the memory returned.
 */
EXPORT char* file_readFileAsString(int fd) {
	size_t	bufSize = 0;
	size_t	bytesRead;
	size_t  nOffset = 0;
	char* pszBuf = NULL;

	while ((bytesRead = Fread(fd, FBUFSIZE, _linebuf)) > 0) {
		if (pszBuf == NULL) {
			bufSize = bytesRead;
			pszBuf = malloc(bufSize+1);
			if (pszBuf == NULL) {
				return NULL;
			}
		} else {
			bufSize += bytesRead;
			pszBuf = realloc(pszBuf, bufSize + 1);
		}
		memcpy(pszBuf+nOffset, _linebuf, bytesRead);
		nOffset = bytesRead;
	}
	if (pszBuf) {
		pszBuf[bufSize] = 0;
	}
	return pszBuf;
}

/*---------------------------------
 * ft_readDocumentFromFile()
 * Standard implementation to read a file in PKS Edit given the file descriptor,
 * a callback method to invoked for each line read and an optional parameter (typically, but not neccessarily the filepointer itself) to
 * be parsed as the first argument to the callback.
 *---------------------------------*/
EXPORT int ft_readDocumentFromFile(int fd, unsigned char * (*lineExtractedCallback)(void *, EDIT_CONFIGURATION*, unsigned char *, unsigned char *), void *par)
{
	int 	cflg,got,len,ofs;
	long	bufferSize;
	char	*q;
	unsigned char *pend;
	unsigned char *bufferStart;

	len = 0;
	bufferSize = 2*FBUFSIZE;
	bufferStart = &_linebuf[128];
	_scratchlen = 0;
	cflg = _crypting;
	_crypting = 0;
	while ((got = Fread(fd,bufferSize,bufferStart)) > 0) {
		if (cflg) {
			char buf[2];
			int cont = Fread(fd,1,buf);

			if (cont)
				Fseek(-1l,fd,1);
			if ((got = crypt_de(bufferStart, got, cont)) < 0) {
				error_showErrorById(IDS_MSGWRONGPW);
				return 0;
			}
		}

		ofs  = bufferSize;
		pend = &bufferStart[got];

		if ((q = (*lineExtractedCallback)(par,doctypes_getDefaultDocumentTypeDescriptor(),&bufferStart[-len],pend)) == 0)
			return 0;
		if ((len = (int)(pend-q)) != 0) {
			if (len < 128) {
				bufferStart = _linebuf+128;
				bufferSize = 2*FBUFSIZE;
			} else {
				bufferStart = _linebuf+FBUFSIZE;
				bufferSize = FBUFSIZE;
			}
			memmove(&bufferStart[-len],q,len);
		}
		if (ofs != got)
			break;
	}
	_scratchstart 	= bufferStart;
	_scratchlen	= len;

	if (got < 0) {
		error_showErrorById(IDS_MSGREADERROR); 
		return 0;
	}
	return 1;
}

/*--------------------------------------
 * ft_initializeEncryption()
 * Checks, whether a file to read needs to be decrypted.
 * If not, return 0, return 1 otherwise.
 *--------------------------------------*/
static int ft_initializeEncryption(EDIT_CONFIGURATION *linp, char *pw, char* pszFilename, int twice)
{
	if ((linp->workmode & O_CRYPTED) == 0 ||
	    dlg_enterPasswordForEncryption(pw, pszFilename, twice) == 0 ||
	    crypt_init(DES_NOFINAL,pw) == 0) {
		pw[0] = 0;
		return 0;
	}
	mouse_setBusyCursor();
	return 1;
}

/*
 * Read the header of a file to auto-detect, whether is encrypted. 
 */
static void ft_handleMagic(int fd, EDIT_CONFIGURATION* documentDescriptor) {
	char szMagic[sizeof _cryptMagic];
	BOOL bHeaderHandled = FALSE;
	if (Fread(fd, sizeof szMagic, szMagic) >= 8) {
		if (memcmp(szMagic, _cryptMagic, sizeof _cryptMagic) == 0) {
			documentDescriptor->workmode |= O_CRYPTED;
			_llseek(fd, sizeof _cryptMagic, SEEK_SET);
			bHeaderHandled = TRUE;
		}
		else if (szMagic[0] == '\xEF' && szMagic[1] == '\xBB' && szMagic[2] == '\xBF') {
			// UTF8 encoded - skip BOM when reading.
			_llseek(fd, 3, SEEK_SET);
			bHeaderHandled = TRUE;
			documentDescriptor->codepage = CP_UTF8;
		}
	}
	if (!bHeaderHandled) {
		_llseek(fd, 0, SEEK_SET);
	}
}

/*--------------------------------------*/
/* ft_readfile()						*/
/*--------------------------------------*/
EXPORT int ft_readfile(FTABLE *fp, EDIT_CONFIGURATION *documentDescriptor, long fFileOffset)
{
	HANDLE			fileHandle;
	int				ret;
	int				nl;
	int				fd;
	register	char *	buf;
	char				pw[64];
	unsigned char *(*f)(FTABLE *par, EDIT_CONFIGURATION *linp, unsigned char *p, unsigned char *pend);

	fd = -1;
	fp->longLinesSplit = 0;
	_flushing = 0;
	fp->lpReadPointer = fp->firstl;
	int workmode = documentDescriptor ? documentDescriptor->workmode : 0;
	if (workmode & WM_WATCH_LOGFILE) {
		fp->flags |= F_WATCH_LOGFILE;
	} else {
		fp->flags &= ~F_WATCH_LOGFILE;
	}
	buf = _scratchstart;
	if (_verbose)
		mouse_setBusyCursor();

	f = ln_createMultipleLinesFromBuffer;
	if (documentDescriptor == 0) {
		documentDescriptor = doctypes_getDefaultDocumentTypeDescriptor();
	}

	if ((nl = documentDescriptor->nl) < 0)
		f = ln_createFromBuffer;

	if (fp->flags & F_NEWFILE) {
nullfile:
		if (!ln_createAndAdd(fp,"",0,(nl < 0) ? LNNOTERM : 0)) {
			goto ret0;
		}
		fp->nlines = 1;
	} else {
		file_getAccessTime(fp->fname, &fp->ti_created, &fp->ti_modified);
		fileHandle = CreateFile(fp->fname, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); 
		if (fileHandle == INVALID_HANDLE_VALUE) {
			if ((fd = file_openFile(fp->fname)) < 0) {
				ret = 0;
				goto readerr;
			}
			fp->flags |= F_RDONLY;
		}
		else {
			fd = (int)(uintptr_t)fileHandle;
		}
		if (fFileOffset) {
			LARGE_INTEGER fileSize;
			if (!GetFileSizeEx(fileHandle, &fileSize) || fileSize.QuadPart < fFileOffset) {
				CloseHandle(fileHandle);
				// File was truncated.
				return 0;
			}
			if (_llseek(fd, fFileOffset, SEEK_SET) != fFileOffset) {
				CloseHandle(fileHandle);
				return 0;
			}
		} else {
			ft_handleMagic(fd, documentDescriptor);
		}
		_crypting = FALSE;
		if (documentDescriptor->workmode & O_CRYPTED) {
			_crypting = ft_initializeEncryption(documentDescriptor, pw, fp->fname, 0);
			if (!_crypting) {
				return 0;
			}
		}
		if ((ret = ft_readDocumentFromFile(fd,f,fp)) == 0) {
			goto readerr;
		}

		if (fp->nlines == 0 && _scratchlen == 0) {	/* 0-file glitch */
			goto nullfile;
		}
		if (_scratchlen) {					/* unterminated line */
			_flushing = 1;
			buf = _scratchstart;
			if (nl >= 0)
				*buf++ = nl;
			if ((*f)(fp,documentDescriptor, &_scratchstart[-_scratchlen],buf) == 0) {
ret0:			ret = 0;
				goto readerr;
			}
			fp->lpReadPointer->lflg |= LNNOTERM;
		}
	}

	if (!ln_createAndAdd(fp,_eof,sizeof _eof -2,LNNOTERM))
		goto ret0;

	if (fp->longLinesSplit > 0 && _verbose) {
		error_showErrorById(IDS_MSGWRAPPEDLINES);
		ft_setFlags(fp, fp->flags | F_CHANGEMARK);
	}

	fp->lastl = fp->lpReadPointer;
	fp->lpReadPointer = fp->firstl;

readerr:

	fp->fileSize = _llseek(fd, 0, SEEK_END);
	if ((GetConfiguration()->options & O_LOCKFILES) == 0) {
		file_closeFile(&fd);
	}

	fp->lockFd = fd;
	if (_verbose) {
		mouse_setDefaultCursor();
	}
	return ret;
}

/*
 * Calculate the name of the backup file to create for file fp. The pszResult
 * string must be big enough to hold EDMAXPATHLEN number of characters.
*/
void ft_getBackupFilename(FTABLE* fp, char* pszResult) {
	char* name = fp->fname;
	char* backupExtension = fp->documentDescriptor->backupExtension;
	if (GetConfiguration()->options & O_CREATE_BACKUP_IN_TEMP_PATH) {
		char* fname = string_getBaseFilename(name);
		string_concatPathAndFilename(pszResult, config_getPKSEditTempPath(), fname);
	}
	else {
		strcpy(pszResult, name);
	}
	if (backupExtension[0]) {
		strcat(pszResult, ".");
		strcat(pszResult, backupExtension);
	}
	else {
		strcat(pszResult, ".bak");
	}
}

/*--------------------------------------*/
/* createBackupFile()					*/
/* create a Backup-File				*/
/*--------------------------------------*/
static void createBackupFile(FTABLE* fp, char* pszBackupName) {	
	char* name = fp->fname;

	Fdelete(pszBackupName);
	// In contrast to the POSIX rename, rename on windows also allows to move files to
	// different file systems.
	if (Frename(0, name, pszBackupName)) {
		error_displayAlertDialog("Cannot rename %s to create backup file %s, errno: %d", name, pszBackupName, errno);
	}
}

/*--------------------------------------------------------------------------
 * file_flushBuffer()
 * Flush a resource file buffer writing out the number of specified bytes and
 * moving the ramainder of the unflushed data to the beginning of the buffer.
 * Return -2, if the disk is full, 1 upon success and 0 for other failures.
 */
int file_flushBuffer(int fd, char* buffer, int size, int rest)
{	
	UINT lWritten;
	if (!size) {
		return 1;
	}
	lWritten = Fwrite(fd, size, buffer);
	if (lWritten != size) {
		if (GetLastError() == ERROR_DISK_FULL) {
			err_noDiskSpace();
			return -2;
		}
		else {
			err_writeErrorOcurred();
		}
	} else {
		if (rest) {
			memmove(buffer, &buffer[size], rest);
		}
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * ft_flushBufferAndCrypt()
 */
static int ft_flushBufferAndCrypt(int fd, int size, int rest, int cont, char *pw)
{
	if (*pw) {
		int news;

		news = crypt_en(_linebuf,size,cont);
		if (cont && news != size) {
			error_displayAlertDialog("bad flushbuffer size");
			return 0;
		}
		size = news;
	}

	return file_flushBuffer(fd, _linebuf, size, rest);
}

/*--------------------------------------------------------------------------
 * ft_getBasenameOf()
 */
static char* ft_getBasenameOf(FTABLE* fp) {
	return string_getBaseFilename(ft_visiblename(fp));
}

/*--------------------------------------*/
/* ft_writefileMode() 					*/
/*--------------------------------------*/
EXPORT int ft_writefileMode(FTABLE *fp, int flags)
{	int			offset;
	int			no;
	int			ret;
	LINE *		lp;
	char		backupFile[EDMAXPATHLEN];
	char		pw[64];
	int 		nl;
	int			cr;
	int			fd = -1;
	int			saveLockFd;
	int			nFlushResult = 0;
	EDIT_CONFIGURATION		*linp = fp->documentDescriptor;

#ifdef DEMO
	return 0;
#else
	BOOL quiet = flags & WFM_QUIET;
	if (!linp) {
		linp = doctypes_getDefaultDocumentTypeDescriptor();
	}

	if ((saveLockFd = fp->lockFd) > 0) {
		file_closeFile(&fp->lockFd);
	}
	lp = fp->firstl;
	if (! (fp->flags & (F_MODIFIED | F_NEWFILE | F_WFORCED | F_APPEND))) 
		return 1;
	nl = linp->nl;
	cr = linp->cr;
	ft_initializeEncryption(linp, pw, fp->fname, 1);

	if (!quiet) {
		char* printedFilename = ft_getBasenameOf(fp);
		error_showMessageInStatusbar((fp->flags & F_NEWFILE) ? IDS_MSGNEWFILE : IDS_MSGSAVEFILE,
				  printedFilename, fp->nlines);
		mouse_setBusyCursor();
	}
	if (!(flags & WFM_AUTOSAVING) &&
		linp->backupExtension[0] &&
	    !(fp-> flags & (F_NEWFILE | F_SAVEAS | F_APPEND | F_ISBACKUPPED))) {
		ft_getBackupFilename(fp, backupFile);
		createBackupFile(fp, backupFile);
		fp->flags |= F_ISBACKUPPED;
	}

	if ( (fp->flags & F_APPEND) == 0 || 
	     ((fd = Fopen(fp->fname,2)) < 0)) {
		int crflags = 0;

		if (fp->flags & F_HIDDEN)
			crflags |= F_HIDDEN;
		fd = Fcreate(fp->fname, crflags);
		if (fd < 0) {
			file_makeFileReadWrite(fp->fname);
			fd = Fcreate(fp->fname, crflags);
		}
		if (fd < 0) {
			error_openingFileFailed(fp->fname,fd);
			ret = 100;
			goto wfail1;
		}
	} else {
		LONG flushSize;
		if ((flushSize = Fseek(0l,fd,2)) < 0L) {
			ret = 100;
			nFlushResult = -2;
			error_displayGenericErrorNumber(-flushSize);
			goto wfail1;
		}
	}
	fp->flags &= ~(F_APPEND|F_NEWFILE);
	offset = 0;
	if (pw[0]) {
		file_flushBuffer(fd, _cryptMagic, 8, 0);
	}
	while (lp != fp->lastl) {		// don't save last line
		if ((no = offset+lp->len) < LINEBUFSIZE) {
			memmove(&_linebuf[offset],lp->lbuf,lp->len);
			offset = no;
			if (LINE_HAS_LINE_END(lp)) {
				if (cr >= 0 && LINE_HAS_CR(lp)) {
					_linebuf[offset++] = cr;
				}
				if (nl >= 0) {
					_linebuf[offset++] = nl;
				}
			}
			lp = lp->next;
		} else {
			no = (offset > 2*FBUFSIZE) ? 2*FBUFSIZE : offset & (~0xF);
			offset -= no;
			nFlushResult = ft_flushBufferAndCrypt(fd, no, offset, 1, pw);
			if (nFlushResult < 1)
				goto wfail;
		}
	}
	nFlushResult = ft_flushBufferAndCrypt(fd, offset, 0, 0, pw);
	if (nFlushResult < 1) {
	wfail:	
		ft_setFlags(fp, fp->flags | F_CHANGEMARK);
		ret = 100;
		goto wfail1;
	}
	ret = 0;
	if (!(flags & WFM_AUTOSAVING)) {
		ln_changeFlag(fp, fp->firstl, fp->lastl, LNMODIFIED, LNSAVED, 1);
		ln_changeFlag(fp, fp->firstl, fp->lastl, LNMODIFIED, LNUNDO_AFTER_SAVE, 0);
		ft_forAllViews(fp, render_repaintLineNumbers, NULL);
		ft_setFlags(fp, fp->flags & ~(F_CHANGEMARK | F_WFORCED));
		ft_settime(&fp->ti_saved);
		fp->fileSize = _llseek(fd, 0, SEEK_END);
	}

wfail1:
	file_closeFile(&fd);
	if (saveLockFd > 0) {
		fp->lockFd = file_openFile(fp->fname);
	}
	if (!quiet)
		mouse_setDefaultCursor();
	if (ret) {
		if (nFlushResult == -2) {	/* delete Files, if Disk is full */
			Fdelete(fp->fname);
		}
		return 0;
	}
	file_getAccessTime(fp->fname, &fp->ti_created, &fp->ti_modified);
	return 1;
#endif
}

/*
 * Return TRUE if for the passed file a backup file had been created previously. 
 */
EXPORT BOOL ft_backupFileExists(FTABLE* fp) {
	char		backupFile[EDMAXPATHLEN];

	ft_getBackupFilename(fp, backupFile);
	return file_exists(backupFile) == 0;
}

/*---------------------------------*/
/* ft_writeFileWithAlternateName()			*/
/*---------------------------------*/
EXPORT int ft_writeFileWithAlternateName(FTABLE *fp)
{	char dirname[512],savefn[512],fname[512];
	int  ret,flags;
	P2LIST *pl;

	strcpy(savefn,fp->fname);
	string_splitFilename(savefn,dirname,fname);
	flags = fp->flags;
	pl = _p2list;

	if ((ret = ft_writefileMode(fp,0)) != 0) {
		while(pl) {
			if (strcmp(pl->p1,dirname) == 0) {
				string_concatPathAndFilename(fp->fname,pl->p2,fname);
				fp->flags = flags;
				if ((ret = ft_writefileMode(fp,0)) == 0)
					break;
			}
			pl = pl->next;
		}
	}

	strcpy(fp->fname,savefn);
	return ret;
}

/*---------------------------------*/
/* ft_writefileAsWithFlags()					*/
/*---------------------------------*/
EXPORT int ft_writefileAsWithFlags(FTABLE *fp, char *fn, int fileFlags, BOOL bVerbose)
{	int ret;

	fp->lockFd = -1;
	int newFlags = (fileFlags | F_MODIFIED);
	if (!(fp->flags & F_APPEND)) {
		newFlags  |= F_SAVEAS;
	}
	ft_setFlags(fp, newFlags);
	ft_setOutputFilename(fp, fn);
	ret = ft_writefileMode(fp, bVerbose ? 0 : WFM_QUIET);
	return ret;
}

/*---------------------------------*/
/* read a file, which is never	*/
/* been redrawn on screen		*/
/* Macrofiles, ...				*/
/*---------------------------------*/
EXPORT int ft_readfileWithOptions(FTABLE *fp, FILE_READ_OPTIONS* pOptions) {
	int ret;

	fp->firstl = 0;
	fp->nlines = 0L;
	fp->nbytes = -1;
	fp->documentDescriptor = 0;
	fp->flags  = F_NORMOPEN;
	fp->lockFd = 0;
	strcpy(fp->fname, pOptions->fro_fileName);

	if (!pOptions->fro_useDefaultDocDescriptor && !doctypes_assignDocumentTypeDescriptor(fp,(EDIT_CONFIGURATION*)0))
		return 0;

	_verbose = 0;
	ret = ft_readfile(fp, fp->documentDescriptor, pOptions->fro_fileReadOffset);
	_verbose = 1;
	file_closeFile(&fp->lockFd);

	return ret;
}

/*---------------------------------*/
/* ft_writeFileAndClose()				*/
/*---------------------------------*/
EXPORT int ft_writeFileAndClose(FTABLE *fp,char *name, int flags)
{	
	int 		ret;

	fp->lastl = 0;
	fp->documentDescriptor = 0;
	ret = ft_writefileAsWithFlags(fp,name,flags, FALSE);
	ln_listfree(fp->firstl);
	fp->firstl = 0;
	fp->lpReadPointer = 0;
	return ret;
}

