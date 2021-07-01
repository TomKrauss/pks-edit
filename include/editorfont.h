/*
 * editorfont.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS 10
 *
 * purpose: text style definitions.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EDITORFONT_H

#define EDITORFONT_H

/*
 * The style of a selected font 
 */
typedef struct tagEDFONTATTRIBUTES {
	char		strikeout;
	char		bOem;
	char		italic;
	char		underline;
	short		weight;			// 0 == default font everything else between 0 and 999 bolden factor
} EDFONTATTRIBUTES;

/*
 * Reusable font/color style object.
 */
typedef struct tagEDTEXTSTYLE {
	struct tagEDTEXTSTYLE* next;
	char		styleName[32];
	char		faceName[32];
	short		size;
	long		fgcolor;
	long		bgcolor;
	short		charset;
	EDFONTATTRIBUTES style;
} EDTEXTSTYLE;


#endif