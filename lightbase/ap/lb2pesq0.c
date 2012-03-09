#include  "lb2all.h"

#include  <string.h>
#include  HD_DIR
#include  HD_IO

#ifdef UNIX
#include  <unistd.h>
#endif

#ifdef    VAXVMS
#include  <file.h>
#include  <types.h>
#include  <stat.h>
#else
#include  <fcntl.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */


#define   MODO_DEF_PSQ   SIM

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2perso.h"
#include  "lb2pesq.h"
#include  "lb2prot1.h"
#include  "lb2trans.h"



#define   PSQ_FMASC      "PSQXXXXXX"
#define   CRIA 'C'
#define   MATA 'M'
#define   FLAG_PADRAO   1


#define   H_CFINDPSQ       "CF_INDPSQ"
#define   H_PSQLST         "E_PSQ_LST"
#define   H_PSQNOT         "M_PSQ_NOT"

#define   H_CFVERDO     "CF_VERDO"     /* qualquer tecla para ver documentos selecionados */


#define   N_BLOCO(r)    ((((r) - 1) / PSQ_SLOT) + 1)
#define   OFF_BLOCO(r)  (((r) - ((N_BLOCO(r) - 1) * ((LONG)PSQ_SLOT))) - 1)

/*---------------------------------------------------------------------*/
/*  Mapa de localizacao em disco                                       */
/*---------------------------------------------------------------------*/
struct psq_mapa {
   UTEXT  mp_lista;   /* Numero da lista ao qual o bloco pertence */
   UTEXT  mp_bloco;   /* numero do bloco que esta neste buffer */
};

/*---------------------------------------------------------------------*/
/*  Controle de buffers                                                */
/*---------------------------------------------------------------------*/
struct {
   SDINDEX *wb_vetor;     /* endereco do buffer */
   COUNT wb_modif;     /* indica que buffer foi modificado */
   COUNT wb_idade;     /* idade do buffer */
} vet_wkbuf[3];


struct psq_mapa *vet_mapa;

COUNT ord_slot = 0; /* flag de ordenacao de lista */
TEXT fname_psq[PATH_MAX+1];
FILE *fp_psq;
COUNT fd_logpsq;
jmp_buf psq_jmperro;
SDINDEX sdi_mor = { -1L, -1, -1, -1, -1};
WIN *jm_wait; /* janela para ficar rodando o ventidador */

#ifdef MSDOS
   COUNT aloc_work( VOID );
   COUNT aloc_mapa( VOID );
   COUNT aloc_lista( VOID );
   SI    psqcmp_testa(SDINDEX *, SDINDEX *);
   COUNT str_meso(TEXT *, TEXT *, COUNT);
   COUNT new_lista(VOID);
   COUNT pega_buf(COUNT);
   VOID  assoc_buf(COUNT, COUNT);
   COUNT loc_mapa(COUNT, COUNT);
   SDINDEX *load_lista(COUNT, COUNT);
   COUNT ex_loadlista(COUNT, COUNT, TEXT *);
   COUNT ex_savelista(COUNT, TEXT *);
   VOID  merge_bloco(COUNT);
   VOID  save_bloco(COUNT);
   VOID  psq_fileger(COUNT);
   VOID  libera_lista(COUNT);
   COUNT v_menos_1(Tab *);
   VOID  atu_tabela(Tab *, COUNT);
   COUNT show_result(COUNT, TEXT *, LONG, LONG, LONG);
   TEXT  *fonema(TEXT*);
   LONG ger_lstfile(FILE *, COUNT);
   FILE *opfile_flags(TEXT *);
   VOID reset_args( TEXT *);
	VOID abre_psqlog( VOID );
	VOID grava_psqlog( TEXT * );
   TEXT *log_interativa( TEXT *, LONG );
#else
   COUNT aloc_work( );
   COUNT aloc_mapa( );
   COUNT aloc_lista( );
   SI    psqcmp_testa( );
   COUNT str_meso( );
   COUNT new_lista( );
   COUNT pega_buf( );
   VOID  assoc_buf( );
   COUNT loc_mapa( );
   SDINDEX *load_lista( );
   COUNT ex_loadlista( );
   COUNT ex_savelista( );
   VOID  merge_bloco( );
   VOID  save_bloco( );
   VOID  psq_fileger( );
   VOID  libera_lista( );
   COUNT v_menos_1( );
   VOID  atu_tabela( );
   COUNT show_result( );
   TEXT  *fonema( );
   LONG ger_lstfile( );
   FILE *opfile_flags( );
   VOID reset_args( );
	VOID abre_psqlog( );
	VOID grava_psqlog( );
	TEXT *log_interativa( );
#endif



COUNT tipo_pesquisa;
#ifndef RUNTIME
   extern TEXT auto_last[];
   extern COUNT ap_flag_auto;
#endif


#ifndef CP_FLAG
F__GLB   COUNT pesquisa(sele)
COUNT sele;
{  COUNT ret = 0, lista_ret, lista_nova, rf_ocorre, rf_docs;
   WIN *jf, *jm, *ji;
   WIN *ja = NULL;
	Tab   t;
   TEXT *ajuda_ant = win_ajuda, *str_hist;
   LONG tot_regs, tot_toind, tot_dels, tot_ativos;
#ifdef MSDOS
   unsigned int di;
#endif

   up_str(GRP_PESQ);

   fp_psq = NULL;
   fd_logpsq = -1;

   abre_psqlog(  );

   str_hist = NULL;

   if ( fd_logpsq != -1 )
      str_hist = malloc( 300 );

   sele_pesq = sele;
   free_record(sele_pesq);

   ja= jf = jm = jm_wait = ji = NULL;

   vet_wkbuf[0].wb_vetor = NULL;
   vet_wkbuf[1].wb_vetor = NULL;
   vet_wkbuf[2].wb_vetor = NULL;

   vet_mapa  = NULL;
   vet_lista = NULL;

   if ( aloc_work() < 0 ) {               /* areas para manuseio de */
      mens_erro(H_NOMEMO, SEM_MEMO);      /* grupos de PSQ_SLOT elementos */
      ret = -1;                           /* de SDINDEX */
      goto fim;
   }

   if ( aloc_mapa() < 0 ) {               /*  Aloca TAM_MAPA estruturas */
      mens_erro(H_NOMEMO, SEM_MEMO);      /* para mapear listas em disco */
      ret = -1;
      goto fim;
   }

   if ( aloc_lista() < 0 ) {              /* Aloca NUM_LISTAS estruturas */
      mens_erro(H_NOMEMO, SEM_MEMO);      /* para controlar registros (SDINDEX) */
      ret = -1;                           /* pertencentes a uma lista */
      goto fim;
   }


   if ( aloc_brilha('A') != 0 ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   w_refresh = 1;

   ja = abre_janela(J_PSQARG);  /* abre janela para argumentos */

	def_tabela(&t, 2, 13, size_x - 6, size_y - 12 - 5,
               MAX_PESQUISAS, MAX_PESQUISAS,
               X_MSC_TAB, MS_CAMPO, MS_BARRA, 1, MS_JANELA, MS_DESTAQUE,
				   ( char *(* )[]) ap_tabpsq, C_PSQ_2WIN, str_nada);

   t.win   = win_make(t.v_col, t.v_lin, t.a_col, t.a_lin, t.col, t.lin);
   t.enter = T_ENTER;  /* so seleciona com ENTER */
   t.comando = v_menos_1;

   tabela(&t); /* so faz imprimir na janela */

   t.comando = NULL; /* reabilita tabala */


   fw_title(ja, C_PSQ_1WIN, ja->w_atrib);
	fwbox(ja);

   fwframe(0, 41, 1, 62, SIMPLES, ja);

	mvwprintw(ja, 0, 46, C_PSQ_OCOR);
	mvwprintw(ja, 0, 64, C_PSQ_DOCS);
	mvwprintw(ja, 1,  2, C_PSQ_1ALIN);
	mvwprintw(ja, 1, 42, C_PSQ_1BLIN);
	mvwprintw(ja, 1, 64, C_PSQ_1CLIN);
	mvwprintw(ja, 7, 51, C_PSQ_RES);


   jm = abre_mensagem(str_nada);
   jf = abre_funcoes(str_nada);
   ji = abre_insercol(C_PSQ_ICOL);

   jm_wait = jm;

   psq_interativa(ja, jm, jf, 'I');

   w_refresh = 0;

   if ( look_regs(sele, &tot_regs, &tot_ativos, &tot_dels, &tot_toind) < 0 ) {
      mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
      ret = -1;
      goto fim;
   }

   if ( tot_toind > 0 ) {
      TEXT *p;

      if ( tot_toind == 1L ) p = TW_PQ_T34;
      else                   p = TW_PQ_T31;


      if ( decida( TW_PQ_T30, TW_PQ_T39, H_CFINDPSQ, 3, 42, "%l%s\n%s\n%s",
                   tot_toind, p, TW_PQ_T32, TW_PQ_T33 ) != 0 ) {
         ret = -1;
         goto fim;
      }
   }

   tipo_pesquisa = 0;
   ret = 0;

   /* retorno de erro de acesso a disco */

#ifdef MSDOS
   di = _DI;  /* longjmp nao restaura di em TURBOC ++ */
#endif

   ret = setjmp(psq_jmperro);

#ifdef MSDOS
   _DI = di;  /* longjmp nao restaura di em TURBOC ++ */
#endif


   if ( ret != 0 ) {
      mens_erro(H_PSQLST, E_PSQ_LST, ret);
      goto fim;
   }

   tipo_pesquisa = 1;

   while ( ret == 0 ) {

      lista_ret = 0;

      if ( tipo_pesquisa ) {
         ret = psq_interativa(ja, jm, jf, 'R');
      }
      else {
         funcoes(jf, funcao(C_PSQ_1ROD));
         ret = psq_tabela(jm, jf, &t);
      }
      /*
      **   Possiveis valores de ret e seus significados :
      **      -1  -  ESC              -> sai da pesquisa
      **       0  -  TAB ou BACK_TAB  -> Alterna tipo de pesquisa
      **       1  -  ENTER ou K_F2    -> Manten a pesquisa
      */

      if ( ret < 0 ) goto fim;

      if ( ret == 0 ) {
         tipo_pesquisa = ! tipo_pesquisa;
         continue;
      }

      hot_palavra[0] = '\0';
      hot_key = 0;

nova_pesq:
refina :

      /* executa pesquisa */

      mens_1(jm, C_PSQ_WAIT );
#ifndef PESQUISA_RECURSIVA
   hot_key = 0;
#endif
      if ( hot_key ) {
         TEXT c;
         if (hot_key == K_F2 ) {
            c = '-';
            if ( lista_ret > 0 ) {
               libera_lista(lista_ret);
               lista_ret = 0;
            }

            push_brilha(INICIALIZA, str_nada);
            reset_args(hot_palavra);  /* atribui nova pesquisa a arg1 */

   			ret = exec_tabela( 0, hot_palavra);  /* WWW para testar sentenca */

            /* ret = carregar(hot_palavra, ""); */
            rf_docs = rf_ocorre = 0L;
            if ( ret > 0 ) {
               conta_docs( ret );
               rf_ocorre = vet_lista[ret].tot_ocor;
               rf_docs   = vet_lista[ret].tot_doc;
            }
         }
         else { /* K_F4 */
            c = '+';
   			lista_nova = exec_tabela( 0, hot_palavra);  /* WWW para testar sentenca */
            /* lista_nova = carregar(hot_palavra, ""); */
            rf_docs = rf_ocorre = 0L;
            if (lista_nova > 0 ) {
               conta_docs( lista_nova );
               rf_ocorre = vet_lista[lista_nova].tot_ocor;
               rf_docs   = vet_lista[lista_nova].tot_doc;
               ret = combina(lista_ret, lista_nova, E );
            }
            else {
               libera_lista(lista_ret);
               lista_ret = 0;
               rf_ocorre = 0;
               rf_docs   = 0;
               ret = 0;
            }
         }


         if (ret > 0 ) {
            conta_docs( ret ); /* conta o numero de documentos */
            /* push_brilha(PUSH, hot_palavra); WWW nao deve fazer posto que passou a ser via tabela */
            if ( str_hist != NULL ) {
         	   sprintf(str_hist, "%c %s - %ld", c, hot_palavra, vet_lista[ret].tot_doc);
				   grava_psqlog(str_hist);
            }
         }
         else {
            if ( str_hist != NULL ) {
         	   sprintf(str_hist, "%c %s - %ld", c, 0L, 0L);
				   grava_psqlog(str_hist);
            }
         }
      }
      else {
         if ( tipo_pesquisa )
            ret = exec_interativa(ja);
         else  {
            if ( str_hist != NULL ) {
         		strcpy(str_hist, "- ");
         		strcat(str_hist, &ap_tabpsq[ret - 1][TAM_PSQPROT]);
         		trim(str_hist);
            }
            ret = exec_tabela( ret, NULL ); /* numero da pesquisa a ser executada */
            if ( str_hist != NULL && ret < 0 )
      			str_hist[0] = '\0';
            atu_tabela(&t, t.d_atr);
         }
      }

      lista_ret = ( ret > 0 ) ? ret : 0;

      /* caso seja uma hot_key a sentenca de log ja estara pronta */
      if ( hot_key == 0 )  {
	      if ( tipo_pesquisa ) {
				if ( str_hist != NULL &&
                 log_interativa(str_hist, vet_lista[ret].tot_doc) != NULL)
					grava_psqlog(str_hist);
			}
			else {
				if ( str_hist != NULL && str_hist[0] != '\0' ) {
					TEXT num[20];
					sprintf(num, " - %ld", vet_lista[ret].tot_doc);
					strcat(str_hist, num);
					grava_psqlog(str_hist);
				}
			}
      }


      /***** caso seja um refinamento ou uma nova pesquisa, mostra resultado */
      if ( hot_key ) {
         if ( show_result(hot_key, hot_palavra, vet_lista[ret].tot_doc,
                           rf_ocorre, rf_docs) == K_ESC ) {
            ret = 0;
         }
      }

      mens_1(jm, str_vazia);

      hot_palavra[0] = '\0';
      hot_key = 0;

      if ( ret > 0 && vet_lista[ret].tot_doc > 0L) {
         ret = visualiza( ret, 0);
      }
      else {
         if ( ret != -5 ) /* ESC na navegacao na janela */
            mens_erro(H_PSQNOT, M_PSQ_NOT);
         ret = 0;
      }

      if ( hot_key )
         goto refina;

      if ( lista_ret )
         libera_lista(lista_ret);

      if ( tipo_pesquisa == 0 )
         atu_tabela(&t, t.l_atr);
   }


fim :

   config_save(ap_nome, v_bases[BASE_EDIT]->bs_nome, 0);

   if ( fp_psq != NULL ) { /* mata arquivo se foi criado */
      psq_fileger(MATA);
   }

   fecha_insercol(ji);
   fecha_funcoes(jf);
   fecha_mensagem(jm);

   jm_wait = NULL;

   win_ajuda = ajuda_ant;

	fwkill( t.win );
	fwkill( ja );

   aloc_brilha('F');

   if ( vet_lista != NULL ) free((TEXT *)vet_lista);
   if ( vet_mapa  != NULL ) free((TEXT *)vet_mapa );

   if ( vet_wkbuf[2].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[2].wb_vetor);
   if ( vet_wkbuf[1].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[1].wb_vetor);
   if ( vet_wkbuf[0].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[0].wb_vetor);

   if ( str_hist != NULL )
      free(str_hist);

   if ( fd_logpsq != -1) {
      close(fd_logpsq);
      fd_logpsq = -1;
   }

   down_str(GRP_PESQ);
   return(ret);
}
#endif


F__GLB   COUNT pre_pesquisa(sele, pesq_definida)
COUNT sele;
TEXT *pesq_definida;
{  COUNT ret = 0, lista_ret;
   WIN *jf, *jm, *ji;
   LONG tot_regs, tot_toind, tot_dels, tot_ativos;
   TEXT *str_hist;
#ifdef MSDOS
   unsigned int di;
#endif

   up_str(GRP_PESQ);

   fp_psq = NULL;
   fd_logpsq = -1;

   abre_psqlog(  );

   str_hist = NULL;

   if ( fd_logpsq != -1 )
      str_hist = malloc( 300 );

   sele_pesq = sele;
   free_record(sele_pesq);

   jf = jm = jm_wait = ji = NULL;

   vet_wkbuf[0].wb_vetor = NULL;
   vet_wkbuf[1].wb_vetor = NULL;
   vet_wkbuf[2].wb_vetor = NULL;

   vet_mapa  = NULL;
   vet_lista = NULL;

   if ( aloc_work() < 0 ) {               /* areas para manuseio de */
      mens_erro(H_NOMEMO, SEM_MEMO);      /* grupos de PSQ_SLOT elementos */
      ret = -1;                           /* de SDINDEX */
      goto fim;
   }

   if ( aloc_mapa() < 0 ) {               /*  Aloca TAM_MAPA estruturas */
      mens_erro(H_NOMEMO, SEM_MEMO);      /* para mapear listas em disco */
      ret = -1;
      goto fim;
   }

   if ( aloc_lista() < 0 ) {              /* Aloca NUM_LISTAS estruturas */
      mens_erro(H_NOMEMO, SEM_MEMO);      /* para controlar registros (SDINDEX) */
      ret = -1;                           /* pertencentes a uma lista */
      goto fim;
   }


   if ( aloc_brilha('A') != 0 ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   jm = abre_mensagem(str_nada);
   jf = abre_funcoes(str_nada);
   ji = abre_insercol(C_PSQ_ICOL);

   jm_wait = jm;

   if ( look_regs(sele, &tot_regs, &tot_ativos, &tot_dels, &tot_toind) < 0 ) {
      mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
      ret = -1;
      goto fim;
   }

   if ( tot_toind > 0 ) {
      TEXT *p;

      if ( tot_toind == 1L ) p = TW_PQ_T34;
      else                   p = TW_PQ_T31;


      if ( decida( TW_PQ_T30, TW_PQ_T39, H_CFINDPSQ, 3, 42, "%l%s\n%s\n%s",
                   tot_toind, p, TW_PQ_T32, TW_PQ_T33 ) != 0 ) {
         ret = -1;
         goto fim;
      }
   }


   ret = 0;

   /* retorno de erro de acesso a disco */

#ifdef MSDOS
   di = _DI;  /* longjmp nao restaura di em TURBOC ++ */
#endif

   ret = setjmp(psq_jmperro);

#ifdef MSDOS
   _DI = di;  /* longjmp nao restaura di em TURBOC ++ */
#endif


   if ( ret != 0 ) {
      mens_erro(H_PSQLST, E_PSQ_LST, ret);
      goto fim;
   }


   lista_ret = 0;

   /* executa pesquisa */

   mens_1(jm, C_PSQ_WAIT );

   if ( str_hist != NULL  ) {
	   strcpy(str_hist, "- ");
	   strcat(str_hist, pesq_definida);
	   trim(str_hist);
   }

   ret = exec_tabela( 0, pesq_definida );

   if ( str_hist != NULL && ret < 0 )
		str_hist[0] = '\0';

   if ( ret > 0 )
      lista_ret = ret;

	if ( str_hist != NULL && str_hist[0] != '\0' ) {
		TEXT num[20];
		sprintf(num, " - %ld", vet_lista[ret].tot_doc);
		strcat(str_hist, num);
		grava_psqlog(str_hist);
	}

   mens_1(jm, str_vazia);

   if ( ret > 0 && vet_lista[ret].tot_doc > 0L)
      ret = visualiza( ret, 1);
   else {
      if ( ret != -5 ) /* ESC na navegacao na janela */
         mens_erro(H_PSQNOT, M_PSQ_NOT);
      ret = 0;
   }

   if ( lista_ret )
      libera_lista(lista_ret);


fim :

   if ( fp_psq != NULL ) { /* mata arquivo se foi criado */
      psq_fileger(MATA);
   }

   fecha_insercol(ji);
   fecha_funcoes(jf);
   fecha_mensagem(jm);

   jm_wait = NULL;

   aloc_brilha('F');

   if ( vet_lista != NULL ) free((TEXT *)vet_lista);
   if ( vet_mapa  != NULL ) free((TEXT *)vet_mapa );

   if ( vet_wkbuf[2].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[2].wb_vetor);
   if ( vet_wkbuf[1].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[1].wb_vetor);
   if ( vet_wkbuf[0].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[0].wb_vetor);

   if ( str_hist != NULL )
      free(str_hist);

   if ( fd_logpsq != -1) {
      close(fd_logpsq);
      fd_logpsq = -1;
   }

   down_str(GRP_PESQ);
   return(ret);
}



F__LOC	 VOID atu_tabela(t, atributo)
Tab *t;
COUNT atributo;
{  COUNT salva;
   TEXT *str;

   str = ( *t->opcoes )[ t->tarja ];

   salva = t->win->w_atrib;
   t->win->w_atrib = atributo;
   fwat(1, t->tarja, t->win);
   fwpsl((UTEXT *)str, t->win, TAM_PSQPROT);
   t->win->w_atrib = salva;
}



/*#  ---------------------------------------------------------------------
**#   Funcao...: aloc_work()
**#   Objetivo.: alocar buffers de trabalho.  Aloca TRES buffers de
**#              PSQ_SLOT elementos ( cada elemento e' do tipo SDINDEX)
**#   Entrada..: Nenhuma
**#   Retorna..: -1 - No caso de nao conseguir areas
**#              0  - caso consiga aloca areas
**#   Utiliza..: vet_wkbuf[]
**#
**#
**/
F__LOC	 COUNT aloc_work()
{  COUNT ret;
   SDINDEX *p1, *p2, *p3;

   p1 = p2 = p3 = NULL;
   ret = -1;

   if ( ( p1 = (SDINDEX *)calloc( PSQ_SLOT + 1, TAM_EVET)) != NULL ) {
      if ( ( p2 = (SDINDEX *)calloc( PSQ_SLOT + 1, TAM_EVET)) != NULL ) {
         if ( ( p3 = (SDINDEX *)calloc( PSQ_SLOT + 1, TAM_EVET)) != NULL ) {
            vet_wkbuf[0].wb_vetor = p1;   vet_wkbuf[0].wb_idade = 0;  vet_wkbuf[0].wb_modif = 0;
            vet_wkbuf[1].wb_vetor = p2;   vet_wkbuf[1].wb_idade = 0;  vet_wkbuf[1].wb_modif = 0;
            vet_wkbuf[2].wb_vetor = p3;   vet_wkbuf[2].wb_idade = 0;  vet_wkbuf[2].wb_modif = 0;
            ret = 0;
         }
         else { free(p1); free(p2); }
      }
      else free(p1);
   }

   return(ret);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: aloc_mapa()
**#   Objetivo.: alocar area para mapear listas em disco. Aloca TAM_MAPA
**#              elementos ( cada elemento e' do tipo psq_mapa)
**#   Entrada..: Nenhuma
**#   Retorna..: -1 - No caso de nao conseguir areas
**#              0  - caso consiga aloca areas
**#   Utiliza..: vet_mapa
**#
**#
**/
F__LOC	 COUNT aloc_mapa()
{  COUNT ret;
   TEXT *p1 = NULL;

   ret = -1;

   if ( ( p1 = calloc( TAM_MAPA + 2, sizeof(struct psq_mapa))) != NULL ) {
      vet_mapa = (struct psq_mapa *)p1;
      ret = 0;
   }

   return(ret);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: aloc_lista()
**#   Objetivo.: alocar areas para controlar utilizacao das listas de
**#              elementos selecionados nas pesquisas. Aloca NUM_LISTAS
**#              elementos ( cada elemento e' do tipo Psq_lista)
**#   Entrada..: Nenhuma
**#   Retorna..: -1 - No caso de nao conseguir areas
**#              0  - caso consiga aloca areas
**#   Utiliza..: vet_lista
**#
**#
**/
F__LOC	 COUNT aloc_lista()
{  COUNT ret, i;
   TEXT *p1 = NULL;

   ret = -1;

   if ( ( p1 = calloc( NUM_LISTAS + 2, sizeof(Psq_lista))) != NULL ) {

      vet_lista = (Psq_lista *) p1;

      for ( i = 0; i < NUM_LISTAS; i++)
         vet_lista[i].n_buf = -1;

      ret = 0;
   }

   return(ret);
}



F__GLB   VOID conta_docs( lista )
COUNT lista;
{  LONG ndocs;
   LONG  reg_ant;
   SDINDEX *p_sdi;
   LONG tot_regs, tot_dels, tot_ativos;

   if (lista < 1 ) numdebug(670);

   reg_ant = 0L;
   ndocs   = 0;

   if ( vet_lista[lista].flag >= 8 ) { /* lista toda ou lista_pronta */
      if ( vet_lista[lista].flag == 9 ) { /* lista toda */
         if ( look_regs(BASE_EDIT, &tot_regs, &tot_ativos, &tot_dels, NULL) == 0 )
            ndocs = tot_ativos;
      }
#ifdef CP_FLAG
      else {
           ndocs = ger_lstfile(vet_lista[lista].file_cp, '?');
      }
#endif
   }
   else {

      p_sdi = prim_sdi(lista);

      while ( p_sdi != NULL ) {

         if ( p_sdi->registro != reg_ant) {
            ndocs++;
            reg_ant = p_sdi->registro;
         }

         p_sdi = prox_sdi(lista);
      }
   }

   vet_lista[lista].tot_doc = ndocs;
   vet_lista[lista].it_item = 0;
}





F__GLB   COUNT carregar( arg, cmp)
TEXT *arg;
TEXT *cmp;
{  COUNT ret, radical, tree, fim, campo, i, pula, interroga;
   COUNT helice = 0, contador = PSQ_SLOT / 8;
   COUNT lista, res, n_ind;
   COUNT ret_trans;
   UTEXT rodape[60], begin[3], chave[LB_KEYMAX + 1], *p, c_ant;
   UTEXT *p1, *p2;
   UTEXT val_key[LB_KEYMAX + 1];
   UTEXT aux[LB_KEYMAX + 1];
   Campo **vetcmp;
   POINTER pos;
   S_INDEX si;
   COUNT tm, tam_min, tam_key;
   DOUBLE limite_db, val_db;
   COUNT op_numerico, aceita_limite, ind, decimais;
   TEXT *ajuda_ant = win_ajuda;
   LONG regs;

   /* so devera ligar esta flag para ordenar lista durante inclusoes */
   /* nao deixar de desligar antes de retornar */
   ord_slot = 1;
   lista    = -1;


   if ( (ret_trans = BEG_TRANS(TR_CARREGAR, 0L, 0)) < 0 ) {
      ret = -1;
      goto fim_gt;
   }


   vetcmp = pos_vetcmp(sele_pesq);

   ret = 0;

   if ( (lista = new_lista()) < 0 ) {
      ret = -1;
      goto fim_gt;
   }

   if ( strcmp(arg, BASE_TODA) == 0 ) {
      vet_lista[lista].flag = 9;

      look_regs(sele_pesq, NULL, &regs, NULL, NULL);

      vet_lista[lista].tot_vet = regs;
      vet_lista[lista].tot_campo = regs;
      vet_lista[lista].tot_doc = regs;
      ret = 0;
      goto fim_gt;
   }

#ifdef CP_FLAG
   /* verifica se campo e flag. Se for Pega lista pronta */
   if ( *cmp != '\0') {
      ind = ver_campo(sele_pesq, cmp, &campo);
      if ( ind >= 0 ) {
         if ( vetcmp[ind]->cp_1futuro == 'F' ) {
            vet_lista[lista].flag = 8;
            vet_lista[lista].file_cp = opfile_flags(vetcmp[ind]->cp_nome);
            if ( vet_lista[lista].file_cp != NULL ) {
               regs = ger_lstfile(vet_lista[lista].file_cp, '?');
            }
            else {
               regs = 0;
            }

            vet_lista[lista].tot_campo = 0;
            vet_lista[lista].tot_vet = regs;
            vet_lista[lista].tot_doc = regs;
            ret = 0;
            goto fim_gt;
         }
      }
   }
#endif


   interroga = 0;
   radical   = 0;
   pos       = 0L;
   campo     = -1;

   for (i=0; i < LB_KEYMAX; i++ )
      val_key[i] = 0;

   if ( *cmp != '\0'  && ver_campo(sele_pesq, cmp, &campo) < 0)
      numdebug(671);


   tree = tipo_argumento(arg, 0); /* 0 nao imprime erros */
   n_ind = AI_NUM(sele_pesq) + tree;

   while ( *arg == '$' || *arg == '^' )
      arg++;  /* Tirar o caracter indicador de FONETICA//INICIO */


   switch ( tree ) {

      case TREE_BRANCOS :
            tree = TREE_PALAVRAS;
            n_ind = AI_NUM(sele_pesq) + TREE_PALAVRAS;

            /* para forcar um posicionamento no primeiro. */
            chave[0] = 1;
            chave[1] = 1;
            chave[2] = '\0';
            chave[3] = '\0';
            GTEKEY( n_ind, (TEXT *)chave, ap_chave_ret, 4);

            chave[0] = 1;
            chave[1] = 1;
            chave[2] = campo + 1;
            chave[3] = '\0';
            tam_min  = tm = 3;
            pos =  GTEKEY( n_ind, (TEXT *)chave, ap_chave_ret, 4);

            break;

      case TREE_CHEIOS :
            tree = TREE_PALAVRAS;
            n_ind = AI_NUM(sele_pesq) + TREE_PALAVRAS;

            /* para forcar um posicionamento no primeiro. */
            chave[0] = 1;
            chave[1] = 3;
            chave[3] = '\0';
            chave[2] = '\0';
            GTEKEY( n_ind, (TEXT *)chave, ap_chave_ret, 4);

            chave[0] = 1;
            chave[1] = 3;
            chave[2] = campo + 1;
            chave[3] = '\0';
            tam_min  = tm = 3;
            pos =  GTEKEY( n_ind, (TEXT *)chave, ap_chave_ret, 4);

            break;

      case TREE_FONETICA :
            n_ind = AI_NUM(sele_pesq) + TREE_HORAS;
                   /* para forcar um posicionamento no primeiro. */
                  chave[0] = 2;
                  chave[1] = 0;
                  chave[2] = 0;
                  GTEKEY( n_ind, (TEXT *)chave, ap_chave_ret, 3);
            chave[0] = 2;
            chave[1] = 2;
            strcpy( (TEXT *)&chave[2],  fonema(arg));
            tam_min = tm = strlen((TEXT *)chave);
            if ( tm > 3 )
               pos = GTEKEY( n_ind, /* FONEMAS esta em TREE_HORAS */
                  (TEXT *)chave, ap_chave_ret, tm+1);

            break;

      case TREE_PALAVRAS :

            strcpy((TEXT *)chave, arg);
            prep_palavra((TEXT *)chave);

            /*------------------verifica se possui '*' ou '?' ----------*/

            tam_min = tm = strlen((TEXT *)chave);

            for (i=0, p = NULL; i < tm; i++ ) {
               if ( chave[i] == '*' ) {
                  tam_min--;
                  if ( p == NULL )  p = &chave[i];
                  if ( i == 0 )  radical = 'S';
                  else if ( i == (tm - 1) )
                           radical = (radical == 'S') ? 'M' : 'P';
               }

               else if ( chave[i] == '?' ) {
                  if (interroga == 0 ) interroga = i + 1;
                  val_key[i] = 1;
                  if ( p == NULL )  p = &chave[i];
               }
            }

            if ( interroga )
               radical = (radical == 0) ? '?' : radical;

            /**/
            /***------ localiza primeira chave ****/
            /**/
            if ( p == chave ) {
               begin[0] = 2;
               begin[1] = '\0';
               pos = GTEKEY( n_ind, (TEXT *)begin, ap_chave_ret, 2);
            }
            else {

               /* Mascara temporariamente a chave */
               if ( p != NULL ) { c_ant = *p;  *p = '\0'; }

               memset(&aux[0], '\0', LB_KEYMAX + 1);
               strcpy((TEXT *)aux, (TEXT *)chave);

               pos = GTEKEY( n_ind, (TEXT *)aux, ap_chave_ret, LB_KEYMAX + 1);

               /* restaura a chave */
               if ( p != NULL ) *p = c_ant;
            }

            break;

      case TREE_DATAS   :
      case TREE_HORAS   :
      case TREE_NUMEROS :

            op_numerico   = '=';
            aceita_limite = 1;

            if ( arg[0] == '>' ) {
               op_numerico   = '>';
               if ( arg[1] != '=' ) aceita_limite = 0;
            }
            else if ( arg[0] == '<' ) {
               op_numerico   = '<';
               if ( arg[1] != '=' ) aceita_limite = 0;
            }

            while(*arg == '>' || *arg == '<' || *arg == '=' || *arg == ' ' )
               arg++;

            switch (tree) {
               case TREE_NUMEROS :
                  tam_min  = tm = TAM_CPNUM + TAM_DECIMAIS + 1;
                  limite_db = str_to_db(arg, '.');
                  db_to_str(limite_db, (TEXT *)aux, 4, 0);
                  for ( i= 0; aux[i] != '.' && aux[i] != '\0'; i++)
                     ;
                  p = chave;
                  memset(p, '0', TAM_CPNUM);
                  strcpy((TEXT *)(p + (TAM_CPNUM - i)),  (TEXT *)aux);

                  break;

               case TREE_DATAS   :
                  tam_min  = tm = 6;
                  p = chave;
                  strcpy((TEXT *)p, conv_data(arg));
                  break;

               case TREE_HORAS   :
                  tam_min  = tm = 4;
                  p = chave;
                  strcpy((TEXT *)p, conv_hora(arg));
                  break;
            }

            switch ( op_numerico ) {
               case '<' :
                  if ( tree == TREE_HORAS ) {
                     /* para pular chaves unica */
                     pos = GTEKEY( n_ind, "00", ap_chave_ret, 2);
                  }
                  else {
                     pos = FRSKEY( n_ind, ap_chave_ret);
                  }
                  break;

               case '>' :
                  pos = GTEKEY( n_ind, (TEXT *)chave, ap_chave_ret, tam_min);
                  break;

               case '=' :
                  pos = GTEKEY( n_ind, (TEXT *)chave, ap_chave_ret, tam_min);
                  break;

            }

            break;
   }


   fim  = 0;

   sprintf((TEXT *)rodape, C_PSQ_WROD, arg);

   if ( radical != 0 && push_palavra(RODAPE, (TEXT *)rodape) < 0 ) {
      ret = -1;
      goto fim_gt;
   }

   while ( uerr_cod == 0 && pos != 0L ) {

      pula = 0;

      if ( tree == TREE_PALAVRAS ) {
         /*********************************** TREE_PALAVRAS **********/
         if ( interroga ) {  /* mascara chave localizada com '?' */
            p1 = (UTEXT *)ap_chave_ret;
            p2 = (UTEXT *)val_key;
            for ( tam_key = 0; *p1 > 31; p1++, p2++, tam_key++) {
               if ( *p2 ) *p1 = '?';
            }
         }
         else if ( radical != 0 ) {
            for ( tam_key = 0, p1 = (UTEXT *)ap_chave_ret; *p1 > 31;
                     p1++, tam_key++)
               ;
         }


         switch ( radical ) {

            case 0   :  if ( memcmp(chave, ap_chave_ret, tam_min ) != 0 ||
                           ct_indkey[tam_min] > 31 ) {
                           fim = 1;
                        }

                        p = (UTEXT *)&ct_indkey[tam_min];

                        break;


            case '?' :  if ( tam_key != tam_min ) {
                           pula = 1;
                           break;
                        }

            case 'P' :  if ( tam_key < tam_min ) {
                           pula = 1;
                           break;
                        }

                        if ( memcmp(chave, ap_chave_ret, tam_min) != 0 ) {
                           if ( interroga == 0 ) fim = 1;
                           else if ( interroga > 1  &&
                                 memcmp(chave, ap_chave_ret, interroga - 1) != 0)
                              fim = 1;
                           else
                              pula = 1;
                        }

                        p = (UTEXT *)&ct_indkey[tam_key];

                        break;

            case 'M' :  if ( tam_key < tam_min ) {
                           pula = 1;
                           break;
                        }

                        if ( str_meso(ap_chave_ret, (TEXT *)&chave[1], tam_min) == 0 ) {
                           pula = 1;
                           break;
                        }

                        p = (UTEXT *)&ct_indkey[tam_key];
                        break;

            case 'S' :
                        if ( strncmp(&ap_chave_ret[tam_key - tam_min],
                                    (TEXT *)&chave[1], tam_min) != 0 ) {
                           pula = 1;
                           break;
                        }
                        p = (UTEXT *)&ct_indkey[tam_key];
                        break;
         }
      }
      else {
         /*********************************** OUTRAS TREES **********/

         switch ( tree ) {

            case TREE_FONETICA :

               if ( memcmp(chave, ap_chave_ret, tam_min ) != 0 ||
                  ct_indkey[tam_min] > 31 ) {
                  fim = 1;
               }

               p = (UTEXT *)&ct_indkey[tam_min];

               break;

            case TREE_NUMEROS :
               p = (UTEXT *)&ct_indkey[tam_min];
               c_ant = *p;
               *p = '\0';
               val_db = str_to_db(ct_indkey, '.');
               *p = c_ant;

               switch ( op_numerico ) {
                  case '=' :
                     if ( val_db != limite_db )
                        fim = 1;
                     break;

                  case '<' :
                     if ( val_db >= limite_db ) {
                        if ( val_db > limite_db || aceita_limite == 0 )
                           fim = 1;
                     }
                     break;

                  case '>' :
                     if ( val_db == limite_db  &&  aceita_limite == 0 )
                        pula = 1;
                     break;
               }

               break;

            case TREE_DATAS   :
            case TREE_HORAS   :

               p = (UTEXT *)&ct_indkey[tam_min];

               res = strncmp((TEXT *)ct_indkey, (TEXT *)chave, tam_min);

               switch ( op_numerico ) {
                  case '=' :
                     if ( res != 0 )
                        fim = 1;
                     break;

                  case '<' :
                     if ( res >= 0 ) {
                        if ( res > 0 || aceita_limite == 0 )
                           fim = 1;
                     }
                     break;

                  case '>' :
                     if ( res == 0  &&  aceita_limite == 0 )
                        pula = 1;
                     break;
               }

               break;


            default           :
               numdebug(673);
               break;
         }

      }

      if ( fim ) break;

      /* para manter performance so testa ESC de vez em quando */
      if ( --contador == 0 ) {
         TEXT *p;
         if ( w_kbhit(curr_win) && fwnqgc(curr_win) == K_ESC ) {
            ret = -1;
            goto fim_gt;
         }
         p="|/-\\|/-\\";
         helice = helice++ & 0x7;
         if ( jm_wait != NULL ) {
            fwat(jm_wait->w_cx-1, jm_wait->w_cy, jm_wait);
            fwpc(*(p+helice), jm_wait);
            fwrefresh(jm_wait);
         }
         contador = PSQ_SLOT / 8;
      }


      if ( ! pula ) {

         vet_lista[lista].tot_ocor++;

         p++;  /* ANANIAS @ tirar qdo tirar 0 NULL da chave */

         to_struct(&si, (UTEXT *)p);

         if ( psq_qualificador == QL_INICIO && si.sequencia != 0 )
            pula = 1;

         if ( !pula && (campo == -1 || si.campo == campo) ) {

            switch(tree) {
               case TREE_PALAVRAS :
                  if ( radical != 0  && push_palavra(PUSH, ct_indkey) < 0 ) {
                     ret = -1;
                     goto sai;
                  }
                  break;

               case TREE_FONETICA :
                  break;

               case TREE_NUMEROS :

                  ind = acha_campo( sele_pesq, si.campo );

                  decimais = vetcmp[ind]->cp_decimais;

                  db_to_str(val_db, (TEXT *)chave, decimais, 1);

                  if ( push_palavra(PUSH, (TEXT *)chave) < 0 ) {
                     ret = -1;
                     goto sai;
                  }
                  break;

               case TREE_DATAS   :
                  if ( push_palavra(PUSH, conv_data((TEXT *)ct_indkey)) < 0 ) {
                     ret = -1;
                     goto sai;
                  }
                  break;

               case TREE_HORAS   :
                  if ( push_palavra(PUSH, conv_hora((TEXT *)ct_indkey)) < 0 ) {
                     ret = -1;
                     goto sai;
                  }
                  break;

               default :
                  numdebug(675);
                  break;
            }


            add_sdi(lista, (SDINDEX *) &si.registro);
         }
      }

      pos = NXTKEY( n_ind, ap_chave_ret);
   }


   if ( uerr_cod != 0 ) {
      mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
      ret = -1;
      goto sai;
   }

   if ( campo == -1 ) {
      vet_lista[lista].tot_campo = 0;
   }


sai :

   if ( radical == 0 ) {
      if ( push_brilha(PUSH, (TEXT *)chave) != 0 )
         ret = -1;
   }


fim_gt :

   switch(ret) {
      case -1 : if ( lista >= 0 )
                   libera_lista(lista);
                break;

      case 0  : if ( vet_lista[lista].tot_vet > 0L ) {
                   add_sdi(lista, &sdi_mor);
                   ret = lista;
                }
                else {
                  libera_lista(lista);
                  ret = 0;
                }
                break;

   }

   if ( ret_trans >= 0 )
      END_TRANS(TR_CARREGAR);

   ord_slot = 0; /* desligando flag de ordenacao de lista */

   win_ajuda = ajuda_ant;
   return(ret);
}



F__GLB   COUNT combina(lista1, lista2, operador)
COUNT lista1, lista2, operador;
{  COUNT lista_res;
   LONG nregs;

   lista_res = 0;

   if ( operador < 0 ) { /* retorna a primeira lista */
      lista_res = lista1;
      goto fim;
   }


   if ( (lista_res = new_lista()) < 0 ) {
      goto fim;
   }


   switch ( operador ) {
      case  E           :  nregs = e(lista1, lista2, lista_res);      break;
      case  OU          :  nregs = ou(lista1, lista2, lista_res, 0);  break;
      case  XOU         :  nregs = ou(lista1, lista2, lista_res, 1);  break;
      case  NOT_OP      :  nregs = not(lista1, lista2, lista_res);    break;


      case  NOCAMPO     :  nregs = nocampo(lista1, lista2, lista_res,1,0,0);
                           break;
      case  NOPARAG     :  nregs = nocampo(lista1, lista2, lista_res,1,1,0);
                           break;
      case  NAFRASE     :  nregs = nocampo(lista1, lista2, lista_res,1,1,1);
                           break;


      case  ADJ         :  nregs = adj_prox(lista1, lista2, lista_res, 1, 1);
                           break;
      case  ADJ1        :
      case  ADJ2        :
      case  ADJ3        :
      case  ADJ4        :
      case  ADJ5        :
      case  ADJ6        :
      case  ADJ7        :
      case  ADJ8        :
      case  ADJ9        :  nregs = adj_prox(lista1, lista2, lista_res,
                                            operador - ADJ1 + 1, 1);
                           break;

      case  PROX        :  nregs = adj_prox(lista1, lista2, lista_res, 1, 0);
                           break;
      case  PROX1       :
      case  PROX2       :
      case  PROX3       :
      case  PROX4       :
      case  PROX5       :
      case  PROX6       :
      case  PROX7       :
      case  PROX8       :
      case  PROX9       :  nregs = adj_prox(lista1, lista2, lista_res,
                                           operador - PROX1 + 1, 0);
                           break;
   }


   if ( nregs > 0 ) {
      vet_lista[lista_res].tot_vet = nregs;
      add_sdi(lista_res, &sdi_mor);
   }
   else {
      libera_lista(lista_res);
      lista_res = 0;
   }

   libera_lista(lista1);
   libera_lista(lista2);

fim :

   /* tava dando erro na pesquisa por tabela na camara em 10/02/94  */
   if ( lista_res > 0 && tipo_pesquisa == 0 ) {
      save_bloco(lista_res);

      /* faz lista_res ficar so no arquivo */
      vet_lista[lista_res].n_buf    = -1;
      vet_lista[lista_res].it_bloco = 0;
      vet_lista[lista_res].it_item  = 0;
   }

   return(lista_res);
}





F__GLB   SI psqcmp_testa(p1, p2)
PFAST SDINDEX *p1,*p2;
{
   if ( p1->registro <= 0L || p2->registro <= 0L ) {
      if ( p1->registro <= 0L && p2->registro <= 0L ) return( 0 );
      else if ( p1->registro <= 0L ) return(1);
      else return(-1);
   }

   if ( p1->registro   <   p2->registro  )   return(-1 );
   if ( p1->registro   >   p2->registro  )   return( 1 );

   if ( p1->campo      <   p2->campo  )   return(-1 );
   if ( p1->campo      >   p2->campo  )   return( 1 );

   if ( p1->paragrafo  <   p2->paragrafo  )   return(-1 );
   if ( p1->paragrafo  >   p2->paragrafo  )   return( 1 );

   if ( p1->frase      <   p2->frase  )   return(-1 );
   if ( p1->frase      >   p2->frase  )   return( 1 );

   if ( p1->sequencia  <   p2->sequencia  )   return(-1 );
   if ( p1->sequencia  >   p2->sequencia  )   return( 1 );

   return(0);
}


F__LOC	 COUNT str_meso(master, proc, tam)
TEXT *master;
TEXT *proc;
COUNT tam;
{  TEXT *p, c_ant;

   c_ant = proc[tam];

   proc[tam] = '\0';

   p = strstr(master, proc);

   proc[tam] = c_ant;

   return((p == NULL) ? 0 : 1 );
}

F__GLB   COUNT push_palavra(flag, argumento)
COUNT flag;
TEXT *argumento;
{  COUNT ret;
   TEXT *ajuda_ant;
	UTEXT str_escape[20];
   WIN *jf=NULL, *jm = NULL;
   extern COUNT br_warning;
   static COUNT qtd_corr;    /* numero de ocorrencias da palavra corrente */
   static COUNT tot_pal;     /* numero de palavras na lista */
   static TEXT pal_corrente[LB_KEYMAX + 1];
   static TEXT rodape[LB_KEYMAX + 11];
   static WIN *jp = NULL;
   static COUNT usou_janela;


   ret = 0;

   /* ANANIAS @@@ mascarar fim de string qdo retirar NULL da final da chave */

   switch ( flag ) {

      case INICIALIZA :

               jp = NULL;

               if ( psq_metacarac ) {
                  jp = abre_janela(J_PSQPAL);
                  if ( jp == NULL ) {
                     ret = -1;
                     goto fim;
                  }
                  fw_title(jp, C_PSQ_WTIT, jp->w_atrib);
	               fwbox(jp);
                  fwbot(jp);
               }

               usou_janela = 0;
               pal_corrente[0] = '\0';

               qtd_corr = 0;
               tot_pal  = 0;
               break;

      case RODAPE :

               strncpy(rodape, argumento, LB_KEYMAX + 10);

               if ( psq_metacarac ) {
                  fw_bottom( jp, rodape );
               }
               break;

      case PUSH :

               if ( psq_metacarac ) {
                  if ( ! usou_janela ) {
                     fwtop(jp);
                     usou_janela = 1;
                  }
               }

               if ( strcmp(pal_corrente, argumento) == 0) { /* nao mudou palavra */
                  qtd_corr++;
                  break;
               }

               tot_pal++;

               if ( qtd_corr > 0 ) { /* para nao passar na primeira diferenca (str_nada) */
                  if ( psq_metacarac ) {
                     fwprintf(jp," %-25.25s   %5.5d\n", pal_corrente, qtd_corr);
                     fwrefresh(jp);
                  }
                  ret = push_brilha(PUSH, pal_corrente);
               }

               strcpy(pal_corrente, argumento);

               qtd_corr = 1;

               break;

      case LBFIM :
               if ( qtd_corr > 0 ) {
                  push_brilha(PUSH, pal_corrente);
               }

               if ( usou_janela ) {

                  if ( qtd_corr > 0 ) {
                     if ( br_warning != 0)
                        fwprintf(jp, "%s", M_TEMMAIS);
                     else
                        fwprintf(jp," %-25.25s   %5.5d\n", pal_corrente, qtd_corr);

                     fwrefresh(jp);
                  }

                  jf = abre_funcoes(funcao("1 3 E N"));
                  jm = abre_mensagem(M_PSQ_WNAV);

                  ajuda_ant = win_ajuda;

                  win_ajuda = "VEPALAV";

      	         pr_escape("13EN", str_escape);
                  ret = w_navega(jm, jp, MIN(tot_pal, VT_BRILHO), (TEXT *)str_escape);

                  if ( ret == K_ESC ) ret = -5;
                  else                ret = 0;

                  fecha_mensagem(jm);
                  fecha_funcoes(jf);
               }

      	      if ( jp != NULL ) fwkill( jp );

               win_ajuda = ajuda_ant;

               break;


      case ABORTA :

      	if ( jp != NULL ) fwkill( jp );

         break;

   }

fim :

   return(ret);
}


#ifdef CP_FLAG
LONG ger_lstfile(fp, oper)
FILE *fp;
COUNT oper;
{  TEXT *work;
   LONG reg;
   COUNT ret;

   work = (TEXT *)&reg;
   work[3] = '\0';


   ret = 1;

   if ( fp != NULL ) {
      switch(oper) {
         case '?' :  fseek(fp, 0L, 2);
                     reg = ftell(fp);
                     reg = reg / 3;
                     break;
         case '0' :  fseek(fp, 0L, 0);
                     ret = fread(work, 3, 1, fp);
                     break;
         case '9' :  fseek(fp, -3L, 2);
                     ret = fread(work, 3, 1, fp);
                     break;
         case 'P' :  ret = fread(work, 3, 1, fp);
                     break;
         case 'A' :  if ( ftell(fp) < 6 )
                        ret = 0;
                     else {
                        fseek(fp, -6L, 1);
                        ret = fread(work, 3, 1, fp);
                     }
                     break;
      }

      if ( ret < 1 )
         reg = 0L;
   }


   return(reg);
}


F__LOC   FILE *opfile_flags(nome)
TEXT *nome;
{  FILE *fp;
   TEXT arq[100], campo[14];

   strcpy(campo, nome);
   campo[8] = '\0';



   if ( info_base( campo, WBS->bs_path, ".lbi", arq) <= 0 ) {
      fp = NULL;
      goto fim;
   }
   else {
#ifdef MSDOS
      fp = fopen(arq, "rb");
#else
      fp = fopen(arq, "r");
#endif
   }

fim:

   return ( fp );
}
#endif






#ifdef CP_FLAG
#ifdef MSDOS
   VOID clear_campos( COUNT );
   VOID marca_campos( VOID );
   COUNT le_pesquisa(COUNT, TEXT *, WIN *);
#else
   VOID clear_campos(  );
   VOID marca_campos(  );
   COUNT le_pesquisa( );
#endif


F__GLB   COUNT pesquisa(sele)
COUNT sele;
{  COUNT ret = 0, lista_ret;
   WIN *jf, *jm, *ji;
   TEXT *ajuda_ant = win_ajuda, *save_pesq=NULL;
   WIN *win=NULL;
#ifdef MSDOS
   unsigned int di;
#endif

   up_str(GRP_PESQ);

   fp_psq = NULL;

   sele_pesq = sele;
   free_record(sele_pesq);

   jf = jm = ji = NULL;

   vet_wkbuf[0].wb_vetor = NULL;
   vet_wkbuf[1].wb_vetor = NULL;
   vet_wkbuf[2].wb_vetor = NULL;

   vet_mapa  = NULL;
   vet_lista = NULL;

   /* A para manuseio de grupos de PSQ_SLOT elementos de SDINDEX */
   /* Aloca TAM_MAPA estruturas para mapear listas em disco */
   /* Aloca NUM_LISTAS estruturas para controlar registros (SDINDEX) pertencentes a uma lista */

   if ( aloc_work() < 0 || aloc_mapa() || aloc_lista() || (aloc_brilha('A') != 0)) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   ret = 0;

   jm = abre_mensagem(str_nada);
   jf = abre_funcoes(str_nada);
   ji = abre_insercol(C_PSQ_ICOL);

   /* retorno de erro de acesso a disco */

#ifdef MSDOS
   di = _DI;  /* longjmp nao restaura di em TURBOC ++ */
#endif

   ret = setjmp(psq_jmperro);

#ifdef MSDOS
   _DI = di;  /* longjmp nao restaura di em TURBOC ++ */
#endif


   if ( ret != 0 ) {
      mens_erro(H_PSQLST, E_PSQ_LST, ret);
      goto fim;
   }

   buf_iotxt[0] = '\0';

   save_pesq = malloc(c_bases[sele]->cb_tamrec);

   if ( ( win = abre_janela(J_REGDISP)) == NULL ) {
      ret = -1;
      goto fim;
   }

   clear_campos(sele);

   while ( ret == 0 ) {

      lista_ret = 0;

      funcoes(jf, funcao(C_PSQ_1ROD));

      ret = le_pesquisa(sele, buf_iotxt, win);

      memcpy(save_pesq, c_bases[sele]->cb_wkarea, c_bases[sele]->cb_tamrec);

      if ( ret < 0 ) goto fim;

      ret = exec_tabela( ret, buf_iotxt );


      if ( ret > 0 )
         lista_ret = ret;

      mens_1(jm, str_vazia);

      if ( ret > 0 && vet_lista[ret].tot_doc > 0L) {
         ret = visualiza( ret, 0);
      }
      else {
         if ( ret != -5 ) /* ESC na navegacao na janela */
            mens_erro(H_PSQNOT, M_PSQ_NOT);
         ret = 0;
      }

      memcpy(c_bases[sele]->cb_wkarea, save_pesq, c_bases[sele]->cb_tamrec);

      marca_campos();

      if ( lista_ret )
         libera_lista(lista_ret);
   }


fim :

   if ( win != NULL )
      fwkill( win );
   if ( save_pesq != NULL )
      free(save_pesq);

   if ( fp_psq != NULL ) { /* mata arquivo se foi criado */
      psq_fileger(MATA);
   }

   fecha_insercol(ji);
   fecha_funcoes(jf);
   fecha_mensagem(jm);

   win_ajuda = ajuda_ant;

   aloc_brilha('F');

   if ( vet_lista != NULL ) free((TEXT *)vet_lista);
   if ( vet_mapa  != NULL ) free((TEXT *)vet_mapa );

   if ( vet_wkbuf[2].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[2].wb_vetor);
   if ( vet_wkbuf[1].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[1].wb_vetor);
   if ( vet_wkbuf[0].wb_vetor != NULL ) free( (TEXT *)vet_wkbuf[0].wb_vetor);

   down_str(GRP_PESQ);
   return(ret);
}
#endif


VOID abre_psqlog( )
{  TEXT *p;
	COUNT fd;


   /* caso o usuario queira um log de pesquisa deve setar a variavel global */
   /* LOGPQS com o nome do arquivo de LOG de pesquisa */
   if ( ( p = getenv("LOGPESQ")) == NULL )
      return;

   if ( access(p, 0) != 0 ) { /* NAO existe arquivo de log de pesquisa */
#ifdef MSDOS
	   fd = open(p, (O_CREAT | O_TRUNC | O_RDWR),
                     (S_IREAD | S_IWRITE));
#else
      fd = creat(p, 0666);
#endif

   }
   else {

#ifdef MSDOS
   	fd = open(p, (O_RDWR | O_DENYNONE) , (S_IREAD | S_IWRITE));
#else
   	fd = open(p, O_RDWR);
#endif
   }

   if ( fd >= 0 ) {
      fd_logpsq = fd;
      return;
   }
   else {
      mens_erro( H_LOGFAULT, E_LOGFAULT);
      return;
   }

}


VOID grava_psqlog(str)
TEXT *str;
{  COUNT gravou, ret, lock_ok;
   TEXT aux[300], aux2[81], data[10], hora[6];


   if ( fd_logpsq < 0 )
      return;

   if ( strlen(str) == 0 )
      return;

   strcpy(data, data_hora('D', 0L));
   strcpy(hora, data_hora('H', 0L));

   sprintf( aux, "%s %s %-8.8s %-15.15s ",
         data, hora, WBS->bs_nome, ap_nome);
#ifndef RUNTIME
   /* caso tenha um numero em automatico. Imprime o mesmo. FHC 08/94 */
   if ( getenv("CROSSLOG") != NULL) {
      if ( auto_last[0] != '\0' ) {
         sprintf(aux2, "<%s> ", auto_last);
         strcat(aux, aux2);
      }
   }
#endif

   strcat( aux, str);
   strcat( aux, "\n");


   lseek(fd_logpsq, 0L, 0);
   lock_ok = -1;
#ifdef MSDOS
   lock_ok = lock(fd_logpsq, 0L, TR_TAM);
#else
#ifndef VAXVMS
   lock_ok = lockf(fd_logpsq, F_LOCK, 0L);
#endif
#endif

   if ( lock_ok != 0 ) {
      debug(" WWW erro em unlock" );
   }


   lseek(fd_logpsq, 0L, 2);
   gravou = write(fd_logpsq, (TEXT *)aux, strlen(aux));

   if ( gravou < 1 )
       debug(" WWW erro na gravacao de log" );

   lseek(fd_logpsq, 0L, 0);

#ifdef MSDOS
      ret = unlock(fd_logpsq, 0L, TR_TAM);
#else
#ifndef VAXVMS
      ret = lockf(fd_logpsq, F_ULOCK, 0L);
#endif
#endif

   if ( ret != 0 ) {
      debug(" WWW erro em unlock" );
   }
}

