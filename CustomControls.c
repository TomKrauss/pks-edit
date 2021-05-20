/*
 * CustomControls.c
 *
 * PROJEKT: PKS-EDIT for ATARI - WINDOWS
 *
 * purpose: customized controls for PKS-EDIT
 *
 * 										created      : 01.01.90
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"

#include "winterf.h"
#include "winfo.h"

#pragma hdrstop

#define GWW_CUSTOMVAL		0
#define GWW_CUSTOMEXTRA		GWW_CUSTOMVAL+sizeof(WORD)

extern HFONT EdCreateFont(EDFONT *pFont);

/*--------------------------------------------------------------------------
 * DrawShadow()
 */
void DrawShadow(HDC hdc,RECT *rcp,int pressed)
{
	int			left,right,top,bottom;
	HPEN		hPen;
	HPEN		hPenTop;
	HPEN		hPenBottom;
	HPEN		hBottomColorPen;

	hBottomColorPen = CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));
	if (pressed) {
		hPenTop = hBottomColorPen;
		hPenBottom = GetStockObject(WHITE_PEN);
	} else {
		hPenBottom = hBottomColorPen;
		hPenTop = GetStockObject(WHITE_PEN);
	}
	left = rcp->left + 1;
	right = rcp->right + rcp->left - 2;
	top = rcp->top + 1;
	bottom = rcp->top+rcp->bottom - 2;
	hPen = SelectObject(hdc,hPenTop);
	MoveTo(hdc,right-1,top);
	LineTo(hdc,left,top);
	LineTo(hdc,left,bottom);
	DeleteObject(SelectObject(hdc,hPenBottom));
	MoveTo(hdc,left+1,bottom);
	LineTo(hdc,right,bottom);
	LineTo(hdc,right,top);
	DeleteObject(SelectObject(hdc,hPen));
}

/*------------------------------------------------------------
 * cust_paintbut()
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
    CLIP_DEFAULT_PRECIS,		// lfClipPrecision;
    PROOF_QUALITY,			// lfQuality;
    FF_DONTCARE, 			// lfPitchAndFamily;
    "Helv",	 			// lfFaceName[LF_FACESIZE];
};

HFONT EdSmallFont(void) {
	static HFONT hSmallFont;

	if (!hSmallFont) {
		hSmallFont = CreateFontIndirect(&_lf);
	}
	return hSmallFont;
}


#define STATE_CHECK		ODS_CHECKED
#define STATE_SEL		ODS_SELECTED
EXPORT void cust_paintbut(HDC hdc, RECT *rcp, HWND hwnd, int ww)
{
	HBRUSH 	hBrush;
	HFONT	hFont;
	DWORD	dwColwi;
	DWORD	dwColtext;
	char 	szBuff[128];

	SetMapMode(hdc,MM_TEXT);
	hFont = SelectObject(hdc, EdSmallFont());
	if (ww & STATE_CHECK) {
		dwColwi = GetSysColor(COLOR_HIGHLIGHT);
		dwColtext = GetSysColor(COLOR_HIGHLIGHTTEXT);
	} else {
		dwColwi = GetSysColor(COLOR_BTNFACE);
		dwColtext = GetSysColor(COLOR_BTNTEXT);
	}
	GetWindowText(hwnd,szBuff,sizeof szBuff);
	hBrush = SelectObject(hdc,CreateSolidBrush(dwColwi));
	SetBkColor(hdc,dwColwi);

	Rectangle(hdc, rcp->left, rcp->top, rcp->right, rcp->bottom);

	DrawShadow(hdc,rcp,ww);

	SetTextColor(hdc,dwColtext);
	rcp->left += 3, rcp->top += 2;
	rcp->right -= 3; rcp->bottom -= 2;
	if (ww) {
		rcp->left++;
		rcp->top++;
	}
	DrawText(hdc, szBuff, -1, rcp, 
			DT_NOPREFIX|DT_WORDBREAK|DT_LEFT|DT_VCENTER);

	DeleteObject(SelectObject(hdc,hBrush));
	SelectObject(hdc,hFont);
}

/*------------------------------------------------------------
 * DrawOutline()
 */
void DrawOutline(HDC hDC, int left, int top, int w, int h)
{
	int			wOldROP2;
	int			wOldBkMode;

	/* setup display context */
	wOldROP2 = SetROP2( hDC, R2_NOT);
	wOldBkMode = SetBkMode( hDC, TRANSPARENT );

	/* draw selector rectangle */
	Rectangle(hDC, left, top, left+w, top+h);

	/* restore display context */
	SetBkMode( hDC, wOldBkMode );
	SetROP2( hDC, wOldROP2 );
}

/*--------------------------------------------------------------------------
 * cust_buttoncharheight()
 */
int cust_buttoncharheight(HWND hwnd)
{
	HFONT		hFont;
	HDC			hdc;
	TEXTMETRIC	tm;
	int			fkcharheight;

	hdc = GetDC(hwnd);
	SetMapMode(hdc,MM_TEXT);
	hFont = SelectObject(hdc, EdSmallFont());
	GetTextMetrics(hdc,&tm);
	fkcharheight = tm.tmHeight + tm.tmExternalLeading;
	SelectObject(hdc,hFont);
	ReleaseDC(hwnd, hdc);
	return fkcharheight;
}

/*------------------------------------------------------------
 * ToggleWndProc()
 */
WINFUNC ToggleWndProc(HWND hwnd,UINT message,WPARAM wParam, LPARAM lParam)
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
			cust_paintbut(hdc, &rc, hwnd, ww);
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
				SendRedraw(hwnd);
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
			SendParentCommand(hwnd,
					(LONG)GetWindowWord(hwnd,GWW_CUSTOMVAL));
			ReleaseCapture();
			return 0;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

/*------------------------------------------------------------
 * MarkSelection()
 */
#define N_ROWS		8
#define N_COLS		32
static void MarkSelection(HDC hdc, int c, int cw, int ch)
{
	if (c < 256)
		DrawOutline(hdc,(c%N_COLS)*cw,(c/N_COLS)*ch,cw,ch);
}

/*------------------------------------------------------------
 * CharSetWndProc()
 */
WINFUNC CharSetWndProc(HWND hwnd,UINT message,WPARAM wParam, LPARAM lParam)
{	char 		buf[N_COLS];
	HDC			hdc;
	RECT		rect;
	HFONT		hFont;
	PAINTSTRUCT	ps;
	TEXTMETRIC 	tm;
	EDFONT		font;
	int			l,c,newc,oldc;
	WINFO		*wp;
	static int	cw,ch;

	switch(message) {
		case WM_CREATE:
			SetWindowWord(hwnd,GWW_CUSTOMVAL,(WORD)-1);
			break;

		case WM_GETDLGCODE:
			return DLGC_WANTARROWS;		
			
		case WM_KEYDOWN : /* key being pressed */
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
			if ((wp = ww_stackwi(0)) == 0) {
				break;
			}
			hdc = BeginPaint(hwnd, &ps);

			SetMapMode(hdc,MM_TEXT);
			memmove(&font, &wp->fnt, sizeof font);
			font.height = 8;
			hFont = SelectObject(hdc, EdCreateFont(&font));
			GetTextMetrics(hdc,&tm);
			ch = tm.tmHeight + tm.tmExternalLeading;
			cw = tm.tmAveCharWidth;
			for (l = 0; l < N_ROWS; l++) {
				for (c = 0; c < DIM(buf); c++)
					buf[c] = c+(l*DIM(buf));
				TextOut(hdc,0,l*ch,buf,c);
			}
			DeleteObject(SelectObject(hdc,hFont));
			newc = GetWindowWord(hwnd,GWW_CUSTOMVAL);
			MarkSelection(hdc,newc,cw,ch);
			if (hwnd == GetFocus()) {
				GetClientRect(hwnd,&rect);
				DrawFocusRect(hdc,&rect);
			}
			EndPaint(hwnd,&ps);
			return 0;

		case WM_CHARCHANGE:
			oldc = GetWindowWord(hwnd,GWW_CUSTOMVAL);
			if (oldc == (WORD) wParam)	{
				return 0;
			}
			hdc = GetDC( hwnd );
			MarkSelection(hdc,oldc,cw,ch);
			SetWindowWord(hwnd,GWW_CUSTOMVAL,(WORD)wParam);
			MarkSelection(hdc,wParam,cw,ch);
			ReleaseDC( hwnd, hdc );
			SendParentCommand(hwnd,
					  MAKELONG(wParam,CSN_CHARSELECT));
			return 0;

		case CS_QUERYCHAR:
			return GetWindowWord(hwnd,GWW_CUSTOMVAL);

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			SendRedraw(hwnd);
			return 1;

		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			newc = LOWORD(lParam) / cw + N_COLS * (HIWORD(lParam) / ch);
			if (newc < 0 || newc >= 256)
				return 0;
			SendMessage(hwnd, WM_CHARCHANGE, newc, 0L);
			return 0;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

/*--------------------------------------------------------------------------
 * class_defaults()
 */
EXPORT void class_defaults(WNDCLASS *wcp)
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
 * cust_register()
 */
EXPORT int cust_register(void)
{
	WNDCLASS  wc;

	class_defaults(&wc);	
	wc.style = CS_PARENTDC | CS_HREDRAW | CS_VREDRAW;
	wc.cbWndExtra = GWW_CUSTOMEXTRA;
	
	wc.lpfnWndProc = ToggleWndProc;
	wc.lpszClassName = "toggle";
	if (!RegisterClass(&wc))
		return 0;
		
	wc.lpfnWndProc = CharSetWndProc;
	wc.lpszClassName = "charset";
	if (!RegisterClass(&wc)) {
		return 0;
	}
     return 1;
}

/*--------------------------------------------------------------------------
 * cust_ownerselection()
 */
void cust_ownerselection(LPDRAWITEMSTRUCT lpdis)
{
	RECT		*rp;

	rp = (LPRECT)&lpdis->rcItem;
	DrawOutline(lpdis->hDC,rp->left,rp->top,rp->right-rp->left,rp->bottom-rp->top);
}

/*--------------------------------------------------------------------------
 * cust_combood()
 */
int cust_combood(LPDRAWITEMSTRUCT lpdis, void (*DrawEntireItem)(), 
	void (*ShowSelection)(LPDRAWITEMSTRUCT lp))
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
		if (!ShowSelection) {
			ShowSelection = cust_ownerselection;
		}
		if (lpdis->itemAction & ODA_DRAWENTIRE) {
	    		FrameRect(lpdis->hDC, &lpdis->rcItem, GetStockObject(WHITE_BRUSH));
			(*DrawEntireItem)(
				lpdis->hDC, &lpdis->rcItem, lpdis->itemData,
				lpdis->itemID, lpdis->CtlID);
			if (lpdis->itemState & ODS_SELECTED) {
				(*ShowSelection)(lpdis);
			}
			if (lpdis->itemState & ODS_FOCUS) {
				DrawFocusRect(lpdis->hDC,&lpdis->rcItem);
			}
		} 
		if (lpdis->itemAction & ODA_SELECT) {
			(*ShowSelection)(lpdis);
		}
		if (lpdis->itemAction & ODA_FOCUS) {
			DrawFocusRect(lpdis->hDC,&lpdis->rcItem);
		}
	}

	/* Return TRUE meaning that we processed this message. */
	return(TRUE);
}

