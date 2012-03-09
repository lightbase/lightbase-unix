
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <ctype.h>
#include   "window.h"
#include   "entrada.h"
#include   "tabela.h"

#ifdef     MSDOS

SC         *masc_trim( SC *, SC * );
ST( SC * ) form_alfa( d_entry *, SC * );
ST( VOID ) w_leftright( d_entry *, SS );
ST( SS )   procura_end( d_entry *, SS );
ST( SS )   pc_alfa( d_entry *, SS );
ST( SS )   ins_alfa( d_entry *, UC );
VOID       imp_alfa( d_entry *, SS, SS );
ST( VOID ) del_alfa( d_entry *, SS );

#else

SC         *masc_trim( );
ST( SC * ) form_alfa( );
ST( VOID ) w_leftright( );
ST( SS )   procura_end( );
ST( SS )   pc_alfa( );
ST( SS )   ins_alfa( );
VOID       imp_alfa( );
ST( VOID ) del_alfa( );

#endif

US  minuscula( c )

US  c;
{
   if ( (UC) c == ' ' )                      return( c );

   if ( (UC) c >= 'a'  &&  (UC) c <= 'z' )   return( c );

   switch ( (UC) c )
   {
	case UG_GRA_a     :
	case UG_AGU_a     :
	case UG_CIR_a     :
	case UG_TIL_a     :
	case UG_TRE_a     :
	case UG_CED_c     :
	case UG_GRA_e     :
	case UG_AGU_e     :
	case UG_CIR_e     :
	case UG_TRE_e     :
	case UG_GRA_i     :
	case UG_AGU_i     :
	case UG_CIR_i     :
	case UG_TRE_i     :
	case UG_TIL_n     :
	case UG_GRA_o     :
	case UG_AGU_o     :
	case UG_CIR_o     :
	case UG_TIL_o     :
	case UG_TRE_o     :
	case UG_COR_o     :
	case UG_GRA_u     :
	case UG_AGU_u     :
	case UG_CIR_u     :
	case UG_TRE_u     :  return( c );

   default           :  return( 0 );
   }
}

US  maiuscula( c )

US  c;
{
   if ( (UC) c == ' ' )                      return( c );

   if ( (UC) c >= 'A'  &&  (UC) c <= 'Z' )   return( c );

   switch ( (UC) c )
   {
	case UG_GRA_A     :
	case UG_AGU_A     :
	case UG_CIR_A     :
	case UG_TIL_A     :
	case UG_TRE_A     :
	case UG_CED_C     :
	case UG_GRA_E     :
	case UG_AGU_E     :
	case UG_CIR_E     :
	case UG_TRE_E     :
	case UG_GRA_I     :
	case UG_AGU_I     :
	case UG_CIR_I     :
	case UG_TRE_I     :
	case UG_TIL_N     :
	case UG_GRA_O     :
	case UG_AGU_O     :
	case UG_CIR_O     :
	case UG_TIL_O     :
	case UG_TRE_O     :
	case UG_COR_O     :
	case UG_GRA_U     :
	case UG_AGU_U     :
	case UG_CIR_U     :
	case UG_TRE_U     :  return( c );

   default           :  return( 0 );
   }
}

US  para_maiuscula( c )

US  c;
{
   if ( ! minuscula( c ) )   return( c );

   if ( (UC) c >= 'a'  &&  (UC) c <= 'z' )   return( c &  (US) ~32 );

   switch ( (UC) c )
   {
	case UG_GRA_a   :   c = UG_GRA_A;   return( c );
	case UG_AGU_a   :   c = UG_AGU_A;   return( c );
	case UG_CIR_a   :   c = UG_CIR_A;   return( c );
	case UG_TIL_a   :   c = UG_TIL_A;   return( c );
	case UG_TRE_a   :   c = UG_TRE_A;   return( c );
	case UG_CED_c   :   c = UG_CED_C;   return( c );
	case UG_GRA_e   :   c = UG_GRA_E;   return( c );
	case UG_AGU_e   :   c = UG_AGU_E;   return( c );
	case UG_CIR_e   :   c = UG_CIR_E;   return( c );
	case UG_TRE_e   :   c = UG_TRE_E;   return( c );
	case UG_GRA_i   :   c = UG_GRA_I;   return( c );
	case UG_AGU_i   :   c = UG_AGU_I;   return( c );
	case UG_CIR_i   :   c = UG_CIR_I;   return( c );
	case UG_TRE_i   :   c = UG_TRE_I;   return( c );
	case UG_TIL_n   :   c = UG_TIL_N;   return( c );
	case UG_GRA_o   :   c = UG_GRA_O;   return( c );
	case UG_AGU_o   :   c = UG_AGU_O;   return( c );
	case UG_CIR_o   :   c = UG_CIR_O;   return( c );
	case UG_TIL_o   :   c = UG_TIL_O;   return( c );
	case UG_TRE_o   :   c = UG_TRE_O;   return( c );
	case UG_COR_o   :   c = UG_COR_O;   return( c );
	case UG_GRA_u   :   c = UG_GRA_U;   return( c );
	case UG_AGU_u   :   c = UG_AGU_U;   return( c );
	case UG_CIR_u   :   c = UG_CIR_U;   return( c );
	case UG_TRE_u   :   c = UG_TRE_U;   return( c );

   default         :                   return( c );
   }
}


SS   ent_alfa( e, cursor )

d_entry    *e;
SS         cursor;
{
   SS     I, coluna, linha, j_aberta;
   SS     tipo_cursor, prep, crit, m_erro = 0, m_aberta = 0;
   UC     ch, atrib, s1[ MAX_EDIT + 1 ], tecla_1 = 0;
   WIN    *janela;

   e->mudou = 0;

   /*** Se ( prepara )() voltar = 0 Edita ***/
   /*** Se ( prepara )() voltar < 0 Retorna 0             NAO Edita ***/
   /*** Se ( prepara )() voltar > 0 Imprime e Retorna 0 - NAO Edita ***/

   prep = ( *e->prepara )( e );

   if ( prep < 0 )   return( 0 );

   e->tam_m = strlen( e->mascara );   e->tam_m = MIN( e->tam_m, MAX_EDIT );

   if ( e->tam_m == 0 )   return( 0 );

   e->str[     e->tam_m ] = '\0';
   e->mascara[ e->tam_m ] = '\0';

   strcpy( (SC *) s1, (SC *) e->str );

   I = strlen( e->str );

   if ( I < e->tam_m )  for (  ;  I < e->tam_m;  I++ )   e->str[ I ] = ' ';

   j_aberta = 0;

   janela = e->win;

   atrib       = janela->w_atrib;
   tipo_cursor = janela->w_curstyle;

   if ( e->visivel > janela->w_len - e->col )
   {
      e->visivel = janela->w_len - e->col;
   }

   if ( prep > 0 || (e->tipo & E_MOSTRA) )
   {
      imp_alfa( e, e->n_atr, e->visivel );
      if ( e->refresh )   fwrefresh( e->win );
      ch = e->tecla = K_END;
      goto  sai;
   }

   fwat( e->col, e->lin, janela );

   if( e->visivel < e->tam_m )
   {
      e->win = win_make( e->tam_m, 1, e->visivel, 1,
                         e->col + janela->w_x - janela->w_ax,
                         e->lin + janela->w_y - janela->w_ay );

      if ( e->win == NULL )  { e->win = janela;  return( -1 ); }

      if ( e->win->w_flags & W_BOXED ) fwunbox( e->win  );

      e->win->w_flags = W_NOSCROL + W_VISIB;

      e->win->w_intr  = 252;

      if ( janela->w_flags & W_HIGH )  e->win->w_flags |= W_HIGH;

      e->win->w_atrib  = janela->w_atrib;
      e->win->w_optatr = janela->w_optatr;
      e->win->w_defatr = janela->w_defatr;
      e->win->campoatr = janela->campoatr;

/***      fwcls( e->win );   ***/

      coluna = e->col;
      linha  = e->lin;

      e->col = 0;
      e->lin = 0;

      j_aberta++;
   }

   if ( e->men == NULL && e->mensagem != NULL && *e->mensagem != '\0' )
   {
      e->men = abre_mensagem( e->mensagem );

      if ( e->men != NULL )  m_aberta++;
   }

   else {

      /*** mens_[123](  ) trata win == NULL e str == NULL ***/
      mens_1( e->men, e->mensagem );
   }

   e->cursor = cursor;

critica:

   while ( 1 )
   {
      ( *e->crit_char )( e );

      if ( tecla_1  ==  0 )   tecla_1 = 1;
      else                    tecla_1 = 2;

      imp_alfa( e, e->e_atr, e->tam_m );

      ch = e->tecla = fwqgc( e->win );


      if ( m_erro && (ch < 32 || (ch >= K_INTR && ch <= K_ALT_F9) ) )
      {
         mens_1( e->men, e->mensagem );
         m_erro = 0;
      }

      if ( strchr( (SC *) e->escape, ch )  !=  NULL )   goto fim;

      switch ( ch )
      {
      default    :
                     break;

      case 21    :   /*** CTRL U -> Limpa Tudo Campo ***/
                     e->cursor = 0;
      case 25    :   /*** CTRL Y -> Limpa ate o fim ***/
                     e->str[ e->cursor ] = '\0';
 			            continue;

      case K_RT  :
                     e->cursor++;
 			            continue;

      case K_LFT  :
                     if ( e->cursor )  e->cursor--;
 			            continue;

      case K_HOME :
                     e->cursor = 0;
 			            continue;

      case K_END  :
                     {
                        SS   t1, t2, t3;

                        t1 = pc_alfa( e, procura_end( e, 0 ) );
                        t2 = pc_alfa( e, procura_end( e, 1 ) );
                        t3 = pc_alfa( e, e->tam_m - 1 );

                        if (      e->cursor == t3 )  {  e->cursor = t1;  }
                        else if ( e->cursor == t2 )  {  e->cursor = t3;  }
                        else if ( e->cursor == t1 )  {  e->cursor = t2;  }
                        else                         {  e->cursor = t1;  }
                     }

 			            continue;

      case K_INS  :
                     insercao  =  !insercao;
 			            continue;

      case K_WORDRIGHT :
                     w_leftright( e, 1 );
                     continue;

      case K_WORDLEFT :
                     w_leftright( e, 0 );
                     continue;

      case K_UP   :
      case K_DN   :
      case K_TAB  :
      case K_BTAB :
      case K_PGDN :
      case K_PGUP :
      case K_CPGDN :
      case K_CPGUP :
                     goto  fim;

      case K_BS   :
                     if ( e->cursor )
                     {
                        SS  i;
                        i = e->cursor--;
                        e->cursor = pc_alfa ( e, e->cursor );
                        if ( i > e->cursor )  del_alfa( e, e->cursor );
                     }
 			            continue;

      case K_DEL  :
                     del_alfa( e, e->cursor );
 			            continue;

      case K_ESC   :
                     strcpy( ( SC * ) e->str, ( SC * ) s1 );
                     goto  fim;

      case K_ENTER :
				         ( *e->crit_char )( e );
                     goto  fim;
      }

      if ( ! e_print( ch ) )
      {
         win_bell( BEL_INVALID );
         continue;
      }

      if ( (e->tipo & E_LIMPA)  &&  tecla_1  ==  1 )
      {
         e->cursor = 0;
         e->str[ 0 ] = '\0';
         w_ungc( ch );
         continue;
      }

      switch ( e->mascara[ e->cursor ] )
      {
      case 'a' :
      case 'A' :
                  if ( minuscula( ch )  ||  maiuscula( ch ) )
                  {
                     if ( ins_alfa( e, ch ) )   goto  fim;
                     break;
                  }

                  if ( e->cursor && ch == e->mascara[e->cursor - 1] )  break;

                  win_bell( BEL_INVALID );
                  if ( !m_erro ){ mens_1( e->men, e->masc_alfa ); m_erro++; }
                  continue;

      case '*' :
      case 'x' :
      case 'X' :
                  if ( ins_alfa( e, ch ) )   goto  fim;
                  break;

      case '9' :
                  if ( ch == ' ' || ( ch >= '0' && ch <= '9' ) )
                  {
                     if ( ins_alfa( e, ch ) )   goto  fim;
                     break;
                  }

                  if ( e->cursor && ch == e->mascara[e->cursor - 1] )  break;

                  win_bell( BEL_INVALID );
                  if ( !m_erro ){ mens_1( e->men, e->masc_num ); m_erro++; }
                  continue;
      }

      if ( m_erro )  {  mens_1( e->men, e->mensagem );  m_erro = 0;  }

   }

fim:

   imp_alfa( e, e->n_atr, e->tam_m );

   if ( e->refresh )   fwrefresh( e->win );

   /*** e->str[ procura_end( e, 0 ) ] = '\0'; ***/

   masc_trim( e->str, e->mascara );

   if ( ch  !=  K_ESC )   e->mudou = strcmp( e->str, ( SC * ) s1 );

   crit = ( *e->critica )( e );

   if ( crit > 0 )
   {
      imp_alfa( e, e->n_atr, e->tam_m );

      if ( e->refresh )   fwrefresh( e->win );
   }

   else if ( crit < 0 )   goto  critica;

   if ( m_aberta )  {  fecha_mensagem( e->men );   e->men = NULL;  }

   if ( j_aberta )
   {
      fwkill( e->win );

      e->col = coluna;
      e->lin = linha;
      e->win = janela;

      imp_alfa( e, e->n_atr, e->visivel );   fwrefresh( e->win );
   }

sai:

   /*** e->str[ procura_end( e, 0 ) ] = '\0'; ***/

   masc_trim( e->str, e->mascara );

   janela->w_curstyle = tipo_cursor;

   janela->w_atrib = atrib;

   if ( prep > 0 )  ch = 0;

   return( ( SS ) ch );
}


SC *masc_trim( dest, masc )

SC *dest, *masc;
{
   SC  *D, *M, *Z;

   for ( Z = D = dest, M = masc;   *D != '\0';   D++, M++ )
   {
      if ( *D == ' ' )   continue;

      if ( *D == *M  &&  strchr( "*9axAX", *M ) == NULL )  continue;

      Z = D + 1;
   }

   *Z = '\0';

   return ( dest );
}


ST( SS ) procura_end( e, flag_zero )

d_entry    *e;
SS         flag_zero;
{
   SS      final, ret;

   final = e->tam_m - 1;

   while ( 1 )
   {
      switch ( e->mascara[ final ] )
      {
         case '*' :
         case 'a' :
         case 'A' :
         case 'x' :
         case 'X' :
         case '9' :
                     if ( e->str[ final ] != ' ' )     goto retorna;

                     else                              final--;
                     break;

         default  :
                     if ( flag_zero  ||  final <= 0 )  goto  retorna;

                     else                              final--;
                     break;
      }
   }

retorna:

   ret = final + 1;

   return ( ret );
}


ST( VOID )  w_leftright( e, right )

d_entry    *e;
SS         right;
{
   SS      antigo;

   antigo = e->cursor;

   if ( right )
   {
      while( e->str[ e->cursor ] != ' ' )
      {
         if ( ++e->cursor >= e->tam_m ) { e->cursor = antigo;  return; }
      }

      while( e->str[ e->cursor ] == ' ' )
      {
         if ( ++e->cursor >= e->tam_m ) { e->cursor = antigo;  return; }
      }
   }

   else
   {
      if ( e->cursor--  <=  0 ) { e->cursor = 0;  return;  }

      while( strchr( "*9axAX", e->mascara[ e->cursor ]) == NULL )
      {
         if ( e->cursor--  <=  0 ) { e->cursor = 0;  return; }
      }

      while( e->str[ e->cursor ] == ' ' )
      {
         if ( e->cursor--  <=  0 ) { e->cursor = 0;  return; }
      }

      while( e->str[ e->cursor ] != ' ' )
      {
         if ( e->cursor--  <=  0 ) { e->cursor = 0;  return;  }
      }

      ++e->cursor;
   }
}


ST( SS ) pc_alfa( e, pos )

d_entry    *e;
SS         pos;
{
   SS      ok, antigo, dir;

   dir = e->tecla;

   e->win->w_curstyle = ( insercao ) ? CS_INTER : CS_NORMAL;

   antigo = pos = COTA( 0, pos, e->tam_m - 1 );

   ok = 0;

   do {

      switch ( e->mascara[ pos ] )
      {
         case '*' :
         case 'a' :
         case 'A' :
         case 'x' :
         case 'X' :
         case '9' :
                     ok = 2;
                     break;

         default  :
                     switch ( dir )
                     {
                     case K_BS   :
                     case K_LFT  :
                                    if ( pos <= 0 ) {
                                       dir = K_RT;
                                       pos = 0;
                                    }

                                    else  pos--;

                                    break;

                     default     :
                                    if ( pos >= e->tam_m )
                                    {
                                       if ( ok++ == 0 ) {
                                          dir = K_LFT;
                                          pos = e->tam_m;
                                       }
                                       else   pos = antigo;
                                    }

                                    else  pos++;

                                    break;
                     }

                     break;
      }

   }  while ( ok != 2);

   return ( pos );
}


ST( VOID ) del_alfa( e, pos )

d_entry      *e;
SS         pos;
{
   SC   *m1, *m2, formato;
   SC   *p1, *p2;

   if ( pos >= e->tam_m )  return;

   e->cursor = pos;

   formato = 0;
   p1 = &e->str[     e->cursor ];
   m1 = &e->mascara[ e->cursor ];

   for (  ;  *p1 != '\0';  p1++, m1++ )
   {
      if ( strchr( "*9axAX",  *m1 ) == NULL )  continue;

      if ( formato == 0 )   formato = *m1;

      for ( p2 = p1 + 1, m2 = m1 + 1;  *p2 != '\0';  p2++, m2++ )
      {
         if ( strchr( "*9axAX",  *m2 ) != NULL )  break;
      }

      if ( *p2 == '\0' || *m2 != formato )    goto  sai;

      else {

         *p1 = *p2;
      }
   }

sai :

   *p1 = ' ';
}


ST( SS ) ins_alfa( e, c1 )

d_entry      *e;
UC         c1;
{
   US    teste;
   SC    *m1, formato;
   SC    *p1, c2;

/***
   teste = e->cursor;
   e->cursor = pc_alfa( e, ++e->cursor );
   if ( teste == e->cursor && e->str[ teste ] != ' ' && e->tam_m > 1 )
   {
      win_bell( BEL_INVALID );
      if ( insercao )  return ( 0 );
   }
   e->cursor = teste;
***/

   if ( !insercao  ||  e->tam_m == 1 )
   {
      e->str[ e->cursor ] = c1;
   }

   else  {

      formato = 0;
      p1 = &e->str[     e->cursor ];
      m1 = &e->mascara[ e->cursor ];

      for (  ;  *p1 != '\0';  p1++, m1++ )
      {
         if ( strchr( "*9axAX",  *m1 ) == NULL )  continue;

         if ( formato == 0 )   formato = *m1;

         if ( *m1 != formato )    goto  sai;

         c2  = *p1;
         *p1 = c1;
         c1  = c2;
      }
sai:
      if ( *m1 == formato && e->tam_m > p1 - e->str )
      {
         *p1++ = c1;
      }
   }

   teste = ++e->cursor;

   if ( teste > (e->cursor = pc_alfa( e, e->cursor ) ) )
   {
      if ( !( e->tipo & E_CONFIRMA ) )     return ( 1 );
   }

   return ( 0 );
}


ST( SC * ) form_alfa( E, dest )

d_entry  *E;
SC       *dest;
{
   SC     *m, *s, *t;
   SS     fim;

   for( fim = E->tam_m, s = E->str + fim - 1;   s >= E->str;   fim--, s-- )
   {
      if ( *s != ' ' )     break;
   }

   for ( t = dest, m = E->mascara, s = E->str;  *s != '\0';  m++, s++, t++ )
   {
      switch ( *m )
      {
      case '*' :
                  if ( s - E->str  <  fim )      *t = '*';
                  else                           *t = ' ';
                  continue;
      case 'A' :
      case 'X' :
                  if ( ! minuscula( *s ) )     *t = *s;
                  else  *t = *s = para_maiuscula( *s );
                  continue;
      case 'a' :
      case 'x' :
      case '9' :
      case 'P' :
                  *t = *s;
                  continue;
      default  :
                  *t = *s = *m;
                  continue;
      }
   }

   for (   ;   *m != '\0';   m++, s++, t++ )
   {
      switch ( *m )
      {
      case 'a' :
      case 'x' :
      case '9' :
      case 'A' :
      case 'X' :
      case '*' :
                  *t = *s = ' ';
                  continue;
      case 'P' :
                  *t = *s;
                  continue;
      default  :
                  *t = *s = *m;
                  continue;
      }
   }

   *t = *s = '\0';

   return ( dest );
}


VOID imp_alfa( e, atr, quant )

d_entry     *e;
SS          atr, quant;
{
   SC     tmp[ MAX_EDIT + 1 ];
   US     ax, ay;

   form_alfa( e, tmp );

   e->win->w_atrib = atr;

   ax = e->win->w_ax;
   ay = e->win->w_ay;

   e->win->w_cx = e->col;
   e->win->w_cy = e->lin;

   fwpsl( (UC *) tmp, e->win, MIN( e->tam_m, quant ) );

   e->cursor = pc_alfa( e, e->cursor );

   e->win->w_ax = ax;
   e->win->w_ay = ay;

   fwat( e->cursor + e->col, e->lin, e->win );
}


