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

typedef struct tagGRAMMAR_PATTERN {
	struct tagGRAMMAR_PATTERN* next;	// linked list of grammar patterns.
	char  name[32];						// The name of the match - should use hierarchical names as for example 'comment.line' or 'keyword.control.java'
										// The names may be "matched" against style selectors specified below
	char* match;						// a simple RE pattern to match this pattern
	char  begin[12];					// mutually exclusive with match, one may define a begin and end marker to match. May span multiple lines
	char  end[12];						// the end marker maybe e.g. '$' to match the end of line.
} GRAMMAR_PATTERN;

typedef struct tagSTYLE_SELECTOR {
	struct tagSTYLE_SELECTOR* next;		// linked list of style selectors.
	char styleName[32];					// the name of a registered editor style - note, the maximum length of style names in PKS-EDIT is 32
	char patternName[32];				// Pattern name or prefix of a pattern name like 'keyword' to match 'keyword.control' and 'keyword.operator'
} STYLE_SELECTOR;

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
	GRAMMAR_PATTERN* patterns;			// The patterns defined for this grammar.
} GRAMMAR;

/*
 * Destroy all loaded grammars and style selectors.
 */
extern void grammar_destroyAll();

/*
 * Find a grammar with the given name. If it cannot be found, try to load the corresponding grammar file
 * on the fly.
 */
extern GRAMMAR* grammar_findNamed(char* pszGrammarName);

#define GRAMMAR_H
#endif
