/*
 * KeyBindingFileIO.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: read the PKSEDIT.KEY File
 *
 * 										created      : 27.01.1991
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 * All Rights Reserved.
 */

#include <windows.h>
#include	<string.h>
#include	"pksedit.h"
#include	"lineoperations.h"
#include	"edierror.h"
#include	"edfuncs.h"
#include	"edifsel.h"
#include	"uc.h"
#include	"winterf.h"

#define	MAX_CONTEXT	32

extern	unsigned char *stralloc(unsigned char *buf);
extern	LPSTR lstrchr(char *s, char c);
extern	LINE *ln_modify(FTABLE *fp, LINE *lp, int o1, int o2);
extern 	long ln_cnt(LINE *lps, LINE *lpe);
extern 	long Atol(char *s);
extern 	char	*TmpDir();
extern int DialogTemplate(unsigned char c, 
			char *(*fpTextForTmplate)(char *s), char *s);
extern unsigned char *BlockAsBuffer(unsigned char *b, unsigned char *end, 
			PASTE *pp);

/*--------------------------------------------------------------------------
 * EXTERNALS
 */
extern 	char 	*_strtolend;
extern	FTABLE 	*_currfile;
extern	FSELINFO 	_linfsel;

extern 	PASTE	*plistenq();
extern	PASTE	*pp_find(int id, PASTELIST *pp);
extern	void 	pp_listfree(PASTELIST **pp);
extern 	int 		lincreatekeytmp(char *linfn, char *tmpfn);
extern 	char *	searchfile(char *);

static 	PASTELIST *_abbrevlist;

/* current macrofile line */
static	LINE      *_macroline;
static	FTABLE	*_keyfile;

/* allow context sensitive makros and mappings */
static	int		_context     = LIN_DEFCTX;
PASTELIST *_esclist[MAX_CONTEXT];


/*--------------------------------------------------------------------------
 * macro_error()
 */
FTABLE _outfile;
static void macro_error(int msgId)
{	
	long 	n;
	char 	msg[128],b[512];

	n = ln_cnt(_keyfile->firstl,_macroline);

	if (LoadString(hInst,msgId,msg,sizeof msg)) {
		wsprintf(b,/*STR*/"Error %s %ld: %s",(LPSTR)_keyfile->fname,n,(LPSTR)msg);
		putline(&_outfile,b);
	}
}

/*--------------------------------------------------------------------------
 * ValidEscapeMacros()
 */
void ValidEscapeMacros(char *pszValid)
{
	PASTELIST *pl;

	pl = _esclist[_currfile->lin->id];
	while (pl != 0) {
		*pszValid++ = pl->id;
		pl = pl->next;
	}
#if 0
	pl = _esclist[0];
	while (pl != 0) {
		*pszValid++ = pl->id;
		pl = pl->next;
	}
# endif
	*pszValid = 0;
}

/*--------------------------------------------------------------------------
 * EscapePasteForId()
 */
static PASTE *EscapePasteForId(int id)
{
	PASTE *		pp;

	if ((pp = pp_find(id, _esclist[_currfile->lin->id])) != 0) {
		return pp;
	}
	return 0 /*pp_find(id,_esclist[0]) */ ;
}

/*--------------------------------------------------------------------------
 * TextForEscapeMacro()
 */
static char *TextForEscapeMacro(char *s)
{
	PASTE *	pp;

	pp = EscapePasteForId(*s);
	return BlockAsBuffer(_linebuf, _linebuf + 256, pp);
}

/*--------------------------------------------------------------------------
 * EdMacroEscape()
 */
int EdMacroEscape(void)
{
	PASTE  *	pp;
	static unsigned char id;
	char 	cIdentChars[256];

	if (!_currfile) {
		return 0;
	}
     ValidEscapeMacros(cIdentChars);
	if ((id = DialogTemplate(id, TextForEscapeMacro, 
		cIdentChars)) == 0) {
		return 0;
	}

	pp = EscapePasteForId(id);

	if (!pp) {
		ed_error(IDS_MSGESCAPEUNDEF);
		return 0;
	}
	return pasteblk(pp, 0, _currfile->lnoffset, 0);
}

/*--------------------------------------------------------------------------
 * skbl()
 */
unsigned char *skbl(register unsigned char *s)
{
	while (*s == ' ' || *s == '\t') s++;
	return (*s ? s : 0L);
}

/*--------------------------------------------------------------------------
 * getquotes()
 */
int _qulen;
char *getquotes(char *d,char *s,int maxlen)
{	char *start = d;

	while (*s && *s != '"') {
		if (*s == '\\' && s[1])
			s++;
		if (--maxlen <= 0) {
			macro_error(IDS_MSGMACROIDTOOLONG);
			return 0;
		}
		*d++ = *s++;
	}
	_qulen = d-start;
	return s;
}

/*--------------------------------------------------------------------------
 * cutquotes()
 */
static int _mandatory = 1;
static char *cutquotes(char **S)
{	char buf[256],*d,*s;

	if ((s = skbl(*S)) == 0 || *s++ != '"') {
		if (_mandatory)
			macro_error(IDS_MSGMISSINGAPOSTRPOHE);
		return 0;
	}
	if ((d = getquotes(buf,s,sizeof(buf))) == 0)
		return 0;

	if (*d == '"')
		d++;
	buf[_qulen] = 0;

	*S = d;
	return stralloc(buf);
}

/*--------------------------------------------------------------------------
 * advtok()
 */
static int advtok(LINE **lp,PASTE *pp,char *s)
{	register LINE *lnlast;
	LINE *lnfirst;
	int cfirst,clast,p1;

	if ((s  = skbl(s)) == 0L)
		return 0;

	s++;				/* skip \" */
	lnfirst = *lp;
	lnlast  = lnfirst;
	cfirst  = p1 = s-lnlast->lbuf;
	for (;;) {
		if ((s = getquotes(s,s,32000)) != 0 &&
		     *s == '"') break;
		lnlast->len = p1+_qulen;
		if ((lnlast = lnlast->next) == 0) {
			macro_error(IDS_MSGMISSINGAPOSTRPOHE);
			return 0;
		}
		s  = lnlast->lbuf;
		p1 = 0;
	}
	clast  = p1+_qulen;
	*lp    = lnlast;
	return blcut(pp,lnfirst,lnlast,cfirst,clast,0);
}

/*--------------------------------------------------------------------------
 * getbuf()
 */
static PASTE *getbuf(LINE **lp,unsigned char *s,PASTELIST **pl,int id)
{	PASTE *pp;
	int   defmacro = (id == 0);

	if (id == 0) {
		if ((s = skbl(s)) == 0L) return 0;
		id = *s++;
	}
	if ((pp = plistenq(id,1,pl)) == 0L)
		return 0;

	if (defmacro) {
		if ((s = skbl(s)) == 0L) return 0;
		if (*s == '!') {
			pp->pflg = 1;
			if (blcut(pp,*lp,*lp,(int)(s-(*lp)->lbuf),(*lp)->len,0))
				return pp;
		}
	}

	if (advtok(lp,pp,(char *)s))
		return pp;

	return 0;
}

/*--------------------------------------------------------------------------
 * setupccl()
 */
static int setupccl(char *s)
{	char *c1,*c2;
	int  v,i1,i2,i3,i4;

	if ((c1 = cutquotes(&s)) == 0)
		return 0;
	if ((c2 = cutquotes(&s)) == 0)
		return 0;

	v = Atol(s);
	i1 = Atol(_strtolend);
	i2 = Atol(_strtolend);
	i3 = Atol(_strtolend);
	i4 = Atol(_strtolend);

	return sm_define(c1,c2,v,i1,i2,i3,i4,_context);
}

/*--------------------------------------------------------------------------
 * set2pars()
 */
static void set2pars(char *s,void (*func)(char *s1,char *s2))
{	char *c1,*c2;

	if ((c1 = cutquotes(&s)) != 0 &&
	    (c2 = cutquotes(&s)) != 0) 
		(*func)(c1,c2);
}

/*--------------------------------------------------------------------------
 * doabbrev()
 */
int doabbrev(FTABLE *fp, LINE *lp,int offs)
{
	struct uclist *up;
	long 		o2;
	int id = 		fp->lin->id;
	int			domacro = 0;

	if ((up = uc_find(id,lp->lbuf,offs,UA_ABBREV)) != 0) {
		domacro = 0;
	} else if ((up = uc_find(id,lp->lbuf,offs,UA_UCMACRO)) != 0) {
		domacro = 1;
	} else {
		return 0;
	}

	o2 = offs-up->len;
	if ((lp = ln_modify(fp,lp,offs,o2)) == 0L)
		return 0;
	curpos(fp->ln,o2);
	return (domacro) ? 
		redrawline(), do_macbyname((char *)up->p) : pasteblk(up->p,0,o2,0);
}

/*--------------------------------------------------------------------------
 * setabbrev()
 */
static int _abid;
static int setabbrev(LINE **lp,char *s)
{	char *shortie;
	PASTE *pp;

	if ((shortie = cutquotes(&s)) == 0 ||
	    (pp = getbuf(lp,s,&_abbrevlist,++_abid)) == 0L)
		return 0;
	return uc_add(shortie,pp,UA_ABBREV,_context);
}

/*--------------------------------------------------------------------------
 * setuc_macro()
 */
static void setuc_macro(char *trigger, char *macroname)
{
	uc_add(trigger,macroname,UA_UCMACRO,_context);
}

/*--------------------------------------------------------------------------
 * parsekeydefs()
 */
static void parsekeydefs(FTABLE *fp)
{	LINE *lp;
	register char *s,c;

	_keyfile = fp;
	lp = fp->firstl;
	while(lp) {
		if (lp->next && lp->len) {
			if (_context == LIN_NOCTX)
				_context = LIN_DEFCTX;
			_macroline = lp;
			s = lp->lbuf;
			c = *s++;
			switch(c) {
				case 'A':
					set2pars(s,setuc_macro);
					break;
				case 'a':
					setabbrev(&lp,s);
					break;
				case 'T':
					getbuf(&lp,s,&_esclist[_context],0);
					break;
				case 'k':
					setupccl(s);
					break;
			}
		}
		lp = lp->next;
	}
}

/*--------------------------------------------------------------------------
 * Mapread()
 */
int Mapread(int context, char *target)
{
	FTABLE 	ft;
	static 	wehavemac;
	int 		ret = 0;

	if (target != 0 && (ret = Readfile(&ft,target,-1)) != 0) {

		if (context >= 0) {
			_context = context;
		} else {
			/*
			 * remove old entries
			 */
			_context = LIN_DEFCTX;
			_abid = 0;
		}

		parsekeydefs(&ft);
		lnlistfree(ft.firstl);

		wehavemac = 1;
	}

	sm_setup();
	fixsets((char *) 0);

	if (_outfile.firstl != 0) {
		char protname[256];
	
		strdcpy(protname,TmpDir(),"KEY.ERR");
		Writeandclose(&_outfile,protname,0);
		stepnofsel(protname,1);
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * EdDocMacrosAdd()
 */
int EdDocMacrosAdd(void)
{
	char	*	fn;

	if (!_currfile || (fn = rw_select(&_linfsel,MADDDOCMAC)) == 0) {
		return 0;
	}

	if (EdSaveFile(SAV_SAVE|SAV_FORCED) == 0) { 
		return 0;
	}

	linkeymerge(fn, _currfile->fname);

	return 0;
}

/*--------------------------------------------------------------------------
 * EdDocMacrosEdit()
 */
int EdDocMacrosEdit(void)
{
	char 	keyfile[256];
	extern char *_datadir;

	if (!_currfile) {
		return 0;
	}
	strdcpy(keyfile, _datadir, "MODI.TMP");
	if (lincreatekeytmp(searchfile(_currfile->lin->liname), keyfile)) {
		return tagopen(keyfile, -1L, (void*)0);
	}
}

#if defined(MACROS20)
/*--------------------------------------------------------------------------
 * SetRulerContext()
 */
SetRulerContext(char *s)
{
	return 1;
}

/*--------------------------------------------------------------------------
 * bind_abbreviation()
 */
static int _abid;
void bind_abbreviation(char *trigger, char *s)
{
	PASTE *pp;

	if ((pp = getbuf(s, &_abbrevlist, ++_abid)) == 0L) {
		return;
	}
	uc_add(stralloc(trigger), pp, UA_ABBREV, _context);
}

/*--------------------------------------------------------------------------
 * bind_abbrevmacro()
 */
void bind_abbrevmacro(char *trigger, char *macroname)
{
	uc_add(stralloc(trigger), stralloc(macroname), UA_UCMACRO, _context);
}

/*--------------------------------------------------------------------------
 * bind_escape()
 */
void bind_escape(char id, char *s)
{
	getbuf(s, &_esclist[_context], id);
}

/*--------------------------------------------------------------------------
 * bind_ccldefinition()
 */
void bind_ccldefinition(char *c1, char *c2, int v, int i1, int i2. int i3. int i4) {
	sm_define(stralloc(c1), stralloc(c2), v, i1, i2, i3, i4, _context);
}

# endif

