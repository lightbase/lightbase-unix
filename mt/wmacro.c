
#ifdef NAODEF

VOID analisa_demo( ) - chamada em fwnqgc() para executar uma funcao macro
SC   *mr_fgets( )    - Fgets() para leitura de macro
SS   mr_read( )      - Read() para leitura de macro
VOID mw_obs( )       - Poe observacoes ou mensagens nas macros de macro
SC   *mw_puts( )     - Puts() para gravacao de macro
SS   mw_putc( )      - Putc() para leitura de macro
SS   mr_fim( )       - Finaliza leitura/execucao de macro
SS   mr_inic( )      - Inicia   leitura/execucao de macro
SS   mw_fim( )       - Finaliza gravacao de macro
SS   mw_inic( )      - Inicia   gravacao de macro

#endif

#include <stdio.h>
#include <string.h>
#include <window.h>
#include "mensagem.h"
#include PROTO
#ifdef   MSDOS
#include <dos.h>
#endif

#define   CONST_DELAY   20
#define   SPEED_MAX     10
#define   MAX_SLEEP     60
#define   MIN_SLEEP     3
#define   MAX_READ      300
#define   MAX_CMD       30

static UC delay_macro = SPEED_MAX; /* delay na leitura do arquivo de macro */

VOID  analisa_demo( W1 )

WIN   *W1;
{
   SS   I, J;
   SI   X, Y, AX, AY;
   SC   str[ MAX_READ + 1 ];
   SC   cmd[ MAX_CMD  + 1 ];
   SC   *P;
   SC   *C1;
   SC   *C2;
   SC   *C3;
   SC   *C4;
   SC   *C5;
   SC   *C6;
   WIN  *W2;

   /*** Este up_str() esta' aqui apenas por garantia, pois ...***/
   /*** ... quem trata esta disponibilidade de strings sao ... ***/
   /*** ... as funcoes de m[rw]inic() e m[rw]fim(). ***/
   up_str( STR_MACRO );

   C1 = loc_str( STR_MACRO, SLEEP );
   C2 = loc_str( STR_MACRO, MESSAGE );
   C3 = loc_str( STR_MACRO, WINDOW );
   C4 = loc_str( STR_MACRO, CLOSE );
   C5 = loc_str( STR_MACRO, SPEED );
   C6 = loc_str( STR_MACRO, COMMENT );

   fwrefresh( W1 );
   fwsetcursor( W1 );

   sscanf( mr_fgets( str ), "%20s", cmd );

   I = 0;

   if      ( strncmp( cmd, C1, strlen(C1) ) == 0 )  I = 1;
   else if ( strncmp( cmd, C2, strlen(C2) ) == 0 )  I = 2;
   else if ( strncmp( cmd, C3, strlen(C3) ) == 0 )  I = 3;
   else if ( strncmp( cmd, C5, strlen(C5) ) == 0 )  I = 5;
   else if ( strncmp( cmd, C6, strlen(C6) ) == 0 )  I = 6;

   switch( I )
   {
   default  :
               break;

   case   1 :  /*** Bell, Tempo em Segundos ***/
               win_bell( BEL_GEN );
               cmd[ 0 ] = '\0';  X = 0;
               sscanf( str, "%20s %d", cmd, &X );
               sleep( COTA( MIN_SLEEP, X, MAX_SLEEP ) );
               break;

   case   2 :  /*** Bell, Mensagem e Tempo ***/
               win_bell( BEL_GEN );
               cmd[ 0 ] = '\0';  X = 0;
               sscanf( str, "%20s %d", cmd, &X );
               for( P = str; *P != '<' && *P != '\0';  P++ ) /* Nada */;
               if ( *P == '<' )  P++;
               for( J = 0; *P != '>' && *P != '\0'; J++, P++ ) str[ J ] = *P;
               str[ J ] = '\0';
               W2 = abre_mensagem( str );
               if ( W2 == NULL )   break;
               fwsetcursor( W2 );
               sleep( COTA( MIN_SLEEP, X, MAX_SLEEP ) );
               fecha_mensagem( W2 );
               break;

   case   3 :  /*** Bell, Janela e Tempo ***/
               win_bell( BEL_GEN );
               cmd[ 0 ] = '\0';  X = 0;  Y = 0;  AX = 0;  AX = 0;
               sscanf( str, "%20s %d %d %d %d", cmd, &X, &Y, &AX, &AY );
               W2 = win_make( AX, AY, AX, AY, X, Y );
               if ( W2 == NULL )   break;
               fwbox( W2 );
               for ( J = 0;   J < size_y  &&  rd_macro != NULL;  J++ )
               {
                  cmd[ 0 ] = '\0';  X = 0;
                  sscanf( mr_fgets( str ) + 1, "%20s %d", cmd, &X );
                  if ( strncmp( cmd, C4, strlen(C4)) == 0 )  break;
                  if ( J > 0 )   fwprintf( W2, "\n" );
                  fwprintf( W2, str );
                  fwrefresh( W2 );
               }
               fwsetcursor( W2 );
               sleep( COTA( MIN_SLEEP, X, MAX_SLEEP ) );
               fwkill( W2 );
               break;

   case   5 :  /*** Speed ***/
               cmd[ 0 ] = '\0';  X = 0;
               sscanf( str, "%20s %d", cmd, &X );
               delay_macro = MIN( X, SPEED_MAX );
               break;

   case   6 :  /*** Comment ***/
               break;
   }
}


SC  *mr_fgets( S )

SC  S[ ];
{
   UC  C;
   SS  I;

   for ( I = 0; I < MAX_READ && mr_read(&C, NAO_FILTRA) == 1; I++ )
   {
      if ( C == UG_LF )      break;
      if ( ! e_print( C ) )  continue;

      S[ I ] = C;
   }

   S[ I ] = 0;

   return ( S );
}


SS   mr_read( PC, filtra )

UC   *PC;
SS   filtra;
{
   SI   C;

#ifdef   MSDOS
   delay( (SPEED_MAX - delay_macro) * CONST_DELAY );
#endif

   if ( rd_macro == NULL )    return( 0 );

   /*** Nao aceitamos caracter ZERO na MACRO, observe et_grava_macro() ***/
   while ((C = fgetc( rd_macro )) == 0 )
      ;

   if ( C == EOF ) {  mr_fim( );  return( 0 );  }

   else  *PC = C;

   /*** Se estivermos filtrando e acontecer de ser UG_LF ... ***/
   /*** ... precisamos saber qual o proximo se for COMANDO ... ***/
   /*** ... ignoramos o UG_LF se nao, aproveitamos o UG_LF ... ***/
   /*** ... e damos w_ungc() no caracter seguinte que tem tem ... ***/
   /*** ... prioridade sobre a leitura de arquivo... ***/
   if ( filtra == FILTRA_NL  &&  C == UG_LF )
   {
      /*** Nao aceitamos ZERO na MACRO, observe et_grava_macro() ***/
      while ((C = fgetc( rd_macro )) == 0 )
         ;

      if ( C == EOF )            return( 1 );   /*** Aproveite o UG_LF ***/
      else if ( C == COMANDO )   *PC = C;
      else                       w_ungc( C );
   }

   return( 1 );
}


VOID mw_obs( opcao, str )

SS   opcao;
SC   *str;
{
   SC  buf[ MAX_READ + 1 ];

   switch ( opcao )
   {
   default :      break;

   case M_MENS :
                  sprintf( buf, "\n%c %30s %100s\n", COMANDO, MESSAGE, str );
                  mw_puts( str );
                  break;

   case M_COMENT :
                  sprintf( buf, "\n%c %30s %100s\n", COMANDO, COMMENT, str );
                  mw_puts( str );
                  break;
   }
}


SC   *mw_puts( str )

SC   *str;
{
   SC   *P;

   for ( P = str;   *P != '\0'  &&  mw_putc( *P ) != -1;   P++ )
      ;

   return ( str );
}


SS   mw_putc( C )

UC   C;
{
   if ( wr_macro != NULL  &&  fputc( C, wr_macro ) == EOF )
   {
      mw_fim( );
      return ( (SS) -1 );
   }

   return ( (SS) C );
}


SS   mr_fim( )
{
   /* Nao leia mais de rd_macro e sim do teclado - STDIN */
   if ( rd_macro != NULL )
   {
      fclose( rd_macro );
      rd_macro = NULL;
      down_str( STR_MACRO );
      return ( 1 );
   }

   return ( 0 );
}


SS mr_inic( arq )

SC  *arq;
{
   delay_macro = SPEED_MAX;  /*** Velocidade Maxima ***/

   if ( rd_macro != NULL )   mr_fim( );

   /*** Em modo TEXTO, para facilitar edicao de COMANDOS de MACRO ***/
   if ((rd_macro = fopen( arq, MODO_RO )) == NULL )   return( 0 );

   up_str( STR_MACRO );
   return( 1 );
}


SS  mw_fim( )
{
   /* Nao grave mais a macro */
   if ( wr_macro != NULL )
   {
      fclose( wr_macro );
      wr_macro = NULL;
      down_str( STR_MACRO );
      return ( 1 );
   }

   return ( 0 );
}

SS mw_inic( arq, apend )

SC   *arq;
SS   apend;
{
   SC   *modo;

   /*** Em modo TEXTO, para facilitar edicao de COMANDOS de MACRO ***/
   modo = ( apend ) ? MODO_AO : MODO_WO;

   if ( wr_macro != NULL )  mw_fim( );

   if ((wr_macro = fopen( arq, modo )) == NULL )   return( 0 );

   up_str( STR_MACRO );
   return( 1 );
}


SS  interrompe_macro( )
/*** Return( 0 ) -> Continua   execucao da macro ***/
/*** Return( 1 ) -> Interrompe execucao da macro ***/
{
   UC    A;
   WIN   *JM = NULL;

   if ( wq_kbhit( JM ) )
   {
      while ( wq_kbhit( JM ) )   w_rgc( JM );
      JM = abre_mensagem( loc_str( STR_MACRO, M_INTER ) );
      A = w_rgc( JM );
      fecha_mensagem( JM );
      if ( A == 27 )    /*** ASCII ESC - Nao usar K_ESC ***/
      {
         mr_fim( );
         return ( 1 );
      }
   }

   return ( 0 );
}



