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

 /*
  * Callback to use to consume the recommendation found by an analyzer.
  * The recommendation text found must be copied inside the callback - if needed.
  * The callback returns the recommendation text pszRecommendation, an arbitrary param further describing the recommendation
  * and an optional callback responsible for returning a help text for the recommendation getting passed the recommendation and the
  * arbitrary param.
  */
typedef void (*ANALYZER_CALLBACK)(const char* pszRecommendation, void* pParam, const char* (*cbHelp)(const char* pszText, void* pParam), 
		const char*(*cbGetHyperlink)(const char* pszText));

/*
 * The analyzer function to operate on a file referred to by a view pointer 'wp'. Only recommendations
 * matching the match function 'fMatch' are considered and passed to the analyzer callback.
 */
typedef void (*ANALYZER_FUNCTION)(WINFO* wp, int (*fMatch)(const char* pszMatch), ANALYZER_CALLBACK fCallback);

/*
 * Register a new analyzer given the name of the analyzer. If an analyzer with the given name
 * exists, it is overridden. Return 1 if a new analyzer was registered and 0 if an existing one
 * was overridden.
 */
extern int analyzer_register(const char* pszName, ANALYZER_FUNCTION f);

/*
 * Destroy all known analyzers.
 */
extern void analyzer_destroyAnalyzers();
/*
 * Extract all recommendations from the file edited in the view identified by 'wp'.
 * Use the analyzer with the given analyzer name. If successful, return 1, otherwise 0.
 */
extern int analyzer_performAnalysis(const char* pszAnalyzerName, WINFO* wp, int (*fMatch)(const char* pszMatch), ANALYZER_CALLBACK fCallback);

/*
 * Register some "default" analyzers, which can be referenced in grammar files given their respective names.
 */
extern void analyzer_registerDefaultAnalyzers();

#define CODEANALYZER_H

#endif // !CODEANALYZER_H

