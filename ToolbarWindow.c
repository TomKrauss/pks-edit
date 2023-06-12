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
#include "pksmacro.h"
#include "actionbindings.h"
#include "pksrc.h"
#include "funcdef.h"
#include "actions.h"
#include "edierror.h"
#include "pksmacro.h"
#include "editorconfiguration.h"
#include "actions.h"
#include "stringutil.h"
#include "findandreplace.h"
#include "themes.h"
#include "linkedlist.h"
#include "dpisupport.h"
#include "xdialog.h"

#define TB_IMAGE_SIZE       18

#define M(T,CMD)      ((int)(T<<8)|CMD)

static HWND hwndSearchBarLabel;
static HWND	hwndToolbar;
static int nToolbarButtons;
static HIMAGELIST hImageList;
static HIMAGELIST hImageListDisabled;
HWND    hwndRebar;

/*
 * Draws a text in an antialiased way.
 */
extern BOOL paint_loadFontAwesome(void* pFontData, DWORD len);
extern HBITMAP tb_createAwesomeIcons(COLORREF nColorRef, int nSize, CHAR_WITH_STYLE icons[], int nIcons);

/*
 * Callback to enable / disable toolbar buttons. 
 */
static void tb_propertyChanged(ACTION_BINDING* pActionBinding, PROPERTY_CHANGE_TYPE type, int newVal) {
    if (type == PC_ENABLED) {
        SendMessage(pActionBinding->ab_hwnd, TB_ENABLEBUTTON,
            M(pActionBinding->ab_type, pActionBinding->ab_item), MAKELPARAM(newVal, 0));
    }
}

/*
 * Register a toolbar action binding. 
 */
static void tb_registerBinding(int nCommand, int nType, TBBUTTON *pButton) {
    char szComment[MAC_COMMENTLEN];
    char szKtext[128];
    MACROREF command = (MACROREF){ .index = nCommand, .typ = nType};
    ACTION_BINDING binding = { .ab_propertyChanged = tb_propertyChanged, .ab_hwnd = hwndToolbar, .ab_item = nCommand, .ab_type = nType };
    action_registerAction(nCommand, nType, binding, FALSE);
    command_getTooltipAndLabel(command, szComment, szKtext);
    if (szKtext[0]) {
        pButton->iString = (intptr_t)_strdup(szKtext);
    }
    else if (nType == CMD_MACRO) {
        MACRO* mp = macro_getByIndex(nCommand);
        macro_getLabelFor(mp, szComment, sizeof szComment);
        pButton->iString = (intptr_t)_strdup(szComment);
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

static void tb_loadFont(int id) {
    HINSTANCE hInstance = hInst; // Or could even be a DLL's HINSTANCE
    HRSRC  hFntRes = FindResource(hInstance, MAKEINTRESOURCE(id), RT_FONT);
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
 * Load the font-awesome font.
 */
static void tb_loadFontAwesome() {
    static BOOL fontLoaded;

    if (fontLoaded) {
        return;
    }
    fontLoaded = TRUE;
    tb_loadFont(IDS_FONT_AWESOME);
    tb_loadFont(IDS_FONT_AWESOME_REGULAR);
}

/*
 * Create an image list with images created from font-awesome icons. 
 */
static HIMAGELIST tb_createImageList(int nIconSize, COLORREF cColor, CHAR_WITH_STYLE icons[], int nIcons) {
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
static CHAR_WITH_STYLE* _currentIconList;
static int _currentIconCount;
void tb_updateImageList(HWND hwnd, CHAR_WITH_STYLE *tbIcons, int nCount) {
    HIMAGELIST hOld1 = hImageList;
    HIMAGELIST hOldDisabled = hImageListDisabled;
    static int nOldIconSize;
    static COLORREF cOldIconColor;
    if (!tbIcons) {
        tbIcons = _currentIconList;
        nCount = _currentIconCount;
    } else {
        free(_currentIconList);
        _currentIconList = tbIcons;
        _currentIconCount = nCount;
    }
    int nIconSize = dpisupport_getTbIconSize(hwnd);
    COLORREF cIconColor = theme_getCurrent()->th_iconColor;
    if (nOldIconSize == nIconSize && cIconColor == cOldIconColor) {
        return;
    }
    cOldIconColor = cIconColor;
    nOldIconSize = nIconSize;
    hImageList = tb_createImageList(nIconSize, cIconColor, tbIcons, nCount);
    SendMessageW(hwndToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImageList);

    hImageListDisabled = tb_createImageList(nIconSize, theme_getCurrent()->th_dialogDisabled, tbIcons, nCount);
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
        ;       // cx is valid
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
    TBBUTTON	* tbb;

	if (hwndToolbar) {
		return hwndToolbar;
	}
    tb_loadFontAwesome();
    memset(&tbabmp, 0, sizeof tbabmp);
    memset(&tbabmp2, 0, sizeof tbabmp2);
    hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_CUSTOMERASE| CCS_NODIVIDER | CCS_NORESIZE | CCS_ADJUSTABLE,
        0, 0, 0, 0, hwndOwner, (HMENU) IDM_TOOLBAR, hInst, NULL);
	if (!hwndToolbar) {
		return NULL;
	}
    SendMessage(hwndToolbar, TB_AUTOSIZE, 0, 0);
    SendMessage(hwndToolbar, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_MIXEDBUTTONS);
    SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0);
    SendMessage(hwndToolbar, TB_SETMAXTEXTROWS, 0, 0);

    TOOLBAR_BUTTON_BINDING* pButtons = bindings_getToolbarBindingsFor(DEFAULT_ACTION_CONTEXT);
    int nButtons = ll_size((LINKED_LIST*)pButtons);
    tbb = calloc(nButtons, sizeof * tbb);
    CHAR_WITH_STYLE* pwImageCodes = calloc(nButtons, sizeof * pwImageCodes);
    int nImageIndex = 0;
    int iIndexExtra = 0;
    for (int nButton = 0; pButtons; nButton++) {
        if (pButtons->tbb_faIcon != 0) {
            pwImageCodes[nImageIndex].symbol = pButtons->tbb_faIcon;
            pwImageCodes[nImageIndex].regular = pButtons->tbb_faRegular;
            tbb[nButton].iBitmap = (int)(iIndexExtra + nImageIndex++);
        }
        if (pButtons->tbb_isSeparator) {
            tbb[nButton].fsStyle = BTNS_SEP;
        } else {
            int nCmd = pButtons->tbb_macref.index;
            int nType = pButtons->tbb_macref.typ;
            tbb[nButton].idCommand = M(nType, nCmd);
            tb_registerBinding(nCmd, nType, &tbb[nButton]);
            tbb[nButton].fsStyle = BTNS_BUTTON;
            const char* pszBoundLabel = bindings_getBoundText(&pButtons->tbb_label);
            if (pszBoundLabel) {
                // do not do this here: tbb[nButton].iString = (INT_PTR)pszBoundLabel; 
                tbb[nButton].fsStyle = BTNS_SHOWTEXT;
            }
        }
        tbb[nButton].fsState = TBSTATE_ENABLED;
        pButtons = pButtons->tbb_next;
    }
    tb_updateImageList(hwndToolbar, pwImageCodes, nImageIndex);
    SendMessage(hwndToolbar, TB_ADDBUTTONS, (WPARAM) nButtons, (LPARAM) (LPTBBUTTON) tbb);
    for (int i = 0; i < nButtons; i++) {
        if (tbb[i].iString) {
            free((void*)(intptr_t)tbb[i].iString);
        }
    }
    HWND hwndTooltip = (HWND) SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0);
    if (hwndTooltip) {
        theme_enableDarkMode(hwndTooltip);
    }

    free(tbb);
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
    static int direction = 1;

    switch (msg) {
    case WM_CHAR:
        if (wParam == VK_RETURN || wParam == VK_ESCAPE || wParam == VK_UP) {
            return 0;
        }
        nRet = CallWindowProc(oldEditProc, hwnd, msg, wParam, lParam);
        Edit_GetText(hwnd, pszBuf, sizeof pszBuf);
        find_incrementally(pszBuf, RE_IGNCASE | RE_WRAPSCAN, direction, FALSE);
        return nRet;
    case WM_SETFOCUS:
        if (previousFocusWnd == NULL) {
            previousFocusWnd = (HWND)wParam;
        }
        PostMessage(hwnd, EM_SETSEL, 0, -1);
        find_startIncrementalSearch();
        PostMessage(GetParent(hwnd), LAB_CHILD_FOCUS_CHANGE, TRUE, 0);
        break;
    case WM_DESTROY:
        // Hack: this should be performed in the DESTROY of the toolbar.
        free(_currentIconList);
        break;
    case WM_KILLFOCUS:
        PostMessage(GetParent(hwnd), LAB_CHILD_FOCUS_CHANGE, FALSE, 0);
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_RETURN:
            //Do your stuff
            Edit_GetText(hwnd, pszBuf, sizeof pszBuf);
            find_incrementally(pszBuf, RE_IGNCASE| RE_WRAPSCAN, direction, TRUE);
            return 0;
        case VK_DOWN:
        case VK_UP: {
                int nVirtState = GetKeyState(VK_CONTROL);
                if (nVirtState) {
                    direction = wParam == VK_DOWN ? 1 : -1;
                    cust_setPostfixIcon(hwndSearchBarLabel, direction == 1 ? 0 : 1);
                    return 0;
                }
            }
            break;
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
    float nFactor = dpisupport_initScalingFactor(hwndOwner);
    hwndIncrementalSearchField = CreateWindowEx(0, WC_EDIT, "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_NOHIDESEL | ES_WANTRETURN,
        0, 0, (int)(nFactor * 180), CW_USEDEFAULT,
        hwndOwner, (HMENU)IDM_INCREMENTAL_SEARCH,
        hInst, NULL);
    SendMessage(hwndIncrementalSearchField, WM_SETFONT, (WPARAM)cust_getDefaultEditorFont(), 0);
    oldEditProc = (WNDPROC)SetWindowLongPtr(hwndIncrementalSearchField, GWLP_WNDPROC, (LONG_PTR)incrementalSearchEditWndProc);
    action_registerAction(CMD_SEARCH_INCREMENTALLY, CMD_CMDSEQ, (ACTION_BINDING) { tb_enableEntryField, 0L, 0 }, TRUE);
    return hwndIncrementalSearchField;
}


/*
 * Move the input focus to the incremental search UI entry field. 
 */
long long find_initiateIncrementalSearch() {
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
    pszKey = macro_getKeyText(NULL, CMD_SEARCH_INCREMENTALLY);
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
    CHAR_WITH_STYLE searchIcons[] = {
        {FA_ICON_SEARCH, 0},
        {FA_ICON_ARROW_DOWN, 0},
        {FA_ICON_ARROW_UP, 0}
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
    HIMAGELIST hImageListSearch = tb_createImageList(TB_IMAGE_SIZE, RGB(88, 88, 88), searchIcons, DIM(searchIcons));
    HICON icons[DIM(searchIcons)] = { 0 };
    for (int i = 0; i < DIM(searchIcons); i++) {
        icons[i] = ImageList_GetIcon(hImageListSearch, i, ILD_TRANSPARENT);
    }
    hwndSearchBarLabel = cust_createLabeledWindow(hwndRebar, icons, TEXT(szText), hwndEntryField);
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

    rbBand.hwndChild = hwndSearchBarLabel;
    rbBand.cxMinChild = dpisupport_getSize(300);
    rbBand.cxIdeal = dpisupport_getSize(320);
    rbBand.cyMinChild = dpisupport_getSize(25);
    // The default width should be set to some value wider than the text. The entry field itself will expand to fill the band.
    rbBand.cx = rbBand.cxMinChild;

    // Add the band that has the entry field.
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)1, (LPARAM)&rbBand);

    return (hwndRebar);
}





