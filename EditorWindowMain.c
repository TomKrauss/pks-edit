/*
 * EditorWindowMain.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: editor windows - painting etc...
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include "trace.h"
#include "caretmovement.h"
#include "edierror.h"
#include "edfuncs.h"
#include "errordialogs.h"
#include "winfo.h"
#include "winterf.h"
#include "pksedit.h"
#include "context.h"
#include "iccall.h"
#include "stringutil.h"
#include "editorconfiguration.h"
#include "desktopicons.h"
#include "propertychange.h"
#include "winutil.h"
#include "actions.h"
#include "themes.h"

#define	WT_WORKWIN		0
#define	WT_RULERWIN		1
#define	GWL_VIEWPTR		GWL_ICCLASSVALUES+sizeof(void*)

#define	PROF_OFFSET		1

typedef struct xywh {
	int x,y,w,h;
} XYWH;

static WINFO *_winlist;

extern int render_singleLineWithAttributesOnDevice(HDC hdc, int x, int y, WINFO* wp, LINE* lp);
extern long sl_thumb2deltapos(WINFO *wp, int horizontal, WORD thumb);
extern char *ft_visiblename(FTABLE *fp);
extern int  mouse_onRulerClicked(WINFO *fp, int x, int y, int msg, int shift);
extern int  mouse_onMouseClicked(WINFO *fp, int x,int y,int b, int nclicks,int shift);
extern void *icEditIconClass;
extern BOOL ic_isIconWindow(HWND hwnd);
extern void st_redraw(BOOL bErase);
extern void xref_selectFileFormat(char *tags);
extern void macro_selectDefaultBindings(void);
extern void SetMenuFor(char *pszContext);

static WINDOWPLACEMENT	_winstates[6];

static int lineNumberWindowWidth = 50;
static int rulerWindowHeight = 20;

/*-----------------------------------------------------------
 * ww_winstate()
 */
EXPORT void ww_winstate(int nId, WINDOWPLACEMENT *wsp)
{
	if (nId >= 0 && nId < DIM(_winstates) && _winstates[nId].length) {
		*wsp = _winstates[nId];
	} else {
		prof_getwinstate(szEditClass,nId,wsp);
	}
}

/*-----------------------------------------------------------
 * ww_savewinstates()
 * Save the state of the currently displayed editor windows for later restore.
 */
EXPORT void ww_savewinstates(void)
{	int nId;

	for (nId = 0; nId < DIM(_winstates); nId++) {
		if (_winstates[nId].length) {
			prof_savewinstate(szEditClass,nId,&_winstates[nId]);
		}
	}
}

/*------------------------------------------------------------
 * ww_createOrDestroyChildWindowOfEditor()
 */
static int ww_createOrDestroyChildWindowOfEditor(
	HWND  hwnd, 
	DWORD winFlags,
	BOOL  visible,
	HWND  *hwndChild,
	char  *szClass, 
	XYWH  *XYPxy,
	WINFO *wp
)
{
	if (visible == FALSE) {
		if (*hwndChild) {
			DestroyWindow(*hwndChild);
			*hwndChild = 0;
		}
		return 0;
	}

	if (*hwndChild) {
		return 1;
	}
	*hwndChild = CreateWindow( szClass, NULL,
		winFlags, XYPxy->x, XYPxy->y, XYPxy->w, XYPxy->h, 
		hwnd, NULL, hInst, (LPVOID)wp);
	return *hwndChild != 0;
}

/*-----------------------------------------------------------
 * ww_createSubWindows()
 */
static int ww_createSubWindows(HWND hwnd, WINFO *wp, XYWH *pWork, XYWH *pRuler, XYWH *pLineInfo) {
	RECT   	rect;
	BOOL	rulerVisible;
	BOOL	lineNumbersVisible;
	int		w;
	int		h;
	int		rh;
	int		rLineNumbers;

	if (wp == 0) {
		return 0;
	}

	GetClientRect(hwnd,&rect);
	w = rect.right-rect.left;
	h = rect.bottom-rect.top;

	rh = rulerWindowHeight;
	pRuler->x = 0;
	pRuler->w = w;
	pRuler->y = 0;
	pRuler->h = rh;

	rulerVisible = (h > rh && (wp->dispmode & SHOWRULER));
	if (!ww_createOrDestroyChildWindowOfEditor(hwnd,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		rulerVisible, &wp->ru_handle, szRulerClass, pRuler, wp)) {
		pRuler->h = 0;
	}

	rLineNumbers = lineNumberWindowWidth;
	pLineInfo->x = 0;
	pLineInfo->w = rLineNumbers;
	pLineInfo->y = pRuler->h;
	pLineInfo->h = h - pRuler->h;
	lineNumbersVisible = (w > rLineNumbers && (wp->dispmode & SHOWLINENUMBERS));
	if (!ww_createOrDestroyChildWindowOfEditor(hwnd,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		lineNumbersVisible, &wp->lineNumbers_handle, szLineNumbersClass, pLineInfo, wp)) {
		pLineInfo->w = 0;
	}


	pWork->x = pLineInfo->w;
	pWork->w = w - pLineInfo->w;
	pWork->y = pRuler->h;
	pWork->h = h - pRuler->h;
	return ww_createOrDestroyChildWindowOfEditor(hwnd,
		WS_HSCROLL|WS_VSCROLL|WS_CHILD|WS_VISIBLE/*|WS_CLIPSIBLINGS*/,
		TRUE,&wp->ww_handle,szWorkAreaClass,pWork, wp);
}

/*-----------------------------------------------------------
 * ww_snapPositionToCharGrid()
 */
static int ww_snapPositionToCharGrid(int x, int grid)
{
	if (grid <= 0)
		return 0;
	return (x + grid - 1) / grid;
}

/*------------------------------------------------------------
 * ww_updateRangeAndCheckBounds()
 * calculate scrollops checking bounds
 */
static void ww_updateRangeAndCheckBounds(long min, int cheight, int h,
				  long *max, long *maxcurs, long *mincurs, 
				  int *scroll_d)
{	int delta,d2;

	if (!cheight)
		return;
	*max = min + (delta = ww_snapPositionToCharGrid(h,cheight));
	delta /= 2;
	d2 = (h % cheight) ? 2 : 1;
	delta -= d2;
	if (delta < 0)
		delta = 1;
	if (*scroll_d > delta)
		*scroll_d = delta;
	*mincurs = (min < *scroll_d) ? min : min + *scroll_d;
	*maxcurs = *max - (*scroll_d + d2);

     if (*mincurs+*scroll_d > *maxcurs) {
     	*mincurs = *maxcurs = (*max-min) / 2;
		*scroll_d = 1;
	}
}

/*------------------------------------------------------------
 * ww_setScrollCheckBounds()
 * calculate scrollops checking bounds
 */
EXPORT void ww_setScrollCheckBounds(WINFO *wp)
{
	ww_updateRangeAndCheckBounds(wp->minln, wp->cheight, wp->workarea.g_h,
			&wp->maxln, &wp->maxcursln, &wp->mincursln, &wp->scroll_dy); 
			
	ww_updateRangeAndCheckBounds(wp->mincol, wp->cwidth, wp->workarea.g_w,
			&wp->maxcol, &wp->maxcurscol, &wp->mincurscol, &wp->scroll_dx); 
}

/*--------------------------------------------------------------------------
 * win_getstate()
 * Get the current window placement.
 */
EXPORT void win_getstate(HWND hwnd, WINDOWPLACEMENT *wsp)
{
	wsp->length = sizeof *wsp;
	GetWindowPlacement(hwnd, wsp);
}

/*--------------------------------------------------------------------------
 * ww_getstate()
 * Return the window placement state for a window.
 */
EXPORT void ww_getstate(WINFO *wp, WINDOWPLACEMENT *wsp)
{
	if (!wp) 
		return;
	win_getstate(wp->edwin_handle,wsp);
}

/*-----------------------------------------------------------
 * ww_hasColumnSelection()
 */
EXPORT int ww_hasColumnSelection(WINFO *wp)
{
	return (wp->workmode & BLK_COLUMN_SELECTION) ? 1 : 0;
}

/*------------------------------------------------------------
 * ww_findwinid2()
 */
EXPORT WINFO *ww_findwinid(int winid)
{	WINFO  *wp;
	
	for (wp = _winlist; wp; wp = wp->next) {
		if (wp->win_id == winid) {
			return wp;
		}
	}
	return NULL;
}

/*--------------------------------------------------------------------------
 * ww_workWinHasFocus()
 * return TRUE, when a work window has the input focus
 */
EXPORT BOOL ww_workWinHasFocus(void)
{
	if (!_winlist || _winlist->ww_handle != GetFocus()) {
		return FALSE;
	}
	return TRUE;
}

/*------------------------------------------------------------
 * ww_winid2hwnd()
 */
EXPORT HWND ww_winid2hwnd(int winid)
{	WINFO  *wp;

	return ((wp = ww_findwinid(winid)) == 0) ? NULL : wp->edwin_handle;
}

/*------------------------------------------------------------
 * ww_winid2fp()
 */
EXPORT FTABLE *ww_winid2fp(int winid)
{	WINFO  *wp;

	return ((wp = ww_findwinid(winid)) == 0) ? NULL : FTPOI(wp);
}

/*------------------------------------------------------------
 * ww_timer()
 * Trigger a timer action.
 */
EXPORT HWND ww_timer(void)
{
	st_redraw(FALSE);
	return NULL;
}

/*------------------------------------------------------------
 * ww_requestFocusInTopWindow()
 * Request focus in the top most window.
 */
void ww_requestFocusInTopWindow(void)
{
	if (_winlist) {
		SetFocus(_winlist->ww_handle);
	}
}

/*------------------------------------------------------------
 * ww_popup()
 * Bring a child to top - if iconized restore.
 */
EXPORT void ww_popup(HWND hwndChild)
{
	if (IsIconic(hwndFrame)) {
		ShowWindow(hwndFrame,SW_RESTORE);
	}
	BringWindowToTop(hwndFrame);

	if (IsIconic(hwndChild)) {
		SendMessage(hwndClient, WM_MDIRESTORE, (WPARAM) hwndChild, 0L);
	}
	SendMessage(hwndClient, WM_MDIACTIVATE, (WPARAM) hwndChild, 0L);
}

/*-----------------------------------------------------------
 * ww_getWindowFromStack()
 * get the num'th window from the top.
 */
WINFO *ww_getWindowFromStack(int num)
{	WINFO *wp;

	for (wp = _winlist; num > 0 && wp && wp->next; num--, wp = wp->next) 
		;
	if (num)
		return 0;
	return wp;
}

/*------------------------------------------------------------
 * EdWindowRegSet()
 */
int EdWindowRegSet(int num)
{	WINFO *wp;
	WINDOWPLACEMENT ws;

	if ((wp = _winlist) == 0) 
		return 0;
	ww_getstate(wp,&ws);
	prof_savewinstate("WinReg",num,&ws);
	error_showMessageInStatusbar(IDS_MSGWIREGDEF,num);
	return 1;
}

/*------------------------------------------------------------
 * ww_arrangeIcons()
 */
static void ww_arrangeIcons(void)
{
	int		nMax;
	int		nIconHeight;
	int		nIconWidth;
	int		nXMax;
	int		nYMax;
	int		nIndex;
	int		nStartIndex;
	char *	pszSlots;
	HWND	hwnd;
	RECT	rMax;
	RECT 	r;

	GetClientRect(hwndClient, &rMax);
	nIconWidth = GetSystemMetrics(SM_CXICONSPACING);
	nIconHeight = GetSystemMetrics(SM_CYICONSPACING);
	nXMax = rMax.right / nIconWidth;
	nYMax = rMax.bottom / nIconHeight;
	if (nXMax < 1) {
		nXMax = 1;
	}
	if (nYMax < 1) {
		nYMax = 1;
	}
	nMax = nXMax * nYMax;
	pszSlots = (char *)calloc(sizeof *pszSlots, nMax);
	for (hwnd = GetWindow(hwndClient, GW_CHILD); hwnd != 0; hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
		if (!ic_isIconWindow(hwnd)) {
			continue;
		}
		GetWindowRect(hwnd,&r);
		ScreenToClient(hwndClient,(POINT*)&r);
		r.left = r.left / nIconWidth;
		r.top = r.top / nIconHeight;
		nIndex = r.top * nXMax + r.left;
		if (nIndex < 0 || nIndex >= nMax) {
			nIndex = 0;
		}
		nStartIndex = nIndex; 
		do {
			if (!pszSlots[nIndex]) {
				break;
			}
			nIndex++;
			if (nIndex >= nMax) {
				nIndex = 0;
			}
		} while(nIndex != nStartIndex);
		pszSlots[nIndex] = 1;
		ic_moveIcon(hwnd, (nIndex % nXMax) * nIconWidth + nIconWidth / 2, 
				(nIndex / nXMax) * nIconHeight + nIconHeight / 2);
	}
	free(pszSlots);
}

/*--------------------------------------------------------------------------
 * ww_enableNotTopmostWindow()
 */
static void ww_enableNotTopmostWindow(int nTopMost, BOOL bHow)
{
	int		nWindow;
	WINFO *	wp;

	for (wp = _winlist, nWindow = 0; wp; nWindow++, wp = wp->next) {
		if (nWindow >= nTopMost) {
			EnableWindow(wp->edwin_handle, bHow);
		}
	}
}

/*------------------------------------------------------------
 * EdWinArrange()
 */
int EdWinArrange(int func)
{
	WINFO *	wp = _winlist;
	WORD  	message;
	HWND  	hwnd;
	WINDOWPLACEMENT	windowplacement;
	WPARAM	wParam;
	LRESULT	ret;

	hwnd = wp ? wp->edwin_handle : hwndFrame;
	wParam = 0;
	switch(func) {
		case WIN_FULL:
			ShowWindow(hwnd,IsZoomed(hwnd) ?
				SW_SHOWNORMAL : SW_SHOWMAXIMIZED); return 1;
		case WIN_TILE:
			message = WM_MDITILE;
			wParam = MDITILE_VERTICAL;
			break;
		case WIN_PAIRS:
			message = WM_MDITILE;
			wParam = MDITILE_SKIPDISABLED;
			ww_enableNotTopmostWindow(2, FALSE);
			break;
		case WIN_HOR:
			message = WM_MDITILE;
			wParam = MDITILE_HORIZONTAL;
			break;
		case WIN_CASCADE:
			message = WM_MDICASCADE; 
			break;
		case WIN_ICONARRANGE: 
			message = WM_MDIICONARRANGE; 
			break;
		case WIN_DESKICON: 		
			hwnd = hwndFrame;
		case WIN_ICONIZED: 		
			ShowWindow(hwnd,IsIconic(hwnd) ?
				SW_RESTORE : SW_SHOWMINIMIZED); return 1;
		default: 
			if (prof_getwinstate("WinReg",(int)func,&windowplacement)) {
				SetWindowPlacement(hwnd, &windowplacement);
			} else {
				error_showErrorById(IDS_MSGUNDEFWINREG);
			}
			return 1;
	}

	ShowWindow(hwndClient,SW_HIDE);
	if (func == WIN_ICONARRANGE) {
		ww_arrangeIcons();
	}
	ret = SendMessage(hwndClient, message, wParam, 0L);
	ShowWindow(hwndClient,SW_SHOW);

	if (func == WIN_PAIRS) {
		ww_enableNotTopmostWindow(2, TRUE);
	}
	return (int) ret;
}

/*-----------------------------------------------------------
 * ww_redrawAllWindows()
 * Redraw all editor windows. If update is true, not only send
 * a repaint message bu also update by painting right away.
 */
void ww_redrawAllWindows(int update)
{	WINFO *wp;

	for (wp = _winlist; wp; wp = wp->next) {
		win_sendRedrawToWindow(wp->ww_handle);
		if (update) {
			UpdateWindow(wp->ww_handle);
		}
	}
}

/*-----------------------------------------------------------
 * ww_setwindowtitle()
 * Update the title of a window.
 */
void ww_setwindowtitle(WINFO *wp)
{	int nr;
	char buf[512];

	if (!wp->edwin_handle)
		return;
	FTABLE* fp = wp->fp;
	nr = wp->win_id;
	char* pName = ft_visiblename(fp);
	if (IsIconic(wp->edwin_handle)) {
		wsprintf(buf,"%d %s",nr,(LPSTR)string_getBaseFilename(pName));
	} else {
		wsprintf(buf,"#%d %s",nr,(LPSTR)pName);
	}
	if (fp->flags & F_MODIFIED) {
		memmove(buf + 2, buf, sizeof buf - 2);
		buf[0] = '*';
		buf[1] = ' ';
	}
	SetWindowText(wp->edwin_handle,buf);
}

/**
 * A property of our editor document has changed. Update the window appropriately.
 */
void ww_documentPropertyChanged(WINFO* wp, PROPERTY_CHANGE* pChange) {
	ww_setwindowtitle(wp);
}

/*-----------------------------------------------------------
 * ww_applyDisplayProperties()
 * Apply all flags from the document descriptor of the edited document on the 
 * window referred to by the passed pointer.
 */
void ww_applyDisplayProperties(WINFO *wp) {
	FTABLE* fp = wp->fp;
	DOCUMENT_DESCRIPTOR *linp = fp->documentDescriptor;

	if (linp->dispmode != wp->dispmode) {
		wp->dispmode = linp->dispmode;
	}
	wp->renderFunction = (wp->dispmode & SHOWATTR) ? render_singleLineWithAttributesOnDevice : render_singleLineOnDevice;

	memmove(&wp->fnt, &linp->fnt, sizeof wp->fnt);
	if (wp->fnt.fgcolor == wp->fnt.bgcolor) {
		wp->fnt.fgcolor = RGB(0,0,0);
		wp->fnt.bgcolor = RGB(255,255,255);
	}

	wp->fnt_handle = 0;
	if (wp->ww_handle) {
		sl_size(wp);
		font_selectStandardFont(wp->ww_handle, wp);
		win_sendRedrawToWindow(wp->ww_handle);
		wt_tcursor(wp,0);
		wt_tcursor(wp,1);
		caret_placeCursorForFile(fp,fp->ln,fp->caret.offset);
	}

	wp->workmode = linp->workmode;
	wp->tfillchar = linp->t1;
	wp->statusline = linp->statusline;
	wp->win_themeName = "default";
	wp->scrollflags = linp->scrollflags;
	wp->cursaftersearch = linp->cursaftersearch;
	wp->vscroll = linp->vscroll;
	wp->scroll_dy = linp->scroll_dy;
	ww_setScrollCheckBounds(wp);
	render_updateCaret(wp);
}

/*-----------------------------------------------------------
 * ww_new()
 */
static int nwindows;
static FSTYLE _fstyles[2] = {
	5,			BLACK_BRUSH,		/* Or */
	5,			BLACK_BRUSH		/* Invert */
};
static WINFO *ww_new(FTABLE *fp,HWND hwnd)
{	WINFO  *wp;

	if ((wp = (WINFO*)ll_insert((LINKED_LIST**)&_winlist,sizeof *wp)) == 0) {
		return 0;
	}

	wp->markstyles[FS_XMARKED] = _fstyles[0];
	wp->markstyles[FS_BMARKED] = _fstyles[1];
	fp->wp = wp;
	wp->fp = fp;
	wp->edwin_handle = hwnd;

	ww_applyDisplayProperties(wp);
		 
	wp->hscroll   = 4;
	wp->scroll_dx = 4;

	wp->win_id = ++nwindows;

	return wp;
}

/*-----------------------------------------------------------
 * ww_destroy()
 */
void ww_destroy(WINFO *wp)
{	int   nId;

	if (wp == NULL) {
		return;
	}
	// If last window of FP
	if (wp->fp != NULL) {
		ft_destroy(wp->fp);
	}
	wp->fp = NULL;
	nId = wp->win_id;
	if (!ll_delete((LINKED_LIST**)&_winlist,wp)) {
		EdTRACE(log_errorArgs(DEBUG_ERR,"failed deleting window props"));
		;
	}

	/* update window handles */
	for (wp = _winlist; wp; wp = wp->next) {
		if (wp->win_id > nId) {
			wp->win_id--;
			ww_setwindowtitle(wp);
		}
	}
	nwindows--;
	EdTRACE({
		if (nwindows == 0 && _winlist)
			log_errorArgs(DEBUG_ERR,"bad winlist");
	});
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
}

/*-----------------------------------------------------------
 * ww_getNumberOfOpenWindows()
 * Returns the number of currently open windows.
 */
int ww_getNumberOfOpenWindows(void)
{
	return nwindows;
}

/*------------------------------------------------------------
 * ww_close()
 * Close the passed editor window.
 */
int ww_close(WINFO *wp)
{	HWND hwndEdit;

	hwndEdit = wp->edwin_handle;

	if (!SendMessage(hwndEdit,WM_QUERYENDSESSION,(WPARAM)0,(LPARAM)0L))
		return 0;
	return
		(int) SendMessage(hwndClient,WM_MDIDESTROY,(WPARAM)hwndEdit,(LPARAM)0L);
}

/*------------------------------------------------------------
 * EditWndProc()
 */
WINFUNC EditWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	WINDOWPLACEMENT 	ws;
	WINFO * wp = (WINFO*)GetWindowLongPtr(hwnd, GWL_VIEWPTR);

	if (message == WM_CREATE || wp != NULL)
   	switch(message) {
	case WM_CREATE:
		{
		LPCREATESTRUCT    	cp;
		XYWH 		   		xyWork,xyRuler,xyLineInfo;
		FTABLE* fp;

		cp = (LPCREATESTRUCT)lParam;
		fp = (FTABLE*)((LPMDICREATESTRUCT)cp->lpCreateParams)->lParam;
		if ((wp = ww_new(fp, hwnd)) == 0) {
			DestroyWindow(hwnd);
			return 0;
		}

		ShowWindow(hwnd, SW_HIDE);
		ww_createSubWindows(hwnd, wp, &xyWork, &xyRuler, &xyLineInfo);
		ww_setwindowtitle(wp);
		SetWindowLongPtr(hwnd,GWL_ICPARAMS, (LONG_PTR)fp->fname);
		SetWindowLongPtr(hwnd,GWL_ICCLASSVALUES,(LONG_PTR) icEditIconClass);
		SetWindowLongPtr(hwnd,GWL_VIEWPTR, (LONG_PTR) wp);
		return 0;
		}

	case WM_ICONCLASSVALUE:
		return GetWindowLongPtr(hwnd,GWL_ICCLASSVALUES);

	case WM_ICONDROP:
		ww_popup(hwnd);
		return macro_onIconAction(hwnd, wParam, lParam);

	case WM_MDIACTIVATE:
		/* EdSwitchContext(hwnd,LOWORD(lParam),CTX_EDIT); */
		if (wParam == 0)
			break;
	case WM_SETFOCUS:
		ll_moveElementToFront((LINKED_LIST**)&_winlist,wp);
		if (!IsIconic(hwnd))
			SetFocus(wp->ww_handle);
		break;
	case WM_MOVE:
		/* drop through */
	case WM_SIZE:
		if (wParam == SIZEZOOMHIDE || wParam == SIZEZOOMSHOW) {
			break;
		}
		ww_getstate(wp,&ws);
		if (ws.showCmd & SW_SHOWMINIMIZED) {
			SetFocus(hwndFrame);
		} else {
			if ((wp->dispmode & SHOWFIXEDWI) == 0 &&
			    wp->win_id < DIM(_winstates) &&
			    wp->win_id >= 0)
				_winstates[wp->win_id] = ws;
		}
		if (message == WM_MOVE)
			break;
		ww_setwindowtitle(wp);
		if (wParam == SIZEICONIC) {
			break;
		}

	case WM_EDWINREORG:
		{
		XYWH xyWork, xyRuler, xyLineWindowSize;

		ww_createSubWindows(hwnd, wp,&xyWork,&xyRuler, &xyLineWindowSize);
		if (wp->ww_handle) {
			MoveWindow(wp->ww_handle,xyWork.x,xyWork.y,
					xyWork.w,xyWork.h,1);
		}
		if (wp->ru_handle) {
			MoveWindow(wp->ru_handle,xyRuler.x,xyRuler.y,
					xyRuler.w,xyRuler.h,1);
		}
		if (wp->lineNumbers_handle) {
			MoveWindow(wp->lineNumbers_handle, xyLineWindowSize.x, xyLineWindowSize.y,
				xyLineWindowSize.w, xyLineWindowSize.h, 1);
		}
		if (message == WM_EDWINREORG) {
			if (wp->ru_handle) {
				win_sendRedrawToWindow(wp->ru_handle);
			}
			return 1;
		}
		if (wp->ru_handle) {
			RedrawWindow(wp->ru_handle, NULL, NULL, RDW_INVALIDATE);
		}
		break;
		}
	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if (IsZoomed(hwnd))
			SendMessage(hwndClient,WM_MDIRESTORE, (WPARAM)hwnd, (LPARAM)0);
		if (!ft_requestToClose(wp->fp))
			return 0;
 		break;

	case WM_DESTROY:
		// Destroy the view window - the document should be destroyed only if this is the last window.
		ww_destroy(wp);
		if (!ww_getNumberOfOpenWindows()) {
			SetMenuFor("initial");
		}
#if defined(DEBUG_MEMORY_USAGE)
          {
		static	char			buf[100];
		wsprintf(buf, "%ld frags remaining", dumpallocated());
		MessageBox(hwndFrame, buf, szAppName, MB_APPLMODAL|MB_OK);
		} 
#endif
		return 0;
    }

    return DefMDIChildProc(hwnd, message, wParam, lParam);
}

/*------------------------------------------------------------
 * ww_updateWindowBounds()
 */
static int ww_updateWindowBounds(WINFO *wp, int w, int h) {
	wp->workarea.g_x = wp->workarea.g_y = 0;
	wp->workarea.g_w = w;
	wp->workarea.g_h = h;

	ww_setScrollCheckBounds(wp);

	EdTRACE(log_errorArgs(DEBUG_TRACE,"SetWiSize to (%ld,%ld,%ld,%ld)",
		   wp->minln,wp->mincol,wp->maxln,wp->maxcol));
	return 1;
}

/*----------------------------------------------------------
 * do_mouse()
 */
int do_mouse(HWND hwnd, int nClicks, UINT message, WPARAM wParam, LPARAM lParam)
{
	int			ret;
	int			button;
	int			shift;
	int			x;
	int			y;
	WINFO *		wp;

	if ((wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)) == 0) {
		return 0;
	}
	if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
		x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
	} else {
		return 0;
	}

	if (wParam & MK_RBUTTON) {
		button = 2;
	} else if (wParam & MK_MBUTTON) {
		button = 3;
	} else {
		button = 1;
	}

	shift = 0;
	if (wParam & MK_CONTROL) {
		shift |= 4;
	}
	if (wParam & MK_SHIFT) {
		shift |= 2;
	}
	if (GetKeyState(VK_MENU) < 0) {
		shift |= 8;
	}
	ret = mouse_onMouseClicked(wp, x, y, button, nClicks, shift);
	return ret;
}

/*------------------------------------------------------------
 * do_slide()
 */
int do_slide(WINFO *wp, UINT message, WPARAM wParam, LPARAM lParam) {	
	int 	delta;
	int		deltapage;
	int		nScrollCode;
	int		nScrollPos;

#if defined(WIN32)
	nScrollCode = LOWORD(wParam);
	nScrollPos = HIWORD(wParam);
#else
	nScrollCode = wParam;
	nScrollPos = LOWORD(lParam);
#endif
	if (message == WM_HSCROLL) {
		deltapage = (wp->maxcol - wp->mincol);
	} else {
		deltapage = (wp->maxln - wp->minln);
	}
	
	switch(nScrollCode) {
		case SB_LINEUP:	delta = -1; break;
		case SB_LINEDOWN:	delta =  1; break;
		case SB_PAGEUP:	delta = -deltapage; break;
		case SB_PAGEDOWN:	delta =  deltapage; break;
		case SB_THUMBTRACK:
			if ((wp->scrollflags & SC_THUMBTRACK) == 0)
				return 0;
		case SB_THUMBPOSITION:	
			delta = nScrollPos -
					((message == WM_VSCROLL) ? 
						wp->minln : wp->mincol);
			break;
		default: return 0;
	}
	
	sl_moved(wp,(message == WM_HSCROLL) ? 0L : (long)delta,
			  (message == WM_HSCROLL) ? (long)delta : 0L, 
			  (wp->scrollflags & SC_CURSORCATCH));
	return 1;
}

/*
 * Button down handler, which will helps listening to tripple clicks.
 */
static void onButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static RECT _lastMouseClickRect = {0};
	static DWORD _tmLastClick = 0;
	static int _cClicks = 0;
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	POINT pt = { x, y };

	DWORD tmClick = GetMessageTime();
	if (message == WM_LBUTTONDBLCLK || message == WM_RBUTTONDBLCLK || message == WM_MBUTTONDBLCLK) {
		_cClicks = 1; 
	} else if (!PtInRect(&_lastMouseClickRect, pt) ||
		tmClick - _tmLastClick > GetDoubleClickTime()) {
		_cClicks = 0;
	}
	_cClicks++;

	_tmLastClick = tmClick;
	SetRect(&_lastMouseClickRect, x, y, x, y);
	InflateRect(&_lastMouseClickRect,
		GetSystemMetrics(SM_CXDOUBLECLK) / 2,
		GetSystemMetrics(SM_CYDOUBLECLK) / 2);
	do_mouse(hwnd, _cClicks, message, wParam, lParam);
}

/*------------------------------------------------------------
 * WorkAreaWndProc()
 */
static WINFUNC WorkAreaWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam) {
	WINFO *	wp;
	int zDelta;

	switch(message) {
	case WM_CREATE:
		{
		wp = (WINFO *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, 0, (LONG_PTR) wp);
		font_selectStandardFont(hwnd, wp);
		return 0;
		}

	case WM_INITMENUPOPUP:
	case WM_MENUSELECT:
	case WM_COMMAND:
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_SYSKEYUP:
	case WM_CHAR:
	case WM_KEYUP:
		return SendMessage(hwndFrame,message,wParam,lParam);

	case WM_MOUSEWHEEL:
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if ((wp = (WINFO*)GetWindowLongPtr(hwnd, 0)) != 0) {
			long dx = 0;
			long dy = zDelta > 0 ? -1 : 1;
			sl_scrollwinrange(wp, &dy, &dx);
			sl_winchanged(wp, dy, dx);
		}
		return TRUE;

	case WM_LBUTTONDOWN:
		if (GetFocus() != hwnd) {
			SetFocus(hwnd);
		}
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		onButtonDown(hwnd, message, wParam, lParam);
		return 0;

	case WM_HSCROLL:
	case WM_VSCROLL:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
			if (do_slide(wp,message,wParam,lParam) == 0) {
				break;
			}
		} else {
			EdTRACE(log_errorArgs(DEBUG_TRACE,"WM_SLIDE in WorkWndProc without file"));
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_PAINT:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
		   render_paintWindow(wp);
		}
		return 0;

	case WM_SIZE:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
			if (!ww_updateWindowBounds(wp, LOWORD(lParam), HIWORD(lParam))) {
				return 0;
			}
			wp->ww_handle = hwnd;
			sl_size(wp);
	    }
	    break;

	case WM_SETFOCUS:
	    if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
			wt_tcursor(wp,1);
			ft_select(wp->fp);
			FTABLE* fp = wp->fp;
			xref_selectFileFormat(fp->documentDescriptor->modename);
			macro_selectDefaultBindings();
	    }
	    else {
			EdTRACE(log_errorArgs(DEBUG_TRACE,
				"WM_SETFOCUS in WorkWndProc without file"));
	    }
	    return 0;

	case WM_KILLFOCUS:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
			if (wp)
				wt_tcursor(wp,0);
		} else {
			EdTRACE(log_errorArgs(DEBUG_TRACE,"WM_KILLFOCUS in WorkWndProc without file"));
		}
		return 0;

	case WM_DESTROY:
		return 0;

    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * draw_ruler()
 * Does the actual painting of the ruler.
 */
static void draw_ruler(WINFO *wp) {
	int 			col;
	int			xPos;
	int			rmargin;
	int			width;
	int			nMiddle;
	RECT			rect;
	FTABLE *		fp = wp->fp;
	DOCUMENT_DESCRIPTOR *pDescriptor = fp->documentDescriptor;
	HDC 		hdc;
	PAINTSTRUCT ps;
	THEME_DATA* pTheme = theme_getByName(wp->win_themeName);

	memset(&ps, 0, sizeof ps);
	hdc = BeginPaint(wp->ru_handle, &ps);
	GetClientRect(wp->ru_handle, &rect);
	HBRUSH bgBrush = CreateSolidBrush(pTheme->th_rulerBackgroundColor);
	FillRect(hdc, &ps.rcPaint, bgBrush);
	DeleteObject(bgBrush);
	nMiddle = rect.bottom / 2 - 3;
	width = wp->cwidth;
	rmargin = ft_getRightMargin(fp);

	HPEN fatMarkerPen = CreatePen(PS_SOLID, 3, pTheme->th_rulerForegroundColor);
	HPEN markerPen = CreatePen(PS_SOLID, 1, pTheme->th_rulerForegroundColor);
	HPEN hPenOld = SelectObject(hdc, markerPen);
	xPos = wp->lineNumbers_handle ? lineNumberWindowWidth : 0;
	int xMin = ps.rcPaint.left - 20;
	int xMax = ps.rcPaint.right + 20;
	for (col = wp->mincol; col <= wp->maxcol; col++, xPos += width) {
		if (xPos < xMin || xPos > xMax) {
			continue;
		}
		if (TABTHERE(pDescriptor,col) || col == pDescriptor->lmargin || col == pDescriptor->rmargin) {
			MoveTo(hdc, xPos, nMiddle - 3);
			if (col == pDescriptor->lmargin || col == rmargin) {
				SelectObject(hdc, fatMarkerPen);
			}
			else {
				SelectObject(hdc, markerPen);
			}
			LineTo(hdc, xPos, nMiddle + 3);
		}
	}
	HPEN borderPen = CreatePen(PS_SOLID, 1, pTheme->th_rulerBorderColor);
	SelectObject(hdc, borderPen);
	MoveTo(hdc, rect.left, rect.bottom-1);
	LineTo(hdc, rect.right, rect.bottom-1);
	SelectObject(hdc, hPenOld);
	DeleteObject(fatMarkerPen);
	DeleteObject(borderPen);
	DeleteObject(markerPen);
	EndPaint(wp->ru_handle,&ps);
}

/*------------------------------------------------------------
 * RulerWndProc()
 * Window Procedure for handling the ruler.
 */
static WINFUNC RulerWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
) {
	WINFO* wp;

	switch(message) {

	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT:
			if ((wp = (WINFO *)GetWindowLongPtr(hwnd,0)) != 0) {
		   		draw_ruler(wp);
			} else {
				EdTRACE(log_errorArgs(DEBUG_TRACE,"WM_PAINT in RulerWndProc without file"));
			}
			break;

		case WM_CREATE:
			wp = (WINFO*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtr(hwnd, 0, (LONG_PTR)wp);
			return 0;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
				mouse_onRulerClicked(wp, (int)LOWORD(lParam), (int)HIWORD(lParam), 
					(int)message, (int)wParam);
			}
			return 0;

		case WM_DESTROY:
			return 0;
	}
	return (DefWindowProc(hwnd, message, wParam, lParam));
}

/*--------------------------------------------------------------------------
 * draw_LineNumbers()
 * Does the painting of the line numbers.
 */
static void draw_lineNumbers(WINFO* wp) {
	int 		row;
	int			yPos;
	RECT			rect;
	size_t		textLen;
	FTABLE* fp = wp->fp;
	int maxln = wp->maxln;
	DOCUMENT_DESCRIPTOR* lin = fp->documentDescriptor;
	HDC 		hdc;
	PAINTSTRUCT ps;
	HFONT		saveFont;
	THEME_DATA* pTheme = theme_getByName(wp->win_themeName);

	hdc = BeginPaint(wp->lineNumbers_handle, &ps);
	GetClientRect(wp->lineNumbers_handle, &rect);
	HBRUSH bgBrush = CreateSolidBrush(pTheme->th_rulerBackgroundColor);
	FillRect(hdc, &ps.rcPaint, bgBrush);
	DeleteObject(bgBrush);

	saveFont = font_selectDefaultEditorFont(wp, hdc,NULL);
	SetTextColor(hdc, pTheme->th_rulerForegroundColor);
	SetBkMode(hdc, TRANSPARENT);
	int padding = 3;
	char text[20];
	if (maxln > fp->nlines-1) {
		maxln = fp->nlines-1;
	}
	for (yPos = rect.top, row = wp->minln; row <= maxln && yPos < rect.top+rect.bottom; row++, yPos += wp->cheight) {
		sprintf(text, "%d:", row + 1);
		RECT textRect;
		textRect.left = rect.left + padding;
		textRect.right = rect.right - padding;
		textRect.top = yPos;
		textRect.bottom = yPos + wp->cheight;
		textLen = strlen(text);
		DrawText(hdc, text, (int)textLen, &textRect, DT_RIGHT|DT_END_ELLIPSIS);
	}
	HPEN markerPen = CreatePen(PS_SOLID, 1, pTheme->th_rulerBorderColor);
	HPEN hPenOld = SelectObject(hdc, markerPen);
	MoveTo(hdc, rect.right-1, rect.top);
	LineTo(hdc, rect.right-1, rect.bottom);
	SelectObject(hdc, hPenOld);
	DeleteObject(markerPen);
	font_selectSystemFixedFont(hdc);
	EndPaint(wp->lineNumbers_handle, &ps);
}

/*------------------------------------------------------------
 * LineNumberWndProc()
 * Window Procedure for handling the ruler.
 */
static WINFUNC LineNumberWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
) {
	WINFO* wp;

	switch (message) {

	case WM_ERASEBKGND:
		return 1;

	case WM_PAINT:
		if ((wp = (WINFO*)GetWindowLongPtr(hwnd, 0)) != 0) {
			draw_lineNumbers(wp);
		}
		else {
			EdTRACE(log_errorArgs(DEBUG_TRACE, "WM_PAINT in LineNumberProc without WINFO pointer"));
		}
		break;

	case WM_CREATE:
		wp = (WINFO*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)wp);
		return 0;

	case WM_DESTROY:
		return 0;
	}
	return (DefWindowProc(hwnd, message, wParam, lParam));
}

/*-----------------------------------------------------------
 * ww_register()
 * Register the window classes for PKS edit editor windows.
 */
int ww_register(void)
{
	if (!win_registerWindowClass(szWorkAreaClass, WorkAreaWndProc,
		(LPSTR)IDC_IBEAM, GetStockObject(WHITE_BRUSH), 0,
		sizeof(void*)) ||
		!win_registerWindowClass(szEditClass, EditWndProc,
			(LPSTR)IDC_ARROW, NULL, "Edit",
			GWL_VIEWPTR + sizeof(void*)) ||
		!win_registerWindowClass(szRulerClass, RulerWndProc,
			(LPSTR)IDC_CROSS, NULL, NULL,
			sizeof(void*)) ||
		!win_registerWindowClass(szLineNumbersClass, LineNumberWndProc,
			(LPSTR)IDC_ARROW, NULL, NULL,
			sizeof(void*))
#if 0
		||
		!win_registerWindowClass(szStatusClass, StatusWndProc,
			(LPSTR)IDC_ARROW, NULL, 0,
			2 * sizeof(LONG))
#endif
		) {
		return 0;
	}
	return 1;
}

