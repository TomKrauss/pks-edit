/*
 * ProgressMonitor.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * display a progress monitor, which can be cancelled by the user.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <windows.h>
#include <commctrl.h>
#include "winterf.h"
#include "trace.h"
#include "lineoperations.h"
#include "winfo.h"
#include "winutil.h"
#include "errordialogs.h"
#include "pksrc.h"
#include "pksedit.h"
#include "dial2.h"
#include "xdialog.h"

extern int	_playing;

HWND hwndAbort;

static DLGPROC lpfnAbort;
static int  	_cancelled;

/*------------------------------------------------------------
 * DlgProgressProc()
 */
static INT_PTR DlgProgressProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_INITDIALOG:
			_cancelled = 0;
			return (INT_PTR)TRUE;

		case WM_COMMAND:
		case WM_CLOSE:
			if (error_displayYesNoConfirmation(IDS_MSGABRT) == IDYES) {
				_cancelled = 1;
			}
			return (INT_PTR)TRUE;

		case WM_DESTROY:
			hwndAbort = 0;
			return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

/*------------------------------------------------------------
 * progress_startMonitor()
 */
void progress_startMonitor(unsigned int ids) {
	char szB1[30],szBuff[256];

	if (!_playing && !hwndAbort) {
		szB1[0] = 0;
		LoadString(ui_getResourceModule(),ids,szB1,sizeof szB1);
		wsprintf(szBuff,"%s..",szB1);
		win_createModelessDialog(&hwndAbort,"DLGABORT",
						  DlgProgressProc,&lpfnAbort);
		if (hwndAbort) {
			HWND hwndPB = GetDlgItem(hwndAbort, IDC_PROGRESS);
			SendMessage(hwndPB, PBM_SETMARQUEE, (WPARAM)1, 40);
			EnableWindow(hwndMDIFrameWindow,FALSE);
			SetDlgItemText(hwndAbort, IDD_STRING1, szBuff);
		}
	}
}

/*------------------------------------------------------------
 * progress_stepIndicator()
 * Animate the actual progress bar displayed in the progress dialog.
 */
void progress_stepIndicator() {
	if (hwndAbort) {
		HWND hwndPB = GetDlgItem(hwndAbort, IDC_PROGRESS);
		MSG msg;
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE | PM_NOYIELD)) {
			DispatchMessage(&msg);
		}
	}
}

/*------------------------------------------------------------
 * progress_showMonitorMessage()
 * Show a progress message and animate the progress control.
 */
void progress_showMonitorMessage(const char* message)
{
	if (hwndAbort) {
		SetDlgItemText(hwndAbort, IDD_STRING2, message);
		progress_stepIndicator();
	}
}

/*------------------------------------------------------------
 * progress_closeMonitor()
 */
void progress_closeMonitor(int always)
{
	if (hwndAbort) {
		EnableWindow(hwndMDIFrameWindow,TRUE);
		win_destroyModelessDialog(&hwndAbort);
	}
}

/*------------------------------------------------------------
 * progress_cancelMonitor()
 * optional force redraw to reflect changes in the current
 * top window due to a workin progress
 */
int progress_cancelMonitor(BOOL bRedraw)
{
	MSG			msg;
	static int 	count;

	if (hwndAbort == NULL) {
		return 0;
	}

	if (PeekMessage(&msg,0,0,0,PM_REMOVE)) {
		if (!IsDialogMessage(hwndAbort,&msg)) {
			DispatchMessage(&msg);
			WINFO* wp = ww_getCurrentEditorWindow();
			if (bRedraw && wp && msg.message == WM_PAINT && (count++ & 0x3) == 0) {
				win_sendRedrawToWindow(wp->ww_handle);
			}
		}
	}

	return _cancelled;
}


