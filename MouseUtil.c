/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * MouseUtil.c
 *
 * operations regarding mouse interaction with the user.
 *
 * 						created: 07.06.91 
 *						author: Tom
 */

#include <windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include "trace.h"
#include "caretmovement.h"
#include "textblocks.h"
#include "edierror.h"
#include "errordialogs.h"
#include "winfo.h"
#include "documenttypes.h"
#include "trace.h"

#include "winterf.h"
#include "pksmacro.h"
#include "actionbindings.h"
#include "funcdef.h"
#include "pksedit.h"
#include "iccall.h" 
#include "pksmacro.h"
#include "winutil.h"

static HCURSOR   hArrowCursor;	// Pointer cursor
static HCURSOR   hHandPointer;		// Hand pointer cursor
static HCURSOR   hHourGlass;		// Hour glass cursor
static HCURSOR   hDefaultCurs;
static HCURSOR	 hSizeNSCursor;
static HCURSOR	 hSizeWECursor;
static HCURSOR	 hSizeNWSECursor;


/**
 * Data structure for implementing mouse drag operations. 
 */
typedef struct tag_MOUSE_DRAG_HANDLER {
	// Called when the drag is initiated. Return 0 to cancel the drag operation.
	int (*dragInit)(WINFO* wp, int x, int y);
	// Called for every mouse move during the drag.
	int (*drag)(WINFO* wp, int x, int y);
	// Called when the mouse drag ends.
	int (*dragEnd)(WINFO* wp, int x, int y, int bCancel);
} MOUSE_DRAG_HANDLER;

/**------------------------------------------------------------------------------------------------
 * Implements text selection using the mouse.
 */
struct tagDRAG_SELECTION_DATA {
	CARET c1;
	CARET c2;
	POINT start;
};

static struct tagDRAG_SELECTION_DATA _dragSelectionData;

static int mouse_selectionDragInit(WINFO* wp, int x, int y) {
	_dragSelectionData.c1 = wp->caret;
	_dragSelectionData.start = (POINT){ x, y };
	bl_hideSelectionInCurrentWindow();
	bl_syncSelectionWithCaret(wp, &_dragSelectionData.c1, MARK_START, NULL);
	return 1;
}

static int mouse_selectionDrag(WINFO* wp, int x, int y) {
	struct tagDRAG_SELECTION_DATA* pData = &_dragSelectionData;

	pData->c2 = wp->caret;
	log_errorArgs(DEBUG_ERR, "x == %d -> offset == %d", x, pData->c2.offset);
	caret_updateSelection(wp, &pData->c1, &pData->c2, pData->start.y < y);
	return 1;
}

static MOUSE_DRAG_HANDLER _mouse_selectionDragHandler = {
	mouse_selectionDragInit,
	mouse_selectionDrag,
	NULL
};

/**------------------------------------------------------------------------------------------------
 * Implements text block movement using the mouse.
 */
static struct tagTEXT_BLOCK_MOVE {
	HCURSOR saveCursor;
	BOOL moving;
} _dragTextBlockMoveData;

static int mouse_textBlockMoveDragInit(WINFO* wp, int x, int y) {
	HCURSOR 	hCursor;
	long ln, col;

	wp->renderer->r_hitTest(wp, x, y, &ln, &col);
	if (!bl_selectionContainsLogicalPoint(wp, ln, col)) {
		// If not starting to actually drag the selection...
		bl_hideSelection(wp, 1);
		return 0;
	}
	_dragTextBlockMoveData.moving = GetAsyncKeyState(VK_SHIFT) == 0;
	HMODULE l = LoadLibrary("ole32.dll");
	hCursor = LoadCursor(l, MAKEINTRESOURCE(_dragTextBlockMoveData.moving ? 5 : 6));
	if (hCursor == 0) {
		_dragTextBlockMoveData.saveCursor = 0;
	} else {
		_dragTextBlockMoveData.saveCursor = SetCursor(hCursor);
		SetClassLongPtr(wp->ww_handle, /*does not work: GCL_HCURSOR*/ -12, (LONG_PTR)hCursor);
	}
	return 1;
}

static int mouse_textBlockMoveDrag(WINFO* wp, int x, int y) {
	return 1;
}

static int mouse_textBlockEndDrag(WINFO* wp, int x, int y, int bCancel) {
	HWND hwnd;
	POINT p;
	int ret = 1;

	if (bCancel) {
		SetClassLongPtr(wp->ww_handle, /*does not work: GCL_HCURSOR*/ -12, (LONG_PTR)_dragTextBlockMoveData.saveCursor);
		return 0;
	}
	GetCursorPos(&p);
	hwnd = WindowFromPoint(p);

	if (hwnd == wp->ww_handle) {
		intptr_t stack[8];
		if (_dragTextBlockMoveData.moving) {
			ret = (int)interpreter_executeFunction(FUNC_EdBlockMove, stack);
		} else {
			ret = (int)interpreter_executeFunction(FUNC_EdBlockCopy, stack);
		}
	} else if (hwnd) {
		ret = PostMessage(hwnd, WM_ICONDROP, ICACT_TEXTBLOCK, 0L);
	}
	if (_dragTextBlockMoveData.saveCursor) {
		SetClassLongPtr(wp->ww_handle, /*does not work: GCL_HCURSOR*/ -12, (LONG_PTR)_dragTextBlockMoveData.saveCursor);
	}
	return ret;
}

static MOUSE_DRAG_HANDLER _mouse_textBlockMovement = {
	mouse_textBlockMoveDragInit,
	mouse_textBlockMoveDrag,
	mouse_textBlockEndDrag
};


/**
 * Return a drag handler for handling mouse drags depending on the current context.
 */
static MOUSE_DRAG_HANDLER* mouse_getDragHandler(WINFO* wp, int x, int y) {
	if (ww_hasSelection(wp)) {
		// TODO: should check, whether x and y is "inside the text block"
		return &_mouse_textBlockMovement;
	}
	return &_mouse_selectionDragHandler;
}

/**
 * Utility function to place the caret according to screen coordinates.
 */
void caret_placeToXY(WINFO* wp, int x, int y) {
	long col, ln;

	wp->renderer->r_hitTest(wp, x, y, &ln, &col);
	if (wp->renderer->r_placeCaretAfterClick(wp, &ln, &col, 1)) {
		wt_curpos(wp, ln, col);
	}
}

/*------------------------------------------------------------
 * mouse_getXYPos()
 * Get the current window relative mouse position.
 */
EXPORT void mouse_getXYPos(HANDLE hwnd, int* x, int* y)
{
	POINT p;

	if (!hwnd)
		return;

	GetCursorPos(&p);
	ScreenToClient(hwnd, &p);
	*x = p.x;
	*y = p.y;
}

/**
 * Add a secondary window caret with the mouse.
 */
long long caret_addSecondaryWithMouse() {
	long col, ln;
	WINFO* wp = ww_getCurrentEditorWindow();

	if (!wp) {
		return 0;
	}
	int x, y;
	mouse_getXYPos(wp->ww_handle, &x, &y);
	wp->renderer->r_hitTest(wp, x, y, &ln, &col);
	LINE* lp = ln_goto(wp->fp, ln);
	if (!lp) {
		return 0;
	}
	col = caret_screen2lineOffset(wp, &(CARET) {.linePointer = lp, .offset = col});
	caret_addSecondary(wp, ln, col);
	return 1;
}

/*------------------------------------------------------------
 * mouse_dispatchUntilButtonRelease()
 * Dispatches mouse messages until a mouse button is released.
 * Return the current mouse position / nModifier state etc... in the passed
 * parameters. Return 0, if the user aborted the operation by pressing
 * the ESC key, 1 otherwise.
 */
EXPORT int mouse_dispatchUntilButtonRelease(int *x, int *y, int *but, int *shift)
{ 	MSG msg;
	int ret = 1;

	while(1) {
		PeekMessage(&msg, 0, 0, 0, PM_REMOVE|PM_NOYIELD);
		switch(msg.message) {
			case WM_TIMER:
				if (msg.wParam == TIM_FRAME)
					break;
				return 1;
			case WM_KEYDOWN:
				if (msg.wParam == VK_ESCAPE) {
					ret = 0;
					goto fine;
				}
				break;
			case WM_CAPTURECHANGED:
				if (!msg.lParam) {
					ret = 0;
					goto fine;
				}
			case WM_ERASEBKGND:
			case WM_PAINT:
				DispatchMessage(&msg);
				break;
			case WM_LBUTTONUP: case WM_MBUTTONUP: case WM_RBUTTONUP:
				DispatchMessage(&msg);
				*but = 0;
				goto fine;
			case WM_NCMOUSEMOVE:
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN: case WM_MBUTTONDOWN: case WM_RBUTTONDOWN:
				*but = (msg.wParam & (MK_LBUTTON|MK_RBUTTON|MK_MBUTTON));
				if (GET_X_LPARAM(msg.lParam) != 0 && GET_Y_LPARAM(msg.lParam) != 0) {
					*x = GET_X_LPARAM(msg.lParam); 
					*y = GET_Y_LPARAM(msg.lParam);
				} else {
					break;
				}
				goto fine;
		}
	}
fine:
	*shift = (msg.wParam & (MK_CONTROL|MK_SHIFT));
	return ret;
}

/*------------------------------------------------------------
 * mouse_setBusyCursor()
 * Display the hourglass cursor.
 */
EXPORT void mouse_setBusyCursor(void) {
	if (hHourGlass == NULL) {
		hHourGlass = LoadCursor(NULL, IDC_WAIT);
	}
	SetCursor(hHourGlass);
}

/*------------------------------------------------------------
 * mouse_setHandCursor()
 * Display a hand cursor.
 */
EXPORT void mouse_setHandCursor(void) {
	if (hHandPointer == NULL) {
		hHandPointer = LoadCursor(NULL, IDC_HAND);
	}
	SetCursor(hHandPointer);
}

/*------------------------------------------------------------
 * mouse_setArrowCursor()
 * Display an arrow cursor.
 */
EXPORT void mouse_setArrowCursor(void) {
	if (hArrowCursor == NULL) {
		hArrowCursor = LoadCursor(NULL, IDC_ARROW);
	}
	SetCursor(hArrowCursor);
}

/*------------------------------------------------------------
 * mouse_setSizeNWSECursor()
 * Display a sizeNWSE cursor.
 */
EXPORT void mouse_setSizeNWSECursor(void) {
	if (hSizeNWSECursor == NULL) {
		hSizeNWSECursor = LoadCursor(NULL, IDC_SIZENWSE);
	}
	SetCursor(hSizeNWSECursor);
}

/*------------------------------------------------------------
 * mouse_setSizeWECursor()
 * Display a sizewe cursor.
 */
EXPORT void mouse_setSizeWECursor(void) {
	if (hSizeWECursor == NULL) {
		hSizeWECursor = LoadCursor(NULL, IDC_SIZEWE);
	}
	SetCursor(hSizeWECursor);
}

/*------------------------------------------------------------
 * mouse_setSizeNSCursor()
 * Display a sizens cursor.
 */
EXPORT void mouse_setSizeNSCursor(void) {
	if (hSizeNSCursor == NULL) {
		hSizeNSCursor = LoadCursor(NULL, IDC_SIZENS);
	}
	SetCursor(hSizeNSCursor);
}

/*------------------------------------------------------------
 * mouse_setDefaultCursor()
 * Display the default cursor (arrow)
 */
EXPORT void mouse_setDefaultCursor(void)
{
	if (hDefaultCurs == NULL) {
		hDefaultCurs = LoadCursor(NULL, IDC_IBEAM);
	}
	SetCursor(hDefaultCurs);
}

/*------------------------------------------------------------
 * EdBlockMouseMark()
 * NOT USED ANY MORE.
 */
EXPORT int EdBlockMouseMark(int typ)
{
	return 0;
}

/*--------------------------------------------------------------------------
 * caret_moveToXY()
 * move the caret to follow the mouse pressed on the screen coordinates x and y.
 */
EXPORT int caret_moveToXY(WINFO* wp, int x, int y)
{
	int b = 1, dummy;
	UINT_PTR id;
	POINT pStart = (POINT){ x, y };

	caret_placeToXY(wp, x, y);
	if (!DragDetect(wp->ww_handle, pStart)) {
		bl_hideSelection(wp, 1);
	} else {
		SetCapture(wp->ww_handle);
		CARET c1 = wp->caret;
		CARET c2;
		LINE* lpPrevious;
		int cPrevious;
		MOUSE_DRAG_HANDLER* pHandler = mouse_getDragHandler(wp, x, y);
		if (pHandler == NULL || !pHandler->dragInit(wp, x, y)) {
			if (pHandler) {
				MOUSE_DRAG_HANDLER* pOtherHandler = mouse_getDragHandler(wp, x, y);
				if (pOtherHandler != pHandler && pOtherHandler->dragInit(wp, x, y)) {
					pHandler = pOtherHandler;
				} else {
					pHandler = NULL;
				}
			}
			if (pHandler == NULL) {
				ReleaseCapture();
				return 0;
			}
		}
		cPrevious = c1.offset;
		lpPrevious = c1.linePointer;
		id = SetTimer(0, 0, 100, 0);
		for (;;) {
			if (!mouse_dispatchUntilButtonRelease(&x, &y, &b, &dummy)) {
				x = pStart.x;
				y = pStart.y;
				b = 0;
			}
			if (!b) {
				break;
			}
			caret_placeToXY(wp, x, y);
			if (ww_isColumnSelectionMode(wp)) {
				long ln;
				long col;
				wp->renderer->r_hitTest(wp, x, y, &ln, &col);
				wp->caret.offset = col;
			}
			c2 = wp->caret;
			if (cPrevious != c2.offset || lpPrevious != c2.linePointer) {
				cPrevious = c2.offset;
				lpPrevious = c2.linePointer;
				if (!pHandler->drag(wp, x, y)) {
					break;
				}
			}
		}
		if (id) {
			KillTimer(0, id);
		}
		if (pHandler->dragEnd) {
			pHandler->dragEnd(wp, x, y, x == pStart.x || y == pStart.y);
		}
		ReleaseCapture();
	}
	return 1;
}

/*
 * Execute the provided command/macro assuming the execution of the action was triggered
 * by the used with some kind of pointing interaction (clicking with the mouse or from a popup menu).
 * Return 1 if successful.
 */
int macro_executeWithPosition(WINFO* wp, MACROREF* pRef, POINT pt) {
	if (pRef->typ == CMD_CMDSEQ) {
		long long llParam;
		int nFunctionNum = macro_getFunctionNumberForCommand(pRef->index, &llParam);
		if (_functionTable[nFunctionNum].flags & EW_FINDCURS) {
			caret_placeToXY(wp, pt.x, pt.y);
		}
	}
	return (int)macro_executeMacro(pRef);
}

/*---------------------------------*/
/* mouse_executeBinding()					*/
/*---------------------------------*/
static int mouse_executeBinding(WINFO *wp, MOUSE_EVENT_BINDING *mp, int x, int y) {
	const char* pszLabel = bindings_getBoundText(&mp->msg);
	if (pszLabel) {
		error_displayErrorInToastWindow(pszLabel);
	}
	return macro_executeWithPosition(wp, &mp->macref, (POINT) { .x = x, .y = y });
}

/*------------------------------------------------------------
 * bindings_getMouseBinding()
 */
extern MOUSE_EVENT_BINDING* bindings_getMouseBinding(int nButton, int nShift, int nClicks, const char* pszActionContext);

/*----------------------------*/
/* mouse_onMouseClicked()			*/
/*----------------------------*/
EXPORT int mouse_onMouseClicked(WINFO *wp, int x, int y, int b, int nclicks, int nModifier)
{
	MOUSE_EVENT_BINDING *	mp = NULL;

	if (mp || (mp = bindings_getMouseBinding(b, nModifier, nclicks, wp->actionContext)) != 0) {
		recorder_stopAutoInsertRecording(NULL, NULL);
		mouse_executeBinding(wp, mp, x, y);
		return 1;
	}
	return 0;
}

