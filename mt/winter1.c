
#ifdef comout

This module controls the interrupt system.

fwintr( win )   interrupts reletive to this window MANAGED BY fwqgc()

#endif

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <window.h>
#include  <menu.h>
#include  <mensagem.h>
#ifdef    MSDOS
#include  <dos.h>
#endif

/* DECLARATIONS REQUIRED FOR PTR TO FUNCTIONS TO WORK */

static SS interrupt_cursor( );

#ifdef   MSDOS

SS     anterior( WIN * );
SS     posterior( WIN * );
SS     calendario( WIN * );
SS     ex_macro( WIN * );
SS     grava_macro( WIN * );
SS     calcula( WIN * );
SS     edita_arq( WIN * );
SS     status( WIN * );

ST( SS ) movechk( WIN * );
ST( SS ) sizechk( WIN * );
ST( SS ) amovchk( WIN * );

SS  domove( WIN * );
SS  dosize( WIN * );
SS  doamov( WIN * );
SS  doshel( WIN * );
SS  dorefr( WIN * );
SS  doprwi( WIN * );
SS  doprsc( WIN * );
SS  doquit( WIN * );

#else

SS     anterior( );
SS     posterior( );
SS     calendario( );
SS     ex_macro( );
SS     grava_macro( );
SS     calcula( );
SS     edita_arq( );
SS     status( );

ST( SS ) movechk( );
ST( SS ) sizechk( );
ST( SS ) amovchk( );

SS  domove( );
SS  dosize( );
SS  doamov( );
SS  doshel( );
SS  dorefr( );
SS  doprwi( );
SS  doprsc( );
SS  doquit( );

#endif

/***
Para acrescentar novas funcoes aumente MAXUTIL
   1   Edita       1L
   2   caLendario  2L
   3   Calcula     4L
   4   Status      8L
   5   DOS/Shell   16L
   6   Grava       32L
   7   eXecuta     64L
   8   Reescreve   128L
   9   aBandona    256L
   10  conFigura   512L
   11  Move        1024L
   12  Tamanho     2048L
   13  Virtual     4096L
   14  Imp_Tela    8192L
   15  Imp_Janela  16384L
   16  Anterior    32768L
   17  Posterior   65536L
***/
#define MAXUTIL 17              /* # OF INTERRUPTS */

static SC *prompts[ (MAXUTIL * 3) + 1 ];

static SS  (*doit[ MAXUTIL ])();

static WIN  *w_intr;

static SS interrupt_cursor( s ) /* Nao pode ser NEAR */

Selecao *s;
{
   if( w_intr->w_flags & W_BOXED )  {

      W_curmove( w_intr->w_x - 1,  w_intr->w_y - 1 );
   }
   else   W_curmove( w_intr->w_x,  w_intr->w_y );

   win_cpush( CS_INTER );
   doflush( );

   s->tecla = fwnqgc( s->win );

   win_cpop();
   doflush( );

   return ( -2 );
}

SS fwintr( win, mainflag )            /* do interrupt on this window */

WIN *win;
UL  mainflag;
{
   SS        a, I;
   SC        *ajuda;
#ifdef   MEDIT
   SS        j_col = 80, j_lin = 1;
#else
   SS        j_col = 80, j_lin = 2;
#endif
   Selecao   mi;
   static SC str_um[] = "1";
   static SS active = 0;
   static SS ut_quant = 0;

db1( "fwintr" );

   if( active ) return 0;
   stak2( win, "FWINTR" );

   /*** Se ja estava em Interrupcao, nao precisa carregar strings ***/
   if ( ut_quant++ == 0 )   up_str( STR_UTIL );

   /* find which interrupts are available */
   ajuda     = win_ajuda;
   win_ajuda = "utilitar";

   while ( 1 )
   {
      active = 1;

      /*** Para interrupt_cursor() posicionar cursor ***/
      w_intr = win;

      a = 0;

      if ( mainflag & U_EDITA )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_22 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_23 );

         doit[ a++ ] = edita_arq;
      }

      if ( mainflag & U_CALENDARIO )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_30 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_31 );

         doit[ a++ ] = calendario;
      }

      if ( mainflag & U_CALCULA )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_24 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_25 );

         doit[ a++ ] = calcula;
      }

      if ( mainflag & U_STATUS )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_20 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_21 );

         doit[ a++ ] = status;
      }

      if ( mainflag & U_SHELL )
      {
#ifdef   MSDOS
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_7 );
#else
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_8 );
#endif
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_9 );

         doit[ a++ ] = doshel;
      }

      if ( mainflag & U_GRAVA )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_26 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_27 );

         doit[ a++ ] = grava_macro;
      }

      if ( mainflag & U_EXECUTA )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_28 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_29 );

         doit[ a++ ] = ex_macro;
      }

      if ( mainflag & U_REESCREVE )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_10 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_11 );

         doit[ a++ ] = dorefr;
      }

      if ( mainflag & U_ABANDONA )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_14 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_15 );

         doit[ a++ ] = doquit;
      }

      if ( mainflag & U_CONFIGURA && fun_config != 0 )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_32 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_33 );

         doit[ a++ ] = fun_config;
      }

      if ( mainflag & U_MOVE && movechk( win ) )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_1 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_2 );

         doit[ a++ ] = domove;
      }

      if ( mainflag & U_TAMANHO && sizechk( win ) )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_3 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_4 );

         doit[ a++ ] = dosize;
      }

      if ( mainflag & U_VIRTUAL && amovchk( win ) )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_5 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_6 );

         doit[ a++ ] = doamov;
      }

      if ( mainflag & U_IMP_TELA )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_12 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_13 );

         doit[ a++ ] = doprsc;
      }

      if ( mainflag & U_IMP_JANELA )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_83 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_84 );

         doit[ a++ ] = doprwi;
      }

      if ( mainflag & U_ANTERIOR )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_16 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_17 );

         doit[ a++ ] = anterior;
      }

      if ( mainflag & U_POSTERIOR )
      {
         prompts[ a * 3 + 0 ] = loc_str( STR_UTIL, UT_18 );
         prompts[ a * 3 + 1 ] = str_um;
         prompts[ a * 3 + 2 ] = loc_str( STR_UTIL, UT_19 );

         doit[ a++ ] = posterior;
      }

      prompts[ a * 3 + 0 ] = NULL;

      I = -1;

      if ( a > 0 )
      {
         def_menu( &mi, j_col, j_lin, 60, 20, 1, MP_CAMPO, MP_BARRA, 1,
                  MP_JANELA, MP_DESTAQUE, &prompts,
                  loc_str( STR_UTIL, UT_34 ), loc_str( STR_UTIL, UT_35 ) );

         mi.comando = interrupt_cursor;

         I = menu( &mi );
      }

      /*** Libere Interrupcao ***/
      active = 0;

      if ( I == -1 )  goto done;

      if ( (*doit[ mi.escolhido ] )( win ) )   goto done;

      /*** Se anterior() ou posterior() tenha mudado de janela ***/
      win = w_intr;
   }

done:

   win_ajuda = ajuda;

   /*** So descarregue as strings, se nao mais precisar ***/
   if ( --ut_quant == 0 )   down_str( STR_UTIL );

   return 0;
}

/***************************** VALIDATION FUNCTIONS ***************************/

ST( SS ) movechk( win )
struct Window *win;
{ SS box;

  if( win->w_flags & W_BOXED) box = 1;
  else box = 0;
  if( win->w_x > box ) return 1;
  if( win->w_y > box ) return 1;
  if( win->w_alen + 2 * box < size_x ) return 1;
  if( win->w_aheight + 2 * box < size_y ) return 1;
  return 0;
}

ST( SS ) sizechk( win )
struct Window *win;
{ if( win == def_win ) return 0;
  if( win->w_len > 2 || win->w_height > 2 ) return 1;
  return 0;
}

ST( SS ) amovchk( win )
struct Window *win;
{
  if( win->w_alen < win->w_len ) return 1;
  if( win->w_aheight < win->w_height ) return 1;
  return 0;
}

SS anterior( win )

WIN   *win;
{
   SS  a;

   for ( a = win->w_num - 1;
         a > 0  &&  win_stak[ a ]->w_intr == 0;   a-- )
               ;
   if ( a > 0 )  win = win_stak[ a ];

   w_intr = win;

   return ( 0 );
}

SS posterior( win )

WIN   *win;
{
   SS   a;

   for ( a = win->w_num + 1;
         a < win_sp  &&  win_stak[ a ]->w_intr == 0;   a++ )
               ;
   if ( a < win_sp )  win = win_stak[ a ];

   w_intr = win;

   return ( 0 );
}

