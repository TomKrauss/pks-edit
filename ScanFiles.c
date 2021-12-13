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
#include "caretmovement.h"
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
#include "findandreplace.h"
#include "mainframe.h"
#include "hashmap.h"

static struct tagSEARCH_CONTEXT {
	FILE* sc_file;
	FTABLE* sc_ftable;
	char* sc_fileName;
	long  sc_line;
	size_t sc_matches;
	size_t sc_files;
} _searchContext;

static int 	 _abortOnFirstMatch,_trymatch,_ignoreBinary;

static const char* _grepFileFormat = "\"%s\", line %ld: %s";

/*
 * Initialize a searchlist file.
 */
extern void xref_initSearchList(FTABLE* fp);

/*
 * Returns a compiled RE_PATTERN to scan the lines in a search result list.
 * Note, that this method returns the pointer to a shared RE_PATTERN data structure
 * and should therefore only be used, if not in conflicts.
 */
extern RE_PATTERN* xref_compileSearchListPattern();

/*
 * Calculate the minimum of two values. 
 */
static const inline size_t xref_min(size_t val, size_t min) {
	return val > min ? min : val;
}

/*--------------------------------------------------------------------------
 * find_inFilesMatchFound()
 * display info in abort dialog box and update the cross reference list.
 */
static RE_PATTERN* _compiledPattern;
static void find_inFilesMatchFound(char *fn, int nStartCol, int nLength, char* pszLine) {
	char szBuf[EDMAXPATHLEN+100];

	_searchContext.sc_matches++;
	if (nStartCol >= 0) {
		wsprintf(szBuf, "%d/%d", nStartCol, nLength);
		if (pszLine) {
			int nMaxVis = 40;
			strcat(szBuf, " - ");
			size_t nLen = xref_min(nStartCol, nMaxVis);
			strncat(szBuf, &pszLine[nStartCol - nLen], nLen);
			strcat(szBuf, "~");
			nLen = xref_min(nLength, nMaxVis);
			strncat(szBuf, &pszLine[nStartCol], nLen);
			strcat(szBuf, "~");
			char* pszEnd = &pszLine[nStartCol + nLength];
			char* pszLineEnd = strchr(pszEnd, '\n');
			nLen = xref_min(pszLineEnd ? pszLineEnd - pszEnd : strlen(pszEnd), nMaxVis);
			strncat(szBuf, pszEnd, nLen);
		}
	} else {
		szBuf[0] = 0;
	}
	// TODO: why _line+1??
	fprintf(_searchContext.sc_file, _grepFileFormat, fn, _searchContext.sc_line+1, szBuf);
	fprintf(_searchContext.sc_file, "\n");
	// Causes the UI of the error file preview to update.
	if ((_searchContext.sc_matches < 1000 && (_searchContext.sc_matches % 100) == 1) ||
		(_searchContext.sc_matches < 10000 && (_searchContext.sc_matches % 1000) == 1) ||
		(_searchContext.sc_matches < 100000 && (_searchContext.sc_matches % 10000) == 1) ||
		(_searchContext.sc_matches % 100000 == 1)) {
		_searchContext.sc_ftable->ti_modified = 0;
	}
}

/*--------------------------------------------------------------------------
 * xref_addSearchListEntry()
 * Add an entry to the "current pSearchExpression list" file in the standard PKS Edit pSearchExpression list
 * navigation format ("filename", line lineNumer: remarks).
 */
int xref_addSearchListEntry(FTABLE* fp, char* fn, long line, char* remark) {
	char buf[EDMAXPATHLEN];

	wsprintf(buf, _grepFileFormat, (LPSTR)fn, line + 1L, (LPSTR)remark);
	if (ln_createAndAdd(fp, buf, (int)strlen(buf), 0)) {
		return 1;
	}
	return 0;
}


/*--------------------------------------------------------------------------
 * find_inLine()
 * 
 */
static unsigned char *find_inLine(void *pFilename, EDIT_CONFIGURATION* linp, unsigned char *p, const unsigned char *qend) {
	char	*	q;
	char 		nl = '\n';
	char *		stepend;
	RE_MATCH	match;

	q = p;
	while (p < qend) {
		if (*p++ == nl) {
longline_scan:
			stepend = p-1;
			if (stepend > q && stepend[-1] == '\r')
				stepend--;
			if (_compiledPattern != NULL) {
				char* pLineStart = q;
				while (q < stepend && regex_match(_compiledPattern, q, stepend, &match)) {
					find_inFilesMatchFound((char*)pFilename, (int)(match.loc1 - pLineStart), (int)(match.loc2 - match.loc1), pLineStart);
					q = match.loc2;
					if (_abortOnFirstMatch) {
						return 0;
					}
				}
			}
			_searchContext.sc_line++;
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

/*
 * Perform a replace in all files found with the given search parameters. 
 */
static int find_replaceInFile(intptr_t pFilename, SEARCH_AND_REPLACE_PARAMETER* pParam) {
	WINFO* wp = NULL;
	char* pszFilename = (char*)pFilename;
	BOOL bSaveAndClose = FALSE;
	if (ft_activateWindowOfFileNamed(pszFilename)) {
		wp = ww_getCurrentEditorWindow();
	} else {
		FTABLE* fp = ft_openFileWithoutFileselector(pszFilename, 0, &(FT_OPEN_OPTIONS) { NULL, -1 });
		if (fp) {
			wp = WIPOI(fp);
			EdSelectWindow(wp->win_id);
		}
		bSaveAndClose = TRUE;
	}
	if (wp == NULL) {
		return 0;
	}
	caret_placeCursorInCurrentFile(wp, 0, 0);
	REPLACE_TEXT_RESULT rResult = EdReplaceText(wp, RNG_GLOBAL, REP_REPLACE);
	if (rResult == RTR_ALL) {
		pParam->options &= ~RE_CONFIRM_REPLACEMENT;
	}
	if (rResult != RTR_ERROR && rResult != RTR_CANCELLED) {
		if (bSaveAndClose) {
			EdSaveFile(SAV_SAVE|SAV_QUIT);
		}
		return 1;
	}
	return 0;
}

/*
 * Scans one file for the configured search pattern.
 * Return 0 on success.
 */
static int find_inFile(intptr_t p1, void* pUnused) {
	char* pszFile = (char*)p1;
	int 	fd;
	_searchContext.sc_line = 0L;

	if ((fd = file_openFile(pszFile)) <= 0) {
		return 0;
	}

	if (!_ignoreBinary || !scan_isBinaryFile(fd)) {
		size_t nOldFound = _searchContext.sc_matches;
		progress_showMonitorMessage(string_abbreviateFileName(pszFile));
		if (!_trymatch) {
			find_inFilesMatchFound(pszFile, -1, 0, 0);
		}
		else {
			long nPage = CP_ACP;
			ft_readDocumentFromFile(fd, &nPage, find_inLine, pszFile);
		}
		if (nOldFound != _searchContext.sc_matches) {
			_searchContext.sc_files++;
		}
	}

	file_closeFile(&fd);

	return 1;

}
/*--------------------------------------------------------------------------
 * matchInFile()
 * scan for a pSearchExpression pattern in file filename
 */
static int matchInFile(char *fn, DTA *stat) {

	if (stat->attrib & _A_SUBDIR) {
		return 0;
	}
	return find_inFile((intptr_t)fn,0) == 1 ? 0 : -1;
}

/*
 * Scan all files from the current search result list 
 */
static int find_matchesInSearchResults(HASHMAP* pFiles) {
	hashmap_forEachKey(pFiles, find_inFile, 0);
	return 1;
}

/*
 * Parse the filename from a 
 */
static unsigned char* find_collectFileFromLine(HASHMAP* pResult, EDIT_CONFIGURATION* pConfig, unsigned char* pStart, unsigned char* pEnd) {
	RE_MATCH match;
	char szBuf[EDMAXPATHLEN];

	while (pStart < pEnd) {
		char* pszLineEnd = strchr(pStart, '\n');
		if (!pszLineEnd) {
			return pStart;
		}
		if (regex_match(_compiledPattern, pStart, pszLineEnd, &match)) {
			size_t nLen = match.braelist[0] - match.braslist[0];
			if (nLen) {
				strncpy(szBuf, match.braslist[0], nLen);
				szBuf[nLen] = 0;
				if (!hashmap_containsKey(pResult, (intptr_t)szBuf)) {
					hashmap_put(pResult, (intptr_t)_strdup(szBuf), (intptr_t)1);
				}
			}
		}
		pStart = pszLineEnd + 1;
	}
	return pEnd;
}

static HASHMAP* find_collectFiles(char* pszStepfile) {
	HASHMAP* pResult = hashmap_create(37, 0, 0);
	int 	fd;

	if ((fd = file_openFile(pszStepfile)) <= 0) {
		return pResult;
	}
	long nPage = CP_ACP;
	ft_readDocumentFromFile(fd, &nPage, find_collectFileFromLine, pResult);
	file_closeFile(&fd);
	return pResult;
}

/*--------------------------------------------------------------------------
 * find_matchesInFiles()
 * Perform a recursive pSearchExpression in a list of pates with a given filename pattern.
 */
int find_matchesInFiles(SEARCH_AND_REPLACE_PARAMETER* pParams, FIND_IN_FILES_ACTION fAction) {
	char *		pathlist;
	char		stepfile[512];
	char		title[512];
	char* pPathes = pParams->pathlist;
	char* pFilenamePattern = pParams->filenamePattern;
	char* pSearchExpression = pParams->searchPattern;
	int nOptions = pParams->options;
	HASHMAP* pFilesMap = NULL;

	_abortOnFirstMatch = (nOptions & RE_SEARCH_ONCE) || fAction == FIF_REPLACE;
	memset(&_searchContext, 0, sizeof _searchContext);
	_ignoreBinary = nOptions & RE_IGNORE_BINARY;
	string_concatPathAndFilename(stepfile, config_getPKSEditTempPath(), "pksedit.grep");
	hist_saveString(FILE_PATTERNS, pFilenamePattern);

	if (nOptions & RE_SEARCH_IN_SEARCH_RESULTS) {
		_compiledPattern = xref_compileSearchListPattern();
		pFilesMap = find_collectFiles(stepfile);
	}
	if (!*pSearchExpression) {
		_trymatch = 0;
	} else {
		if ((_compiledPattern = regex_compileWithDefault(pSearchExpression)) == NULL) {
			return 0;
		}
		_trymatch = 1;
	}

	pathlist = malloc(1024);
	_searchContext.sc_fileName = stepfile;
	char* pMode = nOptions & RE_APPEND_TO_SEARCH_RESULTS ? "a" : "w";
	_searchContext.sc_file = _fsopen(stepfile, pMode, _SH_DENYNO);
	sprintf(title, "Matches of '%s' in '%s'", pSearchExpression, pPathes);
	fprintf(_searchContext.sc_file, "%s\n", title);
	FTABLE* fp;
	if (ft_activateWindowOfFileNamed(stepfile)) {
		fp = ft_getCurrentDocument();
	} else {
		fp = ft_openFileWithoutFileselector(stepfile, 0, &(FT_OPEN_OPTIONS) { DOCK_NAME_BOTTOM, CP_ACP });
	}
	_searchContext.sc_ftable = fp;
	ft_setTitle(fp, title);
	lstrcpy(pathlist,pPathes);
	progress_startMonitor(IDS_ABRTRETREIVE);
	if (nOptions & RE_SEARCH_IN_SEARCH_RESULTS) {
		find_matchesInSearchResults(pFilesMap);
		hashmap_destroySet(pFilesMap);
	} else {
		char* path;
		int nDepth = pParams->fileScanDepth;
		if ((path = strtok(pathlist, ",;")) != 0) {
			do {
				if (_ftw(path, matchInFile, nDepth < 0 ? 999 : nDepth,
					pFilenamePattern, NORMALFILE | ARCHIV | WPROTECT) == 1) break;
			} while ((path = strtok((char*)0, ",;")) != 0);
		}
	}
	fprintf(_searchContext.sc_file, "Found %ld matches in %ld files", (long)_searchContext.sc_matches, (long)_searchContext.sc_files);
	fclose(_searchContext.sc_file);
	progress_closeMonitor(0);
	free(pathlist);
	// Force reload of file.
	fp->ti_modified = 0;
	ft_checkForChangedFiles(FALSE);
	xref_initSearchList(fp);
	if (fAction == FIF_SEARCH) {
		return xref_navigateSearchErrorList(LIST_START);
	}
	_compiledPattern = xref_compileSearchListPattern();
	pFilesMap = find_collectFiles(stepfile);
	if (hashmap_size(pFilesMap) == 0) {
		error_showErrorById(IDS_MSGSTRINGNOTFOUND);
	} else {
		hashmap_forEachKey(pFilesMap, find_replaceInFile, pParams);
	}
	hashmap_destroySet(pFilesMap);
	return 1;
}


