
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <ctype.h>
#include   "window.h"
#include   "entrada.h"
#include   "tabela.h"
#include   "mensagem.h"

#ifdef     MSDOS

ST( VOID ) lista_opcao( d_entry * );
ST( SS )   tab_lista( d_entry * );

SC         *masc_trim( SC *, SC * );
VOID       imp_alfa( d_entry *, SS, SS );

#else

ST( VOID ) lista_opcao( );
ST( SS )   tab_lista( );

SC         *masc_trim( );
VOID       imp_alfa( );

#endif

SS   ent_lista( e, cursor )

d_entry    *e;
SS         cursor;
{
   SS     tipo_cursor, prep, crit, m_aberta = 0;
   SC     s1[ MAX_EDIT + 1 ];
   UC     ch, atrib;
   struct Window *janela;

   e->mudou = 0;

   /*** Se ( prepara )() voltar = 0 Edita ***/
   /*** Se ( prepara )() voltar < 0 Retorna 0             NAO Edita ***/
   /*** Se ( prepara )() voltar > 0 Imprime e Retorna 0 - NAO Edita ***/

   prep = ( *e->prepara )( e );

   if ( prep < 0 )   return( 0 );

   e->tam_m = strlen( e->mascara );    e->tam_m = MIN( e->tam_m, MAX_EDIT );

   if ( e->tam_m == 0 )   return( 0 );

   e->str[     e->tam_m ] = '\0';
   e->mascara[ e->tam_m ] = '\0';

   /* conta numero de possibilidades */
   /* e->piece e e->q_lista sao invalidos como 0 */

   for ( e->piece = e->q_lista = 0;
         piece( e->lista, s1, '@', e->q_lista + 1, e->tam_m )  !=  0;
         e->q_lista++ )
   {
      if ( strcmp( e->str, s1 ) == 0 ) e->piece = e->q_lista + 1;
   }

   strcpy( (SC *) s1, (SC *) e->str );

   if ( e->piece == 0 )
   {
      e->piece = 1;
      piece( e->lista, e->str, '@', 1, e->tam_m );
   }

   janela = e->win;

   atrib       = janela->w_atrib;
   tipo_cursor = janela->w_curstyle;

   if ( e->visivel > janela->w_len - e->col )
   {
      e->visivel = janela->w_len - e->col;
   }

   if ( prep > 0 || (e->tipo & E_MOSTRA) )
   {
      imp_alfa( e, e->n_atr, e->visivel );
      if ( e->refresh )   fwrefresh( e->win );
      ch = e->tecla = K_END;
      goto  sai;
   }

   fwat( e->col, e->lin, janela );

   if ( e->men == NULL && e->mensagem != NULL && *e->mensagem != '\0' )
   {
      e->men = abre_mensagem( e->mensagem );

      if ( e->men != NULL )  m_aberta++;
   }

   else {

      mens_1( e->men, e->mensagem );
   }

   e->cursor = cursor;

critica:

   while ( 1 )
   {
      ( *e->crit_char )( e );

      imp_alfa( e, e->e_atr, e->visivel );

      ch = e->tecla = fwqgc( e->win );

      if ( strchr( (SC *) e->escape, ch )  !=  NULL )   goto fim;

      switch ( ch )
      {
      case ' '    :  /*** ESPACO MOSTRA A OPCAO SEGUINTE ***/
                     lista_opcao( e );
 			            break;

      default    :
                     if( tab_lista( e ) )   goto fim;
                     continue;

      case K_INS  :
                     insercao  =  !insercao;
 			            continue;

      case K_UP   :
      case K_DN   :
      case K_TAB  :
      case K_BTAB :
      case K_PGDN :
      case K_PGUP :
      case K_CPGDN :
      case K_CPGUP :
                     goto  fim;

      case K_ESC   :
                     strcpy( ( SC * ) e->str, ( SC * ) s1 );
                     goto  fim;

      case K_ENTER :
					      ( *e->crit_char )( e );
                     goto  fim;
      }

   }

fim:

   imp_alfa( e, e->n_atr, e->visivel );

   if ( e->refresh )   fwrefresh( e->win );

   /*** e->str[ procura_end( e, 0 ) ] = '\0'; ***/

   masc_trim( e->str, e->mascara );

   if ( ch  !=  K_ESC )   e->mudou = strcmp( e->str, ( SC * ) s1 );

   crit = ( *e->critica )( e );

   if ( crit > 0 )
   {
      imp_alfa( e, e->n_atr, e->visivel );

      if ( e->refresh )   fwrefresh( e->win );
   }

   else if ( crit < 0 )   goto  critica;

   if ( m_aberta )  {  fecha_mensagem( e->men );   e->men = NULL;  }

sai:

   /*** e->str[ procura_end( e, 0 ) ] = '\0'; ***/

   masc_trim( e->str, e->mascara );

   janela->w_curstyle = tipo_cursor;

   janela->w_atrib = atrib;

   if ( prep > 0 )  ch = 0;

   return( ( SS ) ch );
}


ST( VOID )  lista_opcao( E )

d_entry *E;
{
   SC   aux[ MAX_EDIT + 1 ];

   if ( E->q_lista == 0 )  return;

   if ( ++E->piece  >  E->q_lista )   E->piece = 1;

   piece( E->lista, aux, '@', E->piece, E->tam_m );

   strcpy( (SC *) E->str, (SC *) aux );
}


ST( SS )  tab_lista( E )

d_entry     *E;
{
   SS       I, J, K, L;
   Tab      T;
   SC       *lista, **opcoes;
   SC       *titulo, str1[ MAX_EDIT + 1 ];

   if ( E->q_lista == 0 )  return( 1 );

   for ( L = 1, I = 0;
       (K = piece( E->lista, str1, '@', I + 1, E->tam_m ))  !=  0;  I++ )
   {
      L = MAX( K, L );

      for ( J = 0;   str1[ J ] != '\0';   J++ )
      {
         if ( str1[ J ] >= 'A'  &&  str1[ J ] <= 'Z'  &&
              str1[ J ] ==  toupper( E->tecla ) )
         {
            E->piece = I + 1;
            strcpy( E->str, str1 );

            return ( 1 );
         }
      }
   }

   lista = malloc( strlen( E->lista ) + 1 );

   if ( lista == NULL )  return( 0 );

   opcoes = (SC **) calloc( (E->q_lista + 3), sizeof( SC * ));

   if ( opcoes == NULL ) {  free( lista );  return( 0 ); }

   strcpy( lista, E->lista );

   for ( opcoes[ 0 ] = lista, I = 0, J = 1;    lista[ I ] != '\0';    I++ )
   {
      if ( lista[ I ] == '@' )
      {
         lista[ I ] = '\0';

         if ( J  <  E->q_lista ) {
            opcoes[ J ] = &lista[ I + 1 ];
            J++;
         }
      }
   }

   opcoes[ J ] = NULL;

   strcpy( str1, E->mascara );

   str1[ L ] = '\0';

   titulo = loc_str( STR_OBRIG, T_OPCAO );

   def_tabela( &T, E->win->w_x + E->win->w_cx + 1,
               E->win->w_y + E->win->w_cy + 1,
               L + 2, E->q_lista, E->q_lista, E->q_lista,
               str1, MP_CAMPO, MP_BARRA, 1, MP_JANELA, MP_DESTAQUE,
               ( SC * (* )[] ) opcoes,
               L >= strlen ( titulo ) ? titulo : "", "" );

   T.escape    = E->escape;
   T.escolhido = E->piece - 1;

   I = tabela( &T );

   if ( I  !=  -1 )
   {
      E->piece = I + 1;
      strcpy( E->str, opcoes[ I ] );
   }

   free( (SC *) lista );
   free( (SC *) opcoes );

   return ( 1 );
}

SC  *formata_campo( dest, orig, masc, tipo )

SC    *dest, *orig, *masc;
US    tipo;
{
   switch ( tipo & 0xFF )
   {
   default      :
   case E_LISTA :
   case E_ALFA  :    return ( alfa_importa( dest, orig, masc ) );

   case E_VALOR :    return ( valor_importa( dest, orig, masc ) );
   }
}


SC *alfa_importa( dest, orig, masc )

SC   *dest, *orig, *masc;
{
   SC     *M, *O, *D;

   trim( orig );

   for ( D = dest, M = masc, O = orig;   *O != '\0'  &&  *M != '\0';   )
   {
      switch ( *M )
      {
      case 'a' :
      case 'A' :
                  if ( minuscula( *O )  ||  maiuscula( *O ) )
                  {
                     if ( *M  ==  'A' )   *D = para_maiuscula( *O );
                     else                 *D = *O;
                     break;
                  }
                  else  {  O++;  continue;  }

      case 'X' :
                  *D = para_maiuscula( *O );
                  break;

      case '9' :
                  if ( *O == ' '  ||  (*O >= '0'  &&  *O <= '9') )
                  {
                     *D = *O;
                     break;
                  }
                  else  {  O++;  continue;  }

      case '*' :
      case 'x' :
      case 'P' :
                  *D = *O;
                  break;

      default  :
                  if ( *O == ' '  ||  *O == *M )   O++;
                  *D++ = *M++;
                  continue;
      }

      /*** CUIDADO - As vezes NAO passa por aqui !!! ***/
      M++, O++, D++;
   }

   *D = '\0';

   masc_trim( dest, masc );

   return ( dest );
}




