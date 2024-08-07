/*
 * SortText.c
 *
 * Project: PKS Edit for Windows
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
#include <time.h>

#include "alloc.h"
#include "edctype.h"
#include "caretmovement.h"
#include "winfo.h"
#include "textblocks.h"
#include "pksedit.h"
#include "pksmacro.h"
#include "edierror.h"
#include "findandreplace.h"
#include "stringutil.h"
#include "regexp.h"
#include "publicapi.h"

#define	MAX_SORT_TOKENS			128
#define	MAXKEYS			8
#define	MAXDEPTH		6

#define	K_UNIQ			0x1		// skip records whith this key uniq
#define	K_SKIPWHITE		0x2		// Skip whitespace during comparison
#define	K_SORTDICT		0x4		// Dictionary compare mode 
#define K_DESCENDING	0x8		// sort this key field descending

typedef struct tagKEY {
	char	k_flags;			// one of the K_... flags from above
	int		k_clusterLineIndex;	// line # in cluster to sort
	int		k_fieldIndex;		// number / index of field
	int		k_offset;			// offset into field. Either counting from the beginning of the line or from the 
								// beginning of the text as split using the field separator
	int		k_width;			// width of the field
	const char* k_dateFormat;	// date format for field sorted by date - if not assigned, will be determined heuristically
	int		(*k_compare)(const char *s1, const char *s2);
} KEY;

typedef struct keytab {
	int				kt_numberOfKeys;
	int				kt_tokenizeForComparison;	// whether individual fields should be compared. If whole lines should be compared, this is 0
	unsigned char *	kt_fieldSeparators;			// The (optional) field separators used to split key fields.
	KEY	k[MAXKEYS];
} KEYTAB;

static KEYTAB _keytab;

typedef struct {
	LINE	*lp;		// Line making up one record
	int		flags;		// old line flags
	int		nl;
} RECORD;

typedef struct recparams {
	LINE *lpfirst,*lplast;
	int  nrec;		// number of records in line may not supersede ~4000 , so (int) is enough
} RECPARAMS;

typedef struct tagSORT_TOKEN_LIST {
	char*	stl_source;						// the original input from which the column descriptors were created.
	int		stl_numberOfTokens;				// # of tokens in line
	char*	stl_start[MAX_SORT_TOKENS];		// start token # 
	char*	stl_end[MAX_SORT_TOKENS];		// end token # ... 
	int		stl_length[MAX_SORT_TOKENS];	// len token # ... 
} SORT_TOKEN_LIST;

#define	MAXREC		5000

static long		_nlines;
static SORT_OPTION_FLAGS _sortflags;

#define MAX_KEY_SIZE			 4096
#define LN_MARKED_FOR_CLUSTERING 0x800			// lines were marked to be clustered in the sorting function.

/*
 * C replacement of strptime. Parse the string s given a format f into a time struct. If successful
 * return end of the parsed string NULL otherwise.
 */
extern char* strptime(const char* s, const char* f, struct tm* tm);

static char* _dateFormats[] = {
	// Warning: do not use the short form %D as the length of the pattern matters
	// we need to compare patterns by length to the input text length. If the later is shorter
	// we must not try to parse using that format or we get an exception (shortcoming of std::get_time, which is used internally).
	"%Y-%m-%d %H:%M:%S",
	"%m/%d/%y %H:%M:%S",
	"%m/%d/%y",
	"%d.%m.%Y",
	"%Y-%m-%d",
	"%d. %B %y",
	"%H:%M:%S"
};

static KEY* _currentKey;

/*------------------------------------------------------------
 * sort_compareExtractKeyfield()
 * Extract the key fields to compare from the line.
 */
static int sort_compareExtractKeyfield(unsigned char* d, unsigned char* s, int l, int nFlags, char cQuoteChar)
{
	unsigned char* bufferStart = d, c;
	int bKey = nFlags & K_SORTDICT;
	int bBackslash = _sortflags & SO_BACKSLASH_QUOTING;
	int bSkipSpace = nFlags & K_SKIPWHITE;

	if (l > MAX_KEY_SIZE) l = MAX_KEY_SIZE;			/* may fail on large keys */
	while (l > 0) {
		c = *s++;
		l--;
		if ((bBackslash && c == '\\') || (c == cQuoteChar)) {
			c = *s++;
			if (--l <= 0) {
				break;
			}
		}
		if ((!bSkipSpace || string_isSpace(c))
			|| (!bKey || isalnum(c))) {
			*d++ = c;
		}
	}
	*d = 0;
	return (int)(d - bufferStart);
}


/*
 * Generic heuristic float parsing.
 */
double sort_convertToFloat(const char* s1) {
	double f1 = 0;
	double mult = 1;
	int bNeg = 0;
	char c;
	while ((c = * s1++) != 0) {
		if (c == '-') {
			bNeg = 1;
			break;
		}
		if (c == '.' || c == ',') {
			mult = 0.1;
			break;
		}
		if (isdigit(c)) {
			f1 = (float)(c - '0');
			break;
		}
	}
	while ((c = *s1++) != 0) {
		if (isdigit(c)) {
			if (mult == 1) {
				f1 *= 10;
				f1 += (float)(c - '0');
			}
			else {
				f1 += mult * (float)(c - '0');
				mult /= 10.0;
			}
		}
		else if (mult == 1 && (c == '.' || c == ',')) {
			mult = 0.1;
		}
		else {
			break;
		}
	}
	return bNeg ? -f1 : f1;
}

/*--------------------------------------------------------------------------
 * sort_compareDigit()
 */
static int sort_compareDigit(const char *s1, const char *s2)
{
	double f1 = sort_convertToFloat(s1);
	double f2 = sort_convertToFloat(s2);
	if (f1 > f2) {
		return 1;
	}
	if (f1 < f2) {
		return -1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * sort_compareDate()
 */
static int sort_compareDate(const char *s1, const char *s2) {
	struct tm tm1;
	struct tm tm2;
	const char* pszFmt = 0;
	if (_currentKey && _currentKey->k_dateFormat) {
		if (strptime(s1, _currentKey->k_dateFormat, &tm1)) {
			pszFmt = _currentKey->k_dateFormat;
		}
	}
	if (!pszFmt) {
		for (int i = 0; i < DIM(_dateFormats); i++) {
			pszFmt = _dateFormats[i];
			// if len is less we get an exception.
			if (strlen(s1) >= strlen(pszFmt) && strptime(s1, pszFmt, &tm1)) {
				break;
			}
			pszFmt = 0;
		}
		if (!pszFmt) {
			return 0;
		}
		if (_currentKey) {
			_currentKey->k_dateFormat = pszFmt;
		}
	}
	if (!strptime(s2, pszFmt, &tm2)) {
		return 1;
	}
	time_t t1 = mktime(&tm1);
	time_t t2 = mktime(&tm2);
	double diffSecs = difftime(t1, t2);
	if (diffSecs > 0) {
		return 1;
	}
	if (diffSecs < 0) {
		return -1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * sort_tokenize()
 */
static int sort_tokenize(SORT_TOKEN_LIST *vec, unsigned char *s, int nLen, unsigned char *fs_set, int skipseps, char cQuote) {	
	int  i,i1;
	int  nColumns;
	int  bInQuote = 0;
	unsigned char c;
	int bBackslash = _sortflags & SO_BACKSLASH_QUOTING;

	vec->stl_source = s;
	vec->stl_numberOfTokens = 0;
	i 	  = 0;
	nColumns = 0;
	for (; ; ) {
		if (skipseps) {
			while((c = s[i]) != 0 && fs_set[c])
				i++;
		}
		if (nColumns >= MAX_SORT_TOKENS) {
			error_showErrorById(IDS_MSGTOOMANYFIELDS);
			return 0;
		}
		vec->stl_start[nColumns] = &s[i];
		i1 = i;
		while ((c = s[i]) != 0 && (bInQuote || !fs_set[c])) {
			if (c == cQuote) {
				if (s[i + 1] == cQuote) {
					i++;
				}
				else {
					bInQuote = !bInQuote;
				}
			}
			else if (bBackslash && c == '\\' && s[i + 1]) {
				i++;
			}
			i++;
		}
		vec->stl_end[nColumns] 	= &s[i];
		vec->stl_length[nColumns++]	= i-i1;
		if (!c) break;
		if (!skipseps)
			i++;
	}
	vec->stl_numberOfTokens = nColumns;
	return 1;
}

/*--------------------------------------------------------------------------
 * sort_initializeFieldSeparators()
 */
static void sort_initializeFieldSeparators(char *pFieldSeparatorsCharset, char *pFieldSeparators)
{	unsigned char o;

	if (!*pFieldSeparators)
		pFieldSeparators = " \t";

	memset(pFieldSeparatorsCharset,0,256);
	while((o=*pFieldSeparators++) != 0) {
		if (o == '\\') {
			o  = regex_parseOctalNumber(pFieldSeparators);
			pFieldSeparators = _octalloc;
		}
		pFieldSeparatorsCharset[o] = 1;
	}
}

/*--------------------------------------------------------------------------
 * Compile the sort field options / the "key list".
 * Fields are delimited using "," and may contain whitespace
 * Format for one key (sort field): f[0-9]+{l[0-9]+}?{w[0-9]+}?[dDiabu-]
 * f number: 0-based field number in order of appearance
 * l number: 0-based line number offset into cluster in clustered sorting case
 * o number: optional 0-based character offset of field. If fields are separated using field separators, offset into the 
 *             field otherwise offset into the line sorted
 * w number: optional width of field in number of characters
 * Examples
 * - To sort comparing two fields, the 1st one numeric descending and the 2nd one interpreting the value as Date enforcing a field width
 *   of 10 characters use: f0-d,f1w10D
 */
static void sort_initializeKeyList(char *pszKeySpec) {
	KEY		*kp;
	int    	i;

	/* init to zero */
	memset(&_keytab,0,sizeof(_keytab));

	if (!*pszKeySpec) {
		i = 1;
		_keytab.k[0].k_compare = strcmp;
	}
	else {
		i = 0;
		char c;
		kp = 0;
		while ((c = *pszKeySpec++) != 0) {
			if (kp == 0) {
				kp = &_keytab.k[i++];
				kp->k_compare = strcmp;
			}
			switch (c) {
			case ',':
				i++;
				if (i >= MAXKEYS) {
					error_showErrorById(IDS_MSGTOOMANYSORTKEYS);
					goto done;
				}
				kp = 0;
				break;
			case 'f':
				kp->k_fieldIndex = (int)string_convertToLong(pszKeySpec);
				if (kp->k_fieldIndex)
					_keytab.kt_tokenizeForComparison = 1;
				pszKeySpec = (char*)_strtolend;
				break;
			case 'l':
				kp->k_clusterLineIndex = (int)string_convertToLong(pszKeySpec);
				pszKeySpec = (char*)_strtolend;
				break;
			case 'o':
				kp->k_offset = (int)string_convertToLong(pszKeySpec);
				pszKeySpec = (char*)_strtolend;
				break;
			case 'w':
				kp->k_width = (int)string_convertToLong(pszKeySpec);
				pszKeySpec = (char*)_strtolend;
				break;
			case 'd': kp->k_compare = sort_compareDigit; break;
			case 'D': kp->k_compare = sort_compareDate;	 break;
			case 'i': kp->k_compare = _strcmpi;  break;
			case 'a': kp->k_flags |= K_SORTDICT; break;
			case 'b': kp->k_flags |= K_SKIPWHITE; break;
			case 'u': kp->k_flags |= K_UNIQ; break;
			case '-': kp->k_flags |= K_DESCENDING; break;
			case ' ': case '\t': break;
			default: error_showErrorById(IDS_MSGINVALSORTOPT);
			}
		}
	}
done:
	_keytab.kt_numberOfKeys  = i;
}

/*--------------------------------------------------------------------------
 * sort_getLineFromRecord()
 */
static LINE *sort_getLineFromRecord(const RECORD *rp, int num)
{	LINE *lp = rp->lp;

	while (num > 0) {
		lp = lp->next;
		num--;
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * sort_abortProgress()
 */
static int _cancelled;
static int sort_abortProgress(void)
{
	static int _abort;
	if ((_abort++ & 0x3F) == 0 && progress_cancelMonitor(TRUE)) {
		_cancelled = 1;
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * sort_compareRecords()
 */
static int sort_compareRecords(const RECORD *rp1, const RECORD *rp2) {	
	int 	i,ret,l1,l2;
	int		nFieldIndex;
	int		nOffset;
	LINE 	*lp1,*lp2,*lp;
	KEY		*kp;
	SORT_TOKEN_LIST 	v1,v2;
	unsigned char *s1,*s2;

	if (_cancelled || sort_abortProgress()) {
		return 0;
	}
	lp1 = rp1->lp;
	lp2 = rp2->lp;
	char cQuoteChar = (_sortflags & SO_CSV_QUOTING) ? '"' : 0;
	for (i = 0; i < _keytab.kt_numberOfKeys; i++) {
		kp = &_keytab.k[i];
		BOOL bDescending = kp->k_flags & K_DESCENDING;
	/* skip 2 first key line of each rec */
		if ((l1 = kp->k_clusterLineIndex) != 0) {
			if (l1 >= rp1->nl) {
				if (l1 < rp2->nl)
					return bDescending ? 1 : -1;
				else
					continue;
			} else if (l1 >= rp2->nl)
				return bDescending ? -1 : 1;
			lp1 = sort_getLineFromRecord(rp1,l1);
			lp2 = sort_getLineFromRecord(rp2,l1);
		}

	/* evtl. split the lines according to IFS */
		if (_keytab.kt_tokenizeForComparison) {
			if (i == 0 || kp[-1].k_clusterLineIndex != kp->k_clusterLineIndex) {
				sort_tokenize(&v1, lp1->lbuf, lp1->len, _keytab.kt_fieldSeparators, _sortflags & SO_SKIPSEPARATORS, cQuoteChar);
				sort_tokenize(&v2, lp2->lbuf, lp2->len, _keytab.kt_fieldSeparators, _sortflags & SO_SKIPSEPARATORS, cQuoteChar);
			}
		}
		nFieldIndex = kp->k_fieldIndex;
		if (!_keytab.kt_tokenizeForComparison)	{				// the token $0, (whole line) 
			s1 = lp1->lbuf;
			s2 = lp2->lbuf;
			l1 = lp1->len;
			l2 = lp2->len;
		} else {											// token $nFieldIndex
			if (nFieldIndex >= v1.stl_numberOfTokens) {		// check whether $nFieldIndex exists 
				if (nFieldIndex < v2.stl_numberOfTokens)
					return bDescending ? 1 : -1;
				else
					continue;
			} else if (nFieldIndex >= v2.stl_numberOfTokens)
		   		return bDescending ? -1 : 1;
			s1 = v1.stl_start[nFieldIndex];
			s2 = v2.stl_start[nFieldIndex];
			l1 = v1.stl_length[nFieldIndex];
			l2 = v2.stl_length[nFieldIndex];
		}

		// add opt. offset to begin of key field
		if ((nOffset = kp->k_offset) != 0) {
			if (nOffset > l1) {
				s1 += l1;
				l1 = 0;
			} else {
				s1 += nOffset;
				l1 -= nOffset;
			}
			if (nOffset > l2) {
				s2 += l2;
				l2 = 0;
			} else {
				s2 += nOffset;
				l2 -= nOffset;
			}
		}
		if ((nOffset = kp->k_width) != 0) {
			if (nOffset > l1) {
				l1 = 0;
			} else {
				l1 -= nOffset;
			}
			if (nOffset > l2) {
				l2 = 0;
			}
			else {
				l2 -= nOffset;
			}
		}

	/* skip 2 first key line of each rec */
		l1 = sort_compareExtractKeyfield(_linebuf, s1, l1, kp->k_flags, cQuoteChar);
		s1 = _linebuf;
		l2 = sort_compareExtractKeyfield(_linebuf + MAX_KEY_SIZE, s2, l2, kp->k_flags, cQuoteChar);
		s2 = _linebuf + MAX_KEY_SIZE;
		_currentKey = kp;
		if ((ret = (*kp->k_compare)(s1,s2)) != 0)
			return bDescending ? -ret : ret;

		if (kp->k_flags & K_UNIQ)
			for (nOffset = 0, lp=rp2->lp; nOffset < rp2->nl; nOffset++, lp=lp->next)
				lp->lflg |= LNXMARKED;
	}
	_currentKey = 0;
	return 0;
}

/*--------------------------------------------------------------------------
 * sort_quickSortList()
 * sort the "list of lines" from the file.
 * If sorting was aborted (via progress bar), return 0 to not sort the file 
 * contents.
 */
static int sort_quickSortList(RECORD* pRecords, long n) {
	qsort(pRecords, n, sizeof(RECORD), sort_compareRecords);
	return !_cancelled;
}

/*--------------------------------------------------------------------------
 * sort_createRecordsFromLines()
 * build a EdMacroRecord pElements, using a line list
 */
static int sort_createRecordsFromLines(LINE *lpfirst, LINE *lplast, 
			   RE_PATTERN *pPattern, int sortflags,
			   RECORD **pRectab)
{	int 	nrec,nl;
	LINE *lpnext;
	RE_MATCH match;
	RECORD* rectab;
	int nCapacity = 5000;

	rectab = malloc(nCapacity * sizeof * rectab);
	if (!rectab) {
		return 0;
	}
	*pRectab = rectab;
	memset(&match, 0, sizeof match);
	for (nrec = 0;;) {
		if ((sortflags & SO_NOSELECT) || 
			!pPattern ||
			regex_match(pPattern, lpfirst->lbuf,&lpfirst->lbuf[lpfirst->len], &match)) {
			rectab[nrec].lp = lpfirst;
			nl = 1;
			if (sortflags & SO_CLUSTERLINES) {
				for (;;) {
					if (lpfirst == lplast)
						break;
					lpnext = lpfirst->next;
					if (regex_match(pPattern, lpnext->lbuf,&lpnext->lbuf[lpnext->len], &match))
						break;
					lpfirst = lpnext;
					nl++;
				}
			}
			// This is a simplification - ideally we mark only lines really changing their order
			rectab[nrec].flags = lpfirst->lflg|LNMODIFIED;
			rectab[nrec++].nl = nl;
			if (nrec >= nCapacity) {
				nCapacity = 2 * nCapacity;
				*pRectab = realloc(rectab, nCapacity * sizeof * rectab);
				if (!*pRectab) {
					free(rectab);
					return 0;
				}
				rectab = *pRectab;
			}
		}
		if (lpfirst == lplast)
			return nrec;
		lpfirst = lpfirst->next;
	}
	return 1;
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
		fp->firstl->lflg = rectab[0].flags;
		lpd        = 0;
		goto advance;
	} else lpd = rp->lpfirst;
	for (; i < n; i++) {
		lpd->next = rectab[i].lp;
		lpd->next->lflg = rectab[i].flags;
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
}

/*
 * Allocate the undo data for undoing a sort operation.
 */
int sort_allocateUndoStructure(FTABLE* fp, LINE* lpfirst, LINE* lplast, void** p1, void** p2) {
	RECORD* rec, * rp;
	RECPARAMS* recpar;
	LINE* lp;
	long   	nrec, size;

	size = ln_cnt(lpfirst, lplast);
	nrec = size;
	size *= sizeof(RECORD);
	*p1 = 0;
	*p2 = 0;
	if ((rec = malloc(size)) == 0) {
		return 0;
	}

	for (lp = lpfirst, rp = rec; ; lp = lp->next, rp++) {
		rp->lp = lp;
		rp->nl = 1;
		rp->flags = lp->lflg;
		if (lp == lplast) {
			break;
		}
	}

	if ((recpar = malloc(sizeof(RECPARAMS))) == 0) {
		free(rec);
		return 0;
	}
	recpar->lpfirst = lpfirst->prev;
	recpar->lplast = lplast->next;
	recpar->nrec = nrec;
	*p1 = rec;
	*p2 = recpar;
	return 1;

}

/*--------------------------------------------------------------------------
 * sort_saveForUndo()
 */
int sort_saveForUndo(FTABLE *fp, LINE *lpfirst, LINE *lplast)
{	RECORD 	*rec;
	RECPARAMS *recpar;

	if (!sort_allocateUndoStructure(fp, lpfirst, lplast, &rec, &recpar)) {
		return 0;
	}
	undo_saveOperation(fp,(LINE*)rec, (LINE*)recpar,O_LNORDER);
	return 1;
}

/*--------------------------------------------------------------------------
 * ft_sortFile()
 * Sort the current file / document.
 */
int ft_sortFile(FTABLE* fp, int scope, char *fs, char *pszKeySpecification, char *sel, int sortflags)
{
	int  	ret = 0;
	int		n;
	char	fs_set[256];
	RECPARAMS rp;
	long  	l1;
	LINE 	*lpfirst, *lplast;
	MARK		*mps,*mpe;
	RE_PATTERN* pattern = NULL;
	WINFO* wp = WIPOI(fp);
	RECORD* rectab;

	if (!sel[0]) {
		sortflags |= SO_NOSELECT;
		sortflags &= ~SO_CLUSTERLINES;
	} else {
		if ((pattern = regex_compileWithDefault(sel)) == NULL) {
			return 0;
		}
	}
	_cancelled = 0;
	undo_startModification(fp);
	if (scope == RNG_BLOCK) {
		// donot include empty trailing lines in selection
		scope = RNG_BLOCK_LINES;
	}
	if (find_setTextSelection(wp, scope, &mps,&mpe) == RNG_INVALID) {
		return 0;
	}
	lpfirst = mps->m_linePointer;
	lplast =  mpe->m_linePointer;
	bl_hideSelection(wp, 0);

	sort_initializeKeyList(pszKeySpecification);
	sort_initializeFieldSeparators(fs_set, fs);
	if (!*fs_set) {
		_keytab.kt_tokenizeForComparison = 0;
	}
	_keytab.kt_fieldSeparators = fs_set;
	_sortflags = sortflags;
	l1 = wp->caret.ln;

	sort_saveForUndo(fp, lpfirst, lplast);
	if ((n = sort_createRecordsFromLines(lpfirst,lplast,pattern,_sortflags,&rectab)) != 0) {
		rp.lpfirst = lpfirst->prev;
		rp.lplast = lplast ->next;
		rp.nrec = n;
		progress_startMonitor(IDS_ABRTSORT, 1000);
		if (sort_quickSortList(rectab,n)) {
			caret_placeCursorInCurrentFile(wp,0L,0L);
			ln_order(fp,rectab,&rp);
		}
		progress_closeMonitor(0);
		render_repaintAllForFile(fp);
		caret_placeCursorInCurrentFile(wp,l1,0L);
		ret = 1;
	}

	free(rectab);

	return ret;
}
