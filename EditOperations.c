/*
 * EditOperations.c
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS
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
#include "funcdef.h"
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
#include "editoperations.h"
#include "publicapi.h"
#include "codecompletion.h"
#include "formatting.h"

#define	SWAP(a,b)			{	a ^= b, b ^=a, a ^= b;  }
#define	D_EBUG(x)		{/*error_showErrorById(x); render_repaintAllForFile(fp);*/}
#define	HARD_BREAK(lp)	(lp->len == 0)

/*--------------------------------------------------------------------------
 * EXTERNALS
 */

extern LINE *	caret_gotoIdentifierEnd(LINE *lp,long *ln,long *col,int dir);
extern LINE *	caret_advanceCharacter(LINE *lp,long *ln,long *col,int dir,unsigned char match);
extern void 	caret_setMatchFunction(int control, int ids_name, int *c);
extern LINE 	*(*advmatchfunc)();
extern int 	ln_countLeadingSpaces(LINE *l);
extern int 	string_isSpace(unsigned char c);
extern int 	string_countSpacesIn(unsigned char *s, int pos);
extern void 	wt_insline(FTABLE *fp, int caretLine, int nlines);
extern int 	template_expandAbbreviation(WINFO *fp, LINE *lp,int offs);
extern void 	render_updateCaret(WINFO *wp);

extern long 	_multiplier;

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
 * ln_insertIndent()
 * Insert white space characters at the beginning of the passed line so
 * we fill up the space at the beginning of the line, so that the cursor
 * would be visible in the passed column. Return the number of characters
 * inserted parameter &inserted.
 */
LINE *ln_insertIndent(WINFO* wp, LINE *lp, int col, int *inserted)
{	int t,b,expandTabsWith;
	FTABLE* fp = wp->fp;

	if ((expandTabsWith = fp->documentDescriptor->expandTabsWith) == 0) {
		expandTabsWith = ' ';
		t = edit_calculateColumns2TabsBlanks(&wp->indentation,col,&b);
	} else {
		t = 0;
		b = col;
	}

	if ((*inserted = t+b) > 0) {
		if ((lp = ln_modify(fp,lp,0,*inserted)) != 0L) {
			memset(lp->lbuf,'\t', t);
			memset(lp->lbuf+t,expandTabsWith,b);
		}
	}

	return lp;
}

/*--------------------------------------------------------------------------
 * edit_insertIndent()
 * do autoindenting and bracket indenting, if required
 */
int _deltaindent;
static int edit_insertIndent(WINFO *wp, LINE *pPreviousLine, LINE *nlp, int caretColumn, int *newcol) {
	if (wp->workmode & WM_AUTOINDENT) {
		// calculate indentation of line we leave
		caretColumn = format_calculateScreenIndentWithSyntax(wp, pPreviousLine);
	}
	
	caretColumn += _deltaindent * wp->indentation.tabsize;
	_deltaindent = 0;
	if (ln_insertIndent(wp,nlp,caretColumn,newcol) == 0) {
		return 0;
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * edit_handleBracketIndenting()
 */
static int edit_handleBracketIndenting(WINFO *wp, int dir, LINE *lpPrev, LINE *lpCurrent) {
	FTABLE* fp = wp->fp;

	if (!lpPrev || !lpPrev->prev) {
		return FALSE;
	}
	INDENTATION_DELTA tDelta = format_calculateIndentationDelta(wp, lpPrev);
	if (tDelta != ID_OUTDENT_THIS) {
		return TRUE;
	}
	int prevIndent = format_calculateScreenIndent(wp, lpPrev->prev);
	tDelta = format_calculateIndentationDelta(wp, lpPrev->prev);
	int currentIndent = format_calculateScreenIndent(wp, lpPrev);
	int supposedIndent = tDelta == ID_INDENT_NEXT ? prevIndent : prevIndent - wp->indentation.tabsize;
	if (supposedIndent < currentIndent) {
		int o2 = caret_screen2lineOffset(wp, &(CARET) {
			.linePointer = lpPrev,
			.offset = currentIndent
		});
		int o1 = caret_screen2lineOffset(wp, &(CARET) {
			.linePointer = lpPrev,
			.offset = supposedIndent
		});
		if (ln_modify(fp, lpPrev, o2, o1)) {
			render_repaintLine(fp, lpPrev);
		}
	}
	return 1;	
}

/*--------------------------------------------------------------------------
 * edit_postProcessInsertLine()
 */
static int edit_postProcessInsertLine(WINFO *wp, int dir, long ln, long col)
{
	long  	omincol;
	long	ominln;
	LINE*	lp;

	omincol = wp->mincol;
	ominln  = wp->minln;
	caret_placeCursorInCurrentFile(wp, ln,col);
	if (omincol != wp->mincol || ominln != wp->minln) {
		FTABLE* fp = wp->fp;
		render_repaintAllForFile(fp);
	} else {
		wt_insline(wp->fp, wp->caret.ln, 1);
		render_repaintCurrentLine(wp);
	}
	lp = wp->caret.linePointer;
	if (dir < 0) {
		lp = lp->next;
	}

	if ((wp->workmode & WM_AUTOINDENT) == 0) {
		return TRUE;
	}
	return edit_handleBracketIndenting(wp,dir,lp->prev,lp);
}

/*
 * Cursor placement also handling multi-cursors.
 */
static int edit_placeCursor(WINFO* wp, CARET* pCaret, long nLine, long nOffset) {
	if (pCaret == &wp->caret) {
		return caret_placeCursorInCurrentFile(wp, nLine, nOffset);
	}
	pCaret->offset = nOffset;
	if (pCaret->ln != nLine) {
		pCaret->linePointer = ln_relative(pCaret->linePointer, nLine - pCaret->ln);
		pCaret->ln = nLine;
	}
	return 1;
}

/*
 * Find out, whether a list of lines is completely commented out according to a comment specification. 
 */
static BOOL edit_isAllCommented(LINE* lpFirst, const LINE* lpLast, const char* pszCommentStart) {
	if (!pszCommentStart) {
		return FALSE;
	}
	size_t nLen = strlen(pszCommentStart);
	while (lpFirst) {
		char* pszFirst = string_skipBlanks(lpFirst->lbuf);
		if (!pszFirst || strncmp(pszFirst, pszCommentStart, nLen) != 0) {
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
long long edit_toggleComment(WINFO* wp) {
	COMMENT_DESCRIPTOR commentDescriptor;

	if (wp == 0L) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	if (!grammar_getCommentDescriptor(fp->documentDescriptor->grammar, &commentDescriptor)) {
		return 0;
	}
	const char* pszCommentStart = commentDescriptor.comment_single;
	if (!pszCommentStart) {
		pszCommentStart = commentDescriptor.comment_start;
	}
	if (!pszCommentStart) {
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
		lpFirst = mps->m_linePointer;
		lpLast = mpe->m_linePointer;
	}
	// 3 states: -1 comment out, 1 comment in
	int addCommentFlag = edit_isAllCommented(lpFirst, lpLast, pszCommentStart) ? -1 : 1;
	long nLen = (long)strlen(pszCommentStart);
	size_t nLen2 = commentDescriptor.comment_end == 0 ? 0 : strlen(commentDescriptor.comment_end);
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
			strncpy(lpFirst->lbuf + nOffset, pszCommentStart, nLen);
		}
		if (nLen2 > 0) {
			size_t nOffset = lpFirst->len;
			if (addCommentFlag < 0) {
				if ((lpFirst = ln_modify(fp, lpFirst, (int)nOffset, (int)(nOffset - nLen2))) == 0) {
					break;
				}
			} else {
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
long long edit_insertLine(EDIT_LINE_OPTIONS control)
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

	ln_insert(fp,((control == ELO_CARET_FOLLOWS) ? wp->caret.linePointer : wp->caret.linePointer->next),nlp);
	fp->tln = nlp;

	ln = wp->caret.ln;
	if (!control) {
		ln++;
		dir = 1;
	} else {
		dir = -1;
	}

	if (!edit_insertIndent(wp, olp, nlp, (int)FBUFSIZE, &ai)) {
		return FALSE;
	}

	return edit_postProcessInsertLine(wp,dir,ln,(long)ai);
}

/*--------------------------------------------------------------------------
 * EdLineDelete()
 * deleteline
 */
int EdLineDelete(EDIT_LINE_OPTIONS control)
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
		if (control & ELO_APPEND) return 0;
		clast = clast->prev;
		if ((lastlinelen = clast->len) == 0L) return 0;
	} else lastlinelen = 0;
	//bl_append(_undobuf,clfirst,clast,0,0);
	ln_delete(fp,clfirst);

	if (lastlinelen) {
		render_repaintFromLineTo(fp,wp->caret.ln,wp->maxln);
	} else {
		wt_deleteline(fp,wp->caret.ln,0,1);
	}

	return 1;
}

/* 
 * Join two lines for the given window given a caret.
 */
static int edit_joinLines(WINFO* wp, CARET* pCaret) {
	FTABLE* fp = wp->fp;
	LINE* lp = pCaret->linePointer;

	if (!ln_join(fp, lp, lp->next, 1)) {
		return 0;
	}
	wt_deleteline(fp, wp->caret.ln, 1, 1);
	render_repaintCurrentLine(wp);
	return 1;

}

/*--------------------------------------------------------------------------
 * EdLinesJoin()
 * join two lines
 * be sure that cursor is in
 * first line to join
 */
int EdLinesJoin() {
	WINFO* wp = ww_getCurrentEditorWindow();
	return edit_joinLines(wp, &wp -> caret);
}

/*--------------------------------------------------------------------------
 * edit_breakline()
 * break one line in two pieces
 */
static int edit_breakline(WINFO* wp, CARET* pCaret, int soft)
{	LINE   *nlp;
	int    ai;
	FTABLE* fp = wp->fp;

	if ((nlp = ln_break(fp,pCaret->linePointer, pCaret->offset)) == 0L)
		return 0;

	if (soft) {
		pCaret->linePointer->lflg |= LNNOCR;
	} else {
		pCaret->linePointer->lflg &= ~LNNOCR;
	}

	if (fp->documentDescriptor->nl >= 0) {
		pCaret->linePointer->lflg &= ~LNNOTERM;
	}

	render_repaintCurrentLine(wp);
	edit_insertIndent(wp, pCaret->linePointer,nlp, pCaret->offset,&ai);

	return edit_postProcessInsertLine(wp,1, pCaret->ln+1L,(long)ai);
}

/*
 * Split one line at the given caret position.
 */
static int edit_splitLine(WINFO* wp, CARET* pCaret, int flags) {
	int control;
	FTABLE* fp;
	LINE* lp;

	lp = pCaret->linePointer;
	fp = wp->fp;
	control = flags & (RET_PLUS | RET_MINUS);

	if (wp->workmode & WM_INSERT) {
		if (control & RET_PLUS) {
			_deltaindent = 1;
		}
		else if (control & RET_MINUS) {
			_deltaindent = -1;
		}
		edit_breakline(wp, pCaret, flags & RET_SOFT);
	}
	else if (lp->next != fp->lastl) {
		edit_placeCursor(wp, pCaret, pCaret->ln + 1, 0L);
	}
	else if (!control) {
		edit_insertLine(0);
	}
	return 1;
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

	wm = caret_screen2lineOffset(wp, &(CARET){.linePointer = lp, .offset = rmargin});

	if (caret_lineOffset2screen(wp, &(CARET) {.linePointer = lp, .offset = wm}) < rmargin) {
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
static void edit_wrapAround(WINFO *wp, CARET* pCaret) {
	int nextword,delta;

	if (edit_findWrappingPosition(wp, pCaret->linePointer, pCaret->offset,&nextword,ww_getRightMargin(wp)) > 0) {
		delta = wp->caret.offset-nextword;
		edit_placeCursor(wp, pCaret, pCaret->ln,(long)nextword);
		edit_splitLine(wp, pCaret, 0);
		if (delta < 0)
			delta += wp->caret.offset;
		if (delta < 0)
			delta = 0;
		edit_placeCursor(wp, pCaret, pCaret->ln,(long)(pCaret->offset+delta));
	}
}

/*
 * Format a range of text so it fits into the given window bounds (rmargin). 
 * TODO: should use ft_formatText internally for consistent formatting.
 */
static int edit_autoFormatRange(WINFO* wp) {
	register unsigned char* destbuf, * sourcebuf;
	LINE* lp1, * lpnext, * lp;
	LINE* lplast;
	int  indent, col;
	int  numberOfLinesHasChanged, desti, di, rm, lpos, wend, wstart, modified, column;
	unsigned char c;
	long ln, caretLine, caretColumn;
	FTABLE* fp = wp->fp;

	LINE* lpscratch = ln_create(MAXLINELEN);
	destbuf = lpscratch->lbuf;

	rm = ww_getRightMargin(wp);
	numberOfLinesHasChanged = 0;

	caretColumn = 0;
	lplast = fp->lastl;
	ln = caretLine = wp->caret.ln;
	/* watch also previous lines */
	BOOL bStopAfterParagraph = FALSE;
	lp = wp->caret.linePointer;
	caretColumn = wp->caret.offset;
	while (lp->prev && !HARD_BREAK(lp->prev)) {
		lp = lp->prev;
		ln--;
	}
	bStopAfterParagraph = TRUE;

	while (lp != 0) {
		if (edit_findWrappingPosition(wp, lp, -1, &wstart, rm) && wstart < lp->len) {
			/*
			 * if next line is empty, take same indent as current line
			 */
			lpnext = lp->next;
			if (lpnext == fp->lastl || HARD_BREAK(lpnext))
				lpnext = lp;
			indent = ln_countLeadingSpaces(lpnext);
			indent = caret_lineOffset2screen(wp, &(CARET) {.linePointer = lpnext, .offset = indent});

			/* start of next word after word position defined:
			   -> actual line too long
			 */
			ln_markModified(lp);
			if ((lp = ln_break(fp, lp, wstart)) == 0L ||
				(lp = ln_insertIndent(wp, lp, indent, &col)) == 0)
				break;
			lp->prev->lflg |= LNNOCR;

			/* Cursor Position affected ? */
			if (ln < caretLine || (ln == caretLine && caretColumn >= wstart)) {
				if (ln == caretLine && caretColumn >= wstart) {
					caretColumn = indent + caretColumn - wstart;
				}
				caretLine++;
			}
			ln++;

			ln_markModified(lp);
			numberOfLinesHasChanged++;
			D_EBUG("!WRAPPED");
			continue;
		} else {

			modified = 0;
			memmove(destbuf, lp->lbuf, lp->len);
			desti = lp->len;

join_next_words:
			if (desti > 0 && !string_isSpace(lp->lbuf[desti - 1])) {
				destbuf[desti++] = ' ';
			}
			if (bStopAfterParagraph && HARD_BREAK(lp))
				break;
			if (lp == lplast) {
				break;
			}
			lpnext = lp->next;
			if (lpnext->len == 0 && !bStopAfterParagraph) {
				if (lpnext == lplast) {
					break;
				}
				lpnext = lpnext->next;
				if (lpnext == lplast) {
					break;
				}
			}

			/* find out how many words we may steal from next line */
			sourcebuf = lpnext->lbuf;
			lpos = indent = string_countSpacesIn(sourcebuf, lpnext->len);
			wend = 0;
			di = desti;
			do {
				/* scan next word */
				while (lpos < lpnext->len) {
					if (string_isSpace(c = sourcebuf[lpos++])) {
						lpos--;
						break;
					}
					destbuf[di++] = c;
				}
				column = caret_lineOffset2screen(wp, &(CARET){.linePointer= lpscratch, .offset = di});
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
			} while (lpos < lpnext->len);

			/* some words fit */
			if (wend > 0) {

				/* Cursor Position affected ? */
				if (ln + 1 < caretLine) {
					caretLine--;
				}
				else if (ln + 1 == caretLine) {
					if (caretColumn < wstart) {
						caretColumn += (lp->len - indent);
						caretLine--;
					}
					else {
						if (caretColumn > wstart) {
							caretColumn -= (wstart - indent);
						}
						else {
							caretColumn = indent;
						}
					}
				}

				if ((lp = ln_modify(fp, lp, lp->len, desti)) == 0) {
					break;
				}
				render_repaintLine(fp, lp);
				memmove(lp->lbuf, destbuf, desti);
				ln_markModified(lp);
				modified = 1;

				lpnext = lp->next;

				if (wstart >= lpnext->len) {
					/* the whole line was appended */
					lp->lflg = (lp->lflg & ~(LNNOTERM | LNNOCR)) |
						(lpnext->lflg & (LNNOTERM | LNNOCR));
					if (!ln_delete(fp, lpnext)) {
						break;
					}
					numberOfLinesHasChanged++;
					D_EBUG("!DELETED");

					/* look for more */
					goto join_next_words;
				}
				else {
					if ((lpnext = ln_modify(fp, lpnext, wstart, indent)) == 0) {
						break;
					}
					ln_markModified(lp);
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

	ln_removeFlag(lp1, lp, LNMODIFIED);

	long nlines = wp->renderer->r_calculateMaxLine(wp);
	if (caretLine >= nlines) {
		caretLine = nlines - 1;
		caretColumn = lp1->len;
	}

	caret_placeCursorInCurrentFile(wp, caretLine, caretColumn);
	free(lpscratch);
	return 1;
}

/*--------------------------------------------------------------------------
 * edit_autoFormat()
 */
static int edit_autoFormat(WINFO *wp) {
	if ((wp->workmode & WM_AUTOFORMAT) == 0) {
		return 0;
	}
	return edit_autoFormatRange(wp);
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
			if (lp->lbuf[nOffset] != fp->documentDescriptor->expandTabsWith) {
				return nOffset;
			}
		}
		return lineOffsetNextTab;
	}
	return nOffset + 1;
}

static BOOL edit_isSpace(LINE* lp, EDIT_CONFIGURATION* pDescriptor, int nOffset) {
	char c = lp->lbuf[nOffset];
	if (c != '\t' && c != pDescriptor->expandTabsWith) {
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
			}
			else if (!edit_isSpace(lp, fp->documentDescriptor, nNewOffset)) {
				nNewOffset++;
				if (nDelta > 2) {
					nNewOffset++;
				}
			}
			else if (nNewOffset > 0 && !edit_isSpace(lp, fp->documentDescriptor, nNewOffset - 1)) {
				nNewOffset++;
			}
		}
		return nNewOffset;
	}
	return nOffset - 1;
}

/*
 * Delete one character at the given caret position, assuming a control type and an optional match character.
 */
static int edit_deleteChar(WINFO* wp, CARET* pCaret, int control, int nMatchChar) {
	register	LINE* lp, * lp1 = 0;
	FTABLE* fp = wp->fp;
	long		ln, ln1, o2, o1;

	lp = lp1 = pCaret->linePointer;
	o1 = o2 = pCaret->offset;
	ln = ln1 = pCaret->ln;


	if (control > 0 && o1 == lp->len) {
		if (control == MOT_TOEND)
			return 1;
		return edit_joinLines(wp, pCaret);
	}
	else if (control < 0 && o1 == 0) {
		if (control == -MOT_TOEND)
			return 1;
		if ((lp = lp->prev) == 0L)
			return 0;
		edit_placeCursor(wp, pCaret, ln - 1L, (long)lp->len);
		return edit_joinLines(wp, pCaret);
	}
	else switch (control) {
	case  MOT_SINGLE: o2 = edit_findNextOffsetForDeletion(wp, lp, o1); break;
	case  MOT_TOEND:  o2 = lp->len;
		break;
	case -MOT_SINGLE: o1 = edit_findPreviousOffsetForDeletion(wp, lp, o1); break;
	case -MOT_TOEND:  o1 = 0; break;
	case -MOT_WORD: case -MOT_UNTILC: case -MOT_SPACE:
		if ((lp1 = (*advmatchfunc)(lp, &ln, &o1, -1, nMatchChar)) == 0)
			return 0;
		break;
	case MOT_WORD:  case MOT_UNTILC:  case MOT_SPACE:
		if ((lp = (*advmatchfunc)(lp, &ln, &o2, 1, nMatchChar)) == 0)
			return 0;
		break;
	}
	if (control == MOT_SINGLE || control == -MOT_SINGLE) {
		if (!ln_modify(fp, lp, o2, o1))
			return 0;
	}
	else {
		if (o1 == o2) {
			return 0;
		}
		if (!bl_delete(wp, lp1, lp, (int)o1, (int)o2, (int)0, 0)) {
			return 0;
		}
	}

	if (control < 0)
		edit_placeCursor(wp, pCaret, ln, o1);

	if (ln != ln1) {
		render_repaintAllForFile(fp);
	} else {
		render_repaintLine(fp, pCaret->linePointer);
	}
	edit_autoFormat(wp);
	codecomplete_updateCompletionList(wp, FALSE);

	return 1;
}

/*--------------------------------------------------------------------------
 * EdCharInsert()
 * normal Character insert
 */
long long EdCharInsert(int c)
{	FTABLE *		fp;
	EDIT_CONFIGURATION *lnp;
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
		recorder_recordAutoInsertFunction(fp, c);
	}

	if ((GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) && ww_hasSelection(wp)) {
		EdBlockDelete(0);
		if (c == 8 || c == 127) {
			return 1;
		}
	}

	CARET* pCaret = &wp->caret;
	workmode = wp->workmode;
	BOOL bNormalChar = FALSE;
	while (pCaret) {
		int nRet;
		if (c == lnp->nl || c == lnp->cr || (c == 10 && lnp->nl < 0)) {
			nRet = edit_splitLine(wp, pCaret, c == lnp->nl ? RET_SOFT : 0);
		} else if (c == 8) {
			nRet = edit_deleteChar(wp, pCaret, -1, 0);
		}
		else if (c == 127) {
			nRet = edit_deleteChar(wp, pCaret, 1, 0);
		} else {
			bNormalChar = TRUE;
			if (c & 0x100) {
				c &= 0xFF;
			} else if (workmode & WM_OEMMODE) {
				char source[2];
				char string2[10];
				source[0] = c;
				source[1] = 0;
				AnsiToOemBuff(source, string2, 1);
				c = *string2;
			}
			offs = pCaret->offset;
			if (c == '\t' && lnp->expandTabsWith) {
				int n2 = caret_lineOffset2screen(wp, pCaret);
				int n = indent_calculateNextTabStop(n2, &wp->indentation);
				nchars = n - n2;
				c = lnp->expandTabsWith;
			} else {
				nchars = 1;
			}
			if (workmode & WM_INSERT || offs >= pCaret->linePointer->len) {
				len = nchars;
			} else {
				len = 0;
			}

			if ((lp = ln_modify(fp, pCaret->linePointer, offs, offs + len)) == 0L) {
				return 0;
			}
			memset(&lp->lbuf[offs], c, nchars);
			render_repaintLine(fp, pCaret->linePointer);

			offs += nchars;
			edit_placeCursor(wp, pCaret, pCaret->ln, offs);
			nRet = 1;
		}
		if (!nRet) {
			return 0;
		}
		pCaret = pCaret->next;
	}
	if (!bNormalChar) {
		return 1;
	}
	codecomplete_updateCompletionList(wp, FALSE);
	if (!_playing) {

		if (workmode & WM_ABBREV) {
			template_expandAbbreviation(wp,lp,offs);
		}
		if (workmode & WM_SHOWMATCH) {
			uc_showMatchingBracket(wp);
		}

		if (!edit_autoFormat(wp) &&
		    (workmode & WM_AUTOWRAP) && 
			wp->caret.offset >= ww_getRightMargin(wp)) {
			edit_wrapAround(wp, pCaret);
		}
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * EdCharDelete()
 * delete character(s), words, ....
 */
long long EdCharDelete(int control) {
	int matchc;
	WINFO* wp = ww_getCurrentEditorWindow();

	render_updateCaret(wp);
	CARET* pCaret = &wp->caret;
	caret_setMatchFunction(control, IDS_DELUNTILC, &matchc);
	while (pCaret) {
		if (!edit_deleteChar(wp, pCaret, control, matchc)) {
			return 0;
		}
		pCaret = pCaret->next;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * edit_performLineFlagOperation()
 */
long long edit_performLineFlagOperation(MARKED_LINE_OPERATION op) {
	long 	ln;
	FTABLE* fp;
	LINE* lp;
	LINE* last;
	WINFO* wp = ww_getCurrentEditorWindow();
	int changed = 0;
	fp = wp->fp;
	last = fp->lastl->prev;
	lp = fp->firstl;
	switch (op) {

	case MLN_TOGGLE:
		while (1) {
			lp->lflg ^= LNXMARKED;
			if (lp == last)
				break;
			lp = lp->next;
		}
		break;

	case MLN_MAKESOFT:
		changed = ln_changeFlag(fp, lp, last, 0, LNNOCR, 1);
		break;

	case MLN_MAKEHARD:
		changed = ln_changeFlag(fp, lp, last, 0, LNNOCR, 0);
		break;

	case MLN_FINDSOFT:
		ln_changeFlag(fp, lp, last, LNNOCR, LNXMARKED, 1);
		break;

	default:
		for (ln = 0L; lp != 0 && !(lp->lflg & LNXMARKED);
			lp = lp->next, ln++);
		if (!lp)
			return 1;
		caret_placeCursorAndSavePosition(wp, ln, 0L);
		mouse_setBusyCursor();
		if (op == MLN_JOIN)
			ln_joinLines(fp);
		else
			ft_cutMarkedLines(wp, (op == MLN_DELETE));
	}
	if (changed) {
		ft_setFlags(fp, fp->flags | F_CHANGEMARK);
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * EdLineSplit()
 * do cr+lf-Actions
 */
long long EdLineSplit(int flags) {
	WINFO* wp = ww_getCurrentEditorWindow();
	int nRet = 1;

	CARET* pCaret = &wp->caret;
	while (pCaret) {
		nRet = nRet && edit_splitLine(wp, pCaret, flags);
		pCaret = pCaret->next;
	}
	FTABLE* fp = wp->fp;
	render_repaintAllForFile(fp);
	mouse_setDefaultCursor();
	return nRet;
}

/*--------------------------------------------------------------------------
 * EdHideLines()
 */
int EdHideLines(WINFO* wp) {	
	FTABLE *	fp;
	LINE *	lp1;
	LINE *	lp2;

	if (wp == 0 ||
		(fp = wp->fp) == 0 ||
		!ww_hasSelection(wp)) {
		return 0;
	}

	lp1 = wp->blstart->m_linePointer;
	lp2 = wp->blend->m_linePointer;
	bl_hideSelection(wp, 0);

	ln_hide(fp, lp1, lp2);
	caret_moveToLine(wp, ln_cnt(fp->firstl, wp->caret.linePointer) - 1);

	render_repaintAllForFile(fp);
	return 1;
}

/*--------------------------------------------------------------------------
 * EdUnHideLine()
 */
int EdUnHideLine(WINFO* wp)
{
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
int EdExpandAbbreviation(WINFO* wp) {
	if (wp) {
		return
			template_expandAbbreviation(wp->fp,wp->caret.linePointer,wp->caret.offset);
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * edit_convertCharacterCase()
 * Map lower characters to upper, upper to lowers and toggle case depending on
 * the passed operation flag.
 */
extern unsigned char _l2uset[], _u2lset[];
long long edit_convertCharacterCase(CC_OPERATION operation) {
	LINE* lp;
	unsigned char c, c1;
	int  offs;
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE* fp = wp->fp;
	BOOL bBlockMode;
	BOOL bLastLine;
	CARET cStart;
	CARET cEnd;

	if (wp->blstart && wp->blend) {
		bBlockMode = TRUE;
		cStart.linePointer = wp->blstart->m_linePointer;
		cStart.offset = wp->blstart->m_column;
		cEnd.linePointer = wp->blend->m_linePointer;
		cEnd.offset = wp->blend->m_column;
	} else {
		bBlockMode = FALSE;
		cStart = wp->caret;
		cEnd = wp->caret;
		cEnd.offset++;
	}
	lp = cStart.linePointer;
	offs = cStart.offset;
	bLastLine = lp == cEnd.linePointer;
	while(TRUE) {
		c = lp->lbuf[offs];
		c1 = c;
		if (operation == CC_TOGGLE || operation == CC_UPPER) {
			c1 = _l2uset[c];
		}
		if ((c == c1 && operation == CC_TOGGLE) || operation == CC_LOWER) {
			c1 = _u2lset[c];
		}
		if (c1 != c && (lp = ln_modify(fp, lp, offs, offs)) != (LINE*)0) {
			lp->lbuf[offs] = c1;
		}
		if (lp == NULL) {
			break;
		}
		offs++;
		if (offs > lp->len || (bLastLine && offs >= cEnd.offset)) {
			if (bLastLine) {
				break;
			}
			lp = lp->next;
			bLastLine = lp == cEnd.linePointer;
			if (lp == fp->lastl) {
				break;
			}
			offs = 0;
		}
	}
	if (bBlockMode) {
		render_repaintLineRange(fp, wp->blstart->m_linePointer, wp->blend->m_linePointer);
		return 1;
	}
	render_repaintLine(fp, wp->caret.linePointer);
	return EdCursorRight(1);
}

/*
 * Insert a string into the passed file at the current cursor position.
 */
long long edit_insertString(WINFO* wp, const char* pszString) {
	if (!pszString) {
		return 0;
	}
	PASTE buffer;
	memset(&buffer, 0, sizeof buffer);
	size_t nSize = strlen(pszString);
	char* pszCopy = calloc(1, nSize + 2);
	strcpy(pszCopy, pszString);
	// Need a trailing newline for convertTextToPasteBuffer....
	pszCopy[nSize++] = '\n';
	int ret = bl_convertTextToPasteBuffer(&buffer, (char*)pszCopy, (char*)(pszCopy + nSize), '\n', -1, '\r');
	free(pszCopy);
	if (ret) {
		CARET oldCaret = wp->caret;
		bl_pasteBlock(wp, &buffer, 0, oldCaret.offset, 0);
		bl_free(&buffer);
		return 1;
	}
	return 0;
}

