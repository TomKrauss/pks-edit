/*
 * RegularExpression.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: regular expression string scanning automata
 * 		  character class definitions
 *
 * 										created: 04.02.97
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stddef.h>
#include "alloc.h"
#include <string.h>
#include <ctype.h>
#include "regexp.h"
#include "pksedit.h"
#include "edctype.h"

#define	REGEX_ERROR(c)	{ err = c; goto endcompile; }

#define	ISTHERE(ep, c)			(ep[(c & 0xff) >> 3] & bittab[c & 07])
#define	MAXCTAB					32		/* maximum byte length of chartestset */
#define	makeUpperCase(c)		_l2uset[(c)]
#define	makeLowerCase(c)		_u2lset[(c)]
#define	BIG						512		/* big Range Size used by: +,*,{x,} */

static int 	_chsetinited;
unsigned char bittab[] = { 1,2,4,8,16,32,64,128 };
unsigned char _asciitab[256] =
"\0\0\0\0\0\0\0\0\0\020\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
"\0\0\0\0\0\020\0\0\0\0\0\0\0\0\0\040\0\0\0\040\0\051\051"
"\051\051\051\051\051\051\051\051\040\0\0\0\0\0\0\054\054"
"\054\054\054\054\054\054\054\054\054\054\054\054\054\054"
"\054\054\054\054\054\054\054\054\054\054\0\040\0\0\050\0"
"\052\052\052\052\052\052\052\052\052\052\052\052\052\052"
"\052\052\052\052\052\052\052\052\052\052\052\052\0\0\0\0"
"\0\0€\0\0€\0\0\0\0\0\0\0\0\0@\0\0\0\0\0€\0\0\0\0@@\0\0\0"
"€\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
"\0\0\0\0\0\040\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
"\0\0\0\0\0\0\0\0\0\0\0\0";

unsigned char _l2uset[256], _u2lset[256];
static unsigned char  _characterMappingTable[256];
static char 		  _replacedResultBuffer[500];
static unsigned char* _transtabs[8];
static char			  _reSpecialChars[MAXCTAB];

#define	DIM(tab)		(sizeof(tab)/sizeof(tab[0]))

#define	upcase(c)		_l2uset[(c)]
#define	lowcase(c)		_u2lset[(c)]

/***************************************************************************/
/* ERRORS:													*/
/*															*/
/* 1 	= 0-expression encountered								*/
/* 2 	= expressionbuffer overflow								*/
/* 3 	= illegal makro/range combination							*/
/* 4 	= wrong characterclass-definition							*/
/* 5 	= too many Subexpressions								*/
/* 6 	= subexpression-brackets not in balance 					*/
/* 7 	= wrong {..,..} - Syntax 								*/
/* 8 	= { - Number too big									*/
/* 9 	= {i,j}: i must be less than j							*/
/* 10	= \ at end of the line encountered 						*/
/* 11	= Subexpression # unknown								*/
/* 12	= Pipe outside () - Pairs								*/
/*															*/
/***************************************************************************/

static int _reerrmsg[] = {
	 IDS_MSGRENOTNULL,
	 IDS_MSGRECOMPLEXEXPR,
	 IDS_MSGREMACRORANGESYNTAX,
	 IDS_MSGRERANGE,
	 IDS_MSGREMANYBRACKETS,
	 IDS_MSGRENOBRACKETMATCH,
	 IDS_MSGREFROMTO,
	 IDS_MSGREMINMAXOVL,
	 IDS_MSGREMINMAXERR,
	 IDS_MSGREBSLERR,
	 IDS_MSGREBRUNDEF,
	 IDS_MSGREPIPEERR
};


typedef enum {
	START_OF_LINE = 0,
	END_OF_LINE = 1,
	START_OF_IDENTIFIER = 2,
	END_OF_IDENTIFIER = 3,
	ANY_CHAR = 4,
	STRING = 5,
	CHAR_CLASS = 6,
	SINGLE_CHAR = 7,
	CASE_IGNORE_CHAR = 8,
	WHITE_SPACE_CCLASS = 9,
	BACK_REFERENCE = 10,
	GROUP = 11,
	GROUP_END = 12,
	ALTERNATIVE = 13,
	DIGIT_CCLASS = 14,
	WORD_CCLASS = 15,
	END_OF_MATCH = 16,			// marks the end of the pattern
	NON_WHITE_SPACE_CCLASS = 17,
	HEADER = 18,
} MATCH_TYPE;

typedef struct tag_MATCH_RANGE {
	char m_minOccurrence : 7;		// minimum number of occurrence - only define in case of ..._RANGE types
	char m_lazy : 1;				// non gridy match, if != 0 try to match minOccurrence times.
	char m_maxOccurrence;			// max number of occurrence- only define in case of ..._RANGE types
} MATCH_RANGE;

typedef struct tagMATCHER {
	char m_type	 : 7;					// MATCH_TYPE as defined above
	char m_range : 1;					// whether this type has a range information attached.
	union {
		struct tagMATCH_HEADER {
			unsigned char m_minMatchSize;		// Defines the minimum number of bytes matched by the expression.
		} m_header;
		struct tagMATCH_GROUP {
			unsigned char m_offsetNext;			// if alternatives ("a|b") are defined - the offset to the next alternative definition - must be first element in structure.
			char m_nonCapturing : 1;	// for groups - if non capturing this is true.
			char m_bracketNumber : 7;	// The bracket number
			unsigned char m_groupEnd;	// the end of the group
		} m_group;
		struct tagMATCH_GROUP_END {
			char m_nonCapturing : 1;	// for groups - if non capturing this is true.
			char m_bracketNumber : 7;	// The bracket number
			char m_groupStart;			// the number of bytes back to the group start.
		} m_groupEnd;
		struct tagCASE_CHAR {
			char m_c1;
			char m_c2;
		} m_caseChar;
		struct tagSTRING {
			char m_length;
			char m_chars[1];
		} m_string;
		struct tagALTERNATIVE {
			unsigned char m_offsetNext;				// for alternatives - the offset to the next alternative definition or 0, if there is no next 
		} m_alternative;
		char m_char;						// single character to match
		char m_reference;					// A group reference used by back-references.
		char m_characterClass[MAXCTAB];		// character class to match
	} m_param;
} MATCHER;

static int matcherSizes[HEADER + 1] = {
	/*START_OF_LINE*/		1,
	/*END_OF_LINE */		1,
	/*START_OF_IDENTIFIER*/	1,
	/*END_OF_IDENTIFIER*/	1,
	/* ANY_CHAR */			2,
	/* STRING */			-1,			// dynamic
	/* CHAR_CLASS */		sizeof(MATCHER),
	/* SINGLE_CHAR */		2,
	/* CASE_IGNORE_CHAR*/	3,
	/* WHITE_SPACE_CCLASS */ 2,
	/* BACK_REFERENCE */	offsetof(MATCHER, m_param.m_reference)+1,
	/* GROUP */				offsetof(MATCHER, m_param.m_group) + sizeof(struct tagMATCH_GROUP),
	/* GROUP_END */			3,
	/* ALTERNATIVE */		2,
	/* DIGIT_CCLASS */		2,
	/* WORD_CCLASS */		2,
	/* END_OF_MATCH */		1,
	/* NON_WHITE_SPACE_CCLASS */ 2,
	/* HEADER */			2
};

static int matcherSimpleLength[HEADER + 1] = {
	/*START_OF_LINE*/		0,
	/*END_OF_LINE */		0,
	/*START_OF_IDENTIFIER*/	0,
	/*END_OF_IDENTIFIER*/	0,
	/* ANY_CHAR */			1,
	/* STRING */			-1,			// dynamic
	/* CHAR_CLASS */		1,
	/* SINGLE_CHAR */		1,
	/* CASE_IGNORE_CHAR*/	1,
	/* WHITE_SPACE_CCLASS */ 1,
	/* BACK_REFERENCE */	255,
	/* GROUP */				255,
	/* GROUP_END */			0,
	/* ALTERNATIVE */		255,
	/* DIGIT_CCLASS */		1,
	/* WORD_CCLASS */		1,
	/* END_OF_MATCH */		0,
	/* NON_WHITE_SPACE_CCLASS*/ 1,
	/* HEADER */			0
};

extern unsigned char* tlcompile(unsigned char* transtab, unsigned char* t, unsigned char* wt);
static unsigned char* regex_advance(unsigned char* pszBeginOfLine, unsigned char* stringToMatch, unsigned char* endOfStringToMatch, 
	unsigned char* pExpression, unsigned char* pExpressionEnd, RE_MATCH* pMatch);
extern unsigned char _l2uset[256], _u2lset[256];
extern unsigned char bittab[];

/*--------------------------------------------------------------------------
 * createTranslationTable()
 * init the character translation table.
 */
static unsigned char* createTranslationTable(int tl) {
	unsigned char* t;

	if (tl >= DIM(_transtabs)) {
		return (unsigned char*)NULL;
	}
	return ((t = _transtabs[tl]) != 0) ?
		t
		:
		(_transtabs[tl] = malloc(256));
}

/*
 * Returns the actual first matcher segment from a pattern.
 */
static MATCHER* regex_getFirstMatchSection(RE_PATTERN* pPattern) {
	MATCHER* pMatcher = (MATCHER*)pPattern->compiledExpression;
	if (pMatcher->m_type == HEADER) {
		return (MATCHER*)(pPattern->compiledExpression + matcherSizes[HEADER]);
	}
	return pMatcher;
}

/*
 * Convert the character at idx in the destination string to upper or lower -
 * if it has not the correct spelling.
 */
static void _convert(char* pDestination, int idx, int upper, int lower) {
	char c1 = pDestination[idx];
	if (upper) {
		if (!isupper(c1)) {
			pDestination[idx] = toupper(c1);
		}
	}
	else {
		if (!islower(c1)) {
			pDestination[idx] = tolower(c1);
		}
	}
}

/**
 * Implements the "preserve case" functionality.
 *
 */
static void adaptCase(char* pDestination, RE_MATCH* pMatch) {
	char* matched = pMatch->loc1;
	int matchedSize = (int)(pMatch->loc2 - pMatch->loc1);
	int matchIdx = 0;
	int upper = 0;
	int lower = 0;
	for (int i = 0; pDestination[i]; i++) {
		unsigned char c1 = pDestination[i];
		unsigned char c2 = matched[matchIdx];
		if (!isalpha(c1) && !isalpha(c2)) {
			while (!isalpha(c1) && c1) {
				c1 = pDestination[i++];
			}
			while (!isalpha(c2) && matchIdx < matchedSize) {
				c2 = matched[matchIdx++];
			}
			if (!c1) {
				break;
			}
			i--;
			matchIdx--;
		}
		if (isalpha(c2)) {
			if (matchIdx < matchedSize) {
				matchIdx++;
				upper = 0;
				lower = 0;
			}
			if (isupper(c2)) {
				upper = 1;
			}
			else if (islower(c2)) {
				lower = 1;
			}
		}
		_convert(pDestination, i, upper, lower);
	}
}

/*--------------------------------------------------------------------------
 * regex_replaceSearchString()
 * create replace-target for replaced Expressions with \
 * return length of target. Before calling this method, you must once call
 * find_initializeReplaceByExpressionOptions.
 * Supports special escape syntax in replace pattern:
 * - \1 (\2,...\9)  - insert capture groups of the regular expression searched 1-9
 * - \& - insert the whole string found
 * - \0 - insert a 0 character
 * - \l - all successing characters in the replacement string are converted to lower case until the next case modifier is encountered
 * - \u - all successing characters in the replacement string are converted to upper case until the next case modifier is encountered
 * - \e - mark the end of a \l or a \u section and do not convert any more characters after this to lower or upper case
 * Returns the length of the resulting string or -1, if an error occurs (target string would be too long etc...)
 */
int regex_replaceSearchString(REPLACEMENT_PATTERN* pPattern, unsigned char* pDestination, int destinationBufferSize, RE_MATCH* pMatch) {
	int len = 0;
	unsigned char* q, * qend, c, * trans = _characterMappingTable, tl = 0;
	unsigned char* replacePattern = pPattern->preparedReplacementString;

	if (!pPattern->specialProcessingNeeded) {
		size_t len = strlen(pPattern->preparedReplacementString);
		if (len >= destinationBufferSize) {
			return -1;
		}
		strcpy(pDestination, pPattern->preparedReplacementString);
		if (pPattern->preserveCaseConversionNeeded) {
			adaptCase(pDestination, pMatch);
		}
		return (int)len;
	}
	destinationBufferSize -= 2;
	while (*replacePattern) {
		if (*replacePattern == '\\') {
			c = replacePattern[1];
			switch (c) {
			case '&':
				q = pMatch->loc1;
				qend = pMatch->loc2;
				break;
			case '[':
				trans = _transtabs[tl++];
				goto nocpy;
			case 'e':
				trans = _characterMappingTable;
				goto nocpy;
			case '0':
				*pDestination++ = 0;
				len++;
				goto nocpy;
			default:
				if (c >= '1' && c <= '9') {
					c -= '1';
					q = pMatch->braslist[c];
					qend = pMatch->braelist[c];
				}
				else goto normal;
				break;
			}
			len += (int)(qend - q);
			if (len > destinationBufferSize)
				return -1;
			while (q < qend)
				*pDestination++ = trans[*q++];
		nocpy:
			replacePattern += 2;
			continue;
		}
	normal:
		if (len++ > destinationBufferSize) return -1;
		*pDestination++ = trans[*replacePattern++];
	}
	*pDestination = 0;
	if (pPattern->preserveCaseConversionNeeded) {
		adaptCase(pDestination, pMatch);
	}
	return len;
}

static int regex_getOccurrence(char size) {
	return size < 0 ? BIG : size;
}

/*--------------------------------------------------------------------------
 * regex_addCharacterToCharacterClass()
 */
static void regex_addCharacterToCharacterClass(unsigned char* pCharTable, unsigned char c, int flags) {
	int c2;

	if (flags & RE_IGNCASE) {
		c2 = upcase(c);
		pCharTable[c2 >> 3] |= bittab[c2 & 07];
		c = lowcase(c);
	}
	pCharTable[c >> 3] |= bittab[c & 07];
}

/*--------------------------------------------------------------------------
 * regex_compileCharacterClasses()
 * Compile a "lower to upper" character class pattern for subsequent use in regular
 * expressions. a lower to upper character class mapping has the form lowerCharRange=upperCharRange,
 * where a charRange may be defined like a regular expression character class. E.g a-z=A-Z will
 * map all lower case alpha chars to all corresponding upper characters. The resulting character
 * class will be used in PKS Edit for lower / upper case conversions as well to define the valid
 * characters of an identifier used during "word" navigation.
 */
void regex_compileCharacterClasses(unsigned char* pLowerToUpperPattern) {
	int i, j;

	if (_chsetinited && pLowerToUpperPattern == NULL) {
		return;
	}
	_chsetinited = 1;
	unsigned char* pRegexSpecial = "[{*.?(<^$+|";
	memset(_reSpecialChars, 0, sizeof _reSpecialChars);
	while (*pRegexSpecial) {
		regex_addCharacterToCharacterClass(_reSpecialChars, *pRegexSpecial++, 0);
	}
	if (!pLowerToUpperPattern) {
		pLowerToUpperPattern = "a-z=A-Z_";
	}

	for (i = 0; i < 256; i++) {
		_asciitab[i] &= (~_C);
	}

	tlcompile(_l2uset, pLowerToUpperPattern, _asciitab);

	/* calculate the inverse tab */

	memset(_u2lset, 0, sizeof(_u2lset));

	for (i = 0; i < sizeof(_l2uset); i++)
		if ((j = _l2uset[i]) != i)
			_u2lset[j] = i;
	for (i = 0; i < sizeof(_u2lset); i++)
		if (_u2lset[i] == 0)
			_u2lset[i] = i;
}

/*--------------------------------------------------------------------------
 * regex_parseOctalNumber()
 */
unsigned char* _octalloc;
int regex_parseOctalNumber(register unsigned char* s)
{
	register unsigned char c, i;

	switch (c = *s++) {
	case 'n':	i = '\n';	break;
	case 't':	i = '\t';	break;
	case 'r':	i = '\r'; break;
	case '0':  	i = 0;
		/* maximum 3 digits \0ddd */
		if ((c = *s++) <= '7' && c >= '0') {
			i <<= 3; i += (c - '0');
			if ((c = *s++) <= '7' && c >= '0') {
				i <<= 3; i += (c - '0');
				if ((c = *s++) <= '7' && c >= '0') {
					s++;	i <<= 3; i += (c - '0');
				}
			}
		}
		s--;
		break;
	default:	i = c;
	}
	_octalloc = s;
	return i;
}

/*
 * Create a matcher for a single character. 
 */
static char* regex_compileSingleChar(MATCHER* pPattern, int options, char c) {
	if (options & RE_IGNCASE) {
		char c1 = makeUpperCase(c);
		char c2 = makeLowerCase(c);
		if (c1 == c2) {
			pPattern->m_param.m_char = c1;
			pPattern->m_type = SINGLE_CHAR;
		} else {
			pPattern->m_type = CASE_IGNORE_CHAR;
			pPattern->m_param.m_caseChar.m_c1 = c1;
			pPattern->m_param.m_caseChar.m_c2 = c2;
		}
	} else {
		pPattern->m_param.m_char = c;
		pPattern->m_type = SINGLE_CHAR;
	}
	return ((char*)pPattern) + matcherSizes[pPattern->m_type];
}

/*
 * Calculate the offset in the matcher to skip, when traversing match patterns.
 */
static int regex_expressionOffset(MATCHER* pMatcher, RE_MATCH* pResult) {
	if (pMatcher->m_type == GROUP) {
		if (!pMatcher->m_param.m_group.m_nonCapturing && pMatcher->m_param.m_group.m_offsetNext == 0 && 
				(pResult && pResult->braelist[pMatcher->m_param.m_group.m_bracketNumber] == 0)) {
			return matcherSizes[GROUP] + sizeof(MATCH_RANGE);
		}
		return pMatcher->m_param.m_group.m_groupEnd;
	}
	if (pMatcher->m_type == STRING) {
		return pMatcher->m_param.m_string.m_length + 2;
	}
	int tDelta = matcherSizes[pMatcher->m_type];
	if (pMatcher->m_range) {
		tDelta += sizeof(MATCH_RANGE);
	}
	return tDelta;
}

/*
 * Calculate the minimum numbers of character matched by the following sequence of expressions.
 * Only the most simple cases are handled - others will return a high number. This method is
 * used for optimization purpose to limit the number of attempts necessary to
 */
static int regex_calculateMinMatchLen(unsigned char* pExpression, unsigned char* pExpressionEnd) {
	MATCHER* pMatcher;
	int nLen = 0;
	int nDelta;
	while (pExpression < pExpressionEnd && nLen < 255) {
		pMatcher = (MATCHER*)pExpression;
		if (pMatcher->m_type == GROUP) {
			char* pExpressionEndGroup = pExpression + pMatcher->m_param.m_group.m_groupEnd;
			if (pMatcher->m_param.m_group.m_offsetNext) {
				pExpression = pExpressionEndGroup;
				continue;
			}
			pExpression += matcherSizes[GROUP];
			nDelta = regex_calculateMinMatchLen(pExpression + sizeof(MATCH_RANGE), pExpressionEndGroup);
			if (pMatcher->m_range) {
				MATCH_RANGE* pRange = (MATCH_RANGE*)pExpression;
				nDelta *= pRange->m_minOccurrence;
			}
			pExpression = pExpressionEndGroup;
		} else {
			if (pMatcher->m_type == STRING) {
				nDelta = pMatcher->m_param.m_string.m_length;
				pExpression += nDelta+2;
			}
			else {
				nDelta = matcherSimpleLength[pMatcher->m_type];
				pExpression += matcherSizes[pMatcher->m_type];
			}
			if (nDelta == 255) {
				// dont know
				continue;
			}
			if (pMatcher->m_range) {
				MATCH_RANGE* pRange = (MATCH_RANGE*)pExpression;
				nDelta *= pRange->m_minOccurrence;
				pExpression += sizeof(MATCH_RANGE);
			}
		}
		nLen += nDelta;
	}
	return nLen > 255 ? 255 : nLen;
}

/*--------------------------------------------------------------------------
 * regex_compileShellPattern()
 * precompile shell meta format to a regular expression
 * wildcards are: *, ?
 */
static void regex_compileShellPattern(char* dest, char* src) {
	while (*src) {
		switch (*src) {
		case '?': *dest++ = '.'; break;
		case '*': *dest++ = '.', *dest++ = '*'; break;
		case '.': case '[': case ']': case '$': case '^':
		case '{': case '}': case '(': case ')':
		case '+': case '<': case '>': case '|':
			*dest++ = '\\', *dest++ = *src; break;
		case '\\':
			if (src[1] > '0' && src[1] <= '9' || src[1] == 'T') {
				src++;
			}
			else {
				*dest++ = *src++;
				if (!*src)
					goto out;
			}
			/* drop through */
		default:
			*dest++ = *src;
		}
		src++;
	}
out: *dest = 0;
}

/*
 * Compile a sub-expression of an expression (0 for top-level, > 1 for groups 2,... for nested groups). 
 */
static MATCHER* regex_compileSubExpression(RE_OPTIONS* pOptions, RE_PATTERN* pResult, char* pExprStart, char* pPatternStart, char**pExprEnd, char **pPatternEnd, int level) {
	int err = 0;
	int flags = pOptions->flags;
	char* pPatternRun = pPatternStart;
	unsigned char* pInput = pExprStart;
	MATCH_RANGE* pRange = NULL;
	MATCHER* pLastAlternativeStart = level == 0 ? NULL : (MATCHER *)(pPatternStart - matcherSizes[GROUP] - sizeof(MATCH_RANGE));
	MATCHER* pLastPattern = NULL;
	size_t delta;

	while (1) {
		if (pPatternRun >= pOptions->endOfPatternBuf) {
			REGEX_ERROR(2);
		}
		MATCHER* pMatcher = (MATCHER*)pPatternRun;
		unsigned char c = *pInput++;
		if (c == pOptions->eof) {
			if (level > 0) {
				REGEX_ERROR(6);
			}
			pMatcher->m_type = END_OF_MATCH;
			pResult->compiledExpressionEnd = ((char*) pMatcher)+matcherSizes[END_OF_MATCH];
			*pPatternEnd = pPatternRun + 1;
			*pExprEnd = pInput;
			return (MATCHER*)(pPatternRun);

		}
		// remember last pattern not being a range spec.
		if (c != '?' && c != '{' && c != '*' && c != '+') {
			pLastPattern = pMatcher;
		} else {
			if (pLastPattern == NULL) {
				REGEX_ERROR(5);
			}
			if (!pLastPattern->m_range) {
				if (pLastPattern->m_type == GROUP) {
					pRange = (MATCH_RANGE*)(((char*)pLastPattern)+matcherSizes[GROUP]);
				} else {
					pRange = (MATCH_RANGE*)pPatternRun;
					pPatternRun += sizeof(MATCH_RANGE);
				}
				pLastPattern->m_range = 1;
			} else if (c == '?') {
				// non gridy operator - e.g. {2,5}?
				pRange->m_lazy = 1;
				continue;
			}
		}
		switch (c) {
		case '.':
			pMatcher->m_type = ANY_CHAR;
			pPatternRun += matcherSizes[ANY_CHAR];
			break;

		case '|': {
			if (pLastAlternativeStart == NULL) {
				REGEX_ERROR(9);
			}
			delta = (pPatternRun - (char*)pLastAlternativeStart);
			if (delta == 0 || delta >= 256) {
				REGEX_ERROR(1);
			}
			pLastAlternativeStart->m_param.m_alternative.m_offsetNext = (unsigned char)delta;
			pLastAlternativeStart = pMatcher;
			pMatcher->m_type = ALTERNATIVE;
			pPatternRun += matcherSizes[ALTERNATIVE];
			break;
		}

		case '\\':
			if (*pInput != pOptions->eof) {
				char cNext = *pInput++;
				if (cNext == 's') {
					pMatcher->m_type = WHITE_SPACE_CCLASS;
					pPatternRun += matcherSizes[WHITE_SPACE_CCLASS];
				} else if (cNext == 'S') {
					pMatcher->m_type = NON_WHITE_SPACE_CCLASS;
					pPatternRun += matcherSizes[NON_WHITE_SPACE_CCLASS];
				} else if (cNext == 'd') {
					pMatcher->m_type = DIGIT_CCLASS;
					pPatternRun += matcherSizes[DIGIT_CCLASS];
				} else if (cNext == 'w') {
					pMatcher->m_type = WORD_CCLASS;
					pPatternRun += matcherSizes[WORD_CCLASS];
				} else if (cNext >= '1' && cNext <= '9') {
					cNext -= '1';
					if (cNext > pResult->nbrackets) {
						REGEX_ERROR(9);
					}
					pMatcher->m_type = BACK_REFERENCE;
					pMatcher->m_param.m_reference = cNext;
					pPatternRun += matcherSizes[BACK_REFERENCE];
				} else {
					pPatternRun = regex_compileSingleChar(pMatcher, flags, cNext);
				}
			}
			break;
		case '<':
			pMatcher->m_type = START_OF_IDENTIFIER;
			pPatternRun += matcherSizes[START_OF_IDENTIFIER];
			break;

		case '>':
			pMatcher->m_type = END_OF_IDENTIFIER;
			pPatternRun += matcherSizes[END_OF_IDENTIFIER];
			break;

		case '$':
			if (*pInput == pOptions->eof) {
				pMatcher->m_type = END_OF_LINE;
				pPatternRun += matcherSizes[END_OF_LINE];
			} else {
				pPatternRun = regex_compileSingleChar(pMatcher, flags, c);
			}
			break;

		case '^':
			pMatcher->m_type = START_OF_LINE;
			pPatternRun += matcherSizes[START_OF_LINE];
			break;

		case '[': {
			int lastCharacterForClass = -1;
			int negated = 0;
			if ((char*)(pMatcher + 1) >= pOptions->endOfPatternBuf) {
				REGEX_ERROR(2);
			}
			pMatcher->m_type = CHAR_CLASS;
			memset(pMatcher->m_param.m_characterClass, 0, sizeof(pMatcher->m_param.m_characterClass));
			if ((c = *pInput++) == '^') {
				negated = 1;
				c = *pInput++;
			}
			do {
				if (c == '\0') REGEX_ERROR(4);
				if (c == '\\') {
					c = regex_parseOctalNumber(pInput);
					pInput = _octalloc;
				}
				else if (c == '-' && lastCharacterForClass >= 0) {
					if ((c = *pInput++) == ']') {
						regex_addCharacterToCharacterClass(pMatcher->m_param.m_characterClass, '-', flags);
						break;
					}
					if (c == '\\') {
						c = regex_parseOctalNumber(pInput);
						pInput = _octalloc;
					}
					while (lastCharacterForClass < c) {
						regex_addCharacterToCharacterClass(pMatcher->m_param.m_characterClass, lastCharacterForClass, flags);
						lastCharacterForClass++;
					}
				}
				lastCharacterForClass = c;
				regex_addCharacterToCharacterClass(pMatcher->m_param.m_characterClass, c, flags);
			} while ((c = *pInput++) != ']');
			if (negated) {
				for (int i = 0; i < MAXCTAB; i++) pMatcher->m_param.m_characterClass[i] ^= -1;
				pMatcher->m_param.m_characterClass[0] &= 0xFE;
			}
			pPatternRun += matcherSizes[CHAR_CLASS];
			break;
		}
		case '*':
			pRange->m_minOccurrence = 0;
			pRange->m_maxOccurrence = 255;
			break;

		case '+':
			pRange->m_minOccurrence = 1;
			pRange->m_maxOccurrence = 255;
			break;

		case '?':
			pRange->m_minOccurrence = 0;
			pRange->m_maxOccurrence = 1;
			break;


		case '{': { 
			int nRangeSeen = 0;
			int nNumber = 0;
			pRange->m_minOccurrence = 0;
			while ((c = *pInput++) != '}') {
				if (c == pOptions->eof) {
					REGEX_ERROR(6);
				}
				if (c == ',') {
					pRange->m_maxOccurrence = 0;
					pRange->m_minOccurrence = nNumber;
					nRangeSeen = 1;
					nNumber = 0;
				} else if (c >= '0' && c <= '9') {
					nNumber = nNumber * 10 + (c - '0');
				}
			}
			if (!nRangeSeen) {
				pRange->m_maxOccurrence = pRange->m_minOccurrence = nNumber;
			} else {
				pRange->m_maxOccurrence = nNumber == 0 ? 255 : nNumber;
			}
			pRange->m_lazy = 0;
			if (regex_getOccurrence(pRange->m_minOccurrence) > regex_getOccurrence(pRange->m_maxOccurrence)) {
				REGEX_ERROR(8);
			}
			}
			break;

		case '(':
			pMatcher->m_type = GROUP;
			pLastPattern = pMatcher;
			if (*pInput == '?' && pInput[1] == ':') {
				pInput += 2;
				pMatcher->m_param.m_group.m_nonCapturing = 1;
			} else {
				pMatcher->m_param.m_group.m_bracketNumber = pResult->nbrackets++;
			}
			// always leave space for range spec of groups.
			pPatternRun += matcherSizes[GROUP] + sizeof (MATCH_RANGE);
			if (regex_compileSubExpression(pOptions, pResult, pInput, pPatternRun, &pInput, &pPatternRun, level + 1) == NULL) {
				return 0;
			}
			delta = (size_t)(pPatternRun - (unsigned char*)pMatcher);
			if (delta <= 0 || delta >= 256) {
				// 0-size sub-expression or expression to complicated.
				REGEX_ERROR(1);
			}
			pMatcher->m_param.m_group.m_groupEnd = (unsigned char)delta;
			{
				MATCHER* pPatternGroupEnd = (MATCHER * )(pPatternRun-matcherSizes[GROUP_END]);
				pPatternGroupEnd->m_param.m_groupEnd.m_nonCapturing = pMatcher->m_param.m_group.m_nonCapturing;
				pPatternGroupEnd->m_param.m_groupEnd.m_bracketNumber = pMatcher->m_param.m_group.m_bracketNumber;
			}
			break;
		case ')':
			if (level <= 0) {
				REGEX_ERROR(6);
			}
			pMatcher->m_type = GROUP_END;
			pPatternRun += matcherSizes[GROUP_END];
			*pPatternEnd = pPatternRun;
			*pExprEnd = pInput;
			return pMatcher;
		default: {
			if (!(flags & RE_IGNCASE)) {
				char* pStart = pInput-1;
				char c2;
				while ((c2 = *pStart++) != 0) {
					if (c2 == '\\') {
						c2 = *pStart++;
						if (c2 != '\\' && !ISTHERE(_reSpecialChars, c2)) {
							break;
						}
					} else if (ISTHERE(_reSpecialChars, c2)) {
						break;
					}
				}
				int len = (int)(pStart - pInput)-1;
				if (len >= 2) {
					pMatcher->m_type = STRING;
					pStart = pInput-1;
					c = *pStart;
					int i = 0;
					while (--len >= 0) {
						c2 = *pStart++;
						if (c2 == '\\') {
							c2 = *pStart++;
						}
						pMatcher->m_param.m_string.m_chars[i++] = c2;
					}
					pMatcher->m_param.m_string.m_length = i;
					pPatternRun += regex_expressionOffset(pMatcher, 0);
					pMatcher = (MATCHER*)pPatternRun;
					pInput = pStart;
					break;
				}
			}
			pPatternRun = regex_compileSingleChar(pMatcher, flags, c);
			break;
		}
		}
	}
endcompile:
	pResult->errorCode = _reerrmsg[err];
	return 0;
}

/*
 * Produce a pre-compiled expression, when matching simple strings.
 */
static int regex_compileSimpleStringMatch(RE_OPTIONS* pOptions, RE_PATTERN* pResult, char* pExpression, char* pPatternStart) {
	MATCHER* pMatcher = (MATCHER*)pPatternStart;
	size_t len = strlen(pExpression);
	if (len) {
		if (pOptions->flags & RE_IGNCASE) {
			while (*pExpression) {
				char c = *pExpression++;
				pMatcher = (MATCHER*)regex_compileSingleChar(pMatcher, pOptions->flags, c);
			}
		}
		else {
			pMatcher->m_type = STRING;
			pMatcher->m_param.m_string.m_length = (char)strlen(pExpression);
			memcpy(pMatcher->m_param.m_string.m_chars, pExpression, pMatcher->m_param.m_string.m_length);
			pMatcher = (MATCHER*)(pPatternStart + regex_expressionOffset(pMatcher, 0));
		}
	}
	pMatcher->m_type = END_OF_MATCH;
	pResult->compiledExpressionEnd = ((char*)pMatcher) + matcherSizes[END_OF_MATCH];
	return 1;
}

/*
 * If the regular expression is represented by a simple string (not iterations and character classes etc...),
 * extract this string and place it into the result and return 1.
 */
int regex_getPatternString(char* pResult, RE_PATTERN* pPattern) {
	MATCHER* pMatcher = (MATCHER*)regex_getFirstMatchSection(pPattern);
	if (pMatcher->m_type == STRING) {
		strncpy(pResult, pMatcher->m_param.m_string.m_chars, pMatcher->m_param.m_string.m_length);
		pResult[pMatcher->m_param.m_string.m_length] = 0;
		// TODO: was that it? or is more coming?
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * regex_compile()
 * Compile a regular expression - return 1 - if successful. The pMatch of the 
 * compilation will be stored in the pMatch pattern.
 */
int regex_compile(RE_OPTIONS* pOptions, RE_PATTERN* pResult) {
	MATCHER* pMatcher = (MATCHER*) pOptions->patternBuf;
	char* pInput = pOptions->expression;
	char* pExprEnd = NULL;
	char* pPatternEnd = NULL;
	char* pPatternStart;
	char shellbuf[512];

	regex_compileCharacterClasses((unsigned char*)0);
	if (pOptions->flags & RE_SHELLWILD) {
		regex_compileShellPattern(shellbuf, pInput);
		pInput = shellbuf;
	}
	memset(pResult, 0, sizeof * pResult);
	memset(pOptions->patternBuf, 0, (size_t)(pOptions->endOfPatternBuf-pOptions->patternBuf));
	pResult->compiledExpression = pOptions->patternBuf;
	pMatcher->m_type = HEADER;
	pPatternStart = (char*)pMatcher;
	pPatternStart += matcherSizes[HEADER];
	int ret;
	if ((pOptions->flags & (RE_SHELLWILD | RE_DOREX)) == 0) {
		ret = regex_compileSimpleStringMatch(pOptions, pResult, pInput, pPatternStart);
	} else {
		ret = regex_compileSubExpression(pOptions, pResult, pInput, pPatternStart, &pExprEnd, &pPatternEnd, 0) == NULL ? 0 : 1;
	}
	if (ret) {
		pResult->noAdvance = (pOptions->flags & RE_NOADVANCE) ? 1 : 0;
		pMatcher->m_param.m_header.m_minMatchSize = regex_calculateMinMatchLen(pPatternStart, pPatternEnd);
	}
	return ret;
}

/*
 * Try a single match of the given expression. We handle here all matches possibly combined with a range (multiplication) specifier.
 * Return the pointer to the next position in the input string to match if successful or 0 on failure.
 */
static unsigned char* advanceSubGroup(unsigned char* pszBeginOfLine, unsigned char* stringToMatch, unsigned char* endOfStringToMatch, unsigned char* pExpression, RE_MATCH* pResult) {
	MATCHER* pMatcher;
	unsigned char c;
	
	pMatcher = (MATCHER*)pExpression;
	switch (pMatcher->m_type) {
	case SINGLE_CHAR:
		if (pMatcher->m_param.m_char == *stringToMatch++) {
			return stringToMatch;
		}
		return(0);

	case CASE_IGNORE_CHAR:
		c = *stringToMatch++;
		if (pMatcher->m_param.m_caseChar.m_c1 == c || pMatcher->m_param.m_caseChar.m_c2 == c) {
			return stringToMatch;
		}
		return 0;

	case STRING: {
		char* pString = pMatcher->m_param.m_string.m_chars;
		char* pEnd = stringToMatch + pMatcher->m_param.m_string.m_length;
		if (pEnd <= endOfStringToMatch && memcmp(stringToMatch, pString, pMatcher->m_param.m_string.m_length) == 0) {
			return pEnd;
		}
		return 0;
	}
	case ANY_CHAR:
		if (stringToMatch++ < endOfStringToMatch) {
			return stringToMatch;
		}
		return(0);

	case CHAR_CLASS:
		c = *stringToMatch++;
		if (ISTHERE(pMatcher->m_param.m_characterClass, c)) {
			return stringToMatch;
		}
		return(0);

	case WORD_CCLASS:
		c = *stringToMatch++;
		if (isident(c)) {
			return stringToMatch;
		}
		return(0);

	case DIGIT_CCLASS:
		c = *stringToMatch++;
		if (pks_isdigit(c)) {
			return stringToMatch;
		}
		return(0);

	case WHITE_SPACE_CCLASS:
		c = *stringToMatch++;
		if (pks_isspace(c)) {
			return stringToMatch;
		}
		return(0);

	case NON_WHITE_SPACE_CCLASS:
		c = *stringToMatch++;
		if (!pks_isspace(c)) {
			return stringToMatch;
		}
		return(0);

	case GROUP_END: {
		if (!pMatcher->m_param.m_group.m_nonCapturing) {
			int nGroup = pMatcher->m_param.m_groupEnd.m_bracketNumber;
			pResult->braelist[nGroup] = stringToMatch;
			pResult->nbrackets = nGroup + 1;
			return stringToMatch;
		}
		break;
	}

	case GROUP: {
		unsigned char* pExprEnd = pExpression + pMatcher->m_param.m_group.m_groupEnd;
		unsigned char* pExprStart = pExpression + matcherSizes[GROUP];
		unsigned char* pExprNext = pExpression + pMatcher->m_param.m_group.m_offsetNext;
		unsigned char* pExprStop = pMatcher->m_param.m_group.m_offsetNext ? pExprNext : pExprEnd;
		pExprStart += sizeof(MATCH_RANGE);
		int nGroup = 0;
		if (!pMatcher->m_param.m_group.m_nonCapturing) {
			nGroup = pMatcher->m_param.m_group.m_bracketNumber;
			pResult->braslist[nGroup] = stringToMatch;
			pResult->nbrackets = nGroup;
			if (pMatcher->m_param.m_group.m_offsetNext == 0) {
				return stringToMatch;
			}
		}
		unsigned char* pLongestMatch = 0;
		int matchedLen = 0;
		int nSubGroup = 0;
		while(1) {
			unsigned char* e2;
			e2 = regex_advance(pszBeginOfLine, stringToMatch, endOfStringToMatch, pExprStart, pExprStop, pResult);
			if (e2 > pLongestMatch) {
				pLongestMatch = e2;
				matchedLen = (int)(pLongestMatch - stringToMatch);
			}
			if (pExprStop >= pExprEnd) {
				break;
			}
			MATCHER* pAlt = (MATCHER*)pExprStop;
			pExprStart = ((unsigned char*)pAlt) + matcherSizes[pAlt->m_type];
			pExprStop = pAlt->m_param.m_alternative.m_offsetNext ? (((unsigned char*)pAlt) + pAlt->m_param.m_alternative.m_offsetNext) : pExprEnd;
		}
		if (pLongestMatch && !pMatcher->m_param.m_group.m_nonCapturing) {
			pResult->braelist[nGroup] = pLongestMatch;
			pResult->nbrackets = nGroup + 1;
		}
		return pLongestMatch;
	}

	case BACK_REFERENCE: {
		unsigned char* e2;
		unsigned char* bbeg;
		int nRef = pMatcher->m_param.m_reference;
		bbeg = pResult->braslist[nRef];
		size_t nOffset = pResult->braelist[nRef] - bbeg;
		e2 = &stringToMatch[nOffset];
		if (e2 <= endOfStringToMatch && memcmp(stringToMatch, bbeg, nOffset) == 0) {
			return e2;
		}
		return(0);
	}
	}
	return 0;
}

/*
 * Try the next sub-expression match to match the input string. 
 */
static unsigned char* regex_advance(unsigned char* pBeginOfLine, unsigned char* stringToMatch, unsigned char* endOfStringToMatch, 
			unsigned char* pExpression, unsigned char* pExpressionEnd, RE_MATCH* pMatch) {
	MATCHER* pMatcher;
	unsigned char* newPos;
	unsigned char* nextExpression;
	unsigned char* lastMatch;

	if (pExpression >= pExpressionEnd) {
		return stringToMatch;
	}
	while (stringToMatch <= endOfStringToMatch) {
		pMatcher = (MATCHER*)pExpression;
		if (pMatcher->m_range) {
			MATCH_RANGE* pRange = (MATCH_RANGE*)(pExpression + matcherSizes[pMatcher->m_type]);
			int nLow = regex_getOccurrence(pRange->m_minOccurrence);
			int nHigh = regex_getOccurrence(pRange->m_maxOccurrence);
			nextExpression = pExpression + regex_expressionOffset(pMatcher, pMatch);
			lastMatch = 0;
			char* lastLoc2 = 0;
			char* lastBracket = 0;
			int nBracket = pMatch->nbrackets;
			for (int i = 1; i <= nHigh; i++) {
				if (i >= 2 && pMatcher->m_type == GROUP && !pMatcher->m_param.m_group.m_nonCapturing) {
					char* e2 = pMatch->braelist[pMatcher->m_param.m_group.m_bracketNumber];
					if (e2 == 0) {
						newPos = 0;
					} else {
						nextExpression = pExpression + regex_expressionOffset(pMatcher, pMatch);
						char* bbeg = pMatch->braslist[pMatcher->m_param.m_group.m_bracketNumber];
						size_t nSize = e2 - bbeg;
						e2 = stringToMatch + nSize;
						if (e2 <= endOfStringToMatch && memcmp(stringToMatch, bbeg, nSize) == 0) {
							newPos = e2;
						}
						else {
							newPos = 0;
						}
					}
				} else {
					newPos = advanceSubGroup(pBeginOfLine, stringToMatch, endOfStringToMatch, pExpression, pMatch);
				}
				if (newPos == 0 && nLow == 0 && i == 1) {
					// for ? matches try to advance nevertheless.
					newPos = stringToMatch;
				}
				if (newPos == 0) {
					if (i < nLow) {
						return 0;
					}
					break;
				} else {
					stringToMatch = newPos;
					if (i >= nLow) {
						if (*nextExpression == END_OF_MATCH) {
							pMatch->loc2 = newPos;
						} else {
							newPos = regex_advance(pBeginOfLine, newPos, endOfStringToMatch, nextExpression, pExpressionEnd, pMatch);
							pMatch->braelist[pMatch->nbrackets] = 0;
						}
						if (newPos) {
							lastMatch = newPos;
							lastLoc2 = pMatch->loc2;
							if (nBracket >= 0) {
								lastBracket = pMatch->braelist[nBracket];
							}
							if (pRange->m_lazy) {
								break;
							}
						}
					}
				}
			}
			if (!lastMatch && nLow == 0 && pMatcher->m_type == GROUP) {
				// for ? matches try to advance nevertheless.
				lastMatch = stringToMatch;
				lastLoc2 = stringToMatch;
			}
			if (lastMatch) {
				if (lastLoc2) {
					pMatch->loc2 = lastLoc2;
				}
				if (lastBracket) {
					pMatch->braelist[nBracket] = lastBracket;
				}
			}
			return lastMatch;
		}
		switch (pMatcher->m_type) {
		case END_OF_LINE:
			if (stringToMatch == endOfStringToMatch) {
				break;
			}
			return(0);

		case START_OF_IDENTIFIER:
			if (stringToMatch == pBeginOfLine || (stringToMatch > pBeginOfLine && !isident(stringToMatch[-1]))) {
				break;
			}
			return 0;

		case END_OF_IDENTIFIER:
			if (stringToMatch == endOfStringToMatch || !isident(*stringToMatch)) {
				break;
			}
			return 0;

		case END_OF_MATCH:
			pMatch->loc2 = stringToMatch;
			return stringToMatch;

		default:
			stringToMatch = advanceSubGroup(pBeginOfLine, stringToMatch, endOfStringToMatch, pExpression, pMatch);
			if (stringToMatch == NULL) {
				return NULL;
			}
			break;
		}
		pExpression += regex_expressionOffset(pMatcher, pMatch);
		if (pExpression >= pExpressionEnd) {
			pMatch->loc2 = stringToMatch;
			return stringToMatch;
		}
	}
	return 0;
}

/*
 * Checks, whether the passed first character matches a regular expression.
 * TODO: groups with alternatives are currently not handled correctly.
 */
int regex_matchesFirstChar(RE_PATTERN* pPattern, unsigned char c) {
	RE_MATCH match;
	MATCHER* pMatcher = (MATCHER*)regex_getFirstMatchSection(pPattern);
	if (pMatcher->m_type == START_OF_IDENTIFIER) {
		pMatcher =(MATCHER*)(((unsigned char*)pMatcher) + matcherSizes[pMatcher->m_type]);
	}
	unsigned char buf[1];
	buf[0] = c;
	while (pMatcher->m_type != END_OF_MATCH) {
		if (pMatcher->m_type == START_OF_LINE) {
			pMatcher = (MATCHER*)((unsigned char*)pMatcher + matcherSizes[pMatcher->m_type]);
			continue;
		}
		if (pMatcher->m_type == GROUP) {
			pMatcher = (MATCHER*)((unsigned char*)pMatcher + matcherSizes[pMatcher->m_type] + sizeof(MATCH_RANGE));
			continue;
		}
		if (pMatcher->m_type == STRING) {
			return pMatcher->m_param.m_string.m_chars[0] == c;
		} 
		if (advanceSubGroup(buf, buf, buf + 1, (unsigned char*)pMatcher, &match)) {
			return 1;
		}
		if (!pMatcher->m_range) {
			break;
		}
		MATCH_RANGE* pRange = (MATCH_RANGE*) ((unsigned char*)pMatcher + matcherSizes[pMatcher->m_type]);
		if (pRange->m_minOccurrence > 0) {
			break;
		}
		pMatcher = (MATCHER*) ((unsigned char*)pRange + sizeof(MATCH_RANGE));
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * regex_match()
 * The workhorse of the regular expression matching feature - try to match an input string
 * against a pattern.
 */
int regex_match(RE_PATTERN* pPattern, unsigned char* stringToMatch, unsigned char* endOfStringToMatch, RE_MATCH* pMatch) {
	memset(pMatch, 0, sizeof * pMatch);
	if (endOfStringToMatch == NULL) {
		endOfStringToMatch = stringToMatch + strlen(stringToMatch);
	}
	int minLen = 0;
	MATCHER* pMatcher = (MATCHER*)pPattern->compiledExpression;
	if (pMatcher->m_type == HEADER) {
		minLen = pMatcher->m_param.m_header.m_minMatchSize;
		pMatcher = (MATCHER*)(((char*)pMatcher) + matcherSizes[HEADER]);
		if (minLen > (int)(endOfStringToMatch - stringToMatch)) {
			return 0;
		}
	}
	pMatch->loc1 = stringToMatch;
	if (pMatcher->m_type == START_OF_LINE) {
		pPattern->circf = 1;
		pMatcher = (MATCHER*)(((char*)pMatcher) + matcherSizes[START_OF_LINE]);
	}
	char* pszBegin = pPattern->beginOfLine;
	if (pszBegin == NULL) {
		pszBegin = stringToMatch;
	}
	if (pPattern->circf || pPattern->noAdvance) {
		pMatch->circf = pPattern->circf;
		if (pMatch->circf && stringToMatch > pszBegin) {
			return 0;
		}
		if (regex_advance(pszBegin, stringToMatch, endOfStringToMatch, (unsigned char*)pMatcher, pPattern->compiledExpressionEnd, pMatch)) {
			pMatch->matches = 1;
			return 1;
		}
	} else {
		char* pszMax = minLen == 0 ? endOfStringToMatch : (endOfStringToMatch - minLen + 1);
		char fastC = 0;
		if (pMatcher->m_type == STRING) {
			fastC = pMatcher->m_param.m_string.m_chars[0];
		} else if (pMatcher->m_type == SINGLE_CHAR) {
			fastC = pMatcher->m_param.m_char;
		}
		while (stringToMatch < pszMax) {
			if (fastC && *stringToMatch != fastC) {
				stringToMatch++;
				continue;
			}
			if (regex_advance(pszBegin, stringToMatch, endOfStringToMatch, (unsigned char*)pMatcher, pPattern->compiledExpressionEnd, pMatch)) {
				pMatch->loc1 = stringToMatch;
				pMatch->matches = 1;
				return 1;
			}
			memset(pMatch->braelist, 0, sizeof pMatch->braelist);
			stringToMatch++;
		}
	}
	pMatch->loc1 = NULL;
	return 0;
}

/*
 * Returns the minimum length a string must have to be able tobe matched by a compiled
 * pattern. Note, that the returned size is currently not yet completely correct, as alternatives ( (a|b) ) 
 * are currently not handled.
 */
int regex_getMinimumMatchLength(RE_PATTERN* pPattern) {
	MATCHER* pMatcher = (MATCHER*)pPattern->compiledExpression;
	if (pMatcher->m_type == HEADER) {
		return pMatcher->m_param.m_header.m_minMatchSize;
	}
	return 0;
}

/*-----------
 * Returns the contents of a capturing group found in a match.
 */
CAPTURING_GROUP_RESULT regex_getCapturingGroup(RE_MATCH* pMatch, int nGroup, char* result, int maxResultLen) {
	if (nGroup < 0 || nGroup >= pMatch->nbrackets) {
		return BAD_CAPTURING_GROUP;
	}
	int tSize = (int)(pMatch->braelist[nGroup] - pMatch->braslist[nGroup]);
	if (tSize >= maxResultLen) {
		return LINE_TOO_LONG;
	}
	memcpy(result, pMatch->braslist[nGroup], tSize);
	result[tSize] = 0;
	return SUCCESS;
}

/**
 * Initializes a series of replacements with a given replace bx expression and some options regarding the replacent.
 */
int regex_initializeReplaceByExpressionOptions(REPLACEMENT_OPTIONS* pOptions, REPLACEMENT_PATTERN* pPattern) {
	unsigned char 	c;
	unsigned char* dest;
	unsigned char* trpat;
	unsigned char* tab;
	int 			tl = 0;
	int				i;

	for (i = 0; i < 256; i++) {		/* translation default */
		_characterMappingTable[i] = i;
	}
	pPattern->specialProcessingNeeded = 0;
	pPattern->lineSplittingNeeded = 0;
	pPattern->preserveCaseConversionNeeded = pOptions->flags & RE_PRESERVE_CASE ? 1 : 0;
	pPattern->preparedReplacementString = _replacedResultBuffer;
	pPattern->errorCode = 0;
	char* replaceByExpression = pOptions->replacementPattern;
	if (!(pOptions->flags & (RE_DOREX | RE_PRESERVE_CASE))) {
		strcpy(_replacedResultBuffer, replaceByExpression);
		return 1;
	}

	dest = _replacedResultBuffer;
	while ((c = *replaceByExpression++) != 0) {
		if (c == '\\') {
			switch ((c = *replaceByExpression++)) {
			case 'u':
				trpat = _l2uset;
				goto mktrans;
			case 'l':
				trpat = _u2lset;
			mktrans:
				if ((tab = createTranslationTable(tl++)) == 0) {
					pPattern->errorCode = IDS_MSGREMACRORANGESYNTAX;
					return 0;
				}
				memmove(tab, trpat, 256);
				c = '[';
				goto special;
			case '[':
				if ((tab = createTranslationTable(tl++)) == 0)
					return 0;
				if ((replaceByExpression = tlcompile(tab, replaceByExpression,
					(unsigned char*)0)) == 0) {
					pPattern->errorCode = IDS_MSGREMANYBRACKETS;
					return 0;
				}
			case 'e':
			case '&':
				goto special;
			default:
				if (c >= '1' && c <= '9') {
					if ((c - '0') > pOptions->maxCaptureGroups) {
						pPattern->errorCode = IDS_MSGREPIPEERR;
						return 0;
					}
				}
				else {
					c = regex_parseOctalNumber(replaceByExpression - 1);
					replaceByExpression = _octalloc;
					if (!c)
						c = '0';
					else
						break;
				}
			special: pPattern->specialProcessingNeeded = 1;
				*dest++ = '\\';
				break;
			}
		}
		if (c == pOptions->newlineCharacter) {
			pPattern->lineSplittingNeeded = 1;
		}
		*dest++ = c;
	}
	*dest = 0;
	return 1;
}

