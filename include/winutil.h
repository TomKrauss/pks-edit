/*
 * winutil.h
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: utilities for window coordinate caluclation.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 */

#ifndef WINUTIL_H
#include <windows.h>

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
 * win_getstate()
 * Get the current window placement.
 */
extern void win_getstate(HWND hwnd, WINDOWPLACEMENT* wsp);

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


#define WINUTIL_H

#endif // !WINUTIL_H
