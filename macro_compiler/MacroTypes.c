/*
 * MacroTypes.c
 *
 * Describes the types of the PKSMacroC scripting language.
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 13.03.2022
 */

#include "alloc.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "symbols.h"

#define MAX_TYPES		64

static PKS_TYPE_DESCRIPTOR* _typeDescriptors[MAX_TYPES];
static int _maxTypeIndex;

/*
 * Destroy all types previously registered.
 */
void types_destroy() {
	for (int i = 0; i < _maxTypeIndex; i++) {
		free((char*)_typeDescriptors[i]->ptd_name);
		free(_typeDescriptors[i]);
	}
	_maxTypeIndex = 0;
}

/*
 * Register one type in the MacroC type registry.
 */
int types_register(int nPreferredIndex, PKS_TYPE_DESCRIPTOR *pTemplate) {
	PKS_VALUE_TYPE t;
	if (nPreferredIndex >= 0) {
		t = nPreferredIndex;
	} else {
		char* unused;
		SYMBOL s = sym_find(sym_getKeywordContext(), pTemplate->ptd_name, &unused);
		if (s.s_type == S_TYPE_IDENTIFIER) {
			t = s.s.symbol.s_valueType;
		}
		else {
			t = _maxTypeIndex;
			if (t >= MAX_TYPES) {
				return 0;
			}
		}
	}
	PKS_TYPE_DESCRIPTOR* pDescriptor = _typeDescriptors[t];
	if (pDescriptor == 0) {
		pDescriptor = calloc(1, sizeof * pDescriptor);
		pDescriptor->ptd_name = _strdup(pTemplate->ptd_name);
		_typeDescriptors[t] = pDescriptor;
		pDescriptor->ptd_type = t;
		if (pDescriptor->ptd_name[0] != '#') {
			// do not register "special types"
			sym_createSymbol(sym_getKeywordContext(), (char*)pDescriptor->ptd_name, S_TYPE_IDENTIFIER, t, (GENERIC_DATA) { 0 });
		}
	}
	pDescriptor->ptd_callbacks = pTemplate->ptd_callbacks;
	pDescriptor->ptd_hasDynamicSize = pTemplate->ptd_hasDynamicSize;
	pDescriptor->ptd_isValueType = pTemplate->ptd_isValueType;
	pDescriptor->ptd_numberOfProperties = pTemplate->ptd_numberOfProperties;
	if (t >= _maxTypeIndex) {
		_maxTypeIndex = t+1;
	}
	// TODO - more
	return 1;
}

/*
 * Register the default types of PKSMacroC
 */
void types_registerDefaultTypes() {
	if (_maxTypeIndex > 0) {
		return;
	}
	types_register(VT_NIL, &(PKS_TYPE_DESCRIPTOR) { .ptd_name = "#undefined#", .ptd_isValueType = 1});
	types_register(VT_BOOLEAN, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "boolean", .ptd_isValueType = 1});
	types_register(VT_NUMBER, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "int", .ptd_isValueType = 1});
	types_register(VT_STRING, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "string", .ptd_isValueType = 0});
	types_register(VT_FLOAT, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "float", .ptd_isValueType = 1});
	types_register(VT_CHAR, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "char", .ptd_isValueType = 1});
	types_register(VT_RANGE, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "range", .ptd_isValueType = 1});
	types_register(VT_OBJECT_ARRAY, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "#array#", .ptd_isValueType = 0, .ptd_hasDynamicSize = 1});
	types_register(VT_MAP, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "map", .ptd_isValueType = 0, .ptd_hasDynamicSize = 1});
	types_register(VT_AUTO, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "auto", .ptd_isValueType = 1});
}

/*
 * Returns the name of a given PKSMacroC value type.
 */
const char* types_nameFor(PKS_VALUE_TYPE t) {
	if (t == VT_ARRAY) {
		return "string[]";
	}
	PKS_TYPE_DESCRIPTOR* pDescriptor = t>=0 && t < MAX_TYPES ? _typeDescriptors[t] : 0;
	return pDescriptor ? pDescriptor->ptd_name : "??";
}
