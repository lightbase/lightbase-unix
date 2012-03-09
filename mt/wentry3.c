
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <ctype.h>
#include   "window.h"
#include   "entrada.h"

#include   PROTO

US  dayofwk( yr, mo, da )

US yr, mo, da;
{
   US ctmp,dtmp,mtmp,ytmp,n;
   /* if year = 82, assume 1982 */
   if ( yr < 100 )   yr += 1900;

   /* if year given as 182, assume 2182 */
   if ( yr < 1000 )  yr += 2000;

   if ( mo > 2 ) {  mtmp = mo - 2;    ytmp = yr;      }

   else          {  mtmp = mo + 10;   ytmp = yr - 1;  }

   ctmp = ytmp / 100;
   dtmp = ytmp - 100 * ctmp;
   n = ((13 * mtmp - 1) / 5) +
       da + dtmp + (dtmp / 4) + (ctmp / 4) - ctmp - ctmp + 77;

   return( n - 7 * (n / 7) );
}

US  leapyear( yr )

US yr;
{
   US lyr = 0;

   /* if year = 82 assume 1982  */
   if ( yr < 100 )   yr += 1900;

   /* if year given as 182, assume 2182 */
   if ( yr < 1000 )  yr += 2000;

   if ( yr == (4 * (yr / 4)) )       lyr = 1;

   if ( yr == (100 * (yr / 100)) )   lyr = 0;

   if ( yr == (400 * (yr / 400)) )   lyr = 1;

   return( lyr );
}


US  valdate( yr, mo, da )

US yr, mo, da;
{
   /* if year = 82 assume 1982  */
   if ( yr < 100 )   yr += 1900;

   /* if year given as 182, assume 2182 */
   if ( yr < 1000 )  yr += 2000;

   if ( mo < 1 || mo > 12 )   return( 1 );    /* NOT a valid month */

   if ( da < 1 || da > 31 )   return( 4 );    /* NOT a valid day */

   if ( da > 30 && (mo == 9 || mo == 4 || mo == 6 || mo == 11) )
   {
      return( 4 );                            /* NOT a valid day */
   }

   if ( mo == 2 && da > 28 )
   {
      if ( !leapyear( yr ) )  return( 4 );    /* NOT a valid day */

      else  if ( da > 29 )    return( 4 );    /* Not a valid day */
   }

   return( 0 );   /* IS a valid year */
}

/**************************************************************************/
SS    pede_cadeia( prompt, string, tam, mens, masc_char, critica )

SC    *prompt, *string;
SS    tam;
SC    *mens;
SC    masc_char;
SS    ( *critica )( );
{
	SS       C;
	SC       masc[ 122 ];
	WIN      *J;
	d_entry  E;

	if ( tam > 120 )  tam = 120;

	for ( C = 0;   C < tam;   C++ )   masc[ C ] = masc_char;
	masc[ C ] = '\0';

	J = win_make( size_x, 1, size_x, 1, 0, size_y - 1 );

	if ( J == NULL )  return( K_ESC );

	fwprintf( J, "%.20s", prompt );

	d_entrada( &E, J, string, C, masc, mens, J->w_cx, J->w_cy,
				E_ALFA | E_LIMPA | E_CONFIRMA );

	if ( critica )   E.critica = critica;

	C = entrada( &E, 0 );

	fwkill( J );

	return( C );
}

#ifdef NAODEF

SS    pede_str( prompt, string, tam, mens, masc_char, critica )

SC    *prompt, *string;
SS    tam;
SC    *mens;
SC    masc_char;
SS    ( *critica )( );
{
   SS       C, V, P;
   SC       masc[ 81 ];
   WIN      *J;
   d_entry  E;

   if ( tam > 80 )  tam = 80;

   for ( C = 0;   C < tam;   C++ )   masc[ C ] = masc_char;
   masc[ C ] = '\0';

   P = strlen( prompt );

   V = MIN( size_x - 2, P + C );

	J = win_make( V, 1, V, 1, 1, size_y - 4 );

   if ( J == NULL )  return( -1 );

   fwbox( J );

   fwprintf( J, "%.20s", prompt );

   d_entrada( &E, J, string, C, masc, mens, J->w_cx, J->w_cy,
              E_ALFA | E_LIMPA | E_CONFIRMA );

   if ( critica )   E.critica = critica;

   C = entrada( &E, 0 );

   fwkill( J );

   return( C );
}

#endif

SS     men_conf( texto, opcoes, ajuda1, win )

SC     *texto, *opcoes, *ajuda1;
WIN    *win;
{
   SS    C = -1, ret;
   SC    *pos, *ajuda2;
   WIN   *jm, *W;

   ajuda2 = win_ajuda;
   win_ajuda = ajuda1;

	jm = abre_mensagem( texto );

   if ( jm == NULL )   return( K_ESC );

   if ((W = win) == NULL )   W = jm;

   for ( pos = opcoes;   *pos;  pos++ )
   {
      /*** Letras minusculas sao transformadas em CTRL ***/
      if ( *pos >= 'a'  &&  *pos <= 'z' )   *pos -= 96;
   }

   while ( (pos = strchr(opcoes, C ) ) == NULL )
   {
      if ( C != -1 )   win_bell( BEL_INVALID );

      fwrefresh( W );
      fwsetcursor( W );

		C = fwnqgc ( jm );

      if ( C == K_ESC ) {  ret = K_ESC;  goto fim;  }

      if (  C >= 'a'  &&  C <= 'z' )  C -= 32;

      if ( opcoes[ 0 ] == '*' )       C = '*';  /* Qualquer Tecla */
   }

   ret = ( SS ) ( pos -  opcoes );

fim :

   fecha_mensagem( jm );

   win_ajuda = ajuda2;

   return( ret );
}


SS  w_rename( atual, futuro )

SC *atual, *futuro;
{
#ifdef   MSDOS

   return ( rename( atual, futuro ) );

#else
#ifdef   VAXVMS

   return ( rename( atual, futuro ) );

#else

   if ( link( atual, futuro ) == 0 )   return( unlink( atual ) );

   return ( -1 );

#endif
#endif
}

#ifdef   STR_PROPRIA

SC  *w_memmove( dest, orig, tam )

SC  *dest, *orig;
US  tam;
{
   US   I;
   register SC   *A, *B;

   A = dest;
   B = orig;

   if ( A > B )
   {
      A += tam - 1;
      B += tam - 1;

      for ( I = 0;   I < tam;   I++ )   *A-- = *B--;
   }

   else
   {
      for ( I = 0;   I < tam;   I++ )   *A++ = *B++;
   }

   return ( orig );
}


SC  *w_strstr( s1, s2 )

SC  *s1, *s2;
{
   SC  C, *P, *F;
   SS  T;

   C = *s2;
   T = strlen( s2 );
   F = s1 + strlen( s1 );

   for( P = s1;   (P = strchr( P, C )) != NULL  &&  F - P >= T;   P++ )
   {
      if ( strncmp( P, s2, T ) == 0 )    return( P );
   }

   return ( NULL );
}

SI  w_stricmp( s1, s2 )

SC  *s1, *s2;
{
   SI   ret;

   ret = toupper( *s1 ) - toupper( *s2 );

   for (   ;   *s1  &&  *s2  &&  ret == 0;   s1++, s2++ )
   {
      ret = toupper( *s1 ) - toupper( *s2 );
   }

   return( ret );
}


SI  w_strncmp( s1, s2, max )

SC  *s1, *s2;
SS  max;
{
   SI  ret;

   for ( ret = *s1 - *s2;  *s1  &&  *s2  &&  ret == 0  &&  max > 0;  max-- )
   {
      ret = *s1++  -  *s2++;
   }

   return( ret );
}


SC   *w_strlwr( s )

SC   *s;
{
   register UC *s1;

   for ( s1 = (UC *) s;   *s1;   s1++ )   *s1 = tolower( *s1 );

   return( s );
}

SC  *w_strupr( s )

SC  *s;
{
   register UC *s1;

   for ( s1 = (UC *) s;   *s1;   s1++ )   *s1 = toupper( *s1 );

   return( s );
}


#endif


