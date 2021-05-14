
# include	<stdio.h>

extern void c2asc(char *start, char *source, unsigned char hi, unsigned char lo);
extern char *cryptXXXX(char *dest, char *source, void (*f)());

/*---------------------------------*/
/* MAIN						*/
/*---------------------------------*/
int main(int argc,char *argv[])
{
	char		buf[128];
	char		cryptBuf[128];

	while(gets(buf)) {
		cryptXXXX(cryptBuf, buf, c2asc);
		printf("%s -> %s\n", buf, cryptBuf);
	}
	return 0;
}

