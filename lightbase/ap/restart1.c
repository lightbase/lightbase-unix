/* OK */
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>

#include   "restart.h"

#ifdef  MSDOS
#include <ctype.h>
#endif

#define   TAB_CHAR  9

unsigned short  leapyear( yr )

unsigned short yr;
{
   unsigned short lyr = 0;

   /* if year = 82 assume 1982  */
   if ( yr < 100 )   yr += 1900;

   /* if year given as 182, assume 2182 */
   if ( yr < 1000 )  yr += 2000;

   if ( yr == (4 * (yr / 4)) )       lyr = 1;

   if ( yr == (100 * (yr / 100)) )   lyr = 0;

   if ( yr == (400 * (yr / 400)) )   lyr = 1;

   return( lyr );
}


unsigned short  valid_data( yr, mo, da )

unsigned short yr, mo, da;
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

char   *eliminabr( str )

char   *str;
{
   /* ---> Tira todos os brancos e TABs do inicio, fim e meio de "str" */
   char *p, *s;

   s = p = str;

   while( *p )
   {
      while( *p == ' ' ||  *p == TAB_CHAR )   ++p;

      while( *p  &&  *p != ' '  &&  *p != TAB_CHAR )   *s++ = *p++;
   }

   *s = 0;

   return ( str );
}

char   *tirabr( str )

char   *str;
{
   /* ---> Tira todos os brancos e TABs do inicio e do fim de str, e os
         brancos excessivos internos de str.
   */

   char *S, *P;

   S = P = str;

   while( *P == ' ' || *P == TAB_CHAR )   ++P;

   while( *P )
   {
      while( *P != ' ' &&  *P != TAB_CHAR )
      {
         *S++ = *P++;

         if ( ! *P )   break;
      }

      if ( *P == ' ' || *P == TAB_CHAR )
      {
         *S++ = *P++;

         while( *P == ' ' || *P == TAB_CHAR )   ++P;

         if ( ! *P )   --S;
      }
   }

   *S = 0;

   return ( str );
}

int pega_tk( cadeia, separador, result )

char *cadeia, *separador;
PT_TOKEN *result;
{
   /*
      Pega tudo de cadeia até encontrar separador. Quando encontrar
      coloca 'zero' na posicao e retorna 1. Se nao encontrar retorna 0.
   */

   static char *pt_inicio, *pt_start;

   if( cadeia )   pt_inicio = cadeia;
   else           pt_inicio = pt_start;

   result->cadeia = pt_inicio;
   result->achou  = 0;

   for( ; *pt_inicio; ++pt_inicio )
   {
      if( strchr( separador, *pt_inicio ) )
      {
         result->achou = 1;
         result->sep   = *strchr( separador, *pt_inicio );
         *pt_inicio = 0;
         pt_start   = ++pt_inicio;
         goto fim;
      }
   }
   pt_start = pt_inicio;

fim:
   return result->achou;
}

void Strupr( str )
char *str;
{
   while(*str)
   {
      *str = toupper(*str);
      ++str;
   }
   return;
}

