/*
 * ResourceFileIO.c
 *
 * PROJEKT: PKS-EDIT/Resource Compiler for MS - WINDOWS 3.0.1
 *
 * purpose: get and put resources to resource file
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include	<windows.h>
#include	<edtypes.h>
#include	<stdio.h>
#include	<tos.h>
#include	"edfuncs.h"
#include	"resource.h"

extern void 	*ll_insert(void *head,long size);
extern MACRO 	*mac_byindex(int idx);
extern char * 	strmaxcpy(char *pszDest, char *pszSource, int nMax);

extern int	_nmacros;
extern char	*_macroname;

typedef struct macrodata {
	char			cmdbyte;
	unsigned char	namelen;			/* including 0byte */
	unsigned char	commentlen;		/* including 0byte */
	unsigned char 	s1,s2;
	unsigned char 	name[1];
} MACRODATA;

/*--------------------------------------------------------------------------
 * FlushBuffer()
 */
long _flushsize;
int FlushBuffer(int fd, char *buffer, int size, int rest)
{
	if (!size)
		return 1;

	if (Fwrite(fd,size,buffer) >= 0) {
		_flushsize += size;
		if (Fseek(0l,fd,SEEK_CUR) != _flushsize) {
			_flushsize = -1;
			NoDiskSpace();
		} else {
			if (rest) {
				memmove(buffer,&buffer[size],rest);
			}
			return 1;
		}
	} else {
		WriteError();
	}
	return 0;
}

/*------------------------------------------------------------
 * rsc_rdmacros()
 */
#pragma argsused
char *rsc_rdmacros(char *name, unsigned char *p, unsigned char *pend)
{
	int  		len;
	struct macrodata *seqp;
	unsigned char  *datap,*comment;

	do {
		seqp  = (struct macrodata *) p;
		comment = &seqp->name[seqp->namelen];
		datap = comment+seqp->commentlen;
		len   = (seqp->s1 << 8) + seqp->s2;
		p     = &datap[len];
		if (p > pend)
			break;
		if (seqp->cmdbyte != CMD_MACRO) {
			return 0;
		}
		if (mac_insert(seqp->name, comment, datap, len, 0) < 0) {
			return 0;
		}
	} while(p < pend);

	return (unsigned char *) seqp;
}

/*------------------------------------------------------------
 * rsc_wrbuf()
 */
long rsc_wrbuf(int fd, long offset, char *buf, long maxbytes)
{
	if ((long)Fwrite(fd,maxbytes,buf) != maxbytes)
		return -1;
	return maxbytes;
}

/*------------------------------------------------------------
 * rsc_wrtables()
 */
int rsc_wrtables(int fd, char *itemtype, RSCTABLE *rp)
{
	while(rp) {
		if (rsc_put(fd,itemtype,rp->rt_name,0,
				  rsc_wrbuf,rp->rt_data,(long)rp->rt_size) == 0)
			return 0;
		rp = rp->rt_next;
	}
	return 1;
}

/*------------------------------------------------------------
 * rsc_wrmacros()
 */
long rsc_wrmacros(int fd,long offset, char *buf, long maxbytes)
{
	int 		offs,i;
	long		total;
	MACRO  	*mp;
	struct macrodata *seqp;
	unsigned char *datap,*comment;

	offs = 0;
	_flushsize = offset;
	total = 0;

	for (i = 0; i < _nmacros; i++) {
		if ((mp = mac_byindex(i)) != 0 &&
		    (_macroname == 0 || strcmp(_macroname,mp->name) == 0)) {
			if (offs >= maxbytes) {
				offs -= maxbytes;
				total += maxbytes;
				if (!FlushBuffer((int)fd,buf,(int)maxbytes,offs))
					return -1;
			}
			seqp = (struct macrodata *) &buf[offs];
			seqp->cmdbyte 	= CMD_MACRO;
			seqp->namelen = strlen(mp->name)+1;
			seqp->commentlen = strlen(MAC_COMMENT(mp))+1;
			strcpy(seqp->name,mp->name);
			comment = &seqp->name[seqp->namelen];
			strcpy(comment,MAC_COMMENT(mp));
			datap = comment+seqp->commentlen;
			seqp->s1 = mp->size >> 8;
			seqp->s2 = mp->size;
			memmove(datap,MAC_DATA(mp),mp->size);
			offs += (mp->size + (int)(datap-(unsigned char *)seqp));
		}
	}
	total += offs;
	FlushBuffer(fd,buf,offs,0);
	return total;
}

/*--------------------------------------------------------------------------
 * rsc_loadheader()
 */
int rsc_loadheader(int fd, RSCHEADER *hp)
{
	Fseek(0L,fd,SEEK_SET);

	if (Fread(fd,sizeof *hp,hp) != sizeof *hp) {
		return -1;
	}

	if (hp->rs_version != RS_VERSION || 
	    hp->rs_nentities > MAX_ENTITIES) {
		return -2;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * rsc_finditem()
 */
int rsc_finditem(int startitem, char *itemtyp, char *itemname, RSCHEADER *hp)
{
	int 		i;
	ENTITY	*ep;

	for (i = startitem; ; i++) {
		if (i >= (int)hp->rs_nentities) {
			return -1;
		}
		ep = &hp->rs_ent[i];
		if (strcmp(itemtyp,ep->en_typ) == 0 &&
		    (itemname == 0 || strcmp(ep->en_name,itemname) == 0)) {
			return i;
		}
	}
}

/*--------------------------------------------------------------------------
 * rsc_seektoitem()
 */
int rsc_seektoitem(int fd, int startitem, char *itemtyp, char *itemname, RSCHEADER *hp)
{
	int  i;

	if ((i = rsc_finditem(startitem,itemtyp,itemname,hp)) >= 0) {
		Fseek(hp->rs_ent[i].en_offset,fd,SEEK_SET);
		return i;
	}
	return -3;
}

/*--------------------------------------------------------------------------
 * rsc_find()
 */
int rsc_find(int fd, char *itemtyp, char *itemname, RSCHEADER *hp)
{
	int	i;

	if ((i = rsc_loadheader(fd,hp)) < 0)
		return i;

	return rsc_seektoitem(fd, 0, itemtyp, itemname, hp);
}

/*--------------------------------------------------------------------------
 * rsc_put()
 */
int rsc_put(int fd, char *itemtyp, char *itemname, int replace, 
		  long (*wrfunc)(int ,long ,char *, long ),char *buffer, long bufsize)
{
	long 		size;
	long		offset;
	int  		nItem = 0;
	int			ret = 1;
	RSCHEADER	h;
	ENTITY *	ep;

	if ((nItem = rsc_find(fd, itemtyp, itemname, &h)) < 0) {
		nItem = h.rs_nentities++;
		offset = Fseek(0L,fd,SEEK_END);
	} else if (!replace) {
		return 0;
	} else {
		/* not really a replace ............... */
	}

	if (offset < 0 || (size = (*wrfunc)(fd,offset,buffer,bufsize)) < 0)
		return 0;

	ep = &h.rs_ent[nItem];
	strcpy(ep->en_typ,itemtyp);
	if (itemname)
		strcpy(ep->en_name,itemname);
	else
		ep->en_name[0] = 0;
	ep->en_offset = (unsigned short) offset;
	ep->en_size = (unsigned short) size;
	h.rs_size += size;

	Fseek(0L,fd,SEEK_SET);

	if (Fwrite(fd,sizeof h,&h) < 0) {
		ret = 0;
	}

	return ret;
}

/*--------------------------------------------------------------------------
 * rsc_create()
 */
int rsc_create(char *fname, int trunc)
{
	int		fd;
	RSCHEADER	h;

	if (!trunc) {
		if ((fd = Fopen(fname,OF_READWRITE)) > 0)
			return fd;
	}
	if ((fd = Fcreate(fname,0)) < 0)
		return fd;

	blfill(&h,sizeof h,0);

	h.rs_version = RS_VERSION;
	h.rs_nentities = 0;
	h.rs_size = h.rs_hdrsize = sizeof h;
	if (Fwrite(fd,sizeof h,&h) < 0) {
		Fclose(fd);
		return -1;
	}
	return fd;
}

/*--------------------------------------------------------------------------
 * rsc_enumitems()
 */
int rsc_enumitems(RSCFILE *rp, char *itemtyp, int (*func)(char *, ENTITY *))
{
	ENTITY *	ep;
	int 		i;

	for (i = 0; i < (int)rp->rf_hdr->rs_nentities; i++) {
		ep = &rp->rf_hdr->rs_ent[i];
		if (strcmp(itemtyp,ep->en_typ) == 0) {
			if (!(*func)(ep->en_name,ep))
				return 0;
		}
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * rsc_open()
 */
RSCFILE *rsc_open(char *fn, int mode)
{
	int 		fd;
	RSCHEADER	hdr;
	RSCFILE	*rp;

	if (mode != RSC_O_READ)
		return 0;
	if ((fd = Fopen(fn,0)) == HFILE_ERROR) {
		OpenError(fn);
		return 0;
	}

	if (rsc_loadheader(fd,&hdr) < 0)
		return 0;

	if ((rp = (RSCFILE *)_alloc(sizeof *rp)) == 0 ||
	    (rp->rf_hdr = (RSCHEADER *)_alloc(sizeof *rp->rf_hdr)) == 0) {
		Fclose(fd);
		return 0;
	}
	memmove(rp->rf_hdr,&hdr,sizeof hdr);
	rp->rf_mode = mode;
	rp->rf_fd = fd;

	return rp;
}

/*--------------------------------------------------------------------------
 * rsc_close()
 */
int rsc_close(RSCFILE *rp)
{
	int ret = Fclose(rp->rf_fd);

	_free(rp->rf_hdr);
	_free(rp);
	return ret;
}

/*--------------------------------------------------------------------------
 * rsc_load()
 * load a resource with indicated type
 * if an item type may have several sub_items(itemname), setting
 * itemname to (char*)0 indicates: enumerate loading for all
 * subtypes
 */
int rsc_load(RSCFILE *rp, char *itemtyp, char *itemname, char *(*cnvfunc)())
{
	char		*p;
	ENTITY	*ep;
	int		nItem,ret;

	nItem = 0;

	while((nItem = rsc_seektoitem(rp->rf_fd, nItem, itemtyp, itemname, rp->rf_hdr)) >= 0) {
		ep = &rp->rf_hdr->rs_ent[nItem];
		if ((p = _alloc(ep->en_size)) == 0) {
			return 0;
		}

		if (Fread(rp->rf_fd,ep->en_size,p) == ep->en_size) {
			ret = (*cnvfunc)(ep->en_name,p,p+ep->en_size) ? 1 : 0;
		} else {
			ret = 0;
		}

		_free(p);
		if (!ret || itemname)
			break;

		nItem++;
	}

	return ret;
}

/*------------------------------------------------------------
 * rsc_findtable()
 */
RSCTABLE *rsc_findtable(RSCTABLE *rp, char *pszTarget)
{
	char		name[RSC_NAMELEN];

	strmaxcpy(name, pszTarget, RSC_NAMELEN);

	for (;rp ; rp = rp->rt_next) {
		if (strcmp(rp->rt_name, name) == 0) {
			return rp;
		} 
	}
	return 0;
}

/*------------------------------------------------------------
 * rsc_switchtotable()
 */
int rsc_switchtotable(RSCTABLE **Rp, char *pszTarget)
{
	RSCTABLE	*rp;
	char		name[RSC_NAMELEN];

	strmaxcpy(name, pszTarget, RSC_NAMELEN);

	for (rp = *Rp ; rp; rp = rp->rt_next) {
		if (strcmp(rp->rt_name,name) == 0) {
			ll_top(Rp,rp);
			return 1;
		} 
	}
	if ((rp = ll_insert(Rp,sizeof *rp)) == 0) {
		return 0;
	}

	strcpy(rp->rt_name,name);
	return 1;
}

/*------------------------------------------------------------
 * rsc_tablefindcode()
 */
void *rsc_tablefindcode(RSCTABLE *rp, int itemsize, unsigned code)
{
	char *kp;

	for (kp = rp->rt_data; kp < (char*)rp->rt_end; kp += itemsize) {
		if (((KEYBIND*)kp)->keycode == code)
			return kp;
	}
	return (char *)0;
}

/*------------------------------------------------------------
 * rsc_tableresize()
 */
void *rsc_tableresize(RSCTABLE *rp, int itemsize, void *noalloc, BOOL (*emptyFunc)(void *))
{
	unsigned 	size = rp->rt_size + 32*itemsize;
	char *		kp;
	char *		data;
	char *		kp2;

	if ((data = _alloc(size)) == 0) {
		return 0;
	}
	blfill(data, size, 0);
	kp2 = data;
	for (kp = rp->rt_data; kp < (char *)rp->rt_end; ) {
		if (!(*emptyFunc)(kp)) {
			memmove(kp2,kp,itemsize);
			kp2 += itemsize;
		}
		kp += itemsize;
	}

	if (rp->rt_data && rp->rt_data != noalloc) {
		_free(rp->rt_data);
	}
	rp->rt_data = data;
	rp->rt_size = size;
	rp->rt_end =  data+size;
	return kp2;
}

/*------------------------------------------------------------
 * rsc_tablefindcode()
 */
#if 0
void *rsc_tablelookup(RSCTABLE *rp, int itemsize, void *noalloc, unsigned code) 
{
	char *kp,*kpbest = 0;

	for (kp = rp->rt_data; kp < rp->rt_end; kp += itemsize) {
		if (((KEYBIND*)kp)->keycode == code)
			return kp;
		if (((KEYBIND*)kp)->keycode == 0 && kpbest == 0)
			kpbest = kp;
	}
	if (kpbest)
		return kpbest;
	return rsc_tableresize(rp,itemsize, noalloc);
}
#endif
