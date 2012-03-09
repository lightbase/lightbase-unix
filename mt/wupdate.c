
#ifdef comout

This module supplies a number of functions which deal with sub_window
to main_win communication.

wu_mark( win )       /* mark present x,y as being modified */
wu_lmark( win,l )    /* mark x,y for l, as being modified */
wu_update( win )     /* inform main window of mods in win */
wu_remap( x,y,l,h )  /* do full remapping ( win_map ) of windows */

#endif

#include <stdio.h>
#include <window.h>

#ifdef     MSDOS

ST( VOID ) locmap( US, US, US, US, US );

#else

ST( VOID ) locmap( );

#endif


VOID wu_mark( win )     /* mark current position in window as modified */

WIN    *win;
{
   SS    J, L;
   UC   *P, p_offset;

db1( "wu_mark" );
   if( win->w_anymod  >=  2 )   return;

  /* beyond appearant window ? */
   if( win->w_cx  <  win->w_ax   ||
       win->w_cy  <  win->w_ay   ||
       win->w_cx  >  win->w_alen + win->w_ax ||
       win->w_cy  >  win->w_aheight + win->w_ay )    return;

   if ( strange_term == 1 )
   {
      J = win->w_x + win->w_cx - win->w_ax;
      L = win->w_y + win->w_cy - win->w_ay;

      P = main_win->w_mod + (main_win->w_modblock * L) + (J / 8);

      if ( w_linemod[ 0 ][ L ]  <  J + 1 )   w_linemod[ 0 ][ L ] = J + 1;
      if ( w_linemod[ 1 ][ L ]  >  J )       w_linemod[ 1 ][ L ] = J;
   }

   else
   {
      J = win->w_cx;
      L = win->w_cy;

      P = win->w_mod + (win->w_modblock * L) + (J / 8);
   }

   p_offset = 1 << (J % 8);

   *P |= p_offset;

   win->w_anymod = 1;
}


/* mark as modified from current cursor for len bytes*/

VOID wu_lmark( win, len )

WIN  *win;
SS   len;              /* assume len is on one line ! */
{
   SS     J, K, L;
   UC    *P, p_offset;

db1( "wu_lmark" );

   if ( len <= 0  ||  win->w_anymod >= 2 )    return;

   /* beyond appearant window ? */
   if( win->w_cy  <  win->w_ay  ||
       win->w_cx  >  win->w_alen + win->w_ax  ||
       win->w_cy  >  win->w_aheight + win->w_ay )   return;

   if ( win->w_cx + len  >  win->w_alen + win->w_ax )
   {
      len = (win->w_alen + win->w_ax) - win->w_cx;
   }

   if ( strange_term == 1 )
   {
      if ( win->w_cx  <  win->w_ax )
      {
         J = win->w_ax - win->w_cx;

         if ( J  >=  len )   return;
         else                len -= J;

         J = win->w_x;
      }

      else  {  J = win->w_x + win->w_cx - win->w_ax;  }

      L = win->w_y + win->w_cy - win->w_ay;
   
      P = main_win->w_mod + (main_win->w_modblock * L) + (J / 8);

      if ( w_linemod[ 0 ][ L ]  <  J + len )   w_linemod[ 0 ][ L ] = J + len;
      if ( w_linemod[ 1 ][ L ]  >  J )         w_linemod[ 1 ][ L ] = J;
   }

   else
   {
      J = win->w_cx;
      L = win->w_cy;

      P = win->w_mod + (win->w_modblock * L) + (J / 8);
   }

   K = 255 << (J %= 8);

   p_offset = 8 - J;

   if ( len - p_offset <= 0 ) {  K &= 255 >> (p_offset - len);  *P |= K;  }

   else
   {
      *P++ |= K;
   
      len -= p_offset;

      while ( len > 8 ) {  len -= 8;  *P++ |= 255;  }

      if ( len > 0 )  {  K = 255 >> (8 - len);  *P |= K;  }
   }

   win->w_anymod = 1;
}


VOID   wu_update( win )    /* informs main_win of mods, clears mods */

WIN   *win;
{
   SS            a, b, mod_x, bulkflag = 0;
   US            c;
   SC            p_offset, q_offset;
   register UC   *p, *q, *r, *s;    /* p into win, q into main_win */

db1( "wu_update" );

   if( ! win->w_anymod )    return;

   if( win->w_anymod >= 2 ) bulkflag = 1;

   win->w_anymod = 0;

   if ( strange_term == 1  &&  ! bulkflag )   return;

   p = win->w_mod + (win->w_modblock * win->w_ay) + (win->w_ax / 8);
   q = main_win->w_mod + (main_win->w_modblock * win->w_y) + (win->w_x / 8);

   mod_x = (win->w_alen / 8);   if ( win->w_alen  % 8 )  mod_x++;
   
   p_offset = win->w_ax % 8;
   q_offset = win->w_x  % 8;

   for ( r = p, s = q, a = 0;   a < win->w_aheight;   a++ )
   {
      if ( strange_term == 1 )
      {
         if ( w_linemod[ 0 ][ win->w_y + a ]  <  win->w_x + win->w_alen )
         {
            w_linemod[ 0 ][ win->w_y + a ] = win->w_x + win->w_alen;
         }
         if ( w_linemod[ 1 ][ win->w_y + a ]  >  win->w_x )
         {
            w_linemod[ 1 ][ win->w_y + a ] = win->w_x;
         }
      }

      for ( b = 0;   b < mod_x;   b++, *p++ = 0, q++ )
      {
         if ( bulkflag )  c = 0xFF;
         else
         {
            c = *p >> p_offset;
            if ( p_offset )
               c |= *(p + 1) << (8 - p_offset);
         }

         if ( c )
         {
            *q |= c << q_offset;
            if ( q_offset )
               *(q + 1) |= c >> (8 - q_offset);
         }
      }

      p = (r += win->w_modblock);
      q = (s += main_win->w_modblock);
   }
}


static SS li, ri, ti, bi;        /* Left Internal, Right Internal etc. */

VOID   wu_remap( x, y, l, h )    /* rebuild win_map, and inform main_win */
                                 /* area to remap */
SS     x, y, l, h;
{
   SS       wsp;
   WIN      *w;
   register SS    a, b;
   register UC    *p;

/***
fprintf( stdprn, "\n\n\nWU_REMAP\n" );
***/
   /* ASSURE BASE REQUEST IS OK & LOAD li,ri etc. */

   if ( x >= size_x  ||  y >= size_y )   return;

   if ( x + l > size_x )   l = size_x - x;
   if ( y + h > size_y )   h = size_y - y;

   li = x;    ri = x + l;
   ti = y;    bi = y + h;

   /* FIND HIGHEST WINDOW COVERING ENTIRE AREA */

   for ( wsp = win_sp - 1;   wsp >= 0;   wsp-- )
   {
/***
fprintf( stdprn, "Janela %d\n", wsp );
***/

      w = win_stak[wsp];

      if ( w->w_flags & W_BOXED )
      {
         if ( w->w_x > li + 1              ||
              w->w_x + w->w_alen < ri - 1  ||
              w->w_y > ti + 1              ||
              w->w_y + w->w_aheight < bi - 1 )   continue;
      }

      else
      {
         if ( w->w_x > li              ||
              w->w_x + w->w_alen < ri  ||
              w->w_y > ti              ||
              w->w_y + w->w_aheight < bi )   continue;
      }

      break; /* this window covers entire update area */
   }

   /* UPDATE EACH WINDOW */

   if ( wsp == -1 )    wsp++;

   for (    ;   wsp < win_sp;   wsp++ )
   {
      w = win_stak[ wsp ];

      /* remap the window */
      locmap( w->w_x, w->w_y, w->w_alen, w->w_aheight, wsp );

      if( ! (w->w_flags & W_BOXED) )    continue;

/***
fprintf( stdprn, "Janela->FLAGS %d\n", w->w_flags );
***/
      /* remap in the box */
      /* top */
      locmap( w->w_x, w->w_y - 1, w->w_alen, 1, wsp + 128 );

      /* bottom */
      locmap( w->w_x, w->w_y + w->w_aheight, w->w_alen, 1, wsp + 128 );

      /* left */
      locmap( w->w_x - 1, w->w_y - 1, 1, w->w_aheight + 2, wsp + 128 );

      /* right */
      locmap( w->w_x + w->w_alen, w->w_y - 1, 1, w->w_aheight + 2, wsp+128 );
   }

   /* mark mapped area modified */

   for( a = y;   a < y + h;   a++ )
   {
      p = main_win->w_mod + (main_win->w_modblock * a) + (x / 8);

      if ( strange_term == 1 )
      {
         if ( w_linemod[ 0 ][ a ]  <  x + l )  w_linemod[ 0 ][ a ] = x + l;
         if ( w_linemod[ 1 ][ a ]  >  x )      w_linemod[ 1 ][ a ] = x;
      }

      for( b = l + 7;   b > 0;   b -= 8 )    *p++ = 255;
   }

  if( ! main_win->w_anymod )    main_win->w_anymod = 1;
}
       

ST( VOID )   locmap( x, y, l, h, v )

US    x, y, l, h, v;
{
   register UC   *p;
   register SS   a,b;
   unsigned      lw, rw, tw, bw;   /* left, right, top, bottom window */

   /* figure out subset of two boxes */
   lw = li;   if ( x > lw )      lw = x;
   rw = ri;   if ( x + l < rw )  rw = x + l;
   tw = ti;   if ( y > tw )      tw = y;
   bw = bi;   if ( y + h < bw )  bw = y + h;

   /* map in that zone */
   for( b = tw;   b < bw;   b++ )
   {
      for( a = lw, p = win_map + (b * size_x) + a;   a < rw;   a++, p++ )
      {
         *p = v;
      }
   }
}


#ifdef    Otimizar_WU_REMAP

Esta rotina ainda esta com muitos erros
- talvez nao precise marcar main_win->modblock e sim
  colocar win->anymod = 2, cuidado com as BORDAS !
- Para uma maior otimizacao deveriamos abrir todas as janelas ja com Bordas
  ou em FWBOX() e FWUNBOX() apenas remapear as bordas
- Tratar tambem FW_TITLE()

Em vez de chamar wu_remap() chamar w_map() em WIN_MAKE(), FWTOP() -> WMAKE.C
Em vez de chamar wu_remap() chamar w_map() em FWBOX()             -> WBLOCK.C


#ifdef     MSDOS
ST( VOID ) d_map( void );
#else
ST( VOID ) d_map( );
#endif

static   unsigned   WL, WR, WT, WB, WV; /* left, right, top, bottom window */

#define l_map(x,y,l,h,v){WL=(x);WR=(x)+(l);WT=(y);WB=(y)+(h);WV=(v);d_map();}


VOID   b_map( W )  /* rebuild box */

WIN   *W;
{
   SS             X, Y, wsp;
   register SS    a, b;
   register UC    *p, *s;

   wsp = W->w_num;
   X = W->w_x - 1;
   Y = W->w_y - 1;

   if ( X < 0  ||  Y < 0 )    return;

   /* top */
   l_map( X, Y, W->w_alen + 2, 1, wsp + 128 );

   /* bottom */
   l_map( X, W->w_y + W->w_aheight, W->w_alen + 2, 1, wsp + 128 );

   /* left */
   l_map( X, W->w_y, 1, W->w_aheight, wsp + 128 );

   /* right */
   l_map( W->w_x + W->w_alen, W->w_y, 1, W->w_aheight, wsp + 128 );

   /* mark mapped area modified */

   /* TOP */
   X = W->w_x - 1;
   Y = W->w_y - 1;
   p = main_win->w_mod + (Y * main_win->w_modblock) + (X / 8);

   if ( strange_term == 1 )
   {
      if ( w_linemod[ 0 ][ Y ]  <  X + W->w_alen + 2 )
      {
         w_linemod[ 0 ][ Y ] = X + W->w_alen + 2;
      }
      if ( w_linemod[ 1 ][ Y ]  >  X )
      {
         w_linemod[ 1 ][ Y ] = X;
      }
   }

   for ( b = 0;   b < W->w_modblock + 2;   b++ )
   {
      *p++ = 255;
   }

   /* BOTTOM */
   Y += W->w_aheight + 1;
   p = main_win->w_mod + (Y * main_win->w_modblock) + (X / 8);

   if ( strange_term == 1 )
   {
      if ( w_linemod[ 0 ][ Y ]  <  X + W->w_alen + 2 )
      {
         w_linemod[ 0 ][ Y ] = X + W->w_alen + 2;
      }
      if ( w_linemod[ 1 ][ Y ]  >  X )
      {
         w_linemod[ 1 ][ Y ] = X;
      }
   }

   for ( b = 0;   b < W->w_modblock + 2;   b++ )
   {
      *p++ = 255;
   }

   /* LEFT e RIGHT */

   Y = W->w_y;

   p = main_win->w_mod + (Y * main_win->w_modblock) + (X / 8);

   for ( s = p, a = 0;   a < W->w_aheight;   a++, Y++ )
   {
      if ( strange_term == 1 )
      {
         if ( w_linemod[ 0 ][ Y ]  <  X + W->w_alen + 2 )
         {
            w_linemod[ 0 ][ Y ] = X + W->w_alen + 2;
         }
         if ( w_linemod[ 1 ][ Y ]  >  X )
         {
            w_linemod[ 1 ][ Y ] = X;
         }
      }

      *p = 255;
      *(p + (W->w_alen / 8) ) = 255;
      *(p + (W->w_alen / 8) + 1 ) = 255;

      p = (s += main_win->w_modblock);
   }

  if ( ! main_win->w_anymod )    main_win->w_anymod = 1;
}


VOID   w_map( W )  /* rebuild win_map, and inform main_win area to remap */

WIN   *W;
{
   /* remap the window */

   l_map( W->w_x, W->w_y, W->w_alen, W->w_aheight, W->w_num );

   if( W->w_flags & W_BOXED )  b_map( W );

   W->w_anymod = 2;
}


ST( VOID )   d_map( )
{
   register UC   *p;
   register SS   a,b;

   for( b = WT;   b < WB;   b++ )
   {
      for( a = WL, p = win_map + (b * size_x) + a;   a < WR;   a++ )
      {
         *p++ = WV;
      }
   }
}

#endif

