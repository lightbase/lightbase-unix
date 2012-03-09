/*---------------------------------------------------------------------*/
/*  Parametros comportamentais                                         */
/*---------------------------------------------------------------------*/

/* Restricoes de edicao de registros da base */
typedef struct {
   COUNT ativada;
   COUNT hierarquico;
   COUNT UD;
   TEXT *UDcampos;
   COUNT GD;
   TEXT *GDcampos;
   COUNT UR;
   TEXT *URcampos;
   COUNT GR;
   TEXT *GRcampos;
} Rst_base;

/* definicoes especiais da base */
typedef struct {
   COUNT presente;
   COUNT vista_inclusao;
   COUNT vista_aposgravar;
   COUNT vista_altf6;
   TEXT *campos_altf6;
   COUNT tecla_limpa;
   TEXT *campos_limpa;
   COUNT nlin_ap;
   TEXT *linhas_ap[10];
   COUNT nlin_det;
   TEXT *linhas_det[10];
} Esp_base;
                 
typedef struct {
   TEXT tipo_login; /* Tipo de login 0-pede usuario repetitivo
                                     1-pede usuario unica vez
                                     2-utiliza usuario de login(UNIX) */
   TEXT *usuario; /* Usuario que deve utilizar a base */
   TEXT *senha;   /* senha dousuario que deve ser logado */
   TEXT *base; /* base especifidada para ser usada pelo usuario */
   TEXT word_len; /* tamanho minimo p/ uma palavra ser indexada */
   TEXT filemenu[PATH_MAX+1];  /* hipermenu de utilizacao */
   UCOUNT flags_especiais;
                          
} Par_cfg;


/*---------------------------------------------------------------------*/
/*  Definicao de BASE                                                  */
/*---------------------------------------------------------------------*/

#define   TAM_BSFIXO  (TE+TD+TD+TE+TE+5+2+2)

typedef struct {

   /* Atencao nao alterar tamanho total ate a proxima marca sem alterar */
   /* o define TAM_BSFIXO */

   TEXT     bs_nome[TE+1];
   TEXT     bs_path[TD+1];
   TEXT     bs_desc[TD+1];
/***********   TEXT     bs_relacionada[TD+1]; ******/
   TEXT     bs_dono[TE+1];    /* Nome do dono da base                  */
   TEXT     bs_grupo[TE+1];   /* Nome do grupo do dono da base         */
   UCOUNT   bs_idbase;        /* Numero de identificacao da base */
   LONG     bs_nserie;        /* Numero de serie da versao geradora    */
   UCOUNT   bs_xor;

   /****  Total de bytes que deveram ser gravados em acessos :  */

   COUNT    bs_versao;      /* Forma de descricao da base            */
   COUNT    bs_sepdecimal;  /* Separador decimal de inteiros */
   COUNT    bs_u_seq;       /* Ultimo numero de campo                */
   COUNT    bs_release;     /* Release da base comercial             */
   COUNT    bs_n_vis;       /* Numero de vistas disponiveis          */
   COUNT    bs_n_rel;       /* Numero de relatorios disponiveis      */

   ULONG    bs_cr_data;       /* Data de criacao                       */
   ULONG    bs_al_data;       /* Data de alteracao                     */
   ULONG    bs_bk_data;       /* Data do ultimo backup                 */
   COUNT    bs_finger;        /* Identifica base comercial             */
   TEXT     bs_status;        /* Base travada p/ manutencao/modificacao*/
   TEXT     bs_updated;       /* header da base atualizado. deve ser gravado */

   POINTER  bs_swpos;         /* Posicao de stop words  em lb4 */
   POINTER  bs_cppos;         /* Posicao dos campos     em lb4 */
   POINTER  bs_vspos;         /* Posicao das vistas     em lb4 */
   POINTER  bs_rlpos;         /* Posicao dos relatorios em lb4 */
   TEXT     bs_marca[2];      /* marca para rebuld */
} Base ; /* Tipo DB */



/*---------------------------------------------------------------------*/
/*  Registro de controle do arquivo LB1                                */
/*---------------------------------------------------------------------*/
typedef struct {
   TEXT     rc_status;    /* status do registro */
   ULONG    rc_incdata;   /* data de inclusao   */
   ULONG    rc_altdata;   /* data de alteracao  */
   POINTER  rc_pos;       /* posicao do registro em TBF */
} Reg_controle;



/*---------------------------------------------------------------------*/
/*  Itens de vetores                                                   */
/*---------------------------------------------------------------------*/
typedef struct {
   TEXT  vi_tipo;           /* tipo da estrutura */
   COUNT vi_ident;          /* particularizacao da estrutura */
   TEXT  vi_status;         /* status de edicao da estrutura */
                            /* se possivel flags distintos para inclusao/exclusao */

   POINTER vi_pos;
   UCOUNT  vi_ftam;        /* Tamanho da estrutura no arquivo */

   UCOUNT  vi_mmax;        /* Tamanho maximo da estrutura em memoria */
   UCOUNT  vi_nmax;        /* Maximo admissivel de elementos no vetor */

   TEXT   *vi_buffer;      /* Buffer onde se encontram os itens */
   TEXT  **vi_vtit;        /* Vetor de ponteiros para itens do vetor */

   UCOUNT  vi_mtam;        /* Tamanho da estrutura em memoria */
   UCOUNT  vi_nelem;       /* Numero de elementos no vetor */
   COUNT   vi_curr;        /* Item corrente */
   UCOUNT  vi_clen;        /* Tamanho do item corrente */
} Vet_itens;





/*---------------------------------------------------------------------*/
/*  Definicao de PARAMETROS de critica de campos                       */
/*---------------------------------------------------------------------*/
typedef union {
   struct {
      TEXT  tabela[25];   /* Nome do arquivo tabela             */
      TEXT  autonum[25];  /* Mascara para autonum               */
   } tab_auto;
   struct  {
      TEXT     cp_minimo[25];  /* Valor minimo  .....ver cw_param     */
      TEXT     cp_maximo[25];  /* Valor maximo  .....ver cw_param     */
   } intervalo;
   TEXT    cp_msc_form[30];      /* mascara da formula */
} Parametros;



/*---------------------------------------------------------------------*/
/*  Definicao de CAMPO                                                 */
/*---------------------------------------------------------------------*/

typedef struct {
   TEXT     cp_nome[TE+1];   /* Nome sintetico do campo                */
   TEXT     cp_ordem;        /* ordem de aparecimento p/ rebuild */
   COUNT    cp_num;          /* Numero do campo                        */
   COUNT    cp_decimais;     /* Numero de posicoes decimais            */
   TEXT     cp_desc[TD+1];   /* Descricao do campo                     */

   UTEXT    cp_mhor;         /* Parte maxima  horizontal do campo    */
   UTEXT    cp_a_tam;        /* tamanho anterior */

   TEXT     cp_tipo;         /* Tipo de campo : XNDHT$dh             */
   TEXT     cp_a_tipo;       /* Tipo anterior */

   TEXT     cp_mascara[TM+1];  /* Mascara de leitura/display -picture*/

   TEXT     cp_chave_unica; /* Nao podera existir outra chave igual */
   TEXT     cp_branco;      /* Indexa campos sem informacao         */
   TEXT     cp_fonetica;    /* cria arvore para pesquisa fonetica   */
   TEXT     cp_indexavel;   /* Devera ou nao ser incluido no indice */

   TEXT     cp_rodape[52];  /* Mensagem apresentada no rodape       */
   TEXT     cp_help[9];     /* Help de campo                        */

   TEXT     cp_critica;       /* tipo de critica a ser efetuada     */
                              /* 0 - Nenhuma       I - intervalo    */
                              /* M - Mod 11        T - tabela       */
                              /* C - Cpf           G - cGc          */
                              /* A - Autoincremento                 */


	Parametros cp_param;       /* parametros para efetuar critica    */

	TEXT    *cp_olddata;       /* Dados anteriores a edicao do campo */
	TEXT    *cp_newdata;       /* Dados posteriores a edicao do campo*/
   COUNT    cp_oldtam;        /* tamanho de old_data */
   COUNT    cp_newtam;        /* tamanho de new_data */
   ARG     *cp_adoc;          /* Ponteiro associado ao Editor       */
   VOID    *cp_pttab;         /* ponteiro para opcoes de tabela     */
   TEXT     cp_1futuro;       /* Para uso futuro */
   LONG     cp_2futuro;       /* uso futuro */

   TEXT     cp_mod_flag;      /* flag de modificacao                */
                              /* 0 - Nao alterado  1 - incluido     */
                              /* 2 - modificado    3 - excluido     */
   TEXT     cp_reb_flag;      /* flag de processamento              */
   TEXT     cp_crt_flag;      /* flag de critica                    */
   TEXT     cp_tpreg[3];      /* Identificador de registro p/ rebuild */
} Campo; /* Tipo CP ( ou  CE para campo excluido ) */


/*---------------------------------------------------------------------*/
/*  Definicao de TTELA                                                 */
/*---------------------------------------------------------------------*/

typedef struct {

   COUNT    te_append;       /* Campo tipo texto append                 */
   TEXT     te_tela;         /* numero da tela                          */
   TEXT     te_tipo;         /* tipo de operacao : B - Box              */
                             /*                    D - Display          */
                             /*                    S - Saida            */
                             /*                    E - Entrada          */
                             /*                    H - Linha Horizontal */
                             /*                    L - Linha Horizontal */

   COUNT    te_num;          /* Numero do campo                       */
   TEXT     te_base[TB+1];   /* Nome da base ao qual o campo pertence */

   UTEXT    te_linha;        /* linha  (idem)  // lin inic qdo Box   */
   UTEXT    te_coluna;       /* coluna (idem)  // col inic qdo Box   */

   UTEXT    te_vver;         /* visivel Vert.  // lin final qdo Box  */
   UTEXT    te_vhor;         /* visivel Vhor.  // col final qdo Box  */

   TEXT     te_autoclear;   /* Mantem val anterior // tipo Box       */
   TEXT     te_obrigatorio; /* Preenchimento obrigatorio             */

   TEXT     te_ivetcp;       /* indice do campo em vet_cmp    @      */
   TEXT     te_icb;          /* indice da base em ctrl_bases  @      */

   WIN     *te_win;          /* ponteiro para janela associada       */
   TEXT     te_tpreg[3];     /* Identificador de registro p/ rebuild */
} TTela; /* Tipo IV */

/*---------------------------------------------------------------------*/
/*  Definicao de RTELA                                                 */
/*---------------------------------------------------------------------*/

typedef struct {

   COUNT    re_futuro;       /* uso futuro                              */
   TEXT     re_tela;         /* numero da tela                          */
   TEXT     re_tipo;         /* tipo de operacao : B - Box              */
                             /*                    D - Display          */
                             /*                    S - Saida            */
                             /*                    E - Entrada          */
                             /*                    H - Linha Horizontal */
                             /*                    L - Linha Horizontal */

   COUNT    re_num;          /* Numero do campo                       */
   TEXT     re_base[TB+1];   /* Nome da base ao qual o campo pertence */

   UTEXT    re_linha;        /* linha  (idem)  // lin inic qdo Box   */
   UTEXT    re_coluna;       /* coluna (idem)  // col inic qdo Box   */

   UTEXT    re_vver;         /* visivel Vert.  // lin final qdo Box  */
   UTEXT    re_vhor;         /* visivel Vhor.  // col final qdo Box  */

   TEXT     re_ivetcp;       /* indice do campo em vet_cmp    @      */
   TEXT     re_icb;          /* indice da base em ctrl_bases  @      */

   TEXT     re_tot;          /* totaliza   'S' indica sim      */
   TEXT     re_quant;        /* quantifica 'S' indica sim      */
   TEXT     re_media;        /* media      'S' indica sim      */
   TEXT     re_desvio;       /* desvio     'S' indica sim      */

   TEXT     re_param[4];      /* Uso futuro      */

   SS       re_status;       /* nivel de tratamento do item          */

   WIN     *re_win;          /* ponteiro para janela associada       */
   COUNT    re_lin_inicial;  /* numero da proxima linha a ser impressa */
   COUNT    re_indtot;       /* indice no vetor de totais */

   TEXT     re_tpreg[3];     /* Identificador de registro p/ rebuild */
} RTela; /* Tipo IR */

/*---------------------------------------------------------------------*/
/*  Definicao de Acessos                                               */
/*---------------------------------------------------------------------*/
typedef struct {
   TEXT     da_ugnome[TE+1];     /* identificacao usuario ou grupo */
                                 /* obs. : sempre deve ser o 1o campo */
   TEXT     da_base[TE+1] ;      /* nome da base */
   UCOUNT   da_idbase;           /* Identificacao da base - Uso Futuro */
   TEXT     da_dfcampo;          /* poderes do usuario : X indica pode */
   TEXT     da_dfvista;
   TEXT     da_dfacesso;
   TEXT     da_dfrel;
   TEXT     da_dfstop;
   TEXT     da_inclui;
   TEXT     da_altera;
   TEXT     da_exclui;
   TEXT     da_pesquisa;
   TEXT     da_emite;
   TEXT     da_exporta;
   TEXT     da_importa;
   TEXT     da_parcial;
   TEXT     da_total;
   TEXT     da_backup;
   TEXT     da_restore;
   TEXT     da_tabela;   /* permissao para incluir em tabela */

   COUNT    da_rel[MAX_REL + 1];       /* vetor de acesso a relatorios */
   COUNT    da_vistas[MAX_VISTAS + 1]; /* vetor de acesso a vistas */

   TEXT     da_mod_flag;      /* Flag de modificacao */
   TEXT     da_tpreg[3];      /* Identificador de registro p/ rebuild */
} Acesso; /* Tipo DA */

/*---------------------------------------------------------------------*/
/*  Definicao de Relatorios                                            */
/*---------------------------------------------------------------------*/
typedef struct {
	TEXT     rel_ident[TD+1];    /* identificacao do relatorio              */
	TEXT     rel_dfacess[TE+1];  /* para editar permisaao de acesso */
   COUNT    rel_ordem;          /* numero de ordem do relatorio            */
   COUNT    rel_n_elem;         /* numero de elementos no relatorio        */
   COUNT    rel_it_ativo;       /* numero do item ativo (indice em vet_erel) */
   COUNT    rel_old_ativo;      /* numero do antigo item ativo */
   Vet_itens *rel_vet;          /* Endereco de vetor de itens de relatorios */
   POINTER  rel_itpos;          /* Posicao dos itens do relatorio em lb4 */

   TEXT     rel_titulo[51];     /* titulo do relatorio                     */
   TEXT     rel_rodape[51];     /* Rodape do relatorio                     */

   TEXT     rel_tipo;           /* tipo de relatorio (F)orm. (C)ol. (E)tiq */
   TEXT     rel_cabec;          /* imprimir cabec SIM/NAO                  */
   UTEXT    rel_linhas;         /* numero de linhas da folha               */
   UTEXT    rel_colunas;        /* numero de colunas do papel              */

   UTEXT    rel_l_inter;        /* linhas entre etiquetas || tam cabecalho */
   UTEXT    rel_c_inter;        /* colunas entre etiquetas|| tam rodape    */
   UTEXT    rel_n_etq;          /* etiquetas por linha    || uteis no rel  */

   TEXT     rel_caracter;       /* tipo do caracter Normal/Comprimido      */
   TEXT     rel_qualid;         /* Qualidade: Normal/Carta                 */
   TEXT     rel_folha;          /* impressao em folha solta                */

   COUNT    rel_1numcampo;      /* campo de classificacao                  */
   COUNT    rel_1ind;           /* indice de vetcp                         */
   TEXT     rel_1ord;           /* ordem Ascendente/Descendente            */
   UTEXT    rel_1linhas;        /* linhas a pular na quebra (99 pagina)    */
   TEXT     rel_1total;         /* totaliza SIM/NAO                        */
                                                                           
   COUNT    rel_2numcampo;      /* campo de classificacao                  */
   COUNT    rel_2ind;           /* indice de vetcp                         */
   TEXT     rel_2ord;           /* ordem Ascendente/Descendente            */
   UTEXT    rel_2linhas;        /* linhas a pular na quebra (99 pagina)    */
   TEXT     rel_2total;         /* totaliza SIM/NAO                        */
                                                                           
   COUNT    rel_3numcampo;      /* campo de classificacao                  */
   COUNT    rel_3ind;           /* indice de vetcp                         */
   TEXT     rel_3ord;           /* ordem Ascendente/Descendente            */
   UTEXT    rel_3linhas;        /* linhas a pular na quebra (99 pagina)    */
   TEXT     rel_3total;         /* totaliza SIM/NAO                        */
                                                                           
} Rel; /* Tipo RL */                                                       

/*---------------------------------------------------------------------*/
/*  Definicao de Vistas                                                */
/*---------------------------------------------------------------------*/
typedef struct {
	TEXT     vis_ident[TD+1];    /* identificacao do vistas  */
	TEXT     vis_dfacess[TE+1];  /* para editar permisaao de acesso */
   COUNT    vis_ordem;          /* numero de ordem da vista */
/*   COUNT    vis_n_elem; */        /* numero de elementos na vista */
   COUNT    vis_it_ativo;       /* numero do item ativo (indice em vet_evis) */
   COUNT    vis_old_ativo;      /* numero do antigo item ativo */
   Vet_itens *vis_vet;          /* Endereco de vetor de itens de vista */
   POINTER  vis_itpos;          /* Posicao dos itens da vistas em lb4 */
} Vista;




/*---------------------------------------------------------------------*/
/*  Controle de utilizacao da Base                                     */
/*---------------------------------------------------------------------*/

typedef struct {
   TEXT     cb_fordem; /* sele em uso + 1 */
   TEXT     cb_prfile;

   Vet_itens *cb_vvistas;
   Vet_itens *cb_vrelats;

   ULONG        cb_numrec;   /* numero do registro corrente */
   Reg_controle cb_controle; /* controle de acesso ao registro corrente */
   POINTER      cb_aclock;   /* posicao em lock do arquivo de controle  */
   POINTER      cb_adlock;   /* posicao em lock do arquivo de dados     */

   TEXT   **cb_vetstops;
   TEXT    *cb_palstops;
   TEXT    *cb_ioarea;
   TEXT    *cb_wkarea;
   UCOUNT   cb_tamrec;
   TEXT     cb_tpopen;
   TEXT     cb_lockatu;
} Desc_base; /* Tipo DB (TDB) ou D0 (ACESSOS) */

/*---------------------------------------------------------------------*/
/*  Definicao de Usuarios                                              */
/*---------------------------------------------------------------------*/
typedef struct {
   TEXT     us_tipo_reg[3];      /* tipo de registro  = US */
   TEXT     us_ident_usu[TE+1];  /* nome de login do usuario  */
   TEXT     us_senha[TE+1];      /* senha criptografada do usuario */
   TEXT     us_grupo[TE+1];      /* nome do grupo */
   TEXT     us_full_name[TD+1];  /* nome completo do usuario  */
   TEXT     us_loc_trab[TD+1];   /* local de trabalho */
   TEXT     us_fone[50];         /* 2 telefones - (9999) 999-9999 R-9999 */
   TEXT     us_idioma[TE+1];     /* Idioma */
   TEXT     us_menu[TD+1];       /* Menu associado ao usuario */
   TEXT     us_status[2];        /* A-adm  B-def_base  C-comun D-desativado */
                                 /* se for tipo P pode acessar cad. usu. */
   TEXT     us_tmsenha;          /* tamanho da senha original */
   TEXT     us_uso_futuro[4];    /* check tipo usuario + Uso futuro */
   TEXT     us_tpreg[3];         /* Identificador de registro p/ rebuild */
} Usuario; /* Tipo US */

/*---------------------------------------------------------------------*/
/*  Definicao de Grupos                                                */
/*---------------------------------------------------------------------*/
typedef struct {
   TEXT     gr_ident_grp[TE+1];  /* nome do grupo  */
   TEXT     gr_full_name[TD+1];  /* nome completo do responsavel  */
   TEXT     gr_loc_trab[TD+1];   /* local de trabalho */
   TEXT     gr_fone[50];         /* 2 telefones - (9999) 999-9999 R-9999 */
   TEXT     gr_futuro;
   TEXT     gr_tpreg[3];         /* Identificador de registro p/ rebuild */
} Grupo;  /* Tipo GR */



/*---------------------------------------------------------------------*/
/*  ESTRUTURAS DE INDEXACAO                                           */
/*---------------------------------------------------------------------*/
typedef struct 
{
   SC   *frase;
   SC   *palavra;
   SC   *paragrafo;
   SC   *(*stop_words)[ ]; /* Pont. para array de pont. para STOP_WORDS */
   SS   q_stop;            /* Alem do array terminar com NULL aqui devera' */
                           /* conter a Quantidade de STOP_WORDS */
}  S_DELIMIT;


typedef struct 
{  /***** ATENCAO SE ALTERAR MUDAR TAMBEM SDINDEX logo abaixo *****/
   SC   palavra[ LB_KEYMAX ];
   SL   registro;
   US   campo;
   US   paragrafo;  /* So' Campo Texto, outros Campos serao de uma Frase */
   US   frase;      /* So' Campo Texto, outros Campos serao de uma Frase */
   US   sequencia;  /* Sequencia da Palavra na FRASE */
}  S_INDEX;

/*---------------------------------------------------------------------*/
/*  Controle de pesquisa                                               */
/*---------------------------------------------------------------------*/
typedef struct {
   SL   registro;
   US   campo;
   US   paragrafo;  /* So' Campo Texto, outros Campos serao de uma Frase */
   US   frase;      /* So' Campo Texto, outros Campos serao de uma Frase */
   US   sequencia;  /* Sequencia da Palavra na FRASE */
} SDINDEX;

/*---------------------------------------------------------------------*/
/*  Definicao de TABLES                                                */
/*---------------------------------------------------------------------*/
typedef struct {
   TEXT   tb_nome[TB+1];
   TEXT  *tb_inicio;
   COUNT  tb_qtd;
} Tables;

