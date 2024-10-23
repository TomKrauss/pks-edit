/*
 * Levensthein.c
 *
 * Project: PKS Edit for Windows
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
#include "alloc.h"

static int costOfSubstitution(char a, char b, int ignoreCase) {
    if (ignoreCase) {
        return _l2uset[a] == b || _u2lset[a] == b ? 0 : 1;
    }
    return a == b ? 0 : 1;
}

static int minimum(int a, int b, int c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

 /* compare two strings s and t with ls, lt: their respective length */
static long levenshtein_calculateOptimized(const char* s, long ls, const char* t, long lt, int nOptions) {
    int ignoreCase = nOptions & LOPT_IGNORE_CASE;
    int** dp = calloc(sizeof (int*), ls + 1);
    for (int i = 0; i < ls + 1; i++) {
        dp[i] = calloc(sizeof(int), lt + 1);
    }
    for (int i = 0; i <= ls; i++) {
        for (int j = 0; j <= lt; j++) {
            if (i == 0) {
                dp[i][j] = j;
            } else if (j == 0) {
                dp[i][j] = i;
            } else {
                dp[i][j] = minimum(dp[i - 1][j - 1]
                    + costOfSubstitution(s[i - 1], t[j - 1], ignoreCase),
                    dp[i - 1][j] + 1,
                    dp[i][j - 1] + 1);
            }
        }
    }
    int result = dp[ls][lt];
    for (int i = 0; i < ls + 1; i++) {
        free(dp[i]);
    }
    free(dp);
    return result;
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
    return levenshtein_calculateOptimized(s, ls, t, lt, nOptions);
}

