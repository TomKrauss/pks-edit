/*
 * SortText.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: sort text records of variable length (even multiline)
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <string.h>

#include "alloc.h"
#include "edctype.h"
#include "caretmovement.h"
#include "winfo.h"
#include "textblocks.h"
#include "pksedit.h"
#include "edfuncs.h"
#include "edierror.h"
#include "stringutil.h"
#include "regexp.h"

#define	MAXARG			128
#define	MAXKEYS			8
#define	MAXDEPTH			6

typedef struct dvec {
	long n;
	int	w;
} DVEC[6];

#define	K_UNIQ			0x1		/* skip records whith this key uniq */
#define	K_SKIPWHITE		0x2
#define	K_SORTDICT		0x4

typedef struct {
	char	flag;
	int  ln,fld,off;	/* line # in EdMacroRecord, field in line + offset */
	int  (*cmp)(unsigned char *s1,int l1,unsigned char *s2, int l2);
} KEY;

#define	KT_MKTOKEN		0x1

typedef struct keytab {
	int	nkeys;
	int  fl;
	unsigned char *fs_set;
	KEY	k[MAXKEYS];
} KEYTAB;

static KEYTAB _keytab;

typedef struct {
	LINE	*lp;
	int	nl;
} RECORD;

typedef struct recparams {
	LINE *lpfirst,*lplast;
	int  nrec;		/* may not supersede ~4000 , so (int) is enough */
} RECPARAMS;

typedef struct strvec {
	char		*s;			/* the buffer */
	int		ac;			/* # of tokens in line */
	char		*so[MAXARG];	/* start token # ... */
	char		*eo[MAXARG];	/* end token # ... */
	int		lo[MAXARG];	/* len token # ... */
} STRVEC;

/*
 * this is a hack: use the formular backingstore as temporary
 * buffer for the line list
 */
#define	MAXREC			(unsigned int)((32000L)/sizeof(RECORD))

static long	_nlines;
static int	_sortflags;
static RECORD	*_rectab;

extern int  compare_strings(unsigned char* s1, int l1, unsigned char* s2, int l2);

/*------------------------------------------------------------
 * sor_compareStringsCaseIgnore()
 * Like sort_compareRecords strings, but case ignore.
 */
static int sor_compareStringsCaseIgnore(unsigned char* s1, int l1, unsigned char* s2, int l2)
{
	int      len;

	if (l1 > l2)
		len = l2;
	else len = l1;

	while (len > 0) {
		if (_l2uset[*s1++] != _l2uset[*s2++]) {
			l1 = _l2uset[s1[-1]];
			l2 = _l2uset[s2[-1]];
			break;
		}
		else
			len--;
	}
	return l1 - l2;
}

/*------------------------------------------------------------
 * sort_compareExtractKeyLetterWord()
 */
static int sort_compareExtractKeyLetterWord(unsigned char* d, unsigned char* s, int l)
{
	unsigned char* bufferStart = d, c;

	if (l > 2048) l = 2048;			/* may fail on large keys */
	while (l > 0) {
		if (isalnum((c = *s++)))
			*d++ = c;
		l--;
	}
	return (int)(d - bufferStart);
}


/*--------------------------------------------------------------------------
 * sort_convertStringToDigitArray()
 * convert a string to a dig. arr
 */
static char *sort_convertStringToDigitArray(struct dvec *v,char *s, char *e)
{	int i;

	for (i = 0; i < MAXDEPTH; i++,v++) {
		if (s >= e)
			v->n = 0;
		else {
			v->n = string_convertToLong(s);
			v->w = (int)(_strtolend-s);
			s    = _strtolend;
		}
		if (*s == '.')
			s++;
	}
	return s;
}

/*--------------------------------------------------------------------------
 * sort_compareDigit()
 * sort_compareRecords 2 digits
 */
static int sort_genericDigitCompare(char *s1, int l1, char *s2, int l2, int digwise)
{	struct dvec v1[MAXDEPTH],v2[MAXDEPTH],*vp;
	int i,n;

	sort_convertStringToDigitArray(v1,s1,s1+l1);
	sort_convertStringToDigitArray(v2,s2,s2+l2);
	for (i = 0; i < MAXDEPTH; i++) {
		if (v1[i].n != v2[i].n) {
			if (i != 0 && digwise) {
				/* .101 < .2 ! */
				vp = &v2[i];
				if ((n = vp->w-v1[i].w) > 0) vp = &v1[i];
				else n = -n;
				while (n > 0) {
					n--;
					vp->n *= 10;
				}
			}
			return (v1[i].n>v2[i].n) ? 1 : -1;
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * sort_compareDigit()
 */
static int sort_compareDigit(unsigned char *s1, int l1, unsigned char *s2, int l2)
{
	return sort_genericDigitCompare(s1, l1, s2, l2, 1);
}

/*--------------------------------------------------------------------------
 * sort_compareDate()
 */
static int sort_compareDate(unsigned char *s1, int l1, unsigned char *s2, int l2)
{
	return sort_genericDigitCompare(s1, l1, s2, l2, 0);
}

/*--------------------------------------------------------------------------
 * sort_tokenize()
 */
static int sort_tokenize(STRVEC *vec, unsigned char *s, unsigned char *fs_set, int skipseps)
{	int  i,i1;
	int  ac;
	unsigned char c;

	vec->s = s;
	vec->ac= 0;
	i 	  = 0;
	ac 	  = 0;
	for (; ; ) {
		if (skipseps) {
			while((c = s[i]) != 0 && fs_set[c])
				i++;
		}
		if (ac > MAXARG) {
			error_showErrorById(IDS_MSGTOOMUCHFIELDS);
			return 0;
		}
		vec->so[ac] = &s[i];
		i1          = i;
		while((c = s[i]) != 0 && !fs_set[c])
			i++;
		vec->eo[ac] 	= &s[i];
		vec->lo[ac++]	= i-i1;
		if (!c) break;
		if (!skipseps)
			i++;
	}
	vec->ac = ac;
	return 1;
}

/*--------------------------------------------------------------------------
 * sort_initializeFieldSeparators()
 */
static void sort_initializeFieldSeparators(char *set, char *pFieldSeparators)
{	unsigned char o;

	if (!*pFieldSeparators)
		pFieldSeparators = " \t";

	memset(set,0,256);
	while((o=*pFieldSeparators++) != 0) {
		if (o == '\\') {
			o  = regex_parseOctalNumber(pFieldSeparators);
			pFieldSeparators = _octalloc;
		}
		set[o] = 1;
	}
}

/*--------------------------------------------------------------------------
 * sort_initializeKeyList()
 */
static void sort_initializeKeyList(char *s, char *fs_set)
{	STRVEC 	v;
	DVEC		d;
	char		*s2;
	KEY		*kp;
	char		loc_set[256];
	int    	i;
	int		(*cmp)(unsigned char *s1,int l1, unsigned char *s2, int l2);

	/* init to zero */
	memset(&_keytab,0,sizeof(_keytab));

	if (!*s) {
		i = 1;
		_keytab.k[0].cmp = compare_strings;
	} else {

		sort_initializeFieldSeparators(loc_set,",");
		sort_tokenize(&v, s, loc_set, 1);
		if (v.ac > MAXKEYS) {
			error_showErrorById(IDS_MSGTOOMUCHSORTKEYS);
			v.ac = 8;
		}
		for (i = 0; i < v.ac; i++) {
			s2 = sort_convertStringToDigitArray(d,v.so[i],v.eo[i]);
			kp = &_keytab.k[i];
			kp->ln   = d[0].n;
			kp->fld  = d[1].n;
			kp->off  = d[2].n;
			cmp      = compare_strings;
			if (d[1].n)
				_keytab.fl |= KT_MKTOKEN;
			while (s2 < v.eo[i]) {
				switch(*s2) {
					case 'd':	cmp = sort_compareDigit; break;
					case 'D': cmp = sort_compareDate;	 break;
					case 'i': cmp = sor_compareStringsCaseIgnore;  break;
					case 'a': kp->flag |= K_SORTDICT; break;
					case 'b': kp->flag |= K_SKIPWHITE; break;
					case 'u': kp->flag |= K_UNIQ; break;
					default : error_showErrorById(IDS_MSGINVALSORTOPT);
				}
				s2++;
			}
			kp->cmp = cmp;
		}
	}
	_keytab.fs_set = fs_set;
	_keytab.nkeys  = i;
}

/*--------------------------------------------------------------------------
 * sort_getLineFromRecord()
 */
static LINE *sort_getLineFromRecord(RECORD *rp, int num)
{	LINE *lp = rp->lp;

	while (num > 0) {
		lp = lp->next;
		num--;
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * sort_compareRecords()
 */
static int sort_compareRecords(RECORD *rp1, RECORD *rp2)
{	int 		i,off,ret,l1,l2;
	LINE 	*lp1,*lp2,*lp;
	KEY		*kp;
	STRVEC 	v1,v2;
	unsigned char *s1,*s2;

	lp1 = rp1->lp;
	lp2 = rp2->lp;
	for (i = 0; i < _keytab.nkeys; i++) {
		kp = &_keytab.k[i];

	/* skip 2 first key line of each rec */
		if ((l1 = kp->ln) != 0) {
			if (l1 >= rp1->nl) {
				if (l1 < rp2->nl)
					return -1;
				else
					continue;
			} else if (l1 >= rp2->nl)
				return 1;
			lp1 = sort_getLineFromRecord(rp1,l1);
			lp2 = sort_getLineFromRecord(rp2,l1);
		}

	/* evtl. split the lines according to IFS */
		if (_keytab.fl & KT_MKTOKEN) {
			if (i == 0 || kp[-1].ln != kp->ln) {
				sort_tokenize(&v1, lp1->lbuf, _keytab.fs_set, _sortflags & SO_SKIPSEPS);
				sort_tokenize(&v2, lp2->lbuf, _keytab.fs_set, _sortflags & SO_SKIPSEPS);
			}
		}
		off = kp->fld;
		if (--off < 0)	{			/* the token $0, (whole line) */
			s1 = lp1->lbuf;
			s2 = lp2->lbuf;
			l1 = lp1->len;
			l2 = lp2->len;
		} else {					/* token $off	*/
			if (off >= v1.ac) {		/* check whether $off exists */
				if (off < v2.ac)
					return -1;
				else
					continue;
			} else if (off >= v2.ac)
		   		return 1;
			s1 = v1.so[off];
			s2 = v2.so[off];
			l1 = v1.lo[off];
			l2 = v2.lo[off];
		}

	/* add opt. offset to begin of key */
		if ((off = kp->off) != 0) {
			s1 += off;
			s2 += off;
			l1 -= off;
			l2 -= off;
		}

	/* skip 2 first key line of each rec */
		if (kp->flag & K_SORTDICT) {
			l1 = sort_compareExtractKeyLetterWord(_linebuf,s1,l1);
			s1 = _linebuf;
			l2 = sort_compareExtractKeyLetterWord(_linebuf+2048,s2,l2);
			s2 = _linebuf+2048;
		} else	/* is implied above */
		if (kp->flag & K_SKIPWHITE) {
			while (string_isSpace(*s1)) {
				s1++;
				l1--;
			}
			while (string_isSpace(*s2)) {
				s2++;
				l2--;
			}
		}

		if ((ret = (*kp->cmp)(s1,l1,s2,l2)) != 0)
			return ret;

		if (kp->flag & K_UNIQ)
			for (off = 0, lp=rp2->lp; off < rp2->nl; off++, lp=lp->next)
				lp->lflg |= LNXMARKED;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * sort_quickSortList()
 * Sortieren der Zeilenliste
 * (Quicksort nach K&R)
 */
int sort_quickSortList(RECORD *tab,long n)
{    long	gap,i,j,jplusgap;
	int  ret;
	RECORD tmp;
	
	for (gap = n/2; gap > 0; gap >>= 1) {
		for (i = gap; i < n; i++) {
			for (j = i-gap; j >= 0; j -= gap) {
				jplusgap = j+gap;
				
				ret = sort_compareRecords(&tab[j],&tab[jplusgap]);
				
				if (_sortflags & SO_REVERSE) {
					if (ret >= 0) break;
				} else if (ret <= 0) break;
	
				if (progress_cancelMonitor(0))
					return 0;
				/* swap these fellows */
				tmp = tab[j]; 
				tab[j] = tab[jplusgap];
				tab[jplusgap] = tmp;
	
			}
		}
	 }
	 return 1;
}

/*--------------------------------------------------------------------------
 * sort_createRecordsFromLines()
 * build a EdMacroRecord tab, using a line list
 */
static int sort_createRecordsFromLines(LINE *lpfirst, LINE *lplast, 
			   RE_PATTERN *pPattern, int sortflags,
			   RECORD *rectab)
{	int 	nrec,nl;
	LINE *lpnext;
	RE_MATCH match;
 
	memset(&match, 0, sizeof match);
	for (nrec = 0;;) {
		if ((sortflags & SO_NOSELECT) || 
			regex_match(pPattern, lpfirst->lbuf,&lpfirst->lbuf[lpfirst->len], &match)) {
			if (nrec >= MAXREC) {
				error_showErrorById(IDS_MSGTOOMUCHRECORDS);
				return 0;
			}
			rectab[nrec].lp = lpfirst;
			nl              = 1;
			if (sortflags & SO_CLUSTERLINES) {
				for (;;) {
					lpfirst->lflg |= LNREPLACED;
					if (lpfirst == lplast)
						break;
					lpnext = lpfirst->next;
					if (regex_match(pPattern, lpnext->lbuf,&lpnext->lbuf[lpnext->len], &match))
						break;
					lpfirst = lpnext;
					nl++;
				}
			} else 
				lpfirst->lflg |= LNREPLACED;
			_rectab[nrec++].nl = nl;
		}
		if (lpfirst == lplast)
			return nrec;
		lpfirst = lpfirst->next;
	}
}

/*--------------------------------------------------------------------------
 * mk2cndlist()
 * group the not selected lines
 */
static int sort_groupUnselectedLines(LINE *lpfirst, LINE *lplast,int n)
{	int i;

	for (i = n; ;lpfirst = lpfirst->next) {
		if ((lpfirst->lflg & LNREPLACED) == 0) {
			_rectab[i].lp = lpfirst;
			_rectab[i].nl = 1;
			i++;
		}
		if (lpfirst == lplast)
			break;
	}
	return i-n;
}

/*--------------------------------------------------------------------------
 * ln_order()
 */
void ln_order(FTABLE *fp, void *p1, void *p2) {	
	int i,j,n;
	LINE *lpd,*lpend;
	RECORD* rectab;
	RECPARAMS* rp;

	rectab = p1;
	rp = p2;
	n	   = rp->nrec;
	lpend   = rp->lplast;
	i       = 0;
	if (!rp->lpfirst) {
		fp->firstl = rectab[0].lp;
		lpd        = 0;
		goto advance;
	} else lpd = rp->lpfirst;
	for (; i < n; i++) {
		lpd->next = rectab[i].lp;
advance:
		rectab[i].lp->prev = lpd;
		lpd = rectab[i].lp;
		for (j = 1; j < rectab[i].nl; j++)
			lpd = lpd->next;
	}
	lpd  ->next	= lpend;
	lpend->prev	= lpd;
	WINFO* wp = WIPOI(fp);
	wp->caret.linePointer = fp->firstl;
	ft_setFlags(fp, fp->flags | F_CHANGEMARK);
}

/*--------------------------------------------------------------------------
 * undo_cash()
 */
static int undo_cash(FTABLE *fp, LINE *lpfirst, LINE *lplast)
{	RECORD 	*rec,*rp;
	RECPARAMS *recpar;
	LINE	  	*lp;
	long   	nrec,size;

	size  = ln_cnt(lpfirst,lplast);
	nrec  = size;
	size *= sizeof(RECORD);
	if ((rec  = malloc(size)) == 0)
		return 0;

	for (lp = lpfirst, rp = rec; ; lp = lp->next, rp++) {
		rp->lp = lp;
		rp->nl = 1;
		if (lp == lplast)
			break;
	}

	if ((recpar = malloc(sizeof(RECPARAMS))) == 0) {
		free(rec);
		return 0;
	}
	recpar->lpfirst = lpfirst->prev;
	recpar->lplast  = lplast ->next;
	recpar->nrec    = nrec;

	undo_saveOperation(fp,(LINE*)rec, (LINE*)recpar,O_LNORDER);

	return 1;
}

/*--------------------------------------------------------------------------
 * ft_sortFile()
 * Sort the current file / document.
 */
int ft_sortFile(FTABLE* fp, int scope, char *fs, char *keys, char *sel, int sortflags)
{	int  	ret = 0, n,n2;
	char		fs_set[256];
	RECPARAMS rp;
	long  	l1;
	LINE 	*lpfirst, *lplast;
	MARK		*mps,*mpe;
	RE_PATTERN* pattern;
	WINFO* wp = WIPOI(fp);

	if (!sel[0]) {
		sortflags |= SO_NOSELECT;
		sortflags &= ~SO_CLUSTERLINES;
	} else {
		if ((pattern = regex_compileWithDefault(sel)) == NULL) {
			return 0;
		}
	}

	undo_startModification(fp);
	if (find_setTextSelection(scope,wp,&mps,&mpe) == RNG_INVALID) {
		return 0;
	}
	lpfirst = mps->lm;
	lplast =  mpe->lm;
	bl_hideSelection(wp, 0);

	sort_initializeFieldSeparators(fs_set,fs);
	sort_initializeKeyList(keys,fs_set);
	_sortflags = sortflags;

	l1 = wp->caret.ln;
	if ((_rectab = malloc((unsigned )(MAXREC*sizeof(RECORD)))) == 0) {
		return 0;
	}

	if ((n  = sort_createRecordsFromLines(lpfirst,lplast,pattern,_sortflags,_rectab)) != 0) {
		n2 = sort_groupUnselectedLines(lpfirst,lplast,n);
		rp.lpfirst = lpfirst->prev;
		rp.lplast  = lplast ->next;
		rp.nrec    = n+n2;
		progress_startMonitor(IDS_ABRTSORT);
		undo_cash(fp,lpfirst,lplast);
		if (sort_quickSortList(_rectab,n)) {
			caret_placeCursorInCurrentFile(wp,0L,0L);
			ln_order(fp,_rectab,&rp);
		}
		ln_removeFlag(fp->firstl,fp->lastl,LNREPLACED);
		progress_closeMonitor(0);
		render_repaintAllForFile(fp);
		caret_placeCursorInCurrentFile(wp,l1,0L);
		ret = 1;
	}

	free(_rectab);
	_rectab = 0;

	return ret;
}
