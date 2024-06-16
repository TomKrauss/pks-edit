/*
 * FunctionKeyWidget.c
 *
 * Project: PKS Edit for Windows
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
#include <CommCtrl.h>
#include "themes.h"

#include "trace.h"
#include "documentmodel.h"
#include "edierror.h"

#include "winterf.h"
#include "winfo.h"
#include "editorconfiguration.h"

#pragma hdrstop

#include "pksedit.h"
#include "dial2.h"
#include "pksmacro.h"
#include "actionbindings.h"
#include "actions.h"
#include "dpisupport.h"

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
static HWND _hwndTips[MAX_FKEYS];

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
 * Register a function key action binding.
 */
static void fk_registerBinding(int nIdx, int nCommand, int nType) {
	ACTION_BINDING binding = { .ab_propertyChanged = fk_propertyChanged, .ab_hwnd = hwndFkeys, .ab_item = nIdx};
	action_registerAction(nCommand, nType, binding, TRUE);
}


static int fkey_setText(KEY_BINDING* kp, void * pParam) {
	char szComment[256], szKey[64], szKtext[128];
	int keycode1 = (int)(intptr_t)pParam;
	int k = (int)kp->keycode - (int)keycode1;
	if (k >= 0 && k < MAX_FKEYS) {
		fk_registerBinding(k, kp->macref.index, kp->macref.typ);
		command_getTooltipAndLabel(kp->macref, szComment, szKtext);
		wsprintf(szKey, "F%d %s", k + 1, szKtext);
	}
	else {
		return 1;
	}
	SetDlgItemText(hwndFkeys, k + IDD_FKFK1, szKey);
	return 1;
}

/*----------------------------
 * fkey_updateTextOfFunctionKeys()
 * Update the text on the FKEYS keyboard.
 *----------------------------*/
void fkey_updateTextOfFunctionKeys(int state) {
	char		szKey[64];
	int     	i;
	int			shift;
	int			keycode1;

	if (state == _fkeyshiftstate || !hwndFkeys) {
		return;
	}
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

	bindings_forAllKeyBindingsDo(NULL, fkey_setText, (void*)(intptr_t)keycode1);
}

// Description:
//   Creates a tooltip for an item in a dialog box. 
// Parameters:
//   idTool - identifier of an dialog box item.
//   nDlg - window handle of the dialog box.
//   iTooltipItem - index of string resource to use as the tooltip text.
// Returns:
//   The handle to the tooltip.
//
static HWND fkey_createTip(HWND hDlg, int toolID) {
	if (!hDlg) {
		return 0;
	}
	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(0L, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hDlg, NULL,
		hInst, NULL);

	if (!hwndTip) {
		return (HWND)NULL;
	}

	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hDlg;
	toolInfo.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
	toolInfo.uId = (INT_PTR)GetDlgItem(hDlg, toolID);
	toolInfo.lpszText = LPSTR_TEXTCALLBACK;
	toolInfo.lParam = (LPARAM)toolID - IDD_FKFK1;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, 350);
	theme_enableDarkMode(hwndTip);
	return hwndTip;
}


/*
 * Create tooltips for the function keys.
 */
static void fkey_createTips(HWND hwndParent) {
	if (_hwndTips[0]) {
		return;
	}
	for (int i = 0; i < MAX_FKEYS; i++) {
		_hwndTips[i] = fkey_createTip(hwndParent, IDD_FKFK1 + i);
	}
}

/*
 * Initialize the preferred sizes and tooltips of the function keyboard.
 */
static BOOL fkey_initialize(HWND hwnd) {
	HWND hwndFk1 = GetDlgItem(hwnd, IDD_FKFK1);
	if (hwndFk1) {
		int fkcharheight = cust_calculateButtonCharacterHeight(hwndFk1);
		_fkoptheight = fkcharheight + FK_DELTA;
		_fkfkheight = 2 * fkcharheight + FK_DELTA;
		fkey_createTips(hwnd);
		return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------
 * fkey_getKeyboardSize()
 * Get the size of the FKEY keyboard of PKS Edit.
 */
EXPORT int fkey_getKeyboardSize(WORD *w, WORD *h) {
	RECT 	rectK;
	RECT	rectClient;

	if (hwndFkeys == 0) {
		*w = *h = 0;
		return 0;
	}
	if (!fkey_initialize(hwndFkeys)) {
		return 0;
	}
	GetWindowRect(hwndFkeys,&rectK);
	GetClientRect(hwndFkeys,&rectClient);
	*w = (WORD)(rectK.right - rectK.left) + 1;
	*h = (WORD)((rectK.bottom - rectK.top) - rectClient.bottom);
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
static void fkey_resizeSubWindow(HWND hwnd, int item, int x, int width, BOOL bOptButton)
{
	HWND	hwndItem = GetDlgItem(hwnd,item);
	int	height;
	int	y;

	if (!hwndItem) {
		return;
	}

	height = (bOptButton) ? _fkoptheight : _fkfkheight;
	y = (bOptButton && (GetConfiguration()->layoutoptions & OL_FKEYS)) ? _fkfkheight : 0;

	MoveWindow(hwndItem, x, y, width, height, TRUE);
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
	static char szComment[200];
	int  			x;
	int				item;
	RECT	 		r;
	DRAWITEMSTRUCT *dp;
	KEYCODE 		k;
	HWND			hwndItem;
	int				nButtons;

	switch(message) {

	case WM_ERASEBKGND:
		return 0;

	case WM_DRAWITEM:
		dp = (DRAWITEMSTRUCT*)lParam;
		cust_paintButton(dp->hDC, &dp->rcItem, 
				    dp->hwndItem, dp->itemState);
		return 0;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
			case TTN_GETDISPINFO: {
				int state = _fkeyshiftstate;
				int shift = _fkshifts[state];
				LPNMTTDISPINFO pInfo = (LPNMTTDISPINFO)lParam;
				WPARAM keycode1 = (VK_F1 + pInfo->lParam) | shift;
				MACROREF* mp = macro_getKeyBinding(keycode1);
				if (mp) {
					char szText[100];
					command_getTooltipAndLabel(*mp, szComment, szText);
					pInfo->lpszText = szComment;
				} else {
					pInfo->lpszText = NULL;
				}
			}
		}
		break;
	case WM_SIZE:
		GetClientRect(hwnd,&r);
		r.right = LOWORD(lParam);
		int nDelta = r.right - r.left;
		nButtons = dpisupport_getSegmentCount(&r, MAX_FKEYS);
		int y;
		BOOL bFKeys = GetConfiguration()->layoutoptions & OL_FKEYS;
		if (bFKeys) {
			for (item = IDD_FKFK1; item < IDD_FKFK1+nButtons; item++) {
				x = (item - IDD_FKFK1) * nDelta / nButtons;
				y = (item - IDD_FKFK1+1) * nDelta / nButtons;
				fkey_resizeSubWindow(hwnd, item, x, y-x, FALSE);
			}
		} 
		BOOL bOptionKeys = GetConfiguration()->layoutoptions & OL_OPTIONBAR;
		if (bOptionKeys) {
			for (item = IDD_FKFLG1; item < IDD_FKFLG1 + nButtons; item++) {
				x = (item - IDD_FKFLG1) * nDelta / nButtons;
				y = (item - IDD_FKFLG1 + 1) * nDelta / nButtons;
				fkey_resizeSubWindow(hwnd, item, x, y - x, TRUE);
			}
		}
		for (item = IDD_FKFK1; item <= IDD_FKFKLAST; item++) {
			hwndItem = GetDlgItem(hwnd, item);
			ShowWindow(hwndItem, bFKeys && item < IDD_FKFK1 + nButtons);
		}
		for (x = 0, item = IDD_FKFLG1; item <= IDD_FKFLGLAST; item++) {
			hwndItem = GetDlgItem(hwnd, item);
			ShowWindow(hwndItem, bOptionKeys && item < IDD_FKFLG1 + nButtons);
		}

		return FALSE;

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
static void fkey_show(HWND hwndParent) {
	if ((GetConfiguration()->layoutoptions & (OL_FKEYS|OL_OPTIONBAR)) && hwndFkeys == 0) {
		HRSRC  hRes = FindResource(ui_getResourceModule(), szKeys, RT_DIALOG);
		if (hRes == NULL) {
			return;
		}
		DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)LoadResource(ui_getResourceModule(), hRes);
		if (pTemplate == NULL) {
			return;
		}
		hwndFkeys = CreateDialogIndirect(0, pTemplate, hwndParent, NULL);
		if (!hwndFkeys) {
			EdTRACE(log_errorArgs(DEBUG_ERR, "Error creating FKEYS %ld. %ld %ld", GetLastError(), hRes, hwndFkeys));
		}
		fkey_updateTextOfFunctionKeys(-1);
		FreeResource(hRes);
	}
}

/*
 * May be invoked, if the current language changes.
 */
void fkey_languageChanged(HWND hwnd) {
	if (hwndFkeys != NULL) {
		DeleteObject(hwndFkeys);
		hwndFkeys = NULL;
	}
	fkey_show(hwnd);
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
	action_commandEnablementChanged((ACTION_CHANGE_TYPE) {0,0,.act_optionsChanged = 1,.act_commandId = -1});
	return hwndFkeys ? 1 : 0;
}

/*---------------------------------
 * fkey_keyModifierStateChanged()
 * Update the PKS edit FKEYs to display the next
 * group of FKEYS (for alternate modifiers).
 *---------------------------------*/
long long fkey_keyModifierStateChanged() {
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

