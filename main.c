/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * MAIN.C
 *
 * Register Window and Window Classes - main loop
 *
 * 						created: 07.06.91 
 *						Author : TOM
 */

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <ddeml.h>
#include <shellapi.h>
#include "trace.h"

#include "winterf.h"
#include "editab.h"
#include "winfo.h"

#pragma hdrstop

#include "pksedit.h"
#include "edierror.h"
#include "context.h"

#define	PROF_OFFSET	1

extern void 	ft_CheckForChangedFiles(void);
extern void		GetPhase2Args(char *args);
extern void		GetPhase1Args(char *args);
extern LONG 	PksGetKeyBind(WPARAM key);
extern void *	ll_insert(void *head,long size);
extern void 	EditDroppedFiles(HDROP hdrop);
extern BOOL 	InitBuffers(void);
extern void 	ShowMenuHelp(int menId);
extern BOOL 	ww_havefocus(void);
extern void 	st_init(HWND hwndDaddy);
extern void 	status_wh(WORD *width, WORD *height);
extern void 	tb_wh(WORD *width, WORD *height);
extern int 	EdCloseWindow(int winid);
extern void 	ReadConfigFiles(void);
extern int 	clp_setdata(int whichBuffer);
extern HMENU 	menu_getmenuforcontext(char *pszContext);
extern WORD 	TranslateToOrigMenu(WORD wParam);
extern BOOL 	InitEnv(void);
extern EDTIME 	EdGetFileTime(char *fname);
extern char *	MenuTipFor(int menId);

extern BOOL	bTaskFinished;

HINSTANCE		hInst;
HWND   			hwndFrame,hwndClient;
static DWORD	hDDE;
static HDDEDATA	hDDEService;
static HSZ		hszDDECommandLine;
static HSZ		hszDDEExecuteMacro;
static HSZ		hszDDEService;
static int		nInstanceCount;

# define	MAXCONTEXT		3
# define	TIMER_INTERVALL	1000

static HMENU	hDefaultMenu;

char   szFrameClass[] 	= "DeskWin";
char   szEditClass[]  	= "EditWin";
char   szStatusClass[]  	= "StatusWin";
char   szRulerClass[]  	= "RulerWin";
char   szWorkAreaClass[] = "WorkWin";
char   szAppName[]    	= "PKSEDIT";

int		_runInteractive = TRUE;
int		_openIconic = FALSE;

/*------------------------------------------------------------
 * EdMdiCreate()
 */
HWND EdMdiCreate(char *szClass, char *fn, int itemno, long lParam, WINDOWPLACEMENT *wsp)
{
	MDICREATESTRUCT 	mdicreate;
	HWND 				hwndChild;
	WINDOWPLACEMENT 	ws;

	mdicreate.szClass = szClass;
	mdicreate.szTitle = fn;
	mdicreate.hOwner = hInst;
	ws.length = 0;
	if (wsp == 0) {
		wsp = &ws;
		ww_winstate(itemno, wsp);
	} else {
		ws = *wsp;
		wsp = &ws;
	}
	mdicreate.style = 0;
	if (wsp->length) {
		mdicreate.x = wsp->rcNormalPosition.left;
		mdicreate.y = wsp->rcNormalPosition.top;
		mdicreate.cx = wsp->rcNormalPosition.right - wsp->rcNormalPosition.left + 1;
		mdicreate.cy = wsp->rcNormalPosition.bottom - wsp->rcNormalPosition.top + 1;
	} else {
		mdicreate.x = 
		mdicreate.y = 
		mdicreate.cx = 
		mdicreate.cy = CW_USEDEFAULT;
	}
    mdicreate.lParam = lParam;
	hwndChild = (HWND) SendMessage(hwndClient, WM_MDICREATE, 0,
		(LONG)(LPMDICREATESTRUCT)&mdicreate);

	if (hwndChild && ws.length) {
		SetWindowPlacement(hwndChild, wsp);
	} else {
		ShowWindow(hwndChild, SW_SHOW);
	}
	return hwndChild;
}

/*------------------------------------------------------------
 * EdMkWinClass()
 */
int EdMkWinClass(
	char   *szClassName,
	WINFUNCP WinProc,
	LPSTR  lpCursorName,
	HANDLE hbrBackground,
	LPSTR  szIcon,
	int    nExtra
)
{
	WNDCLASS  wc;
	
	class_defaults(&wc);
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WinProc;
	wc.cbWndExtra = nExtra;
	wc.hIcon = (szIcon) ? LoadIcon(hInst, szIcon) : 0;
	if (lpCursorName)
		wc.hCursor = LoadCursor(0, lpCursorName);
	wc.hbrBackground = hbrBackground;
	wc.lpszClassName = szClassName;
	
	return (RegisterClass(&wc));
}

/*------------------------------------------------------------
 * InitApplication()
 */
static BOOL InitApplication(void)
{
#if !defined(WIN32)
	if ((GetWinFlags() & WF_PMODE) == 0) {
		alert("REAL MODE not supported");
		return FALSE;
	}
#endif

	if ( !EdMkWinClass(szFrameClass,FrameWndProc,
			    NULL,COLOR_APPWORKSPACE,"Pks",0) ||
			!ic_register() ||
		 	!ww_register() ||
		 	!fkey_register() ||
		 	!cust_register()) {
	    return FALSE;
	}
	return TRUE;
}

/*------------------------------------------------------------
 * CloseAllChild()
 */
int CloseAllChild(HWND hwndChild)
{
	if (!hwndChild || !IsWindow(hwndChild))
		return 0;

	SendMessage(hwndClient, WM_MDIDESTROY, (WPARAM)hwndChild, (LPARAM)0L);
	return 1;
}

/*------------------------------------------------------------
 * CloseChildWin()
 */
int CloseChildWin(HWND hwndChild,int iconflag)
{	int ret;

	if (!hwndChild || !IsWindow(hwndChild))
		return 0;

	if ((ret = SendMessage(hwndChild, WM_QUERYENDSESSION,iconflag,0L)) == 1) {
		SendMessage(hwndClient, WM_MDIDESTROY, (WPARAM) hwndChild, (LPARAM)0L);
		return 1;
	}

	return (ret) ? 1 : 0;
}

/*------------------------------------------------------------
 * CloseEditChild()
 */
int CloseEditChild(HWND hwndChild)
{
	return CloseChildWin(hwndChild,0);
}

/*------------------------------------------------------------
 * InitInstance()
 */
static BOOL InitInstance(int nCmdShow, LPSTR lpCmdLine)
{
	DWORD				dwStyle;
	WINDOWPLACEMENT 	ws;
	char				szTitle[512];

	InitPrinterDC();
	InitDateformats();
	GetPhase1Args(lpCmdLine);
	ReadConfigFiles();
	hDefaultMenu = LoadMenu(hInst, "PksEdEditMenu");
	if (nInstanceCount > 1) {
		wsprintf(szTitle, "* PKS EDIT *", nInstanceCount);
	} else {
		lstrcpy(szTitle, "PKS EDIT");
	}

	dwStyle = WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN;
	hwndFrame = CreateWindow(szFrameClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, 0, hDefaultMenu, hInst, 0);

	if (!hwndFrame) {
		return FALSE;
	}
	DragAcceptFiles(hwndFrame, TRUE);
	while(!SetTimer(hwndFrame,TIM_FRAME,TIMER_INTERVALL,NULL)) {
		ed_error(IDS_MSGNOTIMER);
		return FALSE;
	}
	if (_openIconic) {
		nCmdShow = SW_SHOWMINNOACTIVE;
	}
	if (prof_getwinstate(szFrameClass, 0, &ws)) {
		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		if (ws.rcNormalPosition.bottom > rect.bottom) {
			ws.rcNormalPosition.bottom = rect.bottom;
		}
		if (ws.rcNormalPosition.right > rect.right) {
			ws.rcNormalPosition.right = rect.right;
		}
		if (ws.rcNormalPosition.left < rect.left) {
			ws.rcNormalPosition.left = rect.left;
		}
		if (ws.rcNormalPosition.top < rect.top) {
			ws.rcNormalPosition.top = rect.top;
		}
		SetWindowPlacement(hwndFrame, &ws);
	}
	ShowWindow(hwndFrame, nCmdShow);
	PostMessage(hwndFrame, WM_EDWINREORG, 0, 0L);
	return TRUE;
}

/*------------------------------------------------------------
 * TranslatePksAccel()
 */
int _translatekeys = 1;
static int TranslatePksAccel(HWND hwnd, MSG *msg)
{	LONG lParam;

	switch(msg->message) {
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYUP:
		case WM_KEYDOWN:
			if (msg->wParam == VK_MENU ||
			    msg->wParam == VK_CONTROL ||
			    msg->wParam == VK_SHIFT)
		    		break;
			if (!ww_havefocus()) {
				if (!(msg->hwnd == hwndClient || GetKeyState(VK_CONTROL) < 0)) {
					break;
				}
			}
			if ((lParam = PksGetKeyBind(msg->wParam)) != 0) {
				if (msg->message == WM_SYSKEYDOWN ||
				    msg->message == WM_KEYDOWN) {
					msg->message = WM_PKSKEY;
					msg->lParam = lParam;
					msg->hwnd = hwnd;
					break;
				}
				return 1;
			}
	}
	return 0;
}

/*------------------------------------------------------------
 * EdSleep()
 */
void EdSleep(void)
{ 	MSG msg;

	GetMessage(&msg, 0, WM_TIMER, WM_TIMER);
	DispatchMessage(&msg); 
}

static HDDEDATA CALLBACK EdDDECallback(UINT uType, UINT uFmt, HCONV hconv,
		HSZ hsz1, HSZ hsz2, HDDEDATA hdata, DWORD dw1, DWORD dw2) {
	char * pszData;

	switch (uType) {
	case XTYP_CONNECT:
		if (!dw2 && hsz2 == hszDDEService) {
			return (HDDEDATA) TRUE;
		}
		return FALSE;
	case XTYP_EXECUTE:
		if (hsz1 == hszDDEExecuteMacro || (
				hsz1 == hszDDECommandLine && hwndFrame != 0)) {
			if ((pszData = DdeAccessData(hdata, 0)) == 0) {
				alert("Cannot access DDE data handle, error %d", DdeGetLastError(hDDE));
			} else {
				ShowWindow(hwndFrame, SW_SHOW);
				if (hsz1 == hszDDECommandLine) {
					GetPhase2Args(pszData);
				} else {
					macs_execute_string(pszData);
				}
				DdeUnaccessData(hdata);
				if (!DdeFreeDataHandle(hdata)) {
					alert("Error freeing DDE data handle");
				}
			}
			return (HDDEDATA) DDE_FACK;
		}
		return DDE_FNOTPROCESSED;
	}
	return 0;
}

static int InitDDE(void) {
	UINT	result;

	if (hDDE) {
		return 1;
	}
	result = DdeInitialize(&hDDE, EdDDECallback, 
		APPCLASS_STANDARD | 
		CBF_FAIL_SELFCONNECTIONS |
		CBF_SKIP_DISCONNECTS |
		CBF_SKIP_REGISTRATIONS, 
		0);
	if (result != DMLERR_NO_ERROR) {
		alert("Got error %d initializing DDE");
		hDDE = 0;
	} else {
		hszDDECommandLine = DdeCreateStringHandle(hDDE, "commandline", CP_WINANSI);
		hszDDEService = DdeCreateStringHandle(hDDE, "pksedit", CP_WINANSI);
		hszDDEExecuteMacro = DdeCreateStringHandle(hDDE, "macro", CP_WINANSI);
	}
	return 1;
}

static void UnInitDDE(void) {
	if (hDDE) {
		if (hDDEService) {
			DdeNameService(hDDE, hszDDEService, 0, DNS_UNREGISTER);
			hDDEService = 0;
		}
		DdeFreeStringHandle(hDDE, hszDDECommandLine);
		DdeFreeStringHandle(hDDE, hszDDEExecuteMacro);
		DdeFreeStringHandle(hDDE, hszDDEService);
		DdeUninitialize(hDDE);
		hDDE = 0;
	}
}

#if 0
static int DelegateArguments(LPSTR lpCmdLine) {
	HCONV		hconv;
	HDDEDATA	hData;
	if ((hconv = DdeConnect(hDDE, hszDDEService, hszDDECommandLine, 0)) != 0) {
		hData = DdeCreateDataHandle(hDDE, lpCmdLine, 256, 0, 0, CF_TEXT, 0);
		DdeClientTransaction(hData, 0xFFFFFFFF, hconv, 0, 0, XTYP_EXECUTE, 5000, 0);
	} else {
		return 0;
	}
	DdeDisconnect(hconv);
	return 1;
}
#endif
static void RegisterServerDDE(void) {
	hDDEService = DdeNameService(hDDE, hszDDEService, 0, DNS_REGISTER|DNS_FILTERON);
}


/*------------------------------------------------------------
 * WinMain()
 */
int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG msg;

	hInst = hInstance;
	if (!InitBuffers()) {
		return FALSE;
	}
	if (!InitEnv()) {	/* need environment for sizing the frame window... */
		return FALSE;
	}
	if (!InitDDE()) {
		return FALSE;
	}
	nInstanceCount = 1;
	if (FindWindow(szFrameClass, 0) != 0) {
		nInstanceCount = 2;
	}
	RegisterServerDDE();
	if (!hPrevInstance) {
		if (!InitApplication())
		    return (FALSE);
	}
	if (!InitInstance(nCmdShow, lpCmdLine)) {
		return (FALSE);
	}
	ic_init();
	GetPhase2Args(lpCmdLine);
	pickread();
	/* show client window now! */
	ShowWindow(hwndClient,SW_SHOW);

	if (!ww_nwi() && _runInteractive) {
		EdEditFile(0L,0L,(char*)0);
	}
	while (GetMessage(&msg, 0, 0, 0)) {
		if ((!_translatekeys || !TranslatePksAccel(hwndFrame,&msg)) &&
		    !TranslateMDISysAccel(hwndClient,&msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg); 
		}
	}

	return (msg.wParam);
}

/*-----------------------------------------------------------
 * EdEnumChildWindows()
 */
int EdEnumChildWindows(int (*funcp)(),LONG lParam)
{    int ret;
	HWND hwndT,hwndNext;

    	for ( hwndT = GetWindow (hwndClient, GW_CHILD); hwndT;
	  	 hwndT = hwndNext) {

	  	hwndNext = hwndT;
	  	do {
			hwndNext = GetWindow (hwndNext, GW_HWNDNEXT);
		} while (hwndNext && GetWindow(hwndNext,GW_OWNER));

		if(!GetWindow(hwndT, GW_OWNER)) {
	     	if ((ret = (*funcp)(hwndT,lParam)) == 0)
	     		break;
	     }
 	}
 
	return ret;
}

/*------------------------------------------------------------
 * EdGetActiveWindow()
 */
HWND EdGetActiveWindow(int includeicons)
{
	HWND hwndChild;
	WORD isWindow;

	hwndChild = (HWND) SendMessage(hwndClient,WM_MDIGETACTIVE, 0, (LPARAM)&isWindow);
	if (!hwndChild || (!includeicons && !isWindow)) {
		return 0;
	}
	return hwndChild;
}

/*------------------------------------------------------------
 * EdCloseAll()
 */
int EdCloseAll(int ic_flag)
{
	ShowWindow(hwndClient,SW_HIDE);
	EdEnumChildWindows(CloseEditChild,0);

	if (ww_nwi() == 0 && ic_flag) {
		EdEnumChildWindows(CloseAllChild,0);
	}

	ShowWindow(hwndClient,SW_SHOW);

	// no exit: still windows alive
	if (ww_nwi() != 0)
		return 0;
	return 1;
}

/*------------------------------------------------------------
 * EdArrangeWin()
 */
int EdArrangeWin(WORD style)
{
    return SendMessage(hwndClient, style, 0, 0L);
}

/*------------------------------------------------------------
 * GetEditMenuText()
 */
void GetEditMenuText(int menunr, char *text)
{
	HMENU	hCurrentMenu;

	hCurrentMenu = GetMenu(hwndFrame);
	GetMenuString(hCurrentMenu, menunr, text, 64, MF_BYCOMMAND);
}
 
/*------------------------------------------------------------
 * GetStdMenuText()
 */
void GetStdMenuText(int menunr, char *text)
{
	GetMenuString(hDefaultMenu, menunr, text, 64, MF_BYCOMMAND);
}
 
/*------------------------------------------------------------
 * SetEditMenuText()
 */
void SetEditMenuText(int menunr, char *text)
{
	HMENU	hCurrentMenu;

	hCurrentMenu = GetMenu(hwndFrame);
	ModifyMenu(hCurrentMenu, menunr, MF_BYCOMMAND|MF_STRING, menunr, text );
}

/*------------------------------------------------------------
 * HiliteEditMenu()
 */
void HiliteEditMenu(int menunr, int hilited)
{
#if 0
	HiliteMenuItem(hwndClient,hDefaultMenu, menunr, hilited ? 
			    MF_HILITE : MF_UNHILITE);
#endif
}

/*--------------------------------------------------------------------------
 * GetHistoryMenu()
 */
HWND GetHistoryMenu(int *pnPosition, int *piCmd)
{
	HMENU	hMenu;
	int		i;

	*pnPosition = 14;
	*piCmd = IDM_HISTORY;
	hMenu = GetMenu(hwndFrame);
	if (hMenu == hDefaultMenu) {
		return GetSubMenu(hDefaultMenu, 0);
	}
	hMenu = GetSubMenu(hMenu, 0);
	if (!hMenu) {
		return 0;
	}
	for (i = *pnPosition = GetMenuItemCount(hMenu); --i >= 0; ) {
		if (GetMenuItemID(hMenu, i) == (unsigned int)*piCmd) {
			*pnPosition = i - 1;
			break;
		}
	}
	return hMenu;
}

/*--------------------------------------------------------------------------
 * PksChangeMenuItem()
 */
void PksChangeMenuItem(HWND hMenu, int nPosition, int nCmd, WORD wFlags,
	LPCSTR lpszItem)
{
	if (GetMenuItemCount(hMenu) <= nPosition) {
		AppendMenu(hMenu, wFlags & ~MF_BYPOSITION, nCmd, lpszItem);
	} else {
		ModifyMenu(hMenu, nPosition, wFlags, nCmd, lpszItem);
	}
}

/*------------------------------------------------------------
 * Finit()
 */
static void Finit(void)
{
	if (_options & O_SAVESET)
		prof_save(0);
	picksave();
	mac_wantclose((_options & O_SAVESEQ) ? 0 : 1);
	TmpJunk();
	HelpQuit();
	UnInitDDE();
}

/*------------------------------------------------------------
 * FrameWndProc()
 */
WINFUNC FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CLIENTCREATESTRUCT 	clientcreate;
	WORD				fwMenu;
	RECT				rect;
	WORD				w;
	WORD				nFkeyHeight;
	WORD				nStatusHeight;
	WORD				nToolbarHeight;
	static BOOL			bHelp = FALSE;
	DWORD				dwStyle;

	switch (message) {
		
		case WM_ACTIVATEAPP:
			if ((BOOL)wParam) {
				ft_CheckForChangedFiles();
			}
			break;
		case WM_CREATE:
			clientcreate.hWindowMenu  = 0;
			clientcreate.idFirstChild = (unsigned int)-1;
			dwStyle = WS_CHILD | WS_CLIPCHILDREN ;

			if (_options & O_MDISCROLL) {
				dwStyle |= (WS_HSCROLL | WS_VSCROLL);
			}
			GetClientRect(hwnd,&rect);
			hwndClient = CreateWindow("MDICLIENT", 0, dwStyle,
				0,0,rect.right,rect.bottom,
				hwnd, (HMENU)0xCAC, hInst, (LPSTR) &clientcreate);
			return 0;

		case WM_DROPFILES:
			EditDroppedFiles((HDROP)wParam);
			return 0;

		case WM_EDWINREORG:
		case WM_MOVE:
		case WM_SIZE:
			if (!IsWindowVisible(hwnd)) {
				return;
			}
			fkey_init(hwnd);
			st_init(hwnd);
			tb_init(hwnd);
			GetClientRect(hwnd,&rect);
			tb_wh(&w, &nToolbarHeight);
			status_wh(&w, &nStatusHeight);
			fkey_wh(&w,&nFkeyHeight);
			rect.top = nToolbarHeight;
			rect.bottom -= (nFkeyHeight + nStatusHeight + nToolbarHeight);
			if (hwndToolbar) {
				ShowWindow(hwndToolbar, nToolbarHeight ? SW_SHOW : SW_HIDE);
				SendMessage(hwndToolbar, message, wParam, lParam);
			}
			if (hwndFkeys) {
				ShowWindow(hwndFkeys, TRUE);
				MoveWindow(hwndFkeys, 0, rect.bottom + rect.top,
					rect.right, nFkeyHeight, 1);
			}
			if (hwndClient) {
				MoveWindow(hwndClient, 0, rect.top, 
					rect.right, rect.bottom - 1, 1);
			}
			if (hwndStatus) {
				ShowWindow(hwndStatus, nStatusHeight ? SW_SHOW : SW_HIDE);
				MoveWindow(hwndStatus, 0, rect.bottom + rect.top + nFkeyHeight + 1,
					rect.right, nStatusHeight, 1);
			}
			return 0;

		case WM_ENTERIDLE:
			if (GetKeyState(VK_F1) & 0x8000) {
				if (wParam == MSGF_MENU) {
					bHelp = TRUE;
					PostMessage(hwnd,WM_KEYDOWN,VK_RETURN,0L);
				} else {
					dlg_help();
				}
			}
			break;

		case WM_TIMER:	
			asfiles();
			ww_timer();
			ic_selecticons();
			break;
#ifdef DEBUG
		case WM_COMPACTING:
			alert("compacting memory");
			break;
#endif

		case WM_WININICHANGE:
			InitPrinterDC();
			InitDateformats();
			break;

		case WM_MENUSELECT:
			fwMenu = GET_WM_MENUSELECT_FLAGS(wParam, lParam);
			if (!GET_WM_MENUSELECT_HMENU(wParam, lParam) && 
				(WORD)GET_WM_MENUSELECT_FLAGS(wParam, lParam) == 0xFFFF) {
			/* Menu closed */
				ShowMenuHelp(-1);
			} else if ((fwMenu & (MF_SYSMENU|MF_SEPARATOR|MF_POPUP)) == 0) {
				ShowMenuHelp((int)GET_WM_MENUSELECT_CMD(wParam, lParam));
			}
			break;

		case WM_INITMENUPOPUP:
			if (!(BOOL)HIWORD(lParam)) {
				mac_setmenukeys((HMENU) wParam);
			}
			break;

		case WM_NOTIFY:
		    switch (((LPNMHDR) lParam)->code) {
		        case TTN_NEEDTEXT: {
		            LPTOOLTIPTEXT lpttt;

		            lpttt = (LPTOOLTIPTEXT) lParam;
		            lpttt->hinst = 0;
                    lpttt->lpszText = MenuTipFor(lpttt->hdr.idFrom);
	            }
				case TBN_QUERYINSERT:
				case TBN_QUERYDELETE:
					return TRUE;
		        break;
		     }
			 break;
		case WM_COMMAND:
			if ((int)(short)GET_WM_COMMAND_ID(wParam, lParam) < 0) {
				break;
			}
			wParam = TranslateToOrigMenu(GET_WM_COMMAND_ID(wParam, lParam));
			if (bHelp) {
				bHelp = FALSE;
				return EdHelpContext(wParam);
			}
			if (do_menu(wParam)) {
				return 1;
			}
			if (!IsWindow(hwnd) || !IsWindow(hwndClient)) {
				return 0;
			}
			break;

		case WM_PKSKEY:
			CloseErrorWin();
			do_key(lParam);
			return 0;

		case WM_PKSOPTOGGLE:
			op_checktoggle(wParam);		
			return 0;

		case WM_CHAR:
			CloseErrorWin();
			do_insertc(wParam);
			return 0;

	    case WM_RENDERALLFORMATS:
	    		OpenClipboard(hwnd);
	    		EmptyClipboard();
			clp_setdata(0);
	    		CloseClipboard();
	    		return 0;
	    		
	    case WM_RENDERFORMAT:
	    		if (wParam == CF_TEXT) {
				clp_setdata(0);
			}			
	    		return 0;

	    case WM_TASKFINISHED:
	    		bTaskFinished = TRUE;
			return 0;

	    case WM_QUERYENDSESSION:
	    case WM_CLOSE:
	    		Finit();
			EdCloseAll(1);
	   		if (ww_nwi()) {
	   			return 0;
			}
			break;

	    case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefFrameProc(hwnd, hwndClient, message, wParam, lParam));
}

/*--------------------------------------------------------------------------
 * SetMenuFor()
 */
void SetMenuFor(char *pszContext)
{
	HMENU		hCurrentMenu;
	HMENU		hNew;

	if (!hwndFrame) {
		return;
	}
	hNew = menu_getmenuforcontext(pszContext);
	if (!hNew) {
		hNew = menu_getmenuforcontext("default");
	}
	if (!hNew) {
		hNew = hDefaultMenu;
	}
	hCurrentMenu = GetMenu(hwndFrame);
	if (hNew == hCurrentMenu) {
		return;
	}

	SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hNew, 0);
	DrawMenuBar(hwndFrame);
}
