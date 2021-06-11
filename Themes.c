/*
 * Themes.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS 10
 *
 * purpose: terminal emulation stuff
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */

#include <windows.h>
#include "themes.h"
#include "arraylist.h"

#define DEFAULT_CONTROL_COLOR 0x00AAAAAA
#define CARET_LINE_COLOR RGB(235, 255, 235)
#define MARKED_LINE_COLOR RGB(200, 200, 255)
#define RULER_BORDER_COLOR RGB(220,220,220)
#define RULER_FOREROUND_COLOR RGB(140,140,140)
#define RULER_BACKGROUND_COLOR RGB(248,248,248)

static THEME_DATA _defaultTheme = {
	"default",
	DEFAULT_CONTROL_COLOR,
	CARET_LINE_COLOR,
	MARKED_LINE_COLOR,
	RULER_BORDER_COLOR,
	RULER_FOREROUND_COLOR,
	RULER_BACKGROUND_COLOR
};

static ARRAY_LIST *_themes;

/*
 * Registers a new Theme. The theme will be passed by value.
 * 
 */
void theme_register(THEME_DATA theme) {
	THEME_DATA* pTheme = calloc(1, sizeof * pTheme);
	memcpy(pTheme, &theme, sizeof theme);
	if (_themes == NULL) {
		_themes = arraylist_create(0);
	}
	arraylist_add(_themes, (void*) pTheme);
}

/*
 * Returns the theme with the given name. If not found, a default theme is returned.
 */
THEME_DATA* theme_getByName(unsigned char* pThemeName) {
	if (_themes != NULL) {
		ARRAY_ITERATOR ip = arraylist_iterator(_themes);
		for (void** pThemePointer = ip.i_buffer; ; pThemePointer++) {
			THEME_DATA* pTheme = *pThemePointer;
			if (strcmp(pTheme->th_name, pThemeName) == 0) {
				return pTheme;
			}
		}
	}
	return &_defaultTheme;
}


