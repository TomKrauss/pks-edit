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

%define parse.error detailed

%token T_NAMESPACE T_REQUIRE T_FOR 
%token T_NEW T_STATIC T_VOID T_TYPE_IDENTIFIER
%token T_IDENT T_NUM T_FLOATING_POINT_NUMBER T_STRING T_CHARACTER T_CONST
%token T_TRUE T_FALSE 
%token T_FUNC T_DOTDOT T_PLUSPLUS T_MINUSMINUS
%token T_IF T_ELSE T_WHILE T_GOTO
%token T_SH_ASSIGN_PLUS T_SH_ASSIGN_MINUS T_SH_ASSIGN_MULT T_SH_ASSIGN_DIV T_SH_ASSIGN_MOD
%token T_SWITCH T_CASE T_DEFAULT T_BREAK T_CONTINUE T_RETURN 
%token T_VARIABLE T_ASSOC_ARROW 
%token T_NMATCH
%token T_ASSIGN T_NE T_EQ T_SHIFT_LEFT T_SHIFT_RIGHT 
%token T_OR T_AND T_LE T_GE T_POWER_TO

%left    T_OR T_AND
%left	'&' '|' '^'
%nonassoc T_NE T_EQ 
%left    '~' T_NMATCH
%left    '<' '>' T_LE T_GE  
%left	T_SHIFT_LEFT T_SHIFT_RIGHT 
%left	'+' '-' 
%left	'*' '/' '%' T_POWER_TO
%left	'[' T_DOTDOT

/* %right	T_NOTGLITCH */

%{

#include <windows.h>
#include <stdio.h>
#include <stddef.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "symbols.h"
#include "arraylist.h"

# define	YYPTRDIFF_T ptrdiff_t
# define	YYPTRDIFF_MAXIMUM SIZE_MAX

# define 	YYSTYPE _YYSTYPE

# define 	MAXEXPR			12
# define	REC_SPACE		32000

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
static void 	bytecode_emitPushParameterInstruction(long long lValue);

static int _arraySize;
static int	_currentNamespaceIndex;
static int  _foreachCursorIndex;
static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				 *switchTableId  = "%sw%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

extern 			int	_bDefiningConst;
extern 			int	_bInHeader;
extern char*    _yyCurrentComment;
extern void		yyerror(const char* s, ...);
extern void		yywarning(char* s, ...);
extern void		yyrequire(const char* pszNamespace);
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern int macro_lookupNamespace(const char* pszNamespacename);
extern void bytecode_emitGotoInstruction(BYTECODE_BUFFER* pBuffer, char *prefix, int level, int bratyp);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nIncrement);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);
extern void bytecode_startSwitchTable(int aLevel);
extern int bytecode_generateAutoLabelNamePrefix(BYTECODE_BUFFER* pBuffer, char *prefix,int level);
extern void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);

/*
 * Write out the recorded case jumps and cleanup switch table.
 */
extern void bytecode_flushSwitchTable(BYTECODE_BUFFER* pBuffer, int aLevel);

/*
 * Add a switch case condition to the current switch table being constructed.
 */
extern void bytecode_addSwitchCondition(BYTECODE_BUFFER* pBuffer, int aLevel, PKS_VALUE_TYPE t, GENERIC_DATA data);

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

static void parser_defineVariable(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal) {
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
	if (sType == VT_STRING && !tVal) {
		tVal = (intptr_t)"";
	}
	sym_createSymbol(_currentIdentifierContext, (char*) pszName, S_VARIABLE, sType, (GENERIC_DATA) {.val = tVal});
}

static void push_newForeachCursor() {
	char buf[32];
	sprintf(buf, "#c%d", _foreachCursorIndex++);
	YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=buf});
}

static PKS_VALUE_TYPE types_pushFieldIndexWithError(PKS_VALUE_TYPE t, const char* pszFieldName) {
	PKS_VALUE_TYPE tProp;
	int nIndex = types_indexForProperty(t, pszFieldName, &tProp);
	if (nIndex < 0) {
		yyerror("unknown field %s of type %s", pszFieldName, types_nameFor(t));
	} else {
		YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue = nIndex});
	}
	free((char*)pszFieldName);
	return tProp;
}

%}

%%

file_structure: {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
			namespace prerequisites global_declarations {
				finalize_header();
			} header_delimiter macro_definitions 

header_delimiter:
			| '!'

macro_definitions: 
			| macro_definition macro_definitions 

namespace:  {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
			| T_NAMESPACE T_IDENT ';' {
				_currentNamespaceIndex = macro_lookupNamespace($2.ident.s);
				free($2.ident.s);
			}

prerequisites:
			| prerequisites require

require:	T_REQUIRE T_STRING ';' {
				yyrequire($2.ident.s);
				free($2.ident.s);
			}

global_declarations:
			| global_declarations constants ';'
			| global_declarations global_var ';' 

scope:		{ $$.ident.scope = MS_GLOBAL; }
			| T_STATIC { $$.ident.scope = MS_LOCAL; }
			
/*-- CONSTANTS --------------------------------------------------*/

constants:	T_CONST constant_list

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

macro_declaration: T_IDENT {
				$$.ident = $1.ident;
			}
			| T_FUNC {
				yyerror("Illegal attempt to redefine native method");
				$$.ident.s = 0;
				$$.ident.stringIsAlloced = 0;
			}

macro_type:	scope T_VOID {
				$$.ident.scope = $1.ident.scope;
			}
			| 
			scope type_name {
				$$.ident.scope = $1.ident.scope;
			}

macrostart: macro_type macro_declaration '(' parameter_list ')' optional_description 
			{	$$.ident = $2.ident;
				$$.ident.scope = $1.ident.scope;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
			
global_var: scope var_decl

var_decl:	type_name variable_identifier assignment_expression {
				parser_defineVariable($2.ident.s, $1.type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $2.ident.s);
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
			} | type_name variable_identifier {
				parser_defineVariable($2.ident.s, $1.type, 0);
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
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
			par_decl
			|
			non_empty_pardecl ',' par_decl


par_decl:	type_name variable_identifier {
				sym_createSymbol(_currentIdentifierContext, $2.ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, $2.ident.s,C_DEFINE_PARAMETER,$1.type,_nparam);
				free($2.ident.s);
				_nparam++;
			}

macro_definition: { init_macroDefinition(); } 
				macrostart block {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName($2.ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",$2.ident.s);
				MACRO_PARAM param = {
					.mp_name = $2.ident.s,
					.mp_comment = _yyCurrentComment,
					.mp_bytecodeLength = _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start,
					.mp_buffer = _bytecodeBuffer.bb_start,
					.mp_scope = $2.ident.scope,
					.mp_namespaceIdx = _currentNamespaceIndex
				};
				_compilerConfiguration->cb_insertNewMacro(&param);
				freeitem(&$2.ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			} 

block:		'{' var_decls stmntlist closing_brace

var_decls:
			| var_decls var_decl ';' 

statement: ';'
			| call_expression ';' { $$.needsPop = 1; }
			| assignment ';' { $$.needsPop = 1; }
			| if_expression { $$.needsPop = 1; }
			| while { $$.needsPop = 1; }
			| foreach { $$.needsPop = 1; }
			| switch_expression { $$.needsPop = 1; }
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
			| error ';' {
				$$.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}

assignment: variable_reference assignment_expression {
				if ($1.ident.type != $2.v.type) {
				// not enough type info yet for this:	yyerror("Trying to assign incompatible types.");
				}
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $1.ident.s);
			}
			|
			variable_reference '.' T_IDENT {
				types_pushFieldIndexWithError($1.ident.type, $3.ident.s);
			} assignment_expression {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)$1.ident.s });
			} 
			|
			variable_reference '[' binary_expression ']' assignment_expression {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)$1.ident.s });
			} 
			|
			variable_reference {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
			} shorthand_assignment_operator simple_expression {
				YY_EMIT(C_BINOP, (GENERIC_DATA){$3.type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $1.ident.s);
			}

shorthand_assignment_operator: 
			T_SH_ASSIGN_MULT { $$.type = BIN_MUL; }
			| T_SH_ASSIGN_DIV { $$.type = BIN_DIV; }
			| T_SH_ASSIGN_MOD { $$.type = BIN_MOD; }
			| T_SH_ASSIGN_PLUS { $$.type = BIN_ADD; }
			| T_SH_ASSIGN_MINUS { $$.type = BIN_SUB; }

assignment_expression:	T_ASSIGN simple_expression	{ $$.v = $2.v;	}

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
			| constructor_expression

constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')' {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){$2.type});
			}

binary_expression:
			s_bterm { $$.type = $1.type;	}
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
			| '!' binary_expression { $$.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
			| '~' binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
			| '+' binary_expression { $$.v = $2.v; }
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
			| variable_reference assignment_expression {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, $1.ident.s);
			}

/*----- conditions ------------------------------------------*/

condition:	'(' binary_expression ')'
			| '(' binary_expression {
				yyerror("Missing closing parenthesis )");
			}

value:		T_VARIABLE {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.type = $1.ident.type;
			}
			| struct_reference
			| string {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=$1.ident.s});
				$$.v.type = C_PUSH_STRING_LITERAL;
				free($1.ident.s);
			}
			| simple_literal {	$$.type = $1.type;	}
			| map_literal {
				$$.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
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

struct_reference: T_VARIABLE '.' T_IDENT {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=$1.ident.s});
				$$.ident.type = types_pushFieldIndexWithError($1.ident.type, $3.ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
			| struct_reference '.' T_IDENT {
				$$.ident.type = types_pushFieldIndexWithError($1.ident.type, $3.ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}

map_literal: '{' {
				_currentArrayLiteral = arraylist_create(1);
			} optional_map_associates closing_brace

optional_map_associates:
			|
			map_associates

map_associates:
			map_associate
			|
			map_associates ',' map_associate

map_associate:
			T_STRING T_ASSOC_ARROW simple_literal { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, $1.ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, $3.v.type, $3.v.data.longValue));
			}
			| T_STRING T_ASSOC_ARROW T_STRING { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, $1.ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, $3.ident.s));
			}

array_literal: '[' {
				_currentArrayLiteral = arraylist_create(1);
			} 
			']'
			| '[' {
				_currentArrayLiteral = arraylist_create(3);
			} array_elements ']'

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

break:		T_BREAK opt_num ';'
			{	bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel-(int)$2.num,BRA_ALWAYS); }

continue:	T_CONTINUE opt_num ';'
			{	bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel-(int)$2.num,BRA_ALWAYS); }

return_expression: 
			T_RETURN optional_bterm {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}

optional_bterm:
			| binary_expression

opt_num	: 	{	$$.num = 1;	}
			| T_NUM	{	$$.num = $1.num; }

case_clauses:
			| case_clause case_clauses

case_clause: case_selector stmntlist

case_selector: T_CASE case_condition ':' 
			|
			T_DEFAULT ':' {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}

case_condition:
			T_NUM {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=$1.num});
			}
			| T_STRING {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=$1.ident.s});
			}
			| T_NUM T_DOTDOT T_NUM {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=$1.num, .range.r_end=$3.num});
			}

label:		T_IDENT ':' {
				bytecode_createBranchLabel(&_bytecodeBuffer, $1.ident.s);
				freeitem(&$1.ident.s);
			}

goto:		T_GOTO T_IDENT {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction($2.ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&$2.ident.s);
			}

in_clause:	'(' type_name variable_identifier {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=$3.ident.s});
				parser_defineVariable($3.ident.s, $2.type, (intptr_t)0);
				if ($3.ident.stringIsAlloced) {
					free($3.ident.s);
				}
			} ':' simple_expression ')' {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}

switch_expression:		
			T_SWITCH  '(' value ')' 
			{
				_breaklevel++;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
			'{' case_clauses '}'
			{
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(&_bytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

foreach:	T_FOR {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lstartid,_breaklevel);
			} in_clause {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			} local_block {
				_breaklevel--;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

while:		T_WHILE {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lstartid,_breaklevel);
			} condition {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			} local_block {
				_breaklevel--;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

local_block: statement | '{' stmntlist closing_brace

closing_brace: '}'

if_expression:
			T_IF condition {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			} local_block {
				_iflevel--;
			} else_clause {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}

else_clause: {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
			| T_ELSE {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
			local_block

stmntlist:	{ $$.needsPop = 0; }
			| statement { 
				if ($1.needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			} stmntlist

closing_paren: ')'

call_expression: T_FUNC '(' {
				// Native call
				_currentNativeMethodCalled = $1.funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				$$.funcp = $1.funcp;
			} parameter_list closing_paren {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction($1.funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
			| function_id_or_pointer '(' {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				$$.funcp = 0;
			} parameter_list closing_paren {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, $1.ident.operation,(GENERIC_DATA){.string = $1.ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&$1.ident.s);
			}

function_id_or_pointer:
			'*' variable_reference {
				$$.ident.operation = C_MACRO_REF;
				$$.ident.s = _strdup($2.ident.s);
				$$.ident.stringIsAlloced = 1;
			}
			|
			T_IDENT {
				$$.ident = $1.ident;
				$$.ident.operation = C_MACRO;
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
	_bInHeader = 1;
}

static void finalize_header() {
	macro_defineNamespaceInitializer(_currentNamespaceIndex, _bytecodeBuffer.bb_start, _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start);
	destroy_bytecodeBuffer();
	_bInHeader = 0;
}

/*---------------------------------*/
/* init_macroDefinition()				*/
/*---------------------------------*/
static void init_macroDefinition(void)
{
	_foreachCursorIndex = 0;
	_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext, 0);
	init_bytecodeBuffer();
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

