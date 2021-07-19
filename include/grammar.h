/*
 * grammar.h
 *
 * PROJEKT: PKS-EDIT for Windows
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

#define	UA_SHOWMATCH	1
#define	UA_ABBREV		2
#define	UA_BRINDENT	3
#define	UA_UCMACRO	4

/*
 * A grammar describes the grammar of a "language" of an input file editable by PKS Edit
 * for purpose like syntax highlighting or section folding etc. Grammars are loaded from
 * files named [scopeName].grammar.json located in the PKS_SYS directory.
 */
typedef struct tagGRAMMAR GRAMMAR;

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

typedef struct tagBRACKET_RULE {
	struct tagBRACKET_RULE* next;
	char lefthand[10];		/* lefthand single word to match */
	char righthand[10];		/* righthand single word to match */
	char d1, d2;			/* delta to add to current bracket level - typically 1 and -1 for left hand and right hand brackets. */
	char ci1[2];			/* automatic bracket indents (look up, down) indent 1-based of previous line and current line */
	char ci2[2];			/* automatic bracket indents cl2 outdent 1-based of previous line and current line */
} BRACKET_RULE;

typedef struct uclist {
	struct uclist* next;
	char	pat[20];		/* pattern for scanning */
	int		len;			/* length of of pattern to scan left to the cursor to match */
	int  	action;			/* type of action to perform */
	union {
		BRACKET_RULE* uc_bracket;	/* if type == UA_SHOWMATCH */
		unsigned char* uc_template;	/* if type == UA_ABBREV - the template to insert */
		unsigned char* uc_macro;	/* if type == UA_MACRO - the name of the macro */
	} p;
} UCLIST;

typedef struct tagNAVIGATION_PATTERN {
	struct tagNAVIGATION_PATTERN* next;
	char compiler[32];				/* Name of navigation type - was historically the name of the compiler producing an output */
	char* pattern;					/* regular expression to match  */
	int filenameCapture;			/* which \? is filename			*/
	int lineNumberCapture;			/* which \? is linenumber		*/
	int commentCapture;				/* which bsl is a comment - interpreted as info to display*/
} NAVIGATION_PATTERN;

/*
 * This method will parse the characters in pszBuf (assumed length is lLength) and determine the lexial
 * tokens defined as LEXICAL_ELEMENTs to be placed in the result buffer. If successful return the number of
 * lexical elements actually found.
 */
extern int grammar_parse(GRAMMAR* pGrammar, LEXICAL_ELEMENT pResult[MAX_LEXICAL_ELEMENT], LEXICAL_STATE starState, char* pszBuf, size_t lLength, int* pLineSpanningEndDetected);

/*
 * Destroy all loaded grammars and style selectors.
 */
extern void grammar_destroyAll();

/*
 * Find a grammar with the given name. If it cannot be found, try to load the corresponding grammar file
 * on the fly.
 */
extern GRAMMAR* grammar_findNamed(char* pszGrammarName);

/*
 * Initialize the style translation table for a given grammar.
 */
extern void grammar_initTokenTypeToStyleTable(GRAMMAR* pGrammar, unsigned char tokenTypeToStyleTable[MAX_TOKEN_TYPE]);

/*
 * Returns all "under cursor actions" for the given grammar.
 */
extern UCLIST* grammar_getUndercursorActions(GRAMMAR* pGrammar);

/*
 * Find an action descriptor to perform given a line buffer and an offset into that buffer. 
 */
extern UCLIST* uc_find(GRAMMAR* pGrammar, char* lineBuffer, int column);

/*
 * Returns the list of navigation patterns for a given grammar.
 */
extern NAVIGATION_PATTERN* grammar_getNavigationPatterns(GRAMMAR* pGrammar);

#define GRAMMAR_H
#endif
