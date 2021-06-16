/*
 * RESOURCE.H
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: definition of macro file structure
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */

# ifndef RESOURCE_H

#define	MAX_ENTITIES			16
#define	RS_VERSION			0x00000900L
#define 	RSC_NAMELEN			10

typedef unsigned short OFFSET;

typedef struct tagRSCTABLE  {
	struct		tagRSCTABLE *rt_next;
	char		rt_name[RSC_NAMELEN];
	int			rt_size;
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

#define	RSC_O_READ		0
#define	RSC_O_CREAT		1
#define	RSC_O_UPDATE		2

typedef struct tagRSCFILE {
	int		rf_fd;
	int		rf_mode;
	RSCHEADER	*rf_hdr;
} RSCFILE;

extern RSCFILE*	rsc_open(char *fn, int mode);
extern int 		rsc_close(RSCFILE *rp);
extern int 		rsc_load(RSCFILE *rp, char *itemtyp, char *itemname, char *(*cnvfunc)());
extern void *	rsc_tablefindcode(RSCTABLE *rp, int itemsize, unsigned code);
extern void*	rsc_tableresize(RSCTABLE* rp, int itemsize, void* noalloc, BOOL(*emptyFunc)(void*));
extern RSCTABLE* rsc_findtable(RSCTABLE *rp, char *pszTarget);
extern int 		rsc_switchtotable(RSCTABLE **Rp, char *pszTarget);
extern int		rsc_create(char* fname, int trunc);
extern int rsc_put(int fd, char* itemtyp, char* itemname, int replace,
	long (*wrfunc)(int, long, char*, long), char* buffer, long bufsize);

/*--------------------------------------------------------------------------
 * file_flushBuffer()
 * Flush a resource file buffer writing out the number of specified bytes and
 * moving the ramainder of the unflushed data to the beginning of the buffer.
 * Return -2, if the disk is full, 1 upon success and 0 for other failures.
 */
extern int file_flushBuffer(int fd, char* buffer, int size, int rest);
extern int rsc_wrtables(int fd, char* itemtype, RSCTABLE* rp);

#define	RESOURCE_H
# endif


