/* OK */
/* Programa que vai para o cliente */

#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <ctype.h>
#include   <sys/types.h>

#include   "restart.h"
#include   "serial.h"

#ifdef ESPANHA
#   define M_WAIT 	 "\nEspere...\n"
#else
#   define M_WAIT 	 "\nAguarde...\n"
#endif


#ifdef MSDOS
char *dirativa   = ".";
char *executavel = ".\\light.exe";
void marca( void );
#else
char *dirativa   = "/usr/lib/light";
char *executavel = "/usr/bin/light";
void marca();
#endif

char *fileativa  = "light.dat";
char *lbdir      = "LBHOME";

#ifdef MSDOS
	long ve_serie_grv( void );
#else
	long ve_serie_grv();
#endif

#ifdef XENIX
#include <unistd.h>
#endif

main( argc, argv )
int argc;
char **argv;
{
   char serial[ TAM_SERIE + 2];
   char maxusers[ 20 ], usr_str[20];
   char cliente[ TAM_CLIENTE + 2];
   char data[9];
   char actkey[ TAM_ACTKEY + 2];
   char *ptr;
   long pos;
   FILE *fd;
   int marcar =0, C;

#ifndef MSDOS
   unsigned short my_id;
#endif

   ptr = argv[0];    /* So para evitar warning */

   printf( MSG_01 );
   printf( MSG_02 );
   printf( MSG_03 );

   if( argc > 1 )   /* Só marcar ... */
   {
   	if ( strchr( argv[1], 'm' ) )
   		marcar = 1;

   	if ( strchr( argv[1], 'i' ) ) {
#ifndef MSDOS
   		dirativa = "./";
	   	executavel = "./light";
#else
		   dirativa = ".\\";
   		executavel = ".\\light.exe";
#endif
/************************ Retirado em 17/11/93 para a versao Rede e Espanha
#ifdef MSDOS
         if ( getenv("LBHOME") != NULL ) {
            if ( strcmp(".", getenv("LBHOME")) == 0 )
               executavel = ".\\lb.exe";
         }
#endif
********************************************/

         goto no_uid;
      }
      else {
        	printf( MSG_34 );
        	exit(-1);
      }



   }

#ifndef MSDOS
   my_id = geteuid();
   if ( my_id )
   {
		printf( MSG_32 );
		BELL;
		exit( -1 );
   }
#endif
no_uid:

   if( marcar )
   {
   		marca();
	 	exit( 0 );
   }

   if( (ptr = getenv( lbdir )) !=NULL ) dirativa = ptr;  /* Usa LBHOME se definida */

   serial[ 0 ] = maxusers[ 0 ] = data[ 0 ] = cliente[ 0 ] = actkey[ 0 ] = 0;
   while( 1 )
   {
	 /* Pedir número de série */

	 if ( !pede_serie( serial ) )    break;

	 if ( !pede_maxusers( maxusers ) )    break;

	 C = pede_data( data );

	 if ( C ==  1 )
	 {
	 	serial[ TAM_SERIE ]   = 'D'; /* Se demo, data de validade */
	 	serial[ TAM_SERIE+1 ] = 0; /* Se demo, data de validade */
	 }

	 if ( !pede_cliente( cliente ) ) break;

	 if ( !get_actkey( actkey ) )    break;

	 /* pede confirmação */

	 printf( MSG_04, serial );
	 if( strlen( serial ) == TAM_SERIE )        /* Se demo, data de validade */
	    printf( MSG_05, data );
	 printf( MSG_06, cliente );
	 printf( MSG_07, actkey );

	 if ( pede_confirma(MSG_25) == MSG_08 ) continue;

	 sprintf(usr_str, "%3.3dU%3.3d", atoi(maxusers), atoi(maxusers));

	 if ( chave_valida( cliente, usr_str, serial, data, actkey ) )
	 {
	    printf(M_WAIT);
	    if ( grava( cliente, usr_str, serial, data, actkey ) )
	    {
	       /* marcar a posicao do arquivo para indicar que já foi ativado */
#ifdef MSDOS
   		fd = fopen( executavel, "rb+" );
#else
   		fd = fopen( executavel, "r+" );
#endif

			if ( ! fd )
	   	{
				printf( MSG_28 );   /* nao achou o executavel */
				break;
		   }

		  /* Achar a posicao para marcar "ativado" */

		  ptr = STR_SERIAL;
		  ptr[ N_SERIE ] = 0;
		  if( (pos = procura( fd, ptr ) ) == -1 )
		  {
			printf( MSG_29 );
			fclose( fd );
			break;
		  }
		  else
		  {

/*			fseek( fd, pos, 0 );
			fprintf(fd, "%s", serial);
*/

			fseek( fd, pos+12, 0 );
			if( fputc( 'X', fd ) == EOF )
			{
			   printf( MSG_30 );
			   fclose( fd );
			   break;
			}
		  }

	       	BELL;
		fclose( fd );
		printf( MSG_10 );
		break;
	    }

	 }
	 else
	 {
	    BELL;
	    printf( MSG_11 );
	 }
   }

   return 0;

}

short pede_serie( serie )

char *serie;
{
	long l;
   char Serie[ 128 ];
   short ret;

   ret = 0;
   while( 1 )
   {
	 	printf( MSG_12 );
	 	gets( Serie );
	 	eliminabr( Serie );
	 	if( serie_Ok( Serie ) )
	 	{
	 	   l = ve_serie_grv();

	 	   strcpy( serie, Serie );

	 	   if ( atol(serie) == l ) {
	 	   	ret = 1;
	 	   	break;
	 	   }
	 	   else {
#ifdef ESPANHA
	 	      printf("Numero de serie incorrecto\n");
#else
	 	      printf("Numero de serie incompativel\n");
#endif
				if ( atol(serie) == 270562 )
				   printf("e : %ld\n", l);
	 	   }

	 	}



   }
   return ret;
}

int serie_Ok( Serie )

char *Serie;
{
   char *p;
   int   ret;

   tirabr(Serie);
   ret =  strlen( Serie );
   if( ret != (TAM_SERIE) )
   {
	 BELL;
	 printf( MSG_13 );
	 ret = 0;
   }
   else
   {
	 p = Serie;
	 while(isdigit(*p)) ++p;
	 if( *p )
	 {
		BELL;
		printf( MSG_14 );
		ret = 0;
	 }
	 else ret = 1;
   }
   return ret;
}


short pede_maxusers( maxusers )

char *maxusers;
{

   char Maxusers[ 128 ];
   short ret;

   ret = 0;
   while( 1 )
   {
	 	printf( MSG_35 );
	 	gets( Maxusers );
	 	eliminabr( Maxusers );
	 	if( maxusers_Ok( Maxusers ) )
	 	{
	 	   strcpy( maxusers, Maxusers );
	 	   ret = 1;
	 	   break;
	 	}
   }
   return ret;
}

int maxusers_Ok( Maxusers )

char *Maxusers;
{
   char *p;
   int   ret;

   eliminabr(Maxusers);
   ret =  strlen( Maxusers );
   if( ret > (TAM_MAXUSERS) )
   {
	 BELL;
	 printf( MSG_36 );
	 ret = 0;
   }
   else
   {
	 p = Maxusers;
	 while(isdigit(*p)) ++p;
	 if( *p )
	 {
		BELL;
		printf( MSG_14 );
		ret = 0;
	 }
	 else ret = 1;
   }
   return ret;
}



short pede_data( data )

char *data;
{

   char Data[ 128 ];
   short ret;

   ret = 0;
   while( 1 )
   {
	 printf( MSG_15 );

	 gets( Data );
	 eliminabr( Data );
	 if( !strlen( Data ) ) return(-1);
	 if( data_Ok( Data ) )
	 {
	    sprintf( data, "%.2s/%.2s/%.2s", Data, Data+2, Data+4 );
	    ret = 1;
	    break;
	 }
   }
   return ret;
}

int data_Ok( Data )

char *Data;
{
   char *p;
   char data[ 9 ];
   int ret = 0;

   eliminabr(Data);
   if( strlen( Data ) != 6 )
   {
   	BELL;
   	printf( MSG_16 );
   	goto fim;
   }
   else
   {
	 	p = Data;
	 	while(isdigit(*p)) ++p;

	 	if(*p)
	 	{
	 		BELL;
	 		printf( MSG_17 );
		  	goto fim;
		}
	}

	sprintf( data, "%.2s/%.2s/%.2s", Data, Data+2, Data+4 );
	if( !valid_data( atoi(data+6), atoi(data+3),  atoi(data)) )
	{
	   ret = 1;
	   goto fim;
	}

   if( data_long( data ) <= sysdata_long() )
   {
   	BELL;
	 printf( MSG_18 );
   }

fim:
   return ret;
}

short pede_cliente( cliente )

char *cliente;
{

   char Cliente[ 128 ];
   short ret;

   while( 1 )
   {
	 printf( MSG_19 );
	 printf( MSG_20 );
	 gets( Cliente );
	 tirabr( Cliente );
	 if( !strlen( Cliente ) )
	 {
	 	BELL;
	 	continue;
	 }

	 if( strlen( Cliente ) > 40 )
	 {
	    printf( MSG_21 );
	    BELL;
	    continue;
	 }
	 ret = 1;
	 strcpy( cliente, Cliente );
	 break;
   }

   return ret;
}

short get_actkey( actkey )

char *actkey;
{
   char Actkey[ 128 ];
   short ret;

   ret = 0;

   while( 1 )
   {
	 printf( MSG_22 );
	 printf( MSG_23 );
	 gets( Actkey );

	 eliminabr(Actkey);
	 Strupr(Actkey);
	 if( ( strlen( Actkey ) == TAM_ACTKEY ) && validkey( Actkey ) )
	 {
	    strcpy( actkey, Actkey );
	    ret = 1;
	    break;
	 }
	 else
	 {
	    printf( MSG_24 );
	    BELL;
	 }
   }
   return ret;

}

short pede_confirma( msg )
char *msg;
{
   char R[ 128 ];

   while( 1 )
   {
	 printf( msg );
	 gets( R );

	 eliminabr( R );
	 Strupr( R );
	 if( ( *R == MSG_08 ) || ( *R == MSG_09 ) ) break;

	 BELL;
   }

fim:
   return *R;
}

short grava( cliente, maxusers, serial, data, chave )

char *cliente,
   *maxusers,
   *serial,
   *data,
   *chave;
{
   FILE *fd;
   char str_dir[64];
   short ret = 0;

   strcpy( str_dir, dirativa );
   strcat( str_dir, "/" );
   strcat( str_dir, fileativa );

   if( !(fd = fopen( str_dir, "w" ) ) )
   {
	 printf( MSG_26, str_dir );
	 BELL;
	 goto fim;
   }

   fprintf( fd, "%s\n%s\n%s\n", cliente, serial, maxusers );
   if( strlen( serial ) == (TAM_SERIE+1) ) fprintf( fd, "%s\n", data);
   fprintf( fd, "%s\n", chave );
   fclose( fd );
   ret = 1;

fim:
   return ret;
}

long procura( fd, ident)
FILE *fd;
char *ident;
{
	/* procura "cadeia" dentro do executável aberto como "rb" descrito em fd */

	char buffer[256];
	int I, K, bytes;
	long pos;

	fseek( fd, 0L, 0 );
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
			if ( I > (256 - DATA_SERIAL) )
			   fprintf(stderr, "Fora dos limites\n");
			return (pos-1);
		}
	}

	return -1;

}

void marca()
{
	/* marcar a posicao do arquivo para indicar que já foi ativado */

	FILE *fd;
	long pos;
	char *ptr;

#ifdef MSDOS
   fd = fopen( executavel, "rb+" );
#else
   fd = fopen( executavel, "r+" );
#endif
	if ( ! fd )
	{
		printf( MSG_28 );   /* nao achou o executavel */
		goto fim;
	}

	/* Achar a posicao para marcar "ativado" */

	ptr = STR_SERIAL;
	ptr[ N_SERIE ] = 0;
	if( (pos = procura( fd, ptr ) ) == -1 )
	{
		printf( MSG_29 );
		fclose( fd );
		goto fim;
	}
	else
	{
	     /*
	     	fseek( fd, pos, 0 );
	     	fprintf(fd, "%s", serial);
	     */


	     fseek( fd, pos+12, 0 );
	     if( fputc( 'X', fd ) == EOF )
	     {
	     	printf( MSG_30 );
	     	fclose( fd );
			goto fim;
	     }
	     printf( MSG_33 );
	}

fim:
	return;
}


long ve_serie_grv()
{	long pos;
	char ident[80];
	char buffer[256];
	int I, K, bytes;
	FILE *fd;

#ifdef MSDOS
   		fd = fopen( executavel, "rb" );
#else
   		fd = fopen( executavel, "r" );
#endif

			if ( ! fd )
	   	{
				printf( MSG_28 );   /* nao achou o executavel */
				exit(1);
		   }

		  /* Achar a posicao para marcar "ativado" */

		  strcpy(ident, STR_SERIAL);
		  ident[ N_SERIE ] = 0;

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
		   fseek(fd, pos-1, 0);
			fread( buffer, 1, 6, fd);
		   buffer[6] = '\0';
		   fclose(fd);
		   return( atol(buffer));
		}
	}



	printf( MSG_29 );
	fclose( fd );
	exit( 1 );
}
