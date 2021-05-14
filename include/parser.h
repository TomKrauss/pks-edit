
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

