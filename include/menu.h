/*
 * menu.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: menu and context menu handling in PKS-Edit
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
  * Open the context menu in the current editor window (if any) close to the current mouse position.
  * The context menu will be popuplated with the appropriate items. Return 1 if successful, 0 otherwise.
  */
extern int menu_openContextMenu();

/*
 * Returns the position where the last context menu was shown.
 */
extern POINT menu_getContextMenuPopupPosition();

#define MENU_H
#endif
