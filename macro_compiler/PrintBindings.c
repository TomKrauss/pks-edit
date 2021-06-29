/*
 * PrintBindings.c
 *
 * print key and mouse mappings
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 13.03.1991
 */

#define	PASTELIST		void

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "alloc.h"
#include "winterf.h"
#include "edfuncs.h"
#include "pksmod.h"

#include "pkscc.h"
#include "funcdef.h"
#include "brackets.h"

#include "resource.h"
#include "stringutil.h"

#if defined(WIN32)
#define	_Windows
#endif

#define COL_KEYCODE		20
#define COL_FUNCTION	30

extern	EDBINDS 	_bindings;
extern	RSCTABLE *_keytables;
extern	RSCTABLE *_mousetables;
extern	MACRO *	macro_getByIndex(int i);
extern	char *	code2key(KEYCODE code);
extern 	char *	mac_name(char *szBuf, MACROREFIDX nIndex, MACROREFTYPE type);
extern 	MACROREF *macro_getMacroIndexForMenu(int nId);

void macro_printListHeader(FILE *fp, char *itemname);

/*
 * c2shift()
 */
char *c2shift(char *s, KEYCODE code)
{
	*s = 0;
	if (code & K_ALTERNATE) {
		strcpy(s,"Alt+"); s += 4;
	}
	if (code & K_HAS_SELECTION) {
		strcpy(s, "Selected+"); s += 4;
	}
	if (code & K_CONTROL) {
		strcpy(s,"Ctrl+"); s += 5;
	}
	if (code & K_SHIFT) {
		strcpy(s,"Shift+"); s += 6;
	}
	return s;
}

static char *c2button(char *b, int button, int shift, int nclicks)
{
	char			*s;

	s = c2shift(b,(KEYCODE)(shift<<8));
	if (button & MBUT_L) {
		strcpy(s,"MLeft+");
		s += 5;
	}
	if (button & MBUT_R) {
		strcpy(s,"MRight+");
		s += 6;
	}
	if (button & MBUT_M) {
		strcpy(s,"MMiddle+");
		s += 7;
	}
	sprintf(s,"(%d)",nclicks);
	return b;
}

/*--------------------------------------------------------------------------
 * key2code()
 * sorry for this: PKEDIT has the following functions also implemented. 
 * but: making Keys more readable than in menu enmtries in dumb GEM menus 
 *      we has to have a local _scantab
 */
#if defined (_Windows)

extern char *_scantab;

KEYCODE key2code(unsigned char *k, int control)
{
	unsigned char *	t;
	unsigned char *	K;
	int  			code;

	if (!k[1]) {
		if ((*k >= '0' && *k <= '9') ||
		    (*k >= 'A' && *k <= 'Z')) {
			return *k | control;
		}
	}

	if (k[0] == '\\') {
		code  = (int)(k[1] - '0') * 100;
		code += (int)(k[2] - '0') * 10;
		code += (int)(k[3] - '0');
		return code | control;
	}

	K = k;
	for (t = _scantab; *t; ) {
		code = *t++;
		for (k = K; *k == *t; k++,t++)
			if (*t == 0) {
				return code | control;
			}
		while(*t++)
			;
	}
	return K_INVALID;
}

#else
static char *_scantab=
/* 0-14   */  "\0Esc\0\x31\0\x32\0\x33\0\x34\0\x35\0\x36\0\x37\0\x38\0\x39\0\x30\0Beta\0\Quote\0Backspace\0"
/* 15-29  */  "Tab\0Q\0W\0E\0R\0T\0Z\0U\0I\0O\0P\0UmlautUE\0Plus\0Return\0Ctr\0"
/* 30-43  */  "A\0S\0D\0F\0G\0H\0J\0K\0L\0UmlautOE\0UmlautAE\0Hash\0ShL\0Tilde\0"
/* 44-53  */  "Y\0X\0C\0V\0B\0N\0M\0Colon\0Dot\0Minus\0"
/* 54-58  */  "ShR\0\0Alt\0Space\0CLock\0"
/* 59-68  */  "F1\0F2\0F3\0F4\0F5\0F6\0F7\0F8\0F9\0F10\0"
/* 69-70  */  "\0\0"
/* 71-83  */  "ClrHome\0Up\0\0NumMinus\0Left\0\0Right\0NumPlus\0\0Down\0\0Insert\0Delete\0"
/* 84-93  */  "F1\0F2\0F3\0F4\0F5\0F6\0F7\0F8\0F9\0F10\0"
/* 94-102 */  "\0\0Less\0Undo\0Help\0NumLBracket\0NumRBracket\0NumSlash\0NumStar\0"
/* 103-112*/  "Num7\0Num8\0Num9\0Num4\0Num5\0Num6\0Num1\0Num2\0Num3\0Num0\0"
/* 113-119*/  "NumDot\0Enter\0Left\0Right\0\0\0\0"
/* 120-132*/  "1\0" "2\0" "3\0" "4\0" "5\0" "6\0" "7\0" "8\0" "9\0" "0\0" "Beta\0'";
KEYCODE key2code(char *K, int control)
{
	char *	t;
	char *	k;
	int  	code;

	for (code = 114, t = _scantab+1; code >= 0; t++ , code-- ) {
fail:	if (*t) {
			k = K;
			while (*t)
				if (*k++ != *t++) {
					while(*t++);
					if (--code < 0)
						return K_INVALID;
					goto fail;
				}
			if (!*k) {
				code = 115-code;

				/*
				 * [Ctl] Cursorkeys left-right
				 */
				if (control & K_CONTROL) {
					if (code == 75)
						code = 115;
					else if (code == 77)
						code = 116;
				}
	
				/*
				 * [Alt]0-9
				 */
				if (code >= 2 && code <= 13 && 
				    (control & (K_ALTERNATE|K_CONTROL)) == K_ALTERNATE)
					code += 118;
				else

				/*
				 * [Sft]F1-F10
				 */
				if (code >= 59 && code <= 68 && 
				    (control & K_SHIFT))
					code += (84-59);

				return control|code;
			}
		}
	}
	return K_INVALID;
}

/*
 * code2key()
 */
static char *code2key(KEYCODE code)
{
	char *	t;
	char *	s;
	int  	i;
	static char b[64];

	s = c2shift(b,code);

	code &= 0xFF;

	if (code <= 0 || code >= 132) 
		return "[?]";

	for (i = 0, t = _scantab; i < code; t++, i++) {
		if (*t) {
			while (*t) 
				t++;
		}
	}

	strcpy(s,t);

	return b;
}

#endif

/*
 * prf3()
 */
void prf3(FILE *fp,char markc,char *s1,char *s2, char *s3)
{
	fprintf(fp,"%-*.*sú %-*.*s%c %s\n",COL_FUNCTION,COL_FUNCTION,s1,
							    COL_KEYCODE,COL_KEYCODE,s2,markc,s3);
}

/*--------------------------------------------------------------------------
 * pr_cmddelim()
 */
static char *pr_cmddelim(char *d,char *command,char delim)
{
	sprintf(d,"%s%c",command,delim);
	return d;
}

/*--------------------------------------------------------------------------
 * pr_comment()
 */
static char *pr_comment(char *d,char *comment)
{
	if (!comment)
		return "";
	sprintf(d,"# %s",comment);
	return d;
}

/*--------------------------------------------------------------------------
 * find_menu_bind()
 */
int find_menu_bind(int idx)
{
	MENUBIND *	mp;
	EDBINDS	*	ep = &_bindings;
	int			i;

	mp = ep->mp;
	for (i = 0; i < *(ep->nmp); i++) {
		if ((int)mp[i].index == idx) {
			return i;
		}
	}
	return -1;
}

/*--------------------------------------------------------------------------
 * printkeybind()
 * print one key binding in the following manner
 * KEYCODE | FUNCTION+PARS | REMARK
 */
static void printkeybind(FILE *fp, KEYBIND *kp, char delim)
{
	EDBINDS *	ep = &_bindings;
	char 	comment[100],command[128],n1[100],n2[100];
	int  		findex;
	char		*funame;
	MENUBIND 	*mp;
	MACRO	*macp;

	if (kp->keycode == 0 || kp->keycode == K_DELETED)
		return;

	findex = kp->macref.index;
	comment[0] = 0;

	/* prepare for invalid entries */

	comment[0] = 0;
	switch(kp->macref.typ) {
		case CMD_MENU:
			mp = &ep->mp[findex];
#if !defined(_Windows)
			sprintf(comment,"%s¯%s",
						menustring(n1,mp->mtitlenum),
						menustring(n2,mp->menunum));
# endif
			findex = mp->index;
			break;
		case CMD_MACRO:
			if ((macp = ep->macp[findex]) != 0)
				strcpy(comment,MAC_COMMENT(macp)); 
			break;
		case CMD_CMDSEQ:
			break;
		default:
			strcpy(comment,"garbagge.."); 
			findex = -1;
			break;
	}

	funame = mac_name(command,findex,(MACROREFTYPE)kp->macref.typ);

 	fprintf(fp,"%-25s= %-25s%s\n",
		code2key(kp->keycode),pr_cmddelim(n1,command,delim),
		pr_comment(n2,comment));
}

/*
 * keycomp()
 *
 * sort KEYBIND entries according to scancode:
 *
 */
static int keycomp(KEYBIND *kp1, KEYBIND *kp2)
{
	if (!kp1->keycode || kp1->keycode == K_DELETED)
		return -1;
	if (!kp2->keycode || kp2->keycode == K_DELETED)
		return 1;
	return kp1->keycode-kp2->keycode;
}

/*
 * tmpname()
 */
char *tmpname(char *filename)
{	static char fn[84];
	static char *tmpdir;

	if (tmpdir == 0) {
		if ((tmpdir = getenv("TMPDIR")) == 0)
			tmpdir = ".";
	}
	string_concatPathAndFilename(fn,tmpdir,filename);
	return fn;
}
	
/*
 * macro_printKeyBindingsCallback()
 */
static int macro_printKeyBindingsCallback(FILE *fp)
{
	RSCTABLE *	rtp;
	KEYBIND *		kps;
	KEYBIND *		kpd;
	int	    		i,n;
	long 		size;

	for (rtp = _keytables; rtp; rtp = rtp->rt_next) {
		kps = (KEYBIND *)rtp->rt_data;
		for (i = 0, n = 0; &kps[i] < (KEYBIND *)rtp->rt_end; i++) {
			if (kps[i].keycode && kps[i].keycode != K_DELETED) {
				n++;
			}
		}
			
		size = n * sizeof *kpd;
		if ((kpd = malloc(size)) == 0) {
			return 0;
		}

		for (i = 0, n = 0; &kps[i] < (KEYBIND *)rtp->rt_end; i++) {
			if (kps[i].keycode && kps[i].keycode != K_DELETED) {
				kpd[n++] = kps[i];
			}
		}
		qsort(kpd,n,sizeof *kpd,(int (*)(const void*, const void*))keycomp);
		fprintf(fp,"\n\n//----------------------------------------\n");
		fprintf(fp,"\n\noverride KEYS %s	# total %d bindings\n\n",
			rtp->rt_name, n);

		for (i = 0; i < n; i++) {
			printkeybind(fp, &kpd[i], i < n-1 ? ',' : ';');
		}

		free(kpd);

	}
	return 1;
}

/*
 * macro_saveKeyBindingsAndDisplay
 */
int macro_saveKeyBindingsAndDisplay(void) {
	return macro_createFileAndDisplay("KEYS", macro_printKeyBindingsCallback);
}


/*
 * printmousebind()
 * command clicks button shift comment
 */
static void printmousebind(FILE *fp, MOUSEBIND *mp, char delim)
{
	char 	command[128],button[30],b2[128],b3[128];

	mac_name(command,mp->macref.index,mp->macref.typ);
	c2button(button,mp->button,mp->shift,mp->nclicks);
 	fprintf(fp,"%-25s= %c %-25s%s\n",
		button,(mp->flags & MO_FINDCURS) ? '+' : ' ',
		pr_cmddelim(b2,command,delim),pr_comment(b3,mp->msg));
}

/*
 * mousecomp()
 *
 * sort MOUSEBIND entries according to Button nclicks shift
 *
 */
static int mousecomp(MOUSEBIND *mp1, MOUSEBIND *mp2)
{	int d;

	if ((d = mp1->button - mp2->button) == 0 &&
	    (d = mp1->nclicks - mp2->nclicks) == 0) {
		d = mp1->shift - mp2->shift;
	}
	return d;
}

/*
 * macro_printMouseBindingCallback
 */
static int macro_printMouseBindingCallback(FILE *fp)
{
	RSCTABLE *	rtp;
	MOUSEBIND *	mps;
	MOUSEBIND *	mpd;
	int	    		i,n;
	long 		size;

	for (rtp = _mousetables; rtp; rtp = rtp->rt_next) {
		mps = (MOUSEBIND *)rtp->rt_data;
		for (i = 0, n = 0; &mps[i] < (MOUSEBIND *)rtp->rt_end; i++) {
			if (mps[i].nclicks && mps[i].button) {
				n++;
			}
		}
			
		size = n * sizeof *mpd;
		if ((mpd = malloc(size)) == 0) {
			return 0;
		}

		for (i = 0, n = 0; &mps[i] < (MOUSEBIND *)rtp->rt_end; i++) {
			if (mps[i].nclicks && mps[i].button) {
				mpd[n++] = mps[i];
			}
		}
		qsort(mpd,n,sizeof *mpd,(int (*)(const void*, const void*))mousecomp);
		fprintf(fp,"\n\n//----------------------------------------\n");
		fprintf(fp,"\n\noverride MOUSE %s	# total %d bindings\n\n",
			rtp->rt_name, n);

		for (i = 0; i < n; i++) {
			printmousebind(fp, &mpd[i], i < n-1 ? ',' : ';');
		}

		free(mpd);

	}
	return 1;
}

/*
 * print the current mouse bindings to a file and display them to the user.
 */
int macro_saveMouseBindingsAndDisplay(void)
{
	return macro_createFileAndDisplay("MOUSE", macro_printMouseBindingCallback);
}

/*--------------------------------------------------------------------------
 * PrintSubMenu()
 */
static void PrintSubMenu(FILE *fp, HMENU hMenu)
{
	MACROREF *	mp;
	HMENU		hSubMenu;
	UINT		wCount;
	UINT		wState;
	char		szText[100];
	char		command[100];
	int			nItem;
	int			wID;

	wCount = GetMenuItemCount(hMenu);
	for (nItem = 0; nItem < (int)wCount; ) {
		wState = GetMenuState(hMenu, nItem, MF_BYPOSITION);
		if (HIBYTE(wState) && 
			(hSubMenu = GetSubMenu(hMenu, nItem)) != 0) {
			GetMenuString(hMenu, nItem, szText, sizeof szText,
				MF_BYPOSITION);
			fprintf(fp,"POPUP \"%s\" {\n", szText);
			PrintSubMenu(fp, hSubMenu);
			fprintf(fp, "}");
		} else if (wState & MF_SEPARATOR) {
			fprintf(fp,"\t----");
		} else {
			GetMenuString(hMenu, nItem, szText, sizeof szText,
				MF_BYPOSITION);
			if ((wID = GetMenuItemID(hMenu, nItem)) <= 0 ||
			    (mp = macro_getMacroIndexForMenu(wID)) == 0) {
				lstrcpy(command, "??");
			} else {
				mac_name(command, mp->index, mp->typ);
			}
			fprintf(fp,"\t\"%s\" = %s", szText, command);
		}
		if (++nItem < (int)wCount) {
			fprintf(fp, ",");
		}
		fprintf(fp, "\n");
	}
}

/*--------------------------------------------------------------------------
 * macro_printMenuCallback ()
 */
static int macro_printMenuCallback(FILE *fp)
{
	HMENU 	hMenu;

	fprintf(fp,"\n\nMENU default\n");
	hMenu = GetMenu(hwndFrame);
	PrintSubMenu(fp, hMenu);
	fprintf(fp,";\n");
	return 1;
}

/*--------------------------------------------------------------------------
 * macro_saveMenuBindingsAndDisplay()
 */
int macro_saveMenuBindingsAndDisplay(void)
{
	return macro_createFileAndDisplay("MENU", macro_printMenuCallback);
}

/*--------------------------------------------------------------------------
 * macro_loadStringResource()
 */
char *macro_loadStringResource(int nIndex)
{
	return nIndex >= 0 && nIndex < _ncmdseq ? _cmdseqtab[nIndex].c_name : "unknown";
}
