/*
 * ProgramArguments.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
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
#include "lineoperations.h"
#include "stringutil.h"
#include "findandreplace.h"
#include "crossreferencelinks.h"
#include "winfo.h"

#define iswhite(c)	(c == ' ' || c == '\t')

extern int 		xref_navigateCrossReference(char *s);
extern void 	xref_openSearchList(char *fn, int cmpflg);
extern void 	prof_setinifile(char *fn);

extern int		_runInteractive;
extern int		_openIconic;

/*------------------------------------------------------------
 * Phase1Arg()
 */
static int Phase1Arg(char *arg)
{
	if ((*arg == '-' || *arg == '/') && arg[1] == 'i') {
		prof_setinifile(arg+2);
	}
	return 1;
}

/*------------------------------------------------------------
 * Phase1Arg()
 */
static int Phase2Arg(char *arg)
{
	long line;
	WINFO* wp;

	if (*arg != '-' && *arg != '/') {
		xref_openFile(arg,0L,(void*)0);
	} else {
		arg += 2;
		switch(arg[-1]) {
		case '/':
			if (ft_getCurrentDocument() != 0) {
				lstrcpy(_currentSearchAndReplaceParams.searchPattern,arg);
				find_expressionAgainInCurrentFile(1);
			}
			break;
		case 'g':	
			line = string_convertToLong(arg) -1L;
			if ((wp = ww_getCurrentEditorWindow()) != NULL) {
				if (caret_placeCursorInCurrentFile(wp, line, 0L)) {
					line = -1L;
				}
			}
			break;
		case 'c':
			if ((wp = ww_getCurrentEditorWindow()) != NULL) {
				caret_placeCursorInCurrentFile(wp, wp->caret.ln,string_convertToLong(arg)-1L);
			}
			break;
		case 't':
			xref_navigateCrossReference(arg);
			break;
		case 'i':
			_openIconic = TRUE;
			break;
		case 'n':
			_runInteractive = FALSE;
			return 0;
		default:
			xref_openSearchList(arg,1); 		/* Edit Compiler-Errors 	*/

		}
	}
	return 1;
}

/*------------------------------------------------------------
 * GetArgs()
 */
static int GetArgs(char *args, int (*argfunc)(char *arg))
{
	char *	dend;
	char *	d;
	char 	argument[256];
	
	dend = &argument[sizeof argument-1];
	while(*args) {
		d = argument;
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
 * GetPhase1Args()
 */
void GetPhase1Args(char *args)
{

	GetArgs(args, Phase1Arg);
}

/*--------------------------------------------------------------------------
 * GetPhase2Args()
 */
void GetPhase2Args(char *args)
{

	GetArgs(args, Phase2Arg);
}


