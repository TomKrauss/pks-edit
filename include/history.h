/*
 * history.h
 *
 * history in PKSEDIT
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 03.04.1991
 */

# ifndef EDHIST_H

#define	MAXHIST 	32

#define MAX_HISTORY_FILES 10

typedef enum {
	SEARCH_PATTERNS = 0,
	SEARCH_AND_REPLACE = 1,
	OPEN_FILES = 2,
	FILE_PATTERNS = 3,
	PATHES = 4,
	OPEN_IN_EDITOR = 5
} HISTORY_TYPE;

// Describes the layout of the mainframe - one docking area
typedef struct tagMAINFRAME_DOCK {
	char do_name[32];
	float do_x;
	float do_y;
	float do_w;
	float do_h;
} MAINFRAME_DOCK;

typedef struct tagSESSION_DATA {
	int sd_searchAndReplaceOptions;						// last selected options for search and replace
	int sd_screenCX;									// screen resolution in X of the point in time when the session data was saved
	int sd_screenCY;									// screen resolution in Y of the point in time when the session data was saved
	WINDOWPLACEMENT sd_mainWindowPlacement;				// window placement of main window
	MAINFRAME_DOCK sd_dock1;							// mainframe dock 1 placement
	MAINFRAME_DOCK sd_dock2;							// mainframe dock 2 placement
	MAINFRAME_DOCK sd_dock3;							// mainframe dock 3 placement
	void* sd_histories[OPEN_IN_EDITOR+1];				// the various histories
} SESSION_DATA;

extern void hist_saveString(HISTORY_TYPE type, const char *string);
extern void hist_fillComboBox(HWND hDlg, WORD nItem, HISTORY_TYPE type);
extern char *hist_getString(HISTORY_TYPE type, int nItem);

/*---------------------------------
 * hist_saveSession()
 * Save the current session - all history entries to the session file 'pszFilename'
 */
extern void hist_saveSession(char* pszFilename);

/*---------------------------------
 * hist_readSession()
 * Read the current session - all history entries from the session file 'pszFilename'
 */
extern int hist_readSession(const char* pszFilename);

/*
 * Destroy all data structures related to the PKS Edit history.
 */
extern void hist_destroy();

/*
 * Returns abritrary additional session data from the last session.
 */
extern SESSION_DATA* hist_getSessionData();

/*
 * Save the placement of a mainframe docking slot.
 */
extern int hist_saveDockingPlacement(int aNumber, char* pszDockName, float x, float y, float w, float h);

/*
 * Read the placement of a mainframe docking slot.
 */
extern int hist_readDockingPlacement(int aNumber, char* pszDockName, float* x, float* y, float* w, float* h);

/*
 * hist_resetHistory(HISTORY_TYPE type)
 * Reset the specified history to be empty.
 */
extern void hist_resetHistory(HISTORY_TYPE type);

#define	EDHIST_H
# endif	/* EDHIST_H */



