#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>
#include  <time.h>
#include  HD_MEMO
#include  HD_DIR
#include  HD_IO

#include  "lb2entra.h"
#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#include  "lb2db.h"

#define   LF13E       "1 3 E"
#define   STR_DUAS    "%s\n%s"

#define   H_CFCRB    "CFCRIABA" /* confirmacao de criacao da base */
#define   H_CRIPTO   "CODCRIPT" /* codigo a ser utilizado na criptografia */


#define   TW_BSC0    loc_str(GRP_DEFBASE, 29)
#define   TW_BSC9    loc_str(GRP_DEFBASE, 30)
#define   TW_BSC1    loc_str(GRP_DEFBASE, 31)
#define   TW_BSC2    loc_str(GRP_DEFBASE, 32)

#define   M_CRBA1    loc_str(GRP_DEFBASE, 33)
#define   M_CRBA2    loc_str(GRP_DEFBASE, 34)

#define   T_CODCRIP    loc_str(GRP_DEFBASE, 74)
#define   R_CRIPT      loc_str(GRP_DEFBASE, 75)


#ifdef MSDOS
   COUNT cria_dir(TEXT *);
#else
   COUNT cria_dir();
#endif

/**
**#  ---------------------------------------------------------------------
**#   Funcao...: cria_base
**#   Objetivo.: Cria os arquivos necessarios para uma base de bados
**#   Entrada..: ANANA
**#   Retorna..:  0  - se criacao OK
**#               -1 - Erro na criacao da base
**#
**/
F__GLB   COUNT cria_base( dir_acriar )
TEXT *dir_acriar;
{  COUNT i, ret, num, narq;
   UCOUNT id;
   LONG ns;
   TEXT nx[PATH_MAX], *pnx;
   time_t tempo;
   POINTER pos;
   COUNT fixo = VIRTUAL | EXCLUSIVE | FIXED;
   COUNT varl = VIRTUAL | EXCLUSIVE | VLENGTH;
   WIN *jm = NULL;


   ret = 0;

   if ( decida(TW_BSC0, TW_BSC9, H_CFCRB,
               2, 28, STR_DUAS, TW_BSC1, TW_BSC2) != 0 ) {
      ret = -1;
      goto fim;
   }

   jm = abre_mensagem(str_vazia);

   /* se necessario, criar diretorio */
   if ( dir_acriar != NULL ) {
      mens_1(jm, M_CRBA1 );
      if (cria_dir(dir_acriar) != 0 ){
         mens_erro(H_CRBA1, E_CRBA1, dir_acriar);
         ret = -1;
         goto fim;
      }
   }


   mens_1(jm, M_CRBA2 );

   /**
   ***   INICIALIZA HEADER
   **/
   strcpy(WBS->bs_dono, ap_nome);
   strcpy(WBS->bs_grupo, ap_grupo);

   time(&tempo);
   WBS->bs_cr_data = tempo;
   WBS->bs_al_data = tempo;
   WBS->bs_bk_data = (time_t)0;


   if ( def_seguranca(WBS) == -1 ) {
      ret = -1;
      goto fim;
   }

   id = WBS->bs_idbase;

   WBS->bs_versao   = VERSAO_CORRENTE;
   WBS->bs_u_seq    = 0;
   WBS->bs_n_vis    = 0;
   WBS->bs_n_rel    = 0;

   WBS->bs_status  = 0;
   WBS->bs_sepdecimal = sep_dec;

   WBS->bs_swpos  = (POINTER)0;
   WBS->bs_cppos  = (POINTER)0;
   WBS->bs_vspos  = (POINTER)0;
   WBS->bs_rlpos  = (POINTER)0;

   WBS->bs_marca[0] = 'B';
   WBS->bs_marca[1] = 'S';



   info_base(WBS->bs_nome, WBS->bs_path, LB_1_EXT, nx);

   /* faz pnx apontar para numero do arquivo LB? */
   pnx = strchr(nx, '\0');
   pnx--;

   narq = lb4_dnum;
   ns = ap_nserie;


   for ( i = 1; i <= 4; i++ ) {
      *pnx = '0' + i;  /* Monta nome do arquivo */

      if ( access(nx, 0) == 0 )
         DELFILE(nx);

      ct_finger  = WBS->bs_finger;
      ct_release = WBS->bs_release;
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

         case 4 : /* Arquivo de descricao da base*/
                  CREDAT(narq, nx, 0, SLOT, varl, ns, id );

                  CLSFIL(narq, COMPLETE);
                  if ( OPNFIL(narq, nx, VIRTUAL | EXCLUSIVE, ap_nserie, id) != 0 ) {
                     mens_erro( H_CRBA2, E_CRBA2 );
                     ret = -1;
                     break;
                  }

                  /* gravacao do header da base */
	               if ( (pos = NEWVREC(narq, sizeof(Base))) != (POINTER)0 ) {

                     lb4_pos = pos;

	                  if ( ! WRTVREC(narq, pos, (TEXT *)WBS, sizeof(Base))) {
                        if ( rec_stop() < 0  ) {
                           mens_erro(H_CRBA2, E_CRBA2);
                           ret  = -1 ;
                        }
                        else {  /* atualiza header do nova base */
                           if ( lb4_upd(1) != 0 )
                              ret = -1;
                        }

                     }

                     LOKREC(narq, FREE, pos); /* so tera um usuario usando */
                  }
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

   /* inclui registro no cadastro de acessos */
   if ( ! add_rec(ace_dnum, ace_inum, D0_ID,
                  WBS->bs_nome, WBS, sizeof(Base))) {
      mens_erro(H_CRBA3, E_CRBA3, uerr_cod);
	   ret = -1;
   }



fim :

	if ( jm ) fecha_mensagem(jm);

   return(ret);
};

/*
**#  ---------------------------------------------------------------------
**#   Funcao...: cria_dir(path)
**#   Objetivo.: criar diretorio
**#   Obs.     : Sempre cria em minusculo convertendo barras
**#
**/
F__LOC   COUNT cria_dir(path)
TEXT *path;
{  COUNT ret;
   TEXT *p, c;
#ifndef MSDOS
   TEXT temp[PATH_MAX + 10];
#endif

   for ( p=path; *p != '\0' ; p++) {
      c = *p;
      if ( c == OUTRA_BARRA) c = BARRA;
      if ( c >= 'A' && c <= 'Z' ) *p = c + ' '; /* tudo minusculo */
   };


#ifdef MSDOS

   c = toupper(path[0]);

   if ( path[1] == ':' && (c >= 'A' && c <= 'Z') )  {
      if ( strlen(path) == 2 ) { ret = 0 ; goto fim; };
      if ( strlen(path) == 3 && path[2] == BARRA ) { ret = 0 ; goto fim; };
   }
   ret = mkdir(path);
#else
   sprintf(temp, "/bin/mkdir %s", path);
   ret = system(temp);
#endif

fim :

   return(ret);
}


