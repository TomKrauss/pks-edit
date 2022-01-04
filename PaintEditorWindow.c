/*
 * PaintEditorWindow.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS
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
#include "customcontrols.h"
#include "syntaxhighlighting.h"

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

static int showSyntaxHighlighting = TRUE;

/*-----------------------------------------
 * Render an arbitrary string with a given length at a position using a font-style.
 */
int render_formattedString(RENDER_CONTEXT* pRC, int x, int y, unsigned char* cBuf, size_t nLength, FONT_STYLE_CLASS nStyle, FONT_STYLE_CLASS* pPreviousStyle) {
	if (nStyle != *pPreviousStyle) {
		if (showSyntaxHighlighting) {
			font_selectFontStyle(pRC->rc_theme, pRC->rc_wp, nStyle, pRC->rc_hdc);
		}
		*pPreviousStyle = nStyle;
	}
	TextOut(pRC->rc_hdc, x, y, cBuf, (int)nLength);
	return x + pRC->rc_wp->cwidth;
}

typedef enum { RS_WORD, RS_SPACE, RS_CONTROL, RS_START, RS_TAB } RENDER_STATE;

/*--------------------------------------------------------------------------
 * markline()
 * Paint the selection in a line.
 */
static void paintSelection(HDC hdc, WINFO* wp, LINE* lp, int y, int lastcol)
{
	RECT 	r;

	r.top = y;
	r.bottom = y + wp->cheight;
	r.left = 0;
	if (ww_isColumnSelectionMode(wp) != 0) {
		r.left = wp->blcol1;
		r.right = wp->blcol2;
	}
	else {
		if (!wp->blstart || !wp->blend) {
			error_displayAlertDialog("bad marked line");
			return;
		}
		if (P_EQ(lp, wp->blstart->m_linePointer))
			r.left = caret_lineOffset2screen(wp, &(CARET) { lp, wp->blstart->m_column});
		else r.left = wp->mincol;

		if (P_EQ(lp, wp->blend->m_linePointer))
			r.right = caret_lineOffset2screen(wp, &(CARET) { lp, wp->blend->m_column});
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

static void render_fillBuf(char* pszBuf, int fillChar, int nLen) {
	if (nLen >= 20) {
		nLen = 19;
	}
	for (int i = 0; i < nLen; i++) {
		pszBuf[i] = fillChar;
	}
	pszBuf[nLen] = 0;
}

/*--------------------------------------------------------------------------
 * render_singleLineOnDevice()
 */
int render_singleLineOnDevice(RENDER_CONTEXT* pRC, int x, int y, LINE *lp, long lineNo) {
	register int startColumn,i,endColumn,indent,textlen;
	register unsigned char 	*d,*s,*send,*dend;
	WINFO* wp = pRC->rc_wp;
	FTABLE* fp = ((FTABLE*)FTPOI(wp));
	char fillbuf[20];
	char eofStyles[20];
	char buf[300];
	unsigned char* pszStyles;
	int flags;
	int startX = x;
	RENDER_STATE state = RS_START;
	int showcontrol;
	int tabFiller = wp->tabDisplayFillCharacter;

	startColumn = wp->mincol;
	endColumn = wp->maxcol+1;
	flags = wp->dispmode;
	showcontrol = flags & SHOWCONTROL;
	showSyntaxHighlighting = flags & SHOW_SYNTAX_HIGHLIGHT;
	textlen = endColumn-startColumn;
	if (showcontrol) {
		render_fillBuf(fillbuf, tabFiller != ' ' && tabFiller && tabFiller != '\t' ? tabFiller : '»', 1);
	}
	d = buf;
	dend = buf + sizeof buf - 1;
	i = 0;
	s = lp->lbuf;
	send = &lp->lbuf[lp->len];
	RENDER_STATE newstate = state;
	x -= startColumn * wp->cwidth;
	FONT_STYLE_CLASS fsPreviousClass = -1;
	FONT_STYLE_CLASS fsClass = FS_NORMAL;
	if (fp->lastl == lp) {
		render_fillBuf(eofStyles, FS_CONTROL_CHARS, lp->len);
		pszStyles = eofStyles;
	} else {
		pszStyles = highlight_calculate(wp->highlighter, wp, lp, lineNo);
	}
	while (i < endColumn && s < send) {
		unsigned char c = *s++;
		FONT_STYLE_CLASS fsNext = (int)*pszStyles++;
		i++;
		if (c == ' ' && showcontrol) {
			newstate = RS_SPACE;
		} else if (c >= ' ') {
			newstate = RS_WORD;
		} else if (c == '\t') {
			indent = indent_calculateTabStop(i, &wp->indentation);
			i--;
			if (indent > endColumn) {
				indent = endColumn;
			}
			newstate = RS_TAB;
		} else {
			newstate = RS_CONTROL;
		}
		if (newstate != state || fsNext != fsClass || d >= dend) {
			textlen = (int)(d - buf);
			if (textlen > 0) {
				int x2 = x + textlen * wp->cwidth;
				if (x2 >= startX) {
					render_formattedString(pRC, x, y, buf, textlen, (showcontrol && state == RS_SPACE) ? FS_CONTROL_CHARS : fsClass, &fsPreviousClass);
				}
				x = x2;
			}
			state = newstate;
			d = buf;
			fsClass = fsNext;
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
				render_formattedString(pRC, x, y, "?", 1, FS_CONTROL_CHARS, &fsPreviousClass);
			}
			x += wp->cwidth;
		} else if (state == RS_TAB) {
			if (showcontrol) {
				if (x >= startX) {
					render_formattedString(pRC, x, y, fillbuf, 1, FS_CONTROL_CHARS, &fsPreviousClass);
				}
			} else if (tabFiller && tabFiller != ' ') {
				render_fillBuf(fillbuf, tabFiller, indent - i);
				render_formattedString(pRC, x, y, fillbuf, indent-i, FS_CONTROL_CHARS, &fsPreviousClass);
			}
			x += (indent - i) * wp->cwidth;
			i = indent;
		}
	}
	textlen = (int)(d - buf);
	if (textlen > 0) {
		int x2 = x+textlen * wp->cwidth;
		if (x2 >= startX) {
			render_formattedString(pRC, x, y, buf, textlen, (showcontrol && state == RS_SPACE) ? FS_CONTROL_CHARS : fsClass, &fsPreviousClass);
		}
		x = x2;
		i += textlen;
	}
	if (showcontrol && i >= wp->mincol && !(lp->lflg & LNNOTERM) && x >= startX && s >= send) {
		render_formattedString(pRC, x, y, (lp->lflg & LNNOCR) ? "¬" : "¶", 1, FS_CONTROL_CHARS, &fsPreviousClass);
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

	int nSize = (int)strlen(buf);
	TextOut(hdc, wp->cwidth, y, buf, nSize);
	dwExtent = win_getTextExtent(hdc, buf, nSize);
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

/*
 * Returns the LINE pointer for a line with a given index.
 */
LINE* ww_getMinLine(WINFO* wp, int idx) {
	if (idx == 0) {
		FTABLE* fp = wp->fp;
		wp->lpMinln = fp->firstl;
		wp->cachedLineIndex = 0;
	} else  if (idx != wp->cachedLineIndex || wp->lpMinln == 0) {
		wp->lpMinln = ln_goto(wp->fp, idx);
		wp->cachedLineIndex = idx;
	}
	return wp->lpMinln;
}

/**
 * Render the custom secondary caret.
 */
static void render_customCaret(WINFO* wp, HDC hdc, int y) {
	int width = wp->secondaryCaretWidth;
	if (width <= 0) {
		width = 1;
	}
	cust_drawOutline(hdc, (wp->secondaryCaret.col - wp->mincol) * wp->cwidth, y, width*wp->cwidth, wp->cheight);
}

/*
 * Render the current window in normal ascii / code mode assuming a fixed character spacing font. 
 */
void render_asciiMode(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	THEME_DATA* pTheme = pCtx->rc_theme;
	int  		newy, visLen;
	long		ln;
	long		minMarkedLine;
	long		maxMarkedLine;
	RECT 		r;
	HBRUSH		hBrushCompareModifiedColor;
	HBRUSH		hBrushCompareAddedColor;
	HBRUSH		hBrushCompareDeletedColor;
	HBRUSH		hBrushCaretLine;
	HDC			hdc = pCtx->rc_hdc;
	WINFO* wp = pCtx->rc_wp;

	hBrushCaretLine = CreateSolidBrush(pTheme->th_caretLineColor);
	hBrushCompareModifiedColor = CreateSolidBrush(pTheme->th_compareModifiedColor);
	hBrushCompareAddedColor = CreateSolidBrush(pTheme->th_compareAddedColor);
	hBrushCompareDeletedColor = CreateSolidBrush(pTheme->th_compareDeletedColor);
	long min = wp->minln;
	long max = wp->maxln;
	LINE* lp;
	lp = ww_getMinLine(wp, min);
	RECT rect;
	GetClientRect(wp->ww_handle, &rect);
	ww_getSelectionLines(wp, &minMarkedLine, &maxMarkedLine);
	int cheight = wp->cheight;
	for (ln = min; lp && ln <= max && y < pClip->bottom;
		lp = lp->next, ln++, y = newy) {
		newy = y + cheight;
		if (newy > pClip->top) {
			HBRUSH hBrush = hBrushBg;
			if (lp->lflg & (LNXMARKED | LN_COMPARE_MODIFIED)) {
				hBrush = hBrushCompareModifiedColor;
			} else if (lp->lflg & LN_COMPARE_ADDED) {
				hBrush = hBrushCompareAddedColor;
			} else if (lp->lflg & LN_COMPARE_DELETED) {
				hBrush = hBrushCompareDeletedColor;
			} else if (lp == wp->caret.linePointer && (wp->dispmode & SHOWCARET_LINE_HIGHLIGHT)) {
				hBrush = hBrushCaretLine;
			}
			r.left = rect.left; 
			r.right = rect.right;
			r.top = y;
			r.bottom = min(pClip->bottom, y + cheight);
			FillRect(hdc, &r, hBrush);
			if (lp->lflg & LNINDIRECT) {
				redraw_indirect(hdc, wp, y, lp);
				visLen = 1;
			} else if (wp->renderer->r_renderLine) {
				visLen = wp->renderer->r_renderLine(pCtx, 0, y, lp, ln);
			}
			if (ln >= minMarkedLine && ln <= maxMarkedLine) {
				paintSelection(hdc, wp, lp, y, visLen);
			}
			if (ln == wp->secondaryCaret.ln && wp->secondaryCaret.linePointer) {
				render_customCaret(wp, hdc, y);
			}
		}
	}

	if (!lp) {
		r.left = rect.left; r.right = rect.right;
		r.top = y;
		r.bottom = min(pClip->bottom, y + (max - ln) * cheight);
		if (r.bottom > r.top) {
			FillRect(hdc, &r, hBrushBg);
		}
	}
	DeleteObject(hBrushCaretLine);
	DeleteObject(hBrushCompareModifiedColor);
	DeleteObject(hBrushCompareAddedColor);
	DeleteObject(hBrushCompareDeletedColor);
}

/*--------------------------------------------------------------------------
 * render_paintWindowParams()
 */
static void render_paintWindowParams(WINFO *wp) {
	HBRUSH		hBrushBg;
	HDC 		hdc;
	HWND		hwnd;
	int			y;
	PAINTSTRUCT ps;
	THEME_DATA* pTheme = theme_getCurrent();

	hwnd = wp->ww_handle;
	hdc = BeginPaint(hwnd, &ps);
	font_selectFontStyle(pTheme, wp, FS_NORMAL, hdc);
	hBrushBg = CreateSolidBrush(pTheme->th_defaultBackgroundColor);
	y = calcy(wp, wp->minln);
	RENDER_CONTEXT renderContext;

	renderContext.rc_hdc = hdc;
	renderContext.rc_theme = pTheme;
	renderContext.rc_printing = FALSE;
	renderContext.rc_wp = wp;
	wp->renderer->r_renderPage(&renderContext, &ps.rcPaint, hBrushBg, y);
	DeleteObject(hBrushBg);
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
	render_paintWindowParams(wp);
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
	if (wp->ru_handle != NULL) {
		InvalidateRect(wp->ru_handle, pRect, 0);
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

/*
 * Repaint one window.
 */
void render_repaintWindow(WINFO* wp) {
	render_repaintForWindow(wp, NULL);
}

/*--------------------------------------------------------------------------
 * render_repaintAllForFile()
 */
EXPORT void render_repaintAllForFile(FTABLE *fp) {
	ft_forAllViews(fp, render_repaintForWindow, NULL);
}

/*
 * Repaint the line numbers in the given view. 
 */
int render_repaintLineNumbers(WINFO* wp, void* pUnused) {
	if (wp->lineNumbers_handle) {
		InvalidateRect(wp->lineNumbers_handle, NULL, 0);
	}
	return 1;
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
 * render_repaintWindowLine()
 * Repaint the given line in the window passed as a parameter.
 */
EXPORT void render_repaintWindowLine(WINFO* wp, long ln) {
	if (wp != 0L) {
		struct tagLINE_REDRAW redraw = { ln, 0, -1};
		render_repaintLineForWindow(wp, &redraw);
	}
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

/*
 * Repaint a specific line (line pointer).
 */
static int render_repaintLinePointerForWindow(WINFO* wp, LINE* lpWhich) {
	int idx = wp->minln;
	struct tagLINE_REDRAW redraw;
	redraw.col1 = 0;
	redraw.col2 = -1;
	LINE* lpFirst = ww_getMinLine(wp, idx);
	while (lpFirst && idx <= wp->maxln) {
		if (lpFirst == lpWhich) {
			redraw.ln = idx;
			render_repaintLineForWindow(wp, &redraw);
			break;
		}
		idx++;
		lpFirst = lpFirst->next;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * render_repaintLine()
 * Send a repaint to the specific line.
 * TODO: currently very expensive for long files.
 */
EXPORT void render_repaintLine(FTABLE *fp, LINE *lpWhich)
{
	ft_forAllViews(fp, render_repaintLinePointerForWindow, lpWhich);
}

/*--------------------------------------------------------------------------
 * render_repaintLineRange()
 * Send a repaint to a range of lines specified by line pointer. Either repaint
 * a specific window (wp != NULL) or all windows for the document fp.
 */
static void render_repaintLineRangeWindowInternal(WINFO* wp, FTABLE* fp, LINE* lpStart, LINE* lpEnd) {
	int idx = ln_indexOf(fp, lpStart);
	if (idx < 0) {
		if (wp) {
			render_repaintForWindow(wp, 0);
		} else {
			render_repaintAllForFile(fp);
		}
		return;
	}
	int nCount = ln_cnt(lpStart, lpEnd);
	if (nCount <= 0) {
		idx = ln_indexOf(fp, lpEnd);
		nCount = ln_cnt(lpEnd, lpStart);
	}
	if (wp) {
		struct tagLINE_FROM_TO param = { idx, idx+nCount};
		render_repaintWindowFromLineTo(wp, &param);
	} else {
		render_repaintFromLineTo(fp, idx, idx + nCount);
	}
}

/*--------------------------------------------------------------------------
 * render_repaintLineRange()
 * Send a repaint to a range of lines specified by line pointer.
 */
EXPORT void render_repaintLineRange(FTABLE* fp, LINE* lpStart, LINE* lpEnd) {
	render_repaintLineRangeWindowInternal(NULL, fp, lpStart, lpEnd);
}

/*--------------------------------------------------------------------------
 * render_repaintLineRangeWindow()
 * Send a repaint to a range of lines specified by line pointer to one window only.
 */
EXPORT void render_repaintLineRangeWindow(WINFO* wp, LINE* lpStart, LINE* lpEnd) {
	render_repaintLineRangeWindowInternal(wp, wp->fp, lpStart, lpEnd);
}

/*
 * Set a fixed zoom factor for a window. 
 */
void ww_setZoom(WINFO* wp, float newFactor) {
	if (wp->zoomFactor == newFactor) {
		return;
	}
	wp->zoomFactor = newFactor;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(wp->ww_handle, &ps);
	font_selectFontStyle(theme_getCurrent(), wp, FS_NORMAL, hdc);
	EndPaint(wp->ww_handle, &ps);
	wt_tcursor(wp, 0);
	wt_tcursor(wp, 1);
	wp->renderer->r_adjustScrollBounds(wp);
	render_repaintForWindow(wp, NULL);

}

/*
 * Increase / decrease the zoom factor of the current window. 
 */
int ww_zoomWindow(int anIncreaseFactor) {
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp == 0) {
		return 0;
	}
	float zoomFactor = wp->zoomFactor;
	if (anIncreaseFactor) {
		zoomFactor = 11 * zoomFactor / 10;
	} else {
		zoomFactor = 10 * zoomFactor / 11;
	}
	if (zoomFactor < 0.4) {
		zoomFactor = 0.4f;
	} else if (zoomFactor > 2.5) {
		zoomFactor = 2.5f;
	}
	ww_setZoom(wp, zoomFactor);
	COMPARISON_LINK* cpl = wp->comparisonLink;
	if (cpl) {
		ww_setZoom(wp == cpl->cl_wpLeft ? cpl->cl_wpRight : cpl->cl_wpLeft, zoomFactor);
	}
	return 1;
}
