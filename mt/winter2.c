#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>

#include  <window.h>
#include  <mensagem.h>

#ifdef    MSDOS
#include  <alloc.h>
#include  <dos.h>
#endif

#ifdef   XENIX
SI       uname( );
#endif

#ifdef    PROTO_ANSI

SC  *sist_oper( SS, SC * );
SC  *datahora( SS, SC * );
SS  curmove( SS *, SS, SS, SC *, SC * );

#else

SC  *sist_oper( );
SC  *datahora( );
SS  curmove( );

#endif

#ifndef   MSDOS
#ifndef   VAXVMS
#include  <sys/utsname.h>
#endif
#endif

/**************************** TASK PERFORMING FUNCTIONS ***********************/

static SS cmx,cmy;      /* new requested location returned by curmove() */


SS curmove( lim, x, y, s1, s2  )   /* move the cursor driver */

SS  lim[4];
SS  x, y;
SC  *s1;
SC  *s2;
{
  WIN   *JM,  *JF;
  SS    a, ret;

  cmx = x; cmy = y;

  JM = abre_mensagem( s1 );
  JF = abre_funcoes( funcao( s2 ) );

  while( 1 )
       {
         W_curmove( cmx,cmy );
         win_cpush( CS_INTER );
         doflush( );
         a = fwnqgc( JM );
         win_cpop();
         switch( a )
               { case K_UP:  if( cmy > lim[1] ) { cmy--; continue; }
                             break;
                 case K_DN:  if( cmy < lim[3] ) { cmy++; continue; }
                             break;
                 case K_RT:  if( cmx < lim[2] ) { cmx++; continue; }
                             break;
                 case K_LFT: if( cmx > lim[0] ) { cmx--; continue; }
                             break;
                 case '\n':
                             ret = 1;
                             goto sai;
                 case K_ESC: cmx = x; cmy = y;
                             ret = 0;
                             goto sai;
               }
         /*** win_bell( BEL_INVALID );  ***/
       }

sai:

   fecha_funcoes(  JF );
   fecha_mensagem( JM );

   return ( ret );
}


SS domove( win )
struct Window *win;
{
   SS loc[4];
   SS box = 1;
   SC        *ajuda;

   ajuda     = win_ajuda;
   win_ajuda = "ut_move";

   /* calculate range of valid moves */
   if( ! ( win->w_flags & W_BOXED )) box = 0;
   loc[0] = 0;
   loc[1] = 0;
   loc[2] = size_x - win->w_alen - box - 1;
   loc[3] = size_y - win->w_aheight - box - 1;
   /* request the move  */
   curmove( loc, win->w_x - box, win->w_y - box,
           loc_str( STR_UTIL, UT_36 ), "N E");
   cmx += box; cmy += box;
   if( cmx != win->w_x || cmy != win->w_y )
   {
      fwmove( cmx,cmy,win );

      if( win->ident && move_win )   ( *move_win )( win );
   }

   win_ajuda = ajuda;
   return 1;
}

SS  dosize( win )
struct Window *win;
{
   SS loc[4];
   SS box = 1;
   SC        *ajuda;

   ajuda     = win_ajuda;
   win_ajuda = "ut_tam";

  /* calculate range of valid sizes */
  if( !( win->w_flags & W_BOXED )) box = 0;
  loc[0] = win->w_x + 2 + box;
  loc[1] = win->w_y + box;
  loc[2] = size_x - box;
  loc[3] = size_y - box;
  if( loc[2] > win->w_x + box + win->w_len - 1 )
       loc[2] = win->w_x + box + win->w_len - 1 ;
  if( loc[3] > win->w_y + box + win->w_height - 1)
       loc[3] = win->w_y + box + win->w_height - 1;
  if( ! curmove( loc,win->w_x + win->w_alen + box - 1,
                 win->w_y + win->w_aheight + box - 1,
                 loc_str( STR_UTIL, UT_37 ), "N E") )
       goto sai;
  cmx -= box;
  cmy -= box;
  cmx -= win->w_x;
  cmy -= win->w_y;
  fwsize( cmx + 1,cmy + 1,win );

  if( win->ident && move_win )   ( *move_win )( win );

sai:

   win_ajuda = ajuda;
   return 1;
}


SS   doamov( win )
struct Window *win;
{
   SS  limx;
   SS  limy;
   SS  presx,presy;
   SS  x_ant, y_ant;
   WIN *JM, *JF, *JI;
   SS  a;
   SC  *ajuda;

   ajuda     = win_ajuda;
   win_ajuda = "ut_virt";

  stak2( win, "DOAMOV" );

  JM = abre_mensagem( loc_str( STR_UTIL, UT_38 ) );
  JF = abre_funcoes( funcao( "N E" ) );
  JI = abre_insercol( "" );

  x_ant = presx = win->w_ax;
  y_ant = presy = win->w_ay;
  limx = win->w_len - win->w_alen;
  limy = win->w_height - win->w_aheight;
  while( 1 )
       {
         insercol( JI, "X = %-3d  Y = %-3d", presx, presy );

         fwamove( presx, presy, win );

         a = fwqgc( win );

         switch( a )
               { case K_UP: if( presy ) { presy--; continue; } break;
                 case K_DN: if( presy < limy ) { presy++; continue; }
                               break;
                 case K_LFT: if( presx ) { presx--; continue; } break;
                 case K_RT: if( presx < limx )
                               { presx++;
                                 continue;
                               }
                            break;
                 case K_ESC:
                               fwamove( x_ant, y_ant, win );
                 case '\n':
                             goto sai;
               }
         /** win_bell( BEL_INVALID ); ***/
       }

sai:

   fecha_insercol( JI );
   fecha_funcoes( JF );
   fecha_mensagem( JM );
   win_ajuda = ajuda;
   return 1;
}


SS  doshel( win )    /* spawn a local shell */

WIN   *win;
{
   win = win;
   /*** Executa o default, "command.com" no DOS e "shell.lb" no UNIX ***/
   win_shell( NULL );
   return 1;
}

SS  dorefr( win )                   /* refresh the terminal */
struct Window *win;
{
   win = win;
   win_redo();
   return 1;
}

SS  doprwi( win )
struct Window *win;
{
   win = win;
   fwprscrn( win );
   return 1;
}

SS doprsc( win )                   /* print the entire terminal */
struct Window *win;
{
   win = win;
   fwprscrn( main_win );
   return 1;
}

SS doquit( win )                   /* terminate program */
struct Window *win;
{
   win = win;
   win_exit( 1 );
   return( 1 );
}


SS  status( win )

WIN  *win;
{
   WIN  *pop;
   SC   S1[ 40 ];
   SC   S2[ 40 ];
   SS   ret;
#ifdef  MEDIT
   SS   lin = 1;
#else
   SS   lin = 2;
#endif
   SC        *ajuda;
   WIN *salva_win = NULL;
   SS  salva_x, salva_y, salva_seg;
   SC  *salva_fmt;

   ret = 0;   win = win;

#ifdef MSDOS
   pop = win_make( 30, 10, 30, 10, size_x, lin );
#else
   pop = win_make( 30, 9, 30, 9, size_x, lin );
#endif

   if ( pop == NULL )    return( 0 );

   ajuda     = win_ajuda;
   win_ajuda = "ut_stat";

   fw_title( pop, loc_str( STR_UTIL, UT_39 ), pop->t_atr );

   fw_bottom( pop, loc_str( STR_UTIL, UT_40 ) );

   /* colocado por ananias em 09/02/95 para setar alarm */
   if ( hm_win != NULL )
   {
      salva_win = hm_win;
      salva_x   = hm_x;
      salva_y   = hm_y;
      salva_seg = hm_seg;
      salva_fmt = hm_fmt;
   }

   hm_halt( );

   hm_set( pop, strlen( loc_str( STR_UTIL, UT_44 )) + 3, 4, 1,
           "%2.2d:%2.2d:%2.2d" );

   fwat( 0, 0, pop );
   fwprintf( pop, " %s%s\n %s%s\n\n",
                  loc_str( STR_UTIL, UT_41 ), versao,
                  loc_str( STR_UTIL, UT_42 ), edicao );
   fwprintf( pop, " %s: %s\n %s: %s",
                  loc_str( STR_UTIL, UT_43 ), datahora( 0, S1 ),
                  loc_str( STR_UTIL, UT_44 ), datahora( 1, S2 ) );
   fwprintf( pop, "\n\n %s: %s", loc_str( STR_UTIL, UT_45 ), sist_oper( 1, S2 ));
#ifndef     MSDOS
   fwprintf( pop, "\n %s: %s", loc_str( STR_UTIL, UT_46 ), sist_oper( 2, S2 ));
   fwprintf( pop, "\n %s: %s", loc_str( STR_UTIL, UT_47 ), sist_oper( 3, S2 ));
#else
   fwprintf( pop, "\n\n %s\n %s: %ld %s",
                  loc_str( STR_UTIL, UT_48 ),
                  loc_str( STR_UTIL, UT_49 ), farcoreleft( ),
                  loc_str( STR_UTIL, UT_50 ) );
#endif

   fwrefresh( pop );
   fwsetcursor( pop );

   while ( 1 )
   {

#ifdef     MSDOS

	   fwat( 0, 4, pop );
	   fwprintf( pop, " %s: %s",
      	            loc_str( STR_UTIL, UT_44 ), datahora( 1, S2 ) );
      fwrefresh( pop );
      fwsetcursor( pop );

#endif

      if ( w_kbhit( pop )  ||  rd_macro != NULL )
      {
         switch( fwnqgc( pop ) )
         {
         case K_ESC   :   ret = 1;    goto fim;
         case K_ENTER :   ret = 0;    goto fim;
         default      :   /*** win_bell( BEL_INVALID ); ***/  break;
         }
      }
   }

fim:

   hm_halt( );

   if ( salva_win != NULL )
   {
      hm_set( salva_win, salva_x, salva_y, salva_seg, salva_fmt );
   }

   fwkill( pop );

   win_ajuda = ajuda;
   return ( ret );
}


SC *sist_oper( flag, str )

SS   flag;
SC   *str;
{
#ifdef    MSDOS
   UC     ver, rel;

   flag =flag;
   _AX = 0x3000;
   geninterrupt( 0x21 );

   ver = _AL;
   rel = _AH;

   sprintf( str, "DOS %d.%d", ver, rel );
   return ( str );

#else
#ifdef   VAXVMS

   return ( "VMS" );

#else

   struct utsname sistema;

   if ( uname( &sistema ) < 0 )   str[ 0 ] = '\0';
   else
   {
      switch ( flag )
      {
      default:
      case 1 :
               strcpy( str, sistema.sysname );
               break;
      case 2 :
               strcpy( str, sistema.release );
               break;
      case 3 :
               strcpy( str, sistema.version );
               break;
      }
   }

   return ( str );
#endif
#endif
}


SC *datahora( flag, str )

SS   flag;
SC   *str;
{  SS acerto = 0;
   time_t     T;
   struct tm *S;

#ifdef  XTOOLS
   extern SL timezone_xtools;
   timezone_xtools = 8 * 60 * 60;
#endif

#ifdef MSDOS
   /* no turboc++ tava dando uma hora a menos */
   T=0;
   S = localtime( &T );
   if ( S->tm_hour == -5 )
      acerto = 1;
#endif

   time( &T );
   S = localtime( &T );

   if ( ! flag ) /*** Data ***/
   {
      sprintf( str, "%3.3s %2.2d %3.3s %4.4d",
                    loc_str( STR_UTIL, S->tm_wday + 64 ),
                    S->tm_mday,
                    loc_str( STR_UTIL, S->tm_mon + 71 ),
                    S->tm_year + 1900 );
   }

   else   /*** HORA ***/
   {

      extern int daylight;

      sprintf( str, "%2.2d:%2.2d:%2.2d", S->tm_hour + acerto,
                                         S->tm_min, S->tm_sec );
   }

   return ( str );
}


