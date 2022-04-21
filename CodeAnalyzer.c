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
							fCallback(pszWord, NULL, NULL);
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

static const char* analyzer_pksTypeFromParamtype(PARAMETER_TYPE_DESCRIPTOR pt) {
	if (pt.pt_type == PARAM_TYPE_EDITOR_WINDOW) {
		return types_nameFor(VT_EDITOR_HANDLE);
	}
	if (pt.pt_type == PARAM_TYPE_STRING) {
		return types_nameFor(VT_STRING);
	}
	if (pt.pt_type == PARAM_TYPE_STRING_ARRAY) {
		return "string[]";
	}
	if (pt.pt_enumVal) {
		static char szType[32];
		const char* pszEnumName = pt.pt_enumVal->pev_name;
		char* pszUnder = strchr(pszEnumName, '_');
		if (pszUnder) {
			size_t nCount = pszUnder - pszEnumName+1;
			memcpy(szType, pszEnumName, nCount);
			strcpy(szType+nCount, pt.pt_type == PARAM_TYPE_BITSET ? "FLAGS" : "ENUM");
			return szType;
		}
	}
	return types_nameFor(VT_NUMBER);
}

static void analyzer_startHelpSection(STRING_BUF* pBuf, const char* pszName) {
	stringbuf_appendString(pBuf, "<p><b>");
	stringbuf_appendString(pBuf, pszName);
	stringbuf_appendString(pBuf, "</b>:<br>");
}

static analyzer_formatJavadocComment(STRING_BUF* pBuf, const char* pszInput) {
	char szTag[64];
	char* pszTag = 0;
	BOOL bParamsFound = 0;
	BOOL bHighlightWord = 0;
	char c;

	analyzer_startHelpSection(pBuf, "Description");
	while ((c = *pszInput++) != 0) {
		if (!pszTag) {
			if (c == '@') {
				pszTag = szTag;
				bHighlightWord = 0;
			}
			else {
				if (bHighlightWord == 1 && isalpha(c)) {
					stringbuf_appendString(pBuf, "<em>");
					bHighlightWord = 2;
				}
				else if (bHighlightWord == 2 && !isalpha(c)) {
					stringbuf_appendString(pBuf, "</em> - ");
					bHighlightWord = 0;
				}
				stringbuf_appendChar(pBuf, c);
			}
		}
		else if (pszTag) {
			if (!isalpha(c)) {
				*pszTag = 0;
				pszTag = 0;
				if (strcmp("param", szTag) == 0) {
					if (!bParamsFound) {
						stringbuf_appendString(pBuf, "</p>");
						analyzer_startHelpSection(pBuf, "Parameters");
						bParamsFound = 1;
					} else {
						stringbuf_appendString(pBuf, "<br>");
					}
					bHighlightWord = 1;
				}
				else {
					szTag[0] = toupper(szTag[0]);
					stringbuf_appendString(pBuf, "</p>");
					analyzer_startHelpSection(pBuf, szTag);
				}
			}
			else {
				*pszTag++ = c;
			}
		}
	}
	stringbuf_appendString(pBuf, "</p>");
}

static const char* analyzer_helpForFunc(const char* pszName, void* pEdFunc) {
	NATIVE_FUNCTION* pFunc = pEdFunc;
	if (!pFunc->nf_description) {
		return 0;
	}
	const char* pszParameterDescription = pFunc->nf_parameters;
	STRING_BUF* pszParams = stringbuf_create(100);
	int nParams = function_getParameterCount(pFunc);
	for (int i = 1; i <= nParams || (pszParameterDescription && *pszParameterDescription); i++) {
		char szParamType[256];
		char szParamName[256];
		if (i > 1) {
			stringbuf_appendString(pszParams, ", ");
		}
		PARAMETER_TYPE_DESCRIPTOR pt = function_getParameterTypeDescriptor(pFunc, i);
		szParamType[0] = 0;
		szParamName[0] = 0;
		if (pszParameterDescription && *pszParameterDescription) {
			char* pszDest = szParamType;
			while (*pszParameterDescription) {
				char c = *pszParameterDescription++;
				if (c == ',') {
					break;
				}
				if (c == ' ') {
					*pszDest = 0;
					pszDest = szParamName;
					continue;
				}
				*pszDest++ = c;
			}
			*pszDest = 0;
		}
		if (!szParamType[0] || pt.pt_type == PARAM_TYPE_ENUM || pt.pt_type == PARAM_TYPE_BITSET) {
			strcpy(szParamType, (char*)analyzer_pksTypeFromParamtype(pt));
		}
		if (!szParamName[0]) {
			sprintf(szParamName, "p%d", i);
		}
		stringbuf_appendString(pszParams, szParamType);
		stringbuf_appendChar(pszParams, ' ');
		stringbuf_appendString(pszParams, szParamName);
	}
	STRING_BUF* pBuf = stringbuf_create(200);
	analyzer_startHelpSection(pBuf, "Synopsis");
	stringbuf_appendString(pBuf, analyzer_pksTypeFromParamtype(function_getParameterTypeDescriptor(pFunc, 0)));
	stringbuf_appendChar(pBuf, ' ');
	stringbuf_appendString(pBuf, pszName);
	stringbuf_appendChar(pBuf, '(');
	stringbuf_appendString(pBuf, stringbuf_getString(pszParams));
	stringbuf_destroy(pszParams);
	stringbuf_appendString(pBuf, "</p>");
	
	analyzer_formatJavadocComment(pBuf, pFunc->nf_description);
	char* pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);

	return pszRet;
}

/*
 * Returns the help for an enum value.
 */
static const char* analyzer_helpForEnum(const char* pszName, PARAMETER_ENUM_VALUE* pEnumValue) {
	return pEnumValue->pev_description ? _strdup(pEnumValue->pev_description) : 0;
}

static const char* analyzer_helpForMacro(const char* pszName, void* pMac) {
	STRING_BUF* pBuf = stringbuf_create(200);
	MACRO* pMacro = pMac;

	analyzer_startHelpSection(pBuf, "Synopsis");
	decompile_printMacroSignature(pMacro, pBuf, 0);
	stringbuf_appendString(pBuf, "</p>");
	char* pszComment = MAC_COMMENT(pMacro);
	if (pszComment) {
		analyzer_formatJavadocComment(pBuf, pszComment);
	}
	char * pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);
	return pszRet;
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
static void analyzer_getMacros(WINFO* wp, int (*fMatch)(const char* pszMatch), ANALYZER_CALLBACK fCallback) {
	char szBuf[256];
	char szFunction[256];
	int detectedEnd = 0;
	FTABLE* fp = wp->fp;
	LINE* lp = wp->caret.linePointer;
	long nLine = wp->caret.ln;
	LEXICAL_ELEMENT lexicalElements[MAX_LEXICAL_ELEMENT];
	HIGHLIGHTER* pHighlighter = wp->highlighter;
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
			if (strcmp(pState, "pksmacroc.identifier") == 0) {
				bInIdent = 1;
				strcpy(szFunction, szBuf);
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
	BOOL bInParams = FALSE;
	char* key;
	SYMBOL sym = szFunction[0] ? sym_find(sym_getGlobalCompilerContext(), szFunction, &key) : (SYMBOL) { 0 };
	if (sym.s_type == S_EDFUNC) {
		// should select the proper constants for the native function currently edited. function_getParameterTypeDescriptor((void*)VALUE(sym),)
		NATIVE_FUNCTION* pFunc = (NATIVE_FUNCTION*)VALUE(sym);
		if (!bInIdent) {
			fCallback(pFunc->nf_name, pFunc, analyzer_helpForFunc);
		}
		int nParameters = function_getParameterCount(pFunc);
		int bEditorParam = 0;
		for (int nPar = 0; nPar < nParameters; nPar++) {
			PARAMETER_TYPE_DESCRIPTOR ptd = function_getParameterTypeDescriptor(pFunc, nPar+1);
			if (nPar == 0 && ptd.pt_type == PARAM_TYPE_EDITOR_WINDOW) {
				bEditorParam = 1;
			}
			if (ptd.pt_enumVal) {
				for (int i = ptd.pt_enumFirstIndex; i < ptd.pt_enumFirstIndex + ptd.pt_enumCount; i++) {
					PARAMETER_ENUM_VALUE* pValue = &_parameterEnumValueTable[i];
					const char* pszName = pValue->pev_name;
					if ((bInIdent && fMatch(pszName)) || nPar == nParamIndex || (bEditorParam && (nPar - 1 == nParamIndex))) {
						fCallback(pszName, pValue, analyzer_helpForEnum);
					}
				}
			}
		}
	}
	for (int i = 0; i < _functionTableSize; i++) {
		NATIVE_FUNCTION* pFunc = &_functionTable[i];
		if (fMatch(pFunc->nf_name)) {
			fCallback(pFunc->nf_name, pFunc, analyzer_helpForFunc);
		}
	}
	for (PKS_VALUE_TYPE vt = VT_FILE; types_existsType(vt); vt++) {
		const char* pName = types_nameFor(vt);
		if (fMatch(pName)) {
			fCallback(pName, NULL, NULL);
		}
	}
	for (int i = 0; i < macro_getNumberOfMacros(); i++) {
		MACRO* mp = macro_getByIndex(i);
		if (mp && fMatch(MAC_NAME(mp))) {
			fCallback(MAC_NAME(mp), mp, analyzer_helpForMacro);
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
	_keyAnalyzerCallback(pszKeycode, NULL, NULL);
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
			fCallback(pszWord, (void*)MACROREF_TO_INTPTR(macref), analyzer_helpForCommand);
		}
	}
	for (int i = 0; (pszWord = faicon_nameForIndex(i)) != 0; i++) {
		if (fMatch(pszWord)) {
			fCallback(pszWord, NULL, NULL);
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
	analyzer_register("pks-macros", analyzer_getMacros);
	analyzer_register("action-bindings", analyzer_getBindingCompletions);
}

/*
 * Destroy all known analyzers. 
 */
void analyzer_destroyAnalyzers() {
	ll_destroy(&_analyzers, NULL);
}
