/*
 * FUNCDEF.H
 *
 * calling a pksedit function by name
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 07.02.1991
 *									
 */

# ifndef FUNCDEF_H
# define	FUNCDEF_H

typedef struct tagTYPEELEM {
	int			te_name;
	long			te_val;
} TYPEELEM;

/*
 * type flags
 */
# define	OF_ELIPSIS	0x1			/* ... parameters may follow */

/*
 * kind of types
 */
# define	OT_ENUM		1			/* enumeration type */
# define	OT_OPTION		2			/* bitfield type */

typedef struct tagOWNTYPE {
	signed char	ot_typ;
	char			ot_flags;
	unsigned char	ot_idx;
	unsigned char	ot_nelem;
	char *		ot_id;
} OWNTYPE;

# define	MAXPARS		10

# define	PAR_USER		 1
# define	PAR_VOID		 0
# define	PAR_INT		-1			/* predefined types */
# define	PAR_STRING	-2
/* # define	PAR_DIALOGOPT	-4 		*/

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

# endif	/* FUNCDEF_H */


