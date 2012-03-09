
#ifdef comout

This module manages the main_win window, and does all terminal updating.

#endif


#include <stdio.h>
#include <string.h>

#ifdef   MSDOS
#include  <signal.h>
#endif

#include <window.h>

#ifdef     MSDOS

/*** Algumas destas funcoes estao em WMAIN0.C ***/
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

VOID  marklines()             /* report if a line has any mods */
{
  register SS a,b;
  register UC *p;

db2( "marklines" );
  for( a = 0;a < main_win->w_height;a++ )
        { w_linemod[ 0 ][ a ] = 0;
          w_linemod[ 1 ][ a ] = 255;
          p = main_win->w_mod + ( a * main_win->w_modblock );
          for( b = main_win->w_modblock;b;b--,p++ )
                if( *p ) /* mod found */
                        { w_linemod[ 0 ][ a ] = 255; break; }
        }
}


VOID  pr_char( val,lastatr,space,x,y )     /* print a character */

US   val;
SS   *lastatr,*space;                  /* flags associated w/ fixline */
SS   x, y;                             /* screen location to print */
{
   US true;
   SS atr;

   db2( "pr_char" );
   /* decide if eol optimizing worth doing */

   true = val >> 15; /*** Mostra sem conversao ***/
   val &= 0x7FFF;    /*** Limpe a flag de conversao ***/
   
   if ( *space )
   {
      if ( val != ' ' )
      {
         *space = 0;
         if ( bufend ) line_done( y, lastatr );
      }

      else
      {
         prbuf[ x ] = val;
         bufend = x + 1;
         return;
      }
   }

   /* locate cursor */
   if ( tcp_x != x  ||  tcp_y != y )   W_curmove( x, y );

   /* decide if atribute switch in order */
   if( ! is_atr( val )) atr = 0;

   else   atr = atr_of( val );

   if ( *lastatr != atr )
   {
      if ( *atrib_ary[ *lastatr ].at_off  &&
           *lastatr  &&
           ! ( *bad_atribs  &&  strchr((SC *) bad_atribs, *lastatr )) )
      {
         doputs( atrib_ary[ *lastatr ].at_off );
      }

/***
Em alguns terminais a sequencia que liga atributos deve ser "\E[0;1m" e ...
... nao apenas "\E[1m", pois em alguns casos certos caracteres nao ficam ...
... com o atributo correto, portanto temos de voltar ao NORMAL antes de ...
... ligar o que queremos. exemplo ANSI_SYS.LBT.
         doputs( atrib_ary[ 0 ].at_on );
***/
      if ( !atr || ! *bad_atribs || ! strchr((SC *) bad_atribs, atr ) )
      {
         doputs( atrib_ary[ atr ].at_on );
      }

      *lastatr = atr;
   }

   /* decide if UC is graphic */
   val = val_of( val );
   
   if ( ! true  &&  is_gr( val ))
   {
      val = gr_val( val );

      if ( val >= MAXGRAPH || ! *UG[ val ].ary )   
      {
         val = gr_val( UG_SP );
         if ( ! *UG[ val ].ary ) {  val = ' ';   goto manda_char;  }
      }

      if ( last_ug  !=  UG[ val ].type )
      {
         if ( last_ug )   ug_normal( ); 

         if ( (last_ug = UG[ val ].type) != 0 )
         {
            if ( last_ug >  MAX_SE )  last_ug = 0;
            
            if ( last_ug && *SE[ last_ug ].start )  
               doputs( SE[ last_ug ].start );
         }
      }
      
      doputs( UG[ val ].ary );
   }

   else
   {
manda_char:
      if ( last_ug )
         ug_normal( );
      if ( (UC) val < ' ' )   val = ' ';
      doputch( val );
   }

   if ( tcp_x >= 0 )  tcp_x++;

   if ( tcp_x >= size_x )
   {
      if ( lc_scrolls )   tcp_x = -1;
      else                tcp_x = size_x - 1;
   }
}

VOID  ug_normal( )
{
   if ( last_ug  &&  *SE[ last_ug ].end )
      doputs( SE[ last_ug ].end );
   last_ug = 0;
}


VOID  line_done( y,lastatr )
SS y;
SS *lastatr;
{
  register US *p;
  register SS x;
  SS xx = 0;

db2( "line_done" );
  for( x = 0,p = prbuf;x < bufend;x++,p++ )
       { if( ! *p ) continue;
         pr_char( *p,lastatr,&xx,x,y );
         *p = 0;
       }
  unbuf();
}

VOID  unbuf()                    /* clean up prbuf buffer */
{
  register US *p;
  register SS a;

db2( "unbuf" );
  for( p = prbuf,a = bufend;a;a--,p++ ) *p = 0;
  bufend = 0;
}

#define toxy( x,y ) ( main_win->w_ary + ( (y) * (US) size_x ) + (x) )

VOID  fixatr( y )/* put in atribute requests for wyse type atributes */
SS y;
{
  register US *p;
  register SS x;
  register UC *b;
  SS sx;
  SS width;
  SS inatrib = 0;
  SS a,c;
  static SS worst = -1;
  SS dud1,dud2;

db2( "fixatr" );
  width = size_x;
  if( y == size_y - 1 && lc_scrolls ) width--;

  /* FIGURE WORST OUT FIRST TIME */
  if( worst == -1 )
       { for( b = bad_atribs;*b;b++ )
               { if( atrib_ary[ *b ].at_onlen > worst )
                       worst = atrib_ary[ *b ].at_onlen;
                 if( atrib_ary[ *b ].at_offlen > worst )
                       worst = atrib_ary[ *b ].at_offlen;
               }
       }

  /* SCAN THE LINE REMOVING STARTS & ENDS THAT WERE THERE */
  p = main_win->w_ary + ( y * main_win->w_len );
  b = badary[y];
  dud1 = 0; dud2 = 0;
  sx = 0; /* apenas para livrar um warning mais adiante */
  for( x = 0;x < width;b++ )
       { if( ! *b ) { p += 8; x += 8; continue; }
         for( c = 1;c < 256;c <<= 1,p++,x++ )
               { if( ! (*b & c)) continue;
                 for( a = 0;a < worst;a++ )
                       pr_char( *( p + a ),&dud1,&dud2,x + a,y );
               }
       }
  w_setmem( badary[y],size_x / 8,0 );

  /* SCAN THE LINE FINDING STARTS & ENDS OF BAD ATRIBUTES */
  p = main_win->w_ary + ( y * main_win->w_len );
  for( x = 0;x < width;x++,p++ )
       { a = 0;
         if( is_atr( *p ))
               { a = atr_of( *p );
                 if( ! strchr( (SC *) bad_atribs,a )) a = 0;
               }
         if((! a ) && ! inatrib ) continue;
         if( a == inatrib ) continue;
         if( inatrib ) dofix( sx,x,y );
         if( a ) sx = x;
         inatrib = a;
       }
  if( inatrib )
       { dofix( sx,x,y ); }
}


VOID  dofix( start,end,y )            /* fix one atribute */
US start,end,y;
{ US atr;                             /* the atribute involved */
  SS a;

db2( "dofix" );
  if( start > end ) return;
  /***  Aqui estava assim
        atr = atr_of( toxy( start,y ) );
        porem dava erro de compilacao
   ***/
  atr = atr_of( *(toxy( start,y )) );
  /* set start to UC worth eating */
  start -= atrib_ary[atr].at_onlen;
  for( a = atrib_ary[atr].at_onlen * 2;a;a--,start++ )
       {
         if( may_i( start,y,atrib_ary[atr].at_onlen,atrib_ary[atr].at_ideo ))
               break;
         mark( start,y,( UC *)0 );
       }
  if( ! a ) return;          /* cancel atribute */
  /* set end to UC worth eating */
  end += atrib_ary[atr].at_offlen - 1;
  for( a = atrib_ary[atr].at_offlen * 2;a;a--,end-- )
       {
         if( may_i( end,y,atrib_ary[atr].at_offlen,atrib_ary[atr].at_ideo ))
               break;
         mark( end,y,( UC *)0 );
       }
  if( !a ) return;
  if( start >= end - 1 ) return;
  /* send end first, atribute flickers less */
  mark( end,y,atrib_ary[atr].at_off );
  mark( start,y,atrib_ary[atr].at_on );
}


SS  may_i( x,y,len,level )  /* see if safe to place atrib mark here */
                                  /* return 0, if may not */
SS x,y,len,level;
{ US *p;
  SS a,b,c;

  if( x < 0 ) return 0;
  if( x + len >= size_x ) return 0;
  if( y == size_y - 1 && lc_scrolls )
       { if( x + len >= size_x - 1 ) return 0; }
  p = toxy( x - len + 1,y );
  for( a = x;a < x + len;a++,p++ )
       { if( level == 0 )  /* level 0 may only overwrite white space */
                if( val_of( *p ) != ' ' ) return 0;
         if( level < 2 )   /* level 1 may not overwrite graphic */
                if( val_of( *p ) > 127 )  return 0;
         /* other terminator may not be overwritten */
         b = a % 8;
         for( c = 1;b;b--,c <<= 1 );
         if( badary[y][a / 8] & c ) return 0;
       }
  /* if no reason to not, then ok. */
  return 1;
}

VOID  mark( x,y,str )        /* put strange atribute mark @ x,y */
SS x,y;
UC *str;
{ SS a;
  SS b;

  db2( "mark" );
  if( x < 0 || x >= size_x ) return;
  a = x % 8;
  for( b = 1;a;a--,b <<= 1 );
  badary[y][x / 8] |= b;
  if( ! str ) return;
  W_curmove( x,y );
  doputs( str );
  tcp_x = -1;          /* don't assume cursor position after mark */
}


VOID  fixscn()         /* a number of lines need clearing, do them */
{
  SS a,b;
  register SS c;
  register US *p;

db2( "fixscn" );
  for( a = size_y - 1,b = 0;a > -1;a--,b++ )
       { if( w_linemod[ 0 ][ a ] ) continue;  /* line must be cleared */
          if( *do_cls || *do_eos )      /* check if always was blank */
               {
                 /* if any not white space, break */
                 for( c = size_x,p = main_win->w_ary + (a * size_x );
                      c;c--,p++ )
                       if( *p != ' ' ) break;
                 if( c ) break;
               }
         else break;
       }
  /* the bth bottom lines need clearing */
  if( b > 0 ) /* involk cl_eos */
        {
          if( b == size_y ) /* clear the bleetin screen */
          {
              prclear();
              return;
          }
          W_curmove( 0,size_y - b );
          pr_eos();            /* if no eos, then eol will be used */
        }
  for( ;a > -1;a-- )
        { if( w_linemod[ 0 ][ a ] )
                { W_curmove( 0,a );
                  pr_eol( main_win->w_len );
                }
        }
}


