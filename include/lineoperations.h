/*
 * lineoperations.h
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * maintain list of lines: delete, insert, modify,....
 *
 * 										created      : 10.02.87
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */


# ifndef	EDITAB_H
#define	EDITAB_H

#include <crtdefs.h>
#include "edtypes.h"
#include "editorfont.h"

#define	ED_VERSION	0x20

#define	LN_ALIGN		2		/* make sure a small linebuffer 	 */
#define	FBUFSIZE		8192		/* maximum filebuffer- and linesize */

#define	LINEBUFSIZE		2*FBUFSIZE+160	/* sizeof of _linebuf		 */
#define	MAXLINELEN		4096

#define	MAXUNDO		10		/* maximum of UNDO-Buffers avail	 */
#define	MAXSHIFT		36		/* maximum shiftwidth 			 */
#define	RM_DELTA		2		/* delta to automatic right margin  */

/*-------- TYPES --------------*/

typedef struct line   {
	int  		len; 		/* linelength in chars */
	unsigned char	lflg;		/* line flags */
	unsigned char	attr;		/* attribute byte */
	struct line	*prev,*next;
	char 		lbuf[LN_ALIGN];
} LINE;

typedef struct tagLINEINDIRECT {
	int			typ;
	union {
		struct {
			long		deepCount;
			long		count;
			LINE *	list;
		} ind;
	} u;
} LINEINDIRECT;

#define	LI_HIDDENLIST		0x0
#define	LI_RULER			0x10

#define	LpIndirect(lp)		((LINEINDIRECT *)(lp)->lbuf)
#define	LpIndirectTyp(lp)	((LINEINDIRECT *)(lp)->lbuf)->typ
#define	LpIndHiddenList(lp)	LpIndirect(lp)->u.ind.list
#define	LpIndNHidden(lp) 	LpIndirect(lp)->u.ind.count
#define	LpIndNTotal(lp) 	LpIndirect(lp)->u.ind.deepCount
#define	LpHasHiddenList(lp)	(((lp)->lflg & LNINDIRECT) && \
							LpIndirectTyp(lp) == LI_HIDDENLIST)

#define	LSTRUCSIZE		(sizeof(LINE) - LN_ALIGN)
#define	LINESIZE(l) 	((long)(LSTRUCSIZE + l->len)) 	 

extern char * _linebuf;

/*---------------------------------*/
/* Pastebuffer					*/
/*---------------------------------*/
typedef struct pastebuf {
	LINE 	*pln;		/* list of lines */
	long 	size,nlines;	/* size & nr of lines */
	int  	pflg;		/* used for macros */
} PASTE;

typedef struct pastelist {
	struct pastelist	*next;
	int  			id;	/* name of buffer */
	PASTE			pbuf;
} PASTELIST;

/*---------------------------------*/
/* EDIT-OPS					*/
/*---------------------------------*/
#define	O_MODIFY			0		/* modify a line */
#define	O_INSERT			1		/* insert a line */
#define	O_DELETE			2		/* EdCharDelete a line */
#define	O_MARK			3		/* mark a textblock */
#define	O_LNORDER		4		/* reorder linelist according to a table */
#define	O_HIDE			5		/* hide a list of lines */
#define	O_UNHIDE			6		/* unhide a list of lines */

/*---------------------------------*/
/* MARKS						*/
/*---------------------------------*/
typedef struct mark {
	struct mark	*next;
	int  		mchar;		/* ID of mark (char || BLOCKSTART,-END)	*/
	LINE 		*lm;
	int  		lc;
} MARK;

/* special marks */

#define	CNOMARK		0x00		/* release mark without freeing it */
#define	MARKSTART		0x100	/* cut&paste - start */
#define	MARKEND		0x101	/*	  "	   - end */

#define	MARKSELSTART	0x102
#define	MARKSELEND		0x103

#define	MARKDOT		0x104

/*---------------------------------*/
/* LINEAL						*/
/*---------------------------------*/
#define	fnt_name		fnt.name
#define	fnt_point		fnt.point
#define	fnt_fgcolor	fnt.fgcolor
#define	fnt_bgcolor	fnt.bgcolor
#define	fnt_charset	fnt.charset

#if defined(_WINUSER_)

typedef struct lineal {
	int			lmargin,rmargin;
	int			tabsize,shiftwidth;
	int			nl,nl2,cr;			/* Lineends */
	int			dispmode;			/* show control... */
	int			workmode;			/* watch brackets, abbrev... */
	int			fno;				/* # of screen font */
	char			liname[16];		/* name of the lineal */
	int			id;					/* # ID for context check */
	char			t1,fillc;		/* Tabulator Character (fill character) */
	char			u2lset[32];		/* wordset and u2l ("abc=xyz") */
	unsigned char 	tbits[512];		/* Bitset real Tabstops */
	unsigned char	statusline[60];	/* the special status */
	WINDOWPLACEMENT	placement;		/* for windows with fixed size */
	EDFONT		fnt;				/* font */
	char			bak[4];			/* Backup extension */
	unsigned char	modename[16];	/* document type name */
	unsigned char  tagtag[12];		/* private tag tag */
	unsigned char	vl[20];			/* "Vorlage" Makro on creation */
	unsigned char	linkey[16];		/* key macro file */
	int			scrollflags;		/* thumbtrack.. */
	int			cursaftersearch;	/* postop, ... */
	int			vscroll;			/* scroll n Lines */
	int			scroll_dy;			/* scroll on dist dy to screen */
	unsigned char	cm[24];			/* makro for closing */
	unsigned char	ts[256];		/* fast access 1st 256 Tabstops */
	unsigned char	res2[256];		/* reserved too .... */
} LINEAL;

extern LINEAL _lineal;

#endif

extern char 	bittab[];

#define	TABTHERE(lin,i)		(lin->tbits[i >> 3] &  bittab[i & 07])
#define	TABPLACE(lin,i)		(lin)->tbits[i >> 3] |= bittab[i & 07]
#define	TABCLEAR(lin,i)		(lin)->tbits[i >> 3] &= (~bittab[i & 07])

/* default Lineal context (Makros are global) */
#define	LIN_DEFCTX	0
#define	LIN_NOCTX		-1
#define	LIN_NOTYETREAD(lp)	(lp->lmargin == -1)

typedef time_t EDTIME;

/*---------------------------------*/
/* Infos about the files			*/
/*---------------------------------*/
typedef void * HIDDENP;

#define	WIPOI(fp)	((WINFO*)fp->wp)
#define	UNDOPOI(fp)	((UNDOBLOCK*)fp->undo)

typedef struct ftable 
{
	struct ftable *next;
	char 	fname[256];
	long 	nlines,ln;
	long	hexoffset;
	long 	lastln,lastcol;	/* start of previous search .. */
	int  	lastmodoffs,		/* last offset for last mdofikation */
			col;   
	int  	flags;
	int  	lnoffset; 		/* current Offset in Linebuffer Memory	*/
	LINE 	*tln;			/* Pointer to current edited line */	    
	LINE 	*firstl,			/* first line */
			*currl,			/* current line */
			*lastl;			/* last line */
	MARK 	*fmark;
	MARK 	*blstart,*blend;   	/* Marks for Block Operations			*/
	int  	blcol1,blcol2;		/* colomn for Blockmarks				*/
	struct lineal	*lin;
	HIDDENP	wp;
	HIDDENP	undo;
	long 	as_time;			/* next time for AUTOSAVE */
	EDTIME	ti_modified;		/* last modification time */
	EDTIME	ti_created;		/* creation time */
	EDTIME  ti_saved;			/* last save time */
	int		lockFd;			/* Filedescriptor for locking - <= 0 if none */
} FTABLE;

extern FTABLE 	*_currfile,*_filetab;
extern int 	_playing;				/* recorder plays its game ... */

void 	edidebug(char *fmt,...);
unsigned char *blfill(void *buf,int count,unsigned char fillbyte);
int 	_chkblk(FTABLE *fp);
int 	chkblk(FTABLE *fp);

/*-------- line ops ----------*/

LINE *ln_hide(FTABLE *fp, LINE *lp1, LINE *lp2);
LINE *ln_settmp(FTABLE *fp,LINE *lp,LINE **lpold);
LINE *ln_modify(FTABLE *fp,LINE *lp,int col1,int col2);
LINE *ln_create(int size);
long ln_find(FTABLE *fp,LINE *lp);
void ln_insert(FTABLE *fp,LINE *pos,LINE *lp);
int ln_delete(FTABLE *fp,LINE *lp);
void lndel_lines(FTABLE *fp);
LINE *ln_cut(LINE *lp,int physize,int start,int end);
LINE *ln_break(FTABLE *fp, LINE *linep, int col);
LINE *ln_split(FTABLE *fp, LINE *lc, int pos2, int pos1);
LINE *ln_join(FTABLE *fp,LINE *ln1,LINE *ln2,int flg);
int lnjoin_lines(FTABLE *fp);
LINE *ln_goto(FTABLE *fp,long l);
LINE *ln_relative(LINE *cl, long l);
LINE *ln_crelgo(FTABLE *fp, long l, long *o);
LINE *ln_relgo(FTABLE *fp,long l);
LINE *ln_gotouserel(FTABLE *fp,long ln);
long ln_cnt(LINE *lps,LINE *lpe);
LINE *ln_findbit(LINE *lp,int bit);
void ln_replace(FTABLE *fp,LINE *oln,LINE *nl);
void ln_m(LINE *lpstart,LINE *lpend,int flg);
void ln_um(LINE *lpstart,LINE *lpend,int flg);
long ln_needbytes(LINE *lp, int nl, int cr);
/*--------------------------------------------------------------------------
 * SelectRange()
 *
 * Select a range of text in the file identified by fp.
 */
extern int SelectRange(int rngetype, FTABLE* fp, MARK** markstart, MARK** markend);

/*
 * Typical data structure for a linked list.
 */
typedef struct linkedList {
	struct linkedList* next;
	char name[4];
} LINKED_LIST;


/*--------------------------------------------------------------------------
 * ll_top()
 * put an element to the top of the linked list
 */
extern int ll_top(void** pointerLinkedList, void* elem);

/*--------------------------------------------------------------------------
 * ll_insert()
 * insert an element to a linked list
 */
extern void* ll_insert(void** pointerLinkedList, long size);

/*--------------------------------------------------------------------------
 * ll_kill()
 * destroy a linked list. Pass a pointer to the head of the linked list and
 * an additional destruction function for one element in the list.
 */
extern void ll_kill(void** pointerLinkedList, int (*destroy)(void* elem));

/*--------------------------------------------------------------------------
 * ll_delete()
 * delete an element in a linked list.Return 1 if the element was successfully deleted.
 */
extern int ll_delete(void** pointerLinkedList, void* element);

/*--------------------------------------------------------------------------
 * ll_count()
 * Count the elements in a linked list.
 */
extern int ll_count(void* linkedList);

/*-------- FILE FLAGS ----------*/

#define	F_INUSE			0x01 	/* Mark filestructure as used	  */ 
#define	F_MODIFIED			0x02 	/* File has been modfied 	  */
#define	F_NEWFILE			0x04 	/* new File to be created	  */
#define	F_APPEND			0x08 	/* append to existing File	  */
#define	F_NORMOPEN			0x10 	/* normal File 			  */
#define	F_SAVEAS			0x20 	/* File is saved under new name */
#define	F_WFORCED			0x40 	/* Forced writing, if not mod.  */
#define	F_ISBACKUPPED		0x80		/* Backup already created	  */
#define	F_STDLINEAL			0x200	/* Use a reasonable standard lineal */
#define	F_HASWIN			0x400	/* file has an associated window */
#define	F_NEEDSAUTOSAVE		0x800 	/* File is not autosaved 	  */
#define	F_HIDDEN			0x1000	/* create File hidden		  */
#define	F_RDONLY			0x2000	/* create File read only		  */
#define	F_ISHELPFILE		0x4000	/* File is a Help Window File   */
#define	F_CHANGEMARK		(F_NEEDSAUTOSAVE|F_MODIFIED)

/*---------- LINEFLAGS ---------*/

#define	LNMARKED			0x01 	/* at least one lineposition marked */
#define	LNDIFFMARK		0x02 	/* Mark for last EdFilesCompare action */
#define	LNREPLACED		0x04 	/* something in line has been replaced */
#define	LNCPMARKED		0x08 	/* Line is within CUT&PASTE-Block	 */
#define	LNNOCR			0x10		/* mark not chapter lines */
#define	LNINDIRECT		0x20		/* indirect flag */
#define	LNNOTERM			0x40		/* unterminated line */
#define	LNXMARKED			0x80		/* marked for EdCharDelete ...	*/

/*---- SIZE OF EXPRESSION BUFFERS -----------------------------*/

#define	ESIZE		200		 	/* maximum Expressionsize		 */

#ifndef 	DIM
#define	DIM(x)		(sizeof(x)/sizeof(x[0]))
#endif

#ifndef HIDDEN
#define HIDDEN		0x2
#endif

/* far modell pointer compare ... bargh..! */
#define P_GT(p1,p2)		((void EDFAR*)(p1) > (void EDFAR*)(p2))
#define P_LT(p1,p2)		((void EDFAR*)(p1) < (void EDFAR*)(p2))
#define P_EQ(p1,p2)		((void EDFAR*)(p1) == (void EDFAR*)(p2))
#define P_GE(p1,p2)		((void EDFAR*)(p1) >= (void EDFAR*)(p2))
#define P_NE(p1,p2)		((void EDFAR*)(p1) != (void EDFAR*)(p2))

#endif	/* EDITAB_H */
