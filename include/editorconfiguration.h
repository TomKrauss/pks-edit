/*
 * editorconfiguration.h
 *
 * PROJEKT: PKS-EDIT for Windows 95
 *
 * Configuration of global options of PKS edit. These are for now
 * saved in a file PKSEDIT.INI by default located in the PKS_SYS folder.
 *
 * 										created      :
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */

#ifndef EDITORCONFIGURATION_H

#include "pathname.h"

#define member_size(type, member) sizeof(((type *)0)->member)

 /*----- Option Flags assignable to EDITOR_CONFIGURATION.options --------------*/

 /* if set, PKS Edit will bring up a confirmation message boxes to confirm saving of files etc... */
#define	WARNINGS		0x1
#define	E_BELL 			0x2
#define	E_FLASH 		0x4		 	/* flash, rather ring the bell */
#define	O_LOCKFILES		0x8			/* lock opened files */

#define	O_READPIC		0x10
#define	O_SAVE_SETTINGS_ON_EXIT	0x20
#define	O_SAVESEQ		0x40
#define	O_GARBAGE_AS	0x80

#define	AUTOWRITE		0x100
#define	O_FORMFOLLOW	0x200
#define	O_MDISCROLL		0x400
#define	O_MESSAGES		0x800		/* message window */

#define	UNDOENABLED		0x2000
#define	O_CUTBUFEQCLIP	0x4000		/* clipboard == cut buffer */
#define	O_HIDE_BLOCK_ON_CARET_MOVE	0x1000		/* hide block marks on caret movement */

 /*----- Option Flags assignable to EDITOR_CONFIGURATION.layoutoptions --------------*/
/* layout options: fkeys, optionbar, statusline */
#define	OL_OPTIONBAR		0x1			/* option bar below function keys */
#define OL_FKEYS			0x2
#define	OL_SHOWSTATUS		0x4			/* show status line */
#define	OL_TOOLBAR			0x8			/* show tool bar */


typedef struct {
	int options;
	int layoutoptions;
	// the number of supported undo steps.
	int nundo;
	// the number of minutes after which an autosave is performed.
	int	asminutes;
	// the output path for temporary files created.
	char pksEditTempPath[EDMAXPATHLEN];
	// The include path,
	char includePath[EDMAXPATHLEN];
	void (*autosaveOnExit)();
} EDITOR_CONFIGURATION;

/**
 * Returns a pointer to the current editor configuration. If not yet initialized it will be initialized on the fly.
 */
extern EDITOR_CONFIGURATION* GetConfiguration();

/*------------------------------------------------------------
 * prof_save()
 * Save the configuration. If interactive is passed, then open
 * a file selector to pick the pksedit.ini file to which the config
 * is saved.
 */
extern int prof_save(EDITOR_CONFIGURATION* configuration, int interactive);

/*--------------------------------------------------------------------------
 * prof_saveaspath()
 * Save the temp path of PKS editor to the pksedit.ini file.
 */
extern void prof_saveaspath(EDITOR_CONFIGURATION* configuration);

#define EDITORCONFIGURATION_H
#endif