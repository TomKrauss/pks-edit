/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * MainFrame.c
 *
 * Main Window and of PKS Edit
 *
 * 						created: 10.09.21
 *						Author : Tom
 */

#include "alloc.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "arraylist.h"
#include "edfuncs.h"
#include "winterf.h"
#include "editorconfiguration.h"
#include "stringutil.h"
#include "helpitem.h"
#include "mainframe.h"
#include "themes.h"
#include "codecompletion.h"
#include "winfo.h"
#include "errordialogs.h"
#include "xdialog.h"
#include "windowselector.h"

extern HINSTANCE		hInst;
extern BOOL	bTaskFinished;
extern void 	st_init(HWND hwndDaddy);
extern void		st_redraw(BOOL bErase);
extern void		st_resize(int nStatusHeight, RECT* pRect);
extern void 	status_wh(WORD* width, WORD* height);
extern void 	tb_wh(WORD* width, WORD* height);
extern BOOL 	ww_workWinHasFocus(void);
extern int 		clp_setdata(int whichBuffer);
extern void 	EditDroppedFiles(HDROP hdrop);

/*------------------------------------------------------------
 * EdCloseAll()
 */
extern int EdCloseAll();

/*------------------------------------------------------------
 * FinalizePksEdit()
 *
 * Invoked, when PKS Edit exits to perform final tasks.
 */
extern void FinalizePksEdit(void);

/*
 * Remove all data structures, clear temp files and dump memory for debugging purpose.
 */
extern void main_cleanup(void);

#define CLOSER_SIZE		16
#define CLOSER_DISTANCE	8
typedef struct tagTAB_PAGE {
	int	  tp_index;
	HWND  tp_hwnd;
	int	  tp_width;
} TAB_PAGE;

typedef struct tagTAB_CONTROL {
	ARRAY_LIST* tc_pages;
	int			tc_firstTabOffset;
	int			tc_stripHeight;
	int			tc_firstVisibleTab;
	int			tc_activeTab;
	int			tc_rolloverTab;
} TAB_CONTROL;

const char *szFrameClass = "PKSEditMainFrame";
static const char* szTabClass = "PKSEditTabControl";

static HWND  hwndTabControl;
static HWND  hwndFrameWindow;
static HICON defaultIcon;
static void* _executeKeyBinding;

#define GWLP_TAB_CONTROL		0

static void tabcontrol_repaintTabs(HWND hwnd, TAB_CONTROL* pControl) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	rect.bottom = rect.top + pControl->tc_stripHeight + 1;
	InvalidateRect(hwnd, &rect, TRUE);
}

static void tabcontrol_resizeActiveTab(HWND hwnd, TAB_CONTROL* pControl) {
	if (pControl->tc_activeTab >= 0) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, pControl->tc_activeTab);
		if (pPage->tp_hwnd) {
			RECT rect;
			GetClientRect(hwnd, &rect);
			MoveWindow(pPage->tp_hwnd, rect.left, rect.top + pControl->tc_stripHeight, rect.right - rect.left, rect.bottom - rect.top - pControl->tc_stripHeight, TRUE);
		}
	}
}

static void tabcontrol_scrollTabs(HWND hwnd, TAB_CONTROL* pControl) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	int xMax = rect.right;
	if (pControl->tc_activeTab < pControl->tc_firstVisibleTab) {
		pControl->tc_firstVisibleTab = 0;
	}
	int nLen = (int)arraylist_size(pControl->tc_pages);
	for (int i = 0; i < nLen; i++) {
		int x = pControl->tc_firstTabOffset;
		for (int j = i; j < nLen; j++) {
			TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, j);
			x += pPage->tp_width + 1;
			if (x >= xMax) {
				break;
			}
			if (j == pControl->tc_activeTab) {
				pControl->tc_firstVisibleTab = i;
				i = nLen;
				break;
			}
		}
	}
}

/*
 * Activate a tab with a given index.
 */
static void tabcontrol_selectTab(HWND hwnd, TAB_CONTROL* pControl, int newIdx) {
	TAB_PAGE* pPage;
	if (pControl->tc_activeTab != newIdx) {
		if (pControl->tc_activeTab >= 0) {
			pPage = arraylist_get(pControl->tc_pages, pControl->tc_activeTab);
			if (pPage->tp_hwnd != NULL) {
				ShowWindow(pPage->tp_hwnd, SW_HIDE);
			}
		}
		pControl->tc_activeTab = newIdx;
		if (newIdx >= 0) {
			pPage = arraylist_get(pControl->tc_pages, pControl->tc_activeTab);
			tabcontrol_resizeActiveTab(hwnd, pControl);
			ShowWindow(pPage->tp_hwnd, SW_SHOW);
			SetFocus(pPage->tp_hwnd);
		}
		tabcontrol_scrollTabs(hwnd, pControl);
		tabcontrol_repaintTabs(hwnd, pControl);
	}
}

/*
 * Activate a tab page with the given window handle.
 */
static int tabcontrol_selectPage(HWND hwnd, HWND hwndPage) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwndTabControl, GWLP_TAB_CONTROL);
	size_t len = arraylist_size(pControl->tc_pages);

	for (int i = 0; i < len; i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		if (pPage->tp_hwnd == hwndPage) {
			tabcontrol_selectTab(hwnd, pControl, i);
			return 1;
		}
	}
	return 0;
}

/*
 * Add a new window with a given tab button to select it. 
 */
int tabcontrol_addTab(HWND hwndTabControl, char* pszTitle, HWND hwndTab) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwndTabControl, GWLP_TAB_CONTROL);
	TAB_PAGE* pData = calloc(1, sizeof * pData);
	int idx = (int)arraylist_size(pControl->tc_pages);
	arraylist_add(pControl->tc_pages, pData);
	pData->tp_index = idx;
	pData->tp_hwnd = hwndTab;
	pData->tp_width = 30 + (int)strlen(pszTitle) * 8;
	RECT rect;
	GetClientRect(hwndTab, &rect);
	rect.bottom = rect.top + pControl->tc_stripHeight;
	tabcontrol_selectTab(hwndTabControl, pControl, idx);
	RedrawWindow(hwndTab, &rect, NULL, RDW_ERASE);
	return 1;
}

/*
 * Calculate the rectangle occupied by the closer button. 
 */
static void tabcontrol_getCloserRect(TAB_PAGE* pPage, int x, int height, RECT* pRect) {
	int nCloserSize = CLOSER_SIZE;
	pRect->left = x + pPage->tp_width - CLOSER_SIZE - CLOSER_DISTANCE;
	pRect->top = (height - nCloserSize) / 2 + 2;
	pRect->right = pRect->left + nCloserSize;
	pRect->bottom = pRect->top + nCloserSize;
}

static void tabcontrol_paintCloser(HDC hdc, RECT* pRect, BOOL bRollover) {
	int nDelta = 4;
	THEME_DATA* pTheme = theme_getDefault();
	LOGBRUSH brush;
	brush.lbColor = pTheme->th_dialogBorder;
	brush.lbHatch = 0;
	brush.lbStyle = PS_SOLID;
	HPEN hPen = ExtCreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, 2, &brush, 0, NULL);
	HPEN hPenOld = SelectObject(hdc, hPen);
	MoveToEx(hdc, pRect->left + nDelta, pRect->top + nDelta, NULL);
	LineTo(hdc, pRect->right - nDelta, pRect->bottom - nDelta);
	MoveToEx(hdc, pRect->right - nDelta, pRect->top + nDelta, NULL);
	LineTo(hdc, pRect->left + nDelta, pRect->bottom - nDelta);
	if (bRollover) {
		MoveToEx(hdc, pRect->left, pRect->top, NULL);
		LineTo(hdc, pRect->right, pRect->top);
		LineTo(hdc, pRect->right, pRect->bottom);
		LineTo(hdc, pRect->left, pRect->bottom);
		LineTo(hdc, pRect->left, pRect->top);
	}
	DeleteObject(SelectObject(hdc, hPenOld));
}

static void tabcontrol_paintTab(HDC hdc, TAB_PAGE* pPage, BOOL bSelected, BOOL bRollover, int x, int y, int height) {
	char szBuffer[128];
	THEME_DATA* pTheme = theme_getDefault();
	int nIconSize = 16;
	int nMargin = 4;
	RECT rect;
	
	if (!pPage->tp_hwnd) {
		sprintf(szBuffer, "Buffer %d", pPage->tp_index);
	} else {
		GetWindowText(pPage->tp_hwnd, szBuffer, sizeof szBuffer);
	}
	size_t nLen = strlen(szBuffer);
	SIZE sText;
	HFONT hFont = SelectObject(hdc, theme_createDialogFont(bSelected ? FW_BOLD : FW_NORMAL));
	GetTextExtentPoint(hdc, szBuffer, (int)nLen, &sText);
	pPage->tp_width = sText.cx + nIconSize + 2 * nMargin + CLOSER_SIZE + 2* CLOSER_DISTANCE;
	rect.left = x;
	rect.right = x + pPage->tp_width;
	rect.top = y;
	rect.bottom = y + height;
	HBRUSH hBrush = CreateSolidBrush(bSelected ? pTheme->th_defaultBackgroundColor : pTheme->th_dialogLight);
	HBRUSH hOld = SelectObject(hdc, hBrush);
	if (bSelected) {
		rect.bottom++;
	} else {
		rect.top += 2;
	}
	FillRect(hdc, &rect, hBrush);
	DeleteObject(SelectObject(hdc, hOld));
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, pTheme->th_dialogForeground);
	if (!bSelected) {
		rect.top += 2;
		rect.left += 2;
	}
	int yIcon = (height - nIconSize) / 2;
	HICON hIcon = pPage->tp_hwnd ? (HICON)SendMessage(pPage->tp_hwnd, WM_GETICON, 0, 0L) : (HICON)NULL;
	if (!hIcon) {
		if (defaultIcon == NULL) {
			defaultIcon = LoadIcon(hInst, "EDIT");
		}
		hIcon = defaultIcon;
	}
	DrawIconEx(hdc, rect.left + nMargin, yIcon, hIcon, nIconSize, nIconSize, 0, NULL, DI_NORMAL);
	rect.left += 2 * nMargin + nIconSize;
	DrawText(hdc, szBuffer, (int)nLen, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(SelectObject(hdc, hFont));
	HPEN hPen = CreatePen(PS_SOLID, 1, pTheme->th_dialogBorder);
	HPEN hPenOld = SelectObject(hdc, hPen);
	tabcontrol_getCloserRect(pPage, x, height, &rect);
	tabcontrol_paintCloser(hdc, &rect, bRollover);
	if (!bSelected) {
		y += 2;
		height -= 2;
	}
	MoveToEx(hdc, x, y+height, NULL);
	LineTo(hdc, x, y);
	int x2 = x + pPage->tp_width;
	LineTo(hdc, x2, y);
	LineTo(hdc, x2, y + height);
	DeleteObject(SelectObject(hdc, hPenOld));
	if (bSelected) {
		hPen = CreatePen(PS_SOLID, 2, pTheme->th_dialogActive);
		hPenOld = SelectObject(hdc, hPen);
		MoveToEx(hdc, x, y + 1, NULL);
		LineTo(hdc, x + pPage->tp_width, y+1);
		DeleteObject(SelectObject(hdc, hPenOld));
	}
}

static void tabcontrol_paintTabs(HWND hwnd, PAINTSTRUCT* ps, TAB_CONTROL* pControl) {
	THEME_DATA* pTheme = theme_getDefault();
	HBRUSH hBrush = CreateSolidBrush(pTheme->th_defaultBackgroundColor);
	HBRUSH hBorderBrush = CreateSolidBrush(pTheme->th_dialogBorder);
	RECT rect;
	GetClientRect(hwnd, &rect);
	int xMax = rect.right;
	FillRect(ps->hdc, &rect, hBrush);
	rect.top += pControl->tc_stripHeight;
	FrameRect(ps->hdc, &rect, hBorderBrush);
	rect.top -= pControl->tc_stripHeight;
	int x = rect.left + pControl->tc_firstTabOffset;
	int y = rect.top;
	for (int i = pControl->tc_firstVisibleTab; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		tabcontrol_paintTab(ps->hdc, pPage, i == pControl->tc_activeTab, i == pControl->tc_rolloverTab, x, y, pControl->tc_stripHeight);
		x += pPage->tp_width+1;
		if (x >= xMax) {
			break;
		}
	}
	DeleteObject(hBrush);
	DeleteObject(hBorderBrush);
}

static void tabcontrol_destroy(TAB_CONTROL* pControl) {
	for (int i = 0; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		free(pPage);
	}
	arraylist_destroy(pControl->tc_pages);
	free(pControl);
}

/*
 * Handle tab selection with the mouse. 
 */
static void tabcontrol_handleButtonDown(HWND hwnd, LPARAM lParam) {
	TAB_CONTROL* pControl = (TAB_CONTROL *) GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	if (y >= pControl->tc_stripHeight) {
		return;
	}
	x -= pControl->tc_firstTabOffset;
	for (int i = pControl->tc_firstVisibleTab; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		if (x < 0) {
			break;
		}
		if (x < pPage->tp_width) {
			if (pControl->tc_rolloverTab == i) {
				if (pPage->tp_hwnd) {
					PostMessage(pPage->tp_hwnd, WM_CLOSE, 0, 0l);
				}
			} else {
				tabcontrol_selectTab(hwnd, pControl, i);
			}
			break;
		}
		x -= pPage->tp_width + 1;
	}
}

static void tabcontrol_setRollover(HWND hwnd, TAB_CONTROL* pControl, int nIndex) {
	if (pControl->tc_rolloverTab != nIndex) {
		pControl->tc_rolloverTab = nIndex;
		tabcontrol_repaintTabs(hwnd, pControl);
	}
}

/*
 * Handle mouse move for rollover effects.
 */
static void tabcontrol_handleMouseMove(HWND hwnd, LPARAM lParam) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	POINT p;
	p.x = GET_X_LPARAM(lParam);
	p.y = GET_Y_LPARAM(lParam);
	if (p.y >= pControl->tc_stripHeight) {
		tabcontrol_setRollover(hwnd, pControl, -1);
		return;
	}
	int x = pControl->tc_firstTabOffset;
	for (int i = pControl->tc_firstVisibleTab; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		RECT rect;
		tabcontrol_getCloserRect(pPage, x, pControl->tc_stripHeight, &rect);
		if (p.y > rect.bottom || p.y < rect.top || x > rect.right) {
			break;
		}
		if (PtInRect(&rect, p)) {
			tabcontrol_setRollover(hwnd, pControl, i);
			return;
		}
		x += pPage->tp_width+1;
	}
	tabcontrol_setRollover(hwnd, pControl, -1);
}

static void tabcontrol_closed(HWND hwnd, HWND hwndChild) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	size_t len = arraylist_size(pControl->tc_pages);

	for (int i = 0; i < len; i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		if (pPage->tp_hwnd == hwndChild) {
			arraylist_remove(pControl->tc_pages, pPage);
			free(pPage);
			if (pControl->tc_activeTab == i) {
				pControl->tc_activeTab = -1;
				if (i >= len - 1) {
					i--;
				}
				tabcontrol_selectTab(hwnd, pControl, i);
			} else if (pControl->tc_activeTab > i) {
				pControl->tc_activeTab--;
			}
			break;
		}
	}
	len = arraylist_size(pControl->tc_pages);
	for (int i = 0; i < len; i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		pPage->tp_index = i;
	}
	tabcontrol_repaintTabs(hwnd, pControl);
}
/*
 * Window procedure of the main frame window.
 */
static LRESULT tabcontrol_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	TAB_CONTROL* pControl;
	PAINTSTRUCT ps;

	switch (message) {
	case WM_CREATE:
		pControl = calloc(1, sizeof * pControl);
		pControl->tc_pages = arraylist_create(5);
		pControl->tc_stripHeight = 26;
		pControl->tc_activeTab = -1;
		pControl->tc_firstTabOffset = 5;
		SetWindowLongPtr(hwnd, GWLP_TAB_CONTROL, (LONG_PTR)pControl);
		break;

	case WM_LBUTTONDOWN: 
		tabcontrol_handleButtonDown(hwnd, lParam);
		break;

	case WM_MOUSEMOVE:
		tabcontrol_handleMouseMove(hwnd, lParam);
		break;

	case WM_PAINT:
		pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
		BeginPaint(hwnd, &ps);
		tabcontrol_paintTabs(hwnd, &ps, pControl);
		EndPaint(hwnd, &ps);
		return FALSE;

	case WM_PARENTNOTIFY:
		if (wParam == WM_DESTROY) {
			tabcontrol_closed(hwnd, (HWND)lParam);
			return FALSE;
		}
		break;

	case WM_ERASEBKGND:
		return FALSE;

	case WM_SIZE:
		// (T) optimize
		InvalidateRect(hwnd, NULL, TRUE); 
		pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
		tabcontrol_scrollTabs(hwnd, pControl);
		tabcontrol_resizeActiveTab(hwnd, pControl);
		return TRUE;

	case WM_DESTROY:
		pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
		tabcontrol_destroy(pControl);
		SetWindowLongPtr(hwnd, GWLP_TAB_CONTROL, (LONG_PTR)NULL);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*------------------------------------------------------------
 * ww_onTimerAction()
 * Triggered, when the timer of the frame window "hits".
 */
static void ww_onTimerAction(void) {
	st_redraw(FALSE);
}

/*
 * Add a window with the given window class to our tab control managing the edit windows. 
 */
HWND mainframe_addWindow(const char* pszChildWindowClass, const char* pszTitle, LPVOID lParam) {

	HWND hwnd = CreateWindow(pszChildWindowClass, pszTitle, WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 10, 10, hwndTabControl, NULL, hInst, lParam);
	if (hwnd == NULL) {
		return 0;
	}
	tabcontrol_addTab(hwndTabControl, (char*)pszTitle, hwnd);
	return hwnd;
}

/*
 * Window procedure of the main frame window.
 */
static LRESULT mainframe_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static BOOL			bHelp = FALSE;
	static BOOL			appActivated;
	WORD				fwMenu;
	RECT				rect;
	WORD				w;
	WORD				nFkeyHeight;
	WORD				nStatusHeight;
	WORD				nToolbarHeight;
	int					idCtrl;

	switch (message) {

	case WM_ACTIVATEAPP:
		if ((BOOL)wParam) {
			appActivated = TRUE;
		}
		break;
	case WM_CREATE:
		GetClientRect(hwnd, &rect);
		hwndTabControl = CreateWindow(szTabClass, 
			"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, rect.left, rect.top, rect.right, rect.bottom,
			hwnd, (HMENU)NULL, hInst, NULL);
		fkey_initKeyboardWidget(hwnd);
		st_init(hwnd);
		tb_initRebar(hwnd);
		PostMessage(hwnd, WM_SIZE, 0, 0);
		return 0;

	case WM_MDIACTIVATE:
		tabcontrol_selectPage(hwndTabControl, (HWND)wParam);
		break;

	case WM_ERASEBKGND: {
		THEME_DATA* pTheme = theme_getDefault();
		HDC hdc = (HDC)wParam;
		RECT rc;
		GetClientRect(hwnd, &rc);
		HBRUSH hbrBg = CreateSolidBrush(pTheme->th_mainWindowBackground);
		FillRect(hdc, &rc, hbrBg);
		DeleteObject(hbrBg);
		}
		return 1;

	case WM_DROPFILES:
		EditDroppedFiles((HDROP)wParam);
		return 0;

	case WM_EDWINREORG:
	case WM_MOVE:
	case WM_SIZE:
		if (!IsWindowVisible(hwnd)) {
			return 0;
		}
		GetClientRect(hwnd, &rect);
		tb_wh(&w, &nToolbarHeight);
		status_wh(&w, &nStatusHeight);
		fkey_getKeyboardSize(&w, &nFkeyHeight);
		rect.top = nToolbarHeight;
		rect.bottom -= (nFkeyHeight + nStatusHeight + nToolbarHeight);
		if (hwndRebar && message == WM_SIZE) {
			ShowWindow(hwndRebar, nToolbarHeight ? SW_SHOW : SW_HIDE);
			SendMessage(hwndRebar, message, wParam, lParam);
		}
		if (hwndFkeys && message == WM_SIZE) {
			ShowWindow(hwndFkeys, TRUE);
			MoveWindow(hwndFkeys, rect.left, rect.bottom + rect.top,
				rect.right, nFkeyHeight, 1);
		}
		if (hwndTabControl && message == WM_SIZE) {
			MoveWindow(hwndTabControl, rect.left+3, rect.top+3,
				rect.right - 6, rect.bottom - 6,  1);
		}
		if (message == WM_SIZE) {
			st_resize(nStatusHeight, &rect);
		}
		return 0;

	case WM_ENTERIDLE:
		if (GetKeyState(VK_F1) & 0x8000) {
			if (wParam == MSGF_MENU) {
				bHelp = TRUE;
				PostMessage(hwnd, WM_KEYDOWN, VK_RETURN, 0L);
			}
			else {
				dlg_help();
			}
		}
		break;

	case WM_ACTIVATE:
		fkey_keyModifierStateChanged();
		break;

	case WM_TIMER:
		if (appActivated) {
			appActivated = FALSE;
			ft_checkForChangedFiles();
		}
		ft_triggerAutosaveAllFiles();
		ww_onTimerAction();
		break;
#ifdef DEBUG
	case WM_COMPACTING:
		error_displayAlertDialog("compacting memory");
		break;
#endif

	case WM_WININICHANGE:
		string_initDateformats();
		break;

	case WM_MENUSELECT:
		fwMenu = GET_WM_MENUSELECT_FLAGS(wParam, lParam);
		if (!GET_WM_MENUSELECT_HMENU(wParam, lParam) &&
			(WORD)GET_WM_MENUSELECT_FLAGS(wParam, lParam) == 0xFFFF) {
			/* Menu closed */
			macro_showHelpForMenu(-1);
		}
		else if ((fwMenu & (MF_SYSMENU | MF_SEPARATOR | MF_POPUP)) == 0) {
			macro_showHelpForMenu((int)GET_WM_MENUSELECT_CMD(wParam, lParam));
		}
		break;

	case WM_INITMENUPOPUP:
		if (!(BOOL)HIWORD(lParam)) {
			macro_assignAcceleratorTextOnMenu((HMENU)wParam);
		}
		break;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case TTN_NEEDTEXT: {
			LPTOOLTIPTEXT lpttt;

			lpttt = (LPTOOLTIPTEXT)lParam;
			lpttt->hinst = 0;
			lpttt->lpszText = macro_getMenuTooltTip((int)lpttt->hdr.idFrom);
		}
		case TBN_QUERYINSERT:
		case TBN_QUERYDELETE:
			return TRUE;
			break;
		}
		break;
	case WM_COMMAND:
		idCtrl = (int)(short)GET_WM_COMMAND_ID(wParam, lParam);
		if (idCtrl < 0) {
			break;
		}
		if (idCtrl == IDM_INCREMENTAL_SEARCH) {
			break;
		}
		wParam = macro_translateToOriginalMenuIndex(idCtrl);
		if (bHelp) {
			bHelp = FALSE;
			return EdHelpContext((DWORD)wParam);
		}
		if (macro_onMenuAction((int)wParam)) {
			return 1;
		}
		if (!IsWindow(hwnd) || !IsWindow(hwndMain)) {
			return 0;
		}
		break;

	case WM_PKSKEY:
		error_closeErrorWindow();
		macro_onKeyPressed(_executeKeyBinding);
		return 0;

	case WM_PKSOPTOGGLE:
		op_onOptionWidgetSelected((int)wParam);
		return 0;

	case WM_CHAR:
		error_closeErrorWindow();
		macro_onCharacterInserted((WORD)wParam);
		return 0;

	case WM_RENDERALLFORMATS:
		OpenClipboard(hwnd);
		EmptyClipboard();
		clp_setdata(0);
		CloseClipboard();
		return 0;

	case WM_RENDERFORMAT:
		if (wParam == CF_TEXT) {
			clp_setdata(0);
		}
		return 0;

	case WM_TASKFINISHED:
		bTaskFinished = TRUE;
		return 0;

	case WM_QUERYENDSESSION:
		DestroyWindow(hwnd);
		break;

	case WM_CLOSE:
		FinalizePksEdit();
		EdCloseAll();
		if (ww_getNumberOfOpenWindows()) {
			return 0;
		}
		main_cleanup();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
 * Register the main wind 
 */
int mainframe_registerWinClass() {
	return 
		win_registerWindowClass((char*)szTabClass, tabcontrol_windowProc,
			NULL, GetSysColorBrush(COLOR_3DFACE), NULL, sizeof (TAB_CONTROL*)) &&
		win_registerWindowClass((char*)szFrameClass, mainframe_windowProc,
			NULL, GetSysColorBrush(COLOR_3DFACE), "APP_ICON", 0);
}

/*
 * Open the Mainframe window. 
 */
HWND mainframe_open(int nInstanceCount, HMENU hDefaultMenu) {
	char szTitle[1200];
	DWORD dwStyle;

	if (nInstanceCount > 1) {
		wsprintf(szTitle, "* PKS EDIT * (%d)", nInstanceCount);
	}
	else {
		lstrcpy(szTitle, "PKS EDIT");
	}
	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	hwndFrameWindow = CreateWindow(szFrameClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, hDefaultMenu, hInst, 0);

	if (!hwndFrameWindow) {
		return 0;
	}
	ShowWindow(hwndFrameWindow, SW_SHOW);
	return hwndFrameWindow;
}

/*
 * Checks, whether there is an active PKS edit window already somewhere on the screen (maybe other process).
 */
BOOL mainframe_findExistingWindow() {
	return FindWindow(szFrameClass, 0) != 0;
}

/*------------------------------------------------------------
 * TranslatePksAccel()
 */
int _translatekeys = 1;
static int TranslatePksAccel(HWND hwnd, MSG* msg) {
	switch (msg->message) {
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYUP:
	case WM_KEYDOWN:
		if (msg->wParam == VK_MENU ||
			msg->wParam == VK_CONTROL ||
			msg->wParam == VK_SHIFT) {
			fkey_keyModifierStateChanged();
			break;
		}
		if (!ww_workWinHasFocus()) {
			if (!(msg->hwnd == hwndMain || GetKeyState(VK_CONTROL) < 0)) {
				break;
			}
		}
		WINFO* wp = ww_getCurrentEditorWindow();
		if (wp && wp->codecomplete_handle && codecomplete_processKey(wp->codecomplete_handle, msg->message, msg->wParam)) {
			return 1;
		}
		if ((_executeKeyBinding = macro_getKeyBinding(msg->wParam)) != 0) {
			if (msg->message == WM_SYSKEYDOWN ||
				msg->message == WM_KEYDOWN) {
				msg->message = WM_PKSKEY;
				msg->hwnd = hwnd;
				break;
			}
			return 1;
		}
	}
	return 0;
}

/*
 * Process the main messages in the mainframe window (in particular keyboard messages etc...). 
 */
int mainframe_messageLoop() {
	MSG msg;

	while (GetMessage(&msg, 0, 0, 0)) {
		if (windowselector_isHandle(msg.hwnd)) {
			DispatchMessage(&msg);
			continue;
		}
		if (!_translatekeys || !TranslatePksAccel(hwndMain, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)(msg.wParam);
}

/*
 * To be invoked, when the window of a child window changes.
 */
void mainframe_windowTitleChanged() {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwndTabControl, GWLP_TAB_CONTROL);
	if (pControl) {
		tabcontrol_repaintTabs(hwndTabControl, pControl);
	}
}

/*-----------------------------------------------------------
 * mainframe_enumChildWindows()
 * Execute a function for every child window optionally 
 * passing an lParam. If the called function returns 0, the enumeration
 * stops.
 */
int mainframe_enumChildWindows(int (*funcp)(), LONG lParam) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwndTabControl, GWLP_TAB_CONTROL);
	int ret;

	while (arraylist_size(pControl->tc_pages) > 0) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, 0);
		if (pPage->tp_hwnd && (ret = (*funcp)(pPage->tp_hwnd, lParam)) == 0) {
			break;
		}
	}

	return ret;
}

