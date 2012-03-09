
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <ctype.h>
#include   "window.h"
#include   "entrada.h"

#ifdef     MSDOS

ST( SS )   norm_masc_valor( d_entry * );
ST( SC * ) form_valor( d_entry *, SC * );
ST( SS )   pc_valor( d_entry *, SS, SS );
ST( VOID ) ins_valor( d_entry *, UC );
ST( VOID ) imp_valor( d_entry *, SS, SS );
ST( VOID ) del_valor( d_entry * );

#else

ST( SS )   norm_masc_valor( );
ST( SC * ) form_valor( );
ST( SS )   pc_valor( );
ST( VOID ) ins_valor( );
ST( VOID ) imp_valor( );
ST( VOID ) del_valor( );

#endif

DB  str_to_db( str, p_dec )

SC  *str;
SS  p_dec;
{
   SC     C, *pi, *po, dec;
   DB     sinal;
   SC     num_str[ 100 ];

   pi = str;
   po = num_str;
   dec = 0;
   sinal = (DB) 1;

   while( (C = *pi++) != 0  )
   {
      if ( C >= '0'  &&  C <= '9' )        *po++ = C;

      else if ( ! dec  &&  C == p_dec ) {  *po++ = '.';  dec++;    }

      else if ( sinal == (DB) 1  &&  C == '-' ) {  sinal = (DB) -1;  }
   }

   *po = '\0';

   return ( sinal * atof( num_str ) );
}


SC     *db_to_str( num_db, dest, max_dec, sep_milhar )

DB num_db;
SC     *dest;
SS     max_dec, sep_milhar;
/*** Sep_milhar == TRUE  -> 1[sep_int]234[sep_dec]56 ***/
/*** Sep_milhar == FALSE -> 1234.56 -> Formato interno ***/
{
   SC  C, *pi, *po;
   SC  aux[ 35 ];
   SS  num_int, num_dec;
   static SC out[ 35 ];

   if ( dest == NULL )   dest = out;

   sprintf( aux, "%.4f", num_db );

   num_int = 0;
   num_dec = 0;

   pi = aux;

   while ( (C = *pi++) != '\0'  &&  C != '.' )
   {
      if ( C >= '0'  &&  C <= '9' )   num_int++;
   }

   if ( C == '.' ) {  while ( *pi++ != '\0' )  num_dec++;  }

   num_dec = MIN( num_dec, max_dec );

   pi = aux;
   po = dest;

   while( (C = *pi++) != 0  )
   {
      if ( C == '.' )
      {
         if ( num_dec )  *po++ = sep_milhar ? sep_dec : '.';
         else            break;
      }

      else if ( C == '-' )        *po++ = '-';

      else if ( C >= '0' && C <= '9' )
      {
         *po++ = C;

         if ( num_int-- > 0 )
         {
            if ( sep_milhar  &&  num_int % 3 == 0  &&  num_int )
            {
               *po++ = sep_int;
            }
         }

         else if ( --num_dec <= 0 )   break;
      }
   }

   *po = '\0';

   return( dest );
}


SC *valor_importa( dest, orig, masc )

SC   *dest, *orig, *masc;
{
   DB   db;
   SS   I, J, decimais;
   SC   *M;

   J = I = strlen( masc );
   if ( I == 0 )  return( dest );

   M = masc + I - 1;

   for ( decimais = -1;   I > 0  &&  decimais == -1;   I--, M-- )
   {
      if ( *M == sep_dec )   decimais = I - 1;
   }

   /*** Se existir decimais, devera' haver pelo menos uma casa ***/
   if ( decimais == -1 || decimais == J - 1 )   decimais = J;

   decimais = J - decimais;

   if ( decimais  >  0 )   decimais--;

   db = str_to_db( orig, sep_dec );

   while ( 1 )
   {
      db_to_str( db, dest, decimais, 1 /* Separa milhar */ );

      if ( strlen( dest )  <=  J )   break;

      db  /=  10;
   }

   return ( ltrim( dest ) );
}


SS   ent_valor( e )

d_entry    *e;
{
   SS     I, coluna, linha, j_aberta;
   SS     tipo_cursor, prep, crit, m_erro = 0, m_aberta = 0;
   UC     ch, atrib, s1[ MAX_EDIT + 1 ], tecla_1 = 0;
   WIN    *janela;

   /*** Se ( prepra )() voltar = 0 Edita ***/
   /*** Se ( prepra )() voltar < 0 Retorna 0             NAO Edita ***/
   /*** Se ( prepra )() voltar > 0 Imprime e Retorna 0 - NAO Edita ***/

   e->mudou = 0;

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

   /*** E->piece sera' usado para informar o ponto decimal em E_VALOR ***/
   e->piece = norm_masc_valor( e );

   atrib       = janela->w_atrib;
   tipo_cursor = janela->w_curstyle;

   if ( prep > 0 || (e->tipo & E_MOSTRA) )
   {
      imp_valor( e, e->n_atr, e->visivel );
      if ( e->refresh )   fwrefresh( e->win );
      ch = e->tecla = K_END;
      goto  sai;
   }

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

   else {  mens_1( e->men, e->mensagem );  }

   e->cursor = e->piece - 1;

   /*** Assim que entrar assuma insercao ate mover o cursor.  ***/
   e->q_lista = 1;

critica:

   while ( 1 )
   {
      ( *e->crit_char )( e );

      if ( tecla_1  ==  0 )   tecla_1 = 1;
      else                    tecla_1 = 2;

      imp_valor( e, e->e_atr, e->tam_m );

      if ( e->cursor != e->piece - 1 )  e->q_lista = 0;

      ch = e->tecla = fwqgc( e->win );

      if ( m_erro && (ch < 32 || (ch >= K_INTR && ch <= K_ALT_F9) ) )
      {
         mens_1( e->men, e->mensagem );
         m_erro = 0;
      }

      if ( strchr( (SC *) e->escape, ch )  !=  NULL )   goto fim;

      if ( ch == sep_dec  ||  ch == sep_int )
      {
         e->cursor = pc_valor( e, e->piece, K_RT );
         continue;
      }

      switch ( ch )
      {
      default      :
                     break;

      case 21    :   /*** CTRL U -> Limpa Tudo Campo ***/
                     e->cursor = pc_valor( e, 0, ch );
      case 25    :   /*** CTRL Y -> Limpa ate o fim ***/
                     e->str[ e->cursor ] = '\0';
                     e->cursor = pc_valor( e, e->piece - 1, K_LFT );
                     continue;

      case K_RT  :
                     e->cursor = pc_valor( e, ++e->cursor, ch );
                     continue;

      case K_LFT  :
                     e->cursor = pc_valor( e, --e->cursor, ch );
                     continue;

      case K_HOME :
                     e->cursor = pc_valor( e, 0, ch );
                     continue;

      case K_END  :
                     e->cursor = pc_valor( e, e->tam_m, ch );
                     continue;

      case K_INS  :
                     insercao  =  !insercao;
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
                     if ( e->cursor  <  e->piece )
                     {
                        SS  I;
                        I = e->cursor;
                        e->cursor = pc_valor( e, --I, K_LFT );
                        del_valor( e );
                        e->cursor = pc_valor( e, ++I, K_RT  );
                     }
                     else
                     {
                        e->cursor = pc_valor( e, --e->cursor, K_LFT );
                        del_valor( e );
                     }
                     continue;

      case K_DEL  :
                     if ( e->str[ e->cursor ] == ' ' )
                     {
                        e->cursor = pc_valor( e, ++e->cursor, K_RT );
                     }
                     del_valor( e );
                     continue;

      case K_ESC   :
                     e->cursor = 0;
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
         e->cursor = pc_valor( e, e->piece, K_LFT );
         e->str[ 0 ] = '\0';
         w_ungc( ch );
         continue;
      }

      switch ( e->mascara[ e->tam_m - 1 ] )
      {
      default  :
      case '9' :
                  if ( ch >= '0' && ch <= '9' )
                  {
                     ins_valor( e, ch );
                     break;
                  }

                  else if ( ch == '-' )
                  {
                     SS   decimais;
                     SC   tmp[ MAX_EDIT + 10 ];
                     DB   db;

                     decimais = e->tam_m - e->piece;
                     if ( decimais > 0 )   decimais--;
                     db = str_to_db( e->str, sep_dec ) * (DB) -1;
                     db_to_str( db, tmp, decimais, 1 /* poe milhar */ );
                     if ( strlen( tmp ) <= e->tam_m )  strcpy( e->str, tmp );
                     break;
                  }

                  win_bell( BEL_INVALID );

                  if ( !m_erro ){ mens_1( e->men, e->masc_num ); m_erro++; }

                  continue;
      }

      if ( m_erro )  {  mens_1( e->men, e->mensagem );  m_erro = 0;  }

   }

fim:

   imp_valor( e, e->n_atr, e->tam_m );

   if ( e->refresh )   fwrefresh( e->win );

   ltrim( e->str );

   if ( ch  !=  K_ESC )   e->mudou = strcmp( e->str, ( SC * ) s1 );

   crit = ( *e->critica )( e );

   if ( crit > 0 )
   {
      imp_valor( e, e->n_atr, e->tam_m );

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

      imp_valor( e, e->n_atr, e->visivel );   fwrefresh( e->win );
   }

sai:

   ltrim( e->str );

   janela->w_curstyle = tipo_cursor;

   janela->w_atrib = atrib;

   if ( prep > 0 )  ch = 0;

   return( ( SS ) ch );
}


ST( VOID ) del_valor( e )

d_entry      *e;
{
   e->cursor = pc_valor( e, e->cursor, K_RT );

   /*** Algo invalido nas conversoes ***/
   e->str[ e->cursor ] = 'n';
}


ST( SS ) pc_valor( e, pos, dir )

d_entry    *e;
SS         pos, dir;
{
   SS      ok, antigo;
   SC      *P;

   e->win->w_curstyle = ( insercao ) ? CS_INTER : CS_NORMAL;

   for ( antigo = 0, P = e->str;
                    *P != '\0' && *P == ' ';   antigo++, P++ )  /* Nada */  ;

   if ( antigo > 0 )    antigo--;
   if ( antigo > 0  &&  e->mascara[ antigo ] == sep_int )  antigo--;

   antigo = pos = COTA( antigo, pos, e->tam_m - 1 );

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


ST( VOID ) ins_valor( e, c1 )

d_entry  *e;
UC       c1;
{
   SC    *p1, *p2, tmp1, tmp2, cursor;

   cursor = e->cursor = pc_valor( e, e->cursor, K_RT );

   if ( e->cursor >= e->tam_m ){  win_bell( BEL_INVALID );  return;  }

   if ( e->tam_m  == 1  || ( ! e->q_lista   &&  ! insercao ) )
   {
      e->str[ e->cursor ] = c1;
      if ( e->cursor != e->piece - 1 )  cursor++;
   }

   else
   {
      if ( e->cursor > e->piece )
      {
         /*** Parte decimal NAO possui mascara ! ***/
         for ( tmp1 = c1, p1 = &e->str[ e->cursor ];   *p1 != '\0';   p1++ )
         {
            tmp2 = *p1;
            *p1  = tmp1;
            tmp1 = tmp2;
         }

         cursor++;
      }

      else
      {
         SC  tmp[ MAX_EDIT + 1 ];

         strcpy( tmp, e->str );

         /*** Parte inteira possui mascara !!! ***/
         for ( tmp1 = c1, p1 = &e->str[ e->cursor ];   p1 >= e->str;    )
         {
            tmp2 = *p1;
            *p1  = tmp1;
            tmp1 = tmp2;

            e->cursor = pc_valor( e, --e->cursor, K_LFT );
            p2 = &e->str[ e->cursor ];

            /*** Se nao conseguio ir para traz, e' melhor parar ***/
            if ( p1 <= p2 )    break;
            p1 = p2;
         }

         /*** Nao consegui inserir ***/
         if ( tmp1 != ' ' ){ strcpy( e->str, tmp );  win_bell(BEL_INVALID); }
      }
   }

   e->cursor = pc_valor( e, cursor, K_RT );
}


ST( SC * ) form_valor( E, str )

d_entry  *E;
SC       *str;
{
   DB   db;
   SS   I, J, decimais;
   SC   *p1, *p2;

   decimais = E->tam_m - E->piece;

   if ( decimais > 0 )   decimais--;

   db = str_to_db( str, sep_dec );
/***
debug( "1 - STR = <%s> \nDB = %f", E->str, db );
***/

   while ( 1 )
   {
      db_to_str( db, str, decimais, 1 /* poe milhar */ );

      if ( (I = strlen( str ))  <=  E->tam_m )   break;

      db  /=  10;
   }

   p1 = str + I - 1;
   p2 = E->str + E->tam_m - 1;

   for ( J = 0;   J < I;          J++ )   *p2-- = *p1--;

   for (      ;   J < E->tam_m;   J++ )   *p2-- = ' ';

   return ( str );
}


ST( VOID ) imp_valor( e, atr, quant )

d_entry     *e;
SS          atr, quant;
{
   SS       ax, ay;

   form_valor( e, e->str );

   e->win->w_atrib = atr;

   ax = e->win->w_ax;
   ay = e->win->w_ay;

   e->win->w_cx = e->col;
   e->win->w_cy = e->lin;

   fwpsl( (UC *) e->str, e->win, MIN( e->tam_m, quant ) );

   e->cursor = pc_valor( e, e->cursor, K_RT );

   e->win->w_ax = ax;
   e->win->w_ay = ay;

   fwat( e->cursor + e->col, e->lin, e->win );
}


ST( SS )   norm_masc_valor( E )

d_entry    *E;
{
   SS      I, J, decimais;
   SC     *M;

   /*** Assume  E->tam_m  >  0 ***/
   I = E->tam_m;
   M = E->mascara + I - 1;

   /*** Se existir decimais, devera' haver pelo menos uma casa ***/
   if ( *M == sep_dec )  *M = '9';

   for ( decimais = -1;   I > 0  &&  decimais == -1;   I--, M-- )
   {
      if ( *M == sep_dec )   decimais = I - 1;
      else                   *M = '9';
   }

   if ( decimais == -1 ) { I = decimais = E->tam_m; M = E->mascara + I - 1; }

   for ( J = 1;   I > 0;   I--, M--, J++ )
   {
      if ( J % 4 )   *M = '9';
      else           *M = sep_int;
   }

   return ( decimais );
}


