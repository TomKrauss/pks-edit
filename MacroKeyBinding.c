/*
 * MacroKeyBinding.c
 *
 * editor command handling and key mapping
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krau�					Author : TOM
 * All Rights Reserved.					created: 11/89
 *									
 */

# include	<tos.h>
# include	<windowsx.h>
# include	<string.h>
# include 	"trace.h"
# include	"editab.h"
# include	"edierror.h"
# include	"pksedit.h"
# include 	"winfo.h"
# include 	"winterf.h"
# include 	"edifsel.h"
# include	"edfuncs.h"
# include	"functab.h"
# include	"dial2.h"
# include	"iccall.h"
# include	"resource.h"
# include 	"xdialog.h"

# define	MENU_TABCHAR	'\010'

extern long		_multiplier;

extern RSCTABLE *	_keytables;
extern RSCTABLE *	_mousetables;
extern RSCTABLE *	_menutables;

extern BOOL 		OptionSet(long nFlag);
extern LPSTR 		lstrchr(char *str, char ch);
extern long		rsc_wrmacros(int fd, long offset, char *buf, long maxbytes);
extern long 		rsc_wrbuf(int fd, long offset, char *buf, long maxbytes);
extern char *		rsc_rdmacros(char *param, unsigned char *p, unsigned char *pend);
extern char *		TmpName(char *dst, char c);
extern char * 		mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);
extern void 		st_seterrmsg(char *msg);
extern int 		cust_combood(LPDRAWITEMSTRUCT lpdis, void (*DrawEntireItem)(), 
					void (*ShowSelection)(LPDRAWITEMSTRUCT lp));
extern void 		key_overridetable(void);
extern void 		mouse_overridetable(void);
extern int 		bind_mouse(MOUSECODE mousecode, MACROREFTYPE typ, 
					MACROREFIDX idx, int augment);
extern int 		menu_addentry(char *pszString, int menutype, 
					MACROREFTYPE mactype, MACROREFTYPE macidx);
extern void 		menu_startdefine(char *szMenu);
extern void 		st_switchtomenumode(BOOL bMenuMode);

extern long 		Atol(char *);
extern unsigned char *stralloc(unsigned char *buf);
extern MACROREF *	menu_getuserdef(int nId);
extern int 		CanExecute(int num, int warn);
extern void 		SetMenuFor(char *pszContext);

int				_recording;
int				_nmacros = MAXMACRO;
char *			_macroname;
char *			_cmdfuncp;
char *			_cmdmaxp;
char *			_cmdparamp;
unsigned char *	_readparamp;
MACRO *			_macrotab[MAXMACRO];
int				_lastinsertedmac = -1;
int				_macedited;
int				_fkeysdirty;
MACROREF			currentSelectedMacro;

static FSELINFO 	_seqfsel = {	"","PKSEDIT.MAC", "*.MAC" };
static int 		_nkeys = MAXMAPKEY;

EDBINDS _bindings = {
	_edfunctab,_cmdseqtab,_macrotab,_keymaptab,_menutab,_ictab,_mousetab,
	&_nfuncs,&_ncmdseq,&_nmacros,&_nkeys,&_nmenus,&_nicbind,&_nmousebind
};

/*------------------------------------------------------------
 * mac_byindex()
 */
MACRO *mac_byindex(int idx)
{
	return _macrotab[idx];
}

/*---------------------------------*/
/* mac_getbyname()				*/
/*---------------------------------*/
int mac_getbyname(char *name)
{	int i;
	MACRO *mp;

	if (name)
	  for (i = 0; i < MAXMACRO; i++) {
		if ((mp = mac_byindex(i)) != 0 && 
			strcmp(mp->name,name) == 0)
				return i;
	  }
	return -1;
}

/*------------------------------------------------------------
 * mac_write()
 */
# define	RSC_MACROS		0x1
# define	RSC_SINGLEMACRO	0x2
# define	RSC_KEYS			0x4
# define	RSC_MICE			0x8
# define	RSC_MENUS			0x10
static int mac_write(int whichresource, char *name)
{
	int 		ret = 1,fd,i;

	if ((fd = rsc_create(name,1)) < 0) {
		tosfnerror(name,fd);
		return 0;
	}

	if ((whichresource & RSC_SINGLEMACRO) == 0)
		_macroname = 0;

	if (whichresource & (RSC_MACROS|RSC_SINGLEMACRO)) {
		if (rsc_put(fd,"MACROS","",0,rsc_wrmacros,_linebuf,(long)FBUFSIZE) == 0) {
err:			ed_error(IDS_MSGNODISKSPACE);
			ret = 0;
			goto done;
		}
	}

	if (whichresource & RSC_KEYS) {
		if (rsc_wrtables(fd,"KEYS",_keytables) == 0) {
			goto err;
		}
	}

	if (whichresource & RSC_MICE) {
		if (rsc_wrtables(fd,"MOUSE",_mousetables) == 0) {
			goto err;
		}
	}

	if (whichresource & RSC_MENUS) {
		if (rsc_wrtables(fd,"MENU",_menutables) == 0) {
			goto err;
		}
	}

done:Fclose(fd);
	_macroname = 0;
	return ret;
}

/*------------------------------------------------------------
 * mac_wantclose()
 */
void mac_wantclose(int warnflg)
{
	char fn[512];

	if (_macedited) {
		if (warnflg == 0) {
			strdcpy(fn,_seqfsel.path,_seqfsel.fname);
			if (mac_write(RSC_MACROS|RSC_KEYS|RSC_MICE|RSC_MENUS,fn))
				_macedited = 0;
		} else {
			if (ed_yn(IDS_MSGSAVESEQ) == IDYES) {
				EdMacrosReadWrite(1);
			}
		}
	}
	_macedited = 0;
}

/*------------------------------------------------------------
 * mac_delete()
 */
int mac_delete(char *name)
{	int idx;

	if ((idx = mac_getbyname(name)) >= 0) {
		key_delbindings(CMD_MACRO,idx);
		destroy(&_macrotab[idx]);
		_macedited = 1;
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------
 * mac_validname()
 */
int mac_validname(char *name, int origidx)
{	int index;

	if (name[0] == 0)
		return 0;
	if (((index = mac_getbyname(name)) >= 0 && index != origidx)) {
		if (origidx < 0)
			ed_error(IDS_MSGMACDOUBLEDEF);
		else
			mac_delete(name);
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * check_fkdirty()
 */
void check_fkdirty(KEYCODE key)
{
	key &= 0xFF;
	if (key >= VK_F1 && key <= VK_F12) {
		_fkeysdirty = 1;
	}
}

/*------------------------------------------------------------
 * mac_scaninsert()
 * index < 0:	delete key-binding
 * index >=0:  add key-binding
 */
int mac_scaninsert(KEYCODE key, int index, MACROREFTYPE typ)
{	KEYBIND *kp;

	if (key == K_DELETED)
		return 1;

	if ((kp = keybound(key)) == 0) {
		if (index < 0)
			return 0;
		if ((kp = keygetbind()) == 0) {
			return 0;
		}
		kp->keycode = key;
		kp->macref.typ = CMD_NONE;
	}

	if (index < 0) {
		key_unbind(kp);
	} else {
		check_fkdirty(key);
		kp->keycode = key;
		kp->macref.index = index;
		kp->macref.typ = typ;
	}
	return 1;
}

/*------------------------------------------------------------
 * mac_addkeys()
 */
static char *mac_addkeys(char *name, KEYBIND *kp, KEYBIND *kplast)
{
	if (!rsc_switchtotable(&_keytables,name)) {
		return 0;
	}

	key_overridetable();

	while (kp < kplast) {
		mac_scaninsert(kp->keycode,kp->macref.index,kp->macref.typ);
		kp++;
	}
	_fkeysdirty = 1;
	return (char *)kplast;
}

/*------------------------------------------------------------
 * mac_addmice()
 */
static char *mac_addmice(char *name, MOUSEBIND *mp, MOUSEBIND *mplast)
{
	if (!rsc_switchtotable(&_mousetables,name)) {
		return 0;
	}

	mouse_overridetable();

	while (mp < mplast) {
		MOUSECODE mcode;
		
		mcode.button = mp->button;
		mcode.nclicks = mp->nclicks;
		mcode.shift = mp->shift;
		bind_mouse(mcode, mp->macref.typ, mp->macref.index, 0);
		mp++;
	}
	return (char *)mplast;
}

/*------------------------------------------------------------
 * mac_menus()
 */
static char *mac_menus(char *name, PUSERMENUBIND mp, 
	PUSERMENUBIND mplast)
{
	menu_startdefine(name);

	while (mp < mplast) {
		if (*mp->szString) {
			if (!menu_addentry(mp->szString, mp->type, 
				mp->macref.typ, mp->macref.index)) {
				return 0;
			}
		}
		mp++;
	}
	return (char *)mplast;
}

/*--------------------------------------------------------------------------
 * mac_switchtodefaulttables()
 */
void mac_switchtodefaulttables(void)
{
	char	*pszDefault = "default";
	char *pszMode;

	if (_currfile) {
		pszMode = _currfile->lin->modename;
	} else {
		pszMode = pszDefault;
	}

	rsc_switchtotable(&_keytables,
		rsc_findtable(_keytables, pszMode) ? pszMode : pszDefault);

	rsc_switchtotable(&_mousetables,
		rsc_findtable(_mousetables, pszMode) ? pszMode : pszDefault);

	SetMenuFor(pszMode);
}

/*------------------------------------------------------------
 * mac_read()
 */
int mac_read(char *fn)
{
	int 	 wasdirty = _macedited;
	int	 	overwritebindings = 0;
	RSCFILE	*rp;

	if (fn == 0) {
		if ((fn = rw_init(&_seqfsel)) == 0)
			return 0;
	}

	if ((rp = rsc_open(fn,RSC_O_READ)) == 0) {
		return 0;
	}

	mac_wantclose(1);

	if (overwritebindings) {
		int i;

		for (i = 0; i < MAXMACRO; i++) {
			destroy(&_macrotab[i]);
		}
	}

	/* load macros */
	if (!rsc_load(rp, "MACROS", "", rsc_rdmacros)) {
		ed_error(IDS_MSGBADMACFORMAT);
		rsc_close(rp);
		return 0;
	}

	/* load key bindings */
	rsc_load(rp,"KEYS",(char*)0,mac_addkeys);

	/* load mouse bindings */
	rsc_load(rp,"MOUSE",(char*)0,mac_addmice);

	/* load menu bindings */
	rsc_load(rp,"MENU",(char*)0,mac_menus);

	rsc_close(rp);

	mac_switchtodefaulttables();

	_macedited = wasdirty;
	return 1;
}

/*------------------------------------------------------------
 * mac_new()
 */
MACRO *mac_new(char* szName, char* szComment, char* bData, int size)
{
	int   nLen,nNamelen,nCommentlen;
	MACRO *mp;
	
	if (!szComment)
		szComment = "";

	nNamelen = lstrlen(szName) + 1;
	nCommentlen = lstrlen(szComment) + 1;

	nLen = nNamelen+nCommentlen;
	if (nLen & 1) {
		nLen++;
	}

	if ((mp = _alloc(MAC_SIZE(nLen+size))) == 0)
		return (MACRO*)0;

	mp->dstart = nLen+sizeof(*mp);

	mp->namelen = nNamelen;
	mp->size = size;

	lstrcpy(mp->name,szName);
	lstrcpy(MAC_COMMENT(mp),szComment);
	memmove(MAC_DATA(mp),bData,size);
	return mp;
}

/*------------------------------------------------------------
 * mac_nameupdate()
 */
void mac_nameupdate(int nIndex, LPSTR szName, LPSTR szComment)
{
	MACRO *mp,*mpold;

	mpold = _macrotab[nIndex];
	if ((mp = mac_new(szName,szComment,MAC_DATA(mpold),mpold->size)) == 0)
		return;
	_macrotab[nIndex] = mp;
	_free(mpold);
}

/*------------------------------------------------------------
 * MacFormatErr()
 */
void MacFormatErr(void)
{
	ed_error(IDS_MSGEXECERR);
}

/*------------------------------------------------------------
 * mac_insert()
 */
int mac_insert(char *name, char *comment, char *macdata, int size)
{	MACRO  *mp;
	int    i;

	if ((i = mac_getbyname(name)) >= 0) {
		destroy(&_macrotab[i]);
	} else {
		for (i = 0; ; i++) {
			if (i >= DIM(_macrotab)) {
				ed_error(IDS_MSGTOOMANYMACROS);
				return 0;
			}
			if (_macrotab[i] == 0) {
				break;
			}
		}
	}

	if ((mp = mac_new(name,comment,macdata,size)) == 0)
		return -1;

	_macrotab[i] = mp;
	_macedited = 1;
	return i;
}

/*------------------------------------------------------------
 * mac_recorder()
 */
static int _recsiz,_lastrecsiz=-1;
int mac_recorder(void)
{	int     size;
	static KEYCODE scan = K_DELETED;
	char    buf[100];
	static  int _ndef;

	if (!_recording) {		/* STOP */
		if (_cmdfuncp && (size = _cmdparamp-_recorder) > 0) {
			wsprintf(buf,"Mac%d",++_ndef);
			if (!mac_getname(&scan,buf,-1)) {
				return 0;
			}
			if (!scan) {
				scan = K_DELETED;
			}
			if ((_lastinsertedmac = mac_insert(buf,"",_recorder,size)) >= 0) {
				mac_scaninsert(scan,_lastinsertedmac,CMD_MACRO);
			}
		}
	} else {						/* START */
		_cmdparamp =  _recorder;	/* maybe &_recorder[recno]	*/
		_cmdmaxp	 =  &_recorder[RECORDERSPACE-12];
	}
	return 1;
}

/*------------------------------------------------------------
 * EdMacrosReadWrite()
 */
int EdMacrosReadWrite(int wrflag)
{	char *fn;

	if ((fn = rw_select(&_seqfsel,(wrflag) ? MWRSEQ : MREADSEQ)) == 0) {
		return 0;
	}

	if (wrflag) {
		if (mac_write(RSC_MACROS|RSC_KEYS|RSC_MICE|RSC_MENUS,fn)) {
			_macedited = 0;
			return 1;
		}
		return 0;
	}

	return mac_read(fn);
}

/*------------------------------------------------------------
 * EdMacroRecord()
 */
int EdMacroRecord(void)
{
	return EdOptionToggle(MAKELONG(1,-1));
}

/*---------------------------------*/
/* do_icon()					*/
/*---------------------------------*/
int do_icon(HWND icHwnd, WPARAM wParam,  LPARAM dropped)
{
	HWND		icdropHwnd = 0;
	ICONBIND  *ipbind;
	char 	*icp0;
	char		*ps1 = 0;
	char		*ps2 = 0;
	char		*ps3 = 0;
	char		szB1[128];
	char		szBtmp[64];
	char		szB2[128];
	int    	dropped_type,dest_type;
	int		i,funcnum,o1,o2;

	if (wParam == ICACT_DROPPED) {
		icdropHwnd = dropped;
		dropped_type = ic_type(icdropHwnd);
	} else {						/* Textblock,... dropped */
		dropped_type = wParam;
	}

	dest_type = ic_type(icHwnd);

	for (i = 0; i < _nicbind; i++) {
		ipbind = &_ictab[i];
		if (ipbind->id != dest_type) {
			continue;
		}

		if (ipbind->pflags & IPCF_USERDEF) {
			return do_macbyname(ic_param(szB1, icHwnd, 0));
		}

		if (ipbind->dropped == dropped_type) {
			o1 = _cmdseqtab[ipbind->index].p;
			icp0 = ic_param(szB1,icHwnd,0);
			if (ipbind->pflags & IPCF_SRCLONG1)
				o1 = Atol(ic_param(szBtmp,icHwnd,2));
			if (ipbind->pflags & IPCF_SRCLONG1)
				o2 = Atol(ic_param(szBtmp,icHwnd,3));
			if (ipbind->pflags & IPCF_SRCSTRING1)
				ps1 = icp0;
			if (ipbind->pflags & IPCF_SRCSTRING2)
				ps2 = ic_param(szB2,icHwnd,1);
			if (ipbind->pflags & IPCF_SRCCHAR)
				o1 |= *icp0;
			if (icdropHwnd) {
				if (ipbind->pflags & IPCF_DROPCHAR)
					o1 |= *(ic_param(szBtmp,icdropHwnd,0));
				if (ipbind->pflags & IPCF_DROPSTRING1)
					ps1 = ic_param(szB1,icdropHwnd,0);
				if (ipbind->pflags & IPCF_DROPSTRING2)
					ps2 = ic_param(szB2,icdropHwnd,1);
				if (ipbind->pflags & IPCF_DROPHWND)
					o1 = icdropHwnd;
			}
			funcnum = _cmdseqtab[ipbind->index].funcnum;
			return do_func(funcnum, (long)(o1), (long)o2, ps1, ps2, ps3);
		}
	}

	ed_error(IDS_MSGBADICONACTION);
	return 0;
}

/*--------------------------------------------------------------------------
 * GetMacrorefForMenu()
 */
MACROREF *GetMacrorefForMenu(int nId)
{
static MACROREF 	macref;
	MACROREF *	mpUser;
	int			i;

	if (nId >= IDM_USERDEF0) {
		if ((mpUser = menu_getuserdef(nId)) != 0) {
			return mpUser;
		}
	} else {
		for (i = 0; i < _nmenus; i++) {
			if (_menutab[i].menunum == nId) {
				macref.typ = CMD_CMDSEQ;
				macref.index = _menutab[i].index;
				return &macref;
			}
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * TranslateToOrigMenu()
 * try to find an internal command/standard menu binding, which 
 * allows us to display an appropriate help for synthetic menus
 */
WORD TranslateToOrigMenu(WORD wParam)
{
	MACROREF *	mpUser;
	int			i;

	if (wParam < IDM_USERDEF0) {
		return wParam;
	}
	if ((mpUser = menu_getuserdef(wParam)) == 0) {
		return wParam;
	}

	if (mpUser->typ == CMD_CMDSEQ) {
		for (i = 0; i < _nmenus; i++) {
			if (_menutab[i].index == mpUser->index) {
				return _menutab[i].menunum;
			}
		}
	}
	return wParam;
}

/*---------------------------------*/
/* do_menu()					*/
/*---------------------------------*/
int do_menu(int menunum)
{
	MACROREF *	mp;

	if ((mp = GetMacrorefForMenu(menunum)) != 0) {
		return mac_runcmd(mp);
	}
	return 0;
}

/*---------------------------------*/
/* mac_addshift()				*/
/*---------------------------------*/
KEYCODE mac_addshift(KEYCODE key)
{
	if ( GetKeyState(VK_SHIFT) < 0)
		key |= K_SHIFT;
	if ( GetKeyState(VK_CONTROL) < 0)
		key |= K_CONTROL;
	if ( GetKeyState(VK_MENU) < 0)
		key |= K_ALTERNATE;
	return key;
}

/*---------------------------------*/
/* PksGetKeyBind()				*/
/*---------------------------------*/
LONG PksGetKeyBind(WPARAM key)
{
	key = mac_addshift(key);
	return (LONG)keybound(key);
}

/*---------------------------------*/
/* mac_runcmd()				*/
/*---------------------------------*/
int mac_runcmd(MACROREF *mp)
{
	COM_1FUNC   *cp;
	MENUBIND    *menp;

	switch (mp->typ) {
		case CMD_CMDSEQ:
			cp = &_cmdseqtab[mp->index];
			return do_seq(cp,cp+1);
		case CMD_MACRO:
			return EdMacroPlay(mp->index);
		case CMD_MENU:
			menp = &_menutab[mp->index];
			cp = &_cmdseqtab[menp->index];
			do_seq(cp,cp+1);
			break;
		default:
			alert("bad command or macro");
	}
	return 0;
}

/*---------------------------------*/
/* do_key()					*/
/*---------------------------------*/
int do_key(LONG keybind)
{
	KEYBIND     *kp;

	if ((kp = (KEYBIND*)keybind) == (KEYBIND *) 0) 
		return 0;

	return mac_runcmd(&kp->macref);
}

/*------------------------------------------------------------
 * do_insertc()
 */
int do_insertc(WORD c)
{
	return 
		do_func(FUNC_EdCharInsert,c,0,(void*)0,(void*)0,(void*)0);
}

/*---------------------------------*/
/* do_macbyname()				*/
/*---------------------------------*/
int do_macbyname(char *name)
{	int i;

	if (!name || !name[0])
		return 0;

	if ((i = mac_getbyname(name)) < 0) {
		ed_error(IDS_MSGUNKNOWNMAC,name);
		return 0;
	}

	return EdMacroPlay(i);
}

/*------------------------------------------------------------
 * mac_findkey()
 */
static KEYCODE mac_findkey(MACROREFTYPE typ, MACROREFIDX index)
{
	KEYBIND 	*kp;

	for (kp = _keytables->rt_data; kp < _keytables->rt_end; kp++) {
		if (kp->macref.typ == typ && kp->macref.index == index)
			return kp->keycode;
	}
	return K_DELETED;
}

/*------------------------------------------------------------
 * mac_setmenukeys()
 * this function sets the menu accelerator text, each time a
 * menu popup is opened
 */
void mac_setmenukeys(HMENU hMenu)
{
	KEYCODE	k;
	int		wCount;
     int		nFuncnum;
	int		wItem;
	int		wID;
	char		string[100];
	char *	d;
	MACROREF *mp;

	if (_fkeysdirty) {
		fkey_settext(-1);
	}

	wCount = GetMenuItemCount(hMenu);
	for (wItem = 0; wItem < wCount; wItem++) {
		if ((wID = GetMenuItemID(hMenu, wItem)) <= 0) {
			continue;
		}
#if 0
		wState = GetMenuState(hMenu, wItem, MF_BYPOSITION);
		if (HIBYTE(wState) != 0) {
		/* popups */
			continue;
		}
# endif
		if ((mp = GetMacrorefForMenu(wID)) == 0) {
			continue;
		}
		if (GetMenuString(hMenu, wItem, string, sizeof string - 1,
			MF_BYPOSITION) < 0) {
			continue;
		}
		for (d = string; *d; d++) {
			if (*d == 9 || *d == 8) {
				break;
			}
		}
		if (*d) {
			if ((k = mac_findkey(mp->typ, mp->index)) != K_DELETED) {
				strcpy(d+1,code2key(k));
			} else {
				d[1] = 0;
			}
			SetEditMenuText(wID, string);
		}
		if (mp->typ == CMD_CMDSEQ) {
			if (mp->index >= _ncmdseq) {
				alert("bad cmdseq");
				continue;
			}
			nFuncnum = _cmdseqtab[mp->index].funcnum;
			EnableMenuItem(hMenu, wItem, 
				CanExecute(nFuncnum, 0) ?
			    		MF_BYPOSITION|MF_ENABLED : 
					MF_BYPOSITION|MF_DISABLED|MF_GRAYED);
			if (nFuncnum == FUNC_EdOptionToggle) {
				if (OptionSet(_cmdseqtab[mp->index].p)) {
					CheckMenuItem(hMenu, wItem, MF_CHECKED|MF_BYPOSITION);
				}
			}
		}
	}
}

/*------------------------------------------------------------
 * SelectedMacro()
 */
static LONG SelectedMacro(HWND hwnd)
{
	LONG		nData;
	int		item;

	if ((item = SendDlgItemMessage(hwnd, IDD_MACROLIST, LB_GETCURSEL, 0, 0)) < 0)
		return -1;
	SendDlgItemMessage(hwnd, IDD_MACROLIST, LB_GETTEXT, (WPARAM)item, (LONG)&nData);
	return nData;
}

/*------------------------------------------------------------
 * SelectedKey()
 */
static LONG SelectedKey(HWND hwnd)
{
	LONG		nKey;
	int		item;

	if ((item = SendDlgItemMessage(hwnd,IDD_LISTBOX2,LB_GETCURSEL,0,0)) < 0)
		return K_DELETED;
	SendDlgItemMessage(hwnd, IDD_LISTBOX2, LB_GETTEXT, (WPARAM)item,
		(LONG)&nKey);
	return nKey;
}

/*------------------------------------------------------------
 * but_enable()
 */
void but_enable(HWND hwnd, WORD nItem, BOOL nWhich)
{
	HWND 	hwndButton;

	hwndButton = GetDlgItem(hwnd,nItem);
	EnableWindow(hwndButton,nWhich);
}

/*------------------------------------------------------------
 * list_startfilling()
 */
HWND list_startfilling(HWND hwnd, WORD nItem,WORD *nCurr)
{
	HWND hwndList;

	hwndList = GetDlgItem(hwnd,nItem);
	*nCurr = SendMessage(hwndList,LB_GETCURSEL,0,0L);
	if ((int)*nCurr < 0)
		*nCurr = 0;
	SendMessage(hwndList,LB_RESETCONTENT,0,0L);
	SendMessage(hwndList,WM_SETREDRAW,FALSE,0L);
	return hwndList;
}

/*------------------------------------------------------------
 * list_endfilling()
 */
void list_endfilling(HWND hwndList, WORD nCurr)
{
	SendMessage(hwndList, WM_SETREDRAW,TRUE,0L);
	SendMessage(hwndList, LB_SETCURSEL, nCurr, 0L);
	SendRedraw(hwndList);
}

/*--------------------------------------------------------------------------
 * MacroSelectByValue()
 */
static BOOL MacroSelectByValue(HWND hwnd, LONG lValue)
{
	int		i;
	HWND 	hwndList;
	WORD 	nItems;
	LONG		nData;

	hwndList = GetDlgItem(hwnd, IDD_MACROLIST);
	nItems = SendMessage(hwndList, LB_GETCOUNT, 0, 0);

	for (i = 0; i < nItems; i++) {
		SendMessage(hwndList, LB_GETTEXT, (WPARAM)i, (LONG)&nData);
		if (nData == lValue) {
			SendMessage(hwndList, LB_SETCURSEL, i, 0);
			return TRUE;
		}
	}
	return FALSE;
}

/*------------------------------------------------------------
 * UpdateMacroList()
 */
static showInternals = 0;
static void UpdateMacroList(HWND hwnd)
{
	HWND 	hwndList;
	int  	i,state;
	WORD 	nCurr;

	hwndList = list_startfilling(hwnd, IDD_MACROLIST, &nCurr);

	for (i = 0; i < MAXMACRO; i++) {
		if (_macrotab[i] != 0) {
			SendMessage(hwndList,LB_ADDSTRING,0,MAKELONG(CMD_MACRO,i));
		}
	}

	state = showInternals = IsDlgButtonChecked(hwnd,IDD_OPT1);

	if (state) {
		for (i = 0; i < _ncmdseq; i++)
			SendMessage(hwndList,LB_ADDSTRING,0,MAKELONG(CMD_CMDSEQ,i));
	}

	list_endfilling(hwndList,nCurr);
}

/*------------------------------------------------------------
 * mac_comment()
 */
char *mac_comment(LPSTR szBuf, LPSTR szB2, WORD nIndex, WORD type)
{
	char *s;

	szB2[0] = 0;

	switch(type) {
		case CMD_MACRO: 
			if (_macrotab[nIndex] == 0) {
				return "";
			}
			lstrcpy(szBuf,MAC_COMMENT(_macrotab[nIndex]));
			break;
		case CMD_MENU:
			nIndex = _menutab[nIndex].index;
			/* drop through */
		default:
			if (LoadString(hInst,nIndex+IDM_CMDCOMMENT,szBuf,256) <= 0)
				return "";
	}

	if ((s = lstrchr(szBuf,';')) != 0) {
		*s = 0;
		lstrcpy(szB2,s+1);
	}

	return szBuf;
}

char *MenuTipFor(int menId) {
	static char szBuf[256];
	char szBuf2[32];
	MACROREF *	mp;

	if (menId != -1 && (mp = GetMacrorefForMenu(menId)) != 0) {
		mac_comment(szBuf, szBuf2, mp->index, mp->typ);
	} else {
		szBuf[0] = 0;
	}
	return szBuf;
}

/*--------------------------------------------------------------------------
 * ShowMenuHelp()
 */
void ShowMenuHelp(int menId)
{
	char			szBuf[256];
	char			szBuf2[32];
	MACROREF *	mp;

	st_switchtomenumode(menId != -1);
	if (menId != -1 && (mp = GetMacrorefForMenu(menId)) != 0) {
		mac_comment(szBuf, szBuf2, mp->index, mp->typ);
		st_seterrmsg(*szBuf ? szBuf : (char *)0);
	} else {
		st_seterrmsg((char *)0);
	}
}

/*------------------------------------------------------------
 * UpdateCommentAndName()
 */
static void UpdateCommentAndName(HWND hwnd)
{
	LONG		nSelected;
	WORD		nIndex,type;
	BOOL		editable;
	char		szName[64],szComment[256],szK[128];

	nSelected = SelectedMacro(hwnd);
	nIndex = HIWORD(nSelected);
	type = LOWORD(nSelected);
	DlgInitString(hwnd, IDD_STRING1, 
				mac_name(szName,nIndex,type), MAC_NAMELEN);
	DlgInitString(hwnd, IDD_STRING2, 
				mac_comment(szComment,szK,nIndex,type),MAC_COMMENTLEN);

	SendDlgItemMessage(hwnd, IDD_STRING1, EM_SETMODIFY, FALSE, 0);
	SendDlgItemMessage(hwnd, IDD_STRING2, EM_SETMODIFY, FALSE, 0);

	editable = (LOWORD(nSelected) == CMD_MACRO) ? TRUE : FALSE;

	SendDlgItemMessage(hwnd, IDD_STRING1, EM_SETREADONLY, !editable, 0);
	SendDlgItemMessage(hwnd, IDD_STRING2, EM_SETREADONLY, !editable, 0);

	/* but_enable(hwnd,IDD_MACSTART,editable); */
	but_enable(hwnd,IDD_MACRENAME,editable);
	but_enable(hwnd,IDD_MACDELETE,editable);
	but_enable(hwnd,IDD_MACEDIT,editable);
}

/*------------------------------------------------------------
 * NewMacroSelected()
 */
static void NewMacroSelected(HWND hwnd)
{
	HWND 	hwndList;
	LONG		nSelected;
	KEYBIND	*kp;
	int		nKeys = 0;
	WORD		nCurr;

	UpdateCommentAndName(hwnd);
	nSelected = SelectedMacro(hwnd);
	hwndList = list_startfilling(hwnd,IDD_LISTBOX2,&nCurr);
	for (kp = _keytables->rt_data; kp < (KEYBIND*)_keytables->rt_end; kp++) {
		if (MAKELONG(kp->macref.typ,kp->macref.index) == nSelected) {
			nKeys++;
			SendMessage(hwndList,LB_ADDSTRING,0,(LONG)kp->keycode);
		}
	}
	list_endfilling(hwndList,0);
	but_enable(hwnd,IDD_MACDELKEY,nKeys ? TRUE : FALSE);
}

/*------------------------------------------------------------
 * FillKeyTables()
 */
static void FillKeyTables(HWND hwnd)
{
	HWND 	hwndList;
	WORD		nCurr;
	RSCTABLE	*rp;

	hwndList = list_startfilling(hwnd,IDD_LISTBOX,&nCurr);
	for (rp = _keytables; rp; rp = rp->rt_next) {
		SendMessage(hwndList,LB_ADDSTRING,0,(LONG)rp->rt_name);
	}
	list_endfilling(hwndList,0);
	SendMessage(hwndList, LB_SELECTSTRING, -1, (LONG)_keytables->rt_name);
}

/*------------------------------------------------------------
 * Update()
 */
static void Update(HWND hwnd, LONG nSelected)
{
	UpdateMacroList(hwnd);
	if (nSelected) {
		SendDlgItemMessage(hwnd, IDD_MACROLIST,
			LB_SELECTSTRING, -1, nSelected);
	}
	NewMacroSelected(hwnd);
}

/*------------------------------------------------------------
 * mac_lboxdraw()
 */
static void mac_lboxdrawitem(HDC hdc, RECT *rcp, DWORD par, int nItem, 
					    int nID)
{
	char	szBuf[128];

	if (nID == IDD_LISTBOX2) {
		lstrcpy(szBuf,code2key(par));
	} else {
		mac_name(szBuf, HIWORD(par), LOWORD(par));
	}
	TextOut(hdc, rcp->left, rcp->top, szBuf, lstrlen(szBuf));
}

/*------------------------------------------------------------
 * mac_getkey()
 */
static KEYCODE mac_getkey(void)
{
	static KEYCODE k;
	static DIALPARS _d[] = { IDD_KEYCODE,	sizeof k,	&k, 0 };

	k = K_DELETED;
	if (DoDialog(DLGKEYCODE,(FARPROC)DlgStdProc,_d) == IDCANCEL ||
		k == 0) {
		return K_DELETED;
	}
	return k;
}
	
/*--------------------------------------------------------------------------
 * CharItemNextSelected()
 */
static int CharItemNextSelected(HWND hwndList, unsigned char ucKey)
{
	int			nItems;
	int			nOrigPos;
	LONG			lValue;
	unsigned char	szBuf[128];
	unsigned char 	ucCmp;
	int 			nPos;

	if (ucKey >= 'A' && ucKey <= 'Z') {
		ucKey += ('a' - 'A');
	} else if (ucKey >= 'a' && ucKey <= 'z') {
		;
	} else {
		return -2;
	}

	nItems = SendMessage(hwndList, LB_GETCOUNT, 0, 0L);
	nPos = SendMessage(hwndList, LB_GETCARETINDEX, 0, 0L);
	nOrigPos = nPos;

	do {

		if (++nPos >= nItems) {
			nPos = 0;
		}
		lValue = SendMessage(hwndList, LB_GETITEMDATA, nPos, 0);
		mac_name(szBuf, HIWORD(lValue), LOWORD(lValue));

		ucCmp = (szBuf[0] == '@') ? szBuf[1] : szBuf[0];
		if (ucCmp >= 'A' && ucCmp <= 'Z') {
			ucCmp += ('a' - 'A');
		}
		if (ucCmp == ucKey) {
			break;
		}

	} while(nPos != nOrigPos);
	
	return nPos;
}

/*------------------------------------------------------------
 * DlgMacEditProc()
 */
BOOL CALLBACK DlgMacEditProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char 			szComment[MAC_COMMENTLEN];
	char			szName[128];
	char			szN2[64];
	WORD			nNotify;
	int				nId;
	int				nKey;
	HWND			hwndListBox;
	MACROREFTYPE	nType;
	MACROREFIDX		nIndex;
	KEYCODE			keycode;
	KEYBIND *		kp;
	LONG			nSelected;
	COMPAREITEMSTRUCT *	cp;

	switch(message) {
		case WM_INITDIALOG:
			form_move(hwnd);
			nSelected = MAKELONG(currentSelectedMacro.typ, 
				currentSelectedMacro.index);
			CheckDlgButton(hwnd,IDD_OPT1,showInternals);
			FillKeyTables(hwnd);
			Update(hwnd, nSelected);
			return TRUE;

		case WM_VKEYTOITEM:
			return -1;

		case WM_CHARTOITEM:
			hwndListBox = GET_WM_CHARTOITEM_HWND(wParam, lParam);
			nKey = GET_WM_CHARTOITEM_CHAR(wParam, lParam); 
			return CharItemNextSelected(hwndListBox, nKey);

		case WM_DRAWITEM:
			return cust_combood((DRAWITEMSTRUCT*)lParam,
				mac_lboxdrawitem, 0);

		case WM_COMPAREITEM:
			cp = (COMPAREITEMSTRUCT*)lParam;
			return lstrcmp(
				mac_name(szName,HIWORD(cp->itemData1),LOWORD(cp->itemData1)),
				mac_name(szN2,HIWORD(cp->itemData2),LOWORD(cp->itemData2)));

		case WM_COMMAND:
			nSelected = SelectedMacro(hwnd);
			GetDlgItemText(hwnd,IDD_STRING1,szName,sizeof szName);
			_macroname = (LOWORD(nSelected) == CMD_MACRO) ?
				_macrotab[HIWORD(nSelected)]->name : 0;
			nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
			switch (nId = GET_WM_COMMAND_ID(wParam, lParam)) {

			case IDD_LISTBOX:
				if (nNotify == LBN_SELCHANGE) {
					if (LbGetText(hwnd,IDD_LISTBOX,szName) > 0) {
						rsc_switchtotable(&_keytables,szName);
						NewMacroSelected(hwnd);
					}
				}
				break;

			case IDD_MACROLIST:
				if (nNotify == LBN_DBLCLK) {
					SendMessage(hwnd,WM_COMMAND,IDD_MACSTART,0L);
				} else if (nNotify == LBN_SELCHANGE) {
					NewMacroSelected(hwnd);
				}
				break;

			case IDD_MACADDKEY:
				if ((keycode = mac_getkey()) == K_DELETED) {
					break;
				}
				if ((kp = keybound(keycode)) != 0 &&
				     ed_yn(IDS_MSGDELOLDKEYBIND,
						 mac_name(szName,kp->macref.index,
						 		kp->macref.typ)) == IDNO)
					break;
				mac_scaninsert(keycode,HIWORD(nSelected),
					LOWORD(nSelected));
				goto upd;

			case IDD_MACDELKEY:
				keycode = SelectedKey(hwnd);
				mac_scaninsert(keycode,-1,0);
				goto upd;

			case IDD_MACDELETE:
				mac_delete(szName);

			case IDD_OPT1:
				Update(hwnd, 0);
				break;

			case IDD_MACRENAME:
				if (nSelected >= 0) {
					GetDlgItemText(hwnd,IDD_STRING2,szComment,
								sizeof szComment);
					mac_nameupdate(HIWORD(nSelected),szName,szComment);
upd: 				_macedited = 1;
					NewMacroSelected(hwnd);
				}
				return TRUE;

			case IDD_MACFINDKEY:
				if ((keycode = mac_getkey()) == K_DELETED) {
					break;
				}
				if ((kp = keybound(keycode)) == 0) {
					ed_error(IDS_MSGKEYNOTBOUND);
				} else {
					nIndex = kp->macref.index;
					nType = kp->macref.typ;
					MacroSelectByValue(hwnd, MAKELONG(nType, nIndex));
					NewMacroSelected(hwnd);
				}
				return TRUE;

			case IDD_STRING1:
			case IDD_STRING2:
				if (nNotify != EN_KILLFOCUS) {
					break;
				}
				if (nSelected && 
				    SendDlgItemMessage(hwnd, nId, EM_GETMODIFY, 0, 0) &&
					ed_yn(IDS_MSGUPDATEMACNAME) == IDYES) {
					SendMessage(hwnd, WM_COMMAND, IDD_MACRENAME, 0L);
				}
				break;

			case IDCANCEL:
			case IDOK:
			case IDD_MACSTART:
			case IDD_MACEDIT:
				if (nSelected) {
					currentSelectedMacro.typ = LOWORD(nSelected);
					currentSelectedMacro.index = HIWORD(nSelected);
				}
				EndDialog(hwnd, wParam);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

