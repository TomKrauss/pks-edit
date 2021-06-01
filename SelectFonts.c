/*
 * SelectFonts.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: selecting fonts
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
#include <commdlg.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"

#pragma hdrstop

#include "dial2.h"

#define italic					res1
#define	IDD_FONTSTRIKEOUT		1040
#define	IDD_FONTUNDERLINE		1041

extern HDC GetPrinterDC(void);

static LOGFONT _lf =  {
    12,					// lfHeight;
    0,					// lfWidth;
 
    0,					// lfEscapement;
    0,					// lfOrientation;
    FW_NORMAL,			// lfWeight;
    
    0,					// lfItalic;
    0,					// lfUnderline;
    0,					// lfStrikeOut;
    
    OEM_CHARSET,			// lfCharSet;
    OUT_DEFAULT_PRECIS,		// lfOutPrecision;
    CLIP_DEFAULT_PRECIS,		// lfClipPrecision;
    DEFAULT_QUALITY,		// lfQuality;
    FIXED_PITCH|FF_DONTCARE, 	// lfPitchAndFamily;
    ""					// lfFaceName[LF_FACESIZE];
};

typedef struct tagenumfont {
	HWND			en_hwnd;
	WORD			en_item;
	WORD			en_oemset;
	BOOL			en_donames;
} ENUMFONT;
typedef ENUMFONT *LPENUMFONT;

/*------------------------------------------------------------
 * EdSetTextEffects()
 */
static BOOL _emBolden = FALSE;
void EdSetTextEffects(int italic, int underline, int bold)
{
	_lf.lfItalic = italic;
	_lf.lfUnderline = underline;
	_emBolden = (bold) ? TRUE : FALSE;
}

/*------------------------------------------------------------
 * EdCreateFont()
 * create a logical font
 */
HFONT EdCreateFont(EDFONT *pFont)
{	HFONT hFont;

	_lf.lfHeight = pFont->height;
	_lf.lfWidth  = pFont->width;
	_lf.lfCharSet = (pFont->bOem) ? OEM_CHARSET : pFont->charset;
	_lf.lfWeight = (_emBolden) ? (FW_BOLD + pFont->weight) : pFont->weight;
	_lf.lfStrikeOut = (unsigned char)pFont->strikeout;
	/* _lf.lfItalic = pFont->italic; */

	lstrcpy(_lf.lfFaceName,pFont->name);

	if ((hFont = CreateFontIndirect(&_lf)) == NULL) {
		return 0;
	}

	return hFont;
}

/*------------------------------------------------------------
 * EdSelectFont()
 * select a font and return handle to old Font
 */
HFONT EdSelectFont(WINFO *wp, HDC hdc)
{
	TEXTMETRIC tm;
	HFONT      oldFont;

	wp->fnt.bOem = (wp->dispmode & SHOWOEM) ? 1 : 0;
	wp->fnt_handle = EdCreateFont(&wp->fnt);

	if (!wp->fnt_handle) {
		return 0;
	}

	if ((oldFont = SelectObject(hdc, (HFONT)wp->fnt_handle)) == NULL) {
		return 0;
	}

	DeleteObject(oldFont);

	GetTextMetrics(hdc,&tm);
	wp->cwidth  = tm.tmAveCharWidth;
	wp->cheight = tm.tmHeight/* + tm.tmExternalLeading */;

	return oldFont;
}

/*------------------------------------------------------------
 * EdUnselectFont()
 */
void EdUnselectFont(HDC hdc)
{	static HFONT stockFont;

	if (!stockFont)
		stockFont = GetStockObject(SYSTEM_FIXED_FONT);

	if (stockFont)
		DeleteObject(SelectObject(hdc,stockFont));
}

/*------------------------------------------------------------
 * EdSelectStdFont()
 */
void EdSelectStdFont(HWND hwnd, WINFO *wp)
{
	HDC hdc = GetDC(hwnd);
	HFONT oldFont = EdSelectFont(wp,hdc);

	DeleteObject(SelectObject(hdc,oldFont));
	ReleaseDC(hwnd,hdc);
}

/*--------------------------------------------------------------------------
 * ChooseFontHookProc()
 */
UINT_PTR CALLBACK ChooseFontHookProc(HWND hDlg, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
	switch(msg) {

	case WM_INITDIALOG:
		form_move(hDlg);
		EnableWindow(GetDlgItem(hDlg, IDD_FONTSTRIKEOUT), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDD_FONTUNDERLINE), FALSE);
     	return TRUE;

	}

	return FALSE;
}

/*--------------------------------------------------------------------------
 * DlgChooseFont()
 */
BOOL DlgChooseFont(HWND hwnd, EDFONT *ep, BOOL bPrinter)
{
	LOGFONT 	lf;
	CHOOSEFONT 	cf;
	HDC			hdc;
	BOOL		bRet;

	hdc = (bPrinter) ? GetPrinterDC() : CreateIC("DISPLAY",NULL,NULL,NULL);

	/* Set all structure fields to zero. */

	blfill(&cf, sizeof cf, 0);
	blfill(&lf, sizeof lf, 0);

	lf.lfHeight = ep->height;
	lf.lfWeight = ep->weight;
	lf.lfWidth = ep->width;
	lf.lfItalic = ep->italic;

#if 0
	lf.lfStrikeOut = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
#endif

	lf.lfCharSet = (BYTE)ep->charset;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
	lstrcpy(lf.lfFaceName, ep->name);

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lf;
	cf.hDC = hdc;
	cf.Flags = (bPrinter) ? 
		CF_ENABLEHOOK|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_FIXEDPITCHONLY|CF_PRINTERFONTS : 
		CF_ENABLEHOOK|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_FIXEDPITCHONLY|CF_SCREENFONTS;
	cf.rgbColors = (COLORREF)ep->fgcolor;
	cf.nFontType = (bPrinter) ? 
		PRINTER_FONTTYPE :
		SCREEN_FONTTYPE;
	cf.lpfnHook = MakeProcInstance(ChooseFontHookProc, hInst);

	if ((bRet = ChooseFont(&cf)) == TRUE) {
		ep->fgcolor = (long)cf.rgbColors;
		lstrcpy(ep->name, lf.lfFaceName);
		ep->charset = lf.lfCharSet;
		ep->height= (short)lf.lfHeight;
		ep->width = (short)lf.lfWidth;
		ep->strikeout = 0;
		ep->italic = lf.lfItalic;
		ep->weight = (short)lf.lfWeight;
	}

	if (cf.lpfnHook != NULL) {
		FreeProcInstance((FARPROC)cf.lpfnHook);
	}
	if (hdc != NULL) {
		DeleteDC(hdc);
	}
	return bRet;
}

