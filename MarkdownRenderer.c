/*
 * MarkdownRenderer.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: A special renderer for markdown files.
 *
 * 										created: 05.02.89
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include <Windows.h>

#include "winfo.h"
#include "caretmovement.h"
#include "linkedlist.h"
#include "editorfont.h"
#include "stringutil.h"
#include "themes.h"

#define NO_COLOR		-1

typedef struct tagRENDER_VIEW_PART RENDER_VIEW_PART;

typedef void (*RENDER_PAINT)(WINFO* wp, RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed);

/*
 * The text style of a "text run"
 */
typedef struct tagFONT_ATTRIBUTES {
	int		strikeout;
	int		italic;
	int		size;
	int		underline;
	int		weight;
	COLORREF bgColor;
	int		fixedFont;
} FONT_ATTRIBUTES;

typedef struct tagTEXT_RUN {
	struct tagTEXT_RUN* tr_next;
	size_t tr_size;
	FONT_ATTRIBUTES tr_attributes;
} TEXT_RUN;

typedef struct tagTEXT_FLOW {
	char* tf_text;
	TEXT_RUN* tf_runs;
} TEXT_FLOW;

typedef enum { 
	MET_BLOCK_QUOTE, 
	MET_ORDERED_LIST, 
	MET_UNORDERED_LIST, 
	MET_IMAGE, 
	MET_NORMAL, 
	MET_HEADER, 
	MET_HORIZONTAL_RULE 
} MDR_ELEMENT_TYPE;

struct tagRENDER_VIEW_PART {
	struct tagRENDER_VIEW_PART* rvp_next;
	LINE* rvp_lpStart;
	RENDER_PAINT rvp_paint;
	int	rvp_marginTop;
	int	rvp_marginBottom;
	int	rvp_marginLeft;
	int	rvp_marginRight;
	MDR_ELEMENT_TYPE rvp_type;
	int	rvp_level;				// for headers and lists - the level.
	union {
		TEXT_FLOW flow;
		struct tagRENDER_VIEW_PART* children;
	} rvp_e;
};

typedef struct tagMDR_ELEMENT_FORMAT {
	int	mef_marginTop;
	int	mef_marginBottom;
	int	mef_marginLeft;
	int	mef_marginRight;
	int mef_charHeight;
	int mef_charWeight;
} MDR_ELEMENT_FORMAT;

#define ATTR_EMPHASIS	0x1
#define ATTR_STRONG		0x2
#define ATTR_STRIKE		0x4
#define ATTR_CODE		0x8

static MDR_ELEMENT_FORMAT _formatText = {
	10, 10, 10, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatBlockQuote = {
	10, 10, 40, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatListLevel1 = {
	1, 1, 45, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatListLevel2 = {
	1, 1, 80, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatH1 = {
	20, 14, 10, 20, 28, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH2 = {
	16, 10, 10, 20, 24, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH3 = {
	10, 5, 10, 20, 20, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH4 = {
	10, 3, 10, 20, 18, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH5 = {
	10, 3, 10, 20, 16, FW_BOLD
};

typedef struct tagMARKDOWN_RENDERER_DATA {
	RENDER_VIEW_PART* md_pElements;	// The render view parts to be rendered
	int md_nElementsPerPage;
} MARKDOWN_RENDERER_DATA;

/*
 * Create a font for the given font-attributes. 
 */
static HFONT mdr_createFont(const FONT_ATTRIBUTES* pAttrs, float fZoom) {
	LOGFONT _lf = {
		12,					// lfHeight;
		0,					// lfWidth;
		0,					// lfEscapement;
		0,					// lfOrientation;
		FW_NORMAL,			// lfWeight;

		0,					// lfItalic;
		0,					// lfUnderline;
		0,					// lfStrikeOut;

		ANSI_CHARSET,		// lfCharSet;
		OUT_DEVICE_PRECIS,	// lfOutPrecision;
		CLIP_DEFAULT_PRECIS,// lfClipPrecision;
		ANTIALIASED_QUALITY,// lfQuality;
		FF_DONTCARE, 		// lfPitchAndFamily;
		"Verdana"			// lfFaceName[LF_FACESIZE];
	};
	_lf.lfHeight = (long)(pAttrs->size * fZoom);
	if (pAttrs->fixedFont) {
		strcpy(_lf.lfFaceName, "Consolas");
		_lf.lfWeight = FW_BOLD;
	}
	if (pAttrs->weight) {
		_lf.lfWeight = pAttrs->weight;
	}
	_lf.lfItalic = pAttrs->italic;
	_lf.lfStrikeOut = pAttrs->strikeout;
	_lf.lfUnderline = pAttrs->underline;
	return CreateFontIndirect(&_lf);
}

static void mdr_paintRule(HDC hdc, int left, int right, int y) {
	THEME_DATA* pTheme = theme_getCurrent();
	HPEN hPen = CreatePen(PS_SOLID, 1, pTheme->th_dialogLightBackground);
	HPEN hPenOld = SelectObject(hdc, hPen);
	MoveTo(hdc, left, y);
	LineTo(hdc, right, y);
	DeleteObject(SelectObject(hdc, hPenOld));
}

static void mdr_renderHorizontalRule(WINFO* wp, RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed) {
	mdr_paintRule(hdc, pBounds->left + 10, pBounds->right - 10, pBounds->top + 5);
	pUsed->top = pBounds->top;
	pUsed->left = pBounds->left;
	pUsed->bottom = pBounds->top + 10;
	pUsed->right = pBounds->right;
}

/*
 * Render a "text flow" - a simple text which contains styled regions aka text runs.
 */
static void mdr_renderTextFlow(WINFO* wp, RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed) {
	THEME_DATA* pTheme = theme_getCurrent();
	int x = pBounds->left + pPart->rvp_marginLeft;
	int y = pBounds->top + pPart->rvp_marginTop;
	int nRight = pBounds->right - pPart->rvp_marginRight;
	pUsed->top = y;
	pUsed->left = x;
	pUsed->right = nRight;
	TEXT_FLOW* pTF = &pPart->rvp_e.flow;
	TEXT_RUN* pTR = pTF->tf_runs;
	int nOffs = 0;
	size_t nLen;
	nLen = pTR->tr_size;
	int nHeight = 0;
	font_setDefaultTextColors(hdc, pTheme);
	HFONT hFont = mdr_createFont(&pTR->tr_attributes, wp->zoomFactor);
	HFONT hOldFont = SelectObject(hdc, hFont);
	if (pPart->rvp_type == MET_UNORDERED_LIST) {
		TextOutW(hdc, x-15, y, pPart->rvp_level == 1 ? L"\u25CF" : L"\u25A0", 1);
	}
	while (nLen > 0 && string_isSpace(pTF->tf_text[nOffs])) {
		nOffs++;
		nLen--;
	}
	while(pTR) {
		SIZE size;
		int nFit;
		GetTextExtentExPoint(hdc, &pTF->tf_text[nOffs], (int)nLen, nRight - x, &nFit, 0 /* TODO: callback for measuring*/, &size);
		if (size.cy > nHeight) {
			nHeight = size.cy;
		}
		if (nFit < nLen) {
			int nFindSpace = 0;
			while (nFit + nFindSpace > 0 && !string_isSpace(pTF->tf_text[nOffs + nFit + nFindSpace])) {
				nFindSpace--;
			}
			if (nFit + nFindSpace > 0) {
				nFit += nFindSpace;
			}
		}
		int nDelta = nHeight > size.cy ? (nHeight - size.cy) / 2 : 0;
		TextOut(hdc, x, y+nDelta, &pTF->tf_text[nOffs], nFit);
		if (pPart->rvp_type == MET_BLOCK_QUOTE) {
			RECT leftRect;
			leftRect.top = y;
			leftRect.bottom = y + nHeight;
			leftRect.left = pBounds->left + _formatText.mef_marginLeft;
			leftRect.right = leftRect.left + 5;
			FillRect(hdc, &leftRect, theme_getDialogLightBackgroundBrush());
		}
		if (nFit < nLen) {
			nOffs += nFit;
			x = pUsed->left;
			y += nHeight;
			nHeight = 0;
			if (y > pBounds->bottom) {
				break;
			}
			nLen -= nFit;
			while (nLen > 0 && string_isSpace(pTF->tf_text[nOffs])) {
				nOffs++;
				nLen--;
			}
		} else {
			x += size.cx;
			nOffs += (int)nLen;
			pTR = pTR->tr_next;
			if (!pTR) {
				break;
			}
			nLen = pTR->tr_size;
			DeleteObject(SelectObject(hdc, hOldFont));
			hFont = mdr_createFont(&pTR->tr_attributes, wp->zoomFactor);
			hOldFont = SelectObject(hdc, hFont);
			if (pTR->tr_attributes.bgColor == NO_COLOR) {
				SetBkMode(hdc, TRANSPARENT);
			} else {
				SetBkMode(hdc, OPAQUE);
				SetBkColor(hdc, pTR->tr_attributes.bgColor);
			}
		}
	}
	DeleteObject(SelectObject(hdc, hOldFont));
	pUsed->bottom = y + nHeight + pPart->rvp_marginTop;
	if (pPart->rvp_type == MET_HEADER && pPart->rvp_level < 3) {
		mdr_paintRule(hdc, pBounds->left + 10, pBounds->right - 10, pUsed->bottom - 5);
	}
}

static BOOL mdr_isTopLevelOrBreak(LINE* lp, MDR_ELEMENT_TYPE mCurrentType) {
	for (int i = 0; i < lp->len; i++) {
		char c = lp->lbuf[i];
		if (c == '-' || c == '#' || c == '*' || c == '+') {
			return TRUE;
		}
		if (c == '>') {
			return mCurrentType != MET_BLOCK_QUOTE;
		}
		if (!string_isSpace(c)) {
			return mCurrentType == MET_HEADER;
		}
	}
	return TRUE;
}

static MDR_ELEMENT_TYPE mdr_determineTopLevelElement(LINE* lp, int *pOffset, int* pLevel, MDR_ELEMENT_FORMAT** pFormat) {
	int i = *pOffset;
	char c = lp->lbuf[i];
	MDR_ELEMENT_TYPE mType = MET_NORMAL;

	if (c == '#') {
		mType = MET_HEADER;
		*pLevel = 1;
		while (i < lp->len - 1 && lp->lbuf[i + 1] == '#') {
			i++;
			(* pLevel)++;
		}
		*pOffset = i;
	} else if (c == '-' || c == '*' || c == '+') {
		mType = MET_UNORDERED_LIST;
		*pLevel = 1;
		if (i < lp->len - 1 && lp->lbuf[i + 1] == c) {
			i++;
			*pLevel = 2;
			if (c == '-' && i < lp->len - 1 && lp->lbuf[i + 1] == c) {
				return MET_HORIZONTAL_RULE;
			}
		}
	} else if (c == '>') {
		mType = MET_BLOCK_QUOTE;
	}
	else if (isdigit(c)) {
		mType = MET_ORDERED_LIST;
		*pLevel = 1;
	}

	switch (mType) {
	case MET_HEADER:
		switch (*pLevel) {
		case 1: *pFormat = &_formatH1; break;
		case 2: *pFormat = &_formatH2; break;
		case 3: *pFormat = &_formatH3; break;
		case 4: *pFormat = &_formatH4; break;
		case 5: *pFormat = &_formatH5; break;
		}
		break;
	case MET_ORDERED_LIST:
	case MET_UNORDERED_LIST: *pFormat = *pLevel == 1 ? &_formatListLevel1 : &_formatListLevel2; break;
	case MET_BLOCK_QUOTE: *pFormat = &_formatBlockQuote; break;
	default: *pFormat = &_formatText; break;
	}
	return mType;
}

static void mdr_appendRun(RENDER_VIEW_PART* pPart, MDR_ELEMENT_FORMAT* pFormat, size_t nSize, int mAttrs) {
	if (nSize == 0 && pPart->rvp_e.flow.tf_runs) {
		return;
	}
	TEXT_RUN* pRun = ll_append(&pPart->rvp_e.flow.tf_runs, sizeof(TEXT_RUN));
	pRun->tr_size = nSize;
	pRun->tr_attributes.size = pFormat->mef_charHeight;
	pRun->tr_attributes.weight = pFormat->mef_charWeight;
	if (mAttrs & ATTR_CODE) {
		pRun->tr_attributes.fixedFont = 1;
		pRun->tr_attributes.bgColor = theme_getCurrent()->th_dialogLightBackground;
	} else {
		pRun->tr_attributes.bgColor = NO_COLOR;
	}
	if (mAttrs & ATTR_EMPHASIS) {
		pRun->tr_attributes.italic = 1;
	}
	if (mAttrs & ATTR_STRONG) {
		pRun->tr_attributes.weight = FW_BOLD;
	}
	if (mAttrs & ATTR_STRIKE) {
		pRun->tr_attributes.strikeout = 1;
	}
}

static LINE* mdr_parseFlow(LINE* lp, RENDER_VIEW_PART* pPart, STRING_BUF* pSB) {
	stringbuf_reset(pSB);
	int nState = 0;
	int nLevel = 0;
	int mAttrs = 0;
	size_t nLastOffset = 0;
	size_t nSize;
	MDR_ELEMENT_FORMAT* pFormat;

	MDR_ELEMENT_TYPE mType = MET_NORMAL;
	while (lp) {
		BOOL bSkipped = FALSE;
		for (int i = 0; i < lp->len; i++) {
			char c = lp->lbuf[i];
			if (nState <= 1 && string_isSpace(c)) {
				continue;
			}
			if (nState == 1 && mType == MET_BLOCK_QUOTE && c == '>' && !bSkipped) {
				bSkipped = TRUE;
				continue;
			}
			if (nState == 0) {
				mType = mdr_determineTopLevelElement(lp, &i, &nLevel, &pFormat);
				if (mType == MET_HORIZONTAL_RULE) {
					pPart->rvp_paint = mdr_renderHorizontalRule;
					return lp->next;
				}
				pPart->rvp_level = nLevel;
				if (mType != MET_NORMAL) {
					nState = 1;
					continue;
				}
			}
			if (nState < 2) {
				nState = 2;
			}
			char c2;
			if (c == '\\' && i < lp->len - 1 && ((c2 = lp->lbuf[i+1]) == '_' || c2 == '\\' || c2 == '*' || c2 == '`')) {
				i++;
				c = lp->lbuf[i];
			} else if (c == '`' || (!(mAttrs & ATTR_CODE) && (c == '*' || c == '_' || c == '~'))) {
				int nToggle = 0;
				if (c == '*' || c == '_') {
					if (i < lp->len - 1 && lp->lbuf[i + 1] == c) {
						nToggle = ATTR_STRONG;
						i++;
						c = c == '*' ? '_' : '*';
						if (i < lp->len - 1 && lp->lbuf[i + 1] == c) {
							nToggle = ATTR_EMPHASIS;
							i++;
						}
					} else {
						nToggle = ATTR_EMPHASIS;
					}
				} else if (c == '~' && i < lp->len - 1 && lp->lbuf[i + 1] == c) {
					nToggle = ATTR_STRIKE;
				} else if (c == '`') {
					nToggle = ATTR_CODE;
				}
				if (nToggle) {
					nSize = stringbuf_size(pSB) - nLastOffset;
					mdr_appendRun(pPart, pFormat, nSize, mAttrs);
					mAttrs ^= nToggle;
					nLastOffset += nSize;
					continue;
				}
			}
			stringbuf_appendChar(pSB, c);
		}
		lp = lp->next;
		if (mdr_isTopLevelOrBreak(lp, mType)) {
			break;
		}
		nState = 1;
		stringbuf_appendChar(pSB, ' ');
	}
	pPart->rvp_type = mType;
	pPart->rvp_marginLeft = pFormat->mef_marginLeft;
	pPart->rvp_marginTop = pFormat->mef_marginTop;
	pPart->rvp_marginRight = pFormat->mef_marginRight;
	pPart->rvp_marginBottom = pFormat->mef_marginBottom;
	pPart->rvp_paint = mdr_renderTextFlow;
	pPart->rvp_e.flow.tf_text = _strdup(stringbuf_getString(pSB));

	nSize = stringbuf_size(pSB) - nLastOffset;
	mdr_appendRun(pPart, pFormat, nSize, mAttrs);

	return lp;
}

static void mdr_parseViewParts(FTABLE* fp, MARKDOWN_RENDERER_DATA* pData) {
	LINE* lp = fp->firstl;
	STRING_BUF * pSB = stringbuf_create(256);
	while (lp) {
		if (!ln_lineIsEmpty(lp)) {
			RENDER_VIEW_PART* pPart = ll_append(&pData->md_pElements, sizeof(RENDER_VIEW_PART));
			pPart->rvp_lpStart = lp;
			lp = mdr_parseFlow(lp, pPart, pSB);
		} else {
			lp = lp->next;
		}
		if (lp == fp->lastl) {
			break;
		}
	}
	stringbuf_destroy(pSB);
}

/*
 * Returns the view part corresponding with the 'nline' line.
 */
static RENDER_VIEW_PART* mdr_getViewPartForLine(RENDER_VIEW_PART* pFirstPart, LINE* pStart, long nLine) {
	// TODO....
	return (RENDER_VIEW_PART * )ll_at((LINKED_LIST*)pFirstPart, nLine);
}

/*
 * Render the current window in hexadecimal display.
 */
static void mdr_renderPage(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	RECT rect;
	WINFO* wp = pCtx->rc_wp;
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	FTABLE* fp = FTPOI(wp);
	GetClientRect(wp->ww_handle, &rect);
	HBRUSH hBrushCaretLine = CreateSolidBrush(pCtx->rc_theme->th_caretLineColor);
	if (!pData->md_pElements) {
		mdr_parseViewParts(fp, pData);
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, fp->firstl, wp->minln);
	RECT occupiedBounds;
	int nElements = 0;
	FillRect(pCtx->rc_hdc, pClip, hBrushBg);
	for (; pPart && rect.top < pClip->bottom; rect.top = occupiedBounds.bottom) {
		nElements++;
		pPart->rvp_paint(wp, pPart, pCtx->rc_hdc, &rect, &occupiedBounds);
		pPart = pPart->rvp_next;
	}
	pData->md_nElementsPerPage = nElements;
}

/*
 * Destroy one view part. 
 */
static int mdr_destroyViewPart(RENDER_VIEW_PART *pRVP) {
	free(pRVP->rvp_e.flow.tf_text);
	ll_destroy(&pRVP->rvp_e.flow.tf_runs, 0);
	return 1;
}

/*
 * Destroy the renderer data. 
 */
static void mdr_destroyData(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (pData) {
		ll_destroy(&pData->md_pElements, mdr_destroyViewPart);
		free(pData->md_pElements);
		pData->md_pElements = NULL;
		free(pData);
	}
}

/*
 * Destroy the cached view parts for now. 
 */
static void mdr_modelChanged(WINFO* wp, MODEL_CHANGE* pChanged) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (pData) {
		ll_destroy(&pData->md_pElements, mdr_destroyViewPart);
		pData->md_pElements = NULL;
	}
}

/*
 * Allocate the markdown renderer data structure.
 */
static void* mdr_allocData(WINFO* wp) {
	return calloc(1, sizeof(MARKDOWN_RENDERER_DATA));
}

/*
 * Calculate the number of logical lines displayed 
 */
static long mdr_calculateMaxLine(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (pData) {
		return ll_size((LINKED_LIST*)pData->md_pElements);
	}
	// TODO: may we need to ensure the renderer is initialized
	return 20;
}

/*
 * Calculate the "longest" line displayed. As we wrap text during rendering the maximum 
 * line length is the number of logical columns available.
 */
static long mdr_calculateLongestLine(WINFO* wp) {
	return wp->maxcol - wp->mincol;
}

/*
 * Calculate the maximum column for column moves. 
 */
static long mdr_calculateMaxColumn(WINFO* wp, long ln, LINE* lp) {
	// TODO: not correct....
	return lp->len;
}

/*
 * Hit testing not yet implemented.
 */
static int mdr_screenOffsetToBuffer(WINFO* wp, long ln, long col, INTERNAL_BUFFER_POS* pPosition) {
	// not implemented
	return 0;
}

static void mdr_scroll(WINFO* wp, int dx, int dy) {
	RedrawWindow(wp->ww_handle, NULL, NULL, RDW_ERASE|RDW_INVALIDATE);
	if (wp->lineNumbers_handle) {
		RedrawWindow(wp->lineNumbers_handle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);

	}
}

static void mdr_scrollSetBounds(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	int nDelta = pData->md_nElementsPerPage;
	if (nDelta <= 0) {
		nDelta = 10;
	}
	wp->mincursln = wp->minln;
	wp->maxcursln = wp->mincursln + nDelta;
	wp->maxln = wp->maxcursln;
	wp->maxcurscol = wp->maxcol = 50;
}

static int mdr_rendererSupportsMode(int aMode) {
	if (aMode == SHOWCARET_LINE_HIGHLIGHT || aMode == SHOWLINENUMBERS || aMode == SHOWRULER) {
		return 0;
	}
	return 1;
}

static RENDERER _mdrRenderer = {
	render_singleLineOnDevice,
	mdr_renderPage,
	caret_placeCursorAndValidate,
	mdr_calculateMaxLine,
	mdr_calculateLongestLine,
	mdr_calculateMaxColumn,
	caret_updateDueToMouseClick,
	mdr_screenOffsetToBuffer,
	mdr_allocData,
	mdr_destroyData,
	mdr_scroll,
	mdr_scrollSetBounds,
	mdr_rendererSupportsMode,
	mdr_modelChanged
};

/*
 * Returns a markdown renderer.
 */
RENDERER* mdr_getRenderer() {
	return &_mdrRenderer;
}


