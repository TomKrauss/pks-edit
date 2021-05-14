
# ifndef _EDEXPR_H

/*---------------------------------*/
/* Expression fÅr COMPILER-ERRORS	*/
/*---------------------------------*/
# define	MAXCOMPILER		8

typedef struct tagexpression {
	char *compiler;	/* Name of Compiler				*/
	char *source;		/* expression in words			*/
	char *fnbsl;		/* which \? is filename			*/
	char *lnbsl;		/* which \? is linenumber		*/
	char *rembsl;		/* which bsl is remark			*/
} TAGEXPR;

extern int		fm_getpos(char *tag, int hlpflg);
extern void 		fm_qpos(int hlpflg);

# define	_EDEXPR_H

#endif

