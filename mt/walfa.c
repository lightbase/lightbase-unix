#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  <window.h>
#include  <mensagem.h>

#include  "menu.h"
#include  "entrada.h"
#include  "tabela.h"
#include  <ethead.h>

main( )
{
	SS     a, L, H, l, h, x, y;
	SS     qtest();

	WIN *jan;

   setvbuf( stdout, malloc( 1024 ), _IOFBF, 1024 );

   env_term = "MDTERM";
   env_dir  = "MDDIR";

	if( win_init( "strings", 0 )) exit( 1 );

	setquit( qtest );

	win_bell( BEL_INVALID );

   jan = win_make( ( SS ) ( size_x + 150 ), ( SS ) ( size_y - 6 ),
                          ( SS ) ( size_x -  30 ), ( SS ) ( size_y - 6 ),
                          ( SS ) ( 1 )           , ( SS ) ( 2 ) );

   fwbox ( jan );
   curr_win = jan;

   w_kbd( 'T', 'e', 's', 't', 'e', 0 );

   fwrefresh( curr_win );
/*****
   {
   int i= 1;
   unsigned int ui=2;
   short s=3;
   unsigned short us=4;
   char c=5;
   unsigned char uc=6;
   void xixi_funcao( int, unsigned int, short, unsigned short, char, unsigned char );

   xixi_funcao( i, ui, s, us, c, uc );

   }
*******/
	while( 1 )
	{
		a = fwqgc( curr_win );

   	if( a < 128 && a >= ' ' && a != '\\' )
		{
			fwpc( a,curr_win );

			continue;
		}

		if( a == '\b' || a == '\n' || a == 7 )
		{
			fwpc( a,curr_win );
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

         case K_F3:
         	fwprintf( curr_win, "K_F3\n");
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
         	fwprintf (curr_win, "Palavra a esquerda \n");
            continue;

         case K_WORDRIGHT:
            fwprintf( curr_win, "Word Right\n" );
         	fwprintf (curr_win, "Palavra a direita \n");
            continue;

         case K_CPGDN:
            fwprintf( curr_win, "Ctrl PgDn\n" );
         	fwprintf (curr_win, "Control PgDn \n");
            continue;

         case K_CPGUP:
            fwprintf( curr_win, "Ctrl PgUp\n" );
         	fwprintf (curr_win, "Control PgUp \n");
            continue;

         case K_UP: if( curr_win->w_cy )
            fwat( curr_win->w_cx, (curr_win->w_cy - 1 ), curr_win );
         	fwprintf (curr_win, "Cima \n");
            continue;

         case K_DN: if( curr_win->w_cy < curr_win->w_height - 1 )
            fwat( curr_win->w_cx,( curr_win->w_cy + 1 ), curr_win );
         	fwprintf (curr_win, "Baixo \n");
            continue;

         case K_RT: if( curr_win->w_cx < curr_win->w_len - 1 )
            fwat( ( curr_win->w_cx + 1 ),curr_win->w_cy, curr_win  );
         	fwprintf (curr_win, "Direita \n");
            continue;

         case K_LFT: if( curr_win->w_cx )
            fwat( ( curr_win->w_cx - 1 ),curr_win->w_cy, curr_win  );
         	fwprintf (curr_win, "Esquerda \n");
            continue;

      }
   }
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

#ifdef    XTOOLS

carga_comxt( ) {  return;  }

#endif

void xixi_funcao( int i, unsigned int ui, short s, unsigned short us, char c, unsigned char uc )
{
	char *ptc;
	int indice;

	debug( "%d %d %d %d %d %d", i, ui, s, us, c, uc );

	ptc = (char *)&i;

	for( indice = 0; indice < 48; indice+=8 )
		debug( "%d %d %d %d %d %d %d %d", ptc[ indice ], ptc[ indice+1 ],
		      ptc[ indice+2 ], ptc[ indice+3 ], ptc[ indice+4 ],
		      ptc[ indice+5 ], ptc[ indice+6 ], ptc[ indice+7 ] );
	return;
}

