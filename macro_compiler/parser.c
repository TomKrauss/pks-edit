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
static ANNOTATION	_currentMethodAnnotation;
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
	if (aLevel) {
		_currentBytecodeBuffer->bb_current = _currentBytecodeBuffer->bb_start;
	}
}

static void parser_flushBuffer(int aLevel) {
	if (aLevel <= 0 ||aLevel >= DIM(_buffers) || !_buffers[aLevel].bb_start) {
		return;
	}
	size_t nBytes = _buffers[aLevel].bb_current - _buffers[aLevel].bb_start;
	if (nBytes != 0) {
		memcpy(_buffers[0].bb_current, _buffers[aLevel].bb_start, nBytes);
		_buffers[0].bb_current += nBytes;
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

static void destroy_annotation() {
	if (_currentMethodAnnotation.a_name) {
		free((char*)_currentMethodAnnotation.a_name);
		hashmap_destroyWithKeysAndValues(_currentMethodAnnotation.a_values);
		_currentMethodAnnotation.a_values = 0;
		_currentMethodAnnotation.a_name = 0;
	}
}

static void start_annotation(const char* pszName) {
	destroy_annotation();
	_currentMethodAnnotation.a_name = pszName;
	_currentMethodAnnotation.a_values = hashmap_create(7, 0 , 0);
}

static void parser_newForeachCursor() {
	YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex++});
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


#line 330 "parser.c"

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
  YYSYMBOL_77_2 = 77,                      /* $@2  */
  YYSYMBOL_header_delimiter = 78,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 79,         /* macro_definitions  */
  YYSYMBOL_m_o_f = 80,                     /* m_o_f  */
  YYSYMBOL_macro_or_native_function = 81,  /* macro_or_native_function  */
  YYSYMBOL_namespace = 82,                 /* namespace  */
  YYSYMBOL_prerequisites = 83,             /* prerequisites  */
  YYSYMBOL_require = 84,                   /* require  */
  YYSYMBOL_global_declarations = 85,       /* global_declarations  */
  YYSYMBOL_scope = 86,                     /* scope  */
  YYSYMBOL_constants = 87,                 /* constants  */
  YYSYMBOL_constant_list = 88,             /* constant_list  */
  YYSYMBOL_constdef = 89,                  /* constdef  */
  YYSYMBOL_90_3 = 90,                      /* $@3  */
  YYSYMBOL_91_4 = 91,                      /* $@4  */
  YYSYMBOL_constant_literal = 92,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 93,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 94,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 95,         /* macro_declaration  */
  YYSYMBOL_macro_type = 96,                /* macro_type  */
  YYSYMBOL_macrostart = 97,                /* macrostart  */
  YYSYMBOL_global_var = 98,                /* global_var  */
  YYSYMBOL_var_decl = 99,                  /* var_decl  */
  YYSYMBOL_optional_description = 100,     /* optional_description  */
  YYSYMBOL_argument_list = 101,            /* argument_list  */
  YYSYMBOL_non_empty_arguments = 102,      /* non_empty_arguments  */
  YYSYMBOL_parameter_list = 103,           /* parameter_list  */
  YYSYMBOL_non_empty_parameters = 104,     /* non_empty_parameters  */
  YYSYMBOL_argument_declaration = 105,     /* argument_declaration  */
  YYSYMBOL_annotations = 106,              /* annotations  */
  YYSYMBOL_annotation = 107,               /* annotation  */
  YYSYMBOL_108_5 = 108,                    /* $@5  */
  YYSYMBOL_annotation_value = 109,         /* annotation_value  */
  YYSYMBOL_annotation_values = 110,        /* annotation_values  */
  YYSYMBOL_native_function_definition = 111, /* native_function_definition  */
  YYSYMBOL_112_6 = 112,                    /* $@6  */
  YYSYMBOL_macro_definition = 113,         /* macro_definition  */
  YYSYMBOL_114_7 = 114,                    /* $@7  */
  YYSYMBOL_block = 115,                    /* block  */
  YYSYMBOL_var_decls = 116,                /* var_decls  */
  YYSYMBOL_statement = 117,                /* statement  */
  YYSYMBOL_assignment = 118,               /* assignment  */
  YYSYMBOL_119_8 = 119,                    /* $@8  */
  YYSYMBOL_shorthand_assignment = 120,     /* shorthand_assignment  */
  YYSYMBOL_121_9 = 121,                    /* $@9  */
  YYSYMBOL_assignment_target = 122,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 123, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 124,    /* assignment_expression  */
  YYSYMBOL_expression = 125,               /* expression  */
  YYSYMBOL_range_expression = 126,         /* range_expression  */
  YYSYMBOL_127_10 = 127,                   /* $@10  */
  YYSYMBOL_range_increment = 128,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 129,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 130,     /* increment_expression  */
  YYSYMBOL_simple_term = 131,              /* simple_term  */
  YYSYMBOL_constructor_expression = 132,   /* constructor_expression  */
  YYSYMBOL_unary_expression = 133,         /* unary_expression  */
  YYSYMBOL_binary_expression = 134,        /* binary_expression  */
  YYSYMBOL_condition = 135,                /* condition  */
  YYSYMBOL_value = 136,                    /* value  */
  YYSYMBOL_string = 137,                   /* string  */
  YYSYMBOL_break = 138,                    /* break  */
  YYSYMBOL_continue = 139,                 /* continue  */
  YYSYMBOL_return_expression = 140,        /* return_expression  */
  YYSYMBOL_optional_bterm = 141,           /* optional_bterm  */
  YYSYMBOL_opt_num = 142,                  /* opt_num  */
  YYSYMBOL_case_clauses = 143,             /* case_clauses  */
  YYSYMBOL_case_clause = 144,              /* case_clause  */
  YYSYMBOL_case_selector = 145,            /* case_selector  */
  YYSYMBOL_case_condition = 146,           /* case_condition  */
  YYSYMBOL_label = 147,                    /* label  */
  YYSYMBOL_goto_expression = 148,          /* goto_expression  */
  YYSYMBOL_switch_expression = 149,        /* switch_expression  */
  YYSYMBOL_150_11 = 150,                   /* $@11  */
  YYSYMBOL_for_loop_expression = 151,      /* for_loop_expression  */
  YYSYMBOL_152_12 = 152,                   /* $@12  */
  YYSYMBOL_for_increment = 153,            /* for_increment  */
  YYSYMBOL_for_to_clause = 154,            /* for_to_clause  */
  YYSYMBOL_155_13 = 155,                   /* $@13  */
  YYSYMBOL_156_14 = 156,                   /* $@14  */
  YYSYMBOL_157_15 = 157,                   /* $@15  */
  YYSYMBOL_opt_for_initializer = 158,      /* opt_for_initializer  */
  YYSYMBOL_159_16 = 159,                   /* $@16  */
  YYSYMBOL_for_clause = 160,               /* for_clause  */
  YYSYMBOL_in_clause = 161,                /* in_clause  */
  YYSYMBOL_162_17 = 162,                   /* $@17  */
  YYSYMBOL_while = 163,                    /* while  */
  YYSYMBOL_164_18 = 164,                   /* $@18  */
  YYSYMBOL_165_19 = 165,                   /* $@19  */
  YYSYMBOL_local_block = 166,              /* local_block  */
  YYSYMBOL_closing_brace = 167,            /* closing_brace  */
  YYSYMBOL_if_expression = 168,            /* if_expression  */
  YYSYMBOL_169_20 = 169,                   /* $@20  */
  YYSYMBOL_170_21 = 170,                   /* $@21  */
  YYSYMBOL_else_clause = 171,              /* else_clause  */
  YYSYMBOL_172_22 = 172,                   /* $@22  */
  YYSYMBOL_stmntlist = 173,                /* stmntlist  */
  YYSYMBOL_closing_paren = 174,            /* closing_paren  */
  YYSYMBOL_call_expression = 175,          /* call_expression  */
  YYSYMBOL_176_23 = 176,                   /* @23  */
  YYSYMBOL_177_24 = 177,                   /* @24  */
  YYSYMBOL_array_constructor = 178,        /* array_constructor  */
  YYSYMBOL_179_25 = 179,                   /* @25  */
  YYSYMBOL_map_constructor = 180,          /* map_constructor  */
  YYSYMBOL_181_26 = 181,                   /* @26  */
  YYSYMBOL_optional_map_associates = 182,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 183,           /* map_associates  */
  YYSYMBOL_map_associate = 184,            /* map_associate  */
  YYSYMBOL_185_27 = 185,                   /* @27  */
  YYSYMBOL_function_id_or_pointer = 186,   /* function_id_or_pointer  */
  YYSYMBOL_parameter = 187,                /* parameter  */
  YYSYMBOL_type_cast = 188,                /* type_cast  */
  YYSYMBOL_type_name = 189,                /* type_name  */
  YYSYMBOL_array_size = 190,               /* array_size  */
  YYSYMBOL_boolean_literal = 191,          /* boolean_literal  */
  YYSYMBOL_float_literal = 192,            /* float_literal  */
  YYSYMBOL_integer_literal = 193,          /* integer_literal  */
  YYSYMBOL_character_literal = 194,        /* character_literal  */
  YYSYMBOL_simple_literal = 195            /* simple_literal  */
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
#define YYLAST   810

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  122
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
       0,   287,   287,   293,   287,   297,   298,   300,   301,   303,
     306,   307,   309,   312,   317,   318,   320,   325,   326,   327,
     329,   330,   334,   337,   338,   340,   340,   340,   354,   357,
     363,   368,   374,   380,   381,   391,   394,   400,   409,   418,
     425,   427,   434,   441,   443,   448,   449,   452,   454,   456,
     457,   459,   460,   461,   465,   474,   480,   481,   483,   483,
     489,   496,   504,   505,   506,   509,   509,   544,   544,   569,
     571,   572,   574,   575,   576,   577,   578,   579,   580,   581,
     582,   583,   584,   585,   586,   587,   588,   591,   596,   600,
     600,   606,   609,   612,   612,   619,   624,   625,   626,   627,
     628,   630,   632,   633,   635,   636,   636,   638,   639,   642,
     643,   646,   650,   651,   652,   654,   658,   659,   660,   661,
     664,   665,   670,   674,   675,   676,   677,   678,   679,   680,
     681,   682,   683,   684,   685,   686,   687,   688,   689,   690,
     691,   692,   693,   694,   695,   696,   697,   698,   704,   705,
     709,   713,   718,   719,   725,   726,   737,   740,   747,   751,
     752,   754,   755,   757,   758,   760,   762,   764,   769,   772,
     775,   779,   785,   792,   791,   808,   808,   824,   825,   827,
     829,   831,   827,   838,   839,   839,   845,   848,   851,   857,
     857,   870,   872,   870,   884,   886,   888,   891,   894,   891,
     901,   905,   905,   912,   913,   915,   917,   917,   928,   928,
     942,   943,   945,   945,   955,   955,   965,   967,   970,   972,
     975,   975,   983,   993,   998,   999,  1004,  1009,  1012,  1017,
    1018,  1022,  1025,  1030,  1035,  1039,  1046,  1051,  1059,  1067,
    1075
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
  "'{'", "']'", "':'", "'}'", "$accept", "file_structure", "$@1", "$@2",
  "header_delimiter", "macro_definitions", "m_o_f",
  "macro_or_native_function", "namespace", "prerequisites", "require",
  "global_declarations", "scope", "constants", "constant_list", "constdef",
  "$@3", "$@4", "constant_literal", "variable_identifier",
  "variable_reference", "macro_declaration", "macro_type", "macrostart",
  "global_var", "var_decl", "optional_description", "argument_list",
  "non_empty_arguments", "parameter_list", "non_empty_parameters",
  "argument_declaration", "annotations", "annotation", "$@5",
  "annotation_value", "annotation_values", "native_function_definition",
  "$@6", "macro_definition", "$@7", "block", "var_decls", "statement",
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

#define YYPACT_NINF (-246)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-224)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -246,    15,    23,  -246,    26,  -246,   -42,    44,  -246,    35,
    -246,    16,   -13,  -246,  -246,    32,    91,    25,    38,  -246,
      40,  -246,    80,  -246,    46,    49,  -246,    80,  -246,  -246,
    -246,  -246,  -246,  -246,    68,  -246,  -246,    46,   110,   131,
       6,  -246,    83,  -246,    21,    72,  -246,  -246,  -246,    96,
    -246,   140,  -246,    85,  -246,   487,  -246,  -246,  -246,  -246,
    -246,  -246,  -246,   150,  -246,  -246,  -246,  -246,  -246,  -246,
     153,   140,  -246,    90,    71,    97,  -246,   166,   -21,  -246,
     122,   149,   487,   487,   487,     3,     5,  -246,   423,  -246,
      83,  -246,  -246,  -246,  -246,  -246,   515,  -246,   176,  -246,
    -246,  -246,   126,   155,  -246,  -246,  -246,   154,   132,   129,
    -246,  -246,  -246,  -246,  -246,   134,  -246,  -246,   135,  -246,
     699,   724,   146,  -246,  -246,  -246,  -246,   305,   569,   143,
     184,  -246,   487,   487,   487,   487,   487,   487,   487,   487,
     487,   487,   487,   487,   487,   487,   487,   487,   487,   487,
     487,   487,   487,   487,   487,   191,  -246,  -246,   145,  -246,
    -246,   198,   153,  -246,   104,   271,   148,   338,  -246,   487,
     156,   157,  -246,  -246,  -246,  -246,   158,   147,   159,  -246,
     649,   724,   724,   724,   -32,   -32,   674,   674,   745,   745,
      63,   699,   699,   699,   724,   745,   745,   146,   146,    63,
      63,    63,   160,  -246,   338,  -246,  -246,  -246,   164,   162,
    -246,    80,   168,    11,    18,   167,  -246,   209,   169,   222,
     222,   487,  -246,   223,   123,   178,  -246,    84,   179,  -246,
      66,   181,  -246,  -246,   183,  -246,   192,  -246,  -246,  -246,
    -246,   186,   194,  -246,   173,  -246,  -246,   454,  -246,  -246,
     184,   235,  -246,   173,   247,   104,  -246,  -246,  -246,    91,
    -246,  -246,  -246,  -246,   487,  -246,   167,  -246,   119,  -246,
     202,   204,   649,  -246,  -246,  -246,   151,  -246,  -246,  -246,
    -246,  -246,  -246,   487,   258,  -246,  -246,  -246,  -246,  -246,
    -246,  -246,  -246,  -246,   119,  -246,   487,  -246,  -246,  -246,
    -246,  -246,  -246,    80,   390,   596,   390,  -246,  -246,   210,
    -246,  -246,  -246,  -246,  -246,  -246,  -246,   487,   540,  -246,
    -246,  -246,   207,   206,   227,  -246,  -246,  -246,  -246,   390,
    -246,  -246,    83,    83,   487,    83,   208,   186,   255,  -246,
     213,  -246,  -246,   623,  -246,   487,  -246,  -246,  -246,   116,
    -246,   216,   390,    28,   214,   221,   116,    84,     5,  -246,
    -246,   265,  -246,   217,  -246,  -246,  -246,  -246,  -246,   137,
    -246,  -246,  -246,   281,  -246,   232,  -246,  -246
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,    12,     1,     0,    14,     0,    17,    13,     0,
      15,     3,     0,    21,    25,     5,     0,     0,     0,    16,
      22,    23,     0,     6,    56,   227,    40,     0,    18,    19,
      25,    30,    31,    32,     0,    58,     4,    56,    67,    56,
       0,   228,    42,    24,     0,     0,     8,    65,     9,     0,
      11,    20,    57,     0,   229,   104,    41,   234,   233,    29,
     236,   231,   232,     0,    26,   240,   238,   237,   239,    28,
      62,    20,    10,     0,     0,     0,   230,     0,   153,   154,
       0,   150,     0,     0,     0,     0,     0,   212,     0,   214,
       0,   101,   103,   120,   114,   123,   102,   112,   151,   113,
     210,   211,     0,     0,   152,   235,    27,    61,    63,     0,
      66,    37,    38,    35,    36,     0,    70,    68,     0,   206,
     116,   117,   118,    34,    33,   119,   222,     0,     0,     0,
     216,   147,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   104,     0,   155,   208,   153,   150,
     121,     0,    62,    59,    45,     0,     0,     0,    53,   104,
       0,    50,   224,    51,   146,   226,     0,     0,   217,   218,
     105,   126,   145,   144,   135,   136,   139,   138,   142,   143,
     137,   127,   128,   130,   125,   140,   141,   129,   131,   132,
     133,   134,     0,   122,     0,    60,    64,    55,     0,    46,
      47,     0,     0,     0,    34,     0,   191,     0,     0,   161,
     161,   159,    72,    70,    93,    86,    83,     0,     0,    92,
       0,     0,    79,    80,     0,    85,     0,    78,    77,    76,
      75,     0,     0,   115,     0,   225,   213,   104,   220,   215,
       0,   107,   124,     0,    43,     0,    54,    87,   188,   183,
     186,   175,   187,   171,     0,   197,     0,   172,     0,   162,
       0,     0,   160,   158,   110,   109,     0,    88,   111,    71,
      86,   204,    74,     0,     0,    84,    81,    82,   196,    69,
      73,   205,   207,    52,     0,   219,     0,   106,   209,    44,
      39,    48,   179,     0,     0,   149,     0,   192,   153,     0,
     156,   157,    99,   100,    96,    97,    98,   104,     0,    89,
     221,   108,     0,   184,     0,   194,   176,   148,   198,     0,
     173,    94,     0,     0,     0,     0,     0,     0,   200,   193,
       0,    91,    90,     0,   185,   104,   195,   201,   199,   163,
     180,     0,     0,     0,     0,     0,   163,     0,     0,   190,
     202,   234,   169,     0,   168,   167,   174,   164,   165,    93,
     178,   177,   181,     0,   166,     0,   170,   182
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -246,  -246,  -246,  -246,  -246,   266,  -246,  -246,  -246,  -246,
    -246,  -246,   291,  -246,  -246,   274,  -246,  -246,  -246,   -26,
     -83,  -246,  -246,   236,  -246,    -7,  -246,  -246,  -246,  -154,
    -246,    57,   275,  -246,  -246,  -246,   161,  -246,  -246,  -246,
    -246,   240,  -246,  -226,  -246,  -246,   -31,  -246,  -246,  -246,
     -82,   -55,  -246,  -246,  -246,  -246,   -30,   226,  -246,  -246,
     -78,    50,  -245,  -246,  -246,  -246,  -246,  -246,   111,   -23,
    -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,
    -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,
    -246,  -246,  -223,    -3,  -246,  -246,  -246,  -246,  -246,  -216,
      77,  -151,  -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,
      86,  -246,  -246,    93,  -246,   -66,  -246,  -246,  -246,   -15,
    -246,   298
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    15,    24,    36,    37,    48,     5,     7,
      10,    11,    73,    17,    20,    21,    22,   106,    64,    34,
      90,   115,    74,    75,    18,   280,   300,   208,   209,   170,
     171,   210,    38,    39,    45,   108,   109,    49,    71,    50,
      51,   226,   165,   227,   228,   333,   229,   276,   230,   317,
      56,   172,    92,   251,   297,   278,   231,    93,    94,    95,
      96,   265,    97,    98,   232,   233,   234,   273,   270,   355,
     356,   357,   363,   235,   236,   237,   340,   238,   304,   372,
     260,   322,   358,   375,   302,   335,   261,   262,   336,   239,
     266,   329,   326,   289,   240,   306,   338,   348,   352,   241,
     292,    99,   167,   204,   100,   127,   101,   130,   177,   178,
     179,   294,   102,   173,   103,    27,    41,    65,    66,    67,
      68,   104
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      91,    42,   125,   126,   120,   121,   122,   112,   131,    26,
     128,   281,   258,   244,   242,     3,   123,   105,   123,   142,
      53,   -34,   129,   309,     8,    13,     4,   149,   150,   151,
     152,   153,   154,   155,    14,    57,    58,    59,    60,     6,
      61,    62,   361,   124,   362,   124,    -7,  -223,     9,   320,
     253,    12,    35,    19,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   242,    54,   325,   259,
     325,    63,   224,   328,   113,   212,  -223,    23,    63,   213,
     263,    28,   114,    31,    32,    25,    33,   214,   211,   202,
     111,    25,    25,   325,    29,    80,   339,    30,   337,   215,
      44,   216,   217,    40,   245,    25,   207,    47,   218,  -203,
    -203,   219,   220,   221,   124,    55,   325,   154,   155,   360,
     283,   284,   308,    57,    58,    79,    60,    35,    61,    62,
      70,   368,   277,   272,   224,    86,   274,   275,    87,    13,
     222,   353,   354,   242,   223,   242,    76,  -203,   225,   159,
     274,   275,    72,    77,   105,    55,   107,   116,   158,    57,
      58,    79,    60,   242,    61,    62,    80,   118,   242,    63,
     312,   313,   314,   315,   316,   256,   305,   -95,   -95,   211,
     119,   -33,   156,   303,   157,   159,   161,   142,   163,   162,
     176,   242,   164,   166,   203,   318,   242,   151,   152,   153,
     154,   155,   175,  -223,   205,    63,    86,   243,   321,    87,
     249,   224,   267,   224,   247,    89,   250,   246,   212,   255,
     248,   252,   213,   254,   257,   264,   269,   268,    25,  -214,
     214,   224,   291,  -214,   279,   282,   224,   285,    80,   286,
     341,   342,   215,   344,   216,   217,   343,   296,   287,   288,
     290,   218,   331,   299,   219,   220,   221,   124,   310,   224,
     311,   319,   212,   334,   224,   369,   213,   323,  -189,   330,
     345,   347,    25,   349,   214,   359,   365,   373,    86,   374,
     351,    87,    80,   222,   366,   376,   215,   223,   216,   217,
     -70,   377,    16,    46,    43,   218,   168,   110,   219,   220,
     221,   124,   301,    77,    52,   117,   307,   169,    78,    57,
      58,    79,    60,   206,    61,    62,    80,   370,   371,   160,
     298,   271,    86,   367,   346,    87,   295,   222,   364,   168,
     293,   223,    69,     0,  -203,    81,    77,     0,     0,     0,
     169,    78,    57,    58,    79,    60,     0,    61,    62,    80,
      82,    83,     0,     0,    84,    85,    86,     0,     0,    87,
       0,     0,  -104,    88,     0,    89,   -49,     0,    81,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   212,     0,    82,    83,   213,     0,    84,    85,    86,
       0,    25,    87,   214,     0,  -104,    88,   -49,    89,     0,
       0,    80,     0,     0,     0,   215,     0,   216,   217,     0,
       0,     0,     0,     0,   218,     0,     0,   219,   220,   221,
     124,    77,     0,     0,    25,     0,    78,    57,    58,    79,
      60,     0,    61,    62,    80,     0,     0,     0,     0,     0,
       0,    86,     0,     0,    87,     0,   222,     0,     0,     0,
     324,     0,    77,    81,     0,     0,   169,    78,    57,    58,
      79,    60,     0,    61,    62,    80,     0,     0,    82,    83,
       0,     0,    84,    85,    86,     0,     0,    87,     0,     0,
       0,    88,     0,    89,    81,    77,     0,     0,     0,     0,
      78,    57,    58,    79,    60,     0,    61,    62,    80,    82,
      83,     0,     0,    84,    85,    86,     0,     0,    87,     0,
       0,     0,    88,     0,    89,     0,     0,    81,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   132,     0,     0,
       0,     0,    82,    83,     0,     0,    84,    85,    86,     0,
       0,    87,     0,     0,     0,    88,   133,    89,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
       0,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   133,     0,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,     0,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,     0,     0,     0,     0,
     133,   332,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,     0,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,     0,     0,   133,   174,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,     0,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,     0,     0,   133,   327,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,     0,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   350,
     133,     0,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,     0,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   133,     0,   134,   135,   136,
     137,     0,     0,   140,   141,   142,   143,   144,   145,     0,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     133,     0,   134,   135,   136,   137,     0,     0,   140,   141,
     142,     0,     0,     0,     0,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,  -224,     0,  -224,  -224,   136,
     137,     0,     0,   140,   141,   142,     0,     0,     0,     0,
    -224,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     136,   137,     0,     0,     0,     0,   142,     0,     0,     0,
       0,     0,     0,     0,   149,   150,   151,   152,   153,   154,
     155
};

static const yytype_int16 yycheck[] =
{
      55,    27,    85,    86,    82,    83,    84,    73,    90,    16,
      88,   227,     1,   167,   165,     0,    13,    14,    13,    51,
      14,    42,    88,   268,    66,     9,     3,    59,    60,    61,
      62,    63,    64,    65,    18,    14,    15,    16,    17,    13,
      19,    20,    14,    40,    16,    40,     0,    68,     4,   294,
     204,    16,     6,    66,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   227,    71,   304,    68,
     306,    60,   165,   306,    13,     1,    68,    55,    60,     5,
      72,    66,    21,    13,    14,    11,    16,    13,   164,   154,
      10,    11,    11,   329,    66,    21,   329,    67,   324,    25,
      42,    27,    28,    64,   169,    11,    12,     7,    34,    35,
      36,    37,    38,    39,    40,    42,   352,    64,    65,   352,
      64,    65,    13,    14,    15,    16,    17,     6,    19,    20,
      68,   357,   224,   221,   227,    61,    23,    24,    64,     9,
      66,    35,    36,   304,    70,   306,    71,    73,   165,    40,
      23,    24,    66,     8,    14,    42,    13,    70,    13,    14,
      15,    16,    17,   324,    19,    20,    21,    11,   329,    60,
      29,    30,    31,    32,    33,   211,   264,    64,    65,   255,
      68,    42,    16,   259,    68,    40,    42,    51,    69,    67,
      16,   352,    68,    68,    13,   283,   357,    61,    62,    63,
      64,    65,    69,    68,    16,    60,    61,    69,   296,    64,
      73,   304,    13,   306,    67,    70,    67,    71,     1,    67,
      72,    71,     5,    69,    66,    68,    14,    68,    11,    16,
      13,   324,    69,    16,    66,    66,   329,    66,    21,    66,
     332,   333,    25,   335,    27,    28,   334,    22,    66,    73,
      66,    34,   317,    16,    37,    38,    39,    40,    66,   352,
      66,    13,     1,    66,   357,   358,     5,   303,    72,    69,
      72,    26,    11,    70,    13,    69,    72,    22,    61,    72,
     345,    64,    21,    66,    73,    14,    25,    70,    27,    28,
      73,    69,    11,    37,    30,    34,     1,    71,    37,    38,
      39,    40,   255,     8,    39,    75,   266,    12,    13,    14,
      15,    16,    17,   162,    19,    20,    21,   358,   358,   103,
     253,   220,    61,   356,   337,    64,   250,    66,   353,     1,
     247,    70,    44,    -1,    73,    40,     8,    -1,    -1,    -1,
      12,    13,    14,    15,    16,    17,    -1,    19,    20,    21,
      55,    56,    -1,    -1,    59,    60,    61,    -1,    -1,    64,
      -1,    -1,    67,    68,    -1,    70,    71,    -1,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,    55,    56,     5,    -1,    59,    60,    61,
      -1,    11,    64,    13,    -1,    67,    68,    69,    70,    -1,
      -1,    21,    -1,    -1,    -1,    25,    -1,    27,    28,    -1,
      -1,    -1,    -1,    -1,    34,    -1,    -1,    37,    38,    39,
      40,     8,    -1,    -1,    11,    -1,    13,    14,    15,    16,
      17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    64,    -1,    66,    -1,    -1,    -1,
      70,    -1,     8,    40,    -1,    -1,    12,    13,    14,    15,
      16,    17,    -1,    19,    20,    21,    -1,    -1,    55,    56,
      -1,    -1,    59,    60,    61,    -1,    -1,    64,    -1,    -1,
      -1,    68,    -1,    70,    40,     8,    -1,    -1,    -1,    -1,
      13,    14,    15,    16,    17,    -1,    19,    20,    21,    55,
      56,    -1,    -1,    59,    60,    61,    -1,    -1,    64,    -1,
      -1,    -1,    68,    -1,    70,    -1,    -1,    40,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,
      -1,    -1,    55,    56,    -1,    -1,    59,    60,    61,    -1,
      -1,    64,    -1,    -1,    -1,    68,    41,    70,    43,    44,
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
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      45,    46,    -1,    -1,    -1,    -1,    51,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    60,    61,    62,    63,    64,
      65
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    75,    76,     0,     3,    82,    13,    83,    66,     4,
      84,    85,    16,     9,    18,    77,    86,    87,    98,    66,
      88,    89,    90,    55,    78,    11,    99,   189,    66,    66,
      67,    13,    14,    16,    93,     6,    79,    80,   106,   107,
      64,   190,    93,    89,    42,   108,    79,     7,    81,   111,
     113,   114,   106,    14,    71,    42,   124,    14,    15,    16,
      17,    19,    20,    60,    92,   191,   192,   193,   194,   195,
      68,   112,    66,    86,    96,    97,    71,     8,    13,    16,
      21,    40,    55,    56,    59,    60,    61,    64,    68,    70,
      94,   125,   126,   131,   132,   133,   134,   136,   137,   175,
     178,   180,   186,   188,   195,    14,    91,    13,   109,   110,
      97,    10,   189,    13,    21,    95,    70,   115,    11,    68,
     134,   134,   134,    13,    40,    94,    94,   179,   134,   189,
     181,   124,    22,    41,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    16,    68,    13,    40,
     131,    42,    67,    69,    68,   116,    68,   176,     1,    12,
     103,   104,   125,   187,    69,    69,    16,   182,   183,   184,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   125,    13,   177,    16,   110,    12,   101,   102,
     105,   189,     1,     5,    13,    25,    27,    28,    34,    37,
      38,    39,    66,    70,    94,    99,   115,   117,   118,   120,
     122,   130,   138,   139,   140,   147,   148,   149,   151,   163,
     168,   173,   175,    69,   103,   125,    71,    67,    72,    73,
      67,   127,    71,   103,    69,    67,    93,    66,     1,    68,
     154,   160,   161,    72,    68,   135,   164,    13,    68,    14,
     142,   142,   134,   141,    23,    24,   121,   124,   129,    66,
      99,   173,    66,    64,    65,    66,    66,    66,    73,   167,
      66,    69,   174,   187,   185,   184,    22,   128,   174,    16,
     100,   105,   158,   189,   152,   134,   169,   135,    13,   136,
      66,    66,    29,    30,    31,    32,    33,   123,   134,    13,
     136,   134,   155,    93,    70,   117,   166,    69,   166,   165,
      69,   125,    71,   119,    66,   159,   162,   173,   170,   166,
     150,   124,   124,   134,   124,    72,   167,    26,   171,    70,
      66,   125,   172,    35,    36,   143,   144,   145,   156,    69,
     166,    14,    16,   146,   193,    72,    73,   143,   173,    94,
     120,   130,   153,    22,    72,   157,    14,    69
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    76,    77,    75,    78,    78,    79,    79,    80,
      81,    81,    82,    82,    83,    83,    84,    85,    85,    85,
      86,    86,    87,    88,    88,    90,    91,    89,    92,    92,
      93,    93,    93,    94,    94,    95,    95,    96,    96,    97,
      98,    99,    99,   100,   100,   101,   101,   102,   102,   103,
     103,   104,   104,   104,   105,   105,   106,   106,   108,   107,
     109,   109,   110,   110,   110,   112,   111,   114,   113,   115,
     116,   116,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   118,   119,
     118,   118,   118,   121,   120,   122,   123,   123,   123,   123,
     123,   124,   125,   125,   126,   127,   126,   128,   128,   129,
     129,   130,   131,   131,   131,   132,   133,   133,   133,   133,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   135,   135,
     136,   136,   136,   136,   137,   137,   138,   139,   140,   141,
     141,   142,   142,   143,   143,   144,   145,   145,   146,   146,
     146,   147,   148,   150,   149,   152,   151,   153,   153,   155,
     156,   157,   154,   158,   159,   158,   160,   160,   160,   162,
     161,   164,   165,   163,   166,   166,   167,   169,   170,   168,
     171,   172,   171,   173,   173,   174,   176,   175,   177,   175,
     175,   175,   179,   178,   181,   180,   182,   182,   183,   183,
     185,   184,   186,   186,   187,   187,   188,   189,   189,   190,
     190,   191,   191,   192,   193,   193,   194,   195,   195,   195,
     195
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     7,     0,     1,     0,     2,     2,
       2,     1,     0,     3,     0,     2,     3,     0,     3,     3,
       0,     1,     2,     1,     3,     0,     0,     5,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     6,
       2,     3,     2,     0,     1,     0,     1,     1,     3,     0,
       1,     1,     3,     1,     2,     1,     0,     2,     0,     5,
       3,     1,     0,     1,     3,     0,     3,     0,     3,     4,
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
#line 287 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2134 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 293 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 2142 "parser.c"
    break;

  case 12: /* namespace: %empty  */
#line 309 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2150 "parser.c"
    break;

  case 13: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 312 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2159 "parser.c"
    break;

  case 16: /* require: T_REQUIRE T_STRING ';'  */
#line 320 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2168 "parser.c"
    break;

  case 20: /* scope: %empty  */
#line 329 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2174 "parser.c"
    break;

  case 21: /* scope: T_STATIC  */
#line 330 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2180 "parser.c"
    break;

  case 25: /* $@3: %empty  */
#line 340 "Parser.y"
                { _bDefiningConst = 1; }
#line 2186 "parser.c"
    break;

  case 26: /* $@4: %empty  */
#line 340 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2192 "parser.c"
    break;

  case 27: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 341 "Parser.y"
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
#line 2208 "parser.c"
    break;

  case 28: /* constant_literal: simple_literal  */
#line 354 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2216 "parser.c"
    break;

  case 29: /* constant_literal: T_STRING  */
#line 357 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2225 "parser.c"
    break;

  case 30: /* variable_identifier: T_IDENT  */
#line 363 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2235 "parser.c"
    break;

  case 31: /* variable_identifier: T_NUM  */
#line 368 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2246 "parser.c"
    break;

  case 32: /* variable_identifier: T_STRING  */
#line 374 "Parser.y"
                                   {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2255 "parser.c"
    break;

  case 33: /* variable_reference: T_VARIABLE  */
#line 380 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2261 "parser.c"
    break;

  case 34: /* variable_reference: T_IDENT  */
#line 381 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2273 "parser.c"
    break;

  case 35: /* macro_declaration: T_IDENT  */
#line 391 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2281 "parser.c"
    break;

  case 36: /* macro_declaration: T_FUNC  */
#line 394 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2291 "parser.c"
    break;

  case 37: /* macro_type: scope T_VOID  */
#line 400 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
				yyval.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(VT_NIL);
				}
			}
#line 2304 "parser.c"
    break;

  case 38: /* macro_type: scope type_name  */
#line 409 "Parser.y"
                                        {
				yyval.ident.type = yyvsp[0].ident.type;
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(yyvsp[0].ident.type);
				}
			}
#line 2317 "parser.c"
    break;

  case 39: /* macrostart: macro_type macro_declaration '(' argument_list ')' optional_description  */
#line 419 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				yyval.ident.type = yyvsp[-5].ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2327 "parser.c"
    break;

  case 41: /* var_decl: type_name variable_identifier assignment_expression  */
#line 427 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});
			}
#line 2340 "parser.c"
    break;

  case 42: /* var_decl: type_name variable_identifier  */
#line 434 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2351 "parser.c"
    break;

  case 44: /* optional_description: T_STRING  */
#line 443 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2360 "parser.c"
    break;

  case 53: /* non_empty_parameters: error  */
#line 461 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 2368 "parser.c"
    break;

  case 54: /* argument_declaration: type_name variable_identifier  */
#line 465 "Parser.y"
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
#line 2383 "parser.c"
    break;

  case 55: /* argument_declaration: T_ELLIPSIS  */
#line 474 "Parser.y"
                                       {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[0].ident.type, "...");
				}
			}
#line 2393 "parser.c"
    break;

  case 58: /* $@5: %empty  */
#line 483 "Parser.y"
                         {
				start_annotation(yyvsp[0].ident.s);
			}
#line 2401 "parser.c"
    break;

  case 59: /* annotation: T_ANNOTATION $@5 '(' annotation_values ')'  */
#line 485 "Parser.y"
                                                    {
				macro_processAnnotation(&_macroParam, &_currentMethodAnnotation);
			}
#line 2409 "parser.c"
    break;

  case 60: /* annotation_value: T_IDENT T_ASSIGN T_STRING  */
#line 489 "Parser.y"
                                            {
				if (_currentMethodAnnotation.a_values) {
					hashmap_put(_currentMethodAnnotation.a_values, (intptr_t)yyvsp[-2].ident.s, (intptr_t)yyvsp[0].ident.s);
				} else {
					free(yyvsp[-2].ident.s);
					free(yyvsp[0].ident.s);
				}
			}
#line 2422 "parser.c"
    break;

  case 61: /* annotation_value: T_IDENT  */
#line 496 "Parser.y"
                                    {
				if (_currentMethodAnnotation.a_values) {
					hashmap_put(_currentMethodAnnotation.a_values, (intptr_t)yyvsp[0].ident.s, (intptr_t)_strdup("true"));
				} else {
					free(yyvsp[0].ident.s);
				}
			}
#line 2434 "parser.c"
    break;

  case 65: /* $@6: %empty  */
#line 509 "Parser.y"
                                         {
					init_native();
				}
#line 2442 "parser.c"
    break;

  case 66: /* native_function_definition: T_NATIVE $@6 macrostart  */
#line 511 "Parser.y"
                                             {
					char* pszLib = 0;
					char* pszMacroCName = 0;
					char* pszSignature = 0;
					if (_currentMethodAnnotation.a_values && strcmp("@Method", _currentMethodAnnotation.a_name) == 0) {
						pszLib = (char*)hashmap_get(_currentMethodAnnotation.a_values, (intptr_t)"lib");
						pszMacroCName = (char*)hashmap_get(_currentMethodAnnotation.a_values, (intptr_t)"name");
						pszSignature = (char*)hashmap_get(_currentMethodAnnotation.a_values, (intptr_t)"signature");
					}
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
#line 2479 "parser.c"
    break;

  case 67: /* $@7: %empty  */
#line 544 "Parser.y"
                  { init_macroDefinition(); }
#line 2485 "parser.c"
    break;

  case 68: /* macro_definition: $@7 macrostart block  */
#line 545 "Parser.y"
                                                 {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName(yyvsp[-1].ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",yyvsp[-1].ident.s);
				bytecode_optimizeInstructions(_currentBytecodeBuffer);
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
#line 2513 "parser.c"
    break;

  case 73: /* statement: call_expression ';'  */
#line 575 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2519 "parser.c"
    break;

  case 74: /* statement: assignment ';'  */
#line 576 "Parser.y"
                                         { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2525 "parser.c"
    break;

  case 75: /* statement: if_expression  */
#line 577 "Parser.y"
                                        { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2531 "parser.c"
    break;

  case 76: /* statement: while  */
#line 578 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2537 "parser.c"
    break;

  case 77: /* statement: for_loop_expression  */
#line 579 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2543 "parser.c"
    break;

  case 78: /* statement: switch_expression  */
#line 580 "Parser.y"
                                            { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2549 "parser.c"
    break;

  case 83: /* statement: block  */
#line 585 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2555 "parser.c"
    break;

  case 84: /* statement: increment_expression ';'  */
#line 586 "Parser.y"
                                                   { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2561 "parser.c"
    break;

  case 86: /* statement: var_decl  */
#line 588 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2569 "parser.c"
    break;

  case 87: /* statement: error ';'  */
#line 591 "Parser.y"
                                    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2578 "parser.c"
    break;

  case 88: /* assignment: variable_reference assignment_expression  */
#line 596 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2586 "parser.c"
    break;

  case 89: /* $@8: %empty  */
#line 600 "Parser.y"
                                                      {
				parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2594 "parser.c"
    break;

  case 90: /* assignment: assignment_target '.' T_IDENT $@8 assignment_expression  */
#line 602 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2602 "parser.c"
    break;

  case 91: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 606 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2610 "parser.c"
    break;

  case 93: /* $@9: %empty  */
#line 612 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2618 "parser.c"
    break;

  case 94: /* shorthand_assignment: variable_reference $@9 shorthand_assignment_operator expression  */
#line 614 "Parser.y"
                                                                   {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2627 "parser.c"
    break;

  case 95: /* assignment_target: variable_reference  */
#line 619 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2635 "parser.c"
    break;

  case 96: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 624 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2641 "parser.c"
    break;

  case 97: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 625 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2647 "parser.c"
    break;

  case 98: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 626 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2653 "parser.c"
    break;

  case 99: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 627 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2659 "parser.c"
    break;

  case 100: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 628 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2665 "parser.c"
    break;

  case 101: /* assignment_expression: T_ASSIGN expression  */
#line 630 "Parser.y"
                                                { yyval.v = yyvsp[0].v;	}
#line 2671 "parser.c"
    break;

  case 102: /* expression: binary_expression  */
#line 632 "Parser.y"
                              { yyval.v = yyvsp[0].v; }
#line 2677 "parser.c"
    break;

  case 105: /* $@10: %empty  */
#line 636 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2683 "parser.c"
    break;

  case 108: /* range_increment: T_DOTDOT binary_expression  */
#line 639 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2689 "parser.c"
    break;

  case 109: /* minusminus_plusplus: T_MINUSMINUS  */
#line 642 "Parser.y"
                                     { yyval.num = -1; }
#line 2695 "parser.c"
    break;

  case 110: /* minusminus_plusplus: T_PLUSPLUS  */
#line 643 "Parser.y"
                                     { yyval.num = 1; }
#line 2701 "parser.c"
    break;

  case 111: /* increment_expression: variable_reference minusminus_plusplus  */
#line 646 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2709 "parser.c"
    break;

  case 112: /* simple_term: value  */
#line 650 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2715 "parser.c"
    break;

  case 115: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 654 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2723 "parser.c"
    break;

  case 116: /* unary_expression: '!' binary_expression  */
#line 658 "Parser.y"
                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2729 "parser.c"
    break;

  case 117: /* unary_expression: '~' binary_expression  */
#line 659 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2735 "parser.c"
    break;

  case 118: /* unary_expression: '+' binary_expression  */
#line 660 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2741 "parser.c"
    break;

  case 119: /* unary_expression: '-' variable_reference  */
#line 661 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2747 "parser.c"
    break;

  case 120: /* binary_expression: simple_term  */
#line 664 "Parser.y"
                                    { yyval.ident.type = yyvsp[0].ident.type;}
#line 2753 "parser.c"
    break;

  case 121: /* binary_expression: type_cast simple_term  */
#line 665 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2762 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression '.' T_IDENT  */
#line 670 "Parser.y"
                                                      {
				yyval.ident.type = parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2771 "parser.c"
    break;

  case 124: /* binary_expression: binary_expression '[' expression ']'  */
#line 675 "Parser.y"
                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2777 "parser.c"
    break;

  case 125: /* binary_expression: binary_expression '~' binary_expression  */
#line 676 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2783 "parser.c"
    break;

  case 126: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 677 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2789 "parser.c"
    break;

  case 127: /* binary_expression: binary_expression '&' binary_expression  */
#line 678 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2795 "parser.c"
    break;

  case 128: /* binary_expression: binary_expression '|' binary_expression  */
#line 679 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2801 "parser.c"
    break;

  case 129: /* binary_expression: binary_expression '+' binary_expression  */
#line 680 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2807 "parser.c"
    break;

  case 130: /* binary_expression: binary_expression '^' binary_expression  */
#line 681 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2813 "parser.c"
    break;

  case 131: /* binary_expression: binary_expression '-' binary_expression  */
#line 682 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2819 "parser.c"
    break;

  case 132: /* binary_expression: binary_expression '*' binary_expression  */
#line 683 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2825 "parser.c"
    break;

  case 133: /* binary_expression: binary_expression '/' binary_expression  */
#line 684 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2831 "parser.c"
    break;

  case 134: /* binary_expression: binary_expression '%' binary_expression  */
#line 685 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2837 "parser.c"
    break;

  case 135: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 686 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2843 "parser.c"
    break;

  case 136: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 687 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2849 "parser.c"
    break;

  case 137: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 688 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2855 "parser.c"
    break;

  case 138: /* binary_expression: binary_expression T_AND binary_expression  */
#line 689 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2861 "parser.c"
    break;

  case 139: /* binary_expression: binary_expression T_OR binary_expression  */
#line 690 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2867 "parser.c"
    break;

  case 140: /* binary_expression: binary_expression '<' binary_expression  */
#line 691 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2873 "parser.c"
    break;

  case 141: /* binary_expression: binary_expression '>' binary_expression  */
#line 692 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2879 "parser.c"
    break;

  case 142: /* binary_expression: binary_expression T_LE binary_expression  */
#line 693 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2885 "parser.c"
    break;

  case 143: /* binary_expression: binary_expression T_GE binary_expression  */
#line 694 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2891 "parser.c"
    break;

  case 144: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 695 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2897 "parser.c"
    break;

  case 145: /* binary_expression: binary_expression T_NE binary_expression  */
#line 696 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2903 "parser.c"
    break;

  case 146: /* binary_expression: '(' binary_expression ')'  */
#line 697 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2909 "parser.c"
    break;

  case 147: /* binary_expression: variable_reference assignment_expression  */
#line 698 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2917 "parser.c"
    break;

  case 149: /* condition: '(' binary_expression  */
#line 705 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2925 "parser.c"
    break;

  case 150: /* value: T_VARIABLE  */
#line 709 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 2934 "parser.c"
    break;

  case 151: /* value: string  */
#line 713 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2944 "parser.c"
    break;

  case 152: /* value: simple_literal  */
#line 718 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 2950 "parser.c"
    break;

  case 153: /* value: T_IDENT  */
#line 719 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 2960 "parser.c"
    break;

  case 154: /* string: T_STRING  */
#line 725 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2966 "parser.c"
    break;

  case 155: /* string: string T_STRING  */
#line 726 "Parser.y"
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
#line 2981 "parser.c"
    break;

  case 156: /* break: T_BREAK opt_num ';'  */
#line 738 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2987 "parser.c"
    break;

  case 157: /* continue: T_CONTINUE opt_num ';'  */
#line 740 "Parser.y"
                                       {
				// in the case this is a continue in a for(int i;b;i++) loop, generate the i++ code before continuing.
				parser_flushBuffer(_breaklevel-(int)yyvsp[-1].num+1);
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); 
			}
#line 2997 "parser.c"
    break;

  case 158: /* return_expression: T_RETURN optional_bterm  */
#line 747 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 3005 "parser.c"
    break;

  case 161: /* opt_num: %empty  */
#line 754 "Parser.y"
                {	yyval.num = 1;	}
#line 3011 "parser.c"
    break;

  case 162: /* opt_num: T_NUM  */
#line 755 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 3017 "parser.c"
    break;

  case 167: /* case_selector: T_DEFAULT ':'  */
#line 764 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 3025 "parser.c"
    break;

  case 168: /* case_condition: integer_literal  */
#line 769 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 3033 "parser.c"
    break;

  case 169: /* case_condition: T_STRING  */
#line 772 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3041 "parser.c"
    break;

  case 170: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 775 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3049 "parser.c"
    break;

  case 171: /* label: T_IDENT ':'  */
#line 779 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3058 "parser.c"
    break;

  case 172: /* goto_expression: T_GOTO T_IDENT  */
#line 785 "Parser.y"
                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3067 "parser.c"
    break;

  case 173: /* $@11: %empty  */
#line 792 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3077 "parser.c"
    break;

  case 174: /* switch_expression: T_SWITCH '(' value ')' $@11 '{' case_clauses '}'  */
#line 798 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3090 "parser.c"
    break;

  case 175: /* $@12: %empty  */
#line 808 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3099 "parser.c"
    break;

  case 176: /* for_loop_expression: T_FOR for_clause $@12 local_block  */
#line 811 "Parser.y"
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
#line 3116 "parser.c"
    break;

  case 179: /* $@13: %empty  */
#line 827 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3124 "parser.c"
    break;

  case 180: /* $@14: %empty  */
#line 829 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3132 "parser.c"
    break;

  case 181: /* $@15: %empty  */
#line 831 "Parser.y"
                                        {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			}
#line 3141 "parser.c"
    break;

  case 182: /* for_to_clause: '(' opt_for_initializer $@13 ';' binary_expression ';' $@14 for_increment $@15 ')'  */
#line 834 "Parser.y"
                              {
				yyval.ident.s = yyvsp[-8].ident.s;
			}
#line 3149 "parser.c"
    break;

  case 184: /* $@16: %empty  */
#line 839 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3157 "parser.c"
    break;

  case 185: /* opt_for_initializer: type_name variable_identifier $@16 assignment_expression  */
#line 841 "Parser.y"
                                                {
				yyval.ident.s = yyvsp[-2].ident.s;
			}
#line 3165 "parser.c"
    break;

  case 186: /* for_clause: for_to_clause  */
#line 845 "Parser.y"
                              {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3173 "parser.c"
    break;

  case 187: /* for_clause: in_clause  */
#line 848 "Parser.y"
                                    {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3181 "parser.c"
    break;

  case 188: /* for_clause: error  */
#line 851 "Parser.y"
                                {
				yyval.ident.s = 0;
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3191 "parser.c"
    break;

  case 189: /* $@17: %empty  */
#line 857 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				parser_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3202 "parser.c"
    break;

  case 190: /* in_clause: '(' type_name variable_identifier $@17 ':' expression ')'  */
#line 862 "Parser.y"
                                             {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
				yyval.ident.s = yyvsp[-4].ident.s;
			}
#line 3214 "parser.c"
    break;

  case 191: /* $@18: %empty  */
#line 870 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3222 "parser.c"
    break;

  case 192: /* $@19: %empty  */
#line 872 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3231 "parser.c"
    break;

  case 193: /* while: T_WHILE $@18 condition $@19 local_block  */
#line 875 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3243 "parser.c"
    break;

  case 197: /* $@20: %empty  */
#line 891 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3252 "parser.c"
    break;

  case 198: /* $@21: %empty  */
#line 894 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3260 "parser.c"
    break;

  case 199: /* if_expression: T_IF condition $@20 local_block $@21 else_clause  */
#line 896 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3269 "parser.c"
    break;

  case 200: /* else_clause: %empty  */
#line 901 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3278 "parser.c"
    break;

  case 201: /* $@22: %empty  */
#line 905 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3288 "parser.c"
    break;

  case 206: /* @23: %empty  */
#line 917 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3300 "parser.c"
    break;

  case 207: /* call_expression: T_FUNC '(' @23 parameter_list closing_paren  */
#line 923 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3310 "parser.c"
    break;

  case 208: /* @24: %empty  */
#line 928 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3321 "parser.c"
    break;

  case 209: /* call_expression: function_id_or_pointer '(' @24 parameter_list closing_paren  */
#line 933 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3335 "parser.c"
    break;

  case 212: /* @25: %empty  */
#line 945 "Parser.y"
                       {
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = &_functionTable[FUNC_CreateArray];
			}
#line 3345 "parser.c"
    break;

  case 213: /* array_constructor: '[' @25 parameter_list ']'  */
#line 949 "Parser.y"
                                             {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
					C_0FUNC,(GENERIC_DATA){FUNC_CreateArray}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
			}
#line 3355 "parser.c"
    break;

  case 214: /* @26: %empty  */
#line 955 "Parser.y"
                     {
		_currentFunctionCallParamIndexP++;
		*_currentFunctionCallParamIndexP = 0;
		yyval.funcp = &_functionTable[FUNC_CreateMap];
	}
#line 3365 "parser.c"
    break;

  case 215: /* map_constructor: '{' @26 optional_map_associates '}'  */
#line 959 "Parser.y"
                                      {
		_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
			C_0FUNC,(GENERIC_DATA){FUNC_CreateMap}, *_currentFunctionCallParamIndexP);
		_currentFunctionCallParamIndexP--;
	}
#line 3375 "parser.c"
    break;

  case 220: /* @27: %empty  */
#line 975 "Parser.y"
                                     { 
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[-1].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[-1].ident.s);
				(*_currentFunctionCallParamIndexP) += 2;
			}
#line 3386 "parser.c"
    break;

  case 222: /* function_id_or_pointer: '*' variable_reference  */
#line 983 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3400 "parser.c"
    break;

  case 223: /* function_id_or_pointer: T_IDENT  */
#line 993 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3409 "parser.c"
    break;

  case 224: /* parameter: expression  */
#line 998 "Parser.y"
                           { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3415 "parser.c"
    break;

  case 225: /* parameter: T_ELLIPSIS expression  */
#line 999 "Parser.y"
                                                { 
				yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; 
				YY_EMIT(C_SPREAD, (GENERIC_DATA){1});
			}
#line 3424 "parser.c"
    break;

  case 226: /* type_cast: '(' type_name ')'  */
#line 1004 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3433 "parser.c"
    break;

  case 227: /* type_name: T_TYPE_IDENTIFIER  */
#line 1009 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3441 "parser.c"
    break;

  case 228: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 1012 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3450 "parser.c"
    break;

  case 230: /* array_size: '[' T_NUM ']'  */
#line 1018 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3458 "parser.c"
    break;

  case 231: /* boolean_literal: T_TRUE  */
#line 1022 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3467 "parser.c"
    break;

  case 232: /* boolean_literal: T_FALSE  */
#line 1025 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3476 "parser.c"
    break;

  case 233: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 1030 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3485 "parser.c"
    break;

  case 234: /* integer_literal: T_NUM  */
#line 1035 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3494 "parser.c"
    break;

  case 235: /* integer_literal: '-' T_NUM  */
#line 1039 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3504 "parser.c"
    break;

  case 236: /* character_literal: T_CHARACTER  */
#line 1046 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3513 "parser.c"
    break;

  case 237: /* simple_literal: integer_literal  */
#line 1051 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3526 "parser.c"
    break;

  case 238: /* simple_literal: float_literal  */
#line 1059 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3539 "parser.c"
    break;

  case 239: /* simple_literal: character_literal  */
#line 1067 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3552 "parser.c"
    break;

  case 240: /* simple_literal: boolean_literal  */
#line 1075 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3565 "parser.c"
    break;


#line 3569 "parser.c"

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

#line 1083 "Parser.y"


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
	parser_switchToBuffer(0);
	_currentBytecodeBuffer->bb_current = _currentBytecodeBuffer->bb_start;
}

static void destroy_bytecodeBuffer() {
	for (int i = 0; i < DIM(_buffers); i++) {
		if (_buffers[i].bb_start) {
			free(_buffers[i].bb_start);
			_buffers[i].bb_start = 0;
			_buffers[i].bb_end = 0;
		}
	}
}

static void init_header() {
	init_bytecodeBuffer();
	_bInHeader = 1;
}

static void init_native() {
	init_bytecodeBuffer();
	_bInHeader = 1;
	_bInNativeDefinition = 1;
}

static void finalize_native() {
	destroy_bytecodeBuffer();
	_bInHeader = 0;
	_bInNativeDefinition = 0;
	destroy_annotation();
	stringbuf_destroy(_nativeMethodArgs);
	stringbuf_destroy(_nativeMethodSignature);
	_nativeMethodArgs = 0;
	_nativeMethodSignature = 0;
}

static void finalize_header() {
	macro_defineNamespaceInitializer(_currentNamespaceIndex, _currentBytecodeBuffer->bb_start, _currentBytecodeBuffer->bb_current - _currentBytecodeBuffer->bb_start);
	destroy_bytecodeBuffer();
	_bInHeader = 0;
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
	init_bytecodeBuffer();
}

/*---------------------------------*/
/* finalize_macroDefinition()					*/
/*---------------------------------*/
static void finalize_macroDefinition(void) {
	destroy_annotation();
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

