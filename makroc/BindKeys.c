/*
 * BindKeys.c
 *
 * PROJEKT: RESOURCE COMPILER for PKS-EDIT MS - WINDOWS 3.0.1
 *
 * purpose: maintain keybind tables
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include <stdio.h>
#include <string.h>

#include "edfuncs.h"
#include "resource.h"
#include "pkscc.h"

extern char *		mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);
extern char *		code2key(KEYCODE k);
extern char *		code2mouse(MOUSECODE code);
extern void 		yyerror(char *,...);
extern KEYBIND *	keybound(KEYCODE keycode);
extern MOUSEBIND *	mousebound(MOUSECODE mcode);
extern MOUSEBIND *	mousegetbind(void);

extern	RSCTABLE *	_keytables;
extern	RSCTABLE *	_mousetables;

/*------------------------------------------------------------
 * key_switchtotable()
 */
int key_switchtotable(char *name)
{
	if (!name) {
		return 0;
	}
	return rsc_switchtotable(&_keytables,name);
}

/*------------------------------------------------------------
 * mouse_switchtotable()
 */
int mouse_switchtotable(char *name)
{
	if (!name) {
		return 0;
	}
	return rsc_switchtotable(&_mousetables,name);
}

/*------------------------------------------------------------
 * bind_mouse()
 * bind a mouse to current key table context
 */
int bind_mouse(MOUSECODE mousecode, MACROREFTYPE typ, MACROREFIDX idx, 
	int flags, int augment)
{
	MOUSEBIND *		mp;

	mp = mousebound(mousecode);
	if (augment && mp) {
		/* do not override existing bindings */
		return 1;
	}

	if (!mp && (mp = mousegetbind()) == 0) {
		return 0;
	}

	mp->macref.typ = typ;
	mp->macref.index = idx;
	mp->button = mousecode.button;
	mp->nclicks = mousecode.nclicks;
	mp->shift = mousecode.shift;
	mp->flags = flags;
	return 1;
}

/*------------------------------------------------------------
 * bind_key()
 * bind a key to current key table context
 */
int bind_key(KEYCODE keycode, MACROREFTYPE typ, MACROREFIDX idx, int augment)
{
	if (augment) {
		if (keybound(keycode)) {
			/* do not override existing bindings */
			return 1;
		}
	}

	check_fkdirty(keycode);
	return mac_scaninsert(keycode,idx,typ);
}

