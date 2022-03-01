
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
#line 45 "Parser.y"


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
								 t == C_PUSH_SMALL_INT_LITERAL)
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

static SYMBOL_TYPE constantTypeFor(SYMBOL_TYPE aValueType) {
	switch(aValueType) {
	case S_FLOAT: return S_CONSTFLOAT;
	case S_STRING: return S_CONSTSTRING;
	case S_CHARACTER: return S_CONSTCHARACTER;
	}
	return S_CONSTNUM;
}



/* Line 189 of yacc.c  */
#line 188 "Parser.tab.c"

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
     T_RANGE = 263,
     T_IDENT = 264,
     T_NUM = 265,
     T_FLOATING_POINT_NUMBER = 266,
     T_STRING = 267,
     T_CHARACTER = 268,
     T_CONST = 269,
     T_TRUE = 270,
     T_FALSE = 271,
     T_TBOOLEAN = 272,
     T_FUNC = 273,
     T_MACRO = 274,
     T_DOTDOT = 275,
     T_PLUSPLUS = 276,
     T_MINUSMINUS = 277,
     T_IF = 278,
     T_ELIF = 279,
     T_ELSE = 280,
     T_WHILE = 281,
     T_GOTO = 282,
     T_BRAEQ = 283,
     T_SH_ASSIGN_PLUS = 284,
     T_SH_ASSIGN_MINUS = 285,
     T_SH_ASSIGN_MULT = 286,
     T_SH_ASSIGN_DIV = 287,
     T_SH_ASSIGN_MOD = 288,
     T_LOCAL = 289,
     T_SWITCH = 290,
     T_BREAK = 291,
     T_CONTINUE = 292,
     T_RETURN = 293,
     T_NUMVAR = 294,
     T_RANGEVAR = 295,
     T_FLOATVAR = 296,
     T_STRINGVAR = 297,
     T_BOOLEANVAR = 298,
     T_CHARACTERVAR = 299,
     T_NMATCH = 300,
     T_ASSIGN = 301,
     T_NE = 302,
     T_EQ = 303,
     T_SHIFT_LEFT = 304,
     T_SHIFT_RIGHT = 305,
     T_OR = 306,
     T_AND = 307,
     T_LE = 308,
     T_GE = 309,
     T_POWER_TO = 310,
     T_VOID = 311,
     T_CMDSEQPREFIX = 312,
     T_GT = 313,
     T_LT = 314
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
#line 289 "Parser.tab.c"

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
#define YYLAST   682

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  71
/* YYNRULES -- Number of rules.  */
#define YYNRULES  177
/* YYNRULES -- Number of states.  */
#define YYNSTATES  264

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   314

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    79,     2,     2,     2,    70,    58,     2,
      74,    75,    68,    66,    73,    67,     2,    69,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    80,    72,
      62,     2,    63,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    71,     2,    78,    60,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    76,    59,    77,    61,     2,     2,     2,
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
      55,    56,    57,    64,    65
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    11,    13,    17,    19,
      23,    24,    25,    31,    33,    35,    37,    39,    41,    43,
      45,    47,    49,    51,    53,    55,    58,    61,    63,    65,
      67,    69,    71,    73,    75,    76,    82,    83,    85,    89,
      91,    94,    97,   102,   103,   106,   111,   115,   117,   120,
     123,   125,   127,   129,   131,   133,   135,   137,   139,   141,
     143,   145,   148,   150,   153,   155,   157,   159,   161,   163,
     166,   168,   172,   174,   179,   183,   186,   189,   191,   193,
     195,   198,   201,   204,   207,   210,   214,   218,   222,   226,
     230,   234,   238,   242,   246,   250,   254,   258,   262,   263,
     265,   268,   272,   276,   280,   284,   288,   292,   296,   300,
     304,   308,   310,   312,   314,   316,   318,   320,   322,   324,
     326,   328,   330,   332,   335,   339,   343,   346,   347,   349,
     350,   352,   358,   359,   363,   366,   369,   370,   375,   376,
     377,   383,   385,   389,   390,   391,   398,   399,   400,   404,
     405,   406,   410,   411,   417,   418,   424,   426,   428,   432,
     434,   436,   440,   442,   444,   446,   448,   450,   452,   454,
     456,   458,   460,   462,   464,   466,   468,   470
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      82,     0,    -1,    -1,    -1,    83,    84,    82,    -1,   100,
      -1,    85,    -1,    14,    86,    72,    -1,    87,    -1,    86,
      73,    87,    -1,    -1,    -1,    88,    91,    46,    90,    89,
      -1,   151,    -1,    12,    -1,     9,    -1,    92,    -1,    10,
      -1,    12,    -1,    42,    -1,    39,    -1,    40,    -1,    44,
      -1,    43,    -1,    41,    -1,    94,     9,    -1,    94,    18,
      -1,    56,    -1,     3,    -1,    17,    -1,     4,    -1,     5,
      -1,     6,    -1,     7,    -1,    -1,    96,    93,    74,    97,
      75,    -1,    -1,    98,    -1,    98,    73,    99,    -1,    99,
      -1,   146,    91,    -1,    95,   101,    -1,    76,   102,   138,
      77,    -1,    -1,   103,   102,    -1,   146,    91,   108,    72,
      -1,   146,    91,    72,    -1,    72,    -1,   140,    72,    -1,
     105,    72,    -1,   133,    -1,   129,    -1,   124,    -1,   119,
      -1,   120,    -1,   121,    -1,   127,    -1,   101,    -1,   111,
      -1,   126,    -1,     1,    -1,    92,   106,    -1,   108,    -1,
     107,   109,    -1,    31,    -1,    32,    -1,    33,    -1,    29,
      -1,    30,    -1,    46,   109,    -1,   110,    -1,    74,   110,
      75,    -1,   113,    -1,   113,    71,   113,    78,    -1,   113,
      20,   113,    -1,    92,    22,    -1,    92,    21,    -1,   117,
      -1,   140,    -1,   112,    -1,   145,   112,    -1,    67,    10,
      -1,    61,   113,    -1,    66,   113,    -1,    67,   113,    -1,
     113,    58,   113,    -1,   113,    59,   113,    -1,   113,    66,
     113,    -1,   113,    60,   113,    -1,   113,    67,   113,    -1,
     113,    68,   113,    -1,   113,    69,   113,    -1,   113,    70,
     113,    -1,   113,    49,   113,    -1,   113,    50,   113,    -1,
     113,    55,   113,    -1,    74,   113,    75,    -1,    74,   115,
      75,    -1,    -1,   116,    -1,    79,   115,    -1,   115,    52,
     115,    -1,   115,    51,   115,    -1,   115,    62,   115,    -1,
     115,    63,   115,    -1,   115,    53,   115,    -1,   115,    54,
     115,    -1,   115,    48,   115,    -1,   115,    47,   115,    -1,
     113,    61,   113,    -1,   113,    45,   113,    -1,   140,    -1,
     117,    -1,    39,    -1,    41,    -1,    40,    -1,    42,    -1,
      44,    -1,    43,    -1,   118,    -1,   151,    -1,     9,    -1,
      12,    -1,   118,    12,    -1,    36,   123,    72,    -1,    37,
     123,    72,    -1,    38,   122,    -1,    -1,   113,    -1,    -1,
      10,    -1,    35,   114,    76,   125,    77,    -1,    -1,   126,
     101,   125,    -1,     9,    80,    -1,    27,     9,    -1,    -1,
      28,     9,   128,    72,    -1,    -1,    -1,    26,   130,   114,
     131,   132,    -1,   104,    -1,    76,   138,    77,    -1,    -1,
      -1,    23,   114,   134,   132,   135,   136,    -1,    -1,    -1,
      25,   137,   132,    -1,    -1,    -1,   104,   139,   138,    -1,
      -1,    18,   141,    74,    97,    75,    -1,    -1,     9,   142,
      74,    97,    75,    -1,   143,    -1,   144,    -1,   143,    73,
     144,    -1,     1,    -1,   113,    -1,    74,   146,    75,    -1,
       5,    -1,     4,    -1,     6,    -1,     7,    -1,    17,    -1,
       3,    -1,     8,    -1,    15,    -1,    16,    -1,    11,    -1,
      10,    -1,    13,    -1,   149,    -1,   148,    -1,   150,    -1,
     147,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   142,   142,   143,   143,   149,   150,   155,   157,   158,
     160,   160,   160,   172,   175,   180,   181,   182,   189,   196,
     197,   198,   199,   200,   201,   205,   208,   214,   215,   216,
     217,   218,   219,   220,   222,   222,   227,   228,   231,   233,
     235,   242,   257,   259,   260,   262,   271,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   301,   312,   317,   323,   324,   325,   326,   327,   329,
     331,   332,   334,   335,   336,   339,   341,   343,   344,   346,
     347,   351,   360,   361,   362,   363,   364,   365,   366,   367,
     368,   369,   370,   371,   372,   373,   374,   378,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   389,   390,   391,
     392,   394,   395,   397,   401,   405,   409,   413,   417,   421,
     426,   427,   433,   434,   445,   448,   452,   456,   457,   459,
     460,   462,   464,   465,   467,   472,   476,   476,   481,   483,
     481,   494,   494,   497,   500,   497,   507,   511,   511,   518,
     519,   519,   525,   525,   535,   535,   546,   548,   549,   550,
     554,   556,   558,   559,   560,   561,   562,   563,   564,   566,
     569,   574,   579,   584,   589,   597,   605,   613
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_TSTRING", "T_TINT", "T_TCHAR",
  "T_TLONG", "T_TFLOAT", "T_RANGE", "T_IDENT", "T_NUM",
  "T_FLOATING_POINT_NUMBER", "T_STRING", "T_CHARACTER", "T_CONST",
  "T_TRUE", "T_FALSE", "T_TBOOLEAN", "T_FUNC", "T_MACRO", "T_DOTDOT",
  "T_PLUSPLUS", "T_MINUSMINUS", "T_IF", "T_ELIF", "T_ELSE", "T_WHILE",
  "T_GOTO", "T_BRAEQ", "T_SH_ASSIGN_PLUS", "T_SH_ASSIGN_MINUS",
  "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV", "T_SH_ASSIGN_MOD", "T_LOCAL",
  "T_SWITCH", "T_BREAK", "T_CONTINUE", "T_RETURN", "T_NUMVAR",
  "T_RANGEVAR", "T_FLOATVAR", "T_STRINGVAR", "T_BOOLEANVAR",
  "T_CHARACTERVAR", "T_NMATCH", "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT",
  "T_SHIFT_RIGHT", "T_OR", "T_AND", "T_LE", "T_GE", "T_POWER_TO", "T_VOID",
  "T_CMDSEQPREFIX", "'&'", "'|'", "'^'", "'~'", "'<'", "'>'", "T_GT",
  "T_LT", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "';'", "','", "'('",
  "')'", "'{'", "'}'", "']'", "'!'", "':'", "$accept", "resources", "$@1",
  "resource", "constants", "constdefs", "constdef", "$@2", "$@3",
  "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "$@4", "parameter_list",
  "non_empty_pardecl", "par_decl", "macro_definition", "block",
  "var_decls", "var_decl", "statement", "assignment",
  "assignment_or_shorthand_assignment", "shorthand_assignment_operator",
  "assignment_expression", "arithmetic_expression", "simple_expression",
  "increment_expression", "s_bterm", "binary_expression", "condition",
  "logical_expression", "term", "value", "string", "break", "continue",
  "return_expression", "optional_bterm", "opt_num", "case", "caselabels",
  "label", "goto", "$@5", "while", "$@6", "$@7", "local_block",
  "if_expression", "$@8", "$@9", "elseclause", "$@10", "stmntlist", "$@11",
  "call_expression", "@12", "@13", "nonempty_parameters", "parameter",
  "type_cast", "type_ident", "boolean_literal", "float_literal",
  "integer_literal", "character_literal", "simple_literal", 0
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
     305,   306,   307,   308,   309,   310,   311,   312,    38,   124,
      94,   126,    60,    62,   313,   314,    43,    45,    42,    47,
      37,    91,    59,    44,    40,    41,   123,   125,    93,    33,
      58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    81,    82,    83,    82,    84,    84,    85,    86,    86,
      88,    89,    87,    90,    90,    91,    91,    91,    91,    92,
      92,    92,    92,    92,    92,    93,    93,    94,    94,    94,
      94,    94,    94,    94,    96,    95,    97,    97,    98,    98,
      99,   100,   101,   102,   102,   103,   103,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   105,   106,   106,   107,   107,   107,   107,   107,   108,
     109,   109,   110,   110,   110,   111,   111,   112,   112,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   114,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   116,   116,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   118,   118,   119,   120,   121,   122,   122,   123,
     123,   124,   125,   125,   126,   127,   128,   127,   130,   131,
     129,   132,   132,   134,   135,   133,   136,   137,   136,   138,
     139,   138,   141,   140,   142,   140,    97,   143,   143,   143,
     144,   145,   146,   146,   146,   146,   146,   146,   146,   147,
     147,   148,   149,   150,   151,   151,   151,   151
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     3,     1,     3,
       0,     0,     5,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     0,     5,     0,     1,     3,     1,
       2,     2,     4,     0,     2,     4,     3,     1,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     2,     1,     1,     1,     1,     1,     2,
       1,     3,     1,     4,     3,     2,     2,     1,     1,     1,
       2,     2,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     0,     1,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     2,     0,     1,     0,
       1,     5,     0,     3,     2,     2,     0,     4,     0,     0,
       5,     1,     3,     0,     0,     6,     0,     0,     3,     0,
       0,     3,     0,     5,     0,     5,     1,     1,     3,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    34,     1,    10,     3,     6,     0,     0,     5,
       0,     8,     0,     4,    43,    41,    28,    30,    31,    32,
      33,    29,    27,     0,     0,     7,    10,    15,    17,    18,
      20,    21,    24,    19,    23,    22,     0,    16,   167,   163,
     162,   164,   165,   168,   166,     0,    43,     0,     0,    25,
      26,     9,     0,    60,   154,   152,     0,   138,     0,     0,
       0,   129,   129,   127,    47,     0,    57,   150,     0,    58,
      53,    54,    55,    52,    59,    56,    51,    50,     0,     0,
      44,     0,   159,   121,   172,   171,   122,   173,   169,   170,
     113,   115,   114,   116,   118,   117,     0,     0,     0,     0,
       0,    37,    39,    79,   160,    77,   119,    78,   156,   157,
       0,     0,   177,   175,   174,   176,   120,    14,    11,    13,
     134,     0,     0,    98,   143,     0,   135,   136,     0,   130,
       0,     0,   128,   126,    76,    75,    67,    68,    64,    65,
      66,     0,    61,     0,    62,     0,    49,    42,    48,    46,
       0,    82,    83,    81,    84,     0,     0,    35,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     123,     0,    80,    40,    12,     0,     0,    98,     0,     0,
      99,    77,    78,     0,   139,     0,   132,   124,   125,     0,
      69,    70,    72,    63,   151,    45,    96,   161,    38,    93,
      94,    95,    85,    86,    88,    87,    89,    90,    91,    92,
     158,     0,     0,   100,     0,     0,    98,    98,    98,    98,
      98,    98,    98,    98,    97,     0,   141,   144,     0,   137,
       0,     0,     0,     0,     0,     0,     0,   155,   153,   110,
     109,   108,   107,   102,   101,   105,   106,   103,   104,     0,
     146,   140,   131,   132,    71,    74,     0,   142,   147,   145,
     133,    73,     0,   148
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    10,    11,    12,   174,   118,
      36,    65,    23,    24,     7,     8,   100,   101,   102,     9,
      66,    45,    46,    67,    68,   142,   143,   144,   190,   191,
      69,   103,   178,   124,   179,   180,   105,   106,    70,    71,
      72,   133,   130,    73,   231,    74,    75,   185,    76,   125,
     228,   227,    77,   183,   250,   259,   262,    78,   145,   107,
     122,   121,   108,   109,   110,   111,   112,   113,   114,   115,
     116
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -216
static const yytype_int16 yypact[] =
{
      12,    18,     2,  -216,  -216,    12,  -216,   -40,    25,  -216,
     -58,  -216,   214,  -216,    16,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,   -33,    17,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,     8,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,   241,    16,   214,   332,  -216,
    -216,  -216,   292,  -216,   -35,  -216,   -16,  -216,    50,    51,
     -16,    54,    54,   421,  -216,   610,  -216,  -216,   -11,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,    -8,    -4,
    -216,   -38,  -216,     6,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,   421,   421,   457,   374,
       9,    13,  -216,  -216,   595,  -216,    73,  -216,    14,  -216,
     504,   214,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,    19,    20,   410,  -216,   -16,  -216,  -216,    15,  -216,
      24,    26,   595,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,   493,  -216,   493,  -216,   241,  -216,  -216,  -216,  -216,
      27,   138,   -30,  -216,   -30,   541,    37,  -216,    16,   421,
     421,   421,   421,   421,   421,   421,   421,   421,   421,   421,
    -216,   421,  -216,  -216,  -216,   332,   332,   410,   568,   247,
    -216,   349,   531,   288,  -216,    28,    79,  -216,  -216,   374,
    -216,  -216,    91,  -216,  -216,  -216,  -216,  -216,  -216,    97,
      97,  -216,   138,   138,   138,   -30,   -30,  -216,  -216,  -216,
    -216,    49,    52,   619,   421,   421,   410,   410,   410,   410,
     410,   410,   410,   410,  -216,   194,  -216,  -216,   288,  -216,
     -35,    55,   -40,    53,     7,   421,   421,  -216,  -216,   595,
     595,   198,   198,   502,   502,   619,   619,   -10,   -10,    57,
      67,  -216,  -216,    79,  -216,   595,   503,  -216,  -216,  -216,
    -216,  -216,   288,  -216
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -216,   121,  -216,  -216,  -216,  -216,   105,  -216,  -216,  -216,
     -41,    -1,  -216,  -216,  -216,  -216,  -127,  -216,   -23,  -216,
      -7,    90,  -216,  -173,  -216,  -216,  -216,    56,    -5,   -47,
    -216,    34,   -46,   -53,   135,  -216,  -114,  -216,  -216,  -216,
    -216,  -216,    83,  -216,  -106,  -182,  -216,  -216,  -216,  -216,
    -216,  -215,  -216,  -216,  -216,  -216,  -216,  -142,  -216,   -44,
    -216,  -216,  -216,   -18,  -216,    -9,  -216,  -216,  -216,  -216,
      96
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -155
static const yytype_int16 yytable[] =
{
      15,    79,   104,   194,   232,    47,    81,   128,   141,   181,
     226,    37,    -2,   251,    25,    26,     4,   132,     3,    38,
      39,    40,    41,    42,    43,   161,    49,   235,    16,    17,
      18,    19,    20,    44,   149,    50,    14,    47,   167,   168,
     169,    48,    21,   220,   221,   120,    37,   263,   211,   212,
     151,   152,   154,   155,    52,   226,   159,   160,   123,   126,
     127,   146,   161,   181,   129,   162,   163,   164,   148,   147,
     173,   232,   184,   165,   166,   167,   168,   169,   236,   182,
    -154,    22,   196,   249,   157,   170,   158,   171,   230,   226,
     156,   186,   258,   175,   176,   192,   187,   192,   188,   195,
     229,    79,   181,   181,   181,   181,   181,   181,   181,   181,
      37,   235,   197,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   237,   104,    13,   238,   254,   104,
     104,    51,   252,   182,   257,   198,    80,   150,   193,    79,
     159,   160,   233,   234,   172,   131,   161,   260,   119,   162,
     163,   164,   161,   210,     0,     0,     0,   165,   166,   167,
     168,   169,   236,   165,   166,   167,   168,   169,   239,   240,
       0,     0,   182,   182,   182,   182,   182,   182,   182,   182,
     156,    79,     0,     0,    79,     0,     0,   159,   160,   255,
     256,     0,     0,   161,     0,    53,     0,    38,    39,    40,
      41,    42,    43,    54,   165,   166,   167,   168,   169,     0,
       0,    44,    55,     0,     0,     0,    47,    56,    79,     0,
      57,    58,    59,    27,    28,   253,    29,     0,     0,    60,
      61,    62,    63,    30,    31,    32,    33,    34,    35,     0,
       0,     0,    53,     0,     0,  -155,  -155,     0,     0,     0,
      54,   220,   221,    30,    31,    32,    33,    34,    35,    55,
     222,   223,     0,     0,    56,     0,    64,    57,    58,    59,
      14,   -43,     0,     0,     0,     0,    60,    61,    62,    63,
      30,    31,    32,    33,    34,    35,     0,     0,     0,    53,
       0,     0,     0,     0,   216,   217,     0,    54,   218,   219,
     220,   221,    84,    85,   117,    87,    55,    88,    89,   222,
     223,    56,   213,    64,    57,    58,    59,    14,  -149,     0,
       0,     0,   224,    60,    61,    62,    63,    30,    31,    32,
      33,    34,    35,    82,     0,    38,    39,    40,    41,    42,
      43,    83,    84,    85,    86,    87,     0,    88,    89,    44,
      55,   241,   242,   243,   244,   245,   246,   247,   248,     0,
      64,     0,     0,     0,   225,     0,     0,     0,     0,     0,
       0,    90,    91,    92,    93,    94,    95,    38,    39,    40,
      41,    42,    43,    83,    84,    85,    86,    87,     0,    88,
      89,    44,    55,    96,     0,     0,  -112,  -112,    97,    98,
    -112,  -112,  -112,  -112,     0,     0,    99,   -36,     0,     0,
       0,  -112,  -112,    90,    91,    92,    93,    94,    95,    83,
      84,    85,    86,    87,  -112,    88,    89,     0,    55,     0,
      83,    84,    85,    86,    87,    96,    88,    89,     0,    55,
      97,    98,     0,     0,     0,     0,     0,     0,    99,    90,
      91,    92,    93,    94,    95,     0,     0,     0,     0,     0,
      90,    91,    92,    93,    94,    95,    83,   153,    85,    86,
      87,    96,    88,    89,     0,    55,    97,    98,     0,     0,
       0,     0,    96,     0,    99,     0,     0,    97,    98,   177,
       0,     0,     0,     0,     0,    99,    90,    91,    92,    93,
      94,    95,    83,    84,    85,    86,    87,     0,    88,    89,
       0,    55,     0,    83,    84,    85,    86,    87,    96,    88,
      89,     0,    55,    97,    98,     0,     0,     0,     0,     0,
       0,    99,    90,    91,    92,    93,    94,    95,     0,     0,
       0,     0,     0,    90,    91,    92,    93,    94,    95,   216,
     217,     0,   159,   160,    96,   220,   221,     0,   161,    97,
      98,   162,   163,   164,   222,   223,     0,   189,     0,   165,
     166,   167,   168,   169,     0,     0,     0,     0,  -111,  -111,
       0,   261,  -111,  -111,  -111,  -111,     0,     0,     0,     0,
     159,   160,     0,  -111,  -111,     0,   161,     0,     0,   162,
     163,   164,     0,     0,     0,     0,  -111,   165,   166,   167,
     168,   169,     0,   214,     0,     0,   196,   159,   160,     0,
       0,     0,     0,   161,     0,     0,   162,   163,   164,   215,
       0,   134,   135,     0,   165,   166,   167,   168,   169,   136,
     137,   138,   139,   140,   159,   160,     0,     0,     0,     0,
     161,     0,     0,   162,   163,   164,   141,     0,     0,     0,
       0,   165,   166,   167,   168,   169,   216,   217,     0,     0,
     218,   219,   220,   221,     0,     0,     0,     0,     0,     0,
       0,   222,   223
};

static const yytype_int16 yycheck[] =
{
       7,    45,    48,   145,   186,    14,    47,    60,    46,   123,
     183,    12,     0,   228,    72,    73,    14,    63,     0,     3,
       4,     5,     6,     7,     8,    55,     9,    20,     3,     4,
       5,     6,     7,    17,    72,    18,    76,    46,    68,    69,
      70,    74,    17,    53,    54,    80,    47,   262,   175,   176,
      96,    97,    98,    99,    46,   228,    49,    50,    74,     9,
       9,    72,    55,   177,    10,    58,    59,    60,    72,    77,
     111,   253,   125,    66,    67,    68,    69,    70,    71,   123,
      74,    56,    75,   225,    75,    12,    73,    73,     9,   262,
      99,    76,    25,    74,    74,   141,    72,   143,    72,    72,
      72,   145,   216,   217,   218,   219,   220,   221,   222,   223,
     111,    20,    75,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,    75,   171,     5,    75,    75,   175,
     176,    26,    77,   177,    77,   158,    46,    81,   143,   183,
      49,    50,   189,   189,   110,    62,    55,   253,    52,    58,
      59,    60,    55,   171,    -1,    -1,    -1,    66,    67,    68,
      69,    70,    71,    66,    67,    68,    69,    70,   214,   215,
      -1,    -1,   216,   217,   218,   219,   220,   221,   222,   223,
     189,   225,    -1,    -1,   228,    -1,    -1,    49,    50,   235,
     236,    -1,    -1,    55,    -1,     1,    -1,     3,     4,     5,
       6,     7,     8,     9,    66,    67,    68,    69,    70,    -1,
      -1,    17,    18,    -1,    -1,    -1,   225,    23,   262,    -1,
      26,    27,    28,     9,    10,   232,    12,    -1,    -1,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    -1,
      -1,    -1,     1,    -1,    -1,    47,    48,    -1,    -1,    -1,
       9,    53,    54,    39,    40,    41,    42,    43,    44,    18,
      62,    63,    -1,    -1,    23,    -1,    72,    26,    27,    28,
      76,    77,    -1,    -1,    -1,    -1,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    -1,    -1,    -1,     1,
      -1,    -1,    -1,    -1,    47,    48,    -1,     9,    51,    52,
      53,    54,    10,    11,    12,    13,    18,    15,    16,    62,
      63,    23,   177,    72,    26,    27,    28,    76,    77,    -1,
      -1,    -1,    75,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,     1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    -1,    15,    16,    17,
      18,   216,   217,   218,   219,   220,   221,   222,   223,    -1,
      72,    -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    40,    41,    42,    43,    44,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    -1,    15,
      16,    17,    18,    61,    -1,    -1,    47,    48,    66,    67,
      51,    52,    53,    54,    -1,    -1,    74,    75,    -1,    -1,
      -1,    62,    63,    39,    40,    41,    42,    43,    44,     9,
      10,    11,    12,    13,    75,    15,    16,    -1,    18,    -1,
       9,    10,    11,    12,    13,    61,    15,    16,    -1,    18,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    74,    39,
      40,    41,    42,    43,    44,    -1,    -1,    -1,    -1,    -1,
      39,    40,    41,    42,    43,    44,     9,    10,    11,    12,
      13,    61,    15,    16,    -1,    18,    66,    67,    -1,    -1,
      -1,    -1,    61,    -1,    74,    -1,    -1,    66,    67,    79,
      -1,    -1,    -1,    -1,    -1,    74,    39,    40,    41,    42,
      43,    44,     9,    10,    11,    12,    13,    -1,    15,    16,
      -1,    18,    -1,     9,    10,    11,    12,    13,    61,    15,
      16,    -1,    18,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    74,    39,    40,    41,    42,    43,    44,    -1,    -1,
      -1,    -1,    -1,    39,    40,    41,    42,    43,    44,    47,
      48,    -1,    49,    50,    61,    53,    54,    -1,    55,    66,
      67,    58,    59,    60,    62,    63,    -1,    74,    -1,    66,
      67,    68,    69,    70,    -1,    -1,    -1,    -1,    47,    48,
      -1,    78,    51,    52,    53,    54,    -1,    -1,    -1,    -1,
      49,    50,    -1,    62,    63,    -1,    55,    -1,    -1,    58,
      59,    60,    -1,    -1,    -1,    -1,    75,    66,    67,    68,
      69,    70,    -1,    45,    -1,    -1,    75,    49,    50,    -1,
      -1,    -1,    -1,    55,    -1,    -1,    58,    59,    60,    61,
      -1,    21,    22,    -1,    66,    67,    68,    69,    70,    29,
      30,    31,    32,    33,    49,    50,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    58,    59,    60,    46,    -1,    -1,    -1,
      -1,    66,    67,    68,    69,    70,    47,    48,    -1,    -1,
      51,    52,    53,    54,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    62,    63
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    82,    83,     0,    14,    84,    85,    95,    96,   100,
      86,    87,    88,    82,    76,   101,     3,     4,     5,     6,
       7,    17,    56,    93,    94,    72,    73,     9,    10,    12,
      39,    40,    41,    42,    43,    44,    91,    92,     3,     4,
       5,     6,     7,     8,    17,   102,   103,   146,    74,     9,
      18,    87,    46,     1,     9,    18,    23,    26,    27,    28,
      35,    36,    37,    38,    72,    92,   101,   104,   105,   111,
     119,   120,   121,   124,   126,   127,   129,   133,   138,   140,
     102,    91,     1,     9,    10,    11,    12,    13,    15,    16,
      39,    40,    41,    42,    43,    44,    61,    66,    67,    74,
      97,    98,    99,   112,   113,   117,   118,   140,   143,   144,
     145,   146,   147,   148,   149,   150,   151,    12,    90,   151,
      80,   142,   141,    74,   114,   130,     9,     9,   114,    10,
     123,   123,   113,   122,    21,    22,    29,    30,    31,    32,
      33,    46,   106,   107,   108,   139,    72,    77,    72,    72,
     108,   113,   113,    10,   113,   113,   146,    75,    73,    49,
      50,    55,    58,    59,    60,    66,    67,    68,    69,    70,
      12,    73,   112,    91,    89,    74,    74,    79,   113,   115,
     116,   117,   140,   134,   114,   128,    76,    72,    72,    74,
     109,   110,   113,   109,   138,    72,    75,    75,    99,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     144,    97,    97,   115,    45,    61,    47,    48,    51,    52,
      53,    54,    62,    63,    75,    76,   104,   132,   131,    72,
       9,   125,   126,   110,   113,    20,    71,    75,    75,   113,
     113,   115,   115,   115,   115,   115,   115,   115,   115,   138,
     135,   132,    77,   101,    75,   113,   113,    77,    25,   136,
     125,    78,   137,   132
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
#line 143 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 160 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 160 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 161 "Parser.y"
    {
				BOOL bString = ((yyvsp[(4) - (5)]).v.type == S_STRING);
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (5)]).ident.s,
					constantTypeFor((yyvsp[(4) - (5)]).v.type), (yyvsp[(4) - (5)]).v.data);
				free((yyvsp[(2) - (5)]).ident.s);
				if (bString) {
					free((yyvsp[(4) - (5)]).v.data.string);
				}
			;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 172 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 175 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = S_STRING;
			;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 180 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 181 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 183 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 190 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 196 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 197 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 198 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 199 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 200 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 201 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 205 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 208 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 222 "Parser.y"
    { startmacro(); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 223 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (5)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 235 "Parser.y"
    {
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 242 "Parser.y"
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

  case 45:

/* Line 1455 of yacc.c  */
#line 262 "Parser.y"
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

  case 46:

/* Line 1455 of yacc.c  */
#line 271 "Parser.y"
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

  case 48:

/* Line 1455 of yacc.c  */
#line 284 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 285 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 286 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 287 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 288 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 289 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 290 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 291 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 292 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 293 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 294 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 295 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 296 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 301 "Parser.y"
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

  case 62:

/* Line 1455 of yacc.c  */
#line 312 "Parser.y"
    {
				_generateShorthandAssignment = 0;
				(yyval).v = (yyvsp[(1) - (1)]).v;
			;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 317 "Parser.y"
    { 
				_generateShorthandAssignment = (yyvsp[(1) - (2)]).type;
				(yyval).v = (yyvsp[(2) - (2)]).v;
			;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 323 "Parser.y"
    { (yyval).type = BIN_MUL; ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 324 "Parser.y"
    { (yyval).type = BIN_DIV; ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 325 "Parser.y"
    { (yyval).type = BIN_MOD; ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
    { (yyval).type = BIN_ADD; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 327 "Parser.y"
    { (yyval).type = BIN_SUB; ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 329 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 331 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 332 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 334 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 336 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 339 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 341 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 343 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 346 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 347 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-(yyvsp[(2) - (2)]).num);
				} else {
					(yyval).type = S_NUMBER;
					(yyval).v.type = C_PUSH_LONG_LITERAL; 
					(yyval).v.data.longValue  = -(yyvsp[(2) - (2)]).num;
				}
			;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 363 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 364 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 365 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 366 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 367 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 368 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 369 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 370 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 371 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 372 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 373 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 374 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 382 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 383 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 384 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 385 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 386 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 387 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 388 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 389 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 390 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 391 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 392 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    { (yyval).type = S_NUMBER; 	 	;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 397 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_NUMBER;
			;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 401 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_FLOAT;
			;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 405 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_RANGE;
			;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 409 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_STRING;
			;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 413 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_BOOLEAN;
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 421 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 426 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 427 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 433 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident;	;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 434 "Parser.y"
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

  case 124:

/* Line 1455 of yacc.c  */
#line 446 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 449 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 452 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 459 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 460 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 467 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 472 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 476 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 481 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 483 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 486 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 497 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 500 "Parser.y"
    {
				_iflevel--;
			;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 502 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 507 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 511 "Parser.y"
    {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 518 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 519 "Parser.y"
    { 
				if ((yyvsp[(1) - (1)]).needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 525 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (1)]).funcp;
				_currentFunctionCallParamIndex = 0;
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
				YY_EMIT(C_SET_PARAMETER_STACK,(GENERIC_DATA){0});
			;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 531 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, _currentFunctionCallParamIndex);
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 535 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndex = 0;
				_calledFunctionOrMacro = (yyvsp[(1) - (1)]).ident.s;
				(yyval).funcp = 0;
				YY_EMIT(C_SET_PARAMETER_STACK,(GENERIC_DATA){0});
			;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 541 "Parser.y"
    {
				YY_EMIT(C_MACRO,(GENERIC_DATA){.string = _calledFunctionOrMacro});
				freeitem(&_calledFunctionOrMacro);
			;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 550 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 554 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; _currentFunctionCallParamIndex++; ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 556 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 558 "Parser.y"
    {	(yyval).type = S_CHARACTER;	;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 559 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 560 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 561 "Parser.y"
    {	(yyval).type = S_FLOAT;	;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 562 "Parser.y"
    {	(yyval).type = S_BOOLEAN; ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 563 "Parser.y"
    {	(yyval).type = S_STRING;	;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 564 "Parser.y"
    {	(yyval).type = S_RANGE;	;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 566 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 569 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 574 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 579 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 584 "Parser.y"
    {
				(yyval).v.type = C_PUSH_CHARACTER_LITERAL; 
				(yyval).v.data.uchar = (char)(yyvsp[(1) - (1)]).num;
			;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 589 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.intValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_NUMBER;
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 597 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_FLOAT;
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 605 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 613 "Parser.y"
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
#line 3045 "Parser.tab.c"
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
#line 621 "Parser.y"


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


