
# ifndef _EDIFSEL_H

typedef struct fselinfo {
	char path[1024];
	char fname[256];
	char search[256];
} FSELINFO;

extern char	*rw_select(FSELINFO *fp, int title);
extern char *rw_init(FSELINFO *fp);
extern char	_fseltarget[];

#define	_EDIFSEL_H

# endif

