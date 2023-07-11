/*
 * Levensthein.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: implementation of the levensthein algorithm for finding similar strings.
 *
 * 										created: 08.06.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include "levensthein.h"
#include "edctype.h"

 /* compare two strings s and t with ls, lt: their respective length and bestFound the best distance found so far */
static long levenshtein_calculateOptimized(const char* s, long ls, const char* t, long lt, long bestFound, int nOptions) {
    long a, b, c;
    int ignoreWS = nOptions & LOPT_IGNORE_WS;

    /* if last letters are the same, the difference is whatever is
     * required to edit the rest of the strings
     */
    while (ls > 0 && lt > 0) {
        char c1 = s[ls - 1];
        char c2 = t[lt - 1];
        if (c1 != c2) {
            if (ignoreWS) {
                int bSpaceFound = 0;
                if (pks_isspace(c1)) {
                    bSpaceFound = 1;
                    ls--;
                }
                if (pks_isspace(c2)) {
                    bSpaceFound = 1;
                    lt--;
                }
                if (bSpaceFound) {
                    continue;
                }
            }
            if (nOptions & LOPT_IGNORE_CASE) {
                if (_l2uset[c1] == c2 || _u2lset[c1] == c2) {
                    ls--;
                    lt--;
                    continue;
                }
            }
            break;
        }
        ls--;
        lt--;
    }
    if (ignoreWS) {
        while (ls > 0 && pks_isspace(s[ls - 1])) {
            ls--;
        }
        while (lt > 0 && pks_isspace(t[lt - 1])) {
            lt--;
        }
    }
    /* if either string is empty, difference is inserting all chars
     * from the other
     */
    if (!ls) return lt;
    if (!lt) return ls;

    if (--bestFound <= 0) {
        // optimization: search tree pruning - we cannot get any better from here - do not recurse but stop.
        return 1;
    }
    /* else try:
     *      changing last letter of s to that of t; or
     *      remove last letter of s; or
     *      remove last letter of t,
     * any of which is 1 edit plus editing the rest of the strings
     */
    a = levenshtein_calculateOptimized(s, ls - 1, t, lt - 1, bestFound, nOptions);
    if (a <= bestFound) {
        bestFound = a;
    }
    b = levenshtein_calculateOptimized(s, ls, t, lt - 1, bestFound, nOptions);
    if (b <= bestFound) {
        bestFound = b;
    }
    c = levenshtein_calculateOptimized(s, ls - 1, t, lt, bestFound, nOptions);
    if (a > b) {
        a = b;
    }
    if (a > c) {
        a = c;
    }
    return a + 1;
}

/* 
 * Compare two strings s and t with ls, lt: their respective length and calculate the Levensthein distance.
 * If -1 are passed as length, the strlen of the compared strings is used.
 * nOptions is a bitwise combination of LOPT_IGNORE_CASE and LOPT_IGNORE_WS affecting the way the distance is calculated.
 */
long levenshtein_calculate(const char* s, int ls, const char* t, long lt, int nOptions) {
    if (ls < 0) {
        ls = (long) strlen(s);
    }
    if (lt < 0) {
        lt = (long) strlen(t);
    }
    int bestFound = ls > lt ? ls : lt;
    return levenshtein_calculateOptimized(s, ls, t, lt, bestFound, nOptions);
}

