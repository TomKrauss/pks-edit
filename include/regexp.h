/*
 * regexp.h
 *
 * PROJECT: PKSEDIT for Windows 
 *
 * purpose: regular expression defines
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 04.02.1991
 */

#ifndef REGEXP_H
#define	REGEXP_H

 /*----- ... --------------*/

#define	RE_DOREX		0x1					// use .{}*?+ and all the stuff
#define	RE_IGNCASE		0x2					// ignore case 
#define	RE_SHELLWILD	0x4					// wildcards are: *?[] 
#define	RE_PRESERVE_CASE	0x8				// applies to replacement only: if set, try to preserve upper lower case spelling of replaced words Dog -> Cat, DOG -> CAT, etc... 
#define RE_IGNORE_BINARY	0x10			// applies to find in files only - if set, do not traverse "binary files". 
#define	RE_NOADVANCE		0x20			// match at the current position passed without trying to find a match later in line.
#define	RE_SEARCH_ONCE		0x40			// applicable only to "find in files" - if set - stop searching after first match
#define RE_APPEND_TO_SEARCH_RESULTS	0x80	// applicable only to "find in files" - if set - append results to existing search result list
#define RE_SEARCH_IN_SEARCH_RESULTS	0x100	// applicable only to "find in files" - if set - consider only files from previous search result list.
#define RE_CONFIRM_REPLACEMENT		0x200	// applicable to replacement only: if set the user is asked before every replacement 
#define RE_CONSIDER_MARKED_LINES	0x400	// whether only marked lines should be considered
#define RE_DEBUG					0x800	// whether debug information should be printed to the console, when matching a pattern.
#define RE_KEEP_CARET				0x1000	// do not move caret during find&replace.
#define	RE_WRAPSCAN 				0x2000	// wrap around to search from the beginning if not found 



#define	NBRA			9		/* maximum number of brackets	*/

/*
 * Result of a matching operation.
 */
typedef struct tagRE_MATCH {
	int		matches;
	int		regerror;
	char*	loc1;
	char*	loc2;
	int		nbrackets;
	char*	braslist[NBRA];
	char*	braelist[NBRA];
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
	int		errorCode;				// the resource ID in case of an error, which can be used for error reporting.
	char*	compiledExpression;
	char*	compiledExpressionEnd;	// points to the end of the compiled expression after the compilation
	const char*	beginOfLine;		// may be set to the beginning of a line to make <> matches more correct.
	int		circf;					// set to true to not advance during matching, but to match only the beginning of the string.
	int		noAdvance;				// set to true to only match exactly at the current position.
	int		debug;					// set to true to print debugging information
	int		nbrackets;
} RE_PATTERN;

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
 * Compile a regular expression passed by argument with standard options. If
 * not successful, return null.
 */
RE_PATTERN* regex_compileWithError(const char* expression);

/*--------------------------------------------------------------------------
 * regex_compileWithDefault()
 * Compile a regular expression passed by argument with standard options.
 */
RE_PATTERN* regex_compileWithDefault(const char* expression);

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
 * Returns the length of the resulting string or -1, if the target string would be too long or -2 if an invalid capturing group is
 * used in the replacement string.
 */
#define REPLACE_ERR_TOO_LONG -1
#define REPLACE_ERR_WRONG_CAPTURING_GROUP -2
extern long regex_replaceSearchString(REPLACEMENT_PATTERN* pPattern, unsigned char* pDestination, long destinationBufferSize, RE_MATCH* pMatch);

/*--------------------------------------------------------------------------
 * regex_compileCharacterClasses()
 * Compile a "lower to upper" character class pattern for subsequent use in regular
 * expressions. a lower to upper character class mapping has the form lowerCharRange=upperCharRange,
 * where a charRange may be defined like a regular expression character class. E.g a-z=A-Z will
 * map all lower case alpha chars to all corresponding upper characters. The resulting character
 * class will be used in PKS Edit for lower / upper case conversions as well to define the valid
 * characters of an identifier used during "word" navigation.
 */
extern void regex_compileCharacterClasses(const unsigned char* pLowerToUpperPattern);

extern unsigned char* _octalloc;
extern int regex_parseOctalNumber(register unsigned char* s);

/*--------------------------------------------------------------------------
 * regex_compile()
 * Compile a regular expression - return 1 - if successful. The result of the
 * compilation will be stored in the result pattern.
 */
extern int regex_compile(RE_OPTIONS* pOptions, RE_PATTERN* pResult);

/*--------------------------------------------------------------------------
 * regex_match()
 * Perform the actual matching.
 */
extern int regex_match(RE_PATTERN* pPattern, const unsigned char* stringToMatch, const unsigned char* endOfStringToMatch, RE_MATCH* result);

/*
 * Returns the minimum length a string must have to be able tobe matched by a compiled
 * pattern. Note, that the returned size is currently not yet completely correct, as alternatives ( (a|b) )
 * are currently not handled.
 */
extern int regex_getMinimumMatchLength(RE_PATTERN* pPattern);

/*
 * If the regular expression is represented by a simple string (not iterations and character classes etc...),
 * extract this string and place it into the result and return 1.
 */
extern int regex_getPatternString(char* pResult, RE_PATTERN* pPattern);

/*
 * Checks, whether the passed first character matches a regular expression.
 */
extern int regex_matchesFirstChar(RE_PATTERN* pPattern, unsigned char c);

/*
 * Checks, whether the pattern starts with a '<' to match words.
 */
extern int regex_matchWordStart(RE_PATTERN* pPattern);

/*
 * Contains the 1st 8 bit values for fast use.
 */
extern unsigned char bittab[8];

#endif

