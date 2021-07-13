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

#define GRAMMAR_H
#endif
