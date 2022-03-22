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
#line 46 "Parser.y"


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

static int	_currentNamespaceIndex;
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
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nHeapIndex, int bLocalVar, int nIncrement);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitLocalAssignment(BYTECODE_BUFFER* pBuffer, int nHeapIndex);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);
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

#define		YY_EMIT(opCode, param)		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, opCode, param);

extern int yylex(void );
void 		bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int typ, unsigned short arraySizeOrParamIndex, int nHeapIndex);

char*		bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int typ);
int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
char*		bytecode_generateAutoLabelName(char *prefix, int num);
int 		function_returnsString(void *ep);
static ARRAY_LIST* _currentArrayLiteral;
static int			_localVariableIndex;

IDENTIFIER_CONTEXT* _currentIdentifierContext;

static void parser_emitAssignment(struct tagPARSE_IDENTIFIER* pIdent) {
	if (pIdent->isLocalVar) {
		_bytecodeBuffer.bb_current = bytecode_emitLocalAssignment(&_bytecodeBuffer, pIdent->heapIndex);
	} else {
		_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, pIdent->s);
	}
}

static void parser_emitPushVariable(struct tagPARSE_IDENTIFIER* pIdent) {
	if (pIdent->isLocalVar) {
		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_PUSH_LOCAL_VARIABLE, (GENERIC_DATA){.intValue=pIdent->heapIndex});
	} else {
		YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=pIdent->s});
	}
}

static void parser_defineVariable(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal, int arraySize) {
	if (arraySize < 0) {
		yyerror("Illegal negative array size %d", arraySize);
	}
	if (_bInHeader) {
		bytecode_defineVariable(&_bytecodeBuffer, pszName, C_DEFINE_VARIABLE, sType, arraySize, 0);
	} else {
		bytecode_defineVariable(&_bytecodeBuffer, pszName, C_DEFINE_LOCAL_VARIABLE, sType, arraySize, _localVariableIndex);
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

static void push_newForeachCursor() {
	YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex++});
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


#line 246 "parser.c"

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
  YYSYMBOL_T_NEW = 6,                      /* T_NEW  */
  YYSYMBOL_T_STATIC = 7,                   /* T_STATIC  */
  YYSYMBOL_T_VOID = 8,                     /* T_VOID  */
  YYSYMBOL_T_TYPE_IDENTIFIER = 9,          /* T_TYPE_IDENTIFIER  */
  YYSYMBOL_T_IDENT = 10,                   /* T_IDENT  */
  YYSYMBOL_T_NUM = 11,                     /* T_NUM  */
  YYSYMBOL_T_FLOATING_POINT_NUMBER = 12,   /* T_FLOATING_POINT_NUMBER  */
  YYSYMBOL_T_STRING = 13,                  /* T_STRING  */
  YYSYMBOL_T_CHARACTER = 14,               /* T_CHARACTER  */
  YYSYMBOL_T_CONST = 15,                   /* T_CONST  */
  YYSYMBOL_T_TRUE = 16,                    /* T_TRUE  */
  YYSYMBOL_T_FALSE = 17,                   /* T_FALSE  */
  YYSYMBOL_T_FUNC = 18,                    /* T_FUNC  */
  YYSYMBOL_T_DOTDOT = 19,                  /* T_DOTDOT  */
  YYSYMBOL_T_PLUSPLUS = 20,                /* T_PLUSPLUS  */
  YYSYMBOL_T_MINUSMINUS = 21,              /* T_MINUSMINUS  */
  YYSYMBOL_T_IF = 22,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 23,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 24,                   /* T_WHILE  */
  YYSYMBOL_T_GOTO = 25,                    /* T_GOTO  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 26,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 27,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 28,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 29,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 30,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_SWITCH = 31,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 32,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 33,                 /* T_DEFAULT  */
  YYSYMBOL_T_BREAK = 34,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 35,                /* T_CONTINUE  */
  YYSYMBOL_T_RETURN = 36,                  /* T_RETURN  */
  YYSYMBOL_T_VARIABLE = 37,                /* T_VARIABLE  */
  YYSYMBOL_T_ASSOC_ARROW = 38,             /* T_ASSOC_ARROW  */
  YYSYMBOL_T_NMATCH = 39,                  /* T_NMATCH  */
  YYSYMBOL_T_ASSIGN = 40,                  /* T_ASSIGN  */
  YYSYMBOL_T_NE = 41,                      /* T_NE  */
  YYSYMBOL_T_EQ = 42,                      /* T_EQ  */
  YYSYMBOL_T_SHIFT_LEFT = 43,              /* T_SHIFT_LEFT  */
  YYSYMBOL_T_SHIFT_RIGHT = 44,             /* T_SHIFT_RIGHT  */
  YYSYMBOL_T_OR = 45,                      /* T_OR  */
  YYSYMBOL_T_AND = 46,                     /* T_AND  */
  YYSYMBOL_T_LE = 47,                      /* T_LE  */
  YYSYMBOL_T_GE = 48,                      /* T_GE  */
  YYSYMBOL_T_POWER_TO = 49,                /* T_POWER_TO  */
  YYSYMBOL_50_ = 50,                       /* '&'  */
  YYSYMBOL_51_ = 51,                       /* '|'  */
  YYSYMBOL_52_ = 52,                       /* '^'  */
  YYSYMBOL_53_ = 53,                       /* '~'  */
  YYSYMBOL_54_ = 54,                       /* '<'  */
  YYSYMBOL_55_ = 55,                       /* '>'  */
  YYSYMBOL_56_ = 56,                       /* '+'  */
  YYSYMBOL_57_ = 57,                       /* '-'  */
  YYSYMBOL_58_ = 58,                       /* '*'  */
  YYSYMBOL_59_ = 59,                       /* '/'  */
  YYSYMBOL_60_ = 60,                       /* '%'  */
  YYSYMBOL_61_ = 61,                       /* '['  */
  YYSYMBOL_62_ = 62,                       /* '!'  */
  YYSYMBOL_63_ = 63,                       /* ';'  */
  YYSYMBOL_64_ = 64,                       /* ','  */
  YYSYMBOL_65_ = 65,                       /* '('  */
  YYSYMBOL_66_ = 66,                       /* ')'  */
  YYSYMBOL_67_ = 67,                       /* '{'  */
  YYSYMBOL_68_ = 68,                       /* '.'  */
  YYSYMBOL_69_ = 69,                       /* ']'  */
  YYSYMBOL_70_ = 70,                       /* ':'  */
  YYSYMBOL_71_ = 71,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 72,                  /* $accept  */
  YYSYMBOL_file_structure = 73,            /* file_structure  */
  YYSYMBOL_74_1 = 74,                      /* $@1  */
  YYSYMBOL_75_2 = 75,                      /* $@2  */
  YYSYMBOL_header_delimiter = 76,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 77,         /* macro_definitions  */
  YYSYMBOL_namespace = 78,                 /* namespace  */
  YYSYMBOL_prerequisites = 79,             /* prerequisites  */
  YYSYMBOL_require = 80,                   /* require  */
  YYSYMBOL_global_declarations = 81,       /* global_declarations  */
  YYSYMBOL_scope = 82,                     /* scope  */
  YYSYMBOL_constants = 83,                 /* constants  */
  YYSYMBOL_constant_list = 84,             /* constant_list  */
  YYSYMBOL_constdef = 85,                  /* constdef  */
  YYSYMBOL_86_3 = 86,                      /* $@3  */
  YYSYMBOL_87_4 = 87,                      /* $@4  */
  YYSYMBOL_constant_literal = 88,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 89,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 90,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 91,         /* macro_declaration  */
  YYSYMBOL_macro_type = 92,                /* macro_type  */
  YYSYMBOL_macrostart = 93,                /* macrostart  */
  YYSYMBOL_global_var = 94,                /* global_var  */
  YYSYMBOL_var_decl = 95,                  /* var_decl  */
  YYSYMBOL_optional_description = 96,      /* optional_description  */
  YYSYMBOL_parameter_list = 97,            /* parameter_list  */
  YYSYMBOL_non_empty_pardecl = 98,         /* non_empty_pardecl  */
  YYSYMBOL_par_decl = 99,                  /* par_decl  */
  YYSYMBOL_macro_definition = 100,         /* macro_definition  */
  YYSYMBOL_101_5 = 101,                    /* $@5  */
  YYSYMBOL_block = 102,                    /* block  */
  YYSYMBOL_var_decls = 103,                /* var_decls  */
  YYSYMBOL_statement = 104,                /* statement  */
  YYSYMBOL_assignment = 105,               /* assignment  */
  YYSYMBOL_106_6 = 106,                    /* $@6  */
  YYSYMBOL_107_7 = 107,                    /* $@7  */
  YYSYMBOL_assignment_target = 108,        /* assignment_target  */
  YYSYMBOL_shorthand_assignment_operator = 109, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 110,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 111,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 112, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 113,         /* range_expression  */
  YYSYMBOL_114_8 = 114,                    /* $@8  */
  YYSYMBOL_range_increment = 115,          /* range_increment  */
  YYSYMBOL_increment_expression = 116,     /* increment_expression  */
  YYSYMBOL_s_bterm = 117,                  /* s_bterm  */
  YYSYMBOL_constructor_expression = 118,   /* constructor_expression  */
  YYSYMBOL_binary_expression = 119,        /* binary_expression  */
  YYSYMBOL_condition = 120,                /* condition  */
  YYSYMBOL_value = 121,                    /* value  */
  YYSYMBOL_struct_reference = 122,         /* struct_reference  */
  YYSYMBOL_map_literal = 123,              /* map_literal  */
  YYSYMBOL_124_9 = 124,                    /* $@9  */
  YYSYMBOL_optional_map_associates = 125,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 126,           /* map_associates  */
  YYSYMBOL_map_associate = 127,            /* map_associate  */
  YYSYMBOL_array_literal = 128,            /* array_literal  */
  YYSYMBOL_129_10 = 129,                   /* $@10  */
  YYSYMBOL_130_11 = 130,                   /* $@11  */
  YYSYMBOL_array_elements = 131,           /* array_elements  */
  YYSYMBOL_array_element = 132,            /* array_element  */
  YYSYMBOL_simple_array_element = 133,     /* simple_array_element  */
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
  YYSYMBOL_goto = 145,                     /* goto  */
  YYSYMBOL_in_clause = 146,                /* in_clause  */
  YYSYMBOL_147_12 = 147,                   /* $@12  */
  YYSYMBOL_switch_expression = 148,        /* switch_expression  */
  YYSYMBOL_149_13 = 149,                   /* $@13  */
  YYSYMBOL_foreach = 150,                  /* foreach  */
  YYSYMBOL_151_14 = 151,                   /* $@14  */
  YYSYMBOL_152_15 = 152,                   /* $@15  */
  YYSYMBOL_while = 153,                    /* while  */
  YYSYMBOL_154_16 = 154,                   /* $@16  */
  YYSYMBOL_155_17 = 155,                   /* $@17  */
  YYSYMBOL_local_block = 156,              /* local_block  */
  YYSYMBOL_closing_brace = 157,            /* closing_brace  */
  YYSYMBOL_if_expression = 158,            /* if_expression  */
  YYSYMBOL_159_18 = 159,                   /* $@18  */
  YYSYMBOL_160_19 = 160,                   /* $@19  */
  YYSYMBOL_else_clause = 161,              /* else_clause  */
  YYSYMBOL_162_20 = 162,                   /* $@20  */
  YYSYMBOL_stmntlist = 163,                /* stmntlist  */
  YYSYMBOL_164_21 = 164,                   /* $@21  */
  YYSYMBOL_closing_paren = 165,            /* closing_paren  */
  YYSYMBOL_call_expression = 166,          /* call_expression  */
  YYSYMBOL_167_22 = 167,                   /* @22  */
  YYSYMBOL_168_23 = 168,                   /* @23  */
  YYSYMBOL_function_id_or_pointer = 169,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 170,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 171,                /* parameter  */
  YYSYMBOL_type_cast = 172,                /* type_cast  */
  YYSYMBOL_type_name = 173,                /* type_name  */
  YYSYMBOL_array_size = 174,               /* array_size  */
  YYSYMBOL_boolean_literal = 175,          /* boolean_literal  */
  YYSYMBOL_float_literal = 176,            /* float_literal  */
  YYSYMBOL_integer_literal = 177,          /* integer_literal  */
  YYSYMBOL_character_literal = 178,        /* character_literal  */
  YYSYMBOL_simple_literal = 179            /* simple_literal  */
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
#define YYLAST   1024

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  225
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  347

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   304


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
       2,     2,     2,    62,     2,     2,     2,    60,    50,     2,
      65,    66,    58,    56,    64,    57,    68,    59,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    70,    63,
      54,     2,    55,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    61,     2,    69,    52,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    67,    51,    71,    53,     2,     2,     2,
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
      45,    46,    47,    48,    49
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   206,   206,   212,   206,   216,   217,   219,   220,   222,
     225,   230,   231,   233,   238,   239,   240,   242,   243,   247,
     250,   251,   253,   253,   253,   267,   270,   276,   281,   288,
     295,   296,   306,   309,   315,   320,   325,   331,   333,   339,
     346,   348,   353,   354,   357,   359,   362,   369,   369,   393,
     395,   396,   398,   399,   400,   401,   402,   403,   404,   405,
     406,   407,   408,   409,   410,   411,   412,   415,   421,   425,
     425,   431,   435,   435,   442,   447,   448,   449,   450,   451,
     453,   455,   456,   459,   460,   462,   463,   463,   465,   466,
     469,   471,   473,   474,   475,   477,   482,   483,   487,   496,
     497,   498,   499,   500,   501,   502,   503,   504,   505,   506,
     507,   508,   509,   510,   511,   512,   513,   514,   515,   516,
     517,   518,   519,   520,   521,   522,   523,   529,   530,   534,
     538,   539,   544,   545,   551,   557,   563,   568,   573,   573,
     577,   579,   582,   584,   587,   591,   596,   596,   600,   600,
     605,   606,   609,   613,   618,   619,   620,   621,   623,   624,
     635,   638,   642,   646,   647,   649,   650,   652,   653,   655,
     657,   659,   664,   667,   670,   674,   679,   684,   684,   700,
     699,   715,   717,   715,   728,   730,   728,   741,   741,   743,
     746,   749,   746,   756,   760,   760,   767,   768,   768,   774,
     776,   776,   787,   787,   803,   813,   817,   823,   825,   826,
     827,   831,   833,   838,   841,   846,   847,   851,   854,   859,
     864,   869,   874,   882,   890,   898
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
  "T_FOR", "T_NEW", "T_STATIC", "T_VOID", "T_TYPE_IDENTIFIER", "T_IDENT",
  "T_NUM", "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER", "T_CONST",
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
  "non_empty_pardecl", "par_decl", "macro_definition", "$@5", "block",
  "var_decls", "statement", "assignment", "$@6", "$@7",
  "assignment_target", "shorthand_assignment_operator",
  "assignment_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@8",
  "range_increment", "increment_expression", "s_bterm",
  "constructor_expression", "binary_expression", "condition", "value",
  "struct_reference", "map_literal", "$@9", "optional_map_associates",
  "map_associates", "map_associate", "array_literal", "$@10", "$@11",
  "array_elements", "array_element", "simple_array_element", "string",
  "break", "continue", "return_expression", "optional_bterm", "opt_num",
  "case_clauses", "case_clause", "case_selector", "case_condition",
  "label", "goto", "in_clause", "$@12", "switch_expression", "$@13",
  "foreach", "$@14", "$@15", "while", "$@16", "$@17", "local_block",
  "closing_brace", "if_expression", "$@18", "$@19", "else_clause", "$@20",
  "stmntlist", "$@21", "closing_paren", "call_expression", "@22", "@23",
  "function_id_or_pointer", "nonempty_parameters", "parameter",
  "type_cast", "type_name", "array_size", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-290)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-207)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -290,    36,    34,  -290,    44,  -290,    -7,    53,  -290,    79,
    -290,    16,    41,  -290,  -290,    50,    92,    64,    65,  -290,
      49,  -290,    80,  -290,   131,    83,  -290,    80,  -290,  -290,
    -290,  -290,  -290,  -290,   107,  -290,   131,   142,    18,  -290,
     110,  -290,   104,  -290,     8,    43,    84,    86,  -290,   248,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,    87,  -290,  -290,
    -290,  -290,   147,   -22,  -290,    93,   -27,   633,   633,   691,
       5,    91,   633,   539,  -290,   110,  -290,  -290,  -290,  -290,
     790,  -290,    95,  -290,  -290,   154,  -290,   100,   763,  -290,
    -290,   459,   152,   106,  -290,   159,   747,   153,  -290,   153,
    -290,  -290,  -290,   111,   227,   894,   842,   113,   160,  -290,
     633,   633,   633,   633,   633,   633,   633,   633,   633,   633,
     633,   633,   633,   633,   633,   633,   633,   633,   633,   633,
     633,   633,   397,   172,  -290,  -290,   119,   117,  -290,   127,
     128,   133,  -290,  -290,   136,  -290,    80,   132,  -290,    38,
     138,  -290,   191,   140,   196,   196,   315,  -290,   -10,   145,
    -290,  -290,   155,    -9,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,   146,   157,   150,   459,  -290,  -290,
    -290,  -290,  -290,  -290,  -290,    60,  -290,  -290,  -290,  -290,
     183,   146,   158,  -290,   894,   747,   940,   940,    39,    39,
     917,   917,   -11,   -11,   163,   963,   963,   963,   747,   -11,
     -11,   153,   153,   163,   163,   163,   162,  -290,   790,  -290,
     459,   215,    92,   477,  -290,  -290,   171,  -290,   633,  -290,
     138,  -290,   129,  -290,   174,   182,   894,  -290,  -290,  -290,
     334,  -290,  -290,   101,  -290,   633,   236,  -290,  -290,  -290,
    -290,   181,   227,  -290,   342,  -290,   160,   229,  -290,   181,
    -290,  -290,  -290,  -290,    92,  -290,   868,   705,  -290,  -290,
     185,  -290,  -290,  -290,  -290,  -290,  -290,  -290,   557,  -290,
    -290,   813,  -290,  -290,  -290,  -290,  -290,  -290,  -290,   633,
    -290,  -290,    80,   705,  -290,   365,  -290,  -290,   705,  -290,
    -290,   110,   110,  -290,  -290,  -290,   146,   233,  -290,   201,
    -290,  -290,   193,  -290,  -290,  -290,    -5,   615,   705,    94,
     199,   203,    -5,   101,   186,  -290,   256,  -290,   207,  -290,
    -290,  -290,  -290,  -290,   275,  -290,  -290
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     9,     1,     0,    11,     0,    14,    10,     0,
      12,     3,     0,    18,    22,     5,     0,     0,     0,    13,
      19,    20,     0,     6,    47,   213,    37,     0,    15,    16,
      22,    27,    28,    29,     0,     4,    47,    17,     0,   214,
      39,    21,     0,     8,     0,     0,     0,     0,   215,     0,
      38,   220,   219,    26,   221,   217,   218,    23,   225,   223,
     222,   224,    25,    34,    35,    32,    33,     0,    50,    48,
     216,   206,     0,   135,   158,     0,   129,     0,     0,     0,
       0,   148,     0,     0,   138,     0,    80,    82,    96,    94,
      81,    92,   130,   133,   134,   131,    93,     0,     0,   132,
      24,     0,     0,     0,   200,     0,   101,   102,    98,   103,
      31,    30,   204,     0,     0,   100,     0,     0,   140,   126,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   159,   202,   135,   129,    97,   210,
       0,    43,    44,   211,   207,   208,     0,   206,   181,    31,
       0,   184,     0,     0,   165,   165,     0,    52,    72,    66,
      63,   197,     0,     0,    64,    59,    60,    61,    65,    62,
      58,    57,    56,    55,     0,     0,     0,     0,   136,   147,
     154,   153,   155,   156,   157,     0,   151,   152,   125,   212,
       0,     0,   141,   142,    86,   105,   124,   123,   114,   115,
     118,   117,   121,   122,   116,   106,   107,   109,   104,   119,
     120,   108,   110,   111,   112,   113,     0,    84,    83,   137,
       0,    40,     0,     0,    46,    67,     0,   175,     0,   190,
       0,   176,     0,   166,     0,     0,   164,   162,    91,    90,
       0,    68,    51,     0,    54,     0,     0,   189,    49,    53,
      95,     0,     0,   149,     0,   139,     0,    88,    99,     0,
      41,    36,    45,   209,     0,   182,   128,     0,   185,   135,
       0,   160,   161,    78,    79,    75,    76,    77,     0,    66,
     198,     0,    69,   199,   201,   150,   145,   144,   143,     0,
      87,   203,     0,     0,   127,     0,   187,   191,     0,   179,
      73,     0,     0,    89,   177,   183,     0,   193,   186,     0,
      71,    70,     0,   188,   194,   192,   167,     0,     0,     0,
       0,     0,   167,     0,     0,   195,   172,   173,     0,   171,
     180,   168,   169,   178,     0,   170,   174
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -290,  -290,  -290,  -290,  -290,   252,  -290,  -290,  -290,  -290,
     281,  -290,  -290,   292,  -290,  -290,  -290,   -26,   -78,  -290,
    -290,  -290,  -290,    -8,  -290,  -175,  -290,    98,  -290,  -290,
     277,  -290,  -268,  -290,  -290,  -290,  -290,  -290,   -79,   -49,
    -290,   192,  -290,  -290,  -290,   237,  -290,   -57,    96,    99,
    -290,  -290,  -290,  -290,  -290,    76,  -290,  -290,  -290,  -290,
      81,  -290,  -290,  -290,  -290,  -290,  -290,   179,    13,  -290,
    -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,  -290,
    -290,  -290,  -290,  -290,  -289,  -194,  -290,  -290,  -290,  -290,
    -290,  -219,  -290,    88,   -99,  -290,  -290,  -290,  -290,   134,
    -290,   -39,  -290,  -290,  -290,  -290,  -290,   -38
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    15,    24,    35,     5,     7,    10,    11,
      16,    17,    20,    21,    22,   100,    57,    34,    85,    67,
      45,    46,    18,   289,   271,   150,   151,   152,    36,    37,
     170,   102,   171,   172,   312,   250,   173,   288,    50,   153,
     226,    87,   267,   300,   174,    88,    89,    90,   239,    91,
      92,    93,   118,   201,   202,   203,    94,   113,   114,   195,
     196,   197,    95,   175,   176,   177,   247,   244,   331,   332,
     333,   338,   178,   179,   275,   322,   180,   319,   181,   236,
     303,   182,   240,   308,   307,   258,   183,   277,   317,   325,
     328,   184,   253,   294,    96,   187,   230,    97,   154,   155,
      98,    27,    39,    58,    59,    60,    61,    99
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      86,    40,   112,   185,    62,    64,   119,   265,    26,   306,
     248,   249,   261,   -30,   315,   110,    63,    25,   -31,   318,
     106,   107,   109,    13,   168,   115,   116,   329,   330,    47,
      49,    14,   124,   125,   290,   306,     3,     4,   130,   335,
     306,   105,   111,  -205,   117,   137,   138,   139,   140,   141,
     142,   -74,   255,    65,     6,   269,     8,     9,   -74,   256,
     306,    66,   156,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   228,   316,    48,   130,   251,
      31,    32,    12,    33,   169,   137,   138,   139,   140,   141,
     142,    25,   157,  -205,    19,   336,   158,   337,   237,   246,
      25,   159,    23,    30,   342,    51,    52,    53,    54,    75,
      55,    56,   323,   160,   262,   161,   162,    28,    29,   263,
     234,    -7,   163,  -196,  -196,   164,   165,   166,   111,   279,
      51,    52,    74,    54,    38,    55,    56,    42,   156,    13,
      49,    68,   101,   157,   185,    70,   103,   158,   104,    80,
    -146,    25,   159,   143,   167,   145,   147,   144,    68,   188,
      75,   186,  -196,   200,   160,   168,   161,   162,   185,   199,
     189,   276,   229,   163,  -205,   105,   164,   165,   166,   111,
      81,   156,  -206,   156,   231,   235,    84,   232,   291,   168,
     233,   241,   130,   238,   185,   242,   185,   243,   252,   185,
      80,   139,   140,   141,   142,   167,   260,   257,   254,    68,
     259,   264,   266,  -196,   142,   168,   297,   168,   270,   185,
     168,   268,   320,   321,   185,   302,   274,   281,   190,   310,
     191,   192,   313,   193,   194,   282,   292,   293,   299,    71,
     168,   309,   343,   -85,    72,   168,   324,   -85,    73,    51,
      52,    74,    54,   327,    55,    56,    75,   -85,   326,   339,
     -85,   -85,   -85,   -85,   340,   344,   314,   345,   334,   -85,
     -85,   -85,   -85,   -85,   -85,    76,   346,   -85,    43,   -85,
     -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,
     -85,    77,   -85,   -85,    78,    79,    80,   -85,   -85,    81,
      82,   -85,   -85,    83,   -85,    84,    71,   -85,    44,   -85,
    -163,    72,    41,    69,  -163,    73,    51,    52,    74,    54,
     272,    55,    56,    75,   227,   148,   278,  -163,  -163,  -163,
    -163,   280,   298,   295,   245,   341,  -163,  -163,  -163,  -163,
    -163,  -163,    76,    51,    52,   296,    54,   301,    55,    56,
     283,   284,   285,   286,   287,     0,   157,   273,    77,     0,
     158,    78,    79,    80,    25,   159,    81,    82,  -163,     0,
      83,     0,    84,    75,     0,     0,  -163,   160,     0,   161,
     162,     0,     0,     0,     0,     0,   163,     0,    71,   164,
     165,   166,   111,    72,     0,     0,     0,    73,    51,    52,
      74,    54,     0,    55,    56,    75,     0,     0,     0,     0,
       0,     0,     0,    80,     0,     0,     0,     0,   167,     0,
       0,     0,    68,     0,    76,     0,   -50,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      77,     0,     0,    78,    79,    80,     0,     0,    81,    82,
     149,     0,    83,     0,    84,    72,   -85,     0,    25,    73,
      51,    52,    74,    54,     0,    55,    56,    75,    71,     0,
       0,     0,     0,    72,     0,     0,     0,    73,    51,    52,
      74,    54,     0,    55,    56,    75,    76,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    77,     0,    76,    78,    79,    80,     0,     0,
      81,    82,     0,   -85,    83,   -42,    84,     0,     0,     0,
      77,     0,     0,    78,    79,    80,     0,     0,    81,    82,
      71,   -85,    83,   -85,    84,    72,     0,     0,    25,    73,
      51,    52,    74,    54,     0,    55,    56,    75,    71,     0,
       0,     0,     0,    72,     0,     0,     0,    73,    51,    52,
      74,    54,     0,    55,    56,    75,    76,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    77,     0,    76,    78,    79,    80,     0,     0,
      81,    82,     0,     0,    83,     0,    84,     0,     0,     0,
      77,     0,     0,    78,    79,    80,    71,     0,    81,    82,
     -85,    72,    83,     0,    84,    73,    51,    52,    74,    54,
       0,    55,    56,    75,    71,     0,     0,     0,     0,    72,
       0,     0,     0,    73,    51,    52,    74,    54,     0,    55,
      56,    75,    76,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    77,     0,
      76,    78,    79,    80,     0,     0,    81,    82,     0,     0,
      83,   -85,    84,     0,     0,     0,    77,     0,     0,    78,
      79,    80,    71,     0,    81,    82,     0,    72,    83,     0,
      84,    73,   108,    52,    74,    54,   157,    55,    56,    75,
     158,     0,     0,     0,    25,   159,     0,     0,     0,     0,
       0,     0,     0,    75,     0,     0,     0,   160,    76,   161,
     162,     0,     0,     0,     0,     0,   163,     0,     0,   164,
     165,   166,   111,     0,    77,     0,     0,    78,    79,    80,
       0,     0,    81,    82,     0,     0,    83,     0,    84,     0,
       0,     0,     0,    80,    71,     0,     0,     0,   167,    72,
       0,     0,   305,   146,    51,    52,    74,    54,     0,    55,
      56,    75,     0,     0,     0,     0,     0,     0,     0,     0,
     124,   125,     0,     0,   128,   129,   130,     0,     0,     0,
     147,   135,   136,   137,   138,   139,   140,   141,   142,   120,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    80,     0,     0,    81,     0,     0,     0,     0,   121,
      84,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   121,     0,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,     0,     0,     0,     0,     0,
       0,   121,   311,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,     0,     0,     0,   121,   198,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
       0,     0,     0,   121,   304,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   121,     0,   122,   123,
     124,   125,     0,     0,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   121,
       0,  -207,  -207,   124,   125,     0,     0,   128,   129,   130,
       0,     0,     0,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   121,     0,   122,   123,   124,   125,     0,     0,
     128,   129,   130,     0,     0,     0,   134,   135,   136,   137,
     138,   139,   140,   141,   142
};

static const yytype_int16 yycheck[] =
{
      49,    27,    80,   102,    42,    44,    85,   201,    16,   277,
      20,    21,   187,    40,   303,    10,     8,     9,    40,   308,
      77,    78,    79,     7,   102,    82,    83,    32,    33,    11,
      40,    15,    43,    44,   253,   303,     0,     3,    49,   328,
     308,    68,    37,    65,    83,    56,    57,    58,    59,    60,
      61,    61,    61,    10,    10,   230,    63,     4,    68,    68,
     328,    18,   101,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   305,    69,    49,   168,
      10,    11,    13,    13,   102,    56,    57,    58,    59,    60,
      61,     9,     1,    65,    63,    11,     5,    13,    70,   166,
       9,    10,    62,    64,   333,    11,    12,    13,    14,    18,
      16,    17,   316,    22,    64,    24,    25,    63,    63,    69,
     156,     0,    31,    32,    33,    34,    35,    36,    37,    10,
      11,    12,    13,    14,    61,    16,    17,    40,   187,     7,
      40,    67,    65,     1,   253,    69,     9,     5,    65,    58,
      69,     9,    10,    68,    63,    65,    37,    13,    67,    10,
      18,    65,    71,    13,    22,   253,    24,    25,   277,    66,
      69,   238,    10,    31,    65,    68,    34,    35,    36,    37,
      61,   230,    65,   232,    66,    63,    67,    64,   255,   277,
      64,    10,    49,    65,   303,    65,   305,    11,    63,   308,
      58,    58,    59,    60,    61,    63,    66,    71,    63,    67,
      63,    38,    64,    71,    61,   303,   264,   305,    13,   328,
     308,    69,   311,   312,   333,   274,    65,    63,    11,   288,
      13,    14,   299,    16,    17,    63,    10,    66,    19,     1,
     328,    66,    66,     5,     6,   333,    23,     9,    10,    11,
      12,    13,    14,    70,    16,    17,    18,    19,    67,    70,
      22,    23,    24,    25,    71,    19,   302,    70,   327,    31,
      32,    33,    34,    35,    36,    37,    11,    39,    36,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     1,    69,    37,    71,
       5,     6,    30,    46,     9,    10,    11,    12,    13,    14,
     232,    16,    17,    18,   142,    98,   240,    22,    23,    24,
      25,   242,   266,   262,   165,   332,    31,    32,    33,    34,
      35,    36,    37,    11,    12,    13,    14,   269,    16,    17,
      26,    27,    28,    29,    30,    -1,     1,   233,    53,    -1,
       5,    56,    57,    58,     9,    10,    61,    62,    63,    -1,
      65,    -1,    67,    18,    -1,    -1,    71,    22,    -1,    24,
      25,    -1,    -1,    -1,    -1,    -1,    31,    -1,     1,    34,
      35,    36,    37,     6,    -1,    -1,    -1,    10,    11,    12,
      13,    14,    -1,    16,    17,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,    63,    -1,
      -1,    -1,    67,    -1,    37,    -1,    71,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,    -1,    -1,    56,    57,    58,    -1,    -1,    61,    62,
       1,    -1,    65,    -1,    67,     6,    69,    -1,     9,    10,
      11,    12,    13,    14,    -1,    16,    17,    18,     1,    -1,
      -1,    -1,    -1,     6,    -1,    -1,    -1,    10,    11,    12,
      13,    14,    -1,    16,    17,    18,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    -1,    37,    56,    57,    58,    -1,    -1,
      61,    62,    -1,    64,    65,    66,    67,    -1,    -1,    -1,
      53,    -1,    -1,    56,    57,    58,    -1,    -1,    61,    62,
       1,    64,    65,    66,    67,     6,    -1,    -1,     9,    10,
      11,    12,    13,    14,    -1,    16,    17,    18,     1,    -1,
      -1,    -1,    -1,     6,    -1,    -1,    -1,    10,    11,    12,
      13,    14,    -1,    16,    17,    18,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    -1,    37,    56,    57,    58,    -1,    -1,
      61,    62,    -1,    -1,    65,    -1,    67,    -1,    -1,    -1,
      53,    -1,    -1,    56,    57,    58,     1,    -1,    61,    62,
      63,     6,    65,    -1,    67,    10,    11,    12,    13,    14,
      -1,    16,    17,    18,     1,    -1,    -1,    -1,    -1,     6,
      -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,    16,
      17,    18,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,
      37,    56,    57,    58,    -1,    -1,    61,    62,    -1,    -1,
      65,    66,    67,    -1,    -1,    -1,    53,    -1,    -1,    56,
      57,    58,     1,    -1,    61,    62,    -1,     6,    65,    -1,
      67,    10,    11,    12,    13,    14,     1,    16,    17,    18,
       5,    -1,    -1,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    -1,    -1,    -1,    22,    37,    24,
      25,    -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    34,
      35,    36,    37,    -1,    53,    -1,    -1,    56,    57,    58,
      -1,    -1,    61,    62,    -1,    -1,    65,    -1,    67,    -1,
      -1,    -1,    -1,    58,     1,    -1,    -1,    -1,    63,     6,
      -1,    -1,    67,    10,    11,    12,    13,    14,    -1,    16,
      17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    -1,    -1,    47,    48,    49,    -1,    -1,    -1,
      37,    54,    55,    56,    57,    58,    59,    60,    61,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    61,    -1,    -1,    -1,    -1,    39,
      67,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    39,    -1,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    69,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    -1,    -1,    -1,    39,    66,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      -1,    -1,    -1,    39,    66,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    39,    -1,    41,    42,
      43,    44,    -1,    -1,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    39,
      -1,    41,    42,    43,    44,    -1,    -1,    47,    48,    49,
      -1,    -1,    -1,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    39,    -1,    41,    42,    43,    44,    -1,    -1,
      47,    48,    49,    -1,    -1,    -1,    53,    54,    55,    56,
      57,    58,    59,    60,    61
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    73,    74,     0,     3,    78,    10,    79,    63,     4,
      80,    81,    13,     7,    15,    75,    82,    83,    94,    63,
      84,    85,    86,    62,    76,     9,    95,   173,    63,    63,
      64,    10,    11,    13,    89,    77,   100,   101,    61,   174,
      89,    85,    40,    77,    82,    92,    93,    11,    69,    40,
     110,    11,    12,    13,    14,    16,    17,    88,   175,   176,
     177,   178,   179,     8,   173,    10,    18,    91,    67,   102,
      69,     1,     6,    10,    13,    18,    37,    53,    56,    57,
      58,    61,    62,    65,    67,    90,   111,   113,   117,   118,
     119,   121,   122,   123,   128,   134,   166,   169,   172,   179,
      87,    65,   103,     9,    65,    68,   119,   119,    11,   119,
      10,    37,    90,   129,   130,   119,   119,   173,   124,   110,
      19,    39,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    68,    13,    65,    10,    37,   117,     1,
      97,    98,    99,   111,   170,   171,   173,     1,     5,    10,
      22,    24,    25,    31,    34,    35,    36,    63,    90,    95,
     102,   104,   105,   108,   116,   135,   136,   137,   144,   145,
     148,   150,   153,   158,   163,   166,    65,   167,    10,    69,
      11,    13,    14,    16,    17,   131,   132,   133,    66,    66,
      13,   125,   126,   127,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   112,   113,   119,    10,
     168,    66,    64,    64,    89,    63,   151,    70,    65,   120,
     154,    10,    65,    11,   139,   139,   119,   138,    20,    21,
     107,   110,    63,   164,    63,    61,    68,    71,   157,    63,
      66,    97,    64,    69,    38,   157,    64,   114,    69,    97,
      13,    96,    99,   171,    65,   146,   119,   159,   120,    10,
     121,    63,    63,    26,    27,    28,    29,    30,   109,    95,
     163,   119,    10,    66,   165,   132,    13,   179,   127,    19,
     115,   165,   173,   152,    66,    67,   104,   156,   155,    66,
     111,    69,   106,   119,    89,   156,   163,   160,   156,   149,
     110,   110,   147,   157,    23,   161,    67,    70,   162,    32,
      33,   140,   141,   142,   111,   156,    11,    13,   143,    70,
      71,   140,   163,    66,    19,    70,    11
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    72,    74,    75,    73,    76,    76,    77,    77,    78,
      78,    79,    79,    80,    81,    81,    81,    82,    82,    83,
      84,    84,    86,    87,    85,    88,    88,    89,    89,    89,
      90,    90,    91,    91,    92,    92,    93,    94,    95,    95,
      96,    96,    97,    97,    98,    98,    99,   101,   100,   102,
     103,   103,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   105,   106,
     105,   105,   107,   105,   108,   109,   109,   109,   109,   109,
     110,   111,   111,   112,   112,   113,   114,   113,   115,   115,
     116,   116,   117,   117,   117,   118,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   120,   120,   121,
     121,   121,   121,   121,   121,   121,   122,   122,   124,   123,
     125,   125,   126,   126,   127,   127,   129,   128,   130,   128,
     131,   131,   132,   132,   133,   133,   133,   133,   134,   134,
     135,   136,   137,   138,   138,   139,   139,   140,   140,   141,
     142,   142,   143,   143,   143,   144,   145,   147,   146,   149,
     148,   151,   152,   150,   154,   155,   153,   156,   156,   157,
     159,   160,   158,   161,   162,   161,   163,   164,   163,   165,
     167,   166,   168,   166,   169,   169,   169,    97,   170,   170,
     170,   171,   172,   173,   173,   174,   174,   175,   175,   176,
     177,   178,   179,   179,   179,   179
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     7,     0,     1,     0,     2,     0,
       3,     0,     2,     3,     0,     3,     3,     0,     1,     2,
       1,     3,     0,     0,     5,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     6,     2,     3,     2,
       0,     1,     0,     1,     1,     3,     2,     0,     3,     4,
       0,     3,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     0,
       5,     5,     0,     4,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     0,     0,     5,     0,     2,
       2,     2,     1,     1,     1,     4,     1,     2,     2,     4,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     3,     2,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     0,     4,
       0,     1,     1,     3,     3,     3,     0,     3,     0,     4,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     3,     2,     0,     1,     0,     1,     0,     2,     2,
       3,     2,     1,     1,     3,     2,     2,     0,     7,     0,
       8,     0,     0,     5,     0,     0,     5,     1,     3,     1,
       0,     0,     6,     0,     0,     3,     0,     0,     3,     1,
       0,     5,     0,     5,     2,     1,     1,     1,     1,     3,
       1,     1,     3,     1,     2,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1
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
#line 206 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2054 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 212 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 2062 "parser.c"
    break;

  case 9: /* namespace: %empty  */
#line 222 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2070 "parser.c"
    break;

  case 10: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 225 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2079 "parser.c"
    break;

  case 13: /* require: T_REQUIRE T_STRING ';'  */
#line 233 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2088 "parser.c"
    break;

  case 17: /* scope: %empty  */
#line 242 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2094 "parser.c"
    break;

  case 18: /* scope: T_STATIC  */
#line 243 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2100 "parser.c"
    break;

  case 22: /* $@3: %empty  */
#line 253 "Parser.y"
                { _bDefiningConst = 1; }
#line 2106 "parser.c"
    break;

  case 23: /* $@4: %empty  */
#line 253 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2112 "parser.c"
    break;

  case 24: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 254 "Parser.y"
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
#line 2128 "parser.c"
    break;

  case 25: /* constant_literal: simple_literal  */
#line 267 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2136 "parser.c"
    break;

  case 26: /* constant_literal: T_STRING  */
#line 270 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2145 "parser.c"
    break;

  case 27: /* variable_identifier: T_IDENT  */
#line 276 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident; 
				yyval.ident.heapIndex = _localVariableIndex;
				yyval.ident.isLocalVar = !_bInHeader;
			}
#line 2155 "parser.c"
    break;

  case 28: /* variable_identifier: T_NUM  */
#line 282 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2166 "parser.c"
    break;

  case 29: /* variable_identifier: T_STRING  */
#line 289 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2175 "parser.c"
    break;

  case 30: /* variable_reference: T_VARIABLE  */
#line 295 "Parser.y"
                                                        {	yyval.ident = yyvsp[0].ident; }
#line 2181 "parser.c"
    break;

  case 31: /* variable_reference: T_IDENT  */
#line 296 "Parser.y"
                                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, _bInHeader ? S_VARIABLE : S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0}, 0);
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2193 "parser.c"
    break;

  case 32: /* macro_declaration: T_IDENT  */
#line 306 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2201 "parser.c"
    break;

  case 33: /* macro_declaration: T_FUNC  */
#line 309 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2211 "parser.c"
    break;

  case 34: /* macro_type: scope T_VOID  */
#line 315 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = 0;
			}
#line 2220 "parser.c"
    break;

  case 35: /* macro_type: scope type_name  */
#line 320 "Parser.y"
                                        {
				yyval.ident.scope = yyvsp[-1].ident.scope;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 2229 "parser.c"
    break;

  case 36: /* macrostart: macro_type macro_declaration '(' parameter_list ')' optional_description  */
#line 326 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2238 "parser.c"
    break;

  case 38: /* var_decl: type_name variable_identifier assignment_expression  */
#line 333 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].ident.type, 0, yyvsp[-2].ident.arraySize);
				parser_emitAssignment(&yyvsp[-1].ident);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
			}
#line 2250 "parser.c"
    break;

  case 39: /* var_decl: type_name variable_identifier  */
#line 339 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, 0, yyvsp[-1].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2261 "parser.c"
    break;

  case 41: /* optional_description: T_STRING  */
#line 348 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2270 "parser.c"
    break;

  case 46: /* par_decl: type_name variable_identifier  */
#line 362 "Parser.y"
                                              {
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_LOCAL_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam}, _localVariableIndex);
				bytecode_defineVariable(&_bytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER,yyvsp[-1].ident.type,_nparam, _localVariableIndex++);
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2281 "parser.c"
    break;

  case 47: /* $@5: %empty  */
#line 369 "Parser.y"
                  { init_macroDefinition(); }
#line 2287 "parser.c"
    break;

  case 48: /* macro_definition: $@5 macrostart block  */
#line 370 "Parser.y"
                                                 {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName(yyvsp[-1].ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",yyvsp[-1].ident.s);
				MACRO_PARAM param = {
					.mp_name = yyvsp[-1].ident.s,
					.mp_comment = _yyCurrentComment,
					.mp_numberOfLocalVariables = _localVariableIndex,
					.mp_bytecodeLength = _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start,
					.mp_buffer = _bytecodeBuffer.bb_start,
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
#line 2314 "parser.c"
    break;

  case 53: /* statement: call_expression ';'  */
#line 399 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2320 "parser.c"
    break;

  case 54: /* statement: assignment ';'  */
#line 400 "Parser.y"
                                         { yyval.needsPop = 1; }
#line 2326 "parser.c"
    break;

  case 55: /* statement: if_expression  */
#line 401 "Parser.y"
                                        { yyval.needsPop = 1; }
#line 2332 "parser.c"
    break;

  case 56: /* statement: while  */
#line 402 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2338 "parser.c"
    break;

  case 57: /* statement: foreach  */
#line 403 "Parser.y"
                                  { yyval.needsPop = 1; }
#line 2344 "parser.c"
    break;

  case 58: /* statement: switch_expression  */
#line 404 "Parser.y"
                                            { yyval.needsPop = 1; }
#line 2350 "parser.c"
    break;

  case 59: /* statement: break  */
#line 405 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2356 "parser.c"
    break;

  case 60: /* statement: continue  */
#line 406 "Parser.y"
                                   { yyval.needsPop = 0; }
#line 2362 "parser.c"
    break;

  case 61: /* statement: return_expression  */
#line 407 "Parser.y"
                                            { yyval.needsPop = 0; }
#line 2368 "parser.c"
    break;

  case 62: /* statement: goto  */
#line 408 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2374 "parser.c"
    break;

  case 63: /* statement: block  */
#line 409 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2380 "parser.c"
    break;

  case 64: /* statement: increment_expression  */
#line 410 "Parser.y"
                                               { yyval.needsPop = 1; }
#line 2386 "parser.c"
    break;

  case 65: /* statement: label  */
#line 411 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2392 "parser.c"
    break;

  case 66: /* statement: var_decl  */
#line 412 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2400 "parser.c"
    break;

  case 67: /* statement: error ';'  */
#line 415 "Parser.y"
                                    {
				yyval.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2410 "parser.c"
    break;

  case 68: /* assignment: variable_reference assignment_expression  */
#line 421 "Parser.y"
                                                     {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2418 "parser.c"
    break;

  case 69: /* $@6: %empty  */
#line 425 "Parser.y"
                                                      {
				types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2426 "parser.c"
    break;

  case 70: /* assignment: assignment_target '.' T_IDENT $@6 assignment_expression  */
#line 427 "Parser.y"
                                                {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2434 "parser.c"
    break;

  case 71: /* assignment: assignment_target '[' binary_expression ']' assignment_expression  */
#line 431 "Parser.y"
                                                                                          {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { 0 });
			}
#line 2442 "parser.c"
    break;

  case 72: /* $@7: %empty  */
#line 435 "Parser.y"
                                           {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2450 "parser.c"
    break;

  case 73: /* assignment: variable_reference $@7 shorthand_assignment_operator simple_expression  */
#line 437 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].binop});
				parser_emitAssignment(&yyvsp[-3].ident);
			}
#line 2459 "parser.c"
    break;

  case 74: /* assignment_target: variable_reference  */
#line 442 "Parser.y"
                                      {
				parser_emitPushVariable(&yyvsp[0].ident);
			}
#line 2467 "parser.c"
    break;

  case 75: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 447 "Parser.y"
                                         { yyval.binop = BIN_MUL; }
#line 2473 "parser.c"
    break;

  case 76: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 448 "Parser.y"
                                          { yyval.binop = BIN_DIV; }
#line 2479 "parser.c"
    break;

  case 77: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 449 "Parser.y"
                                          { yyval.binop = BIN_MOD; }
#line 2485 "parser.c"
    break;

  case 78: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 450 "Parser.y"
                                           { yyval.binop = BIN_ADD; }
#line 2491 "parser.c"
    break;

  case 79: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 451 "Parser.y"
                                            { yyval.binop = BIN_SUB; }
#line 2497 "parser.c"
    break;

  case 80: /* assignment_expression: T_ASSIGN simple_expression  */
#line 453 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2503 "parser.c"
    break;

  case 81: /* simple_expression: binary_expression  */
#line 455 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2509 "parser.c"
    break;

  case 86: /* $@8: %empty  */
#line 463 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2515 "parser.c"
    break;

  case 89: /* range_increment: T_DOTDOT binary_expression  */
#line 466 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2521 "parser.c"
    break;

  case 90: /* increment_expression: variable_reference T_MINUSMINUS  */
#line 469 "Parser.y"
                                                         { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, -1); }
#line 2527 "parser.c"
    break;

  case 91: /* increment_expression: variable_reference T_PLUSPLUS  */
#line 471 "Parser.y"
                                                      { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, yyvsp[-1].ident.heapIndex, yyvsp[-1].ident.isLocalVar, 1); }
#line 2533 "parser.c"
    break;

  case 92: /* s_bterm: value  */
#line 473 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2539 "parser.c"
    break;

  case 95: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 477 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].ident.type});
			}
#line 2547 "parser.c"
    break;

  case 96: /* binary_expression: s_bterm  */
#line 482 "Parser.y"
                                { yyval.ident.type = yyvsp[0].ident.type;}
#line 2553 "parser.c"
    break;

  case 97: /* binary_expression: type_cast s_bterm  */
#line 483 "Parser.y"
                                                {
				yyval.ident.type = yyvsp[-1].ident.type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, yyval.binop);
			}
#line 2562 "parser.c"
    break;

  case 98: /* binary_expression: '-' T_NUM  */
#line 487 "Parser.y"
                                    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-yyvsp[0].num);
				} else {
					yyval.ident.type = VT_NUMBER;
					yyval.v.type = C_PUSH_LONG_LITERAL; 
					yyval.v.data.longValue  = -yyvsp[0].num;
				}
			}
#line 2576 "parser.c"
    break;

  case 99: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 496 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2582 "parser.c"
    break;

  case 100: /* binary_expression: '!' binary_expression  */
#line 497 "Parser.y"
                                                { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2588 "parser.c"
    break;

  case 101: /* binary_expression: '~' binary_expression  */
#line 498 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2594 "parser.c"
    break;

  case 102: /* binary_expression: '+' binary_expression  */
#line 499 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2600 "parser.c"
    break;

  case 103: /* binary_expression: '-' binary_expression  */
#line 500 "Parser.y"
                                                { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &yyvsp[0].v, -1); }
#line 2606 "parser.c"
    break;

  case 104: /* binary_expression: binary_expression '~' binary_expression  */
#line 501 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2612 "parser.c"
    break;

  case 105: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 502 "Parser.y"
                                                                         { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2618 "parser.c"
    break;

  case 106: /* binary_expression: binary_expression '&' binary_expression  */
#line 503 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2624 "parser.c"
    break;

  case 107: /* binary_expression: binary_expression '|' binary_expression  */
#line 504 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2630 "parser.c"
    break;

  case 108: /* binary_expression: binary_expression '+' binary_expression  */
#line 505 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2636 "parser.c"
    break;

  case 109: /* binary_expression: binary_expression '^' binary_expression  */
#line 506 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2642 "parser.c"
    break;

  case 110: /* binary_expression: binary_expression '-' binary_expression  */
#line 507 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2648 "parser.c"
    break;

  case 111: /* binary_expression: binary_expression '*' binary_expression  */
#line 508 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2654 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression '/' binary_expression  */
#line 509 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2660 "parser.c"
    break;

  case 113: /* binary_expression: binary_expression '%' binary_expression  */
#line 510 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2666 "parser.c"
    break;

  case 114: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 511 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2672 "parser.c"
    break;

  case 115: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 512 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2678 "parser.c"
    break;

  case 116: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 513 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2684 "parser.c"
    break;

  case 117: /* binary_expression: binary_expression T_AND binary_expression  */
#line 514 "Parser.y"
                                                                    { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2690 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression T_OR binary_expression  */
#line 515 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2696 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression '<' binary_expression  */
#line 516 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2702 "parser.c"
    break;

  case 120: /* binary_expression: binary_expression '>' binary_expression  */
#line 517 "Parser.y"
                                                                  { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2708 "parser.c"
    break;

  case 121: /* binary_expression: binary_expression T_LE binary_expression  */
#line 518 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2714 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression T_GE binary_expression  */
#line 519 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2720 "parser.c"
    break;

  case 123: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 520 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2726 "parser.c"
    break;

  case 124: /* binary_expression: binary_expression T_NE binary_expression  */
#line 521 "Parser.y"
                                                                   { yyval.ident.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2732 "parser.c"
    break;

  case 125: /* binary_expression: '(' binary_expression ')'  */
#line 522 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2738 "parser.c"
    break;

  case 126: /* binary_expression: variable_reference assignment_expression  */
#line 523 "Parser.y"
                                                                   {
				parser_emitAssignment(&yyvsp[-1].ident);
			}
#line 2746 "parser.c"
    break;

  case 128: /* condition: '(' binary_expression  */
#line 530 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2754 "parser.c"
    break;

  case 129: /* value: T_VARIABLE  */
#line 534 "Parser.y"
                           {
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = yyvsp[0].ident.type;
			}
#line 2763 "parser.c"
    break;

  case 131: /* value: string  */
#line 539 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2773 "parser.c"
    break;

  case 132: /* value: simple_literal  */
#line 544 "Parser.y"
                                         {	yyval.ident.type = yyvsp[0].ident.type;	}
#line 2779 "parser.c"
    break;

  case 133: /* value: map_literal  */
#line 545 "Parser.y"
                                      {
				yyval.ident.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2790 "parser.c"
    break;

  case 134: /* value: array_literal  */
#line 551 "Parser.y"
                                        {
				yyval.ident.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2801 "parser.c"
    break;

  case 135: /* value: T_IDENT  */
#line 557 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				parser_emitPushVariable(&yyvsp[0].ident);
				yyval.ident.type = VT_STRING;  
			}
#line 2811 "parser.c"
    break;

  case 136: /* struct_reference: T_VARIABLE '.' T_IDENT  */
#line 563 "Parser.y"
                                         {
				parser_emitPushVariable(&yyvsp[-2].ident);
				yyval.ident.type = types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2821 "parser.c"
    break;

  case 137: /* struct_reference: struct_reference '.' T_IDENT  */
#line 568 "Parser.y"
                                                       {
				yyval.ident.type = types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2830 "parser.c"
    break;

  case 138: /* $@9: %empty  */
#line 573 "Parser.y"
                 {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2838 "parser.c"
    break;

  case 144: /* map_associate: T_STRING T_ASSOC_ARROW simple_literal  */
#line 587 "Parser.y"
                                                              { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2847 "parser.c"
    break;

  case 145: /* map_associate: T_STRING T_ASSOC_ARROW T_STRING  */
#line 591 "Parser.y"
                                                          { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2856 "parser.c"
    break;

  case 146: /* $@10: %empty  */
#line 596 "Parser.y"
                   {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2864 "parser.c"
    break;

  case 148: /* $@11: %empty  */
#line 600 "Parser.y"
                              {
				_currentArrayLiteral = arraylist_create(3);
			}
#line 2872 "parser.c"
    break;

  case 152: /* array_element: simple_array_element  */
#line 609 "Parser.y"
                                             {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2880 "parser.c"
    break;

  case 153: /* array_element: T_STRING  */
#line 613 "Parser.y"
                                 {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2888 "parser.c"
    break;

  case 154: /* simple_array_element: T_NUM  */
#line 618 "Parser.y"
                                                { yyval.v.type = VT_NUMBER; yyval.v.data.longValue = yyvsp[0].num; }
#line 2894 "parser.c"
    break;

  case 155: /* simple_array_element: T_CHARACTER  */
#line 619 "Parser.y"
                                        { yyval.v.type = VT_CHAR; yyval.v.data.longValue = yyvsp[0].num; }
#line 2900 "parser.c"
    break;

  case 156: /* simple_array_element: T_TRUE  */
#line 620 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 1; }
#line 2906 "parser.c"
    break;

  case 157: /* simple_array_element: T_FALSE  */
#line 621 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 0; }
#line 2912 "parser.c"
    break;

  case 158: /* string: T_STRING  */
#line 623 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2918 "parser.c"
    break;

  case 159: /* string: string T_STRING  */
#line 624 "Parser.y"
                                          { 
				if ((yyval.ident.s = malloc(strlen(yyvsp[-1].ident.s)+strlen(yyvsp[0].ident.s)+1)) != 0) {
					strcat(strcpy(yyval.ident.s,yyvsp[-1].ident.s),yyvsp[0].ident.s);
					free(yyvsp[-1].ident.s);
					free(yyvsp[0].ident.s);
				} else {
					yyerror("buffer overlow");
					yyval.ident = yyvsp[0].ident;
				}
			}
#line 2933 "parser.c"
    break;

  case 160: /* break: T_BREAK opt_num ';'  */
#line 636 "Parser.y"
                        {	bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2939 "parser.c"
    break;

  case 161: /* continue: T_CONTINUE opt_num ';'  */
#line 639 "Parser.y"
                        {	bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2945 "parser.c"
    break;

  case 162: /* return_expression: T_RETURN optional_bterm  */
#line 642 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 2953 "parser.c"
    break;

  case 165: /* opt_num: %empty  */
#line 649 "Parser.y"
                {	yyval.num = 1;	}
#line 2959 "parser.c"
    break;

  case 166: /* opt_num: T_NUM  */
#line 650 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 2965 "parser.c"
    break;

  case 171: /* case_selector: T_DEFAULT ':'  */
#line 659 "Parser.y"
                                      {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 2973 "parser.c"
    break;

  case 172: /* case_condition: T_NUM  */
#line 664 "Parser.y"
                              {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].num});
			}
#line 2981 "parser.c"
    break;

  case 173: /* case_condition: T_STRING  */
#line 667 "Parser.y"
                                   {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 2989 "parser.c"
    break;

  case 174: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 670 "Parser.y"
                                               {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 2997 "parser.c"
    break;

  case 175: /* label: T_IDENT ':'  */
#line 674 "Parser.y"
                            {
				bytecode_createBranchLabel(&_bytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 3006 "parser.c"
    break;

  case 176: /* goto: T_GOTO T_IDENT  */
#line 679 "Parser.y"
                               {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 3015 "parser.c"
    break;

  case 177: /* $@12: %empty  */
#line 684 "Parser.y"
                                                  {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.intValue=_localVariableIndex});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].ident.type, (intptr_t)0, yyvsp[-2].ident.arraySize);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 3028 "parser.c"
    break;

  case 178: /* in_clause: '(' type_name variable_identifier $@12 ':' simple_expression ')'  */
#line 691 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}
#line 3039 "parser.c"
    break;

  case 179: /* $@13: %empty  */
#line 700 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3049 "parser.c"
    break;

  case 180: /* switch_expression: T_SWITCH '(' value ')' $@13 '{' case_clauses '}'  */
#line 706 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(&_bytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3062 "parser.c"
    break;

  case 181: /* $@14: %empty  */
#line 715 "Parser.y"
                      {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lstartid,_breaklevel);
			}
#line 3070 "parser.c"
    break;

  case 182: /* $@15: %empty  */
#line 717 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3079 "parser.c"
    break;

  case 183: /* foreach: T_FOR $@14 in_clause $@15 local_block  */
#line 720 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3091 "parser.c"
    break;

  case 184: /* $@16: %empty  */
#line 728 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lstartid,_breaklevel);
			}
#line 3099 "parser.c"
    break;

  case 185: /* $@17: %empty  */
#line 730 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3108 "parser.c"
    break;

  case 186: /* while: T_WHILE $@16 condition $@17 local_block  */
#line 733 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3120 "parser.c"
    break;

  case 190: /* $@18: %empty  */
#line 746 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3129 "parser.c"
    break;

  case 191: /* $@19: %empty  */
#line 749 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3137 "parser.c"
    break;

  case 192: /* if_expression: T_IF condition $@18 local_block $@19 else_clause  */
#line 751 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3146 "parser.c"
    break;

  case 193: /* else_clause: %empty  */
#line 756 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3155 "parser.c"
    break;

  case 194: /* $@20: %empty  */
#line 760 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3165 "parser.c"
    break;

  case 196: /* stmntlist: %empty  */
#line 767 "Parser.y"
                { yyval.needsPop = 0; }
#line 3171 "parser.c"
    break;

  case 197: /* $@21: %empty  */
#line 768 "Parser.y"
                                    { 
				if (yyvsp[0].needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			}
#line 3181 "parser.c"
    break;

  case 200: /* @22: %empty  */
#line 776 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3193 "parser.c"
    break;

  case 201: /* call_expression: T_FUNC '(' @22 parameter_list closing_paren  */
#line 782 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3203 "parser.c"
    break;

  case 202: /* @23: %empty  */
#line 787 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3214 "parser.c"
    break;

  case 203: /* call_expression: function_id_or_pointer '(' @23 parameter_list closing_paren  */
#line 792 "Parser.y"
                                                       {
				if (yyvsp[-4].ident.operation == C_MACRO_REF_LOCAL) {
					_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.intValue = yyvsp[-4].ident.heapIndex}, *_currentFunctionCallParamIndexP);
				} else {
					_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
					freeitem(&yyvsp[-4].ident.s);
				}
				_currentFunctionCallParamIndexP--;
			}
#line 3228 "parser.c"
    break;

  case 204: /* function_id_or_pointer: '*' variable_reference  */
#line 803 "Parser.y"
                                               {
				yyval.ident.operation =  yyvsp[0].ident.isLocalVar ? C_MACRO_REF_LOCAL : C_MACRO_REF;
				if (yyvsp[0].ident.isLocalVar) {
					yyval.ident.heapIndex = yyvsp[0].ident.heapIndex;
				} else {
					yyval.ident.s = _strdup(yyvsp[0].ident.s);
					yyval.ident.stringIsAlloced = 1;
				}
			}
#line 3242 "parser.c"
    break;

  case 205: /* function_id_or_pointer: T_IDENT  */
#line 813 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3251 "parser.c"
    break;

  case 206: /* function_id_or_pointer: error  */
#line 817 "Parser.y"
                                {
				yyerror("Illegal function call expression");
				yyerrok;
				yyval.ident.s = 0;
			}
#line 3261 "parser.c"
    break;

  case 210: /* nonempty_parameters: error  */
#line 827 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3269 "parser.c"
    break;

  case 211: /* parameter: simple_expression  */
#line 831 "Parser.y"
                                  { yyval.ident.type = yyvsp[0].ident.type; (*_currentFunctionCallParamIndexP)++; }
#line 3275 "parser.c"
    break;

  case 212: /* type_cast: '(' type_name ')'  */
#line 833 "Parser.y"
                                        {	
				yyval.ident.type = yyvsp[-1].ident.type;
				yyval.ident.arraySize = yyvsp[-1].ident.arraySize;
			}
#line 3284 "parser.c"
    break;

  case 213: /* type_name: T_TYPE_IDENTIFIER  */
#line 838 "Parser.y"
                               {
				yyval.ident.arraySize = 0;
			}
#line 3292 "parser.c"
    break;

  case 214: /* type_name: T_TYPE_IDENTIFIER array_size  */
#line 841 "Parser.y"
                                                        {	
				yyval.ident.type = VT_OBJECT_ARRAY;
				yyval.ident.arraySize = yyvsp[0].ident.arraySize;
			}
#line 3301 "parser.c"
    break;

  case 216: /* array_size: '[' T_NUM ']'  */
#line 847 "Parser.y"
                                        {
				yyval.ident.arraySize = (int)yyvsp[-1].num;
			}
#line 3309 "parser.c"
    break;

  case 217: /* boolean_literal: T_TRUE  */
#line 851 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3318 "parser.c"
    break;

  case 218: /* boolean_literal: T_FALSE  */
#line 854 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3327 "parser.c"
    break;

  case 219: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 859 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3336 "parser.c"
    break;

  case 220: /* integer_literal: T_NUM  */
#line 864 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3345 "parser.c"
    break;

  case 221: /* character_literal: T_CHARACTER  */
#line 869 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3354 "parser.c"
    break;

  case 222: /* simple_literal: integer_literal  */
#line 874 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_NUMBER;
			}
#line 3367 "parser.c"
    break;

  case 223: /* simple_literal: float_literal  */
#line 882 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_FLOAT;
			}
#line 3380 "parser.c"
    break;

  case 224: /* simple_literal: character_literal  */
#line 890 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_CHAR;
			}
#line 3393 "parser.c"
    break;

  case 225: /* simple_literal: boolean_literal  */
#line 898 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.ident.type = VT_BOOLEAN;
			}
#line 3406 "parser.c"
    break;


#line 3410 "parser.c"

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

#line 906 "Parser.y"


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

