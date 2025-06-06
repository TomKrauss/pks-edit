/*
 * EditorDialogUtil.c
 * 
 *
 * Project: PKS Edit for Windows
 *
 * purpose: some WINDOWS dialogs for general and special purpose
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
#include <commctrl.h>
#include <windowsx.h>
#include <stdio.h>

#include "trace.h"
#include "documentmodel.h"
#include "fileselector.h"
#include "errordialogs.h"

#include "winterf.h"
#include "winfo.h"
#include "stringutil.h"

#include "pksedit.h"
#include "edierror.h"
#include "resource.h"
#include "dial2.h"
#include "pksrc.h"
#include "pksmacro.h"
#include "actionbindings.h"
#include "history.h"
#include "xdialog.h"
#include "regexp.h"
#include "findandreplace.h"
#include "winutil.h"
#include "themes.h"

#define IDD_APPLYNOW 0x3021
#define ISRADIODLGCTL(i) 	((i) >= IDD_LOWRADIO && (i) <= IDD_HIGHRADIO)
#define ISFLAGDLGCTL(i) 	((i) >= IDD_LOWOPT && (i) <= IDD_HIGHOPT) 

extern int 		_translatekeys;

extern BOOL 	DlgChooseFont(HWND hWnd, char* pszFontName, BOOL bPrinter);

static DIALOG_DESCRIPTOR* _dp;
static DIALOG_DESCRIPTOR* (*_dialogInitParameterCallback)(int pageIndex);
static boolean		bInPropertySheet;
static boolean		bPropertySheetMove;

int	   nCurrentDialog;
HWND   hwndDlg;

/*
 * Return a string resource for the current language. Note, that this method is not
 * re-entrant. Any text loaded must be used right away.
 */
char* dlg_getResourceString(int nId) {
	static char szBuf[1024];

	if (!LoadString(ui_getResourceModule(), nId, szBuf, sizeof szBuf)) {
		return (char*)NULL;
	}
	return szBuf;
}

/*-----------------------------------------------
 * assigns a callback to be invoked to return the DIALOGPARS for a page (in a property sheet)
 * for that particular page, if the page is activated. The callback is passed the index of the
 * property page activated.
 */
void dlg_setXDialogParams(DIALOG_DESCRIPTOR* (*func)(int pageIndex), boolean positionDialogOnInit) {
	_dialogInitParameterCallback = func;
	bInPropertySheet = TRUE;
	bPropertySheetMove = positionDialogOnInit;
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
static HWND CreateToolTip(int toolID, HWND hDlg, int iTooltipItem) {
	if (!toolID || !hDlg || iTooltipItem < 0)
	{
		return FALSE;
	}
	// Get the window of the tool.
	HWND hwndControl = GetDlgItem(hDlg, toolID);

	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(0L, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hDlg, NULL,
		hInst, NULL);

	char szClassname[64];
	GetClassName(hwndControl, szClassname, sizeof szClassname);
	if (strcmp(WC_COMBOBOX, szClassname) == 0) {
		COMBOBOXINFO cbi = {
			.cbSize = sizeof(COMBOBOXINFO)
		};
		if (GetComboBoxInfo(hwndControl, &cbi)) {
			hwndControl = cbi.hwndItem;
		}
	}
	if (!hwndControl || !hwndTip) {
		return (HWND)NULL;
	}

	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hDlg;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hwndControl;
	char 	*pszText;

	if (!(pszText = dlg_getResourceString(iTooltipItem))) {
		return (HWND)NULL;
	}
	toolInfo.lpszText = pszText;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	SendMessage(hwndTip, TTM_SETMAXTIPWIDTH, 0, 350);
	theme_enableDarkMode(hwndTip);
	return hwndTip;
}

static void applyRadioButtons(int idCtrl, DIALOG_ITEM_DESCRIPTOR* dp) {
	int item;
	if (ISRADIODLGCTL(idCtrl)) {
		while ((item = dp->did_controlNumber) != 0) {
			if (ISRADIODLGCTL(item)) {
				if (idCtrl >= item && idCtrl <= item + dp->did_flagOrSize) {
					*(int*)dp->did_data = idCtrl - item;
					break;
				}
			}
			dp++;
		}
	}
}

/*--------------------------------------------------------------------------
 * DoDialog()
 */
int DoDialog(int nIdDialog, DLGPROC DlgProc, DIALOG_DESCRIPTOR *dp) {
	int 		nSave;
	HWND		hwnd;
	INT_PTR		ret;
	DIALOG_DESCRIPTOR* dpOld = _dp;

	bInPropertySheet = FALSE;
	_dp = dp;
	hwnd = GetFocus();
	nSave = nCurrentDialog;
	nCurrentDialog = nIdDialog;

	HRSRC  hRes = FindResource(ui_getResourceModule(), MAKEINTRESOURCE(nIdDialog), RT_DIALOG);
	if (hRes == NULL) {
		log_lastWindowsError("DoDialog");
		_dp = dpOld;
		return -1;
	}
	DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)LoadResource(ui_getResourceModule(), hRes);
	if (pTemplate == NULL) {
		log_lastWindowsError("DoDialog");
		_dp = dpOld;
		return -1;
	}
	ret = DialogBoxIndirect(0, pTemplate, hwndMain, DlgProc);

	if (ret == -1) {
		log_lastWindowsError("DoDialog");
	}
	nCurrentDialog = nSave;
	if (hwnd) {
		SetFocus(hwnd);
	}
	_dp = dpOld;
	return (int)ret;
}

/*--------------------------------------------------------------------------
 * dlg_initString()
 */
void dlg_initString(HWND hDlg, int item, LPSTR szString, int nMax)
{
	SendDlgItemMessage(hDlg, item, EM_LIMITTEXT, nMax, 0L);
	SetDlgItemText(hDlg,item,szString);
	SendDlgItemMessage(hDlg, item, EM_SETMODIFY, FALSE, 0);
}

/*--------------------------------------------------------------------------
 * SubClassWndProc()
 */
WNDPROC SubClassWndProc(int set, HWND hDlg, int item, WNDPROC lpfnNewProc)
{	HWND hwndItem;
	WNDPROC lpfnNowProc;

	hwndItem = GetDlgItem(hDlg,item);
	lpfnNowProc = (WNDPROC)GetWindowLongPtr(hwndItem,GWLP_WNDPROC);
	SetWindowLongPtr(hwndItem,GWLP_WNDPROC, (LONG_PTR) lpfnNewProc);
	return lpfnNowProc;
}

/*------------------------------------------------------------
 * KeyInputWndProc()
 */
static WNDPROC lpfnOldCInput;
static WINFUNC KeyInputWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HWND			h2;
	static KEYCODE key;
	static int down;

	switch(message) {
		case WM_CREATE:
			key = K_DELETED;
			break;

		case WM_SETFOCUS:
			down = 0;
			break;

		case WM_GETDLGCODE:
			return DLGC_WANTMESSAGE;

		case WM_CHARCHANGE:
			key = (KEYCODE)wParam;
			SetWindowText(hwnd,bindings_keycodeToString(key));
			win_sendRedrawToWindow(hwnd);
			return TRUE;

		case CS_QUERYCHAR:
			return key;

		case WM_RBUTTONDOWN:
			SendMessage(hwnd,WM_CHARCHANGE,K_DELETED,0L);
			break;

		case WM_CHAR:
			return FALSE;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (wParam == VK_CONTROL ||
			    wParam == VK_MENU ||
			    wParam == VK_SHIFT)
		    		break;
			if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) {
				key = bindings_addModifierKeysToKeycode((KEYCODE) wParam);
				SendMessage(hwnd,WM_CHARCHANGE,key,0L);
				down++;
			} else if (down) {
				if (down > 1)
					down--;
				else if ((h2 = GetNextDlgTabItem(GetParent(hwnd),hwnd,TRUE)) != 0) {
					SetFocus(h2);
				}
			}
			return TRUE;
	}
	return CallWindowProc(lpfnOldCInput,hwnd,message,wParam,lParam);
}

/*--------------------------------------------------------------------------
 * CharInput()
 */
static WINFUNC dlg_charInputWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{	char szBuff[10];

	switch(message) {
		case WM_GETDLGCODE:
			return DLGC_WANTCHARS;

		case WM_CHAR:
			szBuff[1] = 0;
			szBuff[0] = (char)wParam;
			SetWindowText(hwnd,szBuff);
			win_sendRedrawToWindow(hwnd);
			win_sendParentCommand(hwnd,0L);
			return 0;
	}
	return CallWindowProc(lpfnOldCInput,hwnd,message,wParam,lParam);
}

/*--------------------------------------------------------------------------
 * dlg_setRangeForOperation()
 */
static int dlg_setRangeForOperation(HWND hwnd, int *rangetype, int first)
{
	BOOL bHasSelection;
	int	 item,range;
	char szSel[64];
	HWND hwndList;

	bHasSelection = ww_hasSelection(ww_getCurrentEditorWindow());
	if (bHasSelection) {
		*rangetype = RNG_BLOCK;
	} else if (*rangetype == RNG_BLOCK) {
		*rangetype = RNG_CHAPTER;
	}

	hwndList = GetDlgItem(hwnd, IDD_RNGE);
	SendMessage(hwndList, CB_RESETCONTENT,0,0L);

	szSel[0] = 0;

	for (item = IDS_RNGONCE; item <= IDS_RNGGLOBAL; item++) {
		range = item-IDS_RNGONCE;
		char* pszBuf = dlg_getResourceString(item);
		if (range >= first && (range != RNG_BLOCK || bHasSelection)) {
			SendMessage(hwndList, CB_ADDSTRING, 0, (LPARAM)pszBuf);
		}
		if (range == *rangetype) {
			lstrcpy(szSel, pszBuf);
		}
	}

	SendMessage(hwndList, CB_SELECTSTRING, -1, (LPARAM)szSel);
	return *rangetype;	
}

/*--------------------------------------------------------------------------
 * dlg_getComboBoxText()
 */
static int dlg_getComboBoxText(HWND hwnd, int id, void *szBuff) 
{ 	LRESULT	  item;

	*((char*)szBuff) = 0;
	item = SendDlgItemMessage(hwnd,id,CB_GETCURSEL,0,0);
	if (item == LB_ERR) {
		return LB_ERR;
	}
	SendDlgItemMessage(hwnd,id,CB_GETLBTEXT,item,(LPARAM)szBuff);
	return 1;
}

/*--------------------------------------------------------------------------
 * dlg_getComboBoxSelectedText()
 */
int dlg_getComboBoxSelectedText(HWND hwnd, int id, char **pszBuf) { 	
	LRESULT	  item;

	if (!pszBuf) {
		return 1;
	}
	char* pszString = *pszBuf;
	if (!pszString) {
		return 1;
	}
	item = SendDlgItemMessage(hwnd,id, CB_GETCURSEL, 0, 0);
	if (item == LB_ERR) {
		*((char *)pszString) = 0;
		return LB_ERR;
	}
	SendDlgItemMessage(hwnd,id, CB_GETLBTEXT, item, (LPARAM)pszString);
	return 1;
}

/*--------------------------------------------------------------------------
 * dlg_getListBoxSelection()
 */
int dlg_getListBoxSelection(HWND hwnd, int id, void** pData) {
	LRESULT	  item;

	item = SendDlgItemMessage(hwnd, id, LB_GETCURSEL, 0, 0);
	if (item == LB_ERR) {
		*pData = 0;
		return 0;
	}
	*pData = (void*) SendDlgItemMessage(hwnd, id, LB_GETITEMDATA, item, (LPARAM)0);
	return 1;
}

/*--------------------------------------------------------------------------
 * dlg_getRangeForOperation()
 */
static int dlg_getRangeForOperation(HWND hwnd)
{ 	WORD	 item;
	char*	 pszBuf;
	char	 szCurr[130];

	szCurr[0] = 0;
	if (dlg_getComboBoxText(hwnd, IDD_RNGE, szCurr) == LB_ERR) {
		return RNG_FREE;
	}
	for (item = IDS_RNGONCE; item <= IDS_RNGGLOBAL; item++) {
		pszBuf = dlg_getResourceString(item);
		if (pszBuf && strcmp(pszBuf,szCurr) == 0)
			return item-IDS_RNGONCE+RNG_ONCE;
	}
	return RNG_FREE;
}

/*--------------------------------------------------------------------------
 * dlg_initFromParameters()
 */
BOOL dlg_initFromParameters(HWND hDlg, DIALOG_ITEM_DESCRIPTOR *dp, int nParams)
{
	char 	cbuf[2],numbuf[32];
	int 	item,*ip;
	BOOL	moved;
	HISTORY_TYPE ht;

	moved = FALSE;
	while(--nParams >= 0 && (item = dp->did_controlNumber) != 0) {
		ip = (int*)dp->did_data;
		if (dp->did_initialize) {
			dp->did_initialize(hDlg, dp);
			dp++;
			continue;
		}
		LIST_HANDLER* dpi = dp->did_listhandler;
		if (dpi) {
			dpi->li_fill(hDlg, item, (void*)dpi->li_param);
			dp++;
			continue;
		}
		switch(item) {
			case IDD_POSITIONTCURS:
				moved = win_positionWindowRelativeToCaret(hDlg);
				break;
			case IDD_FILE_PATTERN:
				ht = FILE_PATTERNS;
			case IDD_FINDS2: case IDD_FINDS: case IDD_REPLS:
				if (item == IDD_REPLS) {
					ht = SEARCH_AND_REPLACE;
				} else if (item != IDD_FILE_PATTERN) {
					ht = SEARCH_PATTERNS;
				}
				hist_fillComboBox(hDlg, item, ht, TRUE);
				LPSTR pData = (LPSTR)ip;
				if (pData && *pData) {
					SetDlgItemText(hDlg, item, pData);
				}
				break;
			case IDD_PATH1:
				hist_fillComboBox(hDlg, item, FOLDERS, 1);
				if (!((LPSTR)ip)[0]) {
					break;
				}
				// drop through
			case IDD_STRING1: case IDD_STRING2: case IDD_STRING3:
			case IDD_STRING4: case IDD_STRING5: case IDD_STRING6:
			case IDD_STRING7: 
				dlg_initString(hDlg,item,(LPSTR)ip,dp->did_flagOrSize);
				break;
			case IDD_LONG1:
				sprintf(numbuf,"%ld",*(long*)dp->did_data);
				goto donum;
			case IDD_INT1: case IDD_INT2: case IDD_INT3: 
			case IDD_INT4: case IDD_INT5: case IDD_INT6:
			case IDD_INT7: case IDD_INT8:
				if (*ip < 0) {
					numbuf[0] = 0;
				} else {
					sprintf(numbuf, "%d", *ip);
				}
donum:			dlg_initString(hDlg,item,numbuf,sizeof numbuf-1);
				break;
			case IDD_RNGE:
				dlg_setRangeForOperation(hDlg,ip,dp->did_flagOrSize);
				break;
			case IDD_KEYCODE:
				lpfnOldCInput = SubClassWndProc(1,hDlg,item,
								      KeyInputWndProc);
#if 0
				SendDlgItemMessage(hDlg,item,WM_CHARCHANGE,
								*(KEYCODE*)dp->dp_data,0L);
# endif
				break;
			case IDD_RAWCHAR:
				lpfnOldCInput = SubClassWndProc(1,hDlg,item,dlg_charInputWindowProc);
			case IDD_CHAR:
				cbuf[0] = *(char*)dp->did_data;
				cbuf[1] = 0;
				dlg_initString(hDlg,item,cbuf,sizeof cbuf-1);
				break;
			default:
				if (!ip) {
					break;
				}
				if (ISFLAGDLGCTL(item)) {
					CheckDlgButton(hDlg,item,(*ip & dp->did_flagOrSize) ? 1 : 0);
				} else if (ISRADIODLGCTL(item)) {
					CheckRadioButton(hDlg,item,item+dp->did_flagOrSize,*ip+item);
				}
		}
		dp++;
	}
	return moved;
}

/*--------------------------------------------------------------------------
 * dlg_initialize()
 */
static void dlg_initialize(HWND hDlg, DIALOG_DESCRIPTOR *dpDialog, BOOL initialFlag) {
	static const int _intFields[] = { IDD_INT1, IDD_INT2, IDD_INT3, IDD_INT4, IDD_INT5 };
	static const int _spinFields[] = { IDC_SPIN1, IDC_SPIN2, IDC_SPIN3, IDC_SPIN4, IDC_SPIN5 };
	DIALOG_ITEM_DESCRIPTOR *dp2;
	int		nPars;

	DIALOG_ITEM_DESCRIPTOR* dp = dpDialog->dd_items;
	if (dp == NULL) {
		return;
	}
	for (dp2 = dp, nPars = 0; dp2->did_controlNumber; dp2++) {
		nPars++;
	}
	for (int i = 0; i < DIM(_intFields); i++) {
		int iSpin = _spinFields[i];
		int iBuddy = _intFields[i];
		HWND hwndUpDown = GetDlgItem(hDlg, iSpin);
		HWND hwndBuddy = GetDlgItem(hDlg, iBuddy);
		if (hwndBuddy && hwndUpDown) {
			SendMessage(hwndUpDown, UDM_SETBUDDY, (WPARAM)hwndBuddy, 0);
			SendMessage(hwndUpDown, UDM_SETRANGE, (WPARAM)0, (LPARAM)10);
		}
	}
	HICON hIcon = NULL;
	HWND hwndControl = GetDlgItem(hDlg, IDD_APP_ICON);
	if (hwndControl) {
		hIcon = LoadIcon(hInst, "APP_ICON");
		SendMessage(hwndControl, STM_SETICON, (WPARAM) hIcon, 0);
	}
	if (!dlg_initFromParameters(hDlg, dp, nPars) && initialFlag) {
		if (!bInPropertySheet) {
			win_moveWindowToDefaultPosition(hDlg);
		}
	}
	if (hIcon) {
		DestroyIcon(hIcon);
	}
}

/*--------------------------------------------------------------------------
 * dlg_handleRadioButtonGroup()
 * Custom handling of radio buttons: when an item is checked, make sure, that other items
 * passed in the exclusiveGroup array must be unchecked. Note, that the exclusiveGroup array
 * must be terminated by a 0 value.
 */
void dlg_handleRadioButtonGroup(HWND hDlg, WORD checkedItemControl, ...) {
	va_list exclusiveGroup;
	WORD exclusiveItem;
	va_start(exclusiveGroup, checkedItemControl);
	while ((exclusiveItem = va_arg(exclusiveGroup, WORD)) != 0) {
		if (exclusiveItem != checkedItemControl) {
			CheckDlgButton(hDlg, exclusiveItem, 0);
		}
	}
	va_end(exclusiveGroup);
}

/*--------------------------------------------------------------------------
 * dlg_itemDescriptorForControl()
 */
static DIALOG_ITEM_DESCRIPTOR *dlg_itemDescriptorForControl(DIALOG_DESCRIPTOR *dpDialog, int nItem)
{
	if (dpDialog == NULL) {
		return NULL;
	}
	DIALOG_ITEM_DESCRIPTOR* dp = dpDialog->dd_items;
	while(dp && dp->did_controlNumber != 0) {
		if (dp->did_controlNumber == nItem) {
			return dp;
		}
		dp++;
	}
	return (DIALOG_ITEM_DESCRIPTOR*)NULL;
}

/*--------------------------------------------------------------------------
 * Applies the changes in a dialog. idCtrl is the ID of the item leading to the confirmation of the dialog.
 * The list of item descriptors must be 0-terminated.
 */
BOOL dlg_applyChanges(HWND hDlg, int idCtrl, DIALOG_DESCRIPTOR *dpDialog) {
	char 	cbuf[2],numbuf[32];
	int  	item,*ip;
	BOOL	buttonChecked;

	DIALOG_ITEM_DESCRIPTOR* dp = dpDialog->dd_items;
	if (!dp) {
		return idCtrl == IDCANCEL;
	}
	while((item = dp->did_controlNumber) != 0) {
		if (dp->did_apply) {
			if (!dp->did_apply(hDlg, dp)) {
				return FALSE;
			}
			dp++;
			continue;
		}
		LIST_HANDLER* dpi = dp->did_listhandler;
		if (dpi && dpi->li_get) {
			dpi->li_get(hDlg, item, &dpi->li_param);
			dp++;
			continue;
		}
		ip = (int*)dp->did_data;
		switch(item) {
		case IDD_OPT1:
		case IDD_OPT2:
		case IDD_OPT3:
		case IDD_OPT4:
		case IDD_OPT5:
		case IDD_OPT6:
		case IDD_OPT7:
		case IDD_OPT8:
		case IDD_OPT9:
		case IDD_OPT10:
		case IDD_OPT11:
		case IDD_OPT12:
		case IDD_OPT13:
		case IDD_OPT14:
		case IDD_OPT15:
		case IDD_OPT16:
		case IDD_OPT17:
		case IDD_OPT18:
			ip = (int*)dp->did_data;
			if (!ip) {
				break;
			}
			buttonChecked = IsDlgButtonChecked(hDlg, dp->did_controlNumber);
			*ip = buttonChecked ?
				*ip | dp->did_flagOrSize :
				*ip & ~dp->did_flagOrSize;
			break;
		case IDD_FINDS:   case IDD_FINDS2: case IDD_REPLS:
		case IDD_PATH1:
		case IDD_STRING1: 
		case IDD_STRING2: 
		case IDD_STRING3:
		case IDD_STRING4: 
		case IDD_STRING5:
		case IDD_STRING6:
		case IDD_STRING8:
		case IDD_FILE_PATTERN:
			GetDlgItemText(hDlg, item, (LPSTR)ip, dp->did_flagOrSize);
			if ((item == IDD_FINDS2 && *(char*)ip) ||
				(item == IDD_FINDS)) {
				if (!regex_compileWithError((LPSTR)ip)) {
					return FALSE;
				}
				hist_getSessionData()->sd_searchAndReplaceOptions = _currentSearchAndReplaceParams.options;
				hist_saveString(SEARCH_PATTERNS, (LPSTR)ip);
			}
			if (item == IDD_PATH1) {
				hist_saveString(FOLDERS, (LPSTR)ip);
			}
			if (idCtrl == IDOK && item == IDD_REPLS) {
				if (!find_initializeReplaceByExpression(dp->did_data)) {
					return FALSE;
				}
				hist_saveString(SEARCH_AND_REPLACE, (LPSTR)ip);
			}
			break;
		case IDD_INT1: case IDD_INT2: case IDD_INT3: 
		case IDD_INT4: case IDD_INT5: case IDD_INT6:
		case IDD_INT7:  case IDD_INT8:
		case IDD_LONG1: 
			GetDlgItemText(hDlg,item,numbuf,sizeof numbuf-1);
			if (item == IDD_LONG1) {
				*((long*)dp->did_data) = (long)string_convertToLong(numbuf);
			} else {
				*ip = numbuf[0] == 0 ? -1 : (long)string_convertToLong(numbuf);
			}
			break;
		case IDD_RNGE:
			*ip = dlg_getRangeForOperation(hDlg);
			break;
		case IDD_KEYCODE:
			*(KEYCODE*)dp->did_data = 
				(KEYCODE) SendDlgItemMessage(hDlg,item,
							    CS_QUERYCHAR,0,0L);
			break;
		case IDD_RAWCHAR:
			/* drop through */
		case IDD_CHAR:
			GetDlgItemText(hDlg,item,cbuf,sizeof cbuf);
			*(char*)dp->did_data = cbuf[0];
			break;
		default:
			if (ip == NULL) {
				break;
			}
			if (ISFLAGDLGCTL(item)) {
				*ip = (IsDlgButtonChecked(hDlg, item)) ?
					*ip | dp->did_flagOrSize : 
					*ip & ~dp->did_flagOrSize;
			} else {
				applyRadioButtons(idCtrl, dp);
			}
			break;
		} /*end switch */
		dp++;
	}
	return TRUE;
}

/*
 * Utility to retrieve a title text from a dialog component. 
 */
char* dlg_getTitleResource(HWND hDlg, int idCtrl, char* szButton, size_t nSize) {
	GetWindowText(GetDlgItem(hDlg, idCtrl), szButton, (int)nSize);
	return (szButton[0] == '&') ? szButton + 1 : szButton;
}

/*
 * Used to select a font in a dialog.
 */
BOOL dlg_selectFontCommand(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog) {
	if (DlgChooseFont(hDlg, (char*)pDescriptor->did_data,
		(BOOL)pDescriptor->did_flagOrSize) && bInPropertySheet) {
		PropSheet_Changed(GetParent(hDlg), hDlg);
	}
	return FALSE;
}


/*--------------------------------------------------------------------------
 * dlg_command()
 */
static BOOL dlg_command(HWND hDlg, WPARAM wParam, LPARAM lParam, DIALOG_DESCRIPTOR *pDialog)
{
	int		idCtrl;
	int  	nNotify;
	DIALOG_CB callback;
	DIALOG_ITEM_DESCRIPTOR *pDescriptor;

	callback = 0;
	idCtrl = GET_WM_COMMAND_ID(wParam, lParam);
	nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
	pDescriptor = dlg_itemDescriptorForControl(pDialog, idCtrl);
	if (pDescriptor != NULL) {
		callback = (DIALOG_CB)pDescriptor->did_data;
		LIST_HANDLER* pListHandler = pDescriptor->did_listhandler;
		if (pListHandler != NULL && pListHandler->li_command) {
			pListHandler->li_command(hDlg, idCtrl, nNotify, pListHandler->li_param);
			return FALSE;
		}
	} else {
		callback = NULL;
	}
	if (pDescriptor && pDescriptor->did_command && pDialog) {
		if (pDescriptor->did_command(hDlg, nNotify, lParam, pDescriptor, pDialog)) {
			EndDialog(hDlg, idCtrl);
			return TRUE;
		}
		return FALSE;
	}
	switch(idCtrl) {
		case IDD_REGEXP: case IDD_SHELLJOKER:
			dlg_handleRadioButtonGroup(hDlg,idCtrl,IDD_REGEXP,IDD_SHELLJOKER, 0);
			break;
		case IDD_RAWCHAR: case IDOK: case IDD_BUT3: case IDD_BUT4: case IDD_BUT5:
			if (pDialog && !dlg_applyChanges(hDlg, idCtrl, pDialog)) {
				return FALSE;
			}
			// drop through
		case IDCANCEL:
			if (callback && idCtrl != IDD_RAWCHAR) {
				(*callback)(hDlg,idCtrl);
				return TRUE;
			}
			EndDialog(hDlg,idCtrl);
			return TRUE;
		default:
			if (pDialog) {
				applyRadioButtons(idCtrl, pDialog->dd_items);
			}
			break;
	}
	return FALSE;
}
	
/*--------------------------------------------------------------------------
 * dlg_help()
 * Shows the help system.
 */
void dlg_help(void)
{
	if (nCurrentDialog != 0) {
		EdHelpContext(nCurrentDialog);
		return;
	}
	error_showErrorById(IDS_MSGNOHELP);
}

/*--------------------------------------------------------------------------
 * dlg_handleNotifyMessages()
 */
static BOOL CALLBACK dlg_handleNotifyMessages(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	LPNMHDR		pNmhdr;
	INT			idCtrl;
	HWND		parent;
	int idx;
	static int _currentIndex = -1;

	pNmhdr = (LPNMHDR) lParam;
	idCtrl = (INT)wParam;
	parent = GetParent(hDlg);
	idx = PropSheet_HwndToIndex(parent, hDlg);
	switch(pNmhdr->code) {
	case PSN_SETACTIVE:
		_dp = _dialogInitParameterCallback(idx);
		_currentIndex = idx;
		if (_dp != NULL) {
			dlg_initialize(hDlg, _dp, FALSE);
		}
		PropSheet_UnChanged(parent, hDlg);
		return TRUE;
	case PSN_APPLY:
		if (idx == _currentIndex) {
			if (_dp != NULL) {
				dlg_applyChanges(hDlg, IDOK, _dp);
			}
			PropSheet_UnChanged(parent, hDlg);
		}
		return TRUE;
	case PSN_KILLACTIVE:
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, FALSE);
		break;
	}
	return TRUE;
}

/*
 * Default message handling in all PKS Edit dialogs to e.g. support theming. 
 */
BOOL dlg_disableDarkHandling;
INT_PTR CALLBACK dlg_defaultWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	HWND hwndExplanation;

	if (dlg_disableDarkHandling) {
		return FALSE;
	}
	switch (message) {
	case WM_CTLCOLOR:
	case WM_CTLCOLORBTN: // does not work - need owner draw buttons for that
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORLISTBOX: {
		THEME_DATA* pTheme = theme_getCurrent();
		if (pTheme->th_isWinTheme) {
			return FALSE;
		}
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, pTheme->th_dialogForeground);
		SetBkColor(hdc, pTheme->th_dialogBackground);
		return (INT_PTR)theme_getDialogBackgroundBrush();
	}
	case WM_INITDIALOG:
		theme_enableDarkMode(hDlg);
		theme_prepareChildrenForDarkmode(hDlg);
		hwndExplanation = GetDlgItem(hDlg, IDD_EXPLANATION);
		if (hwndExplanation) {
			SendMessage(hwndExplanation, WM_SETFONT, (WPARAM)theme_getSmallDialogFont(), TRUE);
		}
		return TRUE;
	}
	return FALSE;
}

static DIALOG_HELP_DESCRIPTOR* dlg_findContextHelp(DIALOG_HELP_DESCRIPTOR* pItems, int nItem) {
	if (pItems == NULL) {
		return NULL;
	}
	DIALOG_HELP_DESCRIPTOR* pMain = NULL;
	while (pItems->dhd_link) {
		if (pItems->dhd_itemNumber == nItem) {
			return pItems;
		}
		else if (pItems->dhd_itemNumber == 0) {
			pMain = pItems;
		}
		pItems++;
	}
	return pMain;
}

/*--------------------------------------------------------------------------
 * dlg_standardDialogProcedure()
 */
INT_PTR CALLBACK dlg_standardDialogProcedure(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	MEASUREITEMSTRUCT	*mp;
	COMPAREITEMSTRUCT	*cp;
	int					nNotify;
	int					idCtrl;
	DRAWITEMSTRUCT		*drp;
	DIALOG_ITEM_DESCRIPTOR* pDescriptor;

	idCtrl = GET_WM_COMMAND_ID(wParam, lParam);
	switch(message) {
		case WM_HELP: {
			HELPINFO* pInfo = (HELPINFO*)lParam;
			if (_dp) {
				DIALOG_HELP_DESCRIPTOR* pDhd = dlg_findContextHelp(_dp->dd_helpItems, pInfo == NULL ? 0 : pInfo->iCtrlId);
				if (pDhd != NULL) {
					help_open(pDhd->dhd_link);
				}
			}
			return TRUE;
		}
		case WM_NOTIFY:
			return dlg_handleNotifyMessages(hDlg, wParam, lParam);

		case WM_INITDIALOG:
			hwndDlg = hDlg;
			if (bPropertySheetMove) {
				bPropertySheetMove = FALSE;
				win_moveWindowToDefaultPosition(GetParent(hDlg));
			} else if (_dp != NULL) {
				dlg_initialize(hDlg, _dp, TRUE);
			}
			if (_dp && _dp->dd_tooltips != NULL) {
				for (int i = 0; _dp->dd_tooltips[i].m_itemId > 0; i++) {
					CreateToolTip(_dp->dd_tooltips[i].m_itemId, hDlg, _dp->dd_tooltips[i].m_tooltipStringId);
				}
			}
			break;

		case WM_DESTROY:
			bPropertySheetMove = FALSE;
			bInPropertySheet = FALSE;
			return FALSE;

		case WM_MEASUREITEM:
			mp = (MEASUREITEMSTRUCT*)lParam;
			pDescriptor = dlg_itemDescriptorForControl(_dp, idCtrl);
			if (!pDescriptor || !pDescriptor->did_listhandler || !pDescriptor->did_listhandler->li_measure) {
				break;
			}
			(pDescriptor->did_listhandler->li_measure)(mp);
			return TRUE;

		case WM_COMPAREITEM:
			pDescriptor = dlg_itemDescriptorForControl(_dp, idCtrl);
			if (!pDescriptor || !pDescriptor->did_listhandler || !pDescriptor->did_listhandler->li_compare) {
				break;
			}
			cp = (COMPAREITEMSTRUCT*)lParam;
			return pDescriptor->did_listhandler->li_compare(cp);

		case WM_DRAWITEM:
			pDescriptor = dlg_itemDescriptorForControl(_dp, idCtrl);
			if (!pDescriptor || !pDescriptor->did_listhandler) {
				break;
			}
			LIST_HANDLER* pListHandler = pDescriptor->did_listhandler;
			drp = (DRAWITEMSTRUCT*)lParam;
			return cust_drawComboBoxOwnerDraw(drp, pListHandler->li_draw, pListHandler->li_selection, dlg_disableDarkHandling);

		case WM_COMMAND:
			nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
			if ((nNotify == EN_CHANGE || nNotify == CBN_SELCHANGE || ISFLAGDLGCTL(idCtrl) || ISRADIODLGCTL(idCtrl)) &&
					bInPropertySheet && (LPARAM)GetParent(hDlg) != lParam) {
				PropSheet_Changed(GetParent(hDlg), hDlg);
			}
			if (dlg_command(hDlg,wParam,lParam,_dp)) {
				hwndDlg = 0;
				return TRUE;
			}
			break;
	}
	return dlg_defaultWndProc(hDlg, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * win_callDialogCB()
 * Standard dialog handling in PKS edit allowing to pass a custom dialog procedure.
 * The passed dialog procedure should invoke dlg_standardDialogProcedure for all non
 * custom dialog processing.
 */
int win_callDialogCB(int nId, RECORDED_FORM_DESCRIPTOR *pp, DIALOG_DESCRIPTOR *dp, DLGPROC pCallback)
{ 	int ret = 1;

	if (interpreter_openDialog(pp)) {
		ret = DoDialog(nId, pCallback,dp);
		recorder_recordOperation(pp);
		if (ret == IDCANCEL || ret == -1)
			return 0;
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * win_callDialog()
 */
int win_callDialog(int nId, RECORDED_FORM_DESCRIPTOR* pp, DIALOG_DESCRIPTOR* dp) {
	return win_callDialogCB(nId, pp, dp, dlg_standardDialogProcedure);
}

/*--------------------------------------------------------------------------
 * win_createModelessDialog()
 * Create a modeless dialog, given a callback and a dialog proc.
 */
void win_createModelessDialog(HWND *hwnd,LPSTR szName, INT_PTR (CALLBACK *func)(HWND, UINT, WPARAM, LPARAM),
				      DLGPROC *lplpfnDlgProc) {
	if (*hwnd) {
		return;
	}

	if (!*lplpfnDlgProc) {
		*lplpfnDlgProc = MakeProcInstance(func, hInst);
	}
	*hwnd = CreateDialog(ui_getResourceModule(),szName,hwndMain,*lplpfnDlgProc);
}

/*--------------------------------------------------------------------------
 * win_destroyModelessDialog()
 * Destroy the current modeless dialog. Pass a "pointer!" to a win handle.
 */
void win_destroyModelessDialog(HWND *hwnd)
{
	if (*hwnd) {
		DestroyWindow(*hwnd);
	}
}

static int propSheetSubclassId = 2100;
static LRESULT CALLBACK dlg_propertySheetSubclassProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData) {
	switch (uMsg) {
	case WM_NCDESTROY:
		RemoveWindowSubclass(hwnd, dlg_propertySheetSubclassProc, propSheetSubclassId);
		break;
	case WM_ERASEBKGND: {
		HDC hdc = (HDC)wParam;
		RECT rc;
		GetClientRect(hwnd, &rc);
		FillRect(hdc, &rc, theme_getDialogBackgroundBrush());
	}
	return 1;
	}
	return DefDlgProc(hwnd, uMsg, wParam, lParam);
}

/*
 * Property Sheet callback method for darkmode handling.
 */
intptr_t dlg_propertySheetCallback(HWND hwnd, UINT nMessage, WPARAM wParam, LPARAM lParam) {
	if (nMessage == PSCB_INITIALIZED) {
		SetWindowSubclass(hwnd, dlg_propertySheetSubclassProc, propSheetSubclassId, 0);
		nMessage = WM_INITDIALOG;
		SetDlgItemText(hwnd, IDCANCEL, dlg_getResourceString(IDS_CANCEL));
		SetDlgItemText(hwnd, IDD_APPLYNOW, dlg_getResourceString(IDS_APPLY));
	}
	return dlg_defaultWndProc(hwnd, nMessage, wParam, lParam);
}



