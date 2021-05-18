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
 * (c) Pahlen & Krauﬂ
 */

#include "pksedit.h"
#include "caretmovement.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "lineoperations.h"

extern	int		_playing;

/*--------------------------------------------------------------------------
 * LOCALS
 */
#define	NFRAGS		64

typedef struct undo {
	int nfrags;
	struct upar {
		LINE		*lp,*lp2;
		int		op;
	} p[NFRAGS];
	struct undo *prev;
} UNDO;

typedef struct undoblock {
	struct undoblock *	next;
	UNDO	*			fifo;
	LINE	*			bls;
	LINE *			ble;			/* Start- und Endmarke */
	int				bcs,bce,c1,c2;
	long				ln,col;		/* akt. Cursorposition */
} UNDOBLOCK;

static	int		_undoenabled;

/*--------------------------------------------------------------------------
 * upfree()
 */
static void upfree(UNDO *up)
{
	struct upar *	p;
	UNDO *		prev;
	int    		i;

	while(up) {
		for (i = 0; i < up->nfrags; i++) {
			p = &up->p[i];
			if (p->op != O_INSERT && p->op <= O_LNORDER) {
				if (p->op == O_DELETE) {
					ln_destroy(p->lp);
				} else {
					if (p->lp) {
						_free(p->lp);
					}
				}
			}
			if (p->op == O_LNORDER)
				_free(p->lp2);
		}
		prev = up->prev;
		_free(up);
		up = prev;
	}
}

/*--------------------------------------------------------------------------
 * u_cash()
 */
EXPORT int u_cash(FTABLE *fp, LINE *lp,LINE *lpmark,int op)
{	register UNDO *up;
	register UNDOBLOCK *ubp;
	struct   upar *p;

	if (_undoenabled == 0) {
		if (op == O_MODIFY || op == O_DELETE)
			_free(lp);
		return 0;
	}

	ubp = UNDOPOI(fp);
	up = ubp->fifo;

	if (up == 0 || up->nfrags >= NFRAGS) {
		if ((up = _alloc(sizeof(UNDO))) == 0) {
			_undoenabled = 0;
			return 0;
		}
		blfill(up, sizeof(UNDO), 0);
		up->prev = ubp->fifo;
		ubp->fifo = up;
		up->nfrags = 0;
	}
	p = &up->p[up->nfrags++];
	p->lp     = lp;
	p->lp2    = lpmark;
	p->op     = op;
	return 1;
}

/*--------------------------------------------------------------------------
 * u_free()
 */
static void u_free(UNDOBLOCK *ubp)
{
	if (ubp == 0)
		return;
	upfree(ubp->fifo);
	ubp->fifo = 0;
	_undoenabled = (GetConfiguration()->options & UNDOENABLED) ? 1 : 0;
}

/*--------------------------------------------------------------------------
 * u_new()
 */
EXPORT int u_new(FTABLE *fp)
{    unsigned char *ubp;

	int tempSize = sizeof(UNDOBLOCK);
	if ((ubp = _alloc(tempSize)) == 0) {
		return 0;
	}
	blfill(ubp,tempSize, 0);
	UNDOPOI(fp) = (UNDOBLOCK*)ubp;
	return 1;
}

/*--------------------------------------------------------------------------
 * u_destroy()
 */
EXPORT void u_destroy(FTABLE *fp)
{	UNDOBLOCK *ubp = UNDOPOI(fp);

	if (ubp) {
		u_free(ubp);
		_free(ubp);
		UNDOPOI(fp) = 0;
	}
}

/*--------------------------------------------------------------------------
 * u_init()
 */
EXPORT void u_init(FTABLE *fp)
{	UNDOBLOCK *ubp;
	MARK 	*mp;
	LINE 	*lp1,*lptmp;

	if (_playing)
		return;

	ubp = UNDOPOI(fp);
	u_free(ubp);
	lptmp = fp->tln;
	if (lptmp != 0 && (lp1 = ln_cut(lptmp,lptmp->len,0,lptmp->len)) != 0L) {
		ln_replace(fp,lptmp,lp1);
		lp1->lflg = lptmp->lflg;
		_free(lptmp);
		fp->tln = 0;
	}
	ubp->ln  = fp->ln;
	ubp->col = fp->lnoffset;
	if ((mp = fp->blstart) != 0) {
		ubp->bls = mp->lm;
		ubp->bcs = mp->lc;
	} else ubp->bls = 0;
	if ((mp = fp->blend) != 0) {
		ubp->ble = mp->lm;
		ubp->bce = mp->lc;
	} else ubp->ble = 0;
	ubp->c1  = fp->blcol1;
	ubp->c2  = fp->blcol2;
}

/*--------------------------------------------------------------------------
 * un_do()
 */
EXPORT int un_do(FTABLE *fp)
{	register UNDO 		*up;
	register struct upar *p;
	register UNDOBLOCK  *ubp;
	register UNDO 		*upprevious;
	UNDOBLOCK			usave;
	register LINE 		*lp;
	long	    			ln,col;
	register int 		i;

	if (_playing ||
		(ubp = UNDOPOI(fp), (up = ubp->fifo) == 0)) {
		ed_error(IDS_MSGNOUNDO);
		return 0;
	}
	usave = *ubp;
	ubp->fifo = 0;
	u_init(fp);

	ln  = 0; col = 0;
	cposvalid(fp,&ln,&col,0);

	fp->flags |= F_CHANGEMARK;

	Pastehide(0);

	while(up != 0) {
		for (i = up->nfrags; i > 0; ) {
			i--;
			p = &up->p[i];
			switch(p->op) {
			  case O_HIDE:
			  	ln_unhide(fp, p->lp);
				break;
			  case O_UNHIDE:
				ln_hide(fp, p->lp, p->lp2);
				break;
			  case O_MODIFY:
				if ((lp = p->lp2) == 0)
					lp = fp->firstl;
				else lp = lp->next;
			     p->lp->lflg &= (LNINDIRECT|LNXMARKED|LNNOTERM|LNNOCR);
				ln_replace(fp,lp,p->lp);
				u_cash(fp,lp,p->lp->prev,O_MODIFY);
				break;
			  case O_DELETE:
			     p->lp->lflg &= (LNINDIRECT|LNXMARKED|LNNOTERM|LNNOCR);
				ln_insert(fp, p->lp2, p->lp);
				break;
			  case O_INSERT:
				if ((lp = p->lp2) != 0)
					ln_delete(fp,lp->prev);
				break;
			  case O_LNORDER:
				ln_order(fp,p->lp,p->lp2);
				_free(p->lp);
				_free(p->lp2);
				break;
			}
		}
		up = up->prev;
	}
	ubp = &usave;

	fp->blcol1 = ubp->c1;
	fp->blcol2 = ubp->c2;

	markblk(fp,ubp->bls,ubp->bcs,ubp->ble,ubp->bce);

	curpos(ubp->ln,ubp->col);
	RedrawTotalWindow(fp);

	up = usave.fifo;
	while (up != 0) {
		upprevious = up->prev;
		_free(up);
		up = upprevious;
	}
	return _undoenabled;
}

