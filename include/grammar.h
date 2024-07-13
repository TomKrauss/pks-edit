/*
 * grammar.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: definition of grammars of files to be edited. Modelled
 * after (but not exactly like) textmate.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GRAMMAR_H

#ifndef REGEXP_H
#include "regexp.h"
#endif

#define	UA_SHOWMATCH	1
#define	UA_ABBREV		2
#define	UA_BRINDENT		3
#define	UA_UCMACRO		4
#define	UA_TEMPLATE		5

/*
 * A grammar describes the grammar of a "language" of an input file editable by PKS Edit
 * for purpose like syntax highlighting or section folding etc. Grammars are loaded from
 * files named [scopeName].grammar.json located in the PKS_SYS directory.
 */
typedef struct tagGRAMMAR GRAMMAR;

typedef struct tagWINFO WINFO;

typedef enum {
	UNKNOWN = 0,
	INITIAL,
	CUSTOM_STATE
} LEXICAL_STATE;

#define MAX_LEXICAL_ELEMENT 100
#define MAX_TOKEN_TYPE	64

typedef struct tagLEXICAL_ELEMENT {
	LEXICAL_STATE le_state;			// the lexical state determined for the next le_length characaters.
	int le_length;					// the length of the lexical state.
} LEXICAL_ELEMENT;

typedef struct tagUC_MATCH_PATTERN {
	char*	pattern;			// pattern for scanning 
	int		len;				// minimum length of pattern to scan left to the cursor to match 
	BOOL	ignoreCase;			// whether the input is matched ignore case against the under cursor action
	BOOL	regex;				// whether the input is to be evaluated as a regular expression
	int		lexicalContexts;	// if type is UA_ABBREV, a bitwise combination of the lexical context(s) in which the action is performed.
	RE_PATTERN* rePattern;		// The compile lefthand pattern for patterns
	char*	helpText;			// an optional help text to be displayed during code completion.
} UC_MATCH_PATTERN;

typedef struct tagBRACKET_RULE {
	struct tagBRACKET_RULE* next;
	UC_MATCH_PATTERN lefthand;
	UC_MATCH_PATTERN righthand;	 // righthand bracket match definition
	BOOL dynamicMatch;		// Allows for "calculated matches" in constellation where lefthand and righthand are defined via RE the opposite
							// match may be constructed from the results of a submatch such as </?\1> to match the counter part of an XML entity,
							// when the entity match is defined as <([^>]+)> and </([^>]+)> respectively
	char d1, d2;			// delta to add to current bracket level - typically 1 and -1 for left hand and right hand brackets
	char ci1[2];			// automatic bracket indents (look up, down) indent 1-based of previous line and current line
	char ci2[2];			// automatic bracket indents cl2 outdent 1-based of previous line and current line
} BRACKET_RULE;

typedef struct tagINDENT_PATTERN {
	struct tagINDENT_PATTERN* next;
	UC_MATCH_PATTERN pattern;
	BOOL nextLineOnly;		// whether the next line only should be indented
} INDENT_PATTERN;

typedef struct tagUCLIST {
	struct tagUCLIST* next;
	int  	action;						// type of action to perform 
	BOOL	replaceMatch;				// if true, the match is replaced, when inserting a template.
	UC_MATCH_PATTERN uc_pattern;
	union {
		BRACKET_RULE* uc_bracket;		// if type == UA_SHOWMATCH 
		unsigned char* uc_template;		// if type == UA_ABBREV - the template to insert
		unsigned char* uc_macro;		// if type == UA_MACRO - the name of the macro
	} p;
} UCLIST;

typedef struct tagNAVIGATION_PATTERN {
	struct tagNAVIGATION_PATTERN* next;
	char compiler[32];				// Name of navigation type - was historically the name of the compiler producing an output
	char* pattern;					// regular expression to match
	char* filenamePattern;			// An optional filename pattern used to detect the format for parsing build outputs given in the most simple form (e.g. ".dart" or ".java"
	int filenameCapture;			// which capture group (starting with 1) refers to the filename
	int lineNumberCapture;			// which capture group is linenumber
	int commentCapture;				// which capture group is the section describing additional options for navigating - like a search expression
	int tagCapture;					// for navigation patterns describing a CTAGS tag - the capture group for the described tag - method name / type name / ... itself.
	int tagExtensionFields;			// for navigation patterns describing a CTAGS tag - the capture group containing tag extension fields
	char ebuf[ESIZE];				// The buffer for the compiled regular expression.
	RE_PATTERN rePattern;			// The compiled regular expression pattern.
} NAVIGATION_PATTERN;

// Describes the primary ways to comment code in the described language
typedef struct tagCOMMENT_DESCRIPTOR {
	const char* comment_start;		// This contains the 0-terminated string to start a comment - e.g. "/*"
	const char* comment_end;		// If only a block line comment feature is avaible, this contains the 0-terminated string to end it - e.g. "*/"
	const char* comment2_start;		// This may contain an alternate 0-terminated string to start a comment - e.g. "/*"
	const char* comment2_end;		// If only a block line comment feature is avaible, this an alternate 0-terminated string to end it - e.g. "*/"
	const char* comment_single;		// This contains the 0-terminated string to start a single line comment - e.g. "//"
} COMMENT_DESCRIPTOR;

// URL to execute with ShellExec. $1 in the URL is replaced by searched expression
#define TST_HYPERLINK		"url"
// Lookup the expression in the "current ctags tag file"
#define TST_TAGFILE			"tags"
// Invoke the WinHelp API with the given help file (fn) and the search key
#define	TST_HELPFILE		"help"

/*
 * Cross reference specifications for a grammar. 
 */
typedef struct tagTAGSOURCE {
	struct tagTAGSOURCE* next;
	char type[10];		// the way, this tag source is used.
	char*	fn;
} TAGSOURCE;

/*
 * This method will parse the characters in pszBuf (assumed length is lLength) and determine the lexical
 * tokens defined as LEXICAL_ELEMENTs to be placed in the result buffer. If successful return the number of
 * lexical elements actually found.
 */
extern int grammar_parse(GRAMMAR* pGrammar, LEXICAL_ELEMENT pResult[MAX_LEXICAL_ELEMENT], LEXICAL_STATE startState, char* pszBuf, size_t lLength, int* pLineSpanningEndDetected);

/*
 * Destroy all loaded grammars and style selectors.
 */
extern void grammar_destroyAll();

/*
 * Find a grammar with the given name. If it cannot be found, try to load the corresponding grammar file
 * on the fly.
 */
extern GRAMMAR* grammar_findNamed(const char* pszGrammarName);

/*
 * Returns the name of a grammar.
 */
extern const char* grammar_name(GRAMMAR* pGrammar);

/*
 * Initialize the style translation table for a given grammar.
 */
extern void grammar_initTokenTypeToStyleTable(GRAMMAR* pGrammar, unsigned char tokenTypeToStyleTable[MAX_TOKEN_TYPE]);

/*
 * Returns all "under cursor actions" for the given grammar.
 */
extern UCLIST* grammar_getUndercursorActions(GRAMMAR* pGrammar);

/*
 * Returns the list of navigation patterns for a given grammar.
 */
extern NAVIGATION_PATTERN* grammar_getNavigationPatterns(GRAMMAR* pGrammar);

/*
 * Returns the list of tag sources for a grammar.
 */
extern TAGSOURCE* grammar_getTagSources(GRAMMAR* pGrammar);

/*
 * Returns the comment descriptor for the described language in pDescriptor.
 * If a comment info is available this method returns 1, otherwise 0.
 */
extern int grammar_getCommentDescriptor(GRAMMAR* pGrammar, COMMENT_DESCRIPTOR* pDescriptor);

/*
 * Invoked, when the type of the current active editor changes and will define the identifier
 * according to the current documents grammar.
 */
extern void grammar_documentTypeChanged(GRAMMAR* pGrammar);

/*
 * Insert a selected code template 'up'.
 * If 'bReplaceCurrentWord' is TRUE, the currently selected word / identifier close to the
 * cursor is replaced by the inserted template.
 * If 'nReplacedTextLength' > 0, it is assumed, that this is the number of characters left
 * to the caret to be replaced.
 */
extern int template_insertCodeTemplate(WINFO* wp, UCLIST* up, int nReplacedTextLength, BOOL bReplaceCurrentWord);

/*
 * Creates a String Buffer containing an expanded code template. The returned string buffer must be destroyed by the caller.
 * If an error occurs or there is no current window, this will return NULL.
 */
extern char* template_expandCodeTemplateFor(UCLIST* up);

/*
 * Matches a pattern close to the current caret position. Pattern is the pattern of a template to insert (e.g. for ":+1" => "text to insert",
 * the pattern is ":+1". We try to find out, which part of the pattern is "matched" depending on the caret position. If the caret
 * is positioned like this ":1|+" the matched part to return in szIdentifier is ":1". nMatchedSize is the space available in
 * szIdentifier.
 */
extern void template_matchIdentifier(WINFO* wp, char* pPattern, char* szIdentifier, size_t nMatchedSize);

/*
 * Add all suggestions matching 'pszMatch', which can be derived from a grammar by invoking the addCallback.
 * This includes e.g. the keywords defined in a grammar but also "language specific" analysis results of the surrounding
 * file.
 */
#ifdef CODEANALYZER_H
extern void grammar_addSuggestionsMatching(GRAMMAR* pGrammar, int (*fMatch)(char* pszMatch), ANALYZER_CALLBACK cbAnalyzer);
#endif

/*
 * Returns the name of a code analyzer to use to analyse the code of the document with the given grammar
 * for the purpose of calculating suggestions. The returned name (if analysers are defined) are then to be used
 * to aquire a code analyzer from the code analyzer package.
 */
extern char* grammar_getCodeAnalyzer(GRAMMAR* pGrammar);

/*
 * Returns the name of an evaluator to use to evaluate the current selection in the document with the given grammar.
 * The name of the evaluator is used by the Evaluator package.
 */
extern char* grammar_getEvaluator(GRAMMAR* pGrammar);

/*
 * Returns the name of a formatter to use to format the current selection in the document with the given grammar.
 * The name of the formatter is used by the Formatter package.
 */
extern char* grammar_getFormatter(GRAMMAR* pGrammar);

/*
 * Returns true, if this grammar defines patterns spanning multiple lines making
 * parsing a bit more complex in that a window of lines has to be rescanned to detect
 * multi-line patterns.
 */
extern BOOL grammar_hasLineSpans(GRAMMAR* pGrammar);

/*
 * Returns the name of the wysiwyg renderer for the given grammar or 0, if no
 * special renderer is available for wysiwyg rendering.
 */
extern const char* grammar_wysiwygRenderer(GRAMMAR* pGrammar);

/*
 * Returns the name of the default renderer to be used when editing files for the this grammar or 0, 
 * if the default text renderer should be used.
 */
extern const char* grammar_defaultDisplayMode(GRAMMAR* pGrammar);

/*
 * A lexical context used for determining, whether actions such as code completion etc are available.
 * Allows us to perform context sensitive formatting ( "{" treated differently to simply {) and context
 * sensitive code expansion.
 */
typedef enum { LC_START = 0x1, LC_MULTILINE_COMMENT = 0x2, LC_SINGLE_LINE_COMMENT = 0x4, LC_SINGLE_QUOTED_LITERAL = 0x8, LC_MULTI_QUOTED_LITERAL = 0x10 } LEXICAL_CONTEXT;

/*
 * Calculate the delta indentation defined by a line. This is for a C-file or Java-File +1, if
 * the line contains a { or -1 if the line contains a }. Simple comment checking is performed as
 * well. For pascal it is +1 when a line contains "begin" and -1 when the line contains "end".
 */
extern int grammar_getDeltaIndentation(GRAMMAR* pGrammar, LEXICAL_CONTEXT lcStartCtx, const char* pBuf, size_t nLen);

/*
 * Determines the "lexical context" in a buffer at a given position.
 */
extern LEXICAL_CONTEXT grammar_getLexicalContextAt(GRAMMAR* pGrammar, LEXICAL_CONTEXT nStartState, const char* pBuf, size_t nLen, int nOffset);

/*
 * Returns the lexical context matching a lexical state (used by the highlighter).
 */
extern LEXICAL_CONTEXT grammar_getLexicalContextForState(GRAMMAR* pGrammar, LEXICAL_STATE aState);

/*
 * Returns true, if the lexical state is defined by a "multi-line" definition using
 * begin and end - in other words the state may span several lines.
 */
extern BOOL grammar_isMultilineState(GRAMMAR* pGrammar, LEXICAL_STATE aState);

/*
 * Pre-process an undercursor matching pattern condition.
 * This will allocate resources later to destroy with grammar_destroyUCMatchPattern
 */
void grammar_processMatchPattern(UC_MATCH_PATTERN* pPattern, char* pScopeName);

/*
 * Release the resources allocated for a UC_MATCH_PATTERN
 */
extern void grammar_destroyUCMatchPattern(UC_MATCH_PATTERN* pMatchPattern);

/*
 * Returns the pattern name for a given lexical state as defined in the grammar.
 */
extern const char* grammar_getPatternName(GRAMMAR* pGrammar, LEXICAL_STATE aState);

#define GRAMMAR_H
#endif
