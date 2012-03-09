#include  "lb2all.h"

#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>

#ifdef    VAXVMS
#include  <types.h>
#include  <stat.h>
#else
#include  <sys/types.h>
#include  <sys/stat.h>
#endif

#include  <time.h>

#ifdef MSDOS
#  include  <dos.h>
#  include  <bios.h>
#  include  <dir.h>
#endif
#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2files.h"

#include  "restart.h"
#include  "serial.h"
#include  "lb2head.h"
#include  "lb2prot.h"
#include  "lb2text.h"

extern TEXT *ap_aprovado;
extern TEXT ap_cliente[];
extern COUNT sai_stat;
extern TEXT data_limite[];

#define   STR_GLB 10

#define   M_VENCIDA  loc_str(STR_GLB, 520)
#define   M_SERIEIN  loc_str(STR_GLB, 521)
#define   M_PROTEC   loc_str(STR_GLB, 522)
#define   M_NOFILE   loc_str(STR_GLB, 523)
#define   M_CHAVEIN  loc_str(STR_GLB, 524)
#define   M_PRAZO    loc_str(STR_GLB, 525)


/***********************************************************************/
/***********************************************************************/
/**********  PROTECAO POR VIAS IMPLICITAS  *****************************/
/***********************************************************************/
/***********************************************************************/

#ifdef VIAS_IMPLICITAS

/* nao vai mais ser usado em funcao de toda copia ser prootecao via arquivo */


#define   TAM_LOOP   50
#define   DELTA_TEMPO 120

#define   COMPLICA   { TEXT B[ TAM_LOOP + 1 ]; COUNT I; long S; \
                     S = ftell( FP ); fread( &B, TAM_LOOP, 1, FP ); \
                     for ( I = 5;   I < TAM_LOOP - 5;   I++ ) {  \
                     if ( B[ I ] == 0 || (I % 5) != 3 ) \
                     B[ I ] = B[ I + 1 ] * B[ I - 1 ] / I; \
                     else B[ I ] = B[ I + 2 ] * B[ I - 2 ] / I; } \
                     fseek( FP, S, 0 ); }

F__GLB   COUNT checa_copia( maxusers, dias_restantes, num_serie )
COUNT *maxusers;
COUNT *dias_restantes;
LONG  *num_serie;
{
   COUNT       ret = 0;
   COUNT       I;
   TEXT        buf1[ TAM_LINHA  + 1 ];
   TEXT        buf2[ TAM_LINHA  + 1 ];
   UCOUNT      buf3[ 20 ];
   UCOUNT      x1, x2, x3, x4;
   TEXT       *l1;
   UTEXT       far  *P1;
   FILE       *FP;
   time_t      secs_now, secs_arq;
   struct tm  *tm_now;
   extern TEXT *ap_m0;

   *num_serie = -1L;

#ifdef RUNTIME
   l1 = LINHA_2;
#else
   l1 = LINHA_1;
#endif


#ifdef MSDOS
   FP = fopen( NOME_ARQ, "rb" );
#else
   FP = fopen( NOME_ARQ, "r" );
#endif

   /* para compatibilidade com versao anterior */
   if ( FP ==  NULL ) {
#ifdef MSDOS
      FP = fopen( ARQ_ANT, "rb" );
#else
      FP = fopen( ARQ_ANT, "r" );
#endif
   }

   if ( FP ==  NULL ) {
      *num_serie = 1;
      if ( getenv("LBUSERS") == NULL )
         *maxusers = 1;
      else
         *maxusers = atoi(getenv("LBUSERS"));
      ret = 0;
      ap_aprovado = C_DEMO;
      goto sai;
   }

   /* passa a receber numero de usuarios pelo arquivo */
   fgets( buf1, TAM_LINHA, FP );

   if ( buf1[0] == '#' && buf1[1] == '#' ) {
      *maxusers = MAX(1, atoi(&buf1[2]));
      fgets( buf1, TAM_LINHA, FP );
   }
   else
      *maxusers = 1;


   if ( strncmp( buf1, l1, strlen( l1 ) ) )
      ret = (ret) ? ret : 100;

   time( &secs_now );

   COMPLICA;

   fread( &secs_arq, sizeof(secs_arq), 1, FP );

   if ( secs_now - secs_arq > DELTA_TEMPO || secs_arq > secs_now )
      ret = (ret) ? ret : 101;

   tm_now = localtime( &secs_now );

   COMPLICA;

   fgets( buf1, TAM_LINHA, FP );

   sprintf( buf2, "%d %d %d %d %d\n",
            (tm_now->tm_mday + 'L'),
            ((tm_now->tm_mon  * 'l') + 'I'),
            (((tm_now->tm_year - 'l') * 'i') + 'G'),
            ((((tm_now->tm_wday / 'l') - 'i') * 'g') + 'H'),
            (((((tm_now->tm_yday % 'l') / 'i') - 'g') * 'h') + 'T') );

   if ( strcmp( buf1, buf2 ) )
      ret = (ret) ? ret : 102;

   P1 = MK_FP( 0x40, 0x10 );    buf1[ 0 ] = *P1 + tm_now->tm_mday;
   P1 = MK_FP( 0x41, 0x3  );    buf1[ 1 ] = *P1 * tm_now->tm_mon;
   P1 = MK_FP( 0x40, 0x18 );    buf1[ 2 ] = *P1 ^ tm_now->tm_year;

   COMPLICA;

   fread( buf2, 3, 1, FP );

   for ( I = 0;  I < 3;  I++ )
   {
      if ( buf1[ I ] != buf2[ I ] )
         ret = (ret) ? ret : 103;
   }

   buf1[ 0 ] = tm_now->tm_mday ^ 1919;
   buf1[ 1 ] = tm_now->tm_mon  + 99;
   buf1[ 2 ] = tm_now->tm_year * 1313;
   buf1[ 3 ] = tm_now->tm_wday + 1616;
   buf1[ 4 ] = tm_now->tm_yday * 1212;
   buf1[ 5 ] = tm_now->tm_mday ^ 55;
   buf1[ 6 ] = tm_now->tm_mon  + 1212;
   buf1[ 7 ] = tm_now->tm_year * 99;
   buf1[ 8 ] = tm_now->tm_wday + 77;
   buf1[ 9 ] = tm_now->tm_yday * 88;
   buf1[10 ] = tm_now->tm_yday ^ 2020;

   COMPLICA;

   fread( buf2, 11, 1, FP );

   for ( I = 0;  I < 11;  I++ )
   {
      if ( buf1[ I ] != buf2[ I ] )
         ret = (ret) ? ret : 104;
   }


   fread( num_serie, 4, 1, FP );

   fclose( FP );

   unlink( NOME_ARQ );

   x1 = (UCOUNT) ((secs_arq & 0x7F) << 8);
   x2 = (UCOUNT) (secs_arq & 0x7F00);
   x3 = (UCOUNT) ((secs_arq & 0x7F0000L) >> 8);
   x4 = (UCOUNT) ((secs_arq & 0x7F000000L) >> 16);

   x1 = 'S' | x1;
   x2 = 'i' | x2;
   x3 = 'm' | x3;
   x4 = 'p' | x4;

   buf3[ 0 ] = x1;
   buf3[ 1 ] = x2;
   buf3[ 2 ] = x3;
   buf3[ 3 ] = x4;
   buf3[ 4 ] = 'l';
   buf3[ 5 ] = 'e';

   for ( I = 0;   I < 6;   I++ )
   {
      UCOUNT x;
      if ( bioskey( 1 ) )  x = bioskey( 0 );
      else  x = 0;

      if ( x != buf3[ I ] )
         ret = (ret) ? ret : 105;
   }

fim:

   if ( ret != 0 )
      debug("%s : %d", M_PROTEC, ret);

   *dias_restantes = 0;

   ap_aprovado = ( ret != 0 ) ? C_DEMO : C_OK;

sai :

   return( ret );
}
#endif




/***********************************************************************/
/***********************************************************************/
/**********  PROTECAO POR ARQUIVO DE CONTROLE **************************/
/***********************************************************************/
/***********************************************************************/

#ifndef VIAS_IMPLICITAS

#ifdef MSDOS
   SS copia_valida( char *, char *, char *, char *, char * );
   long  procura( FILE *, char * );
   void  longchar( long, unsigned char *);
   long  charlong( unsigned char *);
#else
   SS copia_valida( );
   long  procura( );
   void  longchar( );
   long  charlong( );
#endif

#ifndef PROT_INFOCON
F__GLB   COUNT checa_copia( maxusers, dias_restantes, num_serie )
COUNT *maxusers;
COUNT *dias_restantes;
LONG  *num_serie;
{  COUNT ret = 0;
	UL dias;
	SC serial[ TAM_SERIE + 2];
	SC cliente[ TAM_CLIENTE + 2];
	SC Maxusers[ 20 ];
	SC data[12];
	SC fileconf[64];
	SI instalado =0;
	SC *str_serial;


	str_serial = STR_SERIAL;
   ap_aprovado = C_DEMO;
   *num_serie = -1L;


   /* procedimento que deve ser pego dentro do proprio desenvolvedor */
	if( str_serial[ N_SERIE ] == '.' ) {
	  	debug("Serialize...");
      ret = -1;
      goto fim;
	}

	/**************** Validar a copia ***************/
	/*****
	    1 - A copia e' valida se data de serializacao + NDIAS > Hoje ...
	    2 - A copia e' valida se arquivo light.dat e' coerente
	******/

	if( str_serial[ INSTALACAO ] == ' ' )   /* se ainda nao ativado */
	{
		memcpy( data, ( UC * ) &str_serial[DATA_TIME], 4 );
#ifdef LITE
   {
      struct stat status;
      TEXT full_name[100];

      strcpy(full_name, ap_home_lb);
      strcat(full_name, STR_BARRA);
      strcat(full_name, USU_FILE);
      strcat(full_name, USU_CAD);

      if ( stat(full_name, &status) == -1 ) {
   		dias = charlong( (unsigned char *)data );
      }
      else {
         dias = status.st_atime;
      }
   }
#else
		dias = charlong( (unsigned char *)data );
#endif
		dias =  ( UL ) time( NULL ) - dias;
      if( dias >= DIF ) {
			/******* copia NAO instalada ja' esta vencida *********/
			str_serial[ INSTALACAO ] = 0;
#ifdef PODE_SAIR
			fwat( 0, 13, curr_win );
			fwsetcursor( curr_win );
			debug("%s (%s)\r\n", M_VENCIDA, &str_serial[ N_SERIE ] );
/*			printf( "%s (%s)\r\n", M_VENCIDA, &str_serial[ N_SERIE ] ); */
#endif
         sai_stat = 1002;
         ret = -1;
         goto fim;
		}
		strncpy( serial, &str_serial[N_SERIE], TAM_SERIE);
		instalado = 0;
	}
	else
	{
		/* A Copia esta' ativada, Validar */

		sprintf( fileconf, "%s%clight.dat", getenv( env_dir ), BARRA );

		if( !copia_valida( fileconf, cliente, Maxusers, serial, data ) ) {
         ret = -1;
         goto fim;
      }


		if( strncmp( serial, &str_serial[N_SERIE], TAM_SERIE  ) )
		{
		   debug("%s %s", M_SERIEIN, serial);
         ret = -1;
         goto fim;
		}
      if ( strlen(serial) == (TAM_SERIE + 1) )
         ap_aprovado = C_DEMO;
      else
         ap_aprovado = C_OK;

		instalado = 1;
	}

	/* A partir daqui a copia esta' autorizada a funcionar */

	serial[6]= '\0';
   *num_serie = atol(serial);

	if ( instalado ) {
      *dias_restantes = 0;
      *maxusers = atoi(Maxusers);
      strcpy(ap_cliente, cliente);
   }
   else {
      *dias_restantes = NDIAS - (int)( dias / 86400L );
      ap_aprovado = C_DEMO;
      if ( getenv("LBUSERS") == NULL )
#ifdef MSDOS
			*maxusers = 1;
#else
			*maxusers = 4;
#endif
      else
         *maxusers = atoi(getenv("LBUSERS"));
   }

fim:

	/* retrona
		-1 : Nao inicializou, vencida, etc.
		 0 : Copia pode ser utilizada.
	*/

   return( ret );
}


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
	 win_bell( BEL_INVALID );
	 debug(M_NOFILE);
	 win_refresh();
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
	SC data[12];
	SC actkey[ TAM_ACTKEY + 2];

	SS ret = 0;

	if( carrega( file, cliente, maxusers, serial, data, actkey ) )
	{

		if( !chave_valida( cliente, maxusers, serial, data, actkey ) )
		{
			win_bell( BEL_INVALID );
			debug(M_CHAVEIN);
			win_refresh();
			goto fim;
		}
	}
	else goto fim;

	/* Validar a data se demonstração */

	if( strlen( serial ) == (TAM_SERIE+1) )
	{
		if( data_long( data ) < sysdata_long() )
		{
         sai_stat = 1001;
			goto fim;
		}
      else
         strncpy(data_limite, data, 10);   /* data limite para imprimir quando sair */
	}

	ret = 1;

	strcpy( ser, serial  );
	strcpy( max, maxusers );
	strcpy( cli, cliente );
	strcpy( dt,  data    );

fim:

	return ret;
}

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


int pega_tk( cadeia, separador, result )

char *cadeia, *separador;
PT_TOKEN *result;
{
   /*
      Pega tudo de cadeia até encontrar separador. Quando encontrar
      coloca 'zero' na posicao e retorna 1. Se nao encontrar retorna 0.
   */

   static char *pt_inicio, *pt_start;

   if( cadeia )   pt_inicio = cadeia;
   else           pt_inicio = pt_start;

   result->cadeia = pt_inicio;
   result->achou  = 0;

   for( ; *pt_inicio; ++pt_inicio )
   {
      if( strchr( separador, *pt_inicio ) )
      {
         result->achou = 1;
         result->sep   = *strchr( separador, *pt_inicio );
         *pt_inicio = 0;
         pt_start   = ++pt_inicio;
         goto fim;
      }
   }
   pt_start = pt_inicio;

fim:
   return result->achou;
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

#endif

#endif
