/* OK */
#include   <stdio.h>
#include   <string.h>
#include   <ctype.h>
#include   <sys/types.h>

#include   "restart.h"

typedef char SC;
typedef long SL;
typedef short int SS;
typedef int SI;
typedef void VOID;

#ifdef MSDOS
int atoi( char *);
pega_param(int, char **);
void pega_dados(char *);
#else
int atoi( );
pega_param( );
void pega_dados( );
#endif

SC   *filelight  = "light.dat";

SC serial[ TAM_SERIE + 2];
char maxusers[ 20 ], usr_str[20];
SC cliente[ TAM_CLIENTE + 1];
SC data[9];
int no_data=0;

main( argc, argv )
int argc;
char **argv;
{
	SC *actkey;
	SS tem_confirma;

	if ( argc > 1 && argv[1][0] != '-' ) {
	   printf("-S Numero de serie\n");
	   printf("-N Numero de Usuarios\n");
	   printf("-D Data Limite\n");
	   printf("-C Nome do cliente\n");
	   printf("-A Arquivo de dados\n");
	   return(0);
	}

	printf( MSG_01 );
	printf( MSG_02 );
	printf( MSG_03 );

#ifdef ESPANHA
	printf("\n\nChaves validas para copias da ESPANHA\n\n");
#else
	printf("\n\nChaves validas para copias no BRASIL\n\n");
#endif


   serial[ 0 ] = maxusers[ 0 ] = data[ 0 ] = cliente[ 0 ] = 0;

   tem_confirma = pega_param(argc, argv);

	while( 1 )
	{
	 /* Pedir número de série */

	 if ( ! serial[0] )
		 if ( !pede_serie( serial ) )  break;

	 if ( ! maxusers[0] )
		 if ( !pede_maxusers( maxusers ) )    break;

	 if ( ! data[0] && no_data == 0)
		 pede_data( data );

	 if ( strlen(data) > 0 )
	 {
	 	serial[ TAM_SERIE ]   = 'D'; /* Se demo, data de validade */
	 	serial[ TAM_SERIE+1 ] = 0; /* Se demo, data de validade */
	 }

	 if ( ! cliente[0] )
		 if ( !pede_cliente( cliente ) )    break;

	 /* pede confirmação */

	 printf( MSG_04, serial );
	 if( strlen( serial ) > TAM_SERIE ) printf( MSG_05, data );
	 printf( MSG_06, cliente );
	 printf( MSG_37, maxusers );

	 if ( tem_confirma == 1 )
		 if ( pede_confirma( MSG_25 ) == MSG_08 ) continue;

	 sprintf(usr_str, "%3.3dU%3.3d", atoi(maxusers), atoi(maxusers));

	 actkey = calc_chave( cliente, usr_str, serial, data );
	 printf( MSG_07, actkey );

	 /* Chave gerada. Perguntar se deseja gravar o arquivo */

	 if( argc == 1 && (pede_confirma( MSG_27 ) == MSG_08) ) break;

	 if ( grava( cliente, usr_str, serial, data, actkey ) )
	 {
		 BELL;
		 printf( MSG_31 );
		 break;
	 }
	}

	return( 0 );
}


pega_param(argc, argv)
int argc;
char **argv;
{  int i, ret = 1;

	for( i = 1;   i < argc;   i++ ) {

      if ( argv[i][0] != '-' )
         continue;

      else if ( toupper(argv[i][1]) ==  'S' ) { /*serie */
         strcpy(serial,&argv[i][2]);
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
	         strcpy(data,&argv[i][2]);
         }
      }
      else if ( toupper(argv[i][1]) ==  'C' ) {  /* cliente */
         strcpy(cliente,&argv[i][2]);
      }
      else if ( toupper(argv[i][1]) ==  'A' ) {  /* arquivo */
         pega_dados(&argv[i][2]);
         ret = 0;
      }
   }

   return(ret);
}



void pega_dados(arq)
char *arq;
{  int t;
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
         strcpy(serial,&linha[2]);
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
	         strcpy(data,&linha[2]);
         }
         else
            no_data = 1;
      }
      else if ( toupper(linha[1]) ==  'C' ) {  /* cliente */
         strcpy(cliente,&linha[2]);
      }
   }

   fclose(fp);
}


SS pede_serie( serie )

SC *serie;
{

	SC Serie[ 128 ];
	SS ret;

	ret = 0;
	while( 1 )
	{
	 	printf( MSG_12 );

	 	gets( Serie );
	 	eliminabr( Serie );
	 	if( serie_Ok( Serie ) )
	 	{
	 	   strcpy( serie, Serie );
	 	   ret = 1;
	 	   break;
	 	}
	}
	return ret;
}

SI serie_Ok( Serie )

char *Serie;
{
   char *p;
   SI ret;

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


SS pede_data( data )

SC *data;
{

	SC Data[ 128 ];
	SS ret;

	ret = 0;
	while( 1 )
	{
	 printf( MSG_15 );

	 gets( Data );
	 eliminabr( Data );
	 if( !strlen( Data ) ) return -1;
	 if( data_Ok( Data ) )
	 {
	    sprintf( data, "%.2s/%.2s/%.2s", Data, Data+2, Data+4 );
	    ret = 1;
	    break;
	 }
	}
	return ret;
}

SI data_Ok( Data )

SC *Data;
{
	SC *p;
	SC data[ 9 ];
	SI ret = 0;

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

SS pede_cliente( cliente )

SC *cliente;
{

	SC Cliente[ 128 ];
	SS ret;

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

SS get_actkey( actkey )

SC *actkey;
{
	SC Actkey[ 128 ];
	SS ret;

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

SS pede_confirma( msg )
char *msg;
{
	SC R[ 128 ];

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

SS grava( cliente, maxusers, serial, data, chave )

SC *cliente,
	*maxusers,
	*serial,
	*data,
	*chave;
{
	FILE *fd;
	SS ret = 0;

	if( !(fd = fopen( filelight, "w" ) ) )
	{
		printf( MSG_26, filelight );
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


