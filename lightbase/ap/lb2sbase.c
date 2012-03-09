#include  "lb2all.h"
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#define   GRP_SBASE    15


#define   H_SBASE    "SELBASE"      /* opcao de selecao de base */
#define   H_INEXB    "INEXBASE"     /* era E_SB_INV base inexistente */
#define   H_USUBAS   "USACBASE"     /* era E_SB_ACE usuario/grupo nao pode acessar base */
#define   H_TOOBAS   "TOOBASES"     /* Muitas bases associadas ao usuario */
#define   H_NOBASE   "NOBASES"      /* Nenhuma base associada ao usuario */


#define   STR_SEPARADOR    "!"
#define   STR_8PT8S        "%-8.8s"
#define   STR_TRACO        "--"

#define   TW_SB00       loc_str(GRP_SBASE,  1)
#define   TW_SB01       loc_str(GRP_SBASE,  2)
#define   TW_SB02       loc_str(GRP_SBASE,  3)
/*
#define   TW_SB10       loc_str(GRP_SBASE,  4)
#define   TW_SB11       loc_str(GRP_SBASE,  5)
#define   TW_SB12       loc_str(GRP_SBASE,  6)
*/
#define   E_INEXB       loc_str(GRP_SBASE,  7)
#define   E_ACEUS       loc_str(GRP_SBASE,  8)
#define   E_TOOBA       loc_str(GRP_SBASE,  9)
#define   R_NODES       loc_str(GRP_SBASE, 10)
#define   T_MBASES      loc_str(GRP_SBASE, 11)
/* #define   R_MBASES      loc_str(GRP_SBASE, 12) */
#define   O_BNOVA       loc_str(GRP_SBASE, 13)
#define   R_BNOVA       loc_str(GRP_SBASE, 14)
#define   O_NENHU       loc_str(GRP_SBASE, 15)
#define   R_NENHU       loc_str(GRP_SBASE, 16)

#define   T_BASEESP     loc_str(GRP_SBASE, 17)
#define   R_BASEESP     loc_str(GRP_SBASE, 18)

#define   R_RTBNOVA       loc_str(GRP_SBASE, 19)

typedef struct {
	TEXT se_nome[TE+1]; /* nome da base */
   TEXT se_flag[2];
   TEXT se_desc[TD+1];
}  Sel_base;

static TEXT *vdb;

extern COUNT ct_semseguranca;


#ifdef MSDOS
   LOCAL TEXT *varre_dx(TEXT *ident, TEXT *grupo, TEXT *);
   LOCAL VOID  push_item(TEXT **, COUNT, TEXT *, TEXT *, TEXT , TEXT *);
   LOCAL COUNT scan_ident(TEXT **vet_sel, TEXT *ident, TEXT *grupo);
   LOCAL COUNT scan_todas( TEXT **vet_sel );
   COUNT trata_ctle(Selecao *);
#else
   TEXT *varre_dx( );
   VOID  push_item( );
   COUNT scan_ident( );
   COUNT scan_todas( );
   COUNT trata_ctle( );
#endif





/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT sel_base()
**#   Objetivo.: Selecionar uma base disponivel para utilizacao
**#   Recebe.. : - sele
**#              - nome_base
**#   Retorna..: (TEXT *)PONT - Nome da base selecionada
**#              (TEXT *)NULL - Erro - Nao conseguiu concluir analise
**#
**/
F__GLB   COUNT sel_base(sele, nbase, mensagem)
COUNT sele;
TEXT *nbase;
TEXT *mensagem;
{	COUNT ret;
   TEXT *ajuda_ant = win_ajuda;
   TEXT base_aux[TE+1];

   if ( par_cfg.base != NULL ) { /* se especificou base so pode selecionar a mesma */
      strcpy(base_aux, par_cfg.base);
      nbase = base_aux;
   }

   up_str(GRP_SBASE);

   win_ajuda = H_SBASE;



inic :

   ret = 0;

   if ( nbase[0] == '\0' ) {
      nbase = varre_dx(ap_nome, ap_grupo, mensagem);
   	if  ( nbase == NULL ) {
         ret = -1;
         goto sai;
      }
   }

   strupr(nbase);

   lib_ativa(sele);

   if ( strcmp(nbase, NO_BASE) == 0 ) { /* deixar sem nenhuma base selec. */
      ret = -1;
      goto fim;
   }

   if ( ! find_rec( ace_dnum, ace_inum, D0_ID, nbase, '=') ) {
      mens_erro(H_INEXB, E_INEXB);
      ret = -1;
      goto fim;
   }

   /* seta protecao em funcao do usuario/grupo */

   if ( seta_acesso(nbase) < 0 ) {
      mens_erro(H_USUBAS, E_ACEUS );
      if ( par_cfg.base != NULL ) { /* se especificou base nao tem outra tentativa */
         ret = -1;
         goto fim;
      }
      else {
         nbase[0] = '\0';
         goto inic;
      }
   }


	if ( open_base(nbase, UTILIZACAO) < 0 ) {
      ret = -1;
      goto fim;
   }

fim :

   if ( ret < 0 ) {
      memset((TEXT *)&ap_acesso_ativo, '\0', sizeof(Acesso));
      strcpy(nbase, str_nada);
   }
   else {
      persona(ap_nome, nbase);
   }

   imp_base(nbase);

sai :

   win_ajuda = ajuda_ant;

   down_str(GRP_SBASE);
   return (ret);
}





F__GLB   COUNT seta_acesso( base )
TEXT *base;
{  COUNT ret = 0;
	TEXT chave[TE+TE+2];
   TEXT *p;

   memset((TEXT *) &ap_acesso_ativo, '\0', sizeof(Acesso));

   if ( base == NULL || base[0] == '\0' ) {
      goto fim;
   }


   if ( ap_status == 'A') {
      strcpy(ap_acesso_ativo.da_ugnome, ap_nome);
      strcpy(ap_acesso_ativo.da_base, base);
      ret = 0;
   }
   else {
      strcpy(chave, base);
      strcat(chave, STR_SEPARADOR);
      strcat(chave, ap_nome);

      p = load_rec( ace_dnum, ace_inum, D1_ID, chave,
                    &ap_acesso_ativo, sizeof(Acesso), '=');

	   if ( p == NULL ) {  /* nao achou em usuario, procura por grupo */

         strcpy(chave, base);
         strcat(chave, STR_SEPARADOR);
         strcat(chave, ap_grupo);

         p = load_rec( ace_dnum, ace_inum, D1_ID, chave,
                       &ap_acesso_ativo, sizeof(Acesso), '=');

   	   if ( p == NULL ) {
            ret = -1;
            goto fim;
         }
      }
   }

#ifdef DEVE_SER_ASSIM
   ... so que WBS nao foi setado e da core dumped ...
   if ( ap_status == 'A' ||
       (ap_status == 'B' && strcmp(ap_nome, WBS->bs_dono) == 0))
      ap_acesso_ativo.da_tabela = 'X';
   else
      ap_acesso_ativo.da_tabela = ' ';
#else
   /* quebra galho WWW */

   if ( ap_status == 'A' || ap_status == 'B' )
      ap_acesso_ativo.da_tabela = 'X';
#endif

fim :

   return(ret);
}




/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT uma_base()
**#   Objetivo.: Selecionar uma base
**#   Recebe.. : - Linha
**#              - posicao inicial
**#              - Restrito : se != 0 so' bases do dono sao listadas
**#              - Pode_nova: se mostra opcao de NOVA_BASE
**#   Retorna..: (TEXT *)PONT - Nome da base selecionada
**#              (TEXT *)NULL - Erro - Nao conseguiu concluir analise
**#
**/
F__GLB   TEXT *uma_base(lin, proposto, restrito, pode_nova, rodape)
COUNT lin;
TEXT *proposto;
COUNT restrito;
COUNT pode_nova;
TEXT *rodape;
{  COUNT ret= 0, conta, tam, inicia_em, salva_seg;
	TEXT  base[TE+1], chave[TE+TE+2], operador, base_oito[TE + 1];
   TEXT *ajuda_ant = win_ajuda;
   TEXT *p_desc;
	static TEXT ret_aux[TE+1];
	Selecao m0;
   Base *rec_base;
   TEXT **vet_sel;
   WIN *jm , *jf;

   RECALFIL(ace_dnum);
   RECALFIL(ace_inum);



   salva_seg = ct_semseguranca;
   ct_semseguranca = 1; /* para fazer com que find_rec consiga achar um proximo */

   up_str(GRP_SBASE);
   win_ajuda = "UMA_BASE";

   memset(base, '\0', TE+1);
   memset(base_oito, '\0', TE+1);

   rec_base = (Base *) calloc(1, sizeof(Base));
   vet_sel  = (TEXT **) calloc((ap_mxbases + 3) * 3, sizeof(TEXT *));
   vdb      = (TEXT  *) calloc((ap_mxbases + 3) , (2 + 10 + 3 + 40 + 2));

   if ( rec_base == NULL || vet_sel == NULL || vdb == NULL ) {
      ret = -1;
      goto sai;
   }

   jf = abre_funcoes(funcao("1 3 E"));
   jm = abre_mensagem(str_nada);

   conta = 0;
   inicia_em = 0;
   strcpy(chave, D0_ID);
   operador = '^';
   tam = strlen(chave);

   while ( 1 ) {

		if ( ! find_rec( ace_dnum, ace_inum, D0_ID, str_nada, operador) ||
           strncmp(ap_chave_ret, chave, tam) != 0 ) {
         break;
      }
      operador = 'N'; /* muda operador para procurar proximo (NEXT) */

      strncpy(base, &ap_chave_ret[tam], TB);

		if ( ! load_rec( ace_dnum, ace_inum, D0_ID, base,
                       rec_base, sizeof(Base), '=') ) {
         ret = -1;
         goto fim;
      }

      /* se usuario e gerente de Base lista so as bases
      ** que foram criadas por ele
      */

      if ( restrito && ap_status != 'A' ) {
         if ( strcmp(ap_nome, rec_base->bs_dono) != 0 )
            continue;
      }

      /* verifica se e ocorrencia a iniciar */
      if ( proposto[0] != '\0' ) {
         if (strcmp(proposto, base) == 0 )
            inicia_em = conta;
      }

		if ( ! load_rec( ace_dnum, ace_inum, D0_ID, base, rec_base, sizeof(Base), '=') ) {
         ret = -1;
         goto fim;
      }

      if ( rec_base->bs_desc[0] != '\0' )
         p_desc = rec_base->bs_desc;
   	else
         p_desc = R_NODES;


      push_item(vet_sel, conta, base, p_desc, '1', rodape);

      conta++;

      if ( conta > ap_mxbases ) {
         mens_erro(H_TOOBAS, E_TOOBA); /* muitas bases associadas ao usuario */
         ret = -1;
         goto fim;
      }
   }

   if ( conta >= 0 ) ret = 0;
   else              ret = -1;

fim:

   if ( ret == 0 ) {

      if ( pode_nova == PODE_NENHUMA || pode_nova == PODE_NOVA ) {
         if ( conta > 0 ) {
            push_item(vet_sel, conta, STR_TRACO, str_nada, '2', rodape);
            conta++;
         }

         if ( pode_nova == PODE_NOVA ) {
#ifdef RUNTIME
            push_item(vet_sel, conta, O_BNOVA, R_RTBNOVA, '1', rodape);
#else
            push_item(vet_sel, conta, O_BNOVA, R_BNOVA, '1', rodape);
#endif
            conta++;
         }
         else {
            push_item(vet_sel, conta, O_NENHU, R_NENHU, '1', rodape);
            conta++;
         }
      }


      tam = COTA(1, conta , (size_y - 4 - lin) );

      def_menu( &m0, 12, lin, 56, tam, 1, MP_CAMPO, MP_BARRA,
              SIMPLES, MP_JANELA, MP_DESTAQUE,
              (TEXT *(*)[]) vet_sel, T_MBASES, str_nada);

      m0.escolhido = inicia_em;
      m0.comando   = trata_ctle;

		ret = menu( &m0 );

		if ( ret >= 0 ) {

         if ( ret == ( conta - 1 ) )
            switch( pode_nova ) {
               case PODE_UMA     : strncpy(ret_aux, vet_sel[OP(ret)], 10); break;
               case PODE_NENHUMA : strncpy(ret_aux, NO_BASE, 10);          break;
               case PODE_NOVA    : strncpy(ret_aux, NEW_BASE, 10);         break;
            }
         else
            strncpy(ret_aux, vet_sel[OP(ret)], 10);

         ret_aux[10] = '\0';

			trim(ret_aux); /* nao tirar este trim */
		}
   }

sai :


   ct_semseguranca = salva_seg ;

	fecha_mensagem(jm);
	fecha_funcoes(jf);

   if ( vdb      != NULL ) free((TEXT *)vdb);
   if ( vet_sel  != NULL ) free((TEXT *)vet_sel);
   if ( rec_base != NULL ) free((TEXT *)rec_base);

   down_str(GRP_SBASE);
   win_ajuda = ajuda_ant;

	return((ret < 0 ) ? (TEXT *) NULL : ret_aux);
};




/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT varre_dx()
**#   Objetivo.: Analisar em cadastro de acessos quais as bases poderao
**#              ser acessadas por um usuario;
**#   Entrada..: ident - identificacao do usuario;
**#              grupo - identificacao do grupo;
**#              mensagem - mensagem de rodape
**#   Retorna..: (TEXT *)PONT - Nome da base selecionada
**#              (TEXT *)NULL - Erro - Nao conseguiu concluir analise
**#
**/
F__LOC   TEXT *varre_dx(ident, grupo, mensagem)
TEXT *ident, *grupo;
TEXT *mensagem;
{  COUNT ret, conta, i, tam, lin;
	TEXT  chave[TE+TE+2], *p_desc;
	static TEXT ret_aux[TE+1];
	Selecao m0;
   Base *rec_base;
   TEXT **vet_sel;
   WIN  *jf = NULL;

   ret = 0;

   rec_base = (Base *) calloc(1, sizeof(Base));
   vet_sel  = (TEXT **) calloc((ap_mxbases + 1) * 3, sizeof(TEXT *));
   vdb      = (TEXT  *) malloc((ap_mxbases + 1) * (2 + 10 + 3 + 40 + 2));

   if ( rec_base == NULL || vet_sel == NULL || vdb == NULL ) {
      ret = -1;
      goto sai;
   }

   if ( ap_status == 'A' )
      conta = scan_todas( vet_sel );
   else
      conta = scan_ident( vet_sel, ident, grupo);

   if ( conta < 0 ) {
      ret = -1;
      goto fim;
   }

   for ( i = 0 ; i < conta; i++) {

      if ( *(vet_sel[AT(i)]) == '2' ) continue;

		strncpy(chave, vet_sel[OP(i)], 10);
      chave[10] = '\0';
		trim(chave); /* nao tirar este trim */

		if ( ! load_rec( ace_dnum, ace_inum, D0_ID, chave,
                       rec_base, sizeof(Base), '=') ) {
         ret = -1;
         goto fim;
      }

      if ( rec_base->bs_desc[0] != '\0' )
         p_desc = rec_base->bs_desc;
   	else
         p_desc = R_NODES;

      push_item(vet_sel, i, chave, p_desc, '1', mensagem);

      if ( conta > ap_mxbases ) {
         mens_erro(H_TOOBAS, E_TOOBA); /* muitas bases associadas ao usuario */
         break;
      }

   }

   if ( conta > 0 )
      ret = 0;
   else  {
      decida( TW_SB00, str_nada, H_NOBASE, 2, 40, "%s\n%s", TW_SB01, TW_SB02);
      ret = -1;
   }

fim:

   if ( ret == 0 ) {

      jf = abre_funcoes(funcao("1 3 E"));

#ifndef RUNTIME
      push_item(vet_sel, conta, STR_TRACO,  str_nada, '2', mensagem);
      conta++;

      push_item(vet_sel, conta,   O_NENHU, R_NENHU, '1', mensagem);
      conta++;
#endif

      tam = MIN (conta, (size_y - 7));
      lin = ((size_y - 5) - tam) / 2;
      lin += 2;

      def_menu( &m0, 12, lin, 56, tam, 1, MP_CAMPO, MP_BARRA,
				    1, MP_JANELA, MP_DESTAQUE,
                (TEXT *(*)[]) vet_sel, T_MBASES, str_nada);

      m0.comando   = trata_ctle;

		ret = menu( &m0 );

		if ( ret >= 0 ) {

#ifndef RUNTIME
         if ( ret == ( conta -1 ) )
            strcpy(ret_aux, NO_BASE);
         else
            strncpy(ret_aux, vet_sel[OP(ret)], 10);
#else
         strncpy(ret_aux, vet_sel[OP(ret)], 10);
#endif
         ret_aux[10] = '\0';
			trim(ret_aux); /* nao tirar este trim */
		}
   }


sai:

	fecha_funcoes(jf);

   if ( vdb      != NULL ) free((TEXT *)vdb);
   if ( vet_sel  != NULL ) free((TEXT *)vet_sel);
   if ( rec_base != NULL ) free((TEXT *)rec_base);

	return((ret < 0 ) ? (TEXT *) NULL : ret_aux);
};




F__LOC   COUNT scan_ident(vet_sel, ident, grupo)
TEXT *vet_sel[];
TEXT *ident, *grupo;
{  COUNT conta, repassa, pode_tracar, salva_seg;
   COUNT operador, tam, i;
   TEXT  base_oito[TE + 1];
	TEXT  base[TE+1], chave[TE+TE+2], aux[TE+1];

   RECALFIL(ace_dnum);
   RECALFIL(ace_inum);

   salva_seg = ct_semseguranca;
   ct_semseguranca = 1; /* para fazer com que find_rec consiga achar um proximo */

   conta = 0;
   repassa = 0;
   pode_tracar = 0;

volta :

   if ( repassa == 0 ) {
      strcpy(chave, D2_ID);
	   strcat(chave, ident);
   }
   else if (repassa == 1) {
      strcpy(chave, D2_ID);
	   strcat(chave, grupo);
   }
   else {
      goto fim;
   }

   operador = '^';

   tam = (COUNT) strlen(chave);

   while ( 1 ) {

		if ( ! find_rec( ace_dnum, ace_inum, D2_ID, &chave[2], operador) ||
           strncmp(ap_chave_ret, chave, tam) != 0 ) {
         break;
      }

      operador = 'N';

      if ( ap_chave_ret[tam] != '!' ) break;

      strncpy(base, &ap_chave_ret[tam+1], TE+1);

      if ( repassa == 1 ) { /* so para grupo */
         for ( i = 0; i < conta; i++ ) {
            strncpy(aux, vet_sel[OP(i)], 10);
            aux[10]='\0';
            trim(aux);  /* nao tirar este trim */
				if ( strcmp(aux, base) == 0 ) break;
         }
         if ( i < conta ) /* achou base na tabela */
            continue;
      }


      if ( repassa == 0 ) pode_tracar = 1;

      if ( repassa == 1 && pode_tracar == 1) {
         pode_tracar = 0;
         push_item(vet_sel, conta, STR_TRACO, str_nada, '2', str_nada);
         conta++;
      }

		sprintf(base_oito, STR_8PT8S, base);

      push_item(vet_sel, conta, base_oito,  str_nada, '1', str_nada);

      conta++;

      if ( conta > ap_mxbases ) {
         mens_erro(H_TOOBAS, E_TOOBA); /* muitas bases associadas ao usuario */
         conta = -1;
         goto fim;
      }
   }

   repassa++;
   goto volta;

fim :

   ct_semseguranca = salva_seg ;

   return( conta );
}


F__LOC   COUNT scan_todas( vet_sel )
TEXT *vet_sel[];
{  COUNT operador, tam, conta, salva_seg;
   TEXT  base_oito[TE + 1];
	TEXT  base[TE+1], chave[TE+TE+2];

   RECALFIL(ace_dnum);
   RECALFIL(ace_inum);

   salva_seg = ct_semseguranca;
   ct_semseguranca = 1; /* para fazer com que find_rec consiga achar um proximo */

   conta = 0;


   strcpy(chave, D0_ID);
   tam = (COUNT)strlen(chave);
   operador = '^';

   while ( 1 ) {

		if ( ! find_rec( ace_dnum, ace_inum, D0_ID, str_nada, operador) ||
           strncmp(ap_chave_ret, chave, tam) != 0 ) {
         break;
      }

      operador = 'N'; /* muda operador para procurar proximo (NEXT) */

      strncpy(base, &ap_chave_ret[tam], TB);

		sprintf(base_oito, STR_8PT8S, base);

      push_item(vet_sel, conta, base_oito, str_nada, '1', str_nada);

      conta++;

      if ( conta > ap_mxbases ) {
         mens_erro(H_TOOBAS, E_TOOBA); /* muitas bases associadas ao usuario */
         conta = -1;
         goto fim;
      }
   }

fim :

   ct_semseguranca = salva_seg;

   return( conta );
}

F__LOC   VOID push_item(vet, ind, base, desc, status, rodape)
TEXT *vet[];
COUNT ind;
TEXT *base, *desc, status, *rodape;
{  TEXT *p, *s;

   s = vdb + (ind * (2 + 10 + 3 + 40 + 2));
   p = s + 2;

	vet[OP(ind)] = p;
	vet[AT(ind)] = s;
	vet[AJ(ind)] = rodape;

   sprintf(p, "%-10.10s - %-40.40s", base, desc);

   vet[AT(ind)][0] = status; vet[AT(ind)][1] = '\0';

   strncpy(vet[OP(ind)], p, TE);
}

F__LOC   COUNT trata_ctle(s)
Selecao *s;
{  COUNT ret, i;
   TEXT aux[10], base[10], *p;



	ret = 0;

   if ( s->tecla == 0x5 ) {
      aux[0] = '\0';
      ret = pede_cadeia(T_BASEESP, aux, TB, R_BASEESP, 'X', NULL);
      fwrefresh(curr_win);
      if ( ret == K_ESC || ret == -1) {
         ret = -1;
      }
      else {
         ret = 0;
         sprintf(base, "%-8.8s", aux);
         for ( i = 0; i < s->n_elem; i++) {
            p = (*s->opcoes)[OP(i)];
            if ( strncmp(base, p, TB) == 0 ) {
               s->escolhido = i;
               ret = -3;
               break;
            }
         }
      }
   }

	return( ret );
}

