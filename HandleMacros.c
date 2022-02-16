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

#include "edfuncs.h"

extern void yyerror(char* s, ...);

/*------------------------------------------------------------
 * mac_name()
 */
char *mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type)
{
	switch(type) {
		case CMD_MACRO:  sprintf(szBuf,"%s",macro_getByIndex(nIndex)->name); break;
		case CMD_CMDSEQ: 
			if (nIndex < 0 || nIndex >= _ncmdseq) {
				sprintf(szBuf,"@Unnamed-%d",nIndex);
			} else {
				strcpy(szBuf + 1, _cmdseqtab[nIndex].c_name);
				szBuf[0] = '@';
			}
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
	if (nIndex < 0 || nIndex >= _ncmdseq) {
		return 0;
	}
	static char szMac[80];
	strcpy(szMac + 1, _cmdseqtab[nIndex].c_name);
	szMac[0] = '@';
	return szMac;
}

/*--------------------------------------------------------------------------
 * macro_getCmdIndexByName()
 */
int macro_getCmdIndexByName(const char *name) {
	int			id;

	// TODO: should perform a hash lookup on a long term run.
	for (id = 0; id < _ncmdseq; id++) {
		if (strcmp(_cmdseqtab[id].c_name, name) == 0) {
			return id;
		}
	}
	yyerror("undefined internal macro %s",name);
	return -1;
}

