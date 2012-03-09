 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "window.h"
#include "mensagem.h"
#include "entrada.h"

#define   extern    /* Declara */
#include  <calcula.h>
#undef    extern

VOID   calculadora( )

{
	SS       R, T;
	UC       escape[ 10 ];
   WIN      *win;
   WIN      *JM;
   WIN      *JF;
   d_entry  E;
	SC       str[  TAM_EXPR + 1 ];
	SC       masc[ TAM_EXPR + 1 ];
	SC       anterior[ TAM_EXPR + 1 ];
	SC       outstr[ 70 ], *ajuda;
#ifdef  MEDIT   
   SS   lin = 1;
#else   
   SS   lin = 2;
#endif   
   static   inicio = 0;
#ifdef        MSDOS
   UI       di;  /* longjmp nao restaura DI no TURBO_C ++ */
#endif

   if ( ! inicio ) {   inicio++;  var_inicio( );  }

   win = win_make( 52, 10, 52, 10, size_x, lin );

   if ( win == NULL )   return;

   win->w_optatr = win->w_boxatr; 
   win->campoatr = win->w_boxatr; 

   up_str( STR_CALC );

   fw_title( win, loc_str( STR_CALC, T_CALC ), win->t_atr );

   sprintf( str, "1 0%s E", loc_str( STR_CALC, CALC_F10 ) );

   JF = abre_funcoes( funcao( str ) );

   JM = abre_mensagem( loc_str( STR_CALC, M_CALEDITA ) );

   for ( R = 0;   R  <  TAM_EXPR;  R++ )   masc[ R ] = 'x';

   masc[ R ] = '\0';

   ajuda = win_ajuda;

   win_ajuda = A_CALCULA;

   escape[ 0 ] = K_F0;
   escape[ 1 ] = outstr[ 0 ] = anterior[ 0 ] = '\0';  T = 0;

   c_verbo = NULL;   /*** Nao existem verbos externos ***/
   dv_zero = '?';  
   hxmod = 0;  /* Decimal */
   c_prompt = 'D';

	while( 1 )
	{
	   len = val = err = ind = paren = 0;

      if ( T == K_UP )
      {  SC aux[10];
         sprintf(aux, "\r%c : ", c_prompt );
         strcpy( str, anterior );
         fwprintf( win, "%s", aux);
      }

      else
      {  SC aux[10];
         sprintf(aux, "\n%c : ", c_prompt );
         str[ 0 ] = '\0';
         fwprintf( win, "%s", aux);
      }

      /* E necessario ter refresh() pois a entrada vai abrir outra janela */

      fwrefresh( win );

      d_entrada( &E, win, str, TAM_EXPR, masc, NULL,
                     win->w_cx, win->w_cy, E_ALFA | E_CONFIRMA );

      E.escape = escape;

      T = entrada( &E, 0 );

      mens_1( JM, loc_str( STR_CALC, M_CALEDITA ) );

      if ( T == K_ESC )   break;

      if ( T == escape[ 0 ] )  {   w_ungs( outstr );   break;  }

		if ( ( len = strlen( str ) ) == 0  ||  T != K_ENTER )    continue;

      strcpy( anterior, str );

#ifdef        MSDOS
   di = _DI;  /* longjmp nao restaura DI no TURBO_C ++ */
#endif

		err = setjmp( SV );

#ifdef        MSDOS
   _DI=di;  /* longjmp nao restaura DI no TURBO_C ++ */
#endif

		if ( err == 0 )
		{
			R = getass( str );

			switch( R )
			{
			case  0: 
				      val = gexpr( str );
			case -1: 
      				break;

			case -2:
			case -3: 
			case -4: 
			case -5: 
      				continue;

			default: val = var.lett[ R - 'a' ];
			}
		}

		if ( paren  &&  ! err )   err = -1;

      if ( err ) {  calc_erro( err, JM );  continue;  }

      fwprintf( win, "\n%s", loc_str( STR_CALC, M_RES_D ) );

		sprintf( outstr, "%-18.13g", val );

      fwprintf( win, "%s\n", outstr );

      for ( R = 0;   outstr[ R ]  >  ' ';   R++ )
         ;

      outstr[ R ] = '\0';

      if ( hxmod  )
      {
		   if ( hxmod == 1 )
		   {
            fwprintf( win, "%s", loc_str( STR_CALC, M_RES_H ) );

			   makhex( val, outstr );
		   }

		   else if ( hxmod == 2 )
		   {
            fwprintf( win, "%s", loc_str( STR_CALC, M_RES_B ) );

			   makbin( val, outstr );
		   }

		   else 
		   {
            fwprintf( win, "%s", loc_str( STR_CALC, CALC_26 ) );

			   makoct( val, outstr );
		   }

         fwprintf( win, "%s\n", outstr );
      }

		vals[ C_V ] = val;

		C_V = ( ++C_V ) % C_RETRO;
	}

   fecha_mensagem( JM );
   fecha_funcoes(  JF );
   fwkill( win );
   win_ajuda = ajuda;

   down_str( STR_CALC );
}


SS   getass( P )

SC     *P;
{
	SS      chmod, V, J;
	SC      ch;
	DB  T;

	chmod = V = J = 0;

	while( J < len )
	{
		ch = P[ J++ ];

		if ( V + chmod == 0 )
		{
			switch ( ch )
			{
			case 'H':
                     chmod = hxmod = 1;
				         c_prompt = 'H';     /* Hexa */
				         ind = J;
				         continue;

			case 'B': 
				         chmod = 1;
				         hxmod = 2;          /* binary */
				         c_prompt = 'B';
         				ind = J;
				         continue;

			case 'O': 
				         chmod = 1;
				         hxmod = 3;          /* Octal */
				         c_prompt = 'O';
         				ind = J;
				         continue;

			case 'D': 
				         hxmod = 0;          /* Decimal */
				         chmod = 1;
				         c_prompt = 'D';
			case ' ':
         				ind = J;
				         continue;
			}

			if ( isupper( ch ) )   ch = tolower( ch );

			if ( strchr( "rep", ch ) )   return 0;

			if ( strchr( "hdb", ch )  &&  isxdigit( P[ J ] ) )   return 0;

			if ( islower( ch )  &&  !isalpha( P[ J ] ) )
         { 
				V = ch; 
				continue; 
			}

			else   return 0;
		}

		switch( ch )
		{
		case ' ': 
			         continue;

		case '=': 
			         ind = J;
         			T = gexpr( P );
         			var.flags |= ( UL ) pow( 2.0, (DB) ( V - 'a' ));
         			var.lett[ V - 'a' ] = T;
         			return V;

		case '\\': 
         			var.flags |= ( UL ) pow( 2.0, (DB) ( V - 'a' ));
         			var.lett[ V - 'a' ] = val;
         			return V;

		default: 
         			return 0;
		}
	}

	if ( V )    return 0;

	else        return -3;
}


DB  gexpr( P )

SC       *P;
{
	DB   temp;

	temp = gterm( P );

	while ( ind < len )
	{
		switch ( P[ ind++ ] )
		{
		case ')': 
			         if ( --paren  <  -20 )   longjmp( SV, -10 );
         			return temp;

		case '-': 
         			temp -= gterm( P );
		case ' ':
         			continue;

		case '+': 
         			temp += gterm( P );
         			continue;

		case '&': 
         			temp = ( SL ) temp  &  ( SL ) gterm(P);
         			continue;

		case '|': 
         			temp = ( SL ) temp  |  ( SL ) gterm( P );
         			continue;

		default: 
         			longjmp( SV, -3 );
		}
	}

	return  temp;
}


DB  gterm( P )

SC   *P;
{
	SC      ch;
	DB  temp, F;

	temp = gpowr( P );

	while( ind < len )
	{
		ch = P[ ind++ ];

		switch( ch )
		{
		case '*':
			         temp *= gpowr( P );
         			continue;

		case '%': 
         			F = gpowr( P );
         			if ( F == 0 )
                  {
                     if ( dv_zero == 'A' )   F = 1;
                     else                    longjmp( SV, -6 );
                  }
         			temp = fmod( temp, F );
		case ' ': 
         			continue;

		case '/': 
         			F = gpowr( P );
         			if ( F == 0 )
                  {
                     if ( dv_zero == 'A' )   F = 1;
                     else                    longjmp( SV, -6 );
                  }
         			temp /= F;
         			continue;

		default:  
         			longjmp( SV, -3 );

		case '+':
		case '-':
		case '&':
		case '|':
		case ')':
         			break;
		}

		ind--;
		break;
	}

	return   temp;
}


DB  gpowr( P )

SC       *P;
{
	SC       ch;
	DB   temp, F;

	temp = garg( P, 0 );

	while( ind < len )
	{
		ch = P[ ind++ ];

		switch( ch )
		{
		case ' ': 
         			continue;

		case '^': 
         			F = garg( P, 0 );
         			if ( F < 0 )   F = ceil( F );
         			temp = pow( temp, F );
         			continue;

		case '=': 
         			longjmp( SV, -9 );

		default:  
         			longjmp( SV, -3 );

		case '+':
		case '-':
		case ')':
		case '*':
		case '/':
		case '%':
		case '&':
		case '|':
         			break;
		}

		ind--;

		break;
	}

	return   temp;
}


DB  garg( P, mod )

SC    *P;
SS    mod;
{
	UC  ch, tmp_ch;
	SS  arg = 0,  vno = 0;
	DB  temp = 0;

	while( ind < len )
	{
		ch = P[ ind++ ];

		if ( isupper( ch ) )   ch = tolower( ch );

		if ( arg )
		{
			if ( ch == '!' )    return fact( temp );

			if ( ch == ' ' )    continue;

			if ( strchr( "+*-/%^&)|", ch ) )
         {
				ind--; 
				return  temp; 
			}

			longjmp( SV, -4 );
		}

		if ( vno )
		{
			if ( ch == '\\' )  {  vno++;   continue;  }

			arg++;

			if ( vno > C_RETRO )   vno = C_RETRO;

			temp = vals[ ( C_RETRO + C_V - vno ) % C_RETRO ];

			ind--;

			continue;
		}

      if ( c_valor( P, ind - 1 ) ) 
		{
			ind--;
			temp = gnum( P, 0 );
			arg++;
			continue;
		}

      /*** Se for Alfa ou inicio de expressao MATRIZ ***/
		if ( islower( ch ) || ch > UG_SP || strchr( "_@#", ch ) )
		{
         /*** Se o char seguinte for Alfa ou expressao MATRIZ ***/
         /*** NAO pode ter BRANCO !!! ***/
         tmp_ch = P[ ind ];
   		if ( ind < len  &&
              (isalpha( tmp_ch ) || isdigit( tmp_ch ) ||
               tmp_ch > UG_SP    || strchr( "[]_@#", tmp_ch ) ) )
			{
				temp = gtrig( P, ch );
				arg++;
				continue;
			}

         if ( ( var.flags & ( UL ) pow( 2.0, (DB) ch - 'a' ) ) == 0 )
         {
				longjmp( SV, -2 );
         }

			temp = var.lett[ ch - 'a' ];
			arg++;
			continue;
		}

		switch( ch )
		{
		case '\\': 
			         vno++;
			         continue;

		case '-': 
			         if ( mod )   longjmp( SV, -3 );

			         return ( -garg( P, 1 ) );

		case '(': 
			         paren++; 
			         if ( paren > 20 )   longjmp( SV, -10 );
			         temp = gexpr( P );
			         arg++;
		case ' ': 
         			continue;

		case '=': 
         			longjmp( SV, -9 );

		default:  
         			longjmp( SV, -4 );
		}
	}

	if ( vno )
	{
		if ( vno > C_RETRO )   vno = C_RETRO;

		return ( vals[ ( C_RETRO + C_V - vno ) % C_RETRO ] );
	}

	if ( arg == 0 )    longjmp( SV, -4 );

	return temp;
}

SS c_valor( P, I )

SC *P;
SS  I;
{
   SS ch, flag = 0;

	ch = P[ I++ ];

   if ( isdigit( ch ) )   return( 1 );

   else if ( ch == 'd'  ||  ch == '.' )
   {
      while ( 1 )
      {
	      ch = P[ I++ ];
         if ( isdigit( ch ) ) {  flag = 1;   continue;  }
			if ( strchr( " +*-/%^&)|", ch )  &&  flag )   return( 1 );
         return ( 0 );
      }
   }

   else if ( ch == 'h' )
   {
      while ( 1 )
      {
	      ch = P[ I++ ];
         if ( isxdigit( ch ) ) {  flag = 1;   continue;  }
			if ( strchr( " +*-/%^&)|", ch )  &&  flag )   return( 1 );
         return ( 0 );
      }
   }

   else if ( ch == 'b' )
   {
      while ( 1 )
      {
	      ch = P[ I++ ];
         if ( ch == '0'  ||  ch == '1' ) {  flag = 1;   continue;  }
			if ( strchr( " +*-/%^&)|", ch )  &&  flag )   return( 1 );
         return ( 0 );
      }
   }

   else if ( ch == 'o' )
   {
      while ( 1 )
      {
	      ch = P[ I++ ];
         if ( ch >= '0'  &&  ch <= '7' ) {  flag = 1;   continue;  }
			if ( strchr( " +*-/%^&)|", ch )  &&  flag )   return( 1 );
         return ( 0 );
      }
   }

   return ( 0 );
}



