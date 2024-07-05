
#ifndef _EDIERROR_H
#define	_EDIERROR_H

#include "edtypes.h"

/*
 * Display an error in a "non-intrusive way" (status line etc...).
 */
extern void error_showError(char* s, va_list ap);

/*
 * Show an error message with a string and optional parameters.
 * If enforceNonBlocking is TRUE, errors are displayed in a non blocking way.
 */
extern void error_showMessage(char* s, ...);

extern void error_showErrorById(int nId, ...);

/*
 * Can be used to temporarily disable the display of status messages in the status bar or the toast window.
 * Useful and to be called from within macros to make execution of macros less verbose.
 */
extern long long error_setShowMessages(BOOL aFlag);

/*
 * Defines the type of flag to apply / get (from an editor) using the EditorChangeOption API.
 */
typedef enum {
	// Change the editor input mode - 2nd parameter is one of the WM_... flags.
	OP_EDIT_MODE,
	// Change the editor display mode - 2nd parameter is one of the SHOW_... flags.
	OP_DISPLAY_MODE,
	// Change the macro recording mode
	OP_MACRO,
	// Change an arbitrary editor setting
	OP_OPTIONS,
	// Change a "file" type flag on the editor passed
	OP_FILEFLAG 
} OP_FLAGTYPE;

#define	OPT_WMODE			(((long)OP_EDIT_MODE)<<16)
#define	OPT_DMODE			(((long)OP_DISPLAY_MODE)<<16)
#define	OPT_GLOBSET			(((long)OP_OPTIONS)<<16L)
#define OPT_RECORD			(((long)OP_MACRO)<<16L)

/*------------------------------------------------------------
 * progress_cancelMonitor()
 * optional force redraw to reflect changes in the current
 * top window due to a workin progress
 */
extern int progress_cancelMonitor(BOOL bRedraw);

/*------------------------------------------------------------
 * progress_closeMonitor()
 */
extern void progress_closeMonitor(int always);

/*------------------------------------------------------------
 * progress_startMonitor()
 */
extern void progress_startMonitor(unsigned int ids, int nMillisToPopup);

/*------------------------------------------------------------
 * progress_showMonitorMessage()
 */
extern void progress_showMonitorMessage(const char* message);

/*------------------------------------------------------------
 * progress_stepIndicator()
 * Animate the actual progress bar displayed in the progress dialog.
 */
extern void progress_stepIndicator();

/*--------------------------------------------------------------------------
 * err_noDiskSpace()
 */
extern void err_noDiskSpace(void);

/*--------------------------------------------------------------------------
 * err_writeErrorOcurred()
 */
extern void err_writeErrorOcurred(void);

/*--------------------------------------------------------------------------
 * err_openFailed()
 */
extern void err_openFailed(char* fname);

/*---------------------------------
 * ln_errorLineTooLong()
 * Display an error about the fact a line would become too long.
 *---------------------------------*/
extern void ln_errorLineTooLong(void);

#define	E_SERIOUSC		'!'


/* Substitution dialog */

/* Fkeys */
#define	NORMFKEY			0
#define	CNTLFKEY			1
#define	ALTFKEY			2
#define	SHIFTFKEY			3

#define	RSCMENU			0
#define	RSCHELP			1

/* Errno Codes */
#define	EWRPROTECT		-13
#define	ENOFILE			-33
#define	ENODIR			-34
#define	EACCESS			-36
#define	ENOAPPL			-128

/* Window Register Params */
#define	WIN_FULL			-1
#define	WIN_TILE			-2
#define	WIN_CASCADE		-3
#define	WIN_ICONIZED		-4
#define	WIN_ICONARRANGE	-5
#define	WIN_DESKICON		-6
#define  WIN_HOR			-7
#define  WIN_PAIRS		     -8

/* Select Window specials */
#define	SEL_CYCLE			-100

#define	FMT_TYPEMASK		(0|1|2|3)

/* Flags for operations changing the selection in PKS Edit (EdSyncSelectionWithCaret) */
typedef enum {
	MARK_START =	 3,
	MARK_END =		 4,
	MARK_ALL =		 5,
	/* for internal use only */
	MARK_COLUMN =	 0x80		,
	/* recalculate start/end relationship */
	MARK_RECALCULATE = 0x100	,
	/* do not hide the selection, if start mark is placed before an end mark - used internally only */
	MARK_NO_HIDE = 0x200
} MARK_SELECTION_OPTION;

#define	NULLP			(void *)0

#endif 	/* _EDIERROR_H */

