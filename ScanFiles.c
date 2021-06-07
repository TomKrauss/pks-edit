/*
 * ScanFiles.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose:
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include <string.h>
#include <malloc.h>

#include "trace.h"
#include "lineoperations.h"
#include "winfo.h"
#include "winterf.h"
#include "pksedit.h"
#include "ftw.h"
#include "edierror.h"
#include "stringutil.h"
#include "errordialogs.h"
#include "fileutil.h"
#include "pathname.h"
#include "regexp.h"
#include "crossreferencelinks.h"

extern char *	_datadir;
static FTABLE	*_outfile;
static long 	_line;
static int 		_abortOnFirstMatch,_trymatch;

/*---------------------------------*/
/* ft_readDocumentFromFile()					*/
/*---------------------------------*/
extern int ft_readDocumentFromFile(int fd, unsigned char* (*f)(FTABLE*, DOCUMENT_DESCRIPTOR*, unsigned char*, unsigned char*), void* par);


/*--------------------------------------------------------------------------
 * present()
 * display info in abort dialog box and update .GRP-file
 */
static int _nfound;
static RE_PATTERN* _compiledPattern;

static void present(char *fn) {
	_nfound++;
	error_showMessageInStatusbar(IDS_MSGNTIMESFOUND, _nfound);
	xref_addSearchListEntry(_outfile,fn,_line,"");
}

/*--------------------------------------------------------------------------
 * xref_addSearchListEntry()
 * Add an entry to the "current pSearchExpression list" file in the standard PKS Edit pSearchExpression list
 * navigation format ("filename", line lineNumer: remarks).
 */
int xref_addSearchListEntry(FTABLE* fp, char* fn, long line, char* remark) {
	char buf[EDMAXPATHLEN];

	wsprintf(buf, "\"%s\", line %ld: %s", (LPSTR)fn, line + 1L, (LPSTR)remark);
	return ln_createAndAdd(fp, buf, lstrlen(buf), 0);
}


/*--------------------------------------------------------------------------
 * scanlines()
 * 
 */
static unsigned char *scanlines(FTABLE *fp, DOCUMENT_DESCRIPTOR* linp, unsigned char *p, unsigned char *qend) {
	register char	*	q;
	register char 		nl = '\n';
	char *			stepend;
	RE_MATCH		match;

	q = p;
	while (p < qend) {
		if (*p++ == nl) {
longline_scan:
			stepend = p-2;
			if (*stepend == '\r')
				stepend--;
			if (_compiledPattern != NULL && step(_compiledPattern, q,stepend, &match)) {
				present(fp->fname);
				if (_abortOnFirstMatch)
					return 0;
			}
			_line++;
			q = p;
		}
	}

	if ((qend - q) >= FBUFSIZE) {

	/* this "line" is too long: simulate break */
		goto longline_scan;

	}

	return q;
}

/*--------------------------------------------------------------------------
 * matchInFile()
 * scan for a pSearchExpression pattern in file fn
 */
static int matchInFile(char *fn, DTA *stat) {	
	int 	fd;

#if defined(WIN32)
	if (stat->attrib & _A_SUBDIR)
#else
	if (stat->ff_attrib & SUBDIR)
#endif
		return 0;

	_line = 0L;
	progress_showMonitorMessage(string_abbreviateFileName(fn));

	if (!_trymatch) {
		present(fn);
		return(0);
	}

	if ((fd = file_openFile(fn)) < 0) {
		return -1;
	}

	ft_readDocumentFromFile(fd,scanlines,fn);

	file_closeFile(&fd);

	return 0;
}

/*--------------------------------------------------------------------------
 * find_matchesInFiles()
 * Perform a recursive pSearchExpression in a list of pates with a given filename pattern.
 */
int find_matchesInFiles(char *pPathes, char* pFilenamePattern, char *pSearchExpression, int nMaxRecursion, int bAbortOnFirstMatch) {
	char *		path;
	char *		pathlist;
	char		stepfile[256];

	_abortOnFirstMatch = bAbortOnFirstMatch;
	_nfound = 0;
	string_concatPathAndFilename(stepfile, _datadir, "pksedit.grp");

	if (!*pSearchExpression) {
		_trymatch = 0;
	} else {
		if ((_compiledPattern = regex_compileWithDefault(pSearchExpression)) != NULL) {
			return 0;
		}
		_trymatch = 1;
	}

	pathlist = _alloc(300);
	_outfile = calloc(1, sizeof *_outfile);
	lstrcpy(pathlist,pPathes);
	progress_startMonitor(IDS_ABRTRETREIVE);
	if ((path = strtok(pathlist,",;")) != 0) {
		do {	
			if (_ftw(path,matchInFile,nMaxRecursion,
					 pFilenamePattern, NORMALFILE|ARCHIV|WPROTECT) == 1) break;
		} while ((path = strtok((char *)0,",;")) != 0);
	}

	ft_writeFileAndClose(_outfile,stepfile,0);
	progress_closeMonitor(0);

	_free(pathlist);
	_free(_outfile);

	return xref_openSearchList(stepfile,0) , 1;
}


