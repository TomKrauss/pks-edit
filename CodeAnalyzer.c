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
#include "edctype.h"
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
#include "streams.h"
#include "crossreferencelinks.h"

typedef struct tagANALYZER {
	struct tagANALYZER* an_next;
	char an_name[32];
	ANALYZER_CONTEXT an_sourceContext;
	ANALYZER_GET_CONTEXT an_getContext;
	ANALYZER_FUNCTION an_function;
} ANALYZER;

static ANALYZER *_analyzers;

/*
 * Calculate the lexical start state at a given line.
 */
extern LEXICAL_CONTEXT highlight_getLexicalStartStateFor(HIGHLIGHTER* pHighlighter, WINFO* wp, LINE* lp);

typedef int (*ADVANCE_TOKEN)(ANALYZER_CONTEXT* pContext, int grammarState, INPUT_STREAM* pStream, STRING_BUF* pToken);

enum XML_GRAMMAR_STATES {
	XML_INITIAL,
	XML_ENTITY,
	XML_END_ENTITY,
	XML_COMMENT,
	XML_CDATA,
	XML_ATTRIBUTE,
	XML_IN_ATTRIBUTE,
};

/*
 * Extract all identifiers from a file regardless of comments etc ignoring the file syntax.
 */
static void analyzer_extractTokens(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CONTEXT* pContext, 
		void (*calculateSuggestion)(ANALYZER_CONTEXT* pContext, STRING_BUF* pBuf), ADVANCE_TOKEN fAdvance) {
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	STRING_BUF* pBuf = stringbuf_create(50);
	int grammarState = 0;
	int c;

	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		grammarState = fAdvance(pContext, grammarState, pStream, pBuf);
		char* pszWord = stringbuf_getString(pBuf);
		if (*pszWord) {
			if (fMatch(pContext, pszWord)) {
				int nScore = codecomplete_calculateScore(pContext->ac_token, pszWord);
				if (calculateSuggestion != 0) {
					calculateSuggestion(pContext, pBuf);
				}
				fCallback(&(ANALYZER_CALLBACK_PARAM) {
					.acp_replacedTextLength = (int)strlen(pContext->ac_token),
					.acp_recommendation = stringbuf_getString(pBuf),
					.acp_score = nScore
				});
			}
			stringbuf_reset(pBuf);
		} else {
			pStream->is_skip(pStream, 1);
		}
	}
	pStream->is_destroy(pStream);
	stringbuf_destroy(pBuf);
}

static int analyzer_checkWord(ANALYZER_CONTEXT* pContext, int grammarState, INPUT_STREAM* pStream, STRING_BUF* pToken) {
	char c;
	BOOL bInWord = FALSE;
	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		if (char_isIdentifier(c)) {
			bInWord = TRUE;
			stringbuf_appendChar(pToken, c);
		} else if (bInWord) {
			break;
		}
	}
	return grammarState;
}

/*
 * Extract all identifiers from a file regardless of comments etc ignoring the file syntax.
 */
static void analyzer_extractWords(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CONTEXT* pContext) {
	analyzer_extractTokens(wp, fMatch, fCallback, pContext, 0, analyzer_checkWord);
}

static void analyzer_calculateXmlSuggestion(ANALYZER_CONTEXT* pContext, STRING_BUF* pToken) {
	char entity[128];
	strncpy(entity, stringbuf_getString(pToken), 128);
	if (pContext->ac_type == XML_END_ENTITY || pContext->ac_type == XML_ENTITY) {
		stringbuf_appendChar(pToken, '>');
		if (pContext->ac_type == XML_ENTITY) {
			stringbuf_appendString(pToken, "${cursor}</");
			stringbuf_appendString(pToken, entity);
			stringbuf_appendChar(pToken, '>');
		}
	}
}

static int analyzer_checkXml(ANALYZER_CONTEXT* pContext, int grammarState, INPUT_STREAM* pStream, STRING_BUF* pToken) {
	char c;
	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		if (grammarState == XML_COMMENT) {
			if (c == '-' && pStream->is_strncmp(pStream, "-->", 3) == 0) {
				pStream->is_skip(pStream, 3);
				grammarState = XML_INITIAL;
			}
		} else if (grammarState == XML_CDATA) {
			if (pStream->is_strncmp(pStream, "]]>", 3) == 0) {
				pStream->is_skip(pStream, 3);
				grammarState = XML_INITIAL;
			}
		} else if (grammarState == XML_INITIAL) {
			if (c == '<') {
				if (pStream->is_strncmp(pStream, "<!--", 4) == 0) {
					pStream->is_skip(pStream, 4);
					grammarState = XML_COMMENT;
				} else if (pStream->is_strncmp(pStream, "<![CDATA[", 9) == 0) {
					pStream->is_skip(pStream, 9);
					grammarState = XML_CDATA;
				} else {
					if (pStream->is_peekc(pStream, 1) == '/') {
						pStream->is_skip(pStream, 1);
						grammarState = XML_END_ENTITY;
					} else {
						grammarState = XML_ENTITY;
					}
				}
			}
		} else if (grammarState == XML_ENTITY || grammarState == XML_END_ENTITY) {
			if (c == ' ' || c == '\r' || c == '\n' || c == '\t') {
				grammarState = XML_ATTRIBUTE;
			} else if (c == '/') {
				pStream->is_skip(pStream, 1);
				grammarState = XML_INITIAL;
			}
			else if (c == '>') {
				grammarState = XML_INITIAL;
			} else if (c == '<') {
				stringbuf_reset(pToken);
				grammarState = XML_ENTITY;
				char nextC = pStream->is_peekc(pStream, 1);
				if (nextC == '/' || nextC == '?') {
					grammarState = XML_END_ENTITY;
					pStream->is_skip(pStream, 1);
				}
			} else if (pContext->ac_type == XML_ENTITY || pContext->ac_type == XML_END_ENTITY) {
				stringbuf_appendChar(pToken, c);
			}
			if (grammarState != XML_ENTITY && stringbuf_size(pToken) > 0) {
				pStream->is_skip(pStream, 1);
				return grammarState;
			}
		} else if (grammarState == XML_ATTRIBUTE) {
			if (c == '=') {
				pStream->is_skip(pStream, 1);
				c = pStream->is_peekc(pStream, 0);
			}
			if (c == '"') {
				grammarState = XML_IN_ATTRIBUTE;
			} else if (pContext->ac_type == XML_ATTRIBUTE && (isalnum(c) || c == '-')) {
				stringbuf_appendChar(pToken, c);
			} else if (c == '/' || c == '?') {
				pStream->is_skip(pStream, 1);
				grammarState = XML_INITIAL;
			} else if (c == '>') {
				grammarState = XML_INITIAL;
			} else if (c == '<') {
				grammarState = XML_ENTITY;
			}
			if ((c == ' ' || c == '\r' || c == '\n' || c == '\t' || grammarState != XML_ATTRIBUTE) && stringbuf_size(pToken) > 0) {
				pStream->is_skip(pStream, 1);
				return grammarState;
			}
		} else if (grammarState == XML_IN_ATTRIBUTE) {
			if (c == '"' || c == '\r' || c == '\n') {
				grammarState = XML_ATTRIBUTE;
			} else if (pContext->ac_type == XML_IN_ATTRIBUTE) {
				stringbuf_appendChar(pToken, c);
			}
			if (grammarState != XML_IN_ATTRIBUTE && stringbuf_size(pToken) > 0) {
				pStream->is_skip(pStream, 1);
				return grammarState;
			}
		}
		pStream->is_skip(pStream, 1);
	}
	return XML_INITIAL;
}

static void analyzer_getXmlContext(WINFO* wp, ANALYZER_CONTEXT* pContext) {
	int nPos = wp->caret.offset;
	char* pBuf = wp->caret.linePointer->lbuf;
	pContext->ac_type = XML_ATTRIBUTE;
	while (--nPos >= 0) {
		unsigned char c = pBuf[nPos];
		if (c != '-' && c != ':' && !isident(c)) {
			if (c == '<') {
				pContext->ac_type = XML_ENTITY;
			} else if (c == '/') {
				pContext->ac_type = XML_END_ENTITY;
			} else if (c == '"' && (nPos == 0 || pBuf[nPos-1] == '=')) {
				pContext->ac_type = XML_IN_ATTRIBUTE;
			}
			break;
		}
	}
	nPos++;
	int nSize = wp->caret.offset - nPos;
	if (nSize > sizeof pContext->ac_token - 2) {
		nSize = sizeof pContext->ac_token - 2;
	}
	strncpy(pContext->ac_token, &pBuf[nPos], nSize);
	pContext->ac_token[nSize] = 0;
}

/*
 * Extract all entity names and attributes from an XML document.
 */
static void analyzer_extractTagsAndAttributes(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CONTEXT* pContext) {
	analyzer_extractTokens(wp, fMatch, fCallback, pContext, analyzer_calculateXmlSuggestion, analyzer_checkXml);
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
static void analyzer_getMacrocCompletions(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CONTEXT* pContext) {
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
	PKS_VALUE_TYPE vtEnum = 0;
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
				vtEnum = ptd.pt_valueType;
				for (int i = 0; i < ptd.pt_enumCount; i++) {
					PARAMETER_ENUM_VALUE* pValue = &ptd.pt_enumVal[i];
					const char* pszName = pValue->pev_name;
					if ((bInIdent && fMatch(pContext, pszName)) || nPar == nParamIndex || (bEditorParam && (nPar - 1 == nParamIndex))) {
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
	for (PKS_VALUE_TYPE vt = VT_MAP_ENTRY; ; vt++) {
		if (!types_existsType(vt)) {
			break;
		}
		if (vt == vtEnum) {
			continue;
		}
		PKS_TYPE_DESCRIPTOR* pDescriptor = types_getTypeDescriptor(types_nameFor(vt));
		if (!pDescriptor || !pDescriptor->ptd_isEnumType) {
			continue;
		}
		for (int i = 0; i < pDescriptor->ptd_numberOfProperties; i++) {
			PARAMETER_ENUM_VALUE* pValue = &pDescriptor->ptd_elements.ptd_enumValues[i];
			const char* pszName = pValue->pev_name;
			if (fMatch(pContext, pszName)) {
				fCallback(&(ANALYZER_CALLBACK_PARAM) {
					.acp_recommendation = pszName,
						.acp_object = pValue,
						.acp_help = macrodoc_helpForEnumValue
				});
			}
		}
	}
	for (int i = 0; i < _functionTableSize; i++) {
		NATIVE_FUNCTION* pFunc = &_functionTable[i];
		if (fMatch(pContext, pFunc->nf_name)) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) { .acp_recommendation = pFunc->nf_name, 
				.acp_help = macrodoc_helpForNativeFunction,
				.acp_object= pFunc,
				.acp_getHyperlinkText = macrodoc_helpForHyperlink
			});
		}
	}
	for (PKS_VALUE_TYPE vt = VT_NIL; types_existsType(vt); vt++) {
		const char* pName = types_nameFor(vt);
		if (fMatch(pContext, pName)) {
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
		if (mp && fMatch(pContext, MAC_NAME(mp))) {
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
static void analyzer_getBindingCompletions(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CONTEXT* pContext) {
	if (wp->caret.offset > 0 && wp->caret.linePointer->lbuf[wp->caret.offset - 1] == '+') {
		_keyAnalyzerCallback = fCallback;
		bindings_addModifiersAndKeycodes(analyzer_addKeycode);
		return;
	}
	const char* pszWord;
	for (int i = 0; (pszWord = macro_getCommandByIndex(i)) != 0; i++) {
		if (fMatch(pContext, pszWord)) {
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
		if (fMatch(pContext, pszWord)) {
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
int analyzer_register(const char* pszName, ANALYZER_FUNCTION f, ANALYZER_GET_CONTEXT fGetContext) {
	ANALYZER* pAnalyzer = _analyzers;

	while (pAnalyzer) {
		if (strcmp(pszName, pAnalyzer->an_name) == 0) {
			pAnalyzer->an_function = f;
			pAnalyzer->an_getContext = fGetContext;
			return 0;
		}
		pAnalyzer = pAnalyzer->an_next;
	}
	ANALYZER* pNew = ll_insert(&_analyzers, sizeof * pNew);
	strncpy(pNew->an_name, pszName, sizeof pNew->an_name);
	pNew->an_function = f;
	pNew->an_getContext = fGetContext;
	return 1;
}

static int analyzer_matchContext(ANALYZER_CONTEXT* pContext, const char* pszWord) {
	// add all words to the completion list, which are not identical to the word searched, but where the word
	// searched / completed is a substring.
	return strcmp(pszWord, pContext->ac_token) != 0 && string_strcasestr(pszWord, pContext->ac_token) != NULL;
}

static analyzer_getDefaultContext(WINFO* wp, ANALYZER_CONTEXT* pContext) {
	xref_findIdentifierCloseToCaret(wp, &wp->caret, 
		pContext->ac_token, pContext->ac_token + sizeof pContext->ac_token, NULL, NULL, FI_BEGIN_WORD_TO_CURSOR);
}

/*
 * Extract all recommendations from the file edited in the view identified by 'wp'.
 * Use the analyzer with the given analyzer name. If successful, return 1, otherwise 0.
 */
int analyzer_performAnalysis(const char* pszAnalyzerName, WINFO* wp, ANALYZER_CALLBACK fCallback) {
	if (pszAnalyzerName == NULL) {
		return 0;
	}
	ANALYZER* pAnalyzer = _analyzers;

	while (pAnalyzer) {
		if (strcmp(pszAnalyzerName, pAnalyzer->an_name) == 0) {
			ANALYZER_CONTEXT context = { 0 };
			pAnalyzer->an_getContext(wp, &context);
			pAnalyzer->an_function(wp, analyzer_matchContext, fCallback, &context);
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
	analyzer_register("words", analyzer_extractWords, analyzer_getDefaultContext);
	analyzer_register("xml", analyzer_extractTagsAndAttributes, analyzer_getXmlContext);
	analyzer_register("pks-macros", analyzer_getMacrocCompletions, analyzer_getDefaultContext);
	analyzer_register("action-bindings", analyzer_getBindingCompletions, analyzer_getDefaultContext);
}

/*
 * Destroy all known analyzers. 
 */
void analyzer_destroyAnalyzers() {
	ll_destroy(&_analyzers, NULL);
}
