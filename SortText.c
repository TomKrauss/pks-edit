/*
 * SortText.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: sort text records of variable length (even multiline)
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

# include	<string.h>
# include	"edctype.h"
# include	"editab.h"
# include	"pksedit.h"
# include	"edfuncs.h"
# include	"edierror.h"

# define	MAXARG			128
# define	MAXKEYS			8
# define	MAXDEPTH			6

typedef struct dvec {
	long n;
	int	w;
} DVEC[6];

# define	K_UNIQ			0x1		/* skip records whith this key uniq */
# define	K_SKIPWHITE		0x2
# define	K_SORTDICT		0x4

typedef struct {
	char	flag;
	int  ln,fld,off;	/* line # in EdMacroRecord, field in line + offset */
	int  (*cmp)(unsigned char *s1,int l1,unsigned char *s2, int l2);
} KEY;

# define	KT_MKTOKEN		0x1

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
# define	MAXREC			(unsigned int)((32000L)/sizeof(RECORD))

static long	_nlines;
static int	_sortflags;
static RECORD	*_rectab;

extern char	_expbuf[];
extern char	*_strtolend;

extern long	Atol(char *s);
extern long	ln_cnt(LINE *lp1, LINE *lp2);

extern int  cmpfold   (unsigned char *s1, int l1,unsigned char *s2, int l2);
extern int  cmpnormal (unsigned char *s1, int l1,unsigned char *s2, int l2);
extern int  crunchdict(unsigned char *d,  unsigned char *s,int l);

/*--------------------------------------------------------------------------
 * s2vec()
 * convert a string to a dig. arr
 */
char *s2vec(struct dvec *v,char *s, char *e)
{	int i;

	for (i = 0; i < MAXDEPTH; i++,v++) {
		if (s >= e)
			v->n = 0;
		else {
			v->n = Atol(s);
			v->w = _strtolend-s;
			s    = _strtolend;
		}
		if (*s == '.')
			s++;
	}
	return s;
}

/*--------------------------------------------------------------------------
 * cmpdigit()
 * compare 2 digits
 */
static int dcmp(char *s1, int l1, char *s2, int l2, int digwise)
{	struct dvec v1[MAXDEPTH],v2[MAXDEPTH],*vp;
	int i,n;

	s2vec(v1,s1,s1+l1);
	s2vec(v2,s2,s2+l2);
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
 * cmpdigit()
 */
static int cmpdigit(unsigned char *s1, int l1, unsigned char *s2, int l2)
{
	return dcmp(s1, l1, s2, l2, 1);
}

/*--------------------------------------------------------------------------
 * cmpdate()
 */
static int cmpdate(unsigned char *s1, int l1, unsigned char *s2, int l2)
{
	return dcmp(s1, l1, s2, l2, 0);
}

/*--------------------------------------------------------------------------
 * tokenize()
 */
static int tokenize(STRVEC *vec, unsigned char *s, unsigned char *fs_set, int skipseps)
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
			ed_error(IDS_MSGTOOMUCHFIELDS);
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
 * initset()
 */
static void initset(char *set, char *fs)
{	unsigned char o;
	extern char *_octalloc;

	if (!*fs)
		fs = " \t";

	blfill(set,256,0);
	while((o=*fs++) != 0) {
		if (o == '\\') {
			o  = octalinput(fs);
			fs = _octalloc;
		}
		set[o] = 1;
	}
}

/*--------------------------------------------------------------------------
 * initkeylist()
 */
static void initkeylist(char *s, char *fs_set)
{	STRVEC 	v;
	DVEC		d;
	char		*s2;
	KEY		*kp;
	char		loc_set[256];
	int    	i;
	int		(*cmp)(unsigned char *s1,int l1, unsigned char *s2, int l2);

	/* init to zero */
	blfill(&_keytab,sizeof(_keytab),0);

	if (!*s) {
		i = 1;
		_keytab.k[0].cmp = cmpnormal;
	} else {

		initset(loc_set,",");
		tokenize(&v, s, loc_set, 1);
		if (v.ac > MAXKEYS) {
			ed_error(IDS_MSGTOOMUCHSORTKEYS);
			v.ac = 8;
		}
		for (i = 0; i < v.ac; i++) {
			s2 = s2vec(d,v.so[i],v.eo[i]);
			kp = &_keytab.k[i];
			kp->ln   = d[0].n;
			kp->fld  = d[1].n;
			kp->off  = d[2].n;
			cmp      = cmpnormal;
			if (d[1].n)
				_keytab.fl |= KT_MKTOKEN;
			while (s2 < v.eo[i]) {
				switch(*s2) {
					case 'd':	cmp = cmpdigit; break;
					case 'D': cmp = cmpdate;	 break;
					case 'i': cmp = cmpfold;  break;
					case 'a': kp->flag |= K_SORTDICT; break;
					case 'b': kp->flag |= K_SKIPWHITE; break;
					case 'u': kp->flag |= K_UNIQ; break;
					default : ed_error(IDS_MSGINVALSORTOPT);
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
 * gtln()
 */
static LINE *gtln(RECORD *rp, int num)
{	LINE *lp = rp->lp;

	while (num > 0) {
		lp = lp->next;
		num--;
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * compare()
 */
static int compare(RECORD *rp1, RECORD *rp2)
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
			lp1 = gtln(rp1,l1);
			lp2 = gtln(rp2,l1);
		}

	/* evtl. split the lines according to IFS */
		if (_keytab.fl & KT_MKTOKEN) {
			if (i == 0 || kp[-1].ln != kp->ln) {
				tokenize(&v1, lp1->lbuf, _keytab.fs_set, _sortflags & SO_SKIPSEPS);
				tokenize(&v2, lp2->lbuf, _keytab.fs_set, _sortflags & SO_SKIPSEPS);
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
			l1 = crunchdict(_linebuf,s1,l1);
			s1 = _linebuf;
			l2 = crunchdict(_linebuf+2048,s2,l2);
			s2 = _linebuf+2048;
		} else	/* is implied above */
		if (kp->flag & K_SKIPWHITE) {
			while (IsSpace(*s1)) {
				s1++;
				l1--;
			}
			while (IsSpace(*s2)) {
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
 * sortlist()
 * Sortieren der Zeilenliste
 * (Quicksort nach K&R)
 */
int sortlist(RECORD *tab,long n)
{    long	gap,i,j,jplusgap;
	int  ret;
	RECORD tmp;
	
	for (gap = n/2; gap > 0; gap >>= 1) {
		for (i = gap; i < n; i++) {
			for (j = i-gap; j >= 0; j -= gap) {
				jplusgap = j+gap;
				
				ret = compare(&tab[j],&tab[jplusgap]);
				
				if (_sortflags & SO_REVERSE) {
					if (ret >= 0) break;
				} else if (ret <= 0) break;
	
				if (ed_abort(0))
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
 * lp2rec()
 * build a EdMacroRecord tab, using a line list
 */
static int lp2rec(LINE *lpfirst, LINE *lplast, 
			   char *selebuf, int sortflags,
			   RECORD *rectab)
{	int 	nrec,nl;
	LINE *lpnext;
 
	for (nrec = 0;;) {
		if ((sortflags & SO_NOSELECT) || 
			step(lpfirst->lbuf,selebuf,&lpfirst->lbuf[lpfirst->len])) {
			if (nrec >= MAXREC) {
				ed_error(IDS_MSGTOOMUCHRECORDS);
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
					if (step(lpnext->lbuf,selebuf,&lpnext->lbuf[lpnext->len]))
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
static int mk2ndlist(LINE *lpfirst, LINE *lplast,int n)
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
void ln_order(FTABLE *fp, RECORD *rectab, RECPARAMS *rp)
{	int i,j,n;
	LINE *lp,*lpd,*lpend;

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
	fp->currl 	= fp->firstl;
	fp->flags	    |= F_CHANGEMARK;
}

/*--------------------------------------------------------------------------
 * undo_cash()
 */
static int undo_cash(FTABLE *fp, LINE *lpfirst, LINE *lplast)
{	RECORD 	*rec,*rp;
	RECPARAMS *recpar;
	LINE	  	*lp;
	long   	i,nrec,size;

	size  = ln_cnt(lpfirst,lplast);
	nrec  = size;
	size *= sizeof(RECORD);
	if ((rec  = _alloc(size)) == 0)
		return 0;

	for (lp = lpfirst, rp = rec; ; lp = lp->next, rp++) {
		rp->lp = lp;
		rp->nl = 1;
		if (lp == lplast)
			break;
	}

	if ((recpar = _alloc(sizeof(RECPARAMS))) == 0) {
		_free(rec);
		return 0;
	}
	recpar->lpfirst = lpfirst->prev;
	recpar->lplast  = lplast ->next;
	recpar->nrec    = nrec;

	u_cash(fp,rec,recpar,O_LNORDER);

	return 1;
}

/*--------------------------------------------------------------------------
 * sort()
 */
int Sort(int scope, char *fs, char *keys, char *sel, int sortflags)
{	int  	ret = 0, n,n2;
	char		fs_set[256];
	RECPARAMS rp;
	FTABLE	*fp;
	long  	l1;
	LINE 	*lpfirst, *lplast;
	MARK		*mps,*mpe;

	if (!sel[0]) {
		sortflags |= SO_NOSELECT;
		sortflags &= ~SO_CLUSTERLINES;
	} else {
		if (!exprinit(sel))
			return 0;
	}

	fp = _currfile;
	u_init(fp);
	if (select_range(scope,fp,&mps,&mpe) == RNG_INVALID) {
		return 0;
	}
	lpfirst = mps->lm;
	lplast =  mpe->lm;
	Pastehide(0);

	initset(fs_set,fs);
	initkeylist(keys,fs_set);
	_sortflags = sortflags;

	l1 = fp->ln;
	if ((_rectab = _alloc((unsigned )(MAXREC*sizeof(RECORD)))) == 0) {
		return 0;
	}

	if ((n  = lp2rec(lpfirst,lplast,_expbuf,_sortflags,_rectab)) != 0) {
		n2 = mk2ndlist(lpfirst,lplast,n);
		rp.lpfirst = lpfirst->prev;
		rp.lplast  = lplast ->next;
		rp.nrec    = n+n2;
		abrt_start(IDS_ABRTSORT);
		undo_cash(fp,lpfirst,lplast);
		if (sortlist(_rectab,n)) {
			curpos(0L,0L);
			ln_order(fp,_rectab,&rp);
		}
		ln_um(fp->firstl,fp->lastl,LNREPLACED);
		abrt_close(0);
		RedrawTotalWindow(fp);
		curpos(l1,0L);
		ret = 1;
	}

	_free(_rectab);
	_rectab = 0;

	return ret;
}
