/*
 * FormatLines.c
 *
 * format lines - 4 justifikation types
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 14.03.1991
 */

#include <windows.h>
#include <string.h>

#include "alloc.h"
#include "caretmovement.h"
#include "edierror.h"
#include "textblocks.h"

#include "pksedit.h"
#include "mouseutil.h"
#include "winfo.h"

extern int edit_autoFormatRange(WINFO* wp, int nRange);

/*---------------------------------
 * ft_formatText()	
 * Formt the text in the current file.
 *---------------------------------*/
int ft_formatText(WINFO* wp, int scope, int type, int flags) {

	return edit_autoFormatRange(wp, scope);
}

