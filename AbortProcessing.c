/*
 * AbortProcessing.C
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: abort processing alert box
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */
#include <windows.h>

#include "winterf.h"
#include "trace.h"
#include "lineoperations.h"
#include "winfo.h"
#include "errordialogs.h"

#include "pksedit.h"
#include "dial2.h"

extern int	_playing;

HWND hwndAbort;

static FARPROC lpfnAbort;
static int  	_aborted;

/*------------------------------------------------------------
 * DlgAbortProc()
 */
BOOL CALLBACK DlgAbortProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
	switch(message) {
		case WM_INITDIALOG:
			_aborted = 0;
			return TRUE;

		case WM_COMMAND:
		case WM_CLOSE:
			if (ed_yn(IDS_MSGABRT) == IDYES) {
				_aborted = 1;
			}
			return TRUE;

		case WM_DESTROY:
			hwndAbort = 0;
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------
 * abrt_start()
 */
void abrt_start(WORD ids)
{
	char			szB1[30],szBuff[256];

	if (!_playing && !hwndAbort) {
		szB1[0] = 0;
		LoadString(hInst,ids,szB1,sizeof szB1);
		wsprintf(szBuff,"%s..",szB1);
		CreateModelessDialog(&hwndAbort,"DLGABORT",
						  DlgAbortProc,&lpfnAbort);
		if (hwndAbort) {
			EnableWindow(hwndFrame,FALSE);
			SetDlgItemText(hwndAbort, IDD_STRING1, szBuff);
		}
	}
}

/*------------------------------------------------------------
 * abrt_message()
 */
void abrt_message(LPSTR message)
{
	if (hwndAbort)
		SetDlgItemText(hwndAbort, IDD_STRING2, message);
}

/*------------------------------------------------------------
 * abrt_close()
 */
void abrt_close(int always)
{
	if (hwndAbort) {
		EnableWindow(hwndFrame,TRUE);
		DestroyModelessDialog(&hwndAbort);
	}
}

/*------------------------------------------------------------
 * ed_abort()
 * optional force redraw to reflect changes in the current
 * top window due to a workin progress
 */
int ed_abort(BOOL bRedraw)
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

	return _aborted;
}


