/*
 * actionbindings.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: methods for registering, printing, defining and deleting action
 * bindings (mouse-triggered, keyboard-triggered, menu-triggered actions)
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef ACTION_BINDINGS_H

// forward declaration
typedef struct tagMACROREF MACROREF;

 // To be used, whenever text is defined in an action binding. Allows to define the text either plain
 // as text or alternatively as a resource given a resource index into PKS-Edits internal resource table.
typedef struct tagBOUND_TEXT {
	const char* bt_text;
	int			bt_resourceId;
} BOUND_TEXT;

typedef unsigned short KEYCODE;

/* ASCII representation */
#define ALT_MARK		'~'
#define CTL_MARK		'^'
#define SFT_MARK		'#'
#define WIN_MARK		'&'
#define SELECTED_MARK	'*'

#define	K_SHIFT			0x300
#define	K_CONTROL		0x400
#define	K_ALTERNATE		0x800
#define	K_HAS_SELECTION 0x1000
#define	K_WINDOWS		0x2000

#define	K_DELETED		0x1FFF		/* Impossible Key Combination	*/
#define	K_INVALID		K_DELETED

typedef struct tagKEY_BINDING {
	KEYCODE		keycode;
	MACROREF	macref;
} KEY_BINDING;

/*
 * Can be used to add all keycode names and modifiers to a suggestion list.
 */
extern void bindings_addModifiersAndKeycodes(int (*fMatch)(const char* pszString), void (*fCallback)(const char* pszString));

extern	char* bindings_keycodeToString(KEYCODE code);
extern KEYCODE bindings_addModifierKeysToKeycode(KEYCODE code);
/*
 * bindings_printModifier()
 * Print the modifiers in the form "Alt+Shift...." into pszDestination and return the character
 * pointer at the end of the printed text.
 */
extern char* bindings_printModifier(char* pszDestination, KEYCODE code);

typedef struct tagTOOLBAR_BUTTON_BINDING {
	struct tagTOOLBAR_BUTTON_BINDING* tbb_next;
	BOOL        tbb_isSeparator;
	wchar_t		tbb_faIcon;							// Font awesome icon code for this button
	BOUND_TEXT	tbb_label;							// Optional label to use. only for buttons displaying a label.
	MACROREF	tbb_macref;
} TOOLBAR_BUTTON_BINDING;

typedef struct tagMENU_ITEM_DEFINITION {
	struct tagMENU_ITEM_DEFINITION* mid_next;
	struct tagMENU_ITEM_DEFINITION* mid_children;	// for sub-menus, this is the list of child menu items.
	HMENU		mid_menuHandle;						// For sub-menus, this is the menu handle of menu item, after it had been created.
	BOUND_TEXT	mid_label;							// The label to display for the menu. If no label is specified, a default for the defined command is used.
	BOOL		mid_isSeparator;					// true for separator menus. Separator menus do not need a label
	BOOL		mid_isHistoryMenu;					// true for "history" menus. Will be replaced during runtime by a list of recently opened files.
	MACROREF	mid_command;						// the associated command to execute.
} MENU_ITEM_DEFINITION;

/*
 * Returns a linked list of context menu entries for a given action context.
 */
extern MENU_ITEM_DEFINITION* bindings_getContextMenuFor(const char* pszActionContext);

/*
 * Returns a linked list of main menu entries for a given action context.
 */
extern MENU_ITEM_DEFINITION* bindings_getMenuBarFor(const char* pszActionContext);

/*
 * Returns a linked list of main menu entries for a popup menu of the application menu bar with the given
 * menu handle and for an action context.
 */
extern MENU_ITEM_DEFINITION* bindings_getMenuBarPopupDefinitionFor(const char* pszActionContext, HMENU hMenu);

/*
 * Returns a linked list of toolbar button bindings for an action context (currently ignored - toolbar is never dynamic).
 */
extern TOOLBAR_BUTTON_BINDING* bindings_getToolbarBindingsFor(const char* pszActionContext);

#define	MBUT_L	0x1
#define	MBUT_R	0x2
#define	MBUT_M	0x4
#define	MBUT_RL	(MBUT_L|MBUT_R)

typedef struct tagMOUSECODE {
	unsigned	button : 4,
		nclicks : 4,			/* # of button clicks */
		shift : 8;			/* kb-state */
} MOUSECODE;

typedef struct tagMOUSE_EVENT_BINDING {
	char 		button;				// mousebutton
	char 		nclicks;			// # of button clicks
	int 		shift;				// kb-state shift/control/...
	MACROREF	macref;
	BOUND_TEXT	msg;				// Optional message to display....
} MOUSE_EVENT_BINDING;

/*------------------------------------------------------------
 * bindings_bindKey()
 * bind a command to a keycode dynamically in a given context.
 */
extern int bindings_bindKey(KEYCODE key, MACROREF macro, const char* pszContext);

/*
 * Delete all key bindings for a given macro ref.
 */
extern int bindings_deleteKeyBindingsForMacroRef(MACROREF aMacroRef);

/*
 * Delete a key binding.
 */
extern int bindings_deleteKeyBinding(KEYCODE key, const char* pszActionContext);

/*
 * Iterate all key bindings and invoke a callback on every keybinding defined for one action context.
 */
extern void bindings_forAllKeyBindingsDo(const char* pszActionContext, int (*callback)(KEY_BINDING* pBinding, void* pParam), void* pParam);

/*
 * Iterate all mouse bindings and invoke a callback on every mousebinding defined for one action context.
 */
extern void bindings_forAllMouseBindingsDo(const char* pszActionContext, int (*callback)(MOUSE_EVENT_BINDING* pBinding, void* pParam), void* pParam);

/*
 * Returns the text to use for an aspect of an action binding, which describes a text to
 * display in an action trigger.
 */
extern const char* bindings_getBoundText(BOUND_TEXT* pText);

/*
 * Find the 1st key bound to a given macro.
 */
extern KEYCODE bindings_findBoundKey(const char* pszActionContext, MACROREF macro);

/*
 * Returns the command to be executed, when a specific keycode was pressed in a given action context.
 */
extern MACROREF bindings_getKeyBinding(KEYCODE keycode, const char* pszActionContext);

/*
 * Name of the default action context to use in PKS-Edit.
 */
extern const char* DEFAULT_ACTION_CONTEXT;

/*---------------------------------
 * bindings_addModifierKeysToKeycode()
 * Add the modifier key bits depending on whether
 * Shift, Control or Alt was pressed together with
 * the key passed as an argument.
 */
extern KEYCODE bindings_addModifierKeysToKeycode(KEYCODE key);

/*--------------------------------------------------------------------------
 * macro_getIndexForKeycode()
 * Return the internall index for a given macro keycode and name.
 */
extern int macro_getIndexForKeycode(KEYCODE* scan, char* name, int oldidx);

#define ACTION_BINDINGS_H
#endif