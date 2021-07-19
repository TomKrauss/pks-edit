/*
 * CrossReferenceLinks.c
 *
 * maintain tags and do "under cursor actions"
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom 
 * created: 28. Dec. 1990
 */

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "winterf.h"
#include "edctype.h"
#include "fileselector.h"
#include "caretmovement.h"
#include "pksedit.h"
#include "editorconfiguration.h"
#include "edierror.h"
#include "errordialogs.h"
#include "iccall.h"
#include "xdialog.h"
#include "dial2.h"
#include "grammar.h"
#include "findandreplace.h"
#include "linkedlist.h"
#include "stringutil.h"
#include "winfo.h"
#include "fileutil.h"
#include "winutil.h"
#include "crossreferencelinks.h"
#include "markpositions.h"
#include "textblocks.h"

#define	TCMD_EXEC				'!'
#define	TCMD_TAGFILE			'^'
#define	TCMD_WINHELP			'&'

typedef struct tag {
	long ln;
	int  cmd;
	char fn[256];
	char rembuf[100];
} TAG;

/*
 * description of the "try" list for tag retrieval
 */

#define	TAGMAXTRY			5

typedef struct tagTAGTRYLIST {
	int		type;
	char	*fn;
} TAGTRYLIST;

typedef struct tagtry {
	struct tagtry *next;
	char		name[32];
	int			max;
	int  		curr;
	TAGTRYLIST 	t[TAGMAXTRY];
} TAGTRY;

static TAGTRY	*_ttry;

static FTABLE	_tagfile,_stepfile;
extern FTABLE 	*ft_getCurrentErrorDocument();

static NAVIGATION_PATTERN __ge = { 
0, "","\"([^\"]+)\", line ([0-9]+): *(.*)",	1,	2,	3 };
static NAVIGATION_PATTERN __te = { 
0, "", "^([^ ]+) ([^ ]+) ([!?^&].*)",   1,	2,	3 };
static NAVIGATION_PATTERN *_exprerror = &__ge;
static NAVIGATION_PATTERN *_searchListNavigationPattern = &__ge;
static NAVIGATION_PATTERN *_tagerror  = &__te;
static NAVIGATION_PATTERN	*_compilerOutputNavigationPatterns;
static int	_compflag;

extern int help_showHelpForKey(LPSTR szFile, LPSTR szKey);

/*---------------------------------*/
/* tags_mk()					*/
/*---------------------------------*/
static char *szTags = "tags";
static intptr_t tags_mk(char *tag, LONG unused)
{
	char	*s;
	char* files;
	TAGTRY	*tp;

	if ((tp = prof_llinsert(&_ttry,sizeof *tp,szTags,tag,&files)) == 0)
		return 0;

	s = strtok(files,",");
	for (tp->max = 0; tp->max < TAGMAXTRY && s; tp->max++) {
		tp->t[tp->max].type = *s++;

		/* is already malloced , dont free old ! */
		tp->t[tp->max].fn = _strdup(s);
		s = strtok((char *)0,",");
	}
	tp->curr = -1;
	free(files);
	return 1;
}

/*
 * Destroy one tagtry element. 
 */
static int xref_destroy(TAGTRY* pTry) {
	for (int i = 0; i < DIM(pTry->t); i++) {
		if (pTry->t[i].fn) {
			free(pTry->t[i].fn);
			pTry->t[i].fn = NULL;
		}
	}
	return 1;
}

static int xref_destroyCmpTag(NAVIGATION_PATTERN* ct) {
	free(ct->pattern);
	return 1;
}

/*
 * Free all memory occupied by the cross reference lists. 
 */
void xref_destroyAllCrossReferenceLists() {
	ll_destroy((LINKED_LIST**)&_ttry, xref_destroy);
	ll_destroy((LINKED_LIST**)&_compilerOutputNavigationPatterns, xref_destroyCmpTag);
}

/*--------------------------------------------------------------------------
 * xref_initFileFormats()
 * init all compiler types
 */
int xref_initFileFormats(void)
{
	return prof_enum(szTags,tags_mk,0L);
}

/*--------------------------------------------------------------------------
 * xref_selectFileFormat()
 * activate a new source list for tag searches (e.g. when
 * changing document type of top level window)
 */
void xref_selectFileFormat(char *tags)
{
	void *p;

	if (!_ttry) {
		return;
	}

	if ((p = ll_find((LINKED_LIST*)_ttry, tags)) != 0 && p != _ttry) {
		/* force tag file reread */
		((TAGTRY *)p)->curr = -1;
		ll_moveElementToFront((LINKED_LIST**)&_ttry, p);
	}
}

/*------------------------------------------------------------------------
 * get_quoted()
 */
static char *get_quoted(char **src)
{
	char *d,*s,*start;

	s = d = start = *src;
	while (*s) {
		if (*s == ',') {
			s++;
			break;
		}
		if (*s == '\\' && s[1])
			s++;
		*d++ = *s++;
	}
	*d = 0;
	*src = s;
	return start;
}

/*--------------------------------------------------------------------------
 * compiler_mk()
 */
static char *szCompiler = "compiler";
static intptr_t compiler_mk(char *compiler, LONG unused)
{
	NAVIGATION_PATTERN 	*ct;
	char		*s, *pszLine;
	char* szNext;

	if ((ct = prof_llinsert(&_compilerOutputNavigationPatterns,sizeof *ct,szCompiler,compiler,&pszLine)) == 0) {
		return 0;
	}
	s = pszLine;
	ct->pattern = _strdup(get_quoted(&s));
	szNext = get_quoted(&s);
	ct->filenameCapture = szNext[0]-'0';
	szNext = get_quoted(&s);
	ct->lineNumberCapture = szNext[0] -'0';
	szNext = get_quoted(&s);
	ct->commentCapture= szNext[0] - '0';
	free(pszLine);
	return 1;
}

/*--------------------------------------------------------------------------
 * xref_restoreFromConfigFile()
 * init all compiler types
 */
int xref_restoreFromConfigFile(void)
{
	return prof_enum(szCompiler,compiler_mk,0L);
}

/*---------------------------------*/
/* tagexprinit()				*/
/*---------------------------------*/
static RE_PATTERN *tagexprinit(char *ebuf,NAVIGATION_PATTERN *s)
{
	_exprerror = s;
	return find_regexCompile(ebuf,_exprerror->pattern,RE_DOREX);
}

/*---------------------------------*/
/* dostep()					*/
/*---------------------------------*/
static TAG *dostep(LINE *lp, RE_PATTERN *pattern)
{
	static TAG _tag;
	RE_MATCH match;

	if (pattern == NULL) {
		return 0;
	}
	if (regex_match(pattern, lp->lbuf, &lp->lbuf[lp->len], &match)) {
		find_initializeReplaceByExpression(_exprerror->pattern);	/* assure a few initialiations */
		regex_getCapturingGroup(&match, _exprerror->filenameCapture-1, _tag.fn, sizeof _tag.fn);
		regex_getCapturingGroup(&match, _exprerror->lineNumberCapture - 1, _linebuf, LINEBUFSIZE);
		_tag.ln = string_convertToLong(_linebuf);
		regex_getCapturingGroup(&match, _exprerror->commentCapture - 1, _tag.rembuf, sizeof _tag.rembuf);
		return &_tag;
	}
	return 0;
}

/*---------------------------------*/
/* xref_readTagFile()					*/
/*---------------------------------*/
static int xref_readTagFile(char* fn, FTABLE* fp)
{
	int ret;

	ln_listfree(fp->firstl);
	ret = ft_readfileWithOptions(fp, fn, -1);
	return ret;
}


/*---------------------------------*/
/* xref_loadTagFile()				*/
/*---------------------------------*/
static int xref_loadTagFile(FTABLE *fp, char *tagFilename)
{	char   *fn;

	if ((fn = file_searchFileInPKSEditLocation(tagFilename)) == 0L) {
		// error_showError("Kann nicht zu Querverweis navigieren. Verweise können nicht geladen werden.", NULL);
		return 0;
	}

	return xref_readTagFile(fn,fp);
}

/*---------------------------------*/
/* xref_isTagCommand()					*/
/*---------------------------------*/
static int xref_isTagCommand(unsigned char c)
{
	if (c == TCMD_TAGFILE ||	/* normal tag command */
	    c == TCMD_EXEC ||	/* execute ... */
	    c == '?' || 		/* unused */
	    c == TCMD_WINHELP	/* call windows help */
	   )	return 1;
	return 0;
}

/*---------------------------------*/
/* xref_processTagExpressions()					*/
/* handle tag optimizations:		*/
/* - * tag ^search				*/
/* - ? tag ^					*/
/*---------------------------------*/
static LINE *xref_processTagExpressions(LINE *lp, char wild, char *d, int max, int *cmd)
{	char *s,*dend,lim;

	while(lp->lbuf[0] == '?' || lp->lbuf[0] == wild) {
		if ((lp = lp->prev) == 0) {
			error_showErrorById(IDS_MSGWRONGTAGFORMAT);
			return 0;
		}
	}
	s    = lp->lbuf;

	if (wild == '?') {
		while(*s && !xref_isTagCommand(*s))
			s++;
		lim  = 0;
		*cmd = *s;
	} else {
		lim = ' ';
	}

	dend = &d[max-1];
	while(*s && *s != lim && d < dend) {
		*d++ = *s++;
	}
	*d = 0;
	return lp;
}

/*--------------------------------------------------------------------------
 * taggetinfo()
 */
static TAG *taggetinfo(LINE *lp)
{
	TAG	*	tp;
	char     	ebuf[ESIZE];
	RE_PATTERN *pPattern = tagexprinit(ebuf, _tagerror);

	if (pPattern == NULL || (tp = dostep(lp, pPattern)) == 0L) {
		/* bad format in tag file */
		return 0;
	}
	if ((lp = xref_processTagExpressions(lp, '?', tp->rembuf,
		sizeof(tp->rembuf), &tp->cmd)) == 0) {
		return 0;
	}
	if (xref_processTagExpressions(lp, '*', tp->fn, sizeof(tp->fn), &tp->cmd) == 0) {
		return 0;
	}
	return tp;
}

/*------------------------------------------------------------
 * taglist_measureitem()
 */
#define		TAGLISTITEMHEIGHT			40
static void taglist_measureitem(MEASUREITEMSTRUCT *mp)
{
	mp->itemHeight = TAGLISTITEMHEIGHT;
	mp->itemWidth = 600;
}

/*------------------------------------------------------------
 * taglist_cmpitem()
 */
static int taglist_cmpitem(COMPAREITEMSTRUCT *cp)
{
	int 	nRet;
	int	nSize;

	nSize = min(((DLGSTRINGLIST *)cp->itemData1)->nSize,
		((DLGSTRINGLIST *)cp->itemData2)->nSize);

	nRet = strncmp(
		((DLGSTRINGLIST *)cp->itemData1)->pszString,
		((DLGSTRINGLIST *)cp->itemData2)->pszString,
		nSize);
	
	if (nRet != 0) {
		return nRet;
	}

	return ((DLGSTRINGLIST *)cp->itemData1)->nSize -
		((DLGSTRINGLIST *)cp->itemData2)->nSize;
}

/*------------------------------------------------------------
 * taglist_drawitem()
 */
static void taglist_drawitem(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl)
{
	LONG			lExtent;
	WORD			wDelta;
	WORD			hDelta;
	DLGSTRINGLIST *dlp;
	LINE *		lp;
	TAG *		tp;
	HICON		hIcon;

	if ((dlp = (DLGSTRINGLIST*)par) == 0) {
		return;
	}

	lp = (LINE *) dlp->pAny;

	lExtent = win_getTextExtent(hdc, dlp->pszString, dlp->nSize);
	wDelta = LOWORD(lExtent) + 20;
	hDelta = HIWORD(lExtent);
	hDelta = (TAGLISTITEMHEIGHT - hDelta) / 2;

	TextOut(hdc, rcp->left + 50, rcp->top + hDelta, dlp->pszString, dlp->nSize);

	if (wDelta < 150) {
		wDelta = 150;
	}
	if ((tp = taggetinfo(lp)) != 0) {

		switch(tp->cmd) {

		case TCMD_EXEC:	
			hIcon = LoadIcon(hInst, "Exec");
			break;
		case TCMD_TAGFILE:	
			hIcon = LoadIcon (hInst, "File");
			break;
		default:			
			hIcon = LoadIcon(NULL, IDI_QUESTION);
			break;
		}
	
		DrawIcon(hdc, 
			rcp->left + (50 - 32) / 2, 
			rcp->top + (rcp->bottom - rcp->top - 32) / 2,
			hIcon);
		TextOut(hdc, rcp->left + 50 + wDelta, rcp->top + hDelta, tp->fn, 
			lstrlen(tp->fn));
		DestroyIcon(hIcon);
	}

}

/*---------------------------------*/
/* findtag()					*/
/*---------------------------------*/
static BOOL _tagCancelled;
static TAG *findtag(char *s, FTABLE *fp)
{
	LINE *			lp;
	LPSTR			pszBlank;
	char     			sbuf[ESIZE];
	char     			ebuf[ESIZE];
	RE_MATCH		match;
	RE_PATTERN*		pPattern;
	DLGSTRINGLIST *	dlp;
	DLGSTRINGLIST *	dlpCurr;

	dlp = 0;
	wsprintf(sbuf,"^[^ ]+ <%s> [!^?&]", (LPSTR)s);
	if (!(pPattern = find_regexCompile(ebuf, sbuf, (int)RE_DOREX))) {
		return 0;
	}

	for (lp = fp->firstl; lp; lp = lp->next) {
		if (regex_match(pPattern, lp->lbuf, &lp->lbuf[lp->len], &match)) {
			if ((dlpCurr = (DLGSTRINGLIST *)ll_insert((LINKED_LIST**)&dlp, sizeof *dlp)) == 0) {
				ll_destroy((LINKED_LIST**)&dlp, (int (*)(void *))0);
				return 0;
			}
			if ((pszBlank = lstrchr(lp->lbuf, ' ')) != 0) {
				dlp->pszString = ++pszBlank;
				if ((pszBlank = lstrchr(pszBlank, ' ')) == 0) {
					pszBlank = &lp->lbuf[lp->len];
				}
				dlp->nSize = (int)(pszBlank - dlp->pszString);
			} else {
				dlp->pszString = lp->lbuf;
				dlp->nSize = lp->len;
			}
			dlpCurr->pAny = lp;
		}
	}

	switch (ll_size((LINKED_LIST*)dlp)) {

	case 0: 
		return 0;

	case 1:
		dlpCurr = dlp;
		break;

	default:
	{
		static DIALLIST dlist = {
			0,
			0, 
			0, 
			taglist_measureitem, 
			taglist_drawitem, 
			0,
			taglist_cmpitem,
			0
		};
		dlpCurr = DoDlgSelectFromList(DLGTAGLIST, dlp, &dlist);
		break;
	}

	}

	if (!dlpCurr) {
		ll_destroy((LINKED_LIST**)&dlp, (int (*)(void *))0);
		_tagCancelled = TRUE;
		return 0;
	}

	lp = dlpCurr->pAny;
	ll_destroy((LINKED_LIST**)&dlp, (int (*)(void *))0);
	return taggetinfo(lp);
}

/*---------------------------------*/
/* gettag()					*/
/*---------------------------------*/
static char *gettag(unsigned char *d,unsigned char *dend,int (*valid)(),int scan2beg) {	
	char *s,*S;
	char *s1=d;
	WINFO *wp;

	if ((wp = ww_getCurrentEditorWindow()) == 0L) 
		return (char *)0;
	S = wp->caret.linePointer->lbuf;
	s = &S[wp->caret.offset];

	if (scan2beg)
		while(s > S && (*valid)(s[-1]))
			s--;

	while ((*valid)(*s) && d < dend)
		*d++ = *s++; 

	*d = 0;
	return(s1);
}

/*---------------------------------*/
/* xref_saveCrossReferenceWord()					*/
/*---------------------------------*/
static char *_tagword;
static char *xref_saveCrossReferenceWord(unsigned char *d,unsigned char *dend)
{
	if (_tagword) {
		strcpy(d,_tagword);
		return _tagword;
	}

	return gettag(d,dend,char_isIdentifier,1);
}

/*------------------*/
/* xref_openFile()	*/
/*------------------*/
int xref_openFile(char *name, long line, WINDOWPLACEMENT *wsp) {	
	int ret = 0;

	if (ft_activateWindowOfFileNamed(name)) {
		if (line >= 0)
			ret = caret_placeCursorMakeVisibleAndSaveLocation(ww_getCurrentEditorWindow(), line,0L);
		else ret = 1;
	} else {
		ret = ft_openFileWithoutFileselector(name,line,wsp);
	}

	return ret;
}

/*---------------------------------*
 * xref_navigateCrossReference()
 * Navigate to the cross reference word given as an argument - no
 * selection of multiple matches yet.
 *---------------------------------*/
int xref_navigateCrossReference(char *s)
{
	TAG *		tp;
	char     		ebuf[ESIZE];
	char	    		fnam[256];
	int			tnum;
	int			ret = 0;
	TAGTRY *		ttp;
	TAGTRYLIST *	ttl;
	FTABLE *		fp;

	if (!s || !*s)
		return 0;

	if ((ttp = _ttry) == 0)
		return 0;

	fp  = &_tagfile;

	find_setCurrentSearchExpression(s);
	_tagword = s;

	_tagCancelled = FALSE;

	for (tnum = 0; tnum < ttp->max && ret == 0 && _tagCancelled == FALSE; tnum++) {
		ttl = &ttp->t[tnum];
		switch(ttl->type) {
			case TCMD_TAGFILE:
				if (tnum != ttp->curr) {
					if (xref_loadTagFile(fp,ttl->fn) == 0)
						break;
					ttp->curr = tnum;
				}
				if ((tp = findtag(s,fp)) != 0L) {
					if (tp->cmd == TCMD_WINHELP) {
						ret = help_showHelpForKey(tp->fn, tp->rembuf+1);
					} else {
						strcpy(fnam,tp->fn);
						fm_savepos(s);
						xref_openFile(fnam,-1L,(WINDOWPLACEMENT*)0);
						if (ft_getCurrentDocument()) {
							RE_PATTERN* pPattern;
							if (pPattern = find_regexCompile(ebuf, tp->rembuf, (int) RE_DOREX)) {
								find_expressionInCurrentFile(1,pPattern,O_WRAPSCAN);
								ret = 1;
							}
						}
					}
				}
				break;
			case TCMD_WINHELP:
				ret = help_showHelpForKey(ttl->fn, s);
				break;
			}
	}

	_tagword = 0;
	if (!ret && !_tagCancelled) {
		error_showErrorById(IDS_MSGUNKNWONTAG,(LPSTR)s);
	}
	return ret;
}

/*---------------------------------
 * xref_openSearchListResultFromLine()
 * Parse the search list result in the current line and try to navigate to
 * the file and line number which are obtained by parsing the line contents.
 *---------------------------------*/
void xref_openSearchListResultFromLine(LINE *lp)
{ 	register TAG  *tp;
	WINDOWPLACEMENT ws,*wsp = 0;
	char ebuf[ESIZE];
	RE_PATTERN *pPattern = tagexprinit(ebuf,_searchListNavigationPattern);

	while((lp = lp->next) != 0L) {		/* we may skip 1st line ! */
		if (lp->len && (tp = dostep(lp,pPattern)) != 0L) {
			if (tp->rembuf[0]) {
				/* this means -> windowstate given */
				wsp = &ws;
				prof_getWindowSettings(tp->rembuf,wsp);
			}
			xref_openFile(tp->fn, tp->ln-1L, wsp);
		}
	}
}

/*
 * Highlight a match in a file opened as a result of a navigation. 
 */
static xref_highlightMatch(long ln, int col, int len) {
	WINFO* wpFound = ww_getCurrentEditorWindow();
	caret_placeCursorInCurrentFile(wpFound, ln, col);
	bl_hideSelection(wpFound, 1);
	CARET caret = wpFound->caret;
	bl_setSelection(wpFound, caret.linePointer, caret.offset, caret.linePointer, len + caret.offset);
	render_repaintCurrentLine(wpFound);
}

/*---------------------------------*/
/* EdErrorNext()				*/
/*---------------------------------*/
int EdErrorNext(int dir) {
	register LINE 	*lp;
 	register TAG  	*tp=0;
	FTABLE	    	*fp;
	long			lineno = 0;
	NAVIGATION_PATTERN *steperror;
	char 			ebuf[ESIZE];
	char			fullname[256];
	WINFO			*wp;

	if ((dir & LIST_USETOPWINDOW) || !ft_getCurrentErrorDocument()) {
	/* treat current window as error list */
		_compflag = 1;
		WINFO* wp = ww_getWindowFromStack(0);
		ft_setCurrentErrorDocument(wp ? wp->fp : NULL);
		if (wp) {
			lineno = wp->caret.ln;
		}
	}

	if ((fp = ft_getCurrentErrorDocument()) == NULL || (lp = fp->lpReadPointer) == 0L) {
notfile:	error_showErrorById(IDS_MSGNOTAGFILE);
		return 0;
	}
	if (_compflag) {
		steperror = _compilerOutputNavigationPatterns;
	} else {
		steperror = _searchListNavigationPattern;
	}
	if (steperror == 0) {
		goto notfile;
	}

	RE_PATTERN *pPattern = tagexprinit(ebuf,steperror);

	switch (dir) {
	case LIST_PREV:
dobackward:
		while((lineno--, (lp = lp->prev) != 0L) && 
		      (tp = dostep(lp, pPattern)) == 0L);
		break;
	case LIST_NEXT:
doforward:
		while((lineno++, (lp = lp->next) != 0L) &&
		      (tp = dostep(lp, pPattern)) == 0L);
		break;
	case LIST_START:
		lp     = fp->firstl;
		lineno = 0;
		if ((tp = dostep(lp, pPattern)) == 0L) goto doforward;
		break;
	case LIST_END:
		lp     = fp->lastl;
		lineno = fp->nlines;
		goto dobackward;
	default:
		if ((tp = dostep(lp, pPattern)) == 0L)
			goto doforward;
		break;
	}
	if (tp && lp) {
		if ((wp = WIPOI(fp)) != 0) {
			/* EdSelectWindow(wp->win_id); */
			caret_placeCursorForFile(wp,lineno,0);
			ln_removeFlag(fp->firstl,fp->lastl,LNXMARKED);
			lp->lflg |= LNXMARKED;
			render_repaintAllForFile(fp);
		} else {
			fp->lpReadPointer = lp;
		}

	/* make file name relativ to list file */
		if (*tp->fn == '/' || tp->fn[1] == ':') {
			lstrcpy(fullname, tp->fn);
		} else {
			string_splitFilename(fp->fname, fullname, (char *)0);
			string_concatPathAndFilename(fullname, fullname, tp->fn);
		}
		if (xref_openFile(fullname, tp->ln-1L, (WINDOWPLACEMENT*)0)) {
			if (tp->rembuf[0]) {
				int col;
				int len;
				if (sscanf(tp->rembuf, "%d/%d", &col, &len) != EOF && len > 0) {
					xref_highlightMatch(tp->ln-1L, col, len);
				}
			}
			return 1;
		}
		return 0;
	}
	error_showErrorById(_compflag ? IDS_MSGNOMOREERRS : 
					 IDS_MSGSTRINGNOTFOUND);
	return 0;
}

/*---------------------------------*/
/* xref_openTagFileOrSearchResults()					*/
/*---------------------------------*/
#define	ST_TAGS		1
#define	ST_ERRORS	2
#define	ST_STEP		3
static FSELINFO _tagfselinfo = { ".", "tags", "*.tag" };
static FSELINFO _grpfselinfo = { ".", "pksedit.grep", "*.grep" };
static FSELINFO _cmpfselinfo = { ".", "errors.err", "*.err" };
static int xref_openTagFileOrSearchResults(int title, int st_type, FSELINFO *fsp)
{
	if (title && fsel_selectFileWithOptions(fsp, title, FALSE) == 0) {
		return 0;
	}

	switch(st_type) {
		case ST_ERRORS:
			if (xref_openFile(_fseltarget, 0L, (WINDOWPLACEMENT*)0) && ft_getCurrentDocument()) {
				EdFileAbandon();
			}
			return EdErrorNext(LIST_START|LIST_USETOPWINDOW);
		case ST_STEP:
			if (xref_readTagFile(_fseltarget,&_stepfile)) {
				_compflag = 0;
				ft_setCurrentErrorDocument(&_stepfile);
				return EdErrorNext(LIST_START);
			}
			return 0;
		case ST_TAGS:
		/*
		 * new tag files are added in front of the tag list
		 * cause all icon tags should stay at the end of the search list
		 */
		if (_ttry == 0)
			break;
		if (_ttry->max < TAGMAXTRY) {
			memmove(&_ttry->t[1],_ttry->t,sizeof(*_ttry->t) * _ttry->max);
			_ttry->max++;
		}
		_ttry->t[0].type = TCMD_TAGFILE;
		_ttry->t[0].fn = _strdup(_fseltarget);
		_ttry->curr = 0;
		return xref_readTagFile(_fseltarget,&_tagfile);
	}
	return 0;
}

typedef struct tagNAVIGATION_INFO_PARSE_RESULT {
	char* ni_fileName;
	long  ni_lineNumber;
	WINDOWPLACEMENT* ni_wp;
} NAVIGATION_INFO_PARSE_RESULT;

static int xref_determineNavigationInfo(WINFO* wp, NAVIGATION_INFO_PARSE_RESULT* pResult, char* szFileBuffer, size_t nFileBufferSize) {
	LINE* lp = wp->caret.linePointer;
	FTABLE* fp = wp->fp;
	RE_PATTERN pattern;
	RE_OPTIONS options;
	RE_MATCH match;
	unsigned char patternBuf[256];

	NAVIGATION_PATTERN* pPattern = grammar_getNavigationPatterns(fp->documentDescriptor->grammar);
	while (pPattern) {
		memset(&options, 0, sizeof options);
		options.flags = RE_DOREX;
		options.patternBuf = patternBuf;
		options.endOfPatternBuf = &patternBuf[sizeof patternBuf];
		options.expression = pPattern->pattern;
		if (regex_compile(&options, &pattern)) {
			char* pszStart = lp->lbuf;
			char* pszCursor = &lp->lbuf[wp->caret.col];
			char* pszEnd = &lp->lbuf[lp->len];
			while (regex_match(&pattern, pszStart, pszEnd, &match)) {
				if (match.loc1 <= pszCursor && match.loc2 >= pszCursor && 
						regex_getCapturingGroup(&match, pPattern->filenameCapture - 1, szFileBuffer, (int)nFileBufferSize) == SUCCESS) {
					pResult->ni_fileName = szFileBuffer;
					pResult->ni_lineNumber = 0;			// for now - to be initialized from capture group
					return 1;
				}
				pszStart = match.loc2;
			}
		}
		pPattern = pPattern->next;
	}
	return 0;
}

/*--------------------------------*/
/* EdFindFileCursor() */
/*---------------------------------*/
int EdFindFileCursor(void)
{	char	*found;
	char	fselpath[512];
	char	filename[128];
	NAVIGATION_INFO_PARSE_RESULT result;
	WINFO* wp = ww_getCurrentEditorWindow();
	extern char *file_searchFileInPath();

	if (wp == 0) {
		return 0;
	}
	if (!xref_determineNavigationInfo(wp, &result, _fseltarget, EDMAXPATHLEN)) {
		return 0;
	}
	string_splitFilename(_fseltarget,fselpath,filename);
	if ((found = file_searchFileInPath(filename,GetConfiguration()->includePath))   != 0 ||
	    (found = file_searchFileInPath(_fseltarget,fselpath)) != 0) {
		return xref_openFile(found, result.ni_lineNumber, (WINDOWPLACEMENT*)0);
	}
	ShellExecute(hwndMDIFrameWindow, "open", _fseltarget, "", ".", SW_SHOWNORMAL);
	return 1;
}

/*---------------------------------*/
/* EdFindTagCursor()			*/
/*---------------------------------*/
int EdFindTagCursor(void)
{
	if (EdFindFileCursor()) {
		return 1;
	}
	return xref_navigateCrossReference(xref_saveCrossReferenceWord(_linebuf, &_linebuf[LINEBUFSIZE]));
}

/*---------------------------------*/
/* EdFindWordCursor() 			*/
/*---------------------------------*/
int EdFindWordCursor(dir)
{	char *s,buf[256];

	if ((s = gettag(buf,&buf[sizeof buf],char_isIdentifier,0)) != 0L) {
		RE_PATTERN *pPattern = regex_compileWithDefault(s);
		return pPattern && find_expressionInCurrentFile(dir, pPattern, _currentSearchAndReplaceParams.options);
	}
	return 0;
}

/*---------------------------------*/
/* xref_openSearchList()				*/
/*---------------------------------*/
void xref_openSearchList(char *fn, int cmpflg)
{
	strcpy(_fseltarget,fn);
	xref_openTagFileOrSearchResults(0, cmpflg ? ST_ERRORS: ST_STEP, &_cmpfselinfo);
}

/*---------------------------------*/
/* EdSearchListRead() 			*/
/*---------------------------------*/
int EdSearchListRead(void)
{
	return xref_openTagFileOrSearchResults(MREAD_SEARCH_LIST,ST_STEP,&_grpfselinfo);
}

/*---------------------------------*/
/* EdTagfileRead()				*/
/*---------------------------------*/
int EdTagfileRead(void)
{
	return xref_openTagFileOrSearchResults(MREADTAG,ST_TAGS,&_tagfselinfo);
}

/*---------------------------------*/
/* EdErrorListRead()			*/
/*---------------------------------*/
int EdErrorListRead(long dummy1, long dummy2) {
	return xref_openTagFileOrSearchResults(MREADCMP,ST_ERRORS,&_cmpfselinfo);
}

