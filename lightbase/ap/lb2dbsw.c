#include  "lb2all.h"

#include  <string.h>
#include  HD_DIR
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#define   GRP_DEFBASE    16

#define     LF1235E       "1 2 3 5 E"

#define   H_NOSTP    "ERRDFSW"   /* Falta de aquivo default de stopword */
#define   H_LDSTP    "ERRLDSW"   /* Erro na carga de stop word para edicao */
#define   H_EDSTP    "ERREDSW"   /* Erro na edicao de stop word */
#define   H_WRSTP    "ERRWRSW"   /* Erro na gravacao de stop word */
#define   H_GRVSW    "ABMODSW"   /* Abandona modificacoes em stop words */


#define   T_STOPW      loc_str(GRP_DEFBASE, 39)
#define   I_STOPW      loc_str(GRP_DEFBASE, 40)
#define   M_STOPW      loc_str(GRP_DEFBASE, 41)


#define   E_NOSTP      loc_str(GRP_DEFBASE, 42)
#define   E_LDSTP      loc_str(GRP_DEFBASE, 43)
#define   E_EDSTP      loc_str(GRP_DEFBASE, 44)
#define   E_WRSTP      loc_str(GRP_DEFBASE, 45)

#define   STP_FILE     loc_str(GRP_DEFBASE, 46)





F__GLB   COUNT rec_stop()
{
   COUNT ret, largura;
   LONG tam_ant;
	POINTER pos, pos_ant;
   WIN *je, *jf, *ji;
   TEXT aux[PATH_MAX +10];
   TEXT *ajuda_ant = win_ajuda;
   UTEXT lin_escape[20];
   ARG *adoc_stp;

   je = jf = ji = NULL;

   win_ajuda    = "STOPWORD";

   adoc_stp = NULL;
   ret = 0;
   tam_ant = 0;


   pos = WBS->bs_swpos;

	je = abre_janela(J_STOPWORD);
   fw_title(je, T_STOPW, je->t_atr);
   fwbot(je);

   largura = je->w_len;


   if ( pos == (POINTER) 0 ) {  /* Inicializacao */

      ret = info_base(STP_FILE, ap_home_lb, str_nada, aux);

      if ( ret < 0 || (ret & PODE_LER) == 0 ) {
         mens_erro(H_NOSTP, E_NOSTP);
         ret = -1;
         goto fim;
      }

      adoc_stp = load_buffer(largura, ET_FILE, aux, 65000L );
   }
   else {  /* edicao */
      adoc_stp = txt_load(lb4_dnum, pos, &tam_ant , largura, 0L);
   }


   if ( adoc_stp == NULL ) {
      mens_erro(H_LDSTP, E_LDSTP);
      ret = -1;
      goto fim;
   }


   if ( pos != (POINTER)0 ) {

      pr_escape(LF1235E, lin_escape);
      adoc_stp->escape = (TEXT *)lin_escape;
      adoc_stp->def_mens = M_STOPW;

	   jf = abre_funcoes(funcao(LF1235E));
      ji = abre_insercol(I_STOPW);

      fwtop(je);
      ret = edit_buffer(adoc_stp, ET_EDITA , je, NULL, 0);

      fecha_insercol(ji);
      fecha_funcoes(jf);

      if ( ret < 0 ) {
         mens_erro(H_EDSTP, E_EDSTP);
         goto fim;
      }

      if ( ret == K_ESC ) {
         if ( adoc_stp->f_mod != 0 ) {
            if ( decida(TW_GGV0, TW_GGV9, H_GRVSW,
                     3, 42, "%s\n%s", TW_GGV1, TW_GGV2) != 0 )
               goto fim;
         }
      }
   }

   if ( adoc_stp->f_mod != 0  || pos == (POINTER) 0 ) {

      pos_ant = pos;

      if ( pos_ant != (POINTER) 0 )
         txt_free(lb4_dnum, pos_ant);

      pos = txt_save(lb4_dnum, adoc_stp, 0L, 0);

      if ( pos != (POINTER) 0 ) {
         WBS->bs_swpos = pos;
         WBS->bs_updated = 1;
      }
      else {
         ret = -1;
         mens_erro(H_WRSTP, E_WRSTP);
         goto fim;
      }
   }

   ret = 0;

fim :

   if ( adoc_stp != NULL ) {
      free_buffer( adoc_stp );
      adoc_stp = NULL;
   }

	if ( je ) fwkill(je);
   win_ajuda    = ajuda_ant;
   return(ret);
}





