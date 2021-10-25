/*
 * clipboard.h
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: handle the standard clipboard
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef  CLIPBOARD_H

/*------------------------------------------------------------
 * clp_ismine()
 * check, whether we are clipboard owner
 */
extern int clp_ismine(void);

/*------------------------------------------------------------
 * clp_setmine()
 * we are clipboard owner !
 */
extern void clp_setmine(void);

/*------------------------------------------------------------
 * clp_setdata()
 * deliver contents of the clipboard to a caller
 * is called after a RENDER... message
 */
extern int clp_setdata(char* pszBufferName);

/*------------------------------------------------------------
 * clp_getdata()
 * get contents of the clipboard for PKS-EDIT
 */
extern int clp_getdata(void);

/*--------------------------------------------------------------------------
 * EdShowClipboard()
 */
extern int EdShowClipboard(char* pszBuffer);


#define CLIPBOARD_H
#endif // ! CLIPBOARD_H

