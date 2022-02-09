
#ifndef _EDIERROR_H
#define	_EDIERROR_H

#include "edtypes.h"

/*
 * Display an error in a "non-intrusive way" (status line etc...).
 */
extern void error_showError(char* s, va_list ap);

/*
 * Show an error message with a string and optional parameters.
 */
extern void error_showMessage(char* s, ...);

extern void error_showErrorById(int nId, ...);

/*
 * Can be used to temporarily disable the display of status messages in the status bar or the toast window.
 * Useful and to be called from within macros to make execution of macros less verbose.
 */
extern int error_setShowMessages(BOOL aFlag);

/**
 * 
 */
extern int EdOptionToggle(long option);

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
extern void progress_startMonitor(unsigned int ids);

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

/* Cursor Motion */
#define	MOT_SINGLE		1
#define	MOT_TOEND			2
#define	MOT_WORD			3
#define	MOT_PAGE			3
#define	MOT_SCREEN		4
#define	MOT_UNTILC		4
#define	MOT_PGRPH			4
#define	MOT_FILE			4           // move to begin / end of file
#define	MOT_SPACE			5           // move to next space
#define	MOT_CENTER		    5           // move to center of page
#define	MOT_XTNDBLOCK		0x100

/* Flags for EdFileClose */
#define	SAV_SAVE			0x1
#define	SAV_QUIT			0x2
#define	SAV_AS			0x4
#define	SAV_FORCED		0x8

/* Flags for EdFileEdit */
#define	OPEN_LINEAL		0x1
#define	OPEN_NOLINFN		0x20
#define	OPEN_DIRGIVEN		0x40
#define	OPEN_NOFN			0x80
#define	OPEN_HISTORY		0x100

/* Flags for printing */
#define	PRT_CURRWI		0x1
#define	PRT_FILE			0x2
#define	PRT_CURRBLK		0x3
#define	PRT_CLIP			0x4
#define	PRT_TRASH			0x5
#define	PRT_TYPES			(1|2|3|4|5)

/* Flags for return */
#define	RET_PLUS			0x1
#define	RET_MINUS			0x2
#define	RET_SOFT			0x4

/* Flags for Listops */
#define	LIST_START		0
#define	LIST_PREV			1
#define	LIST_CURR			2
#define	LIST_NEXT			3
#define	LIST_END			4
#define	LIST_USETOPWINDOW	0x100

/* Flags for align */
#define	AL_CPOS		0x1	/* alle vor Cursorposition ignorieren */
#define	AL_FIX			0x2	/* alle auf fest auf Cursorposition alignen */
#define	AL_END			0x4	/* align Ende Suchmuster */

/* Flags for EdBlockPaste */
#define	PASTE_QUERYID	0x100
#define	PASTE_CLIP		0x200
#define	PASTE_UNDO		0x400
#define	PASTE_ICON		0x1000

/* Flags for Cut&EdBlockPaste */
#define	CUT_QUERYID		0x100
#define	CUT_APPND		0x200
#define	CUT_USEPP		0x400
#define	CUT_DELETE		0x800

/* Flags for EdMacroRecorded */
#define	MAC_AUTO			-1
#define	MAC_LASTREC		-2

/* Flags for formatblk */
#define	FMT_TYPEMASK		(0|1|2|3)
#define	FMT_MARKBLK		0x4
#define	FMT_PRGRPH		0x8
#define	FMT_TXTBLK		0x10
#define	FMT_DIALOG		0x80
#define	FMT_STOPIFDONE		0x100

/* flags for mark block (EdSyncSelectionWithCaret) */
#define	MARK_START		3
#define	MARK_END			4
#define	MARK_ALL			5
#define	MARK_COLUMN		0x80		/* for internal use only */
#define	MARK_RECALCULATE	0x100	/* recalculate start/end relationship */
#define	MARK_NO_HIDE	0x200	/* do not hide the selection, if start mark is placed before an end mark - used internally only */

#define	SO_SKIPSEPS	 	0x1	/* skip multiple Token Seperators */
#define	SO_CLUSTERLINES 	0x2	/* sort records rather than lines */
#define	SO_REVERSE	 	0x4  /* reverse the order of sorting */
#define	SO_NOSELECT	 	0x8  /* sort all lines */
#define	SO_TOKENIZE	 	0x10	/* at least one token key field */


#define	LIST_MACROS		0x1
#define	LIST_KEYS			0x2
#define	LIST_MICE			0x4
#define	LIST_MENUS		0x8

#define	NULLP			(void *)0

#endif 	/* _EDIERROR_H */

