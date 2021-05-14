
# ifndef	DES_H

# define	DES_STANDARD	0	/* standard Data Encryption Algorithm */
# define	DES_NOFINAL	1	/* DEA without initial and final 
						 * permutations for speed */
# define	DES_KEY128	2	/* DEA without permutations and with 128-byte 
						 * key (completely independent subkeys for 
						 * each round) */


# define	DES_BLOCKS	8	/* (de|en)des working on 8 byte blocks */

extern int crypt_init(int mode, char *akey);
extern int crypt_en(char *buf, int size, int cont);
extern int crypt_de(char *buf, int size, int cont);

# define	DES_H
# endif

