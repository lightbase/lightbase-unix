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


#include  "lb2vista.h"

#include  "lb2entd.h"

TTela *pe_global;

#ifdef MSDOS
   VOID  xuxa_campo( Campo *, TTela *, WIN *);
   COUNT disp_campos(COUNT, WIN *);
#else
   VOID  xuxa_campo( );
   COUNT disp_campos( );
#endif

COUNT last_rdcampo; /* ultimo "ind" efetivamente "LIDO" por rd_campo */



F__GLB   TTela *anda(sele, flag, operacao, win, rodape, last_campo)
COUNT sele, flag, operacao;
WIN *win;
TEXT *rodape;
COUNT *last_campo;
{  COUNT maximo, tela_atu, i, tela, inc, inicia_em, retenta, ind;
   COUNT tela_ant;
   TTela **vtevis, *pe;

   ind    = ap_vista.vis_it_ativo;
   vtevis = (TTela **)ap_vista.vis_vet->vi_vtit;
   maximo = qtd_itvis();


   if ( ind < 0 ) {
      flag = PRIMEIRO;
      tela_ant = -1;
      ind = 0;
   }
   else {
      if ( flag == PRIMEIRO ||  ap_vista.vis_old_ativo < 0 )
         tela_ant = -1;
      else
         tela_ant = vtevis[ind]->te_tela;
   }


   tela_atu = vtevis[ind]->te_tela;

   retenta  = 0;

inicio :


#ifdef CP_FLAG
{  extern COUNT tl_captura;
   if ( tl_captura == 1 && vtevis[0]->te_tipo == CAMPO_S )
      vtevis[0]->te_tipo = CAMPO_E;
}
#endif

   switch ( flag ) {

      case PRIMEIRO :   if ( vtevis[0]->te_tipo == CAMPO_E ) {
                           ind = 0;
                           goto fim;
                        };
                        tela      = 0;
                        inicia_em = 0;
                        inc       = 1;
                        if (retenta == 0 ) retenta = 0;
                        break;

      case ULTIMO   :
                        tela      = 0;
                        inicia_em = maximo - 1;
                        inc       = -1;
                        if (retenta == 0 ) retenta = 0;
                        break;

      case LBSOBE   :   tela      = 0;
                       inicia_em = ind - 1;
                        inc       = -1;
                        if (retenta == 0 ) retenta = ULTIMO;
                        break;

      case LBDESCE  :   tela      = 0;
                        inicia_em = ind + 1;
                        inc       = 1;
                        if (retenta == 0 ) retenta = PRIMEIRO;
                        break;

      case TELA_ANT :   tela      = tela_atu - 1;
                        if ( tela == 0 ) {
                           inicia_em = 0;
                           inc = 1;
                        }
                        else {
                           inicia_em = ind - 1;
                           inc = -1;
                        }
                        if (retenta == 0 ) retenta = PRIMEIRO;
                        break;

      case TELA_POS :   tela      = tela_atu + 1;
                        inicia_em = ind + 1;
                        inc = 1;
                        if (retenta == 0 ) retenta = ULTIMO;
                        break;

      case FICA     :   tela      = 0;
                        inicia_em = ind;
                        if (retenta == 0 ) retenta = 0;
                        inc = 1;
                        break;

      case RE_LER   :   tela      = 0;
                        inicia_em = last_rdcampo;
                        if (retenta == 0 ) retenta = 0;
                        inc = 1;
                        break;

   };

   ind = -1;

   for (  i = inicia_em;     i >= 0 && i < maximo ;    i += inc ) {

      if ( inc > 0 && tela != 0 && vtevis[i]->te_tela < tela) continue;
      if ( inc < 0 && tela != 0 && vtevis[i]->te_tela > tela) continue;

#ifdef CP_FLAG
{  extern COUNT tl_captura;
      if ( tl_captura == 1 && vtevis[i]->te_tipo == CAMPO_S )
         vtevis[i]->te_tipo = CAMPO_E;
}
#endif
      if ( operacao == 'D') {
         if ( vtevis[i]->te_tipo == CAMPO_E || vtevis[i]->te_tipo == CAMPO_S ) {
            ind = i;
            break;
         }
      }
      else {
         if ( vtevis[i]->te_tipo == CAMPO_E ) {
            ind = i;
            break;
         };
      }
   };

   if ( ind < 0 && retenta != 0 ) {
      flag = retenta;
      goto inicio;
   }

fim :


   if (ind < 0 ) {
      pe = NULL;
      *last_campo = 0;
   }
   else {
      pe = vtevis[ind];
      ap_vista.vis_old_ativo = ap_vista.vis_it_ativo = ind;
      if ( pe->te_tela != tela_ant ) {
         lista_vista(win, sele, pe->te_tela, UTILIZACAO, rodape);
         if ( disp_campos(sele, win) < 0 ) {
            pe = NULL;
         }
      }

      for ( i = ind + 1; i < maximo ; i++ ) {
         if ( vtevis[i]->te_tipo == CAMPO_E )
            break;
      }

      *last_campo = (i < maximo) ? 0 : 1;
   }

   pe_global = pe;

   return(pe);
}



F__LOC VOID xuxa_campo(pc, pe, win)
Campo *pc;
TTela *pe;
WIN *win;
{  COUNT param;
	d_entry work;
   TEXT *masc;
#ifdef CP_FLAG
   TEXT *masc1="xxxxxxxxxxxxxxxxxxxxxxxx";
   COUNT i;
#endif

   switch (  pc->cp_tipo ) {
      case 'F' :  masc  = pc->cp_param.cp_msc_form;
                  param = E_VALOR;
                  break;

      case 'N' :  masc  = pc->cp_mascara;
                  param = E_VALOR;
                  break;

      default  :  masc = pc->cp_mascara;
                  param = E_ALFA;
                  break;
   }

#ifdef CP_FLAG
{
extern COUNT tl_captura;
if ( tl_captura == 1 && param == E_VALOR) {
   param = E_ALFA;
   for ( i = 0; masc[i] != '\0'; i++) {
      masc1[i] = 'x';
   }
   masc1[i] = '\0';
   masc=masc1;
}
}
#endif

	d_entrada(	&work, win, pc->cp_newdata , pe->te_vhor, masc, str_nada,
               pe->te_coluna, pe->te_linha, (param | E_MOSTRA));

   work.men     = NULL;

	entrada( &work, 0);
}



F__GLB COUNT disp_campos(sele, win)  /*----------------------------------*/
COUNT sele;
WIN *win;
{  COUNT ret, i, tela, ind, n_elem;
   TTela *pe;
   Campo *pc;
   POINTER pos;
   LONG tam;

   ind    = ap_vista.vis_it_ativo;
   ret    = 0;

   if ( ind < 0 ) { numdebug(400); goto fim; }

   tela    = pos_itvis(ind)->te_tela;
   n_elem  = qtd_itvis();

   dn_textos(sele, tela);

   for ( i = 0; i < n_elem; i++ ) {
      pe = pos_itvis(i);            if ( pe == NULL) fim_lb("401");

      if ( pe->te_tela != tela ) continue;
      if ( pe->te_tipo != CAMPO_E && pe->te_tipo != CAMPO_S) continue;

      if ( pe->te_ivetcp < 0 || pe->te_ivetcp > 127 ) {
         debug("ivetcp fora de limites");
         continue;
      }

      pc = pos_vetcmp(sele)[pe->te_ivetcp];

      if ( pc->cp_tipo == CP_TEXTO ) {

         if ( pc->cp_adoc == NULL ) {

            memcpy((TEXT *)&pos, (TEXT *)pc->cp_newdata , sizeof(pos));

            if ( pos != (POINTER) 0 )
               pc->cp_adoc = txt_load(AD_NUM(sele), pos, &tam, pc->cp_mhor, WCB->cb_numrec);
            else
               pc->cp_adoc = load_buffer( pc->cp_mhor, ET_MEM, str_vazia, 0L);

            if ( pc->cp_adoc == NULL ) {
#ifdef ASSIM_E_PIOR
               mens_erro(H_LDDOC, E_LDDOC);
               ret = -1;
#endif
               if ( debug(M_PRDTXT, c_bases[sele]->cb_numrec, pc->cp_nome) == 'M' ) {
                  pc->cp_adoc = NULL;
                  ret = 0;
                  goto escapa;
               }

               goto fim;
            }
         }

         if ( edit_buffer(pc->cp_adoc, ET_MOSTRA, pe->te_win, NULL, 0 ) < 0 ) {
            numdebug(430);
            ret = -1;
            goto fim;
         }
escapa : ;
         
      }
      else {
         xuxa_campo(pc, pe, win);
      }
   }

fim :

   return(ret);
}


F__GLB   VOID disp_mtela(sele, win)
COUNT sele;
WIN *win;
{  COUNT i, tela, numcampo, ind, n_elem;
   TTela *pe;
   Campo *pc;

   ind    = ap_vista.vis_it_ativo;

   if ( ind < 0 ) { numdebug(400); goto fim; }

   tela     = pos_itvis(ind)->te_tela;
   numcampo = pos_itvis(ind)->te_ivetcp;

   if ( numcampo < 0 || numcampo > 127 ) {
      debug("ivetcp fora de limites");
      goto fim;
   }

   n_elem  = qtd_itvis();

   for ( i = 0; i < n_elem; i++ ) {

      pe = pos_itvis(i);

      if ( pe->te_tela != tela ) continue;
      if ( pe->te_tipo != CAMPO_E && pe->te_tipo != CAMPO_S) continue;
      if ( pe->te_ivetcp != numcampo ) continue;
      if ( i == ind ) continue;

      pc = pos_vetcmp(sele)[pe->te_ivetcp];

      if ( pc->cp_tipo == CP_TEXTO ) {
         if ( edit_buffer(pc->cp_adoc, ET_MOSTRA, pe->te_win, NULL, 0 ) < 0 )
            numdebug(403);
      }
      else
         xuxa_campo(pc, pe, win);
   }

fim:

   ind = ind; /* ANANIAS @@@ tirar qdo tirar debug */
}




F__GLB   COUNT so_navega(janela, flag, numero, jm, mens)
WIN *janela;
COUNT flag;
LONG *numero;
WIN *jm;
TEXT *mens;
{  COUNT c;
   TEXT *ajuda_ant = win_ajuda;
   UTEXT *p;
   static UTEXT car_escapes[] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      K_F2, K_ALT_F2, K_F4, K_ALT_F4, K_F5, K_F6, K_ALT_F6, K_F7, K_F8, K_F9, K_F0,
      K_ALT_F7, K_ALT_F8,
      K_ESC, K_ENTER, K_TAB, K_BTAB,
      K_PGUP, K_PGDN, K_UP, K_DN, '\0'
   };

   win_ajuda = "NAVEGA";

   if ( mens == NULL )
      mens_1(jm, M_NAVEGA);
   else
      mens_1(jm, mens);

   do {

		c = fwqgc ( janela );

      p = car_escapes;

      if ( flag == 0 ) p += 10; /* nao aceita numeros como escapes */

      while( *p != '\0' && *p != c )
         p++;

   }  while ( *p == '\0');

   if ( *p >= '0' && *p <= '9' ) {

      w_ungc( c );

      *numero = pega_num();

      c = 0; /* retorna ZERO */
   }

   win_ajuda = ajuda_ant;

   return ( c );
}


F__GLB   VOID dn_textos(sele, tela)
COUNT sele, tela;
{  COUNT ind, n_elem;
   TTela *pe;

   sele = sele;

   n_elem  = qtd_itvis();

   for (ind = 0; ind < n_elem; ind++ ){
      pe = pos_itvis(ind);
      if ( pe->te_win == NULL ) continue;
      if ( pe->te_tela == tela) continue;

      fwkill(pe->te_win);
      pe->te_win = NULL;
   }
}



F__GLB   VOID trata_outs( sele, reprocessando )
COUNT sele, reprocessando;
{  COUNT i, num_cps;
   Campo **vet_cmp, *pc;


   num_cps = qtd_vetcmp(sele);
   vet_cmp = pos_vetcmp(sele);

   for ( i = 0 ; i < num_cps; i++ ) {

      pc = vet_cmp[i];

      if ( pc->cp_rodape[0] != '@' ) continue;

      trata_indireto(pc->cp_rodape, pc, reprocessando);
      pc->cp_newtam = strlen(pc->cp_newdata);
      pc->cp_mod_flag = 1;
      pc->cp_crt_flag = 1;
   }
}

