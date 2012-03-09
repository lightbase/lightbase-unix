
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <time.h>

#include  <window.h>
#include  <entrada.h>
#include  <mensagem.h>

#include  <ethead.h>

SS edita_arq( win )

WIN   *win;
{
   SS       C, doc;
   WIN      *JE;
   WIN      *JF;
   SC       arquivo[ 61 ];
   SC       margem[ 10 ];
   SC       *ajuda;

   win = win;

   arquivo[ 0 ] = margem[ 0 ] = '\0';

   C = pede_cadeia( loc_str( STR_UTIL, UT_51 ), arquivo, 60,
                    loc_str( STR_UTIL, UT_52 ), 'x', NULL );

   if ( C != K_ENTER )  return( 0 );

   pede_cadeia( loc_str( STR_UTIL, UT_53 ), margem, 3,
                loc_str( STR_UTIL, UT_54 ), '9', NULL );

   doc = atoi( margem );

   JE = win_make( size_x / 2, size_y /2, size_x / 2, size_y / 2, 1, 2 );

   if ( JE == NULL )   return( 0 );

   fwbot( JE );

   ajuda     = win_ajuda;
   win_ajuda = "utilitar";

   JF = abre_funcoes( funcao( "2 3 5 E" ) );

   edit_arquivo( arquivo, JE, doc, loc_str( STR_UTIL, UT_55 ), 1 );

   fecha_funcoes(  JF );
   fwkill( JE );
   win_ajuda = ajuda;
   return ( 1 );
}


SS  calcula( win )

WIN  *win;
{
   win = win;
   /*** calculadora( ) ja seta WIN_AJUDA ***/
   calculadora( );
   return ( 1 );
}

#define   TAM_MACRO  40

SS   macro_crit_arq( E )

d_entry   *E;
{
   SS      I;
   FILE   *F;

   if ( E->tecla != K_ENTER  ||  E->str[ 0 ] == '\0' )  return( 0 );

   if ( (F = fopen( E->str, MODO_RO )) != NULL )
   {
      fclose( F );
      I = men_conf( loc_str( STR_UTIL, UT_56 ),
                    loc_str( STR_UTIL, UT_57 ), "existe_a", NULL );
      if ( I )   return ( -1 );
   }

   return ( 0 );
}


SS  grava_macro( win )

WIN   *win;
{
   SS       C;
   SC       *ajuda;
   SC       arq_macro[ TAM_MACRO + 1 ];
   SL       T;

   win = win;

   /*** Se esta' executando uma macro e este comando for o ultimo ... ***/
   /*** ... cancele a operacao, e' residuo de quando encerrou de gravar ***/
   if ( rd_macro != NULL )
   {
      T = ftell( rd_macro );
      fseek( rd_macro, 0L, 2 );
      if ( T == ftell( rd_macro ) ) return( 1 );
      fseek( rd_macro, T, 0 );
   }

   if ( mw_fim( ) )   return( 1 );

   ajuda     = win_ajuda;
   win_ajuda = "gr_macro";
   arq_macro[ 0 ] = '\0';

   C = pede_cadeia( loc_str( STR_UTIL, UT_58 ), arq_macro, TAM_MACRO,
                    loc_str( STR_UTIL, UT_59 ), 'x',
                    macro_crit_arq );

   win_ajuda = ajuda;

   if ( C != K_ENTER  || arq_macro[ 0 ] == '\0' )  return( 0 );

   mw_inic( arq_macro, 0 );

   return ( 1 );
}


SS ex_macro( win )

WIN   *win;
{
   SS       C;
   SC       *ajuda;
   SC       arq_macro[ TAM_MACRO + 1 ];

   win = win;

   mr_fim( );

   ajuda     = win_ajuda;
   win_ajuda = "ex_macro";
   arq_macro[ 0 ] = '\0';

   C = pede_cadeia( loc_str( STR_UTIL, UT_60 ), arq_macro, TAM_MACRO,
                    loc_str( STR_UTIL, UT_61 ), 'x', NULL );

   win_ajuda = ajuda;

   if ( C != K_ENTER  || arq_macro[ 0 ] == '\0' )  return( 0 );

   mr_inic( arq_macro );

   return ( 1 );
}


SC    *nome_dia[ ] =
      {
         str_vazia,  /*   "Domingo",   */
         str_vazia,  /*   "Segunda",   */
         str_vazia,  /*   "Terca",     */
         str_vazia,  /*   "Quarta",    */
         str_vazia,  /*   "Quinta",    */
         str_vazia,  /*   "Sexta",     */
         str_vazia,  /*   "Sabado",    */
      };

SC    *nome_mes[ ] =
      {
         str_vazia,  /*   "",          */
         str_vazia,  /*   "Janeiro",   */
         str_vazia,  /*   "Fevereiro", */
         str_vazia,  /*   "Marco",     */
         str_vazia,  /*   "Abril",     */
         str_vazia,  /*   "Maio",      */
         str_vazia,  /*   "Junho",     */
         str_vazia,  /*   "Julho",     */
         str_vazia,  /*   "Agosto",    */
         str_vazia,  /*   "Setembro",  */
         str_vazia,  /*   "Outubro",   */
         str_vazia,  /*   "Novembro",  */
         str_vazia,  /*   "Dezembro",  */
      };


SS calendario( win )

WIN *win;
{
   SC     rodape[ 80 ];
   SS     I, col, lin, ret = 1;
   US     ano, mes, dia;
   WIN    *JC, *JM;
   time_t secs_now;
   struct tm *tm_now;
#ifdef  MEDIT
   SS   jlin = 1;
#else
   SS   jlin = 2;
#endif
   SC       *ajuda;

   ajuda     = win_ajuda;
   win_ajuda = "calendario";

   win = win;

   JM = abre_mensagem( loc_str( STR_UTIL, UT_62 ) );

   JC = win_make( 27, 9, 27, 9, 0, jlin );

   if ( JC == NULL )  {  ret = 0;  goto sai;  }

   JC->title = loc_str( STR_UTIL, UT_63 );

   for ( I = 64, dia = 0;   dia < 7;   I++, dia++ )
   {
      /* De 0 a 6 */
      nome_dia[ dia ] = loc_str( STR_UTIL, I );
   }

   for ( I = 71, mes = 1;   mes <= 12;   I++, mes++ )
   {
      /* De 1 a 12 */
      nome_mes[ mes ] = loc_str( STR_UTIL, I );
   }

   fwprintf( JC, "\n%3.3s %3.3s %3.3s %3.3s %3.3s %3.3s %3.3s",
             nome_dia[0], nome_dia[1], nome_dia[2],
             nome_dia[3], nome_dia[4], nome_dia[5], nome_dia[6] );

   time( &secs_now );
   tm_now = localtime( &secs_now );
   ano = tm_now->tm_year + 1900;
   mes = tm_now->tm_mon + 1; /*** de 1 a 12 ***/

   while ( 1 )
   {
      for( dia = 1, I = dayofwk( ano, mes, dia );   dia <= 31;   dia++, I++ )
      {
         sprintf( rodape, " %s %d ", nome_mes[ mes ], ano );
         fw_bottom( JC, rodape );

         if ( I > 27  &&  valdate( ano, mes, dia ) )   break;

         col = (I % 7) * 4;
         lin = (I / 7) + 3;
         fwat( col, lin, JC );

         fwprintf( JC, "%3.3d", dia );
      }

      switch( fwqgc( JM ) )
      {
      case K_ENTER : ret = 0;
      case K_ESC   : goto fim;

      case K_UP :
                     ano++;
                     break;
      case K_DN :
                     ano--;
                     break;
      case K_RT :
                     mes++;
                     break;
      case K_LFT :
                     mes--;
                     break;
      }

      if ( mes > 12 )  {  mes = 1;   ano++;  }
      if ( mes < 1  )  {  mes = 12;  ano--;  }

      fwat( 0, 3, JC );
      fwcle( JC );
   }

fim:
   fwkill( JC );
sai:
   fecha_mensagem( JM );
   win_ajuda = ajuda;

   return( ret );
}

VOID setconfig( fcn )  /*** Funcao que sera chamada para CONFIGURACAO ***/

SS (*fcn)();
{
  fun_config = fcn;
}



