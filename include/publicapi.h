/*
 * publicapi.c
 *
 * Project: PKS Edit for Windows
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
	// skip multiple field seperators
	SO_SKIPSEPARATORS = 0x1,
	// sort records rather than lines
	SO_CLUSTERLINES = 0x2,
	// allow for quoting separators using "...."
	SO_CSV_QUOTING = 0x4,
	// allow for quoting separators using \..
	SO_BACKSLASH_QUOTING = 0x8,
	// Sort all lines. Do not use the selection criteria for filtering lines to sort but for 
	// cluster identification
	SO_NOSELECT = 0x10,
	// honor field separators to split key fields
	SO_TOKENIZE = 0x20
} SORT_OPTION_FLAGS;

/* Flags for align operations */
typedef enum {
	// Align all matched text to the fix offset defined by the current caret.
	AL_TO_CURSOR = 0x1,
	// Align to the offset defined by the 1st match
	AL_OFFSET_FIRST_MATCH = 0x2,
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
	MAC_AUTO = -5,
	MAC_LASTREC = -6
} RECORD_MACRO_OPTION;

/*
 * Option to be used to query various internal states of PKS Edit
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