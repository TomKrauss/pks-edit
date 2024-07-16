/*
 * MacroMemory.c
 *
 * PKS Edit interpreter engine simple memory management module
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

static ARRAY_LIST* memory_accessArray(OBJECT_DATA* pData) {
	return (ARRAY_LIST*)pData->od_data.objects[0];
}


static void memory_destroyData(OBJECT_DATA** pData) {
	OBJECT_DATA* pOD = *pData;
	if (pOD) {
		T_FINALIZER finalizer = types_getFinalizer(pOD->od_class);
		if (finalizer) {
			finalizer((PKS_VALUE) { .pkv_managed = 1, .pkv_isPointer = 1, .pkv_type = pOD->od_class, .pkv_data.objectPointer = pOD});
		}
		if (pOD->od_class == VT_MAP) {
			hashmap_destroy(memory_accessMap(pOD), 0);
		} else if (pOD->od_class == VT_OBJECT_ARRAY) {
			arraylist_destroy(memory_accessArray(pOD));
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
		} else if (pData->od_class == VT_OBJECT_ARRAY) {
			ARRAY_LIST* pList = (ARRAY_LIST*)TOP_DATA_POINTER(pData->od_data.objects[0]);
			ARRAY_ITERATOR pIter = arraylist_iterator(pList);
			while (pIter.i_buffer < pIter.i_bufferEnd) {
				TYPED_OBJECT_POINTER top = (TYPED_OBJECT_POINTER) * pIter.i_buffer;
				if (TOP_IS_POINTER(top)) {
					memory_markObject(TOP_DATA_POINTER(top));
				}
				pIter.i_buffer++;
			}
		} else {
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
		for (int i = 0; i < pCtx->ec_localVariableCount; i++) {
			PKS_VALUE* pVar = &pCtx->ec_localVariables[i];
			if (pVar->pkv_isPointer) {
				pVar->pkv_managed = 1;
				memory_markObject(pVar->pkv_data.objectPointer);
			}
		}
		pCtx = pCtx->ec_parent;
	}
	sym_traverseManagedObjects(sym_getGlobalContext(), memory_markObject);
	PKS_VALUE* pValue = pContext->ec_stackBottom;
	while (pValue <= pContext->ec_stackCurrent) {
		if (pValue->pkv_managed) {
			memory_markObject(pValue->pkv_data.objectPointer);
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
	EdTRACE(log_message(DEBUG_TRACE, "GC finished: reclaimed %d objects out of %d", nCollected, nObjects));
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
	int nCapacity = 1;
	if (sType == VT_MAP) {
		nLen = 0;
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
		nCapacity = (int)(nLen+1);
	}
	else if (sType == VT_OBJECT_ARRAY) {
		nLen = 0;
		size_t nArraySize = nInitialSize;
		if (pInput) {
			size_t nInitSize = arraylist_size((ARRAY_LIST*)pInput);
			if (nInitSize > nArraySize) {
				nArraySize = nInitSize;
			}
		}
		pData = calloc(1, sizeof(OBJECT_DATA) + sizeof(OBJECT_DATA*));
		pData->od_data.objects[0] = MAKE_TYPED_OBJECT_POINTER(0, 0, arraylist_create(nArraySize));
	}
	else if (types_isValueType(sType)) {
		return 0;
	} else {
		nLen = nInitialSize;
		nCapacity = (int)nLen;
		pData = calloc(1, sizeof(OBJECT_DATA) + (nLen * sizeof(OBJECT_DATA*)));
	}
	pData->od_size = (int)nLen;
	pData->od_capacity = nCapacity;
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
	OBJECT_DATA* pData = memory_createObjectData(pContext, sType, nInitialSize, pInput);
	if (!pData) {
		return (PKS_VALUE) {
			.pkv_type = sType, .pkv_data.val = (intptr_t)pInput
		};
	}
	if (sType == VT_OBJECT_ARRAY && pInput) {
		ARRAY_LIST* pTarget = memory_accessArray(pData);
		size_t nLen = arraylist_size((ARRAY_LIST*)pInput);
		for (int i = 0; i < nLen; i++) {
			TYPED_OBJECT_POINTER pszPointer = (TYPED_OBJECT_POINTER)arraylist_get((ARRAY_LIST*)pInput, i);
			PKS_VALUE_TYPE t = TOP_TYPE(pszPointer);
			// Hack: t == 0 means we create an object from a native string array - not yet converted to TYPED_OBJECT_POINTERS
			if (t == VT_STRING || t == 0) {
				pszPointer = MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, memory_createObjectData(pContext, VT_STRING, 0, TOP_DATA_POINTER(pszPointer)));
			}
		
			arraylist_add(pTarget, (void*)pszPointer);
		}
		pData->od_size = (int)nLen;
	} else if (sType == VT_MAP && pInput) {
		ARRAY_LIST* pList = (ARRAY_LIST*)pInput;
		int nLen = (int)arraylist_size(pList);
		HASHMAP* pMap = memory_accessMap(pData);
		for (int i = 0; i < nLen-1; i += 2) {
			TYPED_OBJECT_POINTER pszPointer1 = (TYPED_OBJECT_POINTER)arraylist_get(pList, i);
			TYPED_OBJECT_POINTER pszPointer2 = (TYPED_OBJECT_POINTER)arraylist_get(pList, i+1);
			PKS_VALUE_TYPE t = TOP_TYPE(pszPointer2);
			pszPointer1 = MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, memory_createObjectData(pContext, VT_STRING, 0, TOP_DATA_POINTER(pszPointer1)));
			if (t == VT_STRING || t == 0) {
				pszPointer2 = MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, memory_createObjectData(pContext, VT_STRING, 0, TOP_DATA_POINTER(pszPointer2)));
			}
			hashmap_put(pMap, (void*)pszPointer1, pszPointer2);
		}
		pData->od_size = (int)(nLen / 2);
	} else if (sType != VT_STRING && types_isStructuredType(sType)) {
		// structured objects
		pData->od_size = pData->od_capacity;
	}
	return (PKS_VALUE) { .pkv_type = sType, .pkv_data.objectPointer = pData, .pkv_isPointer = 1, .pkv_managed = 1 };
}

static int memory_collectKey(intptr_t k, intptr_t v, void* pParam) {
	PKS_VALUE* pArray = pParam;
	OBJECT_DATA* pData = pArray->pkv_data.objectPointer;
	ARRAY_LIST* pList = memory_accessArray(pData);
	arraylist_add(pList, (void*)k);
	pData->od_size++;
	return 1;
}

static int memory_collectValue(intptr_t k, intptr_t v, void* pParam) {
	PKS_VALUE* pArray = pParam;
	OBJECT_DATA* pData = pArray->pkv_data.objectPointer;
	ARRAY_LIST* pList = memory_accessArray(pData);
	arraylist_add(pList, (void*)v);
	pData->od_size++;
	return 1;
}

static EXECUTION_CONTEXT* _currentContext;

static int memory_collectEntries(intptr_t k, intptr_t v, void* pParam) {
	PKS_VALUE* pArray = pParam;
	OBJECT_DATA* pData = pArray->pkv_data.objectPointer;
	PKS_VALUE_TYPE sMapEntryType = VT_MAP_ENTRY;
	PKS_VALUE vEntry = memory_createObject(_currentContext, sMapEntryType, 2, 0);
	OBJECT_DATA* pEData = vEntry.pkv_data.objectPointer;
	pEData->od_data.objects[0] = (TYPED_OBJECT_POINTER)k;
	pEData->od_data.objects[1] = (TYPED_OBJECT_POINTER)v;
	ARRAY_LIST* pList = memory_accessArray(pData);
	arraylist_add(pList, (void*)MAKE_TYPED_OBJECT_POINTER(1, sMapEntryType, vEntry.pkv_data.objectPointer));
	pData->od_size++;
	return 1;
}

static PKS_VALUE memory_collectElements(EXECUTION_CONTEXT* pContext, PKS_VALUE vTarget, int (*func)(intptr_t k, intptr_t v, void *p)) {
	if (vTarget.pkv_managed && vTarget.pkv_type == VT_MAP) {
		_currentContext = pContext;
		HASHMAP* pMap = memory_accessMap(vTarget.pkv_data.objectPointer);
		PKS_VALUE tempArray = memory_createObject(pContext, VT_OBJECT_ARRAY, hashmap_size(pMap), 0);
		hashmap_forEachEntry(pMap, func, &tempArray);
		return tempArray;
	}
	interpreter_raiseError("Illegal map operation on a non map value %s", types_nameFor(vTarget.pkv_type));
	return (PKS_VALUE) { 0 };
}

/*
 * Returns an array object with all keys contained in the map type value passed as single argument.
 */
PKS_VALUE memory_mapKeys(EXECUTION_CONTEXT* pContext, const PKS_VALUE* pValues, int nArgs) {
	if (nArgs >= 1) {
		PKS_VALUE vTarget = pValues[0];
		return memory_collectElements(pContext, vTarget, memory_collectKey);
	}
	interpreter_raiseError("mapKeys requires an arguments");
	return (PKS_VALUE) {0};
}

/*
 * Returns an array object with all values contained in the map type value passed as single argument.
 */
PKS_VALUE memory_mapValues(EXECUTION_CONTEXT* pContext, const PKS_VALUE* pValues, int nArgs) {
	if (nArgs >= 1) {
		PKS_VALUE vTarget = pValues[0];
		return memory_collectElements(pContext, vTarget, memory_collectValue);
	}
	interpreter_raiseError("mapValues requires an arguments");
	return (PKS_VALUE) { 0 };
}

/*
 * Returns an array object with all MAP_ENTRY objects in the map type value passed as single argument.
 */
PKS_VALUE memory_mapEntries(EXECUTION_CONTEXT* pContext, const PKS_VALUE* pValues, int nArgs) {
	if (nArgs >= 1) {
		PKS_VALUE vTarget = pValues[0];
		return memory_collectElements(pContext, vTarget, memory_collectEntries);
	}
	interpreter_raiseError("mapEntries requires an arguments");
	return (PKS_VALUE) { 0 };
}

/*
 * Add one object to an array type object.
 */
int memory_addObject(EXECUTION_CONTEXT* pContext, PKS_VALUE *vObject, PKS_VALUE vElement) {
	OBJECT_DATA* pData = vObject->pkv_data.objectPointer;
	ARRAY_LIST* pTarget = memory_accessArray(pData);
	BOOL bPointer = vElement.pkv_isPointer;
	arraylist_add(pTarget, (void*)MAKE_TYPED_OBJECT_POINTER(bPointer, vElement.pkv_type, vElement.pkv_data.objectPointer));
	pData->od_size = (int)arraylist_size(pTarget);
	return 1;
}

/*
 * Return the logical size of one object - either the number of chars for a string or the number of instance
 * variables or the length of an array. Return 0 for other primitive types.
 */
int memory_size(PKS_VALUE v) {
	if (v.pkv_managed && v.pkv_isPointer) {
		return ((OBJECT_DATA*)v.pkv_data.objectPointer)->od_size;
	}
	return 0;
}

/*
 * Get a string pointer to the actual string for a value.
 */
const char* memory_accessString(PKS_VALUE v) {
	if (v.pkv_managed && v.pkv_type == VT_STRING) {
		return ((OBJECT_DATA*)v.pkv_data.objectPointer)->od_data.string;
	}
	return "";
}

/*
 * Set a nested pointer of a value at slot nIndex. Can be used to store "handles" in an object rather
 * than a nested object / value.
 */
int memory_setNestedPointer(PKS_VALUE vTarget, int nIndex, TYPED_OBJECT_POINTER vPointer) {
	if (vTarget.pkv_managed) {
		OBJECT_DATA* pPointer = ((OBJECT_DATA*)vTarget.pkv_data.objectPointer);
		if (pPointer->od_class == VT_OBJECT_ARRAY) {
			ARRAY_LIST* pList = memory_accessArray(pPointer);
			arraylist_set(pList, nIndex, (void*)vPointer);
			if (nIndex >= pPointer->od_size) {
				pPointer->od_size = nIndex + 1;
			}
			return 1;
		}
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
	return memory_setNestedPointer(vTarget, nIndex, MAKE_TYPED_OBJECT_POINTER(vElement.pkv_isPointer, vElement.pkv_type, vElement.pkv_data.val));
}

static PKS_VALUE memory_asValue(TYPED_OBJECT_POINTER top) {
	PKS_VALUE_TYPE t = TOP_TYPE(top);
	if (TOP_IS_POINTER(top)) {
		return (PKS_VALUE) {
			.pkv_type = t,
				.pkv_isPointer = 1,
				.pkv_managed = 1,
				.pkv_data.objectPointer = TOP_DATA_POINTER(top)
		};
	}
	if (t == 0) {
		t = VT_BOOLEAN;
	}
	return (PKS_VALUE) {
		.pkv_type = t,
			.pkv_isPointer = 0,
			.pkv_managed = 0,
			.pkv_data.longValue = (long)(intptr_t)TOP_DATA_POINTER(top)
	};
}

/*
 * Access a nested object pointer of a value at slot nIndex.
 */
TYPED_OBJECT_POINTER memory_getNestedObjectPointer(PKS_VALUE v, int nIndex) {
	if (v.pkv_managed) {
		OBJECT_DATA* pPointer = ((OBJECT_DATA*)v.pkv_data.objectPointer);
		if (pPointer->od_class == VT_OBJECT_ARRAY) {
			ARRAY_LIST* pList = memory_accessArray(pPointer);
			return (TYPED_OBJECT_POINTER)arraylist_get(pList, nIndex);
		}
		if (nIndex >= 0 && nIndex < pPointer->od_capacity) {
			return pPointer->od_data.objects[nIndex];
		}
		interpreter_raiseError("Index %d out of range[%d..%d] for accessing array.", nIndex, 0, pPointer->od_capacity);
	}
	return 0;
}

/*
 * Return the "index" of one object in a give array type object.
 */
int memory_indexOf(PKS_VALUE vArray, PKS_VALUE vOther) {
	if (vArray.pkv_managed && vArray.pkv_type == VT_OBJECT_ARRAY) {
		ARRAY_LIST* pList = memory_accessArray(((OBJECT_DATA*)vArray.pkv_data.objectPointer));
		size_t nSize = arraylist_size(pList);
		for (int i = 0; i < nSize; i++) {
			TYPED_OBJECT_POINTER top = (TYPED_OBJECT_POINTER)arraylist_get(pList, i);
			if (TOP_TYPE(top) == vOther.pkv_type) {
				if (vOther.pkv_type == VT_STRING) {
					if (strcmp(memory_accessString(vOther), (char*)TOP_DATA_POINTER(top)) == 0) {
						return i;
					}
				}
				else if (vOther.pkv_data.longValue == (long long)TOP_DATA_POINTER(top)) {
					return i;
				}
			}
		}
	}
	return -1;
}

/*
 * Access the nested object of a value at slot nIndex.
 */
PKS_VALUE memory_getNestedObject(PKS_VALUE v, int nIndex) {
	if (v.pkv_managed) {
		OBJECT_DATA* pPointer = ((OBJECT_DATA*)v.pkv_data.objectPointer);
		if (pPointer->od_class == VT_OBJECT_ARRAY) {
			ARRAY_LIST* pList = memory_accessArray(pPointer);
			TYPED_OBJECT_POINTER top = (TYPED_OBJECT_POINTER)arraylist_get(pList, nIndex);
			return memory_asValue(top);
		}
		if (nIndex >= 0 && nIndex < pPointer->od_size) {
			TYPED_OBJECT_POINTER top = pPointer->od_data.objects[nIndex];
			return memory_asValue(top);
		}
		interpreter_raiseError("Index %d out of range[%d..%d] for accessing array.", nIndex, 0, pPointer->od_capacity);
	}
	// TODO: should we raise an error here too?
	return (PKS_VALUE) { 0 };
}

/*
 * Set a nested object using a key (must be a string) assuming the target object is a map.
 */
int memory_atPutObject(PKS_VALUE vTarget, PKS_VALUE vKey, PKS_VALUE vElement) {
	if (vTarget.pkv_managed && vTarget.pkv_type == VT_MAP && vKey.pkv_type == VT_STRING) {
		HASHMAP* pMap = memory_accessMap(vTarget.pkv_data.objectPointer);
		hashmap_put(pMap,
				(void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, vKey.pkv_data.objectPointer),
				MAKE_TYPED_OBJECT_POINTER(vElement.pkv_isPointer, vElement.pkv_type, vElement.pkv_data.val));
		vTarget.pkv_data.objectPointer->od_size = hashmap_size(pMap);
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
	if (vTarget.pkv_managed && vTarget.pkv_type == VT_MAP && vKey.pkv_type == VT_STRING) {
		HASHMAP* pMap = memory_accessMap(vTarget.pkv_data.objectPointer);
		TYPED_OBJECT_POINTER top= hashmap_get(pMap, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, vKey.pkv_data.objectPointer));
		return memory_asValue(top);
	}
	interpreter_raiseError("Can only access map elements with string type keys.");
	return (PKS_VALUE) { 0 };
}

/*
 * If the passed value is of type "handle" (file handle, editor handle etc...), it is assumed,
 * that one foreign memory nested object exists, which can be uwrapped using this method. In
 * other words: return the FILE* or WINFO* objects associated with the given value.
 */
void* memory_handleForValue(PKS_VALUE vValue) {
	TYPED_OBJECT_POINTER p = memory_getNestedObjectPointer(vValue, 0);
	void *pResult = TOP_DATA_POINTER(p);
	T_CONVERT_HANDLE tConverter = types_getConverterFromMemory(vValue.pkv_type);
	return tConverter ? tConverter(pResult) : pResult;
}

/*
 * Creates a "handle" type PKSMacroC object, which wraps a native C pointer to be passed around
 * in macroC code.
 */
PKS_VALUE memory_createHandleObject(EXECUTION_CONTEXT* pContext, PKS_VALUE_TYPE tType, void* p) {
	PKS_VALUE vResult = memory_createObject(pContext, tType, 1, 0);
	T_CONVERT_HANDLE tConverter = types_getConverterToMemory(tType);
	if (tConverter) {
		p = tConverter(p);
	}
	memory_setNestedPointer(vResult, 0, (MAKE_TYPED_OBJECT_POINTER(0, 0, p)));
	return vResult;
}

