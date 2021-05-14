/*
 * CompareFiles.c
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS 3.0
 *
 * purpose: compare the lines of two windows
 * 		  diffs are marked in both windows, last equal line is positioned
 * 		  corresponding to the current setting of "cursor after search"
 *
 * 										created      : 01.01.90
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>

#include "trace.h"

#include "winterf.h"
#include "lineoperations.h"
#include "winfo.h"
#include "pksedit.h"

#define	different(lp1,lp2)	cmpnormal(lp1->lbuf,lp1->len,lp2->lbuf,lp2->len)
#define	mark(lp)			lp->lflg |= LNXMARKED

extern LINE 	*ln_findbit(LINE *lp, int bit);
extern FTABLE	*ww_stackwi(int num);

/*--------------------------------------------------------------------------
 * findmatch()
 */
/*
	strategy is poor:

	MINFRAG is the minimum # of lines which should be equal
   	otherwise they are not matched against each other.
   	This trick helps keeping track, if a few equal lines 
	are detected at the cost of loosing some frags of equal
	(less then MINFRAG) lines.

	Example:

	a b c d e d e f g h i
	a e f g h i

	matching chunks

	straight forward:	<a>-<a>,<e>-<e>
	with MINFRAG trick: <a>-<a>,<f g h i> - <f g h i>
*/

int MINFRAG = 5;		
static LINE *findmatch(LINE *lp1,LINE *lp2)
{	int n;
	LINE *l1,*l2;

	while(lp2) {
		if (!different(lp1,lp2)) {
			n  = MINFRAG;
			l1 = lp1; l2 = lp2;
			do {
				if ((l1 = l1->next) == 0 ||
				    (l2 = l2->next) == 0 ||
				    --n < 0) goto out;
			} while(!different(l1,l2));
		}
		lp2 = lp2->next;
	}
out:
	return lp2;
}

/*--------------------------------------------------------------------------
 * matchlines()
 */
static long matchlines(FTABLE *fp1,FTABLE *fp2,int dir)
{	
	register	LINE *lp1,*lp2,*lp,*lps1,*lps2,*lnlast1,*lnlast2;
	register	long nmatch = 0,l1,l2;

	if ((lp1 = ln_findbit(fp1->firstl,LNDIFFMARK)) == (LINE *) 0 ||
	    (lp2 = ln_findbit(fp2->firstl,LNDIFFMARK)) == (LINE *) 0 || 
	    dir < 0) {

	     lps1    = lp1; lps2    = lp2;
		lnlast1 = 0;   lnlast2 = 0;
 		lp1 = fp1->firstl;
		lp2 = fp2->firstl;
	} else {
		while(lp1->lflg & LNXMARKED)
			lp1 = lp1->next;

		while(lp2->lflg & LNXMARKED)
			lp2 = lp2->next;
	}
	while (lp1 && lp2) {
		if (different(lp1,lp2)) {
			if (dir < 0) {
				if (P_EQ(lps1,lp1) && P_EQ(lps2,lp2)) {
					if (lnlast1 == 0 || lnlast2 == 0)
						goto out0;
					lp1 = lnlast1;
					lp2 = lnlast2;
					dir = 0;
				} else {
					if (lnlast1) {
						ln_um(lnlast1,lp1,LNXMARKED);
						ln_um(lnlast2,lp2,LNXMARKED);
					}
					lnlast1 = lp1;
					lnlast2 = lp2;
				}
			}
			if (dir >= 0) {
				if (dir > 0) {
					lps1 = fp1->firstl;
					lps2 = fp2->firstl;
				}
				ln_um(lps1,(LINE *)0L,LNXMARKED|LNDIFFMARK);
				ln_um(lps2,(LINE *)0L,LNXMARKED|LNDIFFMARK);
				lp1->lflg |= LNDIFFMARK;
				lp2->lflg |= LNDIFFMARK;
				l1 = ln_cnt(fp1->firstl,lp1) - 1;
				l2 = ln_cnt(fp2->firstl,lp2) - 1;
				center2cpos(fp1,l1,0L);
				center2cpos(fp2,l2,0L);
				if (!dir)
					goto out;
			}
			do {
				if ((lp = findmatch(lp1,lp2)) != (LINE *)0) {
					while (P_NE(lp2,lp)) {
						mark(lp2);
						lp2 = lp2->next;
					}
					if (dir > 0)
						goto out;
					else {
						goto advance;
					}
				}
				mark(lp1);
				lp1 = lp1->next;
			} while(lp1);
			break;
		} else {
advance:
			nmatch++;
			lp1 = lp1->next;
			lp2 = lp2->next;
		}
	}
out0:
	ed_error(IDS_MSGNOMOREDIFFS);
	return nmatch;

out:
	redrawallwi(0);
	return nmatch;
}

/*--------------------------------------------------------------------------
 * EdFilesCompare()
 */
EXPORT int EdFilesCompare(int dir)
{	FTABLE *fp0,*fp1;

	if ((fp0 = ww_stackwi(0)) == 0 ||
	    (fp1 = ww_stackwi(1)) == 0) {
		ed_error(IDS_MSGDIFFTWOWINDOWS);
		return 0;
	}
	return matchlines(fp0,fp1,dir);
}

