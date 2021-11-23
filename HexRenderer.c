/*
 * HexRenderer.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: edit files in hex modus
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include <windows.h>
#include "caretmovement.h"
#include "winfo.h"
#include "documentmodel.h"
#include "themes.h"

#define	HEX_BYTES_PER_LINE		32
#define HEX_ASCII_DISTANCE		3
#define	HEX_MAX_COL				4*HEX_BYTES_PER_LINE+HEX_ASCII_DISTANCE-1
#define IS_IN_HEX_NUMBER_AREA(col)	(col < 3*HEX_BYTES_PER_LINE)

typedef struct tagHEX_RENDERER_DATA {
	LINE* pByteOffsetCache;	// Used to speed up the calculation of "byte offsets" into our line pointers (used in Hex editing)
	long	nCachedByteOffset;	// See above - only valid if pByteOffsetCache is not null
} HEX_RENDERER_DATA;

/*-----------------------------------------
 * Render an arbitrary string with a given length at a position using a font-style.
 */
extern int render_formattedString(RENDER_CONTEXT* pRC, int x, int y, unsigned char* cBuf, size_t nLength, 
	FONT_STYLE_CLASS nStyle, FONT_STYLE_CLASS* pPreviousStyle);

/*
 * Render one line in HEX format. 
 */
static void render_hexLine(RENDER_CONTEXT* pCtx, int y, const char* pszBytes, int nBytes) {
	char szRender[HEX_BYTES_PER_LINE * 6];
	int  nHexOffs = 0;
	int  nAscOffs = HEX_BYTES_PER_LINE*3-1;

	szRender[nAscOffs++] = ' ';
	szRender[nAscOffs++] = ' ';
	szRender[nAscOffs++] = ' ';
	for (int i = 0; i < HEX_BYTES_PER_LINE; i++) {
		unsigned char b;
		if (i >= nBytes) {
			b = 0;
		} else {
			b = pszBytes[i];
		}
		if (b == 0) {
			szRender[nAscOffs++] = ' ';
			szRender[nHexOffs++] = '0';
			szRender[nHexOffs++] = '0';
		}
		else {
			szRender[nAscOffs++] = b < 32 ? '·' : b;
			int lo = b / 16;
			int hi = b % 16;
			szRender[nHexOffs++] = (lo > 9) ? ('A' + lo - 10) : '0' + lo;
			szRender[nHexOffs++] = (hi > 9) ? ('A' + hi - 10) : '0' + hi;
		}
		szRender[nHexOffs++] = ' ';
	}
	WINFO* wp = pCtx->rc_wp;
	TextOut(pCtx->rc_hdc, -(wp->mincol * wp->cwidth), y, szRender, nAscOffs);
}

/*
 * Get the line pointer for a wanted logical hex line to render. If the
 * method returns > 0, the Line pointer points to the corresponding line
 * and the pStartOffset contains the byte offset to the beginning of that line.
 */
static int hex_getLinePointerFor(WINFO* wp,long ln, LINE** pLine, long* pStartOffset, long* pLineOffset) {
	long nOffset = ln * HEX_BYTES_PER_LINE;
	FTABLE* fp = wp->fp;
	HEX_RENDERER_DATA* pData = wp->r_data;
	LINE* lp;
	long nStartOffset;

	if (pData->pByteOffsetCache) {
		lp = pData->pByteOffsetCache;
		nStartOffset = pData->nCachedByteOffset;
		while (nStartOffset > nOffset && lp->prev) {
			lp = lp->prev;
			nStartOffset -= ln_nBytes(lp);
		}
	} else {
		lp = fp->firstl;
		nStartOffset = 0;
	}
	while (lp) {
		long nBytes = ln_nBytes(lp);
		if (nStartOffset + nBytes > nOffset) {
			*pLineOffset = nOffset- nStartOffset;
			break;
		}
		nStartOffset += nBytes;
		lp = lp->next;
		if (lp == fp->lastl) {
			return (nStartOffset + HEX_BYTES_PER_LINE >= nOffset) ? -1 : 0;
		}
	}
	if (lp == NULL) {
		return 0;
	}
	*pStartOffset = nStartOffset;
	*pLine = lp;
	return 1;
}

/*
 * Get the next line of bytes from the current file to be rendered. 
 */
static int hex_getBytes(char* pszBuffer, WINFO* wp, long ln) {
	HEX_RENDERER_DATA* pData = wp->r_data;
	FTABLE* fp = wp->fp;
	long nOffset = ln * HEX_BYTES_PER_LINE;
	LINE* lp;
	long nStartOffset;
	long nLineOffset;

	int nResult = hex_getLinePointerFor(wp, ln, &lp, &nStartOffset, &nLineOffset);
	if (nResult <= 0) {
		return nResult;
	}
	pData->pByteOffsetCache = lp;
	pData->nCachedByteOffset = nStartOffset;
	int nCount = 0;
	int lnOffset = nOffset - nStartOffset;
	while (nCount < HEX_BYTES_PER_LINE) {
		if (lnOffset < lp->len) {
			pszBuffer[nCount++] = lp->lbuf[lnOffset++];
			continue;
		}
		BOOL bEnd = LINE_HAS_LINE_END(lp);
		BOOL bCr = LINE_HAS_CR(lp);
		if (lnOffset == lp->len) {
			if (bEnd) {
				if (bCr) {
					pszBuffer[nCount++] = '\r';
				} else {
					pszBuffer[nCount++] = '\n';
				}
				lnOffset++;
			}
		}
		if (lnOffset > lp->len && bEnd && bCr) {
			pszBuffer[nCount++] = '\n';
		}
		lp = lp->next;
		if (lp == NULL || lp == fp->lastl) {
			break;
		}
		lnOffset = 0;
	}
	if (nCount > HEX_BYTES_PER_LINE) {
		return HEX_BYTES_PER_LINE;
	}
	return nCount;
}

static void render_matchMarker(HDC hdc, THEME_DATA* pTheme, int xOffset, int y, int width, int height, int nColumn) {
	HBRUSH hBrush = CreateSolidBrush(pTheme->th_dialogBorder);

	RECT rect;
	rect.top = y;
	rect.bottom = y + height;
	int x;
	int nCharsMarked = 1;
	if (IS_IN_HEX_NUMBER_AREA(nColumn)) {
		x = nColumn / 3;
		x = 3 * HEX_BYTES_PER_LINE - 1 + HEX_ASCII_DISTANCE + x;
	} else {
		x = nColumn - (HEX_MAX_COL - HEX_BYTES_PER_LINE);
		x = x * 3;
		nCharsMarked = 2;
	}
	rect.left = xOffset + (x * width);
	rect.right = rect.left + (width*nCharsMarked);
	FrameRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}

 /*
  * Render the current window in hexadecimal display.
  */
void render_hexMode(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	WINFO* wp = pCtx->rc_wp;
	FTABLE* fp = FTPOI(wp);
	long ln;
	long newy;
	long min = wp->minln;
	long max = wp->maxln;
	int nLength;
	char szBuffer[HEX_BYTES_PER_LINE+5];
	RECT rect;
	RECT r;
	HBRUSH hBrush;
	HBRUSH hBrushCaretLine;

	int cheight = wp->cheight;
	GetClientRect(wp->ww_handle, &rect);
	hBrushCaretLine = CreateSolidBrush(pCtx->rc_theme->th_caretLineColor);
	for (ln = min; ln <= max && y < pClip->bottom; ln++, y = newy) {
		newy = y + cheight;
		if (newy > pClip->top) {
			nLength = hex_getBytes(szBuffer, wp, ln);
			if (nLength <= 0) {
				if (nLength == -1) {
					r.left = rect.left; r.right = rect.right;
					r.top = y;
					r.bottom = min(pClip->bottom, y + cheight);
					FillRect(pCtx->rc_hdc, &r, hBrushBg);
					FONT_STYLE_CLASS fsPreviousStyle;
					render_formattedString(pCtx, 0, y, fp->lastl->lbuf, fp->lastl->len,
						FS_CONTROL_CHARS, &fsPreviousStyle);
					y = newy;
				}
				break;
			}
			if (ln == wp->caret.ln && (wp->dispmode & SHOWCARET_LINE_HIGHLIGHT)) {
				hBrush = hBrushCaretLine;
			} else {
				hBrush = hBrushBg;
			}
			r.left = rect.left; r.right = rect.right;
			r.top = y;
			r.bottom = min(pClip->bottom, y + cheight);
			FillRect(pCtx->rc_hdc, &r, hBrush);
			render_hexLine(pCtx, y, szBuffer, nLength);
			if (wp->caret.ln == ln) {
				render_matchMarker(pCtx->rc_hdc, pCtx->rc_theme, -wp->mincol * wp->cwidth, y, wp->cwidth, wp->cheight, wp->caret.col);
			}
		}
	}

	r.left = rect.left; r.right = rect.right;
	r.top = y;
	r.bottom = min(pClip->bottom, y + (max - ln) * cheight);
	if (r.bottom > r.top) {
		FillRect(pCtx->rc_hdc, &r, hBrushBg);
	}
	DeleteObject(hBrushCaretLine);
}

static int hex_screenOffsetToBuffer(WINFO* wp, long ln, long col, INTERNAL_BUFFER_POS* pPosition) {
	long nOffset;
	if (hex_getLinePointerFor(wp, ln, &pPosition->ibp_lp, &nOffset, &pPosition->ibp_lineOffset) <= 0) {
		return 0;
	}
	if (IS_IN_HEX_NUMBER_AREA(col)) {
		pPosition->ibp_logicalColumnInLine = col / 3;
	} else {
		pPosition->ibp_logicalColumnInLine = col - (HEX_MAX_COL - HEX_BYTES_PER_LINE);
	}
	pPosition->ibp_byteOffset = nOffset + pPosition->ibp_logicalColumnInLine;
	long nLineOffset = pPosition->ibp_lineOffset;
	LINE* lp = pPosition->ibp_lp;
	for (long i = 0; i < pPosition->ibp_logicalColumnInLine; i++) {
		if (nLineOffset >= lp->len) {
			if (LINE_HAS_LINE_END(lp)) {
				if (LINE_HAS_CR(lp)) {
					i++;
				}
			} else {
				i--;
			}
			nLineOffset = 0;
			lp = lp->next;
			if (!lp) {
				return 0;
			}
			continue;
		}
		nLineOffset++;
	}
	pPosition->ibp_lineOffset = nLineOffset;
	pPosition->ibp_lp = lp;
	return 1;
}

/*
 * Caret movement in hex edit mode. 
 */
static int hex_placeCursorAndValidateDelta(WINFO* wp, long* ln, long offset, long* screenCol, int updateVirtualOffset, int xDelta) {
	int	  o;

	o = *screenCol;
	if (o < 0) {
		*screenCol = 0;
		o = 0;
	}

	if (o == HEX_MAX_COL && xDelta > 0) {
		o = 0;
		*ln = *ln + 1;
	}
	if (o > HEX_MAX_COL) {
		o = HEX_MAX_COL;
	}
	if (IS_IN_HEX_NUMBER_AREA(o) && (o % 3) == 2) {
		if (xDelta < 0) {
			o--;
		} else {
			o++;
		}
	}
	if (o < HEX_MAX_COL - HEX_BYTES_PER_LINE && o > HEX_MAX_COL - HEX_BYTES_PER_LINE - 3) {
		if (xDelta < 0) {
			o = HEX_MAX_COL - HEX_BYTES_PER_LINE-4;
		} else {
			o = HEX_MAX_COL - HEX_BYTES_PER_LINE;
		}
	}
	INTERNAL_BUFFER_POS ibp;
	if (!hex_screenOffsetToBuffer(wp, *ln, o, &ibp)) {
		return 0;
	}
	wp->caret.linePointer = ibp.ibp_lp;
	wp->caret.offset = ibp.ibp_lineOffset;
	if (updateVirtualOffset) {
		wp->caret.virtualOffset = o;
	}
	caret_moveToLine(wp, *ln);
	*screenCol = o;
	render_repaintCurrentLine(wp);
	return 1;
}

/*
 * Caret movement in hex edit mode.
 */
static int hex_placeCursorAndValidate(WINFO* wp, long* ln, long* col, int updateVirtualOffset) {
	return hex_placeCursorAndValidateDelta(wp, ln, *col, col, updateVirtualOffset, 0);
}

static long hex_calculateNLines(WINFO* wp) {
	FTABLE* fp = wp->fp;
	long nBytes = fp->nbytes;
	if (nBytes <= 0) {
		nBytes = 0;
		LINE* lp = fp->firstl;
		while (lp && lp != fp->lastl) {
			nBytes += ln_nBytes(lp);
			lp = lp->next;
		}
		fp->nbytes = nBytes;
	}
	return (nBytes + HEX_BYTES_PER_LINE - 1) / HEX_BYTES_PER_LINE;
}

static long hex_calculateMaxScreenColumn(WINFO* wp) {
	return HEX_MAX_COL - 1;
}

static long hex_calculateMaxColumn(WINFO* wp, long ln, LINE* lp) {
	return hex_calculateMaxScreenColumn(wp);
}

static void* hex_allocData(WINFO* wp) {
	return calloc(1, sizeof(HEX_RENDERER_DATA));
}

static void hex_modelChanged(WINFO* wp, MODEL_CHANGE* pChanged) {
	HEX_RENDERER_DATA *pData = wp->r_data;
	if (pChanged->type == LINE_MODIFIED || pChanged->type == LINE_REPLACED) {
		render_repaintWindow(wp);
		if (pData->pByteOffsetCache == pChanged->lp) {
			return;
		}
	}
	pData->pByteOffsetCache = NULL;
}

static int hex_rendererSupportsMode(int aMode) {
	if (aMode == SHOWCARET_LINE_HIGHLIGHT || aMode == SHOWRULER) {
		return 0;
	}
	return 1;
}

static RENDERER _hexRenderer = {
	render_singleLineOnDevice,
	render_hexMode,
	hex_placeCursorAndValidateDelta,
	hex_calculateNLines,
	hex_calculateMaxScreenColumn,
	hex_calculateMaxColumn,
	hex_placeCursorAndValidate,
	hex_screenOffsetToBuffer,
	hex_allocData,
	0,
	wt_scrollxy,
	ww_setScrollCheckBounds,
	hex_rendererSupportsMode,
	hex_modelChanged
};

/*
 * Returns a hex renderer. 
 */
RENDERER* hex_getRenderer() {
	return &_hexRenderer;
}

