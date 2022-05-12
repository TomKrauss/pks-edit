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


#line 408 "parser.c"

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
  YYSYMBOL_90_2 = 90,                      /* $@2  */
  YYSYMBOL_enum_definition = 91,           /* enum_definition  */
  YYSYMBOL_92_3 = 92,                      /* $@3  */
  YYSYMBOL_native_type_definition = 93,    /* native_type_definition  */
  YYSYMBOL_94_4 = 94,                      /* $@4  */
  YYSYMBOL_typedef_identifier = 95,        /* typedef_identifier  */
  YYSYMBOL_struct_element_definitions = 96, /* struct_element_definitions  */
  YYSYMBOL_struct_element = 97,            /* struct_element  */
  YYSYMBOL_enum_value_definitions = 98,    /* enum_value_definitions  */
  YYSYMBOL_enum_values = 99,               /* enum_values  */
  YYSYMBOL_enum_value = 100,               /* enum_value  */
  YYSYMBOL_opt_enum_assignment = 101,      /* opt_enum_assignment  */
  YYSYMBOL_scope = 102,                    /* scope  */
  YYSYMBOL_constants = 103,                /* constants  */
  YYSYMBOL_constant_list = 104,            /* constant_list  */
  YYSYMBOL_constdef = 105,                 /* constdef  */
  YYSYMBOL_106_5 = 106,                    /* $@5  */
  YYSYMBOL_107_6 = 107,                    /* $@6  */
  YYSYMBOL_constant_literal = 108,         /* constant_literal  */
  YYSYMBOL_variable_identifier = 109,      /* variable_identifier  */
  YYSYMBOL_variable_reference = 110,       /* variable_reference  */
  YYSYMBOL_macro_declaration = 111,        /* macro_declaration  */
  YYSYMBOL_macro_type = 112,               /* macro_type  */
  YYSYMBOL_macrostart = 113,               /* macrostart  */
  YYSYMBOL_global_var = 114,               /* global_var  */
  YYSYMBOL_var_decl = 115,                 /* var_decl  */
  YYSYMBOL_optional_description = 116,     /* optional_description  */
  YYSYMBOL_argument_list = 117,            /* argument_list  */
  YYSYMBOL_non_empty_arguments = 118,      /* non_empty_arguments  */
  YYSYMBOL_parameter_list = 119,           /* parameter_list  */
  YYSYMBOL_non_empty_parameters = 120,     /* non_empty_parameters  */
  YYSYMBOL_argument_declaration = 121,     /* argument_declaration  */
  YYSYMBOL_annotations = 122,              /* annotations  */
  YYSYMBOL_annotation = 123,               /* annotation  */
  YYSYMBOL_124_7 = 124,                    /* $@7  */
  YYSYMBOL_annotation_value = 125,         /* annotation_value  */
  YYSYMBOL_annotation_values = 126,        /* annotation_values  */
  YYSYMBOL_native_function_definition = 127, /* native_function_definition  */
  YYSYMBOL_128_8 = 128,                    /* $@8  */
  YYSYMBOL_macro_definition = 129,         /* macro_definition  */
  YYSYMBOL_130_9 = 130,                    /* $@9  */
  YYSYMBOL_block = 131,                    /* block  */
  YYSYMBOL_var_decls = 132,                /* var_decls  */
  YYSYMBOL_statement = 133,                /* statement  */
  YYSYMBOL_assignment = 134,               /* assignment  */
  YYSYMBOL_135_10 = 135,                   /* $@10  */
  YYSYMBOL_shorthand_assignment = 136,     /* shorthand_assignment  */
  YYSYMBOL_137_11 = 137,                   /* $@11  */
  YYSYMBOL_assignment_target = 138,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 139, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 140,    /* assignment_expression  */
  YYSYMBOL_expression = 141,               /* expression  */
  YYSYMBOL_range_expression = 142,         /* range_expression  */
  YYSYMBOL_143_12 = 143,                   /* $@12  */
  YYSYMBOL_range_increment = 144,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 145,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 146,     /* increment_expression  */
  YYSYMBOL_simple_term = 147,              /* simple_term  */
  YYSYMBOL_constructor_expression = 148,   /* constructor_expression  */
  YYSYMBOL_unary_expression = 149,         /* unary_expression  */
  YYSYMBOL_binary_expression = 150,        /* binary_expression  */
  YYSYMBOL_condition = 151,                /* condition  */
  YYSYMBOL_value = 152,                    /* value  */
  YYSYMBOL_string = 153,                   /* string  */
  YYSYMBOL_break = 154,                    /* break  */
  YYSYMBOL_continue = 155,                 /* continue  */
  YYSYMBOL_return_expression = 156,        /* return_expression  */
  YYSYMBOL_optional_bterm = 157,           /* optional_bterm  */
  YYSYMBOL_opt_num = 158,                  /* opt_num  */
  YYSYMBOL_case_clauses = 159,             /* case_clauses  */
  YYSYMBOL_case_clause = 160,              /* case_clause  */
  YYSYMBOL_case_selector = 161,            /* case_selector  */
  YYSYMBOL_case_condition = 162,           /* case_condition  */
  YYSYMBOL_label = 163,                    /* label  */
  YYSYMBOL_goto_expression = 164,          /* goto_expression  */
  YYSYMBOL_switch_expression = 165,        /* switch_expression  */
  YYSYMBOL_166_13 = 166,                   /* $@13  */
  YYSYMBOL_for_loop_expression = 167,      /* for_loop_expression  */
  YYSYMBOL_168_14 = 168,                   /* $@14  */
  YYSYMBOL_for_increment = 169,            /* for_increment  */
  YYSYMBOL_for_to_clause = 170,            /* for_to_clause  */
  YYSYMBOL_171_15 = 171,                   /* $@15  */
  YYSYMBOL_172_16 = 172,                   /* $@16  */
  YYSYMBOL_173_17 = 173,                   /* $@17  */
  YYSYMBOL_opt_for_initializer = 174,      /* opt_for_initializer  */
  YYSYMBOL_175_18 = 175,                   /* $@18  */
  YYSYMBOL_for_clause = 176,               /* for_clause  */
  YYSYMBOL_in_clause = 177,                /* in_clause  */
  YYSYMBOL_178_19 = 178,                   /* $@19  */
  YYSYMBOL_while = 179,                    /* while  */
  YYSYMBOL_180_20 = 180,                   /* $@20  */
  YYSYMBOL_181_21 = 181,                   /* $@21  */
  YYSYMBOL_local_block = 182,              /* local_block  */
  YYSYMBOL_closing_brace = 183,            /* closing_brace  */
  YYSYMBOL_if_expression = 184,            /* if_expression  */
  YYSYMBOL_185_22 = 185,                   /* $@22  */
  YYSYMBOL_186_23 = 186,                   /* $@23  */
  YYSYMBOL_else_clause = 187,              /* else_clause  */
  YYSYMBOL_188_24 = 188,                   /* $@24  */
  YYSYMBOL_stmntlist = 189,                /* stmntlist  */
  YYSYMBOL_closing_paren = 190,            /* closing_paren  */
  YYSYMBOL_call_expression = 191,          /* call_expression  */
  YYSYMBOL_192_25 = 192,                   /* @25  */
  YYSYMBOL_193_26 = 193,                   /* @26  */
  YYSYMBOL_array_constructor = 194,        /* array_constructor  */
  YYSYMBOL_195_27 = 195,                   /* @27  */
  YYSYMBOL_map_constructor = 196,          /* map_constructor  */
  YYSYMBOL_197_28 = 197,                   /* @28  */
  YYSYMBOL_optional_map_associates = 198,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 199,           /* map_associates  */
  YYSYMBOL_map_associate = 200,            /* map_associate  */
  YYSYMBOL_201_29 = 201,                   /* @29  */
  YYSYMBOL_function_id_or_pointer = 202,   /* function_id_or_pointer  */
  YYSYMBOL_parameter = 203,                /* parameter  */
  YYSYMBOL_type_cast = 204,                /* type_cast  */
  YYSYMBOL_type_name = 205,                /* type_name  */
  YYSYMBOL_array_size = 206,               /* array_size  */
  YYSYMBOL_boolean_literal = 207,          /* boolean_literal  */
  YYSYMBOL_float_literal = 208,            /* float_literal  */
  YYSYMBOL_integer_literal = 209,          /* integer_literal  */
  YYSYMBOL_character_literal = 210,        /* character_literal  */
  YYSYMBOL_simple_literal = 211            /* simple_literal  */
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
#define YYLAST   843

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  135
/* YYNRULES -- Number of rules.  */
#define YYNRULES  263
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  416

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
       0,   365,   365,   365,   375,   376,   378,   379,   382,   385,
     386,   388,   391,   396,   397,   399,   404,   405,   406,   407,
     409,   410,   411,   413,   413,   415,   415,   417,   417,   419,
     423,   427,   428,   430,   434,   435,   437,   438,   440,   444,
     445,   448,   452,   453,   457,   460,   461,   463,   463,   463,
     477,   480,   486,   491,   497,   503,   504,   514,   517,   523,
     532,   541,   548,   550,   557,   564,   566,   571,   572,   575,
     577,   579,   580,   582,   583,   584,   588,   597,   603,   604,
     606,   606,   610,   612,   614,   618,   619,   620,   623,   623,
     654,   654,   680,   682,   683,   685,   686,   687,   688,   689,
     690,   691,   692,   693,   694,   695,   696,   697,   698,   699,
     702,   707,   711,   711,   717,   720,   723,   723,   730,   735,
     736,   737,   738,   739,   741,   743,   744,   746,   747,   747,
     749,   750,   753,   754,   757,   761,   762,   763,   765,   769,
     770,   771,   772,   775,   776,   781,   785,   786,   787,   788,
     789,   790,   791,   792,   793,   794,   795,   796,   797,   798,
     799,   800,   801,   802,   803,   804,   805,   806,   807,   808,
     809,   815,   816,   820,   824,   829,   830,   836,   837,   848,
     851,   858,   862,   863,   865,   866,   868,   869,   871,   873,
     875,   880,   883,   886,   890,   896,   903,   902,   919,   919,
     935,   936,   938,   940,   942,   938,   949,   950,   950,   956,
     959,   962,   968,   968,   981,   983,   981,   995,   997,   999,
    1002,  1005,  1002,  1012,  1016,  1016,  1023,  1024,  1026,  1028,
    1028,  1039,  1039,  1053,  1054,  1056,  1056,  1066,  1066,  1076,
    1078,  1081,  1083,  1086,  1086,  1094,  1104,  1109,  1110,  1115,
    1120,  1123,  1128,  1129,  1133,  1136,  1141,  1146,  1150,  1157,
    1162,  1170,  1178,  1186
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
  "struct_definition", "$@2", "enum_definition", "$@3",
  "native_type_definition", "$@4", "typedef_identifier",
  "struct_element_definitions", "struct_element", "enum_value_definitions",
  "enum_values", "enum_value", "opt_enum_assignment", "scope", "constants",
  "constant_list", "constdef", "$@5", "$@6", "constant_literal",
  "variable_identifier", "variable_reference", "macro_declaration",
  "macro_type", "macrostart", "global_var", "var_decl",
  "optional_description", "argument_list", "non_empty_arguments",
  "parameter_list", "non_empty_parameters", "argument_declaration",
  "annotations", "annotation", "$@7", "annotation_value",
  "annotation_values", "native_function_definition", "$@8",
  "macro_definition", "$@9", "block", "var_decls", "statement",
  "assignment", "$@10", "shorthand_assignment", "$@11",
  "assignment_target", "shorthand_assignment_operator",
  "assignment_expression", "expression", "range_expression", "$@12",
  "range_increment", "minusminus_plusplus", "increment_expression",
  "simple_term", "constructor_expression", "unary_expression",
  "binary_expression", "condition", "value", "string", "break", "continue",
  "return_expression", "optional_bterm", "opt_num", "case_clauses",
  "case_clause", "case_selector", "case_condition", "label",
  "goto_expression", "switch_expression", "$@13", "for_loop_expression",
  "$@14", "for_increment", "for_to_clause", "$@15", "$@16", "$@17",
  "opt_for_initializer", "$@18", "for_clause", "in_clause", "$@19",
  "while", "$@20", "$@21", "local_block", "closing_brace", "if_expression",
  "$@22", "$@23", "else_clause", "$@24", "stmntlist", "closing_paren",
  "call_expression", "@25", "@26", "array_constructor", "@27",
  "map_constructor", "@28", "optional_map_associates", "map_associates",
  "map_associate", "@29", "function_id_or_pointer", "parameter",
  "type_cast", "type_name", "array_size", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-281)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-247)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -281,    32,    23,  -281,    24,  -281,    22,    91,  -281,    82,
    -281,    30,    36,   116,  -281,  -281,  -281,    16,    97,    48,
      94,  -281,  -281,  -281,  -281,   101,  -281,  -281,  -281,    80,
    -281,    87,  -281,  -281,    16,   169,   166,   115,  -281,    87,
    -281,  -281,   118,   126,     1,  -281,  -281,  -281,  -281,  -281,
     138,   125,  -281,  -281,  -281,   133,  -281,   191,  -281,     4,
    -281,   162,   192,    97,  -281,  -281,  -281,  -281,    26,    12,
     191,  -281,   110,     3,   140,   136,  -281,   503,  -281,   172,
     149,  -281,   150,   152,    97,    87,  -281,  -281,  -281,  -281,
    -281,  -281,   207,  -281,  -281,  -281,  -281,  -281,  -281,   180,
    -281,   163,   160,  -281,  -281,  -281,  -281,  -281,   164,  -281,
    -281,  -281,   222,    45,  -281,   167,   193,   503,   503,   503,
       7,    76,  -281,  -281,   142,   162,  -281,  -281,  -281,  -281,
    -281,   546,  -281,   226,  -281,  -281,  -281,   168,   561,  -281,
      21,  -281,     1,   192,     1,  -281,   177,  -281,  -281,   231,
      12,  -281,   113,   404,   179,  -281,   750,   775,    81,  -281,
    -281,  -281,  -281,   114,   236,   616,   183,  -281,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   238,  -281,  -281,   185,  -281,  -281,  -281,   242,  -281,
    -281,  -281,  -281,  -281,  -281,  -281,   186,   190,  -281,    87,
     194,     9,   -40,   195,  -281,   248,   196,   249,   249,   503,
    -281,   252,   -15,   198,  -281,   316,   204,  -281,    75,   205,
    -281,  -281,   216,  -281,   221,  -281,  -281,  -281,  -281,   220,
     223,   202,   264,  -281,   503,   218,   224,  -281,  -281,   228,
     234,   237,  -281,  -281,  -281,   700,   775,   775,   775,   235,
     235,   725,   725,   165,   165,    83,   750,   750,   750,   775,
     165,   165,    81,    81,    83,    83,    83,   219,  -281,   264,
    -281,   289,   113,  -281,  -281,  -281,    97,  -281,  -281,  -281,
    -281,   503,  -281,   195,  -281,    77,  -281,   241,   243,   700,
    -281,  -281,  -281,   158,  -281,  -281,  -281,  -281,  -281,  -281,
     503,   295,  -281,  -281,  -281,  -281,  -281,  -281,  -281,   239,
    -281,  -281,   481,  -281,  -281,   236,   290,  -281,   239,  -281,
    -281,  -281,  -281,    87,   448,   645,   448,  -281,  -281,   240,
    -281,  -281,  -281,  -281,  -281,  -281,  -281,   503,   586,  -281,
    -281,  -281,  -281,    77,  -281,   503,  -281,  -281,   247,   244,
     360,  -281,  -281,  -281,  -281,   448,  -281,  -281,   162,   162,
    -281,  -281,   503,   162,   245,   220,   296,  -281,   259,  -281,
    -281,   674,  -281,   503,  -281,  -281,  -281,   130,  -281,   261,
     448,    20,   257,   268,   130,   316,    76,  -281,  -281,   293,
    -281,   265,  -281,  -281,  -281,  -281,  -281,   128,  -281,  -281,
    -281,   302,  -281,   269,  -281,  -281
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       2,     0,    11,     1,     0,    13,     0,    16,    12,     0,
      14,     4,     0,     0,    43,    47,     5,    78,     0,     0,
       0,    15,    25,    23,    27,     0,    21,    20,    22,    44,
      45,     0,    80,     3,    78,    90,    78,   250,    62,     0,
      17,    18,     0,     0,     0,    19,    47,    52,    53,    54,
       0,     0,     7,    88,     8,     0,    10,    42,    79,     0,
     251,    64,    34,     0,    30,    29,    28,    46,     0,    85,
      42,     9,     0,     0,     0,     0,   252,   127,    63,    39,
       0,    35,    36,     0,    31,     0,   257,   256,    51,   259,
     254,   255,     0,    48,   263,   261,   260,   262,    50,    83,
      84,    86,     0,    89,    59,    60,    57,    58,     0,    93,
      91,   253,     0,   176,   177,     0,   173,     0,     0,     0,
       0,     0,   235,   237,     0,     0,   124,   126,   143,   137,
     146,   125,   135,   174,   136,   233,   234,     0,     0,   175,
       0,    38,     0,     0,     0,    32,     0,   258,    49,     0,
      85,    81,    67,     0,     0,   229,   139,   140,   141,    56,
      55,   142,   245,     0,   239,     0,     0,   170,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     127,     0,   178,   231,   176,   173,   144,    40,     0,    26,
      37,    24,    33,    82,    87,    77,     0,    68,    69,     0,
       0,     0,    56,     0,   214,     0,     0,   184,   184,   182,
      95,    93,   116,   109,   106,     0,     0,   115,     0,     0,
     102,   103,     0,   108,     0,   101,   100,    99,    98,     0,
       0,     0,     0,    75,   127,     0,    72,   247,    73,     0,
       0,   240,   241,   169,   249,   128,   149,   168,   167,   158,
     159,   162,   161,   165,   166,   160,   150,   151,   153,   148,
     163,   164,   152,   154,   155,   156,   157,     0,   145,     0,
      41,    65,     0,    76,   110,   211,   206,   209,   198,   210,
     194,     0,   220,     0,   195,     0,   185,     0,     0,   183,
     181,   133,   132,     0,   111,   134,    94,   109,   227,    97,
       0,     0,   107,   104,   105,   219,    92,    96,   138,     0,
     248,   236,   127,   243,   238,     0,   130,   147,     0,    66,
      61,    70,   202,     0,     0,   172,     0,   215,   176,     0,
     179,   180,   122,   123,   119,   120,   121,   127,     0,   112,
     228,   230,    74,     0,   242,     0,   129,   232,     0,   207,
       0,   217,   199,   171,   221,     0,   196,   117,     0,     0,
     244,   131,     0,     0,     0,     0,   223,   216,     0,   114,
     113,     0,   208,   127,   218,   224,   222,   186,   203,     0,
       0,     0,     0,     0,   186,     0,     0,   213,   225,   257,
     192,     0,   191,   190,   197,   187,   188,   116,   201,   200,
     204,     0,   189,     0,   193,   205
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -281,  -281,  -281,  -281,   308,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,   -30,   266,
    -281,  -281,   206,  -281,  -281,   334,  -281,  -281,   305,  -281,
    -281,  -281,   -38,  -118,  -281,  -281,   278,  -281,    -9,  -281,
    -281,  -281,  -224,  -281,    70,   324,  -281,  -281,  -281,   212,
    -281,  -281,  -281,  -281,   291,  -281,  -280,  -281,  -281,   -33,
    -281,  -281,  -281,  -120,   -77,  -281,  -281,  -281,  -281,   -32,
     229,  -281,  -281,  -111,    73,  -273,  -281,  -281,  -281,  -281,
    -281,   151,   -24,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,  -281,  -281,  -249,    -4,  -281,  -281,  -281,
    -281,  -281,  -221,    44,  -139,  -281,  -281,  -281,  -281,  -281,
    -281,  -281,  -281,    50,  -281,  -281,    51,  -281,   -43,  -281,
    -281,  -281,   -14,  -281,   310
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    17,    33,    34,    54,     5,     7,    10,
      11,    25,    26,    43,    27,    42,    28,    44,    66,    83,
      84,    80,    81,    82,   141,    72,    19,    29,    30,    31,
     148,    93,    50,   125,   108,    73,    74,    20,   307,   330,
     206,   207,   245,   246,   208,    35,    36,    51,   101,   102,
      55,    70,    56,    57,   224,   153,   225,   226,   369,   227,
     303,   228,   347,    78,   247,   127,   326,   356,   305,   229,
     128,   129,   130,   131,   292,   132,   133,   230,   231,   232,
     300,   297,   393,   394,   395,   401,   233,   234,   235,   378,
     236,   334,   410,   287,   358,   396,   413,   332,   373,   288,
     289,   374,   237,   293,   365,   362,   316,   238,   336,   376,
     386,   390,   239,   351,   134,   242,   279,   135,   163,   136,
     164,   250,   251,   252,   353,   137,   248,   138,    39,    60,
      94,    95,    96,    97,   139
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     126,    61,   161,   162,   308,   167,   156,   157,   158,    38,
     285,   301,   302,   165,   240,    64,    -6,    65,   319,   106,
      85,    75,   339,   159,   147,    32,     4,   107,    99,   105,
      77,   100,     3,  -246,    13,   222,   290,   399,   197,   400,
       6,    85,    14,    86,    87,    88,    89,   146,    90,    91,
     160,    15,  -118,  -118,   361,   328,   361,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   276,    76,
     370,   166,   286,    92,   198,   361,   240,   364,    16,    92,
     -56,     8,   159,   338,    86,    87,   114,    89,     9,    90,
      91,    12,   304,    47,    48,    21,    49,   222,   299,   209,
     361,    37,   199,   277,   201,   243,   377,    40,  -246,   160,
     195,    22,    23,   104,    37,   112,    24,    37,   205,   244,
     113,    86,    87,   114,    89,   178,    90,    91,   115,   375,
      92,   398,   310,   311,   223,   187,   188,   189,   190,   191,
     190,   191,    46,   112,   301,   302,    37,   116,   113,    86,
      87,   114,    89,    41,    90,    91,   115,   320,   391,   392,
      45,   283,   117,   118,   406,    32,   119,   120,   121,    53,
     335,   122,    59,    68,   123,   116,  -127,   124,    62,   -71,
     342,   343,   344,   345,   346,   240,    63,   240,    69,   348,
     117,   118,    71,    14,   119,   120,   121,    77,    79,   122,
     109,   111,   123,   172,   173,   124,   222,   140,   222,   178,
     142,   240,   143,   144,   147,   149,   240,   185,   186,   187,
     188,   189,   190,   191,   151,   150,   154,   152,   -55,   209,
     155,   193,   222,   333,   371,   192,   202,   222,   379,   380,
     203,   240,   241,   382,   278,   249,   240,   254,  -246,   280,
     281,   381,   282,   284,   294,   243,   296,   306,   291,   295,
     367,  -237,   222,   309,   312,   112,   318,   222,   407,   244,
     113,    86,    87,   114,    89,   313,    90,    91,   115,   178,
     314,   315,   317,   321,   327,   359,   322,   185,   186,   187,
     188,   189,   190,   191,   323,   324,   389,   116,   329,   325,
     340,   349,   341,   350,   366,   355,   372,   210,   411,   414,
    -212,   383,   117,   118,   211,   385,   119,   120,   121,   387,
      37,   122,   212,   403,   123,   397,  -127,   124,   -71,   404,
     115,   412,    52,   415,   213,    18,   214,   215,   103,   200,
     145,    67,   331,   216,  -226,  -226,   217,   218,   219,   160,
      58,   210,   204,   408,   409,   110,   337,   196,   211,   298,
     405,   384,   357,   352,    37,   354,   212,   402,    98,  -237,
     121,     0,     0,   122,   115,   220,   221,  -226,   213,     0,
     214,   215,     0,     0,     0,     0,     0,   216,     0,     0,
     217,   218,   219,   160,     0,   210,     0,     0,     0,     0,
       0,     0,   211,     0,     0,     0,     0,     0,    37,     0,
     212,     0,     0,     0,   121,     0,     0,   122,   115,   220,
     221,   -93,   213,     0,   214,   215,     0,     0,     0,     0,
       0,   216,     0,     0,   217,   218,   219,   160,     0,   210,
       0,     0,     0,     0,     0,     0,   211,     0,     0,     0,
       0,     0,    37,     0,   212,     0,     0,     0,   121,     0,
       0,   122,   115,   220,   221,  -226,   213,     0,   214,   215,
       0,     0,     0,     0,     0,   216,     0,     0,   217,   218,
     219,   160,   112,     0,     0,     0,   244,   113,    86,    87,
     114,    89,     0,    90,    91,   115,     0,     0,     0,     0,
       0,     0,   121,     0,   112,   122,     0,   220,   360,   113,
      86,    87,   114,    89,   116,    90,    91,   115,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   117,
     118,     0,     0,   119,   120,   121,   116,     0,   122,     0,
       0,   123,     0,     0,   124,     0,     0,     0,     0,     0,
       0,   117,   118,     0,     0,   119,   120,   121,     0,     0,
     122,   168,   112,   123,     0,     0,   124,   194,    86,    87,
     114,    89,     0,    90,    91,   115,     0,     0,     0,     0,
     169,     0,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   195,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    92,   121,     0,     0,   122,     0,
     169,   123,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,     0,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,     0,     0,     0,     0,     0,
     169,   368,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,     0,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,     0,     0,     0,     0,   169,
     253,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,     0,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,     0,     0,     0,     0,   169,   363,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,     0,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   388,   169,     0,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,     0,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   169,
       0,   170,   171,   172,   173,     0,     0,   176,   177,   178,
     179,   180,   181,     0,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   169,     0,   170,   171,   172,   173,
       0,     0,   176,   177,   178,     0,     0,     0,     0,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,  -247,
       0,  -247,  -247,   172,   173,     0,     0,   176,   177,   178,
       0,     0,     0,     0,  -247,   183,   184,   185,   186,   187,
     188,   189,   190,   191
};

static const yytype_int16 yycheck[] =
{
      77,    39,   120,   121,   225,   125,   117,   118,   119,    18,
       1,    26,    27,   124,   153,    14,     0,    16,   242,    16,
      63,    17,   295,    16,    17,     9,     3,    24,    16,    72,
      45,    19,     0,    73,     4,   153,    76,    17,    17,    19,
      16,    84,    12,    17,    18,    19,    20,    85,    22,    23,
      43,    21,    67,    68,   334,   279,   336,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,    75,
     353,   124,    73,    63,    63,   365,   225,   336,    58,    63,
      45,    69,    16,    16,    17,    18,    19,    20,     7,    22,
      23,    19,   222,    16,    17,    69,    19,   225,   219,   152,
     390,    14,   142,   190,   144,     1,   365,    69,    73,    43,
      43,     5,     6,    13,    14,    11,    10,    14,    15,    15,
      16,    17,    18,    19,    20,    54,    22,    23,    24,   360,
      63,   390,    67,    68,   153,    64,    65,    66,    67,    68,
      67,    68,    72,    11,    26,    27,    14,    43,    16,    17,
      18,    19,    20,    69,    22,    23,    24,   244,    38,    39,
      69,   209,    58,    59,   395,     9,    62,    63,    64,    10,
     291,    67,    67,    45,    70,    43,    72,    73,    70,    75,
      32,    33,    34,    35,    36,   334,    70,   336,    73,   310,
      58,    59,    69,    12,    62,    63,    64,    45,    16,    67,
      70,    75,    70,    48,    49,    73,   334,    45,   336,    54,
      71,   360,    72,    71,    17,    45,   365,    62,    63,    64,
      65,    66,    67,    68,    74,    72,    14,    73,    45,   282,
      73,    73,   360,   286,   355,    19,    69,   365,   368,   369,
      19,   390,    73,   373,    16,    19,   395,    74,    73,    17,
      74,   372,    72,    69,    16,     1,    17,    69,    73,    73,
     347,    19,   390,    69,    69,    11,    74,   395,   396,    15,
      16,    17,    18,    19,    20,    69,    22,    23,    24,    54,
      69,    71,    69,    75,    75,   333,    72,    62,    63,    64,
      65,    66,    67,    68,    76,    71,   383,    43,    19,    72,
      69,    16,    69,    74,    74,    25,    69,     1,    25,    17,
      76,    76,    58,    59,     8,    29,    62,    63,    64,    70,
      14,    67,    16,    76,    70,    74,    72,    73,    74,    71,
      24,    76,    34,    74,    28,    11,    30,    31,    70,   143,
      84,    46,   282,    37,    38,    39,    40,    41,    42,    43,
      36,     1,   150,   396,   396,    74,   293,   138,     8,   218,
     394,   375,   328,   322,    14,   325,    16,   391,    68,    19,
      64,    -1,    -1,    67,    24,    69,    70,    71,    28,    -1,
      30,    31,    -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,
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
      -1,    70,    -1,    -1,    73,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    59,    -1,    -1,    62,    63,    64,    -1,    -1,
      67,    25,    11,    70,    -1,    -1,    73,    16,    17,    18,
      19,    20,    -1,    22,    23,    24,    -1,    -1,    -1,    -1,
      44,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    43,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    63,    64,    -1,    -1,    67,    -1,
      44,    70,    46,    47,    48,    49,    50,    51,    52,    53,
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
      65,    66,    67,    68
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    78,    79,     0,     3,    84,    16,    85,    69,     7,
      86,    87,    19,     4,    12,    21,    58,    80,   102,   103,
     114,    69,     5,     6,    10,    88,    89,    91,    93,   104,
     105,   106,     9,    81,    82,   122,   123,    14,   115,   205,
      69,    69,    92,    90,    94,    69,    72,    16,    17,    19,
     109,   124,    81,    10,    83,   127,   129,   130,   122,    67,
     206,   109,    70,    70,    14,    16,    95,   105,    45,    73,
     128,    69,   102,   112,   113,    17,    75,    45,   140,    16,
      98,    99,   100,    96,    97,   205,    17,    18,    19,    20,
      22,    23,    63,   108,   207,   208,   209,   210,   211,    16,
      19,   125,   126,   113,    13,   205,    16,    24,   111,    70,
     131,    75,    11,    16,    19,    24,    43,    58,    59,    62,
      63,    64,    67,    70,    73,   110,   141,   142,   147,   148,
     149,   150,   152,   153,   191,   194,   196,   202,   204,   211,
      45,   101,    71,    72,    71,    96,   109,    17,   107,    45,
      72,    74,    73,   132,    14,    73,   150,   150,   150,    16,
      43,   110,   110,   195,   197,   150,   205,   140,    25,    44,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    19,    73,    16,    43,   147,    17,    63,    95,
      99,    95,    69,    19,   126,    15,   117,   118,   121,   205,
       1,     8,    16,    28,    30,    31,    37,    40,    41,    42,
      69,    70,   110,   115,   131,   133,   134,   136,   138,   146,
     154,   155,   156,   163,   164,   165,   167,   179,   184,   189,
     191,    73,   192,     1,    15,   119,   120,   141,   203,    19,
     198,   199,   200,    74,    74,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   141,    16,   193,
      17,    74,    72,   109,    69,     1,    73,   170,   176,   177,
      76,    73,   151,   180,    16,    73,    17,   158,   158,   150,
     157,    26,    27,   137,   140,   145,    69,   115,   189,    69,
      67,    68,    69,    69,    69,    71,   183,    69,    74,   119,
     141,    75,    72,    76,    71,    72,   143,    75,   119,    19,
     116,   121,   174,   205,   168,   150,   185,   151,    16,   152,
      69,    69,    32,    33,    34,    35,    36,   139,   150,    16,
      74,   190,   203,   201,   200,    25,   144,   190,   171,   109,
      70,   133,   182,    74,   182,   181,    74,   141,    75,   135,
     152,   150,    69,   175,   178,   189,   186,   182,   166,   140,
     140,   150,   140,    76,   183,    29,   187,    70,    69,   141,
     188,    38,    39,   159,   160,   161,   172,    74,   182,    17,
      19,   162,   209,    76,    71,   159,   189,   110,   136,   146,
     169,    25,    76,   173,    17,    74
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    77,    79,    78,    80,    80,    81,    81,    82,    83,
      83,    84,    84,    85,    85,    86,    87,    87,    87,    87,
      88,    88,    88,    90,    89,    92,    91,    94,    93,    95,
      95,    96,    96,    97,    98,    98,    99,    99,   100,   101,
     101,   101,   102,   102,   103,   104,   104,   106,   107,   105,
     108,   108,   109,   109,   109,   110,   110,   111,   111,   112,
     112,   113,   114,   115,   115,   116,   116,   117,   117,   118,
     118,   119,   119,   120,   120,   120,   121,   121,   122,   122,
     124,   123,   125,   125,   125,   126,   126,   126,   128,   127,
     130,   129,   131,   132,   132,   133,   133,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   134,   135,   134,   134,   134,   137,   136,   138,   139,
     139,   139,   139,   139,   140,   141,   141,   142,   143,   142,
     144,   144,   145,   145,   146,   147,   147,   147,   148,   149,
     149,   149,   149,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   150,
     150,   151,   151,   152,   152,   152,   152,   153,   153,   154,
     155,   156,   157,   157,   158,   158,   159,   159,   160,   161,
     161,   162,   162,   162,   163,   164,   166,   165,   168,   167,
     169,   169,   171,   172,   173,   170,   174,   175,   174,   176,
     176,   176,   178,   177,   180,   181,   179,   182,   182,   183,
     185,   186,   184,   187,   188,   187,   189,   189,   190,   192,
     191,   193,   191,   191,   191,   195,   194,   197,   196,   198,
     198,   199,   199,   201,   200,   202,   202,   203,   203,   204,
     205,   205,   206,   206,   207,   207,   208,   209,   209,   210,
     211,   211,   211,   211
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     6,     0,     1,     0,     2,     2,     2,
       1,     0,     3,     0,     2,     3,     0,     3,     3,     4,
       1,     1,     1,     0,     6,     0,     6,     0,     3,     1,
       1,     1,     2,     3,     0,     1,     1,     3,     2,     0,
       2,     3,     0,     1,     2,     1,     3,     0,     0,     5,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     6,     2,     3,     2,     0,     1,     0,     1,     1,
       3,     0,     1,     1,     3,     1,     2,     1,     0,     2,
       0,     5,     3,     1,     1,     0,     1,     3,     0,     3,
       0,     3,     4,     0,     3,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     2,     2,     1,     2,     1,     1,
       2,     2,     0,     5,     5,     1,     0,     4,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     0,     0,     5,
       0,     2,     1,     1,     2,     1,     1,     1,     4,     2,
       2,     2,     2,     1,     2,     3,     1,     4,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     2,     1,     1,     1,     1,     1,     2,     3,
       3,     2,     0,     1,     0,     1,     0,     2,     2,     3,
       2,     1,     1,     3,     2,     2,     0,     8,     0,     4,
       1,     1,     0,     0,     0,    10,     0,     0,     4,     1,
       1,     1,     0,     7,     0,     0,     5,     1,     3,     1,
       0,     0,     6,     0,     0,     3,     0,     2,     1,     0,
       5,     0,     5,     1,     1,     0,     4,     0,     4,     0,
       1,     1,     3,     0,     4,     2,     1,     1,     2,     3,
       1,     2,     2,     3,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1
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
#line 365 "Parser.y"
                {
				init_header();
			}
#line 2257 "parser.c"
    break;

  case 3: /* file_structure: $@1 namespace prerequisites global_declarations header_delimiter macro_definitions  */
#line 371 "Parser.y"
                                          {
				finalize_header();
			}
#line 2265 "parser.c"
    break;

  case 11: /* namespace: %empty  */
#line 388 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2273 "parser.c"
    break;

  case 12: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 391 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2282 "parser.c"
    break;

  case 15: /* require: T_REQUIRE T_STRING ';'  */
#line 399 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2291 "parser.c"
    break;

  case 23: /* $@2: %empty  */
#line 413 "Parser.y"
                            { parser_startTypeDescriptor(0); }
#line 2297 "parser.c"
    break;

  case 25: /* $@3: %empty  */
#line 415 "Parser.y"
                        { parser_startTypeDescriptor(1); }
#line 2303 "parser.c"
    break;

  case 27: /* $@4: %empty  */
#line 417 "Parser.y"
                                 { parser_startTypeDescriptor(0); }
#line 2309 "parser.c"
    break;

  case 29: /* typedef_identifier: T_IDENT  */
#line 419 "Parser.y"
                            {
				parser_finalizeTypeDescriptor(yyvsp[0].ident.s);
			}
#line 2317 "parser.c"
    break;

  case 30: /* typedef_identifier: T_TYPE_IDENTIFIER  */
#line 423 "Parser.y"
                                          {
				parser_finalizeTypeDescriptor(_strdup(types_nameFor(yyvsp[0].ident.type)));
			}
#line 2325 "parser.c"
    break;

  case 33: /* struct_element: type_name variable_identifier ';'  */
#line 430 "Parser.y"
                                                  {
				parser_addTypeProperty(yyvsp[-1].ident.s, (long)yyvsp[-2].num);
			}
#line 2333 "parser.c"
    break;

  case 38: /* enum_value: T_IDENT opt_enum_assignment  */
#line 440 "Parser.y"
                                            {
				parser_addTypeProperty(yyvsp[-1].ident.s, (long)yyvsp[0].num);
			}
#line 2341 "parser.c"
    break;

  case 40: /* opt_enum_assignment: T_ASSIGN T_NUM  */
#line 445 "Parser.y"
                                         {
				yyval.num = yyvsp[0].num;
			}
#line 2349 "parser.c"
    break;

  case 41: /* opt_enum_assignment: T_ASSIGN '-' T_NUM  */
#line 448 "Parser.y"
                                             {
				yyval.num = -yyvsp[0].num;
			}
#line 2357 "parser.c"
    break;

  case 42: /* scope: %empty  */
#line 452 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2363 "parser.c"
    break;

  case 43: /* scope: T_STATIC  */
#line 453 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2369 "parser.c"
    break;

  case 47: /* $@5: %empty  */
#line 463 "Parser.y"
                { _bDefiningConst = 1; }
#line 2375 "parser.c"
    break;

  case 48: /* $@6: %empty  */
#line 463 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2381 "parser.c"
    break;

  case 49: /* constdef: $@5 variable_identifier T_ASSIGN constant_literal $@6  */
#line 464 "Parser.y"
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
#line 2397 "parser.c"
    break;

  case 50: /* constant_literal: simple_literal  */
#line 477 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2405 "parser.c"
    break;

  case 51: /* constant_literal: T_STRING  */
#line 480 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2414 "parser.c"
    break;

  case 52: /* variable_identifier: T_IDENT  */
#line 486 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2424 "parser.c"
    break;

  case 53: /* variable_identifier: T_NUM  */
#line 491 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2435 "parser.c"
    break;

  case 54: /* variable_identifier: T_STRING  */
#line 497 "Parser.y"
                                   {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2444 "parser.c"
    break;

  case 55: /* variable_reference: T_VARIABLE  */
#line 503 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2450 "parser.c"
    break;

  case 56: /* variable_reference: T_IDENT  */
#line 504 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2462 "parser.c"
    break;

  case 57: /* macro_declaration: T_IDENT  */
#line 514 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2470 "parser.c"
    break;

  case 58: /* macro_declaration: T_FUNC  */
#line 517 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2480 "parser.c"
    break;

  case 59: /* macro_type: scope T_VOID  */
#line 523 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
				yyval.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(VT_NIL);
				}
			}
#line 2493 "parser.c"
    break;

  case 60: /* macro_type: scope type_name  */
#line 532 "Parser.y"
                                        {
				yyval.ident.type = yyvsp[0].ident.type;
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(yyvsp[0].ident.type);
				}
			}
#line 2506 "parser.c"
    break;

  case 61: /* macrostart: macro_type macro_declaration '(' argument_list ')' optional_description  */
#line 542 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				yyval.ident.type = yyvsp[-5].ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2516 "parser.c"
    break;

  case 63: /* var_decl: type_name variable_identifier assignment_expression  */
#line 550 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});
			}
#line 2529 "parser.c"
    break;

  case 64: /* var_decl: type_name variable_identifier  */
#line 557 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2540 "parser.c"
    break;

  case 66: /* optional_description: T_STRING  */
#line 566 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2549 "parser.c"
    break;

  case 75: /* non_empty_parameters: error  */
#line 584 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 2557 "parser.c"
    break;

  case 76: /* argument_declaration: type_name variable_identifier  */
#line 588 "Parser.y"
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
#line 2572 "parser.c"
    break;

  case 77: /* argument_declaration: T_ELLIPSIS  */
#line 597 "Parser.y"
                                       {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[0].ident.type, "...");
				}
			}
#line 2582 "parser.c"
    break;

  case 80: /* $@7: %empty  */
#line 606 "Parser.y"
                         {
				start_annotation(yyvsp[0].ident.s);
			}
#line 2590 "parser.c"
    break;

  case 82: /* annotation_value: T_IDENT T_ASSIGN T_STRING  */
#line 610 "Parser.y"
                                            {
				parser_defineAnnotationValue(yyvsp[-2].ident.s, yyvsp[0].ident.s);
			}
#line 2598 "parser.c"
    break;

  case 83: /* annotation_value: T_IDENT  */
#line 612 "Parser.y"
                                    {
				parser_defineAnnotationValue(yyvsp[0].ident.s, _strdup("true"));
			}
#line 2606 "parser.c"
    break;

  case 84: /* annotation_value: T_STRING  */
#line 614 "Parser.y"
                                     {
				parser_defineAnnotationValue(_strdup("value"), yyvsp[0].ident.s);
			}
#line 2614 "parser.c"
    break;

  case 88: /* $@8: %empty  */
#line 623 "Parser.y"
                                         {
					init_native();
				}
#line 2622 "parser.c"
    break;

  case 89: /* native_function_definition: T_NATIVE $@8 macrostart  */
#line 625 "Parser.y"
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
#line 2655 "parser.c"
    break;

  case 90: /* $@9: %empty  */
#line 654 "Parser.y"
                  { init_macroDefinition(); }
#line 2661 "parser.c"
    break;

  case 91: /* macro_definition: $@9 macrostart block  */
#line 655 "Parser.y"
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
#line 2690 "parser.c"
    break;

  case 96: /* statement: call_expression ';'  */
#line 686 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2696 "parser.c"
    break;

  case 97: /* statement: assignment ';'  */
#line 687 "Parser.y"
                                         { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2702 "parser.c"
    break;

  case 98: /* statement: if_expression  */
#line 688 "Parser.y"
                                        { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2708 "parser.c"
    break;

  case 99: /* statement: while  */
#line 689 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2714 "parser.c"
    break;

  case 100: /* statement: for_loop_expression  */
#line 690 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2720 "parser.c"
    break;

  case 101: /* statement: switch_expression  */
#line 691 "Parser.y"
                                            { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2726 "parser.c"
    break;

  case 106: /* statement: block  */
#line 696 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2732 "parser.c"
    break;

  case 107: /* statement: increment_expression ';'  */
#line 697 "Parser.y"
                                                   { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2738 "parser.c"
    break;

  case 109: /* statement: var_decl  */
#line 699 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2746 "parser.c"
    break;

  case 110: /* statement: error ';'  */
#line 702 "Parser.y"
                                    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2755 "parser.c"
    break;

  case 111: /* assignment: variable_reference assignment_expression  */
#line 707 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2763 "parser.c"
    break;

  case 112: /* $@10: %empty  */
#line 711 "Parser.y"
                                                      {
				parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2771 "parser.c"
    break;

  case 113: /* assignment: assignment_target '.' T_IDENT $@10 assignment_expression  */
#line 713 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2779 "parser.c"
    break;

  case 114: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 717 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2787 "parser.c"
    break;

  case 116: /* $@11: %empty  */
#line 723 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2795 "parser.c"
    break;

  case 117: /* shorthand_assignment: variable_reference $@11 shorthand_assignment_operator expression  */
#line 725 "Parser.y"
                                                                   {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2804 "parser.c"
    break;

  case 118: /* assignment_target: variable_reference  */
#line 730 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2812 "parser.c"
    break;

  case 119: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 735 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2818 "parser.c"
    break;

  case 120: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 736 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2824 "parser.c"
    break;

  case 121: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 737 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2830 "parser.c"
    break;

  case 122: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 738 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2836 "parser.c"
    break;

  case 123: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 739 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2842 "parser.c"
    break;

  case 124: /* assignment_expression: T_ASSIGN expression  */
#line 741 "Parser.y"
                                                { yyval.v = yyvsp[0].v;	}
#line 2848 "parser.c"
    break;

  case 125: /* expression: binary_expression  */
#line 743 "Parser.y"
                              { yyval.v = yyvsp[0].v; }
#line 2854 "parser.c"
    break;

  case 128: /* $@12: %empty  */
#line 747 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2860 "parser.c"
    break;

  case 131: /* range_increment: T_DOTDOT binary_expression  */
#line 750 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2866 "parser.c"
    break;

  case 132: /* minusminus_plusplus: T_MINUSMINUS  */
#line 753 "Parser.y"
                                     { yyval.num = -1; }
#line 2872 "parser.c"
    break;

  case 133: /* minusminus_plusplus: T_PLUSPLUS  */
#line 754 "Parser.y"
                                     { yyval.num = 1; }
#line 2878 "parser.c"
    break;

  case 134: /* increment_expression: variable_reference minusminus_plusplus  */
#line 757 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2886 "parser.c"
    break;

  case 135: /* simple_term: value  */
#line 761 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2892 "parser.c"
    break;

  case 138: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 765 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2900 "parser.c"
    break;

  case 139: /* unary_expression: '!' binary_expression  */
#line 769 "Parser.y"
                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2906 "parser.c"
    break;

  case 140: /* unary_expression: '~' binary_expression  */
#line 770 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2912 "parser.c"
    break;

  case 141: /* unary_expression: '+' binary_expression  */
#line 771 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2918 "parser.c"
    break;

  case 142: /* unary_expression: '-' variable_reference  */
#line 772 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2924 "parser.c"
    break;

  case 143: /* binary_expression: simple_term  */
#line 775 "Parser.y"
                                    { yyval.ident.type = yyvsp[0].ident.type;}
#line 2930 "parser.c"
    break;

  case 144: /* binary_expression: type_cast simple_term  */
#line 776 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2939 "parser.c"
    break;

  case 145: /* binary_expression: binary_expression '.' T_IDENT  */
#line 781 "Parser.y"
                                                      {
				yyval.ident.type = parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2948 "parser.c"
    break;

  case 147: /* binary_expression: binary_expression '[' expression ']'  */
#line 786 "Parser.y"
                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2954 "parser.c"
    break;

  case 148: /* binary_expression: binary_expression '~' binary_expression  */
#line 787 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2960 "parser.c"
    break;

  case 149: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 788 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2966 "parser.c"
    break;

  case 150: /* binary_expression: binary_expression '&' binary_expression  */
#line 789 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2972 "parser.c"
    break;

  case 151: /* binary_expression: binary_expression '|' binary_expression  */
#line 790 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2978 "parser.c"
    break;

  case 152: /* binary_expression: binary_expression '+' binary_expression  */
#line 791 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2984 "parser.c"
    break;

  case 153: /* binary_expression: binary_expression '^' binary_expression  */
#line 792 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2990 "parser.c"
    break;

  case 154: /* binary_expression: binary_expression '-' binary_expression  */
#line 793 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2996 "parser.c"
    break;

  case 155: /* binary_expression: binary_expression '*' binary_expression  */
#line 794 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 3002 "parser.c"
    break;

  case 156: /* binary_expression: binary_expression '/' binary_expression  */
#line 795 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 3008 "parser.c"
    break;

  case 157: /* binary_expression: binary_expression '%' binary_expression  */
#line 796 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 3014 "parser.c"
    break;

  case 158: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 797 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 3020 "parser.c"
    break;

  case 159: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 798 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 3026 "parser.c"
    break;

  case 160: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 799 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 3032 "parser.c"
    break;

  case 161: /* binary_expression: binary_expression T_AND binary_expression  */
#line 800 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 3038 "parser.c"
    break;

  case 162: /* binary_expression: binary_expression T_OR binary_expression  */
#line 801 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 3044 "parser.c"
    break;

  case 163: /* binary_expression: binary_expression '<' binary_expression  */
#line 802 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 3050 "parser.c"
    break;

  case 164: /* binary_expression: binary_expression '>' binary_expression  */
#line 803 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 3056 "parser.c"
    break;

  case 165: /* binary_expression: binary_expression T_LE binary_expression  */
#line 804 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 3062 "parser.c"
    break;

  case 166: /* binary_expression: binary_expression T_GE binary_expression  */
#line 805 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 3068 "parser.c"
    break;

  case 167: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 806 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 3074 "parser.c"
    break;

  case 168: /* binary_expression: binary_expression T_NE binary_expression  */
#line 807 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 3080 "parser.c"
    break;

  case 169: /* binary_expression: '(' binary_expression ')'  */
#line 808 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 3086 "parser.c"
    break;

  case 170: /* binary_expression: variable_reference assignment_expression  */
#line 809 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 3094 "parser.c"
    break;

  case 172: /* condition: '(' binary_expression  */
#line 816 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 3102 "parser.c"
    break;

  case 173: /* value: T_VARIABLE  */
#line 820 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 3111 "parser.c"
    break;

  case 174: /* value: string  */
#line 824 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 3121 "parser.c"
    break;

  case 175: /* value: simple_literal  */
#line 829 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 3127 "parser.c"
    break;

  case 176: /* value: T_IDENT  */
#line 830 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 3137 "parser.c"
    break;

  case 177: /* string: T_STRING  */
#line 836 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 3143 "parser.c"
    break;

  case 178: /* string: string T_STRING  */
#line 837 "Parser.y"
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
#line 3158 "parser.c"
    break;

  case 179: /* break: T_BREAK opt_num ';'  */
#line 849 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 3164 "parser.c"
    break;

  case 180: /* continue: T_CONTINUE opt_num ';'  */
#line 851 "Parser.y"
                                       {
				// in the case this is a continue in a for(int i;b;i++) loop, generate the i++ code before continuing.
				parser_flushBuffer(_breaklevel-(int)yyvsp[-1].num+1);
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); 
			}
#line 3174 "parser.c"
    break;

  case 181: /* return_expression: T_RETURN optional_bterm  */
#line 858 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 3182 "parser.c"
    break;

  case 184: /* opt_num: %empty  */
#line 865 "Parser.y"
                {	yyval.num = 1;	}
#line 3188 "parser.c"
    break;

  case 185: /* opt_num: T_NUM  */
#line 866 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 3194 "parser.c"
    break;

  case 190: /* case_selector: T_DEFAULT ':'  */
#line 875 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 3202 "parser.c"
    break;

  case 191: /* case_condition: integer_literal  */
#line 880 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 3210 "parser.c"
    break;

  case 192: /* case_condition: T_STRING  */
#line 883 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3218 "parser.c"
    break;

  case 193: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 886 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3226 "parser.c"
    break;

  case 194: /* label: T_IDENT ':'  */
#line 890 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3235 "parser.c"
    break;

  case 195: /* goto_expression: T_GOTO T_IDENT  */
#line 896 "Parser.y"
                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3244 "parser.c"
    break;

  case 196: /* $@13: %empty  */
#line 903 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3254 "parser.c"
    break;

  case 197: /* switch_expression: T_SWITCH '(' value ')' $@13 '{' case_clauses '}'  */
#line 909 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3267 "parser.c"
    break;

  case 198: /* $@14: %empty  */
#line 919 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3276 "parser.c"
    break;

  case 199: /* for_loop_expression: T_FOR for_clause $@14 local_block  */
#line 922 "Parser.y"
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
#line 3293 "parser.c"
    break;

  case 202: /* $@15: %empty  */
#line 938 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3301 "parser.c"
    break;

  case 203: /* $@16: %empty  */
#line 940 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3309 "parser.c"
    break;

  case 204: /* $@17: %empty  */
#line 942 "Parser.y"
                                        {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			}
#line 3318 "parser.c"
    break;

  case 205: /* for_to_clause: '(' opt_for_initializer $@15 ';' binary_expression ';' $@16 for_increment $@17 ')'  */
#line 945 "Parser.y"
                              {
				yyval.ident.s = yyvsp[-8].ident.s;
			}
#line 3326 "parser.c"
    break;

  case 207: /* $@18: %empty  */
#line 950 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3334 "parser.c"
    break;

  case 208: /* opt_for_initializer: type_name variable_identifier $@18 assignment_expression  */
#line 952 "Parser.y"
                                                {
				yyval.ident.s = yyvsp[-2].ident.s;
			}
#line 3342 "parser.c"
    break;

  case 209: /* for_clause: for_to_clause  */
#line 956 "Parser.y"
                              {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3350 "parser.c"
    break;

  case 210: /* for_clause: in_clause  */
#line 959 "Parser.y"
                                    {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3358 "parser.c"
    break;

  case 211: /* for_clause: error  */
#line 962 "Parser.y"
                                {
				yyval.ident.s = 0;
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3368 "parser.c"
    break;

  case 212: /* $@19: %empty  */
#line 968 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				parser_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3379 "parser.c"
    break;

  case 213: /* in_clause: '(' type_name variable_identifier $@19 ':' expression ')'  */
#line 973 "Parser.y"
                                             {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
				yyval.ident.s = yyvsp[-4].ident.s;
			}
#line 3391 "parser.c"
    break;

  case 214: /* $@20: %empty  */
#line 981 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3399 "parser.c"
    break;

  case 215: /* $@21: %empty  */
#line 983 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3408 "parser.c"
    break;

  case 216: /* while: T_WHILE $@20 condition $@21 local_block  */
#line 986 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3420 "parser.c"
    break;

  case 220: /* $@22: %empty  */
#line 1002 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3429 "parser.c"
    break;

  case 221: /* $@23: %empty  */
#line 1005 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3437 "parser.c"
    break;

  case 222: /* if_expression: T_IF condition $@22 local_block $@23 else_clause  */
#line 1007 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3446 "parser.c"
    break;

  case 223: /* else_clause: %empty  */
#line 1012 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3455 "parser.c"
    break;

  case 224: /* $@24: %empty  */
#line 1016 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3465 "parser.c"
    break;

  case 229: /* @25: %empty  */
#line 1028 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3477 "parser.c"
    break;

  case 230: /* call_expression: T_FUNC '(' @25 parameter_list closing_paren  */
#line 1034 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3487 "parser.c"
    break;

  case 231: /* @26: %empty  */
#line 1039 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3498 "parser.c"
    break;

  case 232: /* call_expression: function_id_or_pointer '(' @26 parameter_list closing_paren  */
#line 1044 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3512 "parser.c"
    break;

  case 235: /* @27: %empty  */
#line 1056 "Parser.y"
                       {
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = &_functionTable[FUNC_CreateArray];
			}
#line 3522 "parser.c"
    break;

  case 236: /* array_constructor: '[' @27 parameter_list ']'  */
#line 1060 "Parser.y"
                                             {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
					C_0FUNC,(GENERIC_DATA){FUNC_CreateArray}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
			}
#line 3532 "parser.c"
    break;

  case 237: /* @28: %empty  */
#line 1066 "Parser.y"
                     {
		_currentFunctionCallParamIndexP++;
		*_currentFunctionCallParamIndexP = 0;
		yyval.funcp = &_functionTable[FUNC_CreateMap];
	}
#line 3542 "parser.c"
    break;

  case 238: /* map_constructor: '{' @28 optional_map_associates '}'  */
#line 1070 "Parser.y"
                                      {
		_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
			C_0FUNC,(GENERIC_DATA){FUNC_CreateMap}, *_currentFunctionCallParamIndexP);
		_currentFunctionCallParamIndexP--;
	}
#line 3552 "parser.c"
    break;

  case 243: /* @29: %empty  */
#line 1086 "Parser.y"
                                     { 
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[-1].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[-1].ident.s);
				(*_currentFunctionCallParamIndexP) += 2;
			}
#line 3563 "parser.c"
    break;

  case 245: /* function_id_or_pointer: '*' variable_reference  */
#line 1094 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3577 "parser.c"
    break;

  case 246: /* function_id_or_pointer: T_IDENT  */
#line 1104 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3586 "parser.c"
    break;

  case 247: /* parameter: expression  */
#line 1109 "Parser.y"
                           { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3592 "parser.c"
    break;

  case 248: /* parameter: T_ELLIPSIS expression  */
#line 1110 "Parser.y"
                                                { 
				yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; 
				YY_EMIT(C_SPREAD, (GENERIC_DATA){1});
			}
#line 3601 "parser.c"
    break;

  case 249: /* type_cast: '(' type_name ')'  */
#line 1115 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3610 "parser.c"
    break;

  case 250: /* type_name: T_TYPE_IDENTIFIER  */
#line 1120 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3618 "parser.c"
    break;

  case 251: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 1123 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3627 "parser.c"
    break;

  case 253: /* array_size: '[' T_NUM ']'  */
#line 1129 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3635 "parser.c"
    break;

  case 254: /* boolean_literal: T_TRUE  */
#line 1133 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3644 "parser.c"
    break;

  case 255: /* boolean_literal: T_FALSE  */
#line 1136 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3653 "parser.c"
    break;

  case 256: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 1141 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3662 "parser.c"
    break;

  case 257: /* integer_literal: T_NUM  */
#line 1146 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3671 "parser.c"
    break;

  case 258: /* integer_literal: '-' T_NUM  */
#line 1150 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3681 "parser.c"
    break;

  case 259: /* character_literal: T_CHARACTER  */
#line 1157 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3690 "parser.c"
    break;

  case 260: /* simple_literal: integer_literal  */
#line 1162 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3703 "parser.c"
    break;

  case 261: /* simple_literal: float_literal  */
#line 1170 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3716 "parser.c"
    break;

  case 262: /* simple_literal: character_literal  */
#line 1178 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3729 "parser.c"
    break;

  case 263: /* simple_literal: boolean_literal  */
#line 1186 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3742 "parser.c"
    break;


#line 3746 "parser.c"

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

#line 1194 "Parser.y"


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

