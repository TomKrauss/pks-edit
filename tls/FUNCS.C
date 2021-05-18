
/*  A Bison parser, made from .\funcs.y  */

#define	MOUSE	258
#define	_ICON	259
#define	ID	260
#define	CHAR	261
#define	IDENT	262
#define	FORM	263
#define	_MENU	264
#define	FLAG	265
#define	KEY	266
#define	CMD	267
#define	STRING	268
#define	NUMBER	269
#define	DOTDOT	270
#define	T_SHIFT	271
#define	T_CTRL	272
#define	T_ALT	273
#define	T_LEFT	274
#define	T_RIGHT	275
#define	T_MIDDLE	276
#define	ENUM	277
#define	OPTION	278

#line 18 ".\funcs.y"


# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdarg.h>
# include <ctype.h>
# include "edfuncs.h"
# include "parsefu.h"
# include "mylib.h"
# include "stringutil.h"

# define	EO_ARRAY(a)			&a[DIM(a)]

	  FILE 		*yyfp;
	  int  		yylineno = 1;

static char 		yytext[512];
static int  		_inbrackets,_currfindex;
static signed char	*fpars = _allfuncs[0].fpars;
static int		_checktypes = 0;
static char		*Rename(char *s);

struct mcode {
	unsigned char	but,shift,click;
};

typedef union yystype {
	char 	*string;
	struct mp *mp;
	int		flags;
	char		c;
	struct cmd *cp;
	struct func {
		char	*name,*par;
	} f;
	long		number;
	struct mcode mousecode;
} _YYSTYPE;

extern void inskey(char *keystring, char *cmdname, int i);

static int parse_flags(char *s);
static int lookup_type(char *typename);
static int close_type(void);
static int new_type(int typ,char *s);
struct llist *make_list(struct llist* list,char *name);

#define alloc(len)	malloc(len)

#define YYSTYPE _YYSTYPE


#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#define	YYACCEPT	return(0)
#define	YYABORT	return(1)
#define	YYERROR	goto yyerrlab
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>



#define	YYFINAL		102
#define	YYFLAG		-32767
#define	YYNTBASE	32

#define YYTRANSLATE(x) ((unsigned)(x) <= 278 ? yytranslate[x] : 61)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    25,
    26,     2,    30,    29,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    31,     2,     2,
    28,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    24,     2,    27,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23
};

static const short yyrline[] = {     0,
    73,    74,    76,    91,    92,   106,   107,   108,   111,   113,
   116,   117,   119,   129,   133,   140,   142,   143,   145,   146,
   148,   149,   151,   152,   154,   155,   157,   158,   166,   167,
   177,   178,   189,   190,   191,   193,   194,   198,   199,   200,
   202,   203,   205,   212,   213,   219,   224,   224,   226,   229,
   230,   239,   240,   241,   246,   247
};

static const char * const yytname[] = {     0,
"error","$illegal.","MOUSE","_ICON","ID","CHAR","IDENT","FORM","_MENU","FLAG",
"KEY","CMD","STRING","NUMBER","DOTDOT","T_SHIFT","T_CTRL","T_ALT","T_LEFT","T_RIGHT",
"T_MIDDLE","ENUM","OPTION","'{'","'('","')'","'}'","'='","','","'+'",
"':'","script"
};

static const short yyr1[] = {     0,
    32,    32,    33,    33,    33,    33,    33,    33,    34,    34,
    35,    35,    36,    37,    37,    38,    39,    39,    40,    40,
    41,    41,    42,    42,    43,    43,    44,    44,    45,    45,
    46,    46,    47,    47,    47,    48,    48,    49,    49,    49,
    50,    50,    51,    52,    53,    54,    52,    55,    55,    56,
    56,    58,    59,    57,    60,    60
};

static const short yyr2[] = {     0,
     1,     2,     9,     4,     5,     4,     4,     3,     3,     3,
     0,     2,     4,     1,     3,     3,     0,     3,     0,     1,
     1,     0,     1,     1,     1,     0,     0,     4,     0,     5,
     0,     4,     1,     1,     1,     0,     3,     1,     1,     1,
     0,     3,     5,     0,     0,     0,     9,     1,     2,     0,
     2,     0,     0,     6,     0,     1
};

static const short yydefact[] = {     0,
     0,     0,     0,     0,     0,     0,     0,     0,     1,    11,
    31,    29,     0,    44,     0,     0,     0,     2,     0,     0,
    11,     0,     0,     0,     0,    27,     0,     0,     0,     9,
    10,     0,     8,    12,    36,     7,     0,     6,    23,    24,
     0,     0,    50,     4,     0,     0,    33,    34,    35,     0,
    41,    31,     0,     0,     5,     0,    22,    50,     0,    14,
    13,    36,    38,    39,    40,     0,     0,    32,    29,    27,
    52,    21,     0,    51,     0,     0,    37,    41,     0,    30,
    28,    26,    45,     0,    15,    42,     0,    25,    53,     0,
     0,    43,     0,    48,    46,     3,    54,    49,    44,    47,
     0,     0
};

static const short yydefgoto[] = {     8,
     9,    10,    20,    21,    59,    -1,    -1,    -1,    73,    41,
    89,    42,    25,    23,    50,    51,    66,    67,    52,    28,
    90,    99,    95,    57,    58,    82,    93,    -1
};

static const short yypact[] = {    -1,
   -18,   -17,    10,    -6,    -4,    21,    22,     1,-32767,    23,
    28,    29,    14,    32,    33,    17,    18,-32767,    15,    19,
    23,    31,    20,    38,    24,     2,    25,    26,    27,-32767,
-32767,    34,-32767,-32767,     9,-32767,    42,-32767,-32767,-32767,
    30,    35,    43,-32767,     2,    36,-32767,-32767,-32767,    37,
    13,    28,    47,    48,-32767,    39,    44,    43,   -10,-32767,
-32767,     9,-32767,-32767,-32767,    40,    41,-32767,    29,     2,
-32767,-32767,    45,-32767,    46,     2,-32767,    13,    49,-32767,
-32767,     2,-32767,    55,-32767,-32767,    50,-32767,-32767,    58,
    51,-32767,    53,    58,-32767,-32767,-32767,-32767,    32,-32767,
    60,-32767
};

static const short yypgoto[] = {-32767,
    63,-32767,    54,-32767,-32767,-32767,-32767,-32767,-32767,   -45,
-32767,    -9,     3,    52,-32767,    12,-32767,     4,-32767,   -22,
-32767,-32767,   -14,    56,-32767,-32767,-32767,-32767
};


#define	YYLAST		114


static const short yytable[] = {    60,
   101,     1,     2,     1,     2,    11,    12,     3,    39,     3,
     4,     5,     4,     5,    40,    75,    13,    14,    76,    15,
     6,     7,     6,     7,    47,    48,    49,    16,    17,    19,
    85,    63,    64,    65,    22,    24,    88,    26,    27,    29,
    30,    31,    32,    35,    37,    33,    36,    46,    53,    56,
    38,    45,    44,    69,    70,    43,    72,    54,    84,   102,
    81,    55,    87,    71,    61,    79,    62,    91,    94,    78,
    18,    80,    83,    77,    34,    92,   100,    96,    97,    98,
     0,    86,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    68,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    74
};

static const short yycheck[] = {    45,
     0,     3,     4,     3,     4,    24,    24,     9,     7,     9,
    12,    13,    12,    13,    13,    26,     7,    24,    29,    24,
    22,    23,    22,    23,    16,    17,    18,     7,     7,     7,
    76,    19,    20,    21,     7,     7,    82,    24,     7,     7,
    24,    24,    28,    13,     7,    27,    27,    14,     7,     7,
    27,    25,    27,     7,     7,    31,    13,    28,    13,     0,
    70,    27,    14,    25,    29,    25,    30,    13,    11,    30,
     8,    69,    28,    62,    21,    26,    99,    27,    26,    94,
    -1,    78,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    52,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    58
};
#define YYPURE 1

#line 1 "D:\LIB\B.SIM"

/* Skeleton output parser for bison,
   copyright (C) 1984 Bob Corbett and Richard Stallman

*/

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define bcopy(b1, b2, length) memcpy((b2),(b1),(length))

#if	1	/*---------------------------------------------------------*/

#if !defined(__STDIO_H)
#include <stdio.h>
#endif
#if !defined(__STRING_H)
#include <string.h>
#endif
#if !defined(__STDLIB_H)
#include <stdlib.h>
#endif

void *alloca (unsigned size);
void yyerror(char *s, ...);
int yylex(void );

#endif	/* TURBO -------------------------------------------------------*/

#define yyerrok	    (yyerrstatus = 0)
#define yyclearin	    (yychar = YYEMPTY)
#define YYEMPTY	    -2
#define YYEOF		    0
#define YYFAIL 	    goto yyerrlab;

#define YYTERROR	    1

#ifndef YYIMPURE
#define YYLEX		    yylex()
#endif

#ifndef YYPURE
#define YYLEX		    yylex(&yylval, &yylloc)
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYIMPURE

int	   yychar;			  /*  the lookahead symbol			  */
YYSTYPE yylval;			  /*  the semantic value of the		  */
						  /*  lookahead symbol				  */

YYLTYPE yylloc;			  /*  location data for the lookahead	  */
						  /*  symbol						  */

int yynerr;				  /*  number of parse errors so far	  */

#ifdef YYDEBUG
int yydebug = 0;			  /*  nonzero means print parse trace	  */
#endif

#endif  /* YYIMPURE */


/*  YYMAXDEPTH indicates the initial size of the parser's stacks 	  */

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 200
#endif

/*  YYMAXLIMIT is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).	*/

#ifndef YYMAXLIMIT
#define YYMAXLIMIT 10000
#endif


#line 80 "D:\LIB\B.SIM"
int yyparse(void )
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
#if defined(YYLSP_NEEDED) || defined(yyoverflow)
  YYLTYPE *yylsp;
#endif
  int yyerrstatus;	    /*  number of tokens to shift before error messages enabled */
  int yychar1; 	    /*  lookahead token as an internal (translated) token number */

  short yyssa[YYMAXDEPTH];	  /*  the state stack				  */
  YYSTYPE yyvsa[YYMAXDEPTH];	  /*  the semantic value stack		  */

  short *yyss = yyssa;		  /*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa; 	  /*  to allow yyoverflow to reallocate them elsewhere */
#if defined(YYLSP_NEEDED) || defined(yyoverflow)
  YYLTYPE yylsa[YYMAXDEPTH];	  /*  the location stack 			  */
  YYLTYPE *yyls = yylsa;
#endif

  int yymaxdepth = YYMAXDEPTH;

#ifndef YYPURE
  int yychar;
  YYSTYPE yylval;
  YYLTYPE yylloc;
#endif

#ifdef YYDEBUG
  extern void YYtrace(char *s, ...);
  extern int yydebug;
#endif


  YYSTYPE yyval;			  /*  the variable used to return		  */
						  /*  semantic values from the action	  */
						  /*  routines 					  */

  int yylen;

#ifdef YYDEBUG
  if (yydebug)
    YYtrace("Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerr = 0;
  yychar = YYEMPTY; 		  /* Cause a token to be read.  */

  /* Initialize stack pointers.
	Waste one element of value and location stack
	so that they stay on the same level as the state stack.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#if defined(YYLSP_NEEDED) || defined(yyoverflow)
  yylsp = yyls;
#endif

/* Push a new state, which is found in	yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yymaxdepth - 1)
    {
	 /* Give user a chance to reallocate the stack */
	 /* Use copies of these so that the &'s don't force the real ones into memory. */
	 YYSTYPE *yyvs1 = yyvs;
#if defined(YYLSP_NEEDED) || defined(yyoverflow)
	 YYLTYPE *yyls1 = yyls;
#endif
	 short *yyss1 = yyss;

	 /* Get the current used size of the three stacks, in elements.	*/
	 int size = yyssp - yyss + 1;

#ifdef yyoverflow
	 /* Each stack pointer address is followed by the size of
	    the data in use in that stack, in bytes.  */
	 yyoverflow("parser stack overflow",
			  &yyss1, size * sizeof (*yyssp),
			  &yyvs1, size * sizeof (*yyvsp),
			  &yyls1, size * sizeof (*yylsp),
			  &yymaxdepth);

	 yyss = yyss1; yyvs = yyvs1; yyls = yyls1;
#else /* no yyoverflow */
	 /* Extend the stack our own way.	*/
	 if (yymaxdepth >= YYMAXLIMIT)
	   yyerror("parser stack overflow");
	 yymaxdepth *= 2;
	 if (yymaxdepth > YYMAXLIMIT)
	   yymaxdepth = YYMAXLIMIT;
	 yyss = (short *) alloca (yymaxdepth * sizeof (*yyssp));
	 bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
	 yyvs = (YYSTYPE *) alloca (yymaxdepth * sizeof (*yyvsp));
	 bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
	 yyls = (YYLTYPE *) alloca (yymaxdepth * sizeof (*yylsp));
	 bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

	 yyssp = yyss + size - 1;
	 yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
	 yylsp = yyls + size - 1;
#endif

#ifdef YYDEBUG
	 if (yydebug)
	   YYtrace("Stack size increased to %d\n", yymaxdepth);
#endif

	 if (yyssp >= yyss + yymaxdepth - 1)
	   YYERROR;
    }

#ifdef YYDEBUG
  if (yydebug)
    YYtrace("Entering state %d\n", yystate);
#endif

/* Do appropriate processing given the current state.	*/
/* Read a lookahead token if we need one and don't already have one.	*/
yyresume:

  /* First try to decide what to do without reference to lookahead token.	*/

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
	or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#ifdef YYDEBUG
	 if (yydebug)
	   YYtrace("Reading a token: ");
#endif
	 yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)			  /* This means end of input. */
    {
	 yychar1 = 0;
	 yychar = YYEOF;		  /* Don't call YYLEX any more */

#ifdef YYDEBUG
	 if (yydebug)
	   YYtrace("Now at end of input.\n");
#endif
    }
  else
    {
	 yychar1 = YYTRANSLATE(yychar);

#ifdef YYDEBUG
	 if (yydebug && yychar1 >= 0 && yytname[yychar1] != (char *) 0)
	   YYtrace("Next token is %d (%s)\n", yychar, yytname[yychar1]);
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
	Negative => reduce, -yyn is rule number.
	Positive => shift, yyn is new state.
	  New state is final state => don't bother to shift,
	  just return success.
	0, or most negative number => error.  */

  if (yyn < 0)
    {
	 if (yyn == YYFLAG)
	   goto yyerrlab;
	 yyn = -yyn;
	 goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#ifdef YYDEBUG
  if (yydebug && yychar1 >= 0 && yytname[yychar1] != (char *) 0)
    YYtrace("Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.	yyn is the number of a rule to reduce with.	*/
yyreduce:
  yylen = yyr2[yyn];
  yyval = yyvsp[1-yylen]; /* implement default value of the action */

#ifdef YYDEBUG
  if (yydebug)
    {
	 if (yylen == 1)
	   YYtrace( "Reducing 1 value via line %d, ",
			  yyrline[yyn]);
	 else
	   YYtrace( "Reducing %d values via line %d, ",
			  yylen, yyrline[yyn]);
    }
#endif


  switch (yyn) {

case 3:
#line 77 ".\funcs.y"
{
				if (_currfunc >= MAXFUNCS)
					yyerror("too many function definitions");

				_allfuncs[_currfunc].ident = Rename(yyvsp[-8].string);
				_allfuncs[_currfunc].name  = yyvsp[-6].string;
				_allfuncs[_currfunc].plist = (struct llist*)yyvsp[-4].string;
				_allfuncs[_currfunc].flags = parse_flags(yyvsp[-2].string);
				_allfuncs[_currfunc].comment = yyvsp[-1].string;
				_allfuncs[_currfunc].idx = _currfindex;
				_currfunc++;
				_currfindex++;
				fpars = _allfuncs[_currfunc].fpars;
			;
    break;}
case 5:
#line 93 ".\funcs.y"
{	struct mp *mp = yyvsp[-1].mp;
	
				while (mp != 0) {
					if (_currmenu >= MAXFUNCS)
						yyerror("too many menu definitions");
	
					_allmenus[_currmenu].menu     = mp->menu;
					_allmenus[_currmenu].cmdname  = mp->cmdname;
					_allmenus[_currmenu].title    = yyvsp[-3].string;
					_currmenu++;
					mp = mp->next;
				}
			;
    break;}
case 8:
#line 109 ".\funcs.y"
{ close_type(); ;
    break;}
case 9:
#line 112 ".\funcs.y"
{	new_type(OT_ENUM,yyvsp[-1].string);	;
    break;}
case 10:
#line 114 ".\funcs.y"
{	new_type(OT_OPTION,yyvsp[-1].string);	;
    break;}
case 13:
#line 119 ".\funcs.y"
{	
				if (_currenelem >= MAXENUMS) {
					yyerror("too many enumerators");
					return 0;
				}
				_localenelemtab[_currenelem].te_name = yyvsp[-3].string;
				_localenelemtab[_currenelem].te_val = yyvsp[-1].number;
				_currenelem++;
			;
    break;}
case 14:
#line 130 ".\funcs.y"
{	yyval.string = (char *)make_list((struct llist*)0,yyvsp[0].string);
				lookup_type(yyvsp[0].string);
			;
    break;}
case 15:
#line 135 ".\funcs.y"
{
				yyval.string = (char *)make_list((struct llist*)yyvsp[-2].string,yyvsp[0].string);	
				lookup_type(yyvsp[0].string);
			;
    break;}
case 16:
#line 140 ".\funcs.y"
{ yyval.string = yyvsp[-1].string; ;
    break;}
case 17:
#line 142 ".\funcs.y"
{ yyval.string = 0; ;
    break;}
case 18:
#line 143 ".\funcs.y"
{ yyval.string = yyvsp[-1].string; ;
    break;}
case 19:
#line 145 ".\funcs.y"
{ yyval.string = 0; ;
    break;}
case 20:
#line 146 ".\funcs.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 21:
#line 148 ".\funcs.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 22:
#line 149 ".\funcs.y"
{ yyval.string = 0; ;
    break;}
case 23:
#line 151 ".\funcs.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 24:
#line 152 ".\funcs.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 25:
#line 154 ".\funcs.y"
{ yyval.string = yyvsp[0].string; ;
    break;}
case 26:
#line 155 ".\funcs.y"
{ yyval.string = 0; ;
    break;}
case 27:
#line 157 ".\funcs.y"
{ yyval.mp = 0; ;
    break;}
case 28:
#line 159 ".\funcs.y"
{
				yyval.mp = (struct mp *)alloc(sizeof (struct mp));
				yyval.mp->next = yyvsp[0].mp;
				yyval.mp->menu = yyvsp[-3].string;
				yyval.mp->cmdname = yyvsp[-1].string;
			;
    break;}
case 30:
#line 168 ".\funcs.y"
{
				_allicons[_curric].cmdname = yyvsp[-4].string;
				_allicons[_curric].flagname = yyvsp[-3].string;
				_allicons[_curric].idname = yyvsp[-2].string;
				_allicons[_curric].dropname = yyvsp[-1].string;
				if (++_curric >= MAXICON) 
					yyerror("too many icon bindings");
			;
    break;}
case 32:
#line 179 ".\funcs.y"
{
				_allmice[_currmouse].cmdname  = yyvsp[-3].string;
				_allmice[_currmouse].flagname = yyvsp[-2].string;
				_allmice[_currmouse].but   	= yyvsp[-1].mousecode.but;
				_allmice[_currmouse].shift 	= yyvsp[-1].mousecode.shift;
				_allmice[_currmouse].click 	= yyvsp[-1].mousecode.click;
				if (++_currmouse >= MAXMOUSE) 
					yyerror("too many mouse bindings");
			;
    break;}
case 33:
#line 189 ".\funcs.y"
{ yyval.number = K_SHIFT; ;
    break;}
case 34:
#line 190 ".\funcs.y"
{ yyval.number = K_CONTROL; ;
    break;}
case 35:
#line 191 ".\funcs.y"
{ yyval.number = K_ALTERNATE; ;
    break;}
case 36:
#line 193 ".\funcs.y"
{	yyval.number = 0; ;
    break;}
case 37:
#line 194 ".\funcs.y"
{ 
				yyval.number = yyvsp[-2].number | yyvsp[0].number; 
			;
    break;}
case 38:
#line 198 ".\funcs.y"
{ yyval.number = MBUT_L; ;
    break;}
case 39:
#line 199 ".\funcs.y"
{ yyval.number = MBUT_R; ;
    break;}
case 40:
#line 200 ".\funcs.y"
{ yyval.number = MBUT_M; ;
    break;}
case 41:
#line 202 ".\funcs.y"
{	yyval.number = 0; ;
    break;}
case 42:
#line 203 ".\funcs.y"
{ yyval.number = yyvsp[-2].number | yyvsp[0].number; ;
    break;}
case 43:
#line 206 ".\funcs.y"
{
				yyval.mousecode.shift = (int)(yyvsp[-4].number >> 8L);
				yyval.mousecode.but   = yyvsp[-3].number;
				yyval.mousecode.click = yyvsp[-1].number;
			;
    break;}
case 45:
#line 214 ".\funcs.y"
{
				hash_comment(_currcmd,yyvsp[-1].string);
				_allcmd[_currcmd] = yyvsp[-2].cp;
				_allcmd[_currcmd]->cmdname = yyvsp[-4].string;
			;
    break;}
case 46:
#line 219 ".\funcs.y"
{
				if (++_currcmd >= MAXCMDS)
					yyerror("too many commands");
			;
    break;}
case 48:
#line 225 ".\funcs.y"
{ inskey(yyvsp[0].string,_allcmd[_currcmd]->cmdname,_currcmd); ;
    break;}
case 49:
#line 227 ".\funcs.y"
{ inskey(yyvsp[-1].string,_allcmd[_currcmd]->cmdname,_currcmd); ;
    break;}
case 50:
#line 229 ".\funcs.y"
{ yyval.cp = 0; ;
    break;}
case 51:
#line 231 ".\funcs.y"
{
				yyval.cp = (struct cmd *)alloc(sizeof (struct cmd));
				yyval.cp->cmdname = 0;
				yyval.cp->next = yyvsp[0].cp;
				yyval.cp->name = yyvsp[-1].f.name;
				yyval.cp->par  = yyvsp[-1].f.par;
			;
    break;}
case 52:
#line 239 ".\funcs.y"
{ _inbrackets =  1; ;
    break;}
case 53:
#line 240 ".\funcs.y"
{ _inbrackets = 0; ;
    break;}
case 54:
#line 241 ".\funcs.y"
{
				yyval.f.name = yyvsp[-5].string;
				yyval.f.par	= yyvsp[-2].string;
			;
    break;}
case 55:
#line 246 ".\funcs.y"
{ yyval.string  = "";			;
    break;}
case 56:
#line 247 ".\funcs.y"
{ yyval.string  = yyvsp[0].string;	;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 328 "D:\LIB\B.SIM"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#ifdef YYDEBUG
  if (yydebug)
    {
	 short *ssp1 = yyss - 1;
	 YYtrace( "state stack now");
	 while (ssp1 != yyssp)
	   YYtrace( " %d", *++ssp1);
	 YYtrace( "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
	 yylsp->first_line = yylloc.first_line;
	 yylsp->first_column = yylloc.first_column;
	 yylsp->last_line = (yylsp-1)->last_line;
	 yylsp->last_column = (yylsp-1)->last_column;
	 yylsp->text = 0;
    }
  else
    {
	 yylsp->last_line = (yylsp+yylen-1)->last_line;
	 yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
	Determine what state that goes to,
	based on the state we popped back to
	and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
	 ++yynerr;
	 yyerror("Syntax error");
    }

  if (yyerrstatus == 3)
    {
	 /* if just tried and failed to reuse lookahead token after an error, discard it.  */

	 /* return failure if at end of input */
	 if (yychar == YYEOF) {
	   yyerror("cannot recover from earlier errors, goodbye");
	   YYABORT;
	 }

#ifdef YYDEBUG
	 if (yydebug && yychar1 >= 0 && yytname[yychar1] != (char *) 0)
	   YYtrace("Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

	 yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
	after shifting the error token.  */

  yyerrstatus = 3;			  /* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:	/* current state does not do anything special for the error token. */

/* #if 0 !!!!!!!!!!!!!!!!!!!!!!*/
  /* This is wrong; only states that explicitly want error tokens
	should shift them.	*/
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.	Otherwise pop it.*/
  if (yyn) goto yydefault;
/* #endif !!!!!!!!!!!!!*/

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYERROR;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#ifdef YYDEBUG
  if (yydebug)
    {
	 short *ssp1 = yyss - 1;
	 YYtrace( "Error: state stack now");
	 while (ssp1 != yyssp)
	   YYtrace( " %d", *++ssp1);
	 YYtrace( "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
	 if (yyn == YYFLAG)
	   goto yyerrpop;
	 yyn = -yyn;
	 goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#ifdef YYDEBUG
  if (yydebug)
    YYtrace("Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 249 ".\funcs.y"


void yyerror(char *msg, ...) 
{
	va_list ap;
	char buf[128];
	extern int _excode;

	va_start(ap, msg);
	vsprintf(buf, msg, ap); 
	va_end(ap);
	fprintf(stderr,"%s in line %d\n",buf,yylineno);
	_excode = 1;
}

/*---------------------------------*/
/* yylex()					*/
/* handmade scanner				*/
/*---------------------------------*/
#define	YYGETC()	fgetc(yyfp)
#define	YLEOF	EOF
static int lookahead;
static int  yylen;

static iscident(int c) {
	return c == '-' || c == '|' || c == '_' || isalnum(c);
}

static int isnospace(int c) {
	return c != EOF && c != '\n' && c != '\r' && c != ' ' && c != '\t';
}

static int isnoquote(int c) {
	return c != '"';
}

static int isnoclose(int c) {
	return c != '>';
}

static int isnumber(int c) {
	return (c >= '0' && c <= '9') || c == 'x' || c == 'X';
}

static void yygets(int c, int (*f)(int c)) 
{
	yylen = 0;
	do {
		yytext[yylen] = c;
		yylen++;
		c = YYGETC();
	} while ((*f)(c));
	yytext[yylen] = 0;
	lookahead     = c;
}

#define S_MOUSE		1

int yylex(void )
{	int c;
	long base,sign = 1;
	char *estart,*s;
	static int state = 0;

	for (;;) {
		if (lookahead) {
			c = lookahead;
			lookahead = 0;
		} else c = YYGETC();
		if (c == YLEOF) return 0;
		if (c == '\n')  yylineno++;
		else switch (c) {
			case ' ': case '\r': case '\t':
				continue;
			case '}':
				state = 0;
			case ',': case '(': case ')': case ':': case ';':
			case '{': case '=': case '!': case '+':
			case '*':
				return c;
			case '#':
				while(c != '\n') {
					if ((c = YYGETC()) == YLEOF)
						return 0;
				}
				yylineno++;
				continue;
			case '.':
				if ((c = YYGETC()) == '.')
					return DOTDOT;
				lookahead = '.';
				break;
			case '-':
				if (_inbrackets)
					goto getid;
				c = YYGETC();
				if (!isnumber(c)) {
					lookahead = c;
					return DOTDOT;
				}
				sign = -1;
				/* drop through */
			case '0': 
			case '1': case '2': case '3': 
			case '4': case '5': case '6': 
			case '7': case '8': case '9':
				yygets(c,isnumber);
				if (_inbrackets)
					goto gotid;
				yylval.number = sign * strtol(yytext,(char **)0,0);
				return NUMBER;
			case '[':
				yygets(c,isnospace);
				yylval.string = stralloc(yytext);
				return KEY;
			case '<':
				yygets(c,isnoclose);
				yylval.string = stralloc(yytext+1);
				lookahead = 0;
				return STRING;
			case '"': 
				yygets(c,isnoquote);
				yylval.string = stralloc(yytext+1);
				lookahead = 0;
				return STRING;
			default:
				if (c == '-' || iscident(c)) {
getid:				yygets(c,iscident);
gotid:			
					if (state == S_MOUSE) {
						if (strcmp(yytext,"Alt") == 0)
							return T_ALT;
						if (strcmp(yytext,"Shift") == 0)
							return T_SHIFT;
						if (strcmp(yytext,"Ctrl") == 0)
							return T_CTRL;
						if (strcmp(yytext,"Right") == 0)
							return T_RIGHT;
						if (strcmp(yytext,"Middle") == 0)
							return T_MIDDLE;
						if (strcmp(yytext,"Left") == 0)
							return T_LEFT;
					}
					if (strcmp(yytext,"FLAG") == 0)
						return FLAG;
					if (strcmp(yytext,"MENU") == 0)
						return _MENU;
					if (strcmp(yytext,"ID") == 0)
						return ID;
					if (strcmp(yytext,"CMD") == 0)
						return CMD;
					if (strcmp(yytext,"ICON") == 0)
						return _ICON;
					if (strcmp(yytext,"enum") == 0)
						return ENUM;
					if (strcmp(yytext,"option") == 0)
						return OPTION;
					if (strcmp(yytext,"MOUSE") == 0) {
						state = S_MOUSE;
						return MOUSE;
					}
					yylval.string = stralloc(yytext);
					return IDENT;
				}
		}
	}
}

/*--------------------------------------------------------------------------
 * make_list()
 */
struct llist *make_list(struct llist* list,char *name)
{
	struct llist *lp,*lp2;

	for (lp2 = list; lp2 && lp2->next; lp2 = lp2->next)
		;

	if ((lp = malloc(sizeof *lp)) == 0 ||
	    (lp->name = stralloc(name)) == 0) {
		fprintf(stderr,"no mem\n");
		exit(1);
	}
	lp->next = 0;

	if (lp2) {
		lp2->next = lp;
		return list;
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * new_type()
 */
static int new_type(int typ,char *s)
{
	if (_currtype >= MAXTYPES) {
		yyerror("too many type definitions");
		return 0;
	}
	if (strcmp(s,"DIALOGOPT") == 0)
		_typetab[_currtype].ot_flags = OF_ELIPSIS;
	else
		_typetab[_currtype].ot_flags = 0;
	_typetab[_currtype].ot_typ = typ;
	_typetab[_currtype].ot_id = s;
	_typetab[_currtype].ot_idx = _currenelem;
	return 1;
}

/*--------------------------------------------------------------------------
 * close_type()
 */
static int close_type(void)
{
	_typetab[_currtype].ot_nelem = 
		_currenelem - _typetab[_currtype].ot_idx;
	return ++_currtype;
}

/*--------------------------------------------------------------------------
 * parse_flags()
 */
static int parse_flags(char *s)
{
	int flags = 0;

	while (*s) {
		switch (*s) {
			case 'C':	flags |= EW_NEEDSCURRF; break;
			case 'm':	flags |= EW_MODIFY; 	  break;
			case 'c':	flags |= EW_CCASH; 	  break;
			case 'B':	flags |= EW_NEEDSBLK;   break;
			case 'F':	flags |= EW_HASFORM; 	  break;
			case 'U': flags |= EW_UNDOFLSH;	  break;
			case 'N': flags |= EW_NOCASH;	  break;
			case 'M': flags |= EW_MULTI;	  break;
			case '*': _fdfunctab[_nfdfuncs++] = _currfunc; break;
			case '-': break;
			default : yyerror("invalid Flag %c",*s);
		}
		s++;
	}
	return flags;
}

/*--------------------------------------------------------------------------
 * find_type()
 */
static int find_type(char *typename)
{
	int i;

	for (i = 0; i < _currtype; i++) {
		if (strcmp(_typetab[i].ot_id,typename) == 0) {
			return PAR_USER + i;
		}
	}


	yyerror("illegal parameter type %s",typename);
	return PAR_VOID;
}

/*--------------------------------------------------------------------------
 * lookup_type()
 */
static int lookup_type(char *t)
{
	char typename[128];

	if (!_checktypes)
		return PAR_VOID;

	strcpy(typename,t);
	strtok(typename,":");

	if (strcmp(typename,"int") == 0 ||
	    strcmp(typename,"bool") == 0) {
		return *fpars++ = PAR_INT;
	}
	if (strcmp(typename,"string") == 0) {
		return *fpars++ = PAR_STRING;
	}
	return *fpars++ = find_type(typename);
}

/*---------------------------------*/
/* usage()					*/
/*---------------------------------*/
static char *_name;
static void usage(void )
{
	fprintf(stderr,
	"usage: %s [ -v ] [ -F FUNCDEFS ] [ -P PKSEDSRC ] [ -C CMDSRC ] [ DESCFILE ]\n",
			_name);
	exit(2);
}

/*--------------------------------------------------------------------------
 * Upcase()
 */
static char Upcase(char c)
{
	return (c >= 'a' && c <= 'z') ?
		(c - 'a' + 'A') : c;
}

/*--------------------------------------------------------------------------
 * Rename()
 */
static char *Rename(char *s)
{
	char *D = s, *d = s;

	while(*s) {
		*d++ = Upcase(*s++);
		while(*s && *s != '-') {
			*d++ = *s++;
		}
		if (*s) {
			s++;
		}
	}
	*d = 0;
	return D;
}

/*---------------------------------*/
/* main()						*/
/*---------------------------------*/
main(int ac, char **av)
{	int  i,c;
	int  printkomma = 0;
	char *cmdtable   = 0,
	     *pksedtable = 0,
		*helpfile   = 0,
		*definefile = 0,
		*helpstringfile = 0,
		*helpstrings = 0,
		*functable  = 0;
	yyfp = stdin;

	_name = (av[0][0]) ? av[0] : "funcs";

	while ((c = getopt(ac,av,"o:d:D:h:H:c:C:p:P:f:F:vs:S:")) != EOF) {
		switch(c) {
		case 'h':
		case 'H': _checktypes = 1; helpfile = optarg; break;
		case 'd':
		case 'D': definefile = optarg; break;
		case 'p':
		case 'P': pksedtable = optarg; break;
		case 'c':
		case 'C': cmdtable = optarg; break;
		case 'f':
		case 'F': _checktypes = 1; functable = optarg; break;
#ifdef YYDEBUG
		case 'v': yydebug  = 1; break;
#endif
		case 'o': printkomma = 1;
		case 's':
		case 'S': helpstringfile = optarg; break;
		default:
			usage();
		}
	}
	
	if (optind < ac) {
		if ((yyfp = fopen(av[optind],"r")) == 0) {
			yyerror("cannot open %s",av[optind]);
			return 1;
		}
	}
     setvbuf(yyfp,(char *) 0, _IOFBF, 8192);
	
	yyparse();

	if (cmdtable && printcmdtable(cmdtable))
		return 1;

	if (pksedtable && printfuncs(pksedtable))
		return 1;

	if (functable && printfunctable(functable))
		return 1;
	
	if (helpfile && printhelp(helpfile))
		return 1;

	if (definefile && printfuncdefines(definefile))
		return 1;

	if (helpstrings && printhelpstrings(helpstrings,1))
		return 1;

	if (helpstringfile && printhelpstrings(helpstringfile,printkomma))
		return 1;

	return 0;
}

