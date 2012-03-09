#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
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



#define   GRP_DFREL    22


#define   e_grafico(c)   ((c>=129 && c <=152) ? 1 : 0 )


#define   H_CFITDEL       "CF_ITDEL"  /* confirmacao de delecao de itens */

#define  MN_R_BORDAS    loc_str(GRP_DFREL, 15)
#define  MN_R_CAMPOS    loc_str(GRP_DFREL, 16)
#define  MN_R_LINHAS    loc_str(GRP_DFREL, 17)
#define  MN_R_APAG2     loc_str(GRP_DFREL, 18)
#define  MN_R_VISIB     loc_str(GRP_DFREL, 19)
#define  MN_R_INS_LIN   loc_str(GRP_DFREL, 20)
#define  MN_R_RET_LIN   loc_str(GRP_DFREL, 21)
#define  MN_R_CARAC     loc_str(GRP_DFREL, 22)


#define  RD_R_BOR       loc_str(GRP_DFREL, 23)
#define  RD_R_CAM       loc_str(GRP_DFREL, 24)
#define  RD_R_LIN       loc_str(GRP_DFREL, 25)
#define  RD_R_APA       loc_str(GRP_DFREL, 26)
#define  RD_R_REL       loc_str(GRP_DFREL, 27)
#define  RD_R_INS       loc_str(GRP_DFREL, 28)
#define  RD_R_RET       loc_str(GRP_DFREL, 29)
#define  RD_R_CAR       loc_str(GRP_DFREL, 30)

#define  I_BORDA    loc_str(GRP_DFREL, 31)
#define  I_LINHA    loc_str(GRP_DFREL, 32)
#define  I_APAGA    loc_str(GRP_DFREL, 33)
#define  I_CAMPO    loc_str(GRP_DFREL, 34)
#define  I_EDITL    loc_str(GRP_DFREL, 35)
#define  I_EDITT    loc_str(GRP_DFREL, 36)


#define  M_TR_VISIB     loc_str(GRP_DFREL, 37)
#define  M_TR_DBORDA    loc_str(GRP_DFREL, 38)
#define  M_TR_TITBORDA  loc_str(GRP_DFREL, 39)
#define  M_TR_DLINHA    loc_str(GRP_DFREL, 40)
#define  M_TR_DELBOX    loc_str(GRP_DFREL, 41)
#define  M_TR_DELVER    loc_str(GRP_DFREL, 42)
#define  M_TR_DELHOR    loc_str(GRP_DFREL, 43)
#define  M_TR_DELCAM    loc_str(GRP_DFREL, 44)

#define  M_TR_NAVREL    loc_str(GRP_DFREL, 45)
#define  M_TR_FUNED     loc_str(GRP_DFREL, 46)

#define  C_RLLICOL     loc_str(GRP_DFREL, 56)
#define  C_RLSOLIN     loc_str(GRP_DFREL, 57)


#ifdef MSDOS
   SS       tira_fora( SS, SS);
   VOID     rel_muda( WIN *, SS );
   VOID     rvisivel( WIN * );
   VOID     rlista_rel( WIN *, SS, SS, SC*);
   RTela   *rinsere_item( SS, SS, SS, SS, SS, SC *, SS );
   SS       ritem_anterior( SS, SS, SS, SS, SS );
   SS       rexiste_item( SS, SS, SS, SS, SS, SS, SS );
   VOID     rins_linha( WIN * );
   VOID     rdel_linha( WIN * );
   SS       centra_borda( d_entry *);
   SS       rmove_cursor( WIN *, SS );
   VOID     rfaz_borda( WIN * );
   VOID     rfaz_linha( WIN * );
   SS       rconflito_item ( SS, SS, SS, SS );
   SS       rdel_confirma ( SS  );
   VOID     rdeleta_item ( SS );
   SS       rapaga( WIN * );
   VOID     rfaz_campo( WIN * );
   SS       red_linha( WIN * );
   VOID     rdisp_lc( WIN *, SC *, SS, SS);
   SS       racha_ordem(SS, SS, SS, SS);
#else
   SS       tira_fora( );
   VOID     rel_muda( );
   VOID     rvisivel( );
   VOID     rlista_rel( );
   RTela   *rinsere_item( );
   SS       ritem_anterior( );
   SS       rexiste_item( );
   VOID     rins_linha( );
   VOID     rdel_linha( );
   SS       centra_borda( );
   SS       rmove_cursor( );
   VOID     rfaz_borda( );
   VOID     rfaz_linha( );
   SS       rconflito_item ( );
   SS       rdel_confirma ( );
   VOID     rdeleta_item ( );
   SS       rapaga( );
   VOID     rfaz_campo( );
   SS       red_linha( );
   VOID     rdisp_lc( );
   SS       racha_ordem( );
#endif



static UC        alterou;
static UC        tela;
static SS        atributo;
static SS        no_menu;
static WIN       *JI;


static RTela **vet_erel;


F__GLB   SS ed_relat( foi_modificado, lins, cols )
SS *foi_modificado;
SS lins, cols;
{
   TEXT *ajuda_ant = win_ajuda;
   SS    I1, I2, ch, L, C, LF, CF;
   WIN   *win = NULL, *JM, *JF, *w_fundo = NULL;
   RTela *PI;
   Campo **vetcp;
   SC    *m_opc[(9 * 3) + 1];
   SC    *p = "1";
   Selecao   M;
   static SC bottom[ 50 ];


   win_ajuda = "DEFREL";

   set_menu(m_opc, 0, MN_R_BORDAS , p, RD_R_BOR);
   set_menu(m_opc, 1, MN_R_CAMPOS , p, RD_R_CAM);
   set_menu(m_opc, 2, MN_R_LINHAS , p, RD_R_LIN);
   set_menu(m_opc, 3, MN_R_APAG2  , p, RD_R_APA);
   set_menu(m_opc, 4, MN_R_VISIB  , p, RD_R_REL);
   set_menu(m_opc, 5, MN_R_INS_LIN, p, RD_R_INS);
   set_menu(m_opc, 6, MN_R_RET_LIN, p, RD_R_RET);
   set_menu(m_opc, 7, MN_R_CARAC,   p, RD_R_CAR);
   set_menu(m_opc, 8, NULL,       "0", NULL);



   if ( lins < size_y-5 || cols < size_x-2 )  {
      w_fundo = abre_win(2, 1, size_y-5, size_x-2, size_y-5, size_x-2, JP);
      if ( w_fundo == NULL )  return( -1 );
   }
   else w_fundo = NULL;


   L   = (size_y - 5) - lins;       /* centralizar janela */
   C   = (size_x - 2) - cols;

   L   = 2 + (( L > 1 ) ? (L / 2 ) : 0);
   C   = 1 + (( C > 1 ) ? (C / 2 ) : 0);

   LF = MIN(lins, (size_y - 5));
   CF = MIN(cols, (size_x - 2));

   win = abre_win(L, C, L + lins - 1, C + cols - 1, LF, CF, JP);

   if ( win == NULL )  return( -1 );

   win->w_flags  &= 1;
   win->w_flags  |= W_VISIB;

   atributo       = win->w_optatr;
   win->w_optatr  = win->w_boxatr;
   win->campoatr  = win->w_boxatr;


   JM = abre_mensagem( M_TR_NAVREL );
   JF = abre_funcoes( funcao( M_TR_FUNED ) );
   JI = abre_insercol( str_nada );

	fwat( 0, 0, win );

   sel_campo(BASE_EDIT, NULL, 1 ); /* 1 so resetar ultimo campo */

   vet_erel = (RTela **) ap_rel.rel_vet->vi_vtit;
   vetcp    = pos_vetcmp(BASE_EDIT);

   alterou = 0;
   tela = 1;
   no_menu = 0;

   if ( tira_fora(lins, cols) != 0 )
      alterou = 1;


   rlista_rel( win, tela, CRIACAO, NULL);

   fwat( 0, 0, win );

   while ( 1 )
   {
      I1 = rconflito_item( win->w_cx, win->w_cy, -1, CAMPO_G );

      if ( I1 != -1 )
      {
         PI = vet_erel[ I1 ];
         I2 = acha_campo( PI->re_icb, PI->re_num);
      }

      if ( I1 == -1  ||  I2 == -1 )
         insercol( JI, I_EDITT );
      else
         insercol( JI, vetcp[ I2 ]->cp_nome );

      if ( ! w_kbhit(win) )
         rdisp_lc( win, bottom, win->w_cy, win->w_cx );

      ch = fwqgc( win );

      if ( rmove_cursor( win, ch ) )   continue;

      switch ( ch )
      {
      case K_PGDN  :
                     if ( rexiste_item( tela + 1, -1, -1, -1,
                                                 -1, -1, -1 ) == -1 )
                     {
                        if ( rexiste_item( tela,  -1, -1, -1,
                                                 -1, -1, -1 ) == -1 )
                        {
                           break;
                        }
                     }

                     tela = 1;
                     rlista_rel( win, tela, CRIACAO, NULL);
 			            break;

      case K_PGUP  :
                     tela = 1;
                     rlista_rel( win, tela, CRIACAO, NULL);
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

                     switch ( menu( &M ) )
                     {
                     default:
                              break;

                     case 0 : /*** BORDA ***/
                              rfaz_borda( win );
                              break;

                     case 1 : /*** CAMPOS ***/
                              rfaz_campo( win );
                              break;

                     case 2 : /*** LINHA ***/
                              rfaz_linha( win );
                              break;

                     case 3 : /*** APAGA ***/
                              rapaga( win );
                              break;

                     case 4 : /*** VISIVEL ***/
                              rvisivel( win );
                              break;

                     case 5 : /*** INSERE LINHA ***/
                              rins_linha( win );
                              break;

                     case 6 : /*** RETIRA LINHA ***/
                              rdel_linha( win );
                              break;

                     case 7 : /*** CARACTERISTICAS DO RELATORIO ***/
                              if ( carac_rel( ) >= 0 )
                                 alterou = 1;
                              break;
                     }

                     no_menu = M.escolhido;
                     rlista_rel( win, tela, CRIACAO, NULL );
 			            break;

      case 14      : /*** INSERE LINHA - CTRL N ***/
                     rins_linha( win );
                     rlista_rel( win, tela, CRIACAO, NULL);
                     break;

      case 25      : /*** RETIRA LINHA - CTRL Y ***/
                     rdel_linha( win );
                     rlista_rel( win, tela, CRIACAO, NULL);
                     break;

      case K_ESC   :
                     if ( alterou )   ch = 1;
                     else             ch = 0;
      case K_F2    : goto fim;

      case K_DEL   :
                     /* Se nao houver item a ser apagado */
                     /* Execute opcao DEFAULT para ser usado em ed_linha() */
                     if ( rapaga( win ) )   break;
      case K_BS    :
      default      :
                     if( e_print( ch ) || ch == K_BS || ch == K_DEL )
                     {
                        w_ungc( ch );

                        rdisp_lc( win, bottom, win->w_cy, -1 );

                        ch = red_linha( win );

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

   if ( w_fundo != NULL ) fwkill( w_fundo );

   win_ajuda = ajuda_ant;
   return( ch );
}



F__LOC   SS  tira_fora( lins, cols)
SS lins, cols;
{
   SS      I, ret = 0;
   RTela  *PI;

   for ( I = 0;   (PI = pos_itrel(I)) != NULL;   I++ )
   {
      if ( PI->re_linha  >=  lins  || PI->re_coluna >= cols ) {
         ret = 1;
         rdeleta_item( I );
         I -= 1;
      }
   }

	return ( ret );
}

F__LOC   VOID   rel_muda( win, campo )
WIN *win;
SS   campo;
{
   SS     I, C, x1, y1, minimo;
   WIN    *jm;
   RTela  *PI;
   Campo  **vetcp;

   alterou = 1;

   PI = vet_erel[ campo ];

   vetcp = pos_vetcmp(BASE_EDIT);

   x1 = PI->re_coluna;
   y1 = PI->re_linha;

   I  = acha_campo( PI->re_icb, PI->re_num);

   if ( I  !=  -1 )
   {
      insercol( JI, vetcp[ I ]->cp_nome );

      jm = abre_mensagem( M_TR_VISIB );

      while( 1 )
      {
         if ( ! w_kbhit(win) )
            rlista_rel( win, tela, CRIACAO, NULL );

         fwat( x1, y1, win );

         C = fwqgc( win );

         if ( C == K_ESC )    break;
         if ( C == K_F2  ) {  w_ungc( K_F2 );  break;  }

         switch( C )
         {
         case K_RT:
                     PI->re_vhor++;
                     if ( vetcp[ I ]->cp_tipo == CP_TEXTO )
                        PI->re_vhor = MIN(PI->re_vhor, vetcp[I]->cp_mhor + 1);
                     else
                        PI->re_vhor = MIN(PI->re_vhor, vetcp[I]->cp_mhor);
                     PI->re_vhor = MIN(PI->re_vhor, win->w_len - win->w_cx - 1);
                     break;

         case K_LFT:
                     if ( vetcp[ I ]->cp_tipo == CP_TEXTO )
                        minimo = MIN(15, vetcp[ I ]->cp_mhor);
                     else
                        minimo = 1;

                     if ( PI->re_vhor > minimo )   PI->re_vhor--;
                     break;
         }
      }

      fecha_mensagem( jm );
   }
}


F__LOC   VOID   rvisivel( win )
WIN    *win;
{
   SS    x1, y1;
   SS    item;

   x1 = win->w_cx;
   y1 = win->w_cy;

   for( item = -1;
      ( item = rconflito_item( x1, y1, item, CAMPO_G ) ) != - 1; item++ )
   {
      rel_muda( win, item );
   }

   fwat( x1, y1, win );
}


F__LOC   VOID  rlista_rel( win, num_tela, flag , rodape)
WIN       *win;
SS        num_tela;
SS        flag;
SC       *rodape;
{
   US     X, Y;
   US     AX, AY;
	SS     I, J, n_itens;
   UC     ug_dot1[ MAX_EDIT + 1 ];
   UC     ug_dot2[ MAX_EDIT + 1 ];
   RTela  *PI, **vterel;
   SC     *PT;

   X = win->w_cx;
   Y = win->w_cy;

   AX = win->w_ax;
   AY = win->w_ay;


   vterel   = (RTela **) ap_rel.rel_vet->vi_vtit;
   n_itens  = ap_rel.rel_vet->vi_nelem;

   for ( I = 0;   I < MAX_EDIT;   I++  )
   {
      ug_dot1[ I ] = (flag == CRIACAO) ? UG_DOT  : ' ';
      ug_dot2[ I ] = (flag == CRIACAO) ? UG_DOT2 : ' ';
   }

   fw_title( win, ap_rel.rel_ident, win->t_atr );

   if ( rodape != NULL )
      fw_bottom( win, rodape );

   fwcls( win );

   for ( I = 0;   I < n_itens;   I++ )
   {
      PI = vterel[ I ];   PT = ( SC * ) PI + sizeof( RTela );

      if ( PI->re_tela  !=  num_tela )   continue;

      switch ( PI->re_tipo )
      {
      case BORDA :
                  fwframe( PI->re_linha, PI->re_coluna,
                           PI->re_vver, PI->re_vhor, 1, win );

                  fwat( PI->re_coluna + 1, PI->re_linha, win );

                  fwpsl( ( UC * ) PT, win, win->w_len );

                  break;

      case LIN_VER :
                  linha_ver( PI->re_coluna, PI->re_linha, PI->re_vver, win );
                  break;

      case LIN_HOR :
                  linha_hor( PI->re_linha, PI->re_coluna, PI->re_vhor, win );
                  break;

      case NOMES :
                  fwat(  PI->re_coluna, PI->re_linha, win );
                  fwpsl( ( UC * ) PT, win, win->w_len );
                  break;

      case CAMPO_E :
                  for ( J = 0;   J < PI->re_vver;   J++ )
                  {
                     fwat( PI->re_coluna, PI->re_linha + J, win );
                     fwpsl( ug_dot1, win, PI->re_vhor );
                  }

                  break;

      case CAMPO_S :
                  for ( J = 0;   J < PI->re_vver;   J++ )
                  {
                     fwat( PI->re_coluna, PI->re_linha + J, win );
                     fwpsl( ug_dot2, win, PI->re_vhor );
                  }

                  break;
      }
   }

   win->w_ax = AX;
   win->w_ay = AY;

   fwat( X, Y, win );
}


F__LOC   RTela  *rinsere_item( tipo, lin, col, rel_ver, rel_hor, texto, num_campo )
SS   tipo;
SS   lin, col, rel_hor, rel_ver;
SC   *texto;
SS   num_campo;
{  SS tam, ordem;
   RTela  *PI;
   SC     *PT;

   alterou = 1;

   if ( texto == NULL )   texto = str_vazia;

   tam = sizeof( RTela ) + strlen( texto ) + 1;

   ordem = racha_ordem(tipo, tela, lin, col);

   PI = (RTela *) add_item(ap_rel.rel_vet, ordem, NULL, tam , 1);

   if ( PI == NULL ) {
      mens_erro(H_MXITEM, E_MXITEM, ordem);
      goto fim;
   }

   memset(PI, '\0', tam);

   PI->re_tela        = tela;
   PI->re_tipo        = tipo;
   PI->re_num         = num_campo;
   PI->re_base[0]     = '\0';
   PI->re_ivetcp      = -1;
   PI->re_icb         = BASE_EDIT;

   PI->re_linha       = lin;
   PI->re_coluna      = col;
   PI->re_vver        = rel_ver;
   PI->re_vhor        = rel_hor;



   PT = ( SC * ) PI + sizeof( RTela );
   strcpy( PT, texto );

fim:

	return ( PI );
}


F__LOC   SS  ritem_anterior( p_tela, tipo, lin, col, n_campo )
SS   p_tela, tipo;
SS   lin, col, n_campo;
{
   SS      I;
   RTela  *PI;

   for ( I = 0;   (PI = pos_itrel(I)) != NULL;   I++ )
   {
      if ( PI->re_num  ==  n_campo   &&   PI->re_tipo  ==  tipo  )
      {
         if ( PI->re_tela  <  p_tela  )   return( I );

         else if ( PI->re_tela  ==  p_tela  )
         {
            if ( PI->re_linha  <  lin  )   return( I );

            else if ( PI->re_linha  ==  lin  )
            {
               if ( PI->re_coluna  <=  col )   return( I );
            }
         }
      }

      continue;
   }

	return ( -1 );
}


F__LOC   SS  rexiste_item( e_tela, tipo, lin, col, rel_ver, rel_hor, num_campo )
SS   e_tela, tipo;
SS   lin, col, rel_hor, rel_ver;
SS   num_campo;
{
   SS      I;
   RTela  *PI;

   for ( I = 0;   (PI = pos_itrel(I)) != NULL;   I++ )
   {
      if ( e_tela    != - 1  &&  PI->re_tela   !=  e_tela    )   continue;
      if ( tipo      != - 1  &&  PI->re_tipo   !=  tipo      )   continue;
      if ( lin       != - 1  &&  PI->re_linha  !=  lin       )   continue;
      if ( col       != - 1  &&  PI->re_coluna !=  col       )   continue;
      if ( rel_ver   != - 1  &&  PI->re_vver   !=  rel_ver   )   continue;
      if ( rel_hor   != - 1  &&  PI->re_vhor   !=  rel_hor   )   continue;
      if ( num_campo != - 1  &&  PI->re_num    !=  num_campo )   continue;

      return ( I );
   }

	return ( -1 );
}


F__LOC   VOID  rins_linha( win )
WIN   *win;
{
   SS     I, Y;
   RTela  *PI;

   alterou = 1;

   Y = win->w_height - 1;

   while ( (I = rexiste_item( tela, -1, Y, -1, -1, -1, -1 ))  !=  -1 )
   {
      rdeleta_item( I );
   }

   for ( Y = win->w_height - 2;  Y >= win->w_cy;  Y-- )
   {
      while ( (I = rexiste_item( tela, -1, Y, -1, -1, -1, -1 ))  !=  -1 )
      {
         PI = vet_erel[ I ];

         PI->re_linha++;

         if ( PI->re_tipo == CAMPO_E  ||  PI->re_tipo == CAMPO_S )
         {
            if( PI->re_vver > win->w_height - PI->re_linha )
            {
               PI->re_vver--;
            }
         }

         else if ( PI->re_tipo == BORDA )
         {
            if( ++PI->re_vver >= win->w_height )
            {
               PI->re_vver--;
            }
         }
      }

      if ( Y == 0 ) break;
   }
}


F__LOC   VOID  rdel_linha( win )
WIN   *win;
{
   SS     I, Y;
   RTela  *PI;

   alterou = 1;

   Y = win->w_cy;

   while ( (I = rexiste_item( tela, -1, Y, -1, -1, -1, -1 ))  !=  -1 )
   {
      rdeleta_item( I );
   }

   for ( Y = win->w_cy + 1;  Y < win->w_height;  Y++ )
   {
      while ( (I = rexiste_item( tela, -1, Y, -1, -1, -1, -1 ))  !=  -1 )
      {
         PI = vet_erel[ I ];

         PI->re_linha--;

         if ( PI->re_tipo == BORDA )
         {
            if( PI->re_vver > 1 )
            {
               PI->re_vver--;
            }
         }
      }
   }
}



F__LOC   SS rcentra_borda( E )
d_entry   *E;
{
   SC     dest[ MAX_EDIT + 1 ];

   if ( E->tecla == K_ESC  ||
        !centraliza( dest, E->str, strlen( E->mascara ) ) )
      return( 0 );

   strcpy( E->str, dest );

   return( 1 );
}


F__LOC   SS rmove_cursor( win, ch )
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
                  if ( win->w_cy )   fwat( win->w_cx, --win->w_cy, win );
                  break;

   case K_DN   :
                  if ( ++win->w_cy  >=  win->w_height )  win->w_cy--;
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


F__LOC   VOID   rfaz_borda( win )
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

      if ( rmove_cursor( win, ch ) )   continue;

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

      E.critica = rcentra_borda;

      entrada( &E, 0 );

      win->w_optatr = win->w_boxatr;

      fwat( x3 + 1, y3 + 1, win );

      rinsere_item( BORDA, y3, x3, y4, x4, E.str, -1 );

      rlista_rel( win, tela, CRIACAO , NULL);
   }

   else
   {
      fwat( x1, y1, win );
   }

sai:

   fecha_mensagem( JM );
}


F__LOC   VOID   rfaz_linha( win )
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

      if ( rmove_cursor( win, ch ) )   continue;

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
         rinsere_item( LIN_VER, y3, x3, y4, x4, NULL, -1 );
      }

      else
      {
         linha_hor( y3, x3, x4, win );
         rinsere_item( LIN_HOR, y3, x3, y4, x4, NULL, -1 );
      }

      rlista_rel( win, tela, CRIACAO , NULL);
   }

   fwat( x1, y1, win );

sai:

   fecha_mensagem( JM );
}


F__LOC   SS rconflito_item ( x1, y1, item, tipo )
SS       x1, y1, item;
SS       tipo;
{
	SS     I;
   RTela  *PI;

   for ( I = ( item <= 0 ) ? 0 : item; (PI = pos_itrel(I)) != NULL;   I++ )
   {
      if ( PI->re_tela  !=  tela )   continue;

      if ( tipo  !=  -1 )
      {
         if ( tipo  ==  CAMPO_G )
         {
            if( PI->re_tipo != CAMPO_E && PI->re_tipo != CAMPO_S ) continue;
         }

         else  if ( tipo  !=  PI->re_tipo )  continue;
      }

      switch ( PI->re_tipo )
      {
      case BORDA :
                  if ( x1 == PI->re_coluna  ||  x1 == PI->re_vhor )
                  {
                     if ( y1 >= PI->re_linha  &&  y1 <= PI->re_vver )
                     {
                        goto   sai;
                     }
                  }

                  if ( y1 == PI->re_linha  ||  y1 == PI->re_vver )
                  {
                     if ( x1 >= PI->re_coluna  &&  x1 <= PI->re_vhor )
                     {
                        goto   sai;
                     }
                  }

                  break;

      case LIN_VER :
                  if ( x1 == PI->re_coluna )
                  {
                     if ( y1 >= PI->re_linha  &&  y1 <= PI->re_vver )
                     {
                        goto   sai;
                     }
                  }

                  break;

      case LIN_HOR :
                  if ( y1 == PI->re_linha )
                  {
                     if ( x1 >= PI->re_coluna  &&  x1 <= PI->re_vhor )
                     {
                        goto   sai;
                     }
                  }

                  break;

      case CAMPO_E :
      case CAMPO_S :
                  if ( x1 >= PI->re_coluna       &&
                       x1 < PI->re_coluna + PI->re_vhor )
                  {
                     if ( y1 >= PI->re_linha     &&
                          y1 < PI->re_vver + PI->re_linha )
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


F__LOC   SS rdel_confirma ( item  )
SS     item;
{
   SC     *P1, del[ 130 ], T1[ 30 ];
   RTela  *PI;
   TEXT snaux[3];
   SC     *PT;
   SS     I;

   snaux[0] = OP_SIM[0];
   snaux[1] = OP_NAO[0];
   snaux[2] = '\0';

   PI = vet_erel[ item ];   PT = ( SC * ) PI + sizeof( RTela );

   switch ( PI->re_tipo )
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

               I  = acha_campo( PI->re_icb, PI->re_num);

               if ( I == -1 )   return( 1 );

               P1 = pos_vetcmp(PI->re_icb)[I]->cp_nome;

               sprintf( del, "%s \"%s\" (%c/%c) ? ", M_TR_DELCAM, P1,
                             OP_SIM[0], OP_NAO[0] );
               break;
   }

   if ( men_conf( del, snaux, H_CFITDEL, NULL) == 0 )   return( 1 );

   else                                 return( 0 );
}


F__LOC   VOID  rdeleta_item ( item )
SS    item;
{
   alterou = 1;

   if ( pos_item( ap_rel.rel_vet, item) == NULL )
      numdebug(210);

   del_item(ap_rel.rel_vet, 1);
}



F__LOC   SS   rapaga( win )
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
      if (( item = rconflito_item ( x1, y1, item, -1 )) == - 1 ) break;

      ret++;

      if ( rdel_confirma ( item  ) )
      {
         rdeleta_item ( item );

         rlista_rel( win, tela, CRIACAO, NULL);

         continue;
      }

      item++;
   }

   return( ret );
}



F__LOC   VOID     rfaz_campo( win )
WIN   *win;
{
   TEXT *ajuda_ant = win_ajuda;
   SS    x1, y1;
   Campo *cmp;

   insercol( JI, I_CAMPO );

   x1 = win->w_cx;
   y1 = win->w_cy;

   fwpc( UG_DOT, win );
   fwat( x1, y1, win );

   fwrefresh( win );

   if ( sel_campo(BASE_EDIT, &cmp, 0) != NULL ) {

      SS  linha, coluna;
      SS  vver, vhor, num;
      SC  *pt_texto;

      num    = cmp->cp_num;
      linha  = y1;
      coluna = x1;

      vhor = MIN( cmp->cp_mhor, win->w_len - coluna - 1 );
      pt_texto = str_vazia;
      vver = 1;

      rinsere_item( CAMPO_S, linha, coluna, vver, vhor, pt_texto, num );

      rlista_rel( win, tela, CRIACAO, NULL );
   }

   fwat( x1, y1, win );

   win_ajuda = ajuda_ant;
}


F__LOC   SS   red_linha( win )
WIN   *win;
{
   SC     str[  MAX_EDIT + 1 ];
   SC     masc[ MAX_EDIT + 1 ];
   US     J, *P;
   SS     I, col;
   UC     escape[ 10 ];
   RTela  *PI;
   d_entry   E;
   WIN *JF;

   JF = abre_funcoes( funcao( "1 2 3 E") );

   insercol( JI, I_EDITL );

   for ( I = 0;   I < MAX_EDIT;   I++ )   masc[ I ] = 0;

   /*** Proteje Titulos das Bordas ***/
   for ( I = 0;   (PI = pos_itrel(I)) != NULL;   I++ )
   {
      if ( PI->re_tipo  !=  BORDA )       continue;
      if ( PI->re_tela  !=  tela )        continue;
      if ( PI->re_linha !=  win->w_cy )   continue;

      for ( J = 0;  J < PI->re_vhor - PI->re_coluna;  J++ )
      {
         masc[ PI->re_coluna + J ] = 'P';
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

      if ((I = rexiste_item( tela, NOMES, win->w_cy, -1, -1, -1, -1 )) != -1 )
      {
         rdeleta_item( I );
      }

      if ( col  !=  -1 )
      {
         rinsere_item( NOMES, win->w_cy, col, -1, -1, &str[ col ], -1 );
      }
   }

   fecha_funcoes(  JF );

   return ( E.tecla );
}


F__LOC   VOID   rdisp_lc( W, bot, lin, col )
WIN *W;
SC  *bot;
SS  lin, col;
{
   lin++; col++;

   if ( col > 0 )   sprintf( bot, C_RLLICOL, lin, col );
   else             sprintf( bot, C_RLSOLIN, lin );

   fw_bottom( W, bot );
}

F__LOC   COUNT racha_ordem(tipo, tela, lin, col)
SS tipo, tela, lin, col;
{  SS I, T1, T2;
   RTela *PI;

   switch ( tipo ) {
      case NOMES   : T1 = 1;   break;
      case LIN_VER :
      case LIN_HOR : T1 = 2;   break;
      case BORDA   : T1 = 3;   break;
      case CAMPO_S : T1 = 4;   break;
      case CAMPO_E : T1 = 5;   break;
   }

   for ( I = 0;   (PI = pos_itrel(I)) != NULL;   I++ ) {

      switch ( PI->re_tipo ) {
         case NOMES   : T2 = 1;   break;
         case LIN_VER :
         case LIN_HOR : T2 = 2;   break;
         case BORDA   : T2 = 3;   break;
         case CAMPO_S : T2 = 4;   break;
         case CAMPO_E : T2 = 5;   break;
      }

      if ( PI->re_tela   >  tela      )   break;
      if ( T2            >  T1        )   break;
      if ( PI->re_linha  >  lin       )   break;
      if ( PI->re_coluna >  col       )   break;
   }

   return(I);
}

