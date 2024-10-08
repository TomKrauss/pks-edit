/*
 * stringutil.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: string utility functions.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef STRINGUTIL_H

typedef int					BOOL;

typedef struct tagSTRING_BUF STRING_BUF;
typedef struct tagWINFO WINFO;

extern const char* _strtolend;

/*--------------------------------------------------------------------------
 * string_convertToLong()
 * Convert the first integer value from the passed String (interpret hex numbers and
 * normal numbers) set the resulting pointer after parsing to _strtolend.
 */
extern long long string_convertToLong(const char* s);

/**
 * Convert a string input to long assuming hexadecimal encoding.
 */
extern long long string_convertToLongBase16(const char* s);

/*
 * Compare a string with a second string to a maximum to the length of the 2nd string
 * and return 0, if both strings match, 1 otherwise. If 'bCaseIgnore' is 1 a case
 * insensitive comparison is performed.
 */
extern int string_compareWithSecond(const unsigned char* s1, const unsigned char* s2, int bCaseIgnore);

/*
 * Copies a source string into a destrination string - at max nMax characters.
 * Returns the pointer to the end of the string.
 */
extern char* strmaxcpy(char* pszDest, const char* pszSource, int nMax);

/*--------------------------------------------------------------------------
 * string_concatPathAndFilename()
 * concat file and dirname and make sure there is one(1!) SLASH between
 * the both
 */
extern void string_concatPathAndFilename(char* dest, const char* pathname, const char* fname);

/*--------------------------------------------------------------------------
 * string_getBaseFilename()
 * return pointer to filename component of a path
 */
extern char* string_getBaseFilename(const char* fullname);

/*
 * Returns a last character of a string.
 */
extern char stringbuf_lastChar(STRING_BUF* pBuf);

/*--------------------------------------------------------------------------
 * string_splitFilename()
 * split a pathname in pathName and filename components
 */
extern void string_splitFilename(const char* completeFileName, char* pathName, char* fileName, size_t maxFilenameLength);

/*------------------------------------------------------------
 * string_abbreviateFileName()
 * Return an abbreviated filename. Attention: you need to copy the generated
 * string. When calling this method again the old memory is overwritten.
 */
extern char* string_abbreviateFileName(const char* filename);

/*------------------------------------------------------------
 * string_getFullPathName()
 * make full pathname
 */
extern char* string_getFullPathName(const char* path, const char* fn, size_t maxPathLen);

/*
 * Free the dereferenced pointer and initialize the space pointed to with NULL.
 */
extern void destroy(void** ap);

/*--------------------------------------------------------------------------
 * string_matchFilename()
 *
 * Match a filename using typical file name pattern matching rules (using * and ?).
 * Multiple patterns may be  specified separated by "," of ";". One may precede
 * a pattern with '!' to explicitly not match a given pattern - e.g. !*.obj will
 * explicitly exclude all files named *.obj.
 */
extern int string_matchFilename(const char* string, const char* pattern);

/*--------------------------------------------------------------------------
 * string_isSpace()
 */
extern BOOL string_isSpace(unsigned char c);

/*--------------------------------------------------------------------------
 * string_countSpacesIn()
 * count spaces up to a given position
 * Spaces include attribute special spaces
 */
extern int string_countSpacesIn(unsigned char* s, int pos);

/*
 * Returns true if the passed character is an identifier in typical programming languages (including umlauts).
 * TODO: add a context sensitive variant of this.
 */
extern BOOL char_isIdentifier(unsigned char c);

/*
 * Returns TRUE, if the passed character is not a space.
 */
extern BOOL char_isNospace(unsigned char c);

/*--------------------------------------------------------------------------
 * string_initDateformats()
 * Initialize the PKS Edit date format.
 */
extern void string_initDateformats(void);

/*
 * Find a substring p2 in a string p1 comparing all characters in a case ignore way.
 */
extern char* string_strcasestr(const char* p1, const char* p2);

/*--------------------------------------------------------------------------
 * string_skipBlanks()
 * Skip over the leading blanks in a string. If the string only contains
 * blanks, return 0 otherwise return a pointer to the first non space character
 * in the string.
 */
extern unsigned char* string_skipBlanks(unsigned char* s);

/*
 * Create a string buffer with a default size. 
 */
extern STRING_BUF* stringbuf_create(size_t nDefaultSize);

/*
 * Add a single character to a string buffer.
 */
extern void stringbuf_appendChar(STRING_BUF* pBuf, unsigned char c);

/*
 * Append a string to a string buffer.
 */
extern void stringbuf_appendString(STRING_BUF* pBuf, const unsigned char * pszString);

/*
 * Append a given number of bytes of a string to a string buffer.
 */
extern void stringbuf_appendStringLength(STRING_BUF* pBuf, const unsigned char* pszString, size_t nAdditional);

/*
 * Assigns options to the string buffer. Note, that this is only legal right
 * after having created the string buffer.
 */
#define SB_COUNT_LINE_NUMBERS 0x1
extern void stringbuf_setFlags(STRING_BUF* pBuf, int nFlags);

/*
 * Returns the "current line number" 0-based in the string stored in the string buffer.
 * This will work only if stringbuf_setFlags(... SB_COUNT_LINE_NUMBER) had been invoked.
 */
extern int stringbuf_getLineNumber(STRING_BUF* pBuf);

/*
 * Returns the number of characters in the string buffer.
 */
extern size_t stringbuf_size(STRING_BUF* pBuf);

/*
 * Returns true, if the string buffer contains only space characters.
 */
extern BOOL stringbuf_isBlank(STRING_BUF* pBuf);

/*
 * Destroy a string buffer.
 */
extern void stringbuf_destroy(STRING_BUF* pBuf);

/*
 * Get a pointer to the actual string constructed in the string buffer.
 */
extern unsigned char* stringbuf_getString(STRING_BUF* pBuf);

/*
 * Reset the string buffer to be empty.
 */
extern void stringbuf_reset(STRING_BUF* pBuf);

/*
 * Resizes the string buffer to a smaller size.
 */
extern void stringbuf_truncate(STRING_BUF* pBuf, size_t nNewSize);

/*
 * Inserts a character at a given position.
 */
extern void stringbuf_insertChar(STRING_BUF* pBuf, int nPosition, char cChar);

typedef struct tagSPRINTF_ARGS {
	WINFO* sa_wp;
	union U_ARG_VALUE {					// 0-terminated data structure.
		long	v_l;
		double	v_d;
		char*	v_s;
	} *sa_values;
} SPRINTF_ARGS;

/*--------------------------------------------------------------------------
 * mysprintf()

	One can append to %x formats the following $ regex_addCharacterToCharacterClass holders to format the corresponding value.
	One sample would be %s$f - prints the file name of a file in string format.

	$O		fileoffset
	$C		current character
	$l		current line
	$c		current col
	$f		Filename
	$F		full path
	$b		Filename without extension
	$w		Window Handle
	$h		Verweispfad - Hilfe
	$t		Verweispfad
	$*		ge�ndert : "*"
	$r		read only: "

	Special % formats supported:
	%D		current date
	%T		current time
*/
extern int mysprintf(char* pDestination, char* format, SPRINTF_ARGS* pArgs);

/*
 * Return a PKS Edit variable to be used e.g. in code templates.
 */
extern void string_getVariable(WINFO* wp, const char* pVar, unsigned char* pResult, size_t nSize);

/*
 * Get a variable and return the value of it in a buffer. The buffer will be valid until
 * the next invocation of this method.
 */
extern char* string_getVariableWithDefaults(const char* pVar);

/*
 * Check, whether a strings starts with a given prefix. Returns 1 if this
 * is the case and 0 otherwise.
 */
int string_startsWith(const char* pszString, const char* pszPrefix);

extern char _pksVersion[];

#define STRINGUTIL_H
#endif