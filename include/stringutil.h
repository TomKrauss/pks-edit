/*
 * stringutil.h
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: string utility functions.
 *
 * 										created      :
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#ifndef STRINGUTIL_H


extern char* _strtolend;

/*--------------------------------------------------------------------------
 * Atol()
 * Convert the first integer value from the passed String (interpret hex numbers and
 * normal numbers) set the resulting pointer after parsing to _strtolend.
 */
extern long Atol(char* s);

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
 * strdcpy()
 * concat file and dirname and make sure there is one(1!) SLASH between
 * the both
 */
extern void strdcpy(char* dest, const char* pathname, const char* fname);

/*--------------------------------------------------------------------------
 * basename()
 * return pointer to filename component of a path
 */
extern char* basename(const char* fullname);

/*--------------------------------------------------------------------------
 * extname()
 * return pointer to the extension component of a path
 */
extern char* extname(const char* fullname);


/*--------------------------------------------------------------------------
 * sfsplit()
 * split a pathname in pathName and filename components
 */
extern void sfsplit(const char* completeFileName, char* pathName, char* fileName);

/*------------------------------------------------------------
 * AbbrevName()
 * Return an abbreviated filename. Attention: you need to copy the generated
 * string. When calling this method again the old memory is overwritten.
 */
extern char* AbbrevName(const char* filename);

/*------------------------------------------------------------
 * OemAbbrevName()
 * Same as AbbrevName, but all characters converted to ISO-8859-1.
 */
extern char* OemAbbrevName(const char* fn);

/*------------------------------------------------------------
 * FullPathName()
 * make full pathname
 */
extern char* FullPathName(const char* path, const char* fn);

/*------------------------------------------------------------
 * stralloc()
 * allocate a copy of the passed string.
 */
unsigned char* stralloc(unsigned char* buf);

/*
 * Free the dereferenced pointer and initialize the space pointed to with NULL.
 */
extern void destroy(char** ap);

#define STRINGUTIL_H
#endif