/*
 * Templates.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: define, evaluate and insert templates into a document.
 *
 * 										created: 27.01.1991
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
+ */

#include <windows.h>
#include <string.h>
#include "tos.h"
#include "pksrc.h"
#include "caretmovement.h"
#include "edierror.h"
#include "pksmacro.h"
#include "fileselector.h"
#include "winterf.h"
#include "winfo.h"
#include "grammar.h"
#include "syntaxhighlighting.h"
#include "stringutil.h"
#include "fileutil.h"
#include "trace.h"
#include "textblocks.h"
#include "regexp.h"
#include "formatting.h"
#include "documenttypes.h"
#include "crossreferencelinks.h"
#include "editorconfiguration.h"
#include "brackets.h"
#include "stringutil.h"
#include "copyright.h"

#define	MAX_CONTEXT	32
#define MAX_SECONDARY 6

typedef struct tagCARET_DELTA {
	long cd_deltaLn;		// If the cursor should be positioned after inserting the template, this is the number of lines relative to the beginning of the inserted code.
	long cd_deltaCol;		// If the cursor should be positioned after inserting the template, this is the number of columns relative to the beginning of the inserted code.
} CARET_DELTA;

typedef struct tagTEMPLATE_ACTION {
	BOOL ta_positionCursor;		// true, if the cursor should be positioned after inserting the template.
	CARET_DELTA ta_cursorDelta;	// If the cursor should be positioned after inserting the template, this is the number of lines relative to the beginning of the inserted code.
	int  ta_secondaryCarets;	// Number of secondary carets.
	CARET_DELTA ta_secondary[MAX_SECONDARY];	// The secondary carets to create.
	long ta_selectionDeltaLn;	// If text should be selected after inserting the template, this is the number of lines relative to the cursor as specified by ta_cursorDeltaLn.
	long ta_selectionDeltaCol;	// If text should be selected after inserting the template, this is the number of columns relative to the cursor as specified by ta_cursorDeltaLn.
} TEMPLATE_ACTION;

/*
 * Calculate the lexical start state at a given line.
 */
extern LEXICAL_CONTEXT highlight_getLexicalStartStateFor(HIGHLIGHTER* pHighlighter, WINFO* wp, LINE* lp);

// forward declaration.
static STRING_BUF* template_expandCodeTemplate(WINFO* wp, TEMPLATE_ACTION* pTAction, int nIndent, const unsigned char* pszSelected, 
	const unsigned char* pszSource);

/*
 * Append a nested template.
 */
static void template_appendTemplate(STRING_BUF* pDest, WINFO* wp, int nIndent, const char* pszTemplateName) {
	FTABLE* fp = FTPOI(wp);
	const char* pszSource = grammar_getTemplate(fp->documentDescriptor->grammar, pszTemplateName);
	if (pszSource == NULL) {
		EdTRACE(log_message(DEBUG_WARN, "Cannot find nested template %s", pszTemplateName));
		return;
	}
	TEMPLATE_ACTION action;
	STRING_BUF* pszExpanded = template_expandCodeTemplate(wp, &action, nIndent, NULL, pszSource);
	if (pszExpanded != NULL) {
		stringbuf_appendString(pDest, stringbuf_getString(pszExpanded));
		stringbuf_destroy(pszExpanded);
	}
}

/*
 * Append the copyright formatted according to the grammar of the document.
 */
static void template_appendCopyright(STRING_BUF* pDest, WINFO* wp, TEMPLATE_ACTION* pAction, int nIndent) {
	FTABLE* fp = FTPOI(wp);
	STRING_BUF *pCopyright = copyright_getFormatted(fp->documentDescriptor);
	if (pCopyright == NULL) {
		EdTRACE(log_message(DEBUG_WARN, "Cannot format copyright or no copyrights defined."));
		return;
	}
	STRING_BUF* pszExpanded = template_expandCodeTemplate(wp, pAction, nIndent, NULL, stringbuf_getString(pCopyright));
	if (pszExpanded != NULL) {
		stringbuf_appendString(pDest, stringbuf_getString(pszExpanded));
		stringbuf_destroy(pszExpanded);
	}
	stringbuf_destroy(pCopyright);
}

/*
 * Expand a code template optionally containing ${....} references and return
 * the expanded text.
 */
static STRING_BUF* template_expandCodeTemplate(WINFO* wp, TEMPLATE_ACTION *pTAction, int nIndent, const unsigned char* pszSelected, 
		const unsigned char* pszSource) {
	size_t nInitialSize = strlen(pszSource);
	size_t nLineBegin = 0;
	STRING_BUF* pResult = stringbuf_create(nInitialSize);
	unsigned char* pVar = NULL;
	unsigned char variable[50];
	unsigned char expandedVariable[512];
	unsigned char c;
	long col = 0;
	long ln = 0;
	char chSpace = ft_getSpaceFillCharacter(wp);
	FTABLE* fp = wp->fp;
	BOOL bUseTabs = fp->documentDescriptor->expandTabsWith == 0;
	char cr = fp->documentDescriptor->cr;
	BOOL bBreak = FALSE;

	while (!bBreak) {
		c = *pszSource++;
		if (c == 0) {
			c = '\n';
			bBreak = TRUE;
		}
		if (pVar) {
			if (c == '}') {
				*pVar = 0;
				expandedVariable[0] = 0;
				if (strcmp("cursor", variable) == 0) {
					pTAction->ta_cursorDelta.cd_deltaCol = col;
					pTAction->ta_cursorDelta.cd_deltaLn = ln;
					pTAction->ta_positionCursor = TRUE;
				}
				else if (strcmp("secondary", variable) == 0) {
					if (pTAction->ta_secondaryCarets < MAX_SECONDARY) {
						CARET_DELTA* pDelta = &pTAction->ta_secondary[pTAction->ta_secondaryCarets++];
						pDelta->cd_deltaCol = col;
						pDelta->cd_deltaLn = ln;
					}
				}
				else if (strcmp("indent", variable) == 0) {
					for (int i = 0; i < nIndent; i++) {
						stringbuf_appendChar(pResult, chSpace);
					}
					col += nIndent;
				}
				else if (strcmp("tab", variable) == 0) {
					for (int i = 0; i < fp->documentDescriptor->tabsize; i++) {
						stringbuf_appendChar(pResult, chSpace);
					}
					col += fp->documentDescriptor->tabsize;
				}
				else if (strcmp("selection_end", variable) == 0) {
					pTAction->ta_selectionDeltaCol = col - pTAction->ta_cursorDelta.cd_deltaCol;
					pTAction->ta_selectionDeltaLn = ln - pTAction->ta_cursorDelta.cd_deltaLn;
				}
				else if (strcmp("word_selection", variable) == 0) {
					strcpy(expandedVariable, pszSelected);
				} else if (strcmp(variable, "copyright") == 0) {
					template_appendCopyright(pResult, wp, pTAction, nIndent);
				} else if (strstr(variable, "template.") == variable) {
					template_appendTemplate(pResult, wp, nIndent, variable + 9);
				} else {
					string_getVariable(wp, variable, expandedVariable, sizeof expandedVariable);
				}
				stringbuf_appendString(pResult, expandedVariable);
				col += (long)strlen(expandedVariable);
				pVar = NULL;
			}
			else if (pVar < variable + sizeof variable - 1) {
				*pVar++ = c;
			}
		} else if (c == '\\' && *pszSource == '$') {
			c = '$';
			pszSource++;
			goto normalText;
		} else if (c == '$' && *pszSource == '{') {
			pszSource++;
			pVar = variable;
		} else {
		normalText:
			if (c == '\n') {
				if (cr) {
					stringbuf_appendChar(pResult, cr);
				}
				ln++;
				col = 0;
			} else {
				col++;
			}
			stringbuf_appendChar(pResult, c);
			if (col == 0) {
				size_t nOffset = stringbuf_size(pResult);
				if (nOffset - nLineBegin > 1 && bUseTabs) {
					long ntabs;
					int nNewSize = ft_compressSpacesToTabs(wp, _linebuf, LINEBUFSIZE, &stringbuf_getString(pResult)[nLineBegin], nOffset - nLineBegin - 1, &ntabs);
					if (ntabs) {
						stringbuf_truncate(pResult, nLineBegin);
						stringbuf_appendStringLength(pResult, _linebuf, nNewSize);
						stringbuf_appendChar(pResult, c);
						nOffset = stringbuf_size(pResult);
					}
				}
				nLineBegin = nOffset;
			}
		}
	}
	return pResult;
}

/*
 * Delete the current identifier left to or under the cursor for the purpose of being replaced by a macro. 
 */
static void template_replaceCurrentWord(WINFO* wp, int nStart, const char* pszIdentifier) {
	if (ww_hasSelection(wp)) {
		EdBlockDelete(wp, 0);
	} else {
		size_t len = strlen(pszIdentifier);
		size_t o1 = nStart;
		size_t o2 = nStart+len;
		if (o2 > wp->caret.linePointer->len) {
			o2 = wp->caret.linePointer->len;
		}
		ln_modify(wp->fp, wp->caret.linePointer, (int)o2, (int)o1);
		wp->caret.offset = (long)o1;
		wp->caret.col = caret_lineOffset2screen(wp, &wp->caret);
		render_repaintCurrentLine(wp);
	}
}

/*
 * Creates a String Buffer containing an expanded code template. The returned string buffer must be destroyed by the caller.
 * If an error occurs or there is no current window, this will return NULL.
 */
char* template_expandCodeTemplateFor(UCLIST* up) {
	char szIdentifier[100];
	WINFO* wp = ww_getCurrentEditorWindow();
	TEMPLATE_ACTION action;

	if (wp == NULL) {
		return NULL;
	}
	xref_getSelectedIdentifier(wp, szIdentifier, sizeof szIdentifier, NULL);
	int nIndent = format_calculateScreenIndent(wp, wp->caret.linePointer);
	STRING_BUF * pBuf = template_expandCodeTemplate(wp, &action, nIndent, szIdentifier, up->p.uc_template);
	char* pResult = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);
	return pResult;
}

/*
 * Matches a pattern close to the current caret position. Pattern is the pattern of a template to insert (e.g. for ":+1" => "text to insert",
 * the pattern is ":+1". We try to find out, which part of the pattern is "matched" depending on the caret position. If the caret
 * is positioned like this ":1|+" the matched part to return in szIdentifier is ":1". nMatchedSize is the space available in
 * szIdentifier.
 */
int template_matchIdentifier(WINFO* wp, char* pPattern, char* szIdentifier, size_t nMatchedSize) {
	size_t nPatternLength;
	*szIdentifier = 0;
	if (pPattern && (nPatternLength = strlen(pPattern)) != 0) {
		int start = wp->caret.offset;
		for (int i = (int)(start - nPatternLength); i < start; i++) {
			size_t nLen = (size_t) start - i;
			if (i >= 0 && strncmp(&wp->caret.linePointer->lbuf[i], pPattern, nLen) == 0) {
				if (nLen > nMatchedSize - 1) {
					nLen = nMatchedSize - 1;
				}
				strncpy(szIdentifier, pPattern, nLen);
				szIdentifier[nLen] = 0;
				return i;
			}
		}
	} else {
		int nOffset = 0;
		xref_getSelectedIdentifier(wp, szIdentifier, sizeof szIdentifier, &nOffset);
		return nOffset;
	}
	return 0;
}

/*
 * Insert a code template defined by pszBuffer.
 *
 * If 'bReplaceCurrentWord' is TRUE, the currently selected word / identifier close to the
 * cursor is replaced by the inserted template.
 * If szIdentifier is not empty, it is the identifier, which will be replaced.
 */
int template_insertCodeTemplateBuffer(WINFO* wp, const char* pszTemplate, const char* szIdentifier, BOOL bReplaceCurrentWord, int nReplacedTextStart) {
	FTABLE* fp = wp->fp;
	EDIT_CONFIGURATION* pConfig = fp->documentDescriptor;
	TEMPLATE_ACTION templateAction;
	int ret = 0;
	memset(&templateAction, 0, sizeof templateAction);
	int nIndent = format_calculateScreenIndent(wp, wp->caret.linePointer);
	STRING_BUF* pSB = template_expandCodeTemplate(wp, &templateAction, nIndent, szIdentifier, pszTemplate);
	PASTE pasteBuffer;
	memset(&pasteBuffer, 0, sizeof pasteBuffer);
	unsigned char* pszText = stringbuf_getString(pSB);
	if (bl_convertTextToPasteBuffer(&pasteBuffer, pszText, pszText + strlen(pszText), pConfig->nl, pConfig->nl2, pConfig->cr)) {
		if (bReplaceCurrentWord) {
			template_replaceCurrentWord(wp, nReplacedTextStart, szIdentifier);
		}
		CARET oldCaret = wp->caret;
		int firstLineCol = oldCaret.col;
		bl_pasteBlock(wp, &pasteBuffer, 0, oldCaret.offset, 0);
		if (templateAction.ta_positionCursor) {
			long col = templateAction.ta_cursorDelta.cd_deltaLn ? templateAction.ta_cursorDelta.cd_deltaCol :
				templateAction.ta_cursorDelta.cd_deltaCol + firstLineCol;
			long ln = templateAction.ta_cursorDelta.cd_deltaLn + oldCaret.ln;
			col = caret_screen2lineOffset(wp, &(CARET) {
				.ln = ln,
					.offset = col
			});
			caret_placeCursorInCurrentFile(wp, ln, col);
			if (templateAction.ta_selectionDeltaCol || templateAction.ta_selectionDeltaLn) {
				bl_hideSelection(wp, 0);
				EdSyncSelectionWithCaret(wp, MARK_START);
				col += templateAction.ta_selectionDeltaCol;
				ln += templateAction.ta_selectionDeltaLn;
				col = caret_screen2lineOffset(wp, &(CARET) {
					.ln = ln,
						.offset = col
				});
				caret_placeCursorInCurrentFile(wp, ln, col);
				EdSyncSelectionWithCaret(wp, MARK_END);
			}
			for (int i = 0; i < templateAction.ta_secondaryCarets; i++) {
				CARET_DELTA* pDelta = &templateAction.ta_secondary[i];
				int secondaryCol = pDelta->cd_deltaLn ? pDelta->cd_deltaCol : firstLineCol + pDelta->cd_deltaCol;
				ln = oldCaret.ln + pDelta->cd_deltaLn;
				secondaryCol = caret_screen2lineOffset(wp, &(CARET) {
					.ln = ln,
						.offset = secondaryCol
				});
				caret_addSecondary(wp, ln, secondaryCol);
			}
		}
		ln_listfree(pasteBuffer.pln);
		ret = 1;
	}
	stringbuf_destroy(pSB);
	return ret;

}

/*
 * Insert a selected code template 'up'. 
 * If 'bReplaceCurrentWord' is TRUE, the currently selected word / identifier close to the
 * cursor is replaced by the inserted template.
 * If 'nReplacedTextStart' > 0, it is assumed, this is the byte offset in the line, where the
 * replacements starts.
 * If 'nReplacedTextLength' > 0, it is assumed, that this is the number of characters left
 * to the caret to be replaced.
 */
int template_insertCodeTemplate(WINFO* wp, UCLIST* up, int nReplacedTextStart, int nReplacedTextLength, BOOL bReplaceCurrentWord) {
	char szIdentifier[100];
	int ret = 0;
	char* pPattern = up->uc_pattern.pattern;
	if (nReplacedTextStart <= 0 && nReplacedTextLength > 0) {
		nReplacedTextStart = wp->caret.offset - nReplacedTextLength;
		if (nReplacedTextStart < 0) {
			nReplacedTextStart = 0;
		}
	}
	if (nReplacedTextLength > 0) {
		if (nReplacedTextLength > sizeof szIdentifier - 1) {
			nReplacedTextLength = (int)sizeof szIdentifier - 1;
		}
		strncpy(szIdentifier, &wp->caret.linePointer->lbuf[nReplacedTextStart], nReplacedTextLength);
		szIdentifier[nReplacedTextLength] = 0;
	} else {
		int nOffset = template_matchIdentifier(wp, pPattern, szIdentifier, sizeof szIdentifier);
		if (nReplacedTextStart <= 0) {
			nReplacedTextStart = nOffset;
		}
	}
	return template_insertCodeTemplateBuffer(wp, up->p.uc_template, szIdentifier, bReplaceCurrentWord, nReplacedTextStart);
}

/*--------------------------------------------------------------------------
 * template_expandAbbreviation()
 */
int template_expandAbbreviation(WINFO* wp, LINE* lp, int offs) {
	UCLIST* up;
	long 		o2;
	int			domacro = 0;
	FTABLE* fp = wp->fp;
	EDIT_CONFIGURATION* pConfig = fp->documentDescriptor;
	GRAMMAR* pGrammar = pConfig->grammar;

	if ((up = uc_find(pGrammar, lp, offs)) == 0) {
		return 0;
	}
	if (up->action == UA_ABBREV) {
		if (up->uc_pattern.lexicalContexts) {
			LEXICAL_CONTEXT lcStart = highlight_getLexicalStartStateFor(wp->highlighter, wp, lp);
			LEXICAL_CONTEXT lcContext = grammar_getLexicalContextAt(pGrammar, lcStart, lp->lbuf, lp->len, offs);
			if ((up->uc_pattern.lexicalContexts & lcContext) == 0) {
				return 0;
			}
		}
		domacro = 0;
	}
	else if (up->action == UA_UCMACRO) {
		domacro = 1;
	}
	else {
		return 0;
	}

	if (up->action != UA_ABBREV) {
		o2 = offs - up->uc_pattern.len;
		if (ln_modify(fp, lp, offs, o2) == 0L)
			return 0;
	}
	else {
		o2 = offs;
	}
	caret_placeCursorInCurrentFile(wp, wp->caret.ln, o2);
	if (!domacro) {
		return template_insertCodeTemplate(wp, up, -1, -1, FALSE);
	}
	render_repaintCurrentLine(wp);
	return macro_executeByName(up->p.uc_macro);
}



