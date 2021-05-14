/*
 * EditSettings.c
 *
 * PROJEKT: PKS-EDIT for Windows 95
 *
 * purpose: Property settings dialogs
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include <windows.h>
#include <commctrl.h>
#include "winterf.h"
#include "pksedit.h"
#include "dial2.h"
#include "pksrc.h"
#include "xdialog.h"
#include "edierror.h"

extern BOOL CALLBACK DlgStdProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
extern void fkey_visibilitychanged(void);

extern int		_nundo;
extern int		_asminutes;
extern char		_aspath[256];

int			bInPropertySheet;

static DIALPARS _d[] = {
	IDD_NOCHANGEONCANCEL,	0,	0,
	IDD_STRING1,	sizeof _aspath,	_aspath,
	IDD_INT1,		sizeof _nundo,		&_nundo,
	IDD_INT2,		sizeof _asminutes,	&_asminutes,
	IDD_OPT1,		WARNINGS,			&_options,
	IDD_OPT2,		E_BELL,			&_options,
	IDD_OPT3,		E_FLASH,			&_options,
	IDD_OPT4,		AUTOWRITE,		&_options,
	IDD_OPT5,		O_SAVESEQ,		&_options,
	IDD_OPT6,		O_READPIC,		&_options,
	IDD_OPT7,		O_SAVESET,		&_options,
	IDD_OPT8,		UNDOENABLED,		&_options,
	IDD_OPT9,		O_FORMFOLLOW,		&_options,
	IDD_OPT10,	O_GARBAGE_AS,		&_options,
	IDD_OPT11,	O_MESSAGES,		&_options,
	IDD_OPT12,	O_MDISCROLL,		&_options,
	IDD_OPT13,	OL_FKEYS,			&_layoutoptions,
	IDD_OPT14,	OL_OPTIONBAR,		&_layoutoptions,
	IDD_OPT15,	OL_SHOWSTATUS,		&_layoutoptions,
	IDD_OPT16,	O_CUTBUFEQCLIP|O_AUTODELBLOCK, &_options,
	IDD_OPT17,	O_LOCKFILES,		&_options,
	IDD_OPT18,	OL_TOOLBAR,			&_layoutoptions,
	0
};

void EdOptionSet(void) {

    PROPSHEETPAGE psp[2];
    PROPSHEETHEADER psh;
	int tempRet;

	InitCommonControls();
	SetXDialogParams(_d);
    psp[0].dwSize = sizeof(psp[0]);
    psp[0].dwFlags = 0;
    psp[0].hInstance = hInst;
    psp[0].pszTemplate = MAKEINTRESOURCE(DLG_OPT_AUTOSAVE);
    psp[0].pszIcon = 0;
    psp[0].pfnDlgProc = DlgStdProc;
    psp[0].pszTitle = 0;
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(psp[0]);
    psp[1].dwFlags = 0;
    psp[1].hInstance = hInst;
    psp[1].pszTemplate = MAKEINTRESOURCE(DLG_OPT_WARNINGS);
    psp[1].pszIcon = 0;
    psp[1].pfnDlgProc = DlgStdProc;
    psp[1].pszTitle = 0;
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

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

	bInPropertySheet = TRUE;
	tempRet = PropertySheet(&psh);
    if (tempRet == 1) {
		prof_save(1);
		fkey_visibilitychanged();
	}
	bInPropertySheet = FALSE;
    return;
}
