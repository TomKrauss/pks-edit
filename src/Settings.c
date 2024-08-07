/*
 * Settings.c
 *
 * Project: PKS Edit for Windows
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
#include "documentmodel.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "documenttypes.h"
#include "winfo.h"
#include "winterf.h"
#include "pksedit.h"
#include "dial2.h"
#include "funcdef.h"
#include "pksmacro.h"
#include "regexp.h"
#include "actions.h"
#include "grammar.h"

#/*---- GLOBALS ---------------*/

int doctypes_documentTypeChanged(int aFlag);
int doc_columnChanged(void);
int recorder_toggleRecording(void);
int bl_setColumnSelection(WINFO* wp);

/*
 * Answers true if we are currently recording macros
 */
extern BOOL recorder_isRecording();

/*
 * Start/stop the macro recorder.
 */
extern void recorder_setRecording(BOOL bStart);

static int _recording;

static int doctypes_changed() {
	return doctypes_documentTypeChanged(FALSE);
}

static int displaymode_changed() {
	WINFO* wp = ww_getCurrentEditorWindow();
	ww_modeChanged(wp);
	return 1;
}

static struct optiontab {
    int  flgkeynr;					//  an optional (maybe 0) dialog id of the item in the option bar.
    int  flag;						// the flag to toggle
	OP_FLAGTYPE  op_type;			// the type of flag to toggle
	int  resetFlag;					// If flag is assigned, reset this flag if != 0
	int  (*func)();					// the function to call after completion.
} _optiontab[] = {
	 IDD_FKFLG6,    SHOW_WYSIWYG_DISPLAY,   OP_DISPLAY_MODE,SHOW_HEX_DISPLAY,	displaymode_changed,
	 IDD_FKFLG1,    WM_INSERT,     OP_EDIT_MODE,   0,	doctypes_changed,
     IDD_FKFLG2,    WM_AUTOINDENT, OP_EDIT_MODE,   0,	doctypes_changed,
     IDD_FKFLG3,    WM_AUTOWRAP,   OP_EDIT_MODE,   0,	doctypes_changed,
     IDD_FKFLG4,    WM_AUTOFORMAT, OP_EDIT_MODE,   0,	doctypes_changed,
     IDD_FKFLG5,    WM_SHOWMATCH,  OP_EDIT_MODE,   0,	doctypes_changed,
     IDD_FKFLG7,    WM_ABBREV,     OP_EDIT_MODE,   0,	doctypes_changed,
     IDD_FKFLG8,    WM_COLUMN_SELECTION,  OP_EDIT_MODE,   WM_LINE_SELECTION,	doc_columnChanged,
     IDD_FKFLG9,    F_RDONLY,      OP_FILEFLAG,   0,	doctypes_changed,
	 IDD_FKFLG10,   F_WATCH_LOGFILE,OP_FILEFLAG,   0,	doctypes_changed,
	 IDD_FKFLG10+1, SHOW_CARET_LINE_HIGHLIGHT,  OP_DISPLAY_MODE,   0,	displaymode_changed,
     IDD_FKFLG10+2, 1,            OP_MACRO,   0,	recorder_toggleRecording,
	 0,				WM_FIX_CAPITALS,   OP_EDIT_MODE,   0,	doctypes_changed,
	 0,             SHOW_CONTROL_CHARS,   OP_DISPLAY_MODE,   0,	displaymode_changed,
	 0,             SHOW_CARET_PRESERVE_COLUMN,    OP_DISPLAY_MODE,   0,	displaymode_changed,
     0,             SHOW_HEX_DISPLAY,       OP_DISPLAY_MODE,   SHOW_WYSIWYG_DISPLAY,	displaymode_changed,
     0,             SHOW_RULER,     OP_DISPLAY_MODE,   0,	displaymode_changed,
	 0,             SHOW_LINENUMBERS,OP_DISPLAY_MODE,   0,	displaymode_changed,
	 0,             SHOW_SYNTAX_HIGHLIGHT,OP_DISPLAY_MODE,   0,	displaymode_changed,
	 0,             WM_LINE_SELECTION,OP_EDIT_MODE,   WM_COLUMN_SELECTION,	doctypes_changed,
	 0,				WM_OEMMODE,    OP_EDIT_MODE,   0,	doctypes_changed,
	 0,				WM_PINNED, OP_EDIT_MODE,   0,	doctypes_changed,
	 -1
};

/*--------------------------------------------------------------------------
 * doctypes_documentTypeChanged()
 * One document has changed - apply the changes to the current editor and make
 * the changes persistent.
 */
int doctypes_documentTypeChanged(int bSave) {
	WINFO  *	wp = ww_getCurrentEditorWindow();
	FTABLE *	fp;
	
	if (wp == 0) {
		return 0;
	}
	fp = wp->fp;
	ww_applyDisplayProperties(wp);
	ww_setwindowtitle(wp, TRUE);
	grammar_documentTypeChanged(fp->documentDescriptor->grammar);

	SendMessage(wp->edwin_handle,WM_EDWINREORG,0,0L);

	action_commandEnablementChanged((ACTION_CHANGE_TYPE) {0,0,1,-1});
	if (!bSave) {
		return 1;
	}
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

/*
 * Returns an int pointer to an "editor flag" so one can read or set it.
 */
int * op_getEditorFlag(WINFO* wp, OP_FLAGTYPE flagType) {
	if (wp != 0) {
		FTABLE* fp = wp->fp;
		if (fp != NULL) {
			if (flagType == OP_FILEFLAG)
				return &fp->flags;
			if (flagType == OP_EDIT_MODE)
				return &fp->documentDescriptor->workmode;
			if (flagType == OP_DISPLAY_MODE)
				return &wp->dispmode;
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * op_getFlagToToggle()
 */
static int *op_getFlagToToggle(WINFO* wp, OP_FLAGTYPE flagType)
{
	if (flagType != OP_MACRO && flagType != OP_OPTIONS) {
		return op_getEditorFlag(wp, flagType);
	} else {
		return (flagType == OP_MACRO) ? &_recording : &(GetConfiguration()->options);
	}
	return (int*)0;
}

/*--------------------------------------------------------------------------
 * op_changeFlag()
 */
static void op_changeFlag(WINFO* wp, struct optiontab *op, int dofunc)
{	int state = 0;
	int *flg;
	
	if ((flg = op_getFlagToToggle(wp, op->op_type)) != 0) {
		if (*flg & op->flag) {
			state = 1;
			if (op->resetFlag) {
				*flg &= ~op->resetFlag;
			}
		}
	}
	if (op->flgkeynr > 0 && hwndFkeys) {
		if (op->op_type == OP_DISPLAY_MODE || op->op_type == OP_EDIT_MODE) {
			BOOL bEnabled = wp != NULL;
			// If the option is currently selected - keep it enabled.
			if (bEnabled) {
				bEnabled = ww_supportsDisplayMode(wp, (EDIT_MODE) { 
					.em_displayMode = (op->op_type == OP_DISPLAY_MODE), 
					.em_flag = op->flag });
			}
			HWND hwndItem = GetDlgItem(hwndFkeys, op->flgkeynr);
			if (bEnabled != IsWindowEnabled(hwndItem)) {
				EnableWindow(hwndItem, bEnabled);
				InvalidateRect(hwndItem, NULL, FALSE);
			}
			if (!bEnabled) {
				state = FALSE;
			}
		}
		SendDlgItemMessage(hwndFkeys, op->flgkeynr, BM_SETCHECK, state, 0L);
	}

	if (op->func && dofunc) {
		(*(op->func))(state);
		if (op->func == recorder_toggleRecording) {
			_recording = recorder_isRecording();
		}
	}
}

/*--------------------------------------------------------------------------
 * op_defineOption()
 */
BOOL op_defineOption(WINFO* wp, long nFlag)
{
	int	*pOpt;

	if ((pOpt = op_getFlagToToggle(wp, HIWORD(nFlag))) != 0) {
		return *pOpt & LOWORD(nFlag);
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * op_toggleOption()
 */
static int op_toggleOption(WINFO* wp, struct optiontab *op)
{    int *flg;

	if ((flg = op_getFlagToToggle(wp, op->op_type)) != 0) {
		BOOL displayMode = op->op_type == OP_DISPLAY_MODE;
		int flag = op->flag & ~(OPT_DMODE | OPT_WMODE);
		EDIT_MODE mode = {
			.em_displayMode = displayMode,
			.em_flag = flag
		};
		if ((*flg & op->flag) == 0 && !wp->renderer->r_supportsMode(mode)) {
			return 0;
		}
		*flg ^= op->flag;
		op_changeFlag(wp, op,1);
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdOptionToggle()
 */
long long EdOptionToggle(WINFO* wp, long par)
{	struct optiontab *op;
	int flag = LOWORD(par);
	OP_FLAGTYPE local = (OP_FLAGTYPE)(short)HIWORD(par);

	for (op = _optiontab; op->flgkeynr >= 0;  op++)
		if (op->flag == flag && op->op_type == local)
			return op_toggleOption(wp, op);
		
	return 0;
}

/*
 * Change an editor option. One can change display modes or input options and add or
 * remove a flag depending on 'nSet' parameter.
 */
int op_changeEditorOption(WINFO* wp, OP_FLAGTYPE flType, int nFlag, int nSet) {
	struct optiontab* op;
	for (op = _optiontab; op->flgkeynr >= 0; op++) {
		if (op->flag == nFlag && op->op_type == flType) {
			int* flg;
			if ((flg = op_getFlagToToggle(wp, op->op_type)) != 0) {
				if (nSet) {
					*flg |= nFlag;
				}
				else {
					*flg &= ~nFlag;
				}
				op_changeFlag(wp, op, 1);
				return *flg;
			}
		}
	}
	return 0;

}

/*
 * Start the recorder. 
 */
int op_startMacroRecording() {
	return (int)EdOptionToggle(ww_getCurrentEditorWindow(), MAKELONG(1, OP_MACRO));
}

/*--------------------------------------------------------------------------
 * op_checktoggles()
 * One of the "option" widgets was selected.
 */
EXPORT int op_onOptionWidgetSelected(int toggle)
{   	struct optiontab *op;

	for (op = _optiontab; op->flgkeynr >= 0; op++) {
		if (op->flgkeynr == toggle) {
			if (op->func == recorder_toggleRecording) {
				/* this check avoids caching of the record 
				 * function itself
				 */
				EdOptionToggle(
					ww_getCurrentEditorWindow(),
					MAKELONG(op->flag,op->op_type));
				_recording = recorder_isRecording();
			} else {
				intptr_t stack[8];
				stack[0] = (intptr_t)ww_getCurrentEditorWindow();
				stack[1] = MAKELONG(op->flag, op->op_type);
				return (int)interpreter_executeFunction(
					FUNC_EdOptionToggle,
						stack);
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
	WINFO* wp = ww_getCurrentEditorWindow();
	for (op = _optiontab; op->flgkeynr >= 0; op++) {
		if (op->op_type == OP_DISPLAY_MODE || op->op_type == OP_EDIT_MODE || op->op_type == OP_FILEFLAG) {
			HWND hwnd = GetDlgItem(hwndFkeys, op->flgkeynr);
			if (!newValue) {
				op_changeFlag(wp, op, 0);
			}
		}
	}
}

/*--------------------------------------------------------------------------
 * op_updateall()
 * The options have changed. Update all UI elements depending on an option.
 */
EXPORT void op_updateall(void)
{   struct optiontab *op;
	static BOOL actionListenerRegistered = FALSE;
	WINFO* wp = ww_getCurrentEditorWindow();

	for (op = _optiontab; op->flgkeynr >= 0; op++) {
     	op_changeFlag(wp, op,0);
	}
	// This ensures, that we are informed, when the enablement / availability of the possibility to
	// change an editor flag needs to be recalculated.
	if (!actionListenerRegistered) {
		actionListenerRegistered = TRUE;
		action_registerAction(CMD_TOGGLE_COLUMN_SELECTION, CMD_CMDSEQ, (ACTION_BINDING) { op_propertyChanged , 0L, 0}, TRUE);
	}
}

