/*
 * ProgressMonitor.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * display a progress monitor, which can be cancelled by the user.
 *
 * 										created: 
 * 										last modified:
 *										author: TOM
 *
 * (c) Pahlen & Krauss
 */
#include <windows.h>

#include "winterf.h"
#include "trace.h"
#include "lineoperations.h"
#include "winfo.h"
#include "errordialogs.h"

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
static BOOL CALLBACK DlgProgressProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_INITDIALOG:
			_cancelled = 0;
			return TRUE;

		case WM_COMMAND:
		case WM_CLOSE:
			if (errorDisplayYesNoConfirmation(IDS_MSGABRT) == IDYES) {
				_cancelled = 1;
			}
			return TRUE;

		case WM_DESTROY:
			hwndAbort = 0;
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------
 * progress_startMonitor()
 */
void progress_startMonitor(unsigned int ids) {
	char szB1[30],szBuff[256];

	if (!_playing && !hwndAbort) {
		szB1[0] = 0;
		LoadString(hInst,ids,szB1,sizeof szB1);
		wsprintf(szBuff,"%s..",szB1);
		win_createModelessDialog(&hwndAbort,"DLGABORT",
						  DlgProgressProc,&lpfnAbort);
		if (hwndAbort) {
			EnableWindow(hwndFrame,FALSE);
			SetDlgItemText(hwndAbort, IDD_STRING1, szBuff);
		}
	}
}

/*------------------------------------------------------------
 * progress_showMonitorMessage()
 */
void progress_showMonitorMessage(const char* message)
{
	if (hwndAbort)
		SetDlgItemText(hwndAbort, IDD_STRING2, message);
}

/*------------------------------------------------------------
 * progress_closeMonitor()
 */
void progress_closeMonitor(int always)
{
	if (hwndAbort) {
		EnableWindow(hwndFrame,TRUE);
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
			if (bRedraw && ft_getCurrentDocument() && msg.message == WM_PAINT && (count++ & 0x3) == 0) {
				render_sendRedrawToWindow(WIPOI(ft_getCurrentDocument())->ww_handle);
			}
		}
	}

	return _cancelled;
}


