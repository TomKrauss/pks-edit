/*
 * HandleMacros.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: common functions for macro handling
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

# if defined( _Windows ) || defined(WIN32)
#include <windows.h>
#include "winterf.h"
#define sprintf wsprintf
# else
#include <stdio.h>
#define	hInst		0
# endif

#include "edfuncs.h"

char *c2shift(char *s, KEYCODE code);

extern void yyerror(char* s, ...);

/*------------------------------------------------------------
 * mac_name()
 */
char *mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type)
{
	switch(type) {
		case CMD_MACRO:  sprintf(szBuf,"%s",macro_getByIndex(nIndex)->name); break;
		case CMD_CMDSEQ: 
			if (nIndex < 0 || nIndex >= _ncmdseq) {
				sprintf(szBuf,"@Unnamed-%d",nIndex);
			} else {
				strcpy(szBuf + 1, _cmdseqtab[nIndex].c_name);
				szBuf[0] = '@';
			}
			break;
		default: sprintf(szBuf,"??"); break;
	}
	return szBuf;
}

char *_scantab=
"\x03" "CANCEL\0" "\x08" "BACK\0" "\x09" "TAB\0" "\x0C" "CLEAR\0" "\x0D" "RETURN\0"
"\x13" "PAUSE\0" "\x14" "CAPITAL\0" "\x1B" "ESCAPE\0" "\x20" "SPACE\0" "\x21" "PRIOR\0"
"\x22" "NEXT\0" "\x23" "END\0" "\x24" "HOME\0" "\x25" "LEFT\0" "\x26" "UP\0"
"\x27" "RIGHT\0" "\x28" "DOWN\0" "\x29" "SELECT\0" "\x2A" "PRINT\0" "\x2B" "EXECUTE\0"
"\x2C" "SNAPSHOT\0" "\x2D" "INSERT\0" "\x2E" "DELETE\0" "\x2F" "HELP\0" "\x60" "NUMPAD0\0"
"\x61" "NUMPAD1\0" "\x62" "NUMPAD2\0" "\x63" "NUMPAD3\0" "\x64" "NUMPAD4\0" "\x65" "NUMPAD5\0"
"\x66" "NUMPAD6\0" "\x67" "NUMPAD7\0" "\x68" "NUMPAD8\0" "\x69" "NUMPAD9\0" "\x6A" "MULTIPLY\0"
"\x6B" "ADD\0" "\x6C" "SEPARATOR\0" "\x6D" "SUBTRACT\0" "\x6E" "DECIMAL\0" "\x6F" "DIVIDE\0"
"\x70" "F1\0" "\x71" "F2\0" "\x72" "F3\0" "\x73" "F4\0" "\x74" "F5\0" "\x75" "F6\0"
"\x76" "F7\0" "\x77" "F8\0" "\x78" "F9\0" "\x79" "F10\0" "\x7A" "F11\0" "\x7B" "F12\0"
"\x7C" "F13\0" "\x7D" "F14\0" "\x7E" "F15\0" "\x7F" "F16\0" "\x90" "NUMLOCK\0\0";

char *code2key(KEYCODE code)
{
	char        *s;
	unsigned char *t;
	static char b[32];

	if (code == K_DELETED)
		return "*NO KEY*";

	s = c2shift(b,code);

	code &= 0xFF;

	if ((code >= '0' && code <= '9') ||
	    (code >= 'A' && code <= 'Z')) {
		*s++ = (char)code;
		*s = 0;
		return b;
	}
		
	for (t = _scantab; *t; ) {
		if (code == *t) {
			t++;
			while((*s++ = *t++) != 0)
				;
			return b;
		}
		while(*t++)
			;
	}

	sprintf(s,"\\%03d",code);
	return b;
}

char *code2mouse(MOUSECODE code)
{
	static char 	b[32];
	char			*s;

	s = c2shift(b,code.shift<<8);
	if (code.button & MBUT_L) {
		strcpy(s,"Left+");
		s += 5;
	}
	if (code.button & MBUT_R) {
		strcpy(s,"Right+");
		s += 6;
	}
	if (code.button & MBUT_M) {
		strcpy(s,"Middle+");
		s += 7;
	}
	sprintf(s,"(%d)",code.nclicks);
	return b;
}

/*--------------------------------------------------------------------------
 * macro_getCmdIndexByName()
 */
int macro_getCmdIndexByName(char *name)
{
	int			id;

	// TODO: should perform a hash lookup on a long term run.
	for (id = 0; id < _ncmdseq; id++) {
		if (strcmp(_cmdseqtab[id].c_name, name) == 0) {
			return id;
		}
	}
	yyerror("undefined internal macro %s",name);
	return -1;
}

