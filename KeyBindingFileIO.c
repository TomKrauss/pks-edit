/*
 * KeyBindingFileIO.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: read the PKSEDIT.KEY File
 *
 * 										created: 27.01.1991
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 * All Rights Reserved.
 */

#include <windows.h>
#include <string.h>
#include "pksedit.h"
#include "caretmovement.h"
#include "edierror.h"
#include "edfuncs.h"
#include "fileselector.h"
#include "brackets.h"
#include "winterf.h"
#include "winfo.h"
#include "stringutil.h"
#include "fileutil.h"
#include "textblocks.h"
#include "regexp.h"
#include "documenttypes.h"
#include "crossreferencelinks.h"

#define	MAX_CONTEXT	32

extern int dlg_displayDialogTemplate(unsigned char c, 
			char *(*fpTextForTmplate)(char *s), char *s);
extern unsigned char *bl_convertPasteBufferToText(unsigned char *b, unsigned char *end, 
			PASTE *pp);

/*--------------------------------------------------------------------------
 * EXTERNALS
 */
extern	FSELINFO 	_linfsel;

extern	int uc_add(char* pat, char* p, int type, int id);
extern 	PASTE	*bl_lookupPasteBuffer();
extern	PASTE	*bl_getTextBlock(int id, PASTELIST *pp);
extern 	int 	doctypes_createTempFileForDocumentType(char *linfn, char *tmpfn);

static 	PASTELIST *_abbrevlist;

/* current macrofile line */
static	LINE      *_macroline;
static	FTABLE	*_keyfile;

/* allow context sensitive makros and mappings */
static	int		_context     = DEFAULT_DOCUMENT_DESCRIPTOR_CTX;
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
		ln_createAndAddSimple(&_outfile,b);
	}
}

/*--------------------------------------------------------------------------
 * ValidEscapeMacros()
 */
void ValidEscapeMacros(char *pszValid)
{
	PASTELIST *pl;

	pl = _esclist[ft_getCurrentDocument()->documentDescriptor->id];
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

	if ((pp = bl_getTextBlock(id, _esclist[ft_getCurrentDocument()->documentDescriptor->id])) != 0) {
		return pp;
	}
	return 0 /*bl_getTextBlock(id,_esclist[0]) */ ;
}

/*--------------------------------------------------------------------------
 * TextForEscapeMacro()
 */
static char *TextForEscapeMacro(char *s)
{
	PASTE *	pp;

	pp = EscapePasteForId(*s);
	return bl_convertPasteBufferToText(_linebuf, _linebuf + 256, pp);
}

/*--------------------------------------------------------------------------
 * EdMacroEscape()
 */
int EdMacroEscape(void)
{
	PASTE  *	pp;
	static unsigned char id;
	char 	cIdentChars[256];

	if (!ft_getCurrentDocument()) {
		return 0;
	}
     ValidEscapeMacros(cIdentChars);
	if ((id = dlg_displayDialogTemplate(id, TextForEscapeMacro, 
		cIdentChars)) == 0) {
		return 0;
	}

	pp = EscapePasteForId(id);

	if (!pp) {
		error_showErrorById(IDS_MSGESCAPEUNDEF);
		return 0;
	}
	return bl_pasteBlock(pp, 0, ft_getCurrentDocument()->caret.offset, 0);
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
	_qulen = (int)(d-start);
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
	return string_allocate(buf);
}

/*--------------------------------------------------------------------------
 * advtok()
 */
static int advtok(LINE **lp,PASTE *pp,char *s)
{	register LINE *lnlast;
	LINE *lnfirst;
	intptr_t cfirst,clast,p1;

	if ((s  = skbl(s)) == 0L)
		return 0;

	s++;				/* skip \" */
	lnfirst = *lp;
	lnlast  = lnfirst;
	cfirst  = p1 = s-lnlast->lbuf;
	for (;;) {
		if ((s = getquotes(s,s,32000)) != 0 &&
		     *s == '"') break;
		lnlast->len = (int)(p1+_qulen);
		if ((lnlast = lnlast->next) == 0) {
			macro_error(IDS_MSGMISSINGAPOSTRPOHE);
			return 0;
		}
		s  = lnlast->lbuf;
		p1 = 0;
	}
	clast  = p1+_qulen;
	*lp    = lnlast;
	return bl_cutTextWithOptions(pp,lnfirst,lnlast,(int)cfirst,(int)clast,0);
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
	if ((pp = bl_lookupPasteBuffer(id,1,pl)) == 0L)
		return 0;

	if (defmacro) {
		if ((s = skbl(s)) == 0L) return 0;
		if (*s == '!') {
			pp->pflg = 1;
			if (bl_cutTextWithOptions(pp,*lp,*lp,(int)(s-(*lp)->lbuf),(*lp)->len,0))
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

	v = string_convertToLong(s);
	i1 = string_convertToLong(_strtolend);
	i2 = string_convertToLong(_strtolend);
	i3 = string_convertToLong(_strtolend);
	i4 = string_convertToLong(_strtolend);

	return sm_defineBracketIndentation(c1,c2,v,i1,i2,i3,i4,_context);
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
	int id = 		fp->documentDescriptor->id;
	int			domacro = 0;
	WINFO* wp = WIPOI(fp);

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
	caret_placeCursorInCurrentFile(wp, fp->ln,o2);
	return (domacro) ? 
		render_repaintCurrentLine(), macro_executeByName((char *)up->p) : bl_pasteBlock(up->p,0,o2,0);
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
	return uc_add(shortie,pp->pln->lbuf,UA_ABBREV,_context);
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
			if (_context == DOCUMENT_DESCRIPTOR_NO_CTX)
				_context = DEFAULT_DOCUMENT_DESCRIPTOR_CTX;
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

	if (target != 0 && (ret = ft_readfileWithOptions(&ft,target,-1)) != 0) {

		if (context >= 0) {
			_context = context;
		} else {
			/*
			 * remove old entries
			 */
			_context = DEFAULT_DOCUMENT_DESCRIPTOR_CTX;
			_abid = 0;
		}

		parsekeydefs(&ft);
		ln_listfree(ft.firstl);

		wehavemac = 1;
	}

	sm_setup();
	regex_compileCharacterClasses((char *) 0);

	if (_outfile.firstl != 0) {
		char protname[256];
	
		string_concatPathAndFilename(protname,file_getTempDirectory(),"KEY.ERR");
		ft_writeFileAndClose(&_outfile,protname,0);
		xref_openSearchList(protname,1);
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * EdDocMacrosAdd()
 */
int EdDocMacrosAdd(void)
{
	char	*	fn;

	if (!ft_getCurrentDocument() || (fn = fsel_selectFileWithOptions(&_linfsel,MADDDOCMAC, TRUE)) == 0) {
		return 0;
	}

	if (EdSaveFile(SAV_SAVE|SAV_FORCED) == 0) { 
		return 0;
	}

	doctypes_mergeDocumentTypes(fn, ft_getCurrentDocument()->fname);

	return 0;
}

/*--------------------------------------------------------------------------
 * EdDocMacrosEdit()
 */
int EdDocMacrosEdit(void)
{
	char 	keyfile[256];
	extern char *_datadir;

	if (!ft_getCurrentDocument()) {
		return 0;
	}
	string_concatPathAndFilename(keyfile, _datadir, "MODI.TMP");
	if (doctypes_createTempFileForDocumentType(file_searchFileInPKSEditLocation(ft_getCurrentDocument()->documentDescriptor->name), keyfile)) {
		return xref_openFile(keyfile, -1L, (void*)0);
	}
	return 0;
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
	uc_add(string_allocate(trigger), pp, UA_ABBREV, _context);
}

/*--------------------------------------------------------------------------
 * bind_abbrevmacro()
 */
void bind_abbrevmacro(char *trigger, char *macroname)
{
	uc_add(string_allocate(trigger), string_allocate(macroname), UA_UCMACRO, _context);
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
	sm_defineBracketIndentation(string_allocate(c1), string_allocate(c2), v, i1, i2, i3, i4, _context);
}

# endif

