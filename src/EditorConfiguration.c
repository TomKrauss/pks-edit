/*
 * EditorConfiguration.c
 *
 * Project: PKS Edit for Windows 95
 *
 * Configuration of global options of PKS edit. These are for now 
 * saved in a file pkseditini.json by default located in the PKS_SYS folder.
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

#include "alloc.h"
#include <windows.h>
#include <commctrl.h>
#include <direct.h>
#include "jsonparser.h"
#include "winterf.h"
#include "trace.h"
#include "pksedit.h"
#include "linkedlist.h"
#include "dial2.h"
#include "pksrc.h"
#include "fontawesome.h"
#include "xdialog.h"
#include "printing.h"
#include "editorconfiguration.h"
#include "pksmacro.h"
#include "stringutil.h"
#include "resource.h"
#include "errordialogs.h"
#include "themes.h"
#include "fileutil.h"
#include "mainframe.h"

extern void fkey_visibilitychanged(void);
extern void tb_updateImageList(HWND hwnd, CHAR_WITH_STYLE* tbIcons, int nCount);

static EDITOR_CONFIGURATION _configuration;

typedef struct tagSEARCH_ENGINE {
	char se_name[32];				// name of the search engine
	char se_command[128];			// The command to execute - actual word to look for in the command must be named $1
} SEARCH_ENGINE;

static SEARCH_ENGINE _searchEngines[] = {
	{"Google", "https://www.google.com/search?q=$1"},
	{"Bing", "https://www.bing.com/search?q=$1"},
	{"Yahoo", "https://search.yahoo.com/search?q=$1"},
	{"DuckDuckGo", "https://duckduckgo.com/?q=$1" },
	{"Ecosia", "https://www.ecosia.org/search?q=$1"},
	{"Ask", "https://www.ask.com/web?q=$1"},
	{"Startpage", "https://www.startpage.com/sp/search?q=$1"},
	{"Infinity Search", "https://infinitysearch.co/results?q=$1"},
	{"Aol", "https://search.aol.com/aol/search?q=$1"},
	{"excite", "https://results.excite.com/serp?q=$1"},
	{"Search", "https://www.search.com/web?q=$1"},
	{"Answers", "https://www.answers.com/search?q=$1"},
	{"Lycos", "https://search20.lycos.com/web/?q=$1"},
	{"Infospace", "https://infospace.com/serp?q=$1"},
	{"WebCrawler", "https://www.webcrawler.com/serp?q=$1"},
	{"Babylon", "http://search.babylon.com/?q=$1"},
	{"Kiddle", "https://www.kiddle.co/s.php?q=$1"},
	{"Yandex", "https://yandex.com/search/?text=$1"},
	{"Wolfram|Alpha", "https://www.wolframalpha.com/input/?i=$1"},
	0
};

static PRTPARAM _prtparams = {
	0,60, 					// options, page length in number of lines
	0,8,0,"",				// Std Font: oemmode,cheight,cwidth,name
	0,0,120,				// lmargin,rmargin, nchars
	5,{1,1},				// tabsize, line spacing
	{	"", 				// header template. A PKS Edit template (see mysprintf) optionally containing multiple segments separated by '!'
		PDT_NONE,
		{0,8,0},			// Header Font: oemmode,cheight,cwidth,name
		0, 2, 0},
	{	"%s$f - %D",		// Footer page element starting with template
		PDT_NONE,
		{0,8,0},
		0, 2, 0}
};

static JSON_MAPPING_RULE _fontRules[] = {
	{	RT_CHAR_ARRAY, "face", offsetof(FONTSPEC, fs_name), sizeof(((FONTSPEC*)NULL)->fs_name)},
	{	RT_INTEGER, "width", offsetof(FONTSPEC, fs_cwidth)},
	{	RT_INTEGER, "height", offsetof(FONTSPEC, fs_cheight)},
	{	RT_END}
};

static const char* _alignmentNames[] = {
	"left",
	"center",
	"right",
	0
};

static const char* _decorationNames[] = {
	"none",
	"line",
	"frame",
	"fill-rect",
	0
};

static const char* _iconSizeNames[] = {
	"small",
	"medium",
	"big",
	"large",
	0
};

static JSON_MAPPING_RULE _pageMarginRules[] = {
	{	RT_CHAR_ARRAY, "template", offsetof(PAGE_MARGIN_ELEMENT, pme_template), sizeof(((PAGE_MARGIN_ELEMENT*)NULL)->pme_template)},
	{	RT_INTEGER, "decoration", offsetof(PAGE_MARGIN_ELEMENT, pme_decoration)},
	{	RT_NESTED_OBJECT, "font", offsetof(PAGE_MARGIN_ELEMENT, pme_font), {.r_t_nestedObjectRules = _fontRules}},
	{	RT_ENUM, "align", offsetof(PAGE_MARGIN_ELEMENT, pme_align), {.r_t_enumNames = _alignmentNames}},
	{	RT_INTEGER, "lines", offsetof(PAGE_MARGIN_ELEMENT, pme_lines)},
	{	RT_INTEGER, "margin", offsetof(PAGE_MARGIN_ELEMENT, pme_margin)},
	{	RT_INTEGER, "spacing-numerator", offsetof(PAGE_MARGIN_ELEMENT, pme_lineSpacing.numerator)},
	{	RT_INTEGER, "spacing-denominator", offsetof(PAGE_MARGIN_ELEMENT, pme_lineSpacing.denominator)},
	{	RT_END}
};

static JSON_MAPPING_RULE _prtparamRules[] = {
	{	RT_NESTED_OBJECT, "header", offsetof(PRTPARAM, header), {.r_t_nestedObjectRules = _pageMarginRules}},
	{	RT_NESTED_OBJECT, "footer", offsetof(PRTPARAM, footer), {.r_t_nestedObjectRules = _pageMarginRules}},
	{	RT_NESTED_OBJECT, "font", offsetof(PRTPARAM, font), {.r_t_nestedObjectRules = _fontRules}},
	{	RT_INTEGER, "pagelen", offsetof(PRTPARAM, pagelen)},
	{	RT_INTEGER, "lmargin", offsetof(PRTPARAM, rmargin)},
	{	RT_INTEGER, "rmargin", offsetof(PRTPARAM, lmargin)},
	{	RT_INTEGER, "tabsize", offsetof(PRTPARAM, tabsize)},
	{	RT_INTEGER, "characters-per-line", offsetof(PRTPARAM, nchars)},
	{	RT_INTEGER, "spacing-numerator", offsetof(PRTPARAM, lnspace.numerator)},
	{	RT_INTEGER, "spacing-denominator", offsetof(PRTPARAM, lnspace.denominator)},
	{	RT_FLAG, "print-headers", offsetof(PRTPARAM, options), PRTO_HEADERS},
	{	RT_FLAG, "print-line-numbers", offsetof(PRTPARAM, options), PRTO_LINE_NUMBERS},
	{	RT_FLAG, "swap-header-alignment", offsetof(PRTPARAM, options), PRTO_SWAP_HEADER_FOOTER_ALIGNMENT},
	{	RT_FLAG, "syntax-highlighting", offsetof(PRTPARAM, options), PRTO_SYNTAX_HIGHLIGHT},
	{	RT_FLAG, "wrap", offsetof(PRTPARAM, options), PRTO_WRAP_LONG_LINES},
	{	RT_END}
};

static JSON_MAPPING_RULE _copRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(BUILD_OUTPUT_PATTERN, cop_name), sizeof(((BUILD_OUTPUT_PATTERN*)NULL)->cop_name)},
	{	RT_CHAR_ARRAY, "pattern", offsetof(BUILD_OUTPUT_PATTERN, cop_pattern), 
			sizeof(((BUILD_OUTPUT_PATTERN*)NULL)->cop_pattern)},
	{	RT_CHAR_ARRAY, "filename-pattern", offsetof(BUILD_OUTPUT_PATTERN, cop_filenamePattern), 
			sizeof(((BUILD_OUTPUT_PATTERN*)NULL)->cop_filenamePattern)},
	{	RT_INTEGER, "filename-capture", offsetof(BUILD_OUTPUT_PATTERN, cop_filenameCapture)},
	{	RT_INTEGER, "linenumber-capture", offsetof(BUILD_OUTPUT_PATTERN, cop_lineNumberCapture)},
	{	RT_INTEGER, "columnnumber-capture", offsetof(BUILD_OUTPUT_PATTERN, cop_columnNumberCapture)},
	{	RT_INTEGER, "comment-capture", offsetof(BUILD_OUTPUT_PATTERN, cop_commentCapture)},
	{	RT_END}
};

static JSON_MAPPING_RULE _editConfigurationRules[] = {
	{	RT_FLAG, "autosave-to-temp", offsetof(EDITOR_CONFIGURATION, options), O_AUTOSAVE_TO_TEMP},
	{	RT_FLAG, "autosave-on-exit", offsetof(EDITOR_CONFIGURATION, options), O_AUTOSAVE_FILES_ON_EXIT},
	{	RT_FLAG, "preserve-history", offsetof(EDITOR_CONFIGURATION, options), O_AUTO_OPEN_HISTORY},
	{	RT_FLAG, "create-back-in-temp-path", offsetof(EDITOR_CONFIGURATION, options), O_CREATE_BACKUP_IN_TEMP_PATH},
	{	RT_FLAG, "cleanup-autosave-files", offsetof(EDITOR_CONFIGURATION, options), O_DELETE_AUTOSAVE_FILES},
	{	RT_FLAG, "flash-on-error", offsetof(EDITOR_CONFIGURATION, options), O_ERROR_FLASH_WINDOW},
	{	RT_FLAG, "sound-on-error", offsetof(EDITOR_CONFIGURATION, options), O_ERROR_TONE},
	{	RT_FLAG, "forms-follow-mouse", offsetof(EDITOR_CONFIGURATION, options), O_FORMFOLLOW},
	{	RT_FLAG, "hide-selection-on-move", offsetof(EDITOR_CONFIGURATION, options), O_HIDE_BLOCK_ON_CARET_MOVE},
	{	RT_FLAG, "lock-files-for-edit", offsetof(EDITOR_CONFIGURATION, options), O_LOCKFILES},
	{	RT_FLAG, "save-clipboards-on-exit", offsetof(EDITOR_CONFIGURATION, options), O_SAVE_CLIPBOARDS_ON_EXIT},
	{	RT_FLAG, "maintain-clipboard-history", offsetof(EDITOR_CONFIGURATION, options), O_SAVE_CLIPBOARD_HISTORY},
	{	RT_FLAG, "reuse-application-running-instance", offsetof(EDITOR_CONFIGURATION, options), O_REUSE_APPLICATION_INSTANCE},
	{	RT_FLAG, "save-macros-on-exit", offsetof(EDITOR_CONFIGURATION, options), O_SAVE_MACROS_ON_EXIT},
	{	RT_FLAG, "show-error-toast", offsetof(EDITOR_CONFIGURATION, options), O_SHOW_MESSAGES_IN_SNACKBAR},
	{	RT_FLAG, "silently-reload-changed-files", offsetof(EDITOR_CONFIGURATION, options), O_SILENTLY_RELOAD_CHANGED_FILES},
	{	RT_FLAG, "undo-enabled", offsetof(EDITOR_CONFIGURATION, options), O_UNDOENABLED},
	{	RT_FLAG, "compact-editor-tabs", offsetof(EDITOR_CONFIGURATION, layoutoptions), OL_COMPACT_TABS},
	{	RT_FLAG, "show-function-keys", offsetof(EDITOR_CONFIGURATION, layoutoptions), OL_FKEYS},
	{	RT_FLAG, "show-optionbar", offsetof(EDITOR_CONFIGURATION, layoutoptions), OL_OPTIONBAR},
	{	RT_FLAG, "show-statusbar", offsetof(EDITOR_CONFIGURATION, layoutoptions), OL_SHOWSTATUS},
	{	RT_FLAG, "show-toolbar", offsetof(EDITOR_CONFIGURATION, layoutoptions), OL_TOOLBAR},
	{	RT_ENUM, "icon-size", offsetof(EDITOR_CONFIGURATION, iconSize), {.r_t_enumNames = _iconSizeNames}},
	{	RT_INTEGER, "undo-history", offsetof(EDITOR_CONFIGURATION, nundo)},
	{	RT_INTEGER, "autosave-time", offsetof(EDITOR_CONFIGURATION, autosaveSeconds)},
	{	RT_INTEGER, "maximum-open-windows", offsetof(EDITOR_CONFIGURATION, maximumNumberOfOpenWindows)},
	{	RT_CHAR_ARRAY, "pruned-search-directories", offsetof(EDITOR_CONFIGURATION, prunedSearchDirectories), sizeof(((EDITOR_CONFIGURATION*)NULL)->prunedSearchDirectories)},
	{	RT_CHAR_ARRAY, "sound-name", offsetof(EDITOR_CONFIGURATION, soundName), sizeof(((EDITOR_CONFIGURATION*)NULL)->soundName)},
	{	RT_CHAR_ARRAY, "theme", offsetof(EDITOR_CONFIGURATION, themeName), sizeof(((EDITOR_CONFIGURATION*)NULL)->themeName)},
	{	RT_CHAR_ARRAY, "temp-path", offsetof(EDITOR_CONFIGURATION, pksEditTempPath), sizeof(((EDITOR_CONFIGURATION*)NULL)->pksEditTempPath)},
	{	RT_CHAR_ARRAY, "include-path", offsetof(EDITOR_CONFIGURATION, includePath), sizeof(((EDITOR_CONFIGURATION*)NULL)->includePath)},
	{	RT_CHAR_ARRAY, "language", offsetof(EDITOR_CONFIGURATION, language), sizeof(((EDITOR_CONFIGURATION*)NULL)->language)},
	{	RT_CHAR_ARRAY, "default-font", offsetof(EDITOR_CONFIGURATION, defaultFontFace), sizeof(((EDITOR_CONFIGURATION*)NULL)->defaultFontFace)},
	{	RT_CHAR_ARRAY, "search-engine", offsetof(EDITOR_CONFIGURATION, searchEngine), sizeof(((EDITOR_CONFIGURATION*)NULL)->searchEngine)},
	{	RT_OBJECT_LIST, "compiler-output-patterns", offsetof(EDITOR_CONFIGURATION, outputPatterns),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _copRules, .ro_nestedSize = sizeof(BUILD_OUTPUT_PATTERN)}}},
	{	RT_END}
};

typedef struct tag_ALL_CONFIGS {
	EDITOR_CONFIGURATION ac_editorConfiguration;
	PRTPARAM ac_printConfiguration;
} ALL_CONFIGS;

static JSON_MAPPING_RULE _allconfigRules[] = {
	{	RT_NESTED_OBJECT, "configuration", offsetof(ALL_CONFIGS, ac_editorConfiguration), {.r_t_nestedObjectRules = _editConfigurationRules}},
	{	RT_NESTED_OBJECT, "print-configuration", offsetof(ALL_CONFIGS, ac_printConfiguration), {.r_t_nestedObjectRules = _prtparamRules}},
	{	RT_END}
};

/*
 * Save the current configuration to the specified config file.
 */
int config_save(const char* pszFilename) {
	ALL_CONFIGS config = {
		.ac_printConfiguration = _prtparams,
		.ac_editorConfiguration = _configuration
	};
	if (config_tempPathIsDefault()) {
		// Do not store the default config path to pkseditini.json. 
		config.ac_editorConfiguration.pksEditTempPath[0] = 0;
	}
	if (!json_marshal(pszFilename, &config, _allconfigRules)) {
		error_showErrorById(IDS_ERROR_SAVING_CONFIG_FILE, pszFilename);
		return 0;
	}
	return 1;
}

/*
 * Destroy all configuration to cleanup.
 */
extern void config_destroy() {
	ll_destroy(&_configuration.outputPatterns, 0);
}

/*
 * Autosave the editor configuration, when PKS Edit exits.
 */
static void config_autosaveConfiguration() {
	EDITOR_CONFIGURATION* config = GetConfiguration();

	macro_autosaveAllBindings(config->options & O_SAVE_MACROS_ON_EXIT ? FALSE : TRUE);
}

/*
 * Initialize the default options of the configuration - used when no config file can be found.
 */
void config_initialize() {
	_configuration.outputPatterns = calloc(1, sizeof(BUILD_OUTPUT_PATTERN));
	BUILD_OUTPUT_PATTERN* pBuildPattern = _configuration.outputPatterns;
	if (pBuildPattern) {
		strcpy(pBuildPattern->cop_name, "PKSMAKROC");
		pBuildPattern->cop_filenameCapture = 1;
		pBuildPattern->cop_lineNumberCapture = 2;
		pBuildPattern->cop_commentCapture = 3;
		strcpy(pBuildPattern->cop_pattern, "^[^ ]+ ([^ ]+) ([0-9]+): *(.*)");
	}
}

/*
 * Read the configuration file.
 */
int config_read(const char* pszfilename) {
	ALL_CONFIGS config;

	memset(&config, 0, sizeof config);
	if (json_parse(pszfilename, TRUE, &config, _allconfigRules)) {
		_configuration = config.ac_editorConfiguration;
		_configuration.autosaveOnExit = config_autosaveConfiguration;
		_prtparams = config.ac_printConfiguration;
		return 1;
	}
	return 0;
}

/*
 * The default configuration.  
 */
static EDITOR_CONFIGURATION _configuration = {
	.options = (O_UNDOENABLED | O_AUTOSAVE_TO_TEMP | O_ERROR_TONE | O_HIDE_BLOCK_ON_CARET_MOVE | O_AUTO_OPEN_HISTORY | O_DELETE_AUTOSAVE_FILES),
	.layoutoptions = (OL_OPTIONBAR | OL_SHOWSTATUS | OL_TOOLBAR | OL_COMPACT_TABS),
	.iconSize = ICS_SMALL,
	.nundo = 100,
	.autosaveSeconds = 10,
	.maximumNumberOfOpenWindows = -1,
	.defaultFontFace = "",
	.includePath = "INCLUCDE;INC",
	.themeName = "system default",
	.language = "Deutsch",
	.soundName = "",
	.prunedSearchDirectories = ".*;build;target",
	.searchEngine = "Google",
	.outputPatterns = 0,
	.pksEditTempPath = 0,
	.autosaveOnExit = config_autosaveConfiguration
};

static DIALOG_ITEM_DESCRIPTOR _dAutoSave[] = {
	{IDD_INT1,		sizeof _configuration.autosaveSeconds,		&_configuration.autosaveSeconds},
	{IDD_OPT1,		O_DELETE_AUTOSAVE_FILES,				&_configuration.options},
	{IDD_STRING1,	sizeof _configuration.pksEditTempPath,	_configuration.pksEditTempPath},
	{IDD_OPT3,		O_AUTO_OPEN_HISTORY,					&_configuration.options},
	{IDD_OPT4,		O_SAVE_MACROS_ON_EXIT,					&_configuration.options},
	{IDD_OPT5,		O_CREATE_BACKUP_IN_TEMP_PATH,			&_configuration.options},
	{IDD_OPT6,		O_SAVE_CLIPBOARDS_ON_EXIT,				&_configuration.options},
	{IDD_OPT7,		O_SAVE_CLIPBOARD_HISTORY,				&_configuration.options},
	{IDD_OPT8,		O_AUTOSAVE_TO_TEMP,						&_configuration.options},
	// Terminate with 0
	{0}
};

static DIALOG_ITEM_DESCRIPTOR _dWarnings[] = {
	{IDD_OPT2,		O_ERROR_TONE,				&_configuration.options},
	{IDD_OPT3,		O_ERROR_FLASH_WINDOW,		&_configuration.options},
	{IDD_OPT4,		O_SHOW_MESSAGES_IN_SNACKBAR,	&_configuration.options},
	// Terminate with 0
	{0}
};

static void conf_fillIconSizes(HWND hwnd, int nItem, void* selValue) {
	SendDlgItemMessage(hwnd, nItem, CB_RESETCONTENT, 0, 0L);
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)dlg_getResourceString(IDS_ICON_SIZE_SMALL));
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)dlg_getResourceString(IDS_ICON_SIZE_MEDIUM));
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)dlg_getResourceString(IDS_ICON_SIZE_BIG));
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)dlg_getResourceString(IDS_ICON_SIZE_LARGE));
	ICONSIZE nCurrent = *(ICONSIZE*)selValue;
	SendDlgItemMessage(hwnd, nItem, CB_SETCURSEL, (WPARAM)nCurrent, (LPARAM)0);
}

static int conf_getIconSize(HWND hwnd, int id, void* pszTemp) {
	LRESULT nItem = SendDlgItemMessage(hwnd, id, CB_GETCURSEL, 0, 0);
	if (nItem >= 0) {
		*((ICONSIZE*)pszTemp) = (int)nItem;
	}
	return nItem != CB_ERR;
}

static LIST_HANDLER _iconSizelist = {
	NULL, conf_fillIconSizes, conf_getIconSize,
	NULL, NULL, NULL, 0 };

static DIALOG_ITEM_DESCRIPTOR _dLayout[] = {
	{IDD_OPT1,		OL_OPTIONBAR,		&_configuration.layoutoptions},
	{IDD_OPT2,		OL_FKEYS,			&_configuration.layoutoptions},
	{IDD_OPT3,		OL_SHOWSTATUS,		&_configuration.layoutoptions},
	{IDD_OPT4,		OL_TOOLBAR,			&_configuration.layoutoptions},
	{IDD_OPT5,		OL_COMPACT_TABS,	&_configuration.layoutoptions},
	{IDD_ICONLIST,	0,					.did_listhandler = &_iconSizelist},
	// Terminate with 0
	{0}
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
	SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)SYSTEM_DEFAULT_THEME);
	THEME_DATA* pThemes = theme_getThemes();
	while (pThemes) {
		SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)pThemes->th_name);
		pThemes = pThemes->th_next;
	}
	char* pItem = (char*)selValue;
	SendDlgItemMessage(hwnd, nItem, CB_SELECTSTRING, (WPARAM)0, (LPARAM)pItem);
}

static void conf_fillSearchEngines(HWND hwnd, int nItem, void* selValue) {
	SendDlgItemMessage(hwnd, nItem, CB_RESETCONTENT, 0, 0L);
	SEARCH_ENGINE* pEngine = _searchEngines;
	while (pEngine->se_name[0]) {
		SendDlgItemMessage(hwnd, nItem, CB_ADDSTRING, 0, (LPARAM)pEngine->se_name);
		pEngine++;
	}
	char* pItem = (char*)selValue;
	SendDlgItemMessage(hwnd, nItem, CB_SELECTSTRING, (WPARAM)0, (LPARAM)pItem);
}

static LIST_HANDLER _themelist = {
	NULL, conf_fillThemes, dlg_getComboBoxSelectedText,
	NULL, NULL, NULL, 0 };

static LIST_HANDLER _searchEnginelist = {
	NULL, conf_fillSearchEngines, dlg_getComboBoxSelectedText,
	NULL, NULL, NULL, 0 };

static LIST_HANDLER _localeslist = {
	NULL, conf_fillLocales, dlg_getComboBoxSelectedText,
	NULL, NULL, NULL, 0 };

static DIALOG_ITEM_DESCRIPTOR _dMisc[] = {
	{IDD_OPT1,		O_UNDOENABLED,					&_configuration.options},
	{IDD_INT1,		sizeof _configuration.nundo,	&_configuration.nundo},
	{IDD_INT2,		sizeof _configuration.maximumNumberOfOpenWindows,&_configuration.maximumNumberOfOpenWindows},
	{IDD_OPT2,		O_HIDE_BLOCK_ON_CARET_MOVE,		&_configuration.options},
	{IDD_OPT3,		O_FORMFOLLOW,					&_configuration.options},
	{IDD_OPT4,		O_LOCKFILES,					&_configuration.options},
	{IDD_OPT5,		O_REUSE_APPLICATION_INSTANCE,&_configuration.options},
	{IDD_OPT18,		O_SILENTLY_RELOAD_CHANGED_FILES,&_configuration.options},
	{IDD_STRINGLIST3, 0,								.did_listhandler = &_searchEnginelist},
	{IDD_STRINGLIST2, 0,								.did_listhandler = &_themelist},
	{IDD_STRINGLIST1, 0,								.did_listhandler = &_localeslist},
	{IDD_FONTSELECT,	TRUE,	_configuration.defaultFontFace, .did_command = dlg_selectFontCommand},
	{IDD_STRING2,	sizeof _configuration.prunedSearchDirectories,	_configuration.prunedSearchDirectories},
	// Terminate with 0
	{0}
};

static DIALOG_HELP_DESCRIPTOR _dAutosaveHelp[] = {
	{.dhd_itemNumber = IDD_INT1, .dhd_link = "manual\\configuration.md#number-of-undo-steps"},
	{.dhd_itemNumber = 0, .dhd_link = "manual\\configuration.md#save-options"},
	{.dhd_link = 0}
};

static DIALOG_HELP_DESCRIPTOR _dWarningsHelp[] = {
	{.dhd_itemNumber = 0, .dhd_link = "manual\\configuration.md#notification-options"},
	{.dhd_link = 0}
};

static DIALOG_HELP_DESCRIPTOR _dLayoutHelp[] = {
	{.dhd_itemNumber = 0, .dhd_link = "manual\\configuration.md#layout-options"},
	{.dhd_link = 0}
};

static DIALOG_HELP_DESCRIPTOR _dMiscHelp[] = {
	{.dhd_itemNumber = 0, .dhd_link = "manual\\configuration.md#miscellaneous-options"},
	{.dhd_link = 0}
};

static DIALOG_DESCRIPTOR _paramsPerPage[] = {
	{
		.dd_items = _dAutoSave, .dd_helpItems = _dAutosaveHelp
	},
	{
		.dd_items = _dWarnings, .dd_helpItems = _dWarningsHelp
	},
	{
		.dd_items = _dLayout, .dd_helpItems = _dLayoutHelp
	},
	{
		.dd_items = _dMisc, .dd_helpItems = _dMiscHelp
	}
};

static const char* _tempPathSettingName = "AutosavePath";

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
 * Returns the current print configuration of PKS Edit.
 */
PRTPARAM* config_getPrintConfiguration() {
	return &_prtparams;
}

/*
 * Returns the temp path into which PKS edit saves autosave and optionally backup files. 
 */
char* config_getPKSEditTempPath() {
	if (!_configuration.pksEditTempPath[0]) {
		config_getDefaultTempPath(_configuration.pksEditTempPath);
	}
	if (file_exists(_configuration.pksEditTempPath) != 0) {
		if (!_mkdir(_configuration.pksEditTempPath)) {
			EdTRACE(log_message(DEBUG_ERR, "Cannot create directory for PKS Edit temp path."));
		}
	}
	return _configuration.pksEditTempPath;
}

/*
 * Returns the configured language with a locale identifier (e.g. "de" or "en").
 */
char* config_getLocale() {
	char* pszLang = GetConfiguration()->language;
	if (pszLang == NULL || _stricmp("english", pszLang) == 0) {
		return "en";
	}
	return "de";
}

/*
 * Returns the command to be used to search for something on the internet
 * with the currently configured search engine.
 */
char* config_getInternetSearchCommand() {
	char* pszSelectedEngine = GetConfiguration()->searchEngine;
	SEARCH_ENGINE* pEngine = _searchEngines;

	while (pEngine->se_name[0]) {
		if (strcmp(pEngine->se_name, pszSelectedEngine) == 0) {
			return pEngine->se_command;
		}
		pEngine++;
	}
	return _searchEngines[0].se_command;
}

/*--------------------------------------------------------------------------
 * config_tempPathIsDefault()
 * Answer true, if the configured temp file path is the default path.
 */
int config_tempPathIsDefault() {
	if (_configuration.pksEditTempPath[0]) {
		char szDefault[EDMAXPATHLEN];
		config_getDefaultTempPath(szDefault);
		return file_fileNamesDiffer(szDefault, _configuration.pksEditTempPath) == FALSE;
	}
	return 0;
}

static DIALOG_DESCRIPTOR* _getDialogParsForPage(int pageIndex) {
	return &_paramsPerPage[pageIndex];
}

void EdOptionSet(void) {
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;
	INT_PTR tempRet;

	EDITOR_CONFIGURATION* pConfig = GetConfiguration();
	_themelist.li_param = (long long*)pConfig->themeName;
	_localeslist.li_param = (long long*)pConfig->language;
	_searchEnginelist.li_param = (long long*)pConfig->searchEngine;
	ICONSIZE* p = &pConfig->iconSize;
	_iconSizelist.li_param = (long long*)p;
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
    psh.pszCaption = (LPSTR) dlg_getResourceString(IDS_OPTIONS);
    psh.nPages = sizeof(psp) / sizeof(psp[0]);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = (PFNPROPSHEETCALLBACK)dlg_propertySheetCallback;

	tempRet = PropertySheet(&psh);
    if (tempRet == 1) {
		theme_setCurrent(pConfig->themeName);
		tb_updateImageList(hwndMain, NULL, 0);
		ui_switchToLanguage(pConfig->language);
		config_saveConfiguration(FALSE);
		fkey_visibilitychanged();
		mainframe_tabLayoutChanged(); 
	}
}

