/*
 * Symbols.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: pksedit variables and symbols in PKSMacroC language.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include <stdio.h>
#include <string.h>

#include "alloc.h"
#include "arraylist.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "hashmap.h"
#include "symbols.h"
#include "stringutil.h"
#include "errordialogs.h"

extern long 	number(char *s);

static SYMBOL	nullSymbol;

typedef enum {	FIND,ENTER,DESTROY} ACTION;

struct tagIDENTIFIER_CONTEXT {
	struct tagIDENTIFIER_CONTEXT* ic_next;
	HASHMAP* ic_table;
};

static IDENTIFIER_CONTEXT _keywordContext;
static IDENTIFIER_CONTEXT _globalContext;
static IDENTIFIER_CONTEXT _globalCompilerContext;

extern void memory_destroy();
extern void types_destroy();

/*
 * Returns the global identifier context.
 */
IDENTIFIER_CONTEXT* sym_getKeywordContext() {
	return &_keywordContext;
}


/*
 * Returns the global identifier context to be used during macro execution.
 */
IDENTIFIER_CONTEXT* sym_getGlobalContext() {
	_globalContext.ic_next = &_keywordContext;
	return &_globalContext;
}

/*
 * Returns the global compiler context.
 */
IDENTIFIER_CONTEXT* sym_getGlobalCompilerContext() {
	_globalCompilerContext.ic_next = &_keywordContext;
	return &_globalCompilerContext;
}

/*
 * Destroy a symbol registered in a symbol table.
 */
static int sym_destroyEntry(intptr_t tKey, intptr_t tValue) {
	SYMBOL* sym = (SYMBOL*)tValue;
	char* pszKey = (char*)tKey;
	if (sym->s_type != S_RUNTIME_VALUE && sym->s_type != S_TYPE_IDENTIFIER) {
		PKS_VALUE_TYPE vt = sym->s.symbol.s_valueType;
		if (vt == VT_STRING) {
			free(sym->s.symbol.s_data.string);
		}
		else if (vt == VT_OBJECT_ARRAY) {
			arraylist_destroyStringList(sym->s.symbol.s_data.stringList);
		}
	}
	free(sym);
	free(pszKey);
	return 1;
}

/*
 * Destroy the macro compiler internal symbol table.
 */
void sym_destroyTable() {
	hashmap_destroy(_keywordContext.ic_table, sym_destroyEntry);
	_keywordContext.ic_table = NULL;
	hashmap_destroy(_globalContext.ic_table, sym_destroyEntry);
	_globalContext.ic_table = NULL;
	hashmap_destroy(_globalCompilerContext.ic_table, sym_destroyEntry);
	_globalCompilerContext.ic_table = NULL;
	memory_destroy();
	types_destroy();
}

/*
 * Push a new identifier context chained to the given parent context and return
 * the new context.
 */
IDENTIFIER_CONTEXT* sym_pushContext(IDENTIFIER_CONTEXT* pParent) {
	IDENTIFIER_CONTEXT* pNew = calloc(1, sizeof * pNew);
	if (!pNew) {
		return NULL;
	}
	pNew->ic_next = pParent;
	return pNew;
}

/*
 * Pop an identifier identifier context chained to the given parent context and return
 * the next context in the context chain. An attempt to pop a NULL context or the
 * global context will be a no-op.
 */
IDENTIFIER_CONTEXT* sym_popContext(IDENTIFIER_CONTEXT* pContext) {
	if (!pContext || pContext == sym_getGlobalContext()) {
		return pContext;
	}
	hashmap_destroy(pContext->ic_table, sym_destroyEntry);
	IDENTIFIER_CONTEXT* pRet = pContext->ic_next;
	free(pContext);
	return pRet;
}

/*---------------------------------
 * sym_create()	
 * Create a symbol table.
 *---------------------------------*/
static int sym_create(IDENTIFIER_CONTEXT* pContext) {
	int nSize = pContext == &_keywordContext ? 337 : HSIZE;
	pContext->ic_table = hashmap_create(nSize, NULL, NULL);
	return pContext->ic_table != NULL;
}

/*
 * Tries to find the most local identifier context in which a symbol is located.
 */
static IDENTIFIER_CONTEXT* sym_findContext(IDENTIFIER_CONTEXT* pContext, const char* key, HASH_ENTRY* pEntry, BOOL bIncludeKeywordContext) {
	if (pContext->ic_next && (pContext->ic_next != &_keywordContext || bIncludeKeywordContext)) {
		IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext->ic_next, key, pEntry, bIncludeKeywordContext);
		if (pFound) {
			return pFound;
		}
	}
	if (pContext->ic_table && hashmap_getEntry(pContext->ic_table, (intptr_t)key, pEntry)) {
		return pContext;
	}
	return 0;
}

/*
 * Remove one symbol from an identifier context.
 */
void sym_remove(IDENTIFIER_CONTEXT* pContext, const char* key) {
	HASH_ENTRY entry;
	IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext, key, &entry, FALSE);
	if (pFound) {
		hashmap_remove(pFound->ic_table, (intptr_t)key);
		free((void*)entry.he_key);
		sym_destroyEntry(0, entry.he_value);
	}
}


/*--------------------------------------------------------------------------
 * Determines the identifier context of a symbol.
 */
IDENTIFIER_CONTEXT* sym_getContext(IDENTIFIER_CONTEXT* pContext, const char* key) {
	HASH_ENTRY entry;
	return sym_findContext(pContext, key, &entry, TRUE);
}

/*--------------------------------------------------------------------------
 * sym_find()
 */
SYMBOL sym_find(IDENTIFIER_CONTEXT* pContext, const char *key,char **key_ret) {
	HASH_ENTRY entry;
	IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext, key, &entry, TRUE);
	if (!pFound) {
		return nullSymbol;
	}
	*key_ret = (char*)entry.he_key;
	SYMBOL* sym = (SYMBOL*)entry.he_value;
	return *sym;
}

/*--------------------------------------------------------------------------
 * sym_insert()
 */
static int sym_insert(IDENTIFIER_CONTEXT* pContext, const char *key, SYMBOL vValue) {
	HASH_ENTRY entry;
	IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext, key, &entry, FALSE);
	if (pFound) {
		hashmap_remove(pFound->ic_table, (intptr_t)entry.he_key);
		key = (char*)entry.he_key;
		sym_destroyEntry(0, entry.he_value);
	}
	else {
		key = _strdup(key);
		pFound = pContext;
		if (!pFound->ic_table) {
			sym_create(pContext);
		}
	}
	SYMBOL* sym = calloc(1, sizeof * sym);
	*sym = vValue;
	hashmap_put(pFound->ic_table, (intptr_t)key, (intptr_t)sym);
	return 1;	
}


/*--------------------------------------------------------------------------
 * sym_createSymbol()
 */
int sym_createSymbol(IDENTIFIER_CONTEXT* pContext, char *name, SYMBOL_TYPE stType, PKS_VALUE_TYPE stValueType, GENERIC_DATA value) {
	SYMBOL v = { .s_type = stType, .s.symbol.s_valueType = stValueType};
	if (stType != S_TYPE_IDENTIFIER) {
		if (stValueType == VT_STRING) {
			if ((v.s.symbol.s_data.string = _strdup(value.string)) == 0) {
				return 0;
			}
			v.s.symbol.s_pointer = TRUE;
		}
		else if (stType == VT_OBJECT_ARRAY) {
			if (value.stringList) {
				void* pClone = arraylist_cloneStringList(value.stringList);
				if (!pClone) {
					return 0;
				}
				v.s.symbol.s_data.stringList = pClone;
			}
			v.s.symbol.s_pointer = TRUE;
		}
		else {
			v.s.symbol.s_data.val = value.val;
			v.s.symbol.s_pointer = FALSE;
		}
	}
	return sym_insert(pContext, name, v);
}

/*
 * Defines a variable reference to a value managed in the scope of the PKSMacroC object memory
 * management. It is assumed, that the corresponding data is "managed" outside the scope of the
 * symbol table.
 */
int inline sym_defineVariable(IDENTIFIER_CONTEXT* pContext, const char* name, PKS_VALUE vValue) {
	vValue.pkv_managed = vValue.pkv_isPointer;
	return sym_insert(pContext, name, (SYMBOL) { .s_type = S_RUNTIME_VALUE, .s.value = vValue });
}

/*
 * Invoked for every entry in the symbol space and executes a callback for all "managed objects".
 */
static int sym_processManagedCB(intptr_t k, intptr_t v, int (*callback)(void* pObject)) {
	SYMBOL* pVal = (SYMBOL*)v;
	if (pVal->s_type == S_RUNTIME_VALUE &&  pVal->s.value.pkv_managed && pVal->s.value.pkv_isPointer) {
		return callback(pVal->s.value.sym_data.objectPointer);
	}
	return 1;
}

/*
 * Execute a callback for all values in the passed context "managed" by the PKS Object memory in this
 * context only.
 */
void sym_traverseManagedObjects(IDENTIFIER_CONTEXT* pContext, int (*callback)(void* pObject)) {
	if (pContext->ic_table) {
		hashmap_forEachEntry(pContext->ic_table, sym_processManagedCB, callback);
	}
}

static int sym_variableDo(intptr_t k, intptr_t v, ARRAY_LIST* pList) {
	SYMBOL* pV = (SYMBOL*)v;
	if (pV->s_type == S_RUNTIME_VALUE) {
		arraylist_add(pList, (void*)k);
	}
	return 1;
}

/*
 * Returns an array list with all variables defined in this context. The returned array list must be freed by the caller.
 */
ARRAY_LIST* sym_getVariables(IDENTIFIER_CONTEXT* pContext) {
	ARRAY_LIST*  resultList = arraylist_create(19);
	if (pContext->ic_table) {
		hashmap_forEachEntry(pContext->ic_table, sym_variableDo, resultList);
	}
	return resultList;
}

/*
 * Can be used to define whether a variable in the given context is defined. 
 */
BOOL sym_existsVariable(IDENTIFIER_CONTEXT* pContext, char* symbolname) {
	HASH_ENTRY entry;
	IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext, symbolname, &entry, FALSE);
	return pFound != 0;
}

/*--------------------------------------------------------------------------
 * sym_getVariable()
 * Returns the value associated with a symbol.
 */
PKS_VALUE sym_getVariable(IDENTIFIER_CONTEXT* pContext, char *symbolname) {

	HASH_ENTRY entry;
	IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext, symbolname, &entry, FALSE);
	if (!pFound) {
		interpreter_raiseError("undefined symbol %s", symbolname);
	}
	char* tmp = (char*)entry.he_key;
	SYMBOL 	sym = *(SYMBOL*)entry.he_value;
	SYMBOL_TYPE sType = sym.s_type;
	if (sType != S_RUNTIME_VALUE) {
		interpreter_raiseError("bad symbol '%s' (type==%d)",symbolname,sType);
		return nullSymbol.s.value;
	}
	return sym.s.value;
}


