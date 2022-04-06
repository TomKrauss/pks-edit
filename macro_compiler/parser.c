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
#line 44 "Parser.y"


#include <windows.h>
#include <stdio.h>
#include <stddef.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "stringutil.h"
#include "funcdef.h"
#include "symbols.h"
#include "arraylist.h"
#include "hashmap.h"

# define	YYPTRDIFF_T ptrdiff_t
# define	YYPTRDIFF_MAXIMUM SIZE_MAX

# define 	YYSTYPE _YYSTYPE

# define 	MAX_BREAK_LEVEL	6
# define	REC_SPACE		32000

static BYTECODE_BUFFER  _buffers[MAX_BREAK_LEVEL];
static BYTECODE_BUFFER* _currentBytecodeBuffer;
static void		*_currentNativeMethodCalled;
static int		_nparam,
				_breaklevel,
				_iflevel;
static      int  _currentFunctionCallParamIndices[32];
static      int  *_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
static 		void init_header(void);
static 		void finalize_header(void);
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
static HASHMAP*		_currentNativeMethodAnnotation;
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


#line 311 "parser.c"

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
  YYSYMBOL_T_ASSOC_ARROW = 41,             /* T_ASSOC_ARROW  */
  YYSYMBOL_T_NMATCH = 42,                  /* T_NMATCH  */
  YYSYMBOL_T_ASSIGN = 43,                  /* T_ASSIGN  */
  YYSYMBOL_T_NE = 44,                      /* T_NE  */
  YYSYMBOL_T_EQ = 45,                      /* T_EQ  */
  YYSYMBOL_T_SHIFT_LEFT = 46,              /* T_SHIFT_LEFT  */
  YYSYMBOL_T_SHIFT_RIGHT = 47,             /* T_SHIFT_RIGHT  */
  YYSYMBOL_T_OR = 48,                      /* T_OR  */
  YYSYMBOL_T_AND = 49,                     /* T_AND  */
  YYSYMBOL_T_LE = 50,                      /* T_LE  */
  YYSYMBOL_T_GE = 51,                      /* T_GE  */
  YYSYMBOL_T_POWER_TO = 52,                /* T_POWER_TO  */
  YYSYMBOL_53_ = 53,                       /* '&'  */
  YYSYMBOL_54_ = 54,                       /* '|'  */
  YYSYMBOL_55_ = 55,                       /* '^'  */
  YYSYMBOL_56_ = 56,                       /* '~'  */
  YYSYMBOL_57_ = 57,                       /* '<'  */
  YYSYMBOL_58_ = 58,                       /* '>'  */
  YYSYMBOL_59_ = 59,                       /* '+'  */
  YYSYMBOL_60_ = 60,                       /* '-'  */
  YYSYMBOL_61_ = 61,                       /* '*'  */
  YYSYMBOL_62_ = 62,                       /* '/'  */
  YYSYMBOL_63_ = 63,                       /* '%'  */
  YYSYMBOL_64_ = 64,                       /* '['  */
  YYSYMBOL_65_ = 65,                       /* '!'  */
  YYSYMBOL_66_ = 66,                       /* ';'  */
  YYSYMBOL_67_ = 67,                       /* ','  */
  YYSYMBOL_68_ = 68,                       /* '('  */
  YYSYMBOL_69_ = 69,                       /* ')'  */
  YYSYMBOL_70_ = 70,                       /* '{'  */
  YYSYMBOL_71_ = 71,                       /* '.'  */
  YYSYMBOL_72_ = 72,                       /* ']'  */
  YYSYMBOL_73_ = 73,                       /* ':'  */
  YYSYMBOL_74_ = 74,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 75,                  /* $accept  */
  YYSYMBOL_file_structure = 76,            /* file_structure  */
  YYSYMBOL_77_1 = 77,                      /* $@1  */
  YYSYMBOL_78_2 = 78,                      /* $@2  */
  YYSYMBOL_header_delimiter = 79,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 80,         /* macro_definitions  */
  YYSYMBOL_namespace = 81,                 /* namespace  */
  YYSYMBOL_prerequisites = 82,             /* prerequisites  */
  YYSYMBOL_require = 83,                   /* require  */
  YYSYMBOL_global_declarations = 84,       /* global_declarations  */
  YYSYMBOL_scope = 85,                     /* scope  */
  YYSYMBOL_constants = 86,                 /* constants  */
  YYSYMBOL_constant_list = 87,             /* constant_list  */
  YYSYMBOL_constdef = 88,                  /* constdef  */
  YYSYMBOL_89_3 = 89,                      /* $@3  */
  YYSYMBOL_90_4 = 90,                      /* $@4  */
  YYSYMBOL_constant_literal = 91,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 92,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 93,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 94,         /* macro_declaration  */
  YYSYMBOL_macro_type = 95,                /* macro_type  */
  YYSYMBOL_macrostart = 96,                /* macrostart  */
  YYSYMBOL_global_var = 97,                /* global_var  */
  YYSYMBOL_var_decl = 98,                  /* var_decl  */
  YYSYMBOL_optional_description = 99,      /* optional_description  */
  YYSYMBOL_parameter_list = 100,           /* parameter_list  */
  YYSYMBOL_non_empty_pardecl = 101,        /* non_empty_pardecl  */
  YYSYMBOL_parameter_declaration = 102,    /* parameter_declaration  */
  YYSYMBOL_annotations = 103,              /* annotations  */
  YYSYMBOL_annotation = 104,               /* annotation  */
  YYSYMBOL_105_5 = 105,                    /* $@5  */
  YYSYMBOL_annotation_value = 106,         /* annotation_value  */
  YYSYMBOL_annotation_values = 107,        /* annotation_values  */
  YYSYMBOL_native_macro_definition = 108,  /* native_macro_definition  */
  YYSYMBOL_109_6 = 109,                    /* $@6  */
  YYSYMBOL_macro_definition = 110,         /* macro_definition  */
  YYSYMBOL_111_7 = 111,                    /* $@7  */
  YYSYMBOL_block = 112,                    /* block  */
  YYSYMBOL_var_decls = 113,                /* var_decls  */
  YYSYMBOL_statement = 114,                /* statement  */
  YYSYMBOL_assignment = 115,               /* assignment  */
  YYSYMBOL_116_8 = 116,                    /* $@8  */
  YYSYMBOL_shorthand_assignment = 117,     /* shorthand_assignment  */
  YYSYMBOL_118_9 = 118,                    /* $@9  */
  YYSYMBOL_assignment_target = 119,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 120, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 121,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 122,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 123, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 124,         /* range_expression  */
  YYSYMBOL_125_10 = 125,                   /* $@10  */
  YYSYMBOL_range_increment = 126,          /* range_increment  */
  YYSYMBOL_minusminus_plusplus = 127,      /* minusminus_plusplus  */
  YYSYMBOL_increment_expression = 128,     /* increment_expression  */
  YYSYMBOL_s_bterm = 129,                  /* s_bterm  */
  YYSYMBOL_constructor_expression = 130,   /* constructor_expression  */
  YYSYMBOL_binary_expression = 131,        /* binary_expression  */
  YYSYMBOL_condition = 132,                /* condition  */
  YYSYMBOL_value = 133,                    /* value  */
  YYSYMBOL_string = 134,                   /* string  */
  YYSYMBOL_break = 135,                    /* break  */
  YYSYMBOL_continue = 136,                 /* continue  */
  YYSYMBOL_return_expression = 137,        /* return_expression  */
  YYSYMBOL_optional_bterm = 138,           /* optional_bterm  */
  YYSYMBOL_opt_num = 139,                  /* opt_num  */
  YYSYMBOL_case_clauses = 140,             /* case_clauses  */
  YYSYMBOL_case_clause = 141,              /* case_clause  */
  YYSYMBOL_case_selector = 142,            /* case_selector  */
  YYSYMBOL_case_condition = 143,           /* case_condition  */
  YYSYMBOL_label = 144,                    /* label  */
  YYSYMBOL_goto_expression = 145,          /* goto_expression  */
  YYSYMBOL_switch_expression = 146,        /* switch_expression  */
  YYSYMBOL_147_11 = 147,                   /* $@11  */
  YYSYMBOL_for_loop_expression = 148,      /* for_loop_expression  */
  YYSYMBOL_149_12 = 149,                   /* $@12  */
  YYSYMBOL_for_increment = 150,            /* for_increment  */
  YYSYMBOL_for_to_clause = 151,            /* for_to_clause  */
  YYSYMBOL_152_13 = 152,                   /* $@13  */
  YYSYMBOL_153_14 = 153,                   /* $@14  */
  YYSYMBOL_154_15 = 154,                   /* $@15  */
  YYSYMBOL_opt_for_initializer = 155,      /* opt_for_initializer  */
  YYSYMBOL_156_16 = 156,                   /* $@16  */
  YYSYMBOL_for_clause = 157,               /* for_clause  */
  YYSYMBOL_in_clause = 158,                /* in_clause  */
  YYSYMBOL_159_17 = 159,                   /* $@17  */
  YYSYMBOL_while = 160,                    /* while  */
  YYSYMBOL_161_18 = 161,                   /* $@18  */
  YYSYMBOL_162_19 = 162,                   /* $@19  */
  YYSYMBOL_local_block = 163,              /* local_block  */
  YYSYMBOL_closing_brace = 164,            /* closing_brace  */
  YYSYMBOL_if_expression = 165,            /* if_expression  */
  YYSYMBOL_166_20 = 166,                   /* $@20  */
  YYSYMBOL_167_21 = 167,                   /* $@21  */
  YYSYMBOL_else_clause = 168,              /* else_clause  */
  YYSYMBOL_169_22 = 169,                   /* $@22  */
  YYSYMBOL_stmntlist = 170,                /* stmntlist  */
  YYSYMBOL_closing_paren = 171,            /* closing_paren  */
  YYSYMBOL_call_expression = 172,          /* call_expression  */
  YYSYMBOL_173_23 = 173,                   /* @23  */
  YYSYMBOL_174_24 = 174,                   /* @24  */
  YYSYMBOL_array_constructor = 175,        /* array_constructor  */
  YYSYMBOL_176_25 = 176,                   /* @25  */
  YYSYMBOL_map_constructor = 177,          /* map_constructor  */
  YYSYMBOL_178_26 = 178,                   /* @26  */
  YYSYMBOL_optional_map_associates = 179,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 180,           /* map_associates  */
  YYSYMBOL_map_associate = 181,            /* map_associate  */
  YYSYMBOL_182_27 = 182,                   /* @27  */
  YYSYMBOL_function_id_or_pointer = 183,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 184,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 185,                /* parameter  */
  YYSYMBOL_type_cast = 186,                /* type_cast  */
  YYSYMBOL_type_name = 187,                /* type_name  */
  YYSYMBOL_array_size = 188,               /* array_size  */
  YYSYMBOL_boolean_literal = 189,          /* boolean_literal  */
  YYSYMBOL_float_literal = 190,            /* float_literal  */
  YYSYMBOL_integer_literal = 191,          /* integer_literal  */
  YYSYMBOL_character_literal = 192,        /* character_literal  */
  YYSYMBOL_simple_literal = 193            /* simple_literal  */
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
#define YYLAST   868

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  119
/* YYNRULES -- Number of rules.  */
#define YYNRULES  237
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  378

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   307


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
       2,     2,     2,    65,     2,     2,     2,    63,    53,     2,
      68,    69,    61,    59,    67,    60,    71,    62,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    73,    66,
      57,     2,    58,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,     2,    72,    55,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    70,    54,    74,    56,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   269,   269,   275,   269,   279,   280,   282,   283,   285,
     288,   293,   294,   296,   301,   302,   303,   304,   306,   307,
     311,   314,   315,   317,   317,   317,   331,   334,   340,   345,
     351,   357,   358,   368,   371,   377,   386,   395,   402,   404,
     411,   418,   420,   425,   426,   429,   431,   434,   443,   449,
     450,   452,   452,   459,   468,   469,   470,   472,   472,   511,
     511,   537,   539,   540,   542,   543,   544,   545,   546,   547,
     548,   549,   550,   551,   552,   553,   554,   555,   556,   559,
     564,   568,   568,   574,   577,   580,   580,   587,   592,   593,
     594,   595,   596,   598,   600,   601,   604,   605,   607,   608,
     608,   610,   611,   614,   615,   618,   622,   623,   624,   626,
     631,   632,   637,   641,   642,   643,   644,   645,   646,   647,
     648,   649,   650,   651,   652,   653,   654,   655,   656,   657,
     658,   659,   660,   661,   662,   663,   664,   665,   666,   667,
     668,   674,   675,   679,   683,   688,   689,   695,   696,   707,
     710,   717,   721,   722,   724,   725,   727,   728,   730,   732,
     734,   739,   742,   745,   749,   755,   762,   761,   778,   778,
     794,   795,   797,   799,   801,   797,   808,   809,   809,   815,
     818,   821,   827,   827,   840,   842,   840,   853,   853,   855,
     858,   861,   858,   868,   872,   872,   879,   880,   882,   884,
     884,   895,   895,   909,   910,   912,   912,   924,   924,   936,
     938,   941,   943,   946,   946,   954,   964,   969,   971,   972,
     973,   977,   978,   983,   988,   991,   996,   997,  1001,  1004,
    1009,  1014,  1018,  1025,  1030,  1038,  1046,  1054
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
  "T_CONTINUE", "T_RETURN", "T_VARIABLE", "T_ASSOC_ARROW", "T_NMATCH",
  "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR",
  "T_AND", "T_LE", "T_GE", "T_POWER_TO", "'&'", "'|'", "'^'", "'~'", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "'!'", "';'", "','",
  "'('", "')'", "'{'", "'.'", "']'", "':'", "'}'", "$accept",
  "file_structure", "$@1", "$@2", "header_delimiter", "macro_definitions",
  "namespace", "prerequisites", "require", "global_declarations", "scope",
  "constants", "constant_list", "constdef", "$@3", "$@4",
  "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "global_var",
  "var_decl", "optional_description", "parameter_list",
  "non_empty_pardecl", "parameter_declaration", "annotations",
  "annotation", "$@5", "annotation_value", "annotation_values",
  "native_macro_definition", "$@6", "macro_definition", "$@7", "block",
  "var_decls", "statement", "assignment", "$@8", "shorthand_assignment",
  "$@9", "assignment_target", "shorthand_assignment_operator",
  "assignment_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@10",
  "range_increment", "minusminus_plusplus", "increment_expression",
  "s_bterm", "constructor_expression", "binary_expression", "condition",
  "value", "string", "break", "continue", "return_expression",
  "optional_bterm", "opt_num", "case_clauses", "case_clause",
  "case_selector", "case_condition", "label", "goto_expression",
  "switch_expression", "$@11", "for_loop_expression", "$@12",
  "for_increment", "for_to_clause", "$@13", "$@14", "$@15",
  "opt_for_initializer", "$@16", "for_clause", "in_clause", "$@17",
  "while", "$@18", "$@19", "local_block", "closing_brace", "if_expression",
  "$@20", "$@21", "else_clause", "$@22", "stmntlist", "closing_paren",
  "call_expression", "@23", "@24", "array_constructor", "@25",
  "map_constructor", "@26", "optional_map_associates", "map_associates",
  "map_associate", "@27", "function_id_or_pointer", "nonempty_parameters",
  "parameter", "type_cast", "type_name", "array_size", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-251)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-217)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -251,    42,    41,  -251,    33,  -251,    16,    80,  -251,    72,
    -251,   124,    26,  -251,  -251,    38,   118,    39,    75,    77,
     138,  -251,    81,  -251,    98,  -251,   146,    83,  -251,    98,
    -251,  -251,  -251,  -251,   143,   138,  -251,  -251,  -251,  -251,
     108,  -251,   146,   144,     2,  -251,   112,    89,   144,  -251,
    -251,    20,  -251,    19,     7,    86,    87,  -251,   516,  -251,
     147,  -251,  -251,  -251,  -251,  -251,  -251,  -251,   149,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,    96,
    -251,  -251,  -251,   154,    40,  -251,    99,   123,   516,   516,
       5,     8,  -251,   516,   452,  -251,   112,  -251,  -251,  -251,
    -251,   543,  -251,   153,  -251,  -251,  -251,   102,   391,  -251,
     128,   106,   107,  -251,  -251,   358,   290,   109,  -251,   777,
     -38,  -251,  -251,  -251,  -251,   325,   686,   602,   120,   159,
    -251,   516,   516,   516,   516,   516,   516,   516,   516,   516,
     516,   516,   516,   516,   516,   516,   516,   516,   516,   516,
     516,   516,   516,   516,   177,  -251,  -251,   125,  -251,  -251,
     176,   147,  -251,  -251,   516,   131,   134,  -251,  -251,   136,
    -251,    98,   141,    10,    13,   145,  -251,   189,   150,   190,
     190,   516,  -251,   193,    97,   148,  -251,    88,   155,  -251,
     -49,   157,  -251,  -251,   158,  -251,   160,  -251,  -251,  -251,
    -251,   156,   162,   151,   358,   140,  -251,  -251,   174,   163,
     152,  -251,   686,   777,   732,   732,   135,   135,   709,   709,
     797,   797,   -33,   755,   755,   755,   777,   797,   797,   -38,
     -38,   -33,   -33,   -33,   167,  -251,   543,  -251,   358,  -251,
    -251,  -251,   213,    79,   483,  -251,  -251,  -251,   118,  -251,
    -251,  -251,  -251,   516,  -251,   145,  -251,   165,  -251,   166,
     168,   686,  -251,  -251,  -251,   105,  -251,  -251,  -251,  -251,
    -251,  -251,   516,   218,  -251,  -251,  -251,  -251,  -251,  -251,
    -251,   171,  -251,  -251,  -251,   159,   220,  -251,   171,  -251,
    -251,  -251,  -251,   516,  -251,  -251,    98,   419,   630,   419,
    -251,  -251,   180,  -251,  -251,  -251,  -251,  -251,  -251,  -251,
     516,   571,  -251,  -251,  -251,   165,  -251,   516,  -251,  -251,
     179,   181,   246,  -251,  -251,  -251,  -251,   419,  -251,  -251,
     112,   112,  -251,   184,   516,   112,   183,   156,   224,  -251,
     188,  -251,  -251,   658,  -251,   516,  -251,  -251,  -251,    60,
    -251,   191,   419,    27,   192,   187,    60,    88,     8,  -251,
    -251,   242,  -251,   199,  -251,  -251,  -251,  -251,  -251,    94,
    -251,  -251,  -251,   252,  -251,   206,  -251,  -251
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     9,     1,     0,    11,     0,    14,    10,     0,
      12,     3,     0,    19,    23,     5,     0,     0,     0,     0,
      49,    13,    20,    21,     0,     6,    59,   224,    38,     0,
      15,    16,    17,    51,     0,    49,    23,    28,    29,    30,
       0,     4,    59,    18,     0,   225,    40,     0,    18,    50,
      22,     0,     8,     0,     0,     0,     0,   226,    98,    39,
      54,    58,   231,   230,    27,   233,   228,   229,     0,    24,
     237,   235,   234,   236,    26,    35,    36,    33,    34,     0,
      62,    60,   227,     0,   146,   147,     0,   143,     0,     0,
       0,     0,   205,     0,     0,   207,     0,    93,    95,   110,
     108,    94,   106,   144,   107,   203,   204,     0,     0,   145,
       0,    55,     0,   232,    25,     0,     0,     0,   199,   115,
     116,    32,    31,   117,   215,     0,   114,     0,     0,   209,
     140,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    98,     0,   148,   201,   146,   143,   111,
       0,    54,    52,   220,    48,     0,    44,    45,   221,   217,
     218,     0,     0,     0,    32,     0,   184,     0,     0,   154,
     154,   152,    64,    62,    85,    78,    75,     0,     0,    84,
       0,     0,    71,    72,     0,    77,     0,    70,    69,    68,
      67,     0,     0,     0,     0,     0,   139,   223,     0,     0,
     210,   211,    99,   119,   138,   137,   128,   129,   132,   131,
     135,   136,   130,   120,   121,   123,   118,   133,   134,   122,
     124,   125,   126,   127,     0,    97,    96,   112,     0,    53,
      56,   222,    41,     0,    98,    47,    79,   181,   176,   179,
     168,   180,   164,     0,   190,     0,   165,     0,   155,     0,
       0,   153,   151,   104,   103,     0,    80,   105,    63,    78,
     197,    66,     0,     0,    76,    73,    74,   189,    61,    65,
     109,     0,   206,   213,   208,     0,   101,   113,     0,    42,
      37,    48,    46,    98,   219,   172,     0,     0,   142,     0,
     185,   146,     0,   149,   150,    91,    92,    88,    89,    90,
      98,     0,    81,   198,   200,     0,   212,     0,   100,   202,
       0,   177,     0,   187,   169,   141,   191,     0,   166,    86,
       0,     0,   214,   102,     0,     0,     0,     0,   193,   186,
       0,    83,    82,     0,   178,    98,   188,   194,   192,   156,
     173,     0,     0,     0,     0,     0,   156,     0,     0,   183,
     195,   231,   162,     0,   161,   160,   167,   157,   158,    85,
     171,   170,   174,     0,   159,     0,   163,   175
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -251,  -251,  -251,  -251,  -251,   234,  -251,  -251,  -251,  -251,
     266,  -251,  -251,   243,  -251,  -251,  -251,   -26,   -89,  -251,
    -251,   230,  -251,    -6,  -251,  -119,  -251,    45,   247,  -251,
    -251,  -251,   129,  -251,  -251,  -251,  -251,   226,  -251,  -250,
    -251,  -251,   -69,  -251,  -251,  -251,   -87,   -58,  -251,   139,
    -251,  -251,  -251,   -65,   186,  -251,   -81,    43,  -240,  -251,
    -251,  -251,  -251,  -251,   116,   -59,  -251,  -251,  -251,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,  -251,  -251,  -220,   -37,
    -251,  -251,  -251,  -251,  -251,  -183,    11,  -111,  -251,  -251,
    -251,  -251,  -251,  -251,  -251,  -251,    17,  -251,  -251,  -251,
      61,  -251,   -21,  -251,  -251,  -251,   -47,  -251,   253
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    15,    26,    41,     5,     7,    10,    11,
      53,    17,    22,    23,    24,   114,    69,    40,    96,    79,
      54,    55,    18,   269,   290,   165,   166,   167,    34,    35,
      47,   111,   112,    19,    20,    42,    43,   186,   116,   187,
     188,   331,   189,   265,   190,   310,    59,   168,   234,    98,
     286,   318,   267,   191,    99,   100,   101,   254,   102,   103,
     192,   193,   194,   262,   259,   355,   356,   357,   363,   195,
     196,   197,   340,   198,   297,   372,   249,   320,   358,   375,
     295,   335,   250,   251,   336,   199,   255,   327,   324,   278,
     200,   299,   338,   348,   352,   201,   314,   104,   204,   238,
     105,   125,   106,   129,   209,   210,   211,   315,   107,   169,
     170,   108,    29,    45,    70,    71,    72,    73,   109
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      97,   123,   124,    46,   270,   202,   205,   119,   120,   130,
      28,   247,   126,   127,   141,   272,    56,   302,   121,   113,
      77,   121,   273,   150,   151,   152,   153,   184,    78,    75,
      27,   153,    76,   154,    62,    63,    64,    65,   154,    66,
      67,   361,     3,   362,     4,   122,     6,   323,   122,   323,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   236,   128,    57,   332,   202,   323,   248,   326,
      68,  -216,     8,   -32,     9,   281,   252,    68,    12,   172,
      27,   291,    21,   173,   171,   353,   354,   266,   184,    27,
     261,   174,   323,    25,   171,    30,   241,   339,  -216,    86,
     185,    37,    38,   175,    39,   176,   177,   263,   264,   288,
     263,   264,   178,  -196,  -196,   179,   180,   181,   122,    27,
     -57,   -57,   360,    13,   305,   306,   307,   308,   309,   337,
      58,    31,    14,    32,    33,   245,    -7,    44,    36,    91,
      48,    51,    92,    13,   182,    58,    80,    60,   183,    82,
     110,   -87,  -196,   113,   115,   117,   -31,   118,   -87,   155,
     156,   160,   298,   161,   368,   208,   162,   203,   301,    62,
      63,    85,    65,   171,    66,    67,   202,   141,   202,   207,
     237,   311,   239,  -216,   148,   149,   150,   151,   152,   153,
     242,   243,   256,   244,   258,   158,   154,   246,   184,  -207,
     184,   202,   282,   253,   268,   283,   202,   171,   257,   285,
     280,   271,   171,   274,   275,    68,   276,   296,   279,   289,
     277,   312,   303,   184,   304,   241,   333,   284,   184,   287,
     313,   202,   317,   341,   342,   334,   202,   172,   344,   328,
     347,   173,   329,   343,  -182,   154,   345,    27,   349,   174,
     359,   366,  -207,   184,   373,   365,   376,    86,   184,   369,
     321,   175,   374,   176,   177,   377,    52,    16,    61,    50,
     178,    81,    49,   179,   180,   181,   122,   351,   292,   370,
     240,   172,   235,   371,   159,   173,   260,   367,   300,   319,
     346,    27,   316,   174,    74,   294,   364,    91,     0,     0,
      92,    86,   182,     0,     0,   175,   183,   176,   177,     0,
     -62,     0,     0,     0,   178,     0,   163,   179,   180,   181,
     122,     0,     0,    83,     0,     0,    27,   164,    84,    62,
      63,    85,    65,     0,    66,    67,    86,     0,     0,     0,
       0,    91,     0,     0,    92,     0,   182,     0,     0,   163,
     183,     0,     0,     0,  -196,    87,    83,     0,     0,    27,
     164,    84,    62,    63,    85,    65,     0,    66,    67,    86,
       0,    88,     0,     0,    89,    90,    91,     0,     0,    92,
      93,     0,   -98,    94,     0,    95,     0,   -43,    87,    83,
       0,     0,     0,     0,   157,    62,    63,    85,    65,     0,
      66,    67,    86,     0,    88,     0,     0,    89,    90,    91,
     172,     0,    92,    93,   173,   -98,    94,   -43,    95,     0,
      27,   158,   174,     0,     0,     0,     0,     0,     0,     0,
      86,     0,     0,     0,   175,     0,   176,   177,     0,     0,
       0,    68,    91,   178,     0,    92,   179,   180,   181,   122,
      83,    95,     0,    27,     0,    84,    62,    63,    85,    65,
       0,    66,    67,    86,     0,     0,     0,     0,     0,     0,
      91,     0,     0,    92,     0,   182,     0,     0,     0,   322,
       0,    83,    87,     0,     0,   293,    84,    62,    63,    85,
      65,     0,    66,    67,    86,     0,     0,     0,    88,     0,
       0,    89,    90,    91,     0,     0,    92,    93,     0,     0,
      94,     0,    95,    87,    83,     0,     0,     0,     0,    84,
      62,    63,    85,    65,     0,    66,    67,    86,     0,    88,
       0,     0,    89,    90,    91,     0,     0,    92,    93,     0,
       0,    94,     0,    95,     0,     0,    87,     0,     0,     0,
       0,     0,     0,     0,     0,   131,     0,     0,     0,     0,
       0,     0,    88,     0,     0,    89,    90,    91,     0,     0,
      92,    93,     0,     0,    94,   132,    95,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,     0,     0,
       0,     0,     0,   132,   154,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,     0,     0,     0,     0,
       0,     0,   154,   330,   132,     0,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,     0,     0,     0,
       0,   206,   132,   154,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,     0,     0,     0,     0,   325,
     132,   154,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,     0,   350,     0,     0,     0,   132,   154,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   132,     0,   133,   134,   135,   136,   154,     0,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   132,     0,  -217,  -217,   135,   136,
     154,     0,   139,   140,   141,     0,     0,     0,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   132,     0,   133,
     134,   135,   136,   154,     0,   139,   140,   141,     0,     0,
       0,   145,   146,   147,   148,   149,   150,   151,   152,   153,
       0,     0,     0,   135,   136,     0,   154,   139,   140,   141,
       0,     0,     0,     0,   146,   147,   148,   149,   150,   151,
     152,   153,     0,   135,   136,     0,     0,     0,   154,   141,
       0,     0,     0,     0,     0,     0,   148,   149,   150,   151,
     152,   153,     0,     0,     0,     0,     0,     0,   154
};

static const yytype_int16 yycheck[] =
{
      58,    90,    91,    29,   187,   116,   125,    88,    89,    96,
      16,     1,    93,    94,    52,    64,    14,   257,    13,    14,
      13,    13,    71,    61,    62,    63,    64,   116,    21,    10,
      11,    64,    53,    71,    14,    15,    16,    17,    71,    19,
      20,    14,     0,    16,     3,    40,    13,   297,    40,   299,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,    94,    72,   315,   187,   327,    68,   299,
      60,    68,    66,    43,     4,   204,    73,    60,    16,     1,
      11,    12,    66,     5,   115,    35,    36,   184,   187,    11,
     181,    13,   352,    65,   125,    66,   164,   327,    68,    21,
     116,    13,    14,    25,    16,    27,    28,    23,    24,   238,
      23,    24,    34,    35,    36,    37,    38,    39,    40,    11,
       6,     7,   352,     9,    29,    30,    31,    32,    33,   322,
      43,    66,    18,    66,     6,   171,     0,    64,    67,    61,
       7,    43,    64,     9,    66,    43,    70,    68,    70,    72,
      13,    64,    74,    14,    68,    11,    43,    68,    71,    16,
      68,    43,   253,    67,   357,    16,    69,    68,    13,    14,
      15,    16,    17,   204,    19,    20,   297,    52,   299,    69,
      13,   272,    16,    68,    59,    60,    61,    62,    63,    64,
      69,    67,    13,    67,    14,    40,    71,    66,   297,    16,
     299,   322,    72,    68,    66,    41,   327,   238,    68,    67,
      69,    66,   243,    66,    66,    60,    66,   248,    66,    16,
      74,    13,    66,   322,    66,   293,   317,    74,   327,    72,
      69,   352,    22,   330,   331,    66,   357,     1,   335,    69,
      26,     5,   310,   334,    73,    71,    73,    11,    70,    13,
      69,    74,    16,   352,    22,    73,    14,    21,   357,   358,
     296,    25,    73,    27,    28,    69,    42,    11,    48,    36,
      34,    55,    35,    37,    38,    39,    40,   345,   243,   358,
     161,     1,   153,   358,   108,     5,   180,   356,   255,   288,
     337,    11,   285,    13,    51,   244,   353,    61,    -1,    -1,
      64,    21,    66,    -1,    -1,    25,    70,    27,    28,    -1,
      74,    -1,    -1,    -1,    34,    -1,     1,    37,    38,    39,
      40,    -1,    -1,     8,    -1,    -1,    11,    12,    13,    14,
      15,    16,    17,    -1,    19,    20,    21,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    64,    -1,    66,    -1,    -1,     1,
      70,    -1,    -1,    -1,    74,    40,     8,    -1,    -1,    11,
      12,    13,    14,    15,    16,    17,    -1,    19,    20,    21,
      -1,    56,    -1,    -1,    59,    60,    61,    -1,    -1,    64,
      65,    -1,    67,    68,    -1,    70,    -1,    72,    40,     8,
      -1,    -1,    -1,    -1,    13,    14,    15,    16,    17,    -1,
      19,    20,    21,    -1,    56,    -1,    -1,    59,    60,    61,
       1,    -1,    64,    65,     5,    67,    68,    69,    70,    -1,
      11,    40,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      21,    -1,    -1,    -1,    25,    -1,    27,    28,    -1,    -1,
      -1,    60,    61,    34,    -1,    64,    37,    38,    39,    40,
       8,    70,    -1,    11,    -1,    13,    14,    15,    16,    17,
      -1,    19,    20,    21,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    -1,    64,    -1,    66,    -1,    -1,    -1,    70,
      -1,     8,    40,    -1,    -1,    12,    13,    14,    15,    16,
      17,    -1,    19,    20,    21,    -1,    -1,    -1,    56,    -1,
      -1,    59,    60,    61,    -1,    -1,    64,    65,    -1,    -1,
      68,    -1,    70,    40,     8,    -1,    -1,    -1,    -1,    13,
      14,    15,    16,    17,    -1,    19,    20,    21,    -1,    56,
      -1,    -1,    59,    60,    61,    -1,    -1,    64,    65,    -1,
      -1,    68,    -1,    70,    -1,    -1,    40,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    56,    -1,    -1,    59,    60,    61,    -1,    -1,
      64,    65,    -1,    -1,    68,    42,    70,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    42,    71,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    71,    72,    42,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    69,    42,    71,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    -1,    -1,    -1,    -1,    69,
      42,    71,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    -1,    66,    -1,    -1,    -1,    42,    71,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    42,    -1,    44,    45,    46,    47,    71,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    42,    -1,    44,    45,    46,    47,
      71,    -1,    50,    51,    52,    -1,    -1,    -1,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    42,    -1,    44,
      45,    46,    47,    71,    -1,    50,    51,    52,    -1,    -1,
      -1,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      -1,    -1,    -1,    46,    47,    -1,    71,    50,    51,    52,
      -1,    -1,    -1,    -1,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    46,    47,    -1,    -1,    -1,    71,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    71
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    76,    77,     0,     3,    81,    13,    82,    66,     4,
      83,    84,    16,     9,    18,    78,    85,    86,    97,   108,
     109,    66,    87,    88,    89,    65,    79,    11,    98,   187,
      66,    66,    66,     6,   103,   104,    67,    13,    14,    16,
      92,    80,   110,   111,    64,   188,    92,   105,     7,   103,
      88,    43,    80,    85,    95,    96,    14,    72,    43,   121,
      68,    96,    14,    15,    16,    17,    19,    20,    60,    91,
     189,   190,   191,   192,   193,    10,   187,    13,    21,    94,
      70,   112,    72,     8,    13,    16,    21,    40,    56,    59,
      60,    61,    64,    65,    68,    70,    93,   122,   124,   129,
     130,   131,   133,   134,   172,   175,   177,   183,   186,   193,
      13,   106,   107,    14,    90,    68,   113,    11,    68,   131,
     131,    13,    40,    93,    93,   176,   131,   131,   187,   178,
     121,    22,    42,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    71,    16,    68,    13,    40,   129,
      43,    67,    69,     1,    12,   100,   101,   102,   122,   184,
     185,   187,     1,     5,    13,    25,    27,    28,    34,    37,
      38,    39,    66,    70,    93,    98,   112,   114,   115,   117,
     119,   128,   135,   136,   137,   144,   145,   146,   148,   160,
     165,   170,   172,    68,   173,   100,    69,    69,    16,   179,
     180,   181,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   123,   124,   131,    13,   174,    16,
     107,   122,    69,    67,    67,    92,    66,     1,    68,   151,
     157,   158,    73,    68,   132,   161,    13,    68,    14,   139,
     139,   131,   138,    23,    24,   118,   121,   127,    66,    98,
     170,    66,    64,    71,    66,    66,    66,    74,   164,    66,
      69,   100,    72,    41,    74,    67,   125,    72,   100,    16,
      99,    12,   102,    12,   185,   155,   187,   149,   131,   166,
     132,    13,   133,    66,    66,    29,    30,    31,    32,    33,
     120,   131,    13,    69,   171,   182,   181,    22,   126,   171,
     152,    92,    70,   114,   163,    69,   163,   162,    69,   122,
      72,   116,   133,   131,    66,   156,   159,   170,   167,   163,
     147,   121,   121,   131,   121,    73,   164,    26,   168,    70,
      66,   122,   169,    35,    36,   140,   141,   142,   153,    69,
     163,    14,    16,   143,   191,    73,    74,   140,   170,    93,
     117,   128,   150,    22,    73,   154,    14,    69
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    77,    78,    76,    79,    79,    80,    80,    81,
      81,    82,    82,    83,    84,    84,    84,    84,    85,    85,
      86,    87,    87,    89,    90,    88,    91,    91,    92,    92,
      92,    93,    93,    94,    94,    95,    95,    96,    97,    98,
      98,    99,    99,   100,   100,   101,   101,   102,   102,   103,
     103,   105,   104,   106,   107,   107,   107,   109,   108,   111,
     110,   112,   113,   113,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     115,   116,   115,   115,   115,   118,   117,   119,   120,   120,
     120,   120,   120,   121,   122,   122,   123,   123,   124,   125,
     124,   126,   126,   127,   127,   128,   129,   129,   129,   130,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   132,   132,   133,   133,   133,   133,   134,   134,   135,
     136,   137,   138,   138,   139,   139,   140,   140,   141,   142,
     142,   143,   143,   143,   144,   145,   147,   146,   149,   148,
     150,   150,   152,   153,   154,   151,   155,   156,   155,   157,
     157,   157,   159,   158,   161,   162,   160,   163,   163,   164,
     166,   167,   165,   168,   169,   168,   170,   170,   171,   173,
     172,   174,   172,   172,   172,   176,   175,   178,   177,   179,
     179,   180,   180,   182,   181,   183,   183,   100,   184,   184,
     184,   185,   185,   186,   187,   187,   188,   188,   189,   189,
     190,   191,   191,   192,   193,   193,   193,   193
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     7,     0,     1,     0,     2,     0,
       3,     0,     2,     3,     0,     3,     3,     3,     0,     1,
       2,     1,     3,     0,     0,     5,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     6,     2,     3,
       2,     0,     1,     0,     1,     1,     3,     2,     1,     0,
       2,     0,     5,     3,     0,     1,     3,     0,     4,     0,
       3,     4,     0,     3,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     2,     1,     1,     2,
       2,     0,     5,     5,     1,     0,     4,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     0,     0,
       5,     0,     2,     1,     1,     2,     1,     1,     1,     4,
       1,     2,     3,     4,     2,     2,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     2,     1,     1,     1,     1,     1,     2,     3,
       3,     2,     0,     1,     0,     1,     0,     2,     2,     3,
       2,     1,     1,     3,     2,     2,     0,     8,     0,     4,
       1,     1,     0,     0,     0,    10,     0,     0,     4,     1,
       1,     1,     0,     7,     0,     0,     5,     1,     3,     1,
       0,     0,     6,     0,     0,     3,     0,     2,     1,     0,
       5,     0,     5,     1,     1,     0,     4,     0,     4,     0,
       1,     1,     3,     0,     4,     2,     1,     1,     1,     3,
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
#line 269 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2119 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 275 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 2127 "parser.c"
    break;

  case 9: /* namespace: %empty  */
#line 285 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2135 "parser.c"
    break;

  case 10: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 288 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2144 "parser.c"
    break;

  case 13: /* require: T_REQUIRE T_STRING ';'  */
#line 296 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2153 "parser.c"
    break;

  case 18: /* scope: %empty  */
#line 306 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2159 "parser.c"
    break;

  case 19: /* scope: T_STATIC  */
#line 307 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2165 "parser.c"
    break;

  case 23: /* $@3: %empty  */
#line 317 "Parser.y"
                { _bDefiningConst = 1; }
#line 2171 "parser.c"
    break;

  case 24: /* $@4: %empty  */
#line 317 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2177 "parser.c"
    break;

  case 25: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 318 "Parser.y"
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
#line 2193 "parser.c"
    break;

  case 26: /* constant_literal: simple_literal  */
#line 331 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2201 "parser.c"
    break;

  case 27: /* constant_literal: T_STRING  */
#line 334 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2210 "parser.c"
    break;

  case 28: /* variable_identifier: T_IDENT  */
#line 340 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2220 "parser.c"
    break;

  case 29: /* variable_identifier: T_NUM  */
#line 345 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2231 "parser.c"
    break;

  case 30: /* variable_identifier: T_STRING  */
#line 351 "Parser.y"
                                   {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2240 "parser.c"
    break;

  case 31: /* variable_reference: T_VARIABLE  */
#line 357 "Parser.y"
                                        {	yyval.ident = yyvsp[0].ident; }
#line 2246 "parser.c"
    break;

  case 32: /* variable_reference: T_IDENT  */
#line 358 "Parser.y"
                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2258 "parser.c"
    break;

  case 33: /* macro_declaration: T_IDENT  */
#line 368 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2266 "parser.c"
    break;

  case 34: /* macro_declaration: T_FUNC  */
#line 371 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2276 "parser.c"
    break;

  case 35: /* macro_type: scope T_VOID  */
#line 377 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
				yyval.ident.type = VT_NIL;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(VT_NIL);
				}
			}
#line 2289 "parser.c"
    break;

  case 36: /* macro_type: scope type_name  */
#line 386 "Parser.y"
                                        {
				yyval.ident.type = yyvsp[0].ident.type;
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
				if (_bInNativeDefinition) {
					parser_startNativeMethod(yyvsp[0].ident.type);
				}
			}
#line 2302 "parser.c"
    break;

  case 37: /* macrostart: macro_type macro_declaration '(' parameter_list ')' optional_description  */
#line 396 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				yyval.ident.type = yyvsp[-5].ident.type;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2312 "parser.c"
    break;

  case 39: /* var_decl: type_name variable_identifier assignment_expression  */
#line 404 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});
			}
#line 2325 "parser.c"
    break;

  case 40: /* var_decl: type_name variable_identifier  */
#line 411 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2336 "parser.c"
    break;

  case 42: /* optional_description: T_STRING  */
#line 420 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2345 "parser.c"
    break;

  case 47: /* parameter_declaration: type_name variable_identifier  */
#line 434 "Parser.y"
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
#line 2360 "parser.c"
    break;

  case 48: /* parameter_declaration: T_ELLIPSIS  */
#line 443 "Parser.y"
                                       {
				if (_bInNativeDefinition) {
					parser_nativeMethodAddParam(yyvsp[0].ident.type, "...");
				}
			}
#line 2370 "parser.c"
    break;

  case 51: /* $@5: %empty  */
#line 452 "Parser.y"
                         {
				if (strcmp(yyvsp[0].ident.s, "@Method") == 0) {
					_currentNativeMethodAnnotation = hashmap_create(3, 0 ,0);
				}
				free(yyvsp[0].ident.s);
			}
#line 2381 "parser.c"
    break;

  case 53: /* annotation_value: T_IDENT T_ASSIGN T_STRING  */
#line 459 "Parser.y"
                                            {
				if (_currentNativeMethodAnnotation) {
					hashmap_put(_currentNativeMethodAnnotation, (intptr_t)yyvsp[-2].ident.s, (intptr_t)yyvsp[0].ident.s);
				} else {
					free(yyvsp[-2].ident.s);
					free(yyvsp[0].ident.s);
				}
			}
#line 2394 "parser.c"
    break;

  case 57: /* $@6: %empty  */
#line 472 "Parser.y"
                          {
					_bInNativeDefinition = 1;
				}
#line 2402 "parser.c"
    break;

  case 58: /* native_macro_definition: $@6 annotations T_NATIVE macrostart  */
#line 475 "Parser.y"
                                                                {
					if (!_currentNativeMethodAnnotation) {
						_currentNativeMethodAnnotation = hashmap_create(3, 0, 0);
					}
					char* pszLib = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"lib");
					char* pszMacroCName = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"name");
					char* pszSignature = (char*)hashmap_get(_currentNativeMethodAnnotation, (intptr_t)"signature");
					char* pszArgs = stringbuf_getString(_nativeMethodArgs);
					if (!pszSignature) {
						pszSignature = stringbuf_getString(_nativeMethodSignature);
					}
					if (!pszMacroCName) {
						pszMacroCName = yyvsp[0].ident.s;
					}
					_bInNativeDefinition = 0;
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
					hashmap_destroyWithKeysAndValues(_currentNativeMethodAnnotation);
					stringbuf_destroy(_nativeMethodArgs);
					stringbuf_destroy(_nativeMethodSignature);
					_nativeMethodArgs = 0;
					_nativeMethodSignature = 0;
				}
#line 2442 "parser.c"
    break;

  case 59: /* $@7: %empty  */
#line 511 "Parser.y"
                  { init_macroDefinition(); }
#line 2448 "parser.c"
    break;

  case 60: /* macro_definition: $@7 macrostart block  */
#line 512 "Parser.y"
                                                 {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName(yyvsp[-1].ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",yyvsp[-1].ident.s);
				bytecode_optimizeInstructions(_currentBytecodeBuffer);
				MACRO_PARAM param = {
					.mp_name = yyvsp[-1].ident.s,
					.mp_comment = _yyCurrentComment,
					.mp_returnType = yyvsp[-1].ident.type,
					.mp_numberOfLocalVariables = _localVariableIndex,
					.mp_bytecodeLength = _currentBytecodeBuffer->bb_current - _currentBytecodeBuffer->bb_start,
					.mp_buffer = _currentBytecodeBuffer->bb_start,
					.mp_scope = yyvsp[-1].ident.scope,
					.mp_sourceFile = yy_getCurrentInputFilename(),
					.mp_namespaceIdx = _currentNamespaceIndex
				};
				_compilerConfiguration->cb_insertNewMacro(&param);
				freeitem(&yyvsp[-1].ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			}
#line 2477 "parser.c"
    break;

  case 65: /* statement: call_expression ';'  */
#line 543 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2483 "parser.c"
    break;

  case 66: /* statement: assignment ';'  */
#line 544 "Parser.y"
                                         { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2489 "parser.c"
    break;

  case 67: /* statement: if_expression  */
#line 545 "Parser.y"
                                        { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2495 "parser.c"
    break;

  case 68: /* statement: while  */
#line 546 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2501 "parser.c"
    break;

  case 69: /* statement: for_loop_expression  */
#line 547 "Parser.y"
                                              { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); }
#line 2507 "parser.c"
    break;

  case 70: /* statement: switch_expression  */
#line 548 "Parser.y"
                                            { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2513 "parser.c"
    break;

  case 75: /* statement: block  */
#line 553 "Parser.y"
                                { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2519 "parser.c"
    break;

  case 76: /* statement: increment_expression ';'  */
#line 554 "Parser.y"
                                                   { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0});  }
#line 2525 "parser.c"
    break;

  case 78: /* statement: var_decl  */
#line 556 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2533 "parser.c"
    break;

  case 79: /* statement: error ';'  */
#line 559 "Parser.y"
                                    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2542 "parser.c"
    break;

  case 80: /* assignment: variable_reference assignment_expression  */
#line 564 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2550 "parser.c"
    break;

  case 81: /* $@8: %empty  */
#line 568 "Parser.y"
                                                      {
				parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2558 "parser.c"
    break;

  case 82: /* assignment: assignment_target '.' T_IDENT $@8 assignment_expression  */
#line 570 "Parser.y"
                                                {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2566 "parser.c"
    break;

  case 83: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 574 "Parser.y"
                                                                                          {
				_currentBytecodeBuffer->bb_current = bytecode_emitInstruction(_currentBytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2574 "parser.c"
    break;

  case 85: /* $@9: %empty  */
#line 580 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2582 "parser.c"
    break;

  case 86: /* shorthand_assignment: variable_reference $@9 shorthand_assignment_operator simple_expression  */
#line 582 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2591 "parser.c"
    break;

  case 87: /* assignment_target: variable_reference  */
#line 587 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2599 "parser.c"
    break;

  case 88: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 592 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2605 "parser.c"
    break;

  case 89: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 593 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2611 "parser.c"
    break;

  case 90: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 594 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2617 "parser.c"
    break;

  case 91: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 595 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2623 "parser.c"
    break;

  case 92: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 596 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2629 "parser.c"
    break;

  case 93: /* assignment_expression: T_ASSIGN simple_expression  */
#line 598 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2635 "parser.c"
    break;

  case 94: /* simple_expression: binary_expression  */
#line 600 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2641 "parser.c"
    break;

  case 99: /* $@10: %empty  */
#line 608 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2647 "parser.c"
    break;

  case 102: /* range_increment: T_DOTDOT binary_expression  */
#line 611 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2653 "parser.c"
    break;

  case 103: /* minusminus_plusplus: T_MINUSMINUS  */
#line 614 "Parser.y"
                                     { yyval.num = -1; }
#line 2659 "parser.c"
    break;

  case 104: /* minusminus_plusplus: T_PLUSPLUS  */
#line 615 "Parser.y"
                                     { yyval.num = 1; }
#line 2665 "parser.c"
    break;

  case 105: /* increment_expression: variable_reference minusminus_plusplus  */
#line 618 "Parser.y"
                                                               { 
				_currentBytecodeBuffer->bb_current = bytecode_emitIncrementExpression(_currentBytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, (int)yyvsp[0].num);
			}
#line 2673 "parser.c"
    break;

  case 106: /* s_bterm: value  */
#line 622 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2679 "parser.c"
    break;

  case 109: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 626 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2687 "parser.c"
    break;

  case 110: /* binary_expression: s_bterm  */
#line 631 "Parser.y"
                                { yyval.ident.type = yyvsp[0].ident.type;}
#line 2693 "parser.c"
    break;

  case 111: /* binary_expression: type_cast s_bterm  */
#line 632 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_currentBytecodeBuffer->bb_current = bytecode_emitBinaryOperation(_currentBytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2702 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression '.' T_IDENT  */
#line 637 "Parser.y"
                                                      {
				yyval.ident.type = parser_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2711 "parser.c"
    break;

  case 113: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 641 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2717 "parser.c"
    break;

  case 114: /* binary_expression: '!' binary_expression  */
#line 642 "Parser.y"
                                                { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2723 "parser.c"
    break;

  case 115: /* binary_expression: '~' binary_expression  */
#line 643 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2729 "parser.c"
    break;

  case 116: /* binary_expression: '+' binary_expression  */
#line 644 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2735 "parser.c"
    break;

  case 117: /* binary_expression: '-' variable_reference  */
#line 645 "Parser.y"
                                                 { _currentBytecodeBuffer->bb_current = bytecode_emitMultiplyWithLiteralExpression(_currentBytecodeBuffer, &yyvsp[0].v, -1); }
#line 2741 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression '~' binary_expression  */
#line 646 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2747 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 647 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2753 "parser.c"
    break;

  case 120: /* binary_expression: binary_expression '&' binary_expression  */
#line 648 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2759 "parser.c"
    break;

  case 121: /* binary_expression: binary_expression '|' binary_expression  */
#line 649 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2765 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression '+' binary_expression  */
#line 650 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2771 "parser.c"
    break;

  case 123: /* binary_expression: binary_expression '^' binary_expression  */
#line 651 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2777 "parser.c"
    break;

  case 124: /* binary_expression: binary_expression '-' binary_expression  */
#line 652 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2783 "parser.c"
    break;

  case 125: /* binary_expression: binary_expression '*' binary_expression  */
#line 653 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2789 "parser.c"
    break;

  case 126: /* binary_expression: binary_expression '/' binary_expression  */
#line 654 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2795 "parser.c"
    break;

  case 127: /* binary_expression: binary_expression '%' binary_expression  */
#line 655 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2801 "parser.c"
    break;

  case 128: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 656 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2807 "parser.c"
    break;

  case 129: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 657 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2813 "parser.c"
    break;

  case 130: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 658 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2819 "parser.c"
    break;

  case 131: /* binary_expression: binary_expression T_AND binary_expression  */
#line 659 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2825 "parser.c"
    break;

  case 132: /* binary_expression: binary_expression T_OR binary_expression  */
#line 660 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2831 "parser.c"
    break;

  case 133: /* binary_expression: binary_expression '<' binary_expression  */
#line 661 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2837 "parser.c"
    break;

  case 134: /* binary_expression: binary_expression '>' binary_expression  */
#line 662 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2843 "parser.c"
    break;

  case 135: /* binary_expression: binary_expression T_LE binary_expression  */
#line 663 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2849 "parser.c"
    break;

  case 136: /* binary_expression: binary_expression T_GE binary_expression  */
#line 664 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2855 "parser.c"
    break;

  case 137: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 665 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2861 "parser.c"
    break;

  case 138: /* binary_expression: binary_expression T_NE binary_expression  */
#line 666 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2867 "parser.c"
    break;

  case 139: /* binary_expression: '(' binary_expression ')'  */
#line 667 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2873 "parser.c"
    break;

  case 140: /* binary_expression: variable_reference assignment_expression  */
#line 668 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2881 "parser.c"
    break;

  case 142: /* condition: '(' binary_expression  */
#line 675 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2889 "parser.c"
    break;

  case 143: /* value: T_VARIABLE  */
#line 679 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 2898 "parser.c"
    break;

  case 144: /* value: string  */
#line 683 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2908 "parser.c"
    break;

  case 145: /* value: simple_literal  */
#line 688 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 2914 "parser.c"
    break;

  case 146: /* value: T_IDENT  */
#line 689 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 2924 "parser.c"
    break;

  case 147: /* string: T_STRING  */
#line 695 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2930 "parser.c"
    break;

  case 148: /* string: string T_STRING  */
#line 696 "Parser.y"
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
#line 2945 "parser.c"
    break;

  case 149: /* break: T_BREAK opt_num ';'  */
#line 708 "Parser.y"
                        {	bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2951 "parser.c"
    break;

  case 150: /* continue: T_CONTINUE opt_num ';'  */
#line 710 "Parser.y"
                                       {
				// in the case this is a continue in a for(int i;b;i++) loop, generate the i++ code before continuing.
				parser_flushBuffer(_breaklevel-(int)yyvsp[-1].num+1);
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); 
			}
#line 2961 "parser.c"
    break;

  case 151: /* return_expression: T_RETURN optional_bterm  */
#line 717 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 2969 "parser.c"
    break;

  case 154: /* opt_num: %empty  */
#line 724 "Parser.y"
                {	yyval.num = 1;	}
#line 2975 "parser.c"
    break;

  case 155: /* opt_num: T_NUM  */
#line 725 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 2981 "parser.c"
    break;

  case 160: /* case_selector: T_DEFAULT ':'  */
#line 734 "Parser.y"
                                      {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 2989 "parser.c"
    break;

  case 161: /* case_condition: integer_literal  */
#line 739 "Parser.y"
                                        {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].v.data.longValue});
			}
#line 2997 "parser.c"
    break;

  case 162: /* case_condition: T_STRING  */
#line 742 "Parser.y"
                                   {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 3005 "parser.c"
    break;

  case 163: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 745 "Parser.y"
                                               {
				bytecode_addSwitchCondition(_currentBytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 3013 "parser.c"
    break;

  case 164: /* label: T_IDENT ':'  */
#line 749 "Parser.y"
                            {
				bytecode_createBranchLabel(_currentBytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3022 "parser.c"
    break;

  case 165: /* goto_expression: T_GOTO T_IDENT  */
#line 755 "Parser.y"
                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,_currentBytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3031 "parser.c"
    break;

  case 166: /* $@11: %empty  */
#line 762 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3041 "parser.c"
    break;

  case 167: /* switch_expression: T_SWITCH '(' value ')' $@11 '{' case_clauses '}'  */
#line 768 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(_currentBytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3054 "parser.c"
    break;

  case 168: /* $@12: %empty  */
#line 778 "Parser.y"
                                         {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3063 "parser.c"
    break;

  case 169: /* for_loop_expression: T_FOR for_clause $@12 local_block  */
#line 781 "Parser.y"
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
#line 3080 "parser.c"
    break;

  case 172: /* $@13: %empty  */
#line 797 "Parser.y"
                                       {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3088 "parser.c"
    break;

  case 173: /* $@14: %empty  */
#line 799 "Parser.y"
                                                    {
				parser_switchToBuffer(_breaklevel+1);
			}
#line 3096 "parser.c"
    break;

  case 174: /* $@15: %empty  */
#line 801 "Parser.y"
                                        {
				YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				parser_switchToBuffer(0);
			}
#line 3105 "parser.c"
    break;

  case 175: /* for_to_clause: '(' opt_for_initializer $@13 ';' binary_expression ';' $@14 for_increment $@15 ')'  */
#line 804 "Parser.y"
                              {
				yyval.ident.s = yyvsp[-8].ident.s;
			}
#line 3113 "parser.c"
    break;

  case 177: /* $@16: %empty  */
#line 809 "Parser.y"
                                                        {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3121 "parser.c"
    break;

  case 178: /* opt_for_initializer: type_name variable_identifier $@16 assignment_expression  */
#line 811 "Parser.y"
                                                {
				yyval.ident.s = yyvsp[-2].ident.s;
			}
#line 3129 "parser.c"
    break;

  case 179: /* for_clause: for_to_clause  */
#line 815 "Parser.y"
                              {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3137 "parser.c"
    break;

  case 180: /* for_clause: in_clause  */
#line 818 "Parser.y"
                                    {
				yyval.ident.s = yyvsp[0].ident.s;
			}
#line 3145 "parser.c"
    break;

  case 181: /* for_clause: error  */
#line 821 "Parser.y"
                                {
				yyval.ident.s = 0;
				yyerror("Illegal for-loop expression.");
				yyerrok;
			}
#line 3155 "parser.c"
    break;

  case 182: /* $@17: %empty  */
#line 827 "Parser.y"
                                                  {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
				parser_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-1].ident.arraySize);
			}
#line 3166 "parser.c"
    break;

  case 183: /* in_clause: '(' type_name variable_identifier $@17 ':' simple_expression ')'  */
#line 832 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
				yyval.ident.s = yyvsp[-4].ident.s;
			}
#line 3178 "parser.c"
    break;

  case 184: /* $@18: %empty  */
#line 840 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lstartid,_breaklevel);
			}
#line 3186 "parser.c"
    break;

  case 185: /* $@19: %empty  */
#line 842 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3195 "parser.c"
    break;

  case 186: /* while: T_WHILE $@18 condition $@19 local_block  */
#line 845 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3207 "parser.c"
    break;

  case 190: /* $@20: %empty  */
#line 858 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3216 "parser.c"
    break;

  case 191: /* $@21: %empty  */
#line 861 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3224 "parser.c"
    break;

  case 192: /* if_expression: T_IF condition $@20 local_block $@21 else_clause  */
#line 863 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3233 "parser.c"
    break;

  case 193: /* else_clause: %empty  */
#line 868 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3242 "parser.c"
    break;

  case 194: /* $@22: %empty  */
#line 872 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(_currentBytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(_currentBytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3252 "parser.c"
    break;

  case 199: /* @23: %empty  */
#line 884 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3264 "parser.c"
    break;

  case 200: /* call_expression: T_FUNC '(' @23 parameter_list closing_paren  */
#line 890 "Parser.y"
                                                       {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3274 "parser.c"
    break;

  case 201: /* @24: %empty  */
#line 895 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3285 "parser.c"
    break;

  case 202: /* call_expression: function_id_or_pointer '(' @24 parameter_list closing_paren  */
#line 900 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3299 "parser.c"
    break;

  case 205: /* @25: %empty  */
#line 912 "Parser.y"
                       {
				_currentNativeMethodCalled = &_functionTable[FUNC_CreateArray];
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[0].funcp;
			}
#line 3310 "parser.c"
    break;

  case 206: /* array_constructor: '[' @25 parameter_list ']'  */
#line 917 "Parser.y"
                                             {
				_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
					C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(_currentNativeMethodCalled)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3321 "parser.c"
    break;

  case 207: /* @26: %empty  */
#line 924 "Parser.y"
                     {
		_currentNativeMethodCalled = &_functionTable[FUNC_CreateMap];
		_currentFunctionCallParamIndexP++;
		*_currentFunctionCallParamIndexP = 0;
		yyval.funcp = yyvsp[0].funcp;
	}
#line 3332 "parser.c"
    break;

  case 208: /* map_constructor: '{' @26 optional_map_associates '}'  */
#line 929 "Parser.y"
                                      {
		_currentBytecodeBuffer->bb_current = bytecode_emitFunctionCall(_currentBytecodeBuffer, 
			C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(_currentNativeMethodCalled)}, *_currentFunctionCallParamIndexP);
		_currentFunctionCallParamIndexP--;
		_currentNativeMethodCalled = 0;
	}
#line 3343 "parser.c"
    break;

  case 213: /* @27: %empty  */
#line 946 "Parser.y"
                                               { 
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[-1].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[-1].ident.s);
				(*_currentFunctionCallParamIndexP) += 2;
			}
#line 3354 "parser.c"
    break;

  case 215: /* function_id_or_pointer: '*' variable_reference  */
#line 954 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3368 "parser.c"
    break;

  case 216: /* function_id_or_pointer: T_IDENT  */
#line 964 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3377 "parser.c"
    break;

  case 220: /* nonempty_parameters: error  */
#line 973 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3385 "parser.c"
    break;

  case 221: /* parameter: simple_expression  */
#line 977 "Parser.y"
                                  { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3391 "parser.c"
    break;

  case 222: /* parameter: T_ELLIPSIS simple_expression  */
#line 978 "Parser.y"
                                                       { 
				yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; 
				YY_EMIT(C_SPREAD, (GENERIC_DATA){1});
			}
#line 3400 "parser.c"
    break;

  case 223: /* type_cast: '(' type_name ')'  */
#line 983 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3409 "parser.c"
    break;

  case 224: /* type_name: T_TYPE_IDENTIFIER  */
#line 988 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3417 "parser.c"
    break;

  case 225: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 991 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3426 "parser.c"
    break;

  case 227: /* array_size: '[' T_NUM ']'  */
#line 997 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3434 "parser.c"
    break;

  case 228: /* boolean_literal: T_TRUE  */
#line 1001 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3443 "parser.c"
    break;

  case 229: /* boolean_literal: T_FALSE  */
#line 1004 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3452 "parser.c"
    break;

  case 230: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 1009 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3461 "parser.c"
    break;

  case 231: /* integer_literal: T_NUM  */
#line 1014 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3470 "parser.c"
    break;

  case 232: /* integer_literal: '-' T_NUM  */
#line 1018 "Parser.y"
                                    {
				yyval.ident.type = VT_NUMBER;
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = -yyvsp[0].num;
			}
#line 3480 "parser.c"
    break;

  case 233: /* character_literal: T_CHARACTER  */
#line 1025 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3489 "parser.c"
    break;

  case 234: /* simple_literal: integer_literal  */
#line 1030 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3502 "parser.c"
    break;

  case 235: /* simple_literal: float_literal  */
#line 1038 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3515 "parser.c"
    break;

  case 236: /* simple_literal: character_literal  */
#line 1046 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3528 "parser.c"
    break;

  case 237: /* simple_literal: boolean_literal  */
#line 1054 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3541 "parser.c"
    break;


#line 3545 "parser.c"

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

#line 1062 "Parser.y"


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

