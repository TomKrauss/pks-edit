/*
 * AsciiHexToLong.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * Ascii hex to long conversion.
 *
 * 										created      : 01.10.91
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krau�
 */

#include "editab.h"

char *_strtolend;

/*--------------------------------------------------------------------------
 * Atol()
 */
EXPORT long Atol(char *s)
{	long i = 0;
	long minus;
	int  o;

	while (*s == ' ' || *s == '\t') s++;	/* skip leading white */  
	if (*s == '-') {
		minus = -1;
		s++;
	} else minus = 1;
	if (*s == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
		for (;;) {
			if  (*s >= '0' && *s <= '9')
				o = (*s - '0');
			else if (*s >= 'A' && *s <= 'F')
				o = (*s -'A'+10);
			else if (*s >= 'a' && *s <= 'f')
				o = (*s -'a'+10);
			else break;
			i *= 16;
			i += o;
			s++;
		}
	} else {
		while (*s >= '0' && *s <= '9') {
			i *= 10;
			i += (*s - '0');
			s++;
		}
	}
	_strtolend = s;
	return minus*i;
}
