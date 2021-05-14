/*
 * TOS.H
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: simulate TOS system calls with WINDOW calls
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 */

# ifndef TOS_H

# if defined( _Windows ) || defined(WIN32)

# ifndef __WINDOWS_H
# include <windows.h>
# endif

# define Fsetdta(dta)		/**/
# if defined(WIN32)
# define FA_NORMAL			_A_NORMAL
# define FA_DIR				_A_SUBDIR
# endif

# define Fcreate(file,mode)	_lcreat(file,mode)
# define Fwrite(fd,size,buf)	_lwrite(fd,(LPSTR)buf,size)
# define Fread(fd,size,buf)	_lread(fd,(LPSTR)buf,size)
# define Fseek(pos,fd,orig)	_llseek(fd,pos,orig)
# define Fopen(name,mode)	_lopen(name,mode)
# define Fclose(fd)			_lclose(fd)
# define Malloc(size)		(LPVOID)MAKELONG(0, GlobalAlloc(GPTR,size))
# define Free(buf)			GlobalFree((GLOBALHANDLE)LOWORD((LONG)buf))

# define Frename(dummy,old,new)	rename(old,new)
# define Fdelete(name)		unlink(name)
# define Ddelete(name)		_rmdir(name)
# define Dcreate(name)		_mkdir(name)

# else /* _Windows */

# include <io.h>
# include	<fcntl.h>

# if !defined(OF_READWRITE)
# define	OF_READWRITE		O_RDWR
# endif

# define Fcreate(file,mode)	_creat(file,mode)
# define Fwrite(fd,size,buf)	_write(fd,buf,size)
# define Fread(fd,size,buf)	_read(fd,buf,size)
# define Fseek(pos,fd,orig)	lseek(fd,pos,orig)
# define Fopen(name,mode)	_open(name,mode)
# define Fclose(fd)			_close(fd)

# define Frename(dummy,old,new)	rename(old,new)
# define Fdelete(name)		unlink(name)
# define Ddelete(name)		rmdir(name)
# define Dcreate(name)		mkdir(name)

# endif	/* _Windows */

# define TOS_H
# endif
