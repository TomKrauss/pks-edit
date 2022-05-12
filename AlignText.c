/*
 * align_text.C
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: align text fragments to screen columns
 *
 * 										created: 28.09.90
 * 										last modified: 20.6.2021
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <string.h>
#include "caretmovement.h"
#include "winfo.h"
#include "regexp.h"
#include "publicapi.h"
#include "findandreplace.h"
#include "edierror.h"
#include "pksedit.h"

/*---------------------------------*/
/* align_text()					*/
/*---------------------------------*/
EXPORT int align_text(char *pszSearch, int scope, char filler, ALIGN_FLAGS flags)
{	int  col,firstcol,aligncol = -1,
		firsti,besti,bestcol,nchars,ret;
	long i;
	FTABLE *fp;
	LINE *lp;
	MARK *mps,*mpe;
	char **loc;
	RE_MATCH match;
	RE_PATTERN* pattern;
	WINFO* wp;

	if (!(pattern = regex_compileWithDefault(pszSearch))) {
		return 0;
	}

	wp = ww_getCurrentEditorWindow();
	fp = wp->fp;
	if (find_setTextSelection(wp, scope,&mps,&mpe) == RNG_INVALID)
		return 0;
	if (filler == 0)
		filler = ' ';
	progress_startMonitor(IDS_ABRTALIGN, 1000);

	if (flags & (AL_CPOS|AL_FIX)) {
		firstcol = caret_lineOffset2screen(wp,&wp->caret);
	}
	else
		firstcol = 0;

	if (flags & AL_FIX)
		aligncol = firstcol;
	else
		/* looking for the right most position */
		for (aligncol = 0, lp = mps->m_linePointer; lp != 0; lp = lp->next) {
			i = caret_screen2lineOffset(wp, &(CARET){ .linePointer = lp, .offset = firstcol });
			if (regex_match(pattern, &lp->lbuf[i],&lp->lbuf[lp->len], &match)) {
				loc = (flags & AL_END) ? &match.loc1 : &match.loc2;
				col = caret_lineOffset2screen(wp, &(CARET) { .linePointer = lp, .offset = (int)(*loc - lp->lbuf)});
				if (col > aligncol)
					aligncol = col;
			}
			if (lp == mpe->m_linePointer || progress_cancelMonitor(1))
				break;
		}

	ret = 1;
	for (lp = mps->m_linePointer; lp != 0; lp = lp->next) {
		besti  = -1;
		firsti = caret_screen2lineOffset(wp, &(CARET){.linePointer = lp, .offset = firstcol });
		if (flags & AL_FIX)
			i = 0;		
		else
			i = firsti;
		while (regex_match(pattern, &lp->lbuf[i],&lp->lbuf[lp->len], &match)) {
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
			bestcol = caret_lineOffset2screen(wp, &(CARET) {.linePointer = lp, .offset = besti});
			nchars  = aligncol - bestcol;
			if ((lp = ln_modify(fp,lp,besti,besti+nchars)) == 0L) {
				ret = 0;
				break;
			}
			memset(&lp->lbuf[besti],filler,nchars);
		}
		if (lp == mpe->m_linePointer || progress_cancelMonitor(1))
			break;
	}
	progress_closeMonitor(0);
	render_repaintAllForFile(fp);
	if ((ret & aligncol) >= 0) {
		caret_placeCursorInCurrentFile(wp, wp->caret.ln,(long )aligncol);
	}
	return ret;
}

