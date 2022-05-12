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
	struct tagPARSE_IDENTIFIER {
		int type;					// PKS Value type. Hack: that this must be the first struct element, as it is somewhat confused with v.type (see below)
		MACRO_SCOPE scope;
		char  operation;			// the current operation constructed (if applicable)
		char  stringIsAlloced : 1;	// whether the string 's' was alloced
		char  isLocalVar : 1;		// used to differentiate between local and global vars
		unsigned short heapIndex;	// for local variables - the index into the local var heap.
		int  arraySize;				// for array type variables the size of the array
		char* s;					// the actual string of the identifier
	} ident;
	TYPEDVAL		v;
	void			*funcp;
	unsigned char	binop;			// Used to remember a concrete binary operation
	unsigned char	needsPop;
} _YYSTYPE;

#define MAX_TYPE_PROPERTIES			32	// cannot declare enums with more enum values and structs with more properties for now.
extern PKS_TYPE_DESCRIPTOR* _currentTypeDescriptor;

#define IS_IN_ENUM_DEFINITION		(_currentTypeDescriptor && _currentTypeDescriptor->ptd_isEnumType)

extern _YYSTYPE	yylval;

extern unsigned char* bytecode_emitInstruction(BYTECODE_BUFFER* pBuffer, unsigned char typ, GENERIC_DATA data);

# endif	/* SCANNER_H */

