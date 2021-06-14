 /*
 * printing.h
 *
 * Print Interface for PKS Edit
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauß					Author : TOM
 * All Rights Reserved.					created: 03.04.1991
 *
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

typedef enum { PRTR_ALL, PRTR_SELECTION, PRTR_CURRENT_PAGE, PRTR_PAGES} PRINTRANGETYPE;

typedef struct tagPRINTRANGE {
	PRINTRANGETYPE prtr_type;		// Print selection, Print everything, Print the "current page", Print the range of pages specified below
	int   prtr_fromPage;			// If prtr_pages is true, this is the 1-based first page to print.
	int   prtr_toPage;				// If prtr_pages is true, this is the 1-based last page to print.
} PRINTRANGE;

typedef struct prtparams {			/* print params */
	/* params */
	PRINTRANGE printRange;
	int		mode;
	int		options;
	int		pagelen;

	FONTSPEC	font;
	int		lmargin,rmargin,nchars;
	int		tabsize;
	LNSPACE	lnspace;				/* 1, 1«, 2 Lines */

	int		marginTop;				// The number of lines to leave space at the top of the page.
	int		headerSize;				// The height of the header in lines
	int		marginBottom;			// The number of lines to leave space at the bottom of the page
	int		footerSize;				// The height of the footer in lines
	FONTSPEC htfont;				/* Font header - footer */
	char	header[512],
			footer[512];			/* text of header and footer */
	int		align;				/* Header / Footer Alignment */
								/* left, center, right */
	
	FONTSPEC	fnfont;
	LNSPACE	fnlnspace;			/* print_singleLineOfText spacing footnotes */
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

/*------------------------------------------------------------
 * print_readWriteConfigFile()
 * Read / write the print configuration of PKS edit depending
 * on the save flag passed as an argument.
 */
extern void print_readWriteConfigFile(int save);

#define	PRT_H

#endif

