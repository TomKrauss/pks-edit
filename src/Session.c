/*
 * Session.c
 *
 * Session/history handling of PKSEDIT
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 03.04.1991
 */

#include <windows.h>
#include <string.h>
#include "documentmodel.h"
#include "history.h"
#include "arraylist.h"
#include "stringutil.h"
#include "jsonparser.h"
#include "crossreferencelinks.h"
#include "winterf.h"
#include "findandreplace.h"
#include "winfo.h"
#include "customcontrols.h"

static JSON_MAPPING_RULE _dockRules[] = {
	{	RT_CHAR_ARRAY, "name", offsetof(MAINFRAME_DOCK, do_name), sizeof(((MAINFRAME_DOCK*)NULL)->do_name)},
	{	RT_FLOAT, "x", offsetof(MAINFRAME_DOCK, do_x),},
	{	RT_FLOAT, "y", offsetof(MAINFRAME_DOCK, do_y),},
	{	RT_FLOAT, "w", offsetof(MAINFRAME_DOCK, do_w),},
	{	RT_FLOAT, "h", offsetof(MAINFRAME_DOCK, do_h),},
	{	RT_END}
};


static JSON_MAPPING_RULE _windowPlacementRules[] = {
	{	RT_INTEGER, "flags", offsetof(WINDOWPLACEMENT, flags),},
	{	RT_INTEGER, "show", offsetof(WINDOWPLACEMENT, showCmd),},
	{	RT_INTEGER, "top", offsetof(WINDOWPLACEMENT, rcNormalPosition.top)},
	{	RT_INTEGER, "left", offsetof(WINDOWPLACEMENT, rcNormalPosition.left)},
	{	RT_INTEGER, "bottom", offsetof(WINDOWPLACEMENT, rcNormalPosition.bottom)},
	{	RT_INTEGER, "right", offsetof(WINDOWPLACEMENT, rcNormalPosition.right)},
	{	RT_END}
};

static JSON_MAPPING_RULE _sessionDataRules[] = {
	{	RT_INTEGER, "screen-width", offsetof(SESSION_DATA, sd_screenCX)},
	{	RT_INTEGER, "screen-height", offsetof(SESSION_DATA, sd_screenCY)},
	{	RT_INTEGER, "search-replace-options", offsetof(SESSION_DATA, sd_searchAndReplaceOptions)},
	{	RT_NESTED_OBJECT, "main-window-placement", offsetof(SESSION_DATA, sd_mainWindowPlacement), {.r_t_nestedObjectRules = _windowPlacementRules}},
	{	RT_NESTED_OBJECT, "dock1", offsetof(SESSION_DATA, sd_dock1), {.r_t_nestedObjectRules = _dockRules}},
	{	RT_NESTED_OBJECT, "dock2", offsetof(SESSION_DATA, sd_dock2), {.r_t_nestedObjectRules = _dockRules}},
	{	RT_NESTED_OBJECT, "dock3", offsetof(SESSION_DATA, sd_dock3), {.r_t_nestedObjectRules = _dockRules}},
	{	RT_STRING_ARRAY, "search-patterns", offsetof(SESSION_DATA, sd_histories[SEARCH_PATTERNS])},
	{	RT_STRING_ARRAY, "replace-patterns", offsetof(SESSION_DATA, sd_histories[SEARCH_AND_REPLACE])},
	{	RT_STRING_ARRAY, "file-patterns", offsetof(SESSION_DATA, sd_histories[FILE_PATTERNS])},
	{	RT_STRING_ARRAY, "open-files", offsetof(SESSION_DATA, sd_histories[OPEN_FILES])},
	{	RT_STRING_ARRAY, "open-editors", offsetof(SESSION_DATA, sd_histories[OPEN_IN_EDITOR])},
	{	RT_STRING_ARRAY, "folders", offsetof(SESSION_DATA, sd_histories[FOLDERS])},
	{	RT_END}
};

static SESSION_DATA _sessionData;

extern void mainframe_readDocks();

/*
 *  Returns the array list containing the list of strings for the requested history type.
 */
static ARRAY_LIST* hist_getHistoryArray(HISTORY_TYPE type) {
	if (!_sessionData.sd_histories[type]) {
		_sessionData.sd_histories[type] = arraylist_create(13);
	}
	return _sessionData.sd_histories[type];
}

static MAINFRAME_DOCK* hist_dockByIndex(int aNumber) {
	switch (aNumber) {
	case 0: return &_sessionData.sd_dock1;
	case 1: return &_sessionData.sd_dock2;
	default: return &_sessionData.sd_dock3;
	}
}

/*
 * Save the placement of a mainframe docking slot.
 */
int hist_saveDockingPlacement(int aNumber, char* pszDockName, float x, float y, float w, float h) {
	MAINFRAME_DOCK* pDock = hist_dockByIndex(aNumber);
	strcpy(pDock->do_name, pszDockName);
	pDock->do_x = x;
	pDock->do_y = y;
	pDock->do_w = w;
	pDock->do_h = h;
	pDock = hist_dockByIndex(aNumber+1);
	if (pDock) {
		pDock->do_name[0] = 0;
	}
	return 1;
}

/*
 * Read the placement of a mainframe docking slot.
 */
int hist_readDockingPlacement(int aNumber, char* pszDockName, float* x, float* y, float* w, float* h) {
	if (aNumber >= 3) {
		return 0;
	}
	MAINFRAME_DOCK* pDock = hist_dockByIndex(aNumber);
	if (!pDock->do_name[0]) {
		return 0;
	}
	strcpy(pszDockName, pDock->do_name);
	*x = pDock->do_x;
	*y = pDock->do_y;
	*w = pDock->do_w;
	*h = pDock->do_h;
	return 1;
}

/*
 * Returns the current history entry.
 */
void hist_getCurrentHistoryEntry(HISTORY_TYPE type, char* pszDest, size_t nSize) {
	pszDest[0] = 0;
	ARRAY_LIST* pArray = hist_getHistoryArray(type);
	size_t nArrayLength;
	if (pArray != NULL && (nArrayLength = arraylist_size(pArray)) > 0) {
		strncpy(pszDest, arraylist_get(pArray, (int)(nArrayLength -1)), nSize-1);
		pszDest[nSize - 1] = 0;
	}
}

/*---------------------------------*/
/* hist_saveString()					*/
/*---------------------------------*/
EXPORT void hist_saveString(HISTORY_TYPE type, const char *string) {
	ARRAY_LIST* pArray = hist_getHistoryArray(type);

	if (arraylist_size(pArray) >= MAXHIST) {
		void* pFirstElement = arraylist_get(pArray, 0);
		arraylist_remove(pArray, pFirstElement);
		free(pFirstElement);
	}
	ARRAY_ITERATOR iterator = arraylist_iterator(pArray);
	while (iterator.i_buffer < iterator.i_bufferEnd) {
		char* pElement = *iterator.i_buffer;
		if (strcmp(string, pElement) == 0) {
			arraylist_remove(pArray, pElement);
			free(pElement);
			break;
		}
		iterator.i_buffer++;
	}

	arraylist_add(pArray, _strdup(string));
}

static ARRAY_LIST* hist_getOpenFilePathes() {
	ARRAY_LIST* pArray = hist_getHistoryArray(FOLDERS);
	WINFO* wp = ww_getCurrentEditorWindow();
	while (wp) {
		char szPath[1024];
		FTABLE* fp = wp->fp;
		string_splitFilename(fp->fname, szPath, 0, 0);
		if (arraylist_indexOfComparing(pArray, szPath, strcmp) < 0) {
			arraylist_insertAt(pArray, _strdup(szPath), 0);
		}
		wp = wp->next;
	}
	return pArray;
}

/*------------------------------------------------------------
 * hist_combo()
 */
EXPORT void hist_fillComboBox(HWND hDlg, WORD nItem, HISTORY_TYPE type, BOOL bForceInit)
{	char *p,*q = 0;
	HWND hwnd = GetDlgItem(hDlg, nItem);
	if (cust_isEditField(hwnd)) {
		return;
	}
	ARRAY_LIST* pArray = type == FOLDERS ? hist_getOpenFilePathes() : hist_getHistoryArray(type);

	SendDlgItemMessage(hDlg, nItem, CB_RESETCONTENT, 0,0L);
	int nSize = (int)arraylist_size(pArray);
	for (int i = nSize; --i >= 0; ) {
		p = arraylist_get(pArray, i);
		if (!q) {
			q = p;
		}
		if (SendDlgItemMessage(hDlg, nItem, CB_ADDSTRING, 0, (LPARAM)p) < 0) {
			break;
		}
	}
	if (bForceInit && q) {
		SetDlgItemText(hDlg, nItem, q);
	}
}

/*--------------------------------------------------------------------------
 * hist_getString()
 */
char *hist_getString(HISTORY_TYPE type, int nItem) {
	ARRAY_LIST* pArray = hist_getHistoryArray(type);

	int nSize = (int)arraylist_size(pArray);
	if (nItem < nSize) {
		return arraylist_get(pArray, nSize - nItem - 1);
	}
	return (char *)0;
}

/*
 * Returns abritrary additional session data from the last session.
 */
SESSION_DATA* hist_getSessionData() {
	return &_sessionData;
}

/*--------------------------------------------------------------------------
 * hist_applyScreenRatio()
 * Apply a screen ratio factor after restoring a window placement depending on the old
 * screen values preserved in the last session.
 */
static void hist_applyScreenRatio(WINDOWPLACEMENT *wsp) {
	wsp->length = sizeof * wsp;
	if (_sessionData.sd_screenCX == 0 || _sessionData.sd_screenCY == 0) {
		return;
	}
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	wsp->ptMinPosition.x = wsp->ptMinPosition.x  * x / _sessionData.sd_screenCX;
	wsp->ptMinPosition.y = wsp->ptMinPosition.y * y / _sessionData.sd_screenCY;
	wsp->ptMaxPosition.x = wsp->ptMaxPosition.x * x / _sessionData.sd_screenCX;
	wsp->ptMaxPosition.y = wsp->ptMaxPosition.y * y / _sessionData.sd_screenCY;

	wsp->rcNormalPosition.left = wsp->rcNormalPosition.left * x / _sessionData.sd_screenCX;
	wsp->rcNormalPosition.right = wsp->rcNormalPosition.right * x / _sessionData.sd_screenCX;

	wsp->rcNormalPosition.top = wsp->rcNormalPosition.top * y / _sessionData.sd_screenCY;
	wsp->rcNormalPosition.bottom = wsp->rcNormalPosition.bottom * y / _sessionData.sd_screenCY;
	if (wsp->rcNormalPosition.left < -10) {
		wsp->rcNormalPosition.left = 0;
	}
	if (wsp->rcNormalPosition.top < -1) {
		wsp->rcNormalPosition.top = 0;
	}
}

/*
 * Convert an array of strings to a line list.
 */
static void hist_convertToLines(LINE** lpFirst, ARRAY_LIST* pArray) {
	LINE* lpPrev = NULL;
	size_t nSize = arraylist_size(pArray);
	for (int i = 0; i < nSize; i++) {
		char* pszString = arraylist_get(pArray, i);
		size_t nLen = strlen(pszString);
		LINE* lp = ln_create((int) nLen);
		memcpy(lp->lbuf, pszString, nLen);
		if (lpPrev) {
			lpPrev->next = lp;
		} else {
			*lpFirst = lp;
		}
		lpPrev = lp;
	}
}

/*---------------------------------
 * hist_readSession()
 * Read the current session - all history entries from the session file 'pszFilename'
 */
EXPORT int hist_readSession(const char* pszFilename) {
	if (json_parse(pszFilename, FALSE, &_sessionData, _sessionDataRules)) {
		hist_applyScreenRatio(&_sessionData.sd_mainWindowPlacement);
		mainframe_readDocks();
		LINE* lp = NULL;
		hist_convertToLines(&lp, hist_getHistoryArray(OPEN_IN_EDITOR));
		xref_openWindowHistory(lp);
		ln_listfree(lp);
		_currentSearchAndReplaceParams.options = hist_getSessionData()->sd_searchAndReplaceOptions;
		return 1;
	}
	return 0;
}

/*---------------------------------
 * hist_saveSession()
 * Save the current session - all history entries to the session file 'pszFilename'
 */
EXPORT void hist_saveSession(char* pszFilename) {	

	_sessionData.sd_screenCX = GetSystemMetrics(SM_CXSCREEN);
	_sessionData.sd_screenCY = GetSystemMetrics(SM_CYSCREEN);
	WINDOWPLACEMENT* wsp = &_sessionData.sd_mainWindowPlacement;
	wsp->length = sizeof * wsp;
	GetWindowPlacement(hwndMain, wsp);
	json_marshal(pszFilename, &_sessionData, _sessionDataRules);
}

/*
 * Destroy a string list containing allocated strings.
 */
static void hist_destroyStringList(ARRAY_LIST* pList) {
	if (pList == 0) {
		return;
	}
	ARRAY_ITERATOR iterator = arraylist_iterator(pList);

	while (iterator.i_buffer < iterator.i_bufferEnd) {
		free(*iterator.i_buffer);
		iterator.i_buffer++;
	}
	arraylist_destroy(pList);
}

/*
 * hist_resetHistory(HISTORY_TYPE type)
 * Reset the specified history to be empty.
 */
EXPORT void hist_resetHistory(HISTORY_TYPE type) {
	ARRAY_LIST* pArray = _sessionData.sd_histories[type];
	if (pArray) {
		hist_destroyStringList(pArray);
		_sessionData.sd_histories[type] = NULL;
	}
}

/*
 * Destroy all data structures related to the PKS Edit session data. 
 */
void hist_destroy() {
	for (int i = 0; i < sizeof(_sessionData.sd_histories) / sizeof(_sessionData.sd_histories[0]); i++) {
		hist_destroyStringList(_sessionData.sd_histories[i]);
	}
}
