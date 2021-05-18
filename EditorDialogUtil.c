/*
 * EditorDialogUtil.c
 * 
 * 
 * 
 *
 * PROJEKT: PKS-EDIT for ATARI - WINDOWS
 *
 * purpose: some WINDOWS dialogs for general and special purpose
 *
 * 										created      : 01.01.90
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winterf.h"
#include "winfo.h"
#include "stringutil.h"

#pragma hdrstop

#include "pksedit.h"
#include "dial2.h"
#include "edfuncs.h"
#include "edhist.h"
#include "xdialog.h"

#define ISRADIODLGCTL(i) 	((i) >= IDD_LOWRADIO && (i) <= IDD_HIGHRADIO)
#define ISFLAGDLGCTL(i) 		(((i) >= IDD_LOWOPT && (i) <= IDD_HIGHOPT) ||\
						(i) == IDD_3S1 || (i) == IDD_3S2)

extern char 		_fseltarget[];
extern int 		_translatekeys;

extern int		mysprintf(FTABLE *fp, char *d,char *format,...);
extern FTABLE		*ww_stackwi(int num);
extern void 		ReturnString(char *string);
extern BOOL 		DlgChooseFont(HWND hWnd, EDFONT *ep, BOOL bPrinter);
extern int 		cust_combood(LPDRAWITEMSTRUCT lpdis, void (*DrawEntireItem)(), 
					void (*ShowSelection)(LPDRAWITEMSTRUCT lp));
extern void 		fsel_title(char *title);

static DIALPARS 	*_dp;
static DIALPARS*	(*_dialogInitParameterCallback)(int pageIndex);
static boolean		bInPropertySheet;
static boolean		bPropertySheetMoved;

int	   nCurrentDialog;
HWND   hwndDlg;

/*-----------------------------------------------
 * Assign a callback to be invoked to return the DIALOGPARS for a page (in a property sheet)
 * for that particular page, if the page is activated. The callback is passed the index of the 
 * property page activated.
 */
void SetXDialogParams(DIALPARS* (*func)(int pageIndex), boolean propertySheetFlag) {
	_dialogInitParameterCallback = func;
	bInPropertySheet = propertySheetFlag;
	bPropertySheetMoved = FALSE;
}

/*--------------------------------------------------------------------------
 * DoDialog()
 */
int DoDialog(int nIdDialog, FARPROC DlgProc, DIALPARS *dp)
{
	int 		nSave;
	HWND		hwnd;
	INT_PTR		ret;

	_translatekeys = 0;
	bInPropertySheet = FALSE;
	if (dp)	{
		_dp = dp;
	}
	hwnd = GetActiveWindow();
	nSave = nCurrentDialog;
	nCurrentDialog = nIdDialog;
	ret = DialogBox(hInst, MAKEINTRESOURCE(nIdDialog), hwndFrame, DlgProc);
	nCurrentDialog = nSave;

	if (hwnd)
		SetActiveWindow(hwnd);
	_translatekeys = 1;

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
FARPROC SubClassWndProc(int set, HWND hDlg, int item, FARPROC lpfnNewProc)
{	HWND hwndItem;
	FARPROC lpfnNowProc;

	hwndItem = GetDlgItem(hDlg,item);
	lpfnNowProc = (FARPROC)GetWindowLongPtr(hwndItem,GWLP_WNDPROC);
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
 * SendParentCommand()
 */
void SendParentCommand(HWND hwnd,  LPARAM lParam)
{
	SendMessage(GetParent(hwnd),WM_COMMAND,GetDlgCtrlID(hwnd),lParam);
}

/*------------------------------------------------------------
 * KeyInputWndProc()
 */
static FARPROC lpfnOldCInput;
WINFUNC KeyInputWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
			key = wParam;
			SetWindowText(hwnd,code2key(key));
			SendRedraw(hwnd);
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
				key = mac_addshift(wParam);
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
WINFUNC CInput(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{	char szBuff[10];

	switch(message) {
		case WM_GETDLGCODE:
			return DLGC_WANTCHARS;

		case WM_CHAR:
			szBuff[1] = 0;
			szBuff[0] = wParam;
			SetWindowText(hwnd,szBuff);
			SendRedraw(hwnd);
			SendParentCommand(hwnd,0L);
			return 0;
	}
	return CallWindowProc(lpfnOldCInput,hwnd,message,wParam,lParam);
}

/*--------------------------------------------------------------------------
 * setwrange()
 */
int setwrange(HWND hwnd, int *rangetype, int first)
{	int    blkvalid,item,range;
	char	  szBuf[64],szSel[64];
	HWND	  hwndList;

	blkvalid = _chkblk(_currfile);
	if (*rangetype == RNG_BLOCK && !blkvalid)
		*rangetype = RNG_CHAPTER;

	hwndList = GetDlgItem(hwnd, IDD_RNGE);
	SendMessage(hwndList, CB_RESETCONTENT,0,0L);

	szSel[0] = 0;

	for (item = IDS_RNGONCE; item <= IDS_RNGGLOBAL; item++) {
		range = item-IDS_RNGONCE;
		LoadString(hInst,item,szBuf,sizeof szBuf);
		if (range >= first && 
		    (range != RNG_BLOCK || blkvalid)) {
			SendMessage(hwndList, CB_ADDSTRING, 0, (LPARAM)szBuf);
		}
		if (range == *rangetype) {
			lstrcpy(szSel,szBuf);
		}
	}

	SendMessage(hwndList, CB_SELECTSTRING, -1, (LPARAM)szSel);
	return *rangetype;	
}

/*--------------------------------------------------------------------------
 * CbGetText()
 */
int CbGetText(HWND hwnd, WORD id, char *szBuff)
{ 	LRESULT	  item;

	item = SendDlgItemMessage(hwnd,id,CB_GETCURSEL,0,0);
	if (item == LB_ERR) {
		*szBuff = 0;
		return LB_ERR;
	}
	SendDlgItemMessage(hwnd,id,CB_GETLBTEXT,item,(LPARAM)szBuff);
	return 1;
}

/*--------------------------------------------------------------------------
 * LbGetText()
 */
int LbGetText(HWND hwnd, WORD id, char *szBuff)
{ 	LRESULT	  item;

	item = SendDlgItemMessage(hwnd,id,LB_GETCURSEL,0,0);
	if (item == LB_ERR) {
		*szBuff = 0;
		return LB_ERR;
	}
	SendDlgItemMessage(hwnd,id,LB_GETTEXT,item,(LPARAM)szBuff);
	return 1;
}

/*--------------------------------------------------------------------------
 * CbSelectItem()
 */
static int CbSelectItem(HWND hwnd, WORD id, char *szBuff)
{
	LRESULT ret;

	ret = SendDlgItemMessage(hwnd, id, CB_SELECTSTRING, -1, (LPARAM)szBuff);
	if (ret < 0)
		SendDlgItemMessage(hwnd, id, CB_SETCURSEL, 0, 0L);
	return (int) ret;
}

/*--------------------------------------------------------------------------
 * getwrange()
 */
int getwrange(HWND hwnd)
{ 	WORD	 item;
	char	 szBuff[30];
	char	 szCurr[130];

	if (CbGetText(hwnd, IDD_RNGE, szCurr) == LB_ERR) {
		return RNG_FREE;
	}
	for (item = IDS_RNGONCE; item <= IDS_RNGGLOBAL; item++) {
		LoadString(hInst,item,szBuff, sizeof szBuff -1);
		if (strcmp(szBuff,szCurr) == 0)
			return item-IDS_RNGONCE+RNG_ONCE;
	}
	return RNG_FREE;
}

/*--------------------------------------------------------------------------
 * DoDldInitPars()
 */
BOOL DoDlgInitPars(HWND hDlg, DIALPARS *dp, int nParams)
{
	char 	cbuf[2],numbuf[32],szBuff[128];
	int 	item,*ip;
	BOOL	moved;
	DIALLIST	*dlp;

	moved = FALSE;
	while(--nParams >= 0 && (item = dp->dp_item) != 0) {
		ip = (int*)dp->dp_data;
		switch(item) {
			case IDD_POSITIONTCURS:
				moved = form_textcursor(hDlg);
				OwnTextCursor(1);
				break;
			case IDD_WINDOWLIST:
			case IDD_ICONLIST:
			case IDD_FONTSEL2COLOR:
				dlp = (DIALLIST*)dp->dp_data;
				(*dlp->li_fill)(hDlg,item,*dlp->li_param);
				break;
			case IDD_CSEL:
				SendDlgItemMessage(hDlg,item,WM_CHARCHANGE,
							    *(unsigned char*)dp->dp_data,0L);
				break;
			case IDD_WINTITLE2:
				SetWindowText(hDlg,(LPSTR)ip);
				break;
			case IDD_WINTITLE:
				LoadString(hInst,(WORD)dp->dp_size,szBuff, sizeof szBuff -1);
				SetWindowText(hDlg,szBuff);
				break;
			case IDD_FINDS2: case IDD_FINDS: case IDD_REPLS:
				hist_2combo(hDlg,item,szBuff,(item != IDD_REPLS) ?
						  &_findhist : &_replhist);
				if (!*(LPSTR)ip)
					lstrcpy((LPSTR)ip,szBuff);
			case IDD_RO1: case IDD_RO2: case IDD_RO3: 
			case IDD_RO4: case IDD_RO5:
				SetDlgItemText(hDlg,item,(LPSTR)ip);
				break;
			case IDD_PATH1:
			case IDD_STRING1: case IDD_STRING2: case IDD_STRING3:
			case IDD_STRING4: case IDD_STRING5: case IDD_STRING6:
			case IDD_STRING7: case IDD_FILE_PATTERN:
				DlgInitString(hDlg,item,(LPSTR)ip,dp->dp_size);
				break;
			case IDD_LONG1:
				wsprintf(numbuf,"%ld",*(long*)dp->dp_data);
				goto donum;
			case IDD_INT1: case IDD_INT2: case IDD_INT3: 
			case IDD_INT4: case IDD_INT5:
				if (*ip < 0) {
					numbuf[0] = 0;
				} else {
					wsprintf(numbuf, "%d", *ip);
				}
donum:			DlgInitString(hDlg,item,numbuf,sizeof numbuf-1);
				break;
			case IDD_RNGE:
				setwrange(hDlg,ip,dp->dp_size);
				break;
			case IDD_KEYCODE:
				lpfnOldCInput = SubClassWndProc(1,hDlg,item,
								      (FARPROC)KeyInputWndProc);
#if 0
				SendDlgItemMessage(hDlg,item,WM_CHARCHANGE,
								*(KEYCODE*)dp->dp_data,0L);
# endif
				break;
			case IDD_RAWCHAR:
				lpfnOldCInput = SubClassWndProc(1,hDlg,item,(FARPROC)CInput);
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
static void DlgInit(HWND hDlg, DIALPARS *dp, BOOL initialFlag)
{
	DIALPARS *dp2;
	int		nPars;

	for (dp2 = dp, nPars = 0; dp2->dp_item; dp2++) {
		nPars++;
	}
	if (!DoDlgInitPars(hDlg, dp, nPars) && initialFlag) {
		if (!bInPropertySheet) {
			form_move(hDlg);
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
	int  	*ip;

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
 * DlgCommand()
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
			*ip = ReplaceAction(idCtrl);
			break;
		case IDD_POSITIONTCURS:
			OwnTextCursor(0);
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
					if (!exprinit((LPSTR)ip)) { 
						return FALSE;
					}
				}
				if ( idCtrl == IDOK && 
					item == IDD_REPLS && 
					!repinit(dp->dp_data)) {
					return FALSE;
				}
			}
			break;
		case IDD_INT1: case IDD_INT2: case IDD_INT3: 
		case IDD_INT4: case IDD_INT5:
		case IDD_LONG1: 
			GetDlgItemText(hDlg,item,numbuf,sizeof numbuf-1);
			if (item == IDD_LONG1) {
				*(long*)dp->dp_data = Atol(numbuf);
			} else {
				*ip = numbuf[0] == 0 ? -1 : Atol(numbuf);
			}
			break;
		case IDD_RNGE:
			*ip = getwrange(hDlg);
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
			}
			break;
		} /*end switch */
		dp++;
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * DlgCommand()
 */
static BOOL DlgCommand(HWND hDlg, WPARAM wParam, LPARAM lParam, DIALPARS *dp)
{
	int		idCtrl;
	char 	fselbuf[256],szBuff[256],szButton[28];
	int  	nNotify,item,(*callback)();
	LONG 	c;
	DIALPARS *dp2;

	callback = 0;
	idCtrl = GET_WM_COMMAND_ID(wParam, lParam);
	nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
	switch(idCtrl) {
		case IDD_PATH2SEL:
		case IDD_PATH1SEL:
			lstrcpy(szBuff,".\\");
			fselbuf[0] = 0;
			GetWindowText(GetDlgItem(hDlg, idCtrl), 
				szButton, sizeof szButton);
			fsel_title(szButton+1);
			if (EdFsel(szBuff,fselbuf,_fseltarget, idCtrl != IDD_PATH1SEL)) {
				if (idCtrl == IDD_PATH1SEL) {
					sfsplit(_fseltarget, fselbuf, NULL);
					SetDlgItemText(hDlg, IDD_PATH1, fselbuf);
				} else {
					SetDlgItemText(hDlg, IDD_PATH1,
						_fseltarget);
				}
			}
			break;
		case IDD_FONTSELECT2:
		case IDD_FONTSELECT:
			if ((dp2 = GetItemDialListData(dp, idCtrl)) != 0) {
				DlgChooseFont(hDlg, (EDFONT *)dp2->dp_data, 
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
				mysprintf((FTABLE*)0,szBuff,
					"DEZ: %03j  OKT: %03i  HEX: 0x%02p  BIN: %08b",c,c,c,c);
				SetDlgItemText(hDlg,IDD_CSELT1,szBuff);
			}
			break;
		case IDD_ICONLIST:
		case IDD_WINDOWLIST:
			if ((dp2 = GetItemDialListData(dp, idCtrl)) != 0 &&
				((DIALLIST*)dp2->dp_data)->li_command) {
				(*((DIALLIST*)dp2->dp_data)->li_command)(
					hDlg, idCtrl, nNotify, 
					((DIALLIST*)dp2->dp_data)->li_param);
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
endd:		if (callback) {
				(*callback)(hDlg,idCtrl);
				return TRUE;
			}
			EndDialog(hDlg,idCtrl);
			return TRUE;
		default:
			if (ISRADIODLGCTL(idCtrl)) {
				while((item = dp->dp_item) != 0) {
					if (ISRADIODLGCTL(item)) {
						if (idCtrl >= item && idCtrl <= item+dp->dp_size) {
							*(int*)dp->dp_data = idCtrl-item;
							break;
						}
					}
					dp++;
				}
			}
			break;
	}
	return FALSE;
}
	
/*--------------------------------------------------------------------------
 * dlg_help()
 */
void dlg_help(void)
{
	if (nCurrentDialog != 0) {
		EdHelpContext(nCurrentDialog);
		return;
	}
	ed_error(IDS_MSGNOHELP);
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
 * DlgStdProc()
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
		DlgInit(hDlg, _dp, FALSE);
		PropSheet_UnChanged(parent, hDlg);
		return TRUE;
	case PSN_APPLY:
		DlgApplyChanges(hDlg, IDOK, _dp);
		PropSheet_UnChanged(parent, hDlg);
		return TRUE;
	case PSN_KILLACTIVE:
		SetWindowLongPtr(hDlg, DWLP_MSGRESULT, FALSE);
		break;
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * DlgStdProc()
 */
BOOL CALLBACK DlgStdProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	MEASUREITEMSTRUCT	*mp;
	COMPAREITEMSTRUCT	*cp;
	int					ic_lboxdrawitem();
	int					nNotify;
	int					idCtrl;
	DRAWITEMSTRUCT		*drp;
	DIALLIST			*dlp;
	
	switch(message) {
		case WM_NOTIFY:
			return DlgNotify(hDlg, wParam, lParam);

		case WM_INITDIALOG:
			hwndDlg = hDlg;
			if (bInPropertySheet) {
				if (!bPropertySheetMoved) {
					bPropertySheetMoved = TRUE;
					form_move(GetParent(hDlg));
				}
			} else if (_dp != NULL) {
				DlgInit(hDlg, _dp, TRUE);
			}
			return TRUE;

		case WM_DESTROY:
			bPropertySheetMoved = TRUE;
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
			return cust_combood(drp, dlp->li_draw, dlp->li_selection);

		case WM_COMMAND:
			nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
			idCtrl = GET_WM_COMMAND_ID(wParam, lParam);
			if ((nNotify == EN_CHANGE || ISFLAGDLGCTL(idCtrl)) &&
					bInPropertySheet && !(GetParent(hDlg) == lParam)) {
				PropSheet_Changed(GetParent(hDlg), hDlg);
			}
			if (DlgCommand(hDlg,wParam,lParam,_dp)) {
				hwndDlg = 0;
				return TRUE;
			}
			break;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * CallDialog()
 */
int CallDialog(int nId, PARAMS *pp, DIALPARS *dp)
{ 	int ret = 1;

	if (param_dialopen(pp)) {
		ret = DoDialog(nId,(FARPROC)DlgStdProc,dp);
		param_record(pp);
		if (ret == IDCANCEL)
			return 0;
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * CreateModelessDialog()
 */
void CreateModelessDialog(HWND *hwnd,LPSTR szName, BOOL (FAR PASCAL *func)(),
				      DLGPROC *lplpfnDlgProc)
{
	if (*hwnd) {
		return;
	}

	if (!*lplpfnDlgProc) {
		*lplpfnDlgProc = MakeProcInstance(func, hInst);
	}
	*hwnd = CreateDialog(hInst,szName,hwndFrame,*lplpfnDlgProc);
}

/*--------------------------------------------------------------------------
 * DestroyModelessDialog()
 */
void DestroyModelessDialog(HWND *hwnd)
{
	if (*hwnd) {
		DestroyWindow(*hwnd);
	}
}

/*------------------------------------------------------------
 * list_lboxfill()
 */
static void list_lboxfill(HWND hwnd, int nItem, void* selValue)
{
	DLGSTRINGLIST		*dlp;

	dlp = (DLGSTRINGLIST *)selValue;
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, FALSE, 0L);
	SendDlgItemMessage(hwnd, nItem, LB_RESETCONTENT, 0, 0L);
	while(dlp) {
		SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, (LPARAM)dlp);
		dlp = dlp->next;
	}
	SendDlgItemMessage(hwnd, nItem, WM_SETREDRAW, TRUE, 0L);
	SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING, -1, (LPARAM) selValue);
}

/*--------------------------------------------------------------------------
 * DoDlgSelectFromList()
 */
DLGSTRINGLIST *DoDlgSelectFromList(int nId, DLGSTRINGLIST *list, DIALLIST *dlist)
{
	DLGSTRINGLIST *elem;
	static DIALPARS _d[] = {
		IDD_ICONLIST,		0,		0,
		0
	};

	if (!list) {
		return 0;
	}
	_d[0].dp_data = dlist;
	dlist->li_param = (long*)&elem;
	dlist->li_fill = list_lboxfill;
	dlist->li_get = LbGetText;
	elem = list;
	if (DoDialog(nId, DlgStdProc, _d) == IDCANCEL) {
		return 0;
	}
	return elem;
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

	if (CallDialog(DLGPROMPT,&_np,_d) != IDOK) {
		*buf = 0;
	}

	ReturnString(buf);

	return 1;
}


