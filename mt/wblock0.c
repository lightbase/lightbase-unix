
#ifdef comout

This module supplies functions which process blocks of data such as:
window refreshing & moving and block clears.

scrolls are found in scrollgrp

function list:

fwrefresh( win )                       - assure mods in a window are displayed
win_refresh()                          - make mods in all windows display
fwcls( win )                           - clear the window 
fwcle( win )                           - clears from cursor to window end. 
fwcll( win )                           - clears from cursor to line end. incl.
fwbox( win )                           - put a box around a window
fw_title( win, title, atr )            - Coloca Titulo Centralizado
fw_bottom( win, bottom )               - Coloca Rodape alinhado pela direita
                                         tipo = 1 Simpels  = 2 Duplas
#endif

#include <stdio.h>
#include <window.h>

VOID fwrefresh( win )           /* assure all mods from window are displayed */

WIN  *win;
{
   db1( "fwrefresh" );
   if( ! win->w_anymod ) {  if ( main_win->w_anymod )  w_fixmain( );  }

   else 
   {
      /* do update */
      if ( strange_term != 1  ||  win->w_anymod >= 2 )   wu_update( win );
      if( ! main_win->w_anymod )  main_win->w_anymod = 1;
      w_fixmain();
   }
}

VOID win_refresh()                  /* refresh all windows */
{
   SS  a;
   SS  adj = 0;
   WIN *win;

   db1( "win_refresh" );

   if( main_win->w_anymod )  adj = 1;

   for( a = 0;   a < win_sp;   a++ )
   {
      win = win_stak[ a ];
      if( ! win->w_anymod )  continue;

      adj = 1; 

      if ( strange_term != 1  ||  win->w_anymod >= 2 )   wu_update( win );
   }

   if( ! adj )  return;

   main_win->w_anymod = 1;

   w_fixmain();
}


VOID fwcls( win ) /* clears the entire window cursor upper left of vertual */

struct Window *win;
{ register SS x;
  register US *p;

  db1( "fwcls" );
  stak1( win, "FWCLS" );

  for( x = win->w_len * win->w_height, p = win->w_ary;  x;  x--, p++ )  {

     *p = ' ' + ( win->w_atrib << 8 );
  }
  win->w_anymod = 2;
  win->w_cx = 0;
  win->w_cy = 0;
}

VOID fwcle( win )                   /* clears from current cursor position
                                  to end of window inclusive.
                                  cursor position remains unchanged */
struct Window *win;
{
  SS x,y,a;
  x = win->w_cx; 
  y = win->w_cy;

  db1( "fwcle" );
  stak1( win, "FWCLE" );

  for( a = y;a < win->w_height;a++ )
       { fwcll( win );
         win->w_cx = 0;
         win->w_cy = a + 1;
       }
  win->w_cx = x;
  win->w_cy = y;
}

VOID fwcll( win )                   /* clears from current cursor position
                                  to end of vertual line inclusive.
                                  cursor position remains unchanged */
struct Window *win;
{
  SS len;
  register SS a;
  register US *p;

  db1( "fwcll" );
  stak1( win, "FWCLL" );
  /* space out the mods */
  p = win->w_ary + ( win->w_len * win->w_cy ) + win->w_cx;
  for( a = win->w_len - win->w_cx;a;a--,p++ )
      *p = ' ' + ( win->w_atrib << 8 );
  /* now report them */
  len = win->w_len - win->w_cx;        /* len = length of mod in apparant */
  wu_lmark( win,len );
}

VOID fwbox( win )                   /* places a box around the window, 
                                  notes: 
                                    - windows have boxes if
                                      main_win->w_flags & W_BOXES is set.
                                    - vertual size never changed, appar.
                                      size is changed if needed, window
                                      shift preferred.
                               */
struct Window *win;
{ 
  stak1( win, "FWBOX" );
  if( win == def_win || win->w_num >= win_sp ) return;

  win->w_flags |= W_BOXED;

  /* assure a fit, if not, adjust location &| appar. size */
  if( win->w_alen > size_x - 2 ) win->w_alen = size_x - 2;
  if( win->w_aheight > size_y - 2 ) win->w_aheight = size_y - 2;
  if( win->w_alen + win->w_x + 1 > size_x ) 
       win->w_x = size_x - 1 - win->w_alen; 
  if( win->w_aheight + win->w_y + 1 > size_y )
       win->w_y = size_y - 1 - win->w_aheight;
  if( win->w_x < 1 ) win->w_x = 1;
  if( win->w_y < 1 ) win->w_y = 1;
  if( win->w_alen + win->w_x + 1 > size_x )
       win->w_alen = size_x - 1 - win->w_x;
  if( win->w_aheight + win->w_y + 1 > size_y )
       win->w_aheight = size_x - 1 - win->w_y;

  wu_remap( win->w_x - 1, win->w_y - 1,win->w_alen + 2, win->w_aheight + 2 );
/***  Em teste WUPDATE.C
  b_map( win );
***/
}
         
VOID  fw_title( win, title, atr )

struct Window  *win;
SC           *title;
SS             atr;
{
   win->title = title;
   win->t_atr = atr;
   fwbox( win );
}

VOID fw_bottom( win, bottom )

struct Window  *win;
SC           *bottom;
{
   win->bottom = bottom;
   fwbox( win );
}

