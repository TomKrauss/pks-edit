/*
 * ErrorDialogs.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: error alert message boxes and other
 * type of user notification in case of errors.
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#ifndef ERRORDIALOGS_H

#include <windows.h>
 
/*------------------------------------------------------------
 * alert()
 */
extern void alert(LPSTR fmt, ...);

/*------------------------------------------------------------
 * ed_yn()
 * Confirmation message box with yes no.
 */
extern int ed_yn(WORD nId, ...);

/*------------------------------------------------------------
 * ed_ync()
 * Confirmation message box with yes no cancel.
 */
extern int ed_ync(WORD nId, ...);

/*------------------------------------------------------------
 * ShowError()
 * Show an error message passing the string format and optional arguments
 * to be formatted with the string format.
 */
extern void ShowError(LPSTR fmt, va_list ap);

/*------------------------------------------------------------
 * ShowMessage()
 * Show an info or error message - primarily in the status bar of PKS Edit.
 * Pass the resource ID and optional args.
 */
extern void ShowMessage(WORD resourceID, ...);

/*------------------------------------------------------------
 * ed_error()
 * Display an error alert given a resource ID + optional arguments.
 */
extern void ed_error(int nId, ...);

/*------------------------------------------------------------
 * tosfnerror()
 */
extern void tosfnerror(char* fn, int fd);

extern void alert(LPSTR fmt, ...);

#define ERRORDIALOGS_H
#endif