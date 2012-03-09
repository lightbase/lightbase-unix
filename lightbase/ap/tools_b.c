#include  "lb2all.h"

#include <string.h>
#include  <time.h>
#include  <ctype.h>

#include  "lb2stru.h"
#include  "lb2files.h"
#include  "lb2bases.h"

#include "tools.h"


#ifdef MSDOS
   VOID cryptum( LONG, LONG);
   VOID encrypt (UTEXT *, UTEXT *);
   SI lb_random(COUNT);
   COUNT primeira_fase( UTEXT * );
   VOID  mistura( UTEXT *, UTEXT * );
   COUNT dobra_semente( UTEXT *, UTEXT * );
   COUNT el_anterior( int, COUNT );
   COUNT el_posterior( int, COUNT );
   COUNT somatoria( UTEXT * );
#else
   COUNT permissao( );
   VOID cryptum( );
   VOID encrypt ( );
   SI   lb_random( );
   VOID ficha_BR( );
   VOID ficha_ESP( );
   COUNT primeira_fase( );
   VOID  mistura( );
   COUNT dobra_semente( );
   COUNT el_anterior( );
   COUNT el_posterior( );
   COUNT somatoria( );
#endif




#ifdef MSDOS
#define   GET_LONG(pl)     (LONG)(*(LONG *)pl)
#define   PUT_LONG(p, l)   memcpy((TEXT *) p, (TEXT *) &l, sizeof(LONG))
#define   GET_LONG4(pl)    (LONG)(*(LONG *)pl)
#define   PUT_LONG4(p, l)  memcpy((TEXT *) p, (TEXT *) &l, sizeof(LONG))
#endif



/* Funcoes para padronizar gravacao/leitura de LONG na memoria */
#ifndef PUT_LONG
F__GLB   VOID PUT_LONG(p, l)
UTEXT *p;
LONG l;
{  UTEXT *pc;
   COUNT i;

   pc = (UTEXT*)&l;

   for ( i = 0; i < sizeof(LONG); i++) {
      *p++ = *pc++;
   }
}
#endif



#ifndef GET_LONG
F__GLB   LONG GET_LONG(p)
UTEXT *p;
{  LONG l;
   UTEXT *pc;
   COUNT i;

   pc = (UTEXT*)&l;

   for ( i = 0; i < sizeof(LONG); i++) {
      *pc++ = *p++;
   }
   return(l);
}
#endif


/* Funcoes para padronizar gravacao/leitura de LONG na memoria */
#ifndef PUT_LONG4
F__GLB   VOID PUT_LONG4(p, l)
UTEXT *p;
LONG l;
{  UTEXT *pc;
   COUNT i;

   pc = (UTEXT*)&l;

   for ( i = 0; i < 4; i++) {
      *p++ = *pc++;
   }
}
#endif



#ifndef GET_LONG4
F__GLB   LONG GET_LONG4(p)
UTEXT *p;
{  LONG l;
   UTEXT *pc;
   COUNT i;

   pc = (UTEXT*)&l;

   for ( i = 0; i < 4; i++) {
      *pc++ = *p++;
   }
   return(l);
}
#endif



F__GLB POINTER get_postexto(fonte, p)
COUNT fonte;
UTEXT *p;
{  POINTER ret_pointer;

	ret_pointer = 0;

	switch (fonte) {
	   case 'N' : /* posicao em newdata long normal */
	   			  ret_pointer = GET_LONG(p);
	   			  break;
	   case 'O' : /* posicao em olddata long truncado (4 bytes iniciais) */
	   			  ret_pointer = GET_LONG4(p);
	   			  break;

	   case 'L' : /* posicao em forma de ponteiro para long */
	   			  ret_pointer = *(POINTER *)p;
                 if ( sizeof(POINTER) > 4 )
                    debug("WWW Atencao tratamento de POINTER com tamanho maior que 4");
	   			  break;

		default  : debug("Forma de Long nao tratada");
		           break;
	}

	if ( ret_pointer < 0 ||  ret_pointer > 0xFFFFFFFF )
	   debug("Posicao de texto provavelmente invalida : %ld", ret_pointer);

	return((LONG)ret_pointer);
}


F__GLB VOID put_postexto(fonte, p, pos)
COUNT fonte;
UTEXT *p;
LONG pos;
{

	switch (fonte) {
	   case 'N' : /* posicao em newdata long normal */
	   			  PUT_LONG(p, pos);
	   			  break;
	   case 'O' : /* posicao em olddata long truncado (4 bytes iniciais) */
	   			  PUT_LONG4(p, pos);
	   			  break;
		default  : debug("Forma de Long nao tratada");
		           break;
	}

}





LONG pega_proximo(pos, fp)
LONG pos;
FILE *fp;
{  COUNT c;
   LONG ret;

 	fseek( fp, pos, 0 );

   while ( 1 ) {

      if ( (c = fgetc(fp)) == EOF ) {
         ret = -1;
         goto fim;
      }

      pos++;

      if ( c == 0xFA ) {
         if ( (c = fgetc(fp)) == EOF ) {
            ret = -1;
            goto fim;
         }
         pos++;
         if ( c == 0xFA ) {
            ret = ftell(fp) - 2;
            break;
         }
      }
   }

fim :

   return(ret);
}



VOID strip_cr(str)
TEXT *str;
{  COUNT i;


   while ( (i=strlen(str)) > 0 ) {
      if ( str[i-1] == '\n' || str[i-1] == '\r') {
         str[i-1] = '\0';
         continue;
      }
      else
         break;
   }
}


VOID strip_br(linha)
TEXT *linha;
{  COUNT tam;
   TEXT *p;

   tam = strlen(linha);

   p = &linha[tam - 1];

   if ( *p == '\n' )
      *p-- = '\0';

   while ( *p == ' ' && p >= linha ) {
      p--;
   }

   if ( *p != '\0' )
      p++;

    *p = '\0';
}




/*************************************************************************/
/*************************************************************************/
/***********      ELABORADA PELO WAGNER                    ***************/
/*************************************************************************/
/*************************************************************************/


F__LOC   COUNT permissao( sem, senha )
TEXT *sem, *senha;
{
   TEXT buffer[32];
   TEXT semente[32];


   sprintf(semente, "%06ld", atol(sem));

   encrypt( (UTEXT *)semente, (UTEXT *)buffer );

   return( (COUNT)!stricmp( senha, buffer ) );
}



VOID encrypt (semente, senha )
UTEXT *semente;
UTEXT *senha;
{
   COUNT  ptr, sigma, tamanho;
   UTEXT valor;

   if (*semente == '\0') {                       /* handle a null string    */
     *senha = '\0';
     return;
   }
   sigma = primeira_fase( semente );
   srand( (semente[0] + sigma) * sigma );
   valor = lb_random( (semente[0] + sigma) * 'a' );
   senha[0] = (valor % ('z' - 'a' + 1)) + 'a';
   ptr = 0;
   while ((semente[ptr] != '\0') && (ptr < 8)) {
     valor = ((semente[ptr] + senha[ptr]) << 1) ^ 0x55;
     senha[++ptr] = (valor % ('z' - 'a' + 1)) + 'a';
   }
   while (ptr < 8) {
      if (ptr == 1)
         senha[ptr] = ((senha[0] + 12) % ('z' - 'a' + 1)) + 'a';
      else
         senha[ptr] = (((senha[ptr-1] + senha[ptr-2]) + 12) % ('z' - 'a' + 1)) + 'a';
      ptr++;
   }
   tamanho = strlen( (TEXT*)semente );
   senha[2] = ((senha[2] + semente[tamanho - 1]) % ('z' - 'a' + 1)) + 'a';
   senha[3] = ((senha[3] + semente[tamanho - 2]) % ('z' - 'a' + 1)) + 'a';
   senha[4] = ((senha[4] + semente[tamanho - 4]) % ('z' - 'a' + 1)) + 'a';
   senha[5] = ((senha[5] + semente[tamanho - 3]) % ('z' - 'a' + 1)) + 'a';
   senha[ptr] = '\0';
#if 0
   for (ptr = 0; ptr < 8; ptr++)               /* insure no :'s in cipher */
      if (senha[ptr] == ':') senha[ptr] = 'X';
#endif
}


COUNT primeira_fase( semente )
UTEXT *semente;
{
   UTEXT buffer[32];
   COUNT somatoria;

   somatoria = dobra_semente( semente, buffer );
   mistura( semente, buffer );
   return somatoria;
}

VOID mistura( semente, buffer )
UTEXT *semente;
UTEXT *buffer;
{
   while (*semente) {
      *semente += *buffer - '0';
      if (*semente > '9')
         *semente -= 10;
      ++semente;
      ++buffer;
   }
}

COUNT dobra_semente( semente, buffer )
UTEXT *semente;
UTEXT *buffer;
{
   COUNT subscrito, soma;
   COUNT dadireita, daesquerda, ultimo;

   ultimo = strlen( (TEXT*)semente ) - 1;
   soma = somatoria( semente );
   daesquerda = (ultimo) % soma;
   dadireita = el_posterior( daesquerda, ultimo );
   for (subscrito = 0; subscrito <= ultimo; subscrito++ )
      if (subscrito & 0x01) {        /* casas impares */
         buffer[subscrito] = semente[dadireita];
         dadireita = el_posterior( dadireita, ultimo );
      }
      else {                         /* casas pares */
         buffer[subscrito] = semente[daesquerda];
         daesquerda = el_anterior( daesquerda, ultimo );
      }
   buffer[subscrito] = '\0';
   return soma;
}

COUNT el_anterior( elem, lim )
COUNT elem, lim;
{
   return (elem == 0) ? lim : --elem;
}

COUNT el_posterior( elem, lim )
COUNT elem, lim;
{
   return (elem == lim) ? 0 : ++elem;
}

COUNT somatoria ( semente )
UTEXT *semente;
{
   COUNT soma = 0;

   while (*semente)
      soma += *semente++;
   return soma;
}

SI lb_random(max)
COUNT max;
{
   return (int)(((long)rand()*max)/0x7FFF);
}




VOID pega_long(pc, po)
UTEXT *pc;
UTEXT *po;
{	COUNT i;
#ifdef LOW_HIGH
   	pc += sizeof(POINTER);
   	for (i = 0; i < sizeof(POINTER); i++ )
   		*--pc = *po++;
#else
      for (i = 0; i < sizeof(POINTER); i++ )
   	   	*pc++ = *po++;
#endif
}


/*--------------------------------------------------------------------*/
SI  w_strnicmp( s1, s2, max)
SC  *s1, *s2;
SS  max;
{
   SI   ret;

   ret = toupper( *s1 ) - toupper( *s2 );

   for (   ;   *s1  &&  *s2  &&  ret == 0 &&  max > 0;   s1++, s2++, max-- )
   {
      ret = toupper( *s1 ) - toupper( *s2 );
   }

   return( ret );
}



#ifdef XX_STR_PROPRIA
/*--------------------------------------------------------------------*/
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
