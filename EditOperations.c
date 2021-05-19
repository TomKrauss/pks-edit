/*
 * EditOperations.c
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS 3.01
 *
 * purpose: inserting and deleting characters, words, lines ...
 * 		  autoformat, wrap and indent
 *
 * 										created      : 04.02.90
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include "trace.h"
#include "functab.h"
#include "caretmovement.h"
#include "winfo.h"
#include "edierror.h"
#include "pksedit.h"
#include "editorconfiguration.h"

#define	SWAP(a,b)			{	a ^= b, b ^=a, a ^= b;  }

/*--------------------------------------------------------------------------
 * EXTERNALS
 */

extern LINE *	cadv_word(LINE *lp,long *ln,long *col,int dir);
extern LINE *	cadv_c(LINE *lp,long *ln,long *col,int dir,unsigned char match);
extern void 	setmatchfunc(int control, int ids_name, int *c);
extern LINE 	*(*advmatchfunc)();
extern int 	ln_leadspce(LINE *l);
extern int 	IsSpace(unsigned char c);
extern int 	TabStop(int col, DOCUMENT_DESCRIPTOR *l);
extern int 	CntSpaces(unsigned char *s, int pos);
extern int 	undoenq(LINE *lnfirst,LINE *lnlast,int cfirst,int clast,int blockflg);
extern int 	sm_bracketindent(FTABLE *fp, LINE *lp1, LINE *lpcurr, 
				 int indent, int *di, int *hbr);
extern int 	shift_lines(FTABLE *fp, long ln, long nlines, int dir);
extern int 	curpos(long ln, long col);
extern void 	wt_insline(WINFO *wp, int nlines);
extern void 	redrawline(void);
extern void 	ln_changeFlag(LINE *lpstart, LINE *lpend, int flagsearch, int flagmark,
				int set);
extern int 	doabbrev(FTABLE *fp, LINE *lp,int offs);
extern void 	updatecursor(WINFO *wp);

extern long 	_multiplier;
extern int 		cursor_width;
extern PASTE	*_undobuf;

/*--------------------------------------------------------------------------
 * CalcCol2TabsBlanks()
 * calculate the number of TABs and SPACEs we need, if we want to insert
 * a certain # of "blank" columns on start of the line
 * try using a maximum # of TABs
 */
int CalcCol2TabsBlanks(DOCUMENT_DESCRIPTOR *linp, int col, int *add_blanks)
{
	int    i,ntabs;

	/* calculate # of tabstops up to given position */
	for (i = 0, ntabs = 0; 
	     *add_blanks = col - i, (i = TabStop(i,linp)) <= col; 
	     ntabs++)
	     ;

	return ntabs;
}

/*--------------------------------------------------------------------------
 * CalcTabs2Col()
 */
int CalcTabs2Col(DOCUMENT_DESCRIPTOR *linp, int tabs)
{	int col;
	
	for (col = 0; tabs > 0; col = TabStop(col,linp))
		tabs--;
	return col;
}

/*--------------------------------------------------------------------------
 * CalcStartIndentation()
 * return # of indents and # of additional blanks for the start of the line
 */
static int CalcStartIndentation(FTABLE *fp,LINE *lp, 
						  int upto, int *add_blanks)
{	int col;

	col = CntSpaces(lp->lbuf,upto);
	col = caret_lineOffset2screen(fp, &(CARET) { lp->lbuf, col});
	return CalcCol2TabsBlanks(fp->documentDescriptor,col,add_blanks);
}

/*--------------------------------------------------------------------------
 * optinswhite()
 */
LINE *optinswhite(FTABLE *fp, LINE *lp, int col, int *inserted)
{	int t,b,fillc;

	if ((fillc = fp->documentDescriptor->fillc) == 0) {
		fillc = ' ';
		t = CalcCol2TabsBlanks(fp->documentDescriptor,col,&b);
	} else {
		t = 0;
		b = col;
	}

	if ((*inserted = t+b) != 0) {
		if ((lp = ln_modify(fp,lp,0,*inserted)) != 0L) {
			blfill(lp->lbuf,t,'\t');
			blfill(lp->lbuf+t,b,fillc);
		}
	}

	return lp;
}

/*--------------------------------------------------------------------------
 * InsIndent()
 * do autoindenting and bracket indenting, if required
 */
static int  _deltaindent;
static int InsIndent(FTABLE *fp, LINE *olp, LINE *nlp, int oldcol, int *newcol)
{	
	DOCUMENT_DESCRIPTOR *		linp;
	int 			t = 0;
	int			b = 0;

	linp = fp->documentDescriptor;
	if (linp->workmode & WM_AUTOINDENT) {
		/* calculate indentation of line we leave */
		t = CalcStartIndentation(fp,olp,oldcol,&b);
	}

	t += _deltaindent;
	_deltaindent = 0;

	oldcol = CalcTabs2Col(fp->documentDescriptor,t)+b;
	if ((nlp = optinswhite(fp,nlp,oldcol,newcol)) == 0) {
		return 0;
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * do_brindent()
 */
static int do_brindent(FTABLE *fp, int dir, LINE *lp1, LINE *lp2)
{	int indent,b,di1,di2,i1,i2,hbr1,hbr2;

	if ((fp->documentDescriptor->workmode & WM_BRINDENT) == 0) {
		return 0;
	}

	i1 = sm_bracketindent(fp,fp->firstl,lp1,0,&di1,&hbr1);
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
		indent = CalcStartIndentation(fp,lp1,lp1->len,&b);
		if (indent != i1) {
			/* first delete all blank characters, then shift */
			if ((lp1 = ln_modify(fp,lp1,CntSpaces(lp1->lbuf,lp1->len),0)) == 0) {
				return 0;
			}
			shift_lines(fp,fp->ln-(long)dir,1L,i1);
		}
	}

	/* current line inside bracket block, or has bracket, or last line
	 * was shifted
	 */
 
	if (i2 || hbr2 || (hbr1 && indent != i1)) {
		lp2 = fp->caret.linePointer;
		indent = CalcStartIndentation(fp,lp2,lp2->len,&b);
		if (indent != i2) {
			shift_lines(fp,fp->ln,1L,i2-indent);
		}
		curpos(fp->ln,i2);
	}

	return 1;	
}

/*--------------------------------------------------------------------------
 * PostInsline()
 */
static int PostInsline(FTABLE *fp, int dir, long ln, long col)
{
	long  	omincol;
	long		ominln;
	WINFO *	wp = WIPOI(fp);
	LINE  *	lp;

	omincol = wp->mincol;
	ominln  = wp->minln;
	curpos(ln,col);
	if (omincol != wp->mincol) {
		RedrawTotalWindow(fp);
	} else {
		if (ominln == wp->minln) {
			wt_insline(wp,1);
		}
		redrawline();
	}
	lp = fp->caret.linePointer;
	if (dir < 0) {
		lp = lp->next;
	}

	return do_brindent(fp,dir,lp->prev,lp);
}

/*--------------------------------------------------------------------------
 * EdLineInsert()
 * create newline and
 * insert it
 */
int EdLineInsert(int control)
{	LINE     	*olp,*nlp;
	FTABLE 	*fp;
	int      	dir,ai;
	long 	ln;

	if ((fp = _currfile) == 0L) 
		return 0;

	olp = fp->caret.linePointer;
	if ((nlp = ln_settmp(fp,(LINE *)0L,&olp)) == 0L) 
		return 0;
	nlp->len = 0;

	ln_insert(fp,((control) ? fp->caret.linePointer : fp->caret.linePointer->next),nlp);
	fp->tln = nlp;

	ln = fp->ln;
	if (!control) {
		ln++;
		dir = 1;
	} else {
		dir = -1;
	}

	InsIndent(fp,olp,nlp,(int )FBUFSIZE,&ai);

	return PostInsline(fp,dir,ln,(long)ai);
}

/*--------------------------------------------------------------------------
 * EdLineDelete()
 * deleteline
 */
int EdLineDelete(control)
{	LINE		*clfirst,*clast;
	FTABLE	*fp;
	WINFO	*wp;
	int 		lastlinelen;

	fp = _currfile;
	wp = WIPOI(fp);
	if (control & 1) {
		curpos((long)(fp->ln-1),0L);
	} else curpos(fp->ln,0L);
	clfirst = fp->caret.linePointer;
	clast   = clfirst->next;
	if (!clast->next) {
		if (control & CUT_APPND) return 0;
		clast = clast->prev;
		if ((lastlinelen = clast->len) == 0L) return 0;
	} else lastlinelen = 0;
	if (control & CUT_APPND) {
		blappnd(_undobuf,clfirst,clast,0,0);
		ln_delete(fp,clfirst);
	} else {
		undoenq(clfirst, clast, (int) 0, (int) lastlinelen, (int) 0); 
	}

	if (lastlinelen) {
		RedrawFromTo(wp,wp->ln,wp->maxln);
	} else {
		wt_deleteline(wp,0,1);
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
{	WINFO *wp = WIPOI(_currfile);
	LINE *lp = _currfile->caret.linePointer;

	if (!ln_join(_currfile,lp,lp->next,1)) 
		return 0;
	/* this makes sure current line is not drawn twice */
	if (wp->cy + wp->cheight < wp->workarea.g_h) {
		wt_deleteline(wp,1,1);
		redrawline();
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * cb_breakline()
 * break one line in two pieces
 */
static int cb_breakline(FTABLE *fp, int soft)
{	LINE   *nlp;
	int    ai;

	if ((nlp = ln_break(fp,fp->caret.linePointer,fp->caret.offset)) == 0L)
		return 0;

	if (soft) {
		fp->caret.linePointer->lflg |= LNNOCR;
	} else {
		fp->caret.linePointer->lflg &= ~LNNOCR;
	}

	if (fp->documentDescriptor->nl >= 0) {
		fp->caret.linePointer->lflg &= ~LNNOTERM;
	}

	redrawline();
	InsIndent(fp,fp->caret.linePointer,nlp,fp->caret.offset,&ai);

	return PostInsline(fp,1,fp->ln+1L,(long)ai);
}

/*--------------------------------------------------------------------------
 * findwrap()
 */
static int findwrap(FTABLE *fp, LINE *lp, int cursoffset, int *nextword,int rmargin)
{	
	int  	i;
	int		wm;
	int		wordpos;
	int		spacepos;	
	char *	s;

	wm = caret_screen2lineOffset(fp, &(CARET){lp, rmargin});

	if (caret_lineOffset2screen(fp, &(CARET) { lp->lbuf, wm}) < rmargin) {
		return 0;
	}

	s = lp->lbuf;
	*nextword = 0;
	spacepos  = 0;
	for (i = lp->len; i > 0; ) {
		i--;
		if (IsSpace(s[i])) {
			wordpos = i+1;
			do 
				i--;
			while (i > 0 && (IsSpace(s[i])));
			if (i > 0) {
				spacepos  = i+1;
				*nextword = wordpos;
			}
			if (i < wm) {
				/*
				 * seems to be a reasonable position -
				 * skip due to cursor is behind a word which
				 * user is editing, so wrapping would cut
				 * this word into 2 pieces
				 */
				if (i == cursoffset-1)
					return 0;
				break;
			}
		}
	}
	return spacepos;
}

/*--------------------------------------------------------------------------
 * RightMargin()
 */
int RightMargin(FTABLE *fp)
{	int 		rmargin;
	WINFO	*wp;

	if ((rmargin = fp->documentDescriptor->rmargin) == 0 && (wp = WIPOI(fp)) != 0)
		rmargin = (wp->maxcol-wp->mincol)-RM_DELTA;
	return rmargin;
}

/*--------------------------------------------------------------------------
 * dowrap()
 */
static void dowrap(FTABLE *fp)
{	int nextword,delta;

	if (findwrap(fp, fp->caret.linePointer,fp->caret.offset,&nextword,RightMargin(fp)) > 0) {
		delta = fp->caret.offset-nextword;
		curpos(fp->ln,(long)nextword);
		EdLineSplit(0);
		if (delta < 0)
			delta += fp->caret.offset;
		if (delta < 0)
			delta = 0;
		curpos(fp->ln,(long)(fp->caret.offset+delta));
	}
}

/*--------------------------------------------------------------------------
 * doauto()
 */
#define	D_EBUG(x)		{/*ed_error(x); RedrawTotalWindow(fp);*/}
#define	HARD_BREAK(lp)	((lp->lflg & LNNOCR) == 0)

int doauto(FTABLE *fp)
{	register unsigned char *destbuf,*sourcebuf;
	LINE *lp1,*lpnext,*lp;
	int  indent,col;
	int  i_d,desti,di,rm,lpos,wend,wstart,modified,column;
	unsigned char c;
	long ln, newln, newcol;

	destbuf = _linebuf;
	if ((fp->documentDescriptor->workmode & WM_AUTOFORMAT) == 0)
		return 0;

	rm = RightMargin(fp);
	i_d = 0;
	lp = fp->caret.linePointer;
	ln = newln = fp->ln;
	newcol = fp->caret.offset;

	/* watch also previous line */
	if (lp->prev && !HARD_BREAK(lp->prev)) {
		lp = lp->prev;
		ln--;
	}

	while (lp != 0) {

		if (findwrap(fp, lp,-1,&wstart,rm) && wstart < lp->len) {
			/*
			 * if next line is empty, take same indent as current line
			 */
			lpnext = lp->next;
			if (lpnext == fp->lastl || HARD_BREAK(lpnext))
				lpnext = lp;
			indent = ln_leadspce(lpnext);
			indent = caret_lineOffset2screen(fp, &(CARET) { lpnext, indent});

			/* start of next word after word position defined:
			   -> actual line too long 
			 */
			lp->lflg |= LNREPLACED;
			if ((lp = ln_break(fp,lp,wstart)) == 0L ||
			    (lp = optinswhite(fp,lp,indent,&col)) == 0)
				break;
			lp->prev->lflg |= LNNOCR;

			/* Cursor Position affected ? */
			if (ln == newln && newcol >= wstart) {
				newln++;
				newcol += (col-wstart);
			}
			ln++;

			lp->lflg |= LNREPLACED;
			i_d++;
	D_EBUG("!WRAPPED");
			continue;
		} else {

			modified = 0;
			memmove(destbuf,lp->lbuf,lp->len);
			desti = lp->len;
			column = -1;

	join_next_words:

			if (desti > 0 && !IsSpace(lp->lbuf[desti-1])) {
				destbuf[desti++] = ' ';
			}
			if (HARD_BREAK(lp))
				break;
			lpnext = lp->next;
			if (lpnext == fp->lastl)
				break;

			/* find out how many words we may steal from next line */
			sourcebuf = lpnext->lbuf;
			lpos = indent = CntSpaces(sourcebuf,lpnext->len);
			wend = 0;
			di = desti;
			do {
				/* scan next word */
				while(lpos < lpnext->len) {
					if (IsSpace(c = sourcebuf[lpos++])) {
						lpos--;
						break;			
					}
					destbuf[di++] = c;
				}

				/* this is for minimizing caret_lineOffset2screen - calls */
				if (column < 0)
					column = caret_lineOffset2screen(fp, destbuf, di);
				else
					column += (di-desti);

				/* fits into current line within wrapmargin ? */
				if (column >= rm)
					break;
				wend = lpos;
				desti = di;

				/* overread spaces */
				while (lpos < lpnext->len) {
					if (!IsSpace(c = sourcebuf[lpos++])) {
						lpos--;
						break;			
					}
					if (c == '\t')
						column = -1;
					destbuf[di++] = c;
				}
				wstart = lpos;
			} while(lpos < lpnext->len);

			/* some words fit */
			if (wend > 0) {

				/* Cursor Position affected ? */
				if (ln+1 == newln) {
					if (newcol < wstart) {
						newln--;
						newcol += lp->len;
					} else {
						if (newcol > wstart) {
							newcol -= (wstart-indent);
						} else {
							newcol = indent;
						}
					}
				}

				if ((lp = ln_modify(fp,lp,lp->len,desti)) == 0) {
					break;
				}
				memmove(lp->lbuf,destbuf,desti);
				lp->lflg |= LNREPLACED;
				modified = 1;

				lpnext = lp->next;

				if (wstart >= lpnext->len) {
					/* the whole line was appended */
					lp->lflg = (lp->lflg & ~(LNNOTERM|LNNOCR))|
							 (lpnext->lflg & (LNNOTERM|LNNOCR));
					if (!ln_delete(fp,lpnext))
						break;
					i_d--;
	D_EBUG("!DELETED");

					/* look for more */
					goto join_next_words;
				} else {
					if ((lpnext = ln_modify(fp,lpnext,wstart,indent)) == 0)
						break;
					lpnext->lflg |= LNREPLACED;
					lp = lpnext->prev;
	D_EBUG("!JOINED WORDS");
				}
			}
		}

		lp = lp->next;
		ln++;

		if (lp != fp->caret.linePointer && !modified)
			break;
	}

	if (i_d) {
		RedrawTotalWindow(fp);
	} else {
		p_redraw();
		redrawline();
	}

	lp1 = fp->caret.linePointer;
	if (lp1->prev)
		lp1 = lp1->prev;

	ln_removeFlag(lp1,lp,LNREPLACED);

	if (newln >= fp->nlines) {
		newln = fp->nlines-1;
		newcol = lp1->len;
	}

	curpos(newln,newcol);
	return 1;
}

/*--------------------------------------------------------------------------
 * EdCharInsert()
 * normal Character insert
 */
int EdCharInsert(int c)
{	FTABLE *		fp;
	DOCUMENT_DESCRIPTOR *		lnp;
	LINE *		lp;
	int			len;
	int			offs;
	int			nchars;
	int			workmode;
	extern int 	_playing;

	fp   = _currfile;
	lnp	= fp->documentDescriptor;
	if (c < 32 && c != 8 && c != '\t' && c != 10 && c != lnp->nl && c != lnp->cr) {
		return 0;
	}

	if (_playing != 2) {
		cash2(fp, c);
	}

	if (c == lnp->nl || c == lnp->cr || (c == 10 && lnp->nl < 0)) {
		return EdLineSplit(c == lnp->nl ? RET_SOFT : 0);
	}

	if ((GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) && _chkblk(fp)) {
		EdBlockDelete(0);
		if (c == 8 || c == 127) {
			return 1;
		}
	}

	if (c == 8)
		return EdCharDelete(-1);

	if (c == 127)
		return EdCharDelete(1);

	workmode = lnp->workmode;

	if (c & 0x100) {
		c &= 0xFF;
	} else if (workmode & WM_OEMMODE) {
		char *string = "A",string2[10];
		*string = c;
		AnsiToOemBuff(string,string2,1);
		c = *string2;
	}

	offs = fp->caret.offset;

	if (c == '\t' && lnp->fillc) {
		nchars = TabStop(fp->col,lnp) - fp->col;
		c = lnp->fillc;
	} else
		nchars = 1;

	if (workmode & WM_INSERT || offs >= fp->caret.linePointer->len) {
		len = nchars;
	} else {
		len = 0;
	}

	if ((lp = ln_modify(fp,fp->caret.linePointer,offs,offs+len)) == 0L) return 0;
	blfill(&lp->lbuf[offs],nchars,c);

#if 0
	redrawlinepart(offs,10000);
#else
	redrawline();
#endif

	offs += nchars;
	curpos(fp->ln,(long)offs);

	if (!_playing) {

		if (workmode & WM_ABBREV) {
			doabbrev(fp,lp,offs);
		}
		if (workmode & WM_SHOWMATCH) {
			showmatch(lp,fp->caret.offset);
		}

		if (!doauto(fp) &&
		    (workmode & WM_AUTOWRAP) && 
		    fp->col >= RightMargin(fp)) {
			dowrap(fp);
		}
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * EdCharDelete()
 * delete character(s), words, ....
 */
int EdCharDelete(control)
{	register	LINE *lp,*lp1=0;
	FTABLE 	*fp = _currfile;
	long		ln,ln1,o2,o1;
	int		matchc;

	cursor_width = 1;
	updatecursor(WIPOI(fp));
	lp = lp1 = fp->caret.linePointer;
	o1 = o2  = fp->caret.offset;
	ln = ln1 = fp->ln;

	setmatchfunc(control,IDS_DELUNTILC,&matchc);

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
		curpos(ln-1L,(long) lp->len);
		return EdLinesJoin();
	} else switch (control) {
		case  MOT_SINGLE: o2++; break;
		case  MOT_TOEND:  o2 = lp->len;
			break;
		case -MOT_SINGLE: o1 = l_advance(lp,o1); break;
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
		if (!undoenq(lp1, lp, (int) o1, (int) o2, (int) 0)) {
			return 0;
		}
	}

	if (control < 0)
		curpos(ln,o1);

	if (ln != ln1) {
		RedrawTotalWindow(fp);
	} else {
		redrawline();
	}
	doauto(fp);

	return 1;
}

/*--------------------------------------------------------------------------
 * EdLineSplit()
 * do cr+lf-Actions
 */
int EdLineSplit(int flags)
{
	int		control;
	FTABLE *	fp;
	LINE	 *	lp;

	fp = _currfile;
	lp = fp->caret.linePointer;
	control = flags & (RET_PLUS|RET_MINUS);

	if (fp->documentDescriptor->workmode & WM_INSERT) {
		if (control & RET_PLUS) {
			_deltaindent = 1;
		} else if (control & RET_MINUS) {
			_deltaindent = -1;
		}
		cb_breakline(fp, flags & RET_SOFT);
	}
	else if (lp->next != fp->lastl) {
		curpos(fp->ln+1,0L);
	}
	else if (!control) {
		EdLineInsert(0);
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

	fp = _currfile;
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
		newcpos(ln,0L);
		MouseBusy();
		if (op == MLN_JOIN)
			lnjoin_lines(fp);	
		else
			mln_cutlines(fp, op);
	}

	RedrawTotalWindow(fp);
	changemouseform();
	return 1;
}

/*--------------------------------------------------------------------------
 * EdHideLines()
 */
int EdHideLines(void)
{
	FTABLE *	fp;
	LINE *	lp1;
	LINE *	lp2;

	if ((fp = _currfile) == 0 ||
		!_chkblk(fp)) {
		return 0;
	}

	lp1 = fp->blstart->lm;
	lp2 = fp->blend->lm;
	Pastehide(0);

	ln_hide(fp, lp1, lp2);
	fp->ln = WIPOI(fp)->ln = ln_cnt(fp->firstl,fp->caret.linePointer)-1;

	RedrawTotalWindow(fp);
	return 1;
}

/*--------------------------------------------------------------------------
 * EdUnHideLine()
 */
int EdUnHideLine(void)
{
	FTABLE *	fp;

	if ((fp = _currfile) == 0) {
		return 0;
	}

	if (ln_unhide(fp, fp->caret.linePointer)) {
		curpos(fp->ln,0L);
		RedrawTotalWindow(fp);
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * EdMouseSelectLines()
 */
void EdMouseSelectLines(int flg)
{	LINE *lp = _currfile->caret.linePointer;
	int  oflg;

	oflg = lp->lflg;
	if (flg)
		lp->lflg |= LNXMARKED;
	else
		lp->lflg &= ~LNXMARKED;
	if (oflg != lp->lflg)
		redrawline();
}

/*--------------------------------------------------------------------------
 * EdExpandAbbreviation()
 */
int EdExpandAbbreviation(void)
{
	if (_currfile) {
		return
			doabbrev(_currfile,_currfile->caret.linePointer,_currfile->caret.offset);
	}
	return 0;
}

