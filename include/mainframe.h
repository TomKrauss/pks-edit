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
 */
extern HWND mainframe_addWindow(const char* pszChildWindowClass, const char* pszTitle, LPVOID lParam);

/*
 * Checks, whether there is an active PKS edit window already somewhere on the screen (maybe other process).
 */
extern BOOL mainframe_findExistingWindow();

/*
 * Process the main messages in the mainframe window (in particular keyboard messages etc...).
 */
int mainframe_messageLoop();

#define MAINFRAME_H
#endif
