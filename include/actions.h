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
 * (c) Pahlen & Krauss
 *
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

/**
 * One of the flags defining the enablement of a command has changed. Recalculate the action enablement
 * and notify all action bindings of the change.
 */
extern void action_commandEnablementChanged();

/**
 * Register an action binding for a command. 
 */
extern void action_registerAction(int commandId, ACTION_BINDING binding);

#define ACTIONS_H
#endif