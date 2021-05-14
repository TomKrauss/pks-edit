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

struct history {
	char *s[MAXHIST];
	int  where;
};

extern struct history _findhist, _replhist, _openhist, _pathhist;

extern void hist_enq(struct history *h, char *string);
extern void hist_2combo(HWND hDlg, WORD nItem, char *firstitem, 
				    struct history *hist);
extern void hist_updatemenu(struct history *hist);
extern char *hist_getstring(struct history *hp, int nItem);
/*--------------------------------------------
* blfill(char *buf,int count,int fillbyte)
* Similar to memset, but return pointer to the end of
* the filled area.
*--------------------------------------------*/
extern unsigned char* blfill(unsigned char* buf, int count, unsigned char fillbyte);

#define	EDHIST_H
# endif	/* EDHIST_H */



