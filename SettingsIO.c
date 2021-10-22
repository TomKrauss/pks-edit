/*
 * SettingsIO.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: save and restore settings
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
#include <dos.h>
#include <stdio.h>
#include "trace.h"
#include "linkedlist.h"
#include "documentmodel.h"
#include "edierror.h"
#include "documenttypes.h"

#include "winfo.h"
#include "winterf.h"
#include "stringutil.h"

#pragma hdrstop

#include "fileselector.h"
#include "pksedit.h"
#include "editorconfiguration.h"
#include "findandreplace.h"
#include "fileutil.h"
#include "mouseutil.h"
#include "winutil.h"
#include "themes.h"

#define	PROF_OFFSET		1

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
 * Save the position and details configured for the main window.
 */
extern void main_saveFrameState();

/*
 * The absolute path name of the last PKSEDIT.INI file read or written. 
 */
static char _pksEditIniFilename[512];
static FSELINFO _setfselinfo = { ".", "PKSEDIT.INI", "*.INI" };
static char *_desk = "desk";
static char* _mainframeDock = "mainframeDock";
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
	if ((fn = fsel_initPathes(&_setfselinfo)) == 0) {
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
	if (file_exists(fn) == 0) {
		string_getFullPathName(_pksEditIniFilename, fn);
	}
}

/*------------------------------------------------------------
 * prof_getPksProfileString()
 * Fetches a string from the PKS profile ini file. 
 */
int prof_getPksProfileString(char *pGroup, char *ident, char *string, int maxlen)
{
	if (!LocatePksEditIni())
		return 0;
	return
		GetPrivateProfileString(pGroup, ident, "", string, maxlen ,_pksEditIniFilename);
}

/*------------------------------------------------------------
 * prof_getPksStandardString()
 * Fetches a string from the standard section of the PKS profile.
 */
int prof_getPksStandardString(char* ident, char* string, int maxlen) {
	return prof_getPksProfileString(_desk, ident, string, maxlen);
}

/*------------------------------------------------------------
 * prof_savestring()
 * Saves a string in the standard section of the PKS profile.
 */
int prof_savePksStandardString(char* ident, char* string) {
	return prof_savestring(_desk, ident, string);
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
 * Returns the window settings from the pksedit.ini file.
 */
EXPORT int prof_getWindowSettings(char *string, WINDOWPLACEMENT *wsp)
{
	GetScreenRatioValues();
	wsp->length = sizeof *wsp;
	wsp->flags = string_convertToLong(string);
	wsp->showCmd = string_convertToLong(_strtolend);
	wsp->ptMinPosition.x = string_convertToLong(_strtolend) * _newXScreen / _oldXScreen;
	wsp->ptMinPosition.y = string_convertToLong(_strtolend) * _newYScreen / _oldYScreen;
	wsp->ptMaxPosition.x = string_convertToLong(_strtolend) * _newXScreen / _oldXScreen;
	wsp->ptMaxPosition.y = string_convertToLong(_strtolend) * _newYScreen / _oldYScreen;
	wsp->rcNormalPosition.left = string_convertToLong(_strtolend) * _newXScreen / _oldXScreen;
	wsp->rcNormalPosition.top = string_convertToLong(_strtolend) * _newYScreen / _oldYScreen;
	wsp->rcNormalPosition.right = string_convertToLong(_strtolend) * _newXScreen / _oldXScreen;
	wsp->rcNormalPosition.bottom = string_convertToLong(_strtolend) * _newYScreen / _oldYScreen;
	if (wsp->rcNormalPosition.left < -10) {
		wsp->rcNormalPosition.left = 0;
	}
	if (wsp->rcNormalPosition.top < -1) {
		wsp->rcNormalPosition.top = 0;
	}
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
	if (!prof_getPksProfileString(_desk,ident,string,sizeof string -1)) {
		return 0;
	}
	return prof_getWindowSettings(string,wsp);
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

/*
 * Save the placement of a mainframe docking slot. 
 */
int prof_saveDockingPlacement(int aNumber, char* pszDockName, float x, float y, float w, float h) {
	char	string[256];
	char	szIdent[20];

	sprintf(string, "%s %.2f %.2f %.2f %.2f", pszDockName, x, y, w, h);
	sprintf(szIdent, "%s%d", _mainframeDock, aNumber);
	prof_savestring(_desk, szIdent, string);
	sprintf(szIdent, "%s%d", _mainframeDock, aNumber+1);
	return prof_savestring(_desk, szIdent, NULL);
}

/*
 * Read the placement of a mainframe docking slot.
 */
int prof_readDockingPlacement(int aNumber, char* pszDockName, float* x, float* y, float* w, float* h) {
	char	string[256];
	char	szIdent[20];
	wsprintf(szIdent, "%s%d", _mainframeDock, aNumber);
	if (!prof_getPksProfileString(_desk, szIdent, string, sizeof string - 1)) {
		return 0;
	}
	return sscanf(string, "%s %f %f %f %f", pszDockName, x, y, w, h) == 5;
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

	prof_getPksProfileString(grp,ident,string,sizeof string -1);
	return string_convertToLong(string);
}

/*------------------------------------------------------------
 * prof_getstdopt()
 */
int prof_getstdopt(void) {
	if (!LocatePksEditIni()) {
		return 0;
	}
	EDITOR_CONFIGURATION* pConfiguration = GetConfiguration();

	prof_getPksProfileString(_desk, "Language", pConfiguration->language, sizeof pConfiguration->language -1);
	if (pConfiguration->language[0]) {
		ui_switchToLanguage(pConfiguration->language);
	}
	prof_getPksProfileString(_desk, "Theme", pConfiguration->themeName, sizeof pConfiguration->themeName - 1);
	if (pConfiguration->themeName[0]) {
		theme_setCurrent(pConfiguration->themeName);
	}
	pConfiguration->options = prof_getlong(_desk,"Options");
	pConfiguration->layoutoptions = prof_getlong(_desk,"Layout");

	_currentSearchAndReplaceParams.options = (int)prof_getlong(_desk,"FindOptions");
	char buf[32];
	prof_getPksProfileString(_desk, "AsInterv", buf, sizeof buf);
	size_t l = strlen(buf);
	int multiplier = 1;
	if (l > 0 && buf[l - 1] != 's') {
		buf[l - 1] = 0;
		multiplier = 60;
	}
	pConfiguration->autosaveSeconds = multiplier * string_convertToLong(buf);
	pConfiguration->nundo = prof_getlong(_desk,"NUBuf");
	pConfiguration->maximumNumberOfOpenWindows = prof_getlong(_desk, "maxOpenWindows");
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

/*------------------------------------------------------------
 * prof_save()
 * Save the configuration. If interactive is passed, then open
 * a file selector to pick the pksedit.ini file to which the config
 * is saved.
 */
int prof_save(EDITOR_CONFIGURATION* configuration, int interactive)
{
	int  	fd;
	char *	fn;

	if (!interactive) {
		fn = _pksEditIniFilename;

	} else {
		LocatePksEditIni();
		string_splitFilename(_pksEditIniFilename, _setfselinfo.path, _setfselinfo.fname);
		FILE_SELECT_PARAMS params;
		params.fsp_saveAs = TRUE;
		params.fsp_encryptedAvailable = FALSE;
		if ((fn = fsel_selectFileWithOptions(&_setfselinfo, MOPTION, &params)) == 0) {
			return 0;
		}
	}

	/* create non existing profiles, others will be updated */
	if (file_exists(fn) < 0) {
		if ((fd = file_createFile(fn)) < 0) {
			return 0;
		}
		_lclose(fd);
	} else {
		prof_killsections(fn, "icons");
	}

	mouse_setBusyCursor();
	lstrcpy(_pksEditIniFilename,fn);

	if (configuration->themeName[0]) {
		prof_savestring(_desk, "Theme", configuration->themeName);
	}
	if (configuration->language[0]) {
		prof_savestring(_desk, "Language", configuration->language);
	}
	prof_savelong(_desk,"Options",(long)configuration->options);
	prof_savelong(_desk,"Layout",(long)configuration->layoutoptions);
	prof_savelong(_desk,"FindOptions",(long)_currentSearchAndReplaceParams.options);
	char szBuf[32];
	sprintf(szBuf, "%ds", configuration->autosaveSeconds);
	prof_savestring(_desk,"AsInterv", szBuf);
	prof_savelong(_desk, "maxOpenWindows", (long)configuration->maximumNumberOfOpenWindows);
	config_saveTempPath();
	prof_savelong(_desk,"NUBuf",(long)configuration->nundo);
	prof_savelong(_desk, _cxscreen, (long)GetSystemMetrics(SM_CXSCREEN));
	prof_savelong(_desk, _cyscreen, (long)GetSystemMetrics(SM_CYSCREEN));

	main_saveFrameState();
	mouse_setDefaultCursor();
	return 1;
}

/*--------------------------------------------------------------------------
 * prof_enum()
 */
int prof_enum(LPSTR grp, intptr_t (*lpfnEnum)(LPSTR, LONG), LONG lParam)
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
void *prof_llinsert(void *pHead, int size, char *pszGroup, char *pszItem, char **idata)
{
	char			szBuf[256],*s;
	struct llist 	*lp;

	if (!prof_getPksProfileString(pszGroup, pszItem, szBuf, sizeof szBuf))
		return 0;

	if ((s = _strdup(szBuf)) == 0) {
		return 0;
	}

	if ((lp = ll_find(*(void**)pHead,pszItem)) == 0) {
		if ((lp = ll_insert(pHead,size)) == 0) {
			free(s);
			return 0;
		}

		lstrcpy(lp->name,pszItem);
	}
	*idata = s;

	return lp;	
}

