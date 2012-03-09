#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

#include  "lb2bases.h"
#include  "lb2prot1.h"
#include  "lb2vista.h"
#include  "lb2locs.h"

#include  "lb2entd.h"

#ifndef RUNTIME
#  define   C_PSQ_NRD  "13690 E"
#  define   C_PSQ_TRD  "135690 E"
#else
#  define   C_PSQ_NRD  "1390 E"
#  define   C_PSQ_TRD  "13590 E"
#endif

extern COUNT  hot_key;
extern TEXT   hot_palavra[ ];
#define   MAX_HOT           70 /* se mudar mude lb2pesq.h */


#ifdef MSDOS
   COUNT et_brilha( ARG * );
   COUNT pega_hot(WIN *);
   COUNT pede_hot(COUNT);
   VOID  ct_enfat( COUNT, WIN *, TTela *, SC **, SS );
#else
   COUNT et_brilha( );
   COUNT pega_hot( );
   COUNT pede_hot( );
   VOID  ct_enfat( );
#endif


extern TTela *pe_global;
static TEXT **pt_br;



F__GLB   COUNT reg_display( sele, nreg, win_disp, aux , pl_brilhar, ok_num, numero, jm, jf, it_ativo)
COUNT sele;
LONG nreg;
WIN *win_disp;
TEXT *aux;
TEXT **pl_brilhar;
COUNT ok_num;        /* indica se deve retornar numero de registro */
LONG *numero;       /* numero do registro a ser retornado */
WIN *jm, *jf;
COUNT it_ativo;
{  COUNT ret, flag, salva, atual, lst_funcao, e_pesquisa;
   COUNT tela, tela_ant, c, last_campo;
   TTela *pe;
   Campo *pc, **vetcmp;
   Vista *vista;
   static UTEXT car_escapes[] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      K_F2, K_ALT_F2, K_F4, K_ALT_F4, K_F6, K_ALT_F6, K_F7, K_F8, K_F9, K_F0,
      K_ALT_F7, K_ALT_F8,
      K_ESC, K_ENTER, K_TAB, K_BTAB, '\0'
   };
   static UTEXT car_e_del[] = {
      K_F2, K_F9, K_ESC, K_ENTER,
      K_ALT_F7, K_ALT_F8,
      K_TAB, K_BTAB, '\0'
   };


   pt_br = pl_brilhar; /* seta global para funcao et_brilha */

   pl_sele = sele;
   pl_win  = win_disp;
   pl_atrib= win_disp->w_defatr;

   if ( pl_brilhar != NULL ) e_pesquisa = 1;
   else                      e_pesquisa = 0;

   vista  = &ap_vista;

   if ( read_record(sele, nreg, 0) != 0 ) {
      ret = -99;
      goto fim;
   };

   ret      = 0;
   tela_ant = -1;
   flag     = FICA;
   salva    = 0;
   lst_funcao = '-';

   if ( it_ativo != -1 )
      ap_vista.vis_it_ativo  = it_ativo;   /* Mudanca de 16/11/93 p/ EMPAX */

   vetcmp = pos_vetcmp(sele);

   if ( aux == NULL ) aux = str_nada;

   while ( ! salva ) {

      pe = anda(sele, flag, DISPLAY, win_disp, aux, &last_campo);

      if ( pe == NULL ) {
         ret = K_ESC;
         break;
      }

      pc = vetcmp[pe->te_ivetcp];

      tela = pe->te_tela;

      if ( tela != tela_ant ) {
         pl_tela = tela;
         tela_ant = tela;

         if ( e_pesquisa )
            brilha_pal(sele, tela, win_disp, pl_brilhar, pl_atrib);
      }


      /*** Tratamento de Help de campo durante a pesquisa.  **/
      /*** Em 26/02/93 Base BAMA ***/

      if ( pc->cp_help[0] != '\0' ) {  /* monta help de campo */
         TEXT u_help[TD+1], path_help[PATH_MAX], *path;

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
      /**** Fim de tratamento a help de campo */


      if ( pc->cp_tipo == CP_TEXTO ) {
         if (lst_funcao != CP_TEXTO ) {
            if ( e_pesquisa ) funcoes(jf, funcao(C_PSQ_TRD));
            else {
               if ( em_del_glb )  funcoes(jf, funcao(C_DEL_TFUN));
               else               funcoes(jf, funcao("1 2 3 5 9 TBE"));
            }
            lst_funcao = CP_TEXTO;
         }

         if ( e_pesquisa ) {
            pc->cp_adoc->escape = (TEXT *)car_escapes;
            pc->cp_adoc->funcao = et_brilha;
         }
         else {
            pc->cp_adoc->escape = (TEXT *)car_e_del;
         }

         pc->cp_adoc->def_mens = M_PSQ_NAV;

         c = edit_buffer(pc->cp_adoc, ET_NAVEGA, pe->te_win, jm, 0 );

#ifdef PESQUISA_RECURSIVA
         /* para permitir que F2 execute a exclusao e nao vire um TAB */
         if ( em_del_glb == 0 &&
              c == K_F2 || c == K_F4 || c == K_ALT_F2 || c == K_ALT_F4 ) {
            if ( c == K_F2 || c == K_F4 ) {
               if ( pega_hot(pe->te_win) == 0 ) /* hot_palavra */
                  c = K_TAB;
            }
            else {
               if ( pede_hot( c ) == 0 ) /* hot_palavra */
                  c = K_TAB;
            }
            if ( c == K_ALT_F4 ) c = K_F4;
            if ( c == K_ALT_F2 ) c = K_F2;
         }
#endif
         mens_1(jm, M_PSQ_NAV);
         pc->cp_adoc->funcao = NULL;

         if ( c >= '0' && c <= '9' ) {
            w_ungc(c);
            c = so_navega(win_disp, ok_num, numero, jm, NULL);
         }
      }
      else {
         if (lst_funcao != 'N' ) {
            if ( e_pesquisa ) funcoes(jf, funcao(C_PSQ_NRD));
            else {
               if ( em_del_glb )  funcoes(jf, funcao(C_DEL_NFUN));
               else               funcoes(jf, funcao("1 2 3 9 TBE"));
            }
            lst_funcao = 'N';
         }
         fwat((COUNT)pe->te_coluna, (COUNT)pe->te_linha, win_disp);
         c = so_navega(win_disp, ok_num, numero, jm, NULL);
#ifdef PESQUISA_RECURSIVA
         /* para permitir que F2 execute a exclusao e nao vire um TAB */
         if ( em_del_glb == 0 &&
              c == K_F2 || c == K_F4 || c == K_ALT_F2 || c == K_ALT_F4 ) {
            if ( c == K_F2 || c == K_F4 ) {
               if ( pega_hot(win_disp) == 0 ) /* hot_palavra */
                  c = K_TAB;
            }
            else {
               if ( pede_hot( c ) == 0 ) /* hot_palavra */
                  c = K_TAB;
            }
            if ( c == K_ALT_F4 ) c = K_F4;
            if ( c == K_ALT_F2 ) c = K_F2;
         }
#endif

      }

      /* Recomposicao do tratamento de help de campo */
      user_help = NULL;


      /*
      ** so navega so retorna K_F2 ou K_F7 ou K_F8 ou K_F9
      ** K_PGDN ou K_PGUP ou K_ESC      ou K_ENTER (@ RICARDO)
      */

		switch ( c ) {

         case 0      :  ret = 0;
                        flag = FICA;
                        goto fim;

         case K_ESC  :  ret = c;
                        salva = -1;
                        break;

         case K_ENTER:
         case K_F0   :
         case K_F7   :
         case K_F8   :  if ( e_pesquisa ) {
                           ret = c;
                           salva = -1;
                        }
                        else flag = FICA;

                        break;

			case K_F2   :
			case K_F4   :  if ( em_del_glb == 0 && e_pesquisa ) {
#ifdef PESQUISA_RECURSIVA
                           hot_key = ret = c;
                           /*WWW ......... hot_palavra..... */
                           goto fim;
#endif
                        }
                        else {
                           ret = c;
                           salva = -1;
                        }
                        break;


         case K_F6   :  if ( e_pesquisa ) {
                           COUNT r=1;
#ifndef RUNTIME
                           r = varre_vista(sele, 1);
#endif
                           if ( r != 1 ) { /* teste se tem campo entrada */
                              mens_erro(H_NOVIS, E_NOVIS);
                              flag = FICA;
                           }
                           else {
                              ret = K_F6;
                              goto fim;
                           }
                        }
                        else
                           flag = FICA;

                        break;

         case K_ALT_F6: if ( e_pesquisa && esp_base.vista_altf6 != -1 ) {
                           /* o Usuario pode definir uma vista padrao para alt_f6 */
                           /* neste caso tambem se aplica uma limpeza de alguns campos */


                           atual = vista->vis_ordem;

                           if ( muda_vista(atual, esp_base.vista_altf6) < 0 ) {
                              ret = -2;
                              goto fim;
                           }

                           ap_limpacampos = 1;

                           tela   = 0;
                           tela_ant = -1;
                           flag   = PRIMEIRO;

                           ret = K_F6;
                           goto fim;
                        }
                        else
                           flag = FICA;

                        break;

         case K_F9   :  atual = vista->vis_ordem;

                        if ( muda_vista(atual, -1) < 0 ) {
                           ret = -2;
                           goto fim;
                        }

                        tela   = 0;
                        tela_ant = -1;
                        flag   = PRIMEIRO;

                        break;

         case K_BTAB  :
         case K_UP    :  flag = LBSOBE;       break;

         case K_TAB   :
         case K_DN    :  flag = LBDESCE;      break;

         case K_ALT_F7:
         case K_PGUP  :  flag = TELA_ANT;     break;

         case K_ALT_F8:
         case K_PGDN  :  flag = TELA_POS;     break;

         default      :  break;
      }
   };


fim :

   pe_global = NULL;

   return(ret);
}

/* -------------------------------brilhar palavra na navegacao do editor */
F__GLB   COUNT et_brilha( args )
ARG *args;
{
#ifdef   NAO_BRILHA

   return 0;

#else

/*   brilha_pal(pl_sele, pl_tela, args->je, pt_br, pl_atrib); */
   ct_enfat( BASE_EDIT, args->je, NULL, pt_br, pl_atrib);
   fwsetcursor( args->je);

   return( 0);
#endif
}





F__LOC COUNT pega_hot(j)
WIN *j;
{  US *P;
   UC C;
   SC tmp_linha[LB_KEYMAX+2];
   SS I;

   P = j->w_ary + (j->w_cy * j->w_len) + j->w_cx;

   for ( I = 0;  I < j->w_len ;  I++, P++ )
   {  C = (UC) *P;
      if ( C != ' ' && C != UG_SP && C != UG_ETPARA && I < LB_KEYMAX) {
         tmp_linha[ I ] = C;
         if ( strchr((SC*)tudo_delimita, C) != NULL )
            break;
      }
      else
         break;
   }

   tmp_linha[I] = '\0';
   strcpy(hot_palavra, tmp_linha);


   return(I);
}

F__LOC COUNT pede_hot( tecla )
COUNT tecla;
{  COUNT ret;
   TEXT *mens, *tit, *help, *masc;
   SC palavra[MAX_HOT + 1];
   WIN *jt;
   DTela nr[2];
   nr[0].destino = NULL;
   nr[1].destino = NULL;

#define   T_PEDE1HOT  "Novo arqumento de pesquisa"
#define   T_PEDE2HOT  "Refinamento de pesquisa"

#define   M_PEDE1HOT  "Informe o novo argumento de pesquisa"
#define   M_PEDE2HOT  "Informe o argumento a ser acrescentado a pesquisa"

#define   H_PEDE1HOT  ""
#define   H_PEDE2HOT  ""

   if ( tecla == K_F2 || tecla == K_ALT_F2 ) {
      tit = T_PEDE1HOT;
      mens= M_PEDE1HOT;
      help= H_PEDE1HOT;
   }
   else {
      tit = T_PEDE2HOT;
      mens= M_PEDE2HOT;
      help= H_PEDE2HOT;
   }

   masc = &str_x80[80 - MAX_HOT];

   seta_dtela(&nr[0], palavra, masc, mens, 1 , 4, 0, E_ALFA, help, NULL, NULL, NULL);

   if ( (jt = abre_janela(J_REFINA)) == NULL ) {
      ret = -1;
      goto sai;
   }

   fw_title(jt, tit, jt->w_atrib);
	fwbox(jt);

   palavra[0] = '\0';

   ret = ed_read(jt, &nr[0], 1, LIMPA_LE, LF13E);

	if ( ret < 0 || ret == K_ESC ) {  /* erro ou ESC */
      ret = 0;
      goto fim;
   }
   else {
   /* WWW para testar sentenca
      if ( (p = strchr(palavra, ' ')) != NULL )
         *p = '\0';
   WWW */
      palavra[MAX_HOT] = '\0';
      strcpy(hot_palavra, palavra);
      ret = 1;
   }

fim :

   fwkill(jt);

sai :

   return(ret);
}

