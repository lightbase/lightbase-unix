#ifdef MSDOS
	COUNT BEG_TRANS(COUNT, LONG, COUNT);
	COUNT END_TRANS(COUNT);
#else
	COUNT BEG_TRANS( );
	COUNT END_TRANS( );
#endif

#define   TR_CARREGAR      1     /* carregar uma lista de argumentos */
#define   TR_NEWRECORD     2     /* cria novo registro */
#define   TR_DELRECORD     3     /* elimina registro */

#define   TR_READRECORD    4     /* le registro */
#define   TR_WRRECORD      5     /* grava registro */

#define   TR_FREERECORD    6     /* Informa que registro nao e mais o corrente */

#define   TR_FLUSHIND      7     /* Flush na indexacao */

#define   TR_PSQUNICA      8

#define   TR_PALINDEX      9
#define   TR_PALDINDEX     10
#define   TR_SALVATXT      11


/*
 * Estados possiveis para cada recurso.
 */
#define    LIVRE		      0
#define    COMPARTILHADO	1
#define    EXCLUSIVO		   2
#define    TRAVADO		   4

#ifndef OK
#define    OK			      0
#endif

/*
 * Tipos de acao a ser tomada por LOCK_REC( ).
*/

#define    LCK_EXCLUSIVO	   'E'
#define    LCK_COMPARTILHADO  'C'
#define    LCK_LIBERA		   'L'
#define    LCK_TRAVA		      'T'
#define    LCK_DESTRAVA		   'D'
#define    LCK_STATUS         'S'

#define   OBRIGATORIO  1
#define   OPCIONAL     0

#define   TR_TAM  ( sizeof(Tr_cabec) + ((ap_max_users + 1) * sizeof(Tr_usu)))


typedef struct {
   LONG  trus_reg;      /* registro corrente do usuario */
   TEXT  trus_nome[15]; /* nome do usuario logado */
   UTEXT trus_zero;     /* fim do nome sempre NULL - Uso para alinhar word */
   TEXT  trus_modo;     /* modo utilizacao registro. E-exclusivo C-compart.*/
   UTEXT trus_fut1;     /* futuro 1 --- preocupacao com alinhamento de Words */
   UTEXT trus_fut2;     /* futuro 2 --- preocupacao com alinhamento de Words */
   UTEXT trus_fut3;     /* futuro 3 --- preocupacao com alinhamento de Words */
   LONG  trus_pid;      /* Ident processo p/ evitar conflito de nomes. Resto de time() */
} Tr_usu;

typedef struct {
   LONG  tr_ident;     /* Identificacao do arquivo de lock inicial */
   UTEXT tr_status;    /* status da base   T-travada   E-Modo exclusivo  0-Livre  C-Compart */
   UTEXT tr_logados;   /* num usuarios com a base aberta */
   UTEXT tr_pesquisa;  /* num usuarios efetuando pesquisa na base */
   TEXT  tr_nome[15];  /* nome em caso de travada/exclusiva */
   TEXT  tr_zero;      /* fim do nome sempre NULL - Uso para alinhar word */
   TEXT  tr_aguarde;   /* Aguardando vez para atualizar (gravar) */
   LONG  tr_pid;       /* Identificacao do processo em uso exclusivo */
   LONG  tr_time;      /* Time do Lock */
} Tr_cabec;




#ifdef TR_DEFINE
   Tr_cabec   *ap_trcabec;
   Tr_usu     *ap_trusu;
   COUNT       ap_trfile;
   COUNT       ap_trind;  /* indice do usuario no vetor de usuarios */
   LONG        ap_trpid;  /* pid do usuario na sessao LIGHT */
   TEXT        usu_usando[TE+1];
   LONG        ap_trident;  /* Numero magico do arquivo de controle */
#else
   extern  Tr_cabec   *ap_trcabec;
   extern  Tr_usu     *ap_trusu;
   extern  COUNT       ap_trfile;
   extern  COUNT       ap_trind;
   extern  LONG        ap_trpid;
   extern  TEXT        usu_usando[];
   extern  LONG        ap_trident;
#endif

