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
 * (c) Pahlen & Krauss
 */

#ifndef THEMES_H

typedef enum { FS_NORMAL, FS_CONTROL_CHARS, FS_COMMENT, FS_IDENTIFIER, FS_KEYWORD, FS_OPERATOR, FS_STRING } FONT_STYLE_CLASS;

typedef struct tagTHEME_DATA {
	unsigned char* th_name;
	COLORREF th_defaultControlColor;
	COLORREF th_caretLineColor;
	COLORREF th_markedLineColor;
	COLORREF th_rulerBorderColor;
	COLORREF th_rulerForegroundColor;
	COLORREF th_rulerBackgroundColor;
} THEME_DATA;

/*
 * Returns the theme with the given name. If not found, a default theme is returned. 
 */
extern THEME_DATA* theme_getByName(unsigned char* pThemeName);

#define THEMES_H

#endif // !THEMES_H
