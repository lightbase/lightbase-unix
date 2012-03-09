
#ifdef comout

This module manages the main_win window, and does all terminal updating.

#endif


#include <stdio.h>
#include <string.h>
#include <signal.h>

#ifdef   MSDOS
#include  <signal.h>
#endif

#define WINMAIN          /* externs will be declared here */
#include <window.h>
#undef extern

/*** Usada em Winit.c ***/
SC  *env_term = "";
SC  *env_dir  = "";


#ifdef     MSDOS

/*** Algumas destas funcoes estao em WMAIN1.C ***/
VOID  ug_normal( void );
VOID  fixatr( SS );
VOID  dofix( US, US, US );
VOID  mark( SS, SS, UC * );
SS    may_i( SS, SS, SS, SS );
VOID  pr_char( US, SS *, SS *, SS, SS );
VOID  line_done( SS, SS * );
VOID  unbuf( VOID );
VOID  fixscn( VOID );
VOID  marklines( VOID );
VOID  fixline( SS );
VOID  pr_eos( VOID );
VOID  pr_eol( SS );

#else

VOID  ug_normal( );
VOID  fixatr( );
VOID  dofix( );
VOID  mark( );
SS    may_i( );
VOID  pr_char( );
VOID  line_done( );
VOID  unbuf( );
VOID  fixscn( );
VOID  marklines( );
VOID  fixline( );
VOID  pr_eos( );
VOID  pr_eol( );

#endif

VOID w_fixmain()      /* fix up the main window ( make terminal match ) */
{ register SS a;
  register US *p;

db1( "w_fixmain" );

   if ( w_refresh )   return;

  if( ! main_win->w_anymod )  goto fim;
  if( main_win->w_anymod == 3 /* clear screen */ )
        { main_win->w_anymod = 0; prclear();
          w_setmem( main_win->w_mod,
                    main_win->w_modblock * main_win->w_height, -1 );
          for( a = size_x * size_y,p = main_win->w_ary;a;a--,p++ ) *p = ' ';
          goto fim;
        }
  if( main_win->w_anymod == 2 /* touch */ )
        { w_setmem( main_win->w_mod,
                    main_win->w_modblock * main_win->w_height,-1 );
          w_setmem( w_linemod[ 0 ],main_win->w_height, 255 );
          w_setmem( w_linemod[ 1 ],main_win->w_height, 0 );
        }

  /* find which lines need modifying */
   else if ( strange_term != 1 )
   {
      marklines( );
      win_cpush( CS_OFF );
   }

   main_win->w_anymod = 0;

/***
fprintf( stdprn, "\n\n" );
***/

   for( a = 0;a < main_win->w_height;a++ )
   {
      if( w_linemod[ 0 ][ a ] )
      {
/***
fprintf( stdprn, "Linha = %d -> Esq[ 1 ] = %d - Dir[ 0 ] = %d\n",
                  a, w_linemod[ 1 ][ a ], w_linemod[ 0 ][ a ] );
***/

         fixline( a );
      }
   }

   if ( strange_term != 1 )
   {
      /* process clearing lines & toeos */
      for( a = 0;   a < main_win->w_height;   a++ )
      {
         if( w_linemod[ 0 ][ a ] )
         {
            fixscn();    /* check out any clear line sequences & fix */
            break;
         }
      }

      w_setmem(main_win->w_mod, main_win->w_modblock * main_win->w_height, 0);
      win_cpop();
   }

fim:
   if ( strange_term != 1 )   doflush( );
}

static WIN  *twin;             /* ptr into presently displayed window */
static UC *w;                  /* window map pointer */
static UC *r;                  /* modified flag pointer */
static UC *b;                  /* bad atributes, on/off flags */
static SS c;
static SS width;               /* width of useful screen, this point */
static US this;
static WIN *bw1, *bw2;         /* Box Window */
static US sl1, sl2, sl3, sl4;  /* Apoio para Titulo */
static SS bad;                 /* bad atribs involved, flag */
static SS lastatr;             /* the atribute that was */
static SS space;               /* eol command optimization */

VOID  fixline( y )        /* optimize a line update main_win & print */

SS y;
{
   register SS x;              /* x axis tracker */
   register US *p,*tptr;       /* map pointers */

   if ( strange_term  ==  1 )
   {
      for( p = prbuf, bufend = 0; bufend < MAXLENG; bufend++, p++ )  *p = 0;
      bufend = 0;
   }

   else
   {
      if( ! *do_eol )   space = 0;
      else              space = 1;
      bad = last_ug = lastatr = 0;
   }

   /* initialise stuff */
   p = main_win->w_ary + ( y * main_win->w_len );
   twin = 0;
   bw2  = 0;
   tptr = 0;
   x    = 0;
   w = win_map + ( y * size_x );
   r = main_win->w_mod + ( y * main_win->w_modblock );
   b = badary[y];
   width = size_x;

   if ( strange_term  !=  1 )
   {
      if ( ( y == size_y - 1 ) && lc_scrolls ) width--;
   }

   if ( strange_term  ==  1 )
   {
      width = MIN( width, w_linemod[ 0 ][ y ] );

      if ( (sl3 = w_linemod[ 1 ][ y ])  !=  255 )
      {
         sl1 = sl3 - (sl3 % 8);   x += sl1;  p += sl1;  w += sl1;
         sl2 = sl3 / 8;           r += sl2;  b += sl2;
      }
   }

   /* through the line, check for mods */

   for(   ;   x < width;   r++, b++ )
   {
      if ( ! *r )        /* no mods for next 8 bytes */
      {
         p += 8; w += 8; tptr += 8; x += 8;
         continue;
      }

      for( c = 1;   c < 256;   c <<= 1, p++, w++, tptr++, x++ )
      {
         if( x >= width )    break;

         if( ! (*r & c) )    continue;           /* no mod here */

         if ( strange_term  ==  1 )   *r &= ~c;

         /* find what UC 'should be' into "this" */

         if( *w > 127 )        /* box graphic and Title */
         {
            bw1 = win_stak[ *w - 128 ];
            bw1->type = COTA( 1, bw1->type, 2 );
            sl4 = this = 0;
            if ( x < bw1->w_x )                     this  = 1;
            if ( y < bw1->w_y )                     this += 2;
            if ( x == bw1->w_x + bw1->w_alen )      this += 4;
            if ( y == bw1->w_y + bw1->w_aheight )   this += 8;

            switch( this )
            {
            case   2:
                        this = UG_HOR + ((bw1->type - 1) * UG_LEN);

                        if ( bw2  !=  bw1 )
                        {
                           bw2 = bw1;
                           if ((sl1 = strlen( bw1->title )) > bw1->w_alen )
                           {
                              sl1 = bw1->w_alen;
                           }
                           sl2 = (bw1->w_alen - sl1) / 2;
                           sl3 = (bw1->w_alen - sl1) % 2;
                        }

                        if( sl1  &&  x >= bw1->w_x + sl2  &&
                            x < bw1->w_x + bw1->w_alen - sl2 - sl3)
                        {
                     	   sl4  = x - (bw1->w_x + sl2);
                     		this = (US) (bw1->title[ sl4 ] & 0xFF);
                           sl4  = this;
                        }

                        break;

            case   8:
                        this = UG_HOR + ((bw1->type - 1) * UG_LEN);

                        if ( bw2  !=  bw1 )
                        {
                           bw2 = bw1;
                           if ((sl1 = strlen( bw1->bottom )) > bw1->w_alen )
                           {
                              sl1 = bw1->w_alen;
                           }
                           sl2 = (bw1->w_alen - sl1);
                        }

                        if ( sl1  &&  sl2 <= x - bw1->w_x )
                        {
				               sl4 = x - (bw1->w_x + sl2);
				               this = (US) (bw1->bottom[ sl4 ] & 0xFF);
                        }

                        break;

            case   1:   this = UG_VER + ((bw1->type - 1) * UG_LEN);   break;
            case   4:   this = UG_VER + ((bw1->type - 1) * UG_LEN);   break;
            case   3:   this = UG_UL  + ((bw1->type - 1) * UG_LEN);   break;
            case   6:   this = UG_UR  + ((bw1->type - 1) * UG_LEN);   break;
            case  12:   this = UG_LR  + ((bw1->type - 1) * UG_LEN);   break;
            case   9:   this = UG_LL  + ((bw1->type - 1) * UG_LEN);   break;
            default :   this = UG_DOT;  /* should never happen */     break;
            }

            if ( sl4 == this )
            {
               if ( (strange_term == 1  &&  bw1->t_atr) ||
                    (bw1->t_atr  &&  !( *bad_atribs  &&
                     strchr( (SC *) bad_atribs, bw1->t_atr ))) )
               {
                  this += (US) (bw1->t_atr << 8 );
               }
            }

            else
            {
               if ( (strange_term == 1  &&  bw1->w_boxatr) ||
                    (bw1->w_boxatr  &&  !( *bad_atribs  &&
                     strchr( (SC *) bad_atribs, bw1->w_boxatr ))) )
               {
                  this += (US) (bw1->w_boxatr << 8 );
               }
            }
         }

         else
         {
            if( win_stak[ *w ] != twin )
            {
               /* new window being updated */
               twin = win_stak[ *w ];
               tptr = twin->w_ary +
                      (((y - twin->w_y) + twin->w_ay) * twin->w_len) +
                      ((x - twin->w_x) + twin->w_ax);
            }

            this = *tptr;
         }

         if( *p == this )   continue;

         /* MODIFICATION CONFIRMED OR BAD ATRIB INVOLVED, PROCESS */

         if ( strange_term != 1 )
         {
            if( *bad_atribs )
            {
               US one, two;
               one = *p;
               if ( is_atr( *p ) )
               {
                  if ( strchr( (SC *) bad_atribs,atr_of( *p )))
                  {
                     bad = 1;
                     one = val_of( *p );
                  }
               }

               two = this;
               if ( is_atr( this ))
               {
                  if( strchr( (SC *) bad_atribs,atr_of( this )))
                  {
                     bad = 1;
                     two = val_of( this );
                  }
               }

               if( *b  &  c )   bad = 1;

               else if( one == two )  /* atrib fix will fix it */
               {
                  *p = this;
                  continue;
               }
            }
         }

         if ( strange_term == 1 )  {  prbuf[ x ]= this;  bufend++;  }

         else  {  pr_char( this, &lastatr, &space, x, y );  }

         *p = this;
      }
   }

#ifdef   MSDOS
   if ( strange_term == 1 )
   {
      if ( bufend )  dos_mem( y, bufend, prbuf );

      tcp_x = tcp_y = -1;

      w_linemod[ 0 ][ y ] = 0;
      w_linemod[ 1 ][ y ] = 255;

      return;
   }
#endif

   /*** So executa isto se STRANGE != 1  ||  ! MSDOS ***/

   /* clean up */
   if( space )           /* double check, is line really empty? */
   {
      for( x = size_x, p = main_win->w_ary + (y * main_win->w_len);
         x;   x--, p++ )
      {
         if( *p != ' ' ) {  space = 0; break;  }
      }
   }

   if( bufend  &&  !space )
   {
      line_done( y, &lastatr );/* eol won't help, send */
   }

   if( bufend )   unbuf();

   if ( last_ug )
      ug_normal( );

   if ( *atrib_ary[ lastatr ].at_off  &&
         lastatr  &&
         ! ( *bad_atribs  &&  strchr((SC *) bad_atribs, lastatr )) )
   {
      doputs( atrib_ary[ lastatr ].at_off );
   }

   doputs( atrib_ary[ 0 ].at_on );

   if( ! space )   w_linemod[ 0 ][ y ] = 0;

   w_linemod[ 1 ][ y ] = 255;

   if( bad )   fixatr( y );     /* atributes need checking, this line */
}


VOID prclear()                    /* do a clear screen sequence */
{ SS a;

db2( "prclear" );

   doputs( atrib_ary[ 0 ].at_on );

  if( *do_cls )
        { doputs( do_cls ); tcp_x = 0; tcp_y = 0;
         /* clear out atribute marks too */
         if( *bad_atribs )
               for( a = 0;a < size_y;a++ )
                       w_setmem( badary[a],size_x / 8,0 );
         return;
       }
  W_curmove( 0,0 );
  pr_eos();
}

VOID  pr_eos()          /* assume cursor positioned, clear to eos */
{ SS a;

db2( "pr_eos" );
  /* assure badary fixed up */
  if( *bad_atribs )
       for( a = tcp_y;a < size_y;a++ )
               w_setmem( badary[a],size_x / 8,0 );
  if( *do_eos ) { doputs( do_eos ); tcp_x = -1; tcp_y = -1; return; }
  for( a = tcp_y;a < main_win->w_height;a++ )
        { W_curmove( 0,a );
          if( a == main_win->w_height - 1 && lc_scrolls )
                pr_eol( main_win->w_len - 1 );
          else  pr_eol( main_win->w_len );
        }
}

VOID  pr_eol( x )        /* from present cursor pos, to eol insert
                                  blanks, sets tcp_x to -1 */
SS x;                          /* # of chars between here & eol */
{ register SS a;

db2( "pr_eol" );
  if( *bad_atribs )
       w_setmem( badary[tcp_y],size_x / 8,0 );
  tcp_x = -1;
  if( *do_eol ) { doputs( do_eol ); return; }
  for( a = x;a;a-- ) doputch( ' ' );
}


SS  atr_of( p )          /* return the atribute associated w/ p */
US p;
{ SS a;

db2( "atr_of" );
  if( ! is_atr( p )) return 0;
  a = p / 256;
  if( a >= MAXATRIB ) a = 1;
  if( *atrib_ary[a].at_on ) return a;
  if( *atrib_ary[1].at_on ) return 1;
  return 0;
}

#ifdef   MSDOS
/* trap control C - Usada em Winit.c */

VOID  handler ( sig )

SI  sig;
{
   sig = sig;
   signal( SIGINT, handler );
   return;
}
#endif


VOID w_movmem( from, to, size )

UC   *from, *to;
SS   size;
{
   register UC *PP, *QQ;
   register SS AA;

   for( PP = from, QQ = to, AA = size;    AA;   AA-- )   *QQ++ = *PP++;
}


VOID w_movint( from, to, size )

US   *from, *to;
SS   size;
{
   register US *PP, *QQ;
   register SS AA;

   for( PP = from, QQ = to, AA = size;   AA;   AA-- )   *QQ++ = *PP++;
}


VOID w_setmem( from, len, value )

UC   *from;
US   len, value;
{
   register UC *PP;
   register US LL, VV;

   for( PP = from, LL = len, VV = value;   LL;   LL-- )    *PP++ = VV;
}


VOID w_setint( from, len, value )

US   *from;
US   len, value;
{
   register US *PP;
   register US LL, VV;

   for( PP = from, LL = len, VV = value;   LL;   LL-- )    *PP++ = VV;
}


