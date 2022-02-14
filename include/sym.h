
# ifndef SMY_H

typedef enum {
	S_KEYWORD = 1,
	S_NUMBER = 2,
	S_STRING = 3,
	S_DOLNUMBER = 4,
	S_DOLSTRING = 5,
	S_EDFUNC = 6,
	S_TYPE = 7,
	S_ENUM = 8,
	S_CONSTNUM = 9,
	S_CONSTSTRING = 10
} SYMBOL_TYPE;

typedef struct tagPKS_VALUE {
	SYMBOL_TYPE		sym_type;
	void*			sym_data;
} PKS_VALUE;

#define	TYPEOF(s)			((s).sym_type)
#define	VALUE(s)			((s).sym_data)
#define	VALUE_AS_INT(s)		((int)(intptr_t)((s).sym_data))
#define	CLEARSYM(s)			{(s).sym_data = 0, (s).sym_type = 0;}
#define	SETTYPE(s,type)		((s).sym_type = type)
#define	SETSYMBOL(s,type,v)	((s).sym_type = type, (s).sym_data = (void*)v)
#define	NULLSYM(s)			(TYPEOF(s) == 0 && VALUE(s) == 0)

#define	HSIZE		697

extern int 		 sym_insert(char *key, SYMBOL_TYPE stType, intptr_t symdata);
extern PKS_VALUE sym_find(char *key, char **key_ret);
extern int 		 sym_create(unsigned nel);
extern int 		 sym_makeInternalSymbol(char *name, SYMBOL_TYPE stType, intptr_t value);

#define	SYM_H
# endif

