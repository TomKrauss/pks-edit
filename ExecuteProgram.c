/*
 * ExecuteProgram.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: executing programs
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include <windows.h>
#if !defined(WIN32)
#include <toolhelp.h>
#endif
#include "edexec.h"
#include "pksedit.h"
#include "edierror.h"
#include "errordialogs.h"
#include "documentmodel.h"
#include "winterf.h"
#include "fileutil.h"
#include "stringutil.h"
#include "textblocks.h"

extern void fsel_changeDirectory(char* pszPath);
extern void xref_openSearchList(char *fn, int cmpflg);
extern void undo_startModification(FTABLE *fp);

extern FTABLE 	_outfile;

BOOL	  		bTaskFinished;

#define	EX_DOS				EX_CD

/*--------------------------------------------------------------------------
 * exec_error()
 */
static void exec_error(char *cmd, int errcode)
{
	error_showErrorById(IDS_MSGEXECERROR,errcode,cmd);
}

/*--------------------------------------------------------------------------
 * EdExecute()
 * EX_RDIN,EX_RDOUT:
 *	this flags are named by seeing the editors behavior, 
 *
 *	EX_RDIN means:	get the output of the command after execution
 *   EX_RDOUT     : write the marked block to an output file before execution
 * 
 */
int EdExecute(long flags, long unused, LPSTR cmdline, LPSTR newdir, LPSTR errfile) {
	int				show;
	char			szTemp[256];
	char			outfile[256];
	char			infile[256];
	char *			tmp = file_getTempDirectory();
	char *			getenv(const char *);
	BOOL			bUInited;
	HANDLE			hStdin, hStdout;

	if (!cmdline) {
		return 0;
	}
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdin = GetStdHandle(STD_INPUT_HANDLE);

	bUInited = FALSE;

	if (newdir && newdir[0]) {
		fsel_changeDirectory(newdir);
	}

	show = (flags & EX_SYMBOL) ? SW_SHOWMINIMIZED : SW_SHOWNORMAL;

	if (flags & EX_DOS) {

		string_concatPathAndFilename(infile, tmp, "IN.___");
		string_concatPathAndFilename(outfile, tmp, "OUT.___");

		if (flags & EX_RDOUT) {
			if (!bl_writeToFile(ww_getCurrentEditorWindow(), infile)) {
				return 0;
			}
		}

		if (errfile && errfile[0]) {
			flags |= EX_RDIN;
			lstrcpy(outfile, errfile);
		}

		wsprintf(szTemp,"cmd /e:1024 /c %s", cmdline);
		cmdline = szTemp;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = show;
	if (flags & (EX_RDCONV|EX_RDOUT)) {
		hStdin = CreateFile(
			infile,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_READONLY,
			NULL);
	}
	if (flags & (EX_RDCONV | EX_RDOUT)) {
		hStdout = CreateFile(
			outfile,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_READONLY,
			NULL);
	}
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdInput = hStdin;
	si.hStdOutput = hStdout;
	si.dwFlags |= STARTF_USESTDHANDLES;
	if (!CreateProcess(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		exec_error(cmdline, (int) GetLastError());
		return 0;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	WINFO* wp = ww_getCurrentEditorWindow();

	if ((flags & EX_RDCONV) && wp) {
		undo_startModification(wp->fp);
		bUInited = TRUE;
		EdBlockDelete(wp, 0);
	}


	if (errfile && errfile[0]) {
		xref_openSearchList(errfile, 1);
	} else if (wp && wp->fp && (flags & EX_RDIN)) {
		if (!bUInited) {
			undo_startModification(wp->fp);
		}
		bl_insertPasteBufFromFile(wp, outfile);
	}

	return 1;
}


