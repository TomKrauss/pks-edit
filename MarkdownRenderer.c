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

#define NO_COLOR			-1		// marker for no color defined
#define PARAGRAPH_OFFSET	15		// offset in pixels between paragraph type elements.

static const char _escapedChars[] = "\\`*_{}[]<>()#+-.!|";

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
	int		lineBreak;
	COLORREF bgColor;
	COLORREF fgColor;
	int		fixedFont;
} FONT_ATTRIBUTES;

typedef struct tagTEXT_RUN {
	struct tagTEXT_RUN* tr_next;
	BOOL tr_image;
	char* tr_link;
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
	MDR_ELEMENT_TYPE rvp_type;
	long rvp_number;			// for numbered lists
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
	int mef_fixedFont;
} MDR_ELEMENT_FORMAT;

#define ATTR_EMPHASIS	0x1
#define ATTR_STRONG		0x2
#define ATTR_STRIKE		0x4
#define ATTR_CODE		0x8
#define ATTR_LINK		0x10
#define ATTR_LINE_BREAK 0x20

static MDR_ELEMENT_FORMAT _formatText = {
	0, PARAGRAPH_OFFSET, 10, 20, 14, FW_NORMAL
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
			for (int i = 0; i < pPart->rvp_level; i++) {
				FillRect(hdc, &leftRect, theme_getDialogLightBackgroundBrush());
				leftRect.left += 10;
				leftRect.right += 10;
			}
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
	if (nSize == 0 && pPart->rvp_e.flow.tf_runs) {
		return NULL;
	}
	TEXT_RUN* pRun = ll_append(&pPart->rvp_e.flow.tf_runs, sizeof(TEXT_RUN));
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
			szBuf[i] = 0;
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
					for (int j = nTextStart; j < nTextEnd; j++) {
						stringbuf_appendChar(pSB, lp->lbuf[j]);
					}
					nSize = stringbuf_size(pSB) - nLastOffset;
					TEXT_RUN* pRun = mdr_appendRun(pPart, pFormat, nSize, nAttr|ATTR_LINK);
					pRun->tr_image = bImage;
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
	MDR_ELEMENT_TYPE mType = MET_NORMAL;
	for (; pPart && rect.top < pClip->bottom; rect.top = occupiedBounds.bottom) {
		nElements++;
		MDR_ELEMENT_TYPE mNextType = pPart->rvp_type;
		if (mNextType != mType) {
			if (mType == MET_BLOCK_QUOTE || mType == MET_ORDERED_LIST || mType == MET_UNORDERED_LIST) {
				rect.top += PARAGRAPH_OFFSET;
			}
			mType = mNextType;
		}
		pPart->rvp_paint(wp, pPart, pCtx->rc_hdc, &rect, &occupiedBounds);
		pPart = pPart->rvp_next;
	}
	pData->md_nElementsPerPage = nElements;
}

/*
 * Destroy a text run possibly release the link. 
 */
static int mdr_destroyRun(TEXT_RUN* pRun) {
	free(pRun->tr_link);
	return 1;
}

/*
 * Destroy one view part. 
 */
static int mdr_destroyViewPart(RENDER_VIEW_PART *pRVP) {
	free(pRVP->rvp_e.flow.tf_text);
	ll_destroy(&pRVP->rvp_e.flow.tf_runs, mdr_destroyRun);
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


