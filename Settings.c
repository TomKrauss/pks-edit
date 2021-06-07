/*
 * Settings.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: toggle and set file editing settings
 *
 *                                               created: 
 *                                               last modified:
 *                                               author       : TOM
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"
#include "editorconfiguration.h"

#include "winfo.h"
#include "winterf.h"

#pragma hdrstop

#include "pksedit.h"
#include "dial2.h"
#include "functab.h"
#include "edfuncs.h"
#include "regexp.h"

#/*---- GLOBALS ---------------*/

/* Autosave */
extern int  _recording;
extern int  _deadkey;

int doctypes_documentTypeChanged(void);
int doc_columnChanged(void);
int macro_toggleRecordMaco(void);
int markcolomns(FTABLE *fp);
void op_updateall(void);

static struct optiontab {
    int  flgkeynr;
    int  flag;
    int  local;
    int  (*func)();
} _optiontab[] = {
     IDD_FKFLG1,    WM_INSERT,     2,   doctypes_documentTypeChanged,
     IDD_FKFLG2,    WM_AUTOINDENT, 2,   doctypes_documentTypeChanged,
     IDD_FKFLG3,    WM_AUTOWRAP,   2,   doctypes_documentTypeChanged,
     IDD_FKFLG4,    WM_AUTOFORMAT, 2,   doctypes_documentTypeChanged,
     IDD_FKFLG5,    WM_SHOWMATCH,  2,   doctypes_documentTypeChanged,
     IDD_FKFLG6,    WM_BRINDENT,   2,   doctypes_documentTypeChanged,
     IDD_FKFLG7,    WM_ABBREV,     2,   doctypes_documentTypeChanged,
     IDD_FKFLG8,    BLK_COLOMN,    2,   doc_columnChanged,
     IDD_FKFLG9,    O_RDONLY,      2,   doctypes_documentTypeChanged,
     IDD_FKFLG10,   WM_OEMMODE,    2,   doctypes_documentTypeChanged,
     IDD_FKFLG10+1, SHOWOEM,       1,   doctypes_documentTypeChanged,
     IDD_FKFLG10+2, 1,            -1,   macro_toggleRecordMaco,
     0,             SHOWCONTROL,   1,   doctypes_documentTypeChanged,
     0,             SHOWSTATUS,    1,   doctypes_documentTypeChanged,
     0,             SHOWHEX,       1,   doctypes_documentTypeChanged,
     0,             SHOWRULER,     1,   doctypes_documentTypeChanged,
     0,             SHOWATTR,      1,   doctypes_documentTypeChanged,
     -1
};

/*--------------------------------------------------------------------------
 * doctypes_documentTypeChanged()
 * One document has changed - apply the changes to the current editor.
 */
int doctypes_documentTypeChanged(void)
{
	WINFO  *	wp;
	FTABLE *	fp;
	
	if ((fp = ft_getCurrentDocument()) == 0 ||
	   (wp = WIPOI(fp)) == 0)
	    return 0;
	
	ww_applyDisplayProperties(wp);
	ww_setwindowtitle(wp);
	regex_compileCharacterClasses(fp->documentDescriptor->u2lset);

	SendMessage(wp->edwin_handle,WM_EDWINREORG,0,0L);

	op_updateall();
	return 1;
}

/*--------------------------------------------------------------------------
 * doc_columnChanged()
 */
static int doc_columnChanged(void)
{
	FTABLE *fp;
    
    if ((fp = ft_getCurrentDocument()) == 0)
         return 0;

    ww_applyDisplayProperties(WIPOI(fp));
    markcolomns(fp);
    return 1;
}
	
/*--------------------------------------------------------------------------
 * OpLocalAdr()
 */
static int *OpLocalAdr(int local)
{	FTABLE *fp;
	
	if (local > 0) {
		if ((fp = ft_getCurrentDocument()) != 0) {
			DOCUMENT_DESCRIPTOR *linp = fp->documentDescriptor;
			
			if (local == 2)
				return &linp->workmode;
			if (local == 1)
				return &linp->dispmode;
		}
	}
	else {
		return (local == -1) ? &_recording : &(GetConfiguration()->options);
	}
	return (int*)0;
}

/*--------------------------------------------------------------------------
 * ChangeFlag()
 */
static void ChangeFlag(struct optiontab *op,int dofunc)
{	register state = 0;
	int *flg;
	
	if ((flg = OpLocalAdr(op->local)) != 0) {
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

	if ((pOpt = OpLocalAdr(HIWORD(nFlag))) != 0) {
		return *pOpt & LOWORD(nFlag);
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * ToggleOption()
 */
static int ToggleOption(struct optiontab *op)
{    int *flg;

	if ((flg = OpLocalAdr(op->local)) != 0) {
		*flg ^= op->flag;
		ChangeFlag(op,1);
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
	int local = (int)(short)HIWORD(par);

	for (op = _optiontab; op->flgkeynr >= 0;  op++)
		if (op->flag == flag && op->local == local)
			return ToggleOption(op);
		
	return 0;
}

/*--------------------------------------------------------------------------
 * op_checktoggles()
 */
EXPORT int op_checktoggle(int toggle)
{   	struct optiontab *op;

	for (op = _optiontab; op->flgkeynr >= 0; op++) {
		if (op->flgkeynr == toggle) {
			if (op->func == macro_toggleRecordMaco) {
				/* this glitch avoids caching of the record 
				 * function itself
				 */
				EdOptionToggle(
					MAKELONG(op->flag,op->local));
			} else {
				return macro_executeFunction(
					FUNC_EdOptionToggle,
						MAKELONG(op->flag,op->local),
						0L,(LPSTR)0,(LPSTR)0, (LPSTR)0);
			}
          }
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * op_updateall()
 */
EXPORT void op_updateall(void)
{   struct optiontab *op;

	for (op = _optiontab; op->flgkeynr >= 0; op++) {
     	ChangeFlag(op,0);
	}
}

