/*																					/*
 * DesktopIcons.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: PKS-EDIT Desktop Icons
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

#include "trace.h"
#include "dial2.h"
#include "winterf.h"
#include "winfo.h"
#include "edtypes.h"
#include "errordialogs.h"

#pragma hdrstop

#include	"iccall.h"
#include "pksedit.h"

#define	GWL_ICICON				GWL_ICCLASSVALUES+sizeof(LONG)
#define	GWL_ICONEXTRA			GWL_ICICON+sizeof(LONG)

#define	PROP_NOTDRAGGED			-4711

typedef struct tagICONCLASS {
	struct tagICONCLASS *next;
	ATOM	ic_name;
	int		ic_type;
	HICON	ic_icon1,ic_icon2;
	HCURSOR	ic_cursor;
} ICONCLASS;

extern int EdConfigureIcons(void);
extern int do_icon(HWND icHwnd, WPARAM wParam,  LPARAM dropped);
extern unsigned char* stralloc(unsigned char* buf);
extern void *ll_insert(void *head,long size);
extern long Atol(char *s);
extern char *_strtolend;
extern int  prof_savestring(char *grp, char *ident, char *string);
extern void prof_adjustpoint(PPOINT pPoint);
extern int  LbGetText(HWND hwnd, WORD id, char *szBuff);
extern HFONT EdSmallFont(void);

ICONCLASS *icEditIconClass;

static char szIconId[]     = "icons";
static char szIconTypes[]  = "icontypes";
static char szIconClass[]  = "PKSIconWindow";
static ICONCLASS *iconclasses;
static int  curric;
static HWND	hwndActive;

/*------------------------------------------------------------
 * EdIconsQuit()
 */
int EdIconsQuit(LONG hwnd) 
{
	return CloseChildWin((HWND)hwnd,1);
}

/*------------------------------------------------------------
 * ic_lboxdraw()
 */
#define	IC_WIDTH	32
void ic_lboxdrawitem(HDC hdc, RECT *rcp, DWORD par, int nItem, int nCtl)
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
 * ic_lboxselchange()
 */
void	ic_lboxselchange(HWND hDlg, WORD nItem, LONG lParam, void *p)
{
	ICONCLASS	*icp;

	icp = 0;
	LbGetText(hDlg, nItem, (char *)&icp);
	if (icp == 0) {
		return;
	}
	EnableWindow(GetDlgItem(hDlg, IDD_PATH2SEL),
		(icp->ic_type == ICID_FILE || icp->ic_type == ICID_DIR) ?
			TRUE : FALSE);
}

/*------------------------------------------------------------
 * ic_lboxmeasureitem()
 */
void ic_lboxmeasureitem(MEASUREITEMSTRUCT *mp)
{
	mp->itemHeight = IC_WIDTH * 3 / 2;
}

/*--------------------------------------------------------------------------
 * ic_enablecallbacks()
 */
void ic_enablecallbacks(HWND hwnd, ICONCLASS *icp)
{
	EnableWindow(GetDlgItem(hwnd, IDD_CALLBACK2), icp ? TRUE: FALSE);
	EnableWindow(GetDlgItem(hwnd, IDD_CALLBACK3), icp ? TRUE: FALSE);
}

/*------------------------------------------------------------
 * ic_lboxfill()
 */
void ic_lboxfill(HWND hwnd, int nItem, long selValue)
{
	ICONCLASS	*icp;

	SendDlgItemMessage(hwnd,nItem,LB_RESETCONTENT,0,0L);

	/* dont add first icon type: the edit window icon 
	 * type may not be used by the user
	 */
	for (icp = iconclasses; icp && icp->next; icp = icp->next)
		SendDlgItemMessage(hwnd,nItem,LB_ADDSTRING,0,(LONG)icp);
	SendDlgItemMessage(hwnd,nItem,LB_SELECTSTRING,(WPARAM)-1,selValue);
	icp = (ICONCLASS *)selValue;
	if (icp) {
		EnableWindow(GetDlgItem(hwnd, IDD_PATH2SEL),
			(icp->ic_type == ICID_FILE || icp->ic_type == ICID_DIR) ?
				TRUE : FALSE);
	}
	ic_enablecallbacks(hwnd, icp);
}

/*------------------------------------------------------------
 * FindChildFromPoint()
 */
HWND FindChildFromPoint(HWND hwnd, POINT *point)
{
	HWND	hwndFound;
	HWND	hwndParent;

	hwndFound = WindowFromPoint(*point);
	if (hwndFound == hwnd) {
		return 0;
	}
	hwndParent = GetParent(hwndFound);
	while(hwndParent != 0) {
		if (hwndParent == hwndClient) {
			return hwndFound;
		}
		hwndFound = hwndParent;
		hwndParent = GetParent(hwndParent);
	}
	return 0;
}

/*------------------------------------------------------------
 * ic_sp()
 */
static int ic_sp(HWND hwnd)
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

	if ((icp = (ICONCLASS*)GetWindowLong(hwnd,GWL_ICCLASSVALUES)) == 0) 
		return 1;

	GetAtomName(icp->ic_name,atomname,sizeof atomname);
	GetWindowText(hwnd,title,sizeof title);
	GetWindowRect(hwnd,&r);
	ScreenToClient(hwndClient,(POINT*)&r);
	wsprintf(szBuf,"%s,%s,%d %d,%s",
		atomname,title,(int)r.left,(int)r.top,
		ic_param(szB2,hwnd,-1));
	wsprintf(id,"I%d",curric);
	curric++;
	prof_savestring(szIconId,id,szBuf);
	return 1;
}

/*------------------------------------------------------------
 * ic_profsavepos()
 */
int ic_profsavepos(char *pszFn)
{
	curric = 0;
	EdEnumChildWindows(ic_sp);
	return 1;
}

/*------------------------------------------------------------
 * ic_dropped()
 */
static int ic_dropped(HWND hwnd, POINT *pt)
{
	HWND		hwndDropped;

	if ((hwndDropped = FindChildFromPoint(hwnd,pt)) != 0) {
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
 * ic_param()
 */
char *ic_param(LPSTR szBuff, HWND hwnd,int n)
{
	char 	*s,szTmp[256];

	s = (LPSTR)GetWindowLong(hwnd,GWL_ICPARAMS);

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
 * ic_type()
 */
int ic_type(HWND hwnd)
{
	ICONCLASS		*icp;

	if ((icp = (ICONCLASS*)SendMessage(hwnd,WM_ICONCLASSVALUE,0,0L)) != 0)
		return icp->ic_type;
	return 0;
}

BOOL ic_isicon(HWND hwnd) {
	CHAR	szClass[64];

	GetClassName(hwnd, szClass, sizeof szClass);
	return lstrcmp(szClass, szIconClass) == 0;
}


/*------------------------------------------------------------
 * ic_getactive()
 */
HWND ic_getactive(ICONCLASS **icp)
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
 */
HWND ic_active(LPSTR szTitle, LPSTR szParams, ICONCLASS **icClass)
{
	HWND		hwnd;
	ICONCLASS	*icp;

	if ((hwnd = ic_getactive(&icp)) == 0) {
		*icClass = 0;
		return 0;
	}
	GetWindowText(hwnd,szTitle,32);
	lstrcpy(szParams,(LPSTR)GetWindowLong(hwnd,GWL_ICPARAMS));
	*icClass = icp;
	return hwnd;
}

/*------------------------------------------------------------
 * ic_mod()
 */
void ic_mod(LPSTR szTitle, LPSTR szParams, ICONCLASS *icClass)
{
	HWND		hwnd;
	ICONCLASS	*icp;
	LPSTR	pp;

	if (icClass == 0 || (hwnd = ic_getactive(&icp)) == 0)
		return;
	SetWindowText(hwnd,szTitle);
	if ((pp = (LPSTR)GetWindowLong(hwnd,GWL_ICPARAMS)) != 0) {
		_free(pp);
	}
	SetWindowLong(hwnd,GWL_ICPARAMS,(LONG)szParams);
	SetWindowLong(hwnd,GWL_ICCLASSVALUES,(LONG)icClass);
	SendMessage(hwnd,WM_ICONSELECT,0,icClass->ic_type);
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
WINFUNC IconWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
			do_icon(hwnd, ICACT_DCLICKED, 0L);
			break;
		case WM_SYSCOMMAND:
			switch (wParam & 0xFFF0) {
				case SC_RESTORE:
					return do_icon(hwnd, ICACT_DCLICKED, 0L);
				case SC_MOVE:			/* make the icon move */
				case SC_TASKLIST:
					break;
				default:
					return 0;
			}
			break;
		case WM_ICONSELECT:
			if ((icp = (ICONCLASS*)GetWindowLong(hwnd,GWL_ICCLASSVALUES)) == 0)
				return 0;
			ic_type = icp->ic_type;
			if ( ic_type != lParam)
				return 0;
			param = ic_param(szBuf,hwnd,0);
			holdIcon = (HICON)GetWindowLong(hwnd,GWL_ICICON);
			hIcon = bl_avail(wParam,*param) ? 
				   	icp->ic_icon1 : icp->ic_icon2;
			if (hIcon == holdIcon)
				return 0;
			SetWindowLong(hwnd,GWL_ICICON,(long)hIcon);
			SendRedraw(hwnd);
			/* drop through */

		case WM_ICONCLASSVALUE:
			return GetWindowLong(hwnd,GWL_ICCLASSVALUES);

		case WM_PAINT:
			hIcon = (HICON) GetWindowLong(hwnd,GWL_ICICON);
			hdc = BeginPaint(hwnd, &ps);
			if (hIcon) {
				GetClientRect(hwnd, &rect);
				SetMapMode(hdc,MM_TEXT);
				DrawIcon(hdc,(rect.right - GetSystemMetrics(SM_CXICON)) / 2,0,hIcon);
				GetWindowText(hwnd, szBuf, sizeof szBuf);
				rect.top = GetSystemMetrics(SM_CYICON);
				hFont = SelectObject(hdc, EdSmallFont());
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
			if ((hIcon = (HICON) GetWindowLong(hwnd,GWL_ICICON)) != 0) {
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
			return do_icon(hwnd, wParam, lParam);

		case WM_SYSKEYDOWN:
			if (wParam == VK_RETURN) {
				EdConfigureIcons();
			}
			return 0;
		case WM_KEYDOWN:
			if (wParam == VK_DELETE) {
				EdIconsQuit((LONG) hwnd);
				return 0;
			}
			if (wParam == VK_RETURN) {
				SendMessage(hwnd, WM_LBUTTONDBLCLK, 0, 0);
			}
			break;
		case WM_ERASEBKGND:
			return 0;
		case WM_DESTROY:
			if ((param = (LPSTR)GetWindowLong(hwnd,GWL_ICPARAMS)) != 0) {
				_free(param);
				SetWindowLong(hwnd,GWL_ICPARAMS,0L);
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

/*------------------------------------------------------------
 * ic_selecticons()
 */
static int ic_sel1(HWND hwnd)
{
	SendMessage(hwnd,WM_ICONSELECT,0,(LONG)ICID_CLIP);
	return 1;
}

static int ic_sel2(HWND hwnd)
{
	SendMessage(hwnd,WM_ICONSELECT,1,(LONG)ICID_TRASH);
	return 1;
}

int _icdirty = 1;
void ic_selecticons(void)
{
	if (_icdirty) {
		EdEnumChildWindows(ic_sel1);
		EdEnumChildWindows(ic_sel2);
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
 * ic_register()
 */
int ic_register(void)
{
	WNDCLASS  wc;

	class_defaults(&wc);
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

	if ((icp = ll_insert(&iconclasses,sizeof *icp)) == 0) {
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
static int ic_mk(LPSTR szIdClass, LONG lParam)
{
	char		 szBuf[256],*szIconName,*szTypeString,*szCursor,*szIcon2;

	GetPksProfileString(szIconTypes, szIdClass, szBuf, sizeof szBuf);

	szTypeString = strtok(szBuf,",");
	szIconName   = strtok((char*)0,",");
	szCursor     = strtok((char*)0,",");
	szIcon2      = strtok((char*)0,",");

	return 
		ic_createclass(szIdClass,*szTypeString,szIconName,szCursor,szIcon2) ? 1 : 0;
}

/*------------------------------------------------------------
 * ic_place()
 */
int ic_position(HWND hwnd, int x, int y)
{
	RECT	r;

	if (x < 0)
		return 0;

	GetClientRect(hwnd,&r);
	MoveWindow(hwnd,x,y,r.right,r.bottom,0);
	return 1;
}

/*------------------------------------------------------------
 * ic_add()
 */
HWND ic_add(ICONCLASS *icp, LPSTR szTitle, LPSTR szParams, int x, int y)
{
	INT			width;
	INT			height;
	HWND		hwnd;

	if (!icp) {
		return 0;
	}
	width = GetSystemMetrics(SM_CXICON) + 30;
	height = GetSystemMetrics(SM_CYICON) + 24 ;
	if ((hwnd = CreateWindowEx(WS_EX_TRANSPARENT, szIconClass,szTitle, WS_CHILD|WS_CLIPSIBLINGS,
			x, y, width, height, hwndClient, 0, hInst, (LPVOID)szParams)) == 0) {
		return 0;
	}
	SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE |
			SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	SetWindowLong(hwnd,GWL_ICCLASSVALUES,(LONG)icp);
	SetWindowLong(hwnd,GWL_ICPARAMS,(LONG)szParams);
	SetWindowLong(hwnd,GWL_ICICON,(LONG)icp->ic_icon1);
	return hwnd;
}

/*------------------------------------------------------------
 * ic_place()
 */
static int ic_place(LPSTR szName, LONG lParam)
{
	char 	*szClassId,*szTitle,*szGeo,*szParams;
	char	szBuff[512];
	int		x,y;
	HWND	hwnd;
	ICONCLASS *icp;

	if (!GetPksProfileString(szIconId,szName,szBuff,sizeof szBuff) ||
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
	hwnd = ic_add(icp,szTitle,szParams,x,y);

	return (int)hwnd;
}

/*------------------------------------------------------------
 * ic_init()
 */
EXPORT int ic_init(void)
{
	icEditIconClass = ic_createclass("Icon-Edit",ICID_EDIT,(LPSTR)0,(LPSTR)0,(LPSTR)0);
	return (prof_enum(szIconTypes,ic_mk,0L) && 
		   prof_enum(szIconId,ic_place,0L));
}

