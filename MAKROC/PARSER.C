
/*  A Bison parser, made from parser.y  */

#define	T_TSTRING	258
#define	T_TINT	259
#define	T_TCHAR	260
#define	T_TLONG	261
#define	T_IDENT	262
#define	T_NUM	263
#define	T_STRING	264
#define	T_OCTNUM	265
#define	T_CONST	266
#define	T_FUNC	267
#define	T_MACRO	268
#define	T_DOTDOT	269
#define	T_IF	270
#define	T_ELIF	271
#define	T_ELSE	272
#define	T_WHILE	273
#define	T_GOTO	274
#define	T_BRAEQ	275
#define	T_LOCAL	276
#define	T_SWITCH	277
#define	T_BREAK	278
#define	T_CONTINUE	279
#define	T_RETURN	280
#define	T_NUMVAR	281
#define	T_STRINGVAR	282
#define	T_NMATCH	283
#define	T_SHIFT	284
#define	T_CTRL	285
#define	T_ALT	286
#define	T_LEFT	287
#define	T_MIDDLE	288
#define	T_RIGHT	289
#define	T_MENUPOPUP	290
#define	T_MENUSEPERATOR	291
#define	T_MENUBREAK	292
#define	T_MENUBARBREAK	293
#define	T_ASSIGN	294
#define	T_NE	295
#define	T_EQ	296
#define	T_OR	297
#define	T_AND	298
#define	T_DEFMACRO	299
#define	T_DEFICON	300
#define	T_DEFALTPATH	301
#define	T_DEFCOMPILER	302
#define	T_DEFDOCTYPES	303
#define	T_DEFMENUS	304
#define	T_DEFKEYS	305
#define	T_DEFMOUSE	306
#define	T_DEFTAGS	307
#define	T_DEFABBREVS	308
#define	T_DEFBRACKETS	309
#define	T_DEFESCAPES	310
#define	T_CMDSEQPREFIX	311

#line 1 "parser.y"

/*
 * PARSER.Y
 *
 * Grammar
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 18.05.1991
 *									
 */
#line 39 "parser.y"


# include	<windows.h>
# include	<stdio.h>
# include	"edfuncs.h"
# include "scanner.h"
# include	"test.h"
# include	"binop.h"
# include	"pkscc.h"
# include "sym.h"

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

extern 		int mac_validatename(unsigned char *name);
static 		void endmacro(void);
static 		void startmacro(void);
			void freeval(struct typedval *v);
static 		void freeitem(char **p);
static 		char *gotolabel(char *prefix, int level, int bratyp);
static		int makelabel(char *prefix,int level);
static		void killabel(char *prefix, int level);
static 		void TestOp(void);
static 		void CloseTestOp(unsigned char type);
static 		int SetOverride(char *s);
static 		void BinOp1(unsigned char type);
static unsigned char PushVal(struct typedval *vp);
static void 	PushParam(struct typedval *vp);

static 		char *lstartid  = "%ls%",
			     *lendid    = "%le%",
				*lreturnid = "%rt%",
				*iffailid  = "%if%",
				*ifdoneid  = "%id%";

int			vname_count;

extern 		int	yyerrflg;
extern 		int	_bDefiningConst;

void 		Alert(char *s, ...);
int 			mac_insert(char *name, char *comment, unsigned char *data, int size);
unsigned char 	*AddComSeq(unsigned char *sp, unsigned char *spend,
				     	 unsigned char typ, long par);
int 			FuncIdx(void *ep);
int 			bind_key(KEYCODE keycode, MACROREFTYPE typ, MACROREFIDX idx, int augment);
int 			bind_mouse(MOUSECODE code, MACROREFTYPE typ, MACROREFIDX idx, int flags, int augment);
int 			MakeInternalSym(char *name, char ed_typ, long value);
int 			IsFormStart(void *ep,int parno);
int 			key_switchtotable(char *name);
int 			mouse_switchtotable(char *name);
void 		mouse_overridetable(void);
int 			mac_getbyname(char *name);
int 			mac_cmdseqbyname(char *name);
KEYCODE 		key2code(char *K, int control);
void 		key_destroytable(char *name);
struct typedval PushBinop(int opd_typ, struct typedval *v1, 
					struct typedval *v2);
void 		PushAssign(char *name, int typ, long val);
void 		PushCreateVariable(char *name, int typ, long val);

char		 	*GotoLabel(char *name, char *sp, char *spend, int typ);
int			MakeLabel(char *name, COM_GOTO *recp);
void 		CloseLabelList(void);
void 		KillLabel(char *name);
char			*LabelAutoName(char *prefix, int num);
int 			IsStringFunc(void *ep);
int 			IsStringType(unsigned char typ);
void 		menu_startdefine(char *szMenu);
int 			menu_addentry(char *pszString, int menutype, 
				MACROREFTYPE mactype, MACROREFTYPE macidx);

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



#define	YYFINAL		315
#define	YYFLAG		-32767
#define	YYNTBASE	76

#define YYTRANSLATE(x) ((unsigned)(x) <= 311 ? yytranslate[x] : 185)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    75,     2,     2,     2,    67,    60,     2,    73,
    74,    65,    62,    70,    63,     2,    66,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    68,    69,    57,
     2,    58,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    64,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    71,    61,    72,    59,     2,     2,     2,     2,
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
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
    56
};

static const short yyrline[] = {     0,
   141,   142,   146,   148,   149,   150,   151,   152,   153,   155,
   156,   254,   259,   261,   262,   263,   265,   271,   276,   277,
   279,   280,   281,   283,   287,   288,   289,   294,   295,   297,
   298,   303,   311,   313,   314,   315,   317,   325,   327,   328,
   330,   331,   332,   334,   335,   337,   346,   354,   356,   357,
   358,   360,   371,   373,   379,   381,   382,   386,   387,   389,
   390,   391,   395,   397,   398,   400,   408,   412,   414,   415,
   417,   417,   418,   426,   427,   429,   430,   434,   434,   437,
   438,   440,   441,   443,   452,   468,   469,   471,   472,   474,
   475,   477,   479,   480,   482,   483,   485,   492,   503,   504,
   505,   506,   506,   507,   508,   509,   510,   511,   512,   513,
   514,   515,   518,   522,   524,   525,   527,   528,   530,   531,
   533,   543,   544,   554,   555,   556,   557,   558,   559,   560,
   561,   562,   566,   566,   568,   568,   569,   570,   570,   572,
   572,   574,   575,   577,   577,   578,   580,   581,   582,   584,
   591,   598,   599,   600,   601,   602,   608,   609,   611,   612,
   614,   618,   622,   626,   628,   629,   640,   643,   646,   650,
   651,   653,   655,   656,   658,   663,   667,   672,   674,   677,
   685,   688,   693,   698,   699,   701,   702,   704,   710,   711,
   716,   718,   719,   721,   724,   727,   731,   748,   750,   752,
   753,   754,   755,   757,   765,   771,   776,   778,   783,   784
};

static const char * const yytname[] = {     0,
"error","$illegal.","T_TSTRING","T_TINT","T_TCHAR","T_TLONG","T_IDENT","T_NUM","T_STRING","T_OCTNUM",
"T_CONST","T_FUNC","T_MACRO","T_DOTDOT","T_IF","T_ELIF","T_ELSE","T_WHILE","T_GOTO","T_BRAEQ",
"T_LOCAL","T_SWITCH","T_BREAK","T_CONTINUE","T_RETURN","T_NUMVAR","T_STRINGVAR","T_NMATCH","T_SHIFT","T_CTRL",
"T_ALT","T_LEFT","T_MIDDLE","T_RIGHT","T_MENUPOPUP","T_MENUSEPERATOR","T_MENUBREAK","T_MENUBARBREAK","T_ASSIGN","T_NE",
"T_EQ","T_OR","T_AND","T_DEFMACRO","T_DEFICON","T_DEFALTPATH","T_DEFCOMPILER","T_DEFDOCTYPES","T_DEFMENUS","T_DEFKEYS",
"T_DEFMOUSE","T_DEFTAGS","T_DEFABBREVS","T_DEFBRACKETS","T_DEFESCAPES","T_CMDSEQPREFIX","'<'","'>'","'~'","'&'",
"'|'","'+'","'-'","'^'","'*'","'/'","'%'","':'","';'","','",
"'{'","'}'","'('","')'","'!'","resources"
};

static const short yyr1[] = {     0,
    76,    77,    76,    78,    78,    78,    78,    78,    78,    79,
    79,    81,    80,    82,    82,    82,    84,    83,    85,    85,
    86,    86,    86,    87,    87,    87,    87,    87,    87,    88,
    88,    90,    89,    91,    91,    91,    92,    92,    93,    93,
    94,    94,    94,    95,    95,    96,    98,    97,    99,    99,
    99,   100,   100,   101,   101,   103,   102,   104,   104,   105,
   105,   105,   106,   107,   107,   108,   108,   109,   110,   110,
   112,   113,   111,   114,   114,   115,   115,   117,   116,   118,
   118,   119,   119,   120,   121,   122,   122,   123,   123,   124,
   124,   125,   126,   126,   127,   127,   128,   128,   129,   129,
   130,   129,   129,   129,   129,   129,   129,   129,   129,   129,
   129,   129,   131,   132,   133,   133,   134,   134,   135,   136,
   135,   137,   137,   137,   137,   137,   137,   137,   137,   137,
   137,   137,   139,   140,   138,   141,   141,   142,   141,   144,
   143,   145,   146,   145,   147,   147,   149,   150,   148,   151,
   151,   152,   152,   152,   152,   152,   153,   153,   154,   154,
   155,   155,   155,   155,   156,   156,   157,   158,   159,   160,
   160,   161,   162,   162,   163,   164,   164,   166,   167,   165,
   169,   170,   168,   171,   171,   172,   172,   174,   173,   175,
   173,   176,   176,   177,   177,   177,   178,   178,   179,   180,
   180,   180,   180,   181,   181,   182,   183,   183,   184,   184
};

static const short yyr2[] = {     0,
     0,     0,     3,     1,     1,     1,     1,     1,     1,     0,
     2,     0,     6,     0,     1,     3,     0,     7,     0,     2,
     0,     1,     3,     1,     1,     1,     4,     1,     1,     0,
     3,     0,     7,     0,     1,     3,     4,     1,     0,     1,
     1,     1,     1,     1,     3,     3,     0,     7,     0,     1,
     3,     3,     1,     1,     1,     0,     3,     0,     3,     1,
     1,     1,     3,     0,     3,     5,     1,     3,     1,     3,
     0,     0,     5,     1,     1,     1,     1,     0,     6,     0,
     1,     1,     3,     2,     3,     0,     1,     0,     1,     0,
     1,     4,     0,     2,     0,     2,     4,     3,     1,     2,
     0,     3,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     2,     1,     3,     1,     2,     1,     0,
     2,     1,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     0,     0,     5,     1,     1,     0,     3,     0,
     3,     0,     0,     3,     1,     1,     0,     0,     4,     0,
     2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     2,     3,     3,     2,     0,
     1,     5,     0,     3,     2,     2,     2,     0,     0,     5,
     0,     0,     6,     0,     2,     0,     2,     0,     5,     0,
     5,     0,     1,     1,     3,     1,     2,     1,     3,     1,
     1,     1,     1,     1,     2,     1,     1,     1,     1,     3
};

static const short yydefact[] = {     2,
    88,    89,    71,     0,     0,     2,     6,     8,     7,     5,
     9,    86,     0,     4,     0,     0,    69,     0,    67,     0,
     0,     0,    10,     3,    87,     0,     0,     0,     0,    68,
    71,    74,    77,    76,    72,    75,     0,    63,     0,     0,
    12,    95,    85,     0,    47,    32,    70,     0,     0,    65,
    11,    14,   203,   201,   200,   202,     0,    95,     0,    80,
    10,    10,     0,     0,     0,     0,    15,   112,   190,   188,
     0,   178,     0,     0,     0,   170,   170,   170,    99,     0,
   110,     0,   101,   106,   107,   108,   105,   111,   109,   104,
   103,     0,     0,    96,     0,     0,    81,    82,     0,     0,
     0,   206,   208,   209,    73,   207,    66,    17,    13,    14,
   175,     0,     0,   133,   181,     0,   176,   177,     0,   171,
     0,     0,   169,   120,   113,   187,     0,    92,   100,    98,
     0,    79,     0,    84,    53,    60,    61,    62,     0,    50,
     0,    56,     0,    38,     0,    35,     0,     0,     0,    19,
    16,     0,     0,   140,     0,   179,   173,   167,   168,   165,
   161,   162,   120,   120,   114,   115,   122,     0,   117,   119,
   163,   120,   164,   102,    97,    83,    48,     0,     0,     0,
    58,    33,     0,    39,    41,    43,    42,    44,     0,   210,
     0,     0,   196,     0,   198,     0,   193,   194,     0,     0,
   138,   137,   134,   136,   147,   182,     0,     0,     0,     0,
   118,   120,     0,     0,     0,   190,   121,   120,   120,   120,
   120,   120,   120,   120,   120,   166,   123,    51,   204,     0,
    52,    54,    55,    57,    59,    36,    40,     0,     0,    46,
    20,     0,   191,     0,   197,   189,   140,     0,   142,     0,
   184,   180,   172,   173,     0,   116,   132,   199,   124,   125,
   126,   128,   127,   129,   130,   131,   205,    37,    45,    29,
    30,    24,    26,    25,    28,     0,    22,   195,   139,   135,
   146,   145,   141,   143,   148,   160,   159,     0,   183,   174,
     0,     0,    18,   140,   150,   185,     0,     0,    23,   144,
   158,   155,   154,   152,   153,   157,   149,     0,   156,    27,
    31,   151,     0,     0,     0
};

static const short yydefgoto[] = {    24,
     1,     6,    41,     7,    52,    66,    67,   150,   192,   276,
   277,   291,     8,    62,   145,   146,   238,   188,   189,   147,
     9,    61,   139,   140,   234,   141,   180,   142,   143,    10,
    21,    22,    11,    16,    17,    18,    48,    35,    80,    12,
    13,    96,    97,    98,    14,    26,    15,    -1,    81,    -1,
    57,    58,    82,   127,    83,   125,   165,   166,   167,   168,
   169,   202,   154,   248,   203,   247,   204,   205,   283,   294,
   284,   249,   250,   295,   307,   308,   309,   285,   170,   171,
    84,    85,    86,   121,    87,   209,    88,    89,    90,   116,
   207,    91,   155,   251,   289,    92,    93,   113,   112,   196,
   197,   198,   172,   215,   231,   104,   105,   173
};

static const short yypact[] = {    22,
   145,-32767,-32767,    13,    21,    22,-32767,-32767,-32767,-32767,
-32767,     8,   -12,-32767,    89,   116,-32767,    23,-32767,    28,
   -16,    16,     7,-32767,-32767,    26,    85,   101,   109,-32767,
-32767,-32767,-32767,-32767,-32767,-32767,   130,-32767,    13,   137,
-32767,   155,-32767,    61,-32767,-32767,-32767,   126,   156,-32767,
-32767,   132,-32767,-32767,-32767,-32767,    40,   155,    23,   155,
     7,     7,   179,   148,   190,   143,   131,-32767,   146,-32767,
   158,-32767,   209,   240,   158,   252,   252,   252,-32767,   222,
-32767,    40,-32767,-32767,-32767,-32767,-32767,-32767,-32767,-32767,
-32767,   176,   193,-32767,   -15,   191,-32767,   211,    23,   141,
   144,-32767,-32767,-32767,-32767,   221,-32767,-32767,-32767,   132,
-32767,   210,   212,-32767,-32767,   158,-32767,-32767,   215,-32767,
   218,   219,-32767,    68,-32767,-32767,   220,-32767,-32767,-32767,
   223,-32767,   155,-32767,-32767,-32767,-32767,-32767,   128,-32767,
   245,-32767,   228,-32767,   225,   226,   254,     6,   283,   227,
-32767,    10,    10,    31,    95,-32767,   290,-32767,-32767,-32767,
-32767,-32767,    68,   120,-32767,-32767,-32767,    14,   207,-32767,
   289,   224,   221,-32767,-32767,-32767,-32767,   195,     5,   199,
    50,-32767,   144,   238,-32767,-32767,-32767,   239,   291,-32767,
   293,   232,-32767,   155,-32767,   230,   235,-32767,   224,   233,
-32767,-32767,-32767,-32767,-32767,-32767,    95,   146,   234,    26,
-32767,   127,   236,   192,   241,-32767,-32767,   142,   142,   142,
   142,   142,   142,   142,   142,-32767,-32767,-32767,-32767,   301,
-32767,-32767,-32767,-32767,-32767,-32767,-32767,     5,     6,-32767,
-32767,    33,-32767,   154,-32767,-32767,    31,   242,   166,   237,
   292,-32767,-32767,   290,   192,-32767,-32767,-32767,   213,   213,
   -22,   -22,   -22,-32767,-32767,-32767,-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,-32767,    17,-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,-32767,-32767,-32767,    95,-32767,-32767,
   -24,   182,-32767,    31,   164,-32767,     5,   304,-32767,-32767,
-32767,-32767,-32767,-32767,-32767,-32767,-32767,   237,-32767,-32767,
-32767,-32767,   312,   313,-32767
};

static const short yypgoto[] = {   314,
-32767,-32767,   167,-32767,-32767,   208,  -219,-32767,-32767,-32767,
    25,-32767,-32767,-32767,   136,-32767,-32767,-32767,    81,-32767,
-32767,-32767,-32767,   147,-32767,-32767,-32767,   -93,-32767,-32767,
   282,-32767,-32767,-32767,   295,-32767,-32767,   -43,    -8,-32767,
-32767,-32767,   189,-32767,-32767,-32767,-32767,-32767,   -26,-32767,
   265,-32767,  -150,-32767,-32767,   229,   165,   163,   159,-32767,
    18,   -42,-32767,-32767,  -216,-32767,-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,-32767,-32767,-32767,    24,  -151,-32767,
-32767,-32767,-32767,   157,-32767,    75,  -153,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,-32767,   248,  -165,-32767,-32767,   180,
-32767,    90,  -146,   -33,  -225,   186,-32767,   273
};


#define	YYLAST		336


static const short yytable[] = {    43,
   195,   195,   217,   210,   206,   199,   199,   148,    59,    36,
   193,   229,   268,    19,   297,    95,    25,   102,   160,    20,
   216,    -1,   275,   124,    59,    70,    99,    23,   115,    32,
   279,    27,   119,   270,    37,   161,   162,   185,   186,   187,
    68,   271,   223,   224,   225,   298,    69,   245,    33,    34,
    36,    70,    38,   130,    71,   134,   252,    72,    73,    74,
   230,    75,    76,    77,    78,    33,    34,    65,   272,   273,
   274,   310,   275,   156,    40,   102,   160,   300,   136,   137,
   138,   -64,   194,  -192,   287,    39,   292,   235,   293,   148,
    36,    44,   195,   161,   162,    68,    42,   199,   286,    99,
   210,    69,   -21,   114,   -21,   201,    70,    45,    79,    71,
    42,  -186,    72,    73,    74,    46,    75,    76,    77,    78,
    33,    34,    53,    54,    55,    56,   163,   102,   160,    53,
    54,    55,    56,    60,   102,   160,    49,   296,    28,    29,
   164,   135,   287,    51,   144,   161,   162,   -58,   -58,   102,
   160,     2,   161,   162,   107,     3,   286,    53,    54,    55,
    56,   102,   160,    79,    63,    42,    65,   161,   162,   136,
   137,   138,   136,   137,   138,   -58,   -58,   -58,   163,   161,
   162,   214,   270,   254,    30,    31,   102,   103,   -78,     4,
   271,   301,   212,     5,    64,   135,   177,   178,   108,   212,
   110,   -58,   -58,   302,   303,   232,   233,   281,   282,   -49,
   -49,   109,   -34,   111,   212,   117,    65,   272,   273,   274,
   304,   305,   306,   136,   137,   138,   194,   100,   101,   255,
   114,   102,   160,   122,   123,   259,   260,   261,   262,   263,
   264,   265,   266,   216,   102,   160,   118,   128,    70,   161,
   162,   218,   219,   220,   221,   222,   223,   224,   225,   120,
   124,   129,   161,   162,   132,   257,   218,   219,   220,   221,
   222,   223,   224,   225,   220,   221,   222,   223,   224,   225,
   133,   149,   152,   179,   153,   157,   158,   159,   174,   181,
   102,   175,   184,   182,   191,   183,   208,   226,   240,   237,
   239,   241,   242,   243,   244,   253,   246,   267,   288,   256,
   311,   314,   315,   313,   258,   280,   299,   151,   236,   269,
    50,   176,    94,   131,   228,    47,   213,   211,   290,   126,
   227,   312,   200,   278,   190,   106
};

static const short yycheck[] = {    26,
   152,   153,   168,   157,   155,   152,   153,   101,    42,    18,
     1,     7,   238,     1,    39,    59,     9,     8,     9,     7,
     7,     0,   242,    39,    58,    12,    60,     7,    71,     7,
   247,    44,    75,     1,     7,    26,    27,    32,    33,    34,
     1,     9,    65,    66,    67,    70,     7,   199,    26,    27,
    59,    12,    69,    69,    15,    99,   207,    18,    19,    20,
    56,    22,    23,    24,    25,    26,    27,    35,    36,    37,
    38,   297,   292,   116,    68,     8,     9,   294,    29,    30,
    31,    69,    73,    74,   250,    70,    70,   181,    72,   183,
    99,     7,   244,    26,    27,     1,    71,   244,   250,   133,
   254,     7,    70,    73,    72,    75,    12,     7,    69,    15,
    71,    72,    18,    19,    20,     7,    22,    23,    24,    25,
    26,    27,     3,     4,     5,     6,    59,     8,     9,     3,
     4,     5,     6,    73,     8,     9,     7,   288,    50,    51,
    73,     1,   308,     7,     1,    26,    27,     7,     8,     8,
     9,     7,    26,    27,     7,    11,   308,     3,     4,     5,
     6,     8,     9,    69,    39,    71,    35,    26,    27,    29,
    30,    31,    29,    30,    31,    32,    33,    34,    59,    26,
    27,   164,     1,   210,    69,    70,     8,     9,    44,    45,
     9,    28,    73,    49,    39,     1,    69,    70,     9,    73,
    70,     7,     8,    40,    41,     7,     8,    42,    43,    69,
    70,    69,    69,    68,    73,     7,    35,    36,    37,    38,
    57,    58,    59,    29,    30,    31,    73,    61,    62,   212,
    73,     8,     9,    77,    78,   218,   219,   220,   221,   222,
   223,   224,   225,     7,     8,     9,     7,    72,    12,    26,
    27,    60,    61,    62,    63,    64,    65,    66,    67,     8,
    39,    69,    26,    27,    74,    74,    60,    61,    62,    63,
    64,    65,    66,    67,    62,    63,    64,    65,    66,    67,
    70,    61,    73,    39,    73,    71,    69,    69,    69,    62,
     8,    69,    39,    69,    68,    70,     7,     9,     8,    62,
    62,     9,    71,    74,    70,    72,    74,     7,    17,    74,
     7,     0,     0,     0,    74,    74,   292,   110,   183,   239,
    39,   133,    58,    95,   178,    31,   164,   163,   254,    82,
   172,   308,   153,   244,   149,    63
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
	 yyerror("parser stack overflow");
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
	   YYtrace("Now at end of inputn");
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

case 2:
#line 142 "parser.y"
{ 
#ifdef YYDEBUG
		yydebug = 1; 
#endif
			;
    break;}
case 12:
#line 255 "parser.y"
{
				menu_startdefine(yyvsp[-1].s);
				freeitem(&yyvsp[-1].s);
			;
    break;}
case 17:
#line 266 "parser.y"
{
				protokoll("Defining sub-menu %s",yyvsp[0].s);
				menu_addentry(yyvsp[0].s, UM_POPUP, 0, 0);
				freeitem(&yyvsp[0].s);
			;
    break;}
case 18:
#line 272 "parser.y"
{
				menu_addentry((char*)0, UM_ENDPOPUP, 0, 0);
			;
    break;}
case 24:
#line 284 "parser.y"
{
				menu_addentry((char *)0, UM_SEPARATOR, 0, 0);
			;
    break;}
case 27:
#line 290 "parser.y"
{
				menu_addentry(yyvsp[-3].s, UM_ITEM, yyvsp[0].macref.typ, yyvsp[0].macref.index);
				freeitem(&yyvsp[-3].s);
			;
    break;}
case 29:
#line 295 "parser.y"
{ yyerror("invalid menu item"); ;
    break;}
case 32:
#line 303 "parser.y"
{
				protokoll("Compile mouse table %s",yyvsp[0].s);
				mouse_switchtotable(yyvsp[0].s);
				if (SetOverride(yyvsp[-2].s) == MODE_OVERRIDE) {
					mouse_overridetable();
				}
				freeitem(&yyvsp[0].s);
			;
    break;}
case 37:
#line 318 "parser.y"
{
				if (yyvsp[-3].mousecode.button && 
				    yyvsp[-1].macref.index != (MACROREFIDX)-1) {
					bind_mouse(yyvsp[-3].mousecode, yyvsp[0].macref.typ, yyvsp[0].macref.index,
						yyvsp[-1].num, _override == MODE_AUGMENT);
				}
			;
    break;}
case 38:
#line 325 "parser.y"
{ yyerror("mouse binding"); ;
    break;}
case 39:
#line 327 "parser.y"
{ yyval.num = 0; ;
    break;}
case 40:
#line 328 "parser.y"
{ yyval.num = MO_FINDCURS; ;
    break;}
case 41:
#line 330 "parser.y"
{ yyval.num = MBUT_L; ;
    break;}
case 42:
#line 331 "parser.y"
{ yyval.num = MBUT_R; ;
    break;}
case 43:
#line 332 "parser.y"
{ yyval.num = MBUT_M; ;
    break;}
case 44:
#line 334 "parser.y"
{ yyval.num = yyvsp[0].num; 		  ;
    break;}
case 45:
#line 335 "parser.y"
{ yyval.num = yyvsp[-2].num | yyvsp[0].num; ;
    break;}
case 46:
#line 338 "parser.y"
{
				yyval.mousecode.shift = yyvsp[-2].num >> 8;
				yyval.mousecode.button = yyvsp[-1].num;
				yyval.mousecode.nclicks = yyvsp[0].num;
			;
    break;}
case 47:
#line 346 "parser.y"
{
				protokoll("Compile key table %s",yyvsp[0].s);
				key_switchtotable(yyvsp[0].s);
				if (SetOverride(yyvsp[-2].s) == MODE_OVERRIDE) {
					key_destroytable(yyvsp[0].s);
				}
				freeitem(&yyvsp[0].s);
			;
    break;}
case 52:
#line 361 "parser.y"
{
				if (yyvsp[-2].num != K_INVALID && 
				    yyvsp[0].macref.index != (MACROREFIDX) -1) {
					if (!bind_key(yyvsp[-2].num, yyvsp[0].macref.typ, 
							   yyvsp[0].macref.index,
							   _override == MODE_AUGMENT)) {
						yyerror("cannot bind key 0x%lx",yyvsp[-2].num);
					}
				}
			;
    break;}
case 53:
#line 371 "parser.y"
{ yyerror("key binding"); ;
    break;}
case 54:
#line 374 "parser.y"
{
				if ((yyval.num = key2code(yyvsp[0].s,_kmod_control)) == K_INVALID)
					yyerror("bad key %s",yyvsp[0].s);
				freeitem(&yyvsp[0].s);
			;
    break;}
case 55:
#line 379 "parser.y"
{	yyval.num = yyvsp[0].num|_kmod_control; 	;
    break;}
case 56:
#line 381 "parser.y"
{	_kmod_control = yyvsp[0].num; ;
    break;}
case 57:
#line 382 "parser.y"
{ 
				yyval.num = yyvsp[0].num;
			;
    break;}
case 58:
#line 386 "parser.y"
{	yyval.num = 0; ;
    break;}
case 59:
#line 387 "parser.y"
{ yyval.num = yyvsp[-2].num | yyvsp[0].num; ;
    break;}
case 60:
#line 389 "parser.y"
{ yyval.num = /* K_SHIFT */ 0x300; ;
    break;}
case 61:
#line 390 "parser.y"
{ yyval.num = K_CONTROL; ;
    break;}
case 62:
#line 391 "parser.y"
{ yyval.num = K_ALTERNATE; ;
    break;}
case 66:
#line 401 "parser.y"
{	
				protokoll("Defining icon for macro %s",yyvsp[0].s);
				freeitem(&yyvsp[-4].s);
				freeitem(&yyvsp[-3].s);
				freeitem(&yyvsp[-2].s);
				freeitem(&yyvsp[0].s);
			;
    break;}
case 67:
#line 408 "parser.y"
{ yyerror("icon definition"); ;
    break;}
case 71:
#line 417 "parser.y"
{ _bDefiningConst = 1; ;
    break;}
case 72:
#line 417 "parser.y"
{ _bDefiningConst = 0; ;
    break;}
case 73:
#line 419 "parser.y"
{
				MakeInternalSym(yyvsp[-3].s,
					(yyvsp[0].v.type == C_STRING1PAR) ? 
					S_CONSTSTRING : S_CONSTNUM, yyvsp[0].v.val);
				freeval(&yyvsp[0].v);
			;
    break;}
case 74:
#line 426 "parser.y"
{	yyval.s = yyvsp[0].s; ;
    break;}
case 75:
#line 427 "parser.y"
{	yyval.s	= yyvsp[0].s; ;
    break;}
case 76:
#line 429 "parser.y"
{	yyval.s = yyvsp[0].s; ;
    break;}
case 77:
#line 430 "parser.y"
{	yyval.s = yyvsp[0].s; ;
    break;}
case 78:
#line 434 "parser.y"
{ startmacro(); ;
    break;}
case 79:
#line 435 "parser.y"
{ yyval.s = yyvsp[-3].s; ;
    break;}
case 84:
#line 443 "parser.y"
{
				int typ;
				
				typ = (yyvsp[-1].type == C_STRING1PAR) ? S_DOLSTRING : S_DOLNUMBER;
				MakeInternalSym(yyvsp[0].s,typ,_nparam);
				PushCreateVariable(yyvsp[0].s,typ,_nparam);
				_nparam++;
			;
    break;}
case 85:
#line 453 "parser.y"
{
				if (!yyerrflg) {
					makelabel(lreturnid,0);
					killabel(lreturnid,0);
					mac_validatename(yyvsp[-2].s);
					_recp = AddComSeq(_recp,_recpend,C_STOP,1);
					protokoll("Defining macro %s",yyvsp[-2].s);
					mac_insert(yyvsp[-2].s,yyvsp[-1].s,_recspace,(int)(_recp-_recspace));
				}
				freeitem(&yyvsp[-1].s);
				freeitem(&yyvsp[-2].s);
				CloseLabelList();
				endmacro();
			;
    break;}
case 86:
#line 468 "parser.y"
{	yyval.s = 0; ;
    break;}
case 87:
#line 469 "parser.y"
{	yyval.s = yyvsp[0].s; ;
    break;}
case 88:
#line 471 "parser.y"
{	yyval.s = ""; ;
    break;}
case 89:
#line 472 "parser.y"
{	yyval.s = yyvsp[0].s; ;
    break;}
case 90:
#line 474 "parser.y"
{	yyval.islocal = 0;	;
    break;}
case 91:
#line 475 "parser.y"
{	yyval.islocal = 1;	;
    break;}
case 97:
#line 485 "parser.y"
{
				MakeInternalSym(yyvsp[-2].s,
					(yyvsp[-3].type == C_STRING1PAR) ? S_STRING : S_NUMBER,
					(yyvsp[-3].type == C_STRING1PAR) ? (long)"" : 0);
				PushAssign(yyvsp[-2].s,yyvsp[-1].v.type,(long)yyvsp[-1].v.val);
				freeval(&yyvsp[-1].v);
				vname_count = 0;
			;
    break;}
case 98:
#line 492 "parser.y"
{
				if (yyvsp[-2].type == C_STRING1PAR) {
					MakeInternalSym(yyvsp[-1].s, S_STRING, (long)"");
					PushAssign(yyvsp[-1].s, C_STRING1PAR, (long)"");
				} else {
					MakeInternalSym(yyvsp[-1].s, S_NUMBER, (long)0);
					PushAssign(yyvsp[-1].s,C_LONG1PAR,(long)0);
				}
				vname_count = 0;
			;
    break;}
case 101:
#line 505 "parser.y"
{ vname_count = 0; ;
    break;}
case 112:
#line 515 "parser.y"
{ yyerror("invalid statement"); ;
    break;}
case 113:
#line 518 "parser.y"
{
				PushAssign(yyvsp[-1].s,yyvsp[0].v.type,(long)yyvsp[0].v.val);
			;
    break;}
case 114:
#line 522 "parser.y"
{ 	yyval.v = yyvsp[0].v;	;
    break;}
case 115:
#line 524 "parser.y"
{ yyval.v = yyvsp[0].v; ;
    break;}
case 116:
#line 525 "parser.y"
{ yyval.v = yyvsp[-1].v; ;
    break;}
case 117:
#line 527 "parser.y"
{ yyval.v = yyvsp[0].v; ;
    break;}
case 118:
#line 528 "parser.y"
{ yyval.v = PushBinop(BIN_NOT,&yyvsp[0].v,0); 	;
    break;}
case 119:
#line 530 "parser.y"
{ yyval.v = yyvsp[0].v; ;
    break;}
case 120:
#line 531 "parser.y"
{
				_recp = AddComSeq(_recp,_recpend,C_FURET,vname_count);
			;
    break;}
case 121:
#line 533 "parser.y"
{
				if (yyvsp[0].funcp && IsStringFunc(yyvsp[0].funcp)) {
					yyval.v.type = C_STRINGVAR;
				} else {
					yyval.v.type = C_LONGVAR;
				}
				yyval.v.val = (long)"__ret__";
				((char*)yyval.v.val)[6] = '0' + vname_count++;
			;
    break;}
case 122:
#line 543 "parser.y"
{ yyval.v = yyvsp[0].v;	;
    break;}
case 123:
#line 544 "parser.y"
{
				if (IsStringType(yyvsp[-1].type) ==
				    IsStringType(yyvsp[0].v.type)) {
					yyval.v = yyvsp[0].v;
				} else {
					yyval.v = PushBinop(BIN_CONVERT,&yyvsp[0].v,0);
					yyval.v.type = (yyvsp[-1].type == C_STRING1PAR) ?
						C_STRINGVAR : C_LONGVAR;
				}
			;
    break;}
case 124:
#line 554 "parser.y"
{ yyval.v = PushBinop(BIN_AND,&yyvsp[-2].v,&yyvsp[0].v); ;
    break;}
case 125:
#line 555 "parser.y"
{ yyval.v = PushBinop(BIN_OR,&yyvsp[-2].v,&yyvsp[0].v); ;
    break;}
case 126:
#line 556 "parser.y"
{ yyval.v = PushBinop(BIN_ADD,&yyvsp[-2].v,&yyvsp[0].v); ;
    break;}
case 127:
#line 557 "parser.y"
{ yyval.v = PushBinop(BIN_XOR,&yyvsp[-2].v,&yyvsp[0].v); ;
    break;}
case 128:
#line 558 "parser.y"
{ yyval.v = PushBinop(BIN_SUB,&yyvsp[-2].v,&yyvsp[0].v); ;
    break;}
case 129:
#line 559 "parser.y"
{ yyval.v = PushBinop(BIN_MUL,&yyvsp[-2].v,&yyvsp[0].v); ;
    break;}
case 130:
#line 560 "parser.y"
{ yyval.v = PushBinop(BIN_DIV,&yyvsp[-2].v,&yyvsp[0].v); ;
    break;}
case 131:
#line 561 "parser.y"
{ yyval.v = PushBinop(BIN_MOD,&yyvsp[-2].v,&yyvsp[0].v); ;
    break;}
case 132:
#line 562 "parser.y"
{ yyval.v = yyvsp[-1].v; ;
    break;}
case 133:
#line 566 "parser.y"
{ TestOp(); ;
    break;}
case 134:
#line 566 "parser.y"
{ CloseTestOp(CT_BRACKETS); ;
    break;}
case 138:
#line 570 "parser.y"
{ TestOp(); ;
    break;}
case 139:
#line 570 "parser.y"
{ CloseTestOp(CT_NOT); ;
    break;}
case 140:
#line 572 "parser.y"
{ TestOp(); ;
    break;}
case 141:
#line 572 "parser.y"
{ CloseTestOp(yyvsp[0].type); ;
    break;}
case 142:
#line 574 "parser.y"
{ yyval.type = CT_BRACKETS; ;
    break;}
case 143:
#line 575 "parser.y"
{ yyval.type = yyvsp[0].type; BinOp1(yyvsp[0].type); ;
    break;}
case 145:
#line 577 "parser.y"
{	yyval.type = CT_AND;	;
    break;}
case 146:
#line 578 "parser.y"
{	yyval.type = CT_OR;	;
    break;}
case 147:
#line 580 "parser.y"
{ TestOp(); ;
    break;}
case 148:
#line 581 "parser.y"
{ BinOp1(yyvsp[0].type);  ;
    break;}
case 149:
#line 582 "parser.y"
{ CloseTestOp(yyvsp[0].type); ;
    break;}
case 150:
#line 584 "parser.y"
{ 	
				if (_stringflg)
					_recp = AddComSeq(_recp,_recpend,C_STRING1PAR,(long)"");
				else
					_recp = AddComSeq(_recp,_recpend,C_LONG1PAR,0L);
				yyval.type = CT_NE;
			;
    break;}
case 151:
#line 591 "parser.y"
{
				int flg = STRINGTYPE(yyvsp[0].type);
				if (_stringflg != flg)
					yyerror("invalid combination in expr of string and integer");
				yyval.type = yyvsp[-1].type | flg;
			;
    break;}
case 152:
#line 598 "parser.y"
{	yyval.type = CT_LT;	;
    break;}
case 153:
#line 599 "parser.y"
{	yyval.type = CT_GT;	;
    break;}
case 154:
#line 600 "parser.y"
{	yyval.type = CT_EQ;	;
    break;}
case 155:
#line 601 "parser.y"
{	yyval.type = CT_NE;	;
    break;}
case 156:
#line 602 "parser.y"
{
				if (_stringflg == CT_NUM)
					yyerror("match operator not allowed on numbers");
				yyval.type = yyvsp[0].type;
			;
    break;}
case 157:
#line 608 "parser.y"
{	yyval.type = CT_MATCH;			;
    break;}
case 158:
#line 609 "parser.y"
{	yyval.type = CT_NMATCH;		;
    break;}
case 159:
#line 611 "parser.y"
{	yyval.type = C_LONG1PAR; 	 	;
    break;}
case 160:
#line 612 "parser.y"
{	yyval.type = PushVal(&yyvsp[0].v);	;
    break;}
case 161:
#line 614 "parser.y"
{
				yyval.v.type = C_LONGVAR;  
				yyval.v.val  = (long) yyvsp[0].s;
			;
    break;}
case 162:
#line 618 "parser.y"
{
				yyval.v.type = C_STRINGVAR;  
				yyval.v.val  = (long) yyvsp[0].s;
			;
    break;}
case 163:
#line 622 "parser.y"
{
				yyval.v.type = C_STRING1PAR;
				yyval.v.val  = (long) yyvsp[0].s;
			;
    break;}
case 164:
#line 626 "parser.y"
{	yyval.v = yyvsp[0].v;	;
    break;}
case 165:
#line 628 "parser.y"
{	yyval.s = yyvsp[0].s;	;
    break;}
case 166:
#line 629 "parser.y"
{ 
				if ((yyval.s = malloc(strlen(yyvsp[-1].s)+strlen(yyvsp[0].s)+1)) != 0) {
					strcat(strcpy(yyval.s,yyvsp[-1].s),yyvsp[0].s);
					free(yyvsp[-1].s);
					free(yyvsp[0].s);
				} else {
					yyerror("buffer overlow");
					yyval.s = yyvsp[0].s;
				}
			;
    break;}
case 167:
#line 641 "parser.y"
{	gotolabel(lendid,_breaklevel-yyvsp[-1].num,BRA_ALWAYS); ;
    break;}
case 168:
#line 644 "parser.y"
{	gotolabel(lstartid,_breaklevel-yyvsp[-1].num,BRA_ALWAYS); ;
    break;}
case 169:
#line 646 "parser.y"
{
				_recp = AddComSeq(_recp,_recpend,C_STOP,yyvsp[0].num);
			;
    break;}
case 170:
#line 650 "parser.y"
{	yyval.num = 1;	;
    break;}
case 171:
#line 651 "parser.y"
{	yyval.num = yyvsp[0].num; ;
    break;}
case 175:
#line 658 "parser.y"
{
				MakeLabel(yyvsp[-1].s,(COM_GOTO *)_recp);
				freeitem(&yyvsp[-1].s);
			;
    break;}
case 176:
#line 663 "parser.y"
{
				_recp = GotoLabel(yyvsp[0].s,_recp,_recpend,BRA_ALWAYS);
				freeitem(&yyvsp[0].s);
			;
    break;}
case 177:
#line 667 "parser.y"
{
				_recp = GotoLabel(yyvsp[0].s,_recp,_recpend,BRA_EQ);
				freeitem(&yyvsp[0].s);
			;
    break;}
case 178:
#line 672 "parser.y"
{
				makelabel(lstartid,_breaklevel);
			;
    break;}
case 179:
#line 674 "parser.y"
{
				gotolabel(lendid,_breaklevel,BRA_EQ);
				_breaklevel++;
			;
    break;}
case 180:
#line 677 "parser.y"
{
				_breaklevel--;
				gotolabel(lstartid,_breaklevel,BRA_ALWAYS);
				makelabel(lendid,_breaklevel);
				killabel(lstartid,_breaklevel);
				killabel(lendid,_breaklevel);
			;
    break;}
case 181:
#line 685 "parser.y"
{
				gotolabel(iffailid,_iflevel,BRA_EQ);
				_iflevel++;
			;
    break;}
case 182:
#line 688 "parser.y"
{
				_iflevel--;
				gotolabel(ifdoneid,_iflevel,BRA_ALWAYS);
				makelabel(iffailid,_iflevel);
				killabel(iffailid,_iflevel);
			;
    break;}
case 183:
#line 693 "parser.y"
{
				makelabel(ifdoneid,_iflevel);
				killabel(ifdoneid,_iflevel);
			;
    break;}
case 188:
#line 704 "parser.y"
{
				_lastfuncp = yyvsp[0].funcp;
				_parno = 0;
				_recp = AddComSeq(_recp,_recpend,C_0FUNC,
							   FuncIdx(yyvsp[0].funcp));
				yyval.funcp = yyvsp[0].funcp;
			;
    break;}
case 190:
#line 711 "parser.y"
{
				_lastfuncp = 0;
				_recp = AddComSeq(_recp,_recpend,C_MACRO,(long)yyvsp[0].s);
				freeitem(&yyvsp[0].s);
				yyval.funcp = 0;
			;
    break;}
case 194:
#line 721 "parser.y"
{
				PushParam(&yyvsp[0].v);
			;
    break;}
case 195:
#line 724 "parser.y"
{
				PushParam(&yyvsp[0].v);
			;
    break;}
case 196:
#line 727 "parser.y"
{
				yyerror("illegal parameters");
			;
    break;}
case 197:
#line 731 "parser.y"
{
				if (ISCONSTINT(yyvsp[0].v.type)) {
					if (yyvsp[-1].type == C_CHAR1PAR &&
					    (yyvsp[0].v.val < 0 || yyvsp[0].v.val > 255))
					    	yyerror("constant out of range");
					if (yyvsp[-1].type == C_STRING1PAR && yyvsp[0].num) {
						yyerror("invalid string cast");
						yyvsp[0].v.val = 0;
					}
				} else if (ISVARINT(yyvsp[0].v.type)) {
					if (yyvsp[-1].type == C_STRING1PAR) {
						yyerror("unsupported cast");
					}
				}
				yyval.v.type = yyvsp[-1].type;
				yyval.v.val = yyvsp[0].num;
			;
    break;}
case 198:
#line 748 "parser.y"
{ yyval.v = yyvsp[0].v; ;
    break;}
case 199:
#line 750 "parser.y"
{	yyval.type = yyvsp[-1].type;		;
    break;}
case 200:
#line 752 "parser.y"
{	yyval.type = C_CHAR1PAR;	;
    break;}
case 201:
#line 753 "parser.y"
{	yyval.type = C_INT1PAR;	;
    break;}
case 202:
#line 754 "parser.y"
{	yyval.type = C_LONG1PAR;	;
    break;}
case 203:
#line 755 "parser.y"
{	yyval.type = C_STRING1PAR;	;
    break;}
case 204:
#line 757 "parser.y"
{
				yyval.macref.typ = CMD_MACRO; 
				if ((yyval.macref.index = mac_getbyname(yyvsp[0].s)) == 
					(MACROREFIDX)-1) {
					yyerror("undefined macro %s",yyvsp[0].s);
				}
				freeitem(&yyvsp[0].s);
			;
    break;}
case 205:
#line 765 "parser.y"
{
				yyval.macref.typ = CMD_CMDSEQ; 
				yyval.macref.index = mac_cmdseqbyname(yyvsp[0].s); 
				freeitem(&yyvsp[0].s);
			;
    break;}
case 206:
#line 771 "parser.y"
{
				yyval.v.type = C_LONG1PAR; 
				yyval.v.val  = yyvsp[0].num;
			;
    break;}
case 207:
#line 777 "parser.y"
{ yyval.v = yyvsp[0].v; ;
    break;}
case 208:
#line 778 "parser.y"
{
				yyval.v.type = C_STRING1PAR;
				yyval.v.val  = (long) yyvsp[0].s;
			;
    break;}
case 209:
#line 783 "parser.y"
{ yyval.v = yyvsp[0].v;	;
    break;}
case 210:
#line 784 "parser.y"
{
				yyval.v.type = yyvsp[-2].v.type;
				yyval.v.val = yyvsp[-2].v.val | yyvsp[0].v.val;
			;
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
	   YYtrace("Discarding token %d (%s)n", yychar, yytname[yychar1]);
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
#line 788 "parser.y"


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
static unsigned char PushVal(struct typedval *vp)
{
	_recp = AddComSeq(_recp,_recpend,vp->type,vp->val);
	freeval(vp);
	return vp->type;
}

/*---------------------------------*/
/* PushParam()					*/
/*---------------------------------*/
static void PushParam(struct typedval *vp)
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
{	char *s,buf[128];
	static FILE *fp;

	if (fp == 0 && (fp = fopen("O:\\PROT","w")) == 0)
		return;
	vfprintf(fp,fmt,&...);
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
void freeval(struct typedval *v)
{
	if ((v->type == C_STRING1PAR /*||
	    v->type == C_STRINGVAR  ||
	    v->type == C_LONGVAR*/) &&
	    *(char*)v->val) {
# if 0
	    	char buf[128];
	    	sprintf(buf,"freeing %d|%s",v->type,
			v->val ? (char*)v->val : "(null)");
		Alert(buf);
# endif
		freeitem((char **)(&v->val));
	}
}

/*---------------------------------*/
/* startmacro()				*/
/*---------------------------------*/
static void startmacro(void)
{
#ifdef YYDEBUG
	yydebug = 1;
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

