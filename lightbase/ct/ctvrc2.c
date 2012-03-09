/*
 *      variable record length low level routines
 *
 */

#include "ctstdr.h"             /* standard i/o header          */

COUNT          tstupd();
CTFILE        *tstfnm();


LOCAL TEXT     lenkey[VARI_LEN], idxkey[VARI_LEN];




VRLEN GTVLEN(datno,recbyt)
COUNT    datno;
POINTER recbyt;
{
        VHDR          vrhdr;
        FAST VATFILE *vnum;

        uerr_cod = 0;

        if ((vnum = tstfnm(datno)) == NULL)                     /* no action */;
        else if (vnum->clstyp != VAT_CLOSE)                     uerr_cod = FMOD_ERR;
        else if (recbyt == DRNZERO)                             uerr_cod = ZREC_ERR;
        else if (getvhdr(vnum,recbyt,&vrhdr))                   /* no action */;
        else if (vrhdr.recmrk != VACT_FLAG)                     uerr_cod = VFLG_ERR;

        if (uerr_cod)                                           return(0);
        else                                                    return(vrhdr.urclen);
}


/* NEWVREC inicialmente procura no arquivo de indices que esta no proprio
**    arquivo, por uma entrada que acomode o tamanho de registro solicitado
**    Caso nao exista o arquivo e expandido pela funcao expfil
*/


LONG NEWVREC(datno,varlen)
COUNT    datno;
VRLEN    varlen;
{
   LONG     pntr,rpntr;
   VRLEN    tamanho;
   VATFILE *vnum;
   VHDR     vrhdr;

   POINTER extfil();

   uerr_cod = 0;
   if ((vnum = tstfnm(datno)) == NULL || chkredf(vnum))
      return(DRNZERO);
   if (vnum->clstyp != VAT_CLOSE) {
      uerr(FMOD_ERR);
      return(DRNZERO);
   }

   frmlkey(lenkey, &varlen, DRNZERO);
   rpntr    = DRNZERO;

   if ((pntr = GTEKEY(datno, lenkey, idxkey, sizeof(VRLEN))) != (POINTER) 0 ){

      /* achou na free_list uma area >= a area procurada */
      /* vai retirar da lista e retornar o endereco como parametro */
      /* pntr = ve_pos(idxkey);  para compensar que funcoes de pesquisa nao */
      /* mais retornam POINTER (ANANIAS)  */

      stripkey(idxkey, &tamanho, &pntr);

      if (getvhdr(vnum, pntr ,&vrhdr))
         /* no action */ ;
      else if (vrhdr.recmrk != VDEL_FLAG)
         uerr(VDLFLG_ERR);
      else if (DELKEY(datno, idxkey, VARI_LEN) != NO_ERROR)
         uerr(VDLK_ERR);
      else
         rpntr = pntr;

      /* ananias este if e para tentar pegar um bug */
      if ( tamanho >= varlen && vrhdr.trclen >= varlen ) {
         if (tstupd(vnum))
            goto err_newvrec;

         return(rpntr);
      }
      else {
         FILE *fp;
         /* a principio nao deveria passar por aqui */
         fp = fopen("lb_warni.$$$", "a");
         fprintf(fp, "? %u ==> ind(%u)  struct(%d)\n", varlen, tamanho, vrhdr.trclen);
         fclose(fp);
         rpntr = DRNZERO;
      }
   }


   if ((varlen + SIZVHDR) < varlen) {
      uerr(VMAX_ERR);
      goto err_newvrec;
   }

/**** ANANIAS pode sumir ate a proxima marca 
   if ((ct_gsrl = ++vnum->sernum) == DRNZERO) {
      uerr_cod = OSRL_ERR;
      goto err_newvrec;
   }
*****/

   pntr = extfil(vnum, varlen + SIZVHDR);

   if (tstupd(vnum))
      goto err_newvrec;

   if (!pntr)
      goto err_newvrec;

   pntr += SIZVHDR;
   vrhdr.recmrk = VACT_FLAG;
   vrhdr.trclen = varlen;
   vrhdr.urclen = 0;
   if (putvhdr(vnum, pntr, &vrhdr))
      goto err_newvrec;

   return(pntr);

err_newvrec:

   return(ct_gsrl = DRNZERO);
}


COUNT RETVREC(datno,recbyt)
COUNT         datno;
LONG                recbyt;
{
        VATFILE *vnum;
        VHDR     vrhdr,nrhdr;
        LONG     nrecbyt;
        VRLEN    test;

        uerr_cod = 0;
        if ((vnum = tstfnm(datno)) == NULL || chkredf(vnum) || getvhdr(vnum,
            recbyt,&vrhdr) || chkvhdr(&vrhdr))
                return(uerr_cod);

        if ((nrecbyt = recbyt + vrhdr.trclen + SIZVHDR) > recbyt &&
            getvhdr(vnum,nrecbyt,&nrhdr) == NO_ERROR &&
            nrhdr.recmrk == VDEL_FLAG) {

            if ((test = vrhdr.trclen + SIZVHDR + nrhdr.trclen) >
                        vrhdr.trclen) { /* combine consecutive deleted areas */
               vrhdr.trclen = test;
               frmlkey(lenkey, &nrhdr.trclen, nrecbyt); /* talves nrecbyt Era DRNZERO */
               if (DELKEY(datno, lenkey, VARI_LEN) != NO_ERROR)
                        return(uerr(VDLK_ERR));
            }
         }

         frmlkey(lenkey, &vrhdr.trclen, recbyt);            /* era DRNZERO */
         vrhdr.recmrk = VDEL_FLAG;
         vrhdr.urclen = 0;
         if (putvhdr(vnum,recbyt,&vrhdr))
            return(uerr_cod);

/*   faz_chave(); ANANIAS */
        return(ADDKEY(datno, lenkey, recbyt, REGADD, VARI_LEN));
}

COUNT WRTVREC(datno,recbyt,recptr,varlen)
COUNT         datno;
LONG                recbyt;
VRLEN                    varlen;
TEXT                           *recptr;
{
        VATFILE *vnum;
        VHDR     vrhdr;
        VRLEN    unused;

        uerr_cod = 0;
        if ((vnum = tstfnm(datno)) == NULL || chkredf(vnum)) return(uerr_cod);
        if (!recbyt)                                       return(uerr(VPNT_ERR));
        if (recptr == NULL)                                return(uerr(DNUL_ERR));
        if (getvhdr(vnum,recbyt,&vrhdr) || chkvhdr(&vrhdr))  return(uerr_cod);
        if (vrhdr.trclen < varlen)                         return(uerr(VLEN_ERR));
        vrhdr.urclen = varlen;
        vrhdr.recmrk = VACT_FLAG;
        unused       = vrhdr.trclen - varlen;

        if (unused > (vnum->reclen + SIZVHDR) && unused > 64)
                vrhdr.trclen = varlen;
        else
                unused = 0;

        if (putvhdr(vnum,recbyt,&vrhdr))             return(uerr_cod);

        if (ctio(CTWRITE,vnum,recbyt,recptr,varlen)) return(uerr_cod);

        if (unused) {   /* reclaim space */
                recbyt       += (varlen + SIZVHDR);
                vrhdr.recmrk  = VDEL_FLAG;
                vrhdr.trclen  = unused - SIZVHDR;
                vrhdr.urclen  = 0;
                if (putvhdr(vnum,recbyt,&vrhdr))
                        return(uerr_cod);
                return(RETVREC(datno,recbyt));
        }
        return(tstupd(vnum));
}







COUNT RDVREC(datno,recbyt,recptr,bufsiz)
PFAST COUNT  datno;
POINTER          recbyt;
TEXT                    *recptr;
VRLEN                    bufsiz;
{
        VRLEN        vrclen;

        if ((vrclen = prprdv(datno,recbyt,recptr,bufsiz)) == 0)
                return(uerr_cod);

        ct_tamlast = vrclen;

        return(ctio(CTREAD,ct_key + datno,recbyt,recptr,vrclen));
}

/* end of ctvrc2.c */


