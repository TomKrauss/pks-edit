/*
 * Compare.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: some compare functions
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */
 
#include <ctype.h>
#include "edtypes.h"
 
extern unsigned char 	_l2uset[256];
extern unsigned char	_u2lset[256];

/*------------------------------------------------------------
 * cmpfold()
 */
EXPORT int cmpfold(unsigned char *s1, int l1,unsigned char *s2, int l2)
{	int      len;

	if (l1 > l2)
		len = l2;
	else len = l1;

	while(len > 0) {
		if (_l2uset[*s1++] != _l2uset[*s2++]) {
			l1 = _l2uset[s1[-1]];
			l2 = _l2uset[s2[-1]];
			break;
		} else
			len--;
	}
	return l1-l2;
}

/*------------------------------------------------------------
 * crunchdict()
 */
EXPORT int crunchdict(unsigned char *d,unsigned char *s,int l) 
{ 	unsigned char *D=d,c;

	if (l > 2048) l = 2048;			/* may fail on large keys */
	while (l > 0) {
		if (isalnum((c = *s++)))
			*d++ = c;
		l--;
	}
	return (int)(d-D);
}

/*------------------------------------------------------------
 * cmpnormal()
 */
EXPORT int cmpnormal(unsigned char *s1,int l1,unsigned char *s2,int l2)
{	int len;

	if (l1 > l2)
		len = l2;
	else len = l1;

	while(len > 0) {
		if (*s1++ != *s2++) {
			l1 = s1[-1];
			l2 = s2[-1];
			break;
		} else
			len--;
	}

	return l1-l2;
}

