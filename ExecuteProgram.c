/*
 * ExecuteProgram.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: executing programs
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include	<windows.h>
#if !defined(WIN32)
#include	<toolhelp.h>
#endif
#include	"pksedit.h"
#include	"edierror.h"
#include	"lineoperations.h"
#include	"winterf.h"

extern void ChangeDirectory(LPSTR pszPath);
extern void stepnofsel(char *fn, int cmpflg);
extern char *searchfile(char *s);
extern void strdcpy(char *dest,char *source,char *fname);
extern char *TmpDir(void);
extern void alert(LPSTR fmt, ...);
extern int ed_abort(BOOL bRedraw);
extern void abrt_start(WORD ids);
extern void abrt_close(int always);
extern void u_init(FTABLE *fp);

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
	ed_error(IDS_MSGEXECERROR,errcode,cmd);
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
	char *			tmp = TmpDir();
	char *			getenv(char *);
	char *			pszPif;
	BOOL				bUInited;

	if (!cmdline) {
		return 0;
	}

	bUInited = FALSE;

	if (newdir && newdir[0]) {
		ChangeDirectory(newdir);
	}

	show = (flags & EX_SYMBOL) ? SW_SHOWMINIMIZED : SW_SHOWNORMAL;

	if (flags & EX_DOS) {

		strdcpy(infile, tmp, "IN.___");
		strdcpy(outfile, tmp, "OUT.___");

		if (flags & EX_RDOUT) {
			if (!_EdBlockWrite(infile)) {
				return 0;
			}
		}

		if (errfile && errfile[0]) {
			flags |= EX_RDIN;
			lstrcpy(outfile, errfile);
		}

		putline(&_outfile, "@echo off");
		if (flags & EX_RDCONV) {
			wsprintf(szTemp,"%s < %s > %s", cmdline, infile, outfile);
		} else if (flags & EX_RDIN) {
			wsprintf(szTemp,"%s > %s", cmdline, outfile);
		} else if (flags & EX_RDOUT) {
			wsprintf(szTemp,"%s < %s", cmdline, infile);
		} else {
			wsprintf(szTemp,"%s", cmdline);
		}
		putline(&_outfile, szTemp);
		strdcpy(szRunBat, _datadir, "RUN.BAT");
		Writeandclose(&_outfile, szRunBat, 0);

		pszPif = searchfile(
			(flags & EX_WAIT) ? "PKSRUNW.PIF" : "PKSRUN.PIF");
		if (!pszPif) {
			alert("PSZRUN(W).PIF not found");
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
		u_init(_currfile);
		bUInited = TRUE;
		EdBlockDelete(1);
	}

#if !defined(WIN32)
	if (!(hTask = GetTaskHandle((HINSTANCE) ret))) {
		alert("invalid hInst %d", ret);
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
			abrt_start(IDS_ABRTCMDCOMPLETE);
			while (1) {
				if (ed_abort(FALSE) || bTaskFinished) {
					break;
				}
			}
			abrt_close(0);
			_playing = savePlaying;
		}
		NotifyUnRegister(NULL);
		FreeProcInstance(lpNotifyProc);
	}
#endif

	if (errfile && errfile[0]) {
		stepnofsel(errfile, 1);
	} else if (_currfile && (flags & EX_RDIN)) {
		if (!bUInited) {
			u_init(_currfile);
		}
		_EdBlockRead(outfile);
	}

	return 1;
}


