#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#define   VOID  void
#define   COUNT short
#define   UTEXT unsigned char
#define   UCOUNT unsigned short
#define   SI int
#define   TEXT char
#define   LONG long
#define   ULONG unsigned long


SI lb_random(COUNT);

#define   TESTE

VOID main( int, UTEXT ** );
VOID cryptum( LONG, LONG );
VOID encrypt (UTEXT *, UTEXT *);


VOID main ( argc, argv )
COUNT argc;
UTEXT **argv;
{  UCOUNT ok;
   LONG num;
   TEXT aux[100];

   printf("\n\n\n\n");
   printf("Copyright 1991 Light Software LTDA\n");
   printf("Copyright 1995 - Light-Infocon\n\n");
   printf("Copyright 1996 - Light-Infocon S/A\n\n");
   printf("Gerador de senhas para FORNECEDORES de BASES\n");

   while ( 1 ) {
      printf("\n\nInforme a matricula do FORNECEDOR : ");
      gets(aux);

      num = atol(aux);
      if ( num > 200 && num < 65000L)
         break;

      putchar(7);
      printf("Matricula de FORNECEDOR deve estar entre 200 e 65000\n");
   }

   cryptum(num, num);

   return;
}


SI lb_random(max)
COUNT max;
{
   return (int)(((long)rand()*max)/0x7FFF);
}



VOID cryptum( qual, ate )
LONG qual, ate;
{
   LONG x;
   UTEXT buf[32], senha[32];

   for (x = qual; x <= ate; x++) {
      sprintf( (TEXT*)buf, "%06ld", x );
      printf( "%s  -  ", buf );
      encrypt( buf, senha );
      printf( "%s\n", senha );
   }
}

VOID  encrypt( UTEXT *, UTEXT * );
COUNT primeira_fase( UTEXT * );
VOID  mistura( UTEXT *, UTEXT * );
COUNT dobra_semente( UTEXT *, UTEXT * );
COUNT anterior( int, COUNT );
COUNT posterior( int, COUNT );
COUNT somatoria( UTEXT * );

COUNT permissao( semente, senha )
char *semente, *senha;
{
   char buffer[32];

   encrypt( (UTEXT*)semente, (UTEXT*)buffer );
   return( !strcmp( senha, buffer ) );
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
   dadireita = posterior( daesquerda, ultimo );
   for (subscrito = 0; subscrito <= ultimo; subscrito++ )
      if (subscrito & 0x01) {        /* casas impares */
         buffer[subscrito] = semente[dadireita];
         dadireita = posterior( dadireita, ultimo );
      }
      else {                         /* casas pares */
         buffer[subscrito] = semente[daesquerda];
         daesquerda = anterior( daesquerda, ultimo );
      }
   buffer[subscrito] = '\0';
   return soma;
}

COUNT anterior( elem, lim )
COUNT elem, lim;
{
   return (elem == 0) ? lim : --elem;
}

COUNT posterior( elem, lim )
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
