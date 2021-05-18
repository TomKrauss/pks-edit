/*
 * EditorConfiguration.c
 *
 * PROJEKT: PKS-EDIT for Windows 95
 *
 * Configuration of global options of PKS edit. These are for now 
 * saved in a file PKSEDIT.INI by default located in the PKS_SYS folder.
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include <commctrl.h>
#include "winterf.h"
#include "pksedit.h"
#include "dial2.h"
#include "pksrc.h"
#include "xdialog.h"
#include "editorconfiguration.h"

extern void fkey_visibilitychanged(void);

/*
 * Autosave the editor configuration, when PKS Edit exits.
 */
static void AutosaveConfiguration() {
	EDITOR_CONFIGURATION* config = GetConfiguration();

	if (config->options & O_SAVE_SETTINGS_ON_EXIT) {
		prof_save(config, FALSE);
	}
	mac_autosave(config->options & O_SAVESEQ ? FALSE : TRUE);
}

/*
 * The default configuration.  
 */
static EDITOR_CONFIGURATION _configuration = {
	(UNDOENABLED | WARNINGS | E_BELL | O_CUTBUFEQCLIP | O_HIDE_BLOCK_ON_CARET_MOVE),
	(OL_OPTIONBAR | OL_SHOWSTATUS),
	1,
	3,
	"",
	"INCLUCDE;INC",
	AutosaveConfiguration
};

static DIALPARS _dAutoSave[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_INT1,		sizeof _configuration.asminutes,		&_configuration.asminutes,
	IDD_OPT1,		O_GARBAGE_AS,							& _configuration.options,
	IDD_STRING1,	sizeof _configuration.pksEditTempPath,	_configuration.pksEditTempPath,
	IDD_OPT2,		O_SAVE_SETTINGS_ON_EXIT,				&_configuration.options,
	IDD_OPT3,		O_READPIC,								& _configuration.options,
	IDD_OPT4,		O_SAVESEQ,								& _configuration.options,
	// Terminate with 0
	0
};

static DIALPARS _dWarnings[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_OPT1,		WARNINGS,			&_configuration.options,
	IDD_OPT2,		E_BELL,				&_configuration.options,
	IDD_OPT3,		E_FLASH,			&_configuration.options,
	IDD_OPT4,		O_MESSAGES,			& _configuration.options,
	// Terminate with 0
	0
};

static DIALPARS _dLayout[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_OPT1,		OL_OPTIONBAR,		&_configuration.layoutoptions,
	IDD_OPT2,		OL_FKEYS,			&_configuration.layoutoptions,
	IDD_OPT3,		OL_SHOWSTATUS,		&_configuration.layoutoptions,
	IDD_OPT4,		OL_TOOLBAR,			&_configuration.layoutoptions,
	// Terminate with 0
	0
};

static DIALPARS _dMisc[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_OPT1,		UNDOENABLED,					& _configuration.options,
	IDD_INT1,		sizeof _configuration.nundo,	&_configuration.nundo,
	IDD_OPT2,		O_HIDE_BLOCK_ON_CARET_MOVE,		& _configuration.options,
	IDD_OPT3,		O_FORMFOLLOW,					& _configuration.options,
	// Terminate with 0
	0
};

static DIALPARS* _paramsPerPage[] = {
	_dAutoSave,
	_dWarnings,
	_dLayout,
	_dMisc
};

/**
 * Returns a pointer to the current editor configuration. If not yet initialized it will be initialized on the fly. 
 */
EDITOR_CONFIGURATION* GetConfiguration() {
	return &_configuration;
}

static DIALPARS* _getDialogParsForPage(int pageIndex) {
	return _paramsPerPage[pageIndex];
}

void EdOptionSet(void) {
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;
	INT_PTR tempRet;

	InitCommonControls();
	SetXDialogParams(_getDialogParsForPage, TRUE);
	memset(&psh, 0, sizeof psh);
	memset(psp, 0, sizeof psp);

	psp[0].dwSize = sizeof(psp[0]);
    psp[0].hInstance = hInst;
    psp[0].pszTemplate = MAKEINTRESOURCE(DLG_OPT_AUTOSAVE);
    psp[0].pfnDlgProc = DlgStdProc;

    psp[1].dwSize = sizeof(psp[1]);
    psp[1].hInstance = hInst;
    psp[1].pszTemplate = MAKEINTRESOURCE(DLG_OPT_WARNINGS);
    psp[1].pfnDlgProc = DlgStdProc;

	psp[2].dwSize = sizeof(psp[2]);
	psp[2].hInstance = hInst;
	psp[2].pszTemplate = MAKEINTRESOURCE(DLG_OPT_LAYOUT);
	psp[2].pfnDlgProc = DlgStdProc;

	psp[3].dwSize = sizeof(psp[3]);
	psp[3].hInstance = hInst;
	psp[3].pszTemplate = MAKEINTRESOURCE(DLG_OPT_MISC);
	psp[3].pfnDlgProc = DlgStdProc;

	psh.dwSize = sizeof(psh);
    psh.dwFlags = PSH_PROPSHEETPAGE;
    psh.hwndParent = hwndFrame;
    psh.hInstance = hInst;
    psh.pszIcon = 0;
    psh.pszCaption = (LPSTR) "Sonstige Einstellungen...";
    psh.nPages = sizeof(psp) / sizeof(psp[0]);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;

	tempRet = PropertySheet(&psh);
    if (tempRet == 1) {
		prof_save(GetConfiguration(), FALSE);
		fkey_visibilitychanged();
	}
    return;
}

