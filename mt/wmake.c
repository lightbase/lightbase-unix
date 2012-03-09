
#ifdef comout 

This module holds the initialization group of functions:

        win_make( L,H,l,h,x,y ) - makes a new window 
        fwkill( window )        - undoes a window, manages def_win & curr_win
        fwtop( window )         - makes window highest priority 
        fwbot( window )         - makes window lowest priority

#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <window.h>

#ifdef    MSDOS

WIN *      alloc_win( SS, SS );  /*** E' usada em winit.c ***/
VOID       free_win( WIN * );    /*** E' usada em winit.c ***/
ST( VOID ) win_limit( WIN * );
VOID  remap( SS, SS, SS );

#else

WIN *    alloc_win( );
VOID     free_win( );
ST( VOID ) win_limit( );
VOID  remap( );

#endif


struct Window *win_make( L,H,l,h,x,y )  /* makes a window ( blank ), and 
                                           places it on top of the window stack.
                                           also returns window ptr, 
                                           window not refreshed.
                                           if 0, then error */
SS L,H;                        /* size of vertual window */
SS l,h;                        /* size of actual window */
SS x,y;                        /* position of upper left of actual on screen.*/
                                /* l,h,x,y may get adjusted to fit */
{
  struct Window *ret;
  SS box = 0,box2 = 0;

db1( "win_make" );
  if( win_sp >= MAXWIN )
        return 0;
  if( l > size_x ) l = size_x;
  if( h > size_y ) h = size_y;
  if( l < 1 ) l = 1;
  if( h < 1 ) h = 1;
#ifdef  TESTE
  if( L > 255 ) L = 255;
  if( H > 255 ) H = 255;
#endif
  if( L < l ) L = l;
  if( H < h ) H = h;
  ret = alloc_win( L,H );
  if( ! ret ) return 0;
  /* establish defaults */
  ret->w_len = L;
  ret->w_height = H;
  ret->w_alen = l;
  ret->w_aheight = h;
  ret->w_x = x;
  ret->w_y = y;
  ret->w_ax = 0;
  ret->w_ay = 0;
  ret->w_cx = 0;
  ret->w_cy = 0;
  ret->w_flags = main_win->w_flags;
  ret->w_intr = main_win->w_intr;
  ret->w_atrib = main_win->w_atrib;
  ret->w_defatr = main_win->w_defatr;
  ret->w_optatr = main_win->w_optatr;
  ret->campoatr = main_win->campoatr;
  ret->w_boxatr = main_win->w_boxatr;
  ret->w_curstyle = main_win->w_curstyle;
  ret->type   = main_win->type;
  ret->title  = main_win->title;
  ret->bottom = main_win->bottom;
  ret->t_atr  = main_win->t_atr;
  ret->ident  = main_win->ident;
  win_limit( ret );             /* assure values not out of bounds */
  /* set basic defaults */
  /* clear the screen */
  w_setint( ret->w_ary,L * H,' ' );
  /* insert it in the stack */
  win_stak[ win_sp ] = ret;
  ret->w_num = win_sp;
{
/***
  if( ret->w_alen * ret->w_aheight > ( size_x * size_y ) * 0.8 )
        win_dominant = win_sp;
  else  win_dominant = -1;
   US A, B;
   A = ret->w_alen * ret->w_aheight;
   B = size_x * size_y;
   if ( A > (B * 4) / 5 )
***/
   if ((SL)(ret->w_alen * ret->w_aheight) > ((SL)(size_x * size_y) * 4) / 5 )
      win_dominant = win_sp;
   else
      win_dominant = -1;
}
  win_sp++;
  /* map window in */
  if( ret->w_flags & W_BOXED )
        { box = 1;
          box2 = 2;
        }
  wu_remap( ret->w_x - box, ret->w_y - box,
            ret->w_alen + box2,ret->w_aheight + box2 );
/***  Em teste WUPDATE.C
  w_map( ret );
***/

  return ret;
}


SS fwkill( win )                      /* 0 or bad window, deletes the window
                                           frees up all memory, and
                                           informs main_win of adjust.
                                           does not refresh.
                                           if( def_win or curr_win = win )
                                                then def_win set to 0,
                                                     curr_win to top of stack
                                        */
struct Window *win;
{ 
  SS wnum,a;
  SS x,y,l,h;

db1( "fwkill" );

  if ( win == NULL || win_stak[ win->w_num ] != win )
  {
    debug( "Fwkill == NULL" );
    return -1; 
  }

  if( win->w_num >= win_sp ) return -1; /* bad window */
  if( win == def_win || win == main_win ) return -1; /* can't kill the base */
  x = win->w_x, y = win->w_y; l = win->w_alen; h = win->w_aheight;
  if( win->w_flags  & W_BOXED )
      { x--; y--; l += 2; h += 2; }
  wnum = win->w_num;
  free_win( win );
  /* adjust the stack */
  win_sp--;
  for( a = wnum;a < win_sp;a++ )
        { win_stak[ a ] = win_stak[ a + 1];
          win_stak[ a ]->w_num = a;
        }
  remap( wnum,win_sp,-1 );
  if( curr_win == win ) curr_win = def_win;
  if( def_win == win ) def_win = win_stak[ win_sp - 0 ];
  /* and map in the changes */
  wu_remap( x,y,l,h );
  win_dominant = -1;
/***  win_refresh();  ***/
  return 0;
}

ST( VOID ) win_limit( w )           /* assures window vars not out of bounds */
struct Window *w;
{ SS box = 0,box2 = 0;

db2( "win_limit" );
  if( w->w_flags & W_BOXED ) { box = 1; box2 = 2; }
  /* assure appearant window within vertual window */
/*** So' vai ser chamada em win_make()
  if( w->w_ax >= w->w_len ) w->w_ax = w->w_len - 1;
  if( w->w_ay >= w->w_height ) w->w_ay = w->w_height - 1;
  if( w->w_alen > w->w_len - w->w_ax ) w->w_alen = w->w_len - w->w_ax;
  if( w->w_aheight > w->w_height - w->w_ay )
         w->w_aheight = w->w_height - w->w_ay;
***/
  /* assure appearant window within main_win */
  if( w->w_alen + box2 > size_x ) w->w_alen = size_x - box2;
  if( w->w_aheight + box2 > size_y ) w->w_aheight = size_y - box2;
  if( w->w_x > size_x - w->w_alen - box ) 
        w->w_x = size_x - w->w_alen - box;
  if( w->w_y > size_y - w->w_aheight - box )
        w->w_y = size_y - w->w_aheight - box;
/*** Os dois if() de cima ja pegam isto !!
  if( w->w_x > size_x - box ) w->w_x = size_x - box;
  if( w->w_y > size_y - box ) w->w_y = size_y - box;
***/
  if( w->w_x < box ) w->w_x = box;
  if( w->w_y < box ) w->w_y = box;
}


WIN *   alloc_win( x,y )          /* allocate a window 0 = err */ 
SS x,y;                                        /* vertual size of window */
{

#ifdef   LIXO_asdf
         Funcionava.OK.Vou.economizar.dois.malloc()
  struct Window *ret;

db2( "alloc_win" );
  /* allocate the window */
  ret = ( struct Window *)malloc( sizeof( struct Window ));
  if( ! ret ) return 0;
  /* allocate window memory */
  ret->w_ary = ( US *)malloc( x * y * sizeof( US ));
  if( ! ret->w_ary ) { free( (SC *)  ret ); return 0; }
  ret->w_modblock = x / 8;
  if( x % 8 ) ret->w_modblock++;
  /* allocate modmap memory */
  ret->w_mod = ( UC *)malloc( ret->w_modblock * y + 10 );
  if( ! ret->w_mod )
        { free( (SC *)  ret->w_ary); free( (SC *)  ret ); return 0; }
  return ret;

#endif

   SS     M;
   UL     T;
   WIN    *ret;

   db2( "alloc_win" );

   M = x / 8;
   if ( x % 8 )  M++;

   /*** CUIDADO POIS A JANELA NAO PODE SER 255 * 255 ***/

   T =  sizeof( WIN )          /* allocate the window */
        + (x * y * sizeof(US)) /* allocate window memory */
        + (M * y + 10 );       /* allocate modmap memory */

   if ( T > 65000L )   return( NULL );

   ret = (WIN *) malloc( (SI) T );

   if ( ret == NULL )  return( NULL );

   ret->w_modblock = M;
   ret->w_ary = (US *) ((SC *)ret + sizeof(WIN));
   ret->w_mod = (UC *) ((SC *)ret + sizeof(WIN) + (x * y * sizeof(US)));

   return ret;
}

VOID   free_win( win )       /* undoes memory associated w/ window */
WIN    *win;
{

#ifdef   LIXO_qwer
         Funcionava OK Vou economizar dois malloc()
db2( "free_win" );
  free( (SC *)  win->w_ary );
  free( (SC *)  win->w_mod );
  free( (SC *)  win );
#endif

db2( "free_win" );

   free( (SC *)  win );
}

VOID  remap( l1,h1,dir )          /* modify win_map ( what was b is a ) */
SS l1,h1;                             /* the low & high value to remap */
SS dir;                               /* direction, if 1 add one 
                                          if -1 subtract */
{ SS l2,h2;                           /* box piece equivelant */
  register SS x;
  register UC *p;

  if( l1 > h1 ) return;
  l2 = l1 + 128; h2 = h1 + 128;
  for( x = size_x * size_y,p = win_map;x;x--,p++ )
       if(( *p >= l1 && *p <= h1 ) || ( *p >= l2 && *p <= h2 ))
               *p += dir;
}


