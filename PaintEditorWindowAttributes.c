/*
 * PaintEditorWindowAttributes.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: redraw attributed text
 *
 * 										created      : Mai 1990 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include 	"trace.h"
#include 	"edierror.h"
#include "lineoperations.h"
#include "winfo.h"

#define  	ESCAPE		'\033'

#define	WP_BOLD		0x1
#define	WP_LIGHT		0x2
#define	WP_KURSIVE	0x4
#define	WP_UNDERLINED	0x8
#define	WP_SUPER		0x10		/* Text Attributbit Superscript	*/
#define	WP_SUB		0x20
#define	HYPHEN		''

extern int 	TabStop(int col, DOCUMENT_DESCRIPTOR *l);

/*--------------------------------------------------------------------------
 * SelectTextAttribute()
 */
static void SelectTextAttribute(HDC hdc,
						  WINFO *wp, 
						  unsigned char attribut, 
						  unsigned char olda)
{	int   a;

	a = attribut & 0xF;
	if (a == olda)
		return;

	EdSetTextEffects(a&WP_KURSIVE,a&WP_UNDERLINED,a&WP_BOLD);
	EdSelectFont(wp,hdc);
	if (a)
		EdSetTextEffects(0,0,wp->fnt.weight);
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

	return x + LOWORD(GetTextExtent(hdc, b, delta));
}

/*--------------------------------------------------------------------------
 * writeattrline()
 */
char *writeattrline(HDC hdc, int x, int y,
	unsigned char *b, LINE *lp, int start, int end, WINFO *wp)
{
	register unsigned char *	d,c;
	register unsigned char *	l,*lend,*ret;
	unsigned char 			attribut,olda;
	register 				col;
	int 	    				ctrl;
     int					ind;
	DOCUMENT_DESCRIPTOR *				lin;

	d    = b;
	col  = 0;
	olda	= 0;
	lin = (FTPOI(wp))->documentDescriptor;
	l    = lp->lbuf;
	lend = &lp->lbuf[lp->len];
	attribut = lp->attr;
	ctrl = PLAINCONTROL(wp->dispmode);
	ret  = b;

	while(col < start && l < lend) {
		c = *l++;
		if (c == ESCAPE) { 
			attribut = *l++; 
		} else if (c == '\t' && ctrl == 0) {
			if ((col = TabStop(col, lin)) > start) {
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
					ind = TabStop(col, lin);
					if (ind > end) {
						ind = end;
					}
					d = blfill(d, ind-col, lin->t1);
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
	ret += (d-b);
	SelectTextAttribute(hdc,wp,0,olda);
	return ret;
}

/*--------------------------------------------------------------------------
 * mkattlist()
 */
void mkattlist(LINE *lp)
{	unsigned char *s,*send,a,e;

	a = 0;
	e = ESCAPE;
	while(lp) {
		lp->attr = a;
		s    = lp->lbuf;
		send = &lp->lbuf[lp->len];
		while (s < send)
			if (*s++ == e)
				a = *s++;
		lp = lp->next;
	}
}

