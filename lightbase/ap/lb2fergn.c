#include  "lb2all.h"

#include   <string.h>
#include  HD_IO

#ifdef UNIX
#ifndef CTSRVR
#include   <sys/types.h>
#include   <sys/stat.h>
#endif
#endif

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comuns a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#define MODO_DEF_FER  1

#include  "lb2fer.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#define   STR_ZERO      "0"
#define   STR_UM        "1"
#define   LF13E       "1 3 E"
#define   STR_DUAS    "%s\n%s"
#define   STR_LEFTN   "%-*s"
#define   STR_S       "%s"
#define   STR_3D      "%3d"
#define   STR_7D      "%7ld"
#define   STR_5P5D    "%5.5d"


#define   H_REBCLS   "E_REBCLS"   /* Erro no fechamento da base apos rebuid */
#define   H_RBAPG    "E_RBAPG"    /* Arquivo da base foi deletado durante reprocessamento */
#define   H_RBRXY    "E_RBRXY"    /* Erro ao renomear arquivo */
#define   H_OTIMIZA  "OP_OTIMI"   /* Opcoes de otimizacao de indexacao */


#ifdef MSDOS
   COUNT tools_exec(Selecao *);
   COUNT informacoes(COUNT, TEXT *, WIN *);
   COUNT abre_log(TEXT *);
   COUNT manut_reb(WIN *);
   COUNT repro_base(WIN *);
   COUNT ren_base(VOID);
   COUNT vl_log(d_entry *);
#else
   COUNT tools_exec( );
   COUNT informacoes( );
   COUNT abre_log( );
   COUNT manut_reb( );
   COUNT repro_base( );
   COUNT ren_base( );
   COUNT vl_log( );
#endif


/*************************** variaveis estaticas **********************/
static COUNT fechou_glb;
static TEXT  base_ant[TB+1];

extern SI    errno;
extern COUNT idx_otimiza;


F__GLB   COUNT tools_base()
{  Selecao m0;
   TEXT *ajuda_ant = win_ajuda;
	WIN *jf;
   TEXT *p;
   TEXT *m_tol[(5 * 3) + 1];

   up_str(GRP_FER);

   win_ajuda    = "MENUTOOL";
   base_ant[0] = '\0';
   fechou_glb = 0;

   p = ( ap_status != 'A' && ap_status != 'B' ) ? STR_ZERO : STR_UM;


   set_menu(m_tol, 0, MN_TL_T01,  STR_UM, MN_TL_R01);
   set_menu(m_tol, 1, MN_TL_T02,       p, MN_TL_R02);
   set_menu(m_tol, 2, MN_TL_T03,       p, MN_TL_R03);
   set_menu(m_tol, 3, MN_TL_T04,       p, MN_TL_R04);
   set_menu(m_tol, 4, NULL,     STR_ZERO, NULL);



   jf = abre_funcoes(funcao(LF13E));


	if (WCB != NULL && WCB->cb_fordem != 0 )
      strcpy(base_ant, WBS->bs_nome); /* para recuperar base ativa */


	def_menu( &m0, 60,  8, 30,  8,  1, MS_CAMPO, MS_BARRA, 1,
             MS_JANELA, MS_DESTAQUE,
				 &m_tol,  MN_TL_TIT, MN_TL_ROD);


   m0.executa = tools_exec;


   if ( (rec_base = (Base *) calloc(1, sizeof(Base))) != NULL ) {
   	menu( &m0 );
        free(rec_base);
   }
   else {
      mens_erro(H_NOMEMO, SEM_MEMO);
   }

/*
**  nao vou mais abrir para so abrir base em lb2sbase
**
**   if ( fechou_glb && base_ant[0] != '\0' ) {
**      if ( open_base(base_ant, UTILIZACAO) == 0 )
**         imp_base(base_ant);
**   }
*/

   fecha_funcoes(jf);

   win_ajuda = ajuda_ant;

   down_str(GRP_FER);

   return ( 0 );
}




F__LOC   COUNT tools_exec(s)
Selecao *s;
{  COUNT ret = 0, tipo_lck;
   COUNT lin;
   TEXT *base;
   WIN  *jm, *ji;
   WIN *j0;

   ji = jm = NULL;
   jl_glb = NULL;
   fpo_glb = NULL;


	ret = s->escolhido;

   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */
   /* menu principal */

   if ( s->tecla == K_ESC ) {
      ret = -1;
      goto fim;
   }


   ji = abre_insercol(str_nada);
   jm = abre_mensagem(M_AGUARDE);

   switch( s->escolhido ) {
      case 0 :   insercol(ji, C_MN_INFORM);  tipo_lck = 0;           break;
      case 1 :   insercol(ji, C_MN_REPROC);  tipo_lck = LCK_TRAVA;   break;
      case 2 :   insercol(ji, C_MN_ZAP);     tipo_lck = LCK_TRAVA;   break;
      case 3 :   insercol(ji, C_MN_EXCLUI);  tipo_lck = LCK_TRAVA;   break;
      default:   ret = -1;
                 goto fim;
   }


   lin=12;

   if ( (base = uma_base(lin, base_ant, RESTRITO, PODE_UMA, M_SELBASE)) == NULL )
      goto fim;

	if ( ! load_rec(ace_dnum, ace_inum, D0_ID, base, rec_base, sizeof(Base), '=') ) {
      mens_erro(H_BADAC, E_BADAC);
      ret = -1;
      goto fim;
   }

   if ( s->escolhido != 3 && rec_base->bs_finger != 0 ) { /* EXCLUIR base pode */
      if ( base_liberada(rec_base) != 'S' ) {
         ret = -1;
         goto fim;
      }
   }


   if (WCB != NULL && WCB->cb_fordem != 0 ) {
      lib_ativa(BASE_EDIT);
      fechou_glb = 1;
   }

	if ( s->escolhido == 1 ) { /* REBUILD necessita de log */
      if ( abre_log( base ) < 0 ) {
         ret = -1;
         goto fim;
      }
   }

   if ( tipo_lck != 0 ) {

      if ( LOCK_RECURSO(base, rec_base->bs_path, tipo_lck) != 0 ) {
         /* operacao negada */
         if ( tipo_lck == LCK_EXCLUSIVO || tipo_lck == LCK_TRAVA )
            mens_erro(H_NOLOCK, E_NOLOCK);
         else
            mens_erro(H_LOCKED, E_LOCKED);
         ret = -1;
		   goto fim;
      }
   }

   j0 = abre_janela(J_UTIL);      /* abre janela para salvar conteudo */

   if ( jl_glb != NULL )
      fwtop(jl_glb);

   switch ( s->escolhido ) {
      case 0 :   informacoes(BASE_EDIT, base, j0); break;
      case 1 :   manut_reb(j0);                   break;
      case 2 :   zap_base(rec_base, ZAP);          break;
      case 3 :   if ( del_base( j0 ) == 0 )
                     base_ant[0] = '\0';           break;
   }

   if ( tipo_lck != 0 ) {
      LOCK_RECURSO(base,  rec_base->bs_path,
                       (tipo_lck == LCK_TRAVA ) ? LCK_DESTRAVA : LCK_LIBERA);
   }

   fwkill( j0 );

fim :

	if ( jl_glb != NULL ) fwkill(jl_glb);
   if ( fpo_glb != NULL ) {
      fclose(fpo_glb);
      fpo_glb = NULL;
   }

   if ( ret == 0 ) ret = s->escolhido;

	fecha_mensagem(jm);
   fecha_insercol(ji);

   return(ret);
}





/*-----------------------------------------------------------------------*/
F__LOC   COUNT informacoes(sele, base, j0)
COUNT sele;
TEXT *base;
WIN *j0;
{  COUNT ret, salva_atr;
   LONG tot_regs, tot_dels, tot_index, tot_toind, tot_ativos;
   TEXT aux[100];
   TEXT *ajuda_ant = win_ajuda;

   win_ajuda = "INFOR_BA";

   tot_regs = tot_dels = tot_index = tot_toind = 0;

   if ( open_base(base, INFORMACAO) != 0 ) {
		ret = -1;
      goto fim;
   }

   fw_title(j0, C_IB_TIT, j0->w_atrib);
	fwbox(j0);


   mvwprintw(j0 ,  1, 6, C_IB_NOME);   mvwprintw(j0 ,  1, 46, C_IB_MAGI);
   mvwprintw(j0 ,  3, 6, C_IB_DESC);
   mvwprintw(j0 ,  5, 6, C_IB_PATH);

   mvwprintw(j0 ,  7,  6, C_IB_DONO);
   mvwprintw(j0 ,  7, 45, C_IB_GRUP);

   mvwprintw(j0 , 10, 6, C_IB_NCAM);
   mvwprintw(j0 , 12, 6, C_IB_NVIS);
   mvwprintw(j0 , 14, 6, C_IB_NREL);

   mvwprintw(j0 , 10, 37, C_IB_CRIA);
   mvwprintw(j0 , 12, 37, C_IB_ALTE);
   mvwprintw(j0 , 14, 37, C_IB_BACK);


   mvwprintw(j0 , 17, 6, C_IB_AREG);
   mvwprintw(j0 , 19, 6, C_IB_DREG);

   mvwprintw(j0 , 17, 37, C_IB_IREG);
   mvwprintw(j0 , 19, 37, C_IB_NREG);

   /* coloca atributo corrente como atributo de campo */
   salva_atr   = j0->w_atrib;
   j0->w_atrib = j0->campoatr;



   sprintf(aux, STR_LEFTN, TB, WBS->bs_nome);       mvwprintw(j0 ,  1, 18, aux);
   sprintf(aux, STR_5P5D ,     WBS->bs_idbase);     mvwprintw(j0 ,  1, 63, aux);
   sprintf(aux, STR_LEFTN, TD, WBS->bs_desc);       mvwprintw(j0 ,  3, 18, aux);
   sprintf(aux, STR_LEFTN, TD, WBS->bs_path);       mvwprintw(j0 ,  5, 18, aux);

   sprintf(aux, STR_LEFTN, TE, WBS->bs_dono);       mvwprintw(j0 ,  7, 18, aux);
   sprintf(aux, STR_LEFTN, TE, WBS->bs_grupo);      mvwprintw(j0 ,  7, 53, aux);


   sprintf(aux, STR_3D, qtd_vetcmp(sele));       mvwprintw(j0 , 10, 27, aux);
   sprintf(aux, STR_3D, WBS->bs_n_vis );         mvwprintw(j0 , 12, 27, aux);
   sprintf(aux, STR_3D, WBS->bs_n_rel );         mvwprintw(j0 , 14, 27, aux);

   sprintf(aux, STR_S, data_hora('D', WBS->bs_cr_data)); mvwprintw(j0 , 10, 60, aux);
   sprintf(aux, STR_S, data_hora('D', WBS->bs_al_data)); mvwprintw(j0 , 12, 60, aux);

   if ( WBS->bs_bk_data > 0 ) sprintf(aux, STR_S, data_hora('D', WBS->bs_bk_data));
   else                       strcpy(aux, C_IB_NOBACK);

   mvwprintw(j0 , 14, 60, aux);


   if ( look_regs(sele, &tot_regs, &tot_ativos, &tot_dels, &tot_toind) < 0 ) {
       ret = -1;
       goto fim;
   }

   tot_index = tot_ativos - tot_toind;


   sprintf(aux, STR_7D, tot_ativos );    mvwprintw(j0 , 17, 27, aux);
   sprintf(aux, STR_7D, tot_dels );      mvwprintw(j0 , 19, 27, aux);

   sprintf(aux, STR_7D, tot_index);    mvwprintw(j0 , 17, 60, aux);
   sprintf(aux, STR_7D, tot_toind);    mvwprintw(j0 , 19, 60, aux);


   j0->w_atrib = salva_atr;

   men_conf(C_IB_GO, "*", win_ajuda, NULL);


fim :

	if (WCB != NULL && WCB->cb_fordem != 0 )
	   lib_ativa(sele);

   win_ajuda = ajuda_ant;

   return(ret);
}




/*-----------------------------------------------------------------------*/
F__GLB   COUNT zap_base(pbase, operacao)
Base *pbase;
COUNT operacao;
{  COUNT ret, i, num, narq;
   UCOUNT id;
   LONG ns;
   TEXT *ajuda_ant = win_ajuda;
   TEXT nx[PATH_MAX], *pnx;
   COUNT fixo = PERMANENT | EXCLUSIVE | FIXED;
   COUNT varl = PERMANENT | EXCLUSIVE | VLENGTH;
#ifdef UNIX
#ifndef CTSRVR
   struct stat st;
   int uid, gid;
#else
/* WWW ERRO! Nao usar isso no modo SERVIDOR */
#endif
#endif

   win_ajuda = "ZAPBASE";
   ret = 0;

   /* pode ser chamada por reprocessa */
   if ( operacao == ZAP ) {
      TEXT aux[60];

      strcpy(aux, TW_LB_D02);
      trim(aux);
      strcat(aux, " ");
      strcat(aux, pbase->bs_nome);

      if ( decida( TW_LB_D00, TW_LB_D99, H_CFZAP, 2, 39, STR_DUAS,
                   TW_LB_D01, aux ) != 1 ) {
         ret = -1;
         goto fim;
      }
   }


   info_base(pbase->bs_nome, pbase->bs_path, LB_1_EXT, nx);

   /* faz pnx apontar para numero do arquivo LB? */
   pnx = strchr(nx, '\0');
   pnx--;

   narq = lb4_dnum;
   ns = ap_nserie;

   id = pbase->bs_idbase;


   for ( i = 1; i <= 3; i++ ) {
      *pnx = '0' + i;  /* Monta nome do arquivo */
#ifdef UNIX
#ifndef CTSRVR
      uid = gid = -1;
#endif
#endif

      if ( access(nx, 0) == 0 )  {
#ifdef UNIX
#ifndef CTSRVR
         if (stat( nx, &st ) != -1) {
            uid = st.st_uid;
            gid = st.st_gid;
         }
#endif
#endif

         DELFILE(nx);
      }

      ct_finger  = pbase->bs_finger;
      ct_release = pbase->bs_release;
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

#ifdef UNIX
#ifndef CTSRVR
      if (uid != -1)
         chown( nx, uid, gid );
#else
/* WWW ERRO! Implementar CHOWN e CHGRP no servidor Light. */
#endif
#endif

      if ( ret != 0 )
         goto fim;
   }

   ret = 0;

fim :

   win_ajuda = ajuda_ant;

   return(ret);
}

/*-----------------------------------------------------------------------*/
F__GLB   COUNT abre_log( nomebase )
TEXT *nomebase;
{  COUNT ret = 0;
   TEXT  log[PATH_MAX+1];
   DTela nr[2];
   WIN  *j1;

   seta_dtela(&nr[0], log, XPATH, C_RP_RLOG, 1, 5, 20, E_ALFA, H_ARQRP, NULL, NULL, vl_log );
   nr[1].destino = NULL;

#ifdef MSDOS
   strcpy(log, C_LOGDOS);
#else
   strcpy(log, C_LOGUNIX);
#endif

   if (log[0] != '\0') strcat(log, STR_BARRA);


   if ( nomebase != NULL && nomebase[0] != '\0') {
      TEXT nome_lwr[30];
      strcpy(nome_lwr, nomebase);
   	strlwr(nome_lwr);

      strcat(log, nome_lwr);
      strcat(log, ".");
      strcat(log, C_TMPLOG);
   }
   else {
      strcat(log, C_TMPLOG);
   }

   win_ajuda    = "REPROBASE";

   j1 = abre_janela(J_ARQLOGRB);   /* abre janela nome do log */

   fw_title(j1, T_JANLOG, j1->w_defatr);
	fwbox(j1);

   ret = ed_read(j1, &nr[0], 1, LIMPA_LE, LF13E);

	if ( j1 ) fwkill( j1 );

	if ( ret == K_ESC ) {  /* erro ou ESC */
		ret = -1;
      goto fim;
   }
   else
      ret = 0;

   if ( ( fpo_glb = fopen(log, "w")) == NULL ) {
      mens_erro(H_OPESO, E_OPE_SO, errno, log);
      ret = -1;
      goto fim;
   }
   else e_log_glb = 0;

   ap_adverte = 0;

	jl_glb = abre_janela( J_LOGREPRO );
   fw_title(jl_glb, C_IN_JLOG, jl_glb->w_defatr);
	fwbox(jl_glb);
   jl_glb->w_flags |=  W_WRAP;
   jl_glb->w_flags &= ~W_NOSCROL;


fim :

   return(ret);
}






/*-----------------------------------------------------------------------*/
F__LOC   COUNT manut_reb( jr )
WIN *jr;
{  COUNT ret = 0;
   TEXT *ajuda_ant = win_ajuda;

   ap_operacao = REPROC;

      ret = decida(TW_OT_D00, TW_OT_D99, H_OTIMIZA, 2, 39, "%s", TW_OT_D01);

      if ( ret < 0 )
         goto fim;
      else if ( ret == 1 )
         idx_otimiza = 'T';
      else
         idx_otimiza = 'E';


   if ( repro_base(jr) != 0 )
      ret = -1;

fim :

   seta_acesso( NULL );  /* deve zerar acesso */

   win_ajuda    = ajuda_ant;

sai:

   idx_otimiza = 'E';
   ap_operacao = 0;

   return(ret);
}





F__LOC   COUNT repro_base(jr)
WIN *jr;
{  COUNT i, ret, base_aberta;
   LONG tot_regs;
   TEXT *ajuda_ant = win_ajuda;
   WIN   *jm, *jf;
   TEXT mens_log[100];
   TEXT titulo[80];
   TEXT *vet_zap;

   win_ajuda = "REPRO_DA";

   versao_velha = 0;

   jm = jf = NULL;
   vet_zap = NULL;
   base_aberta = 0;

   jm = abre_mensagem(str_vazia);
	jf = abre_funcoes(str_vazia);

   ret = 0;

   fwframe( 1,  1,  3, 77, SIMPLES, jr);
   sprintf(titulo, "%s%s", C_RB_TIT, rec_base->bs_nome);
   mvwprintw(jr ,  2, 25, titulo);


   mvwprintw(jr ,  5, 3, C_RB_F1);
   mvwprintw(jr ,  7, 3, C_RB_F2);
   mvwprintw(jr ,  9, 3, C_RB_F3);


   if (  (vet_zap = malloc(256)) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      wr_log(1, SEM_MEMO);
      ret = -1;
      goto fim;
   }
   else for ( i = 0 ; i < 255; i++ ) vet_zap[i] = 0;


   sprintf(mens_log, ML_REPRO, rec_base->bs_nome);
   wr_log(0, mens_log);


   /*********************************   FASE 1 - Reprocessamento de LB4 ***/
   wr_log(0, ML_F1BEG);

   mvwprintw(jr ,  5, 3, C_RB_F1D);       fwrefresh(jr);
   ret = tbd_manut(vet_zap);
   mvwprintw(jr ,  5, 3, C_RB_F1);        fwrefresh(jr);

   if ( ret != 0 ) {
      ret = -1;
      goto fim;
   }
   else
      wr_log(0, ML_F1END);


   /*********************************   FASE 2 - Recuperacao dos dados ***/
   wr_log(0, ML_F2BEG);

   if ( new_base(rec_base) != 0 ) {
      wr_log(1, ML_ERRDAD);
      ret = -1;
      goto fim;
   }

   if ( open_base(rec_base->bs_nome, REPROC) != 0 ) {
		ret = -1;
      goto fim;
   }
   else base_aberta = 1;


   mvwprintw(jr ,  7, 3, C_RB_F2D);       fwrefresh(jr);
   ret = tbp_scan( BASE_EDIT, vet_zap, &tot_regs);
   mvwprintw(jr ,  7, 3, C_RB_F2);        fwrefresh(jr);

   if ( ret != 0 ) {
      ret = -1;
      goto fim;
   }
   else
      wr_log(0, ML_F2END);

   /* para fazer com que possamos somente reindexar total a base em caso de pane */
   if ( close_base( ) != 0 || open_base(rec_base->bs_nome, REPROC) != 0 ) {
      TEXT linha[100];
      mens_erro(H_REBCLS, E_REBCLS, rec_base->bs_nome);
      sprintf(linha, E_REBCLS, rec_base->bs_nome);
      wr_log(1, linha);
      goto fim;
   }

   /*********************************   FASE 3 - Reindexacao dos dados ***/
   wr_log(0, ML_F3BEG);

	funcoes(jf, funcao("E"));
   while( w_kbhit(jr)) w_rgc(jr);  /* limpa teclado */


   mvwprintw(jr ,  9, 3, C_RB_F3D);       fwrefresh(jr);
   ret = tbp_index( BASE_EDIT, tot_regs);
   mvwprintw(jr ,  9, 3, C_RB_F3);        fwrefresh(jr);

   if ( ret != 0 ) {
      ret = -1;
      goto fim;
   }
   else
      wr_log(0, ML_F3END);


   base_aberta = 0;

   if ( close_base( ) != 0 ) {
      TEXT linha[100];
      mens_erro(H_REBCLS, E_REBCLS, rec_base->bs_nome);
      sprintf(linha, E_REBCLS, rec_base->bs_nome);
      wr_log(1, linha);
      goto fim;
   }

   if ( ren_base() != 0 ) {
      ret = -1;
      goto fim;
   }

   ret = 0;

fim :

   if ( base_aberta && close_base( ) != 0 ) {
      TEXT linha[100];
      mens_erro(H_REBCLS, E_REBCLS, rec_base->bs_nome);
      sprintf(linha, E_REBCLS, rec_base->bs_nome);
      wr_log(1, linha);
   }

   if ( vet_zap != NULL ) free(vet_zap);

   if ( jl_glb != NULL ) {
      TEXT xe[3];

      xe[0] = K_ESC;    xe[1] = K_ENTER;  xe[2] = '\0';


	   if ( ap_adverte > 0 ) {
         TEXT mens[100];
         sprintf(mens, M_LOGWAR, ap_adverte);
         wr_log(0, mens);
      }

      if ( jl_glb->w_cy > jl_glb->w_aheight ) {
         mens_1(jm, M_FRB_NAV );
         w_navega(jm, jl_glb, jl_glb->w_cy, xe);
      }
      else {
         mens_1(jm, M_FRB_DIR);
         fwqgc(jm);
      }
   }

   versao_velha = 0;  /* tenho que fazer into para funcoes de manip textos */

	fecha_funcoes(jf);
	fecha_mensagem(jm);

   win_ajuda = ajuda_ant;

   while( w_kbhit(jr)) w_rgc(jr);  /* limpa teclado */

   return( ret );
}



F__LOC   COUNT ren_base()
{  COUNT ret, i;
   TEXT *p, *pnnv, *pnat;
	TEXT bak[PATH_MAX];   /* nome de arquivo backup */
	TEXT novo[PATH_MAX];  /* NOVOS Arquivos */
	TEXT atual[PATH_MAX]; /* Arquivo que seram sobrepostos */
#ifdef  UNIX
#ifndef CTSRVR
   struct stat status;
   int uid = -1, gid = -1;
#endif
#endif


   info_base("@@@", rec_base->bs_path, LB_1_EXT, novo);
   info_base(rec_base->bs_nome, rec_base->bs_path, LB_1_EXT, atual);

   /* faz pnnv apontar para numero do arquivo LB? */
   pnnv = strchr(novo, '\0');
   pnnv--;

   /* faz pnat apontar para numero do arquivo LB? */
   pnat = strchr(atual, '\0');
   pnat--;


   for ( i = 1; i <= 3; i++ ) {

      *pnnv = '0' + i;  /* Monta nome do arquivo temporario @@@ */
      *pnat = '0' + i;  /* Monta nome do arquivo atual */

      if ( access(novo, 0) != 0 ) { /* se NAO existir novo tem algo errado */
         mens_erro(H_RBAPG, E_RBAPG, novo);
         wr_log(1, E_RBREN);
         ret = -1;
         goto fim;
      }

#ifdef  UNIX
#ifndef CTSRVR
      if (stat( atual, &status ) == -1) {
         status.st_uid = status.st_gid = 0xFFFF;
      }
#else
WWW ERRO !!!!!!!! implementar STAT via servidor !!!!!!!!!!!!!!
#endif
#endif

      if ( access(atual, 0) == 0 ) { /* se existir antigo salva como @B[123] */
         /*  Renomeia arquivo original(atual) para XXXXXXXX.@XX (bak)  */
         strcpy(bak, atual);
         p = &bak[strlen(bak) -1];
         while ( *p != '.' )
            p--;
         p++;
         *p = '@';

         if ( access(bak, 0) == 0 ) {

            DELFILE(bak);
         }

         if ( RENFILE(atual, bak) != NO_ERROR ) {
            mens_erro(H_RBRXY, E_RBRXY, atual, bak);
            wr_log(1, E_RBREN);
            ret = -1;
            goto fim;
         }
      }


      if ( RENFILE(novo, atual) != NO_ERROR ) {
         mens_erro(H_RBRXY, E_RBRXY, atual, bak);
         wr_log(1, E_RBREN);
         ret = -1;
         goto fim;
      }

#ifdef  UNIX
#ifndef CTSRVR
      if (status.st_uid != 0xFFFF) {
         chmod( atual, status.st_mode );
         chown( atual, status.st_uid, status.st_gid );
      }
#else
WWW ERRO !!!!!!!! implementar STAT via servidor !!!!!!!!!!!!!!
#endif
#endif

   }

fim :

   return(ret);
}


