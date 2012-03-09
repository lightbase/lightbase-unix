#ifdef CP_FLAG

#include  "lb2all.h"

#include  <string.h>
#include  <time.h>
#ifdef MSDOS
#  include  HD_IO
#endif
#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2locs.h"
#include  "lb2prot1.h"

#include  "lb2vista.h"
#include  "lb2entd.h"

#define   GRP_PESQ          26
#define   MN_CN_T01  loc_str(GRP_PESQ, 89 )
#define   MN_CN_T04  loc_str(GRP_PESQ, 92 )

#define     LF1235E       "1 2Pesquisa 3 5 E"

#ifdef MSDOS
   VOID marca_campos( VOID );
   VOID clear_campos( COUNT );
   COUNT rd_psqcampo(TTela *, Campo *, WIN *, WIN *);
   COUNT le_pesquisa(COUNT, TEXT *, WIN *);
   VOID monta_expr(COUNT, Campo **, TEXT *, COUNT);
   VOID trata_seta(COUNT);
   COUNT edita_expr(TEXT *);
#else
   VOID marca_campos(  );
   VOID clear_campos(  );
   COUNT rd_psqcampo( );
   COUNT le_pesquisa( );
   VOID  monta_expr( );
   VOID trata_seta( );
#endif

/*----------------------------------------------------------------------*/
/* le_pesquisa                                                          */
/*----------------------------------------------------------------------*/
COUNT tl_captura;
F__GLB   COUNT le_pesquisa(sele, expr, win)
COUNT sele;
TEXT *expr;
WIN *win;
{  COUNT ret, flag, salva, last_campo, alt_f2;
   static TEXT aux[30];
   WIN *jm = NULL, *jf = NULL, *ji = NULL;
   TTela *pe;
   Campo *pc;

   tl_captura = 1;
   expr[0] = '\0';


   if ( varre_vista(sele, 1) != 1 ) { /* teste se tem campo entrada */
      mens_erro(H_NOVIS, E_NOVIS);
      ret = -1;
      goto fim;
   }


   jm = abre_mensagem(str_nada);
   ji = abre_insercol("Captura");
   jf = abre_funcoes(str_nada);

volta :

   aux[0] = '\0';

   flag   = FICA;
   salva  = 0;
   alt_f2 = 0;
   ap_vista.vis_it_ativo = -1;

ret_critica : /* retorno para critica.  Dn->cb_cpativo setado p/ invalido */

   while ( salva == 0 ) {

      pe = anda(sele, flag, 'E', win, aux, &last_campo);

      if ( pe == NULL ) {
         ret = -1;
         break;
      }

      pc = pos_vetcmp(pe->te_icb)[pe->te_ivetcp];

      if ( pc->cp_tipo == 'T' ) {
         debug("Nao previsto campo texto");
         ret = -1;
         goto fim;
      }
      else {
         funcoes(jf, funcao("1 2Pesquisa 3 E"));
         ret = rd_psqcampo(pe, pc, win, jm);
      }

		switch ( ret ) {
         case CTRLC   :
            
                        clear_campos(sele);
                        ap_vista.vis_it_ativo = -1;
                        flag = FICA;
                        break;

         case K_UP   :
         case K_DN   :
         case K_LFT  :
         case K_RT   :
                        trata_seta(ret);
                        flag = FICA;
                        break;


         case K_ESC  :  salva = -1;
                        ret = -2;
                        goto fim;

         case K_ALT_F2: alt_f2 = 1;
                        salva = 1;
                        break;

         case K_F2   :  salva = 1;
                        alt_f2 = 0;
                        break;

/*			case K_UP   :  */
         case K_BTAB :  flag = LBSOBE;
                        break;

         case K_PGUP :  flag = TELA_ANT;
                        break;

         case K_PGDN :  flag = TELA_POS;
                        break;

         default     :
         case K_TAB  :
/*         case K_DN   : */
         case K_ENTER:
                        flag = LBDESCE;
                        break;
      }
   };

   if ( salva == 1 ) {
      monta_expr(sele, pos_vetcmp(sele), expr, alt_f2);
      ret = 0;
   }

   if ( alt_f2 ) {
      if ( edita_expr(expr)< 0 ){
         marca_campos();
         goto volta;
      }
   }

fim :


   fecha_funcoes(jf);
   fecha_insercol(ji);
   fecha_mensagem(jm);

sai:

   tl_captura = 0;
   return(ret);
}




F__GLB   COUNT rd_psqcampo(pe, pc, win, jm)
TTela *pe;
Campo *pc;
WIN *win, *jm;
{  COUNT ret, param;
   TEXT *ajuda_ant = win_ajuda, *mascara;
   UTEXT escapes[10], *p;
	d_entry work;
   static COUNT last_key;


   win_ajuda = "CMPUSU";


/*
   if ( pe->te_tipo != CAMPO_E ) {
      ret = so_navega(win, 0, NULL, jm, NULL);
      goto fim;
   }

   if ( pc->cp_tipo == 'N' ) param = E_VALOR;
   else                      param = E_ALFA;
*/
   param = E_ALFA;

   if (pe->te_vhor < strlen(pc->cp_mascara) ) param |= E_CONFIRMA;
/*
   if ( pc->cp_rodape[0] == '@' )
      param |= E_MOSTRA;
*/
   mascara = pc->cp_mascara;

   if ( pc->cp_tipo == 'N' ) {
      mascara = (str_x80 + 80) -  strlen(pc->cp_mascara);
   }

	d_entrada( &work, win, pc->cp_newdata, pe->te_vhor, mascara,
              pc->cp_rodape, pe->te_coluna, pe->te_linha, param);

   pr_escape("2 9 0", escapes); /* prepara string de escapes para entrada */
   
   p = (UTEXT *)strchr((TEXT *)escapes, '\0');
   if ( pc->cp_mhor == 1) {
      *p++ = K_RT;
      *p++ = K_LFT;
   }

   *p++ = K_DN;
   *p++ = K_UP;
   *p++ = CTRLC;
   *p++ = K_ALT_F2;

   work.escape  = escapes;
   work.men     = jm;

   if ( (param & E_MOSTRA) == 0 ) {
      if ( pc->cp_help[0] != '\0' ) {  /* monta help de campo */
         TEXT u_help[TB+1], path_help[PATH_MAX], *path;

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
   }

	ret = entrada( &work, 0);

   user_help = NULL;

#ifndef CP_FLAG
   if ( pc->cp_rodape[0] == '@' ) {
      work.mudou = 1;
      ret = last_key;
      /* estava entrando em loop quando primeiro campo da vista */
      /* resolvi tratar como excessao o F2 e o F9 e o ESC.  Em 26/02/93 base BAMA */

      if ( ret == K_F2 || ret == K_F9 || ret == K_ESC)
         ret = K_ENTER;
   }
   else
      last_key = ret;
#endif

   if ( work.mudou || work.tecla == K_F2 ) {

      pc->cp_mod_flag = 1;
      pc->cp_crt_flag = 1;

      pc->cp_newtam = strlen(work.str);
   }


fim :

   win_ajuda = ajuda_ant;

   return(ret);
}


F__LOC   VOID clear_campos(sele)
COUNT sele;
{  COUNT i, num_cps;
   Campo *pc;

   memset(c_bases[sele]->cb_wkarea, '\0', c_bases[sele]->cb_tamrec);

   num_cps = qtd_vetcmp(sele);

   for ( i = 0 ; i < num_cps; i++ ) {
      pc = pos_vetcmp(sele)[i];
      pc->cp_newtam  = 0;
      pc->cp_olddata = NULL;
      pc->cp_oldtam  = 0;
   }

   memset(c_bases[sele]->cb_ioarea, '\0', c_bases[sele]->cb_tamrec);
}


F__LOC   VOID marca_campos()
{  COUNT i, num_cps, tam;
   Campo *pc;
   COUNT sele = BASE_EDIT;

   num_cps = qtd_vetcmp(sele);

   for ( i = 0 ; i < num_cps; i++ ) {
      pc = pos_vetcmp(sele)[i];
      tam = strlen(pc->cp_newdata);
      pc->cp_newtam  = tam;
      pc->cp_mod_flag = 1;
   }
}

F__LOC   VOID monta_expr(sele, vet_cmp, expr, alt_f2)
COUNT sele;
Campo **vet_cmp;
TEXT *expr;
COUNT alt_f2;
{  COUNT i, num_cps, tamanho, argumentos;
   TEXT aux[100], operador, *dado;
   TEXT nao[10], e[10];
   Campo *pc;
   TEXT *p;

   expr[0] = '\0';
   strcpy(e, MN_CN_T01);
   prep_palavra(e);
   trim(e);

   strcpy(nao, MN_CN_T04);
   prep_palavra(nao);
   trim(nao);

   num_cps  = qtd_vetcmp(sele);
   tamanho = 0;

   /* conta numero de argumentos informados */
   for ( i = 0, argumentos = 0; i < num_cps; i++ ) {
      pc = vet_cmp[i];
      if ( pc->cp_mod_flag == 0 ) continue;
      if ( pc->cp_newtam == 0 )   continue;
      argumentos++;

      /* procura colocar um argumento 'S' para nao usar "**" */
      if ( expr[0] == '\0' /* Jairo && pc->cp_1futuro == 'F' */ ) {
         if ( pc->cp_newdata[0] != OP_NAO[0] ) {
            sprintf(expr, "%s[%s] ", pc->cp_newdata, pc->cp_nome);
            pc->cp_newtam = 0;
         }
      }
   }

   if ( expr[0] == '\0' ) {
      strcpy(expr, "** ");
   }

   for ( i = 0; i < num_cps; i++ ) {
      pc = vet_cmp[i];
      if ( pc->cp_mod_flag == 0 ) continue;
      if ( pc->cp_newtam == 0 )   continue;
      if ( pc->cp_tipo == 'N' || pc->cp_tipo == 'F' ) {
         p = pc->cp_newdata;
         while ( *p == '>' || *p == '<' || *p == '=')
            p++;

         if ( atol(p) == 0 ) continue;
      }

      operador = 'E';
      dado = pc->cp_newdata;

      if ( pc->cp_1futuro == 'F' ) {
         if ( pc->cp_newdata[0] == OP_NAO[0] ) {
            dado = "S";
            operador = 'N';
         }
      }

      if ( operador == 'E' )
         p = e;
      else
         p = nao;

      if ( alt_f2 ) 
         sprintf(aux, "%s %s[%s] \n", p, dado, pc->cp_nome);
      else
         sprintf(aux, "%s %s[%s] ", p, dado, pc->cp_nome);

      tamanho += strlen(aux);

      if ( tamanho > 200 ) {
         strcat(expr, " ...");
         break;
      }

      strcat(expr, aux);
   }
}

F__LOC   VOID trata_seta(seta)
COUNT seta;
{  COUNT maximo, i, ind;
   COUNT m0, m9, t_atu, c_atu, l_atu;
   COUNT qual, dif, dif_min;
   TTela **vtevis, *pe;

   qual = ind    = ap_vista.vis_it_ativo;
   vtevis = (TTela **)ap_vista.vis_vet->vi_vtit;
   maximo = qtd_itvis();


   l_atu = vtevis[ind]->te_linha;
   c_atu = vtevis[ind]->te_coluna;
   t_atu = vtevis[ind]->te_tela;

   /* descobre primeira e ultima linha da tela */
   m0 = 99;
   m9 = -1;

   for ( i = 0 ; i < maximo; i++ ) {
      pe = vtevis[i];
      if ( pe->te_tela != t_atu ) continue;
      if ( pe->te_tipo != CAMPO_E && pe->te_tipo != CAMPO_S ) continue;

      if ( pe->te_linha < m0 )
         m0 = pe->te_linha;

      if ( pe->te_linha > m9 )
         m9 = pe->te_linha;
   }



   if ( seta == K_LFT )
      c_atu--;

   if ( seta == K_RT )
      c_atu++;

volta:

   if ( seta == K_UP ) {
      l_atu--;
      if ( l_atu < m0 )
         l_atu = m9;
   }

   if ( seta == K_DN ) {
      l_atu++;
      if ( l_atu > m9 )
         l_atu = m0;
   }

volta_c:

   dif_min = 99;


   for ( i = 0 ; i < maximo; i++ ) {
      pe = vtevis[i];
      if ( pe->te_tela  != t_atu ) continue;
      if ( pe->te_tipo  != CAMPO_E && pe->te_tipo != CAMPO_S ) continue;
      if ( pe->te_linha != l_atu   ) continue;

      if ( seta == K_UP || seta == K_DN ) {

         dif = abs(pe->te_coluna - c_atu);

         if ( dif < dif_min) {
            qual = i;
            dif_min = dif;
         }
      }
      else {
         if ( seta == K_RT ) {
            qual = i;
            if ( pe->te_coluna >= c_atu ) 
               break;
         }
         else if ( seta == K_LFT ) {
            if ( c_atu < pe->te_coluna ) 
               break;
            qual = i;
         }
      }
   }

   if ( ind == qual ) {
      if ( seta == K_LFT || seta == K_RT ) {
         if ( seta == K_LFT )
            c_atu = 0;
         else 
            c_atu = 99;
         ind = -1;
         goto volta_c;
      }

      else
         goto volta;
   }

   ap_vista.vis_it_ativo = qual;

}


F__LOC COUNT edita_expr(expr)
TEXT *expr;
{  COUNT ret, largura;
   WIN *je, *jf, *ji;
   TEXT *ajuda_ant = win_ajuda;
   UTEXT *p;
   UTEXT lin_escape[20];
   ARG *adoc;
   LONG tam_texto;

   je = jf = ji = NULL;

   win_ajuda    = "EDTEXPR";

   adoc = NULL;
   ret = 0;


	je = abre_janela(J_STOPWORD);
   fw_title(je, "Edicao de sentenca de pesquisa", je->t_atr);
   fwbot(je);

   largura = 0;


   adoc = load_buffer(largura, ET_MEM, expr, strlen(expr));

   if ( adoc == NULL ) {
      mens_erro("H_LDEXPR", "Erro na edicao de expr");
      ret = -1;
      goto fim;
   }


      pr_escape("2", lin_escape);
      adoc->escape = (TEXT *)lin_escape;
      adoc->def_mens = "Monte expressao a ser avaliada";

	   jf = abre_funcoes(funcao(LF1235E));
      ji = abre_insercol("Editando");

      fwtop(je);

      ret = edit_buffer(adoc, ET_EDITA , je, NULL, 0);

      fecha_insercol(ji);
      fecha_funcoes(jf);

      if ( ret == K_ESC ) {
         ret = -1;
      }
      else {

         tam_texto = tam_buffer(adoc, ET_HANDLE);

         adoc->tipo    = ET_MEM;
         adoc->buffer  = expr;
         adoc->buf_len = tam_texto;

         /* salva em memoria */
         save_file(adoc);
         expr[tam_texto] = '\0';

         for ( p = expr; *p != '\0' && tam_texto > 0 ; p++, tam_texto--) {
            if ( *p == '\n') *p = ' ';
            if ( *p == '\r') *p = ' ';
            if ( *p == UG_ETPARA) *p = ' ';
         }

         ret = 0;
      }

fim :

   if ( adoc != NULL ) {
      free_buffer( adoc );
      adoc = NULL;
   }

	if ( je ) fwkill(je);
   win_ajuda    = ajuda_ant;
   return(ret);
}
#endif
