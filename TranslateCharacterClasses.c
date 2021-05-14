/*
 * TranslateCharacterClasses.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: translate character classes
 *
 * 										created      : 25.08.90
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauß
 */

#include "edctype.h"

/*--------------------------------------------------------------------------
 * strorder()
 */
static unsigned char *strorder(unsigned char *b, unsigned char *d, unsigned char lim) {
	unsigned char *e;
	int c1=0;

	e = &d[256];
	if (!*b || *b == lim) {
/*		printf("no chars in subclass\n");
*/		return 0;
	}

	while (*b && *b != lim) {
		if (d >= e) {
ovl:/*		printf("too many chars in class\n");
*/			return 0;
		}
		if (*b == '\\') b++;
		else if (*b == '-' && c1) {
			if (*++b == '\\')
				b++;
			if (c1 < *b) {
				if (&d[*b - c1] >= e)
					goto ovl;
				while (++c1 < *b) 
					*d++ = c1;
			} else {
				if (&d[c1 - *b] >= e)
					goto ovl;
				while (--c1 > 0 && c1 > *b)
					*d++ = c1;
			}
			c1 = 0;
			continue;
		}
		if ((c1 = *b++) == 0)
			break;
		*d++ = c1;
	}
	*d = 0;
	return b;
}

/*--------------------------------------------------------------------------
 * tlcompile()
 * make a translation table out of a notation, used in the following example
 * 
 * [a-z1=A-Z!]	 =>  translate all chars a->A,b->B...z->Z,1->!
 *
 */
unsigned char *tlcompile(unsigned char *_transtab, 
					unsigned char *t, 
					unsigned char *wtable) 
{	unsigned char b1[256],b2[256],*p;
	int i;

	for (i = 0; i < 256; i++)
		_transtab[i] = i;

	/* t++;		/* override	'[' !	*/

	if ((p = strorder(t,b1,'=')) 	== 0 ||
	    (t = strorder(p+1,b2,']')) == 0)
		return 0;

	for (i = 0, p = b2; b1[i]; i++) {
		if (wtable) {
			wtable[b1[i]] |= _C;
			wtable[*p] |= _C;
		}
		_transtab[b1[i]] = *p;
		if (p[1]) p++;
	}
	return t+1;
}

