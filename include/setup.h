
# define	MAXPATLEN		12	/* maximum length of file searchpattern  */
# define	MAXPATHLEN		256	/* maximum pathname length */

typedef struct setup {
	long 		magic;
	int  		npl;
	int			ioffsets[6];
	char		deffnext[256];
	char		fselext [MAXPATLEN*6];
	char		fselpath[MAXPATHLEN*3];
	long		options;
} SETUP;

extern SETUP _setup;

