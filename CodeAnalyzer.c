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

/*
 * Protocol prefix for hyperlinks inside the help window.
 */
#define MACROREF_PROTOCOL				"macroref://"

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

static char* analyzer_printTypeWithLink(const char* pszType) {
	static char szPrinted[256];
	const char* pszDoc = types_getDocumentationFor(pszType);
	if (!pszDoc) {
		return (char*)pszType;
	}
	sprintf(szPrinted, "<a href=\"%s%s\">%s</a>", MACROREF_PROTOCOL, pszType, pszType);
	return szPrinted;
}

static const char* analyzer_pksTypeFromParamtype(PARAMETER_TYPE_DESCRIPTOR pt) {
	if (pt.pt_type == PARAM_TYPE_EDITOR_WINDOW) {
		return analyzer_printTypeWithLink(types_nameFor(VT_EDITOR_HANDLE));
	}
	if (pt.pt_type == PARAM_TYPE_STRING) {
		return analyzer_printTypeWithLink(types_nameFor(VT_STRING));
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
	return analyzer_printTypeWithLink(types_nameFor(VT_NUMBER));
}

static void analyzer_startHelpSection(STRING_BUF* pBuf, const char* pszName) {
	stringbuf_appendString(pBuf, "<p><b>");
	stringbuf_appendString(pBuf, pszName);
	stringbuf_appendString(pBuf, "</b>:<br>");
}

/*
 * Formats a hyperlink reference to be added to the documentation. Will
 * destroy the 2nd parameter as a side effect.
 */
static void analyzer_formatJavadocLink(STRING_BUF* pBuf, STRING_BUF* psbLink) {
	stringbuf_appendString(pBuf, "<a href=\"");
	stringbuf_appendString(pBuf, MACROREF_PROTOCOL);
	stringbuf_appendString(pBuf, stringbuf_getString(psbLink));
	stringbuf_appendString(pBuf, "\">");
	stringbuf_appendString(pBuf, stringbuf_getString(psbLink));
	stringbuf_appendString(pBuf, "</a>");
	stringbuf_destroy(psbLink);
}

static void analyzer_formatJavadocComment(STRING_BUF* pBuf, const char* pszInput) {
	char szTag[64];
	char* pszTag = 0;
	BOOL bParamsFound = 0;
	BOOL bHighlightWord = 0;
	char c;
	STRING_BUF* psbLink = 0;

	analyzer_startHelpSection(pBuf, "Description");
	while ((c = *pszInput++) != 0) {
		if (!pszTag) {
			if (c == '@') {
				pszTag = szTag;
				bHighlightWord = 0;
			} else {
				if (psbLink) {
					if (c == '\n') {
						analyzer_formatJavadocLink(pBuf, psbLink);
						psbLink = 0;
					} else if (!isspace((unsigned char)c)) {
						stringbuf_appendChar(psbLink, c);
					}
					continue;
				}
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
		} else if (pszTag) {
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
				} else if (strcmp("see", szTag) == 0) {
					stringbuf_appendString(pBuf, "</p><b>See also:</b>&nbsp;");
					psbLink = stringbuf_create(64);
				} else {
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
	if (psbLink) {
		analyzer_formatJavadocLink(pBuf, psbLink);
	}
	stringbuf_appendString(pBuf, "</p>");
}

static const char* analyzer_helpForMacrocNativeFunction(const char* pszName, void* pEdFunc) {
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
		} else {
			strcpy(szParamType, analyzer_printTypeWithLink(szParamType));
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
	stringbuf_appendChar(pBuf, ')');
	stringbuf_appendString(pBuf, "</p>");
	
	analyzer_formatJavadocComment(pBuf, pFunc->nf_description);
	char* pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);

	return pszRet;
}

/*
 * Returns the help for an enum value.
 */
static const char* analyzer_helpForMacrocEnum(const char* pszName, PARAMETER_ENUM_VALUE* pEnumValue) {
	STRING_BUF* pBuf = stringbuf_create(200);
	analyzer_startHelpSection(pBuf, "Enum Value");
	stringbuf_appendString(pBuf, "</p><p><i>");
	stringbuf_appendString(pBuf, pszName);
	stringbuf_appendString(pBuf, "</i></p><p>");
	stringbuf_appendString(pBuf, pEnumValue->pev_description ? pEnumValue->pev_description : "NO DESCRIPTION");
	stringbuf_appendString(pBuf, "</p>");
	char* pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);
	return pszRet;
}

static const char* analyzer_helpForMacrocMacro(const char* pszName, void* pMac) {
	STRING_BUF* pBuf = stringbuf_create(200);
	MACRO* pMacro = pMac;

	analyzer_startHelpSection(pBuf, "Synopsis");
	decompile_printMacroSignature(pMacro, pBuf, 0, analyzer_printTypeWithLink);
	stringbuf_appendString(pBuf, "</p>");
	char* pszComment = MAC_COMMENT(pMacro);
	if (pszComment) {
		analyzer_formatJavadocComment(pBuf, pszComment);
	}
	char * pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);
	return pszRet;
}

static char* analyzer_helpForMacrocType(const char* pszType) {
	const char* pszDoc = types_getDocumentationFor(pszType);
	if (!pszDoc) {
		return 0;
	}
	STRING_BUF* pBuf = stringbuf_create(200);
	stringbuf_appendString(pBuf, "<h4>Type ");
	stringbuf_appendString(pBuf, pszType);
	stringbuf_appendString(pBuf, "</h4>");
	analyzer_formatJavadocComment(pBuf, pszDoc);
	char* pszRet = _strdup(stringbuf_getString(pBuf));
	stringbuf_destroy(pBuf);
	return pszRet;
}

static const char* analyzerHelpForMacrocHyperlink(const char* pszUrl) {
	int nSize = (int)strlen(MACROREF_PROTOCOL);
	if (pszUrl && strncmp(pszUrl, MACROREF_PROTOCOL, nSize) == 0) {
		char* ret;
		const char* pszName = pszUrl + nSize;
		char* pszDash = strchr(pszName, '#');
		if (pszDash) {
			pszDash++;
			PARAMETER_ENUM_VALUE* pValue = function_getParameterEnumValue(0, pszDash);
			if (pValue) {
				return analyzer_helpForMacrocEnum(pszDash, pValue);
			}
		}
		SYMBOL sym = sym_find(sym_getGlobalContext(), pszName, &ret);
		if (sym.s_type == S_EDFUNC) {
			return analyzer_helpForMacrocNativeFunction(pszName, (void*)VALUE(sym));
		}
		if (sym.s_type == S_TYPE_IDENTIFIER) {
			return analyzer_helpForMacrocType(pszName);
		}
		for (int i = 0; i < macro_getNumberOfMacros(); i++) {
			MACRO* mp = macro_getByIndex(i);
			if (mp && strcmp(MAC_NAME(mp), pszName) == 0) {
				return analyzer_helpForMacrocMacro(pszName, mp);
			}
		}
	}
	return 0;
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
	char* key;
	SYMBOL sym = szFunction[0] ? sym_find(sym_getGlobalCompilerContext(), szFunction, &key) : (SYMBOL) { 0 };
	if (sym.s_type == S_EDFUNC) {
		// should select the proper constants for the native function currently edited. function_getParameterTypeDescriptor((void*)VALUE(sym),)
		NATIVE_FUNCTION* pFunc = (NATIVE_FUNCTION*)VALUE(sym);
		if (!bInIdent) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) {
				.acp_recommendation = pFunc->nf_name,
					.acp_object = pFunc,
					.acp_help = analyzer_helpForMacrocNativeFunction,
					.acp_getHyperlinkText = analyzerHelpForMacrocHyperlink
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
				for (int i = ptd.pt_enumFirstIndex; i < ptd.pt_enumFirstIndex + ptd.pt_enumCount; i++) {
					PARAMETER_ENUM_VALUE* pValue = &_parameterEnumValueTable[i];
					const char* pszName = pValue->pev_name;
					if ((bInIdent && fMatch(pszName)) || nPar == nParamIndex || (bEditorParam && (nPar - 1 == nParamIndex))) {
						fCallback(&(ANALYZER_CALLBACK_PARAM) {
							.acp_recommendation = pszName,
								.acp_object = pValue,
								.acp_help = analyzer_helpForMacrocEnum
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
				.acp_help = analyzer_helpForMacrocNativeFunction,
				.acp_object= pFunc,
				.acp_getHyperlinkText = analyzerHelpForMacrocHyperlink
			});
		}
	}
	for (PKS_VALUE_TYPE vt = VT_FILE; types_existsType(vt); vt++) {
		const char* pName = types_nameFor(vt);
		if (fMatch(pName)) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) { .acp_recommendation = pName });
		}
	}
	for (int i = 0; i < macro_getNumberOfMacros(); i++) {
		MACRO* mp = macro_getByIndex(i);
		if (mp && fMatch(MAC_NAME(mp))) {
			fCallback(&(ANALYZER_CALLBACK_PARAM) {
				.acp_recommendation = MAC_NAME(mp),
					.acp_object = mp,
					.acp_help = analyzer_helpForMacrocMacro,
					.acp_getHyperlinkText = analyzerHelpForMacrocHyperlink
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
					.acp_getHyperlinkText = analyzerHelpForMacrocHyperlink
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
