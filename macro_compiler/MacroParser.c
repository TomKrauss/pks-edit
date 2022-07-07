/*
 * ParseMacros.c
 *
 * PROJECT: PKSEDIT - Configuration
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 07.02.1991
 */

#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "funcdef.h"
#include "symbols.h"
#include "stringutil.h"
#include "xdialog.h"

extern HINSTANCE	hInst;
extern void yyerror(const char* s, ...);

/*--------------------------------------------------------------------------
 * function_initializeFunctionsAndTypes()
 */
int function_initializeFunctionsAndTypes(void) {
	static int initialized;
	NATIVE_FUNCTION	*		ep;
	NATIVE_FUNCTION *		epend;
	char *			pszCopy;
	int				idx = 0;

	if (initialized) {
		return 1;
	}
	// Initializes the function table sizes.
	function_getNumberOfStaticallyDefinedFunctions();
	initialized = TRUE;
	for (ep = _functionTable, epend = ep+_functionTableSize; ep < epend;  ep++, idx++) {
		if ((pszCopy = (char*)macro_loadStringResource(idx)) == 0 ||
			!sym_createSymbol(sym_getKeywordContext(), pszCopy, S_EDFUNC, 0, (GENERIC_DATA) {
			.val = (intptr_t)ep
		}, 0)) {
			return 0;
		}
	}
	return 1;
}

/*
 * Returns the enum value with the given name in the given enum nPksValueType.
 */
PARAMETER_ENUM_VALUE* function_getParameterEnumValue(const char* pszEnumType, const char* pszEnumName) {
	char* unused;
	SYMBOL sym = sym_find(sym_getKeywordContext(), pszEnumName, &unused);
	if (sym.s_type == S_ENUM) {
		return (PARAMETER_ENUM_VALUE * )VALUE(sym);
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * function_enumValueFor()
 */
long function_enumValueFor(const PARAMETER_ENUM_VALUE *enp)
{
	return enp->pev_val;
}

/*--------------------------------------------------------------------------
 * function_getIndexOfFunction()
 */
int function_getIndexOfFunction(const NATIVE_FUNCTION *ep)
{
	return (int)(size_t)(ep - _functionTable);
}

/*
 * Returns the number of parameters of a native macro function.
 */
int function_getParameterCount(NATIVE_FUNCTION* ep) {
	if (ep->nf_paramCount >= 0) {
		return ep->nf_paramCount;
	}
	function_initializeFunctionsAndTypes();
	char* pT = ep->nf_paramTypes;
	int nCount = 0;
	pT++;

	while (*pT) {
		if (*pT == PARAM_TYPE_ENUM || *pT == PARAM_TYPE_BITSET) {
			while (*pT != '_') {
				if (!*pT) {
					return nCount+1;
				}
				pT++;
			}
		}
		nCount++;
		pT++;
	}
	ep->nf_paramCount = nCount;
	return nCount;
}

/*
 * Returns the parameter descriptor for a function for the n-th parameter. Parameter count
 * starts with 1, parameter nPksValueType 0 is the return nPksValueType of the function.
 */
PARAMETER_TYPE_DESCRIPTOR function_getParameterTypeDescriptor(NATIVE_FUNCTION* ep, int nParamIdx) {
	char* pT = ep->nf_paramTypes;

	function_initializeFunctionsAndTypes();

	while (*pT && --nParamIdx >= 0) {
		if (*pT == PARAM_TYPE_ENUM || *pT == PARAM_TYPE_BITSET) {
			while (*pT != '_') {
				if (!*pT) {
					goto notfound;
				}
				pT++;
			}
		}
		pT++;
	}
	PARAMETER_TYPE tType = *pT;
	PKS_VALUE_TYPE nPksValueType;
	if (tType == PARAM_TYPE_ENUM || tType == PARAM_TYPE_BITSET) {
		char szPrefix[20];
		char* pszDest = szPrefix;
		pT++;
		while (*pT) {
			*pszDest++ = *pT;
			if (*pT++ == '_') {
				break;
			}
		}
		*pszDest = 0;
		PARAMETER_ENUM_VALUE* pMatch;
		int nCount;

		if (types_getEnumDescriptorForEnumPrefix(szPrefix, &pMatch, &nCount, &nPksValueType)) {
			return (PARAMETER_TYPE_DESCRIPTOR) {
				.pt_type = tType,
				.pt_valueType = nPksValueType,
				.pt_enumVal = pMatch,
				.pt_enumCount = nCount
			};
		}
	}
notfound:
	switch (tType) {
	case PARAM_TYPE_EDITOR_WINDOW: nPksValueType = VT_EDITOR_HANDLE; break;
	case PARAM_TYPE_INT: nPksValueType = VT_NUMBER; break;
	default: nPksValueType = VT_STRING; break;
	}
	return (PARAMETER_TYPE_DESCRIPTOR) {
		.pt_type = tType,
		.pt_valueType = nPksValueType
	};
}

/*
 * Can be used to find out, whether a method is more tightly integrated with the macroC VM and works on PKS_VALUES directly.
 * Signature is: PKS_VALUE myMethod(EXECUTION_CONTEXT*pContext, PKS_VALUE* pValues, int nArguments)
 */
int function_hasInternalVMPrototype(NATIVE_FUNCTION* ep) {
	return ep->nf_paramTypes[0] == PARAM_TYPE_PKS_VALUE;
}

/*--------------------------------------------------------------------------
 * function_parameterIsFormStart()
 */
int function_parameterIsFormStart(NATIVE_FUNCTION *ep, int parno) {
	if (parno > 1)
		return 0;
	PARAMETER_TYPE_DESCRIPTOR ptd = function_getParameterTypeDescriptor(ep, parno);
	return ptd.pt_enumVal && (string_startsWith(ptd.pt_enumVal->pev_name, "FORM_"));
}

/*
 * Register a macro C function given the name with which it should be visible in PKSMacroC, the windows proc name, the optional
 * module (if null it is loaded from PKS-Edit), the signature description and an optional help text.
 */
int function_registerNativeFunction(const char* pszMacroCName, const char* pszFunctionName, const char* pszModule, 
		const char* pszSignature, const char* pszDescription, const char* pszParameters) {
	int ret = 1;
	HINSTANCE hInstance = hInst;
	int newTableSize;
	int nStatic = function_getNumberOfStaticallyDefinedFunctions();
	newTableSize = _functionTableSize;
	if (pszModule && strcmp(pszModule, "PKSEDIT") != 0) {
		hInstance = LoadLibrary(pszModule);
	}
	char* existingKey;
	SYMBOL symbol = sym_find(sym_getKeywordContext(), pszMacroCName, &existingKey);
	int nIndex;
	if (symbol.s_type == S_EDFUNC) {
		nIndex = (int)((NATIVE_FUNCTION*)VALUE(symbol) - _functionTable);
	}
	else {
		nIndex = newTableSize++;
	}
	long long (*p)() = GetProcAddress(hInstance, pszFunctionName);
	if (!p && nIndex > nStatic) {
		ret = 0;
		interpreter_raiseError("Cannot find proc address for %s (lib=%s)", pszFunctionName, pszModule ? pszModule : "PKSEDIT");
	}
	else {
		int nMax = MAX_NATIVE_FUNCTIONS;
		if (nIndex >= nMax) {
			interpreter_raiseError("Too many native functions.");
			// not reached
			ret = 0;
		}
		else {
			NATIVE_FUNCTION* pFunc = &_functionTable[nIndex];
			if (nIndex < nStatic) {
				free((char*)pFunc->nf_description);
				free((char*)pFunc->nf_parameters);
				pFunc->nf_description = 0;
				pFunc->nf_parameters = 0;
			} else {
				function_destroyRegisteredNative(pFunc);
				sym_createSymbol(sym_getKeywordContext(), (char*)pszMacroCName, S_EDFUNC, 0, (GENERIC_DATA) {
					.val = (intptr_t)pFunc
				}, 0);
				pFunc->nf_name = _strdup(pszMacroCName);
				pFunc->nf_paramTypes = _strdup(pszSignature);
				pFunc->nf_execute = p;
			}
			pFunc->nf_description = pszDescription ? _strdup(pszDescription) : 0;
			pFunc->nf_parameters = pszParameters ? _strdup(pszParameters) : 0;
			_functionTableSize = newTableSize;
		}
	}
	if (hInstance != hInst) {
		FreeLibrary(hInstance);
	}
	return ret;
}



