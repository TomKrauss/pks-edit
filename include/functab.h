
#ifndef 	_FUNCDEFS_H

#define 	FUNC_EdBlockCopy			2
#define 	FUNC_EdBlockMove			6
#define 	FUNC_EdKeycodeInsert			16
#define 	FUNC_EdCharInsert			17
#define 	FUNC_EdOptionToggle			78
#define		CMD_EdOptionToggleBLK_COLUMN_SELECTION 51
#define		CMD_INITIATE_INCREMENTAL_SEARCH 235

typedef enum { OP_EDIT_MODE, OP_DISPLAY_MODE, OP_MACRO, OP_OPTIONS, OP_FILEFLAG } OP_FLAGTYPE;

#define	OPT_WMODE			(((long)OP_EDIT_MODE)<<16)
#define	OPT_DMODE			(((long)OP_DISPLAY_MODE)<<16)
#define	OPT_GLOBSET			(((long)OP_OPTIONS)<<16L)
#define  OPT_RECORD			(((long)OP_MACRO)<<16L)



#define	_FUNCDEFS_H
#endif
