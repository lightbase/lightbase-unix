#include  "lb2all.h"

#include  <string.h>
#include  <time.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2trans.h"

#include  "lb2vista.h"
#include  "lb2entd.h"

#ifdef    VAXVMS
#include  "mmenu.h"
#else
#include  "../mt/menu.h"
#endif


#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) \
             / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + \
          ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#define   POS_TO_NUM(pos)  (((pos - (POINTER)OFF_AC) / (POINTER) sizeof(Reg_controle)) + (POINTER) 1)


#ifdef MSDOS
   COUNT expa_registro(COUNT);  /* em lb2ent3.c */

   COUNT pack_registro(COUNT);
   VOID  dup_reg(COUNT);
   VOID  tr_aclear(COUNT, COUNT);
   VOID  inic_numericos(COUNT, COUNT);
   COUNT reg_updated(COUNT);
   COUNT fx_wrtexto( RNDFILE, VRLEN);
#else
   COUNT expa_registro( );

   COUNT pack_registro( );
   VOID  dup_reg( );
   VOID  tr_aclear( );
   VOID  inic_numericos( );
   COUNT reg_updated( );
   COUNT fx_wrtexto( );
#endif


/*----------------------------------------------------------------------*/
/* NEW_RECORD                                                           */
/*----------------------------------------------------------------------*/
F__GLB   LONG new_record(sele, padrao)
COUNT sele;
COUNT padrao;
{  COUNT ret;
   COUNT ret_trans;
   LONG record;
   POINTER pos;

   record  = 0L;
   ret     = 0;

   free_record( sele );


   if ( (ret_trans = BEG_TRANS(TR_NEWRECORD, 0L, 0)) < 0 ) {
      ret = -1;
      goto fim;
   }


   pos = FRSKEY(AI_NUM(sele) + TREE_DELETADOS , ap_chave_ret);

   if ( uerr_cod != 0 ) {
      ret = -3;
      goto fim;
   }

   if ( pos == (POINTER) 0) {
      if ( (pos = NEWREC(AC_NUM(sele))) == (POINTER)0 ) {
         ret = -3;
         goto fim;
      };
      record = (LONG)POS_TO_NUM(pos);
   }

   else {

      record = atol(ap_chave_ret);
      pos = NUM_TO_POS(record);

      if ( pal_d_index(sele, TREE_DELETADOS,
                  (UTEXT *)ap_chave_ret, strlen(ap_chave_ret) + 1) != 0 ) {
         ret = -3;
         goto fim;
      }

   }

fim :

   c_bases[sele]->cb_adlock = (POINTER) 0;

   if ( ret == 0 ) {
      c_bases[sele]->cb_numrec = record;
      c_bases[sele]->cb_aclock = pos;

      switch(padrao) {
         default  :
         case  0  : /* Nao preserva nenhuma informacao existente */
         case 'S' : /* preserva informacao existente conforme vista */
               tr_aclear(sele, padrao);  /* extrai campos nao auto clear */
               inic_numericos(sele, padrao);  /* iniclializa numericos/formula com 0 */
               break;

         case 'D' : /* duplica informacoes do registro corrente */
               dup_reg(sele);
               break;
      }

      if ( atu_ac(sele, NOVO_REGISTRO, (POINTER) 0) != 0 ) {
         ret = -1;
         goto fim;
      }
   }
   else {

      c_bases[sele]->cb_numrec = 0;
      c_bases[sele]->cb_aclock = (POINTER) 0;
   }


   if ( ret_trans >= 0 )
      END_TRANS(TR_NEWRECORD);

   switch(ret) {
      case -1 :   break;
      case -3 :   mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
                  break;
   }

   return(ret);
}


F__GLB   COUNT pal_index(sele, indice, chave, tam_chave)
COUNT sele, indice;
UTEXT *chave;
COUNT tam_chave;
{  COUNT ret;
   COUNT ret_trans = 0;

   if ( (ret_trans = BEG_TRANS(TR_PALINDEX, 0L, 0)) < 0 ) {
      ret = -1;
      goto fim;
   }


	ret = ADDKEY(AI_NUM(sele) + indice, (TEXT *)chave, (POINTER) 0 , REGADD, tam_chave);

	if ( uerr_cod  != NO_ERROR ) {
		ret = -1;
		goto fim;
   }

   ret = 0;

fim :

   if ( ret_trans >= 0 )
      END_TRANS(TR_PALINDEX);

   return(ret);
}


F__GLB   COUNT pal_d_index(sele, indice, chave, tam_chave)
COUNT sele, indice;
UTEXT *chave;
COUNT tam_chave;
{  COUNT ret;
   COUNT ret_trans = 0;

   if ( (ret_trans = BEG_TRANS(TR_PALDINDEX, 0L, 0)) < 0 ) {
      ret = -1;
      goto fim;
   }


	ret = DELKEY(AI_NUM(sele) + indice , (TEXT *)chave, tam_chave);

	if ( uerr_cod  != NO_ERROR ) {
      mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
		ret = -1;
		goto fim;
   }

   ret = 0;

fim :

   if ( ret_trans >= 0 )
      END_TRANS(TR_PALDINDEX);

   return(ret);

}

