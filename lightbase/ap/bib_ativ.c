#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#define   VOID  void
#define   COUNT short
#define   UTEXT unsigned char
#define   TEXT char
#define   SI int
#define   LONG long
#define   ULONG unsigned long

#define   TESTE

#ifdef MSDOS
   VOID cryptum( LONG, LONG);
   VOID encrypt (UTEXT *, UTEXT *);
   SI lb_random(COUNT);
   VOID ficha_BR(TEXT *, TEXT *);
   VOID ficha_ESP(TEXT *, TEXT *);
   COUNT primeira_fase( UTEXT * );
   VOID  mistura( UTEXT *, UTEXT * );
   COUNT dobra_semente( UTEXT *, UTEXT * );
   COUNT anterior( int, COUNT );
   COUNT posterior( int, COUNT );
   COUNT somatoria( UTEXT * );
#else
   VOID cryptum( );
   VOID encrypt ( );
   SI lb_random( );
   VOID ficha_BR( );
   VOID ficha_ESP( );
   COUNT primeira_fase( );
   VOID  mistura( );
   COUNT dobra_semente( );
   COUNT anterior( );
   COUNT posterior( );
   COUNT somatoria( );
#endif


int tipo_saida;

main ( argc, argv )
COUNT argc;
TEXT **argv;
{	char aux[30], *c;
	long inic, final;

   argc=argc;

	if ( strcmp(argv[1], "-l") == 0 )
	   tipo_saida = 'l';

	c = getenv("LBCHAVE");

	if ( c == NULL || strcmp(c, "simple") != 0 ) {
	   fprintf(stderr, "Sem autorizacao de execucao\n");
	   exit(1);
	}

	fprintf(stderr, "Numero Inicial.. : ");
	gets(aux);
	inic = atol(aux);

	fprintf(stderr, "Numero Final.... : ");
	gets(aux);
	final = atol(aux);
	if ( final == 0 )
	   final = inic;

   if ( inic > 1000 )
      cryptum(inic, final);

   return 0;
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
   COUNT n=0;
   TEXT numero[32];
   UTEXT aux[32], senha[32];

   for (x = qual; x <= ate; x++) {
      sprintf( numero, "%06ld", x );
      strcpy( (TEXT *)aux, numero);  /* aux vai ser melada por encrupt */
      encrypt( aux, senha );

      strupr((TEXT *)senha);

      if ( tipo_saida == 'l' ) {
         printf("%s - %s\t", numero, senha);
         if ( ++n == 3 ) {
            printf("\n");
            n = 0;
         }
      }
      else {
#ifdef ESPANHA
      	ficha_ESP(numero, (TEXT *)senha);
#else
      	ficha_BR(numero, (TEXT *)senha);
#endif
      }
   }

   printf("\n");

}


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



VOID ficha_BR(numero, senha)
TEXT *numero, *senha;
{  static TEXT nome[30];
   TEXT lixo[30];
   FILE *fp;


   if ( nome[0] == 0 ) {
      printf("Nome do Arquivo a ser gerado ? ");
      gets(nome);
   }

   printf("\n\nTecle <ENTER> para gerar %s para numero %s", nome, numero);
   gets(lixo);

   fp = fopen(nome, "w");

   if ( fp == NULL ) {
      printf("Nao pode abrir %s\n", nome);
      fp = stderr;
   }

   fprintf(fp, "%s\n", numero);
   fprintf(fp, "%s\n", senha);
   fprintf(fp, "\n");
   fprintf(fp, "mbnbiblinfo\n");
   fprintf(fp, "s000210\n");
   fprintf(fp, "ÅÅ\n");

   if ( fp != stderr ) {
      fclose(fp);
   }

   printf("Ok.\n");

   return;
}


VOID ficha_ESP(numero, senha)
TEXT *numero, *senha;
{

   printf("*--------------------------------------------------* \n");
   printf("|                   ATENCAO                        | \n");
   printf("|                                                  | \n");
   printf("|                                                  | \n");
   printf("|                                                  | \n");
   printf("| LOS CODIGOS ABAJOS SON RESPECTIVAMENTE EL NUMERO | \n");
   printf("|                                                  | \n");
   printf("| DE SERIE Y LA LLAVE DE ACTIVACION DE ESTA COPIA  | \n");
   printf("|                                                  | \n");
   printf("| DEL LIGHT BASE. ELLOS SERAN NECESARIOS PARA LA   | \n");
   printf("|                                                  | \n");
   printf("| PRIMERA INSTALACION DE ESTE PRODUCTO. GUARDE     | \n");
   printf("|                                                  | \n");
   printf("| ESTOS NUMEROS EN LUGAR SEGURO PUES ELLOS SON LA  | \n");
   printf("|                                                  | \n");
   printf("| LLAVE MAESTRA DE UTILIZACION DE ESTA COPIA DEL   | \n");
   printf("|                                                  | \n");
   printf("| LIGHT BASE.                                      | \n");
   printf("|                                                  | \n");
   printf("|                                                  | \n");
   printf("|     Numero de serie ... :   %s               | \n", numero);
   printf("|                                                  | \n");
   printf("|     Llave de activacion : %s               | \n", senha);
   printf("|                                                  | \n");
   printf("*--------------------------------------------------* \n");

   return;
}

#ifndef MSDOS
strupr(p)
char *p;
{
	while (*p != '\0') {
	   *p = toupper(*p);
	   p++;
	}
}
#endif
