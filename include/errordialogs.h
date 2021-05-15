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
 */
extern int ed_yn(WORD nId, ...);

/*------------------------------------------------------------
 * ed_ync()
 */
extern int ed_ync(WORD nId, ...);

/*------------------------------------------------------------
 * ShowError()
 */
extern void ShowError(LPSTR fmt, ...);

/*------------------------------------------------------------
 * ed_error()
 * Display an error alert given a resource ID + optional arguments.
 */
extern void ed_error(int nId, ...);

/*------------------------------------------------------------
 * tosfnerror()
 */
extern void tosfnerror(char* fn, int fd);

#define ERRORDIALOGS_H
#endif