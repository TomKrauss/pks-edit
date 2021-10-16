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

typedef enum { FS_NORMAL = 0, FS_CONTROL_CHARS = 1, FS_HYPERLINK = 2 } FONT_STYLE_CLASS;

typedef struct tagWINFO WINFO;

/*
 * The style of a selected font 
 */
typedef struct tagEDFONTATTRIBUTES EDFONTATTRIBUTES;

/*
 * Reusable font/color style object.
 */
typedef struct tagEDTEXTSTYLE EDTEXTSTYLE;

typedef struct tagTHEME_DATA THEME_DATA;

/*------------------------------------------------------------
 * font_selectFontStyle()
 * select a font and return handle to old Font. Optionally pass a font style (may be NULL)
 */
extern void font_selectFontStyle(THEME_DATA* pTheme, WINFO* wp, FONT_STYLE_CLASS nStyleIndex, HDC hdc);

/*------------------------------------------------------------
 * Create a font with a name (possible empty) and a given size.
 * If 'bOem' is true, use an OEM_CHARSET.
 */
extern HFONT font_createFontHandle(char* pszFontName, int size, int bOem, int nWeight);

/*------------------------------------------------------------
 * font_selectStandardFont()
 */
extern void font_selectStandardFont(HWND hwnd, WINFO* wp);

/*
 * Determine the logical style class index for a style name.
 */
extern FONT_STYLE_CLASS font_getStyleClassIndexFor(char* pszStyleName);

#endif