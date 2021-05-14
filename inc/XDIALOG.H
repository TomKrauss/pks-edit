/*
 * XDIALOG.H
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: extended dialog functions
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

# ifndef XDIALOG_H

typedef struct tagDIALPARS {
	int		dp_item;
	int 		dp_size;
	void		*dp_data;
} DIALPARS;

typedef struct tagDIALLIST {
	LONG		*li_param;
	void		(*li_fill)(HWND hDlg, int item, LONG param);
	int			(*li_get) (HWND hDlg, int item, LONG *param);
	void		(*li_measure)(MEASUREITEMSTRUCT *mp);
	void 		(*li_draw)(HDC hdc, RECT *rcp, DWORD par, int nItem, int nCtl);
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

typedef DIALPARS *LPDIALPARS;

void 			SetXDialogParams(DIALPARS *dp);
BOOL			DoDlgInitPars(HWND hDlg, DIALPARS *dp, int nParams);
int  			DoDialog(int nId, FARPROC DlgWndProc, DIALPARS *dp);
void 			DlgInitString(HWND hwnd, int item, LPSTR szString, int nMax);
BOOL CALLBACK	DlgStdProc(HWND,UINT,WPARAM,LPARAM);
# if defined(_EDFUNCS_H)
int 				CallDialog(int nId, PARAMS *pp, DIALPARS *dp);
# endif
void 			DoDlgRetreivePars(HWND hDlg, DIALPARS *dp, int nMax);
DLGSTRINGLIST *	DoDlgSelectFromList(int nId, DLGSTRINGLIST *list, DIALLIST *dp);

#define tedinit(hDlg,item,string)	DlgInitString(hDlg,item,string,sizeof string -1)

# define	XDIALOG_H		1

# endif

