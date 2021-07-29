/*
 * HashMap.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: implementation of a hashmap for fast lookup of keys.
 *
 * 										created: 08.06.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "alloc.h"
#include "hashmap.h"

#define	DIM(x)		(sizeof(x)/sizeof(x[0]))

typedef struct tagHASH_ENTRY {
	intptr_t he_key;
	intptr_t he_value;
} HASH_ENTRY;

typedef struct tagHASHMAP {
	int			ht_size;
	int			ht_capacity;
	HASH_CODE	ht_hashCode;
	HASH_COMPARE ht_compare;
	HASH_ENTRY* ht_entries;
} HASHMAP;

static int _primes[] = {
	13,
	29,
	47,
	89,
	179,
	257
};

/*
 * Calculate the hash code value for a string type key. 
 */
static unsigned int hashmap_hashCodeString(intptr_t pParam) {
	unsigned char* pszString = (unsigned char*)pParam;
	unsigned int nResult = 0;
	int nDelta;
	while ((nDelta = *pszString++) != 0) {
		nResult = 17 * nResult + nDelta;
	}
	return nResult;
}

/*
 * Calculate the hash code value for a string type key ignoring the case.
 */
static unsigned int hashmap_hashCodeStringIgnoreCase(intptr_t pParam) {
	unsigned char* pszString = (unsigned char*)pParam;
	unsigned int nResult = 0;
	int nDelta;
	while ((nDelta = *pszString++) != 0) {
		nResult = 17 * nResult + tolower(nDelta);
	}
	return nResult;
}

/*
 * Calculate a "feasible" size for a hashmap given a desired capacity of elements. 
 */
static int hashmap_nextPrimeFor(int nCapacity) {
	for (int i = 0; i < DIM(_primes); i++) {
		if (_primes[i] > nCapacity) {
			return _primes[i];
		}
	}
	return nCapacity * 2 + 11;
}

static hashmap_allocateTable(HASHMAP* pTable, int nCapacity) {
	int nNumber = hashmap_nextPrimeFor(nCapacity);
	pTable->ht_entries = calloc(nNumber, sizeof(HASH_ENTRY));
	pTable->ht_capacity = nNumber;
}

/*
 * Create a hash table with a desired capacity and a hash function
 * used to calculate the hashcode and a compare function to compare
 * keys. If NULL is passed for the hashCode, we assume the keys are
 * strings.
 */
HASHMAP* hashmap_create(int nCapacity, HASH_CODE hashCode, HASH_COMPARE compareFunction) {
	HASHMAP* pResult = calloc(1, sizeof * pResult);
	pResult->ht_hashCode = hashCode;
	pResult->ht_compare = compareFunction;
	if (!hashCode) {
		pResult->ht_hashCode = hashmap_hashCodeString;
		pResult->ht_compare = (HASH_COMPARE)strcmp;
	}
	hashmap_allocateTable(pResult, nCapacity);
	return pResult;
}

/*
 * Destroy a hashtable. 
 */
void hashmap_destroy(HASHMAP* pTable, HASH_DESTROY_ENTRY destroyCallback) {
	if (!pTable) {
		return;
	}
	if (destroyCallback) {
		for (int i = 0; i < pTable->ht_capacity; i++) {
			if (pTable->ht_entries[i].he_key) {
				destroyCallback(pTable->ht_entries[i].he_key, pTable->ht_entries[i].he_value);
			}
		}
	}
	free(pTable->ht_entries);
	pTable->ht_size = 0;
	free(pTable);
}

/*
 * Find the next free or the existing index of the passed key into the hash table. 
 */
static int hashmap_findIndex(HASHMAP* pTable, intptr_t key) {
	unsigned int nIndex = pTable->ht_hashCode(key) % pTable->ht_capacity;
	intptr_t existingKey;
	int seed = 1;

	while ((existingKey = pTable->ht_entries[nIndex].he_key) != 0) {
		if (pTable->ht_compare(key, existingKey) == 0) {
			return nIndex;
		}
		// quadratic probing
		nIndex += (seed*seed);
		nIndex %= pTable->ht_capacity;
		seed++;
	}
	return nIndex;
}

/*
 * Expand the capacity of the Hashtable. 
 */
static void hashmap_rehash(HASHMAP* pTable, int newCapacity) {
	HASH_ENTRY* pOld = pTable->ht_entries;
	int nOldSize = pTable->ht_capacity;
	hashmap_allocateTable(pTable, newCapacity);
	pTable->ht_size = 0;
	for (int i = 0; i < nOldSize; i++) {
		if (pOld[i].he_key) {
			hashmap_put(pTable, pOld[i].he_key, pOld[i].he_value);
		}
	}
	free(pOld);
}

/*
 * Returns the size of a hash table.
 */
int hashmap_size(HASHMAP* pTable) {
	return pTable->ht_size;
}

/*
 * Put an element into the hash table. Return 0 if an element with the named
 * key existed or 1 if a new entry was added.
 */
int hashmap_put(HASHMAP* pTable, intptr_t key, intptr_t value) {
	if (pTable->ht_size >= pTable->ht_capacity * 3 / 4) {
		hashmap_rehash(pTable, pTable->ht_capacity + 1);
	}
	int nIndex = hashmap_findIndex(pTable, key);
	if (pTable->ht_entries[nIndex].he_key != 0) {
		pTable->ht_entries[nIndex].he_value = value;
		return 0;
	}
	pTable->ht_size++;
	pTable->ht_entries[nIndex].he_key = key;
	pTable->ht_entries[nIndex].he_value = value;
	return 1;
}

/*
 * Check, whether a given key is present in the hash table. 
 */
int hashmap_containsKey(HASHMAP* pTable, intptr_t key) {
	int nIndex = hashmap_findIndex(pTable, key);
	return pTable->ht_entries[nIndex].he_key != 0;
}

/*
 * Return the value under which a particular key was registered or NULL
 * if not corresponding entry exists.
 */
intptr_t hashmap_get(HASHMAP* pTable, intptr_t key) {
	int nIndex = hashmap_findIndex(pTable, key);
	return pTable->ht_entries[nIndex].he_value;
}

/*
 * Can be used to execute a callback for each key defined in a hashmap.
 * The callback is invoked with each key and the additional param passed as an argument.
 */
void hashmap_forEachKey(HASHMAP* pTable, void (*function)(intptr_t k, void* pParam), void* pParam) {
	for (int i = 0; i < pTable->ht_capacity; i++) {
		intptr_t k = pTable->ht_entries[i].he_key;
		if (k) {
			function(k, pParam);
		}
	}
}

/*
 * Can be used to execute a callback for each key defined in a hashmap matching a filter function.
 * The callback is invoked with each key and the corresponding value from the hashmap.
 */
void hashmap_forKeysMatching(HASHMAP* pTable, void (*function)(intptr_t k, intptr_t v), int (*filter)(intptr_t k)) {
	for (int i = 0; i < pTable->ht_capacity; i++) {
		intptr_t k = pTable->ht_entries[i].he_key;
		if (k && filter(k)) {
			function(k, pTable->ht_entries[i].he_value);
		}
	}
}

/*
 * Assigns a new hashing and compare function to a hashmap rehashing the map if required. 
 */
void hashmap_rehashWith(HASHMAP* pTable, HASH_CODE hashCodeFunction, HASH_COMPARE hashCompareFunction) {
	if (pTable->ht_compare != hashCompareFunction || pTable->ht_hashCode != hashCodeFunction) {
		pTable->ht_compare = hashCompareFunction;
		pTable->ht_hashCode = hashCodeFunction;
		hashmap_rehash(pTable, pTable->ht_capacity);
	}
}

/*
 * Turns the hashmap assuming to contain string keys to ignore the case spelling of the keys used.
 */
void hashmap_makeCaseIgnore(HASHMAP* pTable) {
	hashmap_rehashWith(pTable, hashmap_hashCodeStringIgnoreCase, (HASH_COMPARE)_stricmp);
}

