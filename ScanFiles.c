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
#include <stdio.h>
#include <string.h>
#include "alloc.h"

#include "editorconfiguration.h"
#include "trace.h"
#include "documentmodel.h"
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
#include "mainframe.h"

static char* _outfile;
static long 	_line;
static int 		_abortOnFirstMatch,_trymatch,_ignoreBinary;

static const char* _grepFileFormat = "\"%s\", line %ld: %s";

/*
 * Initialize a searchlist file.
 */
extern void xref_initSearchList(FTABLE* fp);

/*--------------------------------------------------------------------------
 * find_inFilesMatchFound()
 * display info in abort dialog box and update the cross reference list.
 */
static int _nfound;
static RE_PATTERN* _compiledPattern;
static void find_inFilesMatchFound(char *fn, int nStartCol, int nLength, char* pszLine) {
	char szBuf[1024];

	_nfound++;
	error_showMessageInStatusbar(IDS_MSGNTIMESFOUND, _nfound);
	if (nStartCol >= 0) {
		wsprintf(szBuf, "%d/%d", nStartCol, nLength);
		if (pszLine) {
			strcat(szBuf, " - ");
			size_t nLen = nStartCol;
			if (nLen > 20) {
				nLen = 20;
			}
			strncat(szBuf, &pszLine[nStartCol - nLen], nLen);
			strcat(szBuf, "~");
			strncat(szBuf, &pszLine[nStartCol], nLength);
			strcat(szBuf, "~");
			char* pszEnd = &pszLine[nStartCol + nLength];
			char* pszLineEnd = strchr(pszEnd, '\n');
			nLen = pszLineEnd ? pszLineEnd - pszEnd : strlen(pszEnd);
			if (nLen > 20) {
				nLen = 20;
			}
			strncat(szBuf, pszEnd, nLen);
		}
	} else {
		szBuf[0] = 0;
	}
	FILE* fp = fopen(_outfile, "a");
	if (fp == 0) {
		// TODO: error handling
		return;
	}
	// TODO: why _line+1??
	fprintf(fp, _grepFileFormat, fn, _line+1, szBuf);
	fprintf(fp, "\n");
	fclose(fp);
}

/*--------------------------------------------------------------------------
 * xref_addSearchListEntry()
 * Add an entry to the "current pSearchExpression list" file in the standard PKS Edit pSearchExpression list
 * navigation format ("filename", line lineNumer: remarks).
 */
int xref_addSearchListEntry(FTABLE* fp, char* fn, long line, char* remark) {
	char buf[EDMAXPATHLEN];

	wsprintf(buf, _grepFileFormat, (LPSTR)fn, line + 1L, (LPSTR)remark);
	if (ln_createAndAdd(fp, buf, lstrlen(buf), 0)) {
		return 1;
	}
	return 0;
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
			if (_compiledPattern != NULL && regex_match(_compiledPattern, q,stepend, &match)) {
				find_inFilesMatchFound((char*)pFilename, (int)(match.loc1-q), (int)(match.loc2-match.loc1), q);
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
 * scan for a pSearchExpression pattern in file filename
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
			find_inFilesMatchFound(fn, -1, 0, 0);
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
	char		stepfile[512];
	char		title[512];

	_abortOnFirstMatch = bAbortOnFirstMatch;
	_nfound = 0;
	_ignoreBinary = nOptions & RE_IGNORE_BINARY;
	string_concatPathAndFilename(stepfile, config_getPKSEditTempPath(), "pksedit.grep");
	hist_saveString(FILE_PATTERNS, pFilenamePattern);

	if (!*pSearchExpression) {
		_trymatch = 0;
	} else {
		if ((_compiledPattern = regex_compileWithDefault(pSearchExpression)) == NULL) {
			return 0;
		}
		_trymatch = 1;
	}

	pathlist = malloc(1024);
	_outfile = stepfile;
	FILE* pFile = fopen(stepfile, "w");
	sprintf(title, "Matches of '%s' in '%s'", pSearchExpression, pPathes);
	fprintf(pFile, "%s\n", title);
	fclose(pFile);
	FTABLE* fp;
	if (ft_activateWindowOfFileNamed(stepfile)) {
		fp = ft_getCurrentDocument();
	} else {
		fp = ft_openFileWithoutFileselector(stepfile, 0, DOCK_NAME_BOTTOM);
	}
	ft_setTitle(fp, title);
	lstrcpy(pathlist,pPathes);
	progress_startMonitor(IDS_ABRTRETREIVE);
	if ((path = strtok(pathlist,",;")) != 0) {
		do {	
			if (_ftw(path,matchInFile,nMaxRecursion,
					 pFilenamePattern, NORMALFILE|ARCHIV|WPROTECT) == 1) break;
		} while ((path = strtok((char *)0,",;")) != 0);
	}
	progress_closeMonitor(0);
	_outfile = NULL;
	free(pathlist);
	// Force reload of file.
	fp->ti_modified = 0;
	ft_checkForChangedFiles(FALSE);
	xref_initSearchList(fp);
	return EdErrorNext(LIST_START);
}


