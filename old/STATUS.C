/*
 * STATUS.C
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: statusline window
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

#include "winterf.h"
#include "editab.h"
#include "winfo.h"

#pragma hdrstop

#include "edierror.h"

# define	GWL_FTABLINK			0
# define	GWL_SEGMENTS			4
# define	MAXSEGMENTS			6
# define	SHADOWDELTA			4

typedef struct tagSTATUSSEGMENTS {
	short	nSegs;
	short	oSeg[MAXSEGMENTS];
	char *	oldStrings[MAXSEGMENTS];
	char *	oldStatusMessage;
} STATUSSEGMENTS, *PSTATUSSEGMENTS;

extern int 	mysprintf(FTABLE *fp, char *d, char *fmt, ...);
extern void 	DrawShadow(HDC hdc,RECT *rcp,int pressed);
extern char *	lstrchr(const char *, const char);
extern unsigned char *stralloc(unsigned char *buf);

static int 	_psenabled;
static HFONT	hStatusFont;

char	*		_LastStatusMessage;
HWND			hwndStatus;

/*--------------------------------------------------------------------------
 * draw_shadow_segments()
 */
static void draw_shadow_segments(HDC hdc, HWND hwnd, BOOL bLastOnly)
{
	RECT		  		rect;
	STATUSSEGMENTS *	sp;
	DWORD			dwBottomShadow;
	HPEN			hPen;
	int				nSegments;
	int				left;
	int				right;
	int				top;
	int				bottom;
	int				nFirstSeg;

	if ((sp = (STATUSSEGMENTS *)GetWindowLong(hwnd, GWL_SEGMENTS)) == 0) {
		return;
	}

	GetClientRect(hwnd,&rect);
	SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	nFirstSeg = (bLastOnly) ? sp->nSegs : 0;
	for (nSegments = nFirstSeg; nSegments <= sp->nSegs; nSegments++) {
		left = sp->oSeg[nSegments];
		right = (nSegments >= sp->nSegs) ? 
			rect.right : sp->oSeg[nSegments+1];
		Rectangle(hdc, left, 0, right, rect.bottom);
	}

	SelectObject(hdc, GetStockObject(WHITE_PEN));
	top = rect.top + 1;
	bottom = rect.bottom - 1;
	for (nSegments = nFirstSeg; nSegments <= sp->nSegs; nSegments++) {
		left = sp->oSeg[nSegments] + 1;
		right = (nSegments >= sp->nSegs) ? 
			rect.right-1 : sp->oSeg[nSegments+1]-1;
		MoveTo(hdc, right, top);
		LineTo(hdc, left, top);
		LineTo(hdc, left, bottom);
	
		right -= 2;
		MoveTo(hdc, right, top + 2);
		LineTo(hdc, right, bottom - 2);
		LineTo(hdc, left + 1, bottom - 2);
	}

	top += 2;
	bottom -= 2;
	dwBottomShadow = GetSysColor(COLOR_BTNSHADOW);
	hPen = SelectObject(hdc, CreatePen(PS_SOLID, 1, dwBottomShadow));
	for (nSegments = nFirstSeg; nSegments <= sp->nSegs; nSegments++) {
		left = sp->oSeg[nSegments] + 3;
		right = (nSegments >= sp->nSegs) ? 
			rect.right-3 : sp->oSeg[nSegments+1]-3;
		MoveTo(hdc, right, top);
		LineTo(hdc, left, top);
		LineTo(hdc, left, bottom);
		if (nSegments < sp->nSegs && sp->oldStrings[nSegments]) {
			_free(sp->oldStrings[nSegments]);
			sp->oldStrings[nSegments] = 0;
		}
	}

	DeleteObject(SelectObject(hdc, hPen));
}

/*------------------------------------------------------------
 * draw_status()
 */
static void draw_status(char *text)
{
	STATUSSEGMENTS *	sp;
 	HDC 				hdc;
	PAINTSTRUCT 		ps;
	RECT				rect;
	BOOL				bChanged;
	LPSTR			pszStrArr[MAXSEGMENTS];
     LPSTR			s;
	LPSTR			pszEnd;
	int				i;
	int				offset;
	int				nSegments;
	int				nRight;

	if (!hwndStatus) {
		return;
	}

	if ((sp = (STATUSSEGMENTS *)GetWindowLong(hwndStatus, GWL_SEGMENTS)) == 0) {
		return;
	}

	if ((hdc = BeginPaint(hwndStatus, &ps)) == 0) {
		return;
	}

	for (nSegments = 0, s = text; ; s++) {
		if (nSegments >= MAXSEGMENTS-1) {
			break;
		}
		if ((pszEnd = lstrchr(s, '!')) != 0) {
			*pszEnd = 0;
		}
		pszStrArr[nSegments++] = s;
		if ((s = pszEnd) == 0) {
			break;
		}
	}

	if (nSegments != sp->nSegs) {
		bChanged = TRUE;
		sp->nSegs = nSegments;
		for (i = 0, offset = 0; i < MAXSEGMENTS; i++) {
			if (i <= nSegments) {
				sp->oSeg[i] = offset;
				offset += 100;
			} else {
				if (sp->oldStrings[i]) {
					_free(sp->oldStrings[i]);
					sp->oldStrings[i] = 0;
				}
				sp->oSeg[i] = 0;
			}
		}
	} else {
		bChanged = FALSE;
	}

	if (bChanged /*|| ps.fErase*/) {
		draw_shadow_segments(hdc, hwndStatus, FALSE);
	}

	SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
	SelectObject(hdc, hStatusFont);
	SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));

	GetClientRect(hwndStatus, &rect);
	nRight = rect.right;

	for (nSegments = 0; nSegments < sp->nSegs; nSegments++) {
		if (nSegments < sp->nSegs - 1) {
			rect.right = sp->oSeg[nSegments+1];
		} else {
			rect.right = nRight;
		}
		rect.left = sp->oSeg[nSegments] + 10;
		s = pszStrArr[nSegments];
		if (!sp->oldStrings[nSegments] ||
			lstrcmp(s, sp->oldStrings[nSegments])) {
			DrawText(hdc, s, -1, &rect, 
				DT_SINGLELINE|DT_VCENTER);
			if (sp->oldStrings[nSegments]) {
				_free(sp->oldStrings[nSegments]);
			}
			sp->oldStrings[nSegments] = stralloc(s);
		}
	}

	if ((s = _LastStatusMessage) != 0) {
		rect.left += 100;
		if (*s == '!') {
			SetTextColor(hdc, RGB(255,0,0));
			s++;
		}
		DrawText(hdc, s, -1, &rect, DT_SINGLELINE|DT_VCENTER);
	}
	EndPaint(hwndStatus,&ps);
}

/*------------------------------------------------------------
 * fmt_status()
 */
static int fmt_status(char *dest)
{
	int 		len;
	WINFO *	wp;

	_psenabled = 0;
	if (!_currfile) {
		return 0;
	} else {
		wp = WIPOI(_currfile);
	}

	len = mysprintf(_currfile, dest,
				 (wp && wp->statusline && (wp->dispmode & SHOWHEX)) ? 
				 wp->statusline : 
				 /*STR*/"%5j$l:%5j$c!%s$&");
	_psenabled = 1;
	return len;
}

/*------------------------------------------------------------
 * st_redraw()
 */
void st_redraw(BOOL bLastSeg)
{
	RECT				rect;
	HWND				hwnd;
	STATUSSEGMENTS *	sp;

	if ((hwnd = hwndStatus) == 0) {
		return;
	}

	if ((sp = (STATUSSEGMENTS *)GetWindowLong(hwndStatus, GWL_SEGMENTS)) == 0) {
		return;
	}

	GetClientRect(hwnd,&rect);

	if (bLastSeg) {
		rect.top += SHADOWDELTA;
		rect.bottom -= SHADOWDELTA;
		rect.left = sp->oSeg[sp->nSegs] + SHADOWDELTA;
	}

	InvalidateRect(hwnd, &rect, FALSE);
}

/*--------------------------------------------------------------------------
 * st_seterrmsg()
 */
void st_seterrmsg(char *msg)
{
	HDC hdc;

	if (_LastStatusMessage) {
		_free(_LastStatusMessage);
		_LastStatusMessage = 0;
	} else if (!msg) {
		return;
	}

	if (msg) {
		_LastStatusMessage = stralloc(msg);
	}

	if (hwndStatus) {
          hdc = GetDC(hwndStatus);
		draw_shadow_segments(hdc, hwndStatus, TRUE);
		st_redraw(TRUE);
		ReleaseDC(hwndStatus, hdc);
	}
}

/*--------------------------------------------------------------------------
 * st_update()
 */
void st_update(void)
{
	if (hwndStatus) {
		UpdateWindow(hwndStatus);
	}
}

/*------------------------------------------------------------
 * StatusWndProc()
 */
WINFUNC StatusWndProc(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC					hdc;
   	STATUSSEGMENTS *	sp;
	char 				szBuf[128];

	switch(message) {

	case WM_CREATE:
		sp = (STATUSSEGMENTS *)_alloc(sizeof *sp);
		blfill(sp, sizeof *sp, 0);
		SetWindowLong(hwnd, GWL_SEGMENTS, (LONG)sp);
		hwndStatus = hwnd;
	     return 0;

	case WM_NCPAINT:
		hdc = GetDC(hwnd);
		draw_shadow_segments(hdc, hwnd, FALSE);
		ReleaseDC(hwnd, hdc);
		return 0;

	case WM_ERASEBKGND:
		draw_shadow_segments((HDC)wParam, hwnd, FALSE);
		return 0;

	case WM_PAINT:
         	fmt_status(szBuf);
		draw_status(szBuf);
		return 0;

	case WM_DESTROY:
		if ((sp = (STATUSSEGMENTS *)GetWindowLong(hwnd, GWL_SEGMENTS)) != 0) {
			_free(sp);
			SetWindowLong(hwnd, GWL_SEGMENTS, 0L);
		}
		hwndStatus = 0;
	     return 0;

	}
	return (DefWindowProc(hwnd, message, wParam, lParam));

}

/*------------------------------------------------------------
 * status_wh()
 */
void status_wh(WORD *width, WORD *height)
{
	HDC 			hdc;
	TEXTMETRIC	tm;
	LOGFONT 		logfont;

	if ((_layoutoptions & OL_SHOWSTATUS) == 0) {
		*width = 0;
		*height = 0;
		return;
	}

	hdc = GetDC(hwndFrame);
	if (!hStatusFont) {
		logfont.lfHeight = 8;
		logfont.lfWidth = 0;

		logfont.lfEscapement = 0;
		logfont.lfOrientation = 0;
		logfont.lfWeight = FW_BOLD;

		logfont.lfItalic = 0;
		logfont.lfUnderline = 0;
		logfont.lfStrikeOut = 0;

		logfont.lfCharSet = DEFAULT_CHARSET;
		logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logfont.lfQuality = DEFAULT_QUALITY;
		logfont.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
		lstrcpy(logfont.lfFaceName, "helv");
		if ((hStatusFont = CreateFontIndirect(&logfont)) == NULL) {
			hStatusFont = GetStockObject(SYSTEM_FONT);
		}
	}

	SelectObject(hdc, hStatusFont);

	GetTextMetrics(hdc,&tm);
	*width  = (unsigned short) tm.tmAveCharWidth;
	*height = tm.tmHeight + tm.tmExternalLeading + 2 * SHADOWDELTA;

	ReleaseDC(hwndFrame,hdc);
}

/*--------------------------------------------------------------------------
 * st_init()
 */
void st_init(HWND hwndDaddy)
{
	if (!hwndStatus) {
		CreateWindow( szStatusClass, NULL,
			WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
			0, 1000, 100, 10, 
			hwndDaddy, NULL, hInst, (LPVOID)0);
	}
}

