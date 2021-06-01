/*
 * regexp.h
 *
 * PROJECT: PKSEDIT for ATARI TOS
 *
 * purpose: regular expression defines
 *
 * (c) Pahlen & Krauß					Author : TOM
 * All Rights Reserved.					created: 04.02.1991
 *									
 */

#ifndef REGEXP_H
#define	REGEXP_H

#define	RE_DOREX		0x1		/* use .{}*?+ and all the stuff */
#define	RE_IGNCASE		0x2		/* ignore case */
#define	RE_SHELLWILD	0x4		/* wildcards are: *?[] */
#define	RE_PRESERVE_CASE	0x8	/* applies to replacement only: if set, try to preserve upper lower case spelling of replaced words Dog -> Cat, DOG -> CAT, etc... */

 /*
  * Result of a matching operation.
  */
typedef struct tagRE_MATCH {
	int		matches;
	int		regerror;
	char* loc1;
	char* loc2;
	int		nbrackets;
	char** braslist;
	char** braelist;
	int		circf;
} RE_MATCH;

typedef struct tagRE_OPTIONS {
	char* expression;			/* the actual expression to compile */
	char* patternBuf;			/* space into which the regular expression is compiled */
	char* endOfPatternBuf;		/* marks the end of the space into which the expression is compiled */
	unsigned char eof;			/* "END OF INPUT" character if any */
	int	flags;					/* some of the RE_CONSTANTS above */
} RE_OPTIONS;

typedef struct tagRE_PATTERN {
	int		errorCode;			// the resource ID in case of an error, which can be used for error reporting.
	char*	compiledExpression;
	int		circf;
	int		nbrackets;
} RE_PATTERN;

extern int compile(RE_OPTIONS* input, RE_PATTERN* result);
extern int step(RE_PATTERN* pPattern, unsigned char *linebuf, unsigned char *endbuf, RE_MATCH *pMatch);

typedef enum enumCAPTURING_GROUP_RESULT {
	SUCCESS = 0,
	LINE_TOO_LONG = -1,
	BAD_CAPTURING_GROUP = -2
} CAPTURING_GROUP_RESULT;

/*-----------
 * Returns the contents of a capturing group found in a match.
 * Returns 0 on success, -1 if the resulting line would be too long or -2 if an illegal capturing group number
 * was specified.
 */
extern CAPTURING_GROUP_RESULT regex_getCapturingGroup(RE_MATCH* pMatch, int nGroup, char* result, int maxResultLen);

/*--------------------------------------------------------------------------
 * regex_compileWithDefault()
 * Compile a regular expression passed by argument with standard options.
 */
RE_PATTERN* regex_compileWithDefault(char* expression);

typedef struct tagREPLACEMENT_OPTIONS {
	char* replacementPattern;
	int   flags;						// currently RE_DOREX and RE_PRESERVE_CASE are supported
	int	  maxCaptureGroups;				// the maximum number of capture groups
	unsigned char newlineCharacter;		// Character representing newline
} REPLACEMENT_OPTIONS;

/*
 * A data structure describing the outcome of a string replacement pre-compilation.
 */
typedef struct tagREPLACEMENT_PATTERN {
	char specialProcessingNeeded : 1;		// cannot simply use the pattern as provided by the user "as is"
	char lineSplittingNeeded : 1;			// after replace - have to split lines
	char preserveCaseConversionNeeded : 1;	// whether the replacement string should preserve lower / upper case of the replaced string.
	int  errorCode;							// anything other then 0 is an error code
	char* preparedReplacementString;		// the prepared string, If no special processing is required, the input string.
} REPLACEMENT_PATTERN;

/**
 * Initializes a series of replacements with a given replace bx expression and some options regarding the replacent.
 */
extern int regex_initializeReplaceByExpressionOptions(REPLACEMENT_OPTIONS *pOptions, REPLACEMENT_PATTERN* pPattern);

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
extern int regex_replaceSearchString(REPLACEMENT_PATTERN* pPattern, unsigned char* pDestination, int destinationBufferSize, RE_MATCH* pMatch);

/*--------------------------------------------------------------------------
 * regex_compileCharacterClasses()
 * Compile a "lower to upper" character class pattern for subsequent use in regular
 * expressions. a lower to upper character class mapping has the form lowerCharRange=upperCharRange,
 * where a charRange may be defined like a regular expression character class. E.g a-z=A-Z will
 * map all lower case alpha chars to all corresponding upper characters. The resulting character
 * class will be used in PKS Edit for lower / upper case conversions as well to define the valid
 * characters of an identifier used during "word" navigation.
 */
extern void regex_compileCharacterClasses(unsigned char* pLowerToUpperPattern);

#endif

