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
#include "fontawesome.h"
#include "customcontrols.h"
#include "winterf.h"
#include "pksedit.h"
#include "edfuncs.h"
#include "pksrc.h"
#include "funcdef.h"
#include "actions.h"
#include "edierror.h"
#include "edfuncs.h"
#include "editorconfiguration.h"
#include "actions.h"
#include "stringutil.h"
#include "findandreplace.h"
#include "themes.h"
#include "dpisupport.h"
#include "xdialog.h"

#define TB_IMAGE_SIZE       18

static HWND	hwndToolbar;
static int nToolbarButtons;
static HIMAGELIST hImageList;
static HIMAGELIST hImageListDisabled;
HWND    hwndRebar;

/*
 * Draws a text in an antialiased way.
 */
extern BOOL paint_loadFontAwesome(void* pFontData, DWORD len);
extern HBITMAP tb_createAwesomeIcons(COLORREF nColorRef, int nSize, wchar_t icons[], int nIcons);

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

/*
 * Load the font-awesome font.
 */
static void tb_loadFontAwesome() {
    static BOOL fontLoaded;

    if (fontLoaded) {
        return;
    }
    fontLoaded = TRUE;
    HINSTANCE hInstance = hInst; // Or could even be a DLL's HINSTANCE
    HRSRC  hFntRes = FindResource(hInstance, MAKEINTRESOURCE(IDS_FONT_AWESOME), RT_FONT);
    if (hFntRes) { // If we have found the resource ... 
        HGLOBAL hFntMem = LoadResource(hInstance, hFntRes); // Load it
        if (hFntMem != NULL) {
            void* pFontData = LockResource(hFntMem); // Lock it into accessible memory
            DWORD len = SizeofResource(hInstance, hFntRes);
            paint_loadFontAwesome(pFontData, len);
            FreeResource(hFntMem);
        }
    }
}

/*
 * Create an image list with images created from font-awesome icons. 
 */
static HIMAGELIST tb_createImageList(int nIconSize, COLORREF cColor, wchar_t icons[], int nIcons) {
    HBITMAP hBmp = tb_createAwesomeIcons(cColor, nIconSize, icons, nIcons);
    HIMAGELIST hList = ImageList_Create(nIconSize, nIconSize,
        ILC_COLOR32 | ILC_HIGHQUALITYSCALE, nIcons, 0
    );
    ImageList_Add(hList, hBmp, 0);
    DeleteObject(hBmp);
    return hList;
}

/*
 * Update the image list for the toolbar.
 */
void tb_updateImageList() {
    wchar_t tbIcons[] = {
        FA_ICON_FILE,
        FA_ICON_FOLDER_OPEN,
        FA_ICON_SAVE,
        FA_ICON_ERASER,
        FA_ICON_CUT,
        FA_ICON_CLIPBOARD,
        FA_ICON_SEARCH,
        FA_ICON_EXCHANGE_ALT,
        FA_ICON_SEARCH_PLUS,
        FA_ICON_ARROW_LEFT,
        FA_ICON_ARROW_RIGHT,
        FA_ICON_UNDO,
        FA_ICON_REDO,
        FA_ICON_PRINT,
        FA_ICON_COG,
        FA_ICON_QUESTION,
        FA_ICON_ARROW_UP,
        FA_ICON_ARROW_DOWN,
        FA_ICON_STOP_CIRCLE
    };
    HIMAGELIST hOld1 = hImageList;
    HIMAGELIST hOldDisabled = hImageListDisabled;
    static int nOldIconSize;
    static COLORREF cOldIconColor;

    int nIconSize = dpisupport_getTbIconSize();
    COLORREF cIconColor = theme_getCurrent()->th_iconColor;
    if (nOldIconSize == nIconSize && cIconColor == cOldIconColor) {
        return;
    }
    cOldIconColor = cIconColor;
    nOldIconSize = nIconSize;
    hImageList = tb_createImageList(nIconSize, cIconColor, tbIcons, DIM(tbIcons));
    SendMessageW(hwndToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImageList);

    hImageListDisabled = tb_createImageList(nIconSize, theme_getCurrent()->th_dialogDisabled, tbIcons, DIM(tbIcons));
    SendMessageW(hwndToolbar, TB_SETDISABLEDIMAGELIST, (WPARAM)0, (LPARAM)hImageListDisabled);

    SendMessage(hwndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(nIconSize, nIconSize));
    if (hOld1) {
        ImageList_Destroy(hOld1);
    }
    if (hOldDisabled) {
        ImageList_Destroy(hOldDisabled);
    }
    DWORD dwBtnSize = (DWORD)SendMessage(hwndToolbar, TB_GETBUTTONSIZE, 0, 0);

    REBARBANDINFO rbBand = { REBARBANDINFO_V6_SIZE };
    rbBand.fMask =
        RBBIM_CHILDSIZE   // child size members are valid.
        | RBBIM_SIZE;       // cx is valid
    rbBand.fStyle = RBBS_FIXEDSIZE;

    // Set values unique to the band with the toolbar.
    rbBand.lpText = TEXT("");
    rbBand.hwndChild = hwndToolbar;
    rbBand.cyChild = LOWORD(dwBtnSize);
    rbBand.cxMinChild = (nToolbarButtons - 2) * HIWORD(dwBtnSize);
    rbBand.cyMinChild = LOWORD(dwBtnSize);
    // The default width is the width of the buttons.
    rbBand.cx = 0;

    // Update the band info for the toolbar.
    SendMessage(hwndRebar, RB_SETBANDINFO, 0, (LPARAM) & rbBand);
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
    tb_loadFontAwesome();
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

    tb_updateImageList();

    iIndexExtra = 0;
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
        if (previousFocusWnd == NULL) {
            previousFocusWnd = (HWND)wParam;
        }
        PostMessage(hwnd, EM_SETSEL, 0, -1);
        find_startIncrementalSearch();
        PostMessage(GetParent(hwnd), LAB_CHILD_FOCUS_CHANGE, TRUE, 0);
        break;
    case WM_KILLFOCUS:
        PostMessage(GetParent(hwnd), LAB_CHILD_FOCUS_CHANGE, FALSE, 0);
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
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_NOHIDESEL | ES_WANTRETURN,
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

    if (uMsg == WM_ERASEBKGND) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect((HDC)wParam, &rc, theme_getDialogBackgroundBrush());
        return TRUE;
    }
        
    return CallWindowProc(rebarOriginalWindowProc, hwnd, uMsg,
        wParam, lParam);
}

/*
 * Initialize the search entry cue banner.
 */
static void tb_initSearchEntryCueBanner(char* pszText) {
    char* pszKey;
    strcpy(pszText, dlg_getResourceString(IDS_SEARCH));
    pszKey = macro_getKeyText(CMD_INITIATE_INCREMENTAL_SEARCH);
    if (pszKey) {
        strcat(pszText, " (");
        strcat(pszText, pszKey);
        strcat(pszText, ")");
    }
}

/*--------------------------------------------------------------------------
 * tb_initRebar()
 * Initialize the PKS Edit top bar (rebar).
 */
HWND tb_initRebar(HWND hwndOwner) {
    HWND hwndEntryField;
    HWND hwndToolbar;
    wchar_t searchIcons[] = {
        FA_ICON_SEARCH
    };

    if (hwndRebar) {
        return hwndRebar;
    }

    // Create the rebar.
    hwndRebar = CreateWindowEx(WS_EX_TOOLWINDOW,
        REBARCLASSNAME,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
        WS_CLIPCHILDREN | RBS_BANDBORDERS | RBS_FIXEDORDER,
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

    tb_updateColors();
    hwndToolbar = tb_initToolbar(hwndRebar);
    hwndEntryField = tb_initSearchEntryField(hwndRebar);
    char szText[80];
    tb_initSearchEntryCueBanner(szText);
    HIMAGELIST hImageListSearch = tb_createImageList(TB_IMAGE_SIZE, RGB(88, 88, 88), searchIcons, 1);
    HWND hwndLabel = cust_createLabeledWindow(hwndRebar, ImageList_GetIcon(hImageListSearch, 0, ILD_TRANSPARENT), TEXT(szText), hwndEntryField);
    ImageList_Destroy(hImageListSearch);

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
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)0, (LPARAM)&rbBand);

    rbBand.hwndChild = hwndLabel;
    rbBand.cxMinChild = 300;
    rbBand.cyMinChild = 25;
    // The default width should be set to some value wider than the text. The entry field itself will expand to fill the band.
    rbBand.cx = 300;

    // Add the band that has the entry field.
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)1, (LPARAM)&rbBand);

    return (hwndRebar);
}





