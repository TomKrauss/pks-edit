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
# define 	STRINGTYPE(type)	((type == C_PUSH_STRING_LITERAL||type == C_PUSH_VARIABLE) ? \
							CT_STRING : CT_NUM)
# define 	ISCONSTINT(t)		(t == C_PUSH_LONG_LITERAL|| t == C_PUSH_INTEGER_LITERAL || \
								 t == C_PUSH_SMALL_INT_LITERAL)
# define	ISVARINT(t)		(t == C_LONGVAR)

# define 	MAXEXPR			12

# define	REC_SPACE			24000

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


#line 212 "parser.c"

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
  YYSYMBOL_T_BRAEQ = 27,                   /* T_BRAEQ  */
  YYSYMBOL_T_SH_ASSIGN_PLUS = 28,          /* T_SH_ASSIGN_PLUS  */
  YYSYMBOL_T_SH_ASSIGN_MINUS = 29,         /* T_SH_ASSIGN_MINUS  */
  YYSYMBOL_T_SH_ASSIGN_MULT = 30,          /* T_SH_ASSIGN_MULT  */
  YYSYMBOL_T_SH_ASSIGN_DIV = 31,           /* T_SH_ASSIGN_DIV  */
  YYSYMBOL_T_SH_ASSIGN_MOD = 32,           /* T_SH_ASSIGN_MOD  */
  YYSYMBOL_T_LOCAL = 33,                   /* T_LOCAL  */
  YYSYMBOL_T_SWITCH = 34,                  /* T_SWITCH  */
  YYSYMBOL_T_CASE = 35,                    /* T_CASE  */
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
  YYSYMBOL_75_ = 75,                       /* '}'  */
  YYSYMBOL_76_ = 76,                       /* ':'  */
  YYSYMBOL_YYACCEPT = 77,                  /* $accept  */
  YYSYMBOL_file_structure = 78,            /* file_structure  */
  YYSYMBOL_79_1 = 79,                      /* $@1  */
  YYSYMBOL_80_2 = 80,                      /* $@2  */
  YYSYMBOL_header_declarations = 81,       /* header_declarations  */
  YYSYMBOL_namespace = 82,                 /* namespace  */
  YYSYMBOL_prerequisites = 83,             /* prerequisites  */
  YYSYMBOL_require = 84,                   /* require  */
  YYSYMBOL_global_declarations = 85,       /* global_declarations  */
  YYSYMBOL_global_vars = 86,               /* global_vars  */
  YYSYMBOL_global_var = 87,                /* global_var  */
  YYSYMBOL_scope = 88,                     /* scope  */
  YYSYMBOL_macro_definitions = 89,         /* macro_definitions  */
  YYSYMBOL_constants = 90,                 /* constants  */
  YYSYMBOL_constant_list = 91,             /* constant_list  */
  YYSYMBOL_constdef = 92,                  /* constdef  */
  YYSYMBOL_93_3 = 93,                      /* $@3  */
  YYSYMBOL_94_4 = 94,                      /* $@4  */
  YYSYMBOL_constant_literal = 95,          /* constant_literal  */
  YYSYMBOL_variable_identifier = 96,       /* variable_identifier  */
  YYSYMBOL_variable_reference = 97,        /* variable_reference  */
  YYSYMBOL_macro_declaration = 98,         /* macro_declaration  */
  YYSYMBOL_macro_type = 99,                /* macro_type  */
  YYSYMBOL_macrostart = 100,               /* macrostart  */
  YYSYMBOL_101_5 = 101,                    /* $@5  */
  YYSYMBOL_optional_description = 102,     /* optional_description  */
  YYSYMBOL_parameter_list = 103,           /* parameter_list  */
  YYSYMBOL_non_empty_pardecl = 104,        /* non_empty_pardecl  */
  YYSYMBOL_par_decl = 105,                 /* par_decl  */
  YYSYMBOL_macro_definition = 106,         /* macro_definition  */
  YYSYMBOL_block = 107,                    /* block  */
  YYSYMBOL_var_decls = 108,                /* var_decls  */
  YYSYMBOL_var_decl = 109,                 /* var_decl  */
  YYSYMBOL_statement = 110,                /* statement  */
  YYSYMBOL_errors = 111,                   /* errors  */
  YYSYMBOL_assignment = 112,               /* assignment  */
  YYSYMBOL_113_6 = 113,                    /* $@6  */
  YYSYMBOL_shorthand_assignment_operator = 114, /* shorthand_assignment_operator  */
  YYSYMBOL_assignment_expression = 115,    /* assignment_expression  */
  YYSYMBOL_simple_expression = 116,        /* simple_expression  */
  YYSYMBOL_binary_expression_or_range = 117, /* binary_expression_or_range  */
  YYSYMBOL_range_expression = 118,         /* range_expression  */
  YYSYMBOL_119_7 = 119,                    /* $@7  */
  YYSYMBOL_range_increment = 120,          /* range_increment  */
  YYSYMBOL_increment_expression = 121,     /* increment_expression  */
  YYSYMBOL_s_bterm = 122,                  /* s_bterm  */
  YYSYMBOL_binary_expression = 123,        /* binary_expression  */
  YYSYMBOL_condition = 124,                /* condition  */
  YYSYMBOL_value = 125,                    /* value  */
  YYSYMBOL_map_literal = 126,              /* map_literal  */
  YYSYMBOL_127_8 = 127,                    /* $@8  */
  YYSYMBOL_map_associates = 128,           /* map_associates  */
  YYSYMBOL_map_associate = 129,            /* map_associate  */
  YYSYMBOL_array_literal = 130,            /* array_literal  */
  YYSYMBOL_131_9 = 131,                    /* $@9  */
  YYSYMBOL_132_10 = 132,                   /* $@10  */
  YYSYMBOL_array_elements = 133,           /* array_elements  */
  YYSYMBOL_array_element = 134,            /* array_element  */
  YYSYMBOL_simple_array_element = 135,     /* simple_array_element  */
  YYSYMBOL_string = 136,                   /* string  */
  YYSYMBOL_break = 137,                    /* break  */
  YYSYMBOL_continue = 138,                 /* continue  */
  YYSYMBOL_return_expression = 139,        /* return_expression  */
  YYSYMBOL_optional_bterm = 140,           /* optional_bterm  */
  YYSYMBOL_opt_num = 141,                  /* opt_num  */
  YYSYMBOL_case = 142,                     /* case  */
  YYSYMBOL_case_expressions = 143,         /* case_expressions  */
  YYSYMBOL_case_expression = 144,          /* case_expression  */
  YYSYMBOL_label = 145,                    /* label  */
  YYSYMBOL_goto = 146,                     /* goto  */
  YYSYMBOL_147_11 = 147,                   /* $@11  */
  YYSYMBOL_in_clause = 148,                /* in_clause  */
  YYSYMBOL_149_12 = 149,                   /* $@12  */
  YYSYMBOL_foreach = 150,                  /* foreach  */
  YYSYMBOL_151_13 = 151,                   /* $@13  */
  YYSYMBOL_152_14 = 152,                   /* $@14  */
  YYSYMBOL_while = 153,                    /* while  */
  YYSYMBOL_154_15 = 154,                   /* $@15  */
  YYSYMBOL_155_16 = 155,                   /* $@16  */
  YYSYMBOL_local_block = 156,              /* local_block  */
  YYSYMBOL_closing_brace = 157,            /* closing_brace  */
  YYSYMBOL_if_expression = 158,            /* if_expression  */
  YYSYMBOL_159_17 = 159,                   /* $@17  */
  YYSYMBOL_160_18 = 160,                   /* $@18  */
  YYSYMBOL_else_clause = 161,              /* else_clause  */
  YYSYMBOL_162_19 = 162,                   /* $@19  */
  YYSYMBOL_stmntlist = 163,                /* stmntlist  */
  YYSYMBOL_164_20 = 164,                   /* $@20  */
  YYSYMBOL_closing_paren = 165,            /* closing_paren  */
  YYSYMBOL_call_expression = 166,          /* call_expression  */
  YYSYMBOL_167_21 = 167,                   /* @21  */
  YYSYMBOL_168_22 = 168,                   /* @22  */
  YYSYMBOL_function_id_or_pointer = 169,   /* function_id_or_pointer  */
  YYSYMBOL_nonempty_parameters = 170,      /* nonempty_parameters  */
  YYSYMBOL_parameter = 171,                /* parameter  */
  YYSYMBOL_type_cast = 172,                /* type_cast  */
  YYSYMBOL_type_name = 173,                /* type_name  */
  YYSYMBOL_array_dimension = 174,          /* array_dimension  */
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
#define YYLAST   1071

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  103
/* YYNRULES -- Number of rules.  */
#define YYNRULES  217
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  332

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
       2,     2,     2,     2,     2,     2,     2,     2,    76,    68,
      58,     2,    59,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    67,     2,    73,    56,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    72,    55,    75,    57,     2,     2,     2,
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
       0,   174,   174,   180,   174,   186,   188,   191,   196,   198,
     200,   205,   206,   207,   209,   210,   212,   214,   215,   217,
     218,   222,   225,   226,   228,   228,   228,   242,   245,   251,
     252,   253,   260,   267,   268,   278,   281,   287,   288,   290,
     290,   296,   298,   303,   304,   307,   309,   311,   318,   339,
     341,   342,   344,   351,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   376,
     382,   383,   385,   390,   395,   395,   404,   405,   406,   407,
     408,   410,   412,   413,   416,   417,   419,   420,   420,   422,
     423,   426,   428,   430,   431,   434,   435,   439,   448,   449,
     450,   451,   452,   453,   454,   455,   456,   457,   458,   459,
     460,   461,   462,   463,   464,   465,   466,   467,   468,   469,
     470,   471,   472,   473,   474,   475,   482,   483,   487,   491,
     496,   497,   503,   509,   515,   515,   520,   522,   525,   529,
     534,   534,   538,   538,   543,   544,   547,   551,   556,   557,
     558,   559,   561,   562,   573,   576,   580,   584,   585,   587,
     588,   590,   592,   593,   595,   597,   602,   606,   606,   611,
     611,   625,   627,   625,   638,   640,   638,   651,   651,   654,
     656,   659,   662,   659,   669,   673,   673,   680,   681,   681,
     688,   689,   691,   691,   702,   702,   714,   720,   724,   730,
     732,   733,   734,   738,   740,   742,   743,   745,   746,   750,
     753,   758,   763,   768,   773,   781,   789,   797
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "T_NAMESPACE",
  "T_REQUIRE", "T_STATIC", "T_FOR", "T_TYPE_IDENTIFIER", "T_IDENT",
  "T_NUM", "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER", "T_CONST",
  "T_TRUE", "T_FALSE", "T_TBOOLEAN", "T_FUNC", "T_MACRO", "T_DOTDOT",
  "T_PLUSPLUS", "T_MINUSMINUS", "T_IF", "T_ELIF", "T_ELSE", "T_WHILE",
  "T_GOTO", "T_BRAEQ", "T_SH_ASSIGN_PLUS", "T_SH_ASSIGN_MINUS",
  "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV", "T_SH_ASSIGN_MOD", "T_LOCAL",
  "T_SWITCH", "T_CASE", "T_BREAK", "T_CONTINUE", "T_RETURN", "T_VARIABLE",
  "T_ASSOC_ARROW", "T_NMATCH", "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT",
  "T_SHIFT_RIGHT", "T_OR", "T_AND", "T_LE", "T_GE", "T_POWER_TO", "T_VOID",
  "T_CMDSEQPREFIX", "'&'", "'|'", "'^'", "'~'", "'<'", "'>'", "T_LT",
  "T_GT", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "';'", "','", "'('",
  "')'", "'{'", "']'", "'!'", "'}'", "':'", "$accept", "file_structure",
  "$@1", "$@2", "header_declarations", "namespace", "prerequisites",
  "require", "global_declarations", "global_vars", "global_var", "scope",
  "macro_definitions", "constants", "constant_list", "constdef", "$@3",
  "$@4", "constant_literal", "variable_identifier", "variable_reference",
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
  "case", "case_expressions", "case_expression", "label", "goto", "$@11",
  "in_clause", "$@12", "foreach", "$@13", "$@14", "while", "$@15", "$@16",
  "local_block", "closing_brace", "if_expression", "$@17", "$@18",
  "else_clause", "$@19", "stmntlist", "$@20", "closing_paren",
  "call_expression", "@21", "@22", "function_id_or_pointer",
  "nonempty_parameters", "parameter", "type_cast", "type_name",
  "array_dimension", "boolean_literal", "float_literal", "integer_literal",
  "character_literal", "simple_literal", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-239)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-199)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -239,    56,    74,  -239,    57,  -239,    89,    34,  -239,    83,
    -239,    89,  -239,    86,    45,    49,  -239,  -239,  -239,  -239,
    -239,  -239,  -239,    93,   107,  -239,    47,    20,    38,  -239,
      44,  -239,    60,  -239,    44,   107,  -239,  -239,  -239,    46,
      43,  -239,  -239,  -239,  -239,  -239,  -239,    88,  -239,     7,
    -239,     2,   446,   107,   539,  -239,  -239,  -239,   194,    55,
    -239,   294,  -239,    63,    92,  -239,     3,    99,   113,  -239,
     124,   125,   113,   149,   149,   370,    30,  -239,     8,  -239,
    -239,  -239,    14,   116,  -239,  -239,  -239,  -239,  -239,  -239,
    -239,  -239,  -239,  -239,   115,   120,   122,  -239,    92,     1,
    -239,  -239,  -239,  -239,  -239,  -239,   153,   751,   751,   783,
    -239,   614,   211,   751,   165,   140,   150,  -239,  -239,  -239,
    -239,   815,  -239,  -239,  -239,   201,  -239,   158,  -239,    25,
      44,  -239,  -239,  -239,  -239,  -239,  -239,  -239,  -239,  -239,
    -239,  -239,  -239,   159,  -239,  -239,   751,  -239,   113,  -239,
    -239,   162,  -239,   163,   167,   900,  -239,  -239,  -239,  -239,
    -239,     9,   143,  -239,   446,  -239,  -239,  -239,  -239,  -239,
    -239,  -239,    59,   114,  -239,   114,   226,   842,   168,   115,
     250,   900,  -239,   229,   107,   751,   751,   751,   751,   751,
     751,   751,   751,   751,   751,   751,   751,   751,   751,   751,
     751,   751,   751,   751,   751,   751,   751,   640,  -239,   573,
     180,  -239,  -239,  -239,  -239,   107,  -239,   539,   871,   160,
    -239,   185,   219,  -239,  -239,  -239,   183,  -239,  -239,  -239,
    -239,  -239,   681,  -239,   539,   217,     5,  -239,  -239,  -239,
    -239,  -239,  -239,  -239,  -239,  -239,    26,  -239,  -239,  -239,
    -239,  -239,   900,    59,   954,   954,   151,   151,   927,   927,
     900,   900,    18,   981,   981,   981,    59,  1004,  1004,   114,
     114,    18,    18,    18,   190,  -239,   815,  -239,    44,   160,
      13,  -239,   500,  -239,  -239,   160,  -239,   258,   192,   219,
     165,  -239,    13,   232,   226,  -239,   250,  -239,   249,  -239,
    -239,  -239,  -239,  -239,  -239,   107,   115,   246,  -239,   196,
      47,  -239,  -239,  -239,  -239,  -239,  -239,  -239,  -239,   751,
    -239,   197,  -239,  -239,  -239,  -239,    62,   707,   160,   203,
    -239,  -239
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     6,     1,     0,     3,     8,     0,    19,     0,
      11,     8,     7,    17,     0,     5,     9,    18,    39,    20,
      10,    24,    13,    14,     0,    12,     0,     0,     0,    22,
       0,    15,   205,    16,     0,    50,    48,    38,    37,     0,
       0,    21,    24,    29,    31,    32,    33,     0,    30,     0,
     206,     0,     0,    50,     0,    35,    36,    23,     0,     0,
     207,     0,    53,     0,    70,   171,    34,     0,     0,   174,
       0,     0,     0,   159,   159,     0,     0,    54,    74,    65,
      68,   188,     0,     0,    66,    61,    62,    63,    60,    67,
      64,    59,    58,    57,   179,     0,     0,    51,   202,   133,
     212,   211,   152,   213,   209,   210,   128,     0,     0,     0,
     134,     0,   140,     0,     0,     0,    44,    46,   203,    83,
      95,    82,    93,   131,   132,   129,    94,   199,   200,     0,
       0,   217,   215,   214,   216,   130,    28,    25,    27,   208,
     198,    81,    52,     0,   165,   192,     0,   181,     0,   166,
     167,     0,   160,     0,     0,   158,   156,    34,   196,    92,
      91,     0,     0,    72,     0,    71,    69,    56,   180,    49,
      55,   194,   100,   101,    97,   102,     0,     0,     0,   179,
       0,    99,   125,    41,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   153,     0,
     133,   128,    96,    47,    26,     0,   172,     0,   127,     0,
     175,     0,   162,   154,   155,   133,     0,    79,    80,    76,
      77,    78,     0,   189,     0,     0,     0,   136,   124,   204,
     141,   148,   147,   149,   150,   151,   179,   145,   146,    42,
      40,    45,    87,   104,   123,   122,   113,   114,   117,   116,
     120,   121,   115,   105,   106,   108,   103,   118,   119,   107,
     109,   110,   111,   112,     0,    85,    84,   201,     0,     0,
       0,   126,     0,   177,   182,     0,   168,     0,     0,   162,
       0,    75,     0,     0,     0,   135,     0,   143,    89,    98,
     169,   173,   191,   190,   193,    50,   179,   184,   176,     0,
       0,   161,   163,    73,   195,   139,   138,   137,   144,     0,
      88,     0,   178,   185,   183,   164,    90,     0,     0,     0,
     186,   170
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -239,  -239,  -239,  -239,  -239,  -239,   264,  -239,  -239,   253,
    -239,   265,  -239,  -239,  -239,   235,  -239,  -239,  -239,   -27,
     -30,  -239,  -239,  -239,  -239,  -239,  -168,  -239,    96,  -239,
     -25,   -47,   -22,   -44,  -239,  -239,  -239,  -239,   -69,   -56,
    -239,    75,  -239,  -239,  -239,   152,   -50,   -60,   123,  -239,
    -239,  -239,    -8,  -239,  -239,  -239,  -239,    -9,  -239,  -239,
    -239,  -239,  -239,  -239,   214,  -239,     0,  -239,     4,  -239,
    -239,  -239,  -239,  -239,  -239,  -239,  -239,  -239,  -239,  -238,
    -147,  -239,  -239,  -239,  -239,  -239,  -153,  -239,    -2,   -49,
    -239,  -239,  -239,  -239,    84,  -239,   -24,  -239,  -239,  -239,
    -239,  -239,   -48
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,     8,     5,     6,    10,    11,    15,    22,
      23,    24,    13,    25,    28,    29,    30,   214,   137,    47,
     114,    39,    40,    26,    27,   250,   115,   116,   117,    19,
      79,    52,    80,   283,    82,    83,   162,   232,    63,   118,
     274,   119,   298,   320,    84,   120,   121,   147,   122,   123,
     176,   236,   237,   124,   179,   180,   246,   247,   248,   125,
      85,    86,    87,   156,   153,    88,   288,   289,    89,    90,
     221,   216,   321,    91,   143,   279,    92,   148,   285,   284,
     169,    93,   219,   307,   324,   328,    94,   164,   304,   126,
     217,   234,    96,   127,   128,   129,    34,    50,   131,   132,
     133,   134,   135
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      48,    36,    33,    95,    48,   141,    97,    51,    81,   163,
     138,   233,   151,    53,   302,   165,    59,   225,   100,   101,
     102,   103,    78,   104,   105,   155,   140,    37,   159,   160,
     130,    53,   240,   210,   100,   101,   102,   103,   157,   104,
     105,   301,    67,   -34,    61,   182,   158,   308,   211,   280,
      61,    55,    43,    44,    17,    45,     3,   172,   173,   175,
      56,   177,    21,   181,   211,     7,   292,   193,   194,    46,
      62,  -197,    38,  -197,   294,   161,   110,     4,   295,   144,
      60,   112,   166,    46,   303,   207,    -4,   178,   220,    76,
     330,    17,   110,     9,    14,   296,   218,   112,    17,   297,
      48,   168,    12,   213,   189,   190,    41,    42,   193,   194,
     195,   193,   194,    20,    32,    95,    54,   200,   201,    35,
      81,   202,   203,   204,   205,   206,   207,    49,   139,   306,
      58,   142,   149,   150,    78,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   276,   152,   322,
     130,    64,  -198,   193,   194,   195,    65,    32,    66,   145,
      95,   227,   228,   229,   230,   231,   291,    67,   204,   205,
     206,   207,    68,   146,   167,    69,    70,    71,   170,    78,
     168,   278,   171,   130,    72,   -33,    73,    74,    75,    46,
     193,   194,   195,   100,   101,   136,   103,    61,   104,   105,
     130,   183,   208,   202,   203,   204,   205,   206,   207,   184,
    -142,   313,  -142,  -142,    76,  -142,  -142,   209,    77,   215,
      95,   223,   282,    95,   222,   224,    95,   235,    81,   239,
     249,   100,   101,   315,   103,   316,   104,   105,    48,    78,
    -197,   300,    78,   286,   287,    78,   290,   293,    97,   241,
     305,   242,   243,   299,   244,   245,   309,   311,   319,   326,
     323,   329,   144,   327,   331,    16,    31,    57,    18,    95,
     251,   212,   275,    53,   226,   325,   317,   318,   154,   312,
     314,   310,     0,   277,   -86,   140,     0,     0,    78,   -86,
     -86,   -86,    99,   100,   101,   102,   103,     0,   104,   105,
       0,    67,     0,   -86,     0,     0,   -86,     0,   -86,   -86,
     -86,   -86,     0,     0,     0,     0,     0,     0,   -86,   -86,
     -86,   -86,   -86,   106,     0,   -86,     0,   -86,   -86,   -86,
     -86,   -86,   -86,   -86,   -86,   -86,   -86,     0,   -86,   -86,
     -86,   107,   -86,   -86,     0,     0,   108,   109,    76,   -86,
     -86,   110,   -86,   -86,   111,   -86,   112,   -86,   113,   -86,
    -157,   140,     0,     0,     0,  -157,  -157,  -157,    99,   100,
     101,   102,   103,     0,   104,   105,     0,    67,     0,     0,
       0,     0,  -157,     0,  -157,  -157,  -157,  -157,     0,     0,
       0,     0,     0,     0,  -157,  -157,  -157,  -157,  -157,   106,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  -157,     0,     0,     0,     0,   107,     0,     0,
       0,     0,   108,   109,    76,     0,     0,   110,  -157,     0,
     111,     0,   112,     0,   113,  -157,  -187,    64,     0,     0,
       0,  -187,    65,    32,    66,     0,     0,     0,     0,     0,
       0,     0,     0,    67,     0,     0,     0,     0,    68,     0,
    -187,    69,    70,    71,     0,     0,     0,     0,     0,     0,
      72,  -187,    73,    74,    75,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -187,     0,
     -50,    64,     0,     0,     0,   -50,    65,    32,    66,     0,
      76,     0,     0,     0,    77,     0,     0,    67,    35,     0,
       0,  -187,    68,     0,   -50,    69,    70,    71,     0,     0,
       0,     0,     0,     0,    72,   -50,    73,    74,    75,    46,
      98,     0,     0,     0,     0,     0,    32,    99,   100,   101,
     102,   103,   -50,   104,   105,     0,    67,     0,     0,     0,
       0,     0,     0,     0,    76,     0,     0,     0,    77,     0,
       0,     0,    35,     0,   140,   -50,     0,     0,   106,     0,
       0,    99,   100,   101,   102,   103,     0,   104,   105,     0,
      67,     0,     0,     0,     0,     0,   107,     0,     0,     0,
       0,   108,   109,    76,     0,     0,   110,     0,   -86,   111,
     -43,   112,   106,   113,     0,   140,     0,     0,     0,     0,
       0,    32,    99,   100,   101,   102,   103,     0,   104,   105,
     107,    67,     0,     0,     0,   108,   109,    76,     0,     0,
     110,   140,   -86,   111,   -86,   112,     0,   113,    99,   100,
     101,   102,   103,   106,   104,   105,     0,    67,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   107,     0,     0,     0,     0,   108,   109,    76,   106,
       0,   110,   140,     0,   111,     0,   112,     0,   113,    99,
     100,   101,   102,   103,     0,   104,   105,   107,    67,     0,
       0,     0,   108,   109,    76,     0,     0,   110,   140,     0,
     111,     0,   112,   -86,   113,    99,   100,   101,   102,   103,
     106,   104,   105,     0,    67,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   107,     0,
       0,     0,     0,   108,   109,    76,   106,     0,   110,   -86,
       0,   111,   140,   112,     0,   113,     0,     0,     0,    99,
     100,   101,   102,   103,   107,   104,   105,     0,    67,   108,
     109,    76,     0,     0,   110,     0,     0,   111,   -86,   112,
       0,   113,     0,     0,   140,     0,     0,     0,     0,     0,
     106,    99,   174,   101,   102,   103,     0,   104,   105,     0,
      67,     0,     0,     0,     0,     0,     0,     0,   107,     0,
       0,     0,     0,   108,   109,    76,     0,     0,   110,     0,
       0,   111,   106,   112,     0,   113,     0,     0,     0,     0,
       0,     0,     0,     0,   185,     0,     0,     0,     0,     0,
     107,     0,     0,     0,     0,   108,   109,    76,     0,     0,
     110,     0,     0,   111,     0,   112,   186,   113,   187,   188,
     189,   190,   191,   192,   193,   194,   195,     0,     0,   196,
     197,   198,   199,   200,   201,     0,     0,   202,   203,   204,
     205,   206,   207,   186,     0,   187,   188,   189,   190,   191,
     192,   193,   194,   195,     0,     0,   196,   197,   198,   199,
     200,   201,     0,     0,   202,   203,   204,   205,   206,   207,
       0,     0,   186,   238,   187,   188,   189,   190,   191,   192,
     193,   194,   195,     0,     0,   196,   197,   198,   199,   200,
     201,     0,     0,   202,   203,   204,   205,   206,   207,     0,
       0,   186,   281,   187,   188,   189,   190,   191,   192,   193,
     194,   195,     0,     0,   196,   197,   198,   199,   200,   201,
       0,     0,   202,   203,   204,   205,   206,   207,   186,     0,
     187,   188,   189,   190,     0,     0,   193,   194,   195,     0,
       0,   196,   197,   198,   199,   200,   201,     0,     0,   202,
     203,   204,   205,   206,   207,   186,     0,  -199,  -199,   189,
     190,     0,     0,   193,   194,   195,     0,     0,     0,     0,
       0,   199,   200,   201,     0,     0,   202,   203,   204,   205,
     206,   207,   186,     0,   187,   188,   189,   190,     0,     0,
     193,   194,   195,     0,     0,     0,     0,     0,   199,   200,
     201,     0,     0,   202,   203,   204,   205,   206,   207,   189,
     190,     0,     0,   193,   194,   195,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   202,   203,   204,   205,
     206,   207
};

static const yytype_int16 yycheck[] =
{
      30,    26,    24,    52,    34,    61,    53,    34,    52,    78,
      58,   164,    72,    35,     1,     1,     9,     8,     9,    10,
      11,    12,    52,    14,    15,    75,     1,     7,    20,    21,
      54,    53,   179,     8,     9,    10,    11,    12,     8,    14,
      15,   279,    17,    42,    42,   114,    76,   285,    39,   217,
      42,     8,     8,     9,     5,    11,     0,   107,   108,   109,
      17,   111,    13,   113,    39,     8,   234,    49,    50,    39,
      68,    70,    52,    70,    69,    67,    67,     3,    73,    76,
      73,    72,    68,    39,    71,    67,     0,   111,   148,    64,
     328,     5,    67,     4,    11,    69,   146,    72,     5,   246,
     130,    75,    68,   130,    45,    46,    68,    69,    49,    50,
      51,    49,    50,    68,     7,   164,    70,    58,    59,    72,
     164,    62,    63,    64,    65,    66,    67,    67,    73,   282,
      42,    68,     8,     8,   164,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,     9,   306,
     184,     1,    70,    49,    50,    51,     6,     7,     8,    70,
     219,    28,    29,    30,    31,    32,   232,    17,    64,    65,
      66,    67,    22,    70,    68,    25,    26,    27,    68,   219,
      75,   215,    70,   217,    34,    42,    36,    37,    38,    39,
      49,    50,    51,     9,    10,    11,    12,    42,    14,    15,
     234,    71,    11,    62,    63,    64,    65,    66,    67,    69,
       9,   290,    11,    12,    64,    14,    15,    69,    68,    70,
     279,    68,    72,   282,    72,    68,   285,    11,   282,    71,
      11,     9,    10,    11,    12,   293,    14,    15,   278,   279,
      70,   278,   282,    68,    35,   285,    73,    40,   305,     9,
     282,    11,    12,    73,    14,    15,     8,    75,    19,   319,
      24,   327,    76,    76,    71,    11,    23,    42,    13,   328,
     184,   129,   207,   305,   161,   310,   294,   296,    74,   289,
     292,   287,    -1,   209,     0,     1,    -1,    -1,   328,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    14,    15,
      -1,    17,    -1,    19,    -1,    -1,    22,    -1,    24,    25,
      26,    27,    -1,    -1,    -1,    -1,    -1,    -1,    34,    35,
      36,    37,    38,    39,    -1,    41,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    -1,    54,    55,
      56,    57,    58,    59,    -1,    -1,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
       0,     1,    -1,    -1,    -1,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    14,    15,    -1,    17,    -1,    -1,
      -1,    -1,    22,    -1,    24,    25,    26,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    52,    -1,    -1,    -1,    -1,    57,    -1,    -1,
      -1,    -1,    62,    63,    64,    -1,    -1,    67,    68,    -1,
      70,    -1,    72,    -1,    74,    75,     0,     1,    -1,    -1,
      -1,     5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    17,    -1,    -1,    -1,    -1,    22,    -1,
      24,    25,    26,    27,    -1,    -1,    -1,    -1,    -1,    -1,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,
       0,     1,    -1,    -1,    -1,     5,     6,     7,     8,    -1,
      64,    -1,    -1,    -1,    68,    -1,    -1,    17,    72,    -1,
      -1,    75,    22,    -1,    24,    25,    26,    27,    -1,    -1,
      -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,    39,
       1,    -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,
      11,    12,    52,    14,    15,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    -1,    -1,    -1,    68,    -1,
      -1,    -1,    72,    -1,     1,    75,    -1,    -1,    39,    -1,
      -1,     8,     9,    10,    11,    12,    -1,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    57,    -1,    -1,    -1,
      -1,    62,    63,    64,    -1,    -1,    67,    -1,    69,    70,
      71,    72,    39,    74,    -1,     1,    -1,    -1,    -1,    -1,
      -1,     7,     8,     9,    10,    11,    12,    -1,    14,    15,
      57,    17,    -1,    -1,    -1,    62,    63,    64,    -1,    -1,
      67,     1,    69,    70,    71,    72,    -1,    74,     8,     9,
      10,    11,    12,    39,    14,    15,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    57,    -1,    -1,    -1,    -1,    62,    63,    64,    39,
      -1,    67,     1,    -1,    70,    -1,    72,    -1,    74,     8,
       9,    10,    11,    12,    -1,    14,    15,    57,    17,    -1,
      -1,    -1,    62,    63,    64,    -1,    -1,    67,     1,    -1,
      70,    -1,    72,    73,    74,     8,     9,    10,    11,    12,
      39,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,
      -1,    -1,    -1,    62,    63,    64,    39,    -1,    67,    68,
      -1,    70,     1,    72,    -1,    74,    -1,    -1,    -1,     8,
       9,    10,    11,    12,    57,    14,    15,    -1,    17,    62,
      63,    64,    -1,    -1,    67,    -1,    -1,    70,    71,    72,
      -1,    74,    -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,
      39,     8,     9,    10,    11,    12,    -1,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,
      -1,    -1,    -1,    62,    63,    64,    -1,    -1,    67,    -1,
      -1,    70,    39,    72,    -1,    74,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    -1,    62,    63,    64,    -1,    -1,
      67,    -1,    -1,    70,    -1,    72,    41,    74,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    -1,    -1,    54,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    64,
      65,    66,    67,    41,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    -1,    -1,    54,    55,    56,    57,
      58,    59,    -1,    -1,    62,    63,    64,    65,    66,    67,
      -1,    -1,    41,    71,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    -1,    -1,    54,    55,    56,    57,    58,
      59,    -1,    -1,    62,    63,    64,    65,    66,    67,    -1,
      -1,    41,    71,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    -1,    -1,    54,    55,    56,    57,    58,    59,
      -1,    -1,    62,    63,    64,    65,    66,    67,    41,    -1,
      43,    44,    45,    46,    -1,    -1,    49,    50,    51,    -1,
      -1,    54,    55,    56,    57,    58,    59,    -1,    -1,    62,
      63,    64,    65,    66,    67,    41,    -1,    43,    44,    45,
      46,    -1,    -1,    49,    50,    51,    -1,    -1,    -1,    -1,
      -1,    57,    58,    59,    -1,    -1,    62,    63,    64,    65,
      66,    67,    41,    -1,    43,    44,    45,    46,    -1,    -1,
      49,    50,    51,    -1,    -1,    -1,    -1,    -1,    57,    58,
      59,    -1,    -1,    62,    63,    64,    65,    66,    67,    45,
      46,    -1,    -1,    49,    50,    51,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,    65,
      66,    67
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    78,    79,     0,     3,    81,    82,     8,    80,     4,
      83,    84,    68,    89,    11,    85,    83,     5,    88,   106,
      68,    13,    86,    87,    88,    90,   100,   101,    91,    92,
      93,    86,     7,   109,   173,    72,   107,     7,    52,    98,
      99,    68,    69,     8,     9,    11,    39,    96,    97,    67,
     174,    96,   108,   109,    70,     8,    17,    92,    42,     9,
      73,    42,    68,   115,     1,     6,     8,    17,    22,    25,
      26,    27,    34,    36,    37,    38,    64,    68,    97,   107,
     109,   110,   111,   112,   121,   137,   138,   139,   142,   145,
     146,   150,   153,   158,   163,   166,   169,   108,     1,     8,
       9,    10,    11,    12,    14,    15,    39,    57,    62,    63,
      67,    70,    72,    74,    97,   103,   104,   105,   116,   118,
     122,   123,   125,   126,   130,   136,   166,   170,   171,   172,
     173,   175,   176,   177,   178,   179,    11,    95,   179,    73,
       1,   116,    68,   151,    76,    70,    70,   124,   154,     8,
       8,   124,     9,   141,   141,   123,   140,     8,    97,    20,
      21,    67,   113,   115,   164,     1,    68,    68,    75,   157,
      68,    70,   123,   123,     9,   123,   127,   123,   173,   131,
     132,   123,   115,    71,    69,    19,    41,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    54,    55,    56,    57,
      58,    59,    62,    63,    64,    65,    66,    67,    11,    69,
       8,    39,   122,    96,    94,    70,   148,   167,   123,   159,
     124,   147,    72,    68,    68,     8,   125,    28,    29,    30,
      31,    32,   114,   163,   168,    11,   128,   129,    71,    71,
     157,     9,    11,    12,    14,    15,   133,   134,   135,    11,
     102,   105,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   117,   118,   123,   171,   173,   152,
     103,    71,    72,   110,   156,   155,    68,    35,   143,   144,
      73,   116,   103,    40,    69,    73,    69,   157,   119,    73,
      96,   156,     1,    71,   165,   109,   163,   160,   156,     8,
     145,    75,   143,   115,   165,    11,   179,   129,   134,    19,
     120,   149,   157,    24,   161,   107,   123,    76,   162,   116,
     156,    71
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    77,    79,    80,    78,    81,    82,    82,    83,    83,
      84,    85,    85,    85,    86,    86,    87,    88,    88,    89,
      89,    90,    91,    91,    93,    94,    92,    95,    95,    96,
      96,    96,    96,    97,    97,    98,    98,    99,    99,   101,
     100,   102,   102,   103,   103,   104,   104,   105,   106,   107,
     108,   108,   109,   109,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     111,   111,   112,   112,   113,   112,   114,   114,   114,   114,
     114,   115,   116,   116,   117,   117,   118,   119,   118,   120,
     120,   121,   121,   122,   122,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   124,   124,   125,   125,
     125,   125,   125,   125,   127,   126,   128,   128,   129,   129,
     131,   130,   132,   130,   133,   133,   134,   134,   135,   135,
     135,   135,   136,   136,   137,   138,   139,   140,   140,   141,
     141,   142,   143,   143,   144,   145,   146,   147,   146,   149,
     148,   151,   152,   150,   154,   155,   153,   156,   156,   157,
     157,   159,   160,   158,   161,   162,   161,   163,   164,   163,
     165,   165,   167,   166,   168,   166,   169,   169,   169,   103,
     170,   170,   170,   171,   172,   173,   173,   174,   174,   175,
     175,   176,   177,   178,   179,   179,   179,   179
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     4,     3,     0,     3,     0,     2,
       3,     0,     2,     2,     0,     2,     2,     0,     1,     0,
       2,     3,     1,     3,     0,     0,     5,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     1,     0,
       6,     0,     1,     0,     1,     3,     1,     2,     3,     4,
       0,     2,     4,     3,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     2,     5,     0,     4,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     0,     0,     5,     0,
       2,     2,     2,     1,     1,     1,     2,     2,     4,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     2,     1,     1,
       1,     1,     1,     1,     0,     4,     1,     3,     3,     3,
       0,     3,     0,     4,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     2,     0,     1,     0,
       1,     5,     0,     2,     3,     2,     2,     0,     4,     0,
       7,     0,     0,     5,     0,     0,     5,     1,     3,     0,
       1,     0,     0,     6,     0,     0,     3,     0,     0,     3,
       1,     1,     0,     5,     0,     5,     2,     1,     1,     1,
       1,     3,     1,     1,     3,     1,     2,     2,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1
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

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
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
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
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
#line 174 "Parser.y"
                {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			}
#line 2075 "parser.c"
    break;

  case 3: /* $@2: %empty  */
#line 180 "Parser.y"
                                            {
				finalize_header();
			}
#line 2083 "parser.c"
    break;

  case 6: /* namespace: %empty  */
#line 188 "Parser.y"
            {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			}
#line 2091 "parser.c"
    break;

  case 7: /* namespace: T_NAMESPACE T_IDENT ';'  */
#line 191 "Parser.y"
                                                  {
				_currentNamespaceIndex = macro_lookupNamespace(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2100 "parser.c"
    break;

  case 10: /* require: T_REQUIRE T_STRING ';'  */
#line 200 "Parser.y"
                                       {
				yyrequire(yyvsp[-1].ident.s);
				free(yyvsp[-1].ident.s);
			}
#line 2109 "parser.c"
    break;

  case 17: /* scope: %empty  */
#line 214 "Parser.y"
       { _macroScope = MS_GLOBAL; }
#line 2115 "parser.c"
    break;

  case 18: /* scope: T_STATIC  */
#line 215 "Parser.y"
                                   { _macroScope = MS_LOCAL; }
#line 2121 "parser.c"
    break;

  case 24: /* $@3: %empty  */
#line 228 "Parser.y"
                { _bDefiningConst = 1; }
#line 2127 "parser.c"
    break;

  case 25: /* $@4: %empty  */
#line 228 "Parser.y"
                                                                                       { _bDefiningConst = 0; }
#line 2133 "parser.c"
    break;

  case 26: /* constdef: $@3 variable_identifier T_ASSIGN constant_literal $@4  */
#line 229 "Parser.y"
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
#line 2149 "parser.c"
    break;

  case 27: /* constant_literal: simple_literal  */
#line 242 "Parser.y"
                                       { 
				yyval.v = yyvsp[0].v; 
			}
#line 2157 "parser.c"
    break;

  case 28: /* constant_literal: T_STRING  */
#line 245 "Parser.y"
                                        {
				yyval.v.data.string = yyvsp[0].ident.s;
				yyval.v.type = VT_STRING;
			}
#line 2166 "parser.c"
    break;

  case 29: /* variable_identifier: T_IDENT  */
#line 251 "Parser.y"
                                                                {	yyval.ident = yyvsp[0].ident; }
#line 2172 "parser.c"
    break;

  case 30: /* variable_identifier: variable_reference  */
#line 252 "Parser.y"
                                                {	yyval.ident = yyvsp[0].ident; }
#line 2178 "parser.c"
    break;

  case 31: /* variable_identifier: T_NUM  */
#line 254 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident;
				yyval.ident.s = "#dummy";
				yyval.ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			}
#line 2189 "parser.c"
    break;

  case 32: /* variable_identifier: T_STRING  */
#line 261 "Parser.y"
                        {
				yyval.ident = yyvsp[0].ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			}
#line 2198 "parser.c"
    break;

  case 33: /* variable_reference: T_VARIABLE  */
#line 267 "Parser.y"
                                                        {	yyval.ident = yyvsp[0].ident; }
#line 2204 "parser.c"
    break;

  case 34: /* variable_reference: T_IDENT  */
#line 268 "Parser.y"
                                                        {   
				yyerror("Using undeclared variable %s", yyvsp[0].ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0});
				free(yyvsp[0].ident.s);
				yyval.ident = yyvsp[0].ident;
			}
#line 2216 "parser.c"
    break;

  case 35: /* macro_declaration: macro_type T_IDENT  */
#line 278 "Parser.y"
                                      {
				yyval.ident = yyvsp[0].ident;
			}
#line 2224 "parser.c"
    break;

  case 36: /* macro_declaration: macro_type T_FUNC  */
#line 281 "Parser.y"
                                            {
				yyerror("Illegal attempt to redefine native method");
				yyval.ident.s = 0;
				yyval.ident.stringIsAlloced = 0;
			}
#line 2234 "parser.c"
    break;

  case 39: /* $@5: %empty  */
#line 290 "Parser.y"
            { init_macroDefinition(); }
#line 2240 "parser.c"
    break;

  case 40: /* macrostart: $@5 macro_declaration '(' parameter_list ')' optional_description  */
#line 291 "Parser.y"
                        {	yyval.ident = yyvsp[-4].ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			}
#line 2248 "parser.c"
    break;

  case 42: /* optional_description: T_STRING  */
#line 298 "Parser.y"
                                 {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free(yyvsp[0].ident.s);
			}
#line 2257 "parser.c"
    break;

  case 47: /* par_decl: type_name variable_identifier  */
#line 311 "Parser.y"
                                              {
				sym_createSymbol(_currentIdentifierContext, yyvsp[0].ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, yyvsp[0].ident.s,C_DEFINE_PARAMETER,yyvsp[-1].type,_nparam);
				free(yyvsp[0].ident.s);
				_nparam++;
			}
#line 2268 "parser.c"
    break;

  case 48: /* macro_definition: scope macrostart block  */
#line 318 "Parser.y"
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
#line 2293 "parser.c"
    break;

  case 52: /* var_decl: type_name variable_identifier assignment_expression ';'  */
#line 344 "Parser.y"
                                                                        {
				makeInternalSymbol(yyvsp[-2].ident.s, yyvsp[-3].type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-2].ident.s);
				if (yyvsp[-2].ident.stringIsAlloced) {
					free(yyvsp[-2].ident.s);
				}
				vname_count = 0;
			}
#line 2306 "parser.c"
    break;

  case 53: /* var_decl: type_name variable_identifier ';'  */
#line 351 "Parser.y"
                                                                {
				makeInternalSymbol(yyvsp[-1].ident.s, yyvsp[-2].type, 0);
				if (yyvsp[-1].ident.stringIsAlloced) {
					free(yyvsp[-1].ident.s);
				}
				vname_count = 0;
			}
#line 2318 "parser.c"
    break;

  case 55: /* statement: call_expression ';'  */
#line 360 "Parser.y"
                                              { yyval.needsPop = 1; }
#line 2324 "parser.c"
    break;

  case 56: /* statement: assignment ';'  */
#line 361 "Parser.y"
                                         { yyval.needsPop = 1; }
#line 2330 "parser.c"
    break;

  case 57: /* statement: if_expression  */
#line 362 "Parser.y"
                                        { yyval.needsPop = 1; }
#line 2336 "parser.c"
    break;

  case 58: /* statement: while  */
#line 363 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2342 "parser.c"
    break;

  case 59: /* statement: foreach  */
#line 364 "Parser.y"
                                  { yyval.needsPop = 1; }
#line 2348 "parser.c"
    break;

  case 60: /* statement: case  */
#line 365 "Parser.y"
                               { yyval.needsPop = 1; }
#line 2354 "parser.c"
    break;

  case 61: /* statement: break  */
#line 366 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2360 "parser.c"
    break;

  case 62: /* statement: continue  */
#line 367 "Parser.y"
                                   { yyval.needsPop = 0; }
#line 2366 "parser.c"
    break;

  case 63: /* statement: return_expression  */
#line 368 "Parser.y"
                                            { yyval.needsPop = 0; }
#line 2372 "parser.c"
    break;

  case 64: /* statement: goto  */
#line 369 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2378 "parser.c"
    break;

  case 65: /* statement: block  */
#line 370 "Parser.y"
                                { yyval.needsPop = 1; }
#line 2384 "parser.c"
    break;

  case 66: /* statement: increment_expression  */
#line 371 "Parser.y"
                                               { yyval.needsPop = 1; }
#line 2390 "parser.c"
    break;

  case 67: /* statement: label  */
#line 372 "Parser.y"
                                { yyval.needsPop = 0; }
#line 2396 "parser.c"
    break;

  case 68: /* statement: var_decl  */
#line 373 "Parser.y"
                                   {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			}
#line 2404 "parser.c"
    break;

  case 69: /* statement: errors ';'  */
#line 376 "Parser.y"
                                     {
				yyval.needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			}
#line 2414 "parser.c"
    break;

  case 72: /* assignment: variable_reference assignment_expression  */
#line 385 "Parser.y"
                                                     {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
				vname_count = 0;
			}
#line 2423 "parser.c"
    break;

  case 73: /* assignment: variable_reference '[' value ']' assignment_expression  */
#line 390 "Parser.y"
                                                                               {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)yyvsp[-4].ident.s });
				vname_count = 0;
			}
#line 2432 "parser.c"
    break;

  case 74: /* $@6: %empty  */
#line 395 "Parser.y"
                                           {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[0].ident.s});
			}
#line 2440 "parser.c"
    break;

  case 75: /* assignment: variable_reference $@6 shorthand_assignment_operator simple_expression  */
#line 397 "Parser.y"
                                                                          {
				YY_EMIT(C_BINOP, (GENERIC_DATA){yyvsp[-1].type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-3].ident.s);
				vname_count = 0;
			}
#line 2450 "parser.c"
    break;

  case 76: /* shorthand_assignment_operator: T_SH_ASSIGN_MULT  */
#line 404 "Parser.y"
                                         { yyval.type = BIN_MUL; }
#line 2456 "parser.c"
    break;

  case 77: /* shorthand_assignment_operator: T_SH_ASSIGN_DIV  */
#line 405 "Parser.y"
                                          { yyval.type = BIN_DIV; }
#line 2462 "parser.c"
    break;

  case 78: /* shorthand_assignment_operator: T_SH_ASSIGN_MOD  */
#line 406 "Parser.y"
                                          { yyval.type = BIN_MOD; }
#line 2468 "parser.c"
    break;

  case 79: /* shorthand_assignment_operator: T_SH_ASSIGN_PLUS  */
#line 407 "Parser.y"
                                           { yyval.type = BIN_ADD; }
#line 2474 "parser.c"
    break;

  case 80: /* shorthand_assignment_operator: T_SH_ASSIGN_MINUS  */
#line 408 "Parser.y"
                                            { yyval.type = BIN_SUB; }
#line 2480 "parser.c"
    break;

  case 81: /* assignment_expression: T_ASSIGN simple_expression  */
#line 410 "Parser.y"
                                                        { yyval.v = yyvsp[0].v;	}
#line 2486 "parser.c"
    break;

  case 82: /* simple_expression: binary_expression  */
#line 412 "Parser.y"
                                     { yyval.v = yyvsp[0].v; }
#line 2492 "parser.c"
    break;

  case 87: /* $@7: %empty  */
#line 420 "Parser.y"
                                                                       { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2498 "parser.c"
    break;

  case 90: /* range_increment: T_DOTDOT binary_expression  */
#line 423 "Parser.y"
                                                     { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); }
#line 2504 "parser.c"
    break;

  case 91: /* increment_expression: variable_reference T_MINUSMINUS  */
#line 426 "Parser.y"
                                                         { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, -1); }
#line 2510 "parser.c"
    break;

  case 92: /* increment_expression: variable_reference T_PLUSPLUS  */
#line 428 "Parser.y"
                                                      { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, yyvsp[-1].ident.s, 1); }
#line 2516 "parser.c"
    break;

  case 93: /* s_bterm: value  */
#line 430 "Parser.y"
                      { yyval.v = yyvsp[0].v; }
#line 2522 "parser.c"
    break;

  case 95: /* binary_expression: s_bterm  */
#line 434 "Parser.y"
                                 { yyval.type = yyvsp[0].type;	}
#line 2528 "parser.c"
    break;

  case 96: /* binary_expression: type_cast s_bterm  */
#line 435 "Parser.y"
                                                        {
				yyval.type = yyvsp[-1].type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, yyval.type);
			}
#line 2537 "parser.c"
    break;

  case 97: /* binary_expression: '-' T_NUM  */
#line 439 "Parser.y"
                                    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-yyvsp[0].num);
				} else {
					yyval.type = VT_NUMBER;
					yyval.v.type = C_PUSH_LONG_LITERAL; 
					yyval.v.data.longValue  = -yyvsp[0].num;
				}
			}
#line 2551 "parser.c"
    break;

  case 98: /* binary_expression: binary_expression '[' binary_expression_or_range ']'  */
#line 448 "Parser.y"
                                                                               { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); }
#line 2557 "parser.c"
    break;

  case 99: /* binary_expression: '!' binary_expression  */
#line 449 "Parser.y"
                                                { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); }
#line 2563 "parser.c"
    break;

  case 100: /* binary_expression: '~' binary_expression  */
#line 450 "Parser.y"
                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); }
#line 2569 "parser.c"
    break;

  case 101: /* binary_expression: '+' binary_expression  */
#line 451 "Parser.y"
                                                { yyval.v = yyvsp[0].v; }
#line 2575 "parser.c"
    break;

  case 102: /* binary_expression: '-' binary_expression  */
#line 452 "Parser.y"
                                                { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &yyvsp[0].v, -1); }
#line 2581 "parser.c"
    break;

  case 103: /* binary_expression: binary_expression '~' binary_expression  */
#line 453 "Parser.y"
                                                                    { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); }
#line 2587 "parser.c"
    break;

  case 104: /* binary_expression: binary_expression T_NMATCH binary_expression  */
#line 454 "Parser.y"
                                                                         { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); }
#line 2593 "parser.c"
    break;

  case 105: /* binary_expression: binary_expression '&' binary_expression  */
#line 455 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); }
#line 2599 "parser.c"
    break;

  case 106: /* binary_expression: binary_expression '|' binary_expression  */
#line 456 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); }
#line 2605 "parser.c"
    break;

  case 107: /* binary_expression: binary_expression '+' binary_expression  */
#line 457 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); }
#line 2611 "parser.c"
    break;

  case 108: /* binary_expression: binary_expression '^' binary_expression  */
#line 458 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); }
#line 2617 "parser.c"
    break;

  case 109: /* binary_expression: binary_expression '-' binary_expression  */
#line 459 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); }
#line 2623 "parser.c"
    break;

  case 110: /* binary_expression: binary_expression '*' binary_expression  */
#line 460 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); }
#line 2629 "parser.c"
    break;

  case 111: /* binary_expression: binary_expression '/' binary_expression  */
#line 461 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); }
#line 2635 "parser.c"
    break;

  case 112: /* binary_expression: binary_expression '%' binary_expression  */
#line 462 "Parser.y"
                                                                        { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); }
#line 2641 "parser.c"
    break;

  case 113: /* binary_expression: binary_expression T_SHIFT_LEFT binary_expression  */
#line 463 "Parser.y"
                                                                                { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); }
#line 2647 "parser.c"
    break;

  case 114: /* binary_expression: binary_expression T_SHIFT_RIGHT binary_expression  */
#line 464 "Parser.y"
                                                                            { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); }
#line 2653 "parser.c"
    break;

  case 115: /* binary_expression: binary_expression T_POWER_TO binary_expression  */
#line 465 "Parser.y"
                                                                         { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); }
#line 2659 "parser.c"
    break;

  case 116: /* binary_expression: binary_expression T_AND binary_expression  */
#line 466 "Parser.y"
                                                                    { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); }
#line 2665 "parser.c"
    break;

  case 117: /* binary_expression: binary_expression T_OR binary_expression  */
#line 467 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); }
#line 2671 "parser.c"
    break;

  case 118: /* binary_expression: binary_expression '<' binary_expression  */
#line 468 "Parser.y"
                                                                  { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); }
#line 2677 "parser.c"
    break;

  case 119: /* binary_expression: binary_expression '>' binary_expression  */
#line 469 "Parser.y"
                                                                  { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); }
#line 2683 "parser.c"
    break;

  case 120: /* binary_expression: binary_expression T_LE binary_expression  */
#line 470 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); }
#line 2689 "parser.c"
    break;

  case 121: /* binary_expression: binary_expression T_GE binary_expression  */
#line 471 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); }
#line 2695 "parser.c"
    break;

  case 122: /* binary_expression: binary_expression T_EQ binary_expression  */
#line 472 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); }
#line 2701 "parser.c"
    break;

  case 123: /* binary_expression: binary_expression T_NE binary_expression  */
#line 473 "Parser.y"
                                                                   { yyval.type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); }
#line 2707 "parser.c"
    break;

  case 124: /* binary_expression: '(' binary_expression ')'  */
#line 474 "Parser.y"
                                                        { yyval.v = yyvsp[-1].v; }
#line 2713 "parser.c"
    break;

  case 125: /* binary_expression: variable_reference assignment_expression  */
#line 475 "Parser.y"
                                                                   {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, yyvsp[-1].ident.s);
				vname_count = 0;
			}
#line 2722 "parser.c"
    break;

  case 127: /* condition: '(' binary_expression  */
#line 483 "Parser.y"
                                                {
				yyerror("Missing closing parenthesis )");
			}
#line 2730 "parser.c"
    break;

  case 128: /* value: T_VARIABLE  */
#line 487 "Parser.y"
                           {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.type = yyvsp[0].ident.type;
			}
#line 2739 "parser.c"
    break;

  case 129: /* value: string  */
#line 491 "Parser.y"
                                 {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				yyval.v.type = C_PUSH_STRING_LITERAL;
				free(yyvsp[0].ident.s);
			}
#line 2749 "parser.c"
    break;

  case 130: /* value: simple_literal  */
#line 496 "Parser.y"
                                         {	yyval.type = yyvsp[0].type;	}
#line 2755 "parser.c"
    break;

  case 131: /* value: map_literal  */
#line 497 "Parser.y"
                                      {
				yyval.type = VT_MAP;
				YY_EMIT(C_PUSH_MAP_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2766 "parser.c"
    break;

  case 132: /* value: array_literal  */
#line 503 "Parser.y"
                                        {
				yyval.type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			}
#line 2777 "parser.c"
    break;

  case 133: /* value: T_IDENT  */
#line 509 "Parser.y"
                                  {
				yyerror("Undefined identifier %s", yyvsp[0].ident.s);
				yyval.v.type = C_PUSH_VARIABLE;  
				yyval.v.data.string  = yyvsp[0].ident.s;
			}
#line 2787 "parser.c"
    break;

  case 134: /* $@8: %empty  */
#line 515 "Parser.y"
                 {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2795 "parser.c"
    break;

  case 138: /* map_associate: T_STRING T_ASSOC_ARROW simple_literal  */
#line 525 "Parser.y"
                                                              { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2804 "parser.c"
    break;

  case 139: /* map_associate: T_STRING T_ASSOC_ARROW T_STRING  */
#line 529 "Parser.y"
                                                          { 
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[-2].ident.s));
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2813 "parser.c"
    break;

  case 140: /* $@9: %empty  */
#line 534 "Parser.y"
                   {
				_currentArrayLiteral = arraylist_create(1);
			}
#line 2821 "parser.c"
    break;

  case 142: /* $@10: %empty  */
#line 538 "Parser.y"
                              {
				_currentArrayLiteral = arraylist_create(3);
			}
#line 2829 "parser.c"
    break;

  case 146: /* array_element: simple_array_element  */
#line 547 "Parser.y"
                                             {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(0, yyvsp[0].v.type, yyvsp[0].v.data.longValue));
			}
#line 2837 "parser.c"
    break;

  case 147: /* array_element: T_STRING  */
#line 551 "Parser.y"
                                 {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, yyvsp[0].ident.s));
			}
#line 2845 "parser.c"
    break;

  case 148: /* simple_array_element: T_NUM  */
#line 556 "Parser.y"
                                                { yyval.v.type = VT_NUMBER; yyval.v.data.longValue = yyvsp[0].num; }
#line 2851 "parser.c"
    break;

  case 149: /* simple_array_element: T_CHARACTER  */
#line 557 "Parser.y"
                                        { yyval.v.type = VT_CHAR; yyval.v.data.longValue = yyvsp[0].num; }
#line 2857 "parser.c"
    break;

  case 150: /* simple_array_element: T_TRUE  */
#line 558 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 1; }
#line 2863 "parser.c"
    break;

  case 151: /* simple_array_element: T_FALSE  */
#line 559 "Parser.y"
                                                { yyval.v.type = VT_BOOLEAN; yyval.v.data.longValue = 0; }
#line 2869 "parser.c"
    break;

  case 152: /* string: T_STRING  */
#line 561 "Parser.y"
                         {	yyval.ident = yyvsp[0].ident; }
#line 2875 "parser.c"
    break;

  case 153: /* string: string T_STRING  */
#line 562 "Parser.y"
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
#line 2890 "parser.c"
    break;

  case 154: /* break: T_BREAK opt_num ';'  */
#line 574 "Parser.y"
                        {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2896 "parser.c"
    break;

  case 155: /* continue: T_CONTINUE opt_num ';'  */
#line 577 "Parser.y"
                        {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)yyvsp[-1].num,BRA_ALWAYS); }
#line 2902 "parser.c"
    break;

  case 156: /* return_expression: T_RETURN optional_bterm  */
#line 580 "Parser.y"
                                                {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			}
#line 2910 "parser.c"
    break;

  case 159: /* opt_num: %empty  */
#line 587 "Parser.y"
                {	yyval.num = 1;	}
#line 2916 "parser.c"
    break;

  case 160: /* opt_num: T_NUM  */
#line 588 "Parser.y"
                                {	yyval.num = yyvsp[0].num; }
#line 2922 "parser.c"
    break;

  case 165: /* label: T_IDENT ':'  */
#line 597 "Parser.y"
                            {
				bytecode_createBranchLabel(&_bytecodeBuffer, yyvsp[-1].ident.s);
				freeitem(&yyvsp[-1].ident.s);
			}
#line 2931 "parser.c"
    break;

  case 166: /* goto: T_GOTO T_IDENT  */
#line 602 "Parser.y"
                               {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&yyvsp[0].ident.s);
			}
#line 2940 "parser.c"
    break;

  case 167: /* $@11: %empty  */
#line 606 "Parser.y"
                                          {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction(yyvsp[0].ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&yyvsp[0].ident.s);
			}
#line 2949 "parser.c"
    break;

  case 169: /* $@12: %empty  */
#line 611 "Parser.y"
                                                  {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=yyvsp[0].ident.s});
				makeInternalSymbol(yyvsp[0].ident.s, yyvsp[-1].type, (intptr_t)0);
				if (yyvsp[0].ident.stringIsAlloced) {
					free(yyvsp[0].ident.s);
				}
			}
#line 2962 "parser.c"
    break;

  case 170: /* in_clause: '(' type_name variable_identifier $@12 ':' simple_expression ')'  */
#line 618 "Parser.y"
                                                    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			}
#line 2973 "parser.c"
    break;

  case 171: /* $@13: %empty  */
#line 625 "Parser.y"
                      {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			}
#line 2981 "parser.c"
    break;

  case 172: /* $@14: %empty  */
#line 627 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 2990 "parser.c"
    break;

  case 173: /* foreach: T_FOR $@13 in_clause $@14 local_block  */
#line 630 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3002 "parser.c"
    break;

  case 174: /* $@15: %empty  */
#line 638 "Parser.y"
                        {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			}
#line 3010 "parser.c"
    break;

  case 175: /* $@16: %empty  */
#line 640 "Parser.y"
                                    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			}
#line 3019 "parser.c"
    break;

  case 176: /* while: T_WHILE $@15 condition $@16 local_block  */
#line 643 "Parser.y"
                                      {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			}
#line 3031 "parser.c"
    break;

  case 179: /* closing_brace: %empty  */
#line 654 "Parser.y"
                        { yyerror("Missing closing brace '}'"); }
#line 3037 "parser.c"
    break;

  case 181: /* $@17: %empty  */
#line 659 "Parser.y"
                                       {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			}
#line 3046 "parser.c"
    break;

  case 182: /* $@18: %empty  */
#line 662 "Parser.y"
                                      {
				_iflevel--;
			}
#line 3054 "parser.c"
    break;

  case 183: /* if_expression: T_IF condition $@17 local_block $@18 else_clause  */
#line 664 "Parser.y"
                                      {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			}
#line 3063 "parser.c"
    break;

  case 184: /* else_clause: %empty  */
#line 669 "Parser.y"
             {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3072 "parser.c"
    break;

  case 185: /* $@19: %empty  */
#line 673 "Parser.y"
                                 {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			}
#line 3082 "parser.c"
    break;

  case 187: /* stmntlist: %empty  */
#line 680 "Parser.y"
                { yyval.needsPop = 0; }
#line 3088 "parser.c"
    break;

  case 188: /* $@20: %empty  */
#line 681 "Parser.y"
                                    { 
				if (yyvsp[0].needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			}
#line 3098 "parser.c"
    break;

  case 191: /* closing_paren: error  */
#line 689 "Parser.y"
                                        {		yyerror("Missing closing )"); }
#line 3104 "parser.c"
    break;

  case 192: /* @21: %empty  */
#line 691 "Parser.y"
                            {
				// Native call
				_currentNativeMethodCalled = yyvsp[-1].funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = yyvsp[-1].funcp;
			}
#line 3116 "parser.c"
    break;

  case 193: /* call_expression: T_FUNC '(' @21 parameter_list closing_paren  */
#line 697 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction(yyvsp[-4].funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			}
#line 3126 "parser.c"
    break;

  case 194: /* @22: %empty  */
#line 702 "Parser.y"
                                                     {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				yyval.funcp = 0;
			}
#line 3137 "parser.c"
    break;

  case 195: /* call_expression: function_id_or_pointer '(' @22 parameter_list closing_paren  */
#line 707 "Parser.y"
                                                       {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, _functionCallOp,(GENERIC_DATA){.string = yyvsp[-4].ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&yyvsp[-4].ident.s);
			}
#line 3147 "parser.c"
    break;

  case 196: /* function_id_or_pointer: '*' variable_reference  */
#line 714 "Parser.y"
                                               {
				_functionCallOp = C_MACRO_REF;
				yyval.ident.s = _strdup(yyvsp[0].ident.s);
				yyval.ident.stringIsAlloced = 1;
			}
#line 3157 "parser.c"
    break;

  case 197: /* function_id_or_pointer: T_IDENT  */
#line 720 "Parser.y"
                                {
				_functionCallOp = C_MACRO;
				yyval.ident = yyvsp[0].ident;
			}
#line 3166 "parser.c"
    break;

  case 198: /* function_id_or_pointer: error  */
#line 724 "Parser.y"
                                {
				yyerror("Illegal function call expression");
				yyerrok;
				yyval.ident.s = 0;
			}
#line 3176 "parser.c"
    break;

  case 202: /* nonempty_parameters: error  */
#line 734 "Parser.y"
                                {
				yyerror("illegal parameters for method call");
			}
#line 3184 "parser.c"
    break;

  case 203: /* parameter: simple_expression  */
#line 738 "Parser.y"
                                  { yyval.type = yyvsp[0].type; (*_currentFunctionCallParamIndexP)++; }
#line 3190 "parser.c"
    break;

  case 204: /* type_cast: '(' type_name ')'  */
#line 740 "Parser.y"
                                        {	yyval.type = yyvsp[-1].type;		}
#line 3196 "parser.c"
    break;

  case 206: /* type_name: T_TYPE_IDENTIFIER array_dimension  */
#line 743 "Parser.y"
                                                                {	yyval.type = VT_OBJECT_ARRAY;	}
#line 3202 "parser.c"
    break;

  case 208: /* array_dimension: '[' T_NUM ']'  */
#line 746 "Parser.y"
                                        {
				_arraySize = (long)yyvsp[-1].num;
			}
#line 3210 "parser.c"
    break;

  case 209: /* boolean_literal: T_TRUE  */
#line 750 "Parser.y"
                        {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 1;
		}
#line 3219 "parser.c"
    break;

  case 210: /* boolean_literal: T_FALSE  */
#line 753 "Parser.y"
                            {
			yyval.v.type = C_PUSH_BOOLEAN_LITERAL; 
			yyval.v.data.booleanValue = 0;
		}
#line 3228 "parser.c"
    break;

  case 211: /* float_literal: T_FLOATING_POINT_NUMBER  */
#line 758 "Parser.y"
                                       {
				yyval.v.type = C_PUSH_FLOAT_LITERAL; 
				yyval.v.data.doubleValue = yyvsp[0].v.data.doubleValue;
			}
#line 3237 "parser.c"
    break;

  case 212: /* integer_literal: T_NUM  */
#line 763 "Parser.y"
                       {
				yyval.v.type = C_PUSH_LONG_LITERAL; 
				yyval.v.data.longValue  = yyvsp[0].num;
			}
#line 3246 "parser.c"
    break;

  case 213: /* character_literal: T_CHARACTER  */
#line 768 "Parser.y"
                               {
				yyval.v.type = C_PUSH_CHARACTER_LITERAL; 
				yyval.v.data.uchar = (char)yyvsp[0].num;
			}
#line 3255 "parser.c"
    break;

  case 214: /* simple_literal: integer_literal  */
#line 773 "Parser.y"
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

  case 215: /* simple_literal: float_literal  */
#line 781 "Parser.y"
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

  case 216: /* simple_literal: character_literal  */
#line 789 "Parser.y"
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

  case 217: /* simple_literal: boolean_literal  */
#line 797 "Parser.y"
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

#line 805 "Parser.y"


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

