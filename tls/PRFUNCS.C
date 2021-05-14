/*
 * PRFUNCS.C
 *
 * print function and key tables for PKSEDIT according to format description
 *
 * PROJECT: PKSEDIT/Tools
 *
 * (c) Pahlen & Krauž					Author : TOM
 * All Rights Reserved.					created: 28.01.1991
 *									
 */

#define Extern

# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "edfuncs.h"
# include "parsefu.h"

#define MAXFUNCS	128
#define DIM(x)		(sizeof(x)/sizeof(x[0]))

extern void yyerror(char *msg, ...);
extern int yylineno;

# define M_index	macref.index
# define M_typ		macref.typ

# if !(defined(min))
extern int min(int a, int b);
extern int max(int a, int b);
# endif

static	char *_comments[MAXCMDS];

KEYBIND	_keymaptab[MAXMAPKEY];

int _excode;

FILE *_fpout;

void LoadString() {}
void mac_byindex() {}

/*--------------------------------------------------------------------------
 * keybound()
 */
static KEYBIND *keybound(KEYCODE k)
{
	KEYBIND *kp;

	for (kp = _keymaptab; kp<&_keymaptab[DIM(_keymaptab)]; kp++) {
		if (kp->keycode == k)
			return kp;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * keygetbind()
 */
static KEYBIND *keygetbind()
{
	KEYBIND *kp;

	for (kp = _keymaptab; kp<&_keymaptab[DIM(_keymaptab)]; kp++) {
		if (kp->keycode == 0)
			return kp;
	}
	return 0;
}

#if defined(MACROS20)
/*--------------------------------------------------------------------------
 * s2shift()
 */
int s2shift(char **k)
{	char *K = *k;
	int  control = 0;

	while(K[1]) {
		switch (*K) {
			case CTL_MARK:	control |= (K_CONTROL>>8); break;
			case SFT_MARK: control |= (K_SHIFT>>8); break;
			case ALT_MARK: control |= (K_ALTERNATE>>8); break;
			default : goto out;
		}
		K++;
	}
out:
	*k = K;
	return control;
}

/*--------------------------------------------------------------------------
 * key2code()
 */
KEYCODE key2code(char *K)
{	char *t,*k;
	int  code,control;
	extern char	*_scantab;
	
	control = s2shift(&K);

	k = K;

	if (!k[1]) {
		if ((*k >= '0' && *k <= '9') ||
		    (*k >= 'A' && *k <= 'Z')) {
			code = *k;
			goto ret;
		}
	}

	if (k[0] == '\\') {
		code  = (int)(k[1] - '0') * 100;
		code += (int)(k[2] - '0') * 10;
		code += (int)(k[3] - '0');
		goto ret;
	}

	for (t = _scantab; *t; ) {
		code = *t++;
		for (k = K; *k == *t; k++,t++)
			if (*t == 0) {
ret:				return (control<<8)|code;
			}
		while(*t++)
			;
	}
	return K_INVALID;
}

/*---------------------------------*/
/* inskey()					*/
/*---------------------------------*/
void inskey(char *keystring, char *cmdname, int i)
{	KEYBIND *kp;
	unsigned short key;

	if (*keystring == 0 || keystring[1] == 0) return;
	key = key2code(keystring+1);
	if (key == K_DELETED) {
		yyerror("invalid key %s in line %d",keystring+1,yylineno);
		return;
	}
	if (keybound(key) != 0) {
		yyerror("key %s(%d) double declared in line %d",
			   keystring,key,yylineno);
		return;
	}
	if ((kp = keygetbind()) == 0) {
		yyerror("too many key defs line %d",yylineno);
		return;
	}
	kp->keycode	= key;
	kp->M_typ		= CMD_CMDSEQ;
	kp->M_index	= i;
}

#else

/*---------------------------------*/
/* inskey()					*/
/*---------------------------------*/
void inskey(char *keystring, char *cmdname, int i)
{	KEYBIND *kp;
	KEYCODE key;
	int	   mindex;

	if (*keystring == 0 || keystring[1] == 0) return;
	key = key2code(keystring+1);
	kp  = keygetbind();
	if (kp->keycode) {
		fprintf(stderr,"key %s double declared in line %d\n",
			   keystring,yylineno);
		_excode = 1;
	} else {
		kp->keycode	= key;
		if (strlen(code2key(key)) <= 4 && (mindex = findmentry(cmdname)) >= 0) {
			kp->M_typ	= CMD_MENU;
			kp->M_index	= mindex;
		} else {
			kp->M_typ	= CMD_CMDSEQ;
			kp->M_index	= i;
		}
	}
}

# endif

/*--------------------------------------------------------------------------
 * hash_comment()
 */
void hash_comment(int cmd, char *comment)
{
	if (cmd >= DIM(_comments)) {
		yyerror("comment table overflow");
		return;
	}
	if (_comments[cmd]) {
		yyerror("oops: redefining comment %d",cmd);
	}
	_comments[cmd] = comment;
}

/*---------------------------------*/
/* some porting functions		*/
/*---------------------------------*/
void mac_formaterr() {}
void YYtrace(char *s) {
	fprintf(stderr,"%s\n",s);
}

/*---------------------------------*/
/* sortlist()					*/
/* (Quicksort nach K&R)			*/
/*---------------------------------*/
static void sortlist(KEYBIND *tab,int n)
{    long	gap,i,j;
	KEYBIND tmp;
	short k1,k2;

     for (gap = n/2; gap > 0; gap >>= 1)
      for (i = gap; i < n; i++) {
       for (j = i-gap; j >= 0; j -= gap) {
	  	k1 = tab[j].keycode;
		k2 = tab[j+gap].keycode;
		if (( (k1 & 0xFF) == (k2 & 0xFF) && (k1 & 0xFF00) < (k2 & 0xFF00)) ||
		    ( (k1 & 0xFF) <  (k2 & 0xFF))) 
			break;
		tmp = tab[j]; tab[j] = tab[j+gap]; tab[j+gap] = tmp;
       }
	 }
}

/*---------------------------------*/
/* findmentry()				*/
/*---------------------------------*/
static int findmentry(char *cmdname)
{	int i;

	for (i = 0; i < _currmenu; i++) {
		if (strcmp(_allmenus[i].cmdname,cmdname) == 0)
			return i;
	}
	return -1;
}

/*---------------------------------*/
/* pr_extern()					*/
/*---------------------------------*/
static void pr_extern(void )
{	int i;

	fprintf(_fpout,"extern int\n");
	for (i = 0; i < _currfunc; i++) {
		if (i) 
			fprintf(_fpout,i % 4 == 0 ? ",\n" : ", ");
		fprintf(_fpout,"%s(long )",_allfuncs[i].name);
	
	}
	fprintf(_fpout,";\n\n");
}

/*---------------------------------*/
/* pr_cmdtab()					*/
/*---------------------------------*/
static void pr_cmdtab(void )
{	int i;

	for (i = 0; i < _currfunc; i++) {
		fprintf(_fpout,"\"%s\",\tCD_EDFUNCP,\t%d,PAR_LONG,\t-1,\n",
				_allfuncs[i].ident,
				_allfuncs[i].idx);
	}
}

/*--------------------------------------------------------------------------
 * quotepars()
 */
static char *quotepars(signed char *p)
{
	static char qp[50],*d,*pend;

	for (d = qp, pend = p+MAXPARS; p < pend && *p != PAR_VOID; p++ ) {
		d += sprintf(d,"\\%03ho",(unsigned char)*p);
	}
	return qp;
}

/*---------------------------------*/
/* pr_functions()				*/
/*---------------------------------*/
static void pr_functions(void )
{	int i;

	fprintf(_fpout,"# include \"funcdef.h\"\n");

	fprintf(_fpout,"/* Functions: */\n");
	fprintf(_fpout,"EDFUNCDEF _functab[] = {");
	for (i = 0; i < _currfunc; i++) {
		if (i)
			fprintf(_fpout,",");
#if defined(ATARI_ST)
		fprintf(_fpout,"\n{ \"%s\",\t%d,\t\"%s\" }",
				_allfuncs[i].ident,
				_allfuncs[i].idx,
				quotepars(_allfuncs[i].fpars));
#else
		fprintf(_fpout,"\n{ \t%d,\t\"%s\" }",
				_allfuncs[i].idx,
				quotepars(_allfuncs[i].fpars));
#endif
	}
	fprintf(_fpout,"\n};\nint _nfunctions = %d;\n",_currfunc);

	fprintf(_fpout,"/* Own Types: */\n");
	fprintf(_fpout,"OWNTYPE _typetab[] = {");
	for (i = 0; i < _currtype; i++) {
		if (i)
			fprintf(_fpout,",");
		fprintf(_fpout,"\n{ %d, 0x%02x, \t%2d,\t%2d,\"%s\" }",
				(int)_typetab[i].ot_typ,
				(int)_typetab[i].ot_flags,
				_typetab[i].ot_idx,
				_typetab[i].ot_nelem,
				_typetab[i].ot_id);
	}
	fprintf(_fpout,"\n};\nint _ntypes = %d;\n",_currtype);

	fprintf(_fpout,"/* Enums: */\n");
	fprintf(_fpout,"TYPEELEM	_enelemtab[] = {");
	for (i = 0; i < _currenelem; i++) {
		if (i) {
			fprintf(_fpout,",");
		}
#if defined(ATARI_ST)
		fprintf(_fpout,"\n{ \"%s\",\t%ld }",
				_localenelemtab[i].te_name,
				_localenelemtab[i].te_val);
#else
		fprintf(_fpout,"\n{ %d,\t%ld }",
				i + IDM_LOWENUMNAME,
				_localenelemtab[i].te_val);
#endif
	}
	fprintf(_fpout,"\n};\nint _nenelems = %d;\n",_currenelem);
}

/*---------------------------------*/
/* pr_flags()					*/
/*---------------------------------*/
static void pr_flags(int flags)
{	unsigned int i;

	for (i = 1; i < 0x8000; i <<= 1) {
		if (flags & i) {
			switch(i) {
				case EW_MODIFY: fprintf(_fpout,"EW_MODIFY | "); break;      
				case EW_NEEDSCURRF: fprintf(_fpout,"EW_NEEDSCURRF | "); break;    
				case EW_UNDOFLSH: fprintf(_fpout,"EW_UNDOFLSH | "); break;      
				case EW_NEEDSBLK: fprintf(_fpout,"EW_NEEDSBLK | "); break;
				case EW_HASFORM: fprintf(_fpout,"EW_HASFORM | "); break;
				case EW_NOCASH: fprintf(_fpout,"EW_NOCASH | "); break;
				case EW_CCASH: fprintf(_fpout,"EW_CCASH | "); break;
				case EW_MULTI: fprintf(_fpout,"EW_MULTI | "); break;
			}
		}
	}
	fprintf(_fpout,"0");
}

/*---------------------------------*/
/* pr_functab()				*/
/*---------------------------------*/
static void pr_functab(void )
{	int i;

	fprintf(_fpout,"EDFUNC _edfunctab[] = {\n");
	for (i = 0; i < _currfunc; i++) {
		if (i) fprintf(_fpout,",\n");
		fprintf(_fpout,"\t%-14s, '!',\t",_allfuncs[i].name);
		pr_flags(_allfuncs[i].flags);
	}
	fprintf(_fpout,"};\n\n");
	fprintf(_fpout,"int _nfuncs = %d;\n",_currfunc);
}

/*---------------------------------*/
/* findfunc()					*/
/*---------------------------------*/
static int findfunc(char *s)
{	int i;

	for (i = 0; i < _currfunc; i++) {
		if (strcmp(_allfuncs[i].name,s) == 0)
			return i;
	}
	yyerror("Function %s not declared",s);
	return -1;
}

/*--------------------------------------------------------------------------
 * findcmdseq()
 */
static int findcmdseq(char *cmdname)
{
	struct cmd 	*cp;
	int			i;

	for (i = 0; i < _currcmd; i++) {
		cp = _allcmd[i];
		if (strcmp(cp->cmdname,cmdname) == 0)
			return i;
	}
	yyerror("Cmdseq %s not declared",cmdname);
	return -1;
}

/*---------------------------------*/
/* pr_menu()					*/
/*---------------------------------*/
static void pr_menu(void )
{	int i,findex;
	struct cmd *cp;

	fprintf(_fpout,"MENUBIND _menutab[] = {\n");
	for (i = 0; i < _currmenu; i++) {
		findex = findcmdseq(_allmenus[i].cmdname);
		if (findex < 0) return;
		if (i)
			fprintf(_fpout,",\n");
#if defined(MACROS20)
		fprintf(_fpout,"%s,\t%s,\tCMD_CMDSEQ\t,%d\t/* %-2d */",
			   _allmenus[i].menu,_allmenus[i].title,
			   findex,i);
#else
#if defined(ATARI_ST)
		fprintf(_fpout,"%s\t,%s\t,%-2d",
			   _allmenus[i].menu,_allmenus[i].title,findex);
#else
		fprintf(_fpout,"%s\t,%-2d",
			   _allmenus[i].menu,findex);
#endif
#endif
	}
	fprintf(_fpout,"\n};\n\n");
	fprintf(_fpout,"int _nmenus = %d;\n",_currmenu);
}

/*---------------------------------*/
/* pr_icons()					*/
/*---------------------------------*/
static void pr_icons(void )
{	int i;

	fprintf(_fpout,"ICONBIND _ictab[] = {\n");
	for (i = 0; i < _curric; i++) {
		if (i)
			fprintf(_fpout,",\n");
#if defined(MACROS20)
		fprintf(_fpout,"%s,\t%s,\t%s,\t{ CMD_CMDSEQ, %-2d}",
			   _allicons[i].idname,_allicons[i].dropname,
			   _allicons[i].flagname,findcmdseq(_allicons[i].cmdname));
#else
		fprintf(_fpout,"%s,\t%s,\t%s, %-2d",
			   _allicons[i].idname,_allicons[i].dropname,
			   _allicons[i].flagname,findcmdseq(_allicons[i].cmdname));
#endif
	}
	fprintf(_fpout,"\n};\n\n");
	fprintf(_fpout,"int _nicbind = %d;\n",_curric);
}

/*---------------------------------*/
/* pr_mice()					*/
/*---------------------------------*/
static void pr_mice(void )
{	int i;

	fprintf(_fpout,"MOUSEBIND _mousetab[MAXMAPMOUSE] = {\n");
	for (i = 0; i < _currmouse; i++) {
		if (i)
			fprintf(_fpout,",\n");
#if 1
		fprintf(_fpout,"{0x%02x,0x%02x,0x%02x,%s,{CMD_CMDSEQ,%2d}, (char *)0}",
			   _allmice[i].but,_allmice[i].shift,_allmice[i].click,
			   _allmice[i].flagname,
			   findcmdseq(_allmice[i].cmdname));
# else
		fprintf(_fpout,"%d,\t%d,\t%d,\t%s,\t%-2d,0,(char *)0,(char *)0",
			   _allmice[i].but,_allmice[i].shift,
			   _allmice[i].click,_allmice[i].flagname,
			   findcmdseq(_allmice[i].cmdname));
# endif
	}
	fprintf(_fpout,"\n};\n\n");
	fprintf(_fpout,"int _nmousebind = %d;\n",_currmouse);
}

/*--------------------------------------------------------------------------
 * isc0_func()
 */
static int isc0_func(int idx)
{
	struct llist *lp = _allfuncs[idx].plist;

	if (!lp || (lp = lp->next) == 0)
		return 1;
	return(strcmp(lp->name,"DIALOGOPT") == 0);
}

/*---------------------------------*/
/* pr_comseq()					*/
/*---------------------------------*/
static void pr_comseq(void )
{	int i,max,n,flg,findex,p1;
	struct cmd * cp;
	int offstab[512];

	fprintf(_fpout,"COM_1FUNC _cmdseqtab[] = {\n");
	for (i = 0, n = 0, flg = 0, max = 0; i < _currcmd; i++) {
		cp = _allcmd[i];
		offstab[n++] = max;
		while (cp) {
			if (flg++) fprintf(_fpout,",\n");
			findex = findfunc(cp->name);
			if (isc0_func(findex)) {
				if (cp->par)
					fprintf(stdout,"useless par to function %s\n",cp->name);
				fprintf(_fpout,"/* %d */ C_0FUNC",flg-1);
			} else {
				if (!cp->par)
					fprintf(stdout,"function %s should have par\n",cp->name);
				fprintf(_fpout,"/* %d */ C_1FUNC",flg-1);
			}
			fprintf(_fpout,", %-2d /* %-10s */, ",findex,cp->name);
			fprintf(_fpout,"%-10s",(cp->par) ? cp->par : "0");
			max += sizeof(COM_1FUNC);
			cp = cp->next;
			if (cp) {
				fprintf(stderr,"warning: multiple commands not yet supported\n");
			}
		}
	}
	offstab[n] = max;
	fprintf(_fpout,"\n};\n\n");
	fprintf(_fpout,"char _recorder[RECORDERSPACE];\n");
	fprintf(_fpout,"int _lcomseq = %d;\n",max);

#ifdef OFFSETTAB
	fprintf(_fpout,"int _cmdoffstab[] = {\n");
	for (i = 0; i < n + 2;		/* leave space for recorder */
	     i++) {
		if (i) {
			fprintf(_fpout,",");
			if ((i & 0x7) == 0)
				fprintf(_fpout,"\n");
		}
		fprintf(_fpout,"%-2d",offstab[i]);
	}
	fprintf(_fpout,"\n};\n\n");
#endif
}

/*---------------------------------*/
/* pr_disable()				*/
/*---------------------------------*/
static void pr_disable(char *which, int needed)
{	int j,i,flg=0,findex;
	struct cmd *cp;
	
	fprintf(_fpout,"unsigned char _%sdisable[] = {\n\t",which);
	for (i = 0; i < _currmenu; i++) {
		if ((j = findcmdseq(_allmenus[i].cmdname)) < 0)
			continue;
		cp = _allcmd[j];
		if ((findex = findfunc(cp->name)) >= 0 &&
		    (_allfuncs[findex].flags & needed)) {
	    		if (flg)
				fprintf(_fpout,((flg%5) == 4) ? ",\n\t" : ", ");
			flg++;
			fprintf(_fpout,"%-10s",_allmenus[i].menu);
		}
	}
	fprintf(_fpout,", 0 /* EOT */\n};\n");
}

/*---------------------------------*/
/* pr_keys()					*/
/*---------------------------------*/
static void pr_keys(void )
{	int i;
	char *ctyp,*comment;
	int	index;

	fprintf(_fpout,"KEYBIND _keymaptab[MAXMAPKEY] = {\n");
	for (i = 0; i < DIM(_keymaptab); i++) {
		if (i) fprintf(_fpout,",\n");
		if (_keymaptab[i].keycode == 0) {
			ctyp = "0";
			comment = "unused";
		} else {
			index = _keymaptab[i].M_index;
			ctyp = "CMD_CMDSEQ";
			comment = "comm-seq";
			if (_keymaptab[i].M_typ == CMD_MENU) {
				index = findcmdseq(_allmenus[index].cmdname);
			}
		}
		fprintf(_fpout,"0x%-4x, %-8s, %d /* %s */",
			_keymaptab[i].keycode,
			ctyp,
			index,
			comment);
	}
	fprintf(_fpout,"};\n\n");
}

/*---------------------------------*/
/* pr_khelp()					*/
/*---------------------------------*/
static void pr_khelp(void )
{	int i,k,idx;
	char *ctyp,*comment,*param;
	struct funcdef *fp;

	fprintf(_fpout,"Tasten - Belegung PKS-EDIT\n");
	for (i = 0; i < DIM(_keymaptab); i++) {
		if (_keymaptab[i].keycode != 0) {
			k = -1;
			idx = _keymaptab[i].M_index;
			switch(_keymaptab[i].M_typ) {
				case CMD_CMDSEQ: 
					k     = findfunc(_allcmd[idx]->name);
					param = _allcmd[idx]->par;
					break;
			}
			if (k >= 0) {
				fp = &_allfuncs[k];
				if (param == 0)
					param = "";
				fprintf(_fpout,"%-15s %s %s\n",code2key(_keymaptab[i].keycode),
					   fp->comment,param);
			}
		}
	}
	fprintf(_fpout,"\n");
}

/*---------------------------------*/
/* prcom2keys()				*/
/*---------------------------------*/
static void pr_com2keys(void )
{
	int 			i;
	int			findex;
	int			printed;
	char *		ctyp;
	char *		comment;
	KEYBIND	*	kp;

#if 0
	fprintf(_fpout,"KEYBIND _cmdorig[] = {\n");
	for (i = 0, printed = 0; i < DIM(_keymaptab); i++) {
		if (_keymaptab[i].keycode == 0) {
			continue;
		} else {
			switch(_keymaptab[i].M_typ) {
			case CMD_MENU	:  
				ctyp    = "CMD_MENU";
				comment = _allmenus[_keymaptab[i].M_index].menu;
				break;
			case CMD_CMDSEQ: 
				ctyp    = "CMD_CMDSEQ"; 
				comment = "comm-seq";
				break;
			default		:  continue;
			}
		}
		if (printed) {
			fprintf(_fpout,",\n");
		}
		fprintf(_fpout,"0x%-4x, %-8s, %d /* %s */",
			_keymaptab[i].keycode,
			ctyp,
			_keymaptab[i].M_index,
			comment);
		printed++;
	}
	fprintf(_fpout,"};\n");
#endif
	fprintf(_fpout,"int _ncmdseq = %d;\n\n",_currcmd);
}

/*---------------------------------*/
/* printfuncdefines(X)			*/
/*---------------------------------*/
int printfuncdefines(char *name)
{	int i,j;
	char *s;
	FILE *fp = fopen(name,"w");
	
	if (fp == 0)
		return -1;

	fprintf(fp,"\n");
	fprintf(fp,"#ifndef 	_FUNCDEFS_H\n");
	fprintf(fp,"\n");
	for (i = 0; i < _nfdfuncs; i++) {
		j = _fdfunctab[i];
		s = _allfuncs[j].name;
		fprintf(fp,"#define 	FUNC_%s			%d\n",s,j);
	}
	fprintf(fp,"\n");
	fprintf(fp,"#define	_FUNCDEFS_H\n");
	fprintf(fp,"#endif\n");

	fclose(fp);
	return 0;
}

/*---------------------------------*/
/* cmd_equal()					*/
/*---------------------------------*/
static int cmd_equal(struct cmd *c1, struct cmd *c2)
{
	while(c1 && c2) {
		if ((c1->cmdname && c2->cmdname) &&
		    (c1->name && c2->name && strcmp(c1->name,c2->name) == 0) &&
		    (c1->par && c2->par && strcmp(c1->par,c2->par) == 0))
		    	return 1;
		c1 = c1->next;
		c2 = c2->next;
	}
	return 0;
}

/*---------------------------------*/
/* check_double_cmds()			*/
/*---------------------------------*/
void check_double_cmds(void)
{	int i,j;

	for (i = 0; i < _currcmd; i++) {
		for (j = 0; j < _currcmd; j++) {
			if (i != j && _allcmd[i] && _allcmd[j] &&
			    cmd_equal(_allcmd[i],_allcmd[j])) {
				fprintf(stderr,"%s(%d) - eq - %s(%d)\n",
						_allcmd[i]->cmdname,i,
						_allcmd[j]->cmdname,j);
			}
		}
	}
}

/*---------------------------------*/
/* printfuncs(X)				*/
/*---------------------------------*/
int printfuncs(char *fname)
{

	if (!_excode) {
		_fpout = fopen(fname,"w");
		setvbuf(_fpout,(char *) 0, _IOFBF, 8192);
		fprintf(_fpout,"#include <windows.h>\n");
		fprintf(_fpout,"#include \"edfuncs.h\"\n");
		fprintf(_fpout,"#include \"pksedit.h\"\n");
		fprintf(_fpout,"#include \"pksedith.h\"\n");
		fprintf(_fpout,"#include \"edierror.h\"\n\n");
		fprintf(_fpout,"# include \"iccall.h\"\n\n");
#if 0
		sortlist(_allfuncs,_currfunc);
#endif
		pr_extern();
		pr_functab();
		pr_menu();
		pr_icons();
		pr_mice();
		pr_comseq();
		pr_com2keys();
		pr_keys();		/* pr_menu ... should be called before */

# if defined(ATARI_ST)
		pr_disable("cf",EW_NEEDSCURRF);
		pr_disable("rdo",EW_MODIFY);
		pr_disable("blk",EW_NEEDSBLK);
# endif

		fclose(_fpout);

	}
	return _excode;
}

/*--------------------------------------------------------------------------
 * hexa2string()
 */
static char *hexa2string(char *hexa,char **adv)
{
	if (*hexa == PAR_VOID)
		return 0;
	*adv = hexa+1;
	switch(*hexa) {
		case PAR_INT: return "int";
		case PAR_STRING: return "string";
		default: return _typetab[*hexa-PAR_USER].ot_id;
	}
}

/*--------------------------------------------------------------------------
 * split_param()
 */
static void split_param(char *full, char *typ, char *name, char *comment, int pos)
{
	char buf[512],*s;

	strcpy(buf,full);
	if ((s = strtok(buf,":")) != 0) {
		strcpy(typ,s);
	} else {
		*typ = 0;
	}
	if ((s = strtok((char*)0,":")) != 0) {
		strcpy(name,s);
	} else {
		sprintf(name,"param%d",pos);
	}
	if ((s = strtok((char*)0,":")) != 0) {
		strcpy(comment,s);
	} else {
		*comment = 0;
	}
}

/*--------------------------------------------------------------------------
 * param_string()
 */
static char *param_string(char *buf,struct llist *lp)
{
	char *d,name[32],typ[32],comment[128];
	int  n = 0;

	if (!lp)
		return "";
	lp = lp->next;
	d = buf;
	while(lp) {
		n++;
		split_param(lp->name,typ,name,comment,n);
np:		if (d != buf)
			*d++ = ',';
		strcpy(d,name);
		while(*d)
			d++;
		lp = lp->next;
	}
	*d = 0;
	return buf;
}

/*--------------------------------------------------------------------------
 * find_in_typetab()
 */
int find_in_typetab(char *typeName)
{
	int i;

	for (i = 0; i < _currtype; i++) {
		if (strcmp(_typetab[i].ot_id, typeName) == 0) {
			return i;
		}
	}
	return -1;
}

/*--------------------------------------------------------------------------
 * param_description()
 */
static void param_description(FILE *fp, struct llist *lp)
{
	char name[64], typ[64], comment[128];
	int  n = 0;

	if (!lp)
		return;
	lp = lp->next;
	while(lp) {
		n++;
		split_param(lp->name,typ,name,comment,n);
		if (find_in_typetab(typ) >= 0) {
			fprintf(fp,"%s<macroc_type_%s>\t%s;\t# %s\n", 
				typ, typ, name, comment);
		} else {
			fprintf(fp,"%s\t%s;\t# %s\n", typ, name, comment);
		}
		lp = lp->next;
	}
}

/*---------------------------------*/
/* funccomp()					*/
/*---------------------------------*/
static int funccomp(f1,f2)
struct funcdef *f1;
struct funcdef *f2;
{
	return strcmp(f1->ident,f2->ident);
}

/*--------------------------------------------------------------------------
 * output_typedescription_for()
 */
void output_typedescription_for(FILE *fpout, char *type)
{
	extern	FILE	*yyfp;
	char 	szBuf[512];
	int		bOutput;

	rewind(yyfp);
	bOutput = 0;
	while(fgets(szBuf, sizeof szBuf, yyfp)) {
		if (szBuf[0] == '}' && bOutput) {
			return;
		}
		if (szBuf[0] == '#') {
			if(bOutput) {
				return;
			}
			if (strstr(szBuf, type)) {
				bOutput = 1;
			}
		}
		if (bOutput && strchr(szBuf, '{') == 0) {
			fputs(szBuf, fpout);
		}
	}
}

/*--------------------------------------------------------------------------
 * print_funcdescription()
 */
void print_funcdescription(void)
{
	int		i,k,n;
	char		buf[256],*dummy,*s,comment[512];
	struct cmd *cp;

	qsort(_allfuncs,_currfunc,sizeof *_allfuncs,funccomp);

	for (i = 0; i < _currfunc; i++) {
// fprintf(stderr,"Funktion %s\n",_allfuncs[i].ident);
		fprintf(_fpout,"***\n");
		fprintf(_fpout,"Funktion:\t%s\n",_allfuncs[i].ident);
		fprintf(_fpout,"Beschreibung:\t%s\n",_allfuncs[i].comment);
		fprintf(_fpout,"Syntax:\tbool %s(%s)\n",
			   _allfuncs[i].ident,
			   param_string(buf,_allfuncs[i].plist));
		param_description(_fpout,_allfuncs[i].plist);
	}

	for (i = 0; i < _currtype; i++) {
		fprintf(_fpout,"***\n");
		fprintf(_fpout,"Type:\t%s\n", _typetab[i].ot_id);
		output_typedescription_for(_fpout, _typetab[i].ot_id);
	}

#if 1	
	fprintf(_fpout,"--------------- Interne Makros -------------------\n");

	for (i = 0; i < _currcmd; i++) {
		cp = _allcmd[i];
		if (cp && cp->cmdname) {
			strcpy(comment,_comments[i] ? _comments[i] : "-");
			if ((s = strchr(comment,';')) != 0)
				*s = 0;
			fprintf(_fpout,"@%-20s %s\n",
				   cp->cmdname,comment);
	   }
	}
# endif
}

/*---------------------------------*/
/* printhelp(X)				*/
/*---------------------------------*/
int printhelp(char *fname)
{
	_fpout = fopen(fname,"w");
	print_funcdescription();
	fclose(_fpout);
	return _excode;
}

/*---------------------------------*/
/* printcmdtable(X)				*/
/*---------------------------------*/
int printcmdtable(char *fname)
{
	if (!_excode) {
		_fpout = fopen(fname,"w");
	     setvbuf(_fpout,(char *) 0, _IOFBF, 8192);
		qsort(_allfuncs,_currfunc,sizeof *_allfuncs,funccomp);
		pr_cmdtab();		
		fclose(_fpout);
	}
	return _excode;
}

/*---------------------------------*/
/* printfunctable(X)			*/
/*---------------------------------*/
int printfunctable(char *fname)
{
	if (!_excode) {
		_fpout = fopen(fname,"w");
	     setvbuf(_fpout,(char *) 0, _IOFBF, 8192);
		pr_functions();		
		fclose(_fpout);
	}
	return _excode;
}

/*---------------------------------*/
/* pr_help()					*/
/*---------------------------------*/
static void pr_help(int komma)
{
	struct cmd * cp;
	int		   i;
	char		   k = (komma) ? ',' : ' ';

	for (i = 0; i < _currcmd; i++) {
		cp = _allcmd[i];
		if (cp && cp->cmdname)
			fprintf(_fpout,"\t%-4d,\t\"%s\"%c\n",
						i+IDM_CMDNAME,cp->cmdname,k);
	}

	for (i = 0; i < DIM(_comments); i++) {
		if (_comments[i])
			fprintf(_fpout,"\t%-4d,\t\"%s\"%c\n",
			   	   i+IDM_CMDCOMMENT,_comments[i],k);
	}

	for (i = 0; i < _currfunc; i++) {
		fprintf(_fpout,"\t%-4d,\t\"%s\"%c\n",
		   	   i+IDM_LOWFUNCNAME,_allfuncs[i].ident,k);
	}

	for (i = 0; i < _currenelem; i++) {
		fprintf(_fpout,"\t%-4d,\t\"%s\"%c\n",
		   	   i+IDM_LOWENUMNAME,_localenelemtab[i].te_name,k);
     }
}

/*---------------------------------*/
/* printhelpstrings()			*/
/*---------------------------------*/
int printhelpstrings(char *fname,int printkomma)
{
	if (!_excode) {
		_fpout = fopen(fname,"w");
	     setvbuf(_fpout,(char *) 0, _IOFBF, 8192);
		pr_help(printkomma);
	     fclose(_fpout);
	}
	return _excode;
}

