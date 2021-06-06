/*
 * FileBufferIO.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: reading and writing file buffers
 *
 * 										created: 10.02.87
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "stringutil.h"

#include "pksedit.h"

#include "edexec.h"
#include "encryption.h"
#include "errordialogs.h"
#include "fileutil.h"
#include "documenttypes.h"
#include "mouseutil.h"

/* #define DEMO 1 /* D E M O V E R S I O N */

/*----- EXTERNALS -----------*/

extern void ft_settime(EDTIME* tp);
extern int  CryptDialog(LPSTR password, int twice);

extern long _flushsize;

int _flushing;

/*----- LOCALS --------------*/

static char _eof[] = "- eof -\n";
static int  _verbose = 1;
static int 	_scratchlen = 0;
static char _crypting;
static unsigned char* _scratchstart;

/*------------------------------*/
/* ft_initializeReadWriteBuffers()			  */
/*------------------------------*/
BOOL ft_initializeReadWriteBuffers(void)
{
	if ((_linebuf = _alloc(LINEBUFSIZE)) == 0) {
		return FALSE;
	}
	_scratchstart = _linebuf+FBUFSIZE;
	return TRUE;
}

/*--------------------------------------
 * Returns the default document type descriptor for situations,
 * where no document type descriptor was assigned to a file.
 */
static DOCUMENT_DESCRIPTOR* GetDefaultDocumentTypeDescriptor() {
	static DOCUMENT_DESCRIPTOR* _defaultDocumentTypeDescriptor;
	if (_defaultDocumentTypeDescriptor == NULL) {
		_defaultDocumentTypeDescriptor = doctypes_createDefaultDocumentTypeDescriptor();
	}
	return _defaultDocumentTypeDescriptor;
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
	_free(((P2LIST*)elem)->p1);
	_free(((P2LIST*)elem)->p2);
	return 1;
}

/*---------------------------------*/
/* list2_insert()				*/
/*---------------------------------*/
static void list2_insert(P2LIST **head, char *p1, char *p2)
{	P2LIST *pl;

	if ((p1 = string_allocate(p1)) != 0 &&
	    (p2 = string_allocate(p2)) != 0 &&
	    (pl = (P2LIST*)ll_insert((LINKED_LIST**)head,sizeof *pl)) != 0) {
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
 * NoDiskSpace()
 */
void NoDiskSpace(void) {
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
 * OpenError()
 */
void OpenError(char *fname)
{
	error_openingFileFailed(fname,0);
}

/*---------------------------------
 * ft_readDocumentFromFile()
 * Standard implementation to read a file in PKS Edit given the file descriptor,
 * a callback method to invoked for each line read and an optional parameter (typically, but not neccessarily the filepointer itself) to
 * be parsed as the first argument to the callback.
 *---------------------------------*/
EXPORT int ft_readDocumentFromFile(int fd, unsigned char * (*lineExtractedCallback)(FTABLE *, DOCUMENT_DESCRIPTOR*, unsigned char *, unsigned char *), void *par)
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

		if ((q = (*lineExtractedCallback)(par,GetDefaultDocumentTypeDescriptor(),&bufferStart[-len],pend)) == 0)
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
static int ft_initializeEncryption(DOCUMENT_DESCRIPTOR *linp, char *pw, int twice)
{
	if ((linp->workmode & O_CRYPTED) == 0 ||
	    CryptDialog(pw, twice) == 0 ||
	    crypt_init(DES_NOFINAL,pw) == 0) {
		pw[0] = 0;
		return 0;
	}
	mouse_setBusyCursor();
	return 1;
}

/*--------------------------------------*/
/* ft_readfile()						*/
/*--------------------------------------*/
EXPORT int ft_readfile(FTABLE *fp, DOCUMENT_DESCRIPTOR *documentDescriptor)
{
	HANDLE			fileHandle;
	int				ret;
	int				nl;
	int				fd;
	register	char *	buf;
	char				pw[32];
	unsigned char *(*f)(FTABLE *par, DOCUMENT_DESCRIPTOR *linp, unsigned char *p, unsigned char *pend);

	fd = -1;
	fp->longLinesSplit = 0;
	_flushing = 0;
	fp->caret.linePointer = fp->firstl;
	buf = _scratchstart;
	if (_verbose)
		mouse_setBusyCursor();

	f = ln_createMultipleLinesFromBuffer;
	if (documentDescriptor == 0) {
		documentDescriptor = GetDefaultDocumentTypeDescriptor();
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
		fp->ti_created = file_getAccessTime(fp->fname);
		fileHandle = CreateFile(fp->fname, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); 
		if (fileHandle == INVALID_HANDLE_VALUE) {
			if ((fd = file_openFile(fp->fname)) < 0) {
				ret = 0;
				goto readerr;
			}
			documentDescriptor->workmode |= O_RDONLY;
		}
		else {
			fd = (int)(uintptr_t)fileHandle;
		}
		_crypting = ft_initializeEncryption(documentDescriptor, pw, 0);
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
			fp->caret.linePointer->lflg |= LNNOTERM;
		}
	}

	if (!ln_createAndAdd(fp,_eof,sizeof _eof -2,LNNOTERM))
		goto ret0;

	if (fp->longLinesSplit > 0 && _verbose) {
		error_showErrorById(IDS_MSGWRAPPEDLINES);
		fp->flags |= F_CHANGEMARK;
	}

	fp->lastl = fp->caret.linePointer;
	fp->caret.linePointer = fp->firstl;

readerr:

	if ((GetConfiguration()->options & O_LOCKFILES) == 0) {
		file_closeFile(&fd);
	}

	fp->lockFd = fd;
	if (_verbose) {
		mouse_setDefaultCursor();
	}
	return ret;
}

/*--------------------------------------*/
/* createBackupFile()					*/
/* create a Backup-File				*/
/*--------------------------------------*/
static void createBackupFile(char *name, char *bak)
{	char backname[256];
	char *ext,*bext = "BAK";

	strcpy(backname,name);
	ext = string_getFileExtension(backname);
	if (ext > backname && ext[-1] != '.') {
		*ext++ = '.';
		*ext = 0;
	}

	if (bak[0])
		bext = bak;
	strcpy(ext,bext); 
	Fdelete(backname);
	Frename(0,name,backname);
}

/*---------------------------------*/
/* ft_flushFile()					*/
/*---------------------------------*/
extern long _flushsize;
static int ft_flushFile(int fd, int size, int rest)
{
	return rsc_flushBuffer(fd,_linebuf,size,rest);
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

	return ft_flushFile(fd,size,rest);
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
EXPORT int ft_writefileMode(FTABLE *fp, int quiet)
{	int			offset;
	int			no;
	int			ret;
	LINE *		lp;
	char		pw[32];
	int 		nl;
	int			cr;
	int			fd = -1;
	int			saveLockFd;
	DOCUMENT_DESCRIPTOR		*linp = fp->documentDescriptor;

#ifdef DEMO
	return 0;
#else
	if (!linp) {
		linp = GetDefaultDocumentTypeDescriptor();
	}

	if ((saveLockFd = fp->lockFd) > 0) {
		file_closeFile(&fp->lockFd);
	}
	lp = fp->firstl;
	if (! (fp->flags & (F_MODIFIED | F_NEWFILE | F_WFORCED | F_APPEND))) 
		return 1;
	nl = linp->nl;
	cr = linp->cr;
	ft_initializeEncryption(linp,pw,1);

	if (!quiet) {
		char* printedFilename = ft_getBasenameOf(fp);
		error_showMessageInStatusbar((fp->flags & F_NEWFILE) ? IDS_MSGNEWFILE : IDS_MSGSAVEFILE,
				  printedFilename, fp->nlines);
		mouse_setBusyCursor();
	}
	if (linp->bak[0] &&
	    !(fp-> flags & (F_NEWFILE | F_SAVEAS | F_APPEND | F_ISBACKUPPED))) {
		createBackupFile(fp->fname, linp->bak);
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
		_flushsize = 0;
	} else {
		if ((_flushsize = Fseek(0l,fd,2)) < 0L) {
			ret = 100;
			error_displayGenericErrorNumber(-_flushsize);
			goto wfail1;
		}
	}
	fp->flags &= ~(F_APPEND|F_NEWFILE);
	offset = 0;
	while (lp != fp->lastl) {		/* don't save last line			  */
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
			if (!ft_flushBufferAndCrypt(fd,no,offset,1,pw))
				goto wfail;
		}
	}
	if (!ft_flushBufferAndCrypt(fd,offset,0,0,pw)) {
wfail:	fp->flags |= F_CHANGEMARK;
		ret = 100;
		goto wfail1;
	}
	ret = 0;
	fp->flags &= ~(F_CHANGEMARK | F_WFORCED);
	ft_settime(&fp->ti_saved);

wfail1:
	file_closeFile(&fd);
	if (saveLockFd > 0) {
		fp->lockFd = file_openFile(fp->fname);
	}
	if (!quiet)
		mouse_setDefaultCursor();
	if (ret) {
		if (_flushsize == -1) {	/* delete Files, if Disk is full */
			Fdelete(fp->fname);
		}
		return 0;
	}
	fp->ti_created = file_getAccessTime(fp->fname);
	return 1;
#endif
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
EXPORT int ft_writefileAsWithFlags(FTABLE *fp,char *fn,int flags)
{	int ret;

	fp->lockFd = -1;
	fp->flags	= (flags | F_MODIFIED);
	if (!(fp->flags & F_APPEND)) {
		fp->flags  |= F_SAVEAS;
	}
	strcpy(fp->fname,fn);
	ret = ft_writefileMode(fp,1);
	return ret;
}

/*---------------------------------*/
/* read a file, which is never	*/
/* been redrawn on screen		*/
/* Macrofiles, ...				*/
/*---------------------------------*/
EXPORT int ft_readfileWithOptions(FTABLE *fp,char *fn,int linflag)
{	int ret = 0;

	/* linflag == 0 => take document descriptor as usually */
	/* linflag <  0 => document descriptor is DEFAULT with '\n' as Terminator */

	fp->firstl = 0;
	fp->nlines = 0L;
	fp->documentDescriptor    = 0;
	fp->flags  = F_NORMOPEN;
	fp->lockFd = 0;
	strcpy(fp->fname,fn);

	if (linflag >= 0 && !doctypes_assignDocumentTypeDescriptor(fp,(DOCUMENT_DESCRIPTOR*)0))
		return 0;

	_verbose = 0;
	ret = ft_readfile(fp, fp->documentDescriptor);
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
	ret = ft_writefileAsWithFlags(fp,name,flags);
	ln_listfree(fp->firstl);
	fp->firstl = 0;
	fp->caret.linePointer  = 0;
	return ret;
}

