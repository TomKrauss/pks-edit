/*
 * evaluator.h
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

#ifndef EVALUATOR_H
#define EVALUATOR_H

typedef enum { EV_EXECUTE } EVALUATION_ACTION;

typedef char* (*EVALUATION_FUNCTION)(const char* pszCode, EVALUATION_ACTION anAction);

/*
 * Register some "default" evaluators, which can be referenced in grammar files given their respective names.
 */
extern void evaluator_registerDefaultEvaluators();

/*
 * Destroy all known evaluators.
 */
extern void evaluator_destroyEvaluators();

/*
 * Evaluate the current selection or the line containing the cursor.
 * If the selection exists a maximum size, return an error.
 */
extern int evaluator_evaluateCurrentSelection();

#endif
