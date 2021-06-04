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
 * (c) Pahlen & Krauss
 *
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

typedef struct tagDLGSTRINGLIST {
	struct tagDLGSTRINGLIST *	next;
	LPSTR						pszString;
	void *						pAny;
	long						nSize;
} DLGSTRINGLIST;

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
extern void			SetXDialogParams(DIALPARS* (*func)(int pageIndex), boolean propertySheetFlag);

extern BOOL			DoDlgInitPars(HWND hDlg, DIALPARS *dp, int nParams);
extern int  		DoDialog(int nId, DLGPROC pDialogProc, DIALPARS *dp, DLG_ITEM_TOOLTIP_MAPPING *pTooltips);
extern void 		DlgInitString(HWND hwnd, int item, LPSTR szString, int nMax);
extern INT_PTR CALLBACK DlgStdProc(HWND,UINT,WPARAM,LPARAM);
# if defined(_EDFUNCS_H)
extern int 			CallDialog(int nId, PARAMS *pp, DIALPARS *dp, DLG_ITEM_TOOLTIP_MAPPING* pTooltips);
# endif
extern void 		DoDlgRetreivePars(HWND hDlg, DIALPARS *dp, int nMax);
extern DLGSTRINGLIST*	DoDlgSelectFromList(int nId, DLGSTRINGLIST *list, DIALLIST *dp);
/*--------------------------------------------------------------------------
 * LbGetText()
 */
extern int LbGetText(HWND hwnd, int id, void* szBuff);

/*--------------------------------------------------------------------------
 * CreateModelessDialog()
 */
extern void CreateModelessDialog(HWND* hwnd, LPSTR szName, INT_PTR(CALLBACK* func)(HWND, UINT, WPARAM, LPARAM),
	DLGPROC* lplpfnDlgProc);

#define tedinit(hDlg,item,string)	DlgInitString(hDlg,item,string,sizeof string -1)

#define	XDIALOG_H		1

# endif

