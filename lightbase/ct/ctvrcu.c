/*
 *      variable record length common utility routines
 *
 */

#include "ctstdr.h"             /* standard i/o header          */

COUNT getvhdr( vnum,pntr,phdr)
PFAST VATFILE *vnum;
LONG                pntr;
VHDR                    *phdr;
{
        if (!pntr)                              return(uerr(VPNT_ERR));
        else if (vnum->clstyp == DAT_CLOSE) return(uerr(FMOD_ERR));
        else
#ifdef UNIFRMAT
            {
                ctio(CTREAD, vnum, pntr - SIZVHDR, phdr, SIZVHDR);
#ifndef LONGVARD
                revbyt(phdr,3);
#else
                revbyt(phdr,5);
                revwrd(&phdr->trclen,2);
#endif
                return(uerr_cod);
            }
#else
                return(ctio(CTREAD,vnum,pntr - SIZVHDR,phdr,SIZVHDR));
#endif
}

COUNT putvhdr( vnum,pntr,phdr)
PFAST VATFILE *vnum;
LONG                pntr;
VHDR                    *phdr;
{
        if (!pntr)
                return(uerr(VPNT_ERR));

#ifdef UNIFRMAT
        cpybuf(ct_buf,phdr,SIZVHDR);
#ifndef LONGVARD
        revbyt(ct_buf,3);
#else
        revbyt(ct_buf,5);
        revwrd(ct_buf,2);
#endif
        return(ctio(CTWRITE, vnum, pntr - SIZVHDR, ct_buf,SIZVHDR));
#else
        return(ctio(CTWRITE, vnum, pntr - SIZVHDR, phdr  ,SIZVHDR));
#endif
}


COUNT frmlkey(pkeybuf, pvlen, pntr)
PFAST TEXT *pkeybuf;
VRLEN      *pvlen;
POINTER     pntr;
{
        FAST COUNT i;
        TEXT      *ppntr, *plen;


        plen     = (TEXT *) pvlen;
        ppntr    = (TEXT *) &pntr;

#ifdef LOW_HIGH
        pkeybuf += VARI_LEN;
        for (i = 0; i++ < sizeof(POINTER); )
                *(--pkeybuf) = *ppntr++;
        for (i = 0; i++ < sizeof(VRLEN); )
                *(--pkeybuf) = *plen++;
#else
        for (i = 0; i++ < sizeof(VRLEN); )
                *pkeybuf++ = *plen++;
        for (i = 0; i++ < sizeof(POINTER); )
                *pkeybuf++ = *ppntr++;
#endif
        return(VARI_LEN);
}

COUNT chkvhdr(phdr)
PFAST VHDR   *phdr;
{
        if (phdr->recmrk == VACT_FLAG || phdr->recmrk == VDEL_FLAG ||
            phdr->recmrk == VNOD_FLAG)
                return(NO_ERROR);
        else
                return(uerr(RVHD_ERR));
}

/*
**prepara read para registro de tamanho variavel (consistencia)
*/
VRLEN prprdv(datno,recbyt,recptr,bufsiz)
PFAST COUNT  datno;
POINTER            recbyt;
TEXT                     *recptr;
VRLEN                            bufsiz;
{
        VHDR    vrhdr;
        CTFILE *tstfnm();

        uerr_cod = 0;   
        if (tstfnm(datno) == NULL)       /* no action */;
        else if (recbyt == DRNZERO)      uerr_cod = ZREC_ERR;
        else if (recptr == NULL)         uerr_cod = DNUL_ERR;
        else if (getvhdr(ct_key + datno,recbyt,&vrhdr) || chkvhdr(&vrhdr))
                /* error condition */;
        else if (vrhdr.urclen > bufsiz)  uerr_cod = VBSZ_ERR;
        else if (vrhdr.urclen == 0)
#ifdef MUSTFRCE
                uerr_cod = ITIM_ERR;
#else
                uerr_cod = VRCL_ERR;
#endif
        else if (vrhdr.recmrk != VACT_FLAG) uerr_cod = VFLG_ERR;


        if (uerr_cod)                       return(0);
        else                                return(vrhdr.urclen);
}

/* end of ctvrcu.c */

COUNT stripkey(pkeybuf, plen, pntr)
PFAST TEXT  *pkeybuf;
VRLEN       *plen;
POINTER     *pntr;
{
        FAST COUNT i;
        TEXT      *pl, *pp;

        pl    = (TEXT *) plen;
        pp    = (TEXT *) pntr;

#ifdef LOW_HIGH
        pkeybuf += VARI_LEN;
        for (i = 0; i++ < sizeof(POINTER); )
                *pp++ = *(--pkeybuf);
        for (i = 0; i++ < sizeof(VRLEN); )
                *pl++ = *(--pkeybuf);
#else
        for (i = 0; i++ < sizeof(VRLEN); )
                *pl++ = *pkeybuf++;
        for (i = 0; i++ < sizeof(POINTER); )
                *pp++ = *pkeybuf++;
#endif
        return(VARI_LEN);
}

