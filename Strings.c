/*
 * STRINGS.C
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
 *
 * 								Author: TOM
 */

#include <windows.h>
#include <direct.h>

#include "pathname.h"

char* strrchr(const char *str, int ch) {	
	const char* strl = str + strlen(str);

	do {
		if (ch == *str)
			return str;
		str = AnsiPrev(str,strl);
	} while(strl > str);
	return NULL;
}

LPSTR lstrchr(char *str, char ch)
{
	while(*str) {
		if (ch == *str)
			return str;
		str = AnsiNext(str);
	}
	return NULL;
}

LPSTR strmaxcpy(LPSTR pszDest, LPSTR pszSource, int nMax)
{
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
void strdcpy(char *dest,char *source,char *fname)
{
	if (*source) {
		while (*source)
			*dest++ = *source++;
		if (dest[-1] != DIRSEPARATOR)
			*dest++ = DIRSEPARATOR;
	}
	while((*dest++ = *fname++) != 0);
}

/*--------------------------------------------------------------------------
 * basename()
 * return pointer to filename component of a path
 */
char *basename(char *fullname)
{	register char *f = fullname;
	register char c;

	while ((c = *fullname++) != 0) 
		if (c == DIRSEPARATOR) f = fullname;
	return f;
}

/*--------------------------------------------------------------------------
 * extname()
 * return pointer to the extension component of a path
 */
char *extname(char *fullname) {	
	char *base = basename(fullname);

	while(*base == '.') base++;		/* skip . and .. */
	while(*base != 0)
		if (*base++ == '.') break;
	return base;
}

/*--------------------------------------------------------------------------
 * sfsplit()
 * split a pathname in path and filename components
 */
void sfsplit(char *source,char *path,char *fn) {
	char *fname = basename(source);

	if (path != NULL) {
		while(source < fname)
			*path++ = *source++;
		*path = 0;
	}
	if (fn != NULL) {
		/*
		 * avoid overruns
		 */
		source = &fn[256];
		while(fn < source && *fname)
			*fn++ = *fname++;
		*fn = 0;
	}
}

/*------------------------------------------------------------
 * FullPathName()
 * make full pathname
 */
char *FullPathName(char *path,char *fn)
{
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
char *AbbrevName(char *fn)
{	int l,i;
	static char aname[32];
	
	if ((l = lstrlen(fn)) < 20)
		return fn;

	for (i = 0; i < 6; i++)
		aname[i] = fn[i];
	aname[i++] = '.';
	aname[i++] = '.';
	l = l - 14;

	if (lstrchr(&fn[l], DIRSEPARATOR)) {
		while(fn[l] && fn[l] != DIRSEPARATOR) {
			l++;
		}
	}

	while(fn[l]) {
		aname[i++] = fn[l++];
	}
	aname[i] = 0;

	return aname;
}

/*------------------------------------------------------------
 * OemAbbrevName()
 */
char *OemAbbrevName(char *fn)
{
	char *ret = AbbrevName(fn);
	OemToAnsi(ret,ret);
	return ret;
}

