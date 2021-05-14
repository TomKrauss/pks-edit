/*********************************************************************/
/* EDSTRING.H                                          VERS 24.07.87 */
/* language-dependent MESSAGES for PKS-EDIT            crea. TOM     */
/*********************************************************************/

extern char *messget();

/*---------------------------------*/
/* Messages                        */
/*---------------------------------*/

# define	MESS_1     messget(MMM1) /* "%d bytes %d lines" */
# define	MESS_2     messget(MMM2) /* "enter SEQ-ID"      */
# define	MESS_3     messget(MMM3) /* "[2][new File][CANCEL|OK]" */
# define	MESS_4     messget(MMM4) /* "[2][RM File|" */
# define	MESS_5     messget(MMM5) /* "][YES|NO]" */
# define	MESS_6     messget(MMM6) /* "ln %d col %d " */         
# define	MESS_7     messget(MMM7) /* "%d tabs %d lines" */
# define	MESS_8     messget(MMM8) /* "replace ? y\\n\\c" */
# define	MESS_9     messget(MMM9) /* "%d subs %d lines" */
# define	MESS_10    messget(MMM10) /* "[2][Save your work|before quitting][SAVE|QUIT|STAY]" */
# define	MESS_11    messget(MMM11) /* "[2][Abandon current Edit][ABANDON|CANCEL]" */
# define	MESS_12    messget(MMM12) /* "[2][ABORT][YES|NO]" */
# define	MESS_13    messget(MMM13) /* "new File " */
# define	MESS_14    messget(MMM14) /* "saving " */
# define	MESS_15    messget(MMM15) /* "Error !" */
# define	MESS_19	messget(MOVRWR)	/* "Datei existier .. " */
#ifdef DEUTSCH
# define	MESS_16    "RSC_DATEI NICHT GEFUNDEN"
#else
# define	MESS_16    "RSC_FILE NOT FOUND"
#endif
# define	MESS_17    messget(MMM17) /* "big Font" */
# define	MESS_18    messget(MMM18) /* "small Font " */

/*---------------------------------*/
/* special FILES                   */
/*---------------------------------*/

# define	FNAM_1     messget(MFN1)       /* Key-Definitions            */
# define	FNAM_2     messget(MFN2)       /* Standard-Tagfile           */
# define	FNAM_3     messget(MFN3)       /* Editor SETUP               */
# define	FNAM_4     messget(MFN4)       /* Standard EDITLIST          */
# define	FNAM_5     messget(MFN5)       /* Errormessage-File (not used) */

/*--------- PRINTER  --------------*/

# define	FNAM_6     messget(MFN6)      /* Printer-Program            */
# define	FNAM_7     messget(MFN7)      /* Printer-Definition         */

/*-------- RSC-FILES --------------*/

# define	FNAM_8     "PKSEDIT.RSC"
# define	FNAM_9     "PKSEDITH.RSC" 
# define	FNAM_10    messget(MFN10)      /* PKSEDIT.GRP                */
