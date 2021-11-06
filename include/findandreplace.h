#ifndef FINDANDREPLACE_H

#include "regexp.h"

typedef struct tagMARK MARK;

typedef struct tagSEARCH_AND_REPLACE_PARAMETER {
	char	searchPattern[500];		// the actual search pattern
	char	replaceWith[500];		// the replacement string or pattern for replace operations - otherwise empty.
	int		options;				// one of the RE_XXX flags defined in regexp.h
	char	pathlist[512];			// for search in files: a list of pathes (separated by ",") to scan
	char	filenamePattern[50];	// for search in files: filename pattern for files to consider.
	int		fileScanDepth;			// for search in files: the maximum folder depth for traversal -1 means: no limit.
} SEARCH_AND_REPLACE_PARAMETER;

extern SEARCH_AND_REPLACE_PARAMETER _currentSearchAndReplaceParams;

/*--------------------------------------------------------------------------
 * find_regexCompile()
 * Compiles a regular expression passing in a char array into which the appropriate compiled expression is placed (should be big enough - typically 500 bytes
 * the pattern to compile and the RE_... flags as specified in regexp.h
 */
extern RE_PATTERN* find_regexCompile(char* compiledExpression, char* pattern, int flags);

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

/*
 * Move the input focus to the incremental search UI entry field.
 */
extern int find_initiateIncrementalSearch();

/*--------------------------------------------------------------------------
 * find_matchesInFiles()
 * Perform a recursive search in a list of files with a given filename pattern.
 * Parameter defines the details about the string to search.
 */
typedef enum { FIF_SEARCH,  FIF_REPLACE} FIND_IN_FILES_ACTION;
extern int find_matchesInFiles(SEARCH_AND_REPLACE_PARAMETER* pParam, FIND_IN_FILES_ACTION fAction);

/*--------------------------------------------------------------------------
 * find_replaceTabsWithSpaces()
 * flg = 1 : expand TABS to SPACES
 * flg = 0 : comp SPACES to TABS
 */
extern int find_replaceTabsWithSpaces(int scope, int flg);

/*--------------------------------------------------------------------------
 * EdReplaceText()
 * replace, mark, count... lines with RE
 */
#ifdef _WINFO_H

// Find and replace actions
typedef enum {
	REP_REPLACE	= 1,		// replace 
	REP_MARK = 2,			// mark lines with occurence 
	REP_COUNT = 3,			// count occurrences
	REP_FIND = 4			// search only
} REPLACE_TEXT_ACTION;

typedef enum {
	RTR_ERROR,			// an error occurred during find and replace
	RTR_FINISHED,		// all occurances were successfully replaced
	RTR_ALL,			// The user asked to replace all remaining occurrences without confirmation
	RTR_NOT_FOUND,		// the string to replace was not found
	RTR_CANCELLED}		// some occurrances were potentially replaced, but the user cancelled the operation
REPLACE_TEXT_RESULT;

extern REPLACE_TEXT_RESULT EdReplaceText(WINFO* wp, int scope, REPLACE_TEXT_ACTION action);
#endif


#define FINDANDREPLACE_H
#endif
