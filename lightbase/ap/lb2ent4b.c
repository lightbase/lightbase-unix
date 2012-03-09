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
/* DEL_RECORD                                                           */
/*----------------------------------------------------------------------*/
F__GLB   COUNT del_record(sele, num)
COUNT sele;
LONG num;
{  COUNT ret, i, num_cps;
   COUNT ret_trans = 0;
   TEXT chave[20];
   Campo **vet_cmp, *pc;
   Reg_controle *rc;

   if ( WCB->cb_lockatu == 0 ) {
      mens_erro(H_ESTATIC, E_ESTATIC);
      ret = -1;
      goto sai;
   }

   if ( c_bases[sele]->cb_numrec == num && c_bases[sele]->cb_controle.rc_pos == 0L ) {
      /* nao ler senao entra em loop */
   }
   else if ( read_record(sele, num, 1) != 0 ) {
      ret = -1;
      goto fim;
   };

   if ( (ret_trans = BEG_TRANS(TR_WRRECORD, num, 0)) < 0 ) {
      ret = -1;
      goto fim;
   }

   rc  = &c_bases[sele]->cb_controle;
   num = c_bases[sele]->cb_numrec;

/******
   if ( rc->rc_status != NOVO_REGISTRO && (ret = read_record(sele, num, 1)) != 0 )
      goto fim;
*******/

   switch(rc->rc_status) {

      case NOVO_REGISTRO   :  /* so devera devolver numero para disponivel */
                              break;

      case ATIVO_INDEX     :
                              if ( reg_index(sele, EXCLUSAO, '-', num, FLUSH_IDX) != 0 ) {
                                 ret = -1;
                                 goto fim;
                              };

                              break;

      case ATIVO_NAO_INDEX :
                              sprintf(chave, "%9.9ld", num);
                              if ( pal_d_index(sele, TREE_NAO_INDEX,
                                 (UTEXT *)chave, strlen(chave) + 1) != 0 ) {
                                 mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
                                 ret = -1;
                                 goto fim;
                              }
                              break;

      case LIBERADO      :
                              break;

      case MORTO_INDEX     :
                              break;

      case MORTO_NAO_INDEX :
                              break;

   }


   /* libera chaves unicas */

   /* Passa a chamar del_record da reindexacao quando um registro possui */
   /* chave unica. Neste caso nao presisa passar por salva_unica */

   if ( ap_operacao != REINDEX && salva_unica(sele, EXCLUSAO, '-') != NULL ) {
      mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
      ret = -1;
      goto fim;
   }



   /* varre vetor de campos procurando campos textos e liberando
   ** espacos ocupados em disco
   */

   vet_cmp  = pos_vetcmp(sele);
   num_cps  = qtd_vetcmp(sele);

   for ( i = 0; i < num_cps; i++ ) {

      pc = vet_cmp[i];

      if ( pc->cp_tipo == 'T' && pc->cp_olddata != NULL ) {
         ret = txt_free( AD_NUM(sele), get_postexto('O', (UTEXT *)pc->cp_olddata));
      }

      pc->cp_oldtam  = 0;
      pc->cp_olddata = NULL;
   }

   if ( rc->rc_pos != (POINTER) 0 ) {
      ret = RETVREC(AD_NUM(sele), rc->rc_pos);
      if ( ret != NO_ERROR ) {
         ret = -1;
         goto fim;
      }
   }

   if ( atu_ac(sele, LIBERADO, rc->rc_pos) != 0 ) {
      ret = -1;
      goto fim;
   }

   sprintf((TEXT *)chave, "%9.9ld", num);
   ret = pal_index(sele, TREE_DELETADOS, (UTEXT *)chave, strlen(chave) + 1);
   if ( ret != 0 ) {
      mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
      ret = -1;
      goto fim;
   }

fim :

   if ( ret == 0 && ++ap_curflushfile >= ap_flushfile ) {
      FLUSHFIL(AC_NUM(sele));
      FLUSHFIL(AD_NUM(sele));
      FLUSHFIL(AI_NUM(sele));
      ap_curflushfile = 0;
   }

   free_record(sele);

   if ( ret_trans >= 0 )
      END_TRANS(TR_WRRECORD);

sai :

   return(ret);
}


F__LOC   VOID dup_reg(sele)
COUNT sele;
{  COUNT i, num_cps;
   Campo *pc;
   POINTER pos;


   num_cps = qtd_vetcmp(sele);

   for ( i = 0 ; i < num_cps; i++ ) {

      pc = pos_vetcmp(sele)[i];

      pc->cp_mod_flag = 1;
      pc->cp_crt_flag = 1;
      pc->cp_newtam = pc->cp_oldtam;

      if ( pc->cp_chave_unica == 'S' ) {
         pc->cp_newdata[0] = '\0';
         pc->cp_newtam = 0;
      }

      if ( pc->cp_tipo    == 'T' && pc->cp_newdata != NULL) {
         pos = get_postexto('N', (UTEXT *)pc->cp_newdata);
         if ( pos != (POINTER) 0 ) {
            pos = txt_dup(AD_NUM(sele), AD_NUM(sele),
                          pos, c_bases[sele]->cb_numrec);
            put_postexto('N', (UTEXT *)pc->cp_newdata, pos);
         }
      }
   }

   memset(c_bases[sele]->cb_ioarea, '\0', c_bases[sele]->cb_tamrec);
}


F__LOC   VOID tr_aclear(sele, deve_salvar)
COUNT sele;
COUNT deve_salvar;
{  COUNT salva, limpa, i, j, num_cps, n_elem;
   Campo *pc;
   TTela *pe;

   memset(c_bases[sele]->cb_wkarea, '\0', c_bases[sele]->cb_tamrec);

   num_cps = qtd_vetcmp(sele);

   for ( i = 0 ; i < num_cps; i++ ) {
      pc = pos_vetcmp(sele)[i];

      limpa = (deve_salvar == 0 ) ? 1 : 0 ;

      if ( pc->cp_oldtam  == 0   ) limpa = 1;
      if ( pc->cp_olddata == NULL) limpa = 1;
      if ( pc->cp_tipo    == 'T' ) limpa = 1;

      if ( limpa == 0 ) {
         n_elem = qtd_itvis();
         for (salva = 0, j = 0; j < n_elem; j++ ){
            pe = pos_itvis(j);
            if ( pe->te_tipo != CAMPO_E )    continue;
            if ( pe->te_num  != pc->cp_num ) continue;
            if ( pe->te_autoclear != 'N')    continue;
            salva = 1;
            break;
         }
      }

      if ( salva == 1 && limpa == 0) {
         memcpy(pc->cp_newdata, pc->cp_olddata, pc->cp_oldtam);
         pc->cp_newtam  = pc->cp_oldtam;
         pc->cp_mod_flag = 1;
         pc->cp_crt_flag = 1;
      }
      else {
         pc->cp_newtam  = 0;
      }

      pc->cp_olddata = NULL;
      pc->cp_oldtam  = 0;
   }

   memset(c_bases[sele]->cb_ioarea, '\0', c_bases[sele]->cb_tamrec);
}

F__LOC   VOID inic_numericos(sele, preserva)
COUNT sele;
COUNT preserva;
{  COUNT i, num_cps;
   Campo *pc;


   num_cps = qtd_vetcmp(sele);

   for ( i = 0 ; i < num_cps; i++ ) {

      pc = pos_vetcmp(sele)[i];

      if ( pc->cp_tipo  == 'N' || pc->cp_tipo  == 'F' ) {
         if ( ! preserva || pc->cp_tipo  == 'F' ) {
            TEXT *masc;
            if ( pc->cp_tipo == 'F' )
               masc = pc->cp_param.cp_msc_form;
            else
               masc = pc->cp_mascara;

            formata_campo(pc->cp_newdata, "0", masc, E_VALOR);
         }
         pc->cp_newtam = strlen(pc->cp_newdata);
         pc->cp_mod_flag = 1;
         pc->cp_crt_flag = 1;
      }
   }
}




F__GLB   VOID help_campo(pc, path_help)
Campo *pc;
TEXT *path_help;
{  COUNT ret = 0;
   TEXT u_help[61], *path;

   path_help[0] = '\0';

   strcpy(u_help, pc->cp_help);

   /* tratamento de helps indiretos. Formato @CAMPO,DEFAULT */
   if ( u_help[0] == '@' ) {
      COUNT ind, num;
      TEXT *p, aux[81];
      p = &u_help[1];
      piece(p, aux, ',', 1, TE);  aux[TE] = '\0';

      ind = ver_campo(BASE_EDIT, aux, &num);

      if ( ind >= 0 &&
               pos_vetcmp(BASE_EDIT)[ind]->cp_tipo != CP_TEXTO &&
               c_bases[BASE_EDIT]->cb_numrec != 0L &&
               pos_vetcmp(BASE_EDIT)[ind]->cp_newtam > 0 ) {
         strncpy(aux, pos_vetcmp(BASE_EDIT)[ind]->cp_newdata, TE);
         aux[TE] = '\0';
      }
      else
         piece(p, aux, ',', 2, TE);  aux[TE] = '\0';

      strcpy(u_help, aux);
   }

   path = v_bases[BASE_EDIT]->bs_path;
   ret = info_base(u_help, path, str_nada, path_help);

   if ( ret >= 0 )
      user_help = path_help;
}


