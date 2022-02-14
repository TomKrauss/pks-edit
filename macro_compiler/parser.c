
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
extern TYPEDVAL		PushBinop(int opd_typ, TYPEDVAL *v1, 
					TYPEDVAL *v2);
/*
 * Multiply an expression and return the TYPEDVAL result
 */
extern TYPEDVAL MultiplyValue(TYPEDVAL* v1, int nNumber);

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
#line 182 "Parser.tab.c"

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
#line 259 "Parser.tab.c"

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
#define YYLAST   279

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  77
/* YYNRULES -- Number of rules.  */
#define YYNRULES  145
/* YYNRULES -- Number of states.  */
#define YYNSTATES  220

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
     126,   128,   131,   133,   134,   137,   139,   142,   145,   148,
     152,   156,   160,   164,   168,   172,   176,   180,   184,   185,
     186,   192,   194,   196,   197,   201,   202,   206,   207,   208,
     212,   214,   216,   217,   218,   223,   224,   227,   229,   231,
     233,   235,   237,   239,   241,   243,   245,   247,   249,   251,
     253,   255,   258,   262,   266,   269,   270,   272,   278,   279,
     283,   286,   289,   292,   293,   294,   300,   302,   306,   307,
     308,   315,   316,   319,   320,   323,   324,   330,   331,   337,
     338,   340,   342,   346,   348,   351,   353,   357,   359,   361,
     363,   365,   367,   369,   371,   373
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      56,     0,    -1,    -1,    -1,    57,    58,    56,    -1,    71,
      -1,    59,    -1,    11,    60,    47,    -1,    61,    -1,    60,
      48,    61,    -1,    -1,    -1,    62,    64,    63,    29,   130,
      -1,     7,    -1,    65,    -1,    27,    -1,    26,    -1,    -1,
      67,    34,     7,    49,    68,    50,    -1,    -1,    69,    -1,
      70,    -1,    70,    48,    69,    -1,   128,    64,    -1,    66,
      72,    73,    -1,    -1,     9,    -1,    51,    74,   120,    52,
      -1,    -1,    75,    74,    -1,   128,    64,    79,    47,    -1,
     128,    64,    47,    -1,    47,    -1,   121,    47,    -1,    -1,
      78,    77,    47,    -1,   116,    -1,   112,    -1,   108,    -1,
     104,    -1,   105,    -1,   106,    -1,   111,    -1,    73,    -1,
     110,    -1,     1,    -1,    65,    79,    -1,    29,    80,    -1,
      81,    -1,    49,    81,    50,    -1,    84,    -1,    38,    80,
      -1,   102,    -1,    -1,    83,   121,    -1,    82,    -1,   127,
      82,    -1,    41,    84,    -1,    42,    84,    -1,    84,    39,
      84,    -1,    84,    40,    84,    -1,    84,    41,    84,    -1,
      84,    43,    84,    -1,    84,    42,    84,    -1,    84,    44,
      84,    -1,    84,    45,    84,    -1,    84,    46,    84,    -1,
      49,    84,    50,    -1,    -1,    -1,    49,    86,    88,    87,
      50,    -1,    90,    -1,    85,    -1,    -1,    53,    89,    88,
      -1,    -1,    91,    95,    92,    -1,    -1,    -1,    94,    93,
      88,    -1,    33,    -1,    32,    -1,    -1,    -1,    96,   101,
      97,    98,    -1,    -1,    99,   101,    -1,    36,    -1,    37,
      -1,    31,    -1,    30,    -1,   100,    -1,    38,    -1,    28,
      -1,   121,    -1,   102,    -1,    26,    -1,    27,    -1,   103,
      -1,   131,    -1,     9,    -1,   103,     9,    -1,    23,   107,
      47,    -1,    24,   107,    47,    -1,    25,   107,    -1,    -1,
       8,    -1,    22,    85,    51,   109,    52,    -1,    -1,   110,
      73,   109,    -1,     7,    54,    -1,    19,     7,    -1,    20,
       7,    -1,    -1,    -1,    18,   113,    85,   114,   115,    -1,
      76,    -1,    51,   120,    52,    -1,    -1,    -1,    15,    85,
     117,    76,   118,   119,    -1,    -1,    17,    76,    -1,    -1,
      76,   120,    -1,    -1,    12,   122,    49,   124,    50,    -1,
      -1,     7,   123,    49,   124,    50,    -1,    -1,   125,    -1,
     126,    -1,   125,    48,   126,    -1,     1,    -1,   127,   102,
      -1,   102,    -1,    49,   128,    50,    -1,     5,    -1,     4,
      -1,     6,    -1,     3,    -1,     8,    -1,   131,    -1,     9,
      -1,   129,    -1,   131,    40,   129,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   130,   130,   131,   131,   137,   138,   143,   145,   146,
     148,   148,   148,   157,   158,   160,   161,   165,   165,   168,
     169,   171,   172,   174,   183,   199,   200,   202,   204,   205,
     207,   214,   225,   226,   227,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   242,   246,   248,   249,
     251,   252,   254,   255,   255,   267,   268,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   292,   292,
     292,   294,   295,   296,   296,   298,   298,   300,   301,   301,
     303,   304,   306,   307,   306,   310,   317,   325,   326,   327,
     328,   329,   335,   336,   338,   339,   341,   345,   349,   353,
     355,   356,   367,   370,   373,   377,   378,   380,   382,   383,
     385,   390,   394,   399,   401,   399,   412,   412,   414,   417,
     414,   427,   428,   430,   431,   433,   433,   440,   440,   447,
     448,   450,   453,   456,   460,   477,   479,   481,   482,   483,
     484,   486,   492,   493,   498,   499
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
  "gotolabel", "while", "$@14", "$@15", "local_block", "if", "$@16",
  "$@17", "elseclause", "stmntlist", "fcall", "@18", "@19", "parlist",
  "nonemptypars", "par", "cast", "type_ident", "constval",
  "constnum_or_string", "constexpr", 0
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
      84,    84,    84,    84,    84,    84,    84,    84,    86,    87,
      85,    88,    88,    89,    88,    91,    90,    92,    93,    92,
      94,    94,    96,    97,    95,    98,    98,    99,    99,    99,
      99,    99,   100,   100,   101,   101,   102,   102,   102,   102,
     103,   103,   104,   105,   106,   107,   107,   108,   109,   109,
     110,   111,   111,   113,   114,   112,   115,   115,   117,   118,
     116,   119,   119,   120,   120,   122,   121,   123,   121,   124,
     124,   125,   125,   125,   126,   126,   127,   128,   128,   128,
     128,   129,   130,   130,   131,   131
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     3,     1,     3,
       0,     0,     5,     1,     1,     1,     1,     0,     6,     0,
       1,     1,     3,     2,     3,     0,     1,     4,     0,     2,
       4,     3,     1,     2,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     3,
       1,     2,     1,     0,     2,     1,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     0,     0,
       5,     1,     1,     0,     3,     0,     3,     0,     0,     3,
       1,     1,     0,     0,     4,     0,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     3,     3,     2,     0,     1,     5,     0,     3,
       2,     2,     2,     0,     0,     5,     1,     3,     0,     0,
       6,     0,     2,     0,     2,     0,     5,     0,     5,     0,
       1,     1,     3,     1,     2,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    17,     1,    10,     3,     6,    25,     0,     5,
       0,     8,     0,     4,    26,     0,     0,     7,    10,    13,
      16,    15,    11,    14,    28,    24,     0,     9,     0,   140,
     138,   137,   139,     0,    28,     0,    19,     0,    45,   127,
     125,     0,   113,     0,     0,     0,   105,   105,   105,    32,
       0,    43,     0,    34,    39,    40,    41,    38,    44,    42,
      37,    36,     0,     0,    29,     0,     0,    20,    21,     0,
     141,   143,   144,    12,   142,   110,     0,     0,    68,   118,
       0,   111,   112,     0,   106,     0,     0,   104,    53,    46,
     124,     0,    27,    33,    31,     0,    18,     0,    23,     0,
       0,     0,    75,     0,   114,   108,   102,   103,   100,    96,
      97,    53,    53,    53,    53,    47,    48,    55,     0,    50,
      52,    98,    53,    99,    35,    30,    22,   145,   133,     0,
     135,     0,   130,   131,     0,     0,    73,    72,    69,    71,
      82,   119,     0,     0,     0,     0,    51,    53,    57,    58,
       0,     0,     0,   127,    54,    53,    53,    53,    53,    53,
      53,    53,    53,   101,    56,   128,     0,   134,   126,    75,
       0,    77,     0,   121,     0,   116,   115,   107,   108,     0,
      49,    67,   136,    59,    60,    61,    63,    62,    64,    65,
      66,   132,    74,    70,    81,    80,    76,    78,    83,    95,
      94,     0,   120,     0,   109,    75,    85,   122,   117,    79,
      93,    90,    89,    87,    88,    92,    84,     0,    91,    86
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    10,    11,    12,    28,    22,
      50,     7,     8,    66,    67,    68,     9,    15,    51,    33,
      34,    52,    91,    53,    89,   115,   116,   117,   118,   119,
     137,   102,   170,   138,   169,   139,   140,   196,   205,   197,
     171,   172,   206,   216,   217,   218,   198,   120,   121,    54,
      55,    56,    85,    57,   144,    58,    59,    60,    80,   142,
     176,    61,   103,   173,   202,    62,    63,    77,    76,   131,
     132,   133,   122,    35,    72,    73,   123
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -151
static const yytype_int16 yypact[] =
{
      11,    14,    16,  -151,  -151,    11,  -151,     8,   -12,  -151,
      23,  -151,    76,  -151,  -151,   -26,    51,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,   138,  -151,    38,  -151,    31,  -151,
    -151,  -151,  -151,    66,   138,    76,   138,   159,  -151,     9,
    -151,    47,  -151,    91,   102,    47,    92,    92,    92,  -151,
     116,  -151,    66,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,    68,    75,  -151,    65,    97,  -151,   107,    76,
    -151,  -151,  -151,  -151,   129,  -151,   131,   135,  -151,  -151,
      47,  -151,  -151,   134,  -151,   141,   142,  -151,   200,  -151,
    -151,   158,  -151,  -151,  -151,   169,  -151,   138,  -151,   179,
       7,     7,    46,   109,  -151,   210,  -151,  -151,  -151,  -151,
    -151,   200,    12,    12,   173,  -151,  -151,  -151,     6,   189,
    -151,   212,   144,   129,  -151,  -151,  -151,  -151,  -151,   138,
    -151,   186,   175,  -151,   144,   187,  -151,  -151,  -151,  -151,
    -151,  -151,   139,     9,   191,   -26,  -151,   198,    93,    93,
     194,   211,   195,  -151,  -151,    12,    12,    12,    12,    12,
      12,    12,    12,  -151,  -151,  -151,    15,  -151,  -151,    46,
     196,   140,    99,   231,    25,  -151,  -151,  -151,   210,   211,
    -151,  -151,  -151,   221,   221,    93,    93,    93,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,   109,  -151,   206,  -151,    46,   182,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,  -151,    99,  -151,  -151
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -151,   254,  -151,  -151,  -151,  -151,   242,  -151,  -151,    45,
       0,  -151,  -151,  -151,   171,  -151,  -151,  -151,   -15,   235,
    -151,   -96,  -151,  -151,   205,   160,   161,   150,  -151,    36,
      21,  -151,  -151,  -150,  -151,  -151,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,  -151,    56,   -98,  -151,  -151,
    -151,  -151,   127,  -151,    96,   -99,  -151,  -151,  -151,  -151,
    -151,  -151,  -151,  -151,  -151,   -51,  -113,  -151,  -151,   176,
    -151,   110,   -91,   -32,   180,  -151,   241
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -130
static const yytype_int16 yytable[] =
{
      25,    90,   130,   130,    69,   154,   145,   141,   128,   134,
     134,    -2,    23,   153,     3,    70,   108,    14,    40,   192,
      70,   108,    16,    70,   108,    24,    38,     4,    29,    30,
      31,    32,    39,   109,   110,    23,   167,    40,   109,   110,
      41,   109,   110,    42,    43,    44,   175,    45,    46,    47,
      48,    20,    21,   112,   113,   209,   129,  -129,    26,   200,
      37,   147,    79,    75,   129,    69,    83,    38,   130,    23,
      17,    18,    49,    39,   199,   134,    24,   -28,    40,   145,
      65,    41,   152,    19,    42,    43,    44,    36,    45,    46,
      47,    48,    20,    21,    88,    78,    78,   152,    81,   136,
      84,   104,    20,    21,   200,   207,   153,    70,   108,    82,
      38,    40,    94,    49,    98,   152,    39,    24,  -123,   199,
      92,    40,    93,   203,    41,   109,   110,    42,    43,    44,
     178,    45,    46,    47,    48,    20,    21,   160,   161,   162,
      38,    29,    30,    31,    32,    88,    39,    96,   148,   149,
     151,    40,    70,   108,    41,    97,    49,    42,    43,    44,
      24,    45,    46,    47,    48,    20,    21,    70,    71,    99,
     109,   110,   194,   195,    86,    87,    29,    30,    31,    32,
     100,    70,   108,   179,   101,   105,    49,    70,   106,   107,
     174,   183,   184,   185,   186,   187,   188,   189,   190,   109,
     110,    29,    30,    31,    32,   124,    70,   108,    70,   108,
     210,   111,   211,   212,   112,   113,   125,   143,   213,   214,
     215,   163,   147,   166,   109,   110,   109,   110,   155,   156,
     157,   158,   159,   160,   161,   162,   165,   168,   111,   112,
     113,   112,   113,   177,   180,   182,   193,   147,   201,   114,
     155,   156,   157,   158,   159,   160,   161,   162,   208,    13,
      27,   181,   157,   158,   159,   160,   161,   162,   126,    64,
      95,   146,   164,   219,   204,   150,   191,   135,    74,   127
};

static const yytype_uint8 yycheck[] =
{
      15,    52,   100,   101,    36,   118,   105,   103,     1,   100,
     101,     0,    12,     7,     0,     8,     9,     9,    12,   169,
       8,     9,    34,     8,     9,    51,     1,    11,     3,     4,
       5,     6,     7,    26,    27,    35,   134,    12,    26,    27,
      15,    26,    27,    18,    19,    20,   142,    22,    23,    24,
      25,    26,    27,    41,    42,   205,    49,    50,     7,   172,
      29,    49,    41,    54,    49,    97,    45,     1,   166,    69,
      47,    48,    47,     7,   172,   166,    51,    52,    12,   178,
      35,    15,   114,     7,    18,    19,    20,    49,    22,    23,
      24,    25,    26,    27,    29,    49,    49,   129,     7,    53,
       8,    80,    26,    27,   217,   201,     7,     8,     9,     7,
       1,    12,    47,    47,    69,   147,     7,    51,    52,   217,
      52,    12,    47,   174,    15,    26,    27,    18,    19,    20,
     145,    22,    23,    24,    25,    26,    27,    44,    45,    46,
       1,     3,     4,     5,     6,    29,     7,    50,   112,   113,
     114,    12,     8,     9,    15,    48,    47,    18,    19,    20,
      51,    22,    23,    24,    25,    26,    27,     8,     9,    40,
      26,    27,    32,    33,    47,    48,     3,     4,     5,     6,
      49,     8,     9,   147,    49,    51,    47,     8,    47,    47,
      51,   155,   156,   157,   158,   159,   160,   161,   162,    26,
      27,     3,     4,     5,     6,    47,     8,     9,     8,     9,
      28,    38,    30,    31,    41,    42,    47,     7,    36,    37,
      38,     9,    49,    48,    26,    27,    26,    27,    39,    40,
      41,    42,    43,    44,    45,    46,    50,    50,    38,    41,
      42,    41,    42,    52,    50,    50,    50,    49,    17,    49,
      39,    40,    41,    42,    43,    44,    45,    46,    52,     5,
      18,    50,    41,    42,    43,    44,    45,    46,    97,    34,
      65,   111,   122,   217,   178,   114,   166,   101,    37,    99
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    56,    57,     0,    11,    58,    59,    66,    67,    71,
      60,    61,    62,    56,     9,    72,    34,    47,    48,     7,
      26,    27,    64,    65,    51,    73,     7,    61,    63,     3,
       4,     5,     6,    74,    75,   128,    49,    29,     1,     7,
      12,    15,    18,    19,    20,    22,    23,    24,    25,    47,
      65,    73,    76,    78,   104,   105,   106,   108,   110,   111,
     112,   116,   120,   121,    74,    64,    68,    69,    70,   128,
       8,     9,   129,   130,   131,    54,   123,   122,    49,    85,
     113,     7,     7,    85,     8,   107,   107,   107,    29,    79,
     120,    77,    52,    47,    47,    79,    50,    48,    64,    40,
      49,    49,    86,   117,    85,    51,    47,    47,     9,    26,
      27,    38,    41,    42,    49,    80,    81,    82,    83,    84,
     102,   103,   127,   131,    47,    47,    69,   129,     1,    49,
     102,   124,   125,   126,   127,   124,    53,    85,    88,    90,
      91,    76,   114,     7,   109,   110,    80,    49,    84,    84,
      81,    84,   128,     7,   121,    39,    40,    41,    42,    43,
      44,    45,    46,     9,    82,    50,    48,   102,    50,    89,
      87,    95,    96,   118,    51,    76,   115,    52,    73,    84,
      50,    50,    50,    84,    84,    84,    84,    84,    84,    84,
      84,   126,    88,    50,    32,    33,    92,    94,   101,   102,
     121,    17,   119,   120,   109,    93,    97,    76,    52,    88,
      28,    30,    31,    36,    37,    38,    98,    99,   100,   101
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
#line 150 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (5)]).s,
					((yyvsp[(5) - (5)]).v.type == C_STRING1PAR) ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(5) - (5)]).v.val);
				freeval(&(yyvsp[(5) - (5)]).v);
			;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 157 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 158 "Parser.y"
    {	(yyval).s	= (yyvsp[(1) - (1)]).s; ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 160 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 161 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 165 "Parser.y"
    { startmacro(); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 166 "Parser.y"
    { (yyval).s = (yyvsp[(3) - (6)]).s; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 174 "Parser.y"
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
#line 184 "Parser.y"
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
#line 199 "Parser.y"
    {	(yyval).s = 0; ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 200 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 207 "Parser.y"
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
#line 214 "Parser.y"
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
#line 227 "Parser.y"
    { vname_count = 0; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 237 "Parser.y"
    {
				yyerror("Invalid statement. Expecting one of function call, assignment, if, while, case, break, continue, return, goto, block or label."); 
			;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 242 "Parser.y"
    {
				PushAssign((yyvsp[(1) - (2)]).s,(yyvsp[(2) - (2)]).v.type,(intptr_t)(yyvsp[(2) - (2)]).v.val);
			;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 246 "Parser.y"
    { 	(yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 248 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 249 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 251 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 252 "Parser.y"
    { (yyval).v = PushBinop(BIN_NOT,&(yyvsp[(2) - (2)]).v,0); 	;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 254 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 255 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_FURET,vname_count);
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 257 "Parser.y"
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
#line 267 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 268 "Parser.y"
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
#line 278 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (2)]).v ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 279 "Parser.y"
    { (yyval).v = MultiplyValue(&(yyvsp[(2) - (2)]).v, -1); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 280 "Parser.y"
    { (yyval).v = PushBinop(BIN_AND,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 281 "Parser.y"
    { (yyval).v = PushBinop(BIN_OR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 282 "Parser.y"
    { (yyval).v = PushBinop(BIN_ADD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 283 "Parser.y"
    { (yyval).v = PushBinop(BIN_XOR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 284 "Parser.y"
    { (yyval).v = PushBinop(BIN_SUB,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 285 "Parser.y"
    { (yyval).v = PushBinop(BIN_MUL,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 286 "Parser.y"
    { (yyval).v = PushBinop(BIN_DIV,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 287 "Parser.y"
    { (yyval).v = PushBinop(BIN_MOD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 288 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 292 "Parser.y"
    { TestOp(); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 292 "Parser.y"
    { CloseTestOp(CT_BRACKETS); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 296 "Parser.y"
    { TestOp(); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 296 "Parser.y"
    { CloseTestOp(CT_NOT); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 298 "Parser.y"
    { TestOp(); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 298 "Parser.y"
    { CloseTestOp((yyvsp[(3) - (3)]).type); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 300 "Parser.y"
    { (yyval).type = CT_BRACKETS; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 301 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; BinOp1((yyvsp[(1) - (1)]).type); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 303 "Parser.y"
    {	(yyval).type = CT_AND;	;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 304 "Parser.y"
    {	(yyval).type = CT_OR;	;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 306 "Parser.y"
    { TestOp(); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 307 "Parser.y"
    { BinOp1((yyvsp[(2) - (2)]).type);  ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 308 "Parser.y"
    { CloseTestOp((yyvsp[(4) - (4)]).type); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 310 "Parser.y"
    { 	
				if (_stringflg)
					_recp = AddComSeq(_recp,_recpend,C_STRING1PAR,(intptr_t)"");
				else
					_recp = AddComSeq(_recp,_recpend,C_LONG1PAR,0L);
				(yyval).type = CT_NE;
			;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 317 "Parser.y"
    {
				int flg = STRINGTYPE((yyvsp[(2) - (2)]).type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				int nType = (yyvsp[(1) - (2)]).type;
				(yyval).type = (nType | flg);
			;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 325 "Parser.y"
    {	(yyval).type = CT_LT;	;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
    {	(yyval).type = CT_GT;	;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 327 "Parser.y"
    {	(yyval).type = CT_EQ;	;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 328 "Parser.y"
    {	(yyval).type = CT_NE;	;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 329 "Parser.y"
    {
				if (_stringflg == CT_NUM)
					yyerror("match operator not allowed on numbers");
				(yyval).type = (yyvsp[(1) - (1)]).type;
			;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 335 "Parser.y"
    {	(yyval).type = CT_MATCH;			;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 336 "Parser.y"
    {	(yyval).type = CT_NMATCH;		;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 338 "Parser.y"
    {	(yyval).type = C_LONG1PAR; 	 	;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 339 "Parser.y"
    {	(yyval).type = PushVal(&(yyvsp[(1) - (1)]).v);	;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 341 "Parser.y"
    {
				(yyval).v.type = C_LONGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 345 "Parser.y"
    {
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 349 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 353 "Parser.y"
    {	(yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 355 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s;	;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 356 "Parser.y"
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

  case 102:

/* Line 1455 of yacc.c  */
#line 368 "Parser.y"
    {	gotolabel(lendid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 371 "Parser.y"
    {	gotolabel(lstartid,_breaklevel-(int)(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 373 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_STOP,(yyvsp[(2) - (2)]).num);
			;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 377 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 378 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 385 "Parser.y"
    {
				MakeLabel((yyvsp[(1) - (2)]).s,(COM_GOTO *)_recp);
				freeitem(&(yyvsp[(1) - (2)]).s);
			;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 390 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 394 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 399 "Parser.y"
    {
				makelabel(lstartid,_breaklevel);
			;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 401 "Parser.y"
    {
				gotolabel(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 404 "Parser.y"
    {
				_breaklevel--;
				gotolabel(lstartid,_breaklevel,BRA_ALWAYS);
				makelabel(lendid,_breaklevel);
				killabel(lstartid,_breaklevel);
				killabel(lendid,_breaklevel);
			;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 414 "Parser.y"
    {
				gotolabel(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    {
				_iflevel--;
				gotolabel(ifdoneid,_iflevel,BRA_ALWAYS);
				makelabel(iffailid,_iflevel);
				killabel(iffailid,_iflevel);
			;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 422 "Parser.y"
    {
				makelabel(ifdoneid,_iflevel);
				killabel(ifdoneid,_iflevel);
			;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 433 "Parser.y"
    {
				_lastfuncp = (yyvsp[(1) - (1)]).funcp;
				_parno = 0;
				_recp = AddComSeq(_recp,_recpend,C_0FUNC,
							   FuncIdx((yyvsp[(1) - (1)]).funcp));
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 440 "Parser.y"
    {
				_lastfuncp = 0;
				_recp = AddComSeq(_recp,_recpend,C_MACRO,(intptr_t)(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
				(yyval).funcp = 0;
			;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 450 "Parser.y"
    {
				PushParam(&(yyvsp[(1) - (1)]).v);
			;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 453 "Parser.y"
    {
				PushParam(&(yyvsp[(3) - (3)]).v);
			;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 456 "Parser.y"
    {
				yyerror("illegal parameters for method call");
			;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 460 "Parser.y"
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

  case 135:

/* Line 1455 of yacc.c  */
#line 477 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 481 "Parser.y"
    {	(yyval).type = C_CHAR1PAR;	;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 482 "Parser.y"
    {	(yyval).type = C_INT1PAR;	;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 483 "Parser.y"
    {	(yyval).type = C_LONG1PAR;	;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 484 "Parser.y"
    {	(yyval).type = C_STRING1PAR;	;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 486 "Parser.y"
    {
				(yyval).v.type = C_LONG1PAR; 
				(yyval).v.val  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 492 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 493 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 498 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 499 "Parser.y"
    {
				(yyval).v.type = (yyvsp[(1) - (3)]).v.type;
				(yyval).v.val = (yyvsp[(1) - (3)]).v.val | (yyvsp[(3) - (3)]).v.val;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 2571 "Parser.tab.c"
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
#line 503 "Parser.y"


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


