#define NO_SPARSE

#include "ctstdr.h"		/* standard i/o header 		*/
#include  <stdlib.h>

#ifdef MSDOS
   int  _CType close	 (int __handle);
   int  _Cdecl dup		 (int __handle);
#else
#  ifndef VAXMS
#     include <sys/ioctl.h>
#  endif
#endif


#ifdef MSDOS
   VOID testa_parametros(VOID);
#else
   VOID testa_parametros( );
#endif

/*
 * see if update on read only file
 */

COUNT chkredf(     ctnum)
FAST CTFILE *ctnum;
{

#ifdef CTSERVER
	ctnum  = ctnum->hmem;
#else
	ctnum -= ctnum->kmem;
#endif

	if (ctnum->flmode & READFIL)  return(uerr_cod = REDF_ERR);
	else                         	return(NO_ERROR);
}

/*
** find available ct file control blocks
*/


COUNT STPUSR()
{
	return(uerr_cod = NO_ERROR);
}


COUNT TRANSACTION(mode)
COUNT		  mode;
{
   mode = mode;
	return(uerr_cod = NO_ERROR);
}


VOID testa_parametros()
{   LONG l;
    TEXT *p;

#ifndef LOW_HIGH
#  ifdef UNIFRMAT
	   ct_ver = (LH_VER | CT_V1_0);
#  else
	   ct_ver = (HL_VER | CT_V1_0);
#  endif
#  ifndef HIGH_LOW
	   terr(200);
#  endif

#else
	ct_ver = (LH_VER | CT_V1_0);
#  ifdef  HIGH_LOW
	   terr(201);
#  endif
#endif


l=1L;
p = (TEXT *) &l;

#ifdef HIGH_LOW
	if ( *(p+3) == 0 ) {
        	fprintf(stderr, "\nArquitetura interna da maquina nao e' HIGH_LOW\nModifique ctoptn.h e ../ap/lb2maces.h");
        	exit(1);
	}
#endif


#ifdef LOW_HIGH
	if ( *p == 0 ) {
        	fprintf(stderr, "\nArquitetura interna da maquina nao e' LOW_HIGH\nModifique ctoptn.h e ../ap/lb2maces.h");
        	exit(1);
	}
#endif


#ifndef NOTFORCE
#  ifndef FPUTFGET
#     ifndef FPUTONLY
	         terr(202);
#     endif
#  endif
#endif

#ifdef  NOTFORCE
#  ifdef  FPUTFGET
	   terr(203);
#  endif
#  ifdef  FPUTONLY
	   terr(203);
#  endif
#else
#  ifdef  FPUTFGET
#     ifdef  FPUTONLY
	      terr(203);
#     endif
#  endif
#endif
}





/* --------------------------------------------------------------------
   routine to initialize buffer space and status variables
 */



COUNT INTREE(bufs,fils,sect)
COUNT bufs,fils,sect;
{
	FAST COUNT i;
	TREEBUFF  *buf;
	CTFILE    *ctnum;
	COUNT      valsiz;
	TEXT      *buforg,*valoff;

   testa_parametros(); /* se nao estiver legal cai fora */

	uerr_cod = 0;
   ct_noerror = NO_ERROR;

	if (bufs < 3 || sect < 1 || fils < 0)  return(uerr(SPRM_ERR));

	if (ct_key != NULL)                    return(uerr(IINT_ERR));

	if ((ct_btree = (TREEBUFF *) mballc(bufs,sizeof(TREEBUFF))) == NULL)
      return(uerr(SPAC_ERR));

	if ((ct_key = (CTFILE *) mballc(fils,sizeof(CTFILE))) == NULL) {
		mbfree(ct_btree);
		return(uerr(SPAC_ERR));
	}

	ct_dat = ct_key;
	ct_vat = ct_key;

	valsiz = sect * SECSIZ + MAXLEN + sizeof(POINTER);    /* ??? */

	if ((ct_origin = (TEXT *) mballc(bufs,valsiz)) == NULL) {
		mbfree(ct_key);
		ct_key = NULL;
		mbfree(ct_btree);
		return(uerr(SPAC_ERR));
	}

	valoff   = (buforg = ct_origin) + STATUS; /* ??? */
	ct_mxbuf = bufs;
	ct_mxfil = fils;
	ct_ndsec = sect;
	ct_lacs  = ct_lfil = ct_numvfil = ct_npath[0] = 0;

	for (i = 0,buf = ct_btree; i++ < ct_mxbuf; buf++) {
		buf->keyid = -1;
		buf->update = 'n';
		buf->nodacs = 0;
		buf->nodorg = buforg;
		buf->ct_kyval = valoff;
		buforg += valsiz;
		valoff += valsiz;
	}

	for (i = 0,ctnum = ct_key; i++ < ct_mxfil; ctnum++) {
		ctnum->usecnt = 0;
		ctnum->chnacs = 'n';
	}

	return(NO_ERROR);
}


/* ------------------------------------------------------------------
   extend file size
*/

POINTER extfil(ctnum, rsize)
PFAST CTFILE  *ctnum;
unsigned int         rsize;

{	/* extfil assumes that (in multi-user system) header is locked */
	POINTER retval,oldval;
	POINTER    oldphy;
	FAST COUNT i;
	COUNT   uerr();

	ctnum         -= ctnum->kmem;
	retval         = (oldval = ctnum->numrec) + 1;
	ctnum->numrec += rsize;
	oldphy	      = ctnum->phyrec;
	if (oldval > ctnum->numrec) {  /* esta pergunta parece absurda (ananias) */
		uerr(FULL_ERR);
		return(DRNZERO);
	} else if (ctnum->numrec <= ctnum->phyrec)
		return(retval);
	else if (ctnum->extsiz <= rsize) {
		ctnum->phyrec = ctnum->numrec;
		return(retval);
	} else { /* extend file by chunk and save directory contents */
		if (ctnum->phyrec < (ctnum->extsiz - 1))
			ctnum->phyrec  = ctnum->extsiz - 1;
		else
			ctnum->phyrec += ctnum->extsiz;

		if (ctnum->phyrec < ctnum->numrec) {
			/* overflow */
			ctnum->phyrec = ctnum->numrec;
			return(retval);
		}

		for (i = 0; i < CTBUFSIZ; i++)
			ct_buf[i] = DELFLG;

		for (i = (ctnum->phyrec - oldphy++) / CTBUFSIZ; i-- > 0; ) {
			if (ctio(CTWRITE,ctnum,oldphy,ct_buf,CTBUFSIZ))
				break;
			oldphy += CTBUFSIZ;
		}
		if ( (i = ctnum->phyrec - oldphy + 1) != 0 ) {
			if (uerr_cod || ctio(CTWRITE,ctnum,oldphy,ct_buf,i)) {
				/* couldn't extend by a chunk */
				uerr_cod = 0;
				ctnum->phyrec = ctnum->numrec;
				return(retval);
			}
		}

		if (mbsave(ctnum))
			return(DRNZERO);
		return(retval);
	}
}


/* --------------------------------------------------------------------
   utility to test for valid file number and determine if file
   corresponding to ctnum has been activated. returns NULL ptr if error.
 */

CTFILE *tstfnm(filno)
COUNT          filno;
{
	FAST CTFILE *retval,*ctnum;
	COUNT        nomemb;

	COUNT        uerr();
	UCOUNT	     inrfil();

	if (ct_mxfil == 0) {
		uerr(FINT_ERR);
		return(NULL);
	} else

	if (filno < 0 || ct_mxfil <= filno) { /* filno out of range */
		uerr(FNUM_ERR);
		return(NULL);
	} else
		retval = ct_key + filno;

	switch (retval->chnacs) {
case 'n':
		uerr_cod = FACS_ERR;
		return(NULL);
case 'y':
		retval->usecnt = inrfil(retval);
		return(retval);
case 'v':
case 'w':
		ctnum          = retval - retval->kmem;
		nomemb         = ctnum->nmem;
		ctnum->usecnt  = 0;
#ifdef CT_ANSI
		if ((ctnum->fd = mbopen(ctnum,ctnum->flmode)) == (RNDFILE) NULL) {
#else
		if ((ctnum->fd = mbopen(ctnum,ctnum->flmode)) < 0) {
#endif
			uerr_cod = VRTO_ERR;
			return(NULL);
		} else {
			ctnum->usecnt = inrfil(retval);
			for (filno = 0; filno++ <= nomemb; ctnum++)
				ctnum->chnacs = ctnum->chnacs == 'w' ? 'm' : 'y';
			return(retval);
		}
default:
		terr(222);
	}

   return( NULL ) ;
}


/* ------------------------------------------------------------ */

COUNT vtclose()

{
	FAST CTFILE *ctnum;
	CTFILE      *fndnum;
	FAST COUNT   i;
	UCOUNT       lstusd;

	COUNT CLSFIL();

	for (i = 0, ctnum = ct_key, fndnum = NULL, lstusd = MAXAGE;
	    i++ < ct_mxfil; ctnum++)
		if (ctnum->usecnt && ctnum->usecnt <= lstusd &&
		    ctnum->chnacs == 'y' && ctnum->kmem < 1 &&
		    !(ctnum->flmode & PERMANENT) && !ctnum->lokcnt)
			lstusd = (fndnum = ctnum)->usecnt;

	if ((ctnum = fndnum) == NULL)
		return(NO);
	else {
		if (CLSFIL(ctnum->filnum,ctnum->flmode))
				return(NO);

		ctnum->usecnt = 0;
		for (i = 0; i++ <= fndnum->nmem; ctnum++)
			ctnum->chnacs = ctnum->chnacs == 'm' ? 'w' : 'v';
		return(YES);
	}
}

/* ---------------------------------------------------------------- */

UCOUNT inrfil(ctnum)
PFAST CTFILE *ctnum;

{
	CTFILE      *calnum;
	FAST COUNT   i;
	UCOUNT       minage;

	ctnum -= (calnum = ctnum)->kmem;

	if (ctnum->flmode & PERMANENT) /* not a virtual file */
		return(0);
	else if (++ct_lfil != 0 )		/* then file age did not rollover */
		return(calnum->usecnt = ct_lfil);

	for (i = 0, ctnum = ct_key, minage = MAXAGE; i++ < ct_mxfil; ctnum++)
		if (ctnum->usecnt && ctnum->usecnt <= minage &&
		    ctnum->kmem < 1)
			minage = ctnum->usecnt;

	ct_lfil = MAXAGE - (--minage);
	for (i = 0, ctnum = ct_key; i++ < ct_mxfil; ctnum++)
		if (ctnum->usecnt)
			ctnum->usecnt -= minage;
	return(calnum->usecnt = ++ct_lfil);
}

/* --------------------------------------------------------------------
   test to see if update flag is set. if not, set it.
 */

COUNT  tstupd(ctnum)
PFAST CTFILE *ctnum;

{

#ifdef NOTFORCE
	COUNT wrthdr();

	ctnum -= ctnum->kmem;

	if (!(ctnum->updflg)) {
		ctnum->updflg = UPDATED;
		if (wrthdr(ctnum))
			return(uerr_cod);
	}
#endif


	return(NO_ERROR);
}



#ifdef UNIFRMAT
VOID revhdr(ctnum)
PFAST CTFILE *ctnum;
{
	revbyt(ctnum,8);

	revbyt(&ctnum->sernum, 24);
	revwrd(&ctnum->sernum,  7);
}
#endif

/* --------------------------------------------------------------------
   read file header record
*/

COUNT  redhdr(ctnum)
PFAST CTFILE *ctnum;
{
	FAST COUNT	i;
	COUNT		nomemb;
	POINTER		recbyt;

	if (ctio(CTREAD, ctnum, recbyt = DRNZERO, ctnum, HDRSIZ))
		return(uerr_cod);

#ifdef UNIFRMAT
	revhdr(ctnum);
#endif

	if (ctnum->clstyp == IDX_CLOSE) {
		nomemb = ctnum->nmem;

		for (i = 1; i <= nomemb; i++) {
			recbyt += HDRSIZ;
			if (ctio(CTREAD,ctnum,recbyt,ctnum + i,HDRSIZ))
				return(uerr_cod);
#ifdef UNIFRMAT
			revhdr(ctnum + i);
#endif
		}

	}
	return(NO_ERROR);
}


/* --------------------------------------------------------------------
   write file header record
 */

COUNT  wrthdr(ctnum)
PFAST CTFILE *ctnum;
{
	FAST COUNT	i;
	COUNT		nomemb;
	POINTER		recbyt;


   ctnum -= ctnum->kmem;
	nomemb = ctnum->nmem;

	for (i = 0,recbyt = 0; i <= nomemb; i++,recbyt += HDRSIZ) {
#ifdef UNIFRMAT
		cpybuf(ct_buf,ctnum + i,HDRSIZ);
		revhdr(ct_buf);
		if (ctio(CTWRITE, ctnum, recbyt, ct_buf, HDRSIZ))
			return(uerr_cod);
#else
		if (ctio(CTWRITE,ctnum,recbyt,ctnum + i,HDRSIZ))
			return(uerr_cod);
#endif
	}

	return(NO_ERROR);
}


COUNT OPNFIL(filno, filnam, filmod, num_serie, id_arq)
COUNT	filno;	/* data number temporary assigned to file */
TEXT	*filnam;	/* pointer to file name */
COUNT	filmod;  /* EXCLUSIVE/SHARED   VIRTUAL/PERMANENT */
LONG  num_serie;
UCOUNT id_arq;
{
	FAST CTFILE *ctnum,*knum;
	COUNT        opnflg;
#ifdef KEYTEST
	COUNT	     i;
#endif
	COUNT        wrthdr(),redhdr(),uerr();


	uerr_cod = opnflg = NO;
	ctnum = ct_key + filno;

	if (filno < 0 || ct_mxfil <= filno)  {
		uerr_cod = FNUM_ERR;
      goto fim;
   }

	if (ctnum->chnacs != 'n'){
		uerr_cod = FUSE_ERR;
      goto fim;
   }

	if ((filmod & NONEXCLUSIVE) == NONEXCLUSIVE){
		/* set READFIL flag if SHARED & READFIL both on */
		filmod &= ~SHARED;
   }

	strcpy(ctnum->flname,filnam);
	ctnum->usecnt = 0;
   ctnum->fd = mbopen(ctnum,filmod);

#ifdef CT_ANSI
	if ( ctnum->fd == (RNDFILE) NULL)
#else
	if ( ctnum->fd < 0)
#endif
	{
      /* does not exist */
		uerr_cod = FNOP_ERR;
      goto fim;
   }

   ct_updwarning = 0;
	opnflg = YES;

	if ( redhdr(ctnum) != NO_ERROR ) {
      goto fim;
   }

	if (ctnum->clstyp < DAT_CLOSE || ctnum->clstyp > VAT_CLOSE) {
      uerr_cod = FUNK_ERR;
      goto fim;
   }

	if (ctnum->verson != ct_ver) {
      uerr_cod = FVER_ERR;
      goto fim;
   }

   /* numeros de serie < que 200 sao consideradas bases publicas */
   if (ctnum->pega1 && ctnum->numserie != num_serie && ctnum->numserie > 200) {
      uerr_cod = SERIE_ERR;
      goto fim;
   }

   if (ctnum->pega1 && ctnum->baseid != id_arq) {
      uerr_cod = IDBASE_ERR;
      goto fim;
   }

	ct_sernum  = ctnum->sernum;
	ct_finger  = ctnum->finger;
	ct_release = (ctnum->release[0] << 8) + ctnum->release[1];

	if ( ctnum->updflg ) {
      if ( ct_ignoraupd )
         ct_updwarning = 1;
      else
         uerr_cod = FCRP_ERR;
   }


	if (uerr_cod) {
		if (opnflg) mbclos(ctnum,COMPLETE);
		goto fim;
	}

	if (ctnum->clstyp == VAT_CLOSE)
		filmod |= VLENGTH;

	if ( ctnum->flmode != filmod ) {
		if (!((ctnum->flmode = filmod) & READFIL) &&
			   wrthdr(ctnum)) {
			opnflg = uerr_cod;
			mbclos(ctnum,COMPLETE);
         uerr_cod = opnflg;
         goto fim;
		}
	}

	ctnum->filnum = filno;
	ctnum->usecnt = inrfil(ctnum);

	for (knum = ctnum; filno <= (ctnum->filnum + ctnum->nmem); filno++,knum++) {
		if (knum->chnacs != 'n' && knum->chnacs != 'm') {
			mbclos(ctnum,COMPLETE);
         uerr_cod = FUSE_ERR;
         goto fim;
		}
		knum->filnum = filno;
		if (filno > ctnum->filnum)

#ifdef CT_ANSI
			knum->fd = (RNDFILE) NULL;
#else
			knum->fd = -1;
#endif

		knum->chnacs = 'y';
		knum->retelm = knum->lokcnt = 0;
		knum->retnod = NODEZERO;
	}

   uerr_cod = NO_ERROR;

fim :

   return(uerr_cod);

}

/* --------------------------------------------------------------------
   write node contained in buffer to disk file. returns non-zero value if
   error.
 */

COUNT wrtnod( buffer )
PFAST TREEBUFF *buffer; /* pointer to buffer containing node to be
			                  written to disk */
{
	FAST KEYFILE *knum; /* key number pointer */
	COUNT membno;

/* set file pointer & member # */

	membno = (knum = ct_key + buffer->keyid)->kmem;

	if (membno > 0)
		knum -= membno;

/* check member number */

	if (buffer->bmem != membno)
		terr(230);

/* move node status info to i/o buffer */

	cpybuf(buffer->nodorg,&buffer->sucesr,STATUS);

#ifdef UNIFRMAT
	revbyt(buffer->nodorg,6);
	revwrd(buffer->nodorg,2);
#endif

/* write node to disk */

	if (ctio(CTWRITE,knum,buffer->nodeid,buffer->nodorg,knum->recsiz))
		return(uerr_cod);

	buffer->update = 'n';

	return(NO_ERROR);
}


/* --------------------------------------------------------------------
   routine to close files

      qualquer alteracao alterar FLUSHFIL()
 */

COUNT CLSFIL(filno,filmod)
COUNT filno;
COUNT filmod;	/* COMPLETE or PARTIAL */
{
	FAST COUNT     i;
	FAST CTFILE   *ctnum;
	FAST TREEBUFF *buf;
	CTFILE        *knum;
	COUNT          lowi,hghi;

	COUNT wrthdr(),wrtnod(),uerr();


	uerr_cod = 0;
	ctnum = ct_key + filno;

	if (filno < 0 || ct_mxfil <= filno) 		return(uerr(FNUM_ERR));
	else if (ctnum->chnacs == 'n')      		return(uerr(FACS_ERR));
	else if (ctnum->kmem > 0)           		return(uerr(KMEM_ERR));
	else if (ctnum->chnacs == 'v') {
      for (i = 0, knum = ctnum; i++ <= ctnum->nmem; knum++)
		   knum->chnacs = 'n';
		return(NO_ERROR);
	}

   if (ctnum->clstyp != DAT_CLOSE) {
	   lowi = ctnum->filnum;
      hghi = lowi + ctnum->nmem;
	   for (i = 0, buf = ct_btree; i < ct_mxbuf; i++, buf++) {
		   if (buf->keyid >= lowi && buf->keyid <= hghi) {
			   if (buf->update == 'y') {
				   if (ctnum->updflg) {
					   if (wrtnod(buf))
						   return(uerr_cod);
				   } else
				      terr(206);
			   }
			   buf->keyid  = -1;
			   buf->nodeid = -1;
		   }
	   }
   }

	for (i = 0,knum = ctnum; i++ <= ctnum->nmem; knum++)
		if (knum->chnacs != 'm')
         knum->chnacs = 'n';

	if (ctnum->updflg) {
      if ( ! ct_ignoraupd )
   		ctnum->updflg = NO;
		if (wrthdr(ctnum))
			return(uerr_cod);
	}

	if (mbclos(ctnum,filmod) < 0)
		return(uerr(FCLS_ERR));

	return(NO_ERROR);
}




/* --------------------------------------------------------------------
   routine to flus files  - Ananias
 */

COUNT RECALFIL(filno)
COUNT filno;
{
	FAST COUNT     i;
	FAST TREEBUFF *buf;
	FAST DATFILE *dnum, *knum;

   if ( ct_semseguranca )
   	return(NO_ERROR);

   if ( ! MULTI_USER ) {
      return(0);
   }

	if ((dnum = tstfnm(filno)) == NULL )
		return(-1);

   /* limpa todos os buffers */
   for (i = 0, buf = ct_btree; i < ct_mxbuf; i++, buf++) {
      buf->nodeid = -1;
      if ( buf->update != 'n' ) {
         TEXT lixo[10];
		   if (buf->update == 'y') {
			   if (dnum->updflg) {
				   if (wrtnod(buf))
					   return(uerr_cod);
			   } else
				   terr(206);
		   }
         fprintf(stderr, "WWW  Buffer atualizado sera desprezado\n");
         gets(lixo);
      }

      buf->update = 'n';
   };

   if ( redhdr(dnum) )
      return(-1);

	dnum->retelm = dnum->lokcnt = 0;
	dnum->retnod = NODEZERO;

	for (knum = dnum; filno <= (dnum->filnum + dnum->nmem); filno++,knum++) {
   	knum->retelm = knum->lokcnt = 0;
   	knum->retnod = NODEZERO;
   }

 	return(0);
}


COUNT FLUSHFIL(filno)
COUNT filno;
{
	FAST COUNT     i;
	FAST CTFILE   *ctnum;
	FAST TREEBUFF *buf;
	COUNT          lowi,hghi;

	COUNT wrthdr(),wrtnod(),uerr();

	if ( ct_semseguranca )
   	return(NO_ERROR);


	uerr_cod = 0;
	ctnum = ct_key + filno;

	if (filno < 0 || ct_mxfil <= filno) 		return(uerr(FNUM_ERR));
	else if (ctnum->chnacs == 'n')      		return(uerr(FACS_ERR));
	else if (ctnum->kmem > 0)           		return(uerr(KMEM_ERR));
	else if (ctnum->chnacs == 'v') {
		return(NO_ERROR);
	}


   lowi = ctnum->filnum;
   hghi = lowi + ctnum->nmem;

   for (i = 0, buf = ct_btree; i < ct_mxbuf; i++, buf++) {
	   if (buf->keyid >= lowi && buf->keyid <= hghi) {
		   if (buf->update == 'y') {
			   if (ctnum->updflg) {
				   if (wrtnod(buf))
					   return(uerr_cod);
			   } else
				   terr(206);
		   }
         if ( ct_maxuser > 1 )
   	      buf->nodeid = -1;
	   }
   }

   /* forca gravacao do header para atualizar tamanho do arquivo */
   /* e necessario para rebuild pegar toto o arquivo */

		if (ctnum->updflg) {
			ctnum->updflg = NO;
			if (wrthdr(ctnum))
				return(uerr_cod);
		}

#ifdef CT_ANSI
		fflush(ctnum->fd);
#else
		close(dup(ctnum->fd));
#endif


	return(NO_ERROR);
}

COUNT DELFILE(filnam)
TEXT	    *filnam;
{
	if (remove(filnam))
		return(uerr_cod = DLTF_ERR);
	else
		return(NO_ERROR);
}

COUNT RENFILE(oldname, newname)
TEXT	    *oldname;
TEXT	    *newname;
{  COUNT ret;

#ifdef MSDOS
   ret = rename(oldname, newname);
   return( ret );
#else
   char buffer[128];
   strcpy(buffer,"mv ");
   strcat(buffer,oldname);
   strcat(buffer," ");
   strcat(buffer,newname);
   return(system(buffer));
#endif
}


COUNT ATUFILE(filno, num_serie, id_arq)
COUNT	filno;	/* data number temporary assigned to file */
LONG  num_serie;
UCOUNT id_arq;
{  COUNT ret;
	CTFILE *ctnum;
   CTFILE base;
   TEXT *p1, *p2;


   ret = 0;

   if ( filno >= 0 ) {  /* arquivo ja aberto */
	   ctnum = ct_key + filno;

	   if (filno < 0 || ct_mxfil <= filno)  {
		   ret = -1;
         goto fim;
      }

	   ctnum->numserie= num_serie;
	   ctnum->baseid  = id_arq;
		ctnum->pega1   = (ctnum->baseid != 0) ? 1 : 0;
      ctnum->updflg  = 'y';
   }
   else {

      p1 = (TEXT *)&base;
      p2 = (TEXT *)&base.numserie;

      ret = p2 - p1;
   }

fim :
   return(ret);
}

LONG PEGA_SERIE(filno)
COUNT	filno;	/* data number temporary assigned to file */
{  LONG ret;
	CTFILE *ctnum;


   ret = 0L;

   if ( filno >= 0 ) {  /* arquivo ja aberto */
	   ctnum = ct_key + filno;
      ret = ctnum->numserie;
   }

fim :

   return(ret);
}

/* end of ctinit.c */
