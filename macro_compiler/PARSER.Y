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

%error-verbose

%token T_NAMESPACE T_STATIC T_FOR 
%token T_TYPE_IDENTIFIER
%token T_IDENT T_NUM T_FLOATING_POINT_NUMBER T_STRING T_CHARACTER T_CONST
%token T_TRUE T_FALSE T_TBOOLEAN
%token T_FUNC T_MACRO T_DOTDOT T_PLUSPLUS T_MINUSMINUS
%token T_IF T_ELIF T_ELSE T_WHILE T_GOTO T_BRAEQ
%token T_SH_ASSIGN_PLUS T_SH_ASSIGN_MINUS T_SH_ASSIGN_MULT T_SH_ASSIGN_DIV T_SH_ASSIGN_MOD
%token T_LOCAL T_SWITCH T_BREAK T_CONTINUE T_RETURN 
%token T_VARIABLE T_ASSOC_ARROW 
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
#include "arraylist.h"

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
static int		_nparam,
				_breaklevel,
				_iflevel,
				_stringflg;
static      int  _currentFunctionCallParamIndices[32];
static      int  *_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
static 		void init_header(void);
static 		void finalize_header(void);
static 		void init_macroDefinition(void);
static 		void finalize_macroDefinition(void);
static 		void freeitem(char **p);
static 		char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp);
static		int bytecode_generateAutoLabelNamePrefix(char *prefix,int level);
static		void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
static void 	bytecode_emitPushParameterInstruction(long long lValue);

static int _arraySize;
static MACRO_SCOPE _macroScope;
static MACROC_INSTRUCTION_OP_CODE _functionCallOp;
static int	_currentNamespaceIndex;
static int  _foreachCursorIndex;
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
extern void		yywarning(char* s, ...);
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern int macro_lookupNamespace(const char* pszNamespacename);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nIncrement);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);
/*
 * Destroy an array list containing TYPE_OBJECT_POINTERs.
 */
extern void bytecode_destroyArraylistWithPointers(ARRAY_LIST* pList);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern unsigned char* bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber);

#define		YY_EMIT(opCode, param)		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, opCode, param);

extern int yylex(void );
void 		bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int typ, long arraySizeOrParamIndex);

char*		bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int typ);
int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
char*		bytecode_generateAutoLabelName(char *prefix, int num);
int 		function_returnsString(void *ep);
static ARRAY_LIST* _currentArrayLiteral;

IDENTIFIER_CONTEXT* _currentIdentifierContext;

static void makeInternalSymbol(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal) {
	if (_arraySize < 0) {
		yyerror("Illegal negative array size %d", _arraySize);
		_arraySize = 0;
	}
	bytecode_defineVariable(&_bytecodeBuffer, pszName, C_DEFINE_VARIABLE, sType, _arraySize);
	_arraySize = 0;
	IDENTIFIER_CONTEXT* pContext = sym_getContext(_currentIdentifierContext, (char*)pszName);
	if (pContext && pContext != sym_getGlobalCompilerContext()) {
		yyerror("Redefinition of variable %s", pszName);
	}
	if (sType == VT_STRING) {
		if (!tVal) {
			tVal = (intptr_t)"";
		}
			

	}
	sym_createSymbol(_currentIdentifierContext, (char*) pszName, S_VARIABLE, sType, (GENERIC_DATA) {.val = tVal});
}

static void push_newForeachCursor() {
	char buf[32];
	sprintf(buf, "#c%d", _foreachCursorIndex++);
	YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=buf});
}

%}

%%

file_structure: {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
			header_declarations {
				finalize_header();
			}
			macro_definitions 

header_declarations:
			namespace global_declarations

namespace:  {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
			| T_NAMESPACE T_IDENT ';' {
				_currentNamespaceIndex = macro_lookupNamespace($2.ident.s);
				free($2.ident.s);
			}

global_declarations:
			| global_declarations constants 
			| global_declarations global_vars

global_vars:
			| global_var global_vars

global_var: scope var_decl

scope: { _macroScope = MS_GLOBAL; }
			| T_STATIC { _macroScope = MS_LOCAL; }
			
macro_definitions: 
			| macro_definitions macro_definition 

/*-- CONSTANTS --------------------------------------------------*/

constants:	T_CONST constant_list ';'

constant_list:
			constdef
			| constant_list ',' constdef

constdef:	{ _bDefiningConst = 1; } variable_identifier T_ASSIGN constant_literal { _bDefiningConst = 0; } 
			{
				BOOL bString = ($4.v.type == VT_STRING);
				sym_createSymbol(_currentIdentifierContext, $2.ident.s,
					S_CONSTANT, $4.v.type, $4.v.data);
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
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
				$$.v.type = VT_STRING;
			}

variable_identifier:	
			T_IDENT 				{	$$.ident = $1.ident; }
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
			T_VARIABLE	 		{	$$.ident = $1.ident; }
			| T_IDENT			{   
				yyerror("Using undeclared variable %s", $1.ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, $1.ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0});
				free($1.ident.s);
				$$.ident = $1.ident;
			}

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
			| T_TYPE_IDENTIFIER

macrostart: { init_macroDefinition(); } macro_declaration '(' parameter_list ')' optional_description 
			{	$$.ident = $2.ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
			

optional_description:
			|
			T_STRING {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free($1.ident.s);
			}

parameter_list:
			| non_empty_pardecl

non_empty_pardecl:
			non_empty_pardecl ',' par_decl 
			|
			par_decl

par_decl:	type_name variable_identifier {
				sym_createSymbol(_currentIdentifierContext, $2.ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, $2.ident.s,C_DEFINE_PARAMETER,$1.type,_nparam);
				free($2.ident.s);
				_nparam++;
			}

macro_definition: scope macrostart block {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName($2.ident.s, -1, 1);
					YY_EMIT(C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",$2.ident.s);
					MACRO_PARAM param = {
						.mp_name = $2.ident.s,
						.mp_comment = _yyCurrentComment,
						.mp_bytecodeLength = _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start,
						.mp_buffer = _bytecodeBuffer.bb_start,
						.mp_scope = _macroScope,
						.mp_namespaceIdx = _currentNamespaceIndex
					};
					_compilerConfiguration->cb_insertNewMacro(&param);
				}
				freeitem(&$2.ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			} 

block:		'{' var_decls stmntlist closing_brace

var_decls:
			| var_decl var_decls

var_decl:	type_name variable_identifier assignment_expression ';'	{
				makeInternalSymbol($2.ident.s, $1.type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $2.ident.s);
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
				vname_count = 0;
			} | type_name variable_identifier ';'	{
				makeInternalSymbol($2.ident.s, $1.type, 0);
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
			| foreach { $$.needsPop = 1; }
			| case { $$.needsPop = 1; }
			| break { $$.needsPop = 0; }
			| continue { $$.needsPop = 0; }
			| return_expression { $$.needsPop = 0; }
			| goto	{ $$.needsPop = 0; }
			| block { $$.needsPop = 1; }
			| increment_expression { $$.needsPop = 1; }
			| label { $$.needsPop = 0; }
			| var_decl {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
			| errors ';' {
				$$.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}

errors:		error 
			| errors error

assignment: variable_reference assignment_expression {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $1.ident.s);
				vname_count = 0;
			}
			|
			variable_reference '[' value ']' assignment_expression {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)$1.ident.s });
				vname_count = 0;
			} 
			|
			variable_reference {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
			} shorthand_assignment_operator arithmetic_expression {
				YY_EMIT(C_BINOP, (GENERIC_DATA){$3.type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $1.ident.s);
				vname_count = 0;
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
			| range_expression

binary_expression_or_range:
			binary_expression
			| range_expression

range_expression:
			| binary_expression T_DOTDOT binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); } range_increment

range_increment:
			| T_DOTDOT binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }

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
					$$.type = VT_NUMBER;
					$$.v.type = C_PUSH_LONG_LITERAL; 
					$$.v.data.longValue  = -$2.num;
				}
			}
			| binary_expression '[' binary_expression_or_range ']' { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
			| '~' binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
			| '+' binary_expression { $$.v = $2.v }
			| '-' binary_expression { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &$2.v, -1); }
			| binary_expression '~' binary_expression   { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
			| binary_expression T_NMATCH binary_expression   { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
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
			| binary_expression T_AND binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
			| binary_expression T_OR binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
			| binary_expression '<' binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
			| binary_expression '>' binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
			| binary_expression T_LE binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
			| binary_expression T_GE binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
			| binary_expression T_EQ binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
			| binary_expression T_NE binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
			| '(' binary_expression ')'	{ $$.v = $2.v; }

/*----- conditions ------------------------------------------*/

condition:	'(' logical_expression ')'
			| '(' binary_expression ')'
			| '(' logical_expression {
				yyerror("Missing closing parenthesis )");
			}

logical_expression:	
			| term
			| '!' logical_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }

term:		call_expression { $$.type = VT_NUMBER; 	 	}
			| value {	$$.type = $1.type;	}

value:		T_VARIABLE {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = $1.ident.type;
			}
			| string {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=$1.ident.s});
				$$.v.type = C_PUSH_STRING_LITERAL;
				free($1.ident.s);
			}
			| simple_literal {	$$.type = $1.type;	}
			| map_literal {
				$$.type = VT_MAP;
				yywarning("Map literals not yet supported");
			}
			| array_literal {	
				$$.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
			| T_IDENT {
				yyerror("Undefined identifier %s", $1.ident.s);
				$$.v.type = C_PUSH_VARIABLE;  
				$$.v.data.string  = $1.ident.s;
			}

map_literal: '(' map_associates ')'

map_associates:
			map_associates ',' map_associate
			| map_associate

map_associate:
			T_STRING T_ASSOC_ARROW simple_literal { free($1.ident.s); }
			| T_STRING T_ASSOC_ARROW T_STRING	  { free($1.ident.s); free($3.ident.s); }

array_literal: '{' {
				_currentArrayLiteral = arraylist_create(1);
			} 
			closing_brace
			| '{' {
				_currentArrayLiteral = arraylist_create(3);
			} array_elements closing_brace

array_elements:
			array_elements ',' array_element
			| array_element

array_element:
			simple_array_element {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, $1.v.type, $1.v.data.longValue));
			}
			|
			T_STRING {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, $1.ident.s));
			}

simple_array_element:
			T_NUM			{ $$.v.type = VT_NUMBER; $$.v.data.longValue = $1.num; }
			| T_CHARACTER	{ $$.v.type = VT_CHAR; $$.v.data.longValue = $1.num; }
			| T_TRUE		{ $$.v.type = VT_BOOLEAN; $$.v.data.longValue = 1; }
			| T_FALSE		{ $$.v.type = VT_BOOLEAN; $$.v.data.longValue = 0; }

string:		T_STRING {	$$.ident = $1.ident; }
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

in_clause:	'(' type_name variable_identifier {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=$3.ident.s});
				makeInternalSymbol($3.ident.s, $2.type, (intptr_t)0);
				if ($3.ident.stringIsAlloced) {
					free($3.ident.s);
				}
			} ':' simple_expression ')' {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}

foreach:	T_FOR {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			} in_clause {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			} local_block {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

while:		T_WHILE {
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

local_block: statement | '{' stmntlist closing_brace

closing_brace:
			{ yyerror("Missing closing brace '}'"); }
			|
			'}'

if_expression:
			T_IF condition {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			} local_block {
				_iflevel--;
			} else_clause {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}

else_clause: {
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

call_expression: T_FUNC '(' {
				// Native call
				_currentNativeMethodCalled = $1.funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				$$.funcp = $1.funcp;
			} parameter_list ')' {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction($1.funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
			| function_id_or_pointer '(' {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				$$.funcp = 0;
			} parameter_list ')' {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, _functionCallOp,(GENERIC_DATA){.string = $1.ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&$1.ident.s);
			}

function_id_or_pointer:
			'*' variable_reference {
				_functionCallOp = C_MACRO_REF;
				$$.ident.s = _strdup($2.ident.s);
				$$.ident.stringIsAlloced = 1;
			}
			|
			T_IDENT {
				_functionCallOp = C_MACRO;
				$$.ident = $1.ident;
			}
			| error {
				yyerror("Illegal function call expression");
				yyerrok;
				$$.ident.s = 0;
			}

parameter_list: nonempty_parameters

nonempty_parameters: parameter 
			| nonempty_parameters ',' parameter
			| error {
				yyerror("illegal parameters for method call");
			}

parameter:	simple_expression { $$.type = $1.type; (*_currentFunctionCallParamIndexP)++; }

type_cast:	'(' type_name ')'	{	$$.type = $2.type;		}

type_name:   T_TYPE_IDENTIFIER
			| T_TYPE_IDENTIFIER array_dimension	{	$$.type = VT_OBJECT_ARRAY;	}

array_dimension: '[' ']'
			| '[' T_NUM ']' {
				_arraySize = (long)$2.num;
			}

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
				$$.type = VT_NUMBER;
			}
			| float_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.type = VT_FLOAT;
			}
			| character_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.type = VT_CHAR;
			}
			| boolean_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.type = VT_BOOLEAN;
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
	if (_currentNativeMethodCalled && function_parameterIsFormStart(_currentNativeMethodCalled,*_currentFunctionCallParamIndexP+1)) {
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

static void init_bytecodeBuffer() {
	if (!_bytecodeBuffer.bb_start) {
		_bytecodeBuffer.bb_end = ((_bytecodeBuffer.bb_start = malloc(REC_SPACE)) == 0) ? 
			_bytecodeBuffer.bb_start : _bytecodeBuffer.bb_start + REC_SPACE;
	}
	_bytecodeBuffer.bb_current = _bytecodeBuffer.bb_start; 
}

static void destroy_bytecodeBuffer() {
	if (_bytecodeBuffer.bb_start) {
		free(_bytecodeBuffer.bb_start);
		_bytecodeBuffer.bb_start = 0;
		_bytecodeBuffer.bb_end = 0;
	}
}

static void init_header() {
	init_bytecodeBuffer(); 
}

static void finalize_header() {
	macro_defineNamespaceInitializer(_currentNamespaceIndex, _bytecodeBuffer.bb_start, _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start);
	destroy_bytecodeBuffer();
}

/*---------------------------------*/
/* init_macroDefinition()				*/
/*---------------------------------*/
static void init_macroDefinition(void)
{
#ifdef YYDEBUG
int	yydebug = 1;
#endif
	_foreachCursorIndex = 0;
	_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
	init_bytecodeBuffer();
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	yyerrflg = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext);
}

/*---------------------------------*/
/* finalize_macroDefinition()					*/
/*---------------------------------*/
static void finalize_macroDefinition(void) {
	destroy_bytecodeBuffer();
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

