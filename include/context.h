/*
 * CONTEXT.H
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: menu context,...
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
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
