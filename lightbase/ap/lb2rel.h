/*---------- lb2rel.h ----------*/
#define   QB_TODOS     0    /* Imprime TODOS os campo de ordenacao Sempre */
#define   QB_ONLY      1    /* Somente imprime campos que quebraram */
#define   QB_IFQUEBRA  2    /* Imprime todos os campos somente se quebrar */

#define   MAX_TOP    20
#define   MAX_BOT    1

#define   SIZE_NR      8    /* numero de posicoes do numero do registro */

/*==========================================================================*/
/*                                                                          */
/*        Mensagens para numdebug - RELATORIOS                              */
/*                                                                          */
/*                                                                          */
/*   2001 -   inic_printer() - device invalido                              */
/*   2002 -   term_printer() - device invalido                              */
/*   2003 -   imp_char()     - device invalido                              */
/*   2004 -   carac_etiq()   - gravacao deve ser efetivada                  */
/*   2005 -                    acao em ordena iqual a zero                  */
/*   2006 -   poe_mat()      - ERRO: POS > MATRIZ                           */
/*   2007 -   carac_imp()    _ gravacao deve ser evetivada                  */
/*                                                                          */
/*==========================================================================*/

#define   GRP_IMPREL     29

#define   LF_13E         "1 3 E"       /* funcoes utilizadas em ed_read */
#define   MASC_NUMPAGE   "###"
#define   M_ESCRITA      "w"
#define   M_LEITURA      "r"
#define   NU_PAGINICIAL  "1"
#define   OP_CONFIRMA    "SN"


#define   H_CFCAN    "CF_RLCAN"    /* cancela impressao do relatorio */
#define   H_IMPREL   "E_IMP_REL"   /* erro ### na impressao do relatorio */
#define   H_OPDRV    "DRVIMPRES"   /* Driver de impressora nao acessivel ou com problemas */

#define   M_EMI_ORDENA        loc_str( GRP_IMPREL, 6 )
#define   M_EMI_IMPREL        loc_str( GRP_IMPREL, 7 )
#define   M_EMI_MENS          loc_str( GRP_IMPREL, 8 )


#define   TW_RL_D00       loc_str(GRP_IMPREL, 9)
#define   TW_RL_D99       loc_str(GRP_IMPREL, 10)
#define   TW_RL_D01       loc_str(GRP_IMPREL, 11)
#define   TW_RL_D02       loc_str(GRP_IMPREL, 12)


#define   E_OPDRV     loc_str(GRP_IMPREL, 48)

#define   E_IMPREL    loc_str(GRP_IMPREL, 501)



#define   QUAL_CARTA    0
#define   COMPRIMIDO    1
#define   ENFATIZADO    2
#define   ITALICO       3
#define   NORMAL        4
#define   SUBLINHADO    5

#define   COMPARA       'C'

#define   ARQ_SPOOL   "SLBXXXXXX"    /* nome arquivo para spooler */

#define   LI 1
#define   CO 43

#define   TM_BUF_ORD     ( ap_psqslot * 30 )

#define   DISPLAY    'D'
#define   COMANDO_REL    'Z'
#define   JA_TRATOU   1
#define   NAO_TRATOU  0
#define   FINALIZA   'F'
#define   SIM        'S'
#define   BRANCO     ' '

#define   FORMATADO  'F'
#define   COLUNADO   'C'
#define   ETIQUETA   'E'

#define   TOTALIZA    'T'
#define   INICIALIZA  '1'
#define   PUSH        '2'
#define   ORDENA      '3'
#define   POP         '4'

#define   CARAC_IMP_WIN  12, 3, (size_y - 7), (size_x - 4),  0,  0

#define PR(l,c)   ((l * prel->rel_colunas) + c)


#define   PRIMEIRO '1'
#define   ULTIMO   'U'
#define   LBSOBE     'S'
#define   LBDESCE    'D'
#define   TELA_ANT 'A'
#define   TELA_POS 'P'
#define   FICA     'F'
#define   RE_LER   'R'

#define   TEXTO   'T'
#define   BORDA   'B'
#define   NOMES   'D'
#define   CAMPO_G 'G'  /*** CAMPO GENERICO - ENTRADA ou SAIDA ***/
#define   CAMPO_S 'S'
#define   CAMPO_E 'E'
#define   LIN_HOR 'H'
#define   LIN_VER 'L'
#define   NO_PAGINAS   1


#define  I_MAXATRIB   8




/*-------------------------------------------------------------------*/
/*          estruturas                                               */
/*-------------------------------------------------------------------*/

struct i_start_end
{
   UC   *start;  /* Sequencia que abre semi-graficos e caracters especiais */
   UC   *end;    /* Sequencia que fecha */
};

struct i_struct
{
   UC   *ary;    /* Sequencia a ser enviada */
   UC   type;    /* Tipo de sequencia que inicia e termina tal caracter */
};

struct tot_col {
   SS n_item;
   DB desv_1, desv_2, desv_3, desv_total;
   DB soma_1, soma_2, soma_3, soma_total;
   SL qtd_1, qtd_2, qtd_3, qtd_total;
};

struct i_atr  {
   UC *at_on;
   UC *at_off;
};

struct car_imp {
   SC driver[ TE+1 ];    /* nome do driver da impressora */
   SC destino[ TD+1 ];   /* nome do fisico do device */
   SC comando[ TD+1 ];   /* Comando a ser executado apos impressao do relatorio */
   UC escrava[7];        /* sequencia para ativar escrava */
   UC video[7];          /* sequencia para ativar video */
   SC pular_pagina;
   SC palavras_negrito;
};

typedef struct {
   TEXT   qb_offset;   /* Offset no buffer de ordenacao */
   TEXT   qb_tamanho;  /* Tamanho no buffer */
   COUNT  qb_ocorre;   /* Quantidade de ocorrencias do item atual */
   Campo *qb_campo;    /* Ponteiro para campo que a informacao pertence */
} Ger_quebra;

typedef struct {
   COUNT sup_linhas;
   COUNT ctl_quebra;
   COUNT replica_txt;
   COUNT linhas_def;
   COUNT nlin_cabec;
   COUNT nlin_rodape;
   COUNT no_display;
   LONG  first_record;
   LONG  last_record;
   TEXT *cabec[MAX_TOP+1];
   TEXT *rodape[MAX_BOT+1];
} Grel;

/*------------------------------------------------------------------------*/
/* inicio de prototipos                                                   */
/*------------------------------------------------------------------------*/
#ifdef MSDOS
   /* lb2rel0  ******/
   SS quebra_reg ( SS, SS, SS);
   SS col_inic_itens( SS );

   /* lb2rel1  ******/
   SS    imprime_pagina( VOID );
   SS    inicializa_pagina(SS);
   LONG  push_linha(COUNT, TEXT *);
   VOID  limpa_flag(RTela **, SS);
   VOID  poe_mat( SS, SS, SC *, SS);

   /* lb2rel3  ******/
   VOID  mont_vt_impr( SC * );
   SS    i_find_params( SS );
   VOID  inic_printer( VOID );
   VOID  term_printer( VOID );

   /* lb2rel4  ******/
   SS    trata_ordenacao( SS, SL ( * )( SS, SL ), WIN *);
   SS    ordena_campos( SS, SL ( * )( SS, SL ), SC * );
   SS    calc_max( RTela **, SS );
   SI    ord_itrel( SC **, SC** );
   VOID traduz_comandos( VOID );

   /* lb2rel5  ******/
   VOID imp_char( UC );
   VOID imp_puts( FILE *, UC * );
#else
   /* lb2rel0  ******/
   SS quebra_reg ( );
   SS col_inic_itens( );

   /* lb2rel1  ******/
   SS    imprime_pagina( );
   SS    inicializa_pagina( );
   LONG  push_linha( );
   VOID  limpa_flag( );
   VOID  poe_mat( );

   /* lb2rel3  ******/
   VOID  mont_vt_impr( );
   SS    i_find_params( );
   VOID  inic_printer( );
   VOID  term_printer( );

   /* lb2rel4  ******/
   SS    trata_ordenacao(  );
   SS    ordena_campos( );
   SS    calc_max( );
   SI    ord_itrel( );
   VOID traduz_comandos( );

   /* lb2rel5  ******/
   VOID imp_char( );
   VOID imp_puts( );
#endif



/*-------------------------------------------------------------------*/
/*     Globais ao modulo de relatorio                                */
/*-------------------------------------------------------------------*/
#ifdef MODO_DEF_REL
   SS      tipo_corrente = 0;
   UC      *i_inic, *i_final, *i_fdl, *i_fdp;
   SC      *driver;
   struct  i_start_end I_SE[ MAX_SE + 1 ]; /* Zero nao usa */
   struct  i_struct IMP[ MAXGRAPH + 1];
   struct  car_imp   c_impress;
   struct  i_atr i_atrib[ I_MAXATRIB ];
   Rel     *prel;
   FILE    *device = NULL;
   SC     **pl_br_rel;
   SS       qtd_plrel;

   SC *mat_pag;
   SS col_inicial_totais;
   SS offset , num_soma;
   SS ult_linha_util;
   UL prim_pagina, num_pagina, off_pagina, qtd_pagina;

   RTela *prt;
   struct tot_col *col_tot;

   Campo *vc[4], *pc_txt;
   Campo **vetcp;
   COUNT max_ord;
   COUNT ord_txt;
   COUNT off_txt;
   COUNT QQ1, QQ2, QQ3;
   COUNT nivel_qbr;
   TEXT *lin_sort;
   Ger_quebra qbr[4];
   Grel *grel; /* parametros de gerencia de impressao */
   TEXT DELIM[5];
#else
   extern FILE *device;
   extern UC   *i_inic, *i_final, *i_fdl, *i_fdp;
   extern SC   *driver;
   extern struct i_start_end I_SE[ ]; /* Zero nao usa */
   extern struct i_struct IMP[ ];
   extern struct i_atr i_atrib[ ];
   extern SS tipo_corrente;
   extern struct car_imp   c_impress;
   extern Rel *prel;
   extern SC     **pl_br_rel;
   extern qtd_plrel;

   extern SC *mat_pag;
   extern SS col_inicial_totais;
   extern COUNT offset, num_soma;
   extern SS ult_linha_util;
   extern UL prim_pagina, num_pagina, off_pagina, qtd_pagina;
   extern RTela *prt;
   extern struct tot_col *col_tot;

   extern Campo *vc[4], *pc_txt;
   extern Campo **vetcp;
   extern COUNT max_ord;
   extern COUNT ord_txt;
   extern COUNT off_txt;
   extern COUNT QQ1, QQ2, QQ3;
   extern COUNT nivel_qbr;
   extern TEXT *lin_sort;
   extern Ger_quebra qbr[ ];
   extern Grel *grel;
   extern TEXT DELIM[];
#endif



