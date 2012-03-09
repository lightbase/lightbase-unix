
#include <stdio.h>
#include <string.h>

#ifdef VAXVMS
#include <file.h>
#else
#include <fcntl.h>
#endif

#include <window.h>
#include "mensagem.h"
#include "menu.h"

#include PROTO

#ifdef   MSDOS
#include  <io.h>
#endif

#include  <ethead.h>

#ifdef    MEDIT

#define   ALT_AJD    size_y - 4
#define   LAR_AJD    size_x - 2
#define   X_VER      1
#define   Y_VER      1
#define   X_EDI      1
#define   Y_EDI      1

#else
#ifdef    XTOOLS

#define   ALT_AJD    size_y - 6
#define   LAR_AJD    size_x - 2
#define   X_VER      1
#define   Y_VER      2
#define   X_EDI      1
#define   Y_EDI      2

#else

#define   ALT_AJD    10
#define   LAR_AJD    50
#define   X_VER      18
#define   Y_VER      7
#define   X_EDI      1
#define   Y_EDI      2

#endif
#endif

#define   TAM_LINHA  128
#define   MAX_ITENS  20
#define   SEP_CAB    ','
#define   SEP_ITEM   '>'

static SC lhelp[ TAM_LINHA + 5 ];
static SC dir_menu[ 42 ];  /* 40 para diretorio de "arqmenu" */

#define   COLUNA     XYLA[ 0 ]
#define   LINHA      XYLA[ 1 ]
#define   LARGURA    XYLA[ 2 ]
#define   ALTURA     XYLA[ 3 ]

static SS SuperEsc = 0;

SS    com_help( MS )
Selecao   *MS;
{
   SS  C;

   if ( SuperEsc )   return  -3;

   C = MS->tecla = fwnqgc( MS->win );

   if ( C == K_F0 ) {  SuperEsc = 1;   w_refresh = 11;  }

   if ( SuperEsc  ||  C == K_ESC  ||  C == K_F6 )   return  -3;

   return  -2;
}

SS    exe_help( MS )
Selecao   *MS;
{
   f_ajuda( (*MS->op_ajuda)[ MS->escolhido ], 1 );

   return ( MS->escolhido );
}

#ifdef   MSDOS
SS    menu_help( SC *arqmenu )
#else
SS    menu_help( arqmenu )
SC    *arqmenu;
#endif
{
   SS    I, A, QI, TM, XYLA[ 4 ], ret = 0;
   SC    *P, *S, *T = NULL, *R = NULL, *tmenu;
   SC    *itens[ MAX_ITENS + 2 ];
   SC    *arqs[  MAX_ITENS + 2 ];
   FILE  *FA = NULL;
   Selecao   MS;

   for ( I = 0;   I < MAX_ITENS;   I++ )   itens[ I ] = arqs[ I ] = NULL;

   if ( (FA = fopen( arqmenu, MODO_RO )) == NULL  ||
        fgets( lhelp, TAM_LINHA, FA ) == NULL  ||
        (I = strlen( lhelp )) == 0  )   goto fim;

   if ( lhelp[ I - 1 ]  !=  '\n' )   ate_nl( FA );
   else                              lhelp[ I - 1 ] = '\0';

   /*** Vamos ver se e' MENU ***/
   if ( strcmp( eliminabr( lhelp ), loc_str( STR_HELP, HELP_7 )))  goto fim;

   if ((TM = strlen( tmenu = loc_str( STR_HELP, HELP_8 ))) == 0 )  goto fim;

   /*** Isolaremos o diretorio do arquivo de MENU ***/
   sprintf( dir_menu, "%.40s", arqmenu );
   for ( P = dir_menu + strlen( dir_menu );
         P >= dir_menu  &&  *P != '\\'  &&  *P != '/';   P-- )   /* Nada */;
   *(++P) = 0;

   /*** Procuremos o cabecalho ***/
   while ( 1 )
   {
      if ( fgets( lhelp, TAM_LINHA, FA ) == NULL )   goto fim;

      if ((I = strlen( lhelp )) == 0 )   continue;

      if ( lhelp[ I - 1 ]  !=  '\n' )    ate_nl( FA );
      else                               lhelp[ I - 1 ] = '\0';

      for ( P = lhelp;   *P == ' ' && *P == TAB_CHAR;   P++ ) /* Nada */;

      if ( strncmp( P, tmenu, TM ) == 0 )   break;
   }

   /*** "lhelp" possui o cabecalho ***/

   /*** Vamos pegar o TITULO ***/
   S = P += TM;
   if ((P = strchr( P, SEP_CAB )) == NULL )   goto fim;
   *P = 0;
   if ((T = malloc( strlen( S ) + 1 )) == NULL )   goto fim;
   strcpy( T, S );

   /*** Agora a COLUNA, LINHA, LARGURA, ALTURA ***/
   for ( I = 0;   I < 4;   I++ )
   {
      for ( P++;   *P == ' ' || *P == TAB_CHAR;   P++ ) /* Nada */;
      XYLA[ I ] = atoi( P );
      for (    ;   *P  &&  *P != SEP_CAB;   P++ ) /* Nada */;
      if ( *P == 0  &&  I < 3 )   goto fim;
   }

   LARGURA = MAX( LARGURA, 2 );
   ALTURA  = MAX( ALTURA,  2 );

   if ( *P == SEP_CAB )   P++;
   if ( *P == ' ' || *P == TAB_CHAR )   P++;

   /*** Vamos pegar o RODAPE ***/
   if ((R = malloc( strlen( P ) + 1 )) == NULL )   goto fim;
   strcpy( R, P );

   /*** Vamos a procura dos ITENS e NOMES de aequivos ***/
   for ( QI = 0;   QI < MAX_ITENS;    )
   {
      if ( fgets( lhelp, TAM_LINHA, FA ) == NULL )   break;

      if ((I = strlen( lhelp )) == 0 )   continue;

      if ( lhelp[ I - 1 ]  !=  '\n' )    ate_nl( FA );
      else                               lhelp[ I - 1 ] = '\0';

      if ( lhelp[ 0 ] == 0 )   continue;

      /*** Garantimos que possuimos um separador e um nome de arquivo ***/
      if ((S = strrchr( lhelp, SEP_ITEM )) == NULL )    continue;
      eliminabr( ++S );
      if ( *S == 0 )   continue;

      /*** Vamos Alocar e preencher os itens ***/
      if ((itens[ QI ] = malloc( LARGURA + 1 )) == NULL )   goto fim;

      for ( I = 0, P = lhelp;   I < LARGURA;   I++ )
      {
         if ( P < S - 1 )   itens[ QI ][ I ] = *P++;
         else               itens[ QI ][ I ] = ' ';
      }
      itens[ QI ][ I ] = 0;

      /*** Agora Alocaremos e preencheremos os arquivos ***/

      if ( *S == '!' )
      {
         A = strlen( diretorio + 1 ) + strlen( S + 1 );
         if ((arqs[ QI ] = malloc( A + 5 )) == NULL )   goto fim;
         sprintf( arqs[ QI ], "%s%c%s", diretorio, DIRSEP, (S + 1) );
      }

      else
      {
         A = strlen( dir_menu ) + strlen( S );
         if ((arqs[ QI ] = malloc( A + 5 )) == NULL )   goto fim;
         sprintf( arqs[ QI ], "%s%s", dir_menu, S );
      }

      /*** Somente aqui incremente QI ***/
      QI++;
   }
   itens[ QI ] = arqs[ QI ] = NULL;

   /*** Temos todos os dados do menu() vamos chama-lo ***/

   def_escolha( &MS, COLUNA, LINHA, LARGURA + 2, ALTURA, 1,
                MP_CAMPO, MP_BARRA, 1, MP_JANELA, MP_DESTAQUE,
                &itens, T, R );

   MS.comando  = com_help;
   MS.executa  = exe_help;
   MS.op_ajuda = &arqs;

   menu( &MS );

   if ( SuperEsc )           ret = 3;
   if ( MS.tecla == K_F6 )   ret = 2;
   else                      ret = 1;

fim:

   if ( FA != NULL )   fclose( FA );

   if ( T != NULL )   free( T );
   if ( R != NULL )   free( R );

   for ( I = 0;   itens[ I ] != NULL;   I++ )   free( itens[ I ] );

   for ( I = 0;   arqs[  I ] != NULL;   I++ )   free( arqs[  I ] );

   return ( ret );
}

VOID  f_ajuda( str1, flagmenu )

SC    *str1;
SS    flagmenu;
{
   SS    edita_arq;
   SC    nome[ 60 ];
   SC    escape[ 5 ];
   SC    f_str[ 25 ];
   WIN   *win = NULL;
   WIN   *JF, *JM;
   ARG   *args;
   static SC ajd_flag = 0;

   if ( ajd_flag  >  10 )   return;
   if ( ajd_flag  &&  ! flagmenu )   return;

   if ( ! ajd_flag++ ) {  SuperEsc = 0;   up_str( STR_HELP );  }

   edita_arq = 0;

   sprintf( f_str, "3 5 6 0%.12s E", loc_str( STR_HELP, HELP_9 ) );

   JM = abre_mensagem( loc_str( STR_HELP, HELP_1 ) );
#ifndef   XTOOLS
   JF = abre_funcoes( funcao( f_str ) );
#endif

   /*** Se estiver em menu_help() o arquivo esta resolvido ***/
   if ( !flagmenu && user_help != NULL && *user_help != '\0' )  str1 = user_help;

   if ( str1 == NULL  ||  *str1 == '\0' )
   {
      debug( "%s", loc_str( STR_HELP, D_ARQ_INVAL ) );
      goto  fim1;
   }

   strlwr( str1 );

   /*** A funcao menu_help() resolve o diretorio ***/
   if ( flagmenu || str1 == user_help )   strcpy( nome, str1 );

   else if ( dir_ajuda != NULL )
             sprintf( nome, "%.40s%c%.8s",
             dir_ajuda, DIRSEP, str1 );

   else      sprintf( nome, "%.40s%c%.8s%c%.8s",
             diretorio, DIRSEP,
             loc_str( STR_HELP, DIR_AJD ), DIRSEP, str1 );

   if( access( nome, 4 ) )
   {
      win_bell( BEL_GEN );
      mens_1( JM, loc_str( STR_HELP, M_AJD1 ), nome );
      fwqgc( JM );
      edita_arq = 1;
      goto fim2;
   }

   switch ( menu_help( nome ) )
   {
   default:
   case  0: break;         /* Nao e' MENU. */

   case  1: goto fim1;     /* E' Menu porem encerre, ja' foi tratado. */

   case  2:                /* E' Menu, porem edite-o. */
            edita_arq = 1;
            goto fim2;

   case  3: goto fim1;     /* E' Menu porem encerre, SuperEsc. */
   }

   win = win_make( LAR_AJD, ALT_AJD, LAR_AJD, ALT_AJD, X_VER, Y_VER );

   if ( win == NULL )   goto  fim1;

   fwbot( win );

   win->type    = 2;
   win->ident   = 255;
   win->w_flags = W_NOSCROL;

   fw_title( win, loc_str( STR_HELP, T_AJUDA ), win->w_boxatr );

   sprintf( escape, "%c%c%c", K_F6, K_F0, K_ESC ); /* Apenas 3 */

   if ( (args = load_buffer( LAR_AJD - 1, ET_FILE, nome, 0L )) != NULL )
   {
      args->escape   = escape;
      args->def_mens = loc_str( STR_HELP, HELP_1 );
      s_edit_buffer( args, ET_NAVEGA, win, JM, 0 );
      if ( args->tecla == K_F6 )    edita_arq = 2;
      if ( args->tecla == K_F0 ) {  SuperEsc  = 1;  w_refresh = 11;  }
      free_buffer( args );
   }

fim2:

   if ( win != NULL )   fwkill( win );

   if ( edit_help  &&  edita_arq )
   {
#ifndef   XTOOLS
      funcoes( JF, funcao( "2 3 5 E" ) );
#endif

      win = win_make( LAR_AJD, ALT_AJD, LAR_AJD, ALT_AJD, X_EDI, Y_EDI );

      if ( win == NULL )   goto  fim1;

      fwbot( win );

      edit_arquivo( nome, win,
                    (edita_arq == 1) ? 0 : LAR_AJD - 1,
                    loc_str(STR_HELP, M_ET_HELP), 1);

      fwkill( win );
   }

fim1:

   fecha_mensagem( JM );
#ifndef   XTOOLS
   fecha_funcoes(  JF );
#endif

   if ( --ajd_flag == 0 )  {  down_str( STR_HELP );   w_refresh = 0;  }
}


SS    w_navega( w_cursor, w_navega, quant, escape )

WIN   *w_cursor;
WIN   *w_navega;
SS    quant;
SC    *escape;
{
   SS   J;
   WIN  *W;
   SS  zoom, Y, L;

   W = w_navega;
   zoom = 0;
   Y = W->w_y;
   L = W->w_aheight;

   while ( 1 )
   {
      fwrefresh( W );

      J = fwqgc( w_cursor );

      if ( strchr( escape, J ) != NULL )   break;

      switch ( J )
      {
      default :
		  win_bell( BEL_INVALID );
		  break;
      case K_ESC :
		  goto fim2;
      case K_F5 :
		  if ( ! zoom )
		  {
		     zoom = 1;
		     fwmove( W->w_x, 2, W );
		     fwsize( W->w_alen, size_y - 5, W );
		  }
		  else
		  {
		     zoom = 0;
		     fwsize( W->w_alen, L, W );
		     fwmove( W->w_x, Y, W );
		  }
		  break;
      case K_DN :
		  if ( W->w_ay  <  quant - W->w_aheight + 1 )
		  {
		     fwamove( W->w_ax, W->w_ay + 1, W );
		  }
		  break;
      case K_UP :
		  if ( W->w_ay )
		  {
		     fwamove( W->w_ax, W->w_ay - 1, W );
		  }
		  break;
      case K_PGDN :
		  W->w_ay  +=  W->w_aheight;

		  if ( W->w_ay  >  quant - W->w_aheight + 1 )
		  {
		     W->w_ay = quant - W->w_aheight + 1;
		  }
		  fwamove( W->w_ax, W->w_ay, W );
		  break;
      case K_PGUP :
		  W->w_ay  -=  W->w_aheight;
		  if ( W->w_ay  >  W->w_height )
		  {
		     W->w_ay = 0;
		  }

		  fwamove( W->w_ax, W->w_ay, W );
		  break;
      case K_CPGDN :
		  fwamove( 0, quant - W->w_aheight + 1, W );
		  break;
      case K_CPGUP :
		  fwamove( 0, 0, W );
		  break;
      }
   }

fim2:

   return ( J );
}


