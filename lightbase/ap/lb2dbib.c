#include  "lb2all.h"

#include  <string.h>
#include  <time.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"


#include  "lb2db.h"



#ifdef MSDOS
   COUNT imp_efetiva( COUNT);
   COUNT at_numserie(COUNT);
   COUNT set_nshdr(TEXT *, LONG, UCOUNT, LONG);
   COUNT inc_em_acessos(VOID);
#else
   COUNT imp_efetiva( );
   COUNT at_numserie( );
   COUNT set_nshdr( );
   COUNT inc_em_acessos( );
#endif

#define   H_IBPERM   "FR_PERMI"    /* importa base : permissao */
#define   H_IBNUMSE  "ID_NUMSE"    /* importa base : numero de serie */
#define   H_IBATIV   "ID_ATIVA"    /* importa base : chave de ativacao */

#define   H_IBPRINV   "IBPERMIN"  /* permissao invalida na importacao da base */
#define   H_IBCHINV   "IBCHAVIN"  /* chave de ativacao incompativel */
#define   H_IBNSINV   "IBNUMSIN"  /* numero de serie diferente */
#define   H_IBUPD4    "ERIBUPD4"  /* Erro na atualizacao da descricao da base */
#define   H_IBATHDR   "ERATHDRA"  /* erro na atualizacao do header do arquivo */
#define   H_IBACES    "IBFILEAC"  /* arquivo nao existe ou nao pode ler/gravar */


#define   R_IBPERM      loc_str(GRP_DEFBASE, 50)
#define   R_IBNUMSE     loc_str(GRP_DEFBASE, 51)
#define   R_IBCHATV     loc_str(GRP_DEFBASE, 52)

#define   T_IBPERM      loc_str(GRP_DEFBASE, 53)
#define   T_IBNUMSE     loc_str(GRP_DEFBASE, 54)
#define   T_IBCHATV     loc_str(GRP_DEFBASE, 55)

#define   M_INCACE      loc_str(GRP_DEFBASE, 56)

#define   E_IBPRINV     loc_str(GRP_DEFBASE, 57)
#define   E_IBCHINV     loc_str(GRP_DEFBASE, 58)

#define   E_IBNSINV     loc_str(GRP_DEFBASE, 520)
#define   E_IBUPD4      loc_str(GRP_DEFBASE, 521)
#define   E_IBATHDR     loc_str(GRP_DEFBASE, 522)




F__GLB   COUNT inport_base(sele)
COUNT sele;
{  COUNT ret, le;
   LONG id_long;
	TEXT strserie[20], activkey[20], permissao[20];
   DTela perm[2], numse[3];
   WIN *j1 = NULL, *j2 = NULL;

   seta_dtela(&perm[0], permissao, X14, R_IBPERM, 0, 11,  0, E_ALFA, H_IBPERM, NULL, NULL, NULL );
   seta_dtela(&perm[1], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

   seta_dtela(&numse[0], strserie, X14, R_IBNUMSE, 0, 11,  0, E_ALFA, H_IBNUMSE, NULL, NULL, NULL );
   seta_dtela(&numse[1], activkey, X14, R_IBCHATV, 1, 11,  0, E_ALFA, H_IBATIV,  NULL, NULL, NULL );
   seta_dtela(&numse[2], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

   /**
   ***   se numero de serie original e < 200 a base e PUBLICA 
   **/

   if ( WBS->bs_nserie > 200L && WBS->bs_idbase > 0  ) {

      j1 = abre_janela(J_NUMWIN);
      j2 = abre_janela(J_ACTWIN);

	   fwbox(j1);
	   fwbox(j2);

	   mvwprintw(j1, 0, 0, T_IBPERM);

	   mvwprintw(j2, 0, 0, T_IBNUMSE);
	   mvwprintw(j2, 1, 0, T_IBCHATV);

      le = 1;

      while ( le > 0 ) {

         permissao[0] = strserie[0]  = activkey[0]  = '\0';
	      ed_read(j1, &perm[0],  1, SO_MOSTRA, "1 E");
         ed_read(j2, &numse[0], 1, SO_MOSTRA, "1 E");

         if ( le == 1 ) {

	         ret = ed_read(j1, &perm[0], 1, LIMPA_LE, "1 E");

	         if ( ret < 0 || ret == K_ESC ) {  /* erro ou ESC */
		         ret = -1;
               goto fim;
            }

            if ( permissao[0] == '\0' )
               le = 2;
            else{
               id_long = atol(permissao);
               if ( id_long == WBS->bs_idbase ){
                  le = 0;
                  break;
               }
               else mens_erro(H_IBPRINV, E_IBPRINV);
            }
         }

         if ( le == 2 ) {

	         ret = ed_read(j2, &numse[0], 1, LIMPA_LE, "1 E");

	         if ( ret < 0 || ret == K_ESC ) {  /* erro ou ESC */
		         ret = -1;
               goto fim;
            }

            if ( strserie[0] == '\0' )
               le = 1;
            else {
               if ( t_numse(strserie, activkey, 0L) != 0 ) {
                  mens_erro(H_IBCHINV, E_IBCHINV);
                  le = 1;
                  continue;
               }
               else {
                  LONG serr;

                  if ( strlen(strserie) > 6 )
                     serr = atol(strserie+6) + 100000L;
                  else
                     serr = atol(strserie);
                  if ( serr == WBS->bs_nserie ){
                     le = 0;
                     break;
                  }
                  else mens_erro(H_IBNSINV, E_IBNSINV);
               }
            }
         }
      }
   }

   ret = imp_efetiva(sele);

fim :

   if ( j2 != NULL ) fwkill(j2);
   if ( j1 != NULL ) fwkill(j1);

   return(ret);
}




F__LOC   COUNT imp_efetiva(sele)
COUNT sele;
{  COUNT ret;
   TEXT  aux[PATH_MAX+10];


   info_base(WBS->bs_nome, WBS->bs_path, LB_4_EXT, aux);

   if ( open_lb4(WBS, aux, 'I', WBS->bs_nserie, WBS->bs_idbase ) != 0 ) {
      ret = -1;
      goto fim;
   }


   WBS->bs_nserie = ap_nserie;

   WBS->bs_bk_data = (ULONG)0;

   strcpy(WBS->bs_dono,   ap_nome);
   strcpy(WBS->bs_grupo,  ap_grupo);

   if ( lb4_upd(1) != 0 ) {
      mens_erro(H_IBUPD4, E_IBUPD4);
	   ret = -1;
      goto fim;
   }

   if ( inc_em_acessos( ) != 0 ) {
	   ret = -1;
      goto fim;
   }


   /* atualiza numero de serie e idbase dos arquivos */
   if ( at_numserie(sele) != 0 ) {
      ret = -1;
      goto fim;
   }

   if ( ATUFILE(lb4_dnum, ap_nserie, WBS->bs_idbase) != 0 ){
      mens_erro(H_IBATHDR, E_IBATHDR);
		ret = -1;
      goto fim;
   }


fim :

   close_lb4();

   return(ret);
}




F__LOC   COUNT inc_em_acessos()
{  COUNT ret;
   WIN *jm = NULL;

   ret = 0;

   jm = abre_mensagem(M_INCACE);

   /* inclui registro no cadastro de acessos */
   if ( ! add_rec(ace_dnum, ace_inum, D0_ID,
                  WBS->bs_nome, WBS, sizeof(Base))) {
      mens_erro(H_CRBA3, E_CRBA3, uerr_cod);
	   ret = -1;
   }


fim :

	if ( jm ) fecha_mensagem(jm);

   return(ret);
}





F__LOC   COUNT at_numserie()
{  COUNT ret, i;
   LONG pos;
   TEXT nx[PATH_MAX], *pnx;


   ret = 0;

   /* so pega offset da estrutura do header do arquivo */
   pos = ATUFILE(-1, 0L, 0);



   info_base(WBS->bs_nome, WBS->bs_path, LB_1_EXT, nx);

   /* faz pnx apontar para numero do arquivo LB? */
   pnx = strchr(nx, '\0');
   pnx--;


   for ( i = 1; i <= 3; i++ ) {
      *pnx = '0' + i;  /* Monta nome do arquivo */

      if ( access(nx, 6) == 0 ) { /* read e write */
         if ( set_nshdr(nx, ap_nserie, WBS->bs_idbase, pos ) != 0 ){
            mens_erro(H_IBATHDR, E_IBATHDR);
		      ret = -1;
            goto fim;
         }
      }
      else {
         mens_erro(H_IBACES, E_NOACES, nx);
         ret = -1;
         goto fim;
      }
   }

fim :

   return(ret);
}


COUNT set_nshdr(nome, serie, idbase, pos)
TEXT *nome;
LONG serie;
UCOUNT idbase;
LONG pos;
{  COUNT ret=0, pega;
   FILE *fp = NULL;

#ifdef MSDOS
   fp = fopen(nome, "rb+");
#else
   fp = fopen(nome, "r+");
#endif

   if ( fp == NULL ) {
      ret = -1;
      goto fim;
   }

	pega  = (idbase != 0) ? 1 : 0;

   fseek(fp, pos, 0);
   fwrite( &serie, 4, 1, fp );
   fwrite( &idbase, 2, 1, fp );
   fwrite( &pega, 2, 1, fp );

fim :

   if ( fp != NULL ) 
      fclose(fp);

   return(ret);
}
