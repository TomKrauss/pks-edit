/*
 * windowselector.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: allows Ctrl+TAB selection of active editor window.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef WINDOWSELECTOR_H
#define WINDOWSELECTOR_H

/*------------------------------------------------------------
 * windowselector_registerWindowClass()
 */
extern int windowselector_registerWindowClass();

/*--------------------------------------------------------------------------
 * windowselector_showWindowList()
 * Shows the window selector list.
 */
extern long long windowselector_showWindowList();

/*
 * Returns != 0, if the passed HWND is a window handle to be used by the window selector.
 */
extern BOOL windowselector_isHandle(const HWND hwnd);

#endif
