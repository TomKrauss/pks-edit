/*
 * MaintainHistory.c
 *
 * history for PKSEDIT
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

#include <windows.h>
#include <string.h>
#include "documentmodel.h"
#include "history.h"
#include "stringutil.h"

#define MAX_HISTORY_FILES 10

extern HMENU his_getHistoryMenu(int *pnPosition, int *piCmd);

typedef struct history {
	char* s[MAXHIST];
	int  where;
} HISTORY;

static HISTORY _histories[PATHES+1];

static struct histdes {
	char* keyword;
	struct history* hist;
} _histsavings[] = {
	"[find]",		&_histories[SEARCH_PATTERNS],
	"[filePatterns]",& _histories[FILE_PATTERNS],
	"[replace]",	&_histories[SEARCH_AND_REPLACE],
	"[open]",		&_histories[OPEN_FILES],
	"[pathes]",		&_histories[PATHES],
	0
};

void win_changeMenuItem(HMENU hMenu, int nPosition, int nCmd, WORD wFlags,
	LPCSTR lpszItem);

/*---------------------------------*/
/* hist_saveString()					*/
/*---------------------------------*/
static void hist_addEntry(HISTORY* h, char* string) {
	char* s;
	int  i;
	if (*string == 0) {
		return;
	}
	/*
	 * first: if present in history, rearrange only
	 */
	for (i = h->where; ; ) {
		if ((s = h->s[i]) != 0 && strcmp(s, string) == 0) {
			if (i == h->where)
				return;
			/* free(slot) and insert actual string in position "where" */
			h->s[i] = 0;
			break;
		}
		s = 0;
		if (--i < 0)
			i = MAXHIST - 1;
		if (i == h->where)
			break;
	}

	/*
	 * not present: make new
	 */
	if (!s && (s = _strdup(string)) == 0)
		return;

	/*
	 * insert and update new position
	 */
	i = (h->where >= MAXHIST - 1) ? 0 : h->where + 1;

	if (h->s[i] != 0)
		free(h->s[i]);
	h->s[i] = s;
	h->where = i;
}


/*---------------------------------*/
/* hist_saveString()					*/
/*---------------------------------*/
EXPORT void hist_saveString(HISTORY_TYPE type, char *string) {
	HISTORY* h = &_histories[type];

	char* s;
	int  i;
	if (*string == 0) {
		return;
	}
	/*
	 * first: if present in history, rearrange only
	 */	
	for (i = h->where; ; ) {
		if ((s = h->s[i]) != 0 && strcmp(s,string) == 0) {
			if (i == h->where)
				return;
			/* free(slot) and insert actual string in position "where" */
			h->s[i] = 0;
			break;
		}
		s = 0;
		if (--i < 0)
			i = MAXHIST-1;
		if (i == h->where)
			break;
	}

	/*
	 * not present: make new
	 */
	if (!s && (s = _strdup(string)) == 0)
		return;

	/*
	 * insert and update new position
	 */
	i = (h->where >= MAXHIST-1) ? 0 : h->where + 1;

 	if (h->s[i] != 0) 
		free(h->s[i]); 
	h->s[i]  = s;
	h->where = i;
}

/*------------------------------------------------------------
 * hist_combo()
 */
EXPORT void hist_fillComboBox(HWND hDlg, WORD nItem, HISTORY_TYPE type)
{	int  i;
	char *p,*q = 0;
	HISTORY* hp = &_histories[type];

	SendDlgItemMessage(hDlg, nItem, CB_RESETCONTENT, 0,0L);
	i = hp->where;
	do {
		if ((p = hp->s[i]) != 0) {
			if (!q)
				q = p;
			if (SendDlgItemMessage(hDlg, nItem, CB_ADDSTRING, 0, (LPARAM)p) < 0) {
				break;
			}
		}
		i = (i <= 0) ? MAXHIST-1 : i-1;
	} while (i != hp->where);
	if (q) {
		SetDlgItemText(hDlg, nItem, q);
	}
}

/*--------------------------------------------------------------------------
 * hist_getString()
 */
char *hist_getString(HISTORY_TYPE type, int nItem) {
	HISTORY* hp = &_histories[type];
	int		i;
	
	i = hp->where;
	do {
		if (hp->s[i] != 0) {
			if (nItem-- <= 0) {
				return hp->s[i];
			}
		}
		i = (i <= 0) ? MAXHIST-1 : i-1;
	} while (i != hp->where);
	return (char *)0;
}

/*--------------------------------------------------------------------------
 * hist_2menu()
 */
void hist_updateMenu(HISTORY_TYPE type) {
	HMENU 	hMenu;
	int		iCmd;
	int		nVisible;
	int 	nPosition;
	int		i;
	char	szTemp[512];
	char *	p;

	hMenu = his_getHistoryMenu(&nPosition, &iCmd);

	nVisible = MAX_HISTORY_FILES;

	for (i = 0; i < nVisible; i++) {
		if ((p = hist_getString(type, i)) == 0) {
			return;
		}
		wsprintf(szTemp, "&%d %s", i + 1, p);
		win_changeMenuItem(hMenu, nPosition, iCmd, 
			MF_BYPOSITION|MF_STRING, szTemp);
		nPosition++;
		iCmd++;
	}
}

/*---------------------------------*/
/* history saving()				*/
/*---------------------------------*/
static void hist_save(FTABLE *fp,struct history *hp)
{	int i;
	char buf[256];

	i = hp->where;
	do {
		i = (i >= MAXHIST-1) ? 0 : i+1;
		if (hp->s[i]) {
			wsprintf(buf,"!%s",hp->s[i]);
			ln_createAndAddSimple(fp,buf);
		}
	} while (i != hp->where);
}

/*---------------------------------
 * save history()
 * save all history entries to the file passed as an argument.
 */
EXPORT void hist_saveAllEntriesTo(FTABLE *fp)
{	struct histdes *hp = _histsavings;

	while(hp->keyword) {
		ln_createAndAddSimple(fp,hp->keyword);
		hist_save(fp,hp->hist);
		hp++;
	}
}

/*---------------------------------*/
/* hist_readLine()					*/
/*---------------------------------*/
EXPORT void hist_readLine(LINE *lp)
{	struct history *hp = 0;

	while (lp != 0 && lp->next != 0) {
		if (lp->len && lp->lbuf[0] == '[') {
			struct histdes *h = _histsavings;
			while(h->keyword) {
				if (strcmp(h->keyword,lp->lbuf) == 0) {
					hp = h->hist;
					break;
				}
				h++;
			}
		}
		else { 
			if (hp)
				hist_addEntry(hp,lp->lbuf+1);		/* +1 -> skip ! mark */
		}
		lp = lp->next;
	}
	hist_updateMenu(OPEN_FILES);
}

/*
 * Destroy all data structures related to the PKS Edit history. 
 */
void hist_destroy() {
	struct histdes* h = _histsavings;
	
	while (h->keyword) {
		if (h->hist) {
			char** pStrings = h->hist->s;
			for (int i = 0; i < MAXHIST; i++) {
				if (pStrings[i]) {
					free(pStrings[i]);
					pStrings[i] = NULL;
				}
			}
		}
		h++;
	}
}
