/*
 * winutil.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: utilities for window coordinate caluclation.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef WINUTIL_H
#include <windows.h>

 /*--------------------------------------------------------------------------
  * win_sendRedrawToWindow()
  * Send a repaint to the window with the passed handle and update it.
  */
extern void win_sendRedrawToWindow(HWND hwnd);

/*--------------------------------------------------------------------------
 * win_moveWindowToXY()
 */
extern void win_moveWindowToXY(HWND hwnd, int meetpoint, int x, int y);

/*--------------------------------------------------------------------------
 * win_positionWindowRelativeToCaret()
 * Move a window relative to the caret of the current active window.
 */
extern int win_positionWindowRelativeToCaret(HWND hwnd);

/*--------------------------------------------------------------------------
 * win_moveWindowCloseToMousePosition()
 * Move a window close to the current mouse position.
 */
extern int win_moveWindowCloseToMousePosition(HWND hwnd);

/*--------------------------------------------------------------------------
 * win_centerWindow()
 * Move the window into the center to the screen.
 */
extern int win_centerWindow(HWND hwnd);


/*--------------------------------------------------------------------------
 * win_moveWindowToDefaultPosition()
 * Move a window relative to either the mouse or position it to the center of
 * the screen depending on the configuration.
 */
extern int win_moveWindowToDefaultPosition(HWND hDlg);

/*--------------------------------------------------------------------------
 * win_destroyModelessDialog()
 * Destroy the current modeless dialog. Pass a "pointer!" to a win handle.
 */
extern void win_destroyModelessDialog(HWND* hwnd);

/**
 * Calculate the dimensions of a text and return the size encoded as a LONG.
 */
extern LONG win_getTextExtent(HDC hdc, char* text, int len);

/*
 * Set the logical extension of the viewport and return the size encoded as a LONG.
 */
extern LONG win_setViewportExtension(HDC hdc, int nx, int ny);

/*
 * Set the extension of the window and return the size encoded as a LONG.
 */
extern LONG win_setWindowExtension(HDC hdc, int nx, int ny);

/*
 * Return the extension of the window encoded as a LONG.
 */
extern LONG win_getWindowExtension(HDC hdc);

/*------------------------------------------------------------
 * win_sendParentCommand()
 */
extern void win_sendParentCommand(HWND hwnd, LPARAM lParam);

/*--------------------------------------------------------------------------
 * win_createModelessDialog()
 * Create a modeless dialog, given a callback and a dialog proc.
 */
extern void win_createModelessDialog(HWND* hwnd, LPSTR szName, INT_PTR(CALLBACK* func)(HWND, UINT, WPARAM, LPARAM),
	DLGPROC* lplpfnDlgProc);

#define WINUTIL_H

#endif // !WINUTIL_H
