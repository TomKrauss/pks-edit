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
#include "winfo.h"
#include "funcdef.h"
#include "stringutil.h"

#define MAX_TYPES		64

static PKS_TYPE_DESCRIPTOR* _typeDescriptors[MAX_TYPES];
static int _maxTypeIndex;

/*
 * Destroy a type descriptor created with ...register...
 */
static void types_destroyDescriptor(PKS_TYPE_DESCRIPTOR* pType) {
	if(!pType) {
		return;
	}
	if (pType->ptd_isEnumType) {
		PARAMETER_ENUM_VALUE* pTd = pType->ptd_elements.ptd_enumValues;
		if (pTd) {
			for (int nProp = 0; nProp < pType->ptd_numberOfProperties; nProp++) {
				free((char*)pTd[nProp].pev_description);
				free((char*)pTd[nProp].pev_name);
			}
			free(pTd);
		}
	}
	else {
		TYPE_PROPERTY_DESCRIPTOR* pDescriptors = pType->ptd_elements.ptd_properties;
		if (pDescriptors) {
			for (int nProp = 0; nProp < pType->ptd_numberOfProperties; nProp++) {
				free((char*)pDescriptors[nProp].tpd_name);
			}
			free(pDescriptors);
		}
	}
	free((char*)pType->ptd_name);
	free((char*)pType->ptd_documentation);
	free(pType);
}

/*
 * Destroy all types previously registered.
 */
void types_destroy() {
	for (int i = 0; i < _maxTypeIndex; i++) {
		types_destroyDescriptor(_typeDescriptors[i]);
		_typeDescriptors[i] = 0;
	}
	_maxTypeIndex = 0;
}

/*
 * Register one type in the MacroC type registry. If 'nPreferredIndex is greater or equals 0
 * it is used as the internal type index in PKSMacroC, if it is negative, a dynamic type index
 * is created.
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
			// do not register "special types names"
			sym_createSymbol(sym_getKeywordContext(), (char*)pDescriptor->ptd_name, S_TYPE_IDENTIFIER, t, (GENERIC_DATA) { 0 }, 0);
		}
		pDescriptor->ptd_isEnumType = pTemplate->ptd_isEnumType;
	} else {
		if (pDescriptor->ptd_isEnumType != pTemplate->ptd_isEnumType) {
			return 0;
		}
		free((char*)pDescriptor->ptd_documentation);
		pDescriptor->ptd_documentation = 0;
	}
	if (pTemplate->ptd_documentation) {
		pDescriptor->ptd_documentation = _strdup(pTemplate->ptd_documentation);
	}
	pDescriptor->ptd_hasDefaultValue = pTemplate->ptd_hasDefaultValue;
	pDescriptor->ptd_callbacks = pTemplate->ptd_callbacks;
	pDescriptor->ptd_hasDynamicSize = pTemplate->ptd_hasDynamicSize;
	pDescriptor->ptd_isValueType = pTemplate->ptd_isValueType;
	pDescriptor->ptd_isHandleType = pTemplate->ptd_isHandleType;
	pDescriptor->ptd_numberOfProperties = pTemplate->ptd_numberOfProperties;
	if (t >= _maxTypeIndex) {
		_maxTypeIndex = t+1;
	}
	if (pTemplate->ptd_isEnumType) {
		if (pTemplate->ptd_elements.ptd_enumValues) {
			pDescriptor->ptd_elements.ptd_enumValues = calloc(pTemplate->ptd_numberOfProperties, sizeof * pTemplate->ptd_elements.ptd_enumValues);
			PARAMETER_ENUM_VALUE* pSource = pTemplate->ptd_elements.ptd_enumValues;
			PARAMETER_ENUM_VALUE* pTarget = pDescriptor->ptd_elements.ptd_enumValues;
			for (int i = 0; i < pDescriptor->ptd_numberOfProperties; i++) {
				if (pSource->pev_description) {
					pTarget->pev_description = _strdup(pSource->pev_description);
				}
				if (pSource->pev_name) {
					pTarget->pev_name = _strdup(pSource->pev_name);
					sym_createSymbol(sym_getKeywordContext(), (char*)pTarget->pev_name, S_ENUM, 0, (GENERIC_DATA) {
						.val = (intptr_t)pTarget
					}, 0);
				}
				pTarget->pev_val = pSource->pev_val;
				pSource++;
				pTarget++;
			}
		}
	}
	else {
		if (pTemplate->ptd_elements.ptd_properties) {
			pDescriptor->ptd_elements.ptd_properties = calloc(pTemplate->ptd_numberOfProperties, sizeof * pTemplate->ptd_elements.ptd_properties);
			for (int i = 0; i < pDescriptor->ptd_numberOfProperties; i++) {
				pDescriptor->ptd_elements.ptd_properties[i].tpd_type = pTemplate->ptd_elements.ptd_properties[i].tpd_type;
				pDescriptor->ptd_elements.ptd_properties[i].tpd_name = _strdup(pTemplate->ptd_elements.ptd_properties[i].tpd_name);
			}
		}
	}
	return 1;
}

/*
 * Returns a pks-value-type for a given type name. 
 */
PKS_VALUE_TYPE types_typeIndexFor(const char* pszTypename) {
	char* ret;
	SYMBOL sym = sym_find(sym_getKeywordContext(), pszTypename, &ret);
	if (NULLSYM(sym) || sym.s_type != S_TYPE_IDENTIFIER) {
		interpreter_raiseError("Illegal typename %s used", pszTypename);
	}
	return sym.s.symbol.s_valueType;
}

/*
 * Close a file, when a FILE type object is garbagge collected.
 */
static void file_close(PKS_VALUE v) {
	FILE* fp = (FILE*)TOP_DATA_POINTER(memory_getNestedObjectPointer(v, 0));
	if (fp) {
		fclose(fp);
		memory_setNestedObject(v, 0, (PKS_VALUE) {.pkv_type = VT_NIL});
	}
}

/*
 * Converts a WINFO structure to a HWND handle to be stored in the EDITOR_HANDLE. 
 */
static void* types_toHwnd(WINFO* wp) {
	return wp->ww_handle;
}

/*
 * Register the default types of PKSMacroC
 */
void types_registerDefaultTypes() {
	if (_maxTypeIndex > 0) {
		return;
	}
	types_register(VT_NIL, &(PKS_TYPE_DESCRIPTOR) { .ptd_name = "#undefined#", .ptd_isValueType = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_BOOLEAN, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "boolean", .ptd_isValueType = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_NUMBER, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "int", .ptd_isValueType = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_STRING, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "string", .ptd_isValueType = 0, .ptd_hasDefaultValue = 1});
	types_register(VT_FLOAT, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "float", .ptd_isValueType = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_CHAR, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "char", .ptd_isValueType = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_RANGE, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "range", .ptd_isValueType = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_OBJECT_ARRAY, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "#array#", .ptd_isValueType = 0, .ptd_hasDynamicSize = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_MAP, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "map", .ptd_isValueType = 0, .ptd_hasDynamicSize = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_AUTO, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = "auto", .ptd_isValueType = 1, .ptd_hasDefaultValue = 1});
	types_register(VT_FILE, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = PKS_TYPE_FILE, .ptd_isValueType = 0, .ptd_objectSize = 1, .ptd_hasDefaultValue = 0, 
		.ptd_isHandleType = 1,
		.ptd_callbacks = {
			.tc_close = (T_FINALIZER)file_close
		}});
	TYPE_PROPERTY_DESCRIPTOR descriptors[] = {
			{.tpd_type = VT_STRING, .tpd_name = "key"},
			{.tpd_type = VT_AUTO, .tpd_name = "value"},
	};
	types_register(VT_MAP_ENTRY, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = PKS_TYPE_MAP_ENTRY, .ptd_isValueType = 0, .ptd_objectSize = 1, .ptd_hasDefaultValue = 0,
		.ptd_elements.ptd_properties = descriptors, .ptd_numberOfProperties = DIM(descriptors)
	});
	types_register(VT_EDITOR_HANDLE, &(PKS_TYPE_DESCRIPTOR) {.ptd_name = PKS_TYPE_EDITOR, .ptd_isValueType = 0, .ptd_objectSize = 1, .ptd_hasDefaultValue = 0,
		.ptd_isHandleType = 1,
		.ptd_callbacks = {
			.tc_handleFromMacroMemory = ww_winfoFromWorkwinHandle,
			.tc_handleToMacroMemory = types_toHwnd,
	}});
}

/*
 * Answer true, if the passed type exists.
 */
int types_existsType(PKS_VALUE_TYPE t) {
	return t >= 0 && t < MAX_TYPES && _typeDescriptors[t];
}

/*
 * Returns the name of a given PKSMacroC value type.
 */
const char* types_nameFor(PKS_VALUE_TYPE t) {
	if (t == VT_NIL) {
		return "void";
	}
	if (t == VT_OBJECT_ARRAY) {
		return "string[]";
	}
	PKS_TYPE_DESCRIPTOR* pDescriptor = t>=0 && t < MAX_TYPES ? _typeDescriptors[t] : 0;
	return pDescriptor ? pDescriptor->ptd_name : "?";
}

/*
 * Return the finalizer for a given value type or 0 if not finalizer exists.
 */
T_FINALIZER types_getFinalizer(PKS_VALUE_TYPE vType) {
	PKS_TYPE_DESCRIPTOR* pDescriptor = _typeDescriptors[vType];
	return pDescriptor ? pDescriptor->ptd_callbacks.tc_close : 0;
}

/*
 * Return the converter for a given value handle or 0 if not converter exists.
 */
T_CONVERT_HANDLE types_getConverterFromMemory(PKS_VALUE_TYPE vType) {
	return _typeDescriptors[vType]->ptd_callbacks.tc_handleFromMacroMemory;
}

/*
 * Return the converter for a given value handle or 0 if not converter exists.
 */
T_CONVERT_HANDLE types_getConverterToMemory(PKS_VALUE_TYPE vType) {
	return _typeDescriptors[vType]->ptd_callbacks.tc_handleToMacroMemory;
}

/*
 * Returns true, if a type is a value type.
 */
BOOL types_isValueType(PKS_VALUE_TYPE vType) {
	return _typeDescriptors[vType]->ptd_isValueType;
}

/*
 * Returns true, if this is a "handle" type wrapping a native C pointer (FILE*, WINFO*, ...) 
 */
BOOL types_isHandleType(PKS_VALUE_TYPE vType) {
	return _typeDescriptors[vType]->ptd_isHandleType;
}

/*
 * Returns true, if objects of the given type are automatically created when declaring them.
 */
BOOL types_hasDefaultValue(PKS_VALUE_TYPE vType) {
	return _typeDescriptors[vType]->ptd_hasDefaultValue;
}

/*
 * Used for structured data types to define the number of instance variables. 
 */
int types_getObjectSize(PKS_VALUE_TYPE t) {
	if (t >= 0 && t < _maxTypeIndex) {
		PKS_TYPE_DESCRIPTOR* pDescriptor = _typeDescriptors[t];
		return pDescriptor->ptd_numberOfProperties;
	}
	return 0;
}

/*
 * Returns true, if the passed value type is a "structured" type.
 */
inline BOOL types_isStructuredType(PKS_VALUE_TYPE t) {
	return types_getObjectSize(t) > 0;
}

/*
 * Returns the index for a given types property name or -1, if that cannot be determined.
 */
int types_indexForProperty(PKS_VALUE_TYPE t, const char* pszPropertyName, PKS_VALUE_TYPE* tProperty) {
	if (t >= 0 && t < _maxTypeIndex) {
		PKS_TYPE_DESCRIPTOR* pDescriptor = _typeDescriptors[t];
		if (pDescriptor != 0 && !pDescriptor->ptd_isEnumType && pDescriptor->ptd_elements.ptd_properties != 0) {
			for (int i = 0; i < pDescriptor->ptd_numberOfProperties; i++) {
				if (strcmp(pszPropertyName, pDescriptor->ptd_elements.ptd_properties[i].tpd_name) == 0) {
					*tProperty = pDescriptor->ptd_elements.ptd_properties[i].tpd_type;
					return i;
				}
			}
		}
	}
	return -1;
}

/*
 * Returns the name of a property of a structured object given the type index and the property index.
 */
const char* types_getPropertyName(PKS_VALUE_TYPE t, int aPropertyIndex) {
	if (t >= 0 && t < _maxTypeIndex) {
		PKS_TYPE_DESCRIPTOR* pDescriptor = _typeDescriptors[t];
		if (pDescriptor != 0 && !pDescriptor->ptd_isEnumType && pDescriptor->ptd_elements.ptd_properties != 0 && pDescriptor->ptd_numberOfProperties > aPropertyIndex) {
			return pDescriptor->ptd_elements.ptd_properties[aPropertyIndex].tpd_name;
		}
	}
	return 0;
}

/*
 * Returns the "simple" documentation for a type.
 */
const char* types_getDocumentationFor(const char* pszTypeName) {
	char* unused;
	SYMBOL s = sym_find(sym_getKeywordContext(), pszTypeName, &unused);
	if (s.s_type != S_TYPE_IDENTIFIER) {
		return 0;
	}
	PKS_VALUE_TYPE t = s.s.symbol.s_valueType;
	return _typeDescriptors[t]->ptd_documentation;
}
