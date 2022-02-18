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
#include <shlwapi.h>
#include <stdio.h>

#include "alloc.h"
#include "trace.h"
#include "winterf.h"
#include "documentmodel.h"
#include "darkmode.h"
#include "winfo.h"
#include "editorconfiguration.h"
#include "errordialogs.h"
#include "pksrc.h"
#include "grammar.h"
#include "crossreferencelinks.h"
#include "pksedit.h"
#include "edierror.h"
#include "mainframe.h"
#include "context.h"
#include "fileutil.h"
#include "pksmacro.h"
#include "stringutil.h"
#include "documenttypes.h"
#include "xdialog.h"
#include "themes.h"
#include "findandreplace.h"
#include "history.h"
#include "actions.h"
#include "codecompletion.h"
#include "markpositions.h"
#include "windowselector.h"
#include "codeanalyzer.h"
#include "evaluator.h"
#include "winutil.h"
#include "textblocks.h"
#include "menu.h"

#define	PROF_OFFSET	1

 /*
  * Destroy the macro compiler internal symbol table.
  */
extern void		faicon_destroy();
extern void		sym_destroyTable();
extern void		bindings_destroy();
/*
 * Destroy all allocated macros.
 */
extern void macro_destroy();
extern void		arguments_getPhase2(char *args);
extern void		arguments_getForPhase1(char *args);
extern void 	init_readConfigFiles(void);
extern BOOL 	init_initializeVariables(void);

extern BOOL	bTaskFinished;

HINSTANCE		hInst;
HWND			hwndMain;
static DWORD	hDDE;
static HDDEDATA	hDDEService;
static HSZ		hszDDECommandLine;
static HSZ		hszDDEExecuteMacro;
static HSZ		hszDDEService;
static int		nInstanceCount;

#define	MAXCONTEXT		3

static HMENU	hDefaultMenu;

static char   szStatusClass[] = "StatusWin";
char   szAppName[] = "PKSEDIT";

int		_runInteractive = TRUE;
int		_openIconic = FALSE;

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

static HMODULE hModuleEnglishUS;

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

	CoInitialize(NULL);
	hModuleEnglishUS = LoadLibrary("pksedit.enu.dll");

	if ( !ww_register() ||
			!mainframe_registerWinClass() ||
		 	!fkey_register() ||
		 	!cust_registerControls() ||
			!codecomplete_registerWindowClass() ||
			!windowselector_registerWindowClass()) {
	    return FALSE;
	}
	analyzer_registerDefaultAnalyzers();
	evaluator_registerDefaultEvaluators();
	return TRUE;
}

/*------------------------------------------------------------
 * ww_closeChildWindow()
 */
int ww_closeChildWindow(HWND hwndChild,int iconflag)
{	LRESULT ret;

	if (!hwndChild || !IsWindow(hwndChild))
		return 0;

	if ((ret = SendMessage(hwndChild, WM_QUERYENDSESSION,iconflag,0L)) == 1) {
		SendMessage(hwndChild, WM_CLOSE, 0, 0);
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

/*
 * Return the module instance handle from which the language resources are loaded.
 */
static HINSTANCE hLanguageInst;
HINSTANCE ui_getResourceModule() {
	return hLanguageInst;
}

/*
 * Switch to the given language. Language is specified for example as "en-US" or "de-DE". 
 */
void ui_switchToLanguage(char* pszLanguage) {
	if (_stricmp("de-de", pszLanguage) == 0 || _stricmp("deutsch", pszLanguage) == 0) {
		hLanguageInst = hInst;
		return;
	}
	if (_stricmp("english", pszLanguage) == 0) {
		pszLanguage = "en-US";
	}
	// for now - hardcoded to English
	SetThreadUILanguage(0x00000409);
	char pszDLLName[80];
	sprintf(pszDLLName, "pksedit.%s.dll", pszLanguage);
	HMODULE hModule = LoadLibrary(pszDLLName);
	if (hModule) {
		hLanguageInst = hModule;
	} else {
		hLanguageInst = hInst;
	}
}

static void checkCommonControlLibraryVersion() {
	HMODULE hDll;
	DWORD dwMajorVersion = -1;

	hDll = LoadLibrary("COMCTL32.DLL");
	if (hDll != NULL) {
		DLLGETVERSIONPROC fn_DllGetVersion;
		DLLVERSIONINFO vi;

		fn_DllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hDll, "DllGetVersion");
		if (fn_DllGetVersion != NULL) {
			vi.cbSize = sizeof(DLLVERSIONINFO);
			fn_DllGetVersion(&vi);
			dwMajorVersion = vi.dwMajorVersion;
		}
		FreeLibrary(hDll);
	}
	EdTRACE(log_errorArgs(DEBUG_ERR, "Common Control Library version is %ld", dwMajorVersion));
}

/*------------------------------------------------------------
 * InitInstance()
 */
static BOOL InitInstance(LPSTR lpCmdLine) {
	char				szTitle[64];
	wchar_t				szwLocale[64];
	char				szLocale[64];

	darkmode_initialize();
	hLanguageInst = hInst;
	GetUserDefaultLocaleName(szwLocale, 64);
	wcstombs(szLocale, szwLocale, wcslen(szwLocale) + 1);
	ui_switchToLanguage(szLocale);
	// Initialize common controls.
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES | ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);
	string_initDateformats();
	checkCommonControlLibraryVersion();
	arguments_getForPhase1(lpCmdLine);
	init_readConfigFiles();
	bl_restorePasteBuffers();
	hDefaultMenu = menu_createMenubar();
	if (nInstanceCount > 1) {
		wsprintf(szTitle, "* PKS EDIT * (%d)", nInstanceCount);
	} else {
		lstrcpy(szTitle, "PKS EDIT");
	}
	hwndMain = mainframe_open(nInstanceCount, hDefaultMenu);
	 
	DragAcceptFiles(hwndMain, TRUE);
	while(!SetTimer(hwndMain,TIM_FRAME,TIMER_INTERVALL,NULL)) {
		error_showErrorById(IDS_MSGNOTIMER);
		return FALSE;
	}
	return TRUE;
}

/*
 * Restore the previously saved size and make PKS Edit visible. 
 */
static void main_restoreSizeAndMakeVisible() {
	WINDOWPLACEMENT 	ws;
	SESSION_DATA* pData = hist_getSessionData();
	if (_openIconic) {
		ShowWindow(hwndMain, SW_SHOWMINIMIZED);
	} else if (pData->sd_mainWindowPlacement.rcNormalPosition.bottom > pData->sd_mainWindowPlacement.rcNormalPosition.top) {
		ws = pData->sd_mainWindowPlacement;
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
		SetWindowPlacement(hwndMain, &ws);
	} else {
		ShowWindow(hwndMain, SW_SHOW);
	}
	ww_adjustWindowSizes();
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
				hsz1 == hszDDECommandLine && hwndMain != 0)) {
			if ((pszData = DdeAccessData(hdata, 0)) == 0) {
				error_displayAlertDialog("Cannot access DDE data handle, error %d", DdeGetLastError(hDDE));
			} else {
				ShowWindow(hwndMain, SW_SHOW);
				if (hsz1 == hszDDECommandLine) {
					arguments_getPhase2(pszData);
				} else {
					macro_executeSingleLineMacro(pszData, TRUE, "\"DDE Command\"");
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

static void dde_uninitialize(void) {
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

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	hInst = hInstance;
	if (!ft_initializeReadWriteBuffers()) {
		return FALSE;
	}
	if (!init_initializeVariables()) {	/* need environment for sizing the frame window... */
		return FALSE;
	}
	nInstanceCount = 1;
	if (mainframe_findExistingWindow()) {
		nInstanceCount = 2;
	}
	RegisterServerDDE();
	if (!hPrevInstance) {
		if (!InitApplication())
		    return (FALSE);
	}
	if (!InitInstance(lpCmdLine)) {
		return (FALSE);
	}

	ft_restorePreviouslyOpenedWindows();
	if (!InitDDE()) {
		return FALSE;
	}
	arguments_getPhase2(lpCmdLine);

	if (!ww_getNumberOfOpenWindows() && _runInteractive) {
		EdEditFile(0L,(char*)0);
	}
	/* show PKS Edit now! */
	main_restoreSizeAndMakeVisible();
	return mainframe_messageLoop();

}

/*------------------------------------------------------------
 * EdCloseAll()
 */
int EdCloseAll() {
	mainframe_enumChildWindows(TRUE, ww_closeEditChild,0);

	// no exit: still windows alive
	if (ww_getNumberOfOpenWindows() != 0) {
		return 0;
	}
	return 1;
}

/*------------------------------------------------------------
 * FinalizePksEdit()
 * 
 * Invoked, when PKS Edit exits to perform final tasks.
 */
void FinalizePksEdit(void)
{
	GetConfiguration()->autosaveOnExit();
	bl_autosavePasteBuffers();
	ft_saveWindowStates();
}

/*
 * Remove all data structures, clear temp files and dump memory for debugging purpose.
 */
void main_cleanup(void) {
	file_clearTempFiles();
	dde_uninitialize();
	doctypes_destroyAllDocumentTypes();
	theme_destroyAllThemeData();
	grammar_destroyAll();
	xref_destroyAllCrossReferenceLists();
	ft_destroyCaches();
	hist_destroy();
	action_destroyAll();
	bl_destroyPasteList(FALSE);
	fm_destroyAll();
	sym_destroyTable();
	faicon_destroy();
	bindings_destroy();
	macro_destroy();
	analyzer_destroyAnalyzers();
	evaluator_destroyEvaluators();
	ww_destroyAll();
	config_destroy();
}


