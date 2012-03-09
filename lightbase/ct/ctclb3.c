/*
 *      Common low level i/o routines
 *
 */

#include "ctstdr.h"             /* standard i/o header          */

#ifdef CT_ANSI
   int fseek();
        size_t fread(),fwrite();
#else
#  ifdef MSDOS
      LONG lseek(int, LONG, int);
      unsigned int read(int, void *, unsigned);
      unsigned int write(int, void *, unsigned);
      int close( int );
#  else
      LONG lseek( );
      unsigned int read( );
      unsigned int write( );
      int close( );
#  endif
#endif


COUNT ctseek(ctnum, recbyt)
PFAST CTFILE *ctnum;
POINTER            recbyt;
{
#ifdef CT_ANSI
        if (fseek(ctnum->fd,ctnum->sekpos = recbyt,SEEK_SET))
                return(SEEK_ERR);
        else
                return(NO_ERROR);
#else
        if (ctnum->sekpos == recbyt)
                return(NO_ERROR);
        else if (lseek(ctnum->fd,ctnum->sekpos = recbyt,0) < 0L)
                return(SEEK_ERR);
        else
                return(NO_ERROR);
#endif
}

COUNT ctio(op_code,ctnum,recbyt,bufadr,iosize)
COUNT  op_code; /* CTREAD or CTWRITE */
CTFILE *ctnum;
LONG      recbyt;
VOID   *bufadr;
SIZE      iosize;
{
        if (ctseek(ctnum,recbyt))
                return(uerr(SEEK_ERR));

        if (!iosize)
                iosize = ctnum->reclen;
        if (op_code == CTREAD) {

#ifdef CT_ANSI
                if (fread(bufadr,iosize,1,ctnum->fd) != 1) {
#else
                if (read(ctnum->fd,bufadr,iosize) != iosize) {
#endif
                        ctnum->sekpos = -1L;
                        return(uerr(READ_ERR));
                }
        } else {
#ifdef CT_ANSI
                if (fwrite(bufadr,iosize,1,ctnum->fd) != 1) {
#else
                if (write(ctnum->fd,bufadr,iosize) != iosize) {
#endif
                        ctnum->sekpos = -1L;
                        return(uerr(WRITE_ERR));
                }
        }
        ctnum->sekpos += iosize;
        return(NO_ERROR);
}

COUNT mbclos( ctnum,clmode)
PFAST CTFILE *ctnum;
COUNT               clmode;     /* COMPLETE or PARTIAL */
{
        if (!(ctnum->flmode & PERMANENT))
                ct_numvfil--;

   clmode = clmode; /* so para tirar warning */

#ifdef CT_ANSI
        return((COUNT) fclose(ctnum->fd));
#else
        return((COUNT) close(ctnum->fd));
#endif
}

/* end of ctclb3.c */
