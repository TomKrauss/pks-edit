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
extern void		function_destroy();

/*
 * Destroy all allocated macros.
 */
extern void macro_destroy();
extern void		arguments_getPhase2(char *args);
extern void		arguments_getForPhase1(char *args);
extern void 	init_readConfigFiles(void);
extern BOOL 	init_initializeVariables(void);
extern void		types_registerDefaultTypes();

HINSTANCE		hInst;
HWND			hwndMain;
static DWORD	hDDE;
static HDDEDATA	hDDEService;
static HSZ		hszDDECommandLine;
static HSZ		hszDDEExecuteMacro;
static HSZ		hszDDEService;

#define	MAXCONTEXT		3

static HMENU	hDefaultMenu;

static char   szStatusClass[] = "StatusWin";
char   szAppName[] = "PKSEDIT";

int		_runInteractive = TRUE;
int		_openIconic = FALSE;
BOOL	_startupComplete = FALSE;

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
	types_registerDefaultTypes();
	analyzer_registerDefaultAnalyzers();
	evaluator_registerDefaultEvaluators();
	return TRUE;
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

	darkmode_initialize();
	hLanguageInst = hInst;
	if (GetConfiguration()->language[0]) {
		ui_switchToLanguage(GetConfiguration()->language);
	}
	else {
		wchar_t		szwLocale[64];
		char		szLocale[64];
		GetUserDefaultLocaleName(szwLocale, 64);
		wcstombs(szLocale, szwLocale, wcslen(szwLocale) + 1);
		ui_switchToLanguage(szLocale);
	}
	// Initialize common controls.
	INITCOMMONCONTROLSEX icex = {
		.dwSize = sizeof(INITCOMMONCONTROLSEX)
	};
	icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES | ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);
	string_initDateformats();
	checkCommonControlLibraryVersion();
	arguments_getForPhase1(lpCmdLine);
	bl_restorePasteBuffers();
	if (GetConfiguration()->themeName[0]) {
		theme_setCurrent(GetConfiguration()->themeName);
	}
	hDefaultMenu = menu_createMenubar();
	hwndMain = mainframe_open(hDefaultMenu);
	 
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

static void dde_uninitialize() {
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

#define PKS_EDIT_APPLICATION_ID "PKSEdit.1"

HANDLE pksEditMutex;

/*
 * Initialize the PKS-Edit DDE server allowing to process command line arguments.
 */
static int dde_initialize(BOOL* pDDEOtherInstanceExists) {
	UINT	result;
	
	if (hDDE) {
		return 1;
	}
	pksEditMutex = OpenMutex(
		MUTEX_ALL_ACCESS, 0, PKS_EDIT_APPLICATION_ID);
	if (pksEditMutex) {
		*pDDEOtherInstanceExists = TRUE;
		pksEditMutex = 0;
	} else {
		pksEditMutex = CreateMutex(0, 0, PKS_EDIT_APPLICATION_ID);
		*pDDEOtherInstanceExists = FALSE;
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
		hszDDEService = DdeCreateStringHandle(hDDE, PKS_EDIT_APPLICATION_ID, CP_WINANSI);
		hDDEService = DdeNameService(hDDE, hszDDEService, 0, DNS_REGISTER | DNS_FILTERON);
		hszDDECommandLine = DdeCreateStringHandle(hDDE, "commandline", CP_WINANSI);
		hszDDEExecuteMacro = DdeCreateStringHandle(hDDE, "macro", CP_WINANSI);
		if (*pDDEOtherInstanceExists) {
			HCONV hPrev = DdeConnect(hDDE, hszDDEService, hszDDECommandLine, 0);
			if (hPrev) {
				DdeNameService(hDDE, hszDDEService, 0, DNS_UNREGISTER);
				hDDEService = 0;
				*pDDEOtherInstanceExists = TRUE;
				EdTRACE(log_errorArgs(DEBUG_TRACE, "There is already a running DDE Server."));
				return 1;
			}
		}
	}
	return 1;
}

/*
 * This implements the "reuse single instance of PKS-Edit" feature. When another PKS-Edit DDE Server is active (other instance 
 * is running), pass on the command line to this instance using DDE and return 1 indicating, that the job is done and this
 * instance may terminate.
 */
static int dde_delegateArguments(LPSTR lpCmdLine) {
	HCONV hconv = 0;
	int ret = 0;
	if ((GetConfiguration()->options & O_REUSE_APPLICATION_INSTANCE)  && 
			lpCmdLine && 
			lpCmdLine[0] && 
			(hconv = DdeConnect(hDDE, hszDDEService, hszDDECommandLine, 0)) != 0) {
		char szBuf[EDMAXPATHLEN];

		strcpy(szBuf, "/p");
		if (GetCurrentDirectory(sizeof szBuf - 3, szBuf + 2) == 0) {
			return 0;
		}
		strcat(szBuf, " ");
		if (strlen(szBuf + strlen(lpCmdLine)) > sizeof szBuf) {
			return 0;
		}
		strcat(szBuf, lpCmdLine);
		HDDEDATA hReturn = DdeClientTransaction(szBuf, (DWORD)(strlen(szBuf)+1), hconv, 0, 0, XTYP_EXECUTE, 5000, 0);
		if (hReturn) {
			DdeFreeDataHandle(hReturn);
			ret = 1;
		}
	}
	if (hconv) {
		DdeDisconnect(hconv);
	}
	return ret;
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
	function_destroy();
	if (pksEditMutex) {
		ReleaseMutex(pksEditMutex);
		pksEditMutex = 0;
	}
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
	if (!hPrevInstance) {
		if (!InitApplication())
		    return (FALSE);
	}
	BOOL bDDEOtherInstanceExists = FALSE;
	init_readConfigFiles();
	if (!dde_initialize(&bDDEOtherInstanceExists)) {
		return FALSE;
	}
	if (bDDEOtherInstanceExists && dde_delegateArguments(lpCmdLine)) {
		main_cleanup();
		return FALSE;
	}
	if (!InitInstance(lpCmdLine)) {
		return (FALSE);
	}
	ft_restorePreviouslyOpenedWindows();
	arguments_getPhase2(lpCmdLine);

	if (!ww_getNumberOfOpenWindows() && _runInteractive) {
		EdEditFile(0L,(char*)0);
	}
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
	/* show PKS Edit now! */
	main_restoreSizeAndMakeVisible();
	_startupComplete = TRUE;
	return mainframe_messageLoop();

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


