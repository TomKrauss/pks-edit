/*
 * align_text.C
 *
 * Project: PKS Edit for Windows
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
#include "documentmodel.h"
#include "winfo.h"
#include "regexp.h"
#include "publicapi.h"
#include "findandreplace.h"
#include "edierror.h"
#include "pksedit.h"

/*---------------------------------*/
/* align_text()					*/
/*---------------------------------*/
EXPORT int align_text(char *pszSearch, RANGE_TYPE scope, char filler, ALIGN_FLAGS flags)
{	int  col,firstcol,aligncol = -1, screencol, nchars, ret;
	long i;
	FTABLE *fp;
	LINE *lp;
	MARK *mps,*mpe;
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
	BOOL bAlignEnd = flags & AL_END;
	if (flags & AL_TO_CURSOR) {
		aligncol = caret_lineOffset2screen(wp,&wp->caret);
	}
	else {
		firstcol = 0;
		/* looking for the matching position for alignment */
		for (aligncol = 0, lp = mps->m_linePointer; lp != 0; lp = lp->next) {
			if (regex_match(pattern, lp->lbuf, &lp->lbuf[lp->len], &match)) {
				char* pMatch = bAlignEnd ? match.loc2 : match.loc1;
				size_t  nOffset = pMatch - lp->lbuf;
				col = caret_lineOffset2screen(wp, &(CARET) {.linePointer = lp, .offset = (int)nOffset});
				if (col > aligncol) {
					aligncol = col;
					if (flags & AL_OFFSET_FIRST_MATCH) {
						break;
					}
				}
			}
			if (lp == mpe->m_linePointer || progress_cancelMonitor(1))
				break;
		}
	}

	ret = 1;
	for (lp = mps->m_linePointer; lp != 0; lp = lp->next) {
		if (regex_match(pattern, lp->lbuf, &lp->lbuf[lp->len], &match)) {
			size_t nOffset = (bAlignEnd ? match.loc2 : match.loc1) - lp->lbuf;
			screencol = caret_lineOffset2screen(wp, &(CARET) {.linePointer = lp, .offset = (int)nOffset});
			if (screencol != aligncol) {
				int nt = 0;
				lp = edit_expandTabsInLineWithSpaces(wp, lp, &nt);
				nchars = aligncol - screencol;
				size_t nMatchLen = match.loc2 - match.loc1;
				int nStart = bAlignEnd ? screencol - (int)nMatchLen : screencol;
				if (nchars > 0) { 
					if ((lp = ln_modify(fp, lp, nStart, nStart + nchars)) == 0L) {
						ret = 0;
						break;
					}
					memset(&lp->lbuf[nStart], filler, nchars);
				} else if (nchars < 0) {
					i = nStart;
					int nTarget = nStart + nchars;
					while (i > nTarget) {
						if (lp->lbuf[i-1] != filler) {
							break;
						}
						i--;
					}
					if ((lp = ln_modify(fp, lp, nStart, i)) == 0L) {
						ret = 0;
						break;
					}
				}
			}
		}
		if (lp == mpe->m_linePointer || progress_cancelMonitor(1)) {
			break;
		}
	}
	progress_closeMonitor(0);
	render_repaintAllForFile(fp);
	if ((ret & aligncol) >= 0) {
		caret_placeCursorInCurrentFile(wp, wp->caret.ln,(long )aligncol);
	}
	return ret;
}

