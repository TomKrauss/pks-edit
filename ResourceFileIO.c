/*
 * ResourceFileIO.c
 *
 * PROJEKT: PKS-EDIT/Resource Compiler for MS - WINDOWS
 *
 * purpose: get and put resources to resource file
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "alloc.h"
#include <edtypes.h>
#include <stdio.h>
#include <tos.h>
#include "edfuncs.h"
#include "linkedlist.h"
#include "documentmodel.h"
#include "resource.h"
#include "edierror.h"
#include "stringutil.h"

extern MACRO 	*macro_getByIndex(int idx);

extern int	_nmacros;
extern char	*_macroname;

typedef struct macrodata {
	char			cmdbyte;
	unsigned char	namelen;			/* including 0byte */
	unsigned char	commentlen;		/* including 0byte */
	unsigned char 	s1,s2;
	unsigned char 	name[1];
} MACRODATA;

/*------------------------------------------------------------
 * rsc_rdmacros()
 */
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
		if (macro_insertNewMacro(seqp->name, comment, datap, len) < 0) {
			return 0;
		}
	} while(p < pend);

	return (unsigned char *) seqp;
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
	total = 0;

	for (i = 0; i < _nmacros; i++) {
		if ((mp = macro_getByIndex(i)) != 0 &&
		    (_macroname == 0 || strcmp(_macroname,mp->name) == 0)) {
			if (offs >= maxbytes) {
				offs -= maxbytes;
				total += maxbytes;
				if (file_flushBuffer((int)fd,buf,(int)maxbytes,offs) < 1)
					return -1;
			}
			seqp = (struct macrodata *) &buf[offs];
			seqp->cmdbyte 	= CMD_MACRO;
			seqp->namelen = (unsigned char)(strlen(mp->name)+1);
			seqp->commentlen = (unsigned char)strlen(MAC_COMMENT(mp))+1;
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
	if (file_flushBuffer(fd, buf, offs, 0) < 1) {
		return -1;
	}
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
	long		offset = 0;
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

	memset(&h,0,sizeof h);

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
		err_openFailed(fn);
		return 0;
	}

	if (rsc_loadheader(fd,&hdr) < 0)
		return 0;

	if ((rp = (RSCFILE *)malloc(sizeof *rp)) == 0 ||
	    (rp->rf_hdr = (RSCHEADER *)malloc(sizeof *rp->rf_hdr)) == 0) {
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

	free(rp->rf_hdr);
	free(rp);
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
	char	*p;
	ENTITY	*ep;
	int		nItem;
	int		ret = 0;

	nItem = 0;

	while((nItem = rsc_seektoitem(rp->rf_fd, nItem, itemtyp, itemname, rp->rf_hdr)) >= 0) {
		ep = &rp->rf_hdr->rs_ent[nItem];
		if ((p = malloc(ep->en_size)) == 0) {
			return 0;
		}

		if (Fread(rp->rf_fd,ep->en_size,p) == ep->en_size) {
			ret = (*cnvfunc)(ep->en_name,p,p+ep->en_size) ? 1 : 0;
		} else {
			ret = 0;
		}

		free(p);
		if (!ret || itemname)
			break;

		nItem++;
	}

	return ret;
}

