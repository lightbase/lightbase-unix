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

#define   H_CDGR1       "CADGRID"    /* Cad grupo Identificacao */
#define   H_CDGR2       "CADGRNO"    /* Cad grupo Nome  */
#define   H_CDGR3       "CADGRLT"    /* Cad grupo Local trabalho */
#define   H_CDGR4       "CADGRFO"    /* Cad grupo Telefone contato */

#define   H_IDINV       "IDGRINV"    /* Identificacao de grupo invalida */
#define   H_JAUSU       "GRPJAUS"    /* Identific ja cadastrada como usuario */
#define   H_GRJAC       "GRJACAD"    /* Identific ja cadastrada */
#define   H_GRNOC       "GRNOCAD"    /* Identific de grupo nao cadastrada */

#define   H_MOSGR       "MOSTRAGR"   /* Mostra Cadastro de grupos */
#define   H_SEMGR       "SEMGRUPO"   /* sem grupos cadastrados */
#define   H_NOPCA       "NOPENRW"    /* nao pode abrir acessos para R/W */

#define   R_CDGR1    loc_str(GRP_CADUSU, 60)
#define   R_CDGR2    loc_str(GRP_CADUSU, 61)
#define   R_CDGR3    loc_str(GRP_CADUSU, 62)
#define   R_CDGR4    loc_str(GRP_CADUSU, 63)


#define   T_CDGR0    loc_str(GRP_CADUSU, 64)
#define   T_CDGR1    loc_str(GRP_CADUSU, 65)
#define   T_CDGR2    loc_str(GRP_CADUSU, 66)
#define   T_CDGR3    loc_str(GRP_CADUSU, 67)
#define   T_CDGR4    loc_str(GRP_CADUSU, 68)

#define   E_JAUSU       loc_str(GRP_CADUSU, 69)
#define   E_GRJAC       loc_str(GRP_CADUSU, 70)
#define   E_GRNOC       loc_str(GRP_CADUSU, 71)

#define   M_WIGRP       loc_str(GRP_CADUSU, 72)
#define   M_WAGRP       loc_str(GRP_CADUSU, 73)
#define   E_SEMGR       loc_str(GRP_CADUSU, 74)
#define   T_GRMOS       loc_str(GRP_CADUSU, 75)
#define   M_GRNAV       loc_str(GRP_CADUSU, 76)

#define   E_NOPCA       loc_str(GRP_CADUSU, 502)
#define   E_NOPRD       loc_str(GRP_CADUSU, 503)

static COUNT   flg_op;
static Grupo   *cad_grp;

extern COUNT ct_semseguranca;


#ifdef MSDOS
   COUNT add_grp( VOID );
   COUNT alt_grp( VOID );
   COUNT ve_grupo(d_entry *);
#else
   COUNT add_grp( );
   COUNT alt_grp( );
   COUNT ve_grupo( );
#endif

/*----------------------------------------------------atu_grupo----------*/
F__GLB   COUNT atu_grupo(opcao)
COUNT opcao;
{  COUNT ret = 0;
	WIN *j0 = NULL;
   DTela g1[2];
   DTela g2[4];

   up_str(GRP_CADUSU);

   cad_grp = NULL;

   /* se foi chamado com UMA_INC o cad_acessos ja esta preparado */

   if ( (cad_grp = (Grupo *) calloc(1, sizeof(Grupo))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }


   /* Identificacao do grupo */
   seta_dtela(&g1[0], cad_grp->gr_ident_grp, X14, R_CDGR1, 1, 28, 0, E_ALFA, H_CDGR1, NULL, NULL, ve_grupo);
   seta_dtela(&g1[1], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

   /* Dados do grupo */
   seta_dtela(&g2[0], cad_grp->gr_full_name, x40, R_CDGR2, 2, 28, 0, E_ALFA,  H_CDGR2, NULL, NULL, NULL );
   seta_dtela(&g2[1], cad_grp->gr_loc_trab,  x40, R_CDGR3, 3, 28, 0, E_ALFA,  H_CDGR3, NULL, NULL, NULL );
   seta_dtela(&g2[2], cad_grp->gr_fone,     FONE, R_CDGR4, 4, 28, 0, E_ALFA,  H_CDGR4, NULL, NULL, NULL );
   seta_dtela(&g2[3], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);


   flg_op = opcao;

   j0 = abre_janela(J_CADGRP);

	fwbox(j0);
   fw_title(j0, T_CDGR0, j0->w_atrib);

	mvwprintw(j0, 1, 1, T_CDGR1);
	mvwprintw(j0, 2, 1, T_CDGR2);
	mvwprintw(j0, 3, 1, T_CDGR3);
	mvwprintw(j0, 4, 1, T_CDGR4);

   do {

le_grp :

		memset((TEXT *)cad_grp, 0, sizeof(Grupo)); /* zera a estrutura */

		ret = ed_read(j0, &g2[0], 0, SO_LIMPA, LF13E);
		ret = ed_read(j0, &g1[0], 0, LIMPA_LE, LF13E);

	   switch (ret) {
         case -1    :
         case K_ESC : goto fim; /* algum erro ja impresso por ed_generica */
                                /* ou grupo teclou ESC */

			case K_UP  : goto le_grp;

         default    : break;
	   }


		switch ( flg_op ){

   		case UMA_INC  :
         case INCLUSAO :   cad_grp->gr_full_name[0] = '\0';
                           cad_grp->gr_loc_trab[0]  = '\0';
									cad_grp->gr_fone[0]      = '\0';

                           ret = ed_read(j0, &g2[0], 0, LIMPA_LE, LF13E);
                           break;

         case ALTERACAO:   ret = ed_read(j0, &g2[0], 0, MOSTRA_LE, LF13E);
                           break;

         case EXCLUSAO :   ret = ed_read(j0, &g2[0], 0, SO_MOSTRA, LF13E);
                           break;
      }


	   switch (ret) {
         case -1    :
         case K_ESC : goto fim; /* algum erro ja impresso por ed_generica */
                              /* ou grupo teclou ESC */

			case K_UP  : goto le_grp;

         default    : break;
	   }



      switch ( flg_op ) {
   		case UMA_INC   :
			case INCLUSAO  :  ret = add_grp();
                           break;

			case ALTERACAO :  ret = alt_grp();
                           break;
		}

		if ( opcao == UMA_INC ) { /* chamada de dentro da inclusao de usuario */
         ret = 0;
         goto fim;
      }

   } while ( ret == 0 );


fim :

   fwkill( j0 );

   if ( cad_grp ) {
      free((TEXT *)cad_grp);
      cad_grp = NULL;
   }

   down_str(GRP_CADUSU);
   return ( ret );
}


/*----------------------------------------------------mos_grupo------------*/
F__GLB   COUNT mos_grupo()
{
   COUNT    ret, totgrp, salva_seg;
   COUNT    usulin = 0;
   TEXT    *ajuda_ant = win_ajuda;
   TEXT    *p, *pu;
	UTEXT    str_escape[20];
   TEXT     chavegr[80];
   TEXT     chaveus[80];
   WIN *jp = NULL, *jf=NULL, *jm = NULL;
   Usuario *cad_usu;

   RECALFIL(ace_dnum);
   RECALFIL(ace_inum);



   salva_seg = ct_semseguranca;
   ct_semseguranca = 1; /* para fazer com que find_rec consiga achar um proximo */

   up_str(GRP_CADUSU);
   win_ajuda = H_MOSGR;

   totgrp  = 0;
   ret     = 0;

   if ( (cad_usu = (Usuario *) calloc(1, sizeof(Usuario))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   if ( (cad_grp = (Grupo *) calloc(1, sizeof(Grupo))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }



   jp = abre_janela(J_MOSGRP);

   fw_title(jp, T_GRMOS, jp->w_atrib);
	fwbox(jp);


   jf = abre_funcoes(funcao(LF13E));
   jm = abre_mensagem(M_AGUARDE);


   if ( ! find_rec(ace_dnum, ace_inum, GR_ID, str_nada, '>') ||
        strncmp(ap_chave_ret, GR_ID, 2) != 0 ) {
		mens_erro(H_SEMGR, E_SEMGR);
		ret = -1;
      goto fim;
   }

   strcpy(chavegr,ap_chave_ret);

   p = load_rec(ace_dnum, ace_inum, GR_ID, &chavegr[2], cad_grp, sizeof(Grupo), '=');

   while ( p != NULL ) {

      if ( usulin < 4 && usulin > 0 )
         fwprintf(jp,"\n");

      fwprintf(jp,"%-14.14s %-40.40s\n",
               cad_grp->gr_ident_grp,cad_grp->gr_full_name);

      /*----- busca usuarios---------------------*/
      if ( ! find_rec(ace_dnum, ace_inum, US_ID, str_nada, '>'))
         goto outrogr;

      strcpy(chaveus,ap_chave_ret);

      pu = load_rec(ace_dnum, ace_inum, US_ID, &chaveus[2],
                   cad_usu, sizeof(Usuario), '=');

      usulin = 0;

      while ( pu != NULL ) {

         if ( strcmp(cad_usu->us_grupo, cad_grp->gr_ident_grp) == 0 ) {

            if ( ++usulin == 1 )
               totgrp++;

            if ( usulin == 4 ) {
               fwprintf(jp,"  %-14.14s \n", cad_usu->us_ident_usu);
               usulin = 0;
            }
            else {
               fwprintf(jp,"  %-14.14s ", cad_usu->us_ident_usu);
            }

            fwrefresh(jp);
         }

         pu = load_rec(ace_dnum, ace_inum, US_ID, str_nada,
                       cad_usu, sizeof(Usuario), 'N');

         if ( strncmp(ap_chave_ret, US_ID, 2) != 0 )
            goto outrogr;
      }

outrogr:

      fwrefresh(jp);

      totgrp++;

      if ( ! find_rec(ace_dnum, ace_inum, GR_ID, &chavegr[2], '='))
         goto mostra;

      p = load_rec(ace_dnum, ace_inum, GR_ID, str_nada,
                   cad_grp, sizeof(Grupo), 'N');

      strcpy(chavegr, ap_chave_ret);

      if ( strncmp(ap_chave_ret, GR_ID, 2) != 0 )
         goto mostra;
   }

mostra:

   ct_semseguranca = salva_seg ;

   mens_1(jm, M_GRNAV);

   pr_escape(LF13E, str_escape);
   ret = w_navega(jm, jp, totgrp, (TEXT *)str_escape);

fim:

   ct_semseguranca = salva_seg ;

   fecha_mensagem(jm);
   fecha_funcoes(jf);

	if ( jp ) fwkill( jp );

   if ( cad_grp ) {
      free((TEXT *)cad_grp);
      cad_grp = NULL;
   }

   if ( cad_usu ) free((TEXT *)cad_usu);

   win_ajuda = ajuda_ant;

   return(ret);
}





/**
**#  ---------------------------------------------------------------------
**#   Funcao...: COUNT ve_grupo()
**#   Objetivo.: Verificar validade de uma operacao no cadastro de grupos
**#              bem como Carregar os dados do grupo cadastrado para
**#              a variavel GLOBAL cad_grp (alteracao/exclusao)
**#   Entrada..: entry - Endereco da estrutura de entrada de dados
**#   Retorna..:  0 -  Se operacao valida
**#              -1 -  Operacao invalida
**#   Utiliza..: Estrutura GLOBAL cad_grp
**#              variavel  GLOBAL flg_op   (operacao INC/ALT/EXC)
**#
**/
F__LOC   COUNT ve_grupo(entry)
d_entry *entry;
{  COUNT ret = 0;
	TEXT *pos;

	if (entry->tecla == K_ESC ) goto fim;

	if ( val_indice(entry->str) != OK) {
      mens_erro(H_IDINV, E_NOMEIN);
		ret = REJEITA;
      goto fim;
   }

   /* nao pode existir como usuario */

   if ( find_rec(ace_dnum, ace_inum, US_ID, entry->str, '=') ) {
		mens_erro(H_JAUSU, E_JAUSU);
		ret = REJEITA;
      goto fim;
   }

   pos = load_rec(ace_dnum, ace_inum, GR_ID, entry->str,
                  cad_grp, sizeof(Grupo), '=');

	switch ( flg_op ) {
		case UMA_INC   :
		case INCLUSAO  :  if (pos != NULL ) {
									mens_erro( H_GRJAC, E_GRJAC);
									ret = -1;
								}
								break;

		case ALTERACAO :
		case EXCLUSAO  :  if (pos == NULL ) {
									mens_erro( H_GRNOC, E_GRNOC);
									ret = -1;
								}
								break;

	}

fim :

   return(ret);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT add_grp()
**#   Objetivo.: inclui registro de grupo no CADASTRO e indexa o mesmo
**#   Entrada..: Nenhuma
**#   Retorno..:  0 - Conseguiu cadastrar os dados e indice Ok
**#              -1 - Erro na inclusao
**#   Utiliza..: . struct global cad_grp
**#
**/
F__LOC   COUNT add_grp()
{  COUNT ret;
   WIN *jm=NULL;

   jm = abre_mensagem(M_WIGRP);

   strcpy(cad_grp->gr_tpreg, GR_ID);

	if ( ! add_rec(ace_dnum, ace_inum, GR_ID, cad_grp->gr_ident_grp,
                cad_grp, sizeof(Grupo)))
      ret = -1;

   fecha_mensagem(jm);

   return(ret);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT alt_grp()
**#   Objetivo.: alterar dados do grupo no "CADASTRO"
**#   Entrada..: Nenhuma
**#   Retorna..:  0 - OK
**#              -1 - Erro - Nao conseguiu atualizar "CADASTRO"
**#   Utiliza..: . struct global cad_grp
**#
**/
F__LOC   COUNT alt_grp()
{  COUNT ret= 0;
   WIN *jm;

   jm = abre_mensagem(M_WAGRP);

   if ( ! exc_rec(ace_dnum, ace_inum, GR_ID,
                  cad_grp->gr_ident_grp, !SO_INDEX) ) {
      ret = -1;
   }
   else {

		if ( ! add_rec(ace_dnum, ace_inum,  GR_ID, cad_grp->gr_ident_grp,
                     cad_grp, sizeof(Grupo))) {
         ret = -1;
      }
   }

   fecha_mensagem(jm);

   return(ret);
}
