/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * DynamicMenus.c
 *
 * Handle menus and context menu and create them from the action binding definitions.
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
#include "menu.h"
#include "trace.h"
#include "xdialog.h"
#include "edctype.h"

static HMENU _contextMenu;
static POINT _contextMenuPosition;

/*
 * Calculates a label to be used for a menu entry and process mnemonics at the same time. Assumption is,
 * that all provided text does not contain mnemonics, so this algorithm will turn '&' into '&&' and
 * tries to insert the first "available free" menmonic spot.
 */
static void menu_determineLabelWithMnemonic(char* pszLabel, CONTEXT_MENU* pMenu, char* pszCharsWithMnemonics) {
	char szTemp[128];
	const char* pszSource;
	pszSource = szTemp;
	BOOL bMnemonicAssigned = FALSE;
	if (!pMenu->cm_label) {
		if (pMenu->cm_resourceId) {
			strcpy(szTemp, dlg_getResourceString(pMenu->cm_resourceId));
		} else {
			char szTooltip[256];
			command_getTooltipAndLabel(pMenu->cm_macref, szTooltip, szTemp);
		}
	} else {
		pszSource = pMenu->cm_label;
	}
	char c;
	while ((c = *pszSource++) != 0) {
		if (c == '&') {
			*pszLabel++ = '&';
		} else if (!bMnemonicAssigned) {
			unsigned char cLower = pks_isupper(c) ? tolower((unsigned char)c) : (unsigned char)c;
			if (!pszCharsWithMnemonics[cLower]) {
				pszCharsWithMnemonics[cLower] = 1;
				*pszLabel++ = '&';
				bMnemonicAssigned = TRUE;
			}
		}
		*pszLabel++ = c;
	}
	*pszLabel++ = '\b';
	*pszLabel++ = 0;
}

static BOOL menu_appendMenuItems(HMENU hMenu, CONTEXT_MENU* pMenu) {
	char szLabel[100];
	char charsWithMnemonic[256];
	BOOL bHasItems = FALSE;
	memset(charsWithMnemonic, 0, sizeof charsWithMnemonic);
	while (pMenu) {
		if (pMenu->cm_isSeparator) {
			AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
		}
		else if (pMenu->cm_children) {
			HMENU hNested = CreateMenu();
			menu_determineLabelWithMnemonic(szLabel, pMenu, charsWithMnemonic);
			AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hNested, szLabel);
			menu_appendMenuItems(hNested, pMenu->cm_children);
		}
		else {
			menu_determineLabelWithMnemonic(szLabel, pMenu, charsWithMnemonic);
			AppendMenu(hMenu, MF_STRING, ((int)pMenu->cm_macref.typ << 16) + pMenu->cm_macref.index, szLabel);
		}
		pMenu = pMenu->cm_next;
		bHasItems = TRUE;
	}
	return bHasItems;
}

/*
 * Populate the current context menu depending on the action context of the current editor window.
 */
static BOOL menu_populateContextMenu(WINFO* wp) {
	CONTEXT_MENU* pMenu = contextmenu_getFor(wp->actionContext);
	return menu_appendMenuItems(_contextMenu, pMenu);
}

static void menu_showContextMenu(HWND hwndParent, WINFO* wp, int x, int y) {
	if (_contextMenu != NULL) {
		DestroyMenu(_contextMenu);
	}
	_contextMenu = CreatePopupMenu();
	if (!menu_populateContextMenu(wp)) {
		return;
	}
	_contextMenuPosition.x = x;
	_contextMenuPosition.y = y;
	ScreenToClient(hwndParent, (LPPOINT)&_contextMenuPosition);

	TrackPopupMenu(_contextMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON,
		x, y, 0, hwndParent, NULL);
}

/*
 * Open the context menu in the current editor window (if any) close to the current mouse position.
 * The context menu will be popuplated with the appropriate items. Return 1 if successful, 0 otherwise.
 */
int menu_openContextMenu() {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (!wp) {
		return 0;
	}
	POINT pt;
	GetCursorPos(&pt);
	menu_showContextMenu(wp->ww_handle, wp, pt.x, pt.y);
	return 1;
}

/*
 * Returns the position where the last context menu was shown.
 */
POINT menu_getContextMenuPopupPosition() {
	return _contextMenuPosition;
}

/*
 * Selects the action context for the main menu of PKS-Edit (e.g. when the currently focussed editor window changes).
 */
void menu_selectActionContext(const char* pszContext) {
	// TODO....
}

