/*
 * AsciiHexToLong.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * Ascii hex to long conversion.
 *
 * 										created: 01.10.91
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "edtypes.h"

char *_strtolend;

/**
 * Convert a string input to long assuming hexadecimal encoding. 
 */
EXPORT long long string_convertToLongBase16(char* s) {
	long long i = 0;
	int o;
	for (;;) {
		if (*s >= '0' && *s <= '9')
			o = (*s - '0');
		else if (*s >= 'A' && *s <= 'F')
			o = (*s - 'A' + 10);
		else if (*s >= 'a' && *s <= 'f')
			o = (*s - 'a' + 10);
		else break;
		i *= 16;
		i += o;
		s++;
	}
	_strtolend = s;
	return i;
}

/*--------------------------------------------------------------------------
 * string_convertToLong()
 */
EXPORT long long string_convertToLong(char *s)
{
	long long i = 0;
	long long minus = 1;

	while (*s == ' ' || *s == '\t') s++;	/* skip leading white */  
	if (*s == '-') {
		minus = -1;
		s++;
	}
	if (*s == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
		i = string_convertToLongBase16(s);
	} else {
		while (*s >= '0' && *s <= '9') {
			i *= 10;
			i += (*s - '0');
			s++;
		}
		_strtolend = s;
	}
	return minus*i;
}

