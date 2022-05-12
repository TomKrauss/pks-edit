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
#include "trace.h"

#define	HEX_BYTES_PER_LINE		32
#define HEX_ASCII_DISTANCE		3
#define	HEX_MAX_COL				4*HEX_BYTES_PER_LINE+HEX_ASCII_DISTANCE-1
#define IS_IN_HEX_NUMBER_AREA(col)	(col < 3*HEX_BYTES_PER_LINE)

typedef struct tagHEX_RENDERER_DATA {
	LINE* pByteOffsetCache;	// Used to speed up the calculation of "byte offsets" into our line pointers (used in Hex editing)
	long	nCachedByteOffset;	// See above - only valid if pByteOffsetCache is not null
	long    nCaretLine;
	int     nCaretColumn;
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
	*pLine = lp;
	if (lp == NULL) {
		return 0;
	}
	*pStartOffset = nStartOffset;
	return 1;
}

/*
 * Get the next line of bytes from the current file to be rendered. 
 */
static int hex_getBytes(LINE* lp, int nStartOffset, char* pszBuffer, WINFO* wp, long ln) {
	long nOffset = ln * HEX_BYTES_PER_LINE;
	HEX_RENDERER_DATA* pData = wp->r_data;
	FTABLE* fp = wp->fp;

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
	x = HEX_MAX_COL - HEX_BYTES_PER_LINE + nColumn;
	rect.left = xOffset + (x * width);
	rect.right = rect.left + (width*nCharsMarked);
	FrameRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}

static BOOL render_hexSelection(HDC hdc, WINFO* wp, int y, LINE* lp, int nLineOffset, LINE* lpSelection, int nStartColumn, int nNumberOfCharsSelected) {
	int nByteOffset = 0;
	while (lp) {
		if (lp == lpSelection) {
			nByteOffset += nStartColumn - nLineOffset;
			if (nByteOffset + nNumberOfCharsSelected < 0) {
				return TRUE;
			}
			break;
		}
		nByteOffset += (ln_nBytes(lp) - nLineOffset);
		if (nByteOffset >= HEX_BYTES_PER_LINE) {
			return FALSE;
		}
		nLineOffset = 0;
		lp = lp->next;
	}
	int xOffset = -wp->mincol * wp->cwidth;
	int cWidth = wp->cwidth;
	int cHeight = wp->cheight;
	RECT rect;
	rect.top = y;
	rect.bottom = y + cHeight;
	int nMax = nByteOffset + nNumberOfCharsSelected;
	BOOL nRet = TRUE;
	if (nMax > HEX_BYTES_PER_LINE) {
		nMax = HEX_BYTES_PER_LINE;
		nRet = FALSE;
	}
	for (int i = nByteOffset; i < nMax; i++) {
		if (i >= 0) {
			rect.left = xOffset + (3 * i * cWidth);
			rect.right = rect.left + (2 * cWidth);
			render_paintSelectionRect(hdc, &rect);
		}
	}
	if (nByteOffset < 0) {
		nByteOffset = 0;
	}
	rect.left = xOffset + (HEX_MAX_COL - HEX_BYTES_PER_LINE + nByteOffset) * cWidth;
	rect.right = rect.left + ((nMax -nByteOffset) * cWidth);
	render_paintSelectionRect(hdc, &rect);
	return nRet;
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
	LINE* lp;
	LINE* lpSelection;
	int nSelectionStart;
	int nSelectionSize;
	HBRUSH hBrush;
	HBRUSH hBrushCaretLine;
	HEX_RENDERER_DATA* pData = wp->r_data;

	int cheight = wp->cheight;
	GetClientRect(wp->ww_handle, &rect);
	hBrushCaretLine = CreateSolidBrush(pCtx->rc_theme->th_caretLineColor);
	if (wp->blstart && wp->blend && wp->blstart->m_linePointer == wp->blend->m_linePointer) {
		lpSelection = wp->blend->m_linePointer;
		nSelectionStart = wp->blstart->m_column;
		nSelectionSize = wp->blend->m_column-wp->blstart->m_column;
	}
	else {
		nSelectionSize = 0;
		lpSelection = 0;
		nSelectionStart = -1;
	}
	for (ln = min; ln <= max && y < pClip->bottom; ln++, y = newy) {
		newy = y + cheight;
		if (newy > pClip->top) {
			long nStartOffset;
			long nLineOffset;
			int nResult = hex_getLinePointerFor(wp, ln, &lp, &nStartOffset, &nLineOffset);
			nLength = nResult <= 0 ? -1 : hex_getBytes(lp, nStartOffset, szBuffer, wp, ln);
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
			if (ln == pData->nCaretLine && (wp->dispmode & SHOW_CARET_LINE_HIGHLIGHT)) {
				hBrush = hBrushCaretLine;
			} else {
				hBrush = hBrushBg;
			}
			r.left = rect.left; r.right = rect.right;
			r.top = y;
			r.bottom = min(pClip->bottom, y + cheight);
			FillRect(pCtx->rc_hdc, &r, hBrush);
			render_hexLine(pCtx, y, szBuffer, nLength);
			if (pData->nCaretLine == ln) {
				render_matchMarker(pCtx->rc_hdc, pCtx->rc_theme, -wp->mincol * wp->cwidth, y, wp->cwidth, wp->cheight, pData->nCaretColumn);
			}
			if (lpSelection) {
				if (render_hexSelection(pCtx->rc_hdc, wp, y, lp, nLineOffset, lpSelection, nSelectionStart, nSelectionSize)) {
					lpSelection = 0;
				}
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

static void hex_bufferOffsetToScreen(WINFO* wp, CARET* pBufferCaret, long* pLine, long* pCol) {
	FTABLE* fp = wp->fp;
	LINE* lp = fp->firstl;
	long nLine = 0;
	long nTotalOffset = 0;
	while (lp && lp != fp->lastl) {
		if (lp == wp->caret.linePointer) {
			nTotalOffset += wp->caret.offset;
			nLine += nTotalOffset / HEX_BYTES_PER_LINE;
			*pLine = nLine;
			*pCol = nTotalOffset % HEX_BYTES_PER_LINE;
			return;
		}
		nTotalOffset += ln_nBytes(lp);
		nLine += nTotalOffset / HEX_BYTES_PER_LINE;
		nTotalOffset %= HEX_BYTES_PER_LINE;
		lp = lp->next;
	}
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

static void hex_repaintScreenLine(WINFO* wp, long nLine) {
	RECT r;
	GetClientRect(wp->ww_handle, &r);
	r.top = (nLine - wp->minln) * wp->cheight;
	r.bottom = r.top + wp->cheight;
	render_invalidateRect(wp, &r);
}

static void hex_updateCaretUI(WINFO* wp, int* pCX, int* pCY, int* pWidth, int* pHeight) {
	HEX_RENDERER_DATA* pData = wp->r_data;
	int nNewCol = pData->nCaretColumn * 3 * wp->cwidth;
	int nOldY = *pCY;
	EdTRACE(log_errorArgs(DEBUG_TRACE, "Setting caret x to %d (cwidth==%d)", *pCX, wp->cwidth));
	*pCY = pData->nCaretLine * wp->cheight;
	if (nNewCol != *pCX) {
		*pCX = nNewCol;
		if (*pCY == nOldY) {
			hex_repaintScreenLine(wp, pData->nCaretLine);
		}
	}
}

/*
 * Caret movement in hex edit mode. 
 */
static int hex_placeCaret(WINFO* wp, long* ln, long offset, long* screenCol, int updateVirtualOffset, int xDelta) {
	if (!caret_placeCursorAndValidate(wp, ln, offset, screenCol, updateVirtualOffset, xDelta)) {
		return 0;
	}
	if (wp->caret.offset > wp->caret.linePointer->len) {
		// TODO: handle insertion at end of file.
		wp->caret.offset = wp->caret.linePointer->len;
	}
	long nLine;
	long nCol;
	hex_bufferOffsetToScreen(wp, &wp->caret, &nLine, &nCol);
	HEX_RENDERER_DATA* pData = wp->r_data;
	if (pData->nCaretLine != nLine) {
		hex_repaintScreenLine(wp, pData->nCaretLine);
	}
	pData->nCaretLine = nLine;
	pData->nCaretColumn = nCol;
	return 1;
}

/*
 * Caret movement in hex edit mode.
 */
static int hex_placeCursorAndValidate(WINFO* wp, long* ln, long* col, int updateVirtualOffset) {
	return hex_placeCaret(wp, ln, *col, col, updateVirtualOffset, 0);
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
	if (aMode == SHOW_RULER) {
		return 0;
	}
	return 1;
}

static int hex_repaint(WINFO* wp, int ln1, int ln2, int col1, int col2) {
	HEX_RENDERER_DATA* pData = wp->r_data;
	if (wp->ww_handle && pData) {
		if (ln1 == ln2) {
			long nMin = wp->minln;
			long nMax = wp->maxln;
			FTABLE* fp = wp->fp;
			LINE* lp1 = ln_goto(fp, ln1);
			long nLine;
			long nCol;
			hex_bufferOffsetToScreen(wp, &(CARET) {.linePointer = lp1, .offset = col1}, & nLine, & nCol);
			if (nLine < nMin || nLine > nMax) {
				return 0;
			}
			hex_repaintScreenLine(wp, nLine);
		}
		else {
			render_invalidateRect(wp, NULL);
		}
		return 1;
	}
	return 0;
}


static RENDERER _hexRenderer = {
	render_singleLineOnDevice,
	render_hexMode,
	.r_placeCaret = hex_placeCaret,
	hex_calculateNLines,
	hex_calculateMaxScreenColumn,
	hex_calculateMaxColumn,
	hex_placeCursorAndValidate,
	hex_screenOffsetToBuffer,
	hex_allocData,
	0,
	wt_scrollxy,
	render_adjustScrollBounds,
	.r_updateCaretUI = hex_updateCaretUI,
	hex_rendererSupportsMode,
	caret_calculateOffsetFromScreen,
	TRUE,
	hex_modelChanged,
	.r_repaint= hex_repaint
};

/*
 * Returns a hex renderer. 
 */
RENDERER* hex_getRenderer() {
	return &_hexRenderer;
}

