
#ifndef EDITORFONT_H

#define EDITORFONT_H

/*
 * Font and color description for a text.
 */
typedef struct tagEDFONT {
	char		name[32];
	short		height;
	short		weight;
	long		fgcolor;
	long		bgcolor;
	short		charset;
	short		width;
	short		strikeout;
	char		bOem;
	char		res1;
} EDFONT;

#endif