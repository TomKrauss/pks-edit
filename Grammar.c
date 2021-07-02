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

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "alloc.h"
#include "grammar.h"
#include "jsonparser.h"
#include "linkedlist.h"

typedef struct tagGRAMMAR_DEFINITIONS {
	GRAMMAR* gd_grammars;
	STYLE_SELECTOR* gd_styleSelectors;
} GRAMMAR_DEFINITIONS;

static GRAMMAR_DEFINITIONS _grammarDefinitions;

static GRAMMAR* grammar_createGrammar() {
	return calloc(1, sizeof(GRAMMAR));
}

static GRAMMAR_PATTERN* grammar_createGrammarPattern() {
	return calloc(1, sizeof(GRAMMAR_PATTERN));
}

static STYLE_SELECTOR* grammar_createStyleSelector() {
	return calloc(1, sizeof(STYLE_SELECTOR));
}


static JSON_MAPPING_RULE _styleSelectorRules[] = {
	{	RT_CHAR_ARRAY, "styleName", offsetof(STYLE_SELECTOR, styleName), sizeof(((STYLE_SELECTOR*)NULL)->styleName)},
	{	RT_CHAR_ARRAY, "patternName", offsetof(STYLE_SELECTOR, patternName), sizeof(((STYLE_SELECTOR*)NULL)->patternName)},
	{	RT_END}
};

static JSON_MAPPING_RULE _patternRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(GRAMMAR_PATTERN, name), sizeof(((GRAMMAR_PATTERN*)NULL)->name)},
	{	RT_ALLOC_STRING, "match", offsetof(GRAMMAR_PATTERN, match)},
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
	{	RT_OBJECT_LIST, "styles", offsetof(GRAMMAR_DEFINITIONS, gd_styleSelectors),
			{.r_t_arrayDescriptor = {grammar_createStyleSelector, _styleSelectorRules}}},
	{	RT_END}
};

static int grammar_destroyPattern(GRAMMAR_PATTERN* pPattern) {
	if (pPattern->match) {
		free(pPattern->match);
	}
	return 1;
}

static int grammar_destroyGrammar(GRAMMAR* pGrammar) {
	ll_destroy((LINKED_LIST**)&pGrammar->patterns, grammar_destroyPattern);
	return 1;
}

/*
 * Destroy all loaded grammars and style selectors. 
 */
void grammar_destroyAll() {
	if (_grammarDefinitions.gd_grammars) {
		ll_destroy((LINKED_LIST**)&_grammarDefinitions.gd_grammars, grammar_destroyGrammar);
	}
	if (_grammarDefinitions.gd_styleSelectors) {
		ll_destroy((LINKED_LIST**)&_grammarDefinitions.gd_styleSelectors, NULL);
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
			ll_add((LINKED_LIST**)&_grammarDefinitions.gd_grammars, (LINKED_LIST*)definitions.gd_grammars);
		}
		if (definitions.gd_styleSelectors != NULL) {
			ll_add((LINKED_LIST**)&_grammarDefinitions.gd_styleSelectors, (LINKED_LIST*)definitions.gd_styleSelectors);
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
