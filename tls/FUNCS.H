
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

