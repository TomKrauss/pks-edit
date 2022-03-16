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
#line 48 "Parser.y"


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
static 		char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp);
static		int bytecode_generateAutoLabelNamePrefix(char *prefix,int level);
static		void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
static void 	bytecode_emitPushParameterInstruction(long long lValue);

static int _arraySize;
static MACRO_SCOPE _macroScope;
static MACROC_INSTRUCTION_OP_CODE _functionCallOp;
static int	_currentNamespaceIndex;
static int  _foreachCursorIndex;
static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

int			vname_count;
extern 		int	_bDefiningConst;
extern char*    _yyCurrentComment;
extern void		yyerror(const char* s, ...);
extern void		yywarning(char* s, ...);
extern void		yyrequire(const char* pszNamespace);
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern int macro_lookupNamespace(const char* pszNamespacename);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nIncrement);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);
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


#line 206 "parser.c"

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
  YYSYMBOL_T_STATIC = 5,                   /* T_STATIC  */
  YYSYMBOL_T_FOR = 6,                      /* T_FOR  */
  YYSYMBOL_T_TYPE_IDENTIFIER = 7,          /* T_TYPE_IDENTIFIER  */
  YYSYMBOL_T_IDENT = 8,                    /* T_IDENT  */
  YYSYMBOL_T_NUM = 9,                      /* T_NUM  */
  YYSYMBOL_T_FLOATING_POINT_NUMBER = 10,   /* T_FLOATING_POINT_NUMBER  */
  YYSYMBOL_T_STRING = 11,                  /* T_STRING  */
  YYSYMBOL_T_CHARACTER = 12,               /* T_CHARACTER  */
  YYSYMBOL_T_CONST = 13,                   /* T_CONST  */
  YYSYMBOL_T_TRUE = 14,                    /* T_TRUE  */
  YYSYMBOL_T_FALSE = 15,                   /* T_FALSE  */
  YYSYMBOL_T_TBOOLEAN = 16,                /* T_TBOOLEAN  */
  YYSYMBOL_T_FUNC = 17,                    /* T_FUNC  */
  YYSYMBOL_T_MACRO = 18,                   /* T_MACRO  */
  YYSYMBOL_T_DOTDOT = 19,                  /* T_DOTDOT  */
  YYSYMBOL_T_PLUSPLUS = 20,                /* T_PLUSPLUS  */
  YYSYMBOL_T_MINUSMINUS = 21,              /* T_MINUSMINUS  */
  YYSYMBOL_T_IF = 22,                      /* T_IF  */
  YYSYMBOL_T_ELIF = 23,                    /* T_ELIF  */
  YYSYMBOL_T_ELSE = 24,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 25,                   /* T_WHILE  */
  YYSYMBOL_T_GOTO = 26,                    /* T_GOTO  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 27,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 28,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 29,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 30,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 31,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_LOCAL = 32,                   /* T_LOCAL  */
  YYSYMBOL_T_SWITCH = 33,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 34,                    /* T_CASE  */
  YYSYMBOL_T_DEFAULT = 35,                 /* T_DEFAULT  */
  YYSYMBOL_T_BREAK = 36,                   /* T_BREAK  */
  YYSYMBOL_T_CONTINUE = 37,                /* T_CONTINUE  */
  YYSYMBOL_T_RETURN = 38,                  /* T_RETURN  */
  YYSYMBOL_T_VARIABLE = 39,                /* T_VARIABLE  */
  YYSYMBOL_T_ASSOC_ARROW = 40,             /* T_ASSOC_ARROW  */
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
  YYSYMBOL_T_VOID = 52,                    /* T_VOID  */
  YYSYMBOL_T_CMDSEQPREFIX = 53,            /* T_CMDSEQPREFIX  */
  YYSYMBOL_54_ = 54,                       /* '&'  */
  YYSYMBOL_55_ = 55,                       /* '|'  */
  YYSYMBOL_56_ = 56,                       /* '^'  */
  YYSYMBOL_57_ = 57,                       /* '~'  */
  YYSYMBOL_58_ = 58,                       /* '<'  */
  YYSYMBOL_59_ = 59,                       /* '>'  */
  YYSYMBOL_T_LT = 60,                      /* T_LT  */
  YYSYMBOL_T_GT = 61,                      /* T_GT  */
  YYSYMBOL_62_ = 62,                       /* '+'  */
  YYSYMBOL_63_ = 63,                       /* '-'  */
  YYSYMBOL_64_ = 64,                       /* '*'  */
  YYSYMBOL_65_ = 65,                       /* '/'  */
  YYSYMBOL_66_ = 66,                       /* '%'  */
  YYSYMBOL_67_ = 67,                       /* '['  */
  YYSYMBOL_68_ = 68,                       /* ';'  */
  YYSYMBOL_69_ = 69,                       /* ','  */
  YYSYMBOL_70_ = 70,                       /* '('  */
  YYSYMBOL_71_ = 71,                       /* ')'  */
  YYSYMBOL_72_ = 72,                       /* '{'  */
  YYSYMBOL_73_ = 73,                       /* ']'  */
  YYSYMBOL_74_ = 74,                       /* '!'  */
  YYSYMBOL_75_ = 75,                       /* ':'  */
  YYSYMBOL_76_ = 76,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 77,                  /* $accept  */
  YYSYMBOL_file_structure = 78,            /* file_structure  */
  YYSYMBOL_79_1 = 79,                      /* $@1  */
  YYSYMBOL_80_2 = 80,                      /* $@2  */
  YYSYMBOL_namespace = 81,                 /* namespace  */
  YYSYMBOL_prerequisites = 82,             /* prerequisites  */
  YYSYMBOL_require = 83,                   /* require  */
  YYSYMBOL_global_declarations = 84,       /* global_declarations  */
  YYSYMBOL_global_var = 85,                /* global_var  */
  YYSYMBOL_scope = 86,                     /* scope  */
  YYSYMBOL_macro_definitions = 87,         /* macro_definitions  */
  YYSYMBOL_constants = 88,                 /* constants  */
  YYSYMBOL_constant_list = 89,             /* constant_list  */
  YYSYMBOL_constdef = 90,                  /* constdef  */
  YYSYMBOL_91_3 = 91,                      /* $@3  */
  YYSYMBOL_92_4 = 92,                      /* $@4  */
  YYSYMBOL_constant_literal = 93,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 94,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 95,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 96,         /* macro_declaration  */
  YYSYMBOL_macro_type = 97,                /* macro_type  */
  YYSYMBOL_macrostart = 98,                /* macrostart  */
  YYSYMBOL_99_5 = 99,                      /* $@5  */
  YYSYMBOL_optional_description = 100,     /* optional_description  */
  YYSYMBOL_parameter_list = 101,           /* parameter_list  */
  YYSYMBOL_non_empty_pardecl = 102,        /* non_empty_pardecl  */
  YYSYMBOL_par_decl = 103,                 /* par_decl  */
  YYSYMBOL_macro_definition = 104,         /* macro_definition  */
  YYSYMBOL_block = 105,                    /* block  */
  YYSYMBOL_var_decls = 106,                /* var_decls  */
  YYSYMBOL_var_decl = 107,                 /* var_decl  */
  YYSYMBOL_statement = 108,                /* statement  */
  YYSYMBOL_errors = 109,                   /* errors  */
  YYSYMBOL_assignment = 110,               /* assignment  */
  YYSYMBOL_111_6 = 111,                    /* $@6  */
  YYSYMBOL_shorthand_assignment_operator = 112, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 113,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 114,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 115, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 116,         /* range_expression  */
  YYSYMBOL_117_7 = 117,                    /* $@7  */
  YYSYMBOL_range_increment = 118,          /* range_increment  */
  YYSYMBOL_increment_expression = 119,     /* increment_expression  */
  YYSYMBOL_s_bterm = 120,                  /* s_bterm  */
  YYSYMBOL_binary_expression = 121,        /* binary_expression  */
  YYSYMBOL_condition = 122,                /* condition  */
  YYSYMBOL_value = 123,                    /* value  */
  YYSYMBOL_map_literal = 124,              /* map_literal  */
  YYSYMBOL_125_8 = 125,                    /* $@8  */
  YYSYMBOL_map_associates = 126,           /* map_associates  */
  YYSYMBOL_map_associate = 127,            /* map_associate  */
  YYSYMBOL_array_literal = 128,            /* array_literal  */
  YYSYMBOL_129_9 = 129,                    /* $@9  */
  YYSYMBOL_130_10 = 130,                   /* $@10  */
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
  YYSYMBOL_147_11 = 147,                   /* $@11  */
  YYSYMBOL_switch_expression = 148,        /* switch_expression  */
  YYSYMBOL_149_12 = 149,                   /* $@12  */
  YYSYMBOL_150_13 = 150,                   /* $@13  */
  YYSYMBOL_foreach = 151,                  /* foreach  */
  YYSYMBOL_152_14 = 152,                   /* $@14  */
  YYSYMBOL_153_15 = 153,                   /* $@15  */
  YYSYMBOL_while = 154,                    /* while  */
  YYSYMBOL_155_16 = 155,                   /* $@16  */
  YYSYMBOL_156_17 = 156,                   /* $@17  */
  YYSYMBOL_local_block = 157,              /* local_block  */
  YYSYMBOL_closing_brace = 158,            /* closing_brace  */
  YYSYMBOL_if_expression = 159,            /* if_expression  */
  YYSYMBOL_160_18 = 160,                   /* $@18  */
  YYSYMBOL_161_19 = 161,                   /* $@19  */
  YYSYMBOL_else_clause = 162,              /* else_clause  */
  YYSYMBOL_163_20 = 163,                   /* $@20  */
  YYSYMBOL_stmntlist = 164,                /* stmntlist  */
  YYSYMBOL_165_21 = 165,                   /* $@21  */
  YYSYMBOL_closing_paren = 166,            /* closing_paren  */
  YYSYMBOL_call_expression = 167,          /* call_expression  */
  YYSYMBOL_168_22 = 168,                   /* @22  */
  YYSYMBOL_169_23 = 169,                   /* @23  */
  YYSYMBOL_function_id_or_pointer = 170,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 171,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 172,                /* parameter  */
  YYSYMBOL_type_cast = 173,                /* type_cast  */
  YYSYMBOL_type_name = 174,                /* type_name  */
  YYSYMBOL_array_dimension = 175,          /* array_dimension  */
  YYSYMBOL_boolean_literal = 176,          /* boolean_literal  */
  YYSYMBOL_float_literal = 177,            /* float_literal  */
  YYSYMBOL_integer_literal = 178,          /* integer_literal  */
  YYSYMBOL_character_literal = 179,        /* character_literal  */
  YYSYMBOL_simple_literal = 180            /* simple_literal  */
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
#define YYLAST   1093

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  104
/* YYNRULES -- Number of rules.  */
#define YYNRULES  219
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  337

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   310


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
       2,     2,     2,    74,     2,     2,     2,    66,    54,     2,
      70,    71,    64,    62,    69,    63,     2,    65,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    75,    68,
      58,     2,    59,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    67,     2,    73,    56,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    72,    55,    76,    57,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    60,
      61
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   168,   168,   174,   168,   178,   181,   186,   187,   189,
     194,   195,   196,   197,   201,   203,   204,   206,   207,   211,
     214,   215,   217,   217,   217,   231,   234,   240,   241,   242,
     249,   256,   257,   267,   270,   276,   277,   279,   279,   285,
     287,   292,   293,   296,   298,   300,   307,   328,   330,   331,
     333,   340,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   362,   365,   371,   372,
     374,   379,   384,   384,   393,   394,   395,   396,   397,   399,
     401,   402,   405,   406,   408,   409,   409,   411,   412,   415,
     417,   419,   420,   423,   424,   428,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     461,   462,   463,   464,   471,   472,   476,   480,   485,   486,
     492,   498,   504,   504,   509,   511,   514,   518,   523,   523,
     527,   527,   532,   533,   536,   540,   545,   546,   547,   548,
     550,   551,   562,   565,   569,   573,   574,   576,   577,   579,
     580,   582,   584,   589,   592,   595,   601,   606,   611,   611,
     626,   627,   626,   638,   640,   638,   651,   653,   651,   664,
     664,   667,   669,   672,   675,   672,   682,   686,   686,   693,
     694,   694,   701,   702,   704,   704,   715,   715,   727,   733,
     737,   743,   745,   746,   747,   751,   753,   755,   756,   758,
     759,   763,   766,   771,   776,   781,   786,   794,   802,   810
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
  "T_STATIC", "T_FOR", "T_TYPE_IDENTIFIER", "T_IDENT", "T_NUM",
  "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER", "T_CONST",
  "T_TRUE", "T_FALSE", "T_TBOOLEAN", "T_FUNC", "T_MACRO", "T_DOTDOT",
  "T_PLUSPLUS", "T_MINUSMINUS", "T_IF", "T_ELIF", "T_ELSE", "T_WHILE",
  "T_GOTO", "T_SH_ASSIGN_PLUS", "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT",
  "T_SH_ASSIGN_DIV", "T_SH_ASSIGN_MOD", "T_LOCAL", "T_SWITCH", "T_CASE",
  "T_DEFAULT", "T_BREAK", "T_CONTINUE", "T_RETURN", "T_VARIABLE",
  "T_ASSOC_ARROW", "T_NMATCH", "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT",
  "T_SHIFT_RIGHT", "T_OR", "T_AND", "T_LE", "T_GE", "T_POWER_TO", "T_VOID",
  "T_CMDSEQPREFIX", "'&'", "'|'", "'^'", "'~'", "'<'", "'>'", "T_LT",
  "T_GT", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "';'", "','", "'('",
  "')'", "'{'", "']'", "'!'", "':'", "'}'", "$accept", "file_structure",
  "$@1", "$@2", "namespace", "prerequisites", "require",
  "global_declarations", "global_var", "scope", "macro_definitions",
  "constants", "constant_list", "constdef", "$@3", "$@4",
  "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "$@5",
  "optional_description", "parameter_list", "non_empty_pardecl",
  "par_decl", "macro_definition", "block", "var_decls", "var_decl",
  "statement", "errors", "assignment", "$@6",
  "shorthand_assignment_operator", "assignment_expression",
  "simple_expression", "binary_expression_or_range", "range_expression",
  "$@7", "range_increment", "increment_expression", "s_bterm",
  "binary_expression", "condition", "value", "map_literal", "$@8",
  "map_associates", "map_associate", "array_literal", "$@9", "$@10",
  "array_elements", "array_element", "simple_array_element", "string",
  "break", "continue", "return_expression", "optional_bterm", "opt_num",
  "case_clauses", "case_clause", "case_selector", "case_condition",
  "label", "goto", "in_clause", "$@11", "switch_expression", "$@12",
  "$@13", "foreach", "$@14", "$@15", "while", "$@16", "$@17",
  "local_block", "closing_brace", "if_expression", "$@18", "$@19",
  "else_clause", "$@20", "stmntlist", "$@21", "closing_paren",
  "call_expression", "@22", "@23", "function_id_or_pointer",
  "nonempty_parameters", "parameter", "type_cast", "type_name",
  "array_dimension", "boolean_literal", "float_literal", "integer_literal",
  "character_literal", "simple_literal", YY_NULLPTR
  };
  return yy_sname[yysymbol];
}
#endif

#define YYPACT_NINF (-263)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-201)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -263,   110,    30,  -263,   106,   121,    60,   118,  -263,   121,
    -263,    62,    33,  -263,  -263,    81,  -263,   107,  -263,   128,
    -263,  -263,   -20,  -263,    23,  -263,  -263,  -263,   107,    73,
    -263,    23,  -263,  -263,  -263,  -263,  -263,  -263,    99,  -263,
      70,    11,  -263,     7,  -263,   -13,  -263,   162,   128,  -263,
    -263,  -263,    77,    35,    89,  -263,   272,  -263,    82,  -263,
    -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,
    -263,   131,   128,   513,  -263,  -263,  -263,  -263,     0,  -263,
      84,   133,   725,   725,   757,    17,    90,   588,  -263,   725,
     138,  -263,  -263,  -263,   864,  -263,  -263,  -263,   170,  -263,
     124,   834,  -263,  -263,  -263,   126,  -263,    41,   127,  -263,
     174,  -263,   189,   189,   349,  -263,    -1,  -263,  -263,  -263,
      13,   132,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,
    -263,  -263,   125,   134,  -263,   126,   139,   135,  -263,  -263,
     136,  -263,    23,  -263,  1026,    94,  -263,    94,  -263,  -263,
     141,   197,   891,   145,   207,   949,  -263,   725,   725,   725,
     725,   725,   725,   725,   725,   725,   725,   725,   725,   725,
     725,   725,   725,   725,   725,   725,   725,   725,   725,   614,
    -263,  -263,   150,  -263,  -263,   151,  -263,   725,  -263,   127,
    -263,   153,  -263,   158,   163,   949,  -263,  -263,  -263,   112,
     225,  -263,   131,  -263,  -263,  -263,  -263,  -263,  -263,   221,
     128,   547,  -263,   513,  -263,  -263,  -263,  -263,  -263,  -263,
      -4,  -263,  -263,  -263,  -263,   194,   -25,  -263,   949,  1026,
     812,   812,     9,     9,   976,   976,   949,   949,   -14,  1003,
    1003,  1003,  1026,   179,   179,    94,    94,   -14,   -14,   -14,
     165,  -263,   864,   513,   128,  -263,   920,   800,  -263,   112,
    -263,  -263,  -263,   167,  -263,  -263,  -263,  -263,  -263,   655,
    -263,  -263,  -263,  -263,  -263,    38,   197,  -263,   176,   207,
    -263,   216,  -263,    38,    23,   800,  -263,   426,  -263,  -263,
     800,   177,   138,  -263,  -263,  -263,  -263,  -263,  -263,  -263,
    -263,   725,  -263,  -263,  -263,  -263,   128,   125,   212,  -263,
    -263,  -263,    18,   172,  -263,  -263,  -263,   178,   681,   800,
      43,   187,  -263,   108,   190,   173,    43,   474,  -263,  -263,
    -263,   192,  -263,  -263,  -263,  -263,  -263
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     5,     1,     0,     7,     0,     0,    10,     7,
       6,     0,     3,     8,     9,    16,    22,    15,    12,     0,
      11,    13,     0,    20,     0,    16,    37,     4,    15,   207,
      14,     0,    19,    22,    27,    29,    30,    31,     0,    28,
       0,     0,    18,     0,   208,     0,    21,     0,    48,    46,
      36,    35,     0,     0,     0,   209,     0,    51,     0,   214,
     213,    26,   215,   211,   212,    23,   219,   217,   216,   218,
      25,     0,    48,     0,    33,    34,   210,   200,   131,   150,
       0,   126,     0,     0,     0,     0,   140,     0,   132,     0,
       0,    79,    81,    93,    80,    91,   129,   130,   127,    92,
       0,     0,   128,    50,    24,    68,   173,    32,     0,   176,
       0,   170,   157,   157,     0,    52,    72,    63,    66,   190,
       0,     0,    64,    59,    60,    61,    65,    62,    58,    57,
      56,    55,   181,     0,    49,   204,     0,    42,    44,   205,
     201,   202,     0,   194,    98,    99,    95,   100,    32,   198,
       0,     0,     0,     0,     0,    97,   123,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     151,   196,   131,   126,    94,     0,   166,     0,   183,     0,
     167,     0,   158,     0,     0,   156,   154,    90,    89,     0,
       0,    70,     0,    69,    67,    54,   182,    47,    53,    39,
       0,     0,    45,     0,   139,   146,   145,   147,   148,   149,
       0,   143,   144,   122,   206,     0,   181,   134,    85,   102,
     121,   120,   111,   112,   115,   114,   118,   119,   113,   103,
     104,   106,   101,   116,   117,   105,   107,   108,   109,   110,
       0,    83,    82,     0,     0,   174,   125,     0,   177,     0,
     152,   153,   131,     0,    77,    78,    74,    75,    76,     0,
     191,    40,    38,    43,   203,     0,     0,   141,     0,     0,
     133,    87,    96,     0,     0,     0,   124,     0,   179,   184,
       0,     0,     0,    73,   193,   192,   195,   142,   137,   136,
     135,     0,    86,   197,   168,   175,    48,   181,   186,   178,
     171,    71,    88,     0,   180,   187,   185,     0,     0,     0,
     159,     0,   188,     0,     0,     0,   159,     0,   169,   164,
     165,     0,   163,   172,   160,   161,   162
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -263,  -263,  -263,  -263,  -263,   253,  -263,  -263,  -263,   256,
     242,  -263,  -263,   238,  -263,  -263,  -263,   -27,   -24,  -263,
    -263,  -263,  -263,  -263,  -189,  -263,    64,  -263,   235,   -67,
     -18,  -240,  -263,  -263,  -263,  -263,   -79,   -54,  -263,    97,
    -263,  -263,  -263,   184,   -74,   101,  -177,  -263,  -263,  -263,
      14,  -263,  -263,  -263,  -263,    16,  -263,  -263,  -263,  -263,
    -263,  -263,   186,   -26,  -263,  -263,  -263,  -263,  -263,  -263,
    -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,  -263,
    -262,  -199,  -263,  -263,  -263,  -263,  -263,  -181,  -263,    19,
     -68,  -263,  -263,  -263,  -263,    93,  -263,   -61,  -263,  -263,
    -263,  -263,  -263,   -41
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    17,     5,     8,     9,    12,    18,    26,
      27,    20,    22,    23,    24,   104,    65,    38,    90,    52,
      53,    40,    41,   272,   136,   137,   138,    28,   117,    71,
     118,   119,   120,   121,   200,   269,    58,   139,   250,    92,
     281,   302,   122,    93,    94,   188,    95,    96,   154,   226,
     227,    97,   150,   151,   220,   221,   222,    98,   123,   124,
     125,   196,   193,   325,   326,   327,   331,   126,   127,   255,
     313,   128,   191,   317,   129,   185,   285,   130,   189,   290,
     289,   207,   131,   257,   308,   316,   319,   132,   202,   296,
      99,   213,   253,   100,   140,   141,   101,    31,    44,    66,
      67,    68,    69,   102
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      39,    30,    91,   133,    45,   134,    70,    39,   144,   145,
     147,   156,   142,   152,   203,   155,    54,   288,    50,   197,
     198,   270,   263,   305,   275,   148,   153,   280,   309,    56,
      72,    34,    35,     4,    36,   165,   166,   201,    15,   294,
     195,    56,   -32,    74,   279,   288,    16,   116,    32,    33,
     288,   206,    75,   179,    72,    57,    37,   322,   165,   166,
     167,   149,    37,    51,   283,   276,   199,   165,   166,   277,
    -199,   174,   175,   176,   177,   178,   179,   323,   324,   288,
      55,   204,   291,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   252,   307,   -17,   314,   295,
       3,  -199,    25,   256,     6,   212,   186,   329,    39,   330,
     262,    59,    60,    79,    62,     7,    63,    64,    10,    11,
      14,  -189,   105,    21,   133,    29,  -189,   106,    29,   107,
      43,    47,    48,   165,   166,   167,   335,    73,    80,   142,
     103,   183,   142,   108,   143,  -189,   109,   110,   176,   177,
     178,   179,    76,  -138,   111,  -189,  -189,   112,   113,   114,
      37,    59,    60,    61,    62,   -31,    63,    64,   116,    86,
      56,   180,   190,  -189,    88,    59,    60,   298,    62,   133,
      63,    64,   142,   284,   181,    85,  -200,   187,   192,   115,
     205,   206,   208,    48,   210,   211,   215,  -189,   216,   217,
     209,   218,   219,   311,   214,   293,   224,   133,   225,   133,
    -199,   254,   133,   259,   161,   162,   260,   312,   165,   166,
     167,   261,   271,   116,   278,   301,   315,   299,   282,   134,
     292,   174,   175,   176,   177,   178,   179,   318,   310,   333,
     320,   133,   264,   265,   266,   267,   268,   304,   328,   133,
      39,   116,    13,   116,   321,   332,   116,   336,    19,   306,
      42,    46,   -84,    77,   273,    49,   251,   -84,   -84,   -84,
      78,    59,    60,    79,    62,   184,    63,    64,    72,    80,
     258,   -84,   297,   300,   -84,   116,   -84,   -84,   -84,   194,
     334,     0,   303,   116,   274,   -84,   -84,   -84,   -84,   -84,
     -84,    81,     0,   -84,     0,   -84,   -84,   -84,   -84,   -84,
     -84,   -84,   -84,   -84,   -84,     0,   -84,   -84,   -84,    82,
     -84,   -84,     0,     0,    83,    84,    85,   -84,   -84,    86,
     -84,   -84,    87,   -84,    88,   -84,    89,     0,   -84,  -155,
      77,     0,     0,     0,  -155,  -155,  -155,    78,    59,    60,
      79,    62,     0,    63,    64,     0,    80,     0,     0,     0,
       0,  -155,     0,  -155,  -155,  -155,     0,     0,     0,     0,
       0,     0,  -155,  -155,  -155,  -155,  -155,  -155,    81,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -155,     0,     0,     0,     0,    82,     0,     0,     0,
       0,    83,    84,    85,     0,     0,    86,  -155,     0,    87,
       0,    88,     0,    89,     0,  -155,   -48,   105,     0,     0,
       0,   -48,   106,    29,   107,     0,     0,     0,     0,     0,
       0,     0,     0,    80,     0,     0,     0,     0,   108,     0,
     -48,   109,   110,     0,     0,     0,     0,     0,     0,   111,
     -48,   -48,   112,   113,   114,    37,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   105,     0,     0,   -48,     0,
     106,    29,   107,     0,     0,     0,     0,     0,     0,     0,
      85,    80,     0,     0,   115,     0,   108,     0,    48,   109,
     110,     0,   -48,     0,     0,     0,     0,   111,  -189,  -189,
     112,   113,   114,    37,   135,     0,     0,     0,     0,     0,
      29,    78,    59,    60,    79,    62,     0,    63,    64,     0,
      80,     0,     0,     0,     0,     0,     0,     0,    85,     0,
       0,     0,   115,     0,     0,     0,    48,     0,    77,     0,
    -189,     0,    81,     0,     0,    78,    59,    60,    79,    62,
       0,    63,    64,     0,    80,     0,     0,     0,     0,     0,
      82,     0,     0,     0,     0,    83,    84,    85,     0,     0,
      86,     0,   -84,    87,   -41,    88,    81,    89,     0,    77,
       0,     0,     0,     0,     0,    29,    78,    59,    60,    79,
      62,     0,    63,    64,    82,    80,     0,     0,     0,    83,
      84,    85,     0,     0,    86,    77,   -84,    87,   -84,    88,
       0,    89,    78,    59,    60,    79,    62,    81,    63,    64,
       0,    80,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    82,     0,     0,     0,     0,
      83,    84,    85,    81,     0,    86,    77,     0,    87,     0,
      88,     0,    89,    78,    59,    60,    79,    62,     0,    63,
      64,    82,    80,     0,     0,     0,    83,    84,    85,     0,
       0,    86,    77,     0,    87,     0,    88,   -84,    89,    78,
      59,    60,    79,    62,    81,    63,    64,     0,    80,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    82,     0,     0,     0,     0,    83,    84,    85,
      81,     0,    86,   -84,     0,    87,    77,    88,     0,    89,
       0,     0,     0,    78,    59,    60,    79,    62,    82,    63,
      64,     0,    80,    83,    84,    85,     0,     0,    86,     0,
       0,    87,   -84,    88,     0,    89,     0,     0,    77,     0,
       0,     0,     0,     0,    81,    78,   146,    60,    79,    62,
       0,    63,    64,     0,    80,     0,     0,     0,     0,     0,
       0,     0,    82,     0,     0,     0,     0,    83,    84,    85,
       0,     0,    86,     0,     0,    87,    81,    88,     0,    89,
       0,   105,     0,     0,     0,     0,   106,    29,   107,     0,
       0,     0,     0,     0,    82,     0,     0,    80,     0,    83,
      84,    85,   108,     0,    86,   109,   110,    87,     0,    88,
       0,    89,     0,   111,     0,    77,   112,   113,   114,    37,
       0,     0,   182,    59,    60,    79,    62,     0,    63,    64,
       0,    80,     0,   158,     0,  -201,  -201,   161,   162,     0,
       0,   165,   166,   167,    85,     0,     0,     0,   115,   171,
     172,   173,   287,   183,   174,   175,   176,   177,   178,   179,
       0,     0,     0,   157,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    85,     0,
       0,    86,     0,     0,     0,   158,    88,   159,   160,   161,
     162,   163,   164,   165,   166,   167,     0,     0,   168,   169,
     170,   171,   172,   173,     0,     0,   174,   175,   176,   177,
     178,   179,   158,     0,   159,   160,   161,   162,   163,   164,
     165,   166,   167,     0,     0,   168,   169,   170,   171,   172,
     173,     0,     0,   174,   175,   176,   177,   178,   179,     0,
       0,   158,   223,   159,   160,   161,   162,   163,   164,   165,
     166,   167,     0,     0,   168,   169,   170,   171,   172,   173,
       0,     0,   174,   175,   176,   177,   178,   179,     0,     0,
     158,   286,   159,   160,   161,   162,   163,   164,   165,   166,
     167,     0,     0,   168,   169,   170,   171,   172,   173,     0,
       0,   174,   175,   176,   177,   178,   179,   158,     0,   159,
     160,   161,   162,     0,     0,   165,   166,   167,     0,     0,
     168,   169,   170,   171,   172,   173,     0,     0,   174,   175,
     176,   177,   178,   179,   158,     0,   159,   160,   161,   162,
       0,     0,   165,   166,   167,     0,     0,     0,     0,     0,
     171,   172,   173,     0,     0,   174,   175,   176,   177,   178,
     179,   161,   162,     0,     0,   165,   166,   167,     0,     0,
       0,     0,     0,     0,   172,   173,     0,     0,   174,   175,
     176,   177,   178,   179
};

static const yytype_int16 yycheck[] =
{
      24,    19,    56,    71,    31,    72,    47,    31,    82,    83,
      84,    90,    73,    87,     1,    89,     9,   257,     7,    20,
      21,   202,   199,   285,   213,     8,    87,   226,   290,    42,
      48,     8,     9,     3,    11,    49,    50,   116,     5,     1,
     114,    42,    42,     8,    69,   285,    13,    71,    68,    69,
     290,    76,    17,    67,    72,    68,    39,   319,    49,    50,
      51,    85,    39,    52,   253,    69,    67,    49,    50,    73,
      70,    62,    63,    64,    65,    66,    67,    34,    35,   319,
      73,    68,   259,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   287,     0,   307,    71,
       0,    70,     5,   187,     8,   142,    75,     9,   142,    11,
       8,     9,    10,    11,    12,     4,    14,    15,    68,    11,
      68,     0,     1,    52,   202,     7,     5,     6,     7,     8,
      67,    42,    72,    49,    50,    51,   327,    70,    17,   210,
      68,    39,   213,    22,    70,    24,    25,    26,    64,    65,
      66,    67,    73,    73,    33,    34,    35,    36,    37,    38,
      39,     9,    10,    11,    12,    42,    14,    15,   202,    67,
      42,    11,     8,    52,    72,     9,    10,    11,    12,   257,
      14,    15,   253,   254,    70,    64,    70,    70,     9,    68,
      68,    76,    68,    72,    69,    69,     9,    76,    11,    12,
      71,    14,    15,   292,    73,   269,    71,   285,    11,   287,
      70,    70,   290,    70,    45,    46,    68,   301,    49,    50,
      51,    68,    11,   257,    40,    19,    24,   278,    73,   306,
      73,    62,    63,    64,    65,    66,    67,    75,    71,    76,
      72,   319,    27,    28,    29,    30,    31,   284,    71,   327,
     284,   285,     9,   287,   318,    75,   290,    75,    12,   287,
      28,    33,     0,     1,   210,    40,   179,     5,     6,     7,
       8,     9,    10,    11,    12,   101,    14,    15,   306,    17,
     189,    19,   276,   279,    22,   319,    24,    25,    26,   113,
     326,    -1,   283,   327,   211,    33,    34,    35,    36,    37,
      38,    39,    -1,    41,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    -1,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    -1,    76,     0,
       1,    -1,    -1,    -1,     5,     6,     7,     8,     9,    10,
      11,    12,    -1,    14,    15,    -1,    17,    -1,    -1,    -1,
      -1,    22,    -1,    24,    25,    26,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    34,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    -1,    -1,    57,    -1,    -1,    -1,
      -1,    62,    63,    64,    -1,    -1,    67,    68,    -1,    70,
      -1,    72,    -1,    74,    -1,    76,     0,     1,    -1,    -1,
      -1,     5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    22,    -1,
      24,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,    52,    -1,
       6,     7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      64,    17,    -1,    -1,    68,    -1,    22,    -1,    72,    25,
      26,    -1,    76,    -1,    -1,    -1,    -1,    33,    34,    35,
      36,    37,    38,    39,     1,    -1,    -1,    -1,    -1,    -1,
       7,     8,     9,    10,    11,    12,    -1,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,
      -1,    -1,    68,    -1,    -1,    -1,    72,    -1,     1,    -1,
      76,    -1,    39,    -1,    -1,     8,     9,    10,    11,    12,
      -1,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    -1,    62,    63,    64,    -1,    -1,
      67,    -1,    69,    70,    71,    72,    39,    74,    -1,     1,
      -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,    11,
      12,    -1,    14,    15,    57,    17,    -1,    -1,    -1,    62,
      63,    64,    -1,    -1,    67,     1,    69,    70,    71,    72,
      -1,    74,     8,     9,    10,    11,    12,    39,    14,    15,
      -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    57,    -1,    -1,    -1,    -1,
      62,    63,    64,    39,    -1,    67,     1,    -1,    70,    -1,
      72,    -1,    74,     8,     9,    10,    11,    12,    -1,    14,
      15,    57,    17,    -1,    -1,    -1,    62,    63,    64,    -1,
      -1,    67,     1,    -1,    70,    -1,    72,    73,    74,     8,
       9,    10,    11,    12,    39,    14,    15,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    57,    -1,    -1,    -1,    -1,    62,    63,    64,
      39,    -1,    67,    68,    -1,    70,     1,    72,    -1,    74,
      -1,    -1,    -1,     8,     9,    10,    11,    12,    57,    14,
      15,    -1,    17,    62,    63,    64,    -1,    -1,    67,    -1,
      -1,    70,    71,    72,    -1,    74,    -1,    -1,     1,    -1,
      -1,    -1,    -1,    -1,    39,     8,     9,    10,    11,    12,
      -1,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    57,    -1,    -1,    -1,    -1,    62,    63,    64,
      -1,    -1,    67,    -1,    -1,    70,    39,    72,    -1,    74,
      -1,     1,    -1,    -1,    -1,    -1,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    57,    -1,    -1,    17,    -1,    62,
      63,    64,    22,    -1,    67,    25,    26,    70,    -1,    72,
      -1,    74,    -1,    33,    -1,     1,    36,    37,    38,    39,
      -1,    -1,     8,     9,    10,    11,    12,    -1,    14,    15,
      -1,    17,    -1,    41,    -1,    43,    44,    45,    46,    -1,
      -1,    49,    50,    51,    64,    -1,    -1,    -1,    68,    57,
      58,    59,    72,    39,    62,    63,    64,    65,    66,    67,
      -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    -1,
      -1,    67,    -1,    -1,    -1,    41,    72,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    -1,    -1,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    64,    65,
      66,    67,    41,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    -1,    -1,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    63,    64,    65,    66,    67,    -1,
      -1,    41,    71,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    -1,    -1,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    64,    65,    66,    67,    -1,    -1,
      41,    71,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    -1,    -1,    54,    55,    56,    57,    58,    59,    -1,
      -1,    62,    63,    64,    65,    66,    67,    41,    -1,    43,
      44,    45,    46,    -1,    -1,    49,    50,    51,    -1,    -1,
      54,    55,    56,    57,    58,    59,    -1,    -1,    62,    63,
      64,    65,    66,    67,    41,    -1,    43,    44,    45,    46,
      -1,    -1,    49,    50,    51,    -1,    -1,    -1,    -1,    -1,
      57,    58,    59,    -1,    -1,    62,    63,    64,    65,    66,
      67,    45,    46,    -1,    -1,    49,    50,    51,    -1,    -1,
      -1,    -1,    -1,    -1,    58,    59,    -1,    -1,    62,    63,
      64,    65,    66,    67
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    78,    79,     0,     3,    81,     8,     4,    82,    83,
      68,    11,    84,    82,    68,     5,    13,    80,    85,    86,
      88,    52,    89,    90,    91,     5,    86,    87,   104,     7,
     107,   174,    68,    69,     8,     9,    11,    39,    94,    95,
      98,    99,    87,    67,   175,    94,    90,    42,    72,   105,
       7,    52,    96,    97,     9,    73,    42,    68,   113,     9,
      10,    11,    12,    14,    15,    93,   176,   177,   178,   179,
     180,   106,   107,    70,     8,    17,    73,     1,     8,    11,
      17,    39,    57,    62,    63,    64,    67,    70,    72,    74,
      95,   114,   116,   120,   121,   123,   124,   128,   134,   167,
     170,   173,   180,    68,    92,     1,     6,     8,    22,    25,
      26,    33,    36,    37,    38,    68,    95,   105,   107,   108,
     109,   110,   119,   135,   136,   137,   144,   145,   148,   151,
     154,   159,   164,   167,   106,     1,   101,   102,   103,   114,
     171,   172,   174,    70,   121,   121,     9,   121,     8,    95,
     129,   130,   121,   174,   125,   121,   113,    19,    41,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    54,    55,
      56,    57,    58,    59,    62,    63,    64,    65,    66,    67,
      11,    70,     8,    39,   120,   152,    75,    70,   122,   155,
       8,   149,     9,   139,   139,   121,   138,    20,    21,    67,
     111,   113,   165,     1,    68,    68,    76,   158,    68,    71,
      69,    69,    94,   168,    73,     9,    11,    12,    14,    15,
     131,   132,   133,    71,    71,    11,   126,   127,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     115,   116,   121,   169,    70,   146,   121,   160,   122,    70,
      68,    68,     8,   123,    27,    28,    29,    30,    31,   112,
     164,    11,   100,   103,   172,   101,    69,    73,    40,    69,
     158,   117,    73,   101,   174,   153,    71,    72,   108,   157,
     156,   123,    73,   114,     1,    71,   166,   132,    11,   180,
     127,    19,   118,   166,    94,   157,   107,   164,   161,   157,
      71,   113,   121,   147,   158,    24,   162,   150,    75,   163,
      72,   114,   157,    34,    35,   140,   141,   142,    71,     9,
      11,   143,    75,    76,   140,   164,    75
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    77,    79,    80,    78,    81,    81,    82,    82,    83,
      84,    84,    84,    84,    85,    86,    86,    87,    87,    88,
      89,    89,    91,    92,    90,    93,    93,    94,    94,    94,
      94,    95,    95,    96,    96,    97,    97,    99,    98,   100,
     100,   101,   101,   102,   102,   103,   104,   105,   106,   106,
     107,   107,   108,   108,   108,   108,   108,   108,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   109,   109,
     110,   110,   111,   110,   112,   112,   112,   112,   112,   113,
     114,   114,   115,   115,   116,   117,   116,   118,   118,   119,
     119,   120,   120,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   121,   121,   121,   121,   121,   121,
     121,   121,   121,   121,   122,   122,   123,   123,   123,   123,
     123,   123,   125,   124,   126,   126,   127,   127,   129,   128,
     130,   128,   131,   131,   132,   132,   133,   133,   133,   133,
     134,   134,   135,   136,   137,   138,   138,   139,   139,   140,
     140,   141,   142,   142,   143,   143,   144,   145,   147,   146,
     149,   150,   148,   152,   153,   151,   155,   156,   154,   157,
     157,   158,   158,   160,   161,   159,   162,   163,   162,   164,
     165,   164,   166,   166,   168,   167,   169,   167,   170,   170,
     170,   101,   171,   171,   171,   172,   173,   174,   174,   175,
     175,   176,   176,   177,   178,   179,   180,   180,   180,   180
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     6,     0,     3,     0,     2,     3,
       0,     2,     2,     3,     2,     0,     1,     0,     2,     3,
       1,     3,     0,     0,     5,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     1,     0,     6,     0,
       1,     0,     1,     3,     1,     2,     3,     4,     0,     2,
       4,     3,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
       2,     5,     0,     4,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     0,     0,     5,     0,     2,     2,
       2,     1,     1,     1,     2,     2,     4,     2,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     3,     2,     1,     1,     1,     1,
       1,     1,     0,     4,     1,     3,     3,     3,     0,     3,
       0,     4,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     3,     3,     2,     0,     1,     0,     1,     0,
       2,     2,     3,     2,     1,     1,     2,     2,     0,     7,
       0,     0,     9,     0,     0,     5,     0,     0,     5,     1,
       3,     0,     1,     0,     0,     6,     0,     0,     3,     0,
       0,     3,     1,     1,     0,     5,     0,     5,     2,     1,
       1,     1,     1,     3,     1,     1,     3,     1,     2,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1
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
#line 168 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2023 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 174 "Parser.y"
                                                                    {
				finalize_header();
			}
#line 2031 "parser.c"
    break;

  case 5: /* namespace: %empty  */
#line 178 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2039 "parser.c"
    break;

  case 6: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 181 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2048 "parser.c"
    break;

  case 9: /* require: T_REQUIRE T_STRING ';'  */
#line 189 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2057 "parser.c"
    break;

  case 13: /* global_declarations: global_declarations T_STATIC T_VOID  */
#line 197 "Parser.y"
                                                              {
				yyerror("Invalid global declaration. Note: 1st macro definition must be globally accessible.");
			}
#line 2065 "parser.c"
    break;

  case 15: /* scope: %empty  */
#line 203 "Parser.y"
                { _macroScope = MS_GLOBAL; }
#line 2071 "parser.c"
    break;

  case 16: /* scope: T_STATIC  */
#line 204 "Parser.y"
                                   { _macroScope = MS_LOCAL; }
#line 2077 "parser.c"
    break;

  case 22: /* $@3: %empty  */
#line 217 "Parser.y"
                { _bDefiningConst = 1; }
#line 2083 "parser.c"
    break;

  case 23: /* $@4: %empty  */
#line 217 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2089 "parser.c"
    break;

  case 24: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 218 "Parser.y"
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
#line 2105 "parser.c"
    break;

  case 25: /* constant_literal: simple_literal  */
#line 231 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2113 "parser.c"
    break;

  case 26: /* constant_literal: T_STRING  */
#line 234 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2122 "parser.c"
    break;

  case 27: /* variable_identifier: T_IDENT  */
#line 240 "Parser.y"
                                                                {	yyval.ident = yyvsp[0].ident; }
#line 2128 "parser.c"
    break;

  case 28: /* variable_identifier: variable_reference  */
#line 241 "Parser.y"
                                                {	yyval.ident = yyvsp[0].ident; }
#line 2134 "parser.c"
    break;

  case 29: /* variable_identifier: T_NUM  */
#line 243 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2145 "parser.c"
    break;

  case 30: /* variable_identifier: T_STRING  */
#line 250 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2154 "parser.c"
    break;

  case 31: /* variable_reference: T_VARIABLE  */
#line 256 "Parser.y"
                                                        {	yyval.ident = yyvsp[0].ident; }
#line 2160 "parser.c"
    break;

  case 32: /* variable_reference: T_IDENT  */
#line 257 "Parser.y"
                                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0});
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2172 "parser.c"
    break;

  case 33: /* macro_declaration: macro_type T_IDENT  */
#line 267 "Parser.y"
                                      {
				yyval.ident = yyvsp[0].ident;
			}
#line 2180 "parser.c"
    break;

  case 34: /* macro_declaration: macro_type T_FUNC  */
#line 270 "Parser.y"
                                            {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2190 "parser.c"
    break;

  case 37: /* $@5: %empty  */
#line 279 "Parser.y"
            { init_macroDefinition(); }
#line 2196 "parser.c"
    break;

  case 38: /* macrostart: $@5 macro_declaration '(' parameter_list ')' optional_description  */
#line 280 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2204 "parser.c"
    break;

  case 40: /* optional_description: T_STRING  */
#line 287 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2213 "parser.c"
    break;

  case 45: /* par_decl: type_name variable_identifier  */
#line 300 "Parser.y"
                                              {
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER,yyvsp[-1].type,_nparam);
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2224 "parser.c"
    break;

  case 46: /* macro_definition: scope macrostart block  */
#line 307 "Parser.y"
                                         {
				bytecode_generateAutoLabelNamePrefix(lreturnid,0);
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
#line 2249 "parser.c"
    break;

  case 50: /* var_decl: type_name variable_identifier assignment_expression ';'  */
#line 333 "Parser.y"
                                                                        {
				makeInternalSymbol(yyvsp[-2].ident.s, yyvsp[-3].type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-2].ident.s);
				if (yyvsp[-2].ident.stringIsAlloced) {
					free(yyvsp[-2].ident.s);
				}
				vname_count = 0;
			}
#line 2262 "parser.c"
    break;

  case 51: /* var_decl: type_name variable_identifier ';'  */
#line 340 "Parser.y"
                                                                {
				makeInternalSymbol(yyvsp[-1].ident.s, yyvsp[-2].type, 0);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				vname_count = 0;
			}
#line 2274 "parser.c"
    break;

  case 53: /* statement: call_expression ';'  */
#line 349 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2280 "parser.c"
    break;

  case 54: /* statement: assignment ';'  */
#line 350 "Parser.y"
                                         { yyval.needsPop = 1; }
#line 2286 "parser.c"
    break;

  case 55: /* statement: if_expression  */
#line 351 "Parser.y"
                                        { yyval.needsPop = 1; }
#line 2292 "parser.c"
    break;

  case 56: /* statement: while  */
#line 352 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2298 "parser.c"
    break;

  case 57: /* statement: foreach  */
#line 353 "Parser.y"
                                  { yyval.needsPop = 1; }
#line 2304 "parser.c"
    break;

  case 58: /* statement: switch_expression  */
#line 354 "Parser.y"
                                            { yyval.needsPop = 1; }
#line 2310 "parser.c"
    break;

  case 59: /* statement: break  */
#line 355 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2316 "parser.c"
    break;

  case 60: /* statement: continue  */
#line 356 "Parser.y"
                                   { yyval.needsPop = 0; }
#line 2322 "parser.c"
    break;

  case 61: /* statement: return_expression  */
#line 357 "Parser.y"
                                            { yyval.needsPop = 0; }
#line 2328 "parser.c"
    break;

  case 62: /* statement: goto  */
#line 358 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2334 "parser.c"
    break;

  case 63: /* statement: block  */
#line 359 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2340 "parser.c"
    break;

  case 64: /* statement: increment_expression  */
#line 360 "Parser.y"
                                               { yyval.needsPop = 1; }
#line 2346 "parser.c"
    break;

  case 65: /* statement: label  */
#line 361 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2352 "parser.c"
    break;

  case 66: /* statement: var_decl  */
#line 362 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2360 "parser.c"
    break;

  case 67: /* statement: errors ';'  */
#line 365 "Parser.y"
                                     {
				yyval.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2370 "parser.c"
    break;

  case 70: /* assignment: variable_reference assignment_expression  */
#line 374 "Parser.y"
                                                     {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
				vname_count = 0;
			}
#line 2379 "parser.c"
    break;

  case 71: /* assignment: variable_reference '[' value ']' assignment_expression  */
#line 379 "Parser.y"
                                                                               {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)yyvsp[-4].ident.s });
				vname_count = 0;
			}
#line 2388 "parser.c"
    break;

  case 72: /* $@6: %empty  */
#line 384 "Parser.y"
                                           {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 2396 "parser.c"
    break;

  case 73: /* assignment: variable_reference $@6 shorthand_assignment_operator simple_expression  */
#line 386 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-3].ident.s);
				vname_count = 0;
			}
#line 2406 "parser.c"
    break;

  case 74: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 393 "Parser.y"
                                         { yyval.type = BIN_MUL; }
#line 2412 "parser.c"
    break;

  case 75: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 394 "Parser.y"
                                          { yyval.type = BIN_DIV; }
#line 2418 "parser.c"
    break;

  case 76: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 395 "Parser.y"
                                          { yyval.type = BIN_MOD; }
#line 2424 "parser.c"
    break;

  case 77: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 396 "Parser.y"
                                           { yyval.type = BIN_ADD; }
#line 2430 "parser.c"
    break;

  case 78: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 397 "Parser.y"
                                            { yyval.type = BIN_SUB; }
#line 2436 "parser.c"
    break;

  case 79: /* assignment_expression: T_ASSIGN simple_expression  */
#line 399 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2442 "parser.c"
    break;

  case 80: /* simple_expression: binary_expression  */
#line 401 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2448 "parser.c"
    break;

  case 85: /* $@7: %empty  */
#line 409 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2454 "parser.c"
    break;

  case 88: /* range_increment: T_DOTDOT binary_expression  */
#line 412 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2460 "parser.c"
    break;

  case 89: /* increment_expression: variable_reference T_MINUSMINUS  */
#line 415 "Parser.y"
                                                         { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, -1); }
#line 2466 "parser.c"
    break;

  case 90: /* increment_expression: variable_reference T_PLUSPLUS  */
#line 417 "Parser.y"
                                                      { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, 1); }
#line 2472 "parser.c"
    break;

  case 91: /* s_bterm: value  */
#line 419 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2478 "parser.c"
    break;

  case 93: /* binary_expression: s_bterm  */
#line 423 "Parser.y"
                                 { yyval.type = yyvsp[0].type;	}
#line 2484 "parser.c"
    break;

  case 94: /* binary_expression: type_cast s_bterm  */
#line 424 "Parser.y"
                                                        {
				yyval.type = yyvsp[-1].type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, yyval.type);
			}
#line 2493 "parser.c"
    break;

  case 95: /* binary_expression: '-' T_NUM  */
#line 428 "Parser.y"
                                    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-yyvsp[0].num);
				} else {
					yyval.type = VT_NUMBER;
					yyval.v.type = C_PUSH_LONG_LITERAL; 
					yyval.v.data.longValue  = -yyvsp[0].num;
				}
			}
#line 2507 "parser.c"
    break;

  case 96: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 437 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2513 "parser.c"
    break;

  case 97: /* binary_expression: '!' binary_expression  */
#line 438 "Parser.y"
                                                { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2519 "parser.c"
    break;

  case 98: /* binary_expression: '~' binary_expression  */
#line 439 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2525 "parser.c"
    break;

  case 99: /* binary_expression: '+' binary_expression  */
#line 440 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2531 "parser.c"
    break;

  case 100: /* binary_expression: '-' binary_expression  */
#line 441 "Parser.y"
                                                { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &yyvsp[0].v, -1); }
#line 2537 "parser.c"
    break;

  case 101: /* binary_expression: binary_expression '~' binary_expression  */
#line 442 "Parser.y"
                                                                    { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2543 "parser.c"
    break;

  case 102: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 443 "Parser.y"
                                                                         { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2549 "parser.c"
    break;

  case 103: /* binary_expression: binary_expression '&' binary_expression  */
#line 444 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2555 "parser.c"
    break;

  case 104: /* binary_expression: binary_expression '|' binary_expression  */
#line 445 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2561 "parser.c"
    break;

  case 105: /* binary_expression: binary_expression '+' binary_expression  */
#line 446 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2567 "parser.c"
    break;

  case 106: /* binary_expression: binary_expression '^' binary_expression  */
#line 447 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2573 "parser.c"
    break;

  case 107: /* binary_expression: binary_expression '-' binary_expression  */
#line 448 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2579 "parser.c"
    break;

  case 108: /* binary_expression: binary_expression '*' binary_expression  */
#line 449 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2585 "parser.c"
    break;

  case 109: /* binary_expression: binary_expression '/' binary_expression  */
#line 450 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2591 "parser.c"
    break;

  case 110: /* binary_expression: binary_expression '%' binary_expression  */
#line 451 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2597 "parser.c"
    break;

  case 111: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 452 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2603 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 453 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2609 "parser.c"
    break;

  case 113: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 454 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2615 "parser.c"
    break;

  case 114: /* binary_expression: binary_expression T_AND binary_expression  */
#line 455 "Parser.y"
                                                                    { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2621 "parser.c"
    break;

  case 115: /* binary_expression: binary_expression T_OR binary_expression  */
#line 456 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2627 "parser.c"
    break;

  case 116: /* binary_expression: binary_expression '<' binary_expression  */
#line 457 "Parser.y"
                                                                  { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2633 "parser.c"
    break;

  case 117: /* binary_expression: binary_expression '>' binary_expression  */
#line 458 "Parser.y"
                                                                  { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2639 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression T_LE binary_expression  */
#line 459 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2645 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression T_GE binary_expression  */
#line 460 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2651 "parser.c"
    break;

  case 120: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 461 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2657 "parser.c"
    break;

  case 121: /* binary_expression: binary_expression T_NE binary_expression  */
#line 462 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2663 "parser.c"
    break;

  case 122: /* binary_expression: '(' binary_expression ')'  */
#line 463 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2669 "parser.c"
    break;

  case 123: /* binary_expression: variable_reference assignment_expression  */
#line 464 "Parser.y"
                                                                   {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
				vname_count = 0;
			}
#line 2678 "parser.c"
    break;

  case 125: /* condition: '(' binary_expression  */
#line 472 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2686 "parser.c"
    break;

  case 126: /* value: T_VARIABLE  */
#line 476 "Parser.y"
                           {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.type = yyvsp[0].ident.type;
			}
#line 2695 "parser.c"
    break;

  case 127: /* value: string  */
#line 480 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2705 "parser.c"
    break;

  case 128: /* value: simple_literal  */
#line 485 "Parser.y"
                                         {	yyval.type = yyvsp[0].type;	}
#line 2711 "parser.c"
    break;

  case 129: /* value: map_literal  */
#line 486 "Parser.y"
                                      {
				yyval.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2722 "parser.c"
    break;

  case 130: /* value: array_literal  */
#line 492 "Parser.y"
                                        {
				yyval.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2733 "parser.c"
    break;

  case 131: /* value: T_IDENT  */
#line 498 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				yyval.v.type = C_PUSH_VARIABLE;  
				yyval.v.data.string  = yyvsp[0].ident.s;
			}
#line 2743 "parser.c"
    break;

  case 132: /* $@8: %empty  */
#line 504 "Parser.y"
                 {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2751 "parser.c"
    break;

  case 136: /* map_associate: T_STRING T_ASSOC_ARROW simple_literal  */
#line 514 "Parser.y"
                                                              { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2760 "parser.c"
    break;

  case 137: /* map_associate: T_STRING T_ASSOC_ARROW T_STRING  */
#line 518 "Parser.y"
                                                          { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2769 "parser.c"
    break;

  case 138: /* $@9: %empty  */
#line 523 "Parser.y"
                   {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2777 "parser.c"
    break;

  case 140: /* $@10: %empty  */
#line 527 "Parser.y"
                              {
				_currentArrayLiteral = arraylist_create(3);
			}
#line 2785 "parser.c"
    break;

  case 144: /* array_element: simple_array_element  */
#line 536 "Parser.y"
                                             {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2793 "parser.c"
    break;

  case 145: /* array_element: T_STRING  */
#line 540 "Parser.y"
                                 {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2801 "parser.c"
    break;

  case 146: /* simple_array_element: T_NUM  */
#line 545 "Parser.y"
                                                { yyval.v.type = VT_NUMBER; yyval.v.data.longValue = yyvsp[0].num; }
#line 2807 "parser.c"
    break;

  case 147: /* simple_array_element: T_CHARACTER  */
#line 546 "Parser.y"
                                        { yyval.v.type = VT_CHAR; yyval.v.data.longValue = yyvsp[0].num; }
#line 2813 "parser.c"
    break;

  case 148: /* simple_array_element: T_TRUE  */
#line 547 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 1; }
#line 2819 "parser.c"
    break;

  case 149: /* simple_array_element: T_FALSE  */
#line 548 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 0; }
#line 2825 "parser.c"
    break;

  case 150: /* string: T_STRING  */
#line 550 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2831 "parser.c"
    break;

  case 151: /* string: string T_STRING  */
#line 551 "Parser.y"
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
#line 2846 "parser.c"
    break;

  case 152: /* break: T_BREAK opt_num ';'  */
#line 563 "Parser.y"
                        {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2852 "parser.c"
    break;

  case 153: /* continue: T_CONTINUE opt_num ';'  */
#line 566 "Parser.y"
                        {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2858 "parser.c"
    break;

  case 154: /* return_expression: T_RETURN optional_bterm  */
#line 569 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 2866 "parser.c"
    break;

  case 157: /* opt_num: %empty  */
#line 576 "Parser.y"
                {	yyval.num = 1;	}
#line 2872 "parser.c"
    break;

  case 158: /* opt_num: T_NUM  */
#line 577 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 2878 "parser.c"
    break;

  case 162: /* case_selector: T_CASE case_condition ':'  */
#line 584 "Parser.y"
                                         {
				yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ});
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
			}
#line 2887 "parser.c"
    break;

  case 164: /* case_condition: T_NUM  */
#line 592 "Parser.y"
                              {
				YY_EMIT(C_PUSH_INTEGER_LITERAL, (GENERIC_DATA){.longValue=yyvsp[0].num});
			}
#line 2895 "parser.c"
    break;

  case 165: /* case_condition: T_STRING  */
#line 595 "Parser.y"
                                   {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2905 "parser.c"
    break;

  case 166: /* label: T_IDENT ':'  */
#line 601 "Parser.y"
                            {
				bytecode_createBranchLabel(&_bytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 2914 "parser.c"
    break;

  case 167: /* goto: T_GOTO T_IDENT  */
#line 606 "Parser.y"
                               {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 2923 "parser.c"
    break;

  case 168: /* $@11: %empty  */
#line 611 "Parser.y"
                                                  {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				makeInternalSymbol(yyvsp[0].ident.s, yyvsp[-1].type, (intptr_t)0);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2936 "parser.c"
    break;

  case 169: /* in_clause: '(' type_name variable_identifier $@11 ':' simple_expression ')'  */
#line 618 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}
#line 2947 "parser.c"
    break;

  case 170: /* $@12: %empty  */
#line 626 "Parser.y"
                                 {
			}
#line 2954 "parser.c"
    break;

  case 171: /* $@13: %empty  */
#line 627 "Parser.y"
                                        {
				_breaklevel++;
			}
#line 2962 "parser.c"
    break;

  case 172: /* switch_expression: T_SWITCH $@12 '(' value ')' $@13 '{' case_clauses '}'  */
#line 631 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
				_breaklevel--;
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 2973 "parser.c"
    break;

  case 173: /* $@14: %empty  */
#line 638 "Parser.y"
                      {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			}
#line 2981 "parser.c"
    break;

  case 174: /* $@15: %empty  */
#line 640 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 2990 "parser.c"
    break;

  case 175: /* foreach: T_FOR $@14 in_clause $@15 local_block  */
#line 643 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3002 "parser.c"
    break;

  case 176: /* $@16: %empty  */
#line 651 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			}
#line 3010 "parser.c"
    break;

  case 177: /* $@17: %empty  */
#line 653 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3019 "parser.c"
    break;

  case 178: /* while: T_WHILE $@16 condition $@17 local_block  */
#line 656 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3031 "parser.c"
    break;

  case 181: /* closing_brace: %empty  */
#line 667 "Parser.y"
                        { yyerror("Missing closing brace '}'"); }
#line 3037 "parser.c"
    break;

  case 183: /* $@18: %empty  */
#line 672 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3046 "parser.c"
    break;

  case 184: /* $@19: %empty  */
#line 675 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3054 "parser.c"
    break;

  case 185: /* if_expression: T_IF condition $@18 local_block $@19 else_clause  */
#line 677 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3063 "parser.c"
    break;

  case 186: /* else_clause: %empty  */
#line 682 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3072 "parser.c"
    break;

  case 187: /* $@20: %empty  */
#line 686 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3082 "parser.c"
    break;

  case 189: /* stmntlist: %empty  */
#line 693 "Parser.y"
                { yyval.needsPop = 0; }
#line 3088 "parser.c"
    break;

  case 190: /* $@21: %empty  */
#line 694 "Parser.y"
                                    { 
				if (yyvsp[0].needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			}
#line 3098 "parser.c"
    break;

  case 193: /* closing_paren: error  */
#line 702 "Parser.y"
                                        {		yyerror("Missing closing )"); }
#line 3104 "parser.c"
    break;

  case 194: /* @22: %empty  */
#line 704 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3116 "parser.c"
    break;

  case 195: /* call_expression: T_FUNC '(' @22 parameter_list closing_paren  */
#line 710 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3126 "parser.c"
    break;

  case 196: /* @23: %empty  */
#line 715 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3137 "parser.c"
    break;

  case 197: /* call_expression: function_id_or_pointer '(' @23 parameter_list closing_paren  */
#line 720 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, _functionCallOp,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&yyvsp[-4].ident.s);
			}
#line 3147 "parser.c"
    break;

  case 198: /* function_id_or_pointer: '*' variable_reference  */
#line 727 "Parser.y"
                                               {
				_functionCallOp = C_MACRO_REF;
				yyval.ident.s = _strdup(yyvsp[0].ident.s);
				yyval.ident.stringIsAlloced = 1;
			}
#line 3157 "parser.c"
    break;

  case 199: /* function_id_or_pointer: T_IDENT  */
#line 733 "Parser.y"
                                {
				_functionCallOp = C_MACRO;
				yyval.ident = yyvsp[0].ident;
			}
#line 3166 "parser.c"
    break;

  case 200: /* function_id_or_pointer: error  */
#line 737 "Parser.y"
                                {
				yyerror("Illegal function call expression");
				yyerrok;
				yyval.ident.s = 0;
			}
#line 3176 "parser.c"
    break;

  case 204: /* nonempty_parameters: error  */
#line 747 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3184 "parser.c"
    break;

  case 205: /* parameter: simple_expression  */
#line 751 "Parser.y"
                                  { yyval.type = yyvsp[0].type; (*_currentFunctionCallParamIndexP)++; }
#line 3190 "parser.c"
    break;

  case 206: /* type_cast: '(' type_name ')'  */
#line 753 "Parser.y"
                                        {	yyval.type = yyvsp[-1].type;		}
#line 3196 "parser.c"
    break;

  case 208: /* type_name: T_TYPE_IDENTIFIER array_dimension  */
#line 756 "Parser.y"
                                                                {	yyval.type = VT_OBJECT_ARRAY;	}
#line 3202 "parser.c"
    break;

  case 210: /* array_dimension: '[' T_NUM ']'  */
#line 759 "Parser.y"
                                        {
				_arraySize = (long)yyvsp[-1].num;
			}
#line 3210 "parser.c"
    break;

  case 211: /* boolean_literal: T_TRUE  */
#line 763 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3219 "parser.c"
    break;

  case 212: /* boolean_literal: T_FALSE  */
#line 766 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3228 "parser.c"
    break;

  case 213: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 771 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3237 "parser.c"
    break;

  case 214: /* integer_literal: T_NUM  */
#line 776 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3246 "parser.c"
    break;

  case 215: /* character_literal: T_CHARACTER  */
#line 781 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3255 "parser.c"
    break;

  case 216: /* simple_literal: integer_literal  */
#line 786 "Parser.y"
                                { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(yyvsp[0].v.data.longValue);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_NUMBER;
			}
#line 3268 "parser.c"
    break;

  case 217: /* simple_literal: float_literal  */
#line 794 "Parser.y"
                                        { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_FLOAT;
			}
#line 3281 "parser.c"
    break;

  case 218: /* simple_literal: character_literal  */
#line 802 "Parser.y"
                                            { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_CHAR;
			}
#line 3294 "parser.c"
    break;

  case 219: /* simple_literal: boolean_literal  */
#line 810 "Parser.y"
                                          { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, yyvsp[0].v.data);
				} else {
					yyval.v = yyvsp[0].v;
				}
				yyval.type = VT_BOOLEAN;
			}
#line 3307 "parser.c"
    break;


#line 3311 "parser.c"

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

#line 818 "Parser.y"


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
/* bytecode_emitGotoInstruction()					*/
/*---------------------------------*/
static char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	if (level < 0)
		yyerror("illegal break/continue level");
	else
		_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(name,&_bytecodeBuffer,bratyp);
	return _bytecodeBuffer.bb_current;
}

/*---------------------------------*/
/* bytecode_generateAutoLabelNamePrefix()					*/
/*---------------------------------*/
static int bytecode_generateAutoLabelNamePrefix(char *prefix,int level)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	return bytecode_createBranchLabel(&_bytecodeBuffer, name);
}

/*---------------------------------*/
/* bytecode_destroyAutoLabelNamePrefix()					*/
/*---------------------------------*/
static void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level)
{	char *name = bytecode_generateAutoLabelName(prefix,level);

	/*
	 * autolabel is not used any more: kill it
	 */
	bytecode_destroyLabelNamed(name);
}

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
}

static void finalize_header() {
	macro_defineNamespaceInitializer(_currentNamespaceIndex, _bytecodeBuffer.bb_start, _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start);
	destroy_bytecodeBuffer();
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

