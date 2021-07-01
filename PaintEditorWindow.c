/*
 * PaintEditorWindow.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: paint edit (work-)windows
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include "trace.h"
#include "caretmovement.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"
#include "errordialogs.h"
#include "winutil.h"
#include "themes.h"

static DWORD _ROPcodes[] = {
	BLACKNESS,          /* D = 0 */
	0x00500A9UL,        /* ~(P|D) */
	0x00A0329UL,        /* ~P & D */
	0x00F0001UL,        /* ~P */
	0x00500325UL,       /* P&~D */
	DSTINVERT,          /* ~D */
	PATINVERT,          /* P^D */
	0x005F00E9UL,       /* ~(P&D) */
	0x00A000C9UL,		/* P&D */
	0x00A50065UL, 		/* ~(P^D) */
	0x00AA0029UL, 		/* D */
	0x00AF0029UL, 		/* ~P|D */
	PATCOPY, 			/* P */
	0x00F50225UL,       /* P|~D */
	0x00FA0089UL,       /* P|D */
	WHITENESS			/* D = 1 */
};

/*-----------------------------------------
 * Render some control characters in control-mode. 
 */
static int render_formattedString(HDC hdc, WINFO* wp, int x, int y, unsigned char* cBuf, size_t nLength, THEME_DATA *pTheme, FONT_STYLE_CLASS nStyle) {
	COLORREF	hControlColor = nStyle == FS_NORMAL ? wp->editFontStyle.fgcolor : pTheme->th_defaultControlColor;

	SetTextColor(hdc, hControlColor);
	TextOut(hdc, x, y, cBuf, (int)nLength);
	return x + wp->cwidth;
}

typedef enum { RS_WORD, RS_SPACE, RS_CONTROL, RS_START, RS_TAB } RENDER_STATE;

/*--------------------------------------------------------------------------
 * markline()
 * Paint the selection in a line.
 */
static void paintSelection(HDC hdc, WINFO* wp, LINE* lp, int y, int lastcol)
{
	RECT 	r;
	FTABLE* fp = FTPOI(wp);

	r.top = y;
	r.bottom = y + wp->cheight;
	r.left = 0;
	if (ww_hasColumnSelection(wp) != 0) {
		r.left = wp->blcol1;
		r.right = wp->blcol2;
	}
	else {
		if (!wp->blstart || !wp->blend) {
			error_displayAlertDialog("bad marked line");
			return;
		}
		if (P_EQ(lp, wp->blstart->lm))
			r.left = caret_lineOffset2screen(wp, &(CARET) { lp, wp->blstart->lc});
		else r.left = wp->mincol;

		if (P_EQ(lp, wp->blend->lm))
			r.right = caret_lineOffset2screen(wp, &(CARET) { lp, wp->blend->lc});
		else r.right = lastcol;
	}
	r.left -= wp->mincol; if (r.left < 0) r.left = 0;

	if (r.right > wp->maxcol + 1)
		r.right = wp->maxcol + 1;
	r.right -= wp->mincol;

	r.left = r.left * wp->cwidth;
	r.right = r.right * wp->cwidth;
	if (r.right > r.left) {
		SelectObject(hdc, GetStockObject(wp->markstyles[FS_BMARKED].style));
		PatBlt(hdc, r.left, r.top,
			r.right - r.left, r.bottom - r.top,
			_ROPcodes[wp->markstyles[FS_BMARKED].mode]);
	}
}

/*--------------------------------------------------------------------------
 * render_singleLineOnDevice()
 */
int render_singleLineOnDevice(HDC hdc, int x, int y, WINFO *wp, LINE *lp)
{	register int 			startColumn,i,endColumn,indent,textlen;
	register unsigned char 	*d,*s,*send;
	EDIT_CONFIGURATION 	*linp = ((FTABLE*)FTPOI(wp))->documentDescriptor;
	/* limited linelength ! */
	char 				buf[1024];
	int					flags;
	int					startX = x;
	RENDER_STATE		state = RS_START;
	int					showcontrol;
	THEME_DATA* pTheme = theme_getByName(wp->win_themeName);

	startColumn = wp->mincol;
	endColumn = wp->maxcol+1;
	flags = wp->dispmode;
	showcontrol = flags & SHOWCONTROL;
	textlen = endColumn-startColumn;

	d = buf;
	i = 0;
	s = lp->lbuf;
	send = &lp->lbuf[lp->len];
	RENDER_STATE newstate = state;
	x -= startColumn * wp->cwidth;
	while (i < endColumn && s < send) {
		unsigned char c = *s++;
		i++;
		if (c == ' ') {
			newstate = RS_SPACE;
		} else if (c >= ' ') {
			newstate = RS_WORD;
		} else if (c == '\t') {
			indent = doctypes_calculateTabStop(i, linp);
			i--;
			if (indent > endColumn) {
				indent = endColumn;
			}
			newstate = RS_TAB;
		} else {
			newstate = RS_CONTROL;
		}
		if (newstate != state) {
			textlen = (int)(d - buf);
			if (textlen > 0) {
				int x2 = x + textlen * wp->cwidth;
				if (x2 >= startX) {
					render_formattedString(hdc, wp, x, y, buf, textlen, pTheme, (showcontrol && state == RS_SPACE) ? FS_CONTROL_CHARS : FS_NORMAL);
				}
				x = x2;
			}
			state = newstate;
			d = buf;
		}
		if (state == RS_WORD) {
			*d++ = c;
		} else if (state == RS_SPACE) {
			if (showcontrol) {
				*d++ = '·';
			} else {
				x += wp->cwidth;
			}
		} else if (state == RS_CONTROL) {
			if (x >= startX) {
				render_formattedString(hdc, wp, x, y, "?", 1, pTheme, FS_CONTROL_CHARS);
			}
			x += wp->cwidth;
		} else if (state == RS_TAB) {
			if (showcontrol && x >= startX) {
				render_formattedString(hdc, wp, x, y, "»", 1, pTheme, FS_CONTROL_CHARS);
			}
			x += (indent - i) * wp->cwidth;
			i = indent;
		}
	}
	textlen = (int)(d - buf);
	if (textlen > 0) {
		int x2 = x+textlen * wp->cwidth;
		if (x2 >= startX) {
			render_formattedString(hdc, wp, x, y, buf, textlen, pTheme, (showcontrol && state == RS_SPACE) ? FS_CONTROL_CHARS : FS_NORMAL);
		}
		x = x2;
		i += textlen;
	}
	if (showcontrol && i >= wp->mincol && !(lp->lflg & LNNOTERM) && x >= startX) {
		render_formattedString(hdc, wp, x, y, (lp->lflg & LNNOCR) ? "¬" : "¶", 1, pTheme, FS_CONTROL_CHARS);
	}
	return (x-startX)/wp->cwidth;
}

/*------------------------------------------------------------
 * calcy()
 */
static int calcy(WINFO *wp,long ln)
{	int y = ln-wp->minln;

	return y * wp->cheight;
}

/*--------------------------------------------------------------------------
 * redraw_indirect()
 */
static void redraw_indirect(HDC hdc, WINFO *wp, int y, LINE *lp)
{
	int		x;
	int		nWidth;
	DWORD	dwExtent;
	char 	buf[100];

	wsprintf(buf," skipped %ld... ", (long)LpIndNTotal(lp));

	TextOut(hdc, wp->cwidth, y, buf, lstrlen(buf));
	dwExtent = win_getTextExtent(hdc, buf, lstrlen(buf));
	nWidth = LOWORD(dwExtent) + 4;
	y = y + wp->cheight/2-1;
	RECT rect;
	GetClientRect(wp->ww_handle, &rect);
	x = rect.right;
	MoveTo(hdc, nWidth, y);
	LineTo(hdc, x, y);

	y += 2;
	MoveTo(hdc, nWidth, y);
	LineTo(hdc, x, y);
}

/*--------------------------------------------------------------------------
 * render_paintWindowParams()
 */
static void render_paintWindowParams(WINFO *wp, long min, long max, int flg)
{
	HBRUSH		hBrushBg;
	HBRUSH		hBrushCaretLine;
	HBRUSH		hBrushMarkedLines;
	HDC 			hdc;
	HWND			hwnd;
	HFONT 		saveFont;
	RECT 		r;
	PAINTSTRUCT 	ps;
	int  		y,newy,visLen;
	long			ln;
	long		minMarkedLine;
	long		maxMarkedLine;
	FTABLE *		fp = FTPOI(wp);
	LINE *		lp;
	THEME_DATA* pTheme = theme_getByName(wp->win_themeName);

	hwnd = wp->ww_handle;

	hdc = BeginPaint(hwnd, &ps);
	saveFont = font_selectDefaultEditorFont(wp,hdc,NULL);
	SetTextColor(hdc,wp->fnt_fgcolor);
	SetBkMode(hdc, TRANSPARENT);
	hBrushBg = CreateSolidBrush(wp->fnt_bgcolor);
	hBrushCaretLine = CreateSolidBrush(pTheme->th_caretLineColor);
	hBrushMarkedLines = CreateSolidBrush(pTheme->th_markedLineColor);

	y = calcy(wp,min);
	lp = ln_goto(fp,min);
	RECT rect;
	GetClientRect(wp->ww_handle, &rect);
	ww_getSelectionLines(wp, &minMarkedLine, &maxMarkedLine);
	for (ln = min; lp && ln <= max && y < ps.rcPaint.bottom;
		lp = lp->next, ln++,  y = newy) {
		newy = y + wp->cheight;
		if (newy > ps.rcPaint.top && 			/* if in redraw area */
		    (flg || (lp->lflg & LNREPLACED))) {	/* if print_singleLineOfText is modified || we redraw all */
			HBRUSH      hBrush = hBrushBg;
			if (lp->lflg & LNXMARKED) {
				hBrush = hBrushMarkedLines;
			} else if (lp == wp->caret.linePointer && (wp->dispmode & SHOWCARET_LINE_HIGHLIGHT)) {
				hBrush = hBrushCaretLine;
			}
			r.left = rect.left; r.right = rect.right;
			r.top = y;
			r.bottom = min(ps.rcPaint.bottom,y+wp->cheight);
			FillRect(hdc,&r,hBrush);
	
			if (lp->lflg & LNINDIRECT) {
				redraw_indirect(hdc, wp, y, lp);
				visLen = 1;
			} else if (wp->renderFunction) {
				visLen = wp->renderFunction(hdc,0,y,wp,lp);
			}
			if (ln >= minMarkedLine && ln <= maxMarkedLine) {
				paintSelection(hdc, wp, lp, y, visLen);
			}
			if (ln == wp->caret.ln)
				render_updateCustomCaret(wp,hdc);
		}
	}

	if (!lp) {
		r.left = rect.left; r.right = rect.right;
		r.top = y;
		r.bottom = min(ps.rcPaint.bottom,y+(max-ln)*wp->cheight);
		FillRect(hdc,&r,hBrushBg);
	}

	DeleteObject(hBrushBg);
	DeleteObject(hBrushCaretLine);
	DeleteObject(hBrushMarkedLines);
	font_selectSystemFixedFont(hdc);
	EndPaint(hwnd,&ps);

}

/*------------------------------------------------------------
 * render_paintWindow()
 * Perform the actual rendering of window passed as an argument.
 */
EXPORT void render_paintWindow(WINFO* wp)
{
	if (wp == NULL) {
		return;
	}
	render_paintWindowParams(wp, wp->minln, wp->maxln, 1);
}

/*
 * Invalidate an area of the work window to cause a repaint and possibly
 * "associated" windows.
 */
static void render_invalidateRect(WINFO* wp, RECT* pRect) {
	RECT r2;
	InvalidateRect(wp->ww_handle, pRect, 0);
	if (wp->lineNumbers_handle != NULL) {
		if (pRect) {
			GetClientRect(wp->lineNumbers_handle, &r2);
			pRect->right = r2.right;
		}
		InvalidateRect(wp->lineNumbers_handle, pRect, 0);
	}

}


/*--------------------------------------------------------------------------
 * render_repaintFromLineTo()
 */
struct tagLINE_FROM_TO {
	long minln;
	long maxln;
};
static int render_repaintWindowFromLineTo(WINFO* wp, struct tagLINE_FROM_TO* pParam) {
	if (wp->ww_handle) {
		RECT rect;
		GetClientRect(wp->ww_handle, &rect);
		if (pParam->minln > wp->minln) {
			rect.top += (pParam->minln - wp->minln) * wp->cheight;
		}
		if (pParam->maxln < wp->maxln - 1) {
			rect.bottom = rect.top + (pParam->maxln - pParam->minln + 1) * wp->cheight;
		}
		render_invalidateRect(wp, &rect);
	}
	return 1;
}

EXPORT void render_repaintFromLineTo(FTABLE* fp, long minln, long maxln) {
	struct tagLINE_FROM_TO param = { minln, maxln };
	ft_forAllViews(fp, render_repaintWindowFromLineTo, &param);
}

/*--------------------------------------------------------------------------
 * render_repaintCurrentFile()
 */
EXPORT void render_repaintCurrentFile(void) {	
	FTABLE *fp;

	if ((fp = ft_getCurrentDocument()) != 0L) {
		render_repaintAllForFile(fp);
	}
}

/*
 * Repaint one window. 
 */
static int render_repaintForWindow(WINFO* wp, void* pUnused) {
	if (wp->ww_handle != NULL) {
		render_invalidateRect(wp, NULL);
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * render_repaintAllForFile()
 */
EXPORT void render_repaintAllForFile(FTABLE *fp) {
	ft_forAllViews(fp, render_repaintForWindow, NULL);
}

/*
 * Redraw part of a line in a window.
 */
struct tagLINE_REDRAW {
	long ln;
	int col1;
	int col2;
};

static int render_repaintLineForWindow(WINFO* wp, struct tagLINE_REDRAW* pRedraw) {
	RECT  	r;
	int col1 = pRedraw->col1;
	int col2 = pRedraw->col2;
	if (col2 < 0) {
		col2 = wp->maxcol;
	}
	if (wp->ww_handle > 0 && pRedraw->ln >= wp->minln && pRedraw->ln <= wp->maxln) {
		if (col1 < wp->mincol) {
			col1 = wp->mincol;
		}
		if (col2 > wp->maxcol) {
			col2 = wp->maxcol;
		}
		GetClientRect(wp->ww_handle, &r);
		r.left += (col1 - wp->mincol) * wp->cwidth;
		r.right = r.left + (col2 - col1) * wp->cwidth;
		r.top += wp->cheight * (pRedraw->ln - wp->minln);
		r.bottom = r.top + wp->cheight;
		render_invalidateRect(wp, &r);
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * render_repaintLinePart()
 * Redraw the line with the given index in the range between col1 and col2.
 * If col2 is negative the whole line is repainted.
 */
EXPORT void render_repaintLinePart(FTABLE *fp, long ln, int col1, int col2)
{
	struct tagLINE_REDRAW param = {ln, col1, col2};
	ft_forAllViews(fp, render_repaintLineForWindow, &param);
}

/*--------------------------------------------------------------------------
 * render_repaintCurrentLine()
 * Repaint the current line in all windows for a file, where "current" is the
 * line containing the caret in the window passed as a parameter.
 */
EXPORT void render_repaintCurrentLine(WINFO* wp) {
	if (wp != 0L) {
		render_repaintLinePart(wp->fp, wp->caret.ln, 0, -1);
	}
}

/*--------------------------------------------------------------------------
 * render_repaintLine()
 * Send a repaint to the specific line.
 */
EXPORT void render_repaintLine(FTABLE *fp, LINE *lpWhich)
{
	int idx = ln_indexOf(fp, lpWhich);
	render_repaintLinePart(fp, idx, 0, -1);
}

/*--------------------------------------------------------------------------
 * render_repaintLineRange()
 * Send a repaint to a range of lines specified by line pointer. We assume,
 * the first line is before the last line.
 */
EXPORT void render_repaintLineRange(FTABLE* fp, LINE* lpStart, LINE* lpEnd)
{
	int idx = ln_indexOf(fp, lpStart);
	if (idx < 0) {
		render_repaintAllForFile(fp);
		return;
	}
	int startIdx = idx;
	while (lpStart != NULL && lpStart != lpEnd) {
		idx++;
		lpStart = lpStart->next;
	}
	render_repaintFromLineTo(fp, startIdx, idx);
}

/*--------------------------------------------------------------------------
 * render_selectCustomCaret()
 */
EXPORT void render_selectCustomCaret(int on)
{
	WINFO 	*wp = ww_getCurrentEditorWindow();

	if (wp == 0)
		return;
	if (on != wp->owncursor) {
		wp->owncursor = on;
		render_repaintCurrentLine(wp);
	}
}

