#define   FOI_MODIFICADO( x)  (ap_winloc[x][TIPO] & 0x80) 
#define   SETA_MODIFICADO( x)  (ap_winloc[x][TIPO] |= 0x80) 

#define   DIR_PERSONA "persona"

#ifdef ULTRIX
#  define   MAX_PESQUISAS    16  /* para atender Camara dos Deputados */
#else
#  define   MAX_PESQUISAS    8
#endif

#define   TAM_LINPSQ       230  /* atencao se mudar mudar 230 em lb_main.c */
#define   TAM_PSQPROT      28
#define   TAB_MODI         1
#define   HIST_MODI        2
#define   TAB_FMT          "%2.2d %8ld %4.4s %s   %-200.200s"
#define   X_MSC_TAB        ap_tabpsq[MAX_PESQUISAS + 1]


#ifdef MODO_DEFINICAO
TEXT     ap_tabctl[MAX_PESQUISAS];
LONG     ap_taboff[MAX_PESQUISAS];
TEXT    *ap_tabpsq[MAX_PESQUISAS+2];
#else   
extern   TEXT  ap_tabctl[];
extern   LONG  ap_taboff[];
extern   TEXT *ap_tabpsq[];
#endif
