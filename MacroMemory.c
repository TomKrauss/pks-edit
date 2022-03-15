/*
 * MacroMemory.c
 *
 * PKS-EDIT interpreter engine simple memory management module
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, vTarget. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 11/89
 */

#include "alloc.h"
#include <string.h>
#include <windows.h>

#include "arraylist.h"
#include "hashmap.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "symbols.h"
#include "trace.h"

#define INITIAL_OT_SIZE			2048

typedef struct tagOBJECT_DATA {
	char od_gcFlag : 1;						// A flag for the mark phase of the mark&sweep algo to mark the object.
	PKS_VALUE_TYPE od_class : 15;			// either a primitive value type or an index into the class object description table.
	int	  od_size;							// either the number of objects (size of array type objects) or the length of a string
	int   od_capacity;						// capacity for number of dependent objects.
	union {
		const char string[0];				// for strings the start of the zero terminated string 
		TYPED_OBJECT_POINTER objects[0];	// for arrays and structured objects the array of dependent objects. For hashmaps contains a pointer to hashmap
	} od_data;
} OBJECT_DATA;

typedef struct tagOBJECT_MEMORY {
	int om_capacity;						// The number of available objects.
	int om_freeSlot;						// last free slot where new objects are inserted
	OBJECT_DATA** om_objects;				// list of allocated objects to be released, when execution halts.
} OBJECT_MEMORY;

static int memory_markObject(void* pPointer);

static OBJECT_MEMORY _objectSpace;

static inline void memory_assignSlot(OBJECT_MEMORY* pMemory, int idx, OBJECT_DATA* pData) {
	pMemory->om_objects[idx] = pData;
}

static HASHMAP* memory_accessMap(OBJECT_DATA* pData) {
	return (HASHMAP*)pData->od_data.objects[0];
}

static void memory_destroyData(OBJECT_DATA** pData) {
	OBJECT_DATA* pOD = *pData;
	if (pOD) {
		T_FINALIZER finalizer = types_getFinalizer(pOD->od_class);
		if (finalizer) {
			finalizer((PKS_VALUE) { .pkv_managed = 1, .pkv_isPointer = 1, .sym_type = pOD->od_class, .sym_data.objectPointer = pOD});
		}
		if (pOD->od_class == VT_MAP) {
			hashmap_destroy(memory_accessMap(pOD), 0);
		}
		free(pOD);
		*pData = 0;
	}
}

/*
 * Free the allocated object memory and all objects remaining in the memory.
 */
void memory_destroy() {
	OBJECT_MEMORY* pMemory = &_objectSpace;
	for (int i = 0; i < pMemory->om_capacity; i++) {
		memory_destroyData(&pMemory->om_objects[i]);
	}
	free(pMemory->om_objects);
	pMemory->om_objects = NULL;
	pMemory->om_capacity = 0;
}

/*
 * Creates a new object memory;
 */
static void memory_create() {
	OBJECT_MEMORY* pMemory = &_objectSpace;
	if (pMemory->om_objects == 0) {
		int nCapacity = INITIAL_OT_SIZE;
		pMemory->om_objects = calloc(nCapacity, sizeof(PKS_VALUE));
		pMemory->om_capacity = nCapacity;
	}
}

static int memory_markMapEntries(intptr_t k, intptr_t v, void* unused) {
	TYPED_OBJECT_POINTER pPointer = k;
	memory_markObject(TOP_DATA_POINTER(pPointer));
	pPointer = v;
	if (TOP_IS_POINTER(pPointer)) {
		memory_markObject(TOP_DATA_POINTER(pPointer));
	}
	return 1;
}

static int memory_markObject(void* pPointer) {
	OBJECT_DATA* pData = pPointer;
	if (pData->od_gcFlag) {
		return 1;
	}
	pData->od_gcFlag = 1;
	if (pData->od_class != VT_STRING) {
		if (pData->od_class == VT_MAP) {
			HASHMAP* pMap = (HASHMAP * )TOP_DATA_POINTER(pData->od_data.objects[0]);
			hashmap_forEachEntry(pMap, memory_markMapEntries, 0);
		}
		else {
			for (int i = 0; i < pData->od_size; i++) {
				TYPED_OBJECT_POINTER top = pData->od_data.objects[i];
				if (TOP_IS_POINTER(top)) {
					memory_markObject(TOP_DATA_POINTER(top));
				}
			}
		}
	}
	return 1;
}

/*
 * 1st phase of GC: mark all reachable objects. 
 */
static void memory_markObjects(EXECUTION_CONTEXT* pContext) {
	OBJECT_MEMORY* pMemory = &_objectSpace;
	size_t nSize = pMemory->om_capacity;
	for (int i = (int)nSize; --i >= 0; ) {
		OBJECT_DATA* pszEntry = pMemory->om_objects[i];
		if (pszEntry) {
			pszEntry->od_gcFlag = 0;
		}
	}
	EXECUTION_CONTEXT* pCtx = pContext;
	while (pCtx) {
		sym_traverseManagedObjects(pCtx->ec_identifierContext, memory_markObject);
		pCtx = pCtx->ec_parent;
	}
	sym_traverseManagedObjects(sym_getGlobalContext(), memory_markObject);
	PKS_VALUE* pValue = pContext->ec_stackBottom;
	while (pValue <= pContext->ec_stackCurrent) {
		if (pValue->pkv_managed) {
			memory_markObject(pValue->sym_data.objectPointer);
		}
		pValue++;
	}
}

/*
 * Try to get rid of some allocated memory not referenced by the stack any more.
 * This method must be invoked with care and not in the middle of an operation
 * allocating more objects not yet referenced or the objects just created might
 * get disposed to early.
 */
void memory_garbaggeCollect(EXECUTION_CONTEXT* pContext) {
	OBJECT_MEMORY* pMemory = &_objectSpace;
	if (!pMemory->om_objects || pMemory->om_freeSlot < pMemory->om_capacity - (INITIAL_OT_SIZE/8)) {
		return;
	}
	memory_markObjects(pContext);
	size_t nSize = pMemory->om_capacity;
	EdTRACE(int nCollected = 0; int nObjects = 0;)
	for (int i = (int)nSize; --i >= 0; ) {
		OBJECT_DATA* pData = pMemory->om_objects[i];
		if (!pData) {
			continue;
		}
		EdTRACE(nObjects++;)
		if (!pData->od_gcFlag) {
			EdTRACE(nCollected++;)
			memory_destroyData(&pMemory->om_objects[i]);
			if (i < pMemory->om_freeSlot) {
				pMemory->om_freeSlot = i;
			}
		}
	}
	EdTRACE(log_errorArgs(DEBUG_TRACE, "GC finished: reclaimed %d objects out of %d", nCollected, nObjects));
}

/*
 * Return the next available object slot into our object table.
 */
static int memory_nextFreeSlot(OBJECT_MEMORY* pMemory) {
	int idx = pMemory->om_freeSlot;
	while (idx < pMemory->om_capacity) {
		if (!pMemory->om_objects[idx]) {
			return idx;
		}
		idx++;
	}
	int nNewCapacity = pMemory->om_capacity + (INITIAL_OT_SIZE/2);
	pMemory->om_objects = realloc(pMemory->om_objects, nNewCapacity * sizeof(OBJECT_DATA*));
	for (int i = pMemory->om_capacity; i < nNewCapacity; i++) {
		memory_assignSlot(pMemory, i, 0);
	}
	idx = pMemory->om_capacity;
	pMemory->om_capacity = nNewCapacity;
	return idx;
}

static int memory_comparePointer(intptr_t a, intptr_t b) {
	return strcmp((TOP_DATA_POINTER(a))->od_data.string, (TOP_DATA_POINTER(b))->od_data.string);
}

static int memory_hashPointer(intptr_t k) {
	return hashmap_hashCodeString((intptr_t)(TOP_DATA_POINTER(k))->od_data.string);
}

static OBJECT_DATA* memory_createObjectData(EXECUTION_CONTEXT* pContext, PKS_VALUE_TYPE sType, int nInitialSize, const void* pInput) {
	memory_create();
	OBJECT_MEMORY* pMemory = &_objectSpace;
	OBJECT_DATA* pData;
	size_t nLen = 0;
	if (sType == VT_MAP) {
		nLen = 1;
		pData = calloc(1, sizeof(OBJECT_DATA) + sizeof(OBJECT_DATA*));
		pData->od_data.objects[0] = MAKE_TYPED_OBJECT_POINTER(0,0, hashmap_create(nInitialSize ? nInitialSize : 19, memory_hashPointer, memory_comparePointer));
	} else if (sType == VT_STRING) {
		if (pInput) {
			nLen = strlen(pInput);
		}
		size_t l = sizeof(OBJECT_DATA) + nLen+1;
		pData = calloc(1, l);
		if (pInput) {
			strcpy((char*)pData->od_data.string, pInput);
		}
	}
	else if (sType == VT_OBJECT_ARRAY) {
		nLen = nInitialSize;
		if (pInput) {
			size_t nInitSize = arraylist_size((ARRAY_LIST*)pInput);
			if (nInitSize > nLen) {
				nLen = (int)nInitSize;
			}
		} else if (nLen <= 0) {
			nLen = 5;
		}
		pData = calloc(1, sizeof(OBJECT_DATA) + (nLen * sizeof(OBJECT_DATA*)));
	}
	else if (types_isValueType(sType)) {
		return 0;
	} else {
		nLen = nInitialSize;
		pData = calloc(1, sizeof(OBJECT_DATA) + (nLen * sizeof(OBJECT_DATA*)));
	}
	pData->od_size = (int)(pInput ? nLen : 0);
	pData->od_capacity = (int)nLen;
	pData->od_class = sType;
	int idx = memory_nextFreeSlot(pMemory);
	memory_assignSlot(pMemory, idx++, pData);
	pMemory->om_freeSlot = idx;
	return pData;
}

/*
 * Creates one object memory and optionally copies the provided native 
 * data into the object memory. Currently two special cases for input are supported:
 * the value is a string - input is interpreted as zero terminated byte string char*,
 * the value is an arraylist of strings - input is interpreted as an ARRAY_LIST containing
 * strings.
 */
PKS_VALUE memory_createObject(EXECUTION_CONTEXT* pContext, PKS_VALUE_TYPE sType, int nInitialSize, const void *pInput) {
	OBJECT_MEMORY* pMemory = &_objectSpace;
	OBJECT_DATA* pData = memory_createObjectData(pContext, sType, nInitialSize, pInput);
	if (!pData) {
		return (PKS_VALUE) {
			.sym_type = sType, .sym_data.val = (intptr_t)pInput
		};
	}
	if (sType == VT_OBJECT_ARRAY && pInput) {
		size_t nLen = pData->od_size;
		for (int i = 0; i < nLen; i++) {
			TYPED_OBJECT_POINTER pszPointer = (TYPED_OBJECT_POINTER)arraylist_get((ARRAY_LIST*)pInput, i);
			PKS_VALUE_TYPE t = TOP_TYPE(pszPointer);
			// Hack: t == 0 means we create an object from a native string array - not yet converted to TYPED_OBJECT_POINTERS
			if (t == VT_STRING || t == 0) {
				pszPointer = MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, memory_createObjectData(pContext, VT_STRING, 0, TOP_DATA_POINTER(pszPointer)));
			}
			pData->od_data.objects[i] = pszPointer;
		}
	} else if (sType == VT_MAP && pInput) {
		size_t nLen = pData->od_size;
		HASHMAP* pMap = memory_accessMap(pData);
		for (int i = 0; i < nLen-1; i += 2) {
			TYPED_OBJECT_POINTER pszPointer1 = (TYPED_OBJECT_POINTER)arraylist_get((ARRAY_LIST*)pInput, i);
			TYPED_OBJECT_POINTER pszPointer2 = (TYPED_OBJECT_POINTER)arraylist_get((ARRAY_LIST*)pInput, i+1);
			PKS_VALUE_TYPE t = TOP_TYPE(pszPointer2);
			pszPointer1 = MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, memory_createObjectData(pContext, VT_STRING, 0, TOP_DATA_POINTER(pszPointer1)));
			if (t == VT_STRING || t == 0) {
				pszPointer2 = MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, memory_createObjectData(pContext, VT_STRING, 0, TOP_DATA_POINTER(pszPointer2)));
			}
			hashmap_put(pMap, pszPointer1, pszPointer2);
		}
	}
	return (PKS_VALUE) { .sym_type = sType, .sym_data.objectPointer = pData, .pkv_isPointer = 1, .pkv_managed = 1 };
}

/*
 * Add one object to an array type object.
 */
int memory_addObject(EXECUTION_CONTEXT* pContext, PKS_VALUE *vObject, PKS_VALUE vElement) {
	if (!vObject->pkv_managed || !vElement.pkv_managed) {
		return 0;
	}
	OBJECT_DATA* pData = vObject->sym_data.objectPointer;
	if (pData->od_size >= pData->od_capacity) {
		size_t oldSize = sizeof(OBJECT_DATA) + pData->od_capacity * sizeof(OBJECT_DATA*);
		pData->od_capacity += 10;
		size_t newSize = sizeof(OBJECT_DATA) + pData->od_capacity * sizeof(OBJECT_DATA*);
		int objIdx = 0;
		while (objIdx < _objectSpace.om_capacity) {
			if (_objectSpace.om_objects[objIdx] == pData) {
				break;
			}
			objIdx++;
		}
		pData = realloc(pData, newSize);
		if (!pData) {
			return 0;
		}
		if (objIdx < _objectSpace.om_capacity) {
			memory_assignSlot(&_objectSpace, objIdx, pData);
		}
		vObject->sym_data.objectPointer = pData;
		size_t diff = newSize - oldSize;
		void* pStart = ((char*)pData) + oldSize;
		memset(pStart, 0, diff);
	}
	BOOL bPointer = vElement.pkv_isPointer;
	pData->od_data.objects[pData->od_size++] = MAKE_TYPED_OBJECT_POINTER(bPointer, vElement.sym_type, vElement.sym_data.objectPointer);
	return 1;
}

/*
 * Return the logical size of one object - either the number of chars for a string or the number of instance
 * variables or the length of an array. Return 0 for other primitive types.
 */
int memory_size(PKS_VALUE v) {
	if (v.pkv_managed && v.pkv_isPointer) {
		return ((OBJECT_DATA*)v.sym_data.objectPointer)->od_size;
	}
	return 0;
}

/*
 * Get a string pointer to the actual string for a value.
 */
const char* memory_accessString(PKS_VALUE v) {
	if (v.pkv_managed && v.sym_type == VT_STRING) {
		return ((OBJECT_DATA*)v.sym_data.objectPointer)->od_data.string;
	}
	return "";
}

/*
 * Set a nested pointer of a value at slot nIndex. Can be used to store "handles" in an object rather
 * than a nested object / value.
 */
int memory_setNestedPointer(PKS_VALUE vTarget, int nIndex, TYPED_OBJECT_POINTER vPointer) {
	if (vTarget.pkv_managed) {
		OBJECT_DATA* pPointer = ((OBJECT_DATA*)vTarget.sym_data.objectPointer);
		if (nIndex >= 0 && nIndex < pPointer->od_capacity) {
			pPointer->od_data.objects[nIndex] = vPointer;
			if (nIndex >= pPointer->od_size) {
				pPointer->od_size = nIndex + 1;
			}
			return 1;
		}
		interpreter_raiseError("Index %d out of range[%d..%d] for assigning array element.", nIndex, 0, pPointer->od_capacity);
	}
	return 0;
}


/*
 * Set a nested object of a value at slot nIndex.
 */
int memory_setNestedObject(PKS_VALUE vTarget, int nIndex, PKS_VALUE vElement) {
	return memory_setNestedPointer(vTarget, nIndex, MAKE_TYPED_OBJECT_POINTER(vElement.pkv_isPointer, vElement.sym_type, vElement.sym_data.val));
}

static PKS_VALUE memory_asValue(TYPED_OBJECT_POINTER top) {
	PKS_VALUE_TYPE t = TOP_TYPE(top);
	if (TOP_IS_POINTER(top)) {
		return (PKS_VALUE) {
			.sym_type = t,
				.pkv_isPointer = 1,
				.pkv_managed = 1,
				.sym_data.objectPointer = TOP_DATA_POINTER(top)
		};
	}
	if (t == 0) {
		t = VT_BOOLEAN;
	}
	return (PKS_VALUE) {
		.sym_type = t,
			.pkv_isPointer = 0,
			.pkv_managed = 0,
			.sym_data.longValue = (long)(intptr_t)TOP_DATA_POINTER(top)
	};
}

/*
 * Access a nested object pointer of a value at slot nIndex.
 */
TYPED_OBJECT_POINTER memory_getNestedObjectPointer(PKS_VALUE v, int nIndex) {
	if (v.pkv_managed) {
		OBJECT_DATA* pPointer = ((OBJECT_DATA*)v.sym_data.objectPointer);
		if (nIndex >= 0 && nIndex < pPointer->od_capacity) {
			return pPointer->od_data.objects[nIndex];
		}
		interpreter_raiseError("Index %d out of range[%d..%d] for accessing array.", nIndex, 0, pPointer->od_capacity);
	}
	return 0;
}

/*
 * Access the nested object of a value at slot nIndex.
 */
PKS_VALUE memory_getNestedObject(PKS_VALUE v, int nIndex) {
	if (v.pkv_managed && v.sym_type == VT_OBJECT_ARRAY) {
		OBJECT_DATA* pPointer = ((OBJECT_DATA*)v.sym_data.objectPointer);
		if (nIndex >= 0 && nIndex < pPointer->od_size) {
			TYPED_OBJECT_POINTER top = pPointer->od_data.objects[nIndex];
			return memory_asValue(top);
		}
		interpreter_raiseError("Index %d out of range[%d..%d] for accessing array.", nIndex, 0, pPointer->od_capacity);
	}
	return (PKS_VALUE) { 0 };
}

/*
 * Set a nested object using a key (must be a string) assuming the target object is a map.
 */
int memory_atPutObject(PKS_VALUE vTarget, PKS_VALUE vKey, PKS_VALUE vElement) {
	if (vTarget.pkv_managed && vTarget.sym_type == VT_MAP && vKey.sym_type == VT_STRING) {
		HASHMAP* pMap = memory_accessMap(vTarget.sym_data.objectPointer);
		hashmap_put(pMap,
				MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, vKey.sym_data.objectPointer),
				MAKE_TYPED_OBJECT_POINTER(vElement.pkv_isPointer, vElement.sym_type, vElement.sym_data.val));
	}
	else {
		interpreter_raiseError("Can assign map elements only to maps with string type keys.");
	}
	return 0;
}

/*
 * Access an object by string key in a map.
 */
PKS_VALUE memory_atObject(PKS_VALUE vTarget, PKS_VALUE vKey) {
	if (vTarget.pkv_managed && vTarget.sym_type == VT_MAP && vKey.sym_type == VT_STRING) {
		HASHMAP* pMap = memory_accessMap(vTarget.sym_data.objectPointer);
		TYPED_OBJECT_POINTER top= hashmap_get(pMap, MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, vKey.sym_data.objectPointer));
		return memory_asValue(top);
	}
	interpreter_raiseError("Can only access map elements with string type keys.");
	return (PKS_VALUE) { 0 };
}

