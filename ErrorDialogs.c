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
# pragma hdrstop

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

/*------------------------------------------------------------
 * errror_openConfigurableAlert()
 */
static int errror_openConfigurableAlert(int buttons, LPSTR fmt, LPSTR ap)
{   char buf[256];

    wvsprintf((LPSTR)buf,(LPSTR)fmt,(LPSTR)ap);
    return MessageBox(hwndMain,buf,szAppName,MB_APPLMODAL|buttons); 
}

/*------------------------------------------------------------
 * error_displayAlertBoxWithOptions()
 */
int error_displayAlertBoxWithOptions(long buttons, long nOptions, const char* fmt)
{
	if (!fmt) {
		return -1;
	}
	return MessageBox(hwndMain, fmt, szAppName, MB_APPLMODAL|buttons); 
}

/*------------------------------------------------------------
 * nIdAlert()
 */
static int nIdAlert(int buttons, WORD nId, LPSTR ap)
{
    char* pszBuf;

	if ((pszBuf = dlg_getResourceString(nId)) == NULL) 
		return -1;
    return errror_openConfigurableAlert(buttons, pszBuf,ap);
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
    (void)errror_openConfigurableAlert(MB_OK|MB_ICONEXCLAMATION,(char*)fmt,ap);
    va_end(ap);
}

/*------------------------------------------------------------
 * error_displayYesNoConfirmation()
 * Display a message box to be answered with yes/no or cancel.
 */
int error_displayYesNoConfirmation(int nId, ...)
{   va_list ap;
    int ret;
    
	if (!(GetConfiguration()->options & WARNINGS))
		return IDYES;

	va_start(ap,nId);
	ret = nIdAlert(MB_YESNO|MB_ICONQUESTION,nId,(LPSTR)ap);
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

	if (!(GetConfiguration()->options & WARNINGS))
		return IDYES;

	va_start(ap,nId);
	ret = nIdAlert(MB_YESNOCANCEL|MB_ICONQUESTION,nId,(LPSTR)ap);
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
 * DlgError()
 */
#define	NSEC		5		/* stay open maximum 10 seconds */
static HWND hwndError;
static UINT_PTR idTimer;
static INT_PTR DlgError(HWND hDlg,UINT message,WPARAM wParam, LPARAM lParam)
{ 	
	WINDOWPLACEMENT 	ws;

	switch(message) {
		case WM_INITDIALOG:
			if (prof_getwinstate("ErrWin",0,&ws)) {
				SetWindowPlacement(hDlg, &ws);
			}
			return FALSE;

		case WM_CLOSE:
		case WM_TIMER:
			ShowWindow(hDlg,SW_HIDE);
			return TRUE;

		case WM_MOVE:
		case WM_SIZE:
			win_getstate(hDlg,&ws);
			prof_savewinstate("ErrWin",0,&ws);
			break;

		case WM_DESTROY:
			if (idTimer) {
				KillTimer(hDlg,idTimer);
				idTimer = 0;
			}
			hwndError = 0;
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------
 * error_displayErrorToast()
 * If configured, popup a temporary dialog window showing an error.
 */
void error_displayErrorToast(const char* fmt, va_list ap)
{ 	static DLGPROC lpfnDlgProc;
	char buf[256];

	wvsprintf(buf,fmt,ap);
	st_seterrmsg(buf);
	st_update();

	if ((GetConfiguration()->options & O_MESSAGES) == 0) {
		return;
	}

	win_createModelessDialog(&hwndError,"DLGERROR",
					  DlgError,&lpfnDlgProc);
	if (hwndError) {
		ShowWindow(hwndError,SW_SHOWNOACTIVATE);
		SetDlgItemText(hwndError,IDD_STRING1,buf);
		if (idTimer) {
			KillTimer(hwndError,idTimer);
		}
		idTimer = SetTimer(hwndError,1,NSEC*1000,0);
	}
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
	if (hwndError) {
		ShowWindow(hwndError,SW_HIDE);
	}
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
	if (GetConfiguration()->options & WARNINGS)
		error_displayErrorToast(s, ap);
	if (GetConfiguration()->options & E_BELL)
		sound_playChime();
	if (GetConfiguration()->options & E_FLASH)
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

	if (!LoadString(ui_getResourceModule(),nId,s,sizeof s))
		return;

	va_start(ap,nId);

	if (*s == '!') {
	    (void)errror_openConfigurableAlert(MB_OK|MB_ICONHAND,s+1,ap);
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
