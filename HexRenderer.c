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

#include <windows.h>
#include "caretmovement.h"
#include "winfo.h"
#include "lineoperations.h"
#include "themes.h"

#define	HEX_BYTES_PER_LINE		32
#define	HEX_MAX_COL				4*HEX_BYTES_PER_LINE+2

/*-----------------------------------------
 * Render an arbitrary string with a given length at a position using a font-style.
 */
extern int render_formattedString(RENDER_CONTEXT* pRC, int x, int y, unsigned char* cBuf, size_t nLength, 
	FONT_STYLE_CLASS nStyle, FONT_STYLE_CLASS* pPreviousStyle);

/*
 * Render one line in HEX format. 
 */
static void render_hexLine(RENDER_CONTEXT* pCtx, int y, char* pszBytes, int nBytes) {
	char szRender[HEX_BYTES_PER_LINE * 6];
	int  nHexOffs = 0;
	int  nAscOffs = HEX_BYTES_PER_LINE*3-1;

	szRender[nAscOffs++] = ' ';
	szRender[nAscOffs++] = ' ';
	szRender[nAscOffs++] = ' ';
	for (int i = 0; i < HEX_BYTES_PER_LINE; i++) {
		char b;
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
static int hex_getLinePointerFor(FTABLE* fp, long ln, LINE** pLine, long* pStartOffset) {
	long nOffset = ln * HEX_BYTES_PER_LINE;
	LINE* lp;
	long nStartOffset;

	if (fp->pByteOffsetCache) {
		lp = fp->pByteOffsetCache;
		nStartOffset = fp->nCachedByteOffset;
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
static int hex_getBytes(char* pszBuffer, FTABLE* fp, long ln) {
	long nOffset = ln * HEX_BYTES_PER_LINE;
	LINE* lp;
	long nStartOffset;

	int nResult = hex_getLinePointerFor(fp, ln, &lp, &nStartOffset);
	if (nResult <= 0) {
		return nResult;
	}
	fp->pByteOffsetCache = lp;
	fp->nCachedByteOffset = nStartOffset;
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
			lnOffset++;
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
			nLength = hex_getBytes(szBuffer, fp, ln);
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

/*
 * Caret movement in hex edit mode. 
 */
static int hex_placeCursorAndValidate(WINFO* wp, long* ln, long* col, int updateVirtualOffset) {
	int	  o;
	FTABLE* fp = wp->fp;

	o = *col;
	if (o < 0) {
		*col = 0;
		o = 0;
	}

	if (o > HEX_MAX_COL) {
		o = HEX_MAX_COL;
	}
	LINE* lp;
	long nOffset;
	if (hex_getLinePointerFor(fp, *ln, &lp, &nOffset) <= 0) {
		return 0;
	}
	wp->caret.linePointer = lp;
	wp->caret.offset = o;
	if (updateVirtualOffset) {
		wp->caret.virtualOffset = o;
	}
	caret_moveToLine(wp, *ln);
	*col = o;

	return 1;
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

static long hex_calculateMaxColumn(WINFO* wp, long ln, LINE* lp) {
	return HEX_MAX_COL;
}


static RENDERER _hexRenderer = {
	render_singleLineOnDevice,
	render_hexMode,
	hex_placeCursorAndValidate,
	hex_calculateNLines,
	hex_calculateMaxColumn,
	hex_placeCursorAndValidate
};

/*
 * Returns a hex renderer. 
 */
RENDERER* hex_getRenderer() {
	return &_hexRenderer;
}

