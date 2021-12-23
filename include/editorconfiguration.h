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
#define	O_AUTOSAVE_TO_TEMP			0x1			// Auto-save files to a temporary directory.
#define	O_ERROR_TONE				0x2			// Play a warning sound on errors
#define	O_ERROR_FLASH_WINDOW 		0x4		 	// flash window, rather playing an error sound
#define	O_LOCKFILES					0x8			// lock opened files 

#define	O_AUTO_OPEN_HISTORY			0x10		// Restore previously opened files
#define	O_SAVE_SETTINGS_ON_EXIT		0x20		// Save the PKS Edit settings on exit
#define	O_SAVE_MACROS_ON_EXIT		0x40		// Automatically save all macros recorded / compiled when exiting
#define	O_DELETE_AUTOSAVE_FILES		0x80		// Unlink temporary autosave files if not needed any more

#define	O_AUTOSAVE_FILES_ON_EXIT	0x100		// Automatically save changed files when closing editor / exiting PKS-Edit.
#define	O_FORMFOLLOW				0x200		// whether dialogs are opened close to the mouse.
#define	O_CREATE_BACKUP_IN_TEMP_PATH 0x400
#define	O_SHOW_MESSAGES_IN_SNACKBAR	 0x800		// display error messages in a popup window / snackbar

#define	O_HIDE_BLOCK_ON_CARET_MOVE	0x1000		// hide block marks on caret movement
#define	O_UNDOENABLED				0x2000		// undo enabled
#define	O_SAVE_CLIPBOARDS_ON_EXIT	0x4000		// save and restore named clipboards when 
#define O_SAVE_CLIPBOARD_HISTORY	0x8000		// whether a history of the clipboard contents is automatically maintained

 /*----- Option Flags assignable to EDITOR_CONFIGURATION.layoutoptions --------------*/
/* layout options: fkeys, optionbar, statusline */
#define	OL_OPTIONBAR		0x1			// option bar below function keys
#define OL_FKEYS			0x2			// show function keyboard
#define	OL_SHOWSTATUS		0x4			// show status line
#define	OL_TOOLBAR			0x8			// show tool bar
#define OL_COMPACT_TABS		0x10		// show compact editor tabs (filename only - not full path)

typedef enum { ICS_SMALL, ICS_MEDIUM, ICS_BIG, ICS_LARGE } ICONSIZE;

typedef struct tagEDITOR_CONFIGURATION {
	int options;
	int layoutoptions;
	ICONSIZE iconSize;
	// the number of supported undo steps.
	int nundo;
	// the number of minutes after which an autosave is performed. If 0 - autosave is disabled.
	int	autosaveSeconds;
	// the maximum number of windows opened. If additional windows are opened, windows of unmodified buffers are recycled
	int maximumNumberOfOpenWindows;
	// the output path for temporary files created.
	char pksEditTempPath[EDMAXPATHLEN];
	// The include path,
	char includePath[EDMAXPATHLEN];
	char themeName[32];
	char language[32];
	char defaultFontFace[32];
	void (*autosaveOnExit)();
} EDITOR_CONFIGURATION;

/**
 * Returns a pointer to the current editor configuration. If not yet initialized it will be initialized on the fly.
 */
extern EDITOR_CONFIGURATION* GetConfiguration();

/*
 * Returns the temp path into which PKS edit saves autosave and optionally backup files.
 */
extern char* config_getPKSEditTempPath();

/*--------------------------------------------------------------------------
 * prof_setinifile()
 */
void prof_setinifile(char* fn);


/*------------------------------------------------------------
 * prof_getws()
 */
extern int prof_getWindowSettings(char* string, WINDOWPLACEMENT* wsp);

/*------------------------------------------------------------
 * prof_getwinstate()
 */
extern int prof_getwinstate(char* wname, int nr, WINDOWPLACEMENT* wsp);

/*------------------------------------------------------------
 * prof_getPksStandardString()
 * Fetches a string from the standard section of the PKS profile.
 */
extern int prof_getPksStandardString(const char* ident, char* string, int maxlen);

/*------------------------------------------------------------
 * prof_savestring()
 * Saves a string in the standard section of the PKS profile.
 */
extern int prof_savePksStandardString(const char* ident, char* string);

/*------------------------------------------------------------
 * prof_printws()
 */
extern void prof_printws(char* buf, WINDOWPLACEMENT* wsp);

/*------------------------------------------------------------
 * prof_savestring()
 */
extern int prof_savestring(const char* grp, const char* ident, char* string);

/*--------------------------------------------------------------------------
 * config_saveTempPath()
 * Save the temp path of PKS editor to the pksedit.ini file.
 */
extern void config_saveTempPath();

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
extern int prof_getPksProfileString(const char* pGroup, const char* ident, char* string, int maxlen);

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