/*
 *	file creation
 *
 *	Version 4.3    Release B
 *			May 12, 1988 17:30
 *
 */

#include "ctstdr.h"		/* standard i/o header 		*/
#include  "ctdefs.h"

#include  <string.h>

/* CREDAT creates the data file with name pointed to by filnam,
   and assigns datno to the data file.
 */

COUNT CREDAT(datno, filnam, datlen, xtdsiz, filmod, num_serie, id_arq)

COUNT  datno;  	/* data number temporary assigned to file 	*/
TEXT  *filnam; 	/* pointer to data file name 			*/
UCOUNT datlen; 	/* minimum record length 			*/
UCOUNT xtdsiz;	   /* how many bytes to extend file		*/
COUNT  filmod;	   /* file mode parameter (VIRTUAL, VLENG or PERMANENT) 	*/
LONG   num_serie;
UCOUNT  id_arq;
{
	FAST CTFILE *ctnum;
	LONG	     temp;
	COUNT mbclos();
	COUNT wrthdr();
	RNDFILE mbopen(),mbcrat();
   COUNT i;

   i = VARI_LEN;
   i += 2; /* ANANIAS.   gasta 2 bytes para compactacao */

	uerr_cod = 0;
	ctnum    = ct_key + datno;
	filmod  &= ~NONEXCLUSIVE;
	if (datno < 0 || ct_mxfil <= datno)
		uerr_cod = FNUM_ERR;

#ifdef CTSERVER
	else if (ctnum->chnacs != 'c')
		terr(993);
#else
	else if (ctnum->chnacs != 'n')
		uerr_cod = FUSE_ERR;
#endif

	else if (filmod & VLENGTH) {

#ifdef VARLDATA
	  	ctnum->recsiz = (ct_ndsec < 2 ? 1 : 2) * SECSIZ;
		ctnum->maxkbn = ctnum->recsiz - STATUS;
	  	ctnum->maxkvn = ctnum->maxkbn / (i + sizeof(POINTER));
	  	if (ctnum->maxkvn < 3)
			uerr_cod = KMIN_ERR;
	   else {
			ctnum->maxkvl = ctnum->maxkbn / i;
			ctnum->maxkbl = (ctnum->maxkvl - 1) * i;
		}
#else
		uerr_cod = FTYP_ERR;
#endif

	} else if (datlen < (sizeof(POINTER) + 1))
		uerr_cod = DREC_ERR;

	if (!uerr_cod) {
	    ctnum->reclen = datlen;
	    ctnum->filnum = datno;
	    ctnum->flmode = filmod;
	    ctnum->usecnt = 0;
	    strcpy(ctnum->flname,filnam);

#ifdef CT_ANSI
	    if ((ctnum->fd = mbopen(ctnum,filmod)) != (RNDFILE) NULL) {
#else
	    if ((ctnum->fd = mbopen(ctnum,filmod)) >= 0) {/* file exists */
#endif

		mbclos(ctnum,filmod);
		uerr_cod = DOPN_ERR;

#ifdef CT_ANSI
	    } else if ((ctnum->fd = mbcrat(ctnum)) == (RNDFILE) NULL)
#else
	    } else if ((ctnum->fd = mbcrat(ctnum)) < 0)
#endif

		uerr_cod = DCRAT_ERR;
	}

	if (uerr_cod)
		return(uerr_cod);
	else {
		ctnum->chnacs  = 'y';
		ctnum->delstk  = DRNZERO;
		ctnum->nmem    = ctnum->kmem = 0;
		if (filmod & VLENGTH) {
			ctnum->numrec = ctnum->recsiz - 1;
			ctnum->clstyp = VAT_CLOSE;
			ctnum->length = VARI_LEN;
		} else {
			ctnum->numrec = (((SECSIZ + datlen - 1) / datlen) *
				datlen) - 1;
			ctnum->clstyp = DAT_CLOSE;
			ctnum->recsiz = ctnum->length = 0;
		}
		ctnum->phyrec  = ctnum->numrec;
		ctnum->extsiz  = xtdsiz;
		ctnum->updflg  = NO;
		ctnum->verson  = ct_ver;
		ctnum->nodstk  = ctnum->root   = NODEZERO;
		ctnum->ktype   = 0;
		ctnum->sernum  = ct_sernum;
		ctnum->numserie= num_serie;
		ctnum->baseid  = id_arq;
		ctnum->pega1   = (ctnum->baseid != 0) ? 1 : 0;

		ctnum->finger      = ct_finger;
		ctnum->release[0]  = (ct_release & 0xFF00) >> 8 ;
		ctnum->release[1]  =  ct_release & 0xFF;



		if ((temp = ct_mxbuf * (LONG) ct_ndsec * SECSIZ) > ctnum->numrec)
			temp = ctnum->numrec + 1;
		ctio(CTWRITE,ctnum,DRNZERO,ct_origin,(UCOUNT) temp);
		if (wrthdr(ctnum))
			return(uerr_cod);

		ctnum->retelm = ctnum->lokcnt = 0;
		ctnum->retnod = NODEZERO;
		ctnum->usecnt = inrfil(ctnum);
		return(NO_ERROR);
	}
}


/* CREIDX creates the index file with name pointed to by filnam,
   assigns keyno to the index file

   keytip :    0 - compacta
               1 - nao compacta ( com problemas )

 */

COUNT CREIDX(keyno, filnam, keylen, keytyp, nomemb, xtdsiz, filmod, num_serie, id_arq)

COUNT  keyno;  	/* key number temporary assigned to index file 		*/
TEXT  *filnam; 	/* pointer index file name 				*/
COUNT  keylen; 	/* key length 						*/
COUNT  keytyp;  /* key type indicator */
COUNT  nomemb;	/* number of additional indices in file			*/
UCOUNT xtdsiz;	/* file extension size					*/
COUNT  filmod;	/* file mode (VIRTUAL or PERMANENT)			*/
LONG   num_serie;
UCOUNT  id_arq;
{
	FAST KEYFILE *knum;
	UCOUNT        i;

	COUNT mbclos();
	COUNT wrthdr();
	RNDFILE mbopen(),mbcrat();

	uerr_cod = 0;

   if ( keytyp != COMPACTA ) {
      fprintf(stderr, "Tratamento de chaves nao compactadas com problema");
		terr(789);
   }

	if (nomemb < 0 || nomemb > MAXMEMB)
		return(uerr(KMEM_ERR));

	if (keyno < 0 || ct_mxfil <= (keyno + nomemb))
		return(uerr(FNUM_ERR));

#ifdef CTSERVER
	for (knum = ct_key + keyno, i = 0; i <= nomemb; i++,knum = knum->xmem)
		if (knum->chnacs != 'c')
			terr(992);
#else
	for (knum = ct_key + keyno, i = 0; i <= nomemb; i++,knum++)
		if (knum->chnacs != 'n')
			return(uerr(FUSE_ERR));
#endif

	filmod	    &= ~NONEXCLUSIVE;
	knum         = ct_key + keyno;
	knum->kmem   = 0;
	knum->recsiz = ct_ndsec * SECSIZ;
	knum->maxkbn = knum->recsiz - STATUS;
	i            = keylen;

   if ( keytyp == COMPACTA )
      i += 2;               /* total e cumun */

         /* o primeiro byte adicional armazenara o tamanho da chave */
         /* o segundo o tamanho da parte comun */

         /* Em NOS INTERMEDIARIOS cada chave e composta por :
            ENDERECO + TOTAL + COMUM + CHAVE
         */

	knum->maxkvn = knum->maxkbn / (i + sizeof(POINTER)); /* isto limita???*/

	if       (knum->maxkvn < 3)     uerr_cod = KMIN_ERR;
	else if  (keylen > MAXLEN)      uerr_cod = KLEN_ERR;

	else {
      knum->maxkvl = knum->maxkbn / i;
      knum->maxkbl = (knum->maxkvl - 1) * i; /* Armazena chave MOR */

      knum->flmode = filmod;
      knum->usecnt = 0;
      strcpy(knum->flname,filnam);

#ifdef CT_ANSI
      if ((knum->fd = mbopen(knum,filmod)) != (RNDFILE) NULL) {
#else
	   if ((knum->fd = mbopen(knum,filmod)) >= 0) {/* file exists */
#endif

         mbclos(knum,filmod);
         uerr_cod = KOPN_ERR;
#ifdef CT_ANSI
      } else if ((knum->fd = mbcrat(knum)) == (RNDFILE) NULL)
#else
      } else if ((knum->fd = mbcrat(knum)) < 0)
#endif
		uerr_cod = KCRAT_ERR;
	}

	if (uerr_cod)
		return(uerr_cod);
	else {
		knum->nmem    = nomemb;
		knum->chnacs  = 'y';
		knum->filnum  = keyno;
		knum->nodstk  = knum->root   = NODEZERO;
		knum->length  = keylen;
		knum->ktype   = keytyp;
		knum->sernum  = DRNZERO;
		knum->extsiz  = xtdsiz;
		i             = (HDRSIZ * (nomemb + 1)) + knum->recsiz - 1; /* ??? */
		knum->numrec  = knum->phyrec = ((i / knum->recsiz) * knum->recsiz) - 1; /* ??? */
		knum->updflg  = NO;
		knum->verson  = ct_ver;
		knum->clstyp  = IDX_CLOSE;
		knum->numserie= num_serie;
		knum->baseid  = id_arq;
		knum->pega1   = (knum->baseid != 0) ? 1 : 0;

		ctio(CTWRITE,knum,DRNZERO,ct_origin,(UCOUNT) (knum->numrec + 1));
		if (wrthdr(knum))
			return(uerr_cod);

		knum->retelm = knum->lokcnt = 0;
		knum->retnod = NODEZERO;
		knum->usecnt = inrfil(knum);
#ifdef CTSERVER
		for (i = 1; i++ <= nomemb; ) {
			knum         = knum->xmem;
			knum->chnacs = 'm';
		}
#else
		for (i = 1; i++ <= nomemb; )
			(++knum)->chnacs = 'm';
#endif
		return(NO_ERROR);
	}
}

COUNT CREMEM(keyno, keylen, keytyp, membno, num_serie, id_arq)

COUNT  keyno;   /* key number temporary to master index file */
COUNT  keylen;  /* key length */
COUNT  keytyp;  /* key type indicator */
COUNT  membno;  /* member number of index in master file	*/
LONG   num_serie;
UCOUNT  id_arq;
{
	FAST KEYFILE *knum;
	COUNT         i;

	COUNT   wrthdr();
	CTFILE *tstfnm();

   if ( keytyp != COMPACTA ) {
      fprintf(stderr, "Tratamento de chaves nao compactadas com problema");
		terr(789);
   }

	uerr_cod = 0;
	if ((knum = tstfnm(keyno)) == NULL)
		/* no action */ ;
	else if (knum->clstyp != IDX_CLOSE)
		uerr_cod = FMOD_ERR;
	else if (membno <= 0 || membno > knum->nmem)
		uerr_cod = KMEM_ERR;
#ifdef CTSERVER
	else {
		for (i = 1; i <= membno; i++)
			knum = knum->xmem;
		if (knum->chnacs != 'm')
			uerr_cod = FUSE_ERR;
	}
#else
	else if ((knum += membno)->chnacs != 'm')
		uerr_cod = FUSE_ERR;
#endif
	if (uerr_cod)
		return(uerr_cod);

	knum->nmem   = -1;
	knum->kmem   = membno;
#ifdef CT_ANSI
	knum->fd     = (RNDFILE) NULL;
#else
	knum->fd     = -1;
#endif
	knum->recsiz = ct_ndsec * SECSIZ;
	knum->maxkbn = knum->recsiz - STATUS;
	i            = keylen;

   if ( keytyp == COMPACTA )
      i += 2;               /* total e cumun */

     /* vide comentario em CREIDX */

	knum->maxkvn = knum->maxkbn / (i + sizeof(POINTER));

	if (knum->maxkvn < 3) 		return(uerr(KMIN_ERR));
	else if (keylen > MAXLEN)  return(uerr(KLEN_ERR));


#ifndef FLOATKEY
	else if (keytyp == SFLOATKEY || keytyp == DFLOATKEY)
		return(uerr(KTYP_ERR));
#endif

	knum->maxkvl = knum->maxkbn / i;
	knum->maxkbl = (knum->maxkvl - 1) * i;

	knum->flmode  = -1;
	knum->usecnt  = 0;
	knum->chnacs  = 'y';
#ifdef CTSERVER
	knum->filnum  = knum - ct_key;
#else
	knum->filnum  = keyno + membno;
#endif
	knum->nodstk  = knum->root   = NODEZERO;
	knum->length  = keylen;
	knum->ktype   = keytyp;
	knum->sernum = DRNZERO;
	knum->extsiz  = -1;
	knum->numrec  = knum->phyrec = - 1;
	knum->updflg  = NO;
	knum->verson  = ct_ver;
	knum->clstyp  = IDX_CLOSE;
	knum->numserie= num_serie;
	knum->baseid  = id_arq;
	knum->pega1   = (knum->baseid != 0) ? 1 : 0;


	if (wrthdr(knum))
		return(uerr_cod);

	knum->retelm = knum->lokcnt = 0;
	knum->retnod = NODEZERO;
	return(NO_ERROR);
}

/* end of ctkeyf.c */
