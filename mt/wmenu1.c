
#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  "window.h"
#include  "menu.h"

#ifdef     MSDOS

ST( VOID ) c_at( SS, SS, WIN * );
VOID       m_tarja ( Selecao *, SS );
ST( VOID ) lista ( Selecao * );
ST( SS ) acha_item ( Selecao *, SS );
ST( SS ) ativo( Selecao *, SS );

#else

ST( VOID ) c_at( );
VOID       m_tarja ( );
ST( VOID ) lista ( );
ST( SS ) acha_item ( );
ST( SS ) ativo( );

#endif

ST( VOID ) c_at( x, y, win )

SS    x, y;
WIN  *win;
{
   win->w_cx = x;
   win->w_cy = y;
}


VOID  m_tarja ( s, atr )

Selecao   *s;
SS       atr;
{
   SS    i, col, lin, achou;
   SC   *p;
   static UC horiz[ 82 ];
   static SS f_horiz = 0;

   if ( ! f_horiz ) /*** Faca isto apenas UMA VEZ ***/
   {
      for ( f_horiz = 0;  f_horiz < sizeof( horiz );  f_horiz++ )
      {
         horiz[ f_horiz ] = UG_HOR;
      }

      horiz[ f_horiz ] = '\0';
   }

   lin = s->tarja % s->v_lin;
   col = (s->tarja / s->v_lin) * (s->t_opcao + 1) + 1;

   /*** Va para o final da tarja para fazer um balanco correto **/

/***
   Vamos ver se realmente precisa do +1
   fwat( col + s->t_opcao + 1, lin, s->win );
***/
   fwat( col + s->t_opcao,     lin, s->win );

   fwat( col, lin, s->win );

   if ( s->tipo == MENU  &&  *( *s->opcoes )[ AT( s->tarja ) ]  ==  '2' )
   {
      s->win->w_atrib = s->b_atr;

      if ( s->num_col == 1 )
      {
         c_at( col - 1, lin, s->win );

         fwpsl( horiz, s->win, s->t_opcao + 2 );
      }

      else {

         c_at( col, lin, s->win );

         fwpsl( horiz, s->win, MAX( 0, s->t_opcao - 2 ) );
      }
   }

   else  {

      c_at( col, lin, s->win );

      p = (*s->opcoes)[ s->tipo == MENU ? OP( s->tarja ) : s->tarja ];

      /*** Quando o item estiver desligado ignore 'atr' ***/

      if ( s->tipo == MENU  &&  *( *s->opcoes )[ AT( s->tarja ) ] != '1' )
      {
         s->win->w_atrib = s->l_atr;

         fwpsl( ( UC * ) p, s->win, s->t_opcao );
      }

      else
      {
         s->win->w_atrib = atr;

         fwpsl( ( UC * ) p, s->win, s->t_opcao );

         if ( atr != TARJAON )
         {
            s->win->w_atrib = s->d_atr;

            for ( i = achou = 0;  *p != '\0';  i++,  p++ )
            {
               if ( *p >= 'A' && *p <= 'Z' )
               {
                  achou++;

                  c_at( i + col, lin, s->win );
                  fwpsl( ( UC * ) p, s->win, 1 );
                  break;
               }
            }

            if ( !achou )
            {
               p = (*s->opcoes)[ s->tipo == MENU  ?
                                  OP( s->tarja )  :  s->tarja ];

               c_at( col, lin, s->win );

               for ( i = 0;  *p != '\0';  i++,  p++ )
               {
                  if ( *p != ' ' )
                  {
                     c_at( i + col, lin, s->win );

                     fwpsl( ( UC * ) p, s->win, 1 );
                     break;
                  }
               }
            }
         }
      }
   }

   fwat( col, lin, s->win );
}


ST( VOID ) lista ( s )

Selecao *s;
{
   s->win->w_atrib = s->l_atr;   fwcls( s->win );

   for( s->tarja = 0;  s->tarja < s->n_elem;  s->tarja++ )
   {
      m_tarja ( s, TARJAOFF );
   }

   s->tarja = 0;   m_tarja ( s, TARJAOFF );
}

#ifdef  XTOOLS

VOID  ma_lista ( s )

Selecao  *s;
{
   SS  tr, ax, ay;

   tr = s->tarja;
   ax = s->win->w_ax;
   ay = s->win->w_ay;

   lista( s );

   s->tarja     = tr;
   s->win->w_ax = ax;
   s->win->w_ay = ay;
}

#endif

ST( SS ) acha_item ( s, ch )

Selecao  *s;
SS       ch;
{
   SS    i, achou, letra;
   SC    *p;

   for ( letra = -1, i = 0, ch = toupper( ch );  i < s->n_elem;  i++ )  {

      for ( achou = 0, p = (*s->opcoes)[ s->tipo == MENU ? OP( i ) : i ];
                                                         *p != '\0';  p++ )
      {
         if ( *p >= 'A' && *p <= 'Z' ) {  achou++;  break;  }
      }

      if ( achou  )  {  if ( ch  ==  *p )   goto  marca;  }

      else  {

         for ( p = (*s->opcoes)[ s->tipo == MENU ? OP( i ) : i ];
                                                        *p == 32;  p++ )
            ;

         if ( letra < 0  &&  ch == toupper( *p ) )   letra = i;
      }
   }

   if ( letra  >=  0 )  {  i = letra;  goto  marca;  }

   return ( ch );

marca:

   if ( s->tipo == MENU  &&  *( *s->opcoes )[ AT( i ) ]  !=  '1' )
      return( ch );

   s->escolhido = i;
	m_tarja ( s, TARJAOFF );
	s->tarja = s->escolhido;
	m_tarja ( s, TARJAON );

   return ( K_ENTER );
}

ST( SS ) ativo( s, item )

Selecao  *s;
SS      item;
{
   SS   dir;

   switch ( s->tecla )
   {
   case K_UP   :
   case K_LFT  :
   case K_END  :
   case K_PGUP :
                  dir = 0;
                  break;

   default     :
                  dir = 1;
                  break;
   }

   item = COTA( 0, item, s->n_elem - 1 );

   while ( s->tipo == MENU  &&  *( *s->opcoes )[ AT( item ) ]  !=  '1' )
   {
      if ( dir )
      {
         if ( item >= s->n_elem )  {  dir = 0;  continue;  }

         else  item++;
      }

      else {

         if ( item == 0 )  {  dir = 1;  continue;  }

         else  item--;
      }
   }

   return ( item );
}


SS escolha( s )

Selecao  *s;
{
   return ( menu( s ) );
}


SS menu( s )

Selecao  *s;
{
   SS     i, ch, mens_aberta = 0;
   SS     primeira_vez = 0;

   /*** Garantimos que existe pelo menos uma tarja habilitada ***/
   for ( i = 0;  i <= s->n_elem    &&
                 s->tipo == MENU   &&
                 *( *s->opcoes )[ AT( i ) ]  !=  '1';  i++ ) /** NADA **/ ;

   if ( s->ok != MENU_OK  ||  i > s->n_elem )  return( -1 );

   if ( s->tipo == MENU  &&  s->men == NULL )
   {
      s->men = abre_mensagem( "" );
      mens_aberta++;
   }

   s->win = win_make(s->v_col, s->v_lin, s->a_col, s->a_lin, s->col, s->lin);

   if ( s->win == NULL ) {  ch = K_ESC;  goto sai;  }

   s->win->w_flags  = W_VISIB;
   s->win->type     = s->borda;
   s->win->w_boxatr = s->b_atr;
   s->win->t_atr    = s->b_atr;

   if ( s->borda )
   {
      if ( *s->titulo  ) {  s->win->title  = s->titulo;  }
      if ( *s->rodape  ) {  s->win->bottom = s->rodape;  }
      fwbox( s->win );
   }

   s->win->w_curstyle = CS_OFF;

   s->tecla = 0;

loop:

   if ( strange_term == 1  &&  primeira_vez == 0 )
   {
      primeira_vez++;
      s->win->w_anymod = 2;
      lista ( s );
      s->win->w_anymod = 0;
   }

   else  lista( s );

   do  {

      s->escolhido = ativo( s, s->escolhido );

      m_tarja ( s, TARJAOFF );

      s->tarja = s->escolhido;

      m_tarja ( s, TARJAON );

      if ( s->ajuda )   win_ajuda = ( *s->op_ajuda )[ s->escolhido ];

      if ( s->tipo == MENU  &&   ! w_kbhit( s->win ) ) {

         mens_1( s->men, (*s->opcoes)[ AJ( s->escolhido ) ] );
      }

      fwrefresh(   s->win );
      fwsetcursor( s->win );

      ch = ( *s->comando )( s );

      if ( ch == -1 )       {  ch = K_ESC;  continue; }

      else if ( ch == -2 )  {  ch = s->tecla;  }

      else if ( ch == -3 )  {  goto  sai;  }

      else                  {  s->tecla = ch = fwnqgc( s->win ); }

      switch ( ch ) {

      case K_RT  :
                     if ( s->escolhido / s->v_lin  <  s->num_col - 1 ) {

                            s->escolhido += s->v_lin;
                     }
/***
                     else if(s->escolhido >= s->n_elem - 1) s->escolhido = 0;
***/
/***
                     else if ( s->num_col   >  1  &&
                               s->escolhido >= s->n_elem  - 1 )
                     {
                        s->escolhido = 0;
                     }
                     else if(s->escolhido >= s->n_elem - 1) s->escolhido = 0;
                     else                                   s->escolhido++;
***/
 			            break;

      case K_LFT  :
                     if ( s->escolhido / s->v_lin  >  0 ) {

                            s->escolhido -= s->v_lin;
                     }
/***
                     else if ( s->escolhido  ==  0 )  s->escolhido = 999;
***/
/***
                     else if ( s->num_col > 1  &&  s->escolhido  ==  0 )
                     {
                        s->escolhido = 999;
                     }
                     else if ( s->escolhido  ==  0 )  s->escolhido = 999;
                     else                             s->escolhido--;
***/
 			            break;

      case K_UP   :
                     if ( s->escolhido  )  {  s->escolhido--;  }
/***
                     if ( s->escolhido  ==  0 )  s->escolhido = 999;
                     else                        s->escolhido--;
***/
 			            break;

      case K_DN   :
                     if ( s->escolhido < s->n_elem - 1 )  s->escolhido++;
/***
                     if ( s->win->w_cy  <  s->win->w_height - 1 ) {

                        s->escolhido++;
                     }
                     if( s->escolhido >= s->n_elem - 1 )  s->escolhido = 0;
                     else                                 s->escolhido++;
***/
 			            break;

      case K_PGDN :
                     s->escolhido += s->win->w_aheight;
/***
                     s->escolhido += s->win->w_aheight * s->num_col;
                     s->escolhido += s->n_elem / 3;
***/
 			            break;

      case K_PGUP :
                     s->escolhido -= s->win->w_aheight;
/***
                     s->escolhido -= s->win->w_aheight * s->num_col;
                     s->escolhido -= s->n_elem / 3;
***/
 			            break;

      case K_HOME :
                     s->escolhido = 0;
 			            break;

      case K_END  :
                     s->escolhido = 9999;
 			            break;

      case K_ENTER : break;

      case K_ESC   : break;

      default      : ch = acha_item( s, ch );
                     break;
      }

   }  while ( ch != K_ESC  &&  ch != K_ENTER );

   fwrefresh( s->win );

   if ( (i = ( *s->executa )( s )) != -1 ){  s->escolhido = i; goto loop; }

sai:

   if ( s->win != NULL )   fwkill( s->win );

   if ( mens_aberta ) {  fecha_mensagem( s->men );   s->men = NULL;  }

   if ( ch == K_ESC )        ch = -1;

   else                      ch = s->escolhido;

   return( ch );
}


