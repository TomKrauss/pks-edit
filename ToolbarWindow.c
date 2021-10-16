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
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include "customcontrols.h"
#include "winterf.h"
#include "pksedit.h"
#include "edfuncs.h"
#include "pksrc.h"
#include "functab.h"
#include "actions.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "actions.h"
#include "stringutil.h"
#include "findandreplace.h"
#include "themes.h"
#include "xdialog.h"


static HWND	hwndToolbar;
static int nToolbarButtons;
HWND    hwndRebar;

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
        action_registerAction(pMref->index, binding, FALSE);
        macro_getComment(szComment, szKtext, pMref->index, pMref->typ);
        if (szKtext[0]) {
            pButton->iString = (intptr_t)_strdup(szKtext);
        }
    }
}

/*------------------------------------------------------------
 * Returns the current size of the toolbar.
 */
void tb_wh(WORD *width, WORD *height)
{
	RECT		rect;

	if (hwndRebar == 0 || (GetConfiguration()->layoutoptions & OL_TOOLBAR) == 0) {
		*width = 0;
		*height = 0;
		return;
	}
	GetWindowRect(hwndRebar, &rect);
	*width = (WORD)(rect.right - rect.left);
	*height = (WORD)(rect.bottom - rect.top);
}

static void tb_updateColors() {
    THEME_DATA* pData = theme_getCurrent();
    SendMessage(hwndRebar, RB_SETBKCOLOR, 0, pData->th_dialogBackground);
}

/*--------------------------------------------------------------------------
 * tb_initToolbar()
 * Initialize the actual toolbar toolbar.
 */
static HWND tb_initToolbar(HWND hwndOwner) {
	TBADDBITMAP	tbabmp;
    TBADDBITMAP	tbabmp2;
    TBBUTTON	tbb[30];
    LRESULT		iIndexExtra;
    int			nButton;

	if (hwndToolbar) {
		return hwndToolbar;
	}

    memset(tbb, 0, sizeof tbb);
    memset(&tbabmp, 0, sizeof tbabmp);
    memset(&tbabmp2, 0, sizeof tbabmp2);
    hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL,
		WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_CUSTOMERASE| CCS_NODIVIDER | CCS_NORESIZE | CCS_ADJUSTABLE,
        0, 0, 0, 0, hwndOwner, (HMENU) IDM_TOOLBAR, hInst, NULL);
	if (!hwndToolbar) {
		return NULL;
	}
    SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, 
        (WPARAM) sizeof(TBBUTTON), 0);

    HBITMAP hBmp = LoadImage(hInst, MAKEINTRESOURCE(IDB_NEW_ICONS), IMAGE_BITMAP, 23*16,16, LR_CREATEDIBSECTION);
    HIMAGELIST hImageList = ImageList_Create(16, 16,
        ILC_COLOR32,
        18, 0
    );
    ImageList_Add(hImageList, hBmp, 0);
    iIndexExtra = 0;
    SendMessageW(hwndToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImageList);

    nButton = 0;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 0);
    tbb[nButton].idCommand = MNEWFILE;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MNEWFILE, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 1);
    tbb[nButton].idCommand = MOPENF;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MOPENF, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 2);
    tbb[nButton].idCommand = MSAVERES;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MSAVERES, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 3);
    tbb[nButton].idCommand = MTDEL;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MTDEL, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 4);
    tbb[nButton].idCommand = MTCUT;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MTCUT, &tbb[nButton]);

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 5);
    tbb[nButton].idCommand = MTPASTE;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MTPASTE, &tbb[nButton]);

	nButton++;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;

	nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 6);
    tbb[nButton].idCommand = MFIND;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MFIND, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 7);
    tbb[nButton].idCommand = MREPLACE;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MREPLACE, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 8);
    tbb[nButton].idCommand = MEDIFLIS;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MEDIFLIS, &tbb[nButton]);

    nButton++;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 9);
    tbb[nButton].idCommand = IDM_GOTO_PREVIOUS;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_GOTO_PREVIOUS, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 10);
    tbb[nButton].idCommand = IDM_GOTO_NEXT;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_GOTO_NEXT, &tbb[nButton]);

    nButton++;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 11);
    tbb[nButton].idCommand = IDM_UNDO;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_UNDO, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 12);
    tbb[nButton].idCommand = IDM_REDO;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_REDO, &tbb[nButton]);

    nButton++;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 13);
    tbb[nButton].idCommand = IDM_PRINTTEXT;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_PRINTTEXT, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 14);
    tbb[nButton].idCommand = MOPTION;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(MOPTION, &tbb[nButton]);

    nButton++;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    
    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 15);
    tbb[nButton].idCommand = IDM_HLPINDEX;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_HLPINDEX, &tbb[nButton]);

    nButton++;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 16);
    tbb[nButton].idCommand = IDM_COMPARE_NAVIGATE_PREV;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_COMPARE_NAVIGATE_PREV, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 17);
    tbb[nButton].idCommand = IDM_COMPARE_NAVIGATE_NEXT;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_COMPARE_NAVIGATE_NEXT, &tbb[nButton]);

    nButton++;
    tbb[nButton].iBitmap = (int)(iIndexExtra + 18);
    tbb[nButton].idCommand = IDM_COMPARE_CLEAR;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tb_registerBinding(IDM_COMPARE_CLEAR, &tbb[nButton]);

    nButton++;
    nToolbarButtons = nButton;
    SendMessage(hwndToolbar, TB_ADDBUTTONS, (WPARAM) nButton, (LPARAM) (LPTBBUTTON) &tbb);
    for (int i = 0; i < nButton; i++) {
        if (tbb[i].iString) {
            free((void*)(intptr_t)tbb[i].iString);
        }
    }
    SendMessage(hwndToolbar, TB_SETMAXTEXTROWS, 0, 0);
    HWND hwndTooltip = (HWND) SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0);
    if (hwndTooltip) {
        theme_enableDarkMode(hwndTooltip);
    }
    return hwndToolbar;
}

/*
 * Custom window procedure subclass for the incremental search edit field.
 */
static WNDPROC oldEditProc;
LRESULT CALLBACK incrementalSearchEditWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND previousFocusWnd;
    LRESULT nRet;
    char pszBuf[256];

    switch (msg) {
    case WM_CHAR:
        if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
            return 0;
        }
        nRet = CallWindowProc(oldEditProc, hwnd, msg, wParam, lParam);
        Edit_GetText(hwnd, pszBuf, sizeof pszBuf);
        find_incrementally(pszBuf, RE_IGNCASE | O_WRAPSCAN, 1, FALSE);
        return nRet;
    case WM_SETFOCUS:
        previousFocusWnd = (HWND)wParam;
        PostMessage(hwnd, EM_SETSEL, 0, -1);
        find_startIncrementalSearch();
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_RETURN:
            //Do your stuff
            Edit_GetText(hwnd, pszBuf, sizeof pszBuf);
            find_incrementally(pszBuf, RE_IGNCASE| O_WRAPSCAN, 1, TRUE);
            return 0;
        case VK_ESCAPE:
            if (previousFocusWnd != NULL) {
                SetFocus(previousFocusWnd);
                previousFocusWnd = NULL;
                return 0;
            }
            break;
        //If not your key, skip to default:
        }
    }
    return CallWindowProc(oldEditProc, hwnd, msg, wParam, lParam);
}
/*
 * Creates the entry field for incremental search in the PKS Edit toolbar. 
 */
static HWND hwndIncrementalSearchField;
static void tb_enableEntryField(ACTION_BINDING * pBinding, PROPERTY_CHANGE_TYPE type, int newValue) {
    if (type == PC_ENABLED) {
        EnableWindow(hwndIncrementalSearchField, newValue);
    }
}
static HWND tb_initSearchEntryField(HWND hwndOwner) {

    hwndIncrementalSearchField = CreateWindowEx(0, WC_EDIT, "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_LEFT | ES_NOHIDESEL | ES_WANTRETURN,
        0, 0, 180, CW_USEDEFAULT,
        hwndOwner, (HMENU)IDM_INCREMENTAL_SEARCH,
        hInst, NULL);
    SendMessage(hwndIncrementalSearchField, WM_SETFONT, (WPARAM)cust_getDefaultEditorFont(), 0);
    oldEditProc = (WNDPROC)SetWindowLongPtr(hwndIncrementalSearchField, GWLP_WNDPROC, (LONG_PTR)incrementalSearchEditWndProc);
    action_registerAction(CMD_INITIATE_INCREMENTAL_SEARCH, (ACTION_BINDING) { tb_enableEntryField, 0L, 0 }, TRUE);
    return hwndIncrementalSearchField;
}


/*
 * Move the input focus to the incremental search UI entry field. 
 */
int find_initiateIncrementalSearch() {
    SetFocus(hwndIncrementalSearchField);
    return 1;
}

/*
 * Custom Rebar window procedure for coloring controls in rebar. 
 */
static WNDPROC rebarOriginalWindowProc;
static LRESULT APIENTRY tb_myRebarProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam) {

    if (uMsg == WM_CTLCOLOREDIT || uMsg == WM_CTLCOLORSTATIC) {
        THEME_DATA* pTheme = theme_getCurrent();
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, pTheme->th_dialogForeground);
        SetBkColor(hdc, pTheme->th_dialogBackground);
        return (LRESULT)theme_getDialogBackgroundBrush();
    }
    if (uMsg == WM_ERASEBKGND) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect((HDC)wParam, &rc, theme_getDialogBackgroundBrush());
        return TRUE;
    }
    return CallWindowProc(rebarOriginalWindowProc, hwnd, uMsg,
        wParam, lParam);
}

/*--------------------------------------------------------------------------
 * tb_initRebar()
 * Initialize the PKS Edit top bar (rebar).
 */
HWND tb_initRebar(HWND hwndOwner) {
    HWND hwndEntryField;
    HWND hwndToolbar;

    if (hwndRebar) {
        return hwndRebar;
    }

    // Create the rebar.
    hwndRebar = CreateWindowEx(WS_EX_TOOLWINDOW,
        REBARCLASSNAME,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
        WS_CLIPCHILDREN | RBS_BANDBORDERS,
        0, 0, 0, 0,
        hwndOwner,
        NULL,
        hInst, // global instance handle
        NULL);

    if (!hwndRebar) {
        return NULL;
    }

    rebarOriginalWindowProc = (WNDPROC)SetWindowLongPtr(hwndRebar,
        GWLP_WNDPROC, (LONG_PTR)tb_myRebarProc);

    THEME_DATA* pTheme = theme_getCurrent();
    tb_updateColors();
    hwndToolbar = tb_initToolbar(hwndRebar);
    hwndEntryField = tb_initSearchEntryField(hwndRebar);

    // Initialize band info used by both bands.
    // as we are - for theming purpose - still rely on elder common controls versions - we do not use sizeof(REBARBANDINFO) here.
    REBARBANDINFO rbBand = { REBARBANDINFO_V6_SIZE };
    rbBand.fMask =
        RBBIM_STYLE       // fStyle is valid.
        | RBBIM_TEXT        // lpText is valid.
        | RBBIM_CHILD       // hwndChild is valid.
        | RBBIM_CHILDSIZE   // child size members are valid.
        | RBBIM_SIZE;       // cx is valid
    rbBand.fStyle = RBBS_FIXEDSIZE;

    // Get the height of the toolbar.
    DWORD dwBtnSize = (DWORD)SendMessage(hwndToolbar, TB_GETBUTTONSIZE, 0, 0);

    // Set values unique to the band with the toolbar.
    rbBand.lpText = TEXT("");
    rbBand.hwndChild = hwndToolbar;
    rbBand.cyChild = LOWORD(dwBtnSize);
    rbBand.cxMinChild = (nToolbarButtons -2) * HIWORD(dwBtnSize);
    rbBand.cyMinChild = LOWORD(dwBtnSize);
    // The default width is the width of the buttons.
    rbBand.cx = 0;

    // Add the band that has the toolbar.
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

    HWND hwndLabel = CreateWindowEx(0, WC_STATIC, TEXT(dlg_getResourceString(IDS_SEARCH)),
        WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
        0, 00, CW_USEDEFAULT, CW_USEDEFAULT,
        hwndOwner, (HMENU)IDM_INCREMENTAL_SEARCH,
        hInst, NULL);
    SendMessage(hwndLabel, WM_SETFONT, (WPARAM)cust_getDefaultEditorFont(), 0);
    rbBand.hwndChild = hwndLabel;
    rbBand.cxMinChild = 40;
    rbBand.cyMinChild = 20;
    rbBand.cx = 40;
    // Add the band that has the label for the entry field.
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

    rbBand.hwndChild = hwndEntryField;
    rbBand.cxMinChild = 250;
    rbBand.cyMinChild = 20;
    // The default width should be set to some value wider than the text. The entry field itself will expand to fill the band.
    rbBand.cx = 250;

    // Add the band that has the entry field.
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

    rbBand.fMask =
        RBBIM_STYLE       // fStyle is valid.
        | RBBIM_CHILDSIZE   // child size members are valid.
        | RBBIM_SIZE;       // cx is valid
    rbBand.cxMinChild = 50;
    rbBand.cyMinChild = 20;
    // The default width should be set to some value wider than the text. The entry field itself will expand to fill the band.
    rbBand.cx = 250;
    rbBand.fStyle = RBBS_FIXEDSIZE;

    // Add the band that has the entry field.
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

    return (hwndRebar);
}





