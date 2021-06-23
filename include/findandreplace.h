#ifndef FINDANDREPLACE_H

#include "regexp.h"

typedef struct tagMARK MARK;

typedef struct tagSEARCH_AND_REPLACE_PARAMETER {
	char	searchPattern[500];
	char	replaceWith[500];	// the replacement string or pattern for replace operations - otherwise empty.
	int		options;			// one of the RE_XXX flags defined in regexp.h
} SEARCH_AND_REPLACE_PARAMETER;

extern SEARCH_AND_REPLACE_PARAMETER _currentSearchAndReplaceParams;

/*--------------------------------------------------------------------------
 * regex_compile()
 * Compiles a regular expression passing in a char array into which the appropriate compiled expression is placed (should be big enough - typically 500 bytes
 * the pattern to compile and the RE_... flags as specified in regexp.h
 */
extern RE_PATTERN* regex_compile(char* compiledExpression, char* pattern, int flags);

/*--------------------------------------------------------------------------
 * regex_compileWithDefault()
 * Compile a regular expression passed by argument with standard options.
 */
extern RE_PATTERN* regex_compileWithDefault(char* expression);

/*--------------------------------------------------------------------------
 * find_setCurrentSearchExpression()
 * Remember the last expression searched for by the user.
 */
extern void find_setCurrentSearchExpression(char* pExpression);

/*--------------------------------------------------------------------------
 * find_initializeReplaceByExpression()
 * Initialize the pattern used as the replace by pattern in find and replace.
 */
extern int find_initializeReplaceByExpression(unsigned char* replaceByExpression);

/*--------------------------------------------------------------------------
 * find_expressionInCurrentFile()
 */
extern int find_expressionInCurrentFile(int dir, RE_PATTERN* pPattern, int options);

/*--------------------------------------------------------------------------
 * find_expressionAgainInCurrentFile()
 */
extern int find_expressionAgainInCurrentFile(dir);

/*
 * Find a string incremental with given options either forward or backward depending on nDirection.
 */
extern int find_incrementally(char* pszString, int nOptions, int nDirection, BOOL bContinue);

/*
 * Start an incremental search operation. This will set the current search position start to the position
 * of the caret in the current file.
 */
extern int find_startIncrementalSearch();

/*--------------------------------------------------------------------------
 * find_matchesInFiles()
 * Perform a recursive pSearchExpression in a list of pates with a given filename pattern.
 */
extern int find_matchesInFiles(char* pPathes, char* pFilenamePattern, char* pSearchExpression, int nOptions, int nMaxRecursion, int bAbortOnFirstMatch);

/*--------------------------------------------------------------------------
 * find_replaceTabsWithSpaces()
 * flg = 1 : expand TABS to SPACES
 * flg = 0 : comp SPACES to TABS
 */
extern int find_replaceTabsWithSpaces(int scope, int flg);

/*--------------------------------------------------------------------------
 * find_selectRangeWithMarkers()
 */
extern int find_selectRangeWithMarkers(int rngdefault, MARK** mps, MARK** mpe);

/*--------------------------------------------------------------------------
 * EdReplaceText()
 * replace, mark, count... lines with RE
 */
extern int EdReplaceText(int scope, int action, int flags);


#define FINDANDREPLACE_H
#endif
