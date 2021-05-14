/*
 * NMALL.C
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: memory managment 
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
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

static PMEMORY _mem[MAXMEMBLOCKS];
static int     _nmemblocks = 0;

/*--------------------------------------------------------------------------
 * nomemory()
 */
void nomemory(void)
{
	ed_error(IDS_MSGNOSPCE); 
}

/*--------------------------------------------------------------------------
 * shareAlloc()
 */
void *shareAlloc(void)
{	
	static char *szRet;
	
	if (!szRet) {
		HANDLE handle;

		if ((handle = GlobalAlloc(GMEM_FIXED|GMEM_DDESHARE,256)) != 0)
			szRet = GlobalLock(handle);
		if (!szRet)
			nomemory();
	}
	return szRet;
}

/*--------------------------------------------------------------------------
 * findslot()
 */
static int near findslot(void _seg *seg)
{	int 		i;
	PMEMORY	pmem;

	for (i = 0; i < MAXMEMBLOCKS; i++) {
		if ((pmem = _mem[i]) != 0 && pmem->seg == seg) {
			return i;
		}
	}
	return -1;
}

/*--------------------------------------------------------------------------
 * FreeMemBlock()
 */
static int near FreeMemBlock(int i)
{	
	PMEMORY 	p;

	if ((p = _mem[i]) == 0) {
		alert("bad memblock free");
		return 0;
	}
		
	if (!GlobalUnlock(p->hndl)) {
		GlobalFree(p->hndl);
	} else {
		alert("failed unlocking 0x%x",p->hndl);
		return 0;
	}

	_nmemblocks--;
	if (_nmemblocks > i)
		memcpy(&_mem[i],&_mem[i+1],(_nmemblocks-i)*sizeof _mem[0]);
	_mem[_nmemblocks] = 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * AllocMemBlock()
 */
static int near AllocMemBlock(void)
{
	GLOBALHANDLE   hndl;
	unsigned 		size;
	PMEMORY		p;
	LPLIST		q;

	if (_nmemblocks >= MAXMEMBLOCKS ||
	    (hndl = WinMalloc(MEMBLOCKSIZE)) == 0L ||
	    (p = (PMEMORY)GlobalLock(hndl)) == 0) {
		nomemory();
		return 0;
	}
	p->seg = (void _seg*)FP_SEG(p);
	if (findslot(p->seg) >= 0) {
		alert("double allocated segment");
		return 0;
	}
	
	p->base.next = p->allocp = (PLIST)(&p->base);
	p->base.size = 0;
	p->naccess = 2;
	p->hndl = hndl;
	_mem[_nmemblocks++] = p;

	size = GlobalSize(hndl) - sizeof *p - 3*sizeof p->base;

	q = (LPLIST)((char*)p+sizeof *p);
	q->size = size;
	_free(q+1);

	p->naccess = 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * _free()
 */
void _free(void far *Ap)
{
	void huge		*ap = Ap;
	void _seg		*seg = (void _seg*) FP_SEG(ap);
	LPLIST 		next,allocp,p,q;
	PMEMORY		pmem;
	int			slot;
	long			big;

	if ((slot = findslot(seg)) < 0) {
		alert("bad free slot for segment 0x%x",seg);
		return;
	}

	pmem = _mem[slot];

	allocp = MK_FP(seg,(unsigned)pmem->allocp);
	p = (LPLIST)ap-1;

	for (big = 1000000L, q = allocp; !((p>q) && (FP_OFF(p)<(unsigned)q->next)); 
		q = MK_FP(seg,(unsigned)q->next)) {
		if (FP_OFF(q)>=(unsigned)q->next) {
			 if (FP_OFF(p) > FP_OFF(q)) {
				break;
			 }
			 if (FP_OFF(p) < (unsigned)q->next) {
				break;
			}
		}
		if (--big < 0) {
			alert("oops: bad free 0x%lx",ap);
			return;
		}
	}
 	if (FP_OFF(p)+p->size == (unsigned)q->next) {
 		next = seg+q->next;
		p->size += next->size;
		p->next  = next->next;
	} else {
		p->next = q->next;
	}
	if (FP_OFF(q)+q->size == FP_OFF(p)) {
		q->size += p->size;
		q->next  = p->next;
	} else {
		q->next = (PLIST)FP_OFF(p);
	}
	pmem->allocp = (PLIST)FP_OFF(q);
	if (--pmem->naccess <= 0)
		FreeMemBlock(slot);
}

/*--------------------------------------------------------------------------
 * dumpallocated()
 */
long dumpallocated(void)
{	int 		i;
	long		nRet;
	PMEMORY	pmem;

	nRet = 0;
	for (i = 0; i < MAXMEMBLOCKS; i++) {
		if ((pmem = _mem[i]) != 0) {
			nRet += pmem->naccess;
		}
	}
	return (long) nRet;
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
 * allocmem()
 */
static LPSTR near allocmem(int slot, unsigned needbytes)
{
	LPLIST 		p,q;
	PMEMORY 		pmem;
	void _seg		*seg;

	if ((pmem = _mem[slot]) == 0)
		return 0;

	seg = pmem->seg;
	q = (LPLIST)(seg+pmem->allocp); 

 	for (p = (LPLIST)(seg+q->next); ; 
 		q=p, p = (LPLIST)(seg+p->next)) {
		if (p->size >= needbytes) {
			if (p->size == needbytes) {
				q->next = p->next;
			} else {
				p->size -= needbytes;
 				p = (LPLIST)((char *)p+p->size);
				p->size = needbytes;
			}
			pmem->allocp = (PLIST)FP_OFF(q);
			break;
		}
		if (FP_OFF(p)==FP_OFF(pmem->allocp))
			return 0;
	}

	pmem->naccess++;
	return (LPSTR)(p+1);
}

/*--------------------------------------------------------------------------
 * _alloc()
 */
void far *_alloc(unsigned nbytes)
{	PMEMORY *p;
	int starttry;
	static int _lastalloc;
	unsigned needbytes;
	void far *ret;

	needbytes = ((nbytes+2*sizeof(LIST)-1)/sizeof(LIST))*sizeof(LIST);
	if ((starttry = _lastalloc) >= _nmemblocks)
		starttry = _lastalloc = 0;
	while (1) {
		if ((ret = allocmem(_lastalloc,needbytes)) != 0)
			return ret;
		_lastalloc++;
		if (_lastalloc >= _nmemblocks)
			_lastalloc = 0;
		if (_lastalloc == starttry)
			break;
	}
	if (AllocMemBlock() == 0)
		return (void far *)0;
	_lastalloc = _nmemblocks-1;
	return allocmem(_lastalloc,needbytes);
}

/*--------------------------------------------------------------------------
 * farmalloc()
 */
EXPORT void far *farmalloc(unsigned nbytes)
{
	return _alloc(nbytes);
}

/*--------------------------------------------------------------------------
 * calloc()
 */
EXPORT void *calloc(size_t nitems, size_t nbytes)
{	void far *d;

	nbytes *= nitems;

 	d = _alloc(nbytes);
	if (d) {
		blfill(d,nbytes,0);
	}
	return d;
}

/*--------------------------------------------------------------------------
 * farcalloc()
 */
EXPORT void far *farcalloc(size_t nitems, size_t nbytes)
{
	return calloc(nitems, nbytes);
}

/*--------------------------------------------------------------------------
 * farfree()
 */
void farfree(void far *Ap)
{
	_free(Ap);
}

/*--------------------------------------------------------------------------
 * farrealloc()
 */
#pragma argsused
EXPORT void far *farrealloc(unsigned nbytes, void *old)
{
	alert("bargh: realloc");
	return 0;
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

/*	   

main()
{
	register i;

	gemdos(2,7);
	for (i = 1; i < 10000; i++) if (!_alloc(i)) {
		printf("%d\n",i); break;
	}
	gemdos(2,7);
	gemdos(7);
}
*/
