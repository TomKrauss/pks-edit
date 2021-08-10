/*
 * EditOperations.c
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS 3.01
 *
 * purpose: inserting and deleting characters, words, lines ...
 * 		  autoformat, wrap and indent
 *
 * 										created: 04.02.90
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "trace.h"
#include "functab.h"
#include "caretmovement.h"
#include "winfo.h"
#include "grammar.h"
#include "brackets.h"
#include "edierror.h"
#include "pksedit.h"
#include "textblocks.h"
#include "editorconfiguration.h"
#include "mouseutil.h"
#include "stringutil.h"

#define	SWAP(a,b)			{	a ^= b, b ^=a, a ^= b;  }
#define	D_EBUG(x)		{/*error_showErrorById(x); render_repaintAllForFile(fp);*/}
#define	HARD_BREAK(lp)	(lp->len == 0)

/*--------------------------------------------------------------------------
 * EXTERNALS
 */

extern LINE *	cadv_gotoIdentifierEnd(LINE *lp,long *ln,long *col,int dir);
extern LINE *	cadv_c(LINE *lp,long *ln,long *col,int dir,unsigned char match);
extern void 	caret_setMatchFunction(int control, int ids_name, int *c);
extern LINE 	*(*advmatchfunc)();
extern int 	ln_countLeadingSpaces(LINE *l);
extern int 	string_isSpace(unsigned char c);
extern int 	string_countSpacesIn(unsigned char *s, int pos);
extern int 	sm_bracketindent(FTABLE *fp, LINE *lp1, LINE *lpcurr, 
				 int indent, int *di, int *hbr);
extern void 	wt_insline(FTABLE *fp, int caretLine, int nlines);
extern void 	ln_changeFlag(LINE *lpstart, LINE *lpend, int flagsearch, int flagmark,
				int set);
extern int 	macro_expandAbbreviation(WINFO *fp, LINE *lp,int offs);
extern void 	render_updateCaret(WINFO *wp);

extern long 	_multiplier;

/*--------------------------------------------------------------------------
 * EdCharDelete()
 * delete character(s), words, ....
 */
int EdCharDelete(int control);

/*--------------------------------------------------------------------------
 * edit_calculateColumns2TabsBlanks()
 * calculate the number of TABs and SPACEs we need, if we want to insert
 * a certain # of "blank" columns on start of the line
 * try using a maximum # of TABs
 */
static int edit_calculateColumns2TabsBlanks(INDENTATION *pIndentation, int col, int *add_blanks)
{
	int    i,ntabs;

	/* calculate # of tabstops up to given position */
	for (i = 0, ntabs = 0; 
	     *add_blanks = col - i, (i = indent_calculateNextTabStop(i, pIndentation)) <= col;
	     ntabs++)
	     ;

	return ntabs;
}

/*--------------------------------------------------------------------------
 * edit_calculateTabs2Columns()
 */
int edit_calculateTabs2Columns(INDENTATION* pIndentation, int tabs)
{	int col;
	
	for (col = 0; tabs > 0; col = indent_calculateNextTabStop(col, pIndentation))
		tabs--;
	return col;
}

/*--------------------------------------------------------------------------
 * edit_calculateStartIndentation()
 * return # of indents and # of additional blanks for the start of the line
 */
static int edit_calculateStartIndentation(WINFO* wp,LINE *lp, 
						  int upto, int *add_blanks) {
	int col;
	FTABLE* fp = wp->fp;

	col = string_countSpacesIn(lp->lbuf,upto);
	col = caret_lineOffset2screen(wp, &(CARET) { lp, col});
	return edit_calculateColumns2TabsBlanks(&wp->indentation,col,add_blanks);
}

/*--------------------------------------------------------------------------
 * ln_insertIndent()
 * Insert white space characters at the beginning of the passed line so
 * we fill up the space at the beginning of the line, so that the cursor
 * would be visible in the passed column. Return the number of characters
 * inserted parameter &inserted.
 */
LINE *ln_insertIndent(WINFO* wp, LINE *lp, int col, int *inserted)
{	int t,b,fillc;
	FTABLE* fp = wp->fp;

	if ((fillc = fp->documentDescriptor->fillc) == 0) {
		fillc = ' ';
		t = edit_calculateColumns2TabsBlanks(&wp->indentation,col,&b);
	} else {
		t = 0;
		b = col;
	}

	if ((*inserted = t+b) != 0) {
		if ((lp = ln_modify(fp,lp,0,*inserted)) != 0L) {
			memset(lp->lbuf,'\t', t);
			memset(lp->lbuf+t,fillc,b);
		}
	}

	return lp;
}

/*--------------------------------------------------------------------------
 * edit_insertIndent()
 * do autoindenting and bracket indenting, if required
 */
static int  _deltaindent;
static int edit_insertIndent(WINFO *wp, LINE *pPreviousLine, LINE *nlp, int caretColumn, int *newcol)
{	
	FTABLE* fp = wp->fp;
	int 			t = 0;
	int			b = 0;

	if (wp->workmode & WM_AUTOINDENT) {
		/* calculate indentation of line we leave */
		t = edit_calculateStartIndentation(wp,pPreviousLine,caretColumn,&b);
	}

	t += _deltaindent;
	_deltaindent = 0;

	caretColumn = edit_calculateTabs2Columns(&wp->indentation, t)+b;
	if ((nlp = ln_insertIndent(wp,nlp,caretColumn,newcol)) == 0) {
		return 0;
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * edit_handleBracketIndenting()
 */
static int edit_handleBracketIndenting(WINFO *wp, int dir, LINE *lp1, LINE *lp2)
{	int indent,b,di1,di2,i1,i2,hbr1,hbr2;

	if ((wp->workmode & WM_BRINDENT) == 0) {
		return FALSE;
	}
	FTABLE* fp = wp->fp;

	i1 = sm_bracketindent(fp,fp->firstl,lp1,0,&di1,&hbr1);
	if (i1 < 0) {
		return FALSE;
	}
	i2 = sm_bracketindent(fp,lp1,lp2,i1,&di2,&hbr2);

	i1 += di1;
	i2 += di2;

	/*
	 * insert line up
	 */
	if (dir < 0) {
		lp1 = lp2;
		SWAP(i1,i2);
		SWAP(di1,di2);
	}

	/* now: the line, where we are, is fp->currl, the line we left, lp1
      * correct the indent for the following cases:
	 * 	- current line, if indent level > 0, or containing bracket
	 *	- last line, if containing a bracket
	 */
	
	/* last line contains bracket */
	if (hbr1) {
		indent = edit_calculateStartIndentation(wp,lp1,lp1->len,&b);
		if (indent != i1) {
			/* first delete all blank characters, then shift */
			if ((lp1 = ln_modify(fp,lp1,string_countSpacesIn(lp1->lbuf,lp1->len),0)) == 0) {
				return 0;
			}
			uc_shiftLinesByIndent(wp,wp->caret.ln-(long)dir,1L,i1);
		}
	}

	/* current line inside bracket block, or has bracket, or last line
	 * was shifted
	 */
 
	if (i2 || hbr2 || (hbr1 && indent != i1)) {
		lp2 = wp->caret.linePointer;
		indent = edit_calculateStartIndentation(wp,lp2,lp2->len,&b);
		if (indent != i2) {
			uc_shiftLinesByIndent(wp,wp->caret.ln,1L,i2-indent);
		}
		caret_placeCursorInCurrentFile(wp, wp->caret.ln,i2);
	}

	return 1;	
}

/*--------------------------------------------------------------------------
 * edit_postProcessInsertLine()
 */
static int edit_postProcessInsertLine(WINFO *wp, int dir, long ln, long col)
{
	long  	omincol;
	long		ominln;
	LINE  *	lp;

	omincol = wp->mincol;
	ominln  = wp->minln;
	caret_placeCursorInCurrentFile(wp, ln,col);
	if (omincol != wp->mincol) {
		FTABLE* fp = wp->fp;
		render_repaintAllForFile(fp);
	} else {
		if (ominln == wp->minln) {
			wt_insline(wp->fp, wp->caret.ln, 1);
		}
		render_repaintCurrentLine(wp);
	}
	lp = wp->caret.linePointer;
	if (dir < 0) {
		lp = lp->next;
	}

	return edit_handleBracketIndenting(wp,dir,lp->prev,lp);
}

/*
 * Find out, whether a list of lines is completely commented out according to a comment specification. 
 */
static BOOL edit_isAllCommented(LINE* lpFirst, LINE* lpLast, COMMENT_DESCRIPTOR* pComment) {
	size_t nLen = strlen(pComment->comment_start);
	while (lpFirst) {
		char* pszFirst = string_skipBlanks(lpFirst->lbuf);
		if (!pszFirst || strncmp(pszFirst, pComment->comment_start, nLen) != 0) {
			return FALSE;
		}
		if (lpFirst == lpLast) {
			break;
		}
		lpFirst = lpFirst->next;
	}
	return TRUE;
}

/*
 * Comment in/out the selected lines in the current editor.
 */
int edit_toggleComment() {
	WINFO* wp = ww_getCurrentEditorWindow();
	COMMENT_DESCRIPTOR commentDescriptor;

	if (wp == 0L) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	if (!grammar_getCommentDescriptor(fp->documentDescriptor->grammar, &commentDescriptor)) {
		return 0;
	}
	LINE* lpFirst = wp->caret.linePointer;
	LINE* lpLast = wp->caret.linePointer;
	if (ww_hasSelection(wp)) {
		MARK* mps;
		MARK* mpe;
		if (find_setTextSelection(wp, RNG_BLOCK_LINES, &mps, &mpe) == RNG_INVALID) {
			return 0;
		}
		lpFirst = mps->lm;
		lpLast = mpe->lm;
	}
	// 3 states: -1 comment out, 1 comment in
	int addCommentFlag = edit_isAllCommented(lpFirst, lpLast, &commentDescriptor) ? -1 : 1;
	size_t nLen = strlen(commentDescriptor.comment_start);
	size_t nLen2 = strlen(commentDescriptor.comment_end);
	int nLines = ln_cnt(lpFirst, lpLast);
	while(nLines > 0) {
		char* pszFirst = string_skipBlanks(lpFirst->lbuf);
		size_t nOffset = pszFirst ? pszFirst - lpFirst->lbuf : lpFirst->len;
		if (addCommentFlag < 0) {
			nOffset -= (nLen * addCommentFlag);
		}
		size_t nOffset2 = nOffset + (nLen * addCommentFlag);
		if ((lpFirst = ln_modify(fp, lpFirst, (int)nOffset, (int)nOffset2)) == 0) {
			break;
		}
		if (addCommentFlag > 0) {
			strncpy(lpFirst->lbuf + nOffset, commentDescriptor.comment_start, nLen);
		}
		if (nLen2 > 0) {
			size_t nOffset = lpFirst->len;
			if (addCommentFlag < 0) {
				if ((lpFirst = ln_modify(fp, lpFirst, (int)nOffset, (int)(nOffset - nLen2))) == 0) {
					break;
				}
			}
			else {
				if ((lpFirst = ln_modify(fp, lpFirst, (int)nOffset, (int)(nOffset + nLen2))) == 0) {
					break;
				}
				strncpy(lpFirst->lbuf + nOffset, commentDescriptor.comment_end, nLen2);
			}
		}
		render_repaintLine(fp, lpFirst);
		lpFirst = lpFirst->next;
		nLines--;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * edit_insertLine()
 * create newline and insert it
 */
int edit_insertLine(int control)
{	LINE     *olp,*nlp;
	FTABLE 	*fp;
	int      dir,ai;
	long 	ln;
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp == 0L) {
		return 0;
	}
	fp = wp->fp;
	olp = wp->caret.linePointer;
	if ((nlp = ln_settmp(fp,(LINE *)0L,&olp)) == 0L) 
		return 0;
	nlp->len = 0;

	ln_insert(fp,((control) ? wp->caret.linePointer : wp->caret.linePointer->next),nlp);
	fp->tln = nlp;

	ln = wp->caret.ln;
	if (!control) {
		ln++;
		dir = 1;
	} else {
		dir = -1;
	}

	edit_insertIndent(wp,olp,nlp,(int )FBUFSIZE,&ai);

	return edit_postProcessInsertLine(wp,dir,ln,(long)ai);
}

/*--------------------------------------------------------------------------
 * EdLineDelete()
 * deleteline
 */
int EdLineDelete(control)
{	LINE		*clfirst,*clast;
	FTABLE	*fp;
	WINFO	*wp = ww_getCurrentEditorWindow();
	int 		lastlinelen;

	fp = wp->fp;
	if (control & 1) {
		caret_placeCursorInCurrentFile(wp, (long)(wp->caret.ln-1),0L);
	} else {
		caret_placeCursorInCurrentFile(wp, wp->caret.ln, 0L);
	}
	clfirst = wp->caret.linePointer;
	clast   = clfirst->next;
	if (!clast->next) {
		if (control & CUT_APPND) return 0;
		clast = clast->prev;
		if ((lastlinelen = clast->len) == 0L) return 0;
	} else lastlinelen = 0;
	if (control & CUT_APPND) {
		bl_append(_undobuf,clfirst,clast,0,0);
		ln_delete(fp,clfirst);
	} else {
		bl_undoIntoUnqBuffer(wp, clfirst, clast, (int) 0, (int) lastlinelen, (int) 0); 
	}

	if (lastlinelen) {
		render_repaintFromLineTo(fp,wp->caret.ln,wp->maxln);
	} else {
		wt_deleteline(fp,wp->caret.ln,0,1);
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * EdLinesJoin()
 * join two lines
 * be sure that cursor is in
 * first line to join
 */
int EdLinesJoin()
{
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE* fp = wp->fp;
	LINE *lp = wp->caret.linePointer;

	if (!ln_join(fp, lp, lp->next, 1)) {
		return 0;
	}
	wt_deleteline(fp, wp->caret.ln, 1,1);
	render_repaintCurrentLine(wp);
	return 1;
}

/*--------------------------------------------------------------------------
 * edit_breakline()
 * break one line in two pieces
 */
static int edit_breakline(WINFO* wp, int soft)
{	LINE   *nlp;
	int    ai;
	FTABLE* fp = wp->fp;

	if ((nlp = ln_break(fp,wp->caret.linePointer,wp->caret.offset)) == 0L)
		return 0;

	if (soft) {
		wp->caret.linePointer->lflg |= LNNOCR;
	} else {
		wp->caret.linePointer->lflg &= ~LNNOCR;
	}

	if (fp->documentDescriptor->nl >= 0) {
		wp->caret.linePointer->lflg &= ~LNNOTERM;
	}

	render_repaintCurrentLine(wp);
	edit_insertIndent(wp,wp->caret.linePointer,nlp,wp->caret.offset,&ai);

	return edit_postProcessInsertLine(wp,1,wp->caret.ln+1L,(long)ai);
}

/*--------------------------------------------------------------------------
 * ww_getRightMargin()
 * Returns the right margin as current configured the way it should be used for e.g. painting.
 */
int ww_getRightMargin(WINFO *wp) {	
	int 		rmargin;

	if ((rmargin = wp->rmargin) == 0) {
		rmargin = (wp->maxcol - wp->mincol) - RM_DELTA;
	}
	return rmargin;
}

/*--------------------------------------------------------------------------
 * edit_findWrappingPosition()
 */
static int edit_findWrappingPosition(WINFO* wp, LINE* lp, int cursoffset, int* nextword, int rmargin)
{
	int  	i;
	int		wm;
	int		wordpos;
	int		spacepos;
	char* s;

	wm = caret_screen2lineOffset(wp, &(CARET){lp, rmargin});

	if (caret_lineOffset2screen(wp, &(CARET) { lp, wm}) < rmargin) {
		return 0;
	}

	s = lp->lbuf;
	*nextword = 0;
	spacepos = 0;
	for (i = lp->len; i > 0; ) {
		i--;
		if (string_isSpace(s[i])) {
			wordpos = i + 1;
			do
				i--;
			while (i > 0 && (string_isSpace(s[i])));
			if (i > 0) {
				spacepos = i + 1;
				*nextword = wordpos;
			}
			if (i < wm) {
				/*
				 * seems to be a reasonable position -
				 * skip due to cursor is behind a word which
				 * user is editing, so wrapping would cut
				 * this word into 2 pieces
				 */
				if (i == cursoffset - 1)
					return 0;
				break;
			}
		}
	}
	return spacepos;
}

/*--------------------------------------------------------------------------
 * edit_wrapAround()
 */
static void edit_wrapAround(WINFO *wp) {
	int nextword,delta;

	if (edit_findWrappingPosition(wp, wp->caret.linePointer,wp->caret.offset,&nextword,ww_getRightMargin(wp)) > 0) {
		delta = wp->caret.offset-nextword;
		caret_placeCursorInCurrentFile(wp, wp->caret.ln,(long)nextword);
		EdLineSplit(0);
		if (delta < 0)
			delta += wp->caret.offset;
		if (delta < 0)
			delta = 0;
		caret_placeCursorInCurrentFile(wp, wp->caret.ln,(long)(wp->caret.offset+delta));
	}
}

/*--------------------------------------------------------------------------
 * edit_autoFormat()
 */
static int edit_autoFormat(WINFO *wp)
{	register unsigned char *destbuf,*sourcebuf;
	LINE *lp1,*lpnext,*lp;
	int  indent,col;
	int  numberOfLinesHasChanged,desti,di,rm,lpos,wend,wstart,modified,column;
	unsigned char c;
	long ln, caretLine, caretColumn;
	FTABLE *fp = wp->fp;

	if ((wp->workmode & WM_AUTOFORMAT) == 0) {
		return 0;
	}

	LINE* lpscratch = ln_create(MAXLINELEN);
	destbuf = lpscratch->lbuf;

	rm = ww_getRightMargin(wp);
	numberOfLinesHasChanged = 0;
	lp = wp->caret.linePointer;
	ln = caretLine = wp->caret.ln;
	caretColumn = wp->caret.offset;

	/* watch also previous lines */
	while (lp->prev && !HARD_BREAK(lp->prev)) {
		lp = lp->prev;
		ln--;
	}

	while (lp != 0) {

		if (edit_findWrappingPosition(wp, lp,-1,&wstart,rm) && wstart < lp->len) {
			/*
			 * if next line is empty, take same indent as current line
			 */
			lpnext = lp->next;
			if (lpnext == fp->lastl || HARD_BREAK(lpnext))
				lpnext = lp;
			indent = ln_countLeadingSpaces(lpnext);
			indent = caret_lineOffset2screen(wp, &(CARET) { lpnext, indent});

			/* start of next word after word position defined:
			   -> actual line too long 
			 */
			lp->lflg |= LNREPLACED;
			if ((lp = ln_break(fp,lp,wstart)) == 0L ||
			    (lp = ln_insertIndent(wp,lp,indent,&col)) == 0)
				break;
			lp->prev->lflg |= LNNOCR;

			/* Cursor Position affected ? */
			if (ln < caretLine || (ln == caretLine && caretColumn >= wstart)) {
				if (ln == caretLine && caretColumn >= wstart) {
					caretColumn = indent+caretColumn - wstart;
				}
				caretLine++;
			}
			ln++;

			lp->lflg |= LNREPLACED;
			numberOfLinesHasChanged++;
	D_EBUG("!WRAPPED");
			continue;
		} else {

			modified = 0;
			memmove(destbuf,lp->lbuf,lp->len);
			desti = lp->len;

	join_next_words:

			if (desti > 0 && !string_isSpace(lp->lbuf[desti-1])) {
				destbuf[desti++] = ' ';
			}
			if (HARD_BREAK(lp))
				break;
			lpnext = lp->next;
			if (lpnext == fp->lastl)
				break;

			/* find out how many words we may steal from next line */
			sourcebuf = lpnext->lbuf;
			lpos = indent = string_countSpacesIn(sourcebuf,lpnext->len);
			wend = 0;
			di = desti;
			do {
				/* scan next word */
				while(lpos < lpnext->len) {
					if (string_isSpace(c = sourcebuf[lpos++])) {
						lpos--;
						break;
					}
					destbuf[di++] = c;
				}
				column = caret_lineOffset2screen(wp, &(CARET){lpscratch, di});
				/* fits into current line within wrapmargin ? */
				if (column >= rm) {
					break;
				}
				wend = lpos;
				desti = di;

				/* skip spaces */
				while (lpos < lpnext->len) {
					if (!string_isSpace(c = sourcebuf[lpos++])) {
						lpos--;
						break;			
					}
					destbuf[di++] = c;
				}
				wstart = lpos;
			} while(lpos < lpnext->len);

			/* some words fit */
			if (wend > 0) {

				/* Cursor Position affected ? */
				if (ln + 1 < caretLine) {
					caretLine--;
				} else if (ln + 1 == caretLine) {
					if (caretColumn < wstart) {
						caretColumn += (lp->len-indent);
						caretLine--;
					} else {
						if (caretColumn > wstart) {
							caretColumn -= (wstart-indent);
						} else {
							caretColumn = indent;
						}
					}
				}

				if ((lp = ln_modify(fp,lp,lp->len,desti)) == 0) {
					break;
				}
				render_repaintLine(fp, lp);
				memmove(lp->lbuf,destbuf,desti);
				lp->lflg |= LNREPLACED;
				modified = 1;

				lpnext = lp->next;

				if (wstart >= lpnext->len) {
					/* the whole line was appended */
					lp->lflg = (lp->lflg & ~(LNNOTERM|LNNOCR))|
							 (lpnext->lflg & (LNNOTERM|LNNOCR));
					if (!ln_delete(fp, lpnext)) {
						break;
					}
					numberOfLinesHasChanged++;
	D_EBUG("!DELETED");

					/* look for more */
					goto join_next_words;
				} else {
					if ((lpnext = ln_modify(fp, lpnext, wstart, indent)) == 0) {
						break;
					}
					lpnext->lflg |= LNREPLACED;
					lp = lpnext->prev;
	D_EBUG("!JOINED WORDS");
				}
			}
		}

		lp = lp->next;
		ln++;
	}

	if (numberOfLinesHasChanged) {
		render_repaintAllForFile(fp);
	}

	lp1 = wp->caret.linePointer;
	if (lp1->prev)
		lp1 = lp1->prev;

	ln_removeFlag(lp1,lp,LNREPLACED);

	if (caretLine >= fp->nlines) {
		caretLine = fp->nlines-1;
		caretColumn = lp1->len;
	}

	caret_placeCursorInCurrentFile(wp, caretLine,caretColumn);
	free(lpscratch);
	return 1;
}

/*--------------------------------------------------------------------------
 * EdCharInsert()
 * normal Character insert
 */
int EdCharInsert(int c)
{	FTABLE *		fp;
	EDIT_CONFIGURATION *		lnp;
	LINE *		lp;
	int			len;
	int			offs;
	int			nchars;
	int			workmode;
	extern int 	_playing;
	WINFO* wp = ww_getCurrentEditorWindow();

	fp = wp->fp;
	lnp = fp->documentDescriptor;
	if (c < 32 && c != 8 && c != '\t' && c != 10 && c != lnp->nl && c != lnp->cr) {
		return 0;
	}

	if (_playing != 2) {
		macro_recordFunction(fp, c);
	}

	if (c == lnp->nl || c == lnp->cr || (c == 10 && lnp->nl < 0)) {
		return EdLineSplit(c == lnp->nl ? RET_SOFT : 0);
	}

	if ((GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) && ww_hasSelection(wp)) {
		EdBlockDelete(0);
		if (c == 8 || c == 127) {
			return 1;
		}
	}

	if (c == 8)
		return EdCharDelete(-1);

	if (c == 127)
		return EdCharDelete(1);

	workmode = wp->workmode;

	if (c & 0x100) {
		c &= 0xFF;
	} else if (workmode & WM_OEMMODE) {
		char *string = "A",string2[10];
		*string = c;
		AnsiToOemBuff(string,string2,1);
		c = *string2;
	}

	offs = wp->caret.offset;

	if (c == '\t' && lnp->fillc) {
		int n2 = caret_lineOffset2screen(wp, &wp->caret);
		int n = indent_calculateNextTabStop(n2, &wp->indentation);
		nchars = n - n2;
		c = lnp->fillc;
	} else {
		nchars = 1;

	}

	if (workmode & WM_INSERT || offs >= wp->caret.linePointer->len) {
		len = nchars;
	} else {
		len = 0;
	}

	if ((lp = ln_modify(fp,wp->caret.linePointer,offs,offs+len)) == 0L) return 0;
	memset(&lp->lbuf[offs],c,nchars);

#if 0
	render_repaintLinePart(offs,10000);
#else
	render_repaintCurrentLine(wp);
#endif

	offs += nchars;
	caret_placeCursorInCurrentFile(wp, wp->caret.ln,(long)offs);

	if (!_playing) {

		if (workmode & WM_ABBREV) {
			macro_expandAbbreviation(wp,lp,offs);
		}
		if (workmode & WM_SHOWMATCH) {
			uc_showMatchingBracket(wp);
		}

		if (!edit_autoFormat(wp) &&
		    (workmode & WM_AUTOWRAP) && 
			wp->caret.offset >= ww_getRightMargin(wp)) {
			edit_wrapAround(wp);
		}
	}
	return 1;
}

/**
 * Return the next column up to which a single character DEL should delete.
 */
static int edit_findNextOffsetForDeletion(WINFO* wp, LINE* lp, int nOffset) {
	if (wp->workmode & WM_DELETE_MULTIPLE_SPACES) {
		FTABLE* fp = wp->fp;
		CARET caret;
		int currentScreenCol = caret_lineOffset2screen(wp, &wp->caret);
		int screenColNextTab = indent_calculateNextTabStop(currentScreenCol, &wp->indentation);
		caret.linePointer = lp;
		caret.offset = screenColNextTab;
		int lineOffsetNextTab = caret_screen2lineOffset(wp, &caret);
		while (++nOffset < lineOffsetNextTab) {
			if (lp->lbuf[nOffset] != fp->documentDescriptor->fillc) {
				return nOffset;
			}
		}
		return lineOffsetNextTab;
	}
	return nOffset + 1;
}

static BOOL edit_isSpace(LINE* lp, EDIT_CONFIGURATION* pDescriptor, int nOffset) {
	char c = lp->lbuf[nOffset];
	if (c != '\t' && c != pDescriptor->fillc) {
		return FALSE;
	}
	return TRUE;
}

/**
 * Return the previous column up to which a single character BACKSPACE should delete.
 */
static int edit_findPreviousOffsetForDeletion(WINFO* wp, LINE* lp, int nOffset) {
	if (wp->workmode & WM_DELETE_MULTIPLE_SPACES) {
		FTABLE* fp = wp->fp;
		CARET caret;
		int currentScreenCol = caret_lineOffset2screen(wp, &wp->caret);
		int screenColPreviousTab = indent_calculatePreviousTabStop(currentScreenCol, &wp->indentation);
		caret.linePointer = lp;
		caret.offset = screenColPreviousTab;
		int lineOffsetPreviousTab = caret_screen2lineOffset(wp, &caret);
		int nNewOffset = nOffset;
		while (--nNewOffset > lineOffsetPreviousTab) {
			if (!edit_isSpace(lp, fp->documentDescriptor, nNewOffset)) {
				break;
			}
		}
		int nDelta = nOffset - nNewOffset;
		if (nDelta > 1) {
			if (nNewOffset < 0) {
				nNewOffset = 0;
			} else if (!edit_isSpace(lp, fp->documentDescriptor, nNewOffset)) {
				nNewOffset++;
				if (nDelta > 2) {
					nNewOffset++;
				}
			} else if (nNewOffset > 0 && !edit_isSpace(lp, fp->documentDescriptor, nNewOffset-1)) {
				nNewOffset++;
			}
		}
		return nNewOffset;
	}
	return nOffset - 1;
}

/*--------------------------------------------------------------------------
 * EdCharDelete()
 * delete character(s), words, ....
 */
int EdCharDelete(int control)
{	register	LINE *lp,*lp1=0;
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE 	*fp = wp->fp;
	long		ln,ln1,o2,o1;
	int		matchc;

	render_updateCaret(wp);
	lp = lp1 = wp->caret.linePointer;
	o1 = o2  = wp->caret.offset;
	ln = ln1 = wp->caret.ln;

	caret_setMatchFunction(control,IDS_DELUNTILC,&matchc);

	if (control > 0 && o1 == lp->len) {
		if (control == MOT_TOEND)
			return 1;
		return EdLinesJoin();
	}
	else if (control < 0 && o1 == 0) {
		if (control == -MOT_TOEND)
			return 1;
		if ((lp = lp->prev) == 0L) 
			return 0;
		caret_placeCursorInCurrentFile(wp, ln-1L,(long) lp->len);
		return EdLinesJoin();
	} else switch (control) {
		case  MOT_SINGLE: o2 = edit_findNextOffsetForDeletion(wp, lp, o1); break;
		case  MOT_TOEND:  o2 = lp->len;
			break;
		case -MOT_SINGLE: o1 = edit_findPreviousOffsetForDeletion(wp, lp,o1); break;
		case -MOT_TOEND:  o1=0; break;
		case -MOT_WORD: case -MOT_UNTILC: case -MOT_SPACE:
			if ((lp1 = (*advmatchfunc)(lp,&ln,&o1,-1,matchc)) == 0) 
				return 0;
			break;
		case MOT_WORD:  case MOT_UNTILC:  case MOT_SPACE:
			if ((lp = (*advmatchfunc)(lp,&ln,&o2,1,matchc)) == 0) 
				return 0;
			break;
	}
	if (control == MOT_SINGLE || control == -MOT_SINGLE) {
		if (!ln_modify(fp,lp,o2,o1))
			return 0;
	} else {
		if (o1 == o2) {
			return 0;
		}
		if (!bl_undoIntoUnqBuffer(wp, lp1, lp, (int) o1, (int) o2, (int) 0)) {
			return 0;
		}
	}

	if (control < 0)
		caret_placeCursorInCurrentFile(wp, ln,o1);

	if (ln != ln1) {
		render_repaintAllForFile(fp);
	} else {
		render_repaintCurrentLine(wp);
	}
	edit_autoFormat(wp);

	return 1;
}

/*--------------------------------------------------------------------------
 * EdLineSplit()
 * do cr+lf-Actions
 */
int EdLineSplit(int flags)
{
	int		control;
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE *	fp;
	LINE	 *	lp;

	fp = wp->fp;
	lp = wp->caret.linePointer;
	control = flags & (RET_PLUS|RET_MINUS);

	if (wp->workmode & WM_INSERT) {
		if (control & RET_PLUS) {
			_deltaindent = 1;
		} else if (control & RET_MINUS) {
			_deltaindent = -1;
		}
		edit_breakline(wp, flags & RET_SOFT);
	}
	else if (lp->next != fp->lastl) {
		caret_placeCursorInCurrentFile(wp, wp->caret.ln+1,0L);
	}
	else if (!control) {
		edit_insertLine(0);
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * EdMarkedLineOp()
 */
int EdMarkedLineOp(int op)
{
	long 	ln;
	FTABLE *	fp;
	LINE *	lp;
	LINE	*	last;
	WINFO* wp = ww_getCurrentEditorWindow();

	fp = wp->fp;
	last = fp->lastl->prev;
	lp = fp->firstl;
	switch(op) {

	case MLN_TOGGLE:
		while(1) {
			lp->lflg ^= LNXMARKED;
			if (lp == last)
				break;
			lp = lp->next;
		}
		break;

	case MLN_MAKESOFT:
		ln_changeFlag(lp,last,LNXMARKED,LNNOCR,1);
		break;

	case MLN_MAKEHARD:
		ln_changeFlag(lp,last,LNXMARKED,LNNOCR,0);
		break;

	case MLN_FINDSOFT:
		ln_changeFlag(lp,last,LNNOCR,LNXMARKED,1);
		break;

	default:
		for (ln = 0L; lp != 0 && !(lp->lflg & LNXMARKED);
			lp = lp->next, ln++);
		if (!lp)
			return 1;
		caret_placeCursorAndSavePosition(wp, ln,0L);
		mouse_setBusyCursor();
		if (op == MLN_JOIN)
			lnjoin_lines(fp);	
		else
			ft_cutMarkedLines(fp, op);
	}

	render_repaintAllForFile(fp);
	mouse_setDefaultCursor();
	return 1;
}

/*--------------------------------------------------------------------------
 * EdHideLines()
 */
int EdHideLines(void)
{	
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE *	fp;
	LINE *	lp1;
	LINE *	lp2;

	if (wp == 0 ||
		(fp = wp->fp) == 0 ||
		!ww_hasSelection(wp)) {
		return 0;
	}

	lp1 = wp->blstart->lm;
	lp2 = wp->blend->lm;
	bl_hideSelection(wp, 0);

	ln_hide(fp, lp1, lp2);
	caret_moveToLine(wp, ln_cnt(fp->firstl, wp->caret.linePointer) - 1);

	render_repaintAllForFile(fp);
	return 1;
}

/*--------------------------------------------------------------------------
 * EdUnHideLine()
 */
int EdUnHideLine(void)
{
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE *	fp;

	if (wp == 0) {
		return 0;
	}
	fp = wp->fp;
	if (ln_unhide(fp, wp->caret.linePointer)) {
		caret_placeCursorInCurrentFile(wp, wp->caret.ln,0L);
		render_repaintAllForFile(fp);
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * EdMouseSelectLines()
 */
void EdMouseSelectLines(int flg)
{	
	WINFO* wp = ww_getCurrentEditorWindow();
	LINE *lp = wp->caret.linePointer;
	int  oflg;

	oflg = lp->lflg;
	if (flg)
		lp->lflg |= LNXMARKED;
	else
		lp->lflg &= ~LNXMARKED;
	if (oflg != lp->lflg)
		render_repaintCurrentLine(wp);
}

/*--------------------------------------------------------------------------
 * EdExpandAbbreviation()
 */
int EdExpandAbbreviation(void)
{
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp) {
		return
			macro_expandAbbreviation(wp->fp,wp->caret.linePointer,wp->caret.offset);
	}
	return 0;
}

