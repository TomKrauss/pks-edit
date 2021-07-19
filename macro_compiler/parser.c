
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
#line 43 "Parser.y"


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
int 			bind_key(KEYCODE keycode, MACROREFTYPE typ, MACROREFIDX idx, int augment);
int 			bind_mouse(MOUSECODE code, MACROREFTYPE typ, MACROREFIDX idx, int flags, int augment);
int 			sym_sym_makeInternalSymbolbol(char *name, char ed_typ, intptr_t value);
int 			IsFormStart(void *ep,int parno);
int 			key_switchtotable(char *name);
int 			mouse_switchtotable(char *name);
void 		mouse_destroyMouseBindings(void);
KEYCODE 		key2code(char *K, int control);
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
void 		menu_initializeDefinition(char *szMenu);
int 			menu_addMenuMacroItem(char *pszString, int menutype, 
				MACROREFTYPE mactype, MACROREFTYPE macidx);


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
     T_SHIFT = 284,
     T_CTRL = 285,
     T_ALT = 286,
     T_LEFT = 287,
     T_MIDDLE = 288,
     T_RIGHT = 289,
     T_HAS_SELECTION = 290,
     T_MENUPOPUP = 291,
     T_MENUSEPERATOR = 292,
     T_MENUBREAK = 293,
     T_MENUBARBREAK = 294,
     T_ASSIGN = 295,
     T_NE = 296,
     T_EQ = 297,
     T_OR = 298,
     T_AND = 299,
     T_DEFMACRO = 300,
     T_DEFICON = 301,
     T_DEFALTPATH = 302,
     T_DEFCOMPILER = 303,
     T_DEFDOCTYPES = 304,
     T_DEFMENUS = 305,
     T_DEFKEYS = 306,
     T_DEFMOUSE = 307,
     T_DEFTAGS = 308,
     T_DEFABBREVS = 309,
     T_DEFBRACKETS = 310,
     T_DEFESCAPES = 311,
     T_CMDSEQPREFIX = 312
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
#line 287 "Parser.tab.c"

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
#define YYLAST   344

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  77
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  108
/* YYNRULES -- Number of rules.  */
#define YYNRULES  208
/* YYNRULES -- Number of states.  */
#define YYNSTATES  316

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   312

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    76,     2,     2,     2,    68,    61,     2,
      74,    75,    66,    63,    71,    64,     2,    67,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    69,    70,
      58,     2,    59,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    65,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    72,    62,    73,    60,     2,     2,     2,
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
      55,    56,    57
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,     9,    11,    13,    15,    17,
      19,    21,    22,    25,    26,    33,    34,    36,    40,    41,
      49,    50,    53,    54,    56,    60,    62,    64,    66,    71,
      73,    75,    76,    80,    81,    89,    90,    92,    96,   101,
     103,   104,   106,   108,   110,   112,   114,   118,   122,   123,
     131,   132,   134,   138,   142,   144,   146,   148,   149,   153,
     154,   158,   160,   162,   164,   166,   170,   171,   175,   181,
     183,   187,   189,   193,   194,   195,   201,   203,   205,   207,
     209,   210,   217,   218,   220,   222,   226,   229,   233,   234,
     236,   237,   239,   244,   245,   248,   253,   257,   259,   262,
     263,   267,   269,   271,   273,   275,   277,   279,   281,   283,
     285,   287,   290,   293,   295,   299,   301,   304,   306,   307,
     310,   312,   315,   319,   323,   327,   331,   335,   339,   343,
     347,   351,   352,   353,   359,   361,   363,   364,   368,   369,
     373,   374,   375,   379,   381,   383,   384,   385,   390,   391,
     394,   396,   398,   400,   402,   404,   406,   408,   410,   412,
     414,   416,   418,   420,   422,   425,   429,   433,   436,   437,
     439,   445,   446,   450,   453,   456,   459,   460,   461,   467,
     468,   469,   476,   477,   480,   481,   484,   485,   491,   492,
     498,   499,   501,   503,   507,   509,   512,   514,   518,   520,
     522,   524,   526,   528,   531,   533,   535,   537,   539
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      78,     0,    -1,    -1,    -1,    79,    80,    78,    -1,   123,
      -1,   108,    -1,    82,    -1,    99,    -1,    91,    -1,   111,
      -1,    -1,    69,     7,    -1,    -1,    50,     7,    81,    83,
      84,    70,    -1,    -1,    85,    -1,    85,    71,    84,    -1,
      -1,    36,     9,    86,    87,    72,    88,    73,    -1,    -1,
      69,     9,    -1,    -1,    89,    -1,    88,    71,    89,    -1,
      37,    -1,    39,    -1,    38,    -1,     9,    90,    40,   181,
      -1,    85,    -1,     1,    -1,    -1,    90,    71,     7,    -1,
      -1,   125,    52,     7,    92,    81,    93,    70,    -1,    -1,
      94,    -1,    94,    71,    93,    -1,    98,    40,    95,   181,
      -1,     1,    -1,    -1,    63,    -1,    32,    -1,    34,    -1,
      33,    -1,    96,    -1,    96,    63,    97,    -1,   106,    97,
       8,    -1,    -1,   125,    51,     7,   100,    81,   101,    70,
      -1,    -1,   102,    -1,   101,    71,   102,    -1,   104,    40,
     181,    -1,     1,    -1,     7,    -1,     8,    -1,    -1,   106,
     105,   103,    -1,    -1,   107,    63,   106,    -1,    29,    -1,
      30,    -1,    31,    -1,    35,    -1,    46,   109,    70,    -1,
      -1,   110,    71,   109,    -1,     7,     7,     7,    40,     7,
      -1,     1,    -1,    11,   112,    70,    -1,   113,    -1,   112,
      71,   113,    -1,    -1,    -1,   114,   116,   115,    40,   183,
      -1,     7,    -1,   117,    -1,    27,    -1,    26,    -1,    -1,
     119,    45,     7,    74,   120,    75,    -1,    -1,   121,    -1,
     122,    -1,   122,    71,   121,    -1,   180,   116,    -1,   118,
     124,   126,    -1,    -1,     9,    -1,    -1,     7,    -1,    72,
     127,   172,    73,    -1,    -1,   128,   127,    -1,   180,   116,
     132,    70,    -1,   180,   116,    70,    -1,    70,    -1,   173,
      70,    -1,    -1,   131,   130,    70,    -1,   168,    -1,   165,
      -1,   161,    -1,   157,    -1,   158,    -1,   159,    -1,   164,
      -1,   126,    -1,   163,    -1,     1,    -1,   117,   132,    -1,
      40,   133,    -1,   134,    -1,    74,   134,    75,    -1,   137,
      -1,    60,   133,    -1,   155,    -1,    -1,   136,   173,    -1,
     135,    -1,   179,   135,    -1,   137,    61,   137,    -1,   137,
      62,   137,    -1,   137,    63,   137,    -1,   137,    65,   137,
      -1,   137,    64,   137,    -1,   137,    66,   137,    -1,   137,
      67,   137,    -1,   137,    68,   137,    -1,    74,   137,    75,
      -1,    -1,    -1,    74,   139,   141,   140,    75,    -1,   143,
      -1,   138,    -1,    -1,    76,   142,   141,    -1,    -1,   144,
     148,   145,    -1,    -1,    -1,   147,   146,   141,    -1,    44,
      -1,    43,    -1,    -1,    -1,   149,   154,   150,   151,    -1,
      -1,   152,   154,    -1,    58,    -1,    59,    -1,    42,    -1,
      41,    -1,   153,    -1,    60,    -1,    28,    -1,   173,    -1,
     155,    -1,    26,    -1,    27,    -1,   156,    -1,   184,    -1,
       9,    -1,   156,     9,    -1,    23,   160,    70,    -1,    24,
     160,    70,    -1,    25,   160,    -1,    -1,     8,    -1,    22,
     138,    72,   162,    73,    -1,    -1,   163,   126,   162,    -1,
       7,    69,    -1,    19,     7,    -1,    20,     7,    -1,    -1,
      -1,    18,   166,   138,   167,   129,    -1,    -1,    -1,    15,
     138,   169,   129,   170,   171,    -1,    -1,    17,   129,    -1,
      -1,   129,   172,    -1,    -1,    12,   174,    74,   176,    75,
      -1,    -1,     7,   175,    74,   176,    75,    -1,    -1,   177,
      -1,   178,    -1,   177,    71,   178,    -1,     1,    -1,   179,
     155,    -1,   155,    -1,    74,   180,    75,    -1,     5,    -1,
       4,    -1,     6,    -1,     3,    -1,     7,    -1,    57,     7,
      -1,     8,    -1,   184,    -1,     9,    -1,   182,    -1,   184,
      62,   182,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   140,   140,   141,   141,   147,   148,   149,   150,   151,
     152,   154,   155,   254,   253,   260,   261,   262,   265,   264,
     275,   276,   278,   279,   280,   282,   286,   287,   288,   293,
     294,   296,   297,   302,   302,   312,   313,   314,   316,   324,
     326,   327,   329,   330,   331,   333,   334,   336,   345,   345,
     355,   356,   357,   359,   370,   372,   378,   380,   380,   385,
     386,   388,   389,   390,   391,   395,   397,   398,   400,   408,
     412,   414,   415,   417,   417,   417,   426,   427,   429,   430,
     434,   434,   437,   438,   440,   441,   443,   452,   468,   469,
     471,   472,   474,   476,   477,   479,   486,   497,   498,   499,
     499,   500,   501,   502,   503,   504,   505,   506,   507,   508,
     509,   512,   516,   518,   519,   521,   522,   524,   525,   525,
     537,   538,   548,   549,   550,   551,   552,   553,   554,   555,
     556,   560,   560,   560,   562,   563,   564,   564,   566,   566,
     568,   569,   569,   571,   572,   574,   575,   574,   578,   585,
     592,   593,   594,   595,   596,   602,   603,   605,   606,   608,
     612,   616,   620,   622,   623,   634,   637,   640,   644,   645,
     647,   649,   650,   652,   657,   661,   666,   668,   666,   679,
     682,   679,   692,   693,   695,   696,   698,   698,   705,   705,
     712,   713,   715,   718,   721,   725,   742,   744,   746,   747,
     748,   749,   751,   759,   765,   771,   772,   777,   778
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
  "T_RETURN", "T_NUMVAR", "T_STRINGVAR", "T_NMATCH", "T_SHIFT", "T_CTRL",
  "T_ALT", "T_LEFT", "T_MIDDLE", "T_RIGHT", "T_HAS_SELECTION",
  "T_MENUPOPUP", "T_MENUSEPERATOR", "T_MENUBREAK", "T_MENUBARBREAK",
  "T_ASSIGN", "T_NE", "T_EQ", "T_OR", "T_AND", "T_DEFMACRO", "T_DEFICON",
  "T_DEFALTPATH", "T_DEFCOMPILER", "T_DEFDOCTYPES", "T_DEFMENUS",
  "T_DEFKEYS", "T_DEFMOUSE", "T_DEFTAGS", "T_DEFABBREVS", "T_DEFBRACKETS",
  "T_DEFESCAPES", "T_CMDSEQPREFIX", "'<'", "'>'", "'~'", "'&'", "'|'",
  "'+'", "'-'", "'^'", "'*'", "'/'", "'%'", "':'", "';'", "','", "'{'",
  "'}'", "'('", "')'", "'!'", "$accept", "resources", "$@1", "resource",
  "inherit", "menus", "$@2", "menudefs", "submenu", "$@3", "menuconnect",
  "menuitems", "menuitem", "menuoptions", "mouses", "$@4", "mousedefs",
  "mouse", "opt_plus", "m_button", "buttons", "mouse_button", "keys",
  "$@5", "keydefs", "key", "ident_or_num", "key_ident", "$@6", "kmod_list",
  "k_modifier", "icons", "icondefs", "icon", "constants", "constdefs",
  "constdef", "$@7", "$@8", "var_ident", "var", "macrostart", "$@9",
  "par_decls", "non_empty_pardecl", "par_decl", "macros", "opt_string",
  "opt_ident", "block", "var_decls", "var_decl", "stmnt", "$@10",
  "assignment", "assgn", "arith", "aexpr", "s_bterm", "$@11", "bterm",
  "cond", "$@12", "$@13", "expr", "$@14", "optandorexpr", "$@15",
  "optandor", "@16", "and_or", "op2", "$@17", "$@18", "optterm2", "relop",
  "strmatchop", "term", "value", "string", "break", "continue", "return",
  "opt_num", "case", "caselabels", "label", "gotolabel", "while", "$@19",
  "$@20", "if", "$@21", "$@22", "elseclause", "stmntlist", "fcall", "@23",
  "@24", "parlist", "nonemptypars", "par", "cast", "type_ident",
  "cmd_or_macro", "constval", "constnum_or_string", "constexpr", 0
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
     305,   306,   307,   308,   309,   310,   311,   312,    60,    62,
     126,    38,   124,    43,    45,    94,    42,    47,    37,    58,
      59,    44,   123,   125,    40,    41,    33
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    77,    78,    79,    78,    80,    80,    80,    80,    80,
      80,    81,    81,    83,    82,    84,    84,    84,    86,    85,
      87,    87,    88,    88,    88,    89,    89,    89,    89,    89,
      89,    90,    90,    92,    91,    93,    93,    93,    94,    94,
      95,    95,    96,    96,    96,    97,    97,    98,   100,    99,
     101,   101,   101,   102,   102,   103,   103,   105,   104,   106,
     106,   107,   107,   107,   107,   108,   109,   109,   110,   110,
     111,   112,   112,   114,   115,   113,   116,   116,   117,   117,
     119,   118,   120,   120,   121,   121,   122,   123,   124,   124,
     125,   125,   126,   127,   127,   128,   128,   129,   129,   130,
     129,   129,   129,   129,   129,   129,   129,   129,   129,   129,
     129,   131,   132,   133,   133,   134,   134,   135,   136,   135,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     137,   139,   140,   138,   141,   141,   142,   141,   144,   143,
     145,   146,   145,   147,   147,   149,   150,   148,   151,   151,
     152,   152,   152,   152,   152,   153,   153,   154,   154,   155,
     155,   155,   155,   156,   156,   157,   158,   159,   160,   160,
     161,   162,   162,   163,   164,   164,   166,   167,   165,   169,
     170,   168,   171,   171,   172,   172,   174,   173,   175,   173,
     176,   176,   177,   177,   177,   178,   178,   179,   180,   180,
     180,   180,   181,   181,   182,   183,   183,   184,   184
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,     3,     1,     1,     1,     1,     1,
       1,     0,     2,     0,     6,     0,     1,     3,     0,     7,
       0,     2,     0,     1,     3,     1,     1,     1,     4,     1,
       1,     0,     3,     0,     7,     0,     1,     3,     4,     1,
       0,     1,     1,     1,     1,     1,     3,     3,     0,     7,
       0,     1,     3,     3,     1,     1,     1,     0,     3,     0,
       3,     1,     1,     1,     1,     3,     0,     3,     5,     1,
       3,     1,     3,     0,     0,     5,     1,     1,     1,     1,
       0,     6,     0,     1,     1,     3,     2,     3,     0,     1,
       0,     1,     4,     0,     2,     4,     3,     1,     2,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     3,     1,     2,     1,     0,     2,
       1,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     0,     0,     5,     1,     1,     0,     3,     0,     3,
       0,     0,     3,     1,     1,     0,     0,     4,     0,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     3,     2,     0,     1,
       5,     0,     3,     2,     2,     2,     0,     0,     5,     0,
       0,     6,     0,     2,     0,     2,     0,     5,     0,     5,
       0,     1,     1,     3,     1,     2,     1,     3,     1,     1,
       1,     1,     1,     2,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,    90,     1,    91,    73,     0,     0,     3,     7,
       9,     8,     6,    10,    88,     0,     5,     0,     0,    71,
       0,    69,     0,     0,     0,    11,     4,    89,     0,     0,
       0,     0,    70,    73,    76,    79,    78,    74,    77,     0,
      65,     0,     0,    13,    93,    87,     0,    48,    33,    72,
       0,     0,    67,    12,    15,   201,   199,   198,   200,     0,
      93,     0,    82,    11,    11,     0,     0,     0,     0,    16,
     110,   188,   186,     0,   176,     0,     0,     0,   168,   168,
     168,    97,     0,   108,     0,    99,   104,   105,   106,   103,
     109,   107,   102,   101,     0,     0,    94,     0,     0,    83,
      84,     0,     0,     0,   204,   206,   207,    75,   205,    68,
      18,    14,    15,   173,     0,     0,   131,   179,     0,   174,
     175,     0,   169,     0,     0,   167,   118,   111,   185,     0,
      92,    98,    96,     0,    81,     0,    86,    54,    61,    62,
      63,    64,     0,    51,     0,    57,     0,    39,     0,    36,
       0,     0,     0,    20,    17,     0,     0,   138,     0,   177,
     171,   165,   166,   163,   159,   160,   118,   118,   112,   113,
     120,     0,   115,   117,   161,   118,   162,   100,    95,    85,
      49,     0,     0,     0,    59,    34,     0,    40,    42,    44,
      43,    45,     0,   208,     0,     0,   194,     0,   196,     0,
     191,   192,     0,     0,   136,   135,   132,   134,   145,   180,
       0,     0,     0,     0,   116,   118,     0,     0,     0,   188,
     119,   118,   118,   118,   118,   118,   118,   118,   118,   164,
     121,    52,   202,     0,    53,    55,    56,    58,    60,    37,
      41,     0,     0,    47,    21,     0,   189,     0,   195,   187,
     138,     0,   140,     0,   182,   178,   170,   171,     0,   114,
     130,   197,   122,   123,   124,   126,   125,   127,   128,   129,
     203,    38,    46,    30,    31,    25,    27,    26,    29,     0,
      23,   193,   137,   133,   144,   143,   139,   141,   146,   158,
     157,     0,   181,   172,     0,     0,    19,   138,   148,   183,
       0,     0,    24,   142,   156,   153,   152,   150,   151,   155,
     147,     0,   154,    28,    32,   149
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     8,    43,     9,    54,    68,    69,   153,
     195,   279,   280,   294,    10,    64,   148,   149,   241,   191,
     192,   150,    11,    63,   142,   143,   237,   144,   183,   145,
     146,    12,    23,    24,    13,    18,    19,    20,    50,    37,
      82,    14,    15,    98,    99,   100,    16,    28,    17,    83,
      59,    60,    84,   129,    85,   127,   168,   169,   170,   171,
     172,   205,   157,   251,   206,   250,   207,   208,   286,   297,
     287,   252,   253,   298,   310,   311,   312,   288,   173,   174,
      86,    87,    88,   123,    89,   212,    90,    91,    92,   118,
     210,    93,   158,   254,   292,    94,    95,   115,   114,   199,
     200,   201,   175,   218,   234,   106,   107,   176
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -232
static const yytype_int16 yypact[] =
{
      24,    53,   126,  -232,  -232,  -232,    12,    98,    24,  -232,
    -232,  -232,  -232,  -232,    93,    66,  -232,   -20,   -29,  -232,
      52,  -232,   101,    56,    79,    85,  -232,  -232,    84,   151,
     161,   167,  -232,  -232,  -232,  -232,  -232,  -232,  -232,   179,
    -232,    12,   181,  -232,   255,  -232,    90,  -232,  -232,  -232,
     159,   173,  -232,  -232,   168,  -232,  -232,  -232,  -232,    43,
     255,    52,   255,    85,    85,   110,   220,   219,   177,   158,
    -232,   162,  -232,   175,  -232,   272,   273,   175,   274,   274,
     274,  -232,   241,  -232,    43,  -232,  -232,  -232,  -232,  -232,
    -232,  -232,  -232,  -232,   212,   213,  -232,    16,   209,  -232,
     215,    52,   152,   160,  -232,  -232,  -232,  -232,   225,  -232,
    -232,  -232,   168,  -232,   214,   216,  -232,  -232,   175,  -232,
    -232,   217,  -232,   221,   222,  -232,    20,  -232,  -232,   223,
    -232,  -232,  -232,   224,  -232,   255,  -232,  -232,  -232,  -232,
    -232,  -232,    96,  -232,   256,  -232,   232,  -232,   227,   228,
     258,    63,   292,   233,  -232,     9,     9,   -25,   105,  -232,
     294,  -232,  -232,  -232,  -232,  -232,    20,   136,  -232,  -232,
    -232,    31,   204,  -232,   295,    95,   225,  -232,  -232,  -232,
    -232,   211,    19,   172,   176,  -232,   160,   240,  -232,  -232,
    -232,   242,   298,  -232,   299,   235,  -232,   255,  -232,   234,
     239,  -232,    95,   237,  -232,  -232,  -232,  -232,  -232,  -232,
     105,   162,   238,    84,  -232,   143,   243,   189,   244,  -232,
    -232,     7,     7,     7,     7,     7,     7,     7,     7,  -232,
    -232,  -232,  -232,   306,  -232,  -232,  -232,  -232,  -232,  -232,
    -232,    19,    63,  -232,  -232,    36,  -232,    13,  -232,  -232,
     -25,   245,   165,   236,   297,  -232,  -232,   294,   189,  -232,
    -232,  -232,   210,   210,    68,    68,    68,  -232,  -232,  -232,
    -232,  -232,  -232,  -232,  -232,  -232,  -232,  -232,  -232,    17,
    -232,  -232,  -232,  -232,  -232,  -232,  -232,  -232,  -232,  -232,
    -232,   105,  -232,  -232,   -17,   164,  -232,   -25,   156,  -232,
      19,   308,  -232,  -232,  -232,  -232,  -232,  -232,  -232,  -232,
    -232,   236,  -232,  -232,  -232,  -232
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -232,   309,  -232,  -232,   157,  -232,  -232,   218,  -231,  -232,
    -232,  -232,    21,  -232,  -232,  -232,   135,  -232,  -232,  -232,
      80,  -232,  -232,  -232,  -232,   142,  -232,  -232,  -232,   -95,
    -232,  -232,   283,  -232,  -232,  -232,   293,  -232,  -232,   -41,
      -9,  -232,  -232,  -232,   190,  -232,  -232,  -232,  -232,   -28,
     267,  -232,  -153,  -232,  -232,   231,   163,   166,   169,  -232,
      11,    37,  -232,  -232,  -220,  -232,  -232,  -232,  -232,  -232,
    -232,  -232,  -232,  -232,  -232,  -232,  -232,    23,  -154,  -232,
    -232,  -232,  -232,   145,  -232,    74,  -156,  -232,  -232,  -232,
    -232,  -232,  -232,  -232,  -232,   248,  -168,  -232,  -232,   180,
    -232,    88,  -149,   -35,  -229,   185,  -232,   275
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -191
static const yytype_int16 yytable[] =
{
      45,   198,   198,   220,   213,   209,   202,   202,   151,    61,
     196,    38,   271,    21,   278,   104,   163,   104,   163,    22,
      97,   104,   163,   300,    -2,    61,   232,   101,   104,   163,
     282,    30,    31,   164,   165,   164,   165,   273,   219,   164,
     165,    32,    33,    72,    70,   274,   164,   165,   248,   116,
      71,   204,    38,     3,   301,    72,   126,   255,    73,    34,
     136,    74,    75,    76,   278,    77,    78,    79,    80,    35,
      36,   313,    67,   275,   276,   277,   233,   303,    35,    36,
     166,   215,   -66,   197,  -190,   290,   132,   197,   295,   238,
     296,   151,    38,   198,   167,   188,   189,   190,   202,   289,
     101,   213,    27,   104,   163,    25,    70,   -22,    39,   -22,
     117,    29,    71,    81,   121,    44,  -184,    72,   104,   105,
      73,   164,   165,    74,    75,    76,    40,    77,    78,    79,
      80,    35,    36,     4,   226,   227,   228,     5,   299,    55,
      56,    57,    58,   290,   104,   163,    55,    56,    57,    58,
      41,   104,   163,   137,    42,   159,    44,   289,    46,   -59,
     -59,   147,   164,   165,    62,   273,   180,   181,    47,   164,
     165,   -80,     6,   274,    48,    81,     7,    44,   217,   235,
     236,   138,   139,   140,   304,   257,    51,   141,    53,   138,
     139,   140,   -59,   -59,   -59,   141,   166,   305,   306,    65,
      67,   275,   276,   277,    67,   138,   139,   140,   284,   285,
     215,   141,   137,    66,   307,   308,   309,   215,   -59,   -59,
     102,   103,   -50,   -50,   124,   125,   258,   109,   110,   112,
     -35,   113,   262,   263,   264,   265,   266,   267,   268,   269,
     138,   139,   140,   219,   104,   163,   141,   111,    72,   116,
     221,   222,   223,   224,   225,   226,   227,   228,    55,    56,
      57,    58,   164,   165,   260,   221,   222,   223,   224,   225,
     226,   227,   228,   223,   224,   225,   226,   227,   228,   119,
     120,   126,   122,   131,   134,   130,   135,   152,   155,   160,
     156,   161,   162,   177,   178,   184,   182,   185,   187,   186,
     104,   211,   194,   240,   229,   242,   243,   245,   244,   246,
     247,   256,   249,   270,   291,   314,   302,    26,   259,   261,
     283,   239,   272,   231,    52,   179,    49,    96,   133,   214,
     154,   293,   128,   216,   315,   281,   203,   193,     0,     0,
     108,     0,     0,     0,   230
};

static const yytype_int16 yycheck[] =
{
      28,   155,   156,   171,   160,   158,   155,   156,   103,    44,
       1,    20,   241,     1,   245,     8,     9,     8,     9,     7,
      61,     8,     9,    40,     0,    60,     7,    62,     8,     9,
     250,    51,    52,    26,    27,    26,    27,     1,     7,    26,
      27,    70,    71,    12,     1,     9,    26,    27,   202,    74,
       7,    76,    61,     0,    71,    12,    40,   210,    15,     7,
     101,    18,    19,    20,   295,    22,    23,    24,    25,    26,
      27,   300,    36,    37,    38,    39,    57,   297,    26,    27,
      60,    74,    70,    74,    75,   253,    70,    74,    71,   184,
      73,   186,   101,   247,    74,    32,    33,    34,   247,   253,
     135,   257,     9,     8,     9,     7,     1,    71,     7,    73,
      73,    45,     7,    70,    77,    72,    73,    12,     8,     9,
      15,    26,    27,    18,    19,    20,    70,    22,    23,    24,
      25,    26,    27,     7,    66,    67,    68,    11,   291,     3,
       4,     5,     6,   311,     8,     9,     3,     4,     5,     6,
      71,     8,     9,     1,    69,   118,    72,   311,     7,     7,
       8,     1,    26,    27,    74,     1,    70,    71,     7,    26,
      27,    45,    46,     9,     7,    70,    50,    72,   167,     7,
       8,    29,    30,    31,    28,   213,     7,    35,     7,    29,
      30,    31,    32,    33,    34,    35,    60,    41,    42,    40,
      36,    37,    38,    39,    36,    29,    30,    31,    43,    44,
      74,    35,     1,    40,    58,    59,    60,    74,     7,     8,
      63,    64,    70,    71,    79,    80,   215,     7,     9,    71,
      70,    69,   221,   222,   223,   224,   225,   226,   227,   228,
      29,    30,    31,     7,     8,     9,    35,    70,    12,    74,
      61,    62,    63,    64,    65,    66,    67,    68,     3,     4,
       5,     6,    26,    27,    75,    61,    62,    63,    64,    65,
      66,    67,    68,    63,    64,    65,    66,    67,    68,     7,
       7,    40,     8,    70,    75,    73,    71,    62,    74,    72,
      74,    70,    70,    70,    70,    63,    40,    70,    40,    71,
       8,     7,    69,    63,     9,    63,     8,    72,     9,    75,
      71,    73,    75,     7,    17,     7,   295,     8,    75,    75,
      75,   186,   242,   181,    41,   135,    33,    60,    97,   166,
     112,   257,    84,   167,   311,   247,   156,   152,    -1,    -1,
      65,    -1,    -1,    -1,   175
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    78,    79,     0,     7,    11,    46,    50,    80,    82,
      91,    99,   108,   111,   118,   119,   123,   125,   112,   113,
     114,     1,     7,   109,   110,     7,    78,     9,   124,    45,
      51,    52,    70,    71,     7,    26,    27,   116,   117,     7,
      70,    71,    69,    81,    72,   126,     7,     7,     7,   113,
     115,     7,   109,     7,    83,     3,     4,     5,     6,   127,
     128,   180,    74,   100,    92,    40,    40,    36,    84,    85,
       1,     7,    12,    15,    18,    19,    20,    22,    23,    24,
      25,    70,   117,   126,   129,   131,   157,   158,   159,   161,
     163,   164,   165,   168,   172,   173,   127,   116,   120,   121,
     122,   180,    81,    81,     8,     9,   182,   183,   184,     7,
       9,    70,    71,    69,   175,   174,    74,   138,   166,     7,
       7,   138,     8,   160,   160,   160,    40,   132,   172,   130,
      73,    70,    70,   132,    75,    71,   116,     1,    29,    30,
      31,    35,   101,   102,   104,   106,   107,     1,    93,    94,
      98,   106,    62,    86,    84,    74,    74,   139,   169,   138,
      72,    70,    70,     9,    26,    27,    60,    74,   133,   134,
     135,   136,   137,   155,   156,   179,   184,    70,    70,   121,
      70,    71,    40,   105,    63,    70,    71,    40,    32,    33,
      34,    96,    97,   182,    69,    87,     1,    74,   155,   176,
     177,   178,   179,   176,    76,   138,   141,   143,   144,   129,
     167,     7,   162,   163,   133,    74,   134,   137,   180,     7,
     173,    61,    62,    63,    64,    65,    66,    67,    68,     9,
     135,   102,     7,    57,   181,     7,     8,   103,   106,    93,
      63,    95,    63,     8,     9,    72,    75,    71,   155,    75,
     142,   140,   148,   149,   170,   129,    73,   126,   137,    75,
      75,    75,   137,   137,   137,   137,   137,   137,   137,   137,
       7,   181,    97,     1,     9,    37,    38,    39,    85,    88,
      89,   178,   141,    75,    43,    44,   145,   147,   154,   155,
     173,    17,   171,   162,    90,    71,    73,   146,   150,   129,
      40,    71,    89,   141,    28,    41,    42,    58,    59,    60,
     151,   152,   153,   181,     7,   154
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
#line 141 "Parser.y"
    { 
#ifdef YYDEBUG
int yydebug = 1; 
#endif
			;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 254 "Parser.y"
    {
				menu_initializeDefinition((yyvsp[(2) - (3)]).s);
				freeitem(&(yyvsp[(2) - (3)]).s);
			;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 265 "Parser.y"
    {
				macro_showStatus("Defining sub-menu %s",(yyvsp[(2) - (2)]).s);
				menu_addMenuMacroItem((yyvsp[(2) - (2)]).s, UM_POPUP, 0, 0);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 271 "Parser.y"
    {
				menu_addMenuMacroItem((char*)0, UM_ENDPOPUP, 0, 0);
			;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 283 "Parser.y"
    {
				menu_addMenuMacroItem((char *)0, UM_SEPARATOR, 0, 0);
			;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 289 "Parser.y"
    {
				menu_addMenuMacroItem((yyvsp[(1) - (4)]).s, UM_ITEM, (yyvsp[(4) - (4)]).macref.typ, (yyvsp[(4) - (4)]).macref.index);
				freeitem(&(yyvsp[(1) - (4)]).s);
			;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 294 "Parser.y"
    { yyerror("invalid menu item"); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 302 "Parser.y"
    {
				macro_showStatus("Compile mouse table %s",(yyvsp[(3) - (3)]).s);
				mouse_switchtotable((yyvsp[(3) - (3)]).s);
				if (SetOverride((yyvsp[(1) - (3)]).s) == MODE_OVERRIDE) {
					mouse_destroyMouseBindings();
				}
				freeitem(&(yyvsp[(3) - (3)]).s);
			;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 317 "Parser.y"
    {
				if ((yyvsp[(1) - (4)]).mousecode.button && 
				    (yyvsp[(3) - (4)]).macref.index != (MACROREFIDX)-1) {
					bind_mouse((yyvsp[(1) - (4)]).mousecode, (yyvsp[(4) - (4)]).macref.typ, (yyvsp[(4) - (4)]).macref.index,
						(yyvsp[(3) - (4)]).num, _override == MODE_AUGMENT);
				}
			;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 324 "Parser.y"
    { yyerror("mouse binding"); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 326 "Parser.y"
    { (yyval).num = 0; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 327 "Parser.y"
    { (yyval).num = 0; ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 329 "Parser.y"
    { (yyval).num = MBUT_L; ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 330 "Parser.y"
    { (yyval).num = MBUT_R; ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 331 "Parser.y"
    { (yyval).num = MBUT_M; ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 333 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (1)]).num; 		  ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 334 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (3)]).num | (yyvsp[(3) - (3)]).num; ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 337 "Parser.y"
    {
				(yyval).mousecode.shift = (yyvsp[(1) - (3)]).num >> 8;
				(yyval).mousecode.button = (yyvsp[(2) - (3)]).num;
				(yyval).mousecode.nclicks = (yyvsp[(3) - (3)]).num;
			;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 345 "Parser.y"
    {
				macro_showStatus("Compile key table %s",(yyvsp[(3) - (3)]).s);
				key_switchtotable((yyvsp[(3) - (3)]).s);
				if (SetOverride((yyvsp[(1) - (3)]).s) == MODE_OVERRIDE) {
					key_destroytable((yyvsp[(3) - (3)]).s);
				}
				freeitem(&(yyvsp[(3) - (3)]).s);
			;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 360 "Parser.y"
    {
				if ((yyvsp[(1) - (3)]).num != K_INVALID && 
				    (yyvsp[(3) - (3)]).macref.index != (MACROREFIDX) -1) {
					if (!bind_key((KEYCODE)(yyvsp[(1) - (3)]).num, (yyvsp[(3) - (3)]).macref.typ, 
							   (yyvsp[(3) - (3)]).macref.index,
							   _override == MODE_AUGMENT)) {
						yyerror("cannot bind key 0x%lx",(yyvsp[(1) - (3)]).num);
					}
				}
			;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 370 "Parser.y"
    { yyerror("key binding"); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 373 "Parser.y"
    {
				if (((yyval).num = key2code((yyvsp[(1) - (1)]).s,_kmod_control)) == K_INVALID)
					yyerror("bad key %s",(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
			;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 378 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num|_kmod_control; 	;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 380 "Parser.y"
    {	_kmod_control = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 381 "Parser.y"
    { 
				(yyval).num = (yyvsp[(3) - (3)]).num;
			;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 385 "Parser.y"
    {	(yyval).num = 0; ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 386 "Parser.y"
    { (yyval).num = (yyvsp[(1) - (3)]).num | (yyvsp[(3) - (3)]).num; ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 388 "Parser.y"
    { (yyval).num = /* K_SHIFT */ 0x300; ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 389 "Parser.y"
    { (yyval).num = K_CONTROL; ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 390 "Parser.y"
    { (yyval).num = K_ALTERNATE; ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 391 "Parser.y"
    { (yyval).num = K_HAS_SELECTION; ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 401 "Parser.y"
    {	
				macro_showStatus("Defining icon for macro %s",(yyvsp[(5) - (5)]).s);
				freeitem(&(yyvsp[(1) - (5)]).s);
				freeitem(&(yyvsp[(2) - (5)]).s);
				freeitem(&(yyvsp[(3) - (5)]).s);
				freeitem(&(yyvsp[(5) - (5)]).s);
			;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 408 "Parser.y"
    { yyerror("icon definition"); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    { _bDefiningConst = 1; ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 417 "Parser.y"
    { _bDefiningConst = 0; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 419 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (5)]).s,
					((yyvsp[(5) - (5)]).v.type == C_STRING1PAR) ? 
					S_CONSTSTRING : S_CONSTNUM, (yyvsp[(5) - (5)]).v.val);
				freeval(&(yyvsp[(5) - (5)]).v);
			;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 426 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 427 "Parser.y"
    {	(yyval).s	= (yyvsp[(1) - (1)]).s; ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 429 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 430 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 434 "Parser.y"
    { startmacro(); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 435 "Parser.y"
    { (yyval).s = (yyvsp[(3) - (6)]).s; ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 443 "Parser.y"
    {
				int typ;
				
				typ = ((yyvsp[(1) - (2)]).type == C_STRING1PAR) ? S_DOLSTRING : S_DOLNUMBER;
				sym_makeInternalSymbol((yyvsp[(2) - (2)]).s,typ,_nparam);
				PushCreateVariable((yyvsp[(2) - (2)]).s,typ,_nparam);
				_nparam++;
			;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 453 "Parser.y"
    {
				if (!yyerrflg) {
					makelabel(lreturnid,0);
					killabel(lreturnid,0);
					macro_validateMacroName((yyvsp[(1) - (3)]).s, -1);
					_recp = AddComSeq(_recp,_recpend,C_STOP,1);
					macro_showStatus("Defining macro %s",(yyvsp[(1) - (3)]).s);
					macro_insertNewMacro((yyvsp[(1) - (3)]).s,(yyvsp[(2) - (3)]).s,_recspace,(int)(_recp-_recspace));
				}
				freeitem(&(yyvsp[(2) - (3)]).s);
				freeitem(&(yyvsp[(1) - (3)]).s);
				CloseLabelList();
				endmacro();
			;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 468 "Parser.y"
    {	(yyval).s = 0; ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 469 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 471 "Parser.y"
    {	(yyval).s = ""; ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 472 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s; ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 479 "Parser.y"
    {
				sym_makeInternalSymbol((yyvsp[(2) - (4)]).s,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? S_STRING : S_NUMBER,
					((yyvsp[(1) - (4)]).type == C_STRING1PAR) ? (intptr_t)"" : 0);
				PushAssign((yyvsp[(2) - (4)]).s,(yyvsp[(3) - (4)]).v.type,(intptr_t)(yyvsp[(3) - (4)]).v.val);
				freeval(&(yyvsp[(3) - (4)]).v);
				vname_count = 0;
			;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 486 "Parser.y"
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

  case 99:

/* Line 1455 of yacc.c  */
#line 499 "Parser.y"
    { vname_count = 0; ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 509 "Parser.y"
    { yyerror("invalid statement"); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 512 "Parser.y"
    {
				PushAssign((yyvsp[(1) - (2)]).s,(yyvsp[(2) - (2)]).v.type,(intptr_t)(yyvsp[(2) - (2)]).v.val);
			;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 516 "Parser.y"
    { 	(yyval).v = (yyvsp[(2) - (2)]).v;	;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 518 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 519 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 521 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 522 "Parser.y"
    { (yyval).v = PushBinop(BIN_NOT,&(yyvsp[(2) - (2)]).v,0); 	;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 524 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 525 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_FURET,vname_count);
			;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 527 "Parser.y"
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

  case 120:

/* Line 1455 of yacc.c  */
#line 537 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 538 "Parser.y"
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

  case 122:

/* Line 1455 of yacc.c  */
#line 548 "Parser.y"
    { (yyval).v = PushBinop(BIN_AND,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 549 "Parser.y"
    { (yyval).v = PushBinop(BIN_OR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 550 "Parser.y"
    { (yyval).v = PushBinop(BIN_ADD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 551 "Parser.y"
    { (yyval).v = PushBinop(BIN_XOR,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 552 "Parser.y"
    { (yyval).v = PushBinop(BIN_SUB,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 553 "Parser.y"
    { (yyval).v = PushBinop(BIN_MUL,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 554 "Parser.y"
    { (yyval).v = PushBinop(BIN_DIV,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 555 "Parser.y"
    { (yyval).v = PushBinop(BIN_MOD,&(yyvsp[(1) - (3)]).v,&(yyvsp[(3) - (3)]).v); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 556 "Parser.y"
    { (yyval).v = (yyvsp[(2) - (3)]).v; ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 560 "Parser.y"
    { TestOp(); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 560 "Parser.y"
    { CloseTestOp(CT_BRACKETS); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 564 "Parser.y"
    { TestOp(); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 564 "Parser.y"
    { CloseTestOp(CT_NOT); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 566 "Parser.y"
    { TestOp(); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 566 "Parser.y"
    { CloseTestOp((yyvsp[(3) - (3)]).type); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 568 "Parser.y"
    { (yyval).type = CT_BRACKETS; ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 569 "Parser.y"
    { (yyval).type = (yyvsp[(1) - (1)]).type; BinOp1((yyvsp[(1) - (1)]).type); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 571 "Parser.y"
    {	(yyval).type = CT_AND;	;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 572 "Parser.y"
    {	(yyval).type = CT_OR;	;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 574 "Parser.y"
    { TestOp(); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 575 "Parser.y"
    { BinOp1((yyvsp[(2) - (2)]).type);  ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 576 "Parser.y"
    { CloseTestOp((yyvsp[(4) - (4)]).type); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 578 "Parser.y"
    { 	
				if (_stringflg)
					_recp = AddComSeq(_recp,_recpend,C_STRING1PAR,(intptr_t)"");
				else
					_recp = AddComSeq(_recp,_recpend,C_LONG1PAR,0L);
				(yyval).type = CT_NE;
			;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 585 "Parser.y"
    {
				int flg = STRINGTYPE((yyvsp[(2) - (2)]).type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				(yyval).type = (yyvsp[(1) - (2)]).type | flg;
			;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 592 "Parser.y"
    {	(yyval).type = CT_LT;	;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 593 "Parser.y"
    {	(yyval).type = CT_GT;	;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 594 "Parser.y"
    {	(yyval).type = CT_EQ;	;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 595 "Parser.y"
    {	(yyval).type = CT_NE;	;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 596 "Parser.y"
    {
				if (_stringflg == CT_NUM)
					yyerror("match operator not allowed on numbers");
				(yyval).type = (yyvsp[(1) - (1)]).type;
			;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 602 "Parser.y"
    {	(yyval).type = CT_MATCH;			;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 603 "Parser.y"
    {	(yyval).type = CT_NMATCH;		;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 605 "Parser.y"
    {	(yyval).type = C_LONG1PAR; 	 	;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 606 "Parser.y"
    {	(yyval).type = PushVal(&(yyvsp[(1) - (1)]).v);	;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 608 "Parser.y"
    {
				(yyval).v.type = C_LONGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 612 "Parser.y"
    {
				(yyval).v.type = C_STRINGVAR;  
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 616 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 620 "Parser.y"
    {	(yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 622 "Parser.y"
    {	(yyval).s = (yyvsp[(1) - (1)]).s;	;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 623 "Parser.y"
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

  case 165:

/* Line 1455 of yacc.c  */
#line 635 "Parser.y"
    {	gotolabel(lendid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 638 "Parser.y"
    {	gotolabel(lstartid,_breaklevel-(yyvsp[(2) - (3)]).num,BRA_ALWAYS); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 640 "Parser.y"
    {
				_recp = AddComSeq(_recp,_recpend,C_STOP,(yyvsp[(2) - (2)]).num);
			;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 644 "Parser.y"
    {	(yyval).num = 1;	;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 645 "Parser.y"
    {	(yyval).num = (yyvsp[(1) - (1)]).num; ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 652 "Parser.y"
    {
				MakeLabel((yyvsp[(1) - (2)]).s,(COM_GOTO *)_recp);
				freeitem(&(yyvsp[(1) - (2)]).s);
			;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 657 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 661 "Parser.y"
    {
				_recp = GotoLabel((yyvsp[(2) - (2)]).s,_recp,_recpend,BRA_EQ);
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 666 "Parser.y"
    {
				makelabel(lstartid,_breaklevel);
			;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 668 "Parser.y"
    {
				gotolabel(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 671 "Parser.y"
    {
				_breaklevel--;
				gotolabel(lstartid,_breaklevel,BRA_ALWAYS);
				makelabel(lendid,_breaklevel);
				killabel(lstartid,_breaklevel);
				killabel(lendid,_breaklevel);
			;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 679 "Parser.y"
    {
				gotolabel(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 682 "Parser.y"
    {
				_iflevel--;
				gotolabel(ifdoneid,_iflevel,BRA_ALWAYS);
				makelabel(iffailid,_iflevel);
				killabel(iffailid,_iflevel);
			;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 687 "Parser.y"
    {
				makelabel(ifdoneid,_iflevel);
				killabel(ifdoneid,_iflevel);
			;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 698 "Parser.y"
    {
				_lastfuncp = (yyvsp[(1) - (1)]).funcp;
				_parno = 0;
				_recp = AddComSeq(_recp,_recpend,C_0FUNC,
							   FuncIdx((yyvsp[(1) - (1)]).funcp));
				(yyval).funcp = (yyvsp[(1) - (1)]).funcp;
			;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 705 "Parser.y"
    {
				_lastfuncp = 0;
				_recp = AddComSeq(_recp,_recpend,C_MACRO,(intptr_t)(yyvsp[(1) - (1)]).s);
				freeitem(&(yyvsp[(1) - (1)]).s);
				(yyval).funcp = 0;
			;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 715 "Parser.y"
    {
				PushParam(&(yyvsp[(1) - (1)]).v);
			;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 718 "Parser.y"
    {
				PushParam(&(yyvsp[(3) - (3)]).v);
			;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 721 "Parser.y"
    {
				yyerror("illegal parameters");
			;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 725 "Parser.y"
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

  case 196:

/* Line 1455 of yacc.c  */
#line 742 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 744 "Parser.y"
    {	(yyval).type = (yyvsp[(2) - (3)]).type;		;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 746 "Parser.y"
    {	(yyval).type = C_CHAR1PAR;	;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 747 "Parser.y"
    {	(yyval).type = C_INT1PAR;	;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 748 "Parser.y"
    {	(yyval).type = C_LONG1PAR;	;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 749 "Parser.y"
    {	(yyval).type = C_STRING1PAR;	;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 751 "Parser.y"
    {
				(yyval).macref.typ = CMD_MACRO; 
				if (((yyval).macref.index = macro_getInternalIndexByName((yyvsp[(1) - (1)]).s)) == 
					(MACROREFIDX)-1) {
					yyerror("undefined macro %s",(yyvsp[(1) - (1)]).s);
				}
				freeitem(&(yyvsp[(1) - (1)]).s);
			;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 759 "Parser.y"
    {
				(yyval).macref.typ = CMD_CMDSEQ; 
				(yyval).macref.index = macro_getCmdIndexByName((yyvsp[(2) - (2)]).s); 
				freeitem(&(yyvsp[(2) - (2)]).s);
			;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 765 "Parser.y"
    {
				(yyval).v.type = C_LONG1PAR; 
				(yyval).v.val  = (yyvsp[(1) - (1)]).num;
			;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 771 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v; ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 772 "Parser.y"
    {
				(yyval).v.type = C_STRING1PAR;
				(yyval).v.val  = (intptr_t) (yyvsp[(1) - (1)]).s;
			;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 777 "Parser.y"
    { (yyval).v = (yyvsp[(1) - (1)]).v;	;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 778 "Parser.y"
    {
				(yyval).v.type = (yyvsp[(1) - (3)]).v.type;
				(yyval).v.val = (yyvsp[(1) - (3)]).v.val | (yyvsp[(3) - (3)]).v.val;
			;}
    break;



/* Line 1455 of yacc.c  */
#line 3008 "Parser.tab.c"
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


