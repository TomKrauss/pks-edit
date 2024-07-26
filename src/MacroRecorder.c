/*
 * MacroRecorder.c
 *
 * record macros in PKS Edit
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * author: Tom
 * created: 13.03.1991
 */

#include <stdio.h>
#include <string.h>

#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "pksedit.h"
#include "documentmodel.h"
#include "markpositions.h"
#include "actionbindings.h"
#include "xdialog.h"
#include "errordialogs.h"
#include "funcdef.h"

/*
 * Start the recorder.
 */
extern int op_startMacroRecording();

static char _recorderByteCodes[RECORDERSPACE];
static char* _cmdfuncp;
static BYTECODE_BUFFER	_currentRecordingBuffer;
typedef struct ccash {
	unsigned char low;
	COM_1FUNC	    b[200];
} CCASH;

static CCASH			_autoInsertCache;


int				_lastinsertedmac = -1;

/*------------------------------------------------------------
 * EdMacroRecord()
 */
int EdMacroRecord(void)
{
	return op_startMacroRecording();
}

/*
 * Answers true if we are currently recording macros
 */
BOOL recorder_isRecording() {
	return _currentRecordingBuffer.bb_current != NULL;
}

/*
 * Start/stop the macro recorder. 
 */
void recorder_setRecording(BOOL bStart) {
	if (bStart) {
		_currentRecordingBuffer.bb_current = _currentRecordingBuffer.bb_start = _recorderByteCodes;
		_currentRecordingBuffer.bb_end = &_recorderByteCodes[RECORDERSPACE - 12];
	} else {
		_currentRecordingBuffer.bb_current = NULL;
	}
}

/*--------------------------------------------------------------------------
 * recorder_pushSequence()
 *
 * this is called, when recording
 * we do not need, to handle all command types, cause in ineractive
 * mode, delta like test or binop are not possible
 */
void recorder_pushSequence(unsigned char typ, void* par) {
	char* spend;
	char* sp;
	int  s;

	if (!recorder_isRecording()) {
		return;
	}

	sp = _currentRecordingBuffer.bb_current;
	s = interpreter_getParameterSize(typ, (char*)par);

	if ((spend = sp + s) > _currentRecordingBuffer.bb_end) {
		error_showErrorById(IDS_MSGMACOVERFLOW);
		EdMacroRecord();			/* STOP the recording machine */
		return;
	}

	switch (typ) {
	case C_0FUNC:
	case C_PUSH_SMALL_INT_LITERAL:
	case C_PUSH_CHARACTER_LITERAL:
		((COM_CHAR1*)sp)->val = (unsigned char)par;
		break;
	case C_FORM_START:
		*(COM_FORM*)sp = *(COM_FORM*)par;
		break;
	case C_1FUNC:
		*(COM_1FUNC*)sp = *(COM_1FUNC*)par;
		break;
	case C_PUSH_INTEGER_LITERAL:
		((COM_INT1*)sp)->val = (int)(intptr_t)par;
		break;
	case C_PUSH_LONG_LITERAL:
		((COM_LONG1*)sp)->val = (long)(intptr_t)par;
		break;
	case C_PUSH_FLOAT_LITERAL:
		((COM_FLOAT1*)sp)->val = (double)(intptr_t)par;
		break;
	case C_PUSH_STRING_LITERAL:
		strcpy(((COM_STRING1*)sp)->s, (char*)par);
		break;
	case C_MACRO:
		strcpy(((COM_MAC*)sp)->name, (char*)par);
		break;
	}

	if (C_ISCMD(typ))
		_cmdfuncp = sp;

	*sp = typ;
	*spend = C_STOP;
	_currentRecordingBuffer.bb_current = spend;
}

/*---------------------------------*/
/* recorder_recordFunctionWithParameters()						*/
/*---------------------------------*/
void recorder_recordFunctionWithParameters(int fnum, intptr_t p, intptr_t p2, char* s1, char* s2) {
	COM_1FUNC c;

	if (!recorder_isRecording())
		return;

	if (_functionTable[fnum].nf_flags & EW_NOCASH)	/* avoid recursion	*/
		return;

	BOOL bEditorFunc = function_getParameterTypeDescriptor(&_functionTable[fnum], 1).pt_type == PARAM_TYPE_EDITOR_WINDOW;
	if (bEditorFunc) {
		p = (intptr_t)p2;
		p2 = (intptr_t) s1;
		s1 = s2;
		s2 = 0;
	}
	c = (COM_1FUNC) {
		.funcnum = fnum,
		.p = (long)p
	};

	recorder_pushSequence(C_1FUNC, (void*)&c);
	if (p2) recorder_pushSequence(C_PUSH_LONG_LITERAL, (void*)p2);
	if (s1) {
		recorder_pushSequence(C_PUSH_STRING_LITERAL, (void*)s1);
		if (s2)
			recorder_pushSequence(C_PUSH_STRING_LITERAL, (void*)s2);
	}
}

/*---------------------------------*/
/* recorder_recordOperation()				*/
/*---------------------------------*/
int recorder_recordOperation(RECORDED_FORM_DESCRIPTOR* pp)
{
	int 		i, opt;
	RECORDED_FORM_ITEM_DESCRIPTOR* dp;
	char* savepos;
	COM_FORM	cf;

	if (!recorder_isRecording())
		return 0;

	savepos = _currentRecordingBuffer.bb_current - interpreter_getParameterSize(C_1FUNC, (char*)0);
	opt = 0;

	if (pp->flags & P_MAYOPEN) {
		if (dlg_displayRecordMacroOptions(&opt) == 0) {
			/* dialog is cancelled */
			_currentRecordingBuffer.bb_current = savepos;
			return 0;
		}
	}

	cf.options = opt;
	cf.nfields = pp->nel;
	/*
	 * slight optimization: if form should be opened uninitialized,
	 * dont EdMacroRecord the parameters
	 */
	if ((opt & FORM_INIT) == 0) {
		cf.nfields = 0;
	}

	recorder_pushSequence(C_FORM_START, &cf);

	dp = pp->el;
	for (i = cf.nfields; i > 0; i--, dp++) {
		switch (dp->cmd_type) {
		case C_PUSH_INTEGER_LITERAL:
			recorder_pushSequence(C_PUSH_INTEGER_LITERAL, (void*)((intptr_t)*dp->p.i));
			break;
		case C_PUSH_SMALL_INT_LITERAL:
			recorder_pushSequence(C_PUSH_SMALL_INT_LITERAL, (void*)((intptr_t)*dp->p.c));
			break;
		case C_PUSH_CHARACTER_LITERAL:
			recorder_pushSequence(C_PUSH_CHARACTER_LITERAL, (void*)((intptr_t)*dp->p.c));
			break;
		case C_PUSH_FLOAT_LITERAL:
			recorder_pushSequence(C_PUSH_FLOAT_LITERAL, (void*)((intptr_t)*dp->p.d));
			break;
		case C_PUSH_LONG_LITERAL:
			recorder_pushSequence(C_PUSH_LONG_LITERAL, (void*)((intptr_t)*dp->p.l));
			break;
		case C_PUSH_STRING_LITERAL:
			recorder_pushSequence(C_PUSH_STRING_LITERAL, (void*)dp->p.s);
			break;
		}
	}

	return 1;
}

/*------------------------------------------------------------
 * recorder_toggleRecording()
 * start/stops the macro recorder.
 */
int recorder_toggleRecording(void) {
	static KEYCODE scan = K_DELETED;
	char    buf[MAC_COMMENTLEN];

	if (recorder_isRecording()) {		// STOP RECORDING
		if (_cmdfuncp && _currentRecordingBuffer.bb_current !=_currentRecordingBuffer.bb_start) {
			static int _macroIndexRecorded = 1;
			while(TRUE) {
				sprintf(buf, "RecordedMacro_%d", _macroIndexRecorded);
				if (macro_getInternalIndexByName(buf) >= 0) {
					_macroIndexRecorded++;
				} else {
					break;
				}
			}
			if (!macro_getIndexForKeycode(&scan, buf, -1)) {
				recorder_setRecording(FALSE);
				return 0;
			}
			if (!scan) {
				scan = K_DELETED;
			}
			MACRO_PARAM pParam = { 
					.mp_name = buf, 
					// +1 - to accomodate for C_STOP byte code
					.mp_bytecodeLength = _currentRecordingBuffer.bb_current-_currentRecordingBuffer.bb_start+1, 
					.mp_buffer = _currentRecordingBuffer.bb_start };
			if ((_lastinsertedmac = macro_insertNewMacro(&pParam)) >= 0) {
				_macroIndexRecorded++;
				bindings_bindKey(scan, (MACROREF) { .index = _lastinsertedmac, .typ = CMD_MACRO }, NULL);
			}
		}
		recorder_setRecording(FALSE);
	}  else {	// START RECORDING
		recorder_setRecording(TRUE);;
	}
	return 1;
}

/*---------------------------------*/
/* recorder_stopAutoInsertRecording()  */
/*---------------------------------*/
void recorder_stopAutoInsertRecording(void **pRecordBufferLow, void **pRecordBufferHigh) {
	_autoInsertCache.low = 0;
	if (pRecordBufferLow) {
		*pRecordBufferLow = (COM_1FUNC*)_autoInsertCache.b;
		*pRecordBufferHigh = (COM_1FUNC*)(&_autoInsertCache.b[DIM(_autoInsertCache.b)]);
	}
}

/*---------------------------------*/
/* recorder_recordAutoInsertFunction()					*/
/*---------------------------------*/
void recorder_recordAutoInsertFunction(FTABLE* fp, int p) {

	if (_autoInsertCache.low < DIM(_autoInsertCache.b) - 2) {
		COM_1FUNC* cp;
		if (!_autoInsertCache.low) {
			fm_savepos(MTE_AUTO_LAST_INSERT);
			undo_startModification(fp);
		}
		cp = &_autoInsertCache.b[_autoInsertCache.low++];
		cp->typ = C_1FUNC;
		cp->funcnum = FUNC_EdCharInsert;
		cp->p = p;
		/*
		 * stop mark
		 */
		cp[1].typ = C_STOP;
	}
}



