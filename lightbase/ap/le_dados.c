/* OK */
/* Programa que vai para o cliente */

#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <ctype.h>
#include   <sys/types.h>
#include  <time.h>

#include   "restart.h"
#include   "serial.h"

#ifdef ESPANHA
#   define M_WAIT 	 "\nEspere...\n"
#else
#   define M_WAIT 	 "\nAguarde...\n"
#endif

#define   TAB_CHAR  9
#define   SL long

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
   long pos;
   FILE *fd;
   int marcar =0, C;

#ifndef MSDOS
   unsigned short my_id;
#endif



   while( 1 )
   {
      serial[ 0 ] = maxusers[ 0 ] = data[ 0 ] = cliente[ 0 ] = actkey[ 0 ] = 0;

    	if ( argc > 1 && strlen(argv[1]) < 7 && atol( argv[1]) > 0 ) {
    		if ( serie_Ok( argv[1] ) )
          	strcpy(serial, argv[1]);
    	}

	 if ( serial[0] == '\0' && !pede_serie( serial ) )    break;

	 if ( !pede_cliente( cliente ) ) break;

	 if ( !pede_maxusers( maxusers ) )    break;

	 pede_data( data );

	 /* pede confirmação */

	 fprintf(stderr, MSG_04, serial );
	 fprintf(stderr, MSG_06, cliente );
	 fprintf(stderr, MSG_37, maxusers );

    if (data[0] != '\0' )
       fprintf(stderr, MSG_05, data );

	 if ( pede_confirma(MSG_25) == MSG_08 ) continue;

	 printf("-S%s\n", serial);
	 printf("-C%s\n", cliente);
	 printf("-N%s\n", maxusers);
	 printf("-D%s\n", data);
    break;

   }

   return 0;

}

short pede_serie( serie )

char *serie;
{

   char Serie[ 128 ];
   short ret;

   ret = 0;
   while( 1 )
   {
	 	fprintf(stderr,  MSG_12 );

	 	if ( gets( Serie ) == NULL )
	 	   exit(1);

	 	eliminabr( Serie );

	 	if ( Serie[0] == '\0' )
	 	   exit(1);

	 	if( serie_Ok( Serie ) )
	 	{
	 	   strcpy( serie, Serie );
 	   	ret = 1;
 	   	break;
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
	 fprintf(stderr, MSG_13 );
	 ret = 0;
   }
   else
   {
	 p = Serie;
	 while(isdigit(*p)) ++p;
	 if( *p )
	 {
		BELL;
		fprintf(stderr,  MSG_14 );
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
	 	fprintf(stderr,  MSG_35 );
	 	gets( Maxusers );
	 	eliminabr( Maxusers );
	 	if( maxusers_Ok( Maxusers ) )
	 	{  sprintf(maxusers, "%03.3d", atoi(Maxusers));
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
	 fprintf(stderr, MSG_36 );
	 ret = 0;
   }
   else
   {
	 p = Maxusers;
	 while(isdigit(*p)) ++p;
	 if( *p )
	 {
		BELL;
		fprintf(stderr,  MSG_14 );
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
	 fprintf(stderr, MSG_15 );

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
   	fprintf(stderr,  MSG_16 );
   	goto fim;
   }
   else
   {
	 	p = Data;
	 	while(isdigit(*p)) ++p;

	 	if(*p)
	 	{
	 		BELL;
	 		fprintf(stderr,  MSG_17 );
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
	 fprintf(stderr, MSG_18 );
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
	 fprintf(stderr, MSG_19 );
	 fprintf(stderr, MSG_20 );

	 if ( gets( Cliente ) == NULL )
	    exit(1);

	 tirabr( Cliente );
    if ( Cliente[0] == '.' )
	 	   exit(1);

	 if( !strlen( Cliente ) )
	 {
	 	BELL;
	 	continue;
	 }

	 if( strlen( Cliente ) > 40 )
	 {
	    fprintf(stderr, MSG_21 );
	    BELL;
	    continue;
	 }
	 ret = 1;
	 strcpy( cliente, Cliente );
	 break;
   }

   return ret;
}




short pede_confirma( msg )
char *msg;
{
   char R[ 128 ], c;

   while( 1 )
   {
	 fprintf(stderr, msg );
	 gets( R );

	 eliminabr( R );
	 c = toupper(*R);

	 if( ( c == MSG_08 ) || ( c == MSG_09 ) ) break;

	 BELL;
   }

fim:
   return toupper(*R);
}


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




SL data_long( data )

char *data;
{
	return (atol(&data[ 6 ]) + 1900)*10000L +
			 atol(&data[ 3 ])*100L +
			 (atol(&data[ 0 ]));
}


SL sysdata_long(  )

{
	time_t secs_now;  /* para avaliar se dentro de prazo de validade */
	struct tm *data;   /* se demonstracao */

	time(&secs_now);
	data = localtime( &secs_now );

	return ( SL )(data->tm_year + 1900)*10000L +
			 ( SL )(data->tm_mon + 1)*100L +
			 ( SL )data->tm_mday;
}
