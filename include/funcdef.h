/*
 * funcdef.h
 *
 * calling a pksedit function by name
 *
 * PROJECT: PKSEDIT
 *
 *  * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 07.02.1991
 */

# ifndef FUNCDEF_H
#define	FUNCDEF_H

typedef struct tagPARAMETER_ENUM_VALUE {
	const char* pev_name;
	long		pev_val;
} PARAMETER_ENUM_VALUE;

typedef enum {
	PARAM_TYPE_BITSET = 'b',		// An "enum" type parameter which can be determined from the PARAMETER_ENUM_VALUE table used as a flag / bitset. Index into that table follows.
	PARAM_TYPE_ENUM = 'e',			// An "enum" type parameter which can be determined from the PARAMETER_ENUM_VALUE table. Index into that table follows.
	PARAM_TYPE_VOID = 0,			// end of parameter list - should support real void
	PARAM_TYPE_INT = 'i',			// this parameter is an integer
	PARAM_TYPE_STRING_ARRAY = 'a',	// this parameter / return value is an arraylist containing strings also allocated. The values are allocated by the function 
									// called and freed by the macro VM later.
	PARAM_TYPE_EDITOR_WINDOW = 'W',	// this parameter must point to a current editor window.
	PARAM_TYPE_PKS_VALUE = 'P',		// Special parameter type used as first parameter type to mark native functions working 
									// on the internal data structures of PKSMacroC and having the 
									// signature: PKS_VALUE myMethod(EXECUTION_CONTEXT*pContext, PKS_VALUE* pValues, int nArguments)
	PARAM_TYPE_STRING = 's'			// this parameter is a String parameter
} PARAMETER_TYPE;

typedef struct tagPARAMETER_TYPE_DESCRIPTOR {
	PARAMETER_TYPE			pt_type;
	PARAMETER_ENUM_VALUE*	pt_enumVal;			// for enum and bitset type parameters the first enum value
	int						pt_enumFirstIndex;	// Index of the first enum value in the enum value table.
	int						pt_enumCount;		// for enum and bitset type parameters the number of enum values
} PARAMETER_TYPE_DESCRIPTOR;

#define	EW_MODIFY		0x1			// function modifies text
#define	EW_NOCASH		0x2			// dont EdMacroRecord them
#define	EW_NEEDSCURRF	0x4			// needs a current file 
#define	EW_UNDOFLSH		0x8			// flush undo buffer before
#define	EW_NEEDSBLK		0x10		// needs a current text block 
#define	EW_HASFORM		0x20		// has a form which is opened, when the function is executed (record form params?)
#define	EW_CCASH		0x40		// put them into char buffer
#define	EW_MULTI		0x80		// has a Multiplier 
#define EW_UNDO_AVAILABLE 0x100		// undo must be possible 
#define EW_REDO_AVAILABLE 0x200		// redo must be possible 
#define	EW_FINDCURS		0x400		// applies to command executed using the mouse: before executing the command position the caret to the clicked point
#define	EW_COMPARISON_MODE	0x800	// only available in comparison mode
#define	EW_CUSTOM_ENABLEMENT 0x1000	// custom enablement function must be evaluated to calculate enablement.

typedef struct tagNATIVE_FUNCTION {
	long long (*nf_execute)();			// the actual callback to invoke 
	char nf_paramCount;					// Cache for storing the parameter count.
	int	 nf_flags;						// see EW_... flags above
	const char* nf_name;				// the name as it can be used inside the PKS Edit macro language to execute this function.
	int (*nf_checkEnabled)(long long pParam);	// Optional enablement function allowing to check, whether the execute function can currently be invoked.
	signed char* nf_paramTypes;			// Signature description of the function. Contains a string encoded using the constants defined above (PAR_...)
	const char* nf_description;			// Optional help text used during code completion.
	const char* nf_parameters;			// Optional comma seperated list of parameters passed (type + name). Used for documentary purpose.
} NATIVE_FUNCTION;

/*
 * Can be used to find out, whether a method is more tightly integrated with the macroC VM and works on PKS_VALUES directly.
 * Signature is: PKS_VALUE myMethod(EXECUTION_CONTEXT*pContext, PKS_VALUE* pValues, int nArguments)
 */
extern int function_hasInternalVMPrototype(NATIVE_FUNCTION* ep);

extern void function_destroyRegisteredNative(NATIVE_FUNCTION* pFunc);

/*
 * Register a macro C function given the name with which it should be visible in PKSMacroC, the windows proc name, the optional
 * module (if null it is loaded from PKS-Edit), the signature description and an optional help text.
 */
int function_registerNativeFunction(const char* pszMacroCName, const char* pszFunctionName, 
		const char* pszModule, const char* pszSignature, const char* pszDescription, const char* pszParameters);

/*
 * Returns the parameter descriptor for a function for the n-th parameter. Parameter count
 * starts with 1, parameter type 0 is the return type of the function.
 */
extern PARAMETER_TYPE_DESCRIPTOR function_getParameterTypeDescriptor(NATIVE_FUNCTION* ep, int nParamIdx);

extern int function_parameterIsFormStart(NATIVE_FUNCTION* ep, int parno);

/*
 * Returns the number of parameters of a native macro function.
 */
extern int function_getParameterCount(NATIVE_FUNCTION* ep);

#define MAX_NATIVE_FUNCTIONS				256

extern int 		_functionTableSize;
extern NATIVE_FUNCTION	_functionTable[];

/*
 * Returns the number of statically defined functions.
 */
extern int function_getNumberOfStaticallyDefinedFunctions();

extern int		_parameterEnumValueTableSize;
extern PARAMETER_ENUM_VALUE	_parameterEnumValueTable[];

#define 	FUNC_EdBlockCopy				2
#define 	FUNC_EdBlockMove				6
#define 	FUNC_EdKeycodeInsert			16
#define 	FUNC_EdCharInsert				17
#define 	FUNC_EdOptionToggle				78
#define		FUNC_CreateArray				95
#define		FUNC_CreateMap					168
#define 	FUNC_Foreach					139
#define 	FUNC_RegisterNative				158

#define CMD_OPEN_FILE	0 
#define CMD_OPEN_NEW_FILE	1 
#define CMD_OPEN_FILE_DIR_GIVEN	2 
#define CMD_OPEN_SPECIAL	3
#define CMD_OPEN_HISTORY	4
#define CMD_OPEN_HISTORY1	5
#define CMD_OPEN_HISTORY2	6
#define CMD_OPEN_HISTORY3	7
#define CMD_OPEN_HISTORY4	8
#define CMD_QUIT_FILE	9 
#define CMD_SAVE_FILE_AND_CLOSE	10 
#define CMD_SAVE_FILE_AS	11 
#define CMD_SAVE_FILE	12 
#define CMD_READ_BLOCK	13 
#define CMD_SAVE_BLOCK	14 
#define CMD_EXIT_EDIT	15 
#define CMD_QUIT_EDIT	16 
#define CMD_FIND_STRING	17 
#define CMD_REPLACE_STRING	18 
#define CMD_FIND_AGAIN	19 
#define CMD_FIND_AGAIN_FORWARD	20 
#define CMD_FIND_AGAIN_BACKWARD	21 
#define CMD_FIND_WORD_FORWARD	22 
#define CMD_FIND_TAG_UNDER_CURSOR	23 
#define CMD_FIND_WORD_BACK	24 
#define CMD_FIND_FILE_UNDER_CURSOR	25 
#define CMD_FIND_IN_FILELIST	26 
#define CMD_FIND_TAG	27 
#define CMD_SET_MARK	28 
#define CMD_GOTO_MARK	29 
#define CMD_GOTO_LINE	30 
#define CMD_GOTO_LAST_POS	31 
#define CMD_GOTO_LAST_INSERTION_POS	32 
#define CMD_MARK_BLOCK_START	33 
#define CMD_MARK_BLOCK_END	34 
#define CMD_MARK_ALL	35 
#define CMD_HIDE_BLOCK_MARKS	36 
#define CMD_FIND_BLOCK_START	37 
#define CMD_FIND_BLOCK_END	38 
#define CMD_CUT_BLOCK	39
#define CMD_CUT_TO_NAMED_BUFFER	40
#define CMD_COPY_TO_NAMED_BUFFER	41 
#define CMD_COPY_TO_CLIPBOARD	42 
#define CMD_PASTE_NAMED_BUFFER	43 
#define CMD_PASTE_CLIPBOARD	44 
#define CMD_PASTE_TRASH	45 
#define CMD_ERRORLIST_CURRENT_LINE	46
#define CMD_DELETE_BLOCK	47
#define CMD_COPY_BLOCK	48 
#define CMD_MOVE_BLOCK	49 
#define CMD_CUT_LINES	50 
#define CMD_TOGGLE_COLUMN_SELECTION	51 
#define CMD_TOGGLE_SHOW_CONTROL	52 
#define CMD_TOGGLE_SHOW_RULER	53 
#define CMD_DIALOG_PRINTER_LAYOUT	54 
#define CMD_TOGGLE_HEX_MODUS	55 
#define CMD_DELETE_MULTIPLE_SPACES	56 
#define CMD_TOGGLE_INSERT_MODE	57 
#define CMD_SET_OPTIONS	58 
#define CMD_FORMAT_TEXT	59 
#define CMD_COMPRESS_TABS	60 
#define CMD_EXPAND_TABS	61 
#define CMD_ALIGN_TEXT	62 
#define CMD_SHIFT_RANGE_RIGHT	63 
#define CMD_SHIFT_RANGE_LEFT	64 
#define CMD_SORT_TEXT	65 
#define CMD_NEXT_DIFFERENCE	66 
#define CMD_PRINT_WINDOW	67 
#define CMD_PRINT_BLOCK	68 
#define CMD_PRINT_FILE	69 
#define CMD_EXECUTE_COMMAND	70 
#define CMD_READ_TAGFILE	71 
#define CMD_READ_SEARCH_LIST	72 
#define CMD_READ_COMPILER_ERRORS	73 
#define CMD_ADD_DOC_MACROS	74 
#define CMD_RECORD_MACRO	75 
#define CMD_DIALOG_MACROS	76 
#define CMD_WRITE_MACROS	77 
#define CMD_READ_MACROS	78 
#define CMD_SAVE_OPTIONS	79 
#define CMD_CONFIGURE_ICONS	80 
#define CMD_DIALOG_DISPLAY_MODE	81 
#define CMD_DIALOG_WORK_MODE	82 
#define CMD_DIALOG_TEXT_CURSOR	83 
#define CMD_DIALOG_MACRO_BINDINGS	84 
#define CMD_SAVE_RULER	85 
#define CMD_READ_RULER	86 
#define CMD_INFO_TO_FILE	87 
#define CMD_CLOSE_ALL_WINDOWS	88 
#define CMD_ARRANGE_ICONS	89 
#define CMD_CASCADE_WINDOWS	90 
#define CMD_TILE_WINDOWS	91 
#define CMD_HELP_README	92
#define CMD_HELP_OVERVIEW	93 
#define CMD_HELP_ON_KEYS	94 
#define CMD_HELP_RELEASE_NOTES	95 
#define CMD_HELP_MANUAL_TOC	96 
#define CMD_SHOW_COPYRIGHT	97 
#define CMD_KEYCODE_INSERT	98 
#define CMD_EVALUATE_SELECTION	99 
#define CMD_TOGGLE_FULL_SCREEN	100 
#define CMD_TOGGLE_SHOW_WYSIWYG	101
#define CMD_SET_WIN_ZOOMED	105 
#define CMD_TILE_DOCKS_HORIZONTALLY	106
#define CMD_TILE_DOCKS_VERTICALLY	107
#define CMD_ERRORLIST_CLICK_LINE	108
#define CMD_OPEN_CONTEXT_MENU	109
#define CMD_SEARCH_ON_INTERNET	110 
#define CMD_UNUSED	111 
#define CMD_SELECT_WINDOW_1	112 
#define CMD_SELECT_WINDOW_2	113 
#define CMD_SELECT_WINDOW_3	114 
#define CMD_SELECT_WINDOW_4	115 
#define CMD_SELECT_WINDOW_5	116 
#define CMD_SELECT_WINDOW_6	117 
#define CMD_SELECT_PREVIOUS_WINDOW	118 
#define CMD_CYCLE_WINDOW	119 
#define CMD_CHAR_UP_LOW	120 
#define CMD_SHIFT_LINE_LEFT	121 
#define CMD_SHIFT_LINE_RIGHT	122 
#define CMD_REPLACE_AGAIN	123 
#define CMD_ERRORLIST_NEXT	124 
#define CMD_ERRORLIST_END	125 
#define CMD_ERRORLIST_PREVIOUS	126 
#define CMD_ERRORLIST_START	127 
#define CMD_CHECK_BRACKETS	128 
#define CMD_INSERT_CHAR_WITH_DIALOG	129 
#define CMD_SHIFT_BETWEEN_BRACKETS_LEFT	130 
#define CMD_SHIFT_BETWEEN_BRACKETS_RIGHT	131 
#define CMD_DELETE_CHAR	132 
#define CMD_DELETE_TO_EOL	133 
#define CMD_DELETE_WORD	134 
#define CMD_DELETE_TO_NEXT_CHAR	135 
#define CMD_DELETE_BACK_WHITE	136 
#define CMD_DELETE_TO_WHITE	137 
#define CMD_DELETE_TO_SOL	138 
#define CMD_DELETE_BACK_WORD	139 
#define CMD_DELETE_UNTIL_PREVIOUS_CHAR	140 
#define CMD_SPLIT_LINE	141 
#define CMD_SPLIT_LINE_INDENT_PLUS	142 
#define CMD_SPLIT_LINE_INDENT_MINUS	143 
#define CMD_JOIN_LINES	144 
#define CMD_CURSOR_UP_LINE	145 
#define CMD_CURSOR_TOP_OF_PAGE	146 
#define CMD_CURSOR_BACK_PAGE	147 
#define CMD_CURSOR_DOWN_LINE	148 
#define CMD_CURSOR_BOTTOM_OF_PAGE	149 
#define CMD_CURSOR_DOWN_PAGE	150 
#define CMD_CURSOR_RIGHT_CHAR	151 
#define CMD_CURSOR_EOL	152 
#define CMD_CURSOR_RIGHT_WORD	153 
#define CMD_CURSOR_UNTIL_CHAR	154 
#define CMD_CURSOR_LEFT_CHAR	155 
#define CMD_CURSOR_SOL	156 
#define CMD_CURSOR_LEFT_WORD	157 
#define CMD_CURSOR_BACK_TO_CHAR	158 
#define CMD_CURSOR_TO_SPACE	159 
#define CMD_CURSOR_BACK_TO_SPACE	160 
#define CMD_CURSOR_HOME	161 
#define CMD_CURSOR_EOF	162 
#define CMD_INSERT_LINE_FOLLOW	163 
#define CMD_DELETE_LINE_STAY	164 
#define CMD_DELETE_LINE_APPEND	165 
#define CMD_DELETE_LINE_FOLLOW	166 
#define CMD_ABANDON_FILE	167 
#define CMD_SAVE_AND_CLOSE	168 
#define CMD_INSERT_LINE_STAY	169 
#define CMD_EDIT_DOC_MACROS	170 
#define CMD_DELETE_NAMED_BUFFERS	171 
#define CMD_DELETE_MARKED_LINES	172 
#define CMD_SET_MULTIPLIKATOR	173 
#define CMD_ESCAPE_MAKRO	174 
#define CMD_SWITCH_FKEYS	175 
#define CMD_DIALOG_DOC_TYPES	176 
#define CMD_RE_INSERT	178 
#define CMD_UNDO	180 
#define CMD_TOGGLE_LINE_MARKS	181 
#define CMD_ICONIZE_EDIT	182 
#define CMD_ICONIZE_WINDOW	183 
#define CMD_LAST_DIFFERENCE	184 
#define CMD_SCROLL_UP_LINE	185 
#define CMD_SCROLL_DOWN_LINE	186 
#define CMD_SCROLL_UP_PAGE	187 
#define CMD_SCROLL_DOWN_PAGE	188 
#define CMD_CURSOR_CENTER_PAGE	189 
#define CMD_SCROLL2_UP_LINE	190 
#define CMD_SCROLL2_DOWN_LINE	191 
#define CMD_SCROLL_CENTER_PAGE	192 
#define CMD_JOIN_MARKED_LINES	193 
#define CMD_EDIT_LOCAL_DOC_MACROS	194 
#define CMD_UNDEFINED	195 
#define CMD_PRINT_CLIPBOARD	196 
#define CMD_PRINT_TRASH	197 
#define CMD_PASTE_UNDO	198 
#define CMD_PASTE_CLIP_ICON	199 
#define CMD_ICON_EXECUTE	200 
#define CMD_REMOVE_ICON	201 
#define CMD_CURSOR_MOUSE_GOTO	202 
#define CMD_DRAG_COPY_TEXT	203 
#define CMD_DRAG_MOVE_TEXT	204 
#define CMD_MOUSE_SELECT_LINES	205 
#define CMD_MOUSE_UNSELECT_LINES	206 
#define CMD_PLAY_LAST_MACRO	207 
#define CMD_MOUSE_MARK_BLOCK	208 
#define CMD_MOUSE_MARK_LINE	209 
#define CMD_MOUSE_MARK_WORD	210 
#define CMD_MOUSE_MARK_TO_SPACE	211 
#define CMD_MOUSE_MARK_PARAGRAPH	212 
#define CMD_MOUSE_UPDATE_BLOCK	213 
#define CMD_MACRO_LIST	214 
#define CMD_KEY_LIST	215 
#define CMD_MOUSE_LIST	216 
#define CMD_COMPILE_MACROS	217 
#define CMD_SPLIT_LINE_SOFT	218 
#define CMD_SHOW_STD_CLIP	219 
#define CMD_ERASE_BLOCK	220 
#define CMD_MENU_LIST	221 
#define CMD_REDO	222 
#define CMD_TOGGLE_SHOW_LINENUMBERS	223 
#define CMD_SELECT_LEFT	224 
#define CMD_SELECT_LEFT_TO_START	225 
#define CMD_SELECT_LEFT_WORD	226 
#define CMD_SELECT_RIGHT	227 
#define CMD_SELECT_RIGHT_WORD	228 
#define CMD_SELECT_RIGHT_TO_END	229 
#define CMD_SELECT_UP	230 
#define CMD_SELECT_DOWN	231 
#define CMD_SELECT_PAGE_UP	232 
#define CMD_SELECT_PAGE_DOWN	233 
#define CMD_SAVE_ALL_FILES	234 
#define CMD_SEARCH_INCREMENTALLY	235 
#define CMD_CLONE_WINDOW	236 
#define CMD_TAB_SHIFT_RIGHT	237 
#define CMD_TAB_SHIFT_LEFT	238 
#define CMD_ZOOM_INCREASE	239 
#define CMD_ZOOM_DECREASE	240 
#define CMD_TOGGLE_COMMENT	241 
#define CMD_GOTO_NEXT_POS	242 
#define CMD_GOTO_NEXT_INSERTION_POS	243 
#define CMD_USE_LINUX_LINEENDS	244 
#define CMD_USE_WINDOWS_LINEENDS	245 
#define CMD_CHAR_TO_UPPER	246 
#define CMD_CHAR_TO_LOWER	247 
#define CMD_COMPARE_NAVIGATE_NEXT	248
#define CMD_COMPARE_NAVIGATE_PREVIOUS	249
#define CMD_COMPARE_CLEAR	250
#define CMD_TOGGLE_LINE_SELECTION	251
#define CMD_TOGGLE_SYNTAX_HIGHLIGHTING	252
#define CMD_MOVE_SELECTION_UP	253
#define CMD_MOVE_SELECTION_DOWN 	254


typedef enum { OP_EDIT_MODE, OP_DISPLAY_MODE, OP_MACRO, OP_OPTIONS, OP_FILEFLAG } OP_FLAGTYPE;

#define	OPT_WMODE			(((long)OP_EDIT_MODE)<<16)
#define	OPT_DMODE			(((long)OP_DISPLAY_MODE)<<16)
#define	OPT_GLOBSET			(((long)OP_OPTIONS)<<16L)
#define  OPT_RECORD			(((long)OP_MACRO)<<16L)

# endif	/* FUNCDEF_H */


