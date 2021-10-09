/*
 * darkmode.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: undocumented Windows darkmode features.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef DARKMODE_H

 /*
  * Initializes Darkmode handling
  */
extern void darkmode_initialize();

extern void darkmode_refreshTitleBarThemeColor(HWND hWnd);

/*
 * Enables dark mode in the corresponding window.
 */
extern BOOL darkmode_allowForWindow(HWND hWnd, BOOL allow);

#define DARKMODE_H
#endif // !DARKMODE_H


