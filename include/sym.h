
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
	S_CONSTSTRING = 10,
	S_FLOAT = 11
} SYMBOL_TYPE;

typedef struct tagPKS_VALUE {
	SYMBOL_TYPE		sym_type;
	GENERIC_DATA	sym_data;
} PKS_VALUE;

#define	TYPEOF(s)			((s).sym_type)
#define	VALUE(s)			((s).sym_data).val
#define	CLEARSYM(s)			{(s).sym_data.val = 0, (s).sym_type = 0;}
#define	SETTYPE(s,type)		((s).sym_type = type)
#define	NULLSYM(s)			(TYPEOF(s) == 0 && VALUE(s) == 0)

#define	HSIZE		697

extern int 		 sym_insert(char *key, SYMBOL_TYPE stType, GENERIC_DATA symdata);
extern PKS_VALUE sym_find(char *key, char **key_ret);
extern int 		 sym_makeInternalSymbol(char *name, SYMBOL_TYPE stType, GENERIC_DATA value);

#define	SYM_H
# endif

