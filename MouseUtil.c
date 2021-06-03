/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * MouseUtil.c
 *
 * some mouse functions
 *
 * 						created: 07.06.91 
 *						Author : TOM
 */

#include <windows.h>
#include <windowsx.h>
#include "trace.h"
#include "caretmovement.h"
#include "textblocks.h"
#include "edierror.h"
#include "errordialogs.h"
#include "winfo.h"

#include "winterf.h"
#include "edfuncs.h"
#include "pksedit.h"
#include "functab.h"
#include "iccall.h" 
#include "resource.h"

static HCURSOR   hHourGlass;		// Hour glass cursor
static HCURSOR   hSaveCurs;

extern void 	caret_calculateOffsetFromScreen(WINFO *wp,int x, int y, long *ln,long *col);
extern long 	ln_indexOf(FTABLE *fp,LINE *lp);
extern void 	st_seterrmsg(char *msg);
extern int 		MousePosition(FTABLE *fp, long bAsk);
extern HWND 	ic_findChildFromPoint(HWND hwnd, POINT *point);

extern MOUSEBIND	_mousetab[MAXMAPMOUSE];

static RSCTABLE __m = {
	0,	"default",	sizeof _mousetab, 	_mousetab,
	(unsigned char *)_mousetab+sizeof(_mousetab)
};

RSCTABLE *_mousetables = &__m;

/*------------------------------------------------------------
 * MouseGetXYPos()
 */
EXPORT void MouseGetXYPos(HANDLE hwnd, int *x, int *y)
{	POINT p;

	if (!hwnd)
		return;
		
	GetCursorPos(&p);
	ScreenToClient(hwnd,&p);
	*x = p.x;
	*y = p.y;
}

/*------------------------------------------------------------
 * graf_mkstate()
 */
EXPORT void graf_mkstate(int *x, int *y, int *but, int *shift)
{ 	MSG msg;

	while(1) {
		PeekMessage(&msg, 0, 0, 0, PM_REMOVE|PM_NOYIELD);
		switch(msg.message) {
			case WM_TIMER:
				if (msg.wParam == TIM_FRAME)
					break;
				return;
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
}

/*------------------------------------------------------------
 * MouseBusy()
 */
EXPORT void MouseBusy(void)
{
	if (hHourGlass == NULL)
		hHourGlass = LoadCursor(NULL, IDC_WAIT);
	hSaveCurs = SetCursor(hHourGlass);
}

/*------------------------------------------------------------
 * MouseNotBusy()
 */
EXPORT void MouseNotBusy(void)
{
	if (hSaveCurs != 0)
		SetCursor(hSaveCurs);
}

/*------------------------------------------------------------
 * changemouseform()
 */
EXPORT void changemouseform(void)
{
	MouseNotBusy();
}

/*------------------------------------------------------------
 * EdBlockMouseMark()
 */
EXPORT int EdBlockMouseMark(int typ)
{	register	markforward;
	int			xx,yy,b,colflg,k,x,y;
	long		ln,col,ln1,col1,saveln = -1,savecol;
	WINFO	*	wp;
	FTABLE	*	fp;
	UINT_PTR	id;

	if ((fp =ft_CurrentDocument()) == 0)
		return 0;

	wp = WIPOI(fp);
	MouseGetXYPos(wp->ww_handle, &x, &y);
	xx = x;
	yy = y;

	ln1 = -1;
	if (typ != MARK_CONT) {
		EdBlockHide();
		markforward = -1;
	} else {
		if (fp->blstart) {
			ln1	= ln_indexOf(fp,fp->blstart->lm);
			col1 = fp->blstart->lc;
		}
		markforward = 1;
	}
#if 0
	if (typ == MARK_RUBBER) {
		unsetmouseevent();
		mf(POINT_HAND);
		graf_rubberbox(x,y,-x,-y,&xx,&yy);
		nx = xx+x+4; ny = yy+y/* - wp->cheight/2 */;
		if (ny > y) {
			colflg = (bl_defineColumnSelectionFromXY(x,y,nx,ny)) ? MARK_COLUMN : 0;
			if (caret_moveToXY(wp,x,y))   EdSyncSelectionWithCaret(colflg);
			if (caret_moveToXY(wp,nx,ny)) EdSyncSelectionWithCaret(colflg|MARK_END);
		}
	}
	else {
#endif
	SetCapture(wp->ww_handle);
	id = SetTimer(0,0,100,0);

	while(1) {
		graf_mkstate(&xx,&yy,&b,&k);
		if (!b) {
			break;
		}
		caret_calculateOffsetFromScreen(wp,xx,yy,&ln,&col);
		colflg = 0;
		if (markforward >= 0 && bl_defineColumnSelectionFromXY(x,y,xx,yy)) {
			long lnx = ln, colx = col;
			caret_updateLineColumn(fp,&lnx,&colx,1);
			colflg = MARK_COLUMN;
		} else {
			if (!caret_updateLineColumn(fp,&ln,&col,1))
				continue;
		}
		if (ln == saveln && col == savecol)
			continue;
		saveln = ln;
		savecol = col;
		if (ln != ln1 || col != col1) {
			if (markforward < 0) {
				wt_curpos(wp,ln,col);
				ln1 = ln;
				col1 = col;
				EdSyncSelectionWithCaret(colflg|MARKSTART);
				markforward = 1;
			} else {
				if ((ln > ln1 || (ln == ln1 && col > col1)) != markforward) {
					/* marking direction changed */
					caret_updateLineColumn(fp,&ln1,&col1,1);
					EdSyncSelectionWithCaret((markforward ? MARK_END : MARK_START)|colflg);
					markforward = !markforward;
					continue;
				} 
				wt_curpos(wp,ln,col);
				EdSyncSelectionWithCaret((markforward ? MARK_END : MARK_START)|colflg);
			}
			UpdateWindow(wp->ww_handle);
		}
	}

	if (id)
		KillTimer(0,id);
	ReleaseCapture();

	return 1;
}

/*------------------------------------------------------------
 * EdMouseMoveText()
 */
EXPORT int EdMouseMoveText(int move)
{
	HCURSOR 	hSave,hCursor;
	int	   	b,ret,dum,x,y;
	WINFO *	wp;
	FTABLE *	fp;

	ret = 0;
	if ((fp = ft_CurrentDocument()) == 0) {
		return 0;
	}
	wp = WIPOI(fp);

	if ((hCursor = LoadCursor(hInst, move ? "CursMove" : "CursCopy")) == 0) {
		hSave = 0;
	} else {
		hSave = SetCursor(hCursor);
	}

#if 0
	MouseGetXYPos(wp->ww_handle, &x, &y);
# endif

	SetCapture(wp->ww_handle);
	do {
		graf_mkstate(&x,&y,&b,&dum);
	} while(b);
	ReleaseCapture();

	POINT p;
	HWND  hwnd;

	GetCursorPos(&p);
	hwnd = ic_findChildFromPoint((HWND)0,&p);

	if (hwnd == wp->edwin_handle) {
		if (MousePosition(fp, 0L)) {
			if (move) 
				ret = do_func(FUNC_EdBlockMove,0L,0L,(void*)0,(void*)0,(void*)0); 
			else 
				ret = do_func(FUNC_EdBlockCopy,0L,0L,(void*)0,(void*)0,(void*)0);
		}
	} else if (hwnd) {
		ret = PostMessage(hwnd,WM_ICONDROP,ICACT_TEXTBLOCK,0L);
	}
	if (hSave)
		SetCursor(hSave);
	return ret;
}

/*--------------------------------------------------------------------------
 * mouseemptyslot()
 */
int mouseemptyslot(MOUSEBIND *mp)
{
	return (mp->button == 0 && mp->nclicks == 0) ? 1 : 0;
}

/*------------------------------------------------------------
 * mouseslot()
 */
static MOUSEBIND *mouseslot(int nButton, int nShift, int nClicks)
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
	return (char *)0;
}

/*--------------------------------------------------------------------------
 * mousebound()
 */
MOUSEBIND *mousebound(MOUSECODE mcode)
{
	return mouseslot(mcode.button, mcode.shift, mcode.nclicks);
}

/*-----------------------------------------------------------
 * mousegetbind()
 */
MOUSEBIND *mousegetbind(void)
{
 	MOUSEBIND 	*mp;

	if ((mp = mouseslot(0, 0, 0)) != 0) {
		return mp;
	}

	return rsc_tableresize(_mousetables, sizeof *mp, 
		_mousetab, (int (*)(void *))mouseemptyslot);
}

/*---------------------------------*/
/* mouse_unbind()				*/
/*---------------------------------*/
void mouse_unbind(MOUSEBIND *mp)
{
	if (mp == 0)
		return;

	mp->button = 0;
	mp->nclicks = 0;
	mp->shift = 0;
	mp->macref.typ = CMD_NONE;
}

/*--------------------------------------------------------------------------
 * mouse_overridetable()
 */
void mouse_overridetable(void)
{
	RSCTABLE *		rp;

	if ((rp = _mousetables) != 0 && rp->rt_data) {
		blfill(rp->rt_data, (int)((char *)rp->rt_end - (char *)rp->rt_data), 0);
	}
}

/*---------------------------------*/
/* mouse_delbindings()			*/
/* delete all mouse bindings to a 	*/
/* for inst. single macro,...		*/
/*---------------------------------*/
void mouse_delbindings(MACROREFTYPE typ, MACROREFIDX val)
{
	RSCTABLE *	rp;
	MOUSEBIND  *	mp;

	for (rp = _mousetables; rp; rp = rp->rt_next) {
		for (mp = (MOUSEBIND*)rp->rt_data; mp < (MOUSEBIND*)rp->rt_end; mp++) {
			if (typ == mp->macref.typ && val == mp->macref.index) {
				mouse_unbind(mp);
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
		ShowError(mp->msg, NULL);
	}
	if (mp->flags & MO_FINDCURS) {
		caret_calculateOffsetFromScreen(wp, x, y, &ln, &col);
		if (caret_updateLineColumn(FTPOI(wp),&ln,&col,1)) {
			wt_curpos(wp,ln,col);
		}
	}

	return mac_runcmd(&mp->macref);
}

/*----------------------------*/
/* do_mbutton()			*/
/*----------------------------*/
EXPORT int do_mbutton(FTABLE *fp, int x, int y, int b, int nclicks, int shift)
{
	MOUSEBIND *	mp;

	if (shift & 0x3) {
		shift |= 0x3;
	}

	if ((mp = mouseslot(b, shift, nclicks)) != 0) {
		stopcash();
		mfunct(WIPOI(fp), mp, x, y);
		return 1;
	}
	return 0;
}

/*----------------------------*/
/* do_linbutton()			*/
/*----------------------------*/
EXPORT int do_linbutton(WINFO *wp, int x, int y, int msg, int shift) {
	long 	ln;
	long		col;
	char		szBuf[100];
	FTABLE* fp = wp->fp;

	caret_calculateOffsetFromScreen(wp,x + wp->cwidth / 2,y,&ln,&col);
	wsprintf(szBuf, /*STR*/"SPALTE: %4ld", col+1);
	st_seterrmsg(szBuf);

	if (msg != WM_MOUSEMOVE) {
		if (msg == WM_RBUTTONDOWN) {
			fp->documentDescriptor->rmargin = col;
		} else {
			ToggleTabStop(fp->documentDescriptor, col);
		}
		SendRedraw(wp->ru_handle);
		EdRedrawWindow(wp);
	}

	return 1;
}

