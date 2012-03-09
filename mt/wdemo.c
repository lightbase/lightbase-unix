
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  <window.h>
#include  <mensagem.h>

#include  "menu.h"
#include  "entrada.h"
#include  "tabela.h"
#include  <ethead.h>


#ifdef    MSDOS

#include  <dos.h>
#include  <alloc.h>

/***
extern unsigned _ovrbuffer = 0x1800;
***/
extern unsigned _stklen    = 7000;

#endif

#ifdef     MSDOS

SS getint( SC * );
VOID Wparams( VOID );
VOID cmd_menu( VOID );
VOID global_par( VOID );

#else

SS getint( );
VOID Wparams( );
VOID cmd_menu( );
VOID global_par( );

#endif

static SS factor;             /* the delay factor ( first parameter ) */
struct Window *wp, *wp_aux;

SS  verbo( str, D )

SC     *str;
DB *D;
{
   if ( strcmp( str, "verbo" ) == 0 )
   {
      *D = 5;
      return ( 0 );
   }

   if ( strcmp( str, "desconto" ) == 0 )
   {
      *D = 10;
      return ( 0 );
   }

   if ( strcmp( str, "tudo_@#,\"[]" ) == 0 )
   {
      *D = 100;
      return ( 0 );
   }

   *D = -1;
   return ( -1 );
}

SI  as_str ( a, b )

SC  **a, **b;
{
   return ( strcmp( *a, *b ) );
}


SI  des_str ( a, b )

SC   **a, **b;
{
   return ( strcmp( *a, *b ) * -1 );
}


SI  ascendente ( a, b )

SC  *a, *b;
{
   if ( *a == *b )   return ( 0 );

   else              return ( ( *a > *b ) ? 1 : -1 );
}


SI  descendente ( a, b )

SC   *a, *b;
{
   if ( *a == *b )    return ( 0 );

   else               return ( ( *a < *b ) ? 1 : -1 );
}


SS teste(s)
Selecao *s;
{
   SS       i;
   Selecao   m;
   static SC     *op2[] =  {
                        "Primeiro",   "1",  "Mensagem Opcao 1",
                        "Segundo",    "1",  "Mensagem Opcao 2",
                        "Terceiro",   "2",  "Mensagem Opcao 3",
                        " quarto",    "1",  "Mensagem Opcao 4",
                        "qUinto",     "1",  "Mensagem Opcao 5",
                        "Sexto",      "1",  "Mensagem Opcao 6",
                        "sEtimo",     "1",  "Mensagem Opcao 7",
                        "oitavo",     "1",  "Mensagem Opcao 8",
                        " n",         "1",  "Mensagem Opcao 9",
                        " D",         "1",  "Mensagem Opcao 10",
                        "BONZE",      "0",  "Mensagem Opcao 11",
                        "CDOZE",      "1",  "Mensagem Opcao 12",
                        "ZTREZETRES", "1",  "Mensagem Opcao 13",
                        NULL,
                     };

   if ( s->escolhido == 0 ) {  return ( -1 );  }

   def_menu( &m, 20, 10, 40, 5, 3, MS_CAMPO, MS_BARRA, 2, MS_JANELA, MS_DESTAQUE, &op2,
             "Menu Chamado pelo M1", "Rodape");
   m.escolhido = s->escolhido;
   i = menu( &m );
   return ( i );
}

US  n_global = 0;

main( argc,argv )
SC **argv;
{
  SS     a, L, H, l, h, x, y;
  SS     qtest();

#ifndef  MSDOS
   setvbuf( stdout, malloc( 1024 ), _IOFBF, 1024 );
#endif

   env_term = "LBTERM";
   env_dir  = "LBDIR";

factor = 5;
if( argc == 2 )
      { factor = 0;
        sscanf( argv[1],"%d",&factor );
      }
  if( win_init( "strings", 0 )) exit( 1 );
  setquit( qtest );

  win_bell( BEL_INVALID );

  w_kbd( 'T', 'e', 's', 't', 'e', 0 );

#ifdef   MSDOS
  fwprintf( curr_win, "ovrbuffer = %X -> %ld Bytes\n", _ovrbuffer, (SL) (_ovrbuffer) * 16 );
  fwprintf( curr_win, "stklen = %d\n", _stklen );
#endif

  while( 1 )
        {
          a = fwqgc( curr_win );

          if( a < 128 && a >= ' ' && a != '\\' )
                {
                  fwpc( a,curr_win );
                  if ( a >= '0' && a <= '9' )  n_global = a - '0';
                  continue;
                }
          if( a == '\b' || a == '\n' || a == 7 )
                { fwpc( a,curr_win );
                  continue;
                }
          switch( a )
                {
                  case K_BTAB:
                             fwprintf( curr_win, "Back Tab\n" );
                             continue;
                  case K_F2:
                             fwprintf( curr_win, "K_F2\n" );
                             continue;
                  case K_F4:
                             fwprintf( curr_win, "K_F4\n" );
                             continue;
                  case K_F5:
                             fwprintf( curr_win, "K_F5\n" );
                             continue;
                  case K_F6:
                             fwprintf( curr_win, "K_F6\n" );
                             continue;
                  case K_F7:
                             fwprintf( curr_win, "K_F7\n" );
                             continue;
                  case K_F8:
                             fwprintf( curr_win, "K_F8\n" );
                             continue;
                  case K_F9:
                             fwprintf( curr_win, "K_F9\n" );
                             continue;
                  case K_F0:
                             fwprintf( curr_win, "K_F10\n" );
                             continue;
                  case K_WORDLEFT:
                             fwprintf( curr_win, "Word Left\n" );
                             continue;
                  case K_WORDRIGHT:
                             fwprintf( curr_win, "Word Right\n" );
                             continue;
                  case K_CPGDN:
                             fwprintf( curr_win, "Ctrl PgDn\n" );
                             continue;
                  case K_CPGUP:
                             fwprintf( curr_win, "Ctrl PgUp\n" );
                             continue;

                  case K_UP: if( curr_win->w_cy )
                                fwat( curr_win->w_cx, (curr_win->w_cy - 1 ), curr_win );
                             continue;
                  case K_DN: if( curr_win->w_cy < curr_win->w_height - 1 )
                                fwat( curr_win->w_cx,( curr_win->w_cy + 1 ), curr_win );
                             continue;
                  case K_RT: if( curr_win->w_cx < curr_win->w_len - 1 )
                                fwat( ( curr_win->w_cx + 1 ),curr_win->w_cy, curr_win  );
                             continue;
                  case K_LFT: if( curr_win->w_cx )
                                fwat( ( curr_win->w_cx - 1 ),curr_win->w_cy, curr_win  );
                              continue;
                }
          if( a != '\\' )
          {
             fwpc( a,curr_win );
             continue;
          }

          switch( fwqgc( curr_win ))
                {
case 'a' :
{
   SS I;
   SC dest[ 100 ];
   static SC *teste[] = {
      "Jairo",
      "   Jairo",
      "Jairo Fonseca da Silva",
      "Jairo    Fonseca    da    Silva",
      "  Jairo   Fonseca da   Silva",
      "Jairo    ",
      "   Jairo      ",
      "Jairo Fonseca da Silva       ",
      "Jairo    Fonseca    da    Silva      ",
      "  Jairo   Fonseca da   Silva           ",
   NULL
   };

   for ( I = 0;   teste[ I ] != NULL;   I++ )
   {
      fwprintf( curr_win, "\n%s", just_line( dest, teste[ I ], 60 ) );
      fwrefresh( curr_win );
   }
}

   continue;

case 'A' :
{
#ifdef    MSDOS
   fwprintf( curr_win, "\nOverlay = %d\n", _ovrbuffer );
   fwrefresh( curr_win );
#endif
   debug( "%%S1-----1 <%s> -> %%S2-----2 <%s>\n%%S3-----3 <%s> -> %%S4-----4 <%s>\n%%S5-----5 <%s> -> %%D6-----6 <%d>",
"Teste de String -> 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 - Abcdefghijklmnopqrstuvwxyz - 1234567890",
"Teste de String -> 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 - Abcdefghijklmnopqrstuvwxyz - 1234567890",
          "Teste de String - Abcdefghijklmnopqrstuvwxyz - 1234567890", "Teste de String - Abcdefghijklmnopqrstuvwxyz - 1234567890",
          "Teste de String - Abcdefghijklmnopqrstuvwxyz - 1234567890", 12345 );
/***
   debug( "kjhkjhkjh23456789%s123456789 12345",
"Teste de String -> 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 - Abcdefghijklmnopqrstuvwxyz - 1234567890",
"Teste");
***/

   win_ajuda = "a_demo";
}

continue;

                  case 'q': /* quit */
                  case 'Q': win_exit( 0 );
                            exit( 0 );
                  case 'G': /* graphic character */
                            x = getint( "graphic" );
                            fwpc( x, curr_win );
                            continue;
                  case 'g': /* quick goto */
                            x = getint( "x" );
                            y = getint( "y" );
                            fwat( x,y,curr_win );
                            continue;
case 'i': /* teste de mens_erro, insercol */
{
   WIN  *JI;

   JI = abre_insercol( "Insercol Muito Grande Mesmo" );
   fwqgc( curr_win );

   insercol( JI, "Pequeno" );
   fwqgc( curr_win );

   fwprintf( curr_win, "\nAgora com Valores" );  fwrefresh( curr_win );
   insercol( JI, "Col = %d  Lin = %d", 123, 456 );
   fwqgc( curr_win );

   fwprintf( curr_win, "\nAgora com Atributos em Col e em %%d de Lin" );  fwrefresh( curr_win );
   insercol( JI, "%4ACol%1A = %d  Lin = %4A%d%1A" );
   fwqgc( curr_win );

   fecha_insercol( JI );

   JI = abre_mensagem( "1234 = %d - Teste Abre_Mensagem = %s", 1234, "Teste Abre_Mensagem" );
   fwqgc( curr_win );
   fecha_mensagem( JI );

   mens_erro( "", "1234 = %d - %4ATeste%1A Mens_Erro = %s", 1234, "Teste Mens_Erro" );
}

continue;
                  case 'r': /* refresh */
                            win_ajuda = "";
                            fwrefresh( curr_win );
                            continue;
                  case 'R': /* global refresh */
                            win_refresh();
                            continue;
case 'M' :
   hm_set( curr_win, size_x - 10, 0, 1, "%2.2d:%2.2d:%2.2d" );
   continue;

                  case 'm': /* make window */
                            L = getint( "L" );
                            H = getint( "H" );
                            l = getint( "l" );
                            h = getint( "h" );
                            x = getint( "x" );
                            y = getint( "y" );
                            curr_win = win_make( L,H,l,h,x,y );
                            continue;
                  case 'f': /* make frame */
                            L = getint( "L" );  /* Linha  Inicial */
                            H = getint( "H" );  /* Coluna Inicial */
                            l = getint( "l" );  /* Linha  Final   */
                            h = getint( "h" );  /* Coluna Final   */
                            x = getint( "x" );  /* Tipo 1 Simples 2 Duplas */
                            fwframe( L, H, l, h, x, curr_win );
                            continue;
                  case 'k': /* kill window */
                            x = getint( "window" );
                            if( x < 0 || x >= win_sp ) continue;
                            fwkill( win_stak[ x ] );
                            continue;
                  case 'p': Wparams();
                            continue;
                  case 'P': global_par();
                            continue;
case 'N':
fwprintf( curr_win, "123456789 123456789 123456789 " );
fwrefresh( curr_win );
   continue;
                  case 'n': /* make a new window current */
                            x = getint( "win_num" );
                            if( x < 0 ) continue;
                            if( x >= win_sp ) continue;
                            curr_win = win_stak[ x ];
                            continue;
                  case 'B': /* ring the bell */
                            fwputs( ( UC * ) "\nBEL_GEN", curr_win );
                            fwrefresh( curr_win );
                            win_bell( BEL_GEN );
                            fwqgc( curr_win );

                            fwputs( ( UC * ) "\nBEL_VALID", curr_win );
                            fwrefresh( curr_win );
                            win_bell( BEL_VALID );
                            fwqgc( curr_win );

                            fwputs( ( UC * ) "\nBEL_INVALID", curr_win );
                            fwrefresh( curr_win );
                            win_bell( BEL_INVALID );
                            fwqgc( curr_win );

                            fwputs( ( UC * ) "\nBEL_LONGJOB", curr_win );
                            fwrefresh( curr_win );
                            win_bell( BEL_LONGJOB );
                            fwqgc( curr_win );

                            fwputs( ( UC * ) "\nBEL_ATTN", curr_win );
                            fwrefresh( curr_win );
                            win_bell( BEL_ATTN );
                            fwqgc( curr_win );

                            fwputs( ( UC * ) "\nBEL_EMERGENCY", curr_win );
                            fwrefresh( curr_win );
                            win_bell( BEL_EMERGENCY );

                            continue;

                  case 'c': cmd_menu();
                            continue;

case 'C':
{
   static SC   str[ 200 ] = "";
   SC          S[ 50 ];
   SS          I;
   DB      D;
   d_entry     e;
   WIN        *JM;

   JM = abre_mensagem( "Calc_avalia" );

   while( 1 )
   {
      d_entrada( &e, curr_win, str, 200,
      "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                                 NULL, 0, curr_win->w_cy, E_ALFA );
      I = entrada( &e, 0 );

      if ( I == K_ESC )    break;

      I = calc_avalia( str, &D, JM, verbo, '?' );

      if ( I < 0 )   sprintf( S, "Erro = %d", I );
      else           sprintf( S, "%-18.13g", D );

      fwprintf( curr_win, "\n<%s>\n", S );
      fwrefresh( curr_win );
   }

   fecha_mensagem( JM );
}

continue;

                  case 'S' :
{
   SS     I;
   static SC     *array[ ] =  {

         "Primeiro",
         "Segundo",
         "Terceiro",
         "Quarto",
         "QuInto",
         "Sexto",
         "Setimo",
         "Oitavo",
         "Nono",
         "Decimo",
   };

   for ( I = 0;   I < 10;   I++ )   fwprintf( curr_win, "\n%s", array[ I ] );

   fwqgc( curr_win );

   fwprintf( curr_win, "\n" );

   qsort( ( SC * ) &array, 10, sizeof( SC * ), as_str );

   for ( I = 0;   I < 10;   I++ )   fwprintf( curr_win, "\n%s", array[ I ] );

   fwqgc( curr_win );

   fwprintf( curr_win, "\n" );

   qsort( ( SC * ) &array, 10, sizeof( SC * ), des_str );

   for ( I = 0;   I < 10;   I++ )   fwprintf( curr_win, "\n%s", array[ I ] );
}

continue;

                  case 's' :
{
   static  SC
      matriz[ ] = "ABCDEFGHIJKLMMNOPQRSTUVXYWZNOPQRSTUVXYWZABCDEFGHIJKL@";

   fwprintf( curr_win, "\n\nOriginal   : %s", matriz );

   /*** Cuidado com o \0, porisso do - 2 = para deixar o @ de fora ***/
   qsort( matriz, sizeof( matriz ) - 2, sizeof( SC ), ascendente );

   fwprintf( curr_win, "\nAscendente : %s", matriz );

   /*** Cuidado com o \0, porisso do - 2 = para deixar o @ de fora ***/
   qsort( matriz, sizeof( matriz ) - 2, sizeof( SC ), descendente );

   fwprintf( curr_win, "\nDescendente: %s", matriz );

   fwrefresh( curr_win );

}

continue;

case 'd' :
   decida( "Titulo", "Abort@Retry@Ignore", "decida", 2, 30, "linha 1\nlinha %d", 2 );
   continue;

                  case 'T':
{
   SS        i;
   Tab       t;
   WIN       *fun;

   static SC     *op1[ 10 ] =  {
         "Primeiro   ",
         "sEgundo    ",
         "Terceiro   ",
   };

   def_tabela( &t, 20, 5, 15, 8, 3, 10, "xxxxxxxxxx",
              MP_CAMPO, MP_BARRA, 1, MP_JANELA, MP_DESTAQUE, &op1,
              " TABELA ", "CTRL-[ANY]" );

   t.enter = n_global;
   t.edita = ALTERA | INSERE | DELETA;

   fun = abre_funcoes( "Teste de Tabela - FUNCOES" );

   i = tabela( &t );

   fecha_funcoes( fun );

   fwprintf( curr_win, "\nEscolha I = %d\n", i + 1 );
   fwrefresh( curr_win );
}
                  break;

                  case 'J':
{
   SS        i;
   Selecao   m;
   static SC     *op1[] =  {
         "Primeiro",  "1",   "Mensagem Opcao 1",
         "Segundo",   "1",   "Mensagem Opcao 2",
         "Terceiro",  "1",   "Mensagem Opcao 3",
         " q ",       "1",   "Mensagem Opcao 4",
         "qUinto",    "1",   "Mensagem Opcao 5",
         "Sexto",     "0",   "Mensagem Opcao 6",
         "sEtimo",    "1",   "Mensagem Opcao 7",
         " oitavo",   "1",   "Mensagem Opcao 8",
         "Nono",      "1",   "Mensagem Opcao 9",
         "DEZ",       "1",   "Mensagem Opcao 10",
         NULL
   };
   static SC     *op2[] =  {
         "Primeiro",  "1",   "Mensagem Opcao 1",
         "Segundo",   "1",   "Mensagem Opcao 2",
         "Terceiro",  "0",   "Mensagem Opcao 3",
         "quarto",    "1",   "Mensagem Opcao 4",
         "qUinto",    "1",   "Mensagem Opcao 5",
         "Sexto",     "1",   "Mensagem Opcao 6",
         "sEtimo",    "1",   "Mensagem Opcao 7",
         "oitavo",    "1",   "Mensagem Opcao 8",
         " N",        "1",   "Mensagem Opcao 9",
         "DEZ",       "1",   "Mensagem Opcao 10",
         "BONZE",     "1",   "Mensagem Opcao 11",
         "CDOZE",     "1",   "Mensagem Opcao 12",
         NULL
   };
   static SC     *op3[] =  {
         "Primeiro",  "1",   "Mensagem Opcao 1",
         "Segundo",   "1",   "Mensagem Opcao 2",
         "Terceiro",  "0",   "Mensagem Opcao 3",
         "quarto",    "1",   "Mensagem Opcao 4",
         "qUinto",    "2",   "Mensagem Opcao 5",
         "Sexto",     "1",   "Mensagem Opcao 6",
         "sEtimo",    "1",   "Mensagem Opcao 7",
         "oitavo",    "1",   "Mensagem Opcao 8",
         " n",        "1",   "Mensagem Opcao 9",
         " D",        "1",   "Mensagem Opcao 10",
         "BONZE",     "2",   "Mensagem Opcao 11",
         "CDOZE",     "1",   "Mensagem Opcao 12",
         NULL
   };

   def_menu( &m, 30, 3, 10, 5, 3, MP_CAMPO, MP_BARRA, 1, MP_JANELA, MP_DESTAQUE, &op1,
            "MENU P", "1 Sai" );
   m.executa = teste;
   i = menu( &m ) + 1;

   def_menu( &m, 0, 0, 80, 1, 15, MP_CAMPO, MP_BARRA, 1, MP_JANELA, MP_DESTAQUE, &op2,
            "Titulo de Cima", "EscEsc, Enter, Setas, PgUp/Dn");
   i = menu( &m ) + 1;

   def_menu( &m, 20, 10, 11, 5, 1, MP_CAMPO, MP_BARRA, 2, MP_JANELA, MP_DESTAQUE, &op3,
            "Titulo", "Rodape");
   i = menu( &m ) + 1;

   i = i;
}

                           continue;

                  case 'E':
{
   Selecao   e;
   static SC     *op1[] =  {
         "Primeiro",
         "Segundo",
         "Terceiro",
         " q ",
         "qUinto",
         "Sexto",
         "sEtimo",
         " oitavo",
         "Nono",
         "DEZ",
         NULL
   };

   def_escolha( &e, 30, 3, 10, 5, 3, MP_CAMPO, MP_BARRA, 1,
                MP_JANELA, MP_DESTAQUE, &op1, "Escolha", "Esc Sai" );

   menu( &e );
}

                           continue;

case 'j':

   fwprintf( curr_win, "\nsize_y = %d --- size_x = %d", size_y, size_x );
   fwprintf( curr_win, "\n%4AIntenso%3APiscante%2AReverso%1ANormal." );
   fwprintf( curr_win, "\n%a%s%a%s%a%s%a%s.",
                              ATR_INTENSO, "Intenso",
                              ATR_BLINK,   "Piscante",
                              ATR_REVERSO, "Reverso",
                              ATR_NORMAL,  "Normal" );
   fwqgc( curr_win );

   for ( a = 0;  a < 35;  a++ ) {

      fwprintf( curr_win, "\n%d> ABCDEFGHIJKLMNOPQRSTUVWXYZ 1234567890 Fim.", a );
   }

   fwrefresh( curr_win );

   continue;

case 'y':
{
   d_entry     e;
   SC          str[ 300 ];

   strcpy( str, "Teste sem Mensagem" );
   d_entrada( &e, curr_win, str, 50,
                              "Aaaaaaaaaaaaaaaaaaaaaa",
                              NULL, 10, 9, E_VALOR | E_CONFIRMA );
   entrada( &e, 0 );
   fwat( 0, 0, curr_win  );
   fwprintf( curr_win, "\n<%s>%-30.30s", str, "" );
   fwrefresh( curr_win );

   strcpy( str, "12,34" );
   d_entrada( &e, curr_win, str, 50,
                              "999.999.999.999,99",
                              "Entre com Salario", 10, 11, E_VALOR | E_CONFIRMA );
   entrada( &e, 0 );
   fwat( 0, 2, curr_win  );
   fwprintf( curr_win, "\n<%s>%-30.30s", str, "" );
   fwrefresh( curr_win );

   strcpy( str, "" );
   d_entrada( &e, curr_win, str, 50,
                              "999.999.999.999,9999",
                              "Entre com DESPESAS", 10, 12, E_VALOR | E_CONFIRMA );
   entrada( &e, 0 );
   fwat( 0, 3, curr_win  );
   fwprintf( curr_win, "\n<%s>%-30.30s", str, "" );
   fwrefresh( curr_win );

   strcpy( str, "12,34" );
   d_entrada( &e, curr_win, str, 10,
                              "999.999.999.999,9999",
                              "Entre com DESPESAS", 10, 13, E_VALOR | E_CONFIRMA );
   entrada( &e, 0 );
   fwat( 0, 4, curr_win  );
   fwprintf( curr_win, "\n<%s>%-30.30s", str, "" );
   fwrefresh( curr_win );
}

            continue;

case 'z':
{
   static SC   tit_rod[ 200 ];
   SC          str[ 200 ];
   d_entry     e;

   strcpy( str, "casado" );
   d_entrada( &e, curr_win, str, 50,
                              "aaaaaaaaaa",
                              "Use BARRA", 10, 8, E_LISTA );
   e.lista = "Solteiro@Casado@Viuvo@ouTros";
   entrada( &e, 0 );
   fwat( 0, 0, curr_win  );
   fwprintf( curr_win, "\n<%s>%-30.30s", str, "" );
   fwrefresh( curr_win );

   strcpy( str, "Teste sem Mensagem ... " );
   d_entrada( &e, curr_win, str, 20,
                              "Xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                              NULL, 10, 9, E_ALFA );
   entrada( &e, 0 );
   strcpy( tit_rod, str );
   curr_win->title  = tit_rod;
   curr_win->bottom = tit_rod;
   fwat( 0, 1, curr_win  );
   fwprintf( curr_win, "\n<%s>%-30.30s", str, "" );
   fwrefresh( curr_win );

   strcpy( str, "" );
   d_entrada( &e, curr_win, str, 50,
                              "****************",
                              "Entre com Dados", 10, 10, E_ALFA | E_CONFIRMA );
   entrada( &e, 0 );
   fwat( 0, 2, curr_win  );
   fwprintf( curr_win, "\n<%s>%-30.30s", str, "" );
   fwrefresh( curr_win );

   strcpy( str, "(061) 274 47 56" );
   d_entrada( &e, curr_win, str, 50,
                              "(999) 999.99.99 (AAAA)",
                              "Entre com Telefone", 10, 14, E_ALFA | E_CONFIRMA );
   entrada( &e, 0 );
   fwat( 0, 3, curr_win  );
   fwprintf( curr_win, "\n<%s>%-30.30s", str, "" );
   fwrefresh( curr_win );
}

continue;

case 'Y':

{
   WIN    *je;
   char   *nomearq = "edtmp";

#ifdef    MSDOS
   fwprintf( curr_win, "\nCoreLeft ANTES  = %l", farcoreleft( ) );
#endif

   je = win_make( 50, 10, 50, 10, size_x, 2 );

   if ( je == NULL )
   {
      fwprintf ( curr_win, "\nNao consigo abrir JANELA\n" );
      continue;
   }

   edit_arquivo( nomearq, je, 49, "Mensagem default de DEMO", 0 );

   fwkill( je );

#ifdef    MSDOS
   fwprintf( curr_win, "\nCoreLeft DEPOIS = %l", farcoreleft( ) );
#endif
}

continue;

case 'Z':

{
   WIN    *je;
   ARG    *args;
   char   *nomearq = "edteste";

#ifdef    MSDOS
   fwprintf( curr_win, "\nCoreLeft ANTES  = %l", farcoreleft( ) );
#endif

   w_refresh = 1;

   je = win_make( 50, 10, 50, 10, size_x, 2 );

   if ( je == NULL )
   {
      fwprintf ( curr_win, "\nNao consigo abrir JANELA\n" );
      continue;
   }

   if ( (args = load_buffer( 0, ET_FILE, nomearq, 0L )) != NULL )
   {
      fw_title( je, nomearq, je->t_atr );
      args->def_mens = "Mensagem default de DEMO";
      edit_buffer( args, ET_MENU, je, NULL, 0 );
      free_buffer( args );
   }

   fwkill( je );

#ifdef    MSDOS
   fwprintf( curr_win, "\nCoreLeft DEPOIS = %l", farcoreleft( ) );
#endif
}

continue;
      }
   }
}

do_sleep()
{ SS a,b;
  a = getint( "sleepfor");
  for( ;a;a--) for( b = 10000 * factor;b;b-- );
  return;
}

l_sleep( x )
SS x;
{ SS a,b;

  for( a = x;a;a-- ) for( b = 10000 * factor;b;b-- );
  return;
}

SS qtest()  /* THIS ROUTINE RUNS WHEN "ABORT" IS CALLED */
{
   W_curmove( 0, 0 );   prclear( );    doflush( );

   printf("\r\nSciencia Informatica  \r\n\n");
   printf("\'Demonstracao\' -> Janelas, Menus, Tabelas, Entrada, Edicao.\r\n" );

   W_curmove( 0, 8 );  doflush( );

   return 1;
}

#define do( a,b ) sprintf( buf,"%-38.38s  %s\n",a,b ); fwputs( buf,curr_win )

VOID global_par()                    /* adjust global parameters */
{
  switch( fwqgc( curr_win ))
        { case 'g': get_flag = getint( "GET FLAG" );
                    return;
        }
  return;
}

VOID Wparams()                    /* adjust window parameters ( for curr_win ) */
{
  switch( fwqgc( curr_win ))
        { case 'a': curr_win->w_atrib = getint( "atrib" );
                    return;
          case 'd': curr_win->w_defatr = getint( "defatr" );
                    return;
          case 'o': curr_win->w_optatr = getint( "optatr" );
                    return;
          case 'c': curr_win->w_curstyle = getint( "curstyle" );
                    return;
          case 'f': curr_win->w_flags = getint( "flags" );
                    return;
          case 'b': curr_win->w_boxatr = getint( "Box atribute" );
                    return;
          case 't': curr_win->t_atr = getint( "Title atribute" );
                    return;
          default:  return;
        }
}

SC  buf[180];

VOID cmd_menu()                      /* test each of the subcommands */
{ SS a,b;

  switch( fwqgc( curr_win ))
        { case 'r': fwrefresh( curr_win );
                    return;
          case 'c': fwcls( curr_win );
                    return;
          case 'l': fwcll( curr_win );
                    return;
          case 'e': fwcle( curr_win );
                    return;
          case 'm': a = getint( "x - rel" );
                    b = getint( "y - rel" );
                    fwamove( a,b,curr_win );
                    return;
          case 'M': a = getint( "x - term" );
                    b = getint( "y - term" );
                    fwmove( a,b,curr_win );
                    return;
          case 's': a = getint( "leng" );
                    b = getint( "height" );
                    fwsize( a,b,curr_win );
                    return;
          case 'g': /* getchar - echoing */
                    fprintf( stderr,"GOT %d\n", fwgc( curr_win ));
                    return;
          case 'u': /* unget SC */
                    a = getint( "unget" );
                    w_ungc( a );
                    return;
          case 'h': /* keyboard check */
                    fprintf( stderr,"HIT RETURNED %d\n",w_kbhit(curr_win));
                    return;
          case 't': /* move curr_win to top */
                    fwtop( curr_win );
                    return;
          case 'T': /* to bottom */
                    fwbot( curr_win ); return;
          case 'B': fwbox( curr_win );
                    return;
          case 'b': fwunbox( curr_win );
                    return;
          default:  return;
        }
}

SS  getint( s )
SC *s;
{
  SS a,c;
  a = 0;
#ifndef MSDOS
  fprintf( stderr,"\nGET %s\n",s );
#endif
  s += 1;
  while( 1 )
        { c = fwqgc( curr_win );
          if( c == '\n' ) return a;
          if( c == '\b' ) a /= 10;
          if( c < '0' || c > '9' ) continue;
          a *= 10;
          a += c - '0';
 }
}

#ifdef    XTOOLS

carga_comxt( ) {  return;  }

#endif

