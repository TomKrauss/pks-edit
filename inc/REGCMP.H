/*
 * REGCMP.H
 *
 * PROJECT: PKSEDIT for ATARI TOS
 *
 * purpose: regular expression defines
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 04.02.1991
 *									
 */

# ifndef 	REGCMP_H
# define	REGCMP_H

# define	RE_DOREX		0x1		/* use .{}*?+ and all the stuff */
# define	RE_IGNCASE	0x2		/* ignore case */
# define	RE_SHELLWILD	0x4		/* wildcards are: *?[] */

extern unsigned char *compile(unsigned char *instring, 
			 		     unsigned char *ep, 
				   		unsigned char *endbuf,
						unsigned char eof,
				   		int flags);

extern int step(unsigned char *linebuf,unsigned char *expbuf,
			 unsigned char *endbuf);

extern	char   *__loc1,*__loc2,*__locs;    /* where the expression is found */
extern	int    _nbrackets,_staronly;		/* # of \d */
extern	char   *_braslist[],*_braelist[];	/* start and end of \.. */

extern	int	  _circf;
extern	int	  _regerror,_regepos;

typedef struct tagREGCMP {
	unsigned char *(*compile)(unsigned char *instring, unsigned char *ep, 
		   		unsigned char *endbuf, unsigned char eof, int flags);
	int    (*step)(unsigned char *linebuf,unsigned char *expbuf,unsigned char *endbuf);
	char   **loc1,**loc2,**locs;
	int    *nbrackets,*staronly;
	char   **braslist[9],**braelist[9];
	int	  *circf;
	int	  *regerror,*regepos;
	int	  *errmsg;
} REGCMP;

extern REGCMP	_regcmp;

#endif

