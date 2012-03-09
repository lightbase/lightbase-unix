/*
 *	data file routines
 *
 */

#include "ctstdr.h"		/* standard i/o header 		*/

POINTER NEWREC(datno)
COUNT datno;
{
	FAST DATFILE *dnum;
	LOCAL POINTER pntr;

	DATFILE *tstfnm();
	POINTER  extfil();

	uerr_cod = 0;
	if ((dnum = tstfnm(datno)) == NULL || chkredf(dnum))
		return(DRNZERO);
	if (dnum->clstyp != DAT_CLOSE) {
		uerr(FMOD_ERR);
		return(DRNZERO);
	}


	if ((ct_gsrl = ++dnum->sernum) == DRNZERO) {
		uerr_cod = OSRL_ERR;
		goto err_newrec;
	}

	if ((pntr = dnum->delstk) == 0) {
		if ((pntr = extfil(dnum,dnum->reclen)) == 0) {
			/* could not extend file */
			goto err_newrec;
		}
	} else {
		if (ctio(CTREAD,dnum,pntr,ct_buf,sizeof(POINTER) + 1)) {
			goto err_newrec;
		}
		if (ct_buf[0] != DELFLG) {
			uerr(DELFLG_ERR);
			goto err_newrec;
		}
#ifdef UNIFRMAT
		revobj(ct_buf + 1,sizeof(POINTER));
#endif
		cpybuf(&dnum->delstk,ct_buf + 1,sizeof(POINTER));
	}

	if (tstupd(dnum))
		goto err_newrec;

	return(pntr);

err_newrec:
	return(ct_gsrl = DRNZERO);
}

COUNT RETREC(datno,recbyt)
COUNT   datno;
POINTER recbyt;
{
	FAST DATFILE *dnum;
	LOCAL POINTER pntr;

	DATFILE *tstfnm();
	COUNT    tstrec();

	uerr_cod = 0;
	if ((dnum = tstfnm(datno)) == NULL || chkredf(dnum) ||
	    tstrec(dnum,recbyt))
		return(uerr_cod);
	if (dnum->clstyp != DAT_CLOSE)
		return(uerr(FMOD_ERR));
	

	pntr = dnum->delstk;
	if (pntr == recbyt) {
		uerr_cod = DDRN_ERR;
		goto err_retrec;
	}
	dnum->delstk = recbyt;
	ct_buf[0] = DELFLG;
	cpybuf(ct_buf + 1,&pntr,sizeof(POINTER));
#ifdef UNIFRMAT
	revobj(ct_buf + 1,sizeof(POINTER));
#endif

	if (ctio(CTWRITE,dnum,recbyt,ct_buf,sizeof(POINTER) + 1)) {
		goto err_retrec;
	}

	if (tstupd(dnum))
		goto err_retrec;

	return(NO_ERROR);

err_retrec:
	return(uerr_cod);
}	



/* --------------------------------------------------------------------
   read data record
*/

COUNT REDREC(datno,recbyt,recptr)
COUNT	datno;
POINTER	recbyt;
TEXT   *recptr;
{
	FAST DATFILE *dnum;

	DATFILE *tstfnm();
	COUNT    tstrec();

	uerr_cod = 0;
	if ((dnum = tstfnm(datno)) == NULL || tstrec(dnum,recbyt))
		return(uerr_cod);
	if (recptr == NULL)
		return(uerr(DNUL_ERR));
	return(ctio(CTREAD,dnum,recbyt,recptr,0));
}

COUNT WRTREC(datno,recbyt,recptr)
COUNT	datno;
POINTER	recbyt;
TEXT   *recptr;
{
	FAST DATFILE *dnum;

	DATFILE *tstfnm();
	COUNT    tstrec();

	uerr_cod = 0;
	if ((dnum = tstfnm(datno)) == NULL || chkredf(dnum) ||
	    tstrec(dnum,recbyt) || tstupd(dnum))
		return(uerr_cod);
	if (recptr == NULL)
		return(uerr(DNUL_ERR));
	return(ctio(CTWRITE,dnum,recbyt,recptr,0));
}


POINTER LSTREC(datno)
COUNT datno;
{
	FAST DATFILE *dnum;

	DATFILE *tstfnm();
	POINTER  extfil();

	uerr_cod = 0;
	if ((dnum = tstfnm(datno)) == NULL )
		return(DRNZERO);
	if (dnum->clstyp != DAT_CLOSE) {
		uerr(FMOD_ERR);
		return(DRNZERO);
	}

   return(dnum->sernum);
}



COUNT tstrec(dnum,recbyt)
PFAST DATFILE  *dnum;
POINTER		recbyt;
{
	if (!recbyt)
		return(uerr(ZREC_ERR));

	if (recbyt > dnum->numrec)
		return(uerr(LEOF_ERR));

	return(NO_ERROR);
}



/* -------------------------------------------------------------------
   lock and unlock data records: lokmod: FREE(0) / ENABLE(2) / READREC
 */

COUNT LOKREC(datno,lokmod,recbyt)
COUNT	     datno,lokmod;
POINTER			  recbyt;
{
	CTFILE      *tstfnm();
	uerr_cod = 0;

   datno  = datno;    /* so para tirar warning */
   lokmod = lokmod;
   recbyt = recbyt;
	return(NO_ERROR);
}

/* end of ctdatf.c */
