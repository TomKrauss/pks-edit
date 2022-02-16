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
#include "sym.h"
#include "pkscc.h"
#include "stringutil.h"
#include "xdialog.h"

/*--------------------------------------------------------------------------
 * function_initializeFunctionsAndTypes()
 */
int function_initializeFunctionsAndTypes(void)
{
	EDFUNC	*		ep;
	EDFUNC *		epend;
	TYPEELEM *		enp;
	TYPEELEM *		enpend;
	char *			pszCopy;
	int				idx = 0;

	for (ep = _edfunctab, epend = ep+_nfuncs; ep < epend;  ep++, idx++) {
		if ((pszCopy = (char*)macro_loadStringResource(idx)) == 0 ||
			!sym_insert(pszCopy, S_EDFUNC, (GENERIC_DATA) {
			.val = (intptr_t)ep
		})) {
			return 0;
		}
	}

	for (int i = 0; i < _ntypes; i++) {
		OWNTYPE* pType = &_typetab[i];
		if (pType->ot_enumPrefix) {
			int bFirst = 0;
			int bLast = 0;
			for (int j = 0; j < _nenelems; j++) {
				const char* pszName = _enelemtab[j].te_name;
				if (strstr(pszName, pType->ot_enumPrefix) == pszName) {
					if (!bFirst) {
						bFirst = 1;
						pType->ot_idx = j;
					}
				} else if (bFirst) {
					pType->ot_nelem = j - pType->ot_idx + 1;
					break;
				}
			}
		}
	}
	for (enp = _enelemtab, enpend = enp+_nenelems; enp < enpend; enp++) {
		if (enp->te_name == 0 ||
			!sym_insert((char*)enp->te_name, S_ENUM, (GENERIC_DATA) {
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
long function_enumValueFor(TYPEELEM *enp)
{
	return enp->te_val;
}

/*--------------------------------------------------------------------------
 * function_getIndexOfFunction()
 */
int function_getIndexOfFunction(EDFUNC *ep)
{
	return (int)(size_t)(ep - _edfunctab);
}

/*
 * Returns the parameter descriptor for a function for the n-th parameter. Parameter count
 * starts with 1, parameter type 0 is the return type of the function.
 */
PARAMETER_TYPE_DESCRIPTOR function_getParameterTypeDescriptor(EDFUNC* ep, int nParamIdx) {
	char* pT = ep->edf_paramTypes;

	while (*pT && --nParamIdx >= 0) {
		if (*pT == PAR_ENUM) {
			pT++;
		}
		pT++;
	}
	if (*pT == 'e') {
		for (int i = 0; i < _ntypes; i++) {
			if (_typetab[i].ot_name == pT[1]) {
				return (PARAMETER_TYPE_DESCRIPTOR) {
					.pt_type = *pT,
					.pt_enumType = &_typetab[i]
				};
			}
		}
	}
	return (PARAMETER_TYPE_DESCRIPTOR) {.pt_type = *pT, .pt_enumType = 0 };
}

/*--------------------------------------------------------------------------
 * function_parameterIsFormStart()
 */
int function_parameterIsFormStart(EDFUNC *ep, int parno) {
	if (parno > 1)
		return 0;
	PARAMETER_TYPE_DESCRIPTOR ptd = function_getParameterTypeDescriptor(ep, parno + 1);
	return ptd.pt_enumType && (ptd.pt_enumType->ot_flags & OF_ELIPSIS);
}

/*--------------------------------------------------------------------------
 * function_returnsString()
 */
int function_returnsString(EDFUNC *ep)
{
	return ep->edf_paramTypes[0] == PAR_STRING;
}

