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
#include <math.h>
#include "arraylist.h"
#include "trace.h"
#include "linkedlist.h"
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
#include "mouseutil.h"
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

#define CLOSER_SIZE			16
#define CLOSER_DISTANCE		8

typedef enum { DS_EDIT_WINDOW, DS_OTHER } DOCKING_SLOT_TYPE;

typedef struct tagDOCKING_SLOT {
	struct tagDOCKING_SLOT* ds_next;
	DOCKING_SLOT_TYPE ds_type;				// The type of windows displayed in this slot
	HWND ds_hwnd;							// The window in this slot.
	float ds_xratio;
	float ds_yratio;
	float ds_wratio;
	float ds_hratio;
	char ds_name[20];
} DOCKING_SLOT;

typedef struct tagTAB_PAGE {
	HWND  tp_hwnd;
	int	  tp_width;
} TAB_PAGE;

typedef struct tagTAB_CONTROL {
	ARRAY_LIST* tc_pages;
	HWND		tc_hwnd;
	int			tc_firstTabOffset;
	int			tc_stripHeight;
	int			tc_firstVisibleTab;
	int			tc_activeTab;
	int			tc_rolloverTab;
} TAB_CONTROL;

const char *szFrameClass = "PKSEditMainFrame";
static const char* szTabClass = "PKSEditTabControl";
static const char* szDragProxyClass = "PKSDragProxyControl";

static TAB_CONTROL* currentDropTarget;
static DOCKING_SLOT* dockingSlots;
static HWND  hwndFrameWindow;
static HICON defaultIcon;
static void* _executeKeyBinding;
static char* szDefaultSlotName = "default";


static DOCKING_SLOT* mainframe_addDockingSlot(DOCKING_SLOT_TYPE dsType, HWND hwnd, char* pszName, float xRatio, float yRatio, float wRatio, float hRatio);

/*
 * Resize our "docking slots" depending on their given ratios
 */
static void mainframe_arrangeDockingSlots(HWND hwnd);
/*
 * Paint one tab of the tabstrip of our tab control displaying the edit tabs.
 */
static void tabcontrol_paintTab(HDC hdc, TAB_PAGE* pPage, BOOL bSelected, BOOL bRollover, int x, int y, int height);

/*
 * Move a tab from one tab control to another.
 */
static void tabcontrol_moveTab(TAB_CONTROL* pSource, TAB_CONTROL* pTarget, TAB_PAGE* pPage);

#define GWLP_TAB_CONTROL		0

/*
 * Paint the drag proxy. 
 */
static void dragproxy_paint(HWND hwnd, PAINTSTRUCT* ps) {
	TAB_PAGE *pPage = (TAB_PAGE*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	RECT rect;
	GetClientRect(hwnd, &rect);
	tabcontrol_paintTab(ps->hdc, pPage, TRUE, FALSE, 0, rect.top, rect.bottom - rect.top);
}

/*
 * Window procedure of the drag proxy window, which represents a dragged tab.
 */
static LRESULT dragproxy_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;

	switch (message) {
	case WM_CREATE: {
		LPCREATESTRUCT pStruct = (LPCREATESTRUCT)lParam;
		SetWindowLongPtr(hwnd, GWLP_TAB_CONTROL, (LONG_PTR)pStruct->lpCreateParams);
		break;
	}

	case WM_NCHITTEST:
		return HTTRANSPARENT;

	case WM_PAINT:
		BeginPaint(hwnd, &ps);
		dragproxy_paint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return FALSE;

	case WM_ERASEBKGND:
		return FALSE;

	case WM_DESTROY:
		SetWindowLongPtr(hwnd, GWLP_TAB_CONTROL, (LONG_PTR)NULL);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
 * Opens a drag-proxy for a tab to be dragged around the screen. 
 */
static HWND dragproxy_open(RECT* pRect, TAB_PAGE *pPage) {
	static HWND hwndProxy;

	if (hwndProxy == NULL) {
		hwndProxy = CreateWindow(szDragProxyClass, "", WS_CHILD, 0, 0, 10, 10, hwndFrameWindow, NULL, hInst, pPage);
		SetWindowPos(hwndProxy, HWND_TOPMOST, 0, 0, 0, 0, 0);
	} else {
		SetWindowLongPtr(hwndProxy, GWLP_TAB_CONTROL, (LONG_PTR)pPage);
	}
	MoveWindow(hwndProxy, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, TRUE);
	ShowWindow(hwndProxy, SW_SHOW);
	return hwndProxy;
}

/*
 * Check, whether the dock displaying the HWND is closeable.
 * If it is the last edit dock, it cannot be closed.
 */
static BOOL mainframe_isCloseableDock(HWND hwnd) {
	DOCKING_SLOT* pSlot = dockingSlots;
	int nEditCount = 0;
	while (pSlot) {
		if (pSlot->ds_type == DS_EDIT_WINDOW) {
			nEditCount++;
		} else if (pSlot->ds_hwnd == hwnd) {
			return TRUE;
		}
		pSlot = pSlot->ds_next;
	}
	return nEditCount > 1;
}

/*
 * Returns a docking slot with a given name or NULL if the slot does not exist.
 */
static DOCKING_SLOT* mainframe_getSlot(const char* pszName) {
	DOCKING_SLOT* pSlot = dockingSlots;
	if (pszName == NULL) {
		pszName = szDefaultSlotName;
	}
	while (pSlot) {
		if (strcmp(pszName, pSlot->ds_name) == 0) {
			return pSlot;
		}
		pSlot = pSlot->ds_next;
	}
	return NULL;
}

/*
 * Resize the default edit docking slots when  a new docking slot is created or when a docking slot is closed.
 */
static void mainframe_applyDefaultSlotSizes() {
	DOCKING_SLOT* pDefaultSlot = mainframe_getSlot(szDefaultSlotName);
	DOCKING_SLOT* pRightSlot = mainframe_getSlot(DOCK_NAME_RIGHT);
	DOCKING_SLOT* pBottomSlot = mainframe_getSlot(DOCK_NAME_BOTTOM);

	float height = pBottomSlot ? 0.5f : 1.0f;
	if (pDefaultSlot != NULL) {
		pDefaultSlot->ds_xratio = 0;
		pDefaultSlot->ds_yratio = 0;
		pDefaultSlot->ds_hratio = height;
		pDefaultSlot->ds_wratio = pRightSlot != NULL ? 0.5f : 1.0f;
	}
	if (pRightSlot != NULL) {
		pRightSlot->ds_wratio = pDefaultSlot != NULL ? 0.5f : 1.0f;
		pRightSlot->ds_xratio = pDefaultSlot != NULL ? 0.5f : 0.0f;
		pRightSlot->ds_yratio = 0;
		pRightSlot->ds_hratio = height;
	}
	if (pBottomSlot != NULL) {
		pBottomSlot->ds_wratio = 1;
		pBottomSlot->ds_hratio = (pDefaultSlot != NULL || pRightSlot != NULL) ? 0.5f : 1.0f;
		pBottomSlot->ds_xratio = 0;
		pBottomSlot->ds_yratio = (pDefaultSlot != NULL || pRightSlot != NULL) ? 0.5f : 0.0f;
	}
}

/*
 * Restore the docking layout, when the mainframe is created. 
 */
static void mainframe_readDocks(HWND hwnd) {
	char szName[32];
	float x, y, w, h;

	for (int i = 0; 1; i++) {
		if (!prof_readDockingPlacement(i, szName, &x, &y, &w, &h)) {
			break;
		}
		mainframe_addDockingSlot(DS_EDIT_WINDOW, hwnd, szName, x, y, w, h);
	}
	if (!dockingSlots) {
		mainframe_addDockingSlot(DS_EDIT_WINDOW, hwnd, szDefaultSlotName, 0.0, 0.0, 1.0f, 1.0f);
	}
}

/*
 * Saves all docking window positions.
 */
static void mainframe_saveDocks() {
	DOCKING_SLOT* pSlot = dockingSlots;
	for (int i = 0; pSlot != NULL; i++, pSlot = pSlot->ds_next) {
		if (!prof_saveDockingPlacement(i, pSlot->ds_name, pSlot->ds_xratio, pSlot->ds_yratio, pSlot->ds_wratio, pSlot->ds_hratio)) {
			break;
		}
	}
}

/*
 * Close the dock containing the window passed. If the dock closed is an edit dock,
 * migrate all child windows of that dock to another dock.
 */
static void mainframe_closeDock(HWND hwnd) {
	DOCKING_SLOT* pSlot = dockingSlots;
	while (pSlot) {
		if (pSlot->ds_hwnd == hwnd) {
			break;
		}
		pSlot = pSlot->ds_next;
	}
	if (pSlot == NULL) {
		return;
	}
	EdTRACE(log_errorArgs(DEBUG_TRACE, "Closing dock %s", pSlot->ds_name));
	DOCKING_SLOT* pSlotUpdate = dockingSlots;
	DOCKING_SLOT* pSlotDefault = dockingSlots;
	while (pSlotUpdate) {
		if (pSlotUpdate == pSlot) {
			pSlotUpdate = pSlotUpdate->ds_next;
			continue;
		}
		if (strcmp(szDefaultSlotName, pSlotUpdate->ds_name) == 0 || (pSlotUpdate->ds_type == DS_EDIT_WINDOW && pSlotDefault == NULL)) {
			pSlotDefault = pSlotUpdate;
		}
		pSlotUpdate = pSlotUpdate->ds_next;
	}
	if (pSlot->ds_type == DS_EDIT_WINDOW) {
		// Migrate contained windows to other dock.
		TAB_CONTROL* pSource = (TAB_CONTROL*) GetWindowLongPtr(pSlot->ds_hwnd, GWLP_TAB_CONTROL);
		TAB_CONTROL* pTarget = (TAB_CONTROL*) GetWindowLongPtr(pSlotDefault->ds_hwnd, GWLP_TAB_CONTROL);
		if (pSource && pTarget) {
			while (pSource->tc_activeTab >= 0) {
				tabcontrol_moveTab(pSource, pTarget, arraylist_get(pSource->tc_pages, pSource->tc_activeTab));
			}
		}
	}
	DestroyWindow(hwnd);
	ll_delete(&dockingSlots, pSlot);
	mainframe_applyDefaultSlotSizes();
	mainframe_arrangeDockingSlots(hwndFrameWindow);
	mainframe_saveDocks();
}

/*
 * Trigger a repaint of the tabstrip of a tab control. 
 */
static void tabcontrol_repaintTabs(HWND hwnd, TAB_CONTROL* pControl) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	rect.bottom = rect.top + pControl->tc_stripHeight + 1;
	InvalidateRect(hwnd, &rect, TRUE);
}

/*
 * Adjust the size of the tab container when the window size has changed.
 */
static void tabcontrol_resizeActiveTab(HWND hwnd, TAB_CONTROL* pControl) {
	if (pControl->tc_activeTab >= 0) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, pControl->tc_activeTab);
		if (pPage->tp_hwnd) {
			RECT rect;
			GetClientRect(hwnd, &rect);
			MoveWindow(pPage->tp_hwnd, rect.left+1, rect.top + pControl->tc_stripHeight+1, 
					rect.right - rect.left-2, rect.bottom - rect.top - pControl->tc_stripHeight-2, TRUE);
		}
	}
}

/*
 * "Scroll" the tabs to make the active tab fully visible. 
 */
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
	HWND hwndOld = NULL;
	if (pControl->tc_activeTab != newIdx) {
		if (pControl->tc_activeTab >= 0) {
			pPage = arraylist_get(pControl->tc_pages, pControl->tc_activeTab);
			hwndOld = pPage->tp_hwnd;
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
	if (hwndOld != NULL) {
		ShowWindow(hwndOld, SW_HIDE);
	}
}

/*
 * Activate a tab page with the given window handle.
 */
static int tabcontrol_selectPage(HWND hwnd, HWND hwndPage) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
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
static int tabcontrol_addTab(HWND hwnd, char* pszTitle, HWND hwndTab) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	TAB_PAGE* pData = calloc(1, sizeof * pData);
	int idx = (int)arraylist_size(pControl->tc_pages);
	arraylist_add(pControl->tc_pages, pData);
	pData->tp_hwnd = hwndTab;
	pData->tp_width = 30 + (int)strlen(pszTitle) * 8;
	RECT rect;
	GetClientRect(hwndTab, &rect);
	rect.bottom = rect.top + pControl->tc_stripHeight;
	tabcontrol_selectTab(hwnd, pControl, idx);
	RedrawWindow(hwndTab, &rect, NULL, RDW_ERASE);
	return 1;
}

/*
 * Add a new window with a given tab button to select it.
 */
static void tabcontrol_removeTab(TAB_CONTROL *pControl, TAB_PAGE *pPage) {
	arraylist_remove(pControl->tc_pages, pPage);
	free(pPage);
	size_t nPages = arraylist_size(pControl->tc_pages);
	pControl->tc_activeTab = -1;
	tabcontrol_selectTab(pControl->tc_hwnd, pControl, (int)(nPages-1));
	tabcontrol_repaintTabs(pControl->tc_hwnd, pControl);
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

/*
 * Paint a closer button. Maybe the whole logic should be moved to an own control.
 */
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

/*
 * Paint one tab of the tabstrip of our tab control displaying the edit tabs. 
 */
static void tabcontrol_paintTab(HDC hdc, TAB_PAGE* pPage, BOOL bSelected, BOOL bRollover, int x, int y, int height) {
	char szBuffer[128];
	THEME_DATA* pTheme = theme_getDefault();
	int nIconSize = 16;
	int nMargin = 4;
	RECT rect;
	
	if (!pPage->tp_hwnd) {
		sprintf(szBuffer, "Page");
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
		LOGBRUSH brush;
		brush.lbColor = pTheme->th_dialogActive;
		brush.lbHatch = 0;
		brush.lbStyle = PS_SOLID;
		hPen = ExtCreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, 2, &brush, 0, NULL);
		hPenOld = SelectObject(hdc, hPen);
		MoveToEx(hdc, x + 1, y + 1, NULL);
		LineTo(hdc, x + pPage->tp_width, y+1);
		DeleteObject(SelectObject(hdc, hPenOld));
	}
}

/*
 * Returns TRUE, if the dock containing the tab control is closeable and return the 
 * bounds of the closer button the rect passed. If the tab is not closable return FALSE.
 */
static BOOL tabcontrol_getDockingCloserRect(HWND hwndTabControl, TAB_CONTROL* pControl, RECT* pRect) {
	if (!mainframe_isCloseableDock(hwndTabControl)) {
		return FALSE;
	}

	GetClientRect(hwndTabControl, pRect);
	pRect->right -= CLOSER_DISTANCE;
	pRect->left = pRect->right - CLOSER_SIZE;
	int dDelta = (pControl->tc_stripHeight - CLOSER_SIZE) / 2;
	pRect->top = dDelta;
	pRect->bottom = pRect->top + CLOSER_SIZE;
	return TRUE;
}

/*
 * Repaint the tab strip of a tab control including the tabs, an optional docking close button etc.
 */
static void tabcontrol_paintTabs(HWND hwnd, PAINTSTRUCT* ps, TAB_CONTROL* pControl) {
	THEME_DATA* pTheme = theme_getDefault();
	HBRUSH hBrush = CreateSolidBrush(pTheme->th_defaultBackgroundColor);
	HBRUSH hBorderBrush = CreateSolidBrush(pTheme->th_dialogBorder);
	RECT rect;
	RECT closerRect;
	GetClientRect(hwnd, &rect);
	BOOL bMayCloseDock = tabcontrol_getDockingCloserRect(hwnd, pControl, &closerRect);
	int xMax = rect.right;
	if (bMayCloseDock) {
		xMax -= CLOSER_SIZE - 2*CLOSER_DISTANCE;
	}
	FillRect(ps->hdc, &rect, hBrush);
	if (pControl == currentDropTarget) {
		HPEN hPen = CreatePen(PS_SOLID, 3, pTheme->th_dialogActive);
		HPEN hPenOld = SelectObject(ps->hdc, hPen);
		MoveTo(ps->hdc, rect.left, rect.top+1);
		LineTo(ps->hdc, rect.right-1, rect.top+1);
		LineTo(ps->hdc, rect.right-1, rect.bottom);
		DeleteObject(SelectObject(ps->hdc, hPenOld));
	}
	rect.top += pControl->tc_stripHeight;
	FrameRect(ps->hdc, &rect, hBorderBrush);
	rect.top -= pControl->tc_stripHeight;
	int x = rect.left + pControl->tc_firstTabOffset;
	int y = rect.top;
	for (int i = pControl->tc_firstVisibleTab; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		if (i > pControl->tc_firstVisibleTab && x + pPage->tp_width >= xMax) {
			break;
		}
		tabcontrol_paintTab(ps->hdc, pPage, i == pControl->tc_activeTab, i == pControl->tc_rolloverTab, x, y, pControl->tc_stripHeight);
		x += pPage->tp_width + 1;
	}
	if (bMayCloseDock) {
		tabcontrol_paintCloser(ps->hdc, &closerRect, FALSE);
	}
	DeleteObject(hBrush);
	DeleteObject(hBorderBrush);
}

/*
 * Free the data structure occupied by a tab control. 
 */
static void tabcontrol_destroy(TAB_CONTROL* pControl) {
	for (int i = 0; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		free(pPage);
	}
	arraylist_destroy(pControl->tc_pages);
	free(pControl);
}

/*
 * Mark a tab control as drop target.
 */
static void tabcontrol_setAcceptDrop(HWND hwnd, BOOL aFlag) {
	TAB_CONTROL* pControl = hwnd == NULL ? NULL: (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	if (pControl == currentDropTarget) {
		return;
	}
	if (currentDropTarget) {
		InvalidateRect(currentDropTarget->tc_hwnd, NULL, TRUE);
	}
	if (pControl) {
		InvalidateRect(pControl->tc_hwnd, NULL, TRUE);
	}
	currentDropTarget = pControl;
}

/*
 * Check, whether a new dock tab control is under the mouse cursor during a drag op. 
 */
static void tabcontrol_dragOver(TAB_CONTROL* pSource) {
	POINT point;
	HWND hwnd;
	char szName[40];

	GetCursorPos(&point);
	hwnd = WindowFromPoint(point);
	while (hwnd) {
		GetClassName(hwnd, szName, sizeof szName);
		if (strcmp(szName, szTabClass) == 0) {
			TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
			if (pControl != pSource) {
				tabcontrol_setAcceptDrop(hwnd, TRUE);
				return;
			}
			break;
		}
		hwnd = GetParent(hwnd);
	}
	tabcontrol_setAcceptDrop(NULL, FALSE);
}

/*
 * Move a tab from one tab control to another. 
 */
static void tabcontrol_moveTab(TAB_CONTROL* pSource, TAB_CONTROL*pTarget, TAB_PAGE* pPage) {
	SetParent(pPage->tp_hwnd, pTarget->tc_hwnd);
	tabcontrol_addTab(pTarget->tc_hwnd, "", pPage->tp_hwnd);
	tabcontrol_removeTab(pSource, pPage);
}

/*
 * A tab of a tab control was dropped on another tab control. Migrate the tab to the new control.
 * if 'bAccept' is FALSE, cancel the operation.
 */
static void tabcontrol_drop(TAB_CONTROL* pSource, TAB_PAGE *pDroppedPage, BOOL bAccept) {
	if (bAccept && currentDropTarget) {
		tabcontrol_moveTab(pSource, currentDropTarget, pDroppedPage);
	}
	tabcontrol_setAcceptDrop(NULL, FALSE);
}

/*
 * Drag a tab across the screen to a potential new tab. 
 */
static BOOL tabcontrol_dragTab(HWND hwnd, TAB_CONTROL* pControl, TAB_PAGE* pPage, int x, int y) {
	int b = 1;
	int dummy;
	HWND hwndProxy;

	if (DragDetect(hwnd, (POINT) { x, y })) {
		SetCapture(hwnd);
		HCURSOR hCursor = LoadCursor(0, IDC_SIZEALL);
		SetCursor(hCursor);
		RECT rect;
		rect.left = x;
		rect.top = y;
		rect.right = pPage->tp_width + rect.left;
		rect.bottom = rect.top + pControl->tc_stripHeight;
		hwndProxy = dragproxy_open(&rect, pPage);
		while (mouse_dispatchUntilButtonRelease(&x, &y, &b, &dummy) && b) {
			POINT p = { x,y };
			ClientToScreen(hwnd, &p);
			ScreenToClient(hwndFrameWindow, &p);
			x = p.x;
			y = p.y;
			MoveWindow(hwndProxy, x - pPage->tp_width / 2, y - pControl->tc_stripHeight / 2, pPage->tp_width, pControl->tc_stripHeight, TRUE);
			InvalidateRect(hwndProxy, NULL, FALSE);
			tabcontrol_dragOver(pControl);
		}
		tabcontrol_drop(pControl, pPage, TRUE);
		ShowWindow(hwndProxy, SW_HIDE);
		mouse_setArrowCursor();
		DeleteObject(hCursor);
		ReleaseCapture();
		return TRUE;
	}
	return FALSE;
}

/*
 * Handle tab selection with the mouse. 
 */
static void tabcontrol_handleButtonDown(HWND hwnd, LPARAM lParam, BOOL bDrag) {
	TAB_CONTROL* pControl = (TAB_CONTROL *) GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	if (y >= pControl->tc_stripHeight) {
		return;
	}
	RECT closerRect;
	if (tabcontrol_getDockingCloserRect(hwnd, pControl, &closerRect) && PtInRect(&closerRect, (POINT) { x, y }) && !bDrag) {
		mainframe_closeDock(hwnd);
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
				if (bDrag) {
					tabcontrol_dragTab(hwnd, pControl, pPage, x, y);
				} else {
					tabcontrol_selectTab(hwnd, pControl, i);
				}
			}
			break;
		}
		x -= pPage->tp_width + 1;
	}
}

static void tabcontrol_setRollover(HWND hwnd, TAB_CONTROL* pControl, int nIndex) {
	if (pControl->tc_rolloverTab != nIndex) {
		pControl->tc_rolloverTab = nIndex;
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		if (nIndex > 0) {
			tme.dwFlags = TME_LEAVE;
		} else {
			tme.dwFlags = 0;
		}
		TrackMouseEvent(&tme);
		tabcontrol_repaintTabs(hwnd, pControl);
	}
}

/*
 * Handle mouse move for rollover effects.
 */
static void tabcontrol_handleMouseMove(HWND hwnd, POINT p) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
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
	tabcontrol_repaintTabs(hwnd, pControl);
}

/*
 * Window procedure of the main frame window.
 */
static LRESULT tabcontrol_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	TAB_CONTROL* pControl;
	PAINTSTRUCT ps;
	static POINT ptDown;

	switch (message) {
	case WM_CREATE:
		pControl = calloc(1, sizeof * pControl);
		pControl->tc_pages = arraylist_create(5);
		pControl->tc_stripHeight = 26;
		pControl->tc_activeTab = -1;
		pControl->tc_rolloverTab = -1;
		pControl->tc_firstTabOffset = 5;
		pControl->tc_hwnd = hwnd;
		SetWindowLongPtr(hwnd, GWLP_TAB_CONTROL, (LONG_PTR)pControl);
		break;

	case WM_LBUTTONDOWN:
		tabcontrol_handleButtonDown(hwnd, lParam, FALSE);
		ptDown.x = GET_X_LPARAM(lParam);
		ptDown.y = GET_Y_LPARAM(lParam);
		break;

	case WM_MOUSELEAVE:
		tabcontrol_handleMouseMove(hwnd, (POINT){ 0,0 });
		break;

	case WM_MOUSEMOVE: {
		POINT p;
		p.x = GET_X_LPARAM(lParam);
		p.y = GET_Y_LPARAM(lParam);
		if (wParam & MK_LBUTTON && (abs(p.x - ptDown.x) > 1 || abs(p.y - ptDown.y) > 1)) {
			if (GetCapture() != hwnd) {
				tabcontrol_handleButtonDown(hwnd, lParam, TRUE);
			}
		} else {
			tabcontrol_handleMouseMove(hwnd, p);
		}
		break;
	}

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
HWND mainframe_addWindow(const char*pszPreferredSlot, const char* pszChildWindowClass, const char* pszTitle, LPVOID lParam) {
	DOCKING_SLOT* pSlot = mainframe_getSlot(pszPreferredSlot);
	if (pSlot == NULL) {
		if (pszPreferredSlot != NULL && strcmp(DOCK_NAME_RIGHT, pszPreferredSlot)) {
			// TODO: add the requested docking slot 
		}
		pSlot = dockingSlots;
	}
	HWND hwnd = CreateWindow(pszChildWindowClass, pszTitle, WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 10, 10, pSlot->ds_hwnd, NULL, hInst, lParam);
	if (hwnd == NULL) {
		return 0;
	}
	tabcontrol_addTab(pSlot->ds_hwnd, (char*)pszTitle, hwnd);
	return hwnd;
}

static DOCKING_SLOT* mainframe_addDockingSlot(DOCKING_SLOT_TYPE dsType, HWND hwnd, char* pszName, float xRatio, float yRatio, float wRatio, float hRatio) {
	HWND hwndTab = CreateWindow(szTabClass,
		"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 10, 10,
		hwnd, (HMENU)NULL, hInst, NULL);
	if (!hwndTab) {
		return NULL;
	}
	DOCKING_SLOT* pSlot = ll_insert(&dockingSlots, sizeof(*dockingSlots));
	strcpy(pSlot->ds_name, pszName);
	pSlot->ds_type = dsType;
	pSlot->ds_hratio = hRatio;
	pSlot->ds_wratio = wRatio;
	pSlot->ds_xratio = xRatio;
	pSlot->ds_yratio = yRatio;
	pSlot->ds_hwnd = hwndTab;
	return pSlot;
}

/*
 * Find the docking parent window for a given editor window.
 */
static DOCKING_SLOT* mainframe_getDockingParent(HWND hwnd) {
	DOCKING_SLOT* pSlot = dockingSlots;
	while (pSlot) {
		if (pSlot->ds_type == DS_EDIT_WINDOW) {
			if (GetParent(hwnd) == pSlot->ds_hwnd) {
				return pSlot;
			}
		}
		pSlot = pSlot->ds_next;
	}
	return NULL;
}

/*
 * Returns the name of the docking slot in which the passed editor window is docked.
 */
char* mainframe_getDockNameFor(HWND hwnd) {
	DOCKING_SLOT* pSlot = mainframe_getDockingParent(hwnd);
	return pSlot != NULL ? pSlot->ds_name : szDefaultSlotName;
}

/*
 * Resize our "docking slots" depending on their given ratios 
 */
static void mainframe_arrangeDockingSlots(HWND hwnd) {
	RECT rect;
	WORD nWidth;
	WORD nToolbarHeight;
	WORD nStatusHeight;
	WORD nFkeyHeight;

	GetClientRect(hwnd, &rect);
	tb_wh(&nWidth, &nToolbarHeight);
	status_wh(&nWidth, &nStatusHeight);
	fkey_getKeyboardSize(&nWidth, &nFkeyHeight);
	rect.top = nToolbarHeight;
	rect.bottom -= (nFkeyHeight + nStatusHeight);
	DOCKING_SLOT* pSlot = dockingSlots;
	int dDelta = 3;
	int width = rect.right - rect.left - dDelta;
	int height = rect.bottom - rect.top - dDelta;
	while (pSlot) {
		MoveWindow(pSlot->ds_hwnd, rect.left + dDelta + (int)(width * pSlot->ds_xratio), rect.top + dDelta + (int)(height * pSlot->ds_yratio),
			rect.left + (int)(width * pSlot->ds_wratio) - dDelta, (int)(height * pSlot->ds_hratio) - dDelta, 1);
		pSlot = pSlot->ds_next;
	}
}

/*
 * Window procedure of the main frame window.
 */
static LRESULT mainframe_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static BOOL			bHelp = FALSE;
	static BOOL			appActivated;
	RECT				rect;
	WORD				fwMenu;
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
		hwndFrameWindow = hwnd;
		mainframe_readDocks(hwnd);
		fkey_initKeyboardWidget(hwnd);
		st_init(hwnd);
		tb_initRebar(hwnd);
		PostMessage(hwnd, WM_SIZE, 0, 0);
		return 0;

	case WM_MDIACTIVATE: {
			DOCKING_SLOT *pSlot = mainframe_getDockingParent((HWND)wParam);
			if (pSlot != NULL) {
				HWND hwndTabControl = pSlot->ds_hwnd;
				ShowWindow(hwndTabControl, SW_SHOW);
				tabcontrol_selectPage(hwndTabControl, (HWND)wParam);
			}
		}
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
		if (hwndRebar) {
			ShowWindow(hwndRebar, nToolbarHeight ? SW_SHOW : SW_HIDE);
			SendMessage(hwndRebar, message, wParam, lParam);
			MoveWindow(hwndRebar, rect.left, 0,
				rect.right-rect.left, nToolbarHeight, 1);
		}
		if (hwndFkeys) {
			ShowWindow(hwndFkeys, TRUE);
			MoveWindow(hwndFkeys, rect.left, rect.bottom + rect.top,
				rect.right, nFkeyHeight, 1);
		}
		mainframe_arrangeDockingSlots(hwnd);
		st_resize(nStatusHeight, &rect);
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
		if (!IsWindow(hwnd) || !IsWindow(hwndFrameWindow)) {
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
		ll_destroy(&dockingSlots, NULL);
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
		win_registerWindowClass((char*)szDragProxyClass, dragproxy_windowProc,
			NULL, GetSysColorBrush(COLOR_3DFACE), NULL, sizeof(TAB_CONTROL*)) &&
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
	return hwndFrameWindow;
}

/*
 * Checks, whether there is an active PKS edit window already somewhere on the screen (maybe other process).
 */
BOOL mainframe_findExistingWindow() {
	return FindWindow(szFrameClass, 0) != 0;
}

/*------------------------------------------------------------
 * mainframe_translateAccelerator()
 */
static int mainframe_translateAccelerator(HWND hwnd, MSG* msg) {
	switch (msg->message) {
	case WM_LBUTTONDOWN: {
		HWND hwndParent = GetParent(msg->hwnd);
		if (GetFocus() != hwndParent) {
			SetFocus(hwndParent);
		}
		break;
	}
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYUP:
	case WM_KEYDOWN:
		if (msg->wParam == VK_MENU ||
			msg->wParam == VK_CONTROL ||
			msg->wParam == VK_SHIFT) {
			if (msg->wParam == VK_CONTROL) {
				PostMessage(msg->hwnd, WM_SETCURSOR, (WPARAM)0, 0);
			}
			fkey_keyModifierStateChanged();
			break;
		}
		if (!ww_workWinHasFocus()) {
			if (!(msg->hwnd == hwndFrameWindow || GetKeyState(VK_CONTROL) < 0)) {
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
		if (windowselector_isHandle(msg.hwnd) ||!mainframe_translateAccelerator(hwndFrameWindow, &msg)) {
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
	DOCKING_SLOT* pSlot = dockingSlots;
	while (pSlot) {
		if (pSlot->ds_type == DS_EDIT_WINDOW) {
			TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(pSlot->ds_hwnd, GWLP_TAB_CONTROL);
			if (pControl) {
				tabcontrol_repaintTabs(pSlot->ds_hwnd, pControl);
			}
		}
		pSlot = pSlot->ds_next;
	}
}

/*-----------------------------------------------------------
 * mainframe_enumChildWindows()
 * Execute a function for every child window optionally 
 * passing an lParam. If the called function returns 0, the enumeration
 * stops.
 */
int mainframe_enumChildWindows(BOOL bHideTabsDuringEnum, int (*funcp)(), LONG lParam) {
	DOCKING_SLOT* pSlot = dockingSlots;
	int ret;
	while (pSlot) {
		if (pSlot->ds_type == DS_EDIT_WINDOW) {
			if (bHideTabsDuringEnum) {
				ShowWindow(pSlot->ds_hwnd, SW_HIDE);
			}
			TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(pSlot->ds_hwnd, GWLP_TAB_CONTROL);
			while (arraylist_size(pControl->tc_pages) > 0) {
				TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, 0);
				if (pPage->tp_hwnd && (ret = (*funcp)(pPage->tp_hwnd, lParam)) == 0) {
					break;
				}
			}
			if (bHideTabsDuringEnum) {
				ShowWindow(pSlot->ds_hwnd, SW_SHOW);
			}
		}
		pSlot = pSlot->ds_next;
	}
	return ret;
}

/*
 * Add new docks to the mainframe. 
 */
int mainframe_manageDocks(MANAGE_DOCKS_TYPE mType) {
	DOCKING_SLOT* pDefaultSlot = mainframe_getSlot(szDefaultSlotName);
	DOCKING_SLOT* pRightSlot = mainframe_getSlot(DOCK_NAME_RIGHT);
	DOCKING_SLOT* pBottomSlot = mainframe_getSlot(DOCK_NAME_BOTTOM);

	if (!pDefaultSlot) {
		pDefaultSlot = mainframe_addDockingSlot(DS_EDIT_WINDOW, hwndFrameWindow, szDefaultSlotName, 0, 0, 1, 1);
		if (!pDefaultSlot) {
			return FALSE;
		}
	}
	if (mType == MD_ADD_HORIZONTAL) {
		if (!pRightSlot) {
			pRightSlot = mainframe_addDockingSlot(DS_EDIT_WINDOW, hwndFrameWindow, DOCK_NAME_RIGHT, 0.5, 0, 0.5, 1);
		}
	} else {
		if (!pBottomSlot) {
			pBottomSlot = mainframe_addDockingSlot(DS_EDIT_WINDOW, hwndFrameWindow, DOCK_NAME_BOTTOM, 0, 0.5, 1, 0.5);
		}
	}
	mainframe_applyDefaultSlotSizes();
	mainframe_arrangeDockingSlots(hwndFrameWindow);
	mainframe_saveDocks();
	return 1;
}

