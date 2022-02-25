
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
#line 42 "Parser.y"


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

static int		_generateShorthandAssignment;
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
extern unsigned char* bytecode_emitShorthandAssignment(BYTECODE_BUFFER* pBuffer, int nBinaryOp, char* pszName);
extern unsigned char* bytecode_emitAssignment(BYTECODE_BUFFER* pBuffer, const char *name);
extern unsigned char* bytecode_emitFunctionCall(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data, int nArgs);

/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern unsigned char* bytecode_emitMultiplyWithLiteralExpression(BYTECODE_BUFFER* pBuffer, TYPEDVAL* v1, int nNumber);

#define		YY_EMIT(opCode, param)		_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, opCode, param);

extern int yylex(void );
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
#line 179 "Parser.tab.c"

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
     T_SH_ASSIGN_PLUS = 283,
     T_SH_ASSIGN_MINUS = 284,
     T_SH_ASSIGN_MULT = 285,
     T_SH_ASSIGN_DIV = 286,
     T_SH_ASSIGN_MOD = 287,
     T_LOCAL = 288,
     T_SWITCH = 289,
     T_BREAK = 290,
     T_CONTINUE = 291,
     T_RETURN = 292,
     T_NUMVAR = 293,
     T_FLOATVAR = 294,
     T_STRINGVAR = 295,
     T_BOOLEANVAR = 296,
     T_NMATCH = 297,
     T_ASSIGN = 298,
     T_NE = 299,
     T_EQ = 300,
     T_SHIFT_LEFT = 301,
     T_SHIFT_RIGHT = 302,
     T_OR = 303,
     T_AND = 304,
     T_LE = 305,
     T_GE = 306,
     T_POWER_TO = 307,
     T_DEFMACRO = 308,
     T_CMDSEQPREFIX = 309,
     T_GT = 310,
     T_LT = 311
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
#line 277 "Parser.tab.c"

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
#define YYLAST   567

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  69
/* YYNRULES -- Number of rules.  */
#define YYNRULES  159
/* YYNRULES -- Number of states.  */
#define YYNSTATES  243

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    74,     2,     2,     2,    67,    60,     2,
      70,    71,    65,    62,    69,    63,     2,    66,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    75,    68,
      56,     2,    57,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    64,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    72,    61,    73,    55,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      58,    59
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
     123,   125,   128,   130,   133,   135,   137,   139,   141,   143,
     146,   148,   152,   154,   157,   160,   163,   165,   167,   169,
     172,   175,   178,   182,   186,   190,   194,   198,   202,   206,
     210,   214,   218,   222,   226,   230,   231,   233,   236,   240,
     244,   248,   252,   256,   260,   264,   268,   272,   276,   278,
     280,   282,   284,   286,   288,   290,   292,   294,   296,   299,
     303,   307,   310,   311,   313,   314,   316,   322,   323,   327,
     330,   333,   334,   339,   340,   341,   347,   349,   353,   354,
     355,   362,   363,   364,   368,   369,   370,   374,   375,   381,
     382,   388,   390,   392,   396,   398,   400,   404,   406,   408,
     410,   412,   414,   416,   418,   420,   422,   424,   426,   428
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      77,     0,    -1,    -1,    -1,    78,    79,    77,    -1,    94,
      -1,    80,    -1,    13,    81,    68,    -1,    82,    -1,    81,
      69,    82,    -1,    -1,    -1,    83,    86,    43,    85,    84,
      -1,   144,    -1,    11,    -1,     8,    -1,    87,    -1,     9,
      -1,    11,    -1,    40,    -1,    38,    -1,    39,    -1,    53,
       8,    -1,    53,    17,    -1,    -1,    90,    88,    70,    91,
      71,    -1,    -1,    92,    -1,    92,    69,    93,    -1,    93,
      -1,   140,    86,    -1,    89,    95,    -1,    72,    96,   132,
      73,    -1,    -1,    97,    96,    -1,   140,    86,   102,    68,
      -1,   140,    86,    68,    -1,    68,    -1,   134,    68,    -1,
      99,    68,    -1,   127,    -1,   123,    -1,   118,    -1,   113,
      -1,   114,    -1,   115,    -1,   121,    -1,    95,    -1,   105,
      -1,   120,    -1,     1,    -1,    87,   100,    -1,   102,    -1,
     101,   103,    -1,    30,    -1,    31,    -1,    32,    -1,    28,
      -1,    29,    -1,    43,   103,    -1,   104,    -1,    70,   104,
      71,    -1,   107,    -1,    55,   103,    -1,    87,    21,    -1,
      87,    20,    -1,   111,    -1,   134,    -1,   106,    -1,   139,
     106,    -1,    62,   107,    -1,    63,   107,    -1,   107,    60,
     107,    -1,   107,    61,   107,    -1,   107,    62,   107,    -1,
     107,    64,   107,    -1,   107,    63,   107,    -1,   107,    65,
     107,    -1,   107,    66,   107,    -1,   107,    67,   107,    -1,
     107,    46,   107,    -1,   107,    47,   107,    -1,   107,    52,
     107,    -1,    70,   107,    71,    -1,    70,   109,    71,    -1,
      -1,   110,    -1,    74,   109,    -1,   109,    49,   109,    -1,
     109,    48,   109,    -1,   109,    56,   109,    -1,   109,    57,
     109,    -1,   109,    50,   109,    -1,   109,    51,   109,    -1,
     109,    45,   109,    -1,   109,    44,   109,    -1,   107,    55,
     107,    -1,   107,    42,   107,    -1,   134,    -1,   111,    -1,
      38,    -1,    39,    -1,    40,    -1,    41,    -1,   112,    -1,
     144,    -1,     8,    -1,    11,    -1,   112,    11,    -1,    35,
     117,    68,    -1,    36,   117,    68,    -1,    37,   116,    -1,
      -1,   107,    -1,    -1,     9,    -1,    34,   108,    72,   119,
      73,    -1,    -1,   120,    95,   119,    -1,     8,    75,    -1,
      26,     8,    -1,    -1,    27,     8,   122,    68,    -1,    -1,
      -1,    25,   124,   108,   125,   126,    -1,    98,    -1,    72,
     132,    73,    -1,    -1,    -1,    22,   108,   128,   126,   129,
     130,    -1,    -1,    -1,    24,   131,   126,    -1,    -1,    -1,
      98,   133,   132,    -1,    -1,    17,   135,    70,    91,    71,
      -1,    -1,     8,   136,    70,    91,    71,    -1,   137,    -1,
     138,    -1,   137,    69,   138,    -1,     1,    -1,   107,    -1,
      70,   140,    71,    -1,     5,    -1,     4,    -1,     6,    -1,
       7,    -1,    16,    -1,     3,    -1,    14,    -1,    15,    -1,
      10,    -1,     9,    -1,   143,    -1,   142,    -1,   141,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   130,   130,   131,   131,   137,   138,   143,   145,   146,
     148,   148,   148,   161,   165,   170,   171,   172,   179,   186,
     187,   188,   192,   195,   201,   201,   206,   207,   210,   212,
     214,   221,   236,   238,   239,   241,   250,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   280,   291,   296,   302,   303,   304,   305,   306,   308,
     310,   311,   313,   314,   320,   322,   324,   325,   327,   328,
     332,   333,   334,   335,   336,   337,   338,   339,   340,   341,
     342,   343,   344,   346,   350,   352,   353,   354,   355,   356,
     357,   358,   359,   360,   361,   362,   363,   364,   366,   367,
     369,   373,   377,   381,   385,   390,   391,   397,   398,   409,
     412,   416,   420,   421,   423,   424,   426,   428,   429,   431,
     436,   440,   440,   445,   447,   445,   458,   458,   461,   464,
     461,   471,   475,   475,   482,   483,   483,   489,   489,   498,
     498,   509,   511,   512,   513,   517,   519,   521,   522,   523,
     524,   525,   526,   528,   531,   536,   541,   546,   554,   562
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
  "T_ELIF", "T_ELSE", "T_WHILE", "T_GOTO", "T_BRAEQ", "T_SH_ASSIGN_PLUS",
  "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV",
  "T_SH_ASSIGN_MOD", "T_LOCAL", "T_SWITCH", "T_BREAK", "T_CONTINUE",
  "T_RETURN", "T_NUMVAR", "T_FLOATVAR", "T_STRINGVAR", "T_BOOLEANVAR",
  "T_NMATCH", "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT",
  "T_OR", "T_AND", "T_LE", "T_GE", "T_POWER_TO", "T_DEFMACRO",
  "T_CMDSEQPREFIX", "'~'", "'<'", "'>'", "T_GT", "T_LT", "'&'", "'|'",
  "'+'", "'-'", "'^'", "'*'", "'/'", "'%'", "';'", "','", "'('", "')'",
  "'{'", "'}'", "'!'", "':'", "$accept", "resources", "$@1", "resource",
  "constants", "constdefs", "constdef", "$@2", "$@3", "constant_literal",
  "variable_identifier", "variable_reference", "macro_declaration",
  "macrostart", "$@4", "parameter_list", "non_empty_pardecl", "par_decl",
  "macro_definition", "block", "var_decls", "var_decl", "statement",
  "assignment", "assignment_or_shorthand_assignment",
  "shorthand_assignment_operator", "assignment_expression",
  "arithmetic_expression", "simple_expression", "increment_expression",
  "s_bterm", "bterm", "condition", "logical_expression", "term", "value",
  "string", "break", "continue", "return_expression", "optional_bterm",
  "opt_num", "case", "caselabels", "label", "goto", "$@5", "while", "$@6",
  "$@7", "local_block", "if_expression", "$@8", "$@9", "elseclause",
  "$@10", "stmntlist", "$@11", "call_expression", "@12", "@13",
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
     305,   306,   307,   308,   309,   126,    60,    62,   310,   311,
      38,   124,    43,    45,    94,    42,    47,    37,    59,    44,
      40,    41,   123,   125,    33,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    78,    77,    79,    79,    80,    81,    81,
      83,    84,    82,    85,    85,    86,    86,    86,    86,    87,
      87,    87,    88,    88,    90,    89,    91,    91,    92,    92,
      93,    94,    95,    96,    96,    97,    97,    98,    98,    98,
      98,    98,    98,    98,    98,    98,    98,    98,    98,    98,
      98,    99,   100,   100,   101,   101,   101,   101,   101,   102,
     103,   103,   104,   104,   105,   105,   106,   106,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   107,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   109,   109,   109,   110,   110,
     111,   111,   111,   111,   111,   111,   111,   112,   112,   113,
     114,   115,   116,   116,   117,   117,   118,   119,   119,   120,
     121,   122,   121,   124,   125,   123,   126,   126,   128,   129,
     127,   130,   131,   130,   132,   133,   132,   135,   134,   136,
     134,    91,   137,   137,   137,   138,   139,   140,   140,   140,
     140,   140,   140,   141,   141,   142,   143,   144,   144,   144
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     3,     1,     3,
       0,     0,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     0,     5,     0,     1,     3,     1,
       2,     2,     4,     0,     2,     4,     3,     1,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     2,     1,     1,     1,     1,     1,     2,
       1,     3,     1,     2,     2,     2,     1,     1,     1,     2,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     0,     1,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     3,
       3,     2,     0,     1,     0,     1,     5,     0,     3,     2,
       2,     0,     4,     0,     0,     5,     1,     3,     0,     0,
       6,     0,     0,     3,     0,     0,     3,     0,     5,     0,
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
      15,    17,    18,    20,    21,    19,     0,    16,   152,   148,
     147,   149,   150,   151,     0,    33,     0,    22,    23,     0,
       9,     0,    50,   139,   137,     0,   123,     0,     0,     0,
     114,   114,   112,    37,     0,    47,   135,     0,    48,    43,
      44,    45,    42,    49,    46,    41,    40,     0,     0,    34,
       0,   144,   106,   156,   155,   107,   153,   154,   100,   101,
     102,   103,     0,     0,     0,     0,    27,    29,    68,   145,
      66,   104,    67,   141,   142,     0,     0,   159,   158,   157,
     105,    14,    11,    13,   119,     0,     0,    85,   128,     0,
     120,   121,     0,   115,     0,     0,   113,   111,    65,    64,
      57,    58,    54,    55,    56,     0,    51,     0,    52,     0,
      39,    32,    38,    36,     0,    70,    71,     0,     0,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   108,     0,    69,    30,    12,     0,     0,    85,
       0,     0,    86,    66,    67,     0,   124,     0,   117,   109,
     110,     0,     0,    59,    60,    62,    53,   136,    35,    83,
     146,    28,    80,    81,    82,    72,    73,    74,    76,    75,
      77,    78,    79,   143,     0,     0,    87,     0,     0,    85,
      85,    85,    85,    85,    85,    85,    85,    84,     0,   126,
     129,     0,   122,     0,     0,     0,    63,     0,     0,   140,
     138,    97,    96,    95,    94,    89,    88,    92,    93,    90,
      91,     0,   131,   125,   116,   117,    61,   127,   132,   130,
     118,     0,   133
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    10,    11,    12,   156,   102,
      26,    54,    17,     7,     8,    85,    86,    87,     9,    55,
      34,    35,    56,    57,   126,   127,   128,   173,   174,    58,
      88,   160,   108,   161,   162,    90,    91,    59,    60,    61,
     117,   114,    62,   214,    63,    64,   167,    65,   109,   211,
     210,    66,   165,   232,   239,   241,    67,   129,    92,   106,
     105,    93,    94,    95,    96,    97,    98,    99,   100
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -204
static const yytype_int16 yypact[] =
{
      11,    21,    14,  -204,  -204,    11,  -204,   -49,   -18,  -204,
     -38,  -204,   149,  -204,    74,  -204,     7,   -33,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,    -4,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,   217,    74,   149,  -204,  -204,   257,
    -204,    84,  -204,   -24,  -204,   -26,  -204,    35,    65,   -26,
      67,    67,   423,  -204,   249,  -204,  -204,    15,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,  -204,    19,    34,  -204,
     -11,  -204,    33,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,   423,   423,   338,    45,    49,  -204,  -204,   486,
    -204,    93,  -204,    50,  -204,   115,   149,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,    58,    61,   353,  -204,   -26,
    -204,  -204,    48,  -204,    66,    69,   486,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,   373,  -204,   373,  -204,   217,
    -204,  -204,  -204,  -204,    70,    20,    20,   464,    68,  -204,
      74,   423,   423,   423,   423,   423,   423,   423,   423,   423,
     423,   423,  -204,   423,  -204,  -204,  -204,   257,   257,   353,
     442,   -31,  -204,   397,   421,    28,  -204,    72,   141,  -204,
    -204,   373,   296,  -204,  -204,   486,  -204,  -204,  -204,  -204,
    -204,  -204,   355,   355,  -204,   355,   355,    20,    20,    20,
    -204,  -204,  -204,  -204,    79,    80,   273,   423,   423,   353,
     353,   353,   353,   353,   353,   353,   353,  -204,   177,  -204,
    -204,    28,  -204,   -24,    90,   -49,  -204,    81,   464,  -204,
    -204,   486,   486,   510,   510,   -17,   -17,   273,   273,    10,
      10,    91,   142,  -204,  -204,   141,  -204,  -204,  -204,  -204,
    -204,    28,  -204
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -204,   160,  -204,  -204,  -204,  -204,   156,  -204,  -204,  -204,
     -27,    -8,  -204,  -204,  -204,  -116,  -204,    37,  -204,    -7,
     155,  -204,  -159,  -204,  -204,  -204,   121,  -115,    24,  -204,
      97,   -36,   -39,    27,  -204,   -58,  -204,  -204,  -204,  -204,
    -204,   146,  -204,   -37,  -161,  -204,  -204,  -204,  -204,  -204,
    -203,  -204,  -204,  -204,  -204,  -204,  -124,  -204,   -32,  -204,
    -204,  -204,    47,  -204,   -13,  -204,  -204,  -204,   164
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -140
static const yytype_int16 yytable[] =
{
      15,    36,    68,    89,    27,   177,   209,   215,   233,    70,
     112,    -2,   176,   199,   200,    37,   116,   201,   202,   203,
     204,     3,    36,    14,    38,   205,   206,     4,    27,    42,
      18,    19,   125,   203,   204,    16,    43,    39,   242,    41,
     207,   194,   195,   110,   107,    44,   135,   136,   137,   163,
      45,   104,   209,    46,    47,    48,   216,   133,   201,   202,
     203,   204,    49,    50,    51,    52,    23,    24,    25,   155,
     166,   138,   143,   111,   215,   164,   113,    28,    29,    30,
      31,    32,   209,   130,   231,   149,   150,   151,    27,   175,
      33,   175,   131,    73,    74,   101,    53,    68,    76,    77,
     208,   163,   132,  -139,   152,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   139,    89,   140,   153,
     168,    89,    89,    72,    73,    74,    75,   164,   157,    76,
      77,   158,    44,    68,   169,   175,   218,   170,   178,   180,
     212,   163,   163,   163,   163,   163,   163,   163,   163,   213,
     219,   220,   236,    78,    79,    80,    81,    20,    21,   138,
      22,   221,   222,   234,   237,    13,   238,   164,   164,   164,
     164,   164,   164,   164,   164,    40,    68,   181,    42,    68,
      28,    29,    30,    31,    32,    43,   196,    23,    24,    25,
      69,   134,   154,    33,    44,    36,   217,   115,   240,    45,
     193,     0,    46,    47,    48,   103,     0,     0,   235,    68,
       0,    49,    50,    51,    52,    23,    24,    25,    42,     0,
       0,     0,     0,     0,     0,    43,   223,   224,   225,   226,
     227,   228,   229,   230,    44,     0,     0,     0,     0,    45,
       0,     0,    46,    47,    48,    53,     0,     0,     0,    14,
     -33,    49,    50,    51,    52,    23,    24,    25,    71,     0,
      28,    29,    30,    31,    32,    72,    73,    74,    75,   118,
     119,    76,    77,    33,    44,     0,     0,   120,   121,   122,
     123,   124,     0,     0,     0,    53,     0,     0,     0,    14,
    -134,     0,   125,     0,     0,    78,    79,    80,    81,    28,
      29,    30,    31,    32,    72,    73,    74,    75,     0,     0,
      76,    77,    33,    44,     0,     0,     0,   199,   200,    82,
      83,   201,   202,   203,   204,     0,     0,    84,   -26,   205,
     206,     0,     0,     0,    78,    79,    80,    81,     0,     0,
       0,    28,    29,    30,    31,    32,    72,    73,    74,    75,
       0,   171,    76,    77,    33,    44,     0,     0,    82,    83,
       0,    72,    73,    74,    75,     0,    84,    76,    77,     0,
      44,     0,     0,     0,     0,     0,    78,    79,    80,    81,
       0,    72,    73,    74,    75,     0,     0,    76,    77,     0,
      44,    78,    79,    80,    81,     0,     0,     0,     0,     0,
      82,    83,     0,     0,     0,     0,     0,   143,    84,     0,
       0,    78,    79,    80,    81,    82,    83,   146,   147,   148,
     149,   150,   151,    84,     0,     0,     0,   159,   171,     0,
       0,    72,    73,    74,    75,    82,    83,    76,    77,     0,
      44,   -99,   -99,   172,     0,   -99,   -99,   -99,   -99,     0,
       0,     0,     0,   -99,   -99,     0,     0,     0,     0,     0,
       0,    78,    79,    80,    81,   -98,   -98,     0,   -99,   -98,
     -98,   -98,   -98,     0,     0,     0,     0,   -98,   -98,     0,
       0,     0,     0,     0,   197,    82,    83,     0,   141,   142,
       0,     0,   -98,    84,   143,     0,     0,   198,     0,     0,
       0,     0,   144,   145,   146,   147,   148,   149,   150,   151,
     141,   142,     0,     0,     0,     0,   143,     0,     0,     0,
       0,     0,     0,     0,   144,   145,   146,   147,   148,   149,
     150,   151,   141,   142,     0,   179,     0,     0,   143,     0,
       0,     0,     0,     0,     0,     0,   144,   145,   146,   147,
     148,   149,   150,   151,  -140,  -140,     0,     0,   201,   202,
     203,   204,     0,     0,     0,     0,   205,   206
};

static const yytype_int16 yycheck[] =
{
       7,    14,    34,    39,    12,   129,   165,   168,   211,    36,
      49,     0,   127,    44,    45,     8,    52,    48,    49,    50,
      51,     0,    35,    72,    17,    56,    57,    13,    36,     1,
      68,    69,    43,    50,    51,    53,     8,    70,   241,    43,
      71,   157,   158,     8,    70,    17,    82,    83,    84,   107,
      22,    75,   211,    25,    26,    27,   171,    68,    48,    49,
      50,    51,    34,    35,    36,    37,    38,    39,    40,    96,
     109,    84,    52,     8,   235,   107,     9,     3,     4,     5,
       6,     7,   241,    68,   208,    65,    66,    67,    96,   125,
      16,   127,    73,     9,    10,    11,    68,   129,    14,    15,
      72,   159,    68,    70,    11,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,    71,   153,    69,    69,
      72,   157,   158,     8,     9,    10,    11,   159,    70,    14,
      15,    70,    17,   165,    68,   171,   172,    68,    68,    71,
      68,   199,   200,   201,   202,   203,   204,   205,   206,     8,
      71,    71,    71,    38,    39,    40,    41,     8,     9,   172,
      11,   197,   198,    73,    73,     5,    24,   199,   200,   201,
     202,   203,   204,   205,   206,    19,   208,   140,     1,   211,
       3,     4,     5,     6,     7,     8,   159,    38,    39,    40,
      35,    70,    95,    16,    17,   208,   172,    51,   235,    22,
     153,    -1,    25,    26,    27,    41,    -1,    -1,   215,   241,
      -1,    34,    35,    36,    37,    38,    39,    40,     1,    -1,
      -1,    -1,    -1,    -1,    -1,     8,   199,   200,   201,   202,
     203,   204,   205,   206,    17,    -1,    -1,    -1,    -1,    22,
      -1,    -1,    25,    26,    27,    68,    -1,    -1,    -1,    72,
      73,    34,    35,    36,    37,    38,    39,    40,     1,    -1,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    20,
      21,    14,    15,    16,    17,    -1,    -1,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    68,    -1,    -1,    -1,    72,
      73,    -1,    43,    -1,    -1,    38,    39,    40,    41,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    -1,    -1,
      14,    15,    16,    17,    -1,    -1,    -1,    44,    45,    62,
      63,    48,    49,    50,    51,    -1,    -1,    70,    71,    56,
      57,    -1,    -1,    -1,    38,    39,    40,    41,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      -1,    55,    14,    15,    16,    17,    -1,    -1,    62,    63,
      -1,     8,     9,    10,    11,    -1,    70,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    38,    39,    40,    41,
      -1,     8,     9,    10,    11,    -1,    -1,    14,    15,    -1,
      17,    38,    39,    40,    41,    -1,    -1,    -1,    -1,    -1,
      62,    63,    -1,    -1,    -1,    -1,    -1,    52,    70,    -1,
      -1,    38,    39,    40,    41,    62,    63,    62,    63,    64,
      65,    66,    67,    70,    -1,    -1,    -1,    74,    55,    -1,
      -1,     8,     9,    10,    11,    62,    63,    14,    15,    -1,
      17,    44,    45,    70,    -1,    48,    49,    50,    51,    -1,
      -1,    -1,    -1,    56,    57,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    39,    40,    41,    44,    45,    -1,    71,    48,
      49,    50,    51,    -1,    -1,    -1,    -1,    56,    57,    -1,
      -1,    -1,    -1,    -1,    42,    62,    63,    -1,    46,    47,
      -1,    -1,    71,    70,    52,    -1,    -1,    55,    -1,    -1,
      -1,    -1,    60,    61,    62,    63,    64,    65,    66,    67,
      46,    47,    -1,    -1,    -1,    -1,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    60,    61,    62,    63,    64,    65,
      66,    67,    46,    47,    -1,    71,    -1,    -1,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,    63,
      64,    65,    66,    67,    44,    45,    -1,    -1,    48,    49,
      50,    51,    -1,    -1,    -1,    -1,    56,    57
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    77,    78,     0,    13,    79,    80,    89,    90,    94,
      81,    82,    83,    77,    72,    95,    53,    88,    68,    69,
       8,     9,    11,    38,    39,    40,    86,    87,     3,     4,
       5,     6,     7,    16,    96,    97,   140,     8,    17,    70,
      82,    43,     1,     8,    17,    22,    25,    26,    27,    34,
      35,    36,    37,    68,    87,    95,    98,    99,   105,   113,
     114,   115,   118,   120,   121,   123,   127,   132,   134,    96,
      86,     1,     8,     9,    10,    11,    14,    15,    38,    39,
      40,    41,    62,    63,    70,    91,    92,    93,   106,   107,
     111,   112,   134,   137,   138,   139,   140,   141,   142,   143,
     144,    11,    85,   144,    75,   136,   135,    70,   108,   124,
       8,     8,   108,     9,   117,   117,   107,   116,    20,    21,
      28,    29,    30,    31,    32,    43,   100,   101,   102,   133,
      68,    73,    68,    68,   102,   107,   107,   107,   140,    71,
      69,    46,    47,    52,    60,    61,    62,    63,    64,    65,
      66,    67,    11,    69,   106,    86,    84,    70,    70,    74,
     107,   109,   110,   111,   134,   128,   108,   122,    72,    68,
      68,    55,    70,   103,   104,   107,   103,   132,    68,    71,
      71,    93,   107,   107,   107,   107,   107,   107,   107,   107,
     107,   107,   107,   138,    91,    91,   109,    42,    55,    44,
      45,    48,    49,    50,    51,    56,    57,    71,    72,    98,
     126,   125,    68,     8,   119,   120,   103,   104,   107,    71,
      71,   107,   107,   109,   109,   109,   109,   109,   109,   109,
     109,   132,   129,   126,    73,    95,    71,    73,    24,   130,
     119,   131,   126
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
#line 131 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 148 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 148 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 149 "Parser.y"
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
#line 161 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
				(yyval).v.type = S_NUMBER;
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 165 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = S_STRING;
			;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 170 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 171 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 173 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 180 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 186 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 187 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 188 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 192 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 195 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 201 "Parser.y"
    { startmacro(); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 202 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (5)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 214 "Parser.y"
    {
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 221 "Parser.y"
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
#line 241 "Parser.y"
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
#line 250 "Parser.y"
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

  case 38:

/* Line 1455 of yacc.c  */
#line 263 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 264 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 265 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 266 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 267 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 268 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 269 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 270 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 271 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 272 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 273 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 274 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 275 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 280 "Parser.y"
    {
				if (_generateShorthandAssignment) {
					_bytecodeBuffer.bb_current = bytecode_emitShorthandAssignment(&_bytecodeBuffer, _generateShorthandAssignment, (yyvsp[(1) - (2)]).ident.s);
				} else {
					_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				}
				vname_count = 0;
				_generateShorthandAssignment = 0;
			;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 291 "Parser.y"
    {
				_generateShorthandAssignment = 0;
				(yyval).v = (yyvsp[(1) - (1)]).v;
			;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 296 "Parser.y"
    { 
				_generateShorthandAssignment = (yyvsp[(1) - (2)]).type;
				(yyval).v = (yyvsp[(2) - (2)]).v;
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 302 "Parser.y"
    { (yyval).type = BIN_MUL; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 303 "Parser.y"
    { (yyval).type = BIN_DIV; ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 304 "Parser.y"
    { (yyval).type = BIN_MOD; ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 305 "Parser.y"
    { (yyval).type = BIN_ADD; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 306 "Parser.y"
    { (yyval).type = BIN_SUB; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 308 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 310 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 311 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 313 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 314 "Parser.y"
    { 
				(yyval).type = S_BOOLEAN;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_NOT, 0);
			;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 320 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 322 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 324 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 327 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 328 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 332 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 333 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 334 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 336 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 337 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 338 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 339 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 340 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 341 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 342 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 343 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 344 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 346 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 354 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 355 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 356 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 357 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 358 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 359 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 363 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 364 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 366 "Parser.y"
    { (yyval).type = S_NUMBER; 	 	;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 367 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 369 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_NUMBER;
			;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 373 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_FLOAT;
			;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 377 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_STRING;
			;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 381 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_BOOLEAN;
			;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 385 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 390 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 391 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 397 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident;	;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 398 "Parser.y"
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

  case 109:

/* Line 1455 of yacc.c  */
#line 410 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 413 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 416 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 423 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 424 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 431 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 436 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 440 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 445 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 447 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 450 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 461 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    {
				_iflevel--;
			;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 466 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 471 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 475 "Parser.y"
    {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 482 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 483 "Parser.y"
    { 
				if ((yyvsp[(1) - (1)]).needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 489 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (1)]).funcp;
				_currentFunctionCallParamIndex = 0;
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 494 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, _currentFunctionCallParamIndex);
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 498 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndex = 0;
				_calledFunctionOrMacro = (yyvsp[(1) - (1)]).ident.s;
				(yyval).funcp = 0;
				YY_EMIT(C_SET_PARAMETER_STACK,(GENERIC_DATA){0});
			;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 504 "Parser.y"
    {
				YY_EMIT(C_MACRO,(GENERIC_DATA){.string = _calledFunctionOrMacro});
				freeitem(&_calledFunctionOrMacro);
			;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 513 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 517 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; _currentFunctionCallParamIndex++; ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 519 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 521 "Parser.y"
    {	(yyval).type = S_CHARACTER;	;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 522 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 523 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 524 "Parser.y"
    {	(yyval).type = S_FLOAT;	;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 525 "Parser.y"
    {	(yyval).type = S_BOOLEAN; ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 526 "Parser.y"
    {	(yyval).type = S_STRING;	;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 528 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 531 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 536 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 541 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 546 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.intValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_NUMBER;
			;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 554 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_FLOAT;
			;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 562 "Parser.y"
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
#line 2888 "Parser.tab.c"
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
#line 570 "Parser.y"


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


