/*** INFOCON *** pers.c *** VERSAO 1.16 *** 89/06/23 ***/

#include	<stdio.h>

#ifndef UNIX
	#include <stdlib.h>
	#include <string.h>
	#include <io.h>

	int faca ( char * );
	int sintaxe ( void );
	void tira_branco ( char * );
	void converte ( char * );
	int opcoes( int *, char **[] );
	int grava_estr ( void );
	int procura_im ( void );

#endif
#include "wprotec.h"

extern  T_PERS is_pers;

char	*_verpers = "4.2";
char	**_lixo6 = &_verpers;

#define		NULO		'\0'
#define		isdigit( c )	( (c) >= '0' && (c) <= '9' )



char	*svolume = NULL,
	*sdata   = NULL,
	*serie = NULL,
	*produto = NULL,
	nome [ MAXNOME  + 1 ],
	cpu  [ MAXCPU   + 1 ],
	buf  [ MAXTAG   + 1 ];
#ifdef	UNIX
char	*modo_abertura = "r+";
#else
char	*modo_abertura = "r+b";
#endif
int	tamanho,
	check,
	c = 1;

int	silencio = 0;
int	filetotal = 0;
long	volume   = 0,
	posicao  = 0,
	qtde     = MAXPROD;
long	atol();

FILE	*fp;

	/* Esquema temporario para solucionar o problema
	 * de personalizacao do modulo im.ovl com
         * VOLUME = -1 (Reginaldo)
	 */

int 	achei_o_im = 0;

main( argc, argv )
int argc;
char **argv;
{

	int	i;

	INI_DEBUG( &argc, &argv, PERS_DBGARQ );
	opcoes( &argc, &argv );

	for( i = 1; i < argc; ++i ) {
		if( faca( argv[i] ) < 0 ) {
			exit( 1 );
		}
		posicao = 0;
	}
	exit( 0 );

	return ( 0 );
}

faca( arq )
char	*arq;
{

	int	nao_achou = 1;

	if( ( fp = fopen( arq, modo_abertura ) ) == NULL ) {
		fprintf( stderr, "ischeck: nao pode abrir '%s'\n",
			arq );
		return -1;
	}

	if( posicao != 0L ) {
		/* o usuario forneceu a posicao inicial */
		if( fseek( fp, posicao, 0 ) == -1 ) {
			fprintf( stderr, "ischeck: erro de fseek\n" );
			return -1;
		}
	}

	/*
	 * vamos procurar a posicao da marca
	 */
	tamanho = strlen( TAG( is_pers ) );
	while( 1 ){
		while( ( c = getc( fp ) ) != TAG( is_pers )[ 0 ] &&
		    c != EOF ){

			/* Esquema temporario para solucionar o problema
	 		* de personalizacao do modulo im.ovl com
         		* VOLUME = -1 (Reginaldo)
	 		*/

			if ( produto != NULL && !strcmp(produto,"iw") && !achei_o_im ) procura_im();
		}
		if( c != TAG( is_pers )[ 0 ] ) {
			if( nao_achou ) {
				fprintf( stderr, "ischeck: nao sei onde posicionar\n" );
				return( 1 );
			} else {
				return 0;
			}
		}
		ungetc( c, fp );

		posicao = ftell( fp );

		if( fread( buf, MAXTAG, 1, fp ) != 1 ) {
			fprintf( stderr, "ischeck: erro de leitura\n" );
			return -1;
		}

		if( strncmp( buf, TAG( is_pers ), tamanho ) == 0 ) {
			DPRINTF( (D_PERS, "Marca encontrada = <%s>", buf ) );
			if( ! silencio ) {
				printf( "ischeck: posicao: %ld\n", posicao );
			}
			if( fseek( fp, posicao, 0 ) == -1 ) {
				fprintf( stderr, "ischeck: erro de fseek\n" );
				return -1;
			}
			grava_estr();
			nao_achou = 0;
			if ( filetotal == 0 ) {
				break;
			}

		}

		if( fseek( fp, posicao + 1, 0 ) == -1 ) {
			fprintf( stderr, "ischeck: erro de fseek\n" );
			return -1;
		}
	}
	fclose( fp );
	return 0;
}

sintaxe()
{

	extern char	*_verpers;

	fprintf( stderr, "\nischeck - V %s ** Utilitario Interno\n\n", _verpers );
	fprintf( stderr, "Sintaxe: ischeck [-s] [-nPERS] [-dAAMMDD] [-pPOS] [-vVOL] [-t] [-Sserie] \n         [-PPRODUTO] programa\n" );
	fprintf( stderr, "     onde:\n" );
	fprintf( stderr, "           -s modo silencioso \n" );
	fprintf( stderr, "           -d protege por data (AAMMDD) \n" );
	fprintf( stderr, "           -p inicia busca a partir da posicao POS \n" );
	fprintf( stderr, "           -n fornece PERS como string de personalizacao \n" );
	fprintf( stderr, "           -v protege com volume maximo = VOL \n" );
	fprintf( stderr, "           -t Procura varias marcas no arquivo\n");
	fprintf( stderr, "           -S Coloca o numero de serie no produto\n");
	fprintf( stderr, "           -P informa o PRODUTO a ser personalizado\n");
	exit( 1 );
	return ( 1 );
}


void tira_branco( s )
char *s;
{
	char		*sp;

	sp = s + strlen( s ) - 1;

	while( sp > s &&
	    ( *sp == ' ' ||
	    *sp == '\t' ||
	    *sp == '\n' ) ) {
		--sp;
	}

	*( sp + 1 ) = NULO;
}

void converte( s )
char *s;
{
	char		*p;

	for( p = s; *p; ++p ) {
		if( *p == '\016' ) {
			*p = *( p + 1 ) | 0200;
			strcpy( p + 1, p + 3 );
		}
	}
}



/****************************************************************************
 * opcoes                                                                   *
 * objetivo:                                                                *
 *	analizar as opcoes do comando e a sintaxe de chamada                     *
 * chamada por:                                                             *
 *	main                                                                     *
 * retorna:                                                                 *
 *	nada                                                                     *
 *	indiretamente, altera argc e argv para que a rotina processa()           *
 *	possa funcionar sem saber nada sobre as opcoes especificadas             *
 * algoritmo:                                                               *
 *	obvio. Altera argc e argv indiretamente                                  *
 ****************************************************************************/

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
			case 's':	/* trabalha em modo silencioso */
				silencio++;
				break;
			case 'v':	/* personaliza por volume	*/
				svolume = ++p;
				volume  = atol( svolume );
				goto prox_arg;
			case 'n':	/* nome do cliente */
				strncpy( nome, ++p, MAXNOME );
				*( nome + MAXNOME ) = NULO;
				goto prox_arg;
#ifdef DIGIX
			case 'c':	/* o numero da cpu  */
				strncpy( cpu, ++p, MAXCPU );
				*( nome + MAXCPU ) = NULO;
				goto prox_arg;
#endif /* DIGIX */
			case 'p':	/* posicao inicial de pesquisa 	*/
				posicao = atol( ++p );
				goto prox_arg;
			case 'd':	/* personaliza por data		*/
				sdata = ++p;
				if( ! valid_date( sdata ) ){
					fprintf( stderr, "ischeck: Data invalida \n" );
					exit( 1 );
					}
				goto prox_arg;
			case 'N':
				qtde = atoi (++p);
				goto prox_arg;
			case 't':
				filetotal++;
				goto prox_arg;
			case 'S':
				serie = ++p;
				goto prox_arg;
			case 'P':
				produto = ++p;
;				goto prox_arg;
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

	if( argc == 1 ){
		sintaxe();
		}

	/*
	 * devolve os novos valores de argc e argv para o beneficio
	 * de inicializa() e de processa()
	 */
	*argcp = argc;
	*argvp = argv;

	return ( 1 );
}

grava_estr(){

	/*-------------------------------------- nome -----*/
	if( ! *nome ) {
		/* o numero de serie nao foi fornecido
		 * na linha de comando
		 */
		if( ! silencio && isatty( 0 ) ) {
			printf( "Digite o nome do cliente: \n" );
			}
		gets( nome );
		}
	tira_branco( nome );
	converte( nome );
	NOME( is_pers )[ 0 ] = NULO;
	if( strlen( nome ) != 0 ) {
		strcpy( NOME( is_pers ), nome );
		}
	DPRINTF( ( D_PERS, "NOME( is_pers ) = <%s>", NOME( is_pers ) ) );


#ifdef DIGIX
	/*-------------------------------------- cpu -----*/
	if( ! *cpu ) {
		if( ! silencio && isatty( 0 ) ) {
			printf( "Digite numero da CPU:\n" );
			}
		gets( cpu );
		}
	tira_branco( cpu );
	CPU( is_pers )[ 0 ] = NULO;
	if( strlen( cpu ) != 0 ){
		strcpy( CPU( is_pers ), cpu );
		}
	DPRINTF( ( D_PERS, "CPU( is_pers ) = <%s>", CPU( is_pers ) ) );
#endif


	/*-------------------------------------- volume ---*/

	VOLUME( is_pers )[ 0 ] = NULO;
	if( svolume != NULO ) {
		strncpy( VOLUME( is_pers ), svolume, MAXVOL );
		*( VOLUME( is_pers ) + MAXVOL ) = NULO;
	}
	DPRINTF( (D_PERS, "VOLUME( is_pers ) = <%s>", VOLUME( is_pers ) ) );

	/* Esquema temporario para solucionar o problema
	 * de personalizacao do modulo im.ovl com
         * VOLUME = -1
	 */

	if ( achei_o_im == 1 ) {
		printf("Achei im.ovl vou fazer volume = -1 \n");
		strncpy( VOLUME(is_pers), "-1", MAXVOL );
		achei_o_im++;
	}

	/*-------------------------------------- data  -----*/
	DATA( is_pers )[ 0 ] = NULO;
	if( sdata != NULO ) {
		strncpy( DATA( is_pers ), sdata, MAXDATA );
		*( DATA( is_pers ) + MAXDATA ) = NULO;
	}
	DPRINTF( (D_PERS, "DATA( is_pers ) = <%s>", DATA( is_pers ) ) );


	if( serie == NULL ) {
		SERIE( is_pers )[ 0 ] = NULO;
	} else {
		strcpy( SERIE( is_pers ), serie );
	}
	MAXTTY( is_pers )[ 0 ] = NULO;
	check = criptografa( NOME( is_pers ), CHECKSUM( is_pers ) );
	DPRINTF( ( D_PERS, "\nCriptografia = <%s>", NOME( is_pers ) ) );
	DPRINTF( ( D_PERS, "Checksum = <%d>\n", check ) );

	CHECKSUM( is_pers )[ 0 ] = ( check & 0xFF00 ) >> 8;
	CHECKSUM( is_pers )[ 1 ] = ( check & 0x00FF );

	PERS( is_pers ) = PE_PERS;  /* indica que estah personalizado a partir de agora */
	if( fwrite( (char *) is_pers, sizeof is_pers, 1, fp ) != 1 ) {
		fprintf( stderr, "ischeck: erro de gravacao\n" );
		exit( 1 );
	}
   return ( 1 );
}

procura_im()
{

	/* Esquema temporario para solucionar o problema
	 * de personalizacao do modulo im.ovl com
         * VOLUME = -1
	 */

	if ( c ==  '.' ) {
		posicao = ftell( fp );
		fread( buf, 7, 1, fp );
		if( strncmp( buf, "/im.ovl", 7 ) == 0 ) {
			achei_o_im = 1 ;
		}
		fseek( fp, posicao, 0 );
	}
	return ( 1 );
}
