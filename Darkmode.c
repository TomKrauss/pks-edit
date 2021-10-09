
// This file contains code from
// https://github.com/stevemk14ebr/PolyHook_2_0/blob/master/sources/IatHook.cpp
// which is licensed under the MIT License.
// See PolyHook_2_0-LICENSE for more information.

#pragma once
#include <SDKDDKVer.h>
#include <Windows.h>
#include <CommCtrl.h>
#include <Uxtheme.h>
#include <WindowsX.h>
#include <Vssym32.h>
#include "darkmode.h"

static void* RVA2VA(void* base, size_t rva) {
	return (void*)((ULONG_PTR)base + rva);
}

static void* DataDirectoryFromModuleBase(void* moduleBase, size_t entryID) {
	PIMAGE_DOS_HEADER dosHdr = (PIMAGE_DOS_HEADER)(moduleBase);
	PIMAGE_NT_HEADERS auto ntHdr = (PIMAGE_NT_HEADERS)RVA2VA(moduleBase, dosHdr->e_lfanew);
	IMAGE_DATA_DIRECTORY * dataDir = (IMAGE_DATA_DIRECTORY*)ntHdr->OptionalHeader.DataDirectory;
	return RVA2VA(moduleBase, dataDir[entryID].VirtualAddress);
}

static PIMAGE_THUNK_DATA FindAddressByName(void* moduleBase, PIMAGE_THUNK_DATA impName, PIMAGE_THUNK_DATA impAddr, const char* funcName) {
	for (; impName->u1.Ordinal; ++impName, ++impAddr)
	{
		if (IMAGE_SNAP_BY_ORDINAL(impName->u1.Ordinal))
			continue;

		PIMAGE_IMPORT_BY_NAME pImport = RVA2VA(moduleBase, impName->u1.AddressOfData);
		if (strcmp(pImport->Name, funcName) != 0)
			continue;
		return impAddr;
	}
	return NULL;
}

static PIMAGE_THUNK_DATA FindAddressByOrdinal(void* moduleBase, PIMAGE_THUNK_DATA impName, PIMAGE_THUNK_DATA impAddr, UINT16 ordinal)
{
	for (; impName->u1.Ordinal; ++impName, ++impAddr)
	{
		if (IMAGE_SNAP_BY_ORDINAL(impName->u1.Ordinal) && IMAGE_ORDINAL(impName->u1.Ordinal) == ordinal)
			return impAddr;
	}
	return NULL;
}

static PIMAGE_THUNK_DATA FindIatThunkInModule(void* moduleBase, const char* dllName, const char* funcName)
{
	PIMAGE_IMPORT_DESCRIPTOR imports = DataDirectoryFromModuleBase(moduleBase, IMAGE_DIRECTORY_ENTRY_IMPORT);
	for (; imports->Name; ++imports)
	{
		if (_stricmp(RVA2VA(moduleBase, imports->Name), dllName) != 0)
			continue;

		PIMAGE_THUNK_DATA origThunk = RVA2VA(moduleBase, imports->OriginalFirstThunk);
		PIMAGE_THUNK_DATA thunk = RVA2VA(moduleBase, imports->FirstThunk);
		return FindAddressByName(moduleBase, origThunk, thunk, funcName);
	}
	return NULL;
}

static PIMAGE_THUNK_DATA FindDelayLoadThunkInModule(void* moduleBase, const char* dllName, UINT16 ordinal) {
	PIMAGE_DELAYLOAD_DESCRIPTOR imports = DataDirectoryFromModuleBase(moduleBase, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
	for (; imports->DllNameRVA; ++imports)
	{
		if (_stricmp(RVA2VA(moduleBase, imports->DllNameRVA), dllName) != 0)
			continue;

		PIMAGE_THUNK_DATA impName = RVA2VA(moduleBase, imports->ImportNameTableRVA);
		PIMAGE_THUNK_DATA impAddr = RVA2VA(moduleBase, imports->ImportAddressTableRVA);
		return FindAddressByOrdinal(moduleBase, impName, impAddr, ordinal);
	}
	return NULL;
}

typedef enum  {
	IHCM_USE_CACHED_VALUE,
	IHCM_REFRESH
} IMMERSIVE_HC_CACHE_MODE;

// 1903 18362
typedef enum {
	Default, 
	AllowDark,
	ForceDark,
	ForceLight,
	Max
} PreferredAppMode;

typedef enum {
	WCA_UNDEFINED = 0,
	WCA_NCRENDERING_ENABLED = 1,
	WCA_NCRENDERING_POLICY = 2,
	WCA_TRANSITIONS_FORCEDISABLED = 3,
	WCA_ALLOW_NCPAINT = 4,
	WCA_CAPTION_BUTTON_BOUNDS = 5,
	WCA_NONCLIENT_RTL_LAYOUT = 6,
	WCA_FORCE_ICONIC_REPRESENTATION = 7,
	WCA_EXTENDED_FRAME_BOUNDS = 8,
	WCA_HAS_ICONIC_BITMAP = 9,
	WCA_THEME_ATTRIBUTES = 10,
	WCA_NCRENDERING_EXILED = 11,
	WCA_NCADORNMENTINFO = 12,
	WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
	WCA_VIDEO_OVERLAY_ACTIVE = 14,
	WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
	WCA_DISALLOW_PEEK = 16,
	WCA_CLOAK = 17,
	WCA_CLOAKED = 18,
	WCA_ACCENT_POLICY = 19,
	WCA_FREEZE_REPRESENTATION = 20,
	WCA_EVER_UNCLOAKED = 21,
	WCA_VISUAL_OWNER = 22,
	WCA_HOLOGRAPHIC = 23,
	WCA_EXCLUDED_FROM_DDA = 24,
	WCA_PASSIVEUPDATEMODE = 25,
	WCA_USEDARKMODECOLORS = 26,
	WCA_LAST = 27
} WINDOWCOMPOSITIONATTRIB;

typedef struct tagWINDOWCOMPOSITIONATTRIBDATA
{
	WINDOWCOMPOSITIONATTRIB Attrib;
	PVOID pvData;
	SIZE_T cbData;
} WINDOWCOMPOSITIONATTRIBDATA;

typedef void (WINAPI* fnRtlGetNtVersionNumbers)(LPDWORD major, LPDWORD minor, LPDWORD build);
typedef int (WINAPI* fnSetWindowCompositionAttribute)(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA* pData);
// 1809 17763
typedef int (WINAPI* fnShouldAppsUseDarkMode)(); // ordinal 132
typedef int (WINAPI* fnAllowDarkModeForWindow)(HWND hWnd, BOOL allow); // ordinal 133
typedef int (WINAPI* fnAllowDarkModeForApp)(BOOL allow); // ordinal 135, in 1809
typedef void(WINAPI* fnFlushMenuThemes)(); // ordinal 136
typedef void (WINAPI* fnRefreshImmersiveColorPolicyState)(); // ordinal 104
typedef int (WINAPI* fnIsDarkModeAllowedForWindow)(HWND hWnd); // ordinal 137
typedef int (WINAPI* fnGetIsImmersiveColorUsingHighContrast)(IMMERSIVE_HC_CACHE_MODE mode); // ordinal 106
typedef HTHEME (WINAPI* fnOpenNcThemeData)(HWND hWnd, LPCWSTR pszClassList); // ordinal 49
// 1903 18362
typedef int (WINAPI* fnShouldSystemUseDarkMode)(); // ordinal 138
typedef PreferredAppMode (WINAPI* fnSetPreferredAppMode)(PreferredAppMode appMode); // ordinal 135, in 1903
typedef int (WINAPI* fnIsDarkModeAllowedForApp)(); // ordinal 139

static fnSetWindowCompositionAttribute _SetWindowCompositionAttribute;
static fnShouldAppsUseDarkMode _ShouldAppsUseDarkMode;
static fnAllowDarkModeForWindow _AllowDarkModeForWindow;
static fnAllowDarkModeForApp _AllowDarkModeForApp;
static fnFlushMenuThemes _FlushMenuThemes;
static fnRefreshImmersiveColorPolicyState _RefreshImmersiveColorPolicyState;
static fnIsDarkModeAllowedForWindow _IsDarkModeAllowedForWindow;
static fnGetIsImmersiveColorUsingHighContrast _GetIsImmersiveColorUsingHighContrast;
static fnOpenNcThemeData _OpenNcThemeData;
// 1903 18362
static fnShouldSystemUseDarkMode _ShouldSystemUseDarkMode;
static fnSetPreferredAppMode _SetPreferredAppMode;

static BOOL g_darkModeSupported = FALSE;
static BOOL g_darkModeEnabled = FALSE;
DWORD g_buildNumber = 0;

static BOOL IsHighContrast() {
	HIGHCONTRASTW highContrast = { sizeof(highContrast) };
	if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
		return highContrast.dwFlags & HCF_HIGHCONTRASTON;
	return FALSE;
}

void darkmode_refreshTitleBarThemeColor(HWND hWnd) {
	BOOL dark = FALSE;
	if (!_IsDarkModeAllowedForWindow) {
		return;
	}
	if (_IsDarkModeAllowedForWindow(hWnd) &&
		_ShouldAppsUseDarkMode() &&
		!IsHighContrast())
	{
		dark = TRUE;
	}
	if (g_buildNumber < 18362)
		SetPropW(hWnd, L"UseImmersiveDarkModeColors", (HANDLE)(&dark));
	else if (_SetWindowCompositionAttribute)
	{
		WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };
		_SetWindowCompositionAttribute(hWnd, &data);
	}
}

/*
 * Enables dark mode in the corresponding window. 
 */
BOOL darkmode_allowForWindow(HWND hWnd, BOOL allow) {
	if (g_darkModeSupported)
		return _AllowDarkModeForWindow(hWnd, allow);
	return FALSE;
}

static BOOL IsColorSchemeChangeMessage(LPARAM lParam)
{
	BOOL is = FALSE;
	if (lParam && CompareStringOrdinal((LPCWCH)(lParam), -1, L"ImmersiveColorSet", -1, TRUE) == CSTR_EQUAL)
	{
		_RefreshImmersiveColorPolicyState();
		is = TRUE;
	}
	_GetIsImmersiveColorUsingHighContrast(IHCM_REFRESH);
	return is;
}

BOOL IsColorSchemeChangeMessage2(UINT message, LPARAM lParam) {
	if (message == WM_SETTINGCHANGE)
		return IsColorSchemeChangeMessage(lParam);
	return FALSE;
}

static void AllowDarkModeForApp(BOOL allow)
{
	if (_AllowDarkModeForApp)
		_AllowDarkModeForApp(allow);
	else if (_SetPreferredAppMode)
		_SetPreferredAppMode(allow ? AllowDark : Default);
}

static fnOpenNcThemeData MyOpenThemeData(HWND hWnd, LPCWSTR classList) {
	if (wcscmp(classList, L"ScrollBar") == 0) {
		hWnd = NULL;
		classList = L"Explorer::ScrollBar";
	}
	return _OpenNcThemeData(hWnd, classList);
}

static void FixDarkScrollBar()
{
	HMODULE hComctl = LoadLibraryExW(L"comctl32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hComctl)
	{
		PIMAGE_THUNK_DATA addr = FindDelayLoadThunkInModule(hComctl, "uxtheme.dll", 49); // OpenNcThemeData
		if (addr)
		{
			DWORD oldProtect;
			if (VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), PAGE_READWRITE, &oldProtect))
			{
				addr->u1.Function = (ULONGLONG)MyOpenThemeData;
				VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), oldProtect, &oldProtect);
			}
		}
	}
}

static BOOL darkmode_checkRequiredBuildNumber(DWORD buildNumber) {
	return (buildNumber == 17763 || // 1809
		buildNumber == 18362 || // 1903
		buildNumber == 18363 || // 1909
		buildNumber == 19041 || // 2004
		buildNumber == 19042 || // 20H2
		buildNumber == 19043 || // 21H1
		buildNumber >= 22000);  // Windows 11 insider builds
}

/*
 * Initializes Darkmode handling 
 */
void darkmode_initialize() {
	fnRtlGetNtVersionNumbers RtlGetNtVersionNumbers = (fnRtlGetNtVersionNumbers)(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
	if (RtlGetNtVersionNumbers)
	{
		DWORD major, minor;
		RtlGetNtVersionNumbers(&major, &minor, &g_buildNumber);
		g_buildNumber &= ~0xF0000000;
		if (major == 10 && minor == 0 && darkmode_checkRequiredBuildNumber(g_buildNumber))
		{
			HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
			if (hUxtheme)
			{
				_OpenNcThemeData = (fnOpenNcThemeData)(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(49)));
				_RefreshImmersiveColorPolicyState = (fnRefreshImmersiveColorPolicyState)(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(104)));
				_GetIsImmersiveColorUsingHighContrast = (fnGetIsImmersiveColorUsingHighContrast)(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(106)));
				_ShouldAppsUseDarkMode = (fnShouldAppsUseDarkMode)(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132)));
				_AllowDarkModeForWindow = (fnAllowDarkModeForWindow)(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));

				FARPROC ord135 = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
				if (g_buildNumber < 18362)
					_AllowDarkModeForApp = (fnAllowDarkModeForApp)(ord135);
				else
					_SetPreferredAppMode = (fnSetPreferredAppMode)(ord135);

				_FlushMenuThemes = (fnFlushMenuThemes)(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136)));
				_IsDarkModeAllowedForWindow = (fnIsDarkModeAllowedForWindow)(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(137)));

				_SetWindowCompositionAttribute = (fnSetWindowCompositionAttribute)(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));

				if (_OpenNcThemeData &&
					_RefreshImmersiveColorPolicyState &&
					_ShouldAppsUseDarkMode &&
					_AllowDarkModeForWindow &&
					(_AllowDarkModeForApp || _SetPreferredAppMode) &&
					_FlushMenuThemes &&
					_IsDarkModeAllowedForWindow)
				{
					g_darkModeSupported = TRUE;

					AllowDarkModeForApp(TRUE);
					_RefreshImmersiveColorPolicyState();

					g_darkModeEnabled = _ShouldAppsUseDarkMode() && !IsHighContrast();

					FixDarkScrollBar();
					_FlushMenuThemes();
				}
			}
		}
	}
}
