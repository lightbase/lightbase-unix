/* OK */
#include   <stdio.h>
#include   <time.h>
#include   <stdlib.h>
#include   <string.h>
#include   <errno.h>
#include   <sys/types.h>

char maxusers[20];
char datastr[20];
char cliente[80];
long numserie;

#ifdef XENIX
#include <unistd.h>
#endif

#ifdef MSDOS
char *arquivo = "light.exe";
#else
char *arquivo = "light";
#endif

#ifdef MSDOS
   long  procura( FILE *, char * );
   void  longchar( long, unsigned char *);
   long  charlong( unsigned char *);
#else
   long  procura( );
   void  longchar( );
   long  charlong( );
#endif

main( argc, argv )
int argc;
char **argv;
{
   /*
	procura o string de identificacao "@@@LIGHT@@@" e coloca o numero
	de serie
   */

	FILE *fd;
	FILE *fd_pos;
	char buffer[256];
	char data[128];
	int I;
	long pos, bytes, novonumserie;
	char *ident = "@@@LIGHT@@@";
	time_t dia;
	struct tm *Tm;

	if ( argc > 1 && argv[1][0] != '-' ) {
	   printf("-S Numero de serie\n");
	   printf("-N Numero de Usuarios\n");
	   printf("-D Data Limite\n");
	   printf("-C Nome do cliente\n");
	   exit(0);
	}

	datastr[0] = 0;
	strcpy(maxusers, "???");
	numserie = 0L;

   pega_param(argc, argv);

#ifdef MSDOS
   fd = fopen( arquivo, "rb+" );
#else
   fd = fopen( arquivo, "r+" );
#endif

	if ( ! fd )
	{
		fprintf(stderr, "Arquivo light nao encontrado\n" );
		return -1;
	}

	if( !( fd_pos = fopen( "pos.dat","r" ) ) )
	{
		printf("Arquivo pos.dat nao encontrado.\n" );
		printf("Pressione Enter para Criar ou ^D para Encerrar.\n");
		I = getchar();
		if ( I == 4 )
			exit(0);
regera:
		fd_pos = fopen( "pos.dat","w" );
		fprintf(stderr, "Achar pos ... \n");
		pos = procura( fd, ident );
		if( pos == -1 )
		{
			fprintf(stderr,"Nao achei ...\n");
			fclose( fd_pos );
			fclose( fd );
			return -1;
		}
		fprintf( fd_pos, "%ld\n", pos );
		fclose( fd_pos );
		fprintf(stderr, "Arquivo pos.dat Atualizado.\n");
		fd_pos = fopen( "pos.dat","r" );
	}

	fgets( buffer, 128, fd_pos );
	buffer[ strlen( buffer ) -1 ] = 0;
	pos = atol( buffer );
	fclose( fd_pos );

	fseek( fd, pos-11, 0 );   /* posicionar onde está marca ... */

	/* Confirmar o que está escrito no arquivo ... */

	bytes = fread( buffer, 1, 128, fd);

	if( ( bytes < 11 ) || strncmp( ident, buffer, 11 ) )
	{
		printf("Posicao informada nao contem identificacao.\n" );
		printf("Pressione Enter para recriar posicao ou ^D para Encerrar.\n" );
		I = getchar();
		if ( I == 4 )
			exit(0);
		goto regera;
	}

	/******** serializar ************/

	time( &dia );
	Tm = localtime( &dia );
		fd_pos = fopen( "serial.dat", "r+");
		if( !fd_pos )
		{
		   fprintf(stderr, "Arquivo serial.dat nao abre ... \n" );
		   return -1;
		}

		fgets( buffer, 128, fd_pos );
		buffer[ strlen( buffer ) -1 ] = 0;
		novonumserie = atol( buffer ) +1;

		if ( numserie == 0 || numserie == novonumserie ) {
		   numserie=novonumserie;

			if ( !cliente[0] ) {
				printf("\nNome do Destinatario da copia:\n");
				gets( cliente );
			}
		}
		else {
		   strcpy(cliente, "Regeracao");
		}

		sprintf( data, "%02d/%02d/%02d", Tm->tm_mday, Tm->tm_mon+1, Tm->tm_year );
		fseek( fd_pos, 0L, 2 );
		fprintf( fd_pos, "%06ld(%s) - %s - %s [%s]\n", numserie, maxusers, data, cliente, datastr );
		if ( novonumserie == numserie) {
			fseek( fd_pos, 0L, 0 );
			fprintf( fd_pos, "%06ld", numserie );
		}
		fclose( fd_pos );

	fclose( fd );
	fd = fopen( arquivo, "r+" );
	fseek( fd, pos , 0);

	I = fprintf( fd, "%06ld%02d%02d%02d ", numserie, Tm->tm_mday, Tm->tm_mon+1, Tm->tm_year );

	if( I < 0 )
	{
	   fprintf(stderr, "ERRO na serializacao.\n" );
	   fclose( fd );
	   return -1;
	}

	longchar( (long)dia-43200L, (unsigned char *)data );
	fwrite( data, 4, 1, fd );   /* valor de time ... */

	fclose( fd );

	fprintf(stderr, "Serializacao completa. Numero: %06ld.\n", numserie );
	printf("%06ld\n", numserie);

	return 0;
}


pega_param(argc, argv)
int argc;
char **argv;
{  int i, ret = 0;

	for( i = 1;   i < argc;   i++ ) {

      if ( argv[i][0] != '-' )
         continue;

      else if ( toupper(argv[i][1]) ==  'S' ) { /* Serie */
         numserie = atol(&argv[i][2]);
      }
      else if ( toupper(argv[i][1]) ==  'N' ) { /* Num usuarios */
         int n;
         char num[10];
         n = atoi(&argv[i][2]);
         if ( n > 0 && n < 1000 ) {
            n = n+1000;
	         sprintf(num,"%d", n);
	         strcpy(maxusers, &num[1]);
	      }
      }

      else if ( toupper(argv[i][1]) ==  'D' ) {  /* data */
         if ( strlen(&argv[i][2]) == 8 ) {
	         strcpy(datastr,&argv[i][2]);
         }
      }
      else if ( toupper(argv[i][1]) ==  'C' ) {  /* cliente */
         strcpy(cliente,&argv[i][2]);
      }
      else if ( toupper(argv[i][1]) ==  'A' ) {  /* arquivo */
         pega_dados(&argv[i][2]);
         ret = 1;
      }
      else if ( toupper(argv[i][1]) ==  'P' ) {  /* Proximo numero */
			FILE *fd_pos;
			char buffer[100];
			fd_pos = fopen( "serial.dat", "r");
			if( !fd_pos )
			{
		   	fprintf(stderr, "Arquivo serial.dat nao abre ... \n" );
		   	return -1;
			}

			fgets( buffer, 128, fd_pos );
			buffer[ strlen( buffer ) -1 ] = 0;
			numserie = atol( buffer ) +1;
			fclose(fd_pos);

			printf("%06.6ld\n", numserie);
			exit(0);
      }
   }

   return(ret);
}




pega_dados(arq)
char *arq;
{  int i, t;
	char linha[110];
	FILE *fp;

	fp = fopen(arq, "r");

	if ( fp == NULL )
	 	return;


	while ( fgets(linha, 100, fp) != NULL ) {


      t = strlen(linha);
      if ( linha[t-1] == '\n' || linha[t-1] == '\r' )
         linha[t-1] = '\0';


      if ( linha[0] != '-' )
         continue;
      else if ( toupper(linha[1]) ==  'S' ) { /*serie */
         numserie=atol(&linha[2]);
      }
      else if ( toupper(linha[1]) ==  'N' ) { /* Num usuarios */
         int n;
         char num[10];
         n = atoi(&linha[2]);
         if ( n > 0 && n < 1000 ) {
            n = n+1000;
	         sprintf(num,"%d", n);
	         strcpy(maxusers, &num[1]);
	      }
      }

      else if ( toupper(linha[1]) ==  'D' ) {  /* data */
         if ( strlen(&linha[2]) == 8 ) {
	         strcpy(datastr,&linha[2]);
         }
      }
      else if ( toupper(linha[1]) ==  'C' ) {  /* cliente */
         strcpy(cliente,&linha[2]);
      }
   }

   fclose(fp);
}


long procura( fd, ident)
FILE *fd;
char *ident;
{
	/* procura "cadeia" dentro do executável aberto como "rb" descrito em fd */

	char buffer[256];
	int I, K, bytes;
	long pos;

	fseek( fd, 0L, 0);
	I = 0; pos = 0L;
	while( (bytes = fread( buffer, 1, 256, fd)) > 0 )
	for ( K=0; K<bytes; ++K )
	{
		pos++;


		if( ident[ I ] )
		{
			if( ident[ I ] == buffer[ K ] ) ++I;
			else                            I = 0;
		}
 		else {
         if ( I > (256 - 17) ) /* DATA_SERIAL */
 				fprintf(stderr, "Atencao Limite de buffer!\n");
 			return (pos-1);
 		}
	}

	return -1;

}

void longchar( l, s )
long l;
unsigned char *s;
{
   /* converte um long em uma sequencia de caracteres low-high */

   int I;

   for( I=0; I<4; ++I )
   {
      s[I] = l%256;
      l = l/256;
   }
/* printf( "\n%02x %02x %02x %02x\n", s[0], s[1], s[2], s[3] ); */
   return;
}

long charlong( s )
unsigned char *s;
{
   int I;
   long l;

   l = 0;
   for( I=4; I>0; --I ) l = l*256+s[I-1];

   return l;
}

