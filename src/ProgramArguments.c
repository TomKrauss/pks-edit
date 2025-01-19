/*
 * ProgramArguments.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose:
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
#include "trace.h"
#include "caretmovement.h"
#include "documentmodel.h"
#include "stringutil.h"
#include "crossreferencelinks.h"
#include "winfo.h"
#include "fileutil.h"
#include "findandreplace.h"
#include "ftw.h"
#include "pksrc.h"
#include "edierror.h"
#include "editorconfiguration.h"

#define iswhite(c)	(c == ' ' || c == '\t')

extern void 	prof_setinifile(char *fn);

extern int		_runInteractive;
extern int		_openIconic;

/*------------------------------------------------------------
 * arguments_parsePhase1()
 */
static int arguments_parsePhase1(char *arg) {
	if (*arg == '-' || *arg == '/') {
		if (arg[1] == 'i') {
			prof_setinifile(arg + 2);
		}
		if (arg[1] == 'v') {
			log_setLogLevel(DEBUG_ALL);
		}
	}
	return 1;
}

static int _maxMultiFileCount;

static int argument_open(const char* pszFilespec, DTA* p) {
	xref_openFile(pszFilespec, 0L, (void*)0);
	if (--_maxMultiFileCount <= 0) {
		error_showErrorById(IDS_TRYING_TO_OPEN_TOO_MANY_FILES, GetConfiguration()->maximumNumberOfOpenWindows);
		return 1;
	}
	return 0;
}

static void argument_openFiles(const char* pszDirectory, const char* pszFilespec) {
	_maxMultiFileCount = GetConfiguration()->maximumNumberOfOpenWindows;
	if (_maxMultiFileCount <= 0) {
		_maxMultiFileCount = 32000;
	}

	if (strchr(pszFilespec, '*') || strchr(pszFilespec, '?')) {
		_ftw(pszDirectory, argument_open, 999, pszFilespec, NORMALFILE | ARCHIV | WPROTECT, NULL);
	} else {
		if (file_exists(pszFilespec) != 0 && pszDirectory[0] != 0) {
			char szDest[EDMAXPATHLEN];
			string_concatPathAndFilename(szDest, pszDirectory, pszFilespec);
			xref_openFile(szDest, 0L, (void*)0);
		} else {
			xref_openFile(pszFilespec, 0L, (void*)0);
		}
	}
}

/*------------------------------------------------------------
 * arguments_parsePhase1()
 */
static int arguments_parsePhase2(char *arg) {
	static char szPath[EDMAXPATHLEN] = ".";
	if (*arg != '-' && *arg != '/') {
		argument_openFiles(szPath, arg);
	} else {
		long line;
		WINFO* wp = ww_getCurrentEditorWindow();

		arg += 2;
		switch(arg[-1]) {
		case '/':
			if (wp != 0) {
				lstrcpy(_currentSearchAndReplaceParams.searchPattern,arg);
				find_expressionAgainInCurrentFile(wp, 1);
			}
			break;
		case 'p':
			strcpy(szPath, arg);
			break;
		case 'l':
		case 'g':	
			line = (long)string_convertToLong(arg) -1L;
			if (wp != NULL) {
				caret_placeCursorInCurrentFile(wp, line, 0L);
			}
			break;
		case 'w':	// to open the just opened file in wysiwyg mode.
			if (wp != NULL) {
				ww_changeDisplayMode(wp, wp->dispmode|SHOW_WYSIWYG_DISPLAY);
			}
			break;
		case 'c':
			if (wp != NULL) {
				caret_placeCursorInCurrentFile(wp, wp->caret.ln, (long)string_convertToLong(arg)-1L);
			}
			break;
		case 't':
			xref_navigateCrossReference(arg);
			break;
		case 'v':
			break;
		case 'i':
			_openIconic = TRUE;
			break;
		case 'n':
			_runInteractive = FALSE;
			return 0;
		case 'T':
			xref_openBuildOutputFile(arg, FALSE, -1); 		/* Edit Compiler-Errors 	*/
			break;
		default:
			argument_openFiles(szPath, arg);
			break;
		}
	}
	return 1;
}

/*------------------------------------------------------------
 * arguments_get()
 */
static int arguments_get(char *args, int (*argfunc)(char *arg))
{
	char *	dend;
	char 	argument[EDMAXPATHLEN] = { 0 };
	
	dend = &argument[sizeof argument-1];
	while(*args) {
		char* d = argument;
		if (*args == '"') {
			args++;
			while(*args && *args != '"') {
				*d++ = *args++;
			}
			args++;
		} else {
			while(*args && !iswhite(*args) && d < dend) {
				*d++ = *args++;
			}
		}
		*d = 0;
		if (*argument && !(*argfunc)(argument)) {
			return 0;
		}
		while(iswhite(*args)) {
			args++;
		}
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * arguments_getForPhase1()
 */
void arguments_getForPhase1(char *args)
{

	arguments_get(args, arguments_parsePhase1);
}

/*--------------------------------------------------------------------------
 * arguments_getPhase2()
 */
void arguments_getPhase2(char *args)
{

	arguments_get(args, arguments_parsePhase2);
}


