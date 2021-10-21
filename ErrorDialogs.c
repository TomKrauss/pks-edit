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

#include <windows.h>
#include <stdio.h>
#include <CommCtrl.h>
#include "alloc.h"
#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"
#include "winfo.h"
#include "winterf.h"
#include "winutil.h"
#include "stringutil.h"
#include "themes.h"
#include "customcontrols.h"

#include <stdarg.h>
#include "dial2.h"
#include "pksedit.h"
#include "xdialog.h"

/*--------------------------------------------------------------------------
 * sound_playChime()
 * Play a chime sound.
 */
extern void sound_playChime(void);

extern void 	st_seterrmsg(char *msg);
extern void 	st_update(void);

static HHOOK hHook;

static LRESULT WINAPI error_messageBoxHook(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode < 0) {
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	}
	CWPSTRUCT* pStruct = (CWPSTRUCT*)lParam;
	if (pStruct->message == WM_INITDIALOG || pStruct->message == WM_CTLCOLORDLG) {
		return dlg_defaultWndProc(pStruct->hwnd, pStruct->message, pStruct->wParam, pStruct->lParam);
	}
	return 1;
}
/*------------------------------------------------------------
 * error_displayAlertBoxWithOptions()
 */
int error_displayAlertBoxWithOptions(long unused, long buttons, const char* fmt) {
	if (!fmt) {
		return -1;
	}
	wchar_t szwAppName[64];
	wchar_t szwFmt[128];
	wchar_t szwDetails[5];
	mbstowcs(szwAppName, szAppName, strlen(szAppName) + 1);
	mbstowcs(szwFmt, fmt, strlen(fmt) + 1);
	mbstowcs(szwDetails, "", 1);
	hHook = SetWindowsHookEx(WH_CALLWNDPROC, error_messageBoxHook, hInst, GetCurrentThreadId());
	int nButtons = 0;
	PCWSTR nIcon = TD_INFORMATION_ICON;
	int nOldButtons = buttons & 0xF;
	int nOldIcon = buttons & 0xF0;
	if (nOldButtons == MB_OK) {
		nButtons |= TDCBF_OK_BUTTON;
	} else if (nOldButtons == MB_OKCANCEL) {
		nButtons |= TDCBF_CANCEL_BUTTON;
		nButtons |= TDCBF_OK_BUTTON;
	} else if (nOldButtons == MB_RETRYCANCEL) {
		nButtons |= TDCBF_RETRY_BUTTON;
		nButtons |= TDCBF_CANCEL_BUTTON;
	} else if (nOldButtons == MB_YESNOCANCEL) {
		nButtons |= TDCBF_YES_BUTTON;
		nButtons |= TDCBF_NO_BUTTON;
		nButtons |= TDCBF_CANCEL_BUTTON;
	} else if (nOldButtons == MB_YESNO) {
		nButtons |= TDCBF_YES_BUTTON;
		nButtons |= TDCBF_NO_BUTTON;
	}
	if (nOldIcon == MB_ICONSTOP) {
		nIcon = TD_SHIELD_ICON;
	} else if (nOldIcon == MB_ICONEXCLAMATION) {
		nIcon = TD_ERROR_ICON;
	} else if (nOldIcon == MB_ICONWARNING) {
		nIcon = TD_WARNING_ICON;
	}
	int ret;
	TaskDialog(hwndMain, hInst, szwAppName, szwFmt, szwDetails, nButtons, nIcon, &ret);
	UnhookWindowsHookEx(hHook);
	return ret;
}

/*------------------------------------------------------------
 * error_openConfigurableAlert()
 */
static int error_openConfigurableAlert(int buttons, LPSTR fmt, va_list ap)
{   char buf[256];

    wvsprintf((LPSTR)buf,(LPSTR)fmt, ap);
	return error_displayAlertBoxWithOptions(0, buttons, buf);
}

/*------------------------------------------------------------
 * nIdAlert()
 */
static int nIdAlert(int buttons, WORD nId, va_list ap)
{
    char* pszBuf;

	if ((pszBuf = dlg_getResourceString(nId)) == NULL) 
		return -1;
    return error_openConfigurableAlert(buttons, pszBuf,ap);
}

/*------------------------------------------------------------
 * error_displayAlertDialog()
 * Display an alert dialog box.
 */
void error_displayAlertDialog(const char* fmt, ...)
{   va_list ap;

    va_start(ap,fmt);
    // make Alert boxes system modal, cause they may be opened
    // in situations, we should not use the input focus
    (void)error_openConfigurableAlert(MB_OK|MB_ICONEXCLAMATION,(char*)fmt,ap);
    va_end(ap);
}

/*------------------------------------------------------------
 * error_displayYesNoConfirmation()
 * Display a message box to be answered with yes/no or cancel.
 */
int error_displayYesNoConfirmation(int nId, ...)
{   va_list ap;
    int ret;
    
	if (!(GetConfiguration()->options & O_WARNINGS))
		return IDYES;

	va_start(ap,nId);
	ret = nIdAlert(MB_YESNO|MB_ICONQUESTION,nId, ap);
	va_end(ap);
	return ret;
}

/*------------------------------------------------------------
 * error_displayYesNoCancelConfirmation()
 * Display a message box to be answered with yes/no or cancel.
 */
int error_displayYesNoCancelConfirmation(int nId, ...)
{	va_list ap;
	int ret;

	if (!(GetConfiguration()->options & O_WARNINGS))
		return IDYES;

	va_start(ap,nId);
	ret = nIdAlert(MB_YESNOCANCEL|MB_ICONQUESTION,nId,ap);
	va_end(ap);
	return ret;
}

/*------------------------------------------------------------
 * Panic()
 */
void Panic(LPSTR s)
{
    error_displayAlertDialog(s);
}

/*------------------------------------------------------------
 * error_displayErrorToast()
 * If configured, popup a temporary dialog window showing an error.
 */
void error_displayErrorToast(const char* fmt, va_list ap)
{
	char buf[256];

	wvsprintf(buf,fmt,ap);
	st_seterrmsg(buf);
	st_update();

	if ((GetConfiguration()->options & O_SHOW_MESSAGES_IN_SNACKBAR) == 0) {
		return;
	}

	cust_createToastWindow(buf);
}

/*------------------------------------------------------------
 * err_show()
 */
static void err_show(int nId, va_list ap) {
	char* pszBuf;

	if ((pszBuf = dlg_getResourceString(nId)) != NULL) {
		error_displayErrorToast(pszBuf, ap);
	}
}

/*------------------------------------------------------------
 * error_showMessageInStatusbar()
 * Show an info or error message - primarily in the status bar of PKS Edit.
 */
void error_showMessageInStatusbar(int nId, ...) {
	va_list 	ap;

	va_start(ap,nId);
	err_show(nId,ap);
	va_end(ap);
}

/*------------------------------------------------------------
 * error_closeErrorWindow()
 */
void error_closeErrorWindow(void)
{
	st_seterrmsg((char *)0);
}

/*--------------------------------------------------------------------------
 * error_signalUsingFlashing()
 */
static void error_signalUsingFlashing(void)
{
	FlashWindow(hwndMain,1);
	FlashWindow(hwndMain,0);
}

/*
 * Display an error in a "non-intrusive way" (status line etc...). 
 */
void error_showError(char* s, va_list ap) {
	if (GetConfiguration()->options & O_WARNINGS)
		error_displayErrorToast(s, ap);
	if (GetConfiguration()->options & O_ERROR_TONE)
		sound_playChime();
	if (GetConfiguration()->options & O_ERROR_FLASH_WINDOW)
		error_signalUsingFlashing();
}

/*------------------------------------------------------------
 * error_showErrorById()
 * Display an error error_displayAlertDialog given a resource ID + optional arguments.
 */
void error_showErrorById(int nId,...)
{
	va_list 	ap;
	char 	s[128];

	if (!LoadString(ui_getResourceModule(), nId, s, sizeof s)) {
		sprintf(s, "Cannot find resource with id %d", nId);
	}

	va_start(ap,nId);

	if (*s == '!') {
	    (void)error_openConfigurableAlert(MB_OK|MB_ICONHAND,s+1,ap);
	} else {
		error_showError(s, ap);
	}
	va_end(ap);
}

/*------------------------------------------------------------
 * error_displayGenericErrorNumber()
 * Report an arbitrary error with an error number.
 */
void error_displayGenericErrorNumber(int num)
{	char buf[100];

	wsprintf(buf,/*STR*/"Allgemeiner Fehler %d", num);
	error_displayAlertDialog(buf);
}

/*------------------------------------------------------------
 * error_openingFileFailed()
 * Display an (OS level) error about a file.
 */
void error_openingFileFailed(char *fn, int fd)
{
	error_showErrorById(IDS_MSGOPEN,(LPSTR)string_abbreviateFileName(fn));
}
