/*																					/*
 * DesktopIcons.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: PKS-EDIT Desktop Icons
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */

#include <stdio.h>
#include "customcontrols.h"

#include "trace.h"
#include "dial2.h"
#include "winterf.h"
#include "winfo.h"
#include "lineoperations.h"
#include "edtypes.h"
#include "errordialogs.h"
#include "stringutil.h"
#include "textblocks.h"
#include "editorconfiguration.h"
#include "desktopicons.h"

#pragma hdrstop

#include "iccall.h"
#include "pksedit.h"

#define	GWL_ICICON				GWL_ICCLASSVALUES+sizeof(void*)
#define	GWL_ICONEXTRA			GWL_ICICON+sizeof(void*)

#define	PROP_NOTDRAGGED			-4711
#define	IC_WIDTH	32

typedef struct tagICONCLASS {
	struct tagICONCLASS *next;
	ATOM	ic_name;
	int		ic_type;
	HICON	ic_icon1,ic_icon2;
	HCURSOR	ic_cursor;
} ICONCLASS;

extern int EdConfigureIcons(void);
extern int mac_onIconAction(HWND icHwnd, WPARAM wParam,  LPARAM dropped);
extern char *_strtolend;
extern int  LbGetText(HWND hwnd, int id, void *szBuff);

ICONCLASS *icEditIconClass;

static char szIconId[]     = "icons";
static char szIconTypes[]  = "icontypes";
static char szIconClass[]  = "PKSIconWindow";
static ICONCLASS *iconclasses;
static int  curric;
static HWND	hwndActive;

/*
 * Get the icon class data from the desktop icon window. 
 */
static ICONCLASS* ic_getData(HWND hwnd) {
	return (ICONCLASS*)GetWindowLongPtr(hwnd, GWL_ICCLASSVALUES);
}

/*------------------------------------------------------------
 * ic_closeIconWindow()
 * Close an icon window.
 */
int ic_closeIconWindow(HWND hwnd) 
{
	return CloseChildWindow((HWND)hwnd,1);
}

static int _windowCount = 0;

static WNDENUMPROC _countWindow(HWND hwnd, LPARAM unused) {
	if (ic_isIconWindow(hwnd)) {
		_windowCount++;
	}
	return TRUE;
}

/**
 * Generate a new title for an icon to create on the desktop.
 */
char* ic_generateIconTitle(ICONCLASS* icp) {
	static char title[30];

	_windowCount = 0;
	EnumChildWindows(hwndClient, _countWindow, 0l);
	_windowCount++;
	switch (icp->ic_type) {
	case ICID_FILE: sprintf(title, "File %d", _windowCount); break;
	case ICID_CLIP: sprintf(title, "Clipboard %d", _windowCount); break;
	case ICID_DIR: sprintf(title, "Folder %d", _windowCount); break;
	case ICID_EXEC: sprintf(title, "Macro %d", _windowCount); break;
	case ICID_HELP: sprintf(title, "Help %d", _windowCount); break;
	case ICID_PRINT: sprintf(title, "Printer %d", _windowCount); break;
	case ICID_TRASH: sprintf(title, "Trashcan %d", _windowCount); break;
	case ICID_UNDO: sprintf(title, "Undo Buffer %d", _windowCount); break;
	default: sprintf(title, "Icon %d", _windowCount); break;
	}
	return title;

}

/*------------------------------------------------------------
 * ic_ownerDrawIconType()
 */
void ic_ownerDrawIconType(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl)
{
	ICONCLASS	*icp;
	char		atomname[32];
	int		x,y;

	if ((icp = (ICONCLASS*)par) == 0 ||
	     GetAtomName(icp->ic_name,atomname,sizeof atomname) == 0)
		return;

	x = rcp->left + IC_WIDTH/2;
	y = rcp->top + (rcp->bottom-rcp->top-IC_WIDTH) / 2;

	TextOut(hdc, x, y, atomname, lstrlen(atomname));

	x += 80;
	DrawIcon(hdc, x, y, icp->ic_icon1);

	if (icp->ic_icon2 != icp->ic_icon1) {
		x += IC_WIDTH*3/2;
		DrawIcon(hdc, x, y, icp->ic_icon2);
	}
}

/*--------------------------------------------------------------------------
 * ic_onIconTypeSelection()
 */
void ic_onIconTypeSelection(HWND hDlg, WORD nItem, LONG lParam, void *p)
{
	ICONCLASS	*icp;

	icp = 0;
	LbGetText(hDlg, nItem, (char *)&icp);
	if (icp == 0) {
		return;
	}
	SetDlgItemText(hDlg, IDD_STRING1, ic_generateIconTitle(icp));
	EnableWindow(GetDlgItem(hDlg, IDD_PATH2SEL),
		(icp->ic_type == ICID_FILE || icp->ic_type == ICID_DIR) ?
		TRUE : FALSE);

}

/**
 * Return the default space occupied by an icon (including padding).
*/
static SIZE ic_getDefaultIconSpace() {
	SIZE size;

	size.cx = GetSystemMetrics(SM_CXICON) + 30;
	size.cy = GetSystemMetrics(SM_CYICON) + 24;
	return size;
}

/*------------------------------------------------------------
 * ic_measureIconTypeItem()
 */
void ic_measureIconTypeItem(MEASUREITEMSTRUCT *mp)
{
	mp->itemHeight = IC_WIDTH * 3 / 2;
}

/*--------------------------------------------------------------------------
 * ic_enableConfigurationDialogIcons()
 */
void ic_enableConfigurationDialogIcons(HWND hwnd, ICONCLASS *icp)
{
	EnableWindow(GetDlgItem(hwnd, IDD_CALLBACK2), icp ? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDD_CALLBACK3), icp ? TRUE: FALSE);
}

/*------------------------------------------------------------
 * ic_fillIconTypeList()
 */
void ic_fillIconTypeList(HWND hwnd, int nItem, void* selValue)
{
	ICONCLASS	*icp;

	SendDlgItemMessage(hwnd,nItem,LB_RESETCONTENT,0,0L);

	/* dont add first icon type: the edit window icon 
	 * type may not be used by the user
	 */
	for (icp = iconclasses; icp && icp->next; icp = icp->next)
		SendDlgItemMessage(hwnd,nItem,LB_ADDSTRING,0,(LPARAM)icp);
	SendDlgItemMessage(hwnd,nItem,LB_SELECTSTRING,(WPARAM)-1,(LPARAM)selValue);
	icp = (ICONCLASS *)selValue;
	if (icp) {
		EnableWindow(GetDlgItem(hwnd, IDD_PATH2SEL),
			(icp->ic_type == ICID_FILE || icp->ic_type == ICID_DIR) ?
				TRUE : FALSE);
	}
	ic_enableConfigurationDialogIcons(hwnd, icp);
}

/*------------------------------------------------------------
 * ic_findChildFromPoint()
 * Find the desktop icon located at the point passed as an argument or
 * 0, if that cannot be found.
 */
HWND ic_findChildFromPoint(HWND hwndParent, POINT *point)
{
	HWND	hwndFound;

	hwndFound = ChildWindowFromPoint(hwndParent, *point);
	if (hwndFound == hwndParent || hwndFound == NULL) {
		return NULL;
	}
	if (ic_isIconWindow(hwndFound)) {
		return hwndFound;
	}
	hwndParent = GetParent(hwndFound);
	while(hwndParent != 0) {
		if (ic_isIconWindow(hwndParent)) {
			return hwndParent;
		}
		hwndParent = GetParent(hwndParent);
	}
	return 0;
}

/*------------------------------------------------------------
 * ic_saveCurrentConfiguration()
 * Save the current icon configuration.
 */
static int ic_saveCurrentConfiguration(HWND hwnd)
{
	SendMessage(hwnd,WM_PROFSAVE,0,0L);
	return 1;
}

/*------------------------------------------------------------
 * ic_profsave()
 */
static int ic_profsave(HWND hwnd)
{
	char 		szB2[256],szBuf[256],title[64],atomname[32],id[12];
	RECT			r;
	ICONCLASS		*icp;

	if ((icp = ic_getData(hwnd)) == 0) {
		return 1;
	}

	GetAtomName(icp->ic_name,atomname,sizeof atomname);
	GetWindowText(hwnd,title,sizeof title);
	GetWindowRect(hwnd,&r);
	ScreenToClient(hwndClient,(POINT*)&r);
	wsprintf(szBuf,"%s,%s,%d %d,%s",
		atomname,title,(int)r.left,(int)r.top,
		ic_getParamForIcon(szB2,hwnd,-1));
	wsprintf(id,"I%d",curric);
	curric++;
	prof_savestring(szIconId,id,szBuf);
	return 1;
}

/*------------------------------------------------------------
 * ic_saveLocationInConfiguration()
 */
int ic_saveLocationInConfiguration()
{
	curric = 0;
	EdEnumChildWindows(ic_saveCurrentConfiguration);
	return 1;
}

/*------------------------------------------------------------
 * ic_dropped()
 */
static int ic_dropped(HWND hwnd, POINT *pt)
{
	HWND		hwndDropped;

	if ((hwndDropped = ic_findChildFromPoint(hwnd,pt)) != 0) {
		PostMessage(hwndDropped,WM_ICONDROP,ICACT_DROPPED,(LPARAM)hwnd);
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------
 * ic_attribs()
 */
static ICONCLASS *ic_attribs(char *szName)
{
	ATOM 		name;
	ICONCLASS 	*iap;

	name = FindAtom(szName);

	for (iap = iconclasses; iap; iap = iap->next) {
		if (iap->ic_name == name)
			break;
	}
	return iap;
}

/*------------------------------------------------------------
 * ic_getParamForIcon()
 */
char *ic_getParamForIcon(LPSTR szBuff, HWND hwnd,int n)
{
	char 	*s,szTmp[256];

	s = (LPSTR)GetWindowLongPtr(hwnd,GWL_ICPARAMS);

	if (s != 0) {
		if (n >= 0) {
			lstrcpy(szTmp,s);
			s = strtok(szTmp,"|");
			while(n-- > 0 && s) {
				s = strtok((char*)0,"|");
			}
			if (s) {
				lstrcpy(szBuff,s);
				s = szBuff;
			}
		}
	}
	return s ? s : "";
}

/*------------------------------------------------------------
 * ic_getIconType()
 */
int ic_getIconType(HWND hwnd)
{
	ICONCLASS		*icp;

	if ((icp = (ICONCLASS*)SendMessage(hwnd,WM_ICONCLASSVALUE,0,0L)) != 0)
		return icp->ic_type;
	return 0;
}

BOOL ic_isIconWindow(HWND hwnd) {
	CHAR	szClass[64];

	GetClassName(hwnd, szClass, sizeof szClass);
	return lstrcmp(szClass, szIconClass) == 0;
}


/*------------------------------------------------------------
 * ic_getActiveIconWindow()
 */
HWND ic_getActiveIconWindow(ICONCLASS **icp)
{
	HWND		hwnd;

	hwnd = hwndActive;
	if (hwnd == 0 ||
	    (*icp = (ICONCLASS*)SendMessage(hwnd,WM_ICONCLASSVALUE,0,0L)) == 0 ||
	    (*icp)->ic_type == ICID_EDIT)
		return 0;
	return hwnd;
}

/*------------------------------------------------------------
 * ic_active()
 * Return the active icon with the given title, params and icon class.
 */
HWND ic_active(const char* szTitle, const char * szParams, ICONCLASS **icClass)
{
	HWND		hwnd;
	ICONCLASS	*icp;

	if ((hwnd = ic_getActiveIconWindow(&icp)) == 0) {
		*icClass = 0;
		return 0;
	}
	GetWindowText(hwnd,szTitle,32);
	lstrcpy(szParams,(LPSTR)GetWindowLongPtr(hwnd,GWL_ICPARAMS));
	*icClass = icp;
	return hwnd;
}

/*------------------------------------------------------------
 * ic_changeIcon()
 * Assign a new title and new parameters to an icon.
 */
void ic_changeIcon(const char* szTitle, const char* szParams, ICONCLASS *icClass)
{
	HWND		hwnd;
	ICONCLASS	*icp;
	char*		pp;

	if (icClass == 0 || (hwnd = ic_getActiveIconWindow(&icp)) == 0)
		return;
	SetWindowText(hwnd,szTitle);
	if ((pp = (LPSTR)GetWindowLongPtr(hwnd,GWL_ICPARAMS)) != 0) {
		_free(pp);
	}
	SetWindowLongPtr(hwnd,GWL_ICPARAMS, (LONG_PTR) szParams);
	SetWindowLongPtr(hwnd,GWL_ICCLASSVALUES,(LONG_PTR) icClass);
	SendMessage(hwnd,WM_ICONSELECT,0,icClass->ic_type);
	SendRedraw(hwnd);
}

static void ic_redrawrect(RECT *pRect) {
	RedrawWindow(hwndClient, pRect, 0, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}


static void ic_redraw(HWND hwnd) {
	RECT	rect;

	if (!hwnd || !GetParent(hwnd)) {
		return;
	}
	GetClientRect(hwnd, &rect);
	MapWindowPoints(hwnd, GetParent(hwnd), (POINT *)&rect, 2);
	ic_redrawrect(&rect);
}


static void ic_setactive(HWND hwnd) {
	HWND	hwndOld;

	hwndOld = hwndActive;
	hwndActive = hwnd;
	ic_redraw(hwndOld);
	ic_redraw(hwndActive);
}
	
/*------------------------------------------------------------
 * IconWndProc()
 */
static WINFUNC IconWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int			ic_type;
	HICON		hIcon,holdIcon;
	ICONCLASS 	*icp;
	LPSTR		param;
	RECT		rect;
	char 		szBuf[256];
	PAINTSTRUCT	ps;
	HDC 		hdc;
	HFONT		hFont;
	POINT		pt;
	INT			nX;
	INT			nY;
static HCURSOR	hPreviousCursor;
static int		nButtonX;
static int		nButtonY;

	switch(message) {
		case WM_LBUTTONDBLCLK:
			mac_onIconAction(hwnd, ICACT_DCLICKED, 0L);
			break;
		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0) {
				case SC_RESTORE:
					return mac_onIconAction(hwnd, ICACT_DCLICKED, 0L);
				case SC_MOVE:			/* make the icon move */
				case SC_TASKLIST:
					break;
				default:
					return 0;
			}
			break;
		case WM_ICONSELECT:
			if ((icp = ic_getData(hwnd)) == 0)
				return 0;
			ic_type = icp->ic_type;
			if ( ic_type != lParam)
				return 0;
			param = ic_getParamForIcon(szBuf,hwnd,0);
			holdIcon = (HICON)GetWindowLongPtr(hwnd,GWL_ICICON);
			hIcon = bl_hasClipboardBlock((int)wParam,*param) ? 
				   	icp->ic_icon1 : icp->ic_icon2;
			if (hIcon == holdIcon)
				return 0;
			SetWindowLongPtr(hwnd,GWL_ICICON, (LONG_PTR) hIcon);
			SendRedraw(hwnd);
			/* drop through */

		case WM_ICONCLASSVALUE:
			return ic_getData(hwnd);

		case WM_PAINT:
			hIcon = (HICON) GetWindowLongPtr(hwnd,GWL_ICICON);
			hdc = BeginPaint(hwnd, &ps);
			if (hIcon) {
				GetClientRect(hwnd, &rect);
				SetMapMode(hdc,MM_TEXT);
				DrawIcon(hdc,(rect.right - GetSystemMetrics(SM_CXICON)) / 2,0,hIcon);
				GetWindowText(hwnd, szBuf, sizeof szBuf);
				rect.top = GetSystemMetrics(SM_CYICON);
				hFont = SelectObject(hdc, cust_getSmallEditorFont());
				if (hwndActive == hwnd) {
					SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
					SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
				} else {
					SetBkColor(hdc, GetSysColor(COLOR_APPWORKSPACE));
					SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
				}
				DrawText(hdc, szBuf, -1, &rect, DT_CENTER | DT_NOPREFIX | DT_WORDBREAK);
				SelectObject(hdc, hFont);
			}
			EndPaint(hwnd,&ps);
			return 0;
		case WM_WINDOWPOSCHANGING:
			ic_redraw(hwnd);
			break;
		case WM_MOVE:
			if (!IsWindowVisible(hwnd))
				return 1;
			ic_redraw(hwnd);
			break;
		case WM_SETFOCUS:
			if (hwnd != hwndActive) {
				ic_setactive(hwnd);
			}
			return 0;
		case WM_LBUTTONDOWN:
			nButtonX = LOWORD(lParam);
			nButtonY = HIWORD(lParam);
			return 0;
		case WM_MOUSEMOVE:
			if (GetCapture() == hwnd) {
				break;
			}
			if (GetKeyState(VK_LBUTTON) >= 0) {
				break;
			}
			if (abs(HIWORD(lParam) - nButtonY) < 1 ||
					abs(LOWORD(lParam) - nButtonX) < 1) {
				break;
			}
			SetCapture(hwnd);
			if ((hIcon = (HICON) GetWindowLongPtr(hwnd,GWL_ICICON)) != 0) {
				ShowWindow(hwnd, FALSE);
				UpdateWindow(hwndClient);
				hPreviousCursor = SetCursor(hIcon);
			}
			break;
		case WM_LBUTTONUP:
			if (GetCapture() == hwnd) {
				ReleaseCapture();
				if (hPreviousCursor) {
					SetCursor(hPreviousCursor);
					hPreviousCursor = 0;
				}
				nX = LOWORD(lParam);
				nY = HIWORD(lParam);
				pt.x = nX;
				pt.y = nY;
				ClientToScreen(hwnd, &pt);
				if (!ic_dropped(hwnd, &pt)) {
					GetClientRect(hwnd, &rect);
					nX -= rect.right / 2;
					nY -= GetSystemMetrics(SM_CYICON) / 2;
					pt.x = nX;
					pt.y = nY;
					MapWindowPoints(hwnd, GetParent(hwnd), &pt, 1);
					SetWindowPos(hwnd, 0, pt.x, pt.y, 0, 0,
							SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);
				} else {
					ShowWindow(hwnd, SW_SHOW);
				}
			}
			SetFocus(hwnd);
			return 0;
		case WM_PROFSAVE:
			ic_profsave(hwnd);
			return 1;

		case WM_ICONDROP:
			return mac_onIconAction(hwnd, wParam, lParam);

		case WM_SYSKEYDOWN:
			if (wParam == VK_RETURN) {
				EdConfigureIcons();
			}
			return 0;
		case WM_KEYDOWN:
			if (wParam == VK_DELETE) {
				ic_closeIconWindow((HWND) hwnd);
				return 0;
			}
			if (wParam == VK_RETURN) {
				SendMessage(hwnd, WM_LBUTTONDBLCLK, 0, 0);
			}
			break;
		case WM_ERASEBKGND:
			return 0;
		case WM_DESTROY:
			if ((param = (LPSTR)GetWindowLongPtr(hwnd,GWL_ICPARAMS)) != 0) {
				_free(param);
				SetWindowLongPtr(hwnd,GWL_ICPARAMS,0L);
			}
			if (hwnd == hwndActive) {
				hwndActive = 0;
			}
			return 0;

		case WM_QUERYENDSESSION:
			if (wParam) {
				GetWindowText(hwnd,szBuf,sizeof szBuf);
				if (ed_yn(IDS_MSGDELICON,szBuf) == IDYES) 
					return 1;
			}
			return 2;

		case WM_QUERYOPEN:
			return 0;
	}
#if 0
	return DefMDIChildProc(hwnd, message, wParam, lParam);
#endif
	return DefWindowProc(hwnd, message, wParam, lParam);
}

static int ic_selectClipboardIcon(HWND hwnd)
{
	SendMessage(hwnd,WM_ICONSELECT,0,(LPARAM)ICID_CLIP);
	return 1;
}

static int ic_selectTrashIcon(HWND hwnd)
{
	SendMessage(hwnd,WM_ICONSELECT,1,(LPARAM)ICID_TRASH);
	return 1;
}

/*------------------------------------------------------------
 * ic_redisplayIcons()
 * Update all icons to reflect the current editor state (clipboard has data etc...)
 */
int _icdirty = 1;
void ic_redisplayIcons(void)
{
	if (_icdirty) {
		EdEnumChildWindows(ic_selectClipboardIcon);
		EdEnumChildWindows(ic_selectTrashIcon);
		_icdirty= 0;
	}
}

/*------------------------------------------------------------
 * ic_classname()
 */
static char *ic_classname(LPSTR d, LPSTR szIconName)
{
	wsprintf(d,"Ic%sClass",szIconName);
	return d;
}

/*------------------------------------------------------------
 * ic_registerDesktopIconClass()
 */
int ic_registerDesktopIconClass(void)
{
	WNDCLASS  wc;

	cust_initializeWindowClassDefaults(&wc);
	wc.style |= CS_DBLCLKS;
	wc.lpfnWndProc = IconWndProc;
	wc.cbWndExtra = GWL_ICONEXTRA;
	wc.hIcon = 0;
	wc.lpszClassName = szIconClass;

	return RegisterClass(&wc);
}

/*------------------------------------------------------------
 * ic_createclass()
 */
static ICONCLASS *ic_createclass(LPSTR szIdClass,char szTypeChar,LPSTR szIconName,
					 	   LPSTR szCursor,LPSTR szIcon2)
{
	ICONCLASS *icp;

	if ((icp = (ICONCLASS*)ll_insert((LINKED_LIST**)&iconclasses,sizeof *icp)) == 0) {
	    	return 0;
	}

	icp->ic_icon1 = icp->ic_icon2 = szIconName ? LoadIcon(hInst, szIconName) : 0;
	if (szIcon2) {
		icp->ic_icon2 = LoadIcon(hInst, szIcon2);
	}
	icp->ic_cursor = (szCursor && *szCursor) ? LoadCursor(hInst,szCursor) : 
									   icp->ic_icon1;
	icp->ic_name = AddAtom(szIdClass);
	icp->ic_type = szTypeChar;
	return icp;
}

/*------------------------------------------------------------
 * ic_mk()
 */
static intptr_t ic_mk(LPSTR szIdClass, LONG lParam)
{
	char		 szBuf[256],*szIconName,*szTypeString,*szCursor,*szIcon2;

	prof_getPksProfileString(szIconTypes, szIdClass, szBuf, sizeof szBuf);

	szTypeString = strtok(szBuf,",");
	szIconName   = strtok((char*)0,",");
	szCursor     = strtok((char*)0,",");
	szIcon2      = strtok((char*)0,",");

	return 
		ic_createclass(szIdClass,*szTypeString,szIconName,szCursor,szIcon2) ? 1 : 0;
}

/*
 * Find the next point on the screen, where an icon can be placed.
 */
static POINT ic_findNextPoint(int width, int height) {
	RECT rect;
	POINT pt;
	pt.x = 20;
	pt.y = 20;
	GetWindowRect(hwndClient, &rect);
	for (int x1 = rect.right - width - 20; x1 > 20; x1 -= 2 * width) {
		for (int y1 = 20; y1 < rect.bottom - 20; y1 += 2 * height) {
			pt.x = x1;
			pt.y = y1;
			if (ic_findChildFromPoint(hwndClient, &pt) == NULL) {
				return pt;
			}
		}
	}
	return pt;
}

/*------------------------------------------------------------
 * ic_moveIcon()
 * Move an icon to a given position. IF CW_USEDEFAULT is specified, the next free position will be used.
 */
int ic_moveIcon(HWND hwnd, int x, int y)
{
	RECT	r;

	if (x == CW_USEDEFAULT && y == CW_USEDEFAULT) {
		SIZE size = ic_getDefaultIconSpace();
		POINT pt = ic_findNextPoint(size.cx, size.cy);
		x = pt.x;
		y = pt.y;
	} else if (x < 0)
		return 0;

	GetClientRect(hwnd,&r);
	MoveWindow(hwnd,x,y,r.right,r.bottom,0);
	return 1;
}

/*------------------------------------------------------------
 * ic_addIcon()
 * Adds an icon to the desktop with the given class descriptor, title and params.
 * Set x and y to CW_USEDEFAULT for a default placement.
 */
HWND ic_addIcon(ICONCLASS *icp, const char* szTitle, const char* szParams, int x, int y)
{
	HWND		hwnd;

	if (!icp) {
		return 0;
	}
	SIZE size = ic_getDefaultIconSpace();
	if (x == CW_USEDEFAULT || y == CW_USEDEFAULT) {
		POINT pt = ic_findNextPoint(size.cx, size.cy);
		x = pt.x;
		y = pt.y;
	}
	if ((hwnd = CreateWindowEx(WS_EX_TRANSPARENT, szIconClass,szTitle, WS_CHILD|WS_CLIPSIBLINGS,
			x, y, size.cx, size.cy, hwndClient, 0, hInst, (LPVOID)szParams)) == 0) {
		return 0;
	}
	SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE |
			SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	SetWindowLongPtr(hwnd,GWL_ICCLASSVALUES, (LONG_PTR) icp);
	SetWindowLongPtr(hwnd,GWL_ICPARAMS, (LONG_PTR) szParams);
	SetWindowLongPtr(hwnd,GWL_ICICON, (LONG_PTR)icp->ic_icon1);
	return hwnd;
}

/*------------------------------------------------------------
 * ic_place()
 */
static intptr_t ic_place(LPSTR szName, LONG lParam)
{
	char 	*szClassId,*szTitle,*szGeo,*szParams;
	char	szBuff[512];
	int		x,y;
	HWND	hwnd;
	ICONCLASS *icp;

	if (!prof_getPksProfileString(szIconId,szName,szBuff,sizeof szBuff) ||
	    (szClassId = strtok(szBuff,",")) == 0 ||
	    (szTitle = strtok((char*)0,",")) == 0||
	    (szGeo  = strtok((char*)0,",")) == 0)
		return 0;
	
	if ((szParams = strtok((char*)0,",")) != 0) {
		szParams = stralloc(szParams);
	}

	if ((icp = ic_attribs(szClassId)) == 0) {
		return 0;
	}
	x = (int)Atol(szGeo);
	y = (int)Atol(_strtolend);
	hwnd = ic_addIcon(icp,szTitle,szParams,x,y);

	return (intptr_t)hwnd;
}

/*------------------------------------------------------------
 * ic_init()
 * Initializes the PKS edit icons.
 */
EXPORT int ic_init(void)
{
	icEditIconClass = ic_createclass("Icon-Edit",ICID_EDIT,(LPSTR)0,(LPSTR)0,(LPSTR)0);
	return (prof_enum(szIconTypes,ic_mk,0L) && 
		   prof_enum(szIconId,ic_place,0L));
}

