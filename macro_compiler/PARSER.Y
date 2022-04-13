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

%token T_NAMESPACE T_REQUIRE T_FOR T_ANNOTATION T_NATIVE
%token T_NEW T_STATIC T_VOID T_TYPE_IDENTIFIER T_ELLIPSIS
%token T_IDENT T_NUM T_FLOATING_POINT_NUMBER T_STRING T_CHARACTER T_CONST
%token T_TRUE T_FALSE 
%token T_FUNC T_DOTDOT T_PLUSPLUS T_MINUSMINUS
%token T_IF T_ELSE T_WHILE T_GOTO
%token T_SH_ASSIGN_PLUS T_SH_ASSIGN_MINUS T_SH_ASSIGN_MULT T_SH_ASSIGN_DIV T_SH_ASSIGN_MOD
%token T_SWITCH T_CASE T_DEFAULT T_BREAK T_CONTINUE T_RETURN 
%token T_VARIABLE 
%token T_NMATCH
%token T_ASSIGN T_NE T_EQ T_SHIFT_LEFT T_SHIFT_RIGHT 
%token T_OR T_AND T_LE T_GE T_POWER_TO

%left    T_OR T_AND
%left	'&' '|' '^' '!'
%nonassoc T_NE T_EQ T_NMATCH '~' 
%left    '<' '>' T_LE T_GE  
%left	T_SHIFT_LEFT T_SHIFT_RIGHT 
%left	'+' '-' 
%left	'*' '/' '%' T_POWER_TO
%left	'[' T_DOTDOT '.'

%{

#include <windows.h>
#include <stdio.h>
#include <stddef.h>

#include "alloc.h"
#include "hashmap.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "stringutil.h"
#include "funcdef.h"
#include "symbols.h"
#include "linkedlist.h"
#include "arraylist.h"

# define	YYPTRDIFF_T ptrdiff_t
# define	YYPTRDIFF_MAXIMUM SIZE_MAX

# define 	YYSTYPE _YYSTYPE

# define 	MAX_BREAK_LEVEL	6
# define	REC_SPACE		32000

static BYTECODE_BUFFER  _buffers[MAX_BREAK_LEVEL];
static BYTECODE_BUFFER* _currentBytecodeBuffer;
static MACRO_PARAM		_macroParam;

static void		*_currentNativeMethodCalled;
static int		_nparam,
				_breaklevel,
				_iflevel;
static      int  _currentFunctionCallParamIndices[32];
static      int  *_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
static 		void init_header(void);
static 		void finalize_header(void);
static 		void init_native(void);
static 		void finalize_native(void);
static 		void init_macroDefinition(void);
static 		void finalize_macroDefinition(void);
static 		void freeitem(char **p);
static void 	bytecode_emitPushParameterInstruction(long long lValue);
static int	_currentNamespaceIndex;

static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				 *switchTableId  = "%sw%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

extern 			int	_bInNativeDefinition;
extern 			int	_bDefiningConst;
extern 			int	_bInHeader;

extern char*    _yyCurrentComment;
extern void		yyerror(const char* s, ...);
extern void		yywarning(char* s, ...);
extern void		yyrequire(const char* pszNamespace);
int				function_getIndexOfFunction(void *ep);
extern int macro_lookupNamespace(const char* pszNamespacename);
extern void bytecode_emitGotoInstruction(BYTECODE_BUFFER* pBuffer, char *prefix, int level, int bratyp);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nHeapIndex, int bLocalVar, int nIncrement);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitLocalAssignment(BYTECODE_BUFFER* pBuffer, int nHeapIndex);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);

/*
 * Eliminate unneccessary instructions from the byte codes generated.
 */
extern void bytecode_optimizeInstructions(BYTECODE_BUFFER* pBuffer);
extern void bytecode_startSwitchTable(int aLevel);
extern int bytecode_generateAutoLabelNamePrefix(BYTECODE_BUFFER* pBuffer, char *prefix,int level);
extern void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
extern const char* yy_getCurrentInputFilename();

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

#define		YY_EMIT(opCode, param)		_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, opCode, param);

extern int yylex(void );
void 		bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int typ, unsigned short arraySizeOrParamIndex, int nHeapIndex);

char*		bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int typ);
int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
char*		bytecode_generateAutoLabelName(char *prefix, int num);
static ANNOTATION*	_currentMethodAnnotations;
static int			_localVariableIndex;
static STRING_BUF*  _nativeMethodSignature;
static STRING_BUF*	_nativeMethodArgs;
IDENTIFIER_CONTEXT* _currentIdentifierContext;

static char* parser_signatureForType(PKS_VALUE_TYPE type) {
	char* pSig = "i";
	if (type == VT_STRING) {
		pSig = "s";
	} else if (type == VT_EDITOR_HANDLE) {
		pSig = "W";
	}
	return pSig;
}

static void parser_startNativeMethod(PKS_VALUE_TYPE t) {
	_nativeMethodSignature = stringbuf_create(128);
	_nativeMethodArgs = stringbuf_create(128);
	stringbuf_appendString(_nativeMethodSignature, parser_signatureForType(t));
}

static void parser_nativeMethodAddParam(PKS_VALUE_TYPE type, char* pszParamName) {
	if (strcmp("...", pszParamName) == 0) {
		stringbuf_appendString(_nativeMethodArgs, pszParamName);
	} else {
		stringbuf_appendString(_nativeMethodSignature, parser_signatureForType(type));
		stringbuf_appendString(_nativeMethodArgs, (char*)types_nameFor(type));
		stringbuf_appendChar(_nativeMethodArgs,' ');
		stringbuf_appendString(_nativeMethodArgs, pszParamName);
	}
	stringbuf_appendChar(_nativeMethodArgs,',');
}

static void parser_switchToBuffer(int aLevel) {
	_bInHeader = aLevel < 0;
	aLevel++;
	if (aLevel >= DIM(_buffers)) {
		yyerror("Nesting level of pushbacks exceeded. (maybe to many nested loops)");
		return;
	}
	_currentBytecodeBuffer = &_buffers[aLevel];
	if (!_currentBytecodeBuffer->bb_start) {
		_currentBytecodeBuffer->bb_end = ((_currentBytecodeBuffer->bb_start = malloc(REC_SPACE)) == 0) ? 
			_currentBytecodeBuffer->bb_start : _currentBytecodeBuffer->bb_start + REC_SPACE;
		_currentBytecodeBuffer->bb_current = _currentBytecodeBuffer->bb_start;
	}
	if (aLevel > 1) {
		_currentBytecodeBuffer->bb_current = _currentBytecodeBuffer->bb_start;
	}
}

static void parser_flushBuffer(int aLevel) {
	if (aLevel <= 0 ||aLevel >= DIM(_buffers)-1 || !_buffers[aLevel+1].bb_start) {
		return;
	}
	aLevel++;
	size_t nBytes = _buffers[aLevel].bb_current - _buffers[aLevel].bb_start;
	if (nBytes != 0) {
		memcpy(_buffers[1].bb_current, _buffers[aLevel].bb_start, nBytes);
		_buffers[1].bb_current += nBytes;
		// _buffers[aLevel].bb_current = _buffers[aLevel].bb_start;
	}
}

static void parser_emitAssignment(struct tagPARSE_IDENTIFIER* pIdent) {
	if (pIdent->isLocalVar) {
		_currentBytecodeBuffer->bb_current = bytecode_emitLocalAssignment(_currentBytecodeBuffer, pIdent->heapIndex);
	} else {
		_currentBytecodeBuffer->bb_current = bytecode_emitAssignment(_currentBytecodeBuffer, pIdent->s);
	}
}

static void parser_emitPushVariable(struct tagPARSE_IDENTIFIER* pIdent) {
	if (pIdent->isLocalVar) {
		_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_PUSH_LOCAL_VARIABLE, (GENERIC_DATA){.intValue=pIdent->heapIndex});
	} else {
		YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=pIdent->s});
	}
}

static void parser_defineVariable(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal, int arraySize) {
	if (arraySize < 0) {
		yyerror("Illegal negative array size %d", arraySize);
	}
	if (_bInHeader) {
		bytecode_defineVariable(_currentBytecodeBuffer, pszName, C_DEFINE_VARIABLE, sType, arraySize, 0);
	} else {
		bytecode_defineVariable(_currentBytecodeBuffer, pszName, C_DEFINE_LOCAL_VARIABLE, sType, arraySize, _localVariableIndex);
	}
	IDENTIFIER_CONTEXT* pContext = sym_getContext(_currentIdentifierContext, (char*)pszName);
	if (pContext && pContext != sym_getGlobalCompilerContext()) {
		yyerror("Redefinition of variable %s", pszName);
	}
	if (sType == VT_STRING && !tVal) {
		tVal = (intptr_t)"";
	}
	if (_bInHeader) {
		sym_createSymbol(_currentIdentifierContext, (char*) pszName, S_VARIABLE, sType, (GENERIC_DATA) {.val = tVal}, 0);
	} else {
		sym_createSymbol(_currentIdentifierContext, (char*) pszName, S_LOCAL_VARIABLE, sType, (GENERIC_DATA) {.val = tVal}, _localVariableIndex++);
	}
}

static void destroy_annotations() {
	ANNOTATION *pAnnotation = _currentMethodAnnotations;
	while(pAnnotation) {
		ANNOTATION* pNext = pAnnotation->a_next;
		free((char*)pAnnotation->a_name);
		hashmap_destroyWithKeysAndValues(pAnnotation->a_values);
		free(pAnnotation);
		pAnnotation = pNext;
	}
	_currentMethodAnnotations = 0;
}

static void start_annotation(const char* pszName) {
	ANNOTATION* pNew = ll_append(&_currentMethodAnnotations, sizeof *pNew);
	pNew->a_name = pszName;
	pNew->a_values = hashmap_create(7, 0 , 0);
}

static void parser_defineAnnotationValue(char* pszKey, char* pszValue) {
	ANNOTATION* pAnnotation = _currentMethodAnnotations;
	while(pAnnotation && pAnnotation->a_next) {
		pAnnotation = pAnnotation->a_next;
	}
	if (pAnnotation) {
		hashmap_put(pAnnotation->a_values, (intptr_t)pszKey, (intptr_t)pszValue);
	} else {
		free(pszKey);
		free(pszValue);
	}
}

static void parser_newForeachCursor() {
	YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex++});
}

static void parser_processNativeMethodAnnotations(char** pszLib, char** pszMacroCName, char** pszSignature) {
	*pszLib = 0;
	*pszMacroCName = 0;
	*pszSignature = 0;
	ANNOTATION* pAnnotation = _currentMethodAnnotations;
	while(pAnnotation) {
		if (strcmp("@Method", pAnnotation->a_name) == 0) {
			*pszLib = (char*)hashmap_get(pAnnotation->a_values, (intptr_t)"lib");
			*pszMacroCName = (char*)hashmap_get(pAnnotation->a_values, (intptr_t)"name");
			*pszSignature = (char*)hashmap_get(pAnnotation->a_values, (intptr_t)"signature");
		}
		pAnnotation = pAnnotation->a_next;
	}
}

static PKS_VALUE_TYPE parser_pushFieldIndexWithError(PKS_VALUE_TYPE t, const char* pszFieldName) {
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
				init_header();
			}
			namespace prerequisites global_declarations header_delimiter macro_definitions {
				finalize_header();
			}

header_delimiter:
			| '!'

macro_definitions: 
			| element_with_annotations macro_definitions 

element_with_annotations:
			annotations macro_or_native_function

macro_or_native_function:
			native_function_definition ';' 
			| macro_definition

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
					S_CONSTANT, $4.v.type, $4.v.data, 0);
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
			T_IDENT {
				$$.ident = $1.ident; 
				$$.ident.heapIndex = _localVariableIndex;
				$$.ident.isLocalVar = !_bInHeader;
			}
			| T_NUM {
				$$.ident = $1.ident;
				$$.ident.s = "#dummy";
				$$.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
			| T_STRING {
				$$.ident = $1.ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}

variable_reference:	
			T_VARIABLE	{	$$.ident = $1.ident; }
			| T_IDENT	{   
				yyerror("Using undeclared variable %s", $1.ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, $1.ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
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
				$$.ident.arraySize = 0;
				$$.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(VT_NIL);
				}
			}
			| 
			scope type_name {
				$$.ident.type = $2.ident.type;
				$$.ident.scope = $1.ident.scope;
				$$.ident.arraySize = $2.ident.arraySize;
				if (_bInNativeDefinition) {
					parser_startNativeMethod($2.ident.type);
				}
			}

macrostart: macro_type macro_declaration '(' argument_list ')' optional_description 
			{	$$.ident = $2.ident;
				$$.ident.scope = $1.ident.scope;
				$$.ident.type = $1.ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
			
global_var: scope var_decl

var_decl:	type_name variable_identifier assignment_expression {
				parser_defineVariable($2.ident.s, $1.ident.type, 0, $1.ident.arraySize);
				parser_emitAssignment(&$2.ident);
				if ($2.ident.stringIsAlloced) {
					free($2.ident.s);
				}
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});
			} | type_name variable_identifier {
				parser_defineVariable($2.ident.s, $1.ident.type, 0, $1.ident.arraySize);
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

argument_list:
			| non_empty_arguments

non_empty_arguments:
			argument_declaration
			|
			non_empty_arguments ',' argument_declaration

parameter_list:
			| non_empty_parameters

non_empty_parameters: parameter
			| non_empty_parameters ',' parameter
			| error {
				yyerror("illegal parameters for method call");
			}

argument_declaration: type_name variable_identifier {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam($1.ident.type, $2.ident.s);
				} else {
					sym_createSymbol(_currentIdentifierContext, $2.ident.s, S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam}, _localVariableIndex);
					bytecode_defineVariable(_currentBytecodeBuffer, $2.ident.s,C_DEFINE_PARAMETER, $1.ident.type,_nparam, _localVariableIndex++);
				}
				free($2.ident.s);
				_nparam++;
			} | T_ELLIPSIS {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam($1.ident.type, "...");
				}
			}

annotations:
			| annotation annotations

annotation: T_ANNOTATION {
				start_annotation($1.ident.s);
			} '(' annotation_values ')' 

annotation_value: T_IDENT T_ASSIGN T_STRING {
				parser_defineAnnotationValue($1.ident.s, $3.ident.s);
			} | T_IDENT {
				parser_defineAnnotationValue($1.ident.s, _strdup("true"));
			} | T_STRING {
				parser_defineAnnotationValue(_strdup("value"), $1.ident.s);
			}

annotation_values: 
				| annotation_value
				| annotation_value ',' annotation_values

native_function_definition: 
				T_NATIVE {
					init_native();
				} macrostart {
					char* pszLib;
					char* pszMacroCName;
					char* pszSignature;
					parser_processNativeMethodAnnotations(&pszLib, &pszMacroCName, &pszSignature);
					char* pszArgs = stringbuf_getString(_nativeMethodArgs);
					if (!pszSignature) {
						pszSignature = stringbuf_getString(_nativeMethodSignature);
					}
					if (!pszMacroCName) {
						pszMacroCName = $3.ident.s;
					}
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszMacroCName});
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=$3.ident.s});
					if (!pszLib) {
						pszLib = "PKSEDIT";
					}
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszLib});
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszSignature});
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=_yyCurrentComment});
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszArgs});
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
							C_0FUNC, (GENERIC_DATA){ FUNC_RegisterNative}, 6);
					// register right away as well, so we can use it.
					function_registerNativeFunction(pszMacroCName, $3.ident.s, pszLib, pszSignature, _yyCurrentComment, pszArgs);
					free($3.ident.s);
					finalize_native();
				} 

macro_definition: { init_macroDefinition(); } 
				macrostart block {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName($2.ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",$2.ident.s);
				bytecode_optimizeInstructions(_currentBytecodeBuffer);
				macro_processAnnotations(&_macroParam, _currentMethodAnnotations);
				_macroParam.mp_name = $2.ident.s;
				_macroParam.mp_comment = _yyCurrentComment;
				_macroParam.mp_returnType = $2.ident.type;
				_macroParam.mp_numberOfLocalVariables = _localVariableIndex;
				_macroParam.mp_bytecodeLength = _currentBytecodeBuffer->bb_current - _currentBytecodeBuffer->bb_start;
				_macroParam.mp_buffer = _currentBytecodeBuffer->bb_start;
				_macroParam.mp_scope = $2.ident.scope;
				_macroParam.mp_sourceFile = yy_getCurrentInputFilename();
				_macroParam.mp_namespaceIdx = _currentNamespaceIndex;
				_compilerConfiguration->cb_insertNewMacro(&_macroParam);
				memset(&_macroParam, 0, sizeof _macroParam);
				freeitem(&$2.ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			} 

block:		'{' var_decls stmntlist closing_brace

var_decls:
			| var_decls var_decl ';' 

statement: ';'
			| call_expression ';' { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
			| assignment ';' { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
			| if_expression { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
			| while { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
			| for_loop_expression { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
			| switch_expression { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
			| break
			| continue
			| return_expression ';'
			| goto_expression ';'
			| block { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
			| increment_expression ';' { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
			| label
			| var_decl {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
			| error ';' {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}

assignment: variable_reference assignment_expression {
				parser_emitAssignment(&$1.ident);
			}
			|
			assignment_target '.' T_IDENT {
				parser_pushFieldIndexWithError($1.ident.type, $3.ident.s);
			} assignment_expression {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			} 
			|
			assignment_target '[' binary_expression ']' assignment_expression {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			} 
			| shorthand_assignment

shorthand_assignment:
			variable_reference {
				parser_emitPushVariable(&$1.ident);
			} shorthand_assignment_operator expression {
				YY_EMIT(C_BINOP, (GENERIC_DATA){$3.binop});
				parser_emitAssignment(&$1.ident);
			}

assignment_target: variable_reference {
				parser_emitPushVariable(&$1.ident);
			}

shorthand_assignment_operator: 
			T_SH_ASSIGN_MULT { $$.binop = BIN_MUL; }
			| T_SH_ASSIGN_DIV { $$.binop = BIN_DIV; }
			| T_SH_ASSIGN_MOD { $$.binop = BIN_MOD; }
			| T_SH_ASSIGN_PLUS { $$.binop = BIN_ADD; }
			| T_SH_ASSIGN_MINUS { $$.binop = BIN_SUB; }

assignment_expression:	T_ASSIGN expression	{ $$.v = $2.v;	}

expression: binary_expression { $$.v = $1.v; }
			| range_expression

range_expression:
			| binary_expression T_DOTDOT binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); } range_increment

range_increment:
			| T_DOTDOT binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }

minusminus_plusplus:
			T_MINUSMINUS { $$.num = -1; }
			| T_PLUSPLUS { $$.num = 1; }

increment_expression: 
			variable_reference minusminus_plusplus { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, $1.ident.s, $1.ident.heapIndex, $1.ident.isLocalVar, (int)$2.num);
			}

simple_term: 	value { $$.v = $1.v; }
			| call_expression 
			| constructor_expression

constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')' {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){$2.ident.type});
			}

unary_expression:  '!' binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
			| '~' binary_expression { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
			| '+' binary_expression { $$.v = $2.v; }
			| '-' variable_reference { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &$2.v, -1); }

binary_expression:
			simple_term { $$.ident.type = $1.ident.type;}
			| type_cast simple_term	{
				$$.ident.type = $1.ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, $$.binop);
			}
			|
			binary_expression '.' T_IDENT {
				$$.ident.type = parser_pushFieldIndexWithError($1.ident.type, $3.ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
			| unary_expression
			| binary_expression '[' expression ']' { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
			| binary_expression '~' binary_expression   { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
			| binary_expression T_NMATCH binary_expression   { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
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
			| binary_expression T_AND binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
			| binary_expression T_OR binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
			| binary_expression '<' binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
			| binary_expression '>' binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
			| binary_expression T_LE binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
			| binary_expression T_GE binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
			| binary_expression T_EQ binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
			| binary_expression T_NE binary_expression { $$.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
			| '(' binary_expression ')'	{ $$.v = $2.v; }
			| variable_reference assignment_expression {
				parser_emitAssignment(&$1.ident);
			}

/*----- conditions ------------------------------------------*/

condition:	'(' binary_expression ')'
			| '(' binary_expression {
				yyerror("Missing closing parenthesis )");
			}

value:		T_VARIABLE {
				parser_emitPushVariable(&$1.ident);
				$$.ident.type = $1.ident.type;
			}
			| string {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=$1.ident.s});
				$$.v.type = C_PUSH_STRING_LITERAL;
				free($1.ident.s);
			}
			| simple_literal {	$$.ident.type = $1.ident.type;	}
			| T_IDENT {
				yyerror("Undefined identifier %s", $1.ident.s);
				parser_emitPushVariable(&$1.ident);
				$$.ident.type = VT_STRING;  
			}

string:		T_STRING {	$$.ident = $1.ident; }
			| string T_STRING { 
				if (($$.ident.s = malloc(strlen($1.ident.s)+strlen($2.ident.s)+1)) != 0) {
					strcat(strcpy($$.ident.s,$1.ident.s),$2.ident.s);
					free($1.ident.s);
					free($2.ident.s);
				} else {
					yyerror("String too long: buffer overflow");
					$$.ident = $2.ident;
				}
			}

break:		T_BREAK opt_num ';'
			{	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)$2.num,BRA_ALWAYS); }

continue:	T_CONTINUE opt_num ';' {
				// in the case this is a continue in a for(int i;b;i++) loop, generate the i++ code before continuing.
				parser_flushBuffer(_breaklevel-(int)$2.num+1);
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)$2.num,BRA_ALWAYS); 
			}

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
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}

case_condition:
			integer_literal {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=$1.v.data.longValue});
			}
			| T_STRING {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=$1.ident.s});
			}
			| T_NUM T_DOTDOT T_NUM {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=$1.num, .range.r_end=$3.num});
			}

label:		T_IDENT ':' {
				bytecode_createBranchLabel(_currentBytecodeBuffer, $1.ident.s);
				freeitem(&$1.ident.s);
			}

goto_expression:
			T_GOTO T_IDENT {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction($2.ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&$2.ident.s);
			}

switch_expression:		
			T_SWITCH  '(' value ')' 
			{
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
			'{' case_clauses '}'
			{
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

for_loop_expression:
			T_FOR for_clause {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			} local_block {
				if ($2.ident.s) {
					sym_remove(_currentIdentifierContext, $2.ident.s);
					free($2.ident.s);
				}
				parser_flushBuffer(_breaklevel);
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

for_increment: increment_expression
			| shorthand_assignment

for_to_clause: '(' opt_for_initializer {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			} ';' binary_expression ';' {
				parser_switchToBuffer(_breaklevel+1);
			} for_increment {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			} ')' {
				$$.ident.s = $2.ident.s;
			}

opt_for_initializer:
			| type_name variable_identifier {
				parser_defineVariable($2.ident.s, $1.ident.type, (intptr_t)0, $1.ident.arraySize);
			} assignment_expression {
				$$.ident.s = $2.ident.s;
			}

for_clause:	for_to_clause {
				$$.ident.s = $1.ident.s;
			}
			| in_clause {
				$$.ident.s = $1.ident.s;
			}
			| error {
				$$.ident.s = 0;
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}

in_clause:	'(' type_name variable_identifier {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				parser_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable($3.ident.s, $2.ident.type, (intptr_t)0, $2.ident.arraySize);
			} ':' expression ')' {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
				$$.ident.s = $3.ident.s;
			}

while:		T_WHILE {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			} condition {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			} local_block {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}

local_block: 
		statement
		|
		'{' stmntlist closing_brace

closing_brace: '}'

if_expression:
			T_IF condition {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			} local_block {
				_iflevel--;
			} else_clause {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}

else_clause: {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
			| T_ELSE {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
			local_block

stmntlist:	
			| statement stmntlist

closing_paren: ')'

call_expression: T_FUNC '(' {
				// Native call
				_currentNativeMethodCalled = $1.funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				$$.funcp = $1.funcp;
			} parameter_list closing_paren {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction($1.funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
			| function_id_or_pointer '(' {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				$$.funcp = 0;
			} parameter_list closing_paren {
				if ($1.ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, $1.ident.operation,(GENERIC_DATA){.intValue = $1.ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, $1.ident.operation,(GENERIC_DATA){.string = $1.ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&$1.ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
			| array_constructor
			| map_constructor

array_constructor: '[' {
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				$$.funcp = &_functionTable[FUNC_CreateArray];
			} parameter_list ']' {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
					C_0FUNC,(GENERIC_DATA){FUNC_CreateArray}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
			}

map_constructor: '{' {
		_currentFunctionCallParamIndexP++;
		*_currentFunctionCallParamIndexP = 0;
		$$.funcp = &_functionTable[FUNC_CreateMap];
	} optional_map_associates '}' {
		_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
			C_0FUNC,(GENERIC_DATA){FUNC_CreateMap}, *_currentFunctionCallParamIndexP);
		_currentFunctionCallParamIndexP--;
	}

optional_map_associates:
			|
			map_associates

map_associates:
			map_associate
			|
			map_associates ',' map_associate

map_associate:
			T_STRING ':' { 
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=$1.ident.s});
				$$.v.type = C_PUSH_STRING_LITERAL;
				free($1.ident.s);
				(*_currentFunctionCallParamIndexP) += 2;
			} value

function_id_or_pointer:
			'*' variable_reference {
				$$.ident.operation =  $2.ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if ($2.ident.isLocalVar) {
					$$.ident.heapIndex = $2.ident.heapIndex;
				} else {
					$$.ident.s = _strdup($2.ident.s);
					$$.ident.stringIsAlloced = 1;
				}
			}
			|
			T_IDENT {
				$$.ident = $1.ident;
				$$.ident.operation = C_MACRO;
			}

parameter:	expression { $$.ident.type = $1.ident.type; (*_currentFunctionCallParamIndexP)++; }
			| T_ELLIPSIS expression { 
				$$.ident.type = $2.ident.type; (*_currentFunctionCallParamIndexP)++; 
				YY_EMIT(C_SPREAD, (GENERIC_DATA){1});
			}

type_cast:	'(' type_name ')'	{	
				$$.ident.type = $2.ident.type;
				$$.ident.arraySize = $2.ident.arraySize;
			}

type_name:   T_TYPE_IDENTIFIER {
				$$.ident.arraySize = 0;
			}
			| T_TYPE_IDENTIFIER array_size	{	
				$$.ident.type = VT_OBJECT_ARRAY;
				$$.ident.arraySize = $2.ident.arraySize;
			}

array_size: '[' ']'
			| '[' T_NUM ']' {
				$$.ident.arraySize = (int)$2.num;
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
			| '-' T_NUM {
				$$.ident.type = VT_NUMBER;
				$$.v.type = C_PUSH_LONG_LITERAL; 
				$$.v.data.longValue  = -$2.num;
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
				$$.ident.type = VT_NUMBER;
			}
			| float_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.ident.type = VT_FLOAT;
			}
			| character_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.ident.type = VT_CHAR;
			}
			| boolean_literal { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, $1.v.data);
				} else {
					$$.v = $1.v;
				}
				$$.ident.type = VT_BOOLEAN;
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


static void init_bytecodeBuffer(int nIndex) {
	parser_switchToBuffer(nIndex);
	_currentBytecodeBuffer->bb_current = _currentBytecodeBuffer->bb_start;
}

static void destroy_bytecodeBuffer(int nStart) {
	for (int i = nStart; i < DIM(_buffers); i++) {
		if (_buffers[i].bb_start) {
			free(_buffers[i].bb_start);
			_buffers[i].bb_start = 0;
			_buffers[i].bb_end = 0;
		}
	}
}

static void init_header() {
	init_bytecodeBuffer(-1);
}

static void init_native() {
	parser_switchToBuffer(-1);
	_bInNativeDefinition = 1;
}

static void finalize_native() {
	_bInNativeDefinition = 0;
	destroy_annotations();
	stringbuf_destroy(_nativeMethodArgs);
	stringbuf_destroy(_nativeMethodSignature);
	_nativeMethodArgs = 0;
	_nativeMethodSignature = 0;
}

static void finalize_header() {
	parser_switchToBuffer(-1);
	macro_defineNamespaceInitializer(_currentNamespaceIndex, _currentBytecodeBuffer->bb_start, _currentBytecodeBuffer->bb_current - _currentBytecodeBuffer->bb_start);
	destroy_bytecodeBuffer(0);
}

/*---------------------------------*/
/* init_macroDefinition()				*/
/*---------------------------------*/
static void init_macroDefinition(void)
{
	_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	_localVariableIndex = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext, 0);
	init_bytecodeBuffer(0);
}

/*---------------------------------*/
/* finalize_macroDefinition()					*/
/*---------------------------------*/
static void finalize_macroDefinition(void) {
	destroy_annotations();
	destroy_bytecodeBuffer(1);
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

