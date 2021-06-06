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
 * compare_strings()
 * Compare Strings and return a smaller value if the second string is smaller as the first.
 */
EXPORT int compare_strings(unsigned char *s1,int l1,unsigned char *s2,int l2)
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

