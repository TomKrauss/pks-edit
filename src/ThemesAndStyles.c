/*
 * ThemesAndStyles.c
 *
 * Project: PKS Edit for Windows
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
#include <Uxtheme.h>
#include <Vssym32.h>
#include "alloc.h"
#include "trace.h"
#include "documentmodel.h"
#include "linkedlist.h"
#include "edierror.h"
#include "jsonparser.h"
#include "editorconfiguration.h"
#include "dpisupport.h"
#include "xdialog.h"
#include "themes.h"
#include "winfo.h"
#include "winterf.h"

#include "dial2.h"
#include "winutil.h"
#include "darkmode.h"

#define DARKMODE_THEME			L"DarkMode_Explorer"

#define	IDD_FONTSTRIKEOUT		1040
#define	IDD_FONTUNDERLINE		1041

#define DEFAULT_BACKGROUND_COLOR RGB(250, 250, 250)
#define DEFAULT_ICON_COLOR RGB(0, 128, 255)
#define CARET_LINE_COLOR RGB(235, 255, 235)
#define COMPARE_MODIFIED_LINE_COLOR RGB(200, 200, 255)
#define COMPARE_ADDED_LINE_COLOR RGB(200, 255, 200)
#define COMPARE_DELETED_LINE_COLOR RGB(255, 200, 200)
#define CHANGED_LINE_COLOR RGB(200, 200, 0)
#define SAVED_CHANGED_LINE_COLOR RGB(120, 255, 120)
#define UNDO_AFTER_SAVE_LINE_COLOR RGB(255, 130, 0)
#define RULER_BORDER_COLOR RGB(220,220,220)
#define RULER_FOREROUND_COLOR RGB(140,140,140)
#define RULER_BACKGROUND_COLOR RGB(248,248,248)
#define MAIN_WINDOW_BACKGROUND_COLOR RGB(120, 120, 120)
#define DIALOG_LIGHT_BACKGROUND_COLOR RGB(228,228,228)

extern HDC print_getPrinterDC(void);
// forward decl.
extern int theme_initThemes(void);

static const char* DEFAULT = "default";

typedef struct ButtonData {
	HTHEME hTheme;
	int iStateID;
} BUTTON_PAINT_DATA;

static BOOL button_ensureTheme(BUTTON_PAINT_DATA* pData, HWND hwnd) {
	if (!pData->hTheme) {
		pData->hTheme = OpenThemeData(hwnd, L"Button");
	}
	return pData->hTheme != NULL;
}

static void button_closeTheme(BUTTON_PAINT_DATA* pData) {
	if (pData->hTheme) {
		CloseThemeData(pData->hTheme);
		pData->hTheme = NULL;
	}
}

static EDTEXTSTYLE defaultTextStyle = {
	NULL,
	"default",
	"Consolas",
	ANSI_CHARSET,
	-12,
	RGB(0, 0, 0),
	-1
};

/*
 * Returns the text style for a given style class.
 */
EDTEXTSTYLE* font_getTextStyleForIndex(THEME_DATA* pTheme, int nIndex) {
	theme_initThemes();
	EDTEXTSTYLE* pStyle;
	if (nIndex < DIM(pTheme->th_styleLookup)) {
		pStyle = pTheme->th_styleLookup[nIndex];
		return pStyle ? pStyle : &defaultTextStyle;
	}
	return &defaultTextStyle;
}


/*------------------------------------------------------------
 * font_createFontWithStyle()
 * create a logical font
 */
static HFONT font_createFontWithStyle(EDTEXTSTYLE *pFont) {
	LOGFONT _lf = {
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
	EDTEXTSTYLE* pDefaultFont = font_getTextStyleForIndex(theme_getCurrent(), FS_NORMAL);

	int size = pFont->size;
	if (size == 0) {
		size = pDefaultFont->size;
	}
	if (pFont->zoomFactor > 0.2) {
		size = (int)(size * pFont->zoomFactor);
	}
	_lf.lfHeight = size;
	_lf.lfWidth = 0;
	_lf.lfCharSet = pFont->charset;
	_lf.lfWeight = pFont->style.weight;
	_lf.lfStrikeOut = (BYTE)pFont->style.strikeout;
	_lf.lfItalic = pFont->style.italic;
	_lf.lfUnderline = (BYTE)pFont->style.underline;
	if (!pFont->faceName[0]) {
		lstrcpy(_lf.lfFaceName, pDefaultFont->faceName[0] ? pDefaultFont->faceName : defaultTextStyle.faceName);
	} else {
		lstrcpy(_lf.lfFaceName, pFont->faceName);
	}
	return CreateFontIndirect(&_lf);
}

static THEME_DATA defaultTheme = {
	NULL,
	"default",
	1,
	0,
	DEFAULT_ICON_COLOR,
	DEFAULT_BACKGROUND_COLOR,
	CHANGED_LINE_COLOR,
	SAVED_CHANGED_LINE_COLOR,
	UNDO_AFTER_SAVE_LINE_COLOR,
	CARET_LINE_COLOR,
	COMPARE_MODIFIED_LINE_COLOR,
	COMPARE_ADDED_LINE_COLOR,
	COMPARE_DELETED_LINE_COLOR,
	RULER_BORDER_COLOR,
	RULER_FOREROUND_COLOR,
	RULER_BACKGROUND_COLOR,
	-1,							// used as a default marker - strictly speaking there is no default for COLORREFs, 
								// so -1 is theoretically an illegal value for a placeholder. Works for us here.
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	MAIN_WINDOW_BACKGROUND_COLOR,
	DIALOG_LIGHT_BACKGROUND_COLOR,
	"Helv",						// (T) use in dialogs.
	-12,
	"Consolas",
	-12,
	-9
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
	{	RT_INTEGER, "darkMode", offsetof(THEME_DATA, th_isDarkMode)},
	{	RT_COLOR, "backgroundColor", offsetof(THEME_DATA, th_defaultBackgroundColor)},
	{	RT_COLOR, "caretLineColor", offsetof(THEME_DATA, th_caretLineColor)},
	{	RT_COLOR, "changedLineColor", offsetof(THEME_DATA, th_changedLineColor)},
	{	RT_COLOR, "savedChangedLineColor", offsetof(THEME_DATA, th_savedChangedLineColor)},
	{	RT_COLOR, "undoAfterSaveChangedLineColor", offsetof(THEME_DATA, th_undoAfterSaveChangedLineColor)},
	{	RT_COLOR, "compareModifiedColor", offsetof(THEME_DATA, th_compareModifiedColor)},
	{	RT_COLOR, "compareAddedColor", offsetof(THEME_DATA, th_compareAddedColor)},
	{	RT_COLOR, "compareDeletedColor", offsetof(THEME_DATA, th_compareDeletedColor)},
	{	RT_COLOR, "iconColor", offsetof(THEME_DATA, th_iconColor)},
	{	RT_COLOR, "rulerBorderColor", offsetof(THEME_DATA, th_rulerBorderColor)},
	{	RT_COLOR, "mainWindowBackgroundColor", offsetof(THEME_DATA, th_mainWindowBackground)},
	{	RT_COLOR, "dialogBackground", offsetof(THEME_DATA, th_dialogBackground)},
	{	RT_COLOR, "dialogForeground", offsetof(THEME_DATA, th_dialogForeground)},
	{	RT_COLOR, "dialogBorder", offsetof(THEME_DATA, th_dialogBorder)},
	{	RT_COLOR, "dialogDisabled", offsetof(THEME_DATA, th_dialogDisabled)},
	{	RT_COLOR, "dialogLight", offsetof(THEME_DATA, th_dialogLight)},
	{	RT_COLOR, "dialogHighlight", offsetof(THEME_DATA, th_dialogHighlight)},
	{	RT_COLOR, "dialogHighlightText", offsetof(THEME_DATA, th_dialogHighlightText)},
	{	RT_COLOR, "dialogMenuHighlight", offsetof(THEME_DATA, th_dialogMenuHighlight)},
	{	RT_COLOR, "dialogLightBackground", offsetof(THEME_DATA, th_dialogLightBackground)},
	{	RT_COLOR, "dialogErrorText", offsetof(THEME_DATA, th_dialogErrorText)},
	{	RT_COLOR, "dialogActiveTab", offsetof(THEME_DATA, th_dialogActiveTab)},
	{	RT_COLOR, "rulerForegroundColor", offsetof(THEME_DATA, th_rulerForegroundColor)},
	{	RT_COLOR, "rulerBackgroundColor", offsetof(THEME_DATA, th_rulerBackgroundColor)},
	{	RT_CHAR_ARRAY, "dialogFont", offsetof(THEME_DATA, th_fontName), sizeof(((THEME_DATA*)NULL)->th_fontName)},
	{	RT_INTEGER, "dialogFontSize", offsetof(THEME_DATA, th_fontSize)},
	{	RT_INTEGER, "mainframeMargin", offsetof(THEME_DATA, th_mainframeMargin)},
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
		defaultTheme.th_dialogBorder = GetSysColor(COLOR_BTNSHADOW);
		defaultTheme.th_dialogLight = GetSysColor(COLOR_3DLIGHT);
		defaultTheme.th_dialogActiveTab = GetSysColor(COLOR_HIGHLIGHT);
		defaultTheme.th_dialogDisabled = GetSysColor(COLOR_GRAYTEXT);
		defaultTheme.th_dialogHighlight = GetSysColor(COLOR_HIGHLIGHT);
		defaultTheme.th_dialogHighlightText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		defaultTheme.th_dialogMenuHighlight = GetSysColor(COLOR_3DHILIGHT);
		defaultTheme.th_dialogErrorText = GetSysColor(COLOR_CAPTIONTEXT);
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

static void theme_initSingle(THEME_DATA* pTheme) {
	pTheme->th_isWinTheme = strcmp(DEFAULT, pTheme->th_name) == 0;
	memset(pTheme->th_styleLookup, 0, sizeof pTheme->th_styleLookup);
	EDTEXTSTYLE* pStyle = pTheme->th_styles;
	int nNextStyle = DIM(_styleNames);
	while (pStyle) {
		int bFound = 0;
		for (int i = 0; i < DIM(_styleNames); i++) {
			if (strcmp(_styleNames[i], pStyle->styleName) == 0) {
				pTheme->th_styleLookup[i] = pStyle;
				bFound = 1;
				break;
			}
		}
		if (!bFound) {
			pTheme->th_styleLookup[nNextStyle++] = pStyle;
		}
		pStyle = pStyle->next;
	}
}

/*--------------------------------------------------------------------------
 * theme_initThemes()
 * init the theme definitions by reading our JSON config file.
 */
int theme_initThemes(void) {
	static int initialized;

	if (initialized) {
		return 1;
	}
	const char* pszFont = GetConfiguration()->defaultFontFace;
	if (*pszFont) {
		strcpy(defaultTextStyle.faceName, pszFont);
	}
	memset(&themeConfiguration, 0, sizeof themeConfiguration);
	if (json_parse("themeconfig.json", TRUE, &themeConfiguration, _themeConfigurationRules)) {
		initialized = 1;
		THEME_DATA* pTheme = themeConfiguration.th_themes;
		while (pTheme) {
			theme_initSingle(pTheme);
			pTheme = pTheme->th_next;
		}
		theme_setCurrent((char*)DEFAULT);
		return 1;
	}
	return 0;
}

/*
 * Determine the logical style class index for a style name.
 */
FONT_STYLE_CLASS font_getStyleClassIndexFor(char* pszStyleName) {
	theme_initThemes();
	THEME_DATA* pTheme = theme_getCurrent();
	for (int i = 0; i < DIM(pTheme->th_styleLookup); i++) {
		EDTEXTSTYLE* pStyle = pTheme->th_styleLookup[i];
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
	THEME_DATA* pTheme = theme_getCurrent();
	return font_createFontHandle(pTheme->th_smallFontName, dpisupport_getSize(pTheme->th_smallFontSize), 0, FW_NORMAL);
}

/*
 * The dialog font is used by PKS edit e.g. in dialogs and in the window selector.
 */
HFONT theme_createDialogFont(int nWeight) {
	THEME_DATA* pTheme = theme_getCurrent();
	return font_createFontHandle(pTheme->th_fontName, dpisupport_getSize(pTheme->th_fontSize), 0, nWeight);
}

/*
 * The small dialog font is used by PKS edit e.g. in explanations in dialogs.
 */
HFONT theme_getSmallDialogFont() {
	static HFONT smallFont;

	if (!smallFont) {
		THEME_DATA* pTheme = theme_getCurrent();
		smallFont = font_createFontHandle("Helvetica", dpisupport_getSize(pTheme->th_smallFontSize), 0, FW_BOLD);
	}
	return smallFont;
}

/*
 * Set the default text colors for the given theme.
 */
void font_setDefaultTextColors(HDC hdc, THEME_DATA* pTheme) {
	EDTEXTSTYLE* pStyle = font_getTextStyleForIndex(pTheme, FS_NORMAL);

	SetTextColor(hdc, pStyle->fgcolor);
	if (pStyle->bgcolor != -1) {
		SetBkColor(hdc, pStyle->bgcolor);
		SetBkMode(hdc, OPAQUE);
	}
	else {
		SetBkMode(hdc, TRANSPARENT);
	}
}

/*------------------------------------------------------------
 * font_selectFontStyle()
 * select a font and return handle to old Font.
 */
void font_selectFontStyle(THEME_DATA* pTheme, WINFO *wp, FONT_STYLE_CLASS nStyleIndex, HDC hdc) {
	EDTEXTSTYLE* pStyle = font_getTextStyleForIndex(pTheme, nStyleIndex);
	EDTEXTSTYLE* pDefaultStyle = font_getTextStyleForIndex(pTheme, FS_NORMAL);

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
		if ((wp->charset && pStyle->charset != wp->charset) || pStyle->hfont == NULL || pStyle->zoomFactor != wp->zoomFactor) {
			if (pStyle->hfont) {
				DeleteObject(pStyle->hfont);
			}
			pStyle->zoomFactor = wp->zoomFactor;
			pStyle->charset = wp->charset;
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
	font_selectFontStyle(theme_getCurrent(), wp, 0, hdc);
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

	}

	return dlg_defaultWndProc(hDlg, msg, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * DlgChooseFont()
 */
BOOL DlgChooseFont(HWND hwnd, char* pszFontName, BOOL bPrinter) {
	LOGFONT 	lf;
	CHOOSEFONT 	cf;
	HDC			hdc;
	BOOL		bRet;

	hdc = (bPrinter) ? print_getPrinterDC() : CreateIC("DISPLAY",NULL,NULL,NULL);

	/* Set all structure fields to zero. */

	memset(&cf, 0, sizeof cf);
	memset(&lf, 0, sizeof lf);

	lf.lfHeight = dpisupport_getSize(12);
	lf.lfWeight = 0;
	lf.lfWidth = 0;
	lf.lfItalic = 0;
	lf.lfStrikeOut = 0;
	lf.lfUnderline = 0;
	lf.lfCharSet = (BYTE)CP_ACP;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
	lstrcpy(lf.lfFaceName, pszFontName);

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lf;
	cf.hDC = hdc;
	cf.Flags = (bPrinter) ? 
		CF_ENABLEHOOK|CF_INITTOLOGFONTSTRUCT|CF_FIXEDPITCHONLY|CF_PRINTERFONTS : 
		CF_ENABLEHOOK|CF_INITTOLOGFONTSTRUCT|CF_FIXEDPITCHONLY|CF_SCREENFONTS;
	cf.rgbColors = 0;
	cf.nFontType = (bPrinter) ? 
		PRINTER_FONTTYPE :
		SCREEN_FONTTYPE;
	cf.lpfnHook = MakeProcInstance(ChooseFontHookProc, hInst);

	if ((bRet = ChooseFont(&cf)) == TRUE) {
		lstrcpy(pszFontName, lf.lfFaceName);
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
	if (strcmp(SYSTEM_DEFAULT_THEME, pThemeName) == 0) {
		pThemeName = darkmode_isSelectedByDefault() ? "dark" : DEFAULT;
	}
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
		PostMessage(hwndMain, WM_THEMECHANGED, 0, 0);
	}
}

/* 
 * Returns the default theme. 
 */
THEME_DATA* theme_getDefault() {
	THEME_DATA* pTheme = themeConfiguration.th_themes;
	while (pTheme) {
		if (strcmp(DEFAULT, pTheme->th_name) == 0) {
			return pTheme;
		}
		pTheme = pTheme->th_next;
	}
	return &defaultTheme;
}

/*
 * Returns the default theme currently selected.
 */
THEME_DATA* theme_getCurrent() {
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
	themeConfiguration.th_currentTheme = &defaultTheme;
}

/*
 * Return the linked list of all defined themes. 
 */
THEME_DATA* theme_getThemes() {
	return themeConfiguration.th_themes;
}

/*
 * Prepare the passed window to be displayed in Windows dark mode look&feel or not,
 * depending on the current theme.
 */
void theme_enableDarkMode(HWND hwnd) {
	static int bRunning;
	if (!bRunning) {
		THEME_DATA* pTheme = theme_getCurrent();
		BOOL bDark = pTheme->th_isDarkMode;
		bRunning = 1;
		darkmode_allowForWindow(hwnd, bDark);
		SetWindowTheme(hwnd, bDark ? DARKMODE_THEME : NULL, NULL);
		darkmode_refreshTitleBarThemeColor(hwnd, bDark);
		bRunning = 0;
	}
}

/*
 * Returns a brush to paint the background of dialogs for the currently selected theme.
 */
HBRUSH theme_getDialogBackgroundBrush() {
	static HBRUSH hBrushBg;
	static THEME_DATA* pOldTheme;
	THEME_DATA* pTheme = theme_getCurrent();
	if (hBrushBg == NULL || pTheme != pOldTheme) {
		if (hBrushBg) {
			DeleteObject(hBrushBg);
		}
		hBrushBg = CreateSolidBrush(pTheme->th_dialogBackground);
		pOldTheme = pTheme;
	}
	return hBrushBg;
}

/*
 * Returns a brush to paint the background of dialogs for the currently selected theme.
 */
HBRUSH theme_getDialogLightBackgroundBrush() {
	static HBRUSH hBrushBg;
	static THEME_DATA* pOldTheme;
	THEME_DATA* pTheme = theme_getCurrent();
	if (hBrushBg == NULL || pTheme != pOldTheme) {
		if (hBrushBg) {
			DeleteObject(hBrushBg);
		}
		hBrushBg = CreateSolidBrush(pTheme->th_dialogLightBackground);
		pOldTheme = pTheme;
	}
	return hBrushBg;
}

/*
 * Custom painting of dark mode group boxes.
 */
static void paintGroupbox(HWND hwnd, HDC hdc, BUTTON_PAINT_DATA* buttonData) {
	DWORD nStyle = GetWindowLong(hwnd, GWL_STYLE);
	int iPartID = BP_GROUPBOX;
	int iStateID = GBS_NORMAL;

	if (nStyle & WS_DISABLED)
	{
		iStateID = GBS_DISABLED;
	}

	RECT rcClient = { 0 };
	GetClientRect(hwnd, &rcClient);

	RECT rcText = rcClient;
	RECT rcBackground = rcClient;

	HFONT hFont = NULL;
	HFONT hOldFont = NULL;
	HFONT hCreatedFont = NULL;
	LOGFONTW lf = { 0 };
	if (SUCCEEDED(GetThemeFont(buttonData->hTheme, hdc, iPartID, iStateID, TMT_FONT, &lf))) {
		hCreatedFont = CreateFontIndirectW(&lf);
		hFont = hCreatedFont;
	}

	if (!hFont) {
		hFont = (HFONT)(SendMessage(hwnd, WM_GETFONT, 0, 0));
	}

	hOldFont = (HFONT)(SelectObject(hdc, hFont));

	WCHAR szText[256] = { 0 };
	GetWindowTextW(hwnd, szText, _countof(szText));

	long style = (long)(GetWindowLongPtr(hwnd, GWL_STYLE));
	BOOL isCenter = (style & BS_CENTER) == BS_CENTER;

	if (szText[0]) {
		SIZE textSize = { 0 };
		GetTextExtentPoint32W(hdc, szText, (int)(wcslen(szText)), &textSize);

		int centerPosX = isCenter ? ((rcClient.right - rcClient.left - textSize.cx) / 2) : 7;

		rcBackground.top += textSize.cy / 2;
		rcText.left += centerPosX;
		rcText.bottom = rcText.top + textSize.cy;
		rcText.right = rcText.left + textSize.cx + 4;

		ExcludeClipRect(hdc, rcText.left, rcText.top, rcText.right, rcText.bottom);
	}
	else
	{
		SIZE textSize = { 0 };
		GetTextExtentPoint32W(hdc, L"M", 1, &textSize);
		rcBackground.top += textSize.cy / 2;
	}

	RECT rcContent = rcBackground;
	GetThemeBackgroundContentRect(buttonData->hTheme, hdc, BP_GROUPBOX, iStateID, &rcBackground, &rcContent);
	ExcludeClipRect(hdc, rcContent.left, rcContent.top, rcContent.right, rcContent.bottom);

	//DrawThemeParentBackground(hwnd, hdc, &rcClient);
	DrawThemeBackground(buttonData->hTheme, hdc, BP_GROUPBOX, iStateID, &rcBackground, NULL);

	SelectClipRgn(hdc, NULL);

	if (szText[0]) {
		rcText.right -= 2;
		rcText.left += 2;

		DTTOPTS dtto = { sizeof(DTTOPTS), DTT_TEXTCOLOR };
		THEME_DATA* pTheme = theme_getCurrent();
		dtto.crText = pTheme->th_dialogForeground;

		DWORD textFlags = isCenter ? DT_CENTER : DT_LEFT;

		DrawThemeTextEx(buttonData->hTheme, hdc, BP_GROUPBOX, iStateID, szText, -1, textFlags | DT_SINGLELINE, &rcText, &dtto);
	}

	if (hCreatedFont) DeleteObject(hCreatedFont);
	SelectObject(hdc, hOldFont);
}

/*
 * Custom window procedure for custom paint group boxes.
 */
static const uintptr_t groupboxSublassId = 2214;
static LRESULT CALLBACK groupboxSubclassProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData) {
	BUTTON_PAINT_DATA* pButtonData = (BUTTON_PAINT_DATA*)(dwRefData);

	switch (uMsg)
	{
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, groupboxSubclassProc, groupboxSublassId);
		button_closeTheme(pButtonData);
		free(pButtonData);
		break;
	case WM_ERASEBKGND:
		if (button_ensureTheme(pButtonData, hWnd)) {
			return TRUE;
		}
		else {
			break;
		}
	case WM_THEMECHANGED:
		button_closeTheme(pButtonData);
		break;
	case WM_PRINTCLIENT:
	case WM_PAINT:
		if (button_ensureTheme(pButtonData, hWnd)) {
			PAINTSTRUCT ps = { 0 };
			HDC hdc = (HDC)(wParam);
			if (!hdc) {
				hdc = BeginPaint(hWnd, &ps);
			}
			paintGroupbox(hWnd, hdc, pButtonData);
			if (ps.hdc) {
				EndPaint(hWnd, &ps);
			}

			return 0;
		}
		break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

/*
 * Custom rendering of checkboxes etc... for dark mode. 
 */
static void theme_renderButton(HWND hwnd, HDC hdc, HTHEME hTheme, int iPartID, int iStateID) {
	RECT rcClient = { 0 };
	WCHAR szText[256] = { 0 };
	DWORD nState = (DWORD)(SendMessage(hwnd, BM_GETSTATE, 0, 0));
	DWORD uiState = (DWORD)(SendMessage(hwnd, WM_QUERYUISTATE, 0, 0));
	DWORD nStyle = GetWindowLong(hwnd, GWL_STYLE);

	HFONT hFont = NULL;
	HFONT hOldFont = NULL;
	HFONT hCreatedFont = NULL;
	LOGFONTW lf = { 0 };
	if (SUCCEEDED(GetThemeFont(hTheme, hdc, iPartID, iStateID, TMT_FONT, &lf))) {
		hCreatedFont = CreateFontIndirectW(&lf);
		hFont = hCreatedFont;
	}

	if (!hFont) {
		hFont = (HFONT)(SendMessage(hwnd, WM_GETFONT, 0, 0));
	}

	hOldFont = (HFONT)(SelectObject(hdc, hFont));

	DWORD dtFlags = DT_LEFT; // DT_LEFT is 0
	dtFlags |= (nStyle & BS_MULTILINE) ? DT_WORDBREAK : DT_SINGLELINE;
	dtFlags |= ((nStyle & BS_CENTER) == BS_CENTER) ? DT_CENTER : (nStyle & BS_RIGHT) ? DT_RIGHT : 0;
	dtFlags |= ((nStyle & BS_VCENTER) == BS_VCENTER) ? DT_VCENTER : (nStyle & BS_BOTTOM) ? DT_BOTTOM : 0;
	dtFlags |= (uiState & UISF_HIDEACCEL) ? DT_HIDEPREFIX : 0;

	if (!(nStyle & BS_MULTILINE) && !(nStyle & BS_BOTTOM) && !(nStyle & BS_TOP))
	{
		dtFlags |= DT_VCENTER;
	}

	GetClientRect(hwnd, &rcClient);
	GetWindowTextW(hwnd, szText, _countof(szText));

	SIZE szBox = { 13, 13 };
	GetThemePartSize(hTheme, hdc, iPartID, iStateID, NULL, TS_DRAW, &szBox);

	RECT rcText = rcClient;
	GetThemeBackgroundContentRect(hTheme, hdc, iPartID, iStateID, &rcClient, &rcText);

	RECT rcBackground = rcClient;
	if (dtFlags & DT_SINGLELINE)
	{
		rcBackground.top += (rcText.bottom - rcText.top - szBox.cy) / 2;
	}
	rcBackground.bottom = rcBackground.top + szBox.cy;
	rcBackground.right = rcBackground.left + szBox.cx;
	rcText.left = rcBackground.right + 3;

	DrawThemeParentBackground(hwnd, hdc, &rcClient);
	DrawThemeBackground(hTheme, hdc, iPartID, iStateID, &rcBackground, NULL);
	THEME_DATA* pTheme = theme_getCurrent();
	DTTOPTS dtto = { sizeof(DTTOPTS), DTT_TEXTCOLOR };
	dtto.crText = pTheme->th_dialogForeground;

	if (nStyle & WS_DISABLED)
	{
		dtto.crText = pTheme->th_dialogDisabled;
	}

	DrawThemeTextEx(hTheme, hdc, iPartID, iStateID, szText, -1, dtFlags, &rcText, &dtto);

	if ((nState & BST_FOCUS) && !(uiState & UISF_HIDEFOCUS))
	{
		RECT rcTextOut = rcText;
		dtto.dwFlags |= DTT_CALCRECT;
		DrawThemeTextEx(hTheme, hdc, iPartID, iStateID, szText, -1, dtFlags | DT_CALCRECT, &rcTextOut, &dtto);
		RECT rcFocus = rcTextOut;
		rcFocus.bottom++;
		rcFocus.left--;
		rcFocus.right++;
		DrawFocusRect(hdc, &rcFocus);
	}

	if (hCreatedFont) DeleteObject(hCreatedFont);
	SelectObject(hdc, hOldFont);
}

/*
 * Custom painting of themed buttons.
 */
static void paintButton(HWND hwnd, HDC hdc, BUTTON_PAINT_DATA* buttonData) {
	DWORD nState = (DWORD)(SendMessage(hwnd, BM_GETSTATE, 0, 0));
	DWORD nStyle = GetWindowLong(hwnd, GWL_STYLE);
	DWORD nButtonStyle = nStyle & 0xF;

	int iPartID = BP_CHECKBOX;
	if (nButtonStyle == BS_CHECKBOX || nButtonStyle == BS_AUTOCHECKBOX) {
		iPartID = BP_CHECKBOX;
	} else if (nButtonStyle == BS_RADIOBUTTON || nButtonStyle == BS_AUTORADIOBUTTON) {
		iPartID = BP_RADIOBUTTON;
	}

	// states of BP_CHECKBOX and BP_RADIOBUTTON are the same
	int iStateID = RBS_UNCHECKEDNORMAL;

	if (nStyle & WS_DISABLED)		iStateID = RBS_UNCHECKEDDISABLED;
	else if (nState & BST_PUSHED)	iStateID = RBS_UNCHECKEDPRESSED;
	else if (nState & BST_HOT)		iStateID = RBS_UNCHECKEDHOT;

	if (nState & BST_CHECKED)		iStateID += 4;

	if (BufferedPaintRenderAnimation(hwnd, hdc))
	{
		return;
	}

	BP_ANIMATIONPARAMS animParams = { sizeof(animParams) };
	animParams.style = BPAS_LINEAR;
	if (iStateID != buttonData->iStateID) {
		GetThemeTransitionDuration(buttonData->hTheme, iPartID, buttonData->iStateID, iStateID, TMT_TRANSITIONDURATIONS, &animParams.dwDuration);
	}

	RECT rcClient = { 0 };
	GetClientRect(hwnd, &rcClient);

	HDC hdcFrom = NULL;
	HDC hdcTo = NULL;
	HANIMATIONBUFFER hbpAnimation = BeginBufferedAnimation(hwnd, hdc, &rcClient, BPBF_COMPATIBLEBITMAP, NULL, &animParams, &hdcFrom, &hdcTo);
	if (hbpAnimation) {
		if (hdcFrom) {
			theme_renderButton(hwnd, hdcFrom, buttonData->hTheme, iPartID, buttonData->iStateID);
		}
		if (hdcTo) {
			theme_renderButton(hwnd, hdcTo, buttonData->hTheme, iPartID, iStateID);
		}

		buttonData->iStateID = iStateID;

		EndBufferedAnimation(hbpAnimation, TRUE);
	} else {
		theme_renderButton(hwnd, hdc, buttonData->hTheme, iPartID, iStateID);

		buttonData->iStateID = iStateID;
	}
}

/*
 * Custom window procedure for custom paint buttons (checkbox etc...). 
 */
static const uintptr_t buttonSublassId = 2215;
static LRESULT CALLBACK buttonSubclassProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData) {

	BUTTON_PAINT_DATA* pButtonData = (BUTTON_PAINT_DATA*)dwRefData;

	switch (uMsg) {
	case WM_UPDATEUISTATE:
		if (HIWORD(wParam) & (UISF_HIDEACCEL | UISF_HIDEFOCUS)) {
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, buttonSubclassProc, buttonSublassId);
		button_closeTheme(pButtonData);
		free(pButtonData);
		break;
	case WM_ERASEBKGND:
		button_ensureTheme(pButtonData, hWnd);
		return TRUE;
	case WM_THEMECHANGED:
		button_closeTheme(pButtonData);
		break;
	case WM_PRINTCLIENT:
	case WM_PAINT: {
		button_ensureTheme(pButtonData, hWnd);
		PAINTSTRUCT ps = { 0 };
			HDC hdc = (HDC)(wParam);
			if (!hdc) {
				hdc = BeginPaint(hWnd, &ps);
			}
			paintButton(hWnd, hdc, pButtonData);
			if (ps.hdc) {
				EndPaint(hWnd, &ps);
			}

			return 0;
		}
	case WM_SIZE:
	case WM_DESTROY:
		BufferedPaintStopAllAnimations(hWnd);
		break;
	case WM_ENABLE: {
			// skip the button's normal wndproc so it won't redraw out of wm_paint
			LRESULT lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
			InvalidateRect(hWnd, NULL, FALSE);
			return lr;
		}
		break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static UINT_PTR tabSubclassID = 2345;

static LRESULT CALLBACK tabSubclassProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData
) {
	UNREFERENCED_PARAMETER(uIdSubclass);
	UNREFERENCED_PARAMETER(dwRefData);
	THEME_DATA* pTheme = theme_getCurrent();

	switch (uMsg) {
	case WM_PAINT: {

		LONG_PTR dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
		if ((dwStyle & TCS_BOTTOM) || (dwStyle & TCS_BUTTONS) || (dwStyle & TCS_VERTICAL)) {
			break;
		}

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, theme_getDialogBackgroundBrush());

		HPEN edgePen = CreatePen(0, 1, pTheme->th_dialogBorder);
		HPEN holdPen = (HPEN)SelectObject(hdc, edgePen);

		HRGN hOldClip = CreateRectRgn(0, 0, 0, 0);
		if (1 != GetClipRgn(hdc, hOldClip))
		{
			DeleteObject(hOldClip);
			hOldClip = NULL;
		}

		HFONT hFont = (HFONT)(SendMessage(hWnd, WM_GETFONT, 0, 0));
		HFONT hOldFont = SelectObject(hdc, hFont);

		POINT ptCursor = { 0 };
		GetCursorPos(&ptCursor);
		ScreenToClient(hWnd, &ptCursor);

		int nTabs = TabCtrl_GetItemCount(hWnd);

		int nSelTab = TabCtrl_GetCurSel(hWnd);
		for (int i = 0; i < nTabs; ++i) {
			RECT rcItem = { 0 };
			TabCtrl_GetItemRect(hWnd, i, &rcItem);

			RECT rcIntersect = { 0 };
			if (IntersectRect(&rcIntersect, &ps.rcPaint, &rcItem)) {
				BOOL bHot = PtInRect(&rcItem, ptCursor);

				POINT edges[] = {
					{rcItem.right - 1, rcItem.top},
					{rcItem.right - 1, rcItem.bottom}
				};
				Polyline(hdc, edges, _countof(edges));
				rcItem.right -= 1;

				HRGN hClip = CreateRectRgnIndirect(&rcItem);

				SelectClipRgn(hdc, hClip);

				SetTextColor(hdc, bHot || i == nSelTab ? pTheme->th_dialogForeground : pTheme->th_dialogBorder);

				FillRect(hdc, &rcItem, (i == nSelTab) ? theme_getDialogBackgroundBrush() : theme_getDialogLightBackgroundBrush());

				SetBkMode(hdc, TRANSPARENT);

				TCHAR label[MAX_PATH];
				label[0] = 0;
				TCITEM tci = { 0 };
				tci.mask = TCIF_TEXT;
				tci.pszText = label;
				tci.cchTextMax = MAX_PATH - 1;

				SendMessage(hWnd, TCM_GETITEM, i, (LPARAM)(&tci));

				RECT rcText = rcItem;
				rcText.left += 6;
				rcText.right -= 3;

				if (i == nSelTab) {
					rcText.bottom -= 4;
				}

				DrawText(hdc, label, -1, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

				DeleteObject(hClip);

				SelectClipRgn(hdc, hOldClip);
			}
		}
		SelectObject(hdc, hOldFont);

		if (hOldClip) {
			SelectClipRgn(hdc, hOldClip);
			DeleteObject(hOldClip);
		}

		DeleteObject(SelectObject(hdc, holdPen));
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, tabSubclassProc, tabSubclassID);
		break;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static UINT_PTR cbSubclassID = 2346;

static LRESULT CALLBACK comboBoxSubclassProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData) {

	THEME_DATA* pTheme = theme_getCurrent();
	switch (uMsg)
	{
	case WM_PAINT:
	{
		if (!pTheme->th_isDarkMode) {
			break;
		}

		RECT rc = { 0 };
		GetClientRect(hWnd, &rc);

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		HPEN edgePen = CreatePen(0, 1, pTheme->th_dialogBorder);
		HPEN holdPen = (HPEN)SelectObject(hdc, edgePen);
		SelectObject(hdc, (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0));
		SetBkColor(hdc, pTheme->th_dialogBackground);

		SelectObject(hdc, GetStockObject(NULL_BRUSH));
		Rectangle(hdc, 0, 0, rc.right, rc.bottom);

		HBRUSH holdBrush = SelectObject(hdc, theme_getDialogBackgroundBrush());
		int delta = dpisupport_getSize(1);
		RECT arrowRc = {
		rc.right - dpisupport_getSize(17), rc.top + delta,
		rc.right - delta, rc.bottom - delta
		};

		// CBS_DROPDOWN text is handled by parent by WM_CTLCOLOREDIT
		LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
		if ((style & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
		{
			RECT bkRc = rc;
			bkRc.left += delta;
			bkRc.top += delta;
			bkRc.right = arrowRc.left - delta;
			bkRc.bottom -= 1;
			FillRect(hdc, &bkRc, theme_getDialogBackgroundBrush());
			LRESULT index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
			if (index != CB_ERR) {
				SetTextColor(hdc, pTheme->th_dialogForeground);
				SetBkColor(hdc, pTheme->th_defaultBackgroundColor);
				size_t bufferLen = (size_t)SendMessage(hWnd, CB_GETLBTEXTLEN, index, 0);
				char* buffer = malloc(bufferLen + 1);
				SendMessage(hWnd, CB_GETLBTEXT, index, (LPARAM)buffer);

				RECT textRc = rc;
				textRc.left += dpisupport_getSize(4);
				textRc.right = arrowRc.left - dpisupport_getSize(5);

				DrawText(hdc, buffer, -1, &textRc, DT_NOPREFIX | DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				free(buffer);
			}
		}

		POINT ptCursor = { 0 };
		GetCursorPos(&ptCursor);
		ScreenToClient(hWnd, &ptCursor);

		BOOL isHot = PtInRect(&rc, ptCursor);

		SetTextColor(hdc, isHot ? pTheme->th_dialogHighlightText : pTheme->th_dialogForeground);
		SetBkColor(hdc, isHot ? pTheme->th_dialogHighlight : pTheme->th_dialogBackground);
		ExtTextOutW(hdc,
			arrowRc.left + (arrowRc.right - arrowRc.left) / 2 - dpisupport_getSize(4),
			arrowRc.top + 3,
			ETO_OPAQUE | ETO_CLIPPED,
			&arrowRc, L"\u02C5",
			1,
			NULL);
		SetBkColor(hdc, pTheme->th_dialogBackground);

		POINT edge[] = {
			{arrowRc.left - 1, arrowRc.top},
			{arrowRc.left - 1, arrowRc.bottom}
		};
		Polyline(hdc, edge, _countof(edge));

		DeleteObject(SelectObject(hdc, holdPen));
		SelectObject(hdc, holdBrush);

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_NCDESTROY: {
		RemoveWindowSubclass(hWnd, comboBoxSubclassProc, uIdSubclass);
		break;
	}
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

/*
 * Custom window procedure for listview headers.
 */
 static const uintptr_t headerSublassId = 2216;
 static LRESULT CALLBACK headerSubclassProc(
	 HWND hWnd,
	 UINT uMsg,
	 WPARAM wParam,
	 LPARAM lParam,
	 UINT_PTR uIdSubclass,
	 DWORD_PTR dwRefData) {

	 switch (uMsg) {
	 case WM_NOTIFY:
		 if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) {
			 LPNMCUSTOMDRAW nmcd = (LPNMCUSTOMDRAW)lParam;
			 switch (nmcd->dwDrawStage) {
			 case CDDS_PREPAINT:
				 return CDRF_NOTIFYITEMDRAW;
			 case CDDS_ITEMPREPAINT: {
				 SetTextColor(nmcd->hdc, theme_getCurrent()->th_dialogForeground);
				 return CDRF_DODEFAULT;
			 }
			 }
		 }
		 break;
	 case WM_NCDESTROY: {
		 RemoveWindowSubclass(hWnd, headerSubclassProc, uIdSubclass);
		 break;
	 }
	 }
	 return DefSubclassProc(hWnd, uMsg, wParam, lParam);
 }

static void subclassComboBoxControl(HWND hwnd) {
	SetWindowSubclass(hwnd, comboBoxSubclassProc, cbSubclassID, 0);
}

/*
 * Invoked to prepare all child windows for being used under darkmode.
 */
static BOOL theme_prepareControlsForDarkMode(HWND hwndControl, LONG lParam) {
	char szClassname[16];
	GetClassName(hwndControl, szClassname, sizeof szClassname);
	if (strcmp(WC_BUTTON, szClassname) == 0) {
		int nButtonStyle = GetWindowLongPtr(hwndControl, GWL_STYLE) & 0xF;
		switch (nButtonStyle) {
		case BS_CHECKBOX:
		case BS_AUTOCHECKBOX:
		case BS_RADIOBUTTON:
		case BS_AUTORADIOBUTTON: {
				BUTTON_PAINT_DATA* pData = calloc(1, sizeof * pData);
				SetWindowSubclass(hwndControl, buttonSubclassProc, buttonSublassId, (DWORD_PTR)pData);
			}
		   break;
		case BS_GROUPBOX: {
				BUTTON_PAINT_DATA* pData = calloc(1, sizeof * pData);
				SetWindowSubclass(hwndControl, groupboxSubclassProc, groupboxSublassId, (DWORD_PTR)pData);
			}
			break;
		case BS_DEFPUSHBUTTON:
		case BS_PUSHBUTTON:
			if (theme_getCurrent()->th_isDarkMode) {
				SetWindowTheme(hwndControl, DARKMODE_THEME, NULL);
			}
			break;
		}
		return TRUE;
	} else if (strcmp(WC_TABCONTROL, szClassname) == 0) {
		SetWindowSubclass(hwndControl, tabSubclassProc, tabSubclassID, 0);
	} else if (strcmp(WC_COMBOBOX, szClassname) == 0) {
		int style = (int)GetWindowLongPtr(hwndControl, GWL_STYLE);
		if ((style & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST || (style & CBS_DROPDOWN) == CBS_DROPDOWN) {
			COMBOBOXINFO cbi = { .cbSize = sizeof(COMBOBOXINFO) };
			BOOL result = GetComboBoxInfo(hwndControl, &cbi);
			if (result == TRUE && cbi.hwndList) {
				//dark scrollbar for listbox of combobox
				SetWindowTheme(cbi.hwndList, DARKMODE_THEME, NULL);
			}
			subclassComboBoxControl(hwndControl);
		}
		return TRUE;
	} else if (strcmp(WC_LISTBOX, szClassname) == 0) {
		if (theme_getCurrent()->th_isDarkMode) {
			SetWindowTheme(hwndControl, DARKMODE_THEME, NULL);
		}
	} else if (strcmp(WC_LISTVIEW, szClassname) == 0) {
		THEME_DATA* pTheme = theme_getCurrent();
		if (pTheme->th_isDarkMode) {
			HWND hwndHeader = ListView_GetHeader(hwndControl);
			darkmode_allowForWindow(hwndHeader, TRUE);
			SetWindowTheme(hwndHeader, L"ItemsView", 0);
			darkmode_allowForWindow(hwndControl, TRUE);
			SetWindowTheme(hwndControl, L"Explorer", 0);
		}
		ListView_SetBkColor(hwndControl, pTheme->th_dialogBackground);
		ListView_SetTextColor(hwndControl, pTheme->th_dialogForeground);
		ListView_SetTextBkColor(hwndControl, pTheme->th_dialogBackground);
		SetWindowSubclass(hwndControl, headerSubclassProc, headerSublassId, 0);
	}
	return TRUE;
}

/*
 * Prepare all children of a given parent to be used in dark mode correctly. 
 */
void theme_prepareChildrenForDarkmode(HWND hParent) {
	EnumChildWindows(hParent, (WNDENUMPROC)theme_prepareControlsForDarkMode, 0);
}

/*
 * Returns the foreground color of a text style with a given name in the current theme.
 * If the style cannot be found a default value is returned.
 */
long theme_textStyleForeground(THEME_DATA* pTheme, const char* pszStylename, long lDefault) {
	EDTEXTSTYLE* pStyle = ll_find((void*)pTheme->th_styles, pszStylename);
	return pStyle ? pStyle->fgcolor : lDefault;
}

/*
 * Returns the background color of a text style with a given name in the current theme.
 * If the style cannot be found a default value is returned.
 */
long theme_textStyleBackground(THEME_DATA* pTheme, const char* pszStylename, long lDefault) {
	EDTEXTSTYLE* pStyle = ll_find((void*)pTheme->th_styles, pszStylename);
	return pStyle ? pStyle->bgcolor : lDefault;
}

/*
 * Returns the font face for a given style name. If the style cannot be found or does not
 * define the font face, a default value is returned.
 */
const char* theme_textStyleFontface(const char* pszStylename, const char* pszDefault) {
	EDTEXTSTYLE* pStyle = ll_find((void*)theme_getCurrent()->th_styles, pszStylename);
	return (pStyle && pStyle->faceName[0]) ? pStyle->faceName : pszDefault;
}

/*
 * Returns the font size of the given style name. If the style cannot be found or does not
 * define the font face, a default value passed as 2nd argument is returned.
 */
int theme_textStyleFontsize(const char* pszStylename, int nDefaultHeight) {
	EDTEXTSTYLE* pStyle = ll_find((void*)theme_getCurrent()->th_styles, pszStylename);
	return (pStyle && pStyle->size) ? pStyle->size : nDefaultHeight;
}

