#include  "lb2maces.h"

#define   FILE_DATNO   0
#define   FILE_KEYNO   3

extern COUNT ct_maxuser, ct_ignoraupd, ct_updwarning, ct_semseguranca;
EXTERN COUNT uerr_cod;
extern TEXT ct_indkey[];
extern COUNT ct_lenkey;

#define   CON   stderr

#define   MAX_CHAVE 50
#define   TIPO_CHAVE 2
#define   DUPL_OK  0
#define   NO_ERROR 0

#define   GR_ID  "GR"
#define   US_ID  "US"
#define   D0_ID  "D0"
#define   DA_ID  "DA"
#define   D1_ID  "D1"
#define   D2_ID  "D2"


#undef GET_LONG
#undef PUT_LONG

#ifdef MSDOS
LONG GET_LONG(UTEXT *);
VOID PUT_LONG(UTEXT *, LONG);
#else
LONG GET_LONG( );
VOID PUT_LONG( );
#endif




#define   LB_KEYMAX   30



#define   D_EQLKEY    1
#define   D_GTEKEY    2
#define   D_NXTKEY    3
#define   D_PRVKEY    4
#define   D_FRSKEY    5
#define   D_LSTKEY    6
#define   D_GTKEY     7
#define   D_LTKEY     8
#define   D_LTEKEY    9


extern COUNT ct_maxuser, ct_ignoraupd, ct_updwarning, ct_semseguranca;
EXTERN COUNT uerr_cod;
extern TEXT ct_indkey[];
extern COUNT ct_lenkey;


typedef struct {
   COUNT totaliza;
   COUNT so_conta;
   COUNT delimita_key;
   COUNT minimo;
   COUNT maximo;
   COUNT forma_long;
   COUNT estrutura_alinhada;
   COUNT lista_pos;
   COUNT recup;
   COUNT cripto;
   LONG  posicao;
   LONG  start;
   COUNT modomanut;
} Param;


#ifdef MAIN
   Param param;
   TEXT  ap_chave_ret[100];
   COUNT calado = 0;
   COUNT off_reg, off_campo, off_parte, off_info;
#else
   extern COUNT calado;
   extern TEXT ap_chave_ret[100];
   extern Param param;
   extern COUNT off_reg, off_campo, off_parte, off_info;
#endif


#define   LOG_S(s)    if ( calado == 0 ) fprintf(stderr, s);
#define   LOG_SL(s,l)   if ( calado == 0 ) fprintf(stderr, s, l);
#define   LOG_SLL(s, l1, l2)   if ( calado == 0 ) fprintf(stderr, s, l1, l2);



#ifdef MSDOS
   VOID teste_ctree(VOID);
   POINTER grava_rec(COUNT, TEXT *);
   POINTER deleta_rec(COUNT, POINTER);
   VOID pesquisa(TEXT *);
   COUNT lista_ind(TEXT *, TEXT *);
   COUNT abre_indice(TEXT *, COUNT);
   VOID  fecha_indice(COUNT);
   VOID abre_arq(TEXT *, TEXT *, COUNT, COUNT, COUNT);
   VOID  fecha_arq(COUNT, COUNT);
   POINTER grava_rec(COUNT, TEXT *);
   POINTER  find_rec(COUNT, COUNT, TEXT *, TEXT *, TEXT);
   TEXT    *load_rec(COUNT, COUNT, TEXT *, TEXT *, VOID_T *, UCOUNT, TEXT);
   COUNT    add_ind (COUNT, COUNT, TEXT *, TEXT *, POINTER);
   POINTER  add_rec (COUNT, COUNT, TEXT *, TEXT *, VOID_T *, UCOUNT);
   LONG pega_proximo(LONG, FILE *);
   VOID strip_cr(TEXT *);
   VOID strip_br(TEXT *);
   COUNT permissao( TEXT *, TEXT *);
   SI lb_random(COUNT);
   VOID pega_long(UTEXT *, UTEXT *);
#else
   VOID teste_ctree( );
   POINTER grava_rec( );
   POINTER deleta_rec( );
   VOID pesquisa( );
   COUNT lista_ind( );
   COUNT abre_indice( );
   VOID  fecha_indice( );
   VOID abre_arq( );
   VOID  fecha_arq( );
   POINTER grava_rec( );
   POINTER  find_rec( );
   TEXT    *load_rec( );
   COUNT    add_ind ( );
   POINTER  add_rec ( );
   LONG pega_proximo( );
   VOID strip_cr( );
   VOID strip_br( );
   COUNT permissao( );
   SI lb_random( );
   VOID pega_long( );
#endif


#ifdef STR_PROPRIA
#define   strnicmp(s1, s2, tam)     w_strnicmp(s1, s2, tam)
#define   strncmp(s1, s2, tam)      w_strncmp(s1, s2, tam)
#endif
