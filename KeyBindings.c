/*
 * KeyBindings.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: translate key names 2 scan code and vc.vs.
 *		  maintain key-function bindings in a hash tab
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

#define MACROREF_TO_INTPTR(m)	(intptr_t)(((unsigned long)m.typ<<16) + (unsigned long)m.index)
#define INTPTR_TO_MACROREF(m)	(MACROREF){.typ = (unsigned char)((unsigned long)m >> 16), .index = (unsigned char)((unsigned long)m&0xFFFF)}

#define MAX_CONTEXT_NAME_LEN		32

typedef struct tagACTION_BINDING {
	struct tagACTION_BINDING* ab_next;		// next action binding context
	char ab_context[MAX_CONTEXT_NAME_LEN];	// possible not null context
	union {
		KEYBIND		keybind;
		MOUSEBIND	mousebind;
	} ab_binding;
} ACTION_BINDING;

typedef struct tagACTION_BINDINGS {
	struct tagACTION_BINDINGS* ab_next;		// next keybinding context
	char ab_context[MAX_CONTEXT_NAME_LEN];	// action bindings context name
	HASHMAP* ab_keyBindingTable;			// keybindings: maps KEYCODES to MACROREFs 
	ACTION_BINDING* ab_mouseBindings;		// the defined mouse bindings
} ACTION_BINDINGS;

static ACTION_BINDINGS* _actionBindings;

typedef struct tagJSON_BINDINGS {
	ACTION_BINDING* keys;
	ACTION_BINDING* mouse;
} JSON_BINDINGS;

static int bindings_parseCommandMP(MACROREF* mp, const char* pszCommand) {
	BOOL bCommand = (pszCommand[0] == '@');
	mp->index = bCommand ? macro_getCmdIndexByName(pszCommand + 1) : macro_getInternalIndexByName(pszCommand);
	mp->typ = bCommand ? CMD_CMDSEQ : CMD_MACRO;
	return 1;
}


static int mouse_parseCommand(ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.mousebind.macref, pszCommand);
}

static int key_parseCommand(ACTION_BINDING* pTarget, const char* pszCommand) {
	return bindings_parseCommandMP(&pTarget->ab_binding.keybind.macref, pszCommand);
}

extern KEYCODE key2code(const unsigned char* k, int control);

static int key_parseModifier(const char* pszKeycode) {
	if (strcmp(pszKeycode, "Shift") == 0) {
		return K_SHIFT;
	}
	else if (strcmp(pszKeycode, "Ctrl") == 0 || strcmp(pszKeycode, "Control") == 0) {
		return K_CONTROL;
	}
	else if (strcmp(pszKeycode, "Alt") == 0) {
		return K_ALTERNATE;
	}
	else if (strcmp(pszKeycode, "Selected") == 0) {
		return K_HAS_SELECTION;
	}
	return 0;
}

static int key_parseKeycode(ACTION_BINDING* pTarget, const char* pszKeycode) {
	int nModifier = 0;
	while (pszKeycode) {
		char* pszNext = strchr(pszKeycode, '+');
		if (pszNext) {
			*pszNext++ = 0;
			nModifier |= key_parseModifier(pszKeycode);
		}
		else {
			pTarget->ab_binding.keybind.keycode = key2code(pszKeycode, nModifier);
			break;
		}
		pszKeycode = pszNext;
	}
	return 1;
}

static int binding_parseMouse(ACTION_BINDING* pTarget, const char* pszMouse) {
	int nModifier = 0;
	int nClicks = 1;
	while (pszMouse) {
		char* pszNext = strchr(pszMouse, '+');
		if (pszNext) {
			*pszNext++ = 0;
			nModifier |= key_parseModifier(pszMouse);
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

static JSON_MAPPING_RULE _mousebindRules[] = {
	{	RT_CHAR_ARRAY, "context", offsetof(ACTION_BINDING, ab_context), sizeof(((ACTION_BINDING*)NULL)->ab_context)},
	{	RT_ALLOC_STRING, "message", offsetof(ACTION_BINDING, ab_binding.mousebind.msg), 0},
	{	RT_STRING_CALLBACK, "command", 0, .r_descriptor = {.r_t_callback = mouse_parseCommand}},
	{	RT_STRING_CALLBACK, "mouse", 0, .r_descriptor = {.r_t_callback = binding_parseMouse}},
	{	RT_END }
};

static ACTION_BINDING* key_createActionBinding() {
	return (ACTION_BINDING*)calloc(1, sizeof(ACTION_BINDING));
}

static JSON_MAPPING_RULE _jsonBindingsRules[] = {
	{	RT_OBJECT_LIST, "key-bindings", offsetof(JSON_BINDINGS, keys), 
		{.r_t_arrayDescriptor = {key_createActionBinding, _keybindRules}}
	},
	{	RT_OBJECT_LIST, "mouse-bindings", offsetof(JSON_BINDINGS, mouse),
		{.r_t_arrayDescriptor = {key_createActionBinding, _mousebindRules}}
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
		pCtx = "default";
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
	free((char*)pBind->ab_binding.mousebind.msg);
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
MOUSEBIND* mouse_getMouseBind(int nButton, int nShift, int nClicks, const char* pszActionContext) {
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
	return (MOUSEBIND*)0;
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
	int (*callback)(KEYBIND * pBinding, void* pParam) = pParam;
	KEYBIND kb;
	kb.keycode = (int)k;
	kb.macref = INTPTR_TO_MACROREF(v);
	return callback(&kb, _callbackParam);
}

/*
 * Iterate all key bindings and invoke a callback on every keybinding defined for one action context. 
 */
void key_bindingsDo(const char* pszActionContext, int (*callback)(KEYBIND* pBinding, void* pParam), void* pParam) {
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
void mouse_bindingsDo(const char* pszActionContext, int (*callback)(MOUSEBIND* pBinding, void* pParam), void* pParam) {
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

static int key_matchMacro(KEYBIND* pBinding, void* pParam) {
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
 * Delete a key binding.
 */
int macro_deleteKeyBinding(KEYCODE key, const char* pszContext) {
	return 0;
}

/*
 * Delete all key bindings for a given macro ref.
 */
int macro_deleteKeyBindingsForMacroRef(MACROREF aMacroRef) {
	return 0;
}


