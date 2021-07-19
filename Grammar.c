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
#include "hashmap.h"
#include "edctype.h"
#include "regexp.h"
#include "grammar.h"
#include "jsonparser.h"
#include "linkedlist.h"
#include "arraylist.h"
#include "editorfont.h"

#define	HAS_CHAR(ep, c)			(ep[(c & 0xff) >> 3] & bittab[c & 07])
#define PLACE_CHAR(ep, c)		ep[c >> 3] |= bittab[c & 07];

#define	DIM(x)		(sizeof(x)/sizeof(x[0]))

typedef struct tagPATTERN_GROUP {
	struct tagPATTERN_GROUP* next;
	LEXICAL_STATE state;				// The lexical state matched by this subgroup - is calculated from the patternName
	char  patternName[32];				// Name of the pattern representing this group.
} PATTERN_GROUP;

typedef struct tagGRAMMAR_PATTERN {
	struct tagGRAMMAR_PATTERN* next;	// linked list of grammar patterns.
	char  name[32];						// The name of the match - should use hierarchical names as for example 'comment.line' or 'keyword.control.java'
										// The names may be "matched" against style selectors specified below
	char* match;						// a simple RE pattern to match this pattern
	int   wordmatch;					// set to true for patterns defined with <.... - used for optimization.
	PATTERN_GROUP* captures;			// describes the captures to be matched against a pattern. 
	char  begin[12];					// mutually exclusive with match, one may define a begin and end marker to match. May span multiple lines
	char  end[12];						// the end marker maybe e.g. '$' to match the end of line. Currently only one multi-line pattern supported.
	HASHMAP* keywords;					// If an array list of keywords exists, these are matched after the pattern has matched.
	char* rePatternBuf;
	BOOL ignoreCase;					// If matches should be performed in a case ignore way.
	BOOL spansLines;					// true, if this pattern spans multiple lines
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
	char indentNextLinePattern[32];		// A regular expression defining the condition on which the next line should indented.
	char unIndentLinePattern[32];		// A regular expression defining the condition on which the next line should unindented.
	char increaseIndentPattern[32];		// A regular expression defining the condition on which the indent should be increased.
	char decreaseIndentPattern[32];		// A regular expression defining the condition on which the indent should be decreased.
	BRACKET_RULE* highlightBrackets;	// The rule patterns for "highlight" bracket matching.
	GRAMMAR_PATTERN* patterns;			// The patterns defined for this grammar.
	short transitions[256];				// The indices into the state transition table. We allow a maximum of 16 re_patterns to match from the initial state.
										// for each character we allow a maximum of two possibilities to match.
	GRAMMAR_PATTERN* patternsByState[16];
	UCLIST* undercursorActions;			// The list of actions to perform on input (either bracket matching or code template insertion etc...).
} GRAMMAR;

static BRACKET_RULE _defaultBracketRule = {
	NULL,
	"",
	"",
	1,
	-1,
	0,4,-4,0
};

typedef struct tagGRAMMAR_DEFINITIONS {
	GRAMMAR* gd_grammars;
} GRAMMAR_DEFINITIONS;

static GRAMMAR_DEFINITIONS _grammarDefinitions;

static BRACKET_RULE* grammar_createBracketRule() {
	BRACKET_RULE* pRule = calloc(1, sizeof(*pRule));
	memcpy(pRule, &_defaultBracketRule, sizeof _defaultBracketRule);
	return pRule;
}

static GRAMMAR* grammar_createGrammar() {
	return calloc(1, sizeof(GRAMMAR));
}

static GRAMMAR_PATTERN* grammar_createGrammarPattern() {
	return calloc(1, sizeof(GRAMMAR_PATTERN));
}

static PATTERN_GROUP* grammar_createPatternGroup() {
	return calloc(1, sizeof(PATTERN_GROUP));
}

static JSON_MAPPING_RULE _bracketRules[] = {
	{	RT_CHAR_ARRAY, "left", offsetof(BRACKET_RULE, lefthand), sizeof(((BRACKET_RULE*)NULL)->lefthand)},
	{	RT_CHAR_ARRAY, "right", offsetof(BRACKET_RULE, righthand), sizeof(((BRACKET_RULE*)NULL)->righthand)},
	{	RT_END}
};


static JSON_MAPPING_RULE _patternGroupRules[] = {
	{	RT_CHAR_ARRAY, "pattern", offsetof(PATTERN_GROUP, patternName), sizeof(((PATTERN_GROUP*)NULL)->patternName)},
	{	RT_END}
};

static JSON_MAPPING_RULE _patternRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(GRAMMAR_PATTERN, name), sizeof(((GRAMMAR_PATTERN*)NULL)->name)},
	{	RT_ALLOC_STRING, "match", offsetof(GRAMMAR_PATTERN, match)},
	{	RT_OBJECT_LIST, "captures", offsetof(GRAMMAR_PATTERN, captures),
			{.r_t_arrayDescriptor = {grammar_createPatternGroup, _patternGroupRules}}},
	{	RT_SET, "keywords", offsetof(GRAMMAR_PATTERN, keywords)},
	{	RT_FLAG, "ignoreCase", offsetof(GRAMMAR_PATTERN, ignoreCase)},
	{	RT_CHAR_ARRAY, "style", offsetof(GRAMMAR_PATTERN, style), sizeof(((GRAMMAR_PATTERN*)NULL)->style)},
	{	RT_CHAR_ARRAY, "begin", offsetof(GRAMMAR_PATTERN, begin), sizeof(((GRAMMAR_PATTERN*)NULL)->begin)},
	{	RT_CHAR_ARRAY, "end", offsetof(GRAMMAR_PATTERN, end), sizeof(((GRAMMAR_PATTERN*)NULL)->end)},
	{	RT_END}
};

static JSON_MAPPING_RULE _grammarRules[] = {
	{	RT_CHAR_ARRAY, "scopeName", offsetof(GRAMMAR, scopeName), sizeof(((GRAMMAR*)NULL)->scopeName)},
	{	RT_ALLOC_STRING, "folderStartMarker", offsetof(GRAMMAR, folderStartMarker)},
	{	RT_ALLOC_STRING, "folderEndMarker", offsetof(GRAMMAR, folderEndMarker)},
	{	RT_CHAR_ARRAY, "indentNextLinePattern", offsetof(GRAMMAR, indentNextLinePattern), sizeof(((GRAMMAR*)NULL)->indentNextLinePattern)},
	{	RT_CHAR_ARRAY, "unIndentLinePattern", offsetof(GRAMMAR, unIndentLinePattern), sizeof(((GRAMMAR*)NULL)->unIndentLinePattern)},
	{	RT_CHAR_ARRAY, "increaseIndentPattern", offsetof(GRAMMAR, increaseIndentPattern), sizeof(((GRAMMAR*)NULL)->increaseIndentPattern)},
	{	RT_CHAR_ARRAY, "decreaseIndentPattern", offsetof(GRAMMAR, decreaseIndentPattern), sizeof(((GRAMMAR*)NULL)->decreaseIndentPattern)},
	{	RT_OBJECT_LIST, "highlightBrackets", offsetof(GRAMMAR, highlightBrackets),
			{.r_t_arrayDescriptor = {grammar_createBracketRule, _bracketRules}}},
	{	RT_OBJECT_LIST, "patterns", offsetof(GRAMMAR, patterns),
			{.r_t_arrayDescriptor = {grammar_createGrammarPattern, _patternRules}}},
	{	RT_END}
};

static JSON_MAPPING_RULE _grammarDefinitionsRules[] = {
	{	RT_OBJECT_LIST, "grammars", offsetof(GRAMMAR_DEFINITIONS, gd_grammars),
			{.r_t_arrayDescriptor = {grammar_createGrammar, _grammarRules}}},
	{	RT_END}
};

static int grammar_destroyHashEntry(intptr_t key, intptr_t value) {
	free((void*)key);
	return 1;
}

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
		hashmap_destroy(pPattern->keywords, grammar_destroyHashEntry);
	}
	return 1;
}

static int grammar_destroyGrammar(GRAMMAR* pGrammar) {
	ll_destroy((LINKED_LIST**)&pGrammar->patterns, grammar_destroyPattern);
	ll_destroy((LINKED_LIST**)&pGrammar->undercursorActions, NULL);
	ll_destroy((LINKED_LIST**)&pGrammar->highlightBrackets, NULL);
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

struct tagCHAR_LOOKUP {
	char char1Table[32];
	char char2Table[32];
	int singleCharKeywordExists;
};

static void grammar_collectChars(intptr_t k, void* pParam) {
	unsigned char* pKey = (unsigned char*)k;
	struct tagCHAR_LOOKUP * pLookup = pParam;

	PLACE_CHAR(pLookup->char1Table, pKey[0]);
	pKey++;
	if (!*pKey) {
		pLookup->singleCharKeywordExists = 1;
	} else {
		while (*pKey) {
			char c2 = *pKey++;
			PLACE_CHAR(pLookup->char2Table, c2);
		}
	}
}

/*
 * Utility function used to check, whether a char should be added to a char class
 * RE Pattern.
 */
static int grammar_takeChar(unsigned char table[], unsigned char c) {
	return c != '-' && c != '^' && c != ']' && HAS_CHAR(table, c);
}

static unsigned char* grammar_calcCharClassCharacters(unsigned char* pResult, unsigned char table[]) {
	for (int i = 0; i < 256; ) {
		if (grammar_takeChar(table, i)) {
			int j = i++;
			while (i < 256 && grammar_takeChar(table, i)) {
				i++;
			}
			if (i - j > 2) {
				*pResult++ = j;
				*pResult++ = '-';
				*pResult++ = i - 1;
			} else {
				*pResult++ = j;
				i = j + 1;
			}
		}
		else {
			i++;
		}
	}
	// TODO: handle keywords starting with '^' and ']'
	if (HAS_CHAR(table, '-')) {
		*pResult++ = '-';
	}
	return pResult;
}

/**
 * This method exists to make the life of the person defining a grammar file easier and 
 * to make keyword parsing more efficient. When defining keywords, one needs to define the
 * keywords only and this method calculates a regular expression from a list of keywords to
 * be matched, so the keywords are matched.
 */
static void grammar_createPatternFromKeywords(GRAMMAR_PATTERN* pGrammarPattern) {
	struct tagCHAR_LOOKUP lookup;
	memset(&lookup, 0, sizeof lookup);
	hashmap_forEachKey(pGrammarPattern->keywords, grammar_collectChars, &lookup);
	char result[256];
	// Assumption: keywords always start on word boundaries.
	strcpy(result, "<[");
	unsigned char* pResult = grammar_calcCharClassCharacters(result+2, lookup.char1Table);
	*pResult++ = ']';
	*pResult++ = '[';
	pResult = grammar_calcCharClassCharacters(pResult, lookup.char2Table);
	*pResult++ = ']';
	*pResult++ = lookup.singleCharKeywordExists ? '*' : '+';
	*pResult++ = '>';
	*pResult = 0;
	pGrammarPattern->match = _strdup(result);
}

/*
 * Util to compile a regular expression. 
 */
RE_PATTERN* grammar_compile(GRAMMAR_PATTERN* pGrammarPattern) {
	if (!pGrammarPattern->match) {
		if (pGrammarPattern->keywords) {
			grammar_createPatternFromKeywords(pGrammarPattern);
		} else {
			return NULL;
		}
	}
	pGrammarPattern->rePattern = calloc(1, sizeof(RE_PATTERN));
	pGrammarPattern->rePatternBuf = malloc(512);
	RE_OPTIONS options;
	memset(&options, 0, sizeof options);
	options.flags = RE_DOREX|RE_NOADVANCE;
	if (pGrammarPattern->ignoreCase) {
		options.flags |= RE_IGNCASE;
		if (pGrammarPattern->keywords) {
			hashmap_makeCaseIgnore(pGrammarPattern->keywords);
		}
	}
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
	return pGrammarPattern->rePattern;
}

/*
 * Add a "transition" for an input character. When cChar is found in state initial or state unknown,
 * we will use the corresponding index to find the pattern to match to match a token in the input.
 */
static void grammar_addCharTransition(GRAMMAR* pGrammar, unsigned char cChar, LEXICAL_STATE state) {
	short oldState = pGrammar->transitions[cChar];
	if (oldState & 0xF00) {
		state <<= 12;
	} else if (oldState & 0xF0) {
		state <<= 8;
	} else if (oldState & 0xF) {
		state <<= 4;
	}
	pGrammar->transitions[cChar] = oldState | state;
}

/*
 * Calculate the lexical state index corresponding to a match pattern. 
 */
static LEXICAL_STATE grammar_patternIndex(GRAMMAR* pGrammar, char* pszPatternName) {
	for (int i = 0; i < DIM(pGrammar->patternsByState); i++) {
		GRAMMAR_PATTERN* pPattern = pGrammar->patternsByState[i];
		if (pPattern && strcmp(pszPatternName, pPattern->name) == 0) {
			return i;
		}
	}
	return INITIAL;
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
	while (pPattern && state < DIM(pGrammar->patternsByState)) {
		pGrammar->patternsByState[state] = pPattern;
		if (pPattern->begin[0] && pPattern->end[0]) {
			pPattern->spansLines = TRUE;
			grammar_addCharTransition(pGrammar, pPattern->begin[0], state);
		}
		else {
			RE_PATTERN* pREPattern = grammar_compile(pPattern);
			if (pREPattern) {
				for (int i = 0; i < 256; i++) {
					if (regex_matchesFirstChar(pREPattern, (unsigned char)i)) {
						grammar_addCharTransition(pGrammar, (unsigned char)i, state);
					}
				}
				pPattern->wordmatch = regex_matchWordStart(pREPattern);
			}
		}
		pPattern = pPattern->next;
		state++;
	}
	pPattern = pGrammar->patterns;
	while (pPattern && state < DIM(pGrammar->patternsByState)) {
		PATTERN_GROUP* pGroup = pPattern->captures;
		while (pGroup) {
			pGroup->state = grammar_patternIndex(pGrammar, pGroup->patternName);
			pGroup = pGroup->next;
		}
		pPattern = pPattern->next;

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
static int grammar_matchKeyword(HASHMAP* pKeywords, char* pKey, char* pKeyEnd) {
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
	return hashmap_containsKey(pKeywords, (intptr_t)szBuf);
}

/**
 * Adds a new token delta found during tokenization of the input stream.
 */
static int grammar_addDelta(LEXICAL_STATE newState, int nDelta, int nElementCount, LEXICAL_ELEMENT *pResult) {
	if (nElementCount >= MAX_LEXICAL_ELEMENT - 1) {
		return nElementCount;
	}
	if (nDelta < 0) {
		return nElementCount;
	}
	if (nElementCount > 0 && (pResult[nElementCount - 1].le_state == newState || pResult[nElementCount - 1].le_length == 0)) {
		pResult[nElementCount - 1].le_length += nDelta;
		pResult[nElementCount - 1].le_state = newState;
	} else {
		pResult[nElementCount].le_length = nDelta;
		pResult[nElementCount].le_state = newState;
		nElementCount++;
	}
	return nElementCount;
}

static int grammar_addStyledGroups(int nElementCount, RE_MATCH* pMatch, PATTERN_GROUP* pGroup,
			LEXICAL_STATE defaultState, LEXICAL_ELEMENT* pResult) {
	int nBracket = 0;
	char* pPrevious = pMatch->loc1;

	while (pGroup) {
		char* p1 = pMatch->braslist[nBracket];
		char* p2 = pMatch->braelist[nBracket];
		nElementCount = grammar_addDelta(defaultState, (int)(p1 - pPrevious), nElementCount, pResult);
		nElementCount = grammar_addDelta(pGroup->state, (int)(p2 - p1), nElementCount, pResult);
		pPrevious = p2;
		pGroup = pGroup->next;
		nBracket++;
	}
	return nElementCount;
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
	GRAMMAR_PATTERN* pStartPattern = pGrammar->patternsByState[startState];
	BOOL spansLines = pStartPattern && pStartPattern->spansLines;
	LEXICAL_STATE lexicalState = (startState == UNKNOWN || spansLines) ? startState : INITIAL;
	LEXICAL_STATE nextState = lexicalState;
	RE_MATCH match;
	int nElementCount = 0;
	size_t nStateOffset = 0;
	char* pszEnd = spansLines ? pStartPattern->end : NULL;
	for (size_t i = 0; i < lLength; ) {
		unsigned char c = pszBuf[i];
		if (pszEnd) {
			if (c == pszEnd[0] && strncmp(&pszBuf[i], pszEnd, strlen(pszEnd)) == 0) {
				*pLineSpanningEndDetected = 1;
				i += 2;
				nElementCount = grammar_addDelta(lexicalState, (int)(i - nStateOffset), nElementCount, pResult);
				nElementCount = grammar_addDelta(INITIAL, 0, nElementCount, pResult);
				nextState = lexicalState = INITIAL;
				pszEnd = NULL;
			} else {
				i++;
				continue;
			}
		} else if (lexicalState == INITIAL || lexicalState == UNKNOWN) {
			short possibleStates = pGrammar->transitions[c];
			if (possibleStates) {
				int matched = 0;
				while (possibleStates) {
					LEXICAL_STATE state = possibleStates & 0xF;
					GRAMMAR_PATTERN* pPattern = pGrammar->patternsByState[state];
					if (!pPattern) {
						break;
					}
					if (pPattern->wordmatch && i > 0 && pks_isalnum(pszBuf[i - 1])) {
						possibleStates >>= 4;
						continue;
					}
					RE_PATTERN* pRePattern = pPattern->rePattern;
					if (pRePattern) {
						pRePattern->beginOfLine = pszBuf;
						if (regex_match(pRePattern, &pszBuf[i], &pszBuf[lLength], &match)) {
							int delta = (int)(match.loc2 - match.loc1);
							int nNewOffset = (int)(match.loc2 - pszBuf);
							if (pPattern->captures) {
								nElementCount = grammar_addDelta(lexicalState, (int)(i - nStateOffset), nElementCount, pResult);
								nElementCount = grammar_addStyledGroups(nElementCount, &match, pPattern->captures, lexicalState, pResult);
								nStateOffset = nNewOffset;
								i = nNewOffset;
								matched = 1;
								break;
							} else {
								if (grammar_matchKeyword(pPattern->keywords, match.loc1, match.loc2)) {
									nElementCount = grammar_addDelta(lexicalState, (int)(i - nStateOffset), nElementCount, pResult);
									nElementCount = grammar_addDelta(state, delta, nElementCount, pResult);
									nStateOffset = nNewOffset;
									i = nNewOffset;
									matched = 1;
									break;
								}
							}
						}
					} else if (strncmp(&pszBuf[i], pPattern->begin, strlen(pPattern->begin)) == 0) {
						pszEnd = pPattern->end;
						nextState = state;
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
			nElementCount = grammar_addDelta(lexicalState, (int)(i - nStateOffset), nElementCount, pResult);
			lexicalState = nextState;
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

/*
 * Returns all "under cursor actions" for the given grammar.
 */
UCLIST* grammar_getUndercursorActions(GRAMMAR* pGrammar) {
	if (pGrammar == NULL) {
		return 0;
	}
	if (!pGrammar->undercursorActions) {
		BRACKET_RULE* pRule = pGrammar->highlightBrackets;
		while (pRule) {
			UCLIST* pList = (UCLIST*)ll_insert((LINKED_LIST**)&pGrammar->undercursorActions, sizeof(UCLIST));
			pList->action = UA_SHOWMATCH;
			pList->len = 0;
			pList->p.uc_bracket = pRule;
			pRule = pRule->next;
		}
	}
	return pGrammar->undercursorActions;
}
