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

typedef struct tagHASHMAP HASHMAP;
typedef unsigned int (*HASH_CODE)(intptr_t aKey);
typedef int (*HASH_COMPARE)(intptr_t anObj1, intptr_t anObj2);
typedef int (*HASH_DESTROY_ENTRY)(intptr_t anObj1, intptr_t anObj2);

/*
 * Returns the size of a hash table.
 */
extern int hashmap_size(HASHMAP* pTable);

/*
 * Create a hash table with a desired capacity and a hash function
 * used to calculate the hashcode and a compare function to compare
 * keys. If NULL is passed for the hashCode, we assume the keys are
 * strings.
 */
extern HASHMAP* hashmap_create(int nCapacity, HASH_CODE hashCode, HASH_COMPARE compareFunction);

/*
 * Destroy a hashtable. One may pass a callback to be invoked for
 * every entry in the table.
 */
extern void hashmap_destroy(HASHMAP* pTable, HASH_DESTROY_ENTRY destroyEntryCallback);

/*
 * Put an element into the hash table. Return 0 if an element with the named
 * key existed or 1 if a new entry was added.
 */
extern int hashmap_put(HASHMAP* pTable, intptr_t key, intptr_t value);

/*
 * Return the value under which a particular key was registered or NULL
 * if not corresponding entry exists.
 */
extern intptr_t hashmap_get(HASHMAP* pTable, intptr_t key);

/*
 * Check, whether a given key is present in the hash table.
 */
extern int hashmap_containsKey(HASHMAP* pTable, intptr_t key);

/*
 * Can be used to execute a callback for each key defined in a hashmap.
 * The callback is invoked with each key and the additional param passed as an argument.
 */
extern void hashmap_forEachKey(HASHMAP* pTable, void (*function)(intptr_t k, void* pParam), void* pParam);

/*
 * Assigns a new hashing and compare function to a hashmap rehashing the map if required.
 */
extern void hashmap_rehashWith(HASHMAP* pTable, HASH_CODE hashCodeFunction, HASH_COMPARE hashCompareFunction);

/*
 * Turns the hashmap assuming to contain string keys to ignore the case spelling of the keys used.
 */
extern void hashmap_makeCaseIgnore(HASHMAP* pTable);

/*
 * Can be used to execute a callback for each key defined in a hashmap matching a filter function.
 * The callback is invoked with each key and the corresponding value from the hashmap.
 */
extern void hashmap_forKeysMatching(HASHMAP* pTable, void (*function)(intptr_t k, intptr_t v), int (*filter)(intptr_t k));

#define HASHTABLE_H

#endif
