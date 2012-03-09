#include  <stdio.h>
#include  <stdlib.h>
#include <ctype.h>
#include  <string.h>

#define   TEXT signed char
#define   SL signed long
#define   COUNT unsigned short
#define   UC unsigned char
#define   UL unsigned long

COUNT valcgc(TEXT *);


main()
{  TEXT str[100];

   strcpy(str, "63005573000128");
   printf( "%s retorno de valcgc = %d\n", str, valcgc(str));

   strcpy(str, "00163672000118");
   printf( "%s retorno de valcgc = %d\n", str, valcgc(str));


   while ( gets(str) != NULL ) {
      if ( str[0] == 0 )
         break;

      printf( "%s retorno de valcgc = %d\n", str, valcgc(str));
   }

   return(0);
}

COUNT valcgc(str)
TEXT *str;
{
	COUNT i, prod, d1, d2;
   TEXT *p;
   TEXT dig[15], *mult;


	if ( strlen(str) < 14)
      return(-1);

   p = str;
   i = 1;
   while ( *p != '\0' && i <= 14 ) {
		if ( isdigit(*p) ) {
         dig[i++] = *p - '0';
      }
      p++;
   }

   if ( i != 15 ) 
      return(-1);

#ifdef FORM_ANT_CGC
   COUNT pp;
   COUNT soma;
/* nao e mais usado pela receita */
   prod = ((dig[1] * 1000) + (dig[3] * 100) + (dig[5] * 10) + dig[7]) * 2;
   for ( i=1, pp=1, soma=0; i <= 5; i++ ) {
      soma = soma + ((prod / pp ) % 10);
      pp = pp * 10;
   }

   prod = dig[2] + dig[4] +dig[6] + dig[8];

   if ( ((soma + prod) % 10) != 0 )
      return(-1);
#endif

   /* calculo do primeiro digito verificador */
   mult = "543298765432";

   for ( prod=0, i = 1; i <= 12; i++) 
      prod = prod + (dig[i] * (mult[i-1] - '0'));

   
   if ( (d1 = prod % 11) < 2 ) d1 = 0;
   else                        d1 = 11 - d1;

   if ( d1 != dig[13])
      return(-1);


   /* calculo do segundo digito verificador */
   mult = "6543298765432";
   for ( prod=0, i = 1; i <= 13; i++) 
      prod = prod + (dig[i] * (mult[i-1] - '0'));

   if ( (d2 = prod % 11) < 2 ) d2 = 0;
   else                        d2 = 11 - d2;

   if ( d2 != dig[14])
      return(-1);

   return(0);
}
