/*** INFOCON *** persona.c *** VERSAO 1.16 *** 89/06/23 ***/

#include	<time.h>
#include	<stdio.h>

#if EDIX || SOR
#include	<sys/utsname.h>
#endif

#ifdef MSDOS
#include <string.h>
#include <stdlib.h>
#endif

#include	"wprotec.h"
#include	"wprotec1.h"

#define	TAM_TERM	3

#define	TAM_DATA	6

#ifdef NAOPODEDEFINIR
#ifdef SOX
typedef long	t_maq;			/* parametros tem tipos diferentes em certas chamadas de funcoes no SOX 	*/
#else
typedef int	t_maq;
#endif
#endif

char	pe_ja_cript = 0;

/*----------------------------------------------------------------------*/

int pe_map()
{
	char		data[ MAXDATA + 1 ];

#ifdef DIGIX
	char		cpu[ MAXCPU + 1 ];
#endif

	int		check = 0,
			chk_check;

	long		tloc;

	struct tm	*dma, *localtime();

	DPRINTF( (D_PERS, "Verificando se foi personalizado" ) );
	if( PERS( cl_pers ) != PE_PERS || PERS( is_pers ) != PE_PERS ) {
		/* Nao passou o "ischeck" e o "person" em cima
		 * do executavel
		 */
		DPRINTF( (D_PERS, "Nao passou 'ischeck' ainda !" ) );
		return( PE_INSTALA );
	}

	/*
	 * verifica checksum da estrutura INFOCON (a do ischeck)
	 */
	DPRINTF( (D_PERS, "Decriptografei a da infocon ..." ) );
	check = decriptografa( NOME( is_pers ), CHECKSUM( is_pers ) );
	DPRINTF( (D_PERS, "check = %d\n", check ) );
	chk_check = ((CHECKSUM( is_pers )[0]&0xFF)<<8) | (CHECKSUM( is_pers )[1] & 0xFF);
	chk_check &= 0xFFFF;
	if( check != chk_check ) {
		DPRINTF( (D_PERS, "checksum infocon = %d, checksum1 = %d", CHECKSUM( is_pers )[ 0 ],CHECKSUM( is_pers )[ 1 ] ) );
		return( PE_INSTALA );
	}

	/*
	 * verifica checksum da estrutura do cliente (a do person)
	 */
	check = decriptografa( NOME( cl_pers ), CHECKSUM( cl_pers ) );
	DPRINTF( (D_PERS, "check = %d\n", check ) );
	chk_check = ((CHECKSUM( cl_pers )[0]&0xFF)<<8) | (CHECKSUM( cl_pers )[1] & 0xFF);
	chk_check &= 0xFFFF;
	if( check != chk_check ) {
		DPRINTF( (D_PERS, "checksum cliente = %d, checksum1 = %d", CHECKSUM( cl_pers )[ 0 ],CHECKSUM( is_pers )[ 1 ] ) );
		return( PE_INSTALA );
	}
	pe_ja_cript = 1; 	/*indica que jah decriptografei */

#ifdef DIGIX
	if( CPU( is_pers )[ 0 ] != NULO ) {

		lenums( cpu );

		if( strcmp( cpu, CPU( is_pers ) ) != 0 ) {
			return( PE_CPU );
		}
	}
#endif

	if( DATA( is_pers )[ 0 ] != NULO ) {
		time( &tloc );

		dma = localtime( &tloc );

		sprintf( data, "%02.2d%02.2d%02.2d",
			dma -> tm_year,
			dma -> tm_mon + 1,
			dma -> tm_mday
		);

		if( strncmp( data, DATA( is_pers ), 6 ) > 0 ) {
			return( PE_DATA );
		}
	}
	return( PE_OK );

}


/*----------------------------------------------------------------------*/
char *pe_pers()
{

	/* pega o nome na estrutura do cliente */
	DPRINTF( (D_PERS, "Nome do cliente = <%s>", NOME( cl_pers ) )  );
	if( NOME( cl_pers )[ 0 ] == NULO ) {
		return( NULO );
	}

	return( NOME( cl_pers ) );
}

/*----------------------------------------------------------------------*/
char *pe_serie()
{

	/* pega o numero de serie na estrutura do cliente */
	DPRINTF( (D_PERS, "Numero de serie = <%s>", SERIE( cl_pers ) )  );
	if( SERIE( cl_pers )[ 0 ] == NULO ) {
		return( NULO );
	}

	return( SERIE( cl_pers ) );
}


/*----------------------------------------------------------------------*/
char *pe_data( ano, mes, dia )
int *ano, *mes, *dia;
{

	char		aux[ 3 ];

	DPRINTF( (D_PERS, "Data = <%s>", DATA( cl_pers ) )  );
	if( DATA( is_pers )[ 0 ] == NULO ) {
		return( NULO );
	}

	strncpy( aux, &DATA( is_pers )[ 0 ], 2 );
	*(aux+2) = '\0';
	*ano = atoi( aux );

	strncpy( aux, &DATA( is_pers )[ 2 ], 2 );
	*(aux+2) = '\0';
	*mes = atoi( aux );

	strncpy( aux, &DATA( is_pers )[ 4 ], 2 );
	*(aux+2) = '\0';
	*dia = atoi( aux );

	return( DATA( is_pers ) );
}


/*----------------------------------------------------------------------*/
int pe_tty()
{

	DPRINTF( (D_PERS, "Numero de terminais = <%s>", MAXTTY( cl_pers ) ) );
	return( atoi( MAXTTY( cl_pers ) ) );
}

/*----------------------------------------------------------------------*/
void pe_x()
{
	if( ! pe_ja_cript ) {
		/*
		 * As estruturas nao foram decriptografadas ainda
		 */
		if( PERS( is_pers ) == PE_PERS ) {
			DPRINTF( (D_PERS, "Decriptografei a da infocon ..." ) );
			decriptografa( NOME( is_pers ), CHECKSUM( is_pers ) );
			}
		if( PERS( cl_pers ) == PE_PERS ) {
			DPRINTF( (D_PERS, "Decriptografei a do cliente ..." ) );
			decriptografa( NOME( cl_pers ), CHECKSUM( cl_pers ) );
			}
		pe_ja_cript = 1;
	}
#ifdef PORTUGUES
	fprintf( stderr, "Personalizacao ...: %s\n", NOME( is_pers ) );
	fprintf( stderr, "Numero de serie ..: %s\n", SERIE( is_pers ) );
#ifdef DIGIX
	fprintf( stderr, "Numero CPU .......: %s\n", CPU( is_pers ) );
#endif /* DIGIX */
	fprintf( stderr, "Volume ...........: %s\n", VOLUME( is_pers ) );
	fprintf( stderr, "Data .............: %s\n", DATA( is_pers ) );
	fprintf( stderr, "Numero terminais..: %s\n", MAXTTY( is_pers ) );

	/* a estrutura de personalizacao do cliente */
	fprintf( stderr, "\n" );
	fprintf( stderr, "Nome do cliente ..: %s\n", NOME( cl_pers ) );
	fprintf( stderr, "Numero de serie ..: %s\n", SERIE( cl_pers ) );
#ifdef DIGIX
	fprintf( stderr, "Numero CPU .......: %s\n", CPU( cl_pers ) );
#endif /* DIGIX */
	fprintf( stderr, "Volume ...........: %s\n", VOLUME( cl_pers ) );
	fprintf( stderr, "Data .............: %s\n", DATA( cl_pers ) );
	fprintf( stderr, "Numero terminais..: %s\n", MAXTTY( cl_pers ) );
#else
	fprintf( stderr, "Custom string ....: %s\n", NOME( is_pers ) );
	fprintf( stderr, "Serial number ....: %s\n", SERIE( is_pers ) );
#ifdef DIGIX
	fprintf( stderr, "CPU number .......: %s\n", CPU( is_pers ) );
#endif /* DIGIX */
	fprintf( stderr, "Volume ...........: %s\n", VOLUME( is_pers ) );
	fprintf( stderr, "Date .............: %s\n", DATA( is_pers ) );
	fprintf( stderr, "TTY number .......: %s\n", MAXTTY( is_pers ) );

	/* a estrutura de personalizacao do cliente */
	fprintf( stderr, "\n" );
	fprintf( stderr, "Licensee name ....: %s\n", NOME( cl_pers ) );
	fprintf( stderr, "Serial number ....: %s\n", SERIE( cl_pers ) );
#ifdef DIGIX
	fprintf( stderr, "CPU number .......: %s\n", CPU( cl_pers ) );
#endif /* DIGIX */
	fprintf( stderr, "Volume ...........: %s\n", VOLUME( cl_pers ) );
	fprintf( stderr, "Date .............: %s\n", DATA( cl_pers ) );
	fprintf( stderr, "TTY number .......: %s\n", MAXTTY( cl_pers ) );
#endif /* PORTUGUES */
}


/*----------------------------------------------------------------------*/
pe_teclou_x( num_arg, vet_arg )
int	num_arg;
char	**vet_arg;
{

	register int	i;

	for( i = 0; i < num_arg; i++ ) {
		if( strcmp( vet_arg[i], "-X" ) == 0 ) {
			/* o cara so quer ver a personalizacao
			 */
			return( 1 );
		}
	}
	return( 0 );
}


/*-----------------------------------------------------------
 * criptografa
 *
 * criptografa todos os caracteres entre dois
 * apontadores dados. Este algoritmo eh inversivel
 * pela funcao 'decriptografa'.
 *
 * retorna um "checksum"
 *
 */
criptografa( bufi, buff )
char *bufi, *buff;
{
	register int	c,
			ant = 0,
			soma = 0;

	for( ; bufi < buff; ++bufi ) {

		c = *bufi;

		c = (c + ant - soma) & 0377;

		ant = c;

		soma += ( c * (ant + 1 ) );

		*bufi = c;
	}
	return( soma & 0xFFFF );
}

/*-----------------------------------------------------------
 * decriptografa
 *
 * decriptografa todos os caracteres entre dois
 * apontadores dados. Este algoritmo desfaz o que a funcao
 * 'criptografa' faz.
 *
 * retorna um "checksum"
 *
 */

decriptografa( bufi, buff )
char *bufi, *buff;
{
	register int	ant = 0,
			soma = 0,
			c;

	for( ; bufi < buff; ++bufi ) {

		c = *bufi & 0377;

		*bufi = ( c - ant + soma ) & 0377;

		ant = c;
		soma += ( c * ( ant + 1 ) );

	}
	return( soma & 0xFFFF );
}

/*-----------------------------------------------------------
 * crypt_key
 *
 * criptografa todos os caracteres entre dois
 * apontadores dados usando um algoritmo NAO INVERSIVEL.
 * Este algoritmo mapeia todos os caracteres gerados para
 * caracteres entre 'a' e 'z' (inclusive). Isto porque o string
 * resultante serah usado como uma senha "digitavel" (Activation Key),
 * que nao pode conter caracteres de controle ou outros caracteres
 * nao imprimiveis.
 *
 * retorna nada
 *
 */
void crypt_key( str_ini, str_fim )
char 	*str_ini,
	*str_fim;
	{

	register int	soma = 0,
			ant = 0,
			c;

	soma = criptografa( str_ini, str_fim );
	--str_fim;
	do {
		c = (*str_fim + ant - soma) & 0377;
		soma += ( c * (ant + 1) );
		ant = c;
		/* mapeia para um numero entre 0 e 25
		 * (as letras do alfabeto) e obtem
 		 * uma letra entre 'a' e 'z'
 		 */
		c = (c % 26) + 'a';
		*str_fim = c;
	} while( str_fim-- > str_ini );
}

#define		isdigit( c )	( (c) >= '0' && (c) <= '9' )
#define	bissexto( x )	( (!(x%4) && (x%100)) || !(x%400) )
static int tab_bissexto[2][13] = { {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
                        {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } };

valid_date( date )
char	*date; 		/* a data no formato AAMMDD */
	{
	register int 	dia,
			mes,
			ano,
			aux;

	/* existem caracteres nao numericos no string ? */
	for( aux = 0; aux < 6; aux++ ){
		if( ! isdigit( *(date+aux) ) ){
			return( 0 );
			}
		}

	/* o tamanho do string eh 6 ? */
	if( *(date+aux) ){
		return( 0 );
		}

	ano = 10 * ( *(date+0) - '0' ) + ( *(date+1) - '0' ) + 1900;
	mes = 10 * ( *(date+2) - '0' ) + ( *(date+3) - '0' );
	dia = 10 * ( *(date+4) - '0' ) + ( *(date+5) - '0' );

	if( mes < 1 || mes > 12 ){
		return( 0 );
		}

	if( dia < 1 || dia > tab_bissexto[ bissexto( ano ) ] [ mes ] ) {
		return( 0 );
		}

	return( 1 );
}


char *ttyconfere( serie, activ_key, vertty )
char	*serie;
char	*activ_key;
int	vertty;
{

	static char	ntty[TAM_TERM+1];
	char		cripserie[ MAXSERIE + 1 ];
	register int	i;
	int		max;
	char		temp[4];
/*	t_maq		size_serie = 0; */

	/* como o algoritmo de geracao da chave de ativacao nao eh
	 * reversivel, tenho que encontrar o numero de terminais na
	 * pesquisa exaustiva
	 */

	max = 999;	/* numero maximo de terminais a tentar */
/*	size_serie = (t_maq)strlen( serie );
*/	for( i = 0; i <= max; ++i ) {
		strcpy( cripserie, serie );
		sprintf( temp, "%03d", i );
		memcpy( cripserie+2, temp, 3 );
		crypt_key( cripserie, cripserie + MAXSERIE );

		if( strcmp( activ_key, cripserie ) == 0 ){
			/* tudo ok */
			sprintf( ntty, "%03d", i );
			return( ntty );
		}
	}

	return( NULL );
}


char *dataconfere( serie, activ_key, verdata )
char	*serie;
char	*activ_key;
int	verdata;
{
	static char	data[TAM_DATA+1];
	char		cripserie[ MAXSERIE + 1 ];
	register int	i;
	int		max;
	void		incrdata();
/*	t_maq		size_serie = 0; */

	/* como o algoritmo de geracao da chave de ativacao nao eh
	 * reversivel, tenho que encontrar a data com pesquisa exaustiva
	 */
	/* inicializa a data de procura
	 * nao inicializa na declaracao pois data eh um variavel static
	 * e na proxima chamada da funcao daria erro
	 */
	istrncpy( data, "910101", TAM_DATA );
	max = 3285;	/* numero maximo dias a tentar (ateh 1999) */
/*	size_serie = (t_maq) strlen( serie );
*/	for( i = 0; i <= max; ++i ) {
		strcpy( cripserie, serie );
		incrdata( data, 1 );	/* incrementa a data de 1 */
		memcpy( cripserie, data, TAM_DATA );

		crypt_key( cripserie, cripserie + MAXSERIE );

		if( strcmp( activ_key, cripserie ) == 0 ){
			/* tudo ok */
			return( data );
		}
	}
	return( NULL );
}

/* nao faco consistencia de qualquer tipo no string recebido */
void  incrdata( data, incr )
char		*data; 		/* a data no formato AAMMDD */
register int	incr;
	{
	register int 	dia;
	register int 	mes;
	register int 	ano;

	ano = 10 * ( *(data+0) - '0' ) + ( *(data+1) - '0' ) + 1900;
	mes = 10 * ( *(data+2) - '0' ) + ( *(data+3) - '0' );
	dia = 10 * ( *(data+4) - '0' ) + ( *(data+5) - '0' );

	while( incr-- > 0 ) {
		if( ++dia > tab_bissexto[ bissexto( ano ) ] [ mes ] ) {
			dia = 1;
			if( ++mes > 12 ) {
			mes = 1;
				if ( ++ano > 1999 ) {
				ano = 1900;
			}
			}
		}
	}
	sprintf( data, "%02d%02d%02d", ano - 1900, mes, dia );
}

void istrncpy( dest, orig, tam )
register char *dest;
char *orig;
register int tam;
{

	strncpy( dest, orig, tam );
	dest[ tam ] = '\0';
}
