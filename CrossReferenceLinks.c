/*
 * CrossReferenceLinks.c
 *
 * maintain tags and do "under cursor actions"
 *
 *											TOM 28. Dec. 1990
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include <string.h>
#include "winterf.h"
#include "edctype.h"
#include "edifsel.h"
#include "lineoperations.h"
#include "winfo.h"
#include "pksedit.h"
#include "edierror.h"
#include "iccall.h"
#include "xdialog.h"
#include "dial2.h"
#include "regexp.h"

extern char	_finds[];
extern char 	*_datadir;
extern int 	_regcompile(char *ebuf, char *pat, int opt);
extern long	Atol();
extern char 	*searchfile(char *s);
extern unsigned char *stralloc(unsigned char *buf);
extern LINE 	*ln_goto(FTABLE *fp,long l);
extern long 	ln_cnt(LINE *lps,LINE *lpe);
extern int	isword(char c), isnospace(char c), isfname(char c);
extern char	*lstrchr(char *,char);
extern FTABLE 	*ww_stackwi(int num);
extern void	*prof_llinsert(void *Head, int size, 
						char *group, char *item, char **idata);
extern void 	ShowError(LPSTR fmt, void *ap);
extern void *	ll_find(void *Head, char *name);
extern int 	ll_top(void *Head, void *Elem);
extern void 	strdcpy(char *dest,char *source,char *fname);

#define	TCMD_EXEC				'!'
#define	TCMD_TAGFILE			'^'
#define	TCMD_WINHELP			'&'

typedef struct tag {
	long ln;
	int  cmd;
	char fn[256];
	char rembuf[100];
} TAG;

/*
 * description of the "try" list for tag retrieval
 */

#define	TAGMAXTRY			5

typedef struct tagtrylist {
	int type;
	char	*fn;
} TAGTRYLIST;

typedef struct tagtry {
	struct tagtry *next;
	char			name[32];
	int			max;
	int  		curr;
	TAGTRYLIST 	t[TAGMAXTRY];
} TAGTRY;

typedef struct tagexpression {
	struct tagexpression *next;
	char compiler[32];	/* Name of Compiler				*/
	char *source;		/* expression in words			*/
	char *fnbsl;		/* which \? is filename			*/
	char *lnbsl;		/* which \? is linenumber		*/
	char *rembsl;		/* which bsl is remark			*/
} TAGEXPR;

static TAGTRY	*_ttry;

static FTABLE	_tagfile,_stepfile;
extern FTABLE 	*_errfile;

static TAGEXPR __ge = { 
0, "","\"([^\"]+)\", line ([0-9]+): *(.*)",	"\\1",	"\\2",	"\\3" };
static TAGEXPR __te = { 
0, "", "^([^ ]+) ([^ ]+) ([!?^&].*)",   "\\1",	"\\2",	"\\3" };
static TAGEXPR *_exprerror = &__ge;
static TAGEXPR *_greperror = &__ge;
static TAGEXPR *_tagerror  = &__te;
static TAGEXPR	*_cmptags;
static TAGEXPR *_compiler;

static int	_compflag;

/*---------------------------------*/
/* tags_mk()					*/
/*---------------------------------*/
static char *szTags = "tags";
static int tags_mk(char *tag)
{
	char		*s;
	TAGTRY	*tp;

	if ((tp = prof_llinsert(&_ttry,sizeof *tp,szTags,tag,&s)) == 0)
		return 0;

	s = strtok(s,",");
	for (tp->max = 0; tp->max < TAGMAXTRY && s; tp->max++) {
		tp->t[tp->max].type = *s++;

		/* is already malloced , dont free old ! */
		tp->t[tp->max].fn = s;
		s = strtok((char *)0,",");
	}
	tp->curr = -1;
	return 1;
}

/*--------------------------------------------------------------------------
 * tags_init()
 * init all compiler types
 */
int tags_init(void)
{
	return prof_enum(szTags,tags_mk,0L);
}

/*--------------------------------------------------------------------------
 * tagselect()
 * activate a new source list for tag searches (e.g. when
 * changing document type of top level window)
 */
void tagselect(char *tags)
{
	void *p;

	if (!_ttry) {
		return;
	}

	if ((p = ll_find(_ttry, tags)) != 0 && p != _ttry) {
		/* force tag file reread */
		((TAGTRY *)p)->curr = -1;
		ll_top(&_ttry, p);
	}
}

/*------------------------------------------------------------------------
 * get_quoted()
 */
static char *get_quoted(char **src)
{
	char *d,*s,*start;

	s = d = start = *src;
	while (*s) {
		if (*s == ',') {
			s++;
			break;
		}
		if (*s == '\\' && s[1])
			s++;
		*d++ = *s++;
	}
	*d = 0;
	*src = s;
	return start;
}

/*--------------------------------------------------------------------------
 * compiler_mk()
 */
static char *szCompiler = "compiler";
static int compiler_mk(char *compiler)
{
	TAGEXPR 	*ct;
	char		*s;

	if ((ct = prof_llinsert(&_cmptags,sizeof *ct,szCompiler,compiler,&s)) == 0) {
		return 0;
	}

	ct->source   = get_quoted(&s);
	ct->fnbsl    = get_quoted(&s);
	ct->lnbsl    = get_quoted(&s);
	ct->rembsl   = get_quoted(&s);
	return 1;
}

/*--------------------------------------------------------------------------
 * compiler_init()
 * init all compiler types
 */
int compiler_init(void)
{
	return prof_enum(szCompiler,compiler_mk,0L);
}

/*---------------------------------*/
/* tagexprinit()				*/
/*---------------------------------*/
static int tagexprinit(char *ebuf,TAGEXPR *s)
{
	_exprerror = s;
	return _regcompile(ebuf,_exprerror->source,RE_DOREX);
}

/*---------------------------------*/
/* dostep()					*/
/*---------------------------------*/
static TAG *dostep(LINE *lp,char *ebuf)
{
	static TAG _tag;

	if (step(lp->lbuf, ebuf, &lp->lbuf[lp->len])) {
		repinit(_exprerror->fnbsl);	/* assure a few initialiations */
		exprsub(_exprerror->fnbsl, _tag.fn, sizeof _tag.fn);
		exprsub(_exprerror->lnbsl, _linebuf, LINEBUFSIZE);
		_tag.ln = Atol(_linebuf);
		exprsub(_exprerror->rembsl, _tag.rembuf, sizeof _tag.rembuf);
		return &_tag;
	}
	return 0;
}

/*---------------------------------*/
/* istagfile()					*/
/*---------------------------------*/
static int istagfile(FTABLE *fp, char *lookfn)
{	char   *fn;

	if ((fn = searchfile(lookfn)) == 0L) {
		tosfnerror(lookfn,-33);
		return 0;
	}

	return readtagf(fn,fp);
}

/*---------------------------------*/
/* istagcmd()					*/
/*---------------------------------*/
static int istagcmd(unsigned char c)
{
	if (c == TCMD_TAGFILE ||	/* normal tag command */
	    c == TCMD_EXEC ||	/* execute ... */
	    c == '?' || 		/* unused */
	    c == TCMD_WINHELP	/* call windows help */
	   )	return 1;
	return 0;
}

/*---------------------------------*/
/* xscan()					*/
/* handle tag optimizations:		*/
/* - * tag ^search				*/
/* - ? tag ^					*/
/*---------------------------------*/
static LINE *xscan(LINE *lp, char wild, char *d, int max, int *cmd)
{	char *s,*dend,lim;

	while(lp->lbuf[0] == '?' || lp->lbuf[0] == wild) {
		if ((lp = lp->prev) == 0) {
			ed_error(IDS_MSGWRONGTAGFORMAT);
			return 0;
		}
	}
	s    = lp->lbuf;

	if (wild == '?') {
		while(*s && !istagcmd(*s))
			s++;
		lim  = 0;
		*cmd = *s;
	} else {
		lim = ' ';
	}

	dend = &d[max-1];
	while(*s && *s != lim && d < dend) {
		*d++ = *s++;
	}
	*d = 0;
	return lp;
}

/*--------------------------------------------------------------------------
 * taggetinfo()
 */
static TAG *taggetinfo(LINE *lp)
{
	TAG	*	tp;
	char     	ebuf[ESIZE];

	tagexprinit(ebuf, _tagerror);
	if ((tp = dostep(lp, ebuf)) == 0L) {
		/* bad format in tag file */
		return 0;
	}
	if ((lp = xscan(lp, '?', tp->rembuf,
		sizeof(tp->rembuf), &tp->cmd)) == 0) {
		return 0;
	}
	if (xscan(lp, '*', tp->fn, sizeof(tp->fn), &tp->cmd) == 0) {
		return 0;
	}
	return tp;
}

/*------------------------------------------------------------
 * taglist_measureitem()
 */
#define		TAGLISTITEMHEIGHT			40
void taglist_measureitem(MEASUREITEMSTRUCT *mp)
{
	mp->itemHeight = TAGLISTITEMHEIGHT;
	mp->itemWidth = 600;
}

/*------------------------------------------------------------
 * taglist_cmpitem()
 */
int taglist_cmpitem(COMPAREITEMSTRUCT *cp)
{
	int 	nRet;
	int	nSize;

	nSize = min(((DLGSTRINGLIST *)cp->itemData1)->nSize,
		((DLGSTRINGLIST *)cp->itemData2)->nSize);

	nRet = strncmp(
		((DLGSTRINGLIST *)cp->itemData1)->pszString,
		((DLGSTRINGLIST *)cp->itemData2)->pszString,
		nSize);
	
	if (nRet != 0) {
		return nRet;
	}

	return ((DLGSTRINGLIST *)cp->itemData1)->nSize -
		((DLGSTRINGLIST *)cp->itemData2)->nSize;
}

/*------------------------------------------------------------
 * taglist_drawitem()
 */
static void taglist_drawitem(HDC hdc, RECT *rcp, DWORD par, int nItem, int nCtl)
{
	LONG			lExtent;
	WORD			wDelta;
	WORD			hDelta;
	DLGSTRINGLIST *dlp;
	LINE *		lp;
	TAG *		tp;
	HICON		hIcon;

	if ((dlp = (DLGSTRINGLIST*)par) == 0) {
		return;
	}

	lp = (LINE *) dlp->pAny;

	lExtent = GetTextExtent(hdc, dlp->pszString, dlp->nSize);
	wDelta = LOWORD(lExtent) + 20;
	hDelta = HIWORD(lExtent);
	hDelta = (TAGLISTITEMHEIGHT - hDelta) / 2;

	TextOut(hdc, rcp->left + 50, rcp->top + hDelta, dlp->pszString, dlp->nSize);

	if (wDelta < 150) {
		wDelta = 150;
	}
	if ((tp = taggetinfo(lp)) != 0) {

		switch(tp->cmd) {

		case TCMD_EXEC:	
			hIcon = LoadIcon(hInst, "Exec");
			break;
		case TCMD_TAGFILE:	
			hIcon = LoadIcon (hInst, "File");
			break;
		default:			
			hIcon = LoadIcon(NULL, IDI_QUESTION);
			break;
		}
	
		DrawIcon(hdc, 
			rcp->left + (50 - 32) / 2, 
			rcp->top + (rcp->bottom - rcp->top - 32) / 2,
			hIcon);
		TextOut(hdc, rcp->left + 50 + wDelta, rcp->top + hDelta, tp->fn, 
			lstrlen(tp->fn));
		DestroyIcon(hIcon);
	}

}

/*---------------------------------*/
/* findtag()					*/
/*---------------------------------*/
static BOOL _tagCancelled;
static TAG *findtag(char *s, FTABLE *fp)
{
	LINE *			lp;
	LPSTR			pszBlank;
	char     			sbuf[ESIZE];
	char     			ebuf[ESIZE];
	DLGSTRINGLIST *	dlp;
	DLGSTRINGLIST *	dlpCurr;

	dlp = 0;
	wsprintf(sbuf,"^[^ ]+ <%s> [!^?&]", (LPSTR)s);
	if (!_regcompile(ebuf, sbuf, (int)RE_DOREX)) {
		return 0;
	}

	for (lp = fp->firstl; lp; lp = lp->next) {
		if (step(lp->lbuf, ebuf, &lp->lbuf[lp->len])) {
			if ((dlpCurr = ll_insert(&dlp, sizeof *dlp)) == 0) {
				ll_kill(&dlp, (int (*)(void *))0);
				return 0;
			}
			if ((pszBlank = lstrchr(lp->lbuf, ' ')) != 0) {
				dlp->pszString = ++pszBlank;
				if ((pszBlank = lstrchr(pszBlank, ' ')) == 0) {
					pszBlank = &lp->lbuf[lp->len];
				}
				dlp->nSize = (int)(pszBlank - dlp->pszString);
			} else {
				dlp->pszString = lp->lbuf;
				dlp->nSize = lp->len;
			}
			dlpCurr->pAny = lp;
		}
	}

	switch (ll_count(dlp)) {

	case 0: 
		return 0;

	case 1:
		dlpCurr = dlp;
		break;

	default:
	{
		static DIALLIST dlist = {
			0,
			0, 
			0, 
			taglist_measureitem, 
			taglist_drawitem, 
			0,
			taglist_cmpitem,
			0
		};
		dlpCurr = DoDlgSelectFromList(DLGTAGLIST, dlp, &dlist);
		break;
	}

	}

	if (!dlpCurr) {
		ll_kill(&dlp, (int (*)(void *))0);
		_tagCancelled = TRUE;
		return 0;
	}

	lp = dlpCurr->pAny;
	ll_kill(&dlp, (int (*)(void *))0);
	return taggetinfo(lp);
}

/*---------------------------------*/
/* readtagf()					*/
/*---------------------------------*/
static int readtagf(char *fn,FTABLE *fp)
{	int ret;

	lnlistfree(fp->firstl);
	ret = Readfile(fp,fn,-1);
	return ret;
}

/*---------------------------------*/
/* gettag()					*/
/*---------------------------------*/
static char *gettag(unsigned char *d,unsigned char *dend,
			     int (*valid)(),int scan2beg)
{	register char *s,*S;
	char     *s1=d;
	register FTABLE *fp;

	if ((fp = _currfile) == 0L) 
		return (char *)0;
	S = fp->currl->lbuf;
	s = &S[fp->lnoffset];

	if (scan2beg)
		while(s > S && (*valid)(s[-1]))
			s--;

	while ((*valid)(*s) && d < dend)
		*d++ = *s++; 

	*d = 0;
	return(s1);
}

/*---------------------------------*/
/* tagword()					*/
/*---------------------------------*/
static char *_tagword;
char *tagword(unsigned char *d,unsigned char *dend)
{
	if (_tagword) {
		strcpy(d,_tagword);
		return _tagword;
	}

	return gettag(d,dend,isword,1);
}

/*------------------*/
/* tagopen()		*/
/*------------------*/
int tagopen(char *name, long line, WINDOWPLACEMENT *wsp) {	
	int ret = 0;

	if (selnfile(name)) {
		if (line >= 0)
			ret = centernewpos(line,0L);
		else ret = 1;
	} else {
		ret = opennofsel(name,line,wsp);
	}

	return ret;
}

/*---------------------------------*/
/* showtag()					*/
/*---------------------------------*/
int showtag(char *s)
{
	TAG *		tp;
	char     		ebuf[ESIZE];
	char	    		fnam[256];
	int			tnum;
	int			ret = 0;
	TAGTRY *		ttp;
	TAGTRYLIST *	ttl;
	FTABLE *		fp;

	if (!s || !*s)
		return 0;

	if ((ttp = _ttry) == 0)
		return 0;

	fp  = &_tagfile;

	setfinds(s);
	_tagword = s;

	_tagCancelled = FALSE;

	for (tnum = 0; tnum < ttp->max && ret == 0 && _tagCancelled == FALSE; tnum++) {
		ttl = &ttp->t[tnum];
		switch(ttl->type) {
			case TCMD_TAGFILE:
				if (tnum != ttp->curr) {
					if (istagfile(fp,ttl->fn) == 0)
						break;
					ttp->curr = tnum;
				}
				if ((tp = findtag(s,fp)) != 0L) {
					if (tp->cmd == TCMD_WINHELP) {
						ret = HelpKey(tp->fn, tp->rembuf+1);
					} else {
						strcpy(fnam,tp->fn);
						fm_savepos(s);
						tagopen(fnam,-1L,(WINDOWPLACEMENT*)0);
						if (_currfile) {
							if (_regcompile(ebuf, tp->rembuf, (int) RE_DOREX)) {
								_findstr(1,ebuf,O_WRAPSCAN);
								ret = 1;
							}
						}
					}
				}
				break;
			case TCMD_WINHELP:
				ret = HelpKey(ttl->fn, s);
				break;
			}
	}

	_tagword = 0;
	if (!ret && !_tagCancelled) {
		ed_error(IDS_MSGUNKNWONTAG,(LPSTR)s);
	}
	return ret;
}

/*---------------------------------*/
/* picstep()					*/
/*---------------------------------*/
void picstep(LINE *lp)
{ 	register TAG  *tp;
	WINDOWPLACEMENT ws,*wsp = 0;
	char ebuf[ESIZE];

	tagexprinit(ebuf,_greperror);
	while((lp = lp->next) != 0L) {		/* we may skip 1st line ! */
		if (lp->len && (tp = dostep(lp,ebuf)) != 0L) {
			if (tp->rembuf[0]) {
				/* this means -> windowstate given */
				wsp = &ws;
				prof_getws(tp->rembuf,wsp);
			}
			tagopen(tp->fn, tp->ln-1L, wsp);
		}
	}
}

/*---------------------------------*/
/* EdErrorNext()				*/
/*---------------------------------*/
int EdErrorNext(int dir)
{
	register LINE 	*lp;
 	register TAG  	*tp=0;
	FTABLE	    	*fp;
	long			lineno;
	TAGEXPR  		*steperror;
	char 		ebuf[ESIZE];
	char			fullname[256];
	WINFO		*wp;

	if ((dir & LIST_USETOPWINDOW) || !_errfile) {
	/* treat current window as error list */
		_compflag = 1;
		_errfile = ww_stackwi(0);
	}

	if ((fp = _errfile) == 0 || (lp = fp->currl) == 0L) {
notfile:	ed_error(IDS_MSGNOTAGFILE);
		return 0;
	}

	lineno = fp->ln;
	if (_compflag) {
		if (!_compiler) {
			_compiler = _cmptags;
		}
		steperror = _compiler;
	} else {
		steperror = _greperror;
	}
	if (steperror == 0) {
		goto notfile;
	}

	tagexprinit(ebuf,steperror);

	switch (dir) {
	case LIST_PREV:
dobackward:
		while((lineno--, (lp = lp->prev) != 0L) && 
		      (tp = dostep(lp,ebuf)) == 0L);
		break;
	case LIST_NEXT:
doforward:
		while((lineno++, (lp = lp->next) != 0L) &&
		      (tp = dostep(lp,ebuf)) == 0L);
		break;
	case LIST_START:
		lp     = fp->firstl;
		lineno = 0;
		if ((tp = dostep(lp,ebuf)) == 0L) goto doforward;
		break;
	case LIST_END:
		lp     = fp->lastl;
		lineno = fp->nlines;
		goto dobackward;
	default:
		if ((tp = dostep(lp,ebuf)) == 0L) 
			goto doforward;
		break;
	}
	if (tp && lp) {
		if ((wp = WIPOI(fp)) != 0) {
			/* EdSelectWindow(wp->win_id); */
			_curpos(fp,lineno,0);
			ln_um(fp->firstl,fp->lastl,LNXMARKED);
			lp->lflg |= LNXMARKED;
			EdRedrawWindow(wp);
		} else {
			fp->currl = lp;
		}

	/* make file name relativ to list file */
		if (*tp->fn == '/' || tp->fn[1] == ':') {
			lstrcpy(fullname, tp->fn);
		} else {
			sfsplit(fp->fname, fullname, (char *)0);
			strdcpy(fullname, fullname, tp->fn);
		}
		if (tagopen(fullname, tp->ln-1L, (WINDOWPLACEMENT*)0)) {
			if (tp->rembuf[0]) {
				ShowError(tp->rembuf, (void*)0);
			}
			return 1;
		}
		return 0;
	}
	ed_error(_compflag ? IDS_MSGNOMOREERRS : 
					 IDS_MSGSTRINGNOTFOUND);
	return 0;
}

/*--------------------------------------------------------------------------
 * stepselectcompiler()
 */
void stepselectcompiler(char *pszName)
{
	TAGEXPR		*tp;

	for (tp = _cmptags; tp; tp = tp->next) {
		if (lstrcmp(tp->compiler, pszName) == 0) {
			_compiler = tp;
			return;
		}
	}
}

/*---------------------------------*/
/* s_t_open()					*/
/*---------------------------------*/
#define	ST_TAGS		1
#define	ST_ERRORS		2
#define	ST_STEP		3
static FSELINFO _tagfselinfo = { ".", "TAGS", "*.TAG" };
static FSELINFO _grpfselinfo = { ".", "PKSEDIT.GRP", "*.GRP" };
static FSELINFO _cmpfselinfo = { ".", "ERRORS.ERR", "*.ERR" };
static int s_t_open(int title, int st_type, FSELINFO *fsp)
{
	if (title && rw_select(fsp, title) == 0) {
		return 0;
	}

	switch(st_type) {
		case ST_ERRORS:
			if (tagopen(_fseltarget, 0L, (WINDOWPLACEMENT*)0) && _currfile) {
				EdFileAbandon(1);
			}
			return EdErrorNext(LIST_START|LIST_USETOPWINDOW);
		case ST_STEP:
			if (readtagf(_fseltarget,&_stepfile)) {
				_compflag = 0;
				_errfile = &_stepfile;
				return EdErrorNext(LIST_START);
			}
			return 0;
		case ST_TAGS:
		/*
		 * new tag files are added in front of the tag list
		 * cause all icon tags should stay at the end of the search list
		 */
		if (_ttry == 0)
			break;
		if (_ttry->max < TAGMAXTRY) {
			memmove(&_ttry->t[1],_ttry->t,sizeof(*_ttry->t) * _ttry->max);
			_ttry->max++;
		}
		_ttry->t[0].type = TCMD_TAGFILE;
		_ttry->t[0].fn = stralloc(_fseltarget);
		_ttry->curr = 0;
		return readtagf(_fseltarget,&_tagfile);
	}
	return 0;
}

/*---------------------------------*/
/* EdFindTagCursor()			*/
/*---------------------------------*/
int EdFindTagCursor(void)
{
	return showtag(tagword(_linebuf,&_linebuf[LINEBUFSIZE]));
}

/*---------------------------------*/
/* EdFindFileCursor()			*/
/*---------------------------------*/
int EdFindFileCursor(void)
{	char *fn,*found;
	char	fselpath[512];
	extern char _incpath[];
	extern char *pathsearch();

	if ((fn = gettag(_linebuf,&_linebuf[LINEBUFSIZE],isfname,1)) == 0) 
		return 0;

#if 0
	fsel_outit(fselext,fselpath);
#else
	sfsplit(_fseltarget,fselpath,(char*)0);
#endif
	if ((found = pathsearch(fn,_incpath))   != 0 ||
	    (found = pathsearch(fn,fselpath))   != 0) {
		return tagopen(found, 0L, (WINDOWPLACEMENT*)0);
	}

	ed_error(IDS_MSGFILENOTFOUND);
	return 0;
}

/*---------------------------------*/
/* EdFindWordCursor() 			*/
/*---------------------------------*/
int EdFindWordCursor(dir)
{	char *s,buf[256];

	if ((s = gettag(buf,&buf[sizeof buf],isword,0)) != 0L) {
		strcpy(_finds,s);
		return findstr(dir);
	}
	return 0;
}

/*---------------------------------*/
/* stepnofsel()				*/
/*---------------------------------*/
void stepnofsel(char *fn, int cmpflg)
{
	strcpy(_fseltarget,fn);
	s_t_open(0, cmpflg ? ST_ERRORS: ST_STEP, &_cmpfselinfo);
}

/*---------------------------------*/
/* EdSearchListRead() 			*/
/*---------------------------------*/
int EdSearchListRead(void)
{
	return s_t_open(MREADSTE,ST_STEP,&_grpfselinfo);
}

/*---------------------------------*/
/* EdTagfileRead()				*/
/*---------------------------------*/
int EdTagfileRead(void)
{
	return s_t_open(MREADTAG,ST_TAGS,&_tagfselinfo);
}

/*---------------------------------*/
/* EdErrorListRead()			*/
/*---------------------------------*/
int EdErrorListRead(long dummy1, long dummy2, char *pszCompiler)
{

	_compiler = _cmptags;
	if (pszCompiler) {
		stepselectcompiler(pszCompiler);
	}
	return s_t_open(MREADCMP,ST_ERRORS,&_cmpfselinfo);
}

