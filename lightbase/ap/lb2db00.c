#include  "lb2all.h"

#include  <string.h>
#include  HD_DIR

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#include  "lb2db.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#define   LF13E       "1 3 E"
#define   STR_DUAS    "%s\n%s"
#define   STR_QUATRO  "%s\n%s\n%s\n%s"

#define   COL_NOME     28   /* coluna da janela onde esta o nome da base */




#define   H_SODEF    "SOABDEF"   /* So' usuarios A e B podem acessar DefBase */

#define   H_DBNOM    "DFBSNOME"   /* Nome da base a ser definida */
#define   H_DBPAT    "DFBSPATH"   /* PATH da base a ser definida */
#define   H_DBDES    "DFBSDESC"   /* Descricao da base a ser definida */

#define   H_NBSIN    "NOMEBSIN"   /* Nome de base invalido */
#define   H_BSEXI    "BASEEXIS"   /* Base ja existe nao pode ser criada */
#define   H_JAARQ    "NOTCRDIR"   /* Nao pode criar diretorio */
#define   H_IMPBS    "IMPOBASE"   /* Confirmacao de importacao de base */
#define   H_EXCAB    "EXCARQSB"   /* Conf.de exclusao dos arquivos da base */
#define   H_IMPIN    "IMPOINPO"   /* Importacao impossivel. Arq. danificado */
#define   H_N4INE    "ARQN4INE"   /* Arquivo de descricao da base inexistente */
#define   H_CHAVE    "CODPERM"    /* Codigo de permissao para a basae */
#define   H_RANGE1   "RANGE1"     /* Valores de identificacao de base */
#define   H_RANGE2   "RANGE2"     /* Valores de release */
#define   H_DEFREST  "DEFREST"    /* Definicao da base restrita ao fornecedor */

#define   H_1FINGER  "FINGER1"    /* personalizacao da base comercial */
#define   H_2FINGER  "FINGER2"    /* personalizacao (release) da base comercial */

#define   H_SOACES   "SODEFACE"    /* so e permitido modificacao de acesso */
#define   H_SOIMP    "SOIMPO"      /* runtime so e permitido importar */


#define   M_DB00  loc_str(GRP_DEFBASE, 1  )
#define   M_DB01  loc_str(GRP_DEFBASE, 2  )
#define   M_DB02  loc_str(GRP_DEFBASE, 3  )
#define   M_DB03  loc_str(GRP_DEFBASE, 4  )
#define   M_DB04  loc_str(GRP_DEFBASE, 5  )
#define   M_DB05  loc_str(GRP_DEFBASE, 6  )
#define   M_DB06  loc_str(GRP_DEFBASE, 7  )

#define   R_DB01  loc_str(GRP_DEFBASE, 8  )
#define   R_DB02  loc_str(GRP_DEFBASE, 9  )
#define   R_DB03  loc_str(GRP_DEFBASE, 10 )
#define   R_DB04  loc_str(GRP_DEFBASE, 11 )
#define   R_DB05  loc_str(GRP_DEFBASE, 12 )
#define   R_DB06  loc_str(GRP_DEFBASE, 13 )


#define   T_DBTIT    loc_str(GRP_DEFBASE, 14)
#define   T_DBROD    loc_str(GRP_DEFBASE, 15)

#define   T_DBNOM    loc_str(GRP_DEFBASE, 16)
#define   T_DBPAT    loc_str(GRP_DEFBASE, 17)
#define   T_DBDES    loc_str(GRP_DEFBASE, 18)

#define   R_DBNOM    loc_str(GRP_DEFBASE, 19)
#define   R_DBPAT    loc_str(GRP_DEFBASE, 20)
#define   R_DBDES    loc_str(GRP_DEFBASE, 21)

#define   E_BSEXI    loc_str(GRP_DEFBASE, 22)

#define   TW_BSI1    loc_str(GRP_DEFBASE, 23)
#define   TW_BSI2    loc_str(GRP_DEFBASE, 24)
#define   TW_BSI9    loc_str(GRP_DEFBASE, 25)

#define   TW_BSE1    loc_str(GRP_DEFBASE, 26)
#define   TW_BSE2    loc_str(GRP_DEFBASE, 27)
#define   TW_BSE9    loc_str(GRP_DEFBASE, 28)


#define   M_MODACE    loc_str(GRP_DEFBASE, 35)


#define   T_DBNDES    loc_str(GRP_DEFBASE, 47)


#define   TW_CODD0    loc_str(GRP_DEFBASE, 60)
#define   TW_CODD9    loc_str(GRP_DEFBASE, 61)
#define   TW_CODD1    loc_str(GRP_DEFBASE, 62)
#define   TW_CODD2    loc_str(GRP_DEFBASE, 63)


#define   T_CODPERM   loc_str(GRP_DEFBASE, 65)
#define   R_DBCOD     loc_str(GRP_DEFBASE, 66)
#define   E_CODMAX    loc_str(GRP_DEFBASE, 67)


#define   T_TFINGER  loc_str(GRP_DEFBASE, 70)
#define   T_1FINGER  loc_str(GRP_DEFBASE, 71)
#define   T_2FINGER  loc_str(GRP_DEFBASE, 72)
#define   R_1FINGER  loc_str(GRP_DEFBASE, 73)
#define   R_2FINGER  loc_str(GRP_DEFBASE, 74)


#define   TW_BSA1    loc_str(GRP_DEFBASE, 75)
#define   TW_BSA2    loc_str(GRP_DEFBASE, 76)
#define   TW_BSA9    loc_str(GRP_DEFBASE, 77)

#define   E_JAARQ    loc_str(GRP_DEFBASE, 501)
#define   E_SODEF    loc_str(GRP_DEFBASE, 502)
#define   E_IMPIN    loc_str(GRP_DEFBASE, 503)
#define   E_N4INE    loc_str(GRP_DEFBASE, 504)

#define   E_RANGE1   loc_str(GRP_DEFBASE, 523)
#define   E_RANGE2   loc_str(GRP_DEFBASE, 524)
#define   E_DEFREST  loc_str(GRP_DEFBASE, 525)


#define   E_SOIMP    loc_str(GRP_DEFBASE, 527)

#ifdef MSDOS
   COUNT def_bcmp( COUNT *, WIN *, COUNT);
   COUNT db_exec(Selecao *);
   COUNT atu_desc( VOID );
   COUNT nova_base(WIN *);
   COUNT vl_newnome(d_entry *);
   COUNT pr_path(d_entry *);
   COUNT vl_path(d_entry *);
   COUNT pr_desc(d_entry *);
   COUNT if_base(TEXT *, TEXT*);
   COUNT alt_acessos(VOID);
   LONG le_seguranca( Base * );
   COUNT vl_idbase(d_entry *);
   COUNT vl_1range(d_entry *);
   COUNT vl_2range(d_entry *);
   COUNT pede_finger( Base *, COUNT );
#else
   COUNT def_bcmp( );
   COUNT db_exec( );
   COUNT atu_desc( );
   COUNT nova_base( );
   COUNT vl_newnome( );
   COUNT pr_path( );
   COUNT vl_path( );
   COUNT pr_desc( );
   COUNT if_base( );
   COUNT alt_acessos( );
   LONG le_seguranca( );
   COUNT vl_idbase( );
   COUNT vl_1range( );
   COUNT vl_2range( );
   COUNT pede_finger( );
#endif



static COUNT deve_criar;   /* controle da necessidade de criacao de Diret. */
static COUNT flag_importa;
static WIN *j1_glb;        /* sera utilizada pela funcao atu_desc */

F__GLB   COUNT def_base()
{  COUNT ret = 0, locked = 0;
   COUNT lin;
   TEXT  base_ant[TE + 1], *p;
   TEXT  aux[PATH_MAX+10];
   WIN *j1;
   WIN *j0;
   TEXT *ajuda_ant = win_ajuda;

   j0 = j1 = NULL;

   up_str(GRP_DEFBASE);
   win_ajuda = "MENUBASE";


   if ( ap_status != 'A' && ap_status != 'B' ) {
      mens_erro(H_SODEF, E_SODEF);
      ret = -1;
      goto sai;
   }


   /* invariavelmente sera fechado o arquivo da base corrente */
   /* isto e necessario para evitar alterar com usuario usando a base */

	if (WCB != NULL && WCB->cb_fordem != 0 ) {
      strcpy(base_ant, WBS->bs_nome); /* para recuperar base ativa caso erro */
		lib_ativa(BASE_EDIT);
   }
   else
      base_ant[0] = '\0';

   /* aloca area para descritor da base em EDICAO */
   if ( ( WBS = (Base *) calloc(1, sizeof(Base))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto sai;
   }

   lin=8;

#ifdef RUNTIME
  p = NEW_BASE;
#else
   if ( (p = uma_base(lin, base_ant, RESTRITO, PODE_NOVA, M_SELBASE)) == NULL ) {
      goto fim2;
   }
#endif

   j0 = abre_janela(J_UTIL);         /* abre janela para salvar conteudo */
   j1_glb = j1 = abre_janela(J_NEWBASE);

   fw_title(j1, T_DBTIT, j1->w_atrib);
	fwbox(j1);

	mvwprintw(j1, 0,  2, T_DBNOM );
	mvwprintw(j1, 0, 38, T_DBPAT );
	mvwprintw(j1, 1,  2, T_DBDES );

	fwbot(j1);

   if ( strcmp(p, NEW_BASE) == 0 ) {    /* Selecionada Opcao NOVA_BASE */

      ret = nova_base(j1);

      switch (ret) {

         case K_ESC  :  /* Desistencia da criacao */
                        goto fim;

         case 0      :  /* Nova base criada OK. Continua Processo */
                        break;

         case 1      :  /* Base Importada com sucesso */
                        ret = 0;
                        goto fim;

         default     :  /* Erro na criacao da base nova. Cancela */
                        ret = -1;
                        goto fim;
      }
   }
   else {
		if ( ! load_rec( ace_dnum, ace_inum, D0_ID, p,
                       WBS, sizeof(Base), '=') ) {
         ret = -1;
         mens_erro(H_BADAC, E_BADAC);
         goto fim;
      }

      if ( WBS->bs_finger != 0 && pode_exportar(WBS->bs_idbase) != 'S' ) {
         ret = -1;
         mens_erro(H_DEFREST, E_DEFREST);
         goto fim;
      }
   }

   if ( info_base(WBS->bs_nome, WBS->bs_path, LB_4_EXT, aux) <= 0 ) {
      ret = -1;
      mens_erro(H_N4INE, E_N4INE);
      goto fim;
   }

   /* Verifica se existe arquivo de lock na base */

   if ( LOCK_RECURSO(WBS->bs_nome, WBS->bs_path, LCK_EXCLUSIVO) != 0 ) {
      /* operacao negada */
      COUNT x = -1;

#ifndef LITE
      x = decida( M_ATENCAO, TW_BSA9, H_SOACES, 2, 46,
                    STR_DUAS, TW_BSA1, TW_BSA2);
#endif

      if  ( x != 1 ) {
         mens_erro(H_NOLOCK, E_NOLOCK);
         ret = -1;
         goto fim;
      }
   }

   else
      locked = 1;

   if ( open_lb4(WBS, aux, 'D', ap_nserie, WBS->bs_idbase ) != 0 ) {
      ret = -1;
      goto fim;
   }

   fwcls(j1);

	mvwprintw(j1, 0,  2, T_DBNOM );
	mvwprintw(j1, 0, 38, T_DBPAT );
	mvwprintw(j1, 1,  2, T_DBDES );

	mvwprintw(j1, 0, COL_NOME, WBS->bs_nome );

   strcpy(aux, WBS->bs_path); aux[18] = '\0';
	mvwprintw(j1, 0, 50, aux );

	mvwprintw(j1, 1, COL_NOME, WBS->bs_desc );

	fwtop(j1);

#ifdef RUNTIME
   rec_prot( BASE_EDIT );
#else
   if ( def_bcmp(&ret, j1, locked) != 0 )
      goto fim;
#endif

   if ( locked && WBS->bs_updated ) {
      if ( lb4_upd(1) != 0 || alt_acessos() != 0)
         ret = -1;
      WBS->bs_updated = 0;
   }

fim :

#ifndef RUNTIME
   if ( locked )
      rlse_campos(BASE_EDIT);
#endif

   close_lb4();

   if ( locked )
      LOCK_RECURSO(WBS->bs_nome, WBS->bs_path, LCK_LIBERA);


   if ( ret == 0 ) {
      /* devera abrir base criada/modifica ao inves da anterior */
      strcpy(base_ant, WBS->bs_nome);
   }

   lib_ativa(BASE_EDIT);
   seta_acesso(NULL);     /* deve zerar acesso */

fim2:

   if ( WBS != NULL ) {
      free(WBS);
      WBS = NULL;
   }

	if ( j1 ) fwkill( j1 );
	if ( j0 ) fwkill( j0 );

sai:

   win_ajuda    = ajuda_ant;
   down_str(GRP_DEFBASE);
   return(0);
}


#ifndef RUNTIME
F__LOC   COUNT def_bcmp( ret_ind, j1, completa)
COUNT *ret_ind;
WIN *j1;
COUNT completa;
{  COUNT ret=0;
   TEXT  aux[PATH_MAX+10];
	Selecao m0;
   WIN *jf=NULL;
   TEXT *m_pr[(7 * 3) + 1];


   set_menu(m_pr, 0, M_DB01, "1", R_DB01);
   set_menu(m_pr, 1, M_DB02, "1", R_DB02);
   set_menu(m_pr, 2, M_DB03, "1", R_DB03);
#ifndef LITE
   set_menu(m_pr, 3, M_DB04, "1", R_DB04);
#else
   set_menu(m_pr, 3, M_DB04, "0", R_DB04);
#endif
   set_menu(m_pr, 4, M_DB05, "1", R_DB05);
   set_menu(m_pr, 5, M_DB06, "1", R_DB06);
   set_menu(m_pr, 6, NULL,   "0", NULL);

   if ( completa == 0 ) {
      /* so sera possivel modificacao de acessos */
#ifndef LITE
      rec_prot( BASE_EDIT );
      ret = 0;
      goto fim;
#endif

   }

   if ( v_campos[BASE_EDIT] != NULL )
      numdebug(201);

   if ( (v_campos[BASE_EDIT] = load_campos(BASE_EDIT, 'E')) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      *ret_ind = -1;
      ret = 1;
      goto fim;
   }

   if ( qtd_vetcmp(BASE_EDIT) == 0 ) {

		rec_campos( BASE_EDIT );

      if ( qtd_vetcmp(BASE_EDIT) <= 0 ) {
         ret = 1;
         goto fim;
      }

		rec_vistas( BASE_EDIT );

		rec_relats( BASE_EDIT );

#ifndef LITE
		rec_prot( BASE_EDIT );
#endif

   }
   else {

      jf = abre_funcoes(funcao(LF13E));

      *ret_ind = 0;

	   while ( *ret_ind >= 0 ) {

		   def_menu( &m0, 34,  8, 30, 15,  1 , MS_CAMPO,  MS_BARRA, SIMPLES,
				      MS_JANELA, MS_DESTAQUE, &m_pr, M_DB00, str_nada);

         m0.escolhido = ret;
         m0.executa = db_exec;

         switch ( *ret_ind = menu( &m0 )) {

		      case  0  :  rec_campos( BASE_EDIT );   break;
		      case  1  :  rec_vistas( BASE_EDIT );   break;
		      case  2  :  rec_relats( BASE_EDIT );   break;
#ifndef LITE
		      case  3  :  rec_prot( BASE_EDIT );     break;
#endif
		      case  4  :  rec_stop( );               break;
		      case  5  :  sprintf(aux,"%-40.40s", WBS->bs_desc );
                     	mvwprintw(j1, 1, COL_NOME, aux);
                        fwrefresh(j1);
                        break;

            default  :  break;

         };
      };

      fecha_funcoes(jf);
   }

fim :

   return(ret);
}

F__LOC   COUNT db_exec(s)
Selecao *s;
{
   if ( s->tecla != K_ESC && s->escolhido == 5 )
		atu_desc( );

   return(-1);
}


F__LOC   COUNT atu_desc( )
{  COUNT ret;
	DTela vt[2];

   seta_dtela(&vt[0], WBS->bs_desc, x40, R_DBDES, 1, COL_NOME,  0, E_ALFA,  H_DBDES, NULL, NULL, NULL );
   vt[1].destino = NULL;

le :

 	ret = ed_read(j1_glb, vt, 1, MOSTRA_LE, LF13E);

	switch (ret) {
      case -1    :
      case K_ESC : ret = -1; goto fim;
      default    :
                   break;
	}


   if ( WBS->bs_finger != 0 && pode_exportar(WBS->bs_idbase) == 'S' ) {
      UCOUNT salva;
      salva = WBS->bs_release;
      if ( pede_finger(WBS, 0) < 0 ) {
         WBS->bs_release = salva;
      }
      else {
         WBS->bs_xor = calc_bsxor(WBS);
      }
   }

   WBS->bs_updated = 1;

fim :

   return(ret);
}
#endif




F__LOC   COUNT nova_base(j1)
WIN *j1;
{  COUNT ret, ca;
   DTela b1[4];
   TEXT  masc_path[PATH_MAX+1];

#ifdef MSDOS
   ca = 18;
   memset(masc_path, 'X', PATH_MAX);
#else
   ca = 23;
   memset(masc_path, 'x', PATH_MAX);
#endif

   masc_path[PATH_MAX]='\0';

   /* Dados da nova base */
   seta_dtela(&b1[0], WBS->bs_nome, XNBASE, R_DBNOM, 0, COL_NOME,  0, E_ALFA,  H_DBNOM, NULL, NULL, vl_newnome);
   seta_dtela(&b1[1], WBS->bs_path, masc_path,  R_DBPAT, 0,   50, ca, E_ALFA,  H_DBPAT, NULL, pr_path, vl_path);
   seta_dtela(&b1[2], WBS->bs_desc, x40,    R_DBDES, 1, COL_NOME,  0, E_ALFA,  H_DBDES, NULL, pr_desc, NULL );
   seta_dtela(&b1[3], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

	fwtop(j1);

le :

 	ret = ed_read(j1, &b1[0], 1, MOSTRA_LE, LF13E);

	switch (ret) {
      case K_ESC  : goto fim;
		case K_UP   :
		case K_PGUP : goto le;
		case K_DN   :
		case K_PGDN :
		case K_ENTER:
      default     : break;
	}

   if ( flag_importa ) {
      if ( inport_base(BASE_EDIT) == 0 )
         ret = 1;  /* base importada com sucesso */
      else
         ret = -1;
   }
   else {
#ifdef RUNTIME
      ret = -1;
#else
      ret = cria_base( (deve_criar) ? WBS->bs_path : NULL );
#endif
   }

fim :

   return(ret);
}





/**
**#  ---------------------------------------------------------------------
**#   Funcao...: vl_newnome
**#   Objetivo.: Criticar nome da nova base
**#   Entrada..: entry - Endereco da estrutura de entrada de dados
**#   Retorna..: ACEITA  -  Se foi aceito nome da base informada
**#              REJEITA -  Nome da base nao foi aceito
**#
**/
F__LOC   COUNT vl_newnome(entry)
d_entry *entry;
{  COUNT ret;
   COUNT tecla;

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP || tecla == K_PGUP) {
		ret = ACEITA;
		goto fim;
	};

	if ( val_indice(entry->str) != OK) {
      mens_erro(H_NBSIN, E_NOMEIN);
      ret = REJEITA;
      goto fim;
   };

   if ( find_rec(ace_dnum, ace_inum, D0_ID, entry->str, '=') ) {
		mens_erro(H_BSEXI, E_BSEXI);
		ret = REJEITA;
   }
   else
      ret = ACEITA;

fim :

	return(ret);
}

/**
**#  ---------------------------------------------------------------------
**#   Funcao...: pr_path
**#   Objetivo.: Propor path caso o mesmo seja nulo
**#   Entrada..: entry - Endereco da estrutura de entrada de dados
**#   Retorna..: 0 - se entrada deve efetuar leitura
**#              1 - se leitura NAO deve ser efetuada
**#
**/
F__LOC   COUNT pr_path(entry)
d_entry *entry;
{
   if ( WBS->bs_nome[0] != 0 ) {
      if ( entry->str[0] == '\0' ) {
         memset(entry->mascara, 'P', strlen(ap_dirbase) + 1); /* +1 para proteger a barra */
         strcpy( entry->str, ap_dirbase);
         strcat( entry->str, STR_BARRA);
         strcat( entry->str, WBS->bs_nome);
#ifdef MSDOS
         strupr(entry->str);
#endif
      }
   }

   return(0);
}

/**
**#  ---------------------------------------------------------------------
**#   Funcao...: vl_path
**#   Objetivo.: Avaliar a existencia/possibilidade de existencia
**#              do diretorio proposto
**#   Entrada..: entry - Endereco da estrutura de entrada de dados
**#   Retorna..: 0 - se entrada deve efetuar leitura
**#              1 - se leitura NAO deve ser efetuada
**#
**/
F__LOC   COUNT vl_path(entry)
d_entry *entry;
{  COUNT ret;
   TEXT aux[PATH_MAX + 1], *p;


	if ( entry->tecla == K_ESC ) {
		ret = ACEITA;
		goto fim;
	};

   if ( entry->str[0] == '\0' ) {  /* nao permite path em branco */
      ret = REJEITA;
      goto fim;
   }

   p = strchr(entry->str, '\0');

   if ( *--p == BARRA)
      *p = '\0';

   /* verifica se situacao do diretorio proposto */

   ret = info_base(str_nada, entry->str, str_nada, aux);

   strcpy(entry->str, aux);

   deve_criar = 0;
   flag_importa = 0;

   switch( ret ) {
      case 0 : /* Nao existe diretorio proposto. Devera criar */
               deve_criar = 1;  /* Nao existe nada */
               break;

      case -1: /* Diretorio Provavelmente ja existe (mas pode ser arq especial) */
               /* se existe arquivo de descricao da base OK, atualize desc */
               ret = if_base(entry->str, WBS->bs_nome);
               switch(ret) {
                  case -1 :   ret = REJEITA;
                              goto fim;

                  case 0  :   break;

                  case 2  :   /* confirmada solicitacao de importacao */
                              ret = ACEITA;
                              goto fim;
               }

               break;

      default: /* existe arquivo com nome proposto */
               mens_erro(H_JAARQ, E_JAARQ);
               ret = REJEITA;
               goto fim;
   }

	ret = ACT_REIMP;

#ifdef RUNTIME
   if ( flag_importa == 0 ) {
      mens_erro(H_SOIMP, E_SOIMP);
      ret = REJEITA;
   }
#endif


fim :

   return(ret);
}



/**
**#  ---------------------------------------------------------------------
**#   Funcao...: pr_desc
**#   Objetivo.: avaliar se dev solicitar ou nao descricao da base
**#              em funcao da importacao de base.
**#   Entrada..: entry - Endereco da estrutura de entrada de dados
**#   Retorna..: 0 - se entrada deve efetuar leitura
**#              1 - se leitura NAO deve ser efetuada
**#
**/
F__LOC   COUNT pr_desc(entry)
d_entry *entry;
{  entry = entry;
   return( flag_importa ? 1 : 0 );
}





/**
**#  ---------------------------------------------------------------------
**#   Funcao...: if_base
**#   Objetivo.: avaliar se existe diretorio/arquivo da base de dados
**#              se existir arquivo de descricao de base, pede confir-
**#              macao de importacao/exclusao dos arquivos
**#   Entrada..: entry - Endereco da estrutura de entrada de dados
**#   Retorna..: 0 - Diretorio valido para criacao da base
**#              -1- Erro de acesso no diretorio
**#              2 - Solicitada importacao da base
**#
**/
F__LOC   COUNT if_base(path, nome)
TEXT *path, *nome;
{  COUNT ret;
   TEXT n4[PATH_MAX+10];
   LONG   loc_serie;
   UCOUNT loc_idarq;

   ret = info_base(nome, path, LB_4_EXT, n4);

   if ( ret == 0 ) /* Diretorio valido e nao existe arquivo */
      goto fim;

   if ( ret == -1 ) /* Erro no acesso */
      goto fim;

   /* 999 para pegar serie/idarq. MANUT_OPEN abre e fecha nesta situacao */
   loc_serie = 0L;
   loc_idarq = 999;

   ret = MANUT_OPEN( lb4_dnum, n4, 'V', &loc_serie, &loc_idarq );

   if ( ret == NO_ERROR || ret == IDBASE_ERR ) {
      /* confirmacao de importacao */
      switch ( decida( M_ATENCAO, TW_BSI9, H_IMPBS, 2, 46,
                    STR_DUAS, TW_BSI1, TW_BSI2)) {
         case -1 :   ret = -1;  goto fim;
         case  0 :   flag_importa = 1;  /* Solicitada importacao de base */
                     if ( open_lb4(WBS, n4, 'I', loc_serie, loc_idarq) == 0 ) {
                        close_lb4();
                        ret =  2;
                        goto fim;
                     }
                     else {
                        mens_erro(H_IMPIN, E_IMPIN);
                        flag_importa = 0;
                     }
      }
   }

#ifdef RUNTIME
   ret = -1;
#else
   /* confirmacao de eliminacao dos arquivos */
   if ( decida( M_ATENCAO, TW_BSE9, H_EXCAB, 2, 44,
                  STR_DUAS, TW_BSE1, TW_BSE2) == 0 ) {
      ret = 0;
      goto fim;
   }
   else ret = -1;
#endif

fim :

   if ( flag_importa == 0 ) {
      loc_serie = 0L;
      loc_idarq = 0;
   }

   return(ret);
}


F__LOC   COUNT alt_acessos()
{  COUNT ret;
   POINTER pos;
   WIN *jm = NULL;

   ret = 0;

   jm = abre_mensagem(M_MODACE);

   pos = find_rec(ace_dnum, ace_inum, D0_ID, WBS->bs_nome, '=');

   if ( pos == 0L ) {
      numdebug(210);
      fim_lb("210");
   }

   /* altera registro no cadastro de acessos */
   if ( ! alt_rec(ace_dnum, ace_inum, D0_ID, pos, WBS, sizeof(Base))) {
      numdebug(211);
	   ret = -1;
   }


fim :

	if ( jm ) fecha_mensagem(jm);

   return(ret);
}

#ifndef RUNTIME
F__GLB   COUNT def_seguranca( wbs )
Base *wbs;
{  COUNT ret;


   WBS->bs_nserie  = ap_nserie;

   wbs->bs_idbase  = 0;
   wbs->bs_finger  = 0;
   wbs->bs_release = 0;
   wbs->bs_xor     = 0;

   ret = decida( TW_CODD0, TW_CODD9, H_CHAVE, 2, 47,
                  STR_DUAS, TW_CODD1, TW_CODD2);

   switch ( ret ) {
      case  0 :   /* Deixa base sem cod acesso */
                  break;

      case  1 :   /* le novo codigo */
                  if ( le_seguranca(wbs) < 0 ) {
                     ret = -1;
                     goto fim;
                  }
                  else {
                     wbs->bs_xor = calc_bsxor(wbs);
                  }
                  break;

      default :
      case -1 :   ret = -1;
                  goto fim;
   }

fim :

   return( ret);
}


F__LOC   LONG le_seguranca( wbs )
Base *wbs;
{  COUNT  ret;
   TEXT aux[20];
	DTela vt[2];
   WIN *j1;
   TEXT *senha;

   j1 = abre_janela(J_CODPERM);

   fw_title(j1, T_CODPERM, j1->w_atrib);
	fwbox(j1);

   aux[0] = '\0';

   seta_dtela(&vt[0], aux, "99999", R_DBCOD, 1, 7,  0, E_ALFA,  H_CHAVE, NULL, NULL, vl_idbase );
   vt[1].destino = NULL;

 	ret = ed_read( j1 , vt, 1, MOSTRA_LE, LF13E);

	switch (ret) {
      case -1    :
      case K_ESC : ret = -1;
                   goto fim;

      default    :
                   break;
	}

   wbs->bs_idbase  = (UCOUNT)atol(aux);

   /*
      se variavel ambiental LBSENHA for compativel com id_base informado
      pede tambem  finger e release
   */

   senha = getenv("LBSENHA");
   if ( senha != NULL ) {
      LONG l;
      l = atol(aux);
      sprintf(aux, "%06ld", l);
      if ( t_numse(aux, senha, 0L) == OK ) {
         if ( pede_finger(wbs, 1) < 0 ) {
            ret = -1;
            goto fim;
         }
      }
   }

   ret = 0;

fim :

	if ( j1 ) fwkill( j1 );

   return(ret);
}



F__LOC   COUNT vl_idbase(entry)
d_entry *entry;
{  COUNT ret;

	ret = ACEITA;

	if ( entry->tecla != K_ESC &&
        atol(entry->str) < 100L || atol(entry->str) > 65000L ) {
      mens_erro(H_CHAVE, E_CODMAX);
      ret = REJEITA;
   };

	return(ret);
}


F__LOC   COUNT pede_finger( wbs, nova_definicao )
Base *wbs;
COUNT nova_definicao;
{  COUNT ret;
   TEXT ident[10];
   TEXT edic[10];
	DTela vt[3];
   WIN *j1;

   ident[0] = '\0';
   edic[0]  = '\0';

   j1 = abre_janela(J_FINGER);

   fw_title(j1, T_TFINGER, j1->w_atrib);
	fwbox(j1);


   mvwprintw(j1,  1,  4, T_1FINGER);
   mvwprintw(j1,  2,  4, T_2FINGER);

   if ( nova_definicao ) {
      seta_dtela(&vt[0], ident, "99999", R_1FINGER, 1, 23,  0, E_ALFA,  H_1FINGER, NULL, NULL, vl_1range );
      seta_dtela(&vt[1], edic,  "99999", R_2FINGER, 2, 23,  0, E_ALFA,  H_2FINGER, NULL, NULL, vl_2range );
      vt[2].destino = NULL;
   }
   else {
      sprintf(ident, "%u", wbs->bs_finger);
      sprintf(edic,  "%u", wbs->bs_release);

      seta_dtela(&vt[0], ident, "99999", R_1FINGER, 1, 23,  0, E_ALFA,  H_1FINGER, NULL, NULL, vl_1range );
      seta_dtela(&vt[1], edic,  "99999", R_2FINGER, 2, 23,  0, E_ALFA,  H_2FINGER, NULL, NULL, vl_2range );
      vt[2].destino = NULL;

      ed_read( j1 , vt, 1, SO_MOSTRA, LF13E);

      seta_dtela(&vt[0], edic,  "99999", R_2FINGER, 2, 23,  0, E_ALFA,  H_2FINGER, NULL, NULL, vl_2range );
      vt[1].destino = NULL;
   }

 	ret = ed_read( j1 , vt, 1, MOSTRA_LE, LF13E);

	switch (ret) {
      case -1    :
      case K_ESC : ret = -1;
                   goto fim;
	}

   if ( nova_definicao ) {
      wbs->bs_finger  = (UCOUNT)atol(ident);
      wbs->bs_release = (UCOUNT)atol(edic);
   }
   else {
      wbs->bs_release = (UCOUNT)atol(edic);
   }

   ret = 0;

fim :

	if ( j1 ) fwkill( j1 );

   return(ret);
}


F__LOC   COUNT vl_1range(entry)
d_entry *entry;
{  COUNT ret;

	ret = ACEITA;

	if ( entry->tecla != K_ESC &&
        atol(entry->str) < 1001L || atol(entry->str) > 1999L ) {
      mens_erro(H_RANGE1, E_RANGE1);
      ret = REJEITA;
   };

	return(ret);
}


F__LOC   COUNT vl_2range(entry)
d_entry *entry;
{  COUNT ret;

	ret = ACEITA;

	if ( entry->tecla != K_ESC &&
        atol(entry->str) < 1L || atol(entry->str) > 1000L ) {
      mens_erro(H_RANGE2, E_RANGE2);
      ret = REJEITA;
   };

	return(ret);
}
#endif


#ifdef RUNTIME
#  define   GRP_FER 27
Base *rec_base;

F__LOC   COUNT deleta_base()
{  COUNT ret = 0, tipo_lck;
   COUNT lin;
   TEXT *base;
   WIN  *jm, *ji;
   WIN *j0;

   ji = jm = NULL;
   jl_glb = NULL;
   fpo_glb = NULL;

   tipo_lck = LCK_TRAVA;

   up_str(GRP_FER);

   if ( (rec_base = (Base *) calloc(1, sizeof(Base))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   ji = abre_insercol(str_nada);
   jm = abre_mensagem(M_AGUARDE);

   lin=12;

   if ( (base = uma_base(lin, "", RESTRITO, PODE_UMA, M_SELBASE)) == NULL )
      goto fim;

	if ( ! load_rec(ace_dnum, ace_inum, D0_ID, base, rec_base, sizeof(Base), '=') ) {
      mens_erro(H_BADAC, E_BADAC);
      ret = -1;
      goto fim;
   }


   if (WCB != NULL && WCB->cb_fordem != 0 )
      lib_ativa(BASE_EDIT);

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

   del_base( j0 );

   lib_ativa(BASE_EDIT);

   fwkill( j0 );

fim :

	fecha_mensagem(jm);
   fecha_insercol(ji);

   if ( rec_base != NULL )
      free(rec_base);

   down_str(GRP_FER);
   return(ret);
}
#endif

