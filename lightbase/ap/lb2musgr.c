#include  "lb2all.h"
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2locs.h"
#include  "lb2prot1.h"

#ifdef  VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#define   GRP_CADUSU    13

#define     LF13E       "1 3 E"


#ifdef MSDOS
   COUNT usu_exec(Selecao *);
#else
   COUNT usu_exec( );
#endif



#define   M_MUS00    loc_str(GRP_CADUSU, 77)
#define   M_MUS99    loc_str(GRP_CADUSU, 78)
#define   M_MUS01    loc_str(GRP_CADUSU, 79)
#define   M_MUS02    loc_str(GRP_CADUSU, 80)
#define   M_MUS03    loc_str(GRP_CADUSU, 81)
#define   M_MUS04    loc_str(GRP_CADUSU, 82)
#define   M_MUS05    loc_str(GRP_CADUSU, 83)
#define   M_MUS06    loc_str(GRP_CADUSU, 84)
#define   M_MUS07    loc_str(GRP_CADUSU, 85)
#define   M_MUS08    loc_str(GRP_CADUSU, 86)
#define   M_MUS09    loc_str(GRP_CADUSU, 87)


#define   R_MUS01    loc_str(GRP_CADUSU, 88)
#define   R_MUS02    loc_str(GRP_CADUSU, 89)
#define   R_MUS03    loc_str(GRP_CADUSU, 90)
#define   R_MUS04    loc_str(GRP_CADUSU, 91)
#define   R_MUS05    loc_str(GRP_CADUSU, 92)
#define   R_MUS06    loc_str(GRP_CADUSU, 93)
#define   R_MUS07    loc_str(GRP_CADUSU, 94)
#define   R_MUS08    loc_str(GRP_CADUSU, 95)
#define   R_MUS09    loc_str(GRP_CADUSU, 96)

#ifndef LITE
F__GLB   COUNT cad_usuarios()
{  Selecao m0;
	WIN *jf;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *m_usu[(10 * 3) + 1];

   up_str(GRP_CADUSU);

   win_ajuda    = "MENUUSU";

   set_menu(m_usu, 0, M_MUS01, "1", R_MUS01);
   set_menu(m_usu, 1, M_MUS02, "1", R_MUS02);
   set_menu(m_usu, 2, M_MUS03, "1", R_MUS03);
   set_menu(m_usu, 3, M_MUS04, "2", R_MUS04);
   set_menu(m_usu, 4, M_MUS05, "1", R_MUS05);
   set_menu(m_usu, 5, M_MUS06, "1", R_MUS06);
   set_menu(m_usu, 6, M_MUS07, "2", R_MUS07);
   set_menu(m_usu, 7, M_MUS08, "1", R_MUS08);
   set_menu(m_usu, 8, M_MUS09, "1", R_MUS09);
   set_menu(m_usu, 9, NULL,    "0", NULL);

   jf = abre_funcoes(funcao(LF13E));

	def_menu( &m0, 35,  8, 30, 15,  1,
             MS_CAMPO, MS_BARRA, 1, MS_JANELA, MS_DESTAQUE,
				 &m_usu, M_MUS00, M_MUS99);

   m0.executa = usu_exec;

	menu( &m0 );          /* Abertura do menu de usuarios */

   fecha_funcoes(jf);

   win_ajuda = ajuda_ant;

   down_str(GRP_CADUSU);
   return ( 0 );
}



F__LOC   COUNT usu_exec(s)
Selecao *s;
{  COUNT ret = 0;

	ret = s-> escolhido;

   if ( s->tecla == K_ESC ) {
      ret = -1;
      goto fim;
   }

	switch ( s->escolhido ) {

		case  0  :  ret = atu_usuario(INCLUSAO);   break;  /* Inclusao  de usuarios */
		case  1  :  ret = atu_usuario(ALTERACAO);  break;  /* Alteracao de usuarios */
		case  2  :  ret = atu_usuario(EXCLUSAO);   break;  /* exclusao  de usuarios */

		case  4  :  ret = atu_grupo(INCLUSAO);     break;  /* Inclusao  de grupos   */
		case  5  :  ret = atu_grupo(ALTERACAO);    break;  /* Alteracao de grupos   */

      case  7  :  ret = mos_usuario();           break;  /* mostra       usuarios */
      case  8  :  ret = mos_grupo();             break;  /* mostra       grupos   */

		default  :  ret = -1;
                  break;
   }

   ret = s->escolhido;

fim :

   return(ret);
}

#endif
