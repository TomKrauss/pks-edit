
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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

/* Line 189 of yacc.c  */
#line 41 "Parser.y"


#include <windows.h>
#include <stdio.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "symbols.h"

# define 	YYSTYPE _YYSTYPE
# define 	STRINGTYPE(type)	((type == C_PUSH_STRING_LITERAL||type == C_PUSH_VARIABLE) ? \
							CT_STRING : CT_NUM)
# define 	ISCONSTINT(t)		(t == C_PUSH_LONG_LITERAL|| t == C_PUSH_INTEGER_LITERAL || \
						 t == C_PUSH_CHARACTER_LITERAL)
# define	ISVARINT(t)		(t == C_LONGVAR)

# define 	MAXEXPR			12

# define	REC_SPACE			24000

static BYTECODE_BUFFER  _bytecodeBuffer;
static void		*_currentNativeMethodCalled;
static char*	_calledFunctionOrMacro;
static int		_nparam,
				_breaklevel,
				_currentFunctionCallParamIndex,
				_iflevel,
				_stringflg;

static 		void endmacro(void);
static 		void startmacro(void);
static 		void freeitem(char **p);
static 		char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp);
static		int bytecode_generateAutoLabelNamePrefix(char *prefix,int level);
static		void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
static void 	bytecode_emitPushParameterInstruction(long lValue);

static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

int			vname_count;
extern 		int	yyerrflg;
extern 		int	_bDefiningConst;
extern char*    _yyCurrentComment;
extern void		yyerror(char* s, ...);
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nIncrement);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern unsigned char* bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber);

#define		YY_EMIT(opCode, param)		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, opCode, param);

extern int yylex(void );
unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
void 		bytecode_defineVariable(BYTECODE_BUFFER* pBuffer, const char *name, int nBytecode, int typ, intptr_t val);

char*		bytecode_emitGotoLabelInstruction(char *name, BYTECODE_BUFFER* pBuffer, int typ);
int			bytecode_createBranchLabel(BYTECODE_BUFFER* pBuffer, char *name);
void 		bytecode_closeOpenLabels(void);
void 		bytecode_destroyLabelNamed(char *name);
char*		bytecode_generateAutoLabelName(char *prefix, int num);
int 		function_returnsString(void *ep);

IDENTIFIER_CONTEXT* _currentIdentifierContext;

static void makeInternalSymbol(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal) {
	bytecode_defineVariable(&_bytecodeBuffer, pszName, C_DEFINE_VARIABLE, sType, tVal);
	sym_makeInternalSymbol(_currentIdentifierContext, (char*) pszName, sType, (GENERIC_DATA) {.val = tVal});
}



/* Line 189 of yacc.c  */
#line 177 "Parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_TSTRING = 258,
     T_TINT = 259,
     T_TCHAR = 260,
     T_TLONG = 261,
     T_TFLOAT = 262,
     T_IDENT = 263,
     T_NUM = 264,
     T_FLOATING_POINT_NUMBER = 265,
     T_STRING = 266,
     T_OCTNUM = 267,
     T_CONST = 268,
     T_TRUE = 269,
     T_FALSE = 270,
     T_BOOLEAN = 271,
     T_FUNC = 272,
     T_MACRO = 273,
     T_DOTDOT = 274,
     T_PLUSPLUS = 275,
     T_MINUSMINUS = 276,
     T_IF = 277,
     T_ELIF = 278,
     T_ELSE = 279,
     T_WHILE = 280,
     T_GOTO = 281,
     T_BRAEQ = 282,
     T_LOCAL = 283,
     T_SWITCH = 284,
     T_BREAK = 285,
     T_CONTINUE = 286,
     T_RETURN = 287,
     T_NUMVAR = 288,
     T_FLOATVAR = 289,
     T_STRINGVAR = 290,
     T_BOOLEANVAR = 291,
     T_NMATCH = 292,
     T_ASSIGN = 293,
     T_NE = 294,
     T_EQ = 295,
     T_SHIFT_LEFT = 296,
     T_SHIFT_RIGHT = 297,
     T_OR = 298,
     T_AND = 299,
     T_LE = 300,
     T_GE = 301,
     T_POWER_TO = 302,
     T_DEFMACRO = 303,
     T_CMDSEQPREFIX = 304,
     T_GT = 305,
     T_LT = 306
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 270 "Parser.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   547

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  71
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  65
/* YYNRULES -- Number of rules.  */
#define YYNRULES  149
/* YYNRULES -- Number of states.  */
#define YYNSTATES  233

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   306

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    69,     2,     2,     2,    62,    55,     2,
      65,    66,    60,    57,    64,    58,     2,    61,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    70,    63,
      51,     2,    52,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    59,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    67,    56,    68,    50,     2,     2,     2,
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
      45,    46,    47,    48,    49,    53,    54
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    11,    13,    17,    19,
      23,    24,    25,    31,    33,    35,    37,    39,    41,    43,
      45,    47,    49,    52,    55,    56,    62,    63,    65,    69,
      71,    74,    77,    82,    83,    86,    91,    95,    97,   100,
     103,   105,   107,   109,   111,   113,   115,   117,   119,   121,
     123,   125,   128,   131,   133,   137,   139,   142,   145,   148,
     150,   152,   154,   157,   160,   163,   167,   171,   175,   179,
     183,   187,   191,   195,   199,   203,   207,   211,   215,   216,
     218,   221,   225,   229,   233,   237,   241,   245,   249,   253,
     257,   261,   263,   265,   267,   269,   271,   273,   275,   277,
     279,   281,   284,   288,   292,   295,   296,   298,   304,   305,
     309,   312,   315,   316,   321,   322,   323,   329,   331,   335,
     336,   337,   344,   345,   348,   349,   350,   354,   355,   361,
     362,   368,   370,   372,   376,   378,   380,   384,   386,   388,
     390,   392,   394,   396,   398,   400,   402,   404,   406,   408
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      72,     0,    -1,    -1,    -1,    73,    74,    72,    -1,    89,
      -1,    75,    -1,    13,    76,    63,    -1,    77,    -1,    76,
      64,    77,    -1,    -1,    -1,    78,    81,    38,    80,    79,
      -1,   135,    -1,    11,    -1,     8,    -1,    82,    -1,     9,
      -1,    11,    -1,    35,    -1,    33,    -1,    34,    -1,    48,
       8,    -1,    48,    17,    -1,    -1,    85,    83,    65,    86,
      66,    -1,    -1,    87,    -1,    87,    64,    88,    -1,    88,
      -1,   131,    81,    -1,    84,    90,    -1,    67,    91,   123,
      68,    -1,    -1,    92,    91,    -1,   131,    81,    95,    63,
      -1,   131,    81,    63,    -1,    63,    -1,   125,    63,    -1,
      94,    63,    -1,   119,    -1,   115,    -1,   110,    -1,   106,
      -1,   107,    -1,   108,    -1,   113,    -1,    90,    -1,    98,
      -1,   112,    -1,     1,    -1,    82,    95,    -1,    38,    96,
      -1,    97,    -1,    65,    97,    66,    -1,   100,    -1,    50,
      96,    -1,    82,    21,    -1,    82,    20,    -1,   104,    -1,
     125,    -1,    99,    -1,   130,    99,    -1,    57,   100,    -1,
      58,   100,    -1,   100,    55,   100,    -1,   100,    56,   100,
      -1,   100,    57,   100,    -1,   100,    59,   100,    -1,   100,
      58,   100,    -1,   100,    60,   100,    -1,   100,    61,   100,
      -1,   100,    62,   100,    -1,   100,    41,   100,    -1,   100,
      42,   100,    -1,   100,    47,   100,    -1,    65,   100,    66,
      -1,    65,   102,    66,    -1,    -1,   103,    -1,    69,   102,
      -1,   102,    44,   102,    -1,   102,    43,   102,    -1,   102,
      51,   102,    -1,   102,    52,   102,    -1,   102,    45,   102,
      -1,   102,    46,   102,    -1,   102,    40,   102,    -1,   102,
      39,   102,    -1,   100,    50,   100,    -1,   100,    37,   100,
      -1,   125,    -1,   104,    -1,    33,    -1,    34,    -1,    35,
      -1,    36,    -1,   105,    -1,   135,    -1,     8,    -1,    11,
      -1,   105,    11,    -1,    30,   109,    63,    -1,    31,   109,
      63,    -1,    32,   100,    -1,    -1,     9,    -1,    29,   101,
      67,   111,    68,    -1,    -1,   112,    90,   111,    -1,     8,
      70,    -1,    26,     8,    -1,    -1,    27,     8,   114,    63,
      -1,    -1,    -1,    25,   116,   101,   117,   118,    -1,    93,
      -1,    67,   123,    68,    -1,    -1,    -1,    22,   101,   120,
      93,   121,   122,    -1,    -1,    24,    93,    -1,    -1,    -1,
      93,   124,   123,    -1,    -1,    17,   126,    65,    86,    66,
      -1,    -1,     8,   127,    65,    86,    66,    -1,   128,    -1,
     129,    -1,   128,    64,   129,    -1,     1,    -1,   100,    -1,
      65,   131,    66,    -1,     5,    -1,     4,    -1,     6,    -1,
       7,    -1,    16,    -1,     3,    -1,    14,    -1,    15,    -1,
      10,    -1,     9,    -1,   134,    -1,   133,    -1,   132,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   127,   127,   128,   128,   134,   135,   140,   142,   143,
     145,   145,   145,   158,   162,   167,   168,   169,   176,   183,
     184,   185,   189,   192,   198,   198,   203,   204,   207,   209,
     211,   218,   233,   235,   236,   238,   247,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   269,   270,   271,
     272,   277,   282,   284,   285,   287,   288,   294,   296,   298,
     299,   301,   302,   306,   307,   308,   309,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   320,   324,   326,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   340,   341,   343,   347,   351,   355,   359,   364,   365,
     371,   372,   383,   386,   389,   393,   394,   396,   398,   399,
     401,   406,   410,   410,   415,   417,   415,   428,   428,   430,
     433,   430,   443,   444,   446,   447,   447,   449,   449,   458,
     458,   469,   471,   472,   473,   477,   479,   481,   482,   483,
     484,   485,   486,   488,   491,   496,   501,   506,   514,   522
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_TSTRING", "T_TINT", "T_TCHAR",
  "T_TLONG", "T_TFLOAT", "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER",
  "T_STRING", "T_OCTNUM", "T_CONST", "T_TRUE", "T_FALSE", "T_BOOLEAN",
  "T_FUNC", "T_MACRO", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS", "T_IF",
  "T_ELIF", "T_ELSE", "T_WHILE", "T_GOTO", "T_BRAEQ", "T_LOCAL",
  "T_SWITCH", "T_BREAK", "T_CONTINUE", "T_RETURN", "T_NUMVAR",
  "T_FLOATVAR", "T_STRINGVAR", "T_BOOLEANVAR", "T_NMATCH", "T_ASSIGN",
  "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR", "T_AND", "T_LE",
  "T_GE", "T_POWER_TO", "T_DEFMACRO", "T_CMDSEQPREFIX", "'~'", "'<'",
  "'>'", "T_GT", "T_LT", "'&'", "'|'", "'+'", "'-'", "'^'", "'*'", "'/'",
  "'%'", "';'", "','", "'('", "')'", "'{'", "'}'", "'!'", "':'", "$accept",
  "resources", "$@1", "resource", "constants", "constdefs", "constdef",
  "$@2", "$@3", "constant_literal", "variable_identifier",
  "variable_reference", "macro_declaration", "macrostart", "$@4",
  "parameter_list", "non_empty_pardecl", "par_decl", "macro_definition",
  "block", "var_decls", "var_decl", "statement", "assignment",
  "assignment_expression", "arithmetic_expression", "simple_expression",
  "increment_expression", "s_bterm", "bterm", "condition",
  "logical_expression", "term", "value", "string", "break", "continue",
  "return", "opt_num", "case", "caselabels", "label", "goto", "$@5",
  "while", "$@6", "$@7", "local_block", "if", "$@8", "$@9", "elseclause",
  "stmntlist", "$@10", "call_expression", "@11", "@12",
  "nonempty_parameters", "parameter", "type_cast", "type_ident",
  "boolean_literal", "float_literal", "integer_literal", "simple_literal", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     126,    60,    62,   305,   306,    38,   124,    43,    45,    94,
      42,    47,    37,    59,    44,    40,    41,   123,   125,    33,
      58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    71,    72,    73,    72,    74,    74,    75,    76,    76,
      78,    79,    77,    80,    80,    81,    81,    81,    81,    82,
      82,    82,    83,    83,    85,    84,    86,    86,    87,    87,
      88,    89,    90,    91,    91,    92,    92,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    94,    95,    96,    96,    97,    97,    98,    98,    99,
      99,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   101,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   103,   103,   104,   104,   104,   104,   104,   104,   104,
     105,   105,   106,   107,   108,   109,   109,   110,   111,   111,
     112,   113,   114,   113,   116,   117,   115,   118,   118,   120,
     121,   119,   122,   122,   123,   124,   123,   126,   125,   127,
     125,    86,   128,   128,   128,   129,   130,   131,   131,   131,
     131,   131,   131,   132,   132,   133,   134,   135,   135,   135
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     3,     1,     3,
       0,     0,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     0,     5,     0,     1,     3,     1,
       2,     2,     4,     0,     2,     4,     3,     1,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     3,     1,     2,     2,     2,     1,
       1,     1,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     0,     1,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     3,     3,     2,     0,     1,     5,     0,     3,
       2,     2,     0,     4,     0,     0,     5,     1,     3,     0,
       0,     6,     0,     2,     0,     0,     3,     0,     5,     0,
       5,     1,     1,     3,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    24,     1,    10,     3,     6,     0,     0,     5,
       0,     8,     0,     4,    33,    31,     0,     0,     7,    10,
      15,    17,    18,    20,    21,    19,     0,    16,   142,   138,
     137,   139,   140,   141,     0,    33,     0,    22,    23,     0,
       9,     0,    50,   129,   127,     0,   114,     0,     0,     0,
     105,   105,     0,    37,     0,    47,   125,     0,    48,    43,
      44,    45,    42,    49,    46,    41,    40,     0,     0,    34,
       0,   134,    99,   146,   145,   100,   143,   144,    93,    94,
      95,    96,     0,     0,     0,     0,    27,    29,    61,   135,
      59,    97,    60,   131,   132,     0,     0,   149,   148,   147,
      98,    14,    11,    13,   110,     0,     0,    78,   119,     0,
     111,   112,     0,   106,     0,     0,   104,    58,    57,     0,
      51,     0,    39,    32,    38,    36,     0,    63,    64,     0,
       0,    25,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   101,     0,    62,    30,    12,     0,
       0,    78,     0,     0,    79,    59,    60,     0,   115,     0,
     108,   102,   103,     0,     0,    52,    53,    55,   126,    35,
      76,   136,    28,    73,    74,    75,    65,    66,    67,    69,
      68,    70,    71,    72,   133,     0,     0,    80,     0,     0,
      78,    78,    78,    78,    78,    78,    78,    78,    77,   120,
       0,   113,     0,     0,     0,    56,     0,     0,   130,   128,
      90,    89,    88,    87,    82,    81,    85,    86,    83,    84,
     122,     0,   117,   116,   107,   108,    54,     0,   121,     0,
     109,   123,   118
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    10,    11,    12,   148,   102,
      26,    54,    17,     7,     8,    85,    86,    87,     9,    55,
      34,    35,    56,    57,   120,   165,   166,    58,    88,   152,
     108,   153,   154,    90,    91,    59,    60,    61,   114,    62,
     203,    63,    64,   159,    65,   109,   200,   223,    66,   157,
     220,   228,    67,   121,    92,   106,   105,    93,    94,    95,
      96,    97,    98,    99,   100
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -152
static const yytype_int16 yypact[] =
{
      10,    25,    16,  -152,  -152,    10,  -152,   -34,    13,  -152,
     -49,  -152,    51,  -152,   178,  -152,    59,     4,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,    42,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,     5,   178,    51,  -152,  -152,   294,
    -152,    81,  -152,    12,  -152,    22,  -152,    86,   104,    22,
     106,   106,   451,  -152,    43,  -152,  -152,    53,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,  -152,  -152,    52,    63,  -152,
       3,  -152,    66,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,   451,   451,   365,    74,    77,  -152,  -152,   485,
    -152,   134,  -152,    87,  -152,     9,    51,  -152,  -152,  -152,
    -152,  -152,  -152,  -152,  -152,    91,    92,   380,  -152,    22,
    -152,  -152,   107,  -152,   112,   113,   485,  -152,  -152,   417,
    -152,     5,  -152,  -152,  -152,  -152,   117,   130,   130,   436,
     120,  -152,   178,   451,   451,   451,   451,   451,   451,   451,
     451,   451,   451,   451,  -152,   451,  -152,  -152,  -152,   294,
     294,   380,   463,    78,  -152,   103,   127,   216,  -152,   124,
     180,  -152,  -152,   417,   328,  -152,  -152,   485,  -152,  -152,
    -152,  -152,  -152,   211,   211,  -152,   211,   211,   130,   130,
     130,  -152,  -152,  -152,  -152,   139,   140,   273,   451,   451,
     380,   380,   380,   380,   380,   380,   380,   380,  -152,  -152,
     259,  -152,    12,   128,   -34,  -152,   148,   436,  -152,  -152,
     485,   485,   366,   366,    33,    33,   273,   273,   165,   165,
     191,   196,  -152,  -152,  -152,   180,  -152,   216,  -152,   151,
    -152,  -152,  -152
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -152,   215,  -152,  -152,  -152,  -152,   197,  -152,  -152,  -152,
     -31,    -8,  -152,  -152,  -152,   -39,  -152,   100,  -152,    -6,
     199,  -152,  -150,  -152,   166,    72,    73,  -152,   144,   -36,
     -38,  -139,  -152,   -58,  -152,  -152,  -152,  -152,   189,  -152,
      19,  -151,  -152,  -152,  -152,  -152,  -152,  -152,  -152,  -152,
    -152,  -152,  -113,  -152,   -32,  -152,  -152,  -152,   108,  -152,
     -14,  -152,  -152,  -152,   213
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -130
static const yytype_int16 yytable[] =
{
      36,    15,    68,    89,    27,    70,    42,   199,   168,   204,
      -2,   112,   187,    43,    18,    19,   116,    72,    73,    74,
      75,    36,    44,    76,    77,     3,    44,    45,    27,     4,
      46,    47,    48,    14,    49,    50,    51,    52,    23,    24,
      25,   119,    78,    79,    80,    81,   127,   128,   129,   155,
     222,   212,   213,   214,   215,   216,   217,   218,   219,    20,
      21,    16,    22,   117,   118,   147,   125,    37,    53,    39,
     130,   158,    14,  -124,   204,   156,    38,   231,   194,   195,
      41,   119,   104,   167,    23,    24,    25,   107,    27,    68,
      73,    74,   101,   155,   110,    76,    77,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   229,    89,
     185,   186,   111,    89,    89,   113,   122,   190,   191,   156,
     123,   192,   193,   194,   195,    68,   124,   167,   207,   196,
     197,  -129,   155,   155,   155,   155,   155,   155,   155,   155,
     131,   132,   -92,   -92,   198,   144,   -92,   -92,   -92,   -92,
     130,   145,   210,   211,   -92,   -92,   149,   150,   156,   156,
     156,   156,   156,   156,   156,   156,   -91,   -91,    68,   -92,
     -91,   -91,   -91,   -91,   160,   161,   162,   135,   -91,   -91,
     169,    28,    29,    30,    31,    32,   171,   201,   202,    68,
     141,   142,   143,   -91,    33,    68,   224,    42,   225,    28,
      29,    30,    31,    32,    43,   208,   209,    36,   192,   193,
     194,   195,    33,    44,   226,   227,    40,    42,    45,   232,
      13,    46,    47,    48,    43,    49,    50,    51,    52,    23,
      24,    25,   172,    44,    69,   205,   126,   206,    45,   146,
     115,    46,    47,    48,   230,    49,    50,    51,    52,    23,
      24,    25,     0,   184,   103,     0,     0,     0,   135,    53,
      42,     0,     0,    14,   -33,     0,     0,    43,   138,   139,
     140,   141,   142,   143,     0,     0,    44,     0,     0,    53,
       0,    45,     0,    14,    46,    47,    48,     0,    49,    50,
      51,    52,    23,    24,    25,    71,     0,    28,    29,    30,
      31,    32,    72,    73,    74,    75,     0,     0,    76,    77,
      33,    44,   190,   191,     0,     0,   192,   193,   194,   195,
       0,     0,    53,     0,   196,   197,   221,    78,    79,    80,
      81,    28,    29,    30,    31,    32,    72,    73,    74,    75,
       0,     0,    76,    77,    33,    44,     0,     0,     0,     0,
       0,    82,    83,     0,     0,     0,     0,     0,     0,    84,
     -26,    78,    79,    80,    81,     0,     0,     0,    28,    29,
      30,    31,    32,    72,    73,    74,    75,     0,   163,    76,
      77,    33,    44,     0,     0,    82,    83,     0,    72,    73,
      74,    75,     0,    84,    76,    77,     0,    44,    78,    79,
      80,    81,     0,     0,     0,  -130,  -130,     0,     0,   192,
     193,   194,   195,    78,    79,    80,    81,   196,   197,     0,
       0,     0,    82,    83,     0,    72,    73,    74,    75,     0,
      84,    76,    77,     0,    44,     0,     0,    82,    83,     0,
       0,     0,     0,     0,     0,    84,     0,     0,     0,   151,
      78,    79,    80,    81,     0,     0,     0,     0,     0,    72,
      73,    74,    75,     0,     0,    76,    77,   163,    44,     0,
       0,     0,     0,     0,    82,    83,     0,   133,   134,     0,
       0,     0,   164,   135,    78,    79,    80,    81,     0,     0,
       0,   136,   137,   138,   139,   140,   141,   142,   143,     0,
     188,     0,   170,     0,   133,   134,     0,     0,    82,    83,
     135,     0,     0,   189,     0,     0,    84,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   133,   134,     0,     0,
       0,     0,   135,     0,     0,     0,     0,     0,     0,     0,
     136,   137,   138,   139,   140,   141,   142,   143
};

static const yytype_int16 yycheck[] =
{
      14,     7,    34,    39,    12,    36,     1,   157,   121,   160,
       0,    49,   151,     8,    63,    64,    52,     8,     9,    10,
      11,    35,    17,    14,    15,     0,    17,    22,    36,    13,
      25,    26,    27,    67,    29,    30,    31,    32,    33,    34,
      35,    38,    33,    34,    35,    36,    82,    83,    84,   107,
     200,   190,   191,   192,   193,   194,   195,   196,   197,     8,
       9,    48,    11,    20,    21,    96,    63,     8,    63,    65,
      84,   109,    67,    68,   225,   107,    17,   227,    45,    46,
      38,    38,    70,   119,    33,    34,    35,    65,    96,   121,
       9,    10,    11,   151,     8,    14,    15,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   221,   145,
     149,   150,     8,   149,   150,     9,    63,    39,    40,   151,
      68,    43,    44,    45,    46,   157,    63,   163,   164,    51,
      52,    65,   190,   191,   192,   193,   194,   195,   196,   197,
      66,    64,    39,    40,    66,    11,    43,    44,    45,    46,
     164,    64,   188,   189,    51,    52,    65,    65,   190,   191,
     192,   193,   194,   195,   196,   197,    39,    40,   200,    66,
      43,    44,    45,    46,    67,    63,    63,    47,    51,    52,
      63,     3,     4,     5,     6,     7,    66,    63,     8,   221,
      60,    61,    62,    66,    16,   227,    68,     1,   204,     3,
       4,     5,     6,     7,     8,    66,    66,   221,    43,    44,
      45,    46,    16,    17,    66,    24,    19,     1,    22,    68,
       5,    25,    26,    27,     8,    29,    30,    31,    32,    33,
      34,    35,   132,    17,    35,   163,    70,   164,    22,    95,
      51,    25,    26,    27,   225,    29,    30,    31,    32,    33,
      34,    35,    -1,   145,    41,    -1,    -1,    -1,    47,    63,
       1,    -1,    -1,    67,    68,    -1,    -1,     8,    57,    58,
      59,    60,    61,    62,    -1,    -1,    17,    -1,    -1,    63,
      -1,    22,    -1,    67,    25,    26,    27,    -1,    29,    30,
      31,    32,    33,    34,    35,     1,    -1,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    -1,    -1,    14,    15,
      16,    17,    39,    40,    -1,    -1,    43,    44,    45,    46,
      -1,    -1,    63,    -1,    51,    52,    67,    33,    34,    35,
      36,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      -1,    -1,    14,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    65,
      66,    33,    34,    35,    36,    -1,    -1,    -1,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    -1,    50,    14,
      15,    16,    17,    -1,    -1,    57,    58,    -1,     8,     9,
      10,    11,    -1,    65,    14,    15,    -1,    17,    33,    34,
      35,    36,    -1,    -1,    -1,    39,    40,    -1,    -1,    43,
      44,    45,    46,    33,    34,    35,    36,    51,    52,    -1,
      -1,    -1,    57,    58,    -1,     8,     9,    10,    11,    -1,
      65,    14,    15,    -1,    17,    -1,    -1,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    -1,    -1,    69,
      33,    34,    35,    36,    -1,    -1,    -1,    -1,    -1,     8,
       9,    10,    11,    -1,    -1,    14,    15,    50,    17,    -1,
      -1,    -1,    -1,    -1,    57,    58,    -1,    41,    42,    -1,
      -1,    -1,    65,    47,    33,    34,    35,    36,    -1,    -1,
      -1,    55,    56,    57,    58,    59,    60,    61,    62,    -1,
      37,    -1,    66,    -1,    41,    42,    -1,    -1,    57,    58,
      47,    -1,    -1,    50,    -1,    -1,    65,    -1,    55,    56,
      57,    58,    59,    60,    61,    62,    41,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    56,    57,    58,    59,    60,    61,    62
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    72,    73,     0,    13,    74,    75,    84,    85,    89,
      76,    77,    78,    72,    67,    90,    48,    83,    63,    64,
       8,     9,    11,    33,    34,    35,    81,    82,     3,     4,
       5,     6,     7,    16,    91,    92,   131,     8,    17,    65,
      77,    38,     1,     8,    17,    22,    25,    26,    27,    29,
      30,    31,    32,    63,    82,    90,    93,    94,    98,   106,
     107,   108,   110,   112,   113,   115,   119,   123,   125,    91,
      81,     1,     8,     9,    10,    11,    14,    15,    33,    34,
      35,    36,    57,    58,    65,    86,    87,    88,    99,   100,
     104,   105,   125,   128,   129,   130,   131,   132,   133,   134,
     135,    11,    80,   135,    70,   127,   126,    65,   101,   116,
       8,     8,   101,     9,   109,   109,   100,    20,    21,    38,
      95,   124,    63,    68,    63,    63,    95,   100,   100,   100,
     131,    66,    64,    41,    42,    47,    55,    56,    57,    58,
      59,    60,    61,    62,    11,    64,    99,    81,    79,    65,
      65,    69,   100,   102,   103,   104,   125,   120,   101,   114,
      67,    63,    63,    50,    65,    96,    97,   100,   123,    63,
      66,    66,    88,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   129,    86,    86,   102,    37,    50,
      39,    40,    43,    44,    45,    46,    51,    52,    66,    93,
     117,    63,     8,   111,   112,    96,    97,   100,    66,    66,
     100,   100,   102,   102,   102,   102,   102,   102,   102,   102,
     121,    67,    93,   118,    68,    90,    66,    24,   122,   123,
     111,    93,    68
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
	    /* Fall through.  */
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

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:

/* Line 1455 of yacc.c  */
#line 128 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 145 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 145 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 146 "Parser.y"
    {
				BOOL bString = ((yyvsp[(4) - (5)]).v.type == S_STRING);
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (5)]).ident.s,
					bString ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(4) - (5)]).v.data);
				free((yyvsp[(2) - (5)]).ident.s);
				if (bString) {
					free((yyvsp[(4) - (5)]).v.data.string);
				}
			;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 158 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
				(yyval).v.type = S_NUMBER;
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 162 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = S_STRING;
			;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 167 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 168 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 170 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 177 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 183 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 184 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 185 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 189 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 192 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 198 "Parser.y"
    { startmacro(); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 199 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (5)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 211 "Parser.y"
    {
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 218 "Parser.y"
    {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (2)]).ident.s, -1, 1);
					YY_EMIT(C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",(yyvsp[(1) - (2)]).ident.s);
					_compilerConfiguration->cb_insertNewMacro((yyvsp[(1) - (2)]).ident.s, _yyCurrentComment, &_bytecodeBuffer);
				}
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				endmacro();
			;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 238 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (4)]).ident.s,
					(yyvsp[(1) - (4)]).type,
					((yyvsp[(1) - (4)]).type == S_STRING) ? (intptr_t)"" : 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(2) - (4)]).ident.s);
				if ((yyvsp[(2) - (4)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (4)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 247 "Parser.y"
    {
				if ((yyvsp[(1) - (3)]).type == S_STRING) {
					makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, (intptr_t)"");
				} else {
					makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, (intptr_t)0);
				}
				if ((yyvsp[(2) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (3)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 272 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 277 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 282 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 284 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 285 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 287 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 288 "Parser.y"
    { 
				(yyval).type = S_BOOLEAN;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_NOT, 0);
			;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 294 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 296 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 298 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 301 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 302 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 306 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 307 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 308 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 309 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 310 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 311 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 312 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 313 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 314 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 315 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 316 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 317 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 318 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 320 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 328 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 329 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 330 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 331 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 332 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 333 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 334 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 336 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 337 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 338 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 340 "Parser.y"
    { (yyval).type = S_NUMBER; 	 	;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 341 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 343 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_NUMBER;
			;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 347 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_FLOAT;
			;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_STRING;
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 355 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_BOOLEAN;
			;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 359 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 364 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 365 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 371 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident;	;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 372 "Parser.y"
    { 
				if (((yyval).ident.s = malloc(strlen((yyvsp[(1) - (2)]).ident.s)+strlen((yyvsp[(2) - (2)]).ident.s)+1)) != 0) {
					strcat(strcpy((yyval).ident.s,(yyvsp[(1) - (2)]).ident.s),(yyvsp[(2) - (2)]).ident.s);
					free((yyvsp[(1) - (2)]).ident.s);
					free((yyvsp[(2) - (2)]).ident.s);
				} else {
					yyerror("buffer overlow");
					(yyval).ident = (yyvsp[(2) - (2)]).ident;
				}
			;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 384 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 387 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 389 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 393 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 401 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 406 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 410 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 415 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 420 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 430 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 433 "Parser.y"
    {
				_iflevel--;
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 438 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 447 "Parser.y"
    { YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 449 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (1)]).funcp;
				_currentFunctionCallParamIndex = 0;
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 454 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, _currentFunctionCallParamIndex);
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 458 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndex = 0;
				_calledFunctionOrMacro = (yyvsp[(1) - (1)]).ident.s;
				(yyval).funcp = 0;
				YY_EMIT(C_SET_PARAMETER_STACK,(GENERIC_DATA){0});
			;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    {
				YY_EMIT(C_MACRO,(GENERIC_DATA){.string = _calledFunctionOrMacro});
				freeitem(&_calledFunctionOrMacro);
			;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 473 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 477 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; _currentFunctionCallParamIndex++; ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 481 "Parser.y"
    {	(yyval).type = S_CHARACTER;	;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 482 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 483 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 484 "Parser.y"
    {	(yyval).type = S_FLOAT;	;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 485 "Parser.y"
    {	(yyval).type = S_BOOLEAN; ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 486 "Parser.y"
    {	(yyval).type = S_STRING;	;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 488 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 491 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 496 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 501 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 506 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.intValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_NUMBER;
			;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 514 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_FLOAT;
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 522 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_BOOLEAN;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 2691 "Parser.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 530 "Parser.y"


/**
 * bytecode_emitPushParameterInstruction()
 * Special processing when pushing integer literals: if the integer literal is pushed as 
 * parameter of a native function and this is form-related native function push a special
 * byte code as a marker.
 */
static void bytecode_emitPushParameterInstruction(long lValue)
{
	int opCode = C_PUSH_INTEGER_LITERAL;
	if (_currentNativeMethodCalled && function_parameterIsFormStart(_currentNativeMethodCalled,_currentFunctionCallParamIndex+1)) {
		opCode = C_FORM_START;
	}
	YY_EMIT(opCode, (GENERIC_DATA){.val = lValue});
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

/*---------------------------------*/
/* startmacro()				*/
/*---------------------------------*/
static void startmacro(void)
{
#ifdef YYDEBUG
int	yydebug = 1;
#endif
	if (!_bytecodeBuffer.bb_start) {
		_bytecodeBuffer.bb_end = ((_bytecodeBuffer.bb_start = malloc(REC_SPACE)) == 0) ? 
			_bytecodeBuffer.bb_start : _bytecodeBuffer.bb_start + REC_SPACE;
	}
	_bytecodeBuffer.bb_current = _bytecodeBuffer.bb_start; 
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	yyerrflg = 0;
	_currentIdentifierContext = sym_pushContext(_currentIdentifierContext);
}

/*---------------------------------*/
/* endmacro()					*/
/*---------------------------------*/
static void endmacro(void) {
	if (_bytecodeBuffer.bb_start) {
		free(_bytecodeBuffer.bb_start);
		_bytecodeBuffer.bb_start = 0;
		_bytecodeBuffer.bb_end = 0;
	}
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


