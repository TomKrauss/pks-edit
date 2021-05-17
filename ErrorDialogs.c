/*
 * ErrorDialogs.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: error alert message boxes and other
 * type of user notification in case of errors.
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"
#include "winfo.h"
#include "winterf.h"

# pragma hdrstop

#include <stdarg.h>
#include "dial2.h"
#include "pksedit.h"

extern char *	AbbrevName(char *fn);
extern void 	st_seterrmsg(char *msg);
extern void 	st_update(void);

/*------------------------------------------------------------
 * nAlert()
 */
static int nAlert(int buttons, LPSTR fmt, LPSTR ap)
{   char buf[256];

    wvsprintf((LPSTR)buf,(LPSTR)fmt,(LPSTR)ap);
    return MessageBox(hwndFrame,buf,szAppName,MB_APPLMODAL|buttons); 
}

/*------------------------------------------------------------
 * Form_Alert()
 */
int Form_Alert(long buttons, long nOptions, LPSTR fmt)
{
	if (!fmt) {
		return -1;
	}
	return MessageBox(hwndFrame, fmt, szAppName, MB_APPLMODAL|buttons); 
}

/*------------------------------------------------------------
 * nIdAlert()
 */
static int nIdAlert(int buttons, WORD nId, LPSTR ap)
{
    char fmt[128];

	if (!LoadString(hInst,nId,fmt,sizeof fmt))
		return -1;
    return nAlert(buttons,fmt,ap);
}

/*------------------------------------------------------------
 * alert()
 */
void alert(LPSTR fmt, ...)
{   va_list ap;

    va_start(ap,fmt);
    // make Alert boxes system modal, cause they may be opened
    // in situations, we should not use the input focus
    (void)nAlert(MB_OK|MB_ICONEXCLAMATION,fmt,ap);
    va_end(ap);
}

/*------------------------------------------------------------
 * ed_yn()
 */
int ed_yn(WORD nId, ...)
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
 * ed_ync()
 */
int ed_ync(WORD nId, ...)
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
    alert(s);
}

/*------------------------------------------------------------
 * DlgError()
 */
#define	NSEC		5		/* stay open maximum 10 seconds */
static HWND hwndError;
static UINT idTimer;
BOOL CALLBACK DlgError(HWND hDlg,UINT message,WPARAM wParam, LPARAM lParam)
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
 * ShowError()
 */
void ShowError(LPSTR fmt, va_list ap)
{ 	static FARPROC lpfnDlgProc;
	char buf[256];

	wvsprintf(buf,fmt,ap);
	st_seterrmsg(buf);
	st_update();

	if ((GetConfiguration()->options & O_MESSAGES) == 0) {
		return;
	}

	CreateModelessDialog(&hwndError,"DLGERROR",
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
static void err_show(int nId, va_list ap)
{
	char		fmt[128];

	if (LoadString(hInst,nId,fmt,sizeof fmt)) {
		ShowError(fmt,ap);
	}
}

/*--------------------------------------------------------------------------
 * Out()
 */
void Out(char *fmt, ...)
{
	va_list 	ap;

	va_start(ap,fmt);
	ShowError(fmt, ap);
	va_end(ap);
}

/*------------------------------------------------------------
 * ShowMessage()
 * Show an info or error message - primarily in the status bar of PKS Edit.
 */
void ShowMessage(WORD nId, ...) {
	va_list 	ap;

	va_start(ap,nId);
	err_show(nId,ap);
	va_end(ap);
}

/*------------------------------------------------------------
 * CloseErrorWin()
 */
void CloseErrorWin(void)
{
	st_seterrmsg((char *)0);
	if (hwndError) {
		ShowWindow(hwndError,SW_HIDE);
	}
}

/*--------------------------------------------------------------------------
 * visiblebell()
 */
static void visiblebell(void)
{
	FlashWindow(hwndFrame,1);
	FlashWindow(hwndFrame,0);
}

/*------------------------------------------------------------
 * ed_error()
 * Display an error alert given a resource ID + optional arguments.
 */
void ed_error(int nId,...)
{
	va_list 	ap;
	char 	s[128];

	if (!LoadString(hInst,nId,s,sizeof s))
		return;

	va_start(ap,nId);

	if (*s == '!') {
	    (void)nAlert(MB_OK|MB_ICONHAND,s+1,ap);
	} else {
		if (GetConfiguration()->options & WARNINGS)
			ShowError(s,ap);
		if (GetConfiguration()->options & E_BELL)
			chime();
		if (GetConfiguration()->options & E_FLASH)
			visiblebell();
	}
	va_end(ap);
}

/*------------------------------------------------------------
 * form_error()
 */
void form_error(int num)
{	char buf[100];

	wsprintf(buf,/*STR*/"Allgemeiner Fehler",num);
	alert(buf);
}

/*------------------------------------------------------------
 * tosfnerror()
 */
void tosfnerror(char *fn,int fd)
{
	ed_error(IDS_MSGOPEN,(LPSTR)AbbrevName(fn));
}
