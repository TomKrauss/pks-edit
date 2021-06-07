/*
 * KEYS.C
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: translate key names 2 scan code and vc.vs.
 *		  maintain key-function bindings in a hash tab
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauž
 */

# include	"edfuncs.h"
# include	"resource.h"

extern char	*_alloc();

static RSCTABLE __k = {
	0,	"default",	sizeof _keymaptab, 	_keymaptab,
	(unsigned char *)_keymaptab+sizeof(_keymaptab)
};

RSCTABLE *_keytables = &__k;

#if !defined(TOOL_COMPILER)

/*---------------------------------*/
/* key_destroytables()			*/
/*---------------------------------*/
void key_destroytables(void)
{
	RSCTABLE 	*rp;
	
	for (rp = _keytables; rp; rp = rp->rt_next) {
		if (rp != &__k) {
			_free(rp);
		}
	}
	_keytables = &__k;
	memset(_keymaptab,0,sizeof _keymaptab);
}

/*---------------------------------*/
/* key_unbind()				*/
/*---------------------------------*/
void key_unbind(KEYBIND *kp)
{
	if (kp == 0)
		return;

	kp->keycode = 0;		/* mark used */
	kp->macref.typ = CMD_NONE;
}

/*---------------------------------*/
/* key_delbindings()			*/
/* delete all key bindings to a 	*/
/* for inst. single macro,...		*/
/*---------------------------------*/
void key_delbindings(MACROREFTYPE typ, MACROREFIDX val)
{
	RSCTABLE *rp;
	KEYBIND  *kp;

	for (rp = _keytables; rp; rp = rp->rt_next) {
		for (kp = rp->rt_data; kp < rp->rt_end; kp++) {
			if (typ == kp->macref.typ && val == kp->macref.index) {
				check_fkdirty(kp->keycode);
				key_unbind(kp);
			}
		}
	}
}

/*-----------------------------------------------------------
 * keyslot()
 */
static KEYBIND *keyslot(KEYCODE keycode)
{
	return rsc_tbfindcode(_keytables,sizeof(KEYBIND),keycode);
}

/*-----------------------------------------------------------
 * keybound()
 */
KEYBIND *keybound(KEYCODE keycode)
{
	return keyslot(keycode);
}

/*-----------------------------------------------------------
 * keygetbind()
 */
KEYBIND *keygetbind(void)
{
 	KEYBIND 	*kp;

	if ((kp = keyslot(0)) != 0)
		return kp;

	return rsc_tbresize(_keytables,sizeof *kp, _keymaptab);
}

# endif

#if defined(ATARI_ST)

char *_scantab=
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

int s2shift(char **k)
{	char *K = *k;
	int  control = 0;

	while(K[1]) {
		switch (*K) {
			case '^':	control |= 0x4; break;
			case '': control |= 0x2; break;
			case '':	control |= 0x8; break;
			default : goto out;
		}
		K++;
	}
out:
	*k = K;
	return control;
}

KEYCODE key2code(char *K)
{	char *t,*k;
	int  code,control;

	control = s2shift(&K);

	for (code = 114, t = _scantab+1; code >= 0; t++ , code-- ) {
fail:
		if (*t) {
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
				if (control & 0x4) {
					if (code == 75)
						code = 115;
					else if (code == 77)
						code = 116;
				}
	
				/*
				 * [Alt]0-9
				 */
				if (code >= 2 && code <= 13 && (control & (0x8|0x4)) == 0x8)
					code += 118;
				else

				/*
				 * [Sft]F1-F10
				 */
				if (code >= 59 && code <= 68 && (control & 0x3))
					code += (84-59);

				return (control<<8)|code;
			}
		}
	}
	return K_INVALID;
}

int s2button(char **S)
{	int button = 0;
	char *s = *S;

	while(1) {
		switch(*s++) {
			case 'l': case 'L':	button |= 0x1; break;
			case 'r': case 'R':	button |= 0x2; break;
			case 0: case ' ': case '\t': *S = s; return button;
		}
	}
}

char *code2key(KEYCODE code)
{	char *t,*s;
	int  i;
	static char b[12];

	s = b;
	if (code & K_ALTERNATE)
		*s++ = '';
	if (code & K_CONTROL)
		*s++ = '^';
	if (code & K_SHIFT)
		*s++ = '';

	code &= 0xFF;
	if (code <= 0 || code >= 132) 
		return "*UNDEFINED*";

	for (i = 0, t = _scantab; i < code; t++, i++) 
		if (*t) while (*t) t++;

	while((*s++ = *t++) != 0)
		;

	return b;
}

#else /* WINDOWS */
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

/*--------------------------------------------------------------------------
 * s2shift()
 */
int s2shift(char **k)
{	char *K = *k;
	int  control = 0;

	while(K[1]) {
		switch (*K) {
			case CTL_MARK:	control |= (K_CONTROL>>8); break;
			case SFT_MARK: control |= (K_SHIFT>>8); break;
			case ALT_MARK: control |= (K_ALTERNATE>>8); break;
			default : goto out;
		}
		K++;
	}
out:
	*k = K;
	return control;
}

/*--------------------------------------------------------------------------
 * key2code()
 */
KEYCODE key2code(char *K)
{	char *t,*k;
	int  code,control;
	extern char	*_scantab;
	
	control = s2shift(&K);

	k = K;

	if (!k[1]) {
		if ((*k >= '0' && *k <= '9') ||
		    (*k >= 'A' && *k <= 'Z')) {
			code = *k;
			goto ret;
		}
	}

	if (k[0] == '\\') {
		code  = (int)(k[1] - '0') * 100;
		code += (int)(k[2] - '0') * 10;
		code += (int)(k[3] - '0');
		goto ret;
	}

	for (t = _scantab; *t; ) {
		code = *t++;
		for (k = K; *k == *t; k++,t++)
			if (*t == 0) {
ret:				return (control<<8)|code;
			}
		while(*t++)
			;
	}
	return K_INVALID;
}


char *code2key(KEYCODE code)
{
	char        *s;
	unsigned char *t;
	static char b[32];

	if (code == K_DELETED)
		return "*NO KEY*";

	s = b;
	if (code & K_ALTERNATE)
		*s++ = ALT_MARK;
	if (code & K_CONTROL)
		*s++ = CTL_MARK;
	if (code & K_SHIFT)
		*s++ = SFT_MARK;

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

#endif

#ifdef TEST
# include	<stdio.h>
int main(int argc,char *argv[])
{	char b[128];
	int code,i;

	if (argc == 3)

	for (;;) {
		printf(" > ");
		if (!gets(b) || !*b)
			break;
		code = key2code(b);
		printf("%s -> %d (%s)\n",b,code,code2key(code));
	}
	
	else if (argc == 2)

	for (i = 0 ; i < 1000; i++)
		key2code(argv[1]);

	else
		printf("Oops ... [args]\n");

	return 0;
}

#endif

