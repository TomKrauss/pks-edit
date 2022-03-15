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

# ifndef SCANNER_H
#define SCANNER_H

#define LEXEOF			0

#define ERR_TOOMANYERR	1
#define ERR_SPACE		2

#ifndef __SETJMP
#include <setjmp.h>
#endif

typedef struct tagTYPEDVAL {
	unsigned char type;
	GENERIC_DATA data;
} TYPEDVAL;

typedef union yytype {
	long long num;
	struct {
		char* s;
		int	stringIsAlloced;
		int type;
	} ident;
	void	*funcp;
	unsigned char type;
	unsigned char needsPop;
	TYPEDVAL v;
} _YYSTYPE;

extern _YYSTYPE	yylval;

extern unsigned char* bytecode_emitInstruction(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data);

# endif	/* SCANNER_H */

