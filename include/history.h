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
	PATHES = 4
} HISTORY_TYPE;

extern void hist_saveString(HISTORY_TYPE type, char *string);
extern void hist_fillComboBox(HWND hDlg, WORD nItem, HISTORY_TYPE type);
extern char *hist_getString(HISTORY_TYPE type, int nItem);
extern void hist_readLine(LINE* lp);
/*---------------------------------
 * save history()
 * save all history entries to the file passed as an argument.
 */
extern void hist_saveAllEntriesTo(FTABLE* fp);

/*
 * Destroy all data structures related to the PKS Edit history.
 */
extern void hist_destroy();

#define	EDHIST_H
# endif	/* EDHIST_H */



