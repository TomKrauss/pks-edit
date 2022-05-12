/*
 * CodeAnalyzer.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: analyze code for the purpose of calculating suggestions.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include "alloc.h"
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "stringutil.h"
#include "documentmodel.h"
#include "linkedlist.h"
#include "winfo.h"
#include "hashmap.h"
#include "codeanalyzer.h"
#include "codecompletion.h"
#include "pksmacro.h"
#include "actionbindings.h"
#include "funcdef.h"
#include "pksmacrocvm.h"
#include "symbols.h"
#include "fontawesome.h"
#include "grammar.h"

typedef struct tagANALYZER {
	struct tagANALYZER* an_next;
	char an_name[32];
	ANALYZER_FUNCTION an_function;
} ANALYZER;

static ANALYZER *_analyzers;

/*
 * Calculate the lexical start state at a given line.
 */
extern LEXICAL_CONTEXT highlight_getLexicalStartStateFor(HIGHLIGHTER* pHighlighter, WINFO* wp, LINE* lp);

/*
 * Extract all identifiers from a file regardless of comments etc ignoring the file syntax.
 */
static void analyzer_extractWords(WINFO* wp, int (*fMatch)(const char* pszMatch), ANALYZER_CALLBACK fCallback) {
	FTABLE* fp = wp->fp;
	LINE* lp = fp->firstl;
	STRING_BUF* pBuf = stringbuf_create(50);

	while (lp && lp != fp->lastl) {
		char* pszRun = lp->lbuf;
		char* pszEnd = &lp->lbuf[lp->len];
		while (pszRun < pszEnd) {
			char c = *pszRun++;
			if (char_isIdentifier(c)) {
				stringbuf_appendChar(pBuf, c);
			} else {
				if (isspace(c)) {
					char* pszWord = stringbuf_getString(pBuf);
					if (*pszWord) {
						if (fMatch(pszWord)) {
							fCallback(&(ANALYZER_CALLBACK_PARAM) {
								.acp_recommendation = pszWord
							});
						}
					}
				}
				stringbuf_reset(pBuf);
			}
		}
		stringbuf_reset(pBuf);
		lp = lp->next;
	}
	stringbuf_destroy(pBuf);
}

static void analyzer_getToken(char* pszDest, LINE* lp, int nStart, int nLen) {
	int nEnd = nLen + nStart;
	while (nStart < nEnd && nStart < lp->len) {
		*pszDest++ = lp->lbuf[nStart++];
	}
	*pszDest = 0;
}

/*
 * Returns a possible macro function names which can be used in PKS Edit macros.
 */
static void analyzer_getMacrocCompletions(WINFO* wp, int (*fMatch)(const char* pszMatch), ANALYZER_CALLBACK fCallback) {
	char szBuf[256];
	char szFunction[256];
	int detectedEnd = 0;
	FTABLE* fp = wp->fp;
	LINE* lp = wp->caret.linePointer;
	LEXICAL_ELEMENT lexicalElements[MAX_LEXICAL_ELEMENT];
	GRAMMAR* pGrammar = fp->documentDescriptor->grammar;
	LEXICAL_STATE lexicalState = highlight_getLexicalStartStateFor(wp->highlighter, wp, lp);
	int nElements = grammar_parse(pGrammar, lexicalElements, lexicalState, lp->lbuf, lp->len, &detectedEnd);
	int nOffset = 0;
	int state = 0;
	int nParamIndex = 0;
	int bInIdent = 0;
	for (int i = 0; i < nElements; i++) {
		if (nOffset >= wp->caret.offset) {
			break;
		}
		const char* pState = grammar_getPatternName(pGrammar, lexicalElements[i].le_state);
		if (pState) {
			bInIdent = 0;
			analyzer_getToken(szBuf, lp, nOffset, lexicalElements[i].le_length);
			int bIsIdent;
			if ((bIsIdent = (strcmp(pState, "pksmacroc.identifier") == 0)) || strcmp(pState, "constant.identifier") == 0 || strcmp(pState, "keyword.pksedit") == 0) {
				bInIdent = 1;
				if (bIsIdent) {
					strcpy(szFunction, szBuf);
				}
			} else if (strcmp(pState, "keyword.function.delimiter") == 0) {
				if (strcmp("(", szBuf) == 0) {
					nParamIndex = 0;
					state++;
				}
				else if (strcmp(")", szBuf) == 0) {
					nParamIndex = -1;
					state--;
				} else if (strcmp(",", szBuf) == 0 && nParamIndex >= 0) {
					nParamIndex++;
				}
				if (!state) {
					szFunction[0] = 0;
				}
			}
		}
		nOffset += lexicalElements[i].le_length;
	}
	char* key;
	SYMBOL sym = szFunction[0] ? sym_find(sym_getGlobalCompilerContext(), szFunction, &key) : (SYMBOL) { 0 };
	if (sym.s_type == S_EDFUNC) {
		// should select the proper constants for the native function currently edited. function_getParameterTypeDescriptor((void*)VALUE(sym),)
		NATIVE_FUNCTION* pFunc = (NATIVE_FUNCTION*)VALUE(sym);
		if (!bInIdent) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) {
				.acp_recommendation = pFunc->nf_name,
					.acp_object = pFunc,
					.acp_help = macrodoc_helpForNativeFunction,
					.acp_getHyperlinkText = macrodoc_helpForHyperlink
			});
		}
		int nParameters = function_getParameterCount(pFunc);
		int bEditorParam = 0;
		for (int nPar = 0; nPar < nParameters; nPar++) {
			PARAMETER_TYPE_DESCRIPTOR ptd = function_getParameterTypeDescriptor(pFunc, nPar+1);
			if (nPar == 0 && ptd.pt_type == PARAM_TYPE_EDITOR_WINDOW) {
				bEditorParam = 1;
			}
			if (ptd.pt_enumVal) {
				for (int i = 0; i < ptd.pt_enumCount; i++) {
					PARAMETER_ENUM_VALUE* pValue = &ptd.pt_enumVal[i];
					const char* pszName = pValue->pev_name;
					if ((bInIdent && fMatch(pszName)) || nPar == nParamIndex || (bEditorParam && (nPar - 1 == nParamIndex))) {
						fCallback(&(ANALYZER_CALLBACK_PARAM) {
							.acp_recommendation = pszName,
								.acp_object = pValue,
								.acp_help = macrodoc_helpForEnumValue
						});
					}
				}
			}
		}
	}
	for (int i = 0; i < _functionTableSize; i++) {
		NATIVE_FUNCTION* pFunc = &_functionTable[i];
		if (fMatch(pFunc->nf_name)) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) { .acp_recommendation = pFunc->nf_name, 
				.acp_help = macrodoc_helpForNativeFunction,
				.acp_object= pFunc,
				.acp_getHyperlinkText = macrodoc_helpForHyperlink
			});
		}
	}
	for (PKS_VALUE_TYPE vt = VT_NIL; types_existsType(vt); vt++) {
		const char* pName = types_nameFor(vt);
		if (fMatch(pName)) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) { 
				.acp_recommendation = pName, 
				.acp_object = (void*)pName, 
				.acp_help = macrodoc_helpForType,
				.acp_getHyperlinkText = macrodoc_helpForHyperlink
			});
		}
	}
	for (int i = 0; i < macro_getNumberOfMacros(); i++) {
		MACRO* mp = macro_getByIndex(i);
		if (mp && fMatch(MAC_NAME(mp))) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) {
				.acp_recommendation = MAC_NAME(mp),
					.acp_object = mp,
					.acp_help = macrodoc_helpForMacro,
					.acp_getHyperlinkText = macrodoc_helpForHyperlink
			});
		}
	}
}

static const char* analyzer_helpForCommand(const char* pszCommandName, void* pCommand) {
	MACROREF macref = INTPTR_TO_MACROREF((intptr_t)pCommand);
	char szTooltip[MAC_COMMENTLEN];
	return _strdup(command_getTooltipAndLabel(macref, szTooltip, NULL));
}

static ANALYZER_CALLBACK _keyAnalyzerCallback;
static void analyzer_addKeycode(const char* pszKeycode) {
	_keyAnalyzerCallback(&(ANALYZER_CALLBACK_PARAM) { .acp_recommendation = pszKeycode });
}

/*
 * Returns a possible code completion in a PKS actionbinding file - either the name of a PKS command, the name of a fontawesome icon
 * or the name of a key.
 */
static void analyzer_getBindingCompletions(WINFO* wp, int (*fMatch)(const char* pszMatch), ANALYZER_CALLBACK fCallback) {
	if (wp->caret.offset > 0 && wp->caret.linePointer->lbuf[wp->caret.offset - 1] == '+') {
		_keyAnalyzerCallback = fCallback;
		bindings_addModifiersAndKeycodes(fMatch, analyzer_addKeycode);
		return;
	}
	const char* pszWord;
	for (int i = 0; (pszWord = macro_getCommandByIndex(i)) != 0; i++) {
		if (fMatch(pszWord)) {
			MACROREF macref = (MACROREF){ .typ = CMD_CMDSEQ, .index = i };
			fCallback(&(ANALYZER_CALLBACK_PARAM) {
				.acp_recommendation = pszWord,
					.acp_object = (void*)MACROREF_TO_INTPTR(macref),
					.acp_help = analyzer_helpForCommand,
					.acp_getHyperlinkText = macrodoc_helpForHyperlink
			});
		}
	}
	for (int i = 0; (pszWord = faicon_nameForIndex(i)) != 0; i++) {
		if (fMatch(pszWord)) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) {
				.acp_recommendation = pszWord
			});
		}
	}
}

/*
 * Register a new analyzer given the name of the analyzer. If an analyzer with the given name
 * exists, it is overridden. Return 1 if a new analyzer was registered and 0 if an existing one
 * was overridden.
 */
int analyzer_register(const char* pszName, ANALYZER_FUNCTION f) {
	ANALYZER* pAnalyzer = _analyzers;

	while (pAnalyzer) {
		if (strcmp(pszName, pAnalyzer->an_name) == 0) {
			pAnalyzer->an_function = f;
			return 0;
		}
		pAnalyzer = pAnalyzer->an_next;
	}
	ANALYZER* pNew = ll_insert(&_analyzers, sizeof * pNew);
	strncpy(pNew->an_name, pszName, sizeof pNew->an_name);
	pNew->an_function = f;
	return 1;
}


/*
 * Extract all recommendations from the file edited in the view identified by 'wp'.
 * Use the analyzer with the given analyzer name. If successful, return 1, otherwise 0.
 */
int analyzer_performAnalysis(const char* pszAnalyzerName, WINFO* wp, int (*fMatch)(const char* pszMatch), ANALYZER_CALLBACK fCallback) {
	if (pszAnalyzerName == NULL) {
		return 0;
	}
	ANALYZER* pAnalyzer = _analyzers;

	while (pAnalyzer) {
		if (strcmp(pszAnalyzerName, pAnalyzer->an_name) == 0) {
			pAnalyzer->an_function(wp, fMatch, fCallback);
			return 1;
		}
		pAnalyzer = pAnalyzer->an_next;
	}
	return 0;
}

/*
 * Register some "default" analyzers, which can be referenced in grammar files given their respective names.
 */
void analyzer_registerDefaultAnalyzers() {
	analyzer_register("words", analyzer_extractWords);
	analyzer_register("pks-macros", analyzer_getMacrocCompletions);
	analyzer_register("action-bindings", analyzer_getBindingCompletions);
}

/*
 * Destroy all known analyzers. 
 */
void analyzer_destroyAnalyzers() {
	ll_destroy(&_analyzers, NULL);
}
