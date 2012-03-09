#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>
#include  <errno.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2prot1.h"

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

#define   TAM_TAB   ap_maxitens
#define   TAM_LINHA 100


#define   DEFINE     'D'
#define   CARREGA    'C'
#define   SELECAO    'S'
#define   FINALIZA   'F'

#define   GRP_TABS  19

#define   H_DEFTAB       "DEF_TAB"     /* definindo tabela */
#define   H_SELTAB       "SEL_TAB"     /* selecionando item da tabela */
#define   H_F2TAB        "CFGRVTAB"    /* Confirmacao de gravacao de tabela (alterada) */


#define   M_SELTB    loc_str(GRP_TABS,  1)
#define   M_INSTB    loc_str(GRP_TABS,  2)
#define   M_ALTTB    loc_str(GRP_TABS,  3)
#define   M_DEFTB    loc_str(GRP_TABS,  4)
#define   M_GRVTB    loc_str(GRP_TABS,  5)


#ifdef MSDOS
   COUNT    tab_novo_item( Tab * );
#else
   COUNT    tab_novo_item( );
#endif

static    SS    flag_alterou;
static    SC    *narq_loc;

extern    SI    errno;

F__GLB   TEXT *file_tabela( arquivo, string, col, lin, a_col, a_lin,
                            mascara, l_atr, t_atr,  borda, b_atr, d_atr,
                            titulo, rodape, permicoes, operacao, lista_opc)
SC  *arquivo, *string;
SS   col, lin, a_col, a_lin;
SC  *mascara;
SS   l_atr, t_atr, borda, b_atr, d_atr;
SC  *titulo, *rodape;
SS   permicoes;
SS   operacao;
VOID **lista_opc;
{
   Tab    T;
   WIN   *jm = NULL, *jf = NULL;
   SS     I, J, escolha, n_elem;
   SC   **opcoes, *ret, *m_rodape;
	UTEXT  str_escape[20];
	TEXT   lin_ins[100], lin_alt[100], tec_fun[15];
   TEXT *ajuda_ant = win_ajuda;

   switch( operacao ) {
      case  DEFINE     :
      case  CARREGA    :

                  win_ajuda   = H_DEFTAB;

                  opcoes = calloc( TAM_TAB + 2, sizeof( SC * ));

                  if ( opcoes ==  NULL ) {
                     ret = NULL;
                     goto sai;
                  }
                  else {
                     *lista_opc = ( VOID *(* )[])opcoes;
                  }

                  J = strlen( mascara );
                  n_elem = file_array( arquivo, opcoes, J, operacao);

                  if ( n_elem < 0 ) {
                     ret = NULL;
                     goto sai;
                  }

                  if ( operacao == CARREGA ) {
                     ret = ( SC * )opcoes;
                     goto sai;
                  }
                  else {
                     escolha = 0; /* nao T_ENTER e Nao T_LETRA */
                     m_rodape = M_DEFTB;
                  }

                  break;

      case  SELECAO    :

                  win_ajuda   = H_SELTAB;
                  opcoes = *lista_opc;
                  escolha = T_ENTER;
                  m_rodape = M_SELTB;

                  for (n_elem = 0; n_elem <= TAM_TAB; n_elem++)
                        if (opcoes[n_elem] == NULL) break;

                  break;

      case  FINALIZA   :
                  opcoes = *lista_opc;
                  if ( opcoes != NULL ) {
                     free_array( opcoes );
                     free( opcoes );
                  }
                  ret = NULL;
                  goto sai;
   }

   flag_alterou = 0;

   def_tabela( &T, col, lin, a_col, (permicoes == 0 ) ? n_elem : a_lin,
               n_elem, TAM_TAB, mascara,
               l_atr, t_atr, borda, b_atr, d_atr,
               ( SC *(* )[]) opcoes, titulo, rodape );

   T.edita   = permicoes;
   T.executa = tab_novo_item;
   T.enter   = escolha;

   if ( permicoes != 0 )
      strcpy(tec_fun, "1 2 3 6 IDE");
   else
      strcpy(tec_fun, "1 3 N E");

	pr_escape(tec_fun, str_escape);
	strncpy(lin_ins, funcao("1 2 3 E"), 99);
	strncpy(lin_alt, funcao("1 2 3 E"), 99);
   T.escape    = str_escape;
   T.mens_ins  = M_INSTB;
   T.mens_alt  = M_ALTTB;
	T.fun_ins   = lin_ins;
	T.fun_alt   = lin_alt;

   jm = abre_mensagem( m_rodape );
	jf = abre_funcoes(funcao(tec_fun));

   /* seta nome de arquivo global para ser usado em executa */
   narq_loc = arquivo;

   I = tabela( &T );

   if ( I >= 0 ) {
      strcpy(string, opcoes[ I ] );
      ret = string;
   }
   else {
      ret = NULL;
   }

   if ( flag_alterou ) {
      if ( decida(TW_GGV0, TW_GGV9, H_F2TAB,
               3, 42, "%s\n%s", TW_GGV1, TW_GGV2) == 0 ) {
         mens_1(jm, M_GRVTB, narq_loc);
         write_array( narq_loc, opcoes );
       }
   }

	fecha_funcoes(jf);
	fecha_mensagem(jm);

sai :

   win_ajuda = ajuda_ant;
   return ( ret );
}





F__LOC   COUNT tab_novo_item( T )
Tab  *T;
{  COUNT ret, I;
   TEXT *str;

   switch ( T->tecla ) {

      case K_DEL :
                  free( ( *T->opcoes )[ T->escolhido ] );
                  ( *T->opcoes )[ T->escolhido ] = NULL;
                  flag_alterou = 1;
                  ret = -2;
                  break;

      case K_F6  :
                  I = strlen( T->e->str );
                  if ( I == 0  || ( str = malloc( I + 1 ) )  ==  NULL ) {
                     ret = T->escolhido;
                     break;
                  }
                  else {
                     strcpy( str, T->e->str );
                     if ( T->escolhido != T->n_elem - 1 )
                        free( ( *T->opcoes )[ T->escolhido ] );
                     ( *T->opcoes )[ T->escolhido ] = str;
                  }
                  flag_alterou = 1;
                  if ( T->e->tecla == K_ENTER )
                     T->e->tecla = K_DN;
                  w_ungc( T->e->tecla );
                  ret = -2;
                  break;

      case K_INS :
                  I = strlen( ( *T->opcoes )[ T->escolhido ] );
                  if ( I == 0 || ( str = malloc( I + 1 ) )  ==  NULL ) {
                     ret = T->escolhido;
                     break;
                  }
                  else {
                     strcpy( str, ( *T->opcoes )[ T->escolhido ] );
                     ( *T->opcoes )[ T->escolhido ] = str;
                  }
                  flag_alterou = 1;
                  if ( T->e->tecla == K_ENTER )
                     T->e->tecla = K_DN;
                  w_ungc( T->e->tecla );
                  ret = -2;
                  break;

      case K_F2  :
                  if ( flag_alterou )
                     write_array( narq_loc, (SC **)T->opcoes );

                  flag_alterou = 0;
                  ret = -2;
                  break;

      default    :
                  ret = -3;
                  break;
   }

   return(ret);
}




