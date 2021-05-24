/*
 * UndoManager.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: undo changes of editor buffer
 *
 * 										created      : 28.01.90
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauß
 */

#include "alloc.h"
#include "pksedit.h"
#include "caretmovement.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"
#include "lineoperations.h"

/*--------------------------------------------------------------------------
 * LOCALS
 */
#define	N_DELTAS		64

#define	UNDOPOI(fp)	((UNDO_STACK*)fp->undo)

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

/*--------------------------------------------------------------------------
 * undo_saveOperation()
 * Remembers the actual operation performed.
 */
EXPORT BOOL undo_saveOperation(FTABLE *fp, LINE *lp, LINE *lpAnchor,int op)
{	register UNDO_OPERATION *pOperation;
	UNDO_STACK* pUndoStack;
	register UNDO_COMMAND *pCommand;
	struct tagUNDO_DELTA *pDelta;

	if (!isUndoEnabled()) {
		if (op == O_MODIFY || op == O_DELETE) {
			free(lp);
		}
		return FALSE;
	}

	pUndoStack = UNDOPOI(fp);
	pCommand = undo_getCurrentCommand(pUndoStack);
	// (A) should be not null
	if (pCommand == NULL) {
		// alert("Bad undo state");
		return FALSE;
	}
	pOperation = pCommand->atomicSteps;

	if (pOperation == NULL || pOperation->numberOfCommands >= N_DELTAS) {
		UNDO_OPERATION* upHead = pOperation;
		if ((pOperation = malloc(sizeof *pOperation)) == 0) {
			return FALSE;
		}
		memset(pOperation, 0, sizeof *pOperation);
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
		pStack->commands[pStack->maximumUndoCommands - 1] = NULL;
	} else {
		UNDO_COMMAND* pCommand = malloc(sizeof * pCommand);
		memset(pCommand, 0, sizeof * pCommand);
		pStack->commands[pStack->numberOfCommands++] = pCommand;
		pStack->current++;
	}
}

/*--------------------------------------------------------------------------
 * undo_startModification()
 * Invoked before a modification on a file is performed.
 */
EXPORT void undo_startModification(FTABLE *fp)
{
	UNDO_STACK *pUndoStack;
	UNDO_COMMAND* pCommand;
	MARK 	*pMark;
	LINE 	*lp1,*lptmp;

	if (_playing)
		return;

	pUndoStack = UNDOPOI(fp);
	undo_allocateCommand(pUndoStack);
	pCommand = undo_getCurrentCommand(pUndoStack);
	pCommand->fileChangedFlag = fp->flags & F_MODIFIED ? TRUE : FALSE;
	lptmp = fp->tln;
	if (lptmp != 0 && (lp1 = ln_cut(lptmp,lptmp->len,0,lptmp->len)) != 0L) {
		ln_replace(fp,lptmp,lp1);
		lp1->lflg = lptmp->lflg;
		free(lptmp);
		fp->tln = NULL;
	}
	pCommand->ln  = fp->ln;
	pCommand->col = fp->caret.offset;
	if ((pMark = fp->blstart) != 0) {
		pCommand->bls = pMark->lm;
		pCommand->bcs = pMark->lc;
	} else pCommand->bls = 0;
	if ((pMark = fp->blend) != 0) {
		pCommand->ble = pMark->lm;
		pCommand->bce = pMark->lc;
	} else pCommand->ble = 0;
	pCommand->c1  = fp->blcol1;
	pCommand->c2  = fp->blcol2;
}

/*--------------------------------------------------------------------------
 * undo_redoLastModification()
 * Redo the last modification undone the actual undo reverting the last operation.
 */
EXPORT BOOL undo_redoLastModification(FTABLE* fp) {
	UNDO_STACK* pStack;

	pStack = UNDOPOI(fp);
	register UNDO_COMMAND* pCommand;
	if (_playing ||
		(pCommand = undo_getCurrentRedoCommand(pStack)) == NULL) {
		ed_error(IDS_MSGNOUNDO);
		return FALSE;
	}
	pStack->current++;
	_undoOperationInProgress = TRUE;

	_undoOperationInProgress = FALSE;
	return TRUE;
}

/*
 * applies a list of undo deltas on a file.
 */
static void applyUndoDeltas(FTABLE *fp, UNDO_COMMAND *pCommand) {
	long	    					ln, col;
	register int 					i;
	register UNDO_OPERATION* pOperation;
	register struct tagUNDO_DELTA* pDelta;
	register LINE* lp;

	ln = 0; col = 0;
	cposvalid(fp, &ln, &col, 0);

	fp->flags |= F_CHANGEMARK;

	Pastehide(0);
	pOperation = pCommand->atomicSteps;
	while (pOperation != NULL) {
		for (i = pOperation->numberOfCommands; i > 0; ) {
			i--;
			pDelta = &pOperation->delta[i];
			switch (pDelta->op) {
			case O_HIDE:
				ln_unhide(fp, pDelta->lp);
				break;
			case O_UNHIDE:
				ln_hide(fp, pDelta->lp, pDelta->lpAnchor);
				break;
			case O_MODIFY:
				if ((lp = pDelta->lpAnchor) == 0) {
					lp = fp->firstl;
				}
				else {
					lp = lp->next;
				}
				pDelta->lp->lflg &= (LNINDIRECT | LNXMARKED | LNNOTERM | LNNOCR);
				ln_replace(fp, lp, pDelta->lp);
				break;
			case O_DELETE:
				pDelta->lp->lflg &= (LNINDIRECT | LNXMARKED | LNNOTERM | LNNOCR);
				ln_insert(fp, pDelta->lpAnchor, pDelta->lp);
				break;
			case O_INSERT:
				if ((lp = pDelta->lpAnchor) != 0) {
					ln_delete(fp, lp->prev);
				}
				break;
			case O_LNORDER:
				ln_order(fp, pDelta->lp, pDelta->lpAnchor);
				free(pDelta->lp);
				free(pDelta->lpAnchor);
				break;
			}
		}
		pOperation = pOperation->prev;
	}
	fp->blcol1 = pCommand->c1;
	fp->blcol2 = pCommand->c2;

	markblk(fp, pCommand->bls, pCommand->bcs, pCommand->ble, pCommand->bce);

	curpos(pCommand->ln, pCommand->col);
	RedrawTotalWindow(fp);

	fp->tln = NULL;
	if (!pCommand->fileChangedFlag) {
		fp->flags &= ~F_MODIFIED;
	}
	else {
		fp->flags |= F_MODIFIED;
	}
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
		ed_error(IDS_MSGNOUNDO);
		return FALSE;
	}
	_undoOperationInProgress = TRUE;
	applyUndoDeltas(fp, pCommand);
	pStack->current--;
	undo_deallocateExecutedCommands(pStack);
	_undoOperationInProgress = FALSE;
	return isUndoEnabled();
}

