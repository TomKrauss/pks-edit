/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "Parser.y"

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
#line 43 "Parser.y"


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


#line 365 "parser.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_NAMESPACE = 3,                /* T_NAMESPACE  */
  YYSYMBOL_T_REQUIRE = 4,                  /* T_REQUIRE  */
  YYSYMBOL_T_FOR = 5,                      /* T_FOR  */
  YYSYMBOL_T_ANNOTATION = 6,               /* T_ANNOTATION  */
  YYSYMBOL_T_NATIVE = 7,                   /* T_NATIVE  */
  YYSYMBOL_T_NEW = 8,                      /* T_NEW  */
  YYSYMBOL_T_STATIC = 9,                   /* T_STATIC  */
  YYSYMBOL_T_VOID = 10,                    /* T_VOID  */
  YYSYMBOL_T_TYPE_IDENTIFIER = 11,         /* T_TYPE_IDENTIFIER  */
  YYSYMBOL_T_ELLIPSIS = 12,                /* T_ELLIPSIS  */
  YYSYMBOL_T_IDENT = 13,                   /* T_IDENT  */
  YYSYMBOL_T_NUM = 14,                     /* T_NUM  */
  YYSYMBOL_T_FLOATING_POINT_NUMBER = 15,   /* T_FLOATING_POINT_NUMBER  */
  YYSYMBOL_T_STRING = 16,                  /* T_STRING  */
  YYSYMBOL_T_CHARACTER = 17,               /* T_CHARACTER  */
  YYSYMBOL_T_CONST = 18,                   /* T_CONST  */
  YYSYMBOL_T_TRUE = 19,                    /* T_TRUE  */
  YYSYMBOL_T_FALSE = 20,                   /* T_FALSE  */
  YYSYMBOL_T_FUNC = 21,                    /* T_FUNC  */
  YYSYMBOL_T_DOTDOT = 22,                  /* T_DOTDOT  */
  YYSYMBOL_T_PLUSPLUS = 23,                /* T_PLUSPLUS  */
  YYSYMBOL_T_MINUSMINUS = 24,              /* T_MINUSMINUS  */
  YYSYMBOL_T_IF = 25,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 26,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 27,                   /* T_WHILE  */
  YYSYMBOL_T_GOTO = 28,                    /* T_GOTO  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 29,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 30,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 31,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 32,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 33,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_SWITCH = 34,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 35,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 36,                 /* T_DEFAULT  */
  YYSYMBOL_T_BREAK = 37,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 38,                /* T_CONTINUE  */
  YYSYMBOL_T_RETURN = 39,                  /* T_RETURN  */
  YYSYMBOL_T_VARIABLE = 40,                /* T_VARIABLE  */
  YYSYMBOL_T_NMATCH = 41,                  /* T_NMATCH  */
  YYSYMBOL_T_ASSIGN = 42,                  /* T_ASSIGN  */
  YYSYMBOL_T_NE = 43,                      /* T_NE  */
  YYSYMBOL_T_EQ = 44,                      /* T_EQ  */
  YYSYMBOL_T_SHIFT_LEFT = 45,              /* T_SHIFT_LEFT  */
  YYSYMBOL_T_SHIFT_RIGHT = 46,             /* T_SHIFT_RIGHT  */
  YYSYMBOL_T_OR = 47,                      /* T_OR  */
  YYSYMBOL_T_AND = 48,                     /* T_AND  */
  YYSYMBOL_T_LE = 49,                      /* T_LE  */
  YYSYMBOL_T_GE = 50,                      /* T_GE  */
  YYSYMBOL_T_POWER_TO = 51,                /* T_POWER_TO  */
  YYSYMBOL_52_ = 52,                       /* '&'  */
  YYSYMBOL_53_ = 53,                       /* '|'  */
  YYSYMBOL_54_ = 54,                       /* '^'  */
  YYSYMBOL_55_ = 55,                       /* '!'  */
  YYSYMBOL_56_ = 56,                       /* '~'  */
  YYSYMBOL_57_ = 57,                       /* '<'  */
  YYSYMBOL_58_ = 58,                       /* '>'  */
  YYSYMBOL_59_ = 59,                       /* '+'  */
  YYSYMBOL_60_ = 60,                       /* '-'  */
  YYSYMBOL_61_ = 61,                       /* '*'  */
  YYSYMBOL_62_ = 62,                       /* '/'  */
  YYSYMBOL_63_ = 63,                       /* '%'  */
  YYSYMBOL_64_ = 64,                       /* '['  */
  YYSYMBOL_65_ = 65,                       /* '.'  */
  YYSYMBOL_66_ = 66,                       /* ';'  */
  YYSYMBOL_67_ = 67,                       /* ','  */
  YYSYMBOL_68_ = 68,                       /* '('  */
  YYSYMBOL_69_ = 69,                       /* ')'  */
  YYSYMBOL_70_ = 70,                       /* '{'  */
  YYSYMBOL_71_ = 71,                       /* ']'  */
  YYSYMBOL_72_ = 72,                       /* ':'  */
  YYSYMBOL_73_ = 73,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 74,                  /* $accept  */
  YYSYMBOL_file_structure = 75,            /* file_structure  */
  YYSYMBOL_76_1 = 76,                      /* $@1  */
  YYSYMBOL_header_delimiter = 77,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 78,         /* macro_definitions  */
  YYSYMBOL_element_with_annotations = 79,  /* element_with_annotations  */
  YYSYMBOL_macro_or_native_function = 80,  /* macro_or_native_function  */
  YYSYMBOL_namespace = 81,                 /* namespace  */
  YYSYMBOL_prerequisites = 82,             /* prerequisites  */
  YYSYMBOL_require = 83,                   /* require  */
  YYSYMBOL_global_declarations = 84,       /* global_declarations  */
  YYSYMBOL_scope = 85,                     /* scope  */
  YYSYMBOL_constants = 86,                 /* constants  */
  YYSYMBOL_constant_list = 87,             /* constant_list  */
  YYSYMBOL_constdef = 88,                  /* constdef  */
  YYSYMBOL_89_2 = 89,                      /* $@2  */
  YYSYMBOL_90_3 = 90,                      /* $@3  */
  YYSYMBOL_constant_literal = 91,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 92,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 93,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 94,         /* macro_declaration  */
  YYSYMBOL_macro_type = 95,                /* macro_type  */
  YYSYMBOL_macrostart = 96,                /* macrostart  */
  YYSYMBOL_global_var = 97,                /* global_var  */
  YYSYMBOL_var_decl = 98,                  /* var_decl  */
  YYSYMBOL_optional_description = 99,      /* optional_description  */
  YYSYMBOL_argument_list = 100,            /* argument_list  */
  YYSYMBOL_non_empty_arguments = 101,      /* non_empty_arguments  */
  YYSYMBOL_parameter_list = 102,           /* parameter_list  */
  YYSYMBOL_non_empty_parameters = 103,     /* non_empty_parameters  */
  YYSYMBOL_argument_declaration = 104,     /* argument_declaration  */
  YYSYMBOL_annotations = 105,              /* annotations  */
  YYSYMBOL_annotation = 106,               /* annotation  */
  YYSYMBOL_107_4 = 107,                    /* $@4  */
  YYSYMBOL_annotation_value = 108,         /* annotation_value  */
  YYSYMBOL_annotation_values = 109,        /* annotation_values  */
  YYSYMBOL_native_function_definition = 110, /* native_function_definition  */
  YYSYMBOL_111_5 = 111,                    /* $@5  */
  YYSYMBOL_macro_definition = 112,         /* macro_definition  */
  YYSYMBOL_113_6 = 113,                    /* $@6  */
  YYSYMBOL_block = 114,                    /* block  */
  YYSYMBOL_var_decls = 115,                /* var_decls  */
  YYSYMBOL_statement = 116,                /* statement  */
  YYSYMBOL_assignment = 117,               /* assignment  */
  YYSYMBOL_118_7 = 118,                    /* $@7  */
  YYSYMBOL_shorthand_assignment = 119,     /* shorthand_assignment  */
  YYSYMBOL_120_8 = 120,                    /* $@8  */
  YYSYMBOL_assignment_target = 121,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 122, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 123,    /* assignment_expression  */
  YYSYMBOL_expression = 124,               /* expression  */
  YYSYMBOL_range_expression = 125,         /* range_expression  */
  YYSYMBOL_126_9 = 126,                    /* $@9  */
  YYSYMBOL_range_increment = 127,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 128,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 129,     /* increment_expression  */
  YYSYMBOL_simple_term = 130,              /* simple_term  */
  YYSYMBOL_constructor_expression = 131,   /* constructor_expression  */
  YYSYMBOL_unary_expression = 132,         /* unary_expression  */
  YYSYMBOL_binary_expression = 133,        /* binary_expression  */
  YYSYMBOL_condition = 134,                /* condition  */
  YYSYMBOL_value = 135,                    /* value  */
  YYSYMBOL_string = 136,                   /* string  */
  YYSYMBOL_break = 137,                    /* break  */
  YYSYMBOL_continue = 138,                 /* continue  */
  YYSYMBOL_return_expression = 139,        /* return_expression  */
  YYSYMBOL_optional_bterm = 140,           /* optional_bterm  */
  YYSYMBOL_opt_num = 141,                  /* opt_num  */
  YYSYMBOL_case_clauses = 142,             /* case_clauses  */
  YYSYMBOL_case_clause = 143,              /* case_clause  */
  YYSYMBOL_case_selector = 144,            /* case_selector  */
  YYSYMBOL_case_condition = 145,           /* case_condition  */
  YYSYMBOL_label = 146,                    /* label  */
  YYSYMBOL_goto_expression = 147,          /* goto_expression  */
  YYSYMBOL_switch_expression = 148,        /* switch_expression  */
  YYSYMBOL_149_10 = 149,                   /* $@10  */
  YYSYMBOL_for_loop_expression = 150,      /* for_loop_expression  */
  YYSYMBOL_151_11 = 151,                   /* $@11  */
  YYSYMBOL_for_increment = 152,            /* for_increment  */
  YYSYMBOL_for_to_clause = 153,            /* for_to_clause  */
  YYSYMBOL_154_12 = 154,                   /* $@12  */
  YYSYMBOL_155_13 = 155,                   /* $@13  */
  YYSYMBOL_156_14 = 156,                   /* $@14  */
  YYSYMBOL_opt_for_initializer = 157,      /* opt_for_initializer  */
  YYSYMBOL_158_15 = 158,                   /* $@15  */
  YYSYMBOL_for_clause = 159,               /* for_clause  */
  YYSYMBOL_in_clause = 160,                /* in_clause  */
  YYSYMBOL_161_16 = 161,                   /* $@16  */
  YYSYMBOL_while = 162,                    /* while  */
  YYSYMBOL_163_17 = 163,                   /* $@17  */
  YYSYMBOL_164_18 = 164,                   /* $@18  */
  YYSYMBOL_local_block = 165,              /* local_block  */
  YYSYMBOL_closing_brace = 166,            /* closing_brace  */
  YYSYMBOL_if_expression = 167,            /* if_expression  */
  YYSYMBOL_168_19 = 168,                   /* $@19  */
  YYSYMBOL_169_20 = 169,                   /* $@20  */
  YYSYMBOL_else_clause = 170,              /* else_clause  */
  YYSYMBOL_171_21 = 171,                   /* $@21  */
  YYSYMBOL_stmntlist = 172,                /* stmntlist  */
  YYSYMBOL_closing_paren = 173,            /* closing_paren  */
  YYSYMBOL_call_expression = 174,          /* call_expression  */
  YYSYMBOL_175_22 = 175,                   /* @22  */
  YYSYMBOL_176_23 = 176,                   /* @23  */
  YYSYMBOL_array_constructor = 177,        /* array_constructor  */
  YYSYMBOL_178_24 = 178,                   /* @24  */
  YYSYMBOL_map_constructor = 179,          /* map_constructor  */
  YYSYMBOL_180_25 = 180,                   /* @25  */
  YYSYMBOL_optional_map_associates = 181,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 182,           /* map_associates  */
  YYSYMBOL_map_associate = 183,            /* map_associate  */
  YYSYMBOL_184_26 = 184,                   /* @26  */
  YYSYMBOL_function_id_or_pointer = 185,   /* function_id_or_pointer  */
  YYSYMBOL_parameter = 186,                /* parameter  */
  YYSYMBOL_type_cast = 187,                /* type_cast  */
  YYSYMBOL_type_name = 188,                /* type_name  */
  YYSYMBOL_array_size = 189,               /* array_size  */
  YYSYMBOL_boolean_literal = 190,          /* boolean_literal  */
  YYSYMBOL_float_literal = 191,            /* float_literal  */
  YYSYMBOL_integer_literal = 192,          /* integer_literal  */
  YYSYMBOL_character_literal = 193,        /* character_literal  */
  YYSYMBOL_simple_literal = 194            /* simple_literal  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   819

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  121
/* YYNRULES -- Number of rules.  */
#define YYNRULES  240
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  378

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   306


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    55,     2,     2,     2,    63,    52,     2,
      68,    69,    61,    59,    67,    60,    65,    62,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    72,    66,
      57,     2,    58,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,     2,    71,    54,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    70,    53,    73,    56,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   322,   322,   322,   329,   330,   332,   333,   336,   339,
     340,   342,   345,   350,   351,   353,   358,   359,   360,   362,
     363,   367,   370,   371,   373,   373,   373,   387,   390,   396,
     401,   407,   413,   414,   424,   427,   433,   442,   451,   458,
     460,   467,   474,   476,   481,   482,   485,   487,   489,   490,
     492,   493,   494,   498,   507,   513,   514,   516,   516,   520,
     522,   524,   528,   529,   530,   533,   533,   564,   564,   590,
     592,   593,   595,   596,   597,   598,   599,   600,   601,   602,
     603,   604,   605,   606,   607,   608,   609,   612,   617,   621,
     621,   627,   630,   633,   633,   640,   645,   646,   647,   648,
     649,   651,   653,   654,   656,   657,   657,   659,   660,   663,
     664,   667,   671,   672,   673,   675,   679,   680,   681,   682,
     685,   686,   691,   695,   696,   697,   698,   699,   700,   701,
     702,   703,   704,   705,   706,   707,   708,   709,   710,   711,
     712,   713,   714,   715,   716,   717,   718,   719,   725,   726,
     730,   734,   739,   740,   746,   747,   758,   761,   768,   772,
     773,   775,   776,   778,   779,   781,   783,   785,   790,   793,
     796,   800,   806,   813,   812,   829,   829,   845,   846,   848,
     850,   852,   848,   859,   860,   860,   866,   869,   872,   878,
     878,   891,   893,   891,   905,   907,   909,   912,   915,   912,
     922,   926,   926,   933,   934,   936,   938,   938,   949,   949,
     963,   964,   966,   966,   976,   976,   986,   988,   991,   993,
     996,   996,  1004,  1014,  1019,  1020,  1025,  1030,  1033,  1038,
    1039,  1043,  1046,  1051,  1056,  1060,  1067,  1072,  1080,  1088,
    1096
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  static const char *const yy_sname[] =
  {
  "end of file", "error", "invalid token", "T_NAMESPACE", "T_REQUIRE",
  "T_FOR", "T_ANNOTATION", "T_NATIVE", "T_NEW", "T_STATIC", "T_VOID",
  "T_TYPE_IDENTIFIER", "T_ELLIPSIS", "T_IDENT", "T_NUM",
  "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER", "T_CONST",
  "T_TRUE", "T_FALSE", "T_FUNC", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS",
  "T_IF", "T_ELSE", "T_WHILE", "T_GOTO", "T_SH_ASSIGN_PLUS",
  "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV",
  "T_SH_ASSIGN_MOD", "T_SWITCH", "T_CASE", "T_DEFAULT", "T_BREAK",
  "T_CONTINUE", "T_RETURN", "T_VARIABLE", "T_NMATCH", "T_ASSIGN", "T_NE",
  "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR", "T_AND", "T_LE", "T_GE",
  "T_POWER_TO", "'&'", "'|'", "'^'", "'!'", "'~'", "'<'", "'>'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "'['", "'.'", "';'", "','", "'('", "')'",
  "'{'", "']'", "':'", "'}'", "$accept", "file_structure", "$@1",
  "header_delimiter", "macro_definitions", "element_with_annotations",
  "macro_or_native_function", "namespace", "prerequisites", "require",
  "global_declarations", "scope", "constants", "constant_list", "constdef",
  "$@2", "$@3", "constant_literal", "variable_identifier",
  "variable_reference", "macro_declaration", "macro_type", "macrostart",
  "global_var", "var_decl", "optional_description", "argument_list",
  "non_empty_arguments", "parameter_list", "non_empty_parameters",
  "argument_declaration", "annotations", "annotation", "$@4",
  "annotation_value", "annotation_values", "native_function_definition",
  "$@5", "macro_definition", "$@6", "block", "var_decls", "statement",
  "assignment", "$@7", "shorthand_assignment", "$@8", "assignment_target",
  "shorthand_assignment_operator", "assignment_expression", "expression",
  "range_expression", "$@9", "range_increment", "minusminus_plusplus",
  "increment_expression", "simple_term", "constructor_expression",
  "unary_expression", "binary_expression", "condition", "value", "string",
  "break", "continue", "return_expression", "optional_bterm", "opt_num",
  "case_clauses", "case_clause", "case_selector", "case_condition",
  "label", "goto_expression", "switch_expression", "$@10",
  "for_loop_expression", "$@11", "for_increment", "for_to_clause", "$@12",
  "$@13", "$@14", "opt_for_initializer", "$@15", "for_clause", "in_clause",
  "$@16", "while", "$@17", "$@18", "local_block", "closing_brace",
  "if_expression", "$@19", "$@20", "else_clause", "$@21", "stmntlist",
  "closing_paren", "call_expression", "@22", "@23", "array_constructor",
  "@24", "map_constructor", "@25", "optional_map_associates",
  "map_associates", "map_associate", "@26", "function_id_or_pointer",
  "parameter", "type_cast", "type_name", "array_size", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-253)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-224)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -253,    15,    39,  -253,    68,  -253,    40,   104,  -253,   102,
    -253,    31,    75,  -253,  -253,  -253,    98,   114,    88,    89,
    -253,    94,  -253,    96,  -253,  -253,    98,   152,   157,   100,
    -253,    96,  -253,  -253,  -253,  -253,  -253,  -253,   132,   109,
    -253,  -253,  -253,   112,  -253,   163,  -253,     2,  -253,   137,
    -253,    19,     4,   163,  -253,    21,    16,   113,   111,  -253,
     486,  -253,  -253,  -253,  -253,  -253,  -253,  -253,   173,  -253,
    -253,  -253,  -253,  -253,  -253,   146,  -253,   129,   128,  -253,
    -253,  -253,  -253,  -253,   131,  -253,  -253,  -253,   187,    43,
    -253,   133,   158,   486,   486,   486,     5,    67,  -253,   422,
    -253,   137,  -253,  -253,  -253,  -253,  -253,   545,  -253,   189,
    -253,  -253,  -253,   135,   517,  -253,  -253,  -253,   190,     4,
    -253,    79,   270,   139,  -253,   729,   754,   106,  -253,  -253,
    -253,  -253,   304,   599,   140,   192,  -253,   486,   486,   486,
     486,   486,   486,   486,   486,   486,   486,   486,   486,   486,
     486,   486,   486,   486,   486,   486,   486,   486,   486,   486,
     198,  -253,  -253,   144,  -253,  -253,  -253,  -253,  -253,   145,
     149,  -253,    96,   147,    10,    20,   150,  -253,   207,   154,
     210,   210,   486,  -253,   209,    59,   160,  -253,    92,   165,
    -253,    51,   170,  -253,  -253,   172,  -253,   176,  -253,  -253,
    -253,  -253,   175,   180,   182,   337,  -253,   486,   181,   193,
    -253,  -253,  -253,  -253,   185,   186,   194,  -253,   679,   754,
     754,   754,   -38,   -38,   704,   704,   130,   130,    57,   729,
     729,   729,   754,   130,   130,   106,   106,    57,    57,    57,
     191,  -253,   337,   250,    79,  -253,  -253,  -253,   114,  -253,
    -253,  -253,  -253,   486,  -253,   150,  -253,   120,  -253,   201,
     202,   679,  -253,  -253,  -253,   119,  -253,  -253,  -253,  -253,
    -253,  -253,   486,   256,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,   203,  -253,  -253,   453,  -253,  -253,   192,   248,  -253,
     203,  -253,  -253,  -253,  -253,    96,   389,   626,   389,  -253,
    -253,   204,  -253,  -253,  -253,  -253,  -253,  -253,  -253,   486,
     570,  -253,  -253,  -253,  -253,   120,  -253,   486,  -253,  -253,
     208,   206,   216,  -253,  -253,  -253,  -253,   389,  -253,  -253,
     137,   137,  -253,  -253,   486,   137,   212,   175,   253,  -253,
     217,  -253,  -253,   653,  -253,   486,  -253,  -253,  -253,   108,
    -253,   219,   389,    27,   218,   220,   108,    92,    67,  -253,
    -253,   254,  -253,   222,  -253,  -253,  -253,  -253,  -253,   122,
    -253,  -253,  -253,   278,  -253,   227,  -253,  -253
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,    11,     1,     0,    13,     0,    16,    12,     0,
      14,     4,     0,    20,    24,     5,    55,     0,     0,     0,
      15,    21,    22,     0,    57,     3,    55,    67,    55,   227,
      39,     0,    17,    18,    24,    29,    30,    31,     0,     0,
       7,    65,     8,     0,    10,    19,    56,     0,   228,    41,
      23,     0,    62,    19,     9,     0,     0,     0,     0,   229,
     104,    40,   234,   233,    28,   236,   231,   232,     0,    25,
     240,   238,   237,   239,    27,    60,    61,    63,     0,    66,
      36,    37,    34,    35,     0,    70,    68,   230,     0,   153,
     154,     0,   150,     0,     0,     0,     0,     0,   212,     0,
     214,     0,   101,   103,   120,   114,   123,   102,   112,   151,
     113,   210,   211,     0,     0,   152,   235,    26,     0,    62,
      58,    44,     0,     0,   206,   116,   117,   118,    33,    32,
     119,   222,     0,     0,     0,   216,   147,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   104,
       0,   155,   208,   153,   150,   121,    59,    64,    54,     0,
      45,    46,     0,     0,     0,    33,     0,   191,     0,     0,
     161,   161,   159,    72,    70,    93,    86,    83,     0,     0,
      92,     0,     0,    79,    80,     0,    85,     0,    78,    77,
      76,    75,     0,     0,     0,     0,    52,   104,     0,    49,
     224,    50,   146,   226,     0,     0,   217,   218,   105,   126,
     145,   144,   135,   136,   139,   138,   142,   143,   137,   127,
     128,   130,   125,   140,   141,   129,   131,   132,   133,   134,
       0,   122,     0,    42,     0,    53,    87,   188,   183,   186,
     175,   187,   171,     0,   197,     0,   172,     0,   162,     0,
       0,   160,   158,   110,   109,     0,    88,   111,    71,    86,
     204,    74,     0,     0,    84,    81,    82,   196,    69,    73,
     115,     0,   225,   213,   104,   220,   215,     0,   107,   124,
       0,    43,    38,    47,   179,     0,     0,   149,     0,   192,
     153,     0,   156,   157,    99,   100,    96,    97,    98,   104,
       0,    89,   205,   207,    51,     0,   219,     0,   106,   209,
       0,   184,     0,   194,   176,   148,   198,     0,   173,    94,
       0,     0,   221,   108,     0,     0,     0,     0,   200,   193,
       0,    91,    90,     0,   185,   104,   195,   201,   199,   163,
     180,     0,     0,     0,     0,     0,   163,     0,     0,   190,
     202,   234,   169,     0,   168,   167,   174,   164,   165,    93,
     178,   177,   181,     0,   166,     0,   170,   182
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -253,  -253,  -253,  -253,   273,  -253,  -253,  -253,  -253,  -253,
    -253,   289,  -253,  -253,   267,  -253,  -253,  -253,   -30,   -94,
    -253,  -253,   249,  -253,    -8,  -253,  -253,  -253,  -158,  -253,
      62,   275,  -253,  -253,  -253,   195,  -253,  -253,  -253,  -253,
     258,  -253,  -252,  -253,  -253,   -47,  -253,  -253,  -253,   -96,
     -60,  -253,  -253,  -253,  -253,   -45,   213,  -253,  -253,   -87,
      71,  -243,  -253,  -253,  -253,  -253,  -253,   141,   -28,  -253,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,  -250,    -7,  -253,  -253,  -253,  -253,  -253,  -184,    42,
    -112,  -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,    46,
    -253,  -253,    45,  -253,   -25,  -253,  -253,  -253,   -18,  -253,
     286
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    16,    25,    26,    42,     5,     7,    10,
      11,    55,    18,    21,    22,    23,   117,    69,    38,   101,
      84,    56,    57,    19,   269,   292,   169,   170,   208,   209,
     171,    27,    28,    39,    77,    78,    43,    53,    44,    45,
     187,   122,   188,   189,   331,   190,   265,   191,   309,    61,
     210,   103,   288,   318,   267,   192,   104,   105,   106,   107,
     254,   108,   109,   193,   194,   195,   262,   259,   355,   356,
     357,   363,   196,   197,   198,   340,   199,   296,   372,   249,
     320,   358,   375,   294,   335,   250,   251,   336,   200,   255,
     327,   324,   278,   201,   298,   338,   348,   352,   202,   313,
     110,   205,   242,   111,   132,   112,   135,   215,   216,   217,
     315,   113,   211,   114,    31,    48,    70,    71,    72,    73,
     115
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     102,    49,   130,   131,   270,   136,   125,   126,   127,    30,
     203,   247,   133,   147,   301,     3,    58,    75,   128,   116,
      76,   154,   155,   156,   157,   158,   159,   160,   185,    82,
      81,    80,    29,    62,    63,    64,    65,    83,    66,    67,
      13,   361,     4,   362,   323,   129,   323,   281,   326,    14,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   332,    59,   134,   323,   203,   339,   248,    68,
     128,     6,   263,   264,   290,   -33,    15,    68,  -223,   266,
      29,   168,   252,   173,   185,   261,   172,   174,    -6,   240,
     323,    60,   360,    29,    24,   175,     8,   129,     9,    35,
      36,  -223,    37,    91,   186,   272,   273,   176,    12,   177,
     178,   159,   160,   -95,   -95,    29,   179,  -203,  -203,   180,
     181,   182,   129,   300,    62,    63,    90,    65,   337,    66,
      67,    20,   245,   353,   354,   263,   264,   282,   304,   305,
     306,   307,   308,    97,    32,    33,    98,   147,   183,    41,
     164,    34,   184,    24,    47,  -203,   297,   156,   157,   158,
     159,   160,    13,   368,    51,   141,   142,    52,    54,    60,
      68,   147,    87,    85,   203,   310,   203,   116,   118,   154,
     155,   156,   157,   158,   159,   160,   119,   120,   123,   121,
     -32,   124,   185,   162,   185,   161,   166,   204,   214,   213,
     203,   241,  -223,   246,   243,   203,   244,   173,   253,   172,
     256,   174,   257,   295,   258,  -214,   268,    29,   185,   175,
     333,   271,  -214,   185,   341,   342,   274,    91,   275,   344,
     203,   176,   276,   177,   178,   203,   279,   343,   277,   329,
     179,   280,   283,   180,   181,   182,   129,   285,   185,   286,
     284,   287,   289,   185,   369,   321,   291,   302,   303,   311,
     317,   173,   312,   328,   334,   174,   373,    97,  -189,   347,
      98,    29,   183,   175,   345,   351,   184,   349,   359,   -70,
     365,    91,   376,   366,   374,   176,   377,   177,   178,    40,
      17,    50,    79,    46,   179,   206,   293,   180,   181,   182,
     129,   370,    88,   371,   167,    86,   207,    89,    62,    63,
      90,    65,   260,    66,    67,    91,   299,   165,   367,   314,
     346,    97,   319,   316,    98,   364,   183,    74,   206,     0,
     184,     0,     0,  -203,    92,    88,     0,     0,     0,   207,
      89,    62,    63,    90,    65,     0,    66,    67,    91,    93,
      94,     0,     0,    95,    96,    97,     0,     0,    98,     0,
       0,  -104,    99,     0,   100,   -48,     0,    92,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     173,     0,    93,    94,   174,     0,    95,    96,    97,     0,
      29,    98,   175,     0,  -104,    99,   -48,   100,     0,     0,
      91,     0,     0,     0,   176,     0,   177,   178,     0,     0,
       0,     0,     0,   179,     0,     0,   180,   181,   182,   129,
      88,     0,     0,    29,     0,    89,    62,    63,    90,    65,
       0,    66,    67,    91,     0,     0,     0,     0,     0,     0,
      97,     0,     0,    98,     0,   183,     0,     0,     0,   322,
       0,    88,    92,     0,     0,   207,    89,    62,    63,    90,
      65,     0,    66,    67,    91,     0,     0,    93,    94,     0,
       0,    95,    96,    97,     0,     0,    98,     0,     0,     0,
      99,     0,   100,    92,    88,     0,     0,     0,     0,    89,
      62,    63,    90,    65,     0,    66,    67,    91,    93,    94,
       0,     0,    95,    96,    97,     0,     0,    98,     0,     0,
       0,    99,     0,   100,     0,    88,    92,     0,     0,     0,
     163,    62,    63,    90,    65,     0,    66,    67,    91,     0,
       0,    93,    94,     0,     0,    95,    96,    97,     0,     0,
      98,     0,     0,     0,    99,     0,   100,   164,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   137,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    68,    97,     0,
       0,    98,     0,     0,     0,     0,   138,   100,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
       0,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   138,     0,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,     0,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,     0,     0,     0,     0,
     138,   330,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,     0,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,     0,     0,   138,   212,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,     0,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,     0,     0,   138,   325,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,     0,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   350,
     138,     0,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,     0,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   138,     0,   139,   140,   141,
     142,     0,     0,   145,   146,   147,   148,   149,   150,     0,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     138,     0,   139,   140,   141,   142,     0,     0,   145,   146,
     147,     0,     0,     0,     0,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,  -224,     0,  -224,  -224,   141,
     142,     0,     0,   145,   146,   147,     0,     0,     0,     0,
    -224,   152,   153,   154,   155,   156,   157,   158,   159,   160
};

static const yytype_int16 yycheck[] =
{
      60,    31,    96,    97,   188,   101,    93,    94,    95,    17,
     122,     1,    99,    51,   257,     0,    14,    13,    13,    14,
      16,    59,    60,    61,    62,    63,    64,    65,   122,    13,
      55,    10,    11,    14,    15,    16,    17,    21,    19,    20,
       9,    14,     3,    16,   296,    40,   298,   205,   298,    18,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   315,    71,    99,   327,   188,   327,    68,    60,
      13,    13,    23,    24,   242,    42,    55,    60,    68,   185,
      11,    12,    72,     1,   188,   182,   121,     5,     0,   159,
     352,    42,   352,    11,     6,    13,    66,    40,     4,    13,
      14,    68,    16,    21,   122,    64,    65,    25,    16,    27,
      28,    64,    65,    64,    65,    11,    34,    35,    36,    37,
      38,    39,    40,    13,    14,    15,    16,    17,   322,    19,
      20,    66,   172,    35,    36,    23,    24,   207,    29,    30,
      31,    32,    33,    61,    66,    66,    64,    51,    66,     7,
      40,    67,    70,     6,    64,    73,   253,    61,    62,    63,
      64,    65,     9,   357,    42,    45,    46,    68,    66,    42,
      60,    51,    71,    70,   296,   272,   298,    14,    42,    59,
      60,    61,    62,    63,    64,    65,    67,    69,    11,    68,
      42,    68,   296,    68,   298,    16,    16,    68,    16,    69,
     322,    13,    68,    66,    69,   327,    67,     1,    68,   244,
      13,     5,    68,   248,    14,    16,    66,    11,   322,    13,
     317,    66,    16,   327,   330,   331,    66,    21,    66,   335,
     352,    25,    66,    27,    28,   357,    66,   334,    73,   309,
      34,    69,    71,    37,    38,    39,    40,    72,   352,    73,
      67,    67,    71,   357,   358,   295,    16,    66,    66,    13,
      22,     1,    69,    69,    66,     5,    22,    61,    72,    26,
      64,    11,    66,    13,    72,   345,    70,    70,    69,    73,
      72,    21,    14,    73,    72,    25,    69,    27,    28,    26,
      11,    34,    53,    28,    34,     1,   244,    37,    38,    39,
      40,   358,     8,   358,   119,    57,    12,    13,    14,    15,
      16,    17,   181,    19,    20,    21,   255,   114,   356,   284,
     337,    61,   290,   287,    64,   353,    66,    51,     1,    -1,
      70,    -1,    -1,    73,    40,     8,    -1,    -1,    -1,    12,
      13,    14,    15,    16,    17,    -1,    19,    20,    21,    55,
      56,    -1,    -1,    59,    60,    61,    -1,    -1,    64,    -1,
      -1,    67,    68,    -1,    70,    71,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    55,    56,     5,    -1,    59,    60,    61,    -1,
      11,    64,    13,    -1,    67,    68,    69,    70,    -1,    -1,
      21,    -1,    -1,    -1,    25,    -1,    27,    28,    -1,    -1,
      -1,    -1,    -1,    34,    -1,    -1,    37,    38,    39,    40,
       8,    -1,    -1,    11,    -1,    13,    14,    15,    16,    17,
      -1,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    -1,    64,    -1,    66,    -1,    -1,    -1,    70,
      -1,     8,    40,    -1,    -1,    12,    13,    14,    15,    16,
      17,    -1,    19,    20,    21,    -1,    -1,    55,    56,    -1,
      -1,    59,    60,    61,    -1,    -1,    64,    -1,    -1,    -1,
      68,    -1,    70,    40,     8,    -1,    -1,    -1,    -1,    13,
      14,    15,    16,    17,    -1,    19,    20,    21,    55,    56,
      -1,    -1,    59,    60,    61,    -1,    -1,    64,    -1,    -1,
      -1,    68,    -1,    70,    -1,     8,    40,    -1,    -1,    -1,
      13,    14,    15,    16,    17,    -1,    19,    20,    21,    -1,
      -1,    55,    56,    -1,    -1,    59,    60,    61,    -1,    -1,
      64,    -1,    -1,    -1,    68,    -1,    70,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    -1,
      -1,    64,    -1,    -1,    -1,    -1,    41,    70,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      -1,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    41,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    -1,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    -1,    -1,    -1,    -1,
      41,    71,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    -1,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    -1,    -1,    41,    69,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    -1,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    -1,    41,    69,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    -1,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      41,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    -1,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    41,    -1,    43,    44,    45,
      46,    -1,    -1,    49,    50,    51,    52,    53,    54,    -1,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      41,    -1,    43,    44,    45,    46,    -1,    -1,    49,    50,
      51,    -1,    -1,    -1,    -1,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    41,    -1,    43,    44,    45,
      46,    -1,    -1,    49,    50,    51,    -1,    -1,    -1,    -1,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    75,    76,     0,     3,    81,    13,    82,    66,     4,
      83,    84,    16,     9,    18,    55,    77,    85,    86,    97,
      66,    87,    88,    89,     6,    78,    79,   105,   106,    11,
      98,   188,    66,    66,    67,    13,    14,    16,    92,   107,
      78,     7,    80,   110,   112,   113,   105,    64,   189,    92,
      88,    42,    68,   111,    66,    85,    95,    96,    14,    71,
      42,   123,    14,    15,    16,    17,    19,    20,    60,    91,
     190,   191,   192,   193,   194,    13,    16,   108,   109,    96,
      10,   188,    13,    21,    94,    70,   114,    71,     8,    13,
      16,    21,    40,    55,    56,    59,    60,    61,    64,    68,
      70,    93,   124,   125,   130,   131,   132,   133,   135,   136,
     174,   177,   179,   185,   187,   194,    14,    90,    42,    67,
      69,    68,   115,    11,    68,   133,   133,   133,    13,    40,
      93,    93,   178,   133,   188,   180,   123,    22,    41,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    16,    68,    13,    40,   130,    16,   109,    12,   100,
     101,   104,   188,     1,     5,    13,    25,    27,    28,    34,
      37,    38,    39,    66,    70,    93,    98,   114,   116,   117,
     119,   121,   129,   137,   138,   139,   146,   147,   148,   150,
     162,   167,   172,   174,    68,   175,     1,    12,   102,   103,
     124,   186,    69,    69,    16,   181,   182,   183,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     124,    13,   176,    69,    67,    92,    66,     1,    68,   153,
     159,   160,    72,    68,   134,   163,    13,    68,    14,   141,
     141,   133,   140,    23,    24,   120,   123,   128,    66,    98,
     172,    66,    64,    65,    66,    66,    66,    73,   166,    66,
      69,   102,   124,    71,    67,    72,    73,    67,   126,    71,
     102,    16,    99,   104,   157,   188,   151,   133,   168,   134,
      13,   135,    66,    66,    29,    30,    31,    32,    33,   122,
     133,    13,    69,   173,   186,   184,   183,    22,   127,   173,
     154,    92,    70,   116,   165,    69,   165,   164,    69,   124,
      71,   118,   135,   133,    66,   158,   161,   172,   169,   165,
     149,   123,   123,   133,   123,    72,   166,    26,   170,    70,
      66,   124,   171,    35,    36,   142,   143,   144,   155,    69,
     165,    14,    16,   145,   192,    72,    73,   142,   172,    93,
     119,   129,   152,    22,    72,   156,    14,    69
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    76,    75,    77,    77,    78,    78,    79,    80,
      80,    81,    81,    82,    82,    83,    84,    84,    84,    85,
      85,    86,    87,    87,    89,    90,    88,    91,    91,    92,
      92,    92,    93,    93,    94,    94,    95,    95,    96,    97,
      98,    98,    99,    99,   100,   100,   101,   101,   102,   102,
     103,   103,   103,   104,   104,   105,   105,   107,   106,   108,
     108,   108,   109,   109,   109,   111,   110,   113,   112,   114,
     115,   115,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   116,   117,   118,
     117,   117,   117,   120,   119,   121,   122,   122,   122,   122,
     122,   123,   124,   124,   125,   126,   125,   127,   127,   128,
     128,   129,   130,   130,   130,   131,   132,   132,   132,   132,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   134,   134,
     135,   135,   135,   135,   136,   136,   137,   138,   139,   140,
     140,   141,   141,   142,   142,   143,   144,   144,   145,   145,
     145,   146,   147,   149,   148,   151,   150,   152,   152,   154,
     155,   156,   153,   157,   158,   157,   159,   159,   159,   161,
     160,   163,   164,   162,   165,   165,   166,   168,   169,   167,
     170,   171,   170,   172,   172,   173,   175,   174,   176,   174,
     174,   174,   178,   177,   180,   179,   181,   181,   182,   182,
     184,   183,   185,   185,   186,   186,   187,   188,   188,   189,
     189,   190,   190,   191,   192,   192,   193,   194,   194,   194,
     194
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     6,     0,     1,     0,     2,     2,     2,
       1,     0,     3,     0,     2,     3,     0,     3,     3,     0,
       1,     2,     1,     3,     0,     0,     5,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     6,     2,
       3,     2,     0,     1,     0,     1,     1,     3,     0,     1,
       1,     3,     1,     2,     1,     0,     2,     0,     5,     3,
       1,     1,     0,     1,     3,     0,     3,     0,     3,     4,
       0,     3,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     2,     1,     1,     2,     2,     0,
       5,     5,     1,     0,     4,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     0,     0,     5,     0,     2,     1,
       1,     2,     1,     1,     1,     4,     2,     2,     2,     2,
       1,     2,     3,     1,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     2,
       1,     1,     1,     1,     1,     2,     3,     3,     2,     0,
       1,     0,     1,     0,     2,     2,     3,     2,     1,     1,
       3,     2,     2,     0,     8,     0,     4,     1,     1,     0,
       0,     0,    10,     0,     0,     4,     1,     1,     1,     0,
       7,     0,     0,     5,     1,     3,     1,     0,     0,     6,
       0,     0,     3,     0,     2,     1,     0,     5,     0,     5,
       1,     1,     0,     4,     0,     4,     0,     1,     1,     3,
       0,     4,     2,     1,     1,     2,     3,     1,     2,     2,
       3,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif



static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yystrlen (yysymbol_name (yyarg[yyi]));
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp = yystpcpy (yyp, yysymbol_name (yyarg[yyi++]));
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 322 "Parser.y"
                {
				init_header();
			}
#line 2165 "parser.c"
    break;

  case 3: /* file_structure: $@1 namespace prerequisites global_declarations header_delimiter macro_definitions  */
#line 325 "Parser.y"
                                                                                                       {
				finalize_header();
			}
#line 2173 "parser.c"
    break;

  case 11: /* namespace: %empty  */
#line 342 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2181 "parser.c"
    break;

  case 12: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 345 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2190 "parser.c"
    break;

  case 15: /* require: T_REQUIRE T_STRING ';'  */
#line 353 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2199 "parser.c"
    break;

  case 19: /* scope: %empty  */
#line 362 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2205 "parser.c"
    break;

  case 20: /* scope: T_STATIC  */
#line 363 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2211 "parser.c"
    break;

  case 24: /* $@2: %empty  */
#line 373 "Parser.y"
                { _bDefiningConst = 1; }
#line 2217 "parser.c"
    break;

  case 25: /* $@3: %empty  */
#line 373 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2223 "parser.c"
    break;

  case 26: /* constdef: $@2 variable_identifier T_ASSIGN constant_literal $@3  */
#line 374 "Parser.y"
                        {
				BOOL bString = (yyvsp[-1].v.type == VT_STRING);
				sym_createSymbol(_currentIdentifierContext, yyvsp[-3].ident.s,
					S_CONSTANT, yyvsp[-1].v.type, yyvsp[-1].v.data, 0);
				if (yyvsp[-3].ident.stringIsAlloced) {
					free(yyvsp[-3].ident.s);
				}
				if (bString) {
					free(yyvsp[-1].v.data.string);
				}
			}
#line 2239 "parser.c"
    break;

  case 27: /* constant_literal: simple_literal  */
#line 387 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2247 "parser.c"
    break;

  case 28: /* constant_literal: T_STRING  */
#line 390 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2256 "parser.c"
    break;

  case 29: /* variable_identifier: T_IDENT  */
#line 396 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2266 "parser.c"
    break;

  case 30: /* variable_identifier: T_NUM  */
#line 401 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2277 "parser.c"
    break;

  case 31: /* variable_identifier: T_STRING  */
#line 407 "Parser.y"
                                   {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2286 "parser.c"
    break;

  case 32: /* variable_reference: T_VARIABLE  */
#line 413 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2292 "parser.c"
    break;

  case 33: /* variable_reference: T_IDENT  */
#line 414 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2304 "parser.c"
    break;

  case 34: /* macro_declaration: T_IDENT  */
#line 424 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2312 "parser.c"
    break;

  case 35: /* macro_declaration: T_FUNC  */
#line 427 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2322 "parser.c"
    break;

  case 36: /* macro_type: scope T_VOID  */
#line 433 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
				yyval.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(VT_NIL);
				}
			}
#line 2335 "parser.c"
    break;

  case 37: /* macro_type: scope type_name  */
#line 442 "Parser.y"
                                        {
				yyval.ident.type = yyvsp[0].ident.type;
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(yyvsp[0].ident.type);
				}
			}
#line 2348 "parser.c"
    break;

  case 38: /* macrostart: macro_type macro_declaration '(' argument_list ')' optional_description  */
#line 452 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				yyval.ident.type = yyvsp[-5].ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2358 "parser.c"
    break;

  case 40: /* var_decl: type_name variable_identifier assignment_expression  */
#line 460 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});
			}
#line 2371 "parser.c"
    break;

  case 41: /* var_decl: type_name variable_identifier  */
#line 467 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2382 "parser.c"
    break;

  case 43: /* optional_description: T_STRING  */
#line 476 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2391 "parser.c"
    break;

  case 52: /* non_empty_parameters: error  */
#line 494 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 2399 "parser.c"
    break;

  case 53: /* argument_declaration: type_name variable_identifier  */
#line 498 "Parser.y"
                                                    {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[-1].ident.type, yyvsp[0].ident.s);
				} else {
					sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam}, _localVariableIndex);
					bytecode_defineVariable(_currentBytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER, yyvsp[-1].ident.type,_nparam, _localVariableIndex++);
				}
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2414 "parser.c"
    break;

  case 54: /* argument_declaration: T_ELLIPSIS  */
#line 507 "Parser.y"
                                       {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[0].ident.type, "...");
				}
			}
#line 2424 "parser.c"
    break;

  case 57: /* $@4: %empty  */
#line 516 "Parser.y"
                         {
				start_annotation(yyvsp[0].ident.s);
			}
#line 2432 "parser.c"
    break;

  case 59: /* annotation_value: T_IDENT T_ASSIGN T_STRING  */
#line 520 "Parser.y"
                                            {
				parser_defineAnnotationValue(yyvsp[-2].ident.s, yyvsp[0].ident.s);
			}
#line 2440 "parser.c"
    break;

  case 60: /* annotation_value: T_IDENT  */
#line 522 "Parser.y"
                                    {
				parser_defineAnnotationValue(yyvsp[0].ident.s, _strdup("true"));
			}
#line 2448 "parser.c"
    break;

  case 61: /* annotation_value: T_STRING  */
#line 524 "Parser.y"
                                     {
				parser_defineAnnotationValue(_strdup("value"), yyvsp[0].ident.s);
			}
#line 2456 "parser.c"
    break;

  case 65: /* $@5: %empty  */
#line 533 "Parser.y"
                                         {
					init_native();
				}
#line 2464 "parser.c"
    break;

  case 66: /* native_function_definition: T_NATIVE $@5 macrostart  */
#line 535 "Parser.y"
                                             {
					char* pszLib;
					char* pszMacroCName;
					char* pszSignature;
					parser_processNativeMethodAnnotations(&pszLib, &pszMacroCName, &pszSignature);
					char* pszArgs = stringbuf_getString(_nativeMethodArgs);
					if (!pszSignature) {
						pszSignature = stringbuf_getString(_nativeMethodSignature);
					}
					if (!pszMacroCName) {
						pszMacroCName = yyvsp[0].ident.s;
					}
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszMacroCName});
					YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
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
					function_registerNativeFunction(pszMacroCName, yyvsp[0].ident.s, pszLib, pszSignature, _yyCurrentComment, pszArgs);
					free(yyvsp[0].ident.s);
					finalize_native();
				}
#line 2497 "parser.c"
    break;

  case 67: /* $@6: %empty  */
#line 564 "Parser.y"
                  { init_macroDefinition(); }
#line 2503 "parser.c"
    break;

  case 68: /* macro_definition: $@6 macrostart block  */
#line 565 "Parser.y"
                                                 {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName(yyvsp[-1].ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",yyvsp[-1].ident.s);
				bytecode_optimizeInstructions(_currentBytecodeBuffer);
				macro_processAnnotations(&_macroParam, _currentMethodAnnotations);
				_macroParam.mp_name = yyvsp[-1].ident.s;
				_macroParam.mp_comment = _yyCurrentComment;
				_macroParam.mp_returnType = yyvsp[-1].ident.type;
				_macroParam.mp_numberOfLocalVariables = _localVariableIndex;
				_macroParam.mp_bytecodeLength = _currentBytecodeBuffer->bb_current - _currentBytecodeBuffer->bb_start;
				_macroParam.mp_buffer = _currentBytecodeBuffer->bb_start;
				_macroParam.mp_scope = yyvsp[-1].ident.scope;
				_macroParam.mp_sourceFile = yy_getCurrentInputFilename();
				_macroParam.mp_namespaceIdx = _currentNamespaceIndex;
				_compilerConfiguration->cb_insertNewMacro(&_macroParam);
				memset(&_macroParam, 0, sizeof _macroParam);
				freeitem(&yyvsp[-1].ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			}
#line 2532 "parser.c"
    break;

  case 73: /* statement: call_expression ';'  */
#line 596 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2538 "parser.c"
    break;

  case 74: /* statement: assignment ';'  */
#line 597 "Parser.y"
                                         { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2544 "parser.c"
    break;

  case 75: /* statement: if_expression  */
#line 598 "Parser.y"
                                        { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2550 "parser.c"
    break;

  case 76: /* statement: while  */
#line 599 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2556 "parser.c"
    break;

  case 77: /* statement: for_loop_expression  */
#line 600 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2562 "parser.c"
    break;

  case 78: /* statement: switch_expression  */
#line 601 "Parser.y"
                                            { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2568 "parser.c"
    break;

  case 83: /* statement: block  */
#line 606 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2574 "parser.c"
    break;

  case 84: /* statement: increment_expression ';'  */
#line 607 "Parser.y"
                                                   { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2580 "parser.c"
    break;

  case 86: /* statement: var_decl  */
#line 609 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2588 "parser.c"
    break;

  case 87: /* statement: error ';'  */
#line 612 "Parser.y"
                                    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2597 "parser.c"
    break;

  case 88: /* assignment: variable_reference assignment_expression  */
#line 617 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2605 "parser.c"
    break;

  case 89: /* $@7: %empty  */
#line 621 "Parser.y"
                                                      {
				parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2613 "parser.c"
    break;

  case 90: /* assignment: assignment_target '.' T_IDENT $@7 assignment_expression  */
#line 623 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2621 "parser.c"
    break;

  case 91: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 627 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2629 "parser.c"
    break;

  case 93: /* $@8: %empty  */
#line 633 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2637 "parser.c"
    break;

  case 94: /* shorthand_assignment: variable_reference $@8 shorthand_assignment_operator expression  */
#line 635 "Parser.y"
                                                                   {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2646 "parser.c"
    break;

  case 95: /* assignment_target: variable_reference  */
#line 640 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2654 "parser.c"
    break;

  case 96: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 645 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2660 "parser.c"
    break;

  case 97: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 646 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2666 "parser.c"
    break;

  case 98: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 647 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2672 "parser.c"
    break;

  case 99: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 648 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2678 "parser.c"
    break;

  case 100: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 649 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2684 "parser.c"
    break;

  case 101: /* assignment_expression: T_ASSIGN expression  */
#line 651 "Parser.y"
                                                { yyval.v = yyvsp[0].v;	}
#line 2690 "parser.c"
    break;

  case 102: /* expression: binary_expression  */
#line 653 "Parser.y"
                              { yyval.v = yyvsp[0].v; }
#line 2696 "parser.c"
    break;

  case 105: /* $@9: %empty  */
#line 657 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2702 "parser.c"
    break;

  case 108: /* range_increment: T_DOTDOT binary_expression  */
#line 660 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2708 "parser.c"
    break;

  case 109: /* minusminus_plusplus: T_MINUSMINUS  */
#line 663 "Parser.y"
                                     { yyval.num = -1; }
#line 2714 "parser.c"
    break;

  case 110: /* minusminus_plusplus: T_PLUSPLUS  */
#line 664 "Parser.y"
                                     { yyval.num = 1; }
#line 2720 "parser.c"
    break;

  case 111: /* increment_expression: variable_reference minusminus_plusplus  */
#line 667 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2728 "parser.c"
    break;

  case 112: /* simple_term: value  */
#line 671 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2734 "parser.c"
    break;

  case 115: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 675 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2742 "parser.c"
    break;

  case 116: /* unary_expression: '!' binary_expression  */
#line 679 "Parser.y"
                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2748 "parser.c"
    break;

  case 117: /* unary_expression: '~' binary_expression  */
#line 680 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2754 "parser.c"
    break;

  case 118: /* unary_expression: '+' binary_expression  */
#line 681 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2760 "parser.c"
    break;

  case 119: /* unary_expression: '-' variable_reference  */
#line 682 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2766 "parser.c"
    break;

  case 120: /* binary_expression: simple_term  */
#line 685 "Parser.y"
                                    { yyval.ident.type = yyvsp[0].ident.type;}
#line 2772 "parser.c"
    break;

  case 121: /* binary_expression: type_cast simple_term  */
#line 686 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2781 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression '.' T_IDENT  */
#line 691 "Parser.y"
                                                      {
				yyval.ident.type = parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2790 "parser.c"
    break;

  case 124: /* binary_expression: binary_expression '[' expression ']'  */
#line 696 "Parser.y"
                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2796 "parser.c"
    break;

  case 125: /* binary_expression: binary_expression '~' binary_expression  */
#line 697 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2802 "parser.c"
    break;

  case 126: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 698 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2808 "parser.c"
    break;

  case 127: /* binary_expression: binary_expression '&' binary_expression  */
#line 699 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2814 "parser.c"
    break;

  case 128: /* binary_expression: binary_expression '|' binary_expression  */
#line 700 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2820 "parser.c"
    break;

  case 129: /* binary_expression: binary_expression '+' binary_expression  */
#line 701 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2826 "parser.c"
    break;

  case 130: /* binary_expression: binary_expression '^' binary_expression  */
#line 702 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2832 "parser.c"
    break;

  case 131: /* binary_expression: binary_expression '-' binary_expression  */
#line 703 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2838 "parser.c"
    break;

  case 132: /* binary_expression: binary_expression '*' binary_expression  */
#line 704 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2844 "parser.c"
    break;

  case 133: /* binary_expression: binary_expression '/' binary_expression  */
#line 705 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2850 "parser.c"
    break;

  case 134: /* binary_expression: binary_expression '%' binary_expression  */
#line 706 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2856 "parser.c"
    break;

  case 135: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 707 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2862 "parser.c"
    break;

  case 136: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 708 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2868 "parser.c"
    break;

  case 137: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 709 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2874 "parser.c"
    break;

  case 138: /* binary_expression: binary_expression T_AND binary_expression  */
#line 710 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2880 "parser.c"
    break;

  case 139: /* binary_expression: binary_expression T_OR binary_expression  */
#line 711 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2886 "parser.c"
    break;

  case 140: /* binary_expression: binary_expression '<' binary_expression  */
#line 712 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2892 "parser.c"
    break;

  case 141: /* binary_expression: binary_expression '>' binary_expression  */
#line 713 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2898 "parser.c"
    break;

  case 142: /* binary_expression: binary_expression T_LE binary_expression  */
#line 714 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2904 "parser.c"
    break;

  case 143: /* binary_expression: binary_expression T_GE binary_expression  */
#line 715 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2910 "parser.c"
    break;

  case 144: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 716 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2916 "parser.c"
    break;

  case 145: /* binary_expression: binary_expression T_NE binary_expression  */
#line 717 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2922 "parser.c"
    break;

  case 146: /* binary_expression: '(' binary_expression ')'  */
#line 718 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2928 "parser.c"
    break;

  case 147: /* binary_expression: variable_reference assignment_expression  */
#line 719 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2936 "parser.c"
    break;

  case 149: /* condition: '(' binary_expression  */
#line 726 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2944 "parser.c"
    break;

  case 150: /* value: T_VARIABLE  */
#line 730 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 2953 "parser.c"
    break;

  case 151: /* value: string  */
#line 734 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2963 "parser.c"
    break;

  case 152: /* value: simple_literal  */
#line 739 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 2969 "parser.c"
    break;

  case 153: /* value: T_IDENT  */
#line 740 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 2979 "parser.c"
    break;

  case 154: /* string: T_STRING  */
#line 746 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2985 "parser.c"
    break;

  case 155: /* string: string T_STRING  */
#line 747 "Parser.y"
                                          { 
				if ((yyval.ident.s = malloc(strlen(yyvsp[-1].ident.s)+strlen(yyvsp[0].ident.s)+1)) != 0) {
					strcat(strcpy(yyval.ident.s,yyvsp[-1].ident.s),yyvsp[0].ident.s);
					free(yyvsp[-1].ident.s);
					free(yyvsp[0].ident.s);
				} else {
					yyerror("String too long: buffer overflow");
					yyval.ident = yyvsp[0].ident;
				}
			}
#line 3000 "parser.c"
    break;

  case 156: /* break: T_BREAK opt_num ';'  */
#line 759 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 3006 "parser.c"
    break;

  case 157: /* continue: T_CONTINUE opt_num ';'  */
#line 761 "Parser.y"
                                       {
				// in the case this is a continue in a for(int i;b;i++) loop, generate the i++ code before continuing.
				parser_flushBuffer(_breaklevel-(int)yyvsp[-1].num+1);
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); 
			}
#line 3016 "parser.c"
    break;

  case 158: /* return_expression: T_RETURN optional_bterm  */
#line 768 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 3024 "parser.c"
    break;

  case 161: /* opt_num: %empty  */
#line 775 "Parser.y"
                {	yyval.num = 1;	}
#line 3030 "parser.c"
    break;

  case 162: /* opt_num: T_NUM  */
#line 776 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 3036 "parser.c"
    break;

  case 167: /* case_selector: T_DEFAULT ':'  */
#line 785 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 3044 "parser.c"
    break;

  case 168: /* case_condition: integer_literal  */
#line 790 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 3052 "parser.c"
    break;

  case 169: /* case_condition: T_STRING  */
#line 793 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3060 "parser.c"
    break;

  case 170: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 796 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3068 "parser.c"
    break;

  case 171: /* label: T_IDENT ':'  */
#line 800 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3077 "parser.c"
    break;

  case 172: /* goto_expression: T_GOTO T_IDENT  */
#line 806 "Parser.y"
                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3086 "parser.c"
    break;

  case 173: /* $@10: %empty  */
#line 813 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3096 "parser.c"
    break;

  case 174: /* switch_expression: T_SWITCH '(' value ')' $@10 '{' case_clauses '}'  */
#line 819 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3109 "parser.c"
    break;

  case 175: /* $@11: %empty  */
#line 829 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3118 "parser.c"
    break;

  case 176: /* for_loop_expression: T_FOR for_clause $@11 local_block  */
#line 832 "Parser.y"
                                      {
				if (yyvsp[-2].ident.s) {
					sym_remove(_currentIdentifierContext, yyvsp[-2].ident.s);
					free(yyvsp[-2].ident.s);
				}
				parser_flushBuffer(_breaklevel);
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3135 "parser.c"
    break;

  case 179: /* $@12: %empty  */
#line 848 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3143 "parser.c"
    break;

  case 180: /* $@13: %empty  */
#line 850 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3151 "parser.c"
    break;

  case 181: /* $@14: %empty  */
#line 852 "Parser.y"
                                        {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			}
#line 3160 "parser.c"
    break;

  case 182: /* for_to_clause: '(' opt_for_initializer $@12 ';' binary_expression ';' $@13 for_increment $@14 ')'  */
#line 855 "Parser.y"
                              {
				yyval.ident.s = yyvsp[-8].ident.s;
			}
#line 3168 "parser.c"
    break;

  case 184: /* $@15: %empty  */
#line 860 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3176 "parser.c"
    break;

  case 185: /* opt_for_initializer: type_name variable_identifier $@15 assignment_expression  */
#line 862 "Parser.y"
                                                {
				yyval.ident.s = yyvsp[-2].ident.s;
			}
#line 3184 "parser.c"
    break;

  case 186: /* for_clause: for_to_clause  */
#line 866 "Parser.y"
                              {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3192 "parser.c"
    break;

  case 187: /* for_clause: in_clause  */
#line 869 "Parser.y"
                                    {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3200 "parser.c"
    break;

  case 188: /* for_clause: error  */
#line 872 "Parser.y"
                                {
				yyval.ident.s = 0;
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3210 "parser.c"
    break;

  case 189: /* $@16: %empty  */
#line 878 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				parser_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3221 "parser.c"
    break;

  case 190: /* in_clause: '(' type_name variable_identifier $@16 ':' expression ')'  */
#line 883 "Parser.y"
                                             {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
				yyval.ident.s = yyvsp[-4].ident.s;
			}
#line 3233 "parser.c"
    break;

  case 191: /* $@17: %empty  */
#line 891 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3241 "parser.c"
    break;

  case 192: /* $@18: %empty  */
#line 893 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3250 "parser.c"
    break;

  case 193: /* while: T_WHILE $@17 condition $@18 local_block  */
#line 896 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3262 "parser.c"
    break;

  case 197: /* $@19: %empty  */
#line 912 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3271 "parser.c"
    break;

  case 198: /* $@20: %empty  */
#line 915 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3279 "parser.c"
    break;

  case 199: /* if_expression: T_IF condition $@19 local_block $@20 else_clause  */
#line 917 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3288 "parser.c"
    break;

  case 200: /* else_clause: %empty  */
#line 922 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3297 "parser.c"
    break;

  case 201: /* $@21: %empty  */
#line 926 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3307 "parser.c"
    break;

  case 206: /* @22: %empty  */
#line 938 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3319 "parser.c"
    break;

  case 207: /* call_expression: T_FUNC '(' @22 parameter_list closing_paren  */
#line 944 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3329 "parser.c"
    break;

  case 208: /* @23: %empty  */
#line 949 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3340 "parser.c"
    break;

  case 209: /* call_expression: function_id_or_pointer '(' @23 parameter_list closing_paren  */
#line 954 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3354 "parser.c"
    break;

  case 212: /* @24: %empty  */
#line 966 "Parser.y"
                       {
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = &_functionTable[FUNC_CreateArray];
			}
#line 3364 "parser.c"
    break;

  case 213: /* array_constructor: '[' @24 parameter_list ']'  */
#line 970 "Parser.y"
                                             {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
					C_0FUNC,(GENERIC_DATA){FUNC_CreateArray}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
			}
#line 3374 "parser.c"
    break;

  case 214: /* @25: %empty  */
#line 976 "Parser.y"
                     {
		_currentFunctionCallParamIndexP++;
		*_currentFunctionCallParamIndexP = 0;
		yyval.funcp = &_functionTable[FUNC_CreateMap];
	}
#line 3384 "parser.c"
    break;

  case 215: /* map_constructor: '{' @25 optional_map_associates '}'  */
#line 980 "Parser.y"
                                      {
		_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
			C_0FUNC,(GENERIC_DATA){FUNC_CreateMap}, *_currentFunctionCallParamIndexP);
		_currentFunctionCallParamIndexP--;
	}
#line 3394 "parser.c"
    break;

  case 220: /* @26: %empty  */
#line 996 "Parser.y"
                                     { 
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[-1].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[-1].ident.s);
				(*_currentFunctionCallParamIndexP) += 2;
			}
#line 3405 "parser.c"
    break;

  case 222: /* function_id_or_pointer: '*' variable_reference  */
#line 1004 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3419 "parser.c"
    break;

  case 223: /* function_id_or_pointer: T_IDENT  */
#line 1014 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3428 "parser.c"
    break;

  case 224: /* parameter: expression  */
#line 1019 "Parser.y"
                           { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3434 "parser.c"
    break;

  case 225: /* parameter: T_ELLIPSIS expression  */
#line 1020 "Parser.y"
                                                { 
				yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; 
				YY_EMIT(C_SPREAD, (GENERIC_DATA){1});
			}
#line 3443 "parser.c"
    break;

  case 226: /* type_cast: '(' type_name ')'  */
#line 1025 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3452 "parser.c"
    break;

  case 227: /* type_name: T_TYPE_IDENTIFIER  */
#line 1030 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3460 "parser.c"
    break;

  case 228: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 1033 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3469 "parser.c"
    break;

  case 230: /* array_size: '[' T_NUM ']'  */
#line 1039 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3477 "parser.c"
    break;

  case 231: /* boolean_literal: T_TRUE  */
#line 1043 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3486 "parser.c"
    break;

  case 232: /* boolean_literal: T_FALSE  */
#line 1046 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3495 "parser.c"
    break;

  case 233: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 1051 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3504 "parser.c"
    break;

  case 234: /* integer_literal: T_NUM  */
#line 1056 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3513 "parser.c"
    break;

  case 235: /* integer_literal: '-' T_NUM  */
#line 1060 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3523 "parser.c"
    break;

  case 236: /* character_literal: T_CHARACTER  */
#line 1067 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3532 "parser.c"
    break;

  case 237: /* simple_literal: integer_literal  */
#line 1072 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3545 "parser.c"
    break;

  case 238: /* simple_literal: float_literal  */
#line 1080 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3558 "parser.c"
    break;

  case 239: /* simple_literal: character_literal  */
#line 1088 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3571 "parser.c"
    break;

  case 240: /* simple_literal: boolean_literal  */
#line 1096 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3584 "parser.c"
    break;


#line 3588 "parser.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1104 "Parser.y"


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

