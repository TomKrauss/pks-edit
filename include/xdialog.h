/*
 * XDIALOG.H
 *
 * Project: PKS Edit for Windows
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

typedef int (*DIALOG_CB)(HWND hDialog, int nItem);

typedef struct tagLIST_HANDLER {
	void*		li_param;
	void		(*li_fill)(HWND hDlg, int item, void* param);
	int			(*li_get) (HWND hDlg, int item, void* param);
	void		(*li_measure)(MEASUREITEMSTRUCT* mp);
	void 		(*li_draw)(HDC hdc, RECT* rcp, void* par, int nItem, int nCtl);
	void		(*li_command)(HWND hDlg, int nItem, int nNotify, void* p);
	int			(*li_compare)(COMPAREITEMSTRUCT* cp);
	void		(*li_selection)(DRAWITEMSTRUCT* dp);
} LIST_HANDLER;

typedef struct tagDIALOG_ITEM_DESCRIPTOR {
	// The id of the control described by this item descriptor.
	int		did_controlNumber;
	// If dp_data points to a data area (e.g. a char* pointer in the case of a input field type dialog item), this
	// is the size of the data available.
	int 	did_flagOrSize;
	// "Initialization data" for the item. The data is copied into or from here into the control during initialization or when 
	// submitting the dialog.
	void* did_data;
	// Invoked, when the dialog is initialized
	void (*did_initialize)(HWND hDialog, struct tagDIALOG_ITEM_DESCRIPTOR* pDescriptor);
	// Invoked, before the dialog is committed. If NULL, nothing is performed.
	// If the changes cannot be applied (e.g. the values are invalid), this method should return FALSE.
	BOOL(*did_apply)(HWND hDialog, struct tagDIALOG_ITEM_DESCRIPTOR* pDescriptor);
	// Invoked, when the dialog receives a WM_COMMAND message for the described item. If
	// the values from the command should be applied before the command should use the pDialogDescriptor to do so.
	BOOL(*did_command)(HWND hDialog, int nNotify, LPARAM lParam, 
			struct tagDIALOG_ITEM_DESCRIPTOR* pDescriptor, struct tagDIALOG_DESCRIPTOR* pDialogDescriptor);
	// Special delegate for list controls.
	LIST_HANDLER* did_listhandler;
} DIALOG_ITEM_DESCRIPTOR;

/*
 * Can be used to connect a section in the PKS Edit manual with the context sensitive help for
 * one item on a dialog.
 */
typedef struct tagDIALOG_HELP_DESCRIPTOR {
	// Pointer to the PKS Edit manual in the form mydocument.md#link
	// Will be opened, when the help icon in the dialog is clicked.
	// If this is NULL - it is the last item in the list of help items.
	char* dhd_link;
	// The number of the item for which this descriptor defines the context sensitive help.
	// If this number is 0, it is the help entry for the dialog itself.
	int   dhd_itemNumber;
} DIALOG_HELP_DESCRIPTOR;

typedef struct tagITEM_TOOLTIP_MAPPING {
	int			m_itemId;
	int			m_tooltipStringId;
} DLG_ITEM_TOOLTIP_MAPPING;

typedef struct tagDIALOG_DESCRIPTOR {
	DIALOG_ITEM_DESCRIPTOR* dd_items;
	DIALOG_HELP_DESCRIPTOR* dd_helpItems;
	DLG_ITEM_TOOLTIP_MAPPING* dd_tooltips;
} DIALOG_DESCRIPTOR;

/*-----------------------------------------------
 * assigns a callback to be invoked to return the DIALOGPARS for a page (in a property sheet)
 * for that particular page, if the page is activated. The callback is passed the index of the
 * property page activated.
 */
extern void			dlg_setXDialogParams(DIALOG_DESCRIPTOR* (*func)(int pageIndex), boolean propertySheetFlag);

extern BOOL			DoDlgInitPars(HWND hDlg, DIALOG_ITEM_DESCRIPTOR *dp, int nParams);
extern int  		DoDialog(int nId, DLGPROC pDialogProc, DIALOG_DESCRIPTOR *dp);
extern void 		DlgInitString(HWND hwnd, int item, LPSTR szString, int nMax);
extern INT_PTR CALLBACK dlg_standardDialogProcedure(HWND,UINT,WPARAM,LPARAM);
/*
 * Default message handling in all PKS Edit dialogs to e.g. support theming.
 */
extern INT_PTR CALLBACK dlg_defaultWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

# if defined(_EDFUNCS_H)
extern int 			win_callDialog(int nId, RECORDED_FORM_DESCRIPTOR *pp, DIALOG_DESCRIPTOR *dp);
/*--------------------------------------------------------------------------
 * win_callDialogCB()
 * Standard dialog handling in PKS edit allowing to pass a custom dialog procedure.
 * The passed dialog procedure should invoke dlg_standardDialogProcedure for all non
 * custom dialog processing.
 */
extern int win_callDialogCB(int nId, RECORDED_FORM_DESCRIPTOR* pp, DIALOG_DESCRIPTOR* dp, DLGPROC pCallback);
# endif

/*--------------------------------------------------------------------------
 * dlg_getListboxText()
 */
extern int dlg_getComboBoxSelectedText(HWND hwnd, int id, char** szBuff);

/*--------------------------------------------------------------------------
 * dlg_getListBoxSelection()
 */
extern int dlg_getListBoxSelection(HWND hwnd, int id, void** pData);

/*--------------------------------------------------------------------------
 * win_createModelessDialog()
 */
extern void win_createModelessDialog(HWND* hwnd, LPSTR szName, INT_PTR(CALLBACK* func)(HWND, UINT, WPARAM, LPARAM),
	DLGPROC* lplpfnDlgProc);

/*
 * Used to select a font in a dialog.
 */
extern BOOL dlg_selectFontCommand(HWND hDlg, int nNotify, LPARAM lParam, DIALOG_ITEM_DESCRIPTOR* pDescriptor, DIALOG_DESCRIPTOR* pDialog);

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
 * dlg_handleRadioButtonGroup()
 * Custom handling of radio buttons: when an item is checked, make sure, that other items
 * passed in the exclusiveGroup array must be unchecked. Note, that the exclusiveGroup array
 * must be terminated by a 0 value.
 */
extern void dlg_handleRadioButtonGroup(HWND hDlg, WORD checkedItemControl, ...);

/*
 * Utility to retrieve a title text from a dialog component.
 */
extern char* dlg_getTitleResource(HWND hDlg, int idCtrl, char* szButton, size_t nSize);

/*--------------------------------------------------------------------------
 * dlg_help()
 * Shows the help system.
 */
extern void dlg_help(void);

/*--------------------------------------------------------------------------
 * Applies the changes in a dialog. idCtrl is the ID of the item leading to the confirmation of the dialog.
 * The list of item descriptors must be 0-terminated.
 */
extern BOOL dlg_applyChanges(HWND hDlg, int idCtrl, DIALOG_DESCRIPTOR* dp);


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

/*--------------------------------------------------------------------------
 * Opens a PKS Edit Help file with an optional Anchor (e.g. manual\edit_operations.md#align)
 */
extern int help_open(char* szFile);

/*
 * Property Sheet callback method for darkmode handling.
 */
intptr_t dlg_propertySheetCallback(HWND hwnd, UINT nMessage, WPARAM wParam, LPARAM lParam);

/*
 * Set to true to avoid dark dialogs (useful in cases were dark schemes cannot be supported currently). 
 */
extern BOOL dlg_disableDarkHandling;

#define	XDIALOG_H		1

# endif

