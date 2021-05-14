/*
 * FileBufferIO.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: reading and writing file buffers
 *
 * 										created      : 10.02.87
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <tos.h>
#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "pksedit.h"

#include "edexec.h"
#include "encryption.h"

/* #define DEMO 1 /* D E M O V E R S I O N */

/*----- EXTERNALS -----------*/

extern void alert(LPSTR fmt, ...);
extern LINE *ln_create();
extern char *getfn();
extern unsigned char* stralloc(unsigned char* buf);
extern char *extname(char *s);
extern char *TmpName(char *dst, char c);
extern void ll_kill(void *head,int (*destroy)(void *elem));
extern void *ll_insert(void *head,long size);
extern int  CryptDialog(LPSTR password, int twice);
extern EDTIME EdGetFileTime(char *fname);
extern int EdMakeReadable(char *fn);

extern long _flushsize;

/*----- LOCALS --------------*/

static char _eof[] = "- eof -\n";
static int  _talking = 1;

char * 	_linebuf;
int 	 	_scratchlen = 0;
LINEAL 	*_rlp;

/*------------------------------*/
/* InitBuffers()			  */
/*------------------------------*/
static unsigned char *_scratchstart;
BOOL InitBuffers(void)
{
	if ((_linebuf = _alloc(LINEBUFSIZE)) == 0) {
		return FALSE;
	}
	_scratchstart = _linebuf+FBUFSIZE;
	return TRUE;
}

/*------------------------------*/
/* lnlistfree()			  */
/*------------------------------*/
EXPORT void lnlistfree(register LINE *lp)
{	LINE *lps;

	while ((lps = lp) != 0) {
		lp = lp->next;
		_free(lps);
	}
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

	if ((p1 = stralloc(p1)) != 0 &&
	    (p2 = stralloc(p2)) != 0 &&
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
		ll_kill(head,list2_destroy);
	} else {
		list2_insert(head,p1,p2);
	}
}

/*---------------------------------*/
/* listPathes_mk()				*/
/*---------------------------------*/
static char *szAltpath = "altpath";
static void listPathes_mk(char *szPath)
{
	char	 szBuf[256];

	GetPksProfileString(szAltpath, szPath, szBuf, sizeof szBuf);
	list2_mk(&_p2list,szPath,szBuf);
}

/*--------------------------------------------------------------------------
 * ap_init()
 */
int ap_init(void)
{
	return prof_enum(szAltpath,listPathes_mk,0L);
}

/*--------------------------------------------------------------------------
 * putline()
 */
EXPORT int putline(FTABLE *fp, char *b)
{
	return createl(fp,b,(int)strlen(b),0);
}

/*---------------------------------*/
/* closeF()					*/
/*---------------------------------*/
EXPORT int closeF(int *fd)
{	int ret;

	if (*fd > 0 && (ret = Fclose(*fd)) < 0) {
		form_error(-ret);
	}
	*fd = -1;
	return ret;
}
extern unsigned char *readlines(FTABLE *,  unsigned char *, unsigned char *);
extern unsigned char *readnoterm(FTABLE *, unsigned char *, unsigned char *);

/*--------------------------------------------------------------------------
 * NoDiskSpace()
 */
void NoDiskSpace(void)
{
	ed_error(IDS_MSGNODISKSPACE);
}

/*--------------------------------------------------------------------------
 * WriteError()
 */
void WriteError(void)
{
	ed_error(IDS_MSGWRITEERR);
}

/*--------------------------------------------------------------------------
 * OpenError()
 */
void OpenError(char *fname)
{
	tosfnerror(fname,0);
}

/*---------------------------------*/
/* readfrags()					*/
/*---------------------------------*/
static char _crypting;
EXPORT int readfrags(int fd, unsigned char * (*f)(FTABLE *, unsigned char *, unsigned char *), void *par)
{
	int 	cflg,got,len,ofs;
	long	fragsize;
	char	*q;
	unsigned char *pend;
	unsigned char *fragstart;

	len = 0;
	fragsize = 2*FBUFSIZE;
	fragstart = &_linebuf[128];
	_scratchlen = 0;
	cflg = _crypting;
	_crypting = 0;
	while ((got = Fread(fd,fragsize,fragstart)) > 0) {
		if (cflg) {
			char buf[2];
			int cont = Fread(fd,1,buf);

			if (cont)
				Fseek(-1l,fd,1);
			if ((got = crypt_de(fragstart, got, cont)) < 0) {
				ed_error(IDS_MSGWRONGPW);
				return 0;
			}
		}

		ofs  = fragsize;
		pend = &fragstart[got];

		if ((q = (*f)(par,&fragstart[-len],pend)) == 0)
			return 0;
		if ((len = pend-q) != 0) {
			if (len < 128) {
				fragstart = _linebuf+128;
				fragsize	 = 2*FBUFSIZE;
			} else {
				fragstart = _linebuf+FBUFSIZE;
				fragsize	 = FBUFSIZE;
			}
			memmove(&fragstart[-len],q,len);
		}
		if (ofs != got)
			break;
	}
	_scratchstart 	= fragstart;
	_scratchlen	= len;

	if (got < 0) {
		ed_error(IDS_MSGREADERROR); 
		return 0;
	}
	return 1;
}

/*--------------------------------------*/
/* init_pw()	 					*/
/*--------------------------------------*/
static int init_pw(LINEAL *linp, char *pw, int twice)
{
	if ((linp->workmode & O_CRYPTED) == 0 ||
	    CryptDialog(pw, twice) == 0 ||
	    crypt_init(DES_NOFINAL,pw) == 0) {
		pw[0] = 0;
		return 0;
	}
	MouseBusy();
	return 1;
}

/*--------------------------------------*/
/* readfile()						*/
/*--------------------------------------*/
int _broken,_flushing;
EXPORT int readfile(FTABLE *fp, LINEAL *linp)
{
	int				ret;
	int				nl;
	int				fd;
	register	char *	buf;
	char				pw[32];
	unsigned char *(*f)(FTABLE *par, unsigned char *p, unsigned char *pend);

	fd = -1;
	_broken = 0;
	_flushing = 0;
	fp->currl = fp->firstl;
	buf = _scratchstart;
	if (_talking)
		MouseBusy();

	f = readlines;
	if (linp == 0)
		linp = &_lineal;
	_rlp = linp;

	if ((nl = linp->nl) < 0)
		f = readnoterm;

	if (fp->flags & F_NEWFILE) {
nullfile:
		if (!createl(fp,"",0,(nl < 0) ? LNNOTERM : 0)) {
			goto ret0;
		}
		fp->nlines = 1;
	} else {
		fp->ti_created = EdGetFileTime(fp->fname);
		fd = CreateFile(fp->fname, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); 
		if (fd < 0) {
			if ((fd = EdOpen(fp->fname)) < 0) {
				ret = 0;
				goto readerr;
			}
			linp->workmode |= O_RDONLY;
		}
		_crypting = init_pw(linp, pw, 0);
		if ((ret = readfrags(fd,f,fp)) == 0) {
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
			if ((*f)(fp,&_scratchstart[-_scratchlen],buf) == 0) {
ret0:			ret = 0;
				goto readerr;
			}
			fp->currl->lflg |= LNNOTERM;
		}
	}

	if (!createl(fp,_eof,sizeof _eof -2,LNNOTERM))
		goto ret0;

	if (_broken && _talking) {
		ed_error(IDS_MSGWRAPPEDLINES);
		fp->flags |= F_CHANGEMARK;
	}

	fp->lastl = fp->currl;
	fp->currl = fp->firstl;

readerr:

	if ((_options & O_LOCKFILES) == 0) {
		closeF(&fd);
	}

	fp->lockFd = fd;
	if (_talking) {
		changemouseform();
	}
	return ret;
}

/*--------------------------------------*/
/* creabackup()					*/
/* create a Backup-File				*/
/*--------------------------------------*/
static void creabackup(char *name, char *bak)
{	char backname[256];
	char *ext,*bext = "BAK";

	strcpy(backname,name);
	ext = extname(backname);
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
/* flush()					*/
/*---------------------------------*/
extern long _flushsize;
int flush(int fd, int size, int rest)
{
	return FlushBuffer(fd,_linebuf,size,rest);
}

/*--------------------------------------------------------------------------
 * FlushBufferAndCrypt()
 */
static int FlushBufferAndCrypt(int fd, int size, int rest, int cont, char *pw)
{
	if (*pw) {
		int news;

		news = crypt_en(_linebuf,size,cont);
		if (cont && news != size) {
			alert("bad flushbuffer size");
			return 0;
		}
		size = news;
	}

	return flush(fd,size,rest);
}

/*--------------------------------------*/
/* writefile() 					*/
/*--------------------------------------*/
EXPORT int writefile(int quiet, FTABLE *fp)
{	int			offset;
	int			no;
	int			ret;
	LINE *		lp;
	char			pw[32];
	int 			nl;
	int			cr;
	int			fd = -1;
	int			saveLockFd;
	LINEAL		*linp = fp->lin;

#ifdef DEMO
	return 0;
#else
	if (!linp) {
		linp = &_lineal;
	}

	if ((saveLockFd = fp->lockFd) > 0) {
		closeF(&fp->lockFd);
	}
	lp = fp->firstl;
	if (! (fp->flags & (F_MODIFIED | F_NEWFILE | F_WFORCED | F_APPEND))) 
		return 1;
	nl = linp->nl;
	cr = linp->cr;
	init_pw(linp,pw,1);

	if (!quiet) {
		ShowMessage((fp->flags & F_NEWFILE) ? IDS_MSGNEWFILE : IDS_MSGSAVEFILE,
				  getfn(fp),fp->nlines);
		MouseBusy();
	}
	if (linp->bak[0] &&
	    !(fp-> flags & (F_NEWFILE | F_SAVEAS | F_APPEND | F_ISBACKUPPED))) {
		creabackup(fp->fname, linp->bak);
		fp->flags |= F_ISBACKUPPED;
	}

	if ( (fp->flags & F_APPEND) == 0 || 
	     ((fd = Fopen(fp->fname,2)) < 0)) {
		int crflags = 0;

		if (fp->flags & F_HIDDEN)
			crflags |= F_HIDDEN;
		fd = Fcreate(fp->fname, crflags);
		if (fd < 0) {
			EdMakeReadable(fp->fname);
			fd = Fcreate(fp->fname, crflags);
		}
		if (fd < 0) {
			tosfnerror(fp->fname,fd);
			ret = 100;
			goto wfail1;
		}
		_flushsize = 0;
	} else {
		if ((_flushsize = Fseek(0l,fd,2)) < 0L) {
			ret = 100;
			form_error(-_flushsize);
			goto wfail1;
		}
	}
	fp->flags &= ~(F_APPEND|F_NEWFILE);
	offset = 0;
	while (lp != fp->lastl) {		/* don't save last line			  */
		if ((no = offset+lp->len) < LINEBUFSIZE) {
			memmove(&_linebuf[offset],lp->lbuf,lp->len);
			offset = no;
			if ((lp->lflg & LNNOTERM) == 0) {
				if (cr >= 0) {
					if ((lp->lflg & LNNOCR) == 0)
						_linebuf[offset++] = cr;
				}
				if (nl >= 0)
					_linebuf[offset++] = nl;
			}
			lp = lp->next;
		} else {
			no = (offset > 2*FBUFSIZE) ? 2*FBUFSIZE : offset & (~0xF);
			offset -= no;
			if (!FlushBufferAndCrypt(fd,no,offset,1,pw))
				goto wfail;
		}
	}
	if (!FlushBufferAndCrypt(fd,offset,0,0,pw)) {
wfail:	fp->flags |= F_CHANGEMARK;
		ret = 100;
		goto wfail1;
	}
	ret = 0;
	fp->flags &= ~(F_CHANGEMARK | F_WFORCED);
	ft_settime(&fp->ti_saved);

wfail1:
	closeF(&fd);
	if (saveLockFd > 0) {
		fp->lockFd = EdOpen(fp->fname);
	}
	if (!quiet)
		changemouseform();
	if (ret) {
		if (_flushsize == -1) {	/* delete Files, if Disk is full */
			Fdelete(fp->fname);
		}
		return 0;
	}
	fp->ti_created = EdGetFileTime(fp->fname);
	return 1;
#endif
}

/*---------------------------------*/
/* write2ndpathfile()			*/
/*---------------------------------*/
EXPORT int write2ndpathfile(FTABLE *fp)
{	char dirname[512],savefn[512],fname[512];
	int  ret,flags;
	P2LIST *pl;

	strcpy(savefn,fp->fname);
	sfsplit(savefn,dirname,fname);
	flags = fp->flags;
	pl = _p2list;

	if ((ret = writefile(0,fp)) != 0) {
		while(pl) {
			if (strcmp(pl->p1,dirname) == 0) {
				strdcpy(fp->fname,pl->p2,fname);
				fp->flags = flags;
				if ((ret = writefile(0,fp)) == 0)
					break;
			}
			pl = pl->next;
		}
	}

	strcpy(fp->fname,savefn);
	return ret;
}

/*---------------------------------*/
/* Writefile()					*/
/*---------------------------------*/
EXPORT int Writefile(FTABLE *fp,char *fn,int flags)
{	int ret;

	fp->lockFd = -1;
	fp->flags	= (flags | F_MODIFIED);
	if (!(fp->flags & F_APPEND)) {
		fp->flags  |= F_SAVEAS;
	}
	strcpy(fp->fname,fn);
	ret = writefile(1,fp);
	return ret;
}

/*---------------------------------*/
/* read a file, which is never	*/
/* been redrawn on screen		*/
/* Macrofiles, ...				*/
/*---------------------------------*/
EXPORT int Readfile(FTABLE *fp,char *fn,int linflag)
{	int ret = 0;

	/* linflag == 0 => take Lineal as usually */
	/* linflag <  0 => Lineal is DEFAULT with '\n' as Terminator */

	fp->firstl = 0;
	fp->nlines = 0L;
	fp->lin    = 0;
	fp->flags  = F_NORMOPEN;
	fp->lockFd = 0;
	strcpy(fp->fname,fn);

	if (linflag >= 0 && !AssignDocumentTypeDescriptor(fp,(LINEAL*)0))
		return 0;

	_talking = 0;
	ret = readfile(fp, fp->lin);
	_talking = 1;
	closeF(&fp->lockFd);

	return ret;
}

/*---------------------------------*/
/* Writeandclose()				*/
/*---------------------------------*/
EXPORT int Writeandclose(FTABLE *fp,char *name, int flags)
{	
	int 		ret;

	fp->lastl = 0;
	fp->lin = 0;
	ret = Writefile(fp,name,flags);
	lnlistfree(fp->firstl);
	fp->firstl = 0;
	fp->currl  = 0;
	return ret;
}

