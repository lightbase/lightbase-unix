#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  "window.h"
#include  "tabela.h"
#include  "mensagem.h"

#ifdef      MSDOS

ST( SS )   acrescenta( Tab *, SC * );
ST( SS )   retira( Tab *, SS, SC * );
ST( SS )   insere( Tab *, SS, SC * );
ST( VOID ) tarja ( Tab *, SS );
ST( VOID ) lista ( Tab *, SC * );
ST( VOID ) a_lista ( Tab *, SC * );
ST( SS )   escolhe ( Tab *, SS );

#else

ST( SS )   acrescenta( );
ST( SS )   retira( );
ST( SS )   insere( );
ST( VOID ) tarja ( );
ST( VOID ) lista ( );
ST( VOID ) a_lista ( );
ST( SS )   escolhe ( );

#endif

SC *tab_ultimo;   /* vai ser global para executa poder saber se e edicao */

static SS executa( t ) /* Nao pode ser NEAR */

Tab   *t;
{
   t = t;

   switch ( t->tecla )
   {
   case K_F6  :
   case K_INS :
   case K_DEL :   return ( -2 );

   default    :   return ( -3 );
   }
}


static SS comando( t ) /* Nao pode ser NEAR */

Tab   *t;
{
   t = t;

   return ( -2 );
}


ST( SS ) acrescenta( t, ultimo )

Tab   *t;
SC    *ultimo;
{
   if ( t->n_elem >= t->n_max )  return( 0 );

   ultimo[ 0 ] = '\0';

   ( *t->opcoes )[ t->n_elem ] = ultimo;

   return( ++t->n_elem );
}


ST( SS )  retira( t, item, ultimo )

Tab   *t;
SS   item;
SC    *ultimo;
{
#ifdef   NAO_DEF
   Estava assim e nao conseguiamos deletar o ultimo arquivo do MEDIT
   if ( item  >=  t->n_elem - 1 )  return( 0 );

   for (  ;  item  <  t->n_elem;  item++ )
   {
      ( *t->opcoes )[ item ] = ( *t->opcoes )[ item + 1 ];
   }

   return( --t->n_elem );
#endif

#ifdef   NAO_DEF
   Assim  conseguiamos deletar o ultimo arquivo do MEDIT
      porem nao apagava da tabela
   if ( item  <  t->n_elem - 1 )
   {
      for (  ;  item  <  t->n_elem;  item++ )
      {
         ( *t->opcoes )[ item ] = ( *t->opcoes )[ item + 1 ];
      }
   }

   return( --t->n_elem );
#endif

   for (  ;  item  <  t->n_elem;  item++ )
   {
      ( *t->opcoes )[ item ] = ( *t->opcoes )[ item + 1 ];
   }

   --t->n_elem;

/* para tratar exclusao do ultimo */
   if ( t->n_elem == (t->n_max -1) && ( *t->opcoes )[ t->n_elem - 1] != ultimo ) {
      ( *t->opcoes )[ t->n_elem ] = ultimo;
      ultimo[0] = 0;
      ++t->n_elem;
   }

   return( t->n_elem );
}


ST( SS ) insere( t, item, ultimo )

Tab   *t;
SS    item;
SC    *ultimo;
{
   SS i, j;

   if ( item  >=  t->n_elem )    return( 0 );

   if ( t->n_elem >= t->n_max )  return( 0 );

   for ( i = t->n_elem, j = item;  j < t->n_elem;  i--, j++ )
   {
      ( *t->opcoes )[ i ] = ( *t->opcoes )[ i - 1 ];
   }

   ultimo[ 0 ] = '\0';

   ( *t->opcoes )[ item ] = ultimo;

   return( ++t->n_elem );
}


ST( VOID ) tarja ( t, atr )

Tab   *t;
SS       atr;
{
   SS    i, col, lin, achou;
   SC   *p;

   t->win->w_atrib = atr;

   lin = t->tarja;
   col = 1;

   /* Para garantir visualizacao adequada dos itens do menu */
   fwat( col + t->t_opcao + 1, lin, t->win );

   fwat( col, lin, t->win );

   for ( i = achou = 0, p = ( *t->opcoes )[ t->tarja ];
         i < t->t_opcao  &&  *p != '\0';       i++, p++ )
   {
      if ( !achou  &&  *p >= 'A' && *p <= 'Z' && t->mascara[ i ] != 'P' )
      {
         achou++;

         if ( atr != t->t_atr )  t->win->w_atrib = t->d_atr;

         fwpc( *p, t->win );
         t->win->w_atrib = atr;
      }

      else   fwpc( *p, t->win );
   }

   fwat( col, lin, t->win );
}


ST( VOID ) lista ( t, ultimo )

Tab   *t;
SC    *ultimo;
{
   t->win->w_atrib = t->l_atr;

   fwat( 0, 0, t->win );

   fwcls( t->win );

   ultimo[ 0 ] = '\0';

   for( t->tarja = 0;  t->tarja < t->n_elem;  t->tarja++ )
   {
      tarja ( t, T_OFF );
   }

   t->tarja = 0;   tarja ( t, T_OFF );
}


ST( VOID ) a_lista ( t, ultimo )

Tab   *t;
SC    *ultimo;
{
   SS  tr, ax, ay;

   tr = t->tarja;
   ax = t->win->w_ax;
   ay = t->win->w_ay;

   lista( t, ultimo );

   t->tarja     = tr;
   t->win->w_ax = ax;
   t->win->w_ay = ay;
}


ST( SS ) escolhe ( t, ch )

Tab      *t;
SS       ch;
{
   SS    i, j;
   SC    *p;

   for ( i = 0, ch = toupper( ch );  i < t->n_elem;  i++ )
   {
      for ( j = 0,  p = (*t->opcoes)[ i ];
            j < t->t_opcao  &&  *p != '\0';   j++, p++ )
      {
         if ( *p >= 'A' && *p <= 'Z'  &&  t->mascara[ j ] != 'P' )
         {
            if ( ch  ==  *p )
            {
               t->escolhido = i;
	            tarja ( t, T_OFF );
	            t->tarja = t->escolhido;
	            tarja ( t, T_ON );

               return ( K_ENTER );
            }

            break;
         }
      }
   }

   return ( ch );
}



SS tabela( t )

Tab  *t;
{
   SS         C, i, tmp, E, abriu_win;
   SC         *pm, *pa, *ultimo, inserindo, editmp[ MAX_EDIT + 1 ];
   d_entry    e;

   if ( t->ok != MENU_OK )  return( -1 );

   pa = win_ajuda;

   t->e = &e;

   if ( t->win == NULL )
   {
      t->win = win_make( t->v_col, t->v_lin,
                         t->a_col, t->a_lin, t->col, t->lin);
      abriu_win = 1;
   }

   else   abriu_win = 0;

   if ( t->win == NULL )   return( -1 );

   t->win->w_flags = W_VISIB + W_HIGH;
   t->win->type    = t->borda;

   t->win->w_optatr = t->t_atr;
   t->win->campoatr = t->l_atr;
   t->win->w_boxatr = t->b_atr;
   t->win->t_atr    = t->b_atr;

   if ( t->borda )
   {
      if ( *t->titulo  ) {  t->win->title  = t->titulo;  }
      if ( *t->rodape  ) {  t->win->bottom = t->rodape;  }
      fwbox( t->win );
   }

   t->win->w_curstyle = CS_OFF;

   if ( (ultimo = malloc( MAX_EDIT  + 1))  ==  NULL )
      ultimo = editmp;

   tab_ultimo = ultimo;

   if ( (t->edita & INSERE)  !=  0 )   acrescenta( t, ultimo );

   t->tecla = 0;

loop:

   lista ( t, ultimo );

   do  {

      t->escolhido = COTA( 0, t->escolhido, t->n_elem - 1 );

      tarja ( t, T_OFF );

      t->tarja = t->escolhido;

      tarja ( t, T_ON );

      fwrefresh(   t->win );
      fwsetcursor( t->win );

      if ( t->comando != NULL )   C = ( *t->comando )( t );

      else   C = -2;   /*** Retorno padrao de comando. Leia do teclado ***/

      tab_ultimo = ultimo; /* em tabela que chama outra tabela pode alterar */

      if ( C == -1 ) {  t->tecla = C = K_ENTER;  goto sai;  }

      else if ( C == -3 )  goto  fim;

      else if ( C >=  0 )
      {
         a_lista ( t, ultimo );
         C = t->tecla;
      }

      else
      {
         inserindo = 0;

         if ( t->escolhido == t->n_elem - 1  &&
              (t->edita & INSERE) != 0 &&
              (*t->opcoes )[ t->n_elem - 1] == ultimo )

         {
            inserindo = 1;
            t->tecla = C = K_F6;
         }

         if ( inserindo == 0 ) {   t->tecla = C = fwnqgc( t->win );  }
      }

      while ( 1 )
      {
         switch ( C ) {

         case K_F6 :
                        if ( !inserindo && !( t->edita & ALTERA ) )  break;
                        tmp = t->escolhido;

                        if( t->escolhido == t->n_elem - 1 ) pm = t->mens_ins;
                        else                                pm = t->mens_alt;

                        strcpy( editmp, (*t->opcoes)[t->escolhido] );

                        d_entrada( &e, t->win, editmp,
                                   t->t_opcao, t->mascara, pm, 1,
                                   t->escolhido, E_ALFA | E_CONFIRMA );

                        t->fun = abre_funcoes( t->fun_alt );
                        e.escape = t->escape;
                        E = entrada( &e, 0 );
                        fecha_funcoes( t->fun );
                        win_ajuda = pa;

                        if ( ! e.mudou )
                        {
                           a_lista( t, ultimo );
                           if ( E == K_ENTER )    break;
                           if ( E == K_ESC   )
                           {
                              if ( ! inserindo )  break;
                              else if ( t->escolhido != 0 )  E = K_UP;
                           }
                           t->tecla = C = E;
                           continue;
                        }

                        else if ((i = (*t->executa)( t )) == -1 || i == -2 )
                        {
                           tab_ultimo = ultimo; /* em tabela que chama outra tabela pode alterar */
                           /*** Confirmou !!! ***/
                           strcpy((SC *) (*t->opcoes)[ tmp ], (SC *) e.str );
                           if ( inserindo ) /*** So' com INSERE ***/
                           {
                              acrescenta( t, ultimo );
                           }

                           /*** i == -1 -> Confirma e sai ***/
                           if ( i == -1 )
                           {
                              t->tecla = C = K_ENTER;
                              goto sai;
                           }
                           /*** i == -2 -> Confirma e fica ***/
                           else
                           {
                              a_lista( t, ultimo );
                              break;
                           }
                        }
                        /*** i == -3 -> Nao confirma e sai ***/
                        else if ( i == -3 )  goto fim;

                        tab_ultimo = ultimo; /* em tabela que chama outra tabela pode alterar */
                        /*** i > 0 - Nao confirma e fica ***/
                        if ( inserindo )
                        {
                           ( *t->opcoes )[ t->n_elem - 1 ] = ultimo;
                        }
                        a_lista ( t, ultimo );
                        break;

         case K_INS :
                        if ( ! (t->edita  & INSERE ) )  break;
                        tmp = t->escolhido;
                        if ( ! insere( t, t->escolhido, ultimo ) )  break;
                        a_lista ( t, ultimo );

                        /*** d_entrada copia direto para o destino ***/
                        d_entrada( &e, t->win, (*t->opcoes)[ t->escolhido ],
                                    t->t_opcao, t->mascara, t->mens_ins, 1,
                                    t->escolhido, E_ALFA | E_CONFIRMA );
                        t->fun = abre_funcoes( t->fun_ins );
                        e.escape = t->escape;
                        E = entrada( &e, 0 );
                        fecha_funcoes( t->fun );
                        win_ajuda = pa;

                        if ( ! e.mudou )
                        {
                           retira( t, tmp, ultimo );
                           a_lista( t, ultimo );
                           break;
                        }

                        else if ((i = (*t->executa)( t )) == -1 || i == -2)
                        {
                           /*** i == -1 -> Confirma e sai ***/
                           if ( i == -1 )
                           {
                              t->tecla = C = K_ENTER;
                              goto sai;
                           }
                           /*** i == -2 -> Confirma e fica ***/
                           else
                           {
                              a_lista ( t, ultimo );
                              break;
                           }
                        }
                        /*** i == -3 -> NAO Confirma e sai ***/
                        else if ( i == -3 )  goto fim;

                        /*** i > 0 -> NAO Confirma e fica ***/
                        retira( t, tmp, ultimo );
                        a_lista ( t, ultimo );
                        break;

         case K_DEL :
                        if ( ! (t->edita  & DELETA ) )  break;
                        else if ((i = (*t->executa)( t )) == -1 || i == -2)
                        {
#ifdef   MEDIT
                           if ( i == -1 )   goto sai;

                           retira( t, t->escolhido, ultimo );
                           a_lista( t, ultimo );

#else

                           retira( t, t->escolhido, ultimo );
                           a_lista( t, ultimo );

                           if ( i == -1 )
                           {
                              t->tecla = C = K_ENTER;
                              goto sai;
                           }
#endif
                        }

                        else if ( i == -3 )  goto fim;
                        break;

         case K_UP   :
                        if ( t->escolhido )   t->escolhido--;
/***
                        if ( t->escolhido  ==  0 )  t->escolhido = 999;
                        else                        t->escolhido--;
***/
 			               break;

         case K_DN   :
                        if( t->escolhido < t->n_elem - 1 )  t->escolhido++;
/***
                        if( t->escolhido >= t->n_elem - 1 ) t->escolhido = 0;
                        else                                t->escolhido++;
***/
 			               break;

         case K_PGDN :
                        t->escolhido += t->n_elem / 3;
 			               break;

         case K_PGUP :
                        t->escolhido -= t->n_elem / 3;
 			               break;

         case K_HOME :
                        t->escolhido = 0;
 			               break;

         case K_END  :
                        t->escolhido = 9999;
 			               break;

         case K_ENTER :
                        if ( (t->enter & T_ENTER) == 0 )
                        {
                           C = 0;   w_ungc( K_DN );
                        }
                        break;

         case K_ESC   : break;

         default      :
                        if ( escolhe( t, C ) == K_ENTER  &&
                             (t->enter & T_LETRA) != 0    )
                        {
                           t->tecla = C = K_ENTER;
                        }
                        else if (strchr((SC *) t->escape, C ) != NULL)
                        {
                           C = K_ESC;
                        }
                        break;
         }

         break;
      }

   }  while ( C != K_ESC  &&  C != K_ENTER );

sai:

/***
   tarja ( t, T_OFF );
***/

   fwrefresh( t->win );

   i = ( *t->executa )( t );

   if ( i >= 0 ){  t->escolhido = i; goto loop; }

   else if ( i == -2 )  goto  loop;

fim:
   if ( t->n_elem <= t->n_max  && ( *t->opcoes )[ t->n_elem - 1 ] == ultimo )
   {
      ( *t->opcoes )[ t->n_elem - 1 ] = NULL;
   }

   if ( ultimo != editmp ) {
      free( ultimo );
   }

   tab_ultimo = ultimo; /* em tabela que chama outra tabela pode alterar */

   if ( abriu_win ) {  fwkill( t->win );  t->win = NULL;  }

   if ( C == K_ESC )  C = -1;

   else  {

      C = t->escolhido;

      if ( t->dest != NULL )
      {
         strcpy( (SC *) t->dest,  (SC *) (*t->opcoes)[t->escolhido] );
      }
   }

   return( C );
}


Tab  *def_tabela( t, col, lin, a_col, a_lin, n_elem, n_max, mascara,
                  l_atr, t_atr,  borda, b_atr, d_atr,  opcoes,
                  titulo, rodape )

Tab       *t;
SS        col, lin, a_col, a_lin, n_elem, n_max;
SC        *mascara;
SS        l_atr, t_atr, borda, b_atr, d_atr;
SC        *(*opcoes)[];
SC        *titulo, *rodape;
{
   t->ok      = 0;
   t->t_opcao = strlen( ( SC * ) mascara );
   t->t_opcao = MAX( t->t_opcao, 1 );
   t->mascara = mascara;
   t->opcoes  = opcoes;
   t->n_max   = MAX( 1, n_max );
   t->n_elem  = MIN( t->n_max, n_elem );

   t->v_col   = MAX( t->t_opcao + 2, strlen( titulo ) );
   t->v_lin   = n_max;
   t->col     = col;
   t->lin     = lin;
   t->a_col   = COTA( 1, a_col + 2, t->v_col );
   t->a_lin   = COTA( 1, a_lin, t->n_max );

   t->enter     = T_ENTER | T_LETRA;
   t->dest      = NULL;
   t->escolhido = 0;
   t->tarja     = 0;
   t->tecla     = 0;
   t->edita     = 0;
   t->l_atr     = l_atr;
   t->t_atr     = t_atr;
   t->borda     = borda;
   t->b_atr     = b_atr;
   t->d_atr     = d_atr;
   t->escape    = ( UC * ) "";
   t->mens_alt  = loc_str( STR_OBRIG, ALTERA_ITEM );
   t->mens_ins  = loc_str( STR_OBRIG, INSERE_ITEM );
   t->fun_alt   = "";
   t->fun_ins   = "";
   t->executa   = executa;
   t->comando   = comando;
   t->titulo    = titulo;
   t->rodape    = rodape;
   t->win       = NULL;
   t->ok        = MENU_OK;

   return ( t );
}


