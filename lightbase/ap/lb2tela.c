#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2vet.h"
#include  "lb2prot1.h"


#include  "lb2vista.h"
/* #include  "lb_tela.h"  */

extern COUNT psq_qtdfonetica;
extern COUNT psq_brqtd;

static WIN   *tmp_win;
static SC   **tmp_palavras, tmp_linha[ MAX_EDIT + 1 ];
static SS     X_COL1, Y_LIN1, X_COL2, Y_LIN2, tmp_atrib;
SI     bq3_comp( );


#ifdef      MSDOS
   SS       brilha_mesmo( S_INDEX  *, SC  *);
   VOID     ct_enfat( COUNT, WIN *, TTela *, SC **, SS );
   VOID     cn_enfat( COUNT, WIN *, TTela *, SC **, SS );
   WIN     *aloc_win(TTela *);
   TEXT  *fonema(TEXT*);
#else
   SS       brilha_mesmo( );
   VOID     ct_enfat( );
   VOID     cn_enfat( );
   WIN     *aloc_win( );
   TEXT  *fonema( );
#endif


F__GLB   VOID  lista_vista( win, sele, num_tela, flag , rodape)
WIN       *win;
SS        sele;
SS        num_tela;
SS        flag;
SC       *rodape;
{
   US     X, Y;
	SS     I, J, K, n_itens;
	SS     q_tela;
   UC     ug_dot1[ MAX_EDIT + 1 ];
   UC     ug_dot2[ MAX_EDIT + 1 ];
   TTela  *PI, **vtevis;
   SC     *PT;
   SC     tit_texto[ MAX_EDIT + 1 ];
   static SC     titulo[ 80 ];
   Campo  **vetcp;

   X = win->w_cx;
   Y = win->w_cy;

   vetcp    = pos_vetcmp(sele);
   vtevis   = (TTela **) ap_vista.vis_vet->vi_vtit;
   n_itens  = ap_vista.vis_vet->vi_nelem;

   for ( I = 0;   I < MAX_EDIT;   I++  )
   {
      ug_dot1[ I ] = (flag == CRIACAO) ? UG_DOT  : ' ';
      ug_dot2[ I ] = (flag == CRIACAO) ? UG_DOT2 : ' ';
   }

   for ( I = 0, q_tela = 1;   I < n_itens;   I++ )
   {
      PI = vtevis[ I ];

      if ( PI->te_tela  >  q_tela )   q_tela = PI->te_tela;
   }

   q_tela = MAX( q_tela, num_tela );

   sprintf( titulo, C_TT_VISTA, ap_vista.vis_ident, num_tela, q_tela );

   fw_title( win, titulo, win->t_atr );

   if ( rodape != NULL )
      fw_bottom( win, rodape );

   fwcls( win );

   for ( I = 0;   I < n_itens;   I++ )
   {
      PI = vtevis[ I ];   PT = ( SC * ) PI + sizeof( TTela );

      if ( PI->te_tela  !=  num_tela )  {
         if ( flag == UTILIZACAO && PI->te_win != NULL ) {
            fwkill(PI->te_win);
            PI->te_win = NULL;
         }
         continue;
      }


      switch ( PI->te_tipo )
      {
      case BORDA :
                  fwframe( PI->te_linha, PI->te_coluna,
                           PI->te_vver, PI->te_vhor, 1, win );

                  fwat( PI->te_coluna + 1, PI->te_linha, win );

                  fwpsl( ( UC * ) PT, win, win->w_len );

                  break;

      case LIN_VER :
                  linha_ver( PI->te_coluna, PI->te_linha, PI->te_vver, win );
                  break;

      case LIN_HOR :
                  linha_hor( PI->te_linha, PI->te_coluna, PI->te_vhor, win );
                  break;

      case NOMES :
                  fwat(  PI->te_coluna, PI->te_linha, win );
                  fwpsl( ( UC * ) PT, win, win->w_len );
                  break;

      case CAMPO_E :
                  for ( J = 0;   J < PI->te_vver;   J++ )
                  {
                     fwat( PI->te_coluna, PI->te_linha + J, win );
                     fwpsl( ug_dot1, win, PI->te_vhor );
                  }

                  K = acha_campo( PI->te_icb, PI->te_num);

                  if ( K != - 1  &&  vetcp[ K ]->cp_tipo == CP_TEXTO )
                  {
                     /* WIN_GERAL desativado */
                     if ( flag == UTILIZACAO && PI->te_win == NULL ) {
                        PI->te_win = aloc_win(PI);
                        if ( PI->te_win == NULL ) {
                           numdebug(14);
                        }
                     }

                     fwframe( PI->te_linha - 1, PI->te_coluna - 1,
                              PI->te_linha + PI->te_vver,
                              PI->te_coluna + PI->te_vhor, 1, win );

                     if ( centraliza( tit_texto, PT, PI->te_vhor ) )
                     {
                        fwat( PI->te_coluna, PI->te_linha - 1, win );

                        fwpsl( ( UC * ) tit_texto, win, win->w_len );
                     }
                  }

                  break;

      case CAMPO_S :
                  for ( J = 0;   J < PI->te_vver;   J++ )
                  {
                     fwat( PI->te_coluna, PI->te_linha + J, win );
                     fwpsl( ug_dot2, win, PI->te_vhor );
                  }

                  K = acha_campo( PI->te_icb, PI->te_num);

                  if ( K != - 1  &&  vetcp[ K ]->cp_tipo == CP_TEXTO )
                  {
                     /* WIN_GERAL desativado */
                     if ( flag == UTILIZACAO && PI->te_win == NULL ) {
                        PI->te_win = aloc_win(PI);
                        if ( PI->te_win == NULL ) {
                           numdebug(14);
                        }
                     }

                     fwframe( PI->te_linha - 1, PI->te_coluna - 1,
                              PI->te_linha + PI->te_vver,
                              PI->te_coluna + PI->te_vhor, 1, win );

                     if ( centraliza( tit_texto, PT, PI->te_vhor ) )
                     {
                        fwat( PI->te_coluna, PI->te_linha - 1, win );

                        fwpsl( ( UC * ) tit_texto, win, win->w_len );
                     }
                  }

                  break;
      }
   }

   fwat( X, Y, win );
}



F__GLB   VOID brilha_pal( sele, num_tela, win, palavras, atributo )
SS        sele;
SS        num_tela;
WIN      *win;
SC      **palavras;
SS        atributo;
{
	SS     I, K, n_itens;
   TTela  *PI, **vtevis;
   Campo  **vetcp;

#ifdef   NAO_BRILHA

   return;

#else

   vetcp    = pos_vetcmp(sele);
   vtevis   = (TTela **) ap_vista.vis_vet->vi_vtit;
   n_itens  = ap_vista.vis_vet->vi_nelem;

   if ( psq_brqtd == 0 )
      return;

   for ( I = 0;   I < n_itens;   I++ )
   {
      PI = vtevis[ I ];

      if ( PI->te_tela  !=  num_tela )   continue;

      switch ( PI->te_tipo ) {
      case CAMPO_S :
      case CAMPO_E :
                     if ( (K = acha_campo( PI->te_icb, PI->te_num)) != -1 )
                        if ( vetcp[ K ]->cp_tipo == CP_TEXTO )
                           ct_enfat( sele, win, PI, palavras, atributo );
                        else
                           cn_enfat( sele, win, PI, palavras, atributo );

                     break;
      }
   }

   fwrefresh( win );
#endif
}







F__GLB   SS centraliza( dest, orig, tam )
SC   *dest, *orig;
SS   tam;
{
   SS     I, J, K, L, ret;
   SC     *P, tmp[ MAX_EDIT + 1 ];

   strcpy( tmp, orig );

   if ( tam == 1 ) tmp[0] = '\0';   /* colocado para truncar titulo */
   else            tmp[tam] = '\0';

   K = strlen( tmp );

   for ( I = 0, J = K - 1;   I <= J;   I++ )
   {
      if ( tmp[ I ] != ' '  &&  ( UC ) tmp[ I ] != UG_HOR )   break;
   }

   for (   ;   J >= 0;   J-- )
   {
      if ( tmp[ J ] != ' '  &&  ( UC ) tmp[ J ] != UG_HOR )   break;
   }

   if ( J < I )    {  ret = 0;   goto  sai;  }

   L = J - I + 1;

   strncpy( tmp, tmp + I, L );    tmp[ L ] = '\0';

   I = tam - L;   J = I % 2;   I = I / 2;   J += I;

   for( P = dest;   I > 0;   P++, I-- )   *P = UG_HOR;

   for( I = 0;      I < L;   P++, I++ )   *P = tmp[ I ];

   for(  ;          J > 0;   P++, J-- )   *P = UG_HOR;

   *P = '\0';

   ret = 1;

sai:

   return ( ret );
}




F__GLB   VOID    linha_hor( y3, x3, x4, win )
SS    y3, x3, x4;
WIN   *win;
{
	SS    ch;

   fwat( x3, y3, win );

   for ( ch = x3;  ch <= x4;  ch++ )  fwpc( UG_HOR, win );
}


F__GLB   VOID    linha_ver( x3, y3, y4, win )
SS    x3, y3, y4;
WIN   *win;
{
	SS    ch;

   fwat( x3, y3, win );

   for ( ch = y3;  ch <= y4;  ch++ )
   {
      fwat( x3, ch, win );

      fwpc( UG_VER, win );
   }
}




F__LOC   SS brilha_mesmo( si, str )
S_INDEX  *si;
SC       *str;
{  SC   aux[LB_KEYMAX + 1], *p;
   SC   *P;
   SS   brilha;

#ifdef   NAO_BRILHA

   return 0;

#else

   brilha = 0;

   P = si->palavra;

   if ( bsearch( (SC *) &P, (SC *) tmp_palavras,
                         psq_brqtd, sizeof(SC *), bq3_comp ) !=  NULL ) {
      brilha = 1;
   }
   else if ( psq_qtdfonetica ) {
      p = aux;
      /* o token 0x2 0x2 vai indicar que o que tem indexado em TREE_HORAS e'fonetico */
      *p     = 2;    /* Indicador de palavra FONETICA */
      *(p+1) = 2;

      strcpy( p+2,  fonema(si->palavra));

      if ( strlen(p) + 1 > 3) {
         if ( bsearch( (SC *) &p, (SC *) tmp_palavras, psq_qtdfonetica,
                       sizeof(SC *), bq3_comp ) !=  NULL ) {
            brilha = 1;
         }
      }
   }

   if ( brilha ) {
      fwat( X_COL1 + (str - tmp_linha), Y_LIN1, tmp_win );
      fwsetatr( tmp_atrib, strlen( si->palavra ), tmp_win );
   }

   return ( 0 );
#endif
}




F__LOC   VOID ct_enfat( sele, win, PI, palavras, atributo )
SS       sele;
WIN     *win;
TTela   *PI;
SC     **palavras;
SS       atributo;
{
   US       *P;
   SS        I, J, X, Y, AX, AY;
   S_INDEX   si;

   if ( PI == NULL )
   	tmp_win = win;
   else
      tmp_win = pos_vetcmp(sele)[PI->te_ivetcp]->cp_adoc->je;

   tmp_atrib    = atributo;
   tmp_palavras = palavras;

   X  = tmp_win->w_cx;
   Y  = tmp_win->w_cy;
   AX = tmp_win->w_ax;
   AY = tmp_win->w_ay;

   X_COL1 = 0;
   X_COL2 = tmp_win->w_len;
   Y_LIN2 = tmp_win->w_height;

   for( Y_LIN1 = 0;   Y_LIN1 < tmp_win->w_height;  Y_LIN1++ )
   {
      P = tmp_win->w_ary + (Y_LIN1 * tmp_win->w_len) + X_COL1;

      for ( I = X_COL1, J = 0;  I < X_COL2;  I++, J++, P++ )
      {
         tmp_linha[ J ] = ((UC) *P == UG_SP) ? ' ' : *P;
      }

      tmp_linha[ J ] = '\0';

      sep_palavra(&si, tmp_linha, &ap_itexto, brilha_mesmo);
   }

   tmp_win->w_cx = X;
   tmp_win->w_cy = Y;

   fwamove( AX, AY, tmp_win );

   fwrefresh( tmp_win );
}


F__LOC   VOID cn_enfat( sele, win, PI, palavras, atributo )
SS       sele;
WIN     *win;
TTela   *PI;
SC     **palavras;
SS       atributo;
{
   US       *P;
   SS        I, J;
   S_INDEX   si;

   sele = sele;

   tmp_win      = win;
   tmp_atrib    = atributo;
   tmp_palavras = palavras;

   X_COL1  = PI->te_coluna;
   Y_LIN1  = PI->te_linha;

   X_COL2  = MIN( X_COL1 + PI->te_vhor, win->w_len );
   Y_LIN2  = MIN( Y_LIN1 + PI->te_vver, win->w_height );

   P = win->w_ary + (Y_LIN1 * win->w_len) + X_COL1;

   for ( I = X_COL1, J = 0;  I < X_COL2;  I++, J++, P++ )
   {
      tmp_linha[ J ] = *P;
   }

   tmp_linha[ J ] = '\0';

   sep_palavra( &si, tmp_linha, &ap_icomun, brilha_mesmo );
}



F__LOC   WIN *aloc_win(pe)
TTela *pe;
{  COUNT off_lin, off_col;
   WIN *j;

   /* Ao mudar JAN_UTIL, mudar o off da janela abaixo */
   off_lin = 2;
   off_col = 1;

   j = win_make( pe->te_vhor, pe->te_vver,
                 pe->te_vhor, pe->te_vver,
                 pe->te_coluna + off_col, pe->te_linha + off_lin);

   if ( j != NULL ) {
	   j->w_flags  = W_VISIB | W_NOSCROL;
      j->w_atrib  = JS_JANELA;
      j->w_optatr = JS_BARRA;
      j->w_defatr = JS_DESTAQUE;
      j->w_boxatr = JS_JANELA;
      j->t_atr    = JS_JANELA;
	   j->campoatr = JS_CAMPO;

      fw_title( j, (TEXT *)pe + sizeof(TTela), j->t_atr );
      fwcls(j);
      fwbot(j);
   }


   return(j);
}



SI  bq3_comp( S1,  S2 )
SC    **S1, **S2;
{
   return( strcmp( *S1, *S2 ) );
}

