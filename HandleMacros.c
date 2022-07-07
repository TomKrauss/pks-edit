/*
 * HandleMacros.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: common functions for macro handling
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

# if defined( _Windows ) || defined(WIN32)
#include <windows.h>
#include "winterf.h"
#define sprintf wsprintf
# else
#include <stdio.h>
#define	hInst		0
# endif

#include "pksmacrocvm.h"
#include "pksmacro.h"

extern void yyerror(char* s, ...);

/*
 * Return the logical function number (index into functionTable) of the function invoked by the command with the given index.
 */
int macro_getFunctionNumberForCommand(int nCommand, long long* llParam) {
	COM_1FUNC* pCommand = &_commandTable[nCommand].c_functionDef;
	*llParam = pCommand->p;
	return pCommand->funcnum;
}

/*------------------------------------------------------------
 * mac_name()
 */
char *mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type)
{
	switch(type) {
		case CMD_MACRO:  sprintf(szBuf,"%s",MAC_NAME(macro_getByIndex(nIndex))); break;
		case CMD_CMDSEQ: 
			if (nIndex >= _commandTableSize) {
				sprintf(szBuf,"@Unnamed-%d",nIndex);
			} else {
				strcpy(szBuf + 1, _commandTable[nIndex].c_name);
				szBuf[0] = '@';
			}
			break;
		case CMD_NAMESPACE:
			sprintf(szBuf, "Namespace '%s'", MAC_NAME(macro_getNamespaceByIdx(nIndex)));
			break;
		default: sprintf(szBuf,"??"); break;
	}
	return szBuf;
}


/*
 * Returns a macro by a given index or NULL if the index is not
 * located in the valid index range. Can be used to iterate all commands.
 */
char* macro_getCommandByIndex(int nIndex) {
	if (nIndex < 0 || nIndex >= _commandTableSize) {
		return 0;
	}
	static char szMac[80];
	strcpy(szMac + 1, _commandTable[nIndex].c_name);
	szMac[0] = '@';
	return szMac;
}

/*--------------------------------------------------------------------------
 * macro_getCmdIndexByName()
 */
int macro_getCmdIndexByName(const char *name) {
	int			id;

	// TODO: should perform a hash lookup on a long term run.
	for (id = 0; id < _commandTableSize; id++) {
		if (strcmp(_commandTable[id].c_name, name) == 0) {
			return id;
		}
	}
	yyerror("undefined internal macro %s",name);
	return -1;
}

