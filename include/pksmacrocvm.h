/*
 * pksmacrocvm.h
 *
 * PKS-MacroC VM byte codes and execution.
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 16.02.2022
 */

#ifndef PKSMACROCVM_H

#include <windows.h>

#define	BIN_ADD			'+'
#define	BIN_SUB			'-'
#define	BIN_MUL			'*'
#define	BIN_DIV			'/'
#define	BIN_MOD			'%'
#define	BIN_POWER		'P'

#define	BIN_SHIFT_LEFT	'L'
#define	BIN_SHIFT_RIGHT	'R'
#define BIN_AT			'['

#define	BIN_OR			'|'
#define	BIN_AND			'&'
#define	BIN_XOR			'^'

#define	BIN_NOT			'~'
#define	BIN_CAST		'C'

#define BIN_RANGE		','

#define IS_UNARY_OPERATOR(op)		(op == BIN_NOT || op == BIN_CAST)

typedef struct tagOBJECT_DATA OBJECT_DATA;

typedef union uGENERIC_DATA {
	unsigned char	uchar;
	intptr_t		val;
	unsigned char	booleanValue;
	OBJECT_DATA*	objectPointer;			// for objects managed by the macroC VM object memory this points to the object data.
	char*			string;
	void*			stringList;
	int				intValue;
	long long		longValue;
	double			doubleValue;
	struct tagRANGE {
		int r_start : 24;
		int r_end : 24;
		int r_increment : 16;
	} range;
} GENERIC_DATA;

// Must match the corresponding symbol types in symbols.h
typedef enum {
	VT_BOOLEAN = 1,
	VT_NUMBER = 2,
	VT_STRING = 3,
	VT_FLOAT = 4,
	VT_CHAR = 5,
	VT_RANGE = 6,
	VT_OBJECT_ARRAY = 7
} PKS_VALUE_TYPE;

typedef struct tagPKS_VALUE {
	int				pkv_managed : 1;				// the memory of this object is managed by the MacroVM object memory
	int				pkv_isPointer : 1;				// the actual data of this value is contained in the linked object
	PKS_VALUE_TYPE	sym_type : 6;					// one of the "basic types" defined by PKS_VALUE_TYPE
	GENERIC_DATA	sym_data;						// the data of this value.
} PKS_VALUE;

typedef struct tagIDENTIFIER_CONTEXT IDENTIFIER_CONTEXT;

#define MAX_STACK_SIZE		256

typedef struct tagOBJECT_MEMORY OBJECT_MEMORY;

struct tagEXECUTION_CONTEXT {
	struct tagEXECUTION_CONTEXT* ec_parent;		// the parent context
	PKS_VALUE* ec_stackBottom;			// the bottom of the value stack
	PKS_VALUE* ec_stackFrame;			// the stack bottom of the stack frame for the current macro executed
	PKS_VALUE* ec_stackCurrent;			// the pointer to the current stack offset
	PKS_VALUE* ec_stackMax;				// the top of the stack. Must not be overridden
	const char* ec_currentFunction;		// name of the current function/macro being executed.
	IDENTIFIER_CONTEXT* ec_identifierContext;
};

typedef struct tagEXECUTION_CONTEXT EXECUTION_CONTEXT;

// Compressed value pointers - used to store as well simple values as also pointers to objects

typedef long long TYPED_OBJECT_POINTER;

#define POINTER_MASK ~(0xFFULL << 56)
#define TOP_DATA_POINTER(p)				(OBJECT_DATA*)((long long)p & POINTER_MASK)
#define TOP_IS_POINTER(p)				(((long long)p) >> 62)
#define TOP_TYPE(p)						((((long long)p) >> 56) & 0x3F)
#define MAKE_TYPED_OBJECT_POINTER(bIsPointer, sType, pPointer)	(((long long)bIsPointer<<62) | ((long long)sType << 56) | (((uintptr_t)pPointer) & POINTER_MASK))

/*
 * Pop one value of the stack of our stack machine
 */
extern PKS_VALUE interpreter_popStackValue(EXECUTION_CONTEXT* pContext);

/*
 * An error has occurred during execution of a macro. Display a descriptive error using the passed format and parameters and abort the execution.
 */
extern void interpreter_raiseError(const char* pFormat, ...);

/*
 * Mini memory management function of PKS MacroC, which temporarily allocates a string to
 * be released later.
 */
extern PKS_VALUE interpreter_allocateString(EXECUTION_CONTEXT* pContext, const char* pszSource);

/*
 * Push one value onto the stack and raise an error, if that fails.
 */
extern int interpreter_pushValueOntoStack(EXECUTION_CONTEXT* pContext, PKS_VALUE nValue);


/*
 Bytecodes of a macro.
 C_LOOP multiplier CMC_{MENU,CMDSEQ} funcnum {par} C_STR1PAR string ...
 all entities padded to ensure int alignment !!

 */

typedef enum {
	// Flow control
	C_STOP  = 0,    			// eof sequence 
	C_0FUNC = 0x1,  			// Function # (char) 
	C_1FUNC = 0x2,  			// Function # (char) + 1 int Param 
	C_MACRO = 0x3,  			// macro "macroname"
	C_MACRO_REF = 0x4,  		// variable reference to a macro to invoke / function pointer

	C_GOTO = 0x5,  				// (conditionally) goto offset

								// Operations
	C_LOGICAL_OPERATION = 0x6,	// Test: binary logical operation between stack[0] and stack[1]
	C_BINOP = 0x7,  			// binary operation between stack[0] and stack[1]
	C_ASSIGN = 0x8,  			// assign: a = stackval
	C_ASSIGN_SLOT = 0x9,  		// assign: a.x = stackval or a[x] = stackval

	// Push objects onto the stack
	C_PUSH_CHARACTER_LITERAL = 0x10,		// Push character literal. 1 Ascii character follows 
	C_PUSH_SMALL_INT_LITERAL = 0x11,		// Push an integer literal in a compacted way (0-255).
	C_PUSH_STRING_LITERAL = 0x12, 			// Push string literal, 1 string Asciistring\0 follows {pad} 
	C_PUSH_INTEGER_LITERAL = 0x13,			// Push Integer literal, pad, 1 int Parameter follows 
	C_PUSH_LONG_LITERAL  = 0x14, 			// Push long literal, pad, 1 long Parameter follows 
	C_PUSH_FLOAT_LITERAL = 0x15, 			// Push floating point literal
	C_PUSH_BOOLEAN_LITERAL   = 0x16,		// Push boolean literal 1 Ascii character follows 
	C_PUSH_ARRAY_LITERAL = 0x17, 	// Push string array literal, n strings\0 follow  
	C_PUSH_VARIABLE = 0x18,					// variable reference to string
	C_FORM_START = 0x19, 					// formular with parameters ...
	
	// Define parameters and variables
	C_DEFINE_PARAMETER = 0x1A,				// create symbol with type and value 
	C_DEFINE_VARIABLE = 0x1B,				// define a variable with type and value 

	// Stack manipulation
	C_SET_STACKFRAME = 0x20,				// start a new stack frame in an invoked method (after parameter have been retrieved)
	C_POP_STACK = 0x22						// pop one element of the stack. Marks the end of a statement.
} MACROC_INSTRUCTION_OP_CODE;


#define	C_IS1PAR(typ)			 (typ & 0x10)
#define	C_ISCMD(typ)			 (typ >= C_0FUNC && typ <= C_MACRO_REF)
#define C_IS_PUSH_OPCODE(opCode) (opCode >= C_PUSH_CHARACTER_LITERAL && opCode <= C_FORM_START)

#define	C_NONE			0xFF

#define	FORM_SHOW		0x40		// form should be opened 
#define	FORM_INIT		0x1			// form " and be prefilled
#define	FORM_REDRAW		0x2			// force redraw 

/*
 * interpreter_getParameterSize()
 * typ: the bytecode
 * s: pointer to the bytecode buffer past(!) the opcode (instructionPointer+1)
 */
extern int interpreter_getParameterSize(unsigned char typ, const char* s);

/*
 * Generic coercion function to coerce PKS-Edit values.
 */
extern PKS_VALUE interpreter_coerce(EXECUTION_CONTEXT* pContext, PKS_VALUE nValue, PKS_VALUE_TYPE tTargetType);

typedef struct tagCOM_1FUNC {
	unsigned char	typ;			// C_1FUNC - carries one explicit param to pass
	unsigned char	funcnum;		// index into editor function table _functionTable
	int				func_args;		// Number of arguments actually passed.
	long			p;				// optional parameter to pass
} COM_1FUNC;

typedef struct tagCOM_0FUNC {
	unsigned char typ;				// C_0FUNC all params are located on the stack
	unsigned char funcnum;			// index in function table 
	int			  func_nargs;		// Number of arguments actually passed.
} COM_0FUNC;

typedef struct tagCOM_MAC {
	unsigned char typ;				// C_MACRO, C_MACRO_REF
	unsigned char reserved;
	int			  func_args;		// Number of arguments actually passed.
	unsigned char name[1];			// 0-term. string padded to even # 
} COM_MAC;

/*
 * Describes an editor command.
 */
typedef struct tagCOMMAND {
	int		  c_index;				// the command index used internally.
	COM_1FUNC c_functionDef;		// the actual functionto execute including flags.
	char*	  c_name;				// name of the command used in macros
} COMMAND;

typedef struct tagCOM_STRING_ARRAYLITERAL {
	unsigned char 	typ;			// C_PUSH_STRING_ARRAY_LITERAL push a string array
	unsigned char   length;			// length of the array
	unsigned int    totalBytes;		// Total size of this instruction in bytes.
	unsigned char	strings[1];		// 'length' number of 0-terminated strings follow now.
} COM_ARRAYLITERAL;


typedef struct tagCOM_ASSIGN {
	unsigned char 	typ;			// C_ASSIGN assign current stack top to a variable or C_ASSIGN_SLOT with slot being stack top and value being next to top.
	unsigned char	name[1];		// variable name of variable assigned
} COM_ASSIGN;

typedef struct tagCOM_DEFINE_SYMBOL {
	unsigned char 	typ;			// C_DEFINE_PARAMETER or C_DEFINE_VARIABLE
	unsigned char 	symtype;		// variable type
	long			value;			// for array type variables the size of the array for parameters the index of the parameter or index for C_ASSIGN_SLOT
	int		    	size;			// size of total structure
	unsigned char	name[1];		// variable name
} COM_DEFINE_SYMBOL;

#define	BRA_ALWAYS		0
#define	BRA_IF_FALSE			1
#define	BRA_IF_TRUE			2

typedef struct tagCOM_GOTO {
	unsigned char typ;				// C_GOTO, C_GOCOND */
	unsigned char branchType;		// BRA_ALWAYS, BRA_IF_FALSE, BRA_IF_TRUE, */
	int		    offset;
} COM_GOTO;

typedef struct tagCOM_BINOP {
	unsigned char typ;				// C_BINOP 
	unsigned char op;				// see above BIN_ADD etc... 
	PKS_VALUE_TYPE targetType;		// for cast operators this is the target type.
} COM_BINOP;

typedef struct tagCOM_CHAR1 {
	unsigned char typ;				// C_PUSH_CHARACTER_LITERAL C_PUSH_LITERAL or C_PUSH_SMALL_INT_LITERAL
	unsigned char val;
} COM_CHAR1;

typedef struct tagCOM_CHAR1 COM_FURET;

typedef struct c_stop {
	unsigned char typ;
	unsigned char rc;
} COM_STOP;

typedef struct c_int1 {
	unsigned char	typ;			// C_PUSH_INTEGER_LITERAL, C_PUSH_LITERAL int
	unsigned char	c_valueType;
	int				val;
} COM_INT1;

typedef struct tagCOM_FLOAT1 {
	unsigned char typ;				// C_PUSH_FLOAT int
	unsigned char c_valueType;
	double		  val;
} COM_FLOAT1;

typedef struct tagCOM_LONG1 {
	unsigned char	typ;			// C_PUSH_LONG_LITERAL, C_PUSH_FLOAT int
	unsigned char	c_valueType;
	intptr_t  		val;
} COM_LONG1;

typedef struct tagCOM_STRING1 {
	unsigned char typ;				// C_STRING1, C_PUSH_VARIABLE
	unsigned char s[1];				// 0-term. string padded to even #
} COM_STRING1;

typedef struct tagCOM_FORM {
	unsigned char	typ;			// CMD_FORMSTART 
	unsigned char	options;		// FORM_SHOW
	int				nfields;		// # of fields in formular
} COM_FORM;

typedef union c_seq {
	COM_1FUNC* fu1;
	COM_0FUNC* fu0;
	COM_MAC* ma;
	COM_GOTO* go;
	COM_FORM* fo;
	COM_STRING1* st1;
	COM_LONG1* lo1;
	COM_INT1* in1;
	COM_CHAR1* ch1;
	unsigned char* sp;
} COM_SEQ;

/*
 * Describes a bytecode buffer for being manipulated or read. 
 */
typedef struct BYTECODE_BUFFER {
	unsigned char* bb_start;
	unsigned char* bb_current;
	unsigned char* bb_end;
} BYTECODE_BUFFER;

typedef struct tagMACRO_PARAM MACRO_PARAM;

typedef struct tagCOMPILER_CONFIGURATION {
	int (*cb_insertNewMacro)(MACRO_PARAM *pParam);
	void  (*cb_showStatus)(char* s, ...);
	BOOL cb_openErrorList;
	char* cb_source;
} COMPILER_CONFIGURATION;

extern COMPILER_CONFIGURATION* _compilerConfiguration;

/*
 * Creates one object memory and optionally copies the provided native
 * data into the object memory. Currently two special cases for input are supported:
 * the value is a string - input is interpreted as zero terminated byte string char*,
 * the value is an arraylist of strings - input is interpreted as an ARRAY_LIST containing
 * strings. nInitialSize is used for creating arrays - otherwise one may safely pass 0.
 */
extern PKS_VALUE memory_createObject(EXECUTION_CONTEXT* pContext, PKS_VALUE_TYPE sType, int nInitialSize, const void* pInput);

/*
 * Return the logical size of one object - either the number of chars for a string or the number of instance
 * variables or the length of an array. Return 0 for other primitive types.
 */
extern int memory_size(PKS_VALUE v);

/*
 * Set a nested object of a value at slot nIndex.
 */
extern int memory_setNestedObject(PKS_VALUE vTarget, int nIndex, PKS_VALUE vElement);

/*
 * Try to get rid of some allocated memory not referenced by the stack any more.
 * This method must be invoked with care and not in the middle of an operation
 * allocating more objects not yet referenced or the objects just created might
 * get disposed to early.
 */
void memory_garbaggeCollect(EXECUTION_CONTEXT* pContext);

/*
 * Free the allocated object memory and all objects remaining in the memory.
 */
extern void memory_destroy();

/*
 * Get a string pointer to the actual string for a value.
 */
extern const char* memory_accessString(PKS_VALUE v);

/*
 * Access the nested object of a value at slot nIndex.
 */
extern PKS_VALUE memory_getNestedObject(PKS_VALUE, int nIndex);

/*
 * Add one object to an array type object.
 */
extern int memory_addObject(EXECUTION_CONTEXT* pContext, PKS_VALUE *vObject, PKS_VALUE vElement);

#define PKSMACROCVM_H
#endif

