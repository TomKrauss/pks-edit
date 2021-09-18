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
#include "edfuncs.h"
#include "pksedit.h"
#include "functab.h"
#include "iccall.h" 
#include "resource.h"
#include "edfuncs.h"
#include "winutil.h"

static HCURSOR   hArrowCursor;	// Pointer cursor
static HCURSOR   hHandPointer;		// Hand pointer cursor
static HCURSOR   hHourGlass;		// Hour glass cursor
static HCURSOR   hDefaultCurs;

extern HWND 	ic_findChildFromPoint(HWND hwnd, POINT *point);

extern MOUSEBIND	_mousetab[MAXMAPMOUSE];

static RSCTABLE __m = {
	0,	"default",	sizeof _mousetab, 	_mousetab,
	(unsigned char *)_mousetab+sizeof(_mousetab)
};

RSCTABLE *_mousetables = &__m;

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
	if ((pData->c1.linePointer == pData->c2.linePointer && pData->c1.offset < pData->c2.offset) || 
		(pData->c1.linePointer != pData->c2.linePointer && pData->start.y < y)) {
		bl_syncSelectionWithCaret(wp, &pData->c1, MARK_START | MARK_NO_HIDE, NULL);
		bl_syncSelectionWithCaret(wp, &pData->c2, MARK_END | MARK_NO_HIDE, NULL);
	} else {
		bl_syncSelectionWithCaret(wp, &pData->c1, MARK_END | MARK_NO_HIDE, NULL);
		bl_syncSelectionWithCaret(wp, &pData->c2, MARK_START | MARK_NO_HIDE, NULL);
	}
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
	FTABLE* fp = wp->fp;
	HCURSOR 	hCursor;

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
		if (_dragTextBlockMoveData.moving) {
			ret = macro_executeFunction(FUNC_EdBlockMove, 0L, 0L, (void*)0, (void*)0, (void*)0);
		} else {
			ret = macro_executeFunction(FUNC_EdBlockCopy, 0L, 0L, (void*)0, (void*)0, (void*)0);
		}
	} else if (hwnd) {
		ret = PostMessage(hwnd, WM_ICONDROP, ICACT_TEXTBLOCK, 0L);
	}
	if (_dragTextBlockMoveData.saveCursor) {
		SetClassLongPtr(wp->ww_handle, /*does not work: GCL_HCURSOR*/ -12, (LONG_PTR)_dragTextBlockMoveData.saveCursor);
	}
	return 1;
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
 * Utility function to regex_addCharacterToCharacterClass the caret according to screen coordinates.
 */
static void caret_placeToXY(WINFO* wp, int x, int y) {
	long col, ln;

	caret_calculateOffsetFromScreen(wp, x, y, &ln, &col);
	if (caret_updateLineColumn(wp, &ln, &col, 1)) {
		wt_curpos(wp, ln, col);
	}
}

/*------------------------------------------------------------
 * mouse_getXYPos()
 * Get the current window relative mouse position.
 */
EXPORT void mouse_getXYPos(HANDLE hwnd, int *x, int *y)
{	POINT p;

	if (!hwnd)
		return;
		
	GetCursorPos(&p);
	ScreenToClient(hwnd,&p);
	*x = p.x;
	*y = p.y;
}

/*------------------------------------------------------------
 * mouse_dispatchUntilButtonRelease()
 * Dispatches mouse messages until a mouse button is released.
 * Return the current mouse position / shift state etc... in the passed
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
	FTABLE* fp = wp->fp;

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
			ReleaseCapture();
			return 0;
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
				caret_calculateOffsetFromScreen(wp, x, y, &ln, &col);
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

/*------------------------------------------------------------
 * EdMouseMoveText()
 * NOT USED ANY MORE
 */
EXPORT int EdMouseMoveText(int move)
{
	return 0;
}

/*--------------------------------------------------------------------------
 * mouse_hasEmptySlot()
 */
static int mouse_hasEmptySlot(MOUSEBIND *mp)
{
	return (mp->button == 0 && mp->nclicks == 0) ? 1 : 0;
}

/*------------------------------------------------------------
 * mouse_getMouseBind()
 */
static MOUSEBIND* mouse_getMouseBind(int nButton, int nShift, int nClicks)
{
	MOUSEBIND *mp;

	for (mp = (MOUSEBIND *)_mousetables->rt_data; 
		mp < (MOUSEBIND *)_mousetables->rt_end; mp++) {
		if (mp->button == nButton &&
		    mp->shift == nShift &&
		    mp->nclicks == nClicks) {
			return mp;
		}
	}
	return (MOUSEBIND*)0;
}

/*--------------------------------------------------------------------------
 * mouse_getMouseBindingForCode()
 */
MOUSEBIND *mouse_getMouseBindingForCode(MOUSECODE mcode)
{
	return mouse_getMouseBind(mcode.button, mcode.shift, mcode.nclicks);
}

/*-----------------------------------------------------------
 * mouse_getDefaultMouseBinding()
 */
MOUSEBIND *mouse_getDefaultMouseBinding(void)
{
 	MOUSEBIND 	*mp;

	if ((mp = mouse_getMouseBind(0, 0, 0)) != 0) {
		return mp;
	}

	return rsc_tableresize(_mousetables, sizeof *mp, 
		_mousetab, (int (*)(void *))mouse_hasEmptySlot);
}

/*---------------------------------*/
/* mouse_removeMouseBinding()				*/
/*---------------------------------*/
void mouse_removeMouseBinding(MOUSEBIND *mp)
{
	if (mp == 0)
		return;

	mp->button = 0;
	mp->nclicks = 0;
	mp->shift = 0;
	mp->macref.typ = CMD_NONE;
}

/*--------------------------------------------------------------------------
 * mouse_destroyMouseBindings()
 */
void mouse_destroyMouseBindings(void)
{
	RSCTABLE *		rp;

	if ((rp = _mousetables) != 0 && rp->rt_data) {
		memset(rp->rt_data, 0, (int)((char *)rp->rt_end - (char *)rp->rt_data));
	}
}

/*---------------------------------*/
/* mouse_deleteBindingsUpTo()			*/
/* delete all mouse bindings to a 	*/
/* for inst. single macro,...		*/
/*---------------------------------*/
void mouse_deleteBindingsUpTo(MACROREFTYPE typ, MACROREFIDX val)
{
	RSCTABLE *	rp;
	MOUSEBIND  *	mp;

	for (rp = _mousetables; rp; rp = rp->rt_next) {
		for (mp = (MOUSEBIND*)rp->rt_data; mp < (MOUSEBIND*)rp->rt_end; mp++) {
			if (typ == mp->macref.typ && val == mp->macref.index) {
				mouse_removeMouseBinding(mp);
			}
		}
	}
}

/*---------------------------------*/
/* mfunct()					*/
/*---------------------------------*/
static int mfunct(WINFO *wp, MOUSEBIND *mp, int x, int y)
{
	long 	ln;
	long 	col;

	if (mp->msg && mp->msg[0]) {
		error_displayErrorToast(mp->msg, NULL);
	}
	if (mp->macref.typ == CMD_CMDSEQ) {
		COM_1FUNC* cp;
		cp = &_cmdseqtab[mp->macref.index].c_functionDef;
		if (_edfunctab[cp->funcnum].flags & EW_FINDCURS) {
			caret_calculateOffsetFromScreen(wp, x, y, &ln, &col);
			if (caret_updateLineColumn(wp, &ln, &col, 1)) {
				wt_curpos(wp, ln, col);
			}
		}
	}

	return macro_executeMacro(&mp->macref);
}

/*----------------------------*/
/* mouse_onMouseClicked()			*/
/*----------------------------*/
EXPORT int mouse_onMouseClicked(WINFO *wp, int x, int y, int b, int nclicks, int shift)
{
	MOUSEBIND *	mp;

	if (shift & 0x3) {
		shift |= 0x3;
	}

	if ((mp = mouse_getMouseBind(b, shift, nclicks)) != 0) {
		macro_stopRecordingFunctions();
		mfunct(wp, mp, x, y);
		return 1;
	}
	return 0;
}

