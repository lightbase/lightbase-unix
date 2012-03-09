#include  <lb2all.h>

#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>
#include  <time.h>

#ifdef MSDOS
#  include  <dos.h>
#  include  <bios.h>
#  include  <dir.h>
#endif

#include  "restart.h"
#include  "serial.h"

#ifdef ESPANHA
#   define   M_NOFILE   "Fichero de Activacion no encontrado."
#   define   M_CHAVEIN  "Clave de Activacion incorrecta."
#   define   M_PRAZO    "Procduto en demonstracion, plazo expirado."
#else
#   define   M_NOFILE   "Arquivo de Ativação não encontrado."
#   define   M_CHAVEIN  "Chave de Ativação não Confere."
#   define   M_PRAZO    "Produto em demonstração, prazo expirado."
#endif


SS carrega( file, cliente, maxusers, serial, data, chave )

SC *file,
	*cliente,
	*maxusers,
	*serial,
	*data,
	*chave;
{
	FILE *fd;
	SS ret = 0;
	SC linha[80];

	if( !(fd = fopen( file, "r" ) ) )
	{
	 fprintf( stderr, "%c\n%s\n\n", 7, M_NOFILE);
	 goto fim;
	}

	/*********************** A T E N C A O ***************************
	 Atencao: Os números 40, 6, 8 e 12 correspondem respectivamente aos
		   tamanhos dos campos Cliente, Serial, Data e Chave.
		   Cuidado com mudanças !!!!!!!!!!!!!!!!!!!
	******************************************************************/

	fgets( linha, 79, fd );       /* Cliente */
	linha[ strlen(linha) - 1 ] = 0;
	sprintf( cliente, "%.40s", linha );
	tirabr( cliente );

	fgets( linha, 79, fd );        /* Serial &*/
	linha[ strlen(linha) - 1 ] = 0;
	sprintf( serial, "%.7s", linha );

	fgets( linha, 79, fd );        /* Maxusers */
	linha[ strlen(linha) - 1 ] = 0;
	sprintf( maxusers, "%s", linha );


	if( strlen( serial ) > 6 )
	{
		fgets( linha, 79, fd );
		linha[ strlen(linha) - 1 ] = 0;
		sprintf( data, "%.8s", linha );
	}
	else *data = 0;

	fgets( linha, 79, fd );
	linha[ strlen(linha) - 1 ] = 0;
	sprintf( chave, "%.12s", linha );

	fclose( fd );
	ret = 1;

fim:
	return ret;
}



#ifdef BBBB
SS copia_valida( file, cli, max, ser, dt )

SC *file,
	*cli,
	*max,
	*ser,
	*dt;
{
	SC serial[ TAM_SERIE + 2];
	SC cliente[ TAM_CLIENTE + 1];
	SC maxusers[ 20 ];
	SC data[9];
	SC actkey[ TAM_ACTKEY + 2];

	SS ret = 0;

	if( carrega( file, cliente, maxusers, serial, data, actkey ) )
	{
		if( !chave_valida( cliente, maxusers, serial, data, actkey ) )
		{
    		fprintf( stderr, "%c\n%s\n\n", 7, M_CHAVEIN);
			goto fim;
		}
	}
	else goto fim;

	/* Validar a data se demonstração */

	if( strlen( serial ) == (TAM_SERIE+1) )
	{
		if( data_long( data ) < sysdata_long() )
		{
			fprintf( stderr, "%c\n%s\n\n", 7,M_PRAZO);
			goto fim;
		}
	}

	strcpy( ser, serial  );
	strcpy( max, maxusers );
	strcpy( cli, cliente );
	strcpy( dt,  data    );
	ret = 1;

fim:
	return ret;
}
#endif

SS chave_valida( cliente, maxusers, serial, data, chave )

SC *cliente,
	*maxusers,
	*serial,
	*data,
	*chave;
{

	SC *strnibles;
	SS ret = 0;

	strnibles = calc_chave( cliente, maxusers, serial, data );
	if( !strcmp( strnibles, chave ) ) ret = 1;

fim:
	return ret;
}


SC *calc_chave( cliente, maxusers, serial, data )

SC *cliente,
	*maxusers,
	*serial,
	*data;
{
	SC strid[ 10 ];
	SC copia_serial[ 30 ];
	memset( strid, 0, 10 );

	strcpy( copia_serial, serial );
/*
	printf( "\nCliente: %s\nMaxusers: %s\nSerial: %s\nData: %s\n",
		cliente, maxusers, serial, data );
*/

#ifdef ESPANHA
	xora_id(copia_serial, "ESPANHA");
#else
	xora_id(copia_serial, "BRASIL");
#endif

	get_strid( cliente, strid );
	xora_id( copia_serial, strid );

	xora_id( copia_serial, maxusers );

	if( *data ) xora_id( copia_serial, data );
	xora_id( copia_serial, copia_serial );

	return str_nible( copia_serial );
}

SC *str_nible( chave )

SC *chave;
{
	static SC nibles[ 2*TAM_SERIE + 2 ];
	SI i,j;

	i = 0;
	for ( j=0; j<TAM_SERIE; j++)
	{
	 nibles[i++] = get_nible(chave[j],0);
	 nibles[i++] = get_nible(chave[j],1);
	}

	nibles[ 2*TAM_SERIE ] = 0;
	return nibles;
}

SC get_nible( c, n )

SC c;
SI n;
{
	SC cc;

	if( n )      cc = c & 0xf;
	else         cc = c >> 4;

	if( cc < 10 ) cc+='0';
	else          cc+='A'-10;

	return cc;
}

VOID get_strid( linha, id )

SC *linha;
SC *id;
{
	PT_TOKEN Tok;
	SC aux[128];
	SI  i = 0, j = 0, k = 0 ;

	while( 1 )
	{
	 k = 0;
	 strcpy( aux, linha );
	 pega_tk( aux, " ", &Tok );
	 while ( *Tok.cadeia )
	 {
	    if ( j < strlen( Tok.cadeia ) )
	    {
		  id[i++] = toupper(Tok.cadeia[j]);
		  if ( i == 8 )
			goto fim;
		  k++;
	    }
	    pega_tk( NULL, " ", &Tok );
	 }
	 j++;
	 if ( !k )
	 {
	    while( i < 8 )
	    {
		  id[i] = 'A' + 8 - i - 1;
		  ++i;
	    }
	    goto fim;
	 }
	}
fim:
	return;
}

VOID xora_id( serial, xorado )

SC *serial, *xorado;
{
	SI i,j,m;

	for ( i=0; serial[i]; ++i )
	{
	        m = i+1;

	        if( m > strlen(xorado) )
	           m = strlen(xorado);

		for ( j=m; xorado[j]; ++j )
		{
			serial[i] = serial[i] ^ xorado[j];
		}
	}

	return;
}




SS validkey( actkey )

SC *actkey;
{
	SI i, ret;
	SC *valid = "0123456789ABCDEF";

	i = ret = 0;
	while( actkey[i] )
		if ( !strchr( valid, actkey[i++] ) ) goto fim;

	ret = 1;

fim:
	return ret;
}

SL data_long( data )

SC *data;
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

VOID longchar( l, s )
SL l;
UC *s;
{
   /* converte um long em uma sequencia de caracteres low-high */

   SI I;

   for( I=0; I<4; ++I )
   {
      s[I] = l%256;
      l = l/256;
   }

   return;
}

SL charlong( s )
UC *s;
{
   SI I;
   SL l;

   l = 0;
   for( I=4; I>0; --I ) l = l*256+s[I-1];

   return l;
}


