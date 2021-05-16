/*
 * StringUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: substitute some standard C-Library functions with the
 *		  correspondant window functions
 * 		  filename string manipulations
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include <direct.h>

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
			return str;
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
			return str;
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

/*--------------------------------------------------------------------------
 * strdcpy()
 * concat file and dirname and make sure there is one(1!) SLASH between
 * the both
 */
void strdcpy(char *dest, const char *pathname, const char *fname) {
	if (*pathname) {
		while (*pathname)
			*dest++ = *pathname++;
		if (dest[-1] != DIRSEPARATOR)
			*dest++ = DIRSEPARATOR;
	}
	while((*dest++ = *fname++) != 0);
}

/*--------------------------------------------------------------------------
 * basename()
 * return pointer to filename component of a pathName
 */
char *basename(const char *fullname)
{	register char *f = fullname;
	register char c;

	while ((c = *fullname++) != 0) 
		if (c == DIRSEPARATOR) f = fullname;
	return f;
}

/*--------------------------------------------------------------------------
 * extname()
 * return pointer to the extension component of a pathName
 */
char *extname(const char *fullname) {
	char *base = basename(fullname);

	while(*base == '.') base++;		/* skip . and .. */
	while(*base != 0)
		if (*base++ == '.') break;
	return base;
}

/*--------------------------------------------------------------------------
 * sfsplit()
 * split a pathname in pathName and filename components
 */
void sfsplit(const char *completeFileName, char *pathName, char *fileName) {
	char *fname = basename(completeFileName);

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
 * FullPathName()
 * make full pathname
 */
char *FullPathName(const char *path, const char *fn) {
#if defined(WIN32)
	char *		pszFn;

	GetFullPathName(fn, 256, path, &pszFn);
#else
	register char *f = fn,*dst = path,c;

	if (fn[1] != ':') {
		*dst++ = GetCurrentDrive() + '@';
		*dst++ = ':';
		if (*fn != DIRSEPARATOR)
			*dst++ = DIRSEPARATOR;
	} else {
		fn += 2;
	}
	if (*fn != DIRSEPARATOR) {
		GetCurrentDirectory(256, dst);
		while(*dst) dst++;
		*dst++ = DIRSEPARATOR;
	}
	while((c = *f) != 0) {
		if (c == '.') {
			if (f[1] == DIRSEPARATOR) {
				f += 2;
				continue;
			} else if (f[1] == '.' && f[2] == DIRSEPARATOR) {
				f += 3;
				dst--;
				while(dst[-1] != DIRSEPARATOR) if (dst-- <= path) return 0;
				continue;
			}
		}
		if (c >= 'a' && c <= 'z') c += ('A'-'a');
		*dst++ = c;
		f++;
	}
	*dst = 0;
#endif	
	return path;
}

/*------------------------------------------------------------
 * AbbrevName()
 */
char *AbbrevName(const char *fn) {	
	int l,i;
	static char aname[64];
	
	if ((l = lstrlen(fn)) < sizeof aname)
		return fn;

	for (i = 0; i < 20; i++) {
		aname[i] = fn[i];
	}
	aname[i++] = '.';
	aname[i++] = '.';
	aname[i++] = '.';
	l = l - 40;
	strcpy(aname + i, fn + l);
	return aname;
}

/*------------------------------------------------------------
 * OemAbbrevName()
 */
char *OemAbbrevName(const char *fn) {
	char *ret = AbbrevName(fn);
	OemToAnsi(ret,ret);
	return ret;
}

