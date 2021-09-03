/*
 * ThemesAndStyles.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: PKS Edit color schemes and font style definitions.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include <windows.h>
#include <commdlg.h>
#include "alloc.h"
#include "trace.h"
#include "lineoperations.h"
#include "linkedlist.h"
#include "edierror.h"
#include "jsonparser.h"

#include "themes.h"
#include "winfo.h"
#include "winterf.h"

#include "dial2.h"
#include "winutil.h"

#define	IDD_FONTSTRIKEOUT		1040
#define	IDD_FONTUNDERLINE		1041

#define DEFAULT_BACKGROUND_COLOR RGB(250, 250, 250)
#define CARET_LINE_COLOR RGB(235, 255, 235)
#define MARKED_LINE_COLOR RGB(200, 200, 255)
#define CHANGED_LINE_COLOR RGB(255, 255, 80)
#define SAVED_CHANGED_LINE_COLOR RGB(120, 255, 120)
#define RULER_BORDER_COLOR RGB(220,220,220)
#define RULER_FOREROUND_COLOR RGB(140,140,140)
#define RULER_BACKGROUND_COLOR RGB(248,248,248)

extern HDC print_getPrinterDC(void);
// forward decl.
extern int theme_initThemes(void);

static const char* DEFAULT = "default";

/*
 * The style of a selected font
 */
typedef struct tagEDFONTATTRIBUTES {
	int			strikeout;
	int			bOem;
	int			italic;
	int			underline;
	int			weight;			// 0 == default font everything else between 0 and 999 is interpreted as weight
} EDFONTATTRIBUTES;

/*
 * Reusable font/color style object.
 */
typedef struct tagEDTEXTSTYLE {
	struct tagEDTEXTSTYLE* next;
	char		styleName[32];
	char		faceName[32];
	char		charset;
	int			size;
	long		fgcolor;
	long		bgcolor;
	float		zoomFactor;
	HFONT		hfont;				// cached font handle.
	EDFONTATTRIBUTES style;
} EDTEXTSTYLE;

static EDTEXTSTYLE defaultTextStyle = {
	NULL,
	"default",
	"Conolas",
	ANSI_CHARSET,
	15,
	RGB(0, 0, 0),
	-1
};

static EDTEXTSTYLE* _styles[50];

/*
 * Returns the text style for a given style class.
 */
static EDTEXTSTYLE* font_getTextStyleForIndex(FONT_STYLE_CLASS nIndex) {
	theme_initThemes();
	if (nIndex < 0 || nIndex >= DIM(_styles)) {
		return &defaultTextStyle;
	}
	EDTEXTSTYLE* pStyle = _styles[nIndex];
	return pStyle ? pStyle : &defaultTextStyle;
}


/*------------------------------------------------------------
 * font_createFontWithStyle()
 * create a logical font
 */
static HFONT font_createFontWithStyle(EDTEXTSTYLE *pFont) {
	static LOGFONT _lf = {
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
		FIXED_PITCH | FF_DONTCARE, 	// lfPitchAndFamily;
		""					// lfFaceName[LF_FACESIZE];
	};
	EDTEXTSTYLE* pDefaultFont = font_getTextStyleForIndex(FS_NORMAL);

	int size = pFont->size;
	if (size == 0) {
		size = pDefaultFont->size;
	}
	if (pFont->zoomFactor > 0.2) {
		size = (int)(size * pFont->zoomFactor);
	}
	_lf.lfHeight = size;
	_lf.lfWidth = 0;
	_lf.lfCharSet = (pFont->style.bOem) ? OEM_CHARSET : pFont->charset;
	_lf.lfWeight = pFont->style.weight;
	_lf.lfStrikeOut = (BYTE)pFont->style.strikeout;
	_lf.lfItalic = pFont->style.italic;
	_lf.lfUnderline = (BYTE)pFont->style.underline;
	if (!pFont->faceName[0]) {
		lstrcpy(_lf.lfFaceName, pDefaultFont->faceName);
	} else {
		lstrcpy(_lf.lfFaceName, pFont->faceName);
	}
	return CreateFontIndirect(&_lf);
}

static THEME_DATA defaultTheme = {
	NULL,
	"default",
	DEFAULT_BACKGROUND_COLOR,
	CHANGED_LINE_COLOR,
	SAVED_CHANGED_LINE_COLOR,
	CARET_LINE_COLOR,
	MARKED_LINE_COLOR,
	RULER_BORDER_COLOR,
	RULER_FOREROUND_COLOR,
	RULER_BACKGROUND_COLOR,
	-1,							// used as a default marker - strictly speaking there is no default for COLORREFs
	-1,
	"Helv",						// (T) use in dialogs.
	8,
	"Consolas",
	12
};

static JSON_MAPPING_RULE _edTextStyleRules[] = {
	{	RT_CHAR_ARRAY, "styleName", offsetof(EDTEXTSTYLE, styleName), sizeof(((EDTEXTSTYLE*)NULL)->styleName)},
	{	RT_CHAR_ARRAY, "faceName", offsetof(EDTEXTSTYLE, faceName), sizeof(((EDTEXTSTYLE*)NULL)->faceName)},
	{	RT_INTEGER, "size", offsetof(EDTEXTSTYLE, size)},
	{	RT_COLOR, "backgroundColor", offsetof(EDTEXTSTYLE, bgcolor)},
	{	RT_COLOR, "foregroundColor", offsetof(EDTEXTSTYLE, fgcolor)},
	{	RT_FLAG, "italic", offsetof(EDTEXTSTYLE, style.italic), 1},
	{	RT_FLAG, "underline", offsetof(EDTEXTSTYLE, style.underline), 1},
	{	RT_FLAG, "strikeout", offsetof(EDTEXTSTYLE, style.strikeout), 1},
	{	RT_INTEGER, "weight", offsetof(EDTEXTSTYLE, style.weight)},
	{	RT_END}
};

static EDTEXTSTYLE* theme_createStyle() {
	EDTEXTSTYLE* pStyle = calloc(sizeof(EDTEXTSTYLE), 1);
	memcpy(pStyle, &defaultTextStyle, sizeof defaultTextStyle);
	pStyle->faceName[0] = 0;
	pStyle->size = 0;
	return pStyle;
}

static JSON_MAPPING_RULE _edThemeRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(THEME_DATA, th_name), sizeof(((THEME_DATA*)NULL)->th_name)},
	{	RT_COLOR, "backgroundColor", offsetof(THEME_DATA, th_defaultBackgroundColor)},
	{	RT_COLOR, "caretLineColor", offsetof(THEME_DATA, th_caretLineColor)},
	{	RT_COLOR, "changedLineColor", offsetof(THEME_DATA, th_changedLineColor)},
	{	RT_COLOR, "savedChangedLineColor", offsetof(THEME_DATA, th_savedChangedLineColor)},
	{	RT_COLOR, "markedLineColor", offsetof(THEME_DATA, th_markedLineColor)},
	{	RT_COLOR, "rulerBorderColor", offsetof(THEME_DATA, th_rulerBorderColor)},
	{	RT_COLOR, "dialogBackground", offsetof(THEME_DATA, th_dialogBackground)},
	{	RT_COLOR, "dialogForeground", offsetof(THEME_DATA, th_dialogForeground)},
	{	RT_COLOR, "rulerForegroundColor", offsetof(THEME_DATA, th_rulerForegroundColor)},
	{	RT_COLOR, "rulerBackgroundColor", offsetof(THEME_DATA, th_rulerBackgroundColor)},
	{	RT_CHAR_ARRAY, "dialogFont", offsetof(THEME_DATA, th_fontName), sizeof(((THEME_DATA*)NULL)->th_fontName)},
	{	RT_INTEGER, "dialogFontSize", offsetof(THEME_DATA, th_fontSize)},
	{	RT_CHAR_ARRAY, "smallFixedFont", offsetof(THEME_DATA, th_smallFontName), sizeof(((THEME_DATA*)NULL)->th_smallFontName)},
	{	RT_INTEGER, "smallFixedFontSize", offsetof(THEME_DATA, th_smallFontSize)},
	{	RT_OBJECT_LIST, "textStyles", offsetof(THEME_DATA, th_styles),
			{.r_t_arrayDescriptor = {theme_createStyle, _edTextStyleRules}}},
	{	RT_END}
};

typedef struct tagTHEME_CONFIGURATION {
	THEME_DATA* th_themes;
	THEME_DATA* th_currentTheme;
} THEME_CONFIGURATION;

static THEME_DATA* theme_createTheme() {
	THEME_DATA* pTheme = calloc(sizeof(THEME_DATA), 1);
	if (defaultTheme.th_dialogBackground == -1) {
		defaultTheme.th_dialogBackground = GetSysColor(COLOR_3DFACE);
		defaultTheme.th_dialogForeground = GetSysColor(COLOR_BTNTEXT);
	}
	memcpy(pTheme, &defaultTheme, sizeof defaultTheme);
	return pTheme;
}

static JSON_MAPPING_RULE _themeConfigurationRules[] = {
	{	RT_OBJECT_LIST, "themes", offsetof(THEME_CONFIGURATION, th_themes),
			{.r_t_arrayDescriptor = {theme_createTheme, _edThemeRules}}},
	{	RT_END}
};

static THEME_CONFIGURATION themeConfiguration;

static char* _styleNames[] = {
	"default",
	"control",
	"hyperlink"
};

/*--------------------------------------------------------------------------
 * theme_initThemes()
 * init the theme definitions by reading our JSON config file.
 */
int theme_initThemes(void) {
	static int initialized;

	if (initialized) {
		return 1;
	}
	initialized = 1;
	memset(&themeConfiguration, 0, sizeof themeConfiguration);
	if (json_parse("themeconfig.json", &themeConfiguration, _themeConfigurationRules)) {
		theme_setCurrent("default");
		return 1;
	}
	return 0;
}

/*
 * Determine the logical style class index for a style name.
 */
FONT_STYLE_CLASS font_getStyleClassIndexFor(char* pszStyleName) {
	theme_initThemes();
	for (int i = 0; i < DIM(_styles); i++) {
		EDTEXTSTYLE* pStyle = _styles[i];
		if (!pStyle) {
			break;
		}
		if (strcmp(pStyle->styleName, pszStyleName) == 0) {
			return i;
		}
	}
	return FS_NORMAL;
}

/*
 * Create a font with a name (possible empty) and a given size.
 * If 'bOem' is true, use an OEM_CHARSET.
 */
HFONT font_createFontHandle(char* pszFontName, int size, int bOem, int nWeight) {
	EDTEXTSTYLE		font;

	memset(&font, 0, sizeof font);
	lstrcpy(font.faceName, pszFontName);
	font.style.weight = nWeight;
	font.size = size;
	font.charset = bOem ? OEM_CHARSET : ANSI_CHARSET;
	return font_createFontWithStyle(&font);
}

/*
 * The small fixed font is used by PKS edit e.g. in code completion windows or the 
 * options key controls.
 */
HFONT theme_createSmallFixedFont() {
	THEME_DATA* pTheme = theme_getDefault();
	return font_createFontHandle(pTheme->th_smallFontName, pTheme->th_smallFontSize, 0, FW_NORMAL);
}

/*
 * The dialog font is used by PKS edit e.g. in dialogs and in the window selector.
 */
HFONT theme_createDialogFont(int nWeight) {
	THEME_DATA* pTheme = theme_getDefault();
	return font_createFontHandle(pTheme->th_fontName, pTheme->th_fontSize, 0, nWeight);
}

/*------------------------------------------------------------
 * font_selectFontStyle()
 * select a font and return handle to old Font.
 */
void font_selectFontStyle(WINFO *wp, FONT_STYLE_CLASS nStyleIndex, HDC hdc) {
	EDTEXTSTYLE* pStyle = font_getTextStyleForIndex(nStyleIndex);
	EDTEXTSTYLE* pDefaultStyle = font_getTextStyleForIndex(FS_NORMAL);

	SetTextColor(hdc, pStyle->fgcolor);
	if (pStyle->bgcolor != -1) {
		SetBkColor(hdc, pStyle->bgcolor);
		SetBkMode(hdc, OPAQUE);
	} else {
		SetBkMode(hdc, TRANSPARENT);
	}
	if (pStyle->faceName[0] == 0 && !pStyle->style.underline && !pStyle->style.italic && !pStyle->style.strikeout && pStyle->style.weight == 0) {
		pStyle = pDefaultStyle;
	}
	if (GetMapMode(hdc) != MM_ANISOTROPIC) {
		int bOem = (wp->dispmode & SHOWOEM) ? 1 : 0;
		if (pStyle->style.bOem != bOem || pStyle->hfont == NULL || pStyle->zoomFactor != wp->zoomFactor) {
			if (pStyle->hfont) {
				DeleteObject(pStyle->hfont);
			}
			pStyle->zoomFactor = wp->zoomFactor;
			pStyle->style.bOem = bOem;
			pStyle->hfont = font_createFontWithStyle(pStyle);
		}

		if (!pStyle->hfont) {
			return;
		}
		// Only if not in printing or preview mode.
		// TODO: kind of hacky way to detect printing mode.
		SelectObject(hdc, pStyle->hfont);
	}

	TEXTMETRIC tm;
	GetTextMetrics(hdc,&tm);
	wp->cwidth  = tm.tmAveCharWidth;
	wp->cheight = tm.tmHeight/* + tm.tmExternalLeading */;
}

/*------------------------------------------------------------
 * font_selectStandardFont()
 */
void font_selectStandardFont(HWND hwnd, WINFO *wp) {
	HDC hdc = GetDC(hwnd);
	font_selectFontStyle(wp, 0, hdc);
	ReleaseDC(hwnd,hdc);
}

/*--------------------------------------------------------------------------
 * ChooseFontHookProc()
 */
static UINT_PTR CALLBACK ChooseFontHookProc(HWND hDlg, UINT msg, WPARAM wParam,
    LPARAM lParam) {

	switch(msg) {

	case WM_INITDIALOG:
		win_moveWindowToDefaultPosition(hDlg);
		EnableWindow(GetDlgItem(hDlg, IDD_FONTSTRIKEOUT), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDD_FONTUNDERLINE), FALSE);
     	return TRUE;

	}

	return FALSE;
}

/*--------------------------------------------------------------------------
 * DlgChooseFont()
 */
BOOL DlgChooseFont(HWND hwnd, EDTEXTSTYLE *ep, BOOL bPrinter)
{
	LOGFONT 	lf;
	CHOOSEFONT 	cf;
	HDC			hdc;
	BOOL		bRet;

	hdc = (bPrinter) ? print_getPrinterDC() : CreateIC("DISPLAY",NULL,NULL,NULL);

	/* Set all structure fields to zero. */

	memset(&cf, 0, sizeof cf);
	memset(&lf, 0, sizeof lf);

	lf.lfHeight = ep->size;
	lf.lfWeight = ep->style.weight;
	lf.lfWidth = 0;
	lf.lfItalic = ep->style.italic;
	lf.lfStrikeOut = ep->style.strikeout;
	lf.lfUnderline = ep->style.underline;
	lf.lfCharSet = (BYTE)ep->charset;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
	lstrcpy(lf.lfFaceName, ep->faceName);

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
		lstrcpy(ep->faceName, lf.lfFaceName);
		ep->charset = lf.lfCharSet;
		ep->size = lf.lfHeight;
		ep->style.underline = lf.lfUnderline;
		ep->style.strikeout = lf.lfStrikeOut;
		ep->style.italic = lf.lfItalic;
		ep->style.weight = (short)lf.lfWeight;
	}

	if (cf.lpfnHook != NULL) {
		FreeProcInstance((FARPROC)cf.lpfnHook);
	}
	if (hdc != NULL) {
		DeleteDC(hdc);
	}
	return bRet;
}

/*
 * Returns the theme with the given name. If not found, a default theme is returned.
 */
static THEME_DATA* theme_getByName(unsigned char* pThemeName) {
	theme_initThemes();
	THEME_DATA* pTheme = themeConfiguration.th_themes;
	while(pTheme != NULL) {
		if (strcmp(pTheme->th_name, pThemeName) == 0) {
			return pTheme;
		}
		pTheme = pTheme->th_next;
	}
	return &defaultTheme;
}

/*
 * Select a new theme to use by a given theme name. If the
 * named theme is not defined, a default theme is selected.
 */
void theme_setCurrent(unsigned char* pszThemeName) {
	THEME_DATA* pTheme = theme_getByName(pszThemeName);
	if (pTheme != themeConfiguration.th_currentTheme) {
		themeConfiguration.th_currentTheme = pTheme;
		memset(_styles, 0, sizeof _styles);
		EDTEXTSTYLE* pStyle = pTheme->th_styles;
		int nNextStyle = DIM(_styleNames);
		while (pStyle) {
			int bFound = 0;
			for (int i = 0; i < DIM(_styleNames); i++) {
				if (strcmp(_styleNames[i], pStyle->styleName) == 0) {
					_styles[i] = pStyle;
					bFound = 1;
					break;
				}
			}
			if (!bFound) {
				_styles[nNextStyle++] = pStyle;
			}
			pStyle = pStyle->next;
		}
		// (T) should be performed via some kind of theme changed event.
		ww_redrawAllWindows(TRUE);

	}
}

/*
 * Returns the default theme currently selected.
 */
THEME_DATA* theme_getDefault() {
	if (themeConfiguration.th_currentTheme == NULL) {
		theme_setCurrent("default");
	}
	return themeConfiguration.th_currentTheme;
}

static int theme_destroyTheme(THEME_DATA* pTheme) {
	ll_destroy(&pTheme->th_styles, NULL);
	return 1;
}

/*
 * Destroy all theme data loaded. 
 */
void theme_destroyAllThemeData() {
	ll_destroy(&themeConfiguration.th_themes, theme_destroyTheme);
}

/*
 * Return the linked list of all defined themes. 
 */
THEME_DATA* theme_getThemes() {
	return themeConfiguration.th_themes;
}

