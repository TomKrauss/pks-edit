
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

static MACRO_SCOPE _macroScope;
static int	_currentNamespaceIndex;
static int	_generateShorthandAssignment;
static int  _foreachCursorIndex;
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
extern int macro_lookupNamespace(const char* pszNamespacename);
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

static void push_newForeachCursor() {
	char buf[32];
	sprintf(buf, "#c%d", _foreachCursorIndex++);
	YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=buf});
}



/* Line 189 of yacc.c  */
#line 200 "Parser.tab.c"

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
     T_FOR = 260,
     T_TAUTO = 261,
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
#line 305 "Parser.tab.c"

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
#define YYLAST   698

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  86
/* YYNRULES -- Number of rules.  */
#define YYNRULES  200
/* YYNRULES -- Number of states.  */
#define YYNSTATES  296

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
      22,    25,    26,    29,    32,    33,    35,    36,    39,    43,
      45,    49,    50,    51,    57,    59,    61,    63,    65,    67,
      69,    71,    73,    75,    77,    79,    81,    84,    87,    89,
      91,    93,    95,    97,    99,   101,   102,   108,   109,   111,
     115,   117,   120,   124,   129,   130,   133,   138,   142,   144,
     147,   150,   152,   154,   156,   158,   160,   162,   164,   166,
     168,   170,   172,   174,   177,   179,   182,   184,   186,   188,
     190,   192,   195,   197,   201,   203,   208,   210,   211,   212,
     218,   219,   222,   225,   228,   230,   232,   234,   237,   240,
     243,   246,   249,   253,   257,   261,   265,   269,   273,   277,
     281,   285,   289,   293,   297,   301,   302,   304,   307,   311,
     315,   319,   323,   327,   331,   335,   339,   343,   347,   349,
     351,   353,   355,   357,   359,   361,   363,   365,   367,   369,
     371,   374,   378,   382,   385,   386,   388,   389,   391,   397,
     398,   402,   405,   408,   409,   414,   415,   423,   424,   425,
     431,   432,   433,   439,   441,   445,   446,   447,   454,   455,
     456,   460,   461,   462,   466,   467,   473,   474,   480,   482,
     484,   488,   490,   492,   496,   498,   500,   502,   504,   506,
     508,   510,   512,   514,   516,   518,   520,   522,   524,   526,
     528
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      86,     0,    -1,    -1,    -1,    87,    89,    88,    95,    -1,
      90,    91,    -1,    -1,     3,    13,    76,    -1,    -1,    91,
      96,    -1,    91,    92,    -1,    -1,    93,    92,    -1,    94,
     114,    -1,    -1,     4,    -1,    -1,    95,   111,    -1,    18,
      97,    76,    -1,    98,    -1,    97,    77,    98,    -1,    -1,
      -1,    99,   102,    50,   101,   100,    -1,   170,    -1,    16,
      -1,    13,    -1,   103,    -1,    14,    -1,    16,    -1,    46,
      -1,    43,    -1,    44,    -1,    48,    -1,    47,    -1,    45,
      -1,   105,    13,    -1,   105,    22,    -1,    60,    -1,     7,
      -1,    21,    -1,     8,    -1,     9,    -1,    10,    -1,    11,
      -1,    -1,   107,   104,    78,   108,    79,    -1,    -1,   109,
      -1,   109,    77,   110,    -1,   110,    -1,   165,   102,    -1,
      94,   106,   112,    -1,    80,   113,   157,    81,    -1,    -1,
     114,   113,    -1,   165,   102,   119,    76,    -1,   165,   102,
      76,    -1,    76,    -1,   159,    76,    -1,   116,    76,    -1,
     152,    -1,   148,    -1,   145,    -1,   138,    -1,   133,    -1,
     134,    -1,   135,    -1,   141,    -1,   112,    -1,   125,    -1,
     140,    -1,     1,    -1,   103,   117,    -1,   119,    -1,   118,
     120,    -1,    35,    -1,    36,    -1,    37,    -1,    33,    -1,
      34,    -1,    50,   120,    -1,   121,    -1,    78,   121,    79,
      -1,   127,    -1,   127,    75,   127,    82,    -1,   122,    -1,
      -1,    -1,   127,    24,   127,   123,   124,    -1,    -1,    24,
     127,    -1,   103,    26,    -1,   103,    25,    -1,   131,    -1,
     159,    -1,   126,    -1,   164,   126,    -1,    71,    14,    -1,
      65,   127,    -1,    70,   127,    -1,    71,   127,    -1,   127,
      62,   127,    -1,   127,    63,   127,    -1,   127,    70,   127,
      -1,   127,    64,   127,    -1,   127,    71,   127,    -1,   127,
      72,   127,    -1,   127,    73,   127,    -1,   127,    74,   127,
      -1,   127,    53,   127,    -1,   127,    54,   127,    -1,   127,
      59,   127,    -1,    78,   127,    79,    -1,    78,   129,    79,
      -1,    -1,   130,    -1,    83,   129,    -1,   129,    56,   129,
      -1,   129,    55,   129,    -1,   129,    66,   129,    -1,   129,
      67,   129,    -1,   129,    57,   129,    -1,   129,    58,   129,
      -1,   129,    52,   129,    -1,   129,    51,   129,    -1,   127,
      65,   127,    -1,   127,    49,   127,    -1,   159,    -1,   131,
      -1,    43,    -1,    45,    -1,    44,    -1,    46,    -1,    48,
      -1,    47,    -1,   132,    -1,   170,    -1,    13,    -1,    16,
      -1,   132,    16,    -1,    40,   137,    76,    -1,    41,   137,
      76,    -1,    42,   136,    -1,    -1,   127,    -1,    -1,    14,
      -1,    39,   128,    80,   139,    81,    -1,    -1,   140,   112,
     139,    -1,    13,    84,    -1,    31,    13,    -1,    -1,    32,
      13,   142,    76,    -1,    -1,    78,   165,   102,   144,    84,
     121,    79,    -1,    -1,    -1,     5,   146,   143,   147,   151,
      -1,    -1,    -1,    30,   149,   128,   150,   151,    -1,   115,
      -1,    80,   157,    81,    -1,    -1,    -1,    27,   128,   153,
     151,   154,   155,    -1,    -1,    -1,    29,   156,   151,    -1,
      -1,    -1,   115,   158,   157,    -1,    -1,    22,    78,   160,
     108,    79,    -1,    -1,    13,    78,   161,   108,    79,    -1,
     162,    -1,   163,    -1,   162,    77,   163,    -1,     1,    -1,
     127,    -1,    78,   165,    79,    -1,     9,    -1,     8,    -1,
      10,    -1,    11,    -1,    21,    -1,     7,    -1,     6,    -1,
      12,    -1,    19,    -1,    20,    -1,    15,    -1,    14,    -1,
      17,    -1,   168,    -1,   167,    -1,   169,    -1,   166,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   156,   156,   162,   156,   168,   170,   173,   178,   179,
     180,   182,   183,   185,   187,   188,   190,   191,   195,   198,
     199,   201,   201,   201,   213,   216,   221,   222,   223,   230,
     237,   238,   239,   240,   241,   242,   246,   249,   255,   256,
     257,   258,   259,   260,   261,   263,   263,   268,   269,   272,
     274,   276,   283,   306,   308,   309,   311,   320,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   351,   362,   367,   373,   374,   375,   376,
     377,   379,   381,   382,   384,   385,   386,   388,   389,   389,
     391,   392,   395,   397,   399,   400,   402,   403,   407,   416,
     417,   418,   419,   420,   421,   422,   423,   424,   425,   426,
     427,   428,   429,   430,   434,   436,   437,   438,   439,   440,
     441,   442,   443,   444,   445,   446,   447,   448,   450,   451,
     453,   457,   461,   465,   469,   473,   477,   482,   483,   489,
     490,   501,   504,   508,   512,   513,   515,   516,   518,   520,
     521,   523,   528,   532,   532,   537,   537,   555,   557,   555,
     568,   570,   568,   581,   581,   584,   587,   584,   594,   598,
     598,   605,   606,   606,   612,   612,   623,   623,   635,   637,
     638,   639,   643,   645,   647,   648,   649,   650,   651,   652,
     653,   654,   656,   659,   664,   669,   674,   679,   687,   695,
     703
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_NAMESPACE", "T_STATIC", "T_FOR",
  "T_TAUTO", "T_TSTRING", "T_TINT", "T_TCHAR", "T_TLONG", "T_TFLOAT",
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
  "global_declarations", "global_vars", "global_var", "scope",
  "macro_definitions", "constants", "constant_list", "constdef", "$@3",
  "$@4", "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "$@5", "parameter_list",
  "non_empty_pardecl", "par_decl", "macro_definition", "block",
  "var_decls", "var_decl", "statement", "assignment",
  "assignment_or_shorthand_assignment", "shorthand_assignment_operator",
  "assignment_expression", "arithmetic_expression", "simple_expression",
  "range_expression", "$@6", "range_increment", "increment_expression",
  "s_bterm", "binary_expression", "condition", "logical_expression",
  "term", "value", "string", "break", "continue", "return_expression",
  "optional_bterm", "opt_num", "case", "caselabels", "label", "goto",
  "$@7", "in_clause", "$@8", "foreach", "$@9", "$@10", "while", "$@11",
  "$@12", "local_block", "if_expression", "$@13", "$@14", "else_clause",
  "$@15", "stmntlist", "$@16", "call_expression", "@17", "@18",
  "nonempty_parameters", "parameter", "type_cast", "type_ident",
  "boolean_literal", "float_literal", "integer_literal",
  "character_literal", "simple_literal", 0
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
      91,    92,    92,    93,    94,    94,    95,    95,    96,    97,
      97,    99,   100,    98,   101,   101,   102,   102,   102,   102,
     103,   103,   103,   103,   103,   103,   104,   104,   105,   105,
     105,   105,   105,   105,   105,   107,   106,   108,   108,   109,
     109,   110,   111,   112,   113,   113,   114,   114,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   116,   117,   117,   118,   118,   118,   118,
     118,   119,   120,   120,   121,   121,   121,   122,   123,   122,
     124,   124,   125,   125,   126,   126,   127,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   127,   128,   129,   129,   129,   129,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   130,   130,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   132,
     132,   133,   134,   135,   136,   136,   137,   137,   138,   139,
     139,   140,   141,   142,   141,   144,   143,   146,   147,   145,
     149,   150,   148,   151,   151,   153,   154,   152,   155,   156,
     155,   157,   158,   157,   160,   159,   161,   159,   108,   162,
     162,   162,   163,   164,   165,   165,   165,   165,   165,   165,
     165,   165,   166,   166,   167,   168,   169,   170,   170,   170,
     170
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     4,     2,     0,     3,     0,     2,
       2,     0,     2,     2,     0,     1,     0,     2,     3,     1,
       3,     0,     0,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     0,     5,     0,     1,     3,
       1,     2,     3,     4,     0,     2,     4,     3,     1,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     2,     1,     1,     1,     1,
       1,     2,     1,     3,     1,     4,     1,     0,     0,     5,
       0,     2,     2,     2,     1,     1,     1,     2,     2,     2,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     0,     1,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     3,     2,     0,     1,     0,     1,     5,     0,
       3,     2,     2,     0,     4,     0,     7,     0,     0,     5,
       0,     0,     5,     1,     3,     0,     0,     6,     0,     0,
       3,     0,     0,     3,     0,     5,     0,     5,     1,     1,
       3,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     6,     1,     0,     3,     8,     0,    16,     5,
       7,    14,    15,    21,    10,    11,     0,     9,    45,    17,
       0,    19,     0,    12,   190,   189,   185,   184,   186,   187,
     191,   188,    13,     0,     0,     0,    18,    21,    26,    28,
      29,    31,    32,    35,    30,    34,    33,     0,    27,     0,
      54,    52,    39,    41,    42,    43,    44,    40,    38,     0,
       0,    20,     0,    87,    57,     0,     0,    54,     0,    36,
      37,   195,   194,    25,   196,   192,   193,    22,   200,   198,
     197,   199,    24,   138,   139,     0,   130,   132,   131,   133,
     135,   134,     0,     0,     0,    87,    81,    82,    86,    96,
      84,    94,   136,    95,     0,   137,    56,    72,   157,     0,
       0,   160,     0,     0,     0,   146,   146,   144,    58,     0,
      69,   172,     0,    70,    65,    66,    67,    64,    71,    68,
      63,    62,    61,     0,     0,    55,   181,     0,     0,    48,
      50,   182,   178,   179,     0,    23,   176,   174,    99,   100,
      98,   101,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   140,    97,
       0,   151,   115,   165,     0,   152,   153,     0,   147,     0,
       0,   145,   143,    93,    92,    79,    80,    76,    77,    78,
      73,    87,    74,     0,    60,    53,    59,     0,    46,     0,
       0,    51,     0,     0,    83,   113,   183,    88,   110,   111,
     112,   102,   103,   105,   104,   106,   107,   108,   109,     0,
       0,   158,   115,     0,     0,   116,    94,    95,     0,   161,
       0,   149,   141,   142,    75,   173,    49,   180,     0,     0,
      90,    85,     0,     0,   117,     0,     0,   115,   115,   115,
     115,   115,   115,   115,   115,   114,     0,   163,   166,     0,
     154,     0,     0,     0,   177,   175,     0,    89,   155,   159,
     127,   126,   125,   124,   119,   118,   122,   123,   120,   121,
       0,   168,   162,   148,   149,    91,     0,   164,   169,   167,
     150,    87,     0,     0,   170,   156
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,     5,     6,     9,    14,    15,    16,
      11,    17,    20,    21,    22,   145,    77,    47,   119,    59,
      60,    34,    35,   138,   139,   140,    19,   120,    66,    67,
     257,   122,   190,   191,    65,    96,    97,    98,   240,   267,
     123,    99,   223,   173,   224,   225,   101,   102,   124,   125,
     126,   182,   179,   127,   262,   128,   129,   230,   221,   286,
     130,   170,   243,   131,   174,   259,   258,   132,   228,   281,
     289,   292,   133,   193,   103,   203,   202,   142,   143,   104,
      33,    78,    79,    80,    81,   105
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -231
static const yytype_int16 yypact[] =
{
    -231,    19,    22,  -231,     9,  -231,  -231,   -49,  -231,   400,
    -231,    70,  -231,  -231,  -231,    26,   677,  -231,  -231,  -231,
      15,  -231,   472,  -231,  -231,  -231,  -231,  -231,  -231,  -231,
    -231,  -231,  -231,   472,   -28,    30,  -231,  -231,  -231,  -231,
    -231,  -231,  -231,  -231,  -231,  -231,  -231,    25,  -231,   -26,
     677,  -231,  -231,  -231,  -231,  -231,  -231,  -231,  -231,   -13,
      13,  -231,   545,     1,  -231,    17,   133,   677,   330,  -231,
    -231,  -231,  -231,  -231,  -231,  -231,  -231,  -231,  -231,  -231,
    -231,  -231,  -231,    -1,  -231,    11,  -231,  -231,  -231,  -231,
    -231,  -231,   424,   424,   460,   377,  -231,  -231,  -231,  -231,
     473,  -231,    47,  -231,   105,  -231,  -231,  -231,  -231,   -17,
      31,  -231,    98,   100,    31,    96,    96,   424,  -231,   331,
    -231,  -231,    39,  -231,  -231,  -231,  -231,  -231,  -231,  -231,
    -231,  -231,  -231,    35,    41,  -231,  -231,   377,    49,    52,
    -231,   608,    54,  -231,   472,  -231,  -231,  -231,   194,    97,
    -231,    97,    53,   226,    57,   424,   424,   424,   424,   424,
     424,   424,   424,   424,   424,   424,   424,   424,  -231,  -231,
      48,  -231,   413,  -231,    31,  -231,  -231,    65,  -231,    61,
      71,   608,  -231,  -231,  -231,  -231,  -231,  -231,  -231,  -231,
    -231,     1,  -231,   133,  -231,  -231,  -231,   559,  -231,   677,
     424,  -231,   330,   330,  -231,  -231,  -231,   608,   343,   343,
    -231,   194,   194,   194,    97,    97,  -231,  -231,  -231,   504,
     677,  -231,   413,   586,   167,  -231,   532,   549,   282,  -231,
      78,   144,  -231,  -231,  -231,  -231,  -231,  -231,    79,    80,
     142,  -231,   472,   282,     2,   424,   424,   413,   413,   413,
     413,   413,   413,   413,   413,  -231,   230,  -231,  -231,   282,
    -231,    83,   101,   -28,  -231,  -231,   424,  -231,  -231,  -231,
     608,   608,   150,   150,   303,   303,     2,     2,   -15,   -15,
     114,   157,  -231,  -231,   144,   608,   113,  -231,  -231,  -231,
    -231,   424,   282,   124,  -231,  -231
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -231,  -231,  -231,  -231,  -231,  -231,  -231,   189,  -231,   195,
    -231,  -231,  -231,   173,  -231,  -231,  -231,   -30,   -21,  -231,
    -231,  -231,  -231,   -95,  -231,    12,  -231,   -34,   148,   204,
     -58,  -231,  -231,  -231,   107,    36,   -91,  -231,  -231,  -231,
    -231,   140,   -61,   -86,   302,  -231,  -167,  -231,  -231,  -231,
    -231,  -231,   112,  -231,   -39,  -220,  -231,  -231,  -231,  -231,
    -231,  -231,  -231,  -231,  -231,  -231,  -230,  -231,  -231,  -231,
    -231,  -231,  -183,  -231,   -60,  -231,  -231,  -231,    55,  -231,
     -59,  -231,  -231,  -231,  -231,   187
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -172
static const yytype_int16 yytable[] =
{
      51,    48,   100,    49,   152,   226,   134,   141,   121,   144,
     235,   263,    48,   269,    83,    71,    72,    84,    74,     3,
      75,    76,     7,    85,    63,     4,    69,    10,   177,   282,
      12,   148,   149,   151,   153,    70,   154,    52,    53,    54,
      55,    56,   251,   252,    86,    87,    88,    89,    90,    91,
      64,    57,    50,   247,   248,   226,   181,   249,   250,   251,
     252,   146,   294,   168,   263,    68,    92,   171,   253,   254,
      -4,    93,    94,   280,    12,    62,   197,   146,   154,    95,
     226,   226,   226,   226,   226,   226,   226,   226,   229,   147,
      58,    36,    37,   106,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   238,   239,   172,
     178,   175,   227,   176,   201,   194,   195,   196,    83,    71,
      72,    84,    74,    48,    75,    76,   220,    85,   198,   199,
     100,   200,   204,   134,   107,   121,   206,   232,   108,   141,
     144,   141,   141,   144,   144,   231,   109,   233,    86,    87,
      88,    89,    90,    91,   260,    85,   158,   261,   264,   265,
     110,   242,   227,   111,   112,   113,   266,   171,   134,   164,
     165,   166,   114,   115,   116,   117,    41,    42,    43,    44,
      45,    46,   283,   134,   270,   271,   288,   227,   227,   227,
     227,   227,   227,   227,   227,   287,   134,   291,   121,   134,
     293,  -172,  -172,   295,    23,   285,    18,   251,   252,   118,
      61,   236,   268,    50,  -171,   135,   253,   254,   247,   248,
      32,    48,   249,   250,   251,   252,   192,   234,   180,   284,
     100,   107,   134,   253,   254,   108,    24,    25,    26,    27,
      28,    29,    30,   109,   169,   290,   255,   156,   157,    82,
     155,    31,    85,   158,     0,   237,     0,   110,     0,     0,
     111,   112,   113,     0,   162,   163,   164,   165,   166,   114,
     115,   116,   117,    41,    42,    43,    44,    45,    46,   156,
     157,     0,     0,   107,     0,   158,     0,   108,   159,   160,
     161,     0,     0,     0,     0,   109,   162,   163,   164,   165,
     166,   167,     0,     0,    85,   205,   118,     0,     0,   110,
      50,   -54,   111,   112,   113,     0,     0,     0,     0,     0,
       0,   114,   115,   116,   117,    41,    42,    43,    44,    45,
      46,   136,     0,     0,     0,     0,    24,    25,    26,    27,
      28,    29,    30,    83,    71,    72,    84,    74,     0,    75,
      76,    31,    85,     0,   247,   248,   183,   184,   118,     0,
     251,   252,   256,     0,   185,   186,   187,   188,   189,   253,
     254,     0,     0,    86,    87,    88,    89,    90,    91,     0,
       0,    63,     0,    24,    25,    26,    27,    28,    29,    30,
      83,    71,    72,    84,    74,    92,    75,    76,    31,    85,
      93,    94,   158,     0,    12,     0,   -11,     0,   137,   -47,
       0,     0,   -11,   162,   163,   164,   165,   166,    13,     0,
      86,    87,    88,    89,    90,    91,    83,    71,    72,    84,
      74,     0,    75,    76,     0,    85,     0,    83,    71,    72,
      84,    74,    92,    75,    76,     0,    85,    93,    94,     0,
       0,     0,     0,     0,     0,   137,    86,    87,    88,    89,
      90,    91,     0,     0,     0,     0,     0,    86,    87,    88,
      89,    90,    91,    83,   150,    72,    84,    74,    92,    75,
      76,     0,    85,    93,    94,    38,    39,     0,    40,    92,
       0,   137,     0,     0,    93,    94,   222,   155,     0,     0,
       0,     0,   137,    86,    87,    88,    89,    90,    91,     0,
       0,     0,     0,     0,     0,    41,    42,    43,    44,    45,
      46,     0,     0,     0,   244,    92,   156,   157,     0,     0,
      93,    94,   158,     0,     0,   159,   160,   161,   137,     0,
       0,     0,     0,   162,   163,   164,   165,   166,   167,   272,
     273,   274,   275,   276,   277,   278,   279,   156,   157,    71,
      72,    73,    74,   158,    75,    76,   159,   160,   161,     0,
       0,     0,     0,     0,   162,   163,   164,   165,   166,     0,
       0,     0,     0,  -129,  -129,     0,   241,  -129,  -129,  -129,
    -129,     0,     0,     0,     0,     0,     0,     0,  -129,  -129,
    -128,  -128,     0,     0,  -128,  -128,  -128,  -128,     0,     0,
       0,  -129,   156,   157,     0,  -128,  -128,     0,   158,     0,
       0,   159,   160,   161,     0,     0,     0,     0,  -128,   162,
     163,   164,   165,   166,     0,   245,     0,     0,   205,   156,
     157,     0,     0,     0,     0,   158,     0,     0,   159,   160,
     161,   246,     0,     0,     0,     0,   162,   163,   164,   165,
     166,   156,   157,     0,     0,     0,     0,   158,     0,     0,
     159,   160,   161,     0,     0,     0,     0,     0,   162,   163,
     164,   165,   166,    24,    25,    26,    27,    28,    29,    30,
       0,     0,     0,     0,     0,     0,     0,     0,    31
};

static const yytype_int16 yycheck[] =
{
      34,    22,    63,    33,    95,   172,    66,    68,    66,    68,
     193,   231,    33,   243,    13,    14,    15,    16,    17,     0,
      19,    20,    13,    22,    50,     3,    13,    76,   114,   259,
       4,    92,    93,    94,    95,    22,    95,     7,     8,     9,
      10,    11,    57,    58,    43,    44,    45,    46,    47,    48,
      76,    21,    80,    51,    52,   222,   117,    55,    56,    57,
      58,    78,   292,    16,   284,    78,    65,    84,    66,    67,
       0,    70,    71,   256,     4,    50,   137,    78,   137,    78,
     247,   248,   249,   250,   251,   252,   253,   254,   174,    78,
      60,    76,    77,    76,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   202,   203,    78,
      14,    13,   172,    13,   144,    76,    81,    76,    13,    14,
      15,    16,    17,   144,    19,    20,    78,    22,    79,    77,
     191,    77,    79,   193,     1,   193,    79,    76,     5,   200,
     199,   202,   203,   202,   203,    80,    13,    76,    43,    44,
      45,    46,    47,    48,    76,    22,    59,    13,    79,    79,
      27,   220,   222,    30,    31,    32,    24,    84,   228,    72,
      73,    74,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    81,   243,   245,   246,    29,   247,   248,   249,
     250,   251,   252,   253,   254,    81,   256,    84,   256,   259,
     291,    51,    52,    79,    15,   266,    11,    57,    58,    76,
      37,   199,   242,    80,    81,    67,    66,    67,    51,    52,
      16,   242,    55,    56,    57,    58,   119,   191,   116,   263,
     291,     1,   292,    66,    67,     5,     6,     7,     8,     9,
      10,    11,    12,    13,   104,   284,    79,    53,    54,    62,
      24,    21,    22,    59,    -1,   200,    -1,    27,    -1,    -1,
      30,    31,    32,    -1,    70,    71,    72,    73,    74,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    53,
      54,    -1,    -1,     1,    -1,    59,    -1,     5,    62,    63,
      64,    -1,    -1,    -1,    -1,    13,    70,    71,    72,    73,
      74,    75,    -1,    -1,    22,    79,    76,    -1,    -1,    27,
      80,    81,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,     1,    -1,    -1,    -1,    -1,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    -1,    19,
      20,    21,    22,    -1,    51,    52,    25,    26,    76,    -1,
      57,    58,    80,    -1,    33,    34,    35,    36,    37,    66,
      67,    -1,    -1,    43,    44,    45,    46,    47,    48,    -1,
      -1,    50,    -1,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    65,    19,    20,    21,    22,
      70,    71,    59,    -1,     4,    -1,     6,    -1,    78,    79,
      -1,    -1,    12,    70,    71,    72,    73,    74,    18,    -1,
      43,    44,    45,    46,    47,    48,    13,    14,    15,    16,
      17,    -1,    19,    20,    -1,    22,    -1,    13,    14,    15,
      16,    17,    65,    19,    20,    -1,    22,    70,    71,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    43,    44,    45,    46,
      47,    48,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,
      46,    47,    48,    13,    14,    15,    16,    17,    65,    19,
      20,    -1,    22,    70,    71,    13,    14,    -1,    16,    65,
      -1,    78,    -1,    -1,    70,    71,    83,    24,    -1,    -1,
      -1,    -1,    78,    43,    44,    45,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    -1,    -1,    -1,   222,    65,    53,    54,    -1,    -1,
      70,    71,    59,    -1,    -1,    62,    63,    64,    78,    -1,
      -1,    -1,    -1,    70,    71,    72,    73,    74,    75,   247,
     248,   249,   250,   251,   252,   253,   254,    53,    54,    14,
      15,    16,    17,    59,    19,    20,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    70,    71,    72,    73,    74,    -1,
      -1,    -1,    -1,    51,    52,    -1,    82,    55,    56,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    67,
      51,    52,    -1,    -1,    55,    56,    57,    58,    -1,    -1,
      -1,    79,    53,    54,    -1,    66,    67,    -1,    59,    -1,
      -1,    62,    63,    64,    -1,    -1,    -1,    -1,    79,    70,
      71,    72,    73,    74,    -1,    49,    -1,    -1,    79,    53,
      54,    -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    63,
      64,    65,    -1,    -1,    -1,    -1,    70,    71,    72,    73,
      74,    53,    54,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    63,    64,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,    73,    74,     6,     7,     8,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    21
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    86,    87,     0,     3,    89,    90,    13,    88,    91,
      76,    95,     4,    18,    92,    93,    94,    96,    94,   111,
      97,    98,    99,    92,     6,     7,     8,     9,    10,    11,
      12,    21,   114,   165,   106,   107,    76,    77,    13,    14,
      16,    43,    44,    45,    46,    47,    48,   102,   103,   102,
      80,   112,     7,     8,     9,    10,    11,    21,    60,   104,
     105,    98,    50,    50,    76,   119,   113,   114,    78,    13,
      22,    14,    15,    16,    17,    19,    20,   101,   166,   167,
     168,   169,   170,    13,    16,    22,    43,    44,    45,    46,
      47,    48,    65,    70,    71,    78,   120,   121,   122,   126,
     127,   131,   132,   159,   164,   170,    76,     1,     5,    13,
      27,    30,    31,    32,    39,    40,    41,    42,    76,   103,
     112,   115,   116,   125,   133,   134,   135,   138,   140,   141,
     145,   148,   152,   157,   159,   113,     1,    78,   108,   109,
     110,   127,   162,   163,   165,   100,    78,    78,   127,   127,
      14,   127,   121,   127,   165,    24,    53,    54,    59,    62,
      63,    64,    70,    71,    72,    73,    74,    75,    16,   126,
     146,    84,    78,   128,   149,    13,    13,   128,    14,   137,
     137,   127,   136,    25,    26,    33,    34,    35,    36,    37,
     117,   118,   119,   158,    76,    81,    76,   127,    79,    77,
      77,   102,   161,   160,    79,    79,    79,   127,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
      78,   143,    83,   127,   129,   130,   131,   159,   153,   128,
     142,    80,    76,    76,   120,   157,   110,   163,   108,   108,
     123,    82,   165,   147,   129,    49,    65,    51,    52,    55,
      56,    57,    58,    66,    67,    79,    80,   115,   151,   150,
      76,    13,   139,   140,    79,    79,    24,   124,   102,   151,
     127,   127,   129,   129,   129,   129,   129,   129,   129,   129,
     157,   154,   151,    81,   112,   127,   144,    81,    29,   155,
     139,    84,   156,   121,   151,    79
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
#line 156 "Parser.y"
    {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 162 "Parser.y"
    {
				finalize_header();
			;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 170 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 173 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace((yyvsp[(2) - (3)]).ident.s);
				free((yyvsp[(2) - (3)]).ident.s);
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 187 "Parser.y"
    { _macroScope = MS_GLOBAL; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 188 "Parser.y"
    { _macroScope = MS_LOCAL; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 201 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 201 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 202 "Parser.y"
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

  case 24:

/* Line 1455 of yacc.c  */
#line 213 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 216 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = S_STRING;
			;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 221 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 222 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 224 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 231 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 237 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 238 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 239 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 240 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 241 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 242 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 246 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 249 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 263 "Parser.y"
    { init_macroDefinition(); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 264 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (5)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 276 "Parser.y"
    {
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 283 "Parser.y"
    {
				if (!yyerrflg) {
					bytecode_generateAutoLabelNamePrefix(lreturnid,0);
					bytecode_destroyAutoLabelNamePrefix(lreturnid,0);
					macro_validateMacroName((yyvsp[(2) - (3)]).ident.s, -1, 1);
					YY_EMIT(C_STOP, (GENERIC_DATA){1});
					_compilerConfiguration->cb_showStatus("Defining macro %s",(yyvsp[(2) - (3)]).ident.s);
					MACRO_PARAM param = {
						.mp_name = (yyvsp[(2) - (3)]).ident.s,
						.mp_comment = _yyCurrentComment,
						.mp_bytecodeLength = _bytecodeBuffer.bb_current - _bytecodeBuffer.bb_start,
						.mp_buffer = _bytecodeBuffer.bb_start,
						.mp_scope = _macroScope,
						.mp_namespaceIdx = _currentNamespaceIndex
					};
					_compilerConfiguration->cb_insertNewMacro(&param);
				}
				freeitem(&(yyvsp[(2) - (3)]).ident.s);
				freeitem(&_yyCurrentComment);
				bytecode_closeOpenLabels();
				finalize_macroDefinition();
			;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 311 "Parser.y"
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

  case 57:

/* Line 1455 of yacc.c  */
#line 320 "Parser.y"
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

  case 59:

/* Line 1455 of yacc.c  */
#line 333 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 334 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 336 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 337 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 338 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 339 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 340 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 341 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 342 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 343 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 344 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 345 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 346 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label."); 
			;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
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

  case 74:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    {
				_generateShorthandAssignment = 0;
				(yyval).v = (yyvsp[(1) - (1)]).v;
			;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 367 "Parser.y"
    { 
				_generateShorthandAssignment = (yyvsp[(1) - (2)]).type;
				(yyval).v = (yyvsp[(2) - (2)]).v;
			;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 373 "Parser.y"
    { (yyval).type = BIN_MUL; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 374 "Parser.y"
    { (yyval).type = BIN_DIV; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 375 "Parser.y"
    { (yyval).type = BIN_MOD; ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 376 "Parser.y"
    { (yyval).type = BIN_ADD; ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 377 "Parser.y"
    { (yyval).type = BIN_SUB; ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 379 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 381 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 382 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 384 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 385 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 389 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 392 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 397 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 402 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 403 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 407 "Parser.y"
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

  case 99:

/* Line 1455 of yacc.c  */
#line 416 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 418 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 419 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 420 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 421 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 422 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 423 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 424 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 426 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 427 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 428 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 429 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 430 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 438 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 439 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 440 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 441 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 442 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 443 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 444 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 445 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 446 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 447 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 448 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 450 "Parser.y"
    { (yyval).type = S_NUMBER; 	 	;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 451 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 453 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_NUMBER;
			;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 457 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_FLOAT;
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 461 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_RANGE;
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 465 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_STRING;
			;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 469 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 473 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_BOOLEAN;
			;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 477 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 482 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 483 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 489 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident;	;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 490 "Parser.y"
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

  case 141:

/* Line 1455 of yacc.c  */
#line 502 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 505 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 508 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 515 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 516 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 523 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 528 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 532 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 537 "Parser.y"
    {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(3) - (3)]).ident.s});
				if ((yyvsp[(2) - (3)]).type == S_STRING) {
					makeInternalSymbol((yyvsp[(3) - (3)]).ident.s, (yyvsp[(2) - (3)]).type, (intptr_t)"");
				} else {
					makeInternalSymbol((yyvsp[(3) - (3)]).ident.s, (yyvsp[(2) - (3)]).type, (intptr_t)0);
				}
				if ((yyvsp[(3) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(3) - (3)]).ident.s);
				}
			;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 548 "Parser.y"
    {
				PKS_VALUE 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 555 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 557 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 560 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 568 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 570 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 573 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 584 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 587 "Parser.y"
    {
				_iflevel--;
			;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 589 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 594 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 598 "Parser.y"
    {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 605 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 606 "Parser.y"
    { 
				if ((yyvsp[(1) - (1)]).needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 612 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (2)]).funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = (yyvsp[(1) - (2)]).funcp;
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 618 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 623 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				_calledFunctionOrMacro = (yyvsp[(1) - (2)]).ident.s;
				(yyval).funcp = 0;
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 629 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_MACRO,(GENERIC_DATA){.string = _calledFunctionOrMacro}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&_calledFunctionOrMacro);
			;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 639 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 643 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; (*_currentFunctionCallParamIndexP)++; ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 645 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 647 "Parser.y"
    {	(yyval).type = S_CHARACTER;	;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 648 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 649 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 650 "Parser.y"
    {	(yyval).type = S_FLOAT;	;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 651 "Parser.y"
    {	(yyval).type = S_BOOLEAN; ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 652 "Parser.y"
    {	(yyval).type = S_STRING;	;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 653 "Parser.y"
    {	(yyval).type = S_NUMBER; /* TODO: introduce type-inference type placeholder here */	;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 654 "Parser.y"
    {	(yyval).type = S_RANGE;	;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 656 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 659 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 664 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 669 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 674 "Parser.y"
    {
				(yyval).v.type = C_PUSH_CHARACTER_LITERAL; 
				(yyval).v.data.uchar = (char)(yyvsp[(1) - (1)]).num;
			;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 679 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.longValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_NUMBER;
			;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 687 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_FLOAT;
			;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 695 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 703 "Parser.y"
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
#line 3231 "Parser.tab.c"
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
#line 711 "Parser.y"


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


