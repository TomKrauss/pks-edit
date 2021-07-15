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
#include "alloc.h"
#include "hashmap.h"

#define	DIM(x)		(sizeof(x)/sizeof(x[0]))

typedef struct tagHASH_ENTRY {
	intptr_t he_key;
	intptr_t he_value;
} HASH_ENTRY;

typedef struct tagHASH_MAP {
	int			ht_size;
	int			ht_capacity;
	HASH_CODE	ht_hashCode;
	HASH_COMPARE ht_compare;
	HASH_ENTRY* ht_entries;
} HASH_MAP;

static int _primes[] = {
	13,
	29,
	47,
	89,
	179,
	257
};

/*
 * Calculate the hash function for a string type key. 
 */
static unsigned int hash_string(intptr_t pParam) {
	unsigned char* pszString = (unsigned char*)pParam;
	unsigned int nResult = 0;
	int nDelta;
	while ((nDelta = *pszString++) != 0) {
		nResult = 17 * nResult + nDelta;
	}
	return nResult;
}

/*
 * Calculate a "feasible" size for a hashmap given a desired capacity of elements. 
 */
static int hash_nextPrimeFor(int nCapacity) {
	for (int i = 0; i < DIM(_primes); i++) {
		if (_primes[i] > nCapacity) {
			return _primes[i];
		}
	}
	return nCapacity * 2 + 11;
}

static hash_allocateTable(HASH_MAP* pTable, int nCapacity) {
	int nNumber = hash_nextPrimeFor(nCapacity);
	pTable->ht_entries = calloc(nNumber, sizeof(HASH_ENTRY));
	pTable->ht_capacity = nNumber;
}

/*
 * Create a hash table with a desired capacity and a hash function
 * used to calculate the hashcode and a compare function to compare
 * keys. If NULL is passed for the hashCode, we assume the keys are
 * strings.
 */
HASH_MAP* hash_create(int nCapacity, HASH_CODE hashCode, HASH_COMPARE compareFunction) {
	HASH_MAP* pResult = calloc(1, sizeof * pResult);
	pResult->ht_hashCode = hashCode;
	pResult->ht_compare = compareFunction;
	if (!hashCode) {
		pResult->ht_hashCode = hash_string;
		pResult->ht_compare = (HASH_COMPARE)strcmp;
	}
	hash_allocateTable(pResult, nCapacity);
	return pResult;
}

/*
 * Destroy a hashtable. 
 */
void hash_destroy(HASH_MAP* pTable, HASH_DESTROY_ENTRY destroyCallback) {
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
static int hash_findIndex(HASH_MAP* pTable, intptr_t key) {
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
static void hash_rehash(HASH_MAP* pTable) {
	HASH_ENTRY* pOld = pTable->ht_entries;
	int nOldSize = pTable->ht_capacity;
	hash_allocateTable(pTable, pTable->ht_capacity + 1);
	pTable->ht_size = 0;
	for (int i = 0; i < nOldSize; i++) {
		if (pOld[i].he_key) {
			hash_put(pTable, pOld[i].he_key, pOld[i].he_value);
		}
	}
	free(pOld);
}

/*
 * Returns the size of a hash table.
 */
int hash_size(HASH_MAP* pTable) {
	return pTable->ht_size;
}

/*
 * Put an element into the hash table. Return 0 if an element with the named
 * key existed or 1 if a new entry was added.
 */
int hash_put(HASH_MAP* pTable, intptr_t key, intptr_t value) {
	if (pTable->ht_size >= pTable->ht_capacity * 3 / 4) {
		hash_rehash(pTable);
	}
	int nIndex = hash_findIndex(pTable, key);
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
int hash_containsKey(HASH_MAP* pTable, intptr_t key) {
	int nIndex = hash_findIndex(pTable, key);
	return pTable->ht_entries[nIndex].he_key != 0;
}