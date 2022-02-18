/*
 * symbols.h
 *
 * Handling identifiers and symbols in PKSMacroC
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 *This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 */

# ifndef SYMBOLS_H

typedef enum {
	S_BOOLEAN = 12,
	S_NUMBER = 2,
	S_STRING = 3,
	S_FLOAT = 11,
	S_KEYWORD = 1,
	S_DOLNUMBER = 4,
	S_DOLSTRING = 5,
	S_EDFUNC = 6,
	S_TYPE = 7,
	S_ENUM = 8,
	S_CONSTNUM = 9,
	S_CONSTSTRING = 10,
} SYMBOL_TYPE;

#define	TYPEOF(s)			((s).sym_type)
#define	VALUE(s)			((s).sym_data).val
#define	CLEARSYM(s)			{(s).sym_data.val = 0, (s).sym_type = 0;}
#define	SETTYPE(s,type)		((s).sym_type = type)
#define	NULLSYM(s)			(TYPEOF(s) == 0 && VALUE(s) == 0)

#define	HSIZE		697

typedef struct tagIDENTIFIER_CONTEXT IDENTIFIER_CONTEXT;

extern int 		 sym_insert(IDENTIFIER_CONTEXT* pContext, char *key, SYMBOL_TYPE stType, GENERIC_DATA symdata);
extern PKS_VALUE sym_find(IDENTIFIER_CONTEXT* pContext, char *key, char **key_ret);
extern int 		 sym_makeInternalSymbol(IDENTIFIER_CONTEXT* pContext, char *name, SYMBOL_TYPE stType, GENERIC_DATA value);
/*
 * Returns the global identifier context.
 */
extern IDENTIFIER_CONTEXT* sym_getGlobalContext();

/*
 * Push a new identifier context chained to the given parent context and return
 * the new context.
 */
extern IDENTIFIER_CONTEXT* sym_pushContext(IDENTIFIER_CONTEXT* pParent);

/*
 * Pop an identifier identifier context chained to the given parent context and return
 * the next context in the context chain. An attempt to pop a NULL context or the
 * global context will be a no-op.
 */
extern IDENTIFIER_CONTEXT* sym_popContext(IDENTIFIER_CONTEXT* pContext);

/*--------------------------------------------------------------------------
 * sym_stringForSymbol()
 */
extern intptr_t sym_stringForSymbol(IDENTIFIER_CONTEXT* pContext, char* symbolname);

/*--------------------------------------------------------------------------
 * sym_floatForSymbol()
 */
extern double sym_floatForSymbol(IDENTIFIER_CONTEXT* pContext, char* symbolname);

/*--------------------------------------------------------------------------
 * sym_integerForSymbol()
 */
extern long sym_integerForSymbol(IDENTIFIER_CONTEXT* pContext, char* symbolname);

#define	SYMBOLS_H
# endif

