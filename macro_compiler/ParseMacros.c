/*
 * ParseMacros.c
 *
 * PROJECT: PKSEDIT - Configuration
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 07.02.1991
 *									
 */

#include "edfuncs.h"
#include "funcdef.h"
#include "sym.h"
#include "pkscc.h"
#include "stringutil.h"

/*--------------------------------------------------------------------------
 * init_funcs()
 */
int init_funcs(void)
{
	EDFUNCDEF	*		ep;
	EDFUNCDEF *		epend;
	TYPEELEM *		enp;
	TYPEELEM *		enpend;
	char *			pszCopy;

	for (ep = _functab, epend = ep+_nfunctions; ep < epend;  ep++) {
		if ((pszCopy = string_allocate(macro_loadStringResource(ep->idx+IDM_LOWFUNCNAME))) == 0 ||
			!sym_insert(pszCopy,S_EDFUNC,(intptr_t)ep)) {
			return 0;
		}
	}

	for (enp = _enelemtab, enpend = enp+_nenelems; enp < enpend; enp++) {
		if ((pszCopy = string_allocate(macro_loadStringResource(enp->te_name))) == 0 ||
			!sym_insert(pszCopy,S_ENUM,(intptr_t)enp)) {
			return 0;
		}
	}

	return 1;
}

/*--------------------------------------------------------------------------
 * EnumValue()
 */
long EnumValue(TYPEELEM *enp)
{
	return enp->te_val;
}

/*--------------------------------------------------------------------------
 * FuncIdx()
 */
int FuncIdx(EDFUNCDEF *ep)
{
	return ep->idx;	
}

/*--------------------------------------------------------------------------
 * IsFormStart()
 */
int IsFormStart(EDFUNCDEF *ep,int parno)
{
	int idx;

	if (parno > 1)
		return 0;

	idx = ep->ftyps[parno+1]-PAR_USER;
	if (idx < 0 || idx >= _ntypes)
		return 0;
	
	if (_typetab[idx].ot_flags & OF_ELIPSIS)
		return 1;
	
	return 0;
}

/*--------------------------------------------------------------------------
 * IsStringFunc()
 */
int IsStringFunc(EDFUNCDEF *ep)
{
	return ep->ftyps[0] == PAR_STRING;
}

