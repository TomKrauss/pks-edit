/*
 * KeyBindingFileIO.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: read the PKSEDIT.KEY File
 *
 * 										created: 27.01.1991
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
+ */

#include <windows.h>
#include <string.h>
#include "tos.h"
#include "pksedit.h"
#include "caretmovement.h"
#include "edierror.h"
#include "edfuncs.h"
#include "fileselector.h"
#include "winterf.h"
#include "winfo.h"
#include "grammar.h"
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
extern 	int 	macro_createTempFile(char *linfn, char *tmpfn);

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

	if (LoadString(ui_getResourceModule(),msgId,msg,sizeof msg)) {
		wsprintf(b,/*STR*/"Error %s %ld: %s",(LPSTR)_keyfile->fname,n,(LPSTR)msg);
		ln_createAndAddSimple(&_outfile,b);
	}
}

/*--------------------------------------------------------------------------
 * macro_getValidEscapeMacros()
 */
static void macro_getValidEscapeMacros(char *pszValid)
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
 * macro_escapePasteForId()
 */
static PASTE *macro_escapePasteForId(int id)
{
	PASTE *		pp;

	if ((pp = bl_getTextBlock(id, _esclist[ft_getCurrentDocument()->documentDescriptor->id])) != 0) {
		return pp;
	}
	return 0 /*bl_getTextBlock(id,_esclist[0]) */ ;
}

/*--------------------------------------------------------------------------
 * macro_textForEscapeMacro()
 */
static char *macro_textForEscapeMacro(char *s)
{
	PASTE *	pp;

	pp = macro_escapePasteForId(*s);
	return bl_convertPasteBufferToText(_linebuf, _linebuf + 256, pp);
}

/*--------------------------------------------------------------------------
 * EdMacroEscape()
 */
int EdMacroEscape(void)
{
	WINFO* wp;
	PASTE  *	pp;
	static unsigned char id;
	char 	cIdentChars[256];

	wp = ww_getCurrentEditorWindow();
	if (wp == NULL) {
		return 0;
	}
     macro_getValidEscapeMacros(cIdentChars);
	if ((id = dlg_displayDialogTemplate(id, macro_textForEscapeMacro, 
		cIdentChars)) == 0) {
		return 0;
	}

	pp = macro_escapePasteForId(id);

	if (!pp) {
		error_showErrorById(IDS_MSGESCAPEUNDEF);
		return 0;
	}
	return bl_pasteBlock(pp, 0, wp->caret.offset, 0);
}

/*--------------------------------------------------------------------------
 * macro_getTextInQuotes()
 */
static int _quotedLength;
char *macro_getTextInQuotes(char *d,char *s,int maxlen)
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
	_quotedLength = (int)(d-start);
	return s;
}

/*--------------------------------------------------------------------------
 * macro_cutTextWithQuotes()
 */
static int _mandatory = 1;
static char *macro_cutTextWithQuotes(char **pszText)
{	char buf[256],*d,*s;

	if ((s = string_skipBlanks(*pszText)) == 0 || *s++ != '"') {
		if (_mandatory)
			macro_error(IDS_MSGMISSINGAPOSTRPOHE);
		return 0;
	}
	if ((d = macro_getTextInQuotes(buf,s,sizeof(buf))) == 0)
		return 0;

	if (*d == '"')
		d++;
	buf[_quotedLength] = 0;

	*pszText = d;
	return _strdup(buf);
}

/*--------------------------------------------------------------------------
 * macro_advanceToNextToken()
 */
static int macro_advanceToNextToken(LINE **lp,PASTE *pp,char *s)
{	register LINE *lnlast;
	LINE *lnfirst;
	intptr_t cfirst,clast,p1;

	if ((s  = string_skipBlanks(s)) == 0L)
		return 0;

	s++;				/* skip \" */
	lnfirst = *lp;
	lnlast  = lnfirst;
	cfirst  = p1 = s-lnlast->lbuf;
	for (;;) {
		if ((s = macro_getTextInQuotes(s,s,32000)) != 0 &&
		     *s == '"') break;
		lnlast->len = (int)(p1+_quotedLength);
		if ((lnlast = lnlast->next) == 0) {
			macro_error(IDS_MSGMISSINGAPOSTRPOHE);
			return 0;
		}
		s  = lnlast->lbuf;
		p1 = 0;
	}
	clast  = p1+_quotedLength;
	*lp    = lnlast;
	return bl_cutTextWithOptions(pp,lnfirst,lnlast,(int)cfirst,(int)clast,0);
}

/*--------------------------------------------------------------------------
 * macro_findTextBuffer()
 */
static PASTE *macro_findTextBuffer(LINE **lp,unsigned char *s,PASTELIST **pl,int id)
{	PASTE *pp;
	int   defmacro = (id == 0);

	if (id == 0) {
		if ((s = string_skipBlanks(s)) == 0L) return 0;
		id = *s++;
	}
	if ((pp = bl_lookupPasteBuffer(id,1,pl)) == 0L)
		return 0;

	if (defmacro) {
		if ((s = string_skipBlanks(s)) == 0L) return 0;
		if (*s == '!') {
			pp->pflg = 1;
			if (bl_cutTextWithOptions(pp,*lp,*lp,(int)(s-(*lp)->lbuf),(*lp)->len,0))
				return pp;
		}
	}

	if (macro_advanceToNextToken(lp, pp, (char*)s)) {
		return pp;
	}

	return 0;
}

/*
 * Expand a code template optionally containing ${....} references and return
 * the expanded text.
 */
static STRING_BUF* macro_expandCodeTemplate(WINFO* wp, unsigned char* pszSource) {
	size_t nInitialSize = strlen(pszSource);
	STRING_BUF* pResult = stringbuf_create(nInitialSize);
	unsigned char* pVar = NULL;
	unsigned char variable[50];
	unsigned char expandedVariable[512];
	unsigned char c;

	while ((c = *pszSource++) != 0) {
		if (pVar) {
			if (c == '}') {
				*pVar = 0;
				string_getVariable(wp, variable, expandedVariable);
				stringbuf_appendString(pResult, expandedVariable);
				pVar = NULL;
			} else if (pVar < variable+sizeof variable-1) {
				*pVar++ = c;
			}
		} else if (c == '$' && *pszSource == '{') {
			pszSource++;
			pVar = variable;
		} else {
			stringbuf_appendChar(pResult, c);
		}
	}
	stringbuf_appendChar(pResult, '\n');
	return pResult;
}

/*--------------------------------------------------------------------------
 * macro_expandAbbreviation()
 */
int macro_expandAbbreviation(WINFO *wp, LINE *lp,int offs) {
	UCLIST *up;
	long 		o2;
	int			domacro = 0;
	FTABLE* fp = wp->fp;
	EDIT_CONFIGURATION* pConfig = fp->documentDescriptor;
	GRAMMAR* pGrammar = pConfig->grammar;

	if ((up = uc_find(pGrammar, lp->lbuf, offs)) == 0) {
		return 0;
	}
	if (up->action == UA_ABBREV) {
		domacro = 0;
	} else if (up->action == UA_UCMACRO) {
		domacro = 1;
	} else {
		return 0;
	}

	o2 = offs-up->len;
	if ((lp = ln_modify(fp,lp,offs,o2)) == 0L)
		return 0;
	caret_placeCursorInCurrentFile(wp, wp->caret.ln,o2);
	if (!domacro) {
		int ret = 0;
		STRING_BUF* pSB = macro_expandCodeTemplate(wp, up->p.uc_template);
		PASTE pasteBuffer;
		memset(&pasteBuffer, 0, sizeof pasteBuffer);
		unsigned char* pszText = stringbuf_getString(pSB);
		if (bl_convertTextToPasteBuffer(&pasteBuffer, pszText, pszText + strlen(pszText), pConfig->nl, pConfig->nl2, pConfig->cr)) {
			bl_pasteBlock(&pasteBuffer, 0, o2, 0);
			ret = 1;
		}
		stringbuf_destroy(pSB);
		return ret;
	}
	render_repaintCurrentLine(wp); 
	return macro_executeByName(up->p.uc_macro);
}

/*--------------------------------------------------------------------------
 * macro_parseKeyDefinitions()
 */
static void macro_parseKeyDefinitions(FTABLE *fp) {	
	LINE *lp;
	char* s;
	char c;

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
				case 'T':
					macro_findTextBuffer(&lp,s,&_esclist[_context],0);
					break;
			}
		}
		lp = lp->next;
	}
}

/*--------------------------------------------------------------------------
 * macro_readMappingFile()
 * Read a menu mapping/keybinding/mouse mapping definition file.
 */
int macro_readMappingFile(int context, char *pszFilename) {
	FTABLE 	ft;
	static 	BOOL bMacrosDefined;
	int 		ret = 0;

	if (pszFilename != 0 && (ret = ft_readfileWithOptions(&ft,pszFilename,-1)) != 0) {

		if (context >= 0) {
			_context = context;
		} else {
			/*
			 * remove old entries
			 */
			_context = DEFAULT_DOCUMENT_DESCRIPTOR_CTX;
		}

		macro_parseKeyDefinitions(&ft);
		ln_listfree(ft.firstl);

		bMacrosDefined = 1;
	}

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

	return 0;
}

/*--------------------------------------------------------------------------
 * macro_createTempFile()
 */
static int macro_createTempFile(char* linfn, char* tmpfn) {
	int		fd;
	int		fd2;
	long		size;

	if ((fd = Fopen(linfn, OF_READ)) < 0) {
		return 0;
	}

	if ((fd2 = Fcreate(tmpfn, 0)) < 0) {
		tmpfn[0] = 0;
	} else {
		while ((size = Fread(fd, FBUFSIZE, _linebuf)) > 0) {
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
 * EdDocMacrosEdit()
 */
int EdDocMacrosEdit(void)
{
	char 	keyfile[256];
	extern char *_pksSysFolder;

	if (!ft_getCurrentDocument()) {
		return 0;
	}
	string_concatPathAndFilename(keyfile, _pksSysFolder, "MODI.TMP");
	if (macro_createTempFile(file_searchFileInPKSEditLocation(ft_getCurrentDocument()->documentDescriptor->name), keyfile)) {
		return xref_openFile(keyfile, -1L, (void*)0);
	}
	return 0;
}

