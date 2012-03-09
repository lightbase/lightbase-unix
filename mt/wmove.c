
#ifdef comout

This module supplies functions which process blocks of data such as:
window refreshing & moving and block clears.

scrolls are found in scrollgrp

function list:

win_redo()                             - make terminal match main_win
fwmove( x,y,win )                      - move appear. window to x,y on main_win
fwamove( x,y,win )                     - move appear. window rel. to vert. 
fwsize( l,h,win )                      - change size of appearant window
                                         tipo = 1 Simpels  = 2 Duplas
#endif

#include <stdio.h>
#include <window.h>

VOID win_redo()
{ SS a;

#ifdef MSDOS
VOID forca_atr( void );
#endif

  main_win->w_anymod = 3;
  w_fixmain();
  for( a = 0;a < win_sp;a++ )
       win_stak[a]->w_anymod = 2;

#ifdef MSDOS
  forca_atr( );
#endif

  win_refresh();
}

VOID fwmove( x,y,win )              /* move position of window on terminal */
US x,y;                      /* new upper left - may get adjusted */
struct Window *win;
{ US ox,oy;
  SS box = 0,box2 = 0;

db1( "fwmove" );
  if( win->w_flags & W_BOXED ) { box = 1; box2 = 2; }
  if( x < box ) x = box;
  if( y < box ) y = box;
  if( x > size_x - win->w_alen - box ) x = size_x - win->w_alen - box;
  if( y > size_y - win->w_aheight - box ) y = size_y - win->w_aheight - box;
  ox = win->w_x; oy = win->w_y;
  win->w_x = x; win->w_y = y;
  wu_remap( ox - box,oy - box,                         /* remap old regeon */
           win->w_alen + box2,win->w_aheight + box2 );
  wu_remap( x - box,y - box,                           /* and new regeon */
           win->w_alen + box2,win->w_aheight + box2 ); 
}

VOID fwamove( x,y,win )             /* move appearant window rel. to vertual */
US x,y;                      /* requested new position */
struct Window *win;
{

db1( "fwamove" );
  if( x > win->w_len - win->w_alen ) x = win->w_len - win->w_alen;
  if( y > win->w_height - win->w_aheight ) 
       y = win->w_height - win->w_aheight;
  win->w_ax = x;
  win->w_ay = y;
  win->w_anymod = 2;
}

VOID fwsize( l,h,win )              /* changes the size of the appearant window */
                             /* may modify w_x, w_y to fit */
                               /* WILL SET W_ax & W_ay to 0 */
US l,h;                      /* new requested length,height */
struct Window *win;
{ US fixl,fixh;
  SS box = 0;
  SS box2 = 0;

db1( "fwsize" );
  /* check for fit */
  if( win->w_flags & W_BOXED ) { box = 1; box2 = 2; }
  if( l > win->w_len ) l = win->w_len;
  if( h > win->w_height ) h = win->w_height;
  if( l > size_x - box2 ) l = size_x - box2;
  if( h > size_y - box2 ) h = size_y - box2;
  if( l + win->w_x > size_x ||
      h + win->w_y > size_y )
       fwmove( size_x - l - box,size_y - l - box,win );
  fwamove( 0,0,win );
  /* adjust */
  fixl = ( l > win->w_alen ) ? l : win->w_alen;
  fixh = ( h > win->w_aheight ) ? h : win->w_aheight;
  win->w_alen = l;
  win->w_aheight = h;
  /* remap the window */
  wu_remap( win->w_x - box,win->w_y - box,fixl + box2,fixh + box2 );
}



