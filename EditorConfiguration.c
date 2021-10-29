/*
 * EditorConfiguration.c
 *
 * PROJEKT: PKS-EDIT for Windows 95
 *
 * Configuration of global options of PKS edit. These are for now 
 * saved in a file PKSEDIT.INI by default located in the PKS_SYS folder.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * created: 
 * last modified:
 * author: Tom
 * 
 */

#include <windows.h>
#include <commctrl.h>
#include <direct.h>
#include "winterf.h"
#include "pksedit.h"
#include "dial2.h"
#include "pksrc.h"
#include "xdialog.h"
#include "editorconfiguration.h"
#include "edfuncs.h"
#include "stringutil.h"
#include "themes.h"
#include "fileutil.h"
#include "mainframe.h"

extern void fkey_visibilitychanged(void);

/*
 * Autosave the editor configuration, when PKS Edit exits.
 */
static void AutosaveConfiguration() {
	EDITOR_CONFIGURATION* config = GetConfiguration();

	if (config->options & O_SAVE_SETTINGS_ON_EXIT) {
		prof_save(config, FALSE);
	}
	macro_autosaveAllBindings(config->options & O_SAVE_MACROS_ON_EXIT ? FALSE : TRUE);
}

/*
 * The default configuration.  
 */
static EDITOR_CONFIGURATION _configuration = {
	(O_UNDOENABLED | O_WARNINGS | O_ERROR_TONE | O_HIDE_BLOCK_ON_CARET_MOVE),
	(OL_OPTIONBAR | OL_SHOWSTATUS),
	1,
	3,
	-1,
	"",
	"INCLUCDE;INC",
	"default",
	"Deutsch",
	AutosaveConfiguration
};

static DIALPARS _dAutoSave[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_INT1,		sizeof _configuration.autosaveSeconds,		&_configuration.autosaveSeconds,
	IDD_OPT1,		O_DELETE_AUTOSAVE_FILES,				& _configuration.options,
	IDD_STRING1,	sizeof _configuration.pksEditTempPath,	_configuration.pksEditTempPath,
	IDD_OPT2,		O_SAVE_SETTINGS_ON_EXIT,				&_configuration.options,
	IDD_OPT3,		O_AUTO_OPEN_HISTORY,					& _configuration.options,
	IDD_OPT4,		O_SAVE_MACROS_ON_EXIT,					& _configuration.options,
	IDD_OPT5,		O_CREATE_BACKUP_IN_TEMP_PATH,			& _configuration.options,
	IDD_OPT6,		O_SAVE_CLIPBOARDS_ON_EXIT,				& _configuration.options,
	IDD_OPT7,		O_SAVE_CLIPBOARD_HISTORY,				& _configuration.options,
	// Terminate with 0
	0
};

static DIALPARS _dWarnings[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_OPT1,		O_WARNINGS,					&_configuration.options,
	IDD_OPT2,		O_ERROR_TONE,				&_configuration.options,
	IDD_OPT3,		O_ERROR_FLASH_WINDOW,		&_configuration.options,
	IDD_OPT4,		O_SHOW_MESSAGES_IN_SNACKBAR,	& _configuration.options,
	// Terminate with 0
	0
};

static DIALPARS _dLayout[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_OPT1,		OL_OPTIONBAR,		&_configuration.layoutoptions,
	IDD_OPT2,		OL_FKEYS,			&_configuration.layoutoptions,
	IDD_OPT3,		OL_SHOWSTATUS,		&_configuration.layoutoptions,
	IDD_OPT4,		OL_TOOLBAR,			&_configuration.layoutoptions,
	IDD_OPT5,		OL_COMPACT_TABS,	&_configuration.layoutoptions,
	// Terminate with 0
	0
};

static void conf_fillLocales(HWND hwnd, int nItem, void* selValue) {
	char* pEngl = "English";
	SendDlgItemMessage(hwnd, nItem, CB_RESETCONTENT, 0, 0L);
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)"Deutsch");
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)pEngl);
	char* pItem = (char*)selValue;
	if (_stricmp("en-us", pItem) == 0) {
		pItem = pEngl;
	}
	SendDlgItemMessage(hwnd, nItem, CB_SELECTSTRING, (WPARAM)0, (LPARAM)pItem);
}

static void conf_fillThemes(HWND hwnd, int nItem, void* selValue) {
	SendDlgItemMessage(hwnd, nItem, CB_RESETCONTENT, 0, 0L);
	THEME_DATA* pThemes = theme_getThemes();
	while (pThemes) {
		SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)pThemes->th_name);
		pThemes = pThemes->th_next;
	}
	char* pItem = (char*)selValue;
	SendDlgItemMessage(hwnd, nItem, CB_SELECTSTRING, (WPARAM)0, (LPARAM)pItem);
}

static DIALLIST _themelist = {
	NULL, conf_fillThemes, dlg_getListboxText,
	NULL, NULL, NULL, 0 };

static DIALLIST _localeslist = {
	NULL, conf_fillLocales, dlg_getListboxText,
	NULL, NULL, NULL, 0 };

static DIALPARS _dMisc[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_OPT1,		O_UNDOENABLED,					& _configuration.options,
	IDD_INT1,		sizeof _configuration.nundo,	&_configuration.nundo,
	IDD_INT2,		sizeof _configuration.maximumNumberOfOpenWindows,& _configuration.maximumNumberOfOpenWindows,
	IDD_OPT2,		O_HIDE_BLOCK_ON_CARET_MOVE,		& _configuration.options,
	IDD_OPT3,		O_FORMFOLLOW,					& _configuration.options,
	IDD_OPT4,		O_LOCKFILES,					& _configuration.options,
	IDD_STRINGLIST2, 0,								&_themelist,
	IDD_STRINGLIST1, 0,								&_localeslist,
	// Terminate with 0
	0
};

static DIALPARS* _paramsPerPage[] = {
	_dAutoSave,
	_dWarnings,
	_dLayout,
	_dMisc
};

static char* _tempPathSettingName = "AsPath";

/**
 * Returns a pointer to the current editor configuration. If not yet initialized it will be initialized on the fly. 
 */
EDITOR_CONFIGURATION* GetConfiguration() {
	return &_configuration;
}

/*
 * Calculates the default temp path used by PKS Edit. 
 */
static void config_getDefaultTempPath(char* pszPath) {
	string_concatPathAndFilename(pszPath, file_getTempDirectory(), "pksedit");
}

/*
 * Returns the temp path into which PKS edit saves autosave and optionally backup files. 
 */
char* config_getPKSEditTempPath() {
	if (!_configuration.pksEditTempPath[0]) {
		prof_getPksStandardString(_tempPathSettingName, _configuration.pksEditTempPath, member_size(EDITOR_CONFIGURATION, pksEditTempPath) - 1);
		if (!_configuration.pksEditTempPath[0]) {
			config_getDefaultTempPath(_configuration.pksEditTempPath);
		}
	}
	if (file_exists(_configuration.pksEditTempPath) != 0) {
		_mkdir(_configuration.pksEditTempPath);
	}
	return _configuration.pksEditTempPath;
}

/*--------------------------------------------------------------------------
 * config_saveTempPath()
 * Save the temp path of PKS editor to the pksedit.ini file.
 */
void config_saveTempPath() {
	if (_configuration.pksEditTempPath[0]) {
		char szDefault[EDMAXPATHLEN];
		config_getDefaultTempPath(szDefault);
		if (strcmp(szDefault, _configuration.pksEditTempPath) != 0) {
			prof_savePksStandardString(_tempPathSettingName, _configuration.pksEditTempPath);
			return;
		}
	}
	prof_savePksStandardString(_tempPathSettingName, NULL);
}


static DIALPARS* _getDialogParsForPage(int pageIndex) {
	return _paramsPerPage[pageIndex];
}

void EdOptionSet(void) {
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;
	INT_PTR tempRet;

	_themelist.li_param = (long long*)GetConfiguration()->themeName;
	_localeslist.li_param = (long long*)GetConfiguration()->language;
	dlg_setXDialogParams(_getDialogParsForPage, TRUE);
	memset(&psh, 0, sizeof psh);
	memset(psp, 0, sizeof psp);

	psp[0].dwSize = sizeof(psp[0]);
    psp[0].hInstance = ui_getResourceModule();
    psp[0].pszTemplate = MAKEINTRESOURCE(DLG_OPT_AUTOSAVE);
    psp[0].pfnDlgProc = dlg_standardDialogProcedure;

    psp[1].dwSize = sizeof(psp[1]);
    psp[1].hInstance = ui_getResourceModule();
    psp[1].pszTemplate = MAKEINTRESOURCE(DLG_OPT_WARNINGS);
    psp[1].pfnDlgProc = dlg_standardDialogProcedure;

	psp[2].dwSize = sizeof(psp[2]);
	psp[2].hInstance = ui_getResourceModule();
	psp[2].pszTemplate = MAKEINTRESOURCE(DLG_OPT_LAYOUT);
	psp[2].pfnDlgProc = dlg_standardDialogProcedure;

	psp[3].dwSize = sizeof(psp[3]);
	psp[3].hInstance = ui_getResourceModule();
	psp[3].pszTemplate = MAKEINTRESOURCE(DLG_OPT_MISC);
	psp[3].pfnDlgProc = dlg_standardDialogProcedure;

	psh.dwSize = sizeof(psh);
    psh.dwFlags = PSH_PROPSHEETPAGE|PSH_USECALLBACK;
    psh.hwndParent = hwndMain;
    psh.hInstance = ui_getResourceModule();
    psh.pszIcon = 0;
    psh.pszCaption = (LPSTR) "Options...";
    psh.nPages = sizeof(psp) / sizeof(psp[0]);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = (PFNPROPSHEETCALLBACK)dlg_propertySheetCallback;

	tempRet = PropertySheet(&psh);
    if (tempRet == 1) {
		EDITOR_CONFIGURATION* pConfig = GetConfiguration();
		theme_setCurrent(pConfig->themeName);
		ui_switchToLanguage(pConfig->language);
		prof_save(pConfig, FALSE);
		fkey_visibilitychanged();
		mainframe_windowTitleChanged();
	}
}

