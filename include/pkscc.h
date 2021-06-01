/*
 * PKSCC.H
 *
 * macro compiler, stddefs.
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krau�					Author : TOM
 * All Rights Reserved.					created: 13.03.1991
 *									
 */

#ifndef PKSCC_H

#ifndef DIM
#define DIM(x)		sizeof(x)/sizeof(x[0])
#endif

#define UNUSED(x)	(x) = (x)

extern char *		StaticLoadString(int nId);
extern void 		protokoll(char *s, ...);
extern int 		ActiveRulerContext(int ctx);

# ifdef __STDIO
extern int 		CreateFileAndDisplay(char *fn, long (*callback)(FILE *fp));
extern int 		printpaste(FILE *fp, void *p);
extern void 		printesclist(FILE *fp, void *p);
# endif

# ifdef _PKSMOD_H
extern EDXPORTS 	*edx;
# endif

#define PKSCC_H
#endif

