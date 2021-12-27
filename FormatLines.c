/*
 * FormatLines.c
 *
 * autoformat text depending on a document type.
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 14.03.1991
 */

#include <windows.h>
#include <string.h>

#include "alloc.h"
#include "caretmovement.h"
#include "edierror.h"
#include "textblocks.h"
#include "stringutil.h"
#include "linkedlist.h"
#include "grammar.h"

#include "pksedit.h"
#include "mouseutil.h"
#include "winfo.h"

typedef struct tagFORMATTER FORMATTER;

typedef int (*F_CALCULATE_INDENT)(FORMATTER* pFormatter, WINFO* wp, const char* pBuffer, size_t nLen, int* pScreenCol);
typedef BOOL(*F_STARTS_NEW_PARAGRAPH)(FORMATTER* pFormatter, LINE* lp);
typedef BOOL(*F_TREAT_AS_EMPTY)(FORMATTER* pFormatter, LINE* lp);
typedef BOOL(*F_SHOULD_WRAP)(FORMATTER* pFormatter, const char* pBuf, int nPosition);
typedef void (*F_FORMAT_LINE)(FORMATTER* pFormatter, WINFO* wp, STRING_BUF* sb, FORMATTING_ALIGNMENT nAlignment);
typedef BOOL(*F_SUPPORTS_FORMAT)(FORMATTER* pFormatter);

struct tagFORMATTER {
	const char* f_name;								// a unique name to identify the formatter.
	const F_CALCULATE_INDENT f_calculateIndent;		// a callback for calculating the buffer offset in a line as it should be used for successive lines.
													// The line prior to the line currently formatted is passed to this method
	const F_STARTS_NEW_PARAGRAPH f_startsNewParagraph; // a callback determining, whether the specified line will start a new paragraph
	const F_SHOULD_WRAP f_maySplitAt;				// Returns true, if the formatter algorithm may attempt to wrap text to the next line at the given position. 
													// nPosition will never exceed the length of the line.
	const F_TREAT_AS_EMPTY f_treatAsEmpty;
	const F_FORMAT_LINE f_formatLine;
	const F_SUPPORTS_FORMAT f_supports;				// Whether text formatting is supported
};

/*
 * Wrapping test for ascii type documents
 */
static BOOL format_maySplitTextlineAt(FORMATTER* pFormatter, const char* pBuf, int nPosition) {
	if (nPosition > 0 && pBuf[nPosition - 1] == '-') {
		return TRUE;
	}
	char c = pBuf[nPosition];
	return (c == ' ' || c == '\t');
}

/*
 * Wrapping test for ascii type documents
 */
static BOOL format_treatTextlineEmpty(FORMATTER* pFormatter, LINE* lp) {
	for (int i = 0; i < lp->len; i++) {
		char c = lp->lbuf[i];
		if (c != ' ' && c != '\t') {
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * Calculates the "indenting" of a line in a text type file. Indents are returned in terms of line offset.
 */
static int format_calculateTextIndent(FORMATTER* pFormatter, WINFO* wp, const char* pBuf, size_t nLen, int *pScreenCol) {
	int j = 0;
	for (int i = 0; i < nLen; i++) {
		char c = pBuf[i];
		if (c == '\t' || c == ' ') {
			j = i+1;
			continue;
		}
		if ((c == '-' || c == '*') && i < nLen-1 && pBuf[i+1] == ' ') {
			j = i + 2;
		}
		break;
	}
	*pScreenCol = caret_bufferOffset2screen(wp, pBuf, j);
	return j;
}

/*
 * Calculates the "indenting" of a line in an arbitrary file. 
 */
static int format_calculateIndent(FORMATTER* pFormatter, WINFO* wp, const char* pBuf, size_t nLen, int* pScreenCol) {
	int j = 0;
	for (int i = 0; i < nLen; i++) {
		char c = pBuf[i];
		if (c == '\t' || c == ' ') {
			j = i + 1;
			continue;
		}
		break;
	}
	*pScreenCol = caret_bufferOffset2screen(wp, pBuf, j);
	return j;
}

/*
 * Calculates the "indenting" of a line in a code file. If the line ends with an "opening brace" ({ in C, Java and the like) the indent
 * is increased, if the line ends with a closing brace (} in C, Java and the like), indent is decreased. Indents are returned in terms of line offset.
 */
static int format_calculateCodeIndent(FORMATTER* pFormatter, WINFO* wp, const char* pBuf, size_t nLen, int *pScreenCol) {
	int j = 0;
	for (int i = 0; i < nLen; i++) {
		char c = pBuf[i];
		if (c == '\t' || c == ' ') {
			j = i + 1;
			continue;
		}
		break;
	}
	int nScreen = caret_bufferOffset2screen(wp, pBuf, j);
	FTABLE* fp = wp->fp;
	int nDeltaIndent = grammar_getDeltaIndentation(fp->documentDescriptor->grammar, pBuf, nLen);
	nScreen += wp->indentation.tabsize * nDeltaIndent;
	*pScreenCol = (nScreen < 0) ? 0 : nScreen;
	j += nDeltaIndent;
	return j < 0 ? 0 : j;
}

/*
 * Determines, whether the passed line will start a new paragraph.
 */
static BOOL format_startsParagraphInTextFiles(FORMATTER* pFormatter, LINE* lp) {
	for (int i = 0; i < lp->len; i++) {
		char c = lp->lbuf[i];
		if (c == '\t' || c == ' ') {
			continue;
		}
		if ((c == '-' || c == '*') && i < lp->len - 1 && lp->lbuf[i + 1] == ' ') {
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

/*
 * Determines, whether the passed line will start a new paragraph.
 */
static BOOL format_startsParagraphInCodeFiles(FORMATTER* pFormatter, LINE* lp) {
	return FALSE;
}

static BOOL format_supportsFormatting(FORMATTER* pFormatter) {
	return TRUE;
}

static BOOL format_dontSupportsFormatting(FORMATTER* pFormatter) {
	return FALSE;
}

static void format_codeLine(FORMATTER* pFormatter, WINFO* wp, STRING_BUF* pBuffer, FORMATTING_ALIGNMENT nAlignment) {
	FTABLE* fp = wp->fp;
	if (fp->documentDescriptor->expandTabsWith == 0) {
		long nt;
		int nLen = ft_compressSpacesToTabs(wp, _linebuf, LINEBUFSIZE, stringbuf_getString(pBuffer), stringbuf_size(pBuffer), &nt);
		if (nt) {
			stringbuf_reset(pBuffer);
			stringbuf_appendStringLength(pBuffer, _linebuf, (size_t)nLen);
		}
	}
}


static void format_textLine(FORMATTER* pFormatter, WINFO* wp, STRING_BUF* pBuffer, FORMATTING_ALIGNMENT nAlignment) {
	size_t nSize = stringbuf_size(pBuffer);
	char* pBuf = stringbuf_getString(pBuffer);
	char chSpace = ft_getSpaceFillCharacter(wp);
	while (nSize > 0 && pBuf[nSize - 1] == chSpace) {
		nSize--;
	}
	stringbuf_truncate(pBuffer, nSize);
	int nDelta = (int)(wp->rmargin - nSize);
	int nInsertFront = 0;
	if (nAlignment == FMT_RIGHT) {
		nInsertFront = nDelta;
	} else if (nAlignment == FMT_CENTER) {
		nInsertFront = (int)nDelta / 2;
	} else if (nAlignment == FMT_JUSTIFIED) {
		if (nDelta <= 0) {
			return;
		}
		int nScreenCol;
		int nStart = pFormatter->f_calculateIndent(pFormatter, wp, pBuf, nSize, &nScreenCol);
		while (nDelta > 0) {
			int nOldDelta = nDelta;
			for (int i = nStart; i < nSize; i++) {
				if (pFormatter->f_maySplitAt(pFormatter, pBuf, i)) {
					stringbuf_insertChar(pBuffer, i, chSpace);
					i++;
					if (--nDelta <= 0) {
						return;
					}
				}
			}
			if (nOldDelta == nDelta) {
				break;
			}
		}
		return;
	}
	for (int i = 0; i < nInsertFront; i++) {
		stringbuf_insertChar(pBuffer, 0, chSpace);
	}
	FTABLE* fp = wp->fp;
	if (fp->documentDescriptor->expandTabsWith == 0) {
		long nt;
		int nLen = ft_compressSpacesToTabs(wp, _linebuf, LINEBUFSIZE, stringbuf_getString(pBuffer), stringbuf_size(pBuffer), &nt);
		if (nt) {
			stringbuf_reset(pBuffer);
			stringbuf_appendStringLength(pBuffer, _linebuf, (size_t)nLen);
		}
	}
}
static FORMATTER _textAndMarkupFormatter = {
	.f_name = "text",
	.f_maySplitAt = format_maySplitTextlineAt,
	.f_calculateIndent = format_calculateTextIndent,
	.f_startsNewParagraph = format_startsParagraphInTextFiles,
	.f_treatAsEmpty = format_treatTextlineEmpty,
	.f_formatLine = format_textLine,
	.f_supports = format_supportsFormatting
};

static FORMATTER _codeFormatter = {
	.f_name = "code",
	.f_maySplitAt = format_maySplitTextlineAt,
	.f_calculateIndent = format_calculateCodeIndent,
	.f_startsNewParagraph = format_startsParagraphInCodeFiles,
	.f_treatAsEmpty = format_treatTextlineEmpty,
	.f_formatLine = format_codeLine,
	.f_supports = format_dontSupportsFormatting
};

static FORMATTER _defaultFormatter = {
	.f_name = "default",
	.f_maySplitAt = format_maySplitTextlineAt,
	.f_calculateIndent = format_calculateIndent,
	.f_startsNewParagraph = format_startsParagraphInTextFiles,
	.f_treatAsEmpty = format_treatTextlineEmpty,
	.f_formatLine = format_codeLine,
	.f_supports = format_dontSupportsFormatting
};


static void format_insertLine(FORMATTER* pFormatter, WINFO* wp, LINE** pDest, STRING_BUF* pBuffer, FORMATTING_ALIGNMENT nAlignment) {
	pFormatter->f_formatLine(pFormatter, wp, pBuffer, nAlignment);
	size_t nSize = stringbuf_size(pBuffer);
	LINE* lpNew = ln_create((int)nSize);
	memcpy(lpNew->lbuf, stringbuf_getString(pBuffer), nSize);
	stringbuf_reset(pBuffer);
	ll_add(pDest, (LINKED_LIST*) lpNew);
}

static LINE* ft_formatInto(FORMATTER* pFormatter, WINFO* wp, LINE* lp, LINE* lplast, FORMATTING_ALIGNMENT nAlignment) {
	LINE* lpDest = 0;
	int nCurrentScreenIndent;
	int nLastWrappingPos;
	int nCurrentLineOffset;
	int nSourceWrappingPos;
	STRING_BUF* sb = stringbuf_create(128);
	BOOL bStartOfParagraph = TRUE;

	char chSpace = ft_getSpaceFillCharacter(wp);
	while (lp) {
		if (lp == lplast) {
			break;
		}
		nCurrentLineOffset = 0;
		int nScreenCol;
		int nIndent = pFormatter->f_calculateIndent(pFormatter, wp, lp->lbuf, lp->len, &nScreenCol);
		nLastWrappingPos = 0;
		while (nCurrentLineOffset < lp->len) {
			if (stringbuf_size(sb) == 0) {
				nCurrentScreenIndent = caret_lineOffset2screen(wp, &(CARET) {
					.linePointer = lp,
						.offset = nIndent
				});
				if (bStartOfParagraph) {
					stringbuf_appendStringLength(sb, lp->lbuf, nIndent);
					bStartOfParagraph = FALSE;
					nCurrentLineOffset = nIndent;
				} else {
					for (int nCol = 0; nCol < nCurrentScreenIndent; nCol++) {
						stringbuf_appendChar(sb, chSpace);
					}
					while (nCurrentLineOffset < lp->len) {
						char c = lp->lbuf[nCurrentLineOffset];
						if (c != chSpace && c != '\t') {
							break;
						}
						nCurrentLineOffset++;
					}
				}
				if (nCurrentLineOffset >= lp->len) {
					break;
				}
			}
			size_t nTargetSize = stringbuf_size(sb);
			if (nCurrentLineOffset > 0 && pFormatter->f_maySplitAt(pFormatter, lp->lbuf, nCurrentLineOffset)) {
				nSourceWrappingPos = nCurrentLineOffset;
				nLastWrappingPos = (int)nTargetSize;
			}
			int nScreen = caret_bufferOffset2screen(wp, stringbuf_getString(sb), (int)nTargetSize);
			if (nScreen >= wp->rmargin && nLastWrappingPos > 0) {
				stringbuf_truncate(sb, nLastWrappingPos);
				format_insertLine(pFormatter, wp, &lpDest, sb, nAlignment);
				nCurrentLineOffset = nSourceWrappingPos;
				nLastWrappingPos = 0;
			} else {
				char c = lp->lbuf[nCurrentLineOffset];
				if (c == '\t') {
					c = chSpace;
				}
				char lastC = stringbuf_lastChar(sb);
				if (lastC == '\t') {
					lastC = ' ';
				}
				if (c != chSpace || lastC != chSpace) {
					stringbuf_appendChar(sb, c);
				}
			}
			nCurrentLineOffset++;
		}
		lp = lp->next;
		if (!lp || lp == lplast) {
			break;
		}
		if (pFormatter->f_startsNewParagraph(pFormatter, lp)) {
			size_t nSize = stringbuf_size(sb);
			if (nSize > 0) {
				format_insertLine(pFormatter, wp, &lpDest, sb, nAlignment);
			}
			while (lp && lp != lplast && pFormatter->f_treatAsEmpty(pFormatter, lp)) {
				stringbuf_reset(sb);
				format_insertLine(pFormatter, wp, &lpDest, sb, nAlignment);
				lp = lp->next;
			}
			bStartOfParagraph = TRUE;
		}
	}
	size_t nSize = stringbuf_size(sb);
	if (nSize > 0) {
		format_insertLine(pFormatter, wp, &lpDest, sb, nAlignment);
	}
	stringbuf_destroy(sb);
	return lpDest;
}

/*
 * Get the formatter for the given view.
 */
static FORMATTER* format_getFormatter(WINFO* wp) {
	FTABLE* fp = wp->fp;
	char* pszFormatter = grammar_getFormatter(fp->documentDescriptor->grammar);
	if (!pszFormatter) {
		return &_defaultFormatter;
	}
	if (strcmp(pszFormatter, _codeFormatter.f_name) == 0) {
		return &_codeFormatter;
	}
	if (strcmp(pszFormatter, _textAndMarkupFormatter.f_name) == 0) {
		return &_textAndMarkupFormatter;
	}
	return &_defaultFormatter;
}

/*---------------------------------
 * ft_formatText()	
 * Format the text in the current file.
 *---------------------------------*/
int ft_formatText(WINFO* wp, int nRange, FORMATTING_ALIGNMENT nAlignment) {
	FORMATTER* pFormatter = format_getFormatter(wp);
	if (!pFormatter->f_supports(pFormatter)) {
		return FALSE;
	}
	FTABLE* fp = wp->fp;
	LINE* lplast = fp->lastl;
	LINE* lp;
	long ln = wp->caret.ln;
	/* watch also previous lines */
	if (nRange == RNG_LINE || nRange == RNG_CHAPTER || nRange == RNG_FROMCURS) {
		lp = wp->caret.linePointer;
		while (lp->prev && !pFormatter->f_startsNewParagraph(pFormatter, lp->prev)) {
			lp = lp->prev;
		}
	}
	else if (nRange == RNG_BLOCK) {
		if (!wp->blstart || !wp->blend) {
			return 0;
		}
		lp = wp->blstart->m_linePointer;
		lplast = wp->blend->m_linePointer;
	}
	else if (nRange == RNG_GLOBAL) {
		lp = fp->firstl;
	}
	PASTE paste;
	memset(&paste, 0, sizeof paste);
	paste.pln = ft_formatInto(pFormatter, wp, lp, lplast, nAlignment);
	LINE* lpTarget = lp->prev;
	bl_delete(wp, lp, lplast, 0, lplast->len, 0, 0);
	lpTarget = lpTarget ? lpTarget->next : fp->firstl;
	int ret = bl_paste(&paste, wp, lpTarget, 0, 0);
	ln_listfree(paste.pln);
	if (ret) {
		wp->caret.linePointer = fp->firstl;
		wp->caret.ln = 0;
		caret_placeCursorInCurrentFile(wp, ln, 0);
		render_repaintAllForFile(fp);
		return TRUE;
	}
	return FALSE;
}

/*
 * Calculates the screen indentation assumed for the line passed as an argument, that is the number of
 * column positions to be empty in a line inserted after the line passed as an argument.
 */
int format_calculateScreenIndent(WINFO* wp, LINE* lp, CI_OPTION cOption) {
	FORMATTER* pFormatter = format_getFormatter(wp);
	int nScreenCol;

	if (cOption == CI_THIS_LINE) {
		format_calculateIndent(pFormatter, wp, lp->lbuf, lp->len, &nScreenCol);
		return nScreenCol;
	}
	pFormatter->f_calculateIndent(pFormatter, wp, lp->lbuf, lp->len, &nScreenCol);
	return nScreenCol;
}


