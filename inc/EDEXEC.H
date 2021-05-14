
#ifndef EDEXEC_H


/*
 * options for execution
 */
# define	EX_RDIN		0x1
# define	EX_RDOUT		0x2
# define	EX_DELBLK		0x4
# define	EX_WAIT		0x8
# define	EX_CLOSEWI		0x10
# define	EX_CLEARSCR		0x20
# define	EX_TEXTCURS		0x40
# define	EX_SHOWMOUSE	0x80
# define	EX_READERR		0x100	/* read errorfile (like -E ... ) */
# define	EX_DEBUG		0x200	/* dont execute, debug only */
# define	EX_PROMPT		0x400	/* prompt for parameters */
# define	EX_SETPATH		0x800	/* cd before exec */
# define	EX_ACC			0x1000	/* Applikation call */
# define	EX_NOSAVE		0x2000	/* dont ask, whether to save files */
# define	EX_WCLIP		0x4000	/* write GEM clipboard before exec */
# define	EX_RCLIP		0x8000	/* read GEM Clipboard after exec */

# define	EX_NEEDSFILE	(EX_RCLIP|EX_WCLIP|EX_RDIN|EX_RDOUT|EX_DELBLK)

/*
 * combinations according to application type
 */
# define	EX_GEMFLAGS		(EX_CLOSEWI|EX_SHOWMOUSE)
# define	EX_TOSFLAGS		(EX_CLOSEWI|EX_CLEARSCR|EX_TEXTCURS)
# define	EX_TTPFLAGS		(EX_CLOSEWI|EX_CLEARSCR)
# define	EX_CONVERT		(EX_DELBLK|EX_RDIN|EX_RDOUT)
# define	EX_CONSUM		(EX_RDOUT|EX_CLOSEWI)
# define	EX_PRODUCE		EX_RDIN
# define	EX_OVERLAY		0

# define	EXE_NOTDONE		-100

# define	EDEXEC_H
#endif

