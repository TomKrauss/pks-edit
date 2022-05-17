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

typedef struct tagPRINT_LINE {
	int wrappedLineOffset;	// normally 0. WHen a line is printed, which is wrapped print_signleLineOfText is called for every "segment line" with this being the
							// offset of the lines printed before.
	void* pElement;				// current element to print - type of element depends on renderer.
	void* pLastElement;			// last element to print
	BOOL produceOutput;		// Whether output should be produced.
	int charHeight;			// character / line height
	int firstc;				// first character index to print (mostly 0)
	int lastc;				// last character index to print (mostly lp->len)
	long lineNumber;		// number of line to print
	long linesPrinted;		// The number of lines printed so far.
	int  pagenumber;		// Current page number
	int xPos;				// x position, where the printing starts
	int yPos;				// y position, where the printing starts
	int maxYPos;			// the available space in y direction
} PRINT_LINE;

typedef struct tagDEVEXTENTS {
	int			xPage, yPage;
	int			xLMargin, xRMargin;
	int			yHeaderPos, yTop,
		yFooterPos, yBottom;
	int			headerSpace, lineHeight;
} DEVEXTENTS;

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

/* Flags for printing */
typedef enum {
	PRT_CURRWI = 0x1,
	PRT_FILE = 0x2,
	PRT_CURRBLK = 0x3,
	PRT_CLIP = 0x4,
	PRT_TRASH = 0x5,
	PRT_TYPES = (1 | 2 | 3 | 4 | 5)
} PRINT_FLAGS;

/*------------------------------------------------------------
 * Print a file with a given name or the current file or selection.
 */
int EdPrint(PRINT_FLAGS what, const char* fname);


#define	_PRINTING_H

#endif

