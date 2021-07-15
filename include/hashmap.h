/*
 * hashmap.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: implementation of a hashtable.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HASHTABLE_H

typedef struct tagHASH_MAP HASH_MAP;
typedef unsigned int (*HASH_CODE)(intptr_t aKey);
typedef int (*HASH_COMPARE)(intptr_t anObj1, intptr_t anObj2);
typedef int (*HASH_DESTROY_ENTRY)(intptr_t anObj1, intptr_t anObj2);

/*
 * Returns the size of a hash table.
 */
extern int hash_size(HASH_MAP* pTable);

/*
 * Create a hash table with a desired capacity and a hash function
 * used to calculate the hashcode and a compare function to compare
 * keys. If NULL is passed for the hashCode, we assume the keys are
 * strings.
 */
extern HASH_MAP* hash_create(int nCapacity, HASH_CODE hashCode, HASH_COMPARE compareFunction);

/*
 * Destroy a hashtable. One may pass a callback to be invoked for
 * every entry in the table.
 */
extern void hash_destroy(HASH_MAP* pTable, HASH_DESTROY_ENTRY destroyEntryCallback);

/*
 * Put an element into the hash table. Return 0 if an element with the named
 * key existed or 1 if a new entry was added.
 */
extern int hash_put(HASH_MAP* pTable, intptr_t key, intptr_t value);

/*
 * Check, whether a given key is present in the hash table.
 */
int hash_containsKey(HASH_MAP* pTable, intptr_t key);

#define HASHTABLE_H

#endif
