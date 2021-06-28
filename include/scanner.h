/*
 * scanner.h
 *
 * scanner definitions
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 *This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 03.02.1991
 */

# ifndef SETLEX_H
#define SETLEX_H

#define LEXEOF			0

#define ERR_TOOMANYERR	1
#define ERR_SPACE		2

#ifndef __SETJMP
#include <setjmp.h>
#endif

typedef struct tagTYPEDVAL {
	unsigned char type;
	intptr_t val;
} TYPEDVAL;

typedef union yytype {
	long	num;
	int		islocal;
	char	*s;
	void	*funcp;
	char	c;
	unsigned char type;
	MACROREF	macref;
	MOUSECODE mousecode;
	TYPEDVAL v;
} _YYSTYPE;

extern _YYSTYPE	yylval;

#ifndef YYEOF
#define YYEOF		0
#endif

# endif	/* SETLEX_H */

