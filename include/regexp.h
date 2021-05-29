/*
 * regexp.h
 *
 * PROJECT: PKSEDIT for ATARI TOS
 *
 * purpose: regular expression defines
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 04.02.1991
 *									
 */

#ifndef REGEXP_H
#define	REGEXP_H

#define	RE_DOREX		0x1		/* use .{}*?+ and all the stuff */
#define	RE_IGNCASE		0x2		/* ignore case */
#define	RE_SHELLWILD	0x4		/* wildcards are: *?[] */

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

/*--------------------------------------------------------------------------
 * regex_compileWithDefault()
 * Compile a regular expression passed by argument with standard options.
 */
RE_PATTERN* regex_compileWithDefault(char* expression);

#endif

