
/*** Definicoes Basicas de Portabilidade ***/

#ifndef MSDOS
#ifndef XENIX
#ifndef IX386
#ifndef SCOUNIX
#ifndef PA800R
#ifndef ULTRIX
#ifndef VAXVMS
#ifndef SUN
#ifndef AIX
#ifndef U6000
#ifndef DECALFA

/***
Se chegar ate' aqui esta' errado ...
... pois algum destes Defines deveria ...
... existir !!!
***/

#define   ERRO_DEFINE_ERRO

#endif /* MSDOS   */
#endif /* XENIX   */
#endif /* IX386   */
#endif /* SCOUNIX */
#endif /* PA800R  */
#endif /* ULTRIX  */
#endif /* VAXVMS  */
#endif /* SUN     */
#endif /* AIX     */
#endif /* U6000   */
#endif /* DECALFA */

#ifndef   ERRO_DEFINE_ERRO

#define   ET_UNDO     /* Habilita UNDO no Editor */

#ifdef    MSDOS

/* #define   PROTO_ANSI   Habilita Prototipos de Funcoes */
#define   VMEM        /* Habilita Memoria Virtual */

#define   MEMORY   <mem.h>
#define   PROTO    <stdlib.h>

#endif

#ifdef AIX

#define   MT_VARARGS  /* Usar <varargs.h> -> PROTO_ANSI DESLIGADA */

#define   MEMORY   <memory.h>
#define   PROTO    <stdlib.h>

#define   STR_PROPRIA
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strupr   w_strupr

#endif

#ifdef ULTRIX

#define   MT_VARARGS  /* Usar <varargs.h> -> PROTO_ANSI DESLIGADA */

#define   MEMORY   <memory.h>
#define   PROTO    <stdlib.h>

#define   STR_PROPRIA
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strncmp  w_strncmp
#define   strupr   w_strupr
#define   strstr   w_strstr

#endif

#ifdef SUN

#define   MT_VARARGS  /* Usar <varargs.h> -> PROTO_ANSI DESLIGADA */

#define   MEMORY   <memory.h>
#define   PROTO    <stdlib.h>

#define   STR_PROPRIA
#define   memmove  w_memmove
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strncmp  w_strncmp
#define   strupr   w_strupr
#define   strstr   w_strstr

#endif

#ifdef VAXVMS

/***
SET TERM/NOECHO   -> Inibe eco para o terminal
        /NOESCAPE -> Aceita sequencias parciais de escape
        /PASTHRU  -> Passa Controls
        /NOTTSYNC -> Xon/Xoff
        /NOLINE   -> Nao intercepta as setas

DEF LBTERM elebra_7.lbt

^C -> Cancela Operacao
^Z -> Cancela Subrotina
^Y -> Cancela Processo
^A -> Insert Mode
^O -> Toggle Output para o Terminal
^U -> Nova linha de edicao

Os arquivos so podem ser abertos se ou possuirem o path completo no
estilo UNIX, comecando por "/", ou simplismente o nome do arquivo.

O nome de arquivo sempre devem possuir o "." separador.
***/

#define   MEMORY   <string.h>
#define   PROTO    <stdlib.h>

#define   unlink   remove
#define   tell( handle )   lseek( handle, 0L, 1 )

#define   STR_PROPRIA
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strupr   w_strupr

#endif

#ifdef PA800R

#define   MT_VARARGS  /* Usar <varargs.h> -> PROTO_ANSI DESLIGADA */

#define   MEMORY   <memory.h>
#define   PROTO    <stdlib.h>

#define   STR_PROPRIA
#define   strlwr   w_strlwr
#define   strupr   w_strupr
#define   stricmp  w_stricmp

#endif

#ifdef SCOUNIX

/* #define   PROTO_ANSI  Habilita Prototipos de Funcoes */

#define   MEMORY   <memory.h>
#define   PROTO    <stdlib.h>

#define   STR_PROPRIA
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strupr   w_strupr

#endif

#ifdef DECALFA

#define   MT_VARARGS
#define   MEMORY   <memory.h>
#define   PROTO    <stdlib.h>

#define   STR_PROPRIA
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strupr   w_strupr
#define   strstr   w_strstr

#endif

#ifdef U6000

/* #define   PROTO_ANSI  Habilita Prototipos de Funcoes */

#define   MEMORY   <memory.h>
#define   PROTO    <stdlib.h>

#define   STR_PROPRIA
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strupr   w_strupr

#endif

#ifdef XENIX

#define   MEMORY   <memory.h>
#define   PROTO    <prototypes.h>

#define   STR_PROPRIA
#define   memmove  w_memmove
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strncmp  w_strncmp
#define   strupr   w_strupr
#define   strstr   w_strstr

#endif

#ifdef IX386

#define   MEMORY   <memory.h>
#define   PROTO    <stdlib.h>

#define   STR_PROPRIA
#define   memmove  w_memmove
#define   stricmp  w_stricmp
#define   strlwr   w_strlwr
#define   strncmp  w_strncmp
#define   strupr   w_strupr
#define   strstr   w_strstr

#endif

/*** Se PROTO_ANSI NAO PODE EXISTIR MT_VARARGS ***/
#ifdef   PROTO_ANSI
#ifdef   MT_VARARGS
#undef   MT_VARARGS
#endif
#endif

/**************************** Habilitar Debugs ******************************/

/* #define ST_DEBUG  */
               /* Se ST_DEBUG estiver ligada habilita                      */
               /*    stak( ) - if( win != win_stak[ win->w_num ] ) return; */

/* #define DB_DEBUG  */
               /* Se DB_DEBUG estiver ligada habilita                      */
               /*    db1(  ) - debug de entrada em cada funcao;            */

/********************** DIRTERORIOS E TIPOS DE TERMINAL *********************/

#ifdef MSDOS

#define INCR_TEXTO  2  /* Final arquivo texto dois caracter CR + LF */
#define MODO_RO   "rt"
#define MODO_WO   "wt"
#define MODO_AO   "at"
#define MODO_AR   "a+t"
#define MODO_RW   "r+t"
#define MODO_WR   "w+t"
#define MODO_BRO  "rb"
#define MODO_BWO  "wb"
#define MODO_BAO  "ab"
#define MODO_BAR  "a+b"
#define MODO_BRW  "r+b"
#define MODO_BWR  "w+b"
#define DIRSEP    '\\'         /*** Separador de Diretorios ***/
#define TERM_DEF  "dos_ms.lbt" /*** Terminal  Default ***/
#define DIR_DEF   "."          /*** Diretorio Default ***/

#else

#define INCR_TEXTO  1  /* Final de arquivo texto apenas 1 caracter LF */
#define MODO_RO   "r"
#define MODO_WO   "w"
#define MODO_AO   "a"
#define MODO_AR   "a+"
#define MODO_RW   "r+"
#define MODO_WR   "w+"
#define MODO_BRO  "r"
#define MODO_BWO  "w"
#define MODO_BAO  "a"
#define MODO_BAR  "a+"
#define MODO_BRW  "r+"
#define MODO_BWR  "w+"
#define DIRSEP    '/'          /*** Separador de Diretorios ***/
#define TERM_DEF  "ansi.lbt"   /*** Terminal  Default ***/
#define DIR_DEF   "."          /*** Diretorio Default ***/

#endif

/****************************** TYPE DEFINES ********************************/

/* defined types used in the window system */

typedef          char  SC;    /* Should be 1 bytes SIGNED   */
typedef unsigned char  UC;    /* Should be 1 bytes UNSIGNED */

typedef          short SS;    /* Should be 2 bytes SIGNED   */
typedef unsigned short US;    /* Should be 2 bytes UNSIGNED */

typedef          long  SL;    /* Should be 4 bytes SIGNED   */
typedef unsigned long  UL;    /* Should be 4 bytes UNSIGNED */

typedef        double  DB;    /* Should be 8 bytes SIGNED   */

typedef          int   SI;    /* Natural int para passagem de paramentros */
typedef unsigned int   UI;    /* Natural int para passagem de paramentros */

                              /* Se void nao existir troque para SS */
#define VOID     void
                              /* Se void nao existir troque para UC */
#define VOIDC    void


#ifdef  MSDOS
#define ST( tipo ) static tipo near
#else
#define ST( tipo ) static tipo
#endif


/************* INFORMATION USED TO PASS VARIABLE # OF ARGUEMENTS **************/

#ifndef MT_VARARGS

#ifdef STACK_CRESCENTE

/*** Stack Crescente ***/
#define inc_char  ( - sizeof( int ) )
#define inc_int   ( - sizeof( int ) )
#define inc_ptr   ( - sizeof( char * ) )
#define inc_long  ( - sizeof( long ) )
#define inc_float ( - sizeof( double ) )

#else

/*** Stack DeCrescente ***/
#define inc_char  ( sizeof( int ) )
#define inc_int   ( sizeof( int ) )
#define inc_ptr   ( sizeof( char * ) )
#define inc_long  ( sizeof( long ) )
#define inc_float ( sizeof( double ) )

#endif

#endif /* MT_VARARGS */


/******************************  WINDOW LIMITATIONS **************************/
/* initially set reletively conserving */

/* length & height of larges used terminal */
#define MAXLENG    132             /* limit 256 */
#define MAXHEIGHT  50              /* limit 256 */

/* maximum number of windows ever available */
#define MAXWIN 70               /* limit 127 */

/* maximum attribute number considered */
#define MAXATRIB  30             /* limit 256 */

/* maximum number of keyboard conversions */
#define MAXCONV 160             /* minimum 50, limit 256 */

/* maximum cursor style number considered */
#define MAXCURS 10                         /* limit 256 */

/* maximum graphic character number considered */
#define MAXGRAPH 127           /* minumum 15, limit 127 */

/* maximum sequences START/END graphic */
#define MAX_SE   2             /* minumum 1, limit 5 */

/* maximum bell number considered */
#define MAXBELLS 10           /* limit 256 */

#endif /* ! ERRO_DEFINE_ERRO */

