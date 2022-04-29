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
	MD_IMAGE*			tr_image;
	int					tr_selectionStart;
	int					tr_selectionLength;
} TEXT_RUN;

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
	if (!bMeasureOnly) {
		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, left, y, right, y + nTableHeight);
		int x = left;
		for (int i = 0; i < nColumns; i++) {
			x += nColumnWidth[i];
			MoveTo(hdc, x, y);
			LineTo(hdc, x, y + nTableHeight);
		}
	}
	DeleteObject(SelectObject(hdc, hPenOld));
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
		hdcMem = CreateCompatibleDC(hdc);
		oldBitmap = SelectObject(hdcMem, pImage->mdi_image);
		GetObject(pImage->mdi_image, sizeof(bitmap), &bitmap);
		int nWidth = pImage->mdi_width ? pImage->mdi_width : 
			(pImage->mdi_height ? bitmap.bmWidth * pImage->mdi_height / bitmap.bmHeight : bitmap.bmWidth);
		int nHeight = pImage->mdi_height ? pImage->mdi_height : 
			(pImage->mdi_width ? bitmap.bmHeight * pImage->mdi_width / bitmap.bmWidth : bitmap.bmHeight);
		nWidth = (int)(nWidth * zoomFactor);
		nHeight = (int)(nHeight * zoomFactor);
		if (!bMeasureOnly) {
			SetStretchBltMode(hdc, COLORONCOLOR);
			StretchBlt(hdc, x, y, nWidth, nHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
		}
		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);
		pSize->cx = nWidth;
		pSize->cy = nHeight;
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
	if (!pTR) {
		return;
	}
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
	HBRUSH hBrOld = SelectObject(hdc, theme_getDialogLightBackgroundBrush());
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
	if (pPart->rvp_data.rvp_flow.tf_text) {
		mdr_renderTextFlow(pMargins, &pPart->rvp_data.rvp_flow, pBounds, &pPart->rvp_bounds, pUsed,
				pPart->rvp_type == MET_BLOCK_QUOTE ? pPart->rvp_level : 0, RTC_ALIGN_LEFT, &rfp);
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
static MDR_ELEMENT_TYPE mdr_determineTopLevelElement(INPUT_STREAM* pStream, int* pLevel, MDR_ELEMENT_FORMAT** pFormat, long* pNumber) {
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
	*pFormat = mdr_getFormatFor(mType, *pLevel);
	return mType;
}

static TEXT_RUN* mdr_appendRun(TEXT_RUN** pRuns, MDR_ELEMENT_FORMAT* pFormat, size_t nSize, FONT_STYLE_DELTA *pFSD, LINE* lp, int nLineOffset) {
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
	return pRun;
}

/*
 * Parse the link URL - allow for things as included title specs. 
 */
static BOOL mdr_parseLinkUrl(char* pszRelative, char* pszBuf, char** pszLink, char** pszTitle, int* nWidth, int* nHeight) {
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
	}
	else {
		strcpy(szLink, pszBuf);
		if (pszRelative && szLink[0] != '#' && strstr(szLink, "//") == 0) {
			char szFilename[EDMAXPATHLEN];
			string_splitFilename(pszRelative, szFilename, NULL);
			string_concatPathAndFilename(szFilename, szFilename, szLink);
			strcpy(szLink, szFilename);
		}
	}
	*pszLink = _strdup(szLink);
	if (szTitle[0]) {
		*pszTitle = _strdup(szTitle);
	}
	else {
		*pszTitle = NULL;
	}
	return TRUE;
}

static BOOL mdr_parseLink(char* pszRelative, INPUT_STREAM* pStream, char* szLinkText,
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
				return mdr_parseLinkUrl(pszRelative, szBuf, pszLink, pszTitle, nWidth, nHeight);
			}
			if (nLinkStart < sizeof szBuf - 1) {
				szBuf[nLinkStart++] = c;
			}
		}
	}
	return FALSE;
}

/*
 * Parse a fenced code block and add the runs to the view part.
 */
static void mdr_parsePreformattedCodeBlock(RENDER_VIEW_PART* pPart, INPUT_STREAM* pStream, STRING_BUF* pSB, BOOL bIndented, BOOL bHTML) {
	size_t nLastOffset = 0;
	int nOffs;
	int nRunStart = 0;
	char c;
	LINE* lp;

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
		else if (!bHTML && (pStream->is_strncmp(pStream, "```", 3) == 0 || pStream->is_strncmp(pStream, "~~~", 3) == 0)) {
			pStream->is_positionToLineStart(pStream, 1);
			return;
		} else if (bHTML && c == '<' && pStream->is_strncmp(pStream, "</pre>", 6) == 0) {
			pStream->is_positionToLineStart(pStream, 1);
			return;
		}
		pPart->rvp_number++;
		nRunStart = pStream->is_inputMark(pStream, &lp);
		while ((c = pStream->is_getc(pStream)) != 0 && c != '\n') {
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
		mdr_appendRun(&pPart->rvp_data.rvp_flow.tf_runs, &_formatFenced, nSize, 
			&(FONT_STYLE_DELTA){ATTR_LINE_BREAK, .fsd_textColor = NO_COLOR, 0, .fsd_fillColor = NO_COLOR, .fsd_strokeColor = NO_COLOR}, lp, nRunStart);
		nLastOffset += nSize;
	}
}

/*
 * Parses the auto-link syntax.
 */
static BOOL mdr_parseAutolinks(TEXT_FLOW* pFlow, INPUT_STREAM* pStream, STRING_BUF* pSB, MDR_ELEMENT_FORMAT* pFormat, FONT_STYLE_DELTA* pFSD, 
		size_t pLastOffset) {
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
		size_t nSize = stringbuf_size(pSB) - pLastOffset;
		mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, pFSD, lp, nLineOffset);
		pLastOffset += nSize;
		char* pszLink;
		char* pszTitle;
		stringbuf_appendString(pSB, szTemp);
		TEXT_RUN* pRun = mdr_appendRun(&pFlow->tf_runs, pFormat, nUrlLen, &(FONT_STYLE_DELTA){ATTR_LINK, 
			.fsd_textColor = NO_COLOR, 0, .fsd_fillColor = NO_COLOR, .fsd_strokeColor = NO_COLOR}, lp, nLineOffset);
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
		pPart->rvp_decoration = calloc(1, sizeof * (pPart->rvp_decoration));
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

static struct tagHTML_TAG_MAPPING {
	const char* tm_element;
	MDR_ELEMENT_TYPE tm_blockElement;
	int tm_elementLevel;				// For HX and corresponding tag types - the level - otherwise 0.
	int tm_textAttr;
	int tm_textAttrClose;
	int tm_indent;
} _tagMappings[] = {
	{"em", MET_NONE, 0, ATTR_EMPHASIS},
	{"sup", MET_NONE, 0, ATTR_SUPER},
	{"sub", MET_NONE, 0, ATTR_SUB},
	{"a", MET_NONE, 0, ATTR_LINK},
	{"b", MET_NONE, 0, ATTR_STRONG},
	{"dt", MET_NONE, 0, 0, ATTR_LINE_BREAK, DEFAULT_LEFT_MARGIN*2},
	{"dd", MET_NONE, 0, ATTR_STRONG, ATTR_LINE_BREAK},
	{"i", MET_NONE, 0, ATTR_EMPHASIS},
	{"u", MET_NONE, 0, ATTR_UNDERLINE},
	{"del", MET_NONE, 0, ATTR_STRIKE},
	{"strike", MET_NONE, 0, ATTR_STRIKE},
	{"s", MET_NONE, 0, ATTR_STRIKE},
	{"code", MET_NONE, 0, ATTR_TAG_CODE},
	{"dl", MET_NONE, 0, ATTR_LINE_BREAK},
	{"span", MET_NONE, 0, 0},
	{"br", MET_NONE, 0, ATTR_LINE_BREAK},
	{"p", MET_PARAGRAPH, 0, ATTR_LINE_BREAK},
	{"script", MET_FENCED_CODE_BLOCK, 0},
	{"pre", MET_FENCED_CODE_BLOCK, 0},
	{"table", MET_TABLE, 0},
	{"hr", MET_HORIZONTAL_RULE, 0},
	{"tr", MET_NONE, 0},
	{"td", MET_NONE, 0},
	{"th", MET_NONE, 0},
	{"h1", MET_HEADER, 1},
	{"h2", MET_HEADER, 2},
	{"h3", MET_HEADER, 3},
	{"h4", MET_HEADER, 4},
	{"h5", MET_HEADER, 5},
	{"h6", MET_HEADER, 6},
	{"ul", MET_NONE, 1},
	{"li", MET_UNORDERED_LIST, 1},
	{"ol", MET_NONE, 1},
	{"tt", MET_NONE, 0, ATTR_TAG_CODE}
};

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
	HPS_WAIT_FOR_VALUE = 4,
	HPS_VALUE = 5
} HTML_PARSE_STATE;
static int mdr_getTag(INPUT_STREAM* pStream, FONT_STYLE_DELTA* pFSD, struct tagHTML_TAG_MAPPING** ppMapping, BOOL* pEnd) {
	char szTag[10];
	char szAttribute[32];
	static char szValue[128];
	char* pszAttr = 0;
	char* pszValue = 0;
	char* pszDest = szTag;
	size_t nDestSize = sizeof szTag;
	HTML_PARSE_STATE nState = HPS_INIT;
	if (pStream->is_peekc(pStream, 0) == '/') {
		*pEnd = 1;
		pStream->is_skip(pStream, 1);
	}
	else {
		*pEnd = 0;
	}
	pFSD->fsd_styleName = 0;
	while (nDestSize > 1) {
		char c = pStream->is_getc(pStream);
		if (c == '>') {
			*pszDest = 0;
			for (int nElement = 0; nElement < DIM(_tagMappings); nElement++) {
				if (strcmp(_tagMappings[nElement].tm_element, szTag) == 0) {
					*ppMapping = &_tagMappings[nElement];
					pFSD->fsd_indent = (*ppMapping)->tm_indent;
					return 1;
				}
			}
			return 0;
		}
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
				nState = HPS_BETWEEN_ATTR;
			}
			break;
		case HPS_WAIT_FOR_VALUE:
		case HPS_BETWEEN_ATTR:
			if (isalnum((unsigned char)c)) {
				pszAttr = szAttribute;
				*pszAttr++ = tolower(c);
				nState = HPS_ATTR;
			}
			if (c == '"' && nState == HPS_WAIT_FOR_VALUE) {
				nState = HPS_VALUE;
				pszValue = szValue;
			}
			break;
		case HPS_ATTR:
			if (isalnum((unsigned char)c)) {
				if (pszAttr < (szAttribute + sizeof szAttribute - 1)) {
					*pszAttr++ = tolower(c);
				}
			}
			else {
				nState = HPS_WAIT_FOR_VALUE;
			}
			break;
		case HPS_VALUE:
			if (c != '"') {
				if (pszValue < (szValue + sizeof szValue - 1)) {
					*pszValue++ = c;
				}
			} else {
				*pszValue = 0;
				if (strcmp("class", szAttribute) == 0) {
					pFSD->fsd_styleName = szValue;
				} else if (strcmp("style", szAttribute) == 0) {
					mdr_parseStyle(pFSD, szValue);
				}
				nState = HPS_BETWEEN_ATTR;
			}
		}
	}
	return 0;
}

/*
 * Parse an HTML entity spec and convert into a character if successful. 
 */
static int mdr_parseEntity(STRING_BUF* pSB, INPUT_STREAM* pStream) {
	for (int nE = 0; nE < DIM(_entities); nE++) {
		size_t nLen = strlen(_entities[nE].entity);
		if (pStream->is_strncmp(pStream, _entities[nE].entity, nLen) == 0) {
			stringbuf_appendChar(pSB, _entities[nE].c);
			pStream->is_skip(pStream, (int)nLen-1);
			return 1;
		}
	}
	return 0;
}

static void mdr_resetFontStyleDelta(FONT_STYLE_DELTA* pFSD) {
	pFSD->fsd_logicalStyles = 0;
	pFSD->fsd_styleName = 0;
	pFSD->fsd_indent = 0;
	pFSD->fsd_textColor = NO_COLOR;
	pFSD->fsd_fillColor = NO_COLOR;
	pFSD->fsd_strokeColor = NO_COLOR;
}

static RENDER_VIEW_PART* mdr_newPart(RENDER_VIEW_PART** pFirst, RENDER_VIEW_PART* pExisting, 
		FONT_STYLE_DELTA* pFSD, MDR_ELEMENT_TYPE mType, int nLevel, MDR_ELEMENT_FORMAT** pFormat) {
	RENDER_VIEW_PART* pPart = (pExisting && pExisting->rvp_data.rvp_flow.tf_text == 0) ? pExisting : ll_append(pFirst, sizeof * pPart);
	if (mType == MET_HORIZONTAL_RULE) {
		pPart->rvp_paint = mdr_renderHorizontalRule;
	} else {
		pPart->rvp_paint = mType == MET_TABLE ? mdr_renderTable : mdr_renderMarkdownBlockPart;
	}
	pPart->rvp_type = MET_NORMAL;
	*pFormat = mdr_getFormatFor(mType, nLevel);
	mdr_applyFormat(pPart, *pFormat);
	if (pFSD->fsd_strokeColor != NO_COLOR || pFSD->fsd_fillColor != NO_COLOR) {
		RENDER_BOX_DECORATION* pDecoration = calloc(1, sizeof * pDecoration);
		pDecoration->rbd_fillColor = pFSD->fsd_fillColor;
		pDecoration->rbd_strokeColor = pFSD->fsd_strokeColor;
		pDecoration->rbd_strokeWidth = pFSD->fsd_strokeWidth;
		pPart->rvp_decoration = pDecoration;
	}
	mdr_resetFontStyleDelta(pFSD);
	pPart->rvp_type = mType;
	if (mType == MET_TABLE) {
		pPart->rvp_data.rvp_table = calloc(1, sizeof(RENDER_TABLE));
	}
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
static void mdr_ensureParagraph(RENDER_VIEW_PART** pFirst, RENDER_VIEW_PART** pPart, TEXT_FLOW** pFlow, MDR_ELEMENT_FORMAT** pFormat, FONT_STYLE_DELTA* pFSD,
		LINE* lpRun) {
	if (!*pPart) {
		*pPart = mdr_newPart(pFirst, 0, pFSD, MET_NORMAL, 0, pFormat);
		(*pPart)->rvp_lpStart = lpRun;
		*pFlow = &(*pPart)->rvp_data.rvp_flow;
	}
}

static void mdr_calculateTableColumns(RENDER_TABLE* pTable) {
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
static void mdr_endPart(RENDER_VIEW_PART** pFirst, RENDER_VIEW_PART** pCurrent, TEXT_FLOW** pFlow) {
	if (*pCurrent && (*pCurrent)->rvp_type != MET_TABLE && (*pCurrent)->rvp_data.rvp_flow.tf_text == 0 && (*pCurrent)->rvp_decoration == 0) {
		return;
	}
	*pCurrent = 0;
	*pFlow = 0;
}

/*
 * Parse a text and convert it into a list of view parts to be rendered later.
 * Note, that the viewparts must be destroyed, when they are not needed any more
 * using mdr_destroyViewParts.
 */
RENDER_VIEW_PART* mdr_parseHTML(INPUT_STREAM* pStream) {
	MDR_ELEMENT_FORMAT* pFormat;
	RENDER_VIEW_PART* pFirst = 0;
	FONT_STYLE_DELTA fsdTable[20];
	FONT_STYLE_DELTA* pfsd = fsdTable;
	STRING_BUF* pSB = stringbuf_create(256);
	RENDER_VIEW_PART* pPart = 0;
	RENDER_TABLE* pTable = 0;
	RENDER_TABLE_ROW* pRow = 0;
	RENDER_TABLE_CELL* pCell = 0;
	TEXT_FLOW* pFlow = 0;
	size_t nLastOffset = 0;
	char c;
	size_t nSize;
	LINE* lpRun;
	int nRunOffset;
	MDR_ELEMENT_TYPE mListType = MET_UNORDERED_LIST;
	int nOlListLevel = 0;
	int nUlListLevel = 0;

	nRunOffset = pStream->is_inputMark(pStream, &lpRun);
	mdr_resetFontStyleDelta(pfsd);
	while ((c = pStream->is_getc(pStream)) != 0) {
		if (c == '&') {
			if (mdr_parseEntity(pSB, pStream)) {
				continue;
			}
		} else if (c == '<') {
			if (mdr_skipHTMLCommentOrDoctype(pStream)) {
				continue;
			}
			BOOL bClose;
			struct tagHTML_TAG_MAPPING* pMapping;
			FONT_STYLE_DELTA fsdNext = *pfsd;
			BOOL bMatch = mdr_getTag(pStream, &fsdNext, &pMapping, &bClose);
			if (bMatch) {
				if (strcmp("ol", pMapping->tm_element) == 0) {
					mListType = MET_ORDERED_LIST;
					nOlListLevel = bClose ? nOlListLevel - 1 : nOlListLevel + 1;
					continue;
				} else if (strcmp("ul", pMapping->tm_element) == 0) {
					mListType = MET_UNORDERED_LIST;
					nUlListLevel = bClose ? nUlListLevel - 1 : nUlListLevel + 1;
					continue;
				} else if (pTable && strcmp("tr", pMapping->tm_element) == 0) {
					pRow = bClose ? 0 : (RENDER_TABLE_ROW*)ll_append((LINKED_LIST**)&pTable->rt_rows, sizeof *pRow);
				} else if (pRow && (strcmp("td", pMapping->tm_element) == 0 || strcmp("th", pMapping->tm_element) == 0)) {
					if (bClose) {
						if (pFlow) {
							nSize = stringbuf_size(pSB) - nLastOffset;
							pFlow->tf_text = _strdup(stringbuf_getString(pSB));
							mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, pfsd, lpRun, nRunOffset);
						}
						stringbuf_reset(pSB);
						nLastOffset = 0;
						pCell = 0;
					} else {
						pCell = (RENDER_TABLE_CELL*)ll_append((LINKED_LIST**)&pRow->rtr_cells, sizeof * pCell);
						pCell->rtc_isHeader = pMapping->tm_element[1] == 'h';
						pFlow = &pCell->rtc_flow;
					}
				}
				nSize = stringbuf_size(pSB) - nLastOffset;
				MDR_ELEMENT_TYPE mType = pMapping->tm_blockElement;
				if (mType != MET_NONE && mdr_supportsNestedTag(pPart, mType)) {
					*pfsd = fsdNext;
					int nLevel = pMapping->tm_elementLevel;
					if (stringbuf_size(pSB)) {
						mdr_ensureParagraph(&pFirst, &pPart, &pFlow, &pFormat, pfsd, lpRun);
						if (nSize) {
							mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, pfsd, lpRun, nRunOffset);
							nRunOffset = pStream->is_inputMark(pStream, &lpRun);
							nLastOffset += nSize;
						}
						pFlow->tf_text = _strdup(stringbuf_getString(pSB));
						stringbuf_reset(pSB);
						nLastOffset = 0;
					}
					if (bClose) {
						if (mType == MET_TABLE && pTable) {
							mdr_calculateTableColumns(pTable);
							pTable = 0;
						}
						mType = MET_NORMAL;
					} else if (strcmp("li", pMapping->tm_element) == 0) {
						mType = mListType;
						nLevel = mType == MET_UNORDERED_LIST ? nUlListLevel : nOlListLevel;
					} 
					if (bClose) {
						pfsd = fsdTable;
						mdr_endPart(&pFirst, &pPart, &pFlow);
					} else {
						mdr_endPart(&pFirst, &pPart, &pFlow);
						pPart = mdr_newPart(&pFirst, pPart, pfsd, mType, nLevel, &pFormat);
						pPart->rvp_lpStart = lpRun;
						pFlow = &pPart->rvp_data.rvp_flow;
						if (mType == MET_TABLE) {
							pTable = pPart->rvp_data.rvp_table;
						}
						if (!bClose && pMapping->tm_blockElement == MET_FENCED_CODE_BLOCK && strcmp("pre", pMapping->tm_element) == 0) {
							pStream->is_positionToLineStart(pStream, 1);
							mdr_parsePreformattedCodeBlock(pPart, pStream, pSB, FALSE, TRUE);
							nSize = stringbuf_size(pSB);
							pFlow->tf_text = _strdup(stringbuf_getString(pSB));
							stringbuf_reset(pSB);
							mdr_endPart(&pFirst, &pPart, &pFlow);
						}
					}
					nRunOffset = pStream->is_inputMark(pStream, &lpRun);
					nLastOffset = 0;
					mdr_skipLeadingSpace(pStream);
					mdr_resetFontStyleDelta(&fsdNext);
					continue;
				}
				BOOL bBreak = bClose ? (pMapping->tm_textAttrClose & ATTR_LINE_BREAK) : (pMapping->tm_textAttr & ATTR_LINE_BREAK);
				if (bBreak) {
					pfsd->fsd_logicalStyles |= ATTR_LINE_BREAK;
					mdr_skipLeadingSpace(pStream);
				}
				if (nSize) {
					mdr_ensureParagraph(&pFirst, &pPart, &pFlow, &pFormat, pfsd, lpRun);
					mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, pfsd, lpRun, nRunOffset);
					nRunOffset = pStream->is_inputMark(pStream, &lpRun);
				}
				nLastOffset += nSize;
				if (bClose && pfsd > fsdTable) {
					pfsd--;
				}
				if (!bBreak && !bClose) {
					if (pfsd < &fsdTable[DIM(fsdTable)]) {
						fsdNext.fsd_logicalStyles |= pMapping->tm_textAttr;
						*++pfsd = fsdNext;
					}
				}
			}
			continue;
		} else if (c == '\n' && pStream->is_peekc(pStream, 0) == '\n') {
			// treat empty line as real line break.
			nSize = stringbuf_size(pSB) - nLastOffset;
			pfsd->fsd_logicalStyles |= ATTR_LINE_BREAK;
			if (nSize) {
				mdr_ensureParagraph(&pFirst, &pPart, &pFlow, &pFormat, pfsd, lpRun);
				mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, pfsd, lpRun, nRunOffset);
			}
			nRunOffset = pStream->is_inputMark(pStream, &lpRun);
			nLastOffset += nSize;
			pStream->is_getc(pStream);
			continue;
		}
		if (c != '\n') {
			stringbuf_appendChar(pSB, c);
			if (c == ' ') {
				mdr_skipLeadingSpace(pStream);
			}
		}
	}
	nSize = stringbuf_size(pSB) - nLastOffset;
	if (nSize) {
		mdr_ensureParagraph(&pFirst, &pPart, &pFlow, &pFormat, pfsd, lpRun);
		if (nSize) {
			mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, pfsd, lpRun, nRunOffset);
		}
		pFlow->tf_text = _strdup(stringbuf_getString(pSB));
	}
	stringbuf_destroy(pSB);
	return pFirst;
}

/*
 * Parses an HTML file and create render view parts for wysiwyg rendering.
 */
static void mdr_parseFileToHTML(WINFO* wp) {
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	RENDER_VIEW_PART* pPart = mdr_parseHTML(pStream);
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	pData->md_pElements = pPart;
	pStream->is_destroy(pStream);
}

/*
 * Parse a top-level element to be rendered with a view part possibly containing nested formatting. 
 */
static void mdr_parseFlow(char* pszRelative, INPUT_STREAM* pStream, RENDER_VIEW_PART* pPart, 
	TEXT_FLOW *pFlow, STRING_BUF* pSB, int nInitialAttrs) {
	stringbuf_reset(pSB);
	int nState = 0;
	int nLevel = 0;
	int nLineOffset = 0;
	int nRunLineOffset = 0;
	FONT_STYLE_DELTA fsd;
	FONT_STYLE_DELTA fsdNext;
	LINE* lpRun;
	size_t nLastOffset = 0;
	size_t nSize;
	MDR_ELEMENT_FORMAT* pFormat = NULL;

	fsd.fsd_logicalStyles = nInitialAttrs;
	mdr_resetFontStyleDelta(&fsd);
	MDR_ELEMENT_TYPE mType = MET_NORMAL;
	BOOL bEnforceBreak = TRUE;
	while (pStream->is_peekc(pStream, 0)) {
		BOOL bSkipped = FALSE;
		char lastC;
		char cNext;
		char c = 0;
		nLineOffset = 0;
		nRunLineOffset = pStream->is_inputMark(pStream, &lpRun);
		for (; (cNext = pStream->is_peekc(pStream, 0)) != 0 && c != '\n'; pStream->is_skip(pStream, 1)) {
			lastC = c;
			c = cNext;
			if (nLineOffset == 0 && pPart != NULL && mdr_isIndentedFencedBlock(pStream, mType)) {
				mType = MET_FENCED_CODE_BLOCK;
				pFormat = &_formatFenced;
				mdr_parsePreformattedCodeBlock(pPart, pStream, pSB, TRUE, FALSE);
				nLastOffset = stringbuf_size(pSB);
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
				if (pPart == NULL) {
					mType = MET_NORMAL;
					nState = 1;
					pFormat = &_formatText;
				} else {
					long nNumber;
					mType = mdr_determineTopLevelElement(pStream, &nLevel, &pFormat, &nNumber);
					nRunLineOffset = pStream->is_inputMark(pStream, &lpRun);
					if (mType == MET_HORIZONTAL_RULE) {
						pPart->rvp_paint = mdr_renderHorizontalRule;
						pPart->rvp_type = MET_HORIZONTAL_RULE;
						return;
					}
					if (mType == MET_ORDERED_LIST || mType == MET_TASK_LIST) {
						pPart->rvp_number = nNumber;
					}
					pPart->rvp_level = nLevel;
					if (mType == MET_FENCED_CODE_BLOCK) {
						mdr_parsePreformattedCodeBlock(pPart, pStream, pSB, FALSE, FALSE);
						nLastOffset = stringbuf_size(pSB);
						goto outer;
					}
					if (mType != MET_NORMAL) {
						nRunLineOffset++;
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
			if (c == '\\' && strchr(_escapedChars, pStream->is_peekc(pStream, 1)) != NULL) {
				pStream->is_skip(pStream, 1);
				c = pStream->is_peekc(pStream, 0);
			} else if (!(fsd.fsd_logicalStyles & ATTR_CODE) && c == '=' && pStream->is_peekc(pStream, 1) == c) {
				pStream->is_skip(pStream, 1);
				nSize = stringbuf_size(pSB) - nLastOffset;
				mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, &fsd, lpRun, nRunLineOffset);
				nRunLineOffset = pStream->is_inputMark(pStream, &lpRun);
				fsd.fsd_logicalStyles ^= ATTR_HIGHLIGHT;
				nLastOffset += nSize;
				continue;
			} else if (c == '`' || (!(fsd.fsd_logicalStyles & ATTR_CODE) && (c == '*' ||
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
text_attrs_changed:
					nSize = stringbuf_size(pSB) - nLastOffset;
					mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, &fsd, lpRun, nRunLineOffset);
					fsd = fsdNext;
					int styles = fsd.fsd_logicalStyles ^ nToggle;
					mdr_resetFontStyleDelta(&fsd);
					fsd.fsd_logicalStyles = styles;
					nRunLineOffset = pStream->is_inputMark(pStream, &lpRun)+1;
					nLastOffset += nSize;
					continue;
				}
			} else if (c == '[' || (c == '!' && pStream->is_peekc(pStream, 1) == '[')) {
				BOOL bImage = c == '!';
				pStream->is_skip(pStream, bImage ? 2 : 1);
				char* pLink;
				char* pTitle = NULL;
				int nWidth = 0;
				int nHeight = 0;
				char szLinkText[256];
				if (mdr_parseLink(pszRelative, pStream, szLinkText, &pLink, &pTitle, &nWidth, &nHeight)) {
					int nAttr = 0;
					nSize = stringbuf_size(pSB) - nLastOffset;
					mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, &fsd, lpRun, nRunLineOffset);
					nRunLineOffset = pStream->is_inputMark(pStream, &lpRun);
					nLastOffset += nSize;
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
					stringbuf_appendStringLength(pSB, &szLinkText[nTextStart], nTextEnd - nTextStart);
					nSize = stringbuf_size(pSB) - nLastOffset;
					fsd.fsd_logicalStyles |= ATTR_LINK;
					TEXT_RUN* pRun = mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, &fsd, lpRun, nTextStart);
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
					pStream->is_skip(pStream, -1);
					nRunLineOffset = pStream->is_inputMark(pStream, &lpRun);
					continue;
				}
			} else if (c == '&') {
				STREAM_OFFSET sp = pStream->is_tell(pStream);
				pStream->is_skip(pStream, 1);
				if (mdr_parseEntity(pSB, pStream)) {
					continue;
				}
				pStream->is_seek(pStream, sp);
			} else if (!(fsd.fsd_logicalStyles & ATTR_CODE) && c == '<') {
				BOOL bClose;
				struct tagHTML_TAG_MAPPING* pMapping;
				STREAM_OFFSET offset = pStream->is_tell(pStream);
				int bSucces = mdr_getTag(pStream, &fsdNext, &pMapping, &bClose);
				if (bSucces) {
					pStream->is_skip(pStream, -1);
					nToggle = pMapping->tm_textAttr;
					if (nToggle & ATTR_LINE_BREAK) {
						nSize = stringbuf_size(pSB) - nLastOffset;
						fsd.fsd_logicalStyles |= ATTR_LINE_BREAK;
						mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, &fsd, lpRun, nRunLineOffset);
						bEnforceBreak = TRUE;
						nLastOffset += nSize;
					} else if (((fsd.fsd_logicalStyles & nToggle) != 0) == bClose || ((fsdNext.fsd_styleName != 0) != (fsd.fsd_styleName != 0))) {
						goto text_attrs_changed;
					}
					mdr_skipSpace(pStream, 0);
					nRunLineOffset = pStream->is_inputMark(pStream, &lpRun);
					continue;
				}
				pStream->is_seek(pStream, offset);
				if (mdr_parseAutolinks(pFlow, pStream, pSB, pFormat, &fsd, nLastOffset)) {
					nLastOffset = stringbuf_size(pSB);
					continue;
				}
			}
			if (c != '\n') {
				stringbuf_appendChar(pSB, c);
			}
		}
		if (c == ' ' && lastC == ' ') {
			bEnforceBreak = TRUE;
		} else {
			stringbuf_appendChar(pSB, ' ');
		}
		nSize = stringbuf_size(pSB) - nLastOffset;
		if (bEnforceBreak) {
			fsd.fsd_logicalStyles |= ATTR_LINE_BREAK;
		}
		mdr_appendRun(&pFlow->tf_runs, pFormat, nSize, &fsd, lpRun, nRunLineOffset);
		nLastOffset += nSize;
		if (mdr_isTopLevelOrBreak(pStream, mType, nLevel)) {
			break;
		}
		nState = 1;
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
	mdr_appendRun(&pFlow->tf_runs, pFormat ? pFormat : &_formatText, nSize, &fsd, lpRun, nRunLineOffset);
}

static RENDER_TABLE_CELL *mdr_parseTableCell(char* pszRelative, RENDER_TABLE_ROW* pRow, STRING_BUF* pContents, 
		LINE*lp, int nLineOffset, BOOL bHeader) {
	RENDER_TABLE_CELL* pCell = ll_append(&pRow->rtr_cells, sizeof * pCell);
	pCell->rtc_isHeader = bHeader;
	STRING_BUF* sb = stringbuf_create(128);
	char* pszBuf = stringbuf_getString(pContents);
	char* pszEnd = pszBuf + strlen(pszBuf)-1;
	while (pszEnd > pszBuf && pszEnd[-1] == ' ') {
		*--pszEnd = 0;
	}
	INPUT_STREAM* pStream = streams_createStringInputStream(pszBuf);
	mdr_parseFlow(pszRelative, pStream, NULL, &pCell->rtc_flow, sb, bHeader ? ATTR_STRONG : 0);
	// Hack: this is the attempt to get selections in table cell working
	// If the table cell contains formatted contents - this won't work correctly.
	TEXT_RUN* pRun = pCell->rtc_flow.tf_runs;
	while (pRun) {
		pRun->tr_lp = lp;
		pRun->tr_lineOffset = nLineOffset;
		pRun = pRun->tr_next;
	}
	pStream->is_destroy(pStream);
	stringbuf_destroy(sb);
	return pCell;
}

/*
 * Parse a table row.
 */
static BOOL mdr_parseTableRow(char* pszRelative, INPUT_STREAM* pStream, RENDER_TABLE* pTable, BOOL bHeader, const byte* columnAlignments, int nMaxColumns) {
	long lnOffset;
	int nColumn = 0;
	int nStart = 0;
	RENDER_TABLE_ROW row;
	char c;
	STRING_BUF* pBuf = stringbuf_create(128);
	LINE* lp;

	memset(&row, 0, sizeof row);
	lnOffset = pStream->is_inputMark(pStream, &lp);
	while ((c = pStream->is_getc(pStream)) != 0 && c != '\n' && nColumn < nMaxColumns) {
		if (c == '|') {
			mdr_skipLeadingSpace(pStream);
			if (nStart > 0) {
				RENDER_TABLE_CELL* pCell = mdr_parseTableCell(pszRelative, &row, pBuf, lp, lnOffset, bHeader);
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
		return FALSE;
	}
	if (nColumn < nMaxColumns && (c == '|' || stringbuf_size(pBuf) > 0)) {
		RENDER_TABLE_CELL* pCell = mdr_parseTableCell(pszRelative, &row, pBuf, lp, lnOffset, bHeader);
		if (pCell) {
			pCell->rtc_align = columnAlignments[nColumn++];
		}
	}
	stringbuf_destroy(pBuf);
	RENDER_TABLE_ROW* pAppended = (RENDER_TABLE_ROW*)ll_append((LINKED_LIST**)&pTable->rt_rows, sizeof row);
	memcpy(pAppended, &row, sizeof row);
	return TRUE;
}

/*
 * Try to interpret some lines as a markdown table. If successful update the line pointer to point beyond the
 * table and return TRUE. If no successful, return FALSE;
 */
static BOOL mdr_parseTable(char* pszRelative, INPUT_STREAM* pStream, RENDER_VIEW_PART* pPart) {
	byte columnAlignments[MAX_TABLE_COLUMNS];
	memset(columnAlignments, 0, sizeof columnAlignments);
	BOOL bStartColon = FALSE;
	BOOL bDashSeen = FALSE;
	int nColumn = -1;
	int nAlign = RTC_ALIGN_LEFT;
	char c;
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
	RENDER_TABLE* pTable = calloc(1, sizeof * pTable);
	pTable->rt_columnCount = nColumn;
	pStream->is_seek(pStream, offsetCurrent);
	if (!mdr_parseTableRow(pszRelative, pStream, pTable, TRUE, columnAlignments, nColumn)) {
		free(pTable);
		return FALSE;
	}
	pStream->is_seek(pStream, offsetCurrent);
	pStream->is_positionToLineStart(pStream, 2);
	while (mdr_parseTableRow(pszRelative, pStream, pTable, FALSE, columnAlignments, nColumn)) {
	}
	mdr_calculateTableColumns(pTable);
	pPart->rvp_data.rvp_table = pTable;
	return TRUE;
}

static void mdr_parseMarkdownFormat(WINFO *wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	STRING_BUF * pSB = stringbuf_create(256);
	RENDER_VIEW_PART* pReuse = NULL;
	int nDelta = 0;
	char c;

	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		if (c != '\n') {
			RENDER_VIEW_PART* pPart;
			pPart = pReuse == NULL ? ll_append(&pData->md_pElements, sizeof(RENDER_VIEW_PART)) : pReuse;
			if (!pPart->rvp_paint) {
				pPart->rvp_paint = mdr_renderMarkdownBlockPart;
			}
			pReuse = NULL;
			pStream->is_inputMark(pStream, &pPart->rvp_lpStart);
			if (mdr_parseTable(fp->fname, pStream, pPart)) {
				pPart->rvp_type = MET_TABLE;
				mdr_applyFormat(pPart, &_formatTable);
				pPart->rvp_paint = mdr_renderTable;
			} else {
				mdr_parseFlow(fp->fname, pStream, pPart, &pPart->rvp_data.rvp_flow, pSB, 0);
			}
			if (pPart->rvp_type == MET_BLOCK_QUOTE && pPart->rvp_data.rvp_flow.tf_text == NULL) {
				pReuse = pPart;
			} else {
				pPart->rvp_margins.m_top += nDelta;
				nDelta = 0;
			}
		} else {
			pStream->is_skip(pStream, 1);
			nDelta += PARAGRAPH_OFFSET/2;
		}
	}
	stringbuf_destroy(pSB);
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
		while (pNext && lpPart == pNext->rvp_lpStart) {
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

static void mdr_navigateAnchor(WINFO* wp, const char* pszAnchor) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return;
	}
	RENDER_VIEW_PART* pPart = pData->md_pElements;
	while (pPart) {
		if (pPart->rvp_type == MET_HEADER) {
			if (mdr_titleMatchesAnchor(&pPart->rvp_data.rvp_flow, 0, pszAnchor)) {
				long nCol = 0;
				long nLine = ln_indexOf(wp->fp, pPart->rvp_lpStart);
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
	.r_context = "markdown-renderer",
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
	_htmlRenderer.r_context = "html-renderer";
	return &_htmlRenderer;
}

