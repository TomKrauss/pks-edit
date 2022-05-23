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

static void parser_startTypeDescriptor(int bEnum) {
	_currentTypeDescriptor = calloc(1, sizeof *_currentTypeDescriptor);
	_currentTypeDescriptor->ptd_isEnumType = bEnum;
	_currentTypeDescriptor->ptd_documentation = _yyCurrentComment;
	_currentTypeDescriptor->ptd_elements.ptd_properties = calloc(MAX_TYPE_PROPERTIES, sizeof (PARAMETER_ENUM_VALUE));
	_yyCurrentComment = 0;
}

static void parser_finalizeTypeDescriptor(char* pszTypename) {
	_currentTypeDescriptor->ptd_name = pszTypename;
	types_register(-1, _currentTypeDescriptor);
	int nParams = 4;
	YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=pszTypename});
	YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(char*)_currentTypeDescriptor->ptd_documentation});
	YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_currentTypeDescriptor->ptd_isEnumType});
	YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_currentTypeDescriptor->ptd_numberOfProperties});
	for (int i = 0; i < _currentTypeDescriptor->ptd_numberOfProperties; i++) {
		TYPE_PROPERTY_DESCRIPTOR* pDescriptor = &_currentTypeDescriptor->ptd_elements.ptd_properties[i];
		YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(char*)pDescriptor->tpd_name});
		YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(char*)pDescriptor->tpd_documentation});
		YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.longValue=(long)pDescriptor->tpd_type});
		nParams += 3;
	}
	_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
			C_0FUNC, (GENERIC_DATA){ FUNC_RegisterType}, nParams);
	types_destroyDescriptor(_currentTypeDescriptor);
	_currentTypeDescriptor = 0;
}

static void parser_addTypeProperty(char* pszName, int nValue) {
	if (_currentTypeDescriptor->ptd_numberOfProperties >= MAX_TYPE_PROPERTIES) {
		yyerror("Type too complex - to many struct elements / enum values.");
		return;
	}
	int i = _currentTypeDescriptor->ptd_numberOfProperties++;
	TYPE_PROPERTY_DESCRIPTOR* pDescriptor = &_currentTypeDescriptor->ptd_elements.ptd_properties[i];
	pDescriptor->tpd_name = pszName;
	pDescriptor->tpd_documentation = _yyCurrentComment;
	_yyCurrentComment = 0;
	pDescriptor->tpd_type = nValue;
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

static int parser_resetIfDependencies() {
	if (_compilerConfiguration->cb_numberOfDependencies != arraylist_size(_compilerConfiguration->cb_dependencies)) {
		_compilerConfiguration->cb_postponed = 1;
		yyrequire(_compilerConfiguration->cb_source);
		parser_switchToBuffer(-1);
		return 1;
	}
	return 0;
}


#line 418 "parser.c"

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
  YYSYMBOL_80_2 = 80,                      /* $@2  */
  YYSYMBOL_header_delimiter = 81,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 82,         /* macro_definitions  */
  YYSYMBOL_element_with_annotations = 83,  /* element_with_annotations  */
  YYSYMBOL_macro_or_native_function = 84,  /* macro_or_native_function  */
  YYSYMBOL_namespace = 85,                 /* namespace  */
  YYSYMBOL_prerequisites = 86,             /* prerequisites  */
  YYSYMBOL_require = 87,                   /* require  */
  YYSYMBOL_global_declarations = 88,       /* global_declarations  */
  YYSYMBOL_typedef = 89,                   /* typedef  */
  YYSYMBOL_struct_definition = 90,         /* struct_definition  */
  YYSYMBOL_91_3 = 91,                      /* $@3  */
  YYSYMBOL_enum_definition = 92,           /* enum_definition  */
  YYSYMBOL_93_4 = 93,                      /* $@4  */
  YYSYMBOL_native_type_definition = 94,    /* native_type_definition  */
  YYSYMBOL_95_5 = 95,                      /* $@5  */
  YYSYMBOL_typedef_identifier = 96,        /* typedef_identifier  */
  YYSYMBOL_struct_element_definitions = 97, /* struct_element_definitions  */
  YYSYMBOL_struct_element = 98,            /* struct_element  */
  YYSYMBOL_enum_value_definitions = 99,    /* enum_value_definitions  */
  YYSYMBOL_enum_values = 100,              /* enum_values  */
  YYSYMBOL_enum_value = 101,               /* enum_value  */
  YYSYMBOL_opt_enum_assignment = 102,      /* opt_enum_assignment  */
  YYSYMBOL_scope = 103,                    /* scope  */
  YYSYMBOL_constants = 104,                /* constants  */
  YYSYMBOL_constant_list = 105,            /* constant_list  */
  YYSYMBOL_constdef = 106,                 /* constdef  */
  YYSYMBOL_107_6 = 107,                    /* $@6  */
  YYSYMBOL_108_7 = 108,                    /* $@7  */
  YYSYMBOL_constant_literal = 109,         /* constant_literal  */
  YYSYMBOL_variable_identifier = 110,      /* variable_identifier  */
  YYSYMBOL_variable_reference = 111,       /* variable_reference  */
  YYSYMBOL_macro_declaration = 112,        /* macro_declaration  */
  YYSYMBOL_macro_type = 113,               /* macro_type  */
  YYSYMBOL_macrostart = 114,               /* macrostart  */
  YYSYMBOL_global_var = 115,               /* global_var  */
  YYSYMBOL_var_decl = 116,                 /* var_decl  */
  YYSYMBOL_optional_description = 117,     /* optional_description  */
  YYSYMBOL_argument_list = 118,            /* argument_list  */
  YYSYMBOL_non_empty_arguments = 119,      /* non_empty_arguments  */
  YYSYMBOL_parameter_list = 120,           /* parameter_list  */
  YYSYMBOL_non_empty_parameters = 121,     /* non_empty_parameters  */
  YYSYMBOL_argument_declaration = 122,     /* argument_declaration  */
  YYSYMBOL_annotations = 123,              /* annotations  */
  YYSYMBOL_annotation = 124,               /* annotation  */
  YYSYMBOL_125_8 = 125,                    /* $@8  */
  YYSYMBOL_annotation_value = 126,         /* annotation_value  */
  YYSYMBOL_annotation_values = 127,        /* annotation_values  */
  YYSYMBOL_native_function_definition = 128, /* native_function_definition  */
  YYSYMBOL_129_9 = 129,                    /* $@9  */
  YYSYMBOL_macro_definition = 130,         /* macro_definition  */
  YYSYMBOL_131_10 = 131,                   /* $@10  */
  YYSYMBOL_block = 132,                    /* block  */
  YYSYMBOL_var_decls = 133,                /* var_decls  */
  YYSYMBOL_statement = 134,                /* statement  */
  YYSYMBOL_assignment = 135,               /* assignment  */
  YYSYMBOL_136_11 = 136,                   /* $@11  */
  YYSYMBOL_shorthand_assignment = 137,     /* shorthand_assignment  */
  YYSYMBOL_138_12 = 138,                   /* $@12  */
  YYSYMBOL_assignment_target = 139,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 140, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 141,    /* assignment_expression  */
  YYSYMBOL_expression = 142,               /* expression  */
  YYSYMBOL_range_expression = 143,         /* range_expression  */
  YYSYMBOL_144_13 = 144,                   /* $@13  */
  YYSYMBOL_range_increment = 145,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 146,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 147,     /* increment_expression  */
  YYSYMBOL_simple_term = 148,              /* simple_term  */
  YYSYMBOL_constructor_expression = 149,   /* constructor_expression  */
  YYSYMBOL_unary_expression = 150,         /* unary_expression  */
  YYSYMBOL_binary_expression = 151,        /* binary_expression  */
  YYSYMBOL_condition = 152,                /* condition  */
  YYSYMBOL_value = 153,                    /* value  */
  YYSYMBOL_string = 154,                   /* string  */
  YYSYMBOL_break = 155,                    /* break  */
  YYSYMBOL_continue = 156,                 /* continue  */
  YYSYMBOL_return_expression = 157,        /* return_expression  */
  YYSYMBOL_optional_bterm = 158,           /* optional_bterm  */
  YYSYMBOL_opt_num = 159,                  /* opt_num  */
  YYSYMBOL_case_clauses = 160,             /* case_clauses  */
  YYSYMBOL_case_clause = 161,              /* case_clause  */
  YYSYMBOL_case_selector = 162,            /* case_selector  */
  YYSYMBOL_case_condition = 163,           /* case_condition  */
  YYSYMBOL_label = 164,                    /* label  */
  YYSYMBOL_goto_expression = 165,          /* goto_expression  */
  YYSYMBOL_switch_expression = 166,        /* switch_expression  */
  YYSYMBOL_167_14 = 167,                   /* $@14  */
  YYSYMBOL_for_loop_expression = 168,      /* for_loop_expression  */
  YYSYMBOL_169_15 = 169,                   /* $@15  */
  YYSYMBOL_for_increment = 170,            /* for_increment  */
  YYSYMBOL_for_to_clause = 171,            /* for_to_clause  */
  YYSYMBOL_172_16 = 172,                   /* $@16  */
  YYSYMBOL_173_17 = 173,                   /* $@17  */
  YYSYMBOL_174_18 = 174,                   /* $@18  */
  YYSYMBOL_opt_for_initializer = 175,      /* opt_for_initializer  */
  YYSYMBOL_176_19 = 176,                   /* $@19  */
  YYSYMBOL_for_clause = 177,               /* for_clause  */
  YYSYMBOL_in_clause = 178,                /* in_clause  */
  YYSYMBOL_179_20 = 179,                   /* $@20  */
  YYSYMBOL_while = 180,                    /* while  */
  YYSYMBOL_181_21 = 181,                   /* $@21  */
  YYSYMBOL_182_22 = 182,                   /* $@22  */
  YYSYMBOL_local_block = 183,              /* local_block  */
  YYSYMBOL_closing_brace = 184,            /* closing_brace  */
  YYSYMBOL_if_expression = 185,            /* if_expression  */
  YYSYMBOL_186_23 = 186,                   /* $@23  */
  YYSYMBOL_187_24 = 187,                   /* $@24  */
  YYSYMBOL_else_clause = 188,              /* else_clause  */
  YYSYMBOL_189_25 = 189,                   /* $@25  */
  YYSYMBOL_stmntlist = 190,                /* stmntlist  */
  YYSYMBOL_closing_paren = 191,            /* closing_paren  */
  YYSYMBOL_call_expression = 192,          /* call_expression  */
  YYSYMBOL_193_26 = 193,                   /* @26  */
  YYSYMBOL_194_27 = 194,                   /* @27  */
  YYSYMBOL_array_constructor = 195,        /* array_constructor  */
  YYSYMBOL_196_28 = 196,                   /* @28  */
  YYSYMBOL_map_constructor = 197,          /* map_constructor  */
  YYSYMBOL_198_29 = 198,                   /* @29  */
  YYSYMBOL_optional_map_associates = 199,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 200,           /* map_associates  */
  YYSYMBOL_map_associate = 201,            /* map_associate  */
  YYSYMBOL_202_30 = 202,                   /* @30  */
  YYSYMBOL_function_id_or_pointer = 203,   /* function_id_or_pointer  */
  YYSYMBOL_parameter = 204,                /* parameter  */
  YYSYMBOL_type_cast = 205,                /* type_cast  */
  YYSYMBOL_type_name = 206,                /* type_name  */
  YYSYMBOL_array_size = 207,               /* array_size  */
  YYSYMBOL_boolean_literal = 208,          /* boolean_literal  */
  YYSYMBOL_float_literal = 209,            /* float_literal  */
  YYSYMBOL_integer_literal = 210,          /* integer_literal  */
  YYSYMBOL_character_literal = 211,        /* character_literal  */
  YYSYMBOL_simple_literal = 212            /* simple_literal  */
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
#define YYLAST   828

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  136
/* YYNRULES -- Number of rules.  */
#define YYNRULES  264
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  417

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
       0,   375,   375,   378,   375,   389,   390,   392,   393,   396,
     399,   400,   402,   405,   410,   411,   413,   418,   419,   420,
     421,   423,   424,   425,   427,   427,   429,   429,   431,   431,
     433,   437,   441,   442,   444,   448,   449,   451,   452,   454,
     458,   459,   462,   466,   467,   471,   474,   475,   477,   477,
     477,   491,   494,   500,   505,   511,   517,   518,   528,   531,
     537,   546,   555,   562,   564,   571,   578,   580,   585,   586,
     589,   591,   593,   594,   596,   597,   598,   602,   611,   617,
     618,   620,   620,   624,   626,   628,   632,   633,   634,   637,
     637,   668,   668,   694,   696,   697,   699,   700,   701,   702,
     703,   704,   705,   706,   707,   708,   709,   710,   711,   712,
     713,   716,   721,   725,   725,   731,   734,   737,   737,   744,
     749,   750,   751,   752,   753,   755,   757,   758,   760,   761,
     761,   763,   764,   767,   768,   771,   775,   776,   777,   779,
     783,   784,   785,   786,   789,   790,   795,   799,   800,   801,
     802,   803,   804,   805,   806,   807,   808,   809,   810,   811,
     812,   813,   814,   815,   816,   817,   818,   819,   820,   821,
     822,   823,   829,   830,   834,   838,   843,   844,   850,   851,
     862,   865,   872,   876,   877,   879,   880,   882,   883,   885,
     887,   889,   894,   897,   900,   904,   910,   917,   916,   933,
     933,   949,   950,   952,   954,   956,   952,   963,   964,   964,
     970,   973,   976,   982,   982,   995,   997,   995,  1009,  1011,
    1013,  1016,  1019,  1016,  1026,  1030,  1030,  1037,  1038,  1040,
    1042,  1042,  1053,  1053,  1067,  1068,  1070,  1070,  1080,  1080,
    1090,  1092,  1095,  1097,  1100,  1100,  1108,  1118,  1123,  1124,
    1129,  1134,  1137,  1142,  1143,  1147,  1150,  1155,  1160,  1164,
    1171,  1176,  1184,  1192,  1200
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
  "file_structure", "$@1", "$@2", "header_delimiter", "macro_definitions",
  "element_with_annotations", "macro_or_native_function", "namespace",
  "prerequisites", "require", "global_declarations", "typedef",
  "struct_definition", "$@3", "enum_definition", "$@4",
  "native_type_definition", "$@5", "typedef_identifier",
  "struct_element_definitions", "struct_element", "enum_value_definitions",
  "enum_values", "enum_value", "opt_enum_assignment", "scope", "constants",
  "constant_list", "constdef", "$@6", "$@7", "constant_literal",
  "variable_identifier", "variable_reference", "macro_declaration",
  "macro_type", "macrostart", "global_var", "var_decl",
  "optional_description", "argument_list", "non_empty_arguments",
  "parameter_list", "non_empty_parameters", "argument_declaration",
  "annotations", "annotation", "$@8", "annotation_value",
  "annotation_values", "native_function_definition", "$@9",
  "macro_definition", "$@10", "block", "var_decls", "statement",
  "assignment", "$@11", "shorthand_assignment", "$@12",
  "assignment_target", "shorthand_assignment_operator",
  "assignment_expression", "expression", "range_expression", "$@13",
  "range_increment", "minusminus_plusplus", "increment_expression",
  "simple_term", "constructor_expression", "unary_expression",
  "binary_expression", "condition", "value", "string", "break", "continue",
  "return_expression", "optional_bterm", "opt_num", "case_clauses",
  "case_clause", "case_selector", "case_condition", "label",
  "goto_expression", "switch_expression", "$@14", "for_loop_expression",
  "$@15", "for_increment", "for_to_clause", "$@16", "$@17", "$@18",
  "opt_for_initializer", "$@19", "for_clause", "in_clause", "$@20",
  "while", "$@21", "$@22", "local_block", "closing_brace", "if_expression",
  "$@23", "$@24", "else_clause", "$@25", "stmntlist", "closing_paren",
  "call_expression", "@26", "@27", "array_constructor", "@28",
  "map_constructor", "@29", "optional_map_associates", "map_associates",
  "map_associate", "@30", "function_id_or_pointer", "parameter",
  "type_cast", "type_name", "array_size", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-282)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-248)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -282,    34,    52,  -282,    90,  -282,    45,    97,  -282,   105,
    -282,  -282,    59,    32,  -282,    22,  -282,  -282,  -282,    16,
     128,    76,    79,  -282,  -282,  -282,    85,  -282,  -282,  -282,
      83,  -282,    14,  -282,  -282,    16,   153,   160,   103,  -282,
      14,  -282,  -282,   109,   112,    89,  -282,  -282,  -282,  -282,
    -282,   130,   110,  -282,  -282,  -282,   119,  -282,   180,  -282,
       4,  -282,   148,   178,   128,  -282,  -282,  -282,  -282,    20,
      -4,   180,  -282,    98,     2,   126,   127,  -282,   503,  -282,
     158,   136,  -282,   138,   137,   128,    14,  -282,  -282,  -282,
    -282,  -282,  -282,   181,  -282,  -282,  -282,  -282,  -282,  -282,
     166,  -282,   141,   140,  -282,  -282,  -282,  -282,  -282,   144,
    -282,  -282,  -282,   205,   -28,  -282,   147,   177,   503,   503,
     503,     7,     3,  -282,  -282,   142,   148,  -282,  -282,  -282,
    -282,  -282,   531,  -282,   204,  -282,  -282,  -282,   151,    77,
    -282,    35,  -282,    89,   178,    89,  -282,   156,  -282,  -282,
     213,    -4,  -282,   102,   404,   162,  -282,   720,   745,   298,
    -282,  -282,  -282,  -282,   114,   217,   586,   163,  -282,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   222,  -282,  -282,   167,  -282,  -282,  -282,   224,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,   171,   174,  -282,
      14,   183,     9,    11,   182,  -282,   234,   184,   241,   241,
     503,  -282,   240,   123,   191,  -282,   316,   193,  -282,    51,
     194,  -282,  -282,   195,  -282,   197,  -282,  -282,  -282,  -282,
     196,   199,   202,   264,  -282,   503,   198,   226,  -282,  -282,
     209,   200,   227,  -282,  -282,  -282,   670,   745,   745,   745,
     760,   760,   695,   695,   248,   248,    55,   720,   720,   720,
     745,   248,   248,   298,   298,    55,    55,    55,   219,  -282,
     264,  -282,   250,   102,  -282,  -282,  -282,   128,  -282,  -282,
    -282,  -282,   503,  -282,   182,  -282,   211,  -282,   231,   232,
     670,  -282,  -282,  -282,   257,  -282,  -282,  -282,  -282,  -282,
    -282,   503,   287,  -282,  -282,  -282,  -282,  -282,  -282,  -282,
     230,  -282,  -282,   481,  -282,  -282,   217,   280,  -282,   230,
    -282,  -282,  -282,  -282,    14,   448,   615,   448,  -282,  -282,
     235,  -282,  -282,  -282,  -282,  -282,  -282,  -282,   503,   556,
    -282,  -282,  -282,  -282,   211,  -282,   503,  -282,  -282,   239,
     242,   360,  -282,  -282,  -282,  -282,   448,  -282,  -282,   148,
     148,  -282,  -282,   503,   148,   243,   196,   291,  -282,   251,
    -282,  -282,   644,  -282,   503,  -282,  -282,  -282,    88,  -282,
     255,   448,    72,   249,   262,    88,   316,     3,  -282,  -282,
     310,  -282,   263,  -282,  -282,  -282,  -282,  -282,   125,  -282,
    -282,  -282,   324,  -282,   268,  -282,  -282
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,    12,     1,     0,    14,     0,     3,    13,     0,
      17,    15,     0,     5,    16,     0,    44,    48,     6,    79,
       0,     0,     0,    26,    24,    28,     0,    22,    21,    23,
      45,    46,     0,    81,     4,    79,    91,    79,   251,    63,
       0,    18,    19,     0,     0,     0,    20,    48,    53,    54,
      55,     0,     0,     8,    89,     9,     0,    11,    43,    80,
       0,   252,    65,    35,     0,    31,    30,    29,    47,     0,
      86,    43,    10,     0,     0,     0,     0,   253,   128,    64,
      40,     0,    36,    37,     0,    32,     0,   258,   257,    52,
     260,   255,   256,     0,    49,   264,   262,   261,   263,    51,
      84,    85,    87,     0,    90,    60,    61,    58,    59,     0,
      94,    92,   254,     0,   177,   178,     0,   174,     0,     0,
       0,     0,     0,   236,   238,     0,     0,   125,   127,   144,
     138,   147,   126,   136,   175,   137,   234,   235,     0,     0,
     176,     0,    39,     0,     0,     0,    33,     0,   259,    50,
       0,    86,    82,    68,     0,     0,   230,   140,   141,   142,
      57,    56,   143,   246,     0,   240,     0,     0,   171,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   128,     0,   179,   232,   177,   174,   145,    41,     0,
      27,    38,    25,    34,    83,    88,    78,     0,    69,    70,
       0,     0,     0,    57,     0,   215,     0,     0,   185,   185,
     183,    96,    94,   117,   110,   107,     0,     0,   116,     0,
       0,   103,   104,     0,   109,     0,   102,   101,   100,    99,
       0,     0,     0,     0,    76,   128,     0,    73,   248,    74,
       0,     0,   241,   242,   170,   250,   129,   150,   169,   168,
     159,   160,   163,   162,   166,   167,   161,   151,   152,   154,
     149,   164,   165,   153,   155,   156,   157,   158,     0,   146,
       0,    42,    66,     0,    77,   111,   212,   207,   210,   199,
     211,   195,     0,   221,     0,   196,     0,   186,     0,     0,
     184,   182,   134,   133,     0,   112,   135,    95,   110,   228,
      98,     0,     0,   108,   105,   106,   220,    93,    97,   139,
       0,   249,   237,   128,   244,   239,     0,   131,   148,     0,
      67,    62,    71,   203,     0,     0,   173,     0,   216,   177,
       0,   180,   181,   123,   124,   120,   121,   122,   128,     0,
     113,   229,   231,    75,     0,   243,     0,   130,   233,     0,
     208,     0,   218,   200,   172,   222,     0,   197,   118,     0,
       0,   245,   132,     0,     0,     0,     0,   224,   217,     0,
     115,   114,     0,   209,   128,   219,   225,   223,   187,   204,
       0,     0,     0,     0,     0,   187,     0,     0,   214,   226,
     258,   193,     0,   192,   191,   198,   188,   189,   117,   202,
     201,   205,     0,   190,     0,   194,   206
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -282,  -282,  -282,  -282,  -282,   308,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,   -94,
     260,  -282,  -282,   206,  -282,  -282,   335,  -282,  -282,   302,
    -282,  -282,  -282,   -39,  -119,  -282,  -282,   289,  -282,   -11,
    -282,  -282,  -282,  -232,  -282,    68,   330,  -282,  -282,  -282,
     218,  -282,  -282,  -282,  -282,   295,  -282,  -281,  -282,  -282,
     -26,  -282,  -282,  -282,  -121,   -78,  -282,  -282,  -282,  -282,
     -25,   236,  -282,  -282,  -112,    84,  -274,  -282,  -282,  -282,
    -282,  -282,   154,   -18,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,  -282,  -282,  -245,     5,  -282,  -282,
    -282,  -282,  -282,  -222,    53,  -140,  -282,  -282,  -282,  -282,
    -282,  -282,  -282,  -282,    63,  -282,  -282,    69,  -282,   -44,
    -282,  -282,  -282,     1,  -282,   325
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    10,    19,    34,    35,    55,     5,     7,
      11,    13,    26,    27,    44,    28,    43,    29,    45,    67,
      84,    85,    81,    82,    83,   142,    73,    21,    30,    31,
      32,   149,    94,    51,   126,   109,    74,    75,    22,   308,
     331,   207,   208,   246,   247,   209,    36,    37,    52,   102,
     103,    56,    71,    57,    58,   225,   154,   226,   227,   370,
     228,   304,   229,   348,    79,   248,   128,   327,   357,   306,
     230,   129,   130,   131,   132,   293,   133,   134,   231,   232,
     233,   301,   298,   394,   395,   396,   402,   234,   235,   236,
     379,   237,   335,   411,   288,   359,   397,   414,   333,   374,
     289,   290,   375,   238,   294,   366,   363,   317,   239,   337,
     377,   387,   391,   240,   352,   135,   243,   280,   136,   164,
     137,   165,   251,   252,   253,   354,   138,   249,   139,    40,
      61,    95,    96,    97,    98,   140
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     127,    62,   162,   163,   309,   168,   157,   158,   159,    39,
     286,   320,   100,   166,   241,   101,    -7,   -57,   107,   160,
      86,    76,   340,   160,   148,    33,   108,    23,    24,   106,
      48,    49,    25,    50,     3,   223,    15,    87,    88,    89,
      90,    86,    91,    92,    16,  -247,   161,   147,   329,   200,
     161,   202,   198,    17,   362,     4,   362,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,    77,
     371,   167,   287,    93,  -247,   362,   241,   291,   113,   400,
      18,   401,   365,   195,    87,    88,   115,    90,   199,    91,
      92,   116,   305,    65,     9,    66,     6,   223,   300,   210,
     362,   105,    38,   278,     8,   244,    38,   206,   311,   312,
     196,   378,   191,   192,    12,   113,   392,   393,    14,   245,
     114,    87,    88,   115,    90,    93,    91,    92,   116,   376,
      93,   122,    38,   224,   123,    41,   399,   124,    42,   302,
     303,   302,   303,   113,    46,    47,    38,   117,   114,    87,
      88,   115,    90,    54,    91,    92,   116,   321,    78,    33,
      60,   284,   118,   119,   407,    69,   120,   121,   122,    63,
     336,   123,    64,    70,   124,   117,  -128,   125,    72,   -72,
    -119,  -119,    16,    78,    80,   241,   110,   241,   148,   349,
     118,   119,   112,   141,   120,   121,   122,   143,   145,   123,
     144,   150,   124,   151,   152,   125,   223,   153,   223,   155,
     156,   241,   -56,   193,   194,   203,   241,   339,    87,    88,
     115,    90,   204,    91,    92,   242,   250,   255,   279,   210,
    -247,   281,   223,   334,   372,   282,   283,   223,   380,   381,
     295,   241,   285,   383,   196,   292,   241,   296,   297,  -238,
     307,   382,   310,   313,   314,   244,   315,   316,   318,   330,
     368,   325,   223,   322,    93,   113,   319,   223,   408,   245,
     114,    87,    88,   115,    90,   324,    91,    92,   116,   343,
     344,   345,   346,   347,   328,   360,   173,   174,   323,   326,
     341,   342,   179,   350,   351,   356,   390,   117,   373,   367,
     186,   187,   188,   189,   190,   191,   192,   211,  -213,   384,
     386,   388,   118,   119,   212,   404,   120,   121,   122,   398,
      38,   123,   213,   405,   124,   412,  -128,   125,   -72,   413,
     116,   415,   416,    53,   214,   146,   215,   216,    20,    68,
     201,   332,   179,   217,  -227,  -227,   218,   219,   220,   161,
     104,   211,   188,   189,   190,   191,   192,    59,   212,   205,
     111,   409,   410,   299,    38,   197,   213,   406,   338,  -238,
     122,   385,   358,   123,   116,   221,   222,  -227,   214,   355,
     215,   216,   353,   403,    99,     0,     0,   217,     0,     0,
     218,   219,   220,   161,     0,   211,     0,     0,     0,     0,
       0,     0,   212,     0,     0,     0,     0,     0,    38,     0,
     213,     0,     0,     0,   122,     0,     0,   123,   116,   221,
     222,   -94,   214,     0,   215,   216,     0,     0,     0,     0,
       0,   217,     0,     0,   218,   219,   220,   161,     0,   211,
       0,     0,     0,     0,     0,     0,   212,     0,     0,     0,
       0,     0,    38,     0,   213,     0,     0,     0,   122,     0,
       0,   123,   116,   221,   222,  -227,   214,     0,   215,   216,
       0,     0,     0,     0,     0,   217,     0,     0,   218,   219,
     220,   161,   113,     0,     0,     0,   245,   114,    87,    88,
     115,    90,     0,    91,    92,   116,     0,     0,     0,     0,
       0,     0,   122,     0,   113,   123,     0,   221,   361,   114,
      87,    88,   115,    90,   117,    91,    92,   116,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   118,
     119,     0,     0,   120,   121,   122,   117,     0,   123,     0,
       0,   124,     0,     0,   125,     0,   169,     0,     0,     0,
       0,   118,   119,     0,     0,   120,   121,   122,     0,     0,
     123,     0,     0,   124,     0,   170,   125,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,     0,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     170,     0,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,     0,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,     0,     0,     0,     0,     0,
     170,   369,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,     0,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,     0,     0,     0,     0,   170,
     254,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,     0,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,     0,     0,     0,     0,   170,   364,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,     0,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   389,   170,     0,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,     0,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   170,
       0,   171,   172,   173,   174,     0,     0,   177,   178,   179,
     180,   181,   182,     0,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   170,     0,   171,   172,   173,   174,
       0,     0,   177,   178,   179,     0,     0,     0,     0,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,  -248,
       0,  -248,  -248,   173,   174,     0,     0,   177,   178,   179,
       0,     0,     0,     0,  -248,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   179,     0,     0,     0,     0,     0,
       0,     0,   186,   187,   188,   189,   190,   191,   192
};

static const yytype_int16 yycheck[] =
{
      78,    40,   121,   122,   226,   126,   118,   119,   120,    20,
       1,   243,    16,   125,   154,    19,     0,    45,    16,    16,
      64,    17,   296,    16,    17,     9,    24,     5,     6,    73,
      16,    17,    10,    19,     0,   154,     4,    17,    18,    19,
      20,    85,    22,    23,    12,    73,    43,    86,   280,   143,
      43,   145,    17,    21,   335,     3,   337,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,    75,
     354,   125,    73,    63,    73,   366,   226,    76,    11,    17,
      58,    19,   337,    16,    17,    18,    19,    20,    63,    22,
      23,    24,   223,    14,     7,    16,    16,   226,   220,   153,
     391,    13,    14,   191,    69,     1,    14,    15,    67,    68,
      43,   366,    67,    68,    19,    11,    38,    39,    69,    15,
      16,    17,    18,    19,    20,    63,    22,    23,    24,   361,
      63,    64,    14,   154,    67,    69,   391,    70,    69,    26,
      27,    26,    27,    11,    69,    72,    14,    43,    16,    17,
      18,    19,    20,    10,    22,    23,    24,   245,    45,     9,
      67,   210,    58,    59,   396,    45,    62,    63,    64,    70,
     292,    67,    70,    73,    70,    43,    72,    73,    69,    75,
      67,    68,    12,    45,    16,   335,    70,   337,    17,   311,
      58,    59,    75,    45,    62,    63,    64,    71,    71,    67,
      72,    45,    70,    72,    74,    73,   335,    73,   337,    14,
      73,   361,    45,    19,    73,    69,   366,    16,    17,    18,
      19,    20,    19,    22,    23,    73,    19,    74,    16,   283,
      73,    17,   361,   287,   356,    74,    72,   366,   369,   370,
      16,   391,    69,   374,    43,    73,   396,    73,    17,    19,
      69,   373,    69,    69,    69,     1,    69,    71,    69,    19,
     348,    71,   391,    75,    63,    11,    74,   396,   397,    15,
      16,    17,    18,    19,    20,    76,    22,    23,    24,    32,
      33,    34,    35,    36,    75,   334,    48,    49,    72,    72,
      69,    69,    54,    16,    74,    25,   384,    43,    69,    74,
      62,    63,    64,    65,    66,    67,    68,     1,    76,    76,
      29,    70,    58,    59,     8,    76,    62,    63,    64,    74,
      14,    67,    16,    71,    70,    25,    72,    73,    74,    76,
      24,    17,    74,    35,    28,    85,    30,    31,    13,    47,
     144,   283,    54,    37,    38,    39,    40,    41,    42,    43,
      71,     1,    64,    65,    66,    67,    68,    37,     8,   151,
      75,   397,   397,   219,    14,   139,    16,   395,   294,    19,
      64,   376,   329,    67,    24,    69,    70,    71,    28,   326,
      30,    31,   323,   392,    69,    -1,    -1,    37,    -1,    -1,
      40,    41,    42,    43,    -1,     1,    -1,    -1,    -1,    -1,
      -1,    -1,     8,    -1,    -1,    -1,    -1,    -1,    14,    -1,
      16,    -1,    -1,    -1,    64,    -1,    -1,    67,    24,    69,
      70,    71,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      -1,    37,    -1,    -1,    40,    41,    42,    43,    -1,     1,
      -1,    -1,    -1,    -1,    -1,    -1,     8,    -1,    -1,    -1,
      -1,    -1,    14,    -1,    16,    -1,    -1,    -1,    64,    -1,
      -1,    67,    24,    69,    70,    71,    28,    -1,    30,    31,
      -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,    40,    41,
      42,    43,    11,    -1,    -1,    -1,    15,    16,    17,    18,
      19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    -1,    11,    67,    -1,    69,    70,    16,
      17,    18,    19,    20,    43,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,
      59,    -1,    -1,    62,    63,    64,    43,    -1,    67,    -1,
      -1,    70,    -1,    -1,    73,    -1,    25,    -1,    -1,    -1,
      -1,    58,    59,    -1,    -1,    62,    63,    64,    -1,    -1,
      67,    -1,    -1,    70,    -1,    44,    73,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    -1,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      44,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    -1,    -1,    -1,    -1,    -1,
      44,    75,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    -1,    -1,    -1,    -1,    44,
      74,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    -1,    -1,    -1,    -1,    44,    74,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    44,    -1,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    -1,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    44,
      -1,    46,    47,    48,    49,    -1,    -1,    52,    53,    54,
      55,    56,    57,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    44,    -1,    46,    47,    48,    49,
      -1,    -1,    52,    53,    54,    -1,    -1,    -1,    -1,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    44,
      -1,    46,    47,    48,    49,    -1,    -1,    52,    53,    54,
      -1,    -1,    -1,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    54,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    62,    63,    64,    65,    66,    67,    68
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    78,    79,     0,     3,    85,    16,    86,    69,     7,
      80,    87,    19,    88,    69,     4,    12,    21,    58,    81,
     103,   104,   115,     5,     6,    10,    89,    90,    92,    94,
     105,   106,   107,     9,    82,    83,   123,   124,    14,   116,
     206,    69,    69,    93,    91,    95,    69,    72,    16,    17,
      19,   110,   125,    82,    10,    84,   128,   130,   131,   123,
      67,   207,   110,    70,    70,    14,    16,    96,   106,    45,
      73,   129,    69,   103,   113,   114,    17,    75,    45,   141,
      16,    99,   100,   101,    97,    98,   206,    17,    18,    19,
      20,    22,    23,    63,   109,   208,   209,   210,   211,   212,
      16,    19,   126,   127,   114,    13,   206,    16,    24,   112,
      70,   132,    75,    11,    16,    19,    24,    43,    58,    59,
      62,    63,    64,    67,    70,    73,   111,   142,   143,   148,
     149,   150,   151,   153,   154,   192,   195,   197,   203,   205,
     212,    45,   102,    71,    72,    71,    97,   110,    17,   108,
      45,    72,    74,    73,   133,    14,    73,   151,   151,   151,
      16,    43,   111,   111,   196,   198,   151,   206,   141,    25,
      44,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    19,    73,    16,    43,   148,    17,    63,
      96,   100,    96,    69,    19,   127,    15,   118,   119,   122,
     206,     1,     8,    16,    28,    30,    31,    37,    40,    41,
      42,    69,    70,   111,   116,   132,   134,   135,   137,   139,
     147,   155,   156,   157,   164,   165,   166,   168,   180,   185,
     190,   192,    73,   193,     1,    15,   120,   121,   142,   204,
      19,   199,   200,   201,    74,    74,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   142,    16,
     194,    17,    74,    72,   110,    69,     1,    73,   171,   177,
     178,    76,    73,   152,   181,    16,    73,    17,   159,   159,
     151,   158,    26,    27,   138,   141,   146,    69,   116,   190,
      69,    67,    68,    69,    69,    69,    71,   184,    69,    74,
     120,   142,    75,    72,    76,    71,    72,   144,    75,   120,
      19,   117,   122,   175,   206,   169,   151,   186,   152,    16,
     153,    69,    69,    32,    33,    34,    35,    36,   140,   151,
      16,    74,   191,   204,   202,   201,    25,   145,   191,   172,
     110,    70,   134,   183,    74,   183,   182,    74,   142,    75,
     136,   153,   151,    69,   176,   179,   190,   187,   183,   167,
     141,   141,   151,   141,    76,   184,    29,   188,    70,    69,
     142,   189,    38,    39,   160,   161,   162,   173,    74,   183,
      17,    19,   163,   210,    76,    71,   160,   190,   111,   137,
     147,   170,    25,    76,   174,    17,    74
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    77,    79,    80,    78,    81,    81,    82,    82,    83,
      84,    84,    85,    85,    86,    86,    87,    88,    88,    88,
      88,    89,    89,    89,    91,    90,    93,    92,    95,    94,
      96,    96,    97,    97,    98,    99,    99,   100,   100,   101,
     102,   102,   102,   103,   103,   104,   105,   105,   107,   108,
     106,   109,   109,   110,   110,   110,   111,   111,   112,   112,
     113,   113,   114,   115,   116,   116,   117,   117,   118,   118,
     119,   119,   120,   120,   121,   121,   121,   122,   122,   123,
     123,   125,   124,   126,   126,   126,   127,   127,   127,   129,
     128,   131,   130,   132,   133,   133,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   135,   136,   135,   135,   135,   138,   137,   139,
     140,   140,   140,   140,   140,   141,   142,   142,   143,   144,
     143,   145,   145,   146,   146,   147,   148,   148,   148,   149,
     150,   150,   150,   150,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   152,   152,   153,   153,   153,   153,   154,   154,
     155,   156,   157,   158,   158,   159,   159,   160,   160,   161,
     162,   162,   163,   163,   163,   164,   165,   167,   166,   169,
     168,   170,   170,   172,   173,   174,   171,   175,   176,   175,
     177,   177,   177,   179,   178,   181,   182,   180,   183,   183,
     184,   186,   187,   185,   188,   189,   188,   190,   190,   191,
     193,   192,   194,   192,   192,   192,   196,   195,   198,   197,
     199,   199,   200,   200,   202,   201,   203,   203,   204,   204,
     205,   206,   206,   207,   207,   208,   208,   209,   210,   210,
     211,   212,   212,   212,   212
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     7,     0,     1,     0,     2,     2,
       2,     1,     0,     3,     0,     2,     3,     0,     3,     3,
       4,     1,     1,     1,     0,     6,     0,     6,     0,     3,
       1,     1,     1,     2,     3,     0,     1,     1,     3,     2,
       0,     2,     3,     0,     1,     2,     1,     3,     0,     0,
       5,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     6,     2,     3,     2,     0,     1,     0,     1,
       1,     3,     0,     1,     1,     3,     1,     2,     1,     0,
       2,     0,     5,     3,     1,     1,     0,     1,     3,     0,
       3,     0,     3,     4,     0,     3,     1,     2,     2,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     2,     1,
       1,     2,     2,     0,     5,     5,     1,     0,     4,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     0,     0,
       5,     0,     2,     1,     1,     2,     1,     1,     1,     4,
       2,     2,     2,     2,     1,     2,     3,     1,     4,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     2,     1,     1,     1,     1,     1,     2,
       3,     3,     2,     0,     1,     0,     1,     0,     2,     2,
       3,     2,     1,     1,     3,     2,     2,     0,     8,     0,
       4,     1,     1,     0,     0,     0,    10,     0,     0,     4,
       1,     1,     1,     0,     7,     0,     0,     5,     1,     3,
       1,     0,     0,     6,     0,     0,     3,     0,     2,     1,
       0,     5,     0,     5,     1,     1,     0,     4,     0,     4,
       0,     1,     1,     3,     0,     4,     2,     1,     1,     2,
       3,     1,     2,     2,     3,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1
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
#line 375 "Parser.y"
                {
				init_header();
			}
#line 2264 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 378 "Parser.y"
                                                {
				if (parser_resetIfDependencies()) {
					YYACCEPT;
				}
			}
#line 2274 "parser.c"
    break;

  case 4: /* file_structure: $@1 namespace prerequisites $@2 global_declarations header_delimiter macro_definitions  */
#line 385 "Parser.y"
                                          {
				finalize_header();
			}
#line 2282 "parser.c"
    break;

  case 12: /* namespace: %empty  */
#line 402 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2290 "parser.c"
    break;

  case 13: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 405 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2299 "parser.c"
    break;

  case 16: /* require: T_REQUIRE T_STRING ';'  */
#line 413 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2308 "parser.c"
    break;

  case 24: /* $@3: %empty  */
#line 427 "Parser.y"
                            { parser_startTypeDescriptor(0); }
#line 2314 "parser.c"
    break;

  case 26: /* $@4: %empty  */
#line 429 "Parser.y"
                        { parser_startTypeDescriptor(1); }
#line 2320 "parser.c"
    break;

  case 28: /* $@5: %empty  */
#line 431 "Parser.y"
                                 { parser_startTypeDescriptor(0); }
#line 2326 "parser.c"
    break;

  case 30: /* typedef_identifier: T_IDENT  */
#line 433 "Parser.y"
                            {
				parser_finalizeTypeDescriptor(yyvsp[0].ident.s);
			}
#line 2334 "parser.c"
    break;

  case 31: /* typedef_identifier: T_TYPE_IDENTIFIER  */
#line 437 "Parser.y"
                                          {
				parser_finalizeTypeDescriptor(_strdup(types_nameFor(yyvsp[0].ident.type)));
			}
#line 2342 "parser.c"
    break;

  case 34: /* struct_element: type_name variable_identifier ';'  */
#line 444 "Parser.y"
                                                  {
				parser_addTypeProperty(yyvsp[-1].ident.s, (long)yyvsp[-2].num);
			}
#line 2350 "parser.c"
    break;

  case 39: /* enum_value: T_IDENT opt_enum_assignment  */
#line 454 "Parser.y"
                                            {
				parser_addTypeProperty(yyvsp[-1].ident.s, (long)yyvsp[0].num);
			}
#line 2358 "parser.c"
    break;

  case 41: /* opt_enum_assignment: T_ASSIGN T_NUM  */
#line 459 "Parser.y"
                                         {
				yyval.num = yyvsp[0].num;
			}
#line 2366 "parser.c"
    break;

  case 42: /* opt_enum_assignment: T_ASSIGN '-' T_NUM  */
#line 462 "Parser.y"
                                             {
				yyval.num = -yyvsp[0].num;
			}
#line 2374 "parser.c"
    break;

  case 43: /* scope: %empty  */
#line 466 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2380 "parser.c"
    break;

  case 44: /* scope: T_STATIC  */
#line 467 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2386 "parser.c"
    break;

  case 48: /* $@6: %empty  */
#line 477 "Parser.y"
                { _bDefiningConst = 1; }
#line 2392 "parser.c"
    break;

  case 49: /* $@7: %empty  */
#line 477 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2398 "parser.c"
    break;

  case 50: /* constdef: $@6 variable_identifier T_ASSIGN constant_literal $@7  */
#line 478 "Parser.y"
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
#line 2414 "parser.c"
    break;

  case 51: /* constant_literal: simple_literal  */
#line 491 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2422 "parser.c"
    break;

  case 52: /* constant_literal: T_STRING  */
#line 494 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2431 "parser.c"
    break;

  case 53: /* variable_identifier: T_IDENT  */
#line 500 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2441 "parser.c"
    break;

  case 54: /* variable_identifier: T_NUM  */
#line 505 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2452 "parser.c"
    break;

  case 55: /* variable_identifier: T_STRING  */
#line 511 "Parser.y"
                                   {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2461 "parser.c"
    break;

  case 56: /* variable_reference: T_VARIABLE  */
#line 517 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2467 "parser.c"
    break;

  case 57: /* variable_reference: T_IDENT  */
#line 518 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2479 "parser.c"
    break;

  case 58: /* macro_declaration: T_IDENT  */
#line 528 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2487 "parser.c"
    break;

  case 59: /* macro_declaration: T_FUNC  */
#line 531 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2497 "parser.c"
    break;

  case 60: /* macro_type: scope T_VOID  */
#line 537 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
				yyval.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(VT_NIL);
				}
			}
#line 2510 "parser.c"
    break;

  case 61: /* macro_type: scope type_name  */
#line 546 "Parser.y"
                                        {
				yyval.ident.type = yyvsp[0].ident.type;
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(yyvsp[0].ident.type);
				}
			}
#line 2523 "parser.c"
    break;

  case 62: /* macrostart: macro_type macro_declaration '(' argument_list ')' optional_description  */
#line 556 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				yyval.ident.type = yyvsp[-5].ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2533 "parser.c"
    break;

  case 64: /* var_decl: type_name variable_identifier assignment_expression  */
#line 564 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});
			}
#line 2546 "parser.c"
    break;

  case 65: /* var_decl: type_name variable_identifier  */
#line 571 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2557 "parser.c"
    break;

  case 67: /* optional_description: T_STRING  */
#line 580 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2566 "parser.c"
    break;

  case 76: /* non_empty_parameters: error  */
#line 598 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 2574 "parser.c"
    break;

  case 77: /* argument_declaration: type_name variable_identifier  */
#line 602 "Parser.y"
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
#line 2589 "parser.c"
    break;

  case 78: /* argument_declaration: T_ELLIPSIS  */
#line 611 "Parser.y"
                                       {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[0].ident.type, "...");
				}
			}
#line 2599 "parser.c"
    break;

  case 81: /* $@8: %empty  */
#line 620 "Parser.y"
                         {
				start_annotation(yyvsp[0].ident.s);
			}
#line 2607 "parser.c"
    break;

  case 83: /* annotation_value: T_IDENT T_ASSIGN T_STRING  */
#line 624 "Parser.y"
                                            {
				parser_defineAnnotationValue(yyvsp[-2].ident.s, yyvsp[0].ident.s);
			}
#line 2615 "parser.c"
    break;

  case 84: /* annotation_value: T_IDENT  */
#line 626 "Parser.y"
                                    {
				parser_defineAnnotationValue(yyvsp[0].ident.s, _strdup("true"));
			}
#line 2623 "parser.c"
    break;

  case 85: /* annotation_value: T_STRING  */
#line 628 "Parser.y"
                                     {
				parser_defineAnnotationValue(_strdup("value"), yyvsp[0].ident.s);
			}
#line 2631 "parser.c"
    break;

  case 89: /* $@9: %empty  */
#line 637 "Parser.y"
                                         {
					init_native();
				}
#line 2639 "parser.c"
    break;

  case 90: /* native_function_definition: T_NATIVE $@9 macrostart  */
#line 639 "Parser.y"
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
#line 2672 "parser.c"
    break;

  case 91: /* $@10: %empty  */
#line 668 "Parser.y"
                  { init_macroDefinition(); }
#line 2678 "parser.c"
    break;

  case 92: /* macro_definition: $@10 macrostart block  */
#line 669 "Parser.y"
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
#line 2707 "parser.c"
    break;

  case 97: /* statement: call_expression ';'  */
#line 700 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2713 "parser.c"
    break;

  case 98: /* statement: assignment ';'  */
#line 701 "Parser.y"
                                         { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2719 "parser.c"
    break;

  case 99: /* statement: if_expression  */
#line 702 "Parser.y"
                                        { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2725 "parser.c"
    break;

  case 100: /* statement: while  */
#line 703 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2731 "parser.c"
    break;

  case 101: /* statement: for_loop_expression  */
#line 704 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2737 "parser.c"
    break;

  case 102: /* statement: switch_expression  */
#line 705 "Parser.y"
                                            { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2743 "parser.c"
    break;

  case 107: /* statement: block  */
#line 710 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2749 "parser.c"
    break;

  case 108: /* statement: increment_expression ';'  */
#line 711 "Parser.y"
                                                   { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2755 "parser.c"
    break;

  case 110: /* statement: var_decl  */
#line 713 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2763 "parser.c"
    break;

  case 111: /* statement: error ';'  */
#line 716 "Parser.y"
                                    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2772 "parser.c"
    break;

  case 112: /* assignment: variable_reference assignment_expression  */
#line 721 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2780 "parser.c"
    break;

  case 113: /* $@11: %empty  */
#line 725 "Parser.y"
                                                      {
				parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2788 "parser.c"
    break;

  case 114: /* assignment: assignment_target '.' T_IDENT $@11 assignment_expression  */
#line 727 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2796 "parser.c"
    break;

  case 115: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 731 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2804 "parser.c"
    break;

  case 117: /* $@12: %empty  */
#line 737 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2812 "parser.c"
    break;

  case 118: /* shorthand_assignment: variable_reference $@12 shorthand_assignment_operator expression  */
#line 739 "Parser.y"
                                                                   {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2821 "parser.c"
    break;

  case 119: /* assignment_target: variable_reference  */
#line 744 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2829 "parser.c"
    break;

  case 120: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 749 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2835 "parser.c"
    break;

  case 121: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 750 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2841 "parser.c"
    break;

  case 122: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 751 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2847 "parser.c"
    break;

  case 123: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 752 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2853 "parser.c"
    break;

  case 124: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 753 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2859 "parser.c"
    break;

  case 125: /* assignment_expression: T_ASSIGN expression  */
#line 755 "Parser.y"
                                                { yyval.v = yyvsp[0].v;	}
#line 2865 "parser.c"
    break;

  case 126: /* expression: binary_expression  */
#line 757 "Parser.y"
                              { yyval.v = yyvsp[0].v; }
#line 2871 "parser.c"
    break;

  case 129: /* $@13: %empty  */
#line 761 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2877 "parser.c"
    break;

  case 132: /* range_increment: T_DOTDOT binary_expression  */
#line 764 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2883 "parser.c"
    break;

  case 133: /* minusminus_plusplus: T_MINUSMINUS  */
#line 767 "Parser.y"
                                     { yyval.num = -1; }
#line 2889 "parser.c"
    break;

  case 134: /* minusminus_plusplus: T_PLUSPLUS  */
#line 768 "Parser.y"
                                     { yyval.num = 1; }
#line 2895 "parser.c"
    break;

  case 135: /* increment_expression: variable_reference minusminus_plusplus  */
#line 771 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2903 "parser.c"
    break;

  case 136: /* simple_term: value  */
#line 775 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2909 "parser.c"
    break;

  case 139: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 779 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2917 "parser.c"
    break;

  case 140: /* unary_expression: '!' binary_expression  */
#line 783 "Parser.y"
                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2923 "parser.c"
    break;

  case 141: /* unary_expression: '~' binary_expression  */
#line 784 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2929 "parser.c"
    break;

  case 142: /* unary_expression: '+' binary_expression  */
#line 785 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2935 "parser.c"
    break;

  case 143: /* unary_expression: '-' variable_reference  */
#line 786 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2941 "parser.c"
    break;

  case 144: /* binary_expression: simple_term  */
#line 789 "Parser.y"
                                    { yyval.ident.type = yyvsp[0].ident.type;}
#line 2947 "parser.c"
    break;

  case 145: /* binary_expression: type_cast simple_term  */
#line 790 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2956 "parser.c"
    break;

  case 146: /* binary_expression: binary_expression '.' T_IDENT  */
#line 795 "Parser.y"
                                                      {
				yyval.ident.type = parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2965 "parser.c"
    break;

  case 148: /* binary_expression: binary_expression '[' expression ']'  */
#line 800 "Parser.y"
                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2971 "parser.c"
    break;

  case 149: /* binary_expression: binary_expression '~' binary_expression  */
#line 801 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2977 "parser.c"
    break;

  case 150: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 802 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2983 "parser.c"
    break;

  case 151: /* binary_expression: binary_expression '&' binary_expression  */
#line 803 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2989 "parser.c"
    break;

  case 152: /* binary_expression: binary_expression '|' binary_expression  */
#line 804 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2995 "parser.c"
    break;

  case 153: /* binary_expression: binary_expression '+' binary_expression  */
#line 805 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 3001 "parser.c"
    break;

  case 154: /* binary_expression: binary_expression '^' binary_expression  */
#line 806 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 3007 "parser.c"
    break;

  case 155: /* binary_expression: binary_expression '-' binary_expression  */
#line 807 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 3013 "parser.c"
    break;

  case 156: /* binary_expression: binary_expression '*' binary_expression  */
#line 808 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 3019 "parser.c"
    break;

  case 157: /* binary_expression: binary_expression '/' binary_expression  */
#line 809 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 3025 "parser.c"
    break;

  case 158: /* binary_expression: binary_expression '%' binary_expression  */
#line 810 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 3031 "parser.c"
    break;

  case 159: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 811 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 3037 "parser.c"
    break;

  case 160: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 812 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 3043 "parser.c"
    break;

  case 161: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 813 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 3049 "parser.c"
    break;

  case 162: /* binary_expression: binary_expression T_AND binary_expression  */
#line 814 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 3055 "parser.c"
    break;

  case 163: /* binary_expression: binary_expression T_OR binary_expression  */
#line 815 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 3061 "parser.c"
    break;

  case 164: /* binary_expression: binary_expression '<' binary_expression  */
#line 816 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 3067 "parser.c"
    break;

  case 165: /* binary_expression: binary_expression '>' binary_expression  */
#line 817 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 3073 "parser.c"
    break;

  case 166: /* binary_expression: binary_expression T_LE binary_expression  */
#line 818 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 3079 "parser.c"
    break;

  case 167: /* binary_expression: binary_expression T_GE binary_expression  */
#line 819 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 3085 "parser.c"
    break;

  case 168: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 820 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 3091 "parser.c"
    break;

  case 169: /* binary_expression: binary_expression T_NE binary_expression  */
#line 821 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 3097 "parser.c"
    break;

  case 170: /* binary_expression: '(' binary_expression ')'  */
#line 822 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 3103 "parser.c"
    break;

  case 171: /* binary_expression: variable_reference assignment_expression  */
#line 823 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 3111 "parser.c"
    break;

  case 173: /* condition: '(' binary_expression  */
#line 830 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 3119 "parser.c"
    break;

  case 174: /* value: T_VARIABLE  */
#line 834 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 3128 "parser.c"
    break;

  case 175: /* value: string  */
#line 838 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 3138 "parser.c"
    break;

  case 176: /* value: simple_literal  */
#line 843 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 3144 "parser.c"
    break;

  case 177: /* value: T_IDENT  */
#line 844 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 3154 "parser.c"
    break;

  case 178: /* string: T_STRING  */
#line 850 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 3160 "parser.c"
    break;

  case 179: /* string: string T_STRING  */
#line 851 "Parser.y"
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
#line 3175 "parser.c"
    break;

  case 180: /* break: T_BREAK opt_num ';'  */
#line 863 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 3181 "parser.c"
    break;

  case 181: /* continue: T_CONTINUE opt_num ';'  */
#line 865 "Parser.y"
                                       {
				// in the case this is a continue in a for(int i;b;i++) loop, generate the i++ code before continuing.
				parser_flushBuffer(_breaklevel-(int)yyvsp[-1].num+1);
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); 
			}
#line 3191 "parser.c"
    break;

  case 182: /* return_expression: T_RETURN optional_bterm  */
#line 872 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 3199 "parser.c"
    break;

  case 185: /* opt_num: %empty  */
#line 879 "Parser.y"
                {	yyval.num = 1;	}
#line 3205 "parser.c"
    break;

  case 186: /* opt_num: T_NUM  */
#line 880 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 3211 "parser.c"
    break;

  case 191: /* case_selector: T_DEFAULT ':'  */
#line 889 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 3219 "parser.c"
    break;

  case 192: /* case_condition: integer_literal  */
#line 894 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 3227 "parser.c"
    break;

  case 193: /* case_condition: T_STRING  */
#line 897 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3235 "parser.c"
    break;

  case 194: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 900 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3243 "parser.c"
    break;

  case 195: /* label: T_IDENT ':'  */
#line 904 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3252 "parser.c"
    break;

  case 196: /* goto_expression: T_GOTO T_IDENT  */
#line 910 "Parser.y"
                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3261 "parser.c"
    break;

  case 197: /* $@14: %empty  */
#line 917 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3271 "parser.c"
    break;

  case 198: /* switch_expression: T_SWITCH '(' value ')' $@14 '{' case_clauses '}'  */
#line 923 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3284 "parser.c"
    break;

  case 199: /* $@15: %empty  */
#line 933 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3293 "parser.c"
    break;

  case 200: /* for_loop_expression: T_FOR for_clause $@15 local_block  */
#line 936 "Parser.y"
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
#line 3310 "parser.c"
    break;

  case 203: /* $@16: %empty  */
#line 952 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3318 "parser.c"
    break;

  case 204: /* $@17: %empty  */
#line 954 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3326 "parser.c"
    break;

  case 205: /* $@18: %empty  */
#line 956 "Parser.y"
                                        {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			}
#line 3335 "parser.c"
    break;

  case 206: /* for_to_clause: '(' opt_for_initializer $@16 ';' binary_expression ';' $@17 for_increment $@18 ')'  */
#line 959 "Parser.y"
                              {
				yyval.ident.s = yyvsp[-8].ident.s;
			}
#line 3343 "parser.c"
    break;

  case 208: /* $@19: %empty  */
#line 964 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3351 "parser.c"
    break;

  case 209: /* opt_for_initializer: type_name variable_identifier $@19 assignment_expression  */
#line 966 "Parser.y"
                                                {
				yyval.ident.s = yyvsp[-2].ident.s;
			}
#line 3359 "parser.c"
    break;

  case 210: /* for_clause: for_to_clause  */
#line 970 "Parser.y"
                              {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3367 "parser.c"
    break;

  case 211: /* for_clause: in_clause  */
#line 973 "Parser.y"
                                    {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3375 "parser.c"
    break;

  case 212: /* for_clause: error  */
#line 976 "Parser.y"
                                {
				yyval.ident.s = 0;
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3385 "parser.c"
    break;

  case 213: /* $@20: %empty  */
#line 982 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				parser_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3396 "parser.c"
    break;

  case 214: /* in_clause: '(' type_name variable_identifier $@20 ':' expression ')'  */
#line 987 "Parser.y"
                                             {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
				yyval.ident.s = yyvsp[-4].ident.s;
			}
#line 3408 "parser.c"
    break;

  case 215: /* $@21: %empty  */
#line 995 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3416 "parser.c"
    break;

  case 216: /* $@22: %empty  */
#line 997 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3425 "parser.c"
    break;

  case 217: /* while: T_WHILE $@21 condition $@22 local_block  */
#line 1000 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3437 "parser.c"
    break;

  case 221: /* $@23: %empty  */
#line 1016 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3446 "parser.c"
    break;

  case 222: /* $@24: %empty  */
#line 1019 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3454 "parser.c"
    break;

  case 223: /* if_expression: T_IF condition $@23 local_block $@24 else_clause  */
#line 1021 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3463 "parser.c"
    break;

  case 224: /* else_clause: %empty  */
#line 1026 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3472 "parser.c"
    break;

  case 225: /* $@25: %empty  */
#line 1030 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3482 "parser.c"
    break;

  case 230: /* @26: %empty  */
#line 1042 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3494 "parser.c"
    break;

  case 231: /* call_expression: T_FUNC '(' @26 parameter_list closing_paren  */
#line 1048 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3504 "parser.c"
    break;

  case 232: /* @27: %empty  */
#line 1053 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3515 "parser.c"
    break;

  case 233: /* call_expression: function_id_or_pointer '(' @27 parameter_list closing_paren  */
#line 1058 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3529 "parser.c"
    break;

  case 236: /* @28: %empty  */
#line 1070 "Parser.y"
                       {
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = &_functionTable[FUNC_CreateArray];
			}
#line 3539 "parser.c"
    break;

  case 237: /* array_constructor: '[' @28 parameter_list ']'  */
#line 1074 "Parser.y"
                                             {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
					C_0FUNC,(GENERIC_DATA){FUNC_CreateArray}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
			}
#line 3549 "parser.c"
    break;

  case 238: /* @29: %empty  */
#line 1080 "Parser.y"
                     {
		_currentFunctionCallParamIndexP++;
		*_currentFunctionCallParamIndexP = 0;
		yyval.funcp = &_functionTable[FUNC_CreateMap];
	}
#line 3559 "parser.c"
    break;

  case 239: /* map_constructor: '{' @29 optional_map_associates '}'  */
#line 1084 "Parser.y"
                                      {
		_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
			C_0FUNC,(GENERIC_DATA){FUNC_CreateMap}, *_currentFunctionCallParamIndexP);
		_currentFunctionCallParamIndexP--;
	}
#line 3569 "parser.c"
    break;

  case 244: /* @30: %empty  */
#line 1100 "Parser.y"
                                     { 
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[-1].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[-1].ident.s);
				(*_currentFunctionCallParamIndexP) += 2;
			}
#line 3580 "parser.c"
    break;

  case 246: /* function_id_or_pointer: '*' variable_reference  */
#line 1108 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3594 "parser.c"
    break;

  case 247: /* function_id_or_pointer: T_IDENT  */
#line 1118 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3603 "parser.c"
    break;

  case 248: /* parameter: expression  */
#line 1123 "Parser.y"
                           { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3609 "parser.c"
    break;

  case 249: /* parameter: T_ELLIPSIS expression  */
#line 1124 "Parser.y"
                                                { 
				yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; 
				YY_EMIT(C_SPREAD, (GENERIC_DATA){1});
			}
#line 3618 "parser.c"
    break;

  case 250: /* type_cast: '(' type_name ')'  */
#line 1129 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3627 "parser.c"
    break;

  case 251: /* type_name: T_TYPE_IDENTIFIER  */
#line 1134 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3635 "parser.c"
    break;

  case 252: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 1137 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3644 "parser.c"
    break;

  case 254: /* array_size: '[' T_NUM ']'  */
#line 1143 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3652 "parser.c"
    break;

  case 255: /* boolean_literal: T_TRUE  */
#line 1147 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3661 "parser.c"
    break;

  case 256: /* boolean_literal: T_FALSE  */
#line 1150 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3670 "parser.c"
    break;

  case 257: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 1155 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3679 "parser.c"
    break;

  case 258: /* integer_literal: T_NUM  */
#line 1160 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3688 "parser.c"
    break;

  case 259: /* integer_literal: '-' T_NUM  */
#line 1164 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3698 "parser.c"
    break;

  case 260: /* character_literal: T_CHARACTER  */
#line 1171 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3707 "parser.c"
    break;

  case 261: /* simple_literal: integer_literal  */
#line 1176 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3720 "parser.c"
    break;

  case 262: /* simple_literal: float_literal  */
#line 1184 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3733 "parser.c"
    break;

  case 263: /* simple_literal: character_literal  */
#line 1192 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3746 "parser.c"
    break;

  case 264: /* simple_literal: boolean_literal  */
#line 1200 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3759 "parser.c"
    break;


#line 3763 "parser.c"

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

#line 1208 "Parser.y"


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
	_compilerConfiguration->cb_numberOfDependencies = (int)arraylist_size(_compilerConfiguration->cb_dependencies);
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

