/*
 * DocumentTypes.c
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS
 *
 * purpose: maintain "Major Mode Configuration Files", the in PKSEDIT
 *		  called RULER Files,
 *		  link configuration files to file name extensions
 *		  tapstop calculating functions
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauß
 */

#include "tos.h"
#include	"lineoperations.h"
#include	"project.h"
#include	"edifsel.h"
#include "edierror.h"
#include	"pksedit.h"

typedef struct tagLINLIST {
	struct tagLINLIST *	ll_next;
	char				ll_name[32];
	char				ll_description[50];
	int					ll_ctx;
	int					ll_ownlineal;
	char				ll_linname[256];
	char   				ll_match[256];
	LINEAL *			ll_lineal;
} LINLIST;

LINEAL _lineal = {
	0,		80,		8,		4,		/* lmargin, rmargin, ts, sw */
	10,		-1,		13,				/* nl1..cr */
	0,		WM_INSERT, 1,				/* dispmode, workingmode, fno */
	"default.lin",
	0,		' ',		0,				/* ctxid, tab - char, fill - char */
	"a-z0-9_=A-Z0-9_",
	{ 0 },							/* Bitset Tabstop */
	"0x%6p$O: 0x%2p$C 0%h$C",			/* status - line */
};

extern void *	ll_insert(void *head,long size);
extern void *	ll_find(void *Head, char *name);
extern LPSTR 	strmaxcpy(LPSTR pszDest, LPSTR pszSource, int nMax);
extern char 	*extname(char *s);
extern char 	*basename(char *s);
extern char 	*searchfile(char *fn);
extern int 		ll_count(void *Head);
extern long 	Atol(char *s);
extern void 	*prof_llinsert(void *Head, int size, char *group, 
						char *item, char **idata);
extern void 	prof_killsections(LPSTR pszFn, LPSTR pszSection);
extern char *	TmpName(char *dst, char c);
extern void 	strdcpy(char *dest,char *source,char *fname);
extern int 	Mapread(int context, char *target);

extern char *	_datadir;

LINLIST *linnew(LINLIST *llp);

static LINLIST *_linl;
static int     _linealid = LIN_DEFCTX;
static int	_ndoctypes;
FSELINFO 		_linfsel = {	"", "DEFAULT.LIN", "*.LIN" };

#define	LINSPACE			((long )&(((LINEAL *)0)->ts))

/*--------------------------------------------------------------------------
 * TabStop()
 * calculate next Tabstop
 */
int TabStop(int col, LINEAL *l)
{
	if (col < DIM(l->ts) && l->ts[col]) 
		return l->ts[col];

	while (++col < 4096 && !TABTHERE(l,col))	
		;
	return col;
}

/*--------------------------------------------------------------------------
 * initfastts()
 */
static void initfastts(LINEAL *lp)
{	int i,ts;

	ts = 0;
	i  = 0;
	for (;;) {
		while (!TABTHERE(lp,ts) && ts < 4096) 
			ts++;
		if (ts >= DIM(lp->ts))
			return;
		while(i < ts) {
			if (i >= DIM(lp->ts))
				return;
			lp->ts[i] = ts;
			i++;
		}
		ts++;
	}
}

/*--------------------------------------------------------------------------
 * lininit()
 */
static int _linited;
void lininit(LINEAL *lp, int ts)
{	int i,ind;

	_linited = 1;
	lp->tabsize = ts;
	blfill(lp->tbits,sizeof(lp->tbits),0);

	for (i = 0, ind = ts; i < 4096; i++) {
		if (i == ind) {
			TABPLACE(lp,i);
			ind += ts;
		}
	}
	initfastts(lp);
}

/*--------------------------------------------------------------------------
 * tabtoggle()
 */
void tabtoggle(LINEAL *linp, int col)
{
	if (TABTHERE(linp,col))
		TABCLEAR(linp,col);
	else
		TABPLACE(linp,col);

	initfastts(linp);

}

/*--------------------------------------------------------------------------
 * related_file_name()
 */
void related_file_name(char *related_name, char *fname, char *newext)
{	char *ext;

	strcpy(related_name,fname);
	ext = extname(related_name);
	if (ext[-1] != '.')
		*ext++ = '.';
	strcpy(ext,newext);
}

/*--------------------------------------------------------------------------
 * linealdocstrings()
 */
void linealdocstrings(LPSTR pszDest, int nMax)
{
	LPSTR		pszEnd;
	LINLIST *	llp;
	int			nCopied;

	pszEnd = pszDest + nMax - 2;
	for (nCopied = 0, llp = _linl; llp != 0; llp = llp->ll_next) {
		if (nCopied > 0) {
			*pszDest++ = '|';
		}
		if (llp->ll_match[0]) {
			pszDest = strmaxcpy(pszDest, 
					llp->ll_description[0] ? 
					llp->ll_description : llp->ll_match, 
					(int)(pszEnd - pszDest));
			*pszDest++ = '|';
			pszDest = strmaxcpy(pszDest, llp->ll_match, 
					(int)(pszEnd - pszDest));
			nCopied++;
		}
	}
}

/*--------------------------------------------------------------------------
 * linload()
 */
static LINEAL *linload(LINLIST *llp)
{
	LINEAL *lp;

	if ((lp = llp->ll_lineal) == 0) {
		if ((lp = _alloc(sizeof *lp)) == 0) {
			return 0;
		}
		llp->ll_lineal = lp;
		if (linealread(llp->ll_linname,lp,llp->ll_ctx,0) == 0) {
			memmove(lp, &_lineal, sizeof _lineal);
		}
		lstrcpy(llp->ll_lineal->modename, llp->ll_name);
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * linnrulers()
 */
int linnrulers(void)
{
	return ll_count(_linl);
}

/*--------------------------------------------------------------------------
 * linfilllbox()
 */
int linfilllbox(HWND hwnd, int nItem)
{
	LINLIST *		llp;
	int			nCnt;

	for (llp = _linl, nCnt = 0; llp != 0; llp = llp->ll_next) {
		SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, (LONG)llp);
		nCnt++;
	}
	return nCnt;
}

/*--------------------------------------------------------------------------
 * lingetdescription()
 */
BOOL lingetdescription(LINLIST *llp, 
	char **ppszId,	char **ppszDescription, char **ppszMatch, char **ppszFname, 
	int **pOwn)
{

	if (!llp) {
		llp = _linl;
	}
	if (!llp) {
		linnew((LINLIST *)0);
		llp = _linl;
	}
	if (ppszId) {
		*ppszId = llp->ll_name;
	}
	if (ppszDescription) {
		*ppszDescription = llp->ll_description;
	}
	if (ppszMatch) {
		*ppszMatch = llp->ll_match;
	}	
	if (ppszFname) {
		*ppszFname = llp->ll_linname;
	}
	if (pOwn) {
		*pOwn = &llp->ll_ownlineal;
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * lincreatekeytmp()
 */
int lincreatekeytmp(char *linfn, char *tmpfn)
{
	int		fd;
	int		fd2;
	long		size;

	if ((fd = Fopen(linfn, OF_READ)) < 0) {
		return 0;
	}

	if ((fd2 = Fcreate(tmpfn, 0)) < 0) {
		tmpfn[0] = 0;
	} else if (Fseek(LINSPACE + 1, fd, 0) > LINSPACE) {
		Fseek(LINSPACE, fd, 0);
		while((size = Fread(fd, FBUFSIZE, _linebuf)) > 0) {
			Fwrite(fd2, size, _linebuf);
		}
	}
	if (fd2 > 0) {
		Fclose(fd2);
	}
	Fclose(fd);
	return tmpfn[0] ? 1 : 0;
}

/*--------------------------------------------------------------------------
 * linkeymerge()
 */
int linkeymerge(char *pszLinealFile, char *pszDocMacFile)
{
	int		fd;
	int		fdDocMac;
	int		fdTmp;
	long		size;
	char		tmpfn[512];

	if ((fd = Fopen(pszLinealFile, OF_READ)) < 0) {
		return 0;
	}

	size = Fread(fd, LINSPACE, _linebuf);
	Fclose(fd);
	if (size != LINSPACE) {
		return 0;
	}

	/* copy lineal to tempfile */
	TmpName(tmpfn, 'K');
	if ((fdTmp = Fcreate(tmpfn, 0)) < 0) {
		return 0;
	}
	Fwrite(fdTmp, LINSPACE, _linebuf);

	if ((fdDocMac = Fopen(pszDocMacFile, OF_READ)) < 0) {
		Fclose(fdTmp);
		return 0;
	}

	/* copy extended document macros to tempfile */
	while((size = Fread(fdDocMac, FBUFSIZE, _linebuf)) > 0) {
		Fwrite(fdTmp, size, _linebuf);
	}

	Fclose(fdDocMac);
	Fclose(fdTmp);

	if ((fdTmp = Fopen(tmpfn, OF_READ)) < 0) {
		return 0;
	}

	if ((fd = Fcreate(pszLinealFile, 0)) < 0) {
		return 0;
	}

	/* copy extended document macros to tempfile */
	while((size = Fread(fdTmp, FBUFSIZE, _linebuf)) > 0) {
		Fwrite(fd, size, _linebuf);
	}

	Fclose(fd);
	Fclose(fdTmp);

	Fdelete(tmpfn);

	return 1;
}

/*--------------------------------------------------------------------------
 * linealread()
 */
static int linealread(char *fname, LINEAL *lp, int id, int forced)
{
	char 	keyfn[512];
	char *	fn;
	int  	fd;

	if ((fn = searchfile(fname)) != 0L && (fd = Fopen(fn,0)) > 0) {
		if (Fread(fd,LINSPACE,lp) != LINSPACE) {
			Fclose(fd);
			return 0;
		}
		Fclose(fd);
		strdcpy(keyfn, _datadir, "MODI.XXX");
		lincreatekeytmp(fn, keyfn);
	} else {
		return 0;
	}

	sfsplit(fname,(char *)0,lp->liname);
	lp->id = id;
	initfastts(lp);

	if (keyfn[0] != 0) {
		Mapread(id, keyfn);
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * lin_match()
 * find the correct lineal for a given file
 * 	1. if own lineal, try to read  own lineal from disc
 * 	2. if common lineal, ...
 *	3. if neither, use standard lineal _lineal
 */
int lin_match(LINEAL *linp, char *filename)
{
	char 			fname[1024];
	char			linealname[1024];
	LINLIST *		llp;
	LINEAL *		lp;
	PROJECTITEM *	pip;

	if ((pip = proj_finditem(filename)) != 0 &&
		pip->pi_doctype != 0) {
		lp = linload((LINLIST *)pip->pi_doctype);
	} else {

		sfsplit(filename,(char *)0, fname);
		for (llp = _linl, lp = 0; llp != 0 && lp == 0; llp = llp->ll_next) {
			if (match(fname,llp->ll_match)) {
				if (llp->ll_ownlineal) {
					related_file_name(filename,linealname,"LIN");
					if (linealread(linealname,linp,llp->ll_ctx,0) != 0) {
						lstrcpy(linp->modename, llp->ll_name);
						return 1;
					}
				}
				if ((lp = linload(llp)) == 0) {
					goto failed;
				}
			}
		}
	}
failed:
	memmove(linp,(lp) ? lp : &_lineal,sizeof _lineal);
	return 1;
}

/*--------------------------------------------------------------------------
 * linassign()
 * assign lineal data to file
 * if linp == 0, read lineal from disc according to filename pattern
 * match
 */
int linassign(FTABLE *fp, LINEAL *linp)
{
	if (!_linited) {
		lininit(&_lineal,_lineal.tabsize);
	}

	if ((fp->lin = _alloc(sizeof *fp->lin)) == 0)
		return 0;

	if (linp) {
		memmove(fp->lin,linp,sizeof *linp);
		return 1;
	}

	lin_match(fp->lin,fp->fname);
	return 1;
}

/*--------------------------------------------------------------------------
 * linname2id()
 */
#if 0
int linname2id(char *name)
{	
	LINLIST	*llp;

	name = basename(name);

	if (strcmp(name,"default") == 0)
		return LIN_DEFCTX;

	if ((llp = ll_find(_linl,name)) != (LINLIST *) 0) {
		return llp->ll_ctx;
	}

	return LIN_NOCTX;
}
# endif

/*--------------------------------------------------------------------------
 * dt_save()
 * save a document type: name + linealfile + match-extensions
 */
static char *szDocTypes = "doctypes";
static int dt_save(LINLIST *lp)
{
	char		szBuf[1024];

	wsprintf(szBuf,"%s,%s,%d,%s", lp->ll_linname, lp->ll_match, 
		lp->ll_ownlineal, lp->ll_description);
	return prof_savestring(szDocTypes,lp->ll_name,szBuf);
}

/*--------------------------------------------------------------------------
 * linsaveall()
 */
void linsaveall(LINLIST *llp)
{
	/*
	 * recursive save of ruler list. called with param == 0 for start of saving
	 */
	if (!llp) {
		prof_killsections((LPSTR)0, szDocTypes);
		linsaveall(_linl);
	} else {
		if (llp->ll_next) {
			linsaveall(llp->ll_next);
		}
		dt_save(llp);
	}
}

/*--------------------------------------------------------------------------
 * linnew()
 */
LINLIST *linnew(LINLIST *llp)
{
	LINLIST * llpNew;
	int		nLen;

	if ((llpNew = ll_insert(&_linl, sizeof *llpNew)) == 0) {
		return 0;
	}
	if (llp) {
	/* do not copy link pointer ! */
		memmove(&llpNew->ll_name, &llp->ll_name, 
			sizeof *llpNew - sizeof llpNew->ll_next);
	}
	llpNew->ll_lineal = 0;
	if ((nLen = lstrlen(llpNew->ll_name)) < sizeof(llpNew->ll_name) - 2) {
		llpNew->ll_name[nLen++] = '*';
		llpNew->ll_name[nLen] = 0;
	}
	return llpNew;
}

/*--------------------------------------------------------------------------
 * lindelete()
 */
void lindelete(LINLIST *llp)
{
	ll_delete(&_linl, llp);
}

/*--------------------------------------------------------------------------
 * lingetprivatefor()
 */
void *lingetprivatefor(char *name)
{
	LINLIST *	llp;

	for (llp = _linl; llp; llp = llp->ll_next) {
		if (lstrcmp(llp->ll_name, name) == 0) {
			return llp;
		}
	}

	return (void *)_linl;
}

/*--------------------------------------------------------------------------
 * linlinealfor()
 */
LINEAL *linlinealfor(void *p)
{
	LINLIST	*llp;

	if ((llp = (LINLIST *)p) == 0) {
		return 0;
	}
	return linload(llp);
}

/*--------------------------------------------------------------------------
 * dt_mk()
 * init a document type
 */
static int dt_mk(char *docname)
{
	char	 	*s,*szDesc,*szLinname,*szMatch,*szOwn;
	LINLIST	*llp;

	if ((llp = prof_llinsert(&_linl,sizeof *llp,szDocTypes,docname,&s)) == 0) {
		return 0;
	}

	/*
	 * if new inserted: generate context id
	 */
	if (llp->ll_ctx == 0)
		llp->ll_ctx = ++_ndoctypes;

	szLinname = strtok(s,",");
	szMatch = strtok((char*)0,",");
	szOwn = strtok((char*)0,",");
	szDesc = strtok((char *)0, ",");

	if (szOwn != 0) {
		/*
		 * if lineal name has changed for wellknown document,
		 * force reload of lineal
		 */
		if (llp->ll_linname[0] &&
		    strcmp(llp->ll_linname,szLinname)) {
			destroy(&llp->ll_lineal);
		}
		strcpy(llp->ll_linname,szLinname);
		strcpy(llp->ll_match,szMatch);
		if (szDesc) {
			strmaxcpy(llp->ll_description, szDesc, sizeof llp->ll_description);
		}
		llp->ll_ownlineal = Atol(szOwn);
	}
	_free(s);
	return 1;
}

/*--------------------------------------------------------------------------
 * dt_init()
 * init all document types
 */
int dt_init(void)
{
	return prof_enum(szDocTypes,dt_mk,0L);
}

/*--------------------------------------------------------------------------
 * EdLineal()
 * wrflag & 2 -> lp is given as par
 */
int EdLineal(int wrflag,LINEAL *lp)
{
	int 		fd;
	LINLIST 	*llp;
	FTABLE 	*fp = _currfile;
	char 	*fn;

	if ((wrflag & 2) == 0 && fp) {
		lp = fp->lin;
	}

	if (!lp)
		return 0;

	if (wrflag & 1) {
		strcpy(_linfsel.fname, lp->liname);
		if (fp) {
			ww_getstate(fp->wp,&lp->placement);
		}
	}

	if ((fn = rw_select(&_linfsel,(wrflag & 1) ? MMWRITE : MMREAD)) == 0) {
		return 0;
	}

	if (wrflag & 1) {
		if ((fd = Fopen(fn, OF_READWRITE)) < 0 && (fd = EdCreate(fn)) < 0) {
			return 0;
		}
	   	sfsplit(fn,(char *)0,lp->liname);
		Fwrite(fd,LINSPACE,lp);
		Fclose(fd);

		/*
		 * use as prototype
		 */
		for (llp = _linl; llp; llp = llp->ll_next) {
			if (llp->ll_lineal && lstrcmp(llp->ll_linname, lp->liname) == 0) {
				memmove(llp->ll_lineal,lp,sizeof *lp);
               }
		}
		return 1;
	}

	if (linealread(fn,lp,lp->id,1)) {
		if ((wrflag & 2) == 0)
			linchange();
		return 1;
	}

	return 0;
}
