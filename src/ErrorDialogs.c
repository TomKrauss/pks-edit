/*
 * ErrorDialogs.c
 *
 * Project: PKS Edit for Windows
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
#include "documentmodel.h"
#include "edierror.h"
#include "errordialogs.h"
#include "fileutil.h"
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

static HHOOK hHook;

static BOOL _messagesDisabled;

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
int error_displayAlertBoxWithOptions(long buttons, const char* fmt) {
	if (!fmt || hHook) {
		return -1;
	}
	wchar_t szwAppName[64];
	wchar_t szwFmt[512];
	wchar_t szwDetails[5];
	mbstowcs(szwAppName, szAppName, strlen(szAppName) + 1);
	size_t nSize = strlen(fmt) + 1;
	if (nSize > sizeof(szwFmt) / sizeof(szwFmt[0])) {
		nSize = 500;
	}
	mbstowcs(szwFmt, fmt, nSize);
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
		nIcon = TD_ERROR_ICON;
	} else if (nOldIcon == MB_ICONEXCLAMATION) {
		nIcon = TD_ERROR_ICON;
	} else if (nOldIcon == MB_ICONWARNING) {
		nIcon = TD_WARNING_ICON;
	}
	int ret;
	TaskDialog(hwndMain, hInst, szwAppName, szwFmt, szwDetails, nButtons, nIcon, &ret);
	UnhookWindowsHookEx(hHook);
	hHook = NULL;
	return ret;
}

/*------------------------------------------------------------
 * error_openConfigurableAlert()
 */
static int error_openConfigurableAlert(int buttons, LPSTR fmt, va_list ap)
{   char szBuf[1024];

	// 1024 == maximum result size for wvsprintf
    wvsprintf(szBuf,(LPSTR)fmt, ap);
	int ret = error_displayAlertBoxWithOptions(buttons, szBuf);
	return ret;
}

/*------------------------------------------------------------
 * error_openAlertBox()
 */
static int error_openAlertBox(int buttons, WORD nId, va_list ap)
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
	error_vdisplayAlertDialog(fmt, ap);
    va_end(ap);
}

/*------------------------------------------------------------
 * error_displayAlertDialog()
 * Display an alert dialog box passing a variadic arg list.
 */
void error_vdisplayAlertDialog(const char* fmt, va_list ap) {
	(void)error_openConfigurableAlert(MB_OK | MB_ICONEXCLAMATION, (char*)fmt, ap);
}

/*------------------------------------------------------------
 * error_displayYesNoConfirmation()
 * Display a message box to be answered with yes/no or cancel.
 */
int error_displayYesNoConfirmation(int nId, ...)
{   va_list ap;
    int ret;
    
	va_start(ap,nId);
	ret = error_openAlertBox(MB_YESNO|MB_ICONQUESTION,nId, ap);
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

	va_start(ap,nId);
	ret = error_openAlertBox(MB_YESNOCANCEL|MB_ICONQUESTION,nId,ap);
	va_end(ap);
	return ret;
}

/*------------------------------------------------------------
 * error_displayErrorToast()
 * If configured, popup a temporary dialog window showing an error.
 */
static void error_displayErrorToast(const char* fmt, va_list ap) {
	char szBuf[1024];
	BOOL bShowToast = fmt[0] != '~';

	if (_messagesDisabled) {
		return;
	}
	wvsprintf(szBuf,bShowToast ? fmt:fmt+1,ap);
	st_setStatusLineMessage(szBuf);

	if (_startupComplete && bShowToast && (GetConfiguration()->options & O_SHOW_MESSAGES_IN_SNACKBAR) != 0) {
		cust_createToastWindow(szBuf);
	}
}

/*
 * Display an error message in a toast window.
 */
void error_displayErrorInToastWindow(const char* fmt, ...) {
	va_list 	ap;

	va_start(ap, fmt);
	error_displayErrorToast(fmt, ap);
	va_end(ap);
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

	if (_messagesDisabled) {
		return;
	}
	va_start(ap,nId);
	err_show(nId,ap);
	va_end(ap);
}

/*------------------------------------------------------------
 * error_closeErrorWindow()
 */
void error_closeErrorWindow(void)
{
	st_setStatusLineMessage((char *)0);
}

/*--------------------------------------------------------------------------
 * error_signalUsingFlashing()
 */
static void error_signalUsingFlashing(void)
{
	if (_messagesDisabled) {
		return;
	}
	FlashWindow(hwndMain,1);
	FlashWindow(hwndMain,0);
}

/*
 * Display an error in a "non-intrusive way" (status line etc...). 
 */
void error_showError(char* s, va_list ap) {
	if (_messagesDisabled) {
		return;
	}
	error_displayErrorToast(s, ap);
	if (GetConfiguration()->options & O_ERROR_TONE)
		sound_playChime();
	if (GetConfiguration()->options & O_ERROR_FLASH_WINDOW)
		error_signalUsingFlashing();
}

/*
 * Show an error message with a string and optional parameters. 
 */
void error_showMessage(char* s, ...) {
	va_list ap;
	va_start(ap, s);
	if (*s == '!') {
		(void)error_openConfigurableAlert(MB_OK | MB_ICONHAND, s + 1, ap);
	}
	else {
		error_showError(s, ap);
	}
	va_end(ap);
}

/*------------------------------------------------------------
 * error_showErrorById()
 * Display an error error_displayAlertDialog given a resource ID + optional arguments.
 */
void error_showErrorById(int nId, ...) {
	va_list 	ap;
	char* s;
	char        buf[256];

	if ((s = dlg_getResourceString(nId)) == 0) {
		s = buf;
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

	wsprintf(buf,/*STR*/"Generic error: %d", num);
	error_displayAlertDialog(buf);
}

/*------------------------------------------------------------
 * error_openingFileFailed()
 * Display an (OS level) error about a file.
 */
void error_openingFileFailed(char *fn, BOOL bNonBlocking, int fd) {
	BOOL bDirectory = FALSE;
	file_getFileAttributes(fn, NULL, NULL, &bDirectory);
	if (bDirectory) {
		if (bNonBlocking) {
			error_showMessageInStatusbar(IDS_FILE_IS_DIRECTORY, (LPSTR)string_abbreviateFileName(fn));
		} else {
			error_showErrorById(IDS_FILE_IS_DIRECTORY, (LPSTR)string_abbreviateFileName(fn));
		}
		return;
	}
	DWORD dw = GetLastError();
	LPVOID lpMsgBuf = 0;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);
	if (bNonBlocking) {
		error_showMessageInStatusbar(IDS_MSGOPEN, (LPSTR)string_abbreviateFileName(fn), lpMsgBuf);
	}
	else {
		error_showErrorById(IDS_MSGOPEN, (LPSTR)string_abbreviateFileName(fn), lpMsgBuf);
	}
	LocalFree(lpMsgBuf);
}

/*
 * Can be used to temporarily disable the display of status messages in the status bar or the toast window.
 * Useful and to be called from within macros to make execution of macros less verbose.
 */
long long error_setShowMessages(BOOL aFlag) {
	_messagesDisabled = !aFlag;
	return 1;
}

