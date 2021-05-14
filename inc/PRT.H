/*
 * prt.h: Print Interface
 */
 
#ifndef	PRT_H

typedef struct tagFONTSPEC {
	int		fs_oemmode;
	int		fs_cheight;
	int		fs_cwidth;
	char	fs_name[128];
} FONTSPEC;

typedef struct tagLNSPACE {
	int 		z;
	int 		n;
} LNSPACE;

typedef struct prtparams {			/* print params */
	/* params */
	int		startpage,
			endpage,
			pageoffs;
	int		mode;
	int		options;
	int		pagelen;

	FONTSPEC	font;
	int		lmargin,rmargin,nchars;
	int		tabsize;
	LNSPACE	lnspace;				/* 1, 1«, 2 Lines */

	int		d1,d2,d3,d4;			/* distances header, footer */
	FONTSPEC htfont;				/* Font header - footer */
	char	header[512],
			footer[512];			/* text of header and footer */
	int		align;				/* Header / Footer Alignment */
								/* left, center, right */
	
	FONTSPEC	fnfont;
	LNSPACE	fnlnspace;			/* Line spacing footnotes */
	int		fnlinelen;			/* length of fn delimter line */
	int		fnd1,fnd2;			/* distances to body and footer */
	int		fnoffset;				/* offset of Fnote */
} PRTPARAM;

/* mode */
#define	PRTM_RAW			0		/* no conversions */
#define	PRTM_WPLUS		0x1		/* Attributes, Hyphens, ...*/

/* linespacing */
#define	PRS_1			0		/* 1  line */
#define	PRS_1AND2			1		/* 1« line */
#define	PRS_2			2		/* 2  line */

/* page formats */
#define	PRF_DINA3			0

/* Alignment */
#define	PRA_LEFT			0
#define	PRA_CENTER		1
#define	PRA_RIGHT			2

/* Lettersizes */
#define	PRTL_PICA			0
#define	PRTL_ELITE		1
#define	PRTL_COND			2
#define	PRTL_EXPANDED		3

#define	PRT_H

#endif

