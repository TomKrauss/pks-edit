/*
 * KeyBinding.c
 *
 * PROJEKT: PKS-EDIT for ATARI - WINDOWS
 *
 * purpose: Function Key Set
 *
 * 										created      : 01.01.90
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include <string.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winterf.h"
#include "winfo.h"
#include "editorconfiguration.h"

#pragma hdrstop

#include "pksedit.h"
#include "dial2.h"
#include "edfuncs.h"

# undef IDD_FKFKLAST
# undef IDD_FKFLGLAST
#define 	IDD_FKFKLAST		IDD_FKFK1+11
#define 	IDD_FKFLGLAST		IDD_FKFLG1+11

#define 	MAX_FKEYS			(IDD_FKFKLAST-IDD_FKFK1+1)
#define 	NSHFT			4
#define 	FKSLEN			21
#define	FK_DELTA			6

#define FKSTATE_SIZE		20

extern FARPROC SubClassWndProc(WORD set, HWND hDlg, WORD item, FARPROC lpfnNewProc);
extern char *	mac_comment(LPSTR szBuf, LPSTR szB2, WORD nIndex, WORD type);
extern int 	cust_buttoncharheight(HWND hwnd);

HWND 		hwndFkeys;
static int 	_fkeyshiftstate;
static int	_fkoptheight;
static int	_fkfkheight;

static char 	*_fkmods[NSHFT] = { "N",   "C",   "A",   "S" };
static KEYCODE _fkshifts[NSHFT] = {0,	K_CONTROL, K_ALTERNATE, K_SHIFT };

/*----------------------------*/
/* setkeylabel()              */
/*----------------------------*/
static void setkeylabel(void )
{
	if (hwndFkeys)
		SetDlgItemText(hwndFkeys,IDD_FKSTATE,_fkmods[_fkeyshiftstate]);
}

/*----------------------------*/
/* fkey_settext()             */
/*----------------------------*/
void fkey_settext(int state)
{
	int     	i;
	int			shift;
	char    	szComment[256],szKey[64],szKtext[128];
	KEYCODE 	keycode1;
	int			k;
	KEYBIND *	kp;
	extern int _fkeysdirty;

	if (state == _fkeyshiftstate ||
	    !hwndFkeys) return;
	if (state == -1)
		state = _fkeyshiftstate;

	_fkeyshiftstate = state;
	shift = _fkshifts[state];
	keycode1 = VK_F1|shift;

	for (i = 0; i < MAX_FKEYS; i++) {
		SetDlgItemText(hwndFkeys,i+IDD_FKFK1,"");
	}

	for (i = 0; i < MAXMAPKEY; i++) {
		kp = &_keymaptab[i];
		k  = (int)kp->keycode - (int)keycode1;
		if (k >= 0 && k < MAX_FKEYS) {
			mac_comment(szComment,szKtext,kp->macref.index,kp->macref.typ);
			wsprintf(szKey,"F%d %s",k+1,szKtext);
			SetDlgItemText(hwndFkeys,k+IDD_FKFK1,szKey);
		}
	}

	setkeylabel();
	_fkeysdirty = 0;
}

/*------------------------------------------------------------
 * fkey_wh()
 */
EXPORT int fkey_wh(WORD *w, WORD *h)
{
	RECT 	rectK;
	RECT	rectClient;

	if (hwndFkeys == 0) {
		*w = *h = 0;
		return 0;
	}
	GetWindowRect(hwndFkeys,&rectK);
	GetClientRect(hwndFkeys,&rectClient);
	*w = (WORD)(rectK.right - rectK.left) + 1;
	*h = (WORD)((rectK.bottom - rectK.top) - rectClient.bottom + 1);
	if (GetConfiguration()->layoutoptions & OL_OPTIONBAR) {
		*h += _fkoptheight;
	}
	if (GetConfiguration()->layoutoptions & OL_FKEYS) {
		*h += _fkfkheight;
	}
	return 1;
}

/*------------------------------------------------------------
 * ResizeSubWindow()
 */
static int ResizeSubWindow(HWND hwnd, int item, int x, int width, BOOL bOptButton)
{
	HWND	hwndItem = GetDlgItem(hwnd,item);
	RECT r;
	int	height;
	int	y;

	if (!hwndItem || !IsWindowVisible(hwndItem)) {
		return 0;
	}
	GetWindowRect(hwndItem,&r);

	height = (bOptButton) ? _fkoptheight : _fkfkheight;
	y = (bOptButton && (GetConfiguration()->layoutoptions & OL_FKEYS)) ? _fkfkheight : 0;

	ScreenToClient(hwnd, (POINT*)&r);
	r.left = x;
	MoveWindow(hwndItem, r.left, y, width, height, TRUE);
	return x + width;
}

/*------------------------------------------------------------
 * FkeysWndProc()
 */
WINFUNC FkeysWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD  			x;
	WORD			item;
	RECT	 		r;
	DRAWITEMSTRUCT *dp;
	KEYCODE 		k;
	BOOL			bShow;
	HWND			hwndItem;
	int				nRight;
	int				nDelta;
	int				fkcharheight;
	int				nButtons;

	switch(message) {

	case WM_CREATE:
		fkcharheight = cust_buttoncharheight(
			GetDlgItem(hwnd, IDD_FKFK1));
		_fkoptheight = fkcharheight + FK_DELTA;
		_fkfkheight = 2 * fkcharheight + FK_DELTA;
		return 0;

	case WM_DRAWITEM:
		dp = (DRAWITEMSTRUCT*)lParam;
		cust_paintbut(dp->hDC, &dp->rcItem, 
				    dp->hwndItem, dp->itemState);
		return 0;

	case WM_SIZE:
	case WM_MOVE:
	case WM_EDWINREORG:
		GetClientRect(hwnd,&r);
		if (r.right < 620+FKSTATE_SIZE) {
			r.right = 620+FKSTATE_SIZE;
			nButtons = 10;
		} else {
			nButtons = 12;
		}
		nRight = r.right;
		nDelta = (r.right - FKSTATE_SIZE) / nButtons;
		if (GetConfiguration()->layoutoptions & OL_FKEYS) {
			for (x = 0, item = IDD_FKFK1; 
				item < IDD_FKFK1+nButtons; item++) {
				x = ResizeSubWindow(hwnd, item, x, nDelta, FALSE);
			}
			ResizeSubWindow(hwnd, IDD_FKSTATE, x, nRight - x, FALSE);
		} 
		for (x = 0, item = IDD_FKFLG1; 
			item < IDD_FKFLG1 + nButtons; item++) {
			x = ResizeSubWindow(hwnd, item, x, 
				(item == IDD_FKFLG1 - 1 + nButtons) ? 
				nRight - x : nDelta, TRUE);
		}
		if (GetConfiguration()->layoutoptions & OL_FKEYS) {
			bShow = TRUE;
		} else {
			bShow = FALSE;
		}
		for (item = IDD_FKFK1; item <= IDD_FKFKLAST; item++) {
			hwndItem = GetDlgItem(hwnd, item);
			ShowWindow(hwndItem, bShow);
		}
		ShowWindow(GetDlgItem(hwnd, IDD_FKSTATE), bShow);

		if (GetConfiguration()->layoutoptions & OL_OPTIONBAR) {
			bShow = TRUE;
		} else {
			bShow = FALSE;
		}
		for (x = 0, item = IDD_FKFLG1; item <= IDD_FKFLGLAST; item++) {
			hwndItem = GetDlgItem(hwnd, item);
			ShowWindow(hwndItem, bShow);
		}
		return TRUE;

	case WM_COMMAND:

		if ((GetConfiguration()->layoutoptions & OL_OPTIONBAR) &&
			wParam >= IDD_FKFLG1 && wParam <= IDD_FKFLGLAST) {
			ww_toppostmessage(WM_PKSOPTOGGLE, wParam, lParam);
			return 0;
		}

		if ((GetConfiguration()->layoutoptions & OL_FKEYS) &&
			wParam >= IDD_FKFK1 && wParam <= IDD_FKFKLAST) {
			k = VK_F1+wParam-IDD_FKFK1;
			k |= _fkshifts[_fkeyshiftstate];
			ww_toppostmessage(WM_SYSKEYDOWN, k , 0);
			return 0;
		}

		if (wParam == IDD_FKSTATE) {
			ww_setfocus();
			return EdFkeysSwitch(1);
		}
		break;

	case WM_DESTROY:
		hwndFkeys = 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

static char *szKeys = "DLGFKEYS";
/*--------------------------------------------------------------------------
 * fkey_show()
 */
static void fkey_show(HWND hwndPapa)
{
	if ((GetConfiguration()->layoutoptions & (OL_FKEYS|OL_OPTIONBAR)) && hwndFkeys == 0) {
		hwndFkeys = CreateDialog(hInst, szKeys, hwndPapa, NULL);
		fkey_settext(-1);
	}
}

/*--------------------------------------------------------------------------
 * fkey_register()
 */
int fkey_register(void)
{	WNDCLASS wc;

	class_defaults(&wc);
	wc.lpfnWndProc = FkeysWndProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.lpszClassName = szKeys;
	return RegisterClass(&wc);
}

/*--------------------------------------------------------------------------
 * fkey_init()
 */
int fkey_init(HWND hwndPapa)
{
	fkey_show(hwndPapa);
	op_updateall();
	return hwndFkeys ? 1 : 0;
}

/*---------------------------------*/
/* EdFkeysSwitch()                 */
/*---------------------------------*/
int EdFkeysSwitch(int delta)
{
	int keys;

    	keys = (_fkeyshiftstate + delta);
	if (keys < 0)
		keys = SHIFTFKEY;
	else
		if (keys > SHIFTFKEY)
			keys = 0;
     fkey_settext(keys);
	return 1;
}

/*--------------------------------------------------------------------------
 * fkey_visibilitychanged()
 */
void fkey_visibilitychanged(void)
{
	if ((GetConfiguration()->layoutoptions & (OL_FKEYS|OL_OPTIONBAR)) == 0 && hwndFkeys) {
		SendMessage(hwndFkeys,WM_CLOSE,0,0L);
	} else {
		fkey_show(hwndFrame);
		if (hwndFkeys) {
			SendMessage(hwndFkeys,WM_EDWINREORG,0,0L);
			op_updateall();
		}
	}
	SendMessage(hwndFrame, WM_EDWINREORG,0,0L);
}

