#define   GRP_PESQ          26

#define   MAX_HOT           70 /* se mudar mude lb2rdisp.c */

#define   VT_BRILHO         180
#define   STR_BRILHO        (VT_BRILHO * (10 + 1))
#define   VT_MAXIMO_PSQ    1000
#define   DEC_PSQ           200
#define   TAM_EVET          sizeof(SDINDEX)
/* #define   TAM_EVET          (sizeof(S_INDEX) - LB_KEYMAX ) */
#define   BRANCO_ID        "\"\""
#define   CHEIO_ID         "++"
#define   BASE_TODA        "**"


#define   PSQ_SLOT    ap_psqslot    /* definido por config */
#define   TAM_MAPA    200  /* devera ser definido por config */
#define   NUM_LISTAS   10  /* devera ser definido por config */

#define   INICIALIZA 'I'
#define   RODAPE     'R'
#define   PUSH       'P'
#define   LBFIM      'F'
#define   ABORTA     'A'

#define   FRENTE 'F'
#define   TRAZ   'T'
#define   NADA   'N'

#define   E           0
#define   OU          1
#define   XOU         2
#define   NOT_OP      3
#define   ADJ         4
#define   PROX        5
#define   NOCAMPO     6
#define   NOPARAG     7
#define   NAFRASE     8

#define   ADJ_B       10
#define   PROX_B      20


#define   ADJ1       (ADJ_B + 1)
#define   ADJ2       (ADJ_B + 2)
#define   ADJ3       (ADJ_B + 3)
#define   ADJ4       (ADJ_B + 4)
#define   ADJ5       (ADJ_B + 5)
#define   ADJ6       (ADJ_B + 6)
#define   ADJ7       (ADJ_B + 7)
#define   ADJ8       (ADJ_B + 8)
#define   ADJ9       (ADJ_B + 9)

#define   PROX1      (PROX_B + 1)
#define   PROX2      (PROX_B + 2)
#define   PROX3      (PROX_B + 3)
#define   PROX4      (PROX_B + 4)
#define   PROX5      (PROX_B + 5)
#define   PROX6      (PROX_B + 6)
#define   PROX7      (PROX_B + 7)
#define   PROX8      (PROX_B + 8)
#define   PROX9      (PROX_B + 9)

#define   QL_FONETICA   'f'
#define   QL_INICIO     'i'


#define   TW_PQ_T00        loc_str(GRP_PESQ, 1)
#define   TW_PQ_T99        loc_str(GRP_PESQ, 2)
#define   TW_PQ_T01        loc_str(GRP_PESQ, 3)
#define   TW_PQ_T02        loc_str(GRP_PESQ, 4)
#define   TW_PQ_T03        loc_str(GRP_PESQ, 5)
#define   TW_PQ_T04        loc_str(GRP_PESQ, 6)


#define   TW_PQ_T30        loc_str(GRP_PESQ, 7 )
#define   TW_PQ_T39        loc_str(GRP_PESQ, 8 )
#define   TW_PQ_T31        loc_str(GRP_PESQ, 9 )
#define   TW_PQ_T32        loc_str(GRP_PESQ, 10)
#define   TW_PQ_T33        loc_str(GRP_PESQ, 11)
#define   TW_PQ_T34        loc_str(GRP_PESQ, 12)


/* #define   C_PSQ_NRD        loc_str(GRP_PESQ, 13) */
/* #define   C_PSQ_TRD        loc_str(GRP_PESQ, 14) */
/* #define   C_DEL_TFUN       loc_str(GRP_PESQ, 15) */
/* #define   C_DEL_NFUN       loc_str(GRP_PESQ, 16) */
/* #define   M_PSQ_NAV        loc_str(GRP_PESQ, 17) */
#define   M_PSQ_WNAV       loc_str(GRP_PESQ, 18)






#define   M_PSQ_NOT   loc_str(GRP_PESQ, 19)
#define   M_PSQ_SEE   loc_str(GRP_PESQ, 20)
#define   M_PSQ_1RA   loc_str(GRP_PESQ, 21)
#define   M_PSQ_2RA   loc_str(GRP_PESQ, 22)
#define   M_PSQ_3RA   loc_str(GRP_PESQ, 23)
#define   M_PSQ_4RA   loc_str(GRP_PESQ, 24)
#define   M_PSQ_5RA   loc_str(GRP_PESQ, 25)

#define   M_PSQ_TB1   loc_str(GRP_PESQ, 26)
#define   M_PSQ_TB2   loc_str(GRP_PESQ, 27)


#define   C_PSQ_1WIN   loc_str(GRP_PESQ, 28)
#define   C_PSQ_OCOR   loc_str(GRP_PESQ, 29)
#define   C_PSQ_DOCS   loc_str(GRP_PESQ, 30)

#define   C_PSQ_RES    loc_str(GRP_PESQ, 31)
#define   C_PSQ_2WIN   loc_str(GRP_PESQ, 32)
#define   C_PSQ_1ROD   loc_str(GRP_PESQ, 34)
#define   C_PSQ_RDUP   loc_str(GRP_PESQ, 35)
#define   C_PSQ_ICOL   loc_str(GRP_PESQ, 36)
#define   C_PSQ_NREG   loc_str(GRP_PESQ, 37)
#define   C_PSQ_ALT    loc_str(GRP_PESQ, 38)
#define   C_PSQ_WROD   loc_str(GRP_PESQ, 39)
#define   C_PSQ_WTIT   loc_str(GRP_PESQ, 40)
#define   C_PSQ_WAIT   loc_str(GRP_PESQ, 41)
#define   C_PSQ_1ALIN  loc_str(GRP_PESQ, 44)
#define   C_PSQ_1BLIN  loc_str(GRP_PESQ, 45)
#define   C_PSQ_1CLIN  loc_str(GRP_PESQ, 46)




#define   E_PSQ_1EDT    loc_str(GRP_PESQ, 47)
#define   E_PSQ_2EDT    loc_str(GRP_PESQ, 501)
#define   E_PSQ_3EDT    loc_str(GRP_PESQ, 48)
#define   E_PSQ_1BR     loc_str(GRP_PESQ, 49)
#define   E_PSQ_2BR     loc_str(GRP_PESQ, 50)
#define   E_PSQ_INT     loc_str(GRP_PESQ, 51)
#define   E_PSQ_AST     loc_str(GRP_PESQ, 52)
#define   E_PSQ_LST     loc_str(GRP_PESQ, 53)
#define   E_PSQ_F10     loc_str(GRP_PESQ, 54)

#define   E_PSQ_1TD     loc_str(GRP_PESQ, 55)
#define   E_PSQ_DUP     loc_str(GRP_PESQ, 56)

#define   E_AE_01       loc_str(GRP_PESQ, 57)
#define   E_AE_02       loc_str(GRP_PESQ, 58)
#define   E_AE_03       loc_str(GRP_PESQ, 59)
#define   E_AE_04       loc_str(GRP_PESQ, 60)
#define   E_AE_05       loc_str(GRP_PESQ, 61)
#define   E_AE_06       loc_str(GRP_PESQ, 62)
#define   E_AE_07       loc_str(GRP_PESQ, 63)
#define   E_AE_08       loc_str(GRP_PESQ, 64)


#define   E_DATAPSQ     loc_str(GRP_PESQ, 65)
#define   E_DATAINV     loc_str(GRP_PESQ, 66)
#define   E_HORAPSQ     loc_str(GRP_PESQ, 67)
#define   E_HORAINV     loc_str(GRP_PESQ, 68)
#define   E_DECPSQ      loc_str(GRP_PESQ, 69)
#define   E_BRANPSQ     loc_str(GRP_PESQ, 70)
#define   E_STOPPSQ     loc_str(GRP_PESQ, 71)



#define   MN_PSQ_T00  loc_str(GRP_PESQ, 72)
#define   MN_PSQ_T99  loc_str(GRP_PESQ, 73)
#define   MN_PSQ_T01  loc_str(GRP_PESQ, 74)
#define   MN_PSQ_T02  loc_str(GRP_PESQ, 75)
#define   MN_PSQ_T03  loc_str(GRP_PESQ, 76)
#define   MN_PSQ_T04  loc_str(GRP_PESQ, 77)

#define   MN_PSQ_R01  loc_str(GRP_PESQ, 78)
#define   MN_PSQ_R02  loc_str(GRP_PESQ, 79)
#define   MN_PSQ_R03  loc_str(GRP_PESQ, 80)
#define   MN_PSQ_R04  loc_str(GRP_PESQ, 81)


#define   MN_PSQ_T21  loc_str(GRP_PESQ, 82)
#define   MN_PSQ_T22  loc_str(GRP_PESQ, 83)

#define   MN_PSQ_R21  loc_str(GRP_PESQ, 84)
#define   MN_PSQ_R22  loc_str(GRP_PESQ, 85)

#define   MN_PSQ_T05  loc_str(GRP_PESQ, 86)
#define   MN_PSQ_R05  loc_str(GRP_PESQ, 87)

#define   TW_PSQ_D00  loc_str(GRP_PESQ, 107)
#define   TW_PSQ_D99  loc_str(GRP_PESQ, 108)
#define   TW_PSQ_D01  loc_str(GRP_PESQ, 109)



#define   MN_CN_T00  loc_str(GRP_PESQ, 88 )
#define   MN_CN_T01  loc_str(GRP_PESQ, 89 )
#define   MN_CN_T02  loc_str(GRP_PESQ, 90 )
#define   MN_CN_T03  loc_str(GRP_PESQ, 91 )
#define   MN_CN_T04  loc_str(GRP_PESQ, 92 )
#define   MN_CN_T05  loc_str(GRP_PESQ, 93 )
#define   MN_CN_T06  loc_str(GRP_PESQ, 94 )
#define   MN_CN_T07  loc_str(GRP_PESQ, 95 )
#define   MN_CN_T08  loc_str(GRP_PESQ, 96 )
#define   MN_CN_T09  loc_str(GRP_PESQ, 97 )


#define   MN_CN_R01  loc_str(GRP_PESQ, 98 )
#define   MN_CN_R02  loc_str(GRP_PESQ, 99 )
#define   MN_CN_R03  loc_str(GRP_PESQ, 100)
#define   MN_CN_R04  loc_str(GRP_PESQ, 101)
#define   MN_CN_R05  loc_str(GRP_PESQ, 102)
#define   MN_CN_R06  loc_str(GRP_PESQ, 103)
#define   MN_CN_R07  loc_str(GRP_PESQ, 104)
#define   MN_CN_R08  loc_str(GRP_PESQ, 105)
#define   MN_CN_R09  loc_str(GRP_PESQ, 106)

#define   C_RF_01R   loc_str(GRP_PESQ, 110)
#define   C_RF_01N   loc_str(GRP_PESQ, 111)
#define   C_RF_02    loc_str(GRP_PESQ, 112)
#define   C_RF_03    loc_str(GRP_PESQ, 113)
#define   C_RF_04    loc_str(GRP_PESQ, 114)
#define   C_RF_05    loc_str(GRP_PESQ, 115)

#define   E_LOGFAULT loc_str(GRP_PESQ, 116)

#define   E_PSQEDIT01   loc_str(GRP_PESQ, 117)
#define   E_CPINTER     loc_str(GRP_PESQ, 118)

#define   M_TEMMAIS     loc_str(GRP_PESQ, 119)


#define   H_LOGFAULT "LOGPSQF"
#define   H_CPINTER  "CPPSQINT"

#define   E_FONEPSQ  loc_str(GRP_PESQ, 502)

/*---------------------------------------------------------------------*/
/*  Controle de pesquisa                                               */
/*---------------------------------------------------------------------*/
typedef struct {
   TEXT flag;       /* nivel de interpretacao (grau) */
   COUNT n_buf;     /* numero do buffer sendo utilizado */
   COUNT it_bloco;  /* ultimo bloco acessado no vetor */
   COUNT it_item;   /* ultimo item acessado no bloco */
   COUNT u_bl_disco;/* ultimo bloco gravado em disco ( maoir ordem ) */
   LONG tot_ocor;   /* numero de ocorrencias do argumemto na base */
   LONG tot_campo;  /* numero de ocorrencias do argumemto no campo */
   LONG tot_doc;    /* numero de documentos que contem o argumento */
   LONG tot_vet;    /* numero de elementos SDINDEX nesta lista */
   FILE *file_cp;    /* Handle para File de campo tipo FLAG */
   SDINDEX sdi_lista;
} Psq_lista;


#ifdef MODO_DEF_PSQ
   COUNT  lista_psq_glb;
   Psq_lista  *vet_lista;
   COUNT  hot_key;
   TEXT   hot_palavra[MAX_HOT + 1];
   COUNT  sele_pesq;
   COUNT  psq_metacarac;
   COUNT  psq_qualificador;
   COUNT  psq_qtdfonetica;
   COUNT  psq_brqtd;
   TEXT **pt_brilhar  = NULL;
   TEXT  *str_brilhar = NULL;
#else
   extern COUNT  lista_psq_glb;
   extern Psq_lista  *vet_lista;
   extern COUNT  hot_key;
   extern TEXT   hot_palavra[ ];
   extern COUNT  sele_pesq;
   extern COUNT  psq_metacarac;
   extern COUNT  psq_qualificador;
   extern COUNT  psq_qtdfonetica;
   extern COUNT  psq_brqtd;
   extern TEXT **pt_brilhar;
   extern TEXT  *str_brilhar;
#endif



#ifdef MSDOS
      COUNT  tipo_argumento(TEXT *, COUNT);
      COUNT  psq_interativa( WIN *, WIN *, WIN *, COUNT );
      COUNT  psq_tabela( WIN *, WIN *, Tab *);
      SDINDEX *load_sdi(COUNT, LONG);

      VOID     add_sdi(COUNT, SDINDEX *);
      SDINDEX *prim_sdi(COUNT);
      SDINDEX *last_sdi(COUNT);
      SDINDEX *prox_sdi(COUNT);
      SDINDEX *back_sdi(COUNT);

      COUNT  exec_interativa( WIN * );
      COUNT  exec_tabela( COUNT, TEXT *);
      COUNT  carregar( TEXT *, TEXT *);
      COUNT  combina ( COUNT, COUNT, COUNT );
      VOID   conta_docs( COUNT );

      COUNT visualiza( COUNT, COUNT );
      COUNT push_brilha(COUNT, TEXT *);
      COUNT aloc_brilha(COUNT);

      COUNT push_palavra(COUNT, TEXT *);

      LONG e( COUNT, COUNT, COUNT );
      LONG ou( COUNT, COUNT, COUNT, COUNT );
      LONG not( COUNT, COUNT, COUNT );
      LONG nocampo( COUNT, COUNT, COUNT, COUNT, COUNT, COUNT);
      LONG adj_prox( COUNT, COUNT, COUNT, COUNT, COUNT);
#else
      COUNT  tipo_argumento( );
      COUNT  psq_interativa( );
      COUNT  psq_tabela( );
      SDINDEX *load_sdi( );

      VOID     add_sdi( );
      SDINDEX *prim_sdi( );
      SDINDEX *last_sdi( );
      SDINDEX *prox_sdi( );
      SDINDEX *back_sdi( );

      COUNT  exec_interativa( );
      COUNT  exec_tabela( );
      COUNT  carregar( );
      COUNT  combina ( );
      VOID   conta_docs( );

      COUNT visualiza( );
      COUNT push_brilha( );
      COUNT aloc_brilha( );

      COUNT push_palavra( );

      LONG e( );
      LONG ou( );
      LONG not( );
      LONG nocampo( );
      LONG adj_prox( );
#endif


