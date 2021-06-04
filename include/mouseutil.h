/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * mouseutil.h
 *
 * operations regarding mouse interaction with the user.
 *
 * 						created: 07.06.91
 *						author: Tom
 */

#ifndef MOUSEUTIL_H

typedef void* HANDLE;

/*------------------------------------------------------------
 * mouse_getXYPos()
 * Get the current window relative mouse position.
 */
extern void mouse_getXYPos(HANDLE hwnd, int* x, int* y);

/*------------------------------------------------------------
 * mouse_dispatchUntilButtonRelease()
 * Dispatches mouse messages until a mouse button is released.
 * Return the current mouse position / shift state etc... in the passed
 * parameters.
 */
extern void mouse_dispatchUntilButtonRelease(int* x, int* y, int* but, int* shift);

/*------------------------------------------------------------
 * mouse_setBusyCursor()
 * Display the hourglass cursor.
 */
extern void mouse_setBusyCursor(void);

/*------------------------------------------------------------
 * mouse_setDefaultCursor()
 * Display the default cursor (arrow)
 */
extern void mouse_setDefaultCursor(void);


#define MOUSEUTIL_H
#endif // !MOUSEUTIL_H
