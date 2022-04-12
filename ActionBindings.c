/*
 * ActionBindings.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: define all action triffers in PKS-Edit (menubar, context-menu, toolbar,
 * keycode, mouse events) and to the actions to be executed.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include "documentmodel.h"
#include "pksmacro.h"
#include "actionbindings.h"
#include "linkedlist.h"
#include "jsonparser.h"
#include "hashmap.h"
#include "arraylist.h"
#include "fontawesome.h"
#include "xdialog.h"

#define MAX_CONTEXT_NAME_LEN		32
#define MAX_ID_LEN					10

typedef struct tagLOCAL_ACTION_BINDING {
	struct tagLOCAL_ACTION_BINDING* ab_next;		// next action binding 
	struct tagLOCAL_ACTION_BINDING* ab_children;	// for nested actions (e.g. menus) - this is the possibly null list of children.
	char ab_context[MAX_CONTEXT_NAME_LEN];	// context name
	char ab_anchor[MAX_ID_LEN];				// anchor name
	char ab_reference[MAX_ID_LEN];			// reference for extending action bindings referring to an anchor
	union {
		KEY_BINDING		keybind;
		MOUSE_EVENT_BINDING	mousebind;
		MENU_ITEM_DEFINITION subMenu;
		TOOLBAR_BUTTON_BINDING toolbarButtonBinding;
	} ab_binding;
} LOCAL_ACTION_BINDING;

typedef struct tagACTION_BINDINGS {
	struct tagACTION_BINDINGS* ab_next;		// next keybinding context
	char ab_context[MAX_CONTEXT_NAME_LEN];	// action bindings context name
	HASHMAP* ab_keyBindingTable;			// keybindings: maps KEYCODES to MACROREFs 
	LOCAL_ACTION_BINDING* ab_mouseBindings;		// the defined mouse bindings
} ACTION_BINDINGS;

static ACTION_BINDINGS* _actionBindings;
static LOCAL_ACTION_BINDING*  _contextMenu;		// the context menu definitions
static LOCAL_ACTION_BINDING*  _menu;				// the main menu definitions
static LOCAL_ACTION_BINDING*  _toolbar;			// the toolbar definitions

typedef struct tagJSON_BINDINGS {
	LOCAL_ACTION_BINDING* keys;
	LOCAL_ACTION_BINDING* mouse;
	LOCAL_ACTION_BINDING* subMenu;
	LOCAL_ACTION_BINDING* menu;
	LOCAL_ACTION_BINDING* toolbarButtonBinding;
} JSON_BINDINGS;

typedef struct tagMODIFIER_NAME {
	const char* mn_mame;
	int	        mn_modifier;
} MODIFIER_NAME;

static MODIFIER_NAME _modifierNames[] = {
	{"Ctrl",	K_CONTROL},
	{"Shift",	K_SHIFT},
	{"Alt",		K_ALTERNATE},
	{"Win",		K_WINDOWS},
	{"Selected",K_HAS_SELECTION}
};

static char* _scantab =
"\x03" "CANCEL\0" "\x08" "BACK\0" "\x09" "TAB\0" "\x0C" "CLEAR\0" "\x0D" "RETURN\0"
"\x13" "PAUSE\0" "\x14" "CAPITAL\0" "\x1B" "ESCAPE\0" "\x20" "SPACE\0" "\x21" "PRIOR\0"
"\x22" "NEXT\0" "\x23" "END\0" "\x24" "HOME\0" "\x25" "LEFT\0" "\x26" "UP\0"
"\x27" "RIGHT\0" "\x28" "DOWN\0" "\x29" "SELECT\0" "\x2A" "PRINT\0" "\x2B" "EXECUTE\0"
"\x2C" "SNAPSHOT\0" "\x2D" "INSERT\0" "\x2E" "DELETE\0" "\x2F" "HELP\0" "\x60" "NUMPAD0\0"
"\x61" "NUMPAD1\0" "\x62" "NUMPAD2\0" "\x63" "NUMPAD3\0" "\x64" "NUMPAD4\0" "\x65" "NUMPAD5\0"
"\x66" "NUMPAD6\0" "\x67" "NUMPAD7\0" "\x68" "NUMPAD8\0" "\x69" "NUMPAD9\0" "\x6A" "MULTIPLY\0"
"\x6B" "ADD\0" "\x6C" "SEPARATOR\0" "\x6D" "SUBTRACT\0" "\x6E" "DECIMAL\0" "\x6F" "DIVIDE\0"
"\x70" "F1\0" "\x71" "F2\0" "\x72" "F3\0" "\x73" "F4\0" "\x74" "F5\0" "\x75" "F6\0"
"\x76" "F7\0" "\x77" "F8\0" "\x78" "F9\0" "\x79" "F10\0" "\x7A" "F11\0" "\x7B" "F12\0"
"\x90" "NUMLOCK\0" "\x91" "SCROLL\0" "\x92" "OEM_NEC_EQUAL\0"
"\xBA" "OEM1\0" "\xBB" "OEM_PLUS\0" "\xBC" "OEM_KOMMA\0" "\xBD" "OEM_MINUS\0" "\xBE"  "OEM_PERIOD\0"
"\xDB" "OEM_4\0" "\xDC" "OEM_5\0" "\xDD" "OEM_6\0" "\xDE" "OEM_7\0"  "\xDF" "OEM_8\0"
"\x7C" "F13\0" "\x7D" "F14\0" "\x7E" "F15\0" "\x7F" "F16\0" "\x90" "NUMLOCK\0\0";

/*
 * Can be used to add all keycode names and modifiers to a suggestion list.
 */
void bindings_addModifiersAndKeycodes(int (*fMatch)(const char* pszString), void (*fCallback)(const char* pszString)) {
	char szBuf[64];
	for (int i = 0; i < sizeof(_modifierNames) / sizeof(_modifierNames[0]); i++) {
		strcpy(szBuf, _modifierNames[i].mn_mame);
		strcat(szBuf, "+");
		fCallback(szBuf);
	}
	for (char* pszCode = _scantab; *pszCode; ) {
		fCallback(pszCode + 1);
		pszCode += strlen(pszCode) + 1;
	}
}

/*
 * bindings_printModifier()
 * Print the modifiers in the form "Alt+Shift...." into pszDestination and return the character
 * pointer at the end of the printed text.
 */
char* bindings_printModifier(char* pszDestination, KEYCODE code) {
	for (int i = 0; i < sizeof(_modifierNames) / sizeof(_modifierNames[0]); i++) {
		if (code & _modifierNames[i].mn_modifier) {
			strcpy(pszDestination, _modifierNames[i].mn_mame);
			pszDestination += strlen(_modifierNames[i].mn_mame);
			*pszDestination++ = '+';
		}
	}
	*pszDestination = 0;
	return pszDestination;
}


/*
 * Parses a modifier (e.g. Shift) and returns the corresponding modifier constant.
 */
static int bindings_parseModifier(const char* pszKeycode) {
	for (int i = 0; i < sizeof(_modifierNames) / sizeof(_modifierNames[0]); i++) {
		if (strcmp(pszKeycode, _modifierNames[i].mn_mame) == 0) {
			return _modifierNames[i].mn_modifier;
		}
	}
	return 0;
}


/*--------------------------------------------------------------------------
 * bindings_parseKeycode()
 * parse a keycode given as a string (e.g. Shift+TAB) and convert to the corresponding
 * KEYCODE.
 */
static KEYCODE bindings_parseKeycode(const unsigned char* pszKeycode) {
	unsigned char* t;
	const unsigned char* K;
	int  			code;
	int nModifier = 0;

	while (1) {
		char* pszNext = strchr(pszKeycode, '+');
		if (pszNext) {
			*pszNext++ = 0;
			nModifier |= bindings_parseModifier(pszKeycode);
		}
		else {
			break;
		}
		pszKeycode = pszNext;
	}
	if (!pszKeycode[1]) {
		if ((*pszKeycode >= '0' && *pszKeycode <= '9') ||
			(*pszKeycode >= 'A' && *pszKeycode <= 'Z')) {
			return *pszKeycode | nModifier;
		}
	}

	if (pszKeycode[0] == '\\') {
		code = (int)(pszKeycode[1] - '0') * 100;
		code += (int)(pszKeycode[2] - '0') * 10;
		code += (int)(pszKeycode[3] - '0');
		return code | nModifier;
	}

	K = pszKeycode;
	for (t = _scantab; *t; ) {
		code = *t++;
		for (pszKeycode = K; *pszKeycode == *t; pszKeycode++, t++)
			if (*t == 0) {
				return code | nModifier;
			}
		while (*t++)
			;
	}
	return K_INVALID;
}

/*
 * Create a string representation of a keycode. 
 */
char* bindings_keycodeToString(KEYCODE code) {
	char* s;
	unsigned char* t;
	static char b[32];

	if (code == K_DELETED)
		return "*NO KEY*";

	s = bindings_printModifier(b, code);

	code &= 0xFF;

	if ((code >= '0' && code <= '9') ||
		(code >= 'A' && code <= 'Z')) {
		*s++ = (char)code;
		*s = 0;
		return b;
	}

	for (t = _scantab; *t; ) {
		if (code == *t) {
			t++;
			while ((*s++ = *t++) != 0)
				;
			return b;
		}
		while (*t++)
			;
	}

	sprintf(s, "\\%03d", code);
	return b;
}

static int bindings_parseCommandMP(MACROREF* mp, const char* pszCommand) {
	BOOL bCommand = (pszCommand[0] == '@');
	int index = bCommand ? macro_getCmdIndexByName(pszCommand + 1) : macro_getInternalIndexByName(pszCommand);
	if (index < 0) {
		mp->index = 0;
		mp->typ = CMD_NONE;
	} else {
		mp->index = index;
		mp->typ = bCommand ? CMD_CMDSEQ : CMD_MACRO;
	}
	return 1;
}

/*
 * Name of the default action context to use in PKS-Edit.
 */
const char* DEFAULT_ACTION_CONTEXT = "default";

static int bindings_parseMouseCommand(LOCAL_ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.mousebind.macref, pszCommand);
}

static int bindings_parseKeyCommand(LOCAL_ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.keybind.macref, pszCommand);
}

static int bindings_parseContextmenuCommand(LOCAL_ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.subMenu.mid_command, pszCommand);
}

static int bindings_parseToolbarCommand(LOCAL_ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.toolbarButtonBinding.tbb_macref, pszCommand);
}

static int bindings_parseAndAssignKeycode(LOCAL_ACTION_BINDING* pTarget, const char* pszKeycode) {
	pTarget->ab_binding.keybind.keycode = bindings_parseKeycode(pszKeycode);
	return 1;
}

static int bindings_parseMouse(LOCAL_ACTION_BINDING* pTarget, const char* pszMouse) {
	int nModifier = 0;
	int nClicks = 1;
	while (pszMouse) {
		char* pszNext = strchr(pszMouse, '+');
		if (pszNext) {
			*pszNext++ = 0;
			nModifier |= bindings_parseModifier(pszMouse);
		} else {
			char* pszBrace = strchr(pszMouse, '(');
			if (pszBrace) {
				*pszBrace++ = 0;
				nClicks = atoi(pszBrace);
			}
			pTarget->ab_binding.mousebind.shift = nModifier;
			pTarget->ab_binding.mousebind.nclicks = nClicks;
			if (strcmp(pszMouse, "MLeft") == 0) {
				pTarget->ab_binding.mousebind.button = MBUT_L;
			} else if (strcmp(pszMouse, "MRight") == 0) {
				pTarget->ab_binding.mousebind.button = MBUT_R;
			} else if (strcmp(pszMouse, "MMiddle") == 0) {
				pTarget->ab_binding.mousebind.button = MBUT_M;
			} else if (strcmp(pszMouse, "MLeft-MRight") == 0) {
				pTarget->ab_binding.mousebind.button = MBUT_RL;
			}
			break;
		}
		pszMouse = pszNext;
	}
	return 1;
}

static JSON_MAPPING_RULE _keybindRules[] = {
	{	RT_CHAR_ARRAY, "context", offsetof(LOCAL_ACTION_BINDING, ab_context), sizeof(((LOCAL_ACTION_BINDING*)NULL)->ab_context)},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = bindings_parseKeyCommand}},
	{	RT_STRING_CALLBACK, "key", 0, .r_descriptor = {.r_t_callback = bindings_parseAndAssignKeycode}},
	{	RT_END }
};

static LOCAL_ACTION_BINDING* bindings_createActionBinding() {
	return (LOCAL_ACTION_BINDING*)calloc(1, sizeof(LOCAL_ACTION_BINDING));
}

static JSON_MAPPING_RULE _mousebindRules[] = {
	{	RT_CHAR_ARRAY, "context", offsetof(LOCAL_ACTION_BINDING, ab_context), sizeof(((LOCAL_ACTION_BINDING*)NULL)->ab_context)},
	{	RT_ALLOC_STRING, "message", offsetof(LOCAL_ACTION_BINDING, ab_binding.mousebind.msg.bt_text), 0},
	{	RT_INTEGER, "message-id", offsetof(LOCAL_ACTION_BINDING, ab_binding.mousebind.msg.bt_resourceId), 0},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = bindings_parseMouseCommand}},
	{	RT_STRING_CALLBACK, "mouse", 0, .r_descriptor = {.r_t_callback = bindings_parseMouse}},
	{	RT_END }
};

static JSON_MAPPING_RULE _menuRules[] = {
	{	RT_CHAR_ARRAY, "context", offsetof(LOCAL_ACTION_BINDING, ab_context), sizeof(((LOCAL_ACTION_BINDING*)NULL)->ab_context)},
	{	RT_CHAR_ARRAY, "anchor", offsetof(LOCAL_ACTION_BINDING, ab_anchor), sizeof(((LOCAL_ACTION_BINDING*)NULL)->ab_anchor)},
	{	RT_CHAR_ARRAY, "insert-at", offsetof(LOCAL_ACTION_BINDING, ab_reference), sizeof(((LOCAL_ACTION_BINDING*)NULL)->ab_reference)},
	{	RT_ALLOC_STRING, "label", offsetof(LOCAL_ACTION_BINDING, ab_binding.subMenu.mid_label.bt_text), 0},
	{	RT_INTEGER, "label-id", offsetof(LOCAL_ACTION_BINDING, ab_binding.subMenu.mid_label.bt_resourceId), 0},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = bindings_parseContextmenuCommand}},
	{	RT_FLAG, "separator", offsetof(LOCAL_ACTION_BINDING, ab_binding.subMenu.mid_isSeparator), 1},
	{	RT_FLAG, "history-menu", offsetof(LOCAL_ACTION_BINDING, ab_binding.subMenu.mid_isHistoryMenu), 1},
	{	RT_FLAG, "macro-menu", offsetof(LOCAL_ACTION_BINDING, ab_binding.subMenu.mid_isMacroMenu), 1},
	{	RT_OBJECT_LIST, "sub-menu", offsetof(LOCAL_ACTION_BINDING, ab_children),
		{.r_t_arrayDescriptor = {bindings_createActionBinding, _menuRules}}
	},
	{	RT_END }
};

static int bindings_parseToolbarIconName(LOCAL_ACTION_BINDING* pTarget, const char* pszIconName) {
	wchar_t wIcon = faicon_codeForName(pszIconName);
	pTarget->ab_binding.toolbarButtonBinding.tbb_faIcon = wIcon ? wIcon : FA_ICON_QUESTION;
	return 1;
}

static JSON_MAPPING_RULE _tbbRules[] = {
	{	RT_CHAR_ARRAY, "context", offsetof(LOCAL_ACTION_BINDING, ab_context), sizeof(((LOCAL_ACTION_BINDING*)NULL)->ab_context)},
	{	RT_CHAR_ARRAY, "anchor", offsetof(LOCAL_ACTION_BINDING, ab_anchor), sizeof(((LOCAL_ACTION_BINDING*)NULL)->ab_anchor)},
	{	RT_CHAR_ARRAY, "insert-at", offsetof(LOCAL_ACTION_BINDING, ab_reference), sizeof(((LOCAL_ACTION_BINDING*)NULL)->ab_reference)},
	{	RT_ALLOC_STRING, "label", offsetof(LOCAL_ACTION_BINDING, ab_binding.toolbarButtonBinding.tbb_label.bt_text), 0},
	{	RT_INTEGER, "label-id", offsetof(LOCAL_ACTION_BINDING, ab_binding.toolbarButtonBinding.tbb_label.bt_resourceId), 0},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = bindings_parseToolbarCommand}},
	{	RT_FLAG, "separator", offsetof(LOCAL_ACTION_BINDING, ab_binding.toolbarButtonBinding.tbb_isSeparator), 1},
	{	RT_STRING_CALLBACK, "icon", 0, .r_descriptor = {.r_t_callback = bindings_parseToolbarIconName}},
	{	RT_END }
};

static JSON_MAPPING_RULE _jsonBindingsRules[] = {
	{	RT_OBJECT_LIST, "key-bindings", offsetof(JSON_BINDINGS, keys), 
		{.r_t_arrayDescriptor = {bindings_createActionBinding, _keybindRules}}
	},
	{	RT_OBJECT_LIST, "mouse-bindings", offsetof(JSON_BINDINGS, mouse),
		{.r_t_arrayDescriptor = {bindings_createActionBinding, _mousebindRules}}
	},
	{	RT_OBJECT_LIST, "context-menu", offsetof(JSON_BINDINGS, subMenu),
		{.r_t_arrayDescriptor = {bindings_createActionBinding, _menuRules}}
	},
	{	RT_OBJECT_LIST, "menu", offsetof(JSON_BINDINGS, menu),
		{.r_t_arrayDescriptor = {bindings_createActionBinding, _menuRules}}
	},
	{	RT_OBJECT_LIST, "toolbar", offsetof(JSON_BINDINGS, toolbarButtonBinding),
		{.r_t_arrayDescriptor = {bindings_createActionBinding, _tbbRules}}
	},
	{	RT_END }
};

static unsigned int bindings_hashCodeForKey(intptr_t p) {
	return (unsigned int)(long long)p;
}

static int bindings_compareKeyCodes(intptr_t keycode1, intptr_t keycode2) {
	return keycode1 == keycode2 ? 0 : 1;
}

/*
 * Lookup the bindings for a named context. If the bindings do not exist yet, they
 * are lazily created.
 */
static ACTION_BINDINGS* bindings_lookupByContext(const char* pCtx) {
	ACTION_BINDINGS* pBindings;
	if (!pCtx || !*pCtx) {
		pCtx = DEFAULT_ACTION_CONTEXT;
		pBindings = _actionBindings;
	}
	else {
		pBindings = ll_find((LINKED_LIST*)_actionBindings, (char*)pCtx);
	}
	if (!pBindings) {
		pBindings = ll_append(&_actionBindings, sizeof(ACTION_BINDINGS));
		strcpy(pBindings->ab_context, pCtx);
		pBindings->ab_keyBindingTable = hashmap_create(37, bindings_hashCodeForKey, bindings_compareKeyCodes);
	}
	return pBindings;
}

static int bindings_destroyMouseBinding(LOCAL_ACTION_BINDING* pBind) {
	free((char*)pBind->ab_binding.mousebind.msg.bt_text);
	return 1;
}

static int bindings_destroyMenu(LOCAL_ACTION_BINDING* pBind) {
	free((char*)pBind->ab_binding.subMenu.mid_label.bt_text);
	ll_destroy((LINKED_LIST**)&pBind->ab_children, bindings_destroyMenu);
	return 1;
}

static int bindings_destroyTBB(LOCAL_ACTION_BINDING* pBind) {
	free((char*)pBind->ab_binding.toolbarButtonBinding.tbb_label.bt_text);
	return 1;
}

static int bindings_destroyActionBindings(ACTION_BINDINGS* pBindings) {
	hashmap_destroy(pBindings->ab_keyBindingTable, NULL);
	ll_destroy((LINKED_LIST**) & pBindings->ab_mouseBindings, bindings_destroyMouseBinding);
	return 1;
}

/*
 * Destroy all action bindings loaded.
 */
void bindings_destroy() {
	ll_destroy((LINKED_LIST**)&_toolbar, bindings_destroyTBB);
	ll_destroy((LINKED_LIST**)&_menu, bindings_destroyMenu);
	ll_destroy((LINKED_LIST**)&_contextMenu, bindings_destroyMenu);
	ll_destroy((LINKED_LIST**)&_actionBindings, bindings_destroyActionBindings);
}

/*
 * Load a grammar definition file from the PKS_SYS directory.
 */
static int bindings_loadFromFile(const char* pszFilename) {
	JSON_BINDINGS definitions;

	memset(&definitions, 0, sizeof definitions);
	if (json_parse(pszFilename, &definitions, _jsonBindingsRules)) {
		LOCAL_ACTION_BINDING* pB = definitions.keys;
		while (pB) {
			ACTION_BINDINGS* pBindings = bindings_lookupByContext(pB->ab_context);
			intptr_t lValue = MACROREF_TO_INTPTR(pB->ab_binding.keybind.macref);
			hashmap_put(pBindings->ab_keyBindingTable, pB->ab_binding.keybind.keycode, lValue);
			pB = pB->ab_next;
		}
		pB = definitions.mouse;
		while (pB) {
			ACTION_BINDINGS* pBindings = bindings_lookupByContext(pB->ab_context);
			LOCAL_ACTION_BINDING* pBNext = pB->ab_next;
			pB->ab_next = pBindings->ab_mouseBindings;
			pBindings->ab_mouseBindings = pB;
			pB = pBNext;
		}
		_contextMenu = definitions.subMenu;
		_toolbar = definitions.toolbarButtonBinding;
		_menu = definitions.menu;
		ll_destroy(&definitions.keys, NULL);
		return 1;
	}
	return 0;
}

/*
 * Returns the command to be executed, when a specific keycode was pressed in a given action context. 
 */
MACROREF bindings_getKeyBinding(KEYCODE keycode, const char* pszActionContext) {
	ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
	intptr_t tResult = hashmap_get(pBindings->ab_keyBindingTable, (intptr_t) keycode);
	if (!tResult && pszActionContext) {
		pBindings = bindings_lookupByContext(NULL);
		tResult = hashmap_get(pBindings->ab_keyBindingTable, (intptr_t)keycode);
	}
	return INTPTR_TO_MACROREF(tResult);
}

/*------------------------------------------------------------
 * bindings_getMouseBinding()
 */
MOUSE_EVENT_BINDING* bindings_getMouseBinding(int nButton, int nShift, int nClicks, const char* pszActionContext) {
	LOCAL_ACTION_BINDING* mp;

	while(TRUE) {
		ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
		mp = pBindings->ab_mouseBindings;
		while (mp) {
			if (mp->ab_binding.mousebind.button == nButton &&
				mp->ab_binding.mousebind.shift == nShift &&
				mp->ab_binding.mousebind.nclicks == nClicks) {
				return &mp->ab_binding.mousebind;
			}
			mp = mp->ab_next;
		}
		if (pszActionContext == NULL) {
			break;
		}
		pszActionContext = NULL;
	}
	return (MOUSE_EVENT_BINDING*)0;
}

/*
 * Loads the default action bindings (keys and mouse) for PKS-Edit from the PKS_SYS directory. 
 */
void bindings_loadActionBindings() {
	bindings_loadFromFile("pksactionbindings.json");
}

/*
 * Process the bindings for one keybind.
 */
static void* _callbackParam;
static int bindings_processKeyBinding(intptr_t k, intptr_t v, void* pParam) {
	int (*callback)(KEY_BINDING * pBinding, void* pParam) = pParam;
	KEY_BINDING kb;
	kb.keycode = (int)k;
	kb.macref = INTPTR_TO_MACROREF(v);
	return callback(&kb, _callbackParam);
}

/*
 * Iterate all key bindings and invoke a callback on every keybinding defined for one action context. 
 */
void bindings_forAllKeyBindingsDo(const char* pszActionContext, int (*callback)(KEY_BINDING* pBinding, void* pParam), void* pParam) {
	_callbackParam = pParam;
	ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
	hashmap_forEachEntry(pBindings->ab_keyBindingTable, bindings_processKeyBinding, callback);
	if (pszActionContext && strcmp(pszActionContext, pBindings->ab_context) != 0) {
		pBindings = bindings_lookupByContext(NULL);
		hashmap_forEachEntry(pBindings->ab_keyBindingTable, bindings_processKeyBinding, callback);
	}
}

/*
 * Iterate all mouse bindings and invoke a callback on every mousebinding defined for one action context.
 */
void bindings_forAllMouseBindingsDo(const char* pszActionContext, int (*callback)(MOUSE_EVENT_BINDING* pBinding, void* pParam), void* pParam) {
	while (TRUE) {
		ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
		LOCAL_ACTION_BINDING* mp = pBindings->ab_mouseBindings;
		while (mp) {
			if (!callback(&mp->ab_binding.mousebind, pParam)) {
				return;
			}
			mp = mp->ab_next;
		}
		if (pszActionContext && strcmp(pszActionContext, pBindings->ab_context) != 0) {
			pszActionContext = NULL;
		} else {
			break;
		}
	}
}

static MACROREF _searchMacroRef;

static int bindings_matchKeybinding(KEY_BINDING* pBinding, void* pParam) {
	if (pBinding->macref.index == _searchMacroRef.index && pBinding->macref.typ == _searchMacroRef.typ) {
		*((KEYCODE*)pParam) = pBinding->keycode;
		return 0;
	}
	return 1;
}

/*
 * Find the 1st key bound to a given macro.
 */
KEYCODE bindings_findBoundKey(const char* pszActionContext, MACROREF macro) {
	KEYCODE foundKeyCode;

	_searchMacroRef = macro;
	foundKeyCode = K_DELETED;
	bindings_forAllKeyBindingsDo(pszActionContext, bindings_matchKeybinding, &foundKeyCode);
	return foundKeyCode;
}

/*------------------------------------------------------------
 * bindings_bindKey()
 * bind a command to a keycode dynamically in a given context.
 */
int bindings_bindKey(KEYCODE key, MACROREF macro, const char* pszActionContext) {
	ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
	hashmap_put(pBindings->ab_keyBindingTable, (intptr_t)key, MACROREF_TO_INTPTR(macro));
	return 1;
}

/*
 * Returns a context menu - recursively callable for nested sub-menus. 
 */
static MENU_ITEM_DEFINITION* bindings_getSubmenuFrom(const char* pszActionContext, LOCAL_ACTION_BINDING* pBinding) {
	MENU_ITEM_DEFINITION* pHead = NULL;
	MENU_ITEM_DEFINITION* pCurrent = NULL;
	while (pBinding) {
		if (strcmp(pszActionContext, pBinding->ab_context) == 0 || !pBinding->ab_context[0] || strcmp(DEFAULT_ACTION_CONTEXT, pBinding->ab_context) == 0) {
			MENU_ITEM_DEFINITION* pNested = NULL;
			if (pBinding->ab_children) {
				pNested = bindings_getSubmenuFrom(pszActionContext, pBinding->ab_children);
				if (!pNested) {
					pBinding = pBinding->ab_next;
					continue;
				}
			}
			if (!pHead) {
				pHead = &pBinding->ab_binding.subMenu;
				pCurrent = pHead;
			}
			else {
				pCurrent->mid_next = &pBinding->ab_binding.subMenu;
				pCurrent = pCurrent->mid_next;
				pCurrent->mid_next = NULL;
			}
			pCurrent->mid_children = pNested;
		}
		pBinding = pBinding->ab_next;
	}
	return pHead;
}

/*
 * Returns a linked list of context menu entries for a given action context.
 */
MENU_ITEM_DEFINITION* bindings_getContextMenuFor(const char* pszActionContext) {
	LOCAL_ACTION_BINDING* pBinding = _contextMenu;

	if (!pszActionContext) {
		pszActionContext = DEFAULT_ACTION_CONTEXT;
	}
	return bindings_getSubmenuFrom(pszActionContext, pBinding);
}

/*
 * Returns a linked list of main menu entries for a given action context.
 */
MENU_ITEM_DEFINITION* bindings_getMenuBarFor(const char* pszActionContext) {
	LOCAL_ACTION_BINDING* pBinding = _menu;

	if (!pszActionContext) {
		pszActionContext = DEFAULT_ACTION_CONTEXT;
	}
	return bindings_getSubmenuFrom(pszActionContext, pBinding);
}

/*
 * Returns a linked list of main menu entries for a popup menu of the application menu bar with the given
 * menu handle and for an action context.
 */
MENU_ITEM_DEFINITION* bindings_getMenuBarPopupDefinitionFor(const char* pszActionContext, HMENU hMenu) {
	LOCAL_ACTION_BINDING* pBinding = _menu;

	if (!pszActionContext) {
		pszActionContext = DEFAULT_ACTION_CONTEXT;
	}
	while (pBinding) {
		if (pBinding->ab_binding.subMenu.mid_menuHandle == hMenu) {
			return bindings_getSubmenuFrom(pszActionContext, pBinding->ab_children);
		}
		pBinding = pBinding->ab_next;
	}
	return NULL;
}

/*
 * Returns a linked list of toolbar button bindings for an action context (currently ignored - toolbar is never dynamic).
 */
TOOLBAR_BUTTON_BINDING* bindings_getToolbarBindingsFor(const char* pszActionContext) {
	LOCAL_ACTION_BINDING* pBinding = _toolbar;
	TOOLBAR_BUTTON_BINDING* pFirst = NULL;
	TOOLBAR_BUTTON_BINDING* pCurrent = NULL;
	TOOLBAR_BUTTON_BINDING* pTb;
	
	while (pBinding) {
		pTb = &pBinding->ab_binding.toolbarButtonBinding;
		if (pCurrent == NULL) {
			pFirst = pCurrent = pTb;
		}
		else {
			pCurrent->tbb_next = pTb;
			pCurrent = pTb;
		}
		pCurrent->tbb_next = NULL;
		pBinding = pBinding->ab_next;
	}
	return pFirst;
}


/*
 * Delete a key binding.
 */
int bindings_deleteKeyBinding(KEYCODE key, const char* pszActionContext) {
	ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
	hashmap_put(pBindings->ab_keyBindingTable, (intptr_t)key, 0);
	return 1;
}

/*
 * Delete all key bindings for a given macro ref.
 */
int bindings_deleteKeyBindingsForMacroRef(MACROREF aMacroRef) {
	return 0;
}

/*
 * Returns the text to use for an aspect of an action binding, which describes a text to
 * display in an action trigger.
 */
const char* bindings_getBoundText(BOUND_TEXT* pText) {
	if (pText->bt_text) {
		return pText->bt_text;
	}
	if (pText->bt_resourceId) {
		return dlg_getResourceString(pText->bt_resourceId);
	}
	return NULL;
}

/*---------------------------------
 * bindings_addModifierKeysToKeycode()
 * Add the modifier key bits depending on whether
 * Shift, Control or Alt was pressed together with
 * the key passed as an argument.
 */
KEYCODE bindings_addModifierKeysToKeycode(KEYCODE key)
{
	if (GetKeyState(VK_SHIFT) < 0)
		key |= K_SHIFT;
	if (GetKeyState(VK_CONTROL) < 0)
		key |= K_CONTROL;
	if (GetKeyState(VK_MENU) < 0)
		key |= K_ALTERNATE;
	if (GetKeyState(VK_LWIN) < 0)
		key |= K_WINDOWS;
	return key;
}

