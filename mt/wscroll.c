
#ifdef comout

This module holds the scroll & pan functions.

Functions included are:

        fwscroll( len,win )     - scroll to open new line(s) @ bottom of window
                                - if len is negative, opens new lines @ top

        fwpan( len,win )        - as scroll but opens new col @ right.

        fwinsline( win )        - opens the current line by moving info below
                                  down 1 line

        fwdelline( win )        - deletes the current line by moving info
                                  below up 1 line

#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <window.h>

#ifdef     MSDOS

VOID       seedominant( SS );        /*** Usadas em wput.c ***/
VOID       shiftmain( SS, SS );      /*** Usadas em wput.c ***/
ST( VOID ) blockjob( US, SS, WIN * );

#else
  
VOID       seedominant( );
VOID       shiftmain( );
ST( VOID ) blockjob( );

#endif


VOID fwscroll( len,win ) /* scroll a window ret: 0 or bad window */
                         /* cursor remains @ same location */
SS len;                  /* # of lines to scroll, if len < 0 then reverse */
struct Window *win;
{ register SS a;
  register US *p,*q;

db1( "fwscroll" );
/***  if( ! len ) { win_refresh(); return; }   ***/
  if( ! len ) { return; }   
  if( abs( len ) >= win->w_height )
        { fwcls( win );
          /*** win_refresh();  ***/
          return;
        }

   if ( strange_term != 1 )
   {
      /* try for block move */
      blockjob( 1,len,win );
   }

  /* in any case do scroll */
  if( len > 0 )
        { /* foreward scroll */
          for( p = win->w_ary,
               q = win->w_ary + ( len * win->w_len ),
               a = win->w_len * ( win->w_height - len );
               a;a--,p++,q++ )
                   *p = *q;
          for( a = win->w_len * len;a;a--,p++ )
            *p = ' ' + (win->w_atrib << 8);
        }
  else  { /* reverse scroll */
          len = len * -1;
          for( p = win->w_ary + ( win->w_height * win->w_len ) - 1,
               q = win->w_ary + ( win->w_len * ( win->w_height - len )) - 1,
               a = win->w_len * ( win->w_height - len );
               a;a--,p--,q-- )
                    *p = *q;
          for( a = win->w_len * len;a;a--,p-- )
            *p = ' ' + (win->w_atrib << 8);
        }
  win->w_anymod = 2;
/***  win_refresh(); ***/
}


VOID fwpan( len,win )  /* horizontal scroll, as fwscroll */
SS len;        /* how far to pan, if negative, script moves left to right */
struct Window *win;
{
  register SS a;
  register US *p,*q;
  SS b;

db1( "fwpan" );
/***  if( ! len ) { win_refresh(); return; } ***/
  if( ! len ) { return; }
  if( abs( len ) >= win->w_len )
        { fwcls( win );
          /*** win_refresh(); ***/
          return;
        }

   if ( strange_term != 1 )
   {
      /* try for block move */
      blockjob( 2,len,win );
   }

  if( len > 0 )
        { /* script right to left */
          /* shift all left */
          for( p = win->w_ary,
               q = win->w_ary + len,
               a = (win->w_len * win->w_height) - len;
               a;a--,p++,q++ )
                        *p = *q;
          /* then whipe out the scroll around */
          for( b = 0;b < win->w_height;b++ )
                 for(   a = len,
                        p = win->w_ary + ((b + 1) * win->w_len) - len;
                        a;a--,p++ )
                                *p = ' ' + (win->w_atrib << 8);
        }
  else  { /* script left to right */
          /* first shift all */
          len = len * -1;
          for( p = win->w_ary + ( win->w_len * win->w_height ) - 1,
               q = p - len,
          a = (win->w_len * win->w_height ) - len;
          a;a--,p--,q-- )
                *p = *q;
          /* then whipe out the scroll around */
          for( b = 0;b < win->w_height;b++ )
                for( a = len,p = win->w_ary + ( b * win->w_len );a;a--,p++ )
                        *p = ' ' + (win->w_atrib << 8);
        }
  win->w_anymod = 2;
/***  win_refresh(); ***/
}


VOID fwinsline( win )  /* inserts a line, by pushing lower info down */
struct Window *win;    /* makes blank line on cursor line */
{
  register SS a;
  register US *p,*q;
  SS line;

db1( "fwinsline" );

   if ( strange_term != 1 )
   {
      /* try for block move */
      blockjob( 3,1,win );
   }

  /* move the data */
  line = win->w_cy;
  for(  a = (win->w_height - line - 1 ) * win->w_len,
        p = win->w_ary + (win->w_len * ( win->w_height - 1) - 1 ),
        q = p + win->w_len;
        a;a--,p--,q-- )
                *q = *p;
  /* blank out present line */
  for(  a = win->w_len,p = win->w_ary + (win->w_len * line );
        a;a--,p++ ) *p = ' ' + (win->w_atrib << 8);
  win->w_anymod = 2;
/***  win_refresh();  ***/
}

VOID fwdelline( win )    /* deletes a line from win, leaving bottom blank */
struct Window *win;
{
  register SS a;
  register US *p,*q;
  SS line;

db1( "fwdelline" );

   if ( strange_term != 1 )
   {
      /* try for block move */
      blockjob( 4,1,win );
   }

  /* move the data */
  line = win->w_cy;
  for(  a = (win->w_height - line - 1 ) * win->w_len,
        p = win->w_ary + (win->w_len * line),
        q = p + win->w_len;
        a; a--,p++,q++ )
                *p = *q;
  /* blank out last line */
  for(  a = win->w_len,
        p = win->w_ary + (  win->w_len * ( win->w_height - 1 ));
        a;a--,p++ )
        *p = ' ' + (win->w_atrib << 8);
  win->w_anymod = 2;
/***  win_refresh();  ***/
}


ST( VOID ) blockjob( mode,len,win ) /* check if block moving is possible */
US mode;                      /* 1 = scroll, 2 = pan, 3 = insert, 4 = del */
SS len;                        /* for scroll/pan, # of lines */
struct Window *win;
{
  SS a;

db2( "blockjob" );
  /* check if possible */
  /* if any "bad_atribs", then can't */
  if( *bad_atribs )
      { register SS x,y;
        for( y = 0;y < size_y;y++ )
               for( x = 0;x < size_x / 8;x++ )
                       if( badary[y][x] ) return;
      }
  switch( mode )
        { case 1: if( len > 0 )
                        { if( lc_scrolls == 0 && ! *do_scroll ) return; }
                  else if( ! *do_unscroll ) return;
                  break;
          case 2: if( len > 0 )
                         if( ! *do_pan ) return;
                  else   if( ! *do_unpan ) return;
                  break;
          case 3: if( ! *do_insline ) return;
                  break;
          case 4: if( ! *do_delline ) return;
        }
  if( win_dominant == -1 ) seedominant( win->w_num ); /* sets win_dominant */
  if( win_dominant != win->w_num )
          return;
  /* this window is dominant, scroll, report & get out */
  win_refresh();     /* MUST REFRESH BEFORE MACHINE SCROLE, ELSE TROUBLE */
  switch( mode )
        { case 1: shiftmain( 0,len * size_x );/* report scroll to main_win */
                  if( len > 0 ) /* scroll */
                        {
                          for( a = len;a;a-- )
                                {
                                  W_curmove( size_x - 1,size_y - 1 );
                                  if( lc_scrolls )
                                        doputch( ' ' );
                                  else doputs( do_scroll );
                                }
                           break;
                         }
                  for( a = len * -1;a;a-- )     /* reverse scroll */
                         doputs( do_unscroll );
                  break;
          case 2: shiftmain( 0,len );   /* report pan to main_win */
                  if( len > 0 ) /* pan */
                        { for( a = len;a;a-- )
                                { doputs( do_pan ); }
                        }
                  else    for( a = len * -1;a;a-- ) /* revers pan */
                                doputs( do_unpan );
                  break;
          case 3: /* insert line */
                  shiftmain(( win->w_cy - win->w_ay + win->w_y) * size_x,
                             size_x * -1 );
                  W_curmove( 0,win->w_cy - win->w_ay + win->w_y );
                  doputs( do_insline );
                  break;
          case 4: /* delline */
                  shiftmain((win->w_cy - win->w_ay + win->w_y) * size_x,
                            size_x );
                  W_curmove( 0,win->w_cy - win->w_ay + win->w_y );
                  doputs( do_delline );
                  break;
        }
  tcp_x = -1; tcp_y = -1;
  main_win->w_anymod = 2;             /* winmain needs complete recheck */
  return;
}

VOID  seedominant( winnum )            /* check if this window dominates */
SS winnum;                             /* sets win_dominant if so */
{
  register SS a;
  SS b;

db2( "seedominant" );

   for( b = ((size_x * size_y) * 2)  / 10, a = size_x * size_y;   a;   a-- )
   {
      if( win_map[ a ] == winnum )  continue;

      if( --b  <=  0 ) {  win_dominant = -1;  return;  }
   }

   win_dominant = winnum;
}


VOID  shiftmain( offset,dist )   /* informs main_win of a shift */
SS offset;                             /* dist from start not affected */
SS dist;                               /* # of bytes to shift, may be neg. */
{
  register SS a;
  register US *p,*q;

db2( "shiftmain" );
  if( dist > 0 )                        /* positive shift ( scroll/pan ) */
        { for(  a = size_x * size_y - offset - dist,
                p = main_win->w_ary + offset,
                q = p + dist;
                a;a--,p++,q++ ) *p = *q;

          for(  a = dist, p = main_win->w_ary + ((size_x * size_y) - dist);
                a;  a--,  p++ )   *p = ' ';

          return;
        }
  if( dist < 0 )
       { for( a = size_x * size_y - offset + dist,
              p = main_win->w_ary + ( size_x * size_y ) - 1,
              q = p + dist;a;a--,p--,q-- )
                       *p = *q;
       }
  return;
}


