/*
 * CONTEXT.H
 *
 * Project: PKS Edit for Windows
 *
 * purpose: menu context,...
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#ifndef CONTEXT_H

typedef struct context {
	HMENU 	hMenu;
	HWND	hWindowMenu;
} EDMENUCONTEXT;

#define CTX_INIT		0
#define CTX_EDIT		1
#define CTX_HELP		2

#define CONTEXT_H
#endif
