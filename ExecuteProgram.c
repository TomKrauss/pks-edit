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
#include "lineoperations.h"
#include "winterf.h"
#include "fileutil.h"
#include "stringutil.h"
#include "textblocks.h"

extern void fsel_changeDirectory(LPSTR pszPath);
extern void xref_openSearchList(char *fn, int cmpflg);
extern void undo_startModification(FTABLE *fp);

extern FTABLE 	_outfile;
extern char * 	_datadir;
extern int	_playing;

BOOL	  		bTaskFinished;

#define	EX_DOS				EX_CD

/*--------------------------------------------------------------------------
 * ExecWaitNotify()
 */
static HTASK hTask;
BOOL FPE_FUNC ExecWaitNotify(WORD wID, DWORD dwData)
{
#if !defined(WIN32)
	if (wID == NFY_EXITTASK && GetCurrentTask() == hTask) {
		PostMessage(hwndFrame, WM_TASKFINISHED, 
				LOBYTE(dwData), NULL);
	}
#endif
	return FALSE;
}

/*--------------------------------------------------------------------------
 * exec_error()
 */
static void exec_error(char *cmd, int errcode)
{
	error_showErrorById(IDS_MSGEXECERROR,errcode,cmd);
}

/*--------------------------------------------------------------------------
 * GetTaskHandle()
 */
static HTASK GetTaskHandle(HINSTANCE hInst)
{
#if defined(WIN32)
	return 0;
#else
	TASKENTRY	te;
	
	te.dwSize = sizeof te;
	TaskFirst(&te);
	do {
		if (te.hInst == hInst) {
			return te.hTask;
		}
	} while(TaskNext(&te));
	return (HTASK) 0;
#endif
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
int EdExecute(long flags, long unused, LPSTR cmdline, LPSTR newdir, LPSTR errfile)
{
	int				ret;
	int				show;
	char				szTemp[256];
	char				outfile[256];
	char				infile[256];
	char				szRunBat[256];
	char *			tmp = file_getTempDirectory();
	char *			getenv(const char *);
	char *			pszPif;
	BOOL				bUInited;

	if (!cmdline) {
		return 0;
	}

	bUInited = FALSE;

	if (newdir && newdir[0]) {
		fsel_changeDirectory(newdir);
	}

	show = (flags & EX_SYMBOL) ? SW_SHOWMINIMIZED : SW_SHOWNORMAL;

	if (flags & EX_DOS) {

		string_concatPathAndFilename(infile, tmp, "IN.___");
		string_concatPathAndFilename(outfile, tmp, "OUT.___");

		if (flags & EX_RDOUT) {
			if (!EdBlockWriteToFile(infile)) {
				return 0;
			}
		}

		if (errfile && errfile[0]) {
			flags |= EX_RDIN;
			lstrcpy(outfile, errfile);
		}

		ln_createAndAddSimple(&_outfile, "@echo off");
		if (flags & EX_RDCONV) {
			wsprintf(szTemp,"%s < %s > %s", cmdline, infile, outfile);
		} else if (flags & EX_RDIN) {
			wsprintf(szTemp,"%s > %s", cmdline, outfile);
		} else if (flags & EX_RDOUT) {
			wsprintf(szTemp,"%s < %s", cmdline, infile);
		} else {
			wsprintf(szTemp,"%s", cmdline);
		}
		ln_createAndAddSimple(&_outfile, szTemp);
		string_concatPathAndFilename(szRunBat, _datadir, "RUN.BAT");
		ft_writeFileAndClose(&_outfile, szRunBat, 0);

		pszPif = file_searchFileInPKSEditLocation(
			(flags & EX_WAIT) ? "PKSRUNW.PIF" : "PKSRUN.PIF");
		if (!pszPif) {
			error_displayAlertDialog("PSZRUN(W).PIF not found");
			return 0;
		}
		wsprintf(szTemp,"%s /e:1024 /c %s", pszPif, szRunBat);
		cmdline = szTemp;
	}

	ret = WinExec(cmdline, show);
	if (ret < 32) {
		exec_error(cmdline,ret);
		return 0;
	}
	if (flags & EX_RDCONV) {
		undo_startModification(ft_getCurrentDocument());
		bUInited = TRUE;
		EdBlockDelete(1);
	}

#if !defined(WIN32)
	if (!(hTask = GetTaskHandle((HINSTANCE) ret))) {
		error_displayAlertDialog("invalid hInst %d", ret);
	}

	if (hTask) {
		LPFNNOTIFYCALLBACK 	lpNotifyProc;

		lpNotifyProc = (LPFNNOTIFYCALLBACK) MakeProcInstance((FARPROC)
				ExecWaitNotify, hInst);
		NotifyRegister(NULL, lpNotifyProc, NF_NORMAL);
		bTaskFinished = FALSE;
		if (flags & (EX_RDIN|EX_WAIT)) {
			int savePlaying = _playing;
			_playing = 0;
			progress_startMonitor(IDS_ABRTCMDCOMPLETE);
			while (1) {
				if (progress_cancelMonitor(FALSE) || bTaskFinished) {
					break;
				}
			}
			progress_closeMonitor(0);
			_playing = savePlaying;
		}
		NotifyUnRegister(NULL);
		FreeProcInstance(lpNotifyProc);
	}
#endif

	if (errfile && errfile[0]) {
		xref_openSearchList(errfile, 1);
	} else if (ft_getCurrentDocument() && (flags & EX_RDIN)) {
		if (!bUInited) {
			undo_startModification(ft_getCurrentDocument());
		}
		bl_insertTextBlockFromFile(outfile);
	}

	return 1;
}


