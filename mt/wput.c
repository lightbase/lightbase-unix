
#ifdef comout

This Header contains the general purpose print to screen functions
( except (f)wprint() )

FUNCTIONS INCLUDE:

   fwputs( s,win )       /* outputs a string */
   fwpc( c,win )         /* outputs a character */
   fwat( x,y,win )       /* adjusts current cursor position */
                         /* x = horiz, y = vert, 0,0 == upper left */
   fwpsl( s, win, tam )  /* filtra CTRL-CHARS ***/
                         /* Imprime na posicao do cursor tam chars **/
                         /* NAO executa nenhum WRAP, imprime so na linha */

functional control characters are:
\r  ( go to start of present line, non_destructive )
\n  ( go to start of next line w/ scroll management )
\b  ( non-destructive back space ) 

#endif

#include <stdio.h>
#include <window.h>

static struct Window *w;

# ifdef     MSDOS

VOID       seedominant( SS );   
VOID       shiftmain( SS, SS );
ST( VOID ) fixwin( WIN * );
ST( VOID ) xwpc( UC );
ST( VOID ) mustscroll( VOID );

#else

VOID       seedominant( );   
VOID       shiftmain( );
ST( VOID ) fixwin( );
ST( VOID ) xwpc( );
ST( VOID ) mustscroll( );

#endif


#define W_ATRIB( x ) ( (w->w_atrib << 8) + (x) ) 


VOID fwpsl( s, win, tam )  /*** filtra CTRL-CHARS ***/

UC   *s;
WIN  *win;
SS   tam;
{ 
   register UC *p;
   register US *pary;
   SS       quant;

   if ( tam > win->w_len - win->w_cx )  tam = win->w_len - win->w_cx;

   if ( tam > 0 )
   {
      for( pary = win->w_ary + ((win->w_cy * win->w_len) + win->w_cx), p = s,
           quant = 0;  quant < tam  &&  *p != '\0';  quant++, p++, pary++ )
      {
         *pary = (*p  <  ' ' ? ' ' : *p) + (win->w_atrib << 8);
      }

      if ( quant ) 
      {
         wu_lmark( win, quant );

         win->w_cx += quant;

         if ( win->w_cx >= win->w_len )  win->w_cx = win->w_len - 1;
      }
   }
}


VOID fwputs( s,win )   
UC *s;                               /* null terminated string */
struct Window *win;
{ 
  register UC *p;
  register US *pary;
  SS len,max;

db1( "fwputs" );
  w = win;
  stak1( w, "FWPUTS" );

  p = s;
  while( *p )
        { 
          if( *p < ' ' ) 
                { xwpc( *p ); p++; continue; } 
          for( max = w->w_len - w->w_cx,len = 0,
               pary = w->w_ary + (( w->w_cy ) * w->w_len +
                                  w->w_cx );
               max;max--,len++,p++,pary++ )
                { if( *p < ' ' ) break;
                  *pary = W_ATRIB( *p );
                }
          if( len )
                { wu_lmark( w,len );
                  w->w_cx += len;
                }
          if( *p < ' ' ) continue;
          if( ! ( w->w_flags & W_WRAP ))
                { /* if no wrap, skip till non printable */
                  while( *p >= ' ' ) p++;
                  continue;
                }
           xwpc( '\n' );   
        }
  if( win->w_flags > 1 ) fixwin( win );
}

VOID fwpc( c,win )                           
UC c;
struct Window *win;
{ 
db1( "fwpc" );
  w = win;
  stak1( w, "FWPC" );
  xwpc( c );
  if( w->w_flags > 1 ) fixwin( win );
}

VOID fwat( x,y,win )
SS x,y;           /* location in actual window to go to, may be negative */
                  /* gets as close as can */
struct Window *win;   
{ 

db1( "fwat" );
  stak1( win, "FWAT" );
  win->w_cx = x;
  win->w_cy = y;

  if( win->w_cx > win->w_len - 1 ) 
        win->w_cx = win->w_len - 1;
  if( win->w_cy > win->w_height - 1 )
        win->w_cy = win->w_height - 1;

  if( win->w_flags > 1 ) fixwin( win );
}


/**** BEGIN HIDDEN FUNCTIONS ****/

/*** FOI RETIRADO W_IMED ***/

ST( VOID ) fixwin( win )                 /* process strange w_flags */
struct Window *win;
{

db2( "fixwin" );
   /* process W_VISIB, W_HIGH & W_IMED flags */
   /*** FOI RETIRADO W_IMED ***/

   if( win->w_flags & W_HIGH )  {

      if( win->w_num != win_sp - 1 )  fwtop( win );
   }

   if( win->w_flags & W_VISIB )  {

      if( win->w_cx  <  win->w_ax ) {

         win->w_ax   = win->w_cx;
         win->w_ax  -= win->w_ax  < (win->w_alen / 8) ?
                                            win->w_ax : (win->w_alen / 8);
         win->w_anymod = 2;
      }

      else if ( win->w_cx  >=  win->w_ax + win->w_alen ) {

         if( win->w_cx  >= win->w_len )  win->w_cx = win->w_len - 1;

         win->w_ax += win->w_cx - (win->w_ax + win->w_alen) + 1;
         win->w_ax += ((win->w_len - win->w_alen - win->w_ax)   <
                       (win->w_alen / 4)                      ) ?
                       (win->w_len - win->w_alen - win->w_ax)   :
                       (win->w_alen / 4);

         win->w_anymod = 2;
      }

      if( win->w_cy  <  win->w_ay )  {
         
         win->w_ay     = win->w_cy;
         win->w_anymod = 2;
      }

      else if ( win->w_cy  >=  win->w_ay + win->w_aheight ) {

         win->w_ay = win->w_ay + (win->w_cy -
                                 (win->w_ay + win->w_aheight)) + 1;
         win->w_anymod = 2;
      }
   }

/***   if ( win->w_flags & W_IMED )  fwrefresh( win ); ***/
}


ST( VOID ) xwpc( c )                /* put a character to window w. w/ all 
                                   sub-processing */
UC c;
{

db2( "xwpc" );
  switch( c )
        { case '\n': if( w->w_cy >= w->w_height - 1 )     mustscroll( );

                     else if( w->w_cy < w->w_height - 1 )  w->w_cy++;

                     w->w_cx = 0;
                     break;

          case '\r': w->w_cx = 0;
                     break;
          case 7:    /* bell */
                     win_bell( BEL_GEN );
                     break;
          case '\b': if( ! w->w_cx ) return;
                     w->w_cx--;

/* UNCOMMENT TO MAKE BACKSPACE DESTRUCTIVE
                     w->w_ary[ w->w_cy * w->w_len + w->w_cx ] = W_ATRIB( ' ' );
                     if( win_map[ ( w->w_y + w->w_cy - w->w_ax ) * size_x +
                                    w->w_x + w->w_cx - w->w_ax ] == w->w_num )
                            wu_mark( w );
 END DESTRUCTIVE BACKSPACE COM-OUT */

                     break;
          default: 
                   w->w_ary[ ( w->w_cy ) * w->w_len + 
                               w->w_cx ] = W_ATRIB( c );
                   if( win_map[ ( w->w_y + w->w_cy - w->w_ay ) * size_x +
                                  w->w_x + w->w_cx - w->w_ax ] == w->w_num )
                        wu_mark( w );
                   if( w->w_cx >=  w->w_len - 1 )
                        { /* may need to wrap/scroll/etc. */
                          if( ! ( w->w_flags & W_WRAP )) return;
                          xwpc( '\n' ); 
                          return;
                        }
                   w->w_cx++;
                   return;
        }

/***  if( w->w_flags > 1 ) fixwin( w );  ***/
}

ST( VOID ) mustscroll( )                  /* processes scroll request */
{
   register SS A;
   register US *P, *Q;

   if( w->w_flags & W_TOTOP ) {  w->w_cy = 0;  return;  }

   if( w->w_flags & W_NOSCROL )  return;

   for( A = (w->w_height - 1) * w->w_len,
        P = w->w_ary, Q = P + w->w_len;   A;   A-- )   *P++ = *Q++;

   for( A = w->w_len;   A;   A-- )   *P++ = W_ATRIB( ' ' );
   
   w->w_anymod = 2;
}


