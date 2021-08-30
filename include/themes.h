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
	unsigned char th_fontName[32];			// dialog font name to use
	unsigned int  th_fontSize;				// dialog font size to use
	unsigned char th_smallFontName[32];		// small dialog font name to use
	unsigned int  th_smallFontSize;			// small dialog font size to use
} THEME_DATA;

/*
 * Returns the theme with the given name. If not found, a default theme is returned. 
 */
extern THEME_DATA* theme_getByName(unsigned char* pThemeName);

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
 * The dialog font is used by PKS edit e.g. in dialogs and in the window selector.
 */
HFONT theme_createDialogFont(int nWeight);

#define THEMES_H

#endif // !THEMES_H
