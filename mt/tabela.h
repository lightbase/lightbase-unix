
#include  "entrada.h"

#define   ALTERA   1
#define   INSERE   2
#define   DELETA   4

#define   T_ENTER  1
#define   T_LETRA  2

typedef struct {

WIN     *win;          /* janela onde estara' a lista de selecao.          */
WIN     *fun;          /* janela onde estara' as FUNCOES Disponiveis       */
d_entry *e;            /* Estrutura de entrada de dados                    */
SC      *(*opcoes)[];  /* ptr para lista de elementos.                     */
                       /* ptr para lista de mensagems de cada item.        */
                       /* ptr para lista "0" inativo  "1" ativo "2" barra  */
SS      escolhido,     /* i/o  indice do escolhido e posicao p/ por tarja. */
        tecla,         /* ultima tecla.                                    */
        tarja,         /* posicao da tarja dentro da janela.               */
        n_elem,        /* numero de elementos                              */
        n_max,         /* numero maximo de elementos                       */
        t_opcao,       /* largura dos elementos na janela.                 */
        ok,            /* Indica se a estrutura foi inicializada OK!       */
        col,           /* Posicao no video da coluna esquerda.             */
        lin,           /* Posicao no video da borda de cima.               */
        v_lin,         /* numero de linhas na janela virtual.              */
        v_col,         /* numero de colunas na janela virtual.             */
        a_lin,         /* numero de linhas na janela aparente.             */
        a_col,         /* numero de colunas na janela aparente.            */
        l_atr,         /* atributo para listar os elementos                */
        t_atr,         /* atributo da tarja de selecao.                    */
        borda,         /* 1 SIMPLES, 2 DUPLAS                              */
        b_atr,         /* atributo da borda.                               */
        d_atr;         /* atributo destaque das opcoes                     */
UC      enter;         /* Permite escolher com K_ENTER e letras mausculas  */
                       /* Bit 0 K_ENTER - Bit 2 Letras                     */
UC      edita;         /* Permite EDICAO da TABELA                         */
UC     *escape;        /* caracteres de escape alem de ESC e ENTER         */
SC     *dest;          /* Se for diferente de NULL, COPIA AUTOMATICAMENTE  */
SC     *mascara;       /* Mascara do Item                                  */
SC     *mens_ins;      /* Mensagem de insercao durante edicao da tabela    */
SC     *mens_alt;      /* Mensagem de alteracao durante edicao da tabela   */
SC     *fun_ins;       /* Funcoes habilitadas durante insercao da tabela   */
SC     *fun_alt;       /* Funcoes habilitadas durante alteracao da tabela  */
SC     *titulo;        /* Titulo                                           */
SC     *rodape;        /* Rodape                                           */
SS    (*comando)( );   /* rotina chamada apos tarjar, -1 caracteriza ENTER */
SS    (*executa)( );   /* rotina chamada apos a esclha de um item          */
                       /* E' Chamada sempre que houver alteracao na TABELA */
                       /*  ou o usuario escolher um item.                  */
                       /* Se retornar -1 Confirma operacao e sai da TABELA */
                       /* Se retornar -2 Confirma operacao mas NAO SAI ""  */
                       /* Outro retorno atribue a t->escolhido NAO CONFIRMA*/

} Tab;

#define   T_ON       t->t_atr
#define   T_OFF      t->l_atr
#define   MENU_OK    1234

#ifdef PROTO_ANSI

SS    tabela( Tab * );
Tab  *def_tabela( Tab *, SS, SS, SS, SS, SS, SS, SC *,
                    SS, SS, SS, SS, SS, SC *(* )[], SC *, SC * );
#else

SS    tabela( );
Tab  *def_tabela( );

#endif

