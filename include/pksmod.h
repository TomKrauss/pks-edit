/*
 * PKSMOD.H
 *
 * Interface between Moduls for PKSEDIT and the master itself
 *
 */

# ifndef _PKSMOD_H
#define	_PKSMOD_H

#ifndef	EDITAB_H
typedef void FTABLE;
typedef void LINE;
typedef void PASTE;
typedef void MARK;
#endif

#ifndef	_EDFUNCS_H
#define	EDBINDS	long
#define	KEYCODE	unsigned
#define	KEYBIND	void
#endif

#ifndef 	__AES__
#define	OBJECT	void 
#endif

#ifndef	ICCALL_H
#define	ICCALL	long
#endif

#ifndef	EDLBOX_H
#define	LISTBOX	void
#endif

#define	PKSM_EDMAGIC		0x504B535DL
#define	PKSM_MODMAGIC		0x50434B5DL

#define	PKSM_OK			0
#define	PKSM_FAIL			1
#define	PKSM_WRONGARG		2
#define	PKSM_STOLEN		1313
#define	PKSM_ILLVERS		1212

typedef int (*INTFUNC)();

typedef struct modxport {
	char		*id;			/* ID for function */
	INTFUNC	f;			/* Functions which are exported */
} MODXPORT;

typedef struct edx {
	FTABLE	**curr,*_ftable;

	/* AES Support */
	void 	(*Aes_draw  )(OBJECT *tree,int centered);
	void 	(*Aes_undraw)(OBJECT *tree,int centered);
	int		(*form_do   )(OBJECT *tree,int edit_obj);
	int		(*Form_EdAlert)(int def_but, const char *b);

	/* Memory */
	void 	*(*_alloc)(unsigned size);
	unsigned char *(*stralloc)(unsigned char *buf);
	void 	(*_free)(void *p);

	/* Files */
	int 		(*Readfile)(FTABLE *fp,char *fn,int linflag);
	void 	(*lnlistfree)(LINE *lp);
	char 	*(*file_searchFileInPKSEditLocation)(char *s);
	void 	(*stepnofsel)(char *fn,int cmpflg);
	int 		(*opennofsel)(char *fn,long ln,int closeother);
	char 	*(*mkpname)(char *path,char *fn);
	int		(*selnfile)(char *name);
	int		(*EdFileAbandon)(int reopen);

	/* Fsel */
	int		(*file_select)(char *path,char *fnam,char *pattern);
	void 	(*fsel_setDialogTitle)(char *title);
	char 	*fseltarget;

	/* black copy check */
	int		(*checkkey)(unsigned char *key,unsigned char *crypt);

	/* div. */
	int		(*abort)(void );

	EDBINDS	*_bindings;
	LINE		*(*find_expr)(int dir,long *Ln,long *Col,LINE *lp,char *ebuf);
	long		*_options;

	/* undo */
	void		(*u_init)(void );
	int		(*EdUndo)(void );

	/* block ops */
	void 	(*bl_free)(PASTE *buf);
	int		(*bl_cut)(PASTE *pp,LINE *l1,LINE *l2,
					int c1,int c2,int freeflg,int colflg);
	int 		(*bl_paste)(PASTE *pb, FTABLE *fp, LINE *lpd, 
					  int col, int colflg);
	int		(*bl_read)(char *fn, PASTE *pb, int rs);
	int		(*bl_write)(char *fn, PASTE *pb,int mode);
	int 		(*bl_join)(PASTE *pd,PASTE *p2);
	PASTE 	*(*bl_addrbyid)(int id,int insert);

	/* accessing lines */
	LINE 	*(*mod_line)(FTABLE *fp,LINE *lp,int col1,int col2);
	void 	(*ln_replace)(FTABLE *fp,LINE *oldlp,LINE *newlp);
	LINE 	*(*ln_create)(int buffersize);
	void 	(*ln_insert)(FTABLE *fp,LINE *lpwhere,LINE *lpins);
	int		(*ln_EdCharDelete)(FTABLE *fp,LINE *lp);
	LINE 	*(*ln_join)(FTABLE *fp,LINE *ln1,LINE *ln2,int delflg);
	LINE 	*(*ln_split)(FTABLE *fp, LINE *lc, int pos2, int pos1);
	long 	(*ln_cnt)(LINE *lps,LINE *lpe);
	LINE 	*(*ln_relative)(LINE *lps, long l);
	long 	(*ln_find)(FTABLE *fp,LINE *lp);
	LINE 	*(*ln_findbit)(LINE *lp,int bit);

	/* marks */
	LINE		*(*mark_goto)(FTABLE *fp, int c, long *ln, long *col);
	MARK		*(*mark_set)(FTABLE *fp, LINE *lp,int offs,int c);

	/* Icons */
	int 		(*ic_isicon)(int obnu);
	ICCALL 	*(*ic_address)(int icnum);
	void 	(*ic_EdCharDelete)(OBJECT *op);
	void 	(*ic_init)(void );
	int 		(*ic_add)(unsigned char id, int x, int y, int symno, 
				     unsigned char c, char **p);
	void 	(*ic_walk)(int id, int (*f)(OBJECT *op, ICCALL *ip, long p), long par);
	int 		(*ic_name2idx)(char *name);

	OBJECT	*(*treeaddr)(int obnum);
	int 		(*getexecparams)(char *prog,char *par,char *errfile,long *opt);

} EDXPORTS;

typedef struct pksmod {

	long 	edmagic;
	EDXPORTS  *edxports;

	long		modmagic;
	char		*modid;
	char		*modname;

	int  	nxports;
	INTFUNC	modexit;
	MODXPORT	*modxports;

	void		*p_basepag;	/* pointer to moduls base page */
	void 	*pksedtext;
	long 	pksedtsize;

} PKSMOD;

# endif

