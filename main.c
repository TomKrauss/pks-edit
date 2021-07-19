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

#include "customcontrols.h"
#include <windowsx.h>
#include <commctrl.h>
#include <ddeml.h>
#include <shellapi.h>

#include "alloc.h"
#include "trace.h"
#include "winterf.h"
#include "lineoperations.h"
#include "winfo.h"
#include "editorconfiguration.h"
#include "errordialogs.h"
#include "pksrc.h"
#include "grammar.h"
#include "crossreferencelinks.h"
#include "pksedit.h"
#include "edierror.h"
#include "context.h"
#include "desktopicons.h"
#include "fileutil.h"
#include "edfuncs.h"
#include "stringutil.h"
#include "documenttypes.h"
#include "xdialog.h"
#include "themes.h"
#include "findandreplace.h"
#include "history.h"
#include "actions.h"

#define	PROF_OFFSET	1

 /*------------------------------------------------------------
  * print_initPrinterDC()
  */
extern void		bl_destroyPasteList();
extern void		print_initPrinterDC(void);
extern void		GetPhase2Args(char *args);
extern void		GetPhase1Args(char *args);
extern void 	EditDroppedFiles(HDROP hdrop);
extern BOOL 	ww_workWinHasFocus(void);
extern void 	st_init(HWND hwndDaddy);
extern void		st_redraw(BOOL bErase);
extern void 	status_wh(WORD *width, WORD *height);
extern void 	tb_wh(WORD *width, WORD *height);
extern void 	init_readConfigFiles(void);
extern int 		clp_setdata(int whichBuffer);
extern HMENU 	menu_getMenuForContext(char *pszContext);
extern BOOL 	init_initializeVariables(void);
extern void		help_quitHelpSystem(void);

extern BOOL	bTaskFinished;

HINSTANCE		hInst;
HWND   			hwndMDIFrameWindow;
HWND			hwndMDIClientWindow;
static DWORD	hDDE;
static HDDEDATA	hDDEService;
static HSZ		hszDDECommandLine;
static HSZ		hszDDEExecuteMacro;
static HSZ		hszDDEService;
static int		nInstanceCount;

#define	MAXCONTEXT		3
#define	TIMER_INTERVALL	1000

static HMENU	hDefaultMenu;

char   szFrameClass[] = "DeskWin";
char   szEditClass[] = "EditWin";
char   szStatusClass[] = "StatusWin";
char   szRulerClass[] = "RulerWin";
char   szWorkAreaClass[] = "WorkWin";
char   szLineNumbersClass[] = "LineNumbers";
char   szAppName[] = "PKSEDIT";

int		_runInteractive = TRUE;
int		_openIconic = FALSE;

/*------------------------------------------------------------
 * win_createMdiChildWindow()
 */
HWND win_createMdiChildWindow(char *szClass, char *fn, int itemno, LPARAM lParam, WINDOWPLACEMENT *wsp)
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
	hwndChild = (HWND) SendMessage(hwndMDIClientWindow, WM_MDICREATE, 0,
		PtrToLong((LPMDICREATESTRUCT)&mdicreate));

	if (hwndChild && ws.length) {
		SetWindowPlacement(hwndChild, wsp);
	} else {
		ShowWindow(hwndChild, SW_SHOW);
	}
	return hwndChild;
}

/*------------------------------------------------------------
 * win_registerWindowClass()
 */
int win_registerWindowClass(
	char   *szClassName,
	WNDPROC WinProc,
	LPSTR  lpCursorName,
	HANDLE hbrBackground,
	LPSTR  szIcon,
	int    nExtra
)
{
	WNDCLASS  wc;
	
	cust_initializeWindowClassDefaults(&wc);
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
		error_displayAlertDialog("REAL MODE not supported");
		return FALSE;
	}
#endif

	if ( !win_registerWindowClass(szFrameClass,FrameWndProc,
			    NULL,GetSysColorBrush(COLOR_3DFACE),"APP_ICON", 0) ||
			!ic_registerDesktopIconClass() ||
		 	!ww_register() ||
		 	!fkey_register() ||
		 	!cust_registerControls()) {
	    return FALSE;
	}
	return TRUE;
}

/*------------------------------------------------------------
 * ww_closeAllChildrenOfWindow()
 */
int ww_closeAllChildrenOfWindow(HWND hwndChild)
{
	if (!hwndChild || !IsWindow(hwndChild))
		return 0;

	SendMessage(hwndMDIClientWindow, WM_MDIDESTROY, (WPARAM)hwndChild, (LPARAM)0L);
	return 1;
}

/*------------------------------------------------------------
 * ww_closeChildWindow()
 */
int ww_closeChildWindow(HWND hwndChild,int iconflag)
{	LRESULT ret;

	if (!hwndChild || !IsWindow(hwndChild))
		return 0;

	if ((ret = SendMessage(hwndChild, WM_QUERYENDSESSION,iconflag,0L)) == 1) {
		SendMessage(hwndMDIClientWindow, WM_MDIDESTROY, (WPARAM) hwndChild, (LPARAM)0L);
		return 1;
	}

	return (ret) ? 1 : 0;
}

/*------------------------------------------------------------
 * ww_closeEditChild()
 */
int ww_closeEditChild(HWND hwndChild)
{
	return ww_closeChildWindow(hwndChild,0);
}

/*------------------------------------------------------------
 * InitInstance()
 */
static BOOL InitInstance(int nCmdShow, LPSTR lpCmdLine)
{
	DWORD				dwStyle;
	WINDOWPLACEMENT 	ws;
	char				szTitle[64];

	print_initPrinterDC();
	string_initDateformats();
	GetPhase1Args(lpCmdLine);
	init_readConfigFiles();
	hDefaultMenu = LoadMenu(hInst, "PksEdEditMenu");
	if (nInstanceCount > 1) {
		wsprintf(szTitle, "* PKS EDIT * (%d)", nInstanceCount);
	} else {
		lstrcpy(szTitle, "PKS EDIT");
	}

	dwStyle = WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN;
	hwndMDIFrameWindow = CreateWindow(szFrameClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, 0, hDefaultMenu, hInst, 0);

	if (!hwndMDIFrameWindow) {
		return FALSE;
	}
	DragAcceptFiles(hwndMDIFrameWindow, TRUE);
	while(!SetTimer(hwndMDIFrameWindow,TIM_FRAME,TIMER_INTERVALL,NULL)) {
		error_showErrorById(IDS_MSGNOTIMER);
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
		SetWindowPlacement(hwndMDIFrameWindow, &ws);
	}
	ShowWindow(hwndMDIFrameWindow, nCmdShow);
	PostMessage(hwndMDIFrameWindow, WM_EDWINREORG, 0, 0L);
	return TRUE;
}

/*------------------------------------------------------------
 * TranslatePksAccel()
 */
int _translatekeys = 1;
static void* _executeKeyBinding;
static int TranslatePksAccel(HWND hwnd, MSG *msg)
{

	switch(msg->message) {
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYUP:
		case WM_KEYDOWN:
			if (msg->wParam == VK_MENU ||
			    msg->wParam == VK_CONTROL ||
			    msg->wParam == VK_SHIFT)
		    		break;
			if (!ww_workWinHasFocus()) {
				if (!(msg->hwnd == hwndMDIClientWindow || GetKeyState(VK_CONTROL) < 0)) {
					break;
				}
			}
			if ((_executeKeyBinding = macro_getKeyBinding(msg->wParam)) != 0) {
				if (msg->message == WM_SYSKEYDOWN ||
				    msg->message == WM_KEYDOWN) {
					msg->message = WM_PKSKEY;
					msg->hwnd = hwnd;
					break;
				}
				return 1;
			}
	}
	return 0;
}

static HDDEDATA CALLBACK EdDDECallback(UINT uType, UINT uFmt, HCONV hconv,
		HSZ hsz1, HSZ hsz2, HDDEDATA hdata, ULONG_PTR dw1, ULONG_PTR dw2) {
	char * pszData;

	switch (uType) {
	case XTYP_CONNECT:
		if (!dw2 && hsz2 == hszDDEService) {
			return (HDDEDATA) TRUE;
		}
		return FALSE;
	case XTYP_EXECUTE:
		if (hsz1 == hszDDEExecuteMacro || (
				hsz1 == hszDDECommandLine && hwndMDIFrameWindow != 0)) {
			if ((pszData = DdeAccessData(hdata, 0)) == 0) {
				error_displayAlertDialog("Cannot access DDE data handle, error %d", DdeGetLastError(hDDE));
			} else {
				ShowWindow(hwndMDIFrameWindow, SW_SHOW);
				if (hsz1 == hszDDECommandLine) {
					GetPhase2Args(pszData);
				} else {
					macro_executeSingleLineMacro(pszData);
				}
				DdeUnaccessData(hdata);
				if (!DdeFreeDataHandle(hdata)) {
					error_displayAlertDialog("Error freeing DDE data handle");
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
		error_displayAlertDialog("Got error %d initializing DDE");
		hDDE = 0;
	} else {
		hszDDECommandLine = DdeCreateStringHandle(hDDE, "commandline", CP_WINANSI);
		hszDDEService = DdeCreateStringHandle(hDDE, "PKSEdit.1", CP_WINANSI);
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
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	MSG msg;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	hInst = hInstance;
	if (!ft_initializeReadWriteBuffers()) {
		return FALSE;
	}
	if (!init_initializeVariables()) {	/* need environment for sizing the frame window... */
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
	// Initialize common controls.
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES | ICC_TAB_CLASSES;
	InitCommonControlsEx(&icex);

	ic_init();
	GetPhase2Args(lpCmdLine);
	ft_restorePreviouslyOpenedWindows();
	/* show client window now! */
	ShowWindow(hwndMDIClientWindow,SW_SHOW);

	if (!ww_getNumberOfOpenWindows() && _runInteractive) {
		EdEditFile(0L,(char*)0);
	}
	while (GetMessage(&msg, 0, 0, 0)) {
		if ((!_translatekeys || !TranslatePksAccel(hwndMDIFrameWindow,&msg)) &&
		    !TranslateMDISysAccel(hwndMDIClientWindow,&msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg); 
		}
	}

	return (int)(msg.wParam);
}

/*-----------------------------------------------------------
 * EdEnumChildWindows()
 */
int EdEnumChildWindows(int (*funcp)(),LONG lParam)
{    int ret;
	HWND hwndT,hwndNext;

    	for ( hwndT = GetWindow (hwndMDIClientWindow, GW_CHILD); hwndT;
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

	hwndChild = (HWND) SendMessage(hwndMDIClientWindow,WM_MDIGETACTIVE, 0, (LPARAM)&isWindow);
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
	ShowWindow(hwndMDIClientWindow,SW_HIDE);
	EdEnumChildWindows(ww_closeEditChild,0);

	if (ww_getNumberOfOpenWindows() == 0 && ic_flag) {
		EdEnumChildWindows(ww_closeAllChildrenOfWindow,0);
	}

	ShowWindow(hwndMDIClientWindow,SW_SHOW);

	// no exit: still windows alive
	if (ww_getNumberOfOpenWindows() != 0)
		return 0;
	return 1;
}

/*------------------------------------------------------------
 * EdArrangeWin()
 */
int EdArrangeWin(WORD style)
{
    return (int)SendMessage(hwndMDIClientWindow, style, 0, 0L);
}

/*------------------------------------------------------------
 * win_getStdMenuText()
 */
void win_getStdMenuText(int menunr, char *text)
{
	GetMenuString(hDefaultMenu, menunr, text, 64, MF_BYCOMMAND);
}
 
/*------------------------------------------------------------
 * win_setEditMenuText()
 */
void win_setEditMenuText(int menunr, char *text)
{
	HMENU	hCurrentMenu;

	hCurrentMenu = GetMenu(hwndMDIFrameWindow);
	ModifyMenu(hCurrentMenu, menunr, MF_BYCOMMAND|MF_STRING, menunr, text );
}

/*--------------------------------------------------------------------------
 * his_getHistoryMenu()
 */
HMENU his_getHistoryMenu(int *pnPosition, int *piCmd)
{
	HMENU	hMenu;
	int		i;
	static int fileMenuItemCount;

	*piCmd = IDM_HISTORY;
	hMenu = GetMenu(hwndMDIFrameWindow);
	// Access the file menu.
	hMenu = GetSubMenu(hMenu, 0);
	if (fileMenuItemCount == 0) {
		fileMenuItemCount = GetMenuItemCount(hMenu);
	}
	*pnPosition = fileMenuItemCount;
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
 * win_changeMenuItem()
 */
void win_changeMenuItem(HMENU hMenu, int nPosition, int nCmd, WORD wFlags,
	LPCSTR lpszItem)
{
	if (GetMenuItemCount(hMenu) <= nPosition) {
		AppendMenu(hMenu, wFlags & ~MF_BYPOSITION, nCmd, lpszItem);
	} else {
		ModifyMenu(hMenu, nPosition, wFlags, nCmd, lpszItem);
	}
}

/*------------------------------------------------------------
 * FinalizePksEdit()
 * 
 * Invoked, when PKS Edit exits to perform final tasks.
 */
static void FinalizePksEdit(void)
{
	GetConfiguration()->autosaveOnExit();
	ft_saveWindowStates();
}

/*
 * Remove all data structures, clear temp files and dump memory for debugging purpose.
 */
static void main_cleanup(void) {
	file_clearTempFiles();
	help_quitHelpSystem();
	UnInitDDE();
	doctypes_destroyAllDocumentTypes();
	theme_destroyAllThemeData();
	grammar_destroyAll();
	xref_destroyAllCrossReferenceLists();
	ft_destroyCaches();
	hist_destroy();
	action_destroyAll();
	bl_destroyPasteList();
	ic_destroyClasses();
}

/*------------------------------------------------------------
 * ww_onTimerAction()
 * Triggered, when the timer of the frame window "hits".
 */
static void ww_onTimerAction(void) {
	st_redraw(FALSE);
}

/*------------------------------------------------------------
 * FrameWndProc()
 */
static BOOL appActivated = FALSE;
LRESULT FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL			bHelp = FALSE;
	CLIENTCREATESTRUCT 	clientcreate;
	WORD				fwMenu;
	RECT				rect;
	WORD				w;
	WORD				nFkeyHeight;
	WORD				nStatusHeight;
	WORD				nToolbarHeight;
	int					idCtrl;
	DWORD				dwStyle;

	switch (message) {
		
		case WM_ACTIVATEAPP:
			if ((BOOL)wParam) {
				appActivated = TRUE;
			}
			break;
		case WM_CREATE:
			clientcreate.hWindowMenu  = 0;
			clientcreate.idFirstChild = (unsigned int)-1;
			dwStyle = WS_CHILD | WS_CLIPCHILDREN;

			if (GetConfiguration()->options & O_MDISCROLL) {
				dwStyle |= (WS_HSCROLL | WS_VSCROLL);
			}
			GetClientRect(hwnd,&rect);
			hwndMDIClientWindow = CreateWindow("MDICLIENT", 0, dwStyle,
				0,0,rect.right,rect.bottom,
				hwnd, (HMENU)0xCAC, hInst, (LPSTR) &clientcreate);
			fkey_initKeyboardWidget(hwnd);
			st_init(hwnd);
			tb_initRebar(hwnd);
			return 0;

		case WM_DROPFILES:
			EditDroppedFiles((HDROP)wParam);
			return 0;

		case WM_EDWINREORG:
		case WM_MOVE:
		case WM_SIZE:
			if (!IsWindowVisible(hwnd)) {
				return 0;
			}
			GetClientRect(hwnd,&rect);
			tb_wh(&w, &nToolbarHeight);
			status_wh(&w, &nStatusHeight);
			fkey_getKeyboardSize(&w,&nFkeyHeight);
			rect.top = nToolbarHeight;
			rect.bottom -= (nFkeyHeight + nStatusHeight + nToolbarHeight);
			if (hwndRebar && message == WM_SIZE) {
				ShowWindow(hwndRebar, nToolbarHeight ? SW_SHOW : SW_HIDE);
				SendMessage(hwndRebar, message, wParam, lParam);
			}
			if (hwndFkeys && message == WM_SIZE) {
				ShowWindow(hwndFkeys, TRUE);
				MoveWindow(hwndFkeys, 0, rect.bottom + rect.top,
					rect.right, nFkeyHeight, 1);
			}
			if (hwndMDIClientWindow && message == WM_SIZE) {
				MoveWindow(hwndMDIClientWindow, 0, rect.top, 
					rect.right, rect.bottom - 1, 1);
			}
			if (hwndStatus && message == WM_SIZE) {
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
			if (appActivated) {
				appActivated = FALSE;
				ft_checkForChangedFiles();
			}
			ft_triggerAutosaveAllFiles();
			ww_onTimerAction();
			ic_redisplayIcons();
			break;
#ifdef DEBUG
		case WM_COMPACTING:
			error_displayAlertDialog("compacting memory");
			break;
#endif

		case WM_WININICHANGE:
			print_initPrinterDC();
			string_initDateformats();
			break;

		case WM_MENUSELECT:
			fwMenu = GET_WM_MENUSELECT_FLAGS(wParam, lParam);
			if (!GET_WM_MENUSELECT_HMENU(wParam, lParam) && 
				(WORD)GET_WM_MENUSELECT_FLAGS(wParam, lParam) == 0xFFFF) {
			/* Menu closed */
				macro_showHelpForMenu(-1);
			} else if ((fwMenu & (MF_SYSMENU|MF_SEPARATOR|MF_POPUP)) == 0) {
				macro_showHelpForMenu((int)GET_WM_MENUSELECT_CMD(wParam, lParam));
			}
			break;

		case WM_INITMENUPOPUP:
			if (!(BOOL)HIWORD(lParam)) {
				macro_assignAcceleratorTextOnMenu((HMENU) wParam);
			}
			break;

		case WM_NOTIFY:
		    switch (((LPNMHDR) lParam)->code) {
		        case TTN_NEEDTEXT: {
		            LPTOOLTIPTEXT lpttt;

		            lpttt = (LPTOOLTIPTEXT) lParam;
		            lpttt->hinst = 0;
                    lpttt->lpszText = macro_getMenuTooltTip((int)lpttt->hdr.idFrom);
	            }
				case TBN_QUERYINSERT:
				case TBN_QUERYDELETE:
					return TRUE;
		        break;
		     }
			 break;
		case WM_COMMAND:
			idCtrl = (int)(short)GET_WM_COMMAND_ID(wParam, lParam);
			if (idCtrl < 0) {
				break;
			}
			if (idCtrl == IDM_INCREMENTAL_SEARCH) {
				break;
			}
			wParam = macro_translateToOriginalMenuIndex(idCtrl);
			if (bHelp) {
				bHelp = FALSE;
				return EdHelpContext((DWORD)wParam);
			}
			if (macro_onMenuAction((int)wParam)) {
				return 1;
			}
			if (!IsWindow(hwnd) || !IsWindow(hwndMDIClientWindow)) {
				return 0;
			}
			break;

		case WM_PKSKEY:
			error_closeErrorWindow();
			macro_onKeyPressed(_executeKeyBinding);
			return 0;

		case WM_PKSOPTOGGLE:
			op_onOptionWidgetSelected((int)wParam);		
			return 0;

		case WM_CHAR:
			error_closeErrorWindow();
			macro_onCharacterInserted((WORD) wParam);
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
	    	FinalizePksEdit();
			EdCloseAll(1);
	   		if (ww_getNumberOfOpenWindows()) {
	   			return 0;
			}
			main_cleanup();
			break;

	    case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefFrameProc(hwnd, hwndMDIClientWindow, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * menu_switchMenusToContext()
 */
void menu_switchMenusToContext(char *pszContext) {
	HMENU		hCurrentMenu;
	HMENU		hNew;

	if (!hwndMDIFrameWindow) {
		return;
	}
	hNew = menu_getMenuForContext(pszContext);
	if (!hNew) {
		hNew = menu_getMenuForContext("default");
	}
	if (!hNew) {
		hNew = hDefaultMenu;
	}
	hCurrentMenu = GetMenu(hwndMDIFrameWindow);
	if (hNew == hCurrentMenu) {
		return;
	}
	SendMessage(hwndMDIClientWindow, WM_MDISETMENU, (WPARAM)hNew, 0);
	DrawMenuBar(hwndMDIFrameWindow);
}

