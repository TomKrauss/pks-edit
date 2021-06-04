
#ifndef EDITORFONT_H

#define EDITORFONT_H


/*
 * The style of a selected font 
 */
typedef struct tagEDFONTSTYLE {
	char		strikeout : 1;
	char		bOem : 1;
	char		italic : 1;
	char		underline : 1;
	short		weight;			// 0 == default font everything else between 0 and 999 bolden factor
} EDFONTSTYLE;
/*
 * Font and color description for a text.
 */
typedef struct tagEDFONT {
	char		name[32];
	short		height;
	short		res1;
	long		fgcolor;
	long		bgcolor;
	short		charset;
	short		width;
	EDFONTSTYLE style;
} EDFONT;


#endif