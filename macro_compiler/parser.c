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
static MACRO_SCOPE _macroScope;
static MACROC_INSTRUCTION_OP_CODE _functionCallOp;
static int	_currentNamespaceIndex;
static int  _foreachCursorIndex;
static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				 *switchTableId  = "%sw%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

int			vname_count;
extern 		int	_bDefiningConst;
extern 		int	_bInHeader;
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

static void makeInternalSymbol(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal) {
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
	if (sType == VT_STRING) {
		if (!tVal) {
			tVal = (intptr_t)"";
		}
			

	}
	sym_createSymbol(_currentIdentifierContext, (char*) pszName, S_VARIABLE, sType, (GENERIC_DATA) {.val = tVal});
}

static void push_newForeachCursor() {
	char buf[32];
	sprintf(buf, "#c%d", _foreachCursorIndex++);
	YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=buf});
}


#line 220 "parser.c"

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
  YYSYMBOL_T_STATIC = 6,                   /* T_STATIC  */
  YYSYMBOL_T_VOID = 7,                     /* T_VOID  */
  YYSYMBOL_T_TYPE_IDENTIFIER = 8,          /* T_TYPE_IDENTIFIER  */
  YYSYMBOL_T_IDENT = 9,                    /* T_IDENT  */
  YYSYMBOL_T_NUM = 10,                     /* T_NUM  */
  YYSYMBOL_T_FLOATING_POINT_NUMBER = 11,   /* T_FLOATING_POINT_NUMBER  */
  YYSYMBOL_T_STRING = 12,                  /* T_STRING  */
  YYSYMBOL_T_CHARACTER = 13,               /* T_CHARACTER  */
  YYSYMBOL_T_CONST = 14,                   /* T_CONST  */
  YYSYMBOL_T_TRUE = 15,                    /* T_TRUE  */
  YYSYMBOL_T_FALSE = 16,                   /* T_FALSE  */
  YYSYMBOL_T_FUNC = 17,                    /* T_FUNC  */
  YYSYMBOL_T_DOTDOT = 18,                  /* T_DOTDOT  */
  YYSYMBOL_T_PLUSPLUS = 19,                /* T_PLUSPLUS  */
  YYSYMBOL_T_MINUSMINUS = 20,              /* T_MINUSMINUS  */
  YYSYMBOL_T_IF = 21,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 22,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 23,                   /* T_WHILE  */
  YYSYMBOL_T_GOTO = 24,                    /* T_GOTO  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 25,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 26,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 27,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 28,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 29,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_SWITCH = 30,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 31,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 32,                 /* T_DEFAULT  */
  YYSYMBOL_T_BREAK = 33,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 34,                /* T_CONTINUE  */
  YYSYMBOL_T_RETURN = 35,                  /* T_RETURN  */
  YYSYMBOL_T_VARIABLE = 36,                /* T_VARIABLE  */
  YYSYMBOL_T_ASSOC_ARROW = 37,             /* T_ASSOC_ARROW  */
  YYSYMBOL_T_NMATCH = 38,                  /* T_NMATCH  */
  YYSYMBOL_T_ASSIGN = 39,                  /* T_ASSIGN  */
  YYSYMBOL_T_NE = 40,                      /* T_NE  */
  YYSYMBOL_T_EQ = 41,                      /* T_EQ  */
  YYSYMBOL_T_SHIFT_LEFT = 42,              /* T_SHIFT_LEFT  */
  YYSYMBOL_T_SHIFT_RIGHT = 43,             /* T_SHIFT_RIGHT  */
  YYSYMBOL_T_OR = 44,                      /* T_OR  */
  YYSYMBOL_T_AND = 45,                     /* T_AND  */
  YYSYMBOL_T_LE = 46,                      /* T_LE  */
  YYSYMBOL_T_GE = 47,                      /* T_GE  */
  YYSYMBOL_T_POWER_TO = 48,                /* T_POWER_TO  */
  YYSYMBOL_49_ = 49,                       /* '&'  */
  YYSYMBOL_50_ = 50,                       /* '|'  */
  YYSYMBOL_51_ = 51,                       /* '^'  */
  YYSYMBOL_52_ = 52,                       /* '~'  */
  YYSYMBOL_53_ = 53,                       /* '<'  */
  YYSYMBOL_54_ = 54,                       /* '>'  */
  YYSYMBOL_55_ = 55,                       /* '+'  */
  YYSYMBOL_56_ = 56,                       /* '-'  */
  YYSYMBOL_57_ = 57,                       /* '*'  */
  YYSYMBOL_58_ = 58,                       /* '/'  */
  YYSYMBOL_59_ = 59,                       /* '%'  */
  YYSYMBOL_60_ = 60,                       /* '['  */
  YYSYMBOL_61_ = 61,                       /* '!'  */
  YYSYMBOL_62_ = 62,                       /* ';'  */
  YYSYMBOL_63_ = 63,                       /* ','  */
  YYSYMBOL_64_ = 64,                       /* '('  */
  YYSYMBOL_65_ = 65,                       /* ')'  */
  YYSYMBOL_66_ = 66,                       /* '{'  */
  YYSYMBOL_67_ = 67,                       /* ']'  */
  YYSYMBOL_68_ = 68,                       /* ':'  */
  YYSYMBOL_69_ = 69,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 70,                  /* $accept  */
  YYSYMBOL_file_structure = 71,            /* file_structure  */
  YYSYMBOL_72_1 = 72,                      /* $@1  */
  YYSYMBOL_73_2 = 73,                      /* $@2  */
  YYSYMBOL_header_delimiter = 74,          /* header_delimiter  */
  YYSYMBOL_macro_definitions = 75,         /* macro_definitions  */
  YYSYMBOL_namespace = 76,                 /* namespace  */
  YYSYMBOL_prerequisites = 77,             /* prerequisites  */
  YYSYMBOL_require = 78,                   /* require  */
  YYSYMBOL_global_declarations = 79,       /* global_declarations  */
  YYSYMBOL_scope = 80,                     /* scope  */
  YYSYMBOL_constants = 81,                 /* constants  */
  YYSYMBOL_constant_list = 82,             /* constant_list  */
  YYSYMBOL_constdef = 83,                  /* constdef  */
  YYSYMBOL_84_3 = 84,                      /* $@3  */
  YYSYMBOL_85_4 = 85,                      /* $@4  */
  YYSYMBOL_constant_literal = 86,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 87,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 88,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 89,         /* macro_declaration  */
  YYSYMBOL_macro_type = 90,                /* macro_type  */
  YYSYMBOL_macrostart = 91,                /* macrostart  */
  YYSYMBOL_global_var = 92,                /* global_var  */
  YYSYMBOL_var_decl = 93,                  /* var_decl  */
  YYSYMBOL_optional_description = 94,      /* optional_description  */
  YYSYMBOL_parameter_list = 95,            /* parameter_list  */
  YYSYMBOL_non_empty_pardecl = 96,         /* non_empty_pardecl  */
  YYSYMBOL_par_decl = 97,                  /* par_decl  */
  YYSYMBOL_macro_definition = 98,          /* macro_definition  */
  YYSYMBOL_99_5 = 99,                      /* $@5  */
  YYSYMBOL_block = 100,                    /* block  */
  YYSYMBOL_var_decls = 101,                /* var_decls  */
  YYSYMBOL_statement = 102,                /* statement  */
  YYSYMBOL_assignment = 103,               /* assignment  */
  YYSYMBOL_104_6 = 104,                    /* $@6  */
  YYSYMBOL_shorthand_assignment_operator = 105, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 106,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 107,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 108, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 109,         /* range_expression  */
  YYSYMBOL_110_7 = 110,                    /* $@7  */
  YYSYMBOL_range_increment = 111,          /* range_increment  */
  YYSYMBOL_increment_expression = 112,     /* increment_expression  */
  YYSYMBOL_s_bterm = 113,                  /* s_bterm  */
  YYSYMBOL_binary_expression = 114,        /* binary_expression  */
  YYSYMBOL_condition = 115,                /* condition  */
  YYSYMBOL_value = 116,                    /* value  */
  YYSYMBOL_map_literal = 117,              /* map_literal  */
  YYSYMBOL_118_8 = 118,                    /* $@8  */
  YYSYMBOL_map_associates = 119,           /* map_associates  */
  YYSYMBOL_map_associate = 120,            /* map_associate  */
  YYSYMBOL_array_literal = 121,            /* array_literal  */
  YYSYMBOL_122_9 = 122,                    /* $@9  */
  YYSYMBOL_123_10 = 123,                   /* $@10  */
  YYSYMBOL_array_elements = 124,           /* array_elements  */
  YYSYMBOL_array_element = 125,            /* array_element  */
  YYSYMBOL_simple_array_element = 126,     /* simple_array_element  */
  YYSYMBOL_string = 127,                   /* string  */
  YYSYMBOL_break = 128,                    /* break  */
  YYSYMBOL_continue = 129,                 /* continue  */
  YYSYMBOL_return_expression = 130,        /* return_expression  */
  YYSYMBOL_optional_bterm = 131,           /* optional_bterm  */
  YYSYMBOL_opt_num = 132,                  /* opt_num  */
  YYSYMBOL_case_clauses = 133,             /* case_clauses  */
  YYSYMBOL_case_clause = 134,              /* case_clause  */
  YYSYMBOL_case_selector = 135,            /* case_selector  */
  YYSYMBOL_case_condition = 136,           /* case_condition  */
  YYSYMBOL_label = 137,                    /* label  */
  YYSYMBOL_goto = 138,                     /* goto  */
  YYSYMBOL_in_clause = 139,                /* in_clause  */
  YYSYMBOL_140_11 = 140,                   /* $@11  */
  YYSYMBOL_switch_expression = 141,        /* switch_expression  */
  YYSYMBOL_142_12 = 142,                   /* $@12  */
  YYSYMBOL_foreach = 143,                  /* foreach  */
  YYSYMBOL_144_13 = 144,                   /* $@13  */
  YYSYMBOL_145_14 = 145,                   /* $@14  */
  YYSYMBOL_while = 146,                    /* while  */
  YYSYMBOL_147_15 = 147,                   /* $@15  */
  YYSYMBOL_148_16 = 148,                   /* $@16  */
  YYSYMBOL_local_block = 149,              /* local_block  */
  YYSYMBOL_closing_brace = 150,            /* closing_brace  */
  YYSYMBOL_if_expression = 151,            /* if_expression  */
  YYSYMBOL_152_17 = 152,                   /* $@17  */
  YYSYMBOL_153_18 = 153,                   /* $@18  */
  YYSYMBOL_else_clause = 154,              /* else_clause  */
  YYSYMBOL_155_19 = 155,                   /* $@19  */
  YYSYMBOL_stmntlist = 156,                /* stmntlist  */
  YYSYMBOL_157_20 = 157,                   /* $@20  */
  YYSYMBOL_closing_paren = 158,            /* closing_paren  */
  YYSYMBOL_call_expression = 159,          /* call_expression  */
  YYSYMBOL_160_21 = 160,                   /* @21  */
  YYSYMBOL_161_22 = 161,                   /* @22  */
  YYSYMBOL_function_id_or_pointer = 162,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 163,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 164,                /* parameter  */
  YYSYMBOL_type_cast = 165,                /* type_cast  */
  YYSYMBOL_type_name = 166,                /* type_name  */
  YYSYMBOL_array_dimension = 167,          /* array_dimension  */
  YYSYMBOL_boolean_literal = 168,          /* boolean_literal  */
  YYSYMBOL_float_literal = 169,            /* float_literal  */
  YYSYMBOL_integer_literal = 170,          /* integer_literal  */
  YYSYMBOL_character_literal = 171,        /* character_literal  */
  YYSYMBOL_simple_literal = 172            /* simple_literal  */
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
#define YYLAST   913

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  70
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  103
/* YYNRULES -- Number of rules.  */
#define YYNRULES  215
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  331

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   303


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
       2,     2,     2,    61,     2,     2,     2,    59,    49,     2,
      64,    65,    57,    55,    63,    56,     2,    58,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    68,    62,
      53,     2,    54,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    60,     2,    67,    51,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    66,    50,    69,    52,     2,     2,     2,
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
      45,    46,    47,    48
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   180,   180,   186,   180,   190,   191,   193,   194,   196,
     199,   204,   205,   207,   212,   213,   214,   216,   217,   221,
     224,   225,   227,   227,   227,   241,   244,   250,   251,   258,
     265,   266,   276,   279,   285,   287,   289,   294,   296,   303,
     311,   313,   318,   319,   322,   324,   327,   334,   334,   356,
     358,   359,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,   371,   372,   373,   374,   375,   378,   384,   389,
     394,   394,   403,   404,   405,   406,   407,   409,   411,   412,
     415,   416,   418,   419,   419,   421,   422,   425,   427,   429,
     430,   433,   434,   438,   447,   448,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   459,   460,   461,   462,
     463,   464,   465,   466,   467,   468,   469,   470,   471,   472,
     473,   474,   481,   482,   486,   490,   495,   496,   502,   508,
     514,   514,   519,   521,   524,   528,   533,   533,   537,   537,
     542,   543,   546,   550,   555,   556,   557,   558,   560,   561,
     572,   575,   579,   583,   584,   586,   587,   589,   590,   592,
     594,   596,   601,   604,   607,   611,   616,   621,   621,   637,
     636,   652,   654,   652,   665,   667,   665,   678,   678,   680,
     683,   686,   683,   693,   697,   697,   704,   705,   705,   711,
     713,   713,   724,   724,   736,   742,   746,   752,   754,   755,
     756,   760,   762,   764,   765,   767,   768,   772,   775,   780,
     785,   790,   795,   803,   811,   819
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
  "T_FOR", "T_STATIC", "T_VOID", "T_TYPE_IDENTIFIER", "T_IDENT", "T_NUM",
  "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER", "T_CONST",
  "T_TRUE", "T_FALSE", "T_FUNC", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS",
  "T_IF", "T_ELSE", "T_WHILE", "T_GOTO", "T_SH_ASSIGN_PLUS",
  "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV",
  "T_SH_ASSIGN_MOD", "T_SWITCH", "T_CASE", "T_DEFAULT", "T_BREAK",
  "T_CONTINUE", "T_RETURN", "T_VARIABLE", "T_ASSOC_ARROW", "T_NMATCH",
  "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR",
  "T_AND", "T_LE", "T_GE", "T_POWER_TO", "'&'", "'|'", "'^'", "'~'", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "'!'", "';'", "','",
  "'('", "')'", "'{'", "']'", "':'", "'}'", "$accept", "file_structure",
  "$@1", "$@2", "header_delimiter", "macro_definitions", "namespace",
  "prerequisites", "require", "global_declarations", "scope", "constants",
  "constant_list", "constdef", "$@3", "$@4", "constant_literal",
  "variable_identifier", "variable_reference", "macro_declaration",
  "macro_type", "macrostart", "global_var", "var_decl",
  "optional_description", "parameter_list", "non_empty_pardecl",
  "par_decl", "macro_definition", "$@5", "block", "var_decls", "statement",
  "assignment", "$@6", "shorthand_assignment_operator",
  "assignment_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@7",
  "range_increment", "increment_expression", "s_bterm",
  "binary_expression", "condition", "value", "map_literal", "$@8",
  "map_associates", "map_associate", "array_literal", "$@9", "$@10",
  "array_elements", "array_element", "simple_array_element", "string",
  "break", "continue", "return_expression", "optional_bterm", "opt_num",
  "case_clauses", "case_clause", "case_selector", "case_condition",
  "label", "goto", "in_clause", "$@11", "switch_expression", "$@12",
  "foreach", "$@13", "$@14", "while", "$@15", "$@16", "local_block",
  "closing_brace", "if_expression", "$@17", "$@18", "else_clause", "$@19",
  "stmntlist", "$@20", "closing_paren", "call_expression", "@21", "@22",
  "function_id_or_pointer", "nonempty_parameters", "parameter",
  "type_cast", "type_name", "array_dimension", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-270)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-197)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -270,    32,    31,  -270,    44,  -270,   -11,    83,  -270,    82,
    -270,    12,    41,  -270,  -270,    46,    97,    47,    52,  -270,
      53,  -270,     7,  -270,   124,    73,  -270,     7,  -270,  -270,
    -270,  -270,  -270,  -270,   102,  -270,   124,   130,    19,  -270,
     106,  -270,   329,  -270,   105,    22,    88,    99,  -270,   235,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,   103,  -270,  -270,
    -270,  -270,   -24,  -270,   107,   131,   651,   651,   664,     2,
     110,   651,   562,  -270,   106,  -270,  -270,  -270,   709,  -270,
    -270,  -270,   162,  -270,   117,   682,  -270,  -270,   470,   364,
    -270,   104,   -13,  -270,   -13,  -270,  -270,  -270,   115,   127,
     784,   732,   118,   175,  -270,   651,   651,   651,   651,   651,
     651,   651,   651,   651,   651,   651,   651,   651,   651,   651,
     651,   651,   651,   651,   651,   651,   651,   437,  -270,  -270,
     125,  -270,  -270,   140,   147,   132,  -270,  -270,   152,  -270,
       7,   156,  -270,    -9,   157,  -270,   214,   161,   216,   216,
     302,  -270,    -6,   166,  -270,  -270,   168,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,   163,   173,   470,
    -270,  -270,  -270,  -270,  -270,  -270,    26,  -270,  -270,  -270,
    -270,   200,   -33,  -270,   784,   104,   830,   830,    42,    42,
     807,   807,   150,   150,   178,   853,   853,   853,   104,   150,
     150,   -13,   -13,   178,   178,   178,   174,  -270,   709,   470,
     230,    97,   501,  -270,  -270,   185,  -270,   651,  -270,   157,
    -270,   119,  -270,   192,   193,   784,  -270,  -270,  -270,   119,
     349,  -270,  -270,    87,  -270,  -270,  -270,  -270,   195,   127,
    -270,   336,   175,  -270,   243,  -270,   195,  -270,  -270,  -270,
    -270,    97,  -270,   758,   167,  -270,  -270,   199,  -270,  -270,
     205,  -270,  -270,  -270,  -270,  -270,   575,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,   651,  -270,  -270,     7,   167,
    -270,   401,  -270,  -270,   167,  -270,   106,  -270,  -270,  -270,
    -270,   163,   252,  -270,   239,  -270,   238,  -270,  -270,  -270,
      95,   593,   167,    40,   240,   247,    95,    87,   244,  -270,
     303,  -270,   254,  -270,  -270,  -270,  -270,  -270,   310,  -270,
    -270
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     9,     1,     0,    11,     0,    14,    10,     0,
      12,     3,     0,    18,    22,     5,     0,     0,     0,    13,
      19,    20,     0,     6,    47,   203,    37,     0,    15,    16,
      22,    27,    28,    29,     0,     4,    47,    17,     0,   204,
      39,    21,     0,     8,     0,     0,     0,     0,   205,     0,
      38,   210,   209,    26,   211,   207,   208,    23,   215,   213,
     212,   214,    25,    34,    35,    32,    33,     0,    50,    48,
     206,   196,   129,   148,     0,   124,     0,     0,     0,     0,
     138,     0,     0,   130,     0,    77,    79,    91,    78,    89,
     127,   128,   125,    90,     0,     0,   126,    24,     0,     0,
     190,    96,    97,    93,    98,    31,    30,   194,     0,     0,
      95,     0,     0,     0,   121,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   149,   192,
     129,   124,    92,   200,     0,    43,    44,   201,   197,   198,
       0,   196,   171,    31,     0,   174,     0,     0,   155,   155,
       0,    52,    70,    66,    63,   187,     0,    64,    59,    60,
      61,    65,    62,    58,    57,    56,    55,     0,     0,     0,
     137,   144,   143,   145,   146,   147,     0,   141,   142,   120,
     202,     0,     0,   132,    83,   100,   119,   118,   109,   110,
     113,   112,   116,   117,   111,   101,   102,   104,    99,   114,
     115,   103,   105,   106,   107,   108,     0,    81,    80,     0,
      40,     0,     0,    46,    67,     0,   165,     0,   180,     0,
     166,     0,   156,     0,     0,   154,   152,    88,    87,     0,
       0,    68,    51,     0,    54,   179,    49,    53,     0,     0,
     139,     0,     0,   131,    85,    94,     0,    41,    36,    45,
     199,     0,   172,   123,     0,   175,   129,     0,   150,   151,
       0,    75,    76,    72,    73,    74,     0,    66,   188,   189,
     191,   140,   135,   134,   133,     0,    84,   193,     0,     0,
     122,     0,   177,   181,     0,   169,     0,    71,    86,   167,
     173,     0,   183,   176,     0,    69,     0,   178,   184,   182,
     157,     0,     0,     0,     0,     0,   157,     0,     0,   185,
     162,   163,     0,   161,   170,   158,   159,   168,     0,   160,
     164
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -270,  -270,  -270,  -270,  -270,   291,  -270,  -270,  -270,  -270,
     292,  -270,  -270,   298,  -270,  -270,  -270,   -25,   -78,  -270,
    -270,  -270,  -270,    -8,  -270,  -170,  -270,   109,  -270,  -270,
     285,  -270,  -252,  -270,  -270,  -270,   -77,   -49,  -270,   206,
    -270,  -270,  -270,   255,   -54,   126,  -183,  -270,  -270,  -270,
     101,  -270,  -270,  -270,  -270,   111,  -270,  -270,  -270,  -270,
    -270,  -270,   197,    45,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -269,
    -186,  -270,  -270,  -270,  -270,  -270,  -233,  -270,   114,   -95,
    -270,  -270,  -270,  -270,   145,  -270,   -41,  -270,  -270,  -270,
    -270,  -270,   -37
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    15,    24,    35,     5,     7,    10,    11,
      16,    17,    20,    21,    22,    97,    57,    34,    84,    67,
      45,    46,    18,   277,   258,   144,   145,   146,    36,    37,
     164,    99,   165,   166,   240,   276,    50,   147,   216,    86,
     254,   286,   167,    87,    88,   228,    89,    90,   113,   192,
     193,    91,   108,   109,   186,   187,   188,    92,   168,   169,
     170,   236,   233,   315,   316,   317,   322,   171,   172,   262,
     306,   173,   304,   174,   225,   289,   175,   229,   294,   293,
     246,   176,   264,   302,   309,   312,   177,   243,   280,    93,
     179,   219,    94,   148,   149,    95,    27,    39,    58,    59,
      60,    61,    96
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      85,   107,    40,    64,   178,    62,   253,   114,    26,   248,
     278,   105,   292,   237,   238,   -31,    31,    32,    13,    33,
     300,   162,   101,   102,   104,   303,    14,   110,   111,    47,
     252,    65,     3,    49,     4,   125,   245,   292,   106,    66,
    -195,   112,   292,   319,   134,   135,   136,   137,   267,   256,
     320,     8,   321,     6,   239,  -195,   270,   150,   301,   226,
     292,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   218,   326,   241,    48,     9,   151,   249,
     125,   163,   152,   250,    12,    25,   153,   132,   133,   134,
     135,   136,   137,    19,    74,    25,   235,    23,   154,    28,
     155,   156,    63,    25,    29,   307,    30,   157,  -186,  -186,
     158,   159,   160,   106,    -7,   223,   313,   314,   266,    51,
      52,    73,    54,    38,    55,    56,    13,   181,   150,   182,
     183,    42,   184,   185,    79,    49,   119,   120,   178,   161,
     123,   124,   125,    68,    68,   141,  -186,   130,   131,   132,
     133,   134,   135,   136,   137,   162,    70,    98,   151,   178,
     -30,   100,   152,   263,   138,    25,   153,  -136,   150,    80,
     150,   139,   180,   190,    74,    83,   162,   191,   154,  -195,
     155,   156,   119,   120,   178,   221,   178,   157,   125,   178,
     158,   159,   160,   106,  -196,   132,   133,   134,   135,   136,
     137,   162,   220,   162,   283,   222,   162,   178,   224,   305,
     288,   227,   178,   230,    79,   231,   232,   297,   242,   161,
     244,   298,   245,   291,   162,   247,    71,   251,   137,   162,
     -82,   255,   257,   -82,    72,    51,    52,    73,    54,   261,
      55,    56,    74,   -82,   268,   269,   -82,   -82,   -82,   -82,
     279,   285,   318,   299,   295,   -82,   -82,   -82,   -82,   -82,
     -82,    75,   296,   -82,   308,   -82,   -82,   -82,   -82,   -82,
     -82,   -82,   -82,   -82,   -82,   -82,   -82,    76,   -82,   -82,
      77,    78,    79,   -82,   -82,    80,    81,   -82,   -82,    82,
     -82,    83,   -82,    71,   -82,   310,   311,  -153,   323,   327,
    -153,    72,    51,    52,    73,    54,   324,    55,    56,    74,
     330,   328,   329,  -153,  -153,  -153,  -153,    43,    41,    44,
     259,    69,  -153,  -153,  -153,  -153,  -153,  -153,    75,    51,
      52,    53,    54,   217,    55,    56,    51,    52,   282,    54,
     142,    55,    56,   284,    76,   265,   234,    77,    78,    79,
     281,   325,    80,    81,  -153,   151,    82,   260,    83,   152,
     287,  -153,    25,   153,   271,   272,   273,   274,   275,     0,
       0,    74,     0,     0,     0,   154,     0,   155,   156,     0,
       0,     0,     0,     0,   157,     0,     0,   158,   159,   160,
     106,     0,   151,     0,     0,     0,   152,     0,     0,    25,
     153,     0,     0,     0,     0,     0,     0,     0,    74,     0,
       0,    79,   154,     0,   155,   156,   161,     0,     0,     0,
      68,   157,     0,  -186,   158,   159,   160,   106,    71,     0,
       0,     0,     0,     0,     0,     0,    72,    51,    52,    73,
      54,     0,    55,    56,    74,     0,     0,     0,    79,     0,
       0,     0,     0,   161,     0,     0,     0,    68,     0,     0,
     -50,   143,     0,    75,     0,     0,     0,     0,    25,    72,
      51,    52,    73,    54,     0,    55,    56,    74,     0,    76,
       0,     0,    77,    78,    79,     0,     0,    80,    81,     0,
       0,    82,    71,    83,   -82,     0,    75,     0,     0,     0,
      72,    51,    52,    73,    54,     0,    55,    56,    74,     0,
       0,     0,    76,     0,     0,    77,    78,    79,     0,     0,
      80,    81,     0,   -82,    82,   -42,    83,    75,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    76,     0,     0,    77,    78,    79,     0,
       0,    80,    81,    71,   -82,    82,   -82,    83,     0,     0,
      25,    72,    51,    52,    73,    54,    71,    55,    56,    74,
       0,     0,     0,     0,    72,    51,    52,    73,    54,     0,
      55,    56,    74,     0,    71,     0,     0,     0,    75,     0,
       0,     0,    72,    51,    52,    73,    54,     0,    55,    56,
      74,    75,     0,     0,    76,     0,     0,    77,    78,    79,
       0,     0,    80,    81,     0,     0,    82,    76,    83,    75,
      77,    78,    79,     0,     0,    80,    81,   -82,     0,    82,
       0,    83,     0,     0,     0,    76,     0,     0,    77,    78,
      79,     0,    71,    80,    81,     0,     0,    82,   -82,    83,
      72,    51,    52,    73,    54,    71,    55,    56,    74,     0,
       0,     0,     0,    72,   103,    52,    73,    54,     0,    55,
      56,    74,     0,    71,     0,     0,     0,    75,     0,     0,
       0,   140,    51,    52,    73,    54,     0,    55,    56,    74,
      75,     0,     0,    76,     0,     0,    77,    78,    79,     0,
       0,    80,    81,     0,     0,    82,    76,    83,   141,    77,
      78,    79,     0,     0,    80,    81,     0,   115,    82,     0,
      83,     0,     0,     0,     0,     0,     0,     0,     0,    79,
       0,     0,    80,     0,     0,     0,     0,   116,    83,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     116,     0,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,     0,     0,     0,   116,   189,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,     0,
       0,     0,   116,   290,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   116,     0,   117,   118,   119,
     120,     0,     0,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   116,     0,
    -197,  -197,   119,   120,     0,     0,   123,   124,   125,     0,
       0,     0,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   116,     0,   117,   118,   119,   120,     0,     0,   123,
     124,   125,     0,     0,     0,   129,   130,   131,   132,   133,
     134,   135,   136,   137
};

static const yytype_int16 yycheck[] =
{
      49,    79,    27,    44,    99,    42,   192,    84,    16,   179,
     243,     9,   264,    19,    20,    39,     9,    10,     6,    12,
     289,    99,    76,    77,    78,   294,    14,    81,    82,    10,
      63,     9,     0,    39,     3,    48,    69,   289,    36,    17,
      64,    82,   294,   312,    57,    58,    59,    60,   231,   219,
      10,    62,    12,     9,    60,    64,   239,    98,   291,    68,
     312,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   317,   162,    67,     4,     1,    63,
      48,    99,     5,    67,    12,     8,     9,    55,    56,    57,
      58,    59,    60,    62,    17,     8,   160,    61,    21,    62,
      23,    24,     7,     8,    62,   301,    63,    30,    31,    32,
      33,    34,    35,    36,     0,   150,    31,    32,     9,    10,
      11,    12,    13,    60,    15,    16,     6,    10,   179,    12,
      13,    39,    15,    16,    57,    39,    42,    43,   243,    62,
      46,    47,    48,    66,    66,    36,    69,    53,    54,    55,
      56,    57,    58,    59,    60,   243,    67,    64,     1,   264,
      39,    64,     5,   227,    12,     8,     9,    67,   219,    60,
     221,    64,    67,    65,    17,    66,   264,    12,    21,    64,
      23,    24,    42,    43,   289,    63,   291,    30,    48,   294,
      33,    34,    35,    36,    64,    55,    56,    57,    58,    59,
      60,   289,    65,   291,   251,    63,   294,   312,    62,   296,
     261,    64,   317,     9,    57,    64,    10,   276,    62,    62,
      62,   285,    69,    66,   312,    62,     1,    37,    60,   317,
       5,    67,    12,     8,     9,    10,    11,    12,    13,    64,
      15,    16,    17,    18,    62,    62,    21,    22,    23,    24,
      65,    18,   311,   288,    65,    30,    31,    32,    33,    34,
      35,    36,    67,    38,    22,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     1,    69,    66,    68,     5,    68,    65,
       8,     9,    10,    11,    12,    13,    69,    15,    16,    17,
      10,    18,    68,    21,    22,    23,    24,    36,    30,    37,
     221,    46,    30,    31,    32,    33,    34,    35,    36,    10,
      11,    12,    13,   137,    15,    16,    10,    11,    12,    13,
      95,    15,    16,   252,    52,   229,   159,    55,    56,    57,
     249,   316,    60,    61,    62,     1,    64,   222,    66,     5,
     256,    69,     8,     9,    25,    26,    27,    28,    29,    -1,
      -1,    17,    -1,    -1,    -1,    21,    -1,    23,    24,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    -1,    33,    34,    35,
      36,    -1,     1,    -1,    -1,    -1,     5,    -1,    -1,     8,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,
      -1,    57,    21,    -1,    23,    24,    62,    -1,    -1,    -1,
      66,    30,    -1,    69,    33,    34,    35,    36,     1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     9,    10,    11,    12,
      13,    -1,    15,    16,    17,    -1,    -1,    -1,    57,    -1,
      -1,    -1,    -1,    62,    -1,    -1,    -1,    66,    -1,    -1,
      69,     1,    -1,    36,    -1,    -1,    -1,    -1,     8,     9,
      10,    11,    12,    13,    -1,    15,    16,    17,    -1,    52,
      -1,    -1,    55,    56,    57,    -1,    -1,    60,    61,    -1,
      -1,    64,     1,    66,    67,    -1,    36,    -1,    -1,    -1,
       9,    10,    11,    12,    13,    -1,    15,    16,    17,    -1,
      -1,    -1,    52,    -1,    -1,    55,    56,    57,    -1,    -1,
      60,    61,    -1,    63,    64,    65,    66,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    55,    56,    57,    -1,
      -1,    60,    61,     1,    63,    64,    65,    66,    -1,    -1,
       8,     9,    10,    11,    12,    13,     1,    15,    16,    17,
      -1,    -1,    -1,    -1,     9,    10,    11,    12,    13,    -1,
      15,    16,    17,    -1,     1,    -1,    -1,    -1,    36,    -1,
      -1,    -1,     9,    10,    11,    12,    13,    -1,    15,    16,
      17,    36,    -1,    -1,    52,    -1,    -1,    55,    56,    57,
      -1,    -1,    60,    61,    -1,    -1,    64,    52,    66,    36,
      55,    56,    57,    -1,    -1,    60,    61,    62,    -1,    64,
      -1,    66,    -1,    -1,    -1,    52,    -1,    -1,    55,    56,
      57,    -1,     1,    60,    61,    -1,    -1,    64,    65,    66,
       9,    10,    11,    12,    13,     1,    15,    16,    17,    -1,
      -1,    -1,    -1,     9,    10,    11,    12,    13,    -1,    15,
      16,    17,    -1,     1,    -1,    -1,    -1,    36,    -1,    -1,
      -1,     9,    10,    11,    12,    13,    -1,    15,    16,    17,
      36,    -1,    -1,    52,    -1,    -1,    55,    56,    57,    -1,
      -1,    60,    61,    -1,    -1,    64,    52,    66,    36,    55,
      56,    57,    -1,    -1,    60,    61,    -1,    18,    64,    -1,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,
      -1,    -1,    60,    -1,    -1,    -1,    -1,    38,    66,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      38,    -1,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    -1,    38,    65,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    -1,    38,    65,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    38,    -1,    40,    41,    42,
      43,    -1,    -1,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    38,    -1,
      40,    41,    42,    43,    -1,    -1,    46,    47,    48,    -1,
      -1,    -1,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    38,    -1,    40,    41,    42,    43,    -1,    -1,    46,
      47,    48,    -1,    -1,    -1,    52,    53,    54,    55,    56,
      57,    58,    59,    60
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    71,    72,     0,     3,    76,     9,    77,    62,     4,
      78,    79,    12,     6,    14,    73,    80,    81,    92,    62,
      82,    83,    84,    61,    74,     8,    93,   166,    62,    62,
      63,     9,    10,    12,    87,    75,    98,    99,    60,   167,
      87,    83,    39,    75,    80,    90,    91,    10,    67,    39,
     106,    10,    11,    12,    13,    15,    16,    86,   168,   169,
     170,   171,   172,     7,   166,     9,    17,    89,    66,   100,
      67,     1,     9,    12,    17,    36,    52,    55,    56,    57,
      60,    61,    64,    66,    88,   107,   109,   113,   114,   116,
     117,   121,   127,   159,   162,   165,   172,    85,    64,   101,
      64,   114,   114,    10,   114,     9,    36,    88,   122,   123,
     114,   114,   166,   118,   106,    18,    38,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    12,    64,
       9,    36,   113,     1,    95,    96,    97,   107,   163,   164,
     166,     1,     5,     9,    21,    23,    24,    30,    33,    34,
      35,    62,    88,    93,   100,   102,   103,   112,   128,   129,
     130,   137,   138,   141,   143,   146,   151,   156,   159,   160,
      67,    10,    12,    13,    15,    16,   124,   125,   126,    65,
      65,    12,   119,   120,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   108,   109,   114,   161,
      65,    63,    63,    87,    62,   144,    68,    64,   115,   147,
       9,    64,    10,   132,   132,   114,   131,    19,    20,    60,
     104,   106,    62,   157,    62,    69,   150,    62,    95,    63,
      67,    37,    63,   150,   110,    67,    95,    12,    94,    97,
     164,    64,   139,   114,   152,   115,     9,   116,    62,    62,
     116,    25,    26,    27,    28,    29,   105,    93,   156,    65,
     158,   125,    12,   172,   120,    18,   111,   158,   166,   145,
      65,    66,   102,   149,   148,    65,    67,   107,   114,    87,
     149,   156,   153,   149,   142,   106,   140,   150,    22,   154,
      66,    68,   155,    31,    32,   133,   134,   135,   107,   149,
      10,    12,   136,    68,    69,   133,   156,    65,    18,    68,
      10
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    70,    72,    73,    71,    74,    74,    75,    75,    76,
      76,    77,    77,    78,    79,    79,    79,    80,    80,    81,
      82,    82,    84,    85,    83,    86,    86,    87,    87,    87,
      88,    88,    89,    89,    90,    90,    91,    92,    93,    93,
      94,    94,    95,    95,    96,    96,    97,    99,    98,   100,
     101,   101,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   103,   103,
     104,   103,   105,   105,   105,   105,   105,   106,   107,   107,
     108,   108,   109,   110,   109,   111,   111,   112,   112,   113,
     113,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   115,   115,   116,   116,   116,   116,   116,   116,
     118,   117,   119,   119,   120,   120,   122,   121,   123,   121,
     124,   124,   125,   125,   126,   126,   126,   126,   127,   127,
     128,   129,   130,   131,   131,   132,   132,   133,   133,   134,
     135,   135,   136,   136,   136,   137,   138,   140,   139,   142,
     141,   144,   145,   143,   147,   148,   146,   149,   149,   150,
     152,   153,   151,   154,   155,   154,   156,   157,   156,   158,
     160,   159,   161,   159,   162,   162,   162,    95,   163,   163,
     163,   164,   165,   166,   166,   167,   167,   168,   168,   169,
     170,   171,   172,   172,   172,   172
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
       1,     1,     1,     1,     1,     1,     1,     2,     2,     5,
       0,     4,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     0,     0,     5,     0,     2,     2,     2,     1,
       1,     1,     2,     2,     4,     2,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     2,     1,     1,     1,     1,     1,     1,
       0,     4,     1,     3,     3,     3,     0,     3,     0,     4,
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
#line 180 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 1991 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 186 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 1999 "parser.c"
    break;

  case 9: /* namespace: %empty  */
#line 196 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2007 "parser.c"
    break;

  case 10: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 199 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2016 "parser.c"
    break;

  case 13: /* require: T_REQUIRE T_STRING ';'  */
#line 207 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2025 "parser.c"
    break;

  case 17: /* scope: %empty  */
#line 216 "Parser.y"
                { _macroScope = MS_GLOBAL; }
#line 2031 "parser.c"
    break;

  case 18: /* scope: T_STATIC  */
#line 217 "Parser.y"
                                   { _macroScope = MS_LOCAL; }
#line 2037 "parser.c"
    break;

  case 22: /* $@3: %empty  */
#line 227 "Parser.y"
                { _bDefiningConst = 1; }
#line 2043 "parser.c"
    break;

  case 23: /* $@4: %empty  */
#line 227 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2049 "parser.c"
    break;

  case 24: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 228 "Parser.y"
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
#line 2065 "parser.c"
    break;

  case 25: /* constant_literal: simple_literal  */
#line 241 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2073 "parser.c"
    break;

  case 26: /* constant_literal: T_STRING  */
#line 244 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2082 "parser.c"
    break;

  case 27: /* variable_identifier: T_IDENT  */
#line 250 "Parser.y"
                                                                {	yyval.ident = yyvsp[0].ident; }
#line 2088 "parser.c"
    break;

  case 28: /* variable_identifier: T_NUM  */
#line 252 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2099 "parser.c"
    break;

  case 29: /* variable_identifier: T_STRING  */
#line 259 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2108 "parser.c"
    break;

  case 30: /* variable_reference: T_VARIABLE  */
#line 265 "Parser.y"
                                                        {	yyval.ident = yyvsp[0].ident; }
#line 2114 "parser.c"
    break;

  case 31: /* variable_reference: T_IDENT  */
#line 266 "Parser.y"
                                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0});
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2126 "parser.c"
    break;

  case 32: /* macro_declaration: T_IDENT  */
#line 276 "Parser.y"
                           {
				yyval.ident = yyvsp[0].ident;
			}
#line 2134 "parser.c"
    break;

  case 33: /* macro_declaration: T_FUNC  */
#line 279 "Parser.y"
                                 {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2144 "parser.c"
    break;

  case 36: /* macrostart: macro_type macro_declaration '(' parameter_list ')' optional_description  */
#line 290 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2152 "parser.c"
    break;

  case 38: /* var_decl: type_name variable_identifier assignment_expression  */
#line 296 "Parser.y"
                                                                    {
				makeInternalSymbol(yyvsp[-1].ident.s, yyvsp[-2].type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				vname_count = 0;
			}
#line 2165 "parser.c"
    break;

  case 39: /* var_decl: type_name variable_identifier  */
#line 303 "Parser.y"
                                                          {
				makeInternalSymbol(yyvsp[0].ident.s, yyvsp[-1].type, 0);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
				vname_count = 0;
			}
#line 2177 "parser.c"
    break;

  case 41: /* optional_description: T_STRING  */
#line 313 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2186 "parser.c"
    break;

  case 46: /* par_decl: type_name variable_identifier  */
#line 327 "Parser.y"
                                              {
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER,yyvsp[-1].type,_nparam);
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2197 "parser.c"
    break;

  case 47: /* $@5: %empty  */
#line 334 "Parser.y"
                  { init_macroDefinition(); }
#line 2203 "parser.c"
    break;

  case 48: /* macro_definition: $@5 macrostart block  */
#line 335 "Parser.y"
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
					.mp_scope = _macroScope,
					.mp_namespaceIdx = _currentNamespaceIndex
				};
				_compilerConfiguration->cb_insertNewMacro(&param);
				freeitem(&yyvsp[-1].ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			}
#line 2228 "parser.c"
    break;

  case 53: /* statement: call_expression ';'  */
#line 362 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2234 "parser.c"
    break;

  case 54: /* statement: assignment ';'  */
#line 363 "Parser.y"
                                         { yyval.needsPop = 1; }
#line 2240 "parser.c"
    break;

  case 55: /* statement: if_expression  */
#line 364 "Parser.y"
                                        { yyval.needsPop = 1; }
#line 2246 "parser.c"
    break;

  case 56: /* statement: while  */
#line 365 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2252 "parser.c"
    break;

  case 57: /* statement: foreach  */
#line 366 "Parser.y"
                                  { yyval.needsPop = 1; }
#line 2258 "parser.c"
    break;

  case 58: /* statement: switch_expression  */
#line 367 "Parser.y"
                                            { yyval.needsPop = 1; }
#line 2264 "parser.c"
    break;

  case 59: /* statement: break  */
#line 368 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2270 "parser.c"
    break;

  case 60: /* statement: continue  */
#line 369 "Parser.y"
                                   { yyval.needsPop = 0; }
#line 2276 "parser.c"
    break;

  case 61: /* statement: return_expression  */
#line 370 "Parser.y"
                                            { yyval.needsPop = 0; }
#line 2282 "parser.c"
    break;

  case 62: /* statement: goto  */
#line 371 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2288 "parser.c"
    break;

  case 63: /* statement: block  */
#line 372 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2294 "parser.c"
    break;

  case 64: /* statement: increment_expression  */
#line 373 "Parser.y"
                                               { yyval.needsPop = 1; }
#line 2300 "parser.c"
    break;

  case 65: /* statement: label  */
#line 374 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2306 "parser.c"
    break;

  case 66: /* statement: var_decl  */
#line 375 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2314 "parser.c"
    break;

  case 67: /* statement: error ';'  */
#line 378 "Parser.y"
                                    {
				yyval.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2324 "parser.c"
    break;

  case 68: /* assignment: variable_reference assignment_expression  */
#line 384 "Parser.y"
                                                     {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
				vname_count = 0;
			}
#line 2333 "parser.c"
    break;

  case 69: /* assignment: variable_reference '[' value ']' assignment_expression  */
#line 389 "Parser.y"
                                                                               {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)yyvsp[-4].ident.s });
				vname_count = 0;
			}
#line 2342 "parser.c"
    break;

  case 70: /* $@6: %empty  */
#line 394 "Parser.y"
                                           {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 2350 "parser.c"
    break;

  case 71: /* assignment: variable_reference $@6 shorthand_assignment_operator simple_expression  */
#line 396 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-3].ident.s);
				vname_count = 0;
			}
#line 2360 "parser.c"
    break;

  case 72: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 403 "Parser.y"
                                         { yyval.type = BIN_MUL; }
#line 2366 "parser.c"
    break;

  case 73: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 404 "Parser.y"
                                          { yyval.type = BIN_DIV; }
#line 2372 "parser.c"
    break;

  case 74: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 405 "Parser.y"
                                          { yyval.type = BIN_MOD; }
#line 2378 "parser.c"
    break;

  case 75: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 406 "Parser.y"
                                           { yyval.type = BIN_ADD; }
#line 2384 "parser.c"
    break;

  case 76: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 407 "Parser.y"
                                            { yyval.type = BIN_SUB; }
#line 2390 "parser.c"
    break;

  case 77: /* assignment_expression: T_ASSIGN simple_expression  */
#line 409 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2396 "parser.c"
    break;

  case 78: /* simple_expression: binary_expression  */
#line 411 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2402 "parser.c"
    break;

  case 83: /* $@7: %empty  */
#line 419 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2408 "parser.c"
    break;

  case 86: /* range_increment: T_DOTDOT binary_expression  */
#line 422 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2414 "parser.c"
    break;

  case 87: /* increment_expression: variable_reference T_MINUSMINUS  */
#line 425 "Parser.y"
                                                         { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, -1); }
#line 2420 "parser.c"
    break;

  case 88: /* increment_expression: variable_reference T_PLUSPLUS  */
#line 427 "Parser.y"
                                                      { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, 1); }
#line 2426 "parser.c"
    break;

  case 89: /* s_bterm: value  */
#line 429 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2432 "parser.c"
    break;

  case 91: /* binary_expression: s_bterm  */
#line 433 "Parser.y"
                                 { yyval.type = yyvsp[0].type;	}
#line 2438 "parser.c"
    break;

  case 92: /* binary_expression: type_cast s_bterm  */
#line 434 "Parser.y"
                                                        {
				yyval.type = yyvsp[-1].type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, yyval.type);
			}
#line 2447 "parser.c"
    break;

  case 93: /* binary_expression: '-' T_NUM  */
#line 438 "Parser.y"
                                    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-yyvsp[0].num);
				} else {
					yyval.type = VT_NUMBER;
					yyval.v.type = C_PUSH_LONG_LITERAL; 
					yyval.v.data.longValue  = -yyvsp[0].num;
				}
			}
#line 2461 "parser.c"
    break;

  case 94: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 447 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2467 "parser.c"
    break;

  case 95: /* binary_expression: '!' binary_expression  */
#line 448 "Parser.y"
                                                { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2473 "parser.c"
    break;

  case 96: /* binary_expression: '~' binary_expression  */
#line 449 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2479 "parser.c"
    break;

  case 97: /* binary_expression: '+' binary_expression  */
#line 450 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2485 "parser.c"
    break;

  case 98: /* binary_expression: '-' binary_expression  */
#line 451 "Parser.y"
                                                { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &yyvsp[0].v, -1); }
#line 2491 "parser.c"
    break;

  case 99: /* binary_expression: binary_expression '~' binary_expression  */
#line 452 "Parser.y"
                                                                    { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2497 "parser.c"
    break;

  case 100: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 453 "Parser.y"
                                                                         { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2503 "parser.c"
    break;

  case 101: /* binary_expression: binary_expression '&' binary_expression  */
#line 454 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2509 "parser.c"
    break;

  case 102: /* binary_expression: binary_expression '|' binary_expression  */
#line 455 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2515 "parser.c"
    break;

  case 103: /* binary_expression: binary_expression '+' binary_expression  */
#line 456 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2521 "parser.c"
    break;

  case 104: /* binary_expression: binary_expression '^' binary_expression  */
#line 457 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2527 "parser.c"
    break;

  case 105: /* binary_expression: binary_expression '-' binary_expression  */
#line 458 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2533 "parser.c"
    break;

  case 106: /* binary_expression: binary_expression '*' binary_expression  */
#line 459 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2539 "parser.c"
    break;

  case 107: /* binary_expression: binary_expression '/' binary_expression  */
#line 460 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2545 "parser.c"
    break;

  case 108: /* binary_expression: binary_expression '%' binary_expression  */
#line 461 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2551 "parser.c"
    break;

  case 109: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 462 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2557 "parser.c"
    break;

  case 110: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 463 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2563 "parser.c"
    break;

  case 111: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 464 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2569 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression T_AND binary_expression  */
#line 465 "Parser.y"
                                                                    { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2575 "parser.c"
    break;

  case 113: /* binary_expression: binary_expression T_OR binary_expression  */
#line 466 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2581 "parser.c"
    break;

  case 114: /* binary_expression: binary_expression '<' binary_expression  */
#line 467 "Parser.y"
                                                                  { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2587 "parser.c"
    break;

  case 115: /* binary_expression: binary_expression '>' binary_expression  */
#line 468 "Parser.y"
                                                                  { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2593 "parser.c"
    break;

  case 116: /* binary_expression: binary_expression T_LE binary_expression  */
#line 469 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2599 "parser.c"
    break;

  case 117: /* binary_expression: binary_expression T_GE binary_expression  */
#line 470 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2605 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 471 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2611 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression T_NE binary_expression  */
#line 472 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2617 "parser.c"
    break;

  case 120: /* binary_expression: '(' binary_expression ')'  */
#line 473 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2623 "parser.c"
    break;

  case 121: /* binary_expression: variable_reference assignment_expression  */
#line 474 "Parser.y"
                                                                   {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
				vname_count = 0;
			}
#line 2632 "parser.c"
    break;

  case 123: /* condition: '(' binary_expression  */
#line 482 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2640 "parser.c"
    break;

  case 124: /* value: T_VARIABLE  */
#line 486 "Parser.y"
                           {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.type = yyvsp[0].ident.type;
			}
#line 2649 "parser.c"
    break;

  case 125: /* value: string  */
#line 490 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2659 "parser.c"
    break;

  case 126: /* value: simple_literal  */
#line 495 "Parser.y"
                                         {	yyval.type = yyvsp[0].type;	}
#line 2665 "parser.c"
    break;

  case 127: /* value: map_literal  */
#line 496 "Parser.y"
                                      {
				yyval.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2676 "parser.c"
    break;

  case 128: /* value: array_literal  */
#line 502 "Parser.y"
                                        {
				yyval.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2687 "parser.c"
    break;

  case 129: /* value: T_IDENT  */
#line 508 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				yyval.v.type = C_PUSH_VARIABLE;  
				yyval.v.data.string  = yyvsp[0].ident.s;
			}
#line 2697 "parser.c"
    break;

  case 130: /* $@8: %empty  */
#line 514 "Parser.y"
                 {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2705 "parser.c"
    break;

  case 134: /* map_associate: T_STRING T_ASSOC_ARROW simple_literal  */
#line 524 "Parser.y"
                                                              { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2714 "parser.c"
    break;

  case 135: /* map_associate: T_STRING T_ASSOC_ARROW T_STRING  */
#line 528 "Parser.y"
                                                          { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2723 "parser.c"
    break;

  case 136: /* $@9: %empty  */
#line 533 "Parser.y"
                   {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2731 "parser.c"
    break;

  case 138: /* $@10: %empty  */
#line 537 "Parser.y"
                              {
				_currentArrayLiteral = arraylist_create(3);
			}
#line 2739 "parser.c"
    break;

  case 142: /* array_element: simple_array_element  */
#line 546 "Parser.y"
                                             {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2747 "parser.c"
    break;

  case 143: /* array_element: T_STRING  */
#line 550 "Parser.y"
                                 {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2755 "parser.c"
    break;

  case 144: /* simple_array_element: T_NUM  */
#line 555 "Parser.y"
                                                { yyval.v.type = VT_NUMBER; yyval.v.data.longValue = yyvsp[0].num; }
#line 2761 "parser.c"
    break;

  case 145: /* simple_array_element: T_CHARACTER  */
#line 556 "Parser.y"
                                        { yyval.v.type = VT_CHAR; yyval.v.data.longValue = yyvsp[0].num; }
#line 2767 "parser.c"
    break;

  case 146: /* simple_array_element: T_TRUE  */
#line 557 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 1; }
#line 2773 "parser.c"
    break;

  case 147: /* simple_array_element: T_FALSE  */
#line 558 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 0; }
#line 2779 "parser.c"
    break;

  case 148: /* string: T_STRING  */
#line 560 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2785 "parser.c"
    break;

  case 149: /* string: string T_STRING  */
#line 561 "Parser.y"
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
#line 2800 "parser.c"
    break;

  case 150: /* break: T_BREAK opt_num ';'  */
#line 573 "Parser.y"
                        {	bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2806 "parser.c"
    break;

  case 151: /* continue: T_CONTINUE opt_num ';'  */
#line 576 "Parser.y"
                        {	bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2812 "parser.c"
    break;

  case 152: /* return_expression: T_RETURN optional_bterm  */
#line 579 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 2820 "parser.c"
    break;

  case 155: /* opt_num: %empty  */
#line 586 "Parser.y"
                {	yyval.num = 1;	}
#line 2826 "parser.c"
    break;

  case 156: /* opt_num: T_NUM  */
#line 587 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 2832 "parser.c"
    break;

  case 161: /* case_selector: T_DEFAULT ':'  */
#line 596 "Parser.y"
                                      {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_NIL, (GENERIC_DATA){.longValue=0});
			}
#line 2840 "parser.c"
    break;

  case 162: /* case_condition: T_NUM  */
#line 601 "Parser.y"
                              {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_NUMBER, (GENERIC_DATA){.longValue=yyvsp[0].num});
			}
#line 2848 "parser.c"
    break;

  case 163: /* case_condition: T_STRING  */
#line 604 "Parser.y"
                                   {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_STRING, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 2856 "parser.c"
    break;

  case 164: /* case_condition: T_NUM T_DOTDOT T_NUM  */
#line 607 "Parser.y"
                                               {
				bytecode_addSwitchCondition(&_bytecodeBuffer, _breaklevel, VT_RANGE, (GENERIC_DATA){.range.r_start=yyvsp[-2].num, .range.r_end=yyvsp[0].num});
			}
#line 2864 "parser.c"
    break;

  case 165: /* label: T_IDENT ':'  */
#line 611 "Parser.y"
                            {
				bytecode_createBranchLabel(&_bytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 2873 "parser.c"
    break;

  case 166: /* goto: T_GOTO T_IDENT  */
#line 616 "Parser.y"
                               {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 2882 "parser.c"
    break;

  case 167: /* $@11: %empty  */
#line 621 "Parser.y"
                                                  {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				makeInternalSymbol(yyvsp[0].ident.s, yyvsp[-1].type, (intptr_t)0);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2895 "parser.c"
    break;

  case 168: /* in_clause: '(' type_name variable_identifier $@11 ':' simple_expression ')'  */
#line 628 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}
#line 2906 "parser.c"
    break;

  case 169: /* $@12: %empty  */
#line 637 "Parser.y"
                        {
				_breaklevel++;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, switchTableId,_breaklevel,BRA_ALWAYS);
				bytecode_startSwitchTable(_breaklevel);
			}
#line 2916 "parser.c"
    break;

  case 170: /* switch_expression: T_SWITCH '(' value ')' $@12 '{' case_clauses '}'  */
#line 643 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, switchTableId,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(switchTableId,_breaklevel);
				bytecode_flushSwitchTable(&_bytecodeBuffer, _breaklevel);
				_breaklevel--;
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 2929 "parser.c"
    break;

  case 171: /* $@13: %empty  */
#line 652 "Parser.y"
                      {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lstartid,_breaklevel);
			}
#line 2937 "parser.c"
    break;

  case 172: /* $@14: %empty  */
#line 654 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 2946 "parser.c"
    break;

  case 173: /* foreach: T_FOR $@13 in_clause $@14 local_block  */
#line 657 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 2958 "parser.c"
    break;

  case 174: /* $@15: %empty  */
#line 665 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lstartid,_breaklevel);
			}
#line 2966 "parser.c"
    break;

  case 175: /* $@16: %empty  */
#line 667 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 2975 "parser.c"
    break;

  case 176: /* while: T_WHILE $@15 condition $@16 local_block  */
#line 670 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(&_bytecodeBuffer, lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 2987 "parser.c"
    break;

  case 180: /* $@17: %empty  */
#line 683 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 2996 "parser.c"
    break;

  case 181: /* $@18: %empty  */
#line 686 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3004 "parser.c"
    break;

  case 182: /* if_expression: T_IF condition $@17 local_block $@18 else_clause  */
#line 688 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3013 "parser.c"
    break;

  case 183: /* else_clause: %empty  */
#line 693 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3022 "parser.c"
    break;

  case 184: /* $@19: %empty  */
#line 697 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(&_bytecodeBuffer, ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(&_bytecodeBuffer, iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3032 "parser.c"
    break;

  case 186: /* stmntlist: %empty  */
#line 704 "Parser.y"
                { yyval.needsPop = 0; }
#line 3038 "parser.c"
    break;

  case 187: /* $@20: %empty  */
#line 705 "Parser.y"
                                    { 
				if (yyvsp[0].needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			}
#line 3048 "parser.c"
    break;

  case 190: /* @21: %empty  */
#line 713 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3060 "parser.c"
    break;

  case 191: /* call_expression: T_FUNC '(' @21 parameter_list closing_paren  */
#line 719 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3070 "parser.c"
    break;

  case 192: /* @22: %empty  */
#line 724 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3081 "parser.c"
    break;

  case 193: /* call_expression: function_id_or_pointer '(' @22 parameter_list closing_paren  */
#line 729 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, _functionCallOp,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&yyvsp[-4].ident.s);
			}
#line 3091 "parser.c"
    break;

  case 194: /* function_id_or_pointer: '*' variable_reference  */
#line 736 "Parser.y"
                                               {
				_functionCallOp = C_MACRO_REF;
				yyval.ident.s = _strdup(yyvsp[0].ident.s);
				yyval.ident.stringIsAlloced = 1;
			}
#line 3101 "parser.c"
    break;

  case 195: /* function_id_or_pointer: T_IDENT  */
#line 742 "Parser.y"
                                {
				_functionCallOp = C_MACRO;
				yyval.ident = yyvsp[0].ident;
			}
#line 3110 "parser.c"
    break;

  case 196: /* function_id_or_pointer: error  */
#line 746 "Parser.y"
                                {
				yyerror("Illegal function call expression");
				yyerrok;
				yyval.ident.s = 0;
			}
#line 3120 "parser.c"
    break;

  case 200: /* nonempty_parameters: error  */
#line 756 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3128 "parser.c"
    break;

  case 201: /* parameter: simple_expression  */
#line 760 "Parser.y"
                                  { yyval.type = yyvsp[0].type; (*_currentFunctionCallParamIndexP)++; }
#line 3134 "parser.c"
    break;

  case 202: /* type_cast: '(' type_name ')'  */
#line 762 "Parser.y"
                                        {	yyval.type = yyvsp[-1].type;		}
#line 3140 "parser.c"
    break;

  case 204: /* type_name: T_TYPE_IDENTIFIER array_dimension  */
#line 765 "Parser.y"
                                                                {	yyval.type = VT_OBJECT_ARRAY;	}
#line 3146 "parser.c"
    break;

  case 206: /* array_dimension: '[' T_NUM ']'  */
#line 768 "Parser.y"
                                        {
				_arraySize = (long)yyvsp[-1].num;
			}
#line 3154 "parser.c"
    break;

  case 207: /* boolean_literal: T_TRUE  */
#line 772 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3163 "parser.c"
    break;

  case 208: /* boolean_literal: T_FALSE  */
#line 775 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3172 "parser.c"
    break;

  case 209: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 780 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3181 "parser.c"
    break;

  case 210: /* integer_literal: T_NUM  */
#line 785 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3190 "parser.c"
    break;

  case 211: /* character_literal: T_CHARACTER  */
#line 790 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3199 "parser.c"
    break;

  case 212: /* simple_literal: integer_literal  */
#line 795 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_NUMBER;
			}
#line 3212 "parser.c"
    break;

  case 213: /* simple_literal: float_literal  */
#line 803 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_FLOAT;
			}
#line 3225 "parser.c"
    break;

  case 214: /* simple_literal: character_literal  */
#line 811 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_CHAR;
			}
#line 3238 "parser.c"
    break;

  case 215: /* simple_literal: boolean_literal  */
#line 819 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_BOOLEAN;
			}
#line 3251 "parser.c"
    break;


#line 3255 "parser.c"

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

#line 827 "Parser.y"


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
#ifdef YYDEBUG
int	yydebug = 1;
#endif
	_foreachCursorIndex = 0;
	_currentFunctionCallParamIndexP = _currentFunctionCallParamIndices;
	init_bytecodeBuffer();
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext);
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

