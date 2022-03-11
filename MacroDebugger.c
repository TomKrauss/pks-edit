/*
 * MacroDebugger.c
 *
 * PKS-EDIT PKS-MacroC debugger
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 3/2022
 */

#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include "arraylist.h"
#include "pksmacrocvm.h"
#include "pksmacro.h"
#include "linkedlist.h"
#include "symbols.h"
#include "pksrc.h"
#include "xdialog.h"

/*
 * Initialize the debugger list views.
 */
static void debugger_initVariablesView(HWND hwnd) {
	LVCOLUMN lvc;

	ListView_SetExtendedListViewStyleEx(hwnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;

	ListView_InsertColumn(hwnd, 0, &lvc);

	lvc.pszText = "Value";
	lvc.cx = 250;
	lvc.iSubItem = 1;
	ListView_InsertColumn(hwnd, 1, &lvc);
}


 /*--------------------------------------------------------------------------
  * debugger_addVariablesTypesToListView()
  * Adds all variable names+values to a list view.
  */
static void debugger_addVariablesTypesToListView(HWND hwndList, IDENTIFIER_CONTEXT* pContext) {
	ListView_DeleteAllItems(hwndList);
	LVITEM lvI;

	lvI.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	lvI.state = 0;
	ARRAY_LIST* pVarnames = sym_getVariables(pContext);
	size_t nSize = arraylist_size(pVarnames);
	for (int i = 0; i < nSize; i++) {
		const char* pszName = arraylist_get(pVarnames, i);
		lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
		lvI.iItem = i;
		lvI.lParam = (LPARAM)pszName;
		lvI.pszText = LPSTR_TEXTCALLBACK;
		lvI.iSubItem = 0;
		if (ListView_InsertItem(hwndList, &lvI) == -1) {
			return;
		}
	}
	arraylist_destroy(pVarnames);
}

/*------------------------------------------------------------
 * doctypes_fillListbox()
 */
static void debugger_fillVariables(HWND hwnd, EXECUTION_CONTEXT* pContext) {
	HWND hwndList = GetDlgItem(hwnd, IDC_LOCAL_VARS);
	debugger_addVariablesTypesToListView(hwndList, pContext->ec_identifierContext);
	hwndList = GetDlgItem(hwnd, IDC_GLOBAL_VARS);
	debugger_addVariablesTypesToListView(hwndList, sym_getGlobalContext());
}

static void debugger_getColumnParameters(NMLVDISPINFO* plvdi, IDENTIFIER_CONTEXT* pContext) {
	char* pszName = (char*)plvdi->item.lParam;
	static char szValueBuffer[256];

	switch (plvdi->item.iSubItem) {
	case 0:
		plvdi->item.pszText = pszName;
		break;
	case 1: {
		PKS_VALUE v = sym_getVariable(pContext, pszName);
		decompile_printValue(szValueBuffer, v);
		plvdi->item.pszText = szValueBuffer;
		break;
	}
	}
}

/*
 * Fills in the method stack.
 */
static void debugger_fillStacktrace(HWND hwndList, EXECUTION_CONTEXT* pContext) {
	SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);
	SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
	while (pContext) {
		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)pContext->ec_currentFunction);
		pContext = pContext->ec_parent;
	}
	SendMessage(hwndList, WM_SETREDRAW, (WPARAM)TRUE, 0L);
	SendMessage(hwndList, LB_SETCURSEL, 0, (LPARAM)0);
}

static void debugger_fillCodeDisplay(HWND hwndList, EXECUTION_CONTEXT* pContext) {
	SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);
	SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
	int nSelection;
	ARRAY_LIST* pDecompiled = deccompile_macroNamed(pContext->ec_currentFunction, DM_CODE, pContext->ec_instructionPointer, &nSelection);
	size_t nElements = arraylist_size(pDecompiled);
	for (int i = 0; i < nElements; i++) {
		SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)arraylist_get(pDecompiled, i));
	}
	arraylist_destroyStringList(pDecompiled);
	ListBox_SetCurSel(hwndList, nSelection);
	SendMessage(hwndList, WM_SETREDRAW, (WPARAM)TRUE, 0L);
}

static EXECUTION_CONTEXT* _stackTopContext;
static char* _debugErrorMessage;

/*
 * Custom dialog procedure for debugger.
 */
static INT_PTR debugger_dialogProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static EXECUTION_CONTEXT* _selectedContext;
	int idCtrl;
	int nNotify;

	switch (message) {
	case WM_INITDIALOG:
		_selectedContext = _stackTopContext;
		SetDlgItemText(hwnd, IDS_ERROR_MESSAGE, _debugErrorMessage);
		debugger_fillStacktrace(GetDlgItem(hwnd, IDC_METHOD_STACK), _stackTopContext);
		debugger_initVariablesView(GetDlgItem(hwnd, IDC_LOCAL_VARS));
		debugger_initVariablesView(GetDlgItem(hwnd, IDC_GLOBAL_VARS));
		debugger_fillVariables(hwnd, _selectedContext);
		debugger_fillCodeDisplay(GetDlgItem(hwnd, IDC_CODE), _selectedContext);
		break;
	case WM_COMMAND:
		idCtrl = GET_WM_COMMAND_ID(wParam, lParam);
		nNotify = GET_WM_COMMAND_CMD(wParam, lParam);
		if (idCtrl == IDC_METHOD_STACK && nNotify == LBN_SELCHANGE) {
			LRESULT nIndex = SendDlgItemMessage(hwnd, idCtrl, LB_GETCURSEL, 0, 0);
			if (nIndex >= 0) {
				_selectedContext = (EXECUTION_CONTEXT*)ll_at((LINKED_LIST*)_stackTopContext, (int)nIndex);
				debugger_fillVariables(hwnd, _selectedContext);
				debugger_fillCodeDisplay(GetDlgItem(hwnd, IDC_CODE), _selectedContext);
			}
		}
		if (idCtrl == IDCANCEL || idCtrl == IDOK) {
			EndDialog(hwnd, idCtrl);
			return TRUE;
		}
		break;
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == LVN_GETDISPINFO) {
			NMLVDISPINFO* plvdi = (NMLVDISPINFO*)lParam;
			debugger_getColumnParameters(plvdi, ((LPNMHDR)lParam)->idFrom == IDC_LOCAL_VARS ? _selectedContext->ec_identifierContext : sym_getGlobalContext());
		}
		break;
	}
	return dlg_standardDialogProcedure(hwnd, message, wParam, lParam);
}

void debugger_open(EXECUTION_CONTEXT* pContext, char* pszError) {
	_stackTopContext = pContext;
	_debugErrorMessage = pszError;
	DoDialog(DLG_DEBUG, debugger_dialogProcedure, 0, 0);
}
