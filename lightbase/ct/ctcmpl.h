/* Turbo C */

#ifdef    VAXVMS
#include <file.h>
#else
#include <fcntl.h>
#endif

#ifdef MSDOS
#define BUPDATE (O_RDWR | O_BINARY )
#define	BCREATE	(O_CREAT | O_TRUNC | O_BINARY | O_RDWR)
#else
#define BUPDATE O_RDWR
#define BCREATE 0666
#ifndef SCOUNIX
#	define remove unlink
#endif
#endif

#define C255	-1

#ifdef WWW_TAVA_ASSIM_NA_REDE
#define   lock(x,y,z)      0  /* uso temporario para funcionar em rede */
#define   unlock(x,y,z)    0  /* uso temporario para funcionar em rede */
#define   lockf(x,y,z)     0  /* uso temporario para funcionar em rede */
#endif


#define ct_NULL	(char *) 0

/* ananias pg 6-2 */
#define cpybuf(dest,srce,n) 		memcpy((dest),(srce),(n))
#define setnul(p)                *p = '\0'

#ifndef FPUTFGET
#define   gtroot(knum)  knum->root
#endif

#ifndef NO_IDXENT
#  ifndef FPUTFGET
#     ifdef NOTFORCE
#        define   LB_UHD
#        define   hdrupd(knum, chgnum)  ((COUNT)(((knum)->nument += (chgnum)) & 0 ))
#     endif
#  endif
#endif
