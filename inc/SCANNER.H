/*
 * SETLEX.H
 *
 * scanner definitions
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 03.02.1991
 *									
 */

# ifndef SETLEX_H
# define SETLEX_H

#define LEXEOF			0

#define ERR_TOOMANYERR	1
#define ERR_SPACE		2

#ifndef __SETJMP
# include	<setjmp.h>
#endif

typedef union yytype {
	long		num;
	int		islocal;
	char		*s;
	void		*funcp;
	char		c;
	unsigned char type;
	MACROREF	macref;
	MOUSECODE mousecode;
	struct typedval {
		unsigned char type;
		long 		val;
	} v;
} _YYSTYPE;

extern _YYSTYPE	yylval;

#ifndef YYEOF
#define YYEOF		0
#endif

# endif	/* SETLEX_H */

