/*
 * history.h
 *
 * history in PKSEDIT
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 03.04.1991
 *									
 */

# ifndef EDHIST_H

#define	MAXHIST 	32

typedef enum {
	SEARCH_PATTERNS = 0,
	SEARCH_AND_REPLACE = 1,
	OPEN_FILES = 2,
	FILE_PATTERNS = 3,
	PATHES = 4
} HISTORY_TYPE;

extern void hist_saveString(HISTORY_TYPE type, char *string);
extern void hist_fillComboBox(HWND hDlg, WORD nItem, HISTORY_TYPE type);
extern void hist_updateMenu(HISTORY_TYPE type);
extern char *hist_getString(HISTORY_TYPE type, int nItem);
extern void hist_readLine(LINE* lp);
/*---------------------------------
 * save history()
 * save all history entries to the file passed as an argument.
 */
extern void hist_saveAllEntriesTo(FTABLE* fp);

#define	EDHIST_H
# endif	/* EDHIST_H */



