
#ifndef   ENTRADA

#define   ENTRADA    1
#define   MAX_EDIT   240
#define   ENT_OK     0x5A

/*** DEFINES DE D_ENTRADA - PARAMETRO TIPO ***/
#define   E_MOSTRA   0x8000L /* So Mostra */
#define   E_CONFIRMA 0x4000L /* Necessita de K_ENTER para confirar */
#define   E_LIMPA    0x2000L /* Limpa o campo se 1 tecla nao for SETAS */
#define   E_VALOR    0x0000L /* Entrada da direita para esquerda */
#define   E_ALFA     0x0001L
#define   E_DATA     0x0002L
#define   E_HORA     0x0003L
#define   E_LISTA    0x0004L /* Muda com BARRA de ESPACO */

/***  DEFINES DE PARAMETRO DO ED_READ()  ***/
#define   LIMPA_LE   0
#define   MOSTRA_LE  1
#define   SO_LIMPA   2
#define   SO_MOSTRA  3

typedef struct {

struct Window *win;  /* i   janela onde esta a entrada de dados            */
struct  Window *men; /*     janela onde esta a mensagem                    */
UC    ok;            /*     Sinaliza se a estrutura foi inicializada OK    */
SS    lin;           /* i   Linha do campo na Janela                       */
SS    col;           /* i   Coluna do campo na Janela                      */
US    tipo;          /* i   0 - Valor  1-AlfaNumerico  2-Data  3-Hora      */
UC    refresh;       /* i   indica se ENTRADA dara' REFRESH na SAIDA       */
UC    piece;         /* i   indica opcao atual em E_LISTA ...              */
                     /*     ... ou posicao decimal em tipo E_VALOR         */
UC    q_lista;       /* i   indica quant. de opcoes em E_LISTA             */
                     /*     ... ou se inicio e' 0 em E_VALOR               */
SC   *lista;         /* i   string com todas as opcoes E_LISTA             */
SS    tecla;         /* o   ultima tecla pressionada                       */
SS    cursor;        /* i-o posicao do cursor no campo ao entrar e sair    */
US    tam_m;         /*     Tamanho da mascara - Uso interno               */
SC   *str;           /* i-o string inicial a ser editado                   */
SS    visivel;       /* i   1a. coluna que aparecera na Janela Aparente    */
UC    mudou;         /* o   Informa se houve mudanca da STRING na EDICAO   */
SS    e_atr;         /* i   atributo de edicao                             */
SS    n_atr;         /* i   atributo de nao edicao                         */
UC   *escape;        /* i   caracteres de escape alem de ESC e ENTER       */
SC   *masc_alfa;     /* i   Mensagem exigindo alfabeticos                  */
SC   *masc_num;      /* i   Mensagem exigindo numericos                    */
SC   *mensagem;      /* i   Curta explicacao sobre o campo editado         */
SS   (*prepara)();   /* i   rotina de preparacao antes da edicao           */
SS   (*critica)();   /* i   rotina de critica apos edicao                  */
SS   (*crit_char)(); /*     rotina p/ critica de str para cada             */
                     /*     caracter digitado	                           */
SC   *mascara;       /* i   picture de edicao e tamanho Janela Virtual     */
                     /*     a - Alfabetico                                 */
                     /*     A - Alfabetico Minusculo -> Maiusculo          */
                     /*     x - Alfa Numerico                              */
                     /*     X - Alfa Numerico Minusculo -> Maiusculo       */
                     /*     9 - Numerico com "+-"                          */
                     /*     * - Alfa Numerico Escondido                    */
}  d_entry;


/*---------------------------------------------------------------------*/
/*  Definicao de TELA para ED_READ                                     */
/*---------------------------------------------------------------------*/

typedef struct {

   SC   *destino;
   SC   *mascara;
   SC   *rodape;
   SS   linha;
   SS   coluna;
	SS   visivel;
	US   param;    /* E_ALFA, E_VALOR ou E_LISTA */
   SC   *help;
   SC   *opcoes;
   SS  (*prep)();
   SS  (*crit)();

}  DTela;

#ifdef PROTO_ANSI

SS   entrada( d_entry  *, SS );
VOID d_entrada( d_entry *, WIN *, SC *, SS, SC *, SC *, SS, SS, US );
SS   ed_read( WIN *, DTela [], SS, SS, SC * );
VOID seta_dtela( DTela *DT, SC *destino, SC *mascara, SC *rodape,
                 SS linha, SS coluna, SS visivel, US param,
                 SC *help, SC *opcoes, SS (*prep)(), SS (*crit)() );

SC   *valor_importa( SC *, SC *, SC * );
SS   ent_valor( d_entry * );

SC   *alfa_importa( SC *, SC *, SC * );
SS   ent_alfa( d_entry *, SS );
SS   ent_lista( d_entry *, SS );

#else

SS   entrada( );
VOID d_entrada( );
SS   ed_read( );
VOID seta_dtela( );

SC   *valor_importa( );
SS   ent_valor( );

SC   *alfa_importa( );
SS   ent_alfa( );
SS   ent_lista( );

#endif

#endif


