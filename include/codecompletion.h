/*
 * codecompletion.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: showing code completion and suggestion windows.
 *
 * 										created: 12.08.21
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CODECOMPLETION_H

 /*------------------------------------------------------------
  * codecomplete_registerWindowClass()
  */
extern int codecomplete_registerWindowClass();

/*
 * Hide the current suggestion window.
 */
extern void codecomplete_hideSuggestionWindow(WINFO* wp);

/*
 * codecomplete_updateCompletionList
 * update the list of completions awailable.
 */
extern void codecomplete_updateCompletionList(WINFO* wp, BOOL bForce);

/*
 * Process keyboard input in the code completion window. If the key is processed, return TRUE,
 * otherwise FALSE.
 */
extern BOOL codecomplete_processKey(HWND hwnd, UINT message, WPARAM wParam);

#ifdef CODEANALYZER_H

/*
 * Calculate a score for a match during code completion. The 'pszSearch' variable contains
 * the word to search, the 'pszFound' variable the recommendation.
 */
extern int codecomplete_calculateScore(ANALYZER_CARET_CONTEXT* pCaretContext, const char* pszFound);
#endif

#define CODECOMPLETION_H

#endif

