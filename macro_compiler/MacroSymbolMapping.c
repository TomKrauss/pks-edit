/*
 * MacroSymbolMapping.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: pksedit macro and resource compiler - symbols
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
#include "edfuncs.h"
#include "pkscc.h"
#include "hashmap.h"
#include "sym.h"
#include "stringutil.h"
#include "errordialogs.h"

extern long 	number(char *s);
extern int 		macro_isParameterStringType(unsigned char typ);
extern int 		macro_getDollarParameter(intptr_t offset, int *typ, intptr_t *value);

static PKS_VALUE	nullSymbol;

typedef enum {	FIND,ENTER,DESTROY} ACTION;

static HASHMAP * _htab;

/*
 * Destroy a symbol registered in a symbol table.
 */
static int sym_destroyEntry(intptr_t tKey, intptr_t tValue) {
	PKS_VALUE* sym = (PKS_VALUE*)tValue;
	SYMBOL_TYPE stType = sym->sym_type;
	if (stType == S_STRING || stType == S_CONSTSTRING) {
		free(sym->sym_data.string);
	}
	free(sym);
	char* pszKey = (char*)tKey;
	free(pszKey);
	return 1;
}

/*
 * Destroy the macro compiler internal symbol table.
 */
void sym_destroyTable() {
	hashmap_destroy(_htab, sym_destroyEntry);
	_htab = NULL;
}

/*---------------------------------
 * sym_create()	
 * Create a symbol table.
 *---------------------------------*/
int sym_create(unsigned nel) {
	_htab = hashmap_create(nel, NULL, NULL);
	return _htab != NULL;
}

/*--------------------------------------------------------------------------
 * sym_find()
 */
PKS_VALUE sym_find(char *key,char **key_ret) {
	HASH_ENTRY entry;
	if (!hashmap_getEntry(_htab, (intptr_t)key, &entry)) {
		return nullSymbol;
	}

	*key_ret = (char*)entry.he_key;
	PKS_VALUE* sym = (PKS_VALUE*)entry.he_value;
	return *sym;
}

/*--------------------------------------------------------------------------
 * sym_insert()
 */
int sym_insert(char *key, SYMBOL_TYPE stType, GENERIC_DATA symdata) {
	HASH_ENTRY entry;
	if (hashmap_getEntry(_htab, (intptr_t)key, &entry)) {
		hashmap_remove(_htab, (intptr_t)entry.he_key);
		key = (char*)entry.he_key;
		sym_destroyEntry(0, entry.he_value);
	} else {
		key = _strdup(key);
	}
	PKS_VALUE* sym = calloc(1, sizeof * sym);
	sym->sym_data = symdata;
	sym->sym_type = stType;
	hashmap_put(_htab, (intptr_t)key, (intptr_t)sym);
	return 1;	
}


/*--------------------------------------------------------------------------
 * sym_makeInternalSymbol()
 */
int sym_makeInternalSymbol(char *name, SYMBOL_TYPE stType, GENERIC_DATA value) {
	if (stType == S_STRING || stType == S_CONSTSTRING) {
		if ((value.string = _strdup(value.string)) == 0) {
			return 0;
		}
	}
	return sym_insert(name, stType, value);
}

/*--------------------------------------------------------------------------
 * sym_getVariable()
 */
static PKS_VALUE sym_getVariable(char *symbolname)
{
	PKS_VALUE 	sym;
	char	*	tmp;

	sym = sym_find(symbolname,&tmp);
	if (NULLSYM(sym)) {
		error_displayAlertDialog("undefined symbol %s",symbolname);
		return sym;
	}

	SYMBOL_TYPE sType = TYPEOF(sym);
	if (sType != S_FLOAT && (sType < S_NUMBER || sType > S_DOLSTRING)) {
		error_displayAlertDialog("bad symbol '%s' (type==%d)",symbolname,sType);
		return nullSymbol;
	}

	return sym;
}

/*--------------------------------------------------------------------------
 * sym_integerForSymbol()
 */
long sym_integerForSymbol(char *symbolname) {
	PKS_VALUE 	sym;
	int		isString;
	intptr_t value;

	sym = sym_getVariable(symbolname);

	if (NULLSYM(sym)) {
		return 0L;
	}

	switch (TYPEOF(sym)) {
	case S_DOLNUMBER: case S_DOLSTRING:
		if (macro_getDollarParameter((intptr_t) VALUE(sym), &isString, &value) == 0) {
			return 0;
		}
		if (!isString) {
			return (long)value;
		}
		return number((char *)value);
	case S_CONSTNUM: case S_NUMBER:
		return (long) (intptr_t)VALUE(sym);
	case S_FLOAT:
		return (long)(double)(intptr_t)VALUE(sym);
	default:
		return number((char*)VALUE(sym));
	}
}

/*--------------------------------------------------------------------------
 * sym_floatForSymbol()
 */
double sym_floatForSymbol(char* symbolname) {
	PKS_VALUE 	sym;

	sym = sym_getVariable(symbolname);
	if (TYPEOF(sym) == S_FLOAT) {
		return sym.sym_data.doubleValue;
	}
	return (double)sym_integerForSymbol(symbolname);
}

/*--------------------------------------------------------------------------
 * sym_stringForSymbol()
 */
intptr_t sym_stringForSymbol(char *symbolname) {
	PKS_VALUE 	sym;
	int		isString;
	intptr_t		value;
	static char buf[20];

	sym = sym_getVariable(symbolname);

	if (NULLSYM(sym)) {
		return (intptr_t)"";
	}

	switch(TYPEOF(sym)) {
	case S_DOLNUMBER: case S_DOLSTRING:
		if (macro_getDollarParameter((intptr_t) VALUE(sym), &isString, &value) == 0) {
			return 0;
		}
		if (isString) {
			return value;
		}
		/* drop through */
	case S_CONSTNUM: case S_NUMBER:
		if (TYPEOF(sym) == S_NUMBER || TYPEOF(sym) == S_CONSTNUM) {
			value = (intptr_t)VALUE(sym);
		}
		sprintf(buf,"%ld", (long)value);
		return (intptr_t)buf;
	case S_FLOAT:
		if (TYPEOF(sym) == S_FLOAT) {
			value = (intptr_t)VALUE(sym);
		}
		sprintf(buf, "%lf", (double)value);
		return (intptr_t)buf;
	default:
		return (intptr_t)VALUE(sym);
	}
}

/*--------------------------------------------------------------------------
 * sym_assignSymbol()
 */
long sym_assignSymbol(char *name, COM_LONG1 *v) {
	SYMBOL_TYPE 	sSymbolType;
	GENERIC_DATA value;

	sSymbolType = macro_isParameterFloatType(v->typ) ? S_FLOAT : (macro_isParameterStringType(v->typ) ? S_STRING : S_NUMBER);
	value = macro_popParameter((unsigned char**)&v);
	return sym_makeInternalSymbol(name, sSymbolType, value);
}

