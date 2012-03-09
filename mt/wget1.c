
#ifdef comout

fwgc( win )      - get a character ( like getchar())
w_ungs ( )       Usa w_ungc( SS ); para colocar uma string
w_kbd ( kbd )    /*** Usa w_ungc( SS ); na ordem natural ***/
                 /*** Exemplo: w_kbd( 'T', 'e', 's', 't', 'e', 0 ); ***/
                 /*** ATENCAO: ZERO termina a cadeia !!! ***/
win_kdsc( key )  - returns description of user defined keys.
#endif

#include <stdio.h>
#include <string.h>
#include <window.h>

#ifdef    MT_VARARGS
#include  <varargs.h>
#endif

SC *win_kdsc( key )   /* a function useful for getting the description */
UC key;               /* give key # ( as per K_ESC, or K_F1 ) */
                      /* returns pointer to the description */
                      /* also works on '\b' && '\n' */
{
   SS a;

   for( a = 0;  a < kc_sp;  a++ )
   {
      if( *kc_ary[ a ].kc_to  ==  key )
      {
         return ( (SC *) kc_ary[ a ].kc_dsc );
      }
   }

   return "";
}


SS fwgc( win ) /* get a character - echo if needed only printables echoed */

WIN  *win;
{
   SS a;

   db1( "fwgc" );

   a = fwqgc( win );

   if ( (a >= ' ' && a <= '~') || a == '\n' || (a >= UG_SP && a != K_BTAB) )
   {
      fwpc( a, win );
      fwrefresh( win );
   }

   return a;
}


VOID w_ungs ( ung_s ) /*** Usa w_ungc( SS ); para colocar uma string ***/

SC    *ung_s;
{
   UC   I;

   for ( I = strlen( ung_s );   I > 0;   ) { w_ungc( (UC) ung_s[ --I ] ); }
}


/* Usa w_ungc( SS ); na ordem natural            */
/* Exemplo: w_kbd( 'T', 'e', 's', 't', 'e', 0 ); */
/* ATENCAO: ZERO termina a cadeia !!!            */

#define  TAM_KBD  60

#ifdef   PROTO_ANSI
VOID w_kbd ( SI kbd, ... )
#else
#ifdef   MT_VARARGS
VOID w_kbd ( va_alist )
va_dcl
#else
VOID w_kbd ( kbd )
SI kbd;
#endif
#endif
{
   SI   i, j = 1;
   SC   strkbd[ TAM_KBD + 1 ];

#ifdef    MT_VARARGS
   va_list   ap;
   va_start( ap );
#else
   SI *pi;
#ifndef   MSDOS
   if ( inc_int < 0 )   j = -1;   /*** Stack Crescente ***/
#endif
#endif

#ifdef    MT_VARARGS

   for ( i = 0, j = va_arg( ap, SI );   i < TAM_KBD  &&  j != 0;   i++ )
   {
      strkbd[ i ] = j;
      j = va_arg( ap, SI );
   }
   va_end( ap );

#else

   for ( i = 0, pi = &kbd;   i < TAM_KBD  &&  *pi != 0;   i++ )
   {
      strkbd[ i ] = *pi;
      pi += j;
   }
#endif

   strkbd[ i ] = '\0';

   w_ungs( strkbd );
}

