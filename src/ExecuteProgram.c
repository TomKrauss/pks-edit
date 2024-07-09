/*
 * ExecuteProgram.c
 *
 * Project: PKS Edit for Windows
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
int EdExecute(long flags, LPSTR cmdline, LPSTR newdir, LPSTR errfile) {
	int				show;
	char			szTemp[1024];
	char			outfile[1024];
	char			infile[1024];
	char *			tmp = file_getTempDirectory();
	char *			getenv(const char *);
	BOOL			bUInited;
	HANDLE			hStdin;
	HANDLE			hStdout;

	if (!cmdline) {
		return 0;
	}
	outfile[0] = 0;
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	BOOL bConsumer = flags & (EX_RDCONV | EX_RDIN);
	BOOL bProducer = flags & (EX_RDCONV | EX_RDOUT);

	bUInited = FALSE;

	if (newdir && newdir[0]) {
		fsel_changeDirectory(newdir);
	}

	show = (flags & EX_SYMBOL) ? SW_SHOWMINIMIZED : SW_SHOWNORMAL;
	DWORD dwCreationFlags = 0;
	if (flags & EX_RUN_IN_SHELL) {

		string_concatPathAndFilename(infile, tmp, "IN.___");
		string_concatPathAndFilename(outfile, tmp, "OUT.___");

		if (errfile && errfile[0]) {
			flags |= EX_RDOUT;
			lstrcpy(outfile, errfile);
		}

		wsprintf(szTemp,"cmd /e:1024 /c %s", cmdline);
		cmdline = szTemp;
		dwCreationFlags = CREATE_NO_WINDOW;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	si.wShowWindow = show;
	SECURITY_ATTRIBUTES saAttr = {
		.nLength = sizeof(saAttr),
		.bInheritHandle = TRUE
	};
	if (bConsumer) {
		if (!bl_writeToFile(ww_getCurrentEditorWindow(), infile)) {
			return 0;
		}
		hStdin = CreateFile(
			infile,
			GENERIC_READ,
			0,
			&saAttr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_READONLY,
			NULL);
	}
	if (bProducer) {
		hStdout = CreateFile(
			outfile,
			GENERIC_WRITE,
			0,
			&saAttr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	}
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.hStdInput = hStdin;
	si.hStdOutput = hStdout;
	si.dwFlags |= STARTF_USESTDHANDLES;
	if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE, dwCreationFlags, NULL, NULL, &si, &pi)) {
		exec_error(cmdline, (int) GetLastError());
		return 0;
	}

	// Wait until child process exits.
	if (bProducer) {
		if (WaitForSingleObject(pi.hProcess, 5000) == WAIT_FAILED) {
			exec_error(cmdline, (int)GetLastError());
		}
		CloseHandle(hStdout);
	}
	if (bConsumer) {
		CloseHandle(hStdin);
	}
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE* fp = wp ? wp->fp : 0;

	if ((flags & EX_RDCONV) && wp) {
		undo_startModification(fp);
		bUInited = TRUE;
		EdBlockDelete(wp, 0);
	}


	if (errfile && errfile[0]) {
		xref_openSearchList(errfile, 1);
	} else if (wp && bProducer) {
		if (!bUInited) {
			undo_startModification(fp);
		}
		bl_insertPasteBufFromFile(wp, outfile);
	}

	return 1;
}


