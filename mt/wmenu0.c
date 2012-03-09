
#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  "window.h"
#include  "menu.h"

#ifdef     MSDOS

ST( Selecao  * ) define( Selecao *, SS, SS, SS, SS, SS,
                         SS, SS, SS, SS, SS, SC *(* )[], SC *, SC * );

static SS nada1 ( Selecao * );   /* Nao pode ser NEAR */
static SS nada2 ( Selecao * );   /* Nao pode ser NEAR */

#else

ST( Selecao  * ) define( );

static SS nada1 ( );   /* Nao pode ser NEAR */
static SS nada2 ( );   /* Nao pode ser NEAR */

#endif

static SS nada1 ( s ) /* Nao pode ser NEAR */

Selecao  *s;
{
   s = s;  return( 0 );
}

static SS nada2 ( s ) /* nao pode ser NEAR */

Selecao  *s;
{
   s = s;  return( -1 );
}

Selecao  *def_menu( s, col, lin, a_col, a_lin, num_col, l_atr, t_atr,
                    borda, b_atr, d_atr, opcoes, titulo, rodape )

Selecao      *s;
SS           col, lin, a_col, a_lin, num_col;
SS           l_atr, t_atr, borda, b_atr, d_atr;
SC           *(*opcoes)[];
SC           *titulo, *rodape;
{
   s->ok   = 0;
   s->tipo = MENU;

   return ( define( s, col, lin, a_col, a_lin, num_col, l_atr,
                    t_atr, borda, b_atr, d_atr, opcoes, titulo, rodape ) );
}


Selecao  *def_escolha( s, col, lin, a_col, a_lin, num_col, l_atr, t_atr,
                       borda, b_atr, d_atr, opcoes, titulo, rodape )

Selecao      *s;
SS           col, lin, a_col, a_lin, num_col;
SS           l_atr, t_atr, borda, b_atr, d_atr;
SC           *(*opcoes)[];
SC           *titulo, *rodape;
{
   s->ok   = 0;
   s->tipo = ESCOLHA;

   return ( define( s, col, lin, a_col, a_lin, num_col, l_atr,
                    t_atr, borda, b_atr, d_atr, opcoes, titulo, rodape ) );
}


ST( Selecao  * )  define( s, col, lin, a_col, a_lin, num_col, l_atr, t_atr,
                          borda, b_atr, d_atr, opcoes, titulo, rodape )

Selecao      *s;
SS           col, lin, a_col, a_lin, num_col;
SS           l_atr, t_atr, borda, b_atr, d_atr;
SC           *(*opcoes)[];
SC           *titulo, *rodape;
{
   SC    *P;

   s->opcoes = opcoes;

   /*** Cuidado com a quantidade de Colunas "num_col" pois e' UC ***/
   for ( s->n_elem = 0,  s->t_opcao = 1;  s->n_elem < 4196;  s->n_elem++ )
   {
      P = (*opcoes)[ s->tipo == MENU ? OP( s->n_elem ) : s->n_elem ];

      if ( P  ==  NULL )   break;

      s->t_opcao = MAX ( s->t_opcao,  strlen( P ) );
   }

   s->t_opcao = COTA( 1, s->t_opcao, VIDEOCOL - 6 );

   s->num_col = COTA( 1, num_col,  s->n_elem );
   s->v_col   = s->num_col * (s->t_opcao + 1) + 1;
   s->v_lin   = s->n_elem / s->num_col;

   if ( s->v_lin * s->num_col < s->n_elem )   s->v_lin++;

   s->col     = col;
   s->lin     = lin;
   s->a_lin   = COTA( 1, a_lin, s->v_lin );
   s->a_col   = COTA( 1, a_col, s->v_col );

   s->escolhido = 0;
   s->tarja     = 0;
   s->tecla     = 0;
   s->l_atr     = l_atr;
   s->t_atr     = t_atr;
   s->borda     = borda;
   s->b_atr     = b_atr;
   s->d_atr     = d_atr;
   s->men       = NULL;
   s->comando   = nada1;
   s->executa   = nada2;
   s->titulo    = titulo;
   s->rodape    = rodape;
   s->ajuda     = 0;
   s->ok        = MENU_OK;

   return ( s );
}


VOID ajuda_menu( s, op_ajuda )

Selecao    *s;
SC         *(*op_ajuda)[];
{
   s->ajuda = 1;

   s->op_ajuda = op_ajuda;
}

