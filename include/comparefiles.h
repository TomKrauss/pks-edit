/*
 * comparefiles.hs
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS 3.0
 *
 * purpose: compare the lines of two windows and navigate
 * comparison results.
 *
 * 										created: 05.10.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#ifndef COMPAREFILES_H

 /*--------------------------------------------------------------------------
  * compare_files()
  * Compare two files identified by their WINFO pointers.
  */
extern int compare_files(WINFO* wp0, WINFO* wp1);

/*
 * Clear the current comparison link.
 */
extern int compare_clear();

 /*
  * Used to navigate the differences found during a comparison. wp points to the first
  * file which must be linked to file to compare to. aDirection might be 1 or -1 to navigate
  * to the next or previous difference
  *
  * If navigation is successful, 1 is returned.
  */
extern int compare_navigate(int aDirection);

#define COMPAREFILES_H
#endif
