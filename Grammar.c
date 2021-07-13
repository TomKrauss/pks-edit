/*
 * Grammar.c
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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "alloc.h"
#include "regexp.h"
#include "grammar.h"
#include "jsonparser.h"
#include "linkedlist.h"
#include "arraylist.h"
#include "editorfont.h"

#define	DIM(x)		(sizeof(x)/sizeof(x[0]))

typedef struct tagGRAMMAR_PATTERN {
	struct tagGRAMMAR_PATTERN* next;	// linked list of grammar patterns.
	char  name[32];						// The name of the match - should use hierarchical names as for example 'comment.line' or 'keyword.control.java'
										// The names may be "matched" against style selectors specified below
	char* match;						// a simple RE pattern to match this pattern
	char  begin[12];					// mutually exclusive with match, one may define a begin and end marker to match. May span multiple lines
	char  end[12];						// the end marker maybe e.g. '$' to match the end of line. Currently only one multi-line pattern supported.
	ARRAY_LIST* keywords;				// If an array list of keywords exists, these are matched after the pattern has matched.
	char* rePatternBuf;
	char  style[32];					// Name of the style class to use.
	RE_PATTERN* rePattern;
} GRAMMAR_PATTERN;

/*
 * A grammar describes the grammar of a "language" of an input file editable by PKS Edit
 * for purpose like syntax highlighting or section folding etc. Grammars are loaded from
 * files named [scopeName].grammar.json located in the PKS_SYS directory.
 */
typedef struct tagGRAMMAR {
	struct tagGRAMMAR* next;			// Grammars are maintained - like many things in PKS Edit - in a linked list.
	char scopeName[32];					// Unique name of the grammar. One can associate document types with the scope name of a grammar
										// in the document type definition.
	char* folderStartMarker;			// regular expression to define the start of foldable regions.
	char* folderEndMarker;				// regular expression to define the end of foldable regions.
	char* pszStartMarker;				// If a multiline pattern is defined - this is the pointer to the start marker.
	char* pszEndMarker;					// If a multiline pattern is defined - this is the pointer to the end marker.
	GRAMMAR_PATTERN* patterns;			// The patterns defined for this grammar.
	short transitions[256];				// The indices into the state transition table. We allow a maximum of 16 re_patterns to match from the initial state.
										// for each character we allow a maximum of two possibilities to match.
	LEXICAL_STATE lineSpanningState;	// the index of the line spanning state (currently only one supported).
	GRAMMAR_PATTERN* patternsByState[16];
} GRAMMAR;

typedef struct tagGRAMMAR_DEFINITIONS {
	GRAMMAR* gd_grammars;
} GRAMMAR_DEFINITIONS;

static GRAMMAR_DEFINITIONS _grammarDefinitions;

static GRAMMAR* grammar_createGrammar() {
	return calloc(1, sizeof(GRAMMAR));
}

static GRAMMAR_PATTERN* grammar_createGrammarPattern() {
	return calloc(1, sizeof(GRAMMAR_PATTERN));
}

static JSON_MAPPING_RULE _patternRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(GRAMMAR_PATTERN, name), sizeof(((GRAMMAR_PATTERN*)NULL)->name)},
	{	RT_ALLOC_STRING, "match", offsetof(GRAMMAR_PATTERN, match)},
	{	RT_STRING_ARRAY, "keywords", offsetof(GRAMMAR_PATTERN, keywords)},
	{	RT_CHAR_ARRAY, "style", offsetof(GRAMMAR_PATTERN, style), sizeof(((GRAMMAR_PATTERN*)NULL)->style)},
	{	RT_CHAR_ARRAY, "begin", offsetof(GRAMMAR_PATTERN, begin), sizeof(((GRAMMAR_PATTERN*)NULL)->begin)},
	{	RT_CHAR_ARRAY, "end", offsetof(GRAMMAR_PATTERN, end), sizeof(((GRAMMAR_PATTERN*)NULL)->end)},
	{	RT_END}
};

static JSON_MAPPING_RULE _grammarRules[] = {
	{	RT_CHAR_ARRAY, "scopeName", offsetof(GRAMMAR, scopeName), sizeof(((GRAMMAR*)NULL)->scopeName)},
	{	RT_ALLOC_STRING, "folderStartMarker", offsetof(GRAMMAR, folderStartMarker)},
	{	RT_ALLOC_STRING, "folderEndMarker", offsetof(GRAMMAR, folderEndMarker)},
	{	RT_OBJECT_LIST, "patterns", offsetof(GRAMMAR, patterns),
			{.r_t_arrayDescriptor = {grammar_createGrammarPattern, _patternRules}}},
	{	RT_END}
};

static JSON_MAPPING_RULE _grammarDefinitionsRules[] = {
	{	RT_OBJECT_LIST, "grammars", offsetof(GRAMMAR_DEFINITIONS, gd_grammars),
			{.r_t_arrayDescriptor = {grammar_createGrammar, _grammarRules}}},
	{	RT_END}
};

static int grammar_destroyPattern(GRAMMAR_PATTERN* pPattern) {
	if (pPattern->match) {
		free(pPattern->match);
	}
	if (pPattern->rePattern) {
		free(pPattern->rePattern);
	}
	if (pPattern->rePatternBuf) {
		free(pPattern->rePatternBuf);
	}
	if (pPattern->keywords) {
		for (int i = 0; i < arraylist_size(pPattern->keywords); i++) {
			free(arraylist_get(pPattern->keywords, i));
		}
		arraylist_destroy(pPattern->keywords);
	}
	return 1;
}

static int grammar_destroyGrammar(GRAMMAR* pGrammar) {
	ll_destroy((LINKED_LIST**)&pGrammar->patterns, grammar_destroyPattern);
	return 1;
}

/*
 * Destroy all loaded grammars. 
 */
void grammar_destroyAll() {
	if (_grammarDefinitions.gd_grammars) {
		ll_destroy((LINKED_LIST**)&_grammarDefinitions.gd_grammars, grammar_destroyGrammar);
	}
}
  
/*
 * Util to compile a regular expression. 
 */
RE_PATTERN* grammar_compile(GRAMMAR_PATTERN* pGrammarPattern) {
	if (!pGrammarPattern->match[0]) {
		return NULL;
	}
	pGrammarPattern->rePattern = calloc(1, sizeof(RE_PATTERN));
	pGrammarPattern->rePatternBuf = malloc(512);
	RE_OPTIONS options;
	memset(&options, 0, sizeof options);
	options.flags = RE_DOREX;
	options.patternBuf = pGrammarPattern->rePatternBuf;
	options.endOfPatternBuf = pGrammarPattern->rePatternBuf + 512;
	options.expression = pGrammarPattern->match;
	if (!regex_compile(&options, pGrammarPattern->rePattern)) {
		free(pGrammarPattern->rePattern);
		pGrammarPattern->rePattern = NULL;
		free(pGrammarPattern->rePatternBuf);
		pGrammarPattern->rePatternBuf = NULL;
		return NULL;
	}
	pGrammarPattern->rePattern->circf = 1;
	return pGrammarPattern->rePattern;
}

/*
 * Add a "transition" for an input character. When cChar is found in state initial or state unknown,
 * we will use the corresponding index to find the pattern to match to match a token in the input.
 */
static void grammar_addCharTransition(GRAMMAR* pGrammar, unsigned char cChar, LEXICAL_STATE state) {
	short oldState = pGrammar->transitions[cChar];
	if (oldState & 0xFF0000) {
		state <<= 12;
	} else if (oldState & 0xFF00) {
		state <<= 8;
	} else if (oldState & 0xFF) {
		state <<= 4;
	}
	pGrammar->transitions[cChar] = oldState + state;
}

/*
 * Initialize the grammar by constructing the basic knowledge necessary to make a fast
 * parsing of the grammar more simple.
 */
static void grammar_initialize(GRAMMAR* pGrammar) {
	GRAMMAR_PATTERN* pPattern = pGrammar->patterns;
	memset(pGrammar->transitions, 0, sizeof pGrammar->transitions);
	memset(pGrammar->patternsByState, 0, sizeof pGrammar->patternsByState);
	LEXICAL_STATE state = CUSTOM_STATE;
	pGrammar->lineSpanningState = -1;
	while (pPattern && state < DIM(pGrammar->patternsByState)) {
		pGrammar->patternsByState[state] = pPattern;
		if (pPattern->begin[0] && pPattern->end[0]) {
			pGrammar->lineSpanningState = state;
			pGrammar->pszStartMarker = pPattern->begin;
			pGrammar->pszEndMarker = pPattern->end;
			grammar_addCharTransition(pGrammar, pGrammar->pszStartMarker[0], state);
		} else {
			RE_PATTERN* pREPattern = grammar_compile(pPattern);
			if (pREPattern) {
				for (int i = 0; i < 256; i++) {
					if (regex_matchesFirstChar(pREPattern, (unsigned char)i)) {
						grammar_addCharTransition(pGrammar, (unsigned char)i, state);
					}
				}
			}
		}
		pPattern = pPattern->next;
		state++;
	}
}

/*
 * Load a grammar definition file from the PKS_SYS directory. 
 */
static int grammar_loadFromFile(char* pszGrammarName) {
	GRAMMAR_DEFINITIONS definitions;
	char szFileName[50];

	memset(&definitions, 0, sizeof definitions);
	sprintf(szFileName, "%s.grammar.json", pszGrammarName);
	if (json_parse(szFileName, &definitions, _grammarDefinitionsRules)) {
		if (definitions.gd_grammars != NULL) {
			grammar_initialize(definitions.gd_grammars);
			ll_add((LINKED_LIST**)&_grammarDefinitions.gd_grammars, (LINKED_LIST*)definitions.gd_grammars);
		}
		return 1;
	}
	return 0;
}

/*
 * Find a grammar with the given name. If it cannot be found, try to load the corresponding grammar file
 * on the fly.
 */
GRAMMAR* grammar_findNamed(char* pszGrammarName) {
	GRAMMAR* pFirst;
	int bFirstTry = 2;

	while (bFirstTry) {
		pFirst = _grammarDefinitions.gd_grammars;
		while (pFirst != NULL) {
			if (strcmp(pszGrammarName, pFirst->scopeName) == 0) {
				return pFirst;
			}
			pFirst = pFirst->next;
		}
		bFirstTry--;
		if (!grammar_loadFromFile(pszGrammarName)) {
			break;
		}
	}
	return NULL;
}

/*
 * Stupid simple linear search to match a keyword in a list of given keywords. 
 */
static int grammar_matchKeyword(ARRAY_LIST* pKeywords, char* pKey, char* pKeyEnd) {
	if (!pKeywords) {
		return 1;
	}
	size_t nLen = pKeyEnd - pKey;
	char szBuf[100];
	if (nLen > sizeof szBuf - 1) {
		return 0;
	}
	strncpy(szBuf, pKey, nLen);
	szBuf[nLen] = 0;
	size_t nMax = arraylist_size(pKeywords);
	for (int i = 0; i < nMax; i++) {
		if (strcmp(szBuf, arraylist_get(pKeywords, i)) == 0) {
			return 1;
		}
	}
	return 0;
}

/*
 * This method will parse the characters in pszBuf (assumed length is lLength) and determine the lexial
 * tokens defined as LEXICAL_ELEMENTs to be placed in the result buffer. If successful return the state at the
 * end of the line.
 */
int grammar_parse(GRAMMAR* pGrammar, LEXICAL_ELEMENT pResult[MAX_LEXICAL_ELEMENT], LEXICAL_STATE startState, char* pszBuf, size_t lLength, int *pLineSpanningEndDetected) {
	if (pGrammar == 0) {
		return 0;
	}
	LEXICAL_STATE lexicalState = (startState == pGrammar->lineSpanningState || startState == UNKNOWN) ? startState : INITIAL;
	LEXICAL_STATE nextState = lexicalState;
	RE_MATCH match;
	int nElementCount = 0;
	size_t nStateOffset = 0;
	char* pszEnd = pGrammar->pszEndMarker;
	size_t skipSize = 0;
	for (size_t i = 0; i < lLength; ) {
		unsigned char c = pszBuf[i];
		if (lexicalState == pGrammar->lineSpanningState && pszEnd) {
			if (c == pszEnd[0] && strncmp(&pszBuf[i], pszEnd, strlen(pszEnd)) == 0) {
				*pLineSpanningEndDetected = 1;
				if (nElementCount >= MAX_LEXICAL_ELEMENT - 1) {
					return nElementCount;
				}
				i += 2;
				pResult[nElementCount].le_state = lexicalState;
				pResult[nElementCount].le_length = (int)(i - nStateOffset);
				nElementCount++;
				pResult[nElementCount].le_state = INITIAL;
				pResult[nElementCount].le_length = 0;
				nElementCount++;
				nextState = lexicalState = INITIAL;
			} else {
				i++;
				continue;
			}
		} else if (lexicalState == INITIAL || lexicalState == UNKNOWN) {
			short possibleStates = pGrammar->transitions[c];
			if (possibleStates) {
				int matched = 0;
				for (int nStateOption = 0; nStateOption < 4; nStateOption++) {
					LEXICAL_STATE state = possibleStates & 0xF;
					GRAMMAR_PATTERN* pPattern = pGrammar->patternsByState[state];
					if (!pPattern) {
						break;
					}
					if (pPattern->rePattern) {
						pPattern->rePattern->beginOfLine = pszBuf;
						if (regex_match(pPattern->rePattern, &pszBuf[i], &pszBuf[lLength], &match)) {
							if (!grammar_matchKeyword(pPattern->keywords, match.loc1, match.loc2)) {
								i += (int)(match.loc2 - match.loc1-1);
								skipSize = 0;
							} else {
								nextState = state;
								skipSize = match.loc2 - match.loc1;
								matched = 1;
							}
							break;
						}
					} else if (strncmp(&pszBuf[i], pPattern->begin, strlen(pPattern->begin)) == 0) {
						nextState = state;
						skipSize = 0;
						matched = 1;
						break;
					}
					possibleStates >>= 4;
				}
				if (!matched) {
					i++;
					continue;
				}
			} else {
				i++;
				continue;
			}
		} else {
			i++;
		}
		if (nextState != lexicalState) {
			if (nElementCount >= MAX_LEXICAL_ELEMENT - 1) {
				return nElementCount;
			}
			if (i > nStateOffset) {
				pResult[nElementCount].le_state = lexicalState;
				pResult[nElementCount].le_length = (int)(i - nStateOffset);
				nElementCount++;
			}
			lexicalState = nextState;
			if (skipSize > 0) {
				pResult[nElementCount].le_state = lexicalState;
				pResult[nElementCount].le_length = (int)skipSize;
				nElementCount++;
				i += skipSize;
				lexicalState = nextState = INITIAL;
			}
			nStateOffset = i;
		}
	}
	if (lLength > nStateOffset) {
		pResult[nElementCount].le_state = lexicalState;
		pResult[nElementCount].le_length = (int)(lLength - nStateOffset);
		nElementCount++;
	}
	return nElementCount;
}

/*
 * Initialize the style translation table for a given grammar. 
 */
void grammar_initTokenTypeToStyleTable(GRAMMAR* pGrammar, unsigned char tokenTypeToStyleTable[MAX_TOKEN_TYPE]) {
	memset(tokenTypeToStyleTable, FS_NORMAL, sizeof(tokenTypeToStyleTable));
	if (pGrammar == 0) {
		return;
	}
	GRAMMAR_PATTERN* pPattern;
	for (int i = 0; i < DIM(pGrammar->patternsByState); i++) {
		pPattern = pGrammar->patternsByState[i];
		if (pPattern != NULL) {
			FONT_STYLE_CLASS fsClass = pPattern->style[0] ? font_getStyleClassIndexFor(pPattern->style) : FS_NORMAL;
			tokenTypeToStyleTable[i] = fsClass;
		}
	}
}

