
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
#line 47 "Parser.y"


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

static 		void init_header(void);
static 		void finalize_header(void);
static 		void init_macroDefinition(void);
static 		void finalize_macroDefinition(void);
static 		void freeitem(char **p);
static 		char *bytecode_emitGotoInstruction(char *prefix, int level, int bratyp);
static		int bytecode_generateAutoLabelNamePrefix(char *prefix,int level);
static		void bytecode_destroyAutoLabelNamePrefix(char *prefix, int level);
static void 	bytecode_emitPushParameterInstruction(long long lValue);

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
#line 190 "Parser.tab.c"

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
     T_NAMESPACE = 258,
     T_STATIC = 259,
     T_IN = 260,
     T_FOREACH = 261,
     T_TSTRING = 262,
     T_TINT = 263,
     T_TCHAR = 264,
     T_TLONG = 265,
     T_TFLOAT = 266,
     T_RANGE = 267,
     T_IDENT = 268,
     T_NUM = 269,
     T_FLOATING_POINT_NUMBER = 270,
     T_STRING = 271,
     T_CHARACTER = 272,
     T_CONST = 273,
     T_TRUE = 274,
     T_FALSE = 275,
     T_TBOOLEAN = 276,
     T_FUNC = 277,
     T_MACRO = 278,
     T_DOTDOT = 279,
     T_PLUSPLUS = 280,
     T_MINUSMINUS = 281,
     T_IF = 282,
     T_ELIF = 283,
     T_ELSE = 284,
     T_WHILE = 285,
     T_GOTO = 286,
     T_BRAEQ = 287,
     T_SH_ASSIGN_PLUS = 288,
     T_SH_ASSIGN_MINUS = 289,
     T_SH_ASSIGN_MULT = 290,
     T_SH_ASSIGN_DIV = 291,
     T_SH_ASSIGN_MOD = 292,
     T_LOCAL = 293,
     T_SWITCH = 294,
     T_BREAK = 295,
     T_CONTINUE = 296,
     T_RETURN = 297,
     T_NUMVAR = 298,
     T_RANGEVAR = 299,
     T_FLOATVAR = 300,
     T_STRINGVAR = 301,
     T_BOOLEANVAR = 302,
     T_CHARACTERVAR = 303,
     T_NMATCH = 304,
     T_ASSIGN = 305,
     T_NE = 306,
     T_EQ = 307,
     T_SHIFT_LEFT = 308,
     T_SHIFT_RIGHT = 309,
     T_OR = 310,
     T_AND = 311,
     T_LE = 312,
     T_GE = 313,
     T_POWER_TO = 314,
     T_VOID = 315,
     T_CMDSEQPREFIX = 316,
     T_GT = 317,
     T_LT = 318
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
#line 295 "Parser.tab.c"

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
#define YYLAST   681

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  77
/* YYNRULES -- Number of rules.  */
#define YYNRULES  187
/* YYNRULES -- Number of states.  */
#define YYNSTATES  276

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   318

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    83,     2,     2,     2,    74,    62,     2,
      78,    79,    72,    70,    77,    71,     2,    73,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    84,    76,
      66,     2,    67,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    75,     2,    82,    64,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    80,    63,    81,    65,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    68,    69
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,    10,    13,    14,    18,    19,
      22,    25,    26,    29,    32,    34,    35,    38,    42,    44,
      48,    49,    50,    56,    58,    60,    62,    64,    66,    68,
      70,    72,    74,    76,    78,    80,    83,    86,    88,    90,
      92,    94,    96,    98,   100,   101,   107,   108,   110,   114,
     116,   119,   122,   127,   128,   131,   136,   140,   142,   145,
     148,   150,   152,   154,   156,   158,   160,   162,   164,   166,
     168,   170,   173,   175,   178,   180,   182,   184,   186,   188,
     191,   193,   197,   199,   204,   208,   211,   214,   216,   218,
     220,   223,   226,   229,   232,   235,   239,   243,   247,   251,
     255,   259,   263,   267,   271,   275,   279,   283,   287,   288,
     290,   293,   297,   301,   305,   309,   313,   317,   321,   325,
     329,   333,   335,   337,   339,   341,   343,   345,   347,   349,
     351,   353,   355,   357,   360,   364,   368,   371,   372,   374,
     375,   377,   383,   384,   388,   391,   394,   395,   400,   401,
     402,   408,   410,   414,   415,   416,   423,   424,   425,   429,
     430,   431,   435,   436,   442,   443,   449,   451,   453,   457,
     459,   461,   465,   467,   469,   471,   473,   475,   477,   479,
     481,   483,   485,   487,   489,   491,   493,   495
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      86,     0,    -1,    -1,    -1,    87,    89,    88,    94,    -1,
      90,    91,    -1,    -1,     3,    13,    76,    -1,    -1,    91,
      95,    -1,    91,    92,    -1,    -1,    93,    92,    -1,     4,
     113,    -1,   113,    -1,    -1,    94,   110,    -1,    18,    96,
      76,    -1,    97,    -1,    96,    77,    97,    -1,    -1,    -1,
      98,   101,    50,   100,    99,    -1,   161,    -1,    16,    -1,
      13,    -1,   102,    -1,    14,    -1,    16,    -1,    46,    -1,
      43,    -1,    44,    -1,    48,    -1,    47,    -1,    45,    -1,
     104,    13,    -1,   104,    22,    -1,    60,    -1,     7,    -1,
      21,    -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,
      -1,   106,   103,    78,   107,    79,    -1,    -1,   108,    -1,
     108,    77,   109,    -1,   109,    -1,   156,   101,    -1,   105,
     111,    -1,    80,   112,   148,    81,    -1,    -1,   113,   112,
      -1,   156,   101,   118,    76,    -1,   156,   101,    76,    -1,
      76,    -1,   150,    76,    -1,   115,    76,    -1,   143,    -1,
     139,    -1,   134,    -1,   129,    -1,   130,    -1,   131,    -1,
     137,    -1,   111,    -1,   121,    -1,   136,    -1,     1,    -1,
     102,   116,    -1,   118,    -1,   117,   119,    -1,    35,    -1,
      36,    -1,    37,    -1,    33,    -1,    34,    -1,    50,   119,
      -1,   120,    -1,    78,   120,    79,    -1,   123,    -1,   123,
      75,   123,    82,    -1,   123,    24,   123,    -1,   102,    26,
      -1,   102,    25,    -1,   127,    -1,   150,    -1,   122,    -1,
     155,   122,    -1,    71,    14,    -1,    65,   123,    -1,    70,
     123,    -1,    71,   123,    -1,   123,    62,   123,    -1,   123,
      63,   123,    -1,   123,    70,   123,    -1,   123,    64,   123,
      -1,   123,    71,   123,    -1,   123,    72,   123,    -1,   123,
      73,   123,    -1,   123,    74,   123,    -1,   123,    53,   123,
      -1,   123,    54,   123,    -1,   123,    59,   123,    -1,    78,
     123,    79,    -1,    78,   125,    79,    -1,    -1,   126,    -1,
      83,   125,    -1,   125,    56,   125,    -1,   125,    55,   125,
      -1,   125,    66,   125,    -1,   125,    67,   125,    -1,   125,
      57,   125,    -1,   125,    58,   125,    -1,   125,    52,   125,
      -1,   125,    51,   125,    -1,   123,    65,   123,    -1,   123,
      49,   123,    -1,   150,    -1,   127,    -1,    43,    -1,    45,
      -1,    44,    -1,    46,    -1,    48,    -1,    47,    -1,   128,
      -1,   161,    -1,    13,    -1,    16,    -1,   128,    16,    -1,
      40,   133,    76,    -1,    41,   133,    76,    -1,    42,   132,
      -1,    -1,   123,    -1,    -1,    14,    -1,    39,   124,    80,
     135,    81,    -1,    -1,   136,   111,   135,    -1,    13,    84,
      -1,    31,    13,    -1,    -1,    32,    13,   138,    76,    -1,
      -1,    -1,    30,   140,   124,   141,   142,    -1,   114,    -1,
      80,   148,    81,    -1,    -1,    -1,    27,   124,   144,   142,
     145,   146,    -1,    -1,    -1,    29,   147,   142,    -1,    -1,
      -1,   114,   149,   148,    -1,    -1,    22,   151,    78,   107,
      79,    -1,    -1,    13,   152,    78,   107,    79,    -1,   153,
      -1,   154,    -1,   153,    77,   154,    -1,     1,    -1,   123,
      -1,    78,   156,    79,    -1,     9,    -1,     8,    -1,    10,
      -1,    11,    -1,    21,    -1,     7,    -1,    12,    -1,    19,
      -1,    20,    -1,    15,    -1,    14,    -1,    17,    -1,   159,
      -1,   158,    -1,   160,    -1,   157,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   146,   146,   152,   146,   158,   160,   161,   163,   164,
     165,   167,   168,   170,   171,   173,   174,   178,   181,   182,
     184,   184,   184,   196,   199,   204,   205,   206,   213,   220,
     221,   222,   223,   224,   225,   229,   232,   238,   239,   240,
     241,   242,   243,   244,   246,   246,   251,   252,   255,   257,
     259,   266,   287,   289,   290,   292,   301,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   331,   342,   347,   353,   354,   355,   356,   357,   359,
     361,   362,   364,   365,   366,   369,   371,   373,   374,   376,
     377,   381,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   408,   410,   411,
     412,   413,   414,   415,   416,   417,   418,   419,   420,   421,
     422,   424,   425,   427,   431,   435,   439,   443,   447,   451,
     456,   457,   463,   464,   475,   478,   482,   486,   487,   489,
     490,   492,   494,   495,   497,   502,   506,   506,   511,   513,
     511,   524,   524,   527,   530,   527,   537,   541,   541,   548,
     549,   549,   555,   555,   565,   565,   576,   578,   579,   580,
     584,   586,   588,   589,   590,   591,   592,   593,   594,   596,
     599,   604,   609,   614,   619,   627,   635,   643
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_NAMESPACE", "T_STATIC", "T_IN",
  "T_FOREACH", "T_TSTRING", "T_TINT", "T_TCHAR", "T_TLONG", "T_TFLOAT",
  "T_RANGE", "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER", "T_STRING",
  "T_CHARACTER", "T_CONST", "T_TRUE", "T_FALSE", "T_TBOOLEAN", "T_FUNC",
  "T_MACRO", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS", "T_IF", "T_ELIF",
  "T_ELSE", "T_WHILE", "T_GOTO", "T_BRAEQ", "T_SH_ASSIGN_PLUS",
  "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV",
  "T_SH_ASSIGN_MOD", "T_LOCAL", "T_SWITCH", "T_BREAK", "T_CONTINUE",
  "T_RETURN", "T_NUMVAR", "T_RANGEVAR", "T_FLOATVAR", "T_STRINGVAR",
  "T_BOOLEANVAR", "T_CHARACTERVAR", "T_NMATCH", "T_ASSIGN", "T_NE", "T_EQ",
  "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR", "T_AND", "T_LE", "T_GE",
  "T_POWER_TO", "T_VOID", "T_CMDSEQPREFIX", "'&'", "'|'", "'^'", "'~'",
  "'<'", "'>'", "T_GT", "T_LT", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "';'", "','", "'('", "')'", "'{'", "'}'", "']'", "'!'", "':'", "$accept",
  "file_structure", "$@1", "$@2", "header_declarations", "namespace",
  "global_declarations", "global_vars", "global_var", "macro_definitions",
  "constants", "constant_list", "constdef", "$@3", "$@4",
  "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "$@5", "parameter_list",
  "non_empty_pardecl", "par_decl", "macro_definition", "block",
  "var_decls", "var_decl", "statement", "assignment",
  "assignment_or_shorthand_assignment", "shorthand_assignment_operator",
  "assignment_expression", "arithmetic_expression", "simple_expression",
  "increment_expression", "s_bterm", "binary_expression", "condition",
  "logical_expression", "term", "value", "string", "break", "continue",
  "return_expression", "optional_bterm", "opt_num", "case", "caselabels",
  "label", "goto", "$@6", "while", "$@7", "$@8", "local_block",
  "if_expression", "$@9", "$@10", "elseclause", "$@11", "stmntlist",
  "$@12", "call_expression", "@13", "@14", "nonempty_parameters",
  "parameter", "type_cast", "type_ident", "boolean_literal",
  "float_literal", "integer_literal", "character_literal",
  "simple_literal", 0
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
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,    38,   124,    94,   126,    60,    62,   317,   318,
      43,    45,    42,    47,    37,    91,    59,    44,    40,    41,
     123,   125,    93,    33,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    85,    87,    88,    86,    89,    90,    90,    91,    91,
      91,    92,    92,    93,    93,    94,    94,    95,    96,    96,
      98,    99,    97,   100,   100,   101,   101,   101,   101,   102,
     102,   102,   102,   102,   102,   103,   103,   104,   104,   104,
     104,   104,   104,   104,   106,   105,   107,   107,   108,   108,
     109,   110,   111,   112,   112,   113,   113,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   115,   116,   116,   117,   117,   117,   117,   117,   118,
     119,   119,   120,   120,   120,   121,   121,   122,   122,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   124,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   126,   126,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   128,   128,   129,   130,   131,   132,   132,   133,
     133,   134,   135,   135,   136,   137,   138,   137,   140,   141,
     139,   142,   142,   144,   145,   143,   146,   147,   146,   148,
     149,   148,   151,   150,   152,   150,   107,   153,   153,   153,
     154,   155,   156,   156,   156,   156,   156,   156,   156,   157,
     157,   158,   159,   160,   161,   161,   161,   161
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     4,     2,     0,     3,     0,     2,
       2,     0,     2,     2,     1,     0,     2,     3,     1,     3,
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
       2,     0,     6,     1,     0,     3,     8,     0,    15,     5,
       7,    44,     0,   177,   173,   172,   174,   175,   178,    20,
     176,    10,    11,     9,    14,     0,     0,     0,    16,    13,
       0,    18,     0,    12,    25,    27,    28,    30,    31,    34,
      29,    33,    32,     0,    26,    53,    51,    38,    40,    41,
      42,    43,    39,    37,     0,     0,    17,    20,     0,     0,
      56,     0,     0,    53,     0,    35,    36,    19,     0,   131,
     182,   181,   132,   183,   179,   180,   162,   123,   125,   124,
     126,   128,   127,     0,     0,     0,     0,    79,    80,    89,
      82,    87,   129,    88,     0,   187,   185,   184,   186,   130,
      55,    70,   164,     0,   148,     0,     0,     0,   139,   139,
     137,    57,     0,    67,   160,     0,    68,    63,    64,    65,
      62,    69,    66,    61,    60,     0,     0,    54,   169,     0,
       0,    47,    49,   170,   166,   167,     0,    24,    21,    23,
       0,     0,    92,    93,    91,    94,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   133,    90,   144,   108,   153,     0,   145,   146,
       0,   140,     0,     0,   138,   136,    86,    85,    77,    78,
      74,    75,    76,    71,     0,    72,     0,    59,    52,    58,
       0,    45,     0,     0,    50,    22,     0,     0,    81,   106,
     171,    84,   103,   104,   105,    95,    96,    98,    97,    99,
     100,   101,   102,     0,   108,     0,     0,   109,    87,    88,
       0,   149,     0,   142,   134,   135,    73,   161,    48,   168,
       0,     0,    83,   110,     0,     0,   108,   108,   108,   108,
     108,   108,   108,   108,   107,     0,   151,   154,     0,   147,
       0,     0,     0,   165,   163,   120,   119,   118,   117,   112,
     111,   115,   116,   113,   114,     0,   156,   150,   141,   142,
     152,   157,   155,   143,     0,   158
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,     5,     6,     9,    21,    22,    11,
      23,    30,    31,    32,   195,   138,    43,   112,    54,    55,
      26,    27,   130,   131,   132,    28,   113,    62,    63,   114,
     115,   183,   184,    61,    87,    88,   116,    89,   215,   166,
     216,   217,    91,    92,   117,   118,   119,   175,   172,   120,
     251,   121,   122,   222,   123,   167,   248,   247,   124,   220,
     266,   272,   274,   125,   186,    93,   141,   140,   134,   135,
      94,    25,    95,    96,    97,    98,    99
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -228
static const yytype_int16 yypact[] =
{
    -228,     8,    17,  -228,    10,  -228,  -228,   -38,  -228,   646,
    -228,    55,   395,  -228,  -228,  -228,  -228,  -228,  -228,  -228,
    -228,  -228,   153,  -228,  -228,   278,   -24,    25,  -228,  -228,
     -70,  -228,   278,  -228,  -228,  -228,  -228,  -228,  -228,  -228,
    -228,  -228,  -228,   -39,  -228,   395,  -228,  -228,  -228,  -228,
    -228,  -228,  -228,  -228,    -6,    -9,  -228,  -228,    23,   377,
    -228,     6,   188,   395,   288,  -228,  -228,  -228,   112,    11,
    -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,  -228,
    -228,  -228,  -228,   413,   413,   449,   330,  -228,  -228,  -228,
     184,  -228,    68,  -228,   124,  -228,  -228,  -228,  -228,  -228,
    -228,  -228,     7,    12,  -228,    79,    80,    12,    81,    81,
     413,  -228,   583,  -228,  -228,    24,  -228,  -228,  -228,  -228,
    -228,  -228,  -228,  -228,  -228,    40,    30,  -228,  -228,   330,
      26,    47,  -228,   568,    56,  -228,   278,  -228,  -228,  -228,
      58,    67,   575,    29,  -228,    29,    69,   132,    70,   413,
     413,   413,   413,   413,   413,   413,   413,   413,   413,   413,
     413,   413,  -228,  -228,  -228,   366,  -228,    12,  -228,  -228,
      72,  -228,    71,    82,   568,  -228,  -228,  -228,  -228,  -228,
    -228,  -228,  -228,  -228,   377,  -228,   188,  -228,  -228,  -228,
     514,  -228,   395,   413,  -228,  -228,   288,   288,  -228,  -228,
    -228,   568,   204,   204,  -228,   575,   575,   575,    29,    29,
    -228,  -228,  -228,   459,   366,   541,   305,  -228,   487,   504,
     240,  -228,    90,   137,  -228,  -228,  -228,  -228,  -228,  -228,
      94,   104,  -228,   614,   413,   413,   366,   366,   366,   366,
     366,   366,   366,   366,  -228,    18,  -228,  -228,   240,  -228,
       7,   107,   -24,  -228,  -228,   568,   568,   261,   261,   422,
     422,   614,   614,   -43,   -43,   109,   168,  -228,  -228,   137,
    -228,  -228,  -228,  -228,   240,  -228
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -228,  -228,  -228,  -228,  -228,  -228,  -228,   176,  -228,  -228,
    -228,  -228,   142,  -228,  -228,  -228,   -14,   -16,  -228,  -228,
    -228,  -228,  -143,  -228,    20,  -228,   -26,   146,    74,  -151,
    -228,  -228,  -228,    88,    32,   128,  -228,   123,   -42,   -97,
     262,  -228,  -162,  -228,  -228,  -228,  -228,  -228,   113,  -228,
     -48,  -218,  -228,  -228,  -228,  -228,  -228,  -227,  -228,  -228,
    -228,  -228,  -228,  -174,  -228,   -61,  -228,  -228,  -228,    31,
    -228,   -62,  -228,  -228,  -228,  -228,   155
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -165
static const yytype_int16 yytable[] =
{
      46,   126,   136,   218,    65,   252,    56,    57,     3,    44,
     170,    59,   227,    66,   240,   241,    44,    90,    58,   101,
       4,   267,   133,     7,   148,    13,    14,    15,    16,    17,
      18,   102,    47,    48,    49,    50,    51,    60,    10,    20,
      76,   142,   143,   145,   147,   103,    52,   275,   104,   105,
     106,   252,   218,   230,   231,    -4,    45,   107,   108,   109,
     110,    37,    38,    39,    40,    41,    42,   148,   174,   246,
     221,   265,    64,    68,   218,   218,   218,   218,   218,   218,
     218,   218,   100,    24,   162,    53,    29,   190,   152,  -164,
     165,   164,   168,   169,   111,   171,    24,   246,    45,   -53,
     187,   158,   159,   160,   219,   191,   189,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
      44,   188,   194,   246,   192,   126,    70,    71,   137,    73,
     136,    74,    75,   193,   136,   136,   196,    69,    70,    71,
      72,    73,    90,    74,    75,   197,    76,   224,   198,   200,
     250,   133,   223,   219,   133,   133,   149,    12,   225,   126,
      13,    14,    15,    16,    17,    18,   249,    77,    78,    79,
      80,    81,    82,   253,    20,   219,   219,   219,   219,   219,
     219,   219,   219,   254,   126,   150,   151,   126,   268,   101,
     270,   152,   255,   256,   153,   154,   155,   271,    33,    67,
     185,   102,   156,   157,   158,   159,   160,   161,   149,   127,
      76,   199,   228,   126,   146,   103,   226,   163,   104,   105,
     106,   273,   173,   139,   229,     0,   269,   107,   108,   109,
     110,    37,    38,    39,    40,    41,    42,   150,   151,     0,
       0,   101,     0,   152,     0,     0,   153,   154,   155,     0,
       0,     0,     0,   102,   156,   157,   158,   159,   160,   161,
       0,     0,    76,   152,   111,     0,     0,   103,    45,  -159,
     104,   105,   106,     0,   156,   157,   158,   159,   160,   107,
     108,   109,   110,    37,    38,    39,    40,    41,    42,   128,
       0,    34,    35,     0,    36,    13,    14,    15,    16,    17,
      18,    69,    70,    71,    72,    73,     0,    74,    75,    20,
      76,     0,  -165,  -165,     0,     0,   111,     0,   240,   241,
     245,    37,    38,    39,    40,    41,    42,   242,   243,     0,
       0,    77,    78,    79,    80,    81,    82,    13,    14,    15,
      16,    17,    18,    69,    70,    71,    72,    73,     0,    74,
      75,    20,    76,    83,     0,     0,   236,   237,    84,    85,
     238,   239,   240,   241,     0,     0,   129,   -46,     0,     0,
       0,   242,   243,    77,    78,    79,    80,    81,    82,    69,
      70,    71,    72,    73,   244,    74,    75,     0,    76,     0,
      69,    70,    71,    72,    73,    83,    74,    75,     0,    76,
      84,    85,    13,    14,    15,    16,    17,    18,   129,    77,
      78,    79,    80,    81,    82,     0,    20,     0,     0,     0,
      77,    78,    79,    80,    81,    82,    69,    70,    71,    72,
      73,    83,    74,    75,     0,    76,    84,    85,     0,     0,
       0,     0,    83,     0,   129,     0,     0,    84,    85,   214,
       0,     0,     0,     0,     0,    86,    77,    78,    79,    80,
      81,    82,    69,   144,    71,    72,    73,     0,    74,    75,
       0,    76,     0,   236,   237,     0,   233,     0,    83,   240,
     241,     0,     0,    84,    85,     0,     0,     0,   242,   243,
       0,   129,    77,    78,    79,    80,    81,    82,   257,   258,
     259,   260,   261,   262,   263,   264,     0,     0,     0,     0,
       0,     0,   150,   151,    83,     0,     0,     0,   152,    84,
      85,   153,   154,   155,     0,     0,     0,   129,     0,   156,
     157,   158,   159,   160,     0,     0,     0,     0,  -122,  -122,
       0,   232,  -122,  -122,  -122,  -122,     0,     0,     0,     0,
       0,     0,     0,  -122,  -122,  -121,  -121,     0,     0,  -121,
    -121,  -121,  -121,     0,     0,     0,  -122,   150,   151,     0,
    -121,  -121,     0,   152,     0,     0,   153,   154,   155,     0,
       0,     0,     0,  -121,   156,   157,   158,   159,   160,     0,
     234,     0,     0,   199,   150,   151,     0,     0,     0,     0,
     152,     0,     0,   153,   154,   155,   235,     0,   176,   177,
       0,   156,   157,   158,   159,   160,   178,   179,   180,   181,
     182,   150,   151,     0,     0,     0,     0,   152,   150,   151,
     153,   154,   155,    59,   152,     0,     0,     0,   156,   157,
     158,   159,   160,     0,     0,   156,   157,   158,   159,   160,
      12,     0,     0,    13,    14,    15,    16,    17,    18,     0,
       0,     0,     0,     0,    19,   236,   237,    20,     0,   238,
     239,   240,   241,     0,     0,     0,     0,     0,     0,     0,
     242,   243
};

static const yytype_int16 yycheck[] =
{
      26,    62,    64,   165,    13,   223,    76,    77,     0,    25,
     107,    50,   186,    22,    57,    58,    32,    59,    32,     1,
       3,   248,    64,    13,    86,     7,     8,     9,    10,    11,
      12,    13,     7,     8,     9,    10,    11,    76,    76,    21,
      22,    83,    84,    85,    86,    27,    21,   274,    30,    31,
      32,   269,   214,   196,   197,     0,    80,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,   129,   110,   220,
     167,   245,    78,    50,   236,   237,   238,   239,   240,   241,
     242,   243,    76,     9,    16,    60,    12,   129,    59,    78,
      78,    84,    13,    13,    76,    14,    22,   248,    80,    81,
      76,    72,    73,    74,   165,    79,    76,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     136,    81,   136,   274,    77,   186,    14,    15,    16,    17,
     192,    19,    20,    77,   196,   197,    78,    13,    14,    15,
      16,    17,   184,    19,    20,    78,    22,    76,    79,    79,
      13,   193,    80,   214,   196,   197,    24,     4,    76,   220,
       7,     8,     9,    10,    11,    12,    76,    43,    44,    45,
      46,    47,    48,    79,    21,   236,   237,   238,   239,   240,
     241,   242,   243,    79,   245,    53,    54,   248,    81,     1,
      81,    59,   234,   235,    62,    63,    64,    29,    22,    57,
     112,    13,    70,    71,    72,    73,    74,    75,    24,    63,
      22,    79,   192,   274,    86,    27,   184,    94,    30,    31,
      32,   269,   109,    68,   193,    -1,   252,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    53,    54,    -1,
      -1,     1,    -1,    59,    -1,    -1,    62,    63,    64,    -1,
      -1,    -1,    -1,    13,    70,    71,    72,    73,    74,    75,
      -1,    -1,    22,    59,    76,    -1,    -1,    27,    80,    81,
      30,    31,    32,    -1,    70,    71,    72,    73,    74,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,     1,
      -1,    13,    14,    -1,    16,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    -1,    19,    20,    21,
      22,    -1,    51,    52,    -1,    -1,    76,    -1,    57,    58,
      80,    43,    44,    45,    46,    47,    48,    66,    67,    -1,
      -1,    43,    44,    45,    46,    47,    48,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    -1,    19,
      20,    21,    22,    65,    -1,    -1,    51,    52,    70,    71,
      55,    56,    57,    58,    -1,    -1,    78,    79,    -1,    -1,
      -1,    66,    67,    43,    44,    45,    46,    47,    48,    13,
      14,    15,    16,    17,    79,    19,    20,    -1,    22,    -1,
      13,    14,    15,    16,    17,    65,    19,    20,    -1,    22,
      70,    71,     7,     8,     9,    10,    11,    12,    78,    43,
      44,    45,    46,    47,    48,    -1,    21,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    48,    13,    14,    15,    16,
      17,    65,    19,    20,    -1,    22,    70,    71,    -1,    -1,
      -1,    -1,    65,    -1,    78,    -1,    -1,    70,    71,    83,
      -1,    -1,    -1,    -1,    -1,    78,    43,    44,    45,    46,
      47,    48,    13,    14,    15,    16,    17,    -1,    19,    20,
      -1,    22,    -1,    51,    52,    -1,   214,    -1,    65,    57,
      58,    -1,    -1,    70,    71,    -1,    -1,    -1,    66,    67,
      -1,    78,    43,    44,    45,    46,    47,    48,   236,   237,
     238,   239,   240,   241,   242,   243,    -1,    -1,    -1,    -1,
      -1,    -1,    53,    54,    65,    -1,    -1,    -1,    59,    70,
      71,    62,    63,    64,    -1,    -1,    -1,    78,    -1,    70,
      71,    72,    73,    74,    -1,    -1,    -1,    -1,    51,    52,
      -1,    82,    55,    56,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    67,    51,    52,    -1,    -1,    55,
      56,    57,    58,    -1,    -1,    -1,    79,    53,    54,    -1,
      66,    67,    -1,    59,    -1,    -1,    62,    63,    64,    -1,
      -1,    -1,    -1,    79,    70,    71,    72,    73,    74,    -1,
      49,    -1,    -1,    79,    53,    54,    -1,    -1,    -1,    -1,
      59,    -1,    -1,    62,    63,    64,    65,    -1,    25,    26,
      -1,    70,    71,    72,    73,    74,    33,    34,    35,    36,
      37,    53,    54,    -1,    -1,    -1,    -1,    59,    53,    54,
      62,    63,    64,    50,    59,    -1,    -1,    -1,    70,    71,
      72,    73,    74,    -1,    -1,    70,    71,    72,    73,    74,
       4,    -1,    -1,     7,     8,     9,    10,    11,    12,    -1,
      -1,    -1,    -1,    -1,    18,    51,    52,    21,    -1,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    67
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    86,    87,     0,     3,    89,    90,    13,    88,    91,
      76,    94,     4,     7,     8,     9,    10,    11,    12,    18,
      21,    92,    93,    95,   113,   156,   105,   106,   110,   113,
      96,    97,    98,    92,    13,    14,    16,    43,    44,    45,
      46,    47,    48,   101,   102,    80,   111,     7,     8,     9,
      10,    11,    21,    60,   103,   104,    76,    77,   101,    50,
      76,   118,   112,   113,    78,    13,    22,    97,    50,    13,
      14,    15,    16,    17,    19,    20,    22,    43,    44,    45,
      46,    47,    48,    65,    70,    71,    78,   119,   120,   122,
     123,   127,   128,   150,   155,   157,   158,   159,   160,   161,
      76,     1,    13,    27,    30,    31,    32,    39,    40,    41,
      42,    76,   102,   111,   114,   115,   121,   129,   130,   131,
     134,   136,   137,   139,   143,   148,   150,   112,     1,    78,
     107,   108,   109,   123,   153,   154,   156,    16,   100,   161,
     152,   151,   123,   123,    14,   123,   120,   123,   156,    24,
      53,    54,    59,    62,    63,    64,    70,    71,    72,    73,
      74,    75,    16,   122,    84,    78,   124,   140,    13,    13,
     124,    14,   133,   133,   123,   132,    25,    26,    33,    34,
      35,    36,    37,   116,   117,   118,   149,    76,    81,    76,
     123,    79,    77,    77,   101,    99,    78,    78,    79,    79,
      79,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,    83,   123,   125,   126,   127,   150,
     144,   124,   138,    80,    76,    76,   119,   148,   109,   154,
     107,   107,    82,   125,    49,    65,    51,    52,    55,    56,
      57,    58,    66,    67,    79,    80,   114,   142,   141,    76,
      13,   135,   136,    79,    79,   123,   123,   125,   125,   125,
     125,   125,   125,   125,   125,   148,   145,   142,    81,   111,
      81,    29,   146,   135,   147,   142
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
        case 2:

/* Line 1455 of yacc.c  */
#line 146 "Parser.y"
    {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 152 "Parser.y"
    {
				finalize_header();
			;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 184 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 184 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 185 "Parser.y"
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

  case 23:

/* Line 1455 of yacc.c  */
#line 196 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 199 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = S_STRING;
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 204 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 205 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 207 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 214 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 220 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 221 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 222 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 223 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 224 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 225 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 229 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 232 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 246 "Parser.y"
    { init_macroDefinition(); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 247 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (5)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 259 "Parser.y"
    {
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 266 "Parser.y"
    {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (2)]).ident.s, -1, 1);
					YY_EMIT(C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",(yyvsp[(1) - (2)]).ident.s);
					MACRO_PARAM param = {
						.mp_name = (yyvsp[(1) - (2)]).ident.s,
						.mp_comment = _yyCurrentComment,
						.mp_bytecodeLength = _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start,
						.mp_buffer = _bytecodeBuffer.bb_start
					};
					_compilerConfiguration->cb_insertNewMacro(&param);
				}
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 292 "Parser.y"
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

  case 56:

/* Line 1455 of yacc.c  */
#line 301 "Parser.y"
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

  case 58:

/* Line 1455 of yacc.c  */
#line 314 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 315 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 316 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 317 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 318 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 319 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 320 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 321 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 322 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 323 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 324 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 325 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 331 "Parser.y"
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

  case 72:

/* Line 1455 of yacc.c  */
#line 342 "Parser.y"
    {
				_generateShorthandAssignment = 0;
				(yyval).v = (yyvsp[(1) - (1)]).v;
			;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 347 "Parser.y"
    { 
				_generateShorthandAssignment = (yyvsp[(1) - (2)]).type;
				(yyval).v = (yyvsp[(2) - (2)]).v;
			;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 353 "Parser.y"
    { (yyval).type = BIN_MUL; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 354 "Parser.y"
    { (yyval).type = BIN_DIV; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 355 "Parser.y"
    { (yyval).type = BIN_MOD; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 356 "Parser.y"
    { (yyval).type = BIN_ADD; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 357 "Parser.y"
    { (yyval).type = BIN_SUB; ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 359 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 364 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 365 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 366 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 369 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 371 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 373 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 376 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 377 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 381 "Parser.y"
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

  case 92:

/* Line 1455 of yacc.c  */
#line 390 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 391 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 392 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 393 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 396 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 397 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 398 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 400 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 401 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 402 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 403 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 404 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 412 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 413 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 414 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 415 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 416 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 418 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 419 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 420 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 421 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 422 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 424 "Parser.y"
    { (yyval).type = S_NUMBER; 	 	;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 427 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_NUMBER;
			;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 431 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_FLOAT;
			;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 435 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_RANGE;
			;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 439 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_STRING;
			;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 443 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 447 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_BOOLEAN;
			;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 451 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 456 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 457 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 463 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident;	;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
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

  case 134:

/* Line 1455 of yacc.c  */
#line 476 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 482 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 489 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 490 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 497 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 502 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 506 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 511 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 513 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 516 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 527 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 530 "Parser.y"
    {
				_iflevel--;
			;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 532 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 537 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 541 "Parser.y"
    {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 548 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 549 "Parser.y"
    { 
				if ((yyvsp[(1) - (1)]).needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 555 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (1)]).funcp;
				_currentFunctionCallParamIndex = 0;
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
				YY_EMIT(C_SET_PARAMETER_STACK,(GENERIC_DATA){0});
			;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 561 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, _currentFunctionCallParamIndex);
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 565 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndex = 0;
				_calledFunctionOrMacro = (yyvsp[(1) - (1)]).ident.s;
				(yyval).funcp = 0;
				YY_EMIT(C_SET_PARAMETER_STACK,(GENERIC_DATA){0});
			;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 571 "Parser.y"
    {
				YY_EMIT(C_MACRO,(GENERIC_DATA){.string = _calledFunctionOrMacro});
				freeitem(&_calledFunctionOrMacro);
			;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 580 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 584 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; _currentFunctionCallParamIndex++; ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 586 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 588 "Parser.y"
    {	(yyval).type = S_CHARACTER;	;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 589 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 590 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 591 "Parser.y"
    {	(yyval).type = S_FLOAT;	;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 592 "Parser.y"
    {	(yyval).type = S_BOOLEAN; ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 593 "Parser.y"
    {	(yyval).type = S_STRING;	;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 594 "Parser.y"
    {	(yyval).type = S_RANGE;	;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 596 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 599 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 604 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 609 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 614 "Parser.y"
    {
				(yyval).v.type = C_PUSH_CHARACTER_LITERAL; 
				(yyval).v.data.uchar = (char)(yyvsp[(1) - (1)]).num;
			;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 619 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.longValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_NUMBER;
			;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 627 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_FLOAT;
			;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 635 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 643 "Parser.y"
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
#line 3079 "Parser.tab.c"
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
#line 651 "Parser.y"


/**
 * bytecode_emitPushParameterInstruction()
 * Special processing when pushing integer literals: if the integer literal is pushed as 
 * parameter of a native function and this is form-related native function push a special
 * byte code as a marker.
 */
static void bytecode_emitPushParameterInstruction(long long lValue)
{
	int opCode = C_PUSH_INTEGER_LITERAL;
	if (_currentNativeMethodCalled && function_parameterIsFormStart(_currentNativeMethodCalled,_currentFunctionCallParamIndex+1)) {
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
	init_bytecodeBuffer();
	_nparam = 0;
	_currentNativeMethodCalled  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	yyerrflg = 0;
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


