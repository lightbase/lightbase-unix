
#ifdef comout

THIS MODULE WORKS A MEMORY MAP ( in this case IBM monochrome or color )
AS IF IT WERE A SIMPLE TERMINAL.

ANY STRANGE TERMINALS  should be custom coded here switched according
to strange_term

FUNCTIONS:

doputch()                                /* as per putchar() */
doputs()                                 /* as per puts (no appended \n) */
doflush()                                /* as per fflush( stdout ) */

uses strange_term

******* CONTROL **********
(Esc)Gxy   move cursor , ^A,^A is upper left
(Esc)U     move cursor up 1
(Esc)D     move cursor down 1
(Esc)R     move cursor right 1
(Esc)Ax    set to atribute x - Atencao NAO MUDAR ver PATR = s[2] em dos_mem()
(Esc)Bx    set bell type to x
(Esc)Cx    set cursor type to x
(Esc)S     clear screen
(Esc)L     clear line

NOTE: at present the memory mapping is undefined for UNIX.
      this may be amended if you desire UNIX memory mapping.

#endif

#include <stdio.h>
#include <window.h>

#define BUFSIZE 1024
#define WIDTH 80
#define HEIGHT 25

#ifdef MSDOS
static UC buf[BUFSIZE];
static SS bufoffs = 0;
#endif

#ifdef     MSDOS

ST( VOID ) bell( VOID );
ST( VOID ) cls( VOID );
ST( VOID ) setcurs( VOID );
ST( VOID ) findmap( VOID );
ST( VOID ) cll( VOID );
ST( VOID ) mputch( UC );
ST( VOID ) mputs( UC * );
ST( VOID ) mflush( VOID );
ST( US far * ) findcur( VOID );

#else

ST( VOID ) bell( );
ST( VOID ) cls( );
ST( VOID ) setcurs( );
ST( VOID ) findmap( );
ST( VOID ) cll( );
ST( VOID ) mputch( );
ST( VOID ) mputs( );
ST( VOID ) mflush( );
ST( US * ) findcur( );

#endif


#ifdef MSDOS

ST( VOID ) mputch( c )
UC c;
{
  buf[bufoffs++] = c;
  if( bufoffs == BUFSIZE ) mflush();
}

ST( VOID ) mputs( s )
UC *s;
{
  UC *p;
  for( p = s;*p;p++ )
        { buf[bufoffs++] = *p;
          if( bufoffs == BUFSIZE ) mflush();
        }
}

#define ATR 7

static US atX = 0,atY = 0,PATR = ATR,BELL = 1,BDUR = 10;
static SS CURSOR = 128,flag = 0, COLOR;
static US far *MAP = NULL;

#define is_atr( c )    (( c ) > 255 )
#define val_of( c )    (( c ) & 255 )
#define is_gr( c )     (( c ) & 128 )
#define gr_val( c )    (( c ) & 127 )


VOID  tipo_cursor_dos( cursor )

SC     *cursor;
{
   if( bufoffs )  mflush( );
   CURSOR = cursor[ 2 ];
   setcurs( );
}

/*** Deve ser STATIC ***/
static   SS ult_atr = 0;

/***
Esta funcao forca que dos_mem() set o proximo atributo corretamente ...
... usada em win_redo().
***/
VOID  forca_atr( )
{
   ult_atr = -1;
}

VOID   dos_mem( y, quant, buf )

US  y, quant, *buf;
{
   SS       atr;
   US       *p, val, ug_sp, true;
   register US far *q;

   ug_sp = *UG[ gr_val( UG_SP ) ].ary;
   if ( ug_sp == 0 )   ug_sp = 250;
   
   if( bufoffs )  mflush( );
   atX = 0;  atY = y;
   q = (US far *) MAP + (WIDTH * atY) + atX; /*** findcur(); ***/

   for( p = buf;   quant;   q++, atX++, p++ )
   {
      if ( *p == 0 )   continue;
      quant--;

      true = *p >> 15;  /*** Mostra sem conversao ***/
      *p  &= 0x7FFF;    /*** Limpe a flag de conversao ***/

      if ((atr = *p >> 8) >= MAXATRIB || ! *atrib_ary[atr].at_on )  atr = 0;

      if ( ult_atr != atr ){ PATR = atrib_ary[atr].at_on[2]; ult_atr = atr; }

      val = val_of( *p );
      
      if ( ! true  &&  is_gr( val ))
      {
         val = gr_val( val );
         if ( val >= MAXGRAPH || ! *UG[ val ].ary )  val = ug_sp;
         else                                        val = *UG[ val ].ary;
      }

      *q = val + ( PATR << 8 );
   }
}


ST( VOID ) mflush()                            /* do output */
{
  register UC *p;
  register US far *q;
  register SS a;

  if( ! bufoffs ) return;
  if( !MAP ) findmap();
/* process a character */
  q = findcur();
  for( p = buf,a = bufoffs;a;a--,p++ )
        { if( !flag )
                { switch( *p )
                        { case '\033': flag = 1;
                                       continue;
                          case '\n':
                                 nl: atY++;
                                     if( atY == HEIGHT )  atY = 0;
                          case '\r': atX = 0; q = findcur(); continue;
                          default: *q = *p + ( PATR << 8 );
                                   q++;
                                   atX++;
                                   if(!( atX % WIDTH )) goto nl;
                                   continue;
                         }
                }
          if( flag == 1 )
                { switch( *p )
                        { case 'G': flag = 2; continue;
                          case 'U':
                                    if( atY )
                                       { atY--; q -= WIDTH; }
                                    flag = 0;
                                    continue;
                          case 'D':
                                    if( atY < HEIGHT - 1 )
                                       { atY++; q += WIDTH; }
                                    flag = 0;
                                    continue;
                          case 'R':
                                    if( atX < WIDTH - 1 )
                                       { atX++; q++; }
                                    flag = 0;
                                    continue;
                          case 'A': flag = 4; continue;
                          case 'B': flag = 5; continue;
                          case 'C': flag = 7; continue;
                          case 'S': flag = 0;
                                    cls();
                                    atX = 0; atY = 0; q = MAP;
                                    continue;
                          case 'L': flag = 0; cll(); continue;
                          default:  flag = 0;
                                    *q = *p + (PATR << 8 );
                                    q++; atX++;
                                    if( ! atX % WIDTH ) goto nl;
                                    continue;
                        }
                }
          switch( flag )
                { case 2: atX = *p - 1; flag = 3; continue;
                  case 3: atY = *p - 1; flag = 0;
                          if( atX >= WIDTH ) atX = WIDTH - 1;
                          if( atY >= HEIGHT ) atY = HEIGHT - 1;
                          q = findcur();
                          continue;
                  case 4: PATR = *p; flag = 0; continue;
                  case 5: BELL = *p; flag = 6; continue;
                  case 6: BDUR = *p; flag = 0; bell(); continue;
                  case 7: CURSOR = *p;
                          flag = 0;
                          continue;
                }
          flag = 0;
        }
  bufoffs = 0;
/* REDISPLAY CURSOR */
  setcurs();
}

ST( VOID ) cll()            /* clears to atr from cursor to end of line */
{
  register US far *p;
  register SS x;
  US a;

  a = ' ' + ( PATR << 8 );                      /* clear value */
  for( p = findcur(),x = atX;(! x ) || ( x % WIDTH );x++,p++ ) *p = a;
}

ST( VOID ) cls()                    /* clears screen to atr */
{
  register US far *p;
  US a;
  register SS x;

  a = ' ' + ( PATR << 8 );
  for( x = WIDTH * HEIGHT,p = MAP;x;x--,p++ )  *p = a;
}

ST( US far * ) findcur()           /* returns ptr into map @ cursor */
{
  return ( US far *)MAP + ( WIDTH * atY ) + atX;
}

/*  DIRECT PC ACCESS  */

#include  <dos.h>

ST( VOID ) findmap( )
{
   geninterrupt( 17 );

  if( ( _AX  &  0x30 ) == 0x30 )
          { MAP = (US far *) 0xB0000000L;  /* MONOCHROME */
            COLOR = 0;
          }
  else    {
            MAP = (US far *) 0xB8000000L;  /* ASSUME COLOUR */
            COLOR =  1;
          }
}

ST( VOID ) setcurs( )
{
   static SS last = -1;
   UC top,bot;

/* SET CURSOR STYLE - if changed */
   if( last != CURSOR )
   {
      bot = 7;

      switch ( CURSOR )
      {
      default :
      case  1 :   top = 5;     break;
      case  2 :   top = 0x20;  break;
      case  3 :   top = 7;     break;
      case  4 :   top = 0;     break;
      }

      _CH = top;
      _CL = bot;
      _AH = 1;
      geninterrupt( 16 );

      last = CURSOR;
   }

/* POSITION CURSOR */
   _AH = 2;
   _DL = atX;
   _DH = atY;
   _BH = 0;
   geninterrupt( 16 );
}

ST( VOID ) bell()
{
   SS  tom;

   if ( BELL == 3  &&  BDUR == 3 )
   {
      for ( tom = 100;  tom <= 3000;  tom += 70 )
      {
         sound( tom );    delay( 1 );
      }
   }

   else {

      sound( BELL * 10 );         delay( BDUR * 3 );
/***
      sound( BELL * 30 - 50 );    delay( BDUR );
      sound( BELL * 30 );         delay( BDUR );
      sound( BELL * 30 - 50 );    delay( BDUR );
***/
   }

   nosound( );
}

#endif


/* set cursor location & style to that required for win */

VOID fwsetcursor( W )

WIN  *W;
{
   SS x, y, I, fora = 0;

   db1( "fwsetcursor" );

   /* move the cursor */
   x = W->w_x + W->w_cx - W->w_ax;
   y = W->w_y + W->w_cy - W->w_ay;

   if ((I = y * size_x + x)  >=  size_x * size_y )  fora = 1;

#ifdef   MSDOS
   if ( strange_term == 1 )
   {
      if ( bufoffs )  mflush( );

      /* decide: natural or hidden */
      if ( !fora && win_map[ I ] == W->w_num ) CURSOR = curstyle[ W->w_curstyle ][ 2 ];
      else                                     CURSOR = curstyle[ CS_HIDDEN ][ 2 ];

      W_curmove( x, y );
      goto fim;
   }
#endif

   W_curmove( x, y );

   /* decide: natural or hidden */
   if ( ! fora  &&  win_map[ I ] == W->w_num )   win_csset( W->w_curstyle );
   else                                          win_csset( CS_HIDDEN );

   doflush( );

fim:
   return;
}

VOID W_curmove( X, Y ) /* move the cursor to x,y (  0,0 =  upper left ) */

SS   X, Y;
{
   X = COTA( 0, X, size_x - 1 );
   Y = COTA( 0, Y, size_y - 1 );

#ifdef   MSDOS
   if ( strange_term == 1 )
   {
      if ( bufoffs )  mflush( );
      atX = X;   atY = Y;
      setcurs( );
      return;
   }
#endif

   WW_curmove( X, Y );
}

VOID doputs( s )                         /* destributed puts processing */
UC *s;
{
#ifdef MSDOS
  if( strange_term == 1 )    mputs( s );
  else
#endif
     fputs( ( SC * ) s, stdout );
}

VOID doputch( c )                        /* destributed putchar processing */
UC c;
{
#ifdef MSDOS
  if( strange_term == 1 )  mputch( c );
  else
#endif
     fputc( c,stdout );
}

VOID doflush()
{
#ifdef MSDOS
  if( strange_term == 1 ) mflush();
  else
#endif
     fflush( stdout );
}
