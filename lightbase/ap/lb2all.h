#include  <stdio.h>
#include  <stdlib.h>
#include  <setjmp.h>

#ifdef MSDOS
#define   GET_LONG(pl)     (LONG)(*(LONG *)pl)
#define   PUT_LONG(p, l)   memcpy((TEXT *) p, (TEXT *) &l, sizeof(LONG))
#define   GET_LONG4(pl)    (LONG)(*(LONG *)pl)
#define   PUT_LONG4(p, l)  memcpy((TEXT *) p, (TEXT *) &l, sizeof(LONG))
#endif

#ifdef MSDOS
#  define   HD_MEMO  <mem.h>
#  define   HD_DIR   <dir.h>
#  define   HD_IO    <io.h>
#  ifndef PESSOAL
#     define COM_PROTECAO
#  endif
#else
#  ifdef    VAXVMS
#    define HD_MEMO  <string.h>
#  else
#    define HD_MEMO  <memory.h>
#  endif
#  define   HD_DIR   "nada.h"
#  define   HD_IO    "nada.h"
#endif

#include  "lb2rtime.h"
#include  "lb2lite.h"

#ifdef VAXVMS
#	include  "window.h"
#	include  "wstring.h"
#	include  "ethead.h"
#else
#	include  "../mt/window.h"
#	include  "../mt/wstring.h"
#	include  "../mt/ethead.h"
#endif

#ifndef TIPOS_LB
#	include  "lb_port.h"
#endif

#define   VERSAO_CORRENTE  620
#define   VERSAO_NOCRIPT   601

#define   F__GLB     /* Token para funcoes GLOBAIS */
#define   F__LOC     /* Token para funcoes LOCAIS  */


#ifndef OK
#  define OK 0
#endif

#ifdef MONO_USUARIO
#  define   LOCK_RECURSO(x,y,z)  0
#  define   BEG_TRANS(x,y,z)     0
#  define   END_TRANS(x)         0
#endif




#ifdef PATH_MAX
#  undef PATH_MAX
#endif

#define   PATH_MAX  (TD + TB + 6)

#define   TD   50             /* tamanho padrao de uma descricao       */
#define   TE   15             /* tamanho padrao de uma especificacao   */
#define   TM   80             /* tamanho padrao de uma mascara         */
#define   TB   8              /* tamanho padrao de ident da base       */


#define   MAX_CAMPOS      125
#define   MAX_PROTECOES   30
#define   MAX_VISTAS      20
#define   MAX_REL         MAX_VISTAS  /* devem ser iguais */
#define   MAX_ELEM        200
#define   MAX_EXC         15
#define   MAX_BASES      100
#define   MAX_IDIOMAS     4
#define   MAX_PRINTS      20

#define   SLOT       1024   /* incremento de crescimento dos arquivos */
#define   MAX_SELE   2
#define   LB_KEYMAX    30  /* se mudar observar ctoptn.h */
#define   LB_KEYCT     64  /* Chave maxima do Metodo de Acesso */
#define   LB_MAXUNICA  (64 - 1 - 1 - 4 )  /* MAX - NULL - CAMPO - REG */



#define   POS_INDEX   '1'
#define   NOW_INDEX   '2'
#define   NO_INDEX    '3'

#define   BASE_EDIT     0        /* indice da base de edicao */



#ifdef MSDOS
#	define   XNBASE      "XXXXXXXX"
#  define   STR_BARRA   "\\"
#  define   BARRA       '\\'
#  define   OUTRA_BARRA '/'
#else
#	define   XNBASE      "XXXXXXXX"
#  define   STR_BARRA   "/"
#  define   BARRA       '/'
#  define   OUTRA_BARRA '\\'
#endif


#define   LM   1    /* tipos de janelas */
#define   JP   2
#define   JS   3
#define   MP   4
#define   MS   5
#define   JE   6


#define INCLUSAO 	'I'
#define ALTERACAO 'A'
#define EXCLUSAO  'E'

/*---------------------------------------------------------------------*/
/*  OUTRAS Declaracoes                                                 */
/*---------------------------------------------------------------------*/
#ifdef ENTRA_TESTE_DATA
#  define   DEMO_VALIDA ( ap_nserie < 123L && data_demo_ok() )
#else
#  define   DEMO_VALIDA ( ap_nserie > -1L)
#endif

#define   CAI_FORA(nr) { if ( ap_nserie < 123 && cai_fora(nr) != (nr-1) ) ; }

#define AUTO_INCREMENTO    1
#define PESQUISA_RECURSIVA 1

