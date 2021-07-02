/*
 * PaintEditorWindowAttributes.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: redraw attributed text
 *
 * 										created: Mai 1990 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "trace.h"
#include "edierror.h"
#include "lineoperations.h"
#include "winfo.h"
#include "winutil.h"

#define  	ESCAPE		'\033'

#define	WP_BOLD		0x1
#define	WP_LIGHT		0x2
#define	WP_KURSIVE	0x4
#define	WP_UNDERLINED	0x8
#define	WP_SUPER		0x10		/* Text Attributbit Superscript	*/
#define	WP_SUB		0x20
#define	HYPHEN		''

extern int 	indent_calculateTabStop(int col, EDIT_CONFIGURATION *l);

/*--------------------------------------------------------------------------
 * SelectTextAttribute()
 */
static void SelectTextAttribute(HDC hdc,
						  WINFO *wp, 
						  unsigned char attribut, 
						  unsigned char olda)
{	int   a;
	EDFONTATTRIBUTES fontStyle;

	a = attribut & 0xF;
	if (a == olda) {
		return;
	}
	fontStyle.italic = a & WP_KURSIVE ? 1 : 0;
	fontStyle.underline = a & WP_UNDERLINED ? 1 : 0;
	fontStyle.weight = a & WP_BOLD ? FW_BOLD : FW_NORMAL;
	font_selectDefaultEditorFont(wp,hdc,&fontStyle);
}

/*--------------------------------------------------------------------------
 * graf_text()
 */
static int  graf_text(HDC hdc,	
				  unsigned char *b,
				  unsigned char *d,
				  WINFO *wp,
				  int x, int y,
				  unsigned char attribut,
				  unsigned char olda)
{	int delta;

	if ((delta = (int)(d-b)) == 0)
		return x;

	SelectTextAttribute(hdc,wp,attribut,olda);
	if (attribut & WP_LIGHT) {
		COLORREF color = RGB(255, 0, 0);
		if (color == (COLORREF)wp->fnt_fgcolor) {
			color = RGB(0, 0, 0);
		}
		SetTextColor(hdc, color);
	} else {
		SetTextColor(hdc,wp->fnt_fgcolor);
	}
	TextOut(hdc,x,y,b,delta);

	return x + LOWORD(win_getTextExtent(hdc, b, delta));
}

/*--------------------------------------------------------------------------
 * ww_calculateLineAttributes()
 * In preparation for rendering "attributed rendering" - calculate the line rendering
 * attributes for a line.
 */
static void ww_calculateLineAttributes(LINE* lp)
{
	unsigned char* s, * send, a, e;

	a = 0;
	e = ESCAPE;
	lp->attr = a;
	s = lp->lbuf;
	send = &lp->lbuf[lp->len];
	while (s < send) {
		if (*s++ == e) {
			a = *s++;
		}
	}
}

/*--------------------------------------------------------------------------
 * render_singleLineWithAttributesOnDevice()
 */
int render_singleLineWithAttributesOnDevice(HDC hdc, int x, int y, WINFO *wp, LINE* lp) {
	char 				buf[512];
	unsigned char *		d,c;
	unsigned char *		l,*lend,*ret;
	unsigned char 		attribut,olda;
	register 			col;
	int 	    		ctrl;
    int					ind;
	EDIT_CONFIGURATION* lin;
	unsigned char* b = buf;
	int start, end;

	start = wp->mincol;
	end = wp->maxcol + 1;
	int textlen = end - start;
	if (textlen >= sizeof buf) {
		textlen = sizeof buf - 1;
		end = textlen + start;
	}
	d    = b;
	col  = 0;
	olda	= 0;
	lin = (FTPOI(wp))->documentDescriptor;
	l    = lp->lbuf;
	lend = &lp->lbuf[lp->len];
	ww_calculateLineAttributes(lp);
	attribut = lp->attr;
	ctrl = PLAINCONTROL(wp->dispmode);
	ret  = b;

	while(col < start && l < lend) {
		c = *l++;
		if (c == ESCAPE) { 
			attribut = *l++; 
		} else if (c == '\t' && ctrl == 0) {
			if ((col = indent_calculateTabStop(col, lin)) > start) {
				if (col > end) {
					col = end;
				}
				d += (col-start);
				break;
			}
		} else {
			col++;
		}
	}

	while (col < end && l < lend) {
		if ((c = *l++) < ' ') {
			if (c == ESCAPE) {
				x = graf_text(hdc,b,d,wp,x,y,attribut,olda);
				ret += (d-b);
				d = b;
				olda = attribut;
				attribut = *l++;
				continue;
			}
			if (ctrl == 0) {
				if (c == HYPHEN) {
					c = '-';
				} else if (c == '\t') {
					ind = indent_calculateTabStop(col, lin);
					if (ind > end) {
						ind = end;
					}
					memset(d, lin->t1, ind-col);
					d += ind - col;
					col = ind;
					continue;
				} else {
					c = ' ';
				}
			} else {
				if (c == 0) {
					c = '0';
				}
			}
		}
		*d++ = c;
		col++;
	}
	x = graf_text(hdc,b,d,wp,x,y,attribut,olda);
	SelectTextAttribute(hdc,wp,0,olda);
	return (int)(d - b);
}

