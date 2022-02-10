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
#include "edfuncs.h"
#include "linkedlist.h"
#include "resource.h"
#include "jsonparser.h"
#include "hashmap.h"
#include "arraylist.h"
#include "fontawesome.h"
#include "xdialog.h"

#define MAX_CONTEXT_NAME_LEN		32
#define MAX_ID_LEN					10

typedef struct tagACTION_BINDING {
	struct tagACTION_BINDING* ab_next;		// next action binding 
	struct tagACTION_BINDING* ab_children;	// for nested actions (e.g. menus) - this is the possibly null list of children.
	char ab_context[MAX_CONTEXT_NAME_LEN];	// context name
	char ab_anchor[MAX_ID_LEN];				// anchor name
	char ab_reference[MAX_ID_LEN];			// reference for extending action bindings referring to an anchor
	union {
		KEY_BINDING		keybind;
		MOUSE_EVENT_BINDING	mousebind;
		MENU_ITEM_DEFINITION subMenu;
		TOOLBAR_BUTTON_BINDING toolbarButtonBinding;
	} ab_binding;
} ACTION_BINDING;

typedef struct tagACTION_BINDINGS {
	struct tagACTION_BINDINGS* ab_next;		// next keybinding context
	char ab_context[MAX_CONTEXT_NAME_LEN];	// action bindings context name
	HASHMAP* ab_keyBindingTable;			// keybindings: maps KEYCODES to MACROREFs 
	ACTION_BINDING* ab_mouseBindings;		// the defined mouse bindings
} ACTION_BINDINGS;

static ACTION_BINDINGS* _actionBindings;
static ACTION_BINDING*  _contextMenu;		// the context menu definitions
static ACTION_BINDING*  _menu;				// the main menu definitions
static ACTION_BINDING*  _toolbar;			// the toolbar definitions

typedef struct tagJSON_BINDINGS {
	ACTION_BINDING* keys;
	ACTION_BINDING* mouse;
	ACTION_BINDING* subMenu;
	ACTION_BINDING* menu;
	ACTION_BINDING* toolbarButtonBinding;
} JSON_BINDINGS;

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

static int mouse_parseCommand(ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.mousebind.macref, pszCommand);
}

static int key_parseCommand(ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.keybind.macref, pszCommand);
}

static int contextmenu_parseCommand(ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.subMenu.mid_command, pszCommand);
}

static int toolbar_parseCommand(ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.toolbarButtonBinding.tbb_macref, pszCommand);
}

static int key_parseKeycode(ACTION_BINDING* pTarget, const char* pszKeycode) {
	pTarget->ab_binding.keybind.keycode = macro_parseKeycode(pszKeycode);
	return 1;
}

static int binding_parseMouse(ACTION_BINDING* pTarget, const char* pszMouse) {
	int nModifier = 0;
	int nClicks = 1;
	while (pszMouse) {
		char* pszNext = strchr(pszMouse, '+');
		if (pszNext) {
			*pszNext++ = 0;
			nModifier |= macro_parseModifier(pszMouse);
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
	{	RT_CHAR_ARRAY, "context", offsetof(ACTION_BINDING, ab_context), sizeof(((ACTION_BINDING*)NULL)->ab_context)},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = key_parseCommand}},
	{	RT_STRING_CALLBACK, "key", 0, .r_descriptor = {.r_t_callback = key_parseKeycode}},
	{	RT_END }
};

static ACTION_BINDING* binding_createActionBinding() {
	return (ACTION_BINDING*)calloc(1, sizeof(ACTION_BINDING));
}

static JSON_MAPPING_RULE _mousebindRules[] = {
	{	RT_CHAR_ARRAY, "context", offsetof(ACTION_BINDING, ab_context), sizeof(((ACTION_BINDING*)NULL)->ab_context)},
	{	RT_ALLOC_STRING, "message", offsetof(ACTION_BINDING, ab_binding.mousebind.msg.bt_text), 0},
	{	RT_INTEGER, "message-id", offsetof(ACTION_BINDING, ab_binding.mousebind.msg.bt_resourceId), 0},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = mouse_parseCommand}},
	{	RT_STRING_CALLBACK, "mouse", 0, .r_descriptor = {.r_t_callback = binding_parseMouse}},
	{	RT_END }
};

static JSON_MAPPING_RULE _menuRules[] = {
	{	RT_CHAR_ARRAY, "context", offsetof(ACTION_BINDING, ab_context), sizeof(((ACTION_BINDING*)NULL)->ab_context)},
	{	RT_CHAR_ARRAY, "anchor", offsetof(ACTION_BINDING, ab_anchor), sizeof(((ACTION_BINDING*)NULL)->ab_anchor)},
	{	RT_CHAR_ARRAY, "insert-at", offsetof(ACTION_BINDING, ab_reference), sizeof(((ACTION_BINDING*)NULL)->ab_reference)},
	{	RT_ALLOC_STRING, "label", offsetof(ACTION_BINDING, ab_binding.subMenu.mid_label.bt_text), 0},
	{	RT_INTEGER, "label-id", offsetof(ACTION_BINDING, ab_binding.subMenu.mid_label.bt_resourceId), 0},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = contextmenu_parseCommand}},
	{	RT_FLAG, "separator", offsetof(ACTION_BINDING, ab_binding.subMenu.mid_isSeparator), 1},
	{	RT_FLAG, "history-menu", offsetof(ACTION_BINDING, ab_binding.subMenu.mid_isHistoryMenu), 1},
	{	RT_OBJECT_LIST, "sub-menu", offsetof(ACTION_BINDING, ab_children),
		{.r_t_arrayDescriptor = {binding_createActionBinding, _menuRules}}
	},
	{	RT_END }
};

static int tbb_parseIconName(ACTION_BINDING* pTarget, const char* pszIconName) {
	wchar_t wIcon = faicon_codeForName(pszIconName);
	pTarget->ab_binding.toolbarButtonBinding.tbb_faIcon = wIcon ? wIcon : FA_ICON_QUESTION;
	return 1;
}

static JSON_MAPPING_RULE _tbbRules[] = {
	{	RT_CHAR_ARRAY, "context", offsetof(ACTION_BINDING, ab_context), sizeof(((ACTION_BINDING*)NULL)->ab_context)},
	{	RT_CHAR_ARRAY, "anchor", offsetof(ACTION_BINDING, ab_anchor), sizeof(((ACTION_BINDING*)NULL)->ab_anchor)},
	{	RT_CHAR_ARRAY, "insert-at", offsetof(ACTION_BINDING, ab_reference), sizeof(((ACTION_BINDING*)NULL)->ab_reference)},
	{	RT_ALLOC_STRING, "label", offsetof(ACTION_BINDING, ab_binding.toolbarButtonBinding.tbb_label.bt_text), 0},
	{	RT_INTEGER, "label-id", offsetof(ACTION_BINDING, ab_binding.toolbarButtonBinding.tbb_label.bt_resourceId), 0},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = toolbar_parseCommand}},
	{	RT_FLAG, "separator", offsetof(ACTION_BINDING, ab_binding.toolbarButtonBinding.tbb_isSeparator), 1},
	{	RT_STRING_CALLBACK, "icon", 0, .r_descriptor = {.r_t_callback = tbb_parseIconName}},
	{	RT_END }
};

static JSON_MAPPING_RULE _jsonBindingsRules[] = {
	{	RT_OBJECT_LIST, "key-bindings", offsetof(JSON_BINDINGS, keys), 
		{.r_t_arrayDescriptor = {binding_createActionBinding, _keybindRules}}
	},
	{	RT_OBJECT_LIST, "mouse-bindings", offsetof(JSON_BINDINGS, mouse),
		{.r_t_arrayDescriptor = {binding_createActionBinding, _mousebindRules}}
	},
	{	RT_OBJECT_LIST, "context-menu", offsetof(JSON_BINDINGS, subMenu),
		{.r_t_arrayDescriptor = {binding_createActionBinding, _menuRules}}
	},
	{	RT_OBJECT_LIST, "menu", offsetof(JSON_BINDINGS, menu),
		{.r_t_arrayDescriptor = {binding_createActionBinding, _menuRules}}
	},
	{	RT_OBJECT_LIST, "toolbar", offsetof(JSON_BINDINGS, toolbarButtonBinding),
		{.r_t_arrayDescriptor = {binding_createActionBinding, _tbbRules}}
	},
	{	RT_END }
};

static unsigned int key_hashCode(intptr_t p) {
	return (unsigned int)(long long)p;
}

static int key_compare(intptr_t keycode1, intptr_t keycode2) {
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
		pBindings->ab_keyBindingTable = hashmap_create(37, key_hashCode, key_compare);
	}
	return pBindings;
}

static int bindings_destroyMouseBinding(ACTION_BINDING* pBind) {
	free((char*)pBind->ab_binding.mousebind.msg.bt_text);
	return 1;
}

static int bindings_destroyMenu(ACTION_BINDING* pBind) {
	free((char*)pBind->ab_binding.subMenu.mid_label.bt_text);
	ll_destroy((LINKED_LIST**)&pBind->ab_children, bindings_destroyMenu);
	return 1;
}

static int bindings_destroyTBB(ACTION_BINDING* pBind) {
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
		ACTION_BINDING* pB = definitions.keys;
		while (pB) {
			ACTION_BINDINGS* pBindings = bindings_lookupByContext(pB->ab_context);
			intptr_t lValue = MACROREF_TO_INTPTR(pB->ab_binding.keybind.macref);
			hashmap_put(pBindings->ab_keyBindingTable, pB->ab_binding.keybind.keycode, lValue);
			pB = pB->ab_next;
		}
		pB = definitions.mouse;
		while (pB) {
			ACTION_BINDINGS* pBindings = bindings_lookupByContext(pB->ab_context);
			ACTION_BINDING* pBNext = pB->ab_next;
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
 * mouse_getMouseBind()
 */
MOUSE_EVENT_BINDING* mouse_getMouseBind(int nButton, int nShift, int nClicks, const char* pszActionContext) {
	ACTION_BINDING* mp;

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
void key_loadBindings() {
	bindings_loadFromFile("pksactionbindings.json");
}

/*
 * Process the bindings for one keybind.
 */
static void* _callbackParam;
static int key_processBinding(intptr_t k, intptr_t v, void* pParam) {
	int (*callback)(KEY_BINDING * pBinding, void* pParam) = pParam;
	KEY_BINDING kb;
	kb.keycode = (int)k;
	kb.macref = INTPTR_TO_MACROREF(v);
	return callback(&kb, _callbackParam);
}

/*
 * Iterate all key bindings and invoke a callback on every keybinding defined for one action context. 
 */
void key_bindingsDo(const char* pszActionContext, int (*callback)(KEY_BINDING* pBinding, void* pParam), void* pParam) {
	_callbackParam = pParam;
	ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
	hashmap_forEachEntry(pBindings->ab_keyBindingTable, key_processBinding, callback);
	if (pszActionContext && strcmp(pszActionContext, pBindings->ab_context) != 0) {
		pBindings = bindings_lookupByContext(NULL);
		hashmap_forEachEntry(pBindings->ab_keyBindingTable, key_processBinding, callback);
	}
}

/*
 * Iterate all mouse bindings and invoke a callback on every mousebinding defined for one action context.
 */
void mouse_bindingsDo(const char* pszActionContext, int (*callback)(MOUSE_EVENT_BINDING* pBinding, void* pParam), void* pParam) {
	while (TRUE) {
		ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
		ACTION_BINDING* mp = pBindings->ab_mouseBindings;
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

static int key_matchMacro(KEY_BINDING* pBinding, void* pParam) {
	if (pBinding->macref.index == _searchMacroRef.index && pBinding->macref.typ == _searchMacroRef.typ) {
		*((KEYCODE*)pParam) = pBinding->keycode;
		return 0;
	}
	return 1;
}

/*
 * Find the 1st key bound to a given macro.
 */
KEYCODE macro_findKey(const char* pszActionContext, MACROREF macro) {
	KEYCODE foundKeyCode;

	_searchMacroRef = macro;
	foundKeyCode = K_DELETED;
	key_bindingsDo(pszActionContext, key_matchMacro, &foundKeyCode);
	return foundKeyCode;
}

/*------------------------------------------------------------
 * macro_bindKey()
 * bind a command to a keycode dynamically in a given context.
 */
int macro_bindKey(KEYCODE key, MACROREF macro, const char* pszActionContext) {
	ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
	hashmap_put(pBindings->ab_keyBindingTable, (intptr_t)key, MACROREF_TO_INTPTR(macro));
	return 1;
}

/*
 * Returns a context menu - recursively callable for nested sub-menus. 
 */
static MENU_ITEM_DEFINITION* submenu_getFrom(const char* pszActionContext, ACTION_BINDING* pBinding) {
	MENU_ITEM_DEFINITION* pHead = NULL;
	MENU_ITEM_DEFINITION* pCurrent = NULL;
	while (pBinding) {
		if (strcmp(pszActionContext, pBinding->ab_context) == 0 || !pBinding->ab_context[0] || strcmp(DEFAULT_ACTION_CONTEXT, pBinding->ab_context) == 0) {
			MENU_ITEM_DEFINITION* pNested = NULL;
			if (pBinding->ab_children) {
				pNested = submenu_getFrom(pszActionContext, pBinding->ab_children);
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
MENU_ITEM_DEFINITION* binding_getContextMenuFor(const char* pszActionContext) {
	ACTION_BINDING* pBinding = _contextMenu;

	if (!pszActionContext) {
		pszActionContext = DEFAULT_ACTION_CONTEXT;
	}
	return submenu_getFrom(pszActionContext, pBinding);
}

/*
 * Returns a linked list of main menu entries for a given action context.
 */
MENU_ITEM_DEFINITION* binding_getMenuBarFor(const char* pszActionContext) {
	ACTION_BINDING* pBinding = _menu;

	if (!pszActionContext) {
		pszActionContext = DEFAULT_ACTION_CONTEXT;
	}
	return submenu_getFrom(pszActionContext, pBinding);
}

/*
 * Returns a linked list of main menu entries for a popup menu of the application menu bar with the given
 * menu handle and for an action context.
 */
MENU_ITEM_DEFINITION* binding_getMenuBarPopupDefinitionFor(const char* pszActionContext, HMENU hMenu) {
	ACTION_BINDING* pBinding = _menu;

	if (!pszActionContext) {
		pszActionContext = DEFAULT_ACTION_CONTEXT;
	}
	while (pBinding) {
		if (pBinding->ab_binding.subMenu.mid_menuHandle == hMenu) {
			return submenu_getFrom(pszActionContext, pBinding->ab_children);
		}
		pBinding = pBinding->ab_next;
	}
	return NULL;
}

/*
 * Returns a linked list of toolbar button bindings for an action context (currently ignored - toolbar is never dynamic).
 */
TOOLBAR_BUTTON_BINDING* binding_getToolbarBindingsFor(const char* pszActionContext) {
	ACTION_BINDING* pBinding = _toolbar;
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
int macro_deleteKeyBinding(KEYCODE key, const char* pszActionContext) {
	ACTION_BINDINGS* pBindings = bindings_lookupByContext(pszActionContext);
	hashmap_put(pBindings->ab_keyBindingTable, (intptr_t)key, 0);
	return 1;
}

/*
 * Delete all key bindings for a given macro ref.
 */
int macro_deleteKeyBindingsForMacroRef(MACROREF aMacroRef) {
	return 0;
}

/*
 * Returns the text to use for an aspect of an action binding, which describes a text to
 * display in an action trigger.
 */
const char* binding_getBoundText(BOUND_TEXT* pText) {
	if (pText->bt_text) {
		return pText->bt_text;
	}
	if (pText->bt_resourceId) {
		return dlg_getResourceString(pText->bt_resourceId);
	}
	return NULL;
}
