/*
 * UndoManager.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: undo changes of editor buffer
 *
 * 										created: 28.01.90
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauß
 */

#include "alloc.h"
#include "pksedit.h"
#include "caretmovement.h"
#include "textblocks.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"
#include "lineoperations.h"
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

 /**
  * Represents a sequence of undo deltas - each delta is the operation on one line in the editor (in one special case two lines)
  */
typedef struct tagUNDO_OPERATION {
	struct tagUNDO_OPERATION* prev;
	int numberOfCommands;
	// The "line" delta
	struct tagUNDO_DELTA {
		LINE* lp;				// The remembered old line
		LINE* lpAnchor;			// The anchor line (previous line to old state) where the old state must be restored or in the case of LN_ORDER, the exchanged line
		int	  op;				// one of O_MODIFY, O_DELETE, O_INSERT, O_LNORDER
	} delta[N_DELTAS];
} UNDO_OPERATION;

/**
 * Represents one high level user interaction with the editor like typing one word.
 */
typedef struct tagUNDO_COMMAND {
	UNDO_OPERATION*		atomicSteps;
	BOOL				fileChangedFlag;
	LINE	*			bls;
	LINE *				ble;		/* start and end marker line- if a current selection exists */
	int					bcs,bce,c1,c2;
	long				ln,col;		/* current cursor position */
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
	return !_undoOperationInProgress && (GetConfiguration()->options & UNDOENABLED ? TRUE : FALSE);
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
	return pStack->current < pStack->numberOfCommands ? pStack->commands[pStack->current+1] : NULL;
}

/*--------------------------------------------------------------------------
 * upfree()
 */
static void upfree(UNDO_OPERATION *up) {
	struct tagUNDO_DELTA*	p;
	UNDO_OPERATION *		prev;
	int    		i;

	while(up) {
		for (i = 0; i < up->numberOfCommands; i++) {
			p = &up->delta[i];
			if (p->lp == NULL) {
				continue;
			}
			if (p->op != O_INSERT && p->op <= O_LNORDER) {
				ln_destroy(p->lp);
			}
			if (p->op == O_LNORDER) {
				free(p->lpAnchor);
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
		// error_displayAlertDialog("Bad undo state");
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
	pDelta->lpAnchor = lpAnchor;
	pDelta->op = op;
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
		if (!_undoOperationInProgress && lp != NULL && (op == O_MODIFY || op == O_DELETE)) {
			free(lp);
		}
		return FALSE;
	}

	pUndoStack = UNDOPOI(fp);
	pCommand = undo_getCurrentCommand(pUndoStack);
	// (A) should be not null
	return add_stepToCommand(pCommand, lp, lpAnchor, op);
}

/*--------------------------------------------------------------------------
 * u_free()
 */
static void u_free(UNDO_COMMAND *ubp)
{
	if (ubp == 0) {
		return;
	}
	upfree(ubp->atomicSteps);
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
	int tempSize = sizeof(*pUndoStack)+((nUndoSupported-10)*sizeof pUndoStack->commands[0]);
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
			u_free(pUndoStack->commands[i]);
		}
		_free(pUndoStack);
		UNDOPOI(fp) = 0;
	}
}

static void undo_deallocateExecutedCommands(UNDO_STACK* pStack) {
	for (int i = pStack->current + 1; i < pStack->numberOfCommands; i++) {
		UNDO_COMMAND* pCommand = pStack->commands[i];
		if (pCommand == NULL) {
			continue;
		}
		UNDO_OPERATION* pStep = pCommand->atomicSteps;
		while (pStep != NULL) {
			for (int j = 0; j < pStep->numberOfCommands; j++) {
				pStep->delta[j].lp = NULL;
			}
			pStep = pStep->prev;
		}
		u_free(pStack->commands[i]);
		pStack->commands[i] = NULL;
	}
}

/*
 * Start a new undo command buffer.
 */
static void undo_allocateCommand(UNDO_STACK* pStack) {
	// If we have possible "re-dos" - cancel them at this point, because we begin something new
	undo_deallocateExecutedCommands(pStack);
	pStack->numberOfCommands = pStack->current+1;
	if (pStack->numberOfCommands >= pStack->maximumUndoCommands) {
		u_free(pStack->commands[0]);
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
	MARK* pMark;
	LINE* lp1, * lptmp;
	pCommand->fileChangedFlag = fp->flags & F_MODIFIED ? TRUE : FALSE;
	lptmp = fp->tln;
	if (lptmp != 0 && (lp1 = ln_cut(lptmp, lptmp->len, 0, lptmp->len)) != 0L) {
		ln_replace(fp, lptmp, lp1);
		lp1->lflg = lptmp->lflg;
		free(lptmp);
		fp->tln = NULL;
	}
	pCommand->ln = fp->ln;
	pCommand->col = fp->caret.offset;
	if ((pMark = fp->blstart) != 0) {
		pCommand->bls = pMark->lm;
		pCommand->bcs = pMark->lc;
	}
	else pCommand->bls = 0;
	if ((pMark = fp->blend) != 0) {
		pCommand->ble = pMark->lm;
		pCommand->bce = pMark->lc;
	}
	else pCommand->ble = 0;
	pCommand->c1 = fp->blcol1;
	pCommand->c2 = fp->blcol2;
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

	pUndoStack = UNDOPOI(fp);
	undo_allocateCommand(pUndoStack);
	pCommand = undo_getCurrentCommand(pUndoStack);
	initUndoCommand(fp, pCommand);
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
}

/*
 * Applies a list of undo deltas on a file.
 * Return an undo-command to redo the changes.
 */
static UNDO_COMMAND* applyUndoDeltas(FTABLE *fp, UNDO_COMMAND *pCommand) {
	long	    					ln, col;
	register int 					i;
	register UNDO_OPERATION* pOperation;
	register struct tagUNDO_DELTA* pDelta;
	register LINE* lp;
	register LINE* lpNext;
	BOOL bRedrawAll = FALSE;
	UNDO_COMMAND* pRedoCommand = malloc(sizeof * pRedoCommand);

	memset(pRedoCommand, 0, sizeof * pRedoCommand);
	initUndoCommand(fp, pRedoCommand);
	ln = 0; col = 0;
	caret_placeCursorAndValidate(fp, &ln, &col, 0);

	bl_hideSelection(0);
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
				lp = ln_hide(fp, pDelta->lp, pDelta->lpAnchor);
				add_stepToCommand(pRedoCommand, lp, NULL, O_HIDE);
				bRedrawAll = TRUE;
				break;
			case O_MODIFY:
				if ((lp = pDelta->lpAnchor) == 0) {
					lp = fp->firstl;
				} else {
					lp = lp->next;
				}
				pDelta->lp->lflg &= (LNINDIRECT | LNXMARKED | LNNOTERM | LNNOCR);
				ln_replace(fp, lp, pDelta->lp);
				render_repaintLine(fp, pDelta->lp);
				add_stepToCommand(pRedoCommand, lp, pDelta->lp->prev, pDelta->op);
				break;
			case O_DELETE:
				pDelta->lp->lflg &= (LNINDIRECT | LNXMARKED | LNNOTERM | LNNOCR);
				ln_insert(fp, pDelta->lpAnchor, pDelta->lp);
				add_stepToCommand(pRedoCommand, pDelta->lp, pDelta->lpAnchor, O_INSERT);
				bRedrawAll = TRUE;
				break;
			case O_INSERT:
				if ((lp = pDelta->lpAnchor) != NULL && (lp = lp->prev) != NULL && (lpNext = lp->next) != NULL && ln_delete(fp, lp)) {
					add_stepToCommand(pRedoCommand, lp, lpNext, O_DELETE);
				}
				bRedrawAll = TRUE;
				break;
			case O_LNORDER:
				ln_order(fp, pDelta->lp, pDelta->lpAnchor);
				add_stepToCommand(pRedoCommand, pDelta->lpAnchor, pDelta->lp, O_LNORDER);
				bRedrawAll = TRUE;
				break;
			}
		}
		pOperation = pOperation->prev;
	}
	fp->blcol1 = pCommand->c1;
	fp->blcol2 = pCommand->c2;

	bl_setSelection(fp, pCommand->bls, pCommand->bcs, pCommand->ble, pCommand->bce);

	caret_placeCursorInCurrentFile(pCommand->ln, pCommand->col);
	if (bRedrawAll) {
		render_repaintAllForFile(fp);
	}

	fp->tln = NULL;
	if (!pCommand->fileChangedFlag) {
		ft_setFlags(fp, fp->flags & ~F_MODIFIED);
	} else {
		ft_setFlags(fp, fp->flags | F_MODIFIED);
	}
	return pRedoCommand;
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
	UNDO_COMMAND* pUndoCommand = applyUndoDeltas(fp, pCommand);
	pStack->current++;
	if (pUndoCommand != NULL) {
		pStack->commands[pStack->current] = pUndoCommand;
	}
	if (pUndoCommand == NULL) {
		undo_deallocateExecutedCommands(pStack);
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
	return isUndoEnabled() && undo_getCurrentRedoCommand(pStack) != NULL;
}


/*
 * Answer true, if undo is available for the given file. 
 */
EXPORT BOOL undo_isUndoAvailable(FTABLE* fp) {
	UNDO_STACK* pStack = UNDOPOI(fp);
	return isUndoEnabled() && undo_getCurrentCommand(pStack) != NULL;
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
	UNDO_COMMAND *pRedoCommand = applyUndoDeltas(fp, pCommand);
	if (pRedoCommand != NULL) {
		pStack->commands[pStack->current] = pRedoCommand;
	}
	pStack->current--;
	if (pRedoCommand == NULL) {
		undo_deallocateExecutedCommands(pStack);
	}
	_undoOperationInProgress = FALSE;
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
	return isUndoEnabled();
}

