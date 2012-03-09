
#ifdef NAODEF

fwprintf( window, fmt )

debug( fmt )              Abre uma Janela imprime FORMATO e espera uma Tecla

WIN *abre_mensagem( str ) Abre janela de mensagem e escreve str

mens_1( win, str )
mens_2( win, str )   Soa BEL_INVALID
mens_3( win, str )   Tamanho 47 caracteres
                     Soa BEL_GEN
                     Escreve Tecle ESC
                     Espera pela tecla ESC

fecha_mensagem(win)  Fecha Janela

WIN *abre_funcoes( str ) Similar a ABRE_MENSAGEM

funcoes( win, str )                Similar a MENS_1(  );

fecha_funcoes( win )               Similar a FECHA_MENSAGEM(  );

SC   *funcao( teclas )             Auxilio a funcoes
                                   Coloca tecla em negrito
                                   Descreve funcao da telca
#endif

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  "window.h"
#include  "mensagem.h"
#include  "menu.h"

#ifdef    MT_VARARGS
#include  <varargs.h>
#endif

#include  PROTO

#ifdef     PROTO_ANSI

ST( VOID ) uprintf( SC **tofmt, VOID (*fcn)( SC * ) );
ST( VOID ) make_f( DB, SC *, SS );
ST( VOID ) make_e( DB, SC *, SS );
ST( VOID ) loadint( SC *, long, SS, SC * );

#else

ST( VOID ) uprintf( );
ST( VOID ) make_f( );
ST( VOID ) make_e( );
ST( VOID ) loadint( );

#endif

#ifdef    XTOOLS

#define   TAM_FUN  80
#define   TAM_MEM  79
#define   TAM_INS  20

#define   COL_FUN  TAM_FUN
#define   COL_MEN  TAM_MEM
#define   LIN_MEN  1
#define   X_MEN    0
#define   Y_MEN    size_y - 3

#define   COL_INS  TAM_INS
#define   LIN_INS  1
#define   X_INS    (size_x - TAM_INS)
#define   Y_INS    size_y - 2

#else

#define   TAM_FUN  80
#define   TAM_MEM  60
#define   TAM_INS  20

#define   COL_FUN  TAM_FUN
#define   COL_MEN  TAM_MEM
#define   LIN_MEN  1
#define   X_MEN    0
#define   Y_MEN    size_y - 2

#define   COL_INS  TAM_INS
#define   LIN_INS  1
#define   X_INS    (size_x - TAM_INS)
#define   Y_INS    size_y - 2

#endif

static VOID towndo( );

static SS   con_inser;
static UC   str_inser[ TAM_INS + 5 ];
static WIN  *w;
UC   fl_mens_on = 1;
UC   fl_funcao_on = 1;
UC   fl_erro_on = 1;

static   WIN  *M_WIN = NULL;
struct   m_stack {
   US       buffer[ COL_MEN + 1 ];
   struct   m_stack *ant;
};
typedef  struct  m_stack  M_STACK;
static   M_STACK *M_FINAL = NULL;

static   WIN  *F_WIN = NULL;
struct   f_stack {
   US       buffer[ COL_FUN + 1 ];
   struct   f_stack *ant;
};
typedef   struct  f_stack  F_STACK;
static    F_STACK *F_FINAL = NULL;

#ifdef    PROTO_ANSI
SS decida( SC *tit, SC *itens, SC *ajuda, SS lin, SS col, SC *formato, ... )
#else
#ifdef    MT_VARARGS
SS    decida( tit, itens, ajuda, lin, col, va_alist )
SC   *tit;
SC   *itens;
SC   *ajuda;
SS    lin;
SS    col;
va_dcl
#else
SS    decida( tit, itens, ajuda, lin, col, formato )
SC   *tit;
SC   *itens;
SC   *ajuda;
SS    lin;
SS    col;
SC   *formato;
#endif
#endif
{
   SS       I, J, alt, lar, X, Y, ret;
   SC       *lista, **opcoes, q_lista, *ajuda_tmp;
   WIN      *J1, *J2;
   WIN      *JM, *JF;
   Selecao  S;
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   ajuda_tmp = win_ajuda;
   win_ajuda = ajuda;

   if ( itens == NULL  ||  *itens == '\0' )
   {
      /*** ATENCAO - NAO PODE ABRIR ESCOLHE() NEM DAR FREE ***/
      lista  = NULL;
      opcoes = NULL;
   }

   else
   {
      lista = malloc( strlen( itens ) + 1 );

      if ( lista == NULL )  {  ret = -2;  goto  fim1;  }

      for ( q_lista = 1, I = 0;   itens[ I ] != '\0';   I++ )
      {
         if ( itens[ I ] == '@' )   q_lista++;
      }

      opcoes = (SC **) malloc( (q_lista + 1 ) * sizeof( SC * ));

      if ( opcoes == NULL ) {  ret = -2;  goto  fim2;  }

      strcpy( lista, itens );

      for ( opcoes[ 0 ] = lista, I = 0, J = 1;   lista[ I ] != '\0';   I++ )
      {
         if ( lista[ I ] == '@' )
         {
            lista[ I ] = '\0';
            if ( J < q_lista )   opcoes[ J++ ] = &lista[ I + 1 ];
         }
      }

      opcoes[ J ] = NULL;
   }

   lar = col + 4;
   alt = lin + 2;
   X   = (size_x / 2) - (lar / 2);
   Y   = (size_y / 2) - (alt / 2);

   J1 = win_make( lar, alt, lar, alt, X, Y );

   if ( J1 == NULL )  {  ret = -2;  goto  fim3;  }

   set_cores( J_ERRO, J1 );

   fwcls( J1 );

   J2 = win_make( col, lin, col, lin, X + 2, Y + 1 );

   if ( J2 == NULL )  {  ret = -2;  goto fim4;  }

   set_cores( J_ERRO, J2 );

   fwcls( J2 );

   fw_title( J1, tit, J1->w_optatr );

   w = J2;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &formato, towndo );
#endif

   JM = abre_mensagem( "" );
   JF = abre_funcoes( "" );

   funcoes( JF, funcao( "1 3" ));

   while( w_kbhit( JM ) )  w_rgc( JM ); /* Limpe teclado ! - evita type_ahead */

   if ( lista == NULL )
   {
      mens_2( JM, loc_str( STR_OBRIG, OBRIG_3 ) );
      while ( (ret = fwqgc( JM )) != K_ENTER && ret != K_ESC ) /* NADA */;
   }

   else
   {
      mens_2( JM, loc_str( STR_OBRIG, OBRIG_4 ) );

      def_escolha( &S, J1->w_x, J1->w_y + J1->w_aheight, size_x, 1, 100,
                  JE_CAMPO, JE_BARRA, 1, JE_JANELA, JE_DESTAQUE,
                  ( SC * (* )[] ) opcoes, "", "" );

      S.col = J1->w_x + ((J1->w_len / 2) - (S.a_col / 2));

      ret = escolha( &S );
   }

   fecha_funcoes( JF );
   fecha_mensagem( JM );

   fwkill( J2 );

fim4:
   fwkill( J1 );

fim3:
   if ( opcoes != NULL )   free( (SC *) opcoes );

fim2:
   if ( lista  != NULL )   free( (SC *) lista );

fim1:
   win_ajuda = ajuda_tmp;
   win_refresh( );
   return ( ret );
}

/*----------------------------------------------------funcao--------------*/
SC   *funcao( teclas )
SC   *teclas;
{
   static SC linha[150];
   SC        *p, *t, aux[40];

   p = teclas;

   linha[0] = '\0';

   while ( *p != '\0' ) {

      switch (*p) {
         case ' ' :                                /*   BRANCO    */
            strcat(linha," ");
				break;
         case '1' :                                /*   F1    */
            strcat( linha, loc_str( STR_OBRIG, OBRIG_5 ) );
				break;
         case '2' :                                 /*   F2    */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_6 ));
            p++;
            t = aux;
            while ( *p != ' ' && p != '\0' ) {
              *t++ = *p++;
            }
            *t = '\0';
            if ( strlen(aux) == 0 ) strcat(linha, loc_str( STR_OBRIG, OBRIG_7 ));
            else {
               strcat(linha, aux);
               strcat(linha, " ");
            }
				break;
         case 'W' :                                 /*   F2 em exclusao */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_8 ));
				break;
         case '3' :                                 /*   F3    */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_9 ));
				break;
         case '4' :                                 /*   F4    */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_10 ));
				break;
         case '5' :                                 /*   F5    */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_11 ));
				break;
         case '6' :                                 /*   F6    */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_12 ));
				break;
         case '7' :                                 /*   F7    */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_13 ));
				break;
         case '8' :                                 /*   F8    */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_14 ));
				break;
         case '9' :                                 /*   F9    */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_15 ));
				break;
			case '0' :                                 /*   F10   */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_16 ));
            p++;
            t = aux;
            while ( *p != ' ' && *p != '\0' ) {
              *t++ = *p++;
            }
            *t = '\0';
            if ( strlen(aux) == 0 ) strcat(linha, loc_str( STR_OBRIG, OBRIG_17 ));
            else {
               strcat(linha, aux);
               strcat(linha, " ");
            }
				break;
         case 'E' :                                 /*   ESC   */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_18 ));
				break;
         case 'N':                                 /*   ENTER */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_19 ));
				break;
         case 'I':                                 /*   Ins  */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_20 ));
				break;
         case 'D':                                 /*   Del  */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_21 ));
				break;
#ifdef   Tirar_isto_aqui
         case 'U':                                 /*   PgUp  */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_22 ));
				break;
         case 'P':                                 /*   PgDn  */
            strcat(linha,loc_str( STR_OBRIG, OBRIG_23 ));
				break;
#endif
      }


      p++;



      if (strlen(linha) > 150 ) {
         debug( loc_str( STR_DISCO, DISCO_4 ), strlen( linha ) );
         return( "" );
      }
	}

	return(linha);
}


static    VOID     imp_inser( S )

UC  *S;
{
   SS   I;

   I = strlen( ( SC * ) S );

   if ( I  >  TAM_INS - con_inser )
   {
      I = TAM_INS - con_inser;

      S[ I ] = '\0';
   }

   strcat( ( SC * ) str_inser, ( SC * ) S );

   con_inser += I;
}


#ifdef    PROTO_ANSI
VOID insercol( WIN *win, SC *str, ... )
#else
#ifdef    MT_VARARGS
VOID insercol( win, va_alist )
WIN  *win;
va_dcl
#else
VOID insercol( win, str )
WIN  *win;
SC             *str;
#endif
#endif
{
#ifdef    MT_VARARGS
   va_list  ap;
   va_start( ap );
#endif

   if ( win == NULL )  return;

   fwat( 0, 0, win );

   con_inser = 0;
   str_inser[ 0 ] = '\0';
#ifdef    MT_VARARGS
   uprintf( ap, imp_inser );
   va_end(  ap );
#else
   uprintf( &str, imp_inser );
#endif

   /***  Atencao  "%20.20s" e' o tamanho de insercol TAM_INS ***/
   fwprintf( win, "%20.20s", str_inser );

   fwrefresh( win );
}


VOID fecha_insercol( win )

WIN *win;
{
   if ( win != NULL )   fwkill( win );
}

#ifdef    PROTO_ANSI
WIN *abre_insercol( SC * str, ... )
#else
#ifdef    MT_VARARGS
WIN *abre_insercol( str )
SC  *str;
#else
WIN *abre_insercol( str )
SC  *str;
#endif
#endif
{
   WIN *win;

   win = win_make( COL_INS, LIN_INS, COL_INS, LIN_INS, X_INS, Y_INS );

   if ( win == NULL )  return( NULL );

   win->w_intr   = 0;
   win->w_atrib  = LM_CAMPO;
   win->campoatr = LM_CAMPO;
   win->w_optatr = LM_BARRA;
   win->w_defatr = LM_DESTAQUE;

   if ( win->w_flags & W_BOXED )  fwunbox( win );

   win->w_flags  = W_NOSCROL + W_VISIB;

   insercol( win, str );

   return( win );
}

VOID set_mensagem( flag )

SS   flag;
{
   fl_mens_on = flag;
}

VOID set_funcao( flag )

SS   flag;
{
   fl_funcao_on = flag;
}

VOID set_erro( flag )

SS   flag;
{
   fl_erro_on = flag;
}


#ifdef    PROTO_ANSI
WIN *abre_mensagem( SC *str, ... )
#else
#ifdef    MT_VARARGS
WIN *abre_mensagem( va_alist )
va_dcl
#else
WIN *abre_mensagem( str )
SC  *str;
#endif
#endif
{
   WIN *win;
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   if ( ! fl_mens_on )    return( NULL );

   if ( M_WIN == NULL )
   {
      win = win_make( COL_MEN, LIN_MEN, COL_MEN, LIN_MEN, X_MEN, Y_MEN );
      if ( win == NULL )  return( NULL );
      win->w_intr   = 0;
      win->w_atrib  = LM_CAMPO;
      win->campoatr = LM_CAMPO;
      win->w_optatr = LM_BARRA;
      win->w_defatr = LM_DESTAQUE;
      if ( win->w_flags & W_BOXED )  fwunbox( win );
      win->w_flags  = W_NOSCROL + W_VISIB;
      M_WIN = win;
   }

   else
   {
      M_STACK  *ptr;
      ptr = (M_STACK *) malloc( sizeof( M_STACK ) );
      if ( ptr == NULL )  return( NULL );
      w_movint( M_WIN->w_ary, ptr->buffer, COL_MEN );
      ptr->ant = M_FINAL;
      M_FINAL = ptr;
   }

   fwat( 0, 0, M_WIN );

   w = M_WIN;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &str, towndo );
#endif

   if ( M_WIN->w_cx  <  TAM_MEM - 1 )  fwcll( M_WIN );
   fwrefresh( M_WIN );
   return( M_WIN );
}


VOID fecha_mensagem( win )

WIN *win;
{
   if ( win == NULL )   return;

   if ( M_FINAL == NULL )
   {
      if ( M_WIN != NULL )   fwkill( M_WIN );
      M_WIN = NULL;
   }

   else
   {
      M_STACK  *ptr;
      if ( M_WIN == NULL )   return;
      w_movint( M_FINAL->buffer, M_WIN->w_ary, COL_MEN );
      ptr = M_FINAL->ant;
      free( (SC *) M_FINAL );
      M_FINAL = ptr;
      M_WIN->w_anymod = 2;
      fwrefresh( M_WIN );
   }
}

#ifdef    PROTO_ANSI
VOID mens_1( WIN *win, SC *str, ... )
#else
#ifdef     MT_VARARGS
VOID mens_1( win, va_alist )
WIN  *win;
va_dcl
#else
VOID mens_1( win, str )
WIN  *win;
SC             *str;
#endif
#endif
{
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   if ( win != M_WIN )    return;
   if ( ! fl_mens_on )       return;
   if ( win == NULL )     return;

   fwat( 0, 0, win );

   w = win;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &str, towndo );
#endif

   if ( win->w_cx  <  TAM_MEM - 1 )  fwcll( win );
   fwrefresh( win );
}


#ifdef    PROTO_ANSI
VOID mens_2( WIN *win, SC *str, ... )
#else
#ifdef    MT_VARARGS
VOID mens_2( win, va_alist )
WIN  *win;
va_dcl
#else
VOID mens_2( win, str )
WIN  *win;
SC             *str;
#endif
#endif
{
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   win_bell( BEL_INVALID );

   if ( win != M_WIN )    return;
   if ( ! fl_mens_on )       return;
   if ( win == NULL )     return;

   fwat( 0, 0, win );

   w = win;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &str, towndo );
#endif

   if ( win->w_cx  <  TAM_MEM - 1 )  fwcll( win );
   fwrefresh( win );
}


#ifdef    PROTO_ANSI
VOID mens_3( WIN *win, SC *str, ... )
#else
#ifdef    MT_VARARGS
VOID mens_3( win, va_alist )
WIN  *win;
va_dcl
#else
VOID mens_3( win, str )
WIN  *win;
SC             *str;
#endif
#endif
{
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   win_bell( BEL_GEN );

   if ( win != M_WIN )    return;
   if ( ! fl_mens_on )       return;
   if ( win == NULL )     return;

   fwat( 0, 0, win );

   w = win;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &str, towndo );
#endif

   fwprintf( win, loc_str( STR_OBRIG, OBRIG_24 ) );

   if ( win->w_cx  <  TAM_MEM - 1 )  fwcll( win );

   while ( fwqgc( win )  !=  K_ESC )
      ;
}


#ifdef    PROTO_ANSI
WIN *abre_funcoes( SC *str, ... )
#else
#ifdef    MT_VARARGS
WIN *abre_funcoes( va_alist )
va_dcl
#else
WIN *abre_funcoes( str )
SC  *str;
#endif
#endif
{
   WIN  *win;
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   if ( ! fl_funcao_on )  return( NULL );

   if ( F_WIN == NULL )
   {
      win = win_make( COL_FUN, 1, COL_FUN, 1, 0, size_y - 1 );
      if ( win == NULL )  return( NULL );
      win->w_intr   = 0;
      win->w_atrib  = LM_CAMPO;
      win->campoatr = LM_CAMPO;
      win->w_optatr = LM_BARRA;
      win->w_defatr = LM_DESTAQUE;
      if ( win->w_flags & W_BOXED )  fwunbox( win );
      win->w_flags  = W_NOSCROL + W_VISIB;
      F_WIN = win;
   }

   else
   {
      F_STACK  *ptr;
      ptr = (F_STACK *) malloc( sizeof( F_STACK ) );
      if ( ptr == NULL )  return( NULL );
      w_movint( F_WIN->w_ary, ptr->buffer, COL_FUN );
      ptr->ant = F_FINAL;
      F_FINAL = ptr;
   }

   fwat( 0, 0, F_WIN );

   w = F_WIN;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &str, towndo );
#endif

   if ( F_WIN->w_cx  <  COL_FUN - 1 )   fwcll( F_WIN );
   fwrefresh( F_WIN );
   return( F_WIN );
}


VOID fecha_funcoes( win )
WIN *win;
{
   if ( win == NULL )   return;

   if ( F_FINAL == NULL )
   {
      if ( F_WIN != NULL )   fwkill( F_WIN );
      F_WIN = NULL;
   }

   else
   {
      F_STACK  *ptr;
      if ( F_WIN == NULL )   return;
      w_movint( F_FINAL->buffer, F_WIN->w_ary, COL_FUN );
      ptr = F_FINAL->ant;
      free( (SC *) F_FINAL );
      F_FINAL = ptr;
      F_WIN->w_anymod = 2;
      fwrefresh( F_WIN );
   }
}


#ifdef    PROTO_ANSI
VOID funcoes( WIN *win, SC *str, ... )
#else
#ifdef    MT_VARARGS
VOID funcoes( win, va_alist )
WIN  *win;
va_dcl
#else
VOID funcoes( win, str )
WIN  *win;
SC   *str;
#endif
#endif
{
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   if ( win != F_WIN )    return;
   if ( ! fl_funcao_on )     return;
   if ( win == NULL )     return;

   fwat( 0, 0, win );

   w = win;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &str, towndo );
#endif

   if ( win->w_cx  <  COL_FUN - 1 )  fwcll( win );
   fwrefresh( win );
}


#ifdef    PROTO_ANSI
SS   mens_erro( SC *ajuda, SC *fmt, ... )
#else
#ifdef    MT_VARARGS
SS   mens_erro( ajuda, va_alist )
SC   *ajuda;
va_dcl
#else
SS   mens_erro( ajuda, fmt )
SC   *ajuda, *fmt;
#endif
#endif
{
   SS     ret;
   SC    *ajuda_ant;
   WIN   *win;
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   if ( ! fl_erro_on )  return( -1 );

#ifndef    MT_VARARGS
   if ( fmt == NULL  ||  *fmt == '\0' )  return( -1 );
#endif

   ajuda_ant = win_ajuda;

   if ( ajuda != NULL )   win_ajuda = ajuda;

   win = win_make( COL_MEN, LIN_MEN, COL_MEN, LIN_MEN, X_MEN, Y_MEN );

   if ( win == NULL ) {  ret = -1;   goto fim;  }

/***
   win->w_intr   = 0;
   win->w_atrib  = JE_CAMPO;
   win->campoatr = JE_CAMPO;
   win->w_optatr = JE_BARRA;
   win->w_defatr = JE_DESTAQUE;
***/

   if ( win->w_flags & W_BOXED )  fwunbox( win );

   win->w_flags  = W_NOSCROL + W_VISIB;

   win_bell( BEL_GEN );

   w = win;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &fmt, towndo );
#endif

   ret = fwqgc( win );

   fwkill( win );

fim:
   win_ajuda = ajuda_ant;

   win_refresh( );
   return ( ret );
}

#ifdef    PROTO_ANSI
SS debug( SC *fmt, ... )
#else
#ifdef    MT_VARARGS
SS debug( va_alist )
va_dcl
#else
SS debug( fmt )
SC    *fmt;
#endif
#endif
{
   SS ret;
   WIN *win;
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   win = win_make( 78, 3, 78, 3, 0, 20 );
   if ( win == NULL )  return( -1 );

   fw_title( win, loc_str( STR_OBRIG, M_ATENTION ), ATR_REVERSO );
   fw_bottom( win, loc_str( STR_OBRIG, B_PRESS ) );

   w = win;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &fmt, towndo );
#endif

   ret = fwqgc( win );

   if ( ret != K_ESC )
   {
      while ( fwqgc( win )  !=  K_ESC )   win_bell( BEL_INVALID );
   }

   fwkill( win );
   win_refresh( );
   return ( ret );
}

#ifdef   PROTO_ANSI
VOID fwprintf( WIN *win, SC *fmt, ... )
#else
#ifdef   MT_VARARGS
VOID fwprintf( win, va_alist )
WIN *win;
va_dcl
#else
VOID fwprintf( win, fmt )
WIN *win;
SC            *fmt;
#endif
#endif
{
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   w = win;
#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &fmt, towndo );
#endif
}


#ifdef   PROTO_ANSI
VOID mvwprintw( WIN *win, SS Y, SS X, SC *fmt, ... )
#else
#ifdef   MT_VARARGS
VOID mvwprintw( win, Y, X, va_alist )
WIN  *win;
SS   Y;
SS   X;
va_dcl
#else
VOID mvwprintw( win, Y, X, fmt )
WIN  *win;
SS   Y;
SS   X;
SC   *fmt;
#endif
#endif
{
#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#endif

   w = win;
   fwat( X, Y, w );

#ifdef    MT_VARARGS
   uprintf( ap, towndo );
   va_end( ap );
#else
   uprintf( &fmt, towndo );
#endif
}


/*********************** NOTE *************************
OTHER FUNCTIONS SUCH AS printf, fprintf, & sprintf
CAN BE REWRITTEN TO WORK THROUGH uprintf VERY EASILY!
******************************************************/

#define   TMP_BUF     160
#define   PRINT_BUF    80

/***
Format controls as per printf ( K&R )
   plus: %x.x@ positions cursor,
         %a  turns atribute next (int) param on,
         %xA turns atribute x on,
             x == 1  w->w_atrib   NORMAL / NOMES
             x == 2  w->w_optatr  REVERSO
             x == 3  ATR_BLINK    PISCANTE
             x == 4  w->w_defatr  INTENSO
             x == 5  w->campoatr  NORMAL / CAMPOS
***/

#ifdef     PROTO_ANSI
ST( VOID ) uprintf( SC **tofmt, VOID (*fcn)( SC * ) )
#else
#ifdef     MT_VARARGS
ST( VOID ) uprintf( ap, fcn )
va_list   ap;
VOID  (*fcn)( );       /* function which processes a string */
#else
ST( VOID ) uprintf( tofmt, fcn )
SC    **tofmt;
VOID  (*fcn)( );       /* function which processes a string */
#endif
#endif
{
#ifndef    MT_VARARGS
   SC  *param;
   SI  *ip;
   DB  *dp;
   SL  *lp;
#endif
   SC  fbuf[ TMP_BUF ];               /* tmp buffer to format into */
   SC  *r, *rr;
   SC  *list;
   SS  i, base, field, precision, bias;
   US  atr;
   DB  d;
   SL  l;

   static   SC  buf[ PRINT_BUF ];
   register SC  *p,*q;

db2( "uprintf" );

   fbuf[ TMP_BUF - 1 ] = 0;
   if ( w != NULL ) atr = w->w_boxatr;

#ifdef    MT_VARARGS
   if ((p = va_arg( ap, SC * )) == NULL )   return;
#else
   param  = (SC *) tofmt;
   param += inc_ptr;
   if ((p = *tofmt) == NULL )   return;
#endif

   for( q = buf;   *p;   p++ )
   {
      if ( *p != '%' )
      {
print_char:   /* process a simple character */

         *q = *p;  q++;

         if ( q > buf + (PRINT_BUF - 2) )
         {
            *q = 0;   (*fcn)(buf);  q = buf;
         }
         continue;
      }

      /**** process a % code *****/
      list = "0123456789abcdef";
      p++;

      /* 1: get bias */
      switch( *p )
      {
      case '-':   bias = 1;   /* right justify */
                  p++;
                  break;
      /* OTHERS MAY WANT TO BE ADDED */
      default:    bias = 0;   /* left justify */
                  break;
      }

      if ( *p == '0' ) {  bias += 8;  p++;  }     /* zero pad */

      /* 2: get field width */
      if ( *p >= '0'  &&  *p <= '9' )
      {
         field = atoi( p );
         for (   ;   *p <= '9' && *p >= '0';   p++ )
            ;
      }

      else  field = -1;

      /* 3: get precision */
      if ( *p == '.' )
      {
         p++;
         precision = atoi( p );
         for(   ;   *p <= '9' && *p >= '0';   p++ )
            ;
      }

      else   precision = -1;

      /* do all the sweet conversions */
      switch( *p )
      {
         /* FORMATS MAY WANT TO BE ADDED HERE */
      case 'c':
#ifdef    MT_VARARGS
                  *q     = va_arg( ap, SC );
#else
                  ip     = (SI *) param;
                  param += inc_char;
                  *q     = *ip;
#endif
                  q++;
                  break;

      case 's':
#ifdef    MT_VARARGS
                  rr     = va_arg( ap, SC * );
#else
                  rr     = *(SC **) param;
                  param += inc_ptr;
#endif
                  if ( precision == -1 )
                  {
                     strncpy( fbuf, rr, (TMP_BUF - 1) );
                     goto loadbuf;
                  }

                  for ( r = fbuf;
                        *rr && precision && r < fbuf + (TMP_BUF - 2);
                        precision--, r++, rr++ )
                  {
                     *r = *rr;
                  }

                  *r = 0;
                  goto loadbuf;

      case 'd':
#ifdef    MT_VARARGS
                  l      = va_arg( ap, SI );
#else
                  ip     = (SI *) param;
                  param += inc_int;
                  l      = *ip;
#endif
                  base   = 10;
                  goto     fixlong;

      case 'o':
#ifdef    MT_VARARGS
                  l      = va_arg( ap, SI );
#else
                  ip     = (SI *) param;
                  param += inc_int;
                  l      = *ip;
#endif
                  base   = 8;
                  goto     fixlong;

      case 'X':   list   = "0123456789ABCDEF";
      case 'x':
#ifdef    MT_VARARGS
                  l      = va_arg( ap, SI );
#else
                  ip     = (SI *) param;
                  param += inc_int;
                  l      = *ip;
#endif
                  base   = 16;
                  goto     fixlong;

      case 'l':
                  p++;
#ifdef    MT_VARARGS
                  l      = va_arg( ap, SL );
#else
                  lp     = (SL *) param;
                  param += inc_long;
                  l     = *lp;
#endif
                  switch( *p )
                  {
                  case 'd':   base = 10;   goto fixlong;
                  case 'o':   base = 8;    goto fixlong;
                  case 'X':   list = "0123456789ABCDEF";
                  case 'x':   base = 16;   goto fixlong;
                  case 'f':   goto dolf;
                  case 'e':   goto dole;
                  case 'g':   goto dolg;
                  default:    p--;
                              base = 10;   goto fixlong;
                  }

dolf:
      case 'f':
#ifdef    MT_VARARGS
                  d      = va_arg( ap, DB );
#else
                  dp     = (DB *) param;
                  param += inc_float;
                  d      = *dp;
#endif
                  if ( precision == -1 )   precision = 6;
                  make_f( d, fbuf, precision );
                  goto loadbuf;

dole:
      case 'e':
#ifdef    MT_VARARGS
                  d      = va_arg( ap, DB );
#else
                  dp     = (DB *) param;
                  param += inc_float;
                  d      = *dp;
#endif
                  if ( precision == -1 )   precision = 6;
                  make_e( d, fbuf, precision );
                  goto loadbuf;

dolg:
      case 'g':
#ifdef    MT_VARARGS
                  d      = va_arg( ap, DB );
#else
                  dp = (DB *) param;
                  param += inc_float;
                  d      = *dp;
#endif
                  if ( precision == -1 )   precision = 6;
                  make_f( d, fbuf, precision );
                  i = strlen( fbuf );

                  make_e( d, fbuf, precision );

                  if ( strlen( fbuf ) >= i )   make_f( d, fbuf, precision );
                  goto loadbuf;

      case '@':
                  if ( ! w )   continue;

                  *q = 0;   (*fcn)( buf );   q = buf;

                  if ( field < 0 )       field = w->w_cx;
                  if ( precision < 0 )   precision = w->w_cy;

                  fwat( field,precision,w );
                  continue;

      case 'a':
#ifdef    MT_VARARGS
                  l      = va_arg( ap, SI );
#else
                  ip     = (SI *) param;
                  param += inc_int;
                  l      = *ip;
#endif
                  if ( ! w )   continue;

                  *q = 0;   (*fcn)( buf );   q = buf;
                  w->w_atrib = l;
                  continue;

      case 'A':
                  if ( ! w )   continue;

                  *q = 0;   (*fcn)( buf );   q = buf;

                  switch ( field )
                  {
                  default:
                  case 1 :    w->w_atrib = atr;          break;
                  case 2 :    w->w_atrib = w->w_optatr;  break;
                  case 3 :    w->w_atrib = ATR_BLINK;    break;
                  case 4 :    w->w_atrib = w->w_defatr;  break;
                  case 5 :    w->w_atrib = w->campoatr;  break;
                  }
                  continue;

      default :
                  goto   print_char;
      }

      continue;

fixlong:  /* l loaded with variable to format - format please */

      r = fbuf;

      if ( l < 0 ) {  *r = '-';  r++;   l *= -1;  }

      if ( ! l )   {  *r = '0';  r++;   *r = 0;   }

      else         loadint( r, l, base, list );

      if ( precision >= 0  &&  strlen( fbuf ) > precision )
      {
         fbuf[ precision ] = 0;

         if ( precision )   fbuf[ precision - 1 ] = '*';
      }

loadbuf:  /* fbuf has info, insert it in place */

      i = field - strlen( fbuf );

      /* i = # bytes to pad */
      if ( !( bias & 1 ))  /* pad first */
      {
         while ( i > 0 )
         {
            if ( bias & 8 )   *q = '0';
            else              *q = ' ';

            q++;

            if ( q > buf + (PRINT_BUF - 2) )
            {
               *q = 0;  (*fcn)( buf );  q = buf;
            }

            i--;
         }
      }

      for( r = fbuf;   *r;   r++ )
      {
         *q = *r;   q++;

         if ( q > buf + (PRINT_BUF - 2) )
         {
            *q = 0;   (*fcn)( buf );   q = buf;
         }
      }

      while( i > 0 )
      {
         if ( bias & 8 )   *q = '0';
         else              *q = ' ';

         q++;

         if ( q > buf + (PRINT_BUF - 2) )
         {
            *q = 0;   (*fcn)( buf );   q = buf;
         }

         i--;
      }
   }

   *q = 0;   (*fcn)( buf );

   if ( w != NULL )  w->w_atrib = atr;

   w = NULL;
}

/* converts long integers to ascii */
ST( VOID ) loadint( buf, val, base, list )

SC   *buf;
SL   val;
SS   base;    /* works any base */
SC   *list;   /* list of ascii conversion values */
{
   SL   x;
   SC   *p;

   p = buf;

   if ( val < 0 )  {  *p = '-';   val *= -1;   p++;  }

   if ( val == 0 ) {  *p = 0;  return;  }

   for( x = val;   x;   x /= base, p++ )
      ;

   *p = 0;   p--;

   for( x = val;   x;   x /= base, p-- )   *p = list[ (SS)(x % base) ];
}


/* load buf with printed version, %e format */
ST( VOID ) make_e( val, buf, prec )

DB   val;
SC   *buf;      /* info load */
SS   prec;      /* # chars after decimal */
{
   SC  *p, *q, *ret;
   SI  exp, neg;
SC *ecvt();

   db2( "make_e" );

   ret = ecvt( val, prec - 1, &exp, &neg );

   if ( ! *ret ) {  *buf = '0';   buf[ 1 ] = 0;   return;  }

   exp--;
   p = buf;
   q = ret;

   if ( neg ) {  *p = '-';   p++;  }

   *p = *q;   p++;   q++;

   *p = '.';  p++;

   while( *q ) {  *p = *q;   p++;   q++;  }

   if ( exp )
   {
      *p = 'e';   p++;

      if ( exp < 0 ) {   *p = '-';   exp *= -1;  }

      else               *p = '+';

      p++;
      *p = '0' + (exp / 10);   p++;
      *p = '0' + (exp % 10);   p++;
   }

   *p = 0;
}


/* makes a %f format result into buf */
ST( VOID ) make_f( val,buf,prec )
DB  val;
SC  *buf;
SS  prec;     /* # digits after dp */
{
   SC  *p, *q, *ret;
   SI  dp, neg;

   SC *fcvt();

db2( "make_f" );

   ret = fcvt( val, prec, &dp, &neg );

   p = buf;
   q = ret;

   if ( ! *q ) {  *buf = '0';   buf[1] = 0;   return;  }

   if ( neg ) {  *p = '-';   p++;  }

   while( *q )
   {
      if ( ! dp ) {  *p = '.';   p++;  }

      dp--;  *p = *q;  p++;  q++;
   }

   *p = 0;
}

static VOID towndo( s ) /*** Nao pode ser NEAR ***/ /* for use by uprintf */
UC  *s;
{
   fwputs( s,w );
}

