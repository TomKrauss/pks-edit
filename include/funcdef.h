/*
 * funcdef.h
 *
 * calling a pksedit function by name
 *
 * PROJECT: PKSEDIT
 *
 *  * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 07.02.1991
 */

# ifndef FUNCDEF_H
#define	FUNCDEF_H

typedef struct tagTYPEELEM {
	const char* te_name;
	long		te_val;
} TYPEELEM;

/*
 * type flags
 */
#define	OF_ELIPSIS	0x1			/* ... parameters may follow */

/*
 * kind of types
 */
#define	OT_ENUM		1			/* enumeration type */
#define	OT_OPTION		2			/* bitfield type */

typedef struct tagOWNTYPE {
	signed char	ot_typ;
	char			ot_flags;
	unsigned char	ot_idx;
	unsigned char	ot_nelem;
	char *		ot_id;
} OWNTYPE;

#define	MAXPARS		10

#define	PAR_USER		 1
#define	PAR_VOID		 0
#define	PAR_INT		-1			/* predefined types */
#define	PAR_STRING	-2
/* #define	PAR_DIALOGOPT	-4 		*/

typedef struct edfuncdef {
	unsigned char	idx;				/* function index edit */
#if defined (_Windows)
	signed char near*	ftyps;
#elif defined(WIN32)
	signed char *		ftyps;
#else
	signed char	ftyps[MAXPARS];	/* parameter type array */
#endif
} EDFUNCDEF;

extern int 		_ntypes,_nfunctions,_nenelems;
extern EDFUNCDEF 	_functab[];
extern OWNTYPE 	_typetab[];
extern TYPEELEM	_enelemtab[];

#define 	FUNC_EdBlockCopy			2
#define 	FUNC_EdBlockMove			6
#define 	FUNC_EdKeycodeInsert			16
#define 	FUNC_EdCharInsert			17
#define 	FUNC_EdOptionToggle			78
#define		CMD_EdOptionToggleBLK_COLUMN_SELECTION 51
#define		CMD_INITIATE_INCREMENTAL_SEARCH 235

typedef enum { OP_EDIT_MODE, OP_DISPLAY_MODE, OP_MACRO, OP_OPTIONS, OP_FILEFLAG } OP_FLAGTYPE;

#define	OPT_WMODE			(((long)OP_EDIT_MODE)<<16)
#define	OPT_DMODE			(((long)OP_DISPLAY_MODE)<<16)
#define	OPT_GLOBSET			(((long)OP_OPTIONS)<<16L)
#define  OPT_RECORD			(((long)OP_MACRO)<<16L)

# endif	/* FUNCDEF_H */


