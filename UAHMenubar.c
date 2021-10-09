
#include <Windows.h>
#include <Uxtheme.h>
#include <vsstyle.h>
#include "uahmenubar.h"
#include "themes.h"

#pragma comment(lib, "uxtheme.lib")

static HTHEME g_menuTheme;

// ugly colors for illustration purposes
static HBRUSH brBarBackground;

static void UAHDrawMenuNCBottomLine(HWND hWnd) {
    MENUBARINFO mbi = { sizeof(mbi) };
    if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi)) {
        return;
    }

    RECT rcClient = { 0 };
    GetClientRect(hWnd, &rcClient);
    MapWindowPoints(hWnd, NULL, (POINT*)&rcClient, 2);

    RECT rcWindow = { 0 };
    GetWindowRect(hWnd, &rcWindow);

    OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

    // the rcBar is offset by the window rect
    RECT rcAnnoyingLine = rcClient;
    rcAnnoyingLine.bottom = rcAnnoyingLine.top;
    rcAnnoyingLine.top--;


    HDC hdc = GetWindowDC(hWnd);
    FillRect(hdc, &rcAnnoyingLine, brBarBackground);
    ReleaseDC(hWnd, hdc);
}

// processes messages related to UAH / custom menubar drawing.
// return true if handled, false to continue with normal processing in your wndproc
BOOL UAHWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr) {
    static HBRUSH brItemBackground;
    static HBRUSH brItemBackgroundHot;
    static HBRUSH brItemBackgroundSelected;
    THEME_DATA* pTheme = theme_getDefault();

    if (brBarBackground == NULL) {
        brBarBackground = CreateSolidBrush(pTheme->th_dialogBackground);
        brItemBackground = CreateSolidBrush(pTheme->th_dialogBackground);
        brItemBackgroundHot = CreateSolidBrush(pTheme->th_dialogHighlight);
        brItemBackgroundSelected = CreateSolidBrush(pTheme->th_dialogMenuHighlight);
    }
    switch (message) {
    case WM_UAHDRAWMENU:
    {
        UAHMENU* pUDM = (UAHMENU*)lParam;
        RECT rc = { 0 };

        // get the menubar rect
        {
            MENUBARINFO mbi = { sizeof(mbi) };
            GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi);

            RECT rcWindow;
            GetWindowRect(hWnd, &rcWindow);

            // the rcBar is offset by the window rect
            rc = mbi.rcBar;
            OffsetRect(&rc, -rcWindow.left, -rcWindow.top);
        }

        FillRect(pUDM->hdc, &rc, brBarBackground);

        return TRUE;
    }
    case WM_UAHDRAWMENUITEM:
    {
        UAHDRAWMENUITEM* pUDMI = (UAHDRAWMENUITEM*)lParam;


        HBRUSH* pbrBackground = &brItemBackground;

        // get the menu item string
        wchar_t menuString[256] = { 0 };
        MENUITEMINFOW mii = { sizeof(mii), MIIM_STRING };
        {
            mii.dwTypeData = menuString;
            mii.cch = (sizeof(menuString) / 2) - 1;

            GetMenuItemInfoW(pUDMI->um.hmenu, pUDMI->umi.iPosition, TRUE, &mii);
        }

        // get the item state for drawing

        DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

        int iTextStateID = 0;
        int iBackgroundStateID = 0;
        {
            if ((pUDMI->dis.itemState & ODS_INACTIVE) | (pUDMI->dis.itemState & ODS_DEFAULT)) {
                // normal display
                iTextStateID = MPI_NORMAL;
                iBackgroundStateID = MPI_NORMAL;
            }
            if (pUDMI->dis.itemState & ODS_HOTLIGHT) {
                // hot tracking
                iTextStateID = MPI_HOT;
                iBackgroundStateID = MPI_HOT;

                pbrBackground = &brItemBackgroundHot;
            }
            if (pUDMI->dis.itemState & ODS_SELECTED) {
                // clicked -- MENU_POPUPITEM has no state for this, though MENU_BARITEM does
                iTextStateID = MPI_HOT;
                iBackgroundStateID = MPI_HOT;

                pbrBackground = &brItemBackgroundSelected;
            }
            if ((pUDMI->dis.itemState & ODS_GRAYED) || (pUDMI->dis.itemState & ODS_DISABLED)) {
                // disabled / grey text
                iTextStateID = MPI_DISABLED;
                iBackgroundStateID = MPI_DISABLED;
            }
            if (pUDMI->dis.itemState & ODS_NOACCEL) {
                dwFlags |= DT_HIDEPREFIX;
            }
        }

        if (!g_menuTheme) {
            g_menuTheme = OpenThemeData(hWnd, L"Menu");
        }

        DTTOPTS opts = { sizeof(opts), DTT_TEXTCOLOR, iTextStateID != MPI_DISABLED ? pTheme->th_dialogForeground : pTheme->th_dialogDisabled};

        FillRect(pUDMI->um.hdc, &pUDMI->dis.rcItem, *pbrBackground);
        DrawThemeTextEx(g_menuTheme, pUDMI->um.hdc, MENU_BARITEM, MBI_NORMAL, menuString, mii.cch, dwFlags, &pUDMI->dis.rcItem, &opts);

        return TRUE;
    }
    case WM_UAHMEASUREMENUITEM:
    {
        *lr = DefWindowProc(hWnd, message, wParam, lParam);
        return TRUE;
    }
    case WM_THEMECHANGED:
    {
        if (g_menuTheme) {
            CloseThemeData(g_menuTheme);
            g_menuTheme = NULL;
        }
        if (brItemBackground) {
            DeleteObject(brItemBackground);
            DeleteObject(brBarBackground);
            DeleteObject(brItemBackgroundHot);
            DeleteObject(brItemBackgroundSelected);
            brBarBackground = NULL;
            brItemBackground = NULL;
            brItemBackgroundHot = NULL;
            brItemBackgroundSelected = NULL;
        }
        // continue processing in main wndproc
        return FALSE;
    }
    case WM_NCPAINT:
    case WM_NCACTIVATE:
        *lr = DefWindowProc(hWnd, message, wParam, lParam);
        UAHDrawMenuNCBottomLine(hWnd);
        return TRUE;
        break;
    default:
        return FALSE;
    }
}

