/*
 * FilePattern.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: TOS - like wildcard string_matchFilename for filenames
 * 		  special is: A,B,C => *.A|*.B|*.C
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
static char *cclass(register char *p, register int sub)
{	register int c, d, not, found;

	if ((not = *p == NOT) != 0)
		p++;
	found = not;
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
			found = !not;
	} while (*++p != ']');

	return(found? p+1: 0);
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
 * Match a filename using ATARI TOS matching rules. Multiple patterns may be 
 * specified separated by "," of ";"
 */
int string_matchFilename(char *string,char *pattern)
{	char *	pszToken;
	char *	pszPatCopy;
	char *	pszStringCopy;

	pszStringCopy = _strdup(string);
	pszPatCopy = _strdup(pattern);
	string_convertToUpperCase(pszStringCopy);
	string_convertToUpperCase(pszPatCopy);
	pszToken = strtok(pszPatCopy, ",;");
	while(pszToken) {
		if (gmatch(pszStringCopy, pszToken)) {
			free(pszStringCopy);
			free(pszPatCopy);
			return 1;
		}
		pszToken = strtok((char *)0, ",;");
	}
	free(pszPatCopy);
	free(pszStringCopy);
	return 0;
}
	

/***************************************************************************/
#ifdef TEST
static fillb2(s,d)
char *s,*d;
{	int i;
	
	for (i = 0; i < 8; i++) {
		if (!*s || *s == '.') break;
		*d++ = *s++;
	}
	while(i < 8) {
		*d++ = ' ';
		i++;
	}
	while(*s && *s != '.') s++;
	if (*s == '.') {
		s++;
		for (i = 0; i < 3; i++) {
			if (!*s) break;
			*d++ = *s++;
		}
		i += 8;
	}
	while(i < 11) {
		*d++ = ' ';
		i++;
	}
	*d = 0;
}

main(argc,argv)
int argc;
char *argv[];
{	char buf[100],b2[256];

	if (argc != 2) exit(1);
	for (;;) {
		printf("%s > ",argv[1]);
		if (gets(buf) == (char *) 0 || buf[0] == 0) break;
		fncpyin(buf,b2,0);
		/*printf("\nMATCH `%s`\n",b2);*/
		if (string_matchFilename(b2,argv[1]) == 1) printf("yes\n");
		else printf("no\n");
	}
}
#endif
