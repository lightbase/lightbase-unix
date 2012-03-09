
#define   OP( x )   ((( x ) * 3) + 0)
#define   AT( x )   ((( x ) * 3) + 1)
#define   AJ( x )   ((( x ) * 3) + 2)

typedef struct {

struct  Window *win;   /* janela onde esta a lista de selecao.             */
struct  Window *men;   /* janela onde esta a ajuda estilo lotus            */
SC     *(*opcoes)[];   /* ptr para lista de elementos.                     */
                       /* ptr para lista de mensagems de cada item.        */
                       /* ptr para lista "0" inativo  "1" ativo "2" barra  */
SS      escolhido,     /* i/o  indice do escolhido e posicao p/ por tarja. */
        tecla,         /* ultima tecla.                                    */
        ok,            /* Indica se a estrutura foi inicializada OK!       */
        col,           /* Posicao no video da coluna esquerda.             */
        lin,           /* Posicao no video da borda de cima.               */
        v_lin,         /* numero de linhas na janela virtual.              */
        v_col,         /* numero de colunas na janela virtual.             */
        a_lin,         /* numero de linhas na janela aparente.             */
        a_col,         /* numero de colunas na janela aparente.            */
        tarja,         /* posicao da tarja dentro da janela.               */
        t_opcao,       /* largura dos elementos na janela.                 */
        n_elem;        /* numero de elementos                              */
UC      num_col,       /* numero de colunas                                */
        tipo,          /* 'M' => Menu   'E' => Escolha                     */
        l_atr,         /* atributo para listar os elementos                */
        t_atr,         /* atributo da tarja de selecao.                    */
        borda,         /* 1 SIMPLES, 2 DUPLAS                              */
        b_atr,         /* atributo da borda.                               */
        d_atr;         /* atributo destaque das opcoes                     */
SS      ajuda;         /* flag que indica se vai ter ajuda ou nao.         */
SC      *(*op_ajuda)[]; /* ptr para lista chaves para Help                 */
SC      *titulo;        /* Titulo                                          */
SC      *rodape;        /* Rodape                                          */
SS      (*comando)();  /* rotina chamada a cada item                       */
SS      (*executa)();  /* rotina chamada apos a esclha de um item          */

} Selecao;

#define   TARJAON    s->t_atr
#define   TARJAOFF   s->l_atr
#define   MENU_OK    1234
#define   MENU       'M'
#define   ESCOLHA    'E'  /* Nao possui mensagens nem opcoes desabilitadas */
#define   VIDEOCOL   def_win->w_len

#ifdef PROTO_ANSI

SS        menu(        Selecao  * );
SS        escolha(     Selecao  * );
Selecao  *def_menu(    Selecao *, SS, SS, SS, SS, SS,
                       SS, SS, SS, SS, SS, SC *(* )[], SC *, SC * );
Selecao  *def_escolha( Selecao *, SS, SS, SS, SS, SS,
                       SS, SS, SS, SS, SS, SC *(* )[], SC *, SC * );
VOID      ajuda_menu(  Selecao *, SC *(* )[] );

#else

SS        menu( );
SS        escolha( );
VOID      ajuda_menu( );
Selecao  *def_menu( );
Selecao  *def_escolha( );

#endif

