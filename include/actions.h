/*
 * actions.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: action framework.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef ACTIONS_H

#include <windows.h>

typedef enum { PC_ENABLED, PC_BINDING } PROPERTY_CHANGE_TYPE;

typedef void (*PROPERTY_CHANGE_LISTENER)(struct tagACTION_BINDING *action, PROPERTY_CHANGE_TYPE type, int newValue);

typedef struct tagACTION_BINDING {
	PROPERTY_CHANGE_LISTENER ab_propertyChanged;
	HWND ab_hwnd;
	int ab_item;
} ACTION_BINDING;

typedef struct tagACTION {
	int ac_commandId;					// index into the command table
	int ac_previousCommandState;
	BOOL ac_enabled;
	ACTION_BINDING ac_bindings[3];		// currently maximum 3 bindings supported per command.
} ACTION;

typedef struct tagACTION_CHANGE_TYPE {
	int act_commandEnablement : 1;		// The enablement of all commands must be re-evaluated.
	int act_keyboardBindings : 1;		// The keyboard bindings have changed.
	int act_optionsChanged : 1;			// The display / edit mode options have changed.
	int act_commandId;					// If a single command is affected, the index of the command is passed here. Otherwise this field should be -1.
} ACTION_CHANGE_TYPE;

#define ACTION_CHANGE_COMMAND_ENABLEMENT (ACTION_CHANGE_TYPE)	{1,0,0,-1}
#define ACTION_CHANGE_COMMAND_ALL		(ACTION_CHANGE_TYPE)	{1,1,1,-1}

/**
 * One of the flags defining the enablement of a command has changed. Recalculate the action enablement
 * and notify all action bindings of the change.
 */
extern void action_commandEnablementChanged(ACTION_CHANGE_TYPE type);

/**
 * Register an action binding for a command. 
 * If bEvaluate is true, the binding is evaluated right away.
 */
extern void action_registerAction(int commandId, ACTION_BINDING binding, BOOL bEvaluate);

/*
 * Deregister all action property change listeners of a given type.
 */
extern void action_deregisterAllActionsWithListener(PROPERTY_CHANGE_LISTENER aListener);

/*
 * Remove all actions and action bindings.
 */
extern void action_destroyAll();

#define ACTIONS_H
#endif