 /*
 * printing.h
 *
 * Print Interface for PKS Edit
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 03.04.1991
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
	PRINTRANGETYPE prtr_type;	// Print selection, Print everything, Print the "current page", Print the range of pages specified below
	int   prtr_fromPage;		// If prtr_pages is true, this is the 1-based first page to print.
	int   prtr_toPage;			// If prtr_pages is true, this is the 1-based last page to print.
} PRINTRANGE;

typedef enum  { 
	PMDT_NONE,		// do not draw a graphical deco for header / footers
	PMDT_FRAME,		// draw a rectangle around header / footer
	PMDT_LINE,		// draw a single line between header / footer and the text
	PMDT_FILL_RECT	// fill the background of the header / footer with a grey rectangle
} 
PAGE_MARGIN_DECO_TYPE;

typedef struct tagPAGE_MARGIN_ELEMENT {
	char pme_text[512];
	PAGE_MARGIN_DECO_TYPE pme_decoType;
} PAGE_MARGIN_ELEMENT;

typedef struct tagPRTPARAMS {	// print params
	/* params */
	PRINTRANGE printRange;
	int		options;
	int		pagelen;

	FONTSPEC font;
	int		lmargin,rmargin,nchars;
	int		tabsize;
	LNSPACE	lnspace;			// 1, 1«, 2 Lines

	int		marginTop;			// The number of lines to leave space at the top of the page.
	int		headerSize;			// The height of the header in lines
	int		marginBottom;		// The number of lines to leave space at the bottom of the page
	int		footerSize;			// The height of the footer in lines
	FONTSPEC htfont;			// Font header - footer
	PAGE_MARGIN_ELEMENT header;	// the header to paint
	PAGE_MARGIN_ELEMENT footer;	// the footer to paint
	int		align;				// Header / Footer Alignment
								// left, center, right
	
	FONTSPEC	fnfont;
	LNSPACE	fnlnspace;			// print_singleLineOfText spacing footnotes
	int		fnlinelen;			// length of filename delimter line 
	int		fnd1,fnd2;			// distances to body and footer
	int		fnoffset;			// offset of Footnote
} PRTPARAM;

/* printing options */
#define	PRTO_LINES		0x1			// print line #
#define	PRTO_PAUSE		0x2			// pause between pages
#define	PRTO_SWAP		0x8			// swap h+f on alternate pages
#define	PRTO_ULFOO		0x20		// limitline footer
#define	PRTO_SWAPMARG	0x40		// swap margins on alternate pages
#define	PRTO_WRAP		0x80		// wrap long lines
#define	PRTO_EVEN		0x100		// print even pages
#define	PRTO_ODD 		0x200		// print odd pages
#define	PRTO_HEADERS	0x400		// print header and footer
#define	PRTO_KEEPS		0x800		// Keeps....
#define	PRTO_SYNTAX_HIGHLIGHT 0x1000 // Display syntax highlighting

// linespacing
#define	PRS_1			0		// 1 line
#define	PRS_1AND2		1		// 1,5 lines
#define	PRS_2			2		// 2 lines

// Page formats 
#define	PRF_DINA3		0

// Alignment 
#define	PRA_LEFT		0
#define	PRA_CENTER		1
#define	PRA_RIGHT		2

// Lettersizes 
#define	PRTL_PICA		0
#define	PRTL_ELITE		1
#define	PRTL_COND		2
#define	PRTL_EXPANDED	3

/*------------------------------------------------------------
 * print_readWriteConfigFile()
 * Read / write the print configuration of PKS edit depending
 * on the save flag passed as an argument.
 */
extern void print_readWriteConfigFile(int save);

#define	PRT_H

#endif

