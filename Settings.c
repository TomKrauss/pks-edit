/*
 * Settings.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: toggle and set file editing settings
 *
 *                                               created: 
 *                                               last modified:
 *                                               author       : TOM
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "documenttypes.h"
#include "winfo.h"
#include "winterf.h"
#include "pksedit.h"
#include "dial2.h"
#include "functab.h"
#include "edfuncs.h"
#include "regexp.h"
#include "actions.h"
#include "grammar.h"

#/*---- GLOBALS ---------------*/

/* Autosave */
extern int  _recording;
extern int  _deadkey;

int doctypes_documentTypeChanged(void);
int doc_columnChanged(void);
int macro_toggleRecordMaco(void);
int bl_setColumnSelection(WINFO* wp);

static struct optiontab {
    int  flgkeynr;
    int  flag;
	OP_FLAGTYPE  op_type;
    int  (*func)();
} _optiontab[] = {
     IDD_FKFLG1,    WM_INSERT,     OP_EDIT_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG2,    WM_AUTOINDENT, OP_EDIT_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG3,    WM_AUTOWRAP,   OP_EDIT_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG4,    WM_AUTOFORMAT, OP_EDIT_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG5,    WM_SHOWMATCH,  OP_EDIT_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG6,    WM_BRINDENT,   OP_EDIT_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG7,    WM_ABBREV,     OP_EDIT_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG8,    BLK_COLUMN_SELECTION,  OP_EDIT_MODE,   doc_columnChanged,
     IDD_FKFLG9,    F_RDONLY,      OP_FILEFLAG,   doctypes_documentTypeChanged,
     IDD_FKFLG10,   WM_OEMMODE,    OP_EDIT_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG10+1, SHOWCARET_LINE_HIGHLIGHT,  OP_DISPLAY_MODE,   doctypes_documentTypeChanged,
     IDD_FKFLG10+2, 1,            OP_MACRO,   macro_toggleRecordMaco,
     0,             SHOWCONTROL,   OP_DISPLAY_MODE,   doctypes_documentTypeChanged,
     0,             SHOWSTATUS,    OP_DISPLAY_MODE,   doctypes_documentTypeChanged,
     0,             SHOWHEX,       OP_DISPLAY_MODE,   doctypes_documentTypeChanged,
     0,             SHOWRULER,     OP_DISPLAY_MODE,   doctypes_documentTypeChanged,
	 0,             SHOWLINENUMBERS,OP_DISPLAY_MODE,   doctypes_documentTypeChanged,
     -1
};

/*--------------------------------------------------------------------------
 * doctypes_documentTypeChanged()
 * One document has changed - apply the changes to the current editor and make
 * the changes persistent.
 */
int doctypes_documentTypeChanged(void) {
	WINFO  *	wp = ww_getCurrentEditorWindow();
	FTABLE *	fp;
	
	if (wp == 0) {
		return 0;
	}
	fp = wp->fp;
	ww_applyDisplayProperties(wp);
	ww_setwindowtitle(wp, NULL);
	grammar_documentTypeChanged(fp->documentDescriptor->grammar);

	SendMessage(wp->edwin_handle,WM_EDWINREORG,0,0L);

	action_commandEnablementChanged((ACTION_CHANGE_TYPE) {0,0,1,-1});
	return doctypes_saveAllDocumentTypes(fp->documentDescriptor, (char*)0);
}

/*--------------------------------------------------------------------------
 * doc_columnChanged()
 */
static int doc_columnChanged(void)
{
	WINFO *wp;
    
    if ((wp = ww_getCurrentEditorWindow()) == 0)
         return 0;

    ww_applyDisplayProperties(wp);
    bl_setColumnSelection(wp);
    return 1;
}
	
/*--------------------------------------------------------------------------
 * op_getFlagToToggle()
 */
static int *op_getFlagToToggle(OP_FLAGTYPE flagType)
{	FTABLE *fp;
	
	if (flagType != OP_MACRO && flagType != OP_OPTIONS) {
		if ((fp = ft_getCurrentDocument()) != 0) {
			EDIT_CONFIGURATION *linp = fp->documentDescriptor;
			if (flagType == OP_FILEFLAG)
				return &fp->flags;
			if (flagType == OP_EDIT_MODE)
				return &linp->workmode;
			if (flagType == OP_DISPLAY_MODE)
				return &linp->dispmode;
		}
	} else {
		return (flagType == OP_MACRO) ? &_recording : &(GetConfiguration()->options);
	}
	return (int*)0;
}

/*--------------------------------------------------------------------------
 * op_changeFlag()
 */
static void op_changeFlag(struct optiontab *op, int dofunc)
{	register state = 0;
	int *flg;
	
	if ((flg = op_getFlagToToggle(op->op_type)) != 0) {
		if (*flg & op->flag) {
			state = 1;
		}
	}
	
	if (op->flgkeynr > 0 && hwndFkeys > 0) {
		SendDlgItemMessage(hwndFkeys,op->flgkeynr,BM_SETCHECK,state,0L);
	}

	if (op->func && dofunc) {
		(*(op->func))(state);
	}
}

/*--------------------------------------------------------------------------
 * op_defineOption()
 */
BOOL op_defineOption(long nFlag)
{
	int	*pOpt;

	if ((pOpt = op_getFlagToToggle(HIWORD(nFlag))) != 0) {
		return *pOpt & LOWORD(nFlag);
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * op_toggleOption()
 */
static int op_toggleOption(struct optiontab *op)
{    int *flg;

	if ((flg = op_getFlagToToggle(op->op_type)) != 0) {
		*flg ^= op->flag;
		op_changeFlag(op,1);
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdOptionToggle()
 */
int EdOptionToggle(long par)
{	struct optiontab *op;
	int flag = LOWORD(par);
	OP_FLAGTYPE local = (OP_FLAGTYPE)(short)HIWORD(par);

	for (op = _optiontab; op->flgkeynr >= 0;  op++)
		if (op->flag == flag && op->op_type == local)
			return op_toggleOption(op);
		
	return 0;
}

/*
 * Start the recorder. 
 */
int op_startMacroRecording() {
	return EdOptionToggle(MAKELONG(1, OP_MACRO));
}

/*--------------------------------------------------------------------------
 * op_checktoggles()
 * One of the "option" widgets was selected.
 */
EXPORT int op_onOptionWidgetSelected(int toggle)
{   	struct optiontab *op;

	for (op = _optiontab; op->flgkeynr >= 0; op++) {
		if (op->flgkeynr == toggle) {
			if (op->func == macro_toggleRecordMaco) {
				/* this glitch avoids caching of the record 
				 * function itself
				 */
				EdOptionToggle(
					MAKELONG(op->flag,op->op_type));
			} else {
				return macro_executeFunction(
					FUNC_EdOptionToggle,
						MAKELONG(op->flag,op->op_type),
						0L,(LPSTR)0,(LPSTR)0, (LPSTR)0);
			}
          }
	}
	return 0;
}

/*
 * The enablement of the set edit or set display mode flags has changed. 
 */
static void op_propertyChanged(ACTION_BINDING* pBinding, PROPERTY_CHANGE_TYPE type, int newValue) {
	struct optiontab* op;

	if (type != PC_ENABLED) {
		return;
	}
	for (op = _optiontab; op->flgkeynr >= 0; op++) {
		if (op->op_type == OP_DISPLAY_MODE || op->op_type == OP_EDIT_MODE || op->op_type == OP_FILEFLAG) {
			HWND hwnd = GetDlgItem(hwndFkeys, op->flgkeynr);
			if (!newValue) {
				op_changeFlag(op, 0);
			}
			EnableWindow(hwnd, newValue);
		}
	}
	InvalidateRect(hwndFkeys, NULL, FALSE);
}

/*--------------------------------------------------------------------------
 * op_updateall()
 * The options have changed. Update all UI elements depending on an option.
 */
EXPORT void op_updateall(void)
{   struct optiontab *op;
	static BOOL actionListenerRegistered = FALSE;

	for (op = _optiontab; op->flgkeynr >= 0; op++) {
     	op_changeFlag(op,0);
	}
	if (!actionListenerRegistered) {
		actionListenerRegistered = TRUE;
		action_registerAction(CMD_EdOptionToggleBLK_COLUMN_SELECTION, (ACTION_BINDING) { op_propertyChanged , 0L, 0}, TRUE);
	}
}

