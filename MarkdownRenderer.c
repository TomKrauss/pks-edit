/*
 * MarkdownRenderer.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: A special renderer for markdown files.
 *
 * 										created: 05.11.21
 * 										last modified: 14.12.21
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include <Windows.h>
#include <CommCtrl.h>
#include <stdio.h>

#include "winfo.h"
#include "trace.h"
#include "caretmovement.h"
#include "customcontrols.h"
#include "linkedlist.h"
#include "editorfont.h"
#include "stringutil.h"
#include "themes.h"
#include "pathname.h"
#include "winutil.h"
#include "pksmacro.h"
#include "mainframe.h"
#include "edctype.h"

#define NO_COLOR			-1		// marker for no color defined
#define PARAGRAPH_OFFSET	15		// offset in pixels between paragraph type elements.
#define DEFAULT_LEFT_MARGIN	10		// offset of most elements to the left of the screen.

extern HINSTANCE		hInst;

static const char _escapedChars[] = "\\`*_{}[]<>()#+-.!|";

// Loads the an image with a name and a format into a HBITMAP.
extern HBITMAP loadimage_load(char* pszName);

typedef struct tagRENDER_VIEW_PART RENDER_VIEW_PART;

typedef void (*RENDER_PAINT)(WINFO* wp, RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed);

typedef struct tagENTITY_MAPPING {
	char	c;
	char    entity[8];
} ENTITY_MAPPING;

static ENTITY_MAPPING _entities[] = {
	{'&', "amp;"},
	{'©', "copy;"},
	{'\'', "apos;"},
	{'<', "lt;"},
	{'>', "gt;"},
	{' ', "nbsp;"},
	{'®', "reg;"},
};

//
// Describes the bounds of a text run with the following shape.
// 
//             +---------+
//       left1 |         |
// +-----------+ top1    |
// |                     |
// |         right2 +----+  bottom2
// |                |
// +----------------+ bottom
//
typedef struct tagRUN_BOUNDS {
	int		left1;
	int		top1;
	int		right2;
	int		bottom2;
	int		left;
	int		top;
	int		right;
	int		bottom;
} RUN_BOUNDS;

/*
 * The text style of a "text run"
 */
typedef struct tagFONT_ATTRIBUTES {
	int		strikeout;
	int		italic;
	int		size;
	int		underline;
	int		weight;
	int		lineBreak;
	BOOL	rollover;
	COLORREF bgColor;
	COLORREF fgColor;
	int		fixedFont;
} FONT_ATTRIBUTES;

typedef struct tagMD_IMAGE {
	int					mdi_width;
	int					mdi_height;
	HBITMAP				mdi_image;
} MD_IMAGE;

typedef struct tagTEXT_RUN {
	struct tagTEXT_RUN* tr_next;
	size_t				tr_size;
	RUN_BOUNDS			tr_bounds;
	FONT_ATTRIBUTES		tr_attributes;
	char*				tr_link;
	char*				tr_title;
	MD_IMAGE*			tr_image;
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
	MET_TASK_LIST,
	MET_NORMAL,
	MET_FENCED_CODE_BLOCK,
	MET_HEADER, 
	MET_TABLE,
	MET_HORIZONTAL_RULE
} MDR_ELEMENT_TYPE;

#define RTC_ALIGN_LEFT		0x1
#define RTC_ALIGN_RIGHT		0x2
#define RTC_ALIGN_CENTER	0x3
#define MAX_TABLE_COLUMNS   256

typedef struct tagRENDER_TABLE_CELL {
	struct tagRENDER_TABLE_CELL* rtc_next;
	BOOL	rtc_isHeader;
	byte	rtc_align;
	int		rtc_width;			// width in pixels or 0 for default width 
	TEXT_FLOW rtc_flow;
} RENDER_TABLE_CELL;

typedef struct tagRENDER_TABLE_ROW {
	struct tagRENDER_TABLE_ROW* rtr_next;
	RENDER_TABLE_CELL* rtr_cells;
} RENDER_TABLE_ROW;

typedef struct tag_RENDER_TABLE {
	RENDER_TABLE_ROW* rt_rows;
	int rt_columnCount;
	int rt_columnWidths[MAX_TABLE_COLUMNS];
	int rt_totalColumnWidth;
} RENDER_TABLE;

typedef struct tagMARGINS {
	int	m_top;
	int	m_bottom;
	int	m_left;
	int	m_right;
} MARGINS;

struct tagRENDER_VIEW_PART {
	struct tagRENDER_VIEW_PART* rvp_next;
	LINE* rvp_lpStart;
	RENDER_PAINT rvp_paint;
	MARGINS rvp_margins;
	RECT rvp_bounds;			// Bounds of an element excluding the margins.
	MDR_ELEMENT_TYPE rvp_type;
	long rvp_number;			// for numbered lists
	int	rvp_level;				// for headers and lists - the level.
	union {
		TEXT_FLOW rvp_flow;
		RENDER_TABLE* rvp_table;
	} rvp_data;
};

typedef struct tagMDR_ELEMENT_FORMAT {
	MARGINS mef_margins;
	int mef_charHeight;
	int mef_charWeight;
	int mef_fixedFont;
} MDR_ELEMENT_FORMAT;

#define ATTR_EMPHASIS	0x1
#define ATTR_STRONG		0x2
#define ATTR_STRIKE		0x4
#define ATTR_CODE		0x8
#define ATTR_HIGHLIGHT		0x10
#define ATTR_LINK		0x20
#define ATTR_LINE_BREAK 0x40

/*
 * Render a "text flow" - a simple text which contains styled regions aka text runs.
 */
static void mdr_renderTextFlow(WINFO* wp, MARGINS* pMargins, TEXT_FLOW* pFlow, HDC hdc, RECT* pBounds, 
	RECT* pPartBounds, RECT* pUsed, int nBlockQuoteLevel, int nAlign, BOOL bSkipSpace);

static MDR_ELEMENT_FORMAT _formatText = {
	0, 0, DEFAULT_LEFT_MARGIN, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatFenced = {
	12, 0, 20, 20, 14, FW_BOLD, 1
};

#define BLOCK_QUOTE_INDENT		25
#define BLOCK_QUOTE_MARGIN		4

static MDR_ELEMENT_FORMAT _formatBlockQuote = {
	0, 0, DEFAULT_LEFT_MARGIN+BLOCK_QUOTE_INDENT, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatBlockQuote2 = {
	0, 0, DEFAULT_LEFT_MARGIN + (2*BLOCK_QUOTE_INDENT), 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatBlockQuote3 = {
	0, 0, DEFAULT_LEFT_MARGIN + (3 * BLOCK_QUOTE_INDENT), 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatTable = {
	10, 10, 10, 10, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatListLevel1 = {
	1, 1, 45, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatListLevel2 = {
	1, 1, 70, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatListLevel3 = {
	1, 1, 95, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatH1 = {
	PARAGRAPH_OFFSET+5, DEFAULT_LEFT_MARGIN, DEFAULT_LEFT_MARGIN, 20, 28, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH2 = {
	PARAGRAPH_OFFSET, DEFAULT_LEFT_MARGIN, DEFAULT_LEFT_MARGIN, 20, 24, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH3 = {
	PARAGRAPH_OFFSET-5, DEFAULT_LEFT_MARGIN, DEFAULT_LEFT_MARGIN, 20, 20, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH4 = {
	PARAGRAPH_OFFSET/2, 3, DEFAULT_LEFT_MARGIN, 20, 18, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH5 = {
	PARAGRAPH_OFFSET/3, 3, DEFAULT_LEFT_MARGIN, 20, 16, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH6 = {
	PARAGRAPH_OFFSET/4, 3, DEFAULT_LEFT_MARGIN, 20, 14, FW_BOLD
};

static MARGINS _tableMargins = {
	10,10,8,8
};

typedef struct tagMARKDOWN_RENDERER_DATA {
	RENDER_VIEW_PART* md_pElements;	// The render view parts to be rendered
	RECT md_lastBounds;
	HWND md_hwndTooltip;
	TEXT_RUN* md_focussedRun;
	BOOL md_displayingEndOfFile;
	long md_lastMinLn;
	int md_nElementsPerPage;
} MARKDOWN_RENDERER_DATA;

/*
 * Checks, whether a point is withing the bounds of a text run.
 */
static BOOL runbounds_contains(RUN_BOUNDS* pBounds, POINT pt) {
	if (pt.y >= pBounds->top1 && pt.y < pBounds->top) {
		return pt.x >= pBounds->left1 && pt.x < pBounds->right;
	}
	if (pt.y >= pBounds->bottom && pt.y < pBounds->bottom2) {
		return pt.x >= pBounds->left && pt.x < pBounds->right2;
	}
	return (pt.y >= pBounds->top && pt.y < pBounds->bottom&& pt.x >= pBounds->left && pt.x < pBounds->right);
}

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
	_lf.lfUnderline = pAttrs->rollover && pAttrs->underline;
	return CreateFontIndirect(&_lf);
}

static void mdr_paintRule(HDC hdc, int left, int right, int y, int nStrokeWidth) {
	THEME_DATA* pTheme = theme_getCurrent();
	HPEN hPen = CreatePen(PS_SOLID, nStrokeWidth, pTheme->th_dialogLightBackground);
	HPEN hPenOld = SelectObject(hdc, hPen);
	MoveTo(hdc, left, y);
	LineTo(hdc, right, y);
	DeleteObject(SelectObject(hdc, hPenOld));
}

static void mdr_renderHorizontalRule(WINFO* wp, RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed) {
	pUsed->top = pBounds->top + pPart->rvp_margins.m_top;
	pUsed->left = pBounds->left + DEFAULT_LEFT_MARGIN;
	pUsed->bottom = pUsed->top + DEFAULT_LEFT_MARGIN + pPart->rvp_margins.m_bottom;
	pUsed->right = pBounds->right - 20;
	mdr_paintRule(hdc, pUsed->left, pUsed->right, (pUsed->top + pUsed->bottom - 3) / 2, 3);
	memcpy(&pPart->rvp_bounds, pUsed, sizeof * pUsed);
}

/*
 * Render a markdown table. 
 */
static void mdr_renderTable(WINFO* wp, RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed) {
	THEME_DATA* pTheme = theme_getCurrent();
	int nColumnWidth[MAX_TABLE_COLUMNS];

	HPEN hPen = CreatePen(PS_SOLID, 1, pTheme->th_dialogLightBackground);
	HPEN hPenOld = SelectObject(hdc, hPen);
	RENDER_TABLE* pTable = pPart->rvp_data.rvp_table;
	int nColumns = pTable->rt_columnCount;
	int left = pBounds->left+pPart->rvp_margins.m_left;
	int right = pBounds->right - pPart->rvp_margins.m_right;
	int nWidth = right - left;
	int nTotal = pTable->rt_totalColumnWidth;
	if (nTotal == 0) {
		nTotal = 1;
	}
	if (nWidth > nTotal) {
		right = left + nTotal;
		nWidth = nTotal;
	}
	for (int i = 0; i < MAX_TABLE_COLUMNS; i++) {
		if (!pTable->rt_columnWidths[i]) {
			break;
		}
		nColumnWidth[i] = pTable->rt_columnWidths[i] * nWidth / nTotal;
	}
	int y = pBounds->top + pPart->rvp_margins.m_top;
	RENDER_TABLE_ROW* pRow = pTable->rt_rows;
	RECT bounds;
	bounds.top = y;
	while (pRow) {
		RENDER_TABLE_CELL* pCell = pRow->rtr_cells;
		RECT usedBounds;
		RECT flowBounds;
		int nMaxHeight = 10;
		int nColumn = 0;
		BOOL bHeader = FALSE;
		bounds.left = left;
		while (pCell) {
			bHeader = pCell->rtc_isHeader;
			bounds.right = bounds.left + nColumnWidth[nColumn];
			bounds.bottom = bounds.top + 3000;
			mdr_renderTextFlow(wp, &_tableMargins, &pCell->rtc_flow, hdc, &bounds, &flowBounds, &usedBounds, 0, pCell->rtc_align, TRUE);
			int nHeight = usedBounds.bottom - usedBounds.top;
			if (nHeight > nMaxHeight) {
				nMaxHeight = nHeight;
			}
			nColumn++;
			bounds.left = bounds.right;
			pCell = pCell->rtc_next;
		}
		pRow = pRow->rtr_next;
		bounds.top += nMaxHeight;
		if (pRow) {
			int nLineY = bounds.top + _tableMargins.m_bottom / 2;
			MoveTo(hdc, left, nLineY);
			LineTo(hdc, right, nLineY);
			if (bHeader) {
				MoveTo(hdc, left, nLineY+1);
				LineTo(hdc, right, nLineY+1);
			}
		}
	}
	int nTableHeight = bounds.top - y + _tableMargins.m_bottom;
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, left, y, right, y + nTableHeight);
	int x = left;
	for (int i = 0; i < nColumns; i++) {
		x += nColumnWidth[i];
		MoveTo(hdc, x, y);
		LineTo(hdc, x, y+nTableHeight);
	}
	DeleteObject(SelectObject(hdc, hPenOld));
	pUsed->top = y;
	pUsed->left = left;
	pUsed->right = right;
	pUsed->bottom = pUsed->top + nTableHeight;
	memcpy(&pPart->rvp_bounds, pUsed, sizeof * pUsed);
}

/*
 * Paint an image displayed in a markdown text.
 */
static void mdr_paintImage(WINFO* wp, HDC hdc, TEXT_RUN* pTR, int x, int y, SIZE* pSize) {
	char szFilename[EDMAXPATHLEN];
	MD_IMAGE* pImage = pTR->tr_image;
	if (!pImage) {
		return;
	}
	if (!pImage->mdi_image) {
		FTABLE* fp = wp->fp;
		string_splitFilename(fp->fname, szFilename, NULL);
		string_concatPathAndFilename(szFilename, szFilename, pTR->tr_link);
		char* pszExt = strrchr(pTR->tr_link, '.');
		if (pszExt && _stricmp(pszExt + 1, "bmp") != 0) {
			pImage->mdi_image = loadimage_load(szFilename);
		} else {
			pImage->mdi_image = LoadImage(NULL, szFilename, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		}
	}
	if (pImage->mdi_image) {
		BITMAP          bitmap;
		HGDIOBJ         oldBitmap;
		HDC             hdcMem;
		hdcMem = CreateCompatibleDC(hdc);
		oldBitmap = SelectObject(hdcMem, pImage->mdi_image);
		GetObject(pImage->mdi_image, sizeof(bitmap), &bitmap);
		int nWidth = pImage->mdi_width ? pImage->mdi_width : 
			(pImage->mdi_height ? bitmap.bmWidth * pImage->mdi_height / bitmap.bmHeight : bitmap.bmWidth);
		int nHeight = pImage->mdi_height ? pImage->mdi_height : 
			(pImage->mdi_width ? bitmap.bmHeight * pImage->mdi_width / bitmap.bmWidth : bitmap.bmHeight);
		nWidth = (int)(nWidth * wp->zoomFactor);
		nHeight = (int)(nHeight * wp->zoomFactor);
		SetStretchBltMode(hdc, COLORONCOLOR);
		StretchBlt(hdc, x, y, nWidth, nHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);
		pSize->cx = nWidth;
		pSize->cy = nHeight;
	} else {
		sprintf(szFilename, "Cannot load image %s", pTR->tr_link);
		TextOut(hdc, x, y, szFilename, (int)strlen(szFilename));
		pSize->cy = 20;
		pSize->cx = 200;
	}
}

/*
 * Paint a checkmark for a task list. 
 */
static void mdr_paintCheckmark(HDC hdc, int x, int y, BOOL bChecked) {
	THEME_DATA* pTheme = theme_getCurrent();
	LOGBRUSH brush;
	brush.lbColor = pTheme->th_dialogBorder;
	brush.lbHatch = 0;
	brush.lbStyle = PS_SOLID;
	HPEN hPen = ExtCreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, 2, &brush, 0, NULL);
	HPEN hPenOld = SelectObject(hdc, hPen);
	int nSize = 13;
	MoveTo(hdc, x, y);
	LineTo(hdc, x+nSize, y);
	LineTo(hdc, x + nSize, y+nSize);
	LineTo(hdc, x, y + nSize);
	LineTo(hdc, x, y);
	if (bChecked) {
		MoveTo(hdc, x+2, y+(nSize/2));
		LineTo(hdc, x + nSize / 3 + 1, y + nSize - 4);
		LineTo(hdc, x + nSize - 3, y + 3);
	}
	DeleteObject(SelectObject(hdc, hPenOld));
}

/*
 * Render a "text flow" - a simple text which contains styled regions aka text runs.
 */
static void mdr_renderTextFlow(WINFO* wp, MARGINS* pMargins, TEXT_FLOW* pFlow, HDC hdc, RECT* pBounds, RECT* pPartBounds, 
		RECT* pUsed, int nBlockQuoteLevel, int nAlign, BOOL bSkipSpace) {
	THEME_DATA* pTheme = theme_getCurrent();
	int x = pBounds->left + pMargins->m_left;
	int y = pBounds->top + pMargins->m_top;
	int nRight = pBounds->right - pMargins->m_right;
	pUsed->top = y;
	pUsed->left = x;
	pUsed->right = nRight;
	TEXT_FLOW* pTF = pFlow;
	TEXT_RUN* pTR = pTF->tf_runs;
	int nOffs = 0;
	size_t nLen;
	nLen = pTR->tr_size;
	int nHeight = 0;
	font_setDefaultTextColors(hdc, pTheme);
	COLORREF cDefault = GetTextColor(hdc);
	HFONT hFont = mdr_createFont(&pTR->tr_attributes, wp->zoomFactor);
	HFONT hOldFont = SelectObject(hdc, hFont);
	if (bSkipSpace) {
		while (nLen > 0 && string_isSpace(pTF->tf_text[nOffs])) {
			nOffs++;
			nLen--;
		}
	}
	pPartBounds->top = y;
	BOOL bRunBegin = TRUE;
	while (pTR) {
		SIZE size;
		int nFit = 0;
		if (pTR->tr_image) {
			mdr_paintImage(wp, hdc, pTR, x, y, &size);
			if (size.cy > nHeight) {
				nHeight = size.cy;
			}
			pPartBounds->bottom = y + nHeight;
			pPartBounds->left = x;
			pPartBounds->right = x + size.cx;
			pTR->tr_bounds.top1 = pTR->tr_bounds.top = y;
			pTR->tr_bounds.bottom = pTR->tr_bounds.bottom2 = pPartBounds->bottom;
			pTR->tr_bounds.left = pTR->tr_bounds.left1 = x;
			pTR->tr_bounds.right = pTR->tr_bounds.right2 = pPartBounds->right;
		} else {
			int nTotalWidth = nRight - x;
			GetTextExtentExPoint(hdc, &pTF->tf_text[nOffs], (int)nLen, nTotalWidth, &nFit, 0 /* TODO: callback for measuring*/, &size);
			if (size.cy > nHeight) {
				nHeight = size.cy;
			}
			pPartBounds->bottom = y + nHeight;
			if (nOffs == 0) {
				pPartBounds->left = x;
				pPartBounds->right = nRight;
			}
			if (bRunBegin) {
				pTR->tr_bounds.top1 = pTR->tr_bounds.top = y;
				pTR->tr_bounds.bottom = pTR->tr_bounds.bottom2 = y + nHeight;
				pTR->tr_bounds.left = pTR->tr_bounds.left1 = x;
				pTR->tr_bounds.right = pTR->tr_bounds.right2 = x + size.cx;
			}
			else {
				if (pTR->tr_bounds.top == pTR->tr_bounds.top1) {
					pTR->tr_bounds.top = y;
				}
				if (nFit < nLen) {
					pTR->tr_bounds.bottom = y + nHeight;
				}
				else {
					pTR->tr_bounds.bottom2 = y + nHeight;
				}
				pTR->tr_bounds.left = x;
				pTR->tr_bounds.right2 = x + size.cx;
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
			int nDeltaX = 0;
			if (bRunBegin && !pTR->tr_next) {
				// TODO alignment supported only for non formatted contents.
				if (nAlign == RTC_ALIGN_RIGHT) {
					nDeltaX = nTotalWidth - size.cx;
				}
				else if (nAlign == RTC_ALIGN_CENTER) {
					nDeltaX = (nTotalWidth - size.cx) / 2;
				}
			}
			TextOut(hdc, x + nDeltaX, y + nDelta, &pTF->tf_text[nOffs], nFit);
			if (nBlockQuoteLevel) {
				RECT leftRect;
				leftRect.top = y - pMargins->m_top;
				leftRect.bottom = y + nHeight + pMargins->m_bottom;
				leftRect.left = pBounds->left + _formatText.mef_margins.m_left;
				leftRect.right = leftRect.left + 5;
				for (int i = 0; i < nBlockQuoteLevel; i++) {
					FillRect(hdc, &leftRect, theme_getDialogLightBackgroundBrush());
					leftRect.left += BLOCK_QUOTE_INDENT;
					leftRect.right += BLOCK_QUOTE_INDENT;
				}
			}
		}
		if (!pTR->tr_image && nFit < nLen && nFit > 0) {
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
			bRunBegin = FALSE;
		}
		else {
			if (pTR->tr_attributes.lineBreak) {
				x = pUsed->left;
				y += nHeight;
			}
			else {
				x += size.cx;
			}
			nOffs += (int)nLen;
			pTR = pTR->tr_next;
			if (!pTR) {
				break;
			}
			bRunBegin = TRUE;
			nLen = pTR->tr_size;
			DeleteObject(SelectObject(hdc, hOldFont));
			hFont = mdr_createFont(&pTR->tr_attributes, wp->zoomFactor);
			hOldFont = SelectObject(hdc, hFont);
			SetTextColor(hdc, pTR->tr_attributes.fgColor == NO_COLOR ? cDefault : pTR->tr_attributes.fgColor);
			if (pTR->tr_attributes.bgColor == NO_COLOR) {
				SetBkMode(hdc, TRANSPARENT);
			}
			else {
				SetBkMode(hdc, OPAQUE);
				SetBkColor(hdc, pTR->tr_attributes.bgColor);
			}
		}
	}
	DeleteObject(SelectObject(hdc, hOldFont));
	pUsed->bottom = y + nHeight + pMargins->m_bottom;
}

/*
 * Render a "text type block part".
 */
static void mdr_renderMarkdownBlockPart(WINFO* wp, RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed) {
	MARGINS* pMargins = &pPart->rvp_margins;
	int x = pBounds->left + pMargins->m_left;
	int y = pBounds->top + pMargins->m_top;
	int nRight = pBounds->right - pMargins->m_right;
	if (pPart->rvp_type == MET_UNORDERED_LIST) {
		TextOutW(hdc, x - 15, y, pPart->rvp_level == 1 ? L"\u25CF" : (pPart->rvp_level == 2 ? L"\u25CB" : L"\u25A0"), 1);
	} else if (pPart->rvp_type == MET_TASK_LIST) {
		mdr_paintCheckmark(hdc, x - 20, y, pPart->rvp_number == 1);
	} else if (pPart->rvp_type == MET_ORDERED_LIST) {
		char szBuf[20];
		sprintf(szBuf, "%ld.", pPart->rvp_number);
		TextOut(hdc, x - 20, y, szBuf, (int)strlen(szBuf));
	} else if (pPart->rvp_type == MET_FENCED_CODE_BLOCK) {
		TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);
		RECT r;
		r.top = y-5;
		r.bottom = r.top + (pPart->rvp_number * tm.tmHeight) + pMargins->m_bottom+10;
		r.left = x-10;
		r.right = nRight > 1200 ? nRight - 100 : nRight;
		HBRUSH hBrOld = SelectObject(hdc, theme_getDialogLightBackgroundBrush());
		HPEN hPen = CreatePen(PS_SOLID, 1, theme_getCurrent()->th_dialogBorder);
		HPEN hPenOld = SelectObject(hdc, hPen);
		Rectangle(hdc, r.left, r.top, r.right, r.bottom);
		SelectObject(hdc, hBrOld);
		DeleteObject(SelectObject(hdc, hPenOld));
	}
	int nDCId = SaveDC(hdc);
	mdr_renderTextFlow(wp, pMargins, &pPart->rvp_data.rvp_flow, hdc, pBounds, &pPart->rvp_bounds, pUsed,
			pPart->rvp_type == MET_BLOCK_QUOTE ? pPart->rvp_level : 0, RTC_ALIGN_LEFT, pPart->rvp_type != MET_FENCED_CODE_BLOCK);
	if (pPart->rvp_type == MET_HEADER && pPart->rvp_level < 3) {
		mdr_paintRule(hdc, pBounds->left + DEFAULT_LEFT_MARGIN, pBounds->right - DEFAULT_LEFT_MARGIN, pUsed->bottom - 2, 1);
	}
	RestoreDC(hdc, nDCId);
}

static BOOL mdr_isTopLevelOrBreak(LINE* lp, MDR_ELEMENT_TYPE mCurrentType, int nLevel) {
	if (!lp) {
		return FALSE;
	}
	if (lp->len == 0) {
		return TRUE;
	}
	for (int i = 0; i < lp->len; i++) {
		char c = lp->lbuf[i];
		if (c == '-' || c == '#' || c == '*' || c == '+') {
			return TRUE;
		}
		if (i == 0 && lp->len >= 3 && (c == '`' || c == '~') && lp->lbuf[1] == c && lp->lbuf[2] == c) {
			return TRUE;
		}
		if (isdigit((unsigned char)c)) {
			string_convertToLong(&lp->lbuf[i]);
			return *_strtolend == '.';
		}
		if (c == '>') {
			if (mCurrentType != MET_BLOCK_QUOTE) {
				return TRUE;
			}
			LINE* lpPrev = lp->prev;
			if (lpPrev && lpPrev->lbuf[0] == '>' && (lpPrev->len == 1 || (lpPrev->len == 2 && lpPrev->lbuf[1] == ' '))) {
				return TRUE;
			}
			int nLevelNew = 1;
			while (i < lp->len - 1 && lp->lbuf[i + 1] == '>') {
				nLevelNew++;
				i++;
			}
			return nLevel != nLevelNew;
		}
		if (!string_isSpace(c)) {
			return mCurrentType == MET_HEADER;
		}
	}
	return TRUE;
}

static int mdr_getLevelFromIndent(LINE* lp, char aListChar) {
	for (int i = 0; i < lp->len; i++) {
		if (lp->lbuf[i] != ' ') {
			return 1 + (i / 2);
		}
	}
	return 1;
}
/*
 * Parse a top level markup element (list, header, code block etc...) 
 */
static MDR_ELEMENT_TYPE mdr_determineTopLevelElement(LINE* lp, int *pOffset, int* pLevel, MDR_ELEMENT_FORMAT** pFormat, long* pNumber) {
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
		*pLevel = mdr_getLevelFromIndent(lp, c);
		if (c == '-' && i < lp->len - 2 && lp->lbuf[i + 1] == c && lp->lbuf[i + 2] == c) {
			return MET_HORIZONTAL_RULE;
		}
		i++;
		BOOL bSpaceFound = FALSE;
		while (i < lp->len && lp->lbuf[i] == ' ') {
			i++;
			bSpaceFound = TRUE;
		}
		if (bSpaceFound) {
			if (i < lp->len - 3 && lp->lbuf[i] == '[' && lp->lbuf[i + 2] == ']') {
				mType = MET_TASK_LIST;
				char cX = lp->lbuf[i + 1];
				*pNumber = cX == 'x' || cX == 'X' ? 1 : 0;
				*pOffset = i + 3;
			}
		} else {
			mType = MET_NORMAL;
		}
	} else if (c == '>') {
		*pLevel = 1;
		mType = MET_BLOCK_QUOTE;
		while (i < lp->len - 1 && lp->lbuf[i + 1] == '>') {
			(*pLevel)++;
			i++;
		}
	} else if (isdigit((unsigned char)c)) {
		*pNumber = (long)string_convertToLong(&lp->lbuf[i]);
		if (*_strtolend == '.') {
			*pOffset = (int)(_strtolend - lp->lbuf) + 1;
			mType = MET_ORDERED_LIST;
			*pLevel = mdr_getLevelFromIndent(lp, c);
		}
	} else if ((c == '`' || c == '~') && lp->len >= 3 && lp->lbuf[1] == c && lp->lbuf[2] == c) {
		mType = MET_FENCED_CODE_BLOCK;
	}

	switch (mType) {
	case MET_HEADER:
		switch (*pLevel) {
		case 1: *pFormat = &_formatH1; break;
		case 2: *pFormat = &_formatH2; break;
		case 3: *pFormat = &_formatH3; break;
		case 4: *pFormat = &_formatH4; break;
		case 5: *pFormat = &_formatH5; break;
		default: *pFormat = &_formatH6; break;
		}
		break;
	case MET_FENCED_CODE_BLOCK:
		*pFormat = &_formatFenced; break;
	case MET_TASK_LIST:
	case MET_ORDERED_LIST:
	case MET_UNORDERED_LIST: *pFormat = *pLevel == 1 ? &_formatListLevel1 : (*pLevel == 2 ? &_formatListLevel2 : &_formatListLevel3); break;
	case MET_BLOCK_QUOTE:
		switch (*pLevel) {
		case 1: *pFormat = &_formatBlockQuote; break;
		case 2: *pFormat = &_formatBlockQuote2; break;
		default: *pFormat = &_formatBlockQuote3; break;
		}
		break;
	default: *pFormat = &_formatText; break;
	}
	return mType;
}

static TEXT_RUN* mdr_appendRun(TEXT_RUN** pRuns, MDR_ELEMENT_FORMAT* pFormat, size_t nSize, int mAttrs) {
	if (nSize == 0 && *pRuns) {
		return NULL;
	}
	TEXT_RUN* pRun = ll_append(pRuns, sizeof(TEXT_RUN));
	pRun->tr_size = nSize;
	pRun->tr_attributes.size = pFormat->mef_charHeight;
	pRun->tr_attributes.weight = pFormat->mef_charWeight;
	pRun->tr_attributes.fixedFont = pFormat->mef_fixedFont;
	if (mAttrs & ATTR_HIGHLIGHT) {
		pRun->tr_attributes.fixedFont = 0;
		pRun->tr_attributes.bgColor = theme_textStyleBackground("highlight", RGB(100, 100, 25)); 
	} else if (mAttrs & ATTR_CODE) {
		pRun->tr_attributes.fixedFont = 1;
		pRun->tr_attributes.bgColor = theme_getCurrent()->th_dialogLightBackground;
	} else {
		pRun->tr_attributes.bgColor = NO_COLOR;
	}
	if (mAttrs & ATTR_LINK) {
		pRun->tr_attributes.underline = 1;
		pRun->tr_attributes.fgColor = theme_textStyleForeground("hyperlink", RGB(120, 120, 255));
	} else {
		pRun->tr_attributes.fgColor = NO_COLOR;
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
	if (mAttrs & ATTR_LINE_BREAK) {
		pRun->tr_attributes.lineBreak = 1;
	}
	return pRun;
}

/*
 * Parse the link URL - allow for things as included title specs. 
 */
static BOOL mdr_parseLinkUrl(char* pszBuf, char** pszLink, char** pszTitle, int* nWidth, int* nHeight) {
	BOOL bAuto = FALSE;
	int nStart = 0;
	if (pszBuf[nStart] == '<') {
		bAuto = TRUE;
		nStart++;
	}
	size_t nLen = strlen(pszBuf+nStart);
	if (nLen == 0) {
		return FALSE;
	}
	if (bAuto) {
		if (pszBuf[nLen - 1] == '>') {
			nLen--;
		} else {
			bAuto = FALSE;
		}
	}
	if (nLen == 0) {
		return FALSE;
	}
	char szTitle[512];
	char szLink[512];
	szTitle[0] = 0;
	char* pszTitleStart = strchr(pszBuf, '"');
	if (pszTitleStart) {
		char* pszTitleEnd = strchr(pszTitleStart, '"');
		if (pszTitleEnd) {
			*pszTitleEnd = 0;
			strcpy(szTitle, pszTitleStart + 1);
			while (pszTitleStart > pszBuf && pszTitleStart[-1] == ' ') {
				pszTitleStart--;
			}
			*pszTitleStart = 0;
		}
	}
	if (nWidth && nHeight) {
		pszTitleStart = strchr(pszBuf, '=');
		if (pszTitleStart) {
			if (pszTitleStart > pszBuf && pszTitleStart[-1] == ' ') {
				pszTitleStart[-1] = 0;
			}
			pszTitleStart++;
			char* pszHeight = strchr(pszTitleStart, 'x');
			if (pszHeight) {
				*pszHeight = 0;
				if (pszHeight > pszTitleStart) {
					*nWidth = atol(pszTitleStart);
				}
				*nHeight = atol(pszHeight + 1);
			}
		}
	}
	if (bAuto && strchr(pszBuf, '/') == 0) {
		sprintf(szLink, "https://%s", pszBuf);
	} else {
		strcpy(szLink, pszBuf);
	}
	*pszLink = _strdup(szLink);
	if (szTitle[0]) {
		*pszTitle = _strdup(szTitle);
	} else {
		*pszTitle = NULL;
	}
	return TRUE;
}

static BOOL mdr_parseLink(LINE* lp, int *pTextEnd, int* pStartPos, char** pszLink, char** pszTitle, int* nWidth, int* nHeight) {
	int i = *pStartPos;
	int nLinkStart = -1;
	while (i < lp->len) {
		char c = lp->lbuf[i++];
		if (nLinkStart < 0) {
			if (c == ']') {
				if (i < lp->len && lp->lbuf[i] == '(') {
					*pTextEnd = i - 1;
					i++;
					nLinkStart = i;
					// support special syntax: [text](<link>)
					if (lp->lbuf[i] == '<') {
						i++;
					}
				} else {
					return FALSE;
				}
			}
		} else if (c == ')') {
			char szBuf[256];
			*pStartPos = i-1;
			i -= nLinkStart;
			strncpy(szBuf, &lp->lbuf[nLinkStart], i-1);
			szBuf[i - 1] = 0;
			return mdr_parseLinkUrl(szBuf, pszLink, pszTitle, nWidth, nHeight);
		}
	}
	return FALSE;
}

/*
 * Parse a fenced code block and add the runs to the view part.
 */
static LINE* mdr_parseFencedCodeBlock(RENDER_VIEW_PART* pPart, LINE* lp, STRING_BUF* pSB, BOOL bIndented) {
	size_t nLastOffset = 0;
	int nOffs;

	while (lp) {
		nOffs = 0;
		if (bIndented) {
			if (lp->lbuf[0] == '\t') {
				nOffs = 1;
			} else {
				if (lp->len <= 3 || lp->lbuf[0] != ' ' || lp->lbuf[1] != ' ' || lp->lbuf[2] != ' ' || lp->lbuf[3] != ' ') {
					return lp;
				}
				nOffs = 4;
			}
		} else if (lp->len >= 3 && (strncmp(lp->lbuf, "```", 3) == 0 || strncmp(lp->lbuf, "~~~", 3) == 0)) {
			return lp->next;
		}
		pPart->rvp_number++;
		while (nOffs < lp->len) {
			char c = lp->lbuf[nOffs++];
			if (c == '\t') {
				size_t nCurrent = stringbuf_size(pSB) - nLastOffset;
				size_t nTab = (nCurrent + 4) / 4 * 4;
				while (nCurrent < nTab) {
					stringbuf_appendChar(pSB, ' ');
					nCurrent++;
				}
			} else {
				stringbuf_appendChar(pSB, c);
			}

		}
		size_t nSize = stringbuf_size(pSB) - nLastOffset;
		if (nSize == 0) {
			// do not skip empty lines.
			stringbuf_appendChar(pSB, ' ');
			nSize++;
		}
		mdr_appendRun(&pPart->rvp_data.rvp_flow.tf_runs, &_formatFenced, nSize, ATTR_LINE_BREAK);
		nLastOffset += nSize;
		lp = lp->next;
	}
	return lp;
}

/*
 * Parses the auto-link syntax.
 */
static BOOL mdr_parseAutolinks(TEXT_FLOW* pFlow, LINE* lp, STRING_BUF* pSB, MDR_ELEMENT_FORMAT* pFormat, int mAttrs, int* pOffset, size_t* pLastOffset) {
	int i = *pOffset;
	int nTextStart = i + 1;
	char* pLast = strchr(&lp->lbuf[nTextStart], '>');
	if (pLast && strchr(&lp->lbuf[i], '.') != 0) {
		size_t nSize = stringbuf_size(pSB) - *pLastOffset;
		mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, mAttrs);
		(*pLastOffset) += nSize;
		char szTemp[512];
		char* pszLink;
		char* pszTitle;
		int nTextEnd = (int)(pLast - lp->lbuf);
		nSize = nTextEnd - nTextStart;
		if (nSize >= sizeof szTemp) {
			nSize = sizeof szTemp-1;
		}
		strncpy(szTemp, &lp->lbuf[nTextStart], nSize);
		szTemp[nSize] = 0;
		stringbuf_appendStringLength(pSB, &lp->lbuf[nTextStart], nSize);
		nSize = stringbuf_size(pSB) - *pLastOffset;
		TEXT_RUN* pRun = mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, ATTR_LINK);
		int nWidth = 0;
		int nHeight = 0;
		if (!mdr_parseLinkUrl(szTemp, &pszLink, &pszTitle, &nWidth, &nHeight)) {
			return FALSE;
		}
		pRun->tr_link = pszLink;
		pRun->tr_title = pszTitle;
		if (pRun->tr_image) {
			pRun->tr_image->mdi_height = nHeight;
			pRun->tr_image->mdi_width = nWidth;
		}
		(* pLastOffset) += nSize;
		*pOffset = nTextEnd;
		return TRUE;
	}
	return FALSE;
}

static void mdr_applyFormat(RENDER_VIEW_PART* pPart, MDR_ELEMENT_FORMAT* pFormat) {
	pPart->rvp_margins = pFormat->mef_margins;
}

static BOOL mdr_isAtWordBorder(LINE* lp, int idx) {
	if (idx == 0) {
		return TRUE;
	}
	char c2 = lp->lbuf[idx - 1];
	if (!pks_isalnum(c2) && c2 != '\\') {
		return TRUE;
	}
	if (idx < lp->len - 1) {
		c2 = lp->lbuf[idx+1];
		if (pks_isalnum(c2)) {
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * Parse a top-level element to be rendered with a view part possibly containing nested formatting. 
 */
static LINE* mdr_parseFlow(LINE* lp, int nStartOffset, int nEndOffset, RENDER_VIEW_PART* pPart, TEXT_FLOW *pFlow, STRING_BUF* pSB, int nInitialAttrs) {
	stringbuf_reset(pSB);
	int nState = 0;
	int nLevel = 0;
	int mAttrs = nInitialAttrs;
	size_t nLastOffset = 0;
	size_t nSize;
	MDR_ELEMENT_FORMAT* pFormat = NULL;

	MDR_ELEMENT_TYPE mType = MET_NORMAL;
	while (lp) {
		BOOL bSkipped = FALSE;
		char c = 0;
		char lastC = 0;
		int nLast = nEndOffset < 0 ? lp->len : nEndOffset;
		BOOL bEnforceBreak = FALSE;
		for (int i = nStartOffset; i < nLast; i++) {
			lastC = c;
			c = lp->lbuf[i];
			if (i == 0 && pPart != NULL && (c == '\t' || (c == ' ' && lp->len >= 4 && lp->lbuf[1] == c && lp->lbuf[2] == c && lp->lbuf[3] == c 
					&& (lp->len == 4 || (lp->lbuf[4] != '-' && lp->lbuf[4] != '*'))))) {
				mType = MET_FENCED_CODE_BLOCK;
				pFormat = &_formatFenced;
				lp = mdr_parseFencedCodeBlock(pPart, lp, pSB, TRUE);
				nLastOffset = stringbuf_size(pSB);
				goto outer;
			} else if (nState <= 1 && string_isSpace(c)) {
				continue;
			}
			if (nState == 1 && mType == MET_BLOCK_QUOTE && c == '>' && !bSkipped) {
				bSkipped = TRUE;
				continue;
			}
			bEnforceBreak = FALSE;
			if (nState == 0) {
				if (pPart == NULL) {
					mType = MET_NORMAL;
					nState = 1;
					pFormat = &_formatText;
				} else {
					long nNumber;
					mType = mdr_determineTopLevelElement(lp, &i, &nLevel, &pFormat, &nNumber);
					if (mType == MET_HORIZONTAL_RULE) {
						pPart->rvp_paint = mdr_renderHorizontalRule;
						pPart->rvp_type = MET_HORIZONTAL_RULE;
						return lp->next;
					}
					if (mType == MET_ORDERED_LIST || mType == MET_TASK_LIST) {
						pPart->rvp_number = nNumber;
					}
					pPart->rvp_level = nLevel;
					if (mType == MET_FENCED_CODE_BLOCK) {
						lp = mdr_parseFencedCodeBlock(pPart, lp->next, pSB, FALSE);
						nLastOffset = stringbuf_size(pSB);
						goto outer;
					}
					if (mType != MET_NORMAL) {
						nState = 1;
						continue;
					}
				}
			}
			if (nState < 2) {
				nState = 2;
			}
			if (c == '\\' && i < lp->len - 1 && strchr(_escapedChars, lp->lbuf[i + 1]) != NULL) {
				i++;
				c = lp->lbuf[i];
			} else if (!(mAttrs & ATTR_CODE) && c == '=' && i < lp->len-1 && lp->lbuf[i+1] == c) {
				i++;
				nSize = stringbuf_size(pSB) - nLastOffset;
				mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, mAttrs);
				mAttrs ^= ATTR_HIGHLIGHT;
				nLastOffset += nSize;
				continue;
			} else if (c == '`' || (!(mAttrs & ATTR_CODE) && (c == '*' ||
				// allow for _ only at word borders.
					(c == '_' && mdr_isAtWordBorder(lp, i)) ||
				c == '~'))) {
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
					i++;
				} else if (c == '`') {
					nToggle = ATTR_CODE;
				}
				if (nToggle) {
					nSize = stringbuf_size(pSB) - nLastOffset;
					mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, mAttrs);
					mAttrs ^= nToggle;
					nLastOffset += nSize;
					continue;
				}
			} else if (c == '[' || (c == '!' && i < lp->len - 1 && lp->lbuf[i + 1] == '[')) {
				BOOL bImage = c == '!';
				int pos = (bImage) ? i + 2 : i + 1;
				int nTextStart = pos;
				int nTextEnd;
				char* pLink;
				char* pTitle = NULL;
				int nWidth = 0;
				int nHeight = 0;
				if (mdr_parseLink(lp, &nTextEnd, &pos, &pLink, &pTitle, &nWidth, &nHeight)) {
					int nAttr = 0;
					nSize = stringbuf_size(pSB) - nLastOffset;
					mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, mAttrs);
					nLastOffset += nSize;
					char c2 = lp->lbuf[nTextStart];
					if (c2 == lp->lbuf[nTextEnd - 1]) {
						if (c2 == '`') {
							nAttr = ATTR_CODE;
						}
						else if (c2 == '*' || c2 == '_') {
							if (lp->lbuf[nTextStart + 1] == c2) {
								nAttr = ATTR_STRONG;
								nTextStart++;
								nTextEnd--;
							} else {
								nAttr = ATTR_EMPHASIS;
							}
						}
						if (nAttr) {
							nTextStart++;
							nTextEnd--;
						}
					}
					stringbuf_appendStringLength(pSB, &lp->lbuf[nTextStart], nTextEnd - nTextStart);
					nSize = stringbuf_size(pSB) - nLastOffset;
					TEXT_RUN* pRun = mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, nAttr | ATTR_LINK);
					if (bImage) {
						pRun->tr_image = calloc(1, sizeof(MD_IMAGE));
						pRun->tr_image->mdi_height = nHeight;
						pRun->tr_image->mdi_width = nWidth;
					}
					if (bImage && !pTitle) {
						pTitle = _strdup(&stringbuf_getString(pSB)[nLastOffset]);
					}
					pRun->tr_link = pLink;
					pRun->tr_title = pTitle;
					nLastOffset += nSize;
					i = pos;
					continue;
				}
			} else if (c == '&') {
				BOOL bFound = FALSE;
				for (int nE = 0; _entities[nE].c; nE++) {
					size_t nLen = strlen(_entities[nE].entity);
					if (strncmp(&lp->lbuf[i+1], _entities[nE].entity, nLen) == 0) {
						stringbuf_appendChar(pSB, _entities[nE].c);
						i += (int)nLen;
						bFound = TRUE;
						continue;
					}
				}
				if (bFound) {
					continue;
				}
			} else if (c == '<') {
				if (strncmp(&lp->lbuf[i], "<br>", 4) == 0) {
					i += 3;
					while (i < lp->len - 1 && lp->lbuf[i+1] == ' ') {
						i++;
					}
					nSize = stringbuf_size(pSB) - nLastOffset;
					mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, mAttrs | ATTR_LINE_BREAK);
					bEnforceBreak = TRUE;
					nLastOffset += nSize;
					continue;
				}
				if (mdr_parseAutolinks(pFlow, lp, pSB, pFormat, mAttrs, &i, &nLastOffset)) {
					continue;
				}
			}
			stringbuf_appendChar(pSB, c);
		}
		if (c == ' ' && lastC == ' ') {
			nSize = stringbuf_size(pSB) - nLastOffset;
			mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, mAttrs|ATTR_LINE_BREAK);
			bEnforceBreak = TRUE;
			nLastOffset += nSize;
		}
		if (nEndOffset >= 0) {
			break;
		}
		lp = lp->next;
		if (mdr_isTopLevelOrBreak(lp, mType, nLevel)) {
			break;
		}
		nState = 1;
		if (!bEnforceBreak) {
			stringbuf_appendChar(pSB, ' ');
		}
	}
outer:
	if (pPart) {
		pPart->rvp_type = mType;
		if (pFormat) {
			mdr_applyFormat(pPart, pFormat);
		}
		pPart->rvp_paint = mdr_renderMarkdownBlockPart;
	}
	size_t nLen = stringbuf_size(pSB);
	pFlow->tf_text = nLen == 0 ? NULL : _strdup(stringbuf_getString(pSB));

	nSize = nLen - nLastOffset;
	mdr_appendRun(&pFlow->tf_runs, pFormat ? pFormat : &_formatText, nSize, mAttrs);

	return lp;
}

static RENDER_TABLE_CELL *mdr_parseTableCell(RENDER_TABLE_ROW* pRow, LINE* lp, BOOL bHeader, int nStartOffset, int nEndOffset) {
	// skip trailing spaces
	while (nEndOffset - 1 > nStartOffset && lp->lbuf[nEndOffset - 1] == ' ') {
		nEndOffset--;
	}
	if (nEndOffset > nStartOffset) {
		RENDER_TABLE_CELL* pCell = ll_append(&pRow->rtr_cells, sizeof * pCell);
		pCell->rtc_isHeader = bHeader;
		STRING_BUF* sb = stringbuf_create(128);
		mdr_parseFlow(lp, nStartOffset, nEndOffset, NULL, &pCell->rtc_flow, sb, bHeader ? ATTR_STRONG : 0);
		stringbuf_destroy(sb);
		return pCell;
	}
	return NULL;
}

/*
 * Parse a table row.
 */
static BOOL mdr_parseTableRow(LINE* lp, RENDER_TABLE* pTable, BOOL bHeader, const byte* columnAlignments, int nMaxColumns) {
	int nColumn = 0;
	int nStart = 0;
	RENDER_TABLE_ROW row;

	memset(&row, 0, sizeof row);
	for (int i = 0; i < lp->len && nColumn < nMaxColumns; i++) {
		char c = lp->lbuf[i];
		if (c == '\\' && i + 1 < lp->len) {
			i++;
		} else if (c == '|') {
			if (i > 0) {
				RENDER_TABLE_CELL* pCell = mdr_parseTableCell(&row, lp, bHeader, nStart, i);
				if (!pCell) {
					break;
				}
				pCell->rtc_align = columnAlignments[nColumn++];
			}
			nStart = i+1;
		}
	}
	if (nColumn == 0) {
		return FALSE;
	}
	if (nStart < lp->len && nColumn < nMaxColumns) {
		RENDER_TABLE_CELL* pCell = mdr_parseTableCell(&row, lp, bHeader, nStart, lp->len);
		if (pCell) {
			pCell->rtc_align = columnAlignments[nColumn++];
		}
	}
	RENDER_TABLE_ROW* pAppended = (RENDER_TABLE_ROW * )ll_append((LINKED_LIST**) & pTable->rt_rows, sizeof row);
	memcpy(pAppended, &row, sizeof row);
	return TRUE;
}

/*
 * Try to interpret some lines as a markdown table. If successful update the line pointer to point beyond the
 * table and return TRUE. If no successful, return FALSE;
 */
static BOOL mdr_parseTable(LINE** pFirst, RENDER_VIEW_PART* pPart) {
	byte columnAlignments[MAX_TABLE_COLUMNS];
	LINE* lp = *pFirst;
	LINE* lpDef = lp->next;
	if (!lpDef) {
		return FALSE;
	}
	memset(columnAlignments, 0, sizeof columnAlignments);
	BOOL bStartColon = FALSE;
	BOOL bDashSeen = FALSE;
	int nColumn = -1;
	int nAlign = RTC_ALIGN_LEFT;
	for (int i = 0; i < lpDef->len; i++) {
		char c = lpDef->lbuf[i];
		if (nColumn == -1) {
			if (pks_isspace(c)) {
				continue;
			}
			if (c == '|' || c == '-' || c== '=' || c == ':') {
				nColumn = 0;
				if (c == ':') {
					bStartColon = TRUE;
				}
				if (c == '-' || c == '=') {
					bDashSeen = TRUE;
				}
			} else {
				return FALSE;
			}
		} else {
			if (c == '-' || c == '=') {
				bDashSeen = TRUE;
			} else if (bDashSeen && c == ':') {
				nAlign = bStartColon ? RTC_ALIGN_CENTER : RTC_ALIGN_RIGHT;
			} else if (!bDashSeen && c == ':') {
				bStartColon = TRUE;
			} else if (bDashSeen && c == '|') {
				columnAlignments[nColumn++] = nAlign;
				bDashSeen = FALSE;
				bStartColon = FALSE;
				nAlign = RTC_ALIGN_LEFT;
			} else {
				return FALSE;
			}
		}
	}
	if (nColumn < 0) {
		return FALSE;
	}
	if (lpDef->lbuf[lpDef->len - 1] != '|') {
		columnAlignments[nColumn++] = nAlign;
	}
	// OK - we have a valid table definition. Now parse header and rows.
	RENDER_TABLE* pTable = calloc(1, sizeof * pTable);
	pTable->rt_columnCount = nColumn;
	if (!mdr_parseTableRow(lp, pTable, TRUE, columnAlignments, nColumn)) {
		free(pTable);
		return FALSE;
	}
	lp = lpDef->next;
	while (lp && mdr_parseTableRow(lp, pTable, FALSE, columnAlignments, nColumn)) {
		lp = lp->next;
	}
	RENDER_TABLE_ROW* pRow = pTable->rt_rows;
	// stupid simple layout mechanism assuming a ave char width of 8 to calculate
	// required column widths.
	while (pRow) {
		nColumn = 0;
		RENDER_TABLE_CELL* pCell = pRow->rtr_cells;
		while (pCell) {
			if (pCell->rtc_flow.tf_text) {
				size_t nLen = strlen(pCell->rtc_flow.tf_text);
				if (nLen > 100) {
					// Force wrapping of long column contents
					nLen = 100;
				}
				int nWidth = (int)nLen * 6;
				nWidth += 30 + _tableMargins.m_left + _tableMargins.m_right;
				if (nWidth > pTable->rt_columnWidths[nColumn]) {
					pTable->rt_columnWidths[nColumn] = nWidth;
				}
			}
			pCell = pCell->rtc_next;
			nColumn++;
		}
		pRow = pRow->rtr_next;
	}
	for (nColumn = 0; nColumn < MAX_TABLE_COLUMNS; nColumn++) {
		pTable->rt_totalColumnWidth += pTable->rt_columnWidths[nColumn];
	}
	pPart->rvp_data.rvp_table = pTable;
	*pFirst = lp;
	return TRUE;
}

static void mdr_parseViewParts(FTABLE* fp, MARKDOWN_RENDERER_DATA* pData) {
	LINE* lp = fp->firstl;
	STRING_BUF * pSB = stringbuf_create(256);
	RENDER_VIEW_PART* pReuse = NULL;
	int nDelta = 0;

	while (lp) {
		if (!ln_lineIsEmpty(lp)) {
			RENDER_VIEW_PART* pPart;
			pPart = pReuse == NULL ? ll_append(&pData->md_pElements, sizeof(RENDER_VIEW_PART)) : pReuse;
			pReuse = NULL;
			pPart->rvp_lpStart = lp;
			if (mdr_parseTable(&lp, pPart)) {
				pPart->rvp_type = MET_TABLE;
				mdr_applyFormat(pPart, &_formatTable);
				pPart->rvp_paint = mdr_renderTable;
			} else {
				lp = mdr_parseFlow(lp, 0, -1, pPart, &pPart->rvp_data.rvp_flow, pSB, 0);
			}
			if (pPart->rvp_type == MET_BLOCK_QUOTE && pPart->rvp_data.rvp_flow.tf_text == NULL) {
				pReuse = pPart;
			} else {
				pPart->rvp_margins.m_top += nDelta;
				nDelta = 0;
			}
		} else {
			lp = lp->next;
			nDelta += PARAGRAPH_OFFSET/2;
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
static RENDER_VIEW_PART* mdr_getViewPartForLine(RENDER_VIEW_PART* pFirstPart, long nLine) {
	return (RENDER_VIEW_PART * )ll_at((LINKED_LIST*)pFirstPart, nLine);
}

static void mdr_updateCaretUI(WINFO* wp, int* pCX, int* pCY, int* pWidth, int* pHeight) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, wp->caret.ln);
	*pCX = DEFAULT_LEFT_MARGIN;
	if (pPart) {
		*pCY = pPart->rvp_bounds.top;
		*pCX = pPart->rvp_bounds.left;
		*pHeight = pPart->rvp_bounds.bottom - pPart->rvp_bounds.top;
		if (*pHeight <= 0) {
			*pHeight = 1;
		}
	}
	*pWidth = 2;
}

static int mdr_rendererSupportsMode(int aMode) {
	if (aMode == SHOWCARET_LINE_HIGHLIGHT || aMode == SHOWLINENUMBERS || aMode == SHOWRULER) {
		return 0;
	}
	return 1;
}

static int mdr_windowSizeChanged(WINFO* wp) {
	// we do not support horizontal scrolling anyways - do not show a horizontal scrollbar.
	wp->maxcurscol = wp->maxcol = 15;
	wp->mincol = 0;
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (pData && pData->md_lastMinLn != wp->minln) {
		InvalidateRect(wp->ww_handle, (LPRECT)0, 0);
	}
	return 1;
}

/*
 * Calculate the number of logical lines displayed
 */
static long mdr_calculateMaxLine(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (pData) {
		if (!pData->md_pElements) {
			mdr_parseViewParts(wp->fp, pData);
		}
		return ll_size((LINKED_LIST*)pData->md_pElements);
	}
	// TODO: may we need to ensure the renderer is initialized
	return 20;
}

static int mdr_adjustScrollBounds(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	int nDelta = pData->md_nElementsPerPage;
	if (nDelta <= 0) {
		if (pData->md_pElements) {
			nDelta = 1;
		} else {
			return 0;
		}
	}
	long nMaxScreen = wp->minln + nDelta + 1;
	size_t nMax = mdr_calculateMaxLine(wp);
	if (nMaxScreen > nMax) {
		nMaxScreen = (long)nMax;
	}
	if (wp->caret.ln < nMaxScreen && wp->caret.ln >= wp->minln) {
		wp->maxcursln = wp->maxln = nMaxScreen;
		return 0;
	}
	if (wp->caret.ln < wp->minln) {
		wp->minln = wp->mincursln = wp->caret.ln;
	} else {
		if (pData->md_displayingEndOfFile) {
			return 0;
		}
		RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, nMaxScreen);
		nDelta = wp->caret.ln - nMaxScreen + 1;
		if (pPart) {
			nDelta += (pPart->rvp_bounds.bottom - pPart->rvp_bounds.top) / 16;
		}
		wp->minln += nDelta;
		if (wp->minln >= nMax-3) {
			wp->minln = (long)nMax - 2;
		}
	}
	if (wp->minln < 0) {
		wp->minln = 0;
	}
	wp->mincursln = wp->minln;
	wp->maxcursln = wp->mincursln + nDelta;
	if (wp->maxcursln > nMaxScreen) {
		wp->maxcursln = nMaxScreen;
	}
	wp->maxln = wp->maxcursln;
	InvalidateRect(wp->ww_handle, (LPRECT)0, 0);
	UpdateWindow(wp->ww_handle);
	if (pData->md_hwndTooltip) {
		ShowWindow(pData->md_hwndTooltip, SW_HIDE);
	}
	int width;
	mdr_updateCaretUI(wp, &wp->cx, &wp->cy, &width, &wp->cheight);
	render_updateCaret(wp);
	sl_size(wp);	
	return 1;
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
	BOOL bSizeChanged = FALSE;
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	if (rect.right != pData->md_lastBounds.right || rect.bottom != pData->md_lastBounds.bottom || wp->minln != pData->md_lastMinLn) {
		bSizeChanged = TRUE;
		pData->md_lastMinLn = wp->minln;
		CopyRect(&pData->md_lastBounds, &rect);
		while (pPart) {
			// clear out old existing bounds.
			memset(&pPart->rvp_bounds, 0, sizeof pPart->rvp_bounds);
			pPart = pPart->rvp_next;
		}
	}
	// Todo - highlight caret line
	HBRUSH hBrushCaretLine = CreateSolidBrush(pCtx->rc_theme->th_caretLineColor);
	if (!pData->md_pElements) {
		mdr_parseViewParts(fp, pData);
	}
	pPart = mdr_getViewPartForLine(pData->md_pElements, wp->minln);
	RECT occupiedBounds;
	int nElements = 0;
	FillRect(pCtx->rc_hdc, pClip, hBrushBg);
	for (; pPart && ((bSizeChanged && rect.top < rect.bottom) || (!bSizeChanged && rect.top < pClip->bottom)); rect.top = occupiedBounds.bottom) {
		MDR_ELEMENT_TYPE mNextType = pPart->rvp_type;
		pPart->rvp_paint(wp, pPart, pCtx->rc_hdc, &rect, &occupiedBounds);
		if (occupiedBounds.bottom > rect.bottom) {
			break;
		}
		pPart = pPart->rvp_next;
		nElements++;
	}
	DeleteObject(hBrushCaretLine);
	if (bSizeChanged) {
		pData->md_nElementsPerPage = nElements-1;
		pData->md_displayingEndOfFile = pPart == NULL;
		//mdr_adjustScrollBounds(wp);
	}
}

/*
 * Destroy a text run possibly release the link. 
 */
static int mdr_destroyRun(TEXT_RUN* pRun) {
	free(pRun->tr_link);
	free(pRun->tr_title);
	if (pRun->tr_image) {
		if (pRun->tr_image->mdi_image) {
			DeleteObject(pRun->tr_image->mdi_image);
		}
		free(pRun->tr_image);
	}
	return 1;
}

/*
 * Destroy a table cell.
 */
static int mdr_destroyTableCell(RENDER_TABLE_CELL* pCell) {
	free(pCell->rtc_flow.tf_text);
	ll_destroy(&pCell->rtc_flow.tf_runs, mdr_destroyRun);
	return 1;
}

/*
 * Destroy a table row.
 */
static int mdr_destroyTableRow(RENDER_TABLE_ROW* pRow) {
	ll_destroy(&pRow->rtr_cells, mdr_destroyTableCell);
	return 1;
}

/*
 * Destroy one view part. 
 */
static int mdr_destroyViewPart(RENDER_VIEW_PART *pRVP) {
	if (pRVP->rvp_type == MET_TABLE) {
		ll_destroy(&pRVP->rvp_data.rvp_table->rt_rows, mdr_destroyTableRow);
		free(pRVP->rvp_data.rvp_table);
	} else {
		free(pRVP->rvp_data.rvp_flow.tf_text);
		ll_destroy(&pRVP->rvp_data.rvp_flow.tf_runs, mdr_destroyRun);
	}
	return 1;
}

/*
 * Destroy the renderer data. 
 */
static void mdr_destroyData(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (pData) {
		if (pData->md_hwndTooltip) {
			DestroyWindow(pData->md_hwndTooltip);
		}
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
		InvalidateRect(wp->ww_handle, (LPRECT)0, 0);
		UpdateWindow(wp->ww_handle);
		//RedrawWindow(wp->ww_handle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
	}
}

/*
 * Allocate the markdown renderer data structure.
 */
static void* mdr_allocData(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pResult = calloc(1, sizeof(MARKDOWN_RENDERER_DATA));
	pResult->md_hwndTooltip = cust_createToolTooltip(wp->ww_handle);
	return pResult;
}

/*
 * Calculate the "longest" line displayed. We do not support horizontal scrolling.
 * Calculate the longes line to be rather small
 */
static long mdr_calculateLongestLine(WINFO* wp) {
	return 10;
}

/*
 * Calculate the maximum column for column moves. 
 */
static long mdr_calculateMaxColumn(WINFO* wp, long ln, LINE* lp) {
	return 10;
}

/*
 * Hit testing not yet implemented.
 */
static int mdr_screenOffsetToBuffer(WINFO* wp, long ln, long col, INTERNAL_BUFFER_POS* pPosition) {
	// not implemented
	return 0;
}

static void mdr_scroll(WINFO* wp, int dx, int dy) {
	if (!dy) {
		return;
	}
	RedrawWindow(wp->ww_handle, NULL, NULL, RDW_ERASE|RDW_INVALIDATE);
}

static int mdr_placeCursorAndValidate(WINFO* wp, long* ln, long offset, const long* col, int updateVirtualOffset, int xDelta) {
	if (*ln < 0) {
		return 0;
	}
	long lnMax = mdr_calculateMaxLine(wp);
	if (*ln >= lnMax) {
		*ln = lnMax > 0 ? lnMax-1 : 0;
	}
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, *ln);
	if (pPart) {
		wp->caret.linePointer = pPart->rvp_lpStart;
		if (pPart->rvp_data.rvp_flow.tf_runs) {
			wp->caret.col = *col;
		}
	}
	wp->caret.ln = *ln;
	wp->caret.offset = offset;
	return 1;
}

static BOOL mdr_hitTestTextRuns(TEXT_RUN* pRuns, POINT pPoint, long* pCol, TEXT_RUN** pMatchedRun) {
	long nCol = 0;
	while (pRuns) {
		if (runbounds_contains(&pRuns->tr_bounds, pPoint)) {
			*pCol = nCol;
			*pMatchedRun = pRuns;
			return TRUE;
		}
		pRuns = pRuns->tr_next;
		nCol++;
	}
	return FALSE;


	return FALSE;
}

static BOOL mdr_hitTestInternal(WINFO* wp, int cx, int cy, long* pLine, long* pCol, RENDER_VIEW_PART** pMatchedPart, TEXT_RUN **pMatchedRun) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return FALSE;
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, wp->minln);
	POINT pt = { cx, cy };
	long ln = wp->minln;
	while (pPart) {
		if (PtInRect(&pPart->rvp_bounds, pt)) {
			if (pPart->rvp_type == MET_TABLE) {
				RENDER_TABLE_ROW* pRows = pPart->rvp_data.rvp_table->rt_rows;
				int nCol = 0;
				while (pRows) {
					RENDER_TABLE_CELL* pCell = pRows->rtr_cells;
					while (pCell) {
						long intraCol;
						if (mdr_hitTestTextRuns(pCell->rtc_flow.tf_runs, (POINT) { cx, cy }, &intraCol, pMatchedRun)) {
							*pMatchedPart = pPart;
							*pLine = ln;
							*pCol = intraCol + nCol;
							return TRUE;
						}
						nCol += ll_size((LINKED_LIST * )pCell->rtc_flow.tf_runs);
						pCell = pCell->rtc_next;
					}
					pRows = pRows->rtr_next;
				}
			} else if (mdr_hitTestTextRuns(pPart->rvp_data.rvp_flow.tf_runs, (POINT) { cx, cy }, pCol, pMatchedRun)) {
				*pMatchedPart = pPart;
				*pLine = ln;
				return TRUE;
			}
			return FALSE;
		}
		pPart = pPart->rvp_next;
		ln++;
	}
	return FALSE;
}

static void mdr_hitTest(WINFO* wp, int cx, int cy, long* pLine, long* pCol) {
	RENDER_VIEW_PART* pMatchP;
	TEXT_RUN* pMatchR;
	mdr_hitTestInternal(wp, cx, cy, pLine, pCol, &pMatchP, &pMatchR);
}

static void mdr_setRollover(HWND hwnd, TEXT_RUN* pRun, BOOL aFlag) {
	if (!pRun || pRun->tr_attributes.rollover == aFlag || !pRun->tr_link) {
		return;
	}
	pRun->tr_attributes.rollover = aFlag;
	InvalidateRect(hwnd, (RECT*) & pRun->tr_bounds, FALSE);
}

static void mdr_mouseMove(WINFO* wp, int x, int y) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return;
	}
	RENDER_VIEW_PART* pMatchP;
	TEXT_RUN* pMatchR = NULL;
	long line;
	long col;
	BOOL bShow = TRUE;
	if (!mdr_hitTestInternal(wp, x, y, &line, &col, &pMatchP, &pMatchR)) {
		bShow = FALSE;
	}
	if (pMatchR == pData->md_focussedRun) {
		return;
	}
	mdr_setRollover(wp->ww_handle, pData->md_focussedRun, FALSE);
	pData->md_focussedRun = pMatchR;
	mdr_setRollover(wp->ww_handle, pMatchR, TRUE);
	TTTOOLINFO toolinfo = { 0 };
	toolinfo.cbSize = sizeof(toolinfo);
	toolinfo.hwnd = wp->ww_handle;
	toolinfo.hinst = hInst;
	if (pMatchR && pMatchR->tr_title) {
		// Activate the tooltip.
		toolinfo.lpszText = pMatchR->tr_title;
		SendMessage(pData->md_hwndTooltip, TTM_UPDATETIPTEXT, (WPARAM)0, (LPARAM)&toolinfo);
		POINT pt;
		pt.x = pMatchR->tr_bounds.left1;
		pt.y = pMatchR->tr_bounds.top1 - 20;
		ClientToScreen(wp->ww_handle, &pt);
		SendMessage(pData->md_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x, pt.y));
	} else {
		bShow = FALSE;
	}
	if (!SendMessage(pData->md_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)bShow, (LPARAM)&toolinfo)) {
		log_errorArgs(DEBUG_ERR, "Activating tooltip failed. Error %ld.", GetLastError());
	}
}

static TEXT_RUN* mdr_getRunAtOffset(RENDER_VIEW_PART* pPart, int nOffset) {
	if (pPart->rvp_type == MET_TABLE) {
		RENDER_TABLE_ROW* pRows = pPart->rvp_data.rvp_table->rt_rows;
		int nOffsCurr = 0;
		while (pRows) {
			RENDER_TABLE_CELL* pCells = pRows->rtr_cells;
			while (pCells) {
				int nCount = ll_size((LINKED_LIST*)pCells->rtc_flow.tf_runs);
				nOffsCurr += nCount;
				if (nOffsCurr > nOffset) {
					return (TEXT_RUN*)ll_at((LINKED_LIST*)pCells->rtc_flow.tf_runs, nOffset-nOffsCurr + nCount);
				}
				pCells = pCells->rtc_next;
			}
			pRows = pRows->rtr_next;
		}
		return NULL;
	}
	else if (pPart->rvp_data.rvp_flow.tf_runs) {
		return (TEXT_RUN*)ll_at((LINKED_LIST*)pPart->rvp_data.rvp_flow.tf_runs, nOffset);
	}
	return NULL;
}

static BOOL mdr_findLink(WINFO* wp, char* pszBuf, size_t nMaxChars, NAVIGATION_INFO_PARSE_RESULT* pResult) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, wp->caret.ln);
	if (pPart) {
		TEXT_RUN* pRun = mdr_getRunAtOffset(pPart, wp->caret.col);
		if (pRun && pRun->tr_link && strlen(pRun->tr_link) < nMaxChars) {
			strcpy(pszBuf, pRun->tr_link);
			char* pszAnchor = strrchr(pszBuf, '#');
			if (pszAnchor) {
				*pszAnchor++ = 0;
				pResult->ni_anchor = pszAnchor;
			}
			pResult->ni_reference = pszBuf;
			pResult->ni_lineNumber = 0;
			pResult->ni_displayMode = wp->dispmode;
			pResult->ni_wp = mainframe_getDockName(wp->edwin_handle);
			return 1;
		}
	}
	return FALSE;
}

static void mdr_navigateToAnchor(WINFO* wp, const char* pszAnchor) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return;
	}
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	for (long nLine = 0; pPart; nLine++) {
		if (pPart->rvp_type == MET_HEADER) {
			char szTitleAsAnchor[512];
			char* pszTitle = pPart->rvp_data.rvp_flow.tf_text;
			if (strlen(pszTitle) < sizeof szTitleAsAnchor) {
				char* pszDest = szTitleAsAnchor;
				while (*pszTitle) {
					char c = *pszTitle++;
					if (c == ' ') {
						c = '-';
					} else if (pks_isupper(c)) {
						c = tolower(c);
					}
					*pszDest++ = c;
				}
				*pszDest = 0;
				if (strcmp(pszAnchor, szTitleAsAnchor) == 0) {
					int nCol = 0;
					caret_placeCursorAndValidate(wp, &nLine, 0, &nCol, 0, 0);
					wt_curpos(wp, nLine, 0);
					return;
				}
			}
		}
		pPart = pPart->rvp_next;
	}
}

static int mdr_updateDueToMouseClick(WINFO* wp, long* ln, long* col, int updateVirtualColumn) {
	return mdr_placeCursorAndValidate(wp, ln, *col, col, updateVirtualColumn, 0);
}

static RENDERER _mdrRenderer = {
	render_singleLineOnDevice,
	mdr_renderPage,
	mdr_placeCursorAndValidate,
	mdr_calculateMaxLine,
	mdr_calculateLongestLine,
	mdr_calculateMaxColumn,
	mdr_updateDueToMouseClick,
	mdr_screenOffsetToBuffer,
	mdr_allocData,
	mdr_destroyData,
	mdr_scroll,
	mdr_windowSizeChanged,
	mdr_adjustScrollBounds,
	mdr_updateCaretUI,
	mdr_rendererSupportsMode,
	mdr_hitTest,
	FALSE,
	mdr_modelChanged,
	.r_context = "markdown-renderer",
	mdr_findLink,
	mdr_navigateToAnchor,
	mdr_mouseMove
};

/*
 * Returns a markdown renderer.
 */
RENDERER* mdr_getRenderer() {
	return &_mdrRenderer;
}


