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
#include "formatting.h"
#include "findandreplace.h"

#include "pksedit.h"
#include "mouseutil.h"
#include "winfo.h"

typedef struct tagFORMATTER_PARAM {
	WINFO* fparam_wp;
	LEXICAL_CONTEXT fparam_context;
	COMMENT_DESCRIPTOR fparam_cd;
} FORMATTER_PARAM;

typedef struct tagFORMATTER FORMATTER;

typedef int (*F_CALCULATE_INDENT)(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, const char* pBuffer, size_t nLen, int* pScreenCol);
typedef BOOL(*F_STARTS_NEW_PARAGRAPH)(FORMATTER* pFormatter, LINE* lp);
typedef BOOL(*F_TREAT_AS_EMPTY)(FORMATTER* pFormatter, LINE* lp);
typedef BOOL(*F_SHOULD_WRAP)(FORMATTER* pFormatter, const char* pBuf, int nPosition);
typedef void (*F_FORMAT_LINE)(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, STRING_BUF* sb, FORMATTING_ALIGNMENT nAlignment);
typedef BOOL(*F_SUPPORTS_FORMAT)(FORMATTER* pFormatter);
typedef LINE* (*F_FORMAT_INTO)(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, LINE* lp, LINE* lplast, FORMATTING_ALIGNMENT nAlignment);
typedef INDENTATION_DELTA(*F_CALC_INDENTATION_DELTA)(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, const char* pBuf, size_t nLen);

struct tagFORMATTER {
	const char* nf_name;								// a unique name to identify the formatter.
	const F_CALCULATE_INDENT f_calculateIndent;		// a callback for calculating the buffer offset in a line as it should be used for successive lines.
													// The line prior to the line currently formatted is passed to this method
	const F_STARTS_NEW_PARAGRAPH f_startsNewParagraph; // a callback determining, whether the specified line will start a new paragraph
	const F_SHOULD_WRAP f_maySplitAt;				// Returns true, if the formatter algorithm may attempt to wrap text to the next line at the given position. 
													// nPosition will never exceed the length of the line.
	const F_TREAT_AS_EMPTY f_treatAsEmpty;
	const F_FORMAT_LINE f_formatLine;
	const F_SUPPORTS_FORMAT f_supports;				// Whether text formatting is supported
	const F_FORMAT_INTO f_formatInto;				// The actual formatter function, which converts a block of lines into a formatted block of lines.
	const F_CALC_INDENTATION_DELTA f_calculateIndentationDelta;
};

/*
 * Calculate the lexical start state at a given line.
 */
extern LEXICAL_CONTEXT highlight_getLexicalStartStateFor(HIGHLIGHTER* pHighlighter, WINFO* wp, LINE* lp);

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
static int format_calculateTextIndent(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, const char* pBuf, size_t nLen, int *pScreenCol) {
	int j = 0;
	WINFO* wp = fparam->fparam_wp;
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
	*pScreenCol = (int)caret_bufferOffset2Screen(wp, pBuf, j);
	return j;
}

/*
 * Calculates the "indenting" of a line in an arbitrary file not checking for any syntactical constructs.
 */
static int format_calculateIndent(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, const char* pBuf, size_t nLen, int* pScreenCol) {
	int j = 0;
	WINFO* wp = fparam->fparam_wp;
	for (int i = 0; i < nLen; i++) {
		char c = pBuf[i];
		if (c == '\t' || c == ' ') {
			j = i + 1;
			continue;
		}
		break;
	}
	*pScreenCol = (int)caret_bufferOffset2Screen(wp, pBuf, j);
	return j;
}

static INDENTATION_DELTA format_calculateCodeIndentationDelta(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, const char* pBuf, size_t nLen) {
	WINFO* wp = fparam->fparam_wp;
	FTABLE* fp = wp->fp;
	LEXICAL_CONTEXT lcStart = fparam->fparam_context;
	if (nLen >= 2 && fparam->fparam_cd.comment_end) {
		COMMENT_DESCRIPTOR* pCD = &fparam->fparam_cd;
		BOOL nStartFound = FALSE;
		for (int i = 0; i < nLen - 1; i++) {
			// todo: allow for case ignore comment introducers like "REM" / "rem"
			if (pCD->comment_end && string_compareWithSecond(&pBuf[i+1], pCD->comment_end, FALSE) == 0) {
				if (nStartFound) {
					nStartFound = FALSE;
					break;
				}
				if (pBuf[0] == ' ') {
					return ID_OUTDENT_SPACE_NEXT;
				}
				break;
			}
			if (pCD->comment_start && string_compareWithSecond(&pBuf[i], pCD->comment_start, FALSE) == 0) {
				nStartFound = TRUE;
			}
		}
		if (nStartFound) {
			return ID_INDENT_SPACE_NEXT;
		}
	}
	int nDeltaIndent = grammar_getDeltaIndentation(fp->documentDescriptor->grammar, lcStart, pBuf, nLen);
	if (nDeltaIndent == 0) {
		return ID_NONE;
	}
	return nDeltaIndent > 0 ? ID_INDENT_NEXT : ID_OUTDENT_THIS;
}

/*
 * Calculates the "indenting" of a line in a code file. If the line ends with an "opening brace" ({ in C, Java and the like) the indent
 * is increased, if the line ends with a closing brace (} in C, Java and the like), indent is decreased. Indents are returned in terms of line offset.
 */
static int format_calculateCodeIndent(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, const char* pBuf, size_t nLen, int *pScreenCol) {
	WINFO* wp = fparam->fparam_wp;
	int screenCol;
	int j = format_calculateIndent(pFormatter, fparam, pBuf, nLen, &screenCol);
	INDENTATION_DELTA idDelta = pFormatter->f_calculateIndentationDelta(pFormatter, fparam, pBuf, nLen);
	int nScreen = (int)caret_bufferOffset2Screen(wp, pBuf, j);
	int ts = fparam->fparam_wp->indentation.tabsize;
	if (idDelta == ID_INDENT_NEXT) {
		nScreen += ts;
	} else if (idDelta == ID_OUTDENT_THIS) {
		nScreen -= ts;
	} else if (idDelta == ID_INDENT_SPACE_NEXT) {
		nScreen++;
	} else if (idDelta == ID_OUTDENT_SPACE_NEXT) {
		nScreen--;
	}
	*pScreenCol = (nScreen < 0) ? 0 : nScreen;
	return j;
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
	return TRUE;
}

static BOOL format_supportsFormatting(FORMATTER* pFormatter) {
	return TRUE;
}

static BOOL format_dontSupportsFormatting(FORMATTER* pFormatter) {
	return FALSE;
}

static void format_codeLine(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, STRING_BUF* pBuffer, FORMATTING_ALIGNMENT nAlignment) {
	WINFO* wp = fparam->fparam_wp;
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


static void format_textLine(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, STRING_BUF* pBuffer, FORMATTING_ALIGNMENT nAlignment) {
	WINFO* wp = fparam->fparam_wp;
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
		int nStart = pFormatter->f_calculateIndent(pFormatter, fparam, pBuf, nSize, &nScreenCol);
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
static void format_insertLine(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, LINE** pDest, STRING_BUF* pBuffer, FORMATTING_ALIGNMENT nAlignment) {
	pFormatter->f_formatLine(pFormatter, fparam, pBuffer, nAlignment);
	size_t nSize = stringbuf_size(pBuffer);
	LINE* lpNew = ln_create((int)nSize);
	memcpy(lpNew->lbuf, stringbuf_getString(pBuffer), nSize);
	stringbuf_reset(pBuffer);
	ll_add(pDest, (LINKED_LIST*) lpNew);
}

static LINE* format_textInto(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, LINE* lp, const LINE* lplast, FORMATTING_ALIGNMENT nAlignment) {
	WINFO* wp = fparam->fparam_wp;
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
		int nIndent = pFormatter->f_calculateIndent(pFormatter, fparam, lp->lbuf, lp->len, &nScreenCol);
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
			int nScreen = (int)caret_bufferOffset2Screen(wp, stringbuf_getString(sb), (int)nTargetSize);
			if (nScreen >= wp->rmargin && nLastWrappingPos > 0) {
				stringbuf_truncate(sb, nLastWrappingPos);
				format_insertLine(pFormatter, fparam, &lpDest, sb, nAlignment);
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
				format_insertLine(pFormatter, fparam, &lpDest, sb, nAlignment);
			}
			while (lp && lp != lplast && pFormatter->f_treatAsEmpty(pFormatter, lp)) {
				stringbuf_reset(sb);
				format_insertLine(pFormatter, fparam, &lpDest, sb, nAlignment);
				lp = lp->next;
			}
			bStartOfParagraph = TRUE;
		}
	}
	size_t nSize = stringbuf_size(sb);
	if (nSize > 0) {
		format_insertLine(pFormatter, fparam, &lpDest, sb, nAlignment);
	}
	stringbuf_destroy(sb);
	return lpDest;
}

static LINE* format_otherInto(FORMATTER* pFormatter, FORMATTER_PARAM* fparam, LINE* lp, LINE* lplast, FORMATTING_ALIGNMENT nAlignment) {
	WINFO* wp = fparam->fparam_wp;
	FTABLE* fp = wp->fp;
	GRAMMAR* pGrammar = fp->documentDescriptor->grammar;
	LINE* lpDest = 0;
	int nCurrentLineOffset;
	STRING_BUF* sb = stringbuf_create(128);

	char chSpace = ft_getSpaceFillCharacter(wp);
	int nCurrentScreenIndent = -1;
	fparam->fparam_context = highlight_getLexicalStartStateFor(wp->highlighter, wp, lp);
	grammar_getCommentDescriptor(pGrammar, &fparam->fparam_cd);
	while (lp) {
		if (pFormatter->f_treatAsEmpty(pFormatter, lp)) {
			format_insertLine(pFormatter, fparam, &lpDest, sb, nAlignment);
			if (lp == lplast) {
				break;
			}
			lp = lp->next;
			continue;
		}
		if (nCurrentScreenIndent < 0) {
			int nScreenCol;
			int nIndent = format_calculateIndent(pFormatter, fparam, lp->lbuf, lp->len, &nScreenCol);
			nCurrentScreenIndent = caret_lineOffset2screen(wp, &(CARET) {
				.linePointer = lp,
				.offset = nIndent
			});
		}
		INDENTATION_DELTA idDelta = pFormatter->f_calculateIndentationDelta(pFormatter, fparam, lp->lbuf, lp->len);
		if (idDelta == ID_OUTDENT_THIS) {
			nCurrentScreenIndent -= wp->indentation.tabsize;
			if (nCurrentScreenIndent < 0) {
				nCurrentScreenIndent = 0;
			}
		}
		nCurrentLineOffset = 0;
		for (int nCol = 0; nCol < nCurrentScreenIndent; nCol++) {
			stringbuf_appendChar(sb, chSpace);
		}
		if (idDelta == ID_INDENT_NEXT) {
			nCurrentScreenIndent += wp->indentation.tabsize;
		} else if (idDelta == ID_INDENT_SPACE_NEXT) {
			nCurrentScreenIndent++;
		} else if (idDelta == ID_OUTDENT_SPACE_NEXT && nCurrentScreenIndent > 0) {
			nCurrentScreenIndent--;
		}
		BOOL bNonSpaceFound = FALSE;
		while (nCurrentLineOffset < lp->len) {
			char c = lp->lbuf[nCurrentLineOffset];
			if ((c != chSpace && c != '\t') || bNonSpaceFound) {
				stringbuf_appendChar(sb, c);
				bNonSpaceFound = TRUE;
			}
			nCurrentLineOffset++;
		}
		format_insertLine(pFormatter, fparam, &lpDest, sb, nAlignment);
		if (lp->len > 0) {
			fparam->fparam_context = grammar_getLexicalContextAt(pGrammar, fparam->fparam_context, lp->lbuf, lp->len, lp->len - 1);
			if (fparam->fparam_context == LC_SINGLE_LINE_COMMENT) {
				fparam->fparam_context = LC_START;
			}
		}
		if (lp == lplast) {
			break;
		}
		lp = lp->next;
	}
	stringbuf_destroy(sb);
	return lpDest;
}

static FORMATTER _textAndMarkupFormatter = {
	.nf_name = "text",
	.f_maySplitAt = format_maySplitTextlineAt,
	.f_calculateIndent = format_calculateTextIndent,
	.f_startsNewParagraph = format_startsParagraphInTextFiles,
	.f_treatAsEmpty = format_treatTextlineEmpty,
	.f_formatLine = format_textLine,
	.f_supports = format_supportsFormatting,
	.f_formatInto = format_textInto,
	.f_calculateIndentationDelta = format_calculateCodeIndentationDelta
};

static FORMATTER _codeFormatter = {
	.nf_name = "code",
	.f_maySplitAt = format_maySplitTextlineAt,
	.f_calculateIndent = format_calculateCodeIndent,
	.f_startsNewParagraph = format_startsParagraphInCodeFiles,
	.f_treatAsEmpty = format_treatTextlineEmpty,
	.f_formatLine = format_codeLine,
	.f_supports = format_supportsFormatting,
	.f_formatInto = format_otherInto,
	.f_calculateIndentationDelta = format_calculateCodeIndentationDelta
};

static FORMATTER _defaultFormatter = {
	.nf_name = "default",
	.f_maySplitAt = format_maySplitTextlineAt,
	.f_calculateIndent = format_calculateIndent,
	.f_startsNewParagraph = format_startsParagraphInTextFiles,
	.f_treatAsEmpty = format_treatTextlineEmpty,
	.f_formatLine = format_codeLine,
	.f_supports = format_dontSupportsFormatting,
	.f_formatInto = format_otherInto,
	.f_calculateIndentationDelta = format_calculateCodeIndentationDelta
};


/*
 * Get the formatter for the given view.
 */
static FORMATTER* format_getFormatter(WINFO* wp) {
	FTABLE* fp = wp->fp;
	EDIT_CONFIGURATION* pDescriptor = fp->documentDescriptor;
	char* pszFormatter = !pDescriptor ? 0 : grammar_getFormatter(pDescriptor->grammar);
	if (!pszFormatter) {
		return &_defaultFormatter;
	}
	if (strcmp(pszFormatter, _codeFormatter.nf_name) == 0) {
		return &_codeFormatter;
	}
	if (strcmp(pszFormatter, _textAndMarkupFormatter.nf_name) == 0) {
		return &_textAndMarkupFormatter;
	}
	return &_defaultFormatter;
}

/*
 * Can be used to determine whether formatting text is supported for the current file.
 */
int ft_supportsFormatting(intptr_t* pUnused) {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (!wp) {
		return 0;
	}
	FORMATTER* pFormatter = format_getFormatter(wp);
	return pFormatter->f_supports(pFormatter);
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
	} else if (nRange == RNG_BLOCK) {
		if (!wp->blstart || !wp->blend) {
			return 0;
		}
		lp = wp->blstart->m_linePointer;
		lplast = wp->blend->m_linePointer;
	} else if (nRange == RNG_GLOBAL) {
		lp = fp->firstl;
	} else {
		return 0;
	}
	bl_hideSelection(wp, FALSE);
	PASTE paste;
	memset(&paste, 0, sizeof paste);
	FORMATTER_PARAM fparam;
	memset(&fparam, 0, sizeof fparam);
	fparam.fparam_wp = wp;
	paste.pln = pFormatter->f_formatInto(pFormatter, &fparam, lp, lplast, nAlignment);
	LINE* lpFormatted = paste.pln;
	LINE* lpReplace = lpFormatted;
	// skip lines already correctly formatted.
	while (lpReplace && lp) {
		if (lpReplace->len != lp->len || memcmp(lpReplace->lbuf, lp->lbuf, lpReplace->len) != 0) {
			break;
		}
		if (lp == lplast) {
			break;
		}
		lpReplace = lpReplace->next;
		lp = lp->next;
	}
	int ret = 1;
	if (lp && lp != lplast) {
		LINE* lpTarget = lp->prev;
		bl_delete(wp, lp, lplast, 0, lplast->len, 0, 0);
		lpTarget = lpTarget ? lpTarget->next : fp->firstl;
		paste.pln = lpReplace;
		ret = bl_paste(&paste, wp, lpTarget, 0, 0);
	}
	ln_listfree(lpFormatted);
	if (ret) {
		wp->caret.linePointer = fp->firstl;
		wp->caret.ln = 0;
		caret_placeCursorInCurrentFile(wp, ln, 0);
		render_repaintAllForFile(fp);
		return TRUE;
	}
	return FALSE;
}

static FORMATTER* format_initParams(WINFO* wp, LINE* lp, FORMATTER_PARAM *pParams) {
	FORMATTER* pFormatter = format_getFormatter(wp);

	memset(pParams, 0, sizeof *pParams);
	pParams->fparam_wp = wp;
	pParams->fparam_context = highlight_getLexicalStartStateFor(wp->highlighter, wp, lp);
	FTABLE* fp = wp->fp;
	GRAMMAR* pGrammar = fp->documentDescriptor->grammar;
	grammar_getCommentDescriptor(pGrammar, &pParams->fparam_cd);
	return pFormatter;
}

/*
 * Calculates the screen indentation assumed for the line passed as an argument, that is the number of
 * column positions to be empty in a line inserted after the line passed as an argument. Syntactical 
 * constructs are honored.
 */
int format_calculateScreenIndentWithSyntax(WINFO* wp, LINE* lp) {
	FORMATTER_PARAM fparam;
	FORMATTER* pFormatter = format_initParams(wp, lp, &fparam);
	int nScreenCol;
	LINE* lpPrev = lp->prev;

	if (lpPrev) {
		INDENTATION_DELTA nDelta = pFormatter->f_calculateIndentationDelta(pFormatter, &fparam, lp->lbuf, lp->len);
		if (nDelta == ID_OUTDENT_THIS) {
			format_calculateCodeIndent(pFormatter, &fparam, lpPrev->lbuf, lpPrev->len, &nScreenCol);
			nScreenCol -= wp->indentation.tabsize;
			return nScreenCol;
		}
	}
	pFormatter->f_calculateIndent(pFormatter, &fparam, lp->lbuf, lp->len, &nScreenCol);
	return nScreenCol;
}

/*
 * Calculates the screen indentation assumed for the line passed as an argument, that is the number of
 * column positions to be empty in a line inserted after the line passed as an argument.
 */
int format_calculateScreenIndent(WINFO* wp, LINE* lp) {
	FORMATTER_PARAM fparam;
	FORMATTER* pFormatter = format_initParams(wp, lp, &fparam);
	int nScreenCol;

	format_calculateIndent(pFormatter, &fparam, lp->lbuf, lp->len, &nScreenCol);
	return nScreenCol;
}

/*
 * Calculates the screen indentation delta operation assumed for the line passed as an argument, which
 * can be used as a hint how indentation should be performed.
 */
int format_calculateIndentationDelta(WINFO* wp, LINE* lp) {
	FORMATTER_PARAM fparam;
	FORMATTER* pFormatter = format_initParams(wp, lp, &fparam);

	fparam.fparam_context = highlight_getLexicalStartStateFor(wp->highlighter, wp, lp);
	return pFormatter->f_calculateIndentationDelta(pFormatter, &fparam, lp->lbuf, lp->len);
}


