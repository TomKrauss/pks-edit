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
#include "editorconfiguration.h"

#define	MAX_CONTEXT	32

extern int dlg_displayDialogTemplate(unsigned char c, 
			char *(*fpTextForTmplate)(char *s), char *s);
extern unsigned char *bl_convertPasteBufferToText(unsigned char *b, unsigned char *end, 
			PASTE *pp);

typedef struct tagTEMPLATE_ACTION {
	long ta_cursorDeltaLn;		// If the cursor should be positioned after inserting the template, this is the number of lines relative to the beginning of the inserted code.
	long ta_cursorDeltaCol;		// If the cursor should be positioned after inserting the template, this is the number of columns relative to the beginning of the inserted code.
	long ta_selectionDeltaLn;	// If text should be selected after inserting the template, this is the number of lines relative to the cursor as specified by ta_cursorDeltaLn.
	long ta_selectionDeltaCol;	// If text should be selected after inserting the template, this is the number of columns relative to the cursor as specified by ta_cursorDeltaLn.
} TEMPLATE_ACTION;

/*--------------------------------------------------------------------------
 * EXTERNALS
 */
extern	FSELINFO 	_linfsel;

extern	PASTE	*bl_getTextBlock(int id, PASTELIST *pp);
extern 	int 	macro_createTempFile(char *linfn, char *tmpfn);

static 	PASTELIST *_abbrevlist;

/* current macrofile line */
static	LINE      *_macroline;
static	FTABLE	*_keyfile;

/* allow context sensitive makros and mappings */
static	int		_context     = DEFAULT_DOCUMENT_DESCRIPTOR_CTX;


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
static STRING_BUF* macro_expandCodeTemplate(WINFO* wp, TEMPLATE_ACTION *pTAction, unsigned char* pszSelected, unsigned char* pszSource) {
	size_t nInitialSize = strlen(pszSource);
	STRING_BUF* pResult = stringbuf_create(nInitialSize);
	unsigned char* pVar = NULL;
	unsigned char variable[50];
	unsigned char expandedVariable[512];
	unsigned char c;
	long col = 0;
	long ln = 0;

	while ((c = *pszSource++) != 0) {
		if (pVar) {
			if (c == '}') {
				*pVar = 0;
				if (strcmp("cursor", variable) == 0) {
					pTAction->ta_cursorDeltaCol = col;
					pTAction->ta_cursorDeltaLn = ln;
					expandedVariable[0] = 0;
				} else if (strcmp("selection_end", variable) == 0) {
					pTAction->ta_selectionDeltaCol = col - pTAction->ta_cursorDeltaCol;
					pTAction->ta_selectionDeltaLn = ln - pTAction->ta_cursorDeltaLn;
					expandedVariable[0] = 0;
				} else if (strcmp("word_selection", variable) == 0) {
					strcpy(expandedVariable, pszSelected);
				} else {
					string_getVariable(wp, variable, expandedVariable);
				}
				stringbuf_appendString(pResult, expandedVariable);
				col += (long)strlen(expandedVariable);
				pVar = NULL;
			} else if (pVar < variable+sizeof variable-1) {
				*pVar++ = c;
			}
		} else if (c == '$' && *pszSource == '{') {
			pszSource++;
			pVar = variable;
		} else {
			if (c == '\n') {
				ln++;
				col = 0;
			} else {
				col++;
			}
			stringbuf_appendChar(pResult, c);
		}
	}
	stringbuf_appendChar(pResult, '\n');
	return pResult;
}

/*
 * Delete the current identifier left to or under the cursor for the purpose of being replaced by a macro. 
 */
static void macro_replaceCurrentWord(WINFO* wp) {
	if (ww_hasSelection(wp)) {
		EdBlockDelete(0);
	} else {
		char szIdentifier[100];
		char* pszBegin;
		char* pszEnd;
		if (xref_findIdentifierCloseToCaret(szIdentifier, szIdentifier + sizeof szIdentifier, &pszBegin, &pszEnd, TRUE)) {
			size_t o1 = pszBegin - wp->caret.linePointer->lbuf;
			size_t o2 = o1 + pszEnd - pszBegin;
			ln_modify(wp->fp, wp->caret.linePointer, (int)o2, (int)o1);
			wp->caret.col = (long)o1;
			render_repaintCurrentLine(wp);
		}
	}
}
/*
 * Insert a selected code template 'up'. 
 * If 'bReplaceCurrentWord' is TRUE, the currently selected word / identifier close to the
 * cursor is replaced by the inserted template.
 */
int macro_insertCodeTemplate(WINFO* wp, UCLIST* up, BOOL bReplaceCurrentWord) {
	char szIdentifier[100];
	FTABLE* fp = wp->fp;
	EDIT_CONFIGURATION* pConfig = fp->documentDescriptor;
	TEMPLATE_ACTION templateAction;
	int ret = 0;
	memset(&templateAction, 0, sizeof templateAction);
	xref_getSelectedIdentifier(szIdentifier, sizeof szIdentifier);
	STRING_BUF* pSB = macro_expandCodeTemplate(wp, &templateAction, szIdentifier, up->p.uc_template);
	PASTE pasteBuffer;
	memset(&pasteBuffer, 0, sizeof pasteBuffer);
	unsigned char* pszText = stringbuf_getString(pSB);
	if (bl_convertTextToPasteBuffer(&pasteBuffer, pszText, pszText + strlen(pszText), pConfig->nl, pConfig->nl2, pConfig->cr)) {
		if (bReplaceCurrentWord) {
			macro_replaceCurrentWord(wp);
		}
		CARET oldCaret = wp->caret;
		bl_pasteBlock(&pasteBuffer, 0, oldCaret.col, 0);
		if (templateAction.ta_cursorDeltaCol != 0 || templateAction.ta_cursorDeltaLn != 0) {
			long col = templateAction.ta_cursorDeltaLn ? templateAction.ta_cursorDeltaCol : templateAction.ta_cursorDeltaCol + oldCaret.col;
			long ln = templateAction.ta_cursorDeltaLn + oldCaret.ln;
			caret_placeCursorInCurrentFile(wp, ln, col);
			if (templateAction.ta_selectionDeltaCol || templateAction.ta_selectionDeltaLn) {
				bl_hideSelection(wp, 0);
				EdSyncSelectionWithCaret(MARK_START);
				col += templateAction.ta_selectionDeltaCol;
				ln += templateAction.ta_selectionDeltaLn;
				caret_placeCursorInCurrentFile(wp, ln, col);
				EdSyncSelectionWithCaret(MARK_END);
			}
		}
		ln_listfree(pasteBuffer.pln);
		ret = 1;
	}
	stringbuf_destroy(pSB);
	return ret;

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
		return macro_insertCodeTemplate(wp, up, FALSE);
	}
	render_repaintCurrentLine(wp); 
	return macro_executeByName(up->p.uc_macro);
}

/*--------------------------------------------------------------------------
 * macro_parseKeyDefinitions()
 */
static void macro_parseKeyDefinitions(FTABLE *fp) {	
	_keyfile = fp;
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
	
		string_concatPathAndFilename(protname, config_getPKSEditTempPath(),"key.err");
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
	FILE_SELECT_PARAMS params;
	params.fsp_saveAs = TRUE;
	params.fsp_encryptedAvailable = FALSE;

	if (!ft_getCurrentDocument() || (fn = fsel_selectFileWithOptions(&_linfsel, MADDDOCMAC, &params)) == 0) {
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

