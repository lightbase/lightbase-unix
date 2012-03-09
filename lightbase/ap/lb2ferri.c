#include  "lb2all.h"

#include  <string.h>
#include  HD_IO
#include  HD_MEMO

#ifdef UNIX
#ifndef CTSRVR
#include   <sys/types.h>
#include   <sys/stat.h>
#endif
#endif

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */


#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2vet.h"

#include  "lb2fer.h"

#ifdef    VAXVMS

#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#else

#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) \
             / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + \
          ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#endif

#define   STR_SLD       "%s %ld"


#define   H_READLOG   "LEIALOG"   /* leia arquivo de log */
#define   H_RBIND     "E_RBIND"   /* erro na reindexacao apos rebuild */
#define   H_CFRBZAP   "CF_RBZAP"  /* confirma zap em rebuild */
#define   H_CFWRREG   "CF_WRREG"  /* confirma zap em rebuild */

extern FILE *fpo_exp;

extern UC   fl_erro_on;
#define   set_erro(x)  fl_erro_on = x

#ifdef MSDOS
   COUNT    dat_open(COUNT);
   COUNT    av_record(POINTER, TEXT *, LONG);
   COUNT    reg_noindex( COUNT, LONG, LONG);
   COUNT    exec_export(COUNT, TEXT *);
#else
   COUNT    dat_open( );
   COUNT    av_record( );
   COUNT    reg_noindex( );
   COUNT    exec_export( );
#endif



static COUNT erro_fatal;
/* LONG ap_nregexp = 0L;      */ /* numero do registro a ser impresso na exportacao */
                           /* o certo seria exportar o registro de selenovo */
                           /* mas esta dando pau. Usei este artificio por falta de tempo */
                           /* para uma solucao mais elegante. Ananias (12/02/95) */

F__GLB   COUNT new_base(pb)
Base *pb;
{  COUNT ret, i, num, narq;
   UCOUNT id;
   LONG ns;
   TEXT nx[PATH_MAX], *pnx;
   COUNT fixo = PERMANENT | EXCLUSIVE | FIXED;
   COUNT varl = PERMANENT | EXCLUSIVE | VLENGTH;

   ret = 0;

   info_base("@@@", pb->bs_path, LB_1_EXT, nx);

   /* faz pnx apontar para numero do arquivo LB? */
   pnx = strchr(nx, '\0');
   pnx--;

   narq = INI_SELE(BASE_EDIT);

   ns = ap_nserie;
   id = pb->bs_idbase;


   for ( i = 1; i <= 3; i++ ) {
      *pnx = '0' + i;  /* Monta nome do arquivo */

#ifndef CTSRVR
      if ( access(nx, 0) == 0 ) {
         DELFILE(nx);
      }
#else
/* WWW Implementar ACCESS( ) via servidor!!!!!! */
#endif

      ct_finger  = pb->bs_finger;
      ct_release = pb->bs_release;
      ct_sernum   = 0L;

      switch ( i ) {
         case 1 : /* Arquivo de controle */
                  CREDAT(narq, nx, sizeof(Reg_controle), SLOT, fixo, ns, id );
                  break;

         case 2 : /* Arquivo de dados */
                  CREDAT(narq, nx, 0, SLOT, varl, ns, id );
                  break;

         case 3 : /* Arquivo de indices */
                  if ( CREIDX(narq, nx, LB_KEYMAX, 0, AI_INDICES - 1,
                              SLOT, varl, ns, id ) == 0 ) {
                     /* cria outros indices do arquivo */
                     for (num = 1; num < AI_INDICES; num++) {
                        COUNT tam_key;

                        tam_key = (num == TREE_UNIQ) ? LB_KEYCT : LB_KEYMAX;

                        if ( CREMEM(narq, tam_key, 0, num, ns, id ) != 0 )
                           break;
                     }
                  };
                  break;
      }


      if ( uerr_cod ) {
         mens_erro(H_CRARQ, E_CRF_MA, uerr_cod, nx);
	      ret = -1;
      }

      CLSFIL(narq, COMPLETE);

      if ( ret != 0 )
         goto fim;
   }

   ret = 0;

fim :

   return(ret);
}




F__GLB   COUNT tbp_scan( sele, vet_zap, regs_na_base )
COUNT sele;
TEXT *vet_zap;
LONG *regs_na_base;
{  COUNT ret, sai, analiza, salva;
   COUNT salva_flush, salva_seg;
   COUNT selenovo, abriu_novo=0;
   LONG nreg;
   ULONG ativos, inativos, recuperados, parcial_rec, perdidos;
   POINTER pos;
   Desc_base *wcb=NULL;
   Reg_controle controle;
   COUNT ind_ant   = ap_pos_index;
   TEXT aux[120];
   WIN *jm;
   extern COUNT ct_semseguranca;

   salva_flush = ap_flushfile;
   salva_seg   = ct_semseguranca;
   ap_flushfile = 100;

   *regs_na_base = 0;
   selenovo = sele + 1;
   ativos = inativos = 0L;
   recuperados = parcial_rec = perdidos = 0L;
   buf_reg = NULL;
   jm = NULL;

   ap_nregexp = 0L;

   if ( (wcb = calloc(1, sizeof(Desc_base))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      wr_log(1, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   wcb->cb_fordem   = selenovo + 1;     /* importante para numero de file a OPEN */
   wcb->cb_prfile   = INI_SELE(selenovo);
   c_bases[selenovo] = wcb;


   jm = abre_mensagem(M_AGUARDE);


   ret = dat_open(selenovo);


   if ( ret != 0 ) {
      ret = (ret > 0) ? 0 : -1;  /* se maior zero. Selecionado um zap na base */
      selenovo = 0;
      goto fim;
   }

   abriu_novo = 1;

   mens_1(jm, M_AGUARDE);

   if ( (buf_reg = malloc(MAX_REG)) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   ap_pos_index = NO_INDEX;

   if ( new_record(sele, 0) < 0 ) {
      wr_log(1, ML_ERNEW);
      ret = -1;
      goto fim;
   }

   pos   = (POINTER)0;
   sai   = salva = 0;

   nreg = 1;
   set_erro(0);

   while ( ! sai ) {

      pos = NUM_TO_POS(nreg);
      nreg++;

      ret = REDREC(AC_NUM(selenovo), pos, (TEXT *)&controle);

      if ( ret != NO_ERROR ) {
         if ( ret == LEOF_ERR ) {
            ret = NO_ERROR;
            sai = 1;
            break;
         }
      }

      analiza = 0;

      switch ( controle.rc_status ) {

         case NO_INDEX         :
         case ATIVO_INDEX      :
         case ATIVO_NAO_INDEX  : controle.rc_status = NO_INDEX;
               ativos++;
               analiza = 1;
               break;

         case NOVO_REGISTRO    :
         case LIBERADO       :
               inativos++;
               break;


         case MORTO_INDEX      :
         case MORTO_NAO_INDEX  : break;

         default : /* numdebug(901); tipo de registro de controle nao previsto */
                   sprintf(aux, ML_CTLINV, nreg - 1);
                   wr_log(1, aux);
                   perdidos++;
                   break;
      }


      if ( analiza ) {

         ret = av_record(controle.rc_pos, vet_zap, nreg - 1);

         switch(ret) {
            case -2 : /* erros diversos ja impressos */
                  sprintf(aux, ML_REGRUIM, nreg - 1);
                  wr_log(1, aux);
                  salva = 0;
                  ret = -1;
                  goto fim;

            case -1 : /* falta memoria */
                  wr_log(1, SEM_MEMO);
                  ret = -1;
                  goto fim;

            case  0 : /* registro recuperado OK */
                  salva = 1;
                  recuperados++;
                  break;

            case 1 : /* registro de campo texto nao foi lido */
                  sprintf(aux, "%s : %ld", ML_LSTTXT, c_bases[sele]->cb_numrec);
                  wr_log(1, aux);
                  salva = 2;
                  parcial_rec++;
                  break;

            case  2 : /* registro principal nao foi lido */
                  salva = 0;
                  perdidos++;
                  break;
         }

         if ( salva ) {
            Reg_controle o_controle;

            controle.rc_pos = 0;
            o_controle =  c_bases[sele]->cb_controle;

            c_bases[sele]->cb_controle = controle;

            if ( write_record(sele, ACUMULA_IDX) != 0 ) {
               if ( n_cp_unica >= 0 ) {
                  TEXT buf_ex[250];
                  sprintf(aux, ML_KEYDES, nreg - 1,
                         pos_vetcmp(sele)[n_cp_unica]->cp_nome,
                         pos_vetcmp(sele)[n_cp_unica]->cp_newdata);
                  wr_log(1, aux);
                  if ( fpo_glb != NULL ) {
                     fpo_exp = fpo_glb;
   						ap_nregexp = nreg - 1; /* exportacao com numero alternativo de nreg */
                     exec_export( sele, buf_ex);
   						ap_nregexp = 0L; /* desabilita exportacao com numero alternativo de nreg */
                     fpo_exp = NULL;
                  }
                  c_bases[sele]->cb_controle = o_controle ;
                  if ( salva == 1 )
                     recuperados--;
                  else
                     parcial_rec--;
                  perdidos++;
                  ret = 0;
                  continue;
               }
               else {
                  wr_log(1, ML_WRREG);
               }
               ret = -2;
               goto fim;
            }
            else{
               (*regs_na_base)++;
               mens_1(jm, ML_RECREG, *regs_na_base);
            }

            if ( new_record(sele, 0) < 0 ) {
               wr_log(1, ML_ERNEW);
               ret = -1;
               goto fim;
            }
         }
      }

      ret = 0;
   }

   if ( ret != NO_ERROR )
      wr_log(1, ML_LSTCTR);

fim :

   set_erro(1);

   sprintf(aux, STR_SLD, C_FER3_AT, ativos);            wr_log(0, aux);
   sprintf(aux, STR_SLD, C_FER3_IN, inativos);          wr_log(0, aux);
   sprintf(aux, STR_SLD, C_FER3_RE, recuperados);       wr_log(0, aux);
   sprintf(aux, STR_SLD, C_FER3_PR, parcial_rec);       wr_log(0, aux);
   sprintf(aux, STR_SLD, C_FER3_NR, perdidos);          wr_log(perdidos, aux);

   if ( ret < 0 )
      mens_erro(H_READLOG, E_READLOG);

   ap_pos_index = ind_ant;
   ap_flushfile = salva_flush;
   ct_semseguranca = salva_seg;

   if ( selenovo && abriu_novo ) {
      CLSFIL(AD_NUM(selenovo), COMPLETE );
      CLSFIL(AC_NUM(selenovo), COMPLETE );
   }

   c_bases[selenovo] = NULL;

   if ( buf_reg != NULL )  free(buf_reg);

   free_record(sele);

   fecha_mensagem(jm);

   if (wcb != NULL ) free(wcb);

   ap_nregexp = 0L; /* desabilita exportacao com numero alternativo de nreg */
   return(ret);
}




F__LOC   COUNT dat_open(sele)
COUNT sele;
{  COUNT ret;
   LONG num_serie;
   UCOUNT id_arq;
   COUNT res1, res2;
	TEXT n1[PATH_MAX]; /* Arquivo de controle  */
	TEXT n2[PATH_MAX]; /* Arquivo de Dados     */


   num_serie = ap_nserie;
   id_arq    = rec_base->bs_idbase;

   /*
   **   Inicialmente verifica se arquivo descritores da base estao ok
   */
   res1 = info_base(rec_base->bs_nome, rec_base->bs_path, LB_1_EXT, n1);
   res2 = info_base(rec_base->bs_nome, rec_base->bs_path, LB_2_EXT, n2);

   if ( (res1 & res2 & PODE_LER) == 0 ){
      if ( decida(M_ATENCAO, OP_SIM_NAO, H_CFRBZAP, 3, 50, "%s\n%s\n%s",
                  TW_ZPRB1, TW_ZPRB2, TW_ZPRB3) == 1)
         ret = 1;
      else
         ret = -1;

      goto fim;
   }


   ret = MANUT_OPEN( AC_NUM(sele), n1, 'F', &num_serie, &id_arq );

   if ( ret == NO_ERROR || ret == FCRP_ERR ) {
      ret = 0;
   }
   else {
      mens_erro(H_OPESO, E_OPE_SO, uerr_cod, n1);
      CLSFIL(AC_NUM(sele), COMPLETE );
      ret = -1;
      goto fim;
   }


   ret = MANUT_OPEN( AD_NUM(sele), n2, 'V', &num_serie, &id_arq );
   if ( ret == NO_ERROR || ret == FCRP_ERR ) {
      ret = 0;
   }
   else {
      mens_erro(H_OPESO, E_OPE_SO, uerr_cod, n2);
      CLSFIL(AD_NUM(sele), COMPLETE );
      CLSFIL(AC_NUM(sele), COMPLETE );
      ret = -1;
      goto fim;
   }

fim :

   return(ret);
}



F__LOC   COUNT av_record(pos, vet_zap, nreg)
POINTER pos;
TEXT *vet_zap;
LONG nreg;
{  COUNT ret, perdeu, mantem;
   COUNT num_cps, efetivos, i, tamlido, campolido, achou;
   UCOUNT tamanho, selenovo;
   TEXT *pr, aux[150];
   Campo **vet_cmp, *pc;
   POINTER pos_ret, pos_texto;

   perdeu = 0;
   ret = 0;
   selenovo = BASE_EDIT + 1;

   tamanho = GTVLEN(AD_NUM(selenovo), pos);

   if ( uerr_cod != NO_ERROR || tamanho == 0) {
      sprintf(aux, ML_REGRUIM, nreg);
      wr_log(1, aux);
      ret = 2;
      goto fim;
   }


   if ( tamanho > MAX_REG ) {
      sprintf(aux, ML_BIGREG, nreg, tamanho);
      wr_log(1, aux);
      ret = 2;
      goto fim;
   }

   ret = RDVREC(AD_NUM(selenovo), pos, buf_reg, tamanho);

   if ( ret != NO_ERROR ) {
      sprintf(aux, ML_REGRUIM, nreg);
      wr_log(1, aux);
      ret = 2;
      goto fim;
   }


/*** WWWW deve descriptografar tam_lastvrec de cb_ioarea */
   /* caso esteja reprocessando versao antes de 1.8 nao tinha cript */
   if ( versao_velha >= VERSAO_CRIPT )
      de_cripta((UTEXT *)buf_reg, tamanho);


   pr = buf_reg;

   vet_cmp  = pos_vetcmp(BASE_EDIT);
   num_cps  = qtd_vetcmp(BASE_EDIT);


   efetivos = *(pr + HDR_REGISTRO - 1 );

   pr      += HDR_REGISTRO;


   memset(c_bases[BASE_EDIT]->cb_wkarea, '\0', c_bases[BASE_EDIT]->cb_tamrec);
   for ( i = 0; i < num_cps; i++ ) {
      pc = vet_cmp[i];
      pc->cp_olddata = NULL;
      pc->cp_oldtam  = 0;
      pc->cp_newtam  = 0;
      pc->cp_mod_flag = 0;
      pc->cp_adoc    = NULL;

      if ( pc->cp_tipo  == 'N' || pc->cp_tipo  == 'F' ) {
         TEXT *masc;
         if ( pc->cp_tipo == 'F' )
            masc = pc->cp_param.cp_msc_form;
         else
            masc = pc->cp_mascara;
         formata_campo(pc->cp_newdata, "0", masc, E_VALOR);
         pc->cp_newtam = strlen(pc->cp_newdata);
         pc->cp_mod_flag = 1;
         pc->cp_crt_flag = 1;
      }
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

      pc = vet_cmp[i];

      if ( achou == 0 ) { /* campo nao existe mais */
         pr += tamlido;
         continue;
      }

      if ( vet_zap[pc->cp_num] > 1 ) { /* era ou sera' CP_TEXTO */
         pr += tamlido;
         continue;
      }

      switch(pc->cp_tipo ) {

         case CP_TEXTO :
            if ( tamlido != 4 ) {
               debug("WWW Campo texto com tamanho != 4 (%d)", tamlido);
               pr += tamlido;
               continue;
            }

            /* tratamento para reprocessar versao 1.5 */
            if ( versao_velha >= VERSAO_LONG )
               pos_texto = get_postexto('O', pr);
            else {
               pos_texto = get_postexto('L', pr);
            }


            if ( pos_texto == (POINTER) 0 ) {
               put_postexto('N', pc->cp_newdata, pos_texto);
               pc->cp_newtam = 0;
            }
            else {
               pos_ret = txt_dup(AD_NUM(selenovo), AD_NUM(BASE_EDIT),
                                   pos_texto, nreg);

               if ( pos_ret == (POINTER) 0 ) {
                  put_postexto('N', (UTEXT *)pc->cp_newdata, pos_ret);
                  pc->cp_newtam = 0;
                  perdeu = 1;
                  pr += tamlido;
                  continue;
               }
               else {
                  put_postexto('N', (UTEXT *)pc->cp_newdata, pos_ret);
                  pc->cp_newtam = sizeof(POINTER);
                  pc->cp_mod_flag = 0; /* campo texto ja esta salvo */
               }
            }

            break;

         case 'F' :

            /* nao faz nada. funcao trata_formula vai tratar */
            break;


         default  :

            if ( pc->cp_mhor < tamlido ) {
               sprintf(aux, ML_CPTRUNC, nreg, pc->cp_nome, pc->cp_newdata);
               wr_log(1, aux);
               mantem = pc->cp_mhor;
            }
            else
               mantem = (COUNT)tamlido;

            if ( vet_zap[pc->cp_num] == 1 ) {  /* campo mudou de tipo */
               TEXT *masc;
               memcpy(aux, pr, mantem);
               aux[mantem] = '\0';
               if ( pc->cp_tipo == 'F' )
                  masc = pc->cp_param.cp_msc_form;
               else
                  masc = pc->cp_mascara;
               formata_campo(pc->cp_newdata, aux, masc,
                              ((pc->cp_tipo == 'N' ) ? E_VALOR : E_ALFA));
               pc->cp_newtam = strlen(pc->cp_newdata);
               pc->cp_mod_flag = 0;
            }
            else {
               memcpy((TEXT *)pc->cp_newdata, pr, mantem);
               pc->cp_newtam  = mantem;
               pc->cp_mod_flag = 0;
            }

            break;
      }

      if ( erro_fatal ) {
         ret = -2;
         goto fim;
      }

      pr += tamlido;
   }

   if ( perdeu > 0 )
      ret = 1;
   else {
      trata_outs(BASE_EDIT, 1);
      trata_formula(BASE_EDIT);
      trata_outs(BASE_EDIT, 1); /* repetido para sanar referencia multipla */

      ret = 0;
   }

fim :

   return(ret);
}



F__GLB   COUNT tbp_index( sele, tot_regs )
COUNT sele;
LONG tot_regs;
{  COUNT ret, interrompe, col;
   LONG nreg;
   ULONG indexados;
   WIN *jm;

   indexados = 0L;


   jm = abre_mensagem(M_WAITIND);
   col = strlen(M_WAITIND) + 1;

   interrompe = 0;
   nreg = 0;

   while ( 1 ) {

      if ( w_kbhit(jm) && fwnqgc(jm) == K_ESC ) {
         if ( decida(TW_RB_D00, TW_RB_D99, "CF_RBCAN",
                     4, 42, "%s\n%s\n%s\n%s",
                     TW_RB_D01, TW_RB_D02, TW_RB_D03, TW_RB_D04 ) == 1 ) {
            interrompe = 1;
            break;
         }
      }

      nreg++;

      ret = read_record(sele, nreg, 1);

      if ( ret > 0 ) {
         ret = 0;
         break;
      }

      if ( ret < 0 ) {
         ret = -1;
         goto fim;
      };

      if ( c_bases[sele]->cb_controle.rc_status != NO_INDEX ) {
         numdebug(903);                /* Status diferente de NO_INDEX */
         ret = -1;
         goto fim;
      }

      if ( atu_ac(sele, ATIVO_INDEX, c_bases[sele]->cb_controle.rc_pos) < 0 ) {
         ret = -1;
         goto fim;
      }

      fwat(col,  0, jm);
      fwprintf( jm, "%l", ++indexados);

      fwrefresh(jm);

      if ( reg_index(sele, INCLUSAO, '+', nreg, ACUMULA_IDX) != 0 ) {
         ret = -1;
         goto fim;
      }
   }


fim :

   ret = fim_index();

   if ( interrompe ) {  /* inclui registros no indice de nao indexados */
      wr_log(1, ML_CANIDX);
      ret = reg_noindex( sele, nreg + 1, tot_regs );
   }

   if (ret == -1 )
      mens_erro(H_RBIND, E_RBIND);

   free_record(sele);

   fecha_mensagem(jm);

   return(ret);
}



F__LOC   COUNT reg_noindex( sele, inicio, fim )
COUNT sele;
LONG inicio, fim;
{  COUNT ret;
   LONG num_rec;
   TEXT chave[20];
   WIN *jm, *jf;

   jm = abre_mensagem(M_WAITMRK);
	jf = abre_funcoes(str_vazia);

   ret = 0;

   for ( num_rec = inicio; num_rec <= fim; num_rec++) {
      sprintf(chave, "%9.9ld", num_rec);
      if ( pal_index(sele, TREE_NAO_INDEX,
            (UTEXT *)chave, strlen(chave) + 1) != 0 ) {
         mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
         ret = -1;
         break;
      }
   }

	fecha_funcoes(jf);
	fecha_mensagem(jm);


   return(ret);
}
