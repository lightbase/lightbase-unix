
/*** Header Principal do Manipulador de Terminais ***/

#ifndef FLAG_WINDOW
#define FLAG_WINDOW  FW

#include <winport.h>

#ifdef MSDOS
#include <conio.h>
#endif


#define   MAX(x,y)        (((x)>(y))?(x):(y))
#define   MIN(x,y)        (((x)<(y))?(x):(y))
#define   COTA(a,x,b)     (MAX(MIN(b,x),a))
#define   INRANGE(a,x,b)  ((((x)<(a))||((x)>(b)))?0:1)
#define   OUTRANGE(a,x,b) ((((x)<(a))||((x)>(b)))?1:0)

/*** DEFINED PSEUDOFUNCTIONS  if p, expects ptr, if c expects UC ***/
#define is_atr( c )    (( c ) > 255 )
#define val_of( c )    (( c ) & 255 )
#define is_gr( c )     (( c ) & 128 )
#define gr_val( c )    (( c ) & 127 )
#define w_atr( c,atr ) (( c ) += ( atr ) << 8 )

/*#########################################################################*/

/****** THE MAIN WINDOW STRUCTURE ******/
/* x,y references 0,0 is always upper left corner */
struct Window
        { UC   w_num;                      /* relational number of window */
          US   w_len,w_height;              /* size of the vertual window */
          US   w_alen,w_aheight;              /* size of appearent window */
          US   w_x,w_y;   /* location of appearant window on display scn. */
          US   w_ax,w_ay;        /* location of appearent window on vert. */
          US   w_cx,w_cy;          /* location of cursor rel to appearant */
          UC   w_flags;                                     /* bit mapped */
          US   w_intr;         /* bit flag of which interrupts are active */
          UC   w_atrib;                         /* present atribute stack */
          UC   w_defatr;                              /* default atribute */
          UC   campoatr;                  /* Atributo de campos nao ativo */
          UC   w_optatr;                             /* optional atribute */
          UC   w_boxatr;                           /* atribute of any box */
          UC   w_curstyle;                      /* active style of cursor */
          US   *w_ary;                            /* array of window info */
          UC   w_anymod;           /* flag 0 = no mods 1 = some 2 = touch */
                     /* main_win ... 3 = clear +100 if window box changed */
          UC   *w_mod;  /* bit flag of window mods, only appearant mapped */
          SS w_modblock;              /* calculation needed for w_mod ary */
          UC type;                        /* Bordas - 1 SIMPLES, 2 DUPLAS */
          SC *title;                                            /* Titulo */
          SC *bottom;                                          /* Rodape' */
          UC t_atr;                                  /* atribute of Title */
          UC ident;         /* Forma para usuario saber qual suas janelas */
                            /* 255 => Janela de Help */
        };

typedef struct Window  WIN;

/*** PARA UTILITARIOS ***/

#define   U_EDITA       1L
#define   U_CALENDARIO  2L
#define   U_CALCULA     4L
#define   U_STATUS      8L
#define   U_DOS         16L
#define   U_SHELL       16L
#define   U_GRAVA       32L
#define   U_EXECUTA     64L
#define   U_REESCREVE   128L
#define   U_ABANDONA    256L
#define   U_CONFIGURA   512L
#define   U_MOVE        1024L
#define   U_TAMANHO     2048L
#define   U_VIRTUAL     4096L
#define   U_IMP_TELA    8192L
#define   U_IMP_JANELA  16384L
#define   U_ANTERIOR    32768L
#define   U_POSTERIOR   65536L



/*** PARA EDITOR ***/

#define   TAB_CHAR   9
#define   EDITA      1
#define   NAVEGA     2
#define   MOSTRA     3

/***  MACROS ***/
/*** Os 2 defines de baixo terao de serem diferentes */
#define   FILTRA_NL   1
#define   NAO_FILTRA  0

#define COMANDO     253
#define M_MENS      0
#define M_COMENT    1


/*** bit masks for w_flags ***/
#define W_BOXED 1                  /* smart lines are placed AROUND window */
/*#define W_IMED  2/*guaranteed no need for refresh but performance suffers*/
#define W_NOSCROL 4                                 /* scroll is inhibited */
#define W_TOTOP 8                  /* instead of scroling go to window top */
#define W_WRAP 16              /* at end of line, go to next ( else stop ) */
#define W_VISIB 32                  /* require that cursor pos, be visible */
#define W_HIGH 64         /* require that usage makes whole window visible */


/*** bit masks for w_intr ***/
#define INT_MOVE 1              /* window is user movable */
#define INT_SIZE 2              /* window size is user adjustable */
#define INT_AMOVE 4             /* vertual window movable reletive to appar. */
#define INT_TOSHELL 8           /* is "spawn temporary shell" operative */
#define INT_REFRESH 16          /* is "redraw the screen" operative */
#define INT_PRWIN 32            /* is "print window" operative */
#define INT_PRSCN 64            /* is "print screen" operative */
#define INT_QUIT 128            /* is it ok for user to abort program */


/****** EXTERNAL VARIABLE GROUP ******/

/* Deverao ter nomes das variaveis ambientais para terminal e diretorio */
/* Estas variaveis sao realmente EXTERN, sao declaradas em WMAIN0.C     */
extern SC *env_term;
extern SC *env_dir;

#ifdef WINMAIN
#define extern
#endif

/* Estas variaveis sao EXTERN apenas quando WINMAIN nao esta definida */
/* WINMAIN E' Definida em WMAIN.C                                     */

extern SS tec_util;   /* Default K_F3 */
extern SS tec_ajuda;  /* Default K_F1 */
extern UL util_flag;  /* Default tudo ligado */
extern SC str_vazia[2]; /* Sera colocado '\0' na inicializacao */
extern UC *win_params;/* the list of the parameters */
extern UC fresh_read; /* Indica se MT podera' esta executando refresh */
extern SC w_refresh;  /* Quando FALSE -> EXECUTA refresh */
                      /*        <= 10 -> edit_buffer() colocara 0  */
extern SC *versao;    /* Versao mostrada em STATUS */
extern SC *edicao;    /* Edicao mostrada em STATUS */
extern WIN *hm_win;   /* Janela onde serao colocados HH:MM */
extern SS hm_x, hm_y; /* Cordenadas X, Y em hm_win */
extern SS hm_seg;     /* Delta em segundos para atualizacao de hm_win */
extern SC *hm_fmt;    /* Formato de HH:MM em hm_win */
extern SL hm_time;    /* Segundos retornado por time( ) */
extern UC hm_ok;      /* Permite impressao do relogio */
extern SC sep_int;    /* Separador Inteiro em campo numerico 1.234 */
extern SC sep_dec;    /* Separador Decimal em campo numerico 0,123 */
extern FILE *rd_macro;/* Leitura automatica  em fwnqgc() - wget.c */
extern FILE *wr_macro;/* Gravacao automatica em fwnqgc() - wget.c */
extern SS w_lastread; /* armazana o ultimo campo lido em ED_READ() */
extern SC *terminal;  /* Contera getenv( env_term ) default DOS_MS ou ANSI */
extern SC *diretorio; /* Contera getenv( env_dir ) default ".\"    ou "./" */
extern UC insercao;   /* Modo insercao ou sobreposicao */
extern SC *win_ajuda; /* aponta para string que a funcao f_ajuda( ) pegara' */
extern SC *dir_ajuda; /* Diretorio opcional de AJUDA */
extern SC *user_help; /* Quando setada, tem preferencia sobre win_ajuda */
extern SC edit_help;  /* Quando setada, permite edicao de help */
/***
   if ( w_linemod[ 0 ][ linha ] == 0 )  linha nao foi alterada
   else    linha foi alterada ate' a coluna indicada, margem direita
   Se linha alterada w_linemod[ 1 ] indica que  linha foi alterada desde a
      coluna indicada, margem esquerda
***/
extern UC w_linemod[ 2 ][ MAXHEIGHT ];
extern US prbuf[ MAXLENG ];
extern SS bufend;
extern SS last_ug;             /* the graphic  that was */

extern WIN *win_stak[ MAXWIN ];
extern SS win_sp;

#ifdef    MSDOS
extern SS (*fun_config)(WIN *);/* Funcao que sera chamada na opcao ... */
                               /* ... CONFIGURACAO em INTERRUPCAO.     */
extern SS (*quitprog)(void );  /* quitprog is run by win_exit if it exists */
                               /* if it's 0, it is not                     */
                               /*  WIN_EXIT IS WIRED TO SIGNAL SIGTERM     */
                               /*  during win_init process                 */
extern SS (*move_win)(WIN *);  /* move_win e' chamada toda vez que janela  */
                               /* for movida uma mudada de tamanho         */
#else
extern SS (*fun_config)( );
extern SS (*quitprog)();
extern SS (*move_win)();
#endif

extern UC *win_map;                      /* which windows own which pixels */
extern UC badary[MAXHEIGHT][MAXLENG / 8];          /* array of atrib marks */
extern UC bad_atribs[MAXATRIB + 1];        /* list of non-simple atributes */
extern SS win_dominant;      /* -1 no domin ant window, else # of dominant */
                            /* window is dominant if it owns 80% of pixels */
extern struct Window *def_win;           /* the window which is "standard" */
extern struct Window *main_win;                /* the screen immage window */
extern struct Window *curr_win;         /* the window currently being used */
extern SS tcp_x, tcp_y;                    /* theoretical current position */
#define   TAM_STTY_G   160
extern SC stty_orig[ TAM_STTY_G + 5 ];              /* return of stty -g */
extern UC get_flag;                                   /* getting control */
/* extern UC get_range[200]; */ /* list of acceptable input for WG_RANGE */


/*** bit masks for get_flag ***/
/* #define WG_NOECHO 1  */ /* getting functions don't echo */
/* #define WG_TOUP   2  */ /* getting functions convert to upper case */
/* #define WG_NOREF  4  */ /* don't refresh the window at every get */
/* #define WG_RANGE  8  */ /* only accepts chars in get_range string */
#define WG_IMPAT     16    /* if part match found, don't wait for the rest */
#define IMPATIENCE   10    /* # of kbhit's to try before becoming impatient */


/****** TERMINAL DEFINITION GROUP ******/

extern UC *init_seq;
extern UC *init_proc;           /* if process exists it will be involked
                                                        as a system() call */
extern UC *init_stty;        /* if stty sequence exists, it will be set,
                                  status returned to "as it was" when done */
extern UC *term_seq;
extern UC *term_proc;                                  /* as init_proc[] */

extern UC size_x, size_y;                   /* leng & height of terminal */
extern UC lc_scrolls;                     /* Last Character scrolls flag */
extern UC mov_cur[20];            /* cursor move string ( as per printf) */
extern UC cur_order;                   /* 0 - x before y else y before x */
extern SS  xplus_const;                                   /* inc x & y by */
extern SS  yplus_const;
extern SS  strange_term; /* if TRUE then custom code exists in W_curmove.c */

extern UC *go_home;
extern UC *go_up;
extern UC *go_dn;
extern UC *go_rt;
extern UC *go_lft;
extern UC go_upx[10];                 /* move more than 1 space on a line */
extern UC go_dnx[10];
extern UC go_rtx[10];
extern UC go_lftx[10];
extern UC *go_nl;                     /* go to start of next line */
extern UC *do_eol;            /* from pres curs pos to eol inclusive */
extern UC *do_eos;    /* cursor will be at first column of line to clear */
extern UC *do_cls;              /* cursor needn't be homed, must be "on" */
extern UC *do_bs;
extern UC *do_insline;                                /* insert a line */
extern UC *do_delline;                                /* delete a line */
extern UC *do_scroll;
extern UC *do_unscroll;
extern UC *do_pan;
extern UC *do_unpan;

/*** user graphics interface ***/

struct start_end_ug
{
   UC   *start;  /* Sequencia que abre semi-graficos e caracters especiais */
   UC   *end;    /* Sequencia que fecha */
};
extern struct start_end_ug SE[ MAX_SE + 1 ]; /* Zero nao usa */

struct ug_struct
{
   UC   *ary;    /* Sequencia a ser enviada */
   UC   type;    /* Tipo de sequencia que inicia e termina tal caracter */
};
extern struct ug_struct UG[ MAXGRAPH ];

#define   SIMPLES  1      /* Tipo da borda */
#define   DUPLAS   2

#define UG_LF     10      /* Line Feed */

/* Quantidade de UG_ de tal forma que UG_VERT + UG_LEN seja UG_VERT2 */
#define UG_LEN    12

#define UG_VER    129     /* vertical member ( | ) */
#define UG_HOR    130     /* horizontal member ( -- ) */
#define UG_UL     131     /* upper left corner piece */
#define UG_UR     132     /* upper right corner piece */
#define UG_LR     133     /* lower right corner piece */
#define UG_LL     134     /* lower left corner piece */
#define UG_TU     135     /* T with point up ( _|_ ) */
#define UG_TR     136     /* ( |- ) */
#define UG_TD     137     /* ( T ) */
#define UG_TL     138     /* ( -| ) */
#define UG_JUN    139     /* ( -|- ) */
#define UG_DOT    140     /* single graphic */
#define UG_VER2   141     /* DUPLO vertical member ( | ) */
#define UG_HOR2   142     /* DUPLO horizontal member ( -- ) */
#define UG_UL2    143     /* DUPLO upper left corner piece */
#define UG_UR2    144     /* DUPLO upper right corner piece */
#define UG_LR2    145     /* DUPLO lower right corner piece */
#define UG_LL2    146     /* DUPLO lower left corner piece */
#define UG_TU2    147     /* DUPLO T with point up ( _|_ ) */
#define UG_TR2    148     /* DUPLO ( |- ) */
#define UG_TD2    149     /* DUPLO ( T ) */
#define UG_TL2    150     /* DUPLO ( -| ) */
#define UG_JUN2   151     /* DUPLO ( -|- ) */
#define UG_DOT2   152     /* single graphic */
#define UG_SET_D  153
#define UG_SET_E  154
#define UG_SET_C  155
#define UG_SET_B  156

#define UG_SP        160  /* FALSO SPACE para EDITOR no DOS deve ser 250 */
#define UG_EXCLA_INV 161
#define UG_PARA      167
#define UG_TREMA     168
#define UG_ORD_a     170
#define UG_MAISMEMOS 177
#define UG_ORD_o     186
#define UG_INTER_INV 191
#define UG_GRA_A     192
#define UG_AGU_A     193
#define UG_CIR_A     194
#define UG_TIL_A     195
#define UG_TRE_A     196
#define UG_BOL_A     197
#define UG_CED_C     199
#define UG_GRA_E     200
#define UG_AGU_E     201
#define UG_CIR_E     202
#define UG_TRE_E     203
#define UG_GRA_I     204
#define UG_AGU_I     205
#define UG_CIR_I     206
#define UG_TRE_I     207
#define UG_TIL_N     209
#define UG_GRA_O     210
#define UG_AGU_O     211
#define UG_CIR_O     212
#define UG_TIL_O     213
#define UG_TRE_O     214
#define UG_COR_O     216
#define UG_GRA_U     217
#define UG_AGU_U     218
#define UG_CIR_U     219
#define UG_TRE_U     220
#define UG_BETA      223
#define UG_GRA_a     224
#define UG_AGU_a     225
#define UG_CIR_a     226
#define UG_TIL_a     227
#define UG_TRE_a     228
#define UG_BOL_a     229
#define UG_CED_c     231
#define UG_GRA_e     232
#define UG_AGU_e     233
#define UG_CIR_e     234
#define UG_TRE_e     235
#define UG_GRA_i     236
#define UG_AGU_i     237
#define UG_CIR_i     238
#define UG_TRE_i     239
#define UG_TIL_n     241
#define UG_GRA_o     242
#define UG_AGU_o     243
#define UG_CIR_o     244
#define UG_TIL_o     245
#define UG_TRE_o     246
#define UG_COR_o     248
#define UG_GRA_u     249
#define UG_AGU_u     250
#define UG_CIR_u     251
#define UG_TRE_u     252
#define UG_MACDEM    253
#define UG_ETPARA    254

/* atributes */

struct atrib_defn
        { UC *at_on;
          UC *at_off;
          UC at_onlen;         /* number of bits consumed by turning on */
          UC at_offlen;       /* number of bits consumed by turning off */
          UC at_ideo;                  /* strange ways of this atribute */
        };

extern struct atrib_defn atrib_ary[MAXATRIB];

/*** standard atribute defines ***/

#define   L_MENS  0
#define   J_PRIN  1
#define   J_SEGU  2
#define   M_PRIN  3
#define   M_SEGU  4
#define   J_ERRO  5
#define   F_ESPE  6

#define   ATR_NATURAL  0
#define   ATR_NORMAL   0

#define   ATR_REVERSO  1
#define   ATR_STANDOUT 1

#define   ATR_BLINK    2
#define   ATR_ALERT    2
#define   ATR_PISCANTE 2

#define   ATR_INTENSO  3
#define   ATR_BOLD     3

#define   LM_CAMPO     0
#define   LM_BARRA     1
#define   LM_ERRO      2
#define   LM_DESTAQUE  3
#define   JP_JANELA    4
#define   JP_CAMPO     5
#define   JP_BARRA     6
#define   JP_DESTAQUE  7
#define   JS_JANELA    8
#define   JS_CAMPO     9
#define   JS_BARRA     10
#define   JS_DESTAQUE  11
#define   MP_JANELA    12
#define   MP_CAMPO     13
#define   MP_BARRA     14
#define   MP_DESTAQUE  15
#define   MS_JANELA    16
#define   MS_CAMPO     17
#define   MS_BARRA     18
#define   MS_DESTAQUE  19
#define   JE_JANELA    20
#define   JE_CAMPO     21
#define   JE_BARRA     22
#define   JE_DESTAQUE  23
#define   FE_JANELA    24
#define   FE_CAMPO     25
#define   FE_BARRA     26
#define   FE_DESTAQUE  27

/* key conversions */
struct kcstruct
        { UC *kc_from;                       /* if terminal sends this */
          UC *kc_to;                     /* program will recieve this */
          UC *kc_dsc;            /* this is user prompt describing key */
        };

extern struct kcstruct kc_ary[ MAXCONV ];
extern SS kc_sp;
extern UC kc_fary[ MAXCONV + 1 ];        /* list of kc_ary->kc_first[0] */

#define K_ENTER      '\n'
#define K_BS         '\b'
#define K_TAB        '\t'

/*** standard key conversion defines ***/

#define K_INTR    128    /* interrupt key */
#define K_ESC     129    /* escape key */
#define K_AJUDA   130    /* HELP KEY */
#define K_UP      131    /* up arrow key */
#define K_DN      132
#define K_RT      133
#define K_LFT     134
#define K_HOME    135
#define K_END     136
#define K_PGUP    137
#define K_PGDN    138
#define K_INS     139
#define K_DEL     140
#define K_F0      141    /* F0 or F10, F1 - F4 most used */
#define K_F1      142
#define K_F2      143
#define K_F3      144
#define K_F4      145
#define K_F5      146
#define K_F6      147
#define K_F7      148
#define K_F8      149
#define K_F9      150

/* note, key descriptions may be found w/ win_kdsc() command */
/* ABOVE 149 Freely user defined */

#define K_ALT_F0    151
#define K_ALT_F1    152
#define K_ALT_F2    153
#define K_ALT_F3    154
#define K_ALT_F4    155
#define K_ALT_F5    156
#define K_ALT_F6    157
#define K_ALT_F7    158
#define K_ALT_F8    159

#define K_SP        160     /* reservado para Falso Branco no BRASCII */
#define K_EXCLA_INV 161
#define K_ALT_A     162     /* Cents */
#define K_ALT_B     163     /* Libra */
#define K_WORDLEFT  164     /* Bolinha com quatro pontas no BRASCII */
#define K_WORDRIGHT 165     /* Simbolo YEN no BRASCII */
#define K_ALT_F9    166     /* Vertical */
#define K_PARA      167
#define K_TREMA     168
#define K_ALT_C     169     /* Copyright */
#define K_ORD_a     170
#define K_ALT_D     171     /* << */
#define K_CPGDN     172     /* Barra com quebra no BRASCII */
#define K_CPGUP     173     /* Caracter SHY no BRASCII */
#define K_ALT_E     174     /* Registrada */
#define K_BTAB      175     /* Traco horizontal superior no BRASCII */
#define K_ALT_F     176     /* Bolinha */
#define K_MAISMENOS 177
#define K_ALT_G     178     /* Elevado a 2 */
#define K_ALT_H     179     /* Elevado a 3 */
#define K_ALT_I     180     /* Acento Agudo */
#define K_ALT_J     181     /* Micro */
#define K_ALT_K     182     /* P invertido - Fim de Paragrafo ? */
#define K_ALT_L     183     /* Pequeno Quadrado */
#define K_ALT_M     184     /* Virgula */
#define K_ALT_N     185     /* Elevado a 4 */
#define K_ORD_o     186
#define K_ALT_O     187     /* >> */
#define K_ALT_P     188     /* 1/4 */
#define K_ALT_Q     189     /* 1/2 */
#define K_ALT_R     190     /* 3/4 */
#define K_INTER_INV 191
#define K_GRA_A     192
#define K_AGU_A     193
#define K_CIR_A     194
#define K_TIL_A     195
#define K_TRE_A     196
#define K_BOL_A     197
#define K_ALT_S     198     /* AE */
#define K_CED_C     199
#define K_GRA_E     200
#define K_AGU_E     201
#define K_CIR_E     202
#define K_TRE_E     203
#define K_GRA_I     204
#define K_AGU_I     205
#define K_CIR_I     206
#define K_TRE_I     207
#define K_ALT_T     208     /* D Cortado */
#define K_TIL_N     209
#define K_GRA_O     210
#define K_AGU_O     211
#define K_CIR_O     212
#define K_TIL_O     213
#define K_TRE_O     214
#define K_ALT_U     215     /* X - Sem identificacao */
#define K_COR_O     216
#define K_GRA_U     217
#define K_AGU_U     218
#define K_CIR_U     219
#define K_TRE_U     220
#define K_ALT_V     221     /* Y Agudo */
#define K_ALT_W     222     /* P Estilizado */
#define K_BETA      223
#define K_GRA_a     224
#define K_AGU_a     225
#define K_CIR_a     226
#define K_TIL_a     227
#define K_TRE_a     228
#define K_BOL_a     229
#define K_ALT_X     230     /* ae */
#define K_CED_c     231
#define K_GRA_e     232
#define K_AGU_e     233
#define K_CIR_e     234
#define K_TRE_e     235
#define K_GRA_i     236
#define K_AGU_i     237
#define K_CIR_i     238
#define K_TRE_i     239
#define K_ALT_Y     240     /* d Cortado */
#define K_TIL_n     241
#define K_GRA_o     242
#define K_AGU_o     243
#define K_CIR_o     244
#define K_TIL_o     245
#define K_TRE_o     246
#define K_ALT_Z     247     /* Dividido */
#define K_COR_o     248
#define K_GRA_u     249
#define K_AGU_u     250
#define K_CIR_u     251
#define K_TRE_u     252
#define K_COM_MAC   253
#define K_ETPARA    254

/* cursor styles */

extern UC *curstyle[MAXCURS];
#define CS_NORMAL 0             /* natural cursor */
#define CS_OFF 1                /* no cursor */
#define CS_HIDDEN 2             /* cursor not on visible window */
#define CS_INTER 3              /* cursor style when in interrupt mode */

/* screen printer control */
extern UC sp_rp;              /* redirect or pipe, flag */
extern SC *sp_dest;           /* destination to print to */
extern UC sp_atrib;           /* send atributes? flag */
extern UC sp_graph;           /* send graphics? flag */
extern UC *sp_eol;            /* end of line sequence */
extern UC *sp_eop;            /* end of page sequence */

/* bell control */
extern UC *bel_ary[ MAXBELLS ];
#define BELPAUSE 10000          /* pause count on reciept of ^A */

/*** standar bell defines ***/

#define BEL_GEN 1               /* general purpose bell */
#define BEL_VALID 2             /* valid entry received bell */
#define BEL_INVALID 3           /* invalid entry bell */
#define BEL_LONGJOB 4           /* extended job complete bell */
#define BEL_ATTN 5              /* attention required */
#define BEL_EMERGENCY 6         /* emergency bell */

#ifdef    ST_DEBUG

#include  "mensagem.h"

#define stak1( w, a ) if( win_stak[ w->w_num ] != w )\
   { fprintf(stderr,"\n%s, %s",a,loc_str(STR_DISCO,M_STDEBUG)); return; }

#define stak2( w, a ) if( win_stak[ w->w_num ] != w )\
   { fprintf(stderr,"\n%s, %s",a,loc_str(STR_DISCO,M_STDEBUG)); return -1; }

#define stak3( w, a ) if( win_stak[ w->w_num ] != w )\
   { fprintf(stderr,"\n%s, %s",a,loc_str(STR_DISCO,M_STDEBUG)); exit( 1 ); }

#else

#define stak1( w, a ) /***   NADA  ***/

#define stak2( w, a ) /***   NADA  ***/

#define stak3( w, a ) /***   NADA  ***/

#endif


#ifdef DB_DEBUG

extern SS dx1;
extern SS dx2;
extern SS dx3;

#define db1( a )  if( dx1 ) fprintf( stderr,"[%s]\n",a )
#define db2( a )  if( dx2 ) fprintf( stderr,"{%s}\n",a )
#define db3( a )  if( dx3 ) fprintf( stderr,"\t\t%s\n",a )

#else

#define db1( a )  /* if( dx1 ) fprintf( stderr,"\n[%s]",a ) */
#define db2( a )  /* if( dx2 ) fprintf( stderr,"\n{%s}",a ) */
#define db3( a )  /* if( dx3 ) fprintf( stderr,"\t\t%s",a ) */

#endif


/******* PROTOTIPOS  *******/

#ifdef PROTO_ANSI

VOID hm_halt( void );
VOID hm_alarme( void );
VOID hm_set( WIN *win, SS x, SS y, SS seg, SC *fmt );
SC   *tira_separador( SC *dir );
VOID mvwprintw( WIN *win, SS Y, SS X, SC *fmt, ... );
VOID w_movmem( UC *from, UC *to, SS size );
VOID w_movint( US *from, US *to, SS size );
VOID w_setmem( UC *from, US len, US value );
VOID w_setint( US *from, US len, US value );
SC   *w_memmove( SC *dest, SC *orig, US tam );
US   dayofwk( US, US, US );
US   leapyear( US );
US   valdate( US, US, US );
SC   *ins_brancos( SC *str, SS quant, SS carac );
SC   *just_line( SC *dest, SC *orig, SS tam );
SS   p_just_line(SC *dest,SC *orig,SS tam,SS len, SS chr, SS pos, SS *off);
SS   w_rename( SC *atual, SC *futuro );
SI   w_stricmp( SC *, SC * );
SI   w_strncmp( SC *, SC *, SS );
SC   *w_strlwr( SC * );
SC   *w_strupr( SC * );
SC   *w_strstr( SC *s1, SC *s2 );
SC   *strip_nl( SC * );
SC   *trim( SC  * );
SC   *ltrim( SC  * );
DB   str_to_db( SC *, SS );
SC   *db_to_str( DB, SC *, SS, SS );
SS   interrompe_macro( void );
SC   *formata_campo( SC *, SC *, SC *, US );
VOID win_shell( SC * );
SS   w_navega( WIN *, WIN *, SS, SC * );
SC   *mr_fgets( SC * );
SS   mr_read( UC *, SS );
VOID analisa_demo( WIN * );
SS   mr_fim( void );
SS   mr_inic( SC * );
SS   mw_fim( void );
SS   mw_inic( SC *, SS );
VOID mw_obs( SS, SC * );
SC   *mw_puts( SC * );
SS   mw_putc( UC );
SS   decida( SC *, SC *, SC *, SS, SS, SC *, ... );
VOID w_kbd ( SI, ... );
VOID set_cores( SS, WIN * );
VOID pr_escape( SC *, UC * );
SS   piece ( SC *, SC *, SS, SS, SS );
SC   *funcao( SC * );
SS   pede_cadeia( SC *prompt, SC *string, SS tam, SC *mens,
                  SC masc_char, SS (*critica)() );
SS   men_conf( SC *texto, SC *opcoes, SC *ajuda1, WIN *win );
SS   calc_avalia( SC *, DB *, WIN *, SS (* )(), SS );
VOID calculadora( void );
VOID f_ajuda( SC *, SS flagmenu );
VOID quick_sort( SC *, US, US, SS (* )() );
VOID win_bell( SS );
VOID win_redo( void );
VOID fwrefresh( WIN * );
VOID win_refresh( void );
VOID win_redo( void );
VOID fwcls( WIN * );
VOID fwcle( WIN * );
VOID fwcll( WIN * );
VOID fwsetatr( UC, US, WIN * );
VOID fwmove( US, US, WIN * );
VOID fwamove( US, US, WIN * );
VOID fwsize( US, US, WIN * );
VOID fwbox( WIN * );
VOID fwunbox( WIN * );
VOID fwframe( SS, SS, SS, SS, SS, WIN * );
VOID fw_title( WIN *, SC *, SS );
VOID fw_bottom( WIN *, SC * );
VOID win_csset( UC );
SS   win_cpush( SS );
SS   win_cpop( void );
VOID fwsetcursor( WIN * );
VOID seta_insert( SS );
US   minuscula( US );
US   maiuscula( US );
US   para_maiuscula( US );
US   exibivel( US );
US   e_print( US );
US   e_branco( US );
US   e_grafico( US );
SS   w_rgc( WIN * );
SS   w_ungc( UC );
VOID w_ungs( SC * );
SS   fwqgc( WIN * );
SS   fwnqgc( WIN * );
SS   fwgc( WIN * );
SS   wq_kbhit( WIN * );
SS   w_kbhit( WIN * );
SC  *win_kdsc( UC );
SS   win_init( SC *arq_men, SS );
VOID win_quit( void );
VOID win_exit( SS );
VOID setconfig( SS (* ) () );
VOID setmove( SS (* ) () );
VOID setquit( SS (* ) () );
WIN *win_make( SS, SS, SS, SS, SS, SS );
SS   fwkill( WIN * );
VOID fwtop( WIN * );
VOID fwbot( WIN * );
SS   fwintr( WIN *, UL );
VOID doputs( UC * );
VOID doputch( UC );
VOID doflush( void );
VOID insercol( WIN *, SC *, ... );
VOID fecha_insercol( WIN * );
VOID fecha_mensagem( WIN * );
VOID mens_1( WIN *,  SC *, ... );
VOID mens_2( WIN *,  SC *, ... );
VOID mens_3( WIN *,  SC *, ... );
WIN *abre_insercol( SC *, ... );
WIN *abre_mensagem( SC *, ... );
SS   mens_erro( SC *, SC *, ... );
VOID funcoes( WIN *, SC *, ... );
WIN *abre_funcoes( SC *, ... );
VOID fecha_funcoes( WIN * );
VOID set_mensagem( SS );
VOID set_funcao( SS );
VOID set_erro( SS );
SS   debug( SC *, ... );
VOID fwprintf( WIN *, SC *, ... );
VOID fwprscrn( WIN * );
VOID fwpsl( UC *, WIN *, SS );
VOID fwputs( UC *, WIN * );
VOID fwpc( UC, WIN * );
VOID fwat( SS, SS, WIN * );
VOID fwscroll( SS, WIN * );
VOID fwpan( SS, WIN * );
VOID fwinsline( WIN * );
VOID w_fixmain( void );
VOID tipo_cursor_dos( SC * );
VOID dos_mem( US, US, US * );
SS   atr_of( US );
VOID prclear( void );
VOID wu_mark( WIN * );
VOID wu_lmark( WIN *, SS );
VOID wu_update( WIN * );
VOID wu_remap( SS, SS, SS, SS );
VOID W_curmove( SS, SS );
VOID WW_curmove( SS, SS );
SC   *eliminabr( SC *str );
VOID ate_nl( FILE *F );
SC   *tirabr( SC *str );

#else

VOID hm_halt( );
VOID hm_alarme( );
VOID hm_set( );
SC   *tira_separador( );
VOID mvwprintw( );
VOID w_movmem( );
VOID w_movint( );
VOID w_setmem( );
VOID w_setint( );
SC   *w_memmove( );
US   dayofwk( );
US   leapyear( );
US   valdate( );
SC   *ins_brancos( );
SC   *just_line( );
SS   p_just_line( );
SS   w_rename( );
SI   w_stricmp( );
SI   w_strncmp( );
SC   *w_strlwr( );
SC   *w_strupr( );
SC   *w_strstr( );
SC   *strip_nl( );
SC   *trim( );
SC   *ltrim( );
DB   str_to_db( );
SC   *db_to_str( );
SS   interrompe_macro( );
SC   *formata_campo( );
VOID win_shell( );
SS   w_navega( );
SC   *mr_fgets( );
SS   mr_read( );
VOID analisa_demo( );
SS   mr_fim( );
SS   mr_inic( );
SS   mw_fim( );
SS   mw_inic( );
VOID mw_obs( );
SC   *mw_puts( );
SS   mw_putc( );
SS   decida( );
VOID w_kbd ( );
VOID set_cores( );
VOID pr_escape( );
SS   piece ( );
SC   *funcao( );
SS   pede_cadeia( );
SS   men_conf( );
SS   calc_avalia( );
VOID calculadora( );
VOID f_ajuda( );
VOID quick_sort( );
VOID win_bell( );
VOID win_redo( );
VOID fwrefresh( );
VOID win_refresh( );
VOID win_redo( );
VOID fwcls( );
VOID fwcle( );
VOID fwcll( );
VOID fwsetatr( );
VOID fwmove( );
VOID fwamove( );
VOID fwsize( );
VOID fwbox( );
VOID fwunbox( );
VOID fwframe( );
VOID fw_title( );
VOID fw_bottom( );
VOID win_csset( );
SS   win_cpush( );
SS   win_cpop( );
VOID fwsetcursor( );
VOID seta_insert( );
US   minuscula( );
US   maiuscula( );
US   para_maiuscula( );
US   exibivel( );
US   e_print( );
US   e_branco( );
US   e_grafico( );
SS   w_rgc( );
SS   w_ungc( );
VOID w_ungs( );
SS   fwqgc( );
SS   fwnqgc( );
SS   fwgc( );
SS   wq_kbhit( );
SS   w_kbhit( );
SC  *win_kdsc( );
SS   win_init( );
VOID win_quit( );
VOID win_exit( );
VOID setconfig( );
VOID setmove( );
VOID setquit( );
WIN *win_make( );
SS   fwkill( );
VOID fwtop( );
VOID fwbot( );
SS   fwintr( );
VOID doputs( );
VOID doputch( );
VOID doflush( );
VOID insercol( );
VOID fecha_insercol( );
VOID fecha_mensagem( );
VOID mens_1( );
VOID mens_2( );
VOID mens_3( );
WIN *abre_insercol( );
WIN *abre_mensagem( );
SS   mens_erro( );
VOID funcoes( );
WIN *abre_funcoes( );
VOID fecha_funcoes( );
VOID set_mensagem( );
VOID set_funcao( );
VOID set_erro( );
SS   debug( );
VOID fwprintf( );
VOID fwprscrn( );
VOID fwpsl( );
VOID fwputs( );
VOID fwpc( );
VOID fwat( );
VOID fwscroll( );
VOID fwpan( );
VOID fwinsline( );
VOID w_fixmain( );
VOID tipo_cursor_dos( );
VOID dos_mem( );
SS   atr_of( );
VOID prclear( );
VOID wu_mark( );
VOID wu_lmark( );
VOID wu_update( );
VOID wu_remap( );
VOID W_curmove( );
VOID WW_curmove( );
SC   *eliminabr( );
VOID ate_nl( );
SC   *tirabr( );

#endif

#endif    /* WINDOW defined to make double including ok */



