#include  "lb2all.h"

#include  <string.h>
#include  HD_MEMO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2prot1.h"


#include  "lb2entd.h"


#define   MASC_UNIC   "%s ........................................"


#ifdef MSDOS
   LONG     pega_nreg( COUNT, COUNT);
   COUNT    exec_iae( COUNT, COUNT, LONG, WIN*);
   LONG     psq_unica( COUNT, Campo *, TEXT *);
#else
   LONG     pega_nreg( );
   COUNT    exec_iae( );
   LONG     psq_unica( );
#endif




F__GLB   COUNT ent_dados(sele, operacao)
COUNT sele, operacao;
{  COUNT ret;
   WIN *win_ent;
   LONG nreg;

   up_str(GRP_ENTD);


   free_record(sele);

   memset(c_bases[sele]->cb_ioarea, '\0', c_bases[sele]->cb_tamrec);


   /* WWW mudanca para vista padrao de inclusao */
   if ( operacao == INCLUSAO && esp_base.presente && esp_base.vista_inclusao != -1)  {
      muda_vista(ap_vista.vis_ordem, esp_base.vista_inclusao);
      ap_vista.vis_old_ativo = -1;
   }


   /* teste se tem campo entrada */
   if ( varre_vista(sele, 1) != 1 ) {
      mens_erro(H_NOVIS, E_NOVIS);
      ret = -1;
      goto sai;
   }

   if ( ( win_ent = abre_janela(J_REGDISP)) == NULL ) {
      ret = -1;
      goto sai;
   }

   if ( operacao == EXCLUSAO )
      win_bell(BEL_ATTN);

   ret = 0;

   while ( 1 )  {

      switch( operacao ) {
         case INCLUSAO :   nreg = 0L;
                           break;

         case ALTERACAO:
         case EXCLUSAO :   nreg = pega_nreg(sele, operacao);

                           if ( nreg == -2 ) ret = -2;
                           else              ret = 0;

                           break;

      };

      if ( ret == -2 ) break;

      ret = exec_iae(sele, operacao, nreg, win_ent);
   };

   ret = 0;

fim :

   dn_textos(sele, 0);

   fwkill( win_ent );

sai :

   down_str(GRP_ENTD);

   return(ret);
}


F__LOC   LONG pega_nreg(sele, operacao)
COUNT sele, operacao;
{  COUNT ret, param, i, num_cps;
   WIN *jt;
   LONG nreg;
   TEXT *mens, ch_unica[100], ch_num[20], *temp, *s2;
   Campo **vet_cmp, *pc;
   DTela nr[3];
   COUNT a_sep_int, a_sep_dec;

   nr[0].destino = NULL;
   nr[1].destino = NULL;
   nr[2].destino = NULL;

   vet_cmp  = pos_vetcmp(sele);
   num_cps  = qtd_vetcmp(sele);

   temp = ch_unica; /* para economizar espaco */

   mens = M_GETNUM;

   /* verifica se encontra algum campo chave unica */

   for ( pc=NULL, i = 0; i < num_cps; i++ ) {

      if ( vet_cmp[i]->cp_chave_unica == 'S' ) {

         pc = vet_cmp[i];

         if ( pc->cp_tipo == 'N' ) param = E_VALOR;
         else                      param = E_ALFA;

         mens = M_GETREG;

         seta_dtela(&nr[1], ch_unica, pc->cp_mascara, mens,
                        3, 44, 25, param, H_CUNICA, NULL, NULL, NULL);

         if ( strlen(pc->cp_desc) > 0 ) {
            s2 = pc->cp_desc;
         }
         else
            s2 = pc->cp_nome;
         break;
      }
   }

   seta_dtela(&nr[0], ch_num, MASC_NREG, mens,
              (pc != NULL) ? 1 : 2 , 44, 0, E_VALOR, H_NREG, NULL, NULL, NULL);


   if ( (jt = abre_janela(J_NREG)) == NULL ) {
      nreg = -1;
      goto sai;
   }

   fw_title(jt, (operacao == ALTERACAO) ? T_ALTERA : T_EXCLUI, jt->w_atrib);
	fwbox(jt);

   sprintf(temp, MASC_UNIC,
      (operacao == ALTERACAO) ? S_REG_ALT : S_REG_EXC);

   temp [40] = '\0';

	mvwprintw(jt , (pc != NULL) ? 1 : 2 , 3, temp);

   if ( pc != NULL ) {
      sprintf(temp, MASC_UNIC, s2);
      temp [40] = '\0';
   	mvwprintw(jt ,  3, 3, temp);
   }

   ch_num[0] = ch_unica[0] = '\0';

   a_sep_int = sep_int;
   a_sep_dec = sep_dec;

le :

   sep_int = '.';
   sep_dec = ',';

   ret = ed_read(jt, &nr[0], 1, LIMPA_LE, LF13E);

   sep_int = a_sep_int;
   sep_dec = a_sep_dec;


	if ( ret < 0 || ret == K_ESC ) {  /* erro ou ESC */
		nreg = (ret == K_ESC) ? -2 : -1;
      goto fim;
   }

   if ( param == E_VALOR && atol(ch_unica) == 0L)
      ch_unica[0] = '\0';

   if ( ch_unica[0] != '\0' )
      nreg = psq_unica(sele, pc , ch_unica );
   else
      nreg = (LONG)str_to_db(ch_num, ',');

   if ( nreg <= 0 ) {
      mens_erro(H_REGINE, E_REGINE);
      goto le;
   }

fim :

   fwkill(jt);

sai :

   return(nreg);
}


F__GLB   COUNT exec_iae( sele, operacao, nreg, win_edit)
COUNT sele, operacao;
LONG nreg;
WIN *win_edit;
{  COUNT ret;
   WIN *ji, *jm, *jf;


   ji = abre_insercol(str_nada);
   jm = abre_mensagem(str_nada);
   jf = abre_funcoes(str_nada);

   ap_vista.vis_it_ativo = -1;

   switch( operacao ) {

      case INCLUSAO  :  insercol(ji, IC_INCLUI);
                        mens_1(jm, R_INCLUI);

                        if ( new_record(sele, 'S') < 0 ) {
                           mens_erro(H_ED001, E_ED_001);
                           ret = -2;
                           goto fim;
                        }

                        ret = edit_record(sele, c_bases[sele]->cb_numrec,
                                          win_edit, -1);
                        break;

      case ALTERACAO :  insercol(ji, IC_ALTERA);
                        mens_1(jm, R_ALTERA);

                        ret = edit_record(sele, nreg, win_edit, -1);
                        if ( ret == -99 ) {
                           mens_erro(H_ED002, E_ED_002, nreg);
                           ret = -1;
                        }

                        break;

      case EXCLUSAO  :  insercol(ji,  IC_EXCLUI);
                        mens_1(jm, R_EXCLUI);

                        em_del_glb = 1;  /* para colocar F2-Exclui */
                        ret = reg_display(sele, nreg, win_edit, NULL,
                                          NULL, 0, NULL, jm, jf, -1);
                        em_del_glb = 0;

                        if ( ret == K_F2 )
                           ret = del_record(sele, nreg);
                        else
                           ret = 0;

                        break;
   }

fim :

   free_record(sele);

   fecha_funcoes(jf);
   fecha_mensagem(jm);
   fecha_insercol(ji);

   return(ret);
}




F__LOC   LONG psq_unica(sele, pc, chave )
COUNT sele;
Campo *pc;
TEXT *chave;
{  LONG reg;

   sele = sele;

   reg = 0L;

   touch_unica(pc, &reg, chave, '<');

   return(reg);
}


