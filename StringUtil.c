/*
 * StringUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: substitute some standard C-Library functions with the
 *		  correspondant window functions
 * 		  filename string manipulations
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "alloc.h"
#include <ctype.h>

#include "edctype.h"
#include "pathname.h"
#include "stringutil.h"

/*
 * Return a pointer to the last matching position, where a character can be found in a String 
 * or NULL if it cannot be found at all.
 */
char* strrchr(const char *str, int ch) {	
	const char* strl = str + strlen(str);

	do {
		if (ch == *str)
			return (char*) str;
		str = AnsiPrev(str,strl);
	} while(strl > str);
	return NULL;
}

/*
 * Return the pointer to the first matching position of a character in a String or NULL
 * if no match is found.
 */
char *lstrchr(const char *str, char ch) {
	while(*str) {
		if (ch == *str)
			return (char*)str;
		str = AnsiNext(str);
	}
	return NULL;
}

/*
 * Copies a pathname string into a destrination string - at max nMax characters.
 * Returns the pointer to the end of the string.
 */
char* strmaxcpy(char* pszDest, const char* pszSource, int nMax) {
	LPSTR		pszEnd;
	
	pszEnd = pszDest + nMax - 1;
	while(pszDest < pszEnd) {
		if ((*pszDest = *pszSource) == 0) {
			return pszDest;
		}
		pszSource++;
		pszDest++;
	}
	*pszDest = 0;
	return pszDest;
}

/*
 * Return true, if the passed char is a directory separator. 
 */
static int string_isdirSeparator(unsigned char c) {
	return c == DIRSEPARATOR || c == '/';
}

/*--------------------------------------------------------------------------
 * string_concatPathAndFilename()
 * concat file and dirname and make sure there is one(1!) SLASH between
 * the both
 */
void string_concatPathAndFilename(char *dest, const char *pathname, const char *fname) {
	if (*pathname) {
		while (*pathname)
			*dest++ = *pathname++;
		if (!string_isdirSeparator(dest[-1]))
			*dest++ = DIRSEPARATOR;
	}
	while((*dest++ = *fname++) != 0);
}

/*--------------------------------------------------------------------------
 * string_getBaseFilename()
 * return pointer to filename component of a pathName
 */
char *string_getBaseFilename(const char *fullname)
{	register const char *f = fullname;
	register char c;

	while ((c = *fullname++) != 0) 
		if (string_isdirSeparator(c)) f = fullname;
	return (char*)f;
}

/*--------------------------------------------------------------------------
 * string_getFileExtension()
 * return pointer to the extension component of a pathName
 */
char *string_getFileExtension(const char *fullname) {
	char *base = string_getBaseFilename(fullname);

	while(*base == '.') base++;		/* skip . and .. */
	while(*base != 0)
		if (*base++ == '.') break;
	return base;
}

/*--------------------------------------------------------------------------
 * string_splitFilename()
 * split a pathname in pathName and filename components
 */
void string_splitFilename(const char *completeFileName, char *pathName, char *fileName) {
	char *fname = string_getBaseFilename(completeFileName);

	if (pathName != NULL) {
		while(completeFileName < fname)
			*pathName++ = *completeFileName++;
		*pathName = 0;
	}
	if (fileName != NULL) {
		/*
		 * avoid overruns
		 */
		completeFileName = &fileName[256];
		while(fileName < completeFileName && *fname)
			*fileName++ = *fname++;
		*fileName = 0;
	}
}

/*------------------------------------------------------------
 * string_getFullPathName()
 * make full pathname
 */
char *string_getFullPathName(const char *path, const char *fn) {
	char *		pszFn;

	GetFullPathName(fn, 256, (char *)path, &pszFn);
	return (char*)path;
}

/*------------------------------------------------------------
 * string_abbreviateFileName()
 */
char *string_abbreviateFileName(const char *fn) {	
	int l,i;
	static char aname[90];
	
	if ((l = lstrlen(fn)) < sizeof aname)
		return (char*)fn;

	for (i = 0; i < (sizeof aname / 2) - 4; i++) {
		aname[i] = fn[i];
	}
	aname[i++] = '.';
	aname[i++] = '.';
	aname[i++] = '.';
	l = l - (sizeof aname / 2);
	strcpy(aname + i, fn + l);
	return aname;
}

/*------------------------------------------------------------
 * string_abbreviateFileNameOem()
 */
char *string_abbreviateFileNameOem(const char *fn) {
	char *ret = string_abbreviateFileName(fn);
	OemToAnsi(ret,ret);
	return ret;
}


#define	isblnk(c)    (c == ' ' || c == '\t' || c == '' || c == '' || c == '')

/*--------------------------------------------------------------------------
 * string_isSpace()
 */
BOOL string_isSpace(unsigned char c) {
	return c <= ' ' && isblnk(c);
}

/*---------------------------------*/
/* string_skipSpacesIn()					*/
/*---------------------------------*/
unsigned char* string_skipSpacesIn(unsigned char* s, unsigned char* send) {
	while (string_isSpace(*s))
		s++;
	return (s > send) ? send : s;
}

/*---------------------------------*/
/* string_skipNonSpaceCharactersIn()				*/
/*---------------------------------*/
unsigned char* string_skipNonSpaceCharactersIn(unsigned char* s, unsigned char* send) {
	while (s < send && !string_isSpace(*s))
		s++;
	return s;
}

/*--------------------------------------------------------------------------
 * string_countSpacesIn()
 * count spaces up to a given position
 * Spaces include attribute special spaces
 */
int string_countSpacesIn(unsigned char* s, int pos) {
	int n;

	for (n = 0; n < pos && string_isSpace(s[n]); n++)
		;
	return n;
}

/*--------------------------------------------------------------------------
 * string_countCharacters()
 * Count the number of occurrences of a character in a string.
 */
int string_countCharacters(char* s, char c) {
	int n = 0;

	while (*s) {
		if (*s++ == c) {
			n++;
		}
	}
	return n;
}

/*
 * Returns true if the passed character is an identifier in typical programming languages (including umlauts).
 * TODO: add a context sensitive variant of this.
 */
BOOL char_isIdentifier(unsigned char c) {
	return  (isalnum(c) || (char)c == 'ö' || (char)c == 'ä' || (char)c == 'ü' || (char)c== 'Ü' || (char)c == 'Ö' || (char)c == 'Ä' || (char)c == 'ß' || c == '_');
}

/*
 * Returns TRUE, if the passed character is not a space.
 */
BOOL char_isNospace(unsigned char c) {
	return !isspace(c);
}

/*
 * Return true, if the character is a valid filename character on the current platform.
 */
BOOL char_isFilename(unsigned char c) {
	return (istosfname(c));
}
