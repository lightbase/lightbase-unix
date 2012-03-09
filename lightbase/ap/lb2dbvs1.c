#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2vet.h"

#include  "lb2db.h"
#include  "lb2vista.h"

#ifdef    VAXVMS
#include  "tabela.h"
#include  "menu.h"
#else
#include  "../mt/tabela.h"
#include  "../mt/menu.h"
#endif


#define   GRP_DFVIS    21

#define   e_grafico(c)   ((c>=129 && c <=152) ? 1 : 0 )

#define   LF123E      "1 2 3 E"
#define   LF1236IDE   "1 2 3 6 IDE"
#define   STR_DUAS    "%s\n%s"
#define   MASC_PROT   "PPPPPPPPPPPPPPPPPPPPPPPPPP" /* Tamanho de M_OBRIGAT etc */

#define   H_CFITDEL       "CF_ITDEL"  /* confirmacao de delecao de itens */
#define   H_TAMVIS        "TAMVIS"    /* tamanho da visibilidade X/Y */

#define  MN_T_BORDAS    loc_str(GRP_DFVIS, 15)
#define  MN_T_CAMPOS    loc_str(GRP_DFVIS, 16)
#define  MN_T_LINHAS    loc_str(GRP_DFVIS, 17)
#define  MN_T_APAG2     loc_str(GRP_DFVIS, 18)
#define  MN_T_TIPOS     loc_str(GRP_DFVIS, 19)
#define  MN_T_VISIV     loc_str(GRP_DFVIS, 20)
#define  MN_T_INS_LIN   loc_str(GRP_DFVIS, 21)
#define  MN_T_RET_LIN   loc_str(GRP_DFVIS, 22)


#define  RD_T_BOR       loc_str(GRP_DFVIS, 23)
#define  RD_T_CAM       loc_str(GRP_DFVIS, 24)
#define  RD_T_LIN       loc_str(GRP_DFVIS, 25)
#define  RD_T_APA       loc_str(GRP_DFVIS, 26)
#define  RD_T_TIP       loc_str(GRP_DFVIS, 27)
#define  RD_T_VIS       loc_str(GRP_DFVIS, 28)
#define  RD_T_INS       loc_str(GRP_DFVIS, 29)
#define  RD_T_RET       loc_str(GRP_DFVIS, 30)


#define  I_ACESSO   loc_str(GRP_DFVIS, 31)
#define  I_BORDA    loc_str(GRP_DFVIS, 32)
#define  I_LINHA    loc_str(GRP_DFVIS, 33)
#define  I_APAGA    loc_str(GRP_DFVIS, 34)
#define  I_CAMPO    loc_str(GRP_DFVIS, 35)
#define  I_EDITL    loc_str(GRP_DFVIS, 36)
#define  I_EDITT    loc_str(GRP_DFVIS, 37)


#define  C_TR_OBR       loc_str(GRP_DFVIS, 38)
#define  C_TR_ENT       loc_str(GRP_DFVIS, 39)
#define  C_TR_SAI       loc_str(GRP_DFVIS, 40)
#define  C_TR_ACL       loc_str(GRP_DFVIS, 41)
#define  C_TR_CAMPO     loc_str(GRP_DFVIS, 42)


#define  M_TR_VISIB     loc_str(GRP_DFVIS, 43)
#define  M_TR_DBORDA    loc_str(GRP_DFVIS, 44)
#define  M_TR_TITBORDA  loc_str(GRP_DFVIS, 45)
#define  M_TR_DLINHA    loc_str(GRP_DFVIS, 46)
#define  M_TR_DELBOX    loc_str(GRP_DFVIS, 47)
#define  M_TR_DELVER    loc_str(GRP_DFVIS, 48)
#define  M_TR_DELHOR    loc_str(GRP_DFVIS, 49)
#define  M_TR_DELCAM    loc_str(GRP_DFVIS, 50)

#define  M_TR_NAVVISTA  loc_str(GRP_DFVIS, 51)
#define  M_TR_FUNED     loc_str(GRP_DFVIS, 52)

#define  C_VSLICOL     loc_str(GRP_DFVIS, 53)
#define  C_VSSOLIN     loc_str(GRP_DFVIS, 54)

#define  M_VISLC       loc_str(GRP_DFVIS, 55)
#define  M_VISCOL      loc_str(GRP_DFVIS, 56)
#define  M_VISTIT      loc_str(GRP_DFVIS, 57)

#ifdef MSDOS
   SS    troca_acesso( Tab * );
   VOID  atu_opcoes( SS, SC *(* )[] );
   VOID  atu_campos( SS, SC *(* )[] );
   VOID  atr_campo( SS );
   VOID  tipo_campos( WIN * );
   VOID  vis_muda( WIN *, SS );
   VOID  visivel( WIN * );
   TTela *insere_item( SS, SS, SS, SS, SS, SC *, SS, SS );
   SS    item_anterior( SS, SS, SS, SS, SS );
   SS    existe_item( SS, SS, SS, SS, SS, SS, SS );
   VOID  t_ins_linha( WIN * );
   VOID  del_linha( WIN * );
   SS    centra_borda( d_entry *);
   SS    move_cursor( WIN *, SS );
   VOID  faz_borda( WIN * );
   VOID  faz_linha( WIN * );
   SS    conflito_item ( SS, SS, SS, SS );
   SS    del_confirma ( SS  );
   VOID  deleta_item ( SS );
   SS    apaga( WIN * );
   VOID  faz_campo( WIN * );
   SS    ed_linha( WIN * );
   VOID  disp_lc( WIN *, SC *, SS, SS);
   SS    vis_direto(TTela *, COUNT, COUNT, WIN *);
   SS    acha_ordem(SS, SS, SS, SS);
#else
   SS    troca_acesso( );
   VOID  atu_opcoes( );
   VOID  atu_campos( );
   VOID  atr_campo( );
   VOID  tipo_campos( );
   VOID  vis_muda( );
   VOID  visivel( );
   TTela *insere_item( );
   SS    item_anterior( );
   SS    existe_item( );
   VOID  t_ins_linha( );
   VOID  del_linha( );
   SS    centra_borda( );
   SS    move_cursor( );
   VOID  faz_borda( );
   VOID  faz_linha( );
   SS    conflito_item ( );
   SS    del_confirma ( );
   VOID  deleta_item ( );
   SS    apaga( );
   VOID  faz_campo( );
   SS    ed_linha( );
   VOID  disp_lc( );
   SS    vis_direto( );
   SS    acha_ordem( );
#endif

static UC        alterou;
static UC        tela;
static SS        atributo;
static SS        no_menu;
static WIN       *JI;
static TTela **vet_evis;


F__GLB   SS ed_tela(foi_modificado)
SS *foi_modificado;
{
   TEXT *ajuda_ant = win_ajuda;
   TEXT *volta_ajuda;
   SS    I1, I2, ch;
   WIN   *win, *JM, *JF;
   TTela *PI;
   Campo **vetcp;
   SC    *m_opc[(9 * 3) + 1];
   SC    *p = "1";
   Selecao   M;
   static SC bottom[ 50 ];


   win_ajuda = "DEFVISTA";

   set_menu(m_opc, 0, MN_T_BORDAS , p, RD_T_BOR);
   set_menu(m_opc, 1, MN_T_CAMPOS , p, RD_T_CAM);
   set_menu(m_opc, 2, MN_T_LINHAS , p, RD_T_LIN);
   set_menu(m_opc, 3, MN_T_APAG2  , p, RD_T_APA);
   set_menu(m_opc, 4, MN_T_TIPOS  , p, RD_T_TIP);
   set_menu(m_opc, 5, MN_T_VISIV  , p, RD_T_VIS);
   set_menu(m_opc, 6, MN_T_INS_LIN, p, RD_T_INS);
   set_menu(m_opc, 7, MN_T_RET_LIN, p, RD_T_RET);
   set_menu(m_opc, 8, NULL,       "0", NULL);

   win = abre_janela(J_UTIL);       /* abre janela para salvar conteudo */

   win->w_flags  &= 1;
   win->w_flags  |= W_VISIB;

   atributo       = win->w_optatr;
   win->w_optatr  = win->w_boxatr;
   win->campoatr  = win->w_boxatr;


   JM = abre_mensagem( M_TR_NAVVISTA );
   JF = abre_funcoes( funcao( M_TR_FUNED ) );
   JI = abre_insercol( str_nada );

	fwat( 0, 0, win );

   sel_campo(BASE_EDIT, NULL, 1 ); /* 1 so resetar ultimo campo */

   vet_evis = (TTela **) ap_vista.vis_vet->vi_vtit;
   vetcp    = pos_vetcmp(BASE_EDIT);

   alterou = 0;
   tela = 1;
   no_menu = 0;


   lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL);

   fwat( 0, 0, win );

   while ( 1 )
   {
      I1 = conflito_item( win->w_cx, win->w_cy, -1, CAMPO_G );

      if ( I1 != -1 )
      {
         PI = pos_itvis(I1);
         I2 = acha_campo( PI->te_icb, PI->te_num);
      }

      if ( I1 == -1  ||  I2 == -1 )
         insercol( JI, I_EDITT );
      else
         insercol( JI, vetcp[ I2 ]->cp_nome );

      if ( ! w_kbhit(win) )
         disp_lc( win, bottom, win->w_cy, win->w_cx );

      ch = fwqgc( win );

      if ( move_cursor( win, ch ) )   continue;

      switch ( ch )
      {
      case K_PGDN  :
                     if ( existe_item( tela + 1, -1, -1, -1,
                                                 -1, -1, -1 ) == -1 )
                     {
                        if ( existe_item( tela,  -1, -1, -1,
                                                 -1, -1, -1 ) == -1 )
                        {
                           break;
                        }
                     }

                     tela = COTA( 1, tela + 1, NO_TELAS );
                     lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL);
 			            break;

      case K_PGUP  :
                     tela = COTA( 1, tela - 1, NO_TELAS );
                     lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL);
 			            break;

      case K_F0    :
      case K_ENTER :
                     {
                        SS   X, Y;
                        X = win->w_cx;  Y = win->w_cy;
                        fwpc( UG_JUN, win );
                        fwat( X, Y, win );
                        fwrefresh( win );
                     }

                     def_menu( &M, size_x, 2, 99, 99, 1, MS_CAMPO, MS_BARRA, 1,
                               MS_JANELA, MS_DESTAQUE, &m_opc, str_nada, str_nada );

                     M.escolhido = no_menu;

                     volta_ajuda = win_ajuda;

                     switch ( menu( &M ) )
                     {
                     default:
                              break;

                     case 0 : /*** BORDA ***/
                              win_ajuda = "F10BORDA";
                              faz_borda( win );
                              break;

                     case 1 : /*** CAMPOS ***/
                              win_ajuda = "F10CAMPO";
                              faz_campo( win );
                              break;

                     case 2 : /*** LINHA ***/
                              win_ajuda = "F10LINHA";
                              faz_linha( win );
                              break;

                     case 3 : /*** APAGA ***/
                              win_ajuda = "F10APAGA";
                              apaga( win );
                              break;

                     case 4 : /*** TIPOS DE CAMPOS ***/
                              win_ajuda = "F10TIPOS";
                              tipo_campos( win );
                              break;

                     case 5 : /*** VISIVEL ***/
                              win_ajuda = "F10VISIB";
                              visivel( win );
                              break;

                     case 6 : /*** INSERE LINHA ***/
                              win_ajuda = "F10INSLI";
                              t_ins_linha( win );
                              break;

                     case 7 : /*** RETIRA LINHA ***/
                              win_ajuda = "F10RETLI";
                              del_linha( win );
                              break;
                     }

                     win_ajuda = volta_ajuda;

                     no_menu = M.escolhido;
                     lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL );
 			            break;

      case 14      : /*** INSERE LINHA - CTRL N ***/
                     t_ins_linha( win );
                     lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL);
                     break;

      case 25      : /*** RETIRA LINHA - CTRL Y ***/
                     del_linha( win );
                     lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL);
                     break;

      case K_ESC   :
                     if ( alterou )   ch = 1;
                     else             ch = 0;
      case K_F2    : goto fim;

      case K_DEL   :
                     /* Se nao houver item a ser apagado */
                     /* Execute opcao DEFAULT para ser usado em ed_linha() */
                     if ( apaga( win ) )   break;
      case K_BS    :
      default      :
                     if( e_print( ch ) || ch == K_BS || ch == K_DEL )
                     {
                        w_ungc( ch );

                        disp_lc( win, bottom, win->w_cy, -1 );

                        ch = ed_linha( win );

                        if ( ch != K_ESC )   w_ungc( ch );
                     }

                     break;

      case K_TAB   : /* Nao podemos chamas ed_linha() pois entra em LOOP */
                     /* Teclas que nao fazem NADA */
                     break;
      }
   }

fim:

   if ( alterou )
      *foi_modificado = 1;

   fecha_insercol( JI );
   fecha_funcoes(  JF );
   fecha_mensagem( JM );

   fwkill( win );

   win_ajuda = ajuda_ant;
   return( ch );
}

F__LOC	VOID   atr_campo( campo )
SS   campo;
{
   WIN    *jm;
   TTela  *PI;
   SS     I;
	Tab    T;
   SC     tit[ 40 ];
   SC     *opcoes[5];

   opcoes[0] = C_TR_OBR;
   opcoes[1] = C_TR_ENT;
   opcoes[2] = C_TR_SAI;
   opcoes[3] = C_TR_ACL;
   opcoes[4] = NULL;

   PI = vet_evis[ campo ];

   I  = acha_campo( PI->te_icb, PI->te_num);

   if ( I != -1 && pos_vetcmp(PI->te_icb)[I]->cp_tipo != 'F' ) {

      sprintf( tit, "%s - %s", C_TR_CAMPO, ( I == - 1 ) ? "NULL" :
                        pos_vetcmp(PI->te_icb)[I]->cp_nome);

      jm = abre_mensagem( M_INVERTE );

	   def_tabela( &T, size_x, 2, size_x, 4, 4, 4, MASC_PROT,
               MS_JANELA, MS_BARRA, 1, MS_JANELA, MS_DESTAQUE,
				   &opcoes, tit, str_nada );

      T.comando = troca_acesso;

      atu_opcoes( campo, &opcoes );

	   tabela( &T );

      atu_campos( campo, &opcoes );

      fecha_mensagem( jm );
   }
}


F__LOC   SS  troca_acesso( T )
Tab   *T;
{
   SS    ret,  C;
   SC    *P, *S;

   insercol( JI, I_ACESSO );

   T->tecla = C = fwnqgc( T->win );

   ret = T->escolhido;

   if ( C != 32 )  {  goto  fim;  }

   alterou = 1;

   P = ( *T->opcoes )[ T->escolhido ];

   S = P + strlen( P ) - strlen( OP_SIM );

   if ( strcmp( S, OP_SIM ) == 0 )  {  strcpy( S, OP_NAO ); }

   else                             {  strcpy( S, OP_SIM ); }

fim :

   fwsetcursor( T->win );
   return( ret );
}


F__LOC	VOID   atu_opcoes( campo, opcoes )
SS        campo;
SC        *(*opcoes)[];
{
   SC     *P, *S;
   TTela  *PI;

   PI = vet_evis[ campo ];



   P = ( *opcoes )[ 0 ];   S = P + strlen( P ) - strlen( OP_SIM );
   if ( PI->te_obrigatorio  ==  'S' )   strcpy( S, OP_SIM );
   else                                 strcpy( S, OP_NAO );


   P = ( *opcoes )[ 1 ];   S = P + strlen( P ) - strlen( OP_SIM );
   if ( PI->te_tipo == CAMPO_E  )       strcpy( S, OP_SIM );
   else                                 strcpy( S, OP_NAO );



   P = ( *opcoes )[ 2 ];   S = P + strlen( P ) - strlen( OP_SIM );
   if ( PI->te_append == 'S'    )       strcpy( S, OP_SIM );
   else                                 strcpy( S, OP_NAO );


   P = ( *opcoes )[ 3 ];   S = P + strlen( P ) - strlen( OP_SIM );
   if ( PI->te_autoclear == 'S' )       strcpy( S, OP_SIM );
   else                                 strcpy( S, OP_NAO );

}


F__LOC	VOID   atu_campos( campo, opcoes )
SS        campo;
SC        *(*opcoes)[];
{  SS     I, texto;
   SC     *P, *S;
   TTela  *PI;

   PI = vet_evis[ campo ];

   I  = acha_campo( PI->te_icb, PI->te_num);

   if ( I == -1 )
      return;

   texto = ( pos_vetcmp(PI->te_icb)[I]->cp_tipo == CP_TEXTO ) ? 1 : 0;

   P = ( *opcoes )[ 0 ];   S = P + strlen( P ) - strlen( OP_SIM );
   if ( !texto && strcmp( S, OP_SIM ) == 0 )  PI->te_obrigatorio  =  'S';
   else                                       PI->te_obrigatorio  =  'N';



   P = ( *opcoes )[ 1 ];   S = P + strlen( P ) - strlen( OP_SIM );
   if ( strcmp( S, OP_SIM ) == 0 )  PI->te_tipo  =  CAMPO_E;
   else                             PI->te_tipo  =  CAMPO_S;


   P = ( *opcoes )[ 2 ];   S = P + strlen( P ) - strlen( OP_SIM );
   if ( strcmp( S, OP_SIM ) == 0 )  PI->te_append = 'S';
   else                             PI->te_append = 'N';


   P = ( *opcoes )[ 3 ];   S = P + strlen( P ) - strlen( OP_SIM );
   if ( texto || (strcmp( S, OP_SIM ) == 0) )  PI->te_autoclear = 'S';
   else                                        PI->te_autoclear = 'N';
}




F__LOC	VOID   tipo_campos( win )
WIN    *win;
{
   SS    x1, y1;
   SS    item;

   x1 = win->w_cx;
   y1 = win->w_cy;

   for( item = -1;
      ( item = conflito_item(x1, y1, item, CAMPO_G)) != - 1; item++ )
   {
      atr_campo( item );
   }
}


F__LOC	VOID   vis_muda( win, campo )
WIN *win;
SS   campo;
{
   SS     I, C, x1, y1;
   WIN    *jm;
   TTela  *PI;
   Campo  **vetcp;

   alterou = 1;

   PI = vet_evis[ campo ];

   vetcp = pos_vetcmp(BASE_EDIT);

   x1 = PI->te_coluna;
   y1 = PI->te_linha;

   I  = acha_campo( PI->te_icb, PI->te_num);

   if ( I  !=  -1 )
   {
      insercol( JI, vetcp[ I ]->cp_nome );

      jm = abre_mensagem( M_TR_VISIB );

      while( 1 )
      {
         if ( ! w_kbhit(win) )
            lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL );

         fwat( x1, y1, win );

         C = fwqgc( win );

         if ( C >= '0' && C <='9' ) {
            w_ungc( C );
            C = vis_direto(PI, vetcp[ I ]->cp_tipo, vetcp[I]->cp_mhor, win );
            if ( C != K_ESC) {
               lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL );
               break;
            }
         }

         if ( C == K_ESC )    break;
         if ( C == K_F2  ) {  w_ungc( K_F2 );  break;  }

         switch( C )
         {
         case K_RT:
                     PI->te_vhor++;
                     if ( vetcp[ I ]->cp_tipo == CP_TEXTO )
                        PI->te_vhor = MIN(PI->te_vhor, vetcp[I]->cp_mhor + 1);
                     else
                        PI->te_vhor = MIN(PI->te_vhor, vetcp[I]->cp_mhor);
                     PI->te_vhor = MIN(PI->te_vhor, win->w_len - win->w_cx - 1);
                     break;

         case K_LFT:

                     if ( PI->te_vhor > 1 )   PI->te_vhor--;
                     break;

         case K_DN:
                     if ( vetcp[ I ]->cp_tipo == CP_TEXTO )
                     {
                        PI->te_vver++;
                        PI->te_vver = MIN( PI->te_vver,
                                           win->w_height - win->w_cy - 1 );
                     }
                     break;

         case K_UP:
                     if ( vetcp[ I ]->cp_tipo == CP_TEXTO )
                     {
                        if ( PI->te_vver > 1 )   PI->te_vver--;
                     }
                     break;
         }
      }

      fecha_mensagem( jm );
   }
}


F__LOC	VOID   visivel( win )
WIN    *win;
{
   SS    x1, y1;
   SS    item;

   x1 = win->w_cx;
   y1 = win->w_cy;

   for( item = -1;
      ( item = conflito_item( x1, y1, item, CAMPO_G ) ) != - 1; item++ )
   {
      vis_muda( win, item );
   }

   fwat( x1, y1, win );
}


F__LOC   TTela  *insere_item( tipo, lin, col, vis_ver, vis_hor, texto, num_campo, obrigatorio )
SS   tipo;
SS   lin, col, vis_hor, vis_ver;
SC   *texto;
SS   num_campo;
SS   obrigatorio;
{  SS tam, ordem;
   TTela  *PI;
   SC     *PT;

   alterou = 1;

   if ( texto == NULL )   texto = str_vazia;

   tam = sizeof( TTela ) + strlen( texto ) + 1;

   ordem = acha_ordem(tipo, tela, lin, col);

   PI = (TTela *) add_item(ap_vista.vis_vet, ordem, NULL, tam , 1);

   if ( PI == NULL ) {
      mens_erro(H_MXITEM, E_MXITEM, ordem);
      goto fim;
   }

   memset(PI, '\0', tam);


   PI->te_tela        = tela;
   PI->te_tipo        = tipo;
   PI->te_num         = num_campo;
   PI->te_base[0]     = '\0';
   PI->te_ivetcp      = -1;
   PI->te_icb         = BASE_EDIT;  /* ANANIAS @@@ */

   PI->te_linha       = lin;
   PI->te_coluna      = col;
   PI->te_vver        = vis_ver;
   PI->te_vhor        = vis_hor;
   PI->te_autoclear   = 'S';
   PI->te_append      = 'N';
   PI->te_obrigatorio = obrigatorio;

   PT = ( SC * ) PI + sizeof( TTela );
   strcpy( PT, texto );

fim:

	return ( PI );
}


F__LOC   SS item_anterior( p_tela, tipo, lin, col, n_campo )
SS   p_tela, tipo;
SS   lin, col, n_campo;
{
   SS      I;
   TTela  *PI;


   for ( I = 0;   (PI = pos_itvis(I)) != NULL;   I++ )
   {
      if ( PI->te_num  ==  n_campo   &&   PI->te_tipo  ==  tipo  )
      {
         if ( PI->te_tela  <  p_tela  )   return( I );

         else if ( PI->te_tela  ==  p_tela  )
         {
            if ( PI->te_linha  <  lin  )   return( I );

            else if ( PI->te_linha  ==  lin  )
            {
               if ( PI->te_coluna  <=  col )   return( I );
            }
         }
      }

      continue;
   }

	return ( -1 );
}


F__LOC   SS existe_item( e_tela, tipo, lin, col, vis_ver, vis_hor, num_campo )
SS   e_tela, tipo;
SS   lin, col, vis_hor, vis_ver;
SS   num_campo;
{
   SS      I;
   TTela  *PI;

   for ( I = 0;   (PI = pos_itvis(I)) != NULL;   I++ )
   {
      if ( e_tela    != - 1  &&  PI->te_tela   !=  e_tela    )   continue;
      if ( tipo      != - 1  &&  PI->te_tipo   !=  tipo      )   continue;
      if ( lin       != - 1  &&  PI->te_linha  !=  lin       )   continue;
      if ( col       != - 1  &&  PI->te_coluna !=  col       )   continue;
      if ( vis_ver   != - 1  &&  PI->te_vver   !=  vis_ver   )   continue;
      if ( vis_hor   != - 1  &&  PI->te_vhor   !=  vis_hor   )   continue;
      if ( num_campo != - 1  &&  PI->te_num    !=  num_campo )   continue;

      return ( I );
   }

	return ( -1 );
}


F__LOC	VOID  t_ins_linha( win )
WIN   *win;
{
   SS     I, Y;
   TTela  *PI;

   alterou = 1;

   Y = win->w_height - 1;

   while ( (I = existe_item( tela, -1, Y, -1, -1, -1, -1 ))  !=  -1 )
   {
      deleta_item( I );
   }

   for ( Y = win->w_height - 2;  Y >= win->w_cy;  Y-- )
   {
      while ( (I = existe_item( tela, -1, Y, -1, -1, -1, -1 ))  !=  -1 )
      {
         PI = vet_evis[ I ];

         PI->te_linha++;

         if ( PI->te_tipo == CAMPO_E  ||  PI->te_tipo == CAMPO_S )
         {
            if( PI->te_vver > win->w_height - PI->te_linha )
            {
               PI->te_vver--;
            }
         }

         else if ( PI->te_tipo == BORDA )
         {
            if( ++PI->te_vver >= win->w_height )
            {
               PI->te_vver--;
            }
         }
      }

      if ( Y == 0 ) break;
   }
}


F__LOC	VOID  del_linha( win )
WIN   *win;
{
   SS     I, Y;
   TTela  *PI;

   alterou = 1;

   Y = win->w_cy;

   while ( (I = existe_item( tela, -1, Y, -1, -1, -1, -1 ))  !=  -1 )
   {
      deleta_item( I );
   }

   for ( Y = win->w_cy + 1;  Y < win->w_height;  Y++ )
   {
      while ( (I = existe_item( tela, -1, Y, -1, -1, -1, -1 ))  !=  -1 )
      {
         PI = vet_evis[ I ];

         PI->te_linha--;

         if ( PI->te_tipo == BORDA )
         {
            if( PI->te_vver > 1 )
            {
               PI->te_vver--;
            }
         }
      }
   }
}


F__LOC   SS        centra_borda( E )
d_entry   *E;
{
   SC     dest[ MAX_EDIT + 1 ];

   dest[ 0 ] = '\0';

   if ( E->tecla == K_ESC  ||
        !centraliza( dest, E->str, strlen( E->mascara ) ) )
      return( 0 );

   strcpy( E->str, dest );

   return( 1 );
}


F__LOC   SS move_cursor( win, ch )
WIN   *win;
SS    ch;
{
   SS   ret = 1;

   switch ( ch )
   {
   default     :
                  ret = 0;
                  break;

   case K_RT   :
                  if ( ++win->w_cx  >=  win->w_len )  win->w_cx = 0;
                  fwat(  win->w_cx, win->w_cy, win );
                  break;

   case K_LFT  :
                  fwat( --win->w_cx, win->w_cy, win );
                  break;

   case K_UP   :
                  fwat( win->w_cx, --win->w_cy, win );
                  break;

   case K_DN   :
                  if ( ++win->w_cy  >=  win->w_height )  win->w_cy = 0;
                  fwat(  win->w_cx, win->w_cy, win );
                  break;

   case K_HOME :
                  if ( win->w_cx  >  0 )  {  win->w_cx = 0;  }
                  else    {  win->w_cx = 0;  win->w_cy = 0;  }
                  fwat( win->w_cx, win->w_cy, win );
                  break;

   case K_END  :
                  if( win->w_cx < win->w_len - 1 ) {  win->w_cx = 255;  }
                  else           {  win->w_cx = 255;  win->w_cy = 255;  }
                  fwat( win->w_cx, win->w_cy, win );
                  break;
   }

   return ( ret );
}


F__LOC	VOID   faz_borda( win )
WIN   *win;
{
   WIN   *JM;
   SS    ch;
   SS    x1, x2, y1, y2;
   SS    x3, x4, y3, y4;
   SS    I;
   SC    masc[ MAX_EDIT + 1 ];
   SC    str[  MAX_EDIT + 1 ];
   d_entry   E;

   insercol( JI, I_BORDA );

   x1 = win->w_cx;  y1 = win->w_cy;

   fwpc( UG_JUN, win );

   fwat( x1, y1, win );

   JM = abre_mensagem( M_TR_DBORDA );

   while ( 1 ) {
      ch = fwqgc( win );

      if ( move_cursor( win, ch ) )   continue;

      else if ( ch == K_ESC )
      {
         fwat(x1,y1,win);
         goto   sai;
      }

      else if ( ch == K_ENTER )
      {
         x2 = win->w_cx;
         y2 = win->w_cy;
         break;
      }

      else    win_bell( BEL_INVALID );
   }

   x3 = MIN( x1, x2 );    y3 = MIN( y1, y2 );

   x4 = MAX( x1, x2 );    y4 = MAX( y1, y2 );

   if ( x1 != x2  &&  y1 != y2  &&  x4 > x3 + 2  &&  y4 > y3 + 1 )
   {
      fwframe( y3, x3, y4, x4, 1, win );

      for ( I = 0;  I < x4 - x3 - 1;  I++ )
      {
         masc[ I ] = 'x';  str[ I ] = UG_HOR;
      }

      masc[ I ] = '\0';   str[ I ] = '\0';

      win->w_optatr = atributo;

      d_entrada( &E, win, str, x4 - x3, masc, M_TR_TITBORDA,
                                    x3 + 1, y3, E_ALFA | E_CONFIRMA );

      E.critica = centra_borda;

      entrada( &E, 0 );

      win->w_optatr = win->w_boxatr;

      fwat( x3 + 1, y3 + 1, win );

      insere_item( BORDA, y3, x3, y4, x4, E.str, -1, 'N' );

      lista_vista( win, BASE_EDIT, tela, CRIACAO , NULL);
   }

   else
   {
      fwat( x1, y1, win );
   }

sai:

   fecha_mensagem( JM );
}



F__LOC	VOID   faz_linha( win )
WIN   *win;
{
   WIN   *JM;
   SS    ch;
   SS    x1, x2, y1, y2;
   SS    x3, x4, y3, y4;

   insercol( JI, I_LINHA );

   x1 = win->w_cx;
   y1 = win->w_cy;
   fwpc( UG_HOR, win );
   fwat( x1, y1, win );

   JM = abre_mensagem( M_TR_DLINHA );

   while ( 1 )
   {
      ch = fwqgc( win );

      if ( move_cursor( win, ch ) )   continue;

      else if ( ch == K_ESC )
      {
         fwat( x1, y1, win );
         goto   sai;
      }

      else if ( ch == K_ENTER )
      {
         x2 = win->w_cx;
         y2 = win->w_cy;
         break;
      }

      else    win_bell( BEL_INVALID );
   }

   x3 = MIN( x1, x2 );    y3 = MIN( y1, y2 );

   x4 = MAX( x1, x2 );    y4 = MAX( y1, y2 );

   if ( x1 == x2  ||  y1 == y2 )
   {
      if ( x1 == x2 )
      {
         linha_ver( x3, y3, y4, win );
         insere_item( LIN_VER, y3, x3, y4, x4, NULL, -1, 'N');
      }

      else
      {
         linha_hor( y3, x3, x4, win );
         insere_item( LIN_HOR, y3, x3, y4, x4, NULL, -1, 'N');
      }

      lista_vista( win, BASE_EDIT, tela, CRIACAO , NULL);
   }

   fwat( x1, y1, win );

sai:

   fecha_mensagem( JM );
}


F__LOC   SS conflito_item ( x1, y1, item, tipo )
SS       x1, y1, item;
SS       tipo;
{
	SS     I;
   TTela  *PI;


   for ( I = ( item <= 0 ) ? 0 : item; (PI = pos_itvis(I)) != NULL;   I++ )
   {
      if ( PI->te_tela  !=  tela )   continue;

      if ( tipo  !=  -1 )
      {
         if ( tipo  ==  CAMPO_G )
         {
            if( PI->te_tipo != CAMPO_E && PI->te_tipo != CAMPO_S ) continue;
         }

         else  if ( tipo  !=  PI->te_tipo )  continue;
      }

      switch ( PI->te_tipo )
      {
      case BORDA :
                  if ( x1 == PI->te_coluna  ||  x1 == PI->te_vhor )
                  {
                     if ( y1 >= PI->te_linha  &&  y1 <= PI->te_vver )
                     {
                        goto   sai;
                     }
                  }

                  if ( y1 == PI->te_linha  ||  y1 == PI->te_vver )
                  {
                     if ( x1 >= PI->te_coluna  &&  x1 <= PI->te_vhor )
                     {
                        goto   sai;
                     }
                  }

                  break;

      case LIN_VER :
                  if ( x1 == PI->te_coluna )
                  {
                     if ( y1 >= PI->te_linha  &&  y1 <= PI->te_vver )
                     {
                        goto   sai;
                     }
                  }

                  break;

      case LIN_HOR :
                  if ( y1 == PI->te_linha )
                  {
                     if ( x1 >= PI->te_coluna  &&  x1 <= PI->te_vhor )
                     {
                        goto   sai;
                     }
                  }

                  break;

      case CAMPO_E :
      case CAMPO_S :
                  if ( x1 >= PI->te_coluna       &&
                       x1 < PI->te_coluna + PI->te_vhor )
                  {
                     if ( y1 >= PI->te_linha     &&
                          y1 < PI->te_vver + PI->te_linha )
                     {
                        goto   sai;
                     }
                  }

                  break;
      }
   }

   I = -1;

sai:

	return( I );
}


F__LOC   SS del_confirma ( item  )
SS     item;
{
   SC     *P1, del[ 130 ], T1[ 30 ];
   TTela  *PI;
   TEXT snaux[3];
   SC     *PT;
   SS     I;

   snaux[0] = OP_SIM[0];
   snaux[1] = OP_NAO[0];
   snaux[2] = '\0';

   PI = vet_evis[ item ];   PT = ( SC * ) PI + sizeof( TTela );

   switch ( PI->te_tipo )
   {
   case BORDA :
               while( (UC) *PT == UG_HOR )  PT++;
               for( P1 = T1; (UC) *PT != UG_HOR  &&
                             (UC) *PT != '\0'; *P1++ = *PT++ ) /* Nada */;
               *P1 = '\0';
               P1 = strlen( T1 ) > 15 ? "..." : str_vazia;
               sprintf( del, "%s \"%.15s%s\" (%c/%c) ? ",
                              M_TR_DELBOX, T1, P1, OP_SIM[0], OP_NAO[0]);
               break;

   case LIN_VER :
               sprintf( del, "%s (%c/%c) ? ", M_TR_DELVER, OP_SIM[0], OP_NAO[0] );
               break;

   case LIN_HOR :
               sprintf( del, "%s (%c/%c) ? ", M_TR_DELHOR, OP_SIM[0], OP_NAO[0] );
               break;

   case CAMPO_E :
   case CAMPO_S :

               I  = acha_campo( PI->te_icb, PI->te_num);

               if ( I == -1 )   return( 1 );

               P1 = pos_vetcmp(PI->te_icb)[I]->cp_nome;

               sprintf( del, "%s \"%s\" (%c/%c) ? ", M_TR_DELCAM, P1,
                             OP_SIM[0], OP_NAO[0] );
               break;
   }

   if ( men_conf( del, snaux, H_CFITDEL, NULL ) == 0 )   return( 1 );

   else                                 return( 0 );
}


F__LOC	VOID  deleta_item ( item )
SS    item;
{
   alterou = 1;

   if ( pos_item( ap_vista.vis_vet, item) == NULL )
      numdebug(210);

   del_item(ap_vista.vis_vet, 1);
}



F__LOC   SS apaga( win )
WIN   *win;
{
   SS    item, ret;
   SS    x1, y1;

   insercol( JI, I_APAGA );

   x1 = win->w_cx;
   y1 = win->w_cy;

   ret  = 0;
   item = -1;

   while ( 1 )
   {
      if (( item = conflito_item ( x1, y1, item, -1 )) == - 1 ) break;

      ret++;

      if ( del_confirma ( item  ) )
      {
         deleta_item ( item );

         lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL);

         continue;
      }

      item++;
   }

   return( ret );
}



F__LOC	VOID     faz_campo( win )
WIN   *win;
{
   TEXT *ajuda_ant = win_ajuda;
   SS    x1, y1, obrig;
   Campo *cmp;

   insercol( JI, I_CAMPO );

   x1 = win->w_cx;
   y1 = win->w_cy;

   fwpc( UG_DOT, win );
   fwat( x1, y1, win );

   fwrefresh( win );

   if ( sel_campo(BASE_EDIT, &cmp, 0) != NULL ) {

      SS  linha, coluna;
      SS  tipo, vver, vhor, num, achou;
      SC  *pt_texto;

      num    = cmp->cp_num;
      linha  = y1;
      coluna = x1;
      tipo   = CAMPO_S;
      obrig  = 'N';

      if ( cmp->cp_tipo == CP_TEXTO )
      {
         if ( linha  == 0 )   linha++;
         if ( coluna == 0 )   coluna++;
         vhor = MIN( cmp->cp_mhor + 1, win->w_len - coluna - 1 );
      }
      else
      {
         vhor = MIN( cmp->cp_mhor, win->w_len - coluna - 1 );
      }

      if ( cmp->cp_tipo == CP_TEXTO )
      {
         TEXT *p, *p1;
         vver = 5;
         vver = MIN( vver, win->w_height - linha - 1 );

         p = cmp->cp_desc;
         p1 = strchr(p, ']');
         if ( p1 != NULL )
            p = p1+1;

         pt_texto = p;
      }

      else {  vver = 1;   pt_texto = str_vazia;  }

      achou = item_anterior( tela, CAMPO_E, linha, coluna, num );

      if ( achou == -1 && cmp->cp_tipo != 'F' ) {
         tipo = CAMPO_E;
         if ( cmp->cp_critica == 'T' || cmp->cp_chave_unica == 'S')
            obrig = 'S';
      }

      insere_item( tipo, linha, coluna, vver, vhor, pt_texto, num, obrig );

      lista_vista( win, BASE_EDIT, tela, CRIACAO, NULL );
   }

   fwat( x1, y1, win );

   win_ajuda = ajuda_ant;
}

F__LOC   SS ed_linha( win )
WIN   *win;
{
   SC     str[  MAX_EDIT + 1 ];
   SC     masc[ MAX_EDIT + 1 ];
   US     J, *P;
   SS     I, col;
   UC     escape[ 10 ];
   TTela  *PI;
   d_entry   E;
   WIN *JF;

   JF = abre_funcoes( funcao( "1 2 3 E") );

   insercol( JI, I_EDITL );

   for ( I = 0;   I < MAX_EDIT;   I++ )   masc[ I ] = 0;

   /*** Proteje Titulos das Bordas ***/
   for ( I = 0;   (PI = pos_itvis(I)) != NULL;   I++ )
   {
      if ( PI->te_tipo  !=  BORDA )       continue;
      if ( PI->te_tela  !=  tela )        continue;
      if ( PI->te_linha !=  win->w_cy )   continue;

      for ( J = 0;  J < PI->te_vhor - PI->te_coluna;  J++ )
      {
         masc[ PI->te_coluna + J ] = 'P';
      }
   }

   /*** Proteje Titulos dos Campos TEXTO ***/
   for ( I = 0;   (PI = pos_itvis(I)) != NULL;   I++ )
   {
      SS K;
      Campo  **vetcp;

      vetcp    = pos_vetcmp(BASE_EDIT);

      if ( PI->te_tipo != CAMPO_S && PI->te_tipo != CAMPO_E )   continue;
      if ( PI->te_tela != tela )    continue;
      if ( PI->te_linha - 1 != win->w_cy )   continue;

      K  = acha_campo( PI->te_icb, PI->te_num);

      if ( K == - 1  ||  vetcp[ K ]->cp_tipo != CP_TEXTO )   continue;

      for ( J = 0;   J < PI->te_vhor;   J++ )
      {
         masc[ J + PI->te_coluna ] = 'P';
      }
   }

   P  = win->w_ary + ( win->w_cy * win->w_len );

   for ( I = 0;  I < win->w_len;  I++, P++ )
   {
      if ( e_grafico( ( UC ) *P ) )  masc[ I ] = 'P';

      else if ( masc[ I ] != 'P' )   masc[ I ] = 'x';

      str[ I ] =  *P;
   }

   masc[ I ] = '\0';   str[ I ] = '\0';

   d_entrada( &E, win, str, win->w_len, masc, str_nada, 0, win->w_cy,
              E_ALFA | E_CONFIRMA );

   escape[ 0 ] = K_F2;
   escape[ 1 ] = 14;   /*** CTRl N ***/
   escape[ 2 ] = 25;   /*** CTRl Y ***/
   escape[ 3 ] = '\0';

   E.escape = escape;

	entrada( &E, win->w_cx );

   if ( E.mudou )
   {
      col = -1;

      for ( I = 0;  I < win->w_len;  I++ )
      {
         if ( masc[ I ] ==  'P' )  str[ I ] = ' ';
      }

      for ( I = 0;  I < win->w_len;  I++ )
      {
         if ( masc[ I ] ==  'P' )  continue;

         if ( e_print( str[ I ] )  &&  str[ I ] != ' ' )   break;
      }

      if ( I  <  win->w_len )
      {
         col = I;

         for ( I = win->w_len - 1;  I > col;  I-- )
         {
            if ( masc[ I ] ==  'P' )  continue;

            if ( e_print( str[ I ] )  &&  str[ I ] != ' ' )   break;
         }

         str[ I + 1 ] = '\0';
      }

      if ((I = existe_item( tela, NOMES, win->w_cy, -1, -1, -1, -1 )) != -1 )
      {
         deleta_item( I );
      }

      if ( col  !=  -1 )
      {
         insere_item( NOMES, win->w_cy, col, -1, -1, &str[ col ], -1, 'N');
      }
   }

   fecha_funcoes(  JF );

   return ( E.tecla );
}


F__LOC	VOID   disp_lc( W, bot, lin, col )
WIN *W;
SC  *bot;
SS  lin, col;
{
   lin++; col++;

   if ( col > 0 )   sprintf( bot, " Lin: %d Col: %d ", lin, col );
   else             sprintf( bot, " Lin: %d ", lin );

   fw_bottom( W, bot );
}



F__LOC   SS vis_direto(PI, tipo, tam, w)  /*-------------------------*/
TTela *PI;
COUNT tipo, tam;
WIN *w;
{  COUNT ret, lin, col, max;
   WIN *jt;
   TEXT str_vis[20],aux[10];
   DTela nr[2];

   nr[0].destino = NULL;
   nr[1].destino = NULL;

   nr[0].destino = str_vis;

   if ( tipo == CP_TEXTO ) {
      nr[0].mascara = "99/99";
      nr[0].rodape  = M_VISLC;
   }
   else {
      nr[0].mascara = "99";
      nr[0].rodape  = M_VISCOL;
   }

   nr[0].linha   = 1;
   nr[0].coluna  = 7;
   nr[0].visivel = 0;
   nr[0].param   = E_ALFA;
   nr[0].help    = H_TAMVIS;
   nr[0].prep    = NULL;
   nr[0].crit    = NULL;


   ret = K_ESC;

   if ( (jt = abre_win(10, 28, 12, 52, 0, 0, JS)) != NULL ) {

      fw_title(jt, M_VISTIT, jt->w_atrib);

	   fwbox(jt);

      str_vis[0] = '\0';

      ret = ed_read(jt, &nr[0], 1, LIMPA_LE, "1 3 E");

      if (ret != K_ESC) {

         if ( tipo == CP_TEXTO ) {
            piece(str_vis, aux, '/', 1, 5 );   lin = abs(atoi(aux));
            piece(str_vis, aux, '/', 2, 5 );   col = abs(atoi(aux));
         }
         else {
            lin = 1;
            col = abs(atoi(str_vis));
         }

         lin = COTA(1, lin, (w->w_height - w->w_cy - 1));

         if ( tipo == CP_TEXTO )
            max = tam + 1;
         else
            max = tam;

         max = MIN(max, w->w_len - w->w_cx - 1);
         col = COTA(1, col, max);

         PI->te_vver = lin;
         PI->te_vhor = col;
      }
   }


fim :

   fwkill(jt);

   return(ret);
}


F__LOC   COUNT acha_ordem(tipo, tela, lin, col)
SS tipo, tela, lin, col;
{  SS I, T1, T2;
   TTela *PI;

   switch ( tipo ) {
      case NOMES   : T1 = 1;   break;
      case LIN_VER :
      case LIN_HOR : T1 = 2;   break;
      case BORDA   : T1 = 3;   break;
      case CAMPO_S : T1 = 4;   break;
      case CAMPO_E : T1 = 5;   break;
   }

   for ( I = 0;   (PI = pos_itvis(I)) != NULL;   I++ ) {

      switch ( PI->te_tipo ) {
         case NOMES   : T2 = 1;   break;
         case LIN_VER :
         case LIN_HOR : T2 = 2;   break;
         case BORDA   : T2 = 3;   break;
         case CAMPO_S : T2 = 4;   break;
         case CAMPO_E : T2 = 5;   break;
      }

      if ( tela < PI->te_tela  )   break;
      if ( tela > PI->te_tela  )   continue;

      if ( T1   < T2 )  break;
      else
         if ( T1 == T2 )
            if ( lin < PI->te_linha ) break;
            else
               if ( lin == PI->te_linha )
                  if ( col < PI->te_coluna )   break;
   }

   return(I);
}
