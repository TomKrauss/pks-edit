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

typedef union uGENERIC_DATA {
	unsigned char uchar;
	intptr_t val;
	unsigned char booleanValue;
	char* string;
	int		intValue;
	long long longValue;
	double   doubleValue;
} GENERIC_DATA;

typedef enum {
	VT_BOOLEAN = 12,
	VT_INTEGER = 2,
	VT_STRING = 3,
	VT_FLOAT = 11,
	VT_CHAR = 13,
	VT_LONG
} PKS_VALUE_TYPE;

typedef struct tagPKS_VALUE {
	PKS_VALUE_TYPE	sym_type;
	GENERIC_DATA	sym_data;
} PKS_VALUE;

typedef struct tagIDENTIFIER_CONTEXT IDENTIFIER_CONTEXT;

struct tagEXECUTION_CONTEXT {
	PKS_VALUE* ec_stackBottom;
	PKS_VALUE* ec_stackCurrent;
	PKS_VALUE* ec_stackMax;
	IDENTIFIER_CONTEXT* ec_identifierContext;
};

typedef struct tagEXECUTION_CONTEXT EXECUTION_CONTEXT;

/*
 Bytecodes of a macro.
 C_LOOP multiplier CMC_{MENU,CMDSEQ} funcnum {par} C_STR1PAR string ...
 all entities padded to ensure int alignment !!

 */

typedef enum {
	C_STOP  = 0,    			// eof sequence 
	C_0FUNC = 0x1,  			// Function # (char) 
	C_1FUNC = 0x2,  			// Function # (char) + 1 int Param 
	C_MACRO = 0x3,  			// macro "macroname"
	C_GOTO  = 0x4,  			// (conditionally) goto offset
	C_TEST  = 0x6,  			// Test: testop p1 p2
	C_BINOP = 0x7,  			// binary operation: binop a b
	C_ASSIGN= 0x8,  			// assign: a = stackval
	C_DEFINE_PARAMETER  = 0x9,  // create symbol with type and value 
	C_CHARACTER_LITERAL = 0x10, // 1 Ascii character follows 
	C_STRING_LITERAL= 0x11, 	// 1 string Asciistring\0 follows {pad} 
	C_INTEGER_LITERAL   = 0x12, // pad, 1 int Parameter follows 
	C_LONG_LITERAL  = 0x13, 	// pad, 1 long Parameter follows 
	C_FLOAT_LITERAL = 0x14, 	// floating point literal
	C_BOOLEAN_LITERAL   = 0x15, // 1 Ascii character follows 
	C_STRINGVAR = 0x16, 		// variable reference to string
	C_LONGVAR   = 0x17, 		// variable reference to long value
	C_FORMSTART = 0x18, 		// formular with parameters ...
	C_DATA  = 0x19, 			// any data ... 
	C_FURET = 0x20, 			// next function return is saved 
	C_FLOATVAR  = 0x21, 		// Floating point parameter
	C_BOOLEANVAR= 0x22, 		// Boolean parameter
	C_DEFINE_VARIABLE   = 0x23  // define a variable with type and value 
} MACROC_INSTRUCTION_OP_CODE;


#define	C_IS1PAR(typ)	(typ & 0x10)
#define	C_ISCMD(typ)	(typ >= C_0FUNC && typ <= C_MACRO)

#define	C_NONE			0xFF

#define	FORM_SHOW		0x40	// form should be opened 
#define	FORM_INIT		0x1		// form " and be prefilled
#define	FORM_REDRAW		0x2		// force redraw 

/*
 * macro_getParameterSize()
 * typ: the bytecode
 * s: pointer to the bytecode buffer past(!) the opcode (instructionPointer+1)
 */
int  macro_getParameterSize(unsigned char typ, const char* s);

/*--------------------------------------------------------------------------
 * macro_popParameter()
 * pop data from execution stack
 */
GENERIC_DATA macro_popParameter(EXECUTION_CONTEXT* pContext, unsigned char** sp);

typedef struct c_1func {
	unsigned char  typ;		/* C_1FUNC or C_0FUNC - defines the number of parameters to pass */
	unsigned char  funcnum;	/* index into editor function table _functionTable */
	long			p;		/* optional parameter to pass */
} COM_1FUNC;

/*
 * Describes an editor command.
 */
typedef struct tagCOMMAND {
	int		  c_index;					// the command index used internally.
	COM_1FUNC c_functionDef;			// the actual functionto execute including flags.
	char* c_name;						// name of the command used in macros
} COMMAND;

typedef struct c_0func {
	unsigned char typ;		/* C_0FUNC */
	unsigned char funcnum;	/* index in function table */
} COM_0FUNC;

typedef struct c_ident {
	unsigned char typ;		/* C_MACRO */
	unsigned char name[1];	/* 0-term. string padded to even # */
} COM_MAC;

typedef struct c_ident COM_VAR;

typedef struct c_assign {
	unsigned char 	typ;		/* C_ASSIGN */
	unsigned char 	res;
	int		    	opoffset;	/* source operand offset */
	int		    	size;	/* size of total assignment */
	unsigned char	name[1];	/* variable name */
						/* operand (src) follows */
} COM_ASSIGN;

typedef struct c_createsym {
	unsigned char 	typ;		// C_DEFINE_PARAMETER or C_DEFINE_VARIABLE
	unsigned char 	symtype;	// variable type
	long long		value;		// value
	int		    	size;		// size of total structure
	unsigned char	name[1];	// variable name
} COM_CREATESYM;

#define	BRA_ALWAYS		0
#define	BRA_NE			1
#define	BRA_EQ			2

typedef struct c_goto {
	unsigned char typ;		/* C_GOTO, C_GOCOND */
	unsigned char bratyp;	/* BRA_ALWAYS, BRA_NE, BRA_EQ, */
	int		    offset;
} COM_GOTO;

typedef struct c_test {
	unsigned char typ;		/* C_TEST */
	unsigned char testop;	/* see test.h */
	int		    p2offset;
	int		    size;
} COM_TEST;

typedef struct c_binop {
	unsigned char typ;		/* C_BINOP */
	unsigned char op;		/* see above BIN_ADD etc... */
	int           op1offset;	/* points to COM_1STRING,.... */
	int           op2offset;	/* points to COM_1STRING,.... */
	int		    size;		/* total size of binop */
	unsigned char result[1]; /* result variable name */
} COM_BINOP;

typedef struct c_char1 {
	unsigned char typ;		/* C_CHARACTER_LITERAL */
	unsigned char val;
} COM_CHAR1;

typedef struct c_char1 COM_FURET;

typedef struct c_stop {
	unsigned char typ;
	unsigned char rc;
} COM_STOP;

typedef struct c_int1 {
	unsigned char typ;		/* CMD_INT1PAR */
	unsigned char res;
	int		    val;
} COM_INT1;

typedef struct c_float1 {
	unsigned char typ;		/* CMD_FLOAT1PAR */
	unsigned char res;
	double		  val;
} COM_FLOAT1;

typedef struct c_long1 {
	unsigned char typ;		/* CMD_LONG1PAR */
	unsigned char res;
	intptr_t  	  val;
} COM_LONG1;

typedef struct c_string1 {
	unsigned char typ;		/* C_STRING1 */
	unsigned char s[1];		/* 0-term. string padded to even # */
} COM_STRING1;

typedef struct c_anydata {
	unsigned char typ;		/* C_DATA */
	unsigned char subtype;
	unsigned long size;		/* sizeof data */
	char		    data[1];	/* ... */
} COM_DATA;

typedef struct c_form {
	unsigned char typ;		/* CMD_FORMSTART */
	unsigned char options;	/* FORM_SHOW,... */
	int		    nfields;	/* # of fields in formular */
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

#define PKSMACROCVM_H
#endif

