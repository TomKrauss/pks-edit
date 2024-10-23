/*
 * CodeAnalyzer.c
 *
 * Project: PKS Edit for Windows
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
#include "linkedlist.h"
#define JSMN_HEADER
#define JSMN_PARENT_LINKS
#include "jsmn.h"
#include "codeanalyzer.h"
#include "jsonparser.h"
#include "documentmodel.h"
#include "winfo.h"
#include "hashmap.h"
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

 /*
  * Returns the mapping rules for the JSON scheme configuration.
  */
extern JSON_MAPPING_RULE* theme_getJsonMapping();

/*
 * Returns the JSON mapping for the copyright_profiles.json config file.
 */
extern JSON_MAPPING_RULE * copyright_getJsonMapping();

/*
 * Returns the JSON mapping for action binding files.
 */
extern JSON_MAPPING_RULE* bindings_getJsonMapping();

extern JSON_ENUM_VALUE* emoji_getSuggestions();

typedef struct tagANALYZER_SCHEMA {
	struct tagANALYZER_SCHEMA* as_next;
	const char * as_fileNamePattern;
	JSON_MAPPING_RULE* as_mappingRules;
} ANALYZER_SCHEMA;

typedef struct tagANALYZER {
	struct tagANALYZER* an_next;
	char an_name[32];
	ANALYZER_CARET_CONTEXT an_sourceContext;
	ANALYZER_GET_CONTEXT an_getCaretContext;
	ANALYZER_FUNCTION an_determineCompletionsFunction;
} ANALYZER;

extern char _eofMarker[];

static ANALYZER *_analyzers;
static ANALYZER_SCHEMA* _analyzerSchemas;

typedef enum JSON_GRAMMAR_STATES {
	JSON_UNKNOWN,
	JSON_KEY,
	JSON_VALUE
} JSON_GRAMMAR_STATES;

/*
 * Calculate the lexical start state at a given line.
 */
extern LEXICAL_CONTEXT highlight_getLexicalStartStateFor(HIGHLIGHTER* pHighlighter, WINFO* wp, LINE* lp);

typedef struct tagANALYZER_PARSING_CONTEXT {
	int apc_grammarState;		// next state for further processing
	int apc_type;				// type of element found
	STRING_BUF* apc_token;
	STRING_BUF* apc_parentToken;
} ANALYZER_PARSING_CONTEXT;

typedef int (*ADVANCE_TOKEN)(ANALYZER_CARET_CONTEXT* pCaretContext, INPUT_STREAM* pStream, ANALYZER_PARSING_CONTEXT *pParsingContext);

typedef enum XML_GRAMMAR_STATES {
	XML_INITIAL,
	XML_ENTITY,
	XML_END_ENTITY,
	XML_COMMENT,
	XML_CDATA,
	XML_ATTRIBUTE,
	XML_SIMPLE_ATTRIBUTE,
	XML_IN_ATTRIBUTE_VALUE,
} XML_GRAMMAR_STATES;

/*
 * Extract all identifiers from a file regardless of comments etc ignoring the file syntax.
 */
static void analyzer_extractTokens(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pCaretContext, 
		void (*calculateSuggestion)(ANALYZER_CARET_CONTEXT* pCaretContext, ANALYZER_PARSING_CONTEXT* pParsingContext, STRING_BUF* pBuf), ADVANCE_TOKEN fAdvance) {
	FTABLE* fp = wp->fp;
	INPUT_STREAM* pStream = streams_createLineInputStream(fp->firstl, 0);
	STRING_BUF* pBuf = stringbuf_create(50);
	STRING_BUF* pParentToken = stringbuf_create(50);
	int c;

	ANALYZER_PARSING_CONTEXT parsingContext = {
		.apc_token = pBuf,
		.apc_parentToken = pParentToken
	};
	while ((c = pStream->is_peekc(pStream, 0)) != 0) {
		fAdvance(pCaretContext, pStream, &parsingContext);
		char* pszWord = stringbuf_getString(pBuf);
		if (*pszWord) {
			if (fMatch(pCaretContext, pszWord)) {
				int nScore = codecomplete_calculateScore(pCaretContext, pszWord);
				if (calculateSuggestion != 0) {
					calculateSuggestion(pCaretContext, &parsingContext, pBuf);
				}
				fCallback(&(ANALYZER_CALLBACK_PARAM) {
					.acp_replacedTextStart = pCaretContext->ac_tokenOffset,
					.acp_replacedTextLength = (int)strlen(pCaretContext->ac_token),
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
	stringbuf_destroy(pParentToken);
}

static int analyzer_checkWord(ANALYZER_CARET_CONTEXT* pCaretContext, INPUT_STREAM* pStream, ANALYZER_PARSING_CONTEXT* pParsingContext) {
	char c;
	BOOL bInWord = FALSE;
	while ((c = pStream->is_getc(pStream)) != 0) {
		if (char_isIdentifier(c)) {
			bInWord = TRUE;
			stringbuf_appendChar(pParsingContext->apc_token, c);
		} else if (bInWord) {
			break;
		}
	}
	return pParsingContext->apc_grammarState;
}

/*
 * Extract all identifiers from a file regardless of comments etc ignoring the file syntax.
 */
static void analyzer_extractWords(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pContext) {
	analyzer_extractTokens(wp, fMatch, fCallback, pContext, 0, analyzer_checkWord);
}

static void analyzer_calculateXmlSuggestion(ANALYZER_CARET_CONTEXT* pCaretContext, ANALYZER_PARSING_CONTEXT* pContext, STRING_BUF* pToken) {
	char entity[128];
	if (pCaretContext->ac_type == XML_INITIAL) {
		stringbuf_insertChar(pToken, 0, '<');
	}
	strncpy(entity, stringbuf_getString(pToken), 128);
	if (pContext->apc_type == XML_END_ENTITY || pContext->apc_type == XML_ENTITY) {
		stringbuf_appendChar(pToken, '>');
		if (pContext->apc_type == XML_ENTITY) {
			stringbuf_appendString(pToken, "${cursor}</");
			stringbuf_appendString(pToken, entity);
			stringbuf_appendChar(pToken, '>');
		} 
	} else if (pContext->apc_type == XML_ATTRIBUTE) {
		stringbuf_appendString(pToken, "=\"${cursor}\"");
	}
}

static BOOL analyzer_isEntityIdentifier(char c) {
	return isalnum(c) || c == '-' || c == ':';
}

static int analyzer_parseXml(ANALYZER_CARET_CONTEXT* pCaretContext, INPUT_STREAM* pStream, ANALYZER_PARSING_CONTEXT* pParsingContext) {
	char c;
	XML_GRAMMAR_STATES grammarState = pParsingContext->apc_grammarState;
	XML_GRAMMAR_STATES caretState = pCaretContext->ac_type;
	STRING_BUF* pToken = pParsingContext->apc_token;
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
				stringbuf_reset(pParsingContext->apc_parentToken);
				grammarState = XML_INITIAL;
			} else if (c == '>') {
				stringbuf_reset(pParsingContext->apc_parentToken);
				grammarState = XML_INITIAL;
			} else if (c == '<') {
				stringbuf_reset(pToken);
				grammarState = XML_ENTITY;
				char nextC = pStream->is_peekc(pStream, 1);
				if (nextC == '/' || nextC == '?') {
					grammarState = XML_END_ENTITY;
					pStream->is_skip(pStream, 1);
				}
			} else if (caretState == XML_ENTITY || caretState == XML_END_ENTITY || caretState == XML_INITIAL) {
				stringbuf_appendChar(pToken, c);
			}
			else if (caretState == XML_ATTRIBUTE) {
				stringbuf_appendChar(pParsingContext->apc_parentToken, c);
			}
			if (grammarState != XML_ENTITY && grammarState != XML_END_ENTITY && stringbuf_size(pToken) > 0) {
				pStream->is_skip(pStream, 1);
				pParsingContext->apc_type = c == '/' ? XML_END_ENTITY : XML_ENTITY;
				pParsingContext->apc_grammarState = grammarState;
				return grammarState;
			}
		} else if (grammarState == XML_ATTRIBUTE) {
			if (c == '=') {
				pStream->is_skip(pStream, 1);
				c = pStream->is_peekc(pStream, 0);
			}
			if (c == '"') {
				grammarState = XML_IN_ATTRIBUTE_VALUE;
			} else if (caretState == XML_ATTRIBUTE &&
				(pCaretContext->ac_token2[0] == 0 || strcmp(pCaretContext->ac_token2, stringbuf_getString(pParsingContext->apc_parentToken)) == 0) &&
				analyzer_isEntityIdentifier(c)) {
				stringbuf_appendChar(pToken, c);
			} else if (c == '/' || c == '?') {
				stringbuf_reset(pParsingContext->apc_parentToken);
				pStream->is_skip(pStream, 1);
				grammarState = XML_INITIAL;
			} else if (c == '>') {
				stringbuf_reset(pParsingContext->apc_parentToken);
				grammarState = XML_INITIAL;
			} else if (c == '<') {
				grammarState = XML_ENTITY;
			}
			if ((c == ' ' || c == '\r' || c == '\n' || c == '\t' || grammarState != XML_ATTRIBUTE) && stringbuf_size(pToken) > 0) {
				pStream->is_skip(pStream, 1);
				pParsingContext->apc_grammarState = grammarState;
				pParsingContext->apc_type = grammarState == XML_IN_ATTRIBUTE_VALUE ? XML_ATTRIBUTE : XML_SIMPLE_ATTRIBUTE;
				return grammarState;
			}
		} else if (grammarState == XML_IN_ATTRIBUTE_VALUE) {
			if (c == '"' || c == '\r' || c == '\n') {
				grammarState = XML_ATTRIBUTE;
			} else if (caretState == XML_IN_ATTRIBUTE_VALUE) {
				stringbuf_appendChar(pToken, c);
			}
			if (grammarState != XML_IN_ATTRIBUTE_VALUE && stringbuf_size(pToken) > 0) {
				pStream->is_skip(pStream, 1);
				pParsingContext->apc_grammarState = XML_ATTRIBUTE;
				pParsingContext->apc_grammarState = grammarState;
				return grammarState;
			}
		}
		pStream->is_skip(pStream, 1);
	}
	pParsingContext->apc_type = XML_INITIAL;
	pParsingContext->apc_grammarState = XML_INITIAL;
	return XML_INITIAL;
}

static void analyzer_getEntityNameBackward(char* pszDest, char* pBuf, int nStartPos) {
	int nEnd = nStartPos;
	char* pEnd = pszDest + 126;
	while (nStartPos > 0) {
		if (pBuf[nStartPos - 1] == '<') {
			while (nStartPos < nEnd) {
				if (pszDest < pEnd && analyzer_isEntityIdentifier(pBuf[nStartPos])) {
					*pszDest++ = pBuf[nStartPos];
					nStartPos++;
				} else {
					break;
				}
			}
			*pszDest = 0;
			return;
		} else {
			nStartPos--;
		}
	}
}

static void analyzer_fillXmlTokenType(ANALYZER_CARET_CONTEXT* pCaretContext) {
	switch (pCaretContext->ac_type) {
	case XML_ATTRIBUTE: strcpy(pCaretContext->ac_tokenTypeName, "attribute"); break;
	case XML_END_ENTITY: strcpy(pCaretContext->ac_tokenTypeName, "end-entity"); break;
	case XML_ENTITY: strcpy(pCaretContext->ac_tokenTypeName, "entity"); break;
	default: pCaretContext->ac_tokenTypeName[0] = 0; break;
	}
}

static void analyzer_getXmlCaretContext(WINFO* wp, ANALYZER_CARET_CONTEXT* pCaretContext) {
	int nPos = wp->caret.offset;
	char* pBuf = wp->caret.linePointer->lbuf;
	pCaretContext->ac_type = XML_ATTRIBUTE;
	while (--nPos >= 0) {
		unsigned char c = pBuf[nPos];
		if (!analyzer_isEntityIdentifier(c)) {
			if (c == '<') {
				pCaretContext->ac_type = XML_ENTITY;
			}
			else if (c == '/') {
				pCaretContext->ac_type = XML_END_ENTITY;
			}
			else if (c == '"' && (nPos == 0 || pBuf[nPos - 1] == '=')) {
				pCaretContext->ac_type = XML_IN_ATTRIBUTE_VALUE;
			}
			break;
		}
	}
	pCaretContext->ac_token2[0] = 0;
	if (pCaretContext->ac_type == XML_ATTRIBUTE) {
		analyzer_getEntityNameBackward(pCaretContext->ac_token2, pBuf, nPos);
		if (pCaretContext->ac_token2[0] == 0) {
			pCaretContext->ac_type = XML_INITIAL;
		}
	}
	nPos++;
	int nOffset;
	for (nOffset = wp->caret.offset; nOffset < wp->caret.linePointer->len; nOffset++) {
		if (!analyzer_isEntityIdentifier(pBuf[nOffset])) {
			break;
		}
	}
	int nSize = nOffset - nPos;
	if (nSize > sizeof pCaretContext->ac_token - 1) {
		nSize = sizeof pCaretContext->ac_token - 1;
	}
	strncpy(pCaretContext->ac_token, &pBuf[nPos], nSize);
	pCaretContext->ac_token[nSize] = 0;
	pCaretContext->ac_tokenOffset = nPos;
	analyzer_fillXmlTokenType(pCaretContext);
}

/*
 * Extract all entity names and attributes from an XML document.
 */
static void analyzer_extractXmlElements(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pContext) {
	analyzer_extractTokens(wp, fMatch, fCallback, pContext, analyzer_calculateXmlSuggestion, analyzer_parseXml);
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
static void analyzer_getMacrocCompletions(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pContext) {
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
	szFunction[0] = 0;
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
static char* _pszKeyCompare;
static void analyzer_addKeycode(const char* pszKeycode) {
	if (strstr(pszKeycode, _pszKeyCompare) == pszKeycode) {
		_keyAnalyzerCallback(&(ANALYZER_CALLBACK_PARAM) { .acp_recommendation = pszKeycode });
	}
}

/*
 * Returns a possible code completion in a PKS actionbinding file - either the name of a PKS command, the name of a fontawesome icon
 * or the name of a key.
 */
static void analyzer_getBindingCompletions(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pCaretContext) {
	if (pCaretContext->ac_type != JSON_VALUE) {
		return;
	}
	if (strcmp(pCaretContext->ac_token2, "key") == 0) {
		_pszKeyCompare = pCaretContext->ac_tokenStart;
		char* pPlus = strrchr(_pszKeyCompare, '+');
		if (pPlus != NULL) {
			_pszKeyCompare = pPlus+1;
		}
		_keyAnalyzerCallback = fCallback;
		bindings_addModifiersAndKeycodes(analyzer_addKeycode);
		return;
	}
	if (strcmp(pCaretContext->ac_token2, "command") != 0) {
		return;
	}
	const char* pszWord;
	for (int i = 0; (pszWord = macro_getCommandByIndex(i)) != 0; i++) {
		if (fMatch(pCaretContext, pszWord)) {
			MACROREF macref = (MACROREF){ .typ = CMD_CMDSEQ, .index = i };
			fCallback(&(ANALYZER_CALLBACK_PARAM) {
				.acp_recommendation = pszWord,
					.acp_object = (void*)MACROREF_TO_INTPTR(macref),
					.acp_help = analyzer_helpForCommand,
					.acp_getHyperlinkText = macrodoc_helpForHyperlink
			});
		}
	}
}

/*
 * Returns the possible code completion in a markdown file.
 */
static void analyzer_getMarkdownCompletions(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pCaretContext) {
	if (pCaretContext->ac_token[0] == ':') {
		JSON_ENUM_VALUE* pValues = emoji_getSuggestions();
		if (pValues != NULL) {
			for (int i = 0; pValues[i].jev_name; i++) {
				const char* pString = pValues[i].jev_name;
				if (fMatch(pCaretContext, pString)) {
					fCallback(&(ANALYZER_CALLBACK_PARAM) {
						.acp_replacedTextStart = pCaretContext->ac_tokenOffset,
							.acp_replacedTextLength = (int)strlen(pCaretContext->ac_token),
							.acp_recommendation = pString,
							.acp_score = codecomplete_calculateScore(pCaretContext, pString),
							.acp_icon = pValues[i].jev_icon
					});
				}
			}
			free(pValues);
		}
		return;
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
			pAnalyzer->an_determineCompletionsFunction = f;
			pAnalyzer->an_getCaretContext = fGetContext;
			return 0;
		}
		pAnalyzer = pAnalyzer->an_next;
	}
	ANALYZER* pNew = ll_insert(&_analyzers, sizeof * pNew);
	strncpy(pNew->an_name, pszName, sizeof pNew->an_name);
	pNew->an_determineCompletionsFunction = f;
	pNew->an_getCaretContext = fGetContext;
	return 1;
}

static int analyzer_matchContext(ANALYZER_CARET_CONTEXT* pContext, const char* pszWord) {
	// add all words to the completion list, which are not identical to the word searched, but where the word
	// searched / completed is a substring.
	char* pszCompare = pContext->ac_tokenStart[0] ? pContext->ac_tokenStart : pContext->ac_token;
	return strcmp(pszWord, pszCompare) != 0 && string_strcasestr(pszWord, pszCompare) != NULL;
}

static analyzer_getDefaultCaretContext(WINFO* wp, ANALYZER_CARET_CONTEXT* pContext) {
	xref_findIdentifierCloseToCaret(wp, &wp->caret, 
		pContext->ac_token, pContext->ac_token + sizeof pContext->ac_token, NULL, NULL, FI_BEGIN_WORD_TO_CURSOR);
}

static int analyzer_matchMarkdownToken(unsigned char c) {
	return c == ':' || c == '`' || isident(c);
}

static analyzer_getMarkdownCaretContext(WINFO* wp, ANALYZER_CARET_CONTEXT* pContext) {
	xref_findMatchingCloseToCaret(wp, &wp->caret,
		pContext->ac_token, pContext->ac_token + sizeof pContext->ac_token, NULL, NULL, analyzer_matchMarkdownToken, FI_BEGIN_WORD_TO_CURSOR);
}

/*
 * Calculate the caret context for the given analyzer name and caret position from the window in which a code
 * completion is request. Return TRUE, if successful.
 */
BOOL analyzer_getCaretContext(const char* pszAnalyzerName, WINFO* wp, ANALYZER_CARET_CONTEXT* pCaretContext) {
	memset(pCaretContext, 0, sizeof * pCaretContext);
	if (pszAnalyzerName != NULL) {
		ANALYZER* pAnalyzer = _analyzers;
		while (pAnalyzer) {
			if (strstr(pszAnalyzerName, pAnalyzer->an_name) != 0) {
				pAnalyzer->an_getCaretContext(wp, pCaretContext);
				return TRUE;
			}
			pAnalyzer = pAnalyzer->an_next;
		}
	}
	xref_getSelectedIdentifier(wp, pCaretContext->ac_token, sizeof pCaretContext->ac_token, &pCaretContext->ac_tokenOffset);
	strcpy(pCaretContext->ac_tokenStart, pCaretContext->ac_token);
	int nOffset = wp->caret.offset - pCaretContext->ac_tokenOffset;
	if (nOffset > 0) {
		pCaretContext->ac_tokenStart[nOffset] = 0;
	}
	return FALSE;
}
/*
 * Extract all recommendations from the file edited in the view identified by 'wp'.
 * Use the analyzer with the given analyzer name. If successful, return 1, otherwise 0.
 */
int analyzer_performAnalysis(const char* pszAnalyzerName, WINFO* wp, ANALYZER_CARET_CONTEXT* pCaretContext, ANALYZER_CALLBACK fCallback) {
	if (pszAnalyzerName == NULL) {
		return 0;
	}
	ANALYZER* pAnalyzer = _analyzers;
	int ret = 0;

	while (pAnalyzer) {
		if (strstr(pszAnalyzerName, pAnalyzer->an_name) != 0) {
			pAnalyzer->an_determineCompletionsFunction(wp, analyzer_matchContext, fCallback, pCaretContext);
			ret = 1;
		}
		pAnalyzer = pAnalyzer->an_next;
	}
	return ret;
}

static void analyzer_getTokenContents(char* pszDestination, const char* pMem, jsmntok_t* pToken, size_t nMaxLen) {
	char szToken[MAX_TOKEN_SIZE];
	json_tokenContents(pMem, pToken, szToken);
	strncpy(pszDestination, szToken, nMaxLen);
	pszDestination[nMaxLen] = 0;
}

static void analyzer_getJsonCaretContext(WINFO* wp, ANALYZER_CARET_CONTEXT* pCaretContext) {
	FTABLE* fp = FTPOI(wp);
	size_t nSize = 0;
	char* pMem = ft_convertToBuffer(fp, NULL, &nSize, fp->firstl);
	pCaretContext->ac_type = JSON_UNKNOWN;
	strcpy(pCaretContext->ac_tokenTypeName, "unknown");
	if (pMem) {
		int numberOfTokens = 0;
		jsmntok_t* tokens = json_parseMemory(pMem, nSize, &numberOfTokens);
		if (tokens != NULL) {
			size_t offset = ln_caretOffset(fp, &wp->caret);
			int nFound = -1;
			for (int i = 0; i < numberOfTokens; i++) {
				if (tokens[i].start <= offset && tokens[i].end >= offset) {
					nFound = i;
				}
				if (tokens[i].start > offset) {
					break;
				}
			}
			if (nFound > 0 && (tokens[nFound].type == JSMN_STRING || tokens[nFound].type == JSMN_PRIMITIVE)) {
				if (pMem[tokens[nFound].end + 1] == ':') {
					analyzer_getTokenContents(pCaretContext->ac_token, pMem, &tokens[nFound], sizeof(pCaretContext->ac_token) - 1);
					strcpy(pCaretContext->ac_tokenStart, pCaretContext->ac_token);
					pCaretContext->ac_tokenStart[offset - tokens[nFound].start] = 0;
					pCaretContext->ac_tokenOffset = wp->caret.offset + (int)(tokens[nFound].start - offset);
					pCaretContext->ac_type = JSON_KEY;
					strcpy(pCaretContext->ac_tokenTypeName, "key");
					int nTakeIt = tokens[nFound].parent;
					while (nTakeIt > 0) {
						jsmntok_t* pTok = &tokens[nTakeIt];
						if ((pTok->type == JSMN_OBJECT || pTok->type == JSMN_ARRAY) && (pTok - 1)->type == JSMN_STRING) {
							int nParent = (pTok - 1)->parent;
							if (nParent < 0 || tokens[nParent].end > offset) {
								break;
							}
						}
						nTakeIt = pTok->parent;
					}
					if (nTakeIt > 0) {
						analyzer_getTokenContents(pCaretContext->ac_token2, pMem, &tokens[nTakeIt - 1], sizeof(pCaretContext->ac_token2) - 1);
					}
				} else {
					if (tokens[nFound].type == JSMN_STRING) {
						analyzer_getTokenContents(pCaretContext->ac_token, pMem, &tokens[nFound], sizeof(pCaretContext->ac_token) - 1);
						strcpy(pCaretContext->ac_tokenStart, pCaretContext->ac_token);
						pCaretContext->ac_tokenStart[offset - tokens[nFound].start] = 0;
					}
					if (nFound > 0 && tokens[nFound - 1].type == JSMN_STRING) {
						analyzer_getTokenContents(pCaretContext->ac_token2, pMem, &tokens[nFound - 1], sizeof(pCaretContext->ac_token2) - 1);
					}
					pCaretContext->ac_tokenOffset = wp->caret.offset + (int)(tokens[nFound].start - offset);
					pCaretContext->ac_type = JSON_VALUE;
					strcpy(pCaretContext->ac_tokenTypeName, "value");
				}
			}
			free(tokens);
		}
		free(pMem);
	}
}

/*
 * Given the name of the parent JSON node (pszName), try to find the list of mapping rules defining the object
 * define via pszName. nDepth is passed as a simple mechanism to avoid endless recursion.
 */
static JSON_MAPPING_RULE* analyzer_findJsonRules(JSON_MAPPING_RULE* pRules, char* pszName, BOOL bFindObject, int nDepth) {
	if (++nDepth > 10) {
		return NULL;
	}
	while (pRules->r_type != RT_END) {
		if (!bFindObject && strcmp(pszName, pRules->r_name) == 0) {
			return pRules;
		}
		if (pRules->r_type == RT_OBJECT_LIST) {
			JSON_MAPPING_RULE* pNested = pRules->r_descriptor.r_t_arrayDescriptor.ro_nestedRules;
			if (pNested && pNested != pRules) {
				if (strcmp(pszName, pRules->r_name) == 0) {
					return pNested;
				}
				pNested = analyzer_findJsonRules(pNested, pszName, bFindObject, nDepth);
				if (pNested != NULL) {
					return pNested;
				}
			}
		}
		pRules++;
	}
	return NULL;
}

static char* analyzer_provideHelpForMappingRule(const char* pszRule, JSON_MAPPING_RULE* pRule) {
	if (pRule->r_description) {
		return _strdup(pRule->r_description);
	}
	return NULL;
}

static char* analyzer_provideHelpForString(const char* pszRule, char* pString) {
	if (pString) {
		return _strdup(pString);
	}
	return NULL;
}

static void analyzer_provideJsonKeySuggestions(JSON_MAPPING_RULE* pRules, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pContext) {
	pRules = analyzer_findJsonRules(pRules, pContext->ac_token2, TRUE, 0);
	while (pRules && pRules->r_type != RT_END) {
		if (fMatch(pContext, pRules->r_name)) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) {
				.acp_replacedTextStart = pContext->ac_tokenOffset,
				.acp_replacedTextLength = (int)strlen(pContext->ac_token),
				.acp_score = codecomplete_calculateScore(pContext, pRules->r_name),
				.acp_recommendation = pRules->r_name,
				.acp_object = (void*)pRules,
				.acp_help = analyzer_provideHelpForMappingRule
			});
		}
		pRules++;
	}
}

static void analyzer_provideJsonValueSuggestions(JSON_MAPPING_RULE* pRules, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pContext) {
	pRules = analyzer_findJsonRules(pRules, pContext->ac_token2, FALSE, 0);
	if (pRules != NULL) {
		if (pRules->r_type == RT_COLOR) {
			for (int i = 0; _cssColors[i].cc_name != NULL; i++) {
				if (fMatch(pContext, _cssColors[i].cc_name)) {
					fCallback(&(ANALYZER_CALLBACK_PARAM) {
						.acp_replacedTextStart = pContext->ac_tokenOffset,
							.acp_replacedTextLength = (int)strlen(pContext->ac_token),
							.acp_recommendation = _cssColors[i].cc_name,
							.acp_object = (void*)pRules,
							.acp_help = analyzer_provideHelpForMappingRule,
							.acp_score = codecomplete_calculateScore(pContext, _cssColors[i].cc_name)+50,
							.acp_icon = {
							.cai_iconType = CAI_COLOR_ICON,
							.cai_data.cai_color = _cssColors[i].cc_color
						}
					});
				}
			}
		}
		if (pRules->r_valueProvider != NULL) {
			JSON_ENUM_VALUE* pValues = pRules->r_valueProvider();
			if (pValues != NULL) {
				for (int i = 0; pValues[i].jev_name; i++) {
					const char* pString = pValues[i].jev_name;
					if (fMatch(pContext, pString)) {
						fCallback(&(ANALYZER_CALLBACK_PARAM) {
							.acp_replacedTextStart = pContext->ac_tokenOffset,
								.acp_replacedTextLength = (int)strlen(pContext->ac_token),
								.acp_recommendation = pString,
								.acp_object = (void*)pValues[i].jev_description,
								.acp_help = analyzer_provideHelpForString,
								.acp_score = codecomplete_calculateScore(pContext, pString)+50,
								.acp_icon = pValues[i].jev_icon
						});
					}
				}
				free(pValues);
			}
		}
	}
}

static void analyzer_calculateJsonSuggestions(WINFO* wp, ANALYZER_MATCH fMatch, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT* pContext) {
	char szFilename[256];
	FTABLE* fp = FTPOI(wp);
	strcpy(szFilename, string_getBaseFilename(fp->fname));
	ANALYZER_SCHEMA* pSchemas = _analyzerSchemas;
	while (pSchemas) {
		if (string_matchFilename(szFilename, pSchemas->as_fileNamePattern)) {
			if (pContext->ac_type == JSON_KEY) {
				analyzer_provideJsonKeySuggestions(pSchemas->as_mappingRules, fMatch, fCallback, pContext);
			} else if (pContext->ac_type == JSON_VALUE) {
				analyzer_provideJsonValueSuggestions(pSchemas->as_mappingRules, fMatch, fCallback, pContext);
			}
			break;
		}
		pSchemas = pSchemas->as_next;
	}
}

static void analyzer_registerSchema(char* pszFilePattern, JSON_MAPPING_RULE* pRule) {
	ANALYZER_SCHEMA* pSchemas = _analyzerSchemas;

	ANALYZER_SCHEMA* pNew = ll_insert(&_analyzerSchemas, sizeof * pNew);
	pNew->as_fileNamePattern = pszFilePattern;
	pNew->as_mappingRules = pRule;
}

/*
 * Register some "default" analyzers, which can be referenced in grammar files given their respective names.
 */
void analyzer_registerDefaultAnalyzers() {
	analyzer_register("words", analyzer_extractWords, analyzer_getDefaultCaretContext);
	analyzer_register("xml", analyzer_extractXmlElements, analyzer_getXmlCaretContext);
	analyzer_register("json", analyzer_calculateJsonSuggestions, analyzer_getJsonCaretContext);
	analyzer_register("pks-macros", analyzer_getMacrocCompletions, analyzer_getDefaultCaretContext);
	analyzer_register("action-bindings", analyzer_getBindingCompletions, analyzer_getJsonCaretContext);
	analyzer_register("markdown", analyzer_getMarkdownCompletions, analyzer_getMarkdownCaretContext);

	analyzer_registerSchema("*.grammar.json", grammar_getJsonMapping());
	analyzer_registerSchema("pkseditconfig.json", doctypes_getJsonMapping());
	analyzer_registerSchema("pksactionbindings.json", bindings_getJsonMapping());
	analyzer_registerSchema("themeconfig.json", theme_getJsonMapping());
	analyzer_registerSchema("copyright_profiles.json", copyright_getJsonMapping());
}

/*
 * Destroy all known analyzers. 
 */
void analyzer_destroyAnalyzers() {
	ll_destroy(&_analyzers, NULL);
	ll_destroy(&_analyzerSchemas, NULL);
}
