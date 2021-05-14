/*
 * PKSMFUNC.H
 *
 * pksedit module function exports
 *
 * PROJECT: PKSEDIT Editor
 *
 * (c) Pahlen & Krauž					Author : TOM
 * All Rights Reserved.					created: 27.01.1991
 *									
 */

# ifndef PKSMFUNC_H
# define	PKSMFUNC_H

# define	searchfile(fn)			(*(edx->searchfile))(fn)
# define	Readfile(a,b,c)		(*(edx->Readfile))(a,b,c)
# define	Form_EdAlert(a,b)		(*(edx->Form_EdAlert))(a,b)
# define	form_do(a,b)			(*(edx->form_do))(a,b)
# define	Aes_draw(a,b)			(*(edx->Aes_draw))(a,b)
# define	Aes_undraw(a,b)		(*(edx->Aes_undraw))(a,b)

# define	lnlistfree(l)			(*(edx->lnlistfree))(l)
# define	stepnofsel(s,f)		(*(edx->stepnofsel))(s,f)
# define	opennofsel(s,l,f)		(*(edx->opennofsel))(s,l,f)
# define	EdFileAbandon(i)			(*(edx->EdFileAbandon))(i)
# define	selnfile(s)			(*(edx->selnfile))(s)
# define	mkpname(p,f)			(*(edx->mkpname))(p,f)

# define	_alloc(s)				(*(edx->_alloc))(s)
# define	_free(s)				(*(edx->_free))(s)
# define	stralloc(s)			(*(edx->stralloc))(s)

# define	fsel_title(s)			(*(edx->fsel_title))(s)
# define	file_select(s,p,f)		(*(edx->file_select))(s,p,f)
# define	fseltarget			edx->fseltarget
# define	edcurrfile			*(edx->curr)

# define	abort()				(*(edx->abort))()
# define	checkkey(k,c)			(*(edx->checkkey))(k,c)

# define	find_expr(dir,Ln,Col,lp,ebuf)	\
							(*(edx->find_expr))(dir,Ln,Col,lp,ebuf)

# define	u_init(void )			(*(edx->u_init))(void )
# define	EdUndo(void )			(*(edx->EdUndo))(void )

# define	bl_free(buf)			(*(edx->bl_free))(buf)
# define	bl_cut(pp,l1,l2,c1,c2,freeflg,colflg)	\
		(*(edx->bl_cut))(pp,l1,l2,c1,c2,freeflg,colflg)
# define	bl_paste(pp,l1,l2,c1,c2,freeflg,colflg)	\
		(*(edx->bl_paste))(pp,l1,l2,c1,c2,freeflg,colflg)
# define	bl_read(fn,pb,rs)	(	*(edx->bl_read))(fn,pb,rs)
# define	bl_write(fn,pb,mode)	(*(edx->bl_write))(fn,pb,mode)
# define	bl_join(pd,p2)			(*(edx->bl_join))(pd,p2)
# define	bl_addrbyid(id,insert)	(*(edx->bl_addrbyid))(id,insert)

/* accessing lines */
# define	mod_line(fp,lp,col1,col2)	(*(edx->mod_line))(fp,lp,col1,col2)
# define	ln_replace(fp,oldlp,newlp)	(*(edx->ln_replace))(fp,oldlp,newlp)
# define	ln_create(buffersize)		(*(edx->ln_create))(buffersize)
# define	ln_insert(fp,lpwhere,lpins)	(*(edx->ln_insert))(fp,lpwhere,lpins)
# define	ln_EdCharDelete(fp,lp)			(*(edx->ln_EdCharDelete))(fp,lp)
# define	ln_join(fp,ln1,ln2,delflg)	(*(edx->ln_join))(fp,ln1,ln2,delflg)
# define	ln_split(fp, lc, pos2, pos1)	(*(edx->ln_split))(fp, lc, pos2, pos1)
# define	ln_cnt(lps,lpe)			(*(edx->ln_cnt))(lps,lpe)
# define	ln_relative(lps, l)			(*(edx->ln_relative))(lps, l)
# define	ln_find(fp,lp)				(*(edx->ln_find))(fp,lp)
# define	ln_findbit(lp,bit)			(*(edx->ln_findbit))(lp,bit)

/* marks */
# define	mark_goto(fp,id,ln,col)		(*(edx->mark_goto))(fp,id,ln,col)
# define	mark_set(fp,lp,offs,c)		(*(edx->mark_set))(fp,lp,offs,c)

/* Icons */
# define	ic_isicon(obnu)			(*(edx->ic_isicon))(obnu)
# define	ic_address(icnum)			(*(edx->ic_address))(icnum)
# define	ic_EdCharDelete(i)				(*(edx->ic_EdCharDelete))(i)
# define	ic_init()					(*(edx->ic_init))()
# define	ic_add(id, x, y, sno, c, p)	(*(edx->ic_add))(id, x, y, sno, c, p)
# define	ic_walk(id, f, par)			(*(edx->ic_walk))(id, f, par)
# define	ic_name2idx(name)			(*(edx->ic_name2idx))(name)

# define	treeaddr(obj)				(*(edx->treeaddr))(obj)

# define	getexecparams(prog,par,errfile,opt) \
						(*(edx->getexecparams))(prog,par,errfile,opt)

# define	macinsert(data,name,size,key)	(*(edx->macinsert))(data,name,size,key)

extern EDXPORTS 	*edx;

# endif	/* PKSMFUNC_H */
