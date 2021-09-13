/*
 * FunctionKeyWidget.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: Function Key Widget implementation
 *
 * 										created: 01.01.90
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "customcontrols.h"
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
#include "actions.h"

# undef IDD_FKFKLAST
# undef IDD_FKFLGLAST
#define 	IDD_FKFKLAST		IDD_FKFK1+11
#define 	IDD_FKFLGLAST		IDD_FKFLG1+11

#define 	MAX_FKEYS			(IDD_FKFKLAST-IDD_FKFK1+1)
#define 	NSHFT			4
#define 	FKSLEN			21
#define	FK_DELTA			6

extern WNDPROC SubClassWndProc(int set, HWND hDlg, int item, WNDPROC lpfnNewProc);

HWND  hwndFkeys;
static int 	_fkeyshiftstate;
static int	_fkoptheight;
static int	_fkfkheight;

static char 	*_fkmods[NSHFT] = { "N",   "C",   "A",   "S" };
static KEYCODE _fkshifts[NSHFT] = {0,	K_CONTROL, K_ALTERNATE, K_SHIFT };

/*
 * Callback to enable / disable toolbar buttons.
 */
static void fk_propertyChanged(ACTION_BINDING* pActionBinding, PROPERTY_CHANGE_TYPE type, int newVal) {
	if (type == PC_ENABLED) {
		EnableWindow(GetDlgItem(hwndFkeys, pActionBinding->ab_item + IDD_FKFK1), newVal);
	}
}

/*
 * Register a toolbar action binding.
 */
static void fk_registerBinding(int nIdx, int nCommand) {
	ACTION_BINDING binding = { fk_propertyChanged, hwndFkeys, nIdx };
	action_registerAction(nCommand, binding, TRUE);
}


/*----------------------------
 * fkey_updateTextOfFunctionKeys()
 * Update the text on the FKEYS keyboard.
 *----------------------------*/
void fkey_updateTextOfFunctionKeys(int state)
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

	action_deregisterAllActionsWithListener(fk_propertyChanged);
	for (i = 0; i < MAX_FKEYS; i++) {
		wsprintf(szKey, "F%d", i + 1);
		SetDlgItemText(hwndFkeys, i + IDD_FKFK1, szKey);
		EnableWindow(GetDlgItem(hwndFkeys, i + IDD_FKFK1), FALSE);
	}

	for (i = 0; i < MAXMAPKEY; i++) {
		kp = &_keymaptab[i];
		k  = (int)kp->keycode - (int)keycode1;
		if (k >= 0 && k < MAX_FKEYS) {
			fk_registerBinding(k, kp->macref.index);
			macro_getComment(szComment,szKtext,kp->macref.index,kp->macref.typ);
			wsprintf(szKey,"F%d %s",k+1,szKtext);
		} else {
			continue;
		}
		SetDlgItemText(hwndFkeys, k + IDD_FKFK1, szKey);
	}

	_fkeysdirty = 0;
}

/*------------------------------------------------------------
 * fkey_getKeyboardSize()
 * Get the size of the FKEY keyboard of PKS Edit.
 */
EXPORT int fkey_getKeyboardSize(WORD *w, WORD *h)
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
 * fkey_resizeSubWindow()
 */
static int fkey_resizeSubWindow(HWND hwnd, int item, int x, int width, BOOL bOptButton)
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
	MoveWindow(hwndItem, r.left, y+1, width, height, TRUE);
	return x + width;
}

/*------------------------------------------------------------
 * ww_toppostmessage()
 */
static int ww_toppostmessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	ww_requestFocusInTopWindow();
	return PostMessage(hwndMain, message, wParam, lParam);
}

/*------------------------------------------------------------
 * FkeysWndProc()
 */
static WINFUNC FkeysWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int  			x;
	int				item;
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
		fkcharheight = cust_calculateButtonCharacterHeight(
			GetDlgItem(hwnd, IDD_FKFK1));
		_fkoptheight = fkcharheight + FK_DELTA;
		_fkfkheight = 2 * fkcharheight + FK_DELTA;
		return 0;

	case WM_DRAWITEM:
		dp = (DRAWITEMSTRUCT*)lParam;
		cust_paintButton(dp->hDC, &dp->rcItem, 
				    dp->hwndItem, dp->itemState);
		return 0;

	case WM_SIZE:
	case WM_MOVE:
	case WM_EDWINREORG:
		GetClientRect(hwnd,&r);
		if (r.right < 620) {
			r.right = 620;
			nButtons = 10;
		} else {
			nButtons = 12;
		}
		nRight = r.right;
		nDelta = r.right - r.left - 1;
		if (GetConfiguration()->layoutoptions & OL_FKEYS) {
			for (x = r.left+1, item = IDD_FKFK1; 
				item < IDD_FKFK1+nButtons; item++) {
				x = fkey_resizeSubWindow(hwnd, item, x, (item - IDD_FKFK1+1)*nDelta/nButtons-x, FALSE);
			}
		} 
		for (x = r.left+1, item = IDD_FKFLG1; 
			item < IDD_FKFLG1 + nButtons; item++) {
			x = fkey_resizeSubWindow(hwnd, item, x, 
				(item - IDD_FKFLG1+1) * nDelta / nButtons - x, TRUE);
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
		if (GetConfiguration()->layoutoptions & OL_OPTIONBAR) {
			bShow = TRUE;
		}
		else {
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
			k = (KEYCODE)(VK_F1+wParam-IDD_FKFK1);
			k |= _fkshifts[_fkeyshiftstate];
			ww_toppostmessage(WM_SYSKEYDOWN, k , 0);
			return 0;
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
static void fkey_show(HWND hwndPapa) {
	if ((GetConfiguration()->layoutoptions & (OL_FKEYS|OL_OPTIONBAR)) && hwndFkeys == 0) {
		HRSRC  hRes = FindResource(ui_getResourceModule(), szKeys, RT_DIALOG);
		DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)LoadResource(ui_getResourceModule(), hRes);
		hwndFkeys = CreateDialogIndirect(0, pTemplate, hwndPapa, NULL);
		if (!hwndFkeys) {
			EdTRACE(log_errorArgs(DEBUG_ERR, "Error creating FKEYS %ld. %ld %ld", GetLastError(), hRes, hwndFkeys));
		}
		fkey_updateTextOfFunctionKeys(-1);
	}
}

/*--------------------------------------------------------------------------
 * fkey_register()
 * Register the window class for the PKS Edit function keys widgets.
 */
int fkey_register(void)
{	WNDCLASS wc;

	cust_initializeWindowClassDefaults(&wc);
	wc.lpfnWndProc = FkeysWndProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.lpszClassName = szKeys;
	return RegisterClass(&wc);
}

/*--------------------------------------------------------------------------
 * fkey_initKeyboardWidget()
 * Initialize the FKEYs keyboard widget.
 */
int fkey_initKeyboardWidget(HWND hwndPapa)
{
	fkey_show(hwndPapa);
	action_commandEnablementChanged((ACTION_CHANGE_TYPE) {0,0,1,-1});
	return hwndFkeys ? 1 : 0;
}

/*---------------------------------
 * fkey_keyModifierStateChanged()
 * Update the PKS edit FKEYs to display the next
 * group of FKEYS (for alternate modifiers).
 *---------------------------------*/
int fkey_keyModifierStateChanged() {
	int keys;

	if (!hwndFkeys) {
		return 0;
	}
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		keys = SHIFTFKEY;
	} else if (GetKeyState(VK_CONTROL) & 0x8000) {
		keys = CNTLFKEY;
	} else if (GetKeyState(VK_MENU) & 0x8000) {
		keys = ALTFKEY;
	} else {
		keys = NORMFKEY;
	}
     fkey_updateTextOfFunctionKeys(keys);
	return 1;
}

/*--------------------------------------------------------------------------
 * fkey_visibilitychanged()
 */
void fkey_visibilitychanged(void) {
	if ((GetConfiguration()->layoutoptions & (OL_FKEYS|OL_OPTIONBAR)) == 0 && hwndFkeys) {
		SendMessage(hwndFkeys,WM_CLOSE,0,0L);
		hwndFkeys = NULL;
	} else {
		fkey_show(hwndMain);
	}
	SendMessage(hwndMain, WM_SIZE,0,0L);
	if (hwndFkeys) {
		SendMessage(hwndFkeys, WM_EDWINREORG, 0, 0L);
		action_commandEnablementChanged((ACTION_CHANGE_TYPE) { 0, 0, 1, -1 });
	}
}

