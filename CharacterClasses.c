/*
 * CharacterClasses.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: character classifications
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

# include <ctype.h>
# include "editab.h"
# include "edctype.h"

# define	isblnk(c)			(c == ' ' || c == '\t' || c == '' || \
					      c == '' || c == '')
					      
/*--------------------------------------------------------------------------
 * IsSpace()
 */
EXPORT int IsSpace(unsigned char c)
{
	return c <= ' ' && isblnk(c);
}

/*---------------------------------*/
/* skipblank()					*/
/*---------------------------------*/
EXPORT unsigned char *skipblank(unsigned char *s, unsigned char *send)
{
	while(IsSpace(*s))
		s++;
	return (s > send) ? send : s;
}

/*---------------------------------*/
/* skipnoblank()				*/
/*---------------------------------*/
EXPORT unsigned char *skipnoblank(unsigned char *s,unsigned char *send)
{
	while(s < send && !IsSpace(*s))
		s++;
	return s;
}

/*--------------------------------------------------------------------------
 * CntSpaces()
 * count spaces up to a given position
 * Spaces include attribute special spaces
 */
EXPORT int CntSpaces(unsigned char *s, int pos)
{	int n;

	for (n = 0; n < pos && IsSpace(s[n]); n++)
		;
	return n;
}

EXPORT int isword(unsigned char c)
{	return  (isalnum(c) || c == 'ö' ||c == 'ä' ||c == 'ü');	}

EXPORT int isnospace(unsigned char c)
{	return ! isspace(c); 	}

EXPORT int isfname(unsigned char c)
{ 	return (istosfname(c)); 	}
