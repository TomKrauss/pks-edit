
#if !defined(__FONT)
# define	__FONT

typedef struct fonttab {
	char *name;		/* font name */
	int	ffhandle;		/* index to fast font tab or -1 */
	int	gdoshandle;	/* Gdos Handle */
	int	point;		/* point size of font */
	int  cw,ch;		/* character cell width and height */
	int	bw,bh;		/* width and height rel. to left cell and baseline */
	int  minc,maxc;	/* minimum and maximum char defined */
} FONTTAB;

extern FONTTAB		_fonts[],*_fontp;

extern void 		g_text();
extern void 		(*_gtext)();
extern int		_fastenable,_nfonts,_aktfont,_fnt_normal,_fnt_small;
extern int		_vgcheight,_vgcwidth,_ch_height,_ch_width;

void estabfont(int ffno, struct wininfo *wp);

#endif		/* __FONT */

