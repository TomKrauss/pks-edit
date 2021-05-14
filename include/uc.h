
# ifndef	UC_H

#define	BRT_MATCH		0
#define	BRT_INDENT	1

#define	UA_SHOWMATCH	1
#define	UA_ABBREV		2
#define	UA_BRINDENT	3
#define	UA_UCMACRO	4

#define	UA_MPLEN		8

struct uclist {
	struct uclist *next;
	int		ctx;				/* -> current lineal */
	char		*pat;			/* pattern for scanning */
	int		len;				/* len of pattern (or -1 for RE) */
	int  	action;			/* type of action to perform */
	void		*p;				/* pointer to actions Parameter */
};

typedef struct uclist UCLIST;

struct uclist *uc_find(int, char *, int , int );

#define	UC_H

# endif

