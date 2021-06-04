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
#include <stdio.h>
#include "trace.h"
#include "caretmovement.h"
#include "edierror.h"
#include "errordialogs.h"
#include "winfo.h"
#include "winterf.h"
#include "pksedit.h"
#include "context.h"
#include "iccall.h"
#include "stringutil.h"
#include "editorconfiguration.h"
#include "desktopicons.h"

#define	WT_WORKWIN		0
#define	WT_RULERWIN		1
#define	GWL_VIEWPTR		GWL_ICCLASSVALUES+sizeof(void*)

#define	PROF_OFFSET		1

typedef struct xywh {
	int x,y,w,h;
} XYWH;

static WINFO *_winlist;

/*--------------------------------------------------------------------------
 * mkattlist()
 */
extern void mkattlist(LINE* lp);
extern int mac_onIconAction(HWND icHwnd, WPARAM wParam,  LPARAM dropped);
extern long sl_thumb2deltapos(WINFO *wp, int horizontal, WORD thumb);
extern char *ft_visiblename(FTABLE *fp);
extern int  mouse_onRulerClicked(WINFO *fp, int x, int y, int msg, int shift);
extern int  mouse_onMouseClicked(FTABLE *fp, int x,int y,int b, int nclicks,int shift);
extern void *icEditIconClass;
extern BOOL ic_isIconWindow(HWND hwnd);
extern void st_redraw(BOOL bErase);
extern void xref_selectFileFormat(char *tags);
extern void mac_switchtodefaulttables(void);
extern void SetMenuFor(char *pszContext);
#if defined(DEBUG_MEMORY_USAGE)
extern long dumpallocated(void);
#endif

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
 * MakeOrKillEditChild()
 */
static int MakeOrKillEditChild(
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
 * MakeSubWis()
 */
static int MakeSubWis(HWND hwnd, WINFO *wp, XYWH *pWork, XYWH *pRuler, XYWH *pLineInfo) {
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
	if (!MakeOrKillEditChild(hwnd,
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
	if (!MakeOrKillEditChild(hwnd,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		lineNumbersVisible, &wp->lineNumbers_handle, szLineNumbersClass, pLineInfo, wp)) {
		pLineInfo->w = 0;
	}


	pWork->x = pLineInfo->w;
	pWork->w = w - pLineInfo->w;
	pWork->y = pRuler->h;
	pWork->h = h - pRuler->h;
	return MakeOrKillEditChild(hwnd,
		WS_HSCROLL|WS_VSCROLL|WS_BORDER|WS_CHILD|WS_VISIBLE/*|WS_CLIPSIBLINGS*/,
		TRUE,&wp->ww_handle,szWorkAreaClass,pWork, wp);
}

/*-----------------------------------------------------------
 * chRound()
 */
static int chRound(int x, int grid)
{
	if (grid <= 0)
		return 0;
	return (x + grid - 1) / grid;
}

/*------------------------------------------------------------
 * SetBounds()
 * calculate scrollops checking bounds
 */
static void SetBounds(long min, int cheight, int h,
				  long *max, long *maxcurs, long *mincurs, 
				  int *scroll_d)
{	int delta,d2;

	if (!cheight)
		return;
	*max = min + (delta = chRound(h,cheight));
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
 * SetScrollCheckBounds()
 * calculate scrollops checking bounds
 */
EXPORT void SetScrollCheckBounds(WINFO *wp)
{
	SetBounds(wp->minln, wp->cheight, wp->workarea.g_h,
			&wp->maxln, &wp->maxcursln, &wp->mincursln, &wp->scroll_dy); 
			
	SetBounds(wp->mincol, wp->cwidth, wp->workarea.g_w,
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
 */
EXPORT void ww_getstate(WINFO *wp, WINDOWPLACEMENT *wsp)
{
	if (!wp) 
		return;
	win_getstate(wp->edwin_handle,wsp);
}

/*-----------------------------------------------------------
 * ww_blkcolomn()
 */
EXPORT int ww_blkcolomn(WINFO *wp)
{
	return (wp->workmode & BLK_COLOMN) ? 1 : 0;
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
 * ww_havefocus()
 * return TRUE, when a work window has the input focus
 */
EXPORT BOOL ww_havefocus(void)
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
 */
EXPORT HWND ww_timer(void)
{
	st_redraw(FALSE);
	return NULL;
}

/*------------------------------------------------------------
 * ww_setfocus()
 */
void ww_setfocus(void)
{
	if (_winlist) {
		SetFocus(_winlist->ww_handle);
	}
}

/*------------------------------------------------------------
 * ww_toppostmessage()
 */
EXPORT int ww_toppostmessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	ww_setfocus();
	return PostMessage(hwndFrame,message,wParam,lParam);
}

/*------------------------------------------------------------
 * ww_popup()
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
 * ww_stackwi()
 * get the num'th window from the top.
 */
WINFO *ww_stackwi(int num)
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
 * ArrangeIcons()
 */
static void ArrangeIcons(void)
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
 * EnableNotTopmost()
 */
static void EnableNotTopmost(int nTopMost, BOOL bHow)
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
			EnableNotTopmost(2, FALSE);
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
		ArrangeIcons();
	}
	ret = SendMessage(hwndClient, message, wParam, 0L);
	ShowWindow(hwndClient,SW_SHOW);

	if (func == WIN_PAIRS) {
		EnableNotTopmost(2, TRUE);
	}
	return (int) ret;
}

/*-----------------------------------------------------------
 * redrawallwi()
 */
void redrawallwi(int update)
{	WINFO *wp;

	for (wp = _winlist; wp; wp = wp->next) {
		render_sendRedrawToWindow(wp->ww_handle);
		if (update) {
			UpdateWindow(wp->ww_handle);
		}
	}
}

/*-----------------------------------------------------------
 * ww_setwindowtitle()
 */
void ww_setwindowtitle(WINFO *wp)
{	int nr;
	char buf[512];

	if (!wp->edwin_handle)
		return;

	nr = wp->win_id;
	if (IsIconic(wp->edwin_handle)) {
		wsprintf(buf,"%d %s",nr,(LPSTR)string_getBaseFilename(ft_visiblename(wp->fp)));
	}
	else {
		wsprintf(buf,"#%d  %s",nr,(LPSTR)ft_visiblename(wp->fp));
	}
	SetWindowText(wp->edwin_handle,buf);
}

/*-----------------------------------------------------------
 * ww_setwindowflags()
 */
void ww_setwindowflags(WINFO *wp) {
	FTABLE* fp = wp->fp;
	DOCUMENT_DESCRIPTOR *linp = fp->documentDescriptor;

	if (linp->dispmode != wp->dispmode) {
		wp->dispmode = linp->dispmode;
		if (wp->dispmode & SHOWATTR) {
			mkattlist(fp->firstl);
		} 
	}

	memmove(&wp->fnt, &linp->fnt, sizeof wp->fnt);
	if (wp->fnt.fgcolor == wp->fnt.bgcolor) {
		wp->fnt.fgcolor = RGB(0,0,0);
		wp->fnt.bgcolor = RGB(255,255,255);
	}

	wp->fnt_handle = 0;
	if (wp->ww_handle) {
		sl_size(wp);
		font_selectStandardFont(wp->ww_handle, wp);
		render_sendRedrawToWindow(wp->ww_handle);
		wt_tcursor(wp,0);
		wt_tcursor(wp,1);
		caret_placeCursorForFile(fp,fp->ln,fp->caret.offset);
	}

	wp->workmode = linp->workmode;
	wp->tfillchar = linp->t1;
	wp->statusline = linp->statusline;
	wp->scrollflags = linp->scrollflags;
	wp->cursaftersearch = linp->cursaftersearch;
	wp->vscroll = linp->vscroll;
	wp->scroll_dy = linp->scroll_dy;
	SetScrollCheckBounds(wp);
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

	ww_setwindowflags(wp);
		 
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
		EdTRACE(Debug(DEBUG_ERR,"failed deleting window props"));
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
			Debug(DEBUG_ERR,"bad winlist");
	});
}

/*-----------------------------------------------------------
 * ww_nwi()
 */
int ww_nwi(void)
{
	return nwindows;
}

/*------------------------------------------------------------
 * ww_close()
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
	WINFO *				wp = (WINFO*)GetWindowLongPtr(hwnd, GWL_VIEWPTR);

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
		MakeSubWis(hwnd, wp, &xyWork, &xyRuler, &xyLineInfo);
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
		return mac_onIconAction(hwnd, wParam, lParam);

	case WM_MDIACTIVATE:
		/* EdSwitchContext(hwnd,LOWORD(lParam),CTX_EDIT); */
		if (wParam == 0)
			break;
	case WM_SETFOCUS:
		ll_moveElementToFront((LINKED_LIST**)&_winlist,wp);
		if (!IsIconic(hwnd))
			SetFocus(wp->ww_handle);
		break;
#if 0
		/* SETFOCUS must(!) be handled by MDIProc!! */
		if (message == WM_SETFOCUS)
			break;

		return 0;
# endif

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

		MakeSubWis(hwnd, wp,&xyWork,&xyRuler, &xyLineWindowSize);
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
				render_sendRedrawToWindow(wp->ru_handle);
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
		if (!ww_nwi()) {
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
 * SetWiSize()
 */
static int SetWiSize(WINFO *wp, int w, int h) {
	wp->workarea.g_x = wp->workarea.g_y = 0;
	wp->workarea.g_w = w;
	wp->workarea.g_h = h;

	SetScrollCheckBounds(wp);

	EdTRACE(Debug(DEBUG_TRACE,"SetWiSize to (%ld,%ld,%ld,%ld)",
		   wp->minln,wp->mincol,wp->maxln,wp->maxcol));
	return 1;
}

/*----------------------------------------------------------
 * do_mouse()
 */
int do_mouse(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int 		ncl = 1;
	int			ret;
	int			but;
	int			shift;
	int			x;
	int			y;
	FTABLE *	fp;

	if ((fp = (FTABLE *) GetWindowLongPtr(hwnd,0)) != 0) {
		x = LOWORD(lParam), y = HIWORD(lParam);
	} else {
		return 0;
	}

	switch(message) {
		case WM_LBUTTONDBLCLK:
			ncl = 2;
		case WM_LBUTTONDOWN: 
			but = 1; break;
		case WM_RBUTTONDBLCLK:
			ncl = 2;
		case WM_RBUTTONDOWN: 
			but = 2; break;
		case WM_MBUTTONDBLCLK:
			ncl = 2;
		case WM_MBUTTONDOWN: 
			but = 3; break;
		case WM_MOUSEMOVE:
			if ((wParam & (MK_LBUTTON|MK_RBUTTON|MK_MBUTTON)) == 0)
				return 0;
			else
				but = (wParam & MK_RBUTTON) ? 2 : 1;
			ncl = 0;
			break;
		default:
			return 0;
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
	ret = mouse_onMouseClicked(fp, x, y, but, ncl, shift);
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

/*------------------------------------------------------------
 * WorkAreaWndProc()
 */
static WINFUNC WorkAreaWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{    WINFO *	wp;

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

	case WM_LBUTTONDOWN:
		if (GetFocus() != hwnd) {
			SetFocus(hwnd);
		}
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		do_mouse(hwnd,message,wParam,lParam);
		return 0;

	case WM_HSCROLL:
	case WM_VSCROLL:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
			if (do_slide(wp,message,wParam,lParam) == 0) {
				break;
			}
		} else {
			EdTRACE(Debug(DEBUG_TRACE,"WM_SLIDE in WorkWndProc without file"));
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
			if (!SetWiSize(wp, LOWORD(lParam), HIWORD(lParam))) {
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
			op_updateall();
			FTABLE* fp = wp->fp;
			xref_selectFileFormat(fp->documentDescriptor->modename);
			mac_switchtodefaulttables();
	    }
	    else {
			EdTRACE(Debug(DEBUG_TRACE,
				"WM_SETFOCUS in WorkWndProc without file"));
	    }
	    return 0;

	case WM_KILLFOCUS:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
			if (wp)
				wt_tcursor(wp,0);
		} else {
			EdTRACE(Debug(DEBUG_TRACE,"WM_KILLFOCUS in WorkWndProc without file"));
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

	memset(&ps, 0, sizeof ps);
	hdc = BeginPaint(wp->ru_handle, &ps);
	GetClientRect(wp->ru_handle, &rect);
	nMiddle = rect.bottom / 2 - 3;
	width = wp->cwidth;
	rmargin = RightMargin(fp);

	HPEN markerPen = CreatePen(PS_SOLID, 3, RGB(80, 80, 80));
	xPos = wp->lineNumbers_handle ? lineNumberWindowWidth : 0;
	int xMin = ps.rcPaint.left - 20;
	int xMax = ps.rcPaint.right + 20;
	for (col = wp->mincol; col <= wp->maxcol; col++, xPos += width) {
		if (xPos < xMin || xPos > xMax) {
			continue;
		}
		MoveTo(hdc, xPos, nMiddle - 3);
		if (col == pDescriptor->lmargin || col == rmargin) {
			SelectObject(hdc, markerPen);
		}
		else {
			SelectObject(hdc, GetStockObject(BLACK_PEN));
		}
		LineTo(hdc, xPos, nMiddle);
		if (TABTHERE(pDescriptor,col)) {
			POINT polyPoints[3];
			polyPoints[0].x = xPos;
			polyPoints[0].y = rect.bottom - 5;
			polyPoints[1].x = xPos + 3;
			polyPoints[1].y = nMiddle + 2;
			polyPoints[2].x = xPos - 2;
			polyPoints[2].y = nMiddle + 2;
			SelectObject(hdc, GetStockObject(WHITE_BRUSH));
			SelectObject(hdc, GetStockObject(NULL_PEN));
			Polygon(hdc, polyPoints, 3);
		}
	}
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

		case WM_PAINT:
			if ((wp = (WINFO *)GetWindowLongPtr(hwnd,0)) != 0) {
		   		draw_ruler(wp);
			} else {
				EdTRACE(Debug(DEBUG_TRACE,"WM_PAINT in RulerWndProc without file"));
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

	hdc = BeginPaint(wp->lineNumbers_handle, &ps);
	GetClientRect(wp->lineNumbers_handle, &rect);

	saveFont = font_selectDefaultEditorFont(wp, hdc,NULL);
	SetTextColor(hdc, RGB(140,140,140));
	/* SetBkColor(hdc,wp->fnt_bgcolor); */
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

	case WM_PAINT:
		if ((wp = (WINFO*)GetWindowLongPtr(hwnd, 0)) != 0) {
			draw_lineNumbers(wp);
		}
		else {
			EdTRACE(Debug(DEBUG_TRACE, "WM_PAINT in RulerWndProc without file"));
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
 */
int ww_register(void)
{
	if (!EdMkWinClass(szWorkAreaClass, WorkAreaWndProc,
		(LPSTR)IDC_IBEAM, GetStockObject(WHITE_BRUSH), 0,
		sizeof(void*)) ||
		!EdMkWinClass(szEditClass, EditWndProc,
			(LPSTR)IDC_ARROW, NULL, "Edit",
			GWL_VIEWPTR + sizeof(void*)) ||
		!EdMkWinClass(szRulerClass, RulerWndProc,
			(LPSTR)IDC_CROSS, GetStockObject(LTGRAY_BRUSH), NULL,
			sizeof(void*)) ||
		!EdMkWinClass(szLineNumbersClass, LineNumberWndProc,
			(LPSTR)IDC_ARROW, CreateSolidBrush(RGB(248,248,248)), NULL,
			sizeof(void*))
#if 0
		||
		!EdMkWinClass(szStatusClass, StatusWndProc,
			(LPSTR)IDC_ARROW, NULL, 0,
			2 * sizeof(LONG))
#endif
		) {
		return 0;
	}
	return 1;
}

