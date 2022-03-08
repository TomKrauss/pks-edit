/*
 * MacroMemory.c
 *
 * PKS-EDIT interpreter engine simple memory management module
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 11/89
 */

#include <string.h>
#include <windows.h>


#include "arraylist.h"

#include "pksmacro.h"
#include "pksmacrocvm.h"

typedef struct tagOBJECT_MEMORY {
	int om_capacity;					// The number of available objects.
	int om_freeSlot;					// last free slot where new objects are inserted
	PKS_VALUE *om_objects;				// list of allocated objects to be released, when execution halts.
} OBJECT_MEMORY;

typedef struct tagOBJECT_DATA {
	short od_gcFlag : 1;				// A flag for the mark phase of the mark&sweep algo to mark the object.
	short od_class : 15;				// either a primitive value type or an index into the class object description table.
	int	  od_size;						// either the number of objects (size of array type objects) or the length of a string
	int   od_capacity;					// capacity for number of dependent objects.
	union {
		const char* string;				// for strings the actual pointer to the string
		struct tagOBJECTDATA* objects;	// for arrays and structured objects the array of dependent objects
	} od_data;
} OBJECT_DATA;

/*
 * Destroy one value from the object memory.
 */
static void memory_destroyValue(PKS_VALUE* pVal) {
	if (pVal->sym_data.string) {
		if (pVal->sym_type == VT_STRING) {
			free(pVal->sym_data.string);
		}
		else {
			arraylist_destroyStringList(pVal->sym_data.stringList);
		}
		pVal->sym_data.string = 0;
		pVal->sym_type = 0;
	}
}

/*
 * Free the allocated object memory and all objects remaining in the memory.
 */
void memory_destroy(OBJECT_MEMORY* pMemory) {
	for (int i = 0; i < pMemory->om_capacity; i++) {
		memory_destroyValue(&pMemory->om_objects[i]);
	}
	free(pMemory->om_objects);
	free(pMemory);
}

/*
 * Try to get rid of some allocated memory not referenced by the stack any more.
 */
static void memory_garbaggeCollect(EXECUTION_CONTEXT* pContext) {
	OBJECT_MEMORY* pMemory = pContext->ec_allocations;
	size_t nSize = pMemory->om_capacity;
	for (int i = (int)nSize; --i >= 0; ) {
		PKS_VALUE* pszEntry = &pMemory->om_objects[i];
		PKS_VALUE* pStackValues = pContext->ec_stackBottom;
		BOOL bDelete = TRUE;
		while (pStackValues <= pContext->ec_stackCurrent) {
			PKS_VALUE v = *pStackValues++;
			if (v.sym_data.string == pszEntry->sym_data.string && (v.sym_type == VT_STRING || v.sym_type == VT_STRING_ARRAY)) {
				bDelete = FALSE;
				break;
			}
		}
		if (bDelete) {
			memory_destroyValue(pszEntry);
			if (i < pMemory->om_freeSlot) {
				pMemory->om_freeSlot = i;
			}
		}
	}
}

/*
 * Adds one value to the object memory.
 */
void memory_add(EXECUTION_CONTEXT* pContext, PKS_VALUE value) {
	OBJECT_MEMORY* pMemory = pContext->ec_allocations;
	int idx = pMemory->om_freeSlot;
	if (idx > pMemory->om_capacity - 10) {
		memory_garbaggeCollect(pContext);
		idx = pMemory->om_freeSlot;
	}
	if (idx >= pMemory->om_capacity) {
		int nNewCapacity = pMemory->om_capacity + 32;
		pMemory->om_objects = realloc(pMemory->om_objects, nNewCapacity * sizeof(PKS_VALUE));
		pMemory->om_capacity = nNewCapacity;
	}
	pMemory->om_objects[idx] = value;
	while (idx < pMemory->om_capacity && pMemory->om_objects[idx].sym_data.string) {
		idx++;
	}
	pMemory->om_freeSlot = idx;
}

/*
 * Creates a new object memory;
 */
OBJECT_MEMORY* memory_create() {
	int nCapacity = 16;
	OBJECT_MEMORY* pResult = calloc(1, sizeof * pResult);
	pResult->om_objects = calloc(nCapacity, sizeof(PKS_VALUE));
	pResult->om_capacity = nCapacity;
	return pResult;
}


