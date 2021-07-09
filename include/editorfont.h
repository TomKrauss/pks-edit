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

typedef enum { FS_NORMAL = 0, FS_CONTROL_CHARS = 1, FS_COMMENT = 2, FS_IDENTIFIER = 3, FS_KEYWORD = 4, FS_OPERATOR = 5, FS_STRING = 6, FS_LAST } FONT_STYLE_CLASS;

typedef struct tagWINFO WINFO;

/*
 * The style of a selected font 
 */
typedef struct tagEDFONTATTRIBUTES EDFONTATTRIBUTES;

/*
 * Reusable font/color style object.
 */
typedef struct tagEDTEXTSTYLE EDTEXTSTYLE;

/*------------------------------------------------------------
 * font_selectFontStyle()
 * select a font and return handle to old Font. Optionally pass a font style (may be NULL)
 */
extern void font_selectFontStyle(WINFO* wp, FONT_STYLE_CLASS nStyleIndex, HDC hdc);

/*------------------------------------------------------------
 * Create a font with a name (possible empty) and a given size.
 * If 'bOem' is true, use an OEM_CHARSET.
 */
extern HFONT font_createFontHandle(char* pszFontName, int size, int bOem);

/*------------------------------------------------------------
 * font_selectStandardFont()
 */
extern void font_selectStandardFont(HWND hwnd, WINFO* wp);

#endif