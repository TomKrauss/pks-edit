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
  * A hint to be passed to open window to define, how the window is displayed in the mainframe.
  */
typedef struct tagOPEN_HINT {
	const char* oh_slotName;		// the name of the slot, where the window ist placed
	BOOL		oh_activate;		// whether the new window should be activated on the tab where it is being placed.
	BOOL		oh_focus;			// whether the new window should be the one, which is finally focused
	BOOL		oh_clone;			// whether this is a cloned (secondary) window
	int			oh_displayMode;		// the display mode of the window
} OPEN_HINT;

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
extern HWND mainframe_open(HMENU hDefaultMenu);

/*
 * Add a window with the given window class to our tab control managing the edit windows.
 * pszPreferredSlot is the name of a docking position to which the window should be added (if the corresponding
 * docking position exists). Pass NULL to use the default edit slot.
 */
extern HWND mainframe_addWindow(OPEN_HINT* pHint, const char* pszChildWindowClass, const char* pszTitle, LPVOID lParam);

/*
 * Returns a default icon to display for editors not having an own icon defined by the Windows file type.
 */
extern HICON mainframe_getDefaultEditorIcon();

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

/*
 * Operations to manage the PKS-Edit editor docks available
 */
typedef enum {
	// Add / ensure, that a horizontal split exists
	MD_ADD_HORIZONTAL, 
	// Add / ensure, that a vertical split exists
	MD_ADD_VERTICAL,
	// Ensure, that the main docking area is visible
	MD_ENSURE_DEFAULT
} MANAGE_DOCKS_TYPE;

#define DOCK_NAME_RIGHT				"rightSlot"
#define DOCK_NAME_BOTTOM			"bottomSlot"
#define DOCK_NAME_DEFAULT			"default"

/*
 * Add new docks to the mainframe.
 */
extern long long mainframe_manageDocks(MANAGE_DOCKS_TYPE mType);

/*
 * Returns a string to be used as an open hint later, when opening the window. Use the edwin_handle window of the window to test.
 */
extern char* mainframe_getOpenHint(HWND hwnd, BOOL bFocus, BOOL bClone, int bDisplayMode);

/*
 * Invoked, when a new editor is activated. Used to mark the current active window.
 */
extern void mainframe_windowActivated(HWND hwndOld, HWND hwndNew);

/*
 * Move a mainframe window to a preferred docking slot.
 */
extern void mainframe_moveWindowAndActivate(HWND hwndEdit, const char* pszPreferredSlot, BOOL bActivate);

/*
 * Parse an open hint text. Note, that the hint text is modified during parsing.
 */
extern OPEN_HINT mainframe_parseOpenHint(char* pszHint);

/*
 * Used to switch to full screen mode and back.
 */
extern long long mainframe_toggleFullScreen();

/*
 * Returns the window dock name for a given window. Use the edwin_handle window of the window to test.
 */
extern char* mainframe_getDockName(HWND hwnd);

#define MAINFRAME_H
#endif
