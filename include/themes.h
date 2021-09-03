/*
 * themes.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS 10
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
	COLORREF th_defaultBackgroundColor;
	COLORREF th_changedLineColor;
	COLORREF th_savedChangedLineColor;
	COLORREF th_caretLineColor;
	COLORREF th_markedLineColor;
	COLORREF th_rulerBorderColor;
	COLORREF th_rulerForegroundColor;
	COLORREF th_rulerBackgroundColor;
	COLORREF th_dialogBackground;
	COLORREF th_dialogForeground;
	COLORREF th_mainWindowBackground;
	unsigned char th_fontName[32];			// dialog font name to use
	unsigned int  th_fontSize;				// dialog font size to use
	unsigned char th_smallFontName[32];		// small dialog font name to use
	unsigned int  th_smallFontSize;			// small dialog font size to use
	EDTEXTSTYLE* th_styles;					// The text styles for this theme.
} THEME_DATA;

/*
 * Select a new theme to use by a given theme name. If the
 * named theme is not defined, a default theme is selected.
 */
extern void theme_setCurrent(unsigned char* pszThemeName);

/*
 * Returns the default theme currently selected.
 */
extern THEME_DATA* theme_getDefault();

/*
 * Destroy all theme data loaded.
 */
extern void theme_destroyAllThemeData();

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
HFONT theme_createDialogFont(int nWeight);

#define THEMES_H

#endif // !THEMES_H
