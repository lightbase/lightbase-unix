
#ifdef comout

                            /* 255 => Janela de Help */:
        win_init( b )           - must be run at program start
        win_shell( ) - Local Shell
        win_quit()              - must be run at program end
        win_exit( x )           - does a clean exit - also run
                                  on signal SIG_TERM
        setquit( fcn )          - sets a function to be run w/ win_exit call

#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <window.h>

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

#include "mensagem.h"
#include "ethead.h"

int vms_init_io ( );
int vms_reset_io( );

#ifdef     PROTO_ANSI

VOID       handler ( SI );
WIN *      alloc_win( SS, SS );
VOID       free_win( WIN * );
ST( VOID ) std_default( VOID );
SS         find_params( VOID );

#else

VOID       handler ( );
WIN *      alloc_win( );
VOID       free_win( );
ST( VOID ) std_default( );
SS         find_params( );

#endif

ST( VOID ) std_default()    /* THIS ROUTINE INSERTS THE STANDARD DEFAULTS
                           INTO THE WINDOW SYSTEM, IF THEY NEED ADJUSTING,
                           ADJUST THEM HERE */
{

db2( "std_default" );
  main_win->w_flags = 0;/* no box, not emediate, scrolable, stop at eol,
                           no visible force, no forced high  */
                        /* boxes may be set by win_init */
  main_win->w_intr = 255;               /* 121 is a reasonable default set */
  main_win->w_atrib  = ATR_NORMAL;
  main_win->w_defatr = ATR_BOLD;
  main_win->w_optatr = ATR_REVERSO;
  main_win->w_boxatr = ATR_NORMAL;
  main_win->campoatr = ATR_NORMAL;
  main_win->t_atr    = ATR_REVERSO;
  main_win->w_curstyle = 0;             /* use basic cursor style */
  main_win->type     = 0;
  main_win->title    = "";
  main_win->bottom   = "";
  main_win->ident = 0; /* Nao mudar, No. para administrar que janela moveu */
                       /* Main_Win - Nao Pode Mover ! */

  main_win->w_anymod = 3; /* clear the screen now */
  w_fixmain();
  main_win->w_anymod = 2;
}

SS win_init( arq_men, b )   /* initial window preparation:
                               - inicializa Mensagens
                               - learns of terminal type.
                               - stty's the standard port.
                               - clears the screen.
                               - allocates main_win & def_win.
                               - establishes values for all variables.
                               ret: 0/err
                             */
SC *arq_men;       /* Arquivo de Mensagens */
SS b;              /* if( b ) then boxes are assumed */
{
#ifndef MSDOS
#ifndef VAXVMS
  FILE *getstty;
#endif
#endif
  SC buf[200];
db1( "win_init" );

   /*** String Vazia ***/
   str_vazia[ 0 ] = '\0';

   /*** Adapte Relogio ***/
   hm_win  = NULL;        /* Desligue o relogio */
   hm_x    = hm_y = 0;    /* Cordenadas X, Y em hm_win */
   hm_seg  = 65;          /* Delta em segundos para atualizacao de hm_win */
   hm_fmt  = "%02d:%02d"; /* Formato de HH:MM em hm_win */
   hm_time = 0;           /* Segundos retornado por time( ) */
   hm_ok   = 0;           /* Permite impressao do relogio */

   /*** Habilita REFRESH ***/
   w_refresh = 0;
   fresh_read = 0;

   /*** Inicialize SEPARADORES ***/
   sep_int = '.';
   sep_dec = ',';

   /*** Inicialize Versao ***/
   versao = "1.0";
   edicao = "Beta";

   /*** Inicialize MACROS ***/
   wr_macro = NULL;
   rd_macro = NULL;

   /* try to find term desc file */
   terminal  = getenv( env_term );
   diretorio = getenv( env_dir  );

   if ( terminal  == NULL  ||  *terminal  == 0 )   terminal  = TERM_DEF;
   if ( diretorio == NULL  ||  *diretorio == 0 )   diretorio = DIR_DEF;

   tira_separador( diretorio );

   /*** Inicialize Mensagens, ':' no caso do MSDOS com "C:dir1" ***/
   if ( arq_men[ 0 ] != DIRSEP  &&  arq_men[ 1 ] != ':' )
   {
      sprintf( buf, "%s%c%s", diretorio, DIRSEP, arq_men );
   }

   else   strcpy( buf, arq_men );

   /*** Inicialize Memoria virtual ***/
#ifdef  VMEM
{
   SC  *D;

#ifdef  MSDOS
   /*** Nao usar loc_str() pois as strings ainda nao foram inicializadas ***/
   if ((D = getenv( "VMDIR" )) != NULL )    VM_dir = D;
   else                                     VM_dir = diretorio;

   tira_separador( VM_dir );

#else
   /*** No Unix o default e' "." ***/
   /*** Nao usar loc_str() pois as strings ainda nao foram inicializadas ***/
   if ((D = getenv( "VMDIR" )) != NULL ||
       (D = getenv( "HOME"  )) != NULL  )   VM_dir = D;
   else                                     VM_dir = diretorio;

   tira_separador( VM_dir );
#endif
}

   if ( vm_start( ) == 0 )   return( -2 );
#endif

   if ( ! start_str( buf ) )    return -3;

   /*** Inicialize Editor ***/
   et_primeiro( );

  /* read the terminal parameters */
  if( find_params()) return -1;
  if( init_seq && *init_seq )  { doputs( init_seq ); doflush( ); }
  if( *atrib_ary[0].at_on  &&  *atrib_ary[0].at_on ) {
    doputs( atrib_ary[ 0 ].at_on );
  }
  if( init_proc && *init_proc )
         system( ( SC * ) init_proc );

#ifdef MSDOS
   *stty_orig = 0;
#else
#ifdef VAXVMS
   /*** vms_init_io() com parametro 0 para nao sair em caso de erros ***/
   if ( vms_init_io( 0 ) )   return( -4 );
   *stty_orig = 0;
#else
#ifdef   ULTRIX

   /* Nao aceita stty cuja saida nao seja para o terminal */
   strcpy( stty_orig, loc_str( STR_OBRIG, OBRIG_34 ) );

   /* initialise the port */
   if ( init_stty && *init_stty )
   {
      sprintf( buf,"stty %s",init_stty );
      system( buf );
   }

#else
   /* initialise the port */
   if ( init_stty && *init_stty )
   {
      getstty = popen( "stty -g", MODO_RO );
      fgets( stty_orig, TAM_STTY_G, getstty );
      stty_orig[ strlen( stty_orig ) - 1 ] = 0;
      pclose( getstty );
#ifdef SCOUNIX
      sprintf( buf,"stty %s 2>/dev/null",init_stty );
#else
      sprintf( buf,"stty %s",init_stty );
#endif
      system( buf );
   }
   else   *stty_orig = 0;
#endif
#endif
#endif

  /* set up blank main_win */
  main_win = alloc_win( size_x,size_y );
  if( ! main_win ) return -1;
  main_win->w_num = 0;
  main_win->w_len = size_x;
  main_win->w_height = size_y;
  main_win->w_alen = size_x;
  main_win->w_aheight = size_y;
  main_win->w_x = 0;
  main_win->w_y = 0;
  main_win->w_ax = 0;
  main_win->w_ay = 0;
  main_win->w_cx = 0;
  main_win->w_cy = 0;
  win_map = (UC *)malloc( size_x * size_y );
  if( ! win_map ) return -1;
  std_default( );
  if( b )
        { main_win->w_flags |= W_BOXED;
          def_win = win_make( size_x - 2,size_y - 2,
                              size_x - 2,size_y - 2,
                              1,1 );
        }
   else
        def_win = win_make( size_x,size_y,size_x,size_y,0,0 );
  if( ! def_win ) { free_win( main_win ); return -1; }
  curr_win = def_win;
  tcp_x = -1; tcp_y = -1;

  /* initialise the terminal */
  win_refresh();

#ifdef MSDOS
   signal( SIGINT,  handler );   /* trap the control C */
#else
   signal( SIGHUP,  win_exit ); /*  1 Queda de Portadora ou Lider de Grupo */
   signal( SIGINT,  win_exit ); /*  2 Interrupt DEL, Ctrl-C */
   signal( SIGQUIT, win_exit ); /*  3 Violacao de Endereco */
   signal( SIGILL,  win_exit ); /*  4 Instrucao ilegal */
   signal( SIGBUS,  win_exit ); /*  10 violacao de endereco */
   signal( SIGSEGV, win_exit ); /*  11 violacao de segmento */
   signal( SIGTERM, win_exit ); /* 15 Soft Signal */
#endif

  fun_config = 0;
  quitprog   = 0;
  move_win   = 0;
  win_ajuda  = NULL;
  user_help  = NULL;
  dir_ajuda  = NULL;
  edit_help  = 1;    /*** Permissao para editar HELP ***/
  insercao   = 0;    /*** Modo insercao DESLigado por default ***/
  tec_util   = K_F3;
  tec_ajuda  = K_F1;
  util_flag  = -1;   /*** Tudo Ligado ***/

  return 0;
}


VOID win_shell( command )

SC   *command;
{
   UC    buf[ 300 ];
#ifdef   MSDOS
   SC   *env_prompt = "PROMPT", prompt[120], *dos_prompt;
   SC    dir[ 100 ];
   SI    erro_hold, drive;
   VOID  setenv( SC **environ );
   SI    holdev( SC *, SI, SC * );
#endif

   /* RESET TERMINAL PROPERLY */
#ifndef MSDOS
#ifdef VAXVMS
   vms_reset_io( );
#else
   if( *stty_orig )
   {
#ifdef SCOUNIX
      sprintf( (SC *) buf,"stty %s 2>/dev/null",stty_orig );
#else
      sprintf( (SC *) buf,"stty %s",stty_orig );
#endif
      system( (SC *) buf );
   }
#endif
#endif

   if( *term_proc )  system( (SC *) term_proc );
   if( *term_seq )   doputs( term_seq );
   doflush( );

   /* INVOKE THE SHELL */
   prclear( );
   win_cpush( CS_INTER );
   doflush( );

#ifdef   MEDIT
   if ( command != NULL )
   {
      fprintf( stdout, "\n%s\n", command );
      doflush( );
   }
#endif

#ifdef MSDOS

   dos_prompt = getenv( env_prompt );
   sprintf( prompt, "%s=%s %s",
            env_prompt, loc_str( STR_OBRIG, PROMPTDOS ), dos_prompt );
   putenv( prompt );
   drive = getdisk( );
   getcwd( (SC *) buf, 80 );

   if ( command == NULL )
      fprintf( stdout, "\n\n%s\n", loc_str( STR_DISCO, SH_DOS ) );

#ifdef  VMEM
   /*** VM_dir ou esta setada pela variavel VMDIR ou e' diretorio ***/
   sprintf( dir, "%s%c", VM_dir, DIRSEP );
#else
   sprintf( dir, "%s%c", diretorio, DIRSEP );
#endif
   /* Falta o arquivo em "dir" para que holdev() procure um arquivo unico */
   /* command == NULL executa "command.com" */
   /*** system( command );  ***/
   setenv( environ );
   erro_hold = holdev( dir, 0, command );

   sprintf( prompt, "%s=%s", env_prompt, dos_prompt );
   putenv( prompt );
   setdisk( drive );
   chdir( (SC *) buf );

#else
   if ( command == NULL )
   {
      sprintf( buf, loc_str( STR_OBRIG, OBRIG_35 ),
                    diretorio, DIRSEP,
                    loc_str( STR_DISCO, SH_UNIX ) );
      {
	      system( buf );
	   }
   }

   else   system( command );
#endif

   /* RESET THE INIT */
   if( *init_seq ) {  doputs( init_seq );  doflush( );  }
   if( *init_proc )   system( (SC *) init_proc );

#ifndef MSDOS
#ifdef VAXVMS
   /*** vms_init_io() com parametro 1 para sair em caso de erro ***/
   vms_init_io( 1 );
#else
   if( *init_stty )
   {
#ifdef SCOUNIX
      sprintf( buf,"stty %s 2>/dev/null",init_stty );
#else
      sprintf( buf,"stty %s",init_stty );
#endif
      system( buf );
   }
#endif
#endif

#if   MEDIT
   if ( command != NULL )
   {
      SI   C;

      fprintf( stdout, "\n%s ", loc_str( STR_DISCO, PRESS_TEC ) );
      doflush( );

      while ((C = fgetc( stdin )) != '\n'  &&  C != '\r' )
         ;
   }
#endif

   win_cpop( );
   doflush( );

   win_redo();

#ifdef MSDOS
   if ( erro_hold )   debug( loc_str( STR_DISCO, DISCO_509 ) );
#endif
}


VOID win_quit()                 /* ALWAYS WORKS, terminates window system
                                   MUST BE RUN before exiting program
                                   is run by Wexit( )                     */
{
#ifndef MSDOS
#ifndef VAXVMS
   UC buf[300];
#endif
#endif

   win_csset( 0 );      /* put cursor normal again! */

#ifndef MSDOS
#ifdef VAXVMS
   vms_reset_io( );
#else
   if( *stty_orig )
   {
#ifdef SCOUNIX
      sprintf( (SC *) buf,"stty %s 2>/dev/null",stty_orig );
#else
      sprintf( (SC *) buf,"stty %s",stty_orig );
#endif
      system( (SC *) buf );
   }
#endif
#endif

   while( win_sp )   free_win( win_stak[ --win_sp ] );

   free_win( main_win );
   free( (SC *)  win_map );

   if ( term_proc && *term_proc )   system( ( SC * ) term_proc );

   if ( term_seq  &&  *term_seq )   doputs( ( UC * )term_seq );

   /* that call frees all memory used by term. defs */
   if ( win_params != NULL )  free( (SC *)  win_params );
   doflush( );

   /*** Finalize Editor ***/
   et_ultimo( );

   /*** Encerre Mensagens ***/
   end_str( );

   /*** Libere Memoria Virtual ***/
#ifdef    VMEM
   vm_end( );
#endif
}


VOID win_exit( x )       /* like exit(), but: cleans up window system,  */
                         /*   and runs any function in (*quitprog)() */
SS x;                          /* value returned */
{
   if( quitprog ) {  if ( ! ( *quitprog )( ) )   return;  }

   else  {  W_curmove( 0, 0 );  prclear( );  doflush( );  }

  win_quit( );

  exit( x );
}


VOID setmove( fcn )
SS (*fcn)();                   /* fcn may equal NULL */
{
   /* move_win e' chamada toda vez que uma janela */
   /* for movida uma mudada de tamanho            */

   move_win = fcn;
}

VOID setquit( fcn )               /* installs supplied function into quitprog,
                                IT IS THE RESPONSIBILITY OF THE SUPPLIED
                                FUNCTION TO ASSURE THAT ANY OTHER VALUE
                                IN quitprog IS NOT OVERRITTEN */
SS (*fcn)();                   /* fcn may equal NULL */
{
  quitprog = fcn;
}


VOID hm_alarme( )
{
#ifdef MSDOS

   return;

#else

   SS  X, Y;
   SC  hm[ 100 ];
   struct tm *tptr;

   if ( hm_win == NULL )   return;

   if ( hm_ok )
   {
      hm_time = time( NULL );
      tptr    = localtime( &hm_time );

      X = hm_win->w_cx;
      Y = hm_win->w_cy;

      sprintf( hm, hm_fmt, tptr->tm_hour, tptr->tm_min, tptr->tm_sec );
      fwat( hm_x, hm_y, hm_win );

      fwpsl( hm, hm_win, strlen( hm ) );

      hm_win->w_cx = X;
      hm_win->w_cy = Y;
   }

   signal( SIGALRM, hm_alarme );
   alarm( hm_seg );
#endif
}


VOID hm_set( win, x, y, seg, fmt )

WIN *win;    /* Janela onde serao colocados HH:MM */
SS  x, y;    /* Cordenadas X, Y em hm_win */
SS  seg;     /* Delta em segundos para atualizacao de hm_win */
SC  *fmt;    /* Formato de HH:MM em hm_win */
{
   hm_win = win;
   hm_x   = x;
   hm_y   = y;
   hm_seg = seg;
   hm_fmt = fmt;

   hm_ok = 1;

   hm_alarme( );

   hm_halt( );
}


VOID hm_halt( )
{
#ifdef MSDOS

   return;

#else

   signal( SIGALRM, SIG_IGN );
   alarm( 0 );
   signal( SIGALRM, SIG_IGN );

#endif
}


