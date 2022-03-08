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

static PKS_VALUE	nullSymbol;

typedef enum {	FIND,ENTER,DESTROY} ACTION;

struct tagIDENTIFIER_CONTEXT {
	struct tagIDENTIFIER_CONTEXT* ic_next;
	HASHMAP* ic_table;
};

static IDENTIFIER_CONTEXT _globalContext;

/*
 * Returns the global identifier context.
 */
IDENTIFIER_CONTEXT* sym_getGlobalContext() {
	return &_globalContext;
}

/*
 * Destroy a symbol registered in a symbol table.
 */
static int sym_destroyEntry(intptr_t tKey, intptr_t tValue) {
	PKS_VALUE* sym = (PKS_VALUE*)tValue;
	SYMBOL_TYPE stType = sym->sym_type;
	char* pszKey = (char*)tKey;
	if (sym->pkv_isPointer && !sym->pkv_managed) {
		if (stType == S_STRING || stType == S_CONSTSTRING) {
			free(sym->sym_data.string);
		}
		else if (stType == S_STRING_ARRAY) {
			arraylist_destroyStringList(sym->sym_data.stringList);
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
	hashmap_destroy(_globalContext.ic_table, sym_destroyEntry);
	_globalContext.ic_table = NULL;
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
	pContext->ic_table = hashmap_create(HSIZE, NULL, NULL);
	return pContext->ic_table != NULL;
}

/*
 * Tries to find the most local identifier context in which a symbol is located.
 */
static IDENTIFIER_CONTEXT* sym_findContext(IDENTIFIER_CONTEXT* pContext, const char* key, HASH_ENTRY* pEntry) {
	if (pContext->ic_next) {
		IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext->ic_next, key, pEntry);
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
void sym_remove(IDENTIFIER_CONTEXT* pContext, char* key) {
	HASH_ENTRY entry;
	IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext, key, &entry);
	if (pFound) {
		hashmap_remove(pFound->ic_table, (intptr_t)key);
		free((void*)entry.he_key);
		sym_destroyEntry(0, entry.he_value);
	}
}


/*--------------------------------------------------------------------------
 * Determines the identifier context of a symbol.
 */
IDENTIFIER_CONTEXT* sym_getContext(IDENTIFIER_CONTEXT* pContext, char* key) {
	HASH_ENTRY entry;
	return sym_findContext(pContext, key, &entry);
}

/*--------------------------------------------------------------------------
 * sym_find()
 */
PKS_VALUE sym_find(IDENTIFIER_CONTEXT* pContext, char *key,char **key_ret) {
	HASH_ENTRY entry;
	IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext, key, &entry);
	if (!pFound) {
		return nullSymbol;
	}
	*key_ret = (char*)entry.he_key;
	PKS_VALUE* sym = (PKS_VALUE*)entry.he_value;
	return *sym;
}

/*--------------------------------------------------------------------------
 * sym_insert()
 */
static int sym_insert(IDENTIFIER_CONTEXT* pContext, char *key, PKS_VALUE vValue) {
	HASH_ENTRY entry;
	IDENTIFIER_CONTEXT* pFound = sym_findContext(pContext, key, &entry);
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
	PKS_VALUE* sym = calloc(1, sizeof * sym);
	*sym = vValue;
	hashmap_put(pFound->ic_table, (intptr_t)key, (intptr_t)sym);
	return 1;	
}


/*--------------------------------------------------------------------------
 * sym_createSymbol()
 */
int sym_createSymbol(IDENTIFIER_CONTEXT* pContext, char *name, SYMBOL_TYPE stType, GENERIC_DATA value) {
	PKS_VALUE v = {.sym_type = stType};
	if (stType == S_STRING || stType == S_CONSTSTRING) {
		if ((v.sym_data.string = _strdup(value.string)) == 0) {
			return 0;
		}
		v.pkv_isPointer = TRUE;
	} else if (stType == S_STRING_ARRAY) {
		if (value.stringList) {
			void* pClone = arraylist_cloneStringList(value.stringList);
			if (!pClone) {
				return 0;
			}
			v.sym_data.stringList = pClone;
		}
		v.pkv_isPointer = TRUE;
	} else {
		v.sym_data.val = value.val;
		v.pkv_isPointer = FALSE;
	}
	return sym_insert(pContext, name, v);
}

/*
 * Defines a variable reference to a value managed in the scope of the PKSMacroC object memory
 * management. It is assumed, that the corresponding data is "managed" outside the scope of the
 * symbol table.
 */
int inline sym_defineVariable(IDENTIFIER_CONTEXT* pContext, char* name, PKS_VALUE vValue) {
	vValue.pkv_managed = FALSE;
	return sym_insert(pContext, name, vValue);
}

/*--------------------------------------------------------------------------
 * sym_getVariable()
 * Returns the value associated with a symbol.
 */
PKS_VALUE sym_getVariable(IDENTIFIER_CONTEXT* pContext, char *symbolname) {
	PKS_VALUE 	sym;
	char* tmp;

	sym = sym_find(pContext, symbolname,&tmp);
	if (NULLSYM(sym)) {
		error_displayAlertDialog("undefined symbol %s",symbolname);
		return sym;
	}

	SYMBOL_TYPE sType = TYPEOF(sym);
	if (sType < S_BOOLEAN || sType > S_STRING_ARRAY) {
		error_displayAlertDialog("bad symbol '%s' (type==%d)",symbolname,sType);
		return nullSymbol;
	}

	return sym;
}


