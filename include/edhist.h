/*
 * EDHIST.H
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
	PATHES = 3
} HISTORY_TYPE;

extern void hist_enq(HISTORY_TYPE type, char *string);
extern void hist_2combo(HWND hDlg, WORD nItem, char *firstitem, 
	HISTORY_TYPE type);
extern void hist_updatemenu(HISTORY_TYPE type);
extern char *hist_getstring(HISTORY_TYPE type, int nItem);

#define	EDHIST_H
# endif	/* EDHIST_H */



