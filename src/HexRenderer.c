/*
 * HexRenderer.c
 *
 * Project: PKS Edit for Windows
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
#include "editorfont.h"
#include "winfo.h"
#include "documentmodel.h"
#include "themes.h"
#include "trace.h"
#include "printing.h"

#define	HEX_BYTES_PER_LINE		32
#define HEX_ASCII_DISTANCE		3
#define	HEX_MAX_COL				4*HEX_BYTES_PER_LINE+HEX_ASCII_DISTANCE-1
#define IS_IN_HEX_NUMBER_AREA(col)	(col < 3*HEX_BYTES_PER_LINE)

extern int render_calculateScrollDelta(long val, long minval, long maxval, int scrollBy);

typedef struct tagHEX_RENDERER_DATA {
	LINE* pByteOffsetCache;	// Used to speed up the calculation of "byte offsets" into our line pointers (used in Hex editing)
	long	nCachedByteOffset;	// See above - only valid if pByteOffsetCache is not null
	long    nCaretLine;
	int     nCaretColumn;
} HEX_RENDERER_DATA;

/*
 * Move the caret in "HEX editing coordinates" - nLine and nCol relative to the hex-representation lines visible
 * on the screen.
 */
static void hex_moveCaretToHexCaretPosition(WINFO* wp, int nLine, int nCol);

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
			szRender[nAscOffs++] = ' ';
			szRender[nHexOffs++] = ' ';
			szRender[nHexOffs++] = ' ';
		} else {
			b = pszBytes[i];
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
		}
		szRender[nHexOffs++] = ' ';
	}
	const WINFO* wp = pCtx->rc_wp;
	TextOut(pCtx->rc_hdc, -(wp->mincol * wp->cwidth), y, szRender, nAscOffs);
}

static void hex_findLine(const WINFO* wp, long nOffset, LINE** pLinePointer, long* pStartOffset) {
	const FTABLE* fp = FTPOI(wp);
	*pLinePointer = fp->firstl;
	*pStartOffset = 0;
	HEX_RENDERER_DATA* pData = wp->r_data;
	LINE* lp = pData->pByteOffsetCache;
	if (lp) {
		if (nOffset > pData->nCachedByteOffset) {
			*pLinePointer = lp;
			*pStartOffset = pData->nCachedByteOffset;
		} else if (nOffset > pData->nCachedByteOffset / 2) {
			while (lp) {
				lp = lp->prev;
				if (!lp) {
					break;
				}
				pData->nCachedByteOffset -= ln_nBytes(lp);
				if (nOffset > pData->nCachedByteOffset) {
					break;
				}
			}
			pData->pByteOffsetCache = lp;
			*pLinePointer = lp;
			*pStartOffset = pData->nCachedByteOffset;
		}
	}
}

/*
 * Get the line pointer for a wanted logical hex line to render. If the
 * method returns > 0, the Line pointer points to the corresponding line
 * and the pStartOffset contains the byte offset to the beginning of that line.
 */
static int hex_getLinePointerFor(const WINFO* wp,long ln, LINE** pLine, long* pStartOffset, long* pLineOffset) {
	long nOffset = ln * HEX_BYTES_PER_LINE;
	const FTABLE* fp = wp->fp;
	HEX_RENDERER_DATA* pData = wp->r_data;
	LINE* lp;
	long nStartOffset;

	hex_findLine(wp, nOffset, &lp, &nStartOffset);
	while (lp) {
		long nBytes = ln_nBytes(lp);
		if (nStartOffset + nBytes > nOffset) {
			pData->nCachedByteOffset = nStartOffset;
			pData->pByteOffsetCache = lp;
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
static int hex_getBytes(LINE* lp, int nStartOffset, char* pszBuffer, const WINFO* wp, long ln) {
	long nOffset = ln * HEX_BYTES_PER_LINE;
	HEX_RENDERER_DATA* pData = wp->r_data;
	const FTABLE* fp = wp->fp;

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

	RECT rect = {
		.top = y,
		.bottom = y + height
	};
	int x;
	int nCharsMarked = 1;
	x = HEX_MAX_COL - HEX_BYTES_PER_LINE + nColumn;
	rect.left = xOffset + (x * width);
	rect.right = rect.left + (width*nCharsMarked);
	FrameRect(hdc, &rect, hBrush);
	DeleteObject(hBrush);
}

static BOOL render_hexSelection(HDC hdc, const WINFO* wp, int y, LINE* lp, int nLineOffset, const LINE* lpSelection, int nStartColumn, int nNumberOfCharsSelected) {
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
	RECT rect = {
		.top = y,
		.bottom = y + cHeight
	};
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
static void render_hexModeFromTo(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y, long min, long max) {
	const WINFO* wp = pCtx->rc_wp;
	const FTABLE* fp = FTPOI(wp);
	long ln;
	int nLength;
	char szBuffer[HEX_BYTES_PER_LINE+5];
	RECT rect;
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
	if (pCtx->rc_printing) {
		FillRect(pCtx->rc_hdc, pClip, hBrushBg);
	} else if (wp->blstart && wp->blend && wp->blstart->m_linePointer == wp->blend->m_linePointer) {
		lpSelection = wp->blend->m_linePointer;
		nSelectionStart = wp->blstart->m_column;
		nSelectionSize = wp->blend->m_column-wp->blstart->m_column;
	}
	else {
		nSelectionSize = 0;
		lpSelection = 0;
		nSelectionStart = -1;
	}
	long newy = 0;
	for (ln = min; ln <= max && y < pClip->bottom; ln++, y = newy) {
		newy = y + cheight;
		if (newy > pClip->top) {
			long nStartOffset;
			long nLineOffset;
			int nResult = hex_getLinePointerFor(wp, ln, &lp, &nStartOffset, &nLineOffset);
			nLength = nResult <= 0 ? -1 : hex_getBytes(lp, nStartOffset, szBuffer, wp, ln);
			if (nLength <= 0) {
				if (nLength == -1) {
					RECT r = {
						.left = rect.left,
						.right = rect.right,
						.top = y
					};
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
			RECT r = {
				.left = rect.left,
				.right = rect.right,
				.top = y
			};
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

	RECT r = {
		.left = rect.left,
		.right = rect.right,
		.top = y
	};
	r.bottom = min(pClip->bottom, y + (max - ln) * cheight);
	if (r.bottom > r.top) {
		FillRect(pCtx->rc_hdc, &r, hBrushBg);
	}
	DeleteObject(hBrushCaretLine);
}

/*
 * Render the current window in hexadecimal display.
 */
void render_hexMode(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	WINFO* wp = pCtx->rc_wp;
	render_hexModeFromTo(pCtx, pClip, hBrushBg, y, wp->minln, wp->maxln);
}

static void hex_bufferOffsetToScreen(WINFO* wp, CARET* pBufferCaret, long* pLine, long* pCol) {
	FTABLE* fp = wp->fp;
	LINE* lp = fp->firstl;
	long nTotalOffset = 0;
	HEX_RENDERER_DATA* pData = wp->r_data;
	if (pData->pByteOffsetCache == wp->caret.linePointer) {
		nTotalOffset = pData->nCachedByteOffset;
		lp = pData->pByteOffsetCache;
	}
	while (lp && lp != fp->lastl) {
		if (lp == wp->caret.linePointer) {
			nTotalOffset += wp->caret.offset;
			*pLine = nTotalOffset / HEX_BYTES_PER_LINE;
			*pCol = nTotalOffset % HEX_BYTES_PER_LINE;
			return;
		}
		nTotalOffset += ln_nBytes(lp);
		lp = lp->next;
	}
}

static int hex_screenOffsetToBufferPos(WINFO* wp, long ln, long col, INTERNAL_BUFFER_POS* pPosition) {
	pPosition->ibp_logicalColumnInLine = col;
	pPosition->ibp_lineOffset = 0;
	long nTotal = ln * HEX_BYTES_PER_LINE + pPosition->ibp_logicalColumnInLine;
	long nCurrentTotal = 0;
	LINE* lp;
	hex_findLine(wp, nTotal, &lp, &nCurrentTotal);
	FTABLE* fp = FTPOI(wp);
	int found = 0;
	while (lp && lp != fp->lastl) {
		long nBytes = ln_nBytes(lp);
		if (nCurrentTotal + nBytes > nTotal) {
			pPosition->ibp_lineOffset = nTotal-nCurrentTotal;
			found = 1;
			break;
		}
		nCurrentTotal += nBytes;
		lp = lp->next;
	}
	pPosition->ibp_byteOffset = nTotal;
	pPosition->ibp_lp = lp;
	return found;
}

static int hex_screenOffsetToBuffer(WINFO* wp, INTERNAL_BUFFER_POS* pPosition) {
	HEX_RENDERER_DATA* pData = wp->r_data;
	return hex_screenOffsetToBufferPos(wp, pData->nCaretLine, pData->nCaretColumn, pPosition);
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
	*pCY = (pData->nCaretLine - wp->minln) * wp->cheight;
	if (nNewCol != *pCX) {
		*pCX = nNewCol;
		if (*pCY == nOldY) {
			hex_repaintScreenLine(wp, pData->nCaretLine);
		}
	}
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

/*
 * Move the caret in "HEX editing coordinates" - nLine and nCol relative to the hex-representation lines visible
 * on the screen.
 */
static void hex_moveCaretToHexCaretPosition(WINFO* wp, int nLine, int nCol) {
	HEX_RENDERER_DATA* pData = wp->r_data;
	FTABLE* fp = FTPOI(wp);
	while (nCol < 0) {
		nCol += HEX_BYTES_PER_LINE;
		nLine--;
	}
	while (nCol >= HEX_BYTES_PER_LINE) {
		nCol -= HEX_BYTES_PER_LINE;
		nLine++;
	}
	if (nLine < 0) {
		return;
	}
	long nMax = hex_calculateNLines(wp);
	if (nLine >= nMax) {
		return;
	}
	INTERNAL_BUFFER_POS position;
	if (!hex_screenOffsetToBufferPos(wp, nLine, nCol, &position)) {
		return;
	}
	if (wp->caret.linePointer != position.ibp_lp) {
		if (wp->caret.linePointer == NULL) {
			wp->caret.linePointer = fp->firstl;
			wp->caret.ln = 0;
		}
		if (nLine > pData->nCaretLine) {
			wp->caret.ln += ln_cnt(wp->caret.linePointer, position.ibp_lp) - 1;
		} else {
			wp->caret.ln -= ln_cnt(position.ibp_lp, wp->caret.linePointer) - 1;
		}
		wp->caret.linePointer = position.ibp_lp;
	}
	wp->caret.offset = position.ibp_lineOffset;
	if (wp->caret.offset > position.ibp_lp->len) {
		wp->caret.offset = position.ibp_lp->len;
	}
	long nOld = pData->nCaretLine;
	pData->nCaretLine = nLine;
	pData->nCaretColumn = nCol;
	if (nOld != nLine) {
		hex_repaintScreenLine(wp, nLine);
		hex_repaintScreenLine(wp, nOld);
	}
}

static void hex_getLineAnnotation(WINFO* wp, long nLine, LINE_ANNOTATION* pAnnotation) {
	wsprintf(pAnnotation->la_text, "%08x", nLine * HEX_BYTES_PER_LINE);
	pAnnotation->la_lineFlag = 0;
}

/*
 * Caret movement in hex edit mode. 
 */
static int hex_placeCaret(WINFO* wp, long* ln, long offset, long* screenCol, int updateVirtualOffset, CARET_MOVEMENT_SPEC* pMovementSpec) {
	long nLine;
	long nCol;
	HEX_RENDERER_DATA* pData = wp->r_data;
	if (!pMovementSpec || pMovementSpec->cms_absolute) {
		FTABLE* fp = FTPOI(wp);
		wp->caret.offset = offset;
		wp->caret.ln = *ln;
		wp->caret.linePointer = ln_goto(fp, *ln);
		hex_bufferOffsetToScreen(wp, &wp->caret, &nLine, &nCol);
	} else {
		nLine = pData->nCaretLine;
		nCol = pData->nCaretColumn;
		if (pMovementSpec->cms_movementY == CRMT_START) {
			nLine = 0;
			nCol = 0;
		} else if (pMovementSpec->cms_movementY == CRMT_END) {
			nLine = hex_calculateNLines(wp) - 1;
			nCol = 0;
		} else if (pMovementSpec->cms_movementY == CRMT_SECTION_DOWN) {
			nLine += wp->maxln-wp->minln;
			long nMax = hex_calculateNLines(wp) - 1;
			if (nLine > nMax) {
				nLine = nMax;
			}
		} else if (pMovementSpec->cms_movementY == CRMT_SECTION_UP) {
			nLine -= wp->maxln - wp->minln;
			if (nLine < 0) {
				nLine = 0;
			}
		} else {
			nLine += pMovementSpec->cms_deltaY;
		}
		if (pMovementSpec->cms_movementX == CRMT_START) {
			nCol = 0;
		}
		else if (pMovementSpec->cms_movementX == CRMT_END) {
			nCol = HEX_BYTES_PER_LINE - 1;
		}
		else {
			nCol += pMovementSpec->cms_deltaX;
		}
	}
	hex_moveCaretToHexCaretPosition(wp, nLine, nCol);
	// screen column not used in context of hex renderer
	*screenCol = 0;
	wp->caret.col = 0;
	*ln = wp->caret.ln;
	return 1;
}

/*
 * Caret movement in hex edit mode.
 */
static int hex_placeCursorAndValidate(WINFO* wp, long* ln, long* col, int updateVirtualOffset) {
	hex_moveCaretToHexCaretPosition(wp, *ln, *col);
	*ln = wp->caret.ln;
	return 1;
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

static int hex_rendererSupportsMode(EDIT_MODE aMode) {
	if (aMode.em_displayMode && aMode.em_flag == SHOW_RULER) {
		return 0;
	}
	if (!aMode.em_displayMode) {
		return (aMode.em_flag & (WM_AUTOFORMAT | WM_AUTOINDENT | WM_ABBREV | WM_AUTOWRAP | WM_SHOWMATCH | WM_COLUMN_SELECTION)) == 0;
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

/*
 * Default printing implementation to print in standard ASCII mode.
 */
PRINT_FRAGMENT_RESULT hex_print(RENDER_CONTEXT* pRC, PRINT_LINE* printLineParam, DEVEXTENTS* pExtents) {
	HDC hdc = pRC->rc_hdc;
	WINFO* wp = pRC->rc_wp;
	int nLinesPerScreen = 56;
	long nMax = printLineParam->yOffset + nLinesPerScreen;
	long nMaxLines = hex_calculateNLines(wp);
	if (nMax > nMaxLines) {
		nMax = nMaxLines;
	}
	RECT rect = {
		.left = pExtents->xLMargin,
		.right = pExtents->xRMargin,
		.top = pExtents->yTop,
		.bottom = pExtents->yBottom
	};
	int nOldDC = SaveDC(hdc);
	HBRUSH hBrush = CreateSolidBrush(pRC->rc_theme->th_defaultBackgroundColor);
	IntersectClipRect(hdc, rect.left, rect.top, rect.right, rect.bottom);
	render_hexModeFromTo(pRC, &rect, hBrush, printLineParam->yPos, printLineParam->yOffset, nMax);
	DeleteObject(hBrush);
	RestoreDC(hdc, nOldDC);
	printLineParam->yOffset = nMax;
	if (nMax < nMaxLines) {
		printLineParam->pagenumber++;
	}
	printLineParam->yPos = 0;
	return nMax >= nMaxLines ? PFR_END : PFR_END_PAGE;
}

static int hex_adjustScrollBounds(WINFO* wp) {
	HEX_RENDERER_DATA* pData = wp->r_data;
	long nLine = pData->nCaretLine;
	long nCol = pData->nCaretColumn;
	long nPrevious = wp->caret.ln;
	wp->caret.ln = nLine;
	int ret = render_adjustScrollBounds(wp);
	wp->caret.ln = nPrevious;
	if (ret) {
		return 1;
	}
	int dx = 0;
	int dy = render_calculateScrollDelta(nLine, wp->mincursln, wp->maxcursln, wp->vscroll);
	if (dy) {
		if (sl_scrollwinrange(wp, &dy, &dx))
			sl_winchanged(wp, dy, dx);
		return 1;
	}
	return 0;
}

/*---------------------------------*
 * hex_calculateOffsetFromScreen()
 * Calculate the offset in the file (line and column) given the X and Y coordinates
 * on which a mouse was clicked. No validation regarding the validity of the column
 * is performed at that point in time.
 */
static void hex_calculateOffsetFromScreen(WINFO* wp, int x, int y, long* line, long* column) {
	int nCol = (x + wp->cwidth / 2)  / wp->cwidth + wp->mincol;
	if (IS_IN_HEX_NUMBER_AREA(nCol)) {
		nCol = nCol / 3;
	} else {
		nCol = nCol - (HEX_MAX_COL - HEX_BYTES_PER_LINE);
	}
	*column = nCol;
	*line = y / wp->cheight + wp->minln;
	if (*line < 0) {
		*line = 0;
	} else {
		long nlines = hex_calculateNLines(wp);
		if (*line >= nlines) {
			*line = nlines - 1;
		}
	}
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
	hex_adjustScrollBounds,
	.r_updateCaretUI = hex_updateCaretUI,
	hex_rendererSupportsMode,
	hex_calculateOffsetFromScreen,
	TRUE,
	hex_modelChanged,
	.r_repaint= hex_repaint,
	.r_printFragment = hex_print,
	.r_getLineAnnotation = hex_getLineAnnotation
};

/*
 * Returns a hex renderer. 
 */
RENDERER* hex_getRenderer() {
	return &_hexRenderer;
}

