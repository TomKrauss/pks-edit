/*
 * UndoManager.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: undo changes of editor buffer
 *
 * 										created: 28.01.90
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "alloc.h"
#include "pksedit.h"
#include "caretmovement.h"
#include "textblocks.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"
#include "documentmodel.h"
#include "winfo.h"
#include "actions.h"

 /*--------------------------------------------------------------------------
 * LOCALS
 */
#define	N_DELTAS		64

#define	UNDOPOI(fp)	((UNDO_STACK*)fp->undo)

 /*--------------------------------------------------------------------------
  * ln_order gets passed an opaque data structure which it can use to re-order
  * the lines in a file.
  */
extern void ln_order(FTABLE* fp, void* p1, void* p2);
/*
 * Allocate the undo data for undoing a sort operation.
 */
extern int sort_allocateUndoStructure(FTABLE* fp, LINE* lpfirst, LINE* lplast, void** p1, void** p2);

 /**
  * Represents a sequence of undo deltas - each delta is the operation on one line in the editor (in one special case two lines)
  */
typedef struct tagUNDO_OPERATION {
	struct tagUNDO_OPERATION* prev;
	int numberOfCommands;
	// The "line" delta
	struct tagUNDO_DELTA {
		LINE* lp;				// The remembered old line
		union {
			LINE* lpAnchor;		// The anchor line (previous line to old state) where the old state must be restored or in the case of LN_ORDER, the exchanged line
			int   flag;
		} oldState;
		int	  op;				// one of O_MODIFY, O_DELETE, O_INSERT, O_LNORDER
	} delta[N_DELTAS];
} UNDO_OPERATION;

/**
 * Represents one high level user interaction with the editor like typing one word.
 */
typedef struct tagUNDO_COMMAND {
	UNDO_OPERATION*		atomicSteps;
	BOOL				fileChangedFlag;
	EDTIME				fileSaveTime;
	LINE	*			bls;
	LINE *				ble;		/* start and end marker line- if a current selection exists */
	int					bcs,bce,c1,c2;
	long				ln,col;		/* current cursor position */
	WINFO*				wp;			// the window for which the caret position and the selection was saved.
} UNDO_COMMAND;

/*
 * Represents the linked list of undo delta.
 */
typedef struct undoStack {
	int numberOfCommands;
	int current;
	int maximumUndoCommands;
	// Reserve 10 slots - so we can inspect them in the debugger.
	UNDO_COMMAND* commands[10];
} UNDO_STACK;

/*
 * The number of undo steps supported. 
 */
static int undo_numberOfUndoSteps() {
	int nUndoSupported = GetConfiguration()->nundo;
	return nUndoSupported > 0 ? nUndoSupported : 1;
}

/**
 * While performing an undo - do not record changes the normal way. The redo buffer is calculated explicitly.
 */
static BOOL _undoOperationInProgress = FALSE;

/*
 * Check whether undo is enabled at all.
 */
static BOOL isUndoEnabled() {
	return !_undoOperationInProgress && (GetConfiguration()->options & O_UNDOENABLED ? TRUE : FALSE);
}

/*
 * Returns the pointer of the "current" undo operation from the undo stack.
 */
static UNDO_COMMAND* undo_getCurrentCommand(UNDO_STACK* pStack) {
	return pStack->current < 0 ? NULL : pStack->commands[pStack->current];
}

/*
 * Returns the pointer of the "current" redo operation from the undo stack.
 */
static UNDO_COMMAND* undo_getCurrentRedoCommand(UNDO_STACK* pStack) {
	return pStack->current < pStack->numberOfCommands - 1 ? pStack->commands[pStack->current+1] : NULL;
}

/*--------------------------------------------------------------------------
 * upfree()
 */
static void upfree(UNDO_OPERATION *up, FTABLE* fp, BOOLEAN freeLines) {
	struct tagUNDO_DELTA*	p;
	UNDO_OPERATION *		prev;
	int    		i;

	while(up) {
		if (freeLines) {
			for (i = 0; i < up->numberOfCommands; i++) {
				p = &up->delta[i];
				if (p->lp == NULL || p->lp == fp->tln) {
					continue;
				}
				if (p->op != O_INSERT && p->op <= O_LNORDER) {
					ln_destroy(p->lp);
				}
				if (p->op == O_LNORDER) {
					free(p->oldState.lpAnchor);
				}
			}
		}
		prev = up->prev;
		free(up);
		up = prev;
	}
}


/**
 * Add one step to an undo command. 
 */
static BOOL add_stepToCommand(UNDO_COMMAND* pCommand, LINE* lp, LINE* lpAnchor, int op) {
	register UNDO_OPERATION* pOperation;
	register struct tagUNDO_DELTA* pDelta;

	if (pCommand == NULL) {
		return FALSE;
	}
	pOperation = pCommand->atomicSteps;

	if (pOperation == NULL || pOperation->numberOfCommands >= N_DELTAS) {
		UNDO_OPERATION* upHead = pOperation;
		if ((pOperation = malloc(sizeof * pOperation)) == 0) {
			return FALSE;
		}
		memset(pOperation, 0, sizeof * pOperation);
		pOperation->prev = upHead;
		pCommand->atomicSteps = pOperation;
	}
	pDelta = &pOperation->delta[pOperation->numberOfCommands++];
	pDelta->lp = lp;
	pDelta->oldState.lpAnchor = lpAnchor;
	pDelta->op = op;
	if (op == O_FLAG) {
		pDelta->oldState.flag = lp->lflg;
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * undo_saveOperation()
 * Remembers the actual operation performed.
 */
EXPORT BOOL undo_saveOperation(FTABLE *fp, LINE *lp, LINE *lpAnchor,int op) {	
	UNDO_STACK* pUndoStack;
	register UNDO_COMMAND *pCommand;

	if (!isUndoEnabled()) {
noundoAvaioable:
		if (!_undoOperationInProgress && lp != NULL && (op == O_MODIFY || op == O_DELETE)) {
			free(lp);
		}
		return FALSE;
	}

	pUndoStack = UNDOPOI(fp);
	pCommand = undo_getCurrentCommand(pUndoStack);
	if (!pCommand) {
		goto noundoAvaioable;
	}
	return add_stepToCommand(pCommand, lp, lpAnchor, op);
}

/*--------------------------------------------------------------------------
 * undo_destroyCommand()
 */
static void undo_destroyCommand(UNDO_COMMAND *ubp, FTABLE* fp, BOOLEAN freelines)
{
	if (ubp == 0) {
		return;
	}
	upfree(ubp->atomicSteps, fp, freelines);
	free(ubp);
}

/*--------------------------------------------------------------------------
 * undo_initializeManager()
 * Initialize the undo manager for text document.
 */
EXPORT BOOL undo_initializeManager(FTABLE *fp)
{	UNDO_STACK *pUndoStack;
	int nUndoSupported = undo_numberOfUndoSteps();

	undo_destroyManager(fp);
	int tempSize = sizeof(*pUndoStack)+(((size_t)nUndoSupported-10)*sizeof pUndoStack->commands[0]);
	if ((pUndoStack = malloc(tempSize)) == 0) {
		return FALSE;
	}
	memset(pUndoStack, 0, tempSize);
	pUndoStack->current = -1;
	pUndoStack->maximumUndoCommands = nUndoSupported;
	UNDOPOI(fp) = pUndoStack;
	return TRUE;
}

/*--------------------------------------------------------------------------
 * undo_destroyManager()
 * Removes the current undo manager and deallocates all undo buffers.
 */
EXPORT void undo_destroyManager(FTABLE *fp)
{	UNDO_STACK *pUndoStack = UNDOPOI(fp);

	if (pUndoStack) {
		for (int i = 0; i < pUndoStack->numberOfCommands; i++) {
			undo_destroyCommand(pUndoStack->commands[i], fp, TRUE);
		}
		free(pUndoStack);
		UNDOPOI(fp) = 0;
	}
}

static void undo_deallocateExecutedCommands(UNDO_STACK* pStack, FTABLE* fp, BOOLEAN bFreeLines) {
	for (int i = pStack->current + 1; i < pStack->numberOfCommands; i++) {
		UNDO_COMMAND* pCommand = pStack->commands[i];
		if (pCommand == NULL) {
			continue;
		}
		undo_destroyCommand(pStack->commands[i], fp, bFreeLines);
		pStack->commands[i] = NULL;
	}
}

/*
 * Start a new undo command buffer.
 */
static void undo_allocateCommand(UNDO_STACK* pStack, FTABLE* fp, BOOLEAN bFreeLines) {
	// If we have possible "re-dos" - cancel them at this point, because we begin something new
	undo_deallocateExecutedCommands(pStack, fp, bFreeLines);
	// TODO: this is a partial work around for the following problem.
	// If we change multiple things in the same line and move the cursor in between only one line delta is recorded currently.
	// This will have successive undos being a "no-op" - only the cursor moves, but nothing is really undone.
	// Try to get rid of some of the "no-op" undos this way.
	if (pStack->current >= 0 && pStack->commands[pStack->current] && pStack->commands[pStack->current]->atomicSteps == NULL) {
		// nothing was really recorded in the previous step - ignore.
		return;
	}
	pStack->numberOfCommands = pStack->current+1;
	if (pStack->numberOfCommands >= pStack->maximumUndoCommands) {
		undo_destroyCommand(pStack->commands[0], fp, TRUE);
		memmove(&pStack->commands[0], &pStack->commands[1], sizeof pStack->commands[0] * (pStack->numberOfCommands - 1));
		pStack->numberOfCommands = pStack->maximumUndoCommands - 1;
		pStack->commands[pStack->numberOfCommands] = NULL;
		pStack->current--;
	}
	UNDO_COMMAND* pCommand = malloc(sizeof * pCommand);
	memset(pCommand, 0, sizeof * pCommand);
	pStack->commands[pStack->numberOfCommands++] = pCommand;
	pStack->current++;
}

/**
 * Initialize the and undo command saving the current editing state to restore later. 
 */
static void initUndoCommand(FTABLE* fp, UNDO_COMMAND* pCommand) {
	WINFO* wp = ww_getCurrentEditorWindow();
	MARK* pMark;
	pCommand->fileChangedFlag = fp->flags & F_MODIFIED ? TRUE : FALSE;
	pCommand->fileSaveTime = fp->ti_saved;

	pCommand->wp = wp;
	pCommand->ln = wp->caret.ln;
	pCommand->col = wp->caret.offset;
	if ((pMark = wp->blstart) != 0) {
		pCommand->bls = pMark->m_linePointer;
		pCommand->bcs = pMark->m_column;
	}
	else pCommand->bls = 0;
	if ((pMark = wp->blend) != 0) {
		pCommand->ble = pMark->m_linePointer;
		pCommand->bce = pMark->m_column;
	}
	else pCommand->ble = 0;
	pCommand->c1 = wp->blcol1;
	pCommand->c2 = wp->blcol2;
}

/*--------------------------------------------------------------------------
 * undo_startModification()
 * Invoked before a modification on a file is performed.
 */
EXPORT void undo_startModification(FTABLE *fp)
{
	UNDO_STACK *pUndoStack;
	UNDO_COMMAND* pCommand;

	if (_playing)
		return;

	// Need to reset the "current edited line" in case we are performing multiple edits in distinct places in the same file.
	// Every undo operation needs to allocate a new modify delta in that case.
	fp->tln = NULL;

	pUndoStack = UNDOPOI(fp);
	undo_allocateCommand(pUndoStack, fp, TRUE);
	pCommand = undo_getCurrentCommand(pUndoStack);
	initUndoCommand(fp, pCommand);
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
}

/*
 * Applies a list of undo deltas on a file.
 * Return an undo-command to redo the changes.
 */
static UNDO_COMMAND* applyUndoDeltas(FTABLE *fp, UNDO_COMMAND *pCommand, BOOL bRedo) {
	long ln, col;
	register int i;
	register UNDO_OPERATION* pOperation;
	register struct tagUNDO_DELTA* pDelta;
	register LINE* lp;
	register LINE* lpNext;
	BOOL bRedrawAll = FALSE;
	UNDO_COMMAND* pRedoCommand = malloc(sizeof * pRedoCommand);
	WINFO* wp = pCommand->wp;
	LINE* lpChange;

	if (!ft_hasView(fp, wp)) {
		wp = NULL;
	}
	memset(pRedoCommand, 0, sizeof * pRedoCommand);
	initUndoCommand(fp, pRedoCommand);
	ln = 0; col = 0;
	if (wp) {
		// Do we need this?
		// wp->renderer->r_placeCaret(wp, &ln, 0, &col, 0,0);
		bl_hideSelection(wp, 0);
	}
	pOperation = pCommand->atomicSteps;
	while (pOperation != NULL) {
		for (i = pOperation->numberOfCommands; i > 0; ) {
			i--;
			pDelta = &pOperation->delta[i];
			switch (pDelta->op) {
			case O_HIDE:
				ln_unhide(fp, pDelta->lp);
				lp = pDelta->lp->next;
				add_stepToCommand(pRedoCommand, lp, lp->next->prev, O_UNHIDE);
				bRedrawAll = TRUE;
				break;
			case O_UNHIDE:
				lp = ln_hide(fp, pDelta->lp, pDelta->oldState.lpAnchor);
				add_stepToCommand(pRedoCommand, lp, NULL, O_HIDE);
				bRedrawAll = TRUE;
				break;
			case O_FLAG:
				lpChange = pDelta->lp;
				add_stepToCommand(pRedoCommand, lpChange, NULL, O_FLAG);
				lpChange->lflg = pDelta->oldState.flag;
				render_repaintLine(fp, lpChange);
				break;
			case O_MODIFY:
				if ((lp = pDelta->oldState.lpAnchor) == 0) {
					lp = fp->firstl;
				} else {
					lp = lp->next;
				}
				lpChange = pDelta->lp;
				if (!bRedo) {
					lpChange->lflg &= (LNINDIRECT | LNXMARKED | LNNOTERM | LNNOCR);
					if (lp->lflg & LNSAVED) {
						lpChange->lflg |= LNUNDO_AFTER_SAVE|LNMODIFIED;
					}
				}
				ln_replace(fp, lp, lpChange);
				render_repaintLine(fp, lpChange);
				add_stepToCommand(pRedoCommand, lp, lpChange->prev, pDelta->op);
				break;
			case O_DELETE:
				lpChange = pDelta->lp;
				if (!bRedo) {
					lpChange->lflg &= (LNINDIRECT | LNXMARKED | LNNOTERM | LNNOCR);
					if (pDelta->oldState.lpAnchor->lflg & LNSAVED) {
						lpChange->lflg |= LNUNDO_AFTER_SAVE | LNMODIFIED;
					}
				}
				ln_insert(fp, pDelta->oldState.lpAnchor, lpChange);
				add_stepToCommand(pRedoCommand, lpChange, pDelta->oldState.lpAnchor, O_INSERT);
				bRedrawAll = TRUE;
				break;
			case O_INSERT:
				if ((lp = pDelta->oldState.lpAnchor) != NULL && (lp = lp->prev) != NULL && (lpNext = lp->next) != NULL && ln_delete(fp, lp)) {
					add_stepToCommand(pRedoCommand, lp, lpNext, O_DELETE);
				}
				bRedrawAll = TRUE;
				break;
			case O_LNORDER: {
				void* rp1 = 0;
				void* rp2 = 0;
				sort_allocateUndoStructure(fp, fp->firstl, fp->lastl->prev, &rp1, &rp2);
				ln_order(fp, pDelta->lp, pDelta->oldState.lpAnchor);
				// Hack: these are not really line pointers, but pointers to a sort algo specific data structure
				free(pDelta->lp);
				free(pDelta->oldState.lpAnchor);
				add_stepToCommand(pRedoCommand, rp1, rp2, O_LNORDER);
				bRedrawAll = TRUE;
				break;

			}
			}
		}
		pOperation = pOperation->prev;
	}
	if (wp) {
		ww_selectWindow(wp);
		wp->blcol1 = pCommand->c1;
		wp->blcol2 = pCommand->c2;
		bl_setSelection(wp, pCommand->bls, pCommand->bcs, pCommand->ble, pCommand->bce);
		if (pCommand->bls && pCommand->ble) {
			render_repaintLineRange(fp, pCommand->bls, pCommand->ble);
		}
		caret_placeCursorInCurrentFile(wp, pCommand->ln, pCommand->col);
	}
	if (bRedrawAll) {
		render_repaintAllForFile(fp);
	}

	fp->tln = NULL;
	if (!pCommand->fileChangedFlag && fp->ti_saved == pCommand->fileSaveTime) {
		ft_setFlags(fp, fp->flags & ~F_MODIFIED);
	} else {
		ft_setFlags(fp, fp->flags | F_MODIFIED);
	}
	return pRedoCommand;
}

/*
 * Replace an undo command in the undo stack possibly freeing the old undo command. 
 */
static void undo_replace(UNDO_STACK* pStack, UNDO_COMMAND* pCommand, FTABLE* fp) {
	if (pStack->commands[pStack->current]) {
		undo_destroyCommand(pStack->commands[pStack->current], fp, FALSE);
	}
	pStack->commands[pStack->current] = pCommand;
}

/*--------------------------------------------------------------------------
 * undo_redoLastModification()
 * Redo the last modification undone the actual undo reverting the last operation.
 */
EXPORT BOOL undo_redoLastModification(FTABLE* fp) {
	UNDO_STACK* pStack;
	UNDO_COMMAND* pCommand;

	pStack = UNDOPOI(fp);
	if (_playing ||
		(pCommand = undo_getCurrentRedoCommand(pStack)) == NULL) {
		error_showErrorById(IDS_MSGNOUNDO);
		return FALSE;
	}
	_undoOperationInProgress = TRUE;
	UNDO_COMMAND* pUndoCommand = applyUndoDeltas(fp, pCommand, TRUE);
	pStack->current++;
	if (pUndoCommand != NULL) {
		undo_replace(pStack, pUndoCommand, fp);
	} else {
		undo_deallocateExecutedCommands(pStack, fp, FALSE);
	}
	_undoOperationInProgress = FALSE;
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
	return TRUE;
}

/*
 * Answer true, if redo is available for the given file.
 */
EXPORT BOOL undo_isRedoAvailable(FTABLE* fp) {
	UNDO_STACK* pStack = UNDOPOI(fp);
	return pStack && isUndoEnabled() && undo_getCurrentRedoCommand(pStack) != NULL;
}


/*
 * Answer true, if undo is available for the given file. 
 */
EXPORT BOOL undo_isUndoAvailable(FTABLE* fp) {
	UNDO_STACK* pStack = UNDOPOI(fp);
	return pStack && isUndoEnabled() && undo_getCurrentCommand(pStack) != NULL;
}

/*--------------------------------------------------------------------------
 * undo_lastModification()
 * Performs the actual undo reverting the last operation.
 */
EXPORT BOOL undo_lastModification(FTABLE *fp)
{
	register UNDO_COMMAND			*pCommand;
	UNDO_STACK						*pStack;

	pStack = UNDOPOI(fp);
	if (_playing ||
		(pCommand = undo_getCurrentCommand(pStack)) == NULL) {
		error_showErrorById(IDS_MSGNOUNDO);
		return FALSE;
	}
	_undoOperationInProgress = TRUE;
	UNDO_COMMAND *pRedoCommand = applyUndoDeltas(fp, pCommand, FALSE);
	if (pRedoCommand != NULL) {
		undo_replace(pStack, pRedoCommand, fp);
	}
	pStack->current--;
	if (pRedoCommand == NULL) {
		undo_deallocateExecutedCommands(pStack, fp, FALSE);
	}
	_undoOperationInProgress = FALSE;
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
	return isUndoEnabled();
}

