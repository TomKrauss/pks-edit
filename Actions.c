/*
 * Actions.c
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

#include "actions.h"
#include "lineoperations.h"
#include "trace.h"
#include "edfuncs.h"
#include "arraylist.h"
#include "editorconfiguration.h"

static ARRAY_LIST* _allActions;

/**
 * One of the flags defining the enablement of a command has changed. Recalculate the action enablement
 * and notify all action bindings of the change.
 */
void action_commandEnablementChanged() {
	// for now: on a long term run - make this part of the action protocol.
	op_updateall();
	if (_allActions == NULL) {
		return;
	}
	ARRAY_ITERATOR pIterator = arraylist_iterator(_allActions);
	BOOL bHasFile = ft_getCurrentDocument() != NULL;
	BOOL bHasSelection = ft_checkSelection(ft_getCurrentDocument());
	while (pIterator.i_buffer < pIterator.i_bufferEnd) {
		ACTION* pAction = *pIterator.i_buffer;
		EDFUNC* pFunc = &_edfunctab[_cmdseqtab[pAction->ac_commandId].c_functionDef.funcnum];
		int flags = pFunc->flags;
		BOOL bEnabled = TRUE;
		if ((flags & EW_NEEDSCURRF) && !bHasFile) {
			bEnabled = FALSE;
		}
		if ((flags & EW_NEEDSBLK) && !bHasSelection) {
			bEnabled = FALSE;
		}
		if (bEnabled != pAction->ac_enabled) {
			pAction->ac_enabled = bEnabled;
			for (int j = 0; j < DIM(pAction->ac_bindings); j++) {
				PROPERTY_CHANGE_LISTENER pListener = pAction->ac_bindings[j].ab_propertyChanged;
				if (pListener) {
					(*pListener)(&pAction->ac_bindings[j], PC_ENABLED, bEnabled);
				}
			}
		}
		pIterator.i_buffer++;
	}
}

/**
 * Register an action binding for a command.
 */
void action_registerAction(int commandId, ACTION_BINDING binding) {
	if (_allActions == NULL) {
		_allActions = arraylist_create(33);
	}
	size_t nSize = arraylist_size(_allActions);
	for (int i = 0; i < nSize; i++) {
		ACTION* pAction = arraylist_get(_allActions, i);
		if (pAction->ac_commandId == commandId) {
			for (int j = 0; j < DIM(pAction->ac_bindings); j++) {
				if (pAction->ac_bindings[j].ab_propertyChanged == NULL) {
					pAction->ac_bindings[j] = binding;
					return;
				}
			}
			EdTRACE(Debug(DEBUG_ERR, "No space left for action binding. Maximum 3 bindings allowed."));
			return;
		}
	}
	ACTION* pNew = calloc(1, sizeof *pNew);
	arraylist_add(_allActions, pNew);
	pNew->ac_commandId = commandId;
	pNew->ac_enabled = -1;
	pNew->ac_bindings[0] = binding;
}
