/*
 * Project: PKS Edit for Windows
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
#include "winterf.h"
#include "editorconfiguration.h"
#include "history.h"
#include "stringutil.h"
#include "helpitem.h"
#include "mainframe.h"
#include "themes.h"
#include "codecompletion.h"
#include "winfo.h"
#include "pksmacro.h"
#include "errordialogs.h"
#include "xdialog.h"
#include "textblocks.h"
#include "customcontrols.h"
#include "mouseutil.h"
#include "windowselector.h"
#include "uahmenubar.h"
#include "darkmode.h"
#include "menu.h"
#include "dpisupport.h"

extern HINSTANCE		hInst;
extern void 	st_init(HWND hwndDaddy);
extern void		st_redraw(BOOL bErase);
extern void		st_resize(int nStatusHeight, RECT* pRect);
extern void 	status_wh(WORD* width, WORD* height);
extern void 	tb_wh(WORD* width, WORD* height);
extern BOOL 	ww_workWinHasFocus(void);
extern int 		clp_setdata(char* pszBufferName);
extern void 	EditDroppedFiles(HDROP hdrop);
extern void		tb_updateImageList(HWND hwnd, wchar_t* tbIcons, int nCount);
/*
 * Activate a tab page with the given window handle.
 */
static int tabcontrol_selectPage(HWND hwnd, const HWND hwndPage);

static const char* _applicationName = "PKS Edit";
static WINFO* _selectedWindow;

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

extern void fkey_visibilitychanged(void);

#define CLOSER_SIZE				13
#define CLOSER_DISTANCE			8
#define SCROLL_BUTTON_WIDTH		16
#define SCROLL_BUTTON_PADDING	4

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
	RECT ds_resizeRect;
} DOCKING_SLOT;

typedef enum { TW_CLOSER, TW_SCROLL_LEFT, TW_SCROLL_RIGHT } TAB_WIDGET_TYPE;

/*
 * A widget to be displayed on a tabstrip like a closer button or a scroll button. 
 */
typedef struct tagTAB_WIDGET {
	BOOL		tw_rollover;
	BOOL		tw_visible;
	TAB_WIDGET_TYPE tw_type;
	RECT		tw_bounds;					// the rect where the widget is displayed
} TAB_WIDGET;

typedef struct tagTAB_PAGE {
	HWND  tp_hwnd;
	int	  tp_width;
} TAB_PAGE;

typedef struct tagTAB_CONTROL {
	ARRAY_LIST* tc_pages;
	HWND		tc_hwnd;
	HWND		tc_hwndTooltip;
	TAB_WIDGET	tc_closer;						// the close button for the whole panel 
	TAB_WIDGET	tc_leftScroller;				// the scroll left button of the tab strip
	TAB_WIDGET	tc_rightScroller;				// the scroll right button of the tab strip
	RECT		tc_tabstripRect;				// the rect where the tabs of the tab control are displayed
	int			tc_activeTooltipIndex;
	int			tc_firstTabOffset;
	int			tc_stripHeight;
	int			tc_firstVisibleTab;
	int			tc_numberOfVisibleTabs;			// The number of tabs currently showing on the screen
	int			tc_activeTab;
	int			tc_rolloverTab;
} TAB_CONTROL;

const char *szFrameClass = "PKSEditMainFrame";
static const char* szTabClass = "PKSEditTabControl";
static const char* szDragProxyClass = "PKSDragProxyControl";

static TAB_CONTROL* currentDropTarget;
static DOCKING_SLOT* dockingSlots;
static HWND  hwndFrameWindow;
HICON defaultEditorIcon;
static MACROREF* _executeKeyBinding;
static BOOL _fullscreenMode;
static char* szDefaultSlotName = DOCK_NAME_DEFAULT;

static DOCKING_SLOT* mainframe_addDockingSlot(DOCKING_SLOT_TYPE dsType, HWND hwnd, char* pszName, float xRatio, float yRatio, float wRatio, float hRatio);

/*
 * "Scroll" the tabs to make the active tab fully visible.
 */
static void tabcontrol_makeActiveTabVisible(HWND hwnd, TAB_CONTROL* pControl);

/*
 * Resize our "docking slots" depending on their given ratios
 */
static void mainframe_arrangeDockingSlots(HWND hwnd);
/*
 * Paint one tab of the tabstrip of our tab control displaying the edit tabs.
 */
static BOOL tabcontrol_paintTab(HDC hdc, TAB_PAGE* pPage, BOOL bSelected, BOOL bRollover, int x, int y, int height, int xMax);

/*
 * Move a tab from one tab control to another.
 */
static void tabcontrol_moveTab(TAB_CONTROL* pSource, TAB_CONTROL* pTarget, TAB_PAGE* pPage, BOOL bActivate);

#define GWLP_TAB_CONTROL		0

// Request to close one dock. wParam == window of the tab control for which the dock is closed.
#define WM_CLOSE_DOCK			WM_USER+1

/*
 * Paint the drag proxy. 
 */
static void dragproxy_paint(HWND hwnd, PAINTSTRUCT* ps) {
	TAB_PAGE *pPage = (TAB_PAGE*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	RECT rect;
	GetClientRect(hwnd, &rect);
	tabcontrol_paintTab(ps->hdc, pPage, TRUE, FALSE, 0, rect.top, rect.bottom - rect.top, 99999);
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

static BOOL mainframe_useFullScreenMode() {
	return _fullscreenMode && dockingSlots->ds_next == NULL;
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

static int tabcontrol_getCloserSize() {
	return dpisupport_getSize(CLOSER_SIZE);
}

/*
 * Returns the title to display for a tab.
 */
static char* tabcontrol_getTitle(HWND hwnd, char* szBuffer, size_t nSize) {
	static char szTitle[128];
	if (!hwnd) {
		strcpy(szBuffer, "Page");
	} else {
		GetWindowText(hwnd, szBuffer, (int)nSize);
		if ((GetConfiguration()->layoutoptions & OL_COMPACT_TABS) &&
			// Hack: we use the initial hashmark to differentiate from the case, that the window title
			// is assigned explicitly (no filename - e.g. the search result list).
			(szBuffer[0] == '#' || szBuffer[0] == '*')) {
			char* pszLast = strrchr(szBuffer, '\\');
			if (pszLast == 0) {
				pszLast = strrchr(szBuffer, '/');
			}
			if (pszLast != 0) {
				memset(szTitle, 0, sizeof szTitle);
				char* pszDest = szTitle;
				while (*szBuffer) {
					char c = *szBuffer++;
					*pszDest++ = c;
					if (c == ' ' && *szBuffer != '#') {
						break;
					}
				}
				strcat(szTitle, pszLast + 1);
				return szTitle;
			}
		}
	}
	return szBuffer;
}

/*
 * Check, whether the dock displaying the HWND is closeable.
 * If it is the last edit dock, it cannot be closed.
 */
static BOOL mainframe_isCloseableDock(const HWND hwnd) {
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
	BOOL bChangeHSize = !pRightSlot || pRightSlot->ds_xratio != pDefaultSlot->ds_wratio;

	float height = pBottomSlot ? 0.5f : 1.0f;
	if (pDefaultSlot != NULL) {
		pDefaultSlot->ds_xratio = 0;
		pDefaultSlot->ds_yratio = 0;
		pDefaultSlot->ds_hratio = height;
		if (bChangeHSize) {
			pDefaultSlot->ds_wratio = pRightSlot != NULL ? 0.5f : 1.0f;
		}
	}
	if (pRightSlot != NULL) {
		if (bChangeHSize) {
			pRightSlot->ds_wratio = pDefaultSlot != NULL ? 0.5f : 1.0f;
			pRightSlot->ds_xratio = pDefaultSlot != NULL ? 0.5f : 0.0f;
		}
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
void mainframe_readDocks() {
	HWND hwnd = hwndMain;
	char szName[32];
	float x, y, w, h;

	for (int i = 0; 1; i++) {
		if (!hist_readDockingPlacement(i, szName, &x, &y, &w, &h)) {
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
		if (!hist_saveDockingPlacement(i, pSlot->ds_name, pSlot->ds_xratio, pSlot->ds_yratio, pSlot->ds_wratio, pSlot->ds_hratio)) {
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
	DOCKING_SLOT* pSlotUpdate = NULL;
	while (pSlot) {
		if (pSlot->ds_hwnd == hwnd) {
			pSlotUpdate = pSlot;
			break;
		}
		pSlot = pSlot->ds_next;
	}
	if (pSlotUpdate == NULL) {
		return;
	}
	EdTRACE(log_errorArgs(DEBUG_TRACE, "Closing dock %s", pSlot->ds_name));
	DOCKING_SLOT* pSlotDefault = mainframe_getSlot(szDefaultSlotName);
	if (pSlotDefault == pSlotUpdate) {
		pSlot = dockingSlots;
		pSlotUpdate = NULL;
		while (pSlot) {
			if (pSlot != pSlotDefault) {
				pSlotUpdate = pSlot;
				hwnd = pSlotUpdate->ds_hwnd;
				break;
			}
			pSlot = pSlot->ds_next;
		}
	}
	if (!pSlotUpdate || !pSlotDefault) {
		return;
	}
	if (pSlotUpdate->ds_type == DS_EDIT_WINDOW) {
		// Migrate contained windows to other dock.
		TAB_CONTROL* pSource = (TAB_CONTROL*) GetWindowLongPtr(pSlotUpdate->ds_hwnd, GWLP_TAB_CONTROL);
		TAB_CONTROL* pTarget = (TAB_CONTROL*) GetWindowLongPtr(pSlotDefault->ds_hwnd, GWLP_TAB_CONTROL);
		if (pSource && pTarget) {
			while (pSource->tc_activeTab >= 0) {
				tabcontrol_moveTab(pSource, pTarget, arraylist_get(pSource->tc_pages, pSource->tc_activeTab), TRUE);
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
 * Trigger a repaint of the tabstrip of a tab controls tab with the given index.
 * If index is -1, repaint all tabs
 */
static void tabcontrol_repaintTab(HWND hwnd, TAB_CONTROL* pControl, int nIndex) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	rect.bottom = rect.top + pControl->tc_stripHeight + 1;
	if (nIndex >= 0) {
		int x = pControl->tc_tabstripRect.left;
		int nLen = (int)arraylist_size(pControl->tc_pages);
		for (int i = pControl->tc_firstVisibleTab; i < nLen; i++) {
			if (i > nIndex) {
				return;
			}
			TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
			int x2 = x + pPage->tp_width;
			if (i == nIndex) {
				rect.left = x;
				rect.right = x2;
				break;
			}
			x = x2;
			if (x > rect.right) {
				return;
			}
		}
	}
	InvalidateRect(hwnd, &rect, FALSE);
}


/*
 * Trigger a repaint of the tabstrip of a tab control. 
 */
static void tabcontrol_repaintTabs(HWND hwnd, TAB_CONTROL* pControl) {
	tabcontrol_repaintTab(hwnd, pControl, -1);
}

/*
 * Adjust the size of the tab container when the window size has changed.
 */
static void tabcontrol_resizeActiveTabContents(HWND hwnd, TAB_CONTROL* pControl) {
	if (pControl->tc_activeTab >= 0) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, pControl->tc_activeTab);
		if (pPage->tp_hwnd) {
			RECT rect;
			GetClientRect(hwnd, &rect);
			int y = rect.top + pControl->tc_stripHeight + 1;
			int h = rect.bottom - rect.top - pControl->tc_stripHeight - 2;
			int x = rect.left + 1;
			int w = rect.right - rect.left - 2;
			if (mainframe_useFullScreenMode()) {
				x--;
				w += 2;
				y = rect.top;
				h = rect.bottom - rect.top;
			}
			MoveWindow(pPage->tp_hwnd, x, y, w, h, TRUE);
		}
	}
}

/*
 * "Scroll" the tabs to make the active tab fully visible. 
 */
static void tabcontrol_makeActiveTabVisible(HWND hwnd, TAB_CONTROL* pControl) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	int xMax = rect.right;
	if (rect.right - rect.left <= 10) {
		return;
	}
	xMax -= tabcontrol_getCloserSize()  + 2 * CLOSER_DISTANCE;
	int nLen = (int)arraylist_size(pControl->tc_pages);
	int x = pControl->tc_tabstripRect.left;
	for (int i = pControl->tc_firstVisibleTab; i < nLen; i++) {
		if (i > pControl->tc_activeTab) {
			break;
		}
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		x += pPage->tp_width;
		if (x >= xMax) {
			break;
		}
		if (i == pControl->tc_activeTab) {
			// active tab already visible - nothing to do.
			return;
		}
	}
	for (int i = 0; i < nLen; i++) {
		x = pControl->tc_tabstripRect.left;
		for (int j = i; j < nLen; j++) {
			TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, j);
			x += pPage->tp_width;
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
 * Find the docking parent window for a given editor window.
 */
static DOCKING_SLOT* mainframe_getDockingParent(const HWND hwnd) {
	DOCKING_SLOT* pSlot = dockingSlots;
	while (pSlot) {
		if (pSlot->ds_type == DS_EDIT_WINDOW) {
			TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(pSlot->ds_hwnd, GWLP_TAB_CONTROL);
			if (pControl == 0) {
				continue;
			}
			for (int i = 0; i < arraylist_size(pControl->tc_pages); i++) {
				TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
				if (pPage->tp_hwnd == hwnd) {
					return pSlot;
				}
			}
		}
		pSlot = pSlot->ds_next;
	}
	return NULL;
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
			tabcontrol_resizeActiveTabContents(hwnd, pControl);
			ShowWindow(pPage->tp_hwnd, SW_SHOW);
			WINFO* wp = ww_getWinfoForHwnd(pPage->tp_hwnd);
			if (wp->linkedWindow != NULL) {
				WINFO* wpLeft = wp->linkedWindow->lw_wpLeft;
				WINFO* wpRight = wp->linkedWindow->lw_wpRight;
				WINFO* wpOther = wp == wpLeft ? wpRight : wpLeft;
				DOCKING_SLOT* pParent1 = mainframe_getDockingParent(pPage->tp_hwnd);
				DOCKING_SLOT* pParent2 = mainframe_getDockingParent(wpOther->edwin_handle);
				if (pParent1 != NULL && pParent2 != NULL && pParent1 != pParent2) {
					tabcontrol_selectPage(pParent2->ds_hwnd, wpOther->edwin_handle);
				}
			}
			SetFocus(pPage->tp_hwnd);
		}
		tabcontrol_makeActiveTabVisible(hwnd, pControl);
		tabcontrol_repaintTabs(hwnd, pControl);
	}
	if (hwndOld != NULL) {
		ShowWindow(hwndOld, SW_HIDE);
	}
}

/*
 * Activate a tab page with the given window handle.
 */
static int tabcontrol_selectPage(HWND hwnd, const HWND hwndPage) {
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
static int tabcontrol_addTab(HWND hwnd, HWND hwndTab, BOOL bSelect) {
	char szTitle[EDMAXPATHLEN];
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	TAB_PAGE* pData = calloc(1, sizeof * pData);
	int idx = (int)arraylist_size(pControl->tc_pages);
	arraylist_add(pControl->tc_pages, pData);
	pData->tp_hwnd = hwndTab;
	char* pszTitle = tabcontrol_getTitle(pData->tp_hwnd, szTitle, sizeof szTitle);
	// Rough estimate for now - the correct size is measured later in repaint.
	pData->tp_width = (int)((30 + (int)strlen(pszTitle) * 7) * dpisupport_getScalingFactorX());
	RECT rect;
	GetClientRect(hwndTab, &rect);
	rect.bottom = rect.top + pControl->tc_stripHeight;
	if (bSelect || idx == 0) {
		tabcontrol_selectTab(hwnd, pControl, idx);
	}
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
	nPages--;
	tabcontrol_selectTab(pControl->tc_hwnd, pControl, (int)(nPages));
	tabcontrol_repaintTabs(pControl->tc_hwnd, pControl);
}

/*
 * Calculate the rectangle occupied by the closer button. 
 */
static void tabcontrol_getCloserRect(TAB_PAGE* pPage, int x, int height, RECT* pRect) {
	int nCloserSize = tabcontrol_getCloserSize();
	pRect->left = x + pPage->tp_width - nCloserSize - CLOSER_DISTANCE;
	pRect->top = (height - nCloserSize) / 2 + 2;
	pRect->right = pRect->left + nCloserSize;
	pRect->bottom = pRect->top + nCloserSize;
}

/*
 * Paint a widget into its bounding rect.
 */
static void tabcontrol_paintWidgetContents(HDC hdc, RECT* pRect, BOOL bRollover, TAB_WIDGET_TYPE twWidgetType) {
	int nDelta = 4;
	int h2;
	THEME_DATA* pTheme = theme_getCurrent();
	LOGBRUSH brush;
	brush.lbColor = pTheme->th_dialogBorder;
	brush.lbHatch = 0;
	brush.lbStyle = PS_SOLID;
	HPEN hPen = ExtCreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_ROUND | PS_ENDCAP_ROUND, 
		dpisupport_getSize(bRollover && twWidgetType == TW_CLOSER ? 4 : 2), &brush, 0, NULL);
	HPEN hPenOld = SelectObject(hdc, hPen);
	h2 = (pRect->bottom - pRect->top) / 2;
	if (bRollover) {
		int inflate = dpisupport_getSize(3);
		InflateRect(pRect, inflate, inflate);
		FillRect(hdc, pRect, theme_getDialogLightBackgroundBrush());
		InflateRect(pRect, -inflate, -inflate);
	}
	switch (twWidgetType) {
	case TW_CLOSER:
		MoveToEx(hdc, pRect->left + nDelta, pRect->top + nDelta, NULL);
		LineTo(hdc, pRect->right - nDelta, pRect->bottom - nDelta);
		MoveToEx(hdc, pRect->right - nDelta, pRect->top + nDelta, NULL);
		LineTo(hdc, pRect->left + nDelta, pRect->bottom - nDelta);
		break;
	case TW_SCROLL_LEFT:
		MoveTo(hdc, pRect->right-nDelta, pRect->top+nDelta);
		LineTo(hdc, pRect->left+nDelta, pRect->top + h2);
		LineTo(hdc, pRect->right-nDelta, pRect->bottom-nDelta);
		LineTo(hdc, pRect->right - nDelta, pRect->top + nDelta);
		break;
	case TW_SCROLL_RIGHT:
		MoveTo(hdc, pRect->left+nDelta, pRect->top+nDelta);
		LineTo(hdc, pRect->right-nDelta, pRect->top + h2);
		LineTo(hdc, pRect->left+nDelta, pRect->bottom-nDelta);
		LineTo(hdc, pRect->left + nDelta, pRect->top + nDelta);
		break;
	}
	DeleteObject(SelectObject(hdc, hPenOld));
}

/*
 * Paint a widget into its bounding rect.
 */
static void tabcontrol_paintWidget(HDC hdc, TAB_WIDGET* pWidget) {
	if (!pWidget->tw_visible) {
		return;
	}
	tabcontrol_paintWidgetContents(hdc, &pWidget->tw_bounds, pWidget->tw_rollover, pWidget->tw_type);
}

#define TAB_ICON_SIZE		16
#define TAB_ICON_MARGIN		4

/*
 * Measure the extends of one tab of the tabstrip of our tab control displaying the edit tabs.
 */
static void tabcontrol_measureTab(HDC hdc, TAB_PAGE* pPage, BOOL bSelected) {
	char szBuffer[EDMAXPATHLEN];
	char* pszTitle;
	int nIconSize = dpisupport_getSize(TAB_ICON_SIZE);
	int nMargin = dpisupport_getSize(TAB_ICON_MARGIN);

	pszTitle = tabcontrol_getTitle(pPage->tp_hwnd, szBuffer, sizeof szBuffer);
	size_t nLen = strlen(pszTitle);
	SIZE sText;
	HFONT hFont = SelectObject(hdc, theme_createDialogFont(bSelected ? FW_BOLD : FW_NORMAL));
	GetTextExtentPoint(hdc, pszTitle, (int)nLen, &sText);
	pPage->tp_width = sText.cx + nIconSize + 2 * nMargin + tabcontrol_getCloserSize() + 2 * dpisupport_getSize(CLOSER_DISTANCE);
	DeleteObject(SelectObject(hdc, hFont));
}

/*
 * Recalculate all sizes of tab headers, close, scroll buttons etc.... for the
 * tab control.
 */
static void tabcontrol_measureTabStrip(HWND hwnd, TAB_CONTROL* pControl) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	if (rect.bottom -rect.top < pControl->tc_stripHeight) {
		// During initialization
		return;
	}
	rect.bottom = rect.top + pControl->tc_stripHeight;
	RECT* pRect;
	if (!mainframe_isCloseableDock(hwnd)) {
		pControl->tc_closer.tw_visible = FALSE;
		pControl->tc_closer.tw_bounds.left = rect.right;
	}
	else {
		int nCloserSize = tabcontrol_getCloserSize();
		pControl->tc_closer.tw_visible = TRUE;
		pRect = &pControl->tc_closer.tw_bounds;
		pRect->right = rect.right - dpisupport_getSize(CLOSER_DISTANCE);
		pRect->left = pRect->right - nCloserSize;
		int dDelta = (pControl->tc_stripHeight - nCloserSize) / 2;
		pRect->top = rect.top + dDelta;
		pRect->bottom = rect.top + pRect->top + nCloserSize;
	}
	pControl->tc_tabstripRect.right = pControl->tc_closer.tw_bounds.left - dpisupport_getSize(SCROLL_BUTTON_PADDING);
	pControl->tc_tabstripRect.top = rect.top;
	pControl->tc_tabstripRect.bottom = rect.bottom;

	HDC hdc = GetWindowDC(hwnd);
	int nTotalWidth = 0;
	int nAvailableSpace = pControl->tc_tabstripRect.right - pControl->tc_tabstripRect.left;
	int nSize = (int) arraylist_size(pControl->tc_pages);
	pControl->tc_numberOfVisibleTabs = nSize;
	for (int i = 0; i < nSize; i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		tabcontrol_measureTab(hdc, pPage, i == pControl->tc_activeTab);
		if (i == pControl->tc_firstVisibleTab) {
			nTotalWidth = pPage->tp_width;
		} else {
			nTotalWidth += pPage->tp_width;
		}
		if (i >= pControl->tc_firstVisibleTab && nTotalWidth > nAvailableSpace) {
			pControl->tc_numberOfVisibleTabs = (i - pControl->tc_firstVisibleTab) + 1;
			break;
		}
	}
	ReleaseDC(hwnd, hdc);

	pRect = &pControl->tc_leftScroller.tw_bounds;
	if (pControl->tc_firstVisibleTab > 0) {
		int delta = (rect.bottom - rect.top - dpisupport_getSize(SCROLL_BUTTON_WIDTH)) / 2;
		pRect->top = rect.top + delta;
		pRect->bottom = rect.bottom- delta;
		pRect->left = pControl->tc_firstTabOffset+ dpisupport_getSize(SCROLL_BUTTON_PADDING);
		pRect->right = pRect->left + dpisupport_getSize(SCROLL_BUTTON_WIDTH);
		pControl->tc_tabstripRect.left = pRect->right + dpisupport_getSize(SCROLL_BUTTON_PADDING);
		pControl->tc_leftScroller.tw_visible = TRUE;
	} else {
		pControl->tc_leftScroller.tw_visible = FALSE;
		pControl->tc_leftScroller.tw_rollover = FALSE;
		pControl->tc_tabstripRect.left = pControl->tc_firstTabOffset;
	}

	pRect = &pControl->tc_rightScroller.tw_bounds;
	if (nTotalWidth > nAvailableSpace) {
		int delta = (rect.bottom - rect.top - dpisupport_getSize(SCROLL_BUTTON_WIDTH)) / 2;
		pRect->top = rect.top + delta;
		pRect->bottom = rect.bottom - delta;
		pRect->right = pControl->tc_closer.tw_bounds.left - dpisupport_getSize(SCROLL_BUTTON_PADDING);
		pRect->left = pRect->right - dpisupport_getSize(SCROLL_BUTTON_WIDTH);
		pControl->tc_tabstripRect.right = pRect->left + dpisupport_getSize(SCROLL_BUTTON_PADDING);
		pControl->tc_rightScroller.tw_visible = TRUE;
	} else {
		pControl->tc_rightScroller.tw_visible = FALSE;
		pControl->tc_rightScroller.tw_rollover = FALSE;
	}

}

/*
 * Returns a default icon to display for editors not having an own icon defined by the Windows file type. 
 */
HICON mainframe_getDefaultEditorIcon() {
	if (defaultEditorIcon == NULL) {
		defaultEditorIcon = LoadIcon(hInst, "EDIT");
	}
	return defaultEditorIcon;
}

/*
 * Paint one tab of the tabstrip of our tab control displaying the edit tabs. 
 */
static BOOL tabcontrol_paintTab(HDC hdc, TAB_PAGE* pPage, BOOL bSelected, BOOL bRollover, int x, int y, int height, int xMax) {
	char szBuffer[EDMAXPATHLEN];
	char* pszTitle;
	THEME_DATA* pTheme = theme_getCurrent();
	int nIconSize = dpisupport_getSize(TAB_ICON_SIZE);
	int nMargin = dpisupport_getSize(TAB_ICON_MARGIN);
	RECT rect;
	
	tabcontrol_measureTab(hdc, pPage, bSelected);
	if (x + pPage->tp_width >= xMax) {
		return FALSE;
	}
	pszTitle = tabcontrol_getTitle(pPage->tp_hwnd, szBuffer, sizeof szBuffer);
	size_t nLen = strlen(pszTitle);
	HFONT hFont = SelectObject(hdc, theme_createDialogFont(bSelected ? FW_BOLD : FW_NORMAL));
	rect.left = x;
	rect.right = x + pPage->tp_width;
	rect.top = y;
	rect.bottom = y + height;
	HBRUSH hBrush = CreateSolidBrush(bSelected ? pTheme->th_defaultBackgroundColor : pTheme->th_dialogLight);
	HBRUSH hOld = SelectObject(hdc, hBrush);
	int border = dpisupport_getSize(2);
	if (bSelected) {
		rect.bottom++;
	} else {
		rect.top += border;
	}
	FillRect(hdc, &rect, hBrush);
	DeleteObject(SelectObject(hdc, hOld));
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, pTheme->th_dialogForeground);
	if (!bSelected) {
		rect.top += border;
		rect.left += border;
	}
	int yIcon = (height - nIconSize) / 2;
	HICON hIcon = pPage->tp_hwnd ? (HICON)SendMessage(pPage->tp_hwnd, WM_GETICON, ICON_SMALL, 0L) : (HICON)NULL;
	if (hIcon == 0) {
		hIcon = mainframe_getDefaultEditorIcon();
	}
	DrawIconEx(hdc, rect.left + nMargin, yIcon, hIcon, nIconSize, nIconSize, 0, NULL, DI_NORMAL);
	rect.left += 2 * nMargin + nIconSize;
	DrawText(hdc, pszTitle, (int)nLen, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	DeleteObject(SelectObject(hdc, hFont));
	HPEN hPen = CreatePen(PS_SOLID, border/2, pTheme->th_dialogBorder);
	HPEN hPenOld = SelectObject(hdc, hPen);
	tabcontrol_getCloserRect(pPage, x, height, &rect);
	tabcontrol_paintWidgetContents(hdc, &rect, bRollover, TW_CLOSER);
	if (!bSelected) {
		y += border;
		height -= border;
	}
	MoveToEx(hdc, x, y+height, NULL);
	LineTo(hdc, x, y);
	int x2 = x + pPage->tp_width;
	LineTo(hdc, x2, y);
	LineTo(hdc, x2, y + height);
	DeleteObject(SelectObject(hdc, hPenOld));
	if (bSelected) {
		WINFO* wp = ww_getCurrentEditorWindow();
		HWND hwndFocus = wp != NULL ? wp->ww_handle : NULL;
		while (hwndFocus) {
			if (hwndFocus == pPage->tp_hwnd) {
				break;
			}
			hwndFocus = GetParent(hwndFocus);
		}
		if (!hwndFocus) {
			return TRUE;
		}
		LOGBRUSH brush;
		brush.lbColor = pTheme->th_dialogActiveTab;
		brush.lbHatch = 0;
		brush.lbStyle = PS_SOLID;
		hPen = ExtCreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, dpisupport_getSize(3), &brush, 0, NULL);
		hPenOld = SelectObject(hdc, hPen);
		MoveToEx(hdc, x + border/2, y + border/2, NULL);
		LineTo(hdc, x + pPage->tp_width-border, y+border/2);
		DeleteObject(SelectObject(hdc, hPenOld));
	}
	return TRUE;
}

/*
 * Repaint the tab strip of a tab control including the tabs, an optional docking close button etc.
 */
static void tabcontrol_paintTabStrip(HWND hwnd, PAINTSTRUCT* ps, TAB_CONTROL* pControl) {
	THEME_DATA* pTheme = theme_getCurrent();
	HBRUSH hBrush = CreateSolidBrush(pTheme->th_dialogBackground);
	HBRUSH hBorderBrush = CreateSolidBrush(pTheme->th_dialogBorder);
	tabcontrol_measureTabStrip(hwnd, pControl);
	RECT rect;
	GetClientRect(hwnd, &rect);
	FillRect(ps->hdc, &rect, hBrush);
	if (pControl == currentDropTarget) {
		HPEN hPen = CreatePen(PS_SOLID, 3, pTheme->th_dialogActiveTab);
		HPEN hPenOld = SelectObject(ps->hdc, hPen);
		MoveTo(ps->hdc, rect.left, rect.top+1);
		LineTo(ps->hdc, rect.right-1, rect.top+1);
		LineTo(ps->hdc, rect.right-1, rect.bottom);
		DeleteObject(SelectObject(ps->hdc, hPenOld));
	}
	rect.top += pControl->tc_stripHeight;
	FrameRect(ps->hdc, &rect, hBorderBrush);
	rect.top -= pControl->tc_stripHeight;
	int x;
	int y;
	y = pControl->tc_tabstripRect.top;
	x = pControl->tc_tabstripRect.left;
	for (int i = pControl->tc_firstVisibleTab; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		if (!tabcontrol_paintTab(ps->hdc, pPage, i == pControl->tc_activeTab, i == pControl->tc_rolloverTab, x, y, 
				pControl->tc_stripHeight, pControl->tc_tabstripRect.right) && i > pControl->tc_firstVisibleTab) {
			break;
		}
		x += pPage->tp_width;
	}
	tabcontrol_paintWidget(ps->hdc, &pControl->tc_leftScroller);
	tabcontrol_paintWidget(ps->hdc, &pControl->tc_rightScroller);
	tabcontrol_paintWidget(ps->hdc, &pControl->tc_closer);
	DeleteObject(hBrush);
	DeleteObject(hBorderBrush);
}

/*
 * Free the data structure occupied by a tab control. 
 */
static void tabcontrol_destroy(TAB_CONTROL* pControl) {
	if (pControl->tc_hwndTooltip) {
		DestroyWindow(pControl->tc_hwndTooltip);
	}
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
static void tabcontrol_dragOver(const TAB_CONTROL* pSource) {
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
static void tabcontrol_moveTab(TAB_CONTROL* pSource, TAB_CONTROL*pTarget, TAB_PAGE* pPage, BOOL bSelect) {
	SetParent(pPage->tp_hwnd, pTarget->tc_hwnd);
	tabcontrol_addTab(pTarget->tc_hwnd, pPage->tp_hwnd, bSelect);
	tabcontrol_removeTab(pSource, pPage);
}

/*
 * Return the tab of a tab control containing the passed page window. 
 */
static TAB_PAGE* tabcontrol_getTab(TAB_CONTROL* pSource, const HWND hwndEdit) {
	for (int i = 0; i < arraylist_size(pSource->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pSource->tc_pages, i);
		if (pPage->tp_hwnd == hwndEdit) {
			return pPage;
		}
	}
	return NULL;
}

/*
 * A tab of a tab control was dropped on another tab control. Migrate the tab to the new control.
 * if 'bAccept' is FALSE, cancel the operation.
 */
static void tabcontrol_drop(TAB_CONTROL* pSource, TAB_PAGE *pDroppedPage, BOOL bAccept) {
	if (bAccept && currentDropTarget) {
		tabcontrol_moveTab(pSource, currentDropTarget, pDroppedPage, TRUE);
	}
	tabcontrol_setAcceptDrop(NULL, FALSE);
}

/*
 * Drag a tab across the screen to a potential new tab. 
 */
static BOOL tabcontrol_dragTab(HWND hwnd, TAB_CONTROL* pControl, TAB_PAGE* pPage, int x, int y) {
	int b = 1;
	int dummy;

	if (DragDetect(hwnd, (POINT) { x, y })) {
		SetCapture(hwnd);
		HCURSOR hCursor = LoadCursor(0, IDC_SIZEALL);
		SetCursor(hCursor);
		RECT rect;
		rect.left = x;
		rect.top = y;
		rect.right = pPage->tp_width + rect.left;
		rect.bottom = rect.top + pControl->tc_stripHeight;
		HWND hwndProxy = dragproxy_open(&rect, pPage);
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

static TAB_PAGE* tabcontrol_getPageFor(TAB_CONTROL* pControl, int x, int* pPageIndex) {
	int tOffset = pControl->tc_tabstripRect.left;
	x -= tOffset;
	for (int i = pControl->tc_firstVisibleTab; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		if (x < 0) {
			break;
		}
		if (x < pPage->tp_width) {
			*pPageIndex = i;
			return pPage;
		}
		x -= pPage->tp_width;
	}
	return 0;
}

/*
 * Scroll the tabs by a given delta.
 */
static void tabcontrol_scrollTabs(HWND hwnd, int nDelta) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	tabcontrol_measureTabStrip(hwnd, pControl);
	int nNew = pControl->tc_firstVisibleTab + nDelta;
	int nTotal = (int)arraylist_size(pControl->tc_pages);
	if (pControl->tc_numberOfVisibleTabs && nTotal < pControl->tc_numberOfVisibleTabs && pControl->tc_firstVisibleTab == 0) {
		return;
	}
	if (nNew >= nTotal) {
		nNew = nTotal - 1;
	}
	if (nNew < 0) {
		nNew = 0;
	}
	if (nNew != pControl->tc_firstVisibleTab) {
		pControl->tc_firstVisibleTab = nNew;
		tabcontrol_repaintTabs(hwnd, pControl);

	}
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
	POINT pt = (POINT){ x, y };
	if (PtInRect(&pControl->tc_closer.tw_bounds, pt) && !bDrag) {
		mainframe_closeDock(hwnd);
		return;
	}
	if (PtInRect(&pControl->tc_leftScroller.tw_bounds, pt) && pControl->tc_firstVisibleTab > 0) {
		tabcontrol_scrollTabs(hwnd, -1);
		return;
	}
	if (PtInRect(&pControl->tc_rightScroller.tw_bounds, pt)) {
		tabcontrol_scrollTabs(hwnd, 1);
		return;
	}
	int i;
	TAB_PAGE* pPage = tabcontrol_getPageFor(pControl, x, &i);
	if (pPage) {
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
	}
}

/* 
 * Change the current "rollover" tab. 
 */
static void tabcontrol_setRollover(HWND hwnd, TAB_CONTROL* pControl, int nIndex, int nTabIndex) {
	BOOL bTrackChanged = FALSE;
	if (pControl->tc_rolloverTab != nIndex) {
		if (pControl->tc_rolloverTab >= 0) {
			tabcontrol_repaintTab(hwnd, pControl, pControl->tc_rolloverTab);
		}
		pControl->tc_rolloverTab = nIndex;
		if (pControl->tc_rolloverTab >= 0) {
			tabcontrol_repaintTab(hwnd, pControl, pControl->tc_rolloverTab);
		}
		bTrackChanged = TRUE;
	}
	BOOL bShow = nTabIndex >= 0;
	if (nTabIndex != pControl->tc_activeTooltipIndex) {
		// Activate the tooltip.
		TTTOOLINFO toolinfo = { 0 };
		toolinfo.cbSize = sizeof(toolinfo);
		toolinfo.hwnd = pControl->tc_hwnd;
		toolinfo.hinst = hInst;
		TAB_PAGE* pPage = nTabIndex >= 0 ? arraylist_get(pControl->tc_pages, nTabIndex) : 0;
		if (pPage) {
			char szBuffer[EDMAXPATHLEN];
			GetWindowText(pPage->tp_hwnd, szBuffer, sizeof szBuffer);
			toolinfo.lpszText = szBuffer;
			SendMessage(pControl->tc_hwndTooltip, TTM_UPDATETIPTEXT, (WPARAM)0, (LPARAM)&toolinfo);
		}
		EdTRACE(log_errorArgs(DEBUG_TRACE, "Trying to activate tooltip for tab %d.", nTabIndex));
		if (!SendMessage(pControl->tc_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)bShow, (LPARAM)&toolinfo)) {
			log_errorArgs(DEBUG_ERR, "Activating tooltip failed. Error %ld.", GetLastError());
		}
		RECT r;
		GetWindowRect(hwnd, &r);
		POINT pt;
		GetCursorPos(&pt);
		int delta = dpisupport_getSize(10);
		pt.y = r.top + pControl->tc_stripHeight + delta;
		SendMessage(pControl->tc_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x + delta, pt.y));
		pControl->tc_activeTooltipIndex = nTabIndex;
		bTrackChanged = TRUE;
	}
	if (bTrackChanged) {
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = hwnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		if (nIndex > 0 || bShow) {
			tme.dwFlags = TME_LEAVE;
		}
		else {
			tme.dwFlags = 0;
		}
		TrackMouseEvent(&tme);
	}
}

/*
 * Set a new rollover state for a widget. 
 */
static BOOL tabcontrol_setWidgetRollover(HWND hwnd, TAB_WIDGET* pWidget, BOOL bRollover) {
	if (bRollover != pWidget->tw_rollover) {
		pWidget->tw_rollover = bRollover;
		RECT rInvalidate = pWidget->tw_bounds;
		int delta = dpisupport_getSize(4);
		InflateRect(&rInvalidate, delta, delta);
		InvalidateRect(hwnd, &rInvalidate, TRUE);
		return TRUE;
	}
	return FALSE;
}

/*
 * Rollover handling for a tabstrip widget. 
 */
static BOOL tabcontrol_handleRollover(HWND hwnd, TAB_WIDGET* pWidget, POINT p) {
	if (pWidget->tw_visible) {
		return tabcontrol_setWidgetRollover(hwnd, pWidget, PtInRect(&pWidget->tw_bounds, p));
	}
	return FALSE;
}

/*
 * Handle mouse move for rollover effects.
 */
static void tabcontrol_handleMouseMove(HWND hwnd, POINT p) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	if (p.y >= pControl->tc_stripHeight) {
		tabcontrol_setWidgetRollover(hwnd, &pControl->tc_leftScroller, FALSE);
		tabcontrol_setWidgetRollover(hwnd, &pControl->tc_rightScroller, FALSE);
		tabcontrol_setWidgetRollover(hwnd, &pControl->tc_closer, FALSE);
		tabcontrol_setRollover(hwnd, pControl, -1, -1);
		return;
	}
	if (tabcontrol_handleRollover(hwnd, &pControl->tc_closer, p) ||
		tabcontrol_handleRollover(hwnd, &pControl->tc_leftScroller, p) ||
		tabcontrol_handleRollover(hwnd, &pControl->tc_rightScroller, p)) {
		return;
	}
	int x = pControl->tc_tabstripRect.left;
	int nTab = -1;
	for (int i = pControl->tc_firstVisibleTab; i < arraylist_size(pControl->tc_pages); i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		if (p.x >= x && p.x < x + pPage->tp_width) {
			nTab = i;
		}
		RECT rect;
		tabcontrol_getCloserRect(pPage, x, pControl->tc_stripHeight, &rect);
		if (p.y > rect.bottom || p.y < rect.top || x > rect.right) {
			break;
		}
		if (PtInRect(&rect, p)) {
			tabcontrol_setRollover(hwnd, pControl, i, nTab);
			return;
		}
		x += pPage->tp_width;
	}
	tabcontrol_setRollover(hwnd, pControl, -1, nTab);
}

static void tabcontrol_closed(HWND hwnd, const HWND hwndChild) {
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	size_t len = arraylist_size(pControl->tc_pages);

	for (int i = 0; i < len; i++) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, i);
		if (pPage->tp_hwnd == hwndChild) {
			arraylist_remove(pControl->tc_pages, pPage);
			free(pPage);
			if (arraylist_size(pControl->tc_pages) == 0 && mainframe_isCloseableDock(hwnd)) {
				pControl->tc_activeTab = -1;
				PostMessage(hwndFrameWindow, WM_CLOSE_DOCK, (WPARAM) hwnd, 0);
				return;
			}
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
 * tabcontrol_createTooltip()
 * Create a tooltip to be optionally displayed for the tabs of the mainframe.
 */
static void tabcontrol_createTooltip(TAB_CONTROL* pControl) {
	pControl->tc_hwndTooltip = cust_createToolTooltip(pControl->tc_hwnd);
}

static void tabcontrol_openContextMenu(HWND hwnd, int x, int y) {
	POINT pt = { .x = x, .y = y };
	ScreenToClient(hwnd, &pt);
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
	int i;
	TAB_PAGE* pPage = tabcontrol_getPageFor(pControl, pt.x, &i);
	if (pPage) {
		WINFO* wp = ww_getWinfoForHwnd(pPage->tp_hwnd);
		if (wp) {
			_selectedWindow = wp;
			FTABLE* fp = FTPOI(wp);
			printf(fp->fname);
			menu_showContextMenu(wp->ww_handle, wp, "editor-tabs", x, y);
		}
	}
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
		pControl->tc_stripHeight = dpisupport_getSize(26);
		pControl->tc_activeTab = -1;
		pControl->tc_rolloverTab = -1;
		pControl->tc_firstTabOffset = dpisupport_getSize(5);
		pControl->tc_hwnd = hwnd;
		pControl->tc_leftScroller.tw_type = TW_SCROLL_LEFT;
		pControl->tc_rightScroller.tw_type = TW_SCROLL_RIGHT;
		pControl->tc_closer.tw_type = TW_CLOSER;
		tabcontrol_createTooltip(pControl);
		SetWindowLongPtr(hwnd, GWLP_TAB_CONTROL, (LONG_PTR)pControl);
		break;

	case WM_MOUSEWHEEL: {
		int nDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		tabcontrol_scrollTabs(hwnd, nDelta > 0 ? 1 : -1);
		break;
	}

	case WM_CONTEXTMENU:
		tabcontrol_openContextMenu(hwnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_LBUTTONDOWN:
		tabcontrol_handleButtonDown(hwnd, lParam, FALSE);
		ptDown.x = GET_X_LPARAM(lParam);
		ptDown.y = GET_Y_LPARAM(lParam);
		break;

	case WM_MOUSELEAVE: {
		tabcontrol_handleMouseMove(hwnd, (POINT) { 0, 0 });
		break;
	}

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
		tabcontrol_paintTabStrip(hwnd, &ps, pControl);
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

	case WM_WINDOWPOSCHANGING: {
		RECT r;
		LPWINDOWPOS pwp = (LPWINDOWPOS)lParam;
		pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
		GetWindowRect(hwnd, &r);
		if (!(pwp->flags & SWP_NOREDRAW)) {
			MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&r, 2);
			if (r.top == pwp->y) {
				GetClientRect(hwnd, &r);
				if (pwp->cy < r.bottom || pwp->cx < r.right) {
					break;
				}
				r.bottom = pControl->tc_tabstripRect.bottom+1;
				InvalidateRect(hwnd, &r, FALSE);
				break;
			} else {
				GetWindowRect(hwnd, &r);
				r.top = pwp->cy;
				r.bottom += 10;
				InvalidateRect(hwnd, &r, FALSE);
			}
		}
		break;
	}
	case WM_WINDOWPOSCHANGED: {
		LPWINDOWPOS pwp = (LPWINDOWPOS)lParam;
		if (!(pwp->flags & SWP_NOSIZE)) {
			RECT r;
			GetClientRect(hwnd, &r);
			pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
			tabcontrol_makeActiveTabVisible(hwnd, pControl);
			tabcontrol_resizeActiveTabContents(hwnd, pControl);
			r.bottom = pControl->tc_tabstripRect.bottom+1;
			InvalidateRect(hwnd, &r, FALSE);
		}
		return TRUE;
	}

	case WM_DESTROY:
		pControl = (TAB_CONTROL*)GetWindowLongPtr(hwnd, GWLP_TAB_CONTROL);
		SetWindowLongPtr(hwnd, GWLP_TAB_CONTROL, (LONG_PTR)NULL);
		tabcontrol_destroy(pControl);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
 * Add a window with the given window class to our tab control managing the edit windows. 
 */
HWND mainframe_addWindow(OPEN_HINT* pHint, const char* pszChildWindowClass, const char* pszTitle, LPVOID lParam) {
	DOCKING_SLOT* pSlot = mainframe_getSlot(pHint->oh_slotName);
	if (pSlot == NULL) {
		mainframe_manageDocks(MD_ENSURE_DEFAULT);
		pSlot = dockingSlots;
	}
	HWND hwnd = CreateWindow(pszChildWindowClass, pszTitle, WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 10, 10, pSlot->ds_hwnd, NULL, hInst, lParam);
	if (hwnd == NULL) {
		return 0;
	}
	tabcontrol_addTab(pSlot->ds_hwnd, hwnd, pHint->oh_activate);
	if (pHint->oh_slotName != NULL) {
		mainframe_moveWindowAndActivate(hwnd, pHint->oh_slotName, pHint->oh_activate);
	}
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
 * Returns the rect describing the "empty space", into which the docking windows
 * can be placed.
 */
static void mainframe_getDockingRect(HWND hwnd, RECT* pRect) {
	WORD nWidth;
	WORD nToolbarHeight;
	WORD nStatusHeight;
	WORD nFkeyHeight;
	GetClientRect(hwnd, pRect);
	tb_wh(&nWidth, &nToolbarHeight);
	status_wh(&nWidth, &nStatusHeight);
	fkey_getKeyboardSize(&nWidth, &nFkeyHeight);
	pRect->top = nToolbarHeight;
	pRect->bottom -= (nFkeyHeight + nStatusHeight);
}

/*
 * Resize our "docking slots" depending on their given ratios 
 */
static void mainframe_arrangeDockingSlots(HWND hwnd) {
	RECT rect;

	mainframe_getDockingRect(hwnd, &rect);
	DOCKING_SLOT* pSlot;
	if (mainframe_useFullScreenMode()) {
		pSlot = mainframe_getDockingParent(GetParent(GetFocus()));
		if (pSlot) {
			MoveWindow(pSlot->ds_hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 1);
			return;
		}
	}
	pSlot = dockingSlots;
	THEME_DATA* pTheme = theme_getCurrent();
	int dDelta = pTheme->th_mainframeMargin;
	int width = rect.right - rect.left - dDelta;
	int height = rect.bottom - rect.top - dDelta;
	while (pSlot) {
		if (pSlot->ds_wratio < 0.1) {
			pSlot->ds_wratio = 0.1f;
		}
		if (pSlot->ds_hratio < 0.1) {
			pSlot->ds_hratio = 0.1f;
		}
		int x = rect.left + dDelta + (int)(width * pSlot->ds_xratio);
		int y = rect.top + dDelta + (int)(height * pSlot->ds_yratio);
		int w = (int)(width * pSlot->ds_wratio) - dDelta;
		int h = (int)(height * pSlot->ds_hratio) - dDelta;
		pSlot->ds_resizeRect = (RECT){ 0,0,0,0 };
		MoveWindow(pSlot->ds_hwnd, x, y, w, h, 1);
		pSlot = pSlot->ds_next;
	}
	// Make the active tab visible, if the size of the docks has changed.
	mainframe_tabLayoutChanged();
	pSlot = dockingSlots;
	while (pSlot) {
		DOCKING_SLOT* pSibblingSlot = dockingSlots;
		RECT rc1;
		GetWindowRect(pSlot->ds_hwnd, &rc1);
		MapWindowPoints(HWND_DESKTOP, hwndFrameWindow, (LPPOINT)&rc1, 2);
		while (pSibblingSlot) {
			if (pSibblingSlot != pSlot) {
				RECT rc2;
				GetWindowRect(pSibblingSlot->ds_hwnd, &rc2);
				MapWindowPoints(HWND_DESKTOP, hwndFrameWindow, (LPPOINT)&rc2, 2);
				if (pSibblingSlot->ds_xratio == pSlot->ds_xratio) {
					if (pSibblingSlot->ds_yratio > pSlot->ds_yratio) {
						pSlot->ds_resizeRect = (RECT){ rc1.left, rc1.bottom, rc2.right, rc2.top };
					}
				} else if (pSibblingSlot->ds_yratio == pSlot->ds_yratio) {
					if (pSibblingSlot->ds_xratio < pSlot->ds_xratio) {
						pSlot->ds_resizeRect = (RECT){ rc2.right, rc1.top, rc1.left, rc1.bottom };
					}
				}
			}
			pSibblingSlot = pSibblingSlot->ds_next;
		}
		pSlot = pSlot->ds_next;
	}
}

/*
 * Given an lParam representing a mouse position, find a splitter rectangle on the main screen. If
 * no rect can be found containing the mouse pointer NULL is returned.
 */
static RECT* mainframe_findSplitterRect(LPARAM lParam) {
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	DOCKING_SLOT* pSlot = dockingSlots;
	MapWindowPoints(HWND_DESKTOP, hwndFrameWindow, (LPPOINT)&pt, 1);
	while (pSlot) {
		if (PtInRect(&pSlot->ds_resizeRect, pt)) {
			return &pSlot->ds_resizeRect;
		}
		pSlot = pSlot->ds_next;
	}
	return NULL;
}

/*
 * Calculate the information necessary to drag a splitter and resize docking slots. 
 */
typedef struct tagSPLIT_DRAG {
	struct tagSPLIT_DRAG* sd_next;
	DOCKING_SLOT* sd_slot;
	BOOL sd_move;				// Is this slot being moved to a new position?
	RECT sd_originalSize;		// The size of the slot, when the drag operation starts.
	RECT sd_newSize;			// The modified size of the slot - result of the split drag.
} SPLIT_DRAG;
static SPLIT_DRAG* mainframe_determineResizedSlots(RECT* pRect, BOOL bDragX) {
	SPLIT_DRAG* pHead = NULL;
	SPLIT_DRAG* pInfo;
	DOCKING_SLOT* pSlot = dockingSlots;
	while (pSlot) {
		RECT rect;
		GetWindowRect(pSlot->ds_hwnd, &rect);
		MapWindowPoints(HWND_DESKTOP, hwndFrameWindow, (LPPOINT)&rect, 2);
		if (bDragX) {
			if (rect.left >= pRect->right || rect.right <= pRect->left) {
				pInfo = ll_insert(&pHead, sizeof * pInfo);
				pInfo->sd_originalSize = rect;
				pInfo->sd_newSize = rect;
				pInfo->sd_slot = pSlot;
				pInfo->sd_move = rect.left >= pRect->right;
			}
		} else {
			if (rect.top >= pRect->bottom || rect.bottom <= pRect->top) {
				pInfo = ll_insert(&pHead, sizeof * pInfo);
				pInfo->sd_originalSize = rect;
				pInfo->sd_newSize = rect;
				pInfo->sd_slot = pSlot;
				pInfo->sd_move = rect.top >= pRect->bottom;
			}
		}
		pSlot = pSlot->ds_next;
	}
	return pHead;
}

/*
 * Apply the split positions dragged to a new position. 
 */
static void mainframe_splitterDragged(HWND hwnd, SPLIT_DRAG* pDrags, BOOL bDragX) {
	SPLIT_DRAG* pProcess = pDrags;
	RECT rcClient;
	mainframe_getDockingRect(hwnd, &rcClient);
	THEME_DATA* pTheme = theme_getCurrent();
	int dDelta = pTheme->th_mainframeMargin;
	InflateRect(&rcClient, -dDelta, -dDelta);
	float fWidth = (float)(rcClient.right - rcClient.left) - 2 * dDelta;
	float fHeight = (float)(rcClient.bottom - rcClient.top) - 2 * dDelta;
	DOCKING_SLOT* pRef = NULL;
	while (pProcess) {
		DOCKING_SLOT* pSlot = pProcess->sd_slot;
		if (bDragX) {
			if (pRef == NULL && pSlot->ds_xratio != 0) {
				pRef = pSlot;
			}
			pSlot->ds_wratio = (float)(pProcess->sd_newSize.right - pProcess->sd_newSize.left) / fWidth;
			pSlot->ds_xratio = (float)(pProcess->sd_newSize.left - rcClient.left) / fWidth;
			if (pSlot->ds_xratio < 0) {
				pSlot->ds_xratio = 0;
			}
			if (pSlot->ds_wratio + pSlot->ds_xratio > 1) {
				pSlot->ds_wratio = 1 - pSlot->ds_xratio;
			}
		} else {
			if (pRef == NULL && pSlot->ds_yratio != 0) {
				pRef = pSlot;
			}
			pSlot->ds_hratio = (float)(pProcess->sd_newSize.bottom - pProcess->sd_newSize.top) / fHeight;
			pSlot->ds_yratio = (float)(pProcess->sd_newSize.top - rcClient.top) / fHeight;
			if (pSlot->ds_yratio < 0) {
				pSlot->ds_yratio = 0;
			}
			if (pSlot->ds_hratio + pSlot->ds_yratio > 1) {
				pSlot->ds_hratio = 1 - pSlot->ds_yratio;
			}
		}
		pProcess = pProcess->sd_next;
	}
	DOCKING_SLOT* pSlot = dockingSlots;
	while (pSlot) {
		if (pSlot != pRef && pRef != NULL) {
			if (bDragX && pSlot->ds_yratio == 0) {
				pSlot->ds_wratio = pRef->ds_xratio;
			} else if (!bDragX) {
				pSlot->ds_hratio = pRef->ds_yratio;
			}
		} 
		pSlot = pSlot->ds_next;
	}
}

/*
 * Handle docking containers splitter dragging.
 */
static BOOL mainframe_dragSplitter(HWND hwnd, LPARAM lParam) {
	BOOL ret = FALSE;
	RECT* pRect = mainframe_findSplitterRect(lParam);
	if (!pRect) {
		return ret;
	}
	POINT ptOrig;
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	int b = 1;
	BOOL bDragX = pRect->right - pRect->left < pRect->bottom - pRect->top;
	GetCursorPos(&ptOrig);
	if (DragDetect(hwnd, (POINT) { x, y })) {
		int dummy;
		int delta;
		SPLIT_DRAG* pDrags = mainframe_determineResizedSlots(pRect, bDragX);
		SPLIT_DRAG* pProcess;
		SetCapture(hwnd);
		while (mouse_dispatchUntilButtonRelease(&x, &y, &b, &dummy) && b) {
			ret = TRUE;
			POINT pt;
			GetCursorPos(&pt);
			pProcess = pDrags;
			while (pProcess) {
				pProcess->sd_newSize = pProcess->sd_originalSize;
				if (bDragX) {
					delta = pt.x - ptOrig.x;
					if (pProcess->sd_move) {
						pProcess->sd_newSize.left += delta;
					} else {
						pProcess->sd_newSize.right += delta;
					}
				} else {
					delta = pt.y - ptOrig.y;
					if (pProcess->sd_move) {
						pProcess->sd_newSize.top += delta;
					} else {
						pProcess->sd_newSize.bottom += delta;
					}
				}
				if (delta != 0) {
					MoveWindow(pProcess->sd_slot->ds_hwnd, pProcess->sd_newSize.left, pProcess->sd_newSize.top,
						pProcess->sd_newSize.right - pProcess->sd_newSize.left, pProcess->sd_newSize.bottom - pProcess->sd_newSize.top, TRUE);
				}
				pProcess = pProcess->sd_next;
			}
		}
		if (ret) {
			mainframe_splitterDragged(hwnd, pDrags, bDragX);
			mainframe_saveDocks();
		}
		ll_destroy(&pDrags, NULL);
		ReleaseCapture();
		mainframe_arrangeDockingSlots(hwnd);
	}
	return ret;
}

static BOOL controlKeyChanged;
static BOOL mainWindowIsOther;

static BOOL mainframe_enumChildWindowProc(HWND hwnd, LPARAM pParam) {
	int* pCount = (int*)pParam;
	if (hwnd == hwndMain) {
		(*pCount)++;
	} else {
		char winClass[64];
		GetClassName(hwnd, winClass, sizeof winClass);
		if (strcmp(winClass, szFrameClass) == 0) {
			char winTitle[64];
			GetWindowText(hwnd, winTitle, sizeof winTitle);
			if (strcmp(_applicationName, winTitle) == 0) {
				mainWindowIsOther = TRUE;
			}
			(*pCount)++;
		}
	}
	return TRUE;
}

/*
 * Checks, whether there is an active PKS edit window already somewhere on the screen (maybe other process).
 */
static int mainframe_countCompanions() {
	int mainFrameCount = 0;
	EnumWindows(mainframe_enumChildWindowProc, (LPARAM) & mainFrameCount);
	return mainFrameCount;
}

/*------------------------------------------------------------
 * ww_closeChildWindow()
 */
static int ww_closeChildWindow(HWND hwndChild, LONG someFlags)
{
	LRESULT ret;

	if (!hwndChild || !IsWindow(hwndChild))
		return 0;
	if (someFlags) {
		WINFO* wp = ww_getWinfoForHwnd(hwndChild);
		if ((someFlags & CWF_EXCLUDE_SELECTED) && (wp == _selectedWindow)) {
			return 1;
		}
		if ((someFlags & CWF_EXCLUDE_CURRENT) && (wp == ww_getCurrentEditorWindow())) {
			return 1;
		}
		if ((someFlags & CWF_EXCLUDE_PINNED) && wp && ww_isPinned(wp)) {
			return 1;
		}
	}
	if ((ret = SendMessage(hwndChild, WM_QUERYENDSESSION, 0, 0L)) == 1) {
		SendMessage(hwndChild, WM_CLOSE, 0, 0);
		return 1;
	}

	return (ret) ? 1 : 0;
}

static void mainframe_updateTitle() {
	static int lastCount;
	char szTitle[120];
	mainWindowIsOther = FALSE;
	int nCount = mainframe_countCompanions();
	if (nCount == lastCount) {
		return;
	}
	lastCount = nCount;
	if (nCount > 1 && mainWindowIsOther) {
		wsprintf(szTitle, "* %s * (%d)", _applicationName, nCount);
	}
	else {
		lstrcpy(szTitle, _applicationName);
	}
	SetWindowText(hwndMain, szTitle);
}

static void ww_propagateThemeChange() {
	WINFO* wp = ww_getCurrentEditorWindow();
	while (wp) {
		PostMessage(wp->ww_handle, WM_THEMECHANGED, 0, 0);
		wp = wp->next;
	}
}

static int main_convertMenuId(int idCtrl) {
	int nType = idCtrl >> 8;
	if (nType == CMD_CMDSEQ || nType == CMD_MACRO) {
		idCtrl = (nType << 12) + (idCtrl & 0xFF);
	}
	return idCtrl;
}

/*
 * Window procedure of the main frame window.
 */
static LRESULT mainframe_windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static BOOL			bHelp = FALSE;
	static BOOL			appActivated;
	static BOOL			closed = FALSE;
	RECT				rect;
	WORD				fwMenu;
	WORD				w;
	WORD				nFkeyHeight;
	WORD				nStatusHeight;
	WORD				nToolbarHeight;
	int					idCtrl;
	LRESULT				lResult;

	if (UAHWndProc(hwnd, message, wParam, lParam, &lResult)) {
		return lResult;
	}
	switch (message) {

	case WM_ACTIVATEAPP:
		appActivated = (BOOL)wParam;
		if (appActivated) {
			mainframe_updateTitle();
		}
		break;
	case WM_CREATE:
		hwndFrameWindow = hwnd;
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
		THEME_DATA* pTheme = theme_getCurrent();
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
			if (nToolbarHeight && (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)) {
				SendMessage(hwndRebar, message, wParam, lParam);
			}
		}
		if (hwndFkeys) {
			if (!IsWindowVisible(hwndFkeys)) {
				ShowWindow(hwndFkeys, TRUE);
			}
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

	case WM_ACTIVATE: {
		if (closed) {
			break;
		}
		WINFO* wp = ww_getCurrentEditorWindow();
		if (wp != NULL && wParam != WA_INACTIVE) {
			SetFocus(wp->edwin_handle);
		}
		fkey_keyModifierStateChanged();
		break;
	}

	case WM_SETFOCUS: {
		WINFO* wp = ww_getCurrentEditorWindow();
		if (wp != NULL) {
			SetFocus(wp->edwin_handle);
		}
		break;
	}

	case WM_LANGUAGE_CHANGED:
		menu_refreshLanguageInMenubar(hwnd);
		fkey_languageChanged(hwnd);
		return TRUE;

	case WM_TIMER:
		if (closed) {
			break;
		}
		BOOL bActive = appActivated && hwndDlg == NULL;
		ft_checkForChangedFiles(bActive);
		if (controlKeyChanged) {
			controlKeyChanged = FALSE;
			fkey_keyModifierStateChanged();
		}
		ft_triggerAutosaveAllFiles();
		if (bActive) {
			st_redraw(FALSE);
		}
		break;

	case WM_WININICHANGE:
		darkmode_IsColorSchemeChangeMessage(message, lParam);
		string_initDateformats();
		if (dpisupport_scalingChanged(hwnd)) {
			WINFO* wp = ww_getCurrentEditorWindow();
			if (wp) {
				ww_setZoom(wp, wp->zoomMagnifier);
			}
			// todo: force repaint with new font-size in open windows.
			goto repaintUI;
		}
		break;

	case WM_MENUSELECT:
		fwMenu = GET_WM_MENUSELECT_FLAGS(wParam, lParam);
		if (!GET_WM_MENUSELECT_HMENU(wParam, lParam) &&
			(WORD)GET_WM_MENUSELECT_FLAGS(wParam, lParam) == 0xFFFF) {
			/* Menu closed */
			macro_showHelpForMenu(-1);
		}
		else if ((fwMenu & (MF_SYSMENU | MF_SEPARATOR | MF_POPUP)) == 0) {
			fwMenu = GET_WM_MENUSELECT_CMD(wParam, lParam);
			macro_showHelpForMenu(main_convertMenuId(fwMenu));
		}
		break;

	case WM_INITMENUPOPUP:
		if (!(BOOL)HIWORD(lParam)) {
			HMENU hMenu = (HMENU)wParam;
			menu_updateMenubarPopup(hMenu);
		}
		break;
	case WM_THEMECHANGED:
		ww_propagateThemeChange();
		darkmode_allowForApp(theme_getCurrent()->th_isDarkMode);
		theme_enableDarkMode(hwndFrameWindow);
		darkmode_flushMenuThemes();
repaintUI:
		tb_updateImageList(hwnd, NULL, 0);
		RedrawWindow(hwnd, NULL, 0, RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
		GetWindowRect(hwnd, &rect);
		// This ugly hack is necessary to force Windows to correctly redraw the frame of PKS-Edit, when the theme changes.
		if (IsZoomed(hwnd)) {
			// Window is currently maximized.
			WINDOWPLACEMENT placement = {0};
			GetWindowPlacement(hwnd, &placement);

			// Remember the old restore rect.
			const RECT oldrect = placement.rcNormalPosition;

			// Change the restore rect to almost the same as the current
			// maximized rect.
			placement.rcNormalPosition = rect;
			placement.rcNormalPosition.right -= 1;
			SetWindowPlacement(hwnd, &placement);

			// Restore and then re-maximize the window. Don't update in-between.
			LockWindowUpdate(hwnd);
			ShowWindow(hwnd, SW_SHOWNORMAL);
			ShowWindow(hwnd, SW_SHOWMAXIMIZED);
			LockWindowUpdate(NULL);

			// Put back the old restore rect.
			placement.rcNormalPosition = oldrect;
			SetWindowPlacement(hwnd, &placement);
		}
		else {
			HDWP defer = BeginDeferWindowPos(2);
			DeferWindowPos(defer, hwnd, NULL, 0, 0,
				rect.right - rect.left - 1, rect.bottom - rect.top,
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			DeferWindowPos(defer, hwnd, NULL, 0, 0, rect.right - rect.left,
				rect.bottom - rect.top,
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			LockWindowUpdate(hwnd);
			EndDeferWindowPos(defer);
			LockWindowUpdate(NULL);
		}
		break;

	case WM_NCHITTEST: {
		RECT* pRect = mainframe_findSplitterRect(lParam);
		if (pRect != NULL) {
			return (LRESULT)(pRect->right - pRect->left > pRect->bottom - pRect->top ?
				HTBOTTOM : HTRIGHT);
		}
		break;
	}

	case WM_NCLBUTTONDOWN:
		if (mainframe_dragSplitter(hwnd, lParam)) {
			return FALSE;
		}
		break;

	case WM_SETCURSOR: {
		WORD ncTest = LOWORD(lParam);
		if (ncTest == HTBOTTOM) {
			mouse_setSizeNSCursor();
			return TRUE;
		}
		else if (ncTest == HTRIGHT) {
			mouse_setSizeWECursor();
			return TRUE;
		}
		break;
	}

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
		if (LOWORD(wParam) == IDM_INCREMENTAL_SEARCH) {
			break;
		}
		idCtrl = (int)(wParam);
		if (idCtrl < 0) {
			break;
		}
		// special hack for toolbars supporting only 16 bit commands ??
		idCtrl = main_convertMenuId(idCtrl);
		int nCommand = macro_translateToOriginalMenuIndex(idCtrl);
		if (bHelp) {
			bHelp = FALSE;
			return EdHelpContext((DWORD)nCommand);
		}
		if (macro_onMenuAction(ww_getCurrentEditorWindow(), nCommand, NULL)) {
			return 1;
		}
		if (!IsWindow(hwnd) || !IsWindow(hwndFrameWindow)) {
			return 0;
		}
		break;

	case WM_PKSKEY:
		error_closeErrorWindow();
		macro_executeMacro(0, _executeKeyBinding);
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
		bl_destroyPasteList(FALSE);
		CloseClipboard();
		return 0;

	case WM_RENDERFORMAT:
		if (wParam == CF_TEXT) {
			clp_setdata(0);
		}
		return 0;

	case WM_CLOSE_DOCK:
		mainframe_closeDock((HWND)wParam);
		break;

	case WM_QUERYENDSESSION:
		DestroyWindow(hwnd);
		break;

	case WM_CLOSE:
		// Must invoke finalize before closing child windows, because we want to save the current session.
		FinalizePksEdit();
		mainframe_closeChildWindows(CWF_ALL);
		if (ww_getNumberOfOpenWindows()) {
			return 0;
		}
		main_cleanup();
		closed = TRUE;
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
HWND mainframe_open(HMENU hDefaultMenu) {
	DWORD dwStyle;

	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
	hwndFrameWindow = CreateWindow(szFrameClass, _applicationName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, 0, hDefaultMenu, hInst, 0);

	if (!hwndFrameWindow) {
		return 0;
	}
	theme_enableDarkMode(hwndFrameWindow);
	return hwndFrameWindow;
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
			controlKeyChanged = TRUE;
			break;
		}
		if (!ww_workWinHasFocus()) {
			if (!(msg->hwnd == hwndFrameWindow)) {
				break;
			}
		}
		WINFO* wp = ww_getCurrentEditorWindow();
		_executeKeyBinding = NULL;
		if (wp) {
			if (wp->codecomplete_handle && codecomplete_processKey(wp->codecomplete_handle, msg->message, msg->wParam)) {
				return 1;
			}
		}
		if (_executeKeyBinding || (_executeKeyBinding = macro_getKeyBinding(msg->wParam)) != 0) {
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
 * To be invoked, when the tab layout of the mainframe has changed due to e.g. a the title of a child window changes
 * or the docking slots are resized.
 */
void mainframe_tabLayoutChanged() {
	DOCKING_SLOT* pSlot = dockingSlots;
	while (pSlot) {
		if (pSlot->ds_type == DS_EDIT_WINDOW) {
			TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(pSlot->ds_hwnd, GWLP_TAB_CONTROL);
			if (pControl) {
				tabcontrol_repaintTabs(pSlot->ds_hwnd, pControl);
				tabcontrol_measureTabStrip(pSlot->ds_hwnd, pControl);
				tabcontrol_makeActiveTabVisible(pSlot->ds_hwnd, pControl);
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
static int mainframe_enumChildWindows(BOOL bHideTabsDuringEnum, int (*funcp)(HWND hwnd, LONG lParam), LONG lParam) {
	DOCKING_SLOT* pSlot = dockingSlots;
	DOCKING_SLOT* pNext;
	int ret = 1;
	while (pSlot) {
		pNext = pSlot->ds_next;
		if (pSlot->ds_type == DS_EDIT_WINDOW) {
			if (bHideTabsDuringEnum) {
				ShowWindow(pSlot->ds_hwnd, SW_HIDE);
			}
			HWND hwndCurrent = pSlot->ds_hwnd;
			TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(hwndCurrent, GWLP_TAB_CONTROL);
			ARRAY_LIST* pCopy = arraylist_clone(pControl->tc_pages);
			size_t nSize = arraylist_size(pCopy);
			for (int i = 0; i < nSize; i++) {
				TAB_PAGE* pPage = arraylist_get(pCopy, i);
				if (pPage->tp_hwnd && (ret = (*funcp)(pPage->tp_hwnd, lParam)) == 0) {
					if (lParam == CWF_ALL && ww_requestToCloseCancelled) {
						arraylist_destroy(pCopy);
						return 0;
					}
					ret = 0;
					break;
				}
				// May be closed by callback - in that case do not continue iterating pages
				if (!IsWindow(hwndCurrent)) {
					break;
				}
			}
			arraylist_destroy(pCopy);
			if (bHideTabsDuringEnum) {
				ShowWindow(hwndCurrent, SW_SHOW);
			}
		}
		pSlot = pNext;
	}
	return ret;
}

/*------------------------------------------------------------
 * Close all child windows given a flag defining the windows to really be closed.
 */
long long mainframe_closeChildWindows(CLOSE_WINDOW_FLAGS someFlags) {
	mainframe_enumChildWindows(TRUE, ww_closeChildWindow, someFlags);

	// no exit: still windows alive
	if (ww_getNumberOfOpenWindows() != 0) {
		return 0;
	}
	return 1;
}

/*
 * Add new docks to the mainframe. 
 */
long long mainframe_manageDocks(MANAGE_DOCKS_TYPE mType) {
	DOCKING_SLOT* pDefaultSlot = mainframe_getSlot(szDefaultSlotName);
	DOCKING_SLOT* pRightSlot = mainframe_getSlot(DOCK_NAME_RIGHT);
	DOCKING_SLOT* pBottomSlot = mainframe_getSlot(DOCK_NAME_BOTTOM);
	BOOL bChanged = FALSE;

	if (!pDefaultSlot) {
		pDefaultSlot = mainframe_addDockingSlot(DS_EDIT_WINDOW, hwndFrameWindow, szDefaultSlotName, 0, 0, 1, 1);
		if (!pDefaultSlot) {
			return FALSE;
		}
		bChanged = TRUE;
	}
	if (mType == MD_ADD_HORIZONTAL) {
		if (!pRightSlot) {
			mainframe_addDockingSlot(DS_EDIT_WINDOW, hwndFrameWindow, DOCK_NAME_RIGHT, 0.5, 0, 0.5, 1);
			bChanged = TRUE;
		}
	} else if (mType == MD_ADD_VERTICAL) {
		if (!pBottomSlot) {
			mainframe_addDockingSlot(DS_EDIT_WINDOW, hwndFrameWindow, DOCK_NAME_BOTTOM, 0, 0.5, 1, 0.5);
			bChanged = TRUE;
		}
	}
	if (bChanged) {
		mainframe_applyDefaultSlotSizes();
		mainframe_arrangeDockingSlots(hwndFrameWindow);
		mainframe_saveDocks();
	}
	return 1;
}

/*
 * Move a mainframe window to a preferred docking slot. 
 */
void mainframe_moveWindowAndActivate(HWND hwndEdit, const char* pszPreferredSlot, BOOL bActivate) {
	if (strcmp(DOCK_NAME_RIGHT, pszPreferredSlot) == 0) {
		mainframe_manageDocks(MD_ADD_HORIZONTAL);
	} else if (strcmp(DOCK_NAME_BOTTOM, pszPreferredSlot) == 0) {
		mainframe_manageDocks(MD_ADD_VERTICAL);
	} else {
		mainframe_manageDocks(MD_ENSURE_DEFAULT);
	}
	DOCKING_SLOT* pSlot = mainframe_getSlot(pszPreferredSlot);
	if (pSlot == NULL) {
		return;
	}
	DOCKING_SLOT* pExisting = mainframe_getDockingParent(hwndEdit);
	if (pExisting == NULL) {
		return;
	}
	TAB_CONTROL* pSource = (TAB_CONTROL*)GetWindowLongPtr(pExisting->ds_hwnd, GWLP_TAB_CONTROL);
	TAB_PAGE* pPage = tabcontrol_getTab(pSource, hwndEdit);
	if (pPage != NULL) {
		TAB_CONTROL* pTarget = (TAB_CONTROL*)GetWindowLongPtr(pSlot->ds_hwnd, GWLP_TAB_CONTROL);
		if (pExisting != pSlot) {
			tabcontrol_moveTab(pSource, pTarget, pPage, bActivate);
		}
		if (bActivate) {
			tabcontrol_selectPage(pSource->tc_hwnd, pPage->tp_hwnd);
		}
	}
}


/*
 * Invoked, when a new editor is activated. Used to mark the current active window.
 */
void mainframe_windowActivated(HWND hwndOld, HWND hwndNew) {
	DOCKING_SLOT* pSlot1 = NULL;
	DOCKING_SLOT* pSlot2 = NULL;
	if (hwndOld != NULL) {
		pSlot1 = mainframe_getDockingParent(hwndOld);
	}
	if (hwndNew != NULL) {
		pSlot2 = mainframe_getDockingParent(hwndNew);
	}
	if (pSlot1 != pSlot2) {
		if (pSlot1 != NULL) {
			tabcontrol_repaintTabs(pSlot1->ds_hwnd, (TAB_CONTROL*) GetWindowLongPtr(pSlot1->ds_hwnd, GWLP_TAB_CONTROL));
		}
		if (pSlot2 != NULL) {
			tabcontrol_repaintTabs(pSlot2->ds_hwnd, (TAB_CONTROL*)GetWindowLongPtr(pSlot2->ds_hwnd, GWLP_TAB_CONTROL));
		}
	}
}

/*
 * Returns the window dock name for a given window.
 */
char* mainframe_getDockName(HWND hwnd) {
	DOCKING_SLOT* pSlot = mainframe_getDockingParent(hwnd);
	if (pSlot == NULL) {
		return szDefaultSlotName;
	}
	return pSlot->ds_name;
}

/*
 * Returns a string to be used as an open hint later, when opening the window.
 */
char* mainframe_getOpenHint(WINFO* wp, BOOL bFocus, BOOL bClone, int nDisplayMode) {
	static char szHint[100];
	HWND hwnd = wp->edwin_handle;
	DOCKING_SLOT* pSlot = mainframe_getDockingParent(hwnd);
	if (pSlot == NULL) {
		return szDefaultSlotName;
	}
	strcpy(szHint, pSlot->ds_name);
	TAB_CONTROL* pControl = (TAB_CONTROL*)GetWindowLongPtr(pSlot->ds_hwnd, GWLP_TAB_CONTROL);
	if (pControl) {
		TAB_PAGE* pPage = arraylist_get(pControl->tc_pages, pControl->tc_activeTab);
		BOOL bActive = pPage && pPage->tp_hwnd == hwnd;
		strcat(szHint, " ");
		strcat(szHint, bActive ? "active" : "-");
		strcat(szHint, " ");
		strcat(szHint, bFocus ? "focus" : "-");
		strcat(szHint, " ");
		strcat(szHint, bClone ? "cloned" : "-");
		strcat(szHint, " ");
		strcat(szHint, wp->linkedWindow != NULL && !wp->linkedWindow->lw_usedForComparison ? "link" : "-");
		char szText[18];
		sprintf(szText, " %x", nDisplayMode);
		strcat(szHint, szText);
	}
	return szHint;
}

/*
 * Parse an open hint text. Note, that the hint text is modified during parsing.
 */
OPEN_HINT mainframe_parseOpenHint(char* pszHint) {
	static char szDock[12];
	BOOL bActive = FALSE;
	BOOL bClone = FALSE;
	BOOL bFocus = FALSE;
	BOOL bLink= FALSE;
	int nDisplayMode = -1;
	if (pszHint != NULL) {
		char* pszDelim = " ";
		char *pszToken = strtok(pszHint, pszDelim);
		if (pszToken) {
			strcpy(szDock, pszToken);
			pszToken = strtok(NULL, pszDelim);
		}
		while (pszToken) {
			if (strcmp("active", pszToken) == 0) {
				bActive = TRUE;
			} else if (strcmp("focus", pszToken) == 0) {
				bFocus = TRUE;
			} else if (strcmp("cloned", pszToken) == 0) {
				bClone = TRUE;
			} else if (strcmp("link", pszToken) == 0) {
				bLink = TRUE;
			} else if (strcmp("-", pszToken) != 0 && strcmp("ffffffff", pszToken) != 0) {
				nDisplayMode = strtol(pszToken, NULL, 16);
			}
			pszToken = strtok(NULL, pszDelim);
		}
	}
	return (OPEN_HINT) {
		.oh_slotName = pszHint,
		.oh_activate = bActive,
		.oh_focus = bFocus,
		.oh_clone = bClone,
		.oh_link = bLink,
		.oh_displayMode = nDisplayMode
	};
}

/*
 * Used to switch to full screen mode and back. 
 */
long long mainframe_toggleFullScreen() {
	static RECT previousBounds;
	static HMENU hDefaultMenu;
	static int oldLayoutOptions;
	DWORD dStyle = GetWindowLong(hwndFrameWindow, GWL_STYLE);
	_fullscreenMode = dStyle & WS_CAPTION;
	POINT point = { 0 };
	HMONITOR hMonitor = MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo = { sizeof(monitorInfo) };
	int nFlags = (OL_FKEYS | OL_OPTIONBAR | OL_SHOWSTATUS | OL_TOOLBAR);

	if (!GetMonitorInfo(hMonitor, &monitorInfo)) {
		return 0;
	}
	if (_fullscreenMode) {
		hDefaultMenu = GetMenu(hwndFrameWindow);
		SetMenu(hwndFrameWindow, NULL);
		oldLayoutOptions = GetConfiguration()->layoutoptions & nFlags;
		GetConfiguration()->layoutoptions &= ~nFlags;
		GetWindowRect(hwndFrameWindow, &previousBounds);
		SetWindowLongPtr(hwndFrameWindow, GWL_STYLE,
			WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_MAXIMIZE);
		SetWindowPos(hwndFrameWindow, 0, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	} else {
		GetConfiguration()->layoutoptions |= oldLayoutOptions;
		SetMenu(hwndFrameWindow, hDefaultMenu);
		SetWindowLongPtr(hwndFrameWindow, GWL_STYLE,
			WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE);
		SetWindowPos(hwndFrameWindow, 0, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
			monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);

	}
	fkey_visibilitychanged();
	mainframe_tabLayoutChanged();
	return 1;
}
