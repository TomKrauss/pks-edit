/*
 * CustomControls.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: customized controls for PKS-EDIT
 *
 * 										created: 01.01.90
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include "customcontrols.h"
#include "trace.h"
#include "documentmodel.h"

#include "winterf.h"
#include "winfo.h"
#include "winutil.h"
#include "themes.h"
#include <commctrl.h>

#pragma hdrstop

#define GWW_CUSTOMVAL		0
#define GWW_CUSTOMEXTRA		GWW_CUSTOMVAL+sizeof(WORD)

 /*
  * Draws a rounded rectangle in an antialiased way.
  */
extern void paint_roundedRect(HDC hdc, COLORREF cColor, float penWidth, int x, int y, int width, int height, int radius);

static const int toastWindowHeight = 40;
static const char* TOGGLE_CLASS = "toggle";
static const char* CHARSET_CLASS = "charset";
static const char* TOAST_CLASS = "pksToast";
static const char* LABELED_CLASS = "pksLabeled";

/*--------------------------------------------------------------------------
 * cust_drawShadow()
 * Draw a shadow around a control
 */
void cust_drawShadow(HDC hdc,RECT *rcp,int odItemState)
{
	static HPEN dialogPen;
	static COLORREF dialogColor;
	int			left,right,top,bottom;
	THEME_DATA* pData = theme_getCurrent();

	if (!dialogPen || pData->th_dialogBorder != dialogColor) {
		if (dialogPen) {
			DeleteObject(dialogPen);
		}
		dialogPen = CreatePen(PS_SOLID, 0, pData->th_dialogBorder);
		dialogColor = pData->th_dialogBorder;
	}
	left = rcp->left;
	right = rcp->right + rcp->left;
	top = rcp->top;
	bottom = rcp->top+rcp->bottom - 1;
	SelectObject(hdc,dialogPen);
	MoveTo(hdc,right,top);
	LineTo(hdc,left,top);
	LineTo(hdc,left,bottom);
	LineTo(hdc,right, bottom);
}

/*------------------------------------------------------------
 * cust_paintButton()
 */
static LOGFONT _lf =  {
    10,					// lfHeight;
    0,					// lfWidth;
 
    0,					// lfEscapement;
    0,					// lfOrientation;
    FW_NORMAL,				// lfWeight;
 
    0,					// lfItalic;
    0,					// lfUnderline;
    0,					// lfStrikeOut;
    
    ANSI_CHARSET,			// lfCharSet;
    OUT_DEFAULT_PRECIS,		// lfOutPrecision;
    CLIP_DEFAULT_PRECIS,	// lfClipPrecision;
    PROOF_QUALITY,			// lfQuality;
    FF_DONTCARE, 			// lfPitchAndFamily;
    "helv",					// lfFaceName[LF_FACESIZE];
};

/**
 * Return a handle to a small editor font.
 */
HFONT cust_getSmallEditorFont(void) {
	static HFONT hSmallFont;

	if (!hSmallFont) {
		_lf.lfHeight = 6;
		hSmallFont = CreateFontIndirect(&_lf);
	}
	return hSmallFont;
}

/**
 * Return a handle to a default font used by controls in PKS Edit.
 */
HFONT cust_getDefaultEditorFont(void) {
	static HFONT hNormalFont;

	if (!hNormalFont) {
		_lf.lfHeight = 8;
		hNormalFont = CreateFontIndirect(&_lf);
	}
	return hNormalFont;
}


#define STATE_CHECK		ODS_CHECKED
#define STATE_SEL		ODS_SELECTED

/**
 * Paint a custom button control.
 */
EXPORT void cust_paintButton(HDC hdc, RECT *rcp, HWND hwnd, int odItemState)
{
	HBRUSH 	hBrush;
	HFONT	hFont;
	DWORD	dwColtext;
	char 	szBuff[128];
	THEME_DATA* pTheme = theme_getCurrent();

	hFont = SelectObject(hdc, cust_getSmallEditorFont());
	if (odItemState & STATE_CHECK) {
		dwColtext = pTheme->th_dialogHighlightText;
	} else {
		dwColtext = pTheme->th_dialogForeground;
	}
	if (odItemState & ODS_DISABLED) {
		dwColtext = pTheme->th_dialogDisabled;
	}
	GetWindowText(hwnd,szBuff,sizeof szBuff);
	hBrush = (odItemState & STATE_CHECK) ? theme_getDialogLightBackgroundBrush() : theme_getDialogBackgroundBrush();
	SetBkMode(hdc,TRANSPARENT);

	//InflateRect(rcp, -2, -2);
	FillRect(hdc, rcp, hBrush);

	//InflateRect(rcp, 1, 1);
	cust_drawShadow(hdc,rcp,odItemState);

	SetTextColor(hdc,dwColtext);
	rcp->left += 3, rcp->top += 2;
	rcp->right -= 3; rcp->bottom -= 2;
	if ((odItemState & (ODS_FOCUS|ODS_DISABLED)) == ODS_FOCUS) {
		rcp->left++;
		rcp->top++;
	}
	DrawText(hdc, szBuff, -1, rcp, DT_NOPREFIX | DT_WORDBREAK | DT_LEFT | DT_VCENTER);

	SelectObject(hdc,hFont);
}

/*------------------------------------------------------------
 * cust_drawOutline()
 * Draw an outline rectangle.
 */
void cust_drawOutline(HDC hDC, int left, int top, int w, int h)
{
	int			wOldROP2;

	/* setup display context */
	wOldROP2 = SetROP2( hDC, R2_NOT);

	/* draw selector rectangle */
	Rectangle(hDC, left, top, left+w, top+h);

	/* restore display context */
	SetROP2( hDC, wOldROP2 );
}

/*
 * cust_createToolTooltip()
 * Create a tooltip to be displayed in/by a target window (hwndTool).
 */
HWND cust_createToolTooltip(HWND hwndTool) {
	HWND hwndTip = CreateWindowEx(0, TOOLTIPS_CLASS, 0,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwndTool, NULL, hInst,
		NULL);

	if (!hwndTip) {
		return NULL;
	}
	theme_enableDarkMode(hwndTip);
	TTTOOLINFO toolinfo = { 0 };
	toolinfo.cbSize = sizeof(toolinfo);
	toolinfo.hwnd = hwndTool;
	toolinfo.lpszText = "Hello world";
	toolinfo.uFlags = TTF_TRACK | TTF_ABSOLUTE;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolinfo);
	return hwndTip;
}


/*--------------------------------------------------------------------------
 * cust_calculateButtonCharacterHeight()
 * Calculates the height of a custom buttons character.
 */
int cust_calculateButtonCharacterHeight(HWND hwnd)
{
	HFONT		hFont;
	HDC			hdc;
	TEXTMETRIC	tm;
	int			fkcharheight;

	hdc = GetDC(hwnd);
	SetMapMode(hdc,MM_TEXT);
	hFont = SelectObject(hdc, cust_getSmallEditorFont());
	GetTextMetrics(hdc,&tm);
	fkcharheight = tm.tmHeight + tm.tmExternalLeading;
	SelectObject(hdc,hFont);
	ReleaseDC(hwnd, hdc);
	return fkcharheight;
}

/*------------------------------------------------------------
 * ToggleWndProc()
 */
static WINFUNC ToggleWndProc(HWND hwnd,UINT message,WPARAM wParam, LPARAM lParam)
{	
	RECT		rc;
	HDC			hdc;
	PAINTSTRUCT	ps;
	int			ww,bit,wwsav;

	switch(message) {
		case WM_ERASEBKGND:
			return 0;

		case WM_PAINT:
			ww = GetWindowWord(hwnd,GWW_CUSTOMVAL);
			GetClientRect(hwnd, &rc);
			hdc = BeginPaint(hwnd, &ps);
			if (!IsWindowEnabled(hwnd)) {
				ww |= ODS_DISABLED;
			}
			cust_paintButton(hdc, &rc, hwnd, ww);
			EndPaint(hwnd,&ps);
			return 0;
		case BM_SETCHECK:
		case BM_GETCHECK:
		case BM_GETSTATE:
		case BM_SETSTATE:
			ww = GetWindowWord(hwnd,GWW_CUSTOMVAL);
			if (message == BM_GETSTATE)
				return (ww & STATE_SEL) ? 1 : 0;
			if (message == BM_GETCHECK)
				return (ww & STATE_CHECK) ? 1 : 0;
			if (message == BM_SETSTATE)
				bit = STATE_SEL;
			else
				bit = STATE_CHECK;
			wwsav = ww;	
			ww = (wParam) ? (ww | bit) : (ww & (~bit));
			SetWindowWord(hwnd,GWW_CUSTOMVAL, (WORD)ww);
			if (ww != wwsav) {
				win_sendRedrawToWindow(hwnd);
			}
			return 0;
		case WM_LBUTTONDOWN:
			SendMessage(hwnd, BM_SETSTATE, 1, 0L);
			SetCapture(hwnd);
			return 0;
		case WM_KEYUP:
			if (wParam != VK_SPACE)
				break;
		case WM_LBUTTONUP:
			SendMessage(hwnd, BM_SETSTATE, 0, 0L);
			SendMessage(hwnd, BM_SETCHECK, 
					GetWindowWord(hwnd,GWW_CUSTOMVAL) ^ STATE_CHECK, 0L);
			win_sendParentCommand(hwnd,
					(LONG)GetWindowWord(hwnd,GWW_CUSTOMVAL));
			ReleaseCapture();
			return 0;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

/*------------------------------------------------------------
 * cust_drawCharSetItemSelection()
 */
#define N_COLS		32
#define N_ROWS		(256 / N_COLS)

 /*
 * Create the font for the char set window procedure
 */
static HFONT charset_createFont(THEME_DATA* pTheme, int nWeight) {
	return font_createFontHandle(pTheme->th_smallFontName, 15, 0, nWeight);
}

static void charset_notifyCharChange(HWND hwnd, int nNewChar) {
	SendMessage(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hwnd), CSN_CHARSELECT), nNewChar);
}

static void charset_rectFor(RECT* pClientRect, RECT* pPaint, int cChar) {
	int h = pClientRect->bottom - pClientRect->top;
	int w = pClientRect->right - pClientRect->left;
	int nCol = cChar % N_COLS;
	int nRow = cChar / N_COLS;
	pPaint->left = nCol * w / N_COLS;
	pPaint->top = nRow * h / N_ROWS;
	pPaint->right = (nCol + 1) * w / N_COLS;
	pPaint->bottom = (nRow + 1) * h / N_ROWS;
}

static void charset_paint(HWND hwnd, HDC hdc) {
	char 		buf[2];
	THEME_DATA* pTheme = theme_getCurrent();
	RECT rc;
	TEXTMETRIC tm;

	GetClientRect(hwnd, &rc);
	SetTextColor(hdc, pTheme->th_dialogForeground);
	SetBkMode(hdc, TRANSPARENT);
	SetMapMode(hdc, MM_TEXT);
	HFONT hFont = SelectObject(hdc, charset_createFont(pTheme, FW_NORMAL));
	GetTextMetrics(hdc, &tm);
	HPEN hPen = CreatePen(PS_SOLID, 1, pTheme->th_dialogBorder);
	HPEN hPenOld = SelectObject(hdc, hPen);
	int h = rc.bottom - rc.top;
	int w = rc.right - rc.left;
	for (int i = 1; i < N_ROWS; i++) {
		int y = i * h / N_ROWS;
		MoveTo(hdc, rc.left, y);
		LineTo(hdc, rc.right, y);
	}
	for (int i = 1; i < N_COLS; i++) {
		int x = i * w / N_COLS;
		MoveTo(hdc, x, rc.top);
		LineTo(hdc, x, rc.bottom);
	}
	MoveTo(hdc, rc.left, rc.top);
	LineTo(hdc, rc.right, rc.top);
	LineTo(hdc, rc.right, rc.bottom);
	LineTo(hdc, rc.left, rc.bottom);
	LineTo(hdc, rc.left, rc.top);
	DeleteObject(SelectObject(hdc, hPenOld));

	HBRUSH hBrush = CreateSolidBrush(pTheme->th_dialogForeground);
	int newc = GetWindowWord(hwnd, GWW_CUSTOMVAL);
	for (int nRow = 0; nRow < N_ROWS; nRow++) {
		for (int nCol = 0; nCol < N_COLS; nCol++) {
			RECT rect;
			int c = nRow * N_COLS + nCol;
			charset_rectFor(&rc, &rect, c);
			buf[0] = c;
			if (newc == c) {
				FillRect(hdc, &rect, hBrush);
				SetTextColor(hdc, pTheme->th_dialogBackground);
				if (hwnd == GetFocus()) {
					DrawFocusRect(hdc, &rect);
				}
			}
			TextOut(hdc, rect.left + (rect.right-rect.left-tm.tmAveCharWidth)/2, rect.top + (rect.bottom-rect.top-tm.tmHeight)/2, buf, 1);
			if (newc == c) {
				SetTextColor(hdc, pTheme->th_dialogForeground);
			}
		}
	}
	DeleteObject(SelectObject(hdc, hFont));
	DeleteObject(hBrush);
}

/*------------------------------------------------------------
 * CharSetWndProc()
 */
static WINFUNC CharSetWndProc(HWND hwnd,UINT message,WPARAM wParam, LPARAM lParam) {
	int	 newc,oldc;
	RECT rc;

	switch(message) {
		case WM_CREATE:
			SetWindowWord(hwnd,GWW_CUSTOMVAL,(WORD)-1);
			break;

		case WM_GETDLGCODE:
			return DLGC_WANTARROWS;		
			
		case WM_KEYDOWN : /* key being odItemState */
			/* process virtual key codes */
			newc = GetWindowWord(hwnd,GWW_CUSTOMVAL);
			switch( wParam ) {
				case VK_HOME: 	newc = 0; break;
				case VK_UP: 	newc -= N_COLS; break;
				case VK_LEFT: 	newc--; break;
				case VK_DOWN: 	newc += N_COLS; break;
				case VK_RIGHT:	newc++; break;
				case VK_END : 	newc = 255; break;
			}
			if (newc >= 256)
				newc -= 256;
			if (newc < 0)
				newc += 256;
			SendMessage(hwnd, WM_CHARCHANGE, newc, 0L);
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT	ps;
				BeginPaint(hwnd, &ps);
				charset_paint(hwnd, ps.hdc);
				EndPaint(hwnd, &ps);
			}
			return 0;
		case WM_ERASEBKGND: {
			HDC hdc = (HDC)wParam;
			GetClientRect(hwnd, &rc);
			FillRect(hdc, &rc, theme_getDialogBackgroundBrush());
		}
	  return 1;

		case WM_CHARCHANGE: {
			RECT rcPaint;
			oldc = GetWindowWord(hwnd,GWW_CUSTOMVAL);
			if (oldc == (WORD) wParam)	{
				return 0;
			}
			GetClientRect(hwnd, &rc);
			if (oldc >= 0) {
				charset_rectFor(&rc, &rcPaint, (int)oldc);
				InvalidateRect(hwnd, &rcPaint, TRUE);
			}
			SetWindowWord(hwnd, GWW_CUSTOMVAL, (WORD)wParam);
			if (wParam != 0) {
				charset_rectFor(&rc, &rcPaint, (int)wParam);
				InvalidateRect(hwnd, &rcPaint, TRUE);
			}
			charset_notifyCharChange(hwnd, (int)wParam);
			return 0;
		}

		case CS_QUERYCHAR:
			return GetWindowWord(hwnd,GWW_CUSTOMVAL);

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			InvalidateRect(hwnd, NULL, TRUE);
			return 1;

		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			{
				GetClientRect(hwnd, &rc);
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);
				newc = x * N_COLS / (rc.right-rc.left) + (y * N_ROWS / (rc.bottom - rc.top)) * N_COLS;
				if (newc < 0 || newc >= 256)
					return 0;
				SendMessage(hwnd, WM_CHARCHANGE, newc, 0L);

			}
			return 0;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

static void toast_paint(HWND hwnd, HDC hdc, char* pszText) {
	THEME_DATA* pTheme = theme_getCurrent();
	RECT rc;
	TEXTMETRIC tm;

	GetClientRect(hwnd, &rc);
	SetTextColor(hdc, pTheme->th_dialogErrorText);
	SetBkMode(hdc, TRANSPARENT);
	SetMapMode(hdc, MM_TEXT);
	HFONT hFont = SelectObject(hdc, charset_createFont(pTheme, FW_BOLD));
	GetTextMetrics(hdc, &tm);
	HPEN hPen = CreatePen(PS_SOLID, 2, pTheme->th_dialogBorder);
	HPEN hPenOld = SelectObject(hdc, hPen);
	MoveTo(hdc, rc.left+1, rc.top+1);
	LineTo(hdc, rc.right-1, rc.top+1);
	LineTo(hdc, rc.right-1, rc.bottom-1);
	LineTo(hdc, rc.left+1, rc.bottom-1);
	LineTo(hdc, rc.left+1, rc.top+1);
	DeleteObject(SelectObject(hdc, hPenOld));

	TextOut(hdc, 10, (rc.bottom - rc.top - tm.tmHeight) / 2, pszText, (int) strlen(pszText));
	DeleteObject(SelectObject(hdc, hFont));
}

/*------------------------------------------------------------
 * ToastWndProc()
 */
#define	NSEC		5		/* stay open maximum 10 seconds */
static UINT_PTR idTimer;
static HWND hwndToastWindow;
static BOOL _toastPainted = TRUE;
static WINFUNC ToastWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	RECT rc;

	switch (message) {
	case WM_PAINT: {
		char szBuf[200];
		PAINTSTRUCT	ps;
		BeginPaint(hwnd, &ps);
		GetWindowText(hwnd, szBuf, sizeof szBuf);
		toast_paint(hwnd, ps.hdc, szBuf);
		EndPaint(hwnd, &ps);
		_toastPainted = TRUE;
	}
	return 0;
	case WM_ERASEBKGND: {
		HDC hdc = (HDC)wParam;
		GetClientRect(hwnd, &rc);
		FillRect(hdc, &rc, theme_getDialogBackgroundBrush());
	}
	return 1;

	case WM_LBUTTONUP:
	case WM_CLOSE:
	case WM_TIMER:
		AnimateWindow(hwndToastWindow, 300, AW_VER_POSITIVE|AW_HIDE);
		if (idTimer) {
			KillTimer(hwnd, idTimer);
			idTimer = 0;
		}
		if (message == WM_LBUTTONUP) {
			return FALSE;
		}
		return TRUE;

	case WM_DESTROY:
		if (idTimer) {
			KillTimer(hwnd, idTimer);
			idTimer = 0;
		}
		hwndToastWindow = NULL;
		return TRUE;

	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*------------------------------------------------------------
 * labeled_windowProcedure()
 */
static WINFUNC labeled_windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT	ps;
	RECT rc;
	static int iconPadding = 5;
	static int iconSize = 18;

	switch (message) {
	case WM_CREATE: {
		LPCREATESTRUCT pCS = (LPCREATESTRUCT)lParam;
		SetWindowLongPtr(hwnd, GWW_CUSTOMVAL, (LONG_PTR)pCS->lpCreateParams);
		break;
	}
	case WM_PAINT: {
		HICON hIcon = (HICON)GetWindowLongPtr(hwnd, GWW_CUSTOMVAL);
		THEME_DATA* pTheme = theme_getCurrent();
		BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);
		HWND hwndChild = GetWindow(hwnd, GW_CHILD);
		FillRect(ps.hdc, &ps.rcPaint, theme_getDialogBackgroundBrush());
		HBRUSH hOldBrush = SelectObject(ps.hdc, GetStockObject(HOLLOW_BRUSH));
		BOOL bFocus = GetFocus() == hwndChild;
		paint_roundedRect(ps.hdc, bFocus ? pTheme->th_iconColor : pTheme->th_dialogLight, bFocus ? 2.0f : 1.0f, 1, 1, rect.right - 4, rect.bottom - 4, 12);
		DrawIconEx(ps.hdc, iconPadding, 4, hIcon, iconSize, iconSize, 0, NULL, DI_NORMAL);
		SelectObject(ps.hdc, hOldBrush);
		EndPaint(hwnd, &ps);
	}
	return 0;

	case LAB_CHILD_FOCUS_CHANGE:
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_CTLCOLOREDIT: {
		THEME_DATA* pTheme = theme_getCurrent();
		HDC hdc = (HDC)wParam;
		SetTextColor(hdc, pTheme->th_dialogForeground);
		SetBkColor(hdc, pTheme->th_dialogBackground);
		return (LRESULT)theme_getDialogBackgroundBrush();
	}

	case WM_SIZE: {
		GetClientRect(hwnd, &rc);
		rc.left += 2*iconPadding + iconSize - 2;
		rc.right -= iconSize;
		rc.top += 5;
		rc.bottom -= 5;
		HWND hwndChild = GetWindow(hwnd, GW_CHILD);
		if (hwndChild) {
			MoveWindow(hwndChild, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		}
	}
				break;
	case WM_DESTROY: {
		HICON hIcon = (HICON)GetWindowLongPtr(hwnd, GWW_CUSTOMVAL);
		DestroyIcon(hIcon);
		SetWindowLongPtr(hwnd, GWW_CUSTOMVAL, 0l);
		break;
	}

	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * cust_initializeWindowClassDefaults()
 * Can be used to initialize the class defaults of a window class to create.
 */
EXPORT void cust_initializeWindowClassDefaults(WNDCLASS *wcp)
{
	wcp->style = CS_HREDRAW | CS_VREDRAW;
	wcp->hInstance = hInst;
	wcp->cbClsExtra = 0;
	wcp->hIcon = 0;
	wcp->hCursor = LoadCursor(0, IDC_ARROW);
	wcp->hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wcp->lpszMenuName = 0; 
}

/*--------------------------------------------------------------------------
 * cust_registerControls()
 * Initialize the PKS edit custom controls registering the window classes.
 */
EXPORT int cust_registerControls(void)
{
	WNDCLASS  wc;

	cust_initializeWindowClassDefaults(&wc);	
	wc.style = CS_PARENTDC | CS_HREDRAW | CS_VREDRAW;
	wc.cbWndExtra = GWW_CUSTOMEXTRA;
	
	wc.lpfnWndProc = ToggleWndProc;
	wc.lpszClassName = TOGGLE_CLASS;
	if (!RegisterClass(&wc))
		return 0;
		
	wc.lpfnWndProc = CharSetWndProc;
	wc.lpszClassName = CHARSET_CLASS;
	if (!RegisterClass(&wc)) {
		return 0;
	}

	wc.lpfnWndProc = ToastWndProc;
	wc.lpszClassName = TOAST_CLASS;
	if (!RegisterClass(&wc)) {
		return 0;
	}

	wc.lpfnWndProc = labeled_windowProcedure;
	wc.lpszClassName = LABELED_CLASS;
	wc.cbWndExtra = sizeof(LONG_PTR);
	if (!RegisterClass(&wc)) {
		return 0;
	}
	return 1;
}

/*
 * Add a cue banner (label) to an edit control. 
 */
static void cust_addCueBanner(HWND hwndEdit, char* pszLabel) {
	size_t nSize = strlen(pszLabel);
	wchar_t* pszwBuf = calloc(1, (nSize + 1) * 2);
	MultiByteToWideChar(CP_ACP, 0, pszLabel, (int)nSize, pszwBuf, (int)nSize);
	SendMessage(hwndEdit, EM_SETCUEBANNER, 0, (LPARAM)pszwBuf);
	free(pszwBuf);
}

/*
 * Create a toast window. 
 */
HWND cust_createToastWindow(char* pszText) {
	if (hwndToastWindow == NULL) {
		hwndToastWindow = CreateWindowEx(0, TOAST_CLASS, NULL, WS_CHILD, 0, 0, 0, 0, hwndMain, NULL, hInst, NULL);
		if (!hwndToastWindow) {
			log_lastWindowsError("createToastWindow");
			return NULL;
		}
	}
	// Create a new timer or reset a previously created timer to the new waiting time.
	idTimer = SetTimer(hwndToastWindow, 1, NSEC * 1000, 0);
	if (!IsWindowVisible(hwndToastWindow)) {
		RECT rect;
		GetClientRect(hwndMain, &rect);
		rect.top = rect.bottom-toastWindowHeight;
		int width = rect.right - rect.left;
		SetWindowPos(hwndToastWindow, HWND_TOP, 0, rect.bottom-toastWindowHeight, width, toastWindowHeight, 
			SWP_DEFERERASE | SWP_NOACTIVATE);
		AnimateWindow(hwndToastWindow, 300, AW_VER_NEGATIVE);
	}
	SetWindowText(hwndToastWindow, pszText);
	if (_toastPainted) {
		InvalidateRect(hwndToastWindow, NULL, TRUE);
		_toastPainted = FALSE;
	}
	return hwndToastWindow;
}

/*--------------------------------------------------------------------------
 * cust_drawComboBoxOwnerDraw()
 */
int cust_drawComboBoxOwnerDraw(LPDRAWITEMSTRUCT lpdis, void (*DrawEntireItem)(), 
	void (*ShowSelection)(LPDRAWITEMSTRUCT lp), BOOL noDarkMode)
{
    if (lpdis->itemID == (UINT)-1) {
		/* We have a request to draw an item in the list box, yet there
		 * are no list box items. This is sent when the user TABS into
		 * an empty list box or an empty list box gets the focus. We
		 * have to indicate (somehow) that this owner-draw list box has
		 * the focus. We do it in response to this message. Note that
		 * lpdis->itemData field would be invalid in this instance so
		 * we can't allow it to fall into our standard routines.
		 */
		DrawFocusRect(lpdis->hDC,&lpdis->rcItem);
    } else {
		THEME_DATA* pTheme = noDarkMode ? theme_getDefault() : theme_getCurrent();
		HBRUSH hBrush = CreateSolidBrush((lpdis->itemState & ODS_SELECTED) ? pTheme->th_dialogHighlight : pTheme->th_dialogBackground);
		FillRect(lpdis->hDC, &lpdis->rcItem, hBrush);
		DeleteObject(hBrush);
		SetTextColor(lpdis->hDC, (lpdis->itemState & ODS_SELECTED) ? pTheme->th_dialogHighlightText : pTheme->th_dialogForeground);
		SetBkMode(lpdis->hDC, TRANSPARENT);
		(*DrawEntireItem)(
			lpdis->hDC, &lpdis->rcItem, lpdis->itemData,
			lpdis->itemID, lpdis->CtlID);
		if (lpdis->itemState & ODS_FOCUS) {
			DrawFocusRect(lpdis->hDC,&lpdis->rcItem);
		}
	}

	/* Return TRUE meaning that we processed this message. */
	return(TRUE);
}

/*
 * Measure a list box row in an owner drawn listbox painting an icon + a text.
 */
void cust_measureListBoxRowWithIcon(MEASUREITEMSTRUCT* mp) {
	mp->itemHeight = LB_ROW_WITH_ICON_HEIGHT;
}

/*
 * Draw a list box row in an owner drawn listbox painting an icon + a text. 
 */
void cust_drawListBoxRowWithIcon(HDC hdc, RECT* rcp, HICON hIcon, char* pszText) {
	int x;
	int y;
	int iconWidth = 16;
	TEXTMETRIC textmetric;

	x = rcp->left + 2;
	y = rcp->top;
	GetTextMetrics(hdc, &textmetric);
	DrawIconEx(hdc, x, y + (LB_ROW_WITH_ICON_HEIGHT-iconWidth)/2, hIcon, iconWidth, iconWidth, 0, NULL, DI_NORMAL);
	x += iconWidth + (2 * 2);
	TextOut(hdc, x, y + (LB_ROW_WITH_ICON_HEIGHT - textmetric.tmHeight) / 2, pszText, (int)strlen(pszText));

}

/*
 * Create a window which displays a child window + an associated prefix icon painting a border around the resulting
 * element..
 */
HWND cust_createLabeledWindow(HWND hwndParent, HICON hIcon, char* pszCueBanner, HWND hwndChild) {
	HWND hwnd = CreateWindow(LABELED_CLASS, NULL, WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT,
		hwndParent, NULL, hInst, hIcon);
	if (!hwnd) {
		log_lastWindowsError("createLabeledWindow");
		return 0;
	}
	cust_addCueBanner(hwndChild, pszCueBanner);
	SetParent(hwndChild, hwnd);
	return hwnd;
}

/*
 * Utility to check, whether a window refers to an edit field.
 */
BOOL cust_isEditField(HWND hwnd) {
	char szClassname[32];
	GetClassName(hwnd, szClassname, sizeof szClassname);
	if (strcmp(WC_EDIT, szClassname) == 0) {
		return TRUE;
	}
	return FALSE;
}
