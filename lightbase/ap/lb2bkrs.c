#include  "lb2all.h"

#include  <string.h>
#include  <time.h>
#include  HD_DIR
#include  HD_IO


#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif


#define   NUM_CONFIG 6

#define   GRP_BKREST   31


#define   H_CFREST      "CF_RESTORE"   /* confirma Restore */
#define   H_CFBACK      "CF_BACKUP"   /* confirma Backup */

#define   H_BKENV       "E_BKENV"      /* nao conseguir preparar ambiente  */
#define   H_BKFAL       "E_BKFAL"      /* nao conseguir executar backup  */
#define   H_BKHDR       "E_BKHDR"      /* nao conseguir atualizar header  */
#define   H_BKEXC       "E_BKEXC"      /* nao conseguir abrir base exclusivo  */



#define   TW_BK_D1  loc_str(GRP_BKREST, 1)
#define   TW_BK_D2  loc_str(GRP_BKREST, 2)

#define   TW_RS_D1  loc_str(GRP_BKREST, 3)
#define   TW_RS_D2  loc_str(GRP_BKREST, 4)

#define   M_SELREST loc_str(GRP_BKREST, 5)

#define   E_BKENV   loc_str(GRP_BKREST, 501)
#define   E_BKFAL   loc_str(GRP_BKREST, 502)
#define   E_BKHDR   loc_str(GRP_BKREST, 503)
#define   E_BKEXC   loc_str(GRP_BKREST, 504)


#ifdef MSDOS
   COUNT trava_base(Base *, TEXT *);
   COUNT solta_base(Base *);
   COUNT hdr_inclose(Base *, COUNT, COUNT);
#else
   COUNT trava_base( );
   COUNT solta_base( );
   COUNT hdr_inclose( );
#endif




F__GLB   VOID restore(sele)
COUNT sele;
{  COUNT ret, travou;
   TEXT  base_ori[TB+1], *nbase;
   TEXT  lin_exec[200], *path_rs;
   Base  *wbs;

   up_str(GRP_BKREST);

   wbs = NULL;
   travou = 0;

   base_ori[0] = '\0';

   if ( decida(str_vazia, TW_RS_D1, H_CFREST, 1, 34, "%s", TW_RS_D2) != 0 ) {
      ret = 0;
      goto fim;
   }


	if (WCB != NULL && WCB->cb_fordem != 0 ) {
      strcpy(base_ori, WBS->bs_nome);
   	lib_ativa(sele);
   }


   if ( (nbase = uma_base(8, base_ori, 1, 0, M_SELREST)) == NULL ) {
      ret = 0;
      goto fim;
   }

   if ( strcmp(nbase, NO_BASE) == 0 ) {
      ret = 0;
      goto fim;
   }

   if ( ( wbs = (Base *) calloc(1, sizeof(Base))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto fim;
   }

	if ( ! load_rec( ace_dnum, ace_inum, D0_ID, nbase,
                     wbs, sizeof(Base), '=') ) {
      mens_erro(H_BADAC, E_BADAC);
      goto fim;
   }


   if ( LOCK_RECURSO(nbase, wbs->bs_path, LCK_TRAVA) == 0 ) {
      /* Conseguiu travar OK */
      travou = 1;
   }
   else {
      /* Base em uso */
		ret = -4;
		goto fim;
   }



   /* se existir rotina de restore no diretorio da base use a propria */
#ifdef MSDOS
   ret = info_base(ap_rest_lb, wbs->bs_path, ".BAT", lin_exec);
#else
   ret = info_base(ap_rest_lb, wbs->bs_path, str_nada, lin_exec);
#endif

   if ( (ret & PODE_LER) == 0)
      path_rs = ap_home_lb;
   else
      path_rs = wbs->bs_path;


   /* lb_home+BARRA+comando + nome_base + path_base + nome_arq_temporario */
   sprintf(lin_exec, "%s%s%s %s %s",
	   path_rs, STR_BARRA,  ap_rest_lb, nbase, wbs->bs_path);

#ifndef MSDOS
	strlwr(lin_exec);
#endif

   win_shell(lin_exec);


   ret = 0;
/* restore nao obriga reprocessar mais a base 1.8 r 4.0 */
/* ret = hdr_inclose(wbs, BASE_MODI, 0); */

fim :

   if ( travou )
      LOCK_RECURSO(nbase, wbs->bs_path, LCK_DESTRAVA);

   switch ( ret ) {
      case -1 :   /* nao conseguir preparar ambiente */
		  mens_erro(H_BKENV, E_BKENV);
		  break;

      case -2 :   /* nao conseguir executar backup */
		  mens_erro(H_BKFAL, E_BKFAL);
		  break;

      case -3 :   /* nao conseguir atualizar header */
		  mens_erro(H_BKHDR, E_BKHDR);
		  break;

      case -4 :   /* nao conseguir abrir base exclusivo */
		  mens_erro(H_BKEXC, E_BKEXC);
		  break;
   }


   if ( wbs != NULL ) free(wbs);

   if ( base_ori[0] != '\0' ) {
      if ( strcmp(base_ori, nbase ) != 0 )
         sel_base(sele, base_ori, M_SELBASE);
   }


   down_str(GRP_BKREST);
}





F__GLB   VOID backup(sele)
COUNT sele;
{  COUNT ret, criou, travou;
   TEXT base[TB+1];
   TEXT aux[PATH_MAX+1], *path, drive[3];
   TEXT lin_exec[200], *path_rs;
   FILE *fp_ctl = NULL;
   Base  *wbs;

   up_str(GRP_BKREST);

   criou = 0;
   travou = 0;
   base[0] = '\0';


	if (WCB == NULL || WCB->cb_fordem == 0 ) {
      goto sai;
   }

   strcpy(base, WBS->bs_nome);

   if ( decida(str_vazia, TW_BK_D1, H_CFBACK, 1, 34, "%s", TW_BK_D2) != 0 ) {
      ret = 0;
      goto sai;
   }

   if ( ( wbs = (Base *) calloc(1, sizeof(Base))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto sai;
   }

   memcpy(wbs, WBS, sizeof(Base));

	lib_ativa(sele);

   if ( LOCK_RECURSO(wbs->bs_nome, wbs->bs_path, LCK_TRAVA) == 0 ) {
      /* Conseguiu travar OK */
      travou = 1;
   }
   else {
      /* Base em uso */
		ret = -4;
		goto fim;
   }


#ifdef MSDOS
   path = &wbs->bs_path[2];
   strcpy(aux,  wbs->bs_path);
   drive[0] = wbs->bs_path[0];
   drive[1] = wbs->bs_path[1];
   drive[2] = '\0';
#else
   drive[0] = '\0';
   path = wbs->bs_path;
   strcpy(aux,  wbs->bs_path);
#endif

	strcat(aux, STR_BARRA);
#ifndef MSDOS
	strlwr(aux);
#endif
   strcat(aux, "lbXXXXXX");

   if ( (char *)mktemp(aux) == NULL ) {
      ret = -1;
      goto fim;
   }

   criou = 1;

   if ( (fp_ctl = fopen(aux, "w")) == NULL ) {
      ret = -1;
      goto fim;
   }

   fprintf(fp_ctl, "%s\n", path);

   if ( fclose(fp_ctl) != 0 )
      numdebug(1203);

   /* se existir rotina de backup no diretorio da base use a propria */
#ifdef MSDOS
   ret = info_base(ap_rest_lb, wbs->bs_path, ".BAT", lin_exec);
#else
   ret = info_base(ap_rest_lb, wbs->bs_path, str_nada, lin_exec);
#endif

   if ( (ret & PODE_LER) == 0)
      path_rs = ap_home_lb;
   else
      path_rs = wbs->bs_path;



   /* lb_home+BARRA+comando + nome_base + path_base + nome_arq_OK + drive */
   sprintf(lin_exec, "%s%s%s %s %s %s %s",
	   path_rs, STR_BARRA,  ap_back_lb, base, path, aux, drive);

#ifndef MSDOS
	strlwr(lin_exec);
#endif

   win_shell(lin_exec);

   criou = 0;

   if ( access(aux, 0 ) == 0 ) {  /* backup nao matou arquivo. FALHOU */
      unlink(aux); /* remove arquivo criado */
      ret = -2;
      goto fim;
   }

   /* backup executado com sucesso. atualiza header */

   if ( hdr_inclose(wbs, 'Z', 'B') != 0 ) {
/**   if ( hdr_inatu( sele, 2 ) != 0 ) { **/
      ret = -3;
      goto fim;
   }

   ret = 0;


fim:

   if ( travou )
      LOCK_RECURSO(base, wbs->bs_path, LCK_DESTRAVA);

   if ( wbs != NULL ) free(wbs);


   switch ( ret ) {
      case -1 :   /* nao conseguir preparar ambiente */
		  mens_erro("E_BKENV", E_BKENV);
		  break;

      case -2 :   /* nao conseguir executar backup */
		  mens_erro("E_BKFAL", E_BKFAL);
		  break;

      case -3 :   /* nao conseguir atualizar header */
		  mens_erro("E_BKHDR", E_BKHDR);
		  break;

      case -4 :   /* nao conseguir abrir base exclusivo */
		  mens_erro("E_BKEXC", E_BKEXC);
		  break;
   }


   sel_base(sele, base, M_SELBASE);

sai :

   if (criou )
      unlink(aux); /* remove arquivo criado */

   down_str(GRP_BKREST);
}



F__LOC   COUNT hdr_inclose(wbs, new_status, touch)
Base *wbs;
COUNT new_status;
COUNT touch;  /* 'B' - atualiza data de Backup */
{  COUNT ret;
	TEXT n4[PATH_MAX]; /* Arquivo de descricao */
	time_t  tempo;

   ret = 0;

   if ( (info_base(wbs->bs_nome, wbs->bs_path, LB_4_EXT, n4) & PODE_LER) == 0){
      ret = -1;
      goto fim;
   }

   if ( open_lb4(wbs, n4, 'D', ap_nserie, wbs->bs_idbase ) != 0 ) {
      ret = -2;
      goto fim;
   }

   /*** marca como LOCK ****/

   time(&tempo);            /* data de atualizacao da base */

   if ( touch == 'B' )
	wbs->bs_bk_data = tempo;

   if ( new_status != 'Z' )
      wbs->bs_status  = new_status;

	if ( WRTVREC(lb4_dnum, lb4_pos, (TEXT *)wbs, sizeof(Base)) != 0 ) {
      mens_erro(H_LB4BAD, E_LB4BAD, uerr_cod);
      ret = -1;
      goto fim;
   }
   else
      FLUSHFIL(lb4_dnum);

fim :

   close_lb4();

   return(ret);
}
