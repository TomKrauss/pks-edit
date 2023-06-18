/*
 * codeanalyzer.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: analyze code for the purpose of calculating suggestions.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#ifndef CODEANALYZER_H

typedef struct tagANALYZER_CALLBACK_PARAM {
	const char* acp_recommendation;									// The text to be displayed to the user as recommended by the code completion window
	const char* (*acp_help)(const char* pszText, void* pObject);	// Callback for determining a help text for the recommendation and the acp_object representing
																	// the object for which a recommendation is provided (e.g. a native function or macro or the like).
	void* acp_object;												// Describes the object for which a recommendation is provided 
																	// (e.g. a native function or macro or the like).
	const char* (*acp_getHyperlinkText)(const char* pszURL);		// Returns a text to be displayed in the help window, when a link in the help window is clicked.
} ANALYZER_CALLBACK_PARAM;

 /*
  * Callback to use to consume the recommendation found by an analyzer.
  * The recommendation text found must be copied inside the callback - if needed.
  * The callback returns the recommendation text pszRecommendation, an arbitrary param further describing the recommendation
  * and an optional callback responsible for returning a help text for the recommendation getting passed the recommendation and the
  * arbitrary param.
  */
typedef void (*ANALYZER_CALLBACK)(ANALYZER_CALLBACK_PARAM* pParam);

typedef struct tagANALYZER_CONTEXT {
	int ac_type;
	char ac_token[128];
} ANALYZER_CONTEXT;

/*
 * Function used for matching valid code completion matches during analysis of a file.
 */
typedef int (*ANALYZER_MATCH)(ANALYZER_CONTEXT* pContext, const char* pszMatch);

/*
 * The analyzer function to operate on a file referred to by a view pointer 'wp'. Only recommendations
 * matching the match function 'fMatch' are considered and passed to the analyzer callback.
 */
typedef void (*ANALYZER_FUNCTION)(WINFO* wp, ANALYZER_MATCH fMatcher, ANALYZER_CALLBACK fCallback, ANALYZER_CONTEXT *pContext);

/*
 * Callback used to get the analyzer context at the cursor position, where a code completion is requested.
 */
typedef void (*ANALYZER_GET_CONTEXT)(WINFO* wp, ANALYZER_CONTEXT* pContext);

/*
 * Register a new analyzer given the name of the analyzer. If an analyzer with the given name
 * exists, it is overridden. Return 1 if a new analyzer was registered and 0 if an existing one
 * was overridden.
 */
extern int analyzer_register(const char* pszName, ANALYZER_FUNCTION f, ANALYZER_GET_CONTEXT fGetContext);

/*
 * Destroy all known analyzers.
 */
extern void analyzer_destroyAnalyzers();
/*
 * Extract all recommendations from the file edited in the view identified by 'wp'.
 * Use the analyzer with the given analyzer name. If successful, return 1, otherwise 0.
 */
extern int analyzer_performAnalysis(const char* pszAnalyzerName, WINFO* wp, ANALYZER_CALLBACK fCallback);

/*
 * Register some "default" analyzers, which can be referenced in grammar files given their respective names.
 */
extern void analyzer_registerDefaultAnalyzers();

#define CODEANALYZER_H

#endif // !CODEANALYZER_H

