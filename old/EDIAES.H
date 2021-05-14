
# ifndef _EDIAES_H

# include	"obspec.h"

# define	_EDIAES_H

# ifndef _VDIPARS_H
# include	"vdipars.h"
# endif

typedef struct options {
	int	index;
	long mask;
} DIALOPTIONS;

# ifdef __AES__

long sgoptions(OBJECT *tree, struct options *op, long flags, int set);
extern OBJECT 	*do_tree(int tnum);
extern OBJECT	*treeaddr(int tnum);

# define	CURRWI_YCOOR	-42

# endif

# endif

