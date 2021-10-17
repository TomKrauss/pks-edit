/*
 * EditorDialogUtil.c
 * 
 *
 * PROJEKT: PKS-EDIT for Windows
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

#include "trace.h"
#include "lineoperations.h"
#include "fileselector.h"
#include "errordialogs.h"

#include "winterf.h"
#include "winfo.h"
#include "stringutil.h"

#include "pksedit.h"
#include "edierror.h"
#include "dial2.h"
#include "pksrc.h"
#include "edfuncs.h"
#include "history.h"
#include "xdialog.h"
#include "regexp.h"
#include "findandreplace.h"
#include "winutil.h"
#include "themes.h"

#define ISRADIODLGCTL(i) 	((i) >= IDD_LOWRADIO && (i) <= IDD_HIGHRADIO)
#define ISFLAGDLGCTL(i) 		(((i) >= IDD_LOWOPT && (i) <= IDD_HIGHOPT) ||\
						(i) == IDD_3S1 || (i) == IDD_3S2)

extern int 		_translatekeys;

extern int		mysprintf(WINFO *wp, char *d,char *format,...);
extern void 	macro_returnString(char *string);
extern BOOL 	DlgChooseFont(HWND hWnd, EDTEXTSTYLE *ep, BOOL bPrinter);

static DLG_ITEM_TOOLTIP_MAPPING* _dtoolTips;
static DIALPARS 	*_dp;
static DIALPARS*	(*_dialogInitParameterCallback)(int pageIndex);
static boolean		bInPropertySheet;
static boolean		bPropertySheetMove;

int	   nCurrentDialog;
HWND   hwndDlg;

/*
 * Return a string resource for the current language. Note, that this method is not
 * re-entrant. Any text loaded must be used right away.
 */
char* dlg_getResourceString(int nId) {
	static char szBuf[256];

	if (!LoadString(ui_getResourceModule(), nId, szBuf, sizeof szBuf)) {
		return (char*)NULL;
	}
	return szBuf;
}

/*-----------------------------------------------
 * sym_assignSymbol a callback to be invoked to return the DIALOGPARS for a page (in a property sheet)
 * for that particular page, if the page is activated. The callback is passed the index of the 
 * property page activated.
 */
void dlg_setXDialogParams(DIALPARS* (*func)(int pageIndex), boolean positionDialogOnInit) {
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
	HWND hwndTool = GetDlgItem(hDlg, toolID);

	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(0L, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hDlg, NULL,
		hInst, NULL);

	if (!hwndTool || !hwndTip)
	{
		return (HWND)NULL;
	}

	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = hDlg;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hwndTool;
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

static void applyRadioButtons(int idCtrl, DIALPARS* dp) {
	int item;
	if (ISRADIODLGCTL(idCtrl)) {
		while ((item = dp->dp_item) != 0) {
			if (ISRADIODLGCTL(item)) {
				if (idCtrl >= item && idCtrl <= item + dp->dp_size) {
					*(int*)dp->dp_data = idCtrl - item;
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
int DoDialog(int nIdDialog, DLGPROC DlgProc, DIALPARS *dp, DLG_ITEM_TOOLTIP_MAPPING* pTooltips) {
	int 		nSave;
	HWND		hwnd;
	INT_PTR		ret;

	bInPropertySheet = FALSE;
	if (dp)	{
		_dp = dp;
	}
	_dtoolTips = pTooltips;
	hwnd = GetActiveWindow();
	nSave = nCurrentDialog;
	nCurrentDialog = nIdDialog;

	HRSRC  hRes = FindResource(ui_getResourceModule(), MAKEINTRESOURCE(nIdDialog), RT_DIALOG);
	DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)LoadResource(ui_getResourceModule(), hRes);
	ret = DialogBoxIndirect(0, pTemplate, hwndMain, DlgProc);

	//ret = DialogBox(ui_getResourceModule(), MAKEINTRESOURCE(nIdDialog), hwndMain, DlgProc);
	if (ret == -1) {
		log_lastWindowsError("DoDialog");
	}
	nCurrentDialog = nSave;
	if (hwnd) {
		SetActiveWindow(hwnd);
	}

	return (int)ret;
}

/*--------------------------------------------------------------------------
 * DlgInitString()
 */
void DlgInitString(HWND hDlg, int item, LPSTR szString, int nMax)
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
	if (set) {
		lpfnNewProc = MakeProcInstance(lpfnNewProc,hInst);
	} 
	SetWindowLongPtr(hwndItem,GWLP_WNDPROC, (LONG_PTR) lpfnNewProc);
	if (!set) {
		FreeProcInstance(lpfnNowProc);
	}
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
			SetWindowText(hwnd,code2key(key));
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
				key = macro_addModifierKeys((KEYCODE) wParam);
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
{	int    blkvalid,item,range;
	char	  szSel[64];
	HWND	  hwndList;

	blkvalid = ww_hasSelection(ww_getCurrentEditorWindow());
	if (*rangetype == RNG_BLOCK && !blkvalid)
		*rangetype = RNG_CHAPTER;

	hwndList = GetDlgItem(hwnd, IDD_RNGE);
	SendMessage(hwndList, CB_RESETCONTENT,0,0L);

	szSel[0] = 0;

	for (item = IDS_RNGONCE; item <= IDS_RNGGLOBAL; item++) {
		range = item-IDS_RNGONCE;
		char* pszBuf = dlg_getResourceString(item);
		if (range >= first && 
		    (range != RNG_BLOCK || blkvalid)) {
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

	item = SendDlgItemMessage(hwnd,id,CB_GETCURSEL,0,0);
	if (item == LB_ERR) {
		*((char *)szBuff) = 0;
		return LB_ERR;
	}
	SendDlgItemMessage(hwnd,id,CB_GETLBTEXT,item,(LPARAM)szBuff);
	return 1;
}

/*--------------------------------------------------------------------------
 * dlg_getListboxText()
 */
int dlg_getListboxText(HWND hwnd, int id, void *szBuff) { 	
	LRESULT	  item;

	item = SendDlgItemMessage(hwnd,id,LB_GETCURSEL,0,0);
	if (item == LB_ERR) {
		*((char *)szBuff) = 0;
		return LB_ERR;
	}
	SendDlgItemMessage(hwnd,id,LB_GETTEXT,item,(LPARAM)szBuff);
	return 1;
}

/*--------------------------------------------------------------------------
 * dlg_selectComboBoxItem()
 */
static int dlg_selectComboBoxItem(HWND hwnd, WORD id, char *szBuff)
{
	LRESULT ret;

	ret = SendDlgItemMessage(hwnd, id, CB_SELECTSTRING, -1, (LPARAM)szBuff);
	if (ret < 0)
		SendDlgItemMessage(hwnd, id, CB_SETCURSEL, 0, 0L);
	return (int) ret;
}

/*--------------------------------------------------------------------------
 * dlg_getRangeForOperation()
 */
static int dlg_getRangeForOperation(HWND hwnd)
{ 	WORD	 item;
	char*	 pszBuf;
	char	 szCurr[130];

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
 * DoDldInitPars()
 */
BOOL DoDlgInitPars(HWND hDlg, DIALPARS *dp, int nParams)
{
	char 	cbuf[2],numbuf[32];
	int 	item,*ip;
	BOOL	moved;
	DIALLIST	*dlp;
	HISTORY_TYPE ht;

	moved = FALSE;
	while(--nParams >= 0 && (item = dp->dp_item) != 0) {
		ip = (int*)dp->dp_data;
		switch(item) {
			case IDD_POSITIONTCURS:
				moved = win_positionWindowRelativeToCaret(hDlg);
				break;
			case IDD_STRINGLIST1:
			case IDD_STRINGLIST2:
				dlp = (DIALLIST*)dp->dp_data;
				(*dlp->li_fill)(hDlg, item, (void*)dlp->li_param);
				break;
			case IDD_WINDOWLIST:
			case IDD_ICONLIST:
			case IDD_FONTSEL2COLOR:
				dlp = (DIALLIST*)dp->dp_data;
				(*dlp->li_fill)(hDlg,item, (void*) (intptr_t) *dlp->li_param);
				break;
			case IDD_CSEL:
				SendDlgItemMessage(hDlg,item,WM_CHARCHANGE,
							    *(unsigned char*)dp->dp_data,0L);
				break;
			case IDD_WINTITLE2:
				SetWindowText(hDlg,(LPSTR)ip);
				break;
			case IDD_WINTITLE: {
					char* pszString = dlg_getResourceString((WORD)dp->dp_size);
					if (pszString) {
						SetWindowText(hDlg, pszString);
					}
				}
				break;
			case IDD_FILE_PATTERN:
				ht = FILE_PATTERNS;
			case IDD_FINDS2: case IDD_FINDS: case IDD_REPLS:
				if (item == IDD_REPLS) {
					ht = SEARCH_AND_REPLACE;
				} else if (item != IDD_FILE_PATTERN) {
					ht = SEARCH_PATTERNS;
				}
				hist_fillComboBox(hDlg, item, ht);
				LPSTR pData = (LPSTR)ip;
				if (pData && *pData) {
					SetDlgItemText(hDlg, item, pData);
				}
				break;
			case IDD_RO1: case IDD_RO2: case IDD_RO3: 
			case IDD_RO4: case IDD_RO5: case IDD_RO6:
				SetDlgItemText(hDlg,item,(LPSTR)ip);
				break;
			case IDD_PATH1:
			case IDD_STRING1: case IDD_STRING2: case IDD_STRING3:
			case IDD_STRING4: case IDD_STRING5: case IDD_STRING6:
			case IDD_STRING7: 
				DlgInitString(hDlg,item,(LPSTR)ip,dp->dp_size);
				break;
			case IDD_LONG1:
				wsprintf(numbuf,"%ld",*(long*)dp->dp_data);
				goto donum;
			case IDD_INT1: case IDD_INT2: case IDD_INT3: 
			case IDD_INT4: case IDD_INT5: case IDD_INT6:
			case IDD_INT7: case IDD_INT8:
				if (*ip < 0) {
					numbuf[0] = 0;
				} else {
					wsprintf(numbuf, "%d", *ip);
				}
donum:			DlgInitString(hDlg,item,numbuf,sizeof numbuf-1);
				break;
			case IDD_RNGE:
				dlg_setRangeForOperation(hDlg,ip,dp->dp_size);
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
				cbuf[0] = *(char*)dp->dp_data;
				cbuf[1] = 0;
				DlgInitString(hDlg,item,cbuf,sizeof cbuf-1);
				break;
			default:
				if (ISFLAGDLGCTL(item)) {
					CheckDlgButton(hDlg,item,(*ip & dp->dp_size) ? 1 : 0);
				} else if (ISRADIODLGCTL(item)) {
					CheckRadioButton(hDlg,item,item+dp->dp_size,*ip+item);
				}
		}
		dp++;
	}
	return moved;
}

/*--------------------------------------------------------------------------
 * DlgInit()
 */
static void DlgInit(HWND hDlg, DIALPARS *dp, BOOL initialFlag) {
	static int _intFields[] = { IDD_INT1, IDD_INT2, IDD_INT3, IDD_INT4, IDD_INT5 };
	static int _spinFields[] = { IDC_SPIN1, IDC_SPIN2, IDC_SPIN3, IDC_SPIN4, IDC_SPIN5 };
	DIALPARS *dp2;
	int		nPars;

	for (dp2 = dp, nPars = 0; dp2->dp_item; dp2++) {
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
	if (!DoDlgInitPars(hDlg, dp, nPars) && initialFlag) {
		if (!bInPropertySheet) {
			win_moveWindowToDefaultPosition(hDlg);
		}
	}
}

/*--------------------------------------------------------------------------
 * Do3State()
 * toggle state of 2 Buttons in one of the following ways:
 * B1, B2, noButton
 */
static void Do3State(HWND hDlg, WORD item, WORD b1, WORD b2)
{
	if (item != b1) {
		if (item != b2)
			return;
		b2 = b1;
		b1 = item;
	}
	/* now: b1 is pressed */
	if (IsDlgButtonChecked(hDlg, b1)) {
		CheckDlgButton(hDlg, b2, 0);
	}
}

/*--------------------------------------------------------------------------
 * GetItemDialListData()
 */
static DIALPARS *GetItemDialListData(DIALPARS *dp, int nItem)
{
	while(dp->dp_item != 0) {
		if (dp->dp_item == nItem) {
			return dp;
		}
		dp++;
	}
	return (DIALPARS*)0;
}

/*--------------------------------------------------------------------------
 * DoDlgRetreivePars()
 */
void DoDlgRetreivePars(HWND hDlg, DIALPARS *dp, int nMax)
{
	while(--nMax >= 0 && dp->dp_item) {
		switch(dp->dp_item) {

		case IDD_STRING1:
		case IDD_STRING2:
		case IDD_STRING3:
		case IDD_STRING4:
		case IDD_STRING5:
		case IDD_STRING6:
		case IDD_STRING7:
		case IDD_FILE_PATTERN:
		case IDD_PATH1:
			GetDlgItemText(hDlg, dp->dp_item, (LPSTR)dp->dp_data, 
				dp->dp_size);
			break;

		}
		dp++;
	}
}

/*--------------------------------------------------------------------------
 * macro_getReplaceActionForControlId()
 */
static int macro_getReplaceActionForControlId(int idCtrl)
{
	switch (idCtrl) {
	case IDD_BUT3: return REP_MARK;
	case IDD_BUT4: return REP_COUNT;
	case IDCANCEL: return 0;
	default:       return REP_REPLACE;
	}
}

/*--------------------------------------------------------------------------
 * DlgApplyChanges()
 */
static BOOL DlgApplyChanges(HWND hDlg, INT idCtrl, DIALPARS *dp)
{
	char 	cbuf[2],numbuf[32];
	int  	item,*ip;
	BOOL	buttonChecked;

	while((item = dp->dp_item) != 0) {
		ip = (int*)dp->dp_data;
		switch(item) {
		case IDD_RECORDRET:
			*ip = macro_getReplaceActionForControlId(idCtrl);
			break;
		case IDD_CSEL:
			*(char*)dp->dp_data = (char)(short)(long)
				SendDlgItemMessage(hDlg,item,CS_QUERYCHAR,0,0L);
			break;
		case IDD_NOCHANGEONCANCEL:	
			if (idCtrl == IDCANCEL) {
				return TRUE;
			}
			break;
		case IDD_STRINGLIST1:
		case IDD_STRINGLIST2: {
			DIALLIST* dpi = dp->dp_data;
			SendDlgItemMessage(hDlg, item, CB_GETLBTEXT, SendDlgItemMessage(hDlg, item, CB_GETCURSEL, 0, 0), (LPARAM)dpi->li_param);
			break;
		}
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
			ip = (int*)dp->dp_data;
			buttonChecked = IsDlgButtonChecked(hDlg, dp->dp_item);
			*ip = buttonChecked ?
				*ip | dp->dp_size :
				*ip & ~dp->dp_size;
			break;
		case IDD_FONTSEL2COLOR:
		case IDD_ICONLIST:
			(*((DIALLIST*)dp->dp_data)->li_get)(hDlg,item,
				((DIALLIST*)dp->dp_data)->li_param);
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
			GetDlgItemText(hDlg,item,(LPSTR)ip,
						dp->dp_size);
			if (idCtrl != IDCANCEL) {
				if ((item == IDD_FINDS2 && *(char*)ip) ||
				    (item == IDD_FINDS)) {
					if (!regex_compileWithDefault((LPSTR)ip)) {
						error_showError("Falscher regulärer Ausdruck.", NULL);
						return FALSE;
					}
					hist_saveString(SEARCH_PATTERNS, (LPSTR)ip);
				}
				if ( idCtrl == IDOK && 
					item == IDD_REPLS && 
					!find_initializeReplaceByExpression(dp->dp_data)) {
					return FALSE;
				}
			}
			break;
		case IDD_INT1: case IDD_INT2: case IDD_INT3: 
		case IDD_INT4: case IDD_INT5: case IDD_INT6:
		case IDD_INT7:  case IDD_INT8:
		case IDD_LONG1: 
			GetDlgItemText(hDlg,item,numbuf,sizeof numbuf-1);
			if (item == IDD_LONG1) {
				*((long*)dp->dp_data) = string_convertToLong(numbuf);
			} else {
				*ip = numbuf[0] == 0 ? -1 : string_convertToLong(numbuf);
			}
			break;
		case IDD_RNGE:
			*ip = dlg_getRangeForOperation(hDlg);
			break;
		case IDD_KEYCODE:
			*(KEYCODE*)dp->dp_data = 
				(KEYCODE) SendDlgItemMessage(hDlg,item,
							    CS_QUERYCHAR,0,0L);
			break;
		case IDD_RAWCHAR:
			/* drop through */
		case IDD_CHAR:
			GetDlgItemText(hDlg,item,cbuf,sizeof cbuf);
			*(char*)dp->dp_data = cbuf[0];
			break;
		default:
			if (ISFLAGDLGCTL(item)) {
				*ip = (IsDlgButtonChecked(hDlg, item)) ?
					*ip | dp->dp_size : 
					*ip & ~dp->dp_size;
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
static char* dlg_getTitleResource(HWND hDlg, int idCtrl, char* szButton, size_t nSize) {
	GetWindowText(GetDlgItem(hDlg, idCtrl), szButton, (int)nSize);
	return (szButton[0] == '&') ? szButton + 1 : szButton;
}

/*--------------------------------------------------------------------------
 * DlgCommand()
 */
static BOOL DlgCommand(HWND hDlg, WPARAM wParam, LPARAM lParam, DIALPARS *dp)
{
	int		idCtrl;
	char 	fselbuf[256],szBuff[256],szButton[28];
	int  	nNotify,(*callback)();
	LONG 	c;
	DIALPARS *dp2;
	char* pszTitle;

	callback = 0;
	idCtrl = GET_WM_COMMAND_ID(wParam, lParam);
	nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
	switch(idCtrl) {
		case IDD_PATH1SEL:
			pszTitle = dlg_getTitleResource(hDlg, idCtrl, szBuff, sizeof szBuff);
			if (fsel_selectFolder(pszTitle, _fseltarget)) {
				SetDlgItemText(hDlg, IDD_PATH1, _fseltarget);
			}
			break;
		case IDD_PATH2SEL:
			lstrcpy(szBuff,".\\");
			fselbuf[0] = 0;
			pszTitle = dlg_getTitleResource(hDlg, idCtrl, szButton, sizeof szButton);
			FILE_SELECT_PARAMS fsp;
			fsp.fsp_saveAs = TRUE;
			fsp.fsp_resultFile = _fseltarget;
			fsp.fsp_inputFile = szBuff;
			fsp.fsp_namePatterns = fselbuf;
			fsp.fsp_title = pszTitle;
			fsp.fsp_encrypted = FALSE;
			if (fsel_selectFile(&fsp)) {
				SetDlgItemText(hDlg, IDD_PATH1, 	_fseltarget);
			}
			break;
		case IDD_FONTSELECT2:
		case IDD_FONTSELECT:
			if ((dp2 = GetItemDialListData(dp, idCtrl)) != 0) {
				DlgChooseFont(hDlg, (EDTEXTSTYLE *)dp2->dp_data, 
					(BOOL)dp2->dp_size);
			}
			break;
		case IDD_3S1: case IDD_3S2:
			Do3State(hDlg,idCtrl,IDD_3S1,IDD_3S2);
			break;
		case IDD_REGEXP: case IDD_SHELLJOKER:
			Do3State(hDlg,idCtrl,IDD_REGEXP,IDD_SHELLJOKER);
			break;
		case IDD_CSEL:
			if (nNotify == CSN_CHARSELECT) {
				c = (LONG)LOWORD(lParam);
				mysprintf((WINFO*)0,szBuff,
					"DEZ: %03j  OKT: %03i  HEX: 0x%02p  BIN: %08b",c,c,c,c);
				SetDlgItemText(hDlg,IDD_CSELT1,szBuff);
			}
			break;
		case IDD_ICONLIST:
		case IDD_WINDOWLIST:
		case IDD_STRINGLIST1:
		case IDD_STRINGLIST2:
			if (nNotify == LBN_SELCHANGE) {
				if ((dp2 = GetItemDialListData(dp, idCtrl)) != 0 &&
					((DIALLIST*)dp2->dp_data)->li_command) {
					(*((DIALLIST*)dp2->dp_data)->li_command)(
						hDlg, idCtrl, nNotify,
						((DIALLIST*)dp2->dp_data)->li_param);
				}
			}
			break;
		case IDD_STRING1:
		case IDD_STRING2:
		case IDD_STRING3:
			if (nNotify != EN_KILLFOCUS) {
				break;
			}
			for (dp2 = dp; dp2->dp_item; dp2++) {
				if (dp2->dp_item == IDD_ENKILLFOCUS && dp2->dp_size == idCtrl) {
					DoDlgRetreivePars(hDlg, dp, 10000);
					callback = (int (*)())dp2->dp_data;
					goto endd;
				}
			}
			break;
		case IDD_NOINITCALLBACK:
			if ((dp2 = GetItemDialListData(dp, idCtrl)) != 0) {
				callback = (int (*)())dp2->dp_data;
				(*callback)(hDlg,idCtrl);
			}
			return TRUE;
		case IDD_CALLBACK1:	case IDD_CALLBACK2:	case IDD_CALLBACK3:
		case IDD_RAWCHAR:
		case IDOK: case IDCANCEL: case IDD_BUT3: case IDD_BUT4: case IDD_BUT5:
			if (idCtrl != IDCANCEL && !DlgApplyChanges(hDlg, idCtrl, dp)) {
				return FALSE;
			}
			if ((dp2 = GetItemDialListData(dp, idCtrl)) != 0) {
				callback = (int (*)())dp2->dp_data;
			}
endd:		if (callback && idCtrl != IDD_RAWCHAR) {
				(*callback)(hDlg,idCtrl);
				return TRUE;
			}
			EndDialog(hDlg,idCtrl);
			return TRUE;
		default:
			applyRadioButtons(idCtrl, dp);
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
 * dlg_getitemdata()
 */
static void *dlg_getitemdata(DIALPARS *dp, WORD item)
{
	while(dp->dp_item) {
		if (dp->dp_item == item)
			return dp->dp_data;
		dp++;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * dlg_standardDialogProcedure()
 */
static BOOL CALLBACK DlgNotify(HWND hDlg, WPARAM wParam, LPARAM lParam)
{	LPNMHDR		pNmhdr;
	INT			idCtrl;
	HWND		parent;
	int idx;

	pNmhdr = (LPNMHDR) lParam;
	idCtrl = (INT)wParam;
	parent = GetParent(hDlg);
	switch(pNmhdr->code) {
	case PSN_SETACTIVE:
		idx = PropSheet_HwndToIndex(parent, hDlg);
		_dp = _dialogInitParameterCallback(idx);
		if (_dp != NULL) {
			DlgInit(hDlg, _dp, FALSE);
		}
		PropSheet_UnChanged(parent, hDlg);
		return TRUE;
	case PSN_APPLY:
		if (_dp != NULL) {
			DlgApplyChanges(hDlg, IDOK, _dp);
		}
		PropSheet_UnChanged(parent, hDlg);
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
		return TRUE;
	}
	return FALSE;
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
	DIALLIST			*dlp;

	switch(message) {
		case WM_NOTIFY:
			return DlgNotify(hDlg, wParam, lParam);

		case WM_INITDIALOG:
			hwndDlg = hDlg;
			if (bPropertySheetMove) {
				bPropertySheetMove = FALSE;
				win_moveWindowToDefaultPosition(GetParent(hDlg));
			} else if (_dp != NULL) {
				DlgInit(hDlg, _dp, TRUE);
			}
			if (_dtoolTips != NULL) {
				for (int i = 0; _dtoolTips[i].m_itemId > 0; i++) {
					CreateToolTip(_dtoolTips[i].m_itemId, hwndDlg, _dtoolTips[i].m_tooltipStringId);
				}
			}
			break;

		case WM_DESTROY:
			bPropertySheetMove = FALSE;
			bInPropertySheet = FALSE;
			return FALSE;

		case WM_MEASUREITEM:
			mp = (MEASUREITEMSTRUCT*)lParam;
			if ((dlp = dlg_getitemdata(_dp, mp->CtlID)) == 0 ||
			    (dlp->li_measure == 0))
				break;
			(*dlp->li_measure)(mp);
			return TRUE;

		case WM_COMPAREITEM:
			cp = (COMPAREITEMSTRUCT*)lParam;
			if ((dlp = dlg_getitemdata(_dp, cp->CtlID)) == 0 ||
			    (dlp->li_compare == 0))
				break;
			return (*dlp->li_compare)(cp);

		case WM_DRAWITEM:
			drp = (DRAWITEMSTRUCT*)lParam;
			if ((dlp = dlg_getitemdata(_dp, drp->CtlID)) == 0 ||
			    (dlp->li_draw == 0))
				break;
			return cust_drawComboBoxOwnerDraw(drp, dlp->li_draw, dlp->li_selection);

		case WM_COMMAND:
			nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
			idCtrl = GET_WM_COMMAND_ID(wParam, lParam);
			if ((nNotify == EN_CHANGE || ISFLAGDLGCTL(idCtrl) || ISRADIODLGCTL(idCtrl)) &&
					bInPropertySheet && (LPARAM)GetParent(hDlg) != lParam) {
				PropSheet_Changed(GetParent(hDlg), hDlg);
			}
			if (DlgCommand(hDlg,wParam,lParam,_dp)) {
				hwndDlg = 0;
				return TRUE;
			}
			break;
	}
	return dlg_defaultWndProc(hDlg, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * win_callDialog()
 */
int win_callDialog(int nId, PARAMS *pp, DIALPARS *dp, DLG_ITEM_TOOLTIP_MAPPING* pTooltips)
{ 	int ret = 1;

	if (macro_openDialog(pp)) {
		ret = DoDialog(nId, dlg_standardDialogProcedure,dp, pTooltips);
		macro_recordOperation(pp);
		if (ret == IDCANCEL || ret == -1)
			return 0;
	}
	return ret;
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

/*--------------------------------------------------------------------------
 * EdPromptAssign()
 */
long EdPromptAssign(long unused1, long unused2, char *prompt, char *init)
{
	char 	buf[128];
	static ITEMS	_i    =  	{ C_STRING1PAR,  (unsigned char * ) 0 };
	static PARAMS	_np   = 	{ 1, P_MAYOPEN, _i	  };
	static DIALPARS _d[] = 	{
		IDD_RO1,		128,			0,
		IDD_STRING1,	128, 		0,
		0
	};

	if (!prompt) {
		prompt = " > ";
	}
	_d[0].dp_data = prompt;
	_d[1].dp_data = buf;
	_i->p.s = buf;
	if (init) {
		lstrcpy(buf, init);
	} else {
		buf[0] = 0;
	}

	if (win_callDialog(DLGPROMPT,&_np,_d, NULL) != IDOK) {
		*buf = 0;
	}

	macro_returnString(buf);

	return 1;
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
	}
	return dlg_defaultWndProc(hwnd, nMessage, wParam, lParam);
}



