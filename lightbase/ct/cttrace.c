/*
 *
 *	rebuild support routines
 *
*/


#ifdef MSDOS
#include  <alloc.h>     /* so vale para DOS */
#else
#  ifdef VAXVMS
#     include <string.h>
#else
#     include <memory.h>
#  endif
#endif

#include "ctstdr.h"		/* standard i/o header 		*/


COUNT CTFNUM( datno )
COUNT datno;
{  FAST DATFILE *dnum;

	dnum = ct_key + datno;

	if (dnum->chnacs != 'y') {
      uerr_cod = FUSE_ERR;
      return(-1);
   }

	return(dnum->fd);
}


COUNT MANUT_OPEN( datno, nome, tipo, num_serie, id_arq)
COUNT datno;
TEXT  *nome;
COUNT tipo;
LONG  *num_serie;
UCOUNT *id_arq;
{  FAST DATFILE *dnum;
   COUNT corrompido;

	uerr_cod = 0;

	dnum = ct_key + datno;

	if (dnum->chnacs != 'n') {
      uerr_cod = FUSE_ERR;
      goto fim;
   }

	cpybuf(dnum->flname, nome, MAX_NAME);

#ifdef CT_ANSI
	if ((dnum->fd = mbopen(dnum,(EXCLUSIVE | PERMANENT))) == (RNDFILE) NULL)
#else
	if ((dnum->fd = mbopen(dnum,(EXCLUSIVE | PERMANENT))) < 0)
#endif

	{
      /* file does not exist */
      uerr_cod = FNOP_ERR;
      goto fim;
   }

	if (redhdr(dnum)) {
		mbclos(dnum, COMPLETE);
      uerr_cod = READ_ERR;
      goto fim;
	};

	ct_sernum  = dnum->sernum;
	ct_finger  = dnum->finger;
	ct_release = (dnum->release[0] << 8) + dnum->release[1];



   /* exclusivo para retornar numeros de serie/id_arq VIDE exemplo lst_ind.c*/
   if ( *num_serie == 0L && *id_arq == 999) {
      *num_serie = dnum->numserie;
      *id_arq    = dnum->baseid;
		mbclos(dnum, COMPLETE);
      uerr_cod  = IDBASE_ERR;
      goto fim;
   }


	if (tipo == 'F' && dnum->clstyp != DAT_CLOSE ) {
      uerr_cod = FUNK_ERR;
      goto fim;
   }

	if (tipo == 'V' && dnum->clstyp != VAT_CLOSE ) {
      uerr_cod = FUNK_ERR;
      goto fim;
   };

	if ( dnum->verson != ct_ver) {
   	uerr_cod = FVER_ERR;
      goto fim;
	}

	corrompido = ( dnum->updflg ) ? 1 : 0;


	dnum->chnacs = 'y';
	dnum->filnum = datno;
	dnum->nmem   = dnum->kmem = 0;
   dnum->clstyp = (dnum->flmode & VLENGTH) ? VAT_CLOSE : DAT_CLOSE;
	dnum->verson = ct_ver;
	dnum->updflg = UPDATED;
	if (dnum->clstyp == VAT_CLOSE) {
      /* dnum->root    = dnum->nodstk = NODEZERO; */
      /* dnum->length  = VARI_LEN; */
      /* dnum->ktype   = 0;        */

	   if (dnum->recsiz > (ct_ndsec * SECSIZ) ||
	       dnum->length < 0                   ||
          dnum->length > MAXLEN ) {
         uerr_cod = FUNK_ERR;
         goto fim;
      }

		dnum->maxkbn = dnum->recsiz - STATUS;
		dnum->maxkvn = dnum->maxkbn / (dnum->length + sizeof(POINTER));
		dnum->maxkvl = dnum->maxkbn / dnum->length;
		dnum->maxkbl = (dnum->maxkvl - 1) * dnum->length;
   }

   /* testa controle */

   if ( *id_arq == 0 && *num_serie == 0L ) {
      *num_serie = dnum->numserie;
      *id_arq    = dnum->baseid;
      uerr_cod  = IDBASE_ERR;
      goto fim;
   }
   else {
      if ( dnum->baseid != 0 &&
           !(*num_serie == dnum->numserie || *id_arq == dnum->baseid )) {
         uerr_cod  = IDBASE_ERR;
         goto fim;
      }
   }

	if ( corrompido ) {
      uerr_cod = FCRP_ERR;
      goto fim;
   }

fim :

	return(uerr_cod);
}


COUNT REG_INFO(datno, pos, tamanho, livre, proximo, status)
COUNT    datno;
POINTER  pos;
UCOUNT  *tamanho, *livre;
POINTER *proximo;
COUNT   *status;
{  FAST DATFILE *dnum;
   COUNT ret;
   UCOUNT lost;
	POINTER curbyt, inicio;
	VHDR    vrhdr;

	dnum = ct_key + datno;
   ret = NO_ERROR;

   if (dnum->clstyp == DAT_CLOSE) {

      if ( pos == (POINTER) 0 ) {
         *proximo = 1;
         *tamanho = 0;
         *livre = 0;
         ret = NO_ERROR;
         goto fim;
      }

      inicio = ((SECSIZ + dnum->reclen - 1) / dnum->reclen) * dnum->reclen;
      curbyt = inicio + ( ( pos - 1 ) * dnum->reclen);
#ifdef NUMREC_DESATUALIZADO
      if ( curbyt > dnum->numrec ) {
         ret = NO_ERROR; /* fim logico do arquivo */
         *livre   = 0;
         *tamanho = 0;
         *proximo = (POINTER) 0;
         goto fim;
      }
#endif
      *tamanho = dnum->reclen;
      *livre   = 0;
      *status  = VACT_FLAG;

#ifdef NUMREC_DESATUALIZADO
      if ( curbyt + *tamanho > dnum->numrec )
         *proximo = (POINTER) 0;
      else
         *proximo = pos + 1;
#else
         *proximo = pos + 1;
#endif

	} else if (dnum->clstyp == VAT_CLOSE) {

      if ( pos == (POINTER) 0 ) {
         *proximo  = dnum->recsiz + SIZVHDR;
         *tamanho = 0;
         *livre = 0;
         ret = NO_ERROR;
         goto fim;
      }
      else                      curbyt = pos;

      ret = getvhdr(dnum, curbyt, &vrhdr);

      if ( ret != NO_ERROR ) {
         ret = READ_ERR;
         goto fim;
      }

      switch ( vrhdr.recmrk ) {

         case VACT_FLAG :  /* registro ativo */
                           *livre   = ( vrhdr.trclen - vrhdr.urclen );
                           *tamanho = vrhdr.urclen;
                           *status  = VACT_FLAG;
                           break;

		   case VDEL_FLAG :  /* registro deletado */
                           *status  = VDEL_FLAG;
                           *livre = vrhdr.trclen;
                           *tamanho = 0;
                           break;

		   case VNOD_FLAG :  /* Registro de controle de espacos disponiveis */
                           *status  = VNOD_FLAG;
                           *livre = vrhdr.trclen;
                           *tamanho = 0;
                           break;

         default        :	/* header de registro variavel invalido */
                           /* tenta localizar um proximo */

                           if ( acha_prox(dnum, &lost, &curbyt) == 0 ) {
                              *status  = VNOD_FLAG;
                              *livre   = lost;
                              *tamanho = 0;
                              vrhdr.trclen = 0;
                              vrhdr.recmrk = 0; /* tipo invalido */
                              ret = RVHD_ERR;
                           }
                           else {
                              ret = LEOF_ERR; /* arquivo terminou fisicamente */
                              *status  = VNOD_FLAG;
                              *livre   = lost;
                              *tamanho = 0;
                              *proximo = (POINTER) 0;
                              goto fim;
                           }
               			   break;
      }

		curbyt  += (vrhdr.trclen + SIZVHDR);

      if ( curbyt > dnum->numrec )
         *proximo = (POINTER) 0;
      else
         *proximo = curbyt;

	} else
	    return(RMOD_ERR);

fim :

	   return( ret );
}


COUNT REG_READ(datno, pos, tamanho, buf)
COUNT    datno;
POINTER  pos;
UCOUNT   tamanho;
TEXT    *buf;
{  FAST DATFILE *dnum;
   COUNT ret;
   POINTER inicio;

	dnum = ct_key + datno;

   if (dnum->clstyp == DAT_CLOSE) {
      inicio = ((SECSIZ + dnum->reclen - 1) / dnum->reclen) * dnum->reclen;
      pos    = inicio + ( ( pos - 1 ) * dnum->reclen);
   }

   ret = ctio(CTREAD, dnum, pos, buf, tamanho);

   return( ret );
}

COUNT acha_prox( dnum, lost, curbyt )
DATFILE *dnum;
UCOUNT  *lost;
POINTER *curbyt;
{  COUNT achou, ret;
   UCOUNT perdeu;
   TEXT ant, ativo, delete, disp;
   POINTER pos;
   TEXT buf[10];

   achou = ant = perdeu = 0;
   pos = *curbyt;
   pos -= SIZVHDR;

   ativo  = (TEXT )VACT_FLAG;
   delete = (TEXT )VDEL_FLAG;
   disp   = (TEXT )VNOD_FLAG;

   while ( ! achou ) {

      ret = ctio(CTREAD, dnum, pos++, buf, 1);

      if ( ret != NO_ERROR ) {
         break;
      }

      perdeu++;

      if ( buf[0] == ativo  && ant == ativo ) achou = 1;
      if ( buf[0] == delete && ant == delete) achou = 1;
      if ( buf[0] == disp   && ant == disp  ) achou = 1;

      ant = buf[0];

   }

   if ( achou ) {
      perdeu -= 2;
      pos    -= 2;
   }

   *lost   = perdeu;
   *curbyt = pos;

   return( ret );
}


/* end of ctrbl2.c */
