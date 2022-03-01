%{
/*
 * PARSER.Y
 *
 * Grammar
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 18.05.1991
 *									
 */
%}

%token T_TSTRING T_TINT T_TCHAR T_TLONG T_TFLOAT T_RANGE
%token T_IDENT T_NUM T_FLOATING_POINT_NUMBER T_STRING T_CHARACTER T_CONST
%token T_TRUE T_FALSE T_TBOOLEAN
%token T_FUNC T_MACRO T_DOTDOT T_PLUSPLUS T_MINUSMINUS
%token T_IF T_ELIF T_ELSE T_WHILE T_GOTO T_BRAEQ
%token T_SH_ASSIGN_PLUS T_SH_ASSIGN_MINUS T_SH_ASSIGN_MULT T_SH_ASSIGN_DIV T_SH_ASSIGN_MOD
%token T_LOCAL T_SWITCH T_BREAK T_CONTINUE T_RETURN 
%token T_NUMVAR T_RANGEVAR T_FLOATVAR T_STRINGVAR T_BOOLEANVAR T_CHARACTERVAR
%token T_NMATCH
%token T_ASSIGN T_NE T_EQ T_SHIFT_LEFT T_SHIFT_RIGHT 
%token T_OR T_AND T_LE T_GE T_POWER_TO
%token T_VOID
%token T_CMDSEQPREFIX

%left    T_OR T_AND
%left	'&' '|' '^'
%nonassoc T_NE T_EQ 
%left    '~' T_NMATCH
%left    '<' '>' T_LT T_GT  
%left	T_SHIFT_LEFT T_SHIFT_RIGHT 
%left	'+' '-' 
%left	'*' '/' '%' T_POWER_TO
%left	'[' T_DOTDOT

/* %right	T_NOTGLITCH */

%{

#include <windows.h>
#include <stdio.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "symbols.h"

# define 	YYSTYPE _YYSTYPE
# define 	STRINGTYPE(type)	((type == C_PUSH_STRING_LITERAL||type == C_PUSH_VARIABLE) ? \
							CT_STRING : CT_NUM)
# define 	ISCONSTINT(t)		(t == C_PUSH_LONG_LITERAL|| t == C_PUSH_INTEGER_LITERAL || \
								 t == C_PUSH_SMALL_INT_LITERAL)
# define	ISVARINT(t)		(t == C_LONGVAR)

# define 	MAXEXPR			12

# define	REC_SPACE			24000

static BYTECODE_BUFFER  _bytecodeBuffer;
static void		*_currentNativeMethodCalled;
static char*	_calledFunctionOrMacro;
static int		_nparam,
				_breaklevel,
				_currentFunctionCallParamIndex,
				_iflevel,
				_stringflg;

static 		void endmacro(void);
static 		void startmacro(void);
static 		void freeitem(char **p);
static 		char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp);
static		int bytecode_generateAutoLabelNamePrefix(char *prefix,int level);
static		void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
static void 	bytecode_emitPushParameterInstruction(long long lValue);

static int		_generateShorthandAssignment;
static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

int			vname_count;
extern 		int	yyerrflg;
extern 		int	_bDefiningConst;
extern char*    _yyCurrentComment;
extern void		yyerror(char* s, ...);
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nIncrement);
extern unsigned char* bytecode_emitShorthandAssignment(BYTECODE_BUFFER* pBuffer, int nBinaryOp, char* pszName);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern unsigned char* bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber);

#define		YY_EMIT(opCode, param)		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, opCode, param);

extern int yylex(void );
void 		bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int typ, intptr_t val);

char*		bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int typ);
int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
char*		bytecode_generateAutoLabelName(char *prefix, int num);
int 		function_returnsString(void *ep);

IDENTIFIER_CONTEXT* _currentIdentifierContext;

static void makeInternalSymbol(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal) {
	bytecode_defineVariable(&_bytecodeBuffer, pszName, C_DEFINE_VARIABLE, sType, tVal);
	sym_makeInternalSymbol(_currentIdentifierContext, (char*) pszName, sType, (GENERIC_DATA) {.val = tVal});
}

static SYMBOL_TYPE constantTypeFor(SYMBOL_TYPE aValueType) {
	switch(aValueType) {
	case S_FLOAT: return S_CONSTFLOAT;
	case S_STRING: return S_CONSTSTRING;
	case S_CHARACTER: return S_CONSTCHARACTER;
	}
	return S_CONSTNUM;
}

%}

%%

resources		: 
			| { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			} resource resources

resource	: macro_definition		
			| constants


/*-- CONSTANTS --------------------------------------------------*/

constants		: T_CONST constdefs ';'

constdefs		: constdef
			| constdefs ',' constdef

constdef:	{ _bDefiningConst = 1; } variable_identifier T_ASSIGN constant_literal { _bDefiningConst = 0; } 
			{
				BOOL bString = ($4.v.type == S_STRING);
				sym_makeInternalSymbol(_currentIdentifierContext, $2.ident.s,
					constantTypeFor($4.v.type), $4.v.data);
				free($2.ident.s);
				if (bString) {
					free($4.v.data.string);
				}
			}

constant_literal:
			simple_literal { 
				$$.v = $1.v; 
			}
			| T_STRING	{
				$$.v.data.string = $1.ident.s;
				$$.v.type = S_STRING;
			}

variable_identifier:	T_IDENT 	{	$$.ident = $1.ident; }
			| variable_reference	{	$$.ident = $1.ident; }
			| T_NUM
			{
				$$.ident = $1.ident;
				$$.ident.s = "#dummy";
				$$.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
			| T_STRING 
			{
				$$.ident = $1.ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}

variable_reference:	
			T_STRINGVAR 		{	$$.ident = $1.ident; }
			| T_NUMVAR 			{	$$.ident = $1.ident; }
			| T_RANGEVAR 		{	$$.ident = $1.ident; }
			| T_CHARACTERVAR	{	$$.ident = $1.ident; }
			| T_BOOLEANVAR		{	$$.ident = $1.ident; }
			| T_FLOATVAR 		{	$$.ident = $1.ident; }

/*-- MACROS -----------------------------------------------------*/

macro_declaration: macro_type T_IDENT {
				$$.ident = $2.ident;
			}
			| macro_type T_FUNC {
				yyerror("Illegal attempt to redefine native method");
				$$.ident.s = 0;
				$$.ident.stringIsAlloced = 0;
			}

macro_type:	T_VOID
			| T_TSTRING
			| T_TBOOLEAN
			| T_TINT 
			| T_TCHAR 
			| T_TLONG 
			| T_TFLOAT

macrostart: { startmacro(); } macro_declaration '(' parameter_list ')' 
			{	$$.ident = $2.ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}

parameter_list:
			| non_empty_pardecl

non_empty_pardecl:
			non_empty_pardecl ',' par_decl 
			|
			par_decl

par_decl: type_ident variable_identifier {
				sym_makeInternalSymbol(_currentIdentifierContext, $2.ident.s, S_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, $2.ident.s,C_DEFINE_PARAMETER,$1.type,_nparam);
				free($2.ident.s);
				_nparam++;
			}

macro_definition: macrostart block {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName($1.ident.s, -1, 1);
					YY_EMIT(C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",$1.ident.s);
					_compilerConfiguration->cb_insertNewMacro($1.ident.s, _yyCurrentComment, &_bytecodeBuffer);
				}
				freeitem(&$1.ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				endmacro();
			} 

block:		'{' var_decls stmntlist '}'

var_decls:
			| var_decl var_decls

var_decl: type_ident variable_identifier assignment_expression ';'	{
				makeInternalSymbol($2.ident.s,
					$1.type,
					($1.type == S_STRING) ? (intptr_t)"" : 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $2.ident.s);
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
				vname_count = 0;
			} | type_ident variable_identifier ';'	{
				if ($1.type == S_STRING) {
					makeInternalSymbol($2.ident.s, $1.type, (intptr_t)"");
				} else {
					makeInternalSymbol($2.ident.s, $1.type, (intptr_t)0);
				}
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
				vname_count = 0;
			}

statement: ';'
			| call_expression ';' { $$.needsPop = 1; }
			| assignment ';' { $$.needsPop = 1; }
			| if_expression { $$.needsPop = 1; }
			| while { $$.needsPop = 1; }
			| case { $$.needsPop = 1; }
			| break { $$.needsPop = 0; }
			| continue { $$.needsPop = 0; }
			| return_expression { $$.needsPop = 0; }
			| goto	{ $$.needsPop = 0; }
			| block { $$.needsPop = 1; }
			| increment_expression { $$.needsPop = 1; }
			| label { $$.needsPop = 0; }
			| error {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			}


assignment	: variable_reference assignment_or_shorthand_assignment {
				if (_generateShorthandAssignment) {
					_bytecodeBuffer.bb_current = bytecode_emitShorthandAssignment(&_bytecodeBuffer, _generateShorthandAssignment, $1.ident.s);
				} else {
					_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $1.ident.s);
				}
				vname_count = 0;
				_generateShorthandAssignment = 0;
			}

assignment_or_shorthand_assignment:
			assignment_expression {
				_generateShorthandAssignment = 0;
				$$.v = $1.v;
			}
			|
			shorthand_assignment_operator arithmetic_expression	{ 
				_generateShorthandAssignment = $1.type;
				$$.v = $2.v;
			}

shorthand_assignment_operator: 
			T_SH_ASSIGN_MULT { $$.type = BIN_MUL; }
			| T_SH_ASSIGN_DIV { $$.type = BIN_DIV; }
			| T_SH_ASSIGN_MOD { $$.type = BIN_MOD; }
			| T_SH_ASSIGN_PLUS { $$.type = BIN_ADD; }
			| T_SH_ASSIGN_MINUS { $$.type = BIN_SUB; }

assignment_expression:	T_ASSIGN arithmetic_expression	{ $$.v = $2.v;	}

arithmetic_expression: simple_expression	{ $$.v = $1.v; }
			| '(' simple_expression ')'		{ $$.v = $2.v; }

simple_expression: binary_expression { $$.v = $1.v; }
			| binary_expression '[' binary_expression ']' { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
			| binary_expression T_DOTDOT binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }

increment_expression: 
			variable_reference T_MINUSMINUS  { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, $1.ident.s, -1); }
			|
			variable_reference T_PLUSPLUS { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, $1.ident.s, 1); }

s_bterm: 	value { $$.v = $1.v; }
			| call_expression 

binary_expression:	s_bterm  { $$.type = $1.type;	}
			| type_cast s_bterm		{
				$$.type = $1.type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, $$.type);
			}
			| '-' T_NUM {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-$2.num);
				} else {
					$$.type = S_NUMBER;
					$$.v.type = C_PUSH_LONG_LITERAL; 
					$$.v.data.longValue  = -$2.num;
				}
			}
			| '~' binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
			| '+' binary_expression { $$.v = $2.v }
			| '-' binary_expression { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &$2.v, -1); }
			| binary_expression '&' binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
			| binary_expression '|' binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
			| binary_expression '+' binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
			| binary_expression '^' binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
			| binary_expression '-' binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
			| binary_expression '*' binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
			| binary_expression '/' binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
			| binary_expression '%' binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
			| binary_expression T_SHIFT_LEFT binary_expression 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
			| binary_expression T_SHIFT_RIGHT binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
			| binary_expression T_POWER_TO binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
			| '(' binary_expression ')'	{ $$.v = $2.v; }

/*----- conditions ------------------------------------------*/

condition:	'(' logical_expression ')'

logical_expression:	
			| term
			| '!' logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
			| logical_expression T_AND logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
			| logical_expression T_OR logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
			| logical_expression '<' logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
			| logical_expression '>' logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
			| logical_expression T_LE logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
			| logical_expression T_GE logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
			| logical_expression T_EQ logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
			| logical_expression T_NE logical_expression { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
			| binary_expression '~' binary_expression   { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
			| binary_expression T_NMATCH binary_expression   { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }

term:		call_expression { $$.type = S_NUMBER; 	 	}
			| value {	$$.type = $1.type;	}

value:		T_NUMVAR {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = S_NUMBER;
			}
			| T_FLOATVAR 	{
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = S_FLOAT;
			}
			| T_RANGEVAR {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = S_RANGE;
			}
			| T_STRINGVAR {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = S_STRING;
			}
			| T_CHARACTERVAR {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = S_CHARACTER;
			}
			| T_BOOLEANVAR {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = S_BOOLEAN;
			}
			| string {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=$1.ident.s});
				$$.v.type = C_PUSH_STRING_LITERAL;
				free($1.ident.s);
			}
			| simple_literal {	$$.type = $1.type;	}
			| T_IDENT {
				yyerror("Undefined identifier %s", $1.ident.s);
				$$.v.type = C_PUSH_VARIABLE;  
				$$.v.data.string  = $1.ident.s;
			}

string:		T_STRING {	$$.ident = $1.ident;	}
			| string T_STRING { 
				if (($$.ident.s = malloc(strlen($1.ident.s)+strlen($2.ident.s)+1)) != 0) {
					strcat(strcpy($$.ident.s,$1.ident.s),$2.ident.s);
					free($1.ident.s);
					free($2.ident.s);
				} else {
					yyerror("buffer overlow");
					$$.ident = $2.ident;
				}
			}

break		: T_BREAK opt_num ';'
			{	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)$2.num,BRA_ALWAYS); }

continue		: T_CONTINUE opt_num ';'
			{	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)$2.num,BRA_ALWAYS); }

return_expression: 
			T_RETURN optional_bterm {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}

optional_bterm:
			| binary_expression

opt_num	: 	{	$$.num = 1;	}
			| T_NUM	{	$$.num = $1.num; }

case:		T_SWITCH condition '{' caselabels '}'

caselabels:
			| label block caselabels

label:		T_IDENT ':' {
				bytecode_createBranchLabel(&_bytecodeBuffer, $1.ident.s);
				freeitem(&$1.ident.s);
			}

goto:		T_GOTO T_IDENT {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction($2.ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&$2.ident.s);
			}
			| T_BRAEQ T_IDENT {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction($2.ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&$2.ident.s);
			} ';'

while		: T_WHILE {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			} condition {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			} local_block {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

local_block: statement | '{' stmntlist '}'

if_expression:
			T_IF condition {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			} local_block {
				_iflevel--;
			} elseclause {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}

elseclause: {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
			| T_ELSE {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
			local_block

stmntlist:	{ $$.needsPop = 0; }
			| statement { 
				if ($1.needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			} stmntlist

call_expression: T_FUNC {
				// Native call
				_currentNativeMethodCalled = $1.funcp;
				_currentFunctionCallParamIndex = 0;
				$$.funcp = $1.funcp;
				YY_EMIT(C_SET_PARAMETER_STACK,(GENERIC_DATA){0});
			} '(' parameter_list ')' {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction($1.funcp)}, _currentFunctionCallParamIndex);
				_currentNativeMethodCalled = 0;
			}
			| T_IDENT {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndex = 0;
				_calledFunctionOrMacro = $1.ident.s;
				$$.funcp = 0;
				YY_EMIT(C_SET_PARAMETER_STACK,(GENERIC_DATA){0});
			} '(' parameter_list ')' {
				YY_EMIT(C_MACRO,(GENERIC_DATA){.string = _calledFunctionOrMacro});
				freeitem(&_calledFunctionOrMacro);
			}

parameter_list: nonempty_parameters

nonempty_parameters: parameter 
			| nonempty_parameters ',' parameter
			| error {
				yyerror("illegal parameters for method call");
			}

parameter:	binary_expression { $$.type = $1.type; _currentFunctionCallParamIndex++; }

type_cast:	'(' type_ident ')'	{	$$.type = $2.type;		}

type_ident:   T_TCHAR				{	$$.type = S_CHARACTER;	}
			| T_TINT				{	$$.type = S_NUMBER;	}
			| T_TLONG				{	$$.type = S_NUMBER;	}
			| T_TFLOAT				{	$$.type = S_FLOAT;	}
			| T_TBOOLEAN			{	$$.type = S_BOOLEAN; }
			| T_TSTRING				{	$$.type = S_STRING;	}
			| T_RANGE				{	$$.type = S_RANGE;	}

boolean_literal: T_TRUE {
			$$.v.type = C_PUSH_BOOLEAN_LITERAL; 
			$$.v.data.booleanValue = 1;
		} | T_FALSE {
			$$.v.type = C_PUSH_BOOLEAN_LITERAL; 
			$$.v.data.booleanValue = 0;
		}

float_literal: T_FLOATING_POINT_NUMBER {
				$$.v.type = C_PUSH_FLOAT_LITERAL; 
				$$.v.data.doubleValue = $1.v.data.doubleValue;
			}

integer_literal: T_NUM {
				$$.v.type = C_PUSH_LONG_LITERAL; 
				$$.v.data.longValue  = $1.num;
			}

character_literal: T_CHARACTER {
				$$.v.type = C_PUSH_CHARACTER_LITERAL; 
				$$.v.data.uchar = (char)$1.num;
			}

simple_literal:	integer_literal { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction($1.v.data.longValue);
				} else {
					$$.v = $1.v;
				}
				$$.type = S_NUMBER;
			}
			| float_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.type = S_FLOAT;
			}
			| character_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.type = S_CHARACTER;
			}
			| boolean_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.type = S_BOOLEAN;
			}
%%

/**
 * bytecode_emitPushParameterInstruction()
 * Special processing when pushing integer literals: if the integer literal is pushed as 
 * parameter of a native function and this is form-related native function push a special
 * byte code as a marker.
 */
static void bytecode_emitPushParameterInstruction(long long lValue)
{
	int opCode = C_PUSH_INTEGER_LITERAL;
	if (_currentNativeMethodCalled && function_parameterIsFormStart(_currentNativeMethodCalled,_currentFunctionCallParamIndex+1)) {
		opCode = C_FORM_START;
	}
	YY_EMIT(opCode, (GENERIC_DATA){.longValue = lValue});
}

/*---------------------------------*/
/* YYtrace()					*/
/*---------------------------------*/
#ifdef YYDEBUG
void YYtrace(char *fmt, ...)
{	va_list ap;
	static FILE *fp;

	if (fp == 0 && (fp = fopen("O:\\PROT","w")) == 0)
		return;
	va_start(fmt, ap);
	vfprintf(fp,fmt,ap);
	va_end(ap);
	fflush(fp);
}
#endif

/*---------------------------------*/
/* bytecode_emitGotoInstruction()					*/
/*---------------------------------*/
static char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	if (level < 0)
		yyerror("illegal break/continue level");
	else
		_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(name,&_bytecodeBuffer,bratyp);
	return _bytecodeBuffer.bb_current;
}

/*---------------------------------*/
/* bytecode_generateAutoLabelNamePrefix()					*/
/*---------------------------------*/
static int bytecode_generateAutoLabelNamePrefix(char *prefix,int level)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	return bytecode_createBranchLabel(&_bytecodeBuffer, name);
}

/*---------------------------------*/
/* bytecode_destroyAutoLabelNamePrefix()					*/
/*---------------------------------*/
static void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	/*
	 * autolabel is not used any more: kill it
	 */
	bytecode_destroyLabelNamed(name);
}

/*---------------------------------*/
/* freeitem()					*/
/*---------------------------------*/
static void freeitem(char **p)
{
	if (*p) {
		free(*p);
		*p = 0;
	}
}

/*---------------------------------*/
/* startmacro()				*/
/*---------------------------------*/
static void startmacro(void)
{
#ifdef YYDEBUG
int	yydebug = 1;
#endif
	if (!_bytecodeBuffer.bb_start) {
		_bytecodeBuffer.bb_end = ((_bytecodeBuffer.bb_start = malloc(REC_SPACE)) == 0) ? 
			_bytecodeBuffer.bb_start : _bytecodeBuffer.bb_start + REC_SPACE;
	}
	_bytecodeBuffer.bb_current = _bytecodeBuffer.bb_start; 
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	yyerrflg = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext);
}

/*---------------------------------*/
/* endmacro()					*/
/*---------------------------------*/
static void endmacro(void) {
	if (_bytecodeBuffer.bb_start) {
		free(_bytecodeBuffer.bb_start);
		_bytecodeBuffer.bb_start = 0;
		_bytecodeBuffer.bb_end = 0;
	}
	_currentIdentifierContext = sym_popContext(_currentIdentifierContext);
}

#if 0
/*---------------------------------*/
/* main()						*/
/*---------------------------------*/
main(int ac, char **av)
{

	av = av;
	ac = ac;

	printf("yyparse ret == %d\n",yyparse());
	return 0;
}
#endif

