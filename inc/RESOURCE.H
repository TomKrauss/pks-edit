/*
 * RESOURCE.H
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: definition of macro file structure
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

# ifndef RESOURCE_H

# define	MAX_ENTITIES			16
# define	RS_VERSION			0x00000900L
# define 	RSC_NAMELEN			10

typedef unsigned short OFFSET;

typedef struct tagRSCTABLE  {
	struct tagRSCTABLE *rt_next;
	char		rt_name[RSC_NAMELEN];
	int		rt_size;
	void		*rt_data;
	void 	*rt_end;
} RSCTABLE;

typedef struct tagENTITY {
	char		en_typ[RSC_NAMELEN];	/* type name: KEYS, MACROS, ... */
	char		en_name[RSC_NAMELEN];	/* for named entities: key tables... */
	OFFSET	en_offset;
	OFFSET	en_size;
} ENTITY;

typedef struct tagRSCHEADER {
	long 	rs_version;
	long		rs_size;
	unsigned	rs_hdrsize;
	unsigned	rs_nentities;
	ENTITY	rs_ent[MAX_ENTITIES];
} RSCHEADER;

# define	RSC_O_READ		0
# define	RSC_O_CREAT		1
# define	RSC_O_UPDATE		2

typedef struct tagRSCFILE {
	int		rf_fd;
	int		rf_mode;
	RSCHEADER	*rf_hdr;
} RSCFILE;

RSCFILE *	rsc_open(char *fn, int mode);
int 		rsc_close(RSCFILE *rp);
int 		rsc_load(RSCFILE *rp, char *itemtyp, char *itemname, char *(*cnvfunc)());
void *	rsc_tablefindcode(RSCTABLE *rp, int itemsize, unsigned code);
void *	rsc_tableresize(RSCTABLE *rp, int itemsize, void *noalloc, int (*empty)(void *p));
RSCTABLE *rsc_findtable(RSCTABLE *rp, char *pszTarget);
int 		rsc_switchtotable(RSCTABLE **Rp, char *pszTarget);

# define	RESOURCE_H
# endif


