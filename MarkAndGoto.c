/*
 * MarkAndGoto.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: maintain a list of the last "goto tag" positions
 *
 * 										created: Jan. 1990
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include "lineoperations.h"
#include "pksedit.h"
#include "edierror.h"
#include "stringutil.h"
#include "crossreferencelinks.h"

#define	NFILEMARKS		16

typedef struct filemark {
	char **curr;
	char **max;
	char *tag[NFILEMARKS];
} FILEMARK;

static FILEMARK
	_tflist = {			/* last 16 tags */
		_tflist.tag, 
		&_tflist.tag[NFILEMARKS-1]
	};

/*--------------------------------------------------------------------------
 * fm_freepos()
 */
#define	fm_freepos(p)	destroy(p)

/*--------------------------------------------------------------------------
 * fm_getincr()
 */
static char **fm_getincr(int incr)
{	FILEMARK  *fp;
	char 	**p;

	fp = &_tflist;
	p  = fp->curr;
	fp->curr += incr;

	if (fp->curr < fp->tag)
		fp->curr = fp->max;
	else
	if (fp->curr > fp->max)
		fp->curr = fp->tag;

	if (incr < 0 && *p && *fp->curr == 0)
		fp->curr = p;

	return fp->curr;
}

/*--------------------------------------------------------------------------
 * fm_savepos()
 * Save a filemark position, so one can later navigate back to the position.
 */
static int avoidq;
int fm_savepos(char *tag)
{	char **tp;

	if (avoidq)
		return 0;

	tp = fm_getincr(1);

	fm_freepos(tp);

	if ((*tp = _strdup(tag)) == 0)
		return 0;

	return 1;
}

/*--------------------------------------------------------------------------
 * fm_mktagstring()
 * Create a String for a file mark.
 */
void fm_mktagstring(char *buf, char *bufend)
{	char *s,**tp,*bufstart;
	FILEMARK *fmp = &_tflist;
	int  n;

	tp = fmp->curr - 4;
	if (tp < fmp->tag)
		tp += NFILEMARKS;

	while(*buf)
		buf++;
	bufstart = buf;
	bufend  -= 4;

	for(n = 0;;) {
		if ((s = *tp) != 0) {
			n++;
			while(*s && buf < bufend)
				*buf++ = *s++;
		} else
			buf = bufstart;
		if (tp == fmp->curr || buf >= bufend)
			break;
		tp++;
		if (tp > fmp->max)
			tp = fmp->tag;
		if (*tp && n) {
			*buf++ = ':';
			*buf++ = ' ';
		}
	}
	*buf = 0;
}

/*--------------------------------------------------------------------------
 * EdGotoPreviousTag()
 */
int EdGotoPreviousTag(void)
{	int		ret;
	char 	**tp;

	tp = fm_getincr(-1);

	if (*tp == 0) {
		error_showErrorById(IDS_MSGNOLASTTAG);
		return 0;
	}

	avoidq = 1;	
	ret = xref_navigateCrossReference(*tp);
	avoidq = 0;

	return  ret;

}


