/*
 * KeyBindings.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: translate key names 2 scan code and vc.vs.
 *		  maintain key-function bindings in a hash tab
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krau�
 */

# include "editab.h"
# include "edfuncs.h"
# include "resource.h"

static RSCTABLE __k = {
	0,	"default",	sizeof _keymaptab, 	_keymaptab,
	(unsigned char *)_keymaptab+sizeof(_keymaptab)
};

RSCTABLE *_keytables = &__k;

/*--------------------------------------------------------------------------
 * key_overridetable()
 */
void key_overridetable(void)
{
	RSCTABLE *		rp;

	if ((rp = _keytables) != 0 && rp->rt_data) {
		blfill(rp->rt_data, (int)((char *)rp->rt_end - (char *)rp->rt_data), 0);
	}
}

/*---------------------------------*/
/* key_destroytables()			*/
/*---------------------------------*/
void key_destroytables(void)
{
	RSCTABLE 	*rp;
	RSCTABLE	*rpnext;
	
	for (rp = _keytables; rp; rp = rpnext) {
		if (rp->rt_data != _keymaptab) {
			_free(rp->rt_data);
		}
		rpnext = rp->rt_next;
		if (rp != &__k) {
			_free(rp);
		}
	}
	_keytables = &__k;
	blfill(_keymaptab,sizeof _keymaptab,0);
}

/*---------------------------------*/
/* key_destroytable()			*/
/*---------------------------------*/
void key_destroytable(char *name)
{
	RSCTABLE 	*rp;

	if ((rp = rsc_findtable(_keytables, name)) != 0) {
		blfill(rp->rt_data, (int)((char *)rp->rt_end-(char *)rp->rt_data), 0);
	}
}

/*---------------------------------*/
/* key_unbind()				*/
/*---------------------------------*/
void key_unbind(KEYBIND *kp)
{
	if (kp == 0)
		return;

	kp->keycode = 0;		/* mark used */
	kp->macref.typ = CMD_NONE;
}

/*---------------------------------*/
/* key_delbindings()			*/
/* delete all key bindings to a 	*/
/* for inst. single macro,...		*/
/*---------------------------------*/
void key_delbindings(MACROREFTYPE typ, MACROREFIDX val)
{
	RSCTABLE *rp;
	KEYBIND  *kp;

	for (rp = _keytables; rp; rp = rp->rt_next) {
		for (kp = rp->rt_data; kp < rp->rt_end; kp++) {
			if (typ == kp->macref.typ && val == kp->macref.index) {
				check_fkdirty(kp->keycode);
				key_unbind(kp);
			}
		}
	}
}

/*-----------------------------------------------------------
 * keyslot()
 */
static KEYBIND *keyslot(KEYCODE keycode)
{
	return rsc_tablefindcode(_keytables,sizeof(KEYBIND),keycode);
}

/*-----------------------------------------------------------
 * keybound()
 */
KEYBIND *keybound(KEYCODE keycode)
{
	return keyslot(keycode);
}

/*--------------------------------------------------------------------------
 * keyemptyslot()
 */
int keyemptyslot(KEYBIND *kp)
{
	if (!kp->keycode || kp->keycode == K_DELETED) {
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------
 * keygetbind()
 */
KEYBIND *keygetbind(void)
{
 	KEYBIND 	*kp;

	if ((kp = keyslot(0)) != 0)
		return kp;

	return rsc_tableresize(_keytables,sizeof *kp, _keymaptab,
		(int (*)(void*))keyemptyslot);
}

