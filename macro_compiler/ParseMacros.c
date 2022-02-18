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

#include "edfuncs.h"
#include "funcdef.h"
#include "symbols.h"
#include "pkscc.h"
#include "stringutil.h"
#include "xdialog.h"

BOOLEAN string_startsWith(const char* pszString, const char* pszPrefix) {
	return strncmp(pszPrefix, pszString, strlen(pszPrefix)) == 0;
}
/*--------------------------------------------------------------------------
 * function_initializeFunctionsAndTypes()
 */
int function_initializeFunctionsAndTypes(void) {
	static int initialized;
	EDFUNC	*		ep;
	EDFUNC *		epend;
	PARAMETER_ENUM_VALUE *		enp;
	PARAMETER_ENUM_VALUE *		enpend;
	char *			pszCopy;
	int				idx = 0;

	if (initialized) {
		return 1;
	}
	initialized = TRUE;
	for (ep = _functionTable, epend = ep+_functionTableSize; ep < epend;  ep++, idx++) {
		if ((pszCopy = (char*)macro_loadStringResource(idx)) == 0 ||
			!sym_insert(sym_getGlobalContext(), pszCopy, S_EDFUNC, (GENERIC_DATA) {
			.val = (intptr_t)ep
		})) {
			return 0;
		}
	}

	for (enp = _parameterEnumValueTable, enpend = enp+_parameterEnumValueTableSize; enp < enpend; enp++) {
		if (enp->pev_name == 0 || !sym_insert(sym_getGlobalContext(), (char*)enp->pev_name, S_ENUM, (GENERIC_DATA) {
			.val = (intptr_t)enp
		})) {
			return 0;
		}
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * function_enumValueFor()
 */
long function_enumValueFor(PARAMETER_ENUM_VALUE *enp)
{
	return enp->pev_val;
}

/*--------------------------------------------------------------------------
 * function_getIndexOfFunction()
 */
int function_getIndexOfFunction(EDFUNC *ep)
{
	return (int)(size_t)(ep - _functionTable);
}

/*
 * Returns the parameter descriptor for a function for the n-th parameter. Parameter count
 * starts with 1, parameter type 0 is the return type of the function.
 */
PARAMETER_TYPE_DESCRIPTOR function_getParameterTypeDescriptor(EDFUNC* ep, int nParamIdx) {
	char* pT = ep->edf_paramTypes;

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
		PARAMETER_ENUM_VALUE* pMatch = 0;
		for (int i = 0; i < _parameterEnumValueTableSize; i++) {
			const char* pszValueName = _parameterEnumValueTable[i].pev_name;
			if (string_startsWith(pszValueName, szPrefix)) {
				if (!pMatch) {
					pMatch = &_parameterEnumValueTable[i];
				}
			} else if (pMatch) {
				return (PARAMETER_TYPE_DESCRIPTOR) {
					.pt_type = tType,
					.pt_enumVal = pMatch,
					.pt_enumCount = (int) (&_parameterEnumValueTable[i] - pMatch)
				};
			}
		}
	}
notfound:
	return (PARAMETER_TYPE_DESCRIPTOR) {.pt_type = tType};
}

/*--------------------------------------------------------------------------
 * function_parameterIsFormStart()
 */
int function_parameterIsFormStart(EDFUNC *ep, int parno) {
	if (parno > 1)
		return 0;
	PARAMETER_TYPE_DESCRIPTOR ptd = function_getParameterTypeDescriptor(ep, parno);
	return ptd.pt_enumVal && (string_startsWith(ptd.pt_enumVal->pev_name, "FORM_"));
}

/*--------------------------------------------------------------------------
 * function_returnsString()
 */
int function_returnsString(EDFUNC *ep)
{
	return ep->edf_paramTypes[0] == PARAM_TYPE_STRING;
}

