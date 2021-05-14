/*
 * MACLIB.C
 *
 * PROJEKT: PKS-EDIT 2.0
 *
 * purpose: common functions for macro handling
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

# if defined( _Windows )
# include	<windows.h>
# include "winterf.h"
# define sprintf wsprintf
# else
# include <stdio.h>
# define	hInst		0
# endif

# include	<string.h>
# include	"edfuncs.h"

extern	MACRO	*mac_byindex(int i);

/*------------------------------------------------------------
 * mac_name()
 */
char *mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type)
{
	char szText[64],*d,*s;

	switch(type) {
		case CMD_MACRO:  sprintf(szBuf,"%s",mac_byindex(nIndex)->name); break;
		case CMD_CMDSEQ: 
			if (LoadString(hInst,nIndex+IDM_CMDNAME,szBuf+1,250) <= 0) {
				sprintf(szBuf,"©Unamed-%d",nIndex);
			} else {
				szBuf[0] = '©';
			}
			break;
		default: sprintf(szBuf,"??"); break;
	}
	return szBuf;
}

/*---------------------------------*/
/* param_space()				*/
/*---------------------------------*/
int param_space(unsigned char typ, char *s)
{	int size;

	switch(typ) {
		case C_0FUNC:
		case C_CHAR1PAR:
			return sizeof(COM_CHAR1);
		case C_1FUNC:
			return sizeof(COM_1FUNC);
		case C_FORMSTART:
			return sizeof(COM_FORM);
		case C_GOTO:
			return sizeof(COM_GOTO);
		case C_STOP:
			return sizeof(COM_STOP);
		case C_INT1PAR:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_INT1);
		case C_LONG1PAR:
			/* only if your alignment = 2,2,2 */
			return sizeof(COM_LONG1);
		case C_STRINGVAR:
		case C_LONGVAR:
		case C_MACRO:
		case C_STRING1PAR:
			size = (sizeof(struct c_ident) - 1 /* pad byte*/) +
					(strlen(s)+1)*sizeof(*s);
			if (size & 1) size++;
			return size;
		case C_TEST:
			return sizeof(COM_TEST);
		case C_DATA:
			size = sizeof(COM_DATA) + ((COM_DATA*)s)->size;
	}
	/*
	 * oops: this is an error
	 */
	mac_formaterr();
	return 10000L;
}

char *_scantab=
#if defined(DOS)
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
# else
/* 0-14   */  "\0Esc\0\x31\0\x32\0\x33\0\x34\0\x35\0\x36\0\x37\0\x38\0\x39\0\x30\0ž\0\'\0Backspace\0"
/* 15-29  */  "Tab\0Q\0W\0E\0R\0T\0Z\0U\0I\0O\0P\0š\0+\0Return\0Ctr\0"
/* 30-43  */  "A\0S\0D\0F\0G\0H\0J\0K\0L\0™\0Ž\0#\0ShL\0~\0"
/* 44-53  */  "Y\0X\0C\0V\0B\0N\0M\0,\0.\0-\0"
/* 54-58  */  "ShR\0\0Alt\0Leer\0CLock\0"
/* 59-68  */  "F1\0F2\0F3\0F4\0F5\0F6\0F7\0F8\0F9\0F10\0"
/* 69-70  */  "\0\0"
/* 71-83  */  "ClrHome\0\0\0Z-\0\0\0\0Z+\0\0\0\0Insert\0Delete\0"
/* 84-93  */  "F1\0F2\0F3\0F4\0F5\0F6\0F7\0F8\0F9\0F10\0"
/* 94-102 */  "\0\0<\0Undo\0Help\0Z(\0Z)\0Z/\0Z*\0"
/* 103-112*/  "Z7\0Z8\0Z9\0Z4\0Z5\0Z6\0Z1\0Z2\0Z3\0Z0\0"
/* 113-119*/  "Z.\0Enter\0\0\0\0\0\0"
/* 120-132*/  "1\0" "2\0" "3\0" "4\0" "5\0" "6\0" "7\0" "8\0" "9\0" "0\0" "ž\0'";
# endif

static char *s2shift(char *s, unsigned code)
{
	if (code & K_ALTERNATE) {
		strcpy(s,"Alt+");
		s += 4;
	}
	if (code & K_CONTROL) {
		strcpy(s,"Ctrl+");
		s += 5;
	}
	if (code & K_SHIFT) {
		strcpy(s,"Shift+");
		s += 6;
	}
	return s;
}

char *code2key(KEYCODE code)
{
	char        *s;
	unsigned char *t;
	static char b[32];

	if (code == K_DELETED)
		return "*NO KEY*";

	s = s2shift(b,code);

	code &= 0xFF;

	if ((code >= '0' && code <= '9') ||
	    (code >= 'A' && code <= 'Z')) {
		*s++ = code;
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

	s = s2shift(b,code.shift<<8);
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


