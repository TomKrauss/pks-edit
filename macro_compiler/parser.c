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
# define	MACRO_BYTECODE_SPACE	32000

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
extern 			int	_bInEnumDefinition;

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
		int nCapacity = aLevel < 1 ? MACRO_BYTECODE_SPACE*4 : MACRO_BYTECODE_SPACE;
		_currentBytecodeBuffer->bb_end = ((_currentBytecodeBuffer->bb_start = malloc(nCapacity)) == 0) ? 
			_currentBytecodeBuffer->bb_start : _currentBytecodeBuffer->bb_start + nCapacity;
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
		hashmap_put(pAnnotation->a_values, pszKey, (intptr_t)pszValue);
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
			*pszLib = (char*)hashmap_get(pAnnotation->a_values, "lib");
			*pszMacroCName = (char*)hashmap_get(pAnnotation->a_values, "name");
			*pszSignature = (char*)hashmap_get(pAnnotation->a_values, "signature");
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


#line 367 "parser.c"

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
  YYSYMBOL_T_TYPEDEF = 4,                  /* T_TYPEDEF  */
  YYSYMBOL_T_ENUM = 5,                     /* T_ENUM  */
  YYSYMBOL_T_STRUCT = 6,                   /* T_STRUCT  */
  YYSYMBOL_T_REQUIRE = 7,                  /* T_REQUIRE  */
  YYSYMBOL_T_FOR = 8,                      /* T_FOR  */
  YYSYMBOL_T_ANNOTATION = 9,               /* T_ANNOTATION  */
  YYSYMBOL_T_NATIVE = 10,                  /* T_NATIVE  */
  YYSYMBOL_T_NEW = 11,                     /* T_NEW  */
  YYSYMBOL_T_STATIC = 12,                  /* T_STATIC  */
  YYSYMBOL_T_VOID = 13,                    /* T_VOID  */
  YYSYMBOL_T_TYPE_IDENTIFIER = 14,         /* T_TYPE_IDENTIFIER  */
  YYSYMBOL_T_ELLIPSIS = 15,                /* T_ELLIPSIS  */
  YYSYMBOL_T_IDENT = 16,                   /* T_IDENT  */
  YYSYMBOL_T_NUM = 17,                     /* T_NUM  */
  YYSYMBOL_T_FLOATING_POINT_NUMBER = 18,   /* T_FLOATING_POINT_NUMBER  */
  YYSYMBOL_T_STRING = 19,                  /* T_STRING  */
  YYSYMBOL_T_CHARACTER = 20,               /* T_CHARACTER  */
  YYSYMBOL_T_CONST = 21,                   /* T_CONST  */
  YYSYMBOL_T_TRUE = 22,                    /* T_TRUE  */
  YYSYMBOL_T_FALSE = 23,                   /* T_FALSE  */
  YYSYMBOL_T_FUNC = 24,                    /* T_FUNC  */
  YYSYMBOL_T_DOTDOT = 25,                  /* T_DOTDOT  */
  YYSYMBOL_T_PLUSPLUS = 26,                /* T_PLUSPLUS  */
  YYSYMBOL_T_MINUSMINUS = 27,              /* T_MINUSMINUS  */
  YYSYMBOL_T_IF = 28,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 29,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 30,                   /* T_WHILE  */
  YYSYMBOL_T_GOTO = 31,                    /* T_GOTO  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 32,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 33,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 34,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 35,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 36,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_SWITCH = 37,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 38,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 39,                 /* T_DEFAULT  */
  YYSYMBOL_T_BREAK = 40,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 41,                /* T_CONTINUE  */
  YYSYMBOL_T_RETURN = 42,                  /* T_RETURN  */
  YYSYMBOL_T_VARIABLE = 43,                /* T_VARIABLE  */
  YYSYMBOL_T_NMATCH = 44,                  /* T_NMATCH  */
  YYSYMBOL_T_ASSIGN = 45,                  /* T_ASSIGN  */
  YYSYMBOL_T_NE = 46,                      /* T_NE  */
  YYSYMBOL_T_EQ = 47,                      /* T_EQ  */
  YYSYMBOL_T_SHIFT_LEFT = 48,              /* T_SHIFT_LEFT  */
  YYSYMBOL_T_SHIFT_RIGHT = 49,             /* T_SHIFT_RIGHT  */
  YYSYMBOL_T_OR = 50,                      /* T_OR  */
  YYSYMBOL_T_AND = 51,                     /* T_AND  */
  YYSYMBOL_T_LE = 52,                      /* T_LE  */
  YYSYMBOL_T_GE = 53,                      /* T_GE  */
  YYSYMBOL_T_POWER_TO = 54,                /* T_POWER_TO  */
  YYSYMBOL_55_ = 55,                       /* '&'  */
  YYSYMBOL_56_ = 56,                       /* '|'  */
  YYSYMBOL_57_ = 57,                       /* '^'  */
  YYSYMBOL_58_ = 58,                       /* '!'  */
  YYSYMBOL_59_ = 59,                       /* '~'  */
  YYSYMBOL_60_ = 60,                       /* '<'  */
  YYSYMBOL_61_ = 61,                       /* '>'  */
  YYSYMBOL_62_ = 62,                       /* '+'  */
  YYSYMBOL_63_ = 63,                       /* '-'  */
  YYSYMBOL_64_ = 64,                       /* '*'  */
  YYSYMBOL_65_ = 65,                       /* '/'  */
  YYSYMBOL_66_ = 66,                       /* '%'  */
  YYSYMBOL_67_ = 67,                       /* '['  */
  YYSYMBOL_68_ = 68,                       /* '.'  */
  YYSYMBOL_69_ = 69,                       /* ';'  */
  YYSYMBOL_70_ = 70,                       /* '{'  */
  YYSYMBOL_71_ = 71,                       /* '}'  */
  YYSYMBOL_72_ = 72,                       /* ','  */
  YYSYMBOL_73_ = 73,                       /* '('  */
  YYSYMBOL_74_ = 74,                       /* ')'  */
  YYSYMBOL_75_ = 75,                       /* ']'  */
  YYSYMBOL_76_ = 76,                       /* ':'  */
  YYSYMBOL_YYACCEPT = 77,                  /* $accept  */
  YYSYMBOL_file_structure = 78,            /* file_structure  */
  YYSYMBOL_79_1 = 79,                      /* $@1  */
  YYSYMBOL_header_delimiter = 80,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 81,         /* macro_definitions  */
  YYSYMBOL_element_with_annotations = 82,  /* element_with_annotations  */
  YYSYMBOL_macro_or_native_function = 83,  /* macro_or_native_function  */
  YYSYMBOL_namespace = 84,                 /* namespace  */
  YYSYMBOL_prerequisites = 85,             /* prerequisites  */
  YYSYMBOL_require = 86,                   /* require  */
  YYSYMBOL_global_declarations = 87,       /* global_declarations  */
  YYSYMBOL_typedef = 88,                   /* typedef  */
  YYSYMBOL_struct_definition = 89,         /* struct_definition  */
  YYSYMBOL_enum_definition = 90,           /* enum_definition  */
  YYSYMBOL_91_2 = 91,                      /* $@2  */
  YYSYMBOL_struct_element_definitions = 92, /* struct_element_definitions  */
  YYSYMBOL_struct_element = 93,            /* struct_element  */
  YYSYMBOL_enum_value_definitions = 94,    /* enum_value_definitions  */
  YYSYMBOL_enum_values = 95,               /* enum_values  */
  YYSYMBOL_enum_value = 96,                /* enum_value  */
  YYSYMBOL_opt_enum_assignment = 97,       /* opt_enum_assignment  */
  YYSYMBOL_scope = 98,                     /* scope  */
  YYSYMBOL_constants = 99,                 /* constants  */
  YYSYMBOL_constant_list = 100,            /* constant_list  */
  YYSYMBOL_constdef = 101,                 /* constdef  */
  YYSYMBOL_102_3 = 102,                    /* $@3  */
  YYSYMBOL_103_4 = 103,                    /* $@4  */
  YYSYMBOL_constant_literal = 104,         /* constant_literal  */
  YYSYMBOL_variable_identifier = 105,      /* variable_identifier  */
  YYSYMBOL_variable_reference = 106,       /* variable_reference  */
  YYSYMBOL_macro_declaration = 107,        /* macro_declaration  */
  YYSYMBOL_macro_type = 108,               /* macro_type  */
  YYSYMBOL_macrostart = 109,               /* macrostart  */
  YYSYMBOL_global_var = 110,               /* global_var  */
  YYSYMBOL_var_decl = 111,                 /* var_decl  */
  YYSYMBOL_optional_description = 112,     /* optional_description  */
  YYSYMBOL_argument_list = 113,            /* argument_list  */
  YYSYMBOL_non_empty_arguments = 114,      /* non_empty_arguments  */
  YYSYMBOL_parameter_list = 115,           /* parameter_list  */
  YYSYMBOL_non_empty_parameters = 116,     /* non_empty_parameters  */
  YYSYMBOL_argument_declaration = 117,     /* argument_declaration  */
  YYSYMBOL_annotations = 118,              /* annotations  */
  YYSYMBOL_annotation = 119,               /* annotation  */
  YYSYMBOL_120_5 = 120,                    /* $@5  */
  YYSYMBOL_annotation_value = 121,         /* annotation_value  */
  YYSYMBOL_annotation_values = 122,        /* annotation_values  */
  YYSYMBOL_native_function_definition = 123, /* native_function_definition  */
  YYSYMBOL_124_6 = 124,                    /* $@6  */
  YYSYMBOL_macro_definition = 125,         /* macro_definition  */
  YYSYMBOL_126_7 = 126,                    /* $@7  */
  YYSYMBOL_block = 127,                    /* block  */
  YYSYMBOL_var_decls = 128,                /* var_decls  */
  YYSYMBOL_statement = 129,                /* statement  */
  YYSYMBOL_assignment = 130,               /* assignment  */
  YYSYMBOL_131_8 = 131,                    /* $@8  */
  YYSYMBOL_shorthand_assignment = 132,     /* shorthand_assignment  */
  YYSYMBOL_133_9 = 133,                    /* $@9  */
  YYSYMBOL_assignment_target = 134,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 135, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 136,    /* assignment_expression  */
  YYSYMBOL_expression = 137,               /* expression  */
  YYSYMBOL_range_expression = 138,         /* range_expression  */
  YYSYMBOL_139_10 = 139,                   /* $@10  */
  YYSYMBOL_range_increment = 140,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 141,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 142,     /* increment_expression  */
  YYSYMBOL_simple_term = 143,              /* simple_term  */
  YYSYMBOL_constructor_expression = 144,   /* constructor_expression  */
  YYSYMBOL_unary_expression = 145,         /* unary_expression  */
  YYSYMBOL_binary_expression = 146,        /* binary_expression  */
  YYSYMBOL_condition = 147,                /* condition  */
  YYSYMBOL_value = 148,                    /* value  */
  YYSYMBOL_string = 149,                   /* string  */
  YYSYMBOL_break = 150,                    /* break  */
  YYSYMBOL_continue = 151,                 /* continue  */
  YYSYMBOL_return_expression = 152,        /* return_expression  */
  YYSYMBOL_optional_bterm = 153,           /* optional_bterm  */
  YYSYMBOL_opt_num = 154,                  /* opt_num  */
  YYSYMBOL_case_clauses = 155,             /* case_clauses  */
  YYSYMBOL_case_clause = 156,              /* case_clause  */
  YYSYMBOL_case_selector = 157,            /* case_selector  */
  YYSYMBOL_case_condition = 158,           /* case_condition  */
  YYSYMBOL_label = 159,                    /* label  */
  YYSYMBOL_goto_expression = 160,          /* goto_expression  */
  YYSYMBOL_switch_expression = 161,        /* switch_expression  */
  YYSYMBOL_162_11 = 162,                   /* $@11  */
  YYSYMBOL_for_loop_expression = 163,      /* for_loop_expression  */
  YYSYMBOL_164_12 = 164,                   /* $@12  */
  YYSYMBOL_for_increment = 165,            /* for_increment  */
  YYSYMBOL_for_to_clause = 166,            /* for_to_clause  */
  YYSYMBOL_167_13 = 167,                   /* $@13  */
  YYSYMBOL_168_14 = 168,                   /* $@14  */
  YYSYMBOL_169_15 = 169,                   /* $@15  */
  YYSYMBOL_opt_for_initializer = 170,      /* opt_for_initializer  */
  YYSYMBOL_171_16 = 171,                   /* $@16  */
  YYSYMBOL_for_clause = 172,               /* for_clause  */
  YYSYMBOL_in_clause = 173,                /* in_clause  */
  YYSYMBOL_174_17 = 174,                   /* $@17  */
  YYSYMBOL_while = 175,                    /* while  */
  YYSYMBOL_176_18 = 176,                   /* $@18  */
  YYSYMBOL_177_19 = 177,                   /* $@19  */
  YYSYMBOL_local_block = 178,              /* local_block  */
  YYSYMBOL_closing_brace = 179,            /* closing_brace  */
  YYSYMBOL_if_expression = 180,            /* if_expression  */
  YYSYMBOL_181_20 = 181,                   /* $@20  */
  YYSYMBOL_182_21 = 182,                   /* $@21  */
  YYSYMBOL_else_clause = 183,              /* else_clause  */
  YYSYMBOL_184_22 = 184,                   /* $@22  */
  YYSYMBOL_stmntlist = 185,                /* stmntlist  */
  YYSYMBOL_closing_paren = 186,            /* closing_paren  */
  YYSYMBOL_call_expression = 187,          /* call_expression  */
  YYSYMBOL_188_23 = 188,                   /* @23  */
  YYSYMBOL_189_24 = 189,                   /* @24  */
  YYSYMBOL_array_constructor = 190,        /* array_constructor  */
  YYSYMBOL_191_25 = 191,                   /* @25  */
  YYSYMBOL_map_constructor = 192,          /* map_constructor  */
  YYSYMBOL_193_26 = 193,                   /* @26  */
  YYSYMBOL_optional_map_associates = 194,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 195,           /* map_associates  */
  YYSYMBOL_map_associate = 196,            /* map_associate  */
  YYSYMBOL_197_27 = 197,                   /* @27  */
  YYSYMBOL_function_id_or_pointer = 198,   /* function_id_or_pointer  */
  YYSYMBOL_parameter = 199,                /* parameter  */
  YYSYMBOL_type_cast = 200,                /* type_cast  */
  YYSYMBOL_type_name = 201,                /* type_name  */
  YYSYMBOL_array_size = 202,               /* array_size  */
  YYSYMBOL_boolean_literal = 203,          /* boolean_literal  */
  YYSYMBOL_float_literal = 204,            /* float_literal  */
  YYSYMBOL_integer_literal = 205,          /* integer_literal  */
  YYSYMBOL_character_literal = 206,        /* character_literal  */
  YYSYMBOL_simple_literal = 207            /* simple_literal  */
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
#define YYLAST   791

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  131
/* YYNRULES -- Number of rules.  */
#define YYNRULES  257
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  409

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   309


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
       2,     2,     2,    58,     2,     2,     2,    66,    55,     2,
      73,    74,    64,    62,    72,    63,    68,    65,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    76,    69,
      60,     2,    61,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    67,     2,    75,    57,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    70,    56,    71,    59,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   324,   324,   324,   334,   335,   337,   338,   341,   344,
     345,   347,   350,   355,   356,   358,   363,   364,   365,   366,
     368,   369,   371,   375,   375,   380,   381,   383,   389,   390,
     392,   393,   395,   407,   408,   411,   415,   416,   420,   423,
     424,   426,   426,   426,   440,   443,   449,   454,   460,   466,
     467,   477,   480,   486,   495,   504,   511,   513,   520,   527,
     529,   534,   535,   538,   540,   542,   543,   545,   546,   547,
     551,   560,   566,   567,   569,   569,   573,   575,   577,   581,
     582,   583,   586,   586,   617,   617,   643,   645,   646,   648,
     649,   650,   651,   652,   653,   654,   655,   656,   657,   658,
     659,   660,   661,   662,   665,   670,   674,   674,   680,   683,
     686,   686,   693,   698,   699,   700,   701,   702,   704,   706,
     707,   709,   710,   710,   712,   713,   716,   717,   720,   724,
     725,   726,   728,   732,   733,   734,   735,   738,   739,   744,
     748,   749,   750,   751,   752,   753,   754,   755,   756,   757,
     758,   759,   760,   761,   762,   763,   764,   765,   766,   767,
     768,   769,   770,   771,   772,   778,   779,   783,   787,   792,
     793,   799,   800,   811,   814,   821,   825,   826,   828,   829,
     831,   832,   834,   836,   838,   843,   846,   849,   853,   859,
     866,   865,   882,   882,   898,   899,   901,   903,   905,   901,
     912,   913,   913,   919,   922,   925,   931,   931,   944,   946,
     944,   958,   960,   962,   965,   968,   965,   975,   979,   979,
     986,   987,   989,   991,   991,  1002,  1002,  1016,  1017,  1019,
    1019,  1029,  1029,  1039,  1041,  1044,  1046,  1049,  1049,  1057,
    1067,  1072,  1073,  1078,  1083,  1086,  1091,  1092,  1096,  1099,
    1104,  1109,  1113,  1120,  1125,  1133,  1141,  1149
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
  "end of file", "error", "invalid token", "T_NAMESPACE", "T_TYPEDEF",
  "T_ENUM", "T_STRUCT", "T_REQUIRE", "T_FOR", "T_ANNOTATION", "T_NATIVE",
  "T_NEW", "T_STATIC", "T_VOID", "T_TYPE_IDENTIFIER", "T_ELLIPSIS",
  "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER",
  "T_CONST", "T_TRUE", "T_FALSE", "T_FUNC", "T_DOTDOT", "T_PLUSPLUS",
  "T_MINUSMINUS", "T_IF", "T_ELSE", "T_WHILE", "T_GOTO",
  "T_SH_ASSIGN_PLUS", "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT",
  "T_SH_ASSIGN_DIV", "T_SH_ASSIGN_MOD", "T_SWITCH", "T_CASE", "T_DEFAULT",
  "T_BREAK", "T_CONTINUE", "T_RETURN", "T_VARIABLE", "T_NMATCH",
  "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR",
  "T_AND", "T_LE", "T_GE", "T_POWER_TO", "'&'", "'|'", "'^'", "'!'", "'~'",
  "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "'.'", "';'",
  "'{'", "'}'", "','", "'('", "')'", "']'", "':'", "$accept",
  "file_structure", "$@1", "header_delimiter", "macro_definitions",
  "element_with_annotations", "macro_or_native_function", "namespace",
  "prerequisites", "require", "global_declarations", "typedef",
  "struct_definition", "enum_definition", "$@2",
  "struct_element_definitions", "struct_element", "enum_value_definitions",
  "enum_values", "enum_value", "opt_enum_assignment", "scope", "constants",
  "constant_list", "constdef", "$@3", "$@4", "constant_literal",
  "variable_identifier", "variable_reference", "macro_declaration",
  "macro_type", "macrostart", "global_var", "var_decl",
  "optional_description", "argument_list", "non_empty_arguments",
  "parameter_list", "non_empty_parameters", "argument_declaration",
  "annotations", "annotation", "$@5", "annotation_value",
  "annotation_values", "native_function_definition", "$@6",
  "macro_definition", "$@7", "block", "var_decls", "statement",
  "assignment", "$@8", "shorthand_assignment", "$@9", "assignment_target",
  "shorthand_assignment_operator", "assignment_expression", "expression",
  "range_expression", "$@10", "range_increment", "minusminus_plusplus",
  "increment_expression", "simple_term", "constructor_expression",
  "unary_expression", "binary_expression", "condition", "value", "string",
  "break", "continue", "return_expression", "optional_bterm", "opt_num",
  "case_clauses", "case_clause", "case_selector", "case_condition",
  "label", "goto_expression", "switch_expression", "$@11",
  "for_loop_expression", "$@12", "for_increment", "for_to_clause", "$@13",
  "$@14", "$@15", "opt_for_initializer", "$@16", "for_clause", "in_clause",
  "$@17", "while", "$@18", "$@19", "local_block", "closing_brace",
  "if_expression", "$@20", "$@21", "else_clause", "$@22", "stmntlist",
  "closing_paren", "call_expression", "@23", "@24", "array_constructor",
  "@25", "map_constructor", "@26", "optional_map_associates",
  "map_associates", "map_associate", "@27", "function_id_or_pointer",
  "parameter", "type_cast", "type_name", "array_size", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-278)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-241)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -278,    55,     8,  -278,     2,  -278,   -40,    82,  -278,    74,
    -278,    26,    27,    31,  -278,  -278,  -278,    19,    84,    33,
      47,  -278,  -278,    30,    62,  -278,  -278,    51,  -278,    16,
    -278,  -278,    19,   123,   132,    75,  -278,    16,  -278,  -278,
      73,    84,  -278,  -278,  -278,  -278,  -278,   101,    79,  -278,
    -278,  -278,    91,  -278,   141,  -278,     3,  -278,   118,   149,
      98,    84,    16,  -278,    22,    71,   141,  -278,    94,     1,
     100,   103,  -278,   481,  -278,   140,   105,  -278,   120,   178,
    -278,   128,  -278,  -278,  -278,  -278,  -278,  -278,   182,  -278,
    -278,  -278,  -278,  -278,  -278,   158,  -278,   133,   130,  -278,
    -278,  -278,  -278,  -278,   143,  -278,  -278,  -278,   192,   -30,
    -278,   145,   169,   481,   481,   481,     6,    11,  -278,  -278,
     423,   118,  -278,  -278,  -278,  -278,  -278,   509,  -278,   200,
    -278,  -278,  -278,   147,   164,  -278,    34,  -278,   205,   149,
    -278,  -278,  -278,  -278,   204,    71,  -278,    99,   346,   151,
    -278,   698,   723,   189,  -278,  -278,  -278,  -278,   261,   206,
     564,   152,  -278,   481,   481,   481,   481,   481,   481,   481,
     481,   481,   481,   481,   481,   481,   481,   481,   481,   481,
     481,   481,   481,   481,   481,   481,   213,  -278,  -278,   157,
    -278,  -278,  -278,   215,  -278,  -278,  -278,  -278,  -278,   159,
     163,  -278,    16,   167,    13,    18,   175,  -278,   221,   186,
     223,   223,   481,  -278,   230,   122,   191,  -278,    87,   194,
    -278,    52,   195,  -278,  -278,   196,  -278,   197,  -278,  -278,
    -278,  -278,   180,   199,   187,   294,  -278,   481,   198,   210,
    -278,  -278,   211,   218,   214,  -278,  -278,  -278,   648,   723,
     723,   723,   234,   234,   673,   673,   464,   464,    54,   698,
     698,   698,   723,   464,   464,   189,   189,    54,    54,    54,
     216,  -278,   294,  -278,   251,    99,  -278,  -278,  -278,    84,
    -278,  -278,  -278,  -278,   481,  -278,   175,  -278,   322,  -278,
     202,   224,   648,  -278,  -278,  -278,   176,  -278,  -278,  -278,
    -278,  -278,  -278,   481,   276,  -278,  -278,  -278,  -278,  -278,
    -278,  -278,   220,  -278,  -278,   452,  -278,  -278,   206,   281,
    -278,   220,  -278,  -278,  -278,  -278,    16,   390,   593,   390,
    -278,  -278,   233,  -278,  -278,  -278,  -278,  -278,  -278,  -278,
     481,   534,  -278,  -278,  -278,  -278,   322,  -278,   481,  -278,
    -278,   239,   245,   131,  -278,  -278,  -278,  -278,   390,  -278,
    -278,   118,   118,  -278,  -278,   481,   118,   246,   180,   286,
    -278,   256,  -278,  -278,   622,  -278,   481,  -278,  -278,  -278,
      97,  -278,   253,   390,    29,   254,   258,    97,    87,    11,
    -278,  -278,   307,  -278,   259,  -278,  -278,  -278,  -278,  -278,
     111,  -278,  -278,  -278,   326,  -278,   272,  -278,  -278
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,    11,     1,     0,    13,     0,    16,    12,     0,
      14,     4,     0,     0,    37,    41,     5,    72,     0,     0,
       0,    15,    23,     0,     0,    21,    20,    38,    39,     0,
      74,     3,    72,    84,    72,   244,    56,     0,    17,    18,
       0,     0,    19,    41,    46,    47,    48,     0,     0,     7,
      82,     8,     0,    10,    36,    73,     0,   245,    58,    28,
       0,    25,     0,    40,     0,    79,    36,     9,     0,     0,
       0,     0,   246,   121,    57,    33,     0,    29,    30,     0,
      26,     0,   251,   250,    45,   253,   248,   249,     0,    42,
     257,   255,   254,   256,    44,    77,    78,    80,     0,    83,
      53,    54,    51,    52,     0,    87,    85,   247,     0,   170,
     171,     0,   167,     0,     0,     0,     0,     0,   229,   231,
       0,     0,   118,   120,   137,   131,   140,   119,   129,   168,
     130,   227,   228,     0,     0,   169,     0,    32,     0,     0,
      22,    27,   252,    43,     0,    79,    75,    61,     0,     0,
     223,   133,   134,   135,    50,    49,   136,   239,     0,   233,
       0,     0,   164,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   121,     0,   172,   225,   170,
     167,   138,    34,     0,    24,    31,    76,    81,    71,     0,
      62,    63,     0,     0,     0,    50,     0,   208,     0,     0,
     178,   178,   176,    89,    87,   110,   103,   100,     0,     0,
     109,     0,     0,    96,    97,     0,   102,     0,    95,    94,
      93,    92,     0,     0,     0,     0,    69,   121,     0,    66,
     241,    67,     0,     0,   234,   235,   163,   243,   122,   143,
     162,   161,   152,   153,   156,   155,   159,   160,   154,   144,
     145,   147,   142,   157,   158,   146,   148,   149,   150,   151,
       0,   139,     0,    35,    59,     0,    70,   104,   205,   200,
     203,   192,   204,   188,     0,   214,     0,   189,     0,   179,
       0,     0,   177,   175,   127,   126,     0,   105,   128,    88,
     103,   221,    91,     0,     0,   101,    98,    99,   213,    86,
      90,   132,     0,   242,   230,   121,   237,   232,     0,   124,
     141,     0,    60,    55,    64,   196,     0,     0,   166,     0,
     209,   170,     0,   173,   174,   116,   117,   113,   114,   115,
     121,     0,   106,   222,   224,    68,     0,   236,     0,   123,
     226,     0,   201,     0,   211,   193,   165,   215,     0,   190,
     111,     0,     0,   238,   125,     0,     0,     0,     0,   217,
     210,     0,   108,   107,     0,   202,   121,   212,   218,   216,
     180,   197,     0,     0,     0,     0,     0,   180,     0,     0,
     207,   219,   251,   186,     0,   185,   184,   191,   181,   182,
     110,   195,   194,   198,     0,   183,     0,   187,   199
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -278,  -278,  -278,  -278,   316,  -278,  -278,  -278,  -278,  -278,
    -278,  -278,  -278,  -278,  -278,   288,  -278,  -278,   212,  -278,
    -278,   339,  -278,  -278,   312,  -278,  -278,  -278,   -36,  -114,
    -278,  -278,   293,  -278,    -8,  -278,  -278,  -278,  -219,  -278,
      88,   335,  -278,  -278,  -278,   226,  -278,  -278,  -278,  -278,
     302,  -278,  -277,  -278,  -278,   -16,  -278,  -278,  -278,  -116,
     -73,  -278,  -278,  -278,  -278,   -14,   244,  -278,  -278,  -107,
      93,  -267,  -278,  -278,  -278,  -278,  -278,   170,    -7,  -278,
    -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,
    -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,
    -278,  -249,    14,  -278,  -278,  -278,  -278,  -278,  -209,    63,
    -136,  -278,  -278,  -278,  -278,  -278,  -278,  -278,  -278,    72,
    -278,  -278,    77,  -278,   -37,  -278,  -278,  -278,     9,  -278,
     330
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    17,    31,    32,    51,     5,     7,    10,
      11,    24,    25,    26,    40,    60,    61,    76,    77,    78,
     137,    68,    19,    27,    28,    29,   143,    89,    47,   121,
     104,    69,    70,    20,   300,   323,   199,   200,   238,   239,
     201,    33,    34,    48,    97,    98,    52,    66,    53,    54,
     217,   148,   218,   219,   362,   220,   296,   221,   340,    74,
     240,   123,   319,   349,   298,   222,   124,   125,   126,   127,
     285,   128,   129,   223,   224,   225,   293,   290,   386,   387,
     388,   394,   226,   227,   228,   371,   229,   327,   403,   280,
     351,   389,   406,   325,   366,   281,   282,   367,   230,   286,
     358,   355,   309,   231,   329,   369,   379,   383,   232,   344,
     130,   235,   272,   131,   158,   132,   159,   243,   244,   245,
     346,   133,   241,   134,    37,    57,    90,    91,    92,    93,
     135
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     122,    58,   156,   157,    62,   162,   151,   152,   153,   301,
      36,     4,   233,   160,   278,   -50,   312,   102,     6,    -6,
      71,   332,   154,   142,    62,   103,    81,   154,    30,     8,
      13,   101,    44,    45,   215,    46,    22,    23,    14,    82,
      83,    84,    85,  -240,    86,    87,   392,    15,   393,   155,
     354,   192,   354,   321,   155,     3,   248,   249,   250,   251,
     252,   253,   254,   255,   256,   257,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,    72,   363,
     357,   354,   233,   161,    16,    88,   279,    95,   203,     9,
      96,  -240,    88,    12,   283,   204,    21,   193,    35,   297,
      41,    35,    38,   205,   215,   292,   354,   100,    35,   370,
     202,   111,   270,    35,   198,   206,    39,   207,   208,   303,
     304,   185,   186,    43,   209,  -220,  -220,   210,   211,   212,
     155,    42,   203,    50,   391,   384,   385,   294,   295,   204,
     216,    30,    56,    59,   368,    35,    64,   205,   294,   295,
    -231,   117,    65,    14,   118,   111,   213,   214,  -220,   206,
      67,   207,   208,    73,   313,    75,   276,    73,   209,    79,
     105,   210,   211,   212,   155,   108,   138,   328,   107,   399,
     189,    82,    83,   110,    85,   136,    86,    87,   111,  -112,
    -112,   233,   139,   233,   140,   117,   341,   141,   118,   142,
     213,   214,   -87,   144,   146,   145,   149,   190,   335,   336,
     337,   338,   339,   215,   -49,   215,   147,   233,   150,   187,
     188,   194,   233,   196,   234,   242,   247,    88,   117,   271,
    -240,   118,   273,   274,   119,   275,   277,   287,   202,   215,
     289,   364,   326,   173,   215,   372,   373,   233,   284,  -231,
     375,   308,   233,   182,   183,   184,   185,   186,   374,   288,
     299,   311,   236,   302,   305,   306,   307,   360,   310,   215,
     322,   333,   108,   314,   215,   400,   237,   109,    82,    83,
     110,    85,   315,    86,    87,   111,   318,   316,   173,   317,
     352,   320,   342,   334,   343,   236,   180,   181,   182,   183,
     184,   185,   186,   382,   112,   108,   348,   359,   365,   237,
     109,    82,    83,   110,    85,   378,    86,    87,   111,   113,
     114,  -206,   376,   115,   116,   117,   380,   390,   118,   397,
     396,   119,   404,  -121,   120,   405,   -65,   112,   331,    82,
      83,   110,    85,   407,    86,    87,   408,   203,    49,    80,
      18,   195,   113,   114,   204,    63,   115,   116,   117,    99,
      35,   118,   205,   324,   119,   190,  -121,   120,   -65,    55,
     111,   197,   106,   401,   206,   402,   207,   208,   191,   330,
     398,   291,   377,   209,   350,    88,   210,   211,   212,   155,
     347,   203,   345,   395,    94,     0,     0,     0,   204,     0,
       0,     0,     0,     0,    35,     0,   205,     0,     0,     0,
     117,     0,     0,   118,   111,   213,   214,  -220,   206,     0,
     207,   208,     0,     0,     0,     0,     0,   209,     0,     0,
     210,   211,   212,   155,   108,     0,     0,    35,     0,   109,
      82,    83,   110,    85,     0,    86,    87,   111,     0,     0,
       0,     0,     0,     0,   117,     0,     0,   118,     0,   213,
     353,     0,     0,   108,     0,     0,   112,   237,   109,    82,
      83,   110,    85,     0,    86,    87,   111,     0,     0,     0,
       0,   113,   114,     0,     0,   115,   116,   117,     0,     0,
     118,     0,   108,   119,     0,   112,   120,   109,    82,    83,
     110,    85,     0,    86,    87,   111,     0,     0,     0,     0,
     113,   114,   167,   168,   115,   116,   117,     0,   173,   118,
       0,     0,   119,     0,   112,   120,   180,   181,   182,   183,
     184,   185,   186,     0,   163,     0,     0,     0,     0,   113,
     114,     0,     0,   115,   116,   117,     0,     0,   118,     0,
       0,   119,     0,   164,   120,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,     0,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   164,     0,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,     0,     0,     0,     0,     0,   164,   361,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,     0,     0,     0,     0,   164,   246,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,     0,     0,     0,     0,   164,   356,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
       0,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   381,   164,     0,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   164,     0,   165,
     166,   167,   168,     0,     0,   171,   172,   173,   174,   175,
     176,     0,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   164,     0,   165,   166,   167,   168,     0,     0,
     171,   172,   173,     0,     0,     0,     0,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,  -241,     0,  -241,
    -241,   167,   168,     0,     0,   171,   172,   173,     0,     0,
       0,     0,  -241,   178,   179,   180,   181,   182,   183,   184,
     185,   186
};

static const yytype_int16 yycheck[] =
{
      73,    37,   116,   117,    41,   121,   113,   114,   115,   218,
      18,     3,   148,   120,     1,    45,   235,    16,    16,     0,
      17,   288,    16,    17,    61,    24,    62,    16,     9,    69,
       4,    68,    16,    17,   148,    19,     5,     6,    12,    17,
      18,    19,    20,    73,    22,    23,    17,    21,    19,    43,
     327,    17,   329,   272,    43,     0,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,    75,   346,
     329,   358,   218,   120,    58,    63,    73,    16,     1,     7,
      19,    73,    63,    19,    76,     8,    69,    63,    14,   215,
      70,    14,    69,    16,   218,   212,   383,    13,    14,   358,
     147,    24,   185,    14,    15,    28,    69,    30,    31,    67,
      68,    67,    68,    72,    37,    38,    39,    40,    41,    42,
      43,    69,     1,    10,   383,    38,    39,    26,    27,     8,
     148,     9,    67,    70,   353,    14,    45,    16,    26,    27,
      19,    64,    73,    12,    67,    24,    69,    70,    71,    28,
      69,    30,    31,    45,   237,    16,   202,    45,    37,    71,
      70,    40,    41,    42,    43,    11,    71,   284,    75,   388,
      16,    17,    18,    19,    20,    45,    22,    23,    24,    67,
      68,   327,    72,   329,    16,    64,   303,    69,    67,    17,
      69,    70,    71,    45,    74,    72,    14,    43,    32,    33,
      34,    35,    36,   327,    45,   329,    73,   353,    73,    19,
      73,    16,   358,    19,    73,    19,    74,    63,    64,    16,
      73,    67,    17,    74,    70,    72,    69,    16,   275,   353,
      17,   348,   279,    54,   358,   361,   362,   383,    73,    19,
     366,    71,   388,    64,    65,    66,    67,    68,   365,    73,
      69,    74,     1,    69,    69,    69,    69,   340,    69,   383,
      19,    69,    11,    75,   388,   389,    15,    16,    17,    18,
      19,    20,    72,    22,    23,    24,    72,    76,    54,    71,
     326,    75,    16,    69,    74,     1,    62,    63,    64,    65,
      66,    67,    68,   376,    43,    11,    25,    74,    69,    15,
      16,    17,    18,    19,    20,    29,    22,    23,    24,    58,
      59,    76,    76,    62,    63,    64,    70,    74,    67,    71,
      76,    70,    25,    72,    73,    76,    75,    43,    16,    17,
      18,    19,    20,    17,    22,    23,    74,     1,    32,    61,
      11,   139,    58,    59,     8,    43,    62,    63,    64,    66,
      14,    67,    16,   275,    70,    43,    72,    73,    74,    34,
      24,   145,    70,   389,    28,   389,    30,    31,   134,   286,
     387,   211,   368,    37,   321,    63,    40,    41,    42,    43,
     318,     1,   315,   384,    64,    -1,    -1,    -1,     8,    -1,
      -1,    -1,    -1,    -1,    14,    -1,    16,    -1,    -1,    -1,
      64,    -1,    -1,    67,    24,    69,    70,    71,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,
      40,    41,    42,    43,    11,    -1,    -1,    14,    -1,    16,
      17,    18,    19,    20,    -1,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    -1,    -1,    67,    -1,    69,
      70,    -1,    -1,    11,    -1,    -1,    43,    15,    16,    17,
      18,    19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,
      -1,    58,    59,    -1,    -1,    62,    63,    64,    -1,    -1,
      67,    -1,    11,    70,    -1,    43,    73,    16,    17,    18,
      19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,
      58,    59,    48,    49,    62,    63,    64,    -1,    54,    67,
      -1,    -1,    70,    -1,    43,    73,    62,    63,    64,    65,
      66,    67,    68,    -1,    25,    -1,    -1,    -1,    -1,    58,
      59,    -1,    -1,    62,    63,    64,    -1,    -1,    67,    -1,
      -1,    70,    -1,    44,    73,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    -1,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    44,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    44,    75,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    -1,    -1,    -1,    -1,    44,    74,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    -1,    -1,    -1,    -1,    44,    74,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    44,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    44,    -1,    46,
      47,    48,    49,    -1,    -1,    52,    53,    54,    55,    56,
      57,    -1,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    44,    -1,    46,    47,    48,    49,    -1,    -1,
      52,    53,    54,    -1,    -1,    -1,    -1,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    44,    -1,    46,
      47,    48,    49,    -1,    -1,    52,    53,    54,    -1,    -1,
      -1,    -1,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    78,    79,     0,     3,    84,    16,    85,    69,     7,
      86,    87,    19,     4,    12,    21,    58,    80,    98,    99,
     110,    69,     5,     6,    88,    89,    90,   100,   101,   102,
       9,    81,    82,   118,   119,    14,   111,   201,    69,    69,
      91,    70,    69,    72,    16,    17,    19,   105,   120,    81,
      10,    83,   123,   125,   126,   118,    67,   202,   105,    70,
      92,    93,   201,   101,    45,    73,   124,    69,    98,   108,
     109,    17,    75,    45,   136,    16,    94,    95,    96,    71,
      92,   105,    17,    18,    19,    20,    22,    23,    63,   104,
     203,   204,   205,   206,   207,    16,    19,   121,   122,   109,
      13,   201,    16,    24,   107,    70,   127,    75,    11,    16,
      19,    24,    43,    58,    59,    62,    63,    64,    67,    70,
      73,   106,   137,   138,   143,   144,   145,   146,   148,   149,
     187,   190,   192,   198,   200,   207,    45,    97,    71,    72,
      16,    69,    17,   103,    45,    72,    74,    73,   128,    14,
      73,   146,   146,   146,    16,    43,   106,   106,   191,   193,
     146,   201,   136,    25,    44,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    19,    73,    16,
      43,   143,    17,    63,    16,    95,    19,   122,    15,   113,
     114,   117,   201,     1,     8,    16,    28,    30,    31,    37,
      40,    41,    42,    69,    70,   106,   111,   127,   129,   130,
     132,   134,   142,   150,   151,   152,   159,   160,   161,   163,
     175,   180,   185,   187,    73,   188,     1,    15,   115,   116,
     137,   199,    19,   194,   195,   196,    74,    74,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     137,    16,   189,    17,    74,    72,   105,    69,     1,    73,
     166,   172,   173,    76,    73,   147,   176,    16,    73,    17,
     154,   154,   146,   153,    26,    27,   133,   136,   141,    69,
     111,   185,    69,    67,    68,    69,    69,    69,    71,   179,
      69,    74,   115,   137,    75,    72,    76,    71,    72,   139,
      75,   115,    19,   112,   117,   170,   201,   164,   146,   181,
     147,    16,   148,    69,    69,    32,    33,    34,    35,    36,
     135,   146,    16,    74,   186,   199,   197,   196,    25,   140,
     186,   167,   105,    70,   129,   178,    74,   178,   177,    74,
     137,    75,   131,   148,   146,    69,   171,   174,   185,   182,
     178,   162,   136,   136,   146,   136,    76,   179,    29,   183,
      70,    69,   137,   184,    38,    39,   155,   156,   157,   168,
      74,   178,    17,    19,   158,   205,    76,    71,   155,   185,
     106,   132,   142,   165,    25,    76,   169,    17,    74
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    77,    79,    78,    80,    80,    81,    81,    82,    83,
      83,    84,    84,    85,    85,    86,    87,    87,    87,    87,
      88,    88,    89,    91,    90,    92,    92,    93,    94,    94,
      95,    95,    96,    97,    97,    97,    98,    98,    99,   100,
     100,   102,   103,   101,   104,   104,   105,   105,   105,   106,
     106,   107,   107,   108,   108,   109,   110,   111,   111,   112,
     112,   113,   113,   114,   114,   115,   115,   116,   116,   116,
     117,   117,   118,   118,   120,   119,   121,   121,   121,   122,
     122,   122,   124,   123,   126,   125,   127,   128,   128,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   130,   131,   130,   130,   130,
     133,   132,   134,   135,   135,   135,   135,   135,   136,   137,
     137,   138,   139,   138,   140,   140,   141,   141,   142,   143,
     143,   143,   144,   145,   145,   145,   145,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   147,   147,   148,   148,   148,
     148,   149,   149,   150,   151,   152,   153,   153,   154,   154,
     155,   155,   156,   157,   157,   158,   158,   158,   159,   160,
     162,   161,   164,   163,   165,   165,   167,   168,   169,   166,
     170,   171,   170,   172,   172,   172,   174,   173,   176,   177,
     175,   178,   178,   179,   181,   182,   180,   183,   184,   183,
     185,   185,   186,   188,   187,   189,   187,   187,   187,   191,
     190,   193,   192,   194,   194,   195,   195,   197,   196,   198,
     198,   199,   199,   200,   201,   201,   202,   202,   203,   203,
     204,   205,   205,   206,   207,   207,   207,   207
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     6,     0,     1,     0,     2,     2,     2,
       1,     0,     3,     0,     2,     3,     0,     3,     3,     4,
       1,     1,     5,     0,     6,     1,     2,     3,     0,     1,
       1,     3,     2,     0,     2,     3,     0,     1,     2,     1,
       3,     0,     0,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     6,     2,     3,     2,     0,
       1,     0,     1,     1,     3,     0,     1,     1,     3,     1,
       2,     1,     0,     2,     0,     5,     3,     1,     1,     0,
       1,     3,     0,     3,     0,     3,     4,     0,     3,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     2,     1,     1,     2,     2,     0,     5,     5,     1,
       0,     4,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     0,     0,     5,     0,     2,     1,     1,     2,     1,
       1,     1,     4,     2,     2,     2,     2,     1,     2,     3,
       1,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     2,     1,     1,     1,
       1,     1,     2,     3,     3,     2,     0,     1,     0,     1,
       0,     2,     2,     3,     2,     1,     1,     3,     2,     2,
       0,     8,     0,     4,     1,     1,     0,     0,     0,    10,
       0,     0,     4,     1,     1,     1,     0,     7,     0,     0,
       5,     1,     3,     1,     0,     0,     6,     0,     0,     3,
       0,     2,     1,     0,     5,     0,     5,     1,     1,     0,
       4,     0,     4,     0,     1,     1,     3,     0,     4,     2,
       1,     1,     2,     3,     1,     2,     2,     3,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1
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
#line 324 "Parser.y"
                {
				init_header();
			}
#line 2194 "parser.c"
    break;

  case 3: /* file_structure: $@1 namespace prerequisites global_declarations header_delimiter macro_definitions  */
#line 330 "Parser.y"
                                          {
				finalize_header();
			}
#line 2202 "parser.c"
    break;

  case 11: /* namespace: %empty  */
#line 347 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2210 "parser.c"
    break;

  case 12: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 350 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2219 "parser.c"
    break;

  case 15: /* require: T_REQUIRE T_STRING ';'  */
#line 358 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2228 "parser.c"
    break;

  case 22: /* struct_definition: T_STRUCT '{' struct_element_definitions '}' T_IDENT  */
#line 371 "Parser.y"
                                                                       {
				free(yyvsp[0].ident.s);
			}
#line 2236 "parser.c"
    break;

  case 23: /* $@2: %empty  */
#line 375 "Parser.y"
                        { _bInEnumDefinition = 1; }
#line 2242 "parser.c"
    break;

  case 24: /* enum_definition: T_ENUM $@2 '{' enum_value_definitions '}' T_IDENT  */
#line 375 "Parser.y"
                                                                                           {
				_bInEnumDefinition = 0;
				free(yyvsp[0].ident.s);
			}
#line 2251 "parser.c"
    break;

  case 27: /* struct_element: type_name variable_identifier ';'  */
#line 383 "Parser.y"
                                                  {
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
			}
#line 2261 "parser.c"
    break;

  case 32: /* enum_value: T_IDENT opt_enum_assignment  */
#line 395 "Parser.y"
                                            {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=""});
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[-1].ident.s});
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=(int)yyvsp[0].num});
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=_yyCurrentComment});
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
						C_0FUNC, (GENERIC_DATA){ FUNC_RegisterEnumValue}, 4);
				// register right away as well, so we can use it.
				function_registerEnum((const char*) 0, yyvsp[-1].ident.s, (int)yyvsp[0].num, _yyCurrentComment);
				free(yyvsp[-1].ident.s);
			}
#line 2277 "parser.c"
    break;

  case 34: /* opt_enum_assignment: T_ASSIGN T_NUM  */
#line 408 "Parser.y"
                                         {
				yyval.num = yyvsp[0].num;
			}
#line 2285 "parser.c"
    break;

  case 35: /* opt_enum_assignment: T_ASSIGN '-' T_NUM  */
#line 411 "Parser.y"
                                             {
				yyval.num = -yyvsp[0].num;
			}
#line 2293 "parser.c"
    break;

  case 36: /* scope: %empty  */
#line 415 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2299 "parser.c"
    break;

  case 37: /* scope: T_STATIC  */
#line 416 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2305 "parser.c"
    break;

  case 41: /* $@3: %empty  */
#line 426 "Parser.y"
                { _bDefiningConst = 1; }
#line 2311 "parser.c"
    break;

  case 42: /* $@4: %empty  */
#line 426 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2317 "parser.c"
    break;

  case 43: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 427 "Parser.y"
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
#line 2333 "parser.c"
    break;

  case 44: /* constant_literal: simple_literal  */
#line 440 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2341 "parser.c"
    break;

  case 45: /* constant_literal: T_STRING  */
#line 443 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2350 "parser.c"
    break;

  case 46: /* variable_identifier: T_IDENT  */
#line 449 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2360 "parser.c"
    break;

  case 47: /* variable_identifier: T_NUM  */
#line 454 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2371 "parser.c"
    break;

  case 48: /* variable_identifier: T_STRING  */
#line 460 "Parser.y"
                                   {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2380 "parser.c"
    break;

  case 49: /* variable_reference: T_VARIABLE  */
#line 466 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2386 "parser.c"
    break;

  case 50: /* variable_reference: T_IDENT  */
#line 467 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2398 "parser.c"
    break;

  case 51: /* macro_declaration: T_IDENT  */
#line 477 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2406 "parser.c"
    break;

  case 52: /* macro_declaration: T_FUNC  */
#line 480 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2416 "parser.c"
    break;

  case 53: /* macro_type: scope T_VOID  */
#line 486 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
				yyval.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(VT_NIL);
				}
			}
#line 2429 "parser.c"
    break;

  case 54: /* macro_type: scope type_name  */
#line 495 "Parser.y"
                                        {
				yyval.ident.type = yyvsp[0].ident.type;
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(yyvsp[0].ident.type);
				}
			}
#line 2442 "parser.c"
    break;

  case 55: /* macrostart: macro_type macro_declaration '(' argument_list ')' optional_description  */
#line 505 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				yyval.ident.type = yyvsp[-5].ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2452 "parser.c"
    break;

  case 57: /* var_decl: type_name variable_identifier assignment_expression  */
#line 513 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});
			}
#line 2465 "parser.c"
    break;

  case 58: /* var_decl: type_name variable_identifier  */
#line 520 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2476 "parser.c"
    break;

  case 60: /* optional_description: T_STRING  */
#line 529 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2485 "parser.c"
    break;

  case 69: /* non_empty_parameters: error  */
#line 547 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 2493 "parser.c"
    break;

  case 70: /* argument_declaration: type_name variable_identifier  */
#line 551 "Parser.y"
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
#line 2508 "parser.c"
    break;

  case 71: /* argument_declaration: T_ELLIPSIS  */
#line 560 "Parser.y"
                                       {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[0].ident.type, "...");
				}
			}
#line 2518 "parser.c"
    break;

  case 74: /* $@5: %empty  */
#line 569 "Parser.y"
                         {
				start_annotation(yyvsp[0].ident.s);
			}
#line 2526 "parser.c"
    break;

  case 76: /* annotation_value: T_IDENT T_ASSIGN T_STRING  */
#line 573 "Parser.y"
                                            {
				parser_defineAnnotationValue(yyvsp[-2].ident.s, yyvsp[0].ident.s);
			}
#line 2534 "parser.c"
    break;

  case 77: /* annotation_value: T_IDENT  */
#line 575 "Parser.y"
                                    {
				parser_defineAnnotationValue(yyvsp[0].ident.s, _strdup("true"));
			}
#line 2542 "parser.c"
    break;

  case 78: /* annotation_value: T_STRING  */
#line 577 "Parser.y"
                                     {
				parser_defineAnnotationValue(_strdup("value"), yyvsp[0].ident.s);
			}
#line 2550 "parser.c"
    break;

  case 82: /* $@6: %empty  */
#line 586 "Parser.y"
                                         {
					init_native();
				}
#line 2558 "parser.c"
    break;

  case 83: /* native_function_definition: T_NATIVE $@6 macrostart  */
#line 588 "Parser.y"
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
#line 2591 "parser.c"
    break;

  case 84: /* $@7: %empty  */
#line 617 "Parser.y"
                  { init_macroDefinition(); }
#line 2597 "parser.c"
    break;

  case 85: /* macro_definition: $@7 macrostart block  */
#line 618 "Parser.y"
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
#line 2626 "parser.c"
    break;

  case 90: /* statement: call_expression ';'  */
#line 649 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2632 "parser.c"
    break;

  case 91: /* statement: assignment ';'  */
#line 650 "Parser.y"
                                         { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2638 "parser.c"
    break;

  case 92: /* statement: if_expression  */
#line 651 "Parser.y"
                                        { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2644 "parser.c"
    break;

  case 93: /* statement: while  */
#line 652 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2650 "parser.c"
    break;

  case 94: /* statement: for_loop_expression  */
#line 653 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2656 "parser.c"
    break;

  case 95: /* statement: switch_expression  */
#line 654 "Parser.y"
                                            { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2662 "parser.c"
    break;

  case 100: /* statement: block  */
#line 659 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2668 "parser.c"
    break;

  case 101: /* statement: increment_expression ';'  */
#line 660 "Parser.y"
                                                   { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2674 "parser.c"
    break;

  case 103: /* statement: var_decl  */
#line 662 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2682 "parser.c"
    break;

  case 104: /* statement: error ';'  */
#line 665 "Parser.y"
                                    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2691 "parser.c"
    break;

  case 105: /* assignment: variable_reference assignment_expression  */
#line 670 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2699 "parser.c"
    break;

  case 106: /* $@8: %empty  */
#line 674 "Parser.y"
                                                      {
				parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2707 "parser.c"
    break;

  case 107: /* assignment: assignment_target '.' T_IDENT $@8 assignment_expression  */
#line 676 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2715 "parser.c"
    break;

  case 108: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 680 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2723 "parser.c"
    break;

  case 110: /* $@9: %empty  */
#line 686 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2731 "parser.c"
    break;

  case 111: /* shorthand_assignment: variable_reference $@9 shorthand_assignment_operator expression  */
#line 688 "Parser.y"
                                                                   {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2740 "parser.c"
    break;

  case 112: /* assignment_target: variable_reference  */
#line 693 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2748 "parser.c"
    break;

  case 113: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 698 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2754 "parser.c"
    break;

  case 114: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 699 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2760 "parser.c"
    break;

  case 115: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 700 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2766 "parser.c"
    break;

  case 116: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 701 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2772 "parser.c"
    break;

  case 117: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 702 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2778 "parser.c"
    break;

  case 118: /* assignment_expression: T_ASSIGN expression  */
#line 704 "Parser.y"
                                                { yyval.v = yyvsp[0].v;	}
#line 2784 "parser.c"
    break;

  case 119: /* expression: binary_expression  */
#line 706 "Parser.y"
                              { yyval.v = yyvsp[0].v; }
#line 2790 "parser.c"
    break;

  case 122: /* $@10: %empty  */
#line 710 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2796 "parser.c"
    break;

  case 125: /* range_increment: T_DOTDOT binary_expression  */
#line 713 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2802 "parser.c"
    break;

  case 126: /* minusminus_plusplus: T_MINUSMINUS  */
#line 716 "Parser.y"
                                     { yyval.num = -1; }
#line 2808 "parser.c"
    break;

  case 127: /* minusminus_plusplus: T_PLUSPLUS  */
#line 717 "Parser.y"
                                     { yyval.num = 1; }
#line 2814 "parser.c"
    break;

  case 128: /* increment_expression: variable_reference minusminus_plusplus  */
#line 720 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2822 "parser.c"
    break;

  case 129: /* simple_term: value  */
#line 724 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2828 "parser.c"
    break;

  case 132: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 728 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2836 "parser.c"
    break;

  case 133: /* unary_expression: '!' binary_expression  */
#line 732 "Parser.y"
                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2842 "parser.c"
    break;

  case 134: /* unary_expression: '~' binary_expression  */
#line 733 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2848 "parser.c"
    break;

  case 135: /* unary_expression: '+' binary_expression  */
#line 734 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2854 "parser.c"
    break;

  case 136: /* unary_expression: '-' variable_reference  */
#line 735 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2860 "parser.c"
    break;

  case 137: /* binary_expression: simple_term  */
#line 738 "Parser.y"
                                    { yyval.ident.type = yyvsp[0].ident.type;}
#line 2866 "parser.c"
    break;

  case 138: /* binary_expression: type_cast simple_term  */
#line 739 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2875 "parser.c"
    break;

  case 139: /* binary_expression: binary_expression '.' T_IDENT  */
#line 744 "Parser.y"
                                                      {
				yyval.ident.type = parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2884 "parser.c"
    break;

  case 141: /* binary_expression: binary_expression '[' expression ']'  */
#line 749 "Parser.y"
                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2890 "parser.c"
    break;

  case 142: /* binary_expression: binary_expression '~' binary_expression  */
#line 750 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2896 "parser.c"
    break;

  case 143: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 751 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2902 "parser.c"
    break;

  case 144: /* binary_expression: binary_expression '&' binary_expression  */
#line 752 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2908 "parser.c"
    break;

  case 145: /* binary_expression: binary_expression '|' binary_expression  */
#line 753 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2914 "parser.c"
    break;

  case 146: /* binary_expression: binary_expression '+' binary_expression  */
#line 754 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2920 "parser.c"
    break;

  case 147: /* binary_expression: binary_expression '^' binary_expression  */
#line 755 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2926 "parser.c"
    break;

  case 148: /* binary_expression: binary_expression '-' binary_expression  */
#line 756 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2932 "parser.c"
    break;

  case 149: /* binary_expression: binary_expression '*' binary_expression  */
#line 757 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2938 "parser.c"
    break;

  case 150: /* binary_expression: binary_expression '/' binary_expression  */
#line 758 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2944 "parser.c"
    break;

  case 151: /* binary_expression: binary_expression '%' binary_expression  */
#line 759 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2950 "parser.c"
    break;

  case 152: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 760 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2956 "parser.c"
    break;

  case 153: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 761 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2962 "parser.c"
    break;

  case 154: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 762 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2968 "parser.c"
    break;

  case 155: /* binary_expression: binary_expression T_AND binary_expression  */
#line 763 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2974 "parser.c"
    break;

  case 156: /* binary_expression: binary_expression T_OR binary_expression  */
#line 764 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2980 "parser.c"
    break;

  case 157: /* binary_expression: binary_expression '<' binary_expression  */
#line 765 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2986 "parser.c"
    break;

  case 158: /* binary_expression: binary_expression '>' binary_expression  */
#line 766 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2992 "parser.c"
    break;

  case 159: /* binary_expression: binary_expression T_LE binary_expression  */
#line 767 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2998 "parser.c"
    break;

  case 160: /* binary_expression: binary_expression T_GE binary_expression  */
#line 768 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 3004 "parser.c"
    break;

  case 161: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 769 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 3010 "parser.c"
    break;

  case 162: /* binary_expression: binary_expression T_NE binary_expression  */
#line 770 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 3016 "parser.c"
    break;

  case 163: /* binary_expression: '(' binary_expression ')'  */
#line 771 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 3022 "parser.c"
    break;

  case 164: /* binary_expression: variable_reference assignment_expression  */
#line 772 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 3030 "parser.c"
    break;

  case 166: /* condition: '(' binary_expression  */
#line 779 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 3038 "parser.c"
    break;

  case 167: /* value: T_VARIABLE  */
#line 783 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 3047 "parser.c"
    break;

  case 168: /* value: string  */
#line 787 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 3057 "parser.c"
    break;

  case 169: /* value: simple_literal  */
#line 792 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 3063 "parser.c"
    break;

  case 170: /* value: T_IDENT  */
#line 793 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 3073 "parser.c"
    break;

  case 171: /* string: T_STRING  */
#line 799 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 3079 "parser.c"
    break;

  case 172: /* string: string T_STRING  */
#line 800 "Parser.y"
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
#line 3094 "parser.c"
    break;

  case 173: /* break: T_BREAK opt_num ';'  */
#line 812 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 3100 "parser.c"
    break;

  case 174: /* continue: T_CONTINUE opt_num ';'  */
#line 814 "Parser.y"
                                       {
				// in the case this is a continue in a for(int i;b;i++) loop, generate the i++ code before continuing.
				parser_flushBuffer(_breaklevel-(int)yyvsp[-1].num+1);
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); 
			}
#line 3110 "parser.c"
    break;

  case 175: /* return_expression: T_RETURN optional_bterm  */
#line 821 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 3118 "parser.c"
    break;

  case 178: /* opt_num: %empty  */
#line 828 "Parser.y"
                {	yyval.num = 1;	}
#line 3124 "parser.c"
    break;

  case 179: /* opt_num: T_NUM  */
#line 829 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 3130 "parser.c"
    break;

  case 184: /* case_selector: T_DEFAULT ':'  */
#line 838 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 3138 "parser.c"
    break;

  case 185: /* case_condition: integer_literal  */
#line 843 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 3146 "parser.c"
    break;

  case 186: /* case_condition: T_STRING  */
#line 846 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3154 "parser.c"
    break;

  case 187: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 849 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3162 "parser.c"
    break;

  case 188: /* label: T_IDENT ':'  */
#line 853 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3171 "parser.c"
    break;

  case 189: /* goto_expression: T_GOTO T_IDENT  */
#line 859 "Parser.y"
                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3180 "parser.c"
    break;

  case 190: /* $@11: %empty  */
#line 866 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3190 "parser.c"
    break;

  case 191: /* switch_expression: T_SWITCH '(' value ')' $@11 '{' case_clauses '}'  */
#line 872 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3203 "parser.c"
    break;

  case 192: /* $@12: %empty  */
#line 882 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3212 "parser.c"
    break;

  case 193: /* for_loop_expression: T_FOR for_clause $@12 local_block  */
#line 885 "Parser.y"
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
#line 3229 "parser.c"
    break;

  case 196: /* $@13: %empty  */
#line 901 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3237 "parser.c"
    break;

  case 197: /* $@14: %empty  */
#line 903 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3245 "parser.c"
    break;

  case 198: /* $@15: %empty  */
#line 905 "Parser.y"
                                        {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			}
#line 3254 "parser.c"
    break;

  case 199: /* for_to_clause: '(' opt_for_initializer $@13 ';' binary_expression ';' $@14 for_increment $@15 ')'  */
#line 908 "Parser.y"
                              {
				yyval.ident.s = yyvsp[-8].ident.s;
			}
#line 3262 "parser.c"
    break;

  case 201: /* $@16: %empty  */
#line 913 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3270 "parser.c"
    break;

  case 202: /* opt_for_initializer: type_name variable_identifier $@16 assignment_expression  */
#line 915 "Parser.y"
                                                {
				yyval.ident.s = yyvsp[-2].ident.s;
			}
#line 3278 "parser.c"
    break;

  case 203: /* for_clause: for_to_clause  */
#line 919 "Parser.y"
                              {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3286 "parser.c"
    break;

  case 204: /* for_clause: in_clause  */
#line 922 "Parser.y"
                                    {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3294 "parser.c"
    break;

  case 205: /* for_clause: error  */
#line 925 "Parser.y"
                                {
				yyval.ident.s = 0;
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3304 "parser.c"
    break;

  case 206: /* $@17: %empty  */
#line 931 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				parser_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3315 "parser.c"
    break;

  case 207: /* in_clause: '(' type_name variable_identifier $@17 ':' expression ')'  */
#line 936 "Parser.y"
                                             {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
				yyval.ident.s = yyvsp[-4].ident.s;
			}
#line 3327 "parser.c"
    break;

  case 208: /* $@18: %empty  */
#line 944 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3335 "parser.c"
    break;

  case 209: /* $@19: %empty  */
#line 946 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3344 "parser.c"
    break;

  case 210: /* while: T_WHILE $@18 condition $@19 local_block  */
#line 949 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3356 "parser.c"
    break;

  case 214: /* $@20: %empty  */
#line 965 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3365 "parser.c"
    break;

  case 215: /* $@21: %empty  */
#line 968 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3373 "parser.c"
    break;

  case 216: /* if_expression: T_IF condition $@20 local_block $@21 else_clause  */
#line 970 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3382 "parser.c"
    break;

  case 217: /* else_clause: %empty  */
#line 975 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3391 "parser.c"
    break;

  case 218: /* $@22: %empty  */
#line 979 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3401 "parser.c"
    break;

  case 223: /* @23: %empty  */
#line 991 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3413 "parser.c"
    break;

  case 224: /* call_expression: T_FUNC '(' @23 parameter_list closing_paren  */
#line 997 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3423 "parser.c"
    break;

  case 225: /* @24: %empty  */
#line 1002 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3434 "parser.c"
    break;

  case 226: /* call_expression: function_id_or_pointer '(' @24 parameter_list closing_paren  */
#line 1007 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3448 "parser.c"
    break;

  case 229: /* @25: %empty  */
#line 1019 "Parser.y"
                       {
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = &_functionTable[FUNC_CreateArray];
			}
#line 3458 "parser.c"
    break;

  case 230: /* array_constructor: '[' @25 parameter_list ']'  */
#line 1023 "Parser.y"
                                             {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
					C_0FUNC,(GENERIC_DATA){FUNC_CreateArray}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
			}
#line 3468 "parser.c"
    break;

  case 231: /* @26: %empty  */
#line 1029 "Parser.y"
                     {
		_currentFunctionCallParamIndexP++;
		*_currentFunctionCallParamIndexP = 0;
		yyval.funcp = &_functionTable[FUNC_CreateMap];
	}
#line 3478 "parser.c"
    break;

  case 232: /* map_constructor: '{' @26 optional_map_associates '}'  */
#line 1033 "Parser.y"
                                      {
		_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
			C_0FUNC,(GENERIC_DATA){FUNC_CreateMap}, *_currentFunctionCallParamIndexP);
		_currentFunctionCallParamIndexP--;
	}
#line 3488 "parser.c"
    break;

  case 237: /* @27: %empty  */
#line 1049 "Parser.y"
                                     { 
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[-1].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[-1].ident.s);
				(*_currentFunctionCallParamIndexP) += 2;
			}
#line 3499 "parser.c"
    break;

  case 239: /* function_id_or_pointer: '*' variable_reference  */
#line 1057 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3513 "parser.c"
    break;

  case 240: /* function_id_or_pointer: T_IDENT  */
#line 1067 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3522 "parser.c"
    break;

  case 241: /* parameter: expression  */
#line 1072 "Parser.y"
                           { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3528 "parser.c"
    break;

  case 242: /* parameter: T_ELLIPSIS expression  */
#line 1073 "Parser.y"
                                                { 
				yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; 
				YY_EMIT(C_SPREAD, (GENERIC_DATA){1});
			}
#line 3537 "parser.c"
    break;

  case 243: /* type_cast: '(' type_name ')'  */
#line 1078 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3546 "parser.c"
    break;

  case 244: /* type_name: T_TYPE_IDENTIFIER  */
#line 1083 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3554 "parser.c"
    break;

  case 245: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 1086 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3563 "parser.c"
    break;

  case 247: /* array_size: '[' T_NUM ']'  */
#line 1092 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3571 "parser.c"
    break;

  case 248: /* boolean_literal: T_TRUE  */
#line 1096 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3580 "parser.c"
    break;

  case 249: /* boolean_literal: T_FALSE  */
#line 1099 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3589 "parser.c"
    break;

  case 250: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 1104 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3598 "parser.c"
    break;

  case 251: /* integer_literal: T_NUM  */
#line 1109 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3607 "parser.c"
    break;

  case 252: /* integer_literal: '-' T_NUM  */
#line 1113 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3617 "parser.c"
    break;

  case 253: /* character_literal: T_CHARACTER  */
#line 1120 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3626 "parser.c"
    break;

  case 254: /* simple_literal: integer_literal  */
#line 1125 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3639 "parser.c"
    break;

  case 255: /* simple_literal: float_literal  */
#line 1133 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3652 "parser.c"
    break;

  case 256: /* simple_literal: character_literal  */
#line 1141 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3665 "parser.c"
    break;

  case 257: /* simple_literal: boolean_literal  */
#line 1149 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3678 "parser.c"
    break;


#line 3682 "parser.c"

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

#line 1157 "Parser.y"


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

