/*
 * NMALL.C
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: memory managment 
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
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
void free(void far *Ap);

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
		free(*ap);
		*ap = 0;
	}
}

