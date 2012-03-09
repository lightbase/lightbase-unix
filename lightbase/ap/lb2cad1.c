#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>
#include  <time.h>
#include  HD_MEMO
#include  HD_DIR
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"


#define   GRP_OPCLS  18




#define   H_NOMOR    "NOMORBAS"   /* nao pode abrir mais bases */
#define   H_FTARQ    "FALTAARQ"   /* Falta arquivo um dos arquivos da Base */
#define   H_OPEN     "OPENARQ"    /* Nao conseguiu abrir arquivo */
#define   H_NOSUP    "UPGRADE"    /* base gerada em versao mais recente */
#define   H_OPBA3    "OUTRAVER"   /* base gerada em outra versao */
#define   H_OPBA7    "CFGFILES"   /* numero de arquivos em config.sys */
#define   H_BSSTAT   "BSSTATFL"   /* flag bs_status setada */
#define   H_CLSBS    "CLSARQBS"   /* erro em close no arquivo da base */
#define   H_LDSTP    "ERRLDSW"   /* Erro na carga de stop word para edicao */
#define   H_MUDDEC   "MUDADEC"   /* Mudanca de decimais da problemas */

#define   M_OPBA1    loc_str(GRP_OPCLS,  1)
#define   M_CLBA1    loc_str(GRP_OPCLS,  2)

#define   E_NOMOR    loc_str(GRP_OPCLS,  3)

#define   E_NOSUP    loc_str(GRP_OPCLS,  4)

#define   E_FTARQ    loc_str(GRP_OPCLS,  5)
#define   E_OPBA3    loc_str(GRP_OPCLS,  6)

#define   TW_HD10    loc_str(GRP_OPCLS,  7)
#define   TW_HD11    loc_str(GRP_OPCLS,  8)
#define   TW_HD12    loc_str(GRP_OPCLS,  9)


#define   E_OPBA7    loc_str(GRP_OPCLS, 501)
#define   E_CLSBAS   loc_str(GRP_OPCLS, 502)


#define   E_LDSTP    loc_str(GRP_OPCLS, 505)
#define   E_MUDDEC   loc_str(GRP_OPCLS, 506)

#define   E_AUTONUM1   loc_str(GRP_OPCLS, 516)
#define   E_AUTONUM2   loc_str(GRP_OPCLS, 517)


#ifdef MSDOS
   COUNT aloc_area( COUNT );
   VOID  lib_area(COUNT);
   COUNT load_stop(VOID);
   COUNT mont_stp(ARG*);
   VOID  stp_prep( S_DELIMIT *, SC *( * )[ ], SC *, SC *, SC * );
   COUNT stp_area(S_INDEX *, TEXT *);
   COUNT stp_seta(S_INDEX *);
   VOID  monta_cripto( Base * );
   SI    bqsw_comp(TEXT **, TEXT **);
   COUNT load_especial(TEXT *);
   VOID rlse_especial(VOID);
   COUNT trata_res(COUNT *, TEXT **, TEXT *);
   COUNT trata_ap(COUNT *, TEXT **, TEXT *);
   COUNT trata_det(COUNT *, TEXT **, TEXT *);
#else
   COUNT aloc_area( );
   VOID  lib_area( );
   COUNT load_stop( );
   COUNT mont_stp( );
   VOID  stp_prep( );
   COUNT stp_area( );
   COUNT stp_seta( );
   VOID  monta_cripto( );
   SI    bqsw_comp( );
   COUNT load_especial( );
   VOID rlse_especial( );
   COUNT trata_res( );
   COUNT trata_ap( );
   COUNT trata_det( );
#endif


#ifdef AUTO_INCREMENTO
#ifdef MSDOS
   VOID auto_close( VOID );
   COUNT auto_init( COUNT, COUNT, TEXT *, TEXT *);
#else
   VOID auto_close( );
   COUNT auto_init( );
#endif
#endif

static  SS        num_stp = 0;
static  SS        area_stp = 0;



/*
**#  ---------------------------------------------------------------------
**#   Funcao...: open_base
**#   Objetivo.: Abre os arquivos de uma base de dados
**#   Entrada..: nome_base - nome da base a ser aberta
**#              flag      - funcao de utilizacaco
**#                          . UTILIZACAO
**#                          . INFORMACAO
**#                          . RELACIONAMENTO
**#                          . REPROCessamento
**#                          . REINDEX
**#   Retorna..:  0  - se abriu OK
**#               -1 - Erro na abertura dos arquivos da base
**#
**#
**/
F__GLB   COUNT open_base( nome_base, flag)
TEXT *nome_base;
COUNT flag;
{  COUNT i, ret, modo, n_sele, narq, oper_lck;
   LONG serie_open;
   Base      *wbs;
   Desc_base *wcb;
   WIN *jm = NULL;
	TEXT *pnx, nx[PATH_MAX+10]; /* 1-Controle  2-Dados   3-Indice  */

   ret = -1;
   wbs = NULL;
   wcb = NULL;

   up_str(GRP_OPCLS);

   /* acha localizador de base disponivel */
   for ( n_sele = -1,  i = 0 ; i < MAX_SELE; i++) {
      if ( v_bases[i] == NULL ) {
         n_sele = i;
         break;
      }
   }

   if ( n_sele == -1 ) {
      mens_erro(H_NOMOR, E_NOMOR);
      goto sai;
   }

   /* aloca area para descritor da base  */
   if ( ( wbs = (Base *) calloc(1, sizeof(Base))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto sai;
   }

   /* aloca area para controle de utilizacao da base  */
   if ( ( wcb = (Desc_base *) calloc(1, sizeof(Desc_base))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      free(wbs);
      goto sai;
   }


	if ( ! load_rec( ace_dnum, ace_inum, D0_ID, nome_base,
                     wbs, sizeof(Base), '=') ) {
      mens_erro(H_BADAC, E_BADAC);
      goto fim;
   }



   wcb->cb_fordem   = n_sele + 1;
   wcb->cb_prfile   = INI_SELE(n_sele);
   wcb->cb_tpopen   = flag;


   wcb->cb_lockatu  = 0;


   v_bases[n_sele] = wbs;
   c_bases[n_sele] = wcb;

   if ( wbs->bs_finger != 0 ) {
      if ( base_liberada(wbs) != 'S' ) {
         ret = -1;
         goto fim;
      }
   }

   if ( ap_max_users == 0 || flag == REPROC || flag == REINDEX ){
      oper_lck = LCK_EXCLUSIVO;
      modo = VIRTUAL | EXCLUSIVE;
   }
   else {
      oper_lck = LCK_COMPARTILHADO;
      modo = VIRTUAL | SHARED;
   }

   if ( LOCK_RECURSO(nome_base, wbs->bs_path, oper_lck) != 0 ) {
      COUNT x;
      /* operacao negada */
      if ( oper_lck == LCK_EXCLUSIVO )
         x = mens_erro(H_NOLOCK, E_NOLOCK);
      else
         x = mens_erro(H_LOCKED, E_LOCKED);

      if ( x == '?' )
         debug("WWW nome_base = [%s]  path=[%s]", nome_base, wbs->bs_path);

      ret = -1;
		goto fim;
   }


   wcb->cb_lockatu  = oper_lck;

   jm = abre_mensagem( M_OPBA1 );

   if ( info_base( wbs->bs_nome, wbs->bs_path, LB_4_EXT, nx) <= 0 ) {
      mens_erro(H_FTARQ, E_FTARQ, nx);
      goto fim;
   }

   if ( access(nx, 6) != 0 ) {
      mens_erro(H_PERMAC, E_PERMAC, nx);
      goto fim;
   }

   serie_open = wbs->bs_nserie; /* antes era ap_nserie */

   if ( open_lb4(wbs, nx, flag, serie_open, wbs->bs_idbase ) != 0 ) {
      ret = -1;
      goto fim;
   }

   /* se open para REBUILD deve abrir arquivos com nome especial */
   info_base( (flag != REPROC) ? wbs->bs_nome : "@@@",
               wbs->bs_path, LB_1_EXT, nx);


   if ( flag != REPROC && flag != INFORMACAO ) {
      if ( wbs->bs_status != 0 ) {
         /* necessidade de reprocessar */
         decida( M_ATENCAO, str_nada, H_BSSTAT,
                  3, 42, "%s\n%s\n%s", TW_HD10, TW_HD11, TW_HD12);
         ret = -1;
         goto fim;
      }

      if ( (wbs->bs_versao / 10) != (VERSAO_CORRENTE / 10)) {
         if ( (wbs->bs_versao / 10) > (VERSAO_CORRENTE / 10))
            mens_erro(H_NOSUP, E_NOSUP);
         else
            mens_erro(H_OPBA3, E_OPBA3);
         goto fim;
      };

      if ( wbs->bs_sepdecimal != sep_dec ) {
         mens_erro(H_MUDDEC, E_MUDDEC);
         ret = -1;
         goto fim;
      };

   }


   /* faz pnx apontar para numero do arquivo LB? */
   pnx = strchr(nx, '\0');
   pnx--;

   for ( i = 1; i<=3 ; i++ ) {
      narq = wcb->cb_prfile + i - 1;
      *pnx = '0' + i;  /* Monta nome do arquivo */
      if ( access(nx, 0) == 0 ) {
         if ( access(nx, 6) != 0 ) {
            mens_erro(H_PERMAC, E_PERMAC, nx);
            goto fim;
         }

	      if ( file_open(narq, nx, modo, serie_open, wbs->bs_idbase ) != 0 )
            goto fim;

         if ( wbs->bs_finger != 0 ) {
            if ( ct_finger != wbs->bs_finger || ct_release != wbs->bs_release){
               mens_erro(H_CFFINGER, E_CFFINGER);
               goto fim;
             }
         }
      }
      else {
         mens_erro(H_FTARQ, E_FTARQ, nx);
         goto fim;
      }
   }

#ifdef VISUAL
   pc_visual = NULL;
#endif

   if ( (v_campos[n_sele] = load_campos(n_sele, flag)) == NULL ) {
      ret = -1;
      goto fim;
   };

   /* carrega definicoes especiais da base */
   *pnx = 'x';
   if ( load_especial( nx ) != 0 ) {
      ret = -1;
      goto fim;
   }

   /* carrega tabelas de consulta a formulas */
   if ( ld_matriz(n_sele) != 0 ) {
      ret = -1;
      goto fim;
   }

   /* carrega primeira vista disponivel para o usuario */

   if (  flag == UTILIZACAO ) {
      ret = open_vista(n_sele, '?', flag);
      if ( ret != 0  ) {
         /* ret == -2 significa vista ainda nao definida */
         ret = -1;
         goto fim;
      }
   }

   /**
   ***   Monta semente de criptografar base
   **/
   if ( wbs->bs_idbase > 0 )
      monta_cripto(wbs);
   else
      memset(ap_cripto, '\0', sizeof(ap_cripto));


   /**
   ***   ALOCACAO DE AREAS PARA REGISTROS ORIGINAL/ALTERADO
   **/

   if ( aloc_area(n_sele) != 0 ) {
      ret = -1;
      goto fim;
   }

   /**
   ***   MONTA VETOR DE STOP WORDS
   **/

   if ( load_stop() < 0 ) {
      ret = -1;
      goto fim;
   }

   /**
   ***   ALOCA AREA PARA TABELA DE PESQUISA
   **/

   if ( flag == UTILIZACAO && tabpsq_inic(0) < 0 ) {
      ret = -1;
      goto fim;
   }

#ifdef AINDA_NAO
   if ( flag == UTILIZACAO ) {
      /* abre base relacionada para RELACIONAMENTO */
	   if ( open_base(BASE_RELACIONADA, RELACIONAMENTO) < 0 ) {
         ret = -1;
         goto fim;
      }
   }
#endif

   /**
   ***   Verifica se podera abrir mais dois descritores de arquivos
   **/

   if ( flag == CRIACAO || flag == UTILIZACAO ) {
      COUNT d1, d2;

      d1 = dup(0);
      d2 = dup(0);

      if ( d1 >= 0 ) close(d1);
      if ( d2 >= 0 ) close(d2);

      if ( d1 < 0  || d2 < 0 ) {
         mens_erro("E_OPBA7", E_OPBA7);
         ret = -1;
         goto fim;
      }
   }

#ifndef RUNTIME
#ifdef AUTO_INCREMENTO
   /*  verificar se tem campo AutoIncrementado definido  */
   /*  Se sim, prepara parametros para executar */
	for( ret = 0; ret < qtd_vetcmp(n_sele); ++ret ) {  /* anda pelos campos */
		if( ((Campo *)v_campos[n_sele]->vi_vtit[ret])->cp_critica == 'A' ) {
			ret = auto_init( n_sele, ret, nome_base, wbs->bs_path );
			if ( ret != 0 ) {
				debug( "%s\n%s", E_AUTONUM1, E_AUTONUM2);
			}
			break;
		}
	}
#endif
#endif

   ret = 0;

fim :

   close_lb4( );

   if ( ret < 0 ) {
      close_base( );
   };

   fecha_mensagem(jm);

sai :

   ap_curflushfile = 0;

   down_str(GRP_OPCLS);
   return(ret);
}



/*
**#  ---------------------------------------------------------------------
**#   Funcao...: close_base( )
**#   Objetivo.: fecha base ativa
**#
**/
COUNT close_base( )
{  COUNT sele, i, ret = 0, flag, narq;
   Base      *wbs;
   Desc_base *wcb;
   WIN *jm = NULL;

   up_str(GRP_OPCLS);

   for (sele = MAX_SELE - 1; sele >=0; sele--) {

         wbs = v_bases[sele];
         wcb = c_bases[sele];

         if ( wbs == NULL || wcb == NULL )
            continue;

         flag = wcb->cb_tpopen;

         if ( flag != UTILIZACAO     && flag != INFORMACAO &&
              flag != RELACIONAMENTO && flag != REPROC &&
              flag != REINDEX ) {
            numdebug(3);
            fim_lb("3");
         }


         if ( wcb->cb_numrec != 0L ) /* se por algum motivo existe reg_ativo, libera */
            free_record(sele);

         jm = abre_mensagem( M_CLBA1 );

         /***   libera area alocada para tabela de pesquisa ***/

         tabpsq_inic(1); /* so libera area */


         /***   libera area alocada para stop words  ***/

         if ( wcb->cb_palstops != NULL )
            free((TEXT *)wcb->cb_palstops);
         if ( wcb->cb_vetstops != NULL )
            free((TEXT *)wcb->cb_vetstops);

         /******   restaura delimitadores eventuais para padrao ***/

         plv_delimita[5] = ' ';
         plv_delimita[6] = ' ';

         tudo_delimita[6] = ' ';
         tudo_delimita[7] = ' ';

         /***   libera area alocada para os registros da base ***/
         lib_area(sele);


         /* libera tabelas de consulta a formulas */
         if ( flag != CRIACAO )
            lib_matriz();

         if ( esp_base.presente == 1 ) {
            rlse_especial();
         }


         if ( pos_vetcmp(sele) != NULL ) {
            /***   libera area alocada para vista   ***/
            if ( flag == UTILIZACAO ) {
               close_vista();
            }

            rlse_campos(sele);
         };


         for ( i = 1 ; i <= 3 ; i++ ) {
            narq = wcb->cb_prfile + i - 1;
            if ( CLSFIL(narq, COMPLETE) != 0)
               ret = -1;
         };

         if ( v_campos[sele] != NULL && ret == -1 ) /* so da mensagem se esta aberta ok */
            mens_erro(H_CLSBS, E_CLS_SO, uerr_cod, "LB?");

         if ( wcb->cb_lockatu != 0  ) {
            LOCK_RECURSO(wbs->bs_nome, wbs->bs_path, LCK_LIBERA);
         }

         free(wcb);
         free(wbs);
         v_bases[sele] = NULL;
         c_bases[sele] = NULL;
   }

 	if ( jm != NULL )
      fecha_mensagem(jm);

#ifndef RUNTIME
#ifdef AUTO_INCREMENTO
   /* fecha campo autoincrementado */
   auto_close( );
#endif
#endif


   down_str(GRP_OPCLS);
   return(ret);
}




/*
**#  ---------------------------------------------------------------------
**#   Funcao...: aloc_area(sele)
**#   Objetivo.: alocar area para leitura e area de edicao de registro
**#
**/
F__LOC   COUNT aloc_area(sele)
COUNT sele;
{  COUNT ret, i;
   COUNT num_cps;
   Campo **vet_cmp;
   UCOUNT tam_rec;
   TEXT *ioarea, *wkarea, *p;

   ioarea = wkarea = NULL;

   vet_cmp = pos_vetcmp(sele);
   num_cps = qtd_vetcmp(sele);

   c_bases[sele]->cb_tamrec = 0;

   /* calcula area total para alocacao de memoria para edicao */
   /* dois bytes iniciais do registro indicando o numero */
   /* de campos existentes no registro + # do registro */

   tam_rec = HDR_REGISTRO;

   for ( i = 0; i < num_cps; i++ ) {

      if ( vet_cmp[i]->cp_tipo == CP_TEXTO ) {
         tam_rec += sizeof(POINTER);
      }
      else {
         tam_rec += (vet_cmp[i]->cp_mhor + 1 );   /* +1 para null */
      }

      /* #campo + #repeticao + tamanho do campo (efetivo) */
      tam_rec += (OFF_CMP + OFF_TAM);
   };


   ioarea = calloc(1 , tam_rec);

   if ( ioarea == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   wkarea = calloc(1 , tam_rec);

   if ( wkarea == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      free((TEXT *)ioarea);
      ioarea = NULL;
      ret = -1;
      goto fim;
   }

   c_bases[sele]->cb_ioarea = ioarea;
   c_bases[sele]->cb_wkarea = wkarea;
   c_bases[sele]->cb_tamrec = tam_rec;

   for ( p = wkarea, i = 0; i < num_cps; i++ ) {

      vet_cmp[i]->cp_newdata = p;
      vet_cmp[i]->cp_newtam  = 0;

      if ( vet_cmp[i]->cp_tipo == CP_TEXTO )
         p += sizeof(POINTER);
      else
         p += (vet_cmp[i]->cp_mhor + 1 );   /* +1 para null */
   };

   ret = 0;

fim :

   return( ret );
}

/*
**#  ---------------------------------------------------------------------
**#   Funcao...: lib_area(sele)
**#   Objetivo.: liberar area alocada para leitura/edicao de registro
**#
**/
F__LOC   VOID lib_area(sele)
COUNT sele;
{
   if ( c_bases[sele]->cb_wkarea != NULL ) {
      free((TEXT *)c_bases[sele]->cb_wkarea);
      c_bases[sele]->cb_wkarea = NULL;
   }

   if ( c_bases[sele]->cb_ioarea != NULL ) {
      free((TEXT *)c_bases[sele]->cb_ioarea);
      c_bases[sele]->cb_ioarea = NULL;
   }
}








F__LOC   COUNT load_stop()
{
   COUNT    ret=0;
   ARG *adoc;
   LONG     tam;
	POINTER  pos;

   ret = 0;
   adoc = NULL;

   pos = WBS->bs_swpos;

   adoc = txt_load(lb4_dnum, pos, &tam , 100, 0L);

   if ( adoc == NULL ) {
      mens_erro(H_LDSTP, E_LDSTP);
      ret = -1;
      goto fim;
   }

   if ( mont_stp(adoc) < 0 ) {
      ret = -1;
      goto fim;
   }

   ret = 0;

fim :

   if ( adoc != NULL ) {
      free_buffer( adoc );
      adoc = NULL;
   }

   return (ret);
}




F__LOC   COUNT mont_stp(adoc)
ARG *adoc;
{  COUNT ret=0;

   TEXT *nul_stpw[1];
   S_DELIMIT sd;
   S_INDEX   si;



   nul_stpw[0] = NULL;
   num_stp = 0;
   area_stp = 0;


   stp_prep( &sd, &nul_stpw, (SC*)tudo_delimita, str_nada, str_nada);
   prep_indice( &si, 0, 0, 0, 0, 0);

   /* inicialmente contabiliza espaco para vetor + palavras */
   if ( lb_palavra( adoc, 0, &si, &sd, stp_area ) != 0 ) {
      ret = -1;
      numdebug(370);
      goto fim;
   }


   c_bases[BASE_EDIT]->cb_vetstops = calloc( num_stp + 1 , sizeof(TEXT *));
   if ( c_bases[BASE_EDIT]->cb_vetstops  == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   c_bases[BASE_EDIT]->cb_palstops = calloc( area_stp + 1, sizeof(TEXT *));

   if ( c_bases[BASE_EDIT]->cb_palstops == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      free((TEXT *)c_bases[BASE_EDIT]->cb_vetstops);
      c_bases[BASE_EDIT]->cb_vetstops = NULL;
      ret = -1;
      goto fim;
   }

   c_bases[BASE_EDIT]->cb_vetstops[num_stp] = NULL;

   num_stp = 0;

   /* depois coloca as palavras nos devidos lugares */
   if ( lb_palavra( adoc, 0, &si, &sd, stp_seta ) != 0 ) {
      ret = -1;
      numdebug(371);
      goto fim;
   }

   stp_prep( &ap_icomun, (TEXT * (*)[] )c_bases[BASE_EDIT]->cb_vetstops,
               (SC*)tudo_delimita, str_nada, str_nada);
   stp_prep( &ap_itexto, (TEXT * (*)[] )c_bases[BASE_EDIT]->cb_vetstops,
               (SC*)plv_delimita, (SC*)fra_delimita, (SC*)prgf_delimita );

fim :


   return(ret);
}



F__LOC   VOID stp_prep( D, stp, plv_delimita, fra_delimita, prgf_delimita )

S_DELIMIT   *D;
SC          *( *stp )[ ];
SC          *plv_delimita;
SC          *fra_delimita;
SC          *prgf_delimita;
{
   US   I;

   for ( I = 0;   ( *stp )[ I ] != NULL;   I++  )
   {
      prep_palavra( ( *stp )[ I ] );
   }

   qsort( (SC *) *stp, I, sizeof( SC * ), (SI (* )()) bqsw_comp );

   D->stop_words = stp;
   D->q_stop     = I;
   D->palavra    = plv_delimita;
   D->frase      = fra_delimita;
   D->paragrafo  = prgf_delimita;
}



F__LOC   COUNT stp_area( si, lixo )
S_INDEX *si;
TEXT *lixo;
{
   lixo=lixo;  /* tirar warning */

   if ( si->palavra[0] != '@' ) {
      num_stp++;
      area_stp += (strlen(si->palavra) + 1 );
   }
   else {
         /******   seta delimitadores eventuais ***/
         UTEXT c1, c2;

         c1 = si->palavra[1];
         c2 = si->palavra[2];

         if ( c1 != '\0' ) {
            plv_delimita[5] = c1;
            tudo_delimita[6] = c1;
         }

         if ( c2 != '\0' ) {
            plv_delimita[6] = c2;
            tudo_delimita[7] = c2;
         }
   }


   return(0);
}

F__LOC   COUNT stp_seta( si )
S_INDEX *si;
{  static TEXT *pd;

   if ( num_stp == 0 ) pd = c_bases[BASE_EDIT]->cb_palstops;

   c_bases[BASE_EDIT]->cb_vetstops[num_stp] = pd;

   strcpy( pd, si->palavra);

   pd += (strlen( si->palavra ) + 1 );

   num_stp++;

   return(0);
}

F__LOC   VOID  monta_cripto( wbs )
Base *wbs;
{
   UCOUNT B = wbs->bs_idbase;
   UCOUNT C = wbs->bs_finger;
   LONG   D = wbs->bs_cr_data;


   /* se nao for base comercial usa B ao invez de C */
   if ( C == 0 )
      C = B;

   /***********************************************
      Esquema de montagem de semente de critografia
      bs_idbase  = B1  B2
      bs_finger  = C1  C2
      bs_cr_data = D1  D2  D3  D4
      =============================================
      B1 C1 C2 D1 D2 D3 B2 D4 B2 C2
   ************************************************/

   ap_cripto[0] = (B & 0xFF00) >> 8;
   ap_cripto[1] = (C & 0xFF00) >> 8;
   ap_cripto[2] = (C & 0xFF);
   ap_cripto[3] = (D & 0xFF000000L) >> 24;
   ap_cripto[4] = (D & 0x00FF0000L) >> 16;
   ap_cripto[5] = (D & 0x0000FF00L) >> 8;
   ap_cripto[6] = (B & 0xFF);
   ap_cripto[7] = (D & 0xFF);
   ap_cripto[8] = (B & 0xFF);
   ap_cripto[9] = (C & 0xFF);
}


F__LOC   SI  bqsw_comp( S1,  S2 )
SC    **S1, **S2;
{
   return( strcmp( *S1, *S2 ) );
}

COUNT load_especial(arq)
TEXT *arq;
{  COUNT ret = -1, i;
   FILE *fp;
   TEXT buf[200];
   
   esp_base.presente = 0;
   esp_base.tecla_limpa = 0;
   esp_base.campos_limpa = NULL;
   esp_base.vista_inclusao = -1;
   esp_base.vista_aposgravar = -1;
   esp_base.vista_altf6 = -1;
   esp_base.campos_altf6 = NULL;

   esp_base.nlin_ap = 0;
   esp_base.nlin_det = 0;
   for (i = 0; i < 10; i++)
      esp_base.linhas_ap[i] = esp_base.linhas_det[i] = NULL;

   rst_base.ativada = 0;
   rst_base.UD = -1;
   rst_base.GD = -1;
   rst_base.UR = -1;
   rst_base.GR = -1;
   rst_base.UDcampos = NULL;
   rst_base.GDcampos = NULL;
   rst_base.URcampos = NULL;
   rst_base.GRcampos = NULL;

   fp = NULL;

   if ( access(arq, 0) != 0 ) {
      ret = 0;
      goto fim;
   }

   if ( (fp = fopen( arq,  "r" )) == NULL ) {
      goto fim;
   }

   esp_base.presente = 1 ;
   rst_base.hierarquico = 0;

   while ( fgets (buf, sizeof(buf), fp) != NULL ) {
      strip_nl(buf);
      if ( strncmp(buf, "HIER", 4) == 0 ) {
         rst_base.hierarquico = 1;
      }
      else if ( strncmp(buf, "TL=", 3) == 0 ) {
         esp_base.tecla_limpa = atoi(&buf[3]);
      }
      else if ( strncmp(buf, "VI=", 3) == 0 ) {
         esp_base.vista_inclusao = atoi(&buf[3]);
      }
      else if ( strncmp(buf, "VA=", 3) == 0 ) {
         esp_base.vista_aposgravar = atoi(&buf[3]);
      }
      else if ( strncmp(buf, "V6=", 3) == 0 ) {
         esp_base.vista_altf6 = atoi(&buf[3]);
      }
      else if ( strncmp(buf, "C6=", 3) == 0 ) {
         esp_base.campos_altf6 = malloc(strlen(&buf[3]) + 1);
         strcpy(esp_base.campos_altf6, &buf[3]);
      }
      else if ( strncmp(buf, "CL=", 3) == 0 ) {
         esp_base.campos_limpa = malloc(strlen(&buf[3]) + 1);
         strcpy(esp_base.campos_limpa, &buf[3]);
      }
      else if ( strncmp(buf, "AP=", 3) == 0 ) {
         if ( trata_ap(&esp_base.nlin_ap, esp_base.linhas_ap, &buf[3]) != 0 )
            goto fim;
      }
      else if ( strncmp(buf, "DT", 2) == 0 ) {
         if ( trata_det(&esp_base.nlin_det, esp_base.linhas_det, &buf[2]) != 0 )
            goto fim;
      }
      else if ( strncmp(buf, "UD=", 3) == 0 ) {
         if ( trata_res(&rst_base.UD, &rst_base.UDcampos, &buf[3]) != 0 )
            goto fim;
      }
      if ( strncmp(buf, "GD=", 3) == 0 ) {
         if ( trata_res(&rst_base.GD, &rst_base.GDcampos, &buf[3]) != 0 )
            goto fim;
      }
      if ( strncmp(buf, "UR=", 3) == 0 ) {
         if ( trata_res(&rst_base.UR, &rst_base.URcampos, &buf[3]) != 0 )
            goto fim;
      }
      if ( strncmp(buf, "GR=", 3) == 0 ) {
         if ( trata_res(&rst_base.GR, &rst_base.GRcampos, &buf[3]) != 0 )
            goto fim;
      }

   }

   ret = 0;

fim :

   if ( ret != 0 ) {
      if ( rst_base.UDcampos != NULL ) free (rst_base.UDcampos );
      if ( rst_base.GDcampos != NULL ) free (rst_base.GDcampos );
      if ( rst_base.URcampos != NULL ) free (rst_base.URcampos );
      if ( rst_base.GRcampos != NULL ) free (rst_base.GRcampos );
      rst_base.UDcampos = NULL; rst_base.UD = -1;
      rst_base.GDcampos = NULL; rst_base.GD = -1;
      rst_base.URcampos = NULL; rst_base.UR = -1;
      rst_base.GRcampos = NULL; rst_base.GR = -1;
      rst_base.ativada = 0;
   }

   if ( fp != NULL )
      fclose(fp);

#ifdef BEDUGANDO
   if ( rst_base.UD != -1 ) debug("WWW UD [%d] - [%s]", rst_base.UD , rst_base.UDcampos );
   if ( rst_base.GD != -1 ) debug("WWW GD [%d] - [%s]", rst_base.GD , rst_base.GDcampos );
   if ( rst_base.UR != -1 ) debug("WWW UR [%d] - [%s]", rst_base.UR , rst_base.URcampos );
   if ( rst_base.GR != -1 ) debug("WWW GR [%d] - [%s]", rst_base.GR , rst_base.GRcampos );

      for (i = 0; i < 10; i++) {
         if ( esp_base.linhas_ap[i] != NULL ) {
            debug("WWW linhas_ap[%d] = [%s]", i, esp_base.linhas_ap[i]);
         }
         if ( esp_base.linhas_det[i] != NULL ) {
            debug("WWW linhas_det[%d] = [%s]", i, esp_base.linhas_det[i]);
         }
      }
#endif

   return(ret);
}


/* ira tratar linhas no formato : UD=CAMPOUSUARIO campo1, campo2, campo3... */
/*     onde campos sao os campos que estaram restritos a edicao pelo USUARIO */
/*     USUARIO pode variar para GRUPO (D=DONO) pode variar para (R=RESPONSAVEL) */
COUNT trata_res(end_cp, end_ptr, buf)
COUNT *end_cp;
TEXT **end_ptr;
TEXT *buf;
{  COUNT ret=0, l, num, ind;
   TEXT *p;

   if ( (p = strchr(buf, ' ')) == NULL ) {
      ret = -1;
      goto fim;
   }

   *p++ = '\0';

   if ( (ind = ver_campo(BASE_EDIT, buf, &num)) == -1 ) {
      ret = -2;
      goto fim;
   }

   if ( (l = strlen(p)) < 1 ) {
      ret = -1;
      goto fim;
   }

   if ( (*end_ptr = malloc(l+2)) == NULL ) {
      ret = -3;
      goto fim;
   }

   strcpy(*end_ptr, p);
   strcat(*end_ptr, ","); /* para poder buscar campos delimitados por virgula */

   strupr(*end_ptr);

   *end_cp = ind;

   rst_base.ativada = 1;

fim:

   if ( ret != 0 ) {
      if ( ret == -1 )
         debug("Linha [%s] nao segue formato definido\nXX=NOMECAMPO campo1, ..., campon", buf);
      else if ( ret == -2 )
         debug("Linha [%s] : Campo inexistente: %s", buf);
      else if ( ret == -3 )
         debug("Sem memoria para alocar");
   }

   return(ret);
}


/* ira tratar linhas no formato : AP=NOME_CP,COME_CP... campo_dest LIN_DET,LIN_DET... */
COUNT trata_ap(end_nlin, vet, buf)
COUNT *end_nlin;
TEXT **vet;
TEXT *buf;
{  COUNT ret=0, ind, num;
   TEXT *p1, *p2, *ptr, campo[80];

   if ( (p1 = strchr(buf, ' ')) == NULL ||
        (p2= strchr(p1+1, ' ')) == NULL ||
        strchr(p2+1, ' ') != NULL) {
      ret = -1;
      goto fim;
   }

   *p2 = '\0';
   ind = ver_campo(BASE_EDIT, p1+1, &num);

   if ( ind == -1 ) {
      strcpy(campo, p1+1);
      *p2 = ' ';
      ret = -2;
      goto fim;
   }

   *p2 = ' ';

   if ( (ptr = malloc(strlen(buf) + 1)) == NULL ) {
      ret = -3;
      goto fim;
   }


   strcpy(ptr, buf);
   strupr(ptr);


   num = *end_nlin;
   if ( num > 8 ) {
      ret = -4;
      goto fim;
   }

   vet[num++] = ptr;

   *end_nlin = num;

fim:

   if ( ret != 0 ) {
      if ( ret == -1 )
         debug("Linha [%s] nao segue formato definido\nAP=NOMECAMPO,NOMECAMPO... campo_dest Linha_det,Linha_det...", buf);
      else if ( ret == -2 )
         debug("Linha [%s]\nNome do Campo inexistente ou invalido: %s", buf, campo);
      else if ( ret == -3 )
         debug("Sem memoria para alocar");
      else if ( ret == -4 )
         debug("Excesso de linhas do tipo AP");
   }

   return(ret);
}


/* ira tratar linhas no formato : DTn=Formato da linha */
COUNT trata_det(end_nlin, vet, buf)
COUNT *end_nlin;
TEXT **vet;
TEXT *buf;
{  COUNT ret=0, num;
   TEXT *ptr;

   end_nlin = end_nlin; /* Tira Warning */

   num = *buf - '0';
   if ( num > 9 || num < 1 || buf[1] != '=' ) {
      ret = -1;
      goto fim;
   }
   
   buf += 2;

   if ( (ptr = malloc(strlen(buf) + 1)) == NULL ) {
      ret = -3;
      goto fim;
   }


   strcpy(ptr, buf);

   vet[num] = ptr;

fim:

   if ( ret != 0 ) {
      if ( ret == -1 )
         debug("Linha [DT%s] nao segue formato definido\nDT[1-9]=Formato da linha", buf);
      else if ( ret == -3 )
         debug("Sem memoria para alocar");
   }

   return(ret);

}


VOID rlse_especial()
{  COUNT i;

      if ( rst_base.UDcampos != NULL ) free (rst_base.UDcampos );
      if ( rst_base.GDcampos != NULL ) free (rst_base.GDcampos );
      if ( rst_base.URcampos != NULL ) free (rst_base.URcampos );
      if ( rst_base.GRcampos != NULL ) free (rst_base.GRcampos );
      rst_base.UDcampos = NULL;
      rst_base.GDcampos = NULL;
      rst_base.URcampos = NULL;
      rst_base.GRcampos = NULL;

      rst_base.ativada = 0;

      esp_base.tecla_limpa = 0;

      esp_base.vista_inclusao = -1;
      esp_base.vista_aposgravar = -1;
      esp_base.vista_altf6 = -1;

      if ( esp_base.campos_limpa != NULL ) {
         free(esp_base.campos_limpa);
         esp_base.campos_limpa = NULL;
      }

      if ( esp_base.campos_altf6 != NULL ) {
         free(esp_base.campos_altf6);
         esp_base.campos_altf6 = NULL;
      }

      esp_base.nlin_ap = 0;
      esp_base.nlin_det = 0;
      for (i = 0; i < 10; i++) {
         if ( esp_base.linhas_ap[i] != NULL ) {
            free(esp_base.linhas_ap[i]);
            esp_base.linhas_ap[i] = NULL;
         }
         if ( esp_base.linhas_det[i] != NULL ) {
            free(esp_base.linhas_det[i]);
            esp_base.linhas_det[i] = NULL;
         }
      }
}
