#include  "lb2all.h"

#include  <string.h>
#include  <time.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2trans.h"

#include  "lb2vista.h"
#include  "lb2rel.h"

#ifdef ENTRA_TESTE_DATA
#define   VER_SE_DEMO { if ( ap_nserie < 100 && ! data_demo_ok() ) {debug("Demostracao simulando leitura do registro 1"); nreg = 1;}}
#endif


#define   H_ED_NOV   "edinovo"  /* Tentativa de accessar registro com status NOVO_REGISTRO */

#define   E_ED_NOV   loc_str(STR_GLB, 594)


#ifdef    VAXVMS

#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#define   POS_TO_NUM(pos)  (((pos - (POINTER)OFF_AC) / (POINTER) sizeof(Reg_controle)) + (POINTER) 1)

#else

#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) \
             / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + \
          ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#define   POS_TO_NUM(pos)  (((pos - (POINTER)OFF_AC) / (POINTER) sizeof(Reg_controle)) + (POINTER) 1)

#endif


#ifdef MSDOS
   COUNT fx_read_record(COUNT, LONG);
   COUNT fx_read_lock(COUNT, LONG);
   COUNT expa_registro(COUNT);
   VOID  tr_aclear(COUNT, COUNT);
   COUNT reg_updated(COUNT);
   COUNT crit_reg(COUNT);
#else
   COUNT fx_read_record( );
   COUNT fx_read_lock( );
   COUNT expa_registro( );
   VOID  tr_aclear( );
   COUNT reg_updated( );
   COUNT crit_reg( );
#endif




/*----------------------------------------------------------------------*/
/* READ_RECORD                                                          */
/*----------------------------------------------------------------------*/
/*
*/

F__GLB   COUNT read_record(sele, nreg, lock)
COUNT  sele;
LONG   nreg;
COUNT  lock;       /* se 1 deve ler com lock */
{  COUNT ret;

   if ( c_bases[sele]->cb_numrec == nreg && c_bases[sele]->cb_controle.rc_pos == 0L ) {
      /* nao libera senao entra em loop */
   }
   else
      free_record(sele);

   if ( (ret = BEG_TRANS(TR_READRECORD, nreg, lock)) == 0 ) {

      if ( lock )
         ret = fx_read_lock(sele, nreg);
      else
         ret = fx_read_record(sele, nreg);

      if ( ret < 0 )
         BEG_TRANS(TR_FREERECORD, nreg, 0);
   }

   return( ret );
}

/*----------------------------------------------------------------------*/
/* READ_RECORD                                                          */
/*----------------------------------------------------------------------*/
/*
      retorna :  0 -> fx_read_record OK
                -1 -> erros diversos
                 1 -> fx_read_record fora de escopo
                 2 -> fx_read_record de registro DISPONIVEL
                 3 -> fx_read_record de registro MORTO
                 4 -> fx_read_record de registro NOVO
*/

F__GLB   COUNT fx_read_record(sele, nreg)
COUNT  sele;
LONG   nreg;
{  COUNT ret, i, num_cps;
   TEXT *mens, aux[80];
   UCOUNT tam_ad;
   POINTER pos_ac, pos_ad;
   Reg_controle *rc;

#ifdef ENTRA_TESTE_DATA
VER_SE_DEMO /* se demo e data valida continua */
#endif


   /* free_record(sele); */                 /* libera o registro atual  */

   rc     = &c_bases[sele]->cb_controle;
   pos_ac = NUM_TO_POS(nreg);

   /******
   *******  Leitura de TBP
   ******/

ac_read:

   ret = REDREC(AC_NUM(sele), pos_ac, (TEXT *)rc);

   if ( ret != NO_ERROR ) {

      if ( ret == LEOF_ERR ) {
         mens_erro(H_REGINE, E_REGINE);
         ret = 1;
         goto fim;
      };


		switch (decida(TW_RDA0, TW_RDA9, H_CFRDREG, 1, 42, "%s", E_REGBAD)) {
         case -1 :   /* ESC */
			case 0  :   /* RETENTA */
                     goto ac_read;

			case 1  :   /* DESISTE */
                     ret = -1;
                     goto fim;

			case 2  :   /* ABORTA */
                     fim_lb(M_SOLIC);
      }
   }

   /******
   *******  Verifica se pode ler TBF
   ******/

   if ( rc->rc_status == LIBERADO ) {
      /*  Retirei pois pode acontecer em sequencial (multi_user) */
      /*  mens_erro(H_REGINE, E_REGINE); */
      ret = 2;
      goto fim;
   }

   if ( rc->rc_status == MORTO_INDEX ) { /* OU MORTO_NAO_INDEX */
      numdebug(430);
      ret = 3;
      goto fim;
   }

   if ( rc->rc_status == NOVO_REGISTRO ) {
      /*  Retirei pois pode acontecer em sequencial (multi_user) */
      /*  numdebug(431);  */
      ret = 4;
      goto fim;
   }

   /******
   *******  Leitura de TBF
   ******/

   pos_ad = rc->rc_pos;

   if ( pos_ad == (POINTER) 0 ) {
      mens_erro(H_POS0AC, E_POS0AC);
      ret = -1;
      goto fim;
   }

   tam_ad = GTVLEN(AD_NUM(sele), pos_ad);

   if ( uerr_cod != 0 ) {
      mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
      ret = -1;
      goto fim;
   }

   if ( tam_ad > c_bases[sele]->cb_tamrec ) {
      mens_erro(H_ATUBASE, E_ATUBASE);
      ret = -1;
      goto fim;
   }

ad_read:

   memset((TEXT *) c_bases[sele]->cb_ioarea, '\0', c_bases[sele]->cb_tamrec);

   ret = RDVREC(AD_NUM(sele), pos_ad,
         c_bases[sele]->cb_ioarea, c_bases[sele]->cb_tamrec);

   if ( ret != NO_ERROR ) {
      switch ( ret ) {
         case  SEEK_ERR :
         case  READ_ERR :  mens = E_TBFREAD;
                           break;
         default         :  sprintf(aux, E_LBNBAD, uerr_cod);
                            mens = aux;
                            break;
      };

		switch (decida(TW_RDA0, TW_RDA9, H_CFRDREG, 1, 42, "%s", mens)) {
         case -1 :   /* ESC */
			case 0  :   /* RETENTA */
                     goto ad_read;

			case 1  :   /* DESISTE */
                     ret = -1;
                     goto fim;

			case 2  :   /* ABORTA */
                     fim_lb(M_SOLIC);
      }
   }

   ret = 0;

fim :

   if ( ret == 0 ) {

      ret = 0;

/*** WWWW deve descriptografar tam_lastvrec de cb_ioarea */
      de_cripta((UTEXT *)WCB->cb_ioarea, ct_tamlast);

      if ( expa_registro(sele) != 0 ) {
         mens_erro(H_PERDAPA, M_PRDPAR, nreg);
         ret = 0;
         /* ret = -1; WWW */
         /* goto fim;     */
      };

      num_cps = qtd_vetcmp(sele);

      for ( i = 0 ; i < num_cps; i++ ) {
         pos_vetcmp(sele)[i]->cp_adoc     = NULL;
         pos_vetcmp(sele)[i]->cp_mod_flag = 0;
         pos_vetcmp(sele)[i]->cp_crt_flag = 0;
      }

      c_bases[sele]->cb_numrec = nreg;
   }

   return( ret );
}


/*----------------------------------------------------------------------*/
/* READ_LOCK                                                            */
/*----------------------------------------------------------------------*/
/*
      retorna :  0 -> fx_read_lock OK
                -2 -> lock nao foi possivel
                -1 -> erros diversos
                 1 -> fx_read_lock fora de escopo
                 2 -> fx_read_lock de registro DISPONIVEL
                 3 -> fx_read_lock de registro MORTO
                 4 -> fx_read_lock de registro NOVO
*/

F__GLB   COUNT fx_read_lock(sele, num)
COUNT sele;
LONG num;
{  COUNT ret, fim;
   Reg_controle controle, *rc;
   POINTER aclock, pos;
/*
   if ( num != c_bases[sele]->cb_numrec )
      free_record(sele);
*/

   /*
   **  Se nao existia lock anterior, tenta dar lock no AC,
   **  caso o lock tenha exito, re-ler o registro de controle e
   **  comparar se a data de alteracao do registro lido e a mesma
   **  do registro de controle.
   */

   aclock = c_bases[sele]->cb_aclock;

   if ( aclock != (POINTER) 0 ) {
      ret = 0;  /* registro solicitado era o corrente e ja estava locked */
      goto fim_gt;
   }

   pos = NUM_TO_POS(num);

   for ( fim = 0 ; fim == 0 ; ) {

      ret = LOKREC(AC_NUM(sele), ENABLE, pos);

      if ( ret == NO_ERROR ) {
         aclock = pos;
         fim = 1;
      }
      else {
         if ( ret == DLOK_ERR ) {
		      switch (decida(TW_RDA0, TW_RDA9, H_REGLOCK, 1, 42, "%s", E_REGLOCK)) {
               case -1 :   /* ESC */
			      case 0  :   /* RETENTA */
                           continue;

			      case 1  :   /* DESISTE */
                           ret = -1;
                           goto fim_gt;

			      case 2  :   /* ABORTA */
                           fim_lb(M_SOLIC);
            }
         }
         else {
            numdebug(434);
            ret = -1;
            goto fim_gt;
         }
      }
   } /* fim do for */



   if ( aclock == (POINTER) 0 ) {
      ret = -2; /* lock nao foi possivel */
      goto fim_gt;
   }


ac_read:


   /* le registro de controle para area temporaria,
   ** so' para conferir com registro corrente
   */
   memset((TEXT *)&controle, 0, sizeof(controle));

   ret = REDREC(AC_NUM(sele), aclock, (TEXT *)&controle);

   if ( ret != NO_ERROR ) {

      if ( ret == LEOF_ERR ) {
         ret = 1;
         goto fim_gt;
      };

		switch (decida(TW_RDA0, TW_RDA9, H_CFRDREG, 1, 42, "%s", E_REGBAD)) {
         case -1 :   /* ESC */
			case 0  :   /* RETENTA */
                     goto ac_read;

			case 1  :   /* DESISTE */
                     ret = -1;
                     goto fim_gt;

			case 2  :   /* ABORTA */
                     fim_lb(M_SOLIC);
      }
   }


   /*
   ** compara para ver se houve alteracao do registro
   ** para evitar re-ler registro
   */

   rc  = &c_bases[sele]->cb_controle;

   if ( controle.rc_status == LIBERADO ) {
      ret = 2;
      goto fim_gt;
   }

   if ( controle.rc_status == MORTO_INDEX || controle.rc_status == MORTO_NAO_INDEX  ) {
      numdebug(435);
      ret = 3;
      goto fim_gt;
   }

   if ( controle.rc_status == NOVO_REGISTRO ) {
      if ( ap_operacao != REINDEX ) {
         mens_erro(H_ED_NOV, E_ED_NOV);
         ret = 4;
      }
      else
         ret = 2;

      goto fim_gt;
   }


   if ( controle.rc_altdata == rc->rc_altdata &&
        controle.rc_pos     == rc->rc_pos    ) {
      ret = 0;
      expa_registro(sele);
      c_bases[sele]->cb_aclock = aclock;/* marca como lock */
      goto fim_gt;
   } else {
      ret = fx_read_record(sele, num);       /* carrega registro sem lock */
      c_bases[sele]->cb_aclock = aclock;/* marca como lock */
   }

fim_gt :

   return(ret);

}





/*----------------------------------------------------------------------*/
/* FREE_RECORD                                                          */
/*----------------------------------------------------------------------*/
F__GLB   VOID free_record(sele)
COUNT sele;
{  COUNT num_cps, i;
   Campo **vet_cmp, *pc;
   LONG nreg;

   if ( c_bases[sele] == NULL )
      return;

   if ( c_bases[sele]->cb_numrec != (ULONG) 0 ) {
      nreg = c_bases[sele]->cb_numrec;

      BEG_TRANS(TR_FREERECORD, nreg, 0);

      /* se registro acabou de ser alocado e nao foi gravado pode liberar */

#ifndef RUNTIME
      if ( c_bases[sele]->cb_controle.rc_status == NOVO_REGISTRO ) {
         del_record(sele, nreg);
      }
#endif
   }

   vet_cmp  = pos_vetcmp(sele);
   num_cps  = qtd_vetcmp(sele);


   for ( i = 0; i < num_cps; i++ ) {

      pc = vet_cmp[i];

      if ( pc->cp_tipo == CP_TEXTO && pc->cp_adoc != NULL ) {
         free_buffer(pc->cp_adoc );
         pc->cp_adoc = NULL;
      }

      pc->cp_mod_flag = 0;
      pc->cp_crt_flag = 0;
      pc->cp_newtam = 0;
   }

   ap_vista.vis_it_ativo  = -1;
   ap_vista.vis_old_ativo = -1;



   memset((TEXT *)&c_bases[sele]->cb_controle, 0, sizeof(Reg_controle));
   c_bases[sele]->cb_aclock = (POINTER) 0;
   c_bases[sele]->cb_adlock = (POINTER) 0;
   c_bases[sele]->cb_numrec = 0L;
}






F__GLB   COUNT expa_registro(sele)
COUNT sele;
{  COUNT ret, num_cps, efetivos, i, tamlido, campolido, achou, tam_deve;
   Campo **vet_cmp;
   TEXT *pr;

   pr = c_bases[sele]->cb_ioarea;

   vet_cmp  = pos_vetcmp(sele);
   num_cps  = qtd_vetcmp(sele);

   memset(c_bases[sele]->cb_wkarea, '\0', c_bases[sele]->cb_tamrec);

   efetivos = *(pr + HDR_REGISTRO - 1 );

   pr      += HDR_REGISTRO;

   for ( i = 0; i < num_cps; i++ ) {
      vet_cmp[i]->cp_olddata = NULL;
      vet_cmp[i]->cp_oldtam  = 0;
      vet_cmp[i]->cp_newtam  = 0;
   }

   while ( efetivos-- ){

      campolido = *pr++;
      tamlido   = *pr++;

      for ( i = 0, achou = 0 ; i < num_cps; i++ ) {
         if (vet_cmp[i]->cp_num == campolido ) {
            achou = 1;
            break;
         }
      }

      if ( achou == 0 ) {
         mens_erro(H_EXRE01, E_EXRE01);
         ret = -1;
         goto fim;
      }

      tam_deve = sizeof(POINTER);

#ifdef DECALFA
      tam_deve = 4;
#endif

      if ( vet_cmp[i]->cp_tipo == 'T' && tamlido != tam_deve ) {
         numdebug(507);
         ret = -1;
         goto fim;
      }

      if ( vet_cmp[i]->cp_mhor < tamlido && vet_cmp[i]->cp_tipo != 'T') {
         mens_erro(H_EXRE02, E_EXRE02);
         ret = -1;
         goto fim;
      }

      memcpy((TEXT *)vet_cmp[i]->cp_newdata, pr, tamlido);
      vet_cmp[i]->cp_newtam  = tamlido;

      vet_cmp[i]->cp_olddata = pr;
      vet_cmp[i]->cp_oldtam  = tamlido;

      pr += tamlido;
   }

   ret = 0;

fim :

   return(ret);

};



F__GLB   LONG reg_corrente(sele, nreg)
COUNT sele;
LONG nreg;
{

   if ( nreg == 0 )
      nreg = c_bases[sele]->cb_numrec;
   else
      nreg = -1;

   return(nreg);
}


F__GLB   LONG prox_reg(sele, nreg)
COUNT sele;
LONG nreg;
{  COUNT ret;
   POINTER pos_ac;
   Reg_controle rc;

   /* para possibilitar impressao de registros por faixa */
   if ( grel != NULL && grel->first_record > nreg ) {
      nreg = grel->first_record - 1;
   }

   while ( 1 ) {
      nreg++;
      pos_ac = NUM_TO_POS(nreg);

      ret = REDREC(AC_NUM(sele), pos_ac, (TEXT *)&rc);

      /* para possibilitar impressao de registros por faixa */
      if ( grel != NULL && grel->last_record != 0 && nreg > grel->last_record ) {
         nreg = 0;
         goto fim;
      }

      if ( ret != NO_ERROR ) {
         if ( ret == LEOF_ERR )
            nreg = 0L;
         else
            nreg = ret = -1;
         goto fim;
      }

      if ( rc.rc_status == ATIVO_INDEX || rc.rc_status == ATIVO_NAO_INDEX || rc.rc_status == NO_INDEX  ) {
         if ( rc.rc_pos != (POINTER) 0 ) {
            break;
         }
      }
   }

fim :

   return(nreg);
}


F__GLB   LONG ante_reg(sele, nreg)
COUNT sele;
LONG nreg;
{  COUNT ret;

   while ( 1 ) {

      nreg--;
      if ( nreg <= 0 ) { goto fim; }

      set_erro(0);
      ret = read_record(sele, nreg, 0);
      set_erro(1);

      if ( ret == -98 ) {continue;}

      if ( ret < 0 ) { nreg = -1; goto fim; }

      else if ( ret == 1 ) { /* fim */
            nreg = 0L;
            break;
      }

      else if ( ret == 0 ) { break; } /* registro localizado */

      else {
         /* disponivel ou novo */
         continue;
      }
   }

fim :

   return(nreg);
}
