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
extern RE_PATTERN* find_regexCompile(char* compiledExpression, const char* pattern, int flags);

/*--------------------------------------------------------------------------
 * regex_compileWithDefault()
 * Compile a regular expression passed by argument with standard options.
 */
extern RE_PATTERN* regex_compileWithDefault(const char* expression);

/*--------------------------------------------------------------------------
 * find_setCurrentSearchExpression()
 * Remember the last expression searched for by the user.
 */
extern void find_setCurrentSearchExpression(const char* pExpression);

/*--------------------------------------------------------------------------
 * find_initializeReplaceByExpression()
 * Initialize the pattern used as the replace by pattern in find and replace.
 */
extern int find_initializeReplaceByExpression(unsigned const char* replaceByExpression);

#ifdef _WINFO_H
/*--------------------------------------------------------------------------
 * find_expressionInCurrentFile()
 */
extern int find_expressionInCurrentFile(WINFO* wp, int dir, RE_PATTERN* pPattern, int options);

/*--------------------------------------------------------------------------
 * find_expressionAgainInCurrentFile()
 */
extern int find_expressionAgainInCurrentFile(WINFO* wp, int dir);
#endif

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
extern long long find_initiateIncrementalSearch();

/*--------------------------------------------------------------------------
 * find_matchesInFiles()
 * Perform a recursive search in a list of files with a given filename pattern.
 * Parameter defines the details about the string to search.
 */
typedef enum { FIF_SEARCH,  FIF_REPLACE} FIND_IN_FILES_ACTION;
extern int find_matchesInFiles(SEARCH_AND_REPLACE_PARAMETER* pParam, FIND_IN_FILES_ACTION fAction);

/* valid working range types */
typedef enum {
    // an invalid range
    RNG_INVALID = -1,
    // perform once / search once, replace once, ...
    RNG_ONCE = 0,
    // perform in line containing caret
    RNG_LINE = 1,
    // perform in a "chapter"
    RNG_CHAPTER = 2,
    // perform in the currently selected range of text
    RNG_BLOCK = 3,
    // perform from begin of file to the current caret
    RNG_TOCURS = 4,
    // perform from current caret to the end of the file
    RNG_FROMCURS = 5,
    // perform globally in the current file
    RNG_GLOBAL = 6,
    // currently not really used
    RNG_FREE = 7,
    /* the current selection, but for the purpose of applying an operation one a list of lines(e.g.toggle - comment or shift - lines).
     * In this case the last selected line is treated as "not selected", if the column is 0.
     */
     RNG_BLOCK_LINES = 8
} RANGE_TYPE;

/*
 * Option to expand tabs to spaces or vice versa
 */
typedef enum {
    CT_EXPAND = 1,
    CT_COMPRESS = 0,
} CT_OPTION;

/*--------------------------------------------------------------------------
 * find_replaceTabsWithSpaces()
 * flg = 1 : expand TABS to SPACES
 * flg = 0 : comp SPACES to TABS
 */
extern int find_replaceTabsWithSpaces(RANGE_TYPE scope, CT_OPTION flg);

typedef struct tagWINFO WINFO;

/*--------------------------------------------------------------------------
 * find_setTextSelection()
 *
 * Select a range of text in the file identified by fp. For rangeType constants, see above.
 */
extern int find_setTextSelection(WINFO* wp, RANGE_TYPE rangeType, MARK** markstart, MARK** markend);

/*--------------------------------------------------------------------------
 * find_selectRangeWithMarkers()
 */
extern int find_selectRangeWithMarkers(RANGE_TYPE rangeType, MARK** mps, MARK** mpe);

/*--------------------------------------------------------------------------
 * edit_replaceText()
 * replace, mark, count... lines with RE
 */

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

#ifdef _WINFO_H
extern REPLACE_TEXT_RESULT edit_replaceText(WINFO* wp, const char* pszSearchPattern, const char* pszReplaceWith, int nOptions, int scope, REPLACE_TEXT_ACTION action);
#endif


#define FINDANDREPLACE_H
#endif
