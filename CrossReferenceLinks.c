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
#include "alloc.h"
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

typedef enum TAG_KIND {
	TK_FUNCTION = 'f', TK_MEMBER = 'm', TK_VARIABLE = 'v', TK_STRUCT = 's', TK_NUMBER_VALUE = 'n', TK_BOOLEAN_VALUE = 'b', TK_STRING_VALUE = 's', TK_OTHER
} TAG_KIND;

/*
 * Describes a xross-reference navigation request. 
 */
typedef struct tag {
	char tagname[128];					// The name of the tag (method, type, ...) itself.
	char filename[EDMAXPATHLEN];		// The file to open
	BOOL isDefinition;					// Whether the reference describes the definition rather than the reference to a language artifact.
	BOOL isPrivate;						// Whether the described element is private or static (C)
	long ln;							// The line number to jump to - may be -1
	char searchCommand[100];			// A regular expression to search
	TAG_KIND kind;						// The type of the element described.
	char referenceDescription[100];		// A description where the described element can be found (e.g. member of struct xxx)
} TAG;

#define	TAGMAXTRY			5

static FTABLE	_tagfile,_stepfile;
extern FTABLE 	*ft_getCurrentErrorDocument();

static NAVIGATION_PATTERN __ge = { 
0, "","\"([^\"]+)\", line ([0-9]+): *(.*)",	1,	2,	3 };
static NAVIGATION_PATTERN _universalCTagsFileFormat =  { 
0, "", "^([^\t]+)\t([^\t]+)\t(/.*/);\"\\s(.*)", 2, 3, 3, 1, 4 };
static NAVIGATION_PATTERN *_exprerror = &__ge;
static NAVIGATION_PATTERN *_searchListNavigationPattern = &__ge;
static NAVIGATION_PATTERN *_tagfileFormatPattern  = &_universalCTagsFileFormat;
static NAVIGATION_PATTERN	*_compilerOutputNavigationPatterns;
static int	_compflag;

extern int help_showHelpForKey(LPSTR szFile, LPSTR szKey);

static int xref_destroyCmpTag(NAVIGATION_PATTERN* ct) {
	free(ct->pattern);
	return 1;
}

/*
 * Free all memory occupied by the cross reference lists. 
 */
void xref_destroyAllCrossReferenceLists() {
	ll_destroy((LINKED_LIST**)&_compilerOutputNavigationPatterns, xref_destroyCmpTag);
}

/*------------------------------------------------------------------------
 * get_quoted()
 */
static char *get_quoted(char **src) {
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
/* xref_initializeNavigationPattern()				*/
/*---------------------------------*/
static RE_PATTERN *xref_initializeNavigationPattern(char *ebuf,NAVIGATION_PATTERN *s)
{
	_exprerror = s;
	return find_regexCompile(ebuf,_exprerror->pattern,RE_DOREX);
}

/*
 * Parse a tag definition from the tag file. If successful, return the tag definition, if not, return 0.
 */
static TAG _tag;
static TAG *xref_parseTagDefinition(LINE *lp, RE_PATTERN *pattern) {
	RE_MATCH match;

	if (pattern == NULL) {
		return 0;
	}
	char extCommand[256];

	if (regex_match(pattern, lp->lbuf, &lp->lbuf[lp->len], &match)) {
		find_initializeReplaceByExpression(_exprerror->pattern);	/* assure a few initialiations */
		regex_getCapturingGroup(&match, _exprerror->filenameCapture-1, _tag.filename, sizeof _tag.filename);
		regex_getCapturingGroup(&match, _exprerror->commentCapture - 1, extCommand, sizeof extCommand);
		if (extCommand[0] == '?' || extCommand[0] == '/') {
			char* pszSource = extCommand+1;
			char* pszDest = _tag.searchCommand;
			while (*pszSource) {
				char c = *pszSource++;
				if (c == '/') {
					*pszDest = 0;
					break;
				}
				if (c == '\\') {
					c = *pszSource++;
				} else if (c == '*' || c == '+' || c == '?') {
					*pszDest++ = '\\';
				}
				*pszDest++ = c;
			}
			_tag.ln = -1;
		} else {
			_tag.ln = string_convertToLong(extCommand);
			_tag.searchCommand[0] = 0;
		}
		regex_getCapturingGroup(&match, _exprerror->tagCapture - 1, _tag.tagname, sizeof _tag.tagname);
		regex_getCapturingGroup(&match, _exprerror->tagExtensionFields - 1, extCommand, sizeof extCommand);
		_tag.kind = extCommand[0];
		if (extCommand[1] == '\t' && _tag.kind == TK_MEMBER) {
			strcpy(_tag.referenceDescription, strtok(extCommand + 2, "\t"));
		}
		_tag.isDefinition = TRUE;
		// TODO: handle reference type tags, etc...
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
	if (c == TST_TAGFILE ||	/* normal tag command */
	    c == TST_EXECECUTE_PROGRAM ||	/* execute ... */
	    c == '?' || 		/* unused */
	    c == TST_HELPFILE	/* call windows help */
	   )	return 1;
	return 0;
}

/*--------------------------------------------------------------------------
 * xref_parseTagDefinitionFromLine()
 */
static TAG *xref_parseTagDefinitionFromLine(LINE *lp) {
	TAG	*	tp;
	char     	ebuf[ESIZE];
	RE_PATTERN *pPattern = xref_initializeNavigationPattern(ebuf, _tagfileFormatPattern);

	if (pPattern == NULL || (tp = xref_parseTagDefinition(lp, pPattern)) == 0L) {
		/* bad format in tag file */
		return 0;
	}
	return tp;
}

/*------------------------------------------------------------
 * taglist_measureitem()
 */
#define		TAGLISTITEMHEIGHT			28
static void taglist_measureitem(MEASUREITEMSTRUCT *mp)
{
	mp->itemHeight = TAGLISTITEMHEIGHT;
	mp->itemWidth = 800;
}

/*------------------------------------------------------------
 * taglist_cmpitem()
 */
static int taglist_cmpitem(COMPAREITEMSTRUCT *cp)
{
	int nRet;
	int nSize;

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
	WORD			wDelta = 220;
	WORD			hDelta;
	int				nIconDelta = 62;
	int				nIconWidth = 16;
	DLGSTRINGLIST*	dlp;
	TAG *			tp;
	HICON			hIcon;

	if ((dlp = (DLGSTRINGLIST*)par) == 0) {
		return;
	}

	tp = (TAG *) dlp->pAny;
	lExtent = win_getTextExtent(hdc, dlp->pszString, dlp->nSize);
	hDelta = HIWORD(lExtent);
	hDelta = (TAGLISTITEMHEIGHT - hDelta) / 2;
	TextOut(hdc, rcp->left + nIconDelta, rcp->top + hDelta, tp->filename, (int)strlen(tp->filename));
	
	hIcon = LoadIcon (hInst, tp->isDefinition ? "NEXT" : "PREVIOUS");
	DrawIconEx(hdc,
		rcp->left + nIconWidth / 2,
		rcp->top + (rcp->bottom - rcp->top - nIconWidth) / 2,
		hIcon, nIconWidth, nIconWidth, 0, NULL, DI_NORMAL);
	char* pszComment = tp->referenceDescription;
	if (!*pszComment) {
		pszComment = tp->searchCommand;
	}
	TextOut(hdc, rcp->left + nIconDelta + wDelta, rcp->top + hDelta, pszComment, lstrlen(pszComment));
	DestroyIcon(hIcon);
}

/*
 * Release the list of dialog list tag references produced to have the user select
 * a tag.
 */
static int xref_deallocateTag(DLGSTRINGLIST* pStringItem) {
	free(pStringItem->pAny);
	return 1;
}

/*
 * Copy a tag definition.
 */
static TAG* xref_allocateTag(TAG* pTag) {
	TAG* pCopy = malloc(sizeof * pCopy);
	memcpy(pCopy, pTag, sizeof * pTag);
	return pCopy;
}

/*---------------------------------*/
/* findtag()					*/
/*---------------------------------*/
static BOOL _tagCancelled;
static TAG *findtag(char *s, FTABLE *fp) {
	LINE *			lp;
	TAG*			pTag;
	char     		sbuf[ESIZE];
	DLGSTRINGLIST *	dlp;
	DLGSTRINGLIST *	dlpCurr;

	dlp = 0;
	wsprintf(sbuf,"%s\t", (LPSTR)s);

	for (lp = fp->firstl; lp; lp = lp->next) {
		if (strstr(lp->lbuf, sbuf) == lp->lbuf && (pTag = xref_parseTagDefinitionFromLine(lp)) != NULL) {
			if ((dlpCurr = (DLGSTRINGLIST *)ll_insert((LINKED_LIST**)&dlp, sizeof *dlp)) == 0) {
				ll_destroy((LINKED_LIST**)&dlp, xref_deallocateTag);
				return 0;
			}
			dlpCurr->pszString = sbuf;
			dlpCurr->nSize = (int)strlen(sbuf);
			dlpCurr->pAny = xref_allocateTag(pTag);
		}
	}

	switch (ll_size((LINKED_LIST*)dlp)) {

	case 0: 
		return 0;

	case 1:
		dlpCurr = dlp;
		break;

	default: {
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
		ll_destroy((LINKED_LIST**)&dlp, xref_deallocateTag);
		_tagCancelled = TRUE;
		return 0;
	}

	pTag = dlpCurr->pAny;
	memcpy(&_tag, pTag, sizeof _tag);
	ll_destroy((LINKED_LIST**)&dlp, xref_deallocateTag);
	return &_tag;
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
int xref_navigateCrossReference(char *s) {
	TAG *		tp;
	char     	ebuf[ESIZE];
	char	    fnam[256];
	int			ret = 0;
	TAGSOURCE* ttl;
	FTABLE *	fp;
	WINFO* wp = ww_getCurrentEditorWindow();

	if (!s || !*s || wp == NULL)
		return 0;
	fp = wp->fp;
	if ((ttl = grammar_getTagSources(fp->documentDescriptor->grammar)) == NULL) {
		return 0;
	}
	fp  = &_tagfile;

	find_setCurrentSearchExpression(s);
	_tagword = _strdup(s);
	s = _tagword;

	_tagCancelled = FALSE;
	while (ttl && ret == 0 && _tagCancelled == FALSE) {
		switch(ttl->type[0]) {
			case TST_TAGFILE:
				if (xref_loadTagFile(fp, ttl->fn) == 0) {
					break;
				}
				if ((tp = findtag(s,fp)) != 0L) {
					strcpy(fnam,tp->filename);
					fm_savepos(s);
					xref_openFile(fnam,-1L,(WINDOWPLACEMENT*)0);
					if (ft_getCurrentDocument()) {
						RE_PATTERN* pPattern;
						if (pPattern = find_regexCompile(ebuf, tp->searchCommand, (int) RE_DOREX)) {
							find_expressionInCurrentFile(1,pPattern,O_WRAPSCAN);
							ret = 1;
						}
					}
				}
				ft_bufdestroy(fp);
				break;
			case TST_HELPFILE:
				ret = help_showHelpForKey(ttl->fn, s);
				break;
			}
		ttl = ttl->next;
	}

	if (!ret && !_tagCancelled) {
		error_showErrorById(IDS_MSGUNKNWONTAG, (LPSTR)s);
	}
	free(_tagword);
	_tagword = 0;
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
	RE_PATTERN *pPattern = xref_initializeNavigationPattern(ebuf,_searchListNavigationPattern);

	while((lp = lp->next) != 0L) {		/* we may skip 1st line ! */
		if (lp->len && (tp = xref_parseTagDefinition(lp,pPattern)) != 0L) {
			if (tp->searchCommand[0]) {
				/* this means -> windowstate given */
				wsp = &ws;
				prof_getWindowSettings(tp->searchCommand,wsp);
			}
			xref_openFile(tp->filename, tp->ln-1L, wsp);
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

	RE_PATTERN *pPattern = xref_initializeNavigationPattern(ebuf,steperror);

	switch (dir) {
	case LIST_PREV:
dobackward:
		while((lineno--, (lp = lp->prev) != 0L) && 
		      (tp = xref_parseTagDefinition(lp, pPattern)) == 0L);
		break;
	case LIST_NEXT:
doforward:
		while((lineno++, (lp = lp->next) != 0L) &&
		      (tp = xref_parseTagDefinition(lp, pPattern)) == 0L);
		break;
	case LIST_START:
		lp     = fp->firstl;
		lineno = 0;
		if ((tp = xref_parseTagDefinition(lp, pPattern)) == 0L) goto doforward;
		break;
	case LIST_END:
		lp     = fp->lastl;
		lineno = fp->nlines;
		goto dobackward;
	default:
		if ((tp = xref_parseTagDefinition(lp, pPattern)) == 0L)
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
		if (*tp->filename == '/' || tp->filename[1] == ':') {
			lstrcpy(fullname, tp->filename);
		} else {
			string_splitFilename(fp->fname, fullname, (char *)0);
			string_concatPathAndFilename(fullname, fullname, tp->filename);
		}
		if (xref_openFile(fullname, tp->ln-1L, (WINDOWPLACEMENT*)0)) {
			if (tp->searchCommand[0]) {
				int col;
				int len;
				if (sscanf(tp->searchCommand, "%d/%d", &col, &len) != EOF && len > 0) {
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

