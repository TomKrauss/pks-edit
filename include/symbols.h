/*
 * symbols.h
 *
 * Support symbols in PKSMacroC. Symbols allow the "scoped" lookup of a names associated value. 
 * They are used during compilation to lookup keywords, native method names, variable names by scope etc...
 * They are used during runtime to store variables in a context (global / method local).
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
	S_KEYWORD = 9,
	S_EDFUNC = 10,
	S_TYPE_IDENTIFIER = 11,
	S_ENUM = 12,
	S_CONSTANT = 13,
	S_VARIABLE = 14,
	S_LOCAL_VARIABLE = 15,
	S_RUNTIME_VALUE = 16
} SYMBOL_TYPE;

typedef struct tagSYMBOL {
	SYMBOL_TYPE				s_type;					// the symbol-type - for runtime variables this is S_RUNTIME_VALUE
	int						s_index;				// for local variables the offset into the local method heap
	union {
		struct {
			int				s_managed : 1;			// 0 in the context of the compilation - not managed by object memory
			int				s_pointer : 1;			// pointer type symbols to be dynamically allocated and freed have this flag set
			PKS_VALUE_TYPE	s_valueType : 8;		// the value type for variables and constants otherwise 0
			GENERIC_DATA	s_data;					// the data.
		} symbol;
		PKS_VALUE value;
	} s;
} SYMBOL;

#define	TYPEOF(xyz)			((xyz).s_type)
#define VALUE_TYPE(xyz)		((xyz).s.symbol.s_valueType)
#define	VALUE(xyz)			((xyz).s.symbol.s_data).val
#define	CLEARSYM(xyz)		{(xyz).s.data.val = 0, (xyz).s_type = 0;}
#define	SETTYPE(xyz,type)	((xyz).s_type = type)
#define	NULLSYM(xyz)		(TYPEOF(xyz) == 0 && VALUE(xyz) == 0)

#define	HSIZE		19

typedef struct tagIDENTIFIER_CONTEXT IDENTIFIER_CONTEXT;

/*
 * Lookup a symbol - used in the context of PKSMacroC complation
 */
extern SYMBOL sym_find(IDENTIFIER_CONTEXT* pContext, const char *key, char **key_ret);

/*
 * Create a symbol primarily to be used in the context of the macroc parser to define
 * keywords, variables, enum values etc... All objects created requiring memory (e.g. strings etc)
 * are copied internally. The variable heap offset is used in local variables to access the value on the method
 * heap.
 */
extern int sym_createSymbol(IDENTIFIER_CONTEXT* pContext, char *name, SYMBOL_TYPE stType, PKS_VALUE_TYPE stValueType, GENERIC_DATA value, int nVariableHeapOffset);

/*
 * Execute a callback for all values in the passed context "managed" by the PKS Object memory.
 */
extern void sym_traverseManagedObjects(IDENTIFIER_CONTEXT* pContext, int (*callback)(void* pObject));

/*
 * Returns an array list with all variables defined in this context. The returned array list must be freed by the caller.
 */
#ifdef ARRAYLIST_H
extern ARRAY_LIST* sym_getVariables(IDENTIFIER_CONTEXT* pContext);
#endif 

/*--------------------------------------------------------------------------
 * Determines the identifier context of a symbol.
 */
extern IDENTIFIER_CONTEXT* sym_getContext(IDENTIFIER_CONTEXT* pContext, const char* key);

/*
 * Defines a variable reference to a value managed in the scope of the PKSMacroC object memory
 * management. It is assumed, that the corresponding data is "managed" outside the scope of the
 * symbol table. This API is used during runtime of a PKSMacroC macro.
 */
extern int sym_defineVariable(IDENTIFIER_CONTEXT* pContext, const char* name, PKS_VALUE vValue);

/*
 * Remove one symbol from an identifier context.
 */
extern void sym_remove(IDENTIFIER_CONTEXT* pContext, const char* key);

/*
 * Returns the global compiler context.
 */
extern IDENTIFIER_CONTEXT* sym_getGlobalCompilerContext();

/*
 * Returns the global identifier context to be used during macro execution.
 */
extern IDENTIFIER_CONTEXT* sym_getGlobalContext();

/*
 * Returns the identifier context, where keywords are registered.
 */
extern IDENTIFIER_CONTEXT* sym_getKeywordContext();

/*
 * Push a new identifier context chained to the given parent context and return
 * the new context.
 */
extern IDENTIFIER_CONTEXT* sym_pushContext(IDENTIFIER_CONTEXT* pParent, int bRuntimeContext);

/*
 * Pop an identifier identifier context chained to the given parent context and return
 * the next context in the context chain. An attempt to pop a NULL context or the
 * global context will be a no-op.
 */
extern IDENTIFIER_CONTEXT* sym_popContext(IDENTIFIER_CONTEXT* pContext);

/*--------------------------------------------------------------------------
 * sym_getVariable()
 * Returns the value associated with a symbol.
 */
extern PKS_VALUE sym_getVariable(IDENTIFIER_CONTEXT* pContext, char* symbolname);

#define	SYMBOLS_H
# endif

