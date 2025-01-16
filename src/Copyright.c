/*
 * Copyright.c
 *
 * Maintain Copyright templates.
 *
 * author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include <Windows.h>
#include "copyright.h"
#include "jsonparser.h"
#include "linkedlist.h"
#include "grammar.h"
#include "stringutil.h"
#include "documentmodel.h"
#include "trace.h"

typedef struct tagCOPYRIGHT {
	struct tagCOPYRIGHT* co_next;
	// The name of the copyright profile.
	char co_name[32];
	// The actual copyright text
	char* co_notice;
} COPYRIGHT;

typedef struct tagCOPYRIGHT_LANGUAGE_OPTION {
	struct tagCOPYRIGHT_LANGUAGE_OPTION* clo_next;
	// Name of a grammar
	char clo_language[32];
	// Whether we should use single line comments to insert a template.
	int  clo_useSingleLineComments;
	// Whether a blank line should be added in front of the copyright
	int  clo_addBlankLineBefore;
	// Whether a blank line should be added at the end of the copyright
	int  clo_addBlankLineAfter;
} COPYRIGHT_LANGUAGE_OPTION;

typedef struct tagCOPYRIGHT_MANAGER {
	// The name of the currently selected copyright profile
	char cm_currentProfile[32];
	COPYRIGHT* cm_copyrightProfiles;
	COPYRIGHT_LANGUAGE_OPTION* cm_languageOptions;
} COPYRIGHT_MANAGER;

static COPYRIGHT_MANAGER _copyrightManager;

static JSON_MAPPING_RULE _copyrightRules[] = {
	{	RT_ALLOC_STRING, "notice", offsetof(COPYRIGHT, co_notice)},
	{	RT_CHAR_ARRAY, "name", offsetof(COPYRIGHT, co_name), sizeof(((COPYRIGHT*)NULL)->co_name)},
	{	RT_END}
};

static JSON_MAPPING_RULE _languageOptionRules[] = {
	{	RT_FLAG, "single-line-comments", offsetof(COPYRIGHT_LANGUAGE_OPTION, clo_useSingleLineComments), 1},
	{	RT_FLAG, "add-blank-line-before", offsetof(COPYRIGHT_LANGUAGE_OPTION, clo_addBlankLineBefore), 1},
	{	RT_FLAG, "add-blank-line-after", offsetof(COPYRIGHT_LANGUAGE_OPTION, clo_addBlankLineAfter), 1},
	{	RT_CHAR_ARRAY, "language", offsetof(COPYRIGHT_LANGUAGE_OPTION, clo_language), sizeof(((COPYRIGHT_LANGUAGE_OPTION*)NULL)->clo_language)},
	{	RT_END}
};

static JSON_MAPPING_RULE _copyrightManagerRules[] = {
	{	RT_CHAR_ARRAY, "default", offsetof(COPYRIGHT_MANAGER, cm_currentProfile), sizeof(((COPYRIGHT_MANAGER*)NULL)->cm_currentProfile)},
	{	RT_OBJECT_LIST, "profiles", offsetof(COPYRIGHT_MANAGER, cm_copyrightProfiles),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _copyrightRules, .ro_nestedSize = sizeof(COPYRIGHT)}}},
	{	RT_OBJECT_LIST, "language-options", offsetof(COPYRIGHT_MANAGER, cm_languageOptions),
			{.r_t_arrayDescriptor = {.ro_nestedRules = _languageOptionRules, .ro_nestedSize = sizeof(COPYRIGHT_LANGUAGE_OPTION)}}},
	{	RT_END}
};

/*
 * Returns the JSON mapping for the copyright_profiles.json config file.
 */
JSON_MAPPING_RULE* copyright_getJsonMapping() {
	return _copyrightManagerRules;
}

/*
 * Load the copyright_profiles.json file from the PKS_SYS directory.
 */
int copyright_loadProfiles() {
	if (json_parse("copyright_profiles.json", TRUE, &_copyrightManager, _copyrightManagerRules)) {
		EdTRACE(log_message(DEBUG_INFO, "Sucessfully loaded copyright profiles."));
		if (_copyrightManager.cm_copyrightProfiles && 
			!ll_find((LINKED_LIST*)_copyrightManager.cm_copyrightProfiles, _copyrightManager.cm_currentProfile)) {
			EdTRACE(log_message(DEBUG_WARN, "Default copyright profile %s not defined.", _copyrightManager.cm_currentProfile));
			strcpy(_copyrightManager.cm_currentProfile, _copyrightManager.cm_copyrightProfiles->co_name);
		}
		return 1;
	}
	return 0;
}


/*
 * Destroy all loaded copyright profiles.
 */
void copyright_destroyProfiles() {
	json_destroy(&_copyrightManager, _copyrightManagerRules);
}

/*
 * Return the copyright enclosed in comments as defined by the grammar passed. The STRING_BUF returned must be freed, if it is not
 * needed any more. If no copyright profiles are defined, or no comment formatting is defined for the passed grammar, NULL is
 * returned.
 */
STRING_BUF* copyright_getFormatted(EDIT_CONFIGURATION* pConfiguration) {
	COMMENT_DESCRIPTOR commentDescriptor;
	GRAMMAR* pGrammar = pConfiguration->grammar;

	if (!grammar_getCommentDescriptor(pGrammar, &commentDescriptor)) {
		return 0;
	}
	COPYRIGHT_LANGUAGE_OPTION* pOptions = ll_find(_copyrightManager.cm_languageOptions, grammar_name(pGrammar));
	BOOL bSingleLineComment = pOptions && pOptions->clo_useSingleLineComments;
	COPYRIGHT* pCopyright = ll_find(_copyrightManager.cm_copyrightProfiles, _copyrightManager.cm_currentProfile);
	if (pCopyright == NULL) {
		return 0;
	}
	char* pszSource = pCopyright->co_notice;
	STRING_BUF* pResult = stringbuf_create(128);
	char linePrefix[32] = { 0 };
	BOOL bLinePrefixed = FALSE;
	if (!bSingleLineComment && commentDescriptor.comment_start) {
		stringbuf_appendString(pResult, commentDescriptor.comment_start);
		stringbuf_appendChar(pResult, '\n');
		const char* pStart = commentDescriptor.comment_start;
		if (strlen(pStart) >= 2 && (pStart[1] == '*' || pStart[1] == '-')) {
			linePrefix[0] = ' ';
			linePrefix[1] = pStart[1];
			linePrefix[2] = ' ';
			bLinePrefixed = TRUE;
		} else {
			linePrefix[0] = ' ';
		}
	} else {
		if (!commentDescriptor.comment_single) {
			stringbuf_destroy(pResult);
			return 0;
		}
		strcpy(linePrefix, commentDescriptor.comment_single);
		strcat(linePrefix, " ");
	}
	if (pOptions != NULL && pOptions->clo_addBlankLineBefore) {
		stringbuf_appendString(pResult, linePrefix);
		stringbuf_appendChar(pResult, '\n');
	}
	stringbuf_appendString(pResult, linePrefix);
	while (*pszSource) {
		char c = *pszSource++;
		stringbuf_appendChar(pResult, c);
		if (*pszSource && c == '\n') {
			stringbuf_appendString(pResult, linePrefix);
		}
	}
	if (pOptions != NULL && pOptions->clo_addBlankLineAfter) {
		stringbuf_appendString(pResult, linePrefix);
		stringbuf_appendChar(pResult, '\n');
	}
	if (!bSingleLineComment && commentDescriptor.comment_end) {
		if (bLinePrefixed) {
			stringbuf_appendChar(pResult, ' ');
		}
		stringbuf_appendString(pResult, commentDescriptor.comment_end);
		stringbuf_appendChar(pResult, '\n');
	}
	return pResult;
}

