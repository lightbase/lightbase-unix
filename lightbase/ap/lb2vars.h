#ifdef MODO_DEFINICAO
#define   DEFDEC  /* Nada */
#else
#define   DEFDEC   extern
#endif

DEFDEC   Par_cfg     par_cfg;  /* parametros compartamentais */
DEFDEC   Rst_base    rst_base; /* Restricoes de edicao da base */
DEFDEC   Esp_base    esp_base; /* Definicoes especiais da base */

DEFDEC   COUNT       ap_limpacampos;  /* flag especial para limpar campos quando muda de vista com alt_f6 */

DEFDEC   COUNT       ace_dnum;
DEFDEC   COUNT       ace_inum;
DEFDEC   COUNT       lb4_dnum;
DEFDEC   COUNT       tab_dnum;
DEFDEC   COUNT       tab_inum;
DEFDEC   POINTER     lb4_pos;
DEFDEC   COUNT       tabelas_aberta;

DEFDEC   TEXT        tmp_fname[PATH_MAX];

DEFDEC   COUNT       ap_adverte;  /* advertencia para erros em logs */

DEFDEC   COUNT       ap_max_users;
DEFDEC   COUNT       ap_ignorauso;

DEFDEC   COUNT       ap_uso_exclusivo;


DEFDEC   TEXT       *ap_aprovado;
DEFDEC   LONG        ap_nserie;

DEFDEC   TEXT        ap_home_lb[PATH_MAX+1];
DEFDEC   TEXT        ap_dirbase[PATH_MAX+1];
DEFDEC   TEXT        ap_chave_ret[LB_KEYMAX+1];

DEFDEC   UTEXT       ap_cripto[11];

DEFDEC   Acesso      ap_acesso_ativo;

DEFDEC   Campo *pc_visual;

DEFDEC   Base        *v_bases[MAX_SELE];
DEFDEC   Desc_base   *c_bases[MAX_SELE];
DEFDEC   Vet_itens   *v_campos[MAX_SELE];
DEFDEC   Vista       ap_vista;
DEFDEC   ARG        *ap_visdoc;

DEFDEC   Rel         ap_rel;
DEFDEC   ARG        *ap_reldoc;

DEFDEC   TEXT        ap_nome[TE+1];
DEFDEC   TEXT        ap_local[TD+1];
DEFDEC   TEXT        ap_grupo[TE+1];
DEFDEC   TEXT        ap_idioma[TE+1];
DEFDEC   TEXT        ap_status;

DEFDEC   TEXT        BASE_RELACIONADA[TD+1];

DEFDEC   TEXT        ap_back_lb[TD+1];
DEFDEC   TEXT        ap_rest_lb[TD+1];
DEFDEC   TEXT        ap_sort_lb[TD+1];

DEFDEC   COUNT       versao_velha;
DEFDEC   COUNT       em_edicao;
DEFDEC   COUNT       ap_mxitens;   /* maximo de elementos de vista/Relatorio */
DEFDEC   COUNT       ap_mxbases;   /* maximo de bases para selecao */


DEFDEC   COUNT       ap_pos_index;
DEFDEC   COUNT       ap_autosave;
DEFDEC   COUNT       ap_sinonimos;
DEFDEC   COUNT       ap_fonetica;
DEFDEC   COUNT       ap_critica;

DEFDEC   COUNT       ap_psqslot;
DEFDEC   COUNT       ap_flushfile;
DEFDEC   COUNT       ap_curflushfile;
DEFDEC   COUNT       ap_maxprinter;

DEFDEC   UTEXT       ap_delitables;
DEFDEC   COUNT       ap_maxmatriz;

DEFDEC   Tables     *ap_vetmatriz;
DEFDEC   TEXT       *ap_vetprinter[MAX_PRINTS];

DEFDEC   TEXT     ap_tabidioma[MAX_IDIOMAS][TE+1];
DEFDEC   TEXT     OP_IDIOMAS[MAX_IDIOMAS * (TE + 1)];
DEFDEC   TEXT     OP_SIM[10];
DEFDEC   TEXT     OP_NAO[10];
DEFDEC   TEXT     op_sn[20];   /* "sim@nao" */

DEFDEC   TEXT     *buf_iotxt; /* buffer para io de texto e load_vet */
DEFDEC   COUNT    ap_extravet; /* Indicativo de informacao a ser lida/gravada apos load/save_vet */


DEFDEC FILE  *fpo_glb;
DEFDEC WIN   *jl_glb;
DEFDEC COUNT e_log_glb;

DEFDEC   COUNT    n_cp_unica;  /* indice do campo invalido em chave unica */
DEFDEC   COUNT    em_del_glb;  /* usada em reg_display e exclui registro */
DEFDEC   COUNT    ap_maxitens;

DEFDEC   UCOUNT   ap_sizeio;

DEFDEC   S_DELIMIT  ap_icomun;
DEFDEC   S_DELIMIT  ap_itexto;


DEFDEC   COUNT  pl_sele;
DEFDEC   WIN   *pl_win;
DEFDEC   COUNT  pl_tela;
DEFDEC   COUNT  pl_atrib;

DEFDEC   COUNT  ap_operacao;


#ifdef MODO_DEFINICAO
TEXT str_nada[] = "";
TEXT S_1[] = "1";
TEXT S_0[] = "0";
TEXT str_XX80[] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
TEXT str_x80[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
TEXT CP_ID[]   = "CP";
TEXT US_ID[]   = "US";
TEXT GR_ID[]   = "GR";
TEXT D0_ID[]   = "D0";
TEXT D1_ID[]   = "D1";
TEXT D2_ID[]   = "D2";
TEXT DA_ID[]   = "DA";
#else
extern TEXT str_nada[];
extern TEXT S_1[];
extern TEXT S_0[];
extern TEXT str_XX80[];
extern TEXT str_x80[];
extern TEXT CP_ID[];
extern TEXT US_ID[];
extern TEXT GR_ID[];
extern TEXT D0_ID[];
extern TEXT D1_ID[];
extern TEXT D2_ID[];
extern TEXT DA_ID[];
#endif


/*****   Globais para indexacao ********/
#ifdef MODO_DEFINICAO
   UTEXT plv_delimita[ ]  = " AAAAXX,()[]<>:\"'\\|*%+=&^$#"; /* AAAA serao substituidos por atributos */
   UTEXT fra_delimita[ ]  = ".;!?";
   UTEXT prgf_delimita[ ] = "P\n\r";  /* P sera mudado por PARA_CHR */
   UTEXT tudo_delimita[ ] = " PAAAAXX\n\r,()[]<>:\"'\\|*%+=&^$#.;!?";  /* idem */
#else
   extern UTEXT plv_delimita[];
   extern UTEXT fra_delimita[];
   extern UTEXT prgf_delimita[];
   extern UTEXT tudo_delimita[];
#endif



#define   WBS     v_bases[0]
#define   WCB     c_bases[0]

#define   WCP_Q(n)   c_bases[n]->cb_q_cmp
#define   WCP_P(n)   c_bases[n]->cb_p_cmp


/****** Global p/ reprocessamento ******************************************/
#ifdef MODO_DEFINICAO
LONG ap_nregexp = 0L;      /* numero do registro a ser impresso na exportacao */
                           /* o certo seria exportar o registro de selenovo */
                           /* mas esta dando pau. Usei este artificio por falta de tempo */
                           /* para uma solucao mais elegante. Ananias (12/02/95) */

#else
extern LONG ap_nregexp;
#endif

