/*
 * ALIGN.C
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: align text fragments to screen columns
 *
 * 										created      : 28.09.90
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <string.h>
#include "lineoperations.h"
#include "regexp.h"
#include "edierror.h"
#include "pksedit.h"

extern int 	chkblk(FTABLE *fp);
extern int	_cphys2scr(FTABLE *fp, char *lbuf,int lnoffs);

extern char 	_expbuf[];

/*---------------------------------*/
/* AlignText()					*/
/*---------------------------------*/
EXPORT int AlignText(char *finds, int scope, char filler, int flags)
{	int  col,firstcol,aligncol = -1,
		i,firsti,besti,bestcol,nchars,ret;
	FTABLE *fp;
	LINE *lp;
	MARK *mps,*mpe;
	char **loc;

	if (!exprinit(finds)) 
		return 0;

	fp = _currfile;
	if (select_range(scope,fp,&mps,&mpe) == RNG_INVALID)
		return 0;
	if (filler == 0)
		filler = ' ';

	abrt_start(IDS_ABRTALIGN);

	if (flags & (AL_CPOS|AL_FIX)) {
		firstcol = _cphys2scr(fp,fp->currl->lbuf,fp->lnoffset);
	}
	else
		firstcol = 0;

	loc = (flags & AL_END) ? &__loc2 : &__loc1;

	if (flags & AL_FIX)
		aligncol = firstcol;
	else
		/* looking for the right most position */
		for (aligncol = 0, lp = mps->lm; lp != 0; lp = lp->next) {
			i = cscr2phys(lp,firstcol);
			if (step(&lp->lbuf[i],_expbuf, &lp->lbuf[lp->len])) {
				col = _cphys2scr(fp,lp->lbuf,(int)(*loc - lp->lbuf));
				if (col > aligncol)
					aligncol = col;
			}
			if (lp == mpe->lm || ed_abort(1))
				break;
		}

	ret = 1;
	for (lp = mps->lm; lp != 0; lp = lp->next) {
		besti  = -1;
		firsti = cscr2phys(lp,firstcol);
		if (flags & AL_FIX)
			i = 0;		
		else
			i = firsti;
		while (step(&lp->lbuf[i],_expbuf,&lp->lbuf[lp->len])) {
			i = *loc - lp->lbuf;
			if (flags & AL_FIX) {
				if (i > firsti)
					break;
			}
			besti = i;
			if (!(flags & AL_FIX))
				break;
			i++;
		}
		if (besti >= 0) {
			bestcol = _cphys2scr(fp,lp->lbuf,besti);
			nchars  = aligncol - bestcol;
			if ((lp = ln_modify(fp,lp,besti,besti+nchars)) == 0L) {
				ret = 0;
				break;
			}
			blfill(&lp->lbuf[besti],nchars,filler);
		}
		if (lp == mpe->lm || ed_abort(1))
			break;
	}
	abrt_close(0);
	RedrawTotalWindow(fp);
	if ((ret & aligncol) >= 0) {
		curpos(fp->ln,(long )aligncol);
	}
	return ret;
}

