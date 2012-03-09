 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <setjmp.h>
#include "window.h"
#include "mensagem.h"
#include "entrada.h"

#include  <calcula.h>


SS  calc_avalia( str, D, JM, v_verbo, flag_dvzero )

SC      *str;
DB  *D;
WIN     *JM;
SS     (*v_verbo)( );
SS     flag_dvzero;
{
   SS   R;
   static   inicio = 0;
#ifdef      MSDOS
   UI       di;  /* longjmp nao restaura DI no TURBO_C ++ */
#endif

   if ( ! inicio ) {   inicio++;  var_inicio( );  }

   c_verbo = v_verbo;   /*** Existem verbos externos - VARIAVEIS ***/
   dv_zero = flag_dvzero;

   len = val = err = ind = paren = 0;

   hxmod = 0; /*** Modo decimal ***/

   len = strlen( str );

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
               break;

      case -1: 
      case -2:
      case -3: 
      case -4: 
      case -5: 
               err = -3;
               break;

      default: val = var.lett[ R - 'a' ];
               break;
      }
   }

   if ( paren  &&  ! err )   err = 1;

   if ( err )   calc_erro( err, JM );

/***
   debug( "Formula = <%s>  \nVal = %18.13g   \nErro = %d", str, val, err );
***/

   *D = val;

   return( err );
}


DB  gtrig( P, C )

SC   *P, C;
{
   UC   ch;
   SS   J = 1, fun;
   DB   temp = 0;

   vf_buf[ 0 ] = C;

   while( ind < len )
   {
      ch = P[ ind++ ];

      if ( isupper( ch ) )   ch = tolower( ch );

      /*** Aqui pode ser uma funcao ou um campo externo ou ... ***/
      /*** ... um pesquisa em matriz tipo #MT01["Indice",1,2] ***/
      /*** NAO PODE ter BRANCOS  ***/
      if ( islower( ch ) || isdigit( ch ) ||
           ch > UG_SP || strchr( "\"\',_[]@#", ch ) )
      {
         if ( J > VERB_LEN )   longjmp( SV, -7 );
         vf_buf[ J++ ] = ch;
         continue;
      }

      vf_buf[ J ] = 0;

      if ( c_verbo != NULL  &&  (*c_verbo)( vf_buf, &temp ) >= 0 )
      {
         ind--;
         return( temp );
      }

      if ( ch == '(' )
      {
         paren++;

         if ( paren > 20 )    longjmp( SV, -10 );

         if ( strcmp( vf_buf, loc_str( STR_CALC, F_LN )  )       == 0 )   fun = 1;
                                                  
         else if ( strcmp( vf_buf, loc_str( STR_CALC, F_LOG )  ) == 0 )   fun = 2;

         else if ( strcmp( vf_buf, loc_str( STR_CALC, F_SIN )  ) == 0 )   fun = 3;

         else if ( strcmp( vf_buf, loc_str( STR_CALC, F_COS )  ) == 0 )   fun = 4;

         else if ( strcmp( vf_buf, loc_str( STR_CALC, F_TAN )  ) == 0 )   fun = 5;

         else if ( strcmp( vf_buf, loc_str( STR_CALC, F_ASIN ) ) == 0 )   fun = 6;

         else if ( strcmp( vf_buf, loc_str( STR_CALC, F_ACOS ) ) == 0 )   fun = 7;

         else if ( strcmp( vf_buf, loc_str( STR_CALC, F_ATAN ) ) == 0 )   fun = 8;

         else      longjmp( SV, -7 );

         temp = gexpr( P );

         switch( fun )
         {
         case 1: 
                  if ( temp <= 0 )   longjmp( SV, -8 );
                  return  log( temp );

         case 2: 
                  if ( temp <= 0 )   longjmp( SV, -8 );
                  return log10( temp );

         case 3: 
                  return  sin( temp );

         case 4: 
                  return  cos( temp );

         case 5: 
                  return tan( temp );

         case 6: 
                  if ( temp > 1  ||  temp < -1 )   longjmp( SV, -8 );
                  return  asin( temp );

         case 7: 
                  if ( temp > 1  ||  temp < -1 )   longjmp( SV, -8 );
                  return  acos( temp );

         case 8: 
                  return  atan( temp );
         }
      }

      break;
   }

   vf_buf[ J ] = 0;

   if ( ind >= len )
   {
      if ( c_verbo != NULL && (*c_verbo)( vf_buf, &temp ) >= 0 )  return( temp );
   }

   longjmp( SV, -7 );

   return 0;       /* dummy return avoids error msg */
}


DB  gnum( P, mod )

SC   *P;
SS   mod;        /* 0 =  dec, 1 = exp, 2 = exp after - */
{
   SC      ch;
   SS      xmod, nmod = 0, inum = 0, dec = 0;
   DB      temp = 0;

   xmod = hxmod;

   while ( ind < len )
   {
      ch = P[ ind++ ];

      if( isupper( ch ) )    ch = tolower( ch );

      if ( !(inum + nmod) )
      {
         if ( strchr( "dhbo", ch ) )
         {
            nmod++;
            xmod = ( ch == 'h' ) + 2 * ( ch == 'b' ) + 3 * ( ch == 'o' );
            continue;
         }

         if ( strchr( "acef", ch ) )
         {
            xmod = 1;
            nmod++;
         }
      }

      if ( isxdigit( ch )  &&  xmod )
      {
         if ( xmod == 2 )
         {
            if ( ch != '1'  &&  ch != '0' )    longjmp( SV, -11 );
            inum++;
            temp = 2 * temp + (ch - 48);
            continue;
         }

         if ( xmod == 3 )
         {
            if ( ch < '0'  ||  ch > '7' )    longjmp( SV, -12 );
            inum++;
            temp = 8 * temp + (ch - 48);
            continue;
         }

         inum++;
         temp = 16 * temp + (ch - 48) - 39 * (ch > 64);
         continue;
      }

      if ( ch == 'e'  &&  inum  &&  mod == 0 )
      {
         return ( temp * pow( 10.0, gnum( P, 1 ) ) );
      }

      if ( isdigit( ch ) )
      {
         inum++;

         if ( dec == 0 )    temp = 10 * temp + (ch - 48);

         else 
         {
            temp += ( (ch - 48) / pow( 10.0, (DB) dec ) );
            dec++;
         }

         continue;
      }

      if ( ch == '.' )
      {
         if ( dec || mod || xmod )   longjmp( SV, -5 );
         dec++;
         continue;
      }

      if ( ch == '-'  &&  inum == 0  &&  mod == 1 )
      {
         return( -gnum( P, 2 ) );
      }

      if ( ch == '+'  &&  inum == 0  &&  mod == 1 )  return( gnum( P, 2 ) );

      if ( inum == 0 )   break;

      if ( strchr( " +/-*^!%)&|", ch ) )
      {
         ind--;
         return temp;
      }

      longjmp( SV, -4 );
   }

   /* jairo, estes dois if() nao ESTAVAM muito certos */
   /* Foram identados corretamente porem nao sei se esta correto */
   if ( inum == 0 )
   {
      if ( dec )   longjmp( SV, -5 );
      else         longjmp( SV, -4 );
   }

   return temp;
}


DB  fact( X )

DB X;
{
   DB  J;

   X = floor( X );

   if ( X > 170 )   longjmp( SV, -4 );
   
   for ( J = X;  J > 2;    )   X *= --J;

   return  X;

/***
   if ( X == floor( X )  &&  X < 17 )
   {
      for ( J = ( SS ) X;  J > 2;    )  X *= --J;

      return  X;
   }

   xx = X * X;

   return ( sqrt( 2 * PI * X ) * pow( X / exp( 1.0 ), X ) *
            ( 1 + 1 / ( 12 * X ) + 1 / ( 288 * xx ) - 139 /
               ( 51840L * xx * X ) - 571 / (2488320L * xx * xx ) ) );
***/
}


VOID    makhex( X, Y )

DB   X;
SC      *Y;
{
   SC      S[ 24 ];
   SS      R, I = 22;
   DB  F;

   S[ 23 ] = '\0';

   F = floor( fabs( X ) );

   while ( F  &&  I > 1 )
   {
      R = ( SS ) fmod( F, 16.0 );
      F = floor( F / 16.0 );
      S[ I-- ] = 0x30 + R + ( 0x7 * ( R > 9 ) );
   }

   if ( I == 22 )       S[ I-- ] = '0';

   else if ( I == 1 )   strcpy( S + 2, "+++ Overflow +++" );

   else if ( X < 0 )    S[ I-- ] = '-';

   strcpy( Y, S + I + 1 );
}


VOID    makbin( X, Y )

DB  X;
SC     *Y;
{
   SC      S[ 44 ];
   SS      R, I = 42;
   DB  F;

   S[ 43 ] = 0;

   F = floor( fabs( X ) );

   while ( F  &&  I > 1 )
   {
      R = ( SS ) fmod( F, 2.0 );
      F = floor( F / 2.0 );
      S[ I-- ] = 0x30 + R;
   }

   if ( I == 42 )      S[ I-- ] = '0';

   else if ( I == 1 )  strcpy( S + 2, "+++ Overflow +++" );

   else if ( X < 0 )   S[ I-- ] = '-';

   strcpy( Y, S + I + 1 );
}


VOID    makoct( X, Y )

DB  X;
SC     *Y;
{
   SC      S[ 44 ];
   SS      R, I = 42;
   DB  F;

   S[ 43 ] = 0;

   F = floor( fabs( X ) );

   while ( F  &&  I > 1 )
   {
      R = ( SS ) fmod( F, 8.0 );
      F = floor( F / 8.0 );
      S[ I-- ] = 0x30 + R;
   }

   if ( I == 42 )      S[ I-- ] = '0';

   else if ( I == 1 )  strcpy( S + 2, "+++ Overflow +++" );

   else if ( X < 0 )   S[ I-- ] = '-';

   strcpy( Y, S + I + 1 );
}


VOID  calc_erro( err, JM )

SS    err;
WIN   *JM;
{
   switch ( err )
   {
   case -1: 
            mens_2( JM, "%s", loc_str( STR_CALC, M_UNB_PAR ) );
            break;
   case -2: 
            mens_2( JM, "%s", loc_str( STR_CALC, M_UNS_VAR ) );
            break;
   case -3: 
            mens_2( JM, "%s", loc_str( STR_CALC, M_ERR_STX ) );
            break;
   case -4: 
            mens_2( JM, "%s", loc_str( STR_CALC, M_ERR_IMP ) );
            break;
   case -5: 
            mens_2( JM, "%s", loc_str( STR_CALC, M_ERR_DEC ) );
            break;
   case -6: 
            if ( dv_zero != 'N' )   mens_2( JM, "%s", loc_str( STR_CALC, M_ERR_ZER ) );
            break;
   case -7: 
            mens_2( JM, "%s - [%s]", loc_str( STR_CALC, M_ERR_VER ), vf_buf );
            break;
   case -8: 
            mens_2( JM, "%s - [%s]", loc_str( STR_CALC, M_ERR_FUN ), vf_buf );
            break;
   case -9: 
            mens_2( JM, "%s", loc_str( STR_CALC, M_ERR_ASS ) );
            break;
   case -10: 
            mens_2( JM, "%s", loc_str( STR_CALC, M_ERR_P20 ) );
            break;
   case -11: 
            mens_2( JM, "%s", loc_str( STR_CALC, M_ERR_BIN ) );
            break;
   case -12: 
            mens_2( JM, "%s", loc_str( STR_CALC, CALC_27 ) );
            break;
   }
}

VOID var_inicio( )
{
   for( C_V = 0;   C_V < C_VARS;   C_V++ )   var.lett[ C_V ] = (DB) 0;

   var.lett[  4 ] = exp( 1.0 ); /*** Variavel 'e' -> Neperiano ***/
   var.lett[ 15 ] = PI;         /*** Variavel 'p' -> Pi        ***/
   var.lett[ 17 ] = PI / 180;   /*** Variavel 'r' -> Constante para ***/
                                /*** transformar  radianos em graus ***/
   var.flags = 163856L;

   for( C_V = 0;   C_V < C_RETRO;   C_V++ )   vals[ C_V ] = 0;
   C_V = 0;
}

