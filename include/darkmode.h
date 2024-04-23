/*
 * darkmode.h
 *
 * Project: PKS Edit for Windows
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

extern void darkmode_refreshTitleBarThemeColor(HWND hWnd, BOOL aDarkFlag);

extern BOOL darkmode_isSelectedByDefault();

/*
 * Enables dark mode in the corresponding window.
 */
extern BOOL darkmode_allowForWindow(HWND hWnd, BOOL allow);

extern BOOL darkmode_IsColorSchemeChangeMessage(UINT message, LPARAM lParam);

/*
 * Allow darkmode for the app or deny it.
 */
extern void darkmode_allowForApp(BOOL allow);

/*
 * To be invoked, when dark mode changes.
 */
extern void darkmode_flushMenuThemes();

#define DARKMODE_H
#endif // !DARKMODE_H


