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


#line 225 "parser.c"

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
  YYSYMBOL_shorthand_assignment_operator = 108, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 109,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 110,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 111, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 112,         /* range_expression  */
  YYSYMBOL_113_8 = 113,                    /* $@8  */
  YYSYMBOL_range_increment = 114,          /* range_increment  */
  YYSYMBOL_increment_expression = 115,     /* increment_expression  */
  YYSYMBOL_s_bterm = 116,                  /* s_bterm  */
  YYSYMBOL_constructor_expression = 117,   /* constructor_expression  */
  YYSYMBOL_binary_expression = 118,        /* binary_expression  */
  YYSYMBOL_condition = 119,                /* condition  */
  YYSYMBOL_value = 120,                    /* value  */
  YYSYMBOL_struct_reference = 121,         /* struct_reference  */
  YYSYMBOL_map_literal = 122,              /* map_literal  */
  YYSYMBOL_123_9 = 123,                    /* $@9  */
  YYSYMBOL_optional_map_associates = 124,  /* optional_map_associates  */
  YYSYMBOL_map_associates = 125,           /* map_associates  */
  YYSYMBOL_map_associate = 126,            /* map_associate  */
  YYSYMBOL_array_literal = 127,            /* array_literal  */
  YYSYMBOL_128_10 = 128,                   /* $@10  */
  YYSYMBOL_129_11 = 129,                   /* $@11  */
  YYSYMBOL_array_elements = 130,           /* array_elements  */
  YYSYMBOL_array_element = 131,            /* array_element  */
  YYSYMBOL_simple_array_element = 132,     /* simple_array_element  */
  YYSYMBOL_string = 133,                   /* string  */
  YYSYMBOL_break = 134,                    /* break  */
  YYSYMBOL_continue = 135,                 /* continue  */
  YYSYMBOL_return_expression = 136,        /* return_expression  */
  YYSYMBOL_optional_bterm = 137,           /* optional_bterm  */
  YYSYMBOL_opt_num = 138,                  /* opt_num  */
  YYSYMBOL_case_clauses = 139,             /* case_clauses  */
  YYSYMBOL_case_clause = 140,              /* case_clause  */
  YYSYMBOL_case_selector = 141,            /* case_selector  */
  YYSYMBOL_case_condition = 142,           /* case_condition  */
  YYSYMBOL_label = 143,                    /* label  */
  YYSYMBOL_goto = 144,                     /* goto  */
  YYSYMBOL_in_clause = 145,                /* in_clause  */
  YYSYMBOL_146_12 = 146,                   /* $@12  */
  YYSYMBOL_switch_expression = 147,        /* switch_expression  */
  YYSYMBOL_148_13 = 148,                   /* $@13  */
  YYSYMBOL_foreach = 149,                  /* foreach  */
  YYSYMBOL_150_14 = 150,                   /* $@14  */
  YYSYMBOL_151_15 = 151,                   /* $@15  */
  YYSYMBOL_while = 152,                    /* while  */
  YYSYMBOL_153_16 = 153,                   /* $@16  */
  YYSYMBOL_154_17 = 154,                   /* $@17  */
  YYSYMBOL_local_block = 155,              /* local_block  */
  YYSYMBOL_closing_brace = 156,            /* closing_brace  */
  YYSYMBOL_if_expression = 157,            /* if_expression  */
  YYSYMBOL_158_18 = 158,                   /* $@18  */
  YYSYMBOL_159_19 = 159,                   /* $@19  */
  YYSYMBOL_else_clause = 160,              /* else_clause  */
  YYSYMBOL_161_20 = 161,                   /* $@20  */
  YYSYMBOL_stmntlist = 162,                /* stmntlist  */
  YYSYMBOL_163_21 = 163,                   /* $@21  */
  YYSYMBOL_closing_paren = 164,            /* closing_paren  */
  YYSYMBOL_call_expression = 165,          /* call_expression  */
  YYSYMBOL_166_22 = 166,                   /* @22  */
  YYSYMBOL_167_23 = 167,                   /* @23  */
  YYSYMBOL_function_id_or_pointer = 168,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 169,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 170,                /* parameter  */
  YYSYMBOL_type_cast = 171,                /* type_cast  */
  YYSYMBOL_type_name = 172,                /* type_name  */
  YYSYMBOL_array_dimension = 173,          /* array_dimension  */
  YYSYMBOL_boolean_literal = 174,          /* boolean_literal  */
  YYSYMBOL_float_literal = 175,            /* float_literal  */
  YYSYMBOL_integer_literal = 176,          /* integer_literal  */
  YYSYMBOL_character_literal = 177,        /* character_literal  */
  YYSYMBOL_simple_literal = 178            /* simple_literal  */
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
#define YYLAST   1061

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  107
/* YYNRULES -- Number of rules.  */
#define YYNRULES  224
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  346

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
       0,   185,   185,   191,   185,   195,   196,   198,   199,   201,
     204,   209,   210,   212,   217,   218,   219,   221,   222,   226,
     229,   230,   232,   232,   232,   246,   249,   255,   256,   263,
     270,   271,   281,   284,   290,   294,   298,   304,   306,   312,
     319,   321,   326,   327,   330,   332,   335,   342,   342,   364,
     366,   367,   369,   370,   371,   372,   373,   374,   375,   376,
     377,   378,   379,   380,   381,   382,   383,   386,   392,   399,
     399,   405,   409,   409,   417,   418,   419,   420,   421,   423,
     425,   426,   429,   430,   432,   433,   433,   435,   436,   439,
     441,   443,   444,   445,   447,   452,   453,   457,   466,   467,
     468,   469,   470,   471,   472,   473,   474,   475,   476,   477,
     478,   479,   480,   481,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   499,   500,   504,   508,
     509,   514,   515,   521,   527,   533,   538,   543,   543,   547,
     549,   552,   554,   557,   561,   566,   566,   570,   570,   575,
     576,   579,   583,   588,   589,   590,   591,   593,   594,   605,
     608,   612,   616,   617,   619,   620,   622,   623,   625,   627,
     629,   634,   637,   640,   644,   649,   654,   654,   670,   669,
     685,   687,   685,   698,   700,   698,   711,   711,   713,   716,
     719,   716,   726,   730,   730,   737,   738,   738,   744,   746,
     746,   757,   757,   769,   775,   779,   785,   787,   788,   789,
     793,   795,   797,   798,   800,   801,   805,   808,   813,   818,
     823,   828,   836,   844,   852
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
  "shorthand_assignment_operator", "assignment_expression",
  "simple_expression", "binary_expression_or_range", "range_expression",
  "$@8", "range_increment", "increment_expression", "s_bterm",
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
  "type_cast", "type_name", "array_dimension", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-289)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-206)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -289,    20,    29,  -289,    33,  -289,   -10,    89,  -289,    82,
    -289,    10,    40,  -289,  -289,    34,    96,    44,    48,  -289,
      42,  -289,    47,  -289,   118,    62,  -289,    47,  -289,  -289,
    -289,  -289,  -289,  -289,    84,  -289,   118,   122,    19,  -289,
      91,  -289,   132,  -289,    43,    24,    65,    64,  -289,   247,
    -289,  -289,  -289,  -289,  -289,  -289,  -289,  -289,  -289,  -289,
    -289,  -289,  -289,  -289,  -289,  -289,  -289,    76,  -289,  -289,
    -289,  -289,   133,   -27,  -289,    92,   -28,   670,   670,   728,
       8,    93,   670,   576,  -289,    91,  -289,  -289,  -289,  -289,
     827,  -289,    90,  -289,  -289,   150,  -289,   102,   800,  -289,
    -289,   496,   358,   104,  -289,   161,   784,   -12,  -289,   -12,
    -289,  -289,  -289,   106,   183,   931,   879,   107,   159,  -289,
     670,   670,   670,   670,   670,   670,   670,   670,   670,   670,
     670,   670,   670,   670,   670,   670,   670,   670,   670,   670,
     670,   670,   434,   168,  -289,  -289,   114,   112,  -289,   121,
     123,   127,  -289,  -289,   131,  -289,    47,   138,  -289,   -37,
     137,  -289,   194,   141,   196,   196,   103,  -289,    -5,   146,
    -289,  -289,   147,  -289,  -289,  -289,  -289,  -289,  -289,  -289,
    -289,  -289,  -289,   145,   156,   154,   496,  -289,  -289,  -289,
    -289,  -289,  -289,  -289,   -14,  -289,  -289,  -289,  -289,   184,
     145,   157,  -289,   931,   784,   977,   977,    41,    41,   954,
     954,   418,   418,   162,  1000,  1000,  1000,   784,   418,   418,
     -12,   -12,   162,   162,   162,   158,  -289,   827,  -289,   496,
     219,    96,   514,  -289,  -289,   170,  -289,   670,  -289,   137,
    -289,   201,  -289,   173,   174,   931,  -289,  -289,  -289,   670,
     229,   124,  -289,  -289,   314,  -289,  -289,  -289,  -289,  -289,
     175,   183,  -289,   171,  -289,   159,   224,  -289,   175,  -289,
    -289,  -289,  -289,    96,  -289,   905,   742,  -289,  -289,   178,
    -289,  -289,   850,  -289,  -289,  -289,  -289,  -289,  -289,   594,
    -289,  -289,  -289,  -289,  -289,  -289,  -289,  -289,   670,  -289,
    -289,    47,   742,  -289,   402,  -289,  -289,   742,  -289,    91,
      91,  -289,  -289,  -289,  -289,   145,   222,  -289,   179,  -289,
    -289,   177,  -289,  -289,  -289,    59,   652,   742,    18,   180,
     202,    59,   314,   185,  -289,   236,  -289,   197,  -289,  -289,
    -289,  -289,  -289,   263,  -289,  -289
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     9,     1,     0,    11,     0,    14,    10,     0,
      12,     3,     0,    18,    22,     5,     0,     0,     0,    13,
      19,    20,     0,     6,    47,   212,    37,     0,    15,    16,
      22,    27,    28,    29,     0,     4,    47,    17,     0,   213,
      39,    21,     0,     8,     0,     0,     0,     0,   214,     0,
      38,   219,   218,    26,   220,   216,   217,    23,   224,   222,
     221,   223,    25,    34,    35,    32,    33,     0,    50,    48,
     215,   205,     0,   134,   157,     0,   128,     0,     0,     0,
       0,   147,     0,     0,   137,     0,    79,    81,    95,    93,
      80,    91,   129,   132,   133,   130,    92,     0,     0,   131,
      24,     0,     0,     0,   199,     0,   100,   101,    97,   102,
      31,    30,   203,     0,     0,    99,     0,     0,   139,   125,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   158,   201,   134,   128,    96,   209,
       0,    43,    44,   210,   206,   207,     0,   205,   180,    31,
       0,   183,     0,     0,   164,   164,     0,    52,    72,    66,
      63,   196,     0,    64,    59,    60,    61,    65,    62,    58,
      57,    56,    55,     0,     0,     0,     0,   135,   146,   153,
     152,   154,   155,   156,     0,   150,   151,   124,   211,     0,
       0,   140,   141,    85,   104,   123,   122,   113,   114,   117,
     116,   120,   121,   115,   105,   106,   108,   103,   118,   119,
     107,   109,   110,   111,   112,     0,    83,    82,   136,     0,
      40,     0,     0,    46,    67,     0,   174,     0,   189,     0,
     175,     0,   165,     0,     0,   163,   161,    90,    89,     0,
       0,     0,    68,    51,     0,    54,   188,    49,    53,    94,
       0,     0,   148,     0,   138,     0,    87,    98,     0,    41,
      36,    45,   208,     0,   181,   127,     0,   184,   134,     0,
     159,   160,     0,    69,    77,    78,    74,    75,    76,     0,
      66,   197,   198,   200,   149,   144,   143,   142,     0,    86,
     202,     0,     0,   126,     0,   186,   190,     0,   178,     0,
       0,    73,    88,   176,   182,     0,   192,   185,     0,    71,
      70,     0,   187,   193,   191,   166,     0,     0,     0,     0,
       0,   166,     0,     0,   194,   171,   172,     0,   170,   179,
     167,   168,   177,     0,   169,   173
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -289,  -289,  -289,  -289,  -289,   240,  -289,  -289,  -289,  -289,
     248,  -289,  -289,   257,  -289,  -289,  -289,   -26,   -78,  -289,
    -289,  -289,  -289,    -8,  -289,  -175,  -289,    86,  -289,  -289,
     274,  -289,  -266,  -289,  -289,  -289,  -289,   -79,   -49,  -289,
     186,  -289,  -289,  -289,   223,  -289,   -56,    83,    85,  -289,
    -289,  -289,  -289,  -289,    60,  -289,  -289,  -289,  -289,    66,
    -289,  -289,  -289,  -289,  -289,  -289,   164,    -1,  -289,  -289,
    -289,  -289,  -289,  -289,  -289,  -289,  -289,  -289,  -289,  -289,
    -289,  -289,  -289,  -288,  -193,  -289,  -289,  -289,  -289,  -289,
    -245,  -289,    63,   -99,  -289,  -289,  -289,  -289,   101,  -289,
     -39,  -289,  -289,  -289,  -289,  -289,   -38
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    15,    24,    35,     5,     7,    10,    11,
      16,    17,    20,    21,    22,   100,    57,    34,    85,    67,
      45,    46,    18,   290,   270,   150,   151,   152,    36,    37,
     170,   102,   171,   172,   310,   251,   289,    50,   153,   225,
      87,   266,   299,   173,    88,    89,    90,   238,    91,    92,
      93,   118,   200,   201,   202,    94,   113,   114,   194,   195,
     196,    95,   174,   175,   176,   246,   243,   330,   331,   332,
     337,   177,   178,   274,   321,   179,   318,   180,   235,   302,
     181,   239,   307,   306,   257,   182,   276,   316,   324,   327,
     183,   254,   293,    96,   186,   229,    97,   154,   155,    98,
      27,    39,    58,    59,    60,    61,    99
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      86,    40,   112,   184,    62,    64,   119,   264,    26,   291,
     305,   260,   -30,   -31,   314,   247,   248,    13,   110,   317,
       3,   106,   107,   109,   168,    14,   115,   116,  -204,   335,
      47,   336,     4,   236,    65,    49,   305,   130,  -204,   334,
     105,   305,    66,     6,   117,   111,   139,   140,   141,   142,
     261,    63,    25,     8,   268,   262,   249,    31,    32,   315,
      33,   305,   156,   250,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   227,   341,    48,   252,
     130,   328,   329,     9,   169,    12,    23,   137,   138,   139,
     140,   141,   142,    19,    71,    25,    30,    28,  -162,    72,
     245,    29,  -162,    73,    51,    52,    74,    54,    -7,    55,
      56,    75,   322,    38,    42,  -162,  -162,  -162,  -162,    13,
     233,    49,    68,    70,  -162,  -162,  -162,  -162,  -162,  -162,
      76,   101,   103,    51,    52,    53,    54,   156,    55,    56,
     284,   285,   286,   287,   288,   184,    77,   104,   143,    78,
      79,    80,  -145,   144,    81,    82,  -162,   145,    83,   185,
      84,   187,   199,   198,  -162,   188,   168,   184,   228,  -204,
     105,   275,    51,    52,   295,    54,  -205,    55,    56,   230,
     156,   231,   156,   282,   189,   232,   190,   191,   168,   192,
     193,   234,   237,   184,   240,   184,   241,   242,   184,   253,
     255,   278,    51,    52,    74,    54,   256,    55,    56,   258,
     259,   265,   263,   142,   168,   296,   168,   267,   184,   168,
     319,   320,   269,   184,   301,   273,   280,   281,   147,   283,
     311,   292,   312,   298,   308,   323,   325,   326,    71,   168,
     338,   342,   -84,    72,   168,   343,   -84,    73,    51,    52,
      74,    54,    81,    55,    56,    75,   -84,   344,    84,   -84,
     -84,   -84,   -84,   339,   345,   313,    43,   333,   -84,   -84,
     -84,   -84,   -84,   -84,    76,    44,   -84,    41,   -84,   -84,
     -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,   -84,
      77,   -84,   -84,    78,    79,    80,   -84,   -84,    81,    82,
     -84,   -84,    83,   -84,    84,   157,   -84,   271,   -84,   158,
      69,   148,   277,    25,   159,   297,   279,   294,   226,   244,
     340,   300,    75,   272,     0,     0,   160,     0,   161,   162,
       0,     0,     0,     0,     0,   163,  -195,  -195,   164,   165,
     166,   111,     0,     0,     0,     0,     0,     0,     0,   157,
       0,     0,     0,   158,     0,     0,     0,    25,   159,     0,
       0,     0,    80,     0,     0,     0,    75,   167,     0,     0,
     160,    68,   161,   162,     0,  -195,     0,     0,     0,   163,
       0,     0,   164,   165,   166,   111,     0,     0,     0,     0,
       0,     0,     0,   157,     0,     0,     0,   158,     0,     0,
       0,    25,   159,     0,     0,     0,    80,     0,     0,     0,
      75,   167,     0,     0,   160,    68,   161,   162,     0,  -195,
       0,     0,     0,   163,     0,    71,   164,   165,   166,   111,
      72,     0,     0,     0,    73,    51,    52,    74,    54,     0,
      55,    56,    75,     0,     0,     0,     0,     0,     0,     0,
      80,   124,   125,     0,     0,   167,     0,   130,     0,    68,
       0,    76,     0,   -50,   137,   138,   139,   140,   141,   142,
       0,     0,     0,     0,     0,     0,     0,    77,     0,     0,
      78,    79,    80,     0,     0,    81,    82,   149,     0,    83,
       0,    84,    72,   -84,     0,    25,    73,    51,    52,    74,
      54,     0,    55,    56,    75,    71,     0,     0,     0,     0,
      72,     0,     0,     0,    73,    51,    52,    74,    54,     0,
      55,    56,    75,    76,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    77,
       0,    76,    78,    79,    80,     0,     0,    81,    82,     0,
     -84,    83,   -42,    84,     0,     0,     0,    77,     0,     0,
      78,    79,    80,     0,     0,    81,    82,    71,   -84,    83,
     -84,    84,    72,     0,     0,    25,    73,    51,    52,    74,
      54,     0,    55,    56,    75,    71,     0,     0,     0,     0,
      72,     0,     0,     0,    73,    51,    52,    74,    54,     0,
      55,    56,    75,    76,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    77,
       0,    76,    78,    79,    80,     0,     0,    81,    82,     0,
       0,    83,     0,    84,     0,     0,     0,    77,     0,     0,
      78,    79,    80,    71,     0,    81,    82,   -84,    72,    83,
       0,    84,    73,    51,    52,    74,    54,     0,    55,    56,
      75,    71,     0,     0,     0,     0,    72,     0,     0,     0,
      73,    51,    52,    74,    54,     0,    55,    56,    75,    76,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    77,     0,    76,    78,    79,
      80,     0,     0,    81,    82,     0,     0,    83,   -84,    84,
       0,     0,     0,    77,     0,     0,    78,    79,    80,    71,
       0,    81,    82,     0,    72,    83,     0,    84,    73,   108,
      52,    74,    54,   157,    55,    56,    75,   158,     0,     0,
       0,    25,   159,     0,     0,     0,     0,     0,     0,     0,
      75,     0,     0,     0,   160,    76,   161,   162,     0,     0,
       0,     0,     0,   163,     0,     0,   164,   165,   166,   111,
       0,    77,     0,     0,    78,    79,    80,     0,     0,    81,
      82,     0,     0,    83,     0,    84,     0,     0,     0,     0,
      80,    71,     0,     0,     0,   167,    72,     0,     0,   304,
     146,    51,    52,    74,    54,     0,    55,    56,    75,     0,
       0,     0,     0,     0,     0,     0,     0,   124,   125,     0,
       0,   128,   129,   130,     0,     0,     0,   147,   135,   136,
     137,   138,   139,   140,   141,   142,   120,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    80,     0,
       0,    81,     0,     0,     0,     0,   121,    84,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   121,
       0,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,     0,     0,     0,     0,     0,     0,   121,   309,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,     0,     0,     0,   121,   197,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,     0,     0,     0,
     121,   303,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   121,     0,   122,   123,   124,   125,     0,
       0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   121,     0,  -206,  -206,
     124,   125,     0,     0,   128,   129,   130,     0,     0,     0,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   121,
       0,   122,   123,   124,   125,     0,     0,   128,   129,   130,
       0,     0,     0,   134,   135,   136,   137,   138,   139,   140,
     141,   142
};

static const yytype_int16 yycheck[] =
{
      49,    27,    80,   102,    42,    44,    85,   200,    16,   254,
     276,   186,    40,    40,   302,    20,    21,     7,    10,   307,
       0,    77,    78,    79,   102,    15,    82,    83,    65,    11,
      11,    13,     3,    70,    10,    40,   302,    49,    65,   327,
      68,   307,    18,    10,    83,    37,    58,    59,    60,    61,
      64,     8,     9,    63,   229,    69,    61,    10,    11,   304,
      13,   327,   101,    68,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   332,    69,   168,
      49,    32,    33,     4,   102,    13,    62,    56,    57,    58,
      59,    60,    61,    63,     1,     9,    64,    63,     5,     6,
     166,    63,     9,    10,    11,    12,    13,    14,     0,    16,
      17,    18,   315,    61,    40,    22,    23,    24,    25,     7,
     156,    40,    67,    69,    31,    32,    33,    34,    35,    36,
      37,    65,     9,    11,    12,    13,    14,   186,    16,    17,
      26,    27,    28,    29,    30,   254,    53,    65,    68,    56,
      57,    58,    69,    13,    61,    62,    63,    65,    65,    65,
      67,    10,    13,    66,    71,    69,   254,   276,    10,    65,
      68,   237,    11,    12,    13,    14,    65,    16,    17,    66,
     229,    64,   231,   249,    11,    64,    13,    14,   276,    16,
      17,    63,    65,   302,    10,   304,    65,    11,   307,    63,
      63,    10,    11,    12,    13,    14,    71,    16,    17,    63,
      66,    64,    38,    61,   302,   263,   304,    69,   327,   307,
     309,   310,    13,   332,   273,    65,    63,    63,    37,    10,
     289,    66,   298,    19,    66,    23,    67,    70,     1,   327,
      70,    66,     5,     6,   332,    19,     9,    10,    11,    12,
      13,    14,    61,    16,    17,    18,    19,    70,    67,    22,
      23,    24,    25,    71,    11,   301,    36,   326,    31,    32,
      33,    34,    35,    36,    37,    37,    39,    30,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,     1,    69,   231,    71,     5,
      46,    98,   239,     9,    10,   265,   241,   261,   142,   165,
     331,   268,    18,   232,    -1,    -1,    22,    -1,    24,    25,
      -1,    -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,
      36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      -1,    -1,    -1,     5,    -1,    -1,    -1,     9,    10,    -1,
      -1,    -1,    58,    -1,    -1,    -1,    18,    63,    -1,    -1,
      22,    67,    24,    25,    -1,    71,    -1,    -1,    -1,    31,
      -1,    -1,    34,    35,    36,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    -1,    -1,    -1,     5,    -1,    -1,
      -1,     9,    10,    -1,    -1,    -1,    58,    -1,    -1,    -1,
      18,    63,    -1,    -1,    22,    67,    24,    25,    -1,    71,
      -1,    -1,    -1,    31,    -1,     1,    34,    35,    36,    37,
       6,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      16,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      58,    43,    44,    -1,    -1,    63,    -1,    49,    -1,    67,
      -1,    37,    -1,    71,    56,    57,    58,    59,    60,    61,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    58,    -1,    -1,    61,    62,     1,    -1,    65,
      -1,    67,     6,    69,    -1,     9,    10,    11,    12,    13,
      14,    -1,    16,    17,    18,     1,    -1,    -1,    -1,    -1,
       6,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      16,    17,    18,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      -1,    37,    56,    57,    58,    -1,    -1,    61,    62,    -1,
      64,    65,    66,    67,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    58,    -1,    -1,    61,    62,     1,    64,    65,
      66,    67,     6,    -1,    -1,     9,    10,    11,    12,    13,
      14,    -1,    16,    17,    18,     1,    -1,    -1,    -1,    -1,
       6,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      16,    17,    18,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    53,
      -1,    37,    56,    57,    58,    -1,    -1,    61,    62,    -1,
      -1,    65,    -1,    67,    -1,    -1,    -1,    53,    -1,    -1,
      56,    57,    58,     1,    -1,    61,    62,    63,     6,    65,
      -1,    67,    10,    11,    12,    13,    14,    -1,    16,    17,
      18,     1,    -1,    -1,    -1,    -1,     6,    -1,    -1,    -1,
      10,    11,    12,    13,    14,    -1,    16,    17,    18,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    53,    -1,    37,    56,    57,
      58,    -1,    -1,    61,    62,    -1,    -1,    65,    66,    67,
      -1,    -1,    -1,    53,    -1,    -1,    56,    57,    58,     1,
      -1,    61,    62,    -1,     6,    65,    -1,    67,    10,    11,
      12,    13,    14,     1,    16,    17,    18,     5,    -1,    -1,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    -1,    22,    37,    24,    25,    -1,    -1,
      -1,    -1,    -1,    31,    -1,    -1,    34,    35,    36,    37,
      -1,    53,    -1,    -1,    56,    57,    58,    -1,    -1,    61,
      62,    -1,    -1,    65,    -1,    67,    -1,    -1,    -1,    -1,
      58,     1,    -1,    -1,    -1,    63,     6,    -1,    -1,    67,
      10,    11,    12,    13,    14,    -1,    16,    17,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    -1,
      -1,    47,    48,    49,    -1,    -1,    -1,    37,    54,    55,
      56,    57,    58,    59,    60,    61,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,
      -1,    61,    -1,    -1,    -1,    -1,    39,    67,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    39,
      -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    -1,    -1,    -1,    -1,    -1,    -1,    39,    69,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    -1,    -1,    -1,    39,    66,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    -1,    -1,    -1,
      39,    66,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    39,    -1,    41,    42,    43,    44,    -1,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    39,    -1,    41,    42,
      43,    44,    -1,    -1,    47,    48,    49,    -1,    -1,    -1,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    39,
      -1,    41,    42,    43,    44,    -1,    -1,    47,    48,    49,
      -1,    -1,    -1,    53,    54,    55,    56,    57,    58,    59,
      60,    61
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    73,    74,     0,     3,    78,    10,    79,    63,     4,
      80,    81,    13,     7,    15,    75,    82,    83,    94,    63,
      84,    85,    86,    62,    76,     9,    95,   172,    63,    63,
      64,    10,    11,    13,    89,    77,   100,   101,    61,   173,
      89,    85,    40,    77,    82,    92,    93,    11,    69,    40,
     109,    11,    12,    13,    14,    16,    17,    88,   174,   175,
     176,   177,   178,     8,   172,    10,    18,    91,    67,   102,
      69,     1,     6,    10,    13,    18,    37,    53,    56,    57,
      58,    61,    62,    65,    67,    90,   110,   112,   116,   117,
     118,   120,   121,   122,   127,   133,   165,   168,   171,   178,
      87,    65,   103,     9,    65,    68,   118,   118,    11,   118,
      10,    37,    90,   128,   129,   118,   118,   172,   123,   109,
      19,    39,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    68,    13,    65,    10,    37,   116,     1,
      97,    98,    99,   110,   169,   170,   172,     1,     5,    10,
      22,    24,    25,    31,    34,    35,    36,    63,    90,    95,
     102,   104,   105,   115,   134,   135,   136,   143,   144,   147,
     149,   152,   157,   162,   165,    65,   166,    10,    69,    11,
      13,    14,    16,    17,   130,   131,   132,    66,    66,    13,
     124,   125,   126,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   111,   112,   118,    10,   167,
      66,    64,    64,    89,    63,   150,    70,    65,   119,   153,
      10,    65,    11,   138,   138,   118,   137,    20,    21,    61,
      68,   107,   109,    63,   163,    63,    71,   156,    63,    66,
      97,    64,    69,    38,   156,    64,   113,    69,    97,    13,
      96,    99,   170,    65,   145,   118,   158,   119,    10,   120,
      63,    63,   118,    10,    26,    27,    28,    29,    30,   108,
      95,   162,    66,   164,   131,    13,   178,   126,    19,   114,
     164,   172,   151,    66,    67,   104,   155,   154,    66,    69,
     106,   110,   118,    89,   155,   162,   159,   155,   148,   109,
     109,   146,   156,    23,   160,    67,    70,   161,    32,    33,
     139,   140,   141,   110,   155,    11,    13,   142,    70,    71,
     139,   162,    66,    19,    70,    11
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
     105,   105,   107,   105,   108,   108,   108,   108,   108,   109,
     110,   110,   111,   111,   112,   113,   112,   114,   114,   115,
     115,   116,   116,   116,   117,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   119,   119,   120,   120,
     120,   120,   120,   120,   120,   121,   121,   123,   122,   124,
     124,   125,   125,   126,   126,   128,   127,   129,   127,   130,
     130,   131,   131,   132,   132,   132,   132,   133,   133,   134,
     135,   136,   137,   137,   138,   138,   139,   139,   140,   141,
     141,   142,   142,   142,   143,   144,   146,   145,   148,   147,
     150,   151,   149,   153,   154,   152,   155,   155,   156,   158,
     159,   157,   160,   161,   160,   162,   163,   162,   164,   166,
     165,   167,   165,   168,   168,   168,    97,   169,   169,   169,
     170,   171,   172,   172,   173,   173,   174,   174,   175,   176,
     177,   178,   178,   178,   178
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
       5,     5,     0,     4,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     0,     0,     5,     0,     2,     2,
       2,     1,     1,     1,     4,     1,     2,     2,     4,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     2,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     0,     4,     0,
       1,     1,     3,     3,     3,     0,     3,     0,     4,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     2,     0,     1,     0,     1,     0,     2,     2,     3,
       2,     1,     1,     3,     2,     2,     0,     7,     0,     8,
       0,     0,     5,     0,     0,     5,     1,     3,     1,     0,
       0,     6,     0,     0,     3,     0,     0,     3,     1,     0,
       5,     0,     5,     2,     1,     1,     1,     1,     3,     1,
       1,     3,     1,     2,     2,     3,     1,     1,     1,     1,
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
#line 185 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2039 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 191 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 2047 "parser.c"
    break;

  case 9: /* namespace: %empty  */
#line 201 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2055 "parser.c"
    break;

  case 10: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 204 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2064 "parser.c"
    break;

  case 13: /* require: T_REQUIRE T_STRING ';'  */
#line 212 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2073 "parser.c"
    break;

  case 17: /* scope: %empty  */
#line 221 "Parser.y"
                { yyval.ident.scope = MS_GLOBAL; }
#line 2079 "parser.c"
    break;

  case 18: /* scope: T_STATIC  */
#line 222 "Parser.y"
                                   { yyval.ident.scope = MS_LOCAL; }
#line 2085 "parser.c"
    break;

  case 22: /* $@3: %empty  */
#line 232 "Parser.y"
                { _bDefiningConst = 1; }
#line 2091 "parser.c"
    break;

  case 23: /* $@4: %empty  */
#line 232 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2097 "parser.c"
    break;

  case 24: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 233 "Parser.y"
                        {
				BOOL bString = (yyvsp[-1].v.type == VT_STRING);
				sym_createSymbol(_currentIdentifierContext, yyvsp[-3].ident.s,
					S_CONSTANT, yyvsp[-1].v.type, yyvsp[-1].v.data);
				if (yyvsp[-3].ident.stringIsAlloced) {
					free(yyvsp[-3].ident.s);
				}
				if (bString) {
					free(yyvsp[-1].v.data.string);
				}
			}
#line 2113 "parser.c"
    break;

  case 25: /* constant_literal: simple_literal  */
#line 246 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2121 "parser.c"
    break;

  case 26: /* constant_literal: T_STRING  */
#line 249 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2130 "parser.c"
    break;

  case 27: /* variable_identifier: T_IDENT  */
#line 255 "Parser.y"
                                                                {	yyval.ident = yyvsp[0].ident; }
#line 2136 "parser.c"
    break;

  case 28: /* variable_identifier: T_NUM  */
#line 257 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2147 "parser.c"
    break;

  case 29: /* variable_identifier: T_STRING  */
#line 264 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2156 "parser.c"
    break;

  case 30: /* variable_reference: T_VARIABLE  */
#line 270 "Parser.y"
                                                        {	yyval.ident = yyvsp[0].ident; }
#line 2162 "parser.c"
    break;

  case 31: /* variable_reference: T_IDENT  */
#line 271 "Parser.y"
                                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0});
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2174 "parser.c"
    break;

  case 32: /* macro_declaration: T_IDENT  */
#line 281 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2182 "parser.c"
    break;

  case 33: /* macro_declaration: T_FUNC  */
#line 284 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2192 "parser.c"
    break;

  case 34: /* macro_type: scope T_VOID  */
#line 290 "Parser.y"
                             {
				yyval.ident.scope = yyvsp[-1].ident.scope;
			}
#line 2200 "parser.c"
    break;

  case 35: /* macro_type: scope type_name  */
#line 294 "Parser.y"
                                        {
				yyval.ident.scope = yyvsp[-1].ident.scope;
			}
#line 2208 "parser.c"
    break;

  case 36: /* macrostart: macro_type macro_declaration '(' parameter_list ')' optional_description  */
#line 299 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident;
				yyval.ident.scope = yyvsp[-5].ident.scope;
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2217 "parser.c"
    break;

  case 38: /* var_decl: type_name variable_identifier assignment_expression  */
#line 306 "Parser.y"
                                                                    {
				parser_defineVariable(yyvsp[-1].ident.s, yyvsp[-2].type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
			}
#line 2229 "parser.c"
    break;

  case 39: /* var_decl: type_name variable_identifier  */
#line 312 "Parser.y"
                                                          {
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].type, 0);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2240 "parser.c"
    break;

  case 41: /* optional_description: T_STRING  */
#line 321 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2249 "parser.c"
    break;

  case 46: /* par_decl: type_name variable_identifier  */
#line 335 "Parser.y"
                                              {
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER,yyvsp[-1].type,_nparam);
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2260 "parser.c"
    break;

  case 47: /* $@5: %empty  */
#line 342 "Parser.y"
                  { init_macroDefinition(); }
#line 2266 "parser.c"
    break;

  case 48: /* macro_definition: $@5 macrostart block  */
#line 343 "Parser.y"
                                                 {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lreturnid,0);
				bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
				macro_validateMacroName(yyvsp[-1].ident.s, -1, 1);
				YY_EMIT(C_STOP, (GENERIC_DATA){1});
				_compilerConfiguration->cb_showStatus("Defining macro %s",yyvsp[-1].ident.s);
				MACRO_PARAM param = {
					.mp_name = yyvsp[-1].ident.s,
					.mp_comment = _yyCurrentComment,
					.mp_bytecodeLength = _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start,
					.mp_buffer = _bytecodeBuffer.bb_start,
					.mp_scope = yyvsp[-1].ident.scope,
					.mp_namespaceIdx = _currentNamespaceIndex
				};
				_compilerConfiguration->cb_insertNewMacro(&param);
				freeitem(&yyvsp[-1].ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			}
#line 2291 "parser.c"
    break;

  case 53: /* statement: call_expression ';'  */
#line 370 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2297 "parser.c"
    break;

  case 54: /* statement: assignment ';'  */
#line 371 "Parser.y"
                                         { yyval.needsPop = 1; }
#line 2303 "parser.c"
    break;

  case 55: /* statement: if_expression  */
#line 372 "Parser.y"
                                        { yyval.needsPop = 1; }
#line 2309 "parser.c"
    break;

  case 56: /* statement: while  */
#line 373 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2315 "parser.c"
    break;

  case 57: /* statement: foreach  */
#line 374 "Parser.y"
                                  { yyval.needsPop = 1; }
#line 2321 "parser.c"
    break;

  case 58: /* statement: switch_expression  */
#line 375 "Parser.y"
                                            { yyval.needsPop = 1; }
#line 2327 "parser.c"
    break;

  case 59: /* statement: break  */
#line 376 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2333 "parser.c"
    break;

  case 60: /* statement: continue  */
#line 377 "Parser.y"
                                   { yyval.needsPop = 0; }
#line 2339 "parser.c"
    break;

  case 61: /* statement: return_expression  */
#line 378 "Parser.y"
                                            { yyval.needsPop = 0; }
#line 2345 "parser.c"
    break;

  case 62: /* statement: goto  */
#line 379 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2351 "parser.c"
    break;

  case 63: /* statement: block  */
#line 380 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2357 "parser.c"
    break;

  case 64: /* statement: increment_expression  */
#line 381 "Parser.y"
                                               { yyval.needsPop = 1; }
#line 2363 "parser.c"
    break;

  case 65: /* statement: label  */
#line 382 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2369 "parser.c"
    break;

  case 66: /* statement: var_decl  */
#line 383 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2377 "parser.c"
    break;

  case 67: /* statement: error ';'  */
#line 386 "Parser.y"
                                    {
				yyval.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2387 "parser.c"
    break;

  case 68: /* assignment: variable_reference assignment_expression  */
#line 392 "Parser.y"
                                                     {
				if (yyvsp[-1].ident.type != yyvsp[0].v.type) {
				// not enough type info yet for this:	yyerror("Trying to assign incompatible types.");
				}
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
			}
#line 2398 "parser.c"
    break;

  case 69: /* $@6: %empty  */
#line 399 "Parser.y"
                                                       {
				types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
			}
#line 2406 "parser.c"
    break;

  case 70: /* assignment: variable_reference '.' T_IDENT $@6 assignment_expression  */
#line 401 "Parser.y"
                                                {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)yyvsp[-4].ident.s });
			}
#line 2414 "parser.c"
    break;

  case 71: /* assignment: variable_reference '[' binary_expression ']' assignment_expression  */
#line 405 "Parser.y"
                                                                                           {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)yyvsp[-4].ident.s });
			}
#line 2422 "parser.c"
    break;

  case 72: /* $@7: %empty  */
#line 409 "Parser.y"
                                           {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 2430 "parser.c"
    break;

  case 73: /* assignment: variable_reference $@7 shorthand_assignment_operator simple_expression  */
#line 411 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-3].ident.s);
			}
#line 2439 "parser.c"
    break;

  case 74: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 417 "Parser.y"
                                         { yyval.type = BIN_MUL; }
#line 2445 "parser.c"
    break;

  case 75: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 418 "Parser.y"
                                          { yyval.type = BIN_DIV; }
#line 2451 "parser.c"
    break;

  case 76: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 419 "Parser.y"
                                          { yyval.type = BIN_MOD; }
#line 2457 "parser.c"
    break;

  case 77: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 420 "Parser.y"
                                           { yyval.type = BIN_ADD; }
#line 2463 "parser.c"
    break;

  case 78: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 421 "Parser.y"
                                            { yyval.type = BIN_SUB; }
#line 2469 "parser.c"
    break;

  case 79: /* assignment_expression: T_ASSIGN simple_expression  */
#line 423 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2475 "parser.c"
    break;

  case 80: /* simple_expression: binary_expression  */
#line 425 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2481 "parser.c"
    break;

  case 85: /* $@8: %empty  */
#line 433 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2487 "parser.c"
    break;

  case 88: /* range_increment: T_DOTDOT binary_expression  */
#line 436 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2493 "parser.c"
    break;

  case 89: /* increment_expression: variable_reference T_MINUSMINUS  */
#line 439 "Parser.y"
                                                         { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, -1); }
#line 2499 "parser.c"
    break;

  case 90: /* increment_expression: variable_reference T_PLUSPLUS  */
#line 441 "Parser.y"
                                                      { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, 1); }
#line 2505 "parser.c"
    break;

  case 91: /* s_bterm: value  */
#line 443 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2511 "parser.c"
    break;

  case 94: /* constructor_expression: T_NEW T_TYPE_IDENTIFIER '(' ')'  */
#line 447 "Parser.y"
                                                        {
				YY_EMIT(C_PUSH_NEW_INSTANCE, (GENERIC_DATA){yyvsp[-2].type});
			}
#line 2519 "parser.c"
    break;

  case 95: /* binary_expression: s_bterm  */
#line 452 "Parser.y"
                                { yyval.type = yyvsp[0].type;	}
#line 2525 "parser.c"
    break;

  case 96: /* binary_expression: type_cast s_bterm  */
#line 453 "Parser.y"
                                                        {
				yyval.type = yyvsp[-1].type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, yyval.type);
			}
#line 2534 "parser.c"
    break;

  case 97: /* binary_expression: '-' T_NUM  */
#line 457 "Parser.y"
                                    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-yyvsp[0].num);
				} else {
					yyval.type = VT_NUMBER;
					yyval.v.type = C_PUSH_LONG_LITERAL; 
					yyval.v.data.longValue  = -yyvsp[0].num;
				}
			}
#line 2548 "parser.c"
    break;

  case 98: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 466 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2554 "parser.c"
    break;

  case 99: /* binary_expression: '!' binary_expression  */
#line 467 "Parser.y"
                                                { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2560 "parser.c"
    break;

  case 100: /* binary_expression: '~' binary_expression  */
#line 468 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2566 "parser.c"
    break;

  case 101: /* binary_expression: '+' binary_expression  */
#line 469 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2572 "parser.c"
    break;

  case 102: /* binary_expression: '-' binary_expression  */
#line 470 "Parser.y"
                                                { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &yyvsp[0].v, -1); }
#line 2578 "parser.c"
    break;

  case 103: /* binary_expression: binary_expression '~' binary_expression  */
#line 471 "Parser.y"
                                                                    { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2584 "parser.c"
    break;

  case 104: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 472 "Parser.y"
                                                                         { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2590 "parser.c"
    break;

  case 105: /* binary_expression: binary_expression '&' binary_expression  */
#line 473 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2596 "parser.c"
    break;

  case 106: /* binary_expression: binary_expression '|' binary_expression  */
#line 474 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2602 "parser.c"
    break;

  case 107: /* binary_expression: binary_expression '+' binary_expression  */
#line 475 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2608 "parser.c"
    break;

  case 108: /* binary_expression: binary_expression '^' binary_expression  */
#line 476 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2614 "parser.c"
    break;

  case 109: /* binary_expression: binary_expression '-' binary_expression  */
#line 477 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2620 "parser.c"
    break;

  case 110: /* binary_expression: binary_expression '*' binary_expression  */
#line 478 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2626 "parser.c"
    break;

  case 111: /* binary_expression: binary_expression '/' binary_expression  */
#line 479 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2632 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression '%' binary_expression  */
#line 480 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2638 "parser.c"
    break;

  case 113: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 481 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2644 "parser.c"
    break;

  case 114: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 482 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2650 "parser.c"
    break;

  case 115: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 483 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2656 "parser.c"
    break;

  case 116: /* binary_expression: binary_expression T_AND binary_expression  */
#line 484 "Parser.y"
                                                                    { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2662 "parser.c"
    break;

  case 117: /* binary_expression: binary_expression T_OR binary_expression  */
#line 485 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2668 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression '<' binary_expression  */
#line 486 "Parser.y"
                                                                  { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2674 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression '>' binary_expression  */
#line 487 "Parser.y"
                                                                  { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2680 "parser.c"
    break;

  case 120: /* binary_expression: binary_expression T_LE binary_expression  */
#line 488 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2686 "parser.c"
    break;

  case 121: /* binary_expression: binary_expression T_GE binary_expression  */
#line 489 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2692 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 490 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2698 "parser.c"
    break;

  case 123: /* binary_expression: binary_expression T_NE binary_expression  */
#line 491 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2704 "parser.c"
    break;

  case 124: /* binary_expression: '(' binary_expression ')'  */
#line 492 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2710 "parser.c"
    break;

  case 125: /* binary_expression: variable_reference assignment_expression  */
#line 493 "Parser.y"
                                                                   {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
			}
#line 2718 "parser.c"
    break;

  case 127: /* condition: '(' binary_expression  */
#line 500 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2726 "parser.c"
    break;

  case 128: /* value: T_VARIABLE  */
#line 504 "Parser.y"
                           {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.type = yyvsp[0].ident.type;
			}
#line 2735 "parser.c"
    break;

  case 130: /* value: string  */
#line 509 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2745 "parser.c"
    break;

  case 131: /* value: simple_literal  */
#line 514 "Parser.y"
                                         {	yyval.type = yyvsp[0].type;	}
#line 2751 "parser.c"
    break;

  case 132: /* value: map_literal  */
#line 515 "Parser.y"
                                      {
				yyval.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2762 "parser.c"
    break;

  case 133: /* value: array_literal  */
#line 521 "Parser.y"
                                        {
				yyval.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2773 "parser.c"
    break;

  case 134: /* value: T_IDENT  */
#line 527 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				yyval.v.type = C_PUSH_VARIABLE;  
				yyval.v.data.string  = yyvsp[0].ident.s;
			}
#line 2783 "parser.c"
    break;

  case 135: /* struct_reference: T_VARIABLE '.' T_IDENT  */
#line 533 "Parser.y"
                                         {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[-2].ident.s});
				yyval.ident.type = types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2793 "parser.c"
    break;

  case 136: /* struct_reference: struct_reference '.' T_IDENT  */
#line 538 "Parser.y"
                                                       {
				yyval.ident.type = types_pushFieldIndexWithError(yyvsp[-2].ident.type, yyvsp[0].ident.s);
				YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); 
			}
#line 2802 "parser.c"
    break;

  case 137: /* $@9: %empty  */
#line 543 "Parser.y"
                 {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2810 "parser.c"
    break;

  case 143: /* map_associate: T_STRING T_ASSOC_ARROW simple_literal  */
#line 557 "Parser.y"
                                                              { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2819 "parser.c"
    break;

  case 144: /* map_associate: T_STRING T_ASSOC_ARROW T_STRING  */
#line 561 "Parser.y"
                                                          { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2828 "parser.c"
    break;

  case 145: /* $@10: %empty  */
#line 566 "Parser.y"
                   {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2836 "parser.c"
    break;

  case 147: /* $@11: %empty  */
#line 570 "Parser.y"
                              {
				_currentArrayLiteral = arraylist_create(3);
			}
#line 2844 "parser.c"
    break;

  case 151: /* array_element: simple_array_element  */
#line 579 "Parser.y"
                                             {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2852 "parser.c"
    break;

  case 152: /* array_element: T_STRING  */
#line 583 "Parser.y"
                                 {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2860 "parser.c"
    break;

  case 153: /* simple_array_element: T_NUM  */
#line 588 "Parser.y"
                                                { yyval.v.type = VT_NUMBER; yyval.v.data.longValue = yyvsp[0].num; }
#line 2866 "parser.c"
    break;

  case 154: /* simple_array_element: T_CHARACTER  */
#line 589 "Parser.y"
                                        { yyval.v.type = VT_CHAR; yyval.v.data.longValue = yyvsp[0].num; }
#line 2872 "parser.c"
    break;

  case 155: /* simple_array_element: T_TRUE  */
#line 590 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 1; }
#line 2878 "parser.c"
    break;

  case 156: /* simple_array_element: T_FALSE  */
#line 591 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 0; }
#line 2884 "parser.c"
    break;

  case 157: /* string: T_STRING  */
#line 593 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2890 "parser.c"
    break;

  case 158: /* string: string T_STRING  */
#line 594 "Parser.y"
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
#line 2905 "parser.c"
    break;

  case 159: /* break: T_BREAK opt_num ';'  */
#line 606 "Parser.y"
                        {	bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2911 "parser.c"
    break;

  case 160: /* continue: T_CONTINUE opt_num ';'  */
#line 609 "Parser.y"
                        {	bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2917 "parser.c"
    break;

  case 161: /* return_expression: T_RETURN optional_bterm  */
#line 612 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 2925 "parser.c"
    break;

  case 164: /* opt_num: %empty  */
#line 619 "Parser.y"
                {	yyval.num = 1;	}
#line 2931 "parser.c"
    break;

  case 165: /* opt_num: T_NUM  */
#line 620 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 2937 "parser.c"
    break;

  case 170: /* case_selector: T_DEFAULT ':'  */
#line 629 "Parser.y"
                                      {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 2945 "parser.c"
    break;

  case 171: /* case_condition: T_NUM  */
#line 634 "Parser.y"
                              {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].num});
			}
#line 2953 "parser.c"
    break;

  case 172: /* case_condition: T_STRING  */
#line 637 "Parser.y"
                                   {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 2961 "parser.c"
    break;

  case 173: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 640 "Parser.y"
                                               {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 2969 "parser.c"
    break;

  case 174: /* label: T_IDENT ':'  */
#line 644 "Parser.y"
                            {
				bytecode_createBranchLabel(&_bytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 2978 "parser.c"
    break;

  case 175: /* goto: T_GOTO T_IDENT  */
#line 649 "Parser.y"
                               {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 2987 "parser.c"
    break;

  case 176: /* $@12: %empty  */
#line 654 "Parser.y"
                                                  {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				parser_defineVariable(yyvsp[0].ident.s, yyvsp[-1].type, (intptr_t)0);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 3000 "parser.c"
    break;

  case 177: /* in_clause: '(' type_name variable_identifier $@12 ':' simple_expression ')'  */
#line 661 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}
#line 3011 "parser.c"
    break;

  case 178: /* $@13: %empty  */
#line 670 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 3021 "parser.c"
    break;

  case 179: /* switch_expression: T_SWITCH '(' value ')' $@13 '{' case_clauses '}'  */
#line 676 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(&_bytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3034 "parser.c"
    break;

  case 180: /* $@14: %empty  */
#line 685 "Parser.y"
                      {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lstartid,_breaklevel);
			}
#line 3042 "parser.c"
    break;

  case 181: /* $@15: %empty  */
#line 687 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3051 "parser.c"
    break;

  case 182: /* foreach: T_FOR $@14 in_clause $@15 local_block  */
#line 690 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3063 "parser.c"
    break;

  case 183: /* $@16: %empty  */
#line 698 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lstartid,_breaklevel);
			}
#line 3071 "parser.c"
    break;

  case 184: /* $@17: %empty  */
#line 700 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3080 "parser.c"
    break;

  case 185: /* while: T_WHILE $@16 condition $@17 local_block  */
#line 703 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3092 "parser.c"
    break;

  case 189: /* $@18: %empty  */
#line 716 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3101 "parser.c"
    break;

  case 190: /* $@19: %empty  */
#line 719 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3109 "parser.c"
    break;

  case 191: /* if_expression: T_IF condition $@18 local_block $@19 else_clause  */
#line 721 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3118 "parser.c"
    break;

  case 192: /* else_clause: %empty  */
#line 726 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3127 "parser.c"
    break;

  case 193: /* $@20: %empty  */
#line 730 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3137 "parser.c"
    break;

  case 195: /* stmntlist: %empty  */
#line 737 "Parser.y"
                { yyval.needsPop = 0; }
#line 3143 "parser.c"
    break;

  case 196: /* $@21: %empty  */
#line 738 "Parser.y"
                                    { 
				if (yyvsp[0].needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			}
#line 3153 "parser.c"
    break;

  case 199: /* @22: %empty  */
#line 746 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3165 "parser.c"
    break;

  case 200: /* call_expression: T_FUNC '(' @22 parameter_list closing_paren  */
#line 752 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3175 "parser.c"
    break;

  case 201: /* @23: %empty  */
#line 757 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3186 "parser.c"
    break;

  case 202: /* call_expression: function_id_or_pointer '(' @23 parameter_list closing_paren  */
#line 762 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, yyvsp[-4].ident.operation,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&yyvsp[-4].ident.s);
			}
#line 3196 "parser.c"
    break;

  case 203: /* function_id_or_pointer: '*' variable_reference  */
#line 769 "Parser.y"
                                               {
				yyval.ident.operation = C_MACRO_REF;
				yyval.ident.s = _strdup(yyvsp[0].ident.s);
				yyval.ident.stringIsAlloced = 1;
			}
#line 3206 "parser.c"
    break;

  case 204: /* function_id_or_pointer: T_IDENT  */
#line 775 "Parser.y"
                                {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.operation = C_MACRO;
			}
#line 3215 "parser.c"
    break;

  case 205: /* function_id_or_pointer: error  */
#line 779 "Parser.y"
                                {
				yyerror("Illegal function call expression");
				yyerrok;
				yyval.ident.s = 0;
			}
#line 3225 "parser.c"
    break;

  case 209: /* nonempty_parameters: error  */
#line 789 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3233 "parser.c"
    break;

  case 210: /* parameter: simple_expression  */
#line 793 "Parser.y"
                                  { yyval.type = yyvsp[0].type; (*_currentFunctionCallParamIndexP)++; }
#line 3239 "parser.c"
    break;

  case 211: /* type_cast: '(' type_name ')'  */
#line 795 "Parser.y"
                                        {	yyval.type = yyvsp[-1].type;		}
#line 3245 "parser.c"
    break;

  case 213: /* type_name: T_TYPE_IDENTIFIER array_dimension  */
#line 798 "Parser.y"
                                                                {	yyval.type = VT_OBJECT_ARRAY;	}
#line 3251 "parser.c"
    break;

  case 215: /* array_dimension: '[' T_NUM ']'  */
#line 801 "Parser.y"
                                        {
				_arraySize = (long)yyvsp[-1].num;
			}
#line 3259 "parser.c"
    break;

  case 216: /* boolean_literal: T_TRUE  */
#line 805 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3268 "parser.c"
    break;

  case 217: /* boolean_literal: T_FALSE  */
#line 808 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3277 "parser.c"
    break;

  case 218: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 813 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3286 "parser.c"
    break;

  case 219: /* integer_literal: T_NUM  */
#line 818 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3295 "parser.c"
    break;

  case 220: /* character_literal: T_CHARACTER  */
#line 823 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3304 "parser.c"
    break;

  case 221: /* simple_literal: integer_literal  */
#line 828 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_NUMBER;
			}
#line 3317 "parser.c"
    break;

  case 222: /* simple_literal: float_literal  */
#line 836 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_FLOAT;
			}
#line 3330 "parser.c"
    break;

  case 223: /* simple_literal: character_literal  */
#line 844 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_CHAR;
			}
#line 3343 "parser.c"
    break;

  case 224: /* simple_literal: boolean_literal  */
#line 852 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_BOOLEAN;
			}
#line 3356 "parser.c"
    break;


#line 3360 "parser.c"

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

#line 860 "Parser.y"


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

