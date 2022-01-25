
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
#line 39 "Parser.y"


#include <windows.h>
#include <stdio.h>

#include "alloc.h"
#include "edfuncs.h"
#include "scanner.h"
#include "test.h"
#include "pkscc.h"
#include "sym.h"

# define 	YYSTYPE _YYSTYPE
# define 	STRINGTYPE(type)	((type == C_STRING1PAR||type == C_STRINGVAR) ? \
							CT_STRING : CT_NUM)
# define 	ISCONSTINT(t)		(t == C_LONG1PAR|| t == C_INT1PAR || \
						 t == C_CHAR1PAR)
# define	ISVARINT(t)		(t == C_LONGVAR)

# define 	MAXEXPR			12

# define	MODE_OVERRIDE		0
# define	MODE_AUGMENT		1
# define	MODE_REPLACE		2

# define	REC_SPACE			24000

static unsigned char *_recspace;
unsigned char *_recp;
unsigned char *_recpend;
static void		*_lastfuncp;
static int		_override,
				_nparam,
				_breaklevel,
				_parno,
				_iflevel,
				_exprlevel,
				_stringflg,
				_kmod_control;
static COM_TEST	*_exprstack[MAXEXPR];

static 		void endmacro(void);
static 		void startmacro(void);
			void freeval(TYPEDVAL *v);
static 		void freeitem(char **p);
static 		char *gotolabel(char *prefix, int level, int bratyp);
static		int makelabel(char *prefix,int level);
static		void killabel(char *prefix, int level);
static 		void TestOp(void);
static 		void CloseTestOp(unsigned char type);
static 		int SetOverride(char *s);
static 		void BinOp1(unsigned char type);
static unsigned char PushVal(TYPEDVAL *vp);
static void 	PushParam(TYPEDVAL *vp);

static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

int			vname_count;

extern 		int	yyerrflg;
extern 		int	_bDefiningConst;
extern void yyerror(char* s, ...);
unsigned char 	*AddComSeq(unsigned char *sp, unsigned char *spend,
				     	 unsigned char typ, intptr_t par);
int				FuncIdx(void *ep);
int 			IsFormStart(void *ep,int parno);
void 		key_destroytable(char *name);
TYPEDVAL		PushBinop(int opd_typ, TYPEDVAL *v1, 
					TYPEDVAL *v2);
extern int yylex(void );
void 		PushAssign(char *name, int typ, intptr_t val);
void 		PushCreateVariable(char *name, int typ, long val);

char		 	*GotoLabel(char *name, char *sp, char *spend, int typ);
int			MakeLabel(char *name, COM_GOTO *recp);
void 		CloseLabelList(void);
void 		KillLabel(char *name);
char			*LabelAutoName(char *prefix, int num);
int 			IsStringFunc(void *ep);


/* Line 189 of yacc.c  */
#line 178 "Parser.tab.c"

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
     T_IDENT = 262,
     T_NUM = 263,
     T_STRING = 264,
     T_OCTNUM = 265,
     T_CONST = 266,
     T_FUNC = 267,
     T_MACRO = 268,
     T_DOTDOT = 269,
     T_IF = 270,
     T_ELIF = 271,
     T_ELSE = 272,
     T_WHILE = 273,
     T_GOTO = 274,
     T_BRAEQ = 275,
     T_LOCAL = 276,
     T_SWITCH = 277,
     T_BREAK = 278,
     T_CONTINUE = 279,
     T_RETURN = 280,
     T_NUMVAR = 281,
     T_STRINGVAR = 282,
     T_NMATCH = 283,
     T_ASSIGN = 284,
     T_NE = 285,
     T_EQ = 286,
     T_OR = 287,
     T_AND = 288,
     T_DEFMACRO = 289,
     T_CMDSEQPREFIX = 290
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
#line 255 "Parser.tab.c"

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
#define YYLAST   231

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  76
/* YYNRULES -- Number of rules.  */
#define YYNRULES  141
/* YYNRULES -- Number of states.  */
#define YYNSTATES  212

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   290

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    53,     2,     2,     2,    46,    39,     2,
      49,    50,    44,    41,    48,    42,     2,    45,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    54,    47,
      36,     2,    37,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    43,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    51,    40,    52,    38,     2,     2,     2,
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
      35
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    11,    13,    17,    19,
      23,    24,    25,    31,    33,    35,    37,    39,    40,    47,
      48,    50,    52,    56,    59,    63,    64,    66,    71,    72,
      75,    80,    84,    86,    89,    90,    94,    96,    98,   100,
     102,   104,   106,   108,   110,   112,   114,   117,   120,   122,
     126,   128,   131,   133,   134,   137,   139,   142,   146,   150,
     154,   158,   162,   166,   170,   174,   178,   179,   180,   186,
     188,   190,   191,   195,   196,   200,   201,   202,   206,   208,
     210,   211,   212,   217,   218,   221,   223,   225,   227,   229,
     231,   233,   235,   237,   239,   241,   243,   245,   247,   249,
     252,   256,   260,   263,   264,   266,   272,   273,   277,   280,
     283,   286,   287,   288,   294,   295,   296,   303,   304,   307,
     308,   311,   312,   318,   319,   325,   326,   328,   330,   334,
     336,   339,   341,   345,   347,   349,   351,   353,   355,   357,
     359,   361
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      56,     0,    -1,    -1,    -1,    57,    58,    56,    -1,    71,
      -1,    59,    -1,    11,    60,    47,    -1,    61,    -1,    60,
      48,    61,    -1,    -1,    -1,    62,    64,    63,    29,   129,
      -1,     7,    -1,    65,    -1,    27,    -1,    26,    -1,    -1,
      67,    34,     7,    49,    68,    50,    -1,    -1,    69,    -1,
      70,    -1,    70,    48,    69,    -1,   127,    64,    -1,    66,
      72,    73,    -1,    -1,     9,    -1,    51,    74,   119,    52,
      -1,    -1,    75,    74,    -1,   127,    64,    79,    47,    -1,
     127,    64,    47,    -1,    47,    -1,   120,    47,    -1,    -1,
      78,    77,    47,    -1,   115,    -1,   112,    -1,   108,    -1,
     104,    -1,   105,    -1,   106,    -1,   111,    -1,    73,    -1,
     110,    -1,     1,    -1,    65,    79,    -1,    29,    80,    -1,
      81,    -1,    49,    81,    50,    -1,    84,    -1,    38,    80,
      -1,   102,    -1,    -1,    83,   120,    -1,    82,    -1,   126,
      82,    -1,    84,    39,    84,    -1,    84,    40,    84,    -1,
      84,    41,    84,    -1,    84,    43,    84,    -1,    84,    42,
      84,    -1,    84,    44,    84,    -1,    84,    45,    84,    -1,
      84,    46,    84,    -1,    49,    84,    50,    -1,    -1,    -1,
      49,    86,    88,    87,    50,    -1,    90,    -1,    85,    -1,
      -1,    53,    89,    88,    -1,    -1,    91,    95,    92,    -1,
      -1,    -1,    94,    93,    88,    -1,    33,    -1,    32,    -1,
      -1,    -1,    96,   101,    97,    98,    -1,    -1,    99,   101,
      -1,    36,    -1,    37,    -1,    31,    -1,    30,    -1,   100,
      -1,    38,    -1,    28,    -1,   120,    -1,   102,    -1,    26,
      -1,    27,    -1,   103,    -1,   130,    -1,     9,    -1,   103,
       9,    -1,    23,   107,    47,    -1,    24,   107,    47,    -1,
      25,   107,    -1,    -1,     8,    -1,    22,    85,    51,   109,
      52,    -1,    -1,   110,    73,   109,    -1,     7,    54,    -1,
      19,     7,    -1,    20,     7,    -1,    -1,    -1,    18,   113,
      85,   114,    76,    -1,    -1,    -1,    15,    85,   116,    76,
     117,   118,    -1,    -1,    17,    76,    -1,    -1,    76,   119,
      -1,    -1,    12,   121,    49,   123,    50,    -1,    -1,     7,
     122,    49,   123,    50,    -1,    -1,   124,    -1,   125,    -1,
     124,    48,   125,    -1,     1,    -1,   126,   102,    -1,   102,
      -1,    49,   127,    50,    -1,     5,    -1,     4,    -1,     6,
      -1,     3,    -1,     8,    -1,   130,    -1,     9,    -1,   128,
      -1,   130,    40,   128,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   126,   126,   127,   127,   133,   134,   139,   141,   142,
     144,   144,   144,   153,   154,   156,   157,   161,   161,   164,
     165,   167,   168,   170,   179,   195,   196,   198,   200,   201,
     203,   210,   221,   222,   223,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   238,   242,   244,   245,
     247,   248,   250,   251,   251,   263,   264,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   286,   286,   286,   288,
     289,   290,   290,   292,   292,   294,   295,   295,   297,   298,
     300,   301,   300,   304,   311,   319,   320,   321,   322,   323,
     329,   330,   332,   333,   335,   339,   343,   347,   349,   350,
     361,   364,   367,   371,   372,   374,   376,   377,   379,   384,
     388,   393,   395,   393,   406,   409,   406,   419,   420,   422,
     423,   425,   425,   432,   432,   439,   440,   442,   445,   448,
     452,   469,   471,   473,   474,   475,   476,   478,   484,   485,
     490,   491
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_TSTRING", "T_TINT", "T_TCHAR",
  "T_TLONG", "T_IDENT", "T_NUM", "T_STRING", "T_OCTNUM", "T_CONST",
  "T_FUNC", "T_MACRO", "T_DOTDOT", "T_IF", "T_ELIF", "T_ELSE", "T_WHILE",
  "T_GOTO", "T_BRAEQ", "T_LOCAL", "T_SWITCH", "T_BREAK", "T_CONTINUE",
  "T_RETURN", "T_NUMVAR", "T_STRINGVAR", "T_NMATCH", "T_ASSIGN", "T_NE",
  "T_EQ", "T_OR", "T_AND", "T_DEFMACRO", "T_CMDSEQPREFIX", "'<'", "'>'",
  "'~'", "'&'", "'|'", "'+'", "'-'", "'^'", "'*'", "'/'", "'%'", "';'",
  "','", "'('", "')'", "'{'", "'}'", "'!'", "':'", "$accept", "resources",
  "$@1", "resource", "constants", "constdefs", "constdef", "$@2", "$@3",
  "var_ident", "var", "macrostart", "$@4", "par_decls",
  "non_empty_pardecl", "par_decl", "macros", "opt_string", "block",
  "var_decls", "var_decl", "stmnt", "$@5", "assignment", "assgn", "arith",
  "aexpr", "s_bterm", "$@6", "bterm", "cond", "$@7", "$@8", "expr", "$@9",
  "optandorexpr", "$@10", "optandor", "@11", "and_or", "op2", "$@12",
  "$@13", "optterm2", "relop", "strmatchop", "term", "value", "string",
  "break", "continue", "return", "opt_num", "case", "caselabels", "label",
  "gotolabel", "while", "$@14", "$@15", "if", "$@16", "$@17", "elseclause",
  "stmntlist", "fcall", "@18", "@19", "parlist", "nonemptypars", "par",
  "cast", "type_ident", "constval", "constnum_or_string", "constexpr", 0
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
     285,   286,   287,   288,   289,   290,    60,    62,   126,    38,
     124,    43,    45,    94,    42,    47,    37,    59,    44,    40,
      41,   123,   125,    33,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    57,    56,    58,    58,    59,    60,    60,
      62,    63,    61,    64,    64,    65,    65,    67,    66,    68,
      68,    69,    69,    70,    71,    72,    72,    73,    74,    74,
      75,    75,    76,    76,    77,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    78,    79,    80,    80,
      81,    81,    82,    83,    82,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    86,    87,    85,    88,
      88,    89,    88,    91,    90,    92,    93,    92,    94,    94,
      96,    97,    95,    98,    98,    99,    99,    99,    99,    99,
     100,   100,   101,   101,   102,   102,   102,   102,   103,   103,
     104,   105,   106,   107,   107,   108,   109,   109,   110,   111,
     111,   113,   114,   112,   116,   117,   115,   118,   118,   119,
     119,   121,   120,   122,   120,   123,   123,   124,   124,   124,
     125,   125,   126,   127,   127,   127,   127,   128,   129,   129,
     130,   130
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     3,     1,     3,
       0,     0,     5,     1,     1,     1,     1,     0,     6,     0,
       1,     1,     3,     2,     3,     0,     1,     4,     0,     2,
       4,     3,     1,     2,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     3,
       1,     2,     1,     0,     2,     1,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     0,     0,     5,     1,
       1,     0,     3,     0,     3,     0,     0,     3,     1,     1,
       0,     0,     4,     0,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     3,     2,     0,     1,     5,     0,     3,     2,     2,
       2,     0,     0,     5,     0,     0,     6,     0,     2,     0,
       2,     0,     5,     0,     5,     0,     1,     1,     3,     1,
       2,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    17,     1,    10,     3,     6,    25,     0,     5,
       0,     8,     0,     4,    26,     0,     0,     7,    10,    13,
      16,    15,    11,    14,    28,    24,     0,     9,     0,   136,
     134,   133,   135,     0,    28,     0,    19,     0,    45,   123,
     121,     0,   111,     0,     0,     0,   103,   103,   103,    32,
       0,    43,     0,    34,    39,    40,    41,    38,    44,    42,
      37,    36,     0,     0,    29,     0,     0,    20,    21,     0,
     137,   139,   140,    12,   138,   108,     0,     0,    66,   114,
       0,   109,   110,     0,   104,     0,     0,   102,    53,    46,
     120,     0,    27,    33,    31,     0,    18,     0,    23,     0,
       0,     0,    73,     0,   112,   106,   100,   101,    98,    94,
      95,    53,    53,    47,    48,    55,     0,    50,    52,    96,
      53,    97,    35,    30,    22,   141,   129,     0,   131,     0,
     126,   127,     0,     0,    71,    70,    67,    69,    80,   115,
       0,     0,     0,     0,    51,    53,     0,     0,     0,   123,
      54,    53,    53,    53,    53,    53,    53,    53,    53,    99,
      56,   124,     0,   130,   122,    73,     0,    75,     0,   117,
     113,   105,   106,     0,    49,    65,   132,    57,    58,    59,
      61,    60,    62,    63,    64,   128,    72,    68,    79,    78,
      74,    76,    81,    93,    92,     0,   116,   107,    73,    83,
     118,    77,    91,    88,    87,    85,    86,    90,    82,     0,
      89,    84
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    10,    11,    12,    28,    22,
      50,     7,     8,    66,    67,    68,     9,    15,    51,    33,
      34,    52,    91,    53,    89,   113,   114,   115,   116,   117,
     135,   102,   166,   136,   165,   137,   138,   190,   198,   191,
     167,   168,   199,   208,   209,   210,   192,   118,   119,    54,
      55,    56,    85,    57,   142,    58,    59,    60,    80,   140,
      61,   103,   169,   196,    62,    63,    77,    76,   129,   130,
     131,   120,   148,    72,    73,   121
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -156
static const yytype_int16 yypact[] =
{
      12,    24,    17,  -156,  -156,    12,  -156,    53,     2,  -156,
      31,  -156,    57,  -156,  -156,   -11,    58,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,    84,  -156,    23,  -156,    47,  -156,
    -156,  -156,  -156,    34,    84,    57,    84,    90,  -156,    26,
    -156,    33,  -156,    86,    88,    33,   101,   101,   101,  -156,
     104,  -156,    34,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,    85,    99,  -156,   -16,    95,  -156,   100,    57,
    -156,  -156,  -156,  -156,   109,  -156,   102,   106,  -156,  -156,
      33,  -156,  -156,   127,  -156,   103,   142,  -156,   130,  -156,
    -156,   147,  -156,  -156,  -156,   148,  -156,    84,  -156,   180,
      21,    21,   -28,    93,  -156,   189,  -156,  -156,  -156,  -156,
    -156,   130,    98,  -156,  -156,  -156,    27,   163,  -156,   188,
     150,   109,  -156,  -156,  -156,  -156,  -156,    84,  -156,   151,
     164,  -156,   150,   160,  -156,  -156,  -156,  -156,  -156,  -156,
      93,    26,   159,   -11,  -156,   126,   165,   141,   166,  -156,
    -156,    18,    18,    18,    18,    18,    18,    18,    18,  -156,
    -156,  -156,    65,  -156,  -156,   -28,   167,    94,    11,   196,
    -156,  -156,   189,   141,  -156,  -156,  -156,   128,   128,    97,
      97,    97,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,    93,  -156,  -156,   -28,   162,
    -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,  -156,    11,
    -156,  -156
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -156,   209,  -156,  -156,  -156,  -156,   200,  -156,  -156,   -18,
      -3,  -156,  -156,  -156,   122,  -156,  -156,  -156,   -15,   186,
    -156,   -98,  -156,  -156,   156,   111,   112,   105,  -156,     9,
     -30,  -156,  -156,  -155,  -156,  -156,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,  -156,  -156,    14,   -99,  -156,  -156,
    -156,  -156,    75,  -156,    54,   -97,  -156,  -156,  -156,  -156,
    -156,  -156,  -156,  -156,   175,  -113,  -156,  -156,   129,  -156,
      66,   -94,   -20,   132,  -156,   192
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -126
static const yytype_int16 yytable[] =
{
      25,   128,   128,   150,    35,   139,   132,   132,   143,    23,
     186,    79,    -2,    88,    35,    83,    69,    65,   149,    70,
     108,    78,   126,    40,     3,   134,    70,   108,     4,    70,
     108,    94,    23,   163,   149,    38,    16,   109,   110,    40,
      24,    39,   170,   201,   109,   110,    40,   109,   110,    41,
     104,    98,    42,    43,    44,   194,    45,    46,    47,    48,
      20,    21,    14,   128,    19,    26,    23,   145,   132,   193,
     127,  -125,    36,    70,   108,   143,    37,    69,    17,    18,
      75,    49,    78,    20,    21,    24,  -119,    29,    30,    31,
      32,   109,   110,    81,    38,    82,   194,   200,    70,    71,
      39,    29,    30,    31,    32,    40,    70,   108,    41,    84,
     193,    42,    43,    44,   127,    45,    46,    47,    48,    20,
      21,   147,    86,    87,   109,   110,   188,   189,   172,    29,
      30,    31,    32,    88,    70,   108,   111,    92,    70,   108,
      49,   156,   157,   158,    24,    96,    93,   145,    97,    99,
     106,   100,   109,   110,   173,   101,   109,   110,    70,   108,
     177,   178,   179,   180,   181,   182,   183,   184,   111,   153,
     154,   155,   156,   157,   158,   145,   109,   110,   105,   112,
     151,   152,   153,   154,   155,   156,   157,   158,    70,   107,
     202,   175,   203,   204,   122,   123,   141,   159,   205,   206,
     207,   161,   151,   152,   153,   154,   155,   156,   157,   158,
     164,   171,   162,   195,    13,   174,   176,   187,    27,   124,
      64,    95,   144,   211,   146,   160,   197,    90,   185,    74,
     133,   125
};

static const yytype_uint8 yycheck[] =
{
      15,   100,   101,   116,    24,   103,   100,   101,   105,    12,
     165,    41,     0,    29,    34,    45,    36,    35,     7,     8,
       9,    49,     1,    12,     0,    53,     8,     9,    11,     8,
       9,    47,    35,   132,     7,     1,    34,    26,    27,    12,
      51,     7,   140,   198,    26,    27,    12,    26,    27,    15,
      80,    69,    18,    19,    20,   168,    22,    23,    24,    25,
      26,    27,     9,   162,     7,     7,    69,    49,   162,   168,
      49,    50,    49,     8,     9,   172,    29,    97,    47,    48,
      54,    47,    49,    26,    27,    51,    52,     3,     4,     5,
       6,    26,    27,     7,     1,     7,   209,   195,     8,     9,
       7,     3,     4,     5,     6,    12,     8,     9,    15,     8,
     209,    18,    19,    20,    49,    22,    23,    24,    25,    26,
      27,   112,    47,    48,    26,    27,    32,    33,   143,     3,
       4,     5,     6,    29,     8,     9,    38,    52,     8,     9,
      47,    44,    45,    46,    51,    50,    47,    49,    48,    40,
      47,    49,    26,    27,   145,    49,    26,    27,     8,     9,
     151,   152,   153,   154,   155,   156,   157,   158,    38,    41,
      42,    43,    44,    45,    46,    49,    26,    27,    51,    49,
      39,    40,    41,    42,    43,    44,    45,    46,     8,    47,
      28,    50,    30,    31,    47,    47,     7,     9,    36,    37,
      38,    50,    39,    40,    41,    42,    43,    44,    45,    46,
      50,    52,    48,    17,     5,    50,    50,    50,    18,    97,
      34,    65,   111,   209,   112,   120,   172,    52,   162,    37,
     101,    99
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    56,    57,     0,    11,    58,    59,    66,    67,    71,
      60,    61,    62,    56,     9,    72,    34,    47,    48,     7,
      26,    27,    64,    65,    51,    73,     7,    61,    63,     3,
       4,     5,     6,    74,    75,   127,    49,    29,     1,     7,
      12,    15,    18,    19,    20,    22,    23,    24,    25,    47,
      65,    73,    76,    78,   104,   105,   106,   108,   110,   111,
     112,   115,   119,   120,    74,    64,    68,    69,    70,   127,
       8,     9,   128,   129,   130,    54,   122,   121,    49,    85,
     113,     7,     7,    85,     8,   107,   107,   107,    29,    79,
     119,    77,    52,    47,    47,    79,    50,    48,    64,    40,
      49,    49,    86,   116,    85,    51,    47,    47,     9,    26,
      27,    38,    49,    80,    81,    82,    83,    84,   102,   103,
     126,   130,    47,    47,    69,   128,     1,    49,   102,   123,
     124,   125,   126,   123,    53,    85,    88,    90,    91,    76,
     114,     7,   109,   110,    80,    49,    81,    84,   127,     7,
     120,    39,    40,    41,    42,    43,    44,    45,    46,     9,
      82,    50,    48,   102,    50,    89,    87,    95,    96,   117,
      76,    52,    73,    84,    50,    50,    50,    84,    84,    84,
      84,    84,    84,    84,    84,   125,    88,    50,    32,    33,
      92,    94,   101,   102,   120,    17,   118,   109,    93,    97,
      76,    88,    28,    30,    31,    36,    37,    38,    98,    99,
     100,   101
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
#line 127 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 144 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 144 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 146 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (5)]).s,
					((yyvsp[(5) - (5)]).v.type == C_STRING1PAR) ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(5) - (5)]).v.val);
				freeval(&(yyvsp[(5) - (5)]).v);
			;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 153 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 154 "Parser.y"
    {	(yyval).s	= (yyvsp[(1) - (1)]).s; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 156 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 157 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 161 "Parser.y"
    { startmacro(); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 162 "Parser.y"
    { (yyval).s = (yyvsp[(3) - (6)]).s; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 170 "Parser.y"
    {
				int typ;
				
				typ = ((yyvsp[(1) - (2)]).type == C_STRING1PAR) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol((yyvsp[(2) - (2)]).s,typ,_nparam);
				PushCreateVariable((yyvsp[(2) - (2)]).s,typ,_nparam);
				_nparam++;
			;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 180 "Parser.y"
    {
				if (!yyerrflg) {
					makelabel(lreturnid,0);
					killabel(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (3)]).s, -1, 1);
					_recp = AddComSeq(_recp,_recpend,C_STOP,1);
					_compilerConfiguration->cb_showStatus("Defining macro %s",(yyvsp[(1) - (3)]).s);
					_compilerConfiguration->cb_insertNewMacro((yyvsp[(1) - (3)]).s,(yyvsp[(2) - (3)]).s,_recspace,(int)(_recp-_recspace));
				}
				freeitem(&(yyvsp[(2) - (3)]).s);
				freeitem(&(yyvsp[(1) - (3)]).s);
				CloseLabelList();
				endmacro();
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 195 "Parser.y"
    {	(yyval).s = 0; ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 196 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 203 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (4)]).s,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? S_STRING : S_NUMBER,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? (intptr_t)"" : 0);
				PushAssign((yyvsp[(2) - (4)]).s,(yyvsp[(3) - (4)]).v.type,(intptr_t)(yyvsp[(3) - (4)]).v.val);
				freeval(&(yyvsp[(3) - (4)]).v);
				vname_count = 0;
			;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 210 "Parser.y"
    {
				if ((yyvsp[(1) - (3)]).type == C_STRING1PAR) {
					sym_makeInternalSymbol((yyvsp[(2) - (3)]).s, S_STRING, (intptr_t)"");
					PushAssign((yyvsp[(2) - (3)]).s, C_STRING1PAR, (intptr_t)"");
				} else {
					sym_makeInternalSymbol((yyvsp[(2) - (3)]).s, S_NUMBER, (intptr_t)0);
					PushAssign((yyvsp[(2) - (3)]).s,C_LONG1PAR,(intptr_t)0);
				}
				vname_count = 0;
			;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 223 "Parser.y"
    { vname_count = 0; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 233 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, block or label."); 
			;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 238 "Parser.y"
    {
				PushAssign((yyvsp[(1) - (2)]).s,(yyvsp[(2) - (2)]).v.type,(intptr_t)(yyvsp[(2) - (2)]).v.val);
			;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 242 "Parser.y"
    { 	(yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 244 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 245 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 247 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 248 "Parser.y"
    { (yyval).v = PushBinop(BIN_NOT,&(yyvsp[(2) - (2)]).v,0); 	;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 250 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 251 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_FURET,vname_count);
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 253 "Parser.y"
    {
				if ((yyvsp[(2) - (2)]).funcp && IsStringFunc((yyvsp[(2) - (2)]).funcp)) {
					(yyval).v.type = C_STRINGVAR;
				} else {
					(yyval).v.type = C_LONGVAR;
				}
				(yyval).v.val = (intptr_t)"__ret__";
				((char*)(yyval).v.val)[6] = '0' + vname_count++;
			;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 263 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 264 "Parser.y"
    {
				if (macro_isParameterStringType((yyvsp[(1) - (2)]).type) ==
				    macro_isParameterStringType((yyvsp[(2) - (2)]).v.type)) {
					(yyval).v = (yyvsp[(2) - (2)]).v;
				} else {
					(yyval).v = PushBinop(BIN_CONVERT,&(yyvsp[(2) - (2)]).v,0);
					(yyval).v.type = ((yyvsp[(1) - (2)]).type == C_STRING1PAR) ?
						C_STRINGVAR : C_LONGVAR;
				}
			;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 274 "Parser.y"
    { (yyval).v = PushBinop(BIN_AND,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 275 "Parser.y"
    { (yyval).v = PushBinop(BIN_OR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 276 "Parser.y"
    { (yyval).v = PushBinop(BIN_ADD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 277 "Parser.y"
    { (yyval).v = PushBinop(BIN_XOR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 278 "Parser.y"
    { (yyval).v = PushBinop(BIN_SUB,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 279 "Parser.y"
    { (yyval).v = PushBinop(BIN_MUL,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 280 "Parser.y"
    { (yyval).v = PushBinop(BIN_DIV,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 281 "Parser.y"
    { (yyval).v = PushBinop(BIN_MOD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 282 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 286 "Parser.y"
    { TestOp(); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 286 "Parser.y"
    { CloseTestOp(CT_BRACKETS); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 290 "Parser.y"
    { TestOp(); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 290 "Parser.y"
    { CloseTestOp(CT_NOT); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 292 "Parser.y"
    { TestOp(); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 292 "Parser.y"
    { CloseTestOp((yyvsp[(3) - (3)]).type); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 294 "Parser.y"
    { (yyval).type = CT_BRACKETS; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 295 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; BinOp1((yyvsp[(1) - (1)]).type); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 297 "Parser.y"
    {	(yyval).type = CT_AND;	;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 298 "Parser.y"
    {	(yyval).type = CT_OR;	;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 300 "Parser.y"
    { TestOp(); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 301 "Parser.y"
    { BinOp1((yyvsp[(2) - (2)]).type);  ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 302 "Parser.y"
    { CloseTestOp((yyvsp[(4) - (4)]).type); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 304 "Parser.y"
    { 	
				if (_stringflg)
					_recp = AddComSeq(_recp,_recpend,C_STRING1PAR,(intptr_t)"");
				else
					_recp = AddComSeq(_recp,_recpend,C_LONG1PAR,0L);
				(yyval).type = CT_NE;
			;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 311 "Parser.y"
    {
				int flg = STRINGTYPE((yyvsp[(2) - (2)]).type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				int nType = (yyvsp[(1) - (2)]).type;
				(yyval).type = (nType | flg);
			;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 319 "Parser.y"
    {	(yyval).type = CT_LT;	;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 320 "Parser.y"
    {	(yyval).type = CT_GT;	;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 321 "Parser.y"
    {	(yyval).type = CT_EQ;	;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 322 "Parser.y"
    {	(yyval).type = CT_NE;	;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 323 "Parser.y"
    {
				if (_stringflg == CT_NUM)
					yyerror("match operator not allowed on numbers");
				(yyval).type = (yyvsp[(1) - (1)]).type;
			;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 329 "Parser.y"
    {	(yyval).type = CT_MATCH;			;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 330 "Parser.y"
    {	(yyval).type = CT_NMATCH;		;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 332 "Parser.y"
    {	(yyval).type = C_LONG1PAR; 	 	;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 333 "Parser.y"
    {	(yyval).type = PushVal(&(yyvsp[(1) - (1)]).v);	;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
    {
				(yyval).v.type = C_LONGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 339 "Parser.y"
    {
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 343 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 347 "Parser.y"
    {	(yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 349 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s;	;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 350 "Parser.y"
    { 
				if (((yyval).s = malloc(strlen((yyvsp[(1) - (2)]).s)+strlen((yyvsp[(2) - (2)]).s)+1)) != 0) {
					strcat(strcpy((yyval).s,(yyvsp[(1) - (2)]).s),(yyvsp[(2) - (2)]).s);
					free((yyvsp[(1) - (2)]).s);
					free((yyvsp[(2) - (2)]).s);
				} else {
					yyerror("buffer overlow");
					(yyval).s = (yyvsp[(2) - (2)]).s;
				}
			;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 362 "Parser.y"
    {	gotolabel(lendid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 365 "Parser.y"
    {	gotolabel(lstartid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 367 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_STOP,(yyvsp[(2) - (2)]).num);
			;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 371 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 372 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 379 "Parser.y"
    {
				MakeLabel((yyvsp[(1) - (2)]).s,(COM_GOTO *)_recp);
				freeitem(&(yyvsp[(1) - (2)]).s);
			;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 384 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 388 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 393 "Parser.y"
    {
				makelabel(lstartid,_breaklevel);
			;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 395 "Parser.y"
    {
				gotolabel(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 398 "Parser.y"
    {
				_breaklevel--;
				gotolabel(lstartid,_breaklevel,BRA_ALWAYS);
				makelabel(lendid,_breaklevel);
				killabel(lstartid,_breaklevel);
				killabel(lendid,_breaklevel);
			;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 406 "Parser.y"
    {
				gotolabel(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 409 "Parser.y"
    {
				_iflevel--;
				gotolabel(ifdoneid,_iflevel,BRA_ALWAYS);
				makelabel(iffailid,_iflevel);
				killabel(iffailid,_iflevel);
			;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 414 "Parser.y"
    {
				makelabel(ifdoneid,_iflevel);
				killabel(ifdoneid,_iflevel);
			;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 425 "Parser.y"
    {
				_lastfuncp = (yyvsp[(1) - (1)]).funcp;
				_parno = 0;
				_recp = AddComSeq(_recp,_recpend,C_0FUNC,
							   FuncIdx((yyvsp[(1) - (1)]).funcp));
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 432 "Parser.y"
    {
				_lastfuncp = 0;
				_recp = AddComSeq(_recp,_recpend,C_MACRO,(intptr_t)(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
				(yyval).funcp = 0;
			;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 442 "Parser.y"
    {
				PushParam(&(yyvsp[(1) - (1)]).v);
			;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 445 "Parser.y"
    {
				PushParam(&(yyvsp[(3) - (3)]).v);
			;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 448 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 452 "Parser.y"
    {
				if (ISCONSTINT((yyvsp[(2) - (2)]).v.type)) {
					if ((yyvsp[(1) - (2)]).type == C_CHAR1PAR &&
					    ((yyvsp[(2) - (2)]).v.val < 0 || (yyvsp[(2) - (2)]).v.val > 255))
					    	yyerror("constant out of range");
					if ((yyvsp[(1) - (2)]).type == C_STRING1PAR && (yyvsp[(2) - (2)]).num) {
						yyerror("invalid string cast");
						(yyvsp[(2) - (2)]).v.val = 0;
					}
				} else if (ISVARINT((yyvsp[(2) - (2)]).v.type)) {
					if ((yyvsp[(1) - (2)]).type == C_STRING1PAR) {
						yyerror("unsupported cast");
					}
				}
				(yyval).v.type = (yyvsp[(1) - (2)]).type;
				(yyval).v.val = (yyvsp[(2) - (2)]).num;
			;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 469 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 471 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 473 "Parser.y"
    {	(yyval).type = C_CHAR1PAR;	;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 474 "Parser.y"
    {	(yyval).type = C_INT1PAR;	;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 475 "Parser.y"
    {	(yyval).type = C_LONG1PAR;	;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 476 "Parser.y"
    {	(yyval).type = C_STRING1PAR;	;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 478 "Parser.y"
    {
				(yyval).v.type = C_LONG1PAR; 
				(yyval).v.val  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 484 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 485 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 490 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 491 "Parser.y"
    {
				(yyval).v.type = (yyvsp[(1) - (3)]).v.type;
				(yyval).v.val = (yyvsp[(1) - (3)]).v.val | (yyvsp[(3) - (3)]).v.val;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 2543 "Parser.tab.c"
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
#line 495 "Parser.y"


/*---------------------------------*/
/* BinOp1()					*/
/*---------------------------------*/
static void BinOp1(unsigned char type)
{	COM_TEST *cp = _exprstack[_exprlevel];

	if (cp) {
		_stringflg = STRINGTYPE(type);
		cp->p2offset = (int)(_recp-(unsigned char *)cp);
	}
}

/*---------------------------------*/
/* CloseTestOp()				*/
/*---------------------------------*/
static void CloseTestOp(unsigned char type)
{	COM_TEST *cp = _exprstack[_exprlevel];

	if (_exprlevel <= 0) {
		yyerror("bad expression");
	} else {
		cp->testop = type;
		cp->size = (int)(_recp - (unsigned char *)cp);
		_exprlevel--;
	}
}

/*---------------------------------*/
/* TestOp()					*/
/*---------------------------------*/
static void TestOp()
{
	if (_exprlevel >= MAXEXPR-1) {
		yyerror("expression too complex");
	} else {
		_exprlevel++;
		_exprstack[_exprlevel] = (COM_TEST*)_recp;
		_recp = AddComSeq(_recp,_recpend,C_TEST,CT_BRACKETS);
	}
}

/*---------------------------------*/
/* PushVal()					*/
/*---------------------------------*/
static unsigned char PushVal(TYPEDVAL *vp)
{
	_recp = AddComSeq(_recp,_recpend,vp->type,vp->val);
	freeval(vp);
	return vp->type;
}

/*---------------------------------*/
/* PushParam()					*/
/*---------------------------------*/
static void PushParam(TYPEDVAL *vp)
{
	if (_lastfuncp && IsFormStart(_lastfuncp,_parno))
		vp->type = C_FORMSTART;
	PushVal(vp);
	_parno++;
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
/* gotolabel()					*/
/*---------------------------------*/
static char *gotolabel(char *prefix, int level, int bratyp)
{	char *name = LabelAutoName(prefix,level);

	if (level < 0)
		yyerror("illegal break/continue level");
	else
		_recp = GotoLabel(name,_recp,_recpend,bratyp);
	return _recp;
}

/*---------------------------------*/
/* makelabel()					*/
/*---------------------------------*/
static int makelabel(char *prefix,int level)
{	char *name = LabelAutoName(prefix,level);

	return MakeLabel(name,(COM_GOTO *)_recp);
}

/*---------------------------------*/
/* killabel()					*/
/*---------------------------------*/
static void killabel(char *prefix, int level)
{	char *name = LabelAutoName(prefix,level);

	/*
	 * autolabel is not used any more: kill it
	 */
	KillLabel(name);
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

/*--------------------------------------------------------------------------
 * SetOverride()
 */
static int SetOverride(char *s)
{
	_override = MODE_REPLACE;
	if (!*s || strcmp(s,"replace") == 0) {
		;
	} else if (strcmp(s,"augment") == 0) {
		_override = MODE_AUGMENT;
	} else if (strcmp(s,"override") == 0) {
		_override = MODE_OVERRIDE;
	} else {
		yyerror("invalid override mode %s",s);
	}
	return _override;
}

/*---------------------------------*/
/* freeval()					*/
/*---------------------------------*/
void freeval(TYPEDVAL *v)
{
	if ((v->type == C_STRING1PAR /*||
	    v->type == C_STRINGVAR  ||
	    v->type == C_LONGVAR*/) &&
	    *(char*)v->val) {
		freeitem((char **)(&v->val));
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
	if (!_recspace) {
		_recpend = ((_recspace = malloc(REC_SPACE)) == 0) ? 
			_recspace : _recspace + REC_SPACE;
	}
	_nparam = 0;
	_recp = _recspace;
	_lastfuncp  = 0;
	_breaklevel = 0;
	_iflevel    = 0;
	yyerrflg = 0;
	_exprlevel = 0;
}

/*---------------------------------*/
/* endmacro()					*/
/*---------------------------------*/
static void endmacro(void)
{
	if (_recspace) {
		free(_recspace);
		_recspace = 0;
		_recpend = 0;
	}
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


