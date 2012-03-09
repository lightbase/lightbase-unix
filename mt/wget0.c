
#ifdef NAODEF

fwgc( win )      - get a character ( like getchar())
fwqgc( win )     - like getchar() but never echos
                   ( other get_flag parameters monitered )
fwnqgc( win )    - chama fwqgc porem SEM REFRESH
                   ( other get_flag parameters monitered )
w_rgc()          - raw getchar - prior to all conversions
                   ( no echo, no get_flag check )
w_ungs ( )       Usa w_ungc( SS ); para colocar uma string
w_ungc( c )      - unget, make to be next UC gotten.
                   ( UC not gotten by w_rgc() )
w_kbd ( kbd )    /*** Usa w_ungc( SS ); na ordem natural ***/
                 /*** Exemplo: w_kbd( 'T', 'e', 's', 't', 'e', 0 ); ***/
                 /*** ATENCAO: ZERO termina a cadeia !!! ***/
w_kbhit()        - returns 0 or UC pending @ w_rgc()
                   bug - if UC is start of a convert,
                         emmediate get not guaranteed.
win_kdsc( key )  - returns description of user defined keys.
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef MSDOS
#include  <dir.h>
#include  <signal.h>
#else
#ifdef   VAXVMS
#include <signal.h>
#else
#ifdef   XENIX
#include  <signal.h>
#else
#include <sys/signal.h>
#endif
#endif
#endif

#include <window.h>
#include <mensagem.h>

#include PROTO

#ifdef   VAXVMS
#include <file.h>
#else
#include <fcntl.h>
#endif

#ifdef   MSDOS
#include  <io.h>
#include  <dos.h>
#endif

#ifdef    PROTO_ANSI
ST( UC )   doconv( WIN *, UC );
UC         *key_retry( UC *str );
#else
ST( UC )   doconv( );
UC         *key_retry( );
#endif

#define MAX_KEYS   80

static UC val_ungot = 0;                /* number of ungot variables */
static UC ungotbuf[ MAX_KEYS + 1 ];     /* ungot vars go in here */

static UC retbuf[ MAX_KEYS + 1 ];       /* max, in convert */
static UC *retry = 0;                  /* if it pts to anything, then conv. failed */

static UC convline[ MAX_KEYS + 1 ];     /* max, out convert */
static UC *pcline = 0;                  /* if it pts to anything then converting */

/*** Os 4 defines de baixo terao de serem diferentes */
#define   W_RGC       0
#define   PCLINE      1
#define   UNGOT       2
#define   MACRO       3

#ifdef VAXVMS

/*
 * Attempt to do some trick QIO's
 */

#include descrip
#include ssdef
#include ttdef
#include iodef

int ttchan;
int vms_key;
int status[ 2 ];

int SYS$QIO( );
int SYS$CANCEL( );
int SYS$ASSIGN( );

int vms_ast_tty( );
int vms_init_io( );
int vms_reset_io( );
int vms_sys_qio( );

int vms_sys_qio( w_exit )

int w_exit;
{
   int ret;

   ret = SYS$QIO( 0, ttchan,
                  IO$_READVBLK |
                  IO$M_NOECHO  |
                  IO$M_NOFILTR,
                  &status[ 0 ],
                  &vms_ast_tty,
                  0,
                  &vms_key,
                  1, 0, 0, 0, 0 ) );


   if ( w_exit  &&  ret  !=  SS$_NORMAL )
   {
      debug( "%s", loc_str( STR_DISCO, DISCO_510 ));
      win_exit( 510 );
   }

   return( ret );
}

int vms_reset_io( )
{
   return( SYS$CANCEL( ttchan ) );
}

int vms_init_io( w_exit )

int w_exit;
{
   $DESCRIPTOR( ttname, "TT" );

   if ( SYS$ASSIGN ( &ttname, &ttchan, 0, 0 )  !=  SS$_NORMAL )
   {
      debug("%s", loc_str( STR_DISCO, DISCO_511 ));
      win_exit( 511 );
   }

   if ( vms_sys_qio( w_exit )  !=  SS$_NORMAL )   return( 1 );

   return( 0 );
}

int vms_ast_tty( )
{
   UC  C, buf[ 2 ];

   /*** Para uma maior supervisao da linha troque os proximos ... ***/
   /*** ... #ifdef NAODEF por #ifdef VAXVMS. ***/

   if ((status[ 0 ] & 0xFFFF) == SS$_CANCEL )
   {
#ifdef   NAODEF
      perror("\nSS$_CANCEL recebido");
      exit (SS$_NORMAL);
#else
      return( 1 );
#endif
   }

   if ((status[ 0 ] & 0xFFFF) != SS$_NORMAL )
   {
#ifdef   NAODEF
      perror("\nQIO ERROR");
      exit( LIB$SIGNAL( status[ 0 ] & 0xFFFF ) );
#else
      return( 1 );
#endif
   }

   if ( ( C = vms_key ) == 0 )   C = 255;

   buf[ 0 ] = C;  buf[ 1 ] = '\0';

   key_retry( buf );

   vms_sys_qio( 1 );
}

#endif /* VAXVMS */


SS w_ungc( c ) /*** ungetchar functional to size of buffer "ungotbuf"  ***/

UC c;
{
   if( val_ungot > (sizeof( ungotbuf ) / sizeof( UC )) - 1 )  return( -1 );
   ungotbuf[ val_ungot++ ] = c;
   return 0;
}


SS   w_kbhit( win ) /*** returns UC destined to be returned by w_rgc() ***/
                    /*** if none,  then returns 0 ***/
WIN  *win;          /*** Se preucupa com EXECUCAO de MACRO ! ***/
{
#ifdef    MEDIT
   if( rd_macro != NULL ) return((SS) 1 ); /* Esta lendo MACRO */

   return ( wq_kbhit( win ) );
#else
   if( rd_macro != NULL ) return((SS) 0 ); /* Esta lendo MACRO */

   return ( wq_kbhit( win ) );
#endif
}


SS  wq_kbhit( win ) /*** returns UC destined to be returned by w_rgc() ***/
                    /*** if none,  then returns 0 ***/
WIN *win;
{
#ifdef MSDOS

   win = win;

   /*** A ordem deve ser esta, veja em fwnqgc( ) a mesma ordem ***/
   if( val_ungot )  return((SS) ungotbuf[ val_ungot - 1 ] );
   if( pcline )     return((SS) *pcline ); /* Convertido em mais de 1 char */
   if( retry )      return((SS) *retry );  /* Conversao nao encontrada */

   return( kbhit( ) );

#else
#ifdef   VAXVMS

   /*** A ordem deve ser esta, veja em fwnqgc( ) a mesma ordem ***/
   if( val_ungot )  return((SS) ungotbuf[ val_ungot - 1 ] );
   if( pcline )     return((SS) *pcline ); /* Convertido em mais de 1 char */
   if( retry )      return((SS) *retry );  /* Conversao nao encontrada */

   /*** No VMS a leitura assincrona le via retry ***/
   return( 0 );

#else

   SI  a;
   UC  c;   /*** ATENCAO - Tem que ser UC, inicializacao e read( ) ***/
   UC  buf[ 2 ];

   /*** A ordem deve ser esta, veja em fwnqgc( ) a mesma ordem ***/
   if( val_ungot )  return((SS) ungotbuf[ val_ungot - 1 ] );
   if( pcline )     return((SS) *pcline ); /* Convertido em mais de 1 char */
   if( retry )      return((SS) *retry );  /* Conversao nao encontrada */

   /*** ATENCAO ***/
   /*** Ligue o Alarme apenas durante o read( ). ***/
   /*** O alarme() sai de toda e qualquer funcao read() com erro -1 ***/
   /*** porisso nao e' conveniente ficar ligado permanentemente ***/
   if ( hm_win != NULL && time( NULL ) >= hm_time + (SL) hm_seg )
   {
      hm_alarme( );
      fwrefresh( hm_win );
      if ( win != NULL )   fwsetcursor( win );
   }

ler:

   fcntl( 0, F_SETFL, O_NDELAY );
   a = read( 0, &c, 1 );
   fcntl( 0, F_SETFL, 0 );

   if ( a == -1 )
   {
      /*** Pode ser forcado pelo alarme() ***/
      /*** Se for o caso reescreva a janela ***/
      if ( hm_win != NULL )
      {
         fwrefresh( hm_win );
         if ( win != NULL )   fwsetcursor( win );
      }
   }

   /*** Desligue o Alarme !!! ***/
   hm_halt( );

   if ( a != 1 )   return 0;
   if ( ! c )      c = 255;

   buf[ 0 ] = c;  buf[ 1 ] = '\0';
   key_retry( buf );

   return((SS) *retry );
#endif
#endif
}


SS fwqgc( win )      /* like fwgc, but no echo */

struct Window *win;
{
   db1( "fwqgc" );

   fwrefresh( win );
   fwsetcursor( win );

   return ( fwnqgc( win ) );
}

SS fwnqgc( win )

struct Window  *win;
{
   UC   A;
   SS   conv = W_RGC;

   /*** val_ungot deve ser tratada em primeiro lugar, para quando ... ***/
   /*** ... uma tecla for lida e seja w_ungc( tecla ) esteja ... ***/
   /*** ... no topo da pilha. Veja next_letter() no Editor. ***/

   /*** Algum caracter foi "unget" - w_ungc( c ) ***/
   if( val_ungot )
   {
      A = ungotbuf[ --val_ungot ];
      conv = UNGOT;                        /*** Use 'A' de ungotbuf ***/
   }

   /*** Sequencia foi convertida em mais de um caracter - doconv( ) ***/
   else if( pcline )
   {
      A = *pcline;
      pcline++;
      if( ! *pcline ) pcline = 0;
      conv = PCLINE;    /*** Complete conversao, use 'A' de pcline ***/
   }

   else if ( rd_macro != NULL )
   {
#ifdef    MEDIT
      if ( mr_read( &A, NAO_FILTRA ) == 1 )
      {
         conv = MACRO;  /*** Use 'A' de rd_macro ***/
      }

      else
      {
         /*** Return Tecla invalida para executar o move_scr() ***/
         return ( -1 );
      }
#else
      if ( interrompe_macro( ) )   goto   recursivo;

      if ( mr_read( &A, FILTRA_NL ) == 1 )
      {
         if ( A != COMANDO )   conv = MACRO;  /*** Use 'A' de rd_macro ***/

         else  {  analisa_demo( win );  goto   recursivo;  }
      }
#endif
   }

   if ( conv == W_RGC ) /** Tem permissao para ler do teclado - STDIN ? **/
   {
      A = w_rgc( win );

      /* check if convert needed */
      if( strchr( (SC *) kc_fary, A ) )
      {
         if ( (A = doconv( win, A )) == 0 )    return fwnqgc( win );
      }
   }

   if (wr_macro != NULL &&
      (conv == W_RGC || conv == PCLINE || conv == MACRO)) mw_putc( A );

   if( A == tec_util )                    /* check for interrupt process */
   {
      fwrefresh( win );
      fwintr( win, util_flag );
      goto   recursivo;
   }

   if( A == tec_ajuda )                    /* check for HELP process */
   {
      fwrefresh( win );
      f_ajuda( win_ajuda, 0 );
      goto   recursivo;
   }

#ifdef DISCADOR
   if( A == K_ALT_F1 )           /* Tecla para ativar programa de discagem */
   {  VOID cli_disca( );
      fwrefresh( win );
      cli_disca( );
      goto   recursivo;
   }
#endif

   return( (SS) A );

recursivo:

   fwrefresh( win );
   fwsetcursor( win );
   return fwnqgc( win );
}


SS  w_rgc( win )    /* raw get UC - no conversions. */

WIN  *win;
{
#ifndef   MSDOS
   SI     a;
#endif
   UC     c;    /*** ATENCAO - Tem que ser UC, inicializacao e read( ) ***/

   win = win;

   /*** doconv( c ) - Sequencia que nao foi convertida ***/
   if( retry )
   {
      c = *retry;
      retry++;
      if( ! *retry ) retry = NULL;
      return((SS) c );
   }

#ifdef MSDOS

#ifdef DISCADOR
   while( ! kbhit( ) )
   {
      cli_chkmsg( );
   }
#endif

   c = getch( );

   /*** Por definicao - JAMAIS poderemos receber Zero !!! ***/
   if ( ! c )   c = 255;

   return((SS) c );

#else
#ifdef   VAXVMS

   /*** Precisamos de uma leitura Sincrona ***/
   while( 1 )
   {
      sleep( 1 );

      if ( retry )
      {
         c = *retry;
         retry++;
         if( ! *retry ) retry = NULL;
         return((SS) c );
      }
   }

#else

   /*** ATENCAO ***/
   /*** Ligue o Alarme apenas durante o read( ). ***/
   /*** O alarme() sai de toda e qualquer funcao read() com erro -1 ***/
   /*** porisso nao e' conveniente ficar ligado permanentemente ***/
   hm_alarme( );

ler:

   a = read( 0, &c, 1 );

   if ( a == -1 )
   {
      /*** Pode ser forcado pelo alarme() ***/
      /*** Se for o caso reescreva a janela ***/
      if ( hm_win != NULL )
      {
         fwrefresh( hm_win );
         if ( win != NULL )   fwsetcursor( win );
      }
      goto  ler;
   }

   /*** Desligue o Alarme !!! ***/
   hm_halt( );

   if ( a != 1 )   c = 0;

   /*** Por definicao - JAMAIS poderemos receber Zero !!! ***/
   if ( ! c )      c = 255;

   return( (SS) c );

#endif
#endif
}


ST(UC) doconv( win, c )/* something may need converting first UC of conversion */

WIN *win;
UC c;
{
   UC    mini[ 2 ];
   SS    a, b, x;
   SS    matching;
   SS    convlen = 1;

   *convline = c; convline[1] = 0; mini[1] = 0;

   /* point a to first convert w same start UC */
   for ( a = 0;   a < kc_sp;   a++ )
   {
      if ( c > *kc_ary[ a ].kc_from )  continue;
      if ( c < *kc_ary[ a ].kc_from )  goto err; /* shouldn't get here */
      break;
   }

   if( a == kc_sp )   goto err;                  /* shouldn't get here */

   /* now build a string to find matching convert */
   matching = 1;

   while( matching )
   {
      matching = 0;

      for( b = a;   b < kc_sp;   b++ )
      {
         x = strncmp( (SC *) convline, (SC *) kc_ary[ b ].kc_from, convlen );

         if( ! x )
         {
            if( ! kc_ary[ b ].kc_from[ convlen ] )
            {
               /* MATCH FOUND */
               strcpy( (SC *) convline, (SC *) kc_ary[ b ].kc_to );
               pcline = convline + 1;
               if ( ! *convline  ||  ! *pcline )   pcline = 0;
               return  *convline;
            }

            matching = 1;
         }

         if( x < 0 )   break;   /* too far, find another string */

         if( x > 0 )   a = b;
      }

      if( ! matching )   break;

      /* must try getting another UC */
      if( get_flag & WG_IMPAT )
      {
         x = 0;
         for( b = IMPATIENCE;   b;   b-- )
         {
            if ( (x = w_kbhit( win )) != 0 )   break;
         }

         if ( !x ) /* AM IMPATIENT, give up */  break;
      }

      x = w_rgc( win );

      /*** Pra que isto ? ***/
      /*** if( x == -1 )   break; ***/  /*** no match ***/

      *mini = x;
      convlen++;
      strcat( (SC *) convline, (SC *) mini );
   }

   /* IF HERE, THEN BAD CONVERT, STUFF LOST CHARS INTO retbuf */
   key_retry( convline + 1 );

   return *convline;

err:
   debug( "\n%s\n", loc_str( STR_DISCO, GET_CORR ) );
   return ( 'Z' );
}


UC *key_retry( str )

UC *str;
{
   SS  A, B;
   UC  buf[ MAX_KEYS + 50 ];

   if ((A = strlen((SC *) str ))  >  0 )
   {
      buf[ 0 ] = '\0';

      if ( retry )   strcpy((SC *) buf, (SC *) retry );

      B = MAX_KEYS - strlen((SC *) buf );
      if ( A  >  B )   A = B;
      str[ A ] = '\0';

      strcat((SC *) buf, (SC *) str );

      strcpy((SC *) retbuf, (SC *) buf );
      retry = retbuf;
      if ( ! *retry )   retry = NULL;
   }

   return( retry );
}


