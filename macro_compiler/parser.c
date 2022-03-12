
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
#line 48 "Parser.y"


#include <windows.h>
#include <stdio.h>

#include "alloc.h"
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "scanner.h"
#include "test.h"
#include "symbols.h"
#include "arraylist.h"

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
extern 		int	yyerrflg;
extern 		int	_bDefiningConst;
extern char*    _yyCurrentComment;
extern void		yyerror(char* s, ...);
extern void		yywarning(char* s, ...);
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
	if (pContext && pContext != sym_getGlobalContext()) {
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



/* Line 189 of yacc.c  */
#line 213 "Parser.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
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
     T_TYPE_IDENTIFIER = 261,
     T_IDENT = 262,
     T_NUM = 263,
     T_FLOATING_POINT_NUMBER = 264,
     T_STRING = 265,
     T_CHARACTER = 266,
     T_CONST = 267,
     T_TRUE = 268,
     T_FALSE = 269,
     T_TBOOLEAN = 270,
     T_FUNC = 271,
     T_MACRO = 272,
     T_DOTDOT = 273,
     T_PLUSPLUS = 274,
     T_MINUSMINUS = 275,
     T_IF = 276,
     T_ELIF = 277,
     T_ELSE = 278,
     T_WHILE = 279,
     T_GOTO = 280,
     T_BRAEQ = 281,
     T_SH_ASSIGN_PLUS = 282,
     T_SH_ASSIGN_MINUS = 283,
     T_SH_ASSIGN_MULT = 284,
     T_SH_ASSIGN_DIV = 285,
     T_SH_ASSIGN_MOD = 286,
     T_LOCAL = 287,
     T_SWITCH = 288,
     T_BREAK = 289,
     T_CONTINUE = 290,
     T_RETURN = 291,
     T_VARIABLE = 292,
     T_NMATCH = 293,
     T_ASSIGN = 294,
     T_NE = 295,
     T_EQ = 296,
     T_SHIFT_LEFT = 297,
     T_SHIFT_RIGHT = 298,
     T_OR = 299,
     T_AND = 300,
     T_LE = 301,
     T_GE = 302,
     T_POWER_TO = 303,
     T_VOID = 304,
     T_CMDSEQPREFIX = 305,
     T_GT = 306,
     T_LT = 307
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
#line 307 "Parser.tab.c"

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
#define YYLAST   1100

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  97
/* YYNRULES -- Number of rules.  */
#define YYNRULES  205
/* YYNRULES -- Number of states.  */
#define YYNSTATES  313

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    71,     2,     2,     2,    63,    51,     2,
      67,    68,    61,    59,    66,    60,     2,    62,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    73,    65,
      55,     2,    56,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,     2,    70,    53,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    69,    52,    72,    54,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    57,    58
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,    10,    13,    14,    18,    19,
      22,    25,    26,    29,    32,    33,    35,    36,    39,    43,
      45,    49,    50,    51,    57,    59,    61,    63,    65,    67,
      69,    71,    73,    76,    79,    81,    83,    84,    91,    92,
      94,    95,    97,   101,   103,   106,   110,   115,   116,   119,
     124,   128,   130,   133,   136,   138,   140,   142,   144,   146,
     148,   150,   152,   154,   156,   158,   160,   163,   165,   168,
     171,   177,   178,   183,   185,   187,   189,   191,   193,   196,
     198,   202,   204,   206,   208,   210,   211,   212,   218,   219,
     222,   225,   228,   230,   232,   234,   237,   240,   245,   248,
     251,   254,   258,   262,   266,   270,   274,   278,   282,   286,
     290,   294,   298,   302,   306,   310,   314,   318,   322,   326,
     330,   334,   338,   342,   346,   350,   353,   354,   356,   359,
     361,   363,   365,   367,   369,   371,   373,   374,   378,   379,
     384,   388,   390,   392,   394,   397,   401,   405,   408,   409,
     411,   412,   414,   420,   421,   425,   428,   431,   432,   437,
     438,   446,   447,   448,   454,   455,   456,   462,   464,   468,
     469,   471,   472,   473,   480,   481,   482,   486,   487,   488,
     492,   493,   499,   500,   506,   509,   511,   513,   515,   517,
     521,   523,   525,   529,   531,   534,   537,   541,   543,   545,
     547,   549,   551,   553,   555,   557
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      75,     0,    -1,    -1,    -1,    76,    78,    77,    84,    -1,
      79,    80,    -1,    -1,     3,     7,    65,    -1,    -1,    80,
      85,    -1,    80,    81,    -1,    -1,    82,    81,    -1,    83,
     104,    -1,    -1,     4,    -1,    -1,    84,   101,    -1,    12,
      86,    65,    -1,    87,    -1,    86,    66,    87,    -1,    -1,
      -1,    88,    91,    39,    90,    89,    -1,   170,    -1,    10,
      -1,     7,    -1,    92,    -1,     8,    -1,    10,    -1,    37,
      -1,     7,    -1,    94,     7,    -1,    94,    16,    -1,    49,
      -1,     6,    -1,    -1,    96,    93,    67,    98,    68,    97,
      -1,    -1,    10,    -1,    -1,    99,    -1,    99,    66,   100,
      -1,   100,    -1,   164,    91,    -1,    83,    95,   102,    -1,
      69,   103,   155,   149,    -1,    -1,   104,   103,    -1,   164,
      91,   110,    65,    -1,   164,    91,    65,    -1,    65,    -1,
     157,    65,    -1,   107,    65,    -1,   150,    -1,   145,    -1,
     142,    -1,   135,    -1,   130,    -1,   131,    -1,   132,    -1,
     138,    -1,   102,    -1,   117,    -1,   137,    -1,   104,    -1,
     106,    65,    -1,     1,    -1,   106,     1,    -1,    92,   110,
      -1,    92,    64,   123,    70,   110,    -1,    -1,    92,   108,
     109,   111,    -1,    29,    -1,    30,    -1,    31,    -1,    27,
      -1,    28,    -1,    39,   111,    -1,   112,    -1,    67,   112,
      68,    -1,   119,    -1,   114,    -1,   119,    -1,   114,    -1,
      -1,    -1,   119,    18,   119,   115,   116,    -1,    -1,    18,
     119,    -1,    92,    20,    -1,    92,    19,    -1,   123,    -1,
     157,    -1,   118,    -1,   163,   118,    -1,    60,     8,    -1,
     119,    64,   113,    70,    -1,    54,   119,    -1,    59,   119,
      -1,    60,   119,    -1,   119,    54,   119,    -1,   119,    38,
     119,    -1,   119,    51,   119,    -1,   119,    52,   119,    -1,
     119,    59,   119,    -1,   119,    53,   119,    -1,   119,    60,
     119,    -1,   119,    61,   119,    -1,   119,    62,   119,    -1,
     119,    63,   119,    -1,   119,    42,   119,    -1,   119,    43,
     119,    -1,   119,    48,   119,    -1,   119,    45,   119,    -1,
     119,    44,   119,    -1,   119,    55,   119,    -1,   119,    56,
     119,    -1,   119,    46,   119,    -1,   119,    47,   119,    -1,
     119,    41,   119,    -1,   119,    40,   119,    -1,    67,   119,
      68,    -1,    67,   121,    68,    -1,    67,   119,    68,    -1,
      67,   121,    -1,    -1,   122,    -1,    71,   121,    -1,   157,
      -1,   123,    -1,    37,    -1,   129,    -1,   170,    -1,   124,
      -1,     7,    -1,    -1,    69,   125,   149,    -1,    -1,    69,
     126,   127,   149,    -1,   127,    66,   128,    -1,   128,    -1,
      10,    -1,    10,    -1,   129,    10,    -1,    34,   134,    65,
      -1,    35,   134,    65,    -1,    36,   133,    -1,    -1,   119,
      -1,    -1,     8,    -1,    33,   120,    69,   136,    72,    -1,
      -1,   137,   102,   136,    -1,     7,    73,    -1,    25,     7,
      -1,    -1,    26,     7,   139,    65,    -1,    -1,    67,   164,
      91,   141,    73,   112,    68,    -1,    -1,    -1,     5,   143,
     140,   144,   148,    -1,    -1,    -1,    24,   146,   120,   147,
     148,    -1,   105,    -1,    69,   155,   149,    -1,    -1,    72,
      -1,    -1,    -1,    21,   120,   151,   148,   152,   153,    -1,
      -1,    -1,    23,   154,   148,    -1,    -1,    -1,   105,   156,
     155,    -1,    -1,    16,    67,   158,    98,    68,    -1,    -1,
     160,    67,   159,    98,    68,    -1,    61,    92,    -1,     7,
      -1,     1,    -1,   161,    -1,   162,    -1,   161,    66,   162,
      -1,     1,    -1,   112,    -1,    67,   164,    68,    -1,     6,
      -1,     6,   165,    -1,    64,    70,    -1,    64,     8,    70,
      -1,    13,    -1,    14,    -1,     9,    -1,     8,    -1,    11,
      -1,   168,    -1,   167,    -1,   169,    -1,   166,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   170,   170,   176,   170,   182,   184,   187,   192,   193,
     194,   196,   197,   199,   201,   202,   204,   205,   209,   212,
     213,   215,   215,   215,   229,   232,   238,   239,   240,   247,
     254,   255,   265,   268,   274,   275,   277,   277,   283,   285,
     290,   291,   294,   296,   298,   305,   328,   330,   331,   333,
     340,   348,   349,   350,   351,   352,   353,   354,   355,   356,
     357,   358,   359,   360,   361,   362,   365,   371,   372,   374,
     379,   384,   384,   393,   394,   395,   396,   397,   399,   401,
     402,   404,   405,   408,   409,   411,   412,   412,   414,   415,
     418,   420,   422,   423,   425,   426,   430,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   458,   459,   460,   461,
     462,   463,   464,   468,   469,   470,   474,   475,   476,   478,
     479,   481,   485,   490,   491,   497,   503,   503,   507,   507,
     512,   513,   516,   520,   521,   532,   535,   539,   543,   544,
     546,   547,   549,   551,   552,   554,   559,   563,   563,   568,
     568,   582,   584,   582,   595,   597,   595,   608,   608,   611,
     613,   616,   619,   616,   626,   630,   630,   637,   638,   638,
     644,   644,   655,   655,   667,   673,   677,   683,   685,   686,
     687,   691,   693,   695,   696,   698,   699,   703,   706,   711,
     716,   721,   726,   734,   742,   750
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_NAMESPACE", "T_STATIC", "T_FOR",
  "T_TYPE_IDENTIFIER", "T_IDENT", "T_NUM", "T_FLOATING_POINT_NUMBER",
  "T_STRING", "T_CHARACTER", "T_CONST", "T_TRUE", "T_FALSE", "T_TBOOLEAN",
  "T_FUNC", "T_MACRO", "T_DOTDOT", "T_PLUSPLUS", "T_MINUSMINUS", "T_IF",
  "T_ELIF", "T_ELSE", "T_WHILE", "T_GOTO", "T_BRAEQ", "T_SH_ASSIGN_PLUS",
  "T_SH_ASSIGN_MINUS", "T_SH_ASSIGN_MULT", "T_SH_ASSIGN_DIV",
  "T_SH_ASSIGN_MOD", "T_LOCAL", "T_SWITCH", "T_BREAK", "T_CONTINUE",
  "T_RETURN", "T_VARIABLE", "T_NMATCH", "T_ASSIGN", "T_NE", "T_EQ",
  "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR", "T_AND", "T_LE", "T_GE",
  "T_POWER_TO", "T_VOID", "T_CMDSEQPREFIX", "'&'", "'|'", "'^'", "'~'",
  "'<'", "'>'", "T_GT", "T_LT", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "';'", "','", "'('", "')'", "'{'", "']'", "'!'", "'}'", "':'", "$accept",
  "file_structure", "$@1", "$@2", "header_declarations", "namespace",
  "global_declarations", "global_vars", "global_var", "scope",
  "macro_definitions", "constants", "constant_list", "constdef", "$@3",
  "$@4", "constant_literal", "variable_identifier", "variable_reference",
  "macro_declaration", "macro_type", "macrostart", "$@5",
  "optional_description", "parameter_list", "non_empty_pardecl",
  "par_decl", "macro_definition", "block", "var_decls", "var_decl",
  "statement", "errors", "assignment", "$@6",
  "shorthand_assignment_operator", "assignment_expression",
  "arithmetic_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@7",
  "range_increment", "increment_expression", "s_bterm",
  "binary_expression", "condition", "logical_expression", "term", "value",
  "array_literal", "$@8", "$@9", "array_elements", "array_element",
  "string", "break", "continue", "return_expression", "optional_bterm",
  "opt_num", "case", "caselabels", "label", "goto", "$@10", "in_clause",
  "$@11", "foreach", "$@12", "$@13", "while", "$@14", "$@15",
  "local_block", "closing_brace", "if_expression", "$@16", "$@17",
  "else_clause", "$@18", "stmntlist", "$@19", "call_expression", "@20",
  "@21", "function_id_or_pointer", "nonempty_parameters", "parameter",
  "type_cast", "type_name", "array_dimension", "boolean_literal",
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
     305,    38,   124,    94,   126,    60,    62,   306,   307,    43,
      45,    42,    47,    37,    91,    59,    44,    40,    41,   123,
      93,    33,   125,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    76,    77,    75,    78,    79,    79,    80,    80,
      80,    81,    81,    82,    83,    83,    84,    84,    85,    86,
      86,    88,    89,    87,    90,    90,    91,    91,    91,    91,
      92,    92,    93,    93,    94,    94,    96,    95,    97,    97,
      98,    98,    99,    99,   100,   101,   102,   103,   103,   104,
     104,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   105,   105,   106,   106,   107,
     107,   108,   107,   109,   109,   109,   109,   109,   110,   111,
     111,   112,   112,   113,   113,   114,   115,   114,   116,   116,
     117,   117,   118,   118,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   120,   120,   120,   121,   121,   121,   122,
     122,   123,   123,   123,   123,   123,   125,   124,   126,   124,
     127,   127,   128,   129,   129,   130,   131,   132,   133,   133,
     134,   134,   135,   136,   136,   137,   138,   139,   138,   141,
     140,   143,   144,   142,   146,   147,   145,   148,   148,   149,
     149,   151,   152,   150,   153,   154,   153,   155,   156,   155,
     158,   157,   159,   157,   160,   160,   160,    98,   161,   161,
     161,   162,   163,   164,   164,   165,   165,   166,   166,   167,
     168,   169,   170,   170,   170,   170
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     4,     2,     0,     3,     0,     2,
       2,     0,     2,     2,     0,     1,     0,     2,     3,     1,
       3,     0,     0,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     1,     1,     0,     6,     0,     1,
       0,     1,     3,     1,     2,     3,     4,     0,     2,     4,
       3,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     2,
       5,     0,     4,     1,     1,     1,     1,     1,     2,     1,
       3,     1,     1,     1,     1,     0,     0,     5,     0,     2,
       2,     2,     1,     1,     1,     2,     2,     4,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     0,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     0,     3,     0,     4,
       3,     1,     1,     1,     2,     3,     3,     2,     0,     1,
       0,     1,     5,     0,     3,     2,     2,     0,     4,     0,
       7,     0,     0,     5,     0,     0,     5,     1,     3,     0,
       1,     0,     0,     6,     0,     0,     3,     0,     0,     3,
       0,     5,     0,     5,     2,     1,     1,     1,     1,     3,
       1,     1,     3,     1,     2,     2,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     6,     1,     0,     3,     8,     0,    16,     5,
       7,    14,    15,    21,    10,    11,     0,     9,    36,    17,
       0,    19,     0,    12,   193,    13,     0,     0,     0,    18,
      21,    26,    28,    29,    30,     0,    27,     0,   194,     0,
      47,    45,    35,    34,     0,     0,    20,     0,     0,   195,
       0,    50,     0,     0,    47,     0,    32,    33,   200,   199,
      25,   201,   197,   198,    22,   205,   203,   202,   204,    24,
     196,   186,   135,   143,     0,   131,     0,     0,     0,     0,
       0,   136,    78,    79,    82,    94,    81,    92,   134,   132,
      93,     0,     0,   133,    49,    67,   161,    31,     0,   164,
       0,     0,     0,   150,   150,     0,    51,    71,    62,    65,
     178,     0,     0,    63,    58,    59,    60,    57,    64,    61,
      56,    55,    54,   169,     0,    48,   190,     0,     0,    41,
      43,   191,   187,   188,     0,    23,   180,    98,    99,    96,
     100,    31,   184,     0,     0,     0,   169,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   144,   182,    95,     0,   155,     0,   171,     0,   156,
     157,     0,   151,     0,     0,   149,   147,    91,    90,     0,
       0,    69,     0,    68,    66,    53,   170,    46,    52,     0,
      38,     0,     0,    44,     0,    80,   122,   192,   137,   142,
     169,   141,    86,   102,   121,   120,   111,   112,   115,   114,
     118,   119,   113,   103,   104,   106,   101,   116,   117,   105,
     107,   108,   109,   110,     0,    84,    83,     0,     0,   162,
       0,     0,   125,   127,    92,    93,     0,   165,     0,   153,
     145,   146,   135,     0,    76,    77,    73,    74,    75,     0,
     179,    39,    37,    42,   189,     0,     0,   139,    88,    97,
       0,     0,     0,   128,   130,   129,   124,   123,     0,   167,
     172,     0,   158,     0,     0,     0,     0,    72,   181,   140,
       0,    87,   183,   159,   163,    47,   169,   174,   166,   152,
     153,    70,    89,     0,   168,   175,   173,   154,     0,     0,
       0,   176,   160
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,     5,     6,     9,    14,    15,    16,
      11,    17,    20,    21,    22,   135,    64,    35,   107,    44,
      45,    27,    28,   262,   128,   129,   130,    19,   108,    53,
     109,   279,   111,   112,   190,   259,    52,    82,   131,   234,
      84,   268,   291,   113,    85,    86,   177,   242,   243,    87,
      88,   146,   147,   210,   211,    89,   114,   115,   116,   186,
     183,   117,   284,   118,   119,   248,   239,   303,   120,   174,
     272,   121,   178,   281,   280,   197,   122,   246,   297,   306,
     309,   123,   192,    90,   204,   237,    91,   132,   133,    92,
      26,    38,    65,    66,    67,    68,    93
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -256
static const yytype_int16 yypact[] =
{
    -256,    25,    38,  -256,    37,  -256,  -256,   -18,  -256,   112,
    -256,    68,  -256,  -256,  -256,    59,    67,  -256,  -256,  -256,
      64,  -256,    20,  -256,    13,  -256,    20,    57,    16,  -256,
    -256,  -256,  -256,  -256,  -256,    36,  -256,     6,  -256,   -10,
      67,  -256,  -256,  -256,    69,    26,  -256,   109,    71,  -256,
     626,  -256,    74,   264,    67,   523,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,    87,  -256,    90,  -256,   755,   755,   786,    11,
     556,   149,  -256,  -256,  -256,  -256,   897,  -256,  -256,   152,
    -256,    97,   124,  -256,  -256,   102,  -256,   -30,   114,  -256,
     175,   177,   114,   178,   178,   142,  -256,     0,  -256,  -256,
    -256,    48,   123,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,   117,   125,  -256,   102,   663,   126,   129,
    -256,  -256,   131,  -256,    20,  -256,  -256,   310,   230,  -256,
     230,  -256,  -256,   132,   866,   136,   117,   189,   755,   755,
     755,   755,   755,   755,   755,   755,   755,   755,   755,   755,
     755,   755,   755,   755,   755,   755,   755,   755,   755,   755,
     489,  -256,  -256,  -256,   138,  -256,   383,  -256,   114,  -256,
    -256,   137,  -256,   147,   148,   982,  -256,  -256,  -256,   295,
     143,  -256,   264,  -256,  -256,  -256,  -256,  -256,  -256,   924,
     198,    67,   593,  -256,   523,  -256,  -256,  -256,  -256,  -256,
      55,  -256,   982,   310,   180,   180,   364,   364,  1009,  1009,
     982,   982,   -11,  1036,  1036,  1036,   310,   832,   832,   230,
     230,   -11,   -11,   -11,   145,  -256,   897,   523,    67,  -256,
     452,   953,   151,  -256,    45,   836,   713,  -256,   164,   225,
    -256,  -256,  -256,   167,  -256,  -256,  -256,  -256,  -256,   626,
    -256,  -256,  -256,  -256,  -256,   170,   189,  -256,   227,  -256,
     179,    20,   713,  -256,  -256,  -256,  -256,  -256,   314,  -256,
    -256,   713,  -256,   173,   176,    57,   212,  -256,  -256,  -256,
     755,  -256,  -256,  -256,  -256,    67,   117,   231,  -256,  -256,
     225,  -256,   982,   182,  -256,  -256,  -256,  -256,   744,   713,
     185,  -256,  -256
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -256,  -256,  -256,  -256,  -256,  -256,  -256,   241,  -256,   246,
    -256,  -256,  -256,   228,  -256,  -256,  -256,   -22,   -19,  -256,
    -256,  -256,  -256,  -256,  -181,  -256,    62,  -256,   -25,   -45,
     -16,   -47,  -256,  -256,  -256,  -256,   -99,     7,   -49,  -256,
     103,  -256,  -256,  -256,   183,   -65,   -70,    27,  -256,  -131,
    -256,  -256,  -256,  -256,     8,  -256,  -256,  -256,  -256,  -256,
     168,  -256,   -17,  -233,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -255,  -136,  -256,  -256,  -256,  -256,
    -256,  -171,  -256,   -48,  -256,  -256,  -256,  -256,    79,  -256,
     -21,  -256,  -256,  -256,  -256,  -256,   235
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -187
static const yytype_int16 yytable[] =
{
      25,    83,    41,    36,    39,   124,   110,    36,   191,   125,
     208,   137,   138,   140,    48,   144,   285,   294,   141,   187,
     188,   260,    42,   265,    54,     3,   298,    31,    32,    50,
      33,   143,   181,    56,   134,   156,   157,  -185,    54,    50,
     185,     4,    57,   175,     7,   244,  -130,    10,    34,   193,
    -130,  -130,  -130,   170,   311,    51,   270,    34,   253,   145,
     142,  -130,   199,    12,   189,    43,  -130,   285,    -4,  -130,
    -130,  -130,    12,    24,   267,    47,    49,    37,  -130,  -130,
    -130,  -130,  -130,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   236,   145,   296,   247,   274,
    -130,   241,   203,   194,  -130,    36,    12,    58,    59,    60,
      61,   266,    62,    63,    13,    71,    40,   196,   245,    29,
      30,    72,    58,    59,    73,    61,    55,    62,    63,    94,
      74,    70,  -148,    71,   124,   110,  -148,  -148,  -148,    72,
      58,    59,    73,    61,  -185,    62,    63,   136,    74,  -138,
     304,    75,   171,  -148,   172,  -148,  -148,  -148,  -148,  -186,
     254,   255,   256,   257,   258,  -148,  -148,  -148,  -148,    75,
     134,   176,   179,   134,   180,    79,   182,   301,   195,   196,
     198,  -148,   275,    81,   200,   201,    76,   202,   124,   209,
     205,    77,    78,    79,   207,   238,   249,  -148,   261,   127,
      83,    81,   250,   251,  -148,   269,   134,   271,   149,   277,
    -187,  -187,   152,   153,   124,   302,   156,   157,   158,   282,
     124,   110,   283,   124,   162,   163,   164,   286,   288,   165,
     166,   167,   168,   169,   170,   290,   175,   292,   299,   293,
     125,    50,    36,   312,   305,   308,    23,    18,    46,   310,
     300,   124,   295,   263,  -177,    95,   287,   273,  -177,    96,
      24,    97,   184,   235,   289,   173,   156,   157,   158,    54,
      74,   264,    69,   307,     0,    98,     0,  -177,    99,   100,
     101,   167,   168,   169,   170,     0,     0,   102,   103,   104,
     105,    34,   252,    58,    59,    73,    61,     0,    62,    63,
       0,     0,     0,  -177,   -47,    95,     0,     0,   -47,    96,
      24,    97,     0,     0,     0,    79,     0,     0,     0,   106,
      74,     0,    75,    40,     0,    98,  -177,   -47,    99,   100,
     101,     0,     0,     0,     0,     0,     0,   102,   103,   104,
     105,    34,   152,   153,     0,     0,   156,   157,   158,     0,
       0,     0,     0,   -47,    81,   163,   164,     0,     0,   165,
     166,   167,   168,   169,   170,    79,     0,     0,     0,   106,
       0,     0,     0,    40,    71,     0,   -47,     0,  -126,  -126,
      72,    58,    59,    73,    61,     0,    62,    63,     0,    74,
       0,     0,     0,     0,  -126,     0,     0,  -126,  -126,  -126,
     156,   157,   158,     0,     0,     0,  -126,  -126,  -126,  -126,
      75,     0,     0,   165,   166,   167,   168,   169,   170,     0,
       0,     0,     0,     0,     0,     0,     0,    76,     0,     0,
       0,     0,    77,    78,    79,     0,     0,     0,  -126,     0,
     127,  -126,    81,    71,   240,     0,     0,  -126,  -126,    72,
      58,    59,    73,    61,     0,    62,    63,     0,    74,     0,
       0,     0,     0,  -126,     0,     0,  -126,  -126,  -126,     0,
       0,     0,     0,     0,     0,  -126,  -126,  -126,  -126,    75,
      71,     0,     0,     0,     0,     0,    72,    58,    59,    73,
      61,     0,    62,    63,     0,    74,     0,     0,     0,     0,
       0,     0,     0,    79,     0,     0,     0,  -126,     0,     0,
    -126,    81,     0,   240,   126,     0,    75,     0,     0,    24,
      72,    58,    59,    73,    61,     0,    62,    63,     0,    74,
       0,     0,     0,    76,     0,     0,     0,     0,    77,    78,
      79,     0,     0,     0,     0,     0,   127,    71,    81,   -85,
      75,     0,    24,    72,    58,    59,    73,    61,     0,    62,
      63,     0,    74,     0,     0,     0,     0,    76,     0,     0,
       0,     0,    77,    78,    79,     0,     0,     0,     0,   -85,
     127,   -40,    81,    75,    71,     0,     0,     0,     0,     0,
      72,    58,    59,    73,    61,     0,    62,    63,     0,    74,
      76,     0,     0,     0,     0,    77,    78,    79,     0,     0,
       0,     0,     0,   127,   -85,    81,     0,    71,     0,     0,
      75,     0,     0,    72,    58,    59,    73,    61,     0,    62,
      63,     0,    74,     0,     0,     0,     0,    76,     0,     0,
       0,     0,    77,    78,    79,     0,     0,     0,     0,   -85,
     127,   -85,    81,    75,    71,     0,     0,     0,     0,    24,
      72,    58,    59,    73,    61,     0,    62,    63,     0,    74,
      76,     0,     0,     0,     0,    77,    78,    79,     0,     0,
       0,   -85,     0,    80,     0,    81,     0,     0,     0,     0,
      75,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,     0,     0,    76,    96,    24,
      97,     0,    77,    78,    79,     0,     0,     0,     0,    74,
     127,     0,    81,     0,    98,     0,     0,    99,   100,   101,
       0,     0,     0,     0,     0,    71,   102,   103,   104,   105,
      34,    72,    58,    59,    73,    61,    71,    62,    63,     0,
      74,     0,    72,    58,    59,    73,    61,     0,    62,    63,
       0,    74,     0,     0,    79,     0,     0,     0,   106,     0,
       0,    75,   278,     0,     0,     0,     0,    71,     0,     0,
       0,     0,    75,    72,   139,    59,    73,    61,    76,    62,
      63,     0,    74,    77,    78,    79,     0,     0,     0,    76,
       0,   127,   -85,    81,    77,    78,    79,     0,     0,     0,
       0,     0,   127,    75,    81,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -129,     0,     0,
      76,  -129,  -129,  -129,     0,    77,    78,    79,     0,     0,
       0,     0,  -129,   127,     0,    81,     0,  -129,     0,     0,
    -129,  -129,  -129,     0,     0,     0,     0,     0,     0,  -129,
    -129,  -129,  -129,  -129,   152,   153,     0,     0,   156,   157,
     158,     0,     0,     0,   148,     0,     0,     0,     0,     0,
       0,   165,   166,   167,   168,   169,   170,     0,     0,     0,
       0,  -129,     0,     0,   149,  -129,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   148,     0,   159,   160,   161,
     162,   163,   164,     0,     0,   165,   166,   167,   168,   169,
     170,     0,     0,     0,   206,   149,     0,   150,   151,   152,
     153,   154,   155,   156,   157,   158,     0,     0,   159,   160,
     161,   162,   163,   164,     0,     0,   165,   166,   167,   168,
     169,   170,   149,     0,   150,   151,   152,   153,   154,   155,
     156,   157,   158,     0,     0,   159,   160,   161,   162,   163,
     164,     0,     0,   165,   166,   167,   168,   169,   170,     0,
       0,   149,   206,   150,   151,   152,   153,   154,   155,   156,
     157,   158,     0,     0,   159,   160,   161,   162,   163,   164,
       0,     0,   165,   166,   167,   168,   169,   170,     0,     0,
     149,   276,   150,   151,   152,   153,   154,   155,   156,   157,
     158,     0,     0,   159,   160,   161,   162,   163,   164,     0,
       0,   165,   166,   167,   168,   169,   170,   149,     0,   150,
     151,   152,   153,     0,     0,   156,   157,   158,     0,     0,
     159,   160,   161,   162,   163,   164,     0,     0,   165,   166,
     167,   168,   169,   170,   149,     0,   150,   151,   152,   153,
       0,     0,   156,   157,   158,     0,     0,     0,     0,     0,
     162,   163,   164,     0,     0,   165,   166,   167,   168,   169,
     170
};

static const yytype_int16 yycheck[] =
{
      16,    50,    27,    22,    26,    53,    53,    26,   107,    54,
     146,    76,    77,    78,     8,    80,   249,   272,     7,    19,
      20,   192,     6,   204,    40,     0,   281,     7,     8,    39,
      10,    80,   102,     7,    55,    46,    47,    67,    54,    39,
     105,     3,    16,    73,     7,   176,     1,    65,    37,     1,
       5,     6,     7,    64,   309,    65,   237,    37,   189,    80,
      79,    16,   127,     4,    64,    49,    21,   300,     0,    24,
      25,    26,     4,     6,   210,    39,    70,    64,    33,    34,
      35,    36,    37,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   127,   278,   178,   240,
      65,   176,   134,    65,    69,   134,     4,     8,     9,    10,
      11,    66,    13,    14,    12,     1,    69,    72,   176,    65,
      66,     7,     8,     9,    10,    11,    67,    13,    14,    65,
      16,    70,     0,     1,   192,   192,     4,     5,     6,     7,
       8,     9,    10,    11,    67,    13,    14,    67,    16,    10,
     296,    37,    10,    21,    67,    23,    24,    25,    26,    67,
      27,    28,    29,    30,    31,    33,    34,    35,    36,    37,
     201,    67,     7,   204,     7,    61,     8,   286,    65,    72,
      65,    49,   240,    69,    68,    66,    54,    66,   246,    10,
      68,    59,    60,    61,    68,    67,    69,    65,    10,    67,
     259,    69,    65,    65,    72,    70,   237,   238,    38,    68,
      40,    41,    42,    43,   272,   290,    46,    47,    48,    65,
     278,   278,     7,   281,    54,    55,    56,    70,    68,    59,
      60,    61,    62,    63,    64,    18,    73,    68,    72,   271,
     295,    39,   271,    68,    23,    73,    15,    11,    30,   308,
     285,   309,   278,   201,     0,     1,   259,   240,     4,     5,
       6,     7,   104,   170,   266,    92,    46,    47,    48,   295,
      16,   202,    47,   300,    -1,    21,    -1,    23,    24,    25,
      26,    61,    62,    63,    64,    -1,    -1,    33,    34,    35,
      36,    37,     7,     8,     9,    10,    11,    -1,    13,    14,
      -1,    -1,    -1,    49,     0,     1,    -1,    -1,     4,     5,
       6,     7,    -1,    -1,    -1,    61,    -1,    -1,    -1,    65,
      16,    -1,    37,    69,    -1,    21,    72,    23,    24,    25,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    35,
      36,    37,    42,    43,    -1,    -1,    46,    47,    48,    -1,
      -1,    -1,    -1,    49,    69,    55,    56,    -1,    -1,    59,
      60,    61,    62,    63,    64,    61,    -1,    -1,    -1,    65,
      -1,    -1,    -1,    69,     1,    -1,    72,    -1,     5,     6,
       7,     8,     9,    10,    11,    -1,    13,    14,    -1,    16,
      -1,    -1,    -1,    -1,    21,    -1,    -1,    24,    25,    26,
      46,    47,    48,    -1,    -1,    -1,    33,    34,    35,    36,
      37,    -1,    -1,    59,    60,    61,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      -1,    -1,    59,    60,    61,    -1,    -1,    -1,    65,    -1,
      67,    68,    69,     1,    71,    -1,    -1,     5,     6,     7,
       8,     9,    10,    11,    -1,    13,    14,    -1,    16,    -1,
      -1,    -1,    -1,    21,    -1,    -1,    24,    25,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    33,    34,    35,    36,    37,
       1,    -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,
      11,    -1,    13,    14,    -1,    16,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    -1,    -1,    -1,    65,    -1,    -1,
      68,    69,    -1,    71,     1,    -1,    37,    -1,    -1,     6,
       7,     8,     9,    10,    11,    -1,    13,    14,    -1,    16,
      -1,    -1,    -1,    54,    -1,    -1,    -1,    -1,    59,    60,
      61,    -1,    -1,    -1,    -1,    -1,    67,     1,    69,    70,
      37,    -1,     6,     7,     8,     9,    10,    11,    -1,    13,
      14,    -1,    16,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      -1,    -1,    59,    60,    61,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    37,     1,    -1,    -1,    -1,    -1,    -1,
       7,     8,     9,    10,    11,    -1,    13,    14,    -1,    16,
      54,    -1,    -1,    -1,    -1,    59,    60,    61,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    -1,     1,    -1,    -1,
      37,    -1,    -1,     7,     8,     9,    10,    11,    -1,    13,
      14,    -1,    16,    -1,    -1,    -1,    -1,    54,    -1,    -1,
      -1,    -1,    59,    60,    61,    -1,    -1,    -1,    -1,    66,
      67,    68,    69,    37,     1,    -1,    -1,    -1,    -1,     6,
       7,     8,     9,    10,    11,    -1,    13,    14,    -1,    16,
      54,    -1,    -1,    -1,    -1,    59,    60,    61,    -1,    -1,
      -1,    65,    -1,    67,    -1,    69,    -1,    -1,    -1,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     1,    -1,    -1,    54,     5,     6,
       7,    -1,    59,    60,    61,    -1,    -1,    -1,    -1,    16,
      67,    -1,    69,    -1,    21,    -1,    -1,    24,    25,    26,
      -1,    -1,    -1,    -1,    -1,     1,    33,    34,    35,    36,
      37,     7,     8,     9,    10,    11,     1,    13,    14,    -1,
      16,    -1,     7,     8,     9,    10,    11,    -1,    13,    14,
      -1,    16,    -1,    -1,    61,    -1,    -1,    -1,    65,    -1,
      -1,    37,    69,    -1,    -1,    -1,    -1,     1,    -1,    -1,
      -1,    -1,    37,     7,     8,     9,    10,    11,    54,    13,
      14,    -1,    16,    59,    60,    61,    -1,    -1,    -1,    54,
      -1,    67,    68,    69,    59,    60,    61,    -1,    -1,    -1,
      -1,    -1,    67,    37,    69,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,
      54,     5,     6,     7,    -1,    59,    60,    61,    -1,    -1,
      -1,    -1,    16,    67,    -1,    69,    -1,    21,    -1,    -1,
      24,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      34,    35,    36,    37,    42,    43,    -1,    -1,    46,    47,
      48,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    65,    -1,    -1,    38,    69,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    18,    -1,    51,    52,    53,
      54,    55,    56,    -1,    -1,    59,    60,    61,    62,    63,
      64,    -1,    -1,    -1,    68,    38,    -1,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    -1,    -1,    51,    52,
      53,    54,    55,    56,    -1,    -1,    59,    60,    61,    62,
      63,    64,    38,    -1,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    -1,    -1,    51,    52,    53,    54,    55,
      56,    -1,    -1,    59,    60,    61,    62,    63,    64,    -1,
      -1,    38,    68,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    -1,    -1,    51,    52,    53,    54,    55,    56,
      -1,    -1,    59,    60,    61,    62,    63,    64,    -1,    -1,
      38,    68,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    -1,    -1,    51,    52,    53,    54,    55,    56,    -1,
      -1,    59,    60,    61,    62,    63,    64,    38,    -1,    40,
      41,    42,    43,    -1,    -1,    46,    47,    48,    -1,    -1,
      51,    52,    53,    54,    55,    56,    -1,    -1,    59,    60,
      61,    62,    63,    64,    38,    -1,    40,    41,    42,    43,
      -1,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      54,    55,    56,    -1,    -1,    59,    60,    61,    62,    63,
      64
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    75,    76,     0,     3,    78,    79,     7,    77,    80,
      65,    84,     4,    12,    81,    82,    83,    85,    83,   101,
      86,    87,    88,    81,     6,   104,   164,    95,    96,    65,
      66,     7,     8,    10,    37,    91,    92,    64,   165,    91,
      69,   102,     6,    49,    93,    94,    87,    39,     8,    70,
      39,    65,   110,   103,   104,    67,     7,    16,     8,     9,
      10,    11,    13,    14,    90,   166,   167,   168,   169,   170,
      70,     1,     7,    10,    16,    37,    54,    59,    60,    61,
      67,    69,   111,   112,   114,   118,   119,   123,   124,   129,
     157,   160,   163,   170,    65,     1,     5,     7,    21,    24,
      25,    26,    33,    34,    35,    36,    65,    92,   102,   104,
     105,   106,   107,   117,   130,   131,   132,   135,   137,   138,
     142,   145,   150,   155,   157,   103,     1,    67,    98,    99,
     100,   112,   161,   162,   164,    89,    67,   119,   119,     8,
     119,     7,    92,   112,   119,   164,   125,   126,    18,    38,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    51,
      52,    53,    54,    55,    56,    59,    60,    61,    62,    63,
      64,    10,    67,   118,   143,    73,    67,   120,   146,     7,
       7,   120,     8,   134,   134,   119,   133,    19,    20,    64,
     108,   110,   156,     1,    65,    65,    72,   149,    65,   119,
      68,    66,    66,    91,   158,    68,    68,    68,   149,    10,
     127,   128,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   113,   114,   119,   159,    67,   140,
      71,   119,   121,   122,   123,   157,   151,   120,   139,    69,
      65,    65,     7,   123,    27,    28,    29,    30,    31,   109,
     155,    10,    97,   100,   162,    98,    66,   149,   115,    70,
      98,   164,   144,   121,   123,   157,    68,    68,    69,   105,
     148,   147,    65,     7,   136,   137,    70,   111,    68,   128,
      18,   116,    68,    91,   148,   104,   155,   152,   148,    72,
     102,   110,   119,   141,   149,    23,   153,   136,    73,   154,
     112,   148,    68
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
#line 170 "Parser.y"
    {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 176 "Parser.y"
    {
				finalize_header();
			;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 184 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 187 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace((yyvsp[(2) - (3)]).ident.s);
				free((yyvsp[(2) - (3)]).ident.s);
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 201 "Parser.y"
    { _macroScope = MS_GLOBAL; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 202 "Parser.y"
    { _macroScope = MS_LOCAL; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 215 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 215 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 216 "Parser.y"
    {
				BOOL bString = ((yyvsp[(4) - (5)]).v.type == VT_STRING);
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(2) - (5)]).ident.s,
					S_CONSTANT, (yyvsp[(4) - (5)]).v.type, (yyvsp[(4) - (5)]).v.data);
				if ((yyvsp[(2) - (5)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (5)]).ident.s);
				}
				if (bString) {
					free((yyvsp[(4) - (5)]).v.data.string);
				}
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 229 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 232 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = VT_STRING;
			;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 238 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 239 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 241 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 248 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 254 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 255 "Parser.y"
    {   
				yyerror("Using undeclared variable %s", (yyvsp[(1) - (1)]).ident.s);
				// auto-correct by introducing variable
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(1) - (1)]).ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {0});
				free((yyvsp[(1) - (1)]).ident.s);
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
			;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 265 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 268 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 277 "Parser.y"
    { init_macroDefinition(); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 278 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (6)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 285 "Parser.y"
    {
				yywarning("Old Style macro descriptions not supported any more. Use C-Syntax style comments to describe macros.");
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 298 "Parser.y"
    {
				sym_createSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_VARIABLE, VT_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 305 "Parser.y"
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

  case 49:

/* Line 1455 of yacc.c  */
#line 333 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (4)]).ident.s, (yyvsp[(1) - (4)]).type, 0);
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(2) - (4)]).ident.s);
				if ((yyvsp[(2) - (4)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (4)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 340 "Parser.y"
    {
				makeInternalSymbol((yyvsp[(2) - (3)]).ident.s, (yyvsp[(1) - (3)]).type, 0);
				if ((yyvsp[(2) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(2) - (3)]).ident.s);
				}
				vname_count = 0;
			;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 349 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 350 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 352 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 353 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 354 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 355 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 356 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 357 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 358 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 359 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 365 "Parser.y"
    {
				(yyval).needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 374 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 379 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitInstruction(&_bytecodeBuffer, C_ASSIGN_SLOT, (GENERIC_DATA) { .string = (char*)(yyvsp[(1) - (5)]).ident.s });
				vname_count = 0;
			;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 384 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
			;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 386 "Parser.y"
    {
				YY_EMIT(C_BINOP, (GENERIC_DATA){(yyvsp[(3) - (4)]).type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (4)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 393 "Parser.y"
    { (yyval).type = BIN_MUL; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    { (yyval).type = BIN_DIV; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    { (yyval).type = BIN_MOD; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 396 "Parser.y"
    { (yyval).type = BIN_ADD; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 397 "Parser.y"
    { (yyval).type = BIN_SUB; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 401 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 402 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 404 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 412 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 415 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 418 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 420 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 422 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 426 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 430 "Parser.y"
    {
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction(-(yyvsp[(2) - (2)]).num);
				} else {
					(yyval).type = VT_NUMBER;
					(yyval).v.type = C_PUSH_LONG_LITERAL; 
					(yyval).v.data.longValue  = -(yyvsp[(2) - (2)]).num;
				}
			;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 439 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 440 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 441 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 442 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 443 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 444 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 445 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 446 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 447 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 448 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 449 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 450 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 451 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 452 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 453 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 454 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 455 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 456 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 457 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 458 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 459 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 460 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 461 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 462 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 463 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 470 "Parser.y"
    {
				yyerror("Missing closing parenthesis )");
			;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 476 "Parser.y"
    { (yyval).type = VT_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 478 "Parser.y"
    { (yyval).type = VT_NUMBER; 	 	;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 481 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = (yyvsp[(1) - (1)]).ident.type;
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 485 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 490 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 491 "Parser.y"
    {	
				(yyval).type = VT_OBJECT_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				bytecode_destroyArraylistWithPointers(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 497 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 503 "Parser.y"
    {
				_currentArrayLiteral = arraylist_create(1);
			;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 507 "Parser.y"
    {
				_currentArrayLiteral = arraylist_create(3);
			;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 516 "Parser.y"
    {
				arraylist_add(_currentArrayLiteral, (void*)MAKE_TYPED_OBJECT_POINTER(1, VT_STRING, (yyvsp[(1) - (1)]).ident.s));
			;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 520 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 521 "Parser.y"
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

  case 145:

/* Line 1455 of yacc.c  */
#line 533 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 536 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 539 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 546 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 547 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 554 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 559 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 563 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 568 "Parser.y"
    {
				push_newForeachCursor();
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(3) - (3)]).ident.s});
				makeInternalSymbol((yyvsp[(3) - (3)]).ident.s, (yyvsp[(2) - (3)]).type, (intptr_t)0);
				if ((yyvsp[(3) - (3)]).ident.stringIsAlloced) {
					free((yyvsp[(3) - (3)]).ident.s);
				}
			;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 575 "Parser.y"
    {
				SYMBOL 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 582 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 584 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 587 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 595 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 597 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 600 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 611 "Parser.y"
    { yyerror("Missing closing brace '}'"); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 616 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 619 "Parser.y"
    {
				_iflevel--;
			;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 621 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 626 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 630 "Parser.y"
    {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 637 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 638 "Parser.y"
    { 
				if ((yyvsp[(1) - (1)]).needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 644 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (2)]).funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = (yyvsp[(1) - (2)]).funcp;
			;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 650 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 655 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = 0;
			;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 660 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, _functionCallOp,(GENERIC_DATA){.string = (yyvsp[(1) - (5)]).ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&(yyvsp[(1) - (5)]).ident.s);
			;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 667 "Parser.y"
    {
				_functionCallOp = C_MACRO_REF;
				(yyval).ident.s = _strdup((yyvsp[(2) - (2)]).ident.s);
				(yyval).ident.stringIsAlloced = 1;
			;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 673 "Parser.y"
    {
				_functionCallOp = C_MACRO;
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
			;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 677 "Parser.y"
    {
				yyerror("Illegal function call expression");
				yyerrok;
				(yyval).ident.s = 0;
			;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 687 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 691 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; (*_currentFunctionCallParamIndexP)++; ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 693 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 696 "Parser.y"
    {	(yyval).type = VT_OBJECT_ARRAY;	;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 699 "Parser.y"
    {
				_arraySize = (long)(yyvsp[(2) - (3)]).num;
			;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 703 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 706 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 711 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 716 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 721 "Parser.y"
    {
				(yyval).v.type = C_PUSH_CHARACTER_LITERAL; 
				(yyval).v.data.uchar = (char)(yyvsp[(1) - (1)]).num;
			;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 726 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.longValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = VT_NUMBER;
			;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 734 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = VT_FLOAT;
			;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 742 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = VT_CHAR;
			;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 750 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_BOOLEAN_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = VT_BOOLEAN;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 3318 "Parser.tab.c"
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
#line 758 "Parser.y"


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


