/*
 * ErrorDialogs.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: error error_displayAlertDialog message boxes and other
 * type of user notification in case of errors.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 */

#ifndef ERRORDIALOGS_H

#include <windows.h>
 
 /*------------------------------------------------------------
  * error_displayAlertDialog()
  * Display an alert dialog box.
  */
extern void error_displayAlertDialog(const char* fmt, ...);

/*------------------------------------------------------------
 * errorDisplayYesNoConfirmation()
 * Confirmation message box with yes no.
 */
extern int errorDisplayYesNoConfirmation(int nId, ...);

/*------------------------------------------------------------
 * error_displayYesNoCancelConfirmation()
 * Confirmation message box with yes no cancel.
 */
extern int error_displayYesNoCancelConfirmation(int nId, ...);

/*------------------------------------------------------------
 * error_showErrorById()
 * Display an error error_displayAlertDialog given a resource ID + optional arguments.
 */
extern void error_showErrorById(int nId, ...);

/*------------------------------------------------------------
 * error_openingFileFailed()
 * Display an (OS level) error about a file.
 */
extern void error_openingFileFailed(char* fn, int fd);

/*------------------------------------------------------------
 * error_displayGenericErrorNumber()
 * Report an arbitrary error with an error number.
 */
extern void error_displayGenericErrorNumber(int num);

/*------------------------------------------------------------
 * error_showErrorById()
 * Display an error error_displayAlertDialog given a resource ID + optional arguments.
 */
extern void error_showErrorById(int nId, ...);

/*------------------------------------------------------------
 * error_displayErrorToast()
 * If configured, popup a temporary dialog window showing an error.
 */
extern void error_displayErrorToast(const char* fmt, va_list ap);

/*
 * Display an error in a "non-intrusive way" (status line etc...).
 */
extern void error_showError(char* s, va_list ap);

/*------------------------------------------------------------
 * error_showMessageInStatusbar()
 * Show an info or error message - primarily in the status bar of PKS Edit.
 */
extern void error_showMessageInStatusbar(int nId, ...);

/*------------------------------------------------------------
 * error_closeErrorWindow()
 */
extern void error_closeErrorWindow(void);

/*------------------------------------------------------------
 * errorDisplayYesNoConfirmation()
 * Display a message box to be answered with yes/no or cancel.
 */
extern int errorDisplayYesNoConfirmation(int nId, ...);

/*------------------------------------------------------------
 * error_displayYesNoCancelConfirmation()
 * Display a message box to be answered with yes/no or cancel.
 */
extern int error_displayYesNoCancelConfirmation(int nId, ...);

/*------------------------------------------------------------
 * error_displayAlertBoxWithOptions()
 */
extern int error_displayAlertBoxWithOptions(long buttons, long nOptions, const char* fmt);

/*--------------------------------------------------------------------------
 * win_positionWindowRelativeToCaret()
 * Move a window relative to the caret of the current active window.
 */
extern int win_positionWindowRelativeToCaret(HWND hwnd);


#define ERRORDIALOGS_H
#endif