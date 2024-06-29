/*
 * StringUtil.c
 *
 * Project: PKS Edit for Windows
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

struct tagSTRING_BUF {
	int sb_flags;				// see SB_ constants
	int sb_lineNumber;
	size_t	sb_capacity;
	unsigned char* sb_string;
	unsigned char* sb_current;
};

/*--------------------------------------------------------------------------
 * string_skipBlanks()
 * Skip over the leading blanks in a string. If the string only contains
 * blanks, return 0 otherwise return a pointer to the first non space character
 * in the string.
 */
unsigned char* string_skipBlanks(unsigned char* s) {
	while (*s == ' ' || *s == '\t') s++;
	return (*s ? s : 0L);
}

/*
 * Find a substring p2 in a string p1 comparing all characters in a case ignore way.
 */
char* string_strcasestr(const char* p1, const char* p2) {
	for (;; p1++) {
		for (size_t i = 0;; i++) {
			unsigned char c2 = (unsigned char) p2[i];
			if (c2 == '\0') {
				return (char*)p1;
			}
			unsigned char c1 = (unsigned char)p1[i];
			if (tolower(c1) != tolower(c2)) {
				break;
			}
		}
		if (*p1 == '\0') {
			return NULL;
		}
	}
}
/*
 * Copies a string into a destrination string - at max nMax characters.
 * Returns the pointer to the end of the string.
 */
char* strmaxcpy(char* pszDest, const char* pszSource, int nMax) {
	LPSTR		pszEnd;
	
	pszEnd = pszDest + nMax;
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
char *string_getFullPathName(const char *path, const char *fn, size_t maxPathLen) {
	char *		pszFn;


	GetFullPathName(fn, (DWORD)maxPathLen, (char *)path, &pszFn);
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


#define	isblnk(c)    (c == ' ' || c == '\t' || c == '' || c == '' || c == '')

/*--------------------------------------------------------------------------
 * string_isSpace()
 */
BOOL string_isSpace(unsigned char c) {
	return c <= ' ' && isblnk(c);
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

/*
 * Compare a string with a second string to a maximum to the length of the 2nd string
 * and return 0, if both strings match, 1 otherwise. If 'bCaseIgnore' is 1 a case 
 * insensitive comparison is performed.
 */
int string_compareWithSecond(const unsigned char* s1, const unsigned char* s2, int bCaseIgnore) {
	unsigned char c2;

	while ((c2 = *s2++) != 0) {
		unsigned char c = *s1++;
		if (c != c2) {
			if (bCaseIgnore) {
				if (pks_islower(c)) {
					c = toupper(c);
				}
				if (pks_islower(c2)) {
					c2 = toupper(c2);
				}
				if (c != c2) {
					return 1;
				}
			} else {
				return 1;
			}
		}
	}
	return 0;
}
/*
 * Returns true if the passed character is an identifier in typical programming languages (including umlauts).
 * TODO: add a context sensitive variant of this.
 */
BOOL char_isIdentifier(unsigned char c) {
	return  isident(c);
}

/*
 * Returns TRUE, if the passed character is not a space.
 */
BOOL char_isNospace(unsigned char c) {
	return !isspace(c);
}

/*
 * Create a string buffer with a default size.
 */
STRING_BUF* stringbuf_create(size_t nDefaultSize) {
	STRING_BUF* pResult = calloc(1, sizeof * pResult);
	pResult->sb_capacity = nDefaultSize;
	pResult->sb_string = calloc(1, nDefaultSize + 1);
	pResult->sb_current = pResult->sb_string;
	return pResult;
}

/*
 * Assigns options to the string buffer. Note, that this is only legal right
 * after having created the string buffer.
 */
void stringbuf_setFlags(STRING_BUF* pBuf, int nFlags) {
	pBuf->sb_flags = nFlags;
}

/*
 * Make sure, a string buffer may hold a number of characters. 
 */
static void stringbuf_accomodateSpace(STRING_BUF* pBuf, size_t nAdditional) {
	size_t nSize = pBuf->sb_current - pBuf->sb_string;
	if (nSize + nAdditional >= pBuf->sb_capacity) {
		size_t nCapacity = nSize + nAdditional + 128;
		unsigned char* pNew = realloc(pBuf->sb_string, nCapacity+1);
		pBuf->sb_current = pNew + (pBuf->sb_current - pBuf->sb_string);
		pBuf->sb_string = pNew;
		pBuf->sb_capacity = nCapacity;
	}
}

/*
 * Returns a last character of a string. 
 */
char stringbuf_lastChar(STRING_BUF* pBuf) {
	if (pBuf->sb_current > pBuf->sb_string) {
		return pBuf->sb_current[-1];
	}
	return 0;
}

/*
 * Add a single character to a string buffer.
 */
void stringbuf_appendChar(STRING_BUF* pBuf, unsigned char c) {
	stringbuf_accomodateSpace(pBuf, 1);
	*pBuf->sb_current++ = c;
	if (c == '\n') {
		pBuf->sb_lineNumber++;
	}
	*pBuf->sb_current = 0;
}

static void stringbuf_countNL(STRING_BUF* pBuf, const char* pszString) {
	char c;
	while ((c = *pszString++) != 0) {
		if (c == '\n') {
			pBuf->sb_lineNumber++;
		}
	}
}

/*
 * Append a string to a string buffer.
 */
void stringbuf_appendString(STRING_BUF* pBuf, const unsigned char* pszString) {
	size_t nAdditional = strlen(pszString);
	stringbuf_accomodateSpace(pBuf, nAdditional);
	strcpy(pBuf->sb_current, pszString);
	if (pBuf->sb_flags & SB_COUNT_LINE_NUMBERS) {
		stringbuf_countNL(pBuf, pszString);
	}
	pBuf->sb_current += nAdditional;
}

/*
 * Append a given number of bytes of a string to a string buffer.
 */
void stringbuf_appendStringLength(STRING_BUF* pBuf, const unsigned char* pszString, size_t nAdditional) {
	stringbuf_accomodateSpace(pBuf, nAdditional);
	strncpy(pBuf->sb_current, pszString, nAdditional);
	if (pBuf->sb_flags & SB_COUNT_LINE_NUMBERS) {
		char c;
		const char* pszEnd = pszString + nAdditional;
		while (pszString < pszEnd && (c = *pszString++) != 0) {
			if (c == '\n') {
				pBuf->sb_lineNumber++;
			}
		}
	}
	pBuf->sb_current[nAdditional] = 0;
	pBuf->sb_current += nAdditional;
}

/*
 * Inserts a character at a given position. 
 */
void stringbuf_insertChar(STRING_BUF* pBuf, int nPosition, char cChar) {
	stringbuf_accomodateSpace(pBuf, 1);
	size_t nSize = stringbuf_size(pBuf);
	if (nPosition >= nSize || nPosition < 0) {
		// TODO: exception.
		return;
	}
	memcpy(pBuf->sb_string + nPosition + 1, pBuf->sb_string + nPosition, (nSize - nPosition) + 1);
	pBuf->sb_string[nPosition] = cChar;
	if (cChar == '\n') {
		pBuf->sb_lineNumber++;
	}
	pBuf->sb_current++;
}

/*
 * Returns the number of characters in the string buffer. 
 */
size_t stringbuf_size(STRING_BUF* pBuf) {
	return pBuf->sb_current - pBuf->sb_string;
}

/*
 * Destroy a string buffer.
 */
void stringbuf_destroy(STRING_BUF* pBuf) {
	free(pBuf->sb_string);
	free(pBuf);
}

/*
 * Get a pointer to the actual string constructed in the string buffer.
 */
unsigned char* stringbuf_getString(STRING_BUF* pBuf) {
	return pBuf->sb_string;
}

/*
 * Reset the string buffer to be empty. 
 */
void stringbuf_reset(STRING_BUF* pBuf) {
	pBuf->sb_current = pBuf->sb_string;
	pBuf->sb_string[0] = 0;
	pBuf->sb_lineNumber = 0;
}

/*
 * Resizes the string buffer to a smaller size.
 */
void stringbuf_truncate(STRING_BUF* pBuf, size_t nNewSize) {
	if (nNewSize >= pBuf->sb_capacity) {
		return;
	}
	pBuf->sb_current = pBuf->sb_string + nNewSize;
	pBuf->sb_current[0] = 0;
	if (pBuf->sb_flags & SB_COUNT_LINE_NUMBERS) {
		pBuf->sb_lineNumber = 0;
		stringbuf_countNL(pBuf, pBuf->sb_string);
	}
}

/*
 * Returns the "current line number" 0-based in the string stored in the string buffer.
 * This will work only if stringbuf_setFlags(... SB_COUNT_LINE_NUMBER) had been invoked.
 */
int stringbuf_getLineNumber(STRING_BUF* pBuf) {
	return pBuf->sb_lineNumber;
}

/*
 * Check, whether a strings starts with a given prefix. 
 */
int string_startsWith(const char* pszString, const char* pszPrefix) {
	return strncmp(pszPrefix, pszString, strlen(pszPrefix)) == 0;
}
