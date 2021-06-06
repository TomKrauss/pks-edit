/*																					/*
 * desktopicons.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: PKS-EDIT Desktop Icons
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */

#ifndef DESKTOPICONS_H

typedef struct tagICONCLASS ICONCLASS;

/*------------------------------------------------------------
 * ic_closeIconWindow()
 * Close an icon window.
 */
extern int ic_closeIconWindow(HWND hwnd);

/*------------------------------------------------------------
 * ic_addIcon()
 * Adds an icon to the desktop with the given class descriptor, title and params.
 * Set x and y to CW_USEDEFAULT for a default placement.
 */
extern HWND ic_addIcon(ICONCLASS* icp, const char* szTitle, const char* szParams, int x, int y);

/*------------------------------------------------------------
 * ic_active()
 * Return the active icon with the given title, params and icon class.
 */
extern HWND ic_active(char* szTitle, char* szParams, ICONCLASS** icClass);

/*------------------------------------------------------------
 * ic_getActiveIconWindow()
 * Returns the window of the active (selected) icon and the class descriptor.
 */
extern HWND ic_getActiveIconWindow(ICONCLASS** icp);

/*------------------------------------------------------------
 * ic_changeIcon()
 * sym_assignSymbol a new title and new parameters to an icon.
 */
extern void ic_changeIcon(const char* szTitle, const char* szParams, ICONCLASS* icClass);

/*--------------------------------------------------------------------------
 * ic_enableConfigurationDialogIcons()
 */
extern void ic_enableConfigurationDialogIcons(HWND hwnd, ICONCLASS* icp);

/*------------------------------------------------------------
 * ic_init()
 * Initializes the PKS edit icons.
 */
extern int ic_init(void);

/**
 * Answer true, if the passed window refers to a desktop icon. 
 */
extern BOOL ic_isIconWindow(HWND hwnd);

/*------------------------------------------------------------
 * ic_moveIcon()
 * Move an icon to a given position. IF CW_USEDEFAULT is specified, the next free position will be used.
 */
extern int ic_moveIcon(HWND hwnd, int x, int y);

/*------------------------------------------------------------
 * ic_redisplayIcons()
 * macro_updateSelectedMacro all icons to reflect the current editor state (clipboard has data etc...)
 */
extern int _icdirty;
extern void ic_redisplayIcons(void);

/*------------------------------------------------------------
 * ic_registerDesktopIconClass()
 */
extern int ic_registerDesktopIconClass(void);

/*------------------------------------------------------------
 * ic_saveLocationInConfiguration()
 */
extern int ic_saveLocationInConfiguration();

/*------------------------------------------------------------
 * ic_fillIconTypeList()
 */
extern void ic_fillIconTypeList(HWND hwnd, int nItem, void* selValue);

/*--------------------------------------------------------------------------
 * ic_onIconTypeSelection()
 */
extern void ic_onIconTypeSelection(HWND hDlg, WORD nItem, LONG lParam, void* p);

/*------------------------------------------------------------
 * ic_measureIconTypeItem()
 */
extern void ic_measureIconTypeItem(MEASUREITEMSTRUCT* mp);

/*------------------------------------------------------------
 * ic_ownerDrawIconType()
 */
extern void ic_ownerDrawIconType(HDC hdc, RECT* rcp, void* par, int nItem, int nCtl);

#define DESKTOPICONS_H

#endif // !DESKTOPICONS_H

