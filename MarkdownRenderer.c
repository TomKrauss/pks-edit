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
#include <stdio.h>

#include "winfo.h"
#include "caretmovement.h"
#include "linkedlist.h"
#include "editorfont.h"
#include "stringutil.h"
#include "themes.h"
#include "pathname.h"
#include "winutil.h"
#include "edfuncs.h"
#include "mainframe.h"

#define NO_COLOR			-1		// marker for no color defined
#define PARAGRAPH_OFFSET	15		// offset in pixels between paragraph type elements.
#define DEFAULT_LEFT_MARGIN	10		// offset of most elements to the left of the screen.

static const char _escapedChars[] = "\\`*_{}[]<>()#+-.!|";

typedef struct tagRENDER_VIEW_PART RENDER_VIEW_PART;

typedef void (*RENDER_PAINT)(WINFO* wp, RENDER_VIEW_PART* pPart, HDC hdc, RECT* pBounds, RECT* pUsed);

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
	int		left;
	int		top;
	int		right;
	int		bottom;
	int		right2;
	int		bottom2;
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
	COLORREF bgColor;
	COLORREF fgColor;
	int		fixedFont;
} FONT_ATTRIBUTES;

typedef struct tagTEXT_RUN {
	struct tagTEXT_RUN* tr_next;
	BOOL				tr_isImage;
	HBITMAP				tr_image;
	char*				tr_link;
	size_t				tr_size;
	RUN_BOUNDS			tr_bounds;
	FONT_ATTRIBUTES		tr_attributes;
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
	MET_FENCED_CODE_BLOCK,
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
	RECT rvp_bounds;			// Bounds of an element excluding the margins.
	MDR_ELEMENT_TYPE rvp_type;
	long rvp_number;			// for numbered lists
	int	rvp_level;				// for headers and lists - the level.
	TEXT_FLOW rvp_flow;
};

typedef struct tagMDR_ELEMENT_FORMAT {
	int	mef_marginTop;
	int	mef_marginBottom;
	int	mef_marginLeft;
	int	mef_marginRight;
	int mef_charHeight;
	int mef_charWeight;
	int mef_fixedFont;
} MDR_ELEMENT_FORMAT;

#define ATTR_EMPHASIS	0x1
#define ATTR_STRONG		0x2
#define ATTR_STRIKE		0x4
#define ATTR_CODE		0x8
#define ATTR_LINK		0x10
#define ATTR_LINE_BREAK 0x20

static MDR_ELEMENT_FORMAT _formatText = {
	0, PARAGRAPH_OFFSET, DEFAULT_LEFT_MARGIN, 20, 14, FW_NORMAL
};

static MDR_ELEMENT_FORMAT _formatFenced = {
	PARAGRAPH_OFFSET, PARAGRAPH_OFFSET, 20, 20, 14, FW_BOLD, 1
};

static MDR_ELEMENT_FORMAT _formatBlockQuote = {
	0, 0, 45, 20, 14, FW_NORMAL
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
	20, 14, DEFAULT_LEFT_MARGIN, 20, 28, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH2 = {
	16, 10, DEFAULT_LEFT_MARGIN, 20, 24, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH3 = {
	10, 5, DEFAULT_LEFT_MARGIN, 20, 20, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH4 = {
	10, 3, DEFAULT_LEFT_MARGIN, 20, 18, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH5 = {
	10, 3, DEFAULT_LEFT_MARGIN, 20, 16, FW_BOLD
};

static MDR_ELEMENT_FORMAT _formatH6 = {
	10, 3, DEFAULT_LEFT_MARGIN, 20, 14, FW_BOLD
};

typedef struct tagMARKDOWN_RENDERER_DATA {
	RENDER_VIEW_PART* md_pElements;	// The render view parts to be rendered
	RECT md_lastBounds;
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
	memcpy(&pPart->rvp_bounds, pUsed, sizeof * pUsed);
}

/*
 * Paint an image displayed in a markdown text.
 */
static void mdr_paintImage(WINFO* wp, HDC hdc, TEXT_RUN* pTR, int x, int y, SIZE* pSize) {
	char szFilename[EDMAXPATHLEN];
	if (!pTR->tr_image) {
		FTABLE* fp = wp->fp;
		string_splitFilename(fp->fname, szFilename, NULL);
		string_concatPathAndFilename(szFilename, szFilename, pTR->tr_link);
		pTR->tr_image = LoadImage(NULL, szFilename, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADFROMFILE);
	}
	if (pTR->tr_image) {
		BITMAP          bitmap;
		HGDIOBJ         oldBitmap;
		HDC             hdcMem;
		hdcMem = CreateCompatibleDC(hdc);
		oldBitmap = SelectObject(hdcMem, pTR->tr_image);
		GetObject(pTR->tr_image, sizeof(bitmap), &bitmap);
		BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);
		pSize->cx = bitmap.bmWidth;
		pSize->cy = bitmap.bmHeight;
	} else {
		sprintf(szFilename, "Cannot load image %s", pTR->tr_link);
		TextOut(hdc, x, y, szFilename, (int)strlen(szFilename));
		pSize->cy = 20;
		pSize->cx = 200;
	}
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
	TEXT_FLOW* pTF = &pPart->rvp_flow;
	TEXT_RUN* pTR = pTF->tf_runs;
	int nOffs = 0;
	size_t nLen;
	nLen = pTR->tr_size;
	int nHeight = 0;
	font_setDefaultTextColors(hdc, pTheme);
	COLORREF cDefault = GetTextColor(hdc);
	HFONT hFont = mdr_createFont(&pTR->tr_attributes, wp->zoomFactor);
	HFONT hOldFont = SelectObject(hdc, hFont);
	if (pPart->rvp_type == MET_UNORDERED_LIST) {
		TextOutW(hdc, x-15, y, pPart->rvp_level == 1 ? L"\u25CF" : (pPart->rvp_level == 2 ? L"\u25CB" : L"\u25A0"), 1);
	} else if (pPart->rvp_type == MET_ORDERED_LIST) {
		char szBuf[20];
		sprintf(szBuf, "%ld.", pPart->rvp_number);
		TextOut(hdc, x - 20, y, szBuf, (int)strlen(szBuf));
	} else if (pPart->rvp_type == MET_FENCED_CODE_BLOCK) {
		TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);
		RECT r;
		r.top = pBounds->top;
		r.bottom = y + (pPart->rvp_number * tm.tmHeight) + pPart->rvp_marginBottom;
		r.left = x-10;
		r.right = nRight;
		FillRect(hdc, &r, theme_getDialogLightBackgroundBrush());
	}
	while (nLen > 0 && string_isSpace(pTF->tf_text[nOffs])) {
		nOffs++;
		nLen--;
	}
	pPart->rvp_bounds.top = y;
	BOOL bRunBegin = TRUE;
	while(pTR) {
		SIZE size;
		int nFit;
		if (pTR->tr_isImage) {
			mdr_paintImage(wp, hdc, pTR, x, y, &size);
			if (size.cy > nHeight) {
				nHeight = size.cy;
			}
			pPart->rvp_bounds.bottom = y + nHeight;
			pPart->rvp_bounds.left = x;
			pPart->rvp_bounds.right = x+size.cx;
		} else {
			GetTextExtentExPoint(hdc, &pTF->tf_text[nOffs], (int)nLen, nRight - x, &nFit, 0 /* TODO: callback for measuring*/, &size);
			if (size.cy > nHeight) {
				nHeight = size.cy;
			}
			pPart->rvp_bounds.bottom = y + nHeight;
			if (nOffs == 0) {
				pPart->rvp_bounds.left = x;
				pPart->rvp_bounds.right = nRight;
			}
			if (bRunBegin) {
				pTR->tr_bounds.top1 = pTR->tr_bounds.top = y;
				pTR->tr_bounds.bottom = pTR->tr_bounds.bottom2 = y + nHeight;
				pTR->tr_bounds.left = pTR->tr_bounds.left1 = x;
				pTR->tr_bounds.right = pTR->tr_bounds.right2 = x + size.cx;
			} else {
				if (pTR->tr_bounds.top == pTR->tr_bounds.top1) {
					pTR->tr_bounds.top = y;
				}
				if (nFit < nLen) {
					pTR->tr_bounds.bottom = y + nHeight;
				} else {
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
			TextOut(hdc, x, y + nDelta, &pTF->tf_text[nOffs], nFit);
			if (pPart->rvp_type == MET_BLOCK_QUOTE) {
				RECT leftRect;
				leftRect.top = y;
				leftRect.bottom = y + nHeight;
				leftRect.left = pBounds->left + _formatText.mef_marginLeft;
				leftRect.right = leftRect.left + 5;
				for (int i = 0; i < pPart->rvp_level; i++) {
					FillRect(hdc, &leftRect, theme_getDialogLightBackgroundBrush());
					leftRect.left += 10;
					leftRect.right += 10;
				}
			}
		}
		if (!pTR->tr_isImage && nFit < nLen && nFit > 0) {
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
		} else {
			if (pTR->tr_attributes.lineBreak) {
				x = pUsed->left;
				y += nHeight;
			} else {
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
			} else {
				SetBkMode(hdc, OPAQUE);
				SetBkColor(hdc, pTR->tr_attributes.bgColor);
			}
		}
	}
	DeleteObject(SelectObject(hdc, hOldFont));
	pUsed->bottom = y + nHeight + pPart->rvp_marginTop + pPart->rvp_marginBottom;
	if (pPart->rvp_type == MET_HEADER && pPart->rvp_level < 3) {
		mdr_paintRule(hdc, pBounds->left + 10, pBounds->right - 10, pUsed->bottom - 15);
	}
}

static BOOL mdr_isTopLevelOrBreak(LINE* lp, MDR_ELEMENT_TYPE mCurrentType, int nLevel) {
	for (int i = 0; i < lp->len; i++) {
		char c = lp->lbuf[i];
		if (c == '-' || c == '#' || c == '*' || c == '+') {
			return TRUE;
		}
		if (isdigit(c)) {
			string_convertToLong(&lp->lbuf[i]);
			return *_strtolend == '.';
		}
		if (c == '>') {
			if (mCurrentType != MET_BLOCK_QUOTE) {
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
		*pLevel = 1;
		if (i < lp->len - 1 && lp->lbuf[i + 1] == c) {
			i++;
			*pOffset = i;
			*pLevel = 2;
			if (i < lp->len - 1 && lp->lbuf[i + 1] == c) {
				if (c == '-') {
					return MET_HORIZONTAL_RULE;
				}
				*pOffset = i+1;
				*pLevel = 3;
			}
		}
	} else if (c == '>') {
		*pLevel = 1;
		mType = MET_BLOCK_QUOTE;
		while (i < lp->len - 1 && lp->lbuf[i + 1] == '>') {
			(*pLevel)++;
			i++;
		}
	} else if (isdigit(c)) {
		*pNumber = string_convertToLong(&lp->lbuf[i]);
		if (*_strtolend == '.') {
			*pOffset = (int)(_strtolend - lp->lbuf) + 1;
			mType = MET_ORDERED_LIST;
			*pLevel = 1;
		}
	} else if (c == '`' && lp->len >= 3 && lp->lbuf[1] == c && lp->lbuf[2] == c) {
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
	case MET_ORDERED_LIST:
	case MET_UNORDERED_LIST: *pFormat = *pLevel == 1 ? &_formatListLevel1 : (*pLevel == 2 ? &_formatListLevel2 : &_formatListLevel3); break;
	case MET_BLOCK_QUOTE: *pFormat = &_formatBlockQuote; break;
	default: *pFormat = &_formatText; break;
	}
	return mType;
}

static TEXT_RUN* mdr_appendRun(RENDER_VIEW_PART* pPart, MDR_ELEMENT_FORMAT* pFormat, size_t nSize, int mAttrs) {
	if (nSize == 0 && pPart->rvp_flow.tf_runs) {
		return NULL;
	}
	TEXT_RUN* pRun = ll_append(&pPart->rvp_flow.tf_runs, sizeof(TEXT_RUN));
	pRun->tr_size = nSize;
	pRun->tr_attributes.size = pFormat->mef_charHeight;
	pRun->tr_attributes.weight = pFormat->mef_charWeight;
	pRun->tr_attributes.fixedFont = pFormat->mef_fixedFont;
	if (mAttrs & ATTR_CODE) {
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

static char* mdr_parseLink(RENDER_VIEW_PART* pPart, LINE* lp, int *pTextEnd, int* pStartPos) {
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
				} else {
					return NULL;
				}
			}
		} else if (c == ')') {
			char szBuf[256];
			*pStartPos = i-1;
			i -= nLinkStart;
			strncpy(szBuf, &lp->lbuf[nLinkStart], i-1);
			szBuf[i-1] = 0;
			return _strdup(szBuf);
		}
	}
	return NULL;
}

/*
 * Parse a fenced code block and add the runs to the view part.
 */
static LINE* mdr_parseFencedCodeBlock(RENDER_VIEW_PART* pPart, LINE* lp, STRING_BUF* pSB) {
	size_t nLastOffset = 0;

	while (lp) {
		if (lp->len >= 3 && strncmp(lp->lbuf, "```", 3) == 0) {
			return lp->next;
		}
		pPart->rvp_number++;
		stringbuf_appendStringLength(pSB, lp->lbuf, lp->len);
		size_t nSize = stringbuf_size(pSB) - nLastOffset;
		mdr_appendRun(pPart, &_formatFenced, nSize, ATTR_LINE_BREAK);
		nLastOffset += nSize;
		lp = lp->next;
	}
	return lp;
}

/*
 * Parses the auto-link syntax.
 */
static BOOL mdr_parseAutolinks(RENDER_VIEW_PART* pPart, LINE* lp, STRING_BUF* pSB, MDR_ELEMENT_FORMAT* pFormat, int mAttrs, int* pOffset, size_t* pLastOffset) {
	int i = *pOffset;
	int nTextStart = i + 1;
	char* pLast = strchr(&lp->lbuf[nTextStart], '>');
	if (pLast && strchr(&lp->lbuf[i], '.') != 0) {
		if (pLast) {
			size_t nSize = stringbuf_size(pSB) - *pLastOffset;
			mdr_appendRun(pPart, pFormat, nSize, mAttrs);
			(*pLastOffset) += nSize;
			char szTemp[512];
			char szLink[512];
			int nTextEnd = (int)(pLast - lp->lbuf);
			nSize = nTextEnd - nTextStart;
			if (nSize >= sizeof szLink) {
				nSize = sizeof szLink-1;
			}
			strncpy(szTemp, &lp->lbuf[nTextStart], nSize);
			szTemp[nSize] = 0;
			if (strchr(&lp->lbuf[i], '/') == 0) {
				sprintf(szLink, "https://%s", szTemp);
			} else {
				strcpy(szLink, szTemp);
			}
			stringbuf_appendStringLength(pSB, &lp->lbuf[nTextStart], nSize);
			nSize = stringbuf_size(pSB) - *pLastOffset;
			TEXT_RUN* pRun = mdr_appendRun(pPart, pFormat, nSize, ATTR_LINK);
			pRun->tr_link = _strdup(szLink);
			(* pLastOffset) += nSize;
			*pOffset = nTextEnd;
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * Parse a top-level element to be rendered with a view part possibly containing nested formatting. 
 */
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
		char c = 0;
		char lastC = 0;
		for (int i = 0; i < lp->len; i++) {
			lastC = c;
			c = lp->lbuf[i];
			if (nState <= 1 && string_isSpace(c)) {
				continue;
			}
			if (nState == 1 && mType == MET_BLOCK_QUOTE && c == '>' && !bSkipped) {
				bSkipped = TRUE;
				continue;
			}
			if (nState == 0) {
				long nNumber;
				mType = mdr_determineTopLevelElement(lp, &i, &nLevel, &pFormat, &nNumber);
				if (mType == MET_HORIZONTAL_RULE) {
					pPart->rvp_paint = mdr_renderHorizontalRule;
					return lp->next;
				}
				if (mType == MET_ORDERED_LIST) {
					pPart->rvp_number = nNumber;
				}
				pPart->rvp_level = nLevel;
				if (mType == MET_FENCED_CODE_BLOCK) {
					lp = mdr_parseFencedCodeBlock(pPart, lp->next, pSB);
					nLastOffset = stringbuf_size(pSB);
					goto outer;
				}
				if (mType != MET_NORMAL) {
					nState = 1;
					continue;
				}
			}
			if (nState < 2) {
				nState = 2;
			}
			if (c == '\\' && i < lp->len - 1 && strchr(_escapedChars, lp->lbuf[i+1]) != NULL) {
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
					i++;
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
			} else if (c == '[' || (c == '!' && i < lp->len - 1 && lp->lbuf[i + 1] == '[')) {
				BOOL bImage = c == '!';
				int pos = (bImage) ? i + 2 : i+1;
				int nTextStart = pos;
				int nTextEnd;
				char* pLink = mdr_parseLink(pPart, lp, &nTextEnd, &pos);
				if (pLink != NULL) {
					int nAttr = 0;
					nSize = stringbuf_size(pSB) - nLastOffset;
					mdr_appendRun(pPart, pFormat, nSize, mAttrs);
					nLastOffset += nSize;
					char c = lp->lbuf[nTextStart];
					if (c == lp->lbuf[nTextEnd - 1]) {
						if (c == '`') {
							nAttr = ATTR_CODE;
						} else if (c == '*' || c == '_') {
							nAttr = ATTR_EMPHASIS;
						}
						if (nAttr) {
							nTextStart++;
							nTextEnd--;
						}
					} 
					stringbuf_appendStringLength(pSB , &lp->lbuf[nTextStart], nTextEnd - nTextStart);
					nSize = stringbuf_size(pSB) - nLastOffset;
					TEXT_RUN* pRun = mdr_appendRun(pPart, pFormat, nSize, nAttr|ATTR_LINK);
					pRun->tr_isImage = bImage;
					pRun->tr_link = pLink;
					nLastOffset += nSize;
					i = pos;
					continue;
				} 
			} else if (c == '<') {
				if (strncmp(&lp->lbuf[i], "<br>", 4) == 0) {
					i += 3;
					while (i < lp->len - 1 && lp->lbuf[i+1] == ' ') {
						i++;
					}
					nSize = stringbuf_size(pSB) - nLastOffset;
					mdr_appendRun(pPart, pFormat, nSize, mAttrs | ATTR_LINE_BREAK);
					nLastOffset += nSize;
					continue;
				}
				if (mdr_parseAutolinks(pPart, lp, pSB, pFormat, mAttrs, &i, &nLastOffset)) {
					continue;
				}
			}
			stringbuf_appendChar(pSB, c);
		}
		if (c == ' ' && lastC == ' ') {
			nSize = stringbuf_size(pSB) - nLastOffset;
			mdr_appendRun(pPart, pFormat, nSize, mAttrs|ATTR_LINE_BREAK);
			nLastOffset += nSize;
		}
		lp = lp->next;
		if (mdr_isTopLevelOrBreak(lp, mType, nLevel)) {
			break;
		}
		nState = 1;
		stringbuf_appendChar(pSB, ' ');
	}
outer:
	pPart->rvp_type = mType;
	pPart->rvp_marginLeft = pFormat->mef_marginLeft;
	pPart->rvp_marginTop = pFormat->mef_marginTop;
	pPart->rvp_marginRight = pFormat->mef_marginRight;
	pPart->rvp_marginBottom = pFormat->mef_marginBottom;
	pPart->rvp_paint = mdr_renderTextFlow;
	pPart->rvp_flow.tf_text = _strdup(stringbuf_getString(pSB));

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
static RENDER_VIEW_PART* mdr_getViewPartForLine(RENDER_VIEW_PART* pFirstPart, long nLine) {
	// TODO....
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

static int mdr_adjustScrollBounds(WINFO* wp) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	int nDelta = pData->md_nElementsPerPage;
	if (nDelta == 0) {
		return 0;
	}
	long nMaxScreen = wp->mincursln + nDelta;
	size_t nMax = ll_size((LINKED_LIST*)pData->md_pElements);
	if (nMaxScreen > nMax) {
		nMaxScreen = (long)nMax;
	}
	if (wp->caret.ln < nMaxScreen && wp->caret.ln >= wp->mincursln) {
		return 0;
	}
	if (wp->caret.ln < wp->mincursln) {
		wp->minln = wp->mincursln = wp->caret.ln;
	} else {
		if (pData->md_displayingEndOfFile) {
			return 0;
		}
		RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, nMaxScreen);
		int nDelta = wp->caret.ln - nMaxScreen + 1;
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
	wp->maxcurscol = wp->maxcol = 50;
	InvalidateRect(wp->ww_handle, (LPRECT)0, 0);
	UpdateWindow(wp->ww_handle);
	int width;
	mdr_updateCaretUI(wp, &wp->cx, &wp->cy, &width, &wp->cheight);
	render_updateCaret(wp);
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
	if (rect.right != pData->md_lastBounds.right || rect.bottom != pData->md_lastBounds.bottom || wp->minln != pData->md_lastMinLn) {
		bSizeChanged = TRUE;
		pData->md_lastMinLn = wp->minln;
		CopyRect(&pData->md_lastBounds, &rect);
	}
	HBRUSH hBrushCaretLine = CreateSolidBrush(pCtx->rc_theme->th_caretLineColor);
	if (!pData->md_pElements) {
		mdr_parseViewParts(fp, pData);
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, wp->minln);
	RECT occupiedBounds;
	int nElements = 0;
	FillRect(pCtx->rc_hdc, pClip, hBrushBg);
	MDR_ELEMENT_TYPE mType = MET_NORMAL;
	for (; pPart && ((bSizeChanged && rect.top < rect.bottom) || (!bSizeChanged && rect.top < pClip->bottom)); rect.top = occupiedBounds.bottom) {
		MDR_ELEMENT_TYPE mNextType = pPart->rvp_type;
		if (mNextType != mType) {
			if (mType == MET_BLOCK_QUOTE || mType == MET_ORDERED_LIST || mType == MET_UNORDERED_LIST) {
				rect.top += PARAGRAPH_OFFSET;
				if (rect.top > rect.bottom) {
					break;
				}
			}
			mType = mNextType;
		}
		pPart->rvp_paint(wp, pPart, pCtx->rc_hdc, &rect, &occupiedBounds);
		pPart = pPart->rvp_next;
		nElements++;
	}
	if (bSizeChanged) {
		pData->md_nElementsPerPage = nElements-1;
		pData->md_displayingEndOfFile = pPart == NULL;
		mdr_adjustScrollBounds(wp);
	}
}

/*
 * Destroy a text run possibly release the link. 
 */
static int mdr_destroyRun(TEXT_RUN* pRun) {
	free(pRun->tr_link);
	if (pRun->tr_image) {
		DeleteObject(pRun->tr_image);
	}
	return 1;
}

/*
 * Destroy one view part. 
 */
static int mdr_destroyViewPart(RENDER_VIEW_PART *pRVP) {
	free(pRVP->rvp_flow.tf_text);
	ll_destroy(&pRVP->rvp_flow.tf_runs, mdr_destroyRun);
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
		RedrawWindow(wp->ww_handle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
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
}

static int mdr_placeCursorAndValidate(WINFO* wp, long* ln, long offset, long* col, int updateVirtualOffset, int xDelta) {
	if (*ln < 0) {
		return 0;
	}
	long lnMax = mdr_calculateMaxLine(wp);
	if (*ln >= lnMax) {
		*ln = lnMax;
	}
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, *ln);
	if (pPart) {
		wp->caret.linePointer = pPart->rvp_lpStart;
		if (pPart->rvp_flow.tf_runs) {
			wp->caret.col = *col;
		}
	}
	wp->caret.ln = *ln;
	wp->caret.offset = offset;
	return 1;
}

static void mdr_hitTest(WINFO* wp, int cx, int cy, long* pLine, long* pCol) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return;
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, wp->minln);
	POINT pt = {cx, cy};
	long ln = wp->minln;
	while (pPart) {
		if (PtInRect(&pPart->rvp_bounds, pt)) {
			TEXT_RUN* pRun = pPart->rvp_flow.tf_runs;
			long nCol = 0;
			while (pRun) {
				if (runbounds_contains(&pRun->tr_bounds, (POINT) { cx, cy })) {
					*pCol = nCol;
					break;
				}
				pRun = pRun->tr_next;
				nCol++;
			}
			*pLine = ln;
			return;
		}
		pPart = pPart->rvp_next;
		ln++;
	}
}

static BOOL mdr_findLink(WINFO* wp, char* pszBuf, size_t nMaxChars, NAVIGATION_INFO_PARSE_RESULT* pResult) {
	MARKDOWN_RENDERER_DATA* pData = wp->r_data;
	if (!pData) {
		return 0;
	}
	RENDER_VIEW_PART* pPart = mdr_getViewPartForLine(pData->md_pElements, wp->caret.ln);
	if (pPart && pPart->rvp_flow.tf_runs) {
		TEXT_RUN* pRun = (TEXT_RUN*) ll_at((LINKED_LIST*)pPart->rvp_flow.tf_runs, wp->caret.col);
		if (pRun && pRun->tr_link && strlen(pRun->tr_link) < nMaxChars) {
			strcpy(pszBuf, pRun->tr_link);
			pResult->ni_fileName = pszBuf;
			pResult->ni_lineNumber = 0;
			pResult->ni_displayMode = SHOWWYSIWYG;
			pResult->ni_wp = mainframe_getDockName(wp->edwin_handle);
			return 1;
		}
	}
	return FALSE;
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
	mdr_adjustScrollBounds,
	mdr_updateCaretUI,
	mdr_rendererSupportsMode,
	mdr_hitTest,
	FALSE,
	mdr_modelChanged,
	NULL,
	mdr_findLink
};

/*
 * Returns a markdown renderer.
 */
RENDERER* mdr_getRenderer() {
	return &_mdrRenderer;
}


