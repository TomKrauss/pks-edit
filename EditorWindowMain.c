/*
 * EditorWindowMain.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: editor windows - painting etc...
 *
 * 										created      :
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include <windows.h>
#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"
#include "winfo.h"
#include "winterf.h"
#include "pksedit.h"
#include "context.h"
#include "iccall.h"
#include "stringutil.h"

#define	WT_WORKWIN		0
#define	WT_RULERWIN		1
#define	GWL_FTABLE		GWL_ICCLASSVALUES+sizeof(void*)

#define	PROF_OFFSET		1

typedef struct xywh {
	int x,y,w,h;
} XYWH;

static WINFO *_winlist;

extern int do_icon(HWND icHwnd, WPARAM wParam,  LPARAM dropped);
extern long sl_thumb2deltapos(WINFO *wp, int horizontal, WORD thumb);
extern char *ft_visiblename(FTABLE *fp);
extern int  do_linbutton(FTABLE *fp, int x, int y, int msg, int shift);
extern int  do_mbutton(FTABLE *fp, int x,int y,int b, int nclicks,int shift);
extern void *icEditIconClass;
extern BOOL ic_isicon(HWND hwnd);
extern void st_redraw(BOOL bErase);
extern void tagselect(char *tags);
extern void mac_switchtodefaulttables(void);
extern void SetMenuFor(char *pszContext);
#if defined(DEBUG_MEMORY_USAGE)
extern long dumpallocated(void);
#endif

static WINDOWPLACEMENT	_winstates[6];

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

/*-----------------------------------------------------------
 * ww_register()
 */
int ww_register(void)
{
	if (!EdMkWinClass(szWorkAreaClass,WorkAreaWndProc,
		      (LPSTR)IDC_IBEAM,GetStockObject(WHITE_BRUSH),0,
		      sizeof(void*)) ||
		!EdMkWinClass(szEditClass,EditWndProc,
		      (LPSTR)IDC_ARROW,NULL,"Edit",
		      GWL_FTABLE+sizeof(void*)) ||
		!EdMkWinClass(szRulerClass,RulerWndProc,
			 (LPSTR)IDC_CROSS,GetStockObject(WHITE_BRUSH),0,
		      sizeof(void*))
#if 0
		       ||
	     !EdMkWinClass(szStatusClass,StatusWndProc,
		      (LPSTR)IDC_ARROW,NULL,0,
		      2 * sizeof(LONG))
#endif
			) {
		return 0;
	}
	return 1;
}
 
/*------------------------------------------------------------
 * MakeOrKillEditChild()
 */
static int MakeOrKillEditChild(
	HWND  hwnd, 
	DWORD winFlags,
	int   living,
	HWND  *hwndChild,
	char  *szClass, 
	XYWH  *XYPxy,
	FTABLE *fp
)
{
	if (living == 0) {
		if (*hwndChild) {
			DestroyWindow(*hwndChild);
			*hwndChild = 0;
		}
		return 0;
	}

	if (*hwndChild)
		return 1;

	*hwndChild = CreateWindow( szClass, NULL,
		winFlags, XYPxy->x, XYPxy->y, XYPxy->w, XYPxy->h, 
		hwnd, NULL, hInst, (LPVOID)fp);
	return *hwndChild != 0;
}

/*-----------------------------------------------------------
 * MakeSubWis()
 */
static int MakeSubWis(HWND hwnd, FTABLE *fp, XYWH *pWork, XYWH *pRuler)
{
	RECT    	rect;
	int		living;
	int		w;
	int		h;
	int		rh;
	WINFO *	wp;

	if ((wp = WIPOI(fp)) == 0)
		return 0;

	GetClientRect(hwnd,&rect);
	w = rect.right-rect.left;
	h = rect.bottom-rect.top;

	rh = 20;
	pRuler->x = 0;
	pRuler->w = w;
	pRuler->y = 0;
	pRuler->h = rh;

	living = (h > rh && (wp->dispmode & SHOWRULER));
	if (!MakeOrKillEditChild(hwnd,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		living,&wp->ru_handle,szRulerClass,pRuler,fp)) {
		pRuler->h = 0;
	}

	pWork->x = 0;
	pWork->w = w;
	pWork->y = pRuler->h;
	pWork->h = h - pRuler->h;
	return MakeOrKillEditChild(hwnd,
		WS_HSCROLL|WS_VSCROLL|WS_BORDER|WS_CHILD|WS_VISIBLE/*|WS_CLIPSIBLINGS*/,
		1,&wp->ww_handle,szWorkAreaClass,pWork,fp);
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
FTABLE *ww_stackwi(int num)
{	WINFO *wp;

	for (wp = _winlist; num > 0 && wp && wp->next; num--, wp = wp->next) 
		;
	if (num || !wp)
		return 0;
	return FTPOI(wp);
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
	ShowMessage(IDS_MSGWIREGDEF,num);
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
		if (!ic_isicon(hwnd)) {
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
		ic_position(hwnd, (nIndex % nXMax) * nIconWidth + nIconWidth / 2, 
				(nIndex / nXMax) * nIconHeight + nIconHeight / 2);
	}
	free(pszSlots);
	return 1;
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
				ed_error(IDS_MSGUNDEFWINREG);
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
		SendRedraw(wp->ww_handle);
		if (update) {
			UpdateWindow(wp->ww_handle);
		}
	}
}

/*-----------------------------------------------------------
 * ww_setwindowtitle()
 */
void ww_setwindowtitle(FTABLE *fp, WINFO *wp)
{	int nr;
	char buf[512];

	if (!wp->edwin_handle)
		return;

	nr = wp->win_id;
	if (IsIconic(wp->edwin_handle)) {
		wsprintf(buf,"%d %s",nr,(LPSTR)basename(ft_visiblename(fp)));
	}
	else {
		wsprintf(buf,"#%d  %s",nr,(LPSTR)ft_visiblename(fp));
	}
	SetWindowText(wp->edwin_handle,buf);
}

/*-----------------------------------------------------------
 * ww_setwindowflags()
 */
void ww_setwindowflags(FTABLE *fp, WINFO *wp)
{	LINEAL *linp = fp->lin;

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
		EdSelectStdFont(wp->ww_handle, wp);
		SendRedraw(wp->ww_handle);
		wt_tcursor(wp,0);
		wt_tcursor(wp,1);
		_curpos(fp,fp->ln,(long)fp->lnoffset);
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

	if ((wp = ll_insert(&_winlist,sizeof *wp)) == 0) {
		return 0;
	}

	wp->markstyles[FS_XMARKED] = _fstyles[0];
	wp->markstyles[FS_BMARKED] = _fstyles[1];
	fp->wp = wp;
	wp->fp = fp;
	wp->edwin_handle = hwnd;

	ww_setwindowflags(fp,wp);

	wp->hscroll   = 4;
	wp->scroll_dx = 4;

	wp->win_id = ++nwindows;

	return wp;
}

/*-----------------------------------------------------------
 * ww_destroy()
 */
void ww_destroy(FTABLE *fp)
{	WINFO *wp = WIPOI(fp);
	int   nId;

	if (wp == 0)
		return;

	nId = wp->win_id;
	if (!ll_delete(&_winlist,wp)) {
		EdTRACE(Debug(DEBUG_ERR,"failed deleting window props"));
		;
	}
	WIPOI(fp) = 0;

	/* update window handles */
	for (wp = _winlist; wp; wp = wp->next) {
		if (wp->win_id > nId) {
			wp->win_id--;
			ww_setwindowtitle(FTPOI(wp),wp);
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
		SendMessage(hwndClient,WM_MDIDESTROY,(WPARAM)hwndEdit,(LPARAM)0L);
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
	FTABLE *			fp = (FTABLE*)GetWindowLongPtr(hwnd, GWL_FTABLE);
	WINDOWPLACEMENT 	ws;
	WINFO *				wp;

	if (message == WM_CREATE || fp != NULL)
   	switch(message) {
	case WM_CREATE:
		{
		LPCREATESTRUCT    	cp;
		LPMDICREATESTRUCT 	mp;
		XYWH 		   		xyWork,xyRuler;

		cp = (LPCREATESTRUCT)lParam;
		mp = (LPMDICREATESTRUCT)cp->lpCreateParams;
		if (fp == NULL) {
			fp = (FTABLE*) mp->lParam;
		}

		if ((wp = ww_new(fp,hwnd)) == 0) {
			DestroyWindow(hwnd);
			return 0;
		}
		ShowWindow(hwnd, SW_HIDE);
		MakeSubWis(hwnd,fp,&xyWork,&xyRuler);
		ww_setwindowtitle(fp,wp);
		SetWindowLongPtr(hwnd,GWL_ICPARAMS,fp->fname);
		SetWindowLongPtr(hwnd,GWL_ICCLASSVALUES,icEditIconClass);
		SetWindowLongPtr(hwnd,GWL_FTABLE,fp);
		return 0;
		}

	case WM_ICONCLASSVALUE:
		return GetWindowLongPtr(hwnd,GWL_ICCLASSVALUES);

	case WM_ICONDROP:
		ww_popup(hwnd);
		return do_icon(hwnd, wParam, lParam);

	case WM_MDIACTIVATE:
		/* EdSwitchContext(hwnd,LOWORD(lParam),CTX_EDIT); */
		if (wParam == 0)
			break;
	case WM_SETFOCUS:
		ll_top(&_winlist,WIPOI(fp));
		if (!IsIconic(hwnd))
			SetFocus(WIPOI(fp)->ww_handle);
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
		wp = WIPOI(fp);
		if (wParam == SIZEZOOMHIDE || wParam == SIZEZOOMSHOW)
			break;
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
		ww_setwindowtitle(fp,wp);
		if (wParam == SIZEICONIC) {
			break;
		}

	case WM_EDWINREORG:
		{
		XYWH  xyWork,xyRuler;

		MakeSubWis(hwnd,fp,&xyWork,&xyRuler);
		wp = WIPOI(fp);
		if (wp->ww_handle) {
			MoveWindow(wp->ww_handle,xyWork.x,xyWork.y,
					xyWork.w,xyWork.h,1);
		}
		if (wp->ru_handle) {
			MoveWindow(wp->ru_handle,xyRuler.x,xyRuler.y,
					xyRuler.w,xyRuler.h,1);
		}
		if (message == WM_EDWINREORG)
			return 1;
		break;
		}
	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if (IsZoomed(hwnd))
			SendMessage(hwndClient,WM_MDIRESTORE, (WPARAM)hwnd, (LPARAM)0);
		if (!ft_wantclose(fp))
			return 0;
 		break;

	case WM_DESTROY:
		ww_destroy(fp);
		/* if last window for this buffer ... */
		ft_destroy(fp);
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
static int SetWiSize(FTABLE *fp, int w, int h)
{	WINFO *wp = fp->wp;

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
	ret = do_mbutton(fp, x, y, but, ncl, shift);
	return ret;
}

/*------------------------------------------------------------
 * do_slide()
 */
int do_slide(WINFO *wp, UINT message, WPARAM wParam, LONG lParam)
{	
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
WINFUNC WorkAreaWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{    FTABLE *	fp;

	switch(message) {
	case WM_CREATE:
		{
		WINFO *wp;

		fp = (FTABLE *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		wp = WIPOI(fp);
		SetWindowLongPtr(hwnd, 0, fp);
		EdSelectStdFont(hwnd, wp);
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
		if ((fp = (FTABLE *) GetWindowLongPtr(hwnd,0)) != 0) {
			if (do_slide(fp->wp,message,wParam,lParam) == 0) {
				break;
			}
		} else {
			EdTRACE(Debug(DEBUG_TRACE,"WM_SLIDE in WorkWndProc without file"));
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_PAINT:
		if ((fp = (FTABLE *) GetWindowLongPtr(hwnd,0)) != 0) {
		   RedrawWmPaint(WIPOI(fp));
		}
		return 0;

	case WM_SIZE:
		if ((fp = (FTABLE *) GetWindowLongPtr(hwnd,0)) != 0) {
#if 0
			GetWindowRect(hwnd, &rect);
			if ((rect.left & 0xF) || ((rect.right - rect.left) & 0xF) != 0xF) {
				HWND hwndDaddy;
				hwndDaddy = GetParent(hwnd);
				GetWindowRect(hwndDaddy, &rectDaddy);
				ScreenToClient(GetParent(hwndDaddy), (PPOINT)&rectDaddy.left);
				ScreenToClient(GetParent(hwndDaddy), (PPOINT)&rectDaddy.right);
				MoveWindow(hwndDaddy, 
					rectDaddy.left - (rect.left & 0xF),
					rectDaddy.top, 
					rectDaddy.right - rectDaddy.left - 
						((rect.right - rect.left) & 0xF) + 0xF, 
					rectDaddy.bottom - rectDaddy.top, TRUE);
				return 0;
			}
# endif
			if (!SetWiSize(fp, LOWORD(lParam), HIWORD(lParam))) {
				return 0;
			}
			WIPOI(fp)->ww_handle = hwnd;
			sl_size(WIPOI(fp));
	    }
	    break;

	case WM_SETFOCUS:
	    if ((fp = (FTABLE *) GetWindowLongPtr(hwnd,0)) != 0) {
			wt_tcursor(fp->wp,1);
			ft_select(fp);
			op_updateall();
			tagselect(fp->lin->modename);
			mac_switchtodefaulttables();
	    }
	    else {
			EdTRACE(Debug(DEBUG_TRACE,
				"WM_SETFOCUS in WorkWndProc without file"));
	    }
	    return 0;

	case WM_KILLFOCUS:
		if ((fp = (FTABLE *) GetWindowLongPtr(hwnd,0)) != 0) {
			if (fp->wp)
				wt_tcursor(fp->wp,0);
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
 * draw_lin()
 */
static void draw_lin(FTABLE *fp)
{
	int 			col;
	int			xPos;
	int			rmargin;
	int			width;
	int			nMiddle;
	RECT			rect;
	WINFO *		wp = WIPOI(fp);
	LINEAL *		lin = fp->lin;
	HDC 			hdc;
	PAINTSTRUCT 	ps;
	POINT		polyPoints[4];

	hdc = BeginPaint(wp->ru_handle, &ps);

	GetClientRect(wp->ru_handle, &rect);
	nMiddle = rect.bottom / 2 - 3;
	width = wp->cwidth;
	rmargin = RightMargin(fp);
	SetPolyFillMode(hdc, WINDING);
	SelectObject(hdc, GetStockObject(BLACK_BRUSH));

	SelectObject(hdc, GetStockObject(WHITE_PEN));
	MoveTo(hdc, 3, nMiddle+1);
	LineTo(hdc, rect.right - 3, nMiddle+1);

	SelectObject(hdc, GetStockObject(BLACK_PEN));
	MoveTo(hdc, 3, nMiddle);
	LineTo(hdc, rect.right - 3, nMiddle);

	for (xPos = 0, col = wp->mincol; col <= wp->maxcol; col++, xPos += width) {
		SelectObject(hdc, GetStockObject(WHITE_PEN));
		MoveTo(hdc, xPos+1, nMiddle - 3);
		LineTo(hdc, xPos+1, nMiddle - 1);
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		MoveTo(hdc, xPos, nMiddle - 3);
		LineTo(hdc, xPos, nMiddle);
		if (col == lin->lmargin || col == rmargin) {
			polyPoints[0].x = xPos-3;
			polyPoints[0].y = 3;
			polyPoints[1].x = xPos;
			polyPoints[1].y = 8;
			polyPoints[2].x = xPos+3;
			polyPoints[2].y = 3;
			Polygon(hdc, polyPoints, 3);
		}
		if (TABTHERE(lin,col)) {
			polyPoints[0].x = xPos;
			polyPoints[0].y = rect.bottom - 5;
			polyPoints[1].x = xPos + 3;
			polyPoints[1].y = nMiddle + 3;
			polyPoints[2].x = xPos - 3;
			polyPoints[2].y = nMiddle + 3;
			Polygon(hdc, polyPoints, 3);
		}
	}

	EndPaint(wp->ru_handle,&ps);
}

/*--------------------------------------------------------------------------
 * shadow_area()
 */
static void shadow_area(HDC hdc, HWND hwnd)
{
	HPEN				hPen;
	RECT		  		rect;
	int				top;
	int				bottom;
	int				left;
	int				right;
	DWORD			dwBottomShadow;

	GetClientRect(hwnd,&rect);
	SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	Rectangle(hdc, 0, 0, rect.right, rect.bottom);

	SelectObject(hdc, GetStockObject(WHITE_PEN));
	top = 1;
	bottom = rect.bottom - 1;
	left = 1;
	right = rect.right-1;
	MoveTo(hdc, right, top);
	LineTo(hdc, left, top);
	LineTo(hdc, left, bottom);
	
	right -= 2;
	MoveTo(hdc, right, top + 2);
	LineTo(hdc, right, bottom - 2);
	LineTo(hdc, left + 1, bottom - 2);

	top += 2;
	bottom -= 2;
	dwBottomShadow = GetSysColor(COLOR_BTNSHADOW);
	hPen = SelectObject(hdc, CreatePen(PS_SOLID, 1, dwBottomShadow));

	left = 3;
	right = rect.right-3;
	MoveTo(hdc, right, top);
	LineTo(hdc, left, top);
	LineTo(hdc, left, bottom);

	DeleteObject(SelectObject(hdc, hPen));
}

/*------------------------------------------------------------
 * RulersWndProc()
 */
WINFUNC RulerWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
)
{	FTABLE *fp;

	switch(message) {
		case WM_ERASEBKGND:
			shadow_area((HDC)wParam,hwnd);
			return 0;

		case WM_PAINT:	
			if ((fp = (FTABLE *)GetWindowLongPtr(hwnd,0)) != 0) {
		   		draw_lin(fp);
			} else {
				EdTRACE(Debug(DEBUG_TRACE,"WM_PAINT in RulerWndProc without file"));
			}
			break;

		case WM_CREATE:
			fp = (FTABLE *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtr(hwnd,0,fp);
			return 0;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			if ((fp = (FTABLE *) GetWindowLongPtr(hwnd,0)) != 0) {
				do_linbutton(fp, (int)LOWORD(lParam), (int)HIWORD(lParam), 
					(int)message, (int)wParam);
			}
			return 0;

		case WM_DESTROY:
			return 0;
	}
	return (DefWindowProc(hwnd, message, wParam, lParam));
}

