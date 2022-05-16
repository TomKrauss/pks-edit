/*
 * publicapi.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose:
 * defines all macro callable editor API functions.
 *
 *
 * 										created: 20.02.87
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef PUBLICAPI_H

#define PUBLICAPI_H

 /* Flags for Listing operations (navigation along search result lists etc. */
typedef enum {
	LIST_START = 0,
	LIST_PREV = 1,
	LIST_CURR = 2,
	LIST_NEXT = 3,
	LIST_END = 4,
	LIST_USETOPWINDOW = 0x100
} LIST_OPERATION_FLAGS;

/*
 * Used in list binding to define which type of binding should be displayed.
 */
typedef enum {
	// Print and display all macros with documentation and source code
	LISTB_MACROS = 0x1,
	// Print and display the current keystroke action bindings
	LISTB_KEYS = 0x2,
	// Print and display the current mouse click action bindings
	LISTB_MICE = 0x4,
	// Print and display the current list of menu bindings
	LISTB_MENUS = 0x8,
	// Print and display the PKSMacroC API
	LISTB_MACROC_API = 0x10
} LIST_BINDING_OPTION;

/* Option flags for sorting text */
typedef enum {
	/* skip multiple Token Seperators */
	SO_SKIPSEPS = 0x1,
	/* sort records rather than lines */
	SO_CLUSTERLINES = 0x2,
	/* reverse the order of sorting */
	SO_REVERSE = 0x4,
	/* sort all lines */
	SO_NOSELECT = 0x8,
	/* at least one token key field */
	SO_TOKENIZE = 0x10
} SORT_OPTION_FLAGS;

/* Flags for align operations */
typedef enum {
	// Ignore all characters in front of caret position
	AL_CPOS = 0x1,
	// Align straight to caret position
	AL_FIX = 0x2,
	// Align to end of search pattern match
	AL_END = 0x4
} ALIGN_FLAGS;

/* Flags for EdBlockPaste */
typedef enum {
	PASTE_QUERYID = 0x100,
	PASTE_CLIP = 0x200,
	PASTE_UNDO = 0x400,
	PASTE_ICON = 0x1000
} PASTE_FLAGS;

/* Flags for Cut&EdBlockPaste */
typedef enum {
	CUT_QUERYID = 0x100,
	CUT_APPND = 0x200,
	CUT_USEPP = 0x400,
	CUT_DELETE = 0x800
} CUT_FLAGS;

/* Flags for EdMacroRecorded */
typedef enum {
	MAC_AUTO = 1,
	MAC_LASTREC = 2
} RECORD_MACRO_OPTION;

/*
 * Option to be used to query various internal states of PKS-Edit
 * from within PKSMacroC.
 */
typedef enum {
	QUERY_CLIPBOARDAVAIL = 0,
	QUERY_HAS_SELECTION_START = 1,
	QUERY_HAS_SELECTION_END = 2,
	QUERY_HAS_SELECTION = 3,
	QUERY_CURRENTFILE = 4,
	QUERY_OPTIONS = 5,
	QUERY_LAYOUTOPTIONS = 6,
	QUERY_WORKMODE = 7,
	QUERY_DISPLAYMODE = 8,
	QUERY_FILEMODIFIED = 9,
	QUERY_BLOCKXTNDMODE = 10
} QUERY_EDITOR_OPTION;


#endif