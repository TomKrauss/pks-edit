/*
 * codeanalyzer.h
 *
 * Project: PKS Edit for Windows
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
	int			acp_score;											// A score for sorting the recommendation
	int			acp_replacedTextLength;								// The length of the original text matched - to be potentially replaced.
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

/*
 * Describes the situation around the input caret, which is used to create potential insertion suggestions.
 */
typedef struct tagANALYZER_CARET_CONTEXT {
	// The type of token found close to the caret.
	int ac_type;
	// A name for the token type - depends on the grammar. Can be used
	char ac_tokenTypeName[32];
	// The token - typically the word under the caret.
	char ac_token[128];
	// A second token close to the caret. Example: when suggesting XML this could be the name of an entity, when trying to match an attribute.
	char ac_token2[128];
} ANALYZER_CARET_CONTEXT;

/*
 * Function used for matching valid code completion matches during analysis of a file.
 */
typedef int (*ANALYZER_MATCH)(ANALYZER_CARET_CONTEXT* pContext, const char* pszMatch);

/*
 * The analyzer function to operate on a file referred to by a view pointer 'wp'. Only recommendations
 * matching the match function 'fMatch' are considered and passed to the analyzer callback.
 */
typedef void (*ANALYZER_FUNCTION)(WINFO* wp, ANALYZER_MATCH fMatcher, ANALYZER_CALLBACK fCallback, ANALYZER_CARET_CONTEXT *pContext);

/*
 * Callback used to get the analyzer context at the cursor position, where a code completion is requested.
 */
typedef void (*ANALYZER_GET_CONTEXT)(WINFO* wp, ANALYZER_CARET_CONTEXT* pContext);

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
 * Calculate the caret context for the given analyzer name and caret position from the window in which a code
 * completion is request. Return TRUE, if successful.
 */
BOOL analyzer_getCaretContext(const char* pszAnalyzerName, WINFO* wp, ANALYZER_CARET_CONTEXT* pCaretContext);

/*
 * Extract all recommendations from the file edited in the view identified by 'wp'.
 * Use the analyzer with the given analyzer name. If successful, return 1, otherwise 0.
 */
extern int analyzer_performAnalysis(const char* pszAnalyzerName, WINFO* wp, ANALYZER_CARET_CONTEXT* pCaretContext, ANALYZER_CALLBACK fCallback);

/*
 * Register some "default" analyzers, which can be referenced in grammar files given their respective names.
 */
extern void analyzer_registerDefaultAnalyzers();

#define CODEANALYZER_H

#endif // !CODEANALYZER_H

