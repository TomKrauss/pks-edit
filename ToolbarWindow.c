/*
 * ToolbarWindow.c
 *
 * PROJEKT: PKS-EDIT for Windows 95
 *
 * purpose: toolbars
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */

#include <windows.h>
#include <commctrl.h>
#include "winterf.h"
#include "pksedit.h"
#include "edfuncs.h"
#include "pksrc.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "actions.h"
#include "stringutil.h"

HWND	hwndToolbar;

/*
 * Callback to enable / disable toolbar buttons. 
 */
static void tb_propertyChanged(ACTION_BINDING* pActionBinding, PROPERTY_CHANGE_TYPE type, int newVal) {
    if (type == PC_ENABLED) {
        SendMessage(pActionBinding->ab_hwnd, TB_ENABLEBUTTON,
            pActionBinding->ab_item, MAKELPARAM(newVal, 0));
    }
}

/*
 * Register a toolbar action binding. 
 */
static void tb_registerBinding(int nCommand, TBBUTTON *pButton) {
    char szComment[128];
    char szKtext[128];
    MACROREF *pMref = macro_getMacroIndexForMenu(nCommand);
    if (pMref != NULL) {
        ACTION_BINDING binding = { tb_propertyChanged, hwndToolbar, nCommand };
        action_registerAction(pMref->index, binding);
        macro_getComment(szComment, szKtext, pMref->index, pMref->typ);
        if (szKtext[0]) {
            pButton->iString = (intptr_t)stralloc(szKtext);
        }
    }
}

/*------------------------------------------------------------
 * Returns the current size of the toolbar.
 */
void tb_wh(WORD *width, WORD *height)
{
	RECT		rect;

	if (hwndToolbar == 0 || (GetConfiguration()->layoutoptions & OL_TOOLBAR) == 0) {
		*width = 0;
		*height = 0;
		return;
	}
	GetWindowRect(hwndToolbar, &rect);
	*width = (WORD)(rect.right - rect.left) + 1;
	*height = (WORD)(rect.bottom - rect.top) + 1;
}

/*--------------------------------------------------------------------------
 * tb_initToolbar()
 * Initialize the PKS Edit toolbar.
 */
void tb_initToolbar(HWND hwndDaddy)
{
	TBADDBITMAP	tbabmp;
	TBBUTTON	tbb[20];
	LRESULT		iIndex;
	int			nButton;

	if (hwndToolbar) {
		return;
	}

    memset(tbb, 0, sizeof tbb);
    memset(&tbabmp, 0, sizeof tbabmp);
    InitCommonControls();
    hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL, 
		WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_ADJUSTABLE,
        0, 0, 0, 0, hwndDaddy, (HMENU) IDM_TOOLBAR, hInst, NULL);
	if (!hwndToolbar) {
		return;
	}
    SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, 
        (WPARAM) sizeof(TBBUTTON), 0);

	tbabmp.hInst = HINST_COMMCTRL;
	tbabmp.nID = IDB_STD_SMALL_COLOR;
	iIndex = SendMessage(hwndToolbar, TB_ADDBITMAP, 15, (LPARAM)&tbabmp);

    nButton = 0;
    tbb[nButton].iBitmap = (int)(iIndex + STD_FILENEW);
    tbb[nButton].idCommand = MNEWFILE;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(MNEWFILE, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_FILEOPEN);
    tbb[nButton].idCommand = MOPENF;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(MOPENF, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_FILESAVE);
    tbb[nButton].idCommand = MSAVERES;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(MSAVERES, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_CUT);
    tbb[nButton].idCommand = MTDEL;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(MTDEL, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_COPY);
    tbb[nButton].idCommand = MTCUT;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(MTCUT, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_PASTE);
    tbb[nButton].idCommand = MTPASTE;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(MTPASTE, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_FIND);
    tbb[nButton].idCommand = MFIND;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(MFIND, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_REPLACE);
    tbb[nButton].idCommand = MREPLACE;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(MREPLACE, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_UNDO);
    tbb[nButton].idCommand = IDM_UNDO;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(IDM_UNDO, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_REDOW);
    tbb[nButton].idCommand = IDM_DOAGAIN;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(IDM_REDO, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_PRINT);
    tbb[nButton].idCommand = IDM_PRINTTEXT;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(IDM_PRINTTEXT, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndex + STD_HELP);
    tbb[nButton].idCommand = IDM_HLPINDEX;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;
    tb_registerBinding(IDM_HLPINDEX, &tbb[nButton]);

	nButton++;
    SendMessage(hwndToolbar, TB_ADDBUTTONS, (WPARAM) nButton, (LPARAM) (LPTBBUTTON) &tbb);
    for (int i = 0; i < nButton; i++) {
        if (tbb[nButton].iString) {
            free((void*)(intptr_t)tbb[nButton].iString);
        }
    }
    SendMessage(hwndToolbar, TB_SETMAXTEXTROWS, 0, 0);
    ShowWindow(hwndToolbar, SW_SHOW);
}

