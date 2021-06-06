/*
 * stringutil.h
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: string utility functions.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#ifndef STRINGUTIL_H

typedef int					BOOL;

extern char* _strtolend;

/*--------------------------------------------------------------------------
 * string_convertToLong()
 * Convert the first integer value from the passed String (interpret hex numbers and
 * normal numbers) set the resulting pointer after parsing to _strtolend.
 */
extern long string_convertToLong(char* s);

/*
 * Return a pointer to the last matching position, where a character can be found in a String
 * or NULL if it cannot be found at all.
 */
extern char* strrchr(const char* str, int ch);

/*
 * Return the pointer to the first matching position of a character in a String or NULL
 * if no match is found.
 */
extern char* lstrchr(const char* str, char ch);

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

/*--------------------------------------------------------------------------
 * string_getFileExtension()
 * return pointer to the extension component of a path
 */
extern char* string_getFileExtension(const char* fullname);


/*--------------------------------------------------------------------------
 * string_splitFilename()
 * split a pathname in pathName and filename components
 */
extern void string_splitFilename(const char* completeFileName, char* pathName, char* fileName);

/*------------------------------------------------------------
 * string_abbreviateFileName()
 * Return an abbreviated filename. Attention: you need to copy the generated
 * string. When calling this method again the old memory is overwritten.
 */
extern char* string_abbreviateFileName(const char* filename);

/*------------------------------------------------------------
 * string_abbreviateFileNameOem()
 * Same as string_abbreviateFileName, but all characters converted to ISO-8859-1.
 */
extern char* string_abbreviateFileNameOem(const char* fn);

/*------------------------------------------------------------
 * string_getFullPathName()
 * make full pathname
 */
extern char* string_getFullPathName(const char* path, const char* fn);

/*------------------------------------------------------------
 * string_allocate()
 * allocate a copy of the passed string.
 */
unsigned char* string_allocate(unsigned char* buf);

/*
 * Free the dereferenced pointer and initialize the space pointed to with NULL.
 */
extern void destroy(void** ap);

/*--------------------------------------------------------------------------
 * string_matchFilename()
 * Match a filename using ATARI TOS matching rules. Multiple patterns may be
 * specified separated by "," of ";"
 */
extern int string_matchFilename(char* string, char* pattern);

/*--------------------------------------------------------------------------
 * stralloc()
 * Allocate a string and copy it.
 */
unsigned char* stralloc(unsigned char* buf);

/*--------------------------------------------------------------------------
 * string_isSpace()
 */
extern BOOL string_isSpace(unsigned char c);

/*---------------------------------*/
/* string_skipSpacesIn()					*/
/*---------------------------------*/
extern unsigned char* string_skipSpacesIn(unsigned char* s, unsigned char* send);

/*---------------------------------*/
/* string_skipNonSpaceCharactersIn()				*/
/*---------------------------------*/
extern unsigned char* string_skipNonSpaceCharactersIn(unsigned char* s, unsigned char* send);

/*--------------------------------------------------------------------------
 * string_countSpacesIn()
 * count spaces up to a given position
 * Spaces include attribute special spaces
 */
extern int string_countSpacesIn(unsigned char* s, int pos);

/*
 * Returns true if the passed character is a letter (a-z + umlauts).
 */
extern BOOL char_isLetter(unsigned char c);

/*
 * Returns TRUE, if the passed character is not a space.
 */
extern BOOL char_isNospace(unsigned char c);

/*
 * Return true, if the character is a valid filename character on the current platform.
 */
extern BOOL char_isFilename(unsigned char c);

/*--------------------------------------------------------------------------
 * string_initDateformats()
 * Initialize the PKS Edit date format.
 */
extern void string_initDateformats(void);

#define STRINGUTIL_H
#endif