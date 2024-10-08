/*
 * themes.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: theming (colors and fonts) for in particular text displayed by PKS Edit.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef THEMES_H

typedef struct tagEDTEXTSTYLE EDTEXTSTYLE;

typedef struct tagTHEME_DATA {
	struct tagTHEME_DATA* th_next;
	unsigned char th_name[32];
	int		 th_isWinTheme;					// true for the default (Windows Theme)
	int		 th_isDarkMode;					// true for all themes, that should use darkmode for window NC areas etc...
	COLORREF th_iconColor;					// color of the icons in the toolbar.
	COLORREF th_defaultBackgroundColor;		// standard background color of text editor text.
	COLORREF th_changedLineColor;
	COLORREF th_savedChangedLineColor;
	COLORREF th_undoAfterSaveChangedLineColor;
	COLORREF th_caretLineColor;
	COLORREF th_compareModifiedColor;
	COLORREF th_compareAddedColor;
	COLORREF th_compareDeletedColor;
	COLORREF th_rulerBorderColor;
	COLORREF th_rulerForegroundColor;
	COLORREF th_rulerBackgroundColor;
	COLORREF th_dialogBackground;
	COLORREF th_dialogForeground;
	COLORREF th_dialogBorder;
	COLORREF th_dialogDisabled;
	COLORREF th_dialogLight;
	COLORREF th_dialogActiveTab;
	COLORREF th_dialogHighlight;
	COLORREF th_dialogHighlightText;
	COLORREF th_dialogMenuHighlight;
	COLORREF th_dialogErrorText;
	COLORREF th_mainWindowBackground;
	COLORREF th_dialogLightBackground;
	unsigned char th_fontName[32];			// dialog font name to use
	unsigned int  th_fontSize;				// dialog font size to use
	unsigned char th_smallFontName[32];		// small dialog font name to use
	unsigned int  th_smallFontSize;			// small dialog font size to use
	unsigned int  th_mainframeMargin;		// The margin from the mainframe tabs to the surrounding window.
	EDTEXTSTYLE* th_styles;					// The text styles for this theme.
	EDTEXTSTYLE* th_styleLookup[50];		// pre-processed array of styles for lookup
} THEME_DATA;

#define WYSIWYG_FONT_STYLE_NAME		"wysiwyg"

/*
 * The style of a selected font
 */
typedef struct tagEDFONTATTRIBUTES {
	int			strikeout;
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
	char*		description;
} EDTEXTSTYLE;

#define SYSTEM_DEFAULT_THEME		"system default"

/*
 * Select a new theme to use by a given theme name. If the
 * named theme is not defined, a default theme is selected.
 */
extern void theme_setCurrent(unsigned char* pszThemeName);

/*
 * Returns the theme currently selected.
 */
extern THEME_DATA* theme_getCurrent();

/*
 * Returns the default theme.
 */
extern THEME_DATA* theme_getDefault();

/*
 * Destroy all theme data loaded.
 */
extern void theme_destroyAllThemeData();

/*
 * Returns the text style for a given style class.
 */
extern EDTEXTSTYLE* font_getTextStyleForIndex(THEME_DATA* pTheme, int nIndex);

/*
 * The small fixed font is used by PKS edit e.g. in code completion windows or the
 * options key controls.
 */
extern HFONT theme_createSmallFixedFont();

/*
 * Return the linked list of all defined themes.
 */
extern THEME_DATA* theme_getThemes();

/*
 * The dialog font is used by PKS edit e.g. in dialogs and in the window selector.
 */
extern HFONT theme_createDialogFont(int nWeight);

/*
 * The small dialog font is used by PKS edit e.g. in explanations in dialogs.
 */
extern HFONT theme_getSmallDialogFont();

/*
 * Prepare the passed window to be displayed in Windows dark mode look&feel or not,
 * depending on the current theme.
 */
extern void theme_enableDarkMode(HWND hwnd);

/*
 * Returns a brush to paint the background of dialogs for the currently selected theme.
 */
extern HBRUSH theme_getDialogBackgroundBrush();

/*
 * Returns a brush to paint the light background of dialogs for the currently selected theme.
 */
extern HBRUSH theme_getDialogLightBackgroundBrush();

/*
 * Prepare all children of a given parent to be used in dark mode correctly.
 */
extern void theme_prepareChildrenForDarkmode(HWND hParent);

/*
 * Set the default text colors for the given theme.
 */
extern void font_setDefaultTextColors(HDC hdc, THEME_DATA* pTheme);

/*
 * Returns the foreground color of a text style with a given name in the current theme.
 * If the style cannot be found a default value is returned.
 */
extern long theme_textStyleForeground(THEME_DATA* pTheme, const char* pszStylename, long lDefault);

/*
 * Returns the background color of a text style with a given name in the current theme.
 * If the style cannot be found a default value is returned.
 */
long theme_textStyleBackground(THEME_DATA* pTheme, const char* pszStylename, long lDefault);

/*
 * Returns the font face for a given style name. If the style cannot be found or does not
 * define the font face, a default value is returned.
 */
extern const char* theme_textStyleFontface(const char* pszStylename, const char* pszDefault);

/*
 * Returns the font size of the given style name. If the style cannot be found or does not
 * define the font face, a default value passed as 2nd argument is returned.
 */
extern int theme_textStyleFontsize(const char* pszStylename, int nDefaultHeight);

#define THEMES_H

#endif // !THEMES_H
