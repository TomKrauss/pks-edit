/*
 * Project: PKS Edit for Windows
 *
 * Menus.c
 *
 * Create and update menubar and context menu depending on the current action binding 
 * definitions.
 *
 * 															updated: 01/26/2022
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 */

#include <windows.h>
#include <stdio.h>

#include "linkedlist.h"
#include "documentmodel.h"
#include "winfo.h"
#include "pksmacrocvm.h"
#include "arraylist.h"
#include "pksmacro.h"
#include "actionbindings.h"
#include "helpitem.h"
#include "stringutil.h"
#include "actions.h"
#include "menu.h"
#include "trace.h"
#include "xdialog.h"
#include "edctype.h"
#include "funcdef.h"
#include "history.h"

static HMENU _contextMenu;
static POINT _contextMenuPosition;

/*--------------------------------------------------------------------------
 * interpreter_canExecuteFunction()
 */
extern BOOL op_defineOption(WINFO* wp, long nFlag);

/*
 * Calculates a label to be used for a menu entry and process mnemonics at the same time. Assumption is,
 * that all provided text does not contain mnemonics, so this algorithm will turn '&' into '&&' and
 * tries to insert the first "available free" menmonic spot.
 */
static void menu_determineLabelWithMnemonic(char* pszLabel, MENU_ITEM_DEFINITION* pMenu, char* pszCharsWithMnemonics, BOOL bSubmenu) {
	char szTemp[128];
	const char* pszSource;
	pszSource = szTemp;
	BOOL bMnemonicAssigned = FALSE;
	const char* pszBoundLabel = bindings_getBoundText(&pMenu->mid_label);
	if (pszBoundLabel) {
		strcpy(szTemp, pszBoundLabel);
	} else {
		char szTooltip[256];
		command_getTooltipAndLabel(pMenu->mid_command, szTooltip, szTemp);
		if (!szTemp[0]) {
			strcpy(szTemp, szTooltip);
		}
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
	if (!bSubmenu) {
		*pszLabel++ = '\b';
	}
	*pszLabel = 0;
	if (!bSubmenu) {
		KEYCODE k;
		if ((k = bindings_findBoundKey(NULL, pMenu->mid_command)) != K_DELETED) {
			strcpy(pszLabel, bindings_keycodeToString(k));
		}
	}
}

/*
 * Append the list of recently opened files to a menu.
 */
static int menu_appendHistoryEntries(HMENU hMenu, BOOL bAppendSeparator) {
	BOOL bFirst = TRUE;
	char* pEntry;
	int nItems = 0;
	int nCmd = IDM_HISTORY;
	for (int i = 0; i < MAX_HISTORY_FILES; i++) {
		if ((pEntry = hist_getString(OPEN_FILES, i)) == 0) {
			break;
		}
		if (bFirst && bAppendSeparator) {
			AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
		}
		bFirst = FALSE;
		char szLabel[512];
		sprintf(szLabel, "&%d %.*s", i + 1, (int)sizeof szLabel, pEntry);
		AppendMenu(hMenu, MF_STRING, nCmd++, szLabel);
		nItems++;
	}
	return nItems;
}

static void menu_enableItem(WINFO* wp, HMENU hMenu, int wItem, int nType, int nIndex) {
	BOOL bEnable = TRUE;
	if (nType == CMD_CMDSEQ) {
		long long llParam;
		int nFuncnum = macro_getFunctionNumberForCommand(nIndex, &llParam);
		bEnable = interpreter_canExecuteNativeFunction(nFuncnum, llParam, 0);
		if (nFuncnum == FUNC_EdOptionToggle) {
			if (op_defineOption(wp, (long)llParam)) {
				CheckMenuItem(hMenu, wItem, MF_CHECKED | MF_BYPOSITION);
			}
		}
	}
	else if (nType == CMD_MACRO) {
		bEnable = interpreter_canExecuteMacro(nIndex, 0);
	}
	EnableMenuItem(hMenu, wItem,
		bEnable ?
		MF_BYPOSITION | MF_ENABLED :
		MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
}

/*
 * Append the list of macro menu function bindings to a menu.
 */
static int menu_appendMacroEntries(WINFO* wp, HMENU hMenu, int wItemIndex, const char* pszPrefix, BOOL bAppendSeparator) {
	char szLabel[512];
	BOOL bFirst = TRUE;
	int nMacros = macro_getNumberOfMacros();
	int nItems = 0;
	for (int i = 0; i < nMacros; i++) {
		MACRO* mp = macro_getByIndex(i);
		if (mp->mc_scope == MS_LOCAL) {
			continue;
		}
		if (pszPrefix && *pszPrefix && strstr(mp->mc_name, pszPrefix) == 0) {
			continue;
		}
		if (bFirst && bAppendSeparator) {
			AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
			bFirst = FALSE;
		}
		macro_getLabelFor(mp, szLabel, sizeof szLabel);
		AppendMenu(hMenu, MF_STRING, (CMD_MACRO << 16) + i, szLabel);
		nItems++;
		menu_enableItem(wp, hMenu, wItemIndex + nItems, CMD_MACRO, i);
	}
	return nItems;
}

static BOOL menu_appendMenuItems(WINFO* wp, HMENU hMenu, MENU_ITEM_DEFINITION* pMenu) {
	char szLabel[100];
	char charsWithMnemonic[256];

	memset(charsWithMnemonic, 0, sizeof charsWithMnemonic);
	int wItem = 0;
	while (pMenu) {
		if (pMenu->mid_isHistoryMenu) {
			wItem += menu_appendHistoryEntries(hMenu, pMenu->mid_isSeparator) - 1;
		} else if (pMenu->mid_isMacroMenu) {
			wItem += menu_appendMacroEntries(wp, hMenu, wItem, pMenu->mid_label.bt_text, pMenu->mid_isSeparator) - 1;
		}  else if (pMenu->mid_isSeparator) {
			AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
		} else if (pMenu->mid_children) {
			HMENU hNested = CreateMenu();
			menu_determineLabelWithMnemonic(szLabel, pMenu, charsWithMnemonic, TRUE);
			AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hNested, szLabel);
			menu_appendMenuItems(wp, hNested, pMenu->mid_children);
		}
		else {
			menu_determineLabelWithMnemonic(szLabel, pMenu, charsWithMnemonic, FALSE);
			AppendMenu(hMenu, MF_STRING, ((int)pMenu->mid_command.typ << 16) + pMenu->mid_command.index, szLabel);
			menu_enableItem(wp, hMenu, wItem, pMenu->mid_command.typ, pMenu->mid_command.index);
		}
		pMenu = pMenu->mid_next;
		wItem++;
	}
	return wItem > 0;
}

/*
 * Populate the current context menu depending on the action context of the current editor window.
 */
static BOOL menu_populateContextMenu(WINFO* wp, const char* pActionContext) {
	MENU_ITEM_DEFINITION* pMenu = bindings_getContextMenuFor(pActionContext);
	interpreter_setContextWindow(wp);
	int ret = menu_appendMenuItems(wp, _contextMenu, pMenu);
	interpreter_setContextWindow(0);
	return ret;
}

/*
 * Show the context menu given the parent window, the window pointer, an action context and the position
 * of the menu.
 */
void menu_showContextMenu(HWND hwndParent, WINFO* wp, const char* pActionContext, int x, int y) {
	if (_contextMenu != NULL) {
		DestroyMenu(_contextMenu);
	}
	_contextMenu = CreatePopupMenu();
	if (!menu_populateContextMenu(wp, pActionContext)) {
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
long long menu_openContextMenu() {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (!wp) {
		return 0;
	}
	POINT pt;
	GetCursorPos(&pt);
	menu_showContextMenu(wp->ww_handle, wp, wp->actionContext, pt.x, pt.y);
	return 1;
}

/*
 * Returns the position where the last context menu was shown.
 */
POINT menu_getContextMenuPopupPosition() {
	return _contextMenuPosition;
}

/*
 * Clear out a popup menu.
 */
static void menu_removeAllItems(HMENU hMenu) {
	for (int i = GetMenuItemCount(hMenu); --i >= 0; ) {
		HMENU hSub = GetSubMenu(hMenu, i);
		if (hSub) {
			DestroyMenu(hSub);
		}
		RemoveMenu(hMenu, i, MF_BYPOSITION);
	}
}

/*
 * Updates a popup of the menu bar every time the popup is opened.
 */
void menu_updateMenubarPopup(HMENU hMenu) {
	WINFO* wp = ww_getCurrentEditorWindow();
	const char* pszContext = wp ? wp->actionContext : DEFAULT_ACTION_CONTEXT;

	MENU_ITEM_DEFINITION* pMenu = bindings_getMenuBarPopupDefinitionFor(pszContext, hMenu);
	if (pMenu != NULL) {
		menu_removeAllItems(hMenu);
		menu_appendMenuItems(ww_getCurrentEditorWindow(), hMenu, pMenu);
	}
}

/*
 * Create the main application menu bar from the actionbinding definitions loaded before.
 */
HMENU menu_createMenubar() {
	HMENU hMenuBar = CreateMenu();
	MENU_ITEM_DEFINITION* pMenu = bindings_getMenuBarFor(DEFAULT_ACTION_CONTEXT);
	char szLabel[100];
	char charsWithMnemonic[256];
	memset(charsWithMnemonic, 0, sizeof charsWithMnemonic);
	while (pMenu) {
		HMENU hMenu = CreateMenu();
		pMenu->mid_menuHandle = hMenu;
		menu_determineLabelWithMnemonic(szLabel, pMenu, charsWithMnemonic, TRUE);
		AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hMenu, szLabel);
		pMenu = pMenu->mid_next;
	}
	return hMenuBar;
}

/*
 * Selects the action context for the main menu of PKS Edit (e.g. when the currently focussed editor window changes).
 */
void menu_selectActionContext(const char* pszContext) {
	// TODO....
}

