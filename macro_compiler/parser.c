
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
int				function_getIndexOfFunction(void *ep);
int 			function_parameterIsFormStart(void *ep,int parno);
extern int macro_lookupNamespace(const char* pszNamespacename);
extern unsigned char* bytecode_emitBinaryOperation(BYTECODE_BUFFER* pBuffer, int nOperationType, PKS_VALUE_TYPE nTargetType);
extern unsigned char* bytecode_emitIncrementExpression(BYTECODE_BUFFER* pBuffer, char* pszName, int nIncrement);
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
static ARRAY_LIST* _currentArrayLiteral;

IDENTIFIER_CONTEXT* _currentIdentifierContext;

static void makeInternalSymbol(const char* pszName, SYMBOL_TYPE sType, intptr_t tVal) {
	bytecode_defineVariable(&_bytecodeBuffer, pszName, C_DEFINE_VARIABLE, sType, tVal);
	IDENTIFIER_CONTEXT* pContext = sym_getContext(_currentIdentifierContext, (char*)pszName);
	if (pContext && pContext != sym_getGlobalContext()) {
		yyerror("Redefinition of variable %s", pszName);
	}
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
#line 204 "Parser.tab.c"

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
     T_STRINGARRAYVAR = 302,
     T_BOOLEANVAR = 303,
     T_CHARACTERVAR = 304,
     T_NMATCH = 305,
     T_ASSIGN = 306,
     T_NE = 307,
     T_EQ = 308,
     T_SHIFT_LEFT = 309,
     T_SHIFT_RIGHT = 310,
     T_OR = 311,
     T_AND = 312,
     T_LE = 313,
     T_GE = 314,
     T_POWER_TO = 315,
     T_VOID = 316,
     T_CMDSEQPREFIX = 317,
     T_GT = 318,
     T_LT = 319
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
#line 310 "Parser.tab.c"

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
#define YYLAST   1511

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  224
/* YYNRULES -- Number of states.  */
#define YYNSTATES  328

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    83,     2,     2,     2,    75,    63,     2,
      80,    81,    73,    71,    78,    72,     2,    74,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    85,    77,
      67,     2,    68,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    76,     2,    79,    65,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    82,    64,    84,    66,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    69,    70
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,    10,    13,    14,    18,    19,
      22,    25,    26,    29,    32,    33,    35,    36,    39,    43,
      45,    49,    50,    51,    57,    59,    61,    63,    65,    67,
      69,    71,    73,    75,    77,    79,    81,    83,    85,    88,
      91,    93,    95,    99,   101,   103,   105,   107,   109,   110,
     116,   117,   119,   123,   125,   128,   132,   137,   138,   141,
     146,   150,   152,   155,   158,   160,   162,   164,   166,   168,
     170,   172,   174,   176,   178,   180,   182,   185,   187,   190,
     193,   194,   199,   201,   203,   205,   207,   209,   212,   214,
     218,   220,   225,   227,   229,   231,   232,   233,   239,   240,
     243,   246,   249,   251,   253,   255,   258,   261,   264,   267,
     270,   274,   278,   282,   286,   290,   294,   298,   302,   306,
     310,   314,   318,   322,   326,   330,   334,   338,   342,   346,
     350,   354,   358,   362,   366,   369,   370,   372,   375,   377,
     379,   381,   383,   385,   387,   389,   391,   393,   395,   397,
     399,   401,   402,   406,   407,   412,   416,   418,   420,   423,
     427,   431,   434,   435,   437,   438,   440,   446,   447,   451,
     454,   457,   458,   463,   464,   472,   473,   474,   480,   481,
     482,   488,   490,   494,   495,   497,   498,   499,   506,   507,
     508,   512,   513,   514,   518,   519,   525,   526,   532,   535,
     537,   539,   541,   543,   547,   549,   551,   555,   557,   559,
     561,   563,   565,   567,   571,   573,   575,   577,   579,   581,
     583,   585,   587,   589,   591
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      87,     0,    -1,    -1,    -1,    88,    90,    89,    96,    -1,
      91,    92,    -1,    -1,     3,    13,    77,    -1,    -1,    92,
      97,    -1,    92,    93,    -1,    -1,    94,    93,    -1,    95,
     115,    -1,    -1,     4,    -1,    -1,    96,   112,    -1,    18,
      98,    77,    -1,    99,    -1,    98,    78,    99,    -1,    -1,
      -1,   100,   103,    51,   102,   101,    -1,   179,    -1,    16,
      -1,    13,    -1,   104,    -1,    14,    -1,    16,    -1,    46,
      -1,    47,    -1,    43,    -1,    44,    -1,    49,    -1,    48,
      -1,    45,    -1,    13,    -1,   106,    13,    -1,   106,    22,
      -1,    61,    -1,     7,    -1,     7,    76,    79,    -1,    21,
      -1,     8,    -1,     9,    -1,    10,    -1,    11,    -1,    -1,
     108,   105,    80,   109,    81,    -1,    -1,   110,    -1,   110,
      78,   111,    -1,   111,    -1,   174,   103,    -1,    95,   107,
     113,    -1,    82,   114,   165,   159,    -1,    -1,   115,   114,
      -1,   174,   103,   121,    77,    -1,   174,   103,    77,    -1,
      77,    -1,   167,    77,    -1,   118,    77,    -1,   160,    -1,
     155,    -1,   152,    -1,   145,    -1,   140,    -1,   141,    -1,
     142,    -1,   148,    -1,   113,    -1,   128,    -1,   147,    -1,
     115,    -1,   117,    77,    -1,     1,    -1,   117,     1,    -1,
     104,   121,    -1,    -1,   104,   119,   120,   122,    -1,    35,
      -1,    36,    -1,    37,    -1,    33,    -1,    34,    -1,    51,
     122,    -1,   123,    -1,    80,   123,    81,    -1,   130,    -1,
     130,    76,   124,    79,    -1,   125,    -1,   130,    -1,   125,
      -1,    -1,    -1,   130,    24,   130,   126,   127,    -1,    -1,
      24,   130,    -1,   104,    26,    -1,   104,    25,    -1,   134,
      -1,   167,    -1,   129,    -1,   173,   129,    -1,    72,    14,
      -1,    66,   130,    -1,    71,   130,    -1,    72,   130,    -1,
     130,    66,   130,    -1,   130,    50,   130,    -1,   130,    63,
     130,    -1,   130,    64,   130,    -1,   130,    71,   130,    -1,
     130,    65,   130,    -1,   130,    72,   130,    -1,   130,    73,
     130,    -1,   130,    74,   130,    -1,   130,    75,   130,    -1,
     130,    54,   130,    -1,   130,    55,   130,    -1,   130,    60,
     130,    -1,   130,    57,   130,    -1,   130,    56,   130,    -1,
     130,    67,   130,    -1,   130,    68,   130,    -1,   130,    58,
     130,    -1,   130,    59,   130,    -1,   130,    53,   130,    -1,
     130,    52,   130,    -1,    80,   130,    81,    -1,    80,   132,
      81,    -1,    80,   130,    81,    -1,    80,   132,    -1,    -1,
     133,    -1,    83,   132,    -1,   167,    -1,   134,    -1,    43,
      -1,    45,    -1,    47,    -1,    44,    -1,    46,    -1,    49,
      -1,    48,    -1,   139,    -1,   179,    -1,   135,    -1,    13,
      -1,    -1,    82,   136,   159,    -1,    -1,    82,   137,   138,
     159,    -1,   138,    78,    16,    -1,    16,    -1,    16,    -1,
     139,    16,    -1,    40,   144,    77,    -1,    41,   144,    77,
      -1,    42,   143,    -1,    -1,   130,    -1,    -1,    14,    -1,
      39,   131,    82,   146,    84,    -1,    -1,   147,   113,   146,
      -1,    13,    85,    -1,    31,    13,    -1,    -1,    32,    13,
     149,    77,    -1,    -1,    80,   174,   103,   151,    85,   123,
      81,    -1,    -1,    -1,     5,   153,   150,   154,   158,    -1,
      -1,    -1,    30,   156,   131,   157,   158,    -1,   116,    -1,
      82,   165,   159,    -1,    -1,    84,    -1,    -1,    -1,    27,
     131,   161,   158,   162,   163,    -1,    -1,    -1,    29,   164,
     158,    -1,    -1,    -1,   116,   166,   165,    -1,    -1,    22,
      80,   168,   109,    81,    -1,    -1,   170,    80,   169,   109,
      81,    -1,    73,   104,    -1,    13,    -1,     1,    -1,   171,
      -1,   172,    -1,   171,    78,   172,    -1,     1,    -1,   123,
      -1,    80,   174,    81,    -1,     9,    -1,     8,    -1,    10,
      -1,    11,    -1,    21,    -1,     7,    -1,     7,    76,    79,
      -1,     6,    -1,    12,    -1,    19,    -1,    20,    -1,    15,
      -1,    14,    -1,    17,    -1,   177,    -1,   176,    -1,   178,
      -1,   175,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   161,   161,   167,   161,   173,   175,   178,   183,   184,
     185,   187,   188,   190,   192,   193,   195,   196,   200,   203,
     204,   206,   206,   206,   218,   221,   227,   228,   229,   236,
     243,   244,   245,   246,   247,   248,   249,   250,   260,   263,
     269,   270,   271,   272,   273,   274,   275,   276,   278,   278,
     283,   284,   287,   289,   291,   298,   321,   323,   324,   326,
     335,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   364,   370,   371,   373,
     378,   378,   387,   388,   389,   390,   391,   393,   395,   396,
     398,   399,   400,   403,   404,   406,   407,   407,   409,   410,
     413,   415,   417,   418,   420,   421,   425,   434,   435,   436,
     437,   438,   439,   440,   441,   442,   443,   444,   445,   446,
     447,   448,   449,   450,   451,   452,   453,   454,   455,   456,
     457,   458,   462,   463,   464,   468,   469,   470,   472,   473,
     475,   479,   483,   487,   491,   495,   499,   503,   508,   509,
     515,   521,   521,   525,   525,   530,   534,   538,   539,   550,
     553,   557,   561,   562,   564,   565,   567,   569,   570,   572,
     577,   581,   581,   586,   586,   604,   606,   604,   617,   619,
     617,   630,   630,   633,   635,   638,   641,   638,   648,   652,
     652,   659,   660,   660,   666,   666,   677,   677,   689,   695,
     699,   705,   707,   708,   709,   713,   715,   717,   718,   719,
     720,   721,   722,   723,   724,   725,   727,   730,   735,   740,
     745,   750,   758,   766,   774
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
  "T_STRINGARRAYVAR", "T_BOOLEANVAR", "T_CHARACTERVAR", "T_NMATCH",
  "T_ASSIGN", "T_NE", "T_EQ", "T_SHIFT_LEFT", "T_SHIFT_RIGHT", "T_OR",
  "T_AND", "T_LE", "T_GE", "T_POWER_TO", "T_VOID", "T_CMDSEQPREFIX", "'&'",
  "'|'", "'^'", "'~'", "'<'", "'>'", "T_GT", "T_LT", "'+'", "'-'", "'*'",
  "'/'", "'%'", "'['", "';'", "','", "']'", "'('", "')'", "'{'", "'!'",
  "'}'", "':'", "$accept", "file_structure", "$@1", "$@2",
  "header_declarations", "namespace", "global_declarations", "global_vars",
  "global_var", "scope", "macro_definitions", "constants", "constant_list",
  "constdef", "$@3", "$@4", "constant_literal", "variable_identifier",
  "variable_reference", "macro_declaration", "macro_type", "macrostart",
  "$@5", "parameter_list", "non_empty_pardecl", "par_decl",
  "macro_definition", "block", "var_decls", "var_decl", "statement",
  "errors", "assignment", "$@6", "shorthand_assignment_operator",
  "assignment_expression", "arithmetic_expression", "simple_expression",
  "binary_expression_or_range", "range_expression", "$@7",
  "range_increment", "increment_expression", "s_bterm",
  "binary_expression", "condition", "logical_expression", "term", "value",
  "array_literal", "$@8", "$@9", "array_elements", "string", "break",
  "continue", "return_expression", "optional_bterm", "opt_num", "case",
  "caselabels", "label", "goto", "$@10", "in_clause", "$@11", "foreach",
  "$@12", "$@13", "while", "$@14", "$@15", "local_block", "closing_brace",
  "if_expression", "$@16", "$@17", "else_clause", "$@18", "stmntlist",
  "$@19", "call_expression", "@20", "@21", "function_id_or_pointer",
  "nonempty_parameters", "parameter", "type_cast", "type_name",
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
     315,   316,   317,    38,   124,    94,   126,    60,    62,   318,
     319,    43,    45,    42,    47,    37,    91,    59,    44,    93,
      40,    41,   123,    33,   125,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    88,    89,    87,    90,    91,    91,    92,    92,
      92,    93,    93,    94,    95,    95,    96,    96,    97,    98,
      98,   100,   101,    99,   102,   102,   103,   103,   103,   103,
     104,   104,   104,   104,   104,   104,   104,   104,   105,   105,
     106,   106,   106,   106,   106,   106,   106,   106,   108,   107,
     109,   109,   110,   110,   111,   112,   113,   114,   114,   115,
     115,   116,   116,   116,   116,   116,   116,   116,   116,   116,
     116,   116,   116,   116,   116,   116,   116,   117,   117,   118,
     119,   118,   120,   120,   120,   120,   120,   121,   122,   122,
     123,   123,   123,   124,   124,   125,   126,   125,   127,   127,
     128,   128,   129,   129,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   131,   131,   131,   132,   132,   132,   133,   133,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   136,   135,   137,   135,   138,   138,   139,   139,   140,
     141,   142,   143,   143,   144,   144,   145,   146,   146,   147,
     148,   149,   148,   151,   150,   153,   154,   152,   156,   157,
     155,   158,   158,   159,   159,   161,   162,   160,   163,   164,
     163,   165,   166,   165,   168,   167,   169,   167,   170,   170,
     170,   109,   171,   171,   171,   172,   173,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   175,   175,   176,   177,
     178,   179,   179,   179,   179
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     4,     2,     0,     3,     0,     2,
       2,     0,     2,     2,     0,     1,     0,     2,     3,     1,
       3,     0,     0,     5,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     1,     3,     1,     1,     1,     1,     1,     0,     5,
       0,     1,     3,     1,     2,     3,     4,     0,     2,     4,
       3,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     2,
       0,     4,     1,     1,     1,     1,     1,     2,     1,     3,
       1,     4,     1,     1,     1,     0,     0,     5,     0,     2,
       2,     2,     1,     1,     1,     2,     2,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     0,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     0,     4,     3,     1,     1,     2,     3,
       3,     2,     0,     1,     0,     1,     5,     0,     3,     2,
       2,     0,     4,     0,     7,     0,     0,     5,     0,     0,
       5,     1,     3,     0,     1,     0,     0,     6,     0,     0,
       3,     0,     0,     3,     0,     5,     0,     5,     2,     1,
       1,     1,     1,     3,     1,     1,     3,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     6,     1,     0,     3,     8,     0,    16,     5,
       7,    14,    15,    21,    10,    11,     0,     9,    48,    17,
       0,    19,     0,    12,   214,   212,   208,   207,   209,   210,
     215,   211,    13,     0,     0,     0,    18,    21,    26,    28,
      29,    32,    33,    36,    30,    31,    35,    34,     0,    27,
       0,     0,    57,    55,    41,    44,    45,    46,    47,    43,
      40,     0,     0,    20,     0,   213,     0,    60,     0,     0,
      57,     0,     0,    38,    39,   219,   218,    25,   220,   216,
     217,    22,   224,   222,   221,   223,    24,   200,   150,   157,
       0,   140,   143,   141,   144,   142,   146,   145,     0,     0,
       0,     0,     0,   151,    87,    88,    92,   104,    90,   102,
     149,   147,   103,     0,     0,   148,    59,    77,   175,    37,
       0,   178,     0,     0,     0,   164,   164,     0,    61,    80,
      72,    75,   192,     0,     0,    73,    68,    69,    70,    67,
      74,    71,    66,    65,    64,   183,     0,    58,    42,   204,
       0,     0,    51,    53,   205,   201,   202,     0,    23,   194,
     107,   108,   106,   109,    37,   198,     0,     0,     0,   183,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   158,   196,   105,     0,   169,     0,
     185,     0,   170,   171,     0,   165,     0,     0,   163,   161,
     101,   100,     0,    79,     0,    78,    76,    63,   184,    56,
      62,     0,    49,     0,     0,    54,     0,    89,   131,   206,
     152,   156,   183,    96,   111,   130,   129,   120,   121,   124,
     123,   127,   128,   122,   112,   113,   115,   110,   125,   126,
     114,   116,   117,   118,   119,     0,    94,    93,     0,     0,
     176,     0,     0,   134,   136,   139,   138,     0,   179,     0,
     167,   159,   160,    85,    86,    82,    83,    84,     0,   193,
      52,   203,     0,     0,   154,    98,    91,     0,     0,     0,
     137,   139,   138,   133,   132,     0,   181,   186,     0,   172,
       0,     0,     0,    81,   195,   155,     0,    97,   197,   173,
     177,    57,   183,   188,   180,   166,   167,    99,     0,   182,
     189,   187,   168,     0,     0,     0,   190,   174
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,     5,     6,     9,    14,    15,    16,
      11,    17,    20,    21,    22,   158,    81,    48,   129,    61,
      62,    34,    35,   151,   152,   153,    19,   130,    69,   131,
     296,   133,   134,   212,   278,    68,   104,   154,   255,   106,
     285,   307,   135,   107,   108,   200,   263,   264,   109,   110,
     169,   170,   232,   111,   136,   137,   138,   209,   206,   139,
     301,   140,   141,   269,   260,   318,   142,   197,   289,   143,
     201,   298,   297,   219,   144,   267,   313,   321,   324,   145,
     214,   112,   226,   258,   113,   155,   156,   114,    33,    82,
      83,    84,    85,   115
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -264
static const yytype_int16 yypact[] =
{
    -264,    42,    41,  -264,    43,  -264,  -264,   -17,  -264,    64,
    -264,    55,  -264,  -264,  -264,    63,   321,  -264,  -264,  -264,
     -49,  -264,   110,  -264,  -264,    -7,  -264,  -264,  -264,  -264,
    -264,  -264,  -264,   110,   -11,    13,  -264,  -264,  -264,  -264,
    -264,  -264,  -264,  -264,  -264,  -264,  -264,  -264,    24,  -264,
      -2,   -32,   321,  -264,     2,  -264,  -264,  -264,  -264,  -264,
    -264,    -1,    18,  -264,   174,  -264,   914,  -264,     3,   362,
     321,    33,   632,  -264,  -264,  -264,  -264,  -264,  -264,  -264,
    -264,  -264,  -264,  -264,  -264,  -264,  -264,  -264,     5,  -264,
      34,  -264,  -264,  -264,  -264,  -264,  -264,  -264,  1034,  1034,
    1107,     4,   710,    67,  -264,  -264,  -264,  -264,  1185,  -264,
    -264,    97,  -264,    39,  1117,  -264,  -264,    40,  -264,   -39,
      45,  -264,   108,   109,    45,   114,   114,   277,  -264,    12,
    -264,  -264,  -264,     9,    52,  -264,  -264,  -264,  -264,  -264,
    -264,  -264,  -264,  -264,  -264,    51,    59,  -264,  -264,    40,
     787,    57,    66,  -264,  -264,    69,  -264,   110,  -264,  -264,
     180,    58,  -264,    58,  -264,  -264,    70,  1148,    81,    51,
     147,  1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,
    1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,  1034,
    1034,  1034,  1034,   954,  -264,  -264,  -264,    87,  -264,   500,
    -264,    45,  -264,  -264,    86,  -264,    99,   105,  1358,  -264,
    -264,  -264,   208,  -264,   362,  -264,  -264,  -264,  -264,  -264,
    -264,  1238,  -264,   321,   126,  -264,   632,  -264,  -264,  -264,
    -264,  -264,   -51,  1358,   180,  1410,  1410,   154,   154,  1384,
    1384,  1358,  1358,     6,  1436,  1436,  1436,   180,   106,   106,
      58,    58,     6,     6,     6,   116,  -264,  1212,   632,   321,
    -264,   583,  1268,   103,  -264,  1298,  1328,   865,  -264,   123,
     172,  -264,  -264,  -264,  -264,  -264,  -264,  -264,   914,  -264,
    -264,  -264,   120,   171,  -264,   179,  -264,   124,   110,   865,
    -264,  -264,  -264,  -264,  -264,   420,  -264,  -264,   865,  -264,
     125,   127,   -11,  -264,  -264,  -264,  1034,  -264,  -264,  -264,
    -264,   321,    51,   187,  -264,  -264,   172,  1358,   132,  -264,
    -264,  -264,  -264,   994,   865,   128,  -264,  -264
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -264,  -264,  -264,  -264,  -264,  -264,  -264,   205,  -264,   210,
    -264,  -264,  -264,   186,  -264,  -264,  -264,   -27,   -20,  -264,
    -264,  -264,  -264,  -201,  -264,    14,  -264,   -33,   -62,   -16,
     -64,  -264,  -264,  -264,  -264,   107,   -46,   -63,  -264,    53,
    -264,  -264,  -264,   136,   -84,  -117,    -5,  -264,  -188,  -264,
    -264,  -264,  -264,  -264,  -264,  -264,  -264,  -264,   131,  -264,
     -58,  -258,  -264,  -264,  -264,  -264,  -264,  -264,  -264,  -264,
    -264,  -264,  -263,  -160,  -264,  -264,  -264,  -264,  -264,  -184,
    -264,   -65,  -264,  -264,  -264,  -264,    38,  -264,   -40,  -264,
    -264,  -264,  -264,   199
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -201
static const yytype_int16 yytable[] =
{
      32,    53,    49,   105,   146,   132,    51,   204,   147,   230,
     215,   265,   302,    49,   160,   161,   163,   164,   167,    66,
      54,    55,    56,    57,    58,   282,   310,   283,    36,    37,
     279,    73,   157,   218,    59,   314,    70,   210,   211,   166,
      74,  -199,     3,   208,     4,    67,   198,    41,    42,    43,
      44,    45,    46,    47,    70,    -4,     7,   287,   302,    12,
      10,   326,   168,    66,   179,   180,   221,    12,    12,    50,
     -11,    52,   284,   291,    60,    64,   -11,    65,    71,    72,
     116,   165,    13,  -153,   268,  -199,   216,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   257,
     168,   312,   148,   194,   159,   262,   179,   180,   181,   195,
    -200,   202,   203,    38,    39,   199,    40,    87,   205,   217,
     225,   190,   191,   192,   266,   218,   220,    49,   222,    88,
      75,    76,    89,    78,   223,    79,    80,   224,    90,   146,
     132,   227,   319,    41,    42,    43,    44,    45,    46,    47,
     175,   176,   229,   231,   179,   180,   181,   259,   270,    91,
      92,    93,    94,    95,    96,    97,   271,   188,   189,   190,
     191,   192,   272,   157,   294,   300,   157,   305,    75,    76,
      77,    78,    98,    79,    80,   286,   292,    99,   100,   101,
     299,   304,   146,   306,   -95,   308,   150,   -95,   103,   327,
     198,   315,   179,   180,   181,   105,   320,   323,   157,   288,
      23,    18,   317,    63,   146,   188,   189,   190,   191,   192,
     146,   132,   303,   146,   175,   176,   213,   280,   179,   180,
     181,   273,   274,   275,   276,   277,   256,   186,   187,   147,
     196,   188,   189,   190,   191,   192,   290,   207,   322,   146,
     325,   309,   281,    86,     0,     0,     0,     0,    49,   316,
       0,     0,     0,     0,     0,     0,     0,  -162,    87,   311,
       0,  -162,  -162,  -162,  -162,  -162,  -162,  -162,  -162,  -162,
      88,    75,    76,    89,    78,    70,    79,    80,  -162,    90,
       0,     0,     0,     0,  -162,     0,  -162,  -162,  -162,  -162,
       0,     0,     0,     0,     0,     0,  -162,  -162,  -162,  -162,
      91,    92,    93,    94,    95,    96,    97,    24,    25,    26,
      27,    28,    29,    30,     0,     0,     0,     0,  -162,     0,
       0,     0,    31,    98,     0,     0,     0,     0,    99,   100,
     101,     0,     0,     0,  -162,     0,     0,   150,     0,   103,
       0,  -162,  -191,   117,     0,     0,  -191,   118,    24,    25,
      26,    27,    28,    29,    30,   119,     0,     0,     0,     0,
       0,     0,     0,    31,    90,     0,     0,     0,     0,   120,
       0,  -191,   121,   122,   123,     0,     0,     0,     0,     0,
       0,   124,   125,   126,   127,    41,    42,    43,    44,    45,
      46,    47,     0,     0,     0,     0,     0,     0,     0,     0,
     -57,   117,     0,  -191,   -57,   118,    24,    25,    26,    27,
      28,    29,    30,   119,     0,   101,     0,     0,     0,   128,
       0,    31,    90,     0,    52,     0,  -191,   120,     0,   -57,
     121,   122,   123,     0,     0,     0,     0,     0,     0,   124,
     125,   126,   127,    41,    42,    43,    44,    45,    46,    47,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -57,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   101,     0,     0,     0,   128,     0,     0,
       0,    87,    52,     0,   -57,  -135,  -135,  -135,  -135,  -135,
    -135,  -135,  -135,    88,    75,    76,    89,    78,     0,    79,
      80,  -135,    90,     0,     0,     0,     0,  -135,     0,     0,
    -135,  -135,  -135,     0,     0,     0,     0,     0,     0,  -135,
    -135,  -135,  -135,    91,    92,    93,    94,    95,    96,    97,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    98,     0,     0,     0,
       0,    99,   100,   101,     0,     0,     0,  -135,     0,     0,
     150,  -135,   103,   261,    87,     0,     0,     0,  -135,  -135,
    -135,  -135,  -135,  -135,  -135,  -135,    88,    75,    76,    89,
      78,     0,    79,    80,  -135,    90,     0,     0,     0,     0,
    -135,     0,     0,  -135,  -135,  -135,     0,     0,     0,     0,
       0,     0,  -135,  -135,  -135,  -135,    91,    92,    93,    94,
      95,    96,    97,   149,     0,     0,     0,     0,    24,    25,
      26,    27,    28,    29,    30,    88,    75,    76,    89,    78,
       0,    79,    80,    31,    90,     0,   101,     0,     0,     0,
    -135,     0,     0,     0,  -135,   103,   261,     0,     0,     0,
       0,     0,     0,     0,     0,    91,    92,    93,    94,    95,
      96,    97,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    98,     0,
       0,     0,     0,    99,   100,   101,     0,     0,     0,     0,
     -95,    87,   150,   -50,   103,     0,    24,    25,    26,    27,
      28,    29,    30,    88,    75,    76,    89,    78,     0,    79,
      80,    31,    90,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    91,    92,    93,    94,    95,    96,    97,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    98,     0,     0,     0,
       0,    99,   100,   101,     0,     0,     0,     0,    87,     0,
     150,   -95,   103,    24,    25,    26,    27,    28,    29,    30,
      88,    75,    76,    89,    78,     0,    79,    80,    31,    90,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      91,    92,    93,    94,    95,    96,    97,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    98,     0,     0,     0,     0,    99,   100,
     101,     0,     0,     0,     0,     0,   117,   150,     0,   103,
     118,    24,    25,    26,    27,    28,    29,    30,   119,     0,
       0,     0,     0,     0,     0,     0,    31,    90,     0,     0,
       0,     0,   120,     0,     0,   121,   122,   123,     0,     0,
       0,     0,     0,     0,   124,   125,   126,   127,    41,    42,
      43,    44,    45,    46,    47,    87,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    88,    75,    76,
      89,    78,     0,    79,    80,     0,    90,     0,   101,     0,
       0,     0,   128,     0,     0,     0,     0,   295,     0,     0,
       0,     0,     0,     0,     0,    87,     0,    91,    92,    93,
      94,    95,    96,    97,     0,     0,     0,    88,    75,    76,
      89,    78,     0,    79,    80,     0,    90,     0,     0,     0,
      98,     0,     0,     0,     0,    99,   100,   101,     0,     0,
       0,   -95,     0,     0,   102,    87,   103,    91,    92,    93,
      94,    95,    96,    97,     0,     0,     0,    88,    75,    76,
      89,    78,     0,    79,    80,     0,    90,     0,     0,     0,
      98,     0,     0,     0,     0,    99,   100,   101,     0,     0,
       0,     0,     0,   -95,   150,    87,   103,    91,    92,    93,
      94,    95,    96,    97,     0,     0,     0,    88,    75,    76,
      89,    78,     0,    79,    80,     0,    90,     0,     0,     0,
      98,     0,     0,     0,     0,    99,   100,   101,     0,     0,
       0,     0,     0,     0,   150,   -95,   103,    91,    92,    93,
      94,    95,    96,    97,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      98,     0,     0,     0,     0,    99,   100,   101,    87,     0,
       0,     0,     0,     0,   150,     0,   103,     0,    87,     0,
      88,   162,    76,    89,    78,     0,    79,    80,     0,    90,
      88,    75,    76,    89,    78,     0,    79,    80,     0,    90,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      91,    92,    93,    94,    95,    96,    97,     0,     0,     0,
      91,    92,    93,    94,    95,    96,    97,     0,     0,     0,
       0,     0,   171,    98,     0,     0,     0,     0,    99,   100,
     101,     0,     0,     0,     0,     0,     0,   150,     0,   103,
     101,     0,     0,     0,     0,     0,     0,     0,   172,   103,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   171,
       0,   182,   183,   184,   185,   186,   187,     0,     0,   188,
     189,   190,   191,   192,   193,     0,     0,     0,     0,   228,
       0,     0,     0,     0,     0,   172,   171,   173,   174,   175,
     176,   177,   178,   179,   180,   181,     0,     0,   182,   183,
     184,   185,   186,   187,     0,     0,   188,   189,   190,   191,
     192,   193,   172,     0,   173,   174,   175,   176,   177,   178,
     179,   180,   181,     0,     0,   182,   183,   184,   185,   186,
     187,     0,     0,   188,   189,   190,   191,   192,   172,     0,
     173,   174,   175,   176,   177,   178,   179,   180,   181,     0,
       0,   182,   183,   184,   185,   186,   187,     0,     0,   188,
     189,   190,   191,   192,     0,     0,     0,     0,   172,   228,
     173,   174,   175,   176,   177,   178,   179,   180,   181,     0,
       0,   182,   183,   184,   185,   186,   187,     0,     0,   188,
     189,   190,   191,   192,     0,     0,     0,     0,  -102,   293,
    -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,     0,
       0,  -102,  -102,  -102,  -102,  -102,  -102,     0,     0,  -102,
    -102,  -102,  -102,  -102,     0,     0,     0,     0,  -103,  -102,
    -103,  -103,  -103,  -103,  -103,  -103,  -103,  -103,  -103,     0,
       0,  -103,  -103,  -103,  -103,  -103,  -103,     0,     0,  -103,
    -103,  -103,  -103,  -103,     0,     0,     0,     0,   172,  -103,
     173,   174,   175,   176,   177,   178,   179,   180,   181,     0,
       0,   182,   183,   184,   185,   186,   187,     0,     0,   188,
     189,   190,   191,   192,   172,     0,   173,   174,   175,   176,
       0,     0,   179,   180,   181,     0,     0,   182,   183,   184,
     185,   186,   187,     0,     0,   188,   189,   190,   191,   192,
     172,     0,  -201,  -201,   175,   176,     0,     0,   179,   180,
     181,     0,     0,     0,     0,     0,   185,   186,   187,     0,
       0,   188,   189,   190,   191,   192,   172,     0,   173,   174,
     175,   176,     0,     0,   179,   180,   181,     0,     0,     0,
       0,     0,   185,   186,   187,     0,     0,   188,   189,   190,
     191,   192
};

static const yytype_int16 yycheck[] =
{
      16,    34,    22,    66,    69,    69,    33,   124,    70,   169,
       1,   199,   270,    33,    98,    99,   100,    13,   102,    51,
       7,     8,     9,    10,    11,   226,   289,    78,    77,    78,
     214,    13,    72,    84,    21,   298,    52,    25,    26,   102,
      22,    80,     0,   127,     3,    77,    85,    43,    44,    45,
      46,    47,    48,    49,    70,     0,    13,   258,   316,     4,
      77,   324,   102,    51,    58,    59,   150,     4,     4,    76,
       6,    82,   232,   261,    61,    51,    12,    79,    76,    80,
      77,   101,    18,    16,   201,    80,    77,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     150,   295,    79,    16,    80,   199,    58,    59,    60,    80,
      80,    13,    13,    13,    14,    80,    16,     1,    14,    77,
     157,    73,    74,    75,   199,    84,    77,   157,    81,    13,
      14,    15,    16,    17,    78,    19,    20,    78,    22,   214,
     214,    81,   312,    43,    44,    45,    46,    47,    48,    49,
      54,    55,    81,    16,    58,    59,    60,    80,    82,    43,
      44,    45,    46,    47,    48,    49,    77,    71,    72,    73,
      74,    75,    77,   223,    81,    13,   226,    16,    14,    15,
      16,    17,    66,    19,    20,    79,   261,    71,    72,    73,
      77,    81,   267,    24,    78,    81,    80,    81,    82,    81,
      85,    84,    58,    59,    60,   278,    29,    85,   258,   259,
      15,    11,   306,    37,   289,    71,    72,    73,    74,    75,
     295,   295,   278,   298,    54,    55,   129,   223,    58,    59,
      60,    33,    34,    35,    36,    37,   193,    67,    68,   311,
     114,    71,    72,    73,    74,    75,   261,   126,   316,   324,
     323,   288,   224,    64,    -1,    -1,    -1,    -1,   288,   302,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,     1,   295,
      -1,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,   311,    19,    20,    21,    22,
      -1,    -1,    -1,    -1,    27,    -1,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,     6,     7,     8,
       9,    10,    11,    12,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    -1,    21,    66,    -1,    -1,    -1,    -1,    71,    72,
      73,    -1,    -1,    -1,    77,    -1,    -1,    80,    -1,    82,
      -1,    84,     0,     1,    -1,    -1,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    21,    22,    -1,    -1,    -1,    -1,    27,
      -1,    29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       0,     1,    -1,    61,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    -1,    73,    -1,    -1,    -1,    77,
      -1,    21,    22,    -1,    82,    -1,    84,    27,    -1,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    73,    -1,    -1,    -1,    77,    -1,    -1,
      -1,     1,    82,    -1,    84,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    -1,    19,
      20,    21,    22,    -1,    -1,    -1,    -1,    27,    -1,    -1,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,
      -1,    71,    72,    73,    -1,    -1,    -1,    77,    -1,    -1,
      80,    81,    82,    83,     1,    -1,    -1,    -1,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    -1,    19,    20,    21,    22,    -1,    -1,    -1,    -1,
      27,    -1,    -1,    30,    31,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,     1,    -1,    -1,    -1,    -1,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      -1,    19,    20,    21,    22,    -1,    73,    -1,    -1,    -1,
      77,    -1,    -1,    -1,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,
      -1,    -1,    -1,    71,    72,    73,    -1,    -1,    -1,    -1,
      78,     1,    80,    81,    82,    -1,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    -1,    19,
      20,    21,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,    -1,
      -1,    71,    72,    73,    -1,    -1,    -1,    -1,     1,    -1,
      80,    81,    82,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    -1,    19,    20,    21,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    -1,    -1,    -1,    -1,    71,    72,
      73,    -1,    -1,    -1,    -1,    -1,     1,    80,    -1,    82,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    21,    22,    -1,    -1,
      -1,    -1,    27,    -1,    -1,    30,    31,    32,    -1,    -1,
      -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,     1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    14,    15,
      16,    17,    -1,    19,    20,    -1,    22,    -1,    73,    -1,
      -1,    -1,    77,    -1,    -1,    -1,    -1,    82,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    13,    14,    15,
      16,    17,    -1,    19,    20,    -1,    22,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      -1,    77,    -1,    -1,    80,     1,    82,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    13,    14,    15,
      16,    17,    -1,    19,    20,    -1,    22,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      -1,    -1,    -1,    79,    80,     1,    82,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    13,    14,    15,
      16,    17,    -1,    19,    20,    -1,    22,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      -1,    -1,    -1,    -1,    80,    81,    82,    43,    44,    45,
      46,    47,    48,    49,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    -1,    -1,    -1,    -1,    71,    72,    73,     1,    -1,
      -1,    -1,    -1,    -1,    80,    -1,    82,    -1,     1,    -1,
      13,    14,    15,    16,    17,    -1,    19,    20,    -1,    22,
      13,    14,    15,    16,    17,    -1,    19,    20,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    48,    49,    -1,    -1,    -1,
      43,    44,    45,    46,    47,    48,    49,    -1,    -1,    -1,
      -1,    -1,    24,    66,    -1,    -1,    -1,    -1,    71,    72,
      73,    -1,    -1,    -1,    -1,    -1,    -1,    80,    -1,    82,
      73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    82,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    24,
      -1,    63,    64,    65,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    75,    76,    -1,    -1,    -1,    -1,    81,
      -1,    -1,    -1,    -1,    -1,    50,    24,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    -1,    -1,    63,    64,
      65,    66,    67,    68,    -1,    -1,    71,    72,    73,    74,
      75,    76,    50,    -1,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    -1,    -1,    63,    64,    65,    66,    67,
      68,    -1,    -1,    71,    72,    73,    74,    75,    50,    -1,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    63,    64,    65,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    75,    -1,    -1,    -1,    -1,    50,    81,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    63,    64,    65,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    75,    -1,    -1,    -1,    -1,    50,    81,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    63,    64,    65,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    75,    -1,    -1,    -1,    -1,    50,    81,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    63,    64,    65,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    75,    -1,    -1,    -1,    -1,    50,    81,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    -1,
      -1,    63,    64,    65,    66,    67,    68,    -1,    -1,    71,
      72,    73,    74,    75,    50,    -1,    52,    53,    54,    55,
      -1,    -1,    58,    59,    60,    -1,    -1,    63,    64,    65,
      66,    67,    68,    -1,    -1,    71,    72,    73,    74,    75,
      50,    -1,    52,    53,    54,    55,    -1,    -1,    58,    59,
      60,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    -1,
      -1,    71,    72,    73,    74,    75,    50,    -1,    52,    53,
      54,    55,    -1,    -1,    58,    59,    60,    -1,    -1,    -1,
      -1,    -1,    66,    67,    68,    -1,    -1,    71,    72,    73,
      74,    75
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    87,    88,     0,     3,    90,    91,    13,    89,    92,
      77,    96,     4,    18,    93,    94,    95,    97,    95,   112,
      98,    99,   100,    93,     6,     7,     8,     9,    10,    11,
      12,    21,   115,   174,   107,   108,    77,    78,    13,    14,
      16,    43,    44,    45,    46,    47,    48,    49,   103,   104,
      76,   103,    82,   113,     7,     8,     9,    10,    11,    21,
      61,   105,   106,    99,    51,    79,    51,    77,   121,   114,
     115,    76,    80,    13,    22,    14,    15,    16,    17,    19,
      20,   102,   175,   176,   177,   178,   179,     1,    13,    16,
      22,    43,    44,    45,    46,    47,    48,    49,    66,    71,
      72,    73,    80,    82,   122,   123,   125,   129,   130,   134,
     135,   139,   167,   170,   173,   179,    77,     1,     5,    13,
      27,    30,    31,    32,    39,    40,    41,    42,    77,   104,
     113,   115,   116,   117,   118,   128,   140,   141,   142,   145,
     147,   148,   152,   155,   160,   165,   167,   114,    79,     1,
      80,   109,   110,   111,   123,   171,   172,   174,   101,    80,
     130,   130,    14,   130,    13,   104,   123,   130,   174,   136,
     137,    24,    50,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    63,    64,    65,    66,    67,    68,    71,    72,
      73,    74,    75,    76,    16,    80,   129,   153,    85,    80,
     131,   156,    13,    13,   131,    14,   144,   144,   130,   143,
      25,    26,   119,   121,   166,     1,    77,    77,    84,   159,
      77,   130,    81,    78,    78,   103,   168,    81,    81,    81,
     159,    16,   138,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   130,   130,   130,   124,   125,   130,   169,    80,
     150,    83,   130,   132,   133,   134,   167,   161,   131,   149,
      82,    77,    77,    33,    34,    35,    36,    37,   120,   165,
     111,   172,   109,    78,   159,   126,    79,   109,   174,   154,
     132,   134,   167,    81,    81,    82,   116,   158,   157,    77,
      13,   146,   147,   122,    81,    16,    24,   127,    81,   103,
     158,   115,   165,   162,   158,    84,   113,   130,   151,   159,
      29,   163,   146,    85,   164,   123,   158,    81
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
#line 161 "Parser.y"
    {
#ifdef YYDEBUG
int yydebug = 1; 
#endif
				init_header();
			;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 167 "Parser.y"
    {
				finalize_header();
			;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 175 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace(MACRO_NAMESPACE_DEFAULT);
			;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 178 "Parser.y"
    {
				_currentNamespaceIndex = macro_lookupNamespace((yyvsp[(2) - (3)]).ident.s);
				free((yyvsp[(2) - (3)]).ident.s);
			;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 192 "Parser.y"
    { _macroScope = MS_GLOBAL; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 193 "Parser.y"
    { _macroScope = MS_LOCAL; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 206 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 206 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 207 "Parser.y"
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
#line 218 "Parser.y"
    { 
				(yyval).v = (yyvsp[(1) - (1)]).v; 
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 221 "Parser.y"
    {
				(yyval).v.data.string = (yyvsp[(1) - (1)]).ident.s;
				(yyval).v.type = S_STRING;
			;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 227 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 228 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 230 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
				(yyval).ident.s = "#dummy";
				(yyval).ident.stringIsAlloced = 0;
				yyerror("Cannot redefine constant / cannot use number as variable identifier.");
			;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 237 "Parser.y"
    {
				(yyval).ident = (yyvsp[(1) - (1)]).ident; 
				yyerror("Identifier cannot be used to define variable (redefinition of constant?).");
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 243 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 244 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 245 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 246 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 247 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 248 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 249 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 250 "Parser.y"
    {   
				yyerror("Using undeclared variable %s", (yyvsp[(1) - (1)]).ident.s);
				// auto-correct by introducing variable
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(1) - (1)]).ident.s, S_NUMBER, (GENERIC_DATA) {0});
				free((yyvsp[(1) - (1)]).ident.s);
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
			;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 260 "Parser.y"
    {
				(yyval).ident = (yyvsp[(2) - (2)]).ident;
			;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 263 "Parser.y"
    {
				yyerror("Illegal attempt to redefine native method");
				(yyval).ident.s = 0;
				(yyval).ident.stringIsAlloced = 0;
			;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 278 "Parser.y"
    { init_macroDefinition(); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 279 "Parser.y"
    {	(yyval).ident = (yyvsp[(2) - (5)]).ident; 
				YY_EMIT(C_SET_STACKFRAME,(GENERIC_DATA){0});
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 291 "Parser.y"
    {
				sym_makeInternalSymbol(_currentIdentifierContext, (yyvsp[(2) - (2)]).ident.s, S_NUMBER, (GENERIC_DATA) {_nparam});
				bytecode_defineVariable(&_bytecodeBuffer, (yyvsp[(2) - (2)]).ident.s,C_DEFINE_PARAMETER,(yyvsp[(1) - (2)]).type,_nparam);
				free((yyvsp[(2) - (2)]).ident.s);
				_nparam++;
			;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 298 "Parser.y"
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

  case 59:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
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

  case 60:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
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

  case 62:

/* Line 1455 of yacc.c  */
#line 348 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 349 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 350 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 351 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 352 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 353 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 354 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 355 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 356 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 357 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 358 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 359 "Parser.y"
    { (yyval).needsPop = 1; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 361 "Parser.y"
    {
				yyerror("Variable declarations outside method variable declaration section not yet supported.");
			;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 364 "Parser.y"
    {
				(yyval).needsPop = 0; 
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, increment_expression, block or label.");
				yyerrok;
			;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 373 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 378 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
			;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 380 "Parser.y"
    {
				YY_EMIT(C_BINOP, (GENERIC_DATA){(yyvsp[(3) - (4)]).type});
				_bytecodeBuffer.bb_current = bytecode_emitAssignment(&_bytecodeBuffer, (yyvsp[(1) - (4)]).ident.s);
				vname_count = 0;
			;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 387 "Parser.y"
    { (yyval).type = BIN_MUL; ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 388 "Parser.y"
    { (yyval).type = BIN_DIV; ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 389 "Parser.y"
    { (yyval).type = BIN_MOD; ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 390 "Parser.y"
    { (yyval).type = BIN_ADD; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 391 "Parser.y"
    { (yyval).type = BIN_SUB; ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 393 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 396 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 398 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AT}); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 407 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 410 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_RANGE}); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 413 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, -1); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 415 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitIncrementExpression(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s, 1); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 420 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 421 "Parser.y"
    {
				(yyval).type = (yyvsp[(1) - (2)]).type;
				_bytecodeBuffer.bb_current = bytecode_emitBinaryOperation(&_bytecodeBuffer, BIN_CAST, (yyval).type);
			;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
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

  case 107:

/* Line 1455 of yacc.c  */
#line 434 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_NOT}); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 435 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 436 "Parser.y"
    { _bytecodeBuffer.bb_current = bytecode_emitMultiplyWithLiteralExpression(&_bytecodeBuffer, &(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 437 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_MATCH}); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 438 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NMATCH}); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 439 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_AND}); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 440 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_OR}); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 441 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_ADD}); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 442 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_XOR}); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 443 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SUB}); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 444 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MUL}); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 445 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_DIV}); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 446 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_MOD}); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 447 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_LEFT}); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 448 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_SHIFT_RIGHT}); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 449 "Parser.y"
    { YY_EMIT(C_BINOP, (GENERIC_DATA){BIN_POWER}); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 450 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_AND}); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 451 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_OR}); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 452 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LT}); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 453 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GT}); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 454 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_LE}); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 455 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_GE}); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 456 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_EQ}); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 457 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NE}); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 458 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 464 "Parser.y"
    {
				yyerror("Missing closing parenthesis )");
			;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 470 "Parser.y"
    { (yyval).type = S_BOOLEAN; YY_EMIT(C_LOGICAL_OPERATION, (GENERIC_DATA){CT_NOT}); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 472 "Parser.y"
    { (yyval).type = S_NUMBER; 	 	;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 473 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 475 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_NUMBER;
			;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_FLOAT;
			;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 483 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_STRING_ARRAY;
			;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 487 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_RANGE;
			;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 491 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_STRING;
			;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 495 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 499 "Parser.y"
    {
				YY_EMIT(C_PUSH_VARIABLE, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).type = S_BOOLEAN;
			;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 503 "Parser.y"
    {
				YY_EMIT(C_PUSH_STRING_LITERAL, (GENERIC_DATA){.string=(yyvsp[(1) - (1)]).ident.s});
				(yyval).v.type = C_PUSH_STRING_LITERAL;
				free((yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 508 "Parser.y"
    {	(yyval).type = (yyvsp[(1) - (1)]).type;	;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 509 "Parser.y"
    {	
				(yyval).type = S_STRING_ARRAY;
				YY_EMIT(C_PUSH_ARRAY_LITERAL, (GENERIC_DATA){.stringList=_currentArrayLiteral});
				arraylist_destroyStringList(_currentArrayLiteral);
				_currentArrayLiteral = 0;
			;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 515 "Parser.y"
    {
				yyerror("Undefined identifier %s", (yyvsp[(1) - (1)]).ident.s);
				(yyval).v.type = C_PUSH_VARIABLE;  
				(yyval).v.data.string  = (yyvsp[(1) - (1)]).ident.s;
			;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 521 "Parser.y"
    {
				_currentArrayLiteral = arraylist_create(1);
			;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 525 "Parser.y"
    {
				_currentArrayLiteral = arraylist_create(3);
			;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 530 "Parser.y"
    {
				arraylist_add(_currentArrayLiteral, (yyvsp[(3) - (3)]).ident.s);
			;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 534 "Parser.y"
    {
				arraylist_add(_currentArrayLiteral, (yyvsp[(1) - (1)]).ident.s);
			;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 538 "Parser.y"
    {	(yyval).ident = (yyvsp[(1) - (1)]).ident; ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 539 "Parser.y"
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

  case 159:

/* Line 1455 of yacc.c  */
#line 551 "Parser.y"
    {	bytecode_emitGotoInstruction(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 554 "Parser.y"
    {	bytecode_emitGotoInstruction(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 557 "Parser.y"
    {
				YY_EMIT(C_STOP,(GENERIC_DATA){0});
			;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 564 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 565 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 572 "Parser.y"
    {
				bytecode_createBranchLabel(&_bytecodeBuffer, (yyvsp[(1) - (2)]).ident.s);
				freeitem(&(yyvsp[(1) - (2)]).ident.s);
			;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 577 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 581 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitGotoLabelInstruction((yyvsp[(2) - (2)]).ident.s,&_bytecodeBuffer,BRA_IF_TRUE);
				freeitem(&(yyvsp[(2) - (2)]).ident.s);
			;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 586 "Parser.y"
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

  case 174:

/* Line 1455 of yacc.c  */
#line 597 "Parser.y"
    {
				PKS_VALUE 	sym;
				char *	key;
				sym = sym_find(_currentIdentifierContext, "foreach",&key);
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((void*)VALUE(sym))}, 3);
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 604 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 606 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 609 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 617 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(lstartid,_breaklevel);
			;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 619 "Parser.y"
    {
				bytecode_emitGotoInstruction(lendid,_breaklevel,BRA_IF_FALSE);
				_breaklevel++;
			;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 622 "Parser.y"
    {
				_breaklevel--;
				bytecode_emitGotoInstruction(lstartid,_breaklevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(lendid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lstartid,_breaklevel);
				bytecode_destroyAutoLabelNamePrefix(lendid,_breaklevel);
			;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 633 "Parser.y"
    { yyerror("Missing closing brace '}'"); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 638 "Parser.y"
    {
				bytecode_emitGotoInstruction(iffailid,_iflevel,BRA_IF_FALSE);
				_iflevel++;
			;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 641 "Parser.y"
    {
				_iflevel--;
			;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 643 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(ifdoneid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(ifdoneid,_iflevel);
			;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 648 "Parser.y"
    {
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 652 "Parser.y"
    {
				bytecode_emitGotoInstruction(ifdoneid,_iflevel,BRA_ALWAYS);
				bytecode_generateAutoLabelNamePrefix(iffailid,_iflevel);
				bytecode_destroyAutoLabelNamePrefix(iffailid,_iflevel);
			;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 659 "Parser.y"
    { (yyval).needsPop = 0; ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 660 "Parser.y"
    { 
				if ((yyvsp[(1) - (1)]).needsPop) {
					YY_EMIT(C_POP_STACK,(GENERIC_DATA){0}); 
				}
			;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 666 "Parser.y"
    {
				// Native call
				_currentNativeMethodCalled = (yyvsp[(1) - (2)]).funcp;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = (yyvsp[(1) - (2)]).funcp;
			;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 672 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, C_0FUNC,(GENERIC_DATA){function_getIndexOfFunction((yyvsp[(1) - (5)]).funcp)}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				_currentNativeMethodCalled = 0;
			;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 677 "Parser.y"
    {
				_currentNativeMethodCalled = 0;
				_currentFunctionCallParamIndexP++;
				*_currentFunctionCallParamIndexP = 0;
				(yyval).funcp = 0;
			;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 682 "Parser.y"
    {
				_bytecodeBuffer.bb_current = bytecode_emitFunctionCall(&_bytecodeBuffer, _functionCallOp,(GENERIC_DATA){.string = (yyvsp[(1) - (5)]).ident.s}, *_currentFunctionCallParamIndexP);
				_currentFunctionCallParamIndexP--;
				freeitem(&(yyvsp[(1) - (5)]).ident.s);
			;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 689 "Parser.y"
    {
				_functionCallOp = C_MACRO_REF;
				(yyval).ident.s = _strdup((yyvsp[(2) - (2)]).ident.s);
				(yyval).ident.stringIsAlloced = 1;
			;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 695 "Parser.y"
    {
				_functionCallOp = C_MACRO;
				(yyval).ident = (yyvsp[(1) - (1)]).ident;
			;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 699 "Parser.y"
    {
				yyerror("Illegal function call expression");
				yyerrok;
				(yyval).ident.s = 0;
			;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 709 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 713 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; (*_currentFunctionCallParamIndexP)++; ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 715 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 717 "Parser.y"
    {	(yyval).type = S_CHARACTER;	;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 718 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 719 "Parser.y"
    {	(yyval).type = S_NUMBER;	;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 720 "Parser.y"
    {	(yyval).type = S_FLOAT;	;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 721 "Parser.y"
    {	(yyval).type = S_BOOLEAN; ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 722 "Parser.y"
    {	(yyval).type = S_STRING;	;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 723 "Parser.y"
    {	(yyval).type = S_STRING_ARRAY;	;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 724 "Parser.y"
    {	(yyval).type = S_AUTO; ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 725 "Parser.y"
    {	(yyval).type = S_RANGE;	;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 727 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 1;
		;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 730 "Parser.y"
    {
			(yyval).v.type = C_PUSH_BOOLEAN_LITERAL; 
			(yyval).v.data.booleanValue = 0;
		;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 735 "Parser.y"
    {
				(yyval).v.type = C_PUSH_FLOAT_LITERAL; 
				(yyval).v.data.doubleValue = (yyvsp[(1) - (1)]).v.data.doubleValue;
			;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 740 "Parser.y"
    {
				(yyval).v.type = C_PUSH_LONG_LITERAL; 
				(yyval).v.data.longValue  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 745 "Parser.y"
    {
				(yyval).v.type = C_PUSH_CHARACTER_LITERAL; 
				(yyval).v.data.uchar = (char)(yyvsp[(1) - (1)]).num;
			;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 750 "Parser.y"
    { 
				if (!_bDefiningConst) {
					bytecode_emitPushParameterInstruction((yyvsp[(1) - (1)]).v.data.longValue);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_NUMBER;
			;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 758 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_FLOAT_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_FLOAT;
			;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 766 "Parser.y"
    { 
				if (!_bDefiningConst) {
					YY_EMIT(C_PUSH_CHARACTER_LITERAL, (yyvsp[(1) - (1)]).v.data);
				} else {
					(yyval).v = (yyvsp[(1) - (1)]).v;
				}
				(yyval).type = S_CHARACTER;
			;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 774 "Parser.y"
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
#line 3566 "Parser.tab.c"
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
#line 782 "Parser.y"


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


