#define   MODULO_DEFINICAO  /* para nao incluir ctgvar.h em ctstdr.h */

#include "ctstdr.h"		/* standard i/o header 		*/

#undef   EXTERN
#define  EXTERN /* */

#include  "ctgvar.h"

#ifdef MSDOS
#  include <fcntl.h>
#  include <share.h>	/* From Borland			*/
   int  _Cdecl open	 (const char *__path, int __access,...);
#else
#  ifdef VAXMS
#     include <file.h>
#  else
#     include <unistd.h>
#  endif
#endif


#ifdef VAXVMS
#  include <types.h>
#  include <stat.h>
#else
#  include <sys/types.h>
#  include <sys/stat.h>
#endif


COUNT ctseek(),uerr();

RNDFILE mbopen(ctnum,opmode)
PFAST CTFILE *ctnum;
COUNT opmode;	/* EXCLUSIVE or SHARED */
{
	RNDFILE retval;
#ifdef MSDOS
	int     acflag,shflag;
#else
	int	acflag;
	int	i;
	long lseek();
#endif


	COUNT vtclose();

	ctnum->sekpos = DRNZERO;
	acflag        = BUPDATE;

#ifdef MSDOS
	if (opmode & SHARED)
		shflag = SH_DENYNO;
	else if (opmode & READFIL) {
		shflag = SH_DENYWR;
		acflag = (O_BINARY | O_RDONLY);
	} else
		shflag = O_DENYALL;
#else
	if (opmode & READFIL)
		acflag = O_RDONLY;
#endif

	if (!(opmode & PERMANENT) && ct_numvfil >= MAXVFIL)
		vtclose();

#ifdef MSDOS
   retval = open(ctnum->flname,(acflag | shflag), 0);

	if ( retval  < 0)  {
		if (vtclose() == YES)
			retval = open(ctnum->flname,(acflag | shflag), 0);
   }
#else
	if ((retval = open(ctnum->flname,acflag)) < 0)
		if (vtclose() == YES)
			retval = open(ctnum->flname,acflag);

	/* obtain lock on entire file if exclusive open */
	if (retval > -1 && !(opmode & NONEXCLUSIVE)) {
		lseek(retval,ctnum->sekpos = HDRSIZ,0);
		if (lockf(retval,F_TLOCK,0L)) {
			close(retval);
			return(-1);
		}
	} else if (retval > -1 && (opmode & NONEXCLUSIVE)) {
		/* check file type to see if index: no lock on index */
		if (read(retval,ctnum,HDRSIZ) != HDRSIZ) {
			close(retval);
			return(-1);
		} else {
#ifdef UNIFRMAT
			revhdr(ctnum);
#endif
			ctnum->sekpos = HDRSIZ;
		}
      if (ctnum->clstyp != IDX_CLOSE && ! (opmode & READFIL)) {
			for (i = HDRSIZ; i < 128; i++) {
				lseek(retval,ctnum->sekpos = i, 0);
				if (lockf(retval,F_TLOCK,1L) == 0)
					break;
			}
			if (i >= 128) {
				close(retval);
				return(-1);
			}
		}
	}
#endif

	if (!(opmode & PERMANENT) && retval >= 0)
		ct_numvfil++;

	return(retval);
}

/* ------------------------------------------------------------ */

RNDFILE mbcrat(ctnum)
PFAST CTFILE *ctnum;
{
	RNDFILE retval;

	COUNT vtclose();

	ctnum->sekpos = DRNZERO;
	if (!(ctnum->flmode & PERMANENT) && ct_numvfil >= MAXVFIL)
		vtclose();

#ifdef MSDOS
	if ((retval = open(ctnum->flname,
	    BCREATE,(S_IREAD | S_IWRITE))) < 0)
		if (vtclose() == YES)
			retval = open(ctnum->flname,
			    BCREATE,
			    (S_IREAD | S_IWRITE));
#else
/*	if ((retval = creat(ctnum->flname,BCREATE)) < 0) */
  	if ((retval = open(ctnum->flname,( O_RDWR | O_CREAT | O_TRUNC), BCREATE)) < 0)

		if (vtclose() == YES)
			retval = open(ctnum->flname,  O_RDWR | O_CREAT | O_TRUNC, BCREATE);

#endif

	if (!(ctnum->flmode & PERMANENT) && retval >= 0)
		ct_numvfil++;

	return(retval);
}

/* ------------------------------------------------------------ */

COUNT mbsave(ctnum)
PFAST CTFILE *ctnum;
{
#ifdef MSDOS
	COUNT   mbclos();
	RNDFILE mbopen();

	if (mbclos(ctnum,ctnum->flmode))
		return(uerr(FSAV_ERR));
	else if ((ctnum->fd = mbopen(ctnum,ctnum->flmode)) < 0)
		return(uerr(FSAV_ERR));
	else
		return(NO_ERROR);
#else
	sync();
	return(NO_ERROR);
#endif
}


