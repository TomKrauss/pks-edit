/*
 * Grammar.c
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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "alloc.h"
#include "errordialogs.h"
#include "hashmap.h"
#include "edctype.h"
#include "regexp.h"
#include "grammar.h"
#include "jsonparser.h"
#include "linkedlist.h"
#include "arraylist.h"
#include "editorfont.h"
#include "stringutil.h"
#include "codeanalyzer.h"
#include "trace.h"

#define	HAS_CHAR(ep, c)			(ep[(c & 0xff) >> 3] & bittab[c & 07])
#define PLACE_CHAR(ep, c)		ep[c >> 3] |= bittab[c & 07];

#define	DIM(x)		(sizeof(x)/sizeof(x[0]))

#define CHARACTER_STATE_SHIFT		(LONGLONG)7l	// 2**CHARACTER_STATE_SHIFT == number of states per start character allowed
#define MAX_STATE					64				// Should be a binary number
#define CHARACTER_STATE_MASK		(LONGLONG)(MAX_STATE-1)

// Code template definition.
typedef struct tagTEMPLATE {
	struct tagTEMPLATE* next;
	// An optional name of a template used for template references and to identify special templates such
	// as `file` or `copyright`.
	char t_name[32];
	UC_MATCH_PATTERN t_pattern;
	unsigned char* t_contents;
	BOOL t_auto;
} TEMPLATE;

typedef struct tagPATTERN_GROUP {
	struct tagPATTERN_GROUP* next;
	LEXICAL_STATE state;				// The lexical state matched by this subgroup - is calculated from the patternName
	char  patternName[32];				// Name of the pattern representing this group.
} PATTERN_GROUP;

typedef struct tagGRAMMAR_PATTERN {
	struct tagGRAMMAR_PATTERN* next;	// linked list of grammar patterns.
	struct tagGRAMMAR_PATTERN* children;// linked list of child patterns. Will be applied if outer pattern is matched.
	char  name[32];						// The name of the match - should use hierarchical names as for example 'comment.line' or 'keyword.control.java'
										// The names may be "matched" against style selectors specified below
	char* match;						// a simple RE pattern to match this pattern
	int   wordmatch;					// set to true for patterns defined with <.... - used for optimization.
	PATTERN_GROUP* captures;			// describes the captures to be matched against a pattern. 
	char  begin[12];					// mutually exclusive with match, one may define a begin and end marker to match. May span multiple lines
	char  end[12];						// the end marker maybe e.g. '$' to match the end of line. Currently only one multi-line pattern supported.
	HASHMAP* keywords;					// If an array list of keywords exists, these are matched after the pattern has matched.
	BOOL keywordsNoIdentifiers;			// Special case: keywords are not delimited by word boundaries.
	BOOL ignoreCase;					// If matches should be performed in a case ignore way.
	BOOL spansLines;					// true, if this pattern spans multiple lines
	char  style[32];					// Name of the style class to use.
	BOOL initialized;					// TRUE, if this pattern had already been initialized.
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
	char description[128];				// The description of the grammar
	char  u2lset[32];					// wordset and u2l ("abc=xyz") 
	char* folderStartMarker;			// regular expression to define the start of foldable regions.
	char* folderEndMarker;				// regular expression to define the end of foldable regions.
	INDENT_PATTERN* increaseIndentPatterns;	// Patterns defining the condition on which the indent should be increased.
	INDENT_PATTERN* decreaseIndentPatterns;	// Patterns defining the condition on which the indent should be decreased.
	BOOL indentPatternsImported;		// If indent patterns where imported from common grammar - this is true.
	char wysiwygRenderer[32];			// The name of the wysiwyg renderer
	char defaultDisplayMode[32];		// The name of the default display mode to use: "hex" or "default" or "wysiwyg"
	BRACKET_RULE* highlightBrackets;	// The rule patterns for "highlight" bracket matching.
	BOOL highlightBracketsImported;		// If highlightBrackets where imported from common grammar - this is true.
	GRAMMAR_PATTERN* patternDefinitions;// The patterns defined directrly in this grammar.
	LONGLONG transitions[256];			// The indices into the state transition table. We allow a maximum of 2 ** (16-6) re_patterns to match from one initial state.
										// for each character we allow a maximum of 6 possibilities to match.
	GRAMMAR_PATTERN* patternsByState[MAX_STATE]; // All patterns applicable in this grammar. Is defined from this grammar and imported grammars. 
										// Max 64 patterns allowed per grammar.
	UCLIST* undercursorActions;			// The list of actions to perform on input (either bracket matching or code template insertion etc...).
	NAVIGATION_PATTERN* navigation;		// The patterns, which can be used to extract hyperlinks to navigate from within a file
	TAGSOURCE* tagSources;				// The list of tag sources to check for cross references
	TEMPLATE* templates;				// The code templates for this grammar.
	char* analyzer;						// Name of a "wellknown" analyzer to use to extract further suggestions from the text of the current document.
	char* evaluator;					// Name of a "wellknown" evaluator, which is able to "execute" the text from the current text selection.
	char* formatter;					// Name of a "wellknown" formatter, which may format pieces of text in the given grammar.
	BOOL hasLineSpanPattern;			// Whether patterns exist, spanning multiple lines.
	COMMENT_DESCRIPTOR commentDescriptor; // The descriptor for the comment patterns.
	BOOL commentDescriptorInitialized;	// whether the comment descriptor had been initialized
	ARRAY_LIST* importedGrammarNames;	// The names of grammars imported or null
} GRAMMAR;

static BRACKET_RULE _defaultBracketRule = {
	NULL,
	{NULL},
	{NULL},
	0,
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
	if (pRule != NULL) {
		memcpy(pRule, &_defaultBracketRule, sizeof _defaultBracketRule);
	}
	return pRule;
}

static JSON_MAPPING_RULE _tagSourceRules[] = {
	{	RT_ALLOC_STRING, "filename", offsetof(TAGSOURCE, fn)},
	{	RT_CHAR_ARRAY, "type", offsetof(TAGSOURCE, type), sizeof(((TAGSOURCE*)NULL)->type)},
	{	RT_END}
};

static JSON_MAPPING_RULE _bracketRules[] = {
	{	RT_ALLOC_STRING, "left", offsetof(BRACKET_RULE, lefthand.pattern)},
	{	RT_ALLOC_STRING, "right", offsetof(BRACKET_RULE, righthand.pattern)},
	{	RT_FLAG, "dynamic-match", offsetof(BRACKET_RULE, dynamicMatch)},
	{	RT_FLAG, "ignore-case", offsetof(BRACKET_RULE, lefthand.ignoreCase), 1},
	{	RT_FLAG, "regex", offsetof(BRACKET_RULE, lefthand.regex), 1},
	{	RT_END}
};

static JSON_MAPPING_RULE _indentPatternRules[] = {
	{	RT_ALLOC_STRING, "pattern", offsetof(INDENT_PATTERN, pattern.pattern)},
	{	RT_FLAG, "ignore-case", offsetof(INDENT_PATTERN, pattern.ignoreCase), 1},
	{	RT_FLAG, "regex", offsetof(INDENT_PATTERN, pattern.regex), 1},
	{	RT_FLAG, "next-line-only", offsetof(INDENT_PATTERN, nextLineOnly), 1},
	{	RT_END}
};


static JSON_MAPPING_RULE _navigationPatternRules[] = {
	{	RT_CHAR_ARRAY, "name",		offsetof(NAVIGATION_PATTERN, compiler), sizeof(((NAVIGATION_PATTERN*)NULL)->compiler)},
	{	RT_ALLOC_STRING, "pattern", offsetof(NAVIGATION_PATTERN, pattern)},
	{	RT_INTEGER, "filenameCapture",	offsetof(NAVIGATION_PATTERN, filenameCapture)},
	{	RT_INTEGER, "lineNumberCapture",	offsetof(NAVIGATION_PATTERN, lineNumberCapture)},
	{	RT_INTEGER, "commentCapture",	offsetof(NAVIGATION_PATTERN, commentCapture)},
	{	RT_END}
};

static JSON_MAPPING_RULE _templateRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(TEMPLATE, t_name), sizeof(((TEMPLATE*)NULL)->t_name)},
	{	RT_ALLOC_STRING, "match", offsetof(TEMPLATE, t_pattern.pattern)},
	{	RT_ALLOC_STRING, "contents", offsetof(TEMPLATE, t_contents)},
	{	RT_ALLOC_STRING, "grammar-context", offsetof(TEMPLATE, t_pattern.grammarContext)},
	{	RT_ALLOC_STRING, "help", offsetof(TEMPLATE, t_pattern.helpText)},
	{	RT_FLAG, "auto-insert", offsetof(TEMPLATE, t_auto), 1},
	{	RT_FLAG, "ignore-case-match", offsetof(TEMPLATE, t_pattern.ignoreCase), 1},
	{	RT_FLAG, "lexical-context-initial", offsetof(TEMPLATE, t_pattern.lexicalContexts), LC_START},
	{	RT_FLAG, "lexical-context-comment", offsetof(TEMPLATE, t_pattern.lexicalContexts), LC_SINGLE_LINE_COMMENT|LC_MULTILINE_COMMENT},
	{	RT_END}
};

static JSON_MAPPING_RULE _patternGroupRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(PATTERN_GROUP, patternName), sizeof(((PATTERN_GROUP*)NULL)->patternName)},
	{	RT_END}
};

/*
 * Returns the styles supported + their respective properties for being used in code completion.
 */
extern JSON_ENUM_VALUE* theme_getStyles();

static JSON_MAPPING_RULE _patternRules[] = {
	{	RT_OBJECT_LIST, "patterns", offsetof(GRAMMAR_PATTERN, children),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _patternRules, .ro_nestedSize = sizeof(GRAMMAR_PATTERN)}}},
	{	RT_CHAR_ARRAY, "name", offsetof(GRAMMAR_PATTERN, name), sizeof(((GRAMMAR_PATTERN*)NULL)->name)},
	{	RT_ALLOC_STRING, "match", offsetof(GRAMMAR_PATTERN, match)},
	{	RT_OBJECT_LIST, "captures", offsetof(GRAMMAR_PATTERN, captures),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _patternGroupRules, .ro_nestedSize = sizeof(PATTERN_GROUP)}}},
	{	RT_SET, "keywords", offsetof(GRAMMAR_PATTERN, keywords)},
	{	RT_FLAG, "keywords-no-identifiers", offsetof(GRAMMAR_PATTERN, keywordsNoIdentifiers), .r_description = "Set this to true, to specify keywords, which are not identifiers."},
	{	RT_FLAG, "ignore-case", offsetof(GRAMMAR_PATTERN, ignoreCase)},
	{	RT_CHAR_ARRAY, "style", offsetof(GRAMMAR_PATTERN, style), sizeof(((GRAMMAR_PATTERN*)NULL)->style), .r_valueProvider = theme_getStyles},
	{	RT_CHAR_ARRAY, "begin", offsetof(GRAMMAR_PATTERN, begin), sizeof(((GRAMMAR_PATTERN*)NULL)->begin)},
	{	RT_CHAR_ARRAY, "end", offsetof(GRAMMAR_PATTERN, end), sizeof(((GRAMMAR_PATTERN*)NULL)->end)},
	{	RT_END}
};

static int grammar_doImport(GRAMMAR* pTargetGrammar, const char* pszScope) {
	if (!grammar_findNamed(pszScope)) {
		error_displayAlertDialog("Cannot find grammar %s imported into %s", pszScope, pTargetGrammar->scopeName);
	}
	if (!pTargetGrammar->importedGrammarNames) {
		pTargetGrammar->importedGrammarNames = arraylist_create(3);
	}
	arraylist_add(pTargetGrammar->importedGrammarNames, _strdup(pszScope));
	return 1;
}

static JSON_MAPPING_RULE _grammarRules[] = {
	{	RT_CHAR_ARRAY, "scopeName", offsetof(GRAMMAR, scopeName), sizeof(((GRAMMAR*)NULL)->scopeName)},
	{	RT_CHAR_ARRAY, "description", offsetof(GRAMMAR, description), sizeof(((GRAMMAR*)NULL)->description)},
	{	RT_STRING_CALLBACK, "import", 0, .r_descriptor = { .r_t_callback = grammar_doImport}, .r_description = "Specify the name of a grammar to be imported into this grammar."},
	{	RT_ALLOC_STRING, "folderStartMarker", offsetof(GRAMMAR, folderStartMarker), .r_description = "Regular expression to define the start of foldable regions."},
	{	RT_ALLOC_STRING, "folderEndMarker", offsetof(GRAMMAR, folderEndMarker), .r_description = "Regular expression to define the end of foldable regions."},
	{	RT_CHAR_ARRAY, "wordCharacterClass", offsetof(GRAMMAR, u2lset), sizeof(((GRAMMAR*)NULL)->u2lset), .r_description = "Specifies the regular expression to describe an identifier in combination with a mapping from upper case to lower case. Example: a-zäöü=A-ZÄÖÜß_0-9"},
	{	RT_OBJECT_LIST, "increaseIndentPatterns", offsetof(GRAMMAR, increaseIndentPatterns),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _indentPatternRules, .ro_nestedSize = sizeof(INDENT_PATTERN)}}},
	{	RT_OBJECT_LIST, "decreaseIndentPatterns", offsetof(GRAMMAR, decreaseIndentPatterns),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _indentPatternRules, .ro_nestedSize = sizeof(INDENT_PATTERN)}}},
	{	RT_CHAR_ARRAY, "defaultDisplayMode", offsetof(GRAMMAR, defaultDisplayMode), sizeof(((GRAMMAR*)NULL)->defaultDisplayMode)},
	{	RT_CHAR_ARRAY, "wysiwygRenderer", offsetof(GRAMMAR, wysiwygRenderer), sizeof(((GRAMMAR*)NULL)->wysiwygRenderer)},
	{	RT_ALLOC_STRING, "analyzer", offsetof(GRAMMAR, analyzer)},
	{	RT_ALLOC_STRING, "evaluator", offsetof(GRAMMAR, evaluator)},
	{	RT_ALLOC_STRING, "formatter", offsetof(GRAMMAR, formatter)},
	{	RT_OBJECT_LIST, "navigation", offsetof(GRAMMAR, navigation),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _navigationPatternRules, .ro_nestedSize = sizeof(NAVIGATION_PATTERN)}}},
	{	RT_OBJECT_LIST, "templates", offsetof(GRAMMAR, templates),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _templateRules, .ro_nestedSize = sizeof(TEMPLATE)}}},
	{	RT_OBJECT_LIST, "highlightBrackets", offsetof(GRAMMAR, highlightBrackets),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _bracketRules, .ro_nestedSize = sizeof(BRACKET_RULE)}}},
	{	RT_OBJECT_LIST, "patterns", offsetof(GRAMMAR, patternDefinitions),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _patternRules, .ro_nestedSize = sizeof(GRAMMAR_PATTERN)}}},
	{	RT_OBJECT_LIST, "tagSources", offsetof(GRAMMAR, tagSources),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _tagSourceRules, .ro_nestedSize = sizeof(TAGSOURCE)}}},
	{	RT_END}
};

static JSON_MAPPING_RULE _grammarDefinitionsRules[] = {
	{	RT_OBJECT_LIST, "grammars", offsetof(GRAMMAR_DEFINITIONS, gd_grammars),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _grammarRules, .ro_nestedSize = sizeof(GRAMMAR)}}},
	{	RT_END}
};

/*
 * Returns the JSON mapping used to parse grammar files.
 */
JSON_MAPPING_RULE* grammar_getJsonMapping() {
	return _grammarDefinitionsRules;
}

static void grammar_destroyREPattern(RE_PATTERN* pREPattern) {
	if (pREPattern) {
		free(pREPattern->compiledExpression);
		free(pREPattern);
	}
}

/*
 * Release the resources allocated for a UC_MATCH_PATTERN
 */
void grammar_destroyUCMatchPattern(UC_MATCH_PATTERN* pMatchPattern) {
	RE_PATTERN* pREPattern = pMatchPattern->rePattern;
	if (pREPattern) {
		grammar_destroyREPattern(pREPattern);
		pMatchPattern->rePattern = NULL;
	}
	free(pMatchPattern->pattern);
	free(pMatchPattern->helpText);
	free(pMatchPattern->grammarContext);
}

static int grammar_destroyPattern(GRAMMAR_PATTERN* pPattern) {
	free(pPattern->match);
	grammar_destroyREPattern(pPattern->rePattern);
	ll_destroy((LINKED_LIST**)&pPattern->children, grammar_destroyPattern);
	ll_destroy((LINKED_LIST**)&pPattern->captures, NULL);
	hashmap_destroySet(pPattern->keywords);
	pPattern->initialized = FALSE;
	return 1;
}

static int grammar_destroyNavigationPattern(NAVIGATION_PATTERN* pPattern) {
	free(pPattern->pattern);
	return 1;
}

static int grammar_destroyTagSource(TAGSOURCE* pSource) {
	free(pSource->fn);
	return 1;
}

static int grammar_destroyTemplates(TEMPLATE* pTemplate) {
	grammar_destroyUCMatchPattern(&pTemplate->t_pattern);
	free(pTemplate->t_contents);
	return 1;
}

static int grammar_destroyBrackets(BRACKET_RULE* pRule) {
	grammar_destroyUCMatchPattern(&pRule->lefthand);
	grammar_destroyUCMatchPattern(&pRule->righthand);
	return 1;
}

static int grammar_destroyIndentation(INDENT_PATTERN* pPattern) {
	grammar_destroyUCMatchPattern(&pPattern->pattern);
	return 1;
}

static int grammar_destroyGrammar(GRAMMAR* pGrammar) {
	ll_destroy((LINKED_LIST**)&pGrammar->patternDefinitions, grammar_destroyPattern);
	ll_destroy((LINKED_LIST**)&pGrammar->undercursorActions, NULL);
	if (!pGrammar->highlightBracketsImported) {
		ll_destroy((LINKED_LIST**)&pGrammar->highlightBrackets, grammar_destroyBrackets);
	}
	if (!pGrammar->indentPatternsImported) {
		ll_destroy((LINKED_LIST**)&pGrammar->increaseIndentPatterns, grammar_destroyIndentation);
		ll_destroy((LINKED_LIST**)&pGrammar->decreaseIndentPatterns, grammar_destroyIndentation);
	}
	ll_destroy((LINKED_LIST**)&pGrammar->navigation, grammar_destroyNavigationPattern);
	ll_destroy((LINKED_LIST**)&pGrammar->tagSources, grammar_destroyTagSource);
	ll_destroy((LINKED_LIST**)&pGrammar->templates, grammar_destroyTemplates);
	arraylist_destroyStringList(pGrammar->importedGrammarNames);
	free(pGrammar->analyzer);
	free(pGrammar->evaluator);
	free(pGrammar->formatter);
	free((void*)pGrammar->commentDescriptor.comment_start);
	free((void*)pGrammar->commentDescriptor.comment2_start);
	free((void*)pGrammar->commentDescriptor.comment_end);
	free((void*)pGrammar->commentDescriptor.comment2_end);
	free((void*)pGrammar->commentDescriptor.comment_single);
	return 1;
}

/*
 * Destroy all loaded grammars. 
 */
void grammar_destroyAll() {
	ll_destroy((LINKED_LIST**)&_grammarDefinitions.gd_grammars, grammar_destroyGrammar);
}

struct tagCHAR_LOOKUP {
	char char1Table[32];
	char char2Table[32];
	BOOL ignoreCase;
	int singleCharKeywordExists;
};

static int grammar_collectChars(intptr_t k, void* pParam) {
	unsigned char* pKey = (unsigned char*)k;
	struct tagCHAR_LOOKUP * pLookup = pParam;

	unsigned char c1 = pKey[0];
	PLACE_CHAR(pLookup->char1Table, c1);
	if (pLookup->ignoreCase) {
		if (pks_islower(c1)) {
			PLACE_CHAR(pLookup->char1Table, toupper(c1));
		} else if (pks_isupper(c1)) {
			PLACE_CHAR(pLookup->char1Table, tolower(c1));
		}
	}
	pKey++;
	if (!*pKey) {
		pLookup->singleCharKeywordExists = 1;
	} else {
		while (*pKey) {
			char c2 = *pKey++;
			PLACE_CHAR(pLookup->char2Table, c2);
			if (pLookup->ignoreCase) {
				if (pks_islower(c2)) {
					PLACE_CHAR(pLookup->char2Table, toupper(c2));
				}
				else if (pks_isupper(c2)) {
					PLACE_CHAR(pLookup->char2Table, tolower(c2));
				}
			}
		}
	}
	return 1;
}

/*
 * Utility function used to check, whether a char should be added to a char class
 * RE Pattern.
 */
static int grammar_takeChar(const unsigned char table[], unsigned char c) {
	return c != '-' && c != '^' && c != ']' && HAS_CHAR(table, c);
}

static unsigned char* grammar_determineCharacterClassCharacters(unsigned char* pResult, unsigned char table[]) {
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

// forward declaration
static int grammar_tokenFound(GRAMMAR* pGrammar, LEXICAL_ELEMENT pResult[MAX_LEXICAL_ELEMENT], LEXICAL_STATE currentState, GRAMMAR_PATTERN* pPattern,
	char* pszBuf, size_t start, size_t end, int nElementCount);

/**
 * This method exists to make the life of the person defining a grammar file easier and 
 * to make keyword parsing more efficient. When defining keywords, one needs to define the
 * keywords only and this method calculates a regular expression from a list of keywords to
 * be matched, so the keywords are matched.
 */
static void grammar_createPatternFromKeywords(GRAMMAR_PATTERN* pGrammarPattern) {
	struct tagCHAR_LOOKUP lookup;
	memset(&lookup, 0, sizeof lookup);
	lookup.ignoreCase = pGrammarPattern->ignoreCase;
	hashmap_forEachKey(pGrammarPattern->keywords, grammar_collectChars, &lookup);
	char result[256];
	strcpy(result, pGrammarPattern->keywordsNoIdentifiers ? "[" : "<[");
	unsigned char* pResult = grammar_determineCharacterClassCharacters(result+strlen(result), lookup.char1Table);
	*pResult++ = ']';
	*pResult++ = '[';
	pResult = grammar_determineCharacterClassCharacters(pResult, lookup.char2Table);
	*pResult++ = ']';
	*pResult++ = lookup.singleCharKeywordExists ? '*' : '+';
	if (!pGrammarPattern->keywordsNoIdentifiers) {
		*pResult++ = '>';
	}
	*pResult = 0;
	pGrammarPattern->match = _strdup(result);
}

static RE_PATTERN* grammar_compileAndCreateRegex(char* pszMatch, char* pszScope, int someFlags) {
	RE_PATTERN *pPattern = calloc(1, sizeof(RE_PATTERN));
	if (pPattern == NULL) {
		return NULL;
	}
	RE_OPTIONS options = {
		.flags = someFlags,
		.patternBuf = malloc(512),
		.expression = pszMatch
	};
	options.endOfPatternBuf = options.patternBuf + 512;
	if (!regex_compile(&options, pPattern)) {
		free(pPattern);
		free(options.patternBuf);
		error_displayAlertDialog("Wrong regular expression %s in grammar", pszMatch, pszScope);
		return NULL;
	}
	size_t nLen = (size_t)(pPattern->compiledExpressionEnd - pPattern->compiledExpression);
	void* pTemp = realloc(pPattern->compiledExpression, nLen);
	if (pTemp != NULL) {
		pPattern->compiledExpression = pTemp;
		pPattern->compiledExpressionEnd = pPattern->compiledExpression + nLen;
	}
	return pPattern;

}
/*
 * Util to compile a regular expression. 
 */
RE_PATTERN* grammar_compile(GRAMMAR* pGrammar, GRAMMAR_PATTERN* pGrammarPattern) {
	if (pGrammarPattern->initialized) {
		return pGrammarPattern->rePattern;
	}
	if (!pGrammarPattern->match) {
		if (pGrammarPattern->keywords) {
			grammar_createPatternFromKeywords(pGrammarPattern);
		} else {
			return NULL;
		}
	}
	int flags = RE_DOREX|RE_NOADVANCE;
	if (pGrammarPattern->ignoreCase) {
		flags |= RE_IGNCASE;
		if (pGrammarPattern->keywords) {
			hashmap_makeCaseIgnore(pGrammarPattern->keywords);
		}
	}
	pGrammarPattern->rePattern = grammar_compileAndCreateRegex(pGrammarPattern->match, pGrammar->scopeName, flags);
	pGrammarPattern->initialized = TRUE;
	return pGrammarPattern->rePattern;
}

/*
 * Add a "transition" for an input character. When cChar is found in state initial or state unknown,
 * we will use the corresponding index to find the pattern to match to match a token in the input.
 */
static void grammar_addCharTransition(GRAMMAR* pGrammar, unsigned char cChar, LEXICAL_STATE state) {
	LONGLONG oldState = pGrammar->transitions[cChar];
	LONGLONG newState = state;
	if (oldState & (CHARACTER_STATE_MASK << (9* CHARACTER_STATE_SHIFT))) {
		error_displayAlertDialog("More than 8 grammar states entered by character '%c'. Grammar '%s' may not work correctly.", cChar, pGrammar->scopeName);
	} else if (oldState & (CHARACTER_STATE_MASK << (8 * CHARACTER_STATE_SHIFT))) {
		newState <<= (8 * CHARACTER_STATE_SHIFT);
	} else if (oldState & (CHARACTER_STATE_MASK << (7 * CHARACTER_STATE_SHIFT))) {
		newState <<= (7 * CHARACTER_STATE_SHIFT);
	} else if (oldState & (CHARACTER_STATE_MASK << (6 * CHARACTER_STATE_SHIFT))) {
		newState <<= (7 * CHARACTER_STATE_SHIFT);
	} else if (oldState & (CHARACTER_STATE_MASK << (5 * CHARACTER_STATE_SHIFT))) {
		newState <<= (6 * CHARACTER_STATE_SHIFT);
	} else if (oldState & (CHARACTER_STATE_MASK << (4* CHARACTER_STATE_SHIFT))) {
		newState <<= (5* CHARACTER_STATE_SHIFT);
	} else if (oldState & (CHARACTER_STATE_MASK << (3* CHARACTER_STATE_SHIFT))) {
		newState <<= (4* CHARACTER_STATE_SHIFT);
	} else if (oldState & (CHARACTER_STATE_MASK << (2* CHARACTER_STATE_SHIFT))) {
		newState <<= (3* CHARACTER_STATE_SHIFT);
	} else if (oldState & (CHARACTER_STATE_MASK << CHARACTER_STATE_SHIFT)) {
		newState <<= (2* CHARACTER_STATE_SHIFT);
	} else if (oldState & CHARACTER_STATE_MASK) {
		newState <<= CHARACTER_STATE_SHIFT;
	}
	pGrammar->transitions[cChar] = oldState | newState;
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
 * Link the capture style states with state indices.
 */
static void grammar_processCaptures(GRAMMAR* pGrammar, GRAMMAR_PATTERN* pPattern, LEXICAL_STATE state) {
	while (pPattern && state < DIM(pGrammar->patternsByState)) {
		PATTERN_GROUP* pGroup = pPattern->captures;
		while (pGroup) {
			pGroup->state = grammar_patternIndex(pGrammar, pGroup->patternName);
			pGroup = pGroup->next;
		}
		if (pPattern->children) {
			grammar_processCaptures(pGrammar, pPattern->children, state);
		}
		pPattern = pPattern->next;
	}
}

/*
 * Prepare the child patterns of a pattern. 
 */
static LEXICAL_STATE grammar_processChildPatterns(GRAMMAR* pGrammar, LEXICAL_STATE stateIdx, GRAMMAR_PATTERN* pPattern) {
	GRAMMAR_PATTERN* pChildPattern = pPattern->children;
	while (pChildPattern) {
		if (stateIdx >= DIM(pGrammar->patternsByState)) {
			return stateIdx;
		}
		if (pChildPattern->begin[0] && pChildPattern->end[0]) {
			pChildPattern->spansLines = TRUE;
			pGrammar->hasLineSpanPattern = TRUE;
		} else {
			RE_PATTERN* pREPattern = grammar_compile(pGrammar, pChildPattern);
			if (pREPattern) {
				pChildPattern->wordmatch = regex_matchWordStart(pREPattern);
			}
		}
		pGrammar->patternsByState[stateIdx++] = pChildPattern;
		stateIdx = grammar_processChildPatterns(pGrammar, stateIdx, pChildPattern);
		pChildPattern = pChildPattern->next;
	}
	return stateIdx;
}

/*
 * Pre-process an undercursor matching pattern condition.
 */
void grammar_processMatchPattern(UC_MATCH_PATTERN* pPattern, char* pScopeName) {
	if (pPattern->regex) {
		int flags = RE_DOREX|RE_NOADVANCE;
		if (pPattern->ignoreCase) {
			flags |= RE_IGNCASE;
		}
		pPattern->rePattern = grammar_compileAndCreateRegex(pPattern->pattern, pScopeName, flags);
		pPattern->len = regex_getMinimumMatchLength(pPattern->rePattern);
	} else {
		pPattern->len = pPattern->pattern == NULL ? 0 : (int)strlen(pPattern->pattern);
	}
}

/*
 * Setup the internal state match tab from a list of grammar patterns defined for a grammar.
 */
static LEXICAL_STATE grammar_definePatterns(GRAMMAR* pGrammar, GRAMMAR_PATTERN* pPattern, LEXICAL_STATE state) {
	GRAMMAR_PATTERN* pPatternStart = pPattern;
	while (pPattern && state < DIM(pGrammar->patternsByState)) {
		pGrammar->patternsByState[state] = pPattern;
		if (pPattern->begin[0] && pPattern->end[0]) {
			pPattern->spansLines = TRUE;
			pGrammar->hasLineSpanPattern = TRUE;
			grammar_addCharTransition(pGrammar, pPattern->begin[0], state);
		}
		else {
			RE_PATTERN* pREPattern = grammar_compile(pGrammar, pPattern);
			if (pREPattern) {
				for (int i = 0; i < 256; i++) {
					if (regex_matchesFirstChar(pREPattern, (unsigned char)i)) {
						grammar_addCharTransition(pGrammar, (unsigned char)i, state);
					}
				}
				pPattern->wordmatch = regex_matchWordStart(pREPattern);
			}
		}
		state++;
		state = grammar_processChildPatterns(pGrammar, state, pPattern);
		pPattern = pPattern->next;
		if (state >= DIM(pGrammar->patternsByState)) {
			error_displayAlertDialog("Too many states defined in grammar %s", pGrammar->scopeName);
			return state;
		}
	}
	grammar_processCaptures(pGrammar, pPatternStart, state);
	return state;
}

/* 
 * Apply certain defaults from an imported grammar.
 */
static void grammar_applyDefaultFromImported(GRAMMAR* pGrammar, GRAMMAR* pImported) {
	if (!pGrammar->formatter && pImported->formatter) {
		pGrammar->formatter = _strdup(pImported->formatter);
	}
	if (!pGrammar->evaluator && pImported->evaluator) {
		pGrammar->evaluator = _strdup(pImported->evaluator);
	}
	if (!pGrammar->u2lset[0] && pImported->u2lset[0]) {
		memcpy(pGrammar->u2lset, pImported->u2lset, sizeof pImported->u2lset);
	}
	if (!pGrammar->analyzer && pImported->analyzer) {
		pGrammar->analyzer = _strdup(pImported->analyzer);
	}
	if (!pGrammar->highlightBrackets && pImported->highlightBrackets) {
		pGrammar->highlightBrackets = pImported->highlightBrackets;
		pGrammar->highlightBracketsImported = TRUE;
	}
	if (!pGrammar->increaseIndentPatterns && !pGrammar->decreaseIndentPatterns && pImported->increaseIndentPatterns) {
		pGrammar->increaseIndentPatterns = pImported->increaseIndentPatterns;
		pGrammar->decreaseIndentPatterns = pImported->decreaseIndentPatterns;
		pGrammar->indentPatternsImported = TRUE;
	}
}

/*
 * Initialize the grammar by constructing the basic knowledge necessary to make a fast
 * parsing of the grammar more simple.
 */
static void grammar_initialize(GRAMMAR* pGrammar) {
	memset(pGrammar->transitions, 0, sizeof pGrammar->transitions);
	memset(pGrammar->patternsByState, 0, sizeof pGrammar->patternsByState);
	LEXICAL_STATE state = CUSTOM_STATE;

	state = grammar_definePatterns(pGrammar, pGrammar->patternDefinitions, state);
	BRACKET_RULE* pBrRule = pGrammar->highlightBrackets;
	while (pBrRule) {
		grammar_processMatchPattern(&pBrRule->lefthand, pGrammar->scopeName);
		pBrRule->righthand.ignoreCase = pBrRule->lefthand.ignoreCase;
		pBrRule->righthand.regex = pBrRule->lefthand.regex;
		grammar_processMatchPattern(&pBrRule->righthand, pGrammar->scopeName);
		pBrRule = pBrRule->next;
	}
	TEMPLATE* pTemplate = pGrammar->templates;
	while (pTemplate) {
		grammar_processMatchPattern(&pTemplate->t_pattern, pGrammar->scopeName);
		if (pTemplate->t_contents == NULL && pTemplate->t_pattern.pattern != NULL) {
			pTemplate->t_contents = _strdup(pTemplate->t_pattern.pattern);
		}
		pTemplate = pTemplate->next;
	}
	INDENT_PATTERN* pIndent = pGrammar->decreaseIndentPatterns;
	while (pIndent) {
		grammar_processMatchPattern(&pIndent->pattern, pGrammar->scopeName);
		pIndent = pIndent->next;
	}
	pIndent = pGrammar->increaseIndentPatterns;
	while (pIndent) {
		grammar_processMatchPattern(&pIndent->pattern, pGrammar->scopeName);
		pIndent = pIndent->next;
	}
	if (pGrammar->importedGrammarNames) {
		ARRAY_ITERATOR iterator = arraylist_iterator(pGrammar->importedGrammarNames);
		void** p = iterator.i_buffer;
		while (p < iterator.i_bufferEnd) {
			GRAMMAR* pImported = grammar_findNamed(*p++);
			if (pImported) {
				state = grammar_definePatterns(pGrammar, pImported->patternDefinitions, state);
				grammar_applyDefaultFromImported(pGrammar, pImported);
			}
		}
	}
}

/*
 * Load a grammar definition file from the PKS_SYS directory. 
 */
static int grammar_loadFromFile(const char* pszGrammarName) {
	GRAMMAR_DEFINITIONS definitions;
	char szFileName[70];

	if (strlen(pszGrammarName) > 55) {
		EdTRACE(log_message(DEBUG_WARN, "Grammar name %s is too long (more than 50 characters)", pszGrammarName));
		return 0;
	}
	memset(&definitions, 0, sizeof definitions);
	sprintf(szFileName, "%s.grammar.json", pszGrammarName);
	if (json_parse(szFileName, TRUE, &definitions, _grammarDefinitionsRules)) {
		if (definitions.gd_grammars != NULL) {
			grammar_initialize(definitions.gd_grammars);
			ll_add(&_grammarDefinitions.gd_grammars, (LINKED_LIST*)definitions.gd_grammars);
		} else {
			EdTRACE(log_message(DEBUG_WARN, "Grammar name %s: no grammar rules defined", pszGrammarName));
			return 0;
		}
		return 1;
	}
	return 0;
}

/*
 * Find a grammar with the given name. If it cannot be found, try to load the corresponding grammar file
 * on the fly.
 */
GRAMMAR* grammar_findNamed(const char* pszGrammarName) {
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
			EdTRACE(log_message(DEBUG_WARN, "Grammar %s: no grammar found", pszGrammarName));
			break;
		}
	}
	return NULL;
}

/*
 * Try to match a keyword by looking it up in the keyword set. 
 */
static int grammar_matchKeyword(HASHMAP* pKeywords, char* pKey, const char* pKeyEnd) {
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
	return hashmap_containsKey(pKeywords, szBuf);
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

/*
 * Styling was defined with capture groups of a pattern - apply the styling from the found capture groups.
 */
static int grammar_addStyledGroups(int nElementCount, RE_MATCH* pMatch, PATTERN_GROUP* pGroup,
			LEXICAL_STATE defaultState, LEXICAL_ELEMENT* pResult, const char* pszStart, int *pNewOffset) {
	int nBracket = 0;
	char* pPrevious = pMatch->loc1;

	while (pGroup) {
		char* p1 = pMatch->braslist[nBracket];
		char* p2 = pMatch->braelist[nBracket];
		nElementCount = grammar_addDelta(defaultState, (int)(p1 - pPrevious), nElementCount, pResult);
		nElementCount = grammar_addDelta(pGroup->state, (int)(p2 - p1), nElementCount, pResult);
		*pNewOffset = (int)(p2 - pszStart);
		pPrevious = p2;
		pGroup = pGroup->next;
		nBracket++;
	}
	return nElementCount;
}

/*
 * Match a grammar pattern 'pPattern' within a range of text (pszBuf -> pszBufEnd). Return 1 if matched and add the proper lexical token
 * runs to the result array.
 */
static int grammar_matchPattern(GRAMMAR* pGrammar, GRAMMAR_PATTERN* pPattern, LEXICAL_STATE currentState, LEXICAL_STATE matchedState, 
	unsigned char* pszBuf, unsigned char* pszBufEnd, size_t i, size_t* pStateOffset,
	int *pElementCount, LEXICAL_ELEMENT pResult[MAX_LEXICAL_ELEMENT]) {

	RE_MATCH match;

	RE_PATTERN* pRePattern = pPattern->rePattern;
	if (pRePattern) {
		pRePattern->beginOfLine = pszBuf;
		if (regex_match(pRePattern, &pszBuf[i], pszBufEnd, &match)) {
			int nNewOffset = (int)(match.loc2 - pszBuf);
			if (pPattern->captures) {
				*pElementCount = grammar_addDelta(currentState, (int)(i - *pStateOffset), *pElementCount, pResult);
				*pElementCount = grammar_addStyledGroups(*pElementCount, &match, pPattern->captures, currentState, pResult, pszBuf, &nNewOffset);
				*pStateOffset = nNewOffset;
				return 1;
			} else if (grammar_matchKeyword(pPattern->keywords, match.loc1, match.loc2)) {
				*pElementCount = grammar_addDelta(currentState, (int)(i - *pStateOffset), *pElementCount, pResult);
				*pElementCount = grammar_tokenFound(pGrammar, pResult, matchedState, pPattern, pszBuf, i, nNewOffset, *pElementCount);
				*pStateOffset = nNewOffset;
				return 1;
			}
		}
	} else if (strncmp(&pszBuf[i], pPattern->begin, strlen(pPattern->begin)) == 0) {
		*pElementCount = grammar_addDelta(currentState, (int)(i - *pStateOffset), *pElementCount, pResult);
		*pStateOffset = i;
		return 1;
	}
	return 0;
}

/*
 * We found a range of text to be marked as styled token. Add the style information to our lexical element list. If nested patterns (child patterns)
 * are defined for the current pattern, try to match these as sub-tokens.
 */
static int grammar_tokenFound(GRAMMAR* pGrammar, LEXICAL_ELEMENT pResult[MAX_LEXICAL_ELEMENT], LEXICAL_STATE currentState, GRAMMAR_PATTERN* pPattern, 
		char* pszBuf, size_t start, size_t end, int nElementCount) {

	if (pPattern && pPattern->children) {
		for (size_t i = start; i < end; i++) {
			GRAMMAR_PATTERN* pChild = pPattern->children;
			while (pChild) {
				RE_PATTERN* pRePattern = pChild->rePattern;
				if (pRePattern) {
					RE_MATCH match;
					pRePattern->beginOfLine = pszBuf;
					if (regex_match(pRePattern, &pszBuf[i], &pszBuf[end], &match)) {
						if (pChild->captures) {
							nElementCount = grammar_addDelta(currentState, (int)(i - start), nElementCount, pResult);
							int nNewOffset;
							nElementCount = grammar_addStyledGroups(nElementCount, &match, pChild->captures, currentState, pResult, pszBuf, &nNewOffset);
							i = nNewOffset;
							break;
						}
						else if (grammar_matchKeyword(pChild->keywords, match.loc1, match.loc2)) {
							nElementCount = grammar_addDelta(currentState, (int)(i - start), nElementCount, pResult);
							LEXICAL_STATE stateIdx = grammar_patternIndex(pGrammar, pChild->name);
							size_t nDelta = match.loc2 - match.loc1;
							nElementCount = grammar_addDelta(stateIdx, (int)nDelta, nElementCount, pResult);
							i = start = match.loc2 - pszBuf;
							break;
						}
					}
				}
				pChild = pChild->next;
			}
		}
	}
	return grammar_addDelta(currentState, (int)(end - start), nElementCount, pResult);
}

/*
 * This method will parse the characters in pszBuf (assumed length is lLength) and determine the lexical
 * tokens defined as LEXICAL_ELEMENTs to be placed in the result buffer. If successful return the state at the
 * end of the line.
 */
int grammar_parse(GRAMMAR* pGrammar, LEXICAL_ELEMENT pResult[MAX_LEXICAL_ELEMENT], LEXICAL_STATE startState, char* pszBuf, size_t lLength, int *pLineSpanningEndDetected) {
	if (pGrammar == 0) {
		return 0;
	}
	GRAMMAR_PATTERN* pRootPattern = pGrammar->patternsByState[startState];
	BOOL spansLines = pRootPattern && pRootPattern->spansLines;
	LEXICAL_STATE currentState = (startState == UNKNOWN || spansLines) ? startState : INITIAL;
	LEXICAL_STATE nextState = currentState;
	int nElementCount = 0;
	size_t nStateOffset = 0;
	char* pszEnd = spansLines ? pRootPattern->end : NULL;
	for (size_t i = 0; i < lLength; ) {
		unsigned char c = pszBuf[i];
		if (pszEnd) {
			size_t pszEndLen = strlen(pszEnd);
			if (c == pszEnd[0] && strncmp(&pszBuf[i], pszEnd, pszEndLen) == 0) {
				*pLineSpanningEndDetected = 1;
				i += pszEndLen;
				nElementCount = grammar_tokenFound(pGrammar, pResult, currentState, pRootPattern, pszBuf, nStateOffset, i, nElementCount);
				nElementCount = grammar_addDelta(INITIAL, 0, nElementCount, pResult);
				nStateOffset = i;
				nextState = currentState = INITIAL;
				pszEnd = NULL;
			} else {
				i++;
				continue;
			}
		} else if (currentState == INITIAL || currentState == UNKNOWN) {
			LONGLONG possibleStates = pGrammar->transitions[c];
			if (possibleStates) {
				int matched = 0;
				while (possibleStates) {
					LEXICAL_STATE state = possibleStates & CHARACTER_STATE_MASK;
					GRAMMAR_PATTERN* pPattern = pGrammar->patternsByState[state];
					if (!pPattern) {
						break;
					}
					if (pPattern->wordmatch && i > 0 && pks_isalnum(pszBuf[i - 1])) {
						possibleStates >>= CHARACTER_STATE_SHIFT;
						continue;
					}
					if (grammar_matchPattern(pGrammar, pPattern, currentState, state, pszBuf, 
							&pszBuf[lLength], i, &nStateOffset, &nElementCount, pResult)) {
						if (!pPattern->rePattern) {
							pRootPattern = pPattern;
							pszEnd = pPattern->end;
							nextState = currentState = state;
							i = nStateOffset + strlen(pPattern->begin);
						} else {
							i = nStateOffset;
						}
						matched = 1;
						break;
					}
					possibleStates >>= CHARACTER_STATE_SHIFT;
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
		if (nextState != currentState) {
			nElementCount = grammar_tokenFound(pGrammar, pResult, currentState, pRootPattern, pszBuf, nStateOffset, i, nElementCount);
			currentState = nextState;
			nStateOffset = i;
		}
	}
	if (lLength > nStateOffset) {
		nElementCount = grammar_tokenFound(pGrammar, pResult, currentState, pRootPattern, pszBuf, nStateOffset, lLength, nElementCount);
	}
	return nElementCount;
}

/*
 * Initialize the style translation table for a given grammar. 
 */
void grammar_initTokenTypeToStyleTable(GRAMMAR* pGrammar, unsigned char tokenTypeToStyleTable[MAX_TOKEN_TYPE]) {
	memset(tokenTypeToStyleTable, FS_NORMAL, MAX_TOKEN_TYPE);
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
			UCLIST* pList = ll_insert(&pGrammar->undercursorActions, sizeof(UCLIST));
			pList->action = UA_SHOWMATCH;
			memcpy(&pList->uc_pattern, &pRule->lefthand, sizeof (pRule->lefthand));
			pList->p.uc_bracket = pRule;
			pRule = pRule->next;
		}
		TEMPLATE* pTemplate = pGrammar->templates;
		while (pTemplate) {
			UCLIST* pList = ll_insert(&pGrammar->undercursorActions, sizeof(UCLIST));
			pList->action = pTemplate->t_auto ? UA_ABBREV : UA_TEMPLATE;
			memcpy(&pList->uc_pattern, &pTemplate->t_pattern, sizeof(pTemplate->t_pattern));
			pList->p.uc_template = pTemplate->t_contents;
			pTemplate = pTemplate->next;
		}

	}
	return pGrammar->undercursorActions;
}

/*
 * Returns the list of navigation patterns for a given grammar.
 */
NAVIGATION_PATTERN* grammar_getNavigationPatterns(GRAMMAR* pGrammar) {
	return pGrammar ? pGrammar->navigation : NULL;
}

/*
 * Returns the list of tag sources for a grammar.
 */
TAGSOURCE* grammar_getTagSources(GRAMMAR* pGrammar) {
	return pGrammar ? pGrammar->tagSources : NULL;
}

/*
 * Determines a lexical context matched by a grammar pattern assuming standard pattern naming conventions.
 */
static LEXICAL_CONTEXT grammar_getContextForPattern(GRAMMAR_PATTERN* pPattern) {
	if (string_strcasestr(pPattern->name, "comment") != NULL) {
		return pPattern->begin[0] && pPattern->end[0] ? LC_MULTILINE_COMMENT : LC_SINGLE_LINE_COMMENT;
	}
	if (string_strcasestr(pPattern->name, "literal") != NULL) {
		if (string_strcasestr(pPattern->name, "string") != NULL) {
			return LC_MULTI_QUOTED_LITERAL;
		}
		if (string_strcasestr(pPattern->name, "character") != NULL) {
			return LC_SINGLE_QUOTED_LITERAL;
		}
	}
	return LC_START;
}

/*
 * Returns the comment descriptor for the described language in pDescriptor.
 * If a comment info is available this method returns 1, otherwise 0.
 */
int grammar_getCommentDescriptor(GRAMMAR* pGrammar, COMMENT_DESCRIPTOR* pDescriptor) {
	char* pszInput;
	char* pszOutput;
	char c;
	int nRet = 0;

	if (pGrammar == NULL) {
		memset(pDescriptor, 0, sizeof * pDescriptor);
		return 0;
	}
	if (pGrammar->commentDescriptorInitialized) {
		memcpy(pDescriptor, &pGrammar->commentDescriptor, sizeof * pDescriptor);
		return 1;
	}
	memset(pDescriptor, 0, sizeof * pDescriptor);
	GRAMMAR_PATTERN* pSingleLineCommentPattern = NULL;
	GRAMMAR_PATTERN* pBlockCommentPattern = NULL;
	GRAMMAR_PATTERN* pOtherBlockCommentPattern = NULL;
	for (int i = CUSTOM_STATE; i < DIM(pGrammar->patternsByState); i++) {
		GRAMMAR_PATTERN* pPattern = pGrammar->patternsByState[i];
		if (!pPattern) {
			continue;
		}
		LEXICAL_CONTEXT lcContext = grammar_getContextForPattern(pPattern);
		if (lcContext == LC_MULTILINE_COMMENT || lcContext == LC_SINGLE_LINE_COMMENT) {
			nRet = 1;
			if (pPattern->begin[0] && pPattern->end[0]) {
				BOOL bDefault = strcmp(pPattern->name, "comment.multiLine") == 0;
				if (bDefault || pBlockCommentPattern == NULL) {
					pBlockCommentPattern = pPattern;
				}
				if (!bDefault) {
					pOtherBlockCommentPattern = pPattern;
				}
			}
			else {
				if (strcmp(pPattern->name, "comment.singleLine") == 0 || pSingleLineCommentPattern == NULL) {
					pSingleLineCommentPattern = pPattern;
				}
			}
		}
	}
	if (pSingleLineCommentPattern) {
		char szTemp[32];
		char* pEnd = szTemp + sizeof szTemp - 1;
		pszInput = pSingleLineCommentPattern->match;
		pszOutput = szTemp;
		while ((c = *pszInput++) != 0 && pszOutput < pEnd) {
			if (c == '^') {
				continue;
			}
			if (c == '.' || c == '[') {
				break;
			}
			*pszOutput++ = c;
		}
		*pszOutput = 0;
		if (szTemp[0]) {
			pDescriptor->comment_single = _strdup(szTemp);
		}
	}
	if (pBlockCommentPattern) {
		pDescriptor->comment_start = _strdup(pBlockCommentPattern->begin);
		pDescriptor->comment_end = _strdup(pBlockCommentPattern->end);
	}
	if (pOtherBlockCommentPattern) {
		pDescriptor->comment2_start = _strdup(pOtherBlockCommentPattern->begin);
		pDescriptor->comment2_end = _strdup(pOtherBlockCommentPattern->end);
	}
	pGrammar->commentDescriptorInitialized = TRUE;
	memcpy(&pGrammar->commentDescriptor, pDescriptor, sizeof * pDescriptor);
	return nRet;
}

/*
 * Invoked, when the type of the current active editor changes and will define the identifier
 * according to the current documents grammar.
 */
void grammar_documentTypeChanged(GRAMMAR* pGrammar) {
	if (pGrammar && pGrammar->u2lset[0]) {
		regex_compileCharacterClasses(pGrammar->u2lset);
	}
	else {
		regex_compileCharacterClasses("a-zäöü=A-ZÄÖÜß_0-9");
	}
}

static int (*pKeywordMatch)(char* param);
static int grammar_addKeyword(char* pszKeyWord, ANALYZER_CALLBACK addCallback) {
	if (pKeywordMatch(pszKeyWord)) {
		(*addCallback)(&(ANALYZER_CALLBACK_PARAM){ .acp_recommendation= pszKeyWord });
	}
	return 1;
}

/*
 * Add all suggestions matching 'pszMatch', which can be derived from a grammar by invoking the addCallback.
 * This includes e.g. the keywords defined in a grammar but also "language specific" analysis results of the surrounding
 * file.
 */
void grammar_addSuggestionsMatching(GRAMMAR* pGrammar, int (*fMatch)(char* pszMatch), ANALYZER_CALLBACK addCallback) {
	if (pGrammar == NULL) {
		return;
	}
	pKeywordMatch = fMatch;
	for (int i = CUSTOM_STATE; i < DIM(pGrammar->patternsByState); i++) {
		GRAMMAR_PATTERN* pPattern = pGrammar->patternsByState[i];
		if (!pPattern) {
			continue;
		}
		if (pPattern->keywords) {
			hashmap_forEachKey(pPattern->keywords, (int (*)(intptr_t, void*)) grammar_addKeyword, addCallback);
		}
	}
}

/*
 * Returns the name of a code analyzer to use to analyse the code of the document with the given grammar
 * for the purpose of calculating suggestions. The returned name (if analysers are defined) are then to be used
 * to aquire a code analyzer from the code analyzer package.
 */
char* grammar_getCodeAnalyzer(GRAMMAR* pGrammar) {
	return pGrammar != NULL ? pGrammar->analyzer : NULL;
}

/*
 * Returns the name of an evaluator to use to evaluate the current selection in the document with the given grammar.
 * The name of the evaluator is used by the Evaluator package.
 */
char* grammar_getEvaluator(GRAMMAR* pGrammar) {
	return pGrammar != NULL ? pGrammar->evaluator : NULL;
}

/*
 * Returns the name of a formatter to use to format the current selection in the document with the given grammar.
 * The name of the formatter is used by the Formatter package.
 */
char* grammar_getFormatter(GRAMMAR* pGrammar) {
	return pGrammar != NULL ? pGrammar->formatter : NULL;
}

/*
 * Returns true, if this grammar defines patterns spanning multiple lines making 
 * parsing a bit more complex in that a window of lines has to be rescanned to detect
 * multi-line patterns.
 */
BOOL grammar_hasLineSpans(GRAMMAR* pGrammar) {
	return pGrammar != NULL && pGrammar->hasLineSpanPattern;
}

/*
 * Returns the name of the wysiwyg renderer for the given grammar or 0, if no
 * special renderer is available.
 */
const char* grammar_wysiwygRenderer(GRAMMAR* pGrammar) {
	return (pGrammar == NULL || pGrammar->wysiwygRenderer[0] == 0) ? NULL : pGrammar->wysiwygRenderer;
}

/*
 * Returns the name of the default display mode to be used when editing files for the this grammar or 0,
 * if the default display mode should be used. Currently display modes "hex", "default" and "wysiwyg" are
 * supported.
 */
const char* grammar_defaultDisplayMode(GRAMMAR* pGrammar) {
	return (pGrammar == NULL || pGrammar->defaultDisplayMode[0] == 0) ? NULL : pGrammar->defaultDisplayMode;
}


static grammar_matchWordInLine(char c, const char* pszMatch, const char* pBuf, int i, size_t nLen) {
	if (c == *pszMatch) {
		size_t nMatchLen = strlen(pszMatch);
		return i + nMatchLen <= nLen && strncmp(pszMatch, &pBuf[i], nMatchLen) == 0;
	}
	return 0;
}

/*
 * Calculate the delta indentation defined by a line. This is for a C-file or Java-File +1, if
 * the line contains a { or -1 if the line contains a }. Simple comment checking is performed as
 * well. For pascal it is +1 when a line contains "begin" and -1 when the line contains "end".
 */
static LEXICAL_CONTEXT grammar_lexicalContextDo(LEXICAL_CONTEXT nState, GRAMMAR* pGrammar, const char* pBuf, size_t nLen, 
		BOOL (*callback)(GRAMMAR* pGrammar, const char* pBuf, int nOffset, size_t nLen, LEXICAL_CONTEXT nState, void *pParam), void* pParam) {
	if (!pGrammar) {
		return nState;
	}
	COMMENT_DESCRIPTOR cd;
	grammar_getCommentDescriptor(pGrammar, &cd);
	for (int i = 0; i < nLen; i++) {
		if (!callback(pGrammar, pBuf, i, nLen, nState, pParam)) {
			break;
		}
		char c = pBuf[i];
		if (nState == LC_MULTILINE_COMMENT) {
			if (grammar_matchWordInLine(c, cd.comment_end, pBuf, i, nLen)) {
				i += (int)strlen(cd.comment_end);
				nState = LC_START;
			}
			continue;
		}
		if (nState == LC_SINGLE_QUOTED_LITERAL || nState == LC_MULTI_QUOTED_LITERAL) {
			char cCompare = nState == LC_SINGLE_QUOTED_LITERAL ? '\'' : '"';
			if (c == '\\' && i < nLen - 1) {
				c = pBuf[++i];
				continue;
			}
			if (c == cCompare) {
				nState = LC_START;
			}
			continue;
		}
		if (c == '"') {
			nState = LC_MULTI_QUOTED_LITERAL;
			continue;
		}
		if (c == '\'') {
			nState = LC_SINGLE_QUOTED_LITERAL;
			continue;
		}
		if (cd.comment_single && grammar_matchWordInLine(c, cd.comment_single, pBuf, i, nLen)) {
			nState = LC_SINGLE_LINE_COMMENT;
			continue;
		}
		if (cd.comment_start && grammar_matchWordInLine(c, cd.comment_start, pBuf, i, nLen)) {
			nState = LC_MULTILINE_COMMENT;
			i += (int)strlen(cd.comment_start);
			continue;
		}
		if (cd.comment2_start && grammar_matchWordInLine(c, cd.comment2_start, pBuf, i, nLen)) {
			nState = LC_MULTILINE_COMMENT;
			i += (int)strlen(cd.comment2_start);
			continue;
		}
	}
	return nState;
}

static BOOL grammar_matchUCPattern(const char* pBeginOfLine, const char* pBuf, const char* pBEnd, UC_MATCH_PATTERN* pPattern) {
	BOOL bMatch;
	if (pPattern->rePattern) {
		RE_MATCH matchResult;
		pPattern->rePattern->beginOfLine = pBeginOfLine;
		bMatch = regex_match(pPattern->rePattern, pBuf, pBEnd, &matchResult);
	} else {
		bMatch = string_compareWithSecond(pBuf, pPattern->pattern, pPattern->ignoreCase) == 0;
	}
	return bMatch;
}

/*
 * Callback to determine the additional syntactical indenting for a buffer by inspecting opening brackets, braces etc...
 */
static BOOL grammar_countIndent(GRAMMAR* pGrammar, const char* pBuf, int nOffset, size_t nLen, LEXICAL_CONTEXT nState, void* pParam) {
	if (nState == LC_SINGLE_LINE_COMMENT) {
		return FALSE;
	}
	if (nState != LC_START) {
		return TRUE;
	}
	INDENT_PATTERN* pPatterns = pGrammar->increaseIndentPatterns;
	while (pPatterns) {
		if (grammar_matchUCPattern(pBuf, &pBuf[nOffset], &pBuf[nLen], &pPatterns->pattern)) {
			(*(int*)pParam)++;
		}
		pPatterns = pPatterns->next;
	}
	pPatterns = pGrammar->decreaseIndentPatterns;
	while (pPatterns) {
		if (grammar_matchUCPattern(pBuf, &pBuf[nOffset], &pBuf[nLen], &pPatterns->pattern)) {
			(*(int*)pParam)--;
		}
		pPatterns = pPatterns->next;
	}
	return TRUE;
}

/*
 * Calculate the delta indentation defined by a line. This is for a C-file or Java-File +1, if 
 * the line contains a { or -1 if the line contains a }. Simple comment checking is performed as
 * well. For pascal it is +1 when a line contains "begin" and -1 when the line contains "end".
 */
int grammar_getDeltaIndentation(GRAMMAR* pGrammar, LEXICAL_CONTEXT nState, const char* pBuf, size_t nLen) {
	if (!pGrammar) {
		return 0;
	}
	if (!pGrammar->increaseIndentPatterns && !pGrammar->decreaseIndentPatterns) {
		return 0;
	}
	int nDelta = 0;
	grammar_lexicalContextDo(nState, pGrammar, pBuf, nLen, grammar_countIndent, &nDelta);
	if (nDelta == 0) {
		return 0;
	}
	if (nDelta < 0) {
		return -1;
	}
	return 1;
}

/*
 * Callback to stop scanning a line, when a particular offset in the line was reached.
 */
static BOOL grammar_waitForOffset(GRAMMAR* pGrammar, const char* pBuf, int nOffset, size_t nLen, LEXICAL_CONTEXT nState, void* pParam) {
	return nOffset < *(int*)pParam;
}

/*
 * Determines the "lexical context" in a buffer at a given position.
 */
LEXICAL_CONTEXT grammar_getLexicalContextAt(GRAMMAR* pGrammar, LEXICAL_CONTEXT nStartState, const char* pBuf, size_t nLen, int nOffset) {
	if (!pGrammar) {
		return nStartState;
	}
	return grammar_lexicalContextDo(nStartState, pGrammar, pBuf, nLen, grammar_waitForOffset, &nOffset);
}

/*
 * Returns the lexical context matching a lexical state (used by the highlighter).
 */
LEXICAL_CONTEXT grammar_getLexicalContextForState(GRAMMAR* pGrammar, LEXICAL_STATE aState) {
	if (pGrammar == NULL) {
		return LC_START;
	}
	GRAMMAR_PATTERN *pPattern = pGrammar->patternsByState[aState];
	if (!pPattern) {
		return LC_START;
	}
	return grammar_getContextForPattern(pPattern);
}

/*
 * Returns true, if the lexical state is defined by a "multi-line" definition using 
 * begin and end - in other words the state may span several lines.
 */
BOOL grammar_isMultilineState(GRAMMAR* pGrammar, LEXICAL_STATE aState) {
	if (pGrammar == NULL) {
		return FALSE;
	}
	GRAMMAR_PATTERN* pPattern = pGrammar->patternsByState[aState];
	if (!pPattern) {
		return FALSE;
	}
	return pPattern->end[0] != 0;
}

/*
 * Returns the pattern name for a given lexical state as defined in the grammar. 
 */
const char* grammar_getPatternName(GRAMMAR* pGrammar, LEXICAL_STATE aState) {
	GRAMMAR_PATTERN* pPattern = pGrammar == 0 ? NULL : pGrammar->patternsByState[aState];
	if (pPattern) {
		return pPattern->name;
	}
	return 0;
}

/*
 * Returns the user readable name / description of a grammar.
 */
const char* grammar_description(GRAMMAR* pGrammar) {
	if (pGrammar->description[0]) {
		return pGrammar->description;
	}
	return pGrammar->scopeName;
}

/*
 * Returns the name of a grammar.
 */
const char* grammar_name(GRAMMAR* pGrammar) {
	return pGrammar->scopeName;
}

/*
 * Returns the contents of a template with a given name or NULL if not found.
 */
const char* grammar_getTemplate(GRAMMAR* pGrammar, const char* pszTemplateName) {
	if (pGrammar == NULL) {
		return NULL;
	}
	TEMPLATE* pTemplate = ll_find(pGrammar->templates, pszTemplateName);
	return pTemplate ? pTemplate->t_contents : NULL;
}

/*
 * Returns the contents of a template with a given name or NULL if not found.
 */
const char* grammar_getFileTemplate(GRAMMAR* pGrammar, const char* pszFilename) {
	if (pGrammar == NULL) {
		return NULL;
	}
	char* pszBase = string_getBaseFilename(pszFilename);
	if (pszBase == NULL) {
		return NULL;
	}
	char* pszExt = strrchr(pszBase, '.');
	if (pszExt != NULL) {
		char szTemplateName[40];
		snprintf(szTemplateName, sizeof szTemplateName, "file_%s", pszExt+1);
		const char* pszResult = grammar_getTemplate(pGrammar, szTemplateName);
		if (pszResult != NULL) {
			return pszResult;
		}
	}
	return grammar_getTemplate(pGrammar, "file");
}


