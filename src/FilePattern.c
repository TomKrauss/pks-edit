/*
 * FilePattern.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: TOS - like wildcard string_matchFilename for filenames
 * 		  special is: A,B,C => *.A|*.B|*.C
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include <stdio.h>
#include "alloc.h"
#include "stringutil.h"

/*--------------------------------------------------------------------------
 * string_convertToUpperCase()
 */
static void string_convertToUpperCase(char *s)
{
	while (*s) {
		if (*s >= 'a' && *s <= 'z')
			*s += ('A' - 'a');
		s++;
	}
}

/*--------------------------------------------------------------------------
 * cclass()
 * character class [...] matching
 */
#define	NOT		'!'							/* might use ^ */
static char *cclass(register char *p, register int sub) {	
	int c;
	int d;
	int bNot;
	int bFound;

	if ((bNot = *p == NOT) != 0)
		p++;
	bFound = bNot;
	do {
		if (*p == '\0')
			return 0;
		c = *p;
		if (p[1] == '-' && p[2] != ']') {
			d = p[2];
			p++;
		} else
			d = c;
		if (c == sub || c <= sub && sub <= d)
			bFound = !bNot;
	} while (*++p != ']');

	return(bFound? p+1: 0);
}

/*------------------------------------------------------
 * gmatch()
 * UNIX filename string_matchFilename (may use *, ?, [], [!...])
 */
static int gmatch(char *s, char *p)
{	
	unsigned char sc, pc;

	while ((pc = *p++) != '\0') {
		sc = *s++;
		switch (pc) {
		case '[':
			if ((p = cclass(p, sc)) == NULL)
				return(0);
			break;

		case '?':
			if (sc == 0)
				return(0);
			break;

		case '*':
			s--;
			do {
				if (*p == '\0' || gmatch(s, p))
					return(1);
			} while (*s++ != '\0');
			return(0);

		case '\\':
			sc = *s++;
			/* drop through */
		default:
			if (sc != pc)
				return(0);
		}
	}
	return(*s == 0);
}

/*--------------------------------------------------------------------------
 * string_matchFilename()
 * 
 * Match a filename using typical file name pattern matching rules (using * and ?). 
 * Multiple patterns may be  specified separated by "," of ";". One may precede
 * a pattern with '!' to explicitly not match a given pattern - e.g. !*.obj will
 * explicitly exclude all files named *.obj.
 */
int string_matchFilename(const char *string, const char *pattern) {	
	char *	pszToken;
	char *	pszPatCopy;
	char *	pszStringCopy;

	pszStringCopy = _strdup(string);
	pszPatCopy = _strdup(pattern);
	string_convertToUpperCase(pszStringCopy);
	string_convertToUpperCase(pszPatCopy);
	pszToken = strtok(pszPatCopy, ",;");
	BOOL matchedAny = 0;
	while(pszToken) {
		BOOL invertMatch = 0;
		char* pszMatch = pszToken;
		if (pszMatch[0] == '!') {
			pszMatch++;
			invertMatch = 1;
		}
		if (gmatch(pszStringCopy, pszMatch)) {
			if (invertMatch) {
				matchedAny = 0;
				break;
			}
			matchedAny = 1;
		} else if (invertMatch) {
			matchedAny = 1;
		}
		pszToken = strtok((char *)0, ",;");
	}
	free(pszPatCopy);
	free(pszStringCopy);
	return matchedAny;
}
	

