/*
 * levensthein.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: implementation of a variable size array list.
 *
 * 										created: 08.09.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef LEVENSTHEIN_H
#define LEVENSTHEIN_H

#define LOPT_IGNORE_CASE	0x1		// ignore case when calculating the levensthein distance
#define LOPT_IGNORE_WS		0x2		// ignore white space when calculating the levensthein distance


/* 
 * Compare two strings s and t with ls, lt: their respective length and calculate the Levensthein distance.
 * If -1 are passed as length, the strlen of the compared strings is used.
 * nOptions is a bitwise combination of LOPT_IGNORE_CASE and LOPT_IGNORE_WS affecting the way the distance is calculated.
 */
extern long levenshtein_calculate(const char* s, int ls, const char* t, long lt, int nOptions);


#endif
