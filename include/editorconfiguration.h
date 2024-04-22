/*
 * editorconfiguration.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * Configuration of global options of PKS edit. These are for now
 * saved in a file pkseditini.json by default located in the PKS_SYS folder.
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

#define DEFAULT_CONFIG_FILE_NAME			"pkseditini.json"

/* Option Flags assignable to EDITOR_CONFIGURATION.options */
typedef enum {
	// if set, PKS Edit will bring up a confirmation message boxes to confirm saving of files etc...
	// Auto-save files to a temporary directory.
	O_AUTOSAVE_TO_TEMP = 0x1,
	// Play a warning sound on errors
	O_ERROR_TONE = 0x2,
	// flash window, rather playing an error sound
	O_ERROR_FLASH_WINDOW  = 0x4,
	// lock opened files 
	O_LOCKFILES = 0x8,
	// Restore previously opened files
	O_AUTO_OPEN_HISTORY = 0x10,
	// Save the PKS Edit settings on exit
	O_SAVE_SETTINGS_ON_EXIT = 0x20,
	// Automatically save all macros recorded / compiled when exiting
	O_SAVE_MACROS_ON_EXIT = 0x40,
	// Unlink temporary autosave files if not needed any more
	O_DELETE_AUTOSAVE_FILES = 0x80,
	// Automatically save changed files when closing editor / exiting PKS-Edit.
	O_AUTOSAVE_FILES_ON_EXIT = 0x100,
	// whether dialogs are opened close to the mouse.
	O_FORMFOLLOW = 0x200,
	O_CREATE_BACKUP_IN_TEMP_PATH= 0x400,
	// display error messages in a popup window / snackbar
	O_SHOW_MESSAGES_IN_SNACKBAR = 0x800,
	// hide block marks on caret movement
	O_HIDE_BLOCK_ON_CARET_MOVE = 0x1000,
	// undo enabled
	O_UNDOENABLED = 0x2000,
	// save and restore named clipboards when 
	O_SAVE_CLIPBOARDS_ON_EXIT = 0x4000,
	// whether a history of the clipboard contents is automatically maintained
	O_SAVE_CLIPBOARD_HISTORY = 0x8000,
	// whether we should enforce to re-use the single running instance of PKS-Edit
	O_REUSE_APPLICATION_INSTANCE = 0x10000,
	// whether we should silently reload files changed externally
	O_SILENTLY_RELOAD_CHANGED_FILES = 0x20000
} CONFIGURATION_FLAG;

/* Editor configuration layout options: fkeys, optionbar, statusline */
typedef enum {
	// option bar below function keys
	OL_OPTIONBAR = 0x1,
	// show function keyboard
	OL_FKEYS = 0x2,
	// show status line
	OL_SHOWSTATUS = 0x4,
	// show tool bar
	OL_TOOLBAR = 0x8,
	// show compact editor tabs (filename only - not full path)
	OL_COMPACT_TABS = 0x10
} CONFIGURATION_LAYOUT_FLAG;

typedef enum { ICS_SMALL, ICS_MEDIUM, ICS_BIG, ICS_LARGE } ICONSIZE;

typedef struct tagCOMPILER_OUTPUT_PATTERN {
	struct tagCOMPILER_OUTPUT_PATTERN* cop_next;
	char cop_name[32];
	char cop_pattern[200];
	int  cop_filenameCapture;
	int  cop_lineNumberCapture;
	int  cop_commentCapture;
} COMPILER_OUTPUT_PATTERN;

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
	char searchEngine[32];						// selected search engine for searching a string on the internet. 
	char soundName[32];							// sound name for playing sounds.
	COMPILER_OUTPUT_PATTERN* outputPatterns;	// Pre-defined compiler output patterns
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

/*
 * Returns the command to be used to search for something on the internet
 * with the currently configured search engine.
 */
extern char* config_getInternetSearchCommand();

/*--------------------------------------------------------------------------
 * prof_setinifile()
 */
void prof_setinifile(char* fn);

/*--------------------------------------------------------------------------
 * config_tempPathIsDefault()
 * Answer true, if the configured temp file path is the default path.
 */
extern int config_tempPathIsDefault();

/*
 * Read the configuration file.
 */
extern int config_read(const char* pszfilename);

/*
 * Save the current configuration to the specified config file.
 */
extern int config_save(const char* pszFilename);

/*
 * Destroy all configuration to cleanup.
 */
extern void config_destroy();

/*------------------------------------------------------------
 * config_saveConfiguration()
 * Save the configuration. If bInteractive is true, open
 * a file selector to pick the pkseditini.json file to which the config
 * is saved. If bInteractive is false, use the default file name
 * from which the configuration had been loaded.
 */
extern long long config_saveConfiguration(int bInteractive);

/*------------------------------------------------------------
 * prof_getstdopt()
 */
extern int prof_getstdopt(void);

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
extern long long fkey_keyModifierStateChanged();

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