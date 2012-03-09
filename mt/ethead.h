
#include  <setjmp.h>

#ifdef    VMEM

#include  <vm.h>

/***
#define   VM_ARRAYS  Nao Pode, pois os arrays precisam estar sempre ...
                     ... disponiveis e o VM nao administra ...
                     ... convenientemente muitos LOCK's.
***/

typedef   VM_HND   PTR;
typedef   VM_HND   VOH;

#else

#define   vm_start( )                          /* Nada */
#define   vm_alloc( size )                     malloc( size )
#define   vm_realloc( voh, newsize )           realloc( voh, newsize )
#define   vm_deref( voh, lock )                (voh = voh) /* Sem Warnings */
#define   vm_unlock( voh )                     (voh = voh) /* Sem Warnings */
#define   vm_free( voh )                       free( voh )
#define   vm_end( )                            /* Nada */

typedef   SC *   PTR;
typedef   SS     VOH;

#endif

#define   CTRL_CHAR    '^'
#define   CTRL_MAX     ' ' /* 32 */
#define   CTRL_VIS     '@' /* 64 */
#define   E_CTRL( C )  ((UC) C < CTRL_MAX )
#define   E_8BITS( C ) ((UC) C >= 128 && mostra_oito > 1 )

#define   LIN_ENTRA    1
#define   LIN_SAI      2

#define   ET_NORMAL    1
#define   ET_SUBLIN    2
#define   ET_ENFAT     3
#define   ET_ITAL      4
#define   ET_FORM1     5
#define   ET_FORM2     6

#define   TAB_MAX      16 /*** Tamanho maximo de TAB ***/

#ifdef    MEDIT
#define   LIN_INIC     500   /* Linhas allocadas no inicio, Sempre > 20 */
#define   INC_LIN      (args->t_lines / 4) /* Linhas reallocadas a mais de cada vez */
#else
#define   LIN_INIC     100   /* Linhas allocadas no inicio, Sempre > 20 */
#define   INC_LIN      (args->t_lines / 4) /* Linhas reallocadas a mais de cada vez */
#endif
#define   TAM_BOT      25
#define   YES          1
#define   NOT          0
#define   LINE_LEN     3000  /* Nao recomendamos maior que 16000 */
#define   MAX_WRAP     240

#ifdef    MEDIT

#ifdef    MSDOS
#define   MAX_LINE     16000 /* MEDIT MSDOS 64000 / sizeof(SC *) == 16000 */
#else
#define   MAX_LINE     32000 /* MEDIT UNIX < 32KB */
#endif

#else

#ifdef    XTOOLS
#define   MAX_LINE     4000  /* XTOOLS 64000 / sizeof(SC *) == 16000 */
#else
#define   MAX_LINE     64000  /* LIGHT  64000 / sizeof(SC *) == 16000 */
#endif

#endif

#define   FILE_LEN     60
#define   disp_file    3    /*** Refresh from Text ***/
#define   disp_line    2    /*** Refresh Line      ***/
#define   delim        '\n'  /* Delimita linhas, texto passado por buffer */
#define   PARA_CHR     UG_ETPARA  /* Marca de fim de paragrafo */

/*** Defines para args->forma parametro em edit_buffer( ) ***/
#define   ET_EDITA     0
#define   ET_NAVEGA    1
#define   ET_MOSTRA    2
#define   ET_APPEND    3
#define   ET_MENU      4

/*** Defines para args->tipo parametro em load_buffer( ) ***/
#define   ET_FILE      'F'
#define   ET_HANDLE    'H'
#define   ET_MEM       'B'
#define   ET_MEDIT     'M'

#define   BUSCA        1
#define   TROCA        2

#ifdef    ET_UNDO

#define   U_DESLIGA     0
#define   U_NEXT        1
#define   U_LINHA       2

#define   U_INIC       '!'
#define   U_ANTES      '<'
#define   U_DEPOIS     '>'
#define   U_SAI        '-'
#define   U_ENTRA      '+'
#define   U_BLOCK      '#'
#endif

/***
De 0 a MAX_SCRAP - 1 e' a quantidade de SCRAPS usado em ^KK, ^KC e ^KV.
MAX_TEMP indica quantidade de arquivos TEMPORARIOS.
MAX_SCRAP + 0 e' usado em Macro default.
***/
#define   MAX_SCRAP    10
#define   MACRO_TMP    (MAX_SCRAP + 0)
#define   MAX_TEMP     (MAX_SCRAP + 1)

#define   BLK_UPPER    'U'
#define   BLK_LOWER    'l'

#define   BLK_CHR      'B'
#define   BLK_COL      'N'
#define   BLK_LIN      'L'
#define   BLK_NOT      'H'

#define TMF   5   /*** Tamanho Minimo do Formato do Paragrafo ***/
/***
Imediatamente antes do Final de Paragrafo poderemos ter o seguinte Formato.
ET_FORM1  MAR_ESQ  MAR_PRI  MAR_DIR  ET_FORM2  ET_PARA
Portanto a linha de Final de Paragrafo que possua ET_FORM deve ter ...
... no MINIMO tamanho maior ou igual a TMF.
***/

#ifdef    MTE_VAR
#define   ET_extern            /* Declara */
#else
#define   ET_extern   extern   /* Referencia */
#endif

/*** Estrutura de BUFFER ***/

typedef struct arg_list ARG;

struct arg_list {

   UC  forma;      /*  0 - ET_EDITA  - Editar
                    *  1 - ET_NAVEGA - Navegar
                    *  2 - ET_MOSTRA - Mostrar e Sair
                    *  3 - ET_APPEND - So Acrescenta
                    */
   WIN *je;        /*  Janela de edicao
                    */
   WIN *jm;        /*  Janela de Mensagens, se for NULL abre-se uma propria.
                    */
   SS doc_mode;    /*  == 0 - Modo texto
                    *  >  0 - Margem direita para word wrapping
                    *       - Recomendamos == arg_list.je->w_len
                    */
   SC bot[TAM_BOT];/*  Lin: 99999  Col: 99999
                    *  123456789 123456789 12345
                    */
   SC *escape;     /*  Teclas de escape
                    */
   SC *def_mens;   /*  Linha de mensagem default
                    */
#ifdef   PROTO_ANSI
   SS (*funcao)( ARG * );
#else
   SS (*funcao)( );
#endif
                   /*  Funcao chamada apos cada refresh, e' passado ...
                    *  ... "ARG *", se voltar -1 encerra edicao.
                    */
#ifdef   PROTO_ANSI
   SS (*admlin)( ARG *, SS, SS );
#else
   SS (*admlin)( );
#endif
                   /*  Funcao chamada apos cada entrada ou saida de linha ...
                    *  ... e' passado o ARG *, a linha e LIN_(SAI|ENTRA).
                    */
   SC tipo;        /*  ET_FILE -> Arquivo "SC *file"
                    *  ET_HANDLE -> Descritor "handle" tamanho "buf_len"
                    *  ET_MEM -> Posicao em memoria "SC * buffer" + "buf_len"
                    *  ET_MEDIT -> Arquivo aberto em "file" como FP
                    */
   SC *file;       /*  Se "tipo" e' ET_FILE, indica um nome de arquivo.
                   /*  Se "tipo" e' ET_MEDIT, indica um FP aberto.
                    */
   SS handle;      /*  Se "tipo" e' ET_HANDLE indica um descritor de arquivo.
                    */
   SC *buffer;     /*  Se "tipo" e' ET_MEM, indica um "SC *".
                    */
   SL buf_len;     /*  Se "tipo" e ET_HANDLE ou ET_MEM, indica o tamanho.
                    */
   SS tmp_ss;      /*  Reserva para usuario. Inteiros.
                    */
   SC *tmp_scp;    /*  Reserva para usuario. Ponteiro para qualquer tipo.
                    */

   /*** Variaveis Internas de controle do Buffer ***/

   /*** Maximo LINHA e COLUNA e' 32.000, pois a variaveis sao SS ... ***/
   /*** ... e sao testadas se menor que ZERO !!! ***/
   /*** Porem observe o tamanho do array (16000 * (SC *) = 64000) ! ***/
   /*** Porem observe o tamanho do array (32000 * (SS)   = 64000) ! ***/
   PTR *ptr;              /* As linhas */
   SS  *len;              /* Tamanho das linhas */
#ifdef   VM_ARRAYS
   VOH VMptr;             /* VM das linhas */
   VOH VMlen;             /* VM dos tamanho das linhas */
#endif
#ifdef    ET_UNDO
   SC   *undo;            /* Nome do arquivo de UNDO */
   SS   redo;             /* Quant. de Redo disponivel */
   FILE *FU;              /* Descritor de arquivo de UNDO */
   UC   flg;              /* Flag se entrou em UNDO */
   SL   pos;              /* ftell() referente ao UNDO inicial */
   SL   ant;              /* ftell() referente ao UNDO anterior */
   SL   off;              /* ftell() referente a posicao corrente */
#endif
   WIN *jzoom;            /* Janela de trabalho do zoom */
   SS  beg_col, end_col;  /* Primeira e ultimas coluna visiveis */
   SS  beg_line;          /* Primeira LINHA visivel */
   SC  abriu_jm;          /* Indica se JM foi aberta pelo editor */
   SS  row, col, clin;    /* LINHA, COLUNA e LINHA_CORRENTE */
   SS  alin;              /* Modo APPEND so' modifica depois desta Linha */
   SS  t_lines;           /* Quant de linhas em uso */
   SS  max_lines;         /* Maxima quantidade de linhas diponivel */
   SC  f_mod;             /* Arquivo modificado ? */
   SC  f_saved;           /* Arquivo ja foi salvo nao tire backup */
   SS  tecla;             /* Ultima tecla pressionada, menor que 0 e' erro */
   SC  estado;            /* 1 - load_buffer( )
                           * 2 - edit_buffer( )
                           */
};

/************** Internal variables *****************************************/

/*** Funcao que move_scr() chama indicando Linha e Coluna corrente ... ***/
/*** ...normalmete e' disp_stat(). ***/
#ifdef     PROTO_ANSI
ET_extern  VOID    (*lin_col)( ARG *, US row, US col, US lin );
#else
ET_extern  VOID    (*lin_col)( );
#endif

ET_extern  UC  undo_on;    /* 0 - Undo Desligado - U_DESLIGA */
                           /* 1 - Next_Undo()    - U_NEXT    */
                           /* 2 - Undo_Linha()   - U_LINHA   */
ET_extern  jmp_buf end_rtn[10];
ET_extern  SI  num_texto;
ET_extern  SI  max_textos;
ET_extern  SS  ult_comando; /* Usado em pegacomando() - Ultimo comado */
ET_extern  SC  *et_ajuda; /* ajuda do editor */
ET_extern  SS  et_quant; /* Quantos buffer ativos */
ET_extern  SS  margin,   /* No PgUp/Dn a ultima linha vai para primeira */
               display,
               cr_new_line, join_lines,
               tab_width, tab_align,
               tab_ug_sp, tab_tab,
               no_br_fim,   /* Tira Brancos e TABs do Final das linhas */
               mostra_oito, /* 0 - Caracter via descricao de terminal */
                            /* 1 - Caracter sem conversao */
                            /* 2 - Decimal, 3 - Hexa */
               br_tab,      /* Troca ao salvar os primeiros Brancos por TABs */
               new_sr,      /* Indica para o SR que editamos outro Buf */
               blk_tipo,
               blk_inifim, /* Segundo INIBLOCO = COPIABLOCO. Norton editor */
               blk_scrap,
               blk_li,
               blk_lf,
               blk_ci,
               blk_cf,
               ins_mode, errorexists;

/********************* Key Board Definition Variables ********************/
/**************************************************************

     Este header define todos os comandos possiveis dentro do
     editor e atribui a cada um deles um numero de referencia
     a ser utilizado pelo modulo de identificacao de comando.
     Com esta estrutura, o identificador podera' ficar confi-
     guravel.

     Alguns comandos nao tem sentido serem configuraveis, como
     por exemplo o "TAB". Neste caso aparecem na lista com um
     sinal >> no inicio da sua descricao .

**************************************************************/

typedef struct {
   SC *ident;        /* identificador do comando */
   UC numero;        /* numero que o identifica entre todos na lista */
   UC escape;        /* diz se o comando e' de escape ... */
} COMANDOS;

typedef struct {
   SC *ident;        /* identificador da tecla */
   UC  numero;       /* codigo dela para o 'MT' */
} TECLAS;

typedef struct {     /* estrutura para armazenar os comandos e as sequen-*/
   UC prim_tecla;    /* cias das teclas para cada um deles */
   UC seg_tecla;
   SS num_comando;
   UC escape;        /* define se e' comando de escape */
} COMSEQ;

#ifdef     MEDIT
#define    MAX_COM   500
#else
#define    MAX_COM   250
#endif
/*** Reserve a mais para as sequencias obrigatorias ***/
#define    COM_TOTAL   MAX_COM + 5
ET_extern  COMSEQ vet_com[ COM_TOTAL ]; /* Sequencias do usuario */
ET_extern  SC     **Hot_Keys;

#define   MAX_MACROS    50
ET_extern SC *mac_files[ MAX_MACROS + 1 ];

#define   MAX_MULTKEY   20
typedef struct {     /* estrutura para armazenar as linhas de funcoes */
   UC tecla;         /* de comandos multiplos */
   SC *funcao;
} KEY_FUNC;
ET_extern KEY_FUNC key_func[ MAX_MULTKEY + 1 ];

/* "def_key_func" e' a linha de funcao default */
ET_extern SC *def_key_func;

/* Nivel de Confirmacao e Geracao de .BAKs para o MEDIT */
ET_extern SS flag_medit;

/* "f_key_func" e' uma funcao que escreve na linha de funcao */
#ifdef   MSDOS
ET_extern VOID (*f_key_func)( SC * );
#else
ET_extern VOID (*f_key_func)( );
#endif

typedef struct {
   SC *cadeia;
   SC  sep;
   SI  achou;
} TOKEN;

#define VOLTAESPACO   1   /* BackSpace */
#define RETORNO       2   /* FIM DE LINHA OU QUEBRA LINHA */
#define SOBE          3   /* SOBE CURSOR PARA A LINHA DE CIMA */
#define DESCE         4   /* DESCE PARA ALINHA DE BAIXO */
#define ESQUERDA      5   /* VOLTA CURSOR UMA POSICAO */
#define DIREITA       6   /* AVANCA CURSOR UMA POSICAO */
#define INILINHA      7   /* VAI PARA O INICIO DA LINHA */
#define FIMLINHA      8   /* VAI PARA O FIM DA LINHA */
#define ROLACIMA      9   /* ROLA A TELA PARA CIMA */
#define ROLABAIXO    10   /* ROLA A TELA PARA BAIXO */
#define PROPAGINA    11   /* AVANCA UMA PAGINA NO TEXTO */
#define ANTPAGINA    12   /* VOLTA UMA PAGINA NO TEXTO */
#define INIARQUIVO   13   /* VAI PARA O INICIO DO TEXTO */
#define FIMARQUIVO   14   /* VAI PARA O FIM DO ARQUIVO */
#define PROPALAVRA   15   /* VAI PARA A PROXIMA PALAVRA */
#define ANTPALAVRA   16   /* VAI PARA A PALAVRA ANTERIOR */
#define PROXTAB      17   /* PROCURA A PROXIMA POSICAO TABULADA */
#define TABANT       18   /* PROCURA A TABULACAO ANTERIOR */
#define TROCATEXTO   19   /* PROCURA E SUBSTITUI UM STRING */
#define DUPLINHA     20   /* DUPLICA A LINHA CORRENTE */
#define PROCTEXTO    21   /* PROCURA UM STRING NO TEXTO */
#define INSLIDEPO    22   /* INSERE UMA LINHA DEPOIS DA CORRENTE */
#define INSLIANTES   23   /*  "      "    "   ANTES   "   "      */
#define JUNTAPROXL   24   /* JUNTA A PROX LINHA NA CORRENTE */
#define TAMTEXTO     25   /* INFORMA O TAMANHO DO TEXTO */
#define VAIPRALIN    26   /* VAI PARA A LINHA DE NUMERO INFORMADO*/
#define APAGAFIMLIN  27   /* APAGA ATE' O FIM DA LINHA CORRENTE */
#define INIBLCHAR    28   /* INICIO DE BLOCO COMUM  */
#define INIBLCOL     29   /* INICIO DE BLOCO COLUNA */
#define INIBLLIN     30   /* INICIO DE BLOCO LINHA  */
#define BLOCOCOPIA   31   /* FIM DE BLOCO GENERICO */
#define BLOCOINSERE  32   /* COPIA    BLOCO GENERICO */
#define BLOCOMOVE    33   /* MOVE      "      "      */
#define DESMARCABLO  34   /* DESMARCA  "      "      */
#define APAGABLOCO   35   /* APAGA     "      "      */
#define EXPORTE      36   /* GRAVA BLOCO EM ARQUIVO */
#define IMPORTE      37   /* GRAVA BLOCO EM ARQUIVO */
#define QUEBRALINHA  38   /* QUEBRA LINHA E MANTEM CURSOR */
#define APAGALINHA   39   /* APAGA A LINHA */
#define APAGACAR     40   /* APAGA O CARACTER SOBRE O CURSOR */
#define APAGAPAL     41   /* APAGA PALAVRA A PARTIR DO CURSOR */
#define INSCTRLE     42   /* INSERIR ATRIBUTOS e CONFIGURAR DIRETIVAS */
#define ABANDONA     43   /* ENCERRA A EDICAO CORRENTE    */
#define SALVARQ      44   /* SALVA O ARQUIVO CORRENTE */
#define INSEREOUNAO  45   /* TROCA ENTRE MODO INSERE/SOBREPOE */
#define ZOOM         46   /* FAZ UM ZOOM DA JANELA CORRENTE  */
#define MUDAJANELA   47   /* MUDA DE JANELA CORRENTE */
#define DIVIDEJANELA 48   /* DIVIDE A JANELA CORRENTE */
#define EXPJANELA    49   /* EXPANDE A JANELA CORRENTE */
#define EDITANOTAS   50   /* EDITA ARQUIVO DE NOTAS */
#define VERARQUIVO   51   /* ABRE ARQUIVO R/O */
#define NOVOARQUIVO  52   /* FORNECER NOVO NOME DE ARQUIVO/BUFFER */
#define TERMINA      53   /* ENCERRA TODA A EDICAO */
#define MENUARQS     54   /* APRESENTA O MENU COM TODOS OS BUFFERS */
#define COMPILA      55   /* COMPILA O BUFFER CORRENTE */
#define PROCPAR      56   /* Procura par de "{([})]" */
#define JANERRO      57   /* Abre janela de erros de compilacao */
#define COMSYSTEM    58   /* Comando externo, ver saida padrao */
#define PLINTELA     59   /* Primeira linha da tela */
#define ULINTELA     60   /* Ultima linha da tela */
#define ROLAESQ      61   /* Rolamento a esquerda */
#define ROLADIR      62   /* Rolamento a direita */
#define SAISALVA     63   /* ENCERRA A EDICAO CORRENTE SALVANDO  */
#define PROXARQUIVO  64   /* Poe na janela corrente proximo arquivo */
#define ANTARQUIVO   65   /* Poe na janela corrente arquivo anterior */
#define MUDATRANS    66   /* Muda SCRAP corrente */
#define JANSYSOUT    67   /* Edita saida padrao do comando system */
#define JANSYSERRO   68   /* Edita saida de erros do comando system */
#define EXECUTA      69   /* Executa comando sem interceptar saidas */
#define GRAVAMACRO   70   /* Grava Macro */
#define EXEMACRO     71   /* Executa macro */
#define ARQMACRO     72   /* Transforma macro em arquivo */
#define REPITABUSCA  73   /* Repita Busca e Substituicao */
#define PROXCARREGA  74   /* Proximo arquivo carregado */
#define ANTCARREGA   75   /* Anterior arquivo carregado */
#define IMPRIMEARQ   76   /* Imprime Arquivo */
#define EDITAVER     77   /* Toggle Editar / Visualizar */
#define TIRALISTA    78   /* Retira o arquivo da lista */
#define DESFAZ       79   /* Desfaz os comnados executados */
#define REFAZ        80   /* Refaz os comnados Desfeitos */
#define INDENTBLOCO  81   /* Indenta Bloco */
#define LINHACIMA    82   /* Linha corrente na primeira linha janela */
#define LINHABAIXO   83   /* Linha corrente na ultima linha janela */
#define LINHAMEIO    84   /* Linha corrente no meio da janela */
#define APPBLCOPIA   85   /* Append bloco para o Scrap */
#define APPBLMOVE    86   /* Append e Erase bloco para o Scrap */
#define BLOCOUPPER   87   /* Coloca Bloco em Maiusculas */
#define BLOCOLOWER   88   /* Coloca Bloco em Minuscula */
#define ORDEMENU     89   /* Ordena Menu */
#define TROCAARQ     90   /* Troca Arquivo entre Janelas */
#define MARCAPOS     91   /* Marca Posicao em um Arquivo */
#define BUSCAPOS     92   /* Vai para posicao previamente marcada */
#define MARGEMPARA   93   /* Margens do Paragrafo */
#define TROCATRAS    94   /* PROCURA E SUBSTITUI UM STRING para Tras */
#define PROCTRAS     95   /* PROCURA UM STRING NO TEXTO para Tras */
#define REPITAFRENTE 96   /* Repita ultima Busca/Troca para Frente */
#define REPITATRAS   97   /* Repita ultima Busca/Troca para Tras */
#define SAICONTEXTO  98   /* Sai gravando contexto de edicao */
#define TECLAQUENTE  99   /* Tecla de disparo de sequencias  */
#define EXPRECHAR   100   /* Recebe um caracter como Expressao */
#define CODCHAR     101   /* Informa em Decimal, Hexa Qual o Char corrente */
#define RECARREGRA  102   /* Atualiza Regras de Compilacao */

/**************** Mneumonicos das Teclas *******************/

#define     NULA         0
#define     CTRLA        1
#define     CTRLB        2
#define     CTRLC        3
#define     CTRLD        4
#define     CTRLE        5
#define     CTRLF        6
#define     CTRLG        7
#define     CTRLH        8
#define     CTRLI        9
#define     CTRLJ        10
#define     CTRLK        11
#define     CTRLL        12
#define     CTRLM        13
#define     CTRLN        14
#define     CTRLO        15
#define     CTRLP        16
#define     CTRLQ        17
#define     CTRLR        18
#define     CTRLS        19
#define     CTRLT        20
#define     CTRLU        21
#define     CTRLV        22
#define     CTRLW        23
#define     CTRLX        24
#define     CTRLY        25
#define     CTRLZ        26
#define     ESCAPE       K_ESC
#define     TECLAVOLTA   K_BS
#define     ENTER        K_ENTER
#define     ENTRA        K_ENTER

#define     F1           K_F1
#define     F2           K_F2
#define     F3           K_F3
#define     F4           K_F4
#define     F5           K_F5
#define     F6           K_F6
#define     F7           K_F7
#define     F8           K_F8
#define     F9           K_F9
#define     F10          K_F0
#define     ALTF1        K_ALT_F1
#define     ALTF2        K_ALT_F2
#define     ALTF3        K_ALT_F3
#define     ALTF4        K_ALT_F4
#define     ALTF5        K_ALT_F5
#define     ALTF6        K_ALT_F6
#define     ALTF7        K_ALT_F7
#define     ALTF8        K_ALT_F8
#define     ALTF9        K_ALT_F9
#define     ALTF10       K_ALT_F0
#define     SETACIMA     K_UP
#define     SETABAIXO    K_DN
#define     SETAESQ      K_LFT
#define     SETADIR      K_RT
#define     PAGINANT     K_PGUP
#define     PAGINAPROX   K_PGDN
#define     INICIO       K_HOME
#define     FIM          K_END
#define     TECINS       K_INS
#define     APAGA        K_DEL
#define     TAB          K_TAB
#define     BACKTAB      K_BTAB
#define     CTRLPGUP     K_CPGUP
#define     CTRLPGDN     K_CPGDN
#define     PALAPROX     K_WORDRIGHT
#define     PALAANT      K_WORDLEFT
#define     ALT_A        K_ALT_A
#define     ALT_B        K_ALT_B
#define     ALT_C        K_ALT_C
#define     ALT_D        K_ALT_D
#define     ALT_E        K_ALT_E
#define     ALT_F        K_ALT_F
#define     ALT_G        K_ALT_G
#define     ALT_H        K_ALT_H
#define     ALT_I        K_ALT_I
#define     ALT_J        K_ALT_J
#define     ALT_K        K_ALT_K
#define     ALT_L        K_ALT_L
#define     ALT_M        K_ALT_M
#define     ALT_N        K_ALT_N
#define     ALT_O        K_ALT_O
#define     ALT_P        K_ALT_P
#define     ALT_Q        K_ALT_Q
#define     ALT_R        K_ALT_R
#define     ALT_S        K_ALT_S
#define     ALT_T        K_ALT_T
#define     ALT_U        K_ALT_U
#define     ALT_V        K_ALT_V
#define     ALT_W        K_ALT_W
#define     ALT_X        K_ALT_X
#define     ALT_Y        K_ALT_Y
#define     ALT_Z        K_ALT_Z

/*********************** Document Mode Variables **********************/

ET_extern  SC  et_atr[ 10 ];
ET_extern  UC  et_just;       /* Justifica linha ? */
ET_extern  US  l_inicio;      /* LINHA Inicial da atualizacao de video */
ET_extern  US  l_fim;         /* LINHA Final   da atualizacao de video */
ET_extern  UC  scroll_pagina; /* Indica se quando chegar ao final da ...    */
                            /* ... salta apenas uma linha ou meia pagina. */

/******************* Necessary Prototypes ********************************/

/*** Sem PROTO_ANSI, funcoes usadas em qsort( ), BCC++ nao aceita !!!
SI comparac(   COMANDOS *ptr1, COMANDOS *ptr2 );
SI comparat(   TECLAS   *ptr1, TECLAS   *ptr2 );
SI comparaseq( COMSEQ   *pt1,  COMSEQ   *pt2  );
SI compara1(   SC       *ptr1, COMANDOS *ptr2 );
SI compara2(   SC       *ptr1, TECLAS   *ptr2 );
***/
SI comparac(   );
SI comparat(   );
SI comparaseq( );
SI compara1(   );
SI compara2(   );

#ifdef    PROTO_ANSI

SS  char_posicao( ARG *, SS linha, SS coluna );
VOID qual_char( ARG * );
VOID exprechar( ARG * );
VOID insere_ctrl( ARG * );
SS   mar_atr( ARG *, SC *L1, SS linha, SS len, SS posicao,
              SS *qatr, SS *qapos, SS *esq, SS *pri, SS *dir );
VOID retira_form( ARG *args, SS linha );
VOID salta_atr( ARG *, SS dir );
VOID C_margens( ARG *args );
VOID set_margens( ARG *, SS linha, SS mar_esq, SS mar_pri, SS mar_dir );
SS   prim_linha( ARG *args, SS linha );
SS   pega_margens( ARG *, SS linha, SS *mar_esq, SS *mar_pri, SS *mar_dir );
VOID avalia_opt( SC *str );
SS   pega_strings( ARG *, SS bt, SS frente );
VOID set_limites( ARG *args, SS repete );
VOID sr_manager( ARG *, SS sr, SS repete, SS frente );
VOID repita_busca( ARG * );
VOID repita_ft( ARG *, SS frente );
SS   s_text( ARG *, SC *linha );
SS   s_line( ARG *, SC *linha, SS lin_num, SS col, SS no_ask );
VOID et_primeiro( void );
VOID et_ultimo( void );
VOID bot_remap( WIN * );
VOID scrap_inic( void );
VOID scrap_free( void );
VOID scrap_unlink( void );
FILE *scrap_write( ARG *args, SS scrap_no, SS append );
FILE *scrap_read(  ARG *args, SS scrap_no );
VOID initela( ARG *args );
VOID fimtela( ARG *args );
VOID leftscroll( ARG *args );
VOID rightscroll( ARG *args );
VOID et_matching( ARG *args );
VOID m_frente( ARG *args, SS ori, SS pro );
VOID m_traz( ARG *args, SS ori, SS pro );
SS   prox_car( ARG *args );
SS   ant_car( ARG *args );
SS   lin_no_final( ARG *, UC *, UC *, SS, SS, SS, SS *, WIN * );
VOID C_d_word( ARG *args );
SS   e_et_atr( ARG *args, SS linha, SS coluna, SS *car );
US   blk_atr( ARG *args, US atr, SS lin, SS col );
US   atr_corrente( ARG *args, SS linha, SS coluna );
SS   tab_linha( ARG *, SC *just, SC *orig, SS linha, SS *col );
SS   margem_linha( ARG *, SC *just, SC *orig, SS linha,
                   SS *off, SS *col, SS *esq );
SS   q_atributos( ARG *args, SS linha, SS posicao, SS pos_real );
SS   add_linbuffer( ARG *, SC * );
VOID diretivas( ARG *args );
SS   cur_aparente( ARG * args, SS linha, SS col );
SS   edit_arquivo( SC *, WIN *, SS, SC *, SS );
SS   edit_buffer( ARG *, SS, WIN *, WIN *, SS );
SS   s_edit_buffer( ARG *, SS, WIN *, WIN *, SS );
VOID set_scr( ARG *, SS, SS );
VOID init_logic( ARG *, SS );
VOID end_logic( ARG *, SS );
VOID pos_line( ARG *, SS );
VOID disp_stat( ARG *, US, US, US );
SS   next_letter( ARG * );
VOID C_cr( ARG * );
VOID C_bk_spc( ARG * );
VOID C_del( ARG * );
VOID C_tab( ARG * );
VOID to_text( ARG *, SS, SS );
VOID insert_line( ARG * );
VOID ins_l_after( ARG * );
VOID ins_l_bef( ARG * );
VOID delete_line( ARG * );
VOID split_line( ARG *, SS, SS );
VOID line_join( ARG * );
VOID C_up( ARG * );
VOID C_down( ARG * );
VOID C_right( ARG * );
VOID C_left( ARG * );
VOID C_home( ARG * );
VOID C_end( ARG * );
VOID C_pgup( ARG * );
VOID C_pgdn( ARG * );
VOID C_beg_file( ARG * );
VOID C_end_file( ARG * );
VOID C_shift_tab( ARG * );
VOID C_next_word( ARG * );
VOID C_prev_word( ARG * );
VOID C_line_blk( ARG * );
VOID C_col_blk( ARG * );
VOID C_chr_blk( ARG * );
VOID next_block( ARG *, SS lin, SS col, SS tipo, SS li, SS ci );
VOID I_right( ARG *, SS car );
VOID I_left( ARG * );
VOID blk_indent( ARG * );
VOID blk_mudatrans( ARG * );
SS   blk_sel( ARG * );
VOID blk_delete( ARG * );
VOID blk_move( ARG *, SS scrap_no, SS append );
VOID blk_copy( ARG *, SS scrap_no, SS append );
VOID blk_upplwr( ARG *, SS upplwr );
VOID UL_lin_block( ARG *, SS upplwr );
VOID UL_col_block( ARG *, SS upplwr );
VOID UL_chr_block( ARG *, SS upplwr );
VOID lin_applwr( ARG *, SC *linha, SS num, SS inicio, SS quant, SS upplwr );
VOID set_block( ARG * );
VOID del_lin_block( ARG * );
VOID copy_lin_block( ARG *, FILE *, SS append );
VOID ins_lin_block( ARG *args, FILE *FA );
VOID move_lin_block( ARG *args, FILE *, SS append );
VOID del_col_block( ARG * );
VOID copy_col_block( ARG *, FILE *, SS append );
VOID ins_col_block( ARG *args, FILE *FA );
VOID move_col_block( ARG *args, FILE *, SS append );
VOID del_chr_block( ARG * );
VOID copy_chr_block( ARG *, FILE *, SS append );
VOID ins_chr_block( ARG *args, FILE *FA );
VOID move_chr_block( ARG *args, FILE *, SS append );
VOID move_scr( ARG * );
VOID linalloc( ARG *, US, US, US );
SS   scr_number( ARG *, SS col );
SS   quit_file( ARG * );
SS   read_file( ARG * );
VOID ins_markers( ARG *, SS );
SS   save_file( ARG * );
VOID C_import( ARG * );
VOID C_export( ARG * );
VOID mem_stat( ARG * );
VOID C_ins( ARG * );
VOID insert_mode( ARG *, SS );
VOID clrerror( ARG * );
SS   def_cmd( SC *arquivo, SC *mac_arq );
VOID sec_key( SS, US );
VOID build_key_tbl( SS );
VOID blk_ins( ARG *, SS );
VOID blk_not( ARG * );
VOID C_blk_not( ARG * );
VOID line_go( ARG * );
VOID C_dup_line( ARG * );
VOID del_end_line( ARG * );
VOID total_wrap( ARG *, SS );
US   text_wrap( ARG *,  US );
VOID linha_anterior( ARG * );
VOID linha_posterior( ARG * );
VOID linha_visivel( ARG * );
VOID ins_linha( ARG *, US );
VOID ret_linha( ARG *, US );
VOID beep( ARG * );
VOID curslin( ARG * );
VOID cursblk( ARG * );
US   ajusta_array( ARG *, US );
ARG  *load_buffer( SS, SS, SC *, SL );
VOID free_buffer( ARG * );
SS   acha_paragrafo( ARG *, SS );
SS   row_touch( ARG *, SS );
VOID clin_touch( ARG *, SS );
VOID C_down_scroll( ARG * );
VOID C_up_scroll( ARG * );
SS   pos_coluna( ARG *, SS linha, SS coluna );
SL   tam_buffer( ARG *, SS tipo );
SS   lin_buffer( ARG *, SS, SC * );
VOID C_zoom( ARG *, SS );
SS   jan_buffer( ARG *, WIN * );
VOID fresh_buffer( ARG * );

/*** Comandos de TECLADO ***/
SS   carga_coms( SC *kbd_file, SC *mac_arq,
                 COMANDOS *comandos, SS ncoms, TECLAS *teclas, SS nteclas );
SS   carga_macs( SC *mac_arq, TECLAS *teclas, SS nteclas, SS comandos );
SS   comp_diret(SC *original,SC *limpo,SS linha,TECLAS *teclas,SS nteclas);
SS   setdefaults( void );
SI   pega_token( SC *cadeia, SC *separador, TOKEN *result );
VOID erro_par( SC *buf, SC *mens, SS linha );
SS   pegacomando( WIN * );
VOID arq_macro( ARG *args );
VOID et_grava_macro( ARG *args );
VOID et_exec_macro( ARG *args );
VOID ajuda_editor( void );
VOID exec_macroarq( ARG *args, SC *arq );

VOID imprime_arq( ARG *args );
SS   tira_br_fim( SC *buf );
SS   br_to_tab( SC *buf );

SS   exp_tabs( ARG *,UC *line,UC *tmp_line,SS max_len,SS cur_len,WIN *JM );
SS   longa_para( ARG *, UC *line, SS cur_len, SS longa_lin, SS *para_used );

#ifdef   ET_UNDO
VOID start_undo( ARG * );
VOID restart_undo( ARG * );
VOID close_undo( ARG * );
VOID end_undo( ARG *, WIN *mens );
VOID next_undo( ARG *, SS clin, SS col );
VOID C_undo( ARG * );
VOID C_redo( ARG * );
VOID undo_linha( ARG *, SS cmd, SS num, SS tam, SC *linha );
VOID undo_block( ARG *, SS cmd, SS ss1, SS ss2, SS ss3, SS ss4 );
#endif

#else

SS  char_posicao( );
VOID qual_char( );
VOID exprechar( );
VOID insere_ctrl( );
SS   mar_atr( );
VOID retira_form( );
VOID salta_atr( );
VOID C_margens( );
VOID set_margens( );
SS   prim_linha( );
SS   pega_margens( );
VOID avalia_opt( );
SS   pega_strings( );
VOID set_limites( );
VOID sr_manager( );
VOID repita_busca( );
VOID repita_ft( );
SS   s_text( );
SS   s_line( );
VOID et_primeiro( );
VOID et_ultimo( );
VOID bot_remap( );
VOID scrap_inic( );
VOID scrap_free( );
VOID scrap_unlink( );
FILE *scrap_write( );
FILE *scrap_read( );
VOID initela( );
VOID fimtela( );
VOID leftscroll( );
VOID rightscroll( );
VOID et_matching( );
VOID m_frente( );
VOID m_traz( );
SS   prox_car( );
SS   ant_car( );
SS   lin_no_final( );
VOID C_d_word( );
SS   e_et_atr( );
US   blk_atr( );
US   atr_corrente( );
SS   tab_linha( );
SS   margem_linha( );
SS   q_atributos( );
SS   add_linbuffer( );
VOID diretivas( );
SS   cur_aparente( );
SS   edit_arquivo( );
SS   edit_buffer( );
SS   s_edit_buffer( );
VOID set_scr( );
VOID init_logic( );
VOID end_logic( );
VOID pos_line( );
VOID disp_stat( );
SS   next_letter( );
VOID C_cr( );
VOID C_bk_spc( );
VOID C_del( );
VOID C_tab( );
VOID to_text( );
VOID insert_line( );
VOID ins_l_after( );
VOID ins_l_bef( );
VOID delete_line( );
VOID split_line( );
VOID line_join( );
VOID C_up( );
VOID C_down( );
VOID C_right( );
VOID C_left( );
VOID C_home( );
VOID C_end( );
VOID C_pgup( );
VOID C_pgdn( );
VOID C_beg_file( );
VOID C_end_file( );
VOID C_shift_tab( );
VOID C_next_word( );
VOID C_prev_word( );
VOID C_line_blk( );
VOID C_col_blk( );
VOID C_chr_blk( );
VOID next_block( );
VOID I_right( );
VOID I_left( );
VOID blk_indent( );
VOID blk_mudatrans( );
SS   blk_sel( );
VOID blk_delete( );
VOID blk_move( );
VOID blk_copy( );
VOID blk_upplwr( );
VOID UL_lin_block( );
VOID UL_col_block( );
VOID UL_chr_block( );
VOID lin_applwr( );
VOID set_block( );
VOID del_lin_block( );
VOID copy_lin_block( );
VOID ins_lin_block( );
VOID move_lin_block( );
VOID del_col_block( );
VOID copy_col_block( );
VOID ins_col_block( );
VOID move_col_block( );
VOID del_chr_block( );
VOID copy_chr_block( );
VOID ins_chr_block( );
VOID move_chr_block( );
VOID move_scr( );
VOID linalloc( );
SS   scr_number( );
SS   quit_file( );
SS   read_file( );
VOID ins_markers( );
SS   save_file( );
VOID C_import( );
VOID C_export( );
VOID mem_stat( );
VOID C_ins( );
VOID insert_mode( );
VOID clrerror( );
SS   def_cmd( );
VOID sec_key( );
VOID build_key_tbl( );
VOID blk_ins( );
VOID blk_not( );
VOID C_blk_not( );
VOID line_go( );
VOID C_dup_line( );
VOID del_end_line( );
VOID total_wrap( );
US   text_wrap( );
VOID linha_anterior( );
VOID linha_posterior( );
VOID linha_visivel( );
VOID ins_linha( );
VOID ret_linha( );
VOID beep( );
VOID curslin( );
VOID cursblk( );
US   ajusta_array( );
ARG  *load_buffer( );
VOID free_buffer( );
SS   acha_paragrafo( );
SS   row_touch( );
VOID clin_touch( );
VOID C_down_scroll( );
VOID C_up_scroll( );
SS   pos_coluna( );
SL   tam_buffer( );
SS   lin_buffer( );
VOID C_zoom( );
SS   jan_buffer( );
VOID fresh_buffer( );

/*** Comandos de TECLADO ***/
SS   carga_coms( );
SS   carga_macs( );
SS   comp_diret( );
SS   setdefaults( );
SI   pega_token( );
VOID erro_par( );
SS   pegacomando( );
VOID arq_macro( );
VOID et_grava_macro( );
VOID et_exec_macro( );
VOID ajuda_editor( );
VOID exec_macroarq( );

VOID imprime_arq( );
SS   tira_br_fim( );
SS   br_to_tab( );
SS   exp_tabs( );
SS   longa_para( );

#ifdef   ET_UNDO
VOID start_undo( );
VOID restart_undo( );
VOID close_undo( );
VOID end_undo( );
VOID next_undo( );
VOID C_undo( );
VOID C_redo( );
VOID undo_linha( );
VOID undo_block( );
#endif

#endif

