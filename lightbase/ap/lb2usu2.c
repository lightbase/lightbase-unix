#include  "lb2all.h"

#include  <string.h>
#include  HD_MEMO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#define   GRP_CADUSU    13

#define     LF13E       "1 3 E"
#define     STR_CHAVE   "%s!%s"
#define     STR_DUAS    "%s\n%s"
#define     UMA_INC 	   'U'      /* inclusao unica */
#define     SO_INDEX    1
#define     FONE "(9999) 999-99-99 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"


#define   H_CDUS1       "CADUSID"    /* Cad usuario Identificacao */
#define   H_CDUS2       "CADUSGP"    /* Cad usuario Grupo */
#define   H_CDUS3       "CADUSSE"    /* Cad usuario Senha */
#define   H_CDUS4       "CADUSNO"    /* Cad usuario Nome completo */
#define   H_CDUS5       "CADUSLT"    /* Cad usuario Local Trabalho */
#define   H_CDUS6       "CADUSFO"    /* Cad usuario Telefone */
#define   H_CDUS7       "CADUSLI"    /* Cad usuario Idioma */
#define   H_CDUS8       "CADUSST"    /* Cad usuario Status */
#define   H_CDUS9       "CADUSMN"    /* Cad usuario menu do usuario */

#define   H_PRIUS       "PRIMUSU"    /* Primeiro usuario deve ser 'Adm' */
#define   H_IDINV       "IDUSINV"    /* Identificacao de usuario invalida */
#define   H_JAGRP       "USJAGRP"    /* Identific ja cadastrada como grupo */
#define   H_USJAC       "USJACAD"    /* Identific ja cadastrada */
#define   H_USNOC       "USNOCAD"    /* Identific de usuario nao cadastrada */
#define   H_CRGRP       "USCDGRP"    /* Cadastramento de grupo por cad. usuario */
#define   H_REDIG       "REDIGSEN"   /* Redigitar senha para confirmacao */
#define   H_EXUSU       "EXCLUIUS"   /* Exclui usuario */
#define   H_NOTD1       "ACESSBAD"   /* Cadastro de acesso danificado */
#define   H_MOSUS       "MOSTRAUS"   /* Mostra Cadastro de usuarios */
#define   H_SEMUS       "ACESSBAD"   /* sem usuarios cadastrados */
#define   H_NOPCA       "NOPENRW"    /* nao pode abrir acessos para R/W */

#define   R_CDUS1    loc_str(GRP_CADUSU, 20)
#define   R_CDUS2    loc_str(GRP_CADUSU, 21)
#define   R_CDUS3    loc_str(GRP_CADUSU, 22)
#define   R_CDUS4    loc_str(GRP_CADUSU, 23)
#define   R_CDUS5    loc_str(GRP_CADUSU, 24)
#define   R_CDUS6    loc_str(GRP_CADUSU, 25)
#define   R_CDUS7    loc_str(GRP_CADUSU, 26)
#define   R_CDUS8    loc_str(GRP_CADUSU, 27)
#define   R_CDUS9    loc_str(GRP_CADUSU, 97)

#define   T_CDUS0    loc_str(GRP_CADUSU, 28)
#define   T_CDUS1    loc_str(GRP_CADUSU, 29)
#define   T_CDUS2    loc_str(GRP_CADUSU, 30)
#define   T_CDUS3    loc_str(GRP_CADUSU, 31)
#define   T_CDUS4    loc_str(GRP_CADUSU, 32)
#define   T_CDUS5    loc_str(GRP_CADUSU, 33)
#define   T_CDUS6    loc_str(GRP_CADUSU, 34)
#define   T_CDUS7    loc_str(GRP_CADUSU, 35)
#define   T_CDUS8    loc_str(GRP_CADUSU, 36)
#define   T_CDUS9    loc_str(GRP_CADUSU, 98)

#define   E_PRIUS       loc_str(GRP_CADUSU, 37)
#define   E_JAGRP       loc_str(GRP_CADUSU, 38)
#define   E_USJAC       loc_str(GRP_CADUSU, 39)
#define   E_USNOC       loc_str(GRP_CADUSU, 40)

#define   TW_US31       loc_str(GRP_CADUSU, 41)
#define   TW_US32       loc_str(GRP_CADUSU, 42)
#define   TW_US39       loc_str(GRP_CADUSU, 43)

#define   E_REDIG       loc_str(GRP_CADUSU, 44)

#define   M_WIUSU       loc_str(GRP_CADUSU, 45)
#define   M_WAUSU       loc_str(GRP_CADUSU, 46)
#define   M_WEUSU       loc_str(GRP_CADUSU, 47)

#define   TW_US41       loc_str(GRP_CADUSU, 48)
#define   TW_US42       loc_str(GRP_CADUSU, 49)
#define   TW_US49       loc_str(GRP_CADUSU, 50)
#define   M_EACBA       loc_str(GRP_CADUSU, 51)
#define   T_MOSTRA      loc_str(GRP_CADUSU, 52)
#define   E_SEMUS       loc_str(GRP_CADUSU, 53)
#define   M_USNAV       loc_str(GRP_CADUSU, 54)

#define   OP_STATUS     loc_str(GRP_CADUSU, 56)

#define   E_NOTD1       loc_str(GRP_CADUSU, 501)
#define   E_NOPCA       loc_str(GRP_CADUSU, 502)
#define   E_NOPRD       loc_str(GRP_CADUSU, 503)




#ifdef MSDOS
   COUNT add_usu( VOID );
   COUNT alt_usu( VOID );
   COUNT exc_usu( VOID );
   COUNT ve_usuario(d_entry *);
   COUNT vl_grupo(d_entry *);
   COUNT vl_senha(d_entry *);
   COUNT pr_idioma(d_entry *);
   COUNT cr_idioma(d_entry *);
   COUNT pr_status(d_entry *);
   COUNT cr_status(d_entry *);
#else
   COUNT add_usu( );
   COUNT alt_usu( );
   COUNT exc_usu( );
   COUNT ve_usuario( );
   COUNT vl_grupo( );
   COUNT vl_senha( );
   COUNT pr_idioma( );
   COUNT cr_idioma( );
   COUNT pr_status( );
   COUNT cr_status( );
#endif




extern COUNT ap_rb_acesso;
extern COUNT ct_semseguranca;


#ifndef LITE

static COUNT   flg_op;
static Usuario *cad_usu;
static TEXT    *wk_status;
static TEXT    *ult_senha;
static TEXT    *new_senha;


/*----------------------------------------------------atu_usuario----------*/
F__GLB   COUNT atu_usuario(opcao)
COUNT opcao;
{  COUNT ret = 0;
	WIN *j0;
   DTela u1[2];
   DTela u2[9];

   up_str(GRP_CADUSU);


	j0        = NULL;
   cad_usu   = NULL;
   wk_status = NULL;
   ult_senha = NULL;
   new_senha = NULL;


   cad_usu   = (Usuario *) calloc(1, sizeof(Usuario));
   wk_status = malloc(TE+1);
   ult_senha = malloc(TE+1);
   new_senha = malloc(TE+1);

   if ( cad_usu == NULL || wk_status == NULL || ult_senha == NULL || new_senha == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   /* Identificacao do usuario */
   seta_dtela(&u1[0], cad_usu->us_ident_usu, X14, R_CDUS1, 1, 28, 0, E_ALFA, H_CDUS1, NULL, NULL, ve_usuario);
   seta_dtela(&u1[1], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);


   /* Dados do usuario */
   seta_dtela(&u2[0], cad_usu->us_grupo,     X14, R_CDUS2, 2, 28, 0, E_ALFA,  H_CDUS2, NULL, NULL, vl_grupo );
   seta_dtela(&u2[1], new_senha,             S14, R_CDUS3, 3, 28, 0, E_ALFA,  H_CDUS3, NULL, NULL, vl_senha );
   seta_dtela(&u2[2], cad_usu->us_full_name, x40, R_CDUS4, 4, 28, 0, E_ALFA,  H_CDUS4, NULL, NULL, NULL );
   seta_dtela(&u2[3], cad_usu->us_loc_trab,  x40, R_CDUS5, 5, 28, 0, E_ALFA,  H_CDUS5, NULL, NULL, NULL );
   seta_dtela(&u2[4], cad_usu->us_fone,      FONE, R_CDUS6, 6, 28, 0, E_ALFA, H_CDUS6, NULL, NULL, NULL );
   seta_dtela(&u2[5], cad_usu->us_idioma,    x14, R_CDUS7, 7, 28, 0, E_LISTA, H_CDUS7, OP_IDIOMAS, pr_idioma, cr_idioma );
   seta_dtela(&u2[6], wk_status,             x14, R_CDUS8, 8, 28, 0, E_LISTA, H_CDUS8, OP_STATUS,  pr_status, cr_status );
   seta_dtela(&u2[7], cad_usu->us_menu,      x40, R_CDUS9, 9, 28, 0, E_ALFA,  H_CDUS9, NULL, NULL, NULL);
   seta_dtela(&u2[8], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);


   flg_op = opcao;


   j0 = abre_janela(J_CADUSU);
	fwbox(j0);

   fw_title(j0, T_CDUS0, j0->w_atrib);

	mvwprintw(j0, 1, 1, T_CDUS1);
	mvwprintw(j0, 2, 1, T_CDUS2);
	mvwprintw(j0, 3, 1, T_CDUS3);
	mvwprintw(j0, 4, 1, T_CDUS4);
	mvwprintw(j0, 5, 1, T_CDUS5);
	mvwprintw(j0, 6, 1, T_CDUS6);
	mvwprintw(j0, 7, 1, T_CDUS7);
	mvwprintw(j0, 8, 1, T_CDUS8);
	mvwprintw(j0, 9, 1, T_CDUS9);

   wk_status[0] = '\0';
   ult_senha[0] = '\0';
   new_senha[0] = '\0';

   do {

le_usu :

		memset((TEXT *)cad_usu, '\0', sizeof(Usuario)); /* zera a estrutura */

		ret = ed_read(j0, &u2[0], 0, SO_LIMPA, LF13E);
		ret = ed_read(j0, &u1[0], 0, LIMPA_LE, LF13E);

	   switch (ret) {
         case -1    :
         case K_ESC : goto fim; /* algum erro ja impresso por ed_generica */
                                /* ou usuario teclou ESC */

			case K_UP  : goto le_usu;

         default    : break;
	   }


		switch ( flg_op ){

         case INCLUSAO :   cad_usu->us_senha[0]     = '\0';
                           cad_usu->us_grupo[0]     = '\0';
                           cad_usu->us_full_name[0] = '\0';
                           cad_usu->us_loc_trab[0]  = '\0';
									cad_usu->us_fone[0]      = '\0';
									cad_usu->us_status[0]    = '\0';
									cad_usu->us_menu[0]      = '\0';
                           strcpy(cad_usu->us_idioma, ap_tabidioma[0]);

                           ret = ed_read(j0, &u2[0], 0, LIMPA_LE, LF13E);
                           break;

         case ALTERACAO:   ret = ed_read(j0, &u2[0], 0, MOSTRA_LE, LF13E);
                           break;

         case EXCLUSAO :   ret = ed_read(j0, &u2[0], 0, SO_MOSTRA, LF13E);
                           break;
      }


	   switch (ret) {
         case -1    :
         case K_ESC : goto fim; /* algum erro ja impresso por ed_generica */
                                /* ou usuario teclou ESC */

			case K_UP  : goto le_usu;

         default    : break;
	   }


      if ( ap_rb_acesso != 0 && cad_usu->us_status[0] != 'A' ) {
         mens_erro(H_PRIUS, E_PRIUS);
         goto le_usu;
      }

      switch ( flg_op ) {
			case INCLUSAO  :  ret = add_usu();
                           break;

			case ALTERACAO :  ret = alt_usu();
                           break;

         case EXCLUSAO  :  ret = exc_usu();
                           break;
		}

      if ( ap_rb_acesso != 0 ) {
         goto fim;
      }


   } while ( ret == 0 );


fim :

   if ( j0 ) fwkill( j0 );

   if ( new_senha ) free(new_senha);
   if ( ult_senha ) free(ult_senha);
   if ( wk_status ) free(wk_status);
   if ( cad_usu ) {
      free((TEXT *)cad_usu  );
      cad_usu = NULL;
   }

   down_str(GRP_CADUSU);
   return ( ret );
}


/*----------------------------------------------------mos_usuario----------*/
F__GLB   COUNT mos_usuario()
{
   COUNT    ret, totusu, salva_seg;
   TEXT    *ajuda_ant = win_ajuda;
   TEXT    *p;
   TEXT     chave[80];
	UTEXT str_escape[20];
   WIN *jp = NULL, *jf=NULL, *jm = NULL;

   RECALFIL(ace_dnum);
   RECALFIL(ace_inum);



   salva_seg = ct_semseguranca;
   ct_semseguranca = 1; /* para fazer com que find_rec consiga achar um proximo */

   up_str(GRP_CADUSU);
   win_ajuda = H_MOSUS;

   totusu  = 0;
   ret     = 0;

   jp = abre_janela(J_MOSUSU);

   fw_title(jp, T_MOSTRA, jp->w_atrib);
	fwbox(jp);


   jf = abre_funcoes(funcao(LF13E));
   jm = abre_mensagem(M_AGUARDE);


   cad_usu   = (Usuario *) calloc(1, sizeof(Usuario));

   if ( cad_usu == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   if ( ! find_rec(ace_dnum, ace_inum, US_ID, str_nada, '>')) {
		mens_erro(H_SEMUS, E_SEMUS);
		ret = -1;
      goto fim;
   }

   strcpy(chave,ap_chave_ret);

   p = load_rec(ace_dnum, ace_inum, US_ID, &chave[2], cad_usu, sizeof(Usuario), '=');

   while ( p != NULL ) {

      fwprintf(jp," %-14.14s %-40.40s %14.14s\n",
               cad_usu->us_ident_usu,cad_usu->us_full_name,cad_usu->us_grupo);
      fwrefresh(jp);
      totusu++;

      p = load_rec(ace_dnum, ace_inum, US_ID, str_nada, cad_usu, sizeof(Usuario), 'N');

      if ( strncmp(ap_chave_ret, US_ID, 2) != 0 ) {
         goto mostra;
      }

   }

mostra:

   ct_semseguranca = salva_seg ;

   mens_1(jm, M_USNAV);

   pr_escape(LF13E, str_escape);
   ret = w_navega(jm, jp, totusu, (TEXT *)str_escape);

fim:

   ct_semseguranca = salva_seg ;

   if ( cad_usu ) {
      free((TEXT *)cad_usu  );
      cad_usu = NULL;
   }


   fecha_mensagem(jm);
   fecha_funcoes(jf);

	if ( jp != NULL ) fwkill( jp );
   win_ajuda = ajuda_ant;

   down_str(GRP_CADUSU);
   return(ret);
}



/**
**#  ---------------------------------------------------------------------
**#   Funcao...: COUNT ve_usuario()
**#   Objetivo.: Verificar validade de uma operacao no cadastro de usuarios
**#              bem como Carregar os dados do usuario cadastrado para
**#              a variavel GLOBAL cad_usu (alteracao/exclusao)
**#   Entrada..: entry - Endereco da estrutura de entrada de dados
**#   Retorna..:  0 -  Se operacao valida
**#              -1 -  Operacao invalida
**#   Utiliza..: Estrutura GLOBAL cad_usu
**#              variavel  GLOBAL flg_op   (operacao INC/ALT/EXC)
**#
**/
F__LOC   COUNT ve_usuario(entry)
d_entry *entry;
{  COUNT ret = 0, i;
	TEXT *pos;

	if (entry->tecla == K_ESC ) goto fim;

	if ( val_indice(entry->str) != OK) {
      mens_erro(H_IDINV, E_NOMEIN);
		ret = REJEITA;
      goto fim;
   }

   /* nao pode existir como grupo */

   if ( find_rec(ace_dnum, ace_inum, GR_ID, entry->str, '=') ) {
		mens_erro(H_JAGRP, E_JAGRP);
		ret = REJEITA;
      goto fim;
   }

   pos = load_rec(ace_dnum, ace_inum, US_ID, entry->str,
                  cad_usu, sizeof(Usuario), '=');

   new_senha[0] = '\0';
   ult_senha[0] = '\0';


	switch ( flg_op ) {
		case INCLUSAO  :  if (pos != NULL ) {
									mens_erro( H_USJAC, E_USJAC);
									ret = -1;
								}
								break;

		case ALTERACAO :
		case EXCLUSAO  :  if (pos == NULL ) {
									mens_erro( H_USNOC, E_USNOC);
									ret = -1;
								}
								break;
	}



   /* prepara apresentacao da senha */
   if ( cad_usu->us_tmsenha > 0 && cad_usu->us_tmsenha < TE ) {
      for (i=0; i < cad_usu->us_tmsenha; i++ )
         new_senha[i] = '*';
      new_senha[i] = '\0';
   }



fim :

   return(ret);
}


/**
**#  ---------------------------------------------------------------------
**#   Funcao...: COUNT vl_grupo
**#   Objetivo.: Verificar se existe grupo especificado em entry->str
**#   Entrada..: entry   - Endereco da estrutura de entrada de dados
**#   Retorna..: ACEITA  -  Se grupo esta cadastrado
**#              REJEITA -  grupo    NAO cadastrado
**#
**/
F__LOC   COUNT vl_grupo(entry)
d_entry *entry;
{  COUNT ret = REJEITA;
   COUNT tecla;
   d_entry salva;

   salva = *entry;

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP || tecla == K_PGUP) {
		ret = ACEITA;
		goto fim;
	};

   trim(entry->str);

   if ( entry->str[0] == '\0' ) {
		ret = ACEITA;
		goto fim;
	};

	if ( val_indice(entry->str) != OK) {
      mens_erro(H_IDINV, E_NOMEIN);
		ret = REJEITA;
   }

	else {

retenta:

      if ( find_rec(ace_dnum, ace_inum, GR_ID, entry->str, '=') == (POINTER) 0 ) {

         ret = decida( str_nada, TW_US39, H_CRGRP, 2, 33,
                       STR_DUAS, TW_US31, TW_US32);

         if ( ret == 0 ) {
            /* para entrar em cad_crupo ja com nome */
            w_ungc(K_ENTER);
            w_ungs(entry->str);
            if ( atu_grupo(UMA_INC) == 0 ) goto retenta;
         }
         ret = REJEITA;
      }
      else
         ret = ACEITA;
   }

fim :
   *entry = salva;

	return(ret);
}



/*#  -----------------------------------------------------------------------------
**#   Funcao...: vl_senha()
**#   Objetivo.: Confirma senha de usuario atravez de dupla digitacao
**/
F__LOC   COUNT vl_senha(entry)
d_entry *entry;
{  COUNT ret;
	COUNT tecla;

	tecla = entry->tecla;
   strupr(entry->str);


	if ( tecla == K_ESC  ) {
		ret = ACEITA;
		goto fim;
	};

	if ( ult_senha[0] == '\0' && entry->mudou == 0 ) {
		ret = ACEITA;
		goto fim;
	};

   trim(entry->str);

   if ( strcmp(entry->str, ult_senha) == 0 ) { /* senha duplamente digitada */
      cad_usu->us_tmsenha = (COUNT)strlen(entry->str);
      if ( cad_usu->us_tmsenha == 0 )
         cad_usu->us_tmsenha = ' ';

      lb_encrypt((UTEXT *) entry->str, (UTEXT *) cad_usu->us_senha, '\0');
      ret = ACEITA;
   }
   else {
      mens_erro(H_REDIG, E_REDIG);
      strcpy(ult_senha, entry->str);
      w_ungc(0X19); /* CTRL Y */
      w_ungc(K_HOME);
      ret = REJEITA;
   }

fim:

   return(ret);
}




/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT add_usu()
**#   Objetivo.: inclui registro de usuario no CADASTRO e indexa o mesmo
**#   Entrada..: Nenhuma
**#   Retorno..:  0 - Conseguiu cadastrar os dados e indice Ok
**#              -1 - Erro na inclusao
**#   Utiliza..: . struct global cad_usu
**#
**/
F__LOC   COUNT add_usu()
{  COUNT ret= 0;
   WIN *jm;

   jm = abre_mensagem(M_WIUSU);

   strcpy(cad_usu->us_tpreg, US_ID);

   cad_usu->us_uso_futuro[0] = (cad_usu->us_status[0] - 'A') + 10;

	if ( ! add_rec(ace_dnum, ace_inum, US_ID, cad_usu->us_ident_usu,
                cad_usu, sizeof(Usuario)))
      ret = -1;


   fecha_mensagem(jm);

   return(ret);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT alt_usu()
**#   Objetivo.: alterar dados do usuario no CADASTRO
**#   Entrada..: Nenhuma
**#   Retorna..:  0 - OK
**#              -1 - Erro - Nao conseguiu atualizar CADASTRO
**#   Utiliza..: . struct global cad_usu
**#
**/
F__LOC   COUNT alt_usu()
{  COUNT ret= 0;
   WIN *jm;

   jm = abre_mensagem(M_WAUSU);

   if ( ! exc_rec(ace_dnum, ace_inum, US_ID,
                  cad_usu->us_ident_usu, !SO_INDEX) ) {
      ret = -1;
   }
   else {
      cad_usu->us_uso_futuro[0] = (cad_usu->us_status[0] - 'A') + 10;
		if ( ! add_rec(ace_dnum, ace_inum,  US_ID, cad_usu->us_ident_usu,
                     cad_usu, sizeof(Usuario))) {
         ret = -1;
      }
   }

   fecha_mensagem(jm);

   return(ret);
}

/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT exc_usu()
**#   Objetivo.: Exclui um usuario do CADASTRO e do indice
**#   Entrada..: Nenhuma
**#   Retorna..:  0 - OK
**#              -1 - Erro - Nao conseguiu excluir usuario do CADASTRO
**#   Utiliza..: . struct global cad_usu
**#              . ap_chave_ret
**#
**/
F__LOC   COUNT exc_usu()
{  COUNT ret= 0, tam;
	TEXT ident[TE+1], base[TE+1], chave[TE+TE+2], chave1[TE+TE+2];
   POINTER pos;
   WIN *jm = NULL;
	WIN *jw = NULL;

   ret = decida( str_nada, TW_US49, H_EXUSU, 2, 37,
                  STR_DUAS, TW_US41, TW_US42);

   if ( ret != 0 ) {
      goto fim;
   }

   jm = abre_mensagem(M_WEUSU);


   jw = abre_janela(J_EXCUSU);
	fwbox( jw );

   /* primeiro exclui registro do tipo US */

	strcpy(ident, cad_usu->us_ident_usu);

	pos = exc_rec(ace_dnum, ace_inum, US_ID, cad_usu->us_ident_usu, !SO_INDEX);

   if ( pos == 0 ) {
      ret = -1;
      goto fim;
   }

   /* uma vez excluido do indice, temos que eliminar as definicoes */
   /* de acesso D2 e depois o D1 da respectiva base. */
   /* estas exclusoes sao efetuadas apenas no indice */


   mvwprintw(jw, 1, 1, M_EACBA);
   fwrefresh(jw);

   strcpy(chave1, D2_ID);
	strcat(chave1, ident);
   tam = (COUNT)strlen(chave1);

   while ( 1 ) {

   	pos = find_rec(ace_dnum, ace_inum, D2_ID, ident, '^');

      if ( ! pos || strncmp(ct_indkey, chave1, tam) != 0 )
         break;

      if ( ct_indkey[tam] != '!' ) break;

      strncpy(base, &ct_indkey[tam+1], TE+1);

		fwat(3, 0, jw); fwcll(jw);
      mvwprintw(jw, 3, 3 + (30 - strlen(base) / 2), base );
      fwrefresh(jw);

      sprintf(chave, STR_CHAVE, ident, base);
      pos = exc_rec(ace_dnum, ace_inum, D2_ID, chave, SO_INDEX);

      sprintf(chave, STR_CHAVE, base, ident);
      pos = exc_rec(ace_dnum, ace_inum, D1_ID, chave, !SO_INDEX);

      if ( pos == 0 ) {
         mens_erro(H_NOTD1, E_NOTD1);
         continue;
      }
   }

fim:

   if ( jw != NULL ) fwkill(jw);
   if ( jm != NULL ) fecha_mensagem(jm);

   return(ret);
}



F__LOC   COUNT pr_idioma(entry)
d_entry *entry;
{  COUNT ret = 0, i;
	TEXT  aux[80];

   ret = 0;

   for ( i = 1 ; i < MAX_IDIOMAS; i++ ) {
      if ( piece(OP_IDIOMAS, aux, '@', i, TE) == 0 ) break;
      if ( strcmp( aux, cad_usu->us_idioma) == 0 ) {
         ret = 1;
      }
   }

   if ( ret == 0 ) {
      if ( cad_usu->us_ident_usu[0] != '\0')
         piece(OP_IDIOMAS, entry->str, '@', 1, TE);
      else
          entry->str[0] = '\0';
   }
   else ret = 0;

   return(ret);

}


F__LOC   COUNT cr_idioma(entry)
d_entry *entry;
{  COUNT ret, i;
	COUNT tecla;
	TEXT  aux[80];

	tecla = entry->tecla;

	if ( tecla == K_ESC  ) {
		ret = ACEITA;
		goto fim;
	};

   ret = REJEITA;

   for ( i = 1 ; i < MAX_IDIOMAS; i++ ) {

      if ( piece(OP_IDIOMAS, aux, '@', i, TE) == 0 ) break;

      if ( strcmp( aux, entry->str) == 0 ) {
         ret = ACEITA;
      }
   }

fim:

   return(ret);
}

F__LOC   COUNT pr_status(entry)
d_entry *entry;
{  COUNT ret = 0;

   ret = 0;


   if ( cad_usu->us_status[0] < 'A' || cad_usu->us_status[0] > 'D' ) {
      cad_usu->us_status[0] = 'A';
   }

   if ( cad_usu->us_ident_usu[0] != '\0')
      piece(OP_STATUS, entry->str, '@', cad_usu->us_status[0] - 'A' + 1, TE);
   else
      entry->str[0] = '\0';

   ret = 0;

   return(ret);
}


F__LOC   COUNT cr_status(entry)
d_entry *entry;
{  COUNT ret, i;
	COUNT tecla;
	TEXT  aux[80];

	tecla = entry->tecla;

	if ( tecla == K_ESC  ) {
		ret = ACEITA;
		goto fim;
	};

   ret = REJEITA;

   for ( i = 1 ; i < 20; i++ ) {

      if ( piece(OP_STATUS, aux, '@', i, TE) == 0 ) break;

      if ( strcmp( aux, entry->str) == 0 ) {
         ret = ACEITA;
         switch( i ) {
            case  1 : cad_usu->us_status[0] = 'A';  break;
            case  2 : cad_usu->us_status[0] = 'B';  break;
            case  3 : cad_usu->us_status[0] = 'C';  break;
            case  4 : cad_usu->us_status[0] = 'D';  break;
         }
         break;
      }
   }

fim:

   return(ret);

}

/* fim do ifdef LITE */
#endif
