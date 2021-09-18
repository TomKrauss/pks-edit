/*
 * editorconfiguration.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * Configuration of global options of PKS edit. These are for now
 * saved in a file PKSEDIT.INI by default located in the PKS_SYS folder.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
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
#define	O_HIDE_BLOCK_ON_CARET_MOVE	0x1000		/* hide block marks on caret movement */

 /*----- Option Flags assignable to EDITOR_CONFIGURATION.layoutoptions --------------*/
/* layout options: fkeys, optionbar, statusline */
#define	OL_OPTIONBAR		0x1			/* option bar below function keys */
#define OL_FKEYS			0x2
#define	OL_SHOWSTATUS		0x4			/* show status line */
#define	OL_TOOLBAR			0x8			/* show tool bar */


typedef struct tagEDITOR_CONFIGURATION {
	int options;
	int layoutoptions;
	// the number of supported undo steps.
	int nundo;
	// the number of minutes after which an autosave is performed.
	int	asminutes;
	// the maximum number of windows opened. If additional windows are opened, windows of unmodified buffers are recycled
	int maximumNumberOfOpenWindows;
	// the output path for temporary files created.
	char pksEditTempPath[EDMAXPATHLEN];
	// The include path,
	char includePath[EDMAXPATHLEN];
	char themeName[32];
	char language[32];
	void (*autosaveOnExit)();
} EDITOR_CONFIGURATION;

/**
 * Returns a pointer to the current editor configuration. If not yet initialized it will be initialized on the fly.
 */
extern EDITOR_CONFIGURATION* GetConfiguration();

/*--------------------------------------------------------------------------
 * prof_setinifile()
 */
void prof_setinifile(char* fn);

/*--------------------------------------------------------------------------
 * prof_adjustpoint()
 */
extern void prof_adjustpoint(PPOINT pPoint);

/*------------------------------------------------------------
 * prof_getws()
 */
extern int prof_getWindowSettings(char* string, WINDOWPLACEMENT* wsp);

/*------------------------------------------------------------
 * prof_getwinstate()
 */
extern int prof_getwinstate(char* wname, int nr, WINDOWPLACEMENT* wsp);

/*------------------------------------------------------------
 * prof_printws()
 */
extern void prof_printws(char* buf, WINDOWPLACEMENT* wsp);

/*------------------------------------------------------------
 * prof_savestring()
 */
extern int prof_savestring(char* grp, char* ident, char* string);

/*--------------------------------------------------------------------------
 * prof_killentry()
 */
extern int prof_killentry(char* grp, char* ident);

/*------------------------------------------------------------
 * prof_savewinstate()
 */
extern int prof_savewinstate(char* wname, int nr, WINDOWPLACEMENT* wsp);

/*
 * Save the placement of a mainframe docking slot.
 */
extern int prof_saveDockingPlacement(int aNumber, char* pszDockName, float x, float y, float w, float h);

/*
 * Read the placement of a mainframe docking slot.
 */
extern int prof_readDockingPlacement(int aNumber, char* pszDockName, float* x, float* y, float* w, float* h);

/*------------------------------------------------------------
 * prof_savelong()
 */
extern int prof_savelong(char* grp, char* ident, LONG val);

/*------------------------------------------------------------
 * prof_getlong()
 */
extern LONG prof_getlong(char* grp, char* ident);

/*------------------------------------------------------------
 * prof_getstdopt()
 */
extern int prof_getstdopt(void);

/*--------------------------------------------------------------------------
 * prof_killsections()
 */
extern void prof_killsections(LPSTR pszFn, LPSTR pszSection);

/*--------------------------------------------------------------------------
 * prof_saveaspath()
 * Save the temp path of PKS editor to the pksedit.ini file.
 */
extern void prof_saveaspath(EDITOR_CONFIGURATION* configuration);

/*------------------------------------------------------------
 * prof_save()
 * Save the configuration. If interactive is passed, then open
 * a file selector to pick the pksedit.ini file to which the config
 * is saved.
 */
extern int prof_save(EDITOR_CONFIGURATION* configuration, int interactive);

/*--------------------------------------------------------------------------
 * prof_enum()
 */
extern int prof_enum(LPSTR grp, intptr_t (*lpfnEnum)(LPSTR, LONG), LONG lParam);

/*--------------------------------------------------------------------------
 * prof_llinsert()
 */
extern void* prof_llinsert(void* Head, int size, char* group, char* item, char** idata);

/*------------------------------------------------------------
 * prof_getPksProfileString()
 * Fetches a string from the PKS profile ini file.
 */
extern int prof_getPksProfileString(char* pGroup, char* ident, char* string, int maxlen);

/*--------------------------------------------------------------------------
 * op_updateall()
 * The options have changed. Update all UI elements depending on an option.
 */
extern void op_updateall(void);

/*--------------------------------------------------------------------------
 * op_checktoggles()
 * One of the "option" widgets was selected.
 */
extern int op_onOptionWidgetSelected(int toggle);

/*---------------------------------
 * fkey_keyModifierStateChanged()
 * Update the PKS edit FKEYs to display the next
 * group of FKEYS (for alternate modifiers).
 *---------------------------------*/
extern int fkey_keyModifierStateChanged();

/*--------------------------------------------------------------------------
 * fkey_register()
 * Register the window class for the PKS Edit function keys widgets.
 */
extern int fkey_register(void);

/*------------------------------------------------------------
 * fkey_getKeyboardSize()
 * Get the size of the FKEY keyboard of PKS Edit.
 */
extern int fkey_getKeyboardSize(WORD* w, WORD* h);

/*--------------------------------------------------------------------------
 * fkey_initKeyboardWidget()
 * Initialize the FKEYs keyboard widget.
 */
extern int fkey_initKeyboardWidget(HWND hwndPapa);

/*----------------------------
 * fkey_updateTextOfFunctionKeys()
 * Update the text on the FKEYS keyboard.
 *----------------------------*/
extern void fkey_updateTextOfFunctionKeys(int state);

/*--------------------------------------------------------------------------
 * tb_initToolbar()
 * Initialize the PKS Edit top level rebar.
 */
extern HWND tb_initRebar(HWND hwndDaddy);

#define EDITORCONFIGURATION_H
#endif