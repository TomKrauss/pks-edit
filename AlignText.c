/*
 * ALIGN.C
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: align text fragments to screen columns
 *
 * 										created: 28.09.90
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include <string.h>
#include "caretmovement.h"
#include "winfo.h"
#include "regexp.h"
#include "edierror.h"
#include "pksedit.h"

/*---------------------------------*/
/* AlignText()					*/
/*---------------------------------*/
EXPORT int AlignText(char *finds, int scope, char filler, int flags)
{	int  col,firstcol,aligncol = -1,
		firsti,besti,bestcol,nchars,ret;
	long i;
	FTABLE *fp;
	LINE *lp;
	MARK *mps,*mpe;
	char **loc;
	RE_MATCH match;
	RE_PATTERN* pattern;

	if (!(pattern = regex_compileWithDefault(finds))) {
		return 0;
	}

	fp = ft_getCurrentDocument();
	if (find_setTextSelection(scope,fp,&mps,&mpe) == RNG_INVALID)
		return 0;
	if (filler == 0)
		filler = ' ';

	progress_startMonitor(IDS_ABRTALIGN);

	if (flags & (AL_CPOS|AL_FIX)) {
		firstcol = caret_lineOffset2screen(fp,&fp->caret);
	}
	else
		firstcol = 0;

	if (flags & AL_FIX)
		aligncol = firstcol;
	else
		/* looking for the right most position */
		for (aligncol = 0, lp = mps->lm; lp != 0; lp = lp->next) {
			i = caret_screen2lineOffset(fp, &(CARET){ lp, firstcol });
			if (step(pattern, &lp->lbuf[i],&lp->lbuf[lp->len], &match)) {
				loc = (flags & AL_END) ? &match.loc1 : &match.loc2;
				col = caret_lineOffset2screen(fp, &(CARET) { lp, (int)(*loc - lp->lbuf)});
				if (col > aligncol)
					aligncol = col;
			}
			if (lp == mpe->lm || progress_cancelMonitor(1))
				break;
		}

	ret = 1;
	for (lp = mps->lm; lp != 0; lp = lp->next) {
		besti  = -1;
		firsti = caret_screen2lineOffset(fp, &(CARET){ lp, firstcol });
		if (flags & AL_FIX)
			i = 0;		
		else
			i = firsti;
		while (step(pattern, &lp->lbuf[i],&lp->lbuf[lp->len], &match)) {
			loc = (flags & AL_END) ? &match.loc1 : &match.loc2;
			i = (long)(*loc - lp->lbuf);
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
			bestcol = caret_lineOffset2screen(fp, &(CARET) { lp, besti});
			nchars  = aligncol - bestcol;
			if ((lp = ln_modify(fp,lp,besti,besti+nchars)) == 0L) {
				ret = 0;
				break;
			}
			memset(&lp->lbuf[besti],filler,nchars);
		}
		if (lp == mpe->lm || progress_cancelMonitor(1))
			break;
	}
	progress_closeMonitor(0);
	render_repaintAllForFile(fp);
	if ((ret & aligncol) >= 0) {
		caret_placeCursorInCurrentFile(fp->ln,(long )aligncol);
	}
	return ret;
}

