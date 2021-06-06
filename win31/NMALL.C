/*
 * NMALL.C
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: memory managment 
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauž
 */

# include 	<dos.h>
# include	<windows.h>
# include 	"editab.h"
# include	"pksedit.h"

# define WinMalloc(n)		GlobalAlloc(GMEM_MOVEABLE,n)
# define MAXMEMBLOCKS		32
# define MEMBLOCKSIZE		64000

typedef struct tagLIST near 	*PLIST;
typedef struct tagLIST far 	*LPLIST;
typedef struct tagHEADER far	*PMEMORY;

typedef struct tagLIST {
	PLIST	next;
	unsigned 	size;
} LIST;

typedef struct tagHEADER {
	GLOBALHANDLE 	hndl;
	void _seg 	*seg;
	int			naccess;
	PLIST		allocp;
	LIST 		base;
} HEADER;

void alert(LPSTR fmt, ...);
void _free(void far *Ap);

/*--------------------------------------------------------------------------
 * nomemory()
 */
void nomemory(void)
{
	ed_error(IDS_MSGNOSPCE); 
}

/*--------------------------------------------------------------------------
 * destroy()
 */
void destroy(char **ap)
{
	if (*ap) {
		_free(*ap);
		*ap = 0;
	}
}

/*--------------------------------------------------------------------------
 * stralloc()
 */
unsigned char *stralloc(unsigned char *buf)
{	unsigned char *d;

	if ((d = _alloc(lstrlen(buf)+1)) != 0) {
		lstrcpy(d,buf);
	}
	return d;
}

