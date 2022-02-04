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

#ifndef	_PRINTING_H

typedef struct tagFONTSPEC {
	int		fs_oemmode;
	int		fs_cheight;
	int		fs_cwidth;
	char	fs_name[128];
} FONTSPEC;

typedef struct tagLNSPACE {		// line spacing encoded as a fraction
	int 		numerator;
	int 		denominator;
} LNSPACE;

typedef enum { PRTR_ALL, PRTR_SELECTION, PRTR_CURRENT_PAGE, PRTR_PAGES} PRINTRANGETYPE;

typedef struct tagPRINTRANGE {
	PRINTRANGETYPE prtr_type;	// Print selection, Print everything, Print the "current page", Print the range of pages specified below
	int   prtr_fromPage;		// If prtr_pages is true, this is the 1-based first page to print.
	int   prtr_toPage;			// If prtr_pages is true, this is the 1-based last page to print.
} PRINTRANGE;

typedef enum  { 
	PDT_NONE,		// do not draw a graphical deco for header / footers
	PDT_LINE,		// draw a single line between header / footer and the text
	PDT_FRAME,		// draw a rectangle around header / footer
	PDT_FILL_RECT	// fill the background of the header / footer with a grey rectangle
} PAGE_DECORATION_TYPE;

typedef struct tagPAGE_MARGIN_ELEMENT {
	char pme_template[512];			// The template from which the resulting text is created
	PAGE_DECORATION_TYPE pme_decoration;
	FONTSPEC pme_font;				// Font 
	int		pme_align;				// Header / Footer Alignment
									// left, center, right
	int		pme_lines;				// number of lines / height of this element in lines
	int		pme_margin;				// the margin (header to top and footer to bottom) of the page measured in lines.
	LNSPACE	pme_lineSpacing;		// line spacing in header / footer
} PAGE_MARGIN_ELEMENT;

typedef struct tagPRTPARAMS {	// print params
	/* params */
	int		options;
	int		pagelen;

	FONTSPEC font;
	int		lmargin,rmargin,nchars;
	int		tabsize;
	LNSPACE	lnspace;			// 1, 1«, 2 Lines

	PAGE_MARGIN_ELEMENT header;	// the header to paint
	PAGE_MARGIN_ELEMENT footer;	// the footer to paint
} PRTPARAM;

/* printing options */
#define	PRTO_LINE_NUMBERS						0x1			// print line #
#define	PRTO_SWAP_HEADER_FOOTER_ALIGNMENT		0x8			// swap h+f on alternate pages
#define	PRTO_WRAP_LONG_LINES					0x80		// wrap long lines
#define	PRTO_HEADERS							0x400		// print header and footer
#define	PRTO_SYNTAX_HIGHLIGHT					0x1000 // Display syntax highlighting

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

/*
 * Returns the current print configuration of PKS Edit.
 */
extern PRTPARAM* config_getPrintConfiguration();

#define	_PRINTING_H

#endif

