/*
 * edexec.h
 *
 * Project: PKS Edit for Windows
 *
 * purpose: Options for executing external commands.
 *
 * 										created: 20.12.2021
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EDEXEC_H

/*
 * Options/flags for executing an external process
 * from within PKS Edit
 */
typedef enum {
EX_RDIN = 0x1,
EX_RDOUT = 0x2,
EX_DELBLK = 0x4,
EX_WAIT = 0x8,
EX_CLOSEWI = 0x10,
EX_CLEARSCR = 0x20,
EX_TEXTCURS = 0x40,
EX_SHOWMOUSE = 0x80,
/* read errorfile (like -E ... ) */
EX_READERR = 0x100,
/* dont execute, debug only */
EX_DEBUG = 0x200,
/* prompt for parameters */
EX_PROMPT = 0x400,
/* cd before exec */
EX_SETPATH = 0x800,
/* Applikation call */
EX_ACC = 0x1000,
/* dont ask, whether to save files */
EX_NOSAVE = 0x2000,
/* write GEM clipboard before exec */
EX_WCLIP = 0x4000,
/* read GEM Clipboard after exec */
EX_RCLIP = 0x8000,
EX_SYMBOL = 0x1,
EX_CD = 0x4,
EX_RDNONE = 0x10,
EX_RDCONV = 0x80
} EXEC_FLAGS;

#define	EX_NEEDSFILE	(EX_RCLIP|EX_WCLIP|EX_RDIN|EX_RDOUT|EX_DELBLK)

/*
 * combinations according to application type
 */
#define	EX_GEMFLAGS		(EX_CLOSEWI|EX_SHOWMOUSE)
#define	EX_TOSFLAGS		(EX_CLOSEWI|EX_CLEARSCR|EX_TEXTCURS)
#define	EX_TTPFLAGS		(EX_CLOSEWI|EX_CLEARSCR)
#define	EX_CONVERT		(EX_DELBLK|EX_RDIN|EX_RDOUT)
#define	EX_CONSUM		(EX_RDOUT|EX_CLOSEWI)
#define	EX_PRODUCE		EX_RDIN
#define	EX_OVERLAY		0

#define	EXE_NOTDONE		-100

#define	EDEXEC_H
#endif

