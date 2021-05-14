/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * OpenFiles.c
 *
 * open files
 *
 * 						created: 07.06.91
 *						Author : TOM
 */

#include "tos.h"
#include <shellapi.h>
#include <time.h>

#include "trace.h"
#include "editab.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"

/* #define DEMO 1 /* D E M O V E R S I O N */


#pragma hdrstop

#include "dos.h"
#include "pathname.h"
#include "edifsel.h"
#include "edhist.h"
#include "pksedit.h"

extern void *	ll_insert(void *head,long size);
extern void *	shareAlloc();
extern char *	AbbrevName(char *fn);
extern char *	OemAbbrevName(char *fn);
extern FTABLE *ww_stackwi(int num);
extern HWND 	ww_winid2hwnd(int winid);
extern DWORD 	SendBrotherMessage(UINT message, LPARAM lParam);
extern int 	lin_match(LINEAL *linp, char *filename);
extern char *	searchfile(char *s);
extern void 	prof_saveaspath(void);
extern int 	PromptAsPath(char *path);
extern LINEAL *linlinealfor(void *p);
extern char *	basename(char *s);
extern int 	createl(FTABLE *fp, char *q, int len, int flags);

extern int  	_asminutes;
extern char 	_aspath[];
extern char *	_datadir;
extern void *	lastSelectedDocType;

char *ft_visiblename(FTABLE *fp);

FTABLE 		*_currfile,*_errfile;
static FTABLE 	*_filelist;

char			*_hisfile = "PKSEDIT.HIS";
int			_ExSave;

/*--------------------------------------------------------------------------
 * make_aspath()
 */
static int make_aspath(char *dname, char *fname)
{	char fn[EDMAXPATHLEN];
	char szBuff[EDMAXPATHLEN];

	if (_aspath[0]) {
		sfsplit(fname,(char *)0,fn);
		strdcpy(szBuff,_aspath,fn);
		FullPathName(dname,szBuff);
		return 1;
	}
	return 0;
}


void ft_CheckForChangedFiles(void) {
	FTABLE *	fp;
	EDTIME		lCurrentTime;
	
	for (fp = _filelist; fp; fp = fp->next) {
		if (fp->ti_created < (lCurrentTime = EdGetFileTime(fp->fname))) {
			if (ed_yn(IDS_MSGFILESHAVECHANGED, OemAbbrevName(fp->fname)) == IDYES) {
				AbandonFile(fp, (LINEAL *)0);			
			}
		}
		fp->ti_created = lCurrentTime;
	}
}

/*---------------------------------*/
/* asfiles()					*/
/* do an autosave				*/
/*---------------------------------*/
#define 	HZ			1000L
int asfiles(void)
{	int	    		ret,flags,saved;
	long     		now,dclicks;
	register FTABLE *fp;
	char     		spath[EDMAXPATHLEN];
	static long 	nchkclick;
	static int 		inas;

	if (inas || (dclicks = (_asminutes * 60L * HZ)) == 0) {
		/* autosave option is OFF */
		return 0;
	}

	now = GetTickCount();
	if (now < nchkclick) {
		return 0;
	}

	inas = 1;

	/* check every 5 secs */
	nchkclick = now + 5 * HZ;

	saved = 0;
	for (fp = _filelist; fp; fp = fp->next) {

		/* file in unsaved state ? */
		flags = fp->flags;
		if ((flags & F_NEEDSAUTOSAVE) == 0)
			continue;

		/* no "autosave needed"-time: set to current + #of minutes */
		if (fp->as_time == 0) {
			fp->as_time = now + dclicks;
			continue;
		}

		/* save maximum 1 file per check cycle */
		if (saved)
			continue;

		if (now < fp->as_time)
			continue;

autosave:
		/* "best before" date expired: do autosave */
		strcpy(spath,fp->fname);

		make_aspath(fp->fname,fp->fname);
		ret = writefile(1, fp);

		/* restore MODIFIED and ISBACKUPED - Flags */
		fp->flags = flags;

		if (ret > 0) {
		/* we autosaved into source file: set state to unmodified */
			if (!_aspath[0])
				fp->flags &= ~F_MODIFIED;
			ShowMessage(IDS_MSGAUBE,ft_visiblename(fp));
		} else if (PromptAsPath(_aspath)) {
		/* let the user correct invalid autosave pathes */
			prof_saveaspath();
			goto autosave;
		}
	
		/* restore file name */
		strcpy(fp->fname,spath);

		/*
		 * maybe autosave failed - reset autosave condition nevertheless
		 * cause autosave probably will fail on next cycle(in 5 sec.)
		 * too
		 */
		fp->flags &= ~F_NEEDSAUTOSAVE;
		fp->as_time = 0;
		saved = ret;

	}
	
	inas = 0;
	return saved;
}

/*--------------------------------------------------------------------------
 * prnfl()
 */
int prnfl(FTABLE *fp, char *fn, long line, char *remark)
{
	char buf[EDMAXPATHLEN];

	wsprintf(buf,"\"%s\", line %ld: %s",(LPSTR)fn,line+1L,(LPSTR)remark);
	return createl(fp, buf, lstrlen(buf), 0);
}

/*---------------------------------*/
/* picksave()					*/
/*---------------------------------*/
void picksave(void )
{	int    				s;
	FTABLE 				ft;
	FTABLE *			fp;
	WINDOWPLACEMENT 	ws;
	char   				szBuff[EDMAXPATHLEN];

	ft.currl = 0;
	putline(&ft,"[files]");

	/* save names of bottom windows first !! */
	for (s = ww_nwi()-1; s >= 0; s--) {
		if ((fp = ww_stackwi(s)) != 0) {
			ww_getstate(WIPOI(fp),&ws);
			prof_printws(szBuff,&ws);
			prnfl(&ft,fp->fname,fp->ln,szBuff);
		}
	}

	hist_allsave(&ft);	

	strdcpy(szBuff, _datadir, _hisfile);
	Writeandclose(&ft, szBuff, FA_NORMAL);
}

/*---------------------------------*/
/* pickread()					*/
/*---------------------------------*/
int pickread(void )
{
	FTABLE 	ft;
	char *	pszFound;

	if (_options & O_READPIC) {
		if ((pszFound = searchfile(_hisfile)) != 0 &&
		    Readfile(&ft, pszFound, -1)) {
			if (_filelist == 0) {
				picstep(ft.firstl);
			}
			hist_read(ft.firstl);
			lnlistfree(ft.firstl);
			return 1;
		}
	}
	return 0;
}

/*------------------------------------------------------------
 * ft_bufdestroy()
 */
void ft_bufdestroy(FTABLE *fp)
{
	ll_kill(&fp->fmark,(int (*)(void *elem))0);
	fp->blstart = 0;
	fp->blend = 0;
	destroy(&fp->lin);
	lnlistfree(fp->firstl);
	fp->tln = fp->firstl = fp->currl = 0;
	closeF(&fp->lockFd);
	u_destroy(fp);
}

/*------------------------------------------------------------
 * ft_settime()
 */
void ft_settime(EDTIME *tp)
{	time (tp);
}

/*
 * Checks whether two file names describe two different files. Returns
 * value != 0, if different
 */
int areFilenamesDifferent(char *fileName1, char *fileName2) {
	char tempFn1[EDMAXPATHLEN];
	char tempFn2[EDMAXPATHLEN];

	GetLongPathName(fileName1, tempFn1, sizeof tempFn1);
	GetLongPathName(fileName2, tempFn2, sizeof tempFn2);
	return lstrcmpi(tempFn1, tempFn2);
}

/*------------------------------------------------------------
 * ft_deleteautosave()
 */
void ft_deleteautosave(FTABLE *fp)
{
	char as_name[EDMAXPATHLEN];

	if ((_options & O_GARBAGE_AS) && make_aspath(as_name,fp->fname)) {
		if (areFilenamesDifferent(fp->fname,as_name)) {
			unlink(as_name);
		}
	}
}

/*------------------------------------------------------------
 * ft_destroy()
 */
void ft_destroy(FTABLE *fp)
{
	EdTRACE(Debug(DEBUG_TRACE,"ft_destroy File 0x%lx",fp));

	ft_deleteautosave(fp);
	ft_bufdestroy(fp);

	ll_delete(&_filelist,fp);

	if (!_filelist || P_EQ(fp,_currfile))
		_currfile = 0;
	if (P_EQ(fp,_errfile))
		_errfile = 0;
}

/*------------------------------------------------------------
 * ft_new()
 */
FTABLE *ft_new(void)
{	FTABLE *fp;

	if ((fp = ll_insert(&_filelist,sizeof *fp)) == 0)
		return 0;

	if (u_new(fp) == 0) {
		ll_delete(&_filelist,fp);
		return 0;
	}
	EdTRACE(Debug(DEBUG_TRACE,"ft_new File 0x%lx",fp));

	return fp;
}

/*------------------------------------------------------------
 * ft_name()
 */
char *ft_visiblename(FTABLE *fp)
{	static char buf[512];

	if (fp)
		OemToAnsi(fp->fname,buf);
	return buf;
}

/*------------------------------------------------------------
 * ft_size()
 * calculate current file size
 */
long ft_size(FTABLE *fp)
{	extern long ln_needbytes(LINE *lp, int nl, int cr);
	long fsize = ln_needbytes(fp->firstl,fp->lin->nl,fp->lin->cr);

	if (fp->lastl) 
		fsize -= fp->lastl->len;
	return fsize;
}

/*------------------------------------------------------------
 * ft_fpbyname()
 */
FTABLE *ft_fpbyname(char *fn)
{	FTABLE *fp;

	for (fp = _filelist; fp; fp = fp->next) {
		if (!areFilenamesDifferent(fp->fname,fn)) {
		    return fp;
		}
	}
	return 0;
}

/*------------------------------------------------------------
 * ft_editing()
 */
int ft_editing(char *fn)
{
	return ft_fpbyname(fn) ? 1 : 0;
}

/*------------------------------------------------------------
 * ft_globalediting()
 */
#if !defined(WIN32)
static DWORD ft_globalediting(char *fn)
{
	return SendBrotherMessage(WMBRD_FINDFILE,(LONG)fn);
}
#endif
/*------------------------------------------------------------
 * ft_openwin()
 */
static int ft_openwin(FTABLE *fp, WINDOWPLACEMENT *wsp)
{
	if (wsp == 0 && fp->lin) {
		if (fp->lin->dispmode & SHOWFIXEDWI) {
			wsp = (WINDOWPLACEMENT*)&fp->lin->placement;
		}
	}
	if (EdMdiCreate(szEditClass, fp->fname, ww_nwi()+1, (long)fp, wsp) == 0) {
		return 0;
	}
	return 1;
}

/*------------------------------------------------------------
 * ft_wantclose()
 */
int ft_wantclose(FTABLE *fp)
{
	if (fp->lin->cm[0]) {
		if (!do_macbyname(fp->lin->cm)) {
			return 0;
		}
	}
	if (fp->flags & F_MODIFIED) {
		ShowWindow(hwndClient,SW_SHOW);
		EdSelectWindow(WIPOI(fp)->win_id);
		if (_ExSave || (_options & AUTOWRITE)) {
	     	return write2ndpathfile(fp);
		}
		switch(ed_ync(IDS_MSGQUITORSAVE,ft_visiblename(fp))) {
			case IDYES:
				if (!write2ndpathfile(fp)) 
					return 0;
				break;
			case IDNO:	 break;
			case IDCANCEL:  return (0);
		}
	}
	hist_enq(&_openhist,fp->fname);
	hist_updatemenu(&_openhist);
	return 1;
}

/*------------------------------------------------------------
 * EdSelectWindow()
 */
int SelectWindow(int winid, BOOL bPopup)
{
	HWND 	hwndChild;
	FTABLE *	fp;

	if (winid < 0) {
		if (winid == SEL_CYCLE) {
			winid = -(ww_nwi() - 1);
		}
		if ((fp = ww_stackwi(-winid)) == 0) {
			return 0;
		}
		winid = WIPOI(fp)->win_id;
	}

	if ((hwndChild = ww_winid2hwnd(winid)) == 0) {
		ed_error(IDS_MSGWRONGWINDOW);
		return 0;
	}

	if (bPopup) {
		ww_popup(hwndChild);
	} else {
		SendMessage(hwndClient,WM_MDIACTIVATE,hwndChild,0L);
	}
	return 1;
}

/*------------------------------------------------------------
 * EdSelectWindow()
 */
int EdSelectWindow(int winid)
{
	return SelectWindow(winid, TRUE);
}

/*------------------------------------------------------------
 * ft_select()
 */
int ft_select(FTABLE *fp)
{
	EdTRACE(Debug(DEBUG_TRACE,"ft_select File 0x%lx",fp));
	_currfile = fp;
	if (fp == 0) {
		return 0;
	}
	fixsets(fp->lin->u2lset);
	return 1;
}

/*------------------------------------------------------------
 * txtfile_select()
 */
static FSELINFO _txtfninfo = {"."};
int txtfile_select(int title, char *result)
{
	char *fn;
	BOOL	bRet;

	bRet = FALSE;
	if ((fn = rw_select(&_txtfninfo, title)) != 0) {
		strcpy(result,fn);
		bRet = TRUE;
	}
	return (int) bRet;
}


/*------------------------------------------------------------
 * selnfile()
 */
int selnfile(char *fn)
{
	FTABLE 	*fp;
	char 	fullname[256];

	FullPathName(fullname,fn);
	if ((fp = ft_fpbyname(fullname)) != 0 && WIPOI(fp) != NULL) {
		return EdSelectWindow(WIPOI(fp)->win_id);
	}
	if (fp != NULL) {
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------
 * opennofsel()
 */
int opennofsel(char *fn, long line, WINDOWPLACEMENT *wsp)
{   
	char 		szResultFn[EDMAXPATHLEN];
	char *		shareString;
	char		szAsPath[EDMAXPATHLEN];
	FTABLE 		*fp;
	int 		newfile,ret;

	szAsPath[0] = 0;
	lastSelectedDocType = 0;
	if (fn) {
		FullPathName(szResultFn,fn);
	} else {
		if (!txtfile_select(MOPENF,szResultFn)) {
			return 0;
		}
	}
	fn = szResultFn;
	// EdToUpper(fn);
#if !defined(WIN32)
	if ((shareString = shareAlloc()) == 0)
		return 0;
	lstrcpy(shareString,fn);
	if (ft_globalediting(shareString) != 0) {
		ret = ed_ync(IDS_MSGINWORK,OemAbbrevName(fn));
		if (ret == IDCANCEL)
			return 0;
		if (ret == IDNO)
			return PostBrotherMessage(WMBRD_SELECTFILE,(LONG)shareString);
	}
#else
	if (ft_editing(fn) != 0) {
		ret = ed_ync(IDS_MSGINWORK,OemAbbrevName(fn));
		if (ret == IDCANCEL) {
			return 0;
		}
		if (ret == IDNO) {
			return selnfile(fn);
		}
	}
#endif
	if (EdStat(fn,0xFF) < 0) {
		if (ed_yn(IDS_MSGQUERYNEWFILE,OemAbbrevName(fn)) == IDNO)
			return 0;
		newfile = 1;
	} else {
		if ((_options & O_GARBAGE_AS) && 
			make_aspath(szAsPath, fn) &&
			areFilenamesDifferent(szAsPath, fn) &&
			EdStat(szAsPath, 0xFF) == 0 &&
			ed_yn(IDS_MSGRECOVER) == IDYES) {
			;
		} else {
			szAsPath[0] = 0;
		}
		newfile = 0;
	}

	if ((fp = ft_new()) == 0) {
		return 0;
	}
	if (szAsPath[0]) {
		lstrcpy(fp->fname, szAsPath);
	} else {
		lstrcpy(fp->fname, fn);
	}
	if (newfile) {
		fp->flags |= F_NEWFILE;
	}
	if (linassign(fp, linlinealfor(lastSelectedDocType)) == 0 ||
         readfile(fp, fp->lin) == 0 || 
	    (lstrcpy(fp->fname, fn), ft_openwin(fp, wsp) == 0)) {
		ft_destroy(fp);
		return 0;
	}

	if (szAsPath[0]) {
		fp->flags |= F_MODIFIED;
	}

	curpos(line,0L);

	if (newfile && fp->lin) {
		do_macbyname(fp->lin->vl);
	}

	return 1;
}

/*------------------------------------------------------------
 * EdEditFile()
 */
int EdEditFile(long editflags, long unused, char *filename)
{
	int		ret;
	int		nEntry;

	if ((editflags & OPEN_DIRGIVEN) && filename) {
		sfsplit(filename,_txtfninfo.path,_txtfninfo.search);
		filename = 0;
	}
	if (editflags & OPEN_HISTORY) {
		nEntry = (editflags & 0xF000) >> 12;
		if ((filename = hist_getstring(&_openhist, nEntry)) == 0) {
			return 0;
		}
	}
	fsel_optionsenable(1);
	ret = opennofsel(filename, 0L, (WINDOWPLACEMENT*)0);
	return ret;
}

/*------------------------------------------------------------
 * AbandonFile()
 */
int AbandonFile(FTABLE *fp, LINEAL *linp)
{
	long   	ln,col;

	if  (fp == 0) {
		return 0;
	}

	if (fp->flags & F_MODIFIED) {
		if (ed_yn(IDS_MSGABANDON) == IDNO)
			return 1;
	}

	ln = fp->ln;
	col = fp->lnoffset;
	fp->ln = 0;
	fp->nlines = 0;
	ft_bufdestroy(fp);

	if (u_new(fp) == 0 || 
	    !linassign(fp, linp) ||
	    !readfile(fp,fp->lin)) {
		fp->flags = 0;
		ww_close(WIPOI(fp));
		return 0;
	}

	fp->flags &= ~F_CHANGEMARK;

	/*
	 * uses currfile----
	 */
	if (ln >= fp->nlines) {
		ln = fp->nlines-1;
	}

	curpos(ln,col);

	linchange();
	RedrawTotalWindow(fp);

	return 1;
}

/*--------------------------------------------------------------------------
 * EdFileAbandon()
 */
void EdFileAbandon(void)
{
	AbandonFile(_currfile, (LINEAL *)0);
}

/*------------------------------------------------------------
 * EdSaveFile()
 */
int EdSaveFile(int flg)
{
	FTABLE *fp;

#ifdef	DEMO
	if (flg & ~SAV_QUIT) {
		alert("Demoversion\nSichern nicht möglich");
		return 0;
	}
#else
	if ((fp = _currfile) == 0) 
		return(0);	 		/* currently no File open */

	if (flg != SAV_QUIT && readonly(fp))
		return 0;

	if (flg & SAV_AS) {
		char newname[512];

		sfsplit(fp->fname,_txtfninfo.path,_txtfninfo.fname);
		fsel_optionsenable(1);
		if (txtfile_select(MSAVEAS, newname) == 0) {
			return 0;
		}
		if (areFilenamesDifferent(newname,fp->fname) && EdStat(newname,0xFF) >= 0) {
			if (ed_yn(IDS_MSGOVERWRITE,OemAbbrevName(newname)) == IDNO)
				return 0;
			/* if ret == "APPEND".... fp->flags |= F_APPEND */
		}
		strcpy(fp->fname,newname);
		ww_setwindowtitle(fp,WIPOI(fp));
		fp->flags |= F_CHANGEMARK;
		if (!(fp->flags & F_APPEND)) fp->flags |= F_SAVEAS;
		fp->flags &= ~F_NEWFILE;
		if (!write2ndpathfile(fp)) 	
			return 0;
	}

	if (flg & SAV_FORCED)
		fp->flags |= F_WFORCED;  /* force to write even if 
							   not edited */

	if ((flg & SAV_SAVE) && !write2ndpathfile(fp)) return 0;

	if (flg & SAV_QUIT) {
		ww_close(WIPOI(fp));
	}
#endif

	return(1);
}

/*--------------------------------------------------------------------------
 * EditDroppedFiles()
 */
void EditDroppedFiles(HDROP hDrop)
{
	LPSTR	pszFileName;
	int		i;
	int		nTotal;
	int		nFileLength;

	nTotal = DragQueryFile( hDrop , 0xFFFFFFFF, NULL, 0 );

	for ( i = 0; i < nTotal; i++ )
	{
		nFileLength  = DragQueryFile( hDrop , i , NULL, 0 );
		pszFileName = _alloc(nFileLength + 1);
		DragQueryFile( hDrop , i, pszFileName, nFileLength + 1 );
		if (!EdEditFile(OPEN_NOFN, (LONG)0, pszFileName)) {
			break;
		}
		_free(pszFileName);
	}
}

