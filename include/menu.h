/*
 * menu.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: menu and context menu handling in PKS Edit
 *
 * 										created: 15.03.91
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MENU_H

 /*
  * Show the context menu given the parent window, the window pointer, an action context and the position
  * of the menu.
  */
extern void menu_showContextMenu(HWND hwndParent, WINFO* wp, const char* pActionContext, int x, int y);

 /*
  * Open the context menu in the current editor window (if any) close to the current mouse position.
  * The context menu will be popuplated with the appropriate items. Return 1 if successful, 0 otherwise.
  */
extern long long menu_openContextMenu();

/*
 * Returns the position where the last context menu was shown.
 */
extern POINT menu_getContextMenuPopupPosition();

/*
 * Selects the action context for the main menu of PKS Edit (e.g. when the currently focussed editor window changes).
 */
extern void menu_selectActionContext(const char* pszContext);

/*
 * Create the main application menu bar from the actionbinding definitions loaded before.
 */
extern HMENU menu_createMenubar();

/*
 * Updates a popup of the menu bar every time the popup is opened.
 */
extern void menu_updateMenubarPopup(HMENU hMenu);

#define MENU_H
#endif
