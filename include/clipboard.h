/*
 * clipboard.h
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: handle the standard clipboard
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
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
extern int clp_setdata(int whichBuffer);

/*------------------------------------------------------------
 * clp_getdata()
 * get contents of the clipboard for PKS-EDIT
 */
extern int clp_getdata(void);

/*--------------------------------------------------------------------------
 * EdShowClipboard()
 */
extern int EdShowClipboard(int whichBuffer);


#define CLIPBOARD_H
#endif // ! CLIPBOARD_H
