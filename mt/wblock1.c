
#ifdef comout

WBLOCK.C

fwunbox( win )                         - remove the box around a window
fwsetatr( atr,len,win )                        - change len chars to atribute.
fwframe( li, ci,  lf, cf, tipo, win )  - Faz uma borda dentro de uma Janela
                                         tipo = 1 Simpels  = 2 Duplas
#endif

#include <stdio.h>
#include <window.h>

#ifdef    MSDOS

VOID  remap( SS, SS, SS ); /*** Em WMAKE.C ***/

#else

VOID  remap( ); /*** Em WMAKE.C ***/

#endif

VOID fwunbox( win )  /* removes any boxes from around the window.
                        note: def_win cannnot be unboxed if boxed
                        by win_init()
                     */
struct Window *win; 
{ 
  if( win->w_num >= win_sp ) return;
  if( win == def_win )
       if( win->w_len < size_x ) return; /* see note above */
  win->w_flags &= ~W_BOXED;
  wu_remap( win->w_x - 1, win->w_y - 1,win->w_alen + 2, win->w_aheight + 2 );
}


VOID fwframe( li, ci,  lf, cf, tipo, win )

SS    li;                /* Linha  Inicial */
SS    ci;                /* Coluna Inicial */
SS    lf;                /* Linha  Final   */
SS    cf;                /* Coluna Final   */
SS    tipo;              /* Bordas - 1 SIMPLES, 2 DUPLAS */
struct Window *win;
{
   SS    c, l, x, y;

   x = win->w_cx;  y = win->w_cy;

   tipo = COTA( 1, tipo, 2 );

   for ( c = ci + 1;  c < cf;  c++ )  
   {
      fwat( c, li, win );   fwpc( UG_HOR + ((tipo - 1) * UG_LEN), win );

      fwat( c, lf, win );   fwpc( UG_HOR + ((tipo - 1) * UG_LEN), win );
   }

   for ( l = li + 1;  l < lf;  l++ )  
   {
      fwat( ci, l, win );   fwpc( UG_VER + ((tipo - 1) * UG_LEN), win );

      fwat( cf, l, win );   fwpc( UG_VER + ((tipo - 1) * UG_LEN), win );
   }

   fwat( ci, li, win );   fwpc( UG_UL + ((tipo - 1) * UG_LEN), win );

   fwat( cf, li, win );   fwpc( UG_UR + ((tipo - 1) * UG_LEN), win );

   fwat( ci, lf, win );   fwpc( UG_LL + ((tipo - 1) * UG_LEN), win );

   fwat( cf, lf, win );   fwpc( UG_LR + ((tipo - 1) * UG_LEN), win );

   fwat( x, y, win );
}

VOID fwsetatr( atr,len,win )   /* give len chars the atribute atr, 
                                  ( from cursor position (inclusive),
                                    cursor doesn't move ) */
UC atr;
US len;                      /* present line only */
struct Window *win;
{ register US *p;
  register SS a;

  db1( "fwsetatr" );
  if( win->w_len - win->w_cx < len ) 
       len = win->w_len - win->w_cx; 
  a = len;
  for( p = win->w_ary + ( win->w_cy * win->w_len ) + win->w_cx;a;a--,p++ )
        *p = (atr << 8) + ( *p & 255 ); 
  wu_lmark( win,len );
}

VOID fwtop( win )                     /* makes window the highest priority */
struct Window *win;                     /* ret 0 or bad window */
{ SS a;
  SS x,y,l,h;

db1( "fwtop" );
  /* flip the stack */ 
  stak1( win, "FWTOP" );
  if( win->w_num >= win_sp ) return;
  x = win->w_x; y = win->w_y; l = win->w_alen; h = win->w_aheight;
  if( win->w_flags  & W_BOXED )
      { x--; y--; l += 2; h += 2; }
  for( a = win->w_num;a < win_sp - 1;a++ )
        { 
          win_stak[ a ] = win_stak[ a + 1 ];
          win_stak[ a ]->w_num = a;
        }
  remap( win->w_num + 1,win_sp - 1,-1 );
/*  for( a = win->w_num + 1;a < win_sp;a++ ) remap( a,a - 1 ); */
  win_stak[ win_sp - 1 ] = win;
  win->w_num = win_sp - 1;
  /* and map it in */
  wu_remap( x,y,l,h );
  if( win->w_alen * win->w_aheight > ( size_x * size_y ) * 0.8 )
        win_dominant = win_sp - 1;
  else  win_dominant = -1;
}

VOID fwbot( win )          /* makes window the lowest priority */
struct Window *win;        /* ret 0 or bad window */
{ SS a;
  SS x,y,l,h;

db1( "fwbot" );
  /* flip the stack */ 
  stak1( win, "FWBOP" );
  if( win->w_num >= win_sp ) return;
  x = win->w_x; y = win->w_y; l = win->w_alen; h = win->w_aheight;
  if( win->w_flags  & W_BOXED )
      { x--; y--; l += 2; h += 2; }
  for( a = win->w_num;a;a-- )
        { win_stak[ a ] = win_stak[ a - 1 ];
          win_stak[ a ]->w_num = a;
        }
  remap( 0,win->w_num - 1,1 );
  win_stak[ 0 ] = win;
  win->w_num = 0;
  /* and map it in */
  wu_remap( x,y,l,h );
  if( win_dominant == 0 ) win_dominant = -1;
}


VOID  set_cores( tipo, win )

SS    tipo;
WIN   *win;
{

   if( win == NULL  ||  win_stak[ win->w_num ] != win )   return;

   switch ( tipo )
   {
   case L_MENS : 
            win->w_atrib  = LM_CAMPO;
            win->w_optatr = LM_BARRA;
            win->w_defatr = LM_DESTAQUE;
            win->w_boxatr = LM_CAMPO;
            win->t_atr    = LM_BARRA;
            win->campoatr = LM_CAMPO;
            break;

   case J_PRIN : 
            win->w_atrib  = JP_JANELA;
            win->w_optatr = JP_BARRA;
            win->w_defatr = JP_DESTAQUE;
            win->w_boxatr = JP_JANELA;
            win->t_atr    = JP_BARRA;
            win->campoatr = JP_CAMPO;
            break;

   case J_SEGU : 
            win->w_atrib  = JS_JANELA;
            win->w_optatr = JS_BARRA;
            win->w_defatr = JS_DESTAQUE;
            win->w_boxatr = JS_JANELA;
            win->t_atr    = JS_BARRA;
            win->campoatr = JS_CAMPO;
            break;

   case M_PRIN : 
            win->w_atrib  = MP_JANELA;
            win->w_optatr = MP_BARRA;
            win->w_defatr = MP_DESTAQUE;
            win->w_boxatr = MP_JANELA;
            win->t_atr    = MP_BARRA;
            win->campoatr = MP_CAMPO;
            break;

   case M_SEGU : 
            win->w_atrib  = MS_JANELA;
            win->w_optatr = MS_BARRA;
            win->w_defatr = MS_DESTAQUE;
            win->w_boxatr = MS_JANELA;
            win->t_atr    = MS_BARRA;
            win->campoatr = MS_CAMPO;
            break;

   case J_ERRO : 
            win->w_atrib  = JE_JANELA;
            win->w_optatr = JE_BARRA;
            win->w_defatr = JE_DESTAQUE;
            win->w_boxatr = JE_JANELA;
            win->t_atr    = JE_BARRA;
            win->campoatr = JE_CAMPO;
            break;

   case F_ESPE : 
            win->w_atrib  = FE_JANELA;
            win->w_optatr = FE_BARRA;
            win->w_defatr = FE_DESTAQUE;
            win->w_boxatr = FE_JANELA;
            win->t_atr    = FE_BARRA;
            win->campoatr = FE_CAMPO;
            break;
   }
}

