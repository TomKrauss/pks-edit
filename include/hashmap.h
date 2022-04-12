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

#ifndef HASHMAP_H

typedef struct tagHASH_ENTRY {
	intptr_t he_key;
	intptr_t he_value;
} HASH_ENTRY;

typedef struct tagARRAY_LIST ARRAY_LIST;
typedef struct tagHASHMAP HASHMAP;
typedef unsigned int (*HASH_CODE)(intptr_t aKey);
typedef int (*HASH_COMPARE)(intptr_t anObj1, intptr_t anObj2);
typedef int (*HASH_DESTROY_ENTRY)(intptr_t anObj1, intptr_t anObj2);

/*
 * Calculate the hash code value for a string type key.
 */
extern unsigned int hashmap_hashCodeString(intptr_t pParam);

/*
 * Returns the size of a hash table.
 */
extern int hashmap_size(HASHMAP* pTable);

/*
 * Return a hash entry for a particular key.
 * If the entry could be found return 1 otherwise 0.
 */
extern int hashmap_getEntry(HASHMAP* pTable, intptr_t key, HASH_ENTRY* pResult);

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
 * Destroy a hashtable also destroying keys and values.
 */
extern void hashmap_destroyWithKeysAndValues(HASHMAP* pTable);

/*
 * Put an element into the hash table. Return 0 if an element with the named
 * key existed or 1 if a new entry was added.
 */
extern int hashmap_put(HASHMAP* pTable, intptr_t key, intptr_t value);

/*
 * Remove an entry from the hashmap given the key.
 */
extern void hashmap_remove(HASHMAP* pTable, intptr_t key);

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
 * If the callback returns 0, the iteration is stopped and the function returns also
 * 0. Otherwise forEachKey returns 1.
 */
extern int hashmap_forEachKey(HASHMAP* pTable, int (*function)(intptr_t k, void* pParam), void* pParam);

/*
 * Can be used to execute a callback for each key / value pair defined in a hashmap.
 * The callback is invoked with each key, the value and the additional param passed as an argument.
 * If the callback returns 0, the iteration is stopped and the function returns also
 * 0. Otherwise forEachKey returns 1.
 */
extern int hashmap_forEachEntry(HASHMAP* pTable, int (*function)(intptr_t k, intptr_t v, void* pParam), void* pParam);

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

/*
 * Return the values contained in a hashmap converted to an array list.
 */
extern ARRAY_LIST* hashmap_values(HASHMAP* pTable);

/*
 * Special version of destroying a hashmap assuming, the hashmap contains keys, which had been allocated as
 * it is typical the case when a "set" of words is created using this hashmap.
 */
extern void hashmap_destroySet(HASHMAP* pTable);

#define HASHMAP_H

#endif
