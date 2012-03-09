#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>
#include  <time.h>

#include  HD_DIR
#include  HD_IO

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
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#define   GRP_REIND   30

#define   STR_DUAS    "%s\n%s"
#define   STR_QUAT    "%s\n%s\n%s\n%s"
#define   STR_LEFTN   "%-*s"
#define   STR_D       "%d"
#define   STR_9P9D    "%9.9ld"

#define  H_CFICCAN   "CF_ICCAN"  /* confirma cancelamento de indexacao completa */
#define  H_CFIPCAN   "CF_IPCAN"  /* confirma cancelamento de indexacao parcial */
#define  H_OTIMIZA   "OP_OTIMI"  /* Opcoes de otimizacao de indexacao */

#define   C_IX_INDEX    loc_str(GRP_REIND, 1)
#define   C_IX_REINDEX  loc_str(GRP_REIND, 2)
#define   C_IX_FRT1     loc_str(GRP_REIND, 3)
#define   C_IX_FRP1     loc_str(GRP_REIND, 4)
#define   C_IX_TOIND    loc_str(GRP_REIND, 5)
#define   C_IX_1WORK    loc_str(GRP_REIND, 6)
#define   C_IX_2WORK    loc_str(GRP_REIND, 7)
#define   C_IX_FINAL    loc_str(GRP_REIND, 8)

#define   TW_IP_D00     loc_str(GRP_REIND, 10)
#define   TW_IP_D99     loc_str(GRP_REIND, 11)
#define   TW_IP_D01     loc_str(GRP_REIND, 12)
#define   TW_IP_D02     loc_str(GRP_REIND, 13)

#define   TW_IC_D00     loc_str(GRP_REIND, 14)
#define   TW_IC_D99     loc_str(GRP_REIND, 15)
#define   TW_IC_D01     loc_str(GRP_REIND, 16)
#define   TW_IC_D02     loc_str(GRP_REIND, 17)
#define   TW_IC_D03     loc_str(GRP_REIND, 18)
#define   TW_IC_D04     loc_str(GRP_REIND, 19)

#define   TW_OT_D00     loc_str(GRP_REIND, 20)
#define   TW_OT_D99     loc_str(GRP_REIND, 21)
#define   TW_OT_D01     loc_str(GRP_REIND, 22)

#ifdef    VAXVMS

#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#else

#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) \
             / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + \
          ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#endif

#define   COMPLETA 'C'
#define   PARCIAL  'P'


#ifdef MSDOS
   COUNT  cria_ibp(COUNT, TEXT *, LONG, UCOUNT);
   VOID abre_rlog( TEXT *);
   COUNT exec_export(COUNT, TEXT *);
#else
   COUNT  cria_ibp( );
   VOID abre_rlog( );
   COUNT exec_export( );
#endif

extern FILE *fpo_exp;

COUNT idx_otimiza;

F__GLB   COUNT index_scan(sele, tipo)
COUNT sele;
COUNT tipo;   /* C-completa   P-parcial */
{  COUNT ret, st_arq, col, abrevia, novo_status, locked=0, fim_normal=0;
   LONG tot_index, tot_toind, nreg;
   UCOUNT magico;
   POINTER pos;
   Reg_controle *rc;
   TEXT base[TB+1];
   COUNT t1, t2;
	TEXT aux[100], chave[15], *p;
   TEXT *path;
	TEXT n3[PATH_MAX]; /* Arquivo de indices */
   TEXT *ajuda_ant = win_ajuda;
   WIN  *j0, *jm, *ji, *jf;
#ifdef UNIX
#ifndef CTSRVR
   int uid, gid;
   struct stat st;
#endif
#endif

   win_ajuda = "INDEXA";

   ap_operacao = REINDEX;

   up_str(GRP_REIND);

   j0 = jm = ji = jf = NULL;
   tot_index = tot_toind = 0;
   abrevia = 0;

   j0 = abre_janela(J_WINREIN);

   ji = abre_insercol(C_IX_INDEX);
   jm = abre_mensagem(str_nada);
	jf = abre_funcoes(funcao("E"));

	if (WCB == NULL || WCB->cb_fordem == 0 ) {
      ret = 0;
      goto fim;
   }

   magico = WBS->bs_idbase;
   strcpy(base, WBS->bs_nome);
   strcpy(aux,  WBS->bs_path);

   if ( tipo == COMPLETA ) {

      ret = decida(TW_OT_D00, TW_OT_D99, H_OTIMIZA, 2, 39, "%s", TW_OT_D01);

      if ( ret < 0 )
         goto fim;
      else if ( ret == 1 )
         idx_otimiza = 'T';
      else
         idx_otimiza = 'E';

   	lib_ativa(sele);

      if ( LOCK_RECURSO(base, aux, LCK_TRAVA) != 0 ) {
         mens_erro(H_NOLOCK, E_NOLOCK);
         ret = -1;
         goto fim;
      }

      locked = 1;


      st_arq = info_base(base, aux, LB_3_EXT, n3);

#ifdef UNIX
#ifndef CTSRVR
      if ( stat( n3, &st ) != -1 ) {
         uid = st.st_uid;
         gid = st.st_gid;
      }
      else {
         uid = gid = -1;
      }
#endif
#endif

      switch ( st_arq ) {
         case  0  :  break;
         default  :  if ( DELFILE(n3) != NO_ERROR ) { /*  elimina arquivo de indice antigo  */
                        mens_erro(H_DLF_SO, E_DLF_SO, n3);
                        ret = -1;
                        goto fim;
                     }

      }

      /* cria novo arquivo de index */
      if ( cria_ibp(sele, n3, ap_nserie, magico) != 0 ) {
         ret = -1;
         goto fim;
      }

#ifdef UNIX
#ifndef CTSRVR
      if ( uid != -1 )
         chown( n3, uid, gid );
#endif
#endif


      if ( (ret = open_base(base, REINDEX)) != 0 ) {
		   ret = -1;
         goto fim;
      }
   }


   mens_1(jm, M_AGUARDE);

   strcpy((path = n3), aux);


   if ( tipo == COMPLETA )
      ret = look_regs(sele, NULL, &tot_toind, NULL, NULL);
   else
      ret = look_regs(sele, NULL, NULL, NULL, &tot_toind);

   if ( ret < 0 ) {
       ret = -1;
       goto fim;
   }


   fw_title(j0, C_IX_REINDEX, j0->w_atrib);
	fwbox(j0);

   if ( tipo == COMPLETA )  p = C_IX_FRT1;
   else                     p = C_IX_FRP1;

   t1 = strlen(p);
   t2 = strlen(WBS->bs_nome);

   col = (j0->w_alen - ( t1 + t2 + 1)) / 2;

   mvwprintw(j0 ,  0, col, p);
   mvwprintw(j0 ,  0, col + t1 + 1, WBS->bs_nome);

   sprintf(aux, "%s %ld", C_IX_TOIND, tot_toind);

   t1 = strlen(aux);

   col = (j0->w_alen - t1) / 2;

   mvwprintw(j0 ,  1, col, aux);
   fwrefresh(j0);


   /* identifica numero do primeiro registro a ser indexado */
   if ( tipo == COMPLETA ) {
      nreg = 1;
   }
   else {
      pos = FRSKEY(AI_NUM(sele) + TREE_NAO_INDEX , ap_chave_ret);
      if ( uerr_cod != 0 ) {
         ret = -2;
         goto fim;
      }
      if ( pos != (POINTER) 0 ) nreg = atol(ap_chave_ret);
      else                      nreg = 0L;
   }

   col = strlen(C_IX_1WORK) + 1;
   mens_1(jm, C_IX_1WORK);

   rc = &(WCB->cb_controle);

   while ( nreg != 0 ) {

      ret = read_record(sele, nreg, 1);

      if ( ret < 0 ) {
         ret = -3;
         goto fim;
      }

      else if ( ret == 1 ) { /* fim */
            ret = 0;
            nreg = 0L;
            break;
      }
      else if ( ret == 0 ) {
            if ( tipo == PARCIAL &&
               ( rc->rc_status != ATIVO_NAO_INDEX && rc->rc_status != NO_INDEX) ) {
               ret = -2;
               goto fim;
            }

            fwat(col, 0, jm);
            fwprintf(jm, "%ld", ++tot_index);
            fwrefresh(jm);
      }

      else if ( ret == 2 || ret == 4 ) { /* disponivel ou novo */

         if ( tipo == COMPLETA ) {
            fwat(col, 0, jm);
            fwprintf(jm, "%ld", ++tot_index);
            fwrefresh(jm);

            /* inclui registro na arvore de deletados*/
            sprintf(chave, "%9.9ld", nreg);
            ret = pal_index(sele, TREE_DELETADOS, (UTEXT *)chave, strlen(chave) + 1);
            if ( ret != 0 ) {
               ret = -2;
               goto fim;
            }
         }
         else
            numdebug(1101);  /* inconsistencia de indices. Reindexe total */

         goto proximo;
      }
      else {
         numdebug(1102); /* tipo de registro de controle nao previsto */
         ret = -1;
         break;
      }

      /**/
      /***** indexacao do registro ******/
      /**/

      if ( tipo == COMPLETA ) {
         if ( salva_unica(sele, INCLUSAO, '+') != NULL ) {
            TEXT buf[250];
            if ( fpo_exp == NULL ) {
               abre_rlog( aux );
            }

            exec_export( sele, buf);

            del_record(sele, nreg); /* para eliminar registro com chave unica duplicada */
            goto proximo;
            /*
            ret = -2;
            goto fim;
            */
         }
      }


      if ( abrevia == 0 ) {
         if ( reg_index(sele, INCLUSAO, '+', nreg, ACUMULA_IDX) != 0 ) {
            ret = -1;
            goto fim;
         }
         novo_status = ATIVO_INDEX;
      }
      else {
         sprintf(chave, STR_9P9D, nreg);

         if ( pal_index(sele, TREE_NAO_INDEX,
               (UTEXT *)chave, strlen(chave) + 1) != 0 ) {
            mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
            ret = -1;
            break;
         }
         novo_status = ATIVO_NAO_INDEX;
      }

      /* retira da arvore de nao indexados */
      if ( tipo == PARCIAL ) {
         sprintf(chave, STR_9P9D, nreg);

         if ( pal_d_index(sele, TREE_NAO_INDEX,
            (UTEXT *)chave, strlen(chave) + 1) != 0 ) {
            ret = -2;
            goto fim;
         }
      }

      if ( atu_ac(sele, novo_status, rc->rc_pos) < 0 ) {
         ret = -1;
         goto fim;
      }


      LOKREC(AC_NUM(sele), FREE, WCB->cb_aclock);
      WCB->cb_aclock = (POINTER) 0;

      /**/
      /*** proximo registro */
      /**/

proximo :

      if ( tipo == COMPLETA ) {

         nreg++;

         if ( abrevia == 0 && w_kbhit(j0) && fwnqgc(j0) == K_ESC ) {
            if ( decida(TW_IC_D00, TW_IC_D99, H_CFICCAN, 4, 44, STR_QUAT,
                        TW_IC_D01, TW_IC_D02, TW_IC_D03, TW_IC_D04 ) == 1 ) {
               abrevia = 1;
               mens_1(jm, C_IX_2WORK);
               col = strlen(C_IX_2WORK) + 1;
            	funcoes(jf, "");
            }
         }
      }
      else {
         sprintf(chave, STR_9P9D, nreg);
         pos = GTKEY(AI_NUM(sele) + TREE_NAO_INDEX, chave, ap_chave_ret, 9);
         if ( uerr_cod != 0 ) {
            ret = -2;
            goto fim;
         }
         if ( pos != (POINTER) 0 ) nreg = atol(ap_chave_ret);
         else                      nreg = 0L;

         if ( w_kbhit(j0) && fwnqgc(j0) == K_ESC ) {
            if ( decida(TW_IP_D00, TW_IP_D99, H_CFIPCAN,
                        2, 30, STR_DUAS, TW_IP_D01, TW_IP_D02 ) == 1 ) {
               nreg = 0L;
               goto fim;
            }
         }
      }
   }


   mens_1(jm, str_nada);

   fim_normal = 1;

   fim_index();

   if ( locked ) {
      LOCK_RECURSO(base, path, LCK_DESTRAVA);
      locked = 0;
   }

   if ( tipo == COMPLETA )
      ret = close_base( );

   if ( fpo_exp != NULL ) {
      fclose( fpo_exp);
   }

   if ( fpo_exp != NULL ) {
       TEXT mens_x[100];
       sprintf( mens_x, "%s : %s", C_IX_FINAL, aux);
       men_conf(mens_x, "*", win_ajuda, NULL);
   }

   fpo_exp = NULL;

/* Retirei em 7/08/92 pois estava dando dois opens
      if ( sel_base(BASE_EDIT, base, M_SELBASE) != 0 ) {
		   ret = -1;
         goto fim;
      }
*/

fim :

   if ( ! fim_normal ) {
      if ( tot_index > 0L )
         fim_index();
      if ( tipo == COMPLETA )
         ret = close_base( );
      if ( locked ) {
         LOCK_RECURSO(base, path, LCK_DESTRAVA);
      }
   }




   if ( tipo == COMPLETA ) {
      if ( sel_base(BASE_EDIT, base, M_SELBASE) != 0 ) {
		   ret = -1;
         goto fim;
      }
   }

   switch( ret ) {
      case -2 :   /* Arquivo de indices nao esta Ok */
                  mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
                  break;

      case -3 :   /* Arquivo de controle nao esta Ok */
                  mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
                  break;
   }

	fecha_funcoes(jf);
	fecha_mensagem(jm);
   fecha_insercol(ji);

	if ( j0 != NULL ) {
      while( w_kbhit(j0)) w_rgc(j0);  /* limpa teclado */
      fwkill(j0);
   }

   down_str(GRP_REIND);
   win_ajuda = ajuda_ant;

   idx_otimiza = 'E';

   if ( fpo_exp != NULL ) {
      fclose( fpo_exp);
      fpo_exp = NULL;
   }

   ap_operacao = 0;

   return(ret);
}


F__LOC   COUNT cria_ibp(sele, n3, nserie, magico)
COUNT sele;
TEXT *n3;
LONG nserie;
UCOUNT magico;
{  COUNT ret = 0, num, narq;
   COUNT varl = PERMANENT | EXCLUSIVE | VLENGTH;

   narq = INI_SELE(sele) + 2;


   /**
   ***   CRIA NOVO ARQUIVO DE INDICE DOS DADOS
   **/
   if ( CREIDX(narq, n3, LB_KEYMAX, 0, AI_INDICES - 1,
           SLOT, varl, nserie, magico ) != 0 ){
      mens_erro(H_CRARQ, E_CRF_MA, ret, n3);
		ret = -1;
      goto fim;
   }
   else {
      /* cria outros indices do arquivo */
      for (num = 1; num < AI_INDICES; num++) {
         COUNT tam_key;

         tam_key = (num == TREE_UNIQ) ? LB_KEYCT : LB_KEYMAX;

         if ( CREMEM(narq, tam_key, 0, num, nserie, magico ) != 0 ) {
            mens_erro(H_CRARQ, E_CRF_MA, ret, n3);
		      ret = -1;
            goto fim;
         }
      }
   }

   ret = CLSFIL(narq, COMPLETE);

   if ( ret != 0 ) {
      mens_erro(H_CLSANO, E_CLS_MA, uerr_cod);
      ret = -1;
   }

fim :

   return(ret);
}


VOID abre_rlog( aux )
TEXT *aux;
{

   strcpy(aux,  WBS->bs_path);

	strcat(aux, STR_BARRA);
#ifndef MSDOS
	strlwr(aux);
#endif
   strcat(aux, "log__XXXXXX");

   if ( (char *)mktemp(aux) == NULL || (fpo_exp = fopen(aux, "w")) == NULL ) {
      debug("Erro na criacao de arquivo de log");
   }

}
