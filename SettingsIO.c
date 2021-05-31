/*
 * SettingsIO.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: save and restore settings
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>
#include <dos.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"
#include "stringutil.h"

#pragma hdrstop

#include "edifsel.h"
#include "pksedit.h"
#include "editorconfiguration.h"
#include "findandreplace.h"

#define	PROF_OFFSET		1

extern void MouseBusy(void);
extern void MouseNotBusy(void);
extern int  EdStat(char *s, int mode);
extern LONG prof_getlong(char *grp,char *ident);

typedef enum {	I_FLAG, I_WINDOW, I_VAL, I_STRING, I_INVAL } ITEMTYPE;
typedef enum {	G_DESK, G_ENVIRON, G_HISTORY } GRPTYPE;

typedef struct item {
	ITEMTYPE	p_typ;
	GRPTYPE	p_grp;
	int		idx;
	char 	*p_name;
} PROFILEITEM;

struct llist {
	struct llist 	*next;
	char			name[4];
};

/*
 * The absolute path name of the last PKSEDIT.INI file read or written. 
 */
static char _pksEditIniFilename[512];
static FSELINFO _setfselinfo = { ".", "PKSEDIT.INI", "*.INI" };
static char *_desk = "desk";
static char *_cxscreen = "CXScreen";
static char *_cyscreen = "CYScreen";

/*------------------------------------------------------------
 * LocatePksEditIni()
 */
static int LocatePksEditIni(void)
{	char *fn;

	if (_pksEditIniFilename[0]) {
		return 1;
	}
	if ((fn = rw_init(&_setfselinfo)) == 0) {
		return 0;
	}
	GetFullPathName(fn, sizeof _pksEditIniFilename, _pksEditIniFilename, NULL);
	return 1;
}

/*--------------------------------------------------------------------------
 * prof_setinifile()
 */
void prof_setinifile(char *fn)
{
	if (EdStat(fn, 0xFF) == 0) {
		FullPathName(_pksEditIniFilename, fn);
	}
}

/*------------------------------------------------------------
 * GetPksProfileString()
 */
int GetPksProfileString(char *grp, char *ident, char *string, int maxlen)
{
	if (!LocatePksEditIni())
		return 0;
	return
		GetPrivateProfileString(grp, ident, "", string, maxlen ,_pksEditIniFilename);
}

/*--------------------------------------------------------------------------
 * GetScreenRationValues()
 */
static long	_newXScreen, _oldXScreen = -1L, _newYScreen, _oldYScreen;
static void GetScreenRatioValues(void)
{
	if (_oldXScreen > 0) {
		return;
	}
	_newXScreen = GetSystemMetrics(SM_CXSCREEN);
	_newYScreen = GetSystemMetrics(SM_CYSCREEN);
	if ((_oldXScreen = prof_getlong(_desk, _cxscreen)) == 0) {
		_oldXScreen = _newXScreen;
	}
	if ((_oldYScreen = prof_getlong(_desk, _cyscreen)) == 0) {
		_oldYScreen = _newYScreen;
	}
}

/*--------------------------------------------------------------------------
 * prof_adjustpoint()
 */
EXPORT void prof_adjustpoint(PPOINT pPoint)
{
	int		y;

	GetScreenRatioValues();
	pPoint->x = (long)pPoint->x * _newXScreen / _oldXScreen;
	y = (long)pPoint->y * _newYScreen / _oldYScreen;
	pPoint->y = y;
}

/*------------------------------------------------------------
 * prof_getws()
 */
EXPORT int prof_getws(char *string, WINDOWPLACEMENT *wsp)
{
	GetScreenRatioValues();
	wsp->length = sizeof *wsp;
	wsp->flags = Atol(string);
	wsp->showCmd = Atol(_strtolend);
	wsp->ptMinPosition.x = Atol(_strtolend) * _newXScreen / _oldXScreen;
	wsp->ptMinPosition.y = Atol(_strtolend) * _newYScreen / _oldYScreen;
	wsp->ptMaxPosition.x = Atol(_strtolend) * _newXScreen / _oldXScreen;
	wsp->ptMaxPosition.y = Atol(_strtolend) * _newYScreen / _oldYScreen;
	wsp->rcNormalPosition.left = Atol(_strtolend) * _newXScreen / _oldXScreen;
	wsp->rcNormalPosition.top = Atol(_strtolend) * _newYScreen / _oldYScreen;
	wsp->rcNormalPosition.right = Atol(_strtolend) * _newXScreen / _oldXScreen;
	wsp->rcNormalPosition.bottom = Atol(_strtolend) * _newYScreen / _oldYScreen;
	return 1;
}

/*------------------------------------------------------------
 * prof_getwinstate()
 */
EXPORT int prof_getwinstate(char *wname, int nr, WINDOWPLACEMENT *wsp)
{
	char 	ident[128];
	char	string[256];

	wsprintf(ident,"%s%d",(LPSTR)wname,nr);
	if (!GetPksProfileString(_desk,ident,string,sizeof string -1)) {
		return 0;
	}
	return prof_getws(string,wsp);
}

/*------------------------------------------------------------
 * prof_printws()
 */
EXPORT void prof_printws(char *buf, WINDOWPLACEMENT *wsp)
{
	wsprintf(buf,"0x%x 0x%x %d %d %d %d %d %d %d %d", 
		wsp->flags, wsp->showCmd,
		wsp->ptMinPosition.x, wsp->ptMinPosition.y,
		wsp->ptMaxPosition.x, wsp->ptMaxPosition.y,
		wsp->rcNormalPosition.left, wsp->rcNormalPosition.top,
		wsp->rcNormalPosition.right, wsp->rcNormalPosition.bottom);
}

/*------------------------------------------------------------
 * prof_savestring()
 */
int prof_savestring(char *grp, char *ident, char *string)
{
	return
		WritePrivateProfileString(grp,ident,string,_pksEditIniFilename);
}

/*--------------------------------------------------------------------------
 * prof_killentry()
 */
int prof_killentry(char *grp, char *ident)
{
	return prof_savestring(grp, ident, (char *)0);
}

/*------------------------------------------------------------
 * prof_savewinstate()
 */
int prof_savewinstate(char *wname, int nr, WINDOWPLACEMENT *wsp)
{	char string[256],ident[128];

	wsprintf(ident,"%s%d",(LPSTR)wname,nr);
	prof_printws(string, wsp);
	return
		prof_savestring(_desk,ident,string);
}

/*------------------------------------------------------------
 * prof_savelong()
 */
int prof_savelong(char *grp, char *ident, LONG val)
{ 	char string[256];

	wsprintf(string,"%ld",val);
	return
		prof_savestring(grp,ident,string);
}

/*------------------------------------------------------------
 * prof_getlong()
 */
LONG prof_getlong(char *grp,char *ident)
{ 	char string[256];

	GetPksProfileString(grp,ident,string,sizeof string -1);
	return Atol(string);
}

/*------------------------------------------------------------
 * prof_getstdopt()
 */
int prof_getstdopt(void)
{
	if (!LocatePksEditIni()) {
		return 0;
	}

	GetConfiguration()->options = prof_getlong(_desk,"Options");
	GetConfiguration()->layoutoptions = prof_getlong(_desk,"Layout");

	_currentSearchAndReplaceParams.options = (int)prof_getlong(_desk,"FindOptions");
	GetConfiguration()->asminutes = prof_getlong(_desk,"AsInterv");
	GetConfiguration()->nundo = prof_getlong(_desk,"NUBuf");
	GetPksProfileString(_desk,"AsPath", GetConfiguration()->pksEditTempPath, member_size(EDITOR_CONFIGURATION, pksEditTempPath) -1);

	return 1;
}

/*--------------------------------------------------------------------------
 * prof_killsections()
 */
void prof_killsections(LPSTR pszFn, LPSTR pszSection)
{
	if (!pszFn) {
		if (!LocatePksEditIni()) {
			return;
		}
		pszFn = _pksEditIniFilename;
	}
	prof_savestring(pszSection, (char *)0, (char *)0);
}

/*--------------------------------------------------------------------------
 * prof_saveaspath()
 * Save the temp path of PKS editor to the pksedit.ini file.
 */
void prof_saveaspath(EDITOR_CONFIGURATION* configuration)
{
	prof_savestring(_desk,"AsPath", configuration->pksEditTempPath);
}

/*------------------------------------------------------------
 * prof_save()
 * Save the configuration. If interactive is passed, then open
 * a file selector to pick the pksedit.ini file to which the config
 * is saved.
 */
int prof_save(EDITOR_CONFIGURATION* configuration, int interactive)
{
	BOOL		bDidExist;
	int  	fd;
	char *	fn;
	WINDOWPLACEMENT 	ws;

	if (!interactive) {
		fn = _pksEditIniFilename;

	} else {
		LocatePksEditIni();
		sfsplit(_pksEditIniFilename, _setfselinfo.path, _setfselinfo.fname);
		if ((fn = rw_select(&_setfselinfo, MOPTION, TRUE)) == 0) {
			return 0;
		}
	}

	/* create non existing profiles, others will be updated */
	if (EdStat(fn,0xFF) < 0) {
		if ((fd = EdCreate(fn)) < 0) {
			return 0;
		}
		bDidExist = FALSE;
		_lclose(fd);
	} else {
		bDidExist = TRUE;
		prof_killsections(fn, "icons");
	}

	MouseBusy();
	lstrcpy(_pksEditIniFilename,fn);

	prof_savelong(_desk,"Options",(long)configuration->options);
	prof_savelong(_desk,"Layout",(long)configuration->layoutoptions);
	prof_savelong(_desk,"FindOptions",(long)_currentSearchAndReplaceParams.options);
	prof_savelong(_desk,"AsInterv",(long)configuration->asminutes);
	prof_saveaspath(configuration);
	prof_savelong(_desk,"NUBuf",(long)configuration->nundo);
	prof_savelong(_desk, _cxscreen, (long)GetSystemMetrics(SM_CXSCREEN));
	prof_savelong(_desk, _cyscreen, (long)GetSystemMetrics(SM_CYSCREEN));

	win_getstate(hwndFrame,&ws);
	prof_savewinstate(szFrameClass,0,&ws);
	ww_savewinstates();
	ic_profsavepos();
	
	if (!bDidExist) {
		SaveAllDocumentTypes((void *)0);
	}

	MouseNotBusy();
	return 1;
}

/*--------------------------------------------------------------------------
 * prof_enum()
 */
int prof_enum(LPSTR grp, int (*lpfnEnum)(LPSTR, LONG), LONG lParam)
{
	char *s;

	if (!LocatePksEditIni()) {
		return 1;
	}

	GetPrivateProfileString(grp, NULL, "", _linebuf, LINEBUFSIZE, _pksEditIniFilename);
	s = _linebuf;
	while(*s) {
		if (!(*lpfnEnum)(s,lParam))
			return 0;
		while(*s++)
			;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * prof_llinsert()
 */
void *prof_llinsert(void *Head, int size, char *group, char *item, char **idata)
{
	char			szBuf[256],*s;
	struct llist 	*lp;

	if (!GetPksProfileString(group, item, szBuf, sizeof szBuf))
		return 0;

	if ((s = stralloc(szBuf)) == 0) {
		return 0;
	}

	if ((lp = (struct llist*)ll_find(*(void**)Head,item)) == 0) {
		if ((lp = (struct llist*)ll_insert(Head,size)) == 0) {
			_free(s);
			return 0;
		}

		lstrcpy(lp->name,item);
	}
	*idata = s;

	return lp;	
}

