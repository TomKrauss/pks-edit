/*
 * ProgramArguments.c
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
 *
 */

#include <windows.h>
#include "trace.h"
#include "lineoperations.h"
#include "stringutil.h"
#include "findandreplace.h"

#define iswhite(c)	(c == ' ' || c == '\t')

extern int 		find_expressionAgainInCurrentFile(int dir);
extern void 	stepselectcompiler(char *pszName);
extern int 		caret_placeCursorInCurrentFile(long ln,long col);
extern int 		showtag(char *s);
extern void 	stepnofsel(char *fn, int cmpflg);
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

	if (*arg != '-' && *arg != '/') {
		tagopen(arg,0L,(void*)0);
	} else {
		arg += 2;
		switch(arg[-1]) {
		case '/':
			if (ft_CurrentDocument() != 0) {
				lstrcpy(_currentSearchAndReplaceParams.searchPattern,arg);
				find_expressionAgainInCurrentFile(1);
			}
			break;
		case 'C':
			stepselectcompiler(arg);
			break;
		case 'g':	
			line = Atol(arg) -1L;
			if (caret_placeCursorInCurrentFile(line,0L)) {
				line = -1L;
			}
			break;
		case 'c':
			if (ft_CurrentDocument()) {
				caret_placeCursorInCurrentFile(ft_CurrentDocument()->ln,Atol(arg)-1L);
			}
			break;
		case 't':
			showtag(arg);
			break;
		case 'i':
			_openIconic = TRUE;
			break;
		case 'n':
			_runInteractive = FALSE;
			return 0;
		default:
			stepnofsel(arg,1); 		/* Edit Compiler-Errors 	*/

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


