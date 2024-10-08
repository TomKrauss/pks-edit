/*
 * Actions.c
 *
 * Project: PKS Edit for Windows
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
 *
 */

#include "actions.h"
#include "documentmodel.h"
#include "edierror.h"
#include "trace.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "funcdef.h"
#include "arraylist.h"
#include "editorconfiguration.h"

static ARRAY_LIST* _allActions;

static BOOL action_isEnabled(const ACTION* pAction) {
	if (pAction->ac_commandType == CMD_CMDSEQ) {
		const COMMAND* pCommand = &_commandTable[pAction->ac_commandId];
		return interpreter_canExecuteNativeFunction(_commandTable[pAction->ac_commandId].c_functionDef.funcnum, pCommand->c_functionDef.p, 0);
	}
	else if (pAction->ac_commandType == CMD_MACRO) {
		return interpreter_canExecuteMacro(pAction->ac_commandId, 0);
	}
	return TRUE;
}

/**
 * One of the flags defining the enablement of a command has changed. Recalculate the action enablement
 * and notify all action bindings of the change.
 */
void action_commandEnablementChanged(ACTION_CHANGE_TYPE type) {
	// for now: on a long term run - make this part of the action protocol.
	if (type.act_optionsChanged) {
		op_updateall();
	}
	if (_allActions == NULL) {
		return;
	}
	ARRAY_ITERATOR pIterator = arraylist_iterator(_allActions);
	while (pIterator.i_buffer < pIterator.i_bufferEnd) {
		ACTION* pAction = *pIterator.i_buffer;
		if (type.act_commandId >= 0 && pAction->ac_commandId != type.act_commandId) {
			pIterator.i_buffer++;
			continue;
		}
		if (type.act_commandEnablement) {
			int  bEnabled = action_isEnabled(pAction);
			if (bEnabled != pAction->ac_enabled) {
				pAction->ac_enabled = bEnabled;
				for (int j = 0; j < DIM(pAction->ac_bindings); j++) {
					PROPERTY_CHANGE_LISTENER pListener = pAction->ac_bindings[j].ab_propertyChanged;
					if (pListener) {
						(*pListener)(&pAction->ac_bindings[j], PC_ENABLED, bEnabled);
					}
				}
			}
		}
		pIterator.i_buffer++;
	}
}

/*
 * Deregister all action property change listeners of a given type. 
 */
void action_deregisterAllActionsWithListener(const PROPERTY_CHANGE_LISTENER aListener) { 
	if (_allActions == NULL) {
		return;
	}
	ARRAY_ITERATOR pIterator = arraylist_iterator(_allActions);
	while (pIterator.i_buffer < pIterator.i_bufferEnd) {
		ACTION* pAction = *pIterator.i_buffer;
		for (int j = 0; j < DIM(pAction->ac_bindings); j++) {
			if (pAction->ac_bindings[j].ab_propertyChanged == aListener) {
				pAction->ac_bindings[j].ab_propertyChanged = NULL;
			}
		}
		pIterator.i_buffer++;
	}
}

static void action_reevaluate(ACTION * pAction, ACTION_BINDING* pBinding) {
	pAction->ac_enabled = action_isEnabled(pAction);
	(*pBinding->ab_propertyChanged)(pBinding, PC_ENABLED, pAction->ac_enabled);
}

/**
 * Register an action binding for a command.
 */
void action_registerAction(int commandId, int nType, ACTION_BINDING binding, BOOL bEvaluate) {
	if (_allActions == NULL) {
		_allActions = arraylist_create(33);
	}
	size_t nSize = arraylist_size(_allActions);
	for (int i = 0; i < nSize; i++) {
		ACTION* pAction = arraylist_get(_allActions, i);
		if (pAction->ac_commandId == commandId && pAction->ac_commandType == nType) {
			for (int j = 0; j < DIM(pAction->ac_bindings); j++) {
				if (pAction->ac_bindings[j].ab_propertyChanged == NULL) {
					pAction->ac_bindings[j] = binding;
					if (bEvaluate) {
						action_reevaluate(pAction, &pAction->ac_bindings[j]);
					}
					return;
				}
			}
			EdTRACE(log_message(DEBUG_ERR, "No space left for action binding. Maximum 3 bindings allowed."));
			return;
		}
	}
	ACTION* pNew = calloc(1, sizeof *pNew);
	if (pNew == NULL) {
		return;
	}
	arraylist_add(_allActions, pNew);
	pNew->ac_commandId = commandId;
	pNew->ac_commandType = nType;
	pNew->ac_enabled = -1;
	pNew->ac_bindings[0] = binding;
	if (bEvaluate) {
		action_reevaluate(pNew, pNew->ac_bindings);
	}
}

/*
 * Remove all actions and action bindings. 
 */
void action_destroyAll() {
	if (_allActions == 0) {
		return;
	}
	size_t nActions = arraylist_size(_allActions);
	for (int i = 0; i < nActions; i++) {
		free(arraylist_get(_allActions, i));
	}
	arraylist_destroy(_allActions);
	_allActions = NULL;
}
