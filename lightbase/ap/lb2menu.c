#include  "lb2all.h"
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2prot1.h"
#include  "lb2locs.h"

#ifdef    VAXVMS
#	include  "menu.h"
#else
#	include  "../mt/menu.h"
#endif

#define   GRP_MENU    17



#define     LF13E       "1 3 E"

#ifndef RUNTIME

#define   M_PR00  loc_str(GRP_MENU,    1)
#define   M_PR99  loc_str(GRP_MENU,    2)
#define   M_PR01  loc_str(GRP_MENU,    3)
#define   M_PR02  loc_str(GRP_MENU,    4)
#define   M_PR03  loc_str(GRP_MENU,    5)
#define   R_PR01  loc_str(GRP_MENU,    6)
#define   R_PR02  loc_str(GRP_MENU,    7)
#define   R_PR03  loc_str(GRP_MENU,    8)


#define   M_DF00  loc_str(GRP_MENU,    9)
#define   M_DF99  loc_str(GRP_MENU,   10)
#define   M_DF01  loc_str(GRP_MENU,   11)
#define   M_DF02  loc_str(GRP_MENU,   12)
#define   M_DF03  loc_str(GRP_MENU,   13)
#define   M_DF04  loc_str(GRP_MENU,   14)
#define   R_DF01  loc_str(GRP_MENU,   15)
#define   R_DF02  loc_str(GRP_MENU,   16)
#define   R_DF03  loc_str(GRP_MENU,   17)
#define   R_DF04  loc_str(GRP_MENU,   18)


#define   M_UT00  loc_str(GRP_MENU,   19)
#define   M_UT99  loc_str(GRP_MENU,   20)
#define   M_UT01  loc_str(GRP_MENU,   21)
#define   M_UT02  loc_str(GRP_MENU,   22)
#define   M_UT03  loc_str(GRP_MENU,   23)
#define   M_UT04  loc_str(GRP_MENU,   24)
#define   M_UT05  loc_str(GRP_MENU,   25)
#define   M_UT06  loc_str(GRP_MENU,   26)
#define   M_UT07  loc_str(GRP_MENU,   27)
#define   M_UT08  loc_str(GRP_MENU,   28)
#define   M_UT09  loc_str(GRP_MENU,   29)

#define   R_UT01  loc_str(GRP_MENU,   30)
#define   R_UT02  loc_str(GRP_MENU,   31)
#define   R_UT03  loc_str(GRP_MENU,   32)
#define   R_UT04  loc_str(GRP_MENU,   33)
#define   R_UT05  loc_str(GRP_MENU,   34)
#define   R_UT06  loc_str(GRP_MENU,   35)
#define   R_UT07  loc_str(GRP_MENU,   36)
#define   R_UT08  loc_str(GRP_MENU,   37)
#define   R_UT09  loc_str(GRP_MENU,   38)


#define   M_MT00  loc_str(GRP_MENU,   39)
#define   M_MT99  loc_str(GRP_MENU,   40)
#define   M_MT01  loc_str(GRP_MENU,   41)
#define   M_MT02  loc_str(GRP_MENU,   42)
#define   M_MT03  loc_str(GRP_MENU,   43)
#define   M_MT04  loc_str(GRP_MENU,   44)
#define   M_MT05  loc_str(GRP_MENU,   45)
#define   M_MT06  loc_str(GRP_MENU,   46)
#define   M_MT07  loc_str(GRP_MENU,   47)
#define   M_MT08  loc_str(GRP_MENU,   48)
#define   M_MT09  loc_str(GRP_MENU,   49)


#define   R_MT01  loc_str(GRP_MENU,   50)
#define   R_MT02  loc_str(GRP_MENU,   51)
#define   R_MT03  loc_str(GRP_MENU,   52)
#define   R_MT04  loc_str(GRP_MENU,   53)
#define   R_MT05  loc_str(GRP_MENU,   54)
#define   R_MT06  loc_str(GRP_MENU,   55)
#define   R_MT07  loc_str(GRP_MENU,   56)
#define   R_MT08  loc_str(GRP_MENU,   57)
#define   R_MT09  loc_str(GRP_MENU,   58)



#ifdef MSDOS
   COUNT prin_exec(Selecao *);
   COUNT prin_com(Selecao *);
   COUNT definicao(VOID);
   COUNT def_exec(Selecao *);
   COUNT def_com(Selecao *);
   VOID  seta_definicao(Selecao *);
   COUNT utilizacao(VOID);
   COUNT uti_exec(Selecao *);
   COUNT uti_com(Selecao *);
   VOID  seta_utilizacao(Selecao *);
   COUNT manutencao(VOID);
   COUNT man_exec(Selecao *);
   COUNT man_com(Selecao *);
   VOID  seta_manutencao(Selecao *);
#else
   COUNT prin_exec( );
   COUNT prin_com( );
   COUNT definicao( );
   COUNT def_exec( );
   COUNT def_com( );
   VOID  seta_definicao( );
   COUNT utilizacao( );
   COUNT uti_exec( );
   COUNT uti_com( );
   VOID  seta_utilizacao( );
   COUNT manutencao( );
   COUNT man_exec( );
   COUNT man_com( );
   VOID  seta_manutencao( );
#endif

static    COUNT     var_opcao;
static    COUNT     menu_aberto;


/*---------------------------------------------------------------------*/
/*  MENU PRINCIPAL                                                     */
/*---------------------------------------------------------------------*/
F__GLB   COUNT principal()
{  COUNT ret;
	Selecao m0;
	WIN *jf;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *m_pr[(4 * 3) + 1];

   up_str(GRP_MENU);

   win_ajuda    = "MENUPRIN";

   set_menu(m_pr, 0, M_PR01, "1", R_PR01);
   set_menu(m_pr, 1, M_PR02, "1", R_PR02);
   set_menu(m_pr, 2, M_PR03, "1", R_PR03);
   set_menu(m_pr, 3, NULL,   "0", NULL);


	def_menu( &m0, 16,  5, 70,  1, 15 , MS_CAMPO, MS_BARRA, DUPLAS,
				 MS_JANELA, MS_DESTAQUE, &m_pr, M_PR00, M_PR99);

   m0.comando = prin_com;
   m0.executa = prin_exec;

   w_ungc(K_ENTER);

	if ( WBS != NULL ) { /* existe base ativa */
      w_ungc(K_RT);
   }

   jf = abre_funcoes(funcao(LF13E));

	ret = menu( &m0 );          /* Abertura do menu de principal*/

   fecha_funcoes(jf);

   win_ajuda = ajuda_ant;

   down_str(GRP_MENU);
   return (ret);
}


F__LOC   COUNT prin_exec(s)
Selecao *s;
{  COUNT ret;

   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */
   /* menu principal */

   if ( s->tecla == K_ESC ) {
      ret = -1;
      goto fim;
   }

   var_opcao   = -1;
   menu_aberto =  0;

   switch ( (ret = s->escolhido) ) {
		case  0  : definicao();    break;
		case  1  : utilizacao();   break;
		case  2  : manutencao();   break;
      default  : break;
   }

   if ( var_opcao != -1 ) {
      menu_aberto = 1;
      ret = var_opcao;
      w_ungc( K_ENTER );
   }

fim :

   return(ret);
}


F__LOC   COUNT prin_com(s)
Selecao *s;
{  COUNT ret;

	ret = 0;

   if ( menu_aberto && ( s->tecla == K_RT || s->tecla == K_LFT ) ) {
      ret = -2;

      if ( s->escolhido == 2  &&  s->tecla == K_RT ) {
         s->tecla = K_HOME;
         w_ungc( K_ENTER );
      }

      else s->tecla = K_ENTER;
   }

	return( ret );
}






/*---------------------------------------------------------------------*/
/*  MODULO DE DEFINICAO                                                */
/*---------------------------------------------------------------------*/

F__LOC   COUNT definicao()
{  COUNT   ret;
   Selecao m0;
   WIN *jf;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *m_def[(5 * 3) + 1];

   win_ajuda    = "MENUDEF";

   set_menu(m_def, 0, M_DF01, "1", R_DF01);
   set_menu(m_def, 1, M_DF02, "1", R_DF02);
   set_menu(m_def, 2, M_DF03, "2", R_DF03);
   set_menu(m_def, 3, M_DF04, "1", R_DF04);
   set_menu(m_def, 4, NULL,   "0", NULL);


loop :

	def_menu( &m0, 17,  6, 30, 15,  1,
             MS_CAMPO, MS_BARRA, 1, MS_JANELA, MS_DESTAQUE,
				 &m_def, M_DF00, M_DF99);

   m0.comando = def_com;
   m0.executa = def_exec;

   seta_definicao(&m0);

   jf = abre_funcoes(funcao(LF13E));

   ret = menu( &m0 );          /* Abertura do menu de definicao */

	if (ret == -4 ) goto loop; /* seleciona base atualiza permissoes */

   fecha_funcoes(jf);

   win_ajuda = ajuda_ant;

	return (ret);
}




F__LOC   COUNT def_exec(s)
Selecao *s;
{  COUNT ret;


   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */
   /* menu principal */

   if ( s->tecla == K_ESC ) {
      ret = -1;
      goto fim;
   }

   switch ( (ret = s->escolhido) ) {

        case  0  :  imp_logo(0); def_base();      break; 

#ifndef LITE
      case  1  :  imp_logo(0); cad_usuarios();  break;
#endif

		case  3  :  sel_base(BASE_EDIT, str_nada, M_SELBASE);
                  seta_definicao(s);
                  break;

		default  :  ret = -1;
                  break;
   }

fim :


   return(ret);
}



F__LOC   COUNT def_com(s)
Selecao *s;
{  COUNT ret = 0;

	switch ( s->tecla ) {

      case K_RT : /* menu de utilizacao */
                  ret = -3;
                  var_opcao = 1;
                  break;

      case K_LFT: /* utilizacao */
                  ret = -3;
                  var_opcao = 3;
                  break;
   }

	return(ret);
}


F__LOC   VOID seta_definicao(m0)
Selecao *m0;
{
   /* A e B podem definir BASES */
   *(*m0->opcoes)[AT(0)] = (ap_status <= 'B') ? '1': '0';

#ifdef LITE
   *(*m0->opcoes)[AT(1)] = '0';  /* nao tem cadastro de acessos */
#else
   /* so' usuarios privilegiados pode cadastrar */
   *(*m0->opcoes)[AT(1)] = (ap_status == 'A') ? '1': '0';
#endif
}


/*---------------------------------------------------------------------*/
/*  MODULO DE UTILIZACAO                                               */
/*---------------------------------------------------------------------*/

F__GLB   COUNT utilizacao()
{  COUNT ret;
   TEXT *ajuda_ant = win_ajuda;
   Selecao m0;
   WIN *jf;
   TEXT *m_uti[(10 * 3) + 1];

   win_ajuda    = "MENUUTIL";

   set_menu(m_uti, 0, M_UT01, "1", R_UT01);
   set_menu(m_uti, 1, M_UT02, "1", R_UT02);
   set_menu(m_uti, 2, M_UT03, "1", R_UT03);
   set_menu(m_uti, 3, M_UT04, "1", R_UT04);
   set_menu(m_uti, 4, M_UT05, "1", R_UT05);
   set_menu(m_uti, 5, M_UT06, "1", R_UT06);
   set_menu(m_uti, 6, M_UT07, "1", R_UT07);
   set_menu(m_uti, 7, M_UT08, "2", R_UT08);
   set_menu(m_uti, 8, M_UT09, "1", R_UT09);
   set_menu(m_uti, 9, NULL,   "0", NULL);


	def_menu( &m0, 32,  6, 30, 15,  1,
             MS_CAMPO, MS_BARRA, 1, MS_JANELA, MS_DESTAQUE,
				 &m_uti, M_UT00, M_UT99);

   m0.comando = uti_com;
   m0.executa = uti_exec;

   seta_utilizacao(&m0);

   jf = abre_funcoes(funcao(LF13E));

   ret = menu( &m0 );          /* Abertura do menu de utilizacao */

   fecha_funcoes(jf);

   win_ajuda = ajuda_ant;

   return (ret);
}


F__LOC   COUNT uti_exec(s)
Selecao *s;
{  COUNT ret;
   TEXT *ajuda_ant = win_ajuda;

   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */
   /* menu principal */

   if ( s->tecla == K_ESC ) {
      ret = -1;
      goto fim;
   }

	switch ( (ret = s->escolhido) ) {
      case  0 :   imp_logo(0); ent_dados(BASE_EDIT, INCLUSAO);    break;
      case  1 :   imp_logo(0); ent_dados(BASE_EDIT, ALTERACAO);   break;
      case  2 :   imp_logo(0); ent_dados(BASE_EDIT, EXCLUSAO);    break;
      case  3 :   imp_logo(0); pesquisa(BASE_EDIT);               break;
      case  4 :   imp_logo(0); emite(BASE_EDIT, NULL, NULL);      break;
      case  5 :   imp_logo(0); exporta(BASE_EDIT, NULL);          break;
      case  6 :   imp_logo(0); importa(BASE_EDIT);                break;

		case  8 :   sel_base(BASE_EDIT, str_nada, M_SELBASE);
                  seta_utilizacao(s);
                  break;

		default :   ret = -1;
                  break;
   }

fim :

   win_ajuda = ajuda_ant;

   return(ret);
}


F__LOC   COUNT uti_com(s)
Selecao *s;
{  COUNT ret = 0;

	switch ( s->tecla ) {

      case K_RT : /* menu de definicao */
                  ret = -3;
                  var_opcao = 2;
                  break;

      case K_LFT: /* menu de manutencao */
                  ret = -3;
                  var_opcao = 0;
                  break;
   }

	return(ret);
}


F__LOC   VOID seta_utilizacao(m0)
Selecao *m0;
{  COUNT i, x;

	if ( WBS == NULL ) {   /* desativa opcoes caso nao haja base ativa */
		for (i=0; i < 7 ; i++) *(*m0->opcoes)[AT(i)] = '0';
	}
	else {
      x = ( ap_status == 'A' ) ? '1' : '0';

		*(*m0->opcoes)[AT(0)] = (ap_acesso_ativo.da_inclui   == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(1)] = (ap_acesso_ativo.da_altera   == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(2)] = (ap_acesso_ativo.da_exclui   == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(3)] = (ap_acesso_ativo.da_pesquisa == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(4)] = (ap_acesso_ativo.da_emite    == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(5)] = (ap_acesso_ativo.da_exporta  == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(6)] = (ap_acesso_ativo.da_importa  == 'X' ) ? '1' : x;

      if ( WCB->cb_lockatu == 0 ) { /* base estatica */
		   *(*m0->opcoes)[AT(0)] = 0;
		   *(*m0->opcoes)[AT(1)] = 0;
		   *(*m0->opcoes)[AT(2)] = 0;
		   *(*m0->opcoes)[AT(6)] = 0;
      }

      /* bases comerciais nao podem ser exportadas */
      if ( WBS->bs_finger != 0 && pode_exportar(WBS->bs_idbase) != 'S' )
		   *(*m0->opcoes)[AT(5)] = 0;
	}
}



/*---------------------------------------------------------------------*/
/*  MODULO DE MANUTENCAO                                               */
/*---------------------------------------------------------------------*/

F__LOC   COUNT manutencao()
{  COUNT   ret;
   Selecao m0;
   WIN *jf;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *m_man[(10 * 3) + 1];

   win_ajuda    = "MENUMANU";

   set_menu(m_man, 0, M_MT01, "1", R_MT01);
   set_menu(m_man, 1, M_MT02, "2", R_MT02);
   set_menu(m_man, 2, M_MT03, "1", R_MT03);
   set_menu(m_man, 3, M_MT04, "1", R_MT04);
   set_menu(m_man, 4, M_MT05, "2", R_MT05);
   set_menu(m_man, 5, M_MT06, "1", R_MT06);
   set_menu(m_man, 6, M_MT07, "1", R_MT07);
   set_menu(m_man, 7, M_MT08, "2", R_MT08);
   set_menu(m_man, 8, M_MT09, "1", R_MT09);
   set_menu(m_man, 9, NULL,   "0", NULL);


	def_menu( &m0, 48,  6, 30, 15,  1, MS_CAMPO, MS_BARRA, 1,
             MS_JANELA, MS_DESTAQUE, &m_man,  M_MT00, M_MT99);

   m0.comando = man_com;
   m0.executa = man_exec;

   seta_manutencao(&m0);

   jf = abre_funcoes(funcao(LF13E));

   ret = menu( &m0 );          /* Abertura do menu de manutencao */

   fecha_funcoes(jf);

   win_ajuda = ajuda_ant;

   return (ret);
}


F__LOC   COUNT man_exec(s)
Selecao *s;
{  COUNT ret;
   TEXT *ajuda_ant = win_ajuda;

   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */
   /* menu principal */

   if ( s->tecla == K_ESC ) {
      ret = -1;
      goto fim;
   }

	switch ( (ret = s->escolhido) ) {

      case  0  :  imp_logo(0); tools_base();                break;
      case  2  :  imp_logo(0); index_scan(BASE_EDIT, 'P');  break;
      case  3  :  imp_logo(0); index_scan(BASE_EDIT, 'C');  break;
      case  5  :  imp_logo(0); backup(BASE_EDIT);           break;
      case  6  :  imp_logo(0);
                  restore(BASE_EDIT);
                  seta_manutencao(s);
                  break;

		case  8  :  sel_base(BASE_EDIT, str_nada, M_SELBASE);
                  seta_manutencao(s);
                  break;

		default  :  ret = -1;
                  break;
   }

fim :

   win_ajuda = ajuda_ant;

   return(ret);
}


F__LOC   COUNT man_com(s)
Selecao *s;
{  COUNT ret = 0;

	switch ( s->tecla ) {

      case K_RT : /* definicao */
                  ret = -3;
                  var_opcao = 0;
                  break;

      case K_LFT: /* menu de utilizacao */
                  ret = -3;
                  var_opcao = 1;
                  break;
   }
	return(ret);
}


F__LOC   VOID seta_manutencao(m0)
Selecao *m0;
{  COUNT x;

	if ( ap_status <= 'B')
      *(*m0->opcoes)[AT(0)] = '1';
	else
      *(*m0->opcoes)[AT(0)] = '0';

	if ( WBS == NULL ) { /* desativa opcoes caso nao haja base ativa */
		*(*m0->opcoes)[AT(2)] = '0';
		*(*m0->opcoes)[AT(3)] = '0';
		*(*m0->opcoes)[AT(5)] = '0';
	}
	else {
      x = ( ap_status == 'A' ) ? '1' : '0';

		*(*m0->opcoes)[AT(2)] = (ap_acesso_ativo.da_parcial  == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(3)] = (ap_acesso_ativo.da_total    == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(5)] = (ap_acesso_ativo.da_backup   == 'X' ) ? '1' : x;
		*(*m0->opcoes)[AT(6)] = (ap_acesso_ativo.da_restore  == 'X' ) ? '1' : x;

      if ( WCB->cb_lockatu == 0 ) { /* base estatica */
		   *(*m0->opcoes)[AT(2)] = 0;
		   *(*m0->opcoes)[AT(3)] = 0;
		   *(*m0->opcoes)[AT(5)] = 0;
      }

	}
}
/* Copia nao RUNTIME termina aqui */
#endif





/* Copia RUNTIME comeca aqui */
#ifdef RUNTIME
#ifdef MSDOS
   COUNT menu_manut( VOID );
   COUNT deleta_base( VOID );
#else
   COUNT menu_manut( );
   COUNT deleta_base( );
#endif

#define   M_PR00   loc_str(GRP_MENU,  60 )
#define   M_PR99   loc_str(GRP_MENU,  61 )

#define   M_RTPSQ  loc_str(GRP_MENU,  62 )
#define   M_RTSEL  loc_str(GRP_MENU,  63 )
#define   M_RTMAN  loc_str(GRP_MENU,  64 )

#define   R_RTPSQ  loc_str(GRP_MENU,  65 )
#define   R_RTSEL  loc_str(GRP_MENU,  66 )
#define   R_RTMAN  loc_str(GRP_MENU,  67 )

#define   M_RTMUS   loc_str(GRP_MENU, 68 )
#define   M_RTIBAS  loc_str(GRP_MENU, 69 )
#define   M_RTEBAS  loc_str(GRP_MENU, 70 )

#define   R_RTMUS   loc_str(GRP_MENU, 71 )
#define   R_RTIBA   loc_str(GRP_MENU, 72 )
#define   R_RTEBA   loc_str(GRP_MENU, 73 )

#define   T_RTMAN   loc_str(GRP_MENU, 74 )


/*---------------------------------------------------------------------*/
/*  MENU PRINCIPAL                                                     */
/*---------------------------------------------------------------------*/
F__GLB   COUNT principal()
{  COUNT ret, col;
	Selecao m0;
	WIN *jf;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *m_pr[(4 * 3) + 1];

   up_str(GRP_MENU);

   win_ajuda    = "MENUPRIN";

   set_menu(m_pr, 0, M_RTPSQ, "1", R_RTPSQ);
   set_menu(m_pr, 1, M_RTSEL, "1", R_RTSEL);
   set_menu(m_pr, 2, NULL,   "0", NULL);
   set_menu(m_pr, 3, NULL,   "0", NULL);

   if ( ap_status == 'A' ) {
      set_menu(m_pr, 2, M_RTMAN, "1", R_RTMAN);
      col = 15;
   }
   else
      col = 25;

/*
	def_menu( &m0, 32,  9, 65, 15, 1, MS_CAMPO, MS_BARRA, DUPLAS,
				 MS_JANELA, MS_DESTAQUE, &m_pr, M_PR00, M_PR99);
*/

	def_menu( &m0, col,  4, 65, 15, 4, MS_CAMPO, MS_BARRA, DUPLAS,
				 MS_JANELA, MS_DESTAQUE, &m_pr, M_PR00, M_PR99);


   jf = abre_funcoes(funcao(LF13E));

   /* forca iniciar com seleciona base */
	if ( WBS == NULL ) /* desativa pesquisa se nao tiver base ativa */
		*(*m0.opcoes)[AT(0)] = '0';
   else
		*(*m0.opcoes)[AT(0)] = '1';


   while ( 1 ) {

      if ( ap_status != 'A' ) {
         if ( WBS == NULL)
            break;
         else
            pesquisa(BASE_EDIT);

      }
      else {
         ret = menu(&m0);

         if ( ret == -1 )
            break;
      }

      switch(ret) {
         case  0 :   imp_logo(0); pesquisa(BASE_EDIT);
                     break;

		   case  1 :   sel_base(BASE_EDIT, str_nada, M_SELBASE);
                     break;

		   case  2 :   menu_manut( );
                     break;
      }

	   if ( WBS == NULL ) /* desativa pesquisa se nao tiver base ativa */
		   *(*m0.opcoes)[AT(0)] = '0';
      else
		   *(*m0.opcoes)[AT(0)] = '1';

      if ( ap_status != 'A' )
		   if ( sel_base(BASE_EDIT, str_nada, M_SELBASE) < 0 )
            break;
   }



   fecha_funcoes(jf);

   win_ajuda = ajuda_ant;

   down_str(GRP_MENU);
   return (ret);
}


F__LOC   COUNT menu_manut()
{  COUNT ret;
	Selecao m0;
	WIN *jf;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *m_pr[(4 * 3) + 1];

   win_ajuda    = "MMANUTRT";

   set_menu(m_pr, 0, M_RTIBAS, "1", R_RTIBA);
   set_menu(m_pr, 1, M_RTEBAS, "1", R_RTEBA);
   set_menu(m_pr, 2, NULL,   "0", NULL);

	def_menu( &m0, 30,  6, 65, 15, 1, MS_CAMPO, MS_BARRA, DUPLAS,
				 MS_JANELA, MS_DESTAQUE, &m_pr, T_RTMAN, "");


   jf = abre_funcoes(funcao(LF13E));

   while ( (ret = menu(&m0)) != -1 ) {

      switch(ret) {

		   case  0 :   def_base();
                     break;

		   case  1 :   deleta_base( );
                     break;
      }
   }

   fecha_funcoes(jf);

   win_ajuda = ajuda_ant;


   return (ret);
}


#endif

