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

static FARPROC lpfnAbort;
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
			if (ed_yn(IDS_MSGABRT) == IDYES) {
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
 * ProgressMonitorStart()
 */
void ProgressMonitorStart(unsigned int ids) {
	char szB1[30],szBuff[256];

	if (!_playing && !hwndAbort) {
		szB1[0] = 0;
		LoadString(hInst,ids,szB1,sizeof szB1);
		wsprintf(szBuff,"%s..",szB1);
		CreateModelessDialog(&hwndAbort,"DLGABORT",
						  DlgProgressProc,&lpfnAbort);
		if (hwndAbort) {
			EnableWindow(hwndFrame,FALSE);
			SetDlgItemText(hwndAbort, IDD_STRING1, szBuff);
		}
	}
}

/*------------------------------------------------------------
 * ProgressMonitorShowMessage()
 */
void ProgressMonitorShowMessage(LPSTR message)
{
	if (hwndAbort)
		SetDlgItemText(hwndAbort, IDD_STRING2, message);
}

/*------------------------------------------------------------
 * ProgressMonitorClose()
 */
void ProgressMonitorClose(int always)
{
	if (hwndAbort) {
		EnableWindow(hwndFrame,TRUE);
		DestroyModelessDialog(&hwndAbort);
	}
}

/*------------------------------------------------------------
 * ProgressMonitorCancel()
 * optional force redraw to reflect changes in the current
 * top window due to a workin progress
 */
int ProgressMonitorCancel(BOOL bRedraw)
{
	MSG			msg;
	static int 	count;

	if (hwndAbort == NULL) {
		return 0;
	}

	if (PeekMessage(&msg,0,0,0,PM_REMOVE)) {
		if (!IsDialogMessage(hwndAbort,&msg)) {
			DispatchMessage(&msg);
			if (bRedraw && _currfile && msg.message == WM_PAINT && (count++ & 0x3) == 0) {
				SendRedraw(WIPOI(_currfile)->ww_handle);
			}
		}
	}

	return _cancelled;
}


