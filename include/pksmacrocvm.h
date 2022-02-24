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
#define	BIN_AND			'&'
#define	BIN_OR			'|'
#define	BIN_MUL			'*'
#define	BIN_DIV			'/'
#define	BIN_MOD			'%'
#define	BIN_OR			'|'
#define	BIN_AND			'&'
#define	BIN_BRACKETS	'('
#define	BIN_SUB			'-'
#define	BIN_NOT			'~'
#define	BIN_XOR			'^'
#define	BIN_CAST		'C'

#define IS_UNARY_OPERATOR(op)		(op == BIN_NOT || op == BIN_CAST)

typedef union uGENERIC_DATA {
	unsigned char uchar;
	intptr_t val;
	unsigned char booleanValue;
	char* string;
	int		intValue;
	long long longValue;
	double   doubleValue;
} GENERIC_DATA;

// Must match the corresponding symbol types in symbols.h
typedef enum {
	VT_BOOLEAN = 1,
	VT_NUMBER = 2,
	VT_STRING = 3,
	VT_FLOAT = 4,
	VT_CHAR = 5
} PKS_VALUE_TYPE;

typedef struct tagPKS_VALUE {
	PKS_VALUE_TYPE	sym_type;
	GENERIC_DATA	sym_data;
} PKS_VALUE;

extern int interpreter_getDollarParameter(intptr_t offset, PKS_VALUE* pValue);

typedef struct tagIDENTIFIER_CONTEXT IDENTIFIER_CONTEXT;

#define MAX_STACK_SIZE		256

struct tagEXECUTION_CONTEXT {
	PKS_VALUE* ec_stackBottom;			// the bottom of the value stack
	PKS_VALUE* ec_stackFrame;			// the stack bottom of the stack frame for the current macro executed
	PKS_VALUE* ec_stackCurrent;			// the pointer to the current stack offset
	PKS_VALUE* ec_parameterStack;		// the pointer to the current parameter stack offset.
	PKS_VALUE* ec_stackMax;				// the top of the stack. Must not be overridden
	void* ec_allocations;				// list of allocated objects to be released, when execution halts.
	IDENTIFIER_CONTEXT* ec_identifierContext;
};

typedef struct tagEXECUTION_CONTEXT EXECUTION_CONTEXT;

/*
 * Pop one value of the stack of our stack machine
 */
extern PKS_VALUE interpreter_popStackValue(EXECUTION_CONTEXT* pContext);

/*
 * Mini memory management function of PKS MacroC, which temporarily allocates a string to
 * be released later.
 */
extern char* interpreter_allocateString(EXECUTION_CONTEXT* pContext, const char* pszSource);

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
	C_GOTO  = 0x4,  			// (conditionally) goto offset

	// Operations
	C_LOGICAL_OPERATION = 0x6,	// Test: binary logical operation between stack[0] and stack[1]
	C_BINOP = 0x7,  			// binary operation between stack[0] and stack[1]
	C_ASSIGN= 0x8,  			// assign: a = stackval

	// Push objects onto the stack
	C_PUSH_CHARACTER_LITERAL = 0x10, // Push character literal. 1 Ascii character follows 
	C_PUSH_STRING_LITERAL = 0x11, 	// Push string literal, 1 string Asciistring\0 follows {pad} 
	C_PUSH_INTEGER_LITERAL   = 0x12, // Push Integer literal, pad, 1 int Parameter follows 
	C_PUSH_LONG_LITERAL  = 0x13, 	// Push long literal, pad, 1 long Parameter follows 
	C_PUSH_FLOAT_LITERAL = 0x14, 	// Push floating point literal
	C_PUSH_BOOLEAN_LITERAL   = 0x15, // Push boolean literal 1 Ascii character follows 
	C_PUSH_VARIABLE = 0x16, 	// variable reference to string
	C_FORM_START = 0x17, 			// formular with parameters ...
	
	// Define parameters and variables
	C_DEFINE_PARAMETER = 0x18,		// create symbol with type and value 
	C_DEFINE_VARIABLE = 0x19,		// define a variable with type and value 

	// Stack manipulation
	C_SET_STACKFRAME = 0x20,		// start a new stack frame in an invoked method (after parameter have been retrieved)
	C_SET_PARAMETER_STACK = 0x21,	// save the current stack pointer as the bottom of the parameter stack.
	C_POP_STACK = 0x22				// pop one element of the stack. Marks the end of a statement.
} MACROC_INSTRUCTION_OP_CODE;


#define	C_IS1PAR(typ)			 (typ & 0x10)
#define	C_ISCMD(typ)			 (typ >= C_0FUNC && typ <= C_MACRO)
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

typedef struct c_1func {
	unsigned char	typ;			// C_1FUNC - carries one explicit param to pass
	unsigned char	funcnum;		// index into editor function table _functionTable
	long			p;				// optional parameter to pass
} COM_1FUNC;

typedef struct c_0func {
	unsigned char typ;				// C_0FUNC all params are located on the stack
	unsigned char funcnum;			// index in function table 
	int			  func_nargs;		// Number of arguments passed
} COM_0FUNC;


/*
 * Describes an editor command.
 */
typedef struct tagCOMMAND {
	int		  c_index;				// the command index used internally.
	COM_1FUNC c_functionDef;		// the actual functionto execute including flags.
	char* c_name;					// name of the command used in macros
} COMMAND;

typedef struct c_ident {
	unsigned char typ;				// C_MACRO 
	unsigned char name[1];			// 0-term. string padded to even # 
} COM_MAC;

typedef struct c_ident COM_VAR;

typedef struct c_assign {
	unsigned char 	typ;			// C_ASSIGN assign current stack top to a variable
	unsigned char	name[1];		// variable name of variable assigned
} COM_ASSIGN;

typedef struct c_createsym {
	unsigned char 	typ;			// C_DEFINE_PARAMETER or C_DEFINE_VARIABLE
	unsigned char 	symtype;		// variable type
	long long		value;			// value
	int		    	size;			// size of total structure
	unsigned char	name[1];		// variable name
} COM_CREATESYM;

#define	BRA_ALWAYS		0
#define	BRA_NE			1
#define	BRA_EQ			2

typedef struct c_goto {
	unsigned char typ;				// C_GOTO, C_GOCOND */
	unsigned char bratyp;			// BRA_ALWAYS, BRA_NE, BRA_EQ, */
	int		    offset;
} COM_GOTO;

typedef struct c_binop {
	unsigned char typ;				// C_BINOP 
	unsigned char op;				// see above BIN_ADD etc... 
	PKS_VALUE_TYPE targetType;		// for cast operators this is the target type.
} COM_BINOP;

typedef struct c_char1 {
	unsigned char typ;				// C_PUSH_CHARACTER_LITERAL C_PUSH_LITERAL char
	unsigned char val;
} COM_CHAR1;

typedef struct c_char1 COM_FURET;

typedef struct c_stop {
	unsigned char typ;
	unsigned char rc;
} COM_STOP;

typedef struct c_int1 {
	unsigned char	typ;			// C_PUSH_INTEGER_LITERAL, C_PUSH_LITERAL int
	unsigned char	c_valueType;
	int				val;
} COM_INT1;

typedef struct c_float1 {
	unsigned char typ;				// C_PUSH_INTEGER_LITERAL, C_PUSH_FLOAT int
	unsigned char	c_valueType;
	double		  val;
} COM_FLOAT1;

typedef struct c_long1 {
	unsigned char typ;				// C_PUSH_LONG_LITERAL, C_PUSH_FLOAT int
	unsigned char	c_valueType;
	intptr_t  	  val;
} COM_LONG1;

typedef struct c_string1 {
	unsigned char typ;				// C_STRING1, C_PUSH_VARIABLE
	unsigned char s[1];				// 0-term. string padded to even #
} COM_STRING1;

typedef struct c_form {
	unsigned char typ;				// CMD_FORMSTART 
	unsigned char options;			// FORM_SHOW
	int		    nfields;			// # of fields in formular
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

typedef struct tagCOMPILER_CONFIGURATION {
	int (*cb_insertNewMacro)(char* name, char* comment, BYTECODE_BUFFER* pBuffer);
	void  (*cb_showStatus)(char* s, ...);
	BOOL cb_openErrorList;
	char* cb_source;
} COMPILER_CONFIGURATION;

extern COMPILER_CONFIGURATION* _compilerConfiguration;

/*------------------------------------------------------------
 * macro_insertNewMacro()
 * Insert a macro with a given name, comment and byte codes. If the named
 * macro already exists, it is deleted.
 */
extern int macro_insertNewMacro(char* name, char* comment, BYTECODE_BUFFER* pBuffer);


#define PKSMACROCVM_H
#endif

