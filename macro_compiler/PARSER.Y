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

%token T_TSTRING T_TINT T_TCHAR T_TLONG T_TFLOAT
%token T_IDENT T_NUM T_FLOATING_POINT_NUMBER T_STRING T_OCTNUM T_CONST
%token T_TRUE T_FALSE T_BOOLEAN
%token T_FUNC T_MACRO T_DOTDOT T_PLUSPLUS T_MINUSMINUS
%token T_IF T_ELIF T_ELSE T_WHILE T_GOTO T_BRAEQ 
%token T_LOCAL T_SWITCH T_BREAK T_CONTINUE T_RETURN 
%token T_NUMVAR T_FLOATVAR T_STRINGVAR T_BOOLEANVAR
%token T_NMATCH
%token T_ASSIGN T_NE T_EQ
%token T_OR T_AND T_LE T_GE
%token T_DEFMACRO 
%token T_CMDSEQPREFIX

%nonassoc T_NE T_EQ '~' T_NMATCH
%left    '<' '>' T_LT T_GT  
%left    T_OR T_AND
%left	'&' '|'
%left	'+' '-' '^'
%left	'*' '/' '%'

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
						 t == C_PUSH_CHARACTER_LITERAL)
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
static void 	bytecode_emitPushParameterInstruction(long lValue);

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
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern unsigned char* bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber);

#define		YY_EMIT(opCode, param)		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, opCode, param);

extern int yylex(void );
unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
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
					bString ? 
					S_CONSTSTRING : S_CONSTNUM, $4.v.data);
				free($2.ident.s);
				if (bString) {
					free($4.v.data.string);
				}
			}

constant_literal:
			simple_literal 	{ 
				$$.v = $1.v; 
				$$.v.type = S_NUMBER;
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
			T_STRINGVAR 	{	$$.ident = $1.ident; }
			| T_NUMVAR 		{	$$.ident = $1.ident; }
			| T_FLOATVAR 	{	$$.ident = $1.ident; }

/*-- MACROS -----------------------------------------------------*/

macro_declaration: T_DEFMACRO T_IDENT {
				$$.ident = $2.ident;
			}
			| T_DEFMACRO T_FUNC {
				yyerror("Illegal attempt to redefine native method");
				$$.ident.s = 0;
				$$.ident.stringIsAlloced = 0;
			}

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
			| call_expression ';'
			| assignment ';'
			| if
			| while
			| case
			| break
			| continue
			| return
			| goto
			| block
			| increment_expression
			| label
			| error {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			}


assignment	: variable_reference assignment_expression {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $1.ident.s);
				vname_count = 0;
			}

assignment_expression:	T_ASSIGN arithmetic_expression	{ $$.v = $2.v;	}

arithmetic_expression: simple_expression	{ $$.v = $1.v; }
			| '(' simple_expression ')'		{ $$.v = $2.v; }

simple_expression: bterm					{ $$.v = $1.v; }
			| '~' arithmetic_expression 	{ 
				$$.type = S_BOOLEAN;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_NOT, 0);
			}

increment_expression: 
			variable_reference T_MINUSMINUS  { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, $1.ident.s, -1); }
			|
			variable_reference T_PLUSPLUS { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, $1.ident.s, 1); }

s_bterm: 	value { $$.v = $1.v; }
			| call_expression {
				if ($1.funcp && function_returnsString($1.funcp)) {
					$$.type = S_STRING;
				} else {
					$$.type = S_NUMBER;
				}
			}

bterm:		s_bterm 	{ $$.type = $1.type;	}
			| type_cast s_bterm		{
				$$.type = $1.type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, $$.type);
			}
			| '+' bterm { $$.v = $2.v }
			| '-' bterm { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &$2.v, -1); }
			| bterm '&' bterm 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
			| bterm '|' bterm 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
			| bterm '+' bterm 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
			| bterm '^' bterm 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
			| bterm '-' bterm 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
			| bterm '*' bterm 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
			| bterm '/' bterm 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
			| bterm '%' bterm 	{ YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }

			| '(' bterm ')'	{ $$.v = $2.v; }

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
			| bterm '~' bterm   { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
			| bterm T_NMATCH bterm   { $$.type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }

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
			| T_STRINGVAR 	{
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = S_STRING;
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

return		: T_RETURN bterm {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}

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
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction($2.ident.s,&_bytecodeBuffer,BRA_EQ);
				freeitem(&$2.ident.s);
			} ';'

while		: T_WHILE {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			} condition {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			} local_block {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

local_block: statement | '{' stmntlist '}'

if			: T_IF condition {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			} statement {
				_iflevel--;
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			} elseclause {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}

elseclause	: 
			| T_ELSE statement 

stmntlist		:
			| statement { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); } stmntlist

call_expression: T_FUNC {
				// Native call
				_currentNativeMethodCalled = $1.funcp;
				_currentFunctionCallParamIndex = 0;
				$$.funcp = $1.funcp;
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

parameter:	bterm { $$.type = $1.type; _currentFunctionCallParamIndex++; }

type_cast:	'(' type_ident ')'	{	$$.type = $2.type;		}

type_ident:   T_TCHAR				{	$$.type = S_CHARACTER;	}
			| T_TINT				{	$$.type = S_NUMBER;	}
			| T_TLONG				{	$$.type = S_NUMBER;	}
			| T_TFLOAT				{	$$.type = S_FLOAT;	}
			| T_BOOLEAN				{	$$.type = S_BOOLEAN; }
			| T_TSTRING				{	$$.type = S_STRING;	}

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

simple_literal:	integer_literal { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction($1.v.data.intValue);
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
static void bytecode_emitPushParameterInstruction(long lValue)
{
	int opCode = C_PUSH_INTEGER_LITERAL;
	if (_currentNativeMethodCalled && function_parameterIsFormStart(_currentNativeMethodCalled,_currentFunctionCallParamIndex+1)) {
		opCode = C_FORM_START;
	}
	YY_EMIT(opCode, (GENERIC_DATA){.val = lValue});
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

