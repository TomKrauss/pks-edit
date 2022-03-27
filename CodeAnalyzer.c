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

typedef struct tagANALYZER {
	struct tagANALYZER* an_next;
	char an_name[32];
	ANALYZER_FUNCTION an_function;
} ANALYZER;

static ANALYZER *_analyzers;

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

static const char* analyzer_pksTypeFromParamtype(PARAMETER_TYPE pt) {
	if (pt == PARAM_TYPE_EDITOR_WINDOW) {
		return types_nameFor(VT_EDITOR_HANDLE);
	}
	if (pt == PARAM_TYPE_STRING) {
		return types_nameFor(VT_STRING);
	}
	return types_nameFor(VT_NUMBER);
}

static const char* analyzer_helpForFunc(const char* pszName, void* pEdFunc) {
	EDFUNC* pFunc = pEdFunc;
	if (!pFunc->edf_description) {
		return 0;
	}
	char* pszRet = malloc(strlen(pszName) + 100 + strlen(pFunc->edf_description));
	sprintf(pszRet, "Synopsis: %s %s(", analyzer_pksTypeFromParamtype(function_getParameterTypeDescriptor(pFunc, 0).pt_type), pszName);
	for (int i = 1; i <= function_getParameterCount(pFunc); i++) {
		char szParamName[20];
		if (i > 1) {
			strcat(pszRet, ", ");
		}
		strcat(pszRet, analyzer_pksTypeFromParamtype(function_getParameterTypeDescriptor(pFunc, i).pt_type));
		sprintf(szParamName, " p%d", i);
		strcat(pszRet, szParamName);
	}
	strcat(pszRet, ")\n");
	strcat(pszRet, pFunc->edf_description);
	return pszRet;
}

static const char* analyzer_helpForMacro(const char* pszName, void* pMac) {
	MACRO* pMacro = pMac;
	char* pszComment = MAC_COMMENT(pMacro);
	if (pszComment) {
		return _strdup(pszComment);
	}
	return NULL;
}

/*
 * Returns a possible macro function names which can be used in PKS Edit macros.
 */
static void analyzer_getMacros(WINFO* wp, int (*fMatch)(const char* pszMatch), ANALYZER_CALLBACK fCallback) {
	BOOL bInParams = FALSE;
	char szIdentifier[256];
	char* pszDest = 0;
	szIdentifier[0] = 0;
	for (int i = 0; i < wp->caret.offset; i++) {
		char c = wp->caret.linePointer->lbuf[i];
		if (bInParams) {
			if (c == ')') {
				szIdentifier[0] = 0;
				bInParams = FALSE;
			}
			continue;
		}
		if (isalpha(c)) {
			if (!pszDest) {
				pszDest = szIdentifier;
			}
			*pszDest++ = c;
			continue;
		} else {
			if (pszDest) {
				*pszDest = 0;
			}
			pszDest = 0;
		}
		if (isspace(c)) {
			continue;
		}
		if (c == '(' && szIdentifier[0]) {
			bInParams = TRUE;
		}
	}
	char* key;
	SYMBOL sym = sym_find(sym_getGlobalCompilerContext(), szIdentifier, &key);
	if (sym.s_type == S_EDFUNC) {
		// should select the proper constants for the native function currently edited. function_getParameterTypeDescriptor((void*)VALUE(sym),)
		for (int i = 0; i < _parameterEnumValueTableSize; i++) {
			const char* pszName = _parameterEnumValueTable[i].pev_name;
			if (fMatch(pszName)) {
				fCallback(pszName, NULL, NULL);
			}
		}
	}
	for (PKS_VALUE_TYPE vt = VT_FILE; types_existsType(vt); vt++) {
		const char* pName = types_nameFor(vt);
		if (fMatch(pName)) {
			fCallback(pName, NULL, NULL);
		}
	}
	for (int i = 0; i < _functionTableSize; i++) {
		EDFUNC* pFunc = &_functionTable[i];
		if (fMatch(pFunc->f_name)) {
			fCallback(pFunc->f_name, pFunc, analyzer_helpForFunc);
		}
	}
	for (int i = 0; i < macro_getNumberOfMacros(); i++) {
		MACRO* mp = macro_getByIndex(i);
		if (!mp) {
			continue;
		}
		if (fMatch(MAC_NAME(mp))) {
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
