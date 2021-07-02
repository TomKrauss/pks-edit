/*
 * editorfont.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS 10
 *
 * purpose: text style definitions.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EDITORFONT_H

#define EDITORFONT_H

typedef struct tagWINFO WINFO;

/*
 * The style of a selected font 
 */
typedef struct tagEDFONTATTRIBUTES {
	int			strikeout;
	int			bOem;
	int			italic;
	int			underline;
	int			weight;			// 0 == default font everything else between 0 and 999 bolden factor
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
	EDFONTATTRIBUTES style;
} EDTEXTSTYLE;

/*------------------------------------------------------------
 * font_createFontWithStyle()
 * create a logical font. If the style is present it is taken
 * from the second parameter, if not, it is taken from the font.
 */
extern HFONT font_createFontWithStyle(EDTEXTSTYLE* pFont, EDFONTATTRIBUTES* pStyle);

/*------------------------------------------------------------
 * font_selectSystemFixedFont()
 */
extern void font_selectSystemFixedFont(HDC hdc);

/*------------------------------------------------------------
 * font_selectDefaultEditorFont()
 * select a font and return handle to old Font. Optionally pass a font style (may be NULL)
 */
extern HFONT font_selectDefaultEditorFont(WINFO* wp, HDC hdc, EDFONTATTRIBUTES* pStyle);

/*------------------------------------------------------------
 * font_createFontWithStyle()
 * create a logical font. If the style is present, it is used - otherwise use style info from the font.
 */
extern HFONT font_createFontWithStyle(EDTEXTSTYLE* pFont, EDFONTATTRIBUTES* pStyle);

/*------------------------------------------------------------
 * font_selectStandardFont()
 */
extern void font_selectStandardFont(HWND hwnd, WINFO* wp);

#endif