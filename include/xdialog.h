/*
 * XDIALOG.H
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: extended dialog functions
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

# ifndef XDIALOG_H

typedef struct tagDIALPARS {
	int		dp_item;
	int 	dp_size;
	void	*dp_data;
} DIALPARS;

typedef struct tagDIALLIST {
	LONG		*li_param;
	void		(*li_fill)(HWND hDlg, int item, void *param);
	int			(*li_get) (HWND hDlg, int item, void *param);
	void		(*li_measure)(MEASUREITEMSTRUCT *mp);
	void 		(*li_draw)(HDC hdc, RECT *rcp, void* par, int nItem, int nCtl);
	void		(*li_command)(HWND hDlg, int nItem, int nNotify, void *p);
	int			(*li_compare)(COMPAREITEMSTRUCT *cp);
	void		(*li_selection)(DRAWITEMSTRUCT *dp);
} DIALLIST;

typedef struct tagITEM_TOOLTIP_MAPPING {
	int			m_itemId;
	int			m_tooltipStringId;
} DLG_ITEM_TOOLTIP_MAPPING;

typedef DIALPARS *LPDIALPARS;

/*-----------------------------------------------
 * sym_assignSymbol a callback to be invoked to return the DIALOGPARS for a page (in a property sheet)
 * for that particular page, if the page is activated. The callback is passed the index of the
 * property page activated.
 */
extern void			dlg_setXDialogParams(DIALPARS* (*func)(int pageIndex), boolean propertySheetFlag);

extern BOOL			DoDlgInitPars(HWND hDlg, DIALPARS *dp, int nParams);
extern int  		DoDialog(int nId, DLGPROC pDialogProc, DIALPARS *dp, DLG_ITEM_TOOLTIP_MAPPING *pTooltips);
extern void 		DlgInitString(HWND hwnd, int item, LPSTR szString, int nMax);
extern INT_PTR CALLBACK dlg_standardDialogProcedure(HWND,UINT,WPARAM,LPARAM);
# if defined(_EDFUNCS_H)
extern int 			win_callDialog(int nId, PARAMS *pp, DIALPARS *dp, DLG_ITEM_TOOLTIP_MAPPING* pTooltips);
# endif
extern void 		DoDlgRetreivePars(HWND hDlg, DIALPARS *dp, int nMax);
/*--------------------------------------------------------------------------
 * dlg_getListboxText()
 */
extern int dlg_getListboxText(HWND hwnd, int id, void* szBuff);

/*--------------------------------------------------------------------------
 * win_createModelessDialog()
 */
extern void win_createModelessDialog(HWND* hwnd, LPSTR szName, INT_PTR(CALLBACK* func)(HWND, UINT, WPARAM, LPARAM),
	DLGPROC* lplpfnDlgProc);

/*--------------------------------------------------------------------------
 * dlg_queryReplace()
 * Open a modeless dialog to query for replacing a text.
 */
extern int dlg_queryReplace(char* search, int slen, char* replace, int dlen);

/*--------------------------------------------------------------------------
 * dlg_displayRecordMacroOptions()
 * Open a window to start recording macro. Returns 1, if the action to record the
 * macro should really start.
 */
extern int dlg_displayRecordMacroOptions(int* o);

/*--------------------------------------------------------------------------
 * dlg_help()
 * Shows the help system.
 */
extern void dlg_help(void);

/*--------------------------------------------------------------------------
 * dlg_displayDialogTemplate()
 */
extern int dlg_displayDialogTemplate(unsigned char c, char* (*fpTextForTmplate)(char* s), char* s);

/*--------------------------------------------------------------------------
 * dlg_closeQueryReplace()
 * Closes the query replace dialog.
 */
extern void dlg_closeQueryReplace(void);

/*
 * Return a string resource for the current language. Note, that this method is not
 * re-entrant. Any text loaded must be used right away.
 */
extern char* dlg_getResourceString(int nId);

/*------------------------------------------------------------
 * dlg_drawFileInfo()
 * Draw the info about a file with a given index. Paints the index (used to select the file),
 * the name of the file a modification marker and the icon according to the file type.
 */
#if defined(EDITAB_H)
extern void dlg_drawFileInfo(HDC hdc, RECT* rcp, HWND hwnd, int nItem, BOOL bSelected);
#endif

/*--------------------------------------------------------------------------
 * EdPromptForCharacter()
 * Prompt for a character. The prompt message is defined via resource ID passed
 * as a parameter.
 */
extern int EdPromptForCharacter(int ids_num);

/*--------------------------------------------------------------------------
 * EdHelpContext()
 */
extern int EdHelpContext(DWORD nCtx);

#define tedinit(hDlg,item,string)	DlgInitString(hDlg,item,string,sizeof string -1)

#define	XDIALOG_H		1

# endif

