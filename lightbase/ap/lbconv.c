#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>

#ifdef MSDOS
#include  <io.h>
#include  <conio.h>
#endif

#if XENIX || PA800R || AIX || SUN
#include <errno.h>  /* Defini variavel errno */
#include <unistd.h>
#endif

#define   SIZEBUF    30000

/** defines **/
#define   TAM_LINHA 1000

FILE *fpi, *fpo;

typedef unsigned char UC;
typedef          char SC;

/* globais */
struct tabela {
   UC in;
   UC *out;
} *tab;

UC mapa_direto[256];
SC furou_direto = 0;
SC so_mostra;

extern int errno;

char *erros[ ] = {
#ifdef ESPANHA
   {"Mensage de error no existe."},
   {"Tabla de conversion no existe."},
   {"No fue posible obtener memoria."}
#else
   "Mensagem de erro inexiste.",
   "Tabela de conversao nao existe.",
   "Nao foi possivel alocar memoria."
#endif
};

int  qtd_linhas;
long linhas;
int percentual, perc_ant;
long total_file;
long conversoes;



/** prototipos **/
#ifdef MSDOS
void le_tab( SC *  );
void erro_fatal( int );
void trata_linha( int , UC * );
void troca_chr( void );
void troca_otima ( unsigned char *);
void imprime( int, unsigned char * );
#else
void le_tab(   );
void erro_fatal(  );
void trata_linha(  );
void troca_chr( );
void troca_otima ( );
void imprime( );
#endif

/** programa **/

void main( argc, argv )
int argc;
char *argv[ ];
{  char c;
   int i;
   unsigned char *buf;
#ifndef MSDOS
   char linha[100];
#endif

   for (i=0; i<256;i++)
      mapa_direto[i] = i;


   if ( argc != 4 ) {
      if ( strcmp(argv[4], "-x") == 0 ) {
         so_mostra = 1;
         goto ok;
      }

#ifdef ESPANHA
      fprintf(stderr, "LBCONV - Conversor de Patrones, (C) SSI - 1991");
      fprintf(stderr, "\nSintaxis : LBCONV tabla fichero nuevo_fichero");
      fprintf(stderr, "\n     tabla         -> contiene los caracteres que seran convertidos.");
      fprintf(stderr, "\n     fichero       -> fichero que sera convertido");
      fprintf(stderr, "\n     nuevo_fichero -> fichero que sera generado con los caracteres convertidos\n\n");
#else
      fprintf(stderr, "LBCONV - Conversor de Padroes, (C) Light Software - 1991");
      fprintf(stderr, "\nUso: LBCONV tabela arquivo novo_arquivo");
      fprintf(stderr, "\n     tabela       -> contem os caracteres a seren convertidos.");
      fprintf(stderr, "\n     arquivo      -> arquivo a ser convertido");
      fprintf(stderr, "\n     novo_arquivo -> arquivo a ser gerado com os caracteres convertidos\n\n");
#endif
      exit( 1 );
   }


ok:

   if ( access(argv[1], 4) != 0 ) {
#ifdef ESPANHA
      fprintf(stderr, "Fichero Tabla no esta accesible\n");
#else
      fprintf(stderr, "Arquivo de tabela nao pode ser lido\n");
#endif
      exit(1);
   }

   if ( access(argv[2], 4) != 0 ) {
#ifdef ESPANHA
      fprintf(stderr, "Fichero de entrada no esta accesible\n");
#else
      fprintf(stderr, "Arquivo de entrada nao pode ser lido\n");
#endif
      exit(1);
   }


   if ( access(argv[3], 0) != -1 ) {
#ifdef ESPANHA
      fprintf(stderr, "Fichero de salida ya existe. Teclee `@` para sobreescribir el fichero : ");
#else
      fprintf(stderr, "Arquivo de saida ja existe. Tecle `@` para sobrepor o arquivo: ");
#endif


#ifdef MSDOS
	c = getch( );
#else
        gets(linha);
        c = linha[0];
#endif

	if ( c != '@' ) {

#ifdef ESPANHA
         fprintf(stderr, "\nOperacion cancelada\n\n");
#else
         fprintf(stderr, "\nOperacao cancelada\n\n");
#endif
         exit(2);
      }
      fprintf(stderr, "\n");
   }


   fpi = fopen(argv[2], "rb");
   errno = 0;


   conversoes = 0L;

   fseek(fpi, 0, 2);
   total_file = ftell(fpi);
   fseek(fpi, 0, 0);

   le_tab( argv[ 1 ] );

   if ( furou_direto == 0 )
      if ( (buf = malloc(SIZEBUF + 2)) != NULL ) {
         fprintf(stderr, "\nLBCONV. Conversao otimizada ativa\n");
         buf[SIZEBUF] = ' ';
         buf[SIZEBUF+1] = ' ';
      }
      else
         furou_direto = 1;

   if ( furou_direto == 0 )
      fpo = fopen(argv[3], "wb");
   else
      fpo = fopen(argv[3], "w");

   percentual = perc_ant = 0;
#ifdef ESPANHA
   fprintf(stderr, "\r%3.3d (%%)  %ld (conversiones)", percentual, conversoes);
#else
   fprintf(stderr, "\r%3.3d (%%)  %ld (conversoes)", percentual, conversoes);
#endif


   if ( furou_direto ) {
      troca_chr( );
   }
   else {
      troca_otima(buf);
   }



#ifdef ESPANHA
   fprintf(stderr, "\r%3.3d (%%)  %ld (conversiones)\n", 100, conversoes);
#else
   fprintf(stderr, "\r%3.3d (%%)  %ld (conversoes)\n", 100, conversoes);
#endif

   fclose(fpi);
   fclose(fpo);
}

void le_tab( nome_tab )
SC *nome_tab;
{
   FILE *fp;
   UC linha[ TAM_LINHA + 1 ];
   int  tam, nlinha;



   if ( (fp = fopen( nome_tab, "r" )) == NULL )
      erro_fatal( 1 );

   qtd_linhas = 0;

   while( fgets( ( SC *) linha, TAM_LINHA, fp ) != NULL )
      qtd_linhas++;

   /** aloca area suficiente para todos os caracteres **/

   tab = ( struct tabela * ) calloc( qtd_linhas+1, sizeof( struct tabela ) );

   if ( tab == NULL )
      erro_fatal( 2 );

   fseek( fp, 0L, SEEK_SET);
   nlinha = 0;
   while( fgets( (SC *)linha, TAM_LINHA, fp ) != NULL )  {
      tam = strlen( (SC *)linha );
      linha[ tam - 1 ] = '\0';
      trata_linha( nlinha++, linha );
   }

}


void trata_linha( nlinha, linha )
int  nlinha;
UC *linha;
{
   UC *aux, t, c;


   furou_direto = 0;

   aux = linha;

   c = tab[ nlinha ].in = *aux;

   aux = (UC *) strchr( (SC *) linha, '|' ) + 1;

   t = strlen( (SC *)aux );

   tab[ nlinha ].out = malloc( t + 1 );
   strcpy((SC * ) tab[ nlinha ].out, (SC *) aux );

   if ( t == 1 )
      mapa_direto[c] = *aux;
   else
      furou_direto = 1;
}



void erro_fatal( n )
int n;
{
   fprintf(stderr, "\n\n<< %s >>", erros[ n ] );
   exit( 1 );
}

void troca_chr ( )
{
   int c, i;

   while ( ( c = fgetc( fpi )) != EOF )
   {
      if ( c == '\r' ) continue;

      for ( i = 0; i < qtd_linhas; i++ ) {
         if ( c == tab[ i ].in ) {
            conversoes++;
            fputs( (SC*)tab[ i ].out, fpo );
            break;
         }
      }

      if ( i == qtd_linhas )
         fputc( c, fpo );

/*
      if ( c == '\n' )
         fprintf(stderr, "\r%ld", ++linhas);
*/
      if ( c == '\n' ) {
         percentual = (int)((ftell(fpi) * 100 ) / total_file );
         if ( percentual != perc_ant ) {
#ifdef ESPANHA
            fprintf(stderr, "\r%3.3d (%%)  %ld (conversiones)", percentual, conversoes);
#else
            fprintf(stderr, "\r%3.3d (%%)  %ld (conversoes)", percentual, conversoes);
#endif
            perc_ant = percentual;
         }
      }

   }
}




void troca_otima ( buf )
unsigned char *buf;
{
   int leu;
   long leia, falta, total_lido;
   register int i;

   falta = total_file;
   total_lido = 0;
   conversoes = 0;




   while ( falta > 0 ) {

      leia = falta;

      if ( leia > SIZEBUF )
         leia = SIZEBUF;

      if ( (leu = fread(buf, 1, (unsigned short)leia, fpi)) != leia ) {
         fprintf(stderr, "\n\nErro na leitura do arquivo");
         exit(1);
      }

      /* so para mostrar palavras que tem acento */
      if ( so_mostra ) {
         for ( i = 0; i < leu; i++ ) {
            if ( buf[i] > 127 )
               imprime(i, buf);
         }
      }
      else {
         for ( i = 0; i < leu; i++ ) {
            if ( mapa_direto[buf[i]] != buf[i] ) {
               buf[i] = mapa_direto[buf[i]];
               conversoes++;
            }
         }

         if ( fwrite(buf, 1, leu, fpo) != leu ) {
            fprintf(stderr, "\n\nErro na gravacao do arquivo");
            exit(1);
         }
      }


      falta -= leu;
      total_lido += leu;

      percentual = (int)((total_lido * 100 ) / total_file );

      if ( percentual != perc_ant ) {
#ifdef ESPANHA
         fprintf(stderr, "\r%3.3d (%%)  %ld (conversiones)", percentual, conversoes);
#else
         fprintf(stderr, "\r%3.3d (%%)  %ld (conversoes)", percentual, conversoes);
#endif
         perc_ant = percentual;
      }
   }
}



void imprime(ind, buf)
int ind;
unsigned char *buf;
{  char palavra[100], nova[100];
   static int conta[256];
   int i, j, c;

   j = ind;

   c = buf[ind];

   if ( conta[c] > 10 )
      return;

   conta[c]++;

   while ( j > 0 && buf[j] != ' ')
      j--;

   i = 0;

   j++;

   while ( j < SIZEBUF && buf[j] != ' ' && i < 99)
      palavra[i++] = buf[j++];

   palavra[i] = '\0';

   strcpy(nova, palavra);

   j = strlen(palavra);

   for ( i = 0; i < j; i++ ) {
      if ( mapa_direto[nova[i]] != nova[i] ) {
         int cc;
         cc = mapa_direto[nova[i]];
         if ( cc != 0 )
            nova[i] = cc;
         else
            nova[i] = '?';
      }
   }

   printf("%s ==== %s\n", palavra, nova);
}

