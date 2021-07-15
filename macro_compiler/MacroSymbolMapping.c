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
#include "sym.h"
#include "stringutil.h"
#include "errordialogs.h"

extern long 	number(char *s);
extern int 		macro_isParameterStringType(unsigned char typ);
extern int 		macro_getDollarParameter(intptr_t offset, int *typ, intptr_t *value);

static SYMBOL	nullSymbol;

/*---------------------------------*/
/* HASHTABLE					*/
/*---------------------------------*/
typedef struct hentry {
	char 	*key;
	SYMBOL 	sym;
} ENTRY;

typedef enum {	FIND,ENTER,DESTROY} ACTION;

static ENTRY *	_htab;
static unsigned  	_hsize,_overflow;

/*---------------------------------*/
/* hindex()					*/
/* Hash-Funktion (fast) nach Wirth	*/
/*---------------------------------*/
static unsigned int hindex(unsigned char *s)
{
	unsigned char c;
	unsigned short l=0;

	while ((c = *s++) != 0)
		l += c;
	return l;
}

/*--------------------------------------------------------------------------
 * hashed symbol lookup
 * hfind()
 */
static ENTRY *hfind(char *key,ACTION action)
{	register unsigned sonde = 1;
	register  unsigned h;
	register  ENTRY *hp;
	static 	ENTRY _ep;

	if (!_htab) {
		if (!hash_create(HSIZE)) {
			return 0;
		}
	}

	h = hindex(key);
	do {
		while(h >= _hsize)
			h -= _hsize;
		hp = &_htab[h];

		if (!hp->key) {
			if (action == DESTROY)
				return 0;
			if (action == FIND)
				return 0;

			/* action == ENTER */
			hp->key = key;
			return hp;

		} else if (!strcmp(key,hp->key)) {
		    	if (action == ENTER || action == FIND)
				return hp;
			/* destroy: return copy */
			_ep.key = hp->key;
			_ep.sym = hp->sym;
			hp->key = 0;
			return &_ep;
		}
		h += sonde; 
		sonde += 2;
	} while (sonde < _overflow);

	if (action == ENTER)
		error_displayAlertDialog("hash table overflow\n");
	return 0;
}

/*---------------------------------*/
/* hash_create()				*/
/* neue Tabelle erzeugen			*/
/* mit nel == 0, alte zerstoeren	*/
/*---------------------------------*/
int hash_create(unsigned nel)
{
	if (_htab) {
		if (_hsize == nel) {
			return 1;
		}
		free(_htab);
		_htab = 0;
	}
	if (nel > 0) {
		_hsize    = nel;
	     _overflow = nel * 4 / 5;
		nel *= sizeof(_htab[0]);
		if ((_htab  = malloc(nel)) == 0)
		    return 0;

		memset(_htab, 0, nel);
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * sym_find()
 */
SYMBOL sym_find(char *key,char **key_ret)
{
	ENTRY	*ep;

	if ((ep = hfind(key,FIND)) != 0) {
		*key_ret = ep->key;
		return ep->sym;
	}
	return nullSymbol;	
}

/*--------------------------------------------------------------------------
 * sym_insert()
 */
int sym_insert(char *key, int symtype, intptr_t symdata)
{
	ENTRY	*ep;

	if ((ep = hfind(key,ENTER)) != 0) {
		SETSYMBOL(ep->sym,symtype,symdata);
		return 1;
	}
	return 0;	
}

/*--------------------------------------------------------------------------
 * sym_destroy()
 * Currently not used.
 */
static intptr_t sym_destroy(char *key)
{
	ENTRY	*ep;
	intptr_t 	val;

	if ((ep = hfind(key,DESTROY)) != 0) {
		free(ep->key);
		val = (intptr_t) VALUE(ep->sym);
		CLEARSYM(ep->sym);
		return val;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * sym_makeInternalSymbol()
 */
int sym_makeInternalSymbol(char *name, char ed_typ, intptr_t value) {
	ENTRY	*ep;
	int		type;

	type = ed_typ;
	if (ed_typ == S_STRING || ed_typ == S_CONSTSTRING) {
		if ((value = (intptr_t)_strdup((char*)value)) == 0) {
			return 0;
		}
	}

	if ((ep = hfind(name,FIND)) != 0) {
		/* redefinition */
		if (TYPEOF(ep->sym) == S_STRING || 
			TYPEOF(ep->sym) == S_CONSTSTRING) {
			free((void*)VALUE(ep->sym));
		}
		SETSYMBOL(ep->sym,type,value);
		return 1;
	} else {
		if ((name = _strdup(name)) == 0) {
			return 0;
		}
	}

	return sym_insert(name,type,value);
}

/*--------------------------------------------------------------------------
 * GetVariable()
 */
static SYMBOL GetVariable(char *symbolname)
{
	SYMBOL 	sym;
	char	*	tmp;

	sym = sym_find(symbolname,&tmp);
	if (NULLSYM(sym)) {
		error_displayAlertDialog("undefined symbol %s",symbolname);
		return sym;
	}

	if (TYPEOF(sym) < S_NUMBER || TYPEOF(sym) > S_DOLSTRING) {
		error_displayAlertDialog("bad symbol %s (%x)",symbolname,TYPEOF(sym));
		return nullSymbol;
	}

	return sym;
}

/*--------------------------------------------------------------------------
 * sym_integerForSymbol()
 */
long sym_integerForSymbol(char *symbolname) {
	SYMBOL 	sym;
	int		isString;
	intptr_t value;

	sym = GetVariable(symbolname);

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
	default:
		return number((char*)VALUE(sym));
	}
}

/*--------------------------------------------------------------------------
 * sym_stringForSymbol()
 */
intptr_t sym_stringForSymbol(char *symbolname) {
	SYMBOL 	sym;
	int		isString;
	intptr_t		value;
	static char buf[20];

	sym = GetVariable(symbolname);

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
	default:
		return (intptr_t)VALUE(sym);
	}
}

/*--------------------------------------------------------------------------
 * sym_assignSymbol()
 */
long sym_assignSymbol(char *name, COM_LONG1 *v) {
	int 	typ;
	intptr_t value;

	typ = macro_isParameterStringType(v->typ) ? S_STRING : S_NUMBER;
	value = macro_popParameter((unsigned char **)&v);
	return sym_makeInternalSymbol(name,typ,value);
}

