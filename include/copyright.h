/*
 * copyright.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: manage copyright templates.
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef COPYRIGHT_H
#define COPYRIGHT_H

 /*
  * Load the copyright profiles from the PKS_SYS directory.
  */
extern int copyright_loadProfiles();

/*
 * Destroy all loaded copyright profiles.
 */
extern void copyright_destroyProfiles();

#ifdef STRINGUTIL_H
/*
 * Return the copyright enclosed in comments as defined by the grammar passed. The STRING_BUF returned must be freed, if it is not
 * needed any more. If no copyright profiles are defined, or no comment formatting is defined for the passed grammar, NULL is
 * returned.
 */
extern STRING_BUF* copyright_getFormatted(EDIT_CONFIGURATION* pConfiguration);
#endif

#endif