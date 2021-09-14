/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * mainframe.h
 *
 * Main Window and of PKS Edit
 *
 * 						created: 10.09.21
 *						Author : Tom
 */


#ifndef MAINFRAME_H

/*
 * To be invoked, when the window of a child window changes. 
 */
extern void mainframe_windowTitleChanged();

 /*
  * Register the main wind
  */
extern int mainframe_registerWinClass();

/*
 * Open the Mainframe window.
 */
extern HWND mainframe_open(int nInstanceCount, HMENU hDefaultMenu);

/*
 * Add a window with the given window class to our tab control managing the edit windows.
 * pszPreferredSlot is the name of a docking position to which the window should be added (if the corresponding
 * docking position exists). Pass NULL to use the default edit slot.
 */
extern HWND mainframe_addWindow(const char* pszPreferredSlot, const char* pszChildWindowClass, const char* pszTitle, LPVOID lParam);

/*
 * Checks, whether there is an active PKS edit window already somewhere on the screen (maybe other process).
 */
extern BOOL mainframe_findExistingWindow();

/*
 * Process the main messages in the mainframe window (in particular keyboard messages etc...).
 */
int mainframe_messageLoop();

/*-----------------------------------------------------------
 * mainframe_enumChildWindows()
 * Execute a function for every child window optionally
 * passing an lParam. If the called function returns 0, the enumeration
 * stops. If bHideTabsDuringEnum is TRUE, the container displaying the enumerated
 * windows is temporarily hidden while executing the function.
 */
int mainframe_enumChildWindows(BOOL bHideTabsDuringEnum, int (*funcp)(), LONG lParam);

typedef enum { MD_ADD_HORIZONTAL, MD_ADD_VERTICAL } MANAGE_DOCKS_TYPE;

#define DOCK_NAME_RIGHT				"rightSlot"
#define DOCK_NAME_BOTTOM			"bottomSlot"

/*
 * Add new docks to the mainframe.
 */
extern int mainframe_manageDocks(MANAGE_DOCKS_TYPE mType);

/*
 * Returns the name of the docking slot in which the passed editor window is docked.
 */
extern char* mainframe_getDockNameFor(HWND hwnd);

#define MAINFRAME_H
#endif
