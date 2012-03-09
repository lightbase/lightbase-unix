
#ifdef  VM_TEST

#define MSDOS

typedef          char  SC;
typedef unsigned char  UC;
typedef          short SS;
typedef unsigned short US;
typedef          long  SL;
typedef unsigned long  UL;
typedef        double  DB;
typedef          int   SI;
typedef unsigned int   UI;
#define VOID     void
#define VOIDC    void

#define DIRSEP   '\\'
#define MODO_BWR "w+b"

#else

#include  "window.h"

#endif /* VM_TEST */

#define   huge /* Nada */


/***************************************************************************/
/* Tipos e limites */
/***************************************************************************/
typedef US VM_TAM;  /* Limita TAMANHO da maior PAGINA e BLOCO */
typedef US VM_HND;  /* Limita QUANT. de HANDLES */
typedef UC VM_PGN;  /* Limita QUANT. de PAGINAS */
typedef UC VM_RES;  /* RESERVA de ocupacao e SINAL de Liberado no bloco */
typedef UC VM_SEN;  /* Tipo da senha - Ver BL_SENHA */


/***************************************************************************/
/* Variaveis e tipos de erros */
/***************************************************************************/

#define RET_ER( erro ) {  VM_erro = (erro);   return 0;  }
#define VM_ER1  1  /* pg_alloc() Nao conseguio alocar pagina em memoria */
#define VM_ER2  2  /* pg_free() Pagina nao esta em memoria */
#define VM_ER3  3  /* pg_maior() Bloco nao Inicializado */
#define VM_ER4  4  /* pg_maior() Bloco nao esta LIVRE */

#define VM_ER6  6  /* bl_livre() Proximo bloco nao tem senha */
#define VM_ER7  7  /* bl_livre() Calculo quant. de blocos em uma pagina */
#define VM_ER8  8  /* bl_alloc() Nao pode alocar bloco de tamanho ZERO */
#define VM_ER9  9  /* bl_alloc() Pagina nao esta' em memoria */
#define VM_ER10 10 /* bl_alloc() Nao tem espaco na pagina */
#define VM_ER11 11 /* bl_alloc() Bloco nao inicializado */
#define VM_ER12 12 /* bl_alloc() Bloco com espaco nao foi localizado */

#define VM_ER14 14 /* bl_free() Pagina nao esta' em memoria */
#define VM_ER15 15 /* bl_free() Bloco nao inicializado */
#define VM_ER16 16 /* bl_free() Bloco ja esta liberado */
#define VM_ER17 17 /* bl_realloc() Pagina nao esta' em memoria */
#define VM_ER18 18 /* bl_realloc() Bloco fornecido nao inicializado */
#define VM_ER19 19 /* bl_realloc() Bloco posterior nao inicializado */
#define VM_ER20 20 /* bl_realloc() Bloco esta liberado */
#define VM_ER21 21 /* bl_realloc() Pagina lotado, nao consegue realocar */
#define VM_ER22 22 /* bl_realloc() Pagina lotada, nao consegue realocar */


#define VM_ER25 25 /* hd_alloc() Erro de Memoria Grupo nao nicializado */
#define VM_ER26 26 /* hd_alloc() Grupo indica handle livre mas nao tem */
#define VM_ER27 27 /* hd_alloc() Nao existe mais handle livres */
#define VM_ER28 28 /* hd_free() Handle maior que GR_QTD */


#define VM_ER31 31 /* hd_free() GROUP->END nao esta alocado */
#define VM_ER32 32 /* hd_get() Handle maior que GR_QTD */

#define VM_ER34 34 /* hd_get() GROUP->END nao esta alocado */
#define VM_ER35 35 /* hd_set() Handle maior que GR_QTD */

#define VM_ER37 37 /* hd_set() GROUP->END nao esta alocado */
#define VM_ER38 38 /* vm_pgdisp() Nenhuma pagina com area disponivel */
#define VM_ER39 39 /* vm_alloc() Area > BL_MAX */
#define VM_ER40 40 /* vm_pgslot() Interno, Slot->mem != Page->mem */
#define VM_ER41 41 /* vm_pgslot() Pagina esta em memoria mas nao nos SLOTs */

#define VM_ER43 43 /* sw_start() Nao consegue nome arquivo de SWAP */
#define VM_ER44 44 /* sw_start() Nome de arquivo temporario > SW_FNAME */
#define VM_ER45 45 /* sw_start() Nao consegue abrir arquivo de SWAP */
#define VM_ER46 46 /* sw_out() Pagina nao esta em MEMORIA */
#define VM_ER47 47 /* sw_out() ERRO FATAL DE GRAVACAO NO ARQUIVO DE SWAP */
#define VM_ER48 48 /* sw_in() SLOT->MEM == NULL */
#define VM_ER49 49 /* sw_in() ERRO FATAL DE LEITURA NO ARQUIVO DE SWAP */
#define VM_ER50 50 /* vm_deref() A PAGINA esta' desabilitada.       */
#define VM_ER51 51 /* vm_deref() O BLOCO nao confere SENHA. */
#define VM_ER52 52 /* vm_deref() O BLOCO esta' LIVRE.       */
#define VM_ER53 53 /* vm_deref() O HANDLE esta' LIVRE.       */
#define VM_ER54 54 /* vm_realloc() Area > BL_MAX. */
#define VM_ER55 55 /* vm_realloc() Pagina esta' desabilitada */
#define VM_ER56 56 /* bl_tamreal() Pagina nao esta' em memoria */
#define VM_ER57 57 /* bl_tamreal() Bloco nao inicializado */
#define VM_ER58 58 /* bl_tamreal() Bloco ja esta liberado */
#define VM_ER59 59 /* hd_free() Handle == ZERO */
#define VM_ER60 60 /* hd_get() Handle == ZERO */
#define VM_ER61 61 /* hd_set() Handle == ZERO */
#define VM_ER62 62 /* sw_in() Nao sabemos aonde esta' a PAGINA */
#define VM_ER63 63 /* vm_free() PAGINA desabilitada */
#define VM_ER64 64 /* vm_free() HANDLE ja' livre */
#define VM_ER65 65 /* hd_free() HANDLE ja' livre */
#define VM_ER66 66 /* pg_livre() Bloco nao inicializado */
#define VM_ER67 67 /* pg_livre() Bloco nao esta LIVRE */
#define VM_ER68 68 /* bl_alloc() Bloco nao esta LIVRE */
#define VM_ER69 69 /* pg_retira() Bloco nao inicializado */
#define VM_ER70 70 /* pg_retira() Bloco nao esta LIVRE */
#define VM_ER71 71 /* pg_retira() Bloco nao encontrado */

/***************************************************************************/
/* Defines e estruturas referentes a PAGINA */
/* PAGINA contem os BLOCOS e podem estar em qualquer MIDIA */
/* POREM devem estar na MEMORIA para serem acessados os blocos */
/***************************************************************************/
#define PG_QTD    250    /* Quantidade maxima de paginas */
#define PG_TAM    16500  /* Tamanho da pagina */

#define PG_DES           0     /* Pagina Desabilitada, deve ser ZERO */
#define PG_MEM           1     /* Pagina em Memoria */
#define PG_DSK           2     /* Pagina em Disco */
#define PG_XMS           3     /* Pagina em XMS */
#define PG_LOC( L )      ((UC)(L) & 7)
#define DESABILIT( S )   ((US)PG_LOC( S ) == PG_DES)
#define MEMORIA( S )     ((US)PG_LOC( S ) == PG_MEM)
#define DISCO( S )       ((US)PG_LOC( S ) == PG_DSK)
#define XMS( S )         ((US)PG_LOC( S ) == PG_XMS)

struct  S_Page
{
   VM_TAM  maior;   /* Maior area disponivel na pagina */
   VM_TAM  livre;   /* Lista invertida dos Blocos livres */
   SC huge *mem;    /* Endereco de memoria */
                    /* Offset no arquivo de swap */
                    /* Handle de XMS, EMM etc. */
   UC      loc;     /* Os TRES bit menos significativo indica onde se ... */
                    /* ... esta a pagina PG_DES, PG_MEM, PG_DSK, PG_XMS. */
};
typedef struct S_Page  Page;


/***************************************************************************/
/* Defines e estruturas referentes a BLOCOS */
/* BLOCO sao as proprias areas alocadas na PGINA */
/***************************************************************************/
#define BL_MAX          (PG_TAM - BL_SIZE) /* Tamanho da pagina */
#define BL_SENHA        0xA5               /* 0xA5C3L -> US */
#define BL_LIVRE        0                  /* Bloco disponivel */
#define BL_ALLOC        1                  /* Bloco ocupado */
#define BL_STSTAT( S )  ( (S) << 7 )
#define BL_STAT( S )    (((S) >> 7 ) & 1)
#define BL_RES( S )     ( (S) & 127)
#define DISPONIVEL( S ) (BL_STAT( S ) == BL_LIVRE)
#define BL_SIZE         (sizeof( Bloco ))
#define BL_ALIGN( N )   (((((N) - 1) / BL_SIZE) * BL_SIZE) + BL_SIZE)
#define BL_MIN          10  /* Bloco Minimo   */
#define RES_MIN         10  /* Reserva Minima */
#define RES_MAX         30  /* Reserva Maxima */
                            /* RES_MAX > BL_SIZE + BL_MIN + RES_MIN */
                            /* RES_MAX + BL_SIZE + BL_MIN + RES_MIN < 127 */

struct S_Bloco
{
   VM_SEN  senha;   /* Assinatura -> BL_SENHA */
   VM_TAM  tam;     /* Tamanho total do Bloco */
   VM_RES  status;  /* O bit mais significativo indica se ... */
                    /* ... o bloco esta BL_LIVRE ou BL_ALLOC. */
                    /* Os outros bits  e' diferenca entre ... */
                    /* ... area total e ocupada. RESERVA !    */
};
typedef struct S_Bloco Bloco;


/***************************************************************************/
/* Defines e estruturas referentes a HANDLE */
/* HANDLE sao alias do BLOCO dentro do VM */
/* O programa pede uma area e recebe um HANDLE que devera ser ... */
/* ... fornecido ao VM toda vez que se quizer ter acesso ao BLOCO. */
/***************************************************************************/
#define HD_SIZE   (sizeof( Handle ))

struct S_Handle
{
   VM_PGN  page;   /* Em qual pagina esta o BLOCO referente a este HANDLE */
                   /* ZERO indica HANDLE LIVRE */
   VM_TAM  off;    /* Qual o OFFSET deste BLOCO na PAGINA */
                   /* Se Handle estiver Livre - Indica o Proximo Livre */
                   /* ZERO indica fim da lista */
};
typedef struct S_Handle Handle;


/***************************************************************************/
/* Defines e estruturas referentes a GROUP */
/* GROUP e' um conjunto de GR_HND HANDLEs */
/***************************************************************************/
#define   GR_QTD   64     /* Quantidade de GROUPs */
#define   GR_HND   1000   /* Quantidade de Handles em um GROUP */

struct S_Group
{
   VM_HND  livre;  /* Lista dos Handles Livres */
                   /* ZERO indica sem Handles Livres */
   VM_HND  qtd;    /* Quant. de Handles Livres */
                   /* Flag para liberar GROUP */
   SC huge *mem;   /* Endereco em memoria */
};
typedef struct S_Group Group;


/***************************************************************************/
/* Defines e estruturas referentes a SLOTs */
/* SLOTs sao areas de memorias onse sao habilitadas as PAGINAS. */
/* Administram LRU - Least Recently Used */
/***************************************************************************/
#define   SL_QTD   30      /* Quantidade Maxima de SLOTs */

struct S_Slot
{
   SC huge *mem;   /* Endereco em memoria */
   VM_PGN  page;   /* Qual PAGINA esta neste SLOT */
   VM_PGN  lru;    /* Entre 0 e SL_QTD */
                   /* 0      -> SLOT esta' vazio */
                   /* 1      -> indica Most  Recently Used */
                   /* SL_QTD -> indica Least Recently Used */
};
typedef struct S_Slot Slot;


/***************************************************************************/
/* Variaveis */
/***************************************************************************/
#ifdef    VM_VARS
#define   VM_extern  /* Nada */
#else
#define   VM_extern  extern
#endif

VM_extern SC      VM_erro;
VM_extern FILE   *VM_file;              /* Nome do arquivo para Swap */
VM_extern VM_PGN  VM_slqtd;             /* Quant de Slots disponiveis */
VM_extern Group   VM_group[ GR_QTD ];
VM_extern Slot    VM_slot[  SL_QTD ];
VM_extern Page    VM_page[  PG_QTD ];

/*** As proximas variaveis sao realmente "extern" sao pre-inicializadas ***/
extern    UC      VM_maxslot;    /* Quant. Maxima de SLOTs */
extern    UC      VM_minslot;    /* Quant. Minima de SLOTs */
extern    UC      VM_mingroup;   /* Quant. Minima de HANDLEs */
extern    VM_RES  VM_res;        /* Reserva Default */
extern    SL      VM_coreleft;   /* Reserva de memoria para alocar SLOT */
extern    SC     *VM_dir;        /* Diretorio do arquivo do VM */

/***************************************************************************/
/* Prototipos de funcoes */
/***************************************************************************/

/* VM.C */
SC      *vm_deref( VM_HND handle, SS lock );
VM_HND  vm_unlock( VM_HND handle );
VM_HND  vm_start( void );
VM_HND  vm_end( void );
VM_HND  vm_pgdisp( VM_TAM tam );
VM_HND  vm_pgslot( VM_PGN page );
VM_HND  vm_alloc( VM_TAM tam );
VM_HND  vm_free( VM_HND handle );
VM_HND  vm_realloc( VM_HND handle, VM_TAM tam );

/* VM_PAGE.C */
VM_PGN  pg_start( void );
VM_PGN  pg_end( void );
VM_PGN  pg_alloc( Page *, SC huge *mem );
VM_PGN  pg_free( Page * );
VM_TAM  pg_maior( Page * );
VM_TAM  pg_ultimo( Page *, Bloco * );
VM_TAM  pg_retira( Page *, Bloco *retira );
VM_TAM  pg_livre( Page *, Bloco *livre );
VM_TAM  bl_alloc( Page *, VM_TAM tam, VM_RES reserva );
VM_TAM  bl_free( Page *, VM_TAM off );
VM_TAM  bl_realloc( Page *, VM_TAM off, VM_TAM tam );
VM_TAM  bl_tamreal( Page *, VM_TAM off, VM_TAM *tam );

/* VM_GROUP.C */
VM_HND  gr_start( void );
VM_HND  gr_end( void );
VM_HND  gr_alloc( Group *G );
VM_HND  gr_free( Group *G );
VM_HND  hd_alloc( void );
VM_HND  hd_free( VM_HND handle );
VM_HND  hd_get( VM_HND handle, VM_PGN *page, VM_TAM *off );
VM_HND  hd_set( VM_HND handle, VM_PGN page, VM_TAM off );

/* SLOT.C */
VM_PGN  sl_start( void );
VM_PGN  sl_end( void );
VM_PGN  sl_mru( VM_PGN slot );
VM_PGN  sl_lru( VM_PGN slot );

/* SWAP.C */
VM_TAM sw_start( void );
VM_TAM sw_end( void );
VM_TAM sw_out( Slot *slot );
VM_TAM sw_in( Slot *slot, Page *page );

