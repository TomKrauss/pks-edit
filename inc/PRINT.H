/************************************************************************/
/* PKSPRINT.H										VERS 8/87   */
/* Include-Datei fÅr das Druckerprogramm fÅr PKSEDIT.PRG	crea TOM	  */
/************************************************************************/

#ifndef _FPCURRL
typedef long FTABLE;
#endif

# define	MAXDAEMON	2

typedef struct daemoninfo {
	long   eda4,eda5;
	FTABLE *ft,*cf;			/* table of all files and current		*/
	char	  *fseltarget;
#ifdef MEGAMAX
	int    (*fselfunc)();
	int	  (*titlefunc)();
	int	  (*readfunc)();
	int	  (*clearfunc)();
#else	TURBO
	int	  (*fselfunc)(char *,char *,char *,int ,int );
	void	  (*titlefunc)(char *);
	int    (*readfunc)(FTABLE *,char *,int ,int ,int );
	void	  (*clearfunc)(LINE *);
#endif
	char   *prname;
	int 	  tabsize;
	int 	  prsem;
	struct daemon {
#ifdef MEGAMAX
		int	  	(*func)();
#else
		int cdecl	(*func)(int );
#endif
		char		*fname;
		int		flags;
	} daemon[MAXDAEMON];
} DAEMONINFO;

# define	_EDA4		/* 0	*/
# define	_EDA5		4

/*---------------------------------*/
/* Daemonflags					*/
/*---------------------------------*/
# define	ISGEMPROG		1

/*---------------------------------*/

# define	PRINTERDAEMON	0

/*---------------------------------*/
/* Aufrufparameterkonvention		*/
/* fÅr Daemonfunktionen			*/
/*---------------------------------*/
# define	D_EXIT			1
# define	D_FCALL		0

# include	"prt.h"

/*---------------------------------*/
/* Programm-Returns 			*/
/*---------------------------------*/

# define	PRTOK		0		/* alles hat geklappt			*/
# define	PRTNOPRT 	1		/* Fehler beim Drucken			*/
# define	PRTNOSERV	2		/* kein Konfigurationsfile		*/
# define	PRWRONGARG	3		/* falsche Argumente			*/

/*---------------------------------*/
/* Optionen					*/
/*---------------------------------*/

# define	OPT_HEADER		0x01
# define	OPT_LNUMBER		0x02

/*---------------------------------*/
/* The Q-Structure				*/
/*---------------------------------*/

typedef struct prqueue {
	char 	fn[82];
	int		ncop;
	int		firstp,lastp;		/* first and last page to print		*/
	char		menflg;			/* flags for menuhandler				*/
	char		state;			/* State of this entry				*/
	int 		options;
	int		devfd;			/* File-descriptor - Outputdevice		*/
} PRQUEUE;

/*---------------------------------*/
/* Print-States				*/
/*---------------------------------*/

# define	PS_UNUSED		0x0		/* this entry is not used			*/
# define	PS_WAITING		0x1		/* waiting for output				*/
# define	PS_RUNNING		0x2		/* currently outputting				*/
# define	PS_SLEEPING		0x3		/* output in progress but sleeping		*/
# define	PS_SUSPENDED	0x4		/* suspended after error				*/


