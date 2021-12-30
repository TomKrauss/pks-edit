/*
 * formatting.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: formatting routines
 *
 * 										created: 30.12.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#ifndef FORMATTING_H

/*
 * Calculates the screen indentation assumed for the line passed as an argument, that is the number of
 * column positions to be empty in a line inserted after the line passed as an argument.
 */
extern int format_calculateScreenIndentWithSyntax(WINFO* wp, LINE* lp);

typedef enum {
	ID_NONE,
	ID_INDENT_NEXT,		// subsequent lines should be indented +1
	ID_OUTDENT_THIS,	// the line starting the current line should be indented -1
	ID_INDENT_SPACE_NEXT,// subsequent lines should be indented by one space only
	ID_OUTDENT_SPACE_NEXT,// subsequent lines should be outdented by one space only
	ID_TEMP_INDENT		// only the current line should be indented +1
} INDENTATION_DELTA;

/*
 * Calculates the delta of an indentation.
 */
extern INDENTATION_DELTA format_calculateIndentationDelta(WINFO* wp, LINE* lp);

/*
 * Calculates the screen indentation assumed for the line passed as an argument, that is the number of
 * column positions to be empty in a line inserted after the line passed as an argument. Syntactical
 * constructs are honored.
 */
extern int format_calculateScreenIndentWithSyntax(WINFO* wp, LINE* lp);

/*
 * Calculates the screen indentation assumed for the line passed as an argument, that is the number of
 * column positions to be empty in a line inserted after the line passed as an argument.
 */
extern int format_calculateScreenIndent(WINFO* wp, LINE* lp);

#define FORMATTING_H
#endif
