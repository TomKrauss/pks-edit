/*
 * ScanFiles.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: search in files and folders
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
#include <malloc.h>

#include "trace.h"
#include "lineoperations.h"
#include "winfo.h"
#include "winterf.h"
#include "pksedit.h"
#include "history.h"
#include "ftw.h"
#include "edierror.h"
#include "stringutil.h"
#include "errordialogs.h"
#include "fileutil.h"
#include "pathname.h"
#include "regexp.h"
#include "crossreferencelinks.h"

extern char *	_pksSysFolder;
static FTABLE	*_outfile;
static long 	_line;
static int 		_abortOnFirstMatch,_trymatch,_ignoreBinary;

/*--------------------------------------------------------------------------
 * find_inFilesMatchFound()
 * display info in abort dialog box and update the cross reference list.
 */
static int _nfound;
static RE_PATTERN* _compiledPattern;
static void find_inFilesMatchFound(char *fn, int nStartCol, int nLength) {
	char szBuf[128];

	_nfound++;
	error_showMessageInStatusbar(IDS_MSGNTIMESFOUND, _nfound);
	if (nStartCol >= 0) {
		wsprintf(szBuf, "%d/%d", nStartCol, nLength);
	} else {
		szBuf[0] = 0;
	}
	xref_addSearchListEntry(_outfile, fn, _line, szBuf);
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
 * find_inLine()
 * 
 */
static unsigned char *find_inLine(void *pFilename, EDIT_CONFIGURATION* linp, unsigned char *p, unsigned char *qend) {
	register char	*	q;
	register char 		nl = '\n';
	char *			stepend;
	RE_MATCH		match;

	q = p;
	while (p < qend) {
		if (*p++ == nl) {
longline_scan:
			stepend = p-1;
			if (stepend > q && stepend[-1] == '\r')
				stepend--;
			if (_compiledPattern != NULL && step(_compiledPattern, q,stepend, &match)) {
				find_inFilesMatchFound((char*)pFilename, (int)(match.loc1-q), (int)(match.loc2-match.loc1));
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

/*
 * Heuristic attempt to test, whether a file is a binary file.
 */
static int scan_isBinaryFile(int fd) {
	char buffer[1024];
	int nBytesRead;
	int nControl = 0;

	nBytesRead = _lread(fd, buffer, sizeof buffer);
	for (int i = 0; i < nBytesRead; i++) {
		unsigned char c = (unsigned char)buffer[i];
		if (c != '\n' && c != '\t' && c != '\r' && iscntrl(c)) {
			if (++nControl > 5) {
				return 1;
			}
		}
	}
	_llseek(fd, 0l, SEEK_SET);
	return nControl > 5;
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

	if ((fd = file_openFile(fn)) <= 0) {
		return -1;
	}

	if (!_ignoreBinary || !scan_isBinaryFile(fd)) {
		progress_showMonitorMessage(string_abbreviateFileName(fn));
		if (!_trymatch) {
			find_inFilesMatchFound(fn, -1, 0);
		} else {
			ft_readDocumentFromFile(fd, find_inLine, fn);
		}
	}

	file_closeFile(&fd);

	return 0;
}

/*--------------------------------------------------------------------------
 * find_matchesInFiles()
 * Perform a recursive pSearchExpression in a list of pates with a given filename pattern.
 */
int find_matchesInFiles(char *pPathes, char* pFilenamePattern, char *pSearchExpression, int nOptions, int nMaxRecursion, int bAbortOnFirstMatch) {
	char *		path;
	char *		pathlist;
	char		stepfile[256];

	_abortOnFirstMatch = bAbortOnFirstMatch;
	_nfound = 0;
	_ignoreBinary = nOptions & RE_IGNORE_BINARY;
	string_concatPathAndFilename(stepfile, _pksSysFolder, "pksedit.grp");
	hist_saveString(FILE_PATTERNS, pFilenamePattern);

	if (!*pSearchExpression) {
		_trymatch = 0;
	} else {
		if ((_compiledPattern = regex_compileWithDefault(pSearchExpression)) == NULL) {
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


