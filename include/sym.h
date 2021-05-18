
# ifndef SMY_H

typedef struct tagSYMBOL {
	unsigned char		sym_type;
	void*				sym_data;
} SYMBOL;

#define	TYPEOF(s)			((s).sym_type)
#define	VALUE(s)			((s).sym_data)
#define	CLEARSYM(s)		{(s).sym_data = 0, (s).sym_type = 0;}
#define	SETTYPE(s,type)	((s).sym_type = type)
#define	SETSYMBOL(s,type,v)	((s).sym_type = type, (s).sym_data = v)
#define	NULLSYM(s)		(TYPEOF(s) == 0 && VALUE(s) == 0)

#define	S_KEYWORD		1
#define	S_NUMBER		2
#define	S_STRING		3
#define	S_DOLNUMBER	4
#define	S_DOLSTRING	5
#define	S_EDFUNC		6
#define	S_TYPE		7
#define	S_ENUM		8
#define	S_CONSTNUM	9
#define	S_CONSTSTRING	10

#define	HSIZE		697

extern long 	sym_destroy(char *key);
extern int 	sym_insert(char *key, int symtype, intptr_t symdata);
extern SYMBOL 	sym_find(char *key, char **key_ret);
extern int 	hash_create(unsigned nel);
extern int 	MakeInternalSym(char *name, char ed_typ, intptr_t value);

#define	SYM_H
# endif

