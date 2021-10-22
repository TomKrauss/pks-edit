/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * DynamicMenus.c
 *
 * Handle dynamic menus depending on defined macros etc...
 *
 * 						created: 07.06.91 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 */

#include <windows.h>

#include "linkedlist.h"
#include "documentmodel.h"
#include "winfo.h"
#include "resource.h"
#include "edfuncs.h"
#include "helpitem.h"
#include "stringutil.h"
#include "actions.h"
#include "trace.h"

#define	MENU_TABCHAR	(char)8

typedef struct tagMENUS {
	struct tagMENUS *	next;
	char				name[30];
	HMENU				hmenu;
} MENUS, *PMENUS;

static PMENUS 	_titlemenus;
static HMENU	hNextDestroy;
RSCTABLE *	_menutables;

/*--------------------------------------------------------------------------
 * menu_overrideMenuBinding()
 */
static void menu_overrideMenuBinding(void)
{
	RSCTABLE 	*	rp;
	int			nIdx;
	PUSERMENUBIND	pMenu;

	if (_menutables == 0) {
		return;
	}
	rp = _menutables;
	pMenu = (PUSERMENUBIND)rp->rt_data;
	if (pMenu == 0) {
		return;
	}
	for (nIdx = 0; pMenu[nIdx].szString[0]; nIdx++) {
		if (pMenu[nIdx].type == UM_POPUP &&
			pMenu[nIdx].handle != 0) {
		}
	}
	memset(rp->rt_data, 0, (int)((char *)rp->rt_end - (char *)rp->rt_data));
}

/*--------------------------------------------------------------------------
 * menu_initializeDefinition()
 */
void menu_initializeDefinition(char *szMenu) {
	PMENUS pTitleMenu;

	if (rsc_switchtotable(&_menutables, szMenu) == 0) {
		return;
	}
	menu_overrideMenuBinding();
	if ((pTitleMenu = ll_find(_titlemenus, szMenu)) != 0) {
		hNextDestroy = pTitleMenu->hmenu;
		ll_delete(&_titlemenus, pTitleMenu);
	}
	if (hNextDestroy) {
		DestroyMenu(hNextDestroy);
		hNextDestroy = 0;
	}
}

/*--------------------------------------------------------------------------
 * menu_isEmptySlot()
 */
static int menu_isEmptySlot(PUSERMENUBIND mp)
{
	if (!mp->szString[0]) {
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * menu_addMenuMacroItem()
 */
int menu_addMenuMacroItem(char *pszTemp, int menutype, MACROREFTYPE mactype, MACROREFTYPE macidx) {
	RSCTABLE 	*	rp;
	int				nIdx;
	PUSERMENUBIND	pMenu;
	char *			pszRun;
	char			pszString[256];

	if (_menutables == 0 && rsc_switchtotable(&_menutables, "default") == 0) {
		return 0;
	}

	/* make a copy for allowing to modify..*/
	lstrcpy(pszString, pszTemp);
	rp = _menutables;
	pMenu = (PUSERMENUBIND)rp->rt_data;
	for (nIdx = 0; ; nIdx++) {
		if (!pMenu || &pMenu[nIdx] >= (PUSERMENUBIND)rp->rt_end) {
			if (rsc_tableresize(_menutables, sizeof *pMenu, (void*)0,
				(int (*)(void*))menu_isEmptySlot) == 0) {
				return 0;
			}
			pMenu = (PUSERMENUBIND)rp->rt_data;
		}
		if (!pMenu[nIdx].szString[0]) {
			break;
		}
	}

	pMenu += nIdx;
	if (!pszString || !pszString[0]) {
		pMenu->szString[0] = ' ';
	} else {
		/* add Tab character for accelerator key tagging */
		for (pszRun = pszString; *pszRun; pszRun++) {
			;
		}
		/* at least one character is in string here */
		if (pszRun[-1] != MENU_TABCHAR) {	
			*pszRun++ = MENU_TABCHAR;
			*pszRun = 0;
		}
		strmaxcpy(pMenu->szString, pszString, sizeof pMenu->szString);
	}
	pMenu->macref.typ = mactype;
	pMenu->macref.index = macidx;

	if ((pMenu->type = menutype) == UM_ITEM) {
		pMenu->handle = (void*)(intptr_t)((mactype + 1) * IDM_USERDEF0 + macidx);
	}

	return 1;
}


/*--------------------------------------------------------------------------
 * menu_createMenu()
 */
static HMENU menu_createMenu(BOOL bSub)
{
	RSCTABLE 	*	rp;
	HMENU		hMenu;
	HMENU		phSubMenu[10];		/* maximum level of cascading */
	HMENU		hMenuRet;
	int			nIdx;
	int			nCascade;
	PUSERMENUBIND	pMenu;
	PMENUS		pTitleMenu;

	if (_menutables == 0) {
		return 0;
	}
	hMenuRet = 0;
	rp = _menutables;
	if ((pMenu = (PUSERMENUBIND)rp->rt_data) == 0) {
		return 0;
	}

	/*
	 * is this menu already created ?
	 * if, then just return handle
	 */
	if ((pTitleMenu = ll_find(_titlemenus, rp->rt_name)) != 0) {
		if (bSub) {
			return (HMENU)pMenu->handle;
		} else {
			return pTitleMenu->hmenu;
		}
	}
	
	if ((pTitleMenu = ll_insert(&_titlemenus,
		sizeof *pTitleMenu)) == 0) {
		return 0;
	}
	strmaxcpy(pTitleMenu->name, rp->rt_name, sizeof pTitleMenu->name);
	pTitleMenu->hmenu = hMenu = CreateMenu();
	if (!bSub) {
		hMenuRet = hMenu;
	}

	nCascade = 0;
	memset(phSubMenu, 0, sizeof phSubMenu);

	for (nIdx = 0; pMenu->szString[0]; nIdx++, pMenu++) {

		switch (pMenu->type) {
	
		case UM_POPUP:
			pMenu->handle = (void*)CreatePopupMenu();
			AppendMenu(hMenu, MF_POPUP, (UINT_PTR)pMenu->handle, (LPCSTR) pMenu->szString);
			if (!hMenuRet && bSub) {
				hMenuRet = (HMENU)pMenu->handle;
			}
			phSubMenu[nCascade++] = hMenu;
			hMenu = (HMENU)pMenu->handle;
			break;
	
		case UM_ITEM:
			if (hMenu) {
				AppendMenu(hMenu, MF_STRING, (UINT_PTR)pMenu->handle, (LPCSTR) pMenu->szString);
			}
			break;

		case UM_SEPARATOR:
			if (hMenu) {
				AppendMenu(hMenu, MF_SEPARATOR, 0, (char *)0);
			}
			break;
	
		case UM_ENDPOPUP:
			if (--nCascade >= 0) {
				hMenu = phSubMenu[nCascade];
			}
			break;
		}
	}
	
	return hMenuRet;
}

/*--------------------------------------------------------------------------
 * menu_getUserDefinedMacro()
 * user defined Ids of menus are calculated in the following way:
 *
 * the macroref - index is added to the (macroref-type+1) times IDM_USERDEF0
 */
MACROREF *menu_getUserDefinedMacro(int nId)
{
	static MACROREF	macref;

	if (nId < IDM_USERDEF0) {
		return 0;
	}
	macref.index = nId % IDM_USERDEF0;
	macref.typ = (nId - macref.index) / IDM_USERDEF0 - 1;
	return &macref;
}

/*--------------------------------------------------------------------------
 * EdMenuTrackPopup()
 */

int EdMenuTrackPopup(long unused1, long unused2, char *szPopup)
{
	HMENU	hMenu;
	POINT 	pt;
	WINFO *	wp;
	int		buttonType;

	wp = ww_getCurrentEditorWindow();
	if (!wp) {
		return 0;
	}
	if (!rsc_findtable(_menutables, szPopup)) {
		return 0;
	}
	rsc_switchtotable(&_menutables, szPopup);

	GetCursorPos(&pt);

	if ((hMenu = menu_createMenu(TRUE)) == 0) {
		return 0;
	}

	buttonType = ( GetKeyState(VK_LBUTTON) & 0x8000 ) ? 
		TPM_LEFTBUTTON : TPM_RIGHTBUTTON;
	TrackPopupMenu(hMenu, TPM_CENTERALIGN | buttonType, pt.x, pt.y, 0,
		wp->ww_handle, (RECT *)0);
     return 1;
}

/*--------------------------------------------------------------------------
 * menu_getMenuForContext()
 */
HMENU menu_getMenuForContext(char *pszContext) {

	if (!rsc_findtable(_menutables, pszContext)) {
		return 0;
	}
	rsc_switchtotable(&_menutables, pszContext);
	return menu_createMenu(FALSE);
}


