
#ifndef _EDIERROR_H
#define	_EDIERROR_H

#include "edtypes.h"

extern void ed_error(int nId, ...);
extern int EdOptionToggle(long option);

/*------------------------------------------------------------
 * ProgressMonitorCancel()
 * optional force redraw to reflect changes in the current
 * top window due to a workin progress
 */
extern int ProgressMonitorCancel(BOOL bRedraw);

/*------------------------------------------------------------
 * ProgressMonitorClose()
 */
extern void ProgressMonitorClose(int always);

/*------------------------------------------------------------
 * ProgressMonitorStart()
 */
extern void ProgressMonitorStart(unsigned int ids);

#define	E_SERIOUSC		'!'

#define	OPT_WMODE			(2L<<16)
#define	OPT_DMODE			(1L<<16)
#define	OPT_GLOBSET		(0L)
#define  OPT_RECORD		(-1L<<16L)


/*----- display modes --------------*/

#define	SHOWCONTROL		0x1
#define	SHOWSTATUS		0x2
#define	SHOWHEX			0x4
#define	SHOWRULER			0x8
#define	SHOWATTR			0x10
#define	SHOWOEM			0x20
#define	SHOWHIDELINES		0x40
#define	SHOWFIXEDWI		0x80
#define	SHOWHIDEHSLIDER	0x100
#define	SHOWHIDEVSLIDER	0x200
#define	SHOWSTATUSBOT		0x400

#define	PLAINCONTROL(mode)	(((mode) & SHOWCONTROL) && !((mode) & SHOWRULER))

/*----- working modes --------------*/

#define	WM_INSERT		0x1
#define	WM_SHOWMATCH	0x2
#define	WM_ABBREV		0x4
#define	WM_AUTOWRAP	0x8
#define	WM_AUTOINDENT	0x10
#define	O_RDONLY		0x20
#define	O_FIXEDWI		0x40
#define	BLK_LINES		0x80
#define	WM_AUTOFORMAT	0x100
#define	WM_BRINDENT	0x200
#define	BLK_COLOMN	0x400
#define	WM_OEMMODE	0x800
#define	O_CRYPTED		0x1000

/*----- ... --------------*/

# ifndef RE_DOREX
#define	RE_DOREX			0x1		/* use .{}*?+ and all the stuff */
#define	RE_IGNCASE		0x2		/* ignore case */
#define	RE_SHELLWILD		0x4		/* wildcards are: *?[] */
# endif
#define	O_WRAPSCAN 		0x8		/* wrap scan on search */

/* Substitution dialog */

#define	OREP_INQ			0x10		/* substitute query */
#define	OREP_MARKED		0x20		/* only in marked lines */

#define	REP_REPLACE		1		/* replace */
#define	REP_MARK			2		/* mark lines with occurence */
#define	REP_COUNT			3		/* count occurrences */
#define	REP_FIND			4		/* search only */

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
#define	MOT_FILE			4
#define	MOT_SPACE			5
#define	MOT_CENTER		5
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

/* Flags for marked line delta */
#define	MLN_DELETE		0
#define	MLN_JOIN			1
#define	MLN_TOGGLE		2
#define	MLN_FINDSOFT		3
#define	MLN_MAKESOFT		4
#define	MLN_MAKEHARD		5
#define	MLN_CUT			6

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
#define	PASTE_XUNDO		0x800
#define	PASTE_ICON		0x1000

/* Flags for Cut&EdBlockPaste */
#define	CUT_QUERYID		0x100
#define	CUT_APPND			0x200
#define	CUT_USEPP			0x400

/* Flags for EdMacroRecorded */
#define	MAC_AUTO			-1
#define	MAC_LASTREC		-2

/* Flags for formatblk */
#define	FMT_ADJLEFT		0
#define	FMT_ADJRIGHT		1
#define	FMT_ADJCENTER		2
#define	FMT_ADJBLOCK		3
#define	FMT_TYPEMASK		(0|1|2|3)
#define	FMT_MARKBLK		0x4
#define	FMT_PRGRPH		0x8
#define	FMT_TXTBLK		0x10
#define	FMT_WPFORMAT		0x20
#define	FMT_INDENT		0x40
#define	FMT_DIALOG		0x80
#define	FMT_STOPIFDONE		0x100

/* Flags for mouse mark textblock */
#define	MARK_RUBBER		0
#define	MARK_DRAG			1
#define	MARK_CONT			2

/* flags for mark block (EdBlockMark) */
#define	MARK_START		3
#define	MARK_END			4
#define	MARK_ALL			5
#define	MARK_COLUMN		0x80		/* for internal use only */
#define	MARK_RECALCULATE	0x100	/* recalculate start/end relationship */

/* valid working range types */
#define	RNG_INVALID		-1
#define	RNG_ONCE			0
#define	RNG_LINE			1
#define	RNG_CHAPTER		2
#define	RNG_BLOCK			3
#define	RNG_TOCURS		4
#define	RNG_FROMCURS		5
#define	RNG_GLOBAL		6
#define	RNG_FREE			7

/* goto textmark (lastpos) */
#define	TM_LASTSEARCH	0
#define	TM_LASTINSERT	1

#define	SO_SKIPSEPS	 	0x1	/* skip multiple Token Seperators */
#define	SO_CLUSTERLINES 	0x2	/* sort records rather than lines */
#define	SO_REVERSE	 	0x4  /* reverse the order of sorting */
#define	SO_NOSELECT	 	0x8  /* sort all lines */
#define	SO_TOKENIZE	 	0x10	/* at least one token key field */

/* printing options */
#define	PRTO_LINES		0x1		/* print line # */
#define	PRTO_PAUSE		0x2		/* pause between pages */
#define	PRTO_SWAP			0x8		/* swap h+f on alternate pages */
#define	PRTO_ULHEA		0x10		/* underline header */
#define	PRTO_ULFOO		0x20		/* limitline footer */
#define	PRTO_SWAPMARG		0x40		/* swap margins on alternate pages */
#define	PRTO_WRAP			0x80		/* wrap long lines */
#define	PRTO_EVEN			0x100	/* print even pages */
#define	PRTO_ODD 			0x200	/* print odd pages */
#define	PRTO_HEADERS		0x400	/* print header and footer */
#define	PRTO_KEEPS		0x800	/* Keeps.... */

#define	LIST_MACROS		0x1
#define	LIST_KEYS			0x2
#define	LIST_MICE			0x4
#define	LIST_MENUS		0x8

#define	NULLP			(void *)0

#endif 	/* _EDIERROR_H */

