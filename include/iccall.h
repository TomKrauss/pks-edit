
# ifndef	ICCALL_H

#define	ICACT_DCLICKED		0		/* icon was double clicked */
#define	ICACT_TEXTBLOCK	1		/* textblock was dragged */
#define	ICACT_DROPPED		2		/* icon #... was dragged onto icon */
#define	ICACT_BYTYPE		0		/* icon with type ... " */

/*
 * Icon Types
 */
#define	ICID_TRASH		'T'
#define	ICID_FILE			'F'
#define	ICID_DIR			'D'
#define	ICID_EXEC			'X'
#define	ICID_CLIP			'C'
#define	ICID_UNDO			'U'
#define	ICID_PRINT		'P'
#define	ICID_EDIT			'E'

typedef struct iccall {
	char		ictype;		/* id character for PKSEDIT.SET and reference */
	char		icsym;		/* icon(!)-idx of symbol in symbol tree */
	char		*cmd;		/* param 1 */
	char		*params;		/* ... */
	char		*errfile;
	int		options;
} ICCALL;

typedef struct icparams {
	char *num,*name,*p1,*p2,*p3;
} ICPARAMS;

extern int 	ic_type(HWND hwnd);
extern int	ic_init(void);
extern char 	*ic_param(LPSTR szBuff, HWND hwnd,int n);

#define	ICCALL_H

# endif

