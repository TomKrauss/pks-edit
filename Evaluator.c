/*
 * Evaluator.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: define evaluators able to evaluate certain
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include <Windows.h>
#include "winfo.h"
#include "linkedlist.h"
#include "documentmodel.h"
#include "evaluator.h"
#include "pksmacro.h"
#include "grammar.h"
#include "textblocks.h"
#include "edierror.h"
#include "pksrc.h"

#define MAX_SELECTION_SIZE_TO_EXECUTE		16000

typedef struct tagEVALUATOR {
	struct tagEVALUATOR* ev_next;
	char ev_name[32];
	EVALUATION_FUNCTION ev_function;
} EVALUATOR;

static EVALUATOR* _evaluators;

static char* evaluator_evaluateMacros(const char* pszCode, EVALUATION_ACTION anAction) {
	macro_executeSingleLineMacro(pszCode, FALSE, "Evaluation");
	return NULL;
}

/*
 * Register a new evaluator given the name of the evaluator. If an evaluator with the given name
 * exists, it is overridden. Return 1 if a new evaluator was registered and 0 if an existing one
 * was overridden.
 */
int evaluator_register(const char* pszName, EVALUATION_FUNCTION f) {
	EVALUATOR* pEvaluator = _evaluators;

	while (pEvaluator) {
		if (strcmp(pszName, pEvaluator->ev_name) == 0) {
			pEvaluator->ev_function = f;
			return 0;
		}
		pEvaluator = pEvaluator->ev_next;
	}
	EVALUATOR* pNew = ll_insert(&_evaluators, sizeof * pNew);
	strncpy(pNew->ev_name, pszName, sizeof pNew->ev_name);
	pNew->ev_function = f;
	return 1;
}


/*
 * Register some "default" evaluators, which can be referenced in grammar files given their respective names.
 */
void evaluator_registerDefaultEvaluators() {
	evaluator_register("pks-macros", evaluator_evaluateMacros);
}

/*
 * Destroy all known evaluators.
 */
void evaluator_destroyEvaluators() {
	ll_destroy(&_evaluators, NULL);
}


/*
 * Evaluate the current selection or the line containing the cursor.
 * If the selection exists a maximum size, return an error.
 */
long long evaluator_evaluateCurrentSelection() {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (wp == NULL) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	char* pszEvaluatorName = grammar_getEvaluator(fp->documentDescriptor->grammar);
	EVALUATOR* pEvaluator = pszEvaluatorName ? ll_find(_evaluators, pszEvaluatorName) : NULL;
	if (pEvaluator == NULL) {
		error_showErrorById(IDS_NO_EVALUATION_SUPPORTED);
		return 0;
	}
	BOOL bSuccess = TRUE;
	char* pszText;
	if (!ww_hasSelection(wp)) {
		pszText = _strdup(wp->caret.linePointer->lbuf);
	} else {
		size_t nMaxSize = MAX_SELECTION_SIZE_TO_EXECUTE;
		pszText = calloc(nMaxSize, 1);
		bSuccess = bl_getSelectedText(wp, pszText, nMaxSize);
	}
	if (bSuccess) {
		pEvaluator->ev_function(pszText, EV_EXECUTE);
	} else {
		error_showErrorById(IDS_EVALUATION_FAILED);
	}
	free(pszText);
	return 1;
}
