/*** INFOCON *** %M% *** VERSAO %R%.%L% *** %E% ***/

/* Este programa gera sequencias de numeros de serie
 * dos softwares INFOCON, baseado no seguinte modelo:
 *
 * Cada numero de serie eh acompanhado de sua chave de ativacao
 * (Activation Key), que eh obtida a partir de um algoritmo de
 * transformacao sobre o numero de serie - criptografia.
 *
 */

#include 	<stdio.h>

#ifndef UNIX
	#include <stdlib.h>
	#include <string.h>
	#include <io.h>

	char *incrementa ( char *, char * );
	int opcoes( int *, char **[] );
	int sintaxe ( void );
	int criptografa ( char *, char * );
	int decriptografa ( char *, char * );
	int crypt_key ( char *, char * );
#endif

#include	   "debug.h"
#include	   "persona.h"

char	serie[ MAXNOME + 1 ],		/* armazena o numero de serie inicial	*/
	activ_key[ MAXNOME + 1 ];	/* armazena a chave de ativacao 	*/
long	quantidade = 1;			/* quantas sequencias iremos gerar	*/
char 	silencio = 0;			/* trabalha em modo silencioso	?	*/
int	numtty = 0;			/* numero maximo de terminais */

main( argc, argv )
int	argc;
char	*argv[];
	{

	register long 	count;
	int		size_serie;
	char		*incrementa();

	INI_DEBUG( &argc, &argv, PERS_DBGARQ );

	opcoes( &argc, &argv );

	if( ! *serie ){
		if( !silencio && isatty( 0 ) ){
			fprintf( stdout, "Digite o numero de serie inicial: \n" );
			}
		gets( serie );
		}



	for( count = 0; count < quantidade; count++ ){
		char	temp[4];

		strcpy( activ_key, serie );
		if( numtty != 0 ) {
			/* quando tem terminais, substitui
			 * algumas letras do numero de chave pelo
			 * numero de terminais
			 */
			sprintf( temp, "%03d", numtty );
			memcpy( activ_key+2, temp, 3 );
		}

		size_serie = strlen( activ_key );
		DPRINTF( (D_PERS, "tamanho = <%d>", size_serie ) );
		crypt_key( activ_key, activ_key + size_serie );
		DPRINTF( (D_PERS, "serie       = <%s>\n\t      serie cript = <%s>", serie, activ_key ) );
		fprintf( stdout, "%s\t%s\n", serie, activ_key );
		incrementa( serie, serie + size_serie - 1 );
		}

	return ( 1 );

	}


/*-----------------------------------------------------------------------
 * incrementa
 * objetivo:
 *	incrementar o numero de serie.
 *
 * retorna:
 *	o argumento recebido
 */

char *incrementa( str_ini, str_fim )
char	*str_ini,	/* o inicio da cadeia numerica 			*/
	*str_fim;	/* o ultimo caractere da cadeia (nao o nulo)	*/
	{

	if( ++(*str_fim) > '9' ){
		*(str_fim) = '0';
		if( str_fim > str_ini ){
			incrementa( str_ini, str_fim - 1 );
			}
		}
	return( str_ini );
	}


/*-----------------------------------------------------------------------
 * opcoes
 * objetivo:
 *	analizar as opcoes do comando e a sintaxe de chamada
 * retorna:
 *	nada
 *	indiretamente, altera argc e argv para que a rotina processa()
 *	possa funcionar sem saber nada sobre as opcoes especificadas
 * algoritmo:
 *	obvio. Altera argc e argv indiretamente
 */

opcoes(argcp, argvp)
int *argcp;	/* aponta para argc, o qual e' mudado no final da rotina */
char **argvp[];	/* aponta para argv */
{
	register int argc;
	register char **argv;
	char *p;

	argc = *argcp;
	argv = *argvp;


	while( argc > 1 && *argv[1] == '-' ) {
		p = argv[1]+1;
		while( *p ) {
			switch( *p ) {
			case 'i':	/* sequencia inicial */
				strncpy( serie, ++p, MAXSERIE );
				*(serie + MAXSERIE) = '\0';
				goto prox_arg;
			case 'n':	/* numero de sequencias	*/
				quantidade = atol( ++p );
				goto prox_arg;
			case 's':	/* modo silencioaso */
				silencio++;
				goto prox_arg;
			case 't':	/* numero maximo de terminais */
				numtty = atoi( ++p );
				goto prox_arg;
			default:	/* opcao invalida  		*/
				sintaxe();
				break;
			} /* fim switch */
			p++;
		}
prox_arg:
		argc--;
		argv++;
	} /* fim while */

	/*
	 * devolve os novos valores de argc e argv para o beneficio
	 * de inicializa() e de processa()
	 */
	*argcp = argc;
	*argvp = argv;

	return ( 1 );
}

/*-----------------------------------------------------------------------
 * sintaxe()
 * objetivo:
 *	indica a forma de ativacao do comando
 */
sintaxe()
	{
	fprintf( stderr, "\nSintaxe: serie [-s] [-iNUMSERIE] [-nQUANT] [-tNUMTTY]\n" );
	fprintf( stderr, "onde: \n" );
	fprintf( stderr, "	-s Modo silencioso \n" );
	fprintf( stderr, "	-i Especifica NUMSERIE como numero inicial \n" );
	fprintf( stderr, "	-n Gera QUANT numeros de serie \n" );
	fprintf( stderr, "      -t Gera para NUMTTY terminais\n" );
	exit( 1 );
	return ( 1 );
	}

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
 *
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
crypt_key( str_ini, str_fim )
char 	*str_ini, *str_fim;
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
	return ( 1 );
}

