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
#include <windowsx.h>
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
#include "jsonparser.h"
#include "hashmap.h"
#include "streams.h"
#include "htmlrendering.h"

#define NO_COLOR			-1		// marker for no color defined
#define PARAGRAPH_OFFSET	15		// offset in pixels between paragraph type elements.
#define DEFAULT_LEFT_MARGIN	10		// offset of most elements to the left of the screen.

#define IS_UNORDERED_LIST_START(c)	(c == '-' || c == '*' || c == '+')

extern HINSTANCE		hInst;

static const char _escapedChars[] = "\\`*_{}[]<>()#+-.!|&";

// Loads the an image with a name and a format into a HBITMAP.
extern HBITMAP loadimage_load(char* pszName);

typedef void (*RENDER_PAINT)(RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed, float zoomFactor, BOOL bMeasureOnly);

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
	{'@', "#64;"},
	{'/', "#47;"},
	{' ', "nbsp;"},
	{'®', "reg;"},
	{'¥', "yen;"}
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
	int		indent;
	BOOL	rollover;
	BOOL    superscript;
	BOOL    subscript;
	COLORREF bgColor;
	COLORREF fgColor;
	int		fixedFont;
} FONT_ATTRIBUTES;

typedef struct tagMD_IMAGE {
	int					mdi_width;
	int					mdi_height;
	int					mdi_borderWidth;
	COLORREF			mdi_borderColor;
	HBITMAP				mdi_image;
} MD_IMAGE;

typedef struct tagFONT_STYLE_DELTA {
	int fsd_logicalStyles;
	int fsd_indent;
	COLORREF fsd_textColor;
	const char* fsd_styleName;
	COLORREF fsd_fillColor;
	COLORREF fsd_strokeColor;
	int fsd_strokeWidth;
} FONT_STYLE_DELTA;


typedef struct tagTEXT_RUN {
	struct tagTEXT_RUN* tr_next;
	LINE*				tr_lp;					
	int					tr_lineOffset;			// The line offset where the text run starts
	size_t				tr_size;
	RUN_BOUNDS			tr_bounds;
	FONT_ATTRIBUTES		tr_attributes;
	char*				tr_link;
	char*				tr_title;
	char*				tr_anchor;
	MD_IMAGE*			tr_image;
	int					tr_selectionStart;
	int					tr_selectionLength;
} TEXT_RUN;

/*
 * Forward decl.: destroy a text run possibly release the link.
 */
static int mdr_destroyRun(TEXT_RUN* pRun);

typedef struct tagTEXT_FLOW {
	char* tf_text;
	TEXT_RUN* tf_runs;
} TEXT_FLOW;

typedef enum { 
	MET_NONE = -1,				// marker element type for HTML parsing
	MET_BLOCK_QUOTE, 
	MET_ORDERED_LIST, 
	MET_UNORDERED_LIST, 
	MET_IMAGE, 
	MET_TASK_LIST,
	MET_NORMAL,
	MET_FENCED_CODE_BLOCK,
	MET_HEADER, 
	MET_TABLE,
	MET_PARAGRAPH,
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
	int rt_borderWidth;			// width of the table border (by default 1)
	float rt_width;				// width of total screen size in percent or 0 to use default size
} RENDER_TABLE;

typedef struct tagMARGINS {
	int	m_top;
	int	m_bottom;
	int	m_left;
	int	m_right;
} MARGINS;

typedef struct tagRENDER_BOX_DECORATION {
	COLORREF rbd_fillColor;				// background fill color or NO_COLOR
	COLORREF rbd_strokeColor;			// box stroke color or NO_COLOR
	int      rbd_strokeWidth;			// width of the stroke pen
	BOOL	 rbd_useThemeFillColor;		// Flag: we use a default theme dependent fill color
	BOOL	 rbd_useThemeStrokeColor;	// Flag: we use a default theme dependent stroke color
} RENDER_BOX_DECORATION;

struct tagRENDER_VIEW_PART {
	struct tagRENDER_VIEW_PART* rvp_next;
	LINE* rvp_lpStart;
	RENDER_PAINT rvp_paint;
	MARGINS rvp_margins;
	RENDER_BOX_DECORATION* rvp_decoration; // optional box decoration for a view
	RECT rvp_bounds;					// Bounds of an element excluding the margins.
	int rvp_height;						// The height in pixels.
	BOOL rvp_layouted;					// TRUE, if the layout bounds of the view part are valid.
	MDR_ELEMENT_TYPE rvp_type;
	long rvp_number;					// for numbered lists
	int	rvp_level;						// for headers and lists - the level.
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
	RENDER_BOX_DECORATION* mef_decoration;
} MDR_ELEMENT_FORMAT;

#define ATTR_EMPHASIS	0x1
#define ATTR_STRONG		0x2
#define ATTR_STRIKE		0x4
#define ATTR_CODE		0x8
#define ATTR_HIGHLIGHT	0x10
#define ATTR_LINK		0x20
#define ATTR_LINE_BREAK 0x40
#define ATTR_UNDERLINE	0x80
#define ATTR_TAG_CODE	0x100
#define ATTR_SUPER		0x200
#define ATTR_SUB		0x400

typedef struct tagRENDER_FLOW_PARAMS {
	HDC rfp_hdc;
	BOOL rfp_skipSpace;
	float rfp_zoomFactor;
	BOOL rfp_measureOnly;
} RENDER_FLOW_PARAMS;

/*
 * Render a "text flow" - a simple text which contains styled regions aka text runs.
 */
static void mdr_renderTextFlow(MARGINS* pMargins, TEXT_FLOW* pFlow, RECT* pBounds, 
	RECT* pPartBounds, RECT* pUsed, int nBlockQuoteLevel, int nAlign, RENDER_FLOW_PARAMS *pRFP);

static MDR_ELEMENT_FORMAT _formatText = {
	0, 0, DEFAULT_LEFT_MARGIN, 20, 14, FW_NORMAL
};

static RENDER_BOX_DECORATION _fencedBlock = {
	.rbd_useThemeFillColor = 1,
	.rbd_useThemeStrokeColor = 1,
	.rbd_strokeWidth = 1
};

static MDR_ELEMENT_FORMAT _formatFenced = {
	12, 6, 20, 20, 14, FW_BOLD, 1, .mef_decoration = &_fencedBlock
};

#define BLOCK_QUOTE_INDENT		25
#define BLOCK_QUOTE_MARGIN		4

static MDR_ELEMENT_FORMAT _formatParagraph = {
	0, 10, DEFAULT_LEFT_MARGIN, 10, 14, FW_NORMAL
};

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
	HWND md_hwndTooltip;
	TEXT_RUN* md_focussedRun;
	long md_minln;
	RENDER_VIEW_PART* md_caretView;
	int md_flowCaretIndex;
	int md_caretLineIndex;
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
	static LOGFONT _defaultFont = {
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
	};
	LOGFONT _lf;
	if (!_defaultFont.lfFaceName[0]) {
		const char* pszStyle = theme_textStyleFontface("wysiwyg", "Verdana");
		strcpy(_defaultFont.lfFaceName, pszStyle);
	}
	memcpy(&_lf, &_defaultFont, sizeof _lf);
	long nHeight = (long)(pAttrs->size * fZoom);
	if (pAttrs->subscript || pAttrs->superscript) {
		nHeight /= 2;
	}
	_lf.lfHeight = nHeight;
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

static void mdr_renderHorizontalRule(RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed, float zoomFactor, BOOL bMeasureOnly) {
	pUsed->top = pBounds->top + pPart->rvp_margins.m_top;
	pUsed->left = pBounds->left + DEFAULT_LEFT_MARGIN;
	pUsed->bottom = pUsed->top + DEFAULT_LEFT_MARGIN + pPart->rvp_margins.m_bottom;
	pUsed->right = pBounds->right - 20;
	if (!bMeasureOnly) {
		mdr_paintRule(hdc, pUsed->left, pUsed->right, (pUsed->top + pUsed->bottom - 3) / 2, 3);
	}
	memcpy(&pPart->rvp_bounds, pUsed, sizeof * pUsed);
	pPart->rvp_height = pUsed->bottom - pBounds->top;
	pPart->rvp_layouted = TRUE;
}

/*
 * Render a markdown table. 
 */
static void mdr_renderTable(RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed, float zoomFactor, BOOL bMeasureOnly) {
	THEME_DATA* pTheme = theme_getCurrent();
	int nColumnWidth[MAX_TABLE_COLUMNS];
	struct tagROW_BORDER {
		int rb_header;
		int rb_y;
	} rowBorders[128];
	int nRowBorderIndex = 0;
	RENDER_TABLE* pTable = pPart->rvp_data.rvp_table;
	int nColumns = pTable->rt_columnCount;
	int left = pBounds->left+pPart->rvp_margins.m_left;
	int right = pBounds->right - pPart->rvp_margins.m_right;
	int nWidth = right - left;
	if (pTable->rt_width > 0) {
		nWidth = (int)(nWidth * pTable->rt_width);
		right = left + nWidth;
	}
	int nTotal = pTable->rt_totalColumnWidth;
	if (nTotal == 0) {
		nTotal = 1;
	}
	if (nWidth > nTotal && pTable->rt_width == 0) {
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
	RENDER_FLOW_PARAMS rfp = {
		.rfp_skipSpace = TRUE,
		.rfp_hdc = hdc,
		.rfp_zoomFactor = zoomFactor,
		.rfp_measureOnly = bMeasureOnly
	};
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
			if (pCell->rtc_flow.tf_text) {
				mdr_renderTextFlow(&_tableMargins, &pCell->rtc_flow, &bounds, &flowBounds, &usedBounds, 0, pCell->rtc_align, &rfp);
			}
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
		if (pRow && !bMeasureOnly) {
			rowBorders[nRowBorderIndex].rb_y = bounds.top + _tableMargins.m_bottom / 2;
			rowBorders[nRowBorderIndex].rb_header = bHeader;
			if (nRowBorderIndex < DIM(rowBorders)-1) {
				nRowBorderIndex++;
			}
		}
	}
	int nTableHeight = bounds.top - y + _tableMargins.m_bottom;
	if (!bMeasureOnly && pTable->rt_borderWidth > 0) {
		HPEN hPen = CreatePen(PS_SOLID, pTable->rt_borderWidth, pTheme->th_dialogLightBackground);
		HPEN hPenOld = SelectObject(hdc, hPen);
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, left, y, right, y + nTableHeight);
		int x = left;
		int nDelta = pTable->rt_borderWidth / 2;
		for (int i = 0; i < nColumns; i++) {
			x += nColumnWidth[i];
			MoveTo(hdc, x, y);
			LineTo(hdc, x, y + nTableHeight - nDelta);
		}
		for (int i = 0; i < nRowBorderIndex; i++) {
			int nLineY = rowBorders[i].rb_y;
			MoveTo(hdc, left, nLineY);
			LineTo(hdc, right, nLineY);
			if (rowBorders[i].rb_header) {
				MoveTo(hdc, left, nLineY + 1);
				LineTo(hdc, right, nLineY + 1);
			}
		}
		DeleteObject(SelectObject(hdc, hPenOld));
	}
	pUsed->top = y;
	pUsed->left = left;
	pUsed->right = right;
	pUsed->bottom = pUsed->top + nTableHeight;
	memcpy(&pPart->rvp_bounds, pUsed, sizeof * pUsed);
	pPart->rvp_height = pUsed->bottom - pBounds->top;
	pPart->rvp_layouted = TRUE;
}

/*
 * Paint an image displayed in a markdown text.
 */
static void mdr_paintImage(HDC hdc, TEXT_RUN* pTR, int x, int y, SIZE* pSize, float zoomFactor, BOOL bMeasureOnly) {
	MD_IMAGE* pImage = pTR->tr_image;
	if (!pImage) {
		pSize->cx = 20;
		pSize->cy = 20;
		return;
	}
	if (!pImage->mdi_image) {
		char* pszExt = strrchr(pTR->tr_link, '.');
		if (pszExt && _stricmp(pszExt + 1, "bmp") != 0) {
			pImage->mdi_image = loadimage_load(pTR->tr_link);
		} else {
			pImage->mdi_image = LoadImage(NULL, pTR->tr_link, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		}
	}
	if (pImage->mdi_image) {
		BITMAP          bitmap;
		HGDIOBJ         oldBitmap;
		HDC             hdcMem;
		GetObject(pImage->mdi_image, sizeof(bitmap), &bitmap);
		int nWidth = pImage->mdi_width ? pImage->mdi_width : 
			(pImage->mdi_height ? bitmap.bmWidth * pImage->mdi_height / bitmap.bmHeight : bitmap.bmWidth);
		int nHeight = pImage->mdi_height ? pImage->mdi_height : 
			(pImage->mdi_width ? bitmap.bmHeight * pImage->mdi_width / bitmap.bmWidth : bitmap.bmHeight);
		nWidth = (int)(nWidth * zoomFactor);
		nHeight = (int)(nHeight * zoomFactor);
		int nBorder = pImage->mdi_borderWidth;
		if (!bMeasureOnly) {
			hdcMem = CreateCompatibleDC(hdc);
			oldBitmap = SelectObject(hdcMem, pImage->mdi_image);
			SetStretchBltMode(hdc, COLORONCOLOR);
			int nDelta = nBorder / 2;
			StretchBlt(hdc, x+nDelta, y + nDelta, nWidth, nHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
			SelectObject(hdcMem, oldBitmap);
			DeleteDC(hdcMem);
			if (nBorder) {
				HPEN hPen = CreatePen(PS_SOLID, nBorder, pImage->mdi_borderColor);
				HBRUSH hBrush = GetStockBrush(NULL_BRUSH);
				HBRUSH hBrushOld = SelectObject(hdc, hBrush);
				HPEN hPenOld = SelectObject(hdc, hPen);
				Rectangle(hdc, x, y, x + nWidth + nBorder, y + nHeight + nBorder);
				SelectObject(hdc, hBrushOld);
				DeleteObject(SelectObject(hdc, hPenOld));
			}
		}
		pSize->cx = nWidth+nBorder;
		pSize->cy = nHeight+nBorder;
	} else {
		char szFilename[512];
		sprintf(szFilename, "Cannot load image %s", pTR->tr_link);
		if (!bMeasureOnly) {
			TextOut(hdc, x, y, szFilename, (int)strlen(szFilename));
		}
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

static void mdr_paintSelection(HDC hdc, int x, int y, RENDER_FLOW_PARAMS* pRFP, TEXT_RUN* pRun, int nOffs, int nDeltaPainted, int nFit, TEXT_FLOW* pFlow) {
	SIZE startSize;
	SIZE selectionSize;
	int nUnused;
	char* pszText = pFlow->tf_text;

	int nStart = pRun->tr_selectionStart - nDeltaPainted;
	if (nStart < 0 || nStart > nFit) {
		return;
	}
	GetTextExtentExPoint(hdc, &pszText[nOffs], nStart,
		20000, &nUnused, 0, &startSize);
	GetTextExtentExPoint(hdc, &pszText[nOffs + nStart], pRun->tr_selectionLength,
		2000, &nUnused, 0, &selectionSize);
	RECT selectionRect;
	selectionRect.top = y;
	selectionRect.bottom = selectionRect.top + selectionSize.cy;
	selectionRect.left = x + startSize.cx;
	selectionRect.right = selectionRect.left + selectionSize.cx;
	render_paintSelectionRect(hdc, &selectionRect);
}

/*
 * Render a "text flow" - a simple text which contains styled regions aka text runs.
 */
static void mdr_renderTextFlow(MARGINS* pMargins, TEXT_FLOW* pFlow, RECT* pBounds, RECT* pPartBounds, 
		RECT* pUsed, int nBlockQuoteLevel, int nAlign, RENDER_FLOW_PARAMS *pRFP) {
	THEME_DATA* pTheme = theme_getCurrent();
	int x = pBounds->left + pMargins->m_left;
	int y = pBounds->top + pMargins->m_top;
	int nRight = pBounds->right - pMargins->m_right;
	int nDeltaPainted;
	pUsed->top = pBounds->top;
	pUsed->left = x;
	pUsed->right = nRight;
	TEXT_FLOW* pTF = pFlow;
	TEXT_RUN* pTR = pTF->tf_runs;
	int nOffs = 0;
	size_t nLen;
	nLen = pTR->tr_size;
	int nHeight = 0;
	HDC hdc = pRFP->rfp_hdc;
	font_setDefaultTextColors(hdc, pTheme);
	COLORREF cDefault = GetTextColor(hdc);
	HFONT hFont = mdr_createFont(&pTR->tr_attributes, pRFP->rfp_zoomFactor);
	HFONT hOldFont = SelectObject(hdc, hFont);
	if (pRFP->rfp_skipSpace) {
		while (nLen > 0 && isspace((unsigned char)pTF->tf_text[nOffs])) {
			nOffs++;
			nLen--;
		}
	}
	pPartBounds->top = y;
	BOOL bRunBegin = TRUE;
	while (pTR) {
		SIZE size;
		int nFit = 0;
		x += pTR->tr_attributes.indent;
		if (pTR->tr_image) {
			mdr_paintImage(hdc, pTR, x, y, &size, pRFP->rfp_zoomFactor, pRFP->rfp_measureOnly);
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
				nDeltaPainted = 0;
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
			SetTextColor(hdc, pTR->tr_attributes.fgColor == NO_COLOR ? cDefault : pTR->tr_attributes.fgColor);
			int nSuperDelta = 2 * size.cy / 3;
			if (pTR->tr_attributes.subscript) {
				nDelta += nSuperDelta;
			}
			else if (pTR->tr_attributes.superscript) {
				nDelta -= nSuperDelta;
			}
			if (!pRFP->rfp_measureOnly) {
				TextOut(hdc, x + nDeltaX, y + nDelta, &pTF->tf_text[nOffs], nFit);
				if (pTR->tr_selectionLength) {
					mdr_paintSelection(hdc, x + nDeltaX, y + nDelta, pRFP, pTR, nOffs, nDeltaPainted, nFit, pTF);
				}
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
		}
		if (!pTR->tr_image && nFit < nLen && nFit > 0) {
			nOffs += nFit;
			nDeltaPainted += nFit;
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
				nDeltaPainted++;
			}
			bRunBegin = FALSE;
		}
		else {
			if (pTR->tr_attributes.lineBreak) {
				x = pUsed->left;
				y += nHeight;
				nHeight = 0;
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
			hFont = mdr_createFont(&pTR->tr_attributes, pRFP->rfp_zoomFactor);
			hOldFont = SelectObject(hdc, hFont);
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

static void mdr_paintFillDecoration(HDC hdc, RENDER_VIEW_PART* pPart, RECT* pBounds) {
	MARGINS* pMargins = &pPart->rvp_margins;
	int x = pBounds->left + pMargins->m_left;
	int y = pBounds->top + pMargins->m_top;
	int nRight = pBounds->right - pMargins->m_right;

	RENDER_BOX_DECORATION* pDecoration = pPart->rvp_decoration;
	RECT r;
	int nWhitespace = pMargins->m_top / 2;
	r.top = y - pMargins->m_top + nWhitespace;
	if (pPart->rvp_layouted) {
		r.bottom = r.top + (pPart->rvp_bounds.bottom - pPart->rvp_bounds.top) + pMargins->m_bottom + pMargins->m_top - nWhitespace;
	}
	else {
		TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);
		r.bottom = r.top + (pPart->rvp_number * tm.tmHeight) + +pMargins->m_bottom + pMargins->m_top - nWhitespace;
	}
	r.left = x - 10;
	r.right = nRight > 1200 ? nRight - 100 : nRight;
	int nWidth = pDecoration->rbd_strokeWidth;
	if (!nWidth) {
		nWidth = 1;
	}
	COLORREF cColor = pDecoration->rbd_useThemeStrokeColor ? theme_getCurrent()->th_dialogBorder : pDecoration->rbd_strokeColor;
	COLORREF cFillColor = pDecoration->rbd_useThemeFillColor ? theme_getCurrent()->th_dialogLightBackground : pDecoration->rbd_fillColor;
	HPEN hPen = cColor == NO_COLOR ? GetStockObject(NULL_PEN) : CreatePen(PS_SOLID, nWidth, cColor);
	HBRUSH hBrush = cFillColor == NO_COLOR ? GetStockBrush(NULL_BRUSH) : CreateSolidBrush(cFillColor);
	HBRUSH hBrushOld = SelectObject(hdc, hBrush);
	HPEN hPenOld = SelectObject(hdc, hPen);
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);
	if (cFillColor != NO_COLOR) {
		DeleteObject(SelectObject(hdc, hBrushOld));
	}
	if (cColor != NO_COLOR) {
		DeleteObject(SelectObject(hdc, hPenOld));
	}
}
/*
 * Render a "text type block part".
 */
static void mdr_renderMarkdownBlockPart(RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed, float zoomFactor, BOOL bMeasureOnly) {
	MARGINS* pMargins = &pPart->rvp_margins;
	int x = pBounds->left + pMargins->m_left;
	int y = pBounds->top + pMargins->m_top;
	if (!bMeasureOnly) {
		if (pPart->rvp_type == MET_UNORDERED_LIST) {
			TextOutW(hdc, x - 15, y, pPart->rvp_level == 1 ? L"\u25CF" : (pPart->rvp_level == 2 ? L"\u25CB" : L"\u25A0"), 1);
		}
		else if (pPart->rvp_type == MET_TASK_LIST) {
			mdr_paintCheckmark(hdc, x - 20, y, pPart->rvp_number == 1);
		}
		else if (pPart->rvp_type == MET_ORDERED_LIST) {
			char szBuf[20];
			sprintf(szBuf, "%ld.", pPart->rvp_number);
			TextOut(hdc, x - 20, y, szBuf, (int)strlen(szBuf));
		}
		else if (pPart->rvp_decoration) {
			mdr_paintFillDecoration(hdc, pPart, pBounds);
		}
	}
	int nDCId = SaveDC(hdc);
	TEXT_FLOW* pFlow = &pPart->rvp_data.rvp_flow;
	RENDER_FLOW_PARAMS rfp = {
		.rfp_hdc = hdc,
		.rfp_zoomFactor = zoomFactor,
		.rfp_measureOnly = bMeasureOnly,
		.rfp_skipSpace = pPart->rvp_type != MET_FENCED_CODE_BLOCK
	};
	if (pFlow->tf_text && pFlow->tf_runs) {
		mdr_renderTextFlow(pMargins, pFlow, pBounds, &pPart->rvp_bounds, pUsed,
				pPart->rvp_type == MET_BLOCK_QUOTE ? pPart->rvp_level : 0, RTC_ALIGN_LEFT, &rfp);
	} else {
		pUsed->bottom = y + 10 + pMargins->m_bottom;
	}
	if (!bMeasureOnly && pPart->rvp_type == MET_HEADER && pPart->rvp_level < 3) {
		mdr_paintRule(hdc, pBounds->left + DEFAULT_LEFT_MARGIN, pBounds->right - DEFAULT_LEFT_MARGIN, pUsed->bottom - 2, 1);
	}
	RestoreDC(hdc, nDCId);
	pPart->rvp_height = pUsed->bottom - pBounds->top;
	pPart->rvp_layouted = TRUE;
}

static BOOL mdr_isTopLevelOrBreak(INPUT_STREAM* pStream, MDR_ELEMENT_TYPE mCurrentType, int nLevel) {
	char c;
	if ((c = pStream->is_peekc(pStream, 0)) == 0) {
		return FALSE;
	}
	if (c == '\n') {
		return TRUE;
	}
	BOOL bRet = TRUE;
	STREAM_OFFSET offset = pStream->is_tell(pStream);
	while (1) {
		if (IS_UNORDERED_LIST_START(c) || c == '#') {
			break;
		}
		if ((c == '`' || c == '~') && pStream->is_peekc(pStream, 1) == c && pStream->is_peekc(pStream, 2) == c) {
			break;
		}
		if (isdigit((unsigned char)c)) {
			while ((c = pStream->is_getc(pStream)) != 0) {
				if (c == '.') {
					bRet = TRUE;
					break;
				}
				if (!isdigit((unsigned char)c)) {
					bRet = FALSE;
					break;
				}
			}
			break;
		}
		if (c == '>') {
			if (mCurrentType != MET_BLOCK_QUOTE) {
				break;
			}
#if 0
			LINE* lpPrev = lp->prev;
			if (lpPrev && lpPrev->lbuf[0] == '>' && (lpPrev->len == 1 || (lpPrev->len == 2 && lpPrev->lbuf[1] == ' '))) {
				bRet = TRUE;
				break;
			}
#endif
			int nLevelNew = 1;
			while (pStream->is_peekc(pStream, 1) == '>') {
				nLevelNew++;
				pStream->is_skip(pStream, 1);
			}
			bRet = nLevel != nLevelNew;
			break;
		}
		if (!string_isSpace(c)) {
			bRet = mCurrentType == MET_HEADER;
			break;
		}
		c = pStream->is_getc(pStream);
	}
	pStream->is_seek(pStream, offset);
	return bRet;
}

static int mdr_getLevelFromIndent(INPUT_STREAM* pStream, char aListChar) {
	int cCol = 0;
	int nRet = 1;
	STREAM_OFFSET offset = pStream->is_positionToLineStart(pStream, 0);

	for (int i = 0; ; i++) {
		char c = pStream->is_peekc(pStream, i);
		if (c == '\n') {
			break;
		}
		if (c == ' ') {
			cCol++;
		}
		else if (c == '\t') {
			cCol += 2;
		} else {
			nRet = 1 + (cCol / 2);
			break;
		}
	}
	pStream->is_seek(pStream, offset);
	return nRet;
}

/*
 * Skip all space in the input. Return TRUE, if we found at least one space character.
 */
static BOOL mdr_skipSpace(INPUT_STREAM* pStream, int nOffset) {
	BOOL bFound = FALSE;
	char c;
	while ((c = pStream->is_peekc(pStream, nOffset)) == ' ' || c == '\t') {
		bFound = TRUE;
		pStream->is_skip(pStream, 1);
	}
	return bFound;
}

/*
 * Returns the default element style format for a given render element type.
 */
static MDR_ELEMENT_FORMAT* mdr_getFormatFor(MDR_ELEMENT_TYPE mType, int nLevel) {
	switch (mType) {
	case MET_HEADER:
		switch (nLevel) {
		case 1: return &_formatH1; 
		case 2: return &_formatH2; 
		case 3: return &_formatH3; 
		case 4: return &_formatH4; 
		case 5: return &_formatH5; 
		default: return &_formatH6; 
		}
	case MET_PARAGRAPH:
		return &_formatParagraph;
	case MET_FENCED_CODE_BLOCK:
		return &_formatFenced;
	case MET_TASK_LIST:
	case MET_ORDERED_LIST:
	case MET_UNORDERED_LIST: return nLevel== 1 ? &_formatListLevel1 : (nLevel == 2 ? &_formatListLevel2 : &_formatListLevel3); 
	case MET_BLOCK_QUOTE:
		switch (nLevel) {
		case 1: return &_formatBlockQuote; 
		case 2: return &_formatBlockQuote2; 
		default: return &_formatBlockQuote3; 
		}
		break;
	}
	return &_formatText;
}

/*
 * Parse a top level markup element (list, header, code block etc...) 
 */
static MDR_ELEMENT_TYPE mdr_determineTopLevelElement(INPUT_STREAM* pStream, int* pLevel, long* pNumber) {
	char c = pStream->is_peekc(pStream, 0);
	MDR_ELEMENT_TYPE mType = MET_NORMAL;

	if (c == '#') {
		mType = MET_HEADER;
		*pLevel = 1;
		pStream->is_skip(pStream, 1);
		while (pStream->is_peekc(pStream, 0) == '#') {
			pStream->is_skip(pStream, 1);
			(* pLevel)++;
		}
	} else if (IS_UNORDERED_LIST_START(c)) {
		mType = MET_UNORDERED_LIST;
		if (c == '-' && pStream->is_peekc(pStream, 1) == c && pStream->is_peekc(pStream, 2) == c) {
			pStream->is_positionToLineStart(pStream, 1);
			return MET_HORIZONTAL_RULE;
		}
		*pLevel = mdr_getLevelFromIndent(pStream, c);
		BOOL bSpaceFound = mdr_skipSpace(pStream, 1);
		if (bSpaceFound) {
			if (pStream->is_peekc(pStream, 1) == '[' && pStream->is_peekc(pStream, 3) == ']') {
				mType = MET_TASK_LIST;
				char cX = pStream->is_peekc(pStream, 2);
				*pNumber = cX == 'x' || cX == 'X' ? 1 : 0;
				pStream->is_skip(pStream, 3);
			}
		} else {
			mType = MET_NORMAL;
		}
	} else if (c == '>') {
		*pLevel = 1;
		mType = MET_BLOCK_QUOTE;
		pStream->is_skip(pStream, 1);
		while (pStream->is_peekc(pStream, 0) == '>') {
			(*pLevel)++;
			pStream->is_skip(pStream, 1);
		}
	} else if (isdigit((unsigned char)c)) {
		*pNumber = (c - '0');
		while ((c = pStream->is_peekc(pStream, 1)) != 0) {
			if (isdigit((unsigned char)c)) {
				*pNumber = 10 * (*pNumber) + (c - '0');
			} else if (c == '.') {
				pStream->is_skip(pStream, 2);
				mType = MET_ORDERED_LIST;
				break;
			}
			pStream->is_skip(pStream, 1);
		}
		*pLevel = mdr_getLevelFromIndent(pStream, c);
	} else if ((c == '`' || c == '~') && pStream->is_peekc(pStream, 1) == c && pStream->is_peekc(pStream, 2) == c) {
		pStream->is_positionToLineStart(pStream, 1);
		mType = MET_FENCED_CODE_BLOCK;
	}
	return mType;
}

static TEXT_RUN* mdr_appendRun(TEXT_RUN** pRuns, MDR_ELEMENT_FORMAT* pFormat, size_t nSize, FONT_STYLE_DELTA *pFSD, LINE* lp, int nLineOffset, char** ppszLink) {
	int mAttrs = pFSD->fsd_logicalStyles;
	if (nSize == 0 && *pRuns && (mAttrs & ATTR_LINE_BREAK) == 0) {
		return NULL;
	}
	if (!pFormat) {
		pFormat = &_formatText;
	}
	TEXT_RUN* pRun = ll_append(pRuns, sizeof(TEXT_RUN));
	pRun->tr_size = nSize;
	pRun->tr_lp = lp;
	pRun->tr_lineOffset = nLineOffset;
	pRun->tr_attributes.size = pFormat->mef_charHeight;
	pRun->tr_attributes.weight = pFormat->mef_charWeight;
	pRun->tr_attributes.fixedFont = pFormat->mef_fixedFont;
	if (mAttrs & ATTR_HIGHLIGHT) {
		pRun->tr_attributes.fixedFont = 0;
		pRun->tr_attributes.bgColor = theme_textStyleBackground("highlight", RGB(100, 100, 25)); 
	} else if (mAttrs & (ATTR_CODE|ATTR_TAG_CODE)) {
		pRun->tr_attributes.fixedFont = 1;
		pRun->tr_attributes.bgColor = theme_getCurrent()->th_dialogLightBackground;
	} else {
		pRun->tr_attributes.bgColor = NO_COLOR;
	}
	if (mAttrs & ATTR_SUB) {
		pRun->tr_attributes.subscript = 1;
	} else if (mAttrs & ATTR_SUPER) {
		pRun->tr_attributes.superscript = 1;
	}
	if (mAttrs & (ATTR_LINK|ATTR_UNDERLINE)) {
		pRun->tr_attributes.underline = 1;
	}
	if (pFSD->fsd_styleName) {
		pRun->tr_attributes.fgColor = theme_textStyleForeground(pFSD->fsd_styleName, NO_COLOR);
	} else {
		if (mAttrs & ATTR_LINK) {
			pRun->tr_attributes.fgColor = theme_textStyleForeground("hyperlink", RGB(120, 120, 255));
		}
		else {
			pRun->tr_attributes.fgColor = pFSD->fsd_textColor;
			pFSD->fsd_textColor = NO_COLOR;
		}
	}
	pRun->tr_attributes.indent = pFSD->fsd_indent;
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
	pFSD->fsd_logicalStyles &= ~(ATTR_LINE_BREAK | ATTR_LINK);
	if (ppszLink && *ppszLink) {
		pRun->tr_link = *ppszLink;
		*ppszLink = 0;
	}
	return pRun;
}

/*
 * Process a link by optionally prepending a Base URL (if it is a relative link). If 'bAuto' is true,
 * automatically extend the protocol portion of the link.
 * 
 * Returns an allocated result to be freed later.
 */
static char* mdr_processUrlWithBase(const char* pszBaseURL, char* pszLink, BOOL bAuto) {
	char szFullURL[EDMAXPATHLEN];
	if (bAuto && strchr(pszLink, '/') == 0) {
		sprintf(szFullURL, "https://%s", pszLink);
		return _strdup(szFullURL);
	}
	if (pszBaseURL && pszLink[0] != '#' && strstr(pszLink, "//") == 0) {
		string_splitFilename(pszBaseURL, szFullURL, NULL);
		string_concatPathAndFilename(szFullURL, szFullURL, pszLink);
		return _strdup(szFullURL);
	}
	return _strdup(pszLink);
}

/*
 * Parse the link URL - allow for things as included title specs. 
 */
static BOOL mdr_parseLinkUrl(const char* pszBaseURL, char* pszBuf, char** pszLink, char** pszTitle, int* nWidth, int* nHeight) {
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
	*pszLink = mdr_processUrlWithBase(pszBaseURL, pszBuf, bAuto);
	if (szTitle[0]) {
		*pszTitle = _strdup(szTitle);
	}
	else {
		*pszTitle = NULL;
	}
	return TRUE;
}

typedef struct tagHTML_PARSER_STATE {
	STRING_BUF* hps_text;								// the current text contents of a block level node.
	int					hps_lastTextOffset;				// end in hps_text of the last text run
	int					hps_blockLevel;					// simple book keeping of open block levels 
	const char* hps_baseUrl;							// The base URL from where the document is loaded.
	MDR_ELEMENT_TYPE	hps_listType;
	int					hps_orderedListListLevel;
	int					hps_orderedListIndices[10];		// Auto numbering of ordered lists
	int					hps_unorderedListLevel;
	LINE* hps_lp;
	int					hps_runOffset;
	HASHMAP* hps_lastAttributes;
	MDR_ELEMENT_FORMAT* hps_blockFormat;
	FONT_STYLE_DELTA	hps_styleTable[20];
	FONT_STYLE_DELTA* hps_currentStyle;
	RENDER_VIEW_PART** hps_head;
	RENDER_VIEW_PART* hps_part;
	RENDER_TABLE* hps_table;
	RENDER_TABLE_ROW* hps_tableRow;
	RENDER_TABLE_CELL* hps_tableCell;
} HTML_PARSER_STATE;

static void mdr_ensureParagraph(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState);

static BOOL mdr_parseLink(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, char* szLinkText,
	char** pszLink, char** pszTitle, int* nWidth, int* nHeight) {
	char szBuf[256];
	int nLinkStart = -1;
	char c;
	char* szLinkEnd = szLinkText + 255;
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
		if (nLinkStart < 0) {
			if (c == ']' && pStream->is_peekc(pStream, 0) == '(') {
				*szLinkText = 0;
				nLinkStart = 0;
				// support special syntax: [text](<link>)
				if (pStream->is_peekc(pStream, 1) == '<') {
					pStream->is_skip(pStream, 2);
				}
				else {
					pStream->is_skip(pStream, 1);
				}
			}
			else if (szLinkText < szLinkEnd) {
				*szLinkText++ = c;
			}
		}
		else {
			if (c == ')') {
				szBuf[nLinkStart] = 0;
				return mdr_parseLinkUrl(pState->hps_baseUrl, szBuf, pszLink, pszTitle, nWidth, nHeight);
			}
			if (nLinkStart < sizeof szBuf - 1) {
				szBuf[nLinkStart++] = c;
			}
		}
	}
	return FALSE;
}

static TEXT_RUN** mdr_getBlockRunsOf(RENDER_VIEW_PART* pPart) {
	return &pPart->rvp_data.rvp_flow.tf_runs;
}

/*
 * Parse a fenced code block and add the runs to the view part.
 */
static void mdr_parsePreformattedCodeBlock(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, BOOL bIndented, const char* pEndTag) {
	size_t nLastOffset = 0;
	int nOffs;
	char c;
	LINE* lp;
	char szTag[32];

	if (pEndTag) {
		sprintf(szTag, "</%s>", pEndTag);
	}
	RENDER_VIEW_PART* pPart = pState->hps_part;
	if (!pPart) {
		mdr_ensureParagraph(pStream, pState);
		pPart = pState->hps_part;
	}
	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		nOffs = 0;
		if (bIndented) {
			if (c == '\t') {
				nOffs = 1;
			} else {
				if (c != ' ' || pStream->is_strncmp(pStream, "    ", 4) != 0) {
					return;
				}
				nOffs = 4;
			}
		}
		else if (pEndTag == 0 && (pStream->is_strncmp(pStream, "```", 3) == 0 || pStream->is_strncmp(pStream, "~~~", 3) == 0)) {
			pStream->is_positionToLineStart(pStream, 1);
			return;
		} else if (pEndTag && c == '<' && pStream->is_strncmp(pStream, szTag, strlen(szTag)) == 0) {
			pStream->is_positionToLineStart(pStream, 1);
			return;
		}
		pPart->rvp_number++;
		int nRunStart = pStream->is_inputMark(pStream, &lp);
		pStream->is_skip(pStream, nOffs);
		while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
			if (c == '\t') {
				size_t nCurrent = stringbuf_size(pState->hps_text) - nLastOffset;
				size_t nTab = (nCurrent + 4) / 4 * 4;
				while (nCurrent < nTab) {
					stringbuf_appendChar(pState->hps_text, ' ');
					nCurrent++;
				}
			} else {
				stringbuf_appendChar(pState->hps_text, c);
			}

		}
		size_t nSize = stringbuf_size(pState->hps_text) - nLastOffset;
		if (nSize == 0) {
			// do not skip empty lines.
			stringbuf_appendChar(pState->hps_text, ' ');
			nSize++;
		}
		mdr_appendRun(mdr_getBlockRunsOf(pPart), &_formatFenced, nSize,
			&(FONT_STYLE_DELTA){ATTR_LINE_BREAK, .fsd_textColor = NO_COLOR, 0, .fsd_fillColor = NO_COLOR, .fsd_strokeColor = NO_COLOR}, lp, nRunStart, 0);
		nLastOffset += nSize;
	}
}

/*
 * Parses the auto-link syntax.
 */
static BOOL mdr_parseAutolinks(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, TEXT_FLOW* pFlow, MDR_ELEMENT_FORMAT* pFormat, FONT_STYLE_DELTA* pFSD,
		size_t nLastOffset) {
	char szTemp[512];
	STREAM_OFFSET offset = pStream->is_tell(pStream);
	BOOL bDot = FALSE;
	char c;
	LINE* lp;
	int nUrlLen = 0;
	pStream->is_skip(pStream, 1);
	int nLineOffset = pStream->is_inputMark(pStream, &lp);
	while ((c = pStream->is_getc(pStream)) != '>' && c != 0 && c != '\n') {
		if (c == '.') {
			bDot = TRUE;
		}
		if (nUrlLen < sizeof szTemp - 1) {
			szTemp[nUrlLen++] = c;
		}
	}
	szTemp[nUrlLen] = 0;
	if (bDot) {
		size_t nSize = stringbuf_size(pState->hps_text) - nLastOffset;
		mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, pFSD, lp, nLineOffset, 0);
		char* pszLink;
		char* pszTitle;
		stringbuf_appendString(pState->hps_text, szTemp);
		TEXT_RUN* pRun = mdr_appendRun(&pFlow->tf_runs, pFormat, nUrlLen, &(FONT_STYLE_DELTA){ATTR_LINK, 
			.fsd_textColor = NO_COLOR, 0, .fsd_fillColor = NO_COLOR, .fsd_strokeColor = NO_COLOR}, lp, nLineOffset, 0);
		int nWidth = 0;
		int nHeight = 0;
		if (!mdr_parseLinkUrl(0, szTemp, &pszLink, &pszTitle, &nWidth, &nHeight)) {
			return FALSE;
		}
		pRun->tr_link = pszLink;
		pRun->tr_title = pszTitle;
		if (pRun->tr_image) {
			pRun->tr_image->mdi_height = nHeight;
			pRun->tr_image->mdi_width = nWidth;
		}
		return TRUE;
	}
	pStream->is_seek(pStream, offset);
	return FALSE;
}

static void mdr_applyFormat(RENDER_VIEW_PART* pPart, MDR_ELEMENT_FORMAT* pFormat) {
	pPart->rvp_margins = pFormat->mef_margins;
	if (pFormat->mef_decoration) {
		if (!pPart->rvp_decoration) {
			pPart->rvp_decoration = calloc(1, sizeof * (pPart->rvp_decoration));
		}
		memcpy(pPart->rvp_decoration, pFormat->mef_decoration, sizeof * (pPart->rvp_decoration));
	}
}

static BOOL mdr_isAtWordBorder(INPUT_STREAM* pStream) {
	char c = pStream->is_peekc(pStream, -1);
	if (c == '\n') {
		return TRUE;
	}
	if (!pks_isalnum(c) && c != '\\') {
		return TRUE;
	}
	c = pStream->is_peekc(pStream, 1);
	if (pks_isalnum(c)) {
		return FALSE;
	}
	return TRUE;
}

static BOOL mdr_isIndentedFencedBlock(INPUT_STREAM* pStream, int mType) {
	char cColumn = 0;
	if (mType == MET_ORDERED_LIST || mType == MET_UNORDERED_LIST) {
		return FALSE;
	}
	char c;
	STREAM_OFFSET offset = pStream->is_tell(pStream);
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
		if (c == '\t') {
			cColumn = 4;
			continue;
		}
		if (c != ' ') {
			pStream->is_seek(pStream, offset);
			if (IS_UNORDERED_LIST_START(c)) {
				return FALSE;
			}
			return cColumn >= 4;
		}
		cColumn++;
	}
	pStream->is_seek(pStream, offset);
	return FALSE;
}

typedef enum {HTT_INLINE, HTT_BLOCK, HTT_SPECIAL} HTML_TAG_TYPE;

typedef struct tagHTML_TAG_MAPPING HTML_TAG_MAPPING;

static struct tagHTML_TAG_MAPPING {
	const char* tm_tagName;
	HTML_TAG_TYPE tm_tagType;
	MDR_ELEMENT_TYPE tm_blockElement;
	int tm_elementLevel;				// For H1, H2,... and corresponding tag types - the level - otherwise 0.
	int tm_textAttr;
	int tm_textAttrClose;
	int tm_indent;
	int tm_noCloseTag;
	int tm_returnValues;
} _tagMappings[] = {
	{"em", HTT_INLINE, MET_NONE, 0, ATTR_EMPHASIS},
	{"sup", HTT_INLINE, MET_NONE, 0, ATTR_SUPER},
	{"sub", HTT_INLINE, MET_NONE, 0, ATTR_SUB},
	{"a", HTT_INLINE, MET_NONE, 0, ATTR_LINK, .tm_returnValues = 1},
	{"img", HTT_INLINE, MET_IMAGE, .tm_returnValues = 1, .tm_noCloseTag = 1},
	{"b", HTT_INLINE, MET_NONE, 0, ATTR_STRONG},
	{"dt", HTT_INLINE, MET_NONE, 0, 0, ATTR_LINE_BREAK, DEFAULT_LEFT_MARGIN*2},
	{"dd", HTT_INLINE, MET_NONE, 0, ATTR_STRONG, ATTR_LINE_BREAK},
	{"dl", HTT_INLINE, MET_NONE, 0, ATTR_LINE_BREAK},
	{"i", HTT_INLINE, MET_NONE, 0, ATTR_EMPHASIS},
	{"u", HTT_INLINE, MET_NONE, 0, ATTR_UNDERLINE},
	{"del", HTT_INLINE, MET_NONE, 0, ATTR_STRIKE},
	{"strike", HTT_INLINE, MET_NONE, 0, ATTR_STRIKE},
	{"s", HTT_INLINE, MET_NONE, 0, ATTR_STRIKE},
	{"code", HTT_INLINE, MET_NONE, 0, ATTR_TAG_CODE},
	{"span", HTT_INLINE, MET_NONE, 0, 0},
	{"br", HTT_INLINE, MET_NONE, 0, ATTR_LINE_BREAK, .tm_noCloseTag = 1},
	{"p", HTT_BLOCK, MET_PARAGRAPH, 0, ATTR_LINE_BREAK},
	{"script", HTT_BLOCK, MET_FENCED_CODE_BLOCK, 0},
	{"pre", HTT_BLOCK, MET_FENCED_CODE_BLOCK, 0},
	{"table", HTT_BLOCK, MET_TABLE, 0, .tm_returnValues = 1},
	{"hr", HTT_BLOCK, MET_HORIZONTAL_RULE, 0},
	{"tr", HTT_SPECIAL, MET_NONE, 0},
	{"td", HTT_SPECIAL, MET_NONE, 0},
	{"th", HTT_SPECIAL, MET_NONE, 0},
	{"h1", HTT_BLOCK, MET_HEADER, 1},
	{"h2", HTT_BLOCK, MET_HEADER, 2},
	{"h3", HTT_BLOCK, MET_HEADER, 3},
	{"h4", HTT_BLOCK, MET_HEADER, 4},
	{"h5", HTT_BLOCK, MET_HEADER, 5},
	{"h6", HTT_BLOCK, MET_HEADER, 6},
	{"ul", HTT_SPECIAL, MET_NONE, 1},
	{"li", HTT_BLOCK, MET_UNORDERED_LIST, 1},
	{"ol", HTT_SPECIAL, MET_NONE, 1},
	{"tt", HTT_INLINE, MET_NONE, 0, ATTR_TAG_CODE}
};

typedef struct tagHTML_TAG {
	HTML_TAG_MAPPING* ht_descriptor;
	BOOL ht_isClose;
	BOOL ht_isOpen;
	HASHMAP* ht_attributes;
} HTML_TAG;

/*
 * Parse a CSS style border specification.
 */
static void mdr_parseBorder(FONT_STYLE_DELTA* pFSD, char* pszBorder) {
	char* pszNext = strtok(pszBorder, " ");
	while (pszNext) {
		if (isdigit((unsigned char)pszNext[0])) {
			pFSD->fsd_strokeWidth = pszNext[0] - '0';
		} else if (strcmp("solid", pszNext) != 0) {
			pFSD->fsd_strokeColor = json_convertColor(pszNext);
		}
		pszNext = strtok(0, " ");
	}
}

/*
 * Parse a CSS style specification. Currently only color: #xxx / color: red or similar specifications
 * are supported.
 */
static void mdr_parseStyle(FONT_STYLE_DELTA* pFSD, const char* pszStyleSpec) {
	int tState = 0;
	char szAttribute[128];
	char szAttrValue[128];
	char* pszAttr;
	char* pszValue;
	char c;

	while (1) {
		c = *pszStyleSpec++;
		if (tState == 0) {
			if (isalpha((unsigned char)c)) {
				pszAttr = szAttribute;
				tState = 1;
			}
			else {
				if (!c) {
					break;
				}
				continue;
			}
		}
		if (tState == 1) {
			if (!c) {
				break;
			}
			if (c == ':') {
				*pszAttr = 0;
				tState = 2;
				pszValue = szAttrValue;
				continue;
			}
			*pszAttr++ = c;
			continue;
		}
		if (tState == 2) {
			if (c == ';' || c == 0) {
				*pszValue = 0;
				tState = 0;
				if (strcmp("color", szAttribute) == 0) {
					pFSD->fsd_textColor = json_convertColor(szAttrValue);
				} else if (strcmp("background", szAttribute) == 0) {
					pFSD->fsd_fillColor = json_convertColor(szAttrValue);
				} else if (strcmp("border", szAttribute) == 0) {
					mdr_parseBorder(pFSD, szAttrValue);
				} else if (strcmp("text-decoration", szAttribute) == 0) {
					if (strcmp("underline", szAttrValue)) {
						pFSD->fsd_logicalStyles |= ATTR_UNDERLINE;
					}
					else if (strcmp("line-through", szAttrValue)) {
						pFSD->fsd_logicalStyles |= ATTR_STRIKE;
					}
				}
				if (!c) {
					break;
				}
			} else if (pszValue != szAttrValue || c != ' ') {
				*pszValue++ = c;
			}
		}
	}
}

/*
 * Parse an HTML tag: <b> or </em> or the like.
 */
typedef enum {
	HPS_INIT = 0,
	HPS_ELEM = 1,
	HPS_BETWEEN_ATTR = 2,
	HPS_ATTR = 3,
	HPS_OPTIONAL_QUOTED_VALUE = 4,
	HPS_WAIT_FOR_VALUE = 5,
	HPS_VALUE = 6
} HTML_PARSE_STATE;
static int mdr_getTag(INPUT_STREAM* pStream, FONT_STYLE_DELTA* pFSD, HTML_TAG* pTag) {
	char szTag[10];
	int bTermQuote = 0;
	char lastC = 0;
	char szAttribute[32];
	static char szValue[128];
	char* pszAttr = 0;
	char* pszValue = 0;
	char* pszDest = szTag;
	size_t nDestSize = sizeof szTag;
	HTML_TAG_MAPPING* pResult = 0;

	HTML_PARSE_STATE nState = HPS_INIT;
	pTag->ht_isClose = 0;
	pTag->ht_isOpen = 0;
	pTag->ht_attributes = 0;
	pTag->ht_descriptor = 0;
	if (pStream->is_peekc(pStream, 0) == '/') {
		pTag->ht_isClose = 1;
		pStream->is_skip(pStream, 1);
	}
	else {
		pTag->ht_isOpen = 1;
	}
	pFSD->fsd_styleName = 0;
	while (nDestSize > 1) {
		char c = pStream->is_getc(pStream);
		switch (nState) {
		case HPS_INIT:
			if (isalnum((unsigned char)c)) {
				nState = HPS_ELEM;
				*pszDest++ = tolower(c);
			}
			break;
		case HPS_ELEM:
			if (isalnum((unsigned char)c)) {
				if (pszDest < szTag + nDestSize - 1) {
					*pszDest++ = tolower(c);
				}
			}
			else {
				*pszDest = 0;
				for (int nElement = 0; nElement < DIM(_tagMappings); nElement++) {
					if (strcmp(_tagMappings[nElement].tm_tagName, szTag) == 0) {
						pResult = &_tagMappings[nElement];
						break;
					}
				}
				nState = HPS_BETWEEN_ATTR;
			}
			break;
		case HPS_OPTIONAL_QUOTED_VALUE:
			if (c == '"') {
				nState = HPS_VALUE;
				bTermQuote = 1;
			}
			else if (c != ' ') {
				nState = HPS_VALUE;
				*pszValue++ = c;
				bTermQuote = 0;
			}
			break;
		case HPS_WAIT_FOR_VALUE:
		case HPS_BETWEEN_ATTR:
			if (isalnum((unsigned char)c)) {
				pszAttr = szAttribute;
				*pszAttr++ = tolower(c);
				nState = HPS_ATTR;
			}
			break;
		case HPS_ATTR:
			if (isalnum((unsigned char)c)) {
				if (pszAttr < (szAttribute + sizeof szAttribute - 1)) {
					*pszAttr++ = tolower(c);
				}
			} else if (c == '=') {
				*pszAttr = 0;
				bTermQuote = 0;
				nState = HPS_OPTIONAL_QUOTED_VALUE;
				pszValue = szValue;
			} else {
				*pszAttr = 0;
				nState = HPS_BETWEEN_ATTR;
			}
			break;
		case HPS_VALUE:
			if ((bTermQuote && c != '"') || (!bTermQuote && c != ' ' && c != '/' && c != '>')) {
				if (pszValue < (szValue + sizeof szValue - 1)) {
					*pszValue++ = c;
				}
			} else {
				*pszValue = 0;
				if (strcmp("class", szAttribute) == 0) {
					pFSD->fsd_styleName = szValue;
				} else if (strcmp("style", szAttribute) == 0) {
					mdr_parseStyle(pFSD, szValue);
				} else if (pResult && pResult->tm_returnValues) {
					if (!pTag->ht_attributes) {
						pTag->ht_attributes = hashmap_create(13, 0, 0);
					}
					hashmap_put(pTag->ht_attributes, _strdup(szAttribute), (intptr_t)_strdup(szValue));
				} 
				nState = HPS_BETWEEN_ATTR;
			}
		}
		if (c == '>') {
			if (pResult) {
				pTag->ht_descriptor = pResult;
				pFSD->fsd_indent = pResult->tm_indent;
				if (lastC == '/' && !pResult->tm_noCloseTag) {
					pTag->ht_isClose = 1;
				}
				return 1;
			}
			return 0;
		} else if (c != ' ') {
			lastC = c;
		}
	}
	return 0;
}

/*
 * Parse an HTML entity spec and convert into a character if successful. 
 */
static int mdr_parseEntity(STRING_BUF* pSB, INPUT_STREAM* pStream) {
	char szBuf[32];
	char* pszDest;
	if (pStream->is_peekc(pStream, 0) == '#' && pStream->is_peekc(pStream, 1) == 'x') {
		pStream->is_skip(pStream, 2);
		pszDest = szBuf;
		char c;
		while ((c = pStream->is_getc(pStream)) != 0 && c != ';' && pszDest < szBuf + sizeof szBuf - 1) {
			*pszDest++ = c;
		}
		*pszDest = 0;
		stringbuf_appendChar(pSB, (char)string_convertToLongBase16(szBuf));
		return 1;
	}
	for (int nE = 0; nE < DIM(_entities); nE++) {
		size_t nLen = strlen(_entities[nE].entity);
		if (pStream->is_strncmp(pStream, _entities[nE].entity, nLen) == 0) {
			stringbuf_appendChar(pSB, _entities[nE].c);
			pStream->is_skip(pStream, (int)nLen);
			return 1;
		}
	}
	return 0;
}

/*
 * Create a new RENDER_TABLE with default values.
 */
static RENDER_TABLE* mdr_newTable(int nColumnCount) {
	RENDER_TABLE* pResult = calloc(1, sizeof * pResult);
	pResult->rt_borderWidth = 1;
	pResult->rt_columnCount = nColumnCount;
	return pResult;
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

static void mdr_destroyTable(RENDER_VIEW_PART* pRVP) {
	ll_destroy(&pRVP->rvp_data.rvp_table->rt_rows, mdr_destroyTableRow);
	free(pRVP->rvp_data.rvp_table);
	pRVP->rvp_data.rvp_table = 0;
}

static void mdr_resetFontStyleDelta(FONT_STYLE_DELTA* pFSD) {
	pFSD->fsd_logicalStyles = 0;
	pFSD->fsd_styleName = 0;
	pFSD->fsd_indent = 0;
	pFSD->fsd_textColor = NO_COLOR;
	pFSD->fsd_fillColor = NO_COLOR;
	pFSD->fsd_strokeColor = NO_COLOR;
}

static RENDER_VIEW_PART* mdr_newPart(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, MDR_ELEMENT_TYPE mType, int nLevel) {
	RENDER_VIEW_PART* pExisting = pState->hps_part;
	RENDER_VIEW_PART* pPart = (pExisting && pExisting->rvp_data.rvp_flow.tf_text == 0) ? pExisting : ll_append(pState->hps_head, sizeof * pPart);
	if (pPart->rvp_data.rvp_table) {
		mdr_destroyTable(pPart);
	}
	if (pPart->rvp_decoration) {
		free(pPart->rvp_decoration);
		pPart->rvp_decoration = 0;
	}
	switch (mType) {
	case MET_HORIZONTAL_RULE: pPart->rvp_paint = mdr_renderHorizontalRule; break;
	case MET_TABLE: pPart->rvp_paint = mdr_renderTable; break;
	default: pPart->rvp_paint = mdr_renderMarkdownBlockPart; break;
	}
	pPart->rvp_type = mType;
	pState->hps_blockFormat = mdr_getFormatFor(mType, nLevel);
	pPart->rvp_level = nLevel;
	mdr_applyFormat(pPart, pState->hps_blockFormat);
	FONT_STYLE_DELTA* pFSD = pState->hps_currentStyle;
	if (pFSD->fsd_strokeColor != NO_COLOR || pFSD->fsd_fillColor != NO_COLOR) {
		RENDER_BOX_DECORATION* pDecoration = calloc(1, sizeof * pDecoration);
		pDecoration->rbd_fillColor = pFSD->fsd_fillColor;
		pDecoration->rbd_strokeColor = pFSD->fsd_strokeColor;
		pDecoration->rbd_strokeWidth = pFSD->fsd_strokeWidth;
		pPart->rvp_decoration = pDecoration;
	}
	mdr_resetFontStyleDelta(pFSD);
	if (mType == MET_TABLE) {
		pPart->rvp_data.rvp_table = mdr_newTable(0);
	}
	pStream->is_inputMark(pStream, &pPart->rvp_lpStart);
	pState->hps_blockLevel = 1;
	return pPart;
}

/*
 * Skip all leading space chars in an input string.
 */
static void mdr_skipLeadingSpace(INPUT_STREAM* pStream) {
	while (isspace((unsigned char) pStream->is_peekc(pStream, 0))) {
		pStream->is_skip(pStream, 1);
	}
}

static BOOL mdr_skipHTMLCommentOrDoctype(INPUT_STREAM* pStream) {
	if (pStream->is_peekc(pStream, 0) == '!') {
		pStream->is_skip(pStream, 3);
		char c;
		while ((c = pStream->is_peekc(pStream, 0)) != 0) {
			if (c == '>') {
				pStream->is_skip(pStream, 1);
				break;
			}
			pStream->is_skip(pStream, 1);
		}
		return TRUE;
	}
	return FALSE;
}

/*
 * Ensures, there is a paragraph, when HTML contents is added.
 */
static void mdr_ensureParagraph(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState) {
	if (!pState->hps_part) {
		FONT_STYLE_DELTA fsd = *pState->hps_currentStyle;
		pState->hps_part = mdr_newPart(pStream, pState, MET_NORMAL, 0);
		*pState->hps_currentStyle = fsd;
	}
}

static void mdr_finishTableSetup(RENDER_TABLE* pTable) {
	if (!pTable) {
		return;
	}
	RENDER_TABLE_ROW* pRow = pTable->rt_rows;
	int nColumn = 0;
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
			} else {
				pTable->rt_columnWidths[nColumn] = 20;
			}
			if (pCell->rtc_flow.tf_runs && pCell->rtc_isHeader) {
				pCell->rtc_flow.tf_runs->tr_attributes.weight = FW_BOLD;
			}
			pCell = pCell->rtc_next;
			nColumn++;
		}
		pRow = pRow->rtr_next;
	}
	if (!pTable->rt_columnCount) {
		pTable->rt_columnCount = nColumn;
	}
	for (nColumn = 0; nColumn < MAX_TABLE_COLUMNS; nColumn++) {
		pTable->rt_totalColumnWidth += pTable->rt_columnWidths[nColumn];
	}
}

static BOOL mdr_supportsNestedTag(RENDER_VIEW_PART* pPart, MDR_ELEMENT_TYPE mNested) {
	if (!pPart) {
		return TRUE;
	}
	MDR_ELEMENT_TYPE mType = pPart->rvp_type;
	if (mNested == MET_PARAGRAPH || mNested == MET_FENCED_CODE_BLOCK) {
		return mType != MET_TABLE && mType != MET_FENCED_CODE_BLOCK && mType != MET_HORIZONTAL_RULE &&
			mType != MET_UNORDERED_LIST && mType != MET_ORDERED_LIST;
	}
	return TRUE;
}

/*
 * End a currently open part. For now regular text based parts not containing a text will be discarded.
 */
static int mdr_endPart(HTML_PARSER_STATE* pState) {
	if (!pState->hps_part) {
		return 0;
	}
	if (pState->hps_part->rvp_type == MET_TABLE) {
		mdr_finishTableSetup(pState->hps_table);
		pState->hps_table = 0;
		pState->hps_tableCell = 0;
		pState->hps_tableRow = 0;
	} else if (pState->hps_part->rvp_type != MET_HORIZONTAL_RULE && pState->hps_part->rvp_data.rvp_flow.tf_text == 0 && pState->hps_part->rvp_decoration == 0) {
		mdr_resetFontStyleDelta(pState->hps_currentStyle);
		pState->hps_blockLevel = 0;
		return 0;
	}
	pState->hps_blockLevel = 0;
	pState->hps_part = 0;
	return 1;
}

static int mdr_applyImageAttributes(HTML_PARSER_STATE* pState, HASHMAP* pValues) {
	RENDER_VIEW_PART* pPart = pState->hps_part;
	if (!pPart || pPart->rvp_type == MET_TABLE || !pValues) {
		return 0;
	}
	char* pszLink = (char*)hashmap_get(pValues, "src");
	char* pszTitle = (char*)hashmap_get(pValues, "alt");
	if (!pszTitle) {
		pszTitle = (char*)hashmap_get(pValues, "title");
		if (!pszTitle) {
			pszTitle = (char*)hashmap_get(pValues, "name");
		}
	}
	char* pszW = (char*)hashmap_get(pValues, "width");
	char* pszH = (char*)hashmap_get(pValues, "height");
	FONT_STYLE_DELTA* pfsd = pState->hps_currentStyle;
	pfsd->fsd_logicalStyles |= ATTR_LINK;
	char* pszText = " ";
	int nLen = (int)strlen(pszText);
	stringbuf_appendString(pState->hps_text, pszText);
	TEXT_RUN* pRun = mdr_appendRun(mdr_getBlockRunsOf(pPart), pState->hps_blockFormat, nLen, pfsd, pState->hps_lp, 0, 0);
	if (pszTitle) {
		pRun->tr_title = _strdup(pszTitle);
	}
	MD_IMAGE* pImage = calloc(1, sizeof(MD_IMAGE));
	pRun->tr_image = pImage;
	if (pszW) {
		pImage->mdi_width = (int)string_convertToLong(pszW);
	}
	if (pszH) {
		pImage->mdi_height = (int)string_convertToLong(pszH);
	}
	if (pfsd->fsd_strokeColor != NO_COLOR && pfsd->fsd_strokeWidth > 0) {
		pImage->mdi_borderColor = pfsd->fsd_strokeColor;
		pImage->mdi_borderWidth = pfsd->fsd_strokeWidth;
	}
	pRun->tr_link = mdr_processUrlWithBase(pState->hps_baseUrl, pszLink, FALSE);
	return nLen;
}

/*
 * Initialize the parser state
 */
static void mdr_initParserState(HTML_PARSER_STATE* pState, RENDER_VIEW_PART** pHead, const char* pszBaseUrl) {
	memset(pState, 0, sizeof *pState);
	pState->hps_listType = MET_UNORDERED_LIST;
	pState->hps_text = stringbuf_create(256);
	pState->hps_head = pHead;
	pState->hps_currentStyle = pState->hps_styleTable;
	pState->hps_baseUrl = pszBaseUrl;
	mdr_resetFontStyleDelta(pState->hps_currentStyle);
}

/*
 * Apply the attributes of the last open tag on a text run.
 */
static void mdr_applyRunAttributes(HTML_PARSER_STATE* pState, TEXT_RUN* pRun) {
	if (pRun && pState->hps_lastAttributes) {
		char* pszLink = (char*)hashmap_get(pState->hps_lastAttributes, "href");
		if (pszLink) {
			pRun->tr_link = mdr_processUrlWithBase(pState->hps_baseUrl, pszLink, FALSE);
		}
		char* pszTitle = (char*)hashmap_get(pState->hps_lastAttributes, "title");
		if (pszTitle) {
			pRun->tr_title = _strdup(pszTitle);
		}
		hashmap_destroyWithKeysAndValues(pState->hps_lastAttributes);
		pState->hps_lastAttributes = 0;
	}
}

static TEXT_FLOW* mdr_getTextFlowEnsureInit(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState) {
	if (!pState->hps_part) {
		mdr_ensureParagraph(pStream, pState);
	}
	if (pState->hps_table && !pState->hps_tableCell) {
		return 0;
	}
	return pState->hps_tableCell ? &pState->hps_tableCell->rtc_flow : &pState->hps_part->rvp_data.rvp_flow;

}

static TEXT_FLOW* mdr_endRun(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, BOOL bForced) {
	size_t nSize = stringbuf_size(pState->hps_text);
	if (!nSize && !bForced) {
		return 0;
	}
	TEXT_FLOW* pFlow = mdr_getTextFlowEnsureInit(pStream, pState);
	int nDelta = (int)(nSize - pState->hps_lastTextOffset);
	if (pFlow && (nDelta || bForced)) {
		TEXT_RUN* pRun = mdr_appendRun(&pFlow->tf_runs, pState->hps_blockFormat, nDelta, pState->hps_currentStyle, pState->hps_lp, pState->hps_runOffset, 0);
		mdr_applyRunAttributes(pState, pRun);
		pState->hps_lastTextOffset += nDelta;
		pState->hps_runOffset = pStream->is_inputMark(pStream, &pState->hps_lp);
	}
	return pFlow;
}

static void mdr_closeTextElement(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, BOOL bForced) {
	TEXT_FLOW* pFlow = mdr_endRun(pStream, pState, bForced);
	if (!pFlow) {
		return;
	}
	pFlow->tf_text = _strdup(stringbuf_getString(pState->hps_text));
	stringbuf_reset(pState->hps_text);
	pState->hps_lastTextOffset = 0;
	pState->hps_tableCell = 0;
}

static void mdr_finalizeParserState(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState) {
	mdr_closeTextElement(pStream, pState, FALSE);
	stringbuf_destroy(pState->hps_text);
	hashmap_destroyWithKeysAndValues(pState->hps_lastAttributes);
}

static void mdr_onTagSpecialHandling(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HTML_TAG* pTag) {
	const char* pszTN = pTag->ht_descriptor->tm_tagName;
	if (strcmp("ol", pszTN) == 0) {
		pState->hps_listType = MET_ORDERED_LIST;
		int nLevel = pState->hps_orderedListListLevel;
		pState->hps_orderedListListLevel = pTag->ht_isClose ? nLevel - 1 : nLevel + 1;
		if (!pTag->ht_isClose) {
			pState->hps_orderedListIndices[nLevel] = 1;
		}
		return;
	}
	if (strcmp("ul", pszTN) == 0) {
		pState->hps_listType = MET_UNORDERED_LIST;
		pState->hps_unorderedListLevel = pTag->ht_isClose ? pState->hps_unorderedListLevel - 1 : pState->hps_unorderedListLevel + 1;
		return;
	}
	if (strcmp("tr", pszTN) == 0) {
		if (pTag->ht_isClose) {
			pState->hps_tableRow = 0;
		}
		else if (pState->hps_table) {
			pState->hps_tableRow = (RENDER_TABLE_ROW*)ll_append((LINKED_LIST**)&pState->hps_table->rt_rows, sizeof * pState->hps_tableRow);
		}
		return;
	}
	if (strcmp("td", pszTN) == 0 || strcmp("th", pszTN) == 0) {
		if (pTag->ht_isClose) {
			if (pState->hps_tableCell) {
				mdr_closeTextElement(pStream, pState, TRUE);
			}
			pState->hps_tableCell = 0;
		}
		else if (pState->hps_tableRow) {
			pState->hps_tableCell = (RENDER_TABLE_CELL*)ll_append((LINKED_LIST**)&pState->hps_tableRow->rtr_cells, sizeof * pState->hps_tableCell);
			pState->hps_tableCell->rtc_isHeader = pTag->ht_descriptor->tm_tagName[1] == 'h';
		}
		mdr_skipLeadingSpace(pStream);
		return;
	}
}

static int mdr_processOpeningAnchor(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HASHMAP* pAttributes) {
	if (!pAttributes) {
		return 0;
	}
	char* pszAnchor = (char*)hashmap_get(pAttributes, "name");
	if (pszAnchor) {
		TEXT_FLOW* pFlow = mdr_endRun(pStream, pState, TRUE);
		if (pFlow) {
			pFlow->tf_runs->tr_anchor = _strdup(pszAnchor);
		}
	}
	return hashmap_containsKey(pAttributes, "href");
}

static void mdr_onInlineTag(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HTML_TAG* pTag) {
	if (pTag->ht_isClose) {
		if (pTag->ht_descriptor->tm_noCloseTag) {
			return;
		}
		pState->hps_currentStyle->fsd_logicalStyles |= pTag->ht_descriptor->tm_textAttrClose;
		mdr_endRun(pStream, pState, FALSE);
		if (pState->hps_currentStyle > pState->hps_styleTable) {
			pState->hps_currentStyle--;
		}
	} else {
		const char* pszTag = pTag->ht_descriptor->tm_tagName;
		if (strcmp("img", pszTag) == 0) {
			pState->hps_lastTextOffset += mdr_applyImageAttributes(pState, pTag->ht_attributes);
			return;
		}
		if (strcmp("a", pszTag) == 0 && !mdr_processOpeningAnchor(pStream, pState, pTag->ht_attributes)) {
			return;
		}
		BOOL bBreak = pTag->ht_descriptor->tm_textAttr & ATTR_LINE_BREAK;
		if (bBreak) {
			pState->hps_currentStyle->fsd_logicalStyles |= ATTR_LINE_BREAK;
			mdr_skipLeadingSpace(pStream);
		}
		mdr_endRun(pStream, pState, bBreak);
		FONT_STYLE_DELTA fsdNext = *pState->hps_currentStyle;
		fsdNext.fsd_logicalStyles |= pTag->ht_descriptor->tm_textAttr;
		BOOL bPop = pTag->ht_descriptor->tm_noCloseTag;
		if (pState->hps_currentStyle < &pState->hps_styleTable[DIM(pState->hps_styleTable) - 1]) {
			pState->hps_currentStyle++;
			*pState->hps_currentStyle = fsdNext;
		} else {
			bPop = FALSE;
		}
		if (bPop) {
			pState->hps_currentStyle--;
		}
	}
}

static void mdr_applyTableAttributes(RENDER_TABLE* pTable, HASHMAP* pAttributes) {
	if (!pAttributes) {
		return;
	}
	char* pszBorder = (char*)hashmap_get(pAttributes, "border");
	if (pszBorder) {
		pTable->rt_borderWidth = (int)string_convertToLong(pszBorder);
	}
	char* pszWidth = (char*)hashmap_get(pAttributes, "width");
	if (pszWidth) {
		pTable->rt_width = (float)string_convertToLong(pszWidth) / 100.0f;
	}
}

static void mdr_onBlockLevelTag(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HTML_TAG* pTag) {
	MDR_ELEMENT_TYPE mType = pTag->ht_descriptor->tm_blockElement;
	if (pTag->ht_isClose && pTag->ht_isOpen) {
		if (!pState->hps_blockLevel) {
			pState->hps_part = mdr_newPart(pStream, pState, mType, pTag->ht_descriptor->tm_elementLevel);
			mdr_endPart(pState);
		}
		return;
	}
	if (pTag->ht_isClose) {
		if (pState->hps_blockLevel > 0) {
			pState->hps_blockLevel--;
		}
		if (pState->hps_blockLevel > 0) {
			return;
		}
	}
	if (mType == MET_TABLE && !pState->hps_table && pTag->ht_isOpen) {
		// For tables force a new part start
		pState->hps_blockLevel = 0;
	}
	// close current block if either it was an "auto-created" block (blocklevel == 0) or it is explicitly closed and the block
	if (pTag->ht_isClose || pState->hps_blockLevel == 0) {
		mdr_closeTextElement(pStream, pState, FALSE);
		if (mType == MET_TABLE || !pState->hps_table) {
			mdr_endPart(pState);
		}
		if (pTag->ht_isClose) {
			return;
		}
	}
	if (pState->hps_table) {
		return;
	}
	pState->hps_blockLevel++;
	if (pState->hps_blockLevel <= 1) {
		int nLevel = pTag->ht_descriptor->tm_elementLevel;
		if (mType == MET_UNORDERED_LIST) {
			mType = pState->hps_listType;
		}
		if (mType == MET_UNORDERED_LIST) {
			nLevel = pState->hps_unorderedListLevel;
		} else if (mType == MET_ORDERED_LIST) {
			nLevel = pState->hps_orderedListListLevel;
		}
		pState->hps_part = mdr_newPart(pStream, pState, mType, nLevel);
		if (mType == MET_ORDERED_LIST) {
			pState->hps_part->rvp_number = pState->hps_orderedListIndices[nLevel-1]++;
		}
		mdr_skipLeadingSpace(pStream);
		if (mType == MET_TABLE) {
			pState->hps_table = pState->hps_part->rvp_data.rvp_table;
			mdr_applyTableAttributes(pState->hps_table, pTag->ht_attributes);
		} else if (mType == MET_FENCED_CODE_BLOCK) {
			mdr_parsePreformattedCodeBlock(pStream, pState, FALSE, pTag->ht_descriptor->tm_tagName);
			if (stringbuf_size(pState->hps_text)) {
				pState->hps_part->rvp_data.rvp_flow.tf_text = _strdup(stringbuf_getString(pState->hps_text));
				stringbuf_reset(pState->hps_text);
			}
			else {
				free(pState->hps_part->rvp_decoration);
				pState->hps_part->rvp_decoration = 0;
			}
			pState->hps_lastTextOffset = 0;
			mdr_endPart(pState);
		}
	}
}

static void mdr_onHtmlTag(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, HTML_TAG* pTag, FONT_STYLE_DELTA* pFSDDelta) {
	HTML_TAG_TYPE tType = pTag->ht_descriptor->tm_tagType;

	FONT_STYLE_DELTA* pOldFSD = pState->hps_currentStyle;
	FONT_STYLE_DELTA fsdOld = *pOldFSD;
	if (!pTag->ht_isClose) {
		*pState->hps_currentStyle = *pFSDDelta;
	}
	if (tType == HTT_SPECIAL) {
		mdr_onTagSpecialHandling(pStream, pState, pTag);
	} else if (tType == HTT_INLINE) {
		mdr_onInlineTag(pStream, pState, pTag);
	} else {
		mdr_onBlockLevelTag(pStream, pState, pTag);
	}
	if (pTag->ht_attributes) {
		hashmap_destroyWithKeysAndValues(pState->hps_lastAttributes);
		pState->hps_lastAttributes = pTag->ht_attributes;
	}
	if (!pTag->ht_isClose) {
		*pOldFSD = fsdOld;
	}
}

/*
 * Parse a text and convert it into a list of view parts to be rendered later.
 * Note, that the viewparts must be destroyed, when they are not needed any more
 * using mdr_destroyViewParts.
 */
RENDER_VIEW_PART* mdr_parseHTML(INPUT_STREAM* pStream, const char* pszBaseURL) {
	RENDER_VIEW_PART* pFirst = 0;
	char c;
	size_t nSize;
	HTML_PARSER_STATE state;

	mdr_initParserState(&state, &pFirst, pszBaseURL);
	while ((c = pStream->is_getc(pStream)) != 0) {
		if (c == '&') {
			if (mdr_parseEntity(state.hps_text, pStream)) {
				continue;
			}
		} else if (c == '<') {
			if (mdr_skipHTMLCommentOrDoctype(pStream)) {
				continue;
			}
			HTML_TAG tag;
			FONT_STYLE_DELTA fsdNext = *state.hps_currentStyle;
			BOOL bMatch = mdr_getTag(pStream, &fsdNext, &tag);
			if (bMatch) {
				mdr_onHtmlTag(pStream, &state, &tag, &fsdNext);
			}
			continue;
		} else if (c == '\n' && pStream->is_peekc(pStream, 0) == '\n' && state.hps_table == 0) {
			// treat empty line as real line break.
			nSize = stringbuf_size(state.hps_text) - state.hps_lastTextOffset;
			state.hps_currentStyle->fsd_logicalStyles |= ATTR_LINE_BREAK;
			if (nSize) {
				mdr_ensureParagraph(pStream, &state);
				TEXT_RUN* pRun = mdr_appendRun(mdr_getBlockRunsOf(state.hps_part), state.hps_blockFormat, nSize, state.hps_currentStyle,
					state.hps_lp, state.hps_runOffset, 0);
				mdr_applyRunAttributes(&state, pRun);
				state.hps_lastTextOffset += (int)nSize;
			}
			state.hps_runOffset = pStream->is_inputMark(pStream, &state.hps_lp);
			pStream->is_getc(pStream);
			continue;
		}
		if (c != '\n') {
			if (stringbuf_size(state.hps_text) == 0) {
				state.hps_runOffset = pStream->is_inputMark(pStream, &state.hps_lp)-1;
			}
			stringbuf_appendChar(state.hps_text, c);
			if (c == ' ') {
				mdr_skipLeadingSpace(pStream);
			}
		}
	}
	mdr_finalizeParserState(pStream, &state);
	return pFirst;
}

/*
 * Parses an HTML file and create render view parts for wysiwyg rendering.
 */
static void mdr_parseFileToHTML(WINFO* wp) {
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	RENDER_VIEW_PART* pPart = mdr_parseHTML(pStream, fp->fname);
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	pData->md_pElements = pPart;
	pStream->is_destroy(pStream);
}

static TEXT_RUN* mdr_appendRunState(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, FONT_STYLE_DELTA* pFSD) {
	TEXT_FLOW* pFlow = mdr_getTextFlowEnsureInit(pStream, pState);
	if (!pFlow) {
		return 0;
	}
	int nSize = (int)(stringbuf_size(pState->hps_text) - pState->hps_lastTextOffset);
	TEXT_RUN* pResult = mdr_appendRun(&pFlow->tf_runs, pState->hps_blockFormat, nSize, pFSD, pState->hps_lp, pState->hps_runOffset, 0);
	pState->hps_runOffset = pStream->is_inputMark(pStream, &pState->hps_lp);
	pState->hps_lastTextOffset += (int)nSize;
	return pResult;
}

/*
 * Parse a top-level element to be rendered with a view part possibly containing nested formatting. 
 */
static void mdr_parseFlow(INPUT_STREAM* pStream, HTML_PARSER_STATE*pState, int nInitialAttrs) {
	stringbuf_reset(pState->hps_text);
	pState->hps_lastTextOffset = 0;
	long nNumber;
	int nState = 0;
	int nLevel = 0;
	int nInPart = 0;
	FONT_STYLE_DELTA fsd;
	FONT_STYLE_DELTA fsdNext;

	fsd.fsd_logicalStyles = nInitialAttrs;
	mdr_resetFontStyleDelta(&fsd);
	MDR_ELEMENT_TYPE mType = MET_NORMAL;
	BOOL bEnforceBreak = TRUE;
	while (pStream->is_peekc(pStream, 0)) {
		BOOL bSkipped = FALSE;
		char lastC = 0;
		char cNext;
		char c = 0;
		int nLineOffset = 0;
		pState->hps_runOffset = pStream->is_inputMark(pStream, &pState->hps_lp);
		for (; (cNext = pStream->is_peekc(pStream, 0)) != 0 && c != '\n'; pStream->is_skip(pStream, 1)) {
			lastC = c;
			c = cNext;
			if (nLineOffset == 0 && mdr_isIndentedFencedBlock(pStream, mType)) {
				mType = MET_FENCED_CODE_BLOCK;
				pState->hps_blockFormat = &_formatFenced;
				mdr_parsePreformattedCodeBlock(pStream, pState, TRUE, 0);
				pState->hps_lastTextOffset = (int)stringbuf_size(pState->hps_text);
				goto outer;
			} else if (nState <= 1 && string_isSpace(c)) {
				continue;
			}
			nLineOffset = 1;
			if (nState == 1 && mType == MET_BLOCK_QUOTE && c == '>' && !bSkipped) {
				bSkipped = TRUE;
				continue;
			}
			if (nState == 0) {
				if (pState->hps_blockLevel == 0) {
					mType = mdr_determineTopLevelElement(pStream, &nLevel, &nNumber);
					pState->hps_part = mdr_newPart(pStream, pState, mType, nLevel);
					if (mType == MET_HORIZONTAL_RULE) {
						return;
					}
					if (mType == MET_ORDERED_LIST || mType == MET_TASK_LIST) {
						pState->hps_part->rvp_number = nNumber;
					}
					if (mType == MET_FENCED_CODE_BLOCK) {
						mdr_parsePreformattedCodeBlock(pStream, pState, FALSE, 0);
						pState->hps_lastTextOffset = (int)stringbuf_size(pState->hps_text);
						goto outer;
					}
					if (mType != MET_NORMAL) {
						pState->hps_runOffset++;
						nState = 1;
						continue;
					}
				}
			}
			if (nState < 2) {
				nState = 2;
			}
			bEnforceBreak = FALSE;
			int nToggle = 0;
			fsdNext = fsd;
			BOOL bEscaped = fsd.fsd_logicalStyles & ATTR_CODE;
			if (c == '\\' && strchr(_escapedChars, pStream->is_peekc(pStream, 1)) != NULL) {
				pStream->is_skip(pStream, 1);
				c = pStream->is_peekc(pStream, 0);
			} else if (!bEscaped && c == '=' && pStream->is_peekc(pStream, 1) == c) {
				pStream->is_skip(pStream, 1);
				mdr_appendRunState(pStream, pState, &fsd);
				fsd.fsd_logicalStyles ^= ATTR_HIGHLIGHT;
				continue;
			} else if (c == '`' || (!bEscaped && (c == '*' ||
				// allow for _ only at word borders.
					(c == '_' && mdr_isAtWordBorder(pStream)) ||
				c == '~'))) {
				if (c == '*' || c == '_') {
					if (pStream->is_peekc(pStream, 1) == c) {
						nToggle = ATTR_STRONG;
						pStream->is_skip(pStream, 1);
						c = c == '*' ? '_' : '*';
						if (pStream->is_peekc(pStream, 1) == c) {
							nToggle = ATTR_EMPHASIS;
							pStream->is_skip(pStream, 1);
						}
					} else {
						nToggle = ATTR_EMPHASIS;
					}
				} else if (c == '~' && pStream->is_peekc(pStream, 1) == c) {
					nToggle = ATTR_STRIKE;
					pStream->is_skip(pStream, 1);
				} else if (c == '`') {
					nToggle = ATTR_CODE;
				}
				if (nToggle) {
					mdr_appendRunState(pStream, pState, &fsd);
					fsd = fsdNext;
					int styles = fsd.fsd_logicalStyles ^ nToggle;
					mdr_resetFontStyleDelta(&fsd);
					fsd.fsd_logicalStyles = styles;
					continue;
				}
			} else if (!bEscaped && (c == '[' || (c == '!' && pStream->is_peekc(pStream, 1) == '['))) {
				BOOL bImage = c == '!';
				pStream->is_skip(pStream, bImage ? 2 : 1);
				char* pLink;
				char* pTitle = NULL;
				int nWidth = 0;
				int nHeight = 0;
				char szLinkText[256];
				if (mdr_parseLink(pStream, pState, szLinkText, &pLink, &pTitle, &nWidth, &nHeight)) {
					int nAttr = 0;
					mdr_appendRunState(pStream, pState, &fsd);
					int nTextStart = 0;
					char c2 = szLinkText[0];
					int nTextEnd = (int)strlen(szLinkText);
					if (c2 == szLinkText[nTextEnd - 1]) {
						if (c2 == '`') {
							nAttr = ATTR_CODE;
						}
						else if (c2 == '*' || c2 == '_') {
							if (szLinkText[nTextStart + 1] == c2) {
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
					stringbuf_appendStringLength(pState->hps_text, &szLinkText[nTextStart], nTextEnd - nTextStart);
					fsd.fsd_logicalStyles |= ATTR_LINK;
					TEXT_RUN* pRun = mdr_appendRunState(pStream, pState, &fsd);
					if (bImage) {
						pRun->tr_image = calloc(1, sizeof(MD_IMAGE));
						pRun->tr_image->mdi_height = nHeight;
						pRun->tr_image->mdi_width = nWidth;
					}
					if (bImage && !pTitle) {
						pTitle = _strdup(&stringbuf_getString(pState->hps_text)[pState->hps_lastTextOffset]);
					}
					pRun->tr_link = pLink;
					pRun->tr_title = pTitle;
					pStream->is_skip(pStream, -1);
					pState->hps_runOffset = pStream->is_inputMark(pStream, &pState->hps_lp);
					continue;
				}
			} else if (c == '&') {
				STREAM_OFFSET sp = pStream->is_tell(pStream);
				pStream->is_skip(pStream, 1);
				if (mdr_parseEntity(pState->hps_text, pStream)) {
					pStream->is_skip(pStream, -1);
					continue;
				}
				pStream->is_seek(pStream, sp);
			} else if (!bEscaped && c == '<') {
				HTML_TAG tag;
				STREAM_OFFSET offset = pStream->is_tell(pStream);
				fsdNext = *pState->hps_currentStyle;
				pStream->is_skip(pStream, 1);
				int bSucces = mdr_getTag(pStream, &fsdNext, &tag);
				if (bSucces) {
					mdr_onHtmlTag(pStream, pState, &tag, &fsdNext);
					pStream->is_skip(pStream, -1);
					if (tag.ht_isOpen) {
						fsd = fsdNext;
					}
					continue;
				}
				pStream->is_seek(pStream, offset);
				if (mdr_parseAutolinks(pStream, pState, mdr_getTextFlowEnsureInit(pStream, pState), pState->hps_blockFormat, &fsd, pState->hps_lastTextOffset)) {
					pState->hps_lastTextOffset = (int)stringbuf_size(pState->hps_text);
					continue;
				}
			}
			if (c != '\n') {
				stringbuf_appendChar(pState->hps_text, c);
			}
		}
		if (c == ' ' && lastC == ' ') {
			bEnforceBreak = TRUE;
		} else {
			stringbuf_appendChar(pState->hps_text, ' ');
		}
		if (bEnforceBreak) {
			fsd.fsd_logicalStyles |= ATTR_LINE_BREAK;
		}
		mdr_appendRunState(pStream, pState, &fsd);
		if (mdr_isTopLevelOrBreak(pStream, mType, nLevel)) {
			break;
		}
		nState = 1;
	}
outer:
	if (pState->hps_part) {
		pState->hps_part->rvp_type = mType;
		if (pState->hps_blockFormat) {
			mdr_applyFormat(pState->hps_part, pState->hps_blockFormat);
		}
	}
	mdr_closeTextElement(pStream, pState, FALSE);
}

static RENDER_TABLE_CELL *mdr_parseTableCell(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState, RENDER_TABLE_ROW* pRow, STRING_BUF* pContents, 
		LINE*lp, int nLineOffset, BOOL bHeader) {
	RENDER_TABLE_CELL* pCell = ll_append(&pRow->rtr_cells, sizeof * pCell);
	pCell->rtc_isHeader = bHeader;
	STRING_BUF* sb = stringbuf_create(128);
	char* pszBuf = stringbuf_getString(pContents);
	char* pszEnd = pszBuf + strlen(pszBuf)-1;
	while (pszEnd > pszBuf && pszEnd[-1] == ' ') {
		*--pszEnd = 0;
	}
	INPUT_STREAM* pInnerStream = streams_createStringInputStream(pszBuf);
	pState->hps_tableCell = pCell;
	mdr_parseFlow(pInnerStream, pState, bHeader ? ATTR_STRONG : 0);
	pState->hps_tableCell = 0;
	// Hack: this is the attempt to get selections in table cell working
	// If the table cell contains formatted contents - this won't work correctly.
	TEXT_RUN* pRun = pCell->rtc_flow.tf_runs;
	while (pRun) {
		pRun->tr_lp = lp;
		pRun->tr_lineOffset = nLineOffset;
		pRun = pRun->tr_next;
	}
	pStream->is_destroy(pInnerStream);
	stringbuf_destroy(sb);
	return pCell;
}

/*
 * Parse a table row.
 */
static BOOL mdr_parseTableRow(INPUT_STREAM* pStream, RENDER_TABLE* pTable, HTML_PARSER_STATE* pState, BOOL bHeader, const byte* columnAlignments, int nMaxColumns) {
	long lnOffset;
	int nColumn = 0;
	int nStart = 0;
	RENDER_TABLE_ROW row;
	char c;
	STRING_BUF* pBuf = stringbuf_create(128);
	LINE* lp;
	STREAM_OFFSET offset = pStream->is_tell(pStream);

	memset(&row, 0, sizeof row);
	lnOffset = pStream->is_inputMark(pStream, &lp);
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n' && nColumn < nMaxColumns) {
		if (c == '|') {
			mdr_skipLeadingSpace(pStream);
			if (nStart > 0) {
				RENDER_TABLE_CELL* pCell = mdr_parseTableCell(pStream, pState, &row, pBuf, lp, lnOffset, bHeader);
				if (!pCell) {
					break;
				}
				pCell->rtc_align = columnAlignments[nColumn++];
			}
			stringbuf_reset(pBuf);
			lnOffset = pStream->is_inputMark(pStream, &lp);
			nStart = 1;
		}
		else {
			if (c == '\\' && (c = pStream->is_peekc(pStream, 0)) != 0) {
				pStream->is_skip(pStream, 1);
			}
			stringbuf_appendChar(pBuf, c);
			nStart = 1;
		}
	}
	if (nColumn == 0) {
		stringbuf_destroy(pBuf);
		pStream->is_seek(pStream, offset);
		return FALSE;
	}
	if (nColumn < nMaxColumns && (c == '|' || stringbuf_size(pBuf) > 0)) {
		RENDER_TABLE_CELL* pCell = mdr_parseTableCell(pStream, pState, &row, pBuf, lp, lnOffset, bHeader);
		if (pCell) {
			pCell->rtc_align = columnAlignments[nColumn++];
		}
	}
	stringbuf_destroy(pBuf);
	RENDER_TABLE_ROW* pAppended = (RENDER_TABLE_ROW*)ll_append((LINKED_LIST**)&pTable->rt_rows, sizeof row);
	memcpy(pAppended, &row, sizeof row);
	pStream->is_seek(pStream, offset);
	pStream->is_positionToLineStart(pStream, 1);
	return TRUE;
}

/*
 * Try to interpret some lines as a markdown table. If successful update the line pointer to point beyond the
 * table and return TRUE. If no successful, return FALSE;
 */
static BOOL mdr_parseTable(INPUT_STREAM* pStream, HTML_PARSER_STATE* pState) {
	byte columnAlignments[MAX_TABLE_COLUMNS];
	memset(columnAlignments, 0, sizeof columnAlignments);
	BOOL bStartColon = FALSE;
	BOOL bDashSeen = FALSE;
	int nColumn = -1;
	int nAlign = RTC_ALIGN_LEFT;
	char c;
	RENDER_VIEW_PART* pPart = pState->hps_part;
	if (pPart == 0) {
		// Create a normal part and initialize below (not using factory) as pTable was created already.
		pState->hps_part = pPart = mdr_newPart(pStream, pState, MET_NORMAL, 0);
	}
	STREAM_OFFSET offsetCurrent = pStream->is_positionToLineStart(pStream, 1);
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
		if (nColumn == -1) {
			if (isspace(c)) {
				continue;
			}
			if (c == '|' || c == '-' || c == '=' || c == ':') {
				nColumn = 0;
				if (c == ':') {
					bStartColon = TRUE;
				}
				if (c == '-' || c == '=') {
					nColumn = 1;
					bDashSeen = TRUE;
				}
			}
			else {
				pStream->is_seek(pStream, offsetCurrent);
				return FALSE;
			}
		}
		else {
			if (c == '-' || c == '=') {
				bDashSeen = TRUE;
			}
			else if (bDashSeen && c == ':') {
				nAlign = bStartColon ? RTC_ALIGN_CENTER : RTC_ALIGN_RIGHT;
			}
			else if (!bDashSeen && c == ':') {
				bStartColon = TRUE;
			}
			else if (bDashSeen && c == '|') {
				columnAlignments[nColumn++] = nAlign;
				bDashSeen = FALSE;
				bStartColon = FALSE;
				nAlign = RTC_ALIGN_LEFT;
			} else {
				pStream->is_seek(pStream, offsetCurrent);
				return FALSE;
			}
		}
	}
	if (nColumn < 0) {
		pStream->is_seek(pStream, offsetCurrent);
		return FALSE;
	}
# if 0
	if (lpDef->lbuf[lpDef->len - 1] != '|') {
		columnAlignments[nColumn++] = nAlign;
	}
#endif
	// OK - we have a valid table definition. Now parse header and rows.
	RENDER_TABLE* pTable = mdr_newTable(nColumn);
	pStream->is_seek(pStream, offsetCurrent);
	if (!mdr_parseTableRow(pStream, pTable, pState, TRUE, columnAlignments, nColumn)) {
		free(pTable);
		return FALSE;
	}
	pStream->is_seek(pStream, offsetCurrent);
	pStream->is_positionToLineStart(pStream, 2);
	while (mdr_parseTableRow(pStream, pTable, pState, FALSE, columnAlignments, nColumn)) {
	}
	pPart->rvp_data.rvp_table = pTable;
	pPart->rvp_type = MET_TABLE;
	pPart->rvp_paint = mdr_renderTable;
	mdr_finishTableSetup(pTable);
	mdr_applyFormat(pPart, &_formatTable);
	return TRUE;
}

static void mdr_parseMarkdownFormat(WINFO *wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	char c;
	HTML_PARSER_STATE state;
	int nDelta = 0;

	mdr_initParserState(&state, &pData->md_pElements, fp->fname);
	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		if (c != '\n') {
			if (!mdr_parseTable(pStream, &state)) {
				state.hps_blockLevel = 0;
				mdr_parseFlow(pStream, &state, 0);
			}
			if (state.hps_part) {
				state.hps_part->rvp_margins.m_top += nDelta;
				nDelta = 0;
			}
			mdr_endPart(&state);
		} else {
			nDelta += PARAGRAPH_OFFSET / 2;
			pStream->is_skip(pStream, 1);
		}
	}
	mdr_finalizeParserState(pStream, &state);
	pStream->is_destroy(pStream);
}

/*
 * Returns the n-th view part.
 */
static RENDER_VIEW_PART* mdr_getViewPartAt(RENDER_VIEW_PART* pFirstPart, long n) {
	return (RENDER_VIEW_PART * )ll_at((LINKED_LIST*)pFirstPart, n);
}

/*
 * Returns the view part corresponding with the passed line pointer.
 */
static RENDER_VIEW_PART* mdr_getViewPartForLine(RENDER_VIEW_PART* pFirstPart, LINE* lp, int* pIndex) {
	if (!pFirstPart) {
		return 0;
	}
	RENDER_VIEW_PART* pNext;
	LINE* lpPart = pFirstPart->rvp_lpStart;
	*pIndex = 0;
	while (lpPart) {
		if (lpPart == lp) {
			return pFirstPart;
		}
		lpPart = lpPart->next;
		pNext = pFirstPart->rvp_next;
		while (pNext && (lpPart == pNext->rvp_lpStart || pNext->rvp_lpStart == pFirstPart->rvp_lpStart)) {
			pFirstPart = pNext;
			(*pIndex)++;
			pNext = pNext->rvp_next;
		}
	}
	return 0;
}

static void mdr_updateCaretUI(WINFO* wp, int* pCX, int* pCY, int* pWidth, int* pHeight) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	RENDER_VIEW_PART* pPart = pData->md_caretView;
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

static int mdr_supportsMode(int aMode) {
	if (aMode == SHOWCARET_LINE_HIGHLIGHT || aMode == SHOWLINENUMBERS || aMode == SHOWRULER) {
		return 0;
	}
	return 1;
}

/*
 * Invoked, when the size of the window has changed - update scrollbars to display visible
 * area etc...
 */
static int mdr_windowSizeChanged(HWND hwnd, MARKDOWN_RENDERER_DATA* pData, BOOL bUpdateScrollbarOnly) {
	if (!pData) {
		return 0;
	}
	if (!bUpdateScrollbarOnly) {
		mdr_invalidateViewpartsLayout(pData->md_pElements);
		InvalidateRect(hwnd, 0, FALSE);
		UpdateWindow(hwnd);
	}
	SCROLLINFO info = {
		.nMin = 0,
		.cbSize = sizeof info,
		.fMask = SIF_RANGE | SIF_PAGE | SIF_POS
	};
	info.nPos = 0;
	ShowScrollBar(hwnd, SB_HORZ, FALSE);
	SIZE size;
	mdr_getViewpartsExtend(pData->md_pElements, &size, -1);
	info.nMax = size.cy;
	RECT rect;
	GetClientRect(hwnd, &rect);
	info.nPage = rect.bottom - rect.top;
	mdr_getViewpartsExtend(pData->md_pElements, &size, pData->md_minln);
	info.nPos = size.cy;
	SetScrollInfo(hwnd, SB_VERT, &info, 1);
	return 1;
}

/*
 * Returns the assumed bounds of one view part. The view part must be layouted for this to work.
 */
static void mdr_getViewpartExtend(RENDER_VIEW_PART* pPart, SIZE* pSize) {
	if (pPart->rvp_layouted) {
		pSize->cy = pPart->rvp_height;
		pSize->cx = (pPart->rvp_bounds.right - pPart->rvp_margins.m_right) - (pPart->rvp_bounds.left - pPart->rvp_margins.m_left);
	}
}

/*
 * Returns the view part for an y pixel position assuming all view parts are laid
 * out on one page.
 */
static int mdr_getViewpartAtY(RENDER_VIEW_PART* pFirst, int nY) {
	int nIndex = 0;
	while (pFirst) {
		if (pFirst->rvp_layouted) {
			SIZE sPart;
			mdr_getViewpartExtend(pFirst, &sPart);
			nY -= sPart.cy;
			if (nY < 0) {
				return nIndex;
			}
		}
		pFirst = pFirst->rvp_next;
		nIndex++;
	}
	return -1;
}

/*
 * The vertical scrollbar was dragged - update the window.
 */
static int mdr_scrolled(HWND hwnd, MARKDOWN_RENDERER_DATA* pData, WPARAM wParam) {
	int nMin = pData->md_minln;
	int nCode = LOWORD(wParam);
	if (nCode == SB_LINEUP) {
		nMin--;
	}
	else if (nCode == SB_LINEDOWN) {
		nMin++;
	}
	else if (nCode == SB_PAGEDOWN) {
		nMin += 5;
	}
	else if (nCode == SB_PAGEUP) {
		nMin -= 20;
	}
	else if (nCode == SB_TOP) {
		nMin = 0;
	}
	else if (nCode == SB_BOTTOM) {
		nMin = ll_size((LINKED_LIST*)pData->md_pElements)-2;
	}
	else if (nCode == SB_ENDSCROLL) {
		return 0;
	}
	else {
		nMin = mdr_getViewpartAtY(pData->md_pElements, HIWORD(wParam));
	}
	if (nMin < 0) {
		nMin = 0;
	}
	if (pData->md_minln != nMin) {
		pData->md_minln = nMin;
		mdr_windowSizeChanged(hwnd, pData, TRUE);
		InvalidateRect(hwnd, 0, FALSE);
		return 1;
	}
	return 0;
}

/*
 * Calculate the number of logical lines displayed
 */
static long mdr_calculateMaxLine(WINFO* wp) {
	FTABLE* fp = FTPOI(wp);
	return fp->nlines;
}

static int mdr_adjustScrollBounds(WINFO* wp) {
	SIZE s2;
	SIZE s1;
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	int oldminLn = pData->md_minln;
	if (pData->md_caretLineIndex <= pData->md_minln) {
		pData->md_minln = pData->md_caretLineIndex;
	} else {
		RECT rect;
		GetClientRect(wp->ww_handle, &rect);
		int height = rect.bottom - rect.top;
		mdr_getViewpartsExtend(pData->md_pElements, &s2, pData->md_caretLineIndex+1);
		while(1) {
			mdr_getViewpartsExtend(pData->md_pElements, &s1, pData->md_minln);
			int delta = s2.cy - s1.cy;
			if (delta > height) {
				pData->md_minln++;
			}
			else {
				break;
			}
		}
	}
	if (pData->md_minln < 0) {
		pData->md_minln = 0;
	}
	int delta = oldminLn - pData->md_minln;
	if (delta == 0) {
		return 0;
	}
	RECT r;
	GetClientRect(wp->ww_handle, &r);
	RENDER_VIEW_PART* pPartStartOfPage = mdr_getViewPartAt(pData->md_pElements, pData->md_minln);
	// Hack: for now we need to update maxcursln and mincursln as they are used indepently of the renderer.
	wp->mincursln = pData->md_minln;
	wp->maxcursln = mdr_getViewpartAtY(pPartStartOfPage, r.bottom - r.top) + wp->mincursln;
	if (pData->md_hwndTooltip) {
		ShowWindow(pData->md_hwndTooltip, SW_HIDE);
	}
	int width;
	mdr_windowSizeChanged(wp->ww_handle, wp->r_data, TRUE);
	if (delta == 1) {
		UpdateWindow(wp->ww_handle);
		ScrollWindow(wp->ww_handle, 0, pPartStartOfPage->rvp_height, &r, (LPRECT)0);
	} else if (delta < 0 && delta >= -3) {
		int h = 0;
		RENDER_VIEW_PART* pPart = mdr_getViewPartAt(pData->md_pElements, oldminLn);
		while (oldminLn != pData->md_minln && pPart) {
			mdr_getViewpartExtend(pPart, &s1);
			h += s1.cy;
			pPart = pPart->rvp_next;
			oldminLn++;
		}
		UpdateWindow(wp->ww_handle);
		ScrollWindow(wp->ww_handle, 0, -h, &r, (LPRECT)0);
	} else {
		InvalidateRect(wp->ww_handle, (LPRECT)0, 0);
	}
	UpdateWindow(wp->ww_handle);
	mdr_updateCaretUI(wp, &wp->cx, &wp->cy, &width, &wp->cheight);
	render_updateCaret(wp);
	return 1;
}

/*
 * Invalidate the layout of all view parts starting with 'pFirst'.
 */
void mdr_invalidateViewpartsLayout(RENDER_VIEW_PART* pFirst) {
	while (pFirst) {
		// clear out old existing bounds.
		memset(&pFirst->rvp_bounds, 0, sizeof pFirst->rvp_bounds);
		pFirst->rvp_layouted = 0;
		pFirst->rvp_height = 0;
		pFirst = pFirst->rvp_next;
	}
}

/*
 * Returns the total extent in pixels of the current layout described by the list
 * of view parts. If nUpToPart is greater or equals to 0, we will get the extend
 * up to not including the part with the given index. If -1 is passed we will get
 * the extent of all viewparts.
 */
void mdr_getViewpartsExtend(RENDER_VIEW_PART* pFirst, SIZE* pSize, int nUpToPart) {
	pSize->cx = 10;
	pSize->cy = 0;
	int nIndex = 0;
	while (pFirst) {
		if (nUpToPart >= 0 && nUpToPart == nIndex) {
			break;
		}
		if (pFirst->rvp_layouted) {
			SIZE sPart;
			mdr_getViewpartExtend(pFirst, &sPart);
			int nWidth = sPart.cx;
			if (nWidth > pSize->cy) {
				pSize->cx = nWidth;
			}
			pSize->cy += sPart.cy;
		}
		pFirst = pFirst->rvp_next;
		nIndex++;
	}
}

typedef struct tagSELECTION_PARAM {
	LINE* sp_lp;
	int   sp_offset;
	int   sp_size;
} SELECTION_PARAM;

static int mdr_defineSelectionForRuns(TEXT_RUN* pRun, SELECTION_PARAM* pParam) {
	while (pRun) {
		int nStart = pParam->sp_offset - pRun->tr_lineOffset;
		if (pRun->tr_lp == pParam->sp_lp && nStart >= 0 && nStart + pParam->sp_size <= pRun->tr_size) {
			pRun->tr_selectionStart = nStart;
			pRun->tr_selectionLength = pParam->sp_size;
		}
		else {
			pRun->tr_selectionLength = 0;
		}
		pRun = pRun->tr_next;
	}
	return 1;
}

/*
 * Visitor function visiting all text runs traversable from a view part and executing a callback function on each list
 * of text runs with a custom parameter. If the callback returns 0, visiting stops.
 */
static int mdr_forRunListDo(RENDER_VIEW_PART* pPart, int (*runCallback)(TEXT_RUN* pRunList, void* pParam), void* pParam) {
	if (pPart->rvp_paint == mdr_renderMarkdownBlockPart) {
		TEXT_RUN* pRun = pPart->rvp_data.rvp_flow.tf_runs;
		return runCallback(pRun, pParam);
	}
	else if (pPart->rvp_paint == mdr_renderTable) {
		RENDER_TABLE_ROW* pRows = pPart->rvp_data.rvp_table->rt_rows;
		while (pRows) {
			RENDER_TABLE_CELL* pCell = pRows->rtr_cells;
			while (pCell) {
				if (!runCallback(pCell->rtc_flow.tf_runs, pParam)) {
					return 0;
				}
				pCell = pCell->rtc_next;
			}
			pRows = pRows->rtr_next;
		}
	}
	return 1;
}

static void mdr_defineSelectionForPart(RENDER_VIEW_PART* pPart, LINE* lp, int nOffset, int nSize) {
	mdr_forRunListDo(pPart, mdr_defineSelectionForRuns, &(SELECTION_PARAM){.sp_lp = lp, .sp_offset = nOffset, .sp_size = nSize});
}

/*
 * Render the current window displaying MARKDOWN/HTML wysiwyg contents.
 */
static void mdr_renderPage(RENDER_CONTEXT* pCtx, void (*parsePage)(WINFO* wp), RECT* pClip, HBRUSH hBrushBg, int y) {
	RECT rect;
	WINFO* wp = pCtx->rc_wp;
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	GetClientRect(wp->ww_handle, &rect);
	if (!pData->md_pElements) {
		parsePage(wp);
	}
	RECT occupiedBounds;
	int nElements = 0;
	FillRect(pCtx->rc_hdc, pClip, hBrushBg);
	BOOL bEndOfPage = FALSE;
	RENDER_VIEW_PART* pPart;
	for (pPart = pData->md_pElements; pPart; pPart = pPart->rvp_next) {
		if (!pPart->rvp_layouted) {
			pPart->rvp_paint(pPart, pCtx->rc_hdc, &rect, &occupiedBounds, wp->zoomFactor, TRUE);
		}
	}
	pPart = mdr_getViewPartAt(pData->md_pElements, pData->md_minln);
	for (; pPart; rect.top = occupiedBounds.bottom) {
		if (wp->blstart && wp->blend && wp->blstart->m_linePointer == wp->blend->m_linePointer) {
			LINE* lp = wp->blstart->m_linePointer;
			int nSize = wp->blend->m_column - wp->blstart->m_column;
			mdr_defineSelectionForPart(pPart, lp, wp->blstart->m_column, nSize);
		}
		if (!bEndOfPage || !pPart->rvp_layouted) {
			pPart->rvp_paint(pPart, pCtx->rc_hdc, &rect, &occupiedBounds, wp->zoomFactor, bEndOfPage);
		}
		pPart = pPart->rvp_next;
		if (occupiedBounds.bottom > rect.bottom) {
			if (!bEndOfPage) {
				bEndOfPage = TRUE;
			}
		}
		nElements++;
	}
}

/*
 * Render the current window displaying MARKDOWN wysiwyg contents.
 */
static void mdr_renderMarkdownFormatPage(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	mdr_renderPage(pCtx, mdr_parseMarkdownFormat, pClip, hBrushBg, y);
}

/*
 * Render the current window displaying HTML wysiwyg contents.
 */
static void mdr_renderHTMLFormatPage(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y) {
	mdr_renderPage(pCtx, mdr_parseFileToHTML, pClip, hBrushBg, y);
}

/*
 * Render some render view parts as a response to a WM_PAINT message into the window given with 'hwnd',
 * using the repaint areas and device context defined in 'ps' assuming the screen top is scrolled to
 * logical position 'nTopY' render the linked list of view parts starting with 'pFirst'.
 */
void mdr_renderViewparts(HWND hwnd, PAINTSTRUCT* ps, int nTopY, RENDER_VIEW_PART* pFirst) {
	RECT rect;
	RECT occupiedBounds;
	HDC hdc = ps->hdc;
	RECT* pClip = &ps->rcPaint;
	GetClientRect(hwnd, &rect);
	HBRUSH hBrushBg = theme_getDialogBackgroundBrush();
	RENDER_VIEW_PART* pPart = pFirst;

	FillRect(hdc, pClip, hBrushBg);
	rect.top -= nTopY;
	for (; pPart && rect.top < pClip->bottom; rect.top = occupiedBounds.bottom) {
		SIZE sPart;
		mdr_getViewpartExtend(pPart, &sPart);
		if (pPart->rvp_layouted && rect.top+sPart.cy < pClip->top) {
			occupiedBounds.bottom = rect.top + sPart.cy;
		} else {
			pPart->rvp_paint(pPart, hdc, &rect, &occupiedBounds, 1.0f, FALSE);
		}
		pPart = pPart->rvp_next;
		if (pPart == 0 || (pPart->rvp_layouted && occupiedBounds.bottom > rect.bottom)) {
			break;
		}
	}
}

/*
 * Destroy a text run possibly release the link. 
 */
static int mdr_destroyRun(TEXT_RUN* pRun) {
	free(pRun->tr_link);
	free(pRun->tr_title);
	free(pRun->tr_anchor);
	if (pRun->tr_image) {
		if (pRun->tr_image->mdi_image) {
			DeleteObject(pRun->tr_image->mdi_image);
		}
		free(pRun->tr_image);
	}
	return 1;
}

/*
 * Destroy one view part. 
 */
static int mdr_destroyViewPart(RENDER_VIEW_PART *pRVP) {
	if (pRVP->rvp_type == MET_TABLE) {
		mdr_destroyTable(pRVP);
	} else {
		free(pRVP->rvp_data.rvp_flow.tf_text);
		ll_destroy(&pRVP->rvp_data.rvp_flow.tf_runs, mdr_destroyRun);
	}
	free(pRVP->rvp_decoration);
	return 1;
}

/*
 * Destroy a list of view parts releasing unneeded memory.
 */
void mdr_destroyViewParts(RENDER_VIEW_PART** pHEAD) {
	ll_destroy(pHEAD, mdr_destroyViewPart);
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
		mdr_destroyViewParts(&pData->md_pElements);
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
		// do not: UpdateWindow(wp->ww_handle); at this point the model may not be completely edited - further editing operations
		// may follow - so invalidate only and paint when really done.
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
	return 200000;
}

/*
 * Calculate the maximum column for column moves. 
 */
static long mdr_calculateMaxColumn(WINFO* wp, long ln, LINE* lp) {
	return 200000;
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

static int mdr_placeCaret(WINFO* wp, long* ln, long offset, long* col, int updateVirtualOffset, int xDelta) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	// This is a hack for checking, whether we only scrolled up/down by 1: ideally relative caret positioning should be passed on to the renderer
	// if we scroll one buffer line up or down we simulate a scroll up/down by markdown paragrah.
	int dl = offset == wp->caret.offset ? *ln - wp->caret.ln : 0;
	RENDER_VIEW_PART* pPart = pData->md_caretView;
	if (dl < 0 && pData->md_caretLineIndex == 0) {
		return 0;
	}
	if (pPart) {
		if (dl == 1 && pPart->rvp_next) {
			dl = ln_cnt(pPart->rvp_lpStart, pPart->rvp_next->rvp_lpStart) - 1;
			if (dl == 0) {
				dl = 1;
			}
			*ln = wp->caret.ln + dl;
		}
		else if (dl == -1 && pData->md_caretLineIndex > 0) {
			RENDER_VIEW_PART* pPartPrevious = mdr_getViewPartAt(pData->md_pElements, pData->md_caretLineIndex - 1);
			dl = ln_cnt(pPartPrevious->rvp_lpStart, pPart->rvp_lpStart) - 1;
			if (dl == 0) {
				dl = 1;
			}
			*ln = wp->caret.ln - dl;
		}
	}
	int nColumn = *col;
	if (!caret_placeCursorAndValidate(wp, ln, offset, col, updateVirtualOffset, xDelta)) {
		return 0;
	}
	int nMDCaretLine;
	pPart = mdr_getViewPartForLine(pData->md_pElements, wp->caret.linePointer, &nMDCaretLine);
	pData->md_caretView = pPart;
	pData->md_caretLineIndex = nMDCaretLine;
	if (pPart) {
		pData->md_flowCaretIndex = nColumn;
	}
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
}

typedef struct tagHIT_TEST_RUNLIST {
	long htr_currentColumn;
	long htr_resultColumn;
	int  htr_cx;
	int  htr_cy;
	TEXT_RUN* htr_matchedRun;
} HIT_TEST_RUNLIST;

static int mdr_hitTestRunList(TEXT_RUN* pRuns, HIT_TEST_RUNLIST* pParam) {
	long intraCol;
	pParam->htr_matchedRun = 0;
	if (mdr_hitTestTextRuns(pRuns, (POINT) { pParam->htr_cx, pParam->htr_cy }, & intraCol, & pParam->htr_matchedRun)) {
		pParam->htr_resultColumn = intraCol + pParam->htr_currentColumn;
		return FALSE;
	}
	pParam->htr_currentColumn += ll_size((LINKED_LIST*)pRuns);
	return TRUE;
}

static BOOL mdr_hitTestInternal(MARKDOWN_RENDERER_DATA* pData, int cx, int cy, long* pLine, long* pCol, 
			RENDER_VIEW_PART** pMatchedPart, TEXT_RUN **pMatchedRun) {
	if (!pData) {
		return FALSE;
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartAt(pData->md_pElements, pData->md_minln);
	POINT pt = { cx, cy };
	HIT_TEST_RUNLIST param = {
		.htr_cx = cx,
		.htr_cy = cy,
	};
	long ln = pData->md_minln;
	while (pPart) {
		if (PtInRect(&pPart->rvp_bounds, pt)) {
			*pLine = ln; 
			param.htr_currentColumn = 0;
			param.htr_matchedRun = 0;
			mdr_forRunListDo(pPart, mdr_hitTestRunList, &param);
			if (param.htr_matchedRun) {
				*pMatchedRun = param.htr_matchedRun;
				*pMatchedPart = pPart;
				*pLine = ln;
				*pCol = param.htr_resultColumn;
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
	mdr_hitTestInternal(wp->r_data, cx, cy, pLine, pCol, &pMatchP, &pMatchR);
}

static void mdr_setRollover(HWND hwnd, TEXT_RUN* pRun, BOOL aFlag) {
	if (!pRun || pRun->tr_attributes.rollover == aFlag || !pRun->tr_link) {
		return;
	}
	pRun->tr_attributes.rollover = aFlag;
	InvalidateRect(hwnd, (RECT*) & pRun->tr_bounds, FALSE);
}

static void mdr_mouseMove(HWND hwnd, MARKDOWN_RENDERER_DATA* pData, int x, int y) {
	RENDER_VIEW_PART* pMatchP;
	TEXT_RUN* pMatchR = NULL;
	long line;
	long col;
	BOOL bShow = TRUE;
	if (!mdr_hitTestInternal(pData, x, y, &line, &col, &pMatchP, &pMatchR)) {
		bShow = FALSE;
	}
	if (pMatchR == pData->md_focussedRun) {
		return;
	}
	mdr_setRollover(hwnd, pData->md_focussedRun, FALSE);
	pData->md_focussedRun = pMatchR;
	mdr_setRollover(hwnd, pMatchR, TRUE);
	TTTOOLINFO toolinfo = { 0 };
	toolinfo.cbSize = sizeof(toolinfo);
	toolinfo.hwnd = hwnd;
	toolinfo.hinst = hInst;
	if (pMatchR && pMatchR->tr_title) {
		// Activate the tooltip.
		toolinfo.lpszText = pMatchR->tr_title;
		SendMessage(pData->md_hwndTooltip, TTM_UPDATETIPTEXT, (WPARAM)0, (LPARAM)&toolinfo);
		POINT pt;
		pt.x = pMatchR->tr_bounds.left1;
		pt.y = pMatchR->tr_bounds.top1 - 20;
		ClientToScreen(hwnd, &pt);
		SendMessage(pData->md_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x, pt.y));
	} else {
		bShow = FALSE;
	}
	if (!SendMessage(pData->md_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)bShow, (LPARAM)&toolinfo)) {
		log_errorArgs(DEBUG_ERR, "Activating tooltip failed. Error %ld.", GetLastError());
	}
}

/*
 * Returns the renderer data for the given window. 
 */
static MARKDOWN_RENDERER_DATA* mdr_dataFromWindow(HWND hwnd) {
	WINFO* wp = ww_winfoFromWorkwinHandle(hwnd);
	return wp ? wp->r_data : 0;
}

/*
 * Custom window procedure used in markdown renderers.
 */
static LRESULT mdr_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	MARKDOWN_RENDERER_DATA* pData;

	switch (message) {
	case WM_MOUSEMOVE:
		if ((pData = mdr_dataFromWindow(hwnd)) != 0) {
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			mdr_mouseMove(hwnd, pData, xPos, yPos);
		}
		break;
	case WM_MOUSEWHEEL:
		if ((pData = mdr_dataFromWindow(hwnd)) != 0) {
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			mdr_scrolled(hwnd, pData, zDelta < 0 ? SB_LINEDOWN : SB_LINEUP);
		}
		return 0;
	case WM_VSCROLL:
		if ((pData = mdr_dataFromWindow(hwnd)) != 0) {
			mdr_scrolled(hwnd, pData, wParam);
		}
		return 0;
	case WM_SHOWWINDOW:
		if (!wParam) {
			return 0;
		}
	case WM_SIZE:
		if (message == WM_SIZE && !IsWindowVisible(hwnd)) {
			return 0;
		}
		if ((pData = mdr_dataFromWindow(hwnd)) != 0) {
			mdr_windowSizeChanged(hwnd, pData, FALSE);
			return 0;
		}
		break;
	}
	return render_defaultWindowProc(hwnd, message, wParam, lParam);
}

typedef struct tag_RUN_OFFSET {
	int ro_inputOffset;
	int ro_currentOffset;
	TEXT_RUN* ro_match;
} RUN_OFFSET;

static int mdr_getRunFromRunsAtOffset(TEXT_RUN* pRuns, RUN_OFFSET* pOffsets) {
	int nCount = ll_size((LINKED_LIST*)pRuns);
	pOffsets->ro_currentOffset += nCount;
	if (pOffsets->ro_currentOffset > pOffsets->ro_inputOffset) {
		pOffsets->ro_match = (TEXT_RUN*)ll_at((LINKED_LIST*)pRuns, pOffsets->ro_inputOffset - pOffsets->ro_currentOffset + nCount);
		return 0;
	}
	return 1;
}

static TEXT_RUN* mdr_getRunAtOffset(RENDER_VIEW_PART* pPart, int nOffset) {
	RUN_OFFSET pParam = (RUN_OFFSET){ .ro_inputOffset = nOffset, .ro_currentOffset = 0, .ro_match = 0 };
	mdr_forRunListDo(pPart, mdr_getRunFromRunsAtOffset, &pParam);
	return pParam.ro_match;
}

static BOOL mdr_findLink(WINFO* wp, char* pszBuf, size_t nMaxChars, NAVIGATION_INFO_PARSE_RESULT* pResult) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	RENDER_VIEW_PART* pPart = pData->md_caretView;
	if (pPart) {
		TEXT_RUN* pRun = mdr_getRunAtOffset(pPart, pData->md_flowCaretIndex);
		if (pRun && !pRun->tr_image && pRun->tr_link && strlen(pRun->tr_link) < nMaxChars) {
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

/*
 * Check, whether a text flow matches an anchor. 
 * Used to navigate to an anchor inside markdown documents.
 */
static int mdr_titleMatchesAnchor(TEXT_FLOW* pTitle, const char*pszTitleAnchor, const char* pszAnchor) {
	if (pszTitleAnchor && _stricmp(pszTitleAnchor, pszAnchor) == 0) {
		return 1;
	}
	if (!pTitle) {
		return 0;
	}
	char szTitleAsAnchor[512];
	char* pszTitle = pTitle->tf_text;
	if (strlen(pszTitle) < sizeof szTitleAsAnchor) {
		char* pszDest = szTitleAsAnchor;
		while (*pszTitle) {
			char c = *pszTitle++;
			if (c == ' ') {
				if (!*pszTitle) {
					break;
				}
				c = '-';
			}
			*pszDest++ = c;
		}
		*pszDest = 0;
		return (_stricmp(pszAnchor, szTitleAsAnchor) == 0);
	}
	return 0;
}

typedef struct tagANCHOR_MATCH {
	const char* am_matchAnchor;
	int         am_offsetIdx;
	int			am_matched;
} ANCHOR_MATCH;

/*
 * Match an anchor agains the anchor added to a run.
 */
static int mdr_matchAnchor(TEXT_RUN* pRun, ANCHOR_MATCH* pMatch) {
	if (pRun->tr_anchor && mdr_titleMatchesAnchor(0, pRun->tr_anchor, pMatch->am_matchAnchor)) {
		pMatch->am_matched = 1;
		return 0;
	}
	pMatch->am_offsetIdx++;
	return 1;
}

static void mdr_navigateAnchor(WINFO* wp, const char* pszAnchor) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return;
	}
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	long nCol = 0;
	long nLine;
	while (pPart) {
		ANCHOR_MATCH match = { .am_matchAnchor = pszAnchor, .am_matched = 0, .am_offsetIdx = 0 };
		mdr_forRunListDo(pPart, mdr_matchAnchor, &match);
		if (match.am_matched) {
			nCol = match.am_offsetIdx;
			goto anchorMatched;
		}
		if (pPart->rvp_type == MET_HEADER) {
			if (mdr_titleMatchesAnchor(&pPart->rvp_data.rvp_flow, 0, pszAnchor)) {
anchorMatched:	
				nLine = ln_indexOf(wp->fp, pPart->rvp_lpStart);
				mdr_placeCaret(wp, &nLine, 0, &nCol, 0, 0);
				wt_curpos(wp, nLine, 0);
				return;
			}
		}
		pPart = pPart->rvp_next;
	}
}

static int mdr_placeCaretAfterClick(WINFO* wp, long* ln, long* col, int updateVirtualColumn) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	RENDER_VIEW_PART* rvp = mdr_getViewPartAt(pData->md_pElements, *ln);
	if (rvp) {
		FTABLE* fp = wp->fp;
		*ln = ln_cnt(fp->firstl, rvp->rvp_lpStart)-1;
	}
	return mdr_placeCaret(wp, ln, *col, col, updateVirtualColumn, 0);
}

static int mdr_repaint(WINFO* wp, int ln1, int ln2, int col1, int col2) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (wp->ww_handle && pData) {
		FTABLE* fp = wp->fp;
		LINE* lp1 = ln_goto(fp, ln1);
		LINE* lp2 = ln_goto(fp, ln2);
		int dummy;
		RENDER_VIEW_PART* p1 = mdr_getViewPartForLine(pData->md_pElements, lp1, &dummy);
		RENDER_VIEW_PART* p2 = mdr_getViewPartForLine(pData->md_pElements, lp2, &dummy);
		if (p1 != 0 && p1 == p2) {
			render_invalidateRect(wp, &p1->rvp_bounds);
		}
		else {
			render_invalidateRect(wp, NULL);
		}
		return 1;
	}
	return 0;
}

static RENDERER _mdrRenderer = {
	render_singleLineOnDevice,
	.r_renderPage = mdr_renderMarkdownFormatPage,
	.r_placeCaret = mdr_placeCaret,
	mdr_calculateMaxLine,
	mdr_calculateLongestLine,
	mdr_calculateMaxColumn,
	.r_placeCaretAfterClick = mdr_placeCaretAfterClick,
	mdr_screenOffsetToBuffer,
	mdr_allocData,
	mdr_destroyData,
	mdr_scroll,
	mdr_adjustScrollBounds,
	.r_updateCaretUI = mdr_updateCaretUI,
	.r_supportsMode = mdr_supportsMode,
	.r_hitTest = mdr_hitTest,
	FALSE,
	.r_modelChanged = mdr_modelChanged,
	.r_repaint = mdr_repaint,
	.r_context = "wysiwyg-renderer",
	.r_findLink = mdr_findLink,
	.r_navigateAnchor = mdr_navigateAnchor,
	.r_wndProc = mdr_wndProc
};

static RENDERER _htmlRenderer = {
	.r_renderPage = mdr_renderHTMLFormatPage,
};

/*
 * Returns a markdown renderer.
 */
RENDERER* mdr_getRenderer() {
	return &_mdrRenderer;
}

/*
 * Returns an HTML wysiwyg renderer.
 */
RENDERER* mdr_getHTMLRenderer() {
	memcpy(&_htmlRenderer, &_mdrRenderer, sizeof _mdrRenderer);
	_htmlRenderer.r_renderPage = mdr_renderHTMLFormatPage;
	_htmlRenderer.r_context = "wysiwyg-renderer";
	return &_htmlRenderer;
}

