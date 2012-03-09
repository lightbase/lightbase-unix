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
#include  "lb2entra.h"
#include  "lb2perso.h"
#include  "lb2pesq.h"
#include  "lb2prot1.h"



#define   DELI_CAMPO '['
#define   DELI_FCAMPO ']'

#define   END  0
#define   CON  1
#define   PAL  2
#define   NIV  3
#define   PES  4

/*** Tamanho minimo do nome ***/
#define  TAM_NOME  5
#define  MAXLINE 230




#define   H_AE_01     "E_AE_01"
#define   H_AE_02     "E_AE_02"
#define   H_AE_03     "E_AE_03"
#define   H_AE_04     "E_AE_04"
#define   H_AE_05     "E_AE_05"
#define   H_AE_06     "E_AE_06"
#define   H_AE_07     "E_AE_07"
#define   H_AE_08     "E_AE_08"


struct   ordem
{
   SC    *nome;
   SS     tipo;
   SS     nivel;
   struct  ordem  *prox;
};

typedef struct ordem ordem;

static ordem *inicio = (ordem *) 0;
static ordem *final  = (ordem *) 0;


#ifdef MSDOS
   COUNT tab_pesquisa(Tab *);
   COUNT edt_psq(COUNT, TEXT *);
   COUNT tab_parcial( TEXT *, TEXT *, TEXT *);
   COUNT val_operador(TEXT *);
   COUNT val_argumento(TEXT *);
   COUNT vf_arg(d_entry *);
   COUNT vf_cppsq(d_entry *);
   COUNT vf_conector(d_entry *);
   ordem  *poe_ordem( SC *, SS, SS );
   SS      get_token( SC *, SC **, SS );
   SS      parse( SC * );
   SS      mk_pesquisa( VOID );
   VOID    libera( VOID );
   COUNT    le_pesquisa(COUNT, TEXT *);
#else
   COUNT tab_pesquisa( );
   COUNT edt_psq( );
   COUNT tab_parcial( );
   COUNT val_operador( );
   COUNT val_argumento( );
   COUNT vf_arg( );
   COUNT vf_cppsq( );
   COUNT vf_conector( );
   ordem  *poe_ordem( );
   SS      get_token( );
   SS      parse( );
   SS      mk_pesquisa( );
   VOID    libera( );
   COUNT le_pesquisa( );
#endif





/* retorna :
   -1 : sai da pesquisa
    0 : Muda para pesquisa por argumentos
    > 0 : executa a pesquisa numero : retorno

    D--> Display
    S--> Selecao

*/

F__GLB   COUNT psq_tabela(jm, jf, t )
WIN  *jm, *jf;
Tab *t;
{  COUNT ret;
	TEXT  lin_fun[100];
	UTEXT str_escape[20];
   TEXT *ajuda_ant = win_ajuda;

   win_ajuda = "PSQ_TABELA";

   funcoes(jf, funcao("1 2 3 6 E"));
   mens_1(jm, M_PSQ_TB1);

#ifdef CP_FLAG
	pr_escape("1 2 3 0 TBE", str_escape);  /* So CP_FLAG pode 0 */
#else
	pr_escape("1 2 3 TBE", str_escape);  /* So CP_FLAG pode 0 */
#endif

	strncpy(lin_fun, funcao("1 2 3 E"), 99);

   t->mens_alt  = M_PSQ_TB2;
	t->fun_alt   = lin_fun;
   t->escape    = str_escape;
	t->executa   = tab_pesquisa;
   t->edita     = ALTERA;

   tabela(t);

   switch ( t->tecla ) {

      case K_ESC :   /* abandonar pesquisa */
                     ret = -1;
                     goto fim;

      case K_TAB :
      case K_BTAB:   /* mudar para pesquisa por argumentos */
                     ret =  0;
                     goto fim;

      case K_ENTER:
      case K_F2   :  /* executar pesquisa */
                     ret = t->escolhido + 1;
                     goto fim;
   }

fim :

   win_ajuda = ajuda_ant;

   fwsetcursor( t->win );
   return(ret);
}



F__GLB   COUNT exec_tabela(numero, str)
COUNT numero;
TEXT *str;
{
   COUNT ind_vet, lista;
   LONG regs;
   TEXT sentenca[250];

   lista = 0;

   psq_metacarac = 0;  /* desabilita visualizacao de palavras na janela */

   ind_vet = numero - 1;

   if ( hot_key == 0 )
      push_brilha(INICIALIZA, NULL); /* NULL vai virar str_brilhar */

   if ( push_palavra(INICIALIZA, str_nada) < 0 ) {
      lista = 0;
      goto fim;
   }

   if ( str == NULL ) {
      strcpy(sentenca, &ap_tabpsq[ind_vet][TAM_PSQPROT]);
      trim(sentenca);
   }
   else {
      strcpy(sentenca, str);
      trim(sentenca);
   }


   if (sentenca[0] == '\0' ) {
      lista = 0;
      goto fim;
   }

   if ( parse(sentenca) > 0 ) {
      lista = mk_pesquisa();
      libera();
   }

   if ( lista > 0 ) {
      conta_docs( lista ); /* conta o numero de documentos */
      regs = vet_lista[lista].tot_doc;
   }
   else {
      regs = 0;
   }

   if ( str == NULL ) {
      sprintf(ap_tabpsq[ind_vet], TAB_FMT, ind_vet + 1, regs, C_PSQ_EM,
            data_hora('D', 0L),  sentenca);

      ap_tabctl[ind_vet] |= HIST_MODI;
   }

fim :

   push_palavra(LBFIM, str_nada);

   return(lista);
}




F__LOC	 ordem  *poe_ordem( nome, tipo, nivel )
SC  *nome;
SS   tipo, nivel;
{
   SS     tam;
   ordem  *ptr;

   tam = strlen( nome ) + 1;
   if ( tam < TAM_NOME )   tam = TAM_NOME;

   ptr = (ordem *) malloc( sizeof( ordem ) );

   if ( ptr != NULL ) {
      ptr->nome = (SC *) malloc( tam );
      if ( ptr->nome != NULL )
         strcpy( ptr->nome, nome );
      ptr->tipo = tipo;
      ptr->nivel = nivel;
      ptr->prox = (ordem *) 0;

      if ( inicio == (ordem *) 0 )  inicio = ptr;
      if ( final  != (ordem *) 0 )  final->prox = ptr;
   }


   return( final = ptr );
}


F__LOC	 SS    get_token( str_inic, str_fim, nivel )
SC  *str_inic;
SC  **str_fim;
SS   nivel;
{
   SS     I, num_op;
   SC    *P;
   SC    simbolo[ MAXLINE + 1 ];

   str_fim = str_fim;  /*** Evitar warnings ***/

   for ( P = str_inic;   *P == ' ' || *P == '\t';   P++ )
      /* Nada */;

   if ( *P == '\0' ) {  *str_fim = P;  return( END );  }

   if ( *P == '(' || *P == ')' ) {  *str_fim = P;  return( NIV );  }

   for( I = 0;
        *P != '(' && *P != ')' && *P != '\0' && *P != ' ' && *P != '\t';
        I++, P++ )
   {
      if ( I == MAXLINE )  break;

      simbolo[ I ] = *P;
   }

   simbolo[ I ] = '\0';

   *str_fim = P;

   if ( (num_op = val_operador( simbolo )) >= 0 )
   {
      simbolo[0] = num_op;
      simbolo[1] = '\0';
      poe_ordem( simbolo, CON, nivel );
      return( CON );
   }

   else
   {
      poe_ordem( simbolo, PAL, nivel );
      return( PAL );
   }
}


F__LOC	 SS    parse( str )
SC  *str;
{
   SS   in_nivel = 1;
   SS   I, nivel, primeiro;
   SC  *S1;
   SC  token;

   for ( primeiro = 0, nivel = in_nivel, I = PAL, token = PAL, S1 = str;
         I != END;    )
   {
      I = get_token( S1, &S1, nivel );

      if ( primeiro == 0 )  primeiro++;

      if ( token == PAL )
      {
         switch( I )
         {
         case PAL :  if ( val_argumento(final->nome)) {
                        mens_erro(H_AE_08, E_AE_08, final->nome);
                        return( -1 );
                     }
                     token = CON;
                     break;
         case NIV :
                     if ( *S1++ == ')' ) {
                        mens_erro(H_AE_01, E_AE_01);
                        return( -1 );
                     }
                     else
                        ++nivel;
                     break;
         case CON :
                     mens_erro(H_AE_02, E_AE_02);
                     return( -2 );
         case END :
                     if ( nivel != in_nivel || primeiro == 0 )
                     {
                        mens_erro(H_AE_03, E_AE_03);
                        return( -3 );
                     }
                     return( 1 );
         }
      }

      else  /*** token == CON ***/
      {
         switch( I )
         {
         case CON :
                     token = PAL;
                     break;
         case NIV :
                     if ( *S1++ == '(' )
                     {
                        mens_erro(H_AE_04, E_AE_04);
                        return( -4 );
                     }
                     else if ( nivel == 1 )
                     {
                        mens_erro(H_AE_05, E_AE_05);
                        return( -5 );
                     }
                     else                   --nivel;
                     break;
         case PAL :
                     mens_erro(H_AE_06, E_AE_06);
                     return( -6 );
         case END :
                     if ( nivel != in_nivel || primeiro == 0 )
                     {
                        mens_erro(H_AE_07, E_AE_07);
                        return( -7 );
                     }
                     return( 1 );
         }
      }
   }

   /*** Parece que apenas para evitar warnings ***/
   return( 2 );
}


F__LOC	 SS mk_pesquisa( )
{
   SS       I, max, C;
   ordem     *Q, *S, *T;

   C = -1;

   while ( 1 )
   {
      /*** Ache o primeiro maior ***/
      for ( max = 0, T = inicio;   T != (ordem *) 0;   T = T->prox )
      {
         if ( T->nivel > max ) {  max = T->nivel;  S = T;  }
      }

      if ( max == 0 )  break;

      /*** Quantos do mesmo nivel do maior sao consectutivos ***/
      for ( I = 0, T = S; T != (ordem *) 0 && T->nivel == max; T = T->prox )
      {
         I++;
      }

      if ( I == 1 )
      {
         /* Se existir parentesis em apenas uma palavra diminua seu nivel */
         /* Exemplo 'Jairo e (magda)'. */
         /* Os parentesis serao reduzidos a uma unica pesquisa */
         /* Exemplo 'Jairo e (magda e juju)'. */
         if ( max > 1 )
         {
            S->nivel--;
            continue;
         }
         /* Se apenas existir uma palavra de nivel minimo ... */
         /* ... pesquize e encerre. */
         /* Exemplo 'Jairo' */
         else /* max == 1 */
         {
            C = tab_parcial( S->nome, NULL, NULL );
            break;
         }
      }

      if ( I < 3 ) { numdebug(650); return( -1 ); }

      /*** 'S' esta posicionado no primeiro, e tem no minimo 3 ***/
      T = S->prox;
      Q = T->prox;
      C = tab_parcial( S->nome, T->nome, Q->nome );

      /*** Lista Valida ? ***/
      if ( C < 0 )  break;

      /*** Muda nome da palavra para uma lista ja pesquisada ***/
      S->tipo = PES;
      S->nome[ 0 ] = ' ';
      S->nome[ 1 ] = C;    /*** No. da lista ***/
      S->nome[ 2 ] = '\0';

      /*** Retira 'T' e 'Q' da ordem de itens ***/
      S->prox = Q->prox;

      if ( final == Q )  final = S;

      /*** libere areas ***/
      free( T->nome );   free((TEXT *) T );
      free( Q->nome );   free((TEXT *) Q );
   }

   return ( C );
}


F__LOC	 VOID libera( )

{
   SS     I;
   ordem  *T;

   for ( I = 1;   (T = inicio) != (ordem *) 0;   I++ )
   {
      free( T->nome );
      inicio = T->prox;
      free( (TEXT *)T );
   }

   final = inicio;
}


F__LOC   COUNT val_operador(str)
TEXT *str;
{  COUNT ret;
   d_entry gato;

   gato.str   = str;
   gato.tecla = 0;

   if ( vf_conector(&gato) == ACEITA ) {
      ret = gato.tecla;
   }
   else ret = -1;

fim :

   return(ret);
}

F__LOC   COUNT val_argumento(str)
TEXT *str;
{  COUNT ret, tam;
   TEXT campo[TD + 1];
   TEXT palavra[LB_KEYMAX + 1];
   d_entry gato;

   piece(str, palavra, DELI_CAMPO, 1, LB_KEYMAX);
   piece(str, campo, DELI_CAMPO, 2, TD);
   trim(palavra);
   trim(campo);


   tam = strlen(palavra);
   if ( tam > 0 ) {
      gato.str   = palavra;
      gato.tecla = 0;

      if ( vf_arg(&gato) != ACEITA ) {
         ret = -1;
         goto fim;
      }
   }


   tam = strlen(campo);
   if ( tam > 0 && campo[tam-1] == DELI_FCAMPO ) {
      campo[tam-1] = '\0';
   }

   gato.str   = campo;
   gato.tecla = 0;
   gato.lista = palavra;

   if ( vf_cppsq(&gato) != ACEITA ) {
      ret = -1;
      goto fim;
   }


   ret = 0;

fim :

   return(ret);
}


F__LOC	 COUNT tab_parcial(oper1, str_conec, oper2)
TEXT *oper1, *str_conec, *oper2;
{  TEXT campo[TD + 1];
   TEXT palavra[LB_KEYMAX + 1];
   TEXT *p_arg;
   COUNT lista, lista1, lista2, tam, conec;

   lista = 0;

   if ( str_conec == NULL ) conec = -1;
   else                     conec = str_conec[0];

   if ( conec < 0 ) {
      if ( oper1 == NULL || oper2 != NULL )
         numdebug(651);
   }




   /*------------------------------------------------ 1o  argumento ------*/
   lista = 0;
   p_arg = oper1;

   if ( p_arg != NULL ) {
      if ( p_arg[0] == ' ' ) {
         lista = p_arg[1];
         if ( lista >= NUM_LISTAS ) {
            numdebug(652);
            lista = -1;
         }
      }
      else {
         piece(p_arg, palavra, DELI_CAMPO, 1, LB_KEYMAX);
         piece(p_arg, campo, DELI_CAMPO, 2, TD);
         trim(palavra);
         trim(campo);
         tam = strlen(campo);
         if ( campo[tam-1] == DELI_FCAMPO ) {
            campo[tam-1] = '\0';
         }
         if ( palavra[0] != '\0' ) {
            lista = carregar(palavra, campo);
         }
      }
   }

   if ( lista < 0 || conec < 0 ) {
      goto fim;
   }

   lista1 = lista;

   /*------------------------------------------------ 2o  argumento ------*/
   lista = 0;
   p_arg = oper2;

   if ( p_arg != NULL ) {
      if ( p_arg[0] == ' ' ) {
         lista = p_arg[1];
         if ( lista >= NUM_LISTAS ) {
            numdebug(653);
            lista = -1;
         }
      }
      else {
         piece(p_arg, palavra, DELI_CAMPO, 1, LB_KEYMAX);
         piece(p_arg, campo, DELI_CAMPO, 2, TD);
         trim(palavra);
         trim(campo);
         tam = strlen(campo);
         if ( campo[tam-1] == DELI_FCAMPO ) {
            campo[tam-1] = '\0';
         }
         if ( palavra[0] != '\0' ) {
            lista = carregar(palavra, campo);
         }
      }
   }

   if ( lista < 0 ) {
      goto fim;
   }

   lista2 = lista;

   /*------------------------------------------------   operador  ------*/


   lista = combina(lista1, lista2, conec);

fim :

   return(lista);
}




F__LOC   COUNT tab_pesquisa(tp)
Tab *tp;
{  TEXT aux[MAXLINE + 1];
	COUNT ret;

   ret = tp->escolhido;

   switch (tp->tecla ) {
		case K_DEL  :  numdebug(654); break;
		case K_INS  :  numdebug(655); break;

      case K_ESC :   /* Abandonar pesquisa */
                     ret = -3;
						   break;

      case K_ENTER :
                     strcpy(aux, &ap_tabpsq[tp->escolhido][TAM_PSQPROT]);
                     trim(aux);
		               if (aux[0] == '\0') {
                        w_ungc(K_HOME);
                        w_ungc(K_F6);
                        ret = tp->escolhido;
                     }
                     else
                        ret = -3;

                     break;

		case K_F2    :
                     ret = -3; /* executa pesquisa */
	                  break;

#ifdef CP_FLAG
		case K_F0  :  /*  pesquisa por tela */
                     le_pesquisa(BASE_EDIT, &tp->e->str[TAM_PSQPROT]);
#endif

		case K_F6   :  /* Alterar pesuisa */
							edt_psq(tp->escolhido, tp->e->str);
                     if ( parse(&tp->e->str[TAM_PSQPROT]) < 0 ) {
                        w_ungc(K_F6);
                     }
                     libera();
                     break;

		case K_BTAB :
		case K_TAB  :  ret = -3; /* escape desta tabela */
	                  break;

		default      : numdebug(656);
                     break;
   }


fim :

   fwsetcursor( tp->win );
   return(ret);
};


F__LOC	 COUNT edt_psq(ind_vet, str_psq)
COUNT ind_vet;
TEXT *str_psq;
{  COUNT ret;

   ret = ind_vet;

   if ( ap_tabpsq[ind_vet] == NULL ) {
      numdebug(657);
      goto fim;
   }

   if ( strcmp(ap_tabpsq[ind_vet], str_psq) == 0 ) {
      goto fim;
   }

   sprintf(ap_tabpsq[ind_vet], TAB_FMT, ind_vet + 1, 0L, C_PSQ_EM,
      "  /  /  ", &str_psq[TAM_PSQPROT]);

   ap_tabctl[ind_vet] = TAB_MODI;

fim :

   return(ret);
}


