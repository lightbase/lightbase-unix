/*** INFOCON *** %M% *** VERSAO %R%.%L% *** %E% ***/


/* Este programa personaliza os produtos da infocon
 * durante sua instalacao. Os dados solicitados ao
 * usuario sao: NOME, NUMERO DE SERIE e ACTIVATION KEY.
 *
 * 1. O produto soh eh instalado se a Activation Key for correta.
 *
 */


#ifdef	SOX
#include	<stddef.h>
#endif	/* SOX */

#include	<stdio.h>
#include	"wprotec.h"

#ifndef UNIX
	#include <stdlib.h>
	#include <string.h>
	#include <io.h>

	int sintaxe ( void );
	int opcoes( int *, char **[] );
	int msg_maxtry ( void );
	FILE *localiza ( char *, T_PERS );
	void monta_pers ( T_PERS pers, char	*, char *, char *, char *, char * );

	#ifdef MUITO_LENTO
		int destrincha  ( char *, char *, char **, char ** );
   #endif

   int troca_nome = 0;
#endif


extern T_PERS is_pers;
extern T_PERS cl_pers;

char	*_verpers = "4.2";

#define	TAM_TERM 3	/* O tamanho da cadeia no numero de serie
							 * que representa o numero de terminais.
							 * Esta cadeia inicia no terceiro caractere
							 * do numero de serie.
							 */

#define TAM_DATA 6	/* tamanho de um string data sem o nulo (AAMMDD) */

#define	MAX_TRY	3	/* maximo de vezes permitido para digitacao
							 * de uma Activation Key errada. Se atingido,
							 * o produto nao eh instalado.
							 */

#ifdef PORTUGUES

#ifndef  UNIX
#define	MSG_SINTAX  	"Sintaxe: person [-Xstd] <prog> [prog ...] \n"
#else
#define	MSG_SINTAX  	"Sintaxe: person [-Xstd] <prog> [prog ...] \n"
#endif
#define	MSG_READERROR	"person: erro de leitura \n"
#define	MSG_WRITEERROR	"person: erro de gravacao \n"
#define	MSG_POSERROR	"person: erro de posicionamento \n"
#define	MSG_OPENERROR	"person: nao pode abrir '%s' \n"
#define	MSG_NOINFOCON	"person: '%s' nao eh um produto da INFOCON\n"
#define	MSG_SERIAL		"Entre com o numero de serie: \n"
#define	MSG_NAME			"Entre com nome do cliente: \n"
#define	MSG_KEY			"Entre com a chave de ativacao: \n"
#define	MSG_MAXTRY		"Maximo de tentativas ultrapassado. Produto nao instalado \n"
#define	MSG_TRYAGAIN	"Chave de Ativacao nao confere. Tente novamente. \n\n"
#define	MSG_PERS			"Personalizando '%s' ... \n"
#define	MSG_BADSERIAL	"Numero de serie nao confere.\n"
#define	MSG_NUMTTY		"Produto liberado para %d terminais\n"

#else

#ifndef  UNIX
#define	MSG_SINTAX		"Syntax: person [-Xstd] <prog> [prog ...]\n"
#else
#define	MSG_SINTAX		"Syntax: person [-Xstd] <prog> [prog ...]\n"
#endif
#define	MSG_READERROR	"person: read error \n"
#define	MSG_WRITEERROR	"person: write error \n"
#define	MSG_POSERROR	"person: file seek error \n"
#define	MSG_OPENERROR	"person: cannot open '%s' \n"
#define	MSG_NOINFOCON	"person: '%s' is not an INFOCON product \n"
#define	MSG_SERIAL		"Serial number: \n"
#define	MSG_NAME			"Licensee's name: \n"
#define	MSG_KEY			"Enter the Activation Key: \n"
#define	MSG_MAXTRY		"Too many tries. Product not installed.\n"
#define	MSG_TRYAGAIN	"Invalid Activation Key. Try again. \n\n"
#define	MSG_PERS			"Customizing '%s' ... \n"
#define	MSG_BADSERIAL	"Invalid Serial Number.\n"
#define	MSG_NUMTTY		"Product may be used with %d terminals\n"

#endif /* PORTUGUES */

#ifdef UNIX
char	*modo_abertura = "r+";
#else /* MSDOS */
char	*modo_abertura = "r+b";
#endif /* UNIX */

char	activ_key[ MAXNOME + 1 ];	/* para a leitura da Activation Key 	*/
char	cripserie[ MAXNOME + 1 ];	/* o numero de serie criptografado	*/
char	serie    [ MAXNOME + 1 ];	/* o numero de serie soliicitado	*/
char	nome     [ MAXNOME + 1 ];	/* o nome do cliente			*/
char	buf[ MAXTAG + 1 ];		/* guardar a marca (tag) para pesquisa 	*/

#ifdef MAURAO
#ifdef SOX
typedef long	t_maq;			/* parametros tem tipos diferentes em certas chamadas de funcoes no SOX 	*/
#else
typedef int	t_maq;
#endif
#endif

t_maq	size_tag = 0;								/* o tamanho da marca (tag)		*/
t_maq	size_serie = 0; 						/* o tamanho do numero de serie		*/
long 	posicao = 0;						/* lembra posicao do inicio da marca	*/
int	check;							/* temporario para obtencao do checksum	*/
int	c;										/* ler caracteres aa procura da marca	*/
int	silencio = 0;							/* trabalha em modo silencioso		*/
int	vertty = 0;							/* verifica o numero max. de terminais	*/
int	verdata = 0;							/* verifica data na activation key	*/
int	try = 0;										/* conta o numero de tentativas 	*/

char	*is_gets();
long	atol();
void	monta_pers();

#ifndef UNIX
void pede_nome ( int, char ** );
#else
void pede_nome ( );
#endif

main( argc, argv )
int argc;
char **argv;
{

	FILE	*localiza();
	char	*ttyconfere();
	char	*dataconfere();
	char	*ntty = "999";
	char	*data = "";
	register int prog;	/* indice para os nomes dos programas
				 * passados na linha de comando.
				 */
	FILE	*fp;		/* o programa a personalizar		*/

	INI_DEBUG( &argc, &argv, PERS_DBGARQ );
	opcoes( &argc, &argv );

	/* solicitar nome, numero de serie e a Activation Key.
	 */
	pede_nome ( argc, argv );

	/* solicita numero de serie a Activation Key
	 * (no maximo MAX_TRY tentativas )
	 */
tenta:
	while( (try++ < MAX_TRY) || msg_maxtry() ) {

		!silencio && isatty( 0 ) && printf( MSG_SERIAL );
		gets( serie );

		!silencio && isatty( 0 ) && printf( MSG_KEY );
		gets( activ_key );

		size_serie = (t_maq) strlen( serie );
		if( size_serie != MAXSERIE ) {
			printf( MSG_TRYAGAIN );
			continue;
		}

		DPRINTF( (D_PERS, "nome             = <%s>", nome ) );
		DPRINTF( (D_PERS, "serie            = <%s>", serie ) );
		DPRINTF( (D_PERS, "tam serie        = <%d>", size_serie ) );
		DPRINTF( (D_PERS, "chave fornecida  = <%s>", activ_key ) );
		DPRINTF( (D_PERS, "chave real       = <%s>", cripserie ) );
/*
		fprintf(stderr, "nome             = <%s>", nome ) ;
		fprintf(stderr, "serie            = <%s>", serie );
		fprintf(stderr, "tam serie        = <%d>", size_serie );
		fprintf(stderr, "chave fornecida  = <%s>", activ_key );
		fprintf(stderr, "chave real       = <%s>", cripserie ); */

		strcpy( cripserie, serie );
		crypt_key( cripserie, cripserie + MAXSERIE );
		if( strcmp( activ_key, cripserie ) == 0 ){
			/* a chave de ativacao fornecida bate com a do numero
			 * de serie fornecido (interno). Portanto, entendo que
			 * nao eh necessario procurar data e numero de
			 * terminais. Eles serao apagados.
			 */
			DPRINTF( (D_PERS, "chave bateu com a do numero de serie interno" ) );
			break;
		}

		DPRINTF( (D_PERS, "chave fornecida NAO bateu com a do numero de serie interno" ) );

		if( (ntty = ttyconfere( serie, activ_key, vertty )) != NULL ) {
			break; /* tentativa correta */
                } else {
                        ntty = "999";
		}

		if( (data = dataconfere( serie, activ_key, verdata )) != NULL ) {
			break; /* tentativa correta */
		}

		printf( MSG_TRYAGAIN );
		DPRINTF( (D_PERS, "Chave de ativacao NAO confere: <%s>", activ_key ) );
	} /* while try */

	for( prog = 1; prog < argc; prog++ ) {

		/* procura a estrutura is_pers
		 */
		if( (fp = localiza( argv[prog], is_pers )) == NULL ) {
			exit( 1 );
		}

		if( PERS( is_pers ) != PE_PERS ) {
			printf( MSG_BADSERIAL );
			exit( 1 );
		}

		/* ve se tem um numero de serie na estrutura do ischeck
		 */
		decriptografa( NOME( is_pers ), CHECKSUM( is_pers ) );

		if( strlen( SERIE( is_pers ) ) == MAXSERIE &&
		    strcmp( SERIE( is_pers ), serie ) != 0 ) {
			/* para produtos onde ischeck deixou
			 * um numero de serie, quero
			 * que o numero de serie esteja batendo
			 */
			printf( MSG_TRYAGAIN );
			DPRINTF( (D_PERS, "serie fornecida nao confere com a interna  <%s> <%s>", serie, SERIE( is_pers) ) );
			fclose( fp );
			goto tenta;
		}

		!silencio && isatty( 0 ) && printf( MSG_PERS, argv[ prog ] );

		/* A estrutura is_pers soh passou a ser modificada (gravada)
		 * depois da introducao da opcao "-d".
		 * A opcao -d tambem deve TIRAR a data, quando a activation
		 * key fornecida casa com a do numero de serie original.
		 */
		monta_pers( is_pers, NOME(is_pers), SERIE(is_pers), "", VOLUME(is_pers), data == NULL ? "" : data );

		if( fseek( fp, posicao, 0 ) == -1 ) {
			printf( MSG_POSERROR );
			exit( 1 );
		}
  		if( fwrite( (char *) is_pers, sizeof is_pers, 1, fp ) != 1 ) {
			printf( MSG_WRITEERROR );
			exit( 1 );
		}
		fclose( fp );

		/* agora trata da estrutura cl_pers */
		if( (fp = localiza( argv[prog], cl_pers )) == NULL ) {
			exit( 1 );
		}

		/* OK. Ja estou com a estrutura cl_pers ...
		 */
		if( PERS( cl_pers ) == PE_PERS ) {
			/* Jah passou o "person" por aqui antes.
			 * decriptografo a estrutura, para nao perder as
			 * demais informacoes nela contidas: data, volume ...
			 */
			decriptografa( NOME( cl_pers ), CHECKSUM( cl_pers ) );
		}

		/* copia na estrutura o nome, a serie e
		 * ajusta o numero maximo de terminais
		 */

#ifndef UNIX
		if (  strncmp (  NOME ( cl_pers ), "nome", 4  ) != 0 )
		{
			if ( troca_nome )
				monta_pers( cl_pers, nome, serie, ntty, "", "" );
			else
			{
				char nome_aux [ MAXNOME + 1 ];
				strncpy ( nome_aux, NOME ( cl_pers ), MAXNOME );
				monta_pers( cl_pers, nome_aux, serie, ntty, "", "" );
			}

		}
		else
			monta_pers( cl_pers, nome, serie, ntty, "", "" );
#else
		 monta_pers( cl_pers, nome, serie, ntty, "", "" );
#endif
		if( fseek( fp, posicao, 0 ) == -1 ) {
			printf( MSG_POSERROR );
			exit( 1 );
		}
  		if( fwrite( (char *) cl_pers, sizeof cl_pers, 1, fp ) != 1 ) {
			printf( MSG_WRITEERROR );
			exit( 1 );
		}
		DPRINTF( (D_PERS, "Personalizei '%s' ...", argv[ prog ] ) );
		fclose( fp );
	}
	if( atoi( ntty ) != 0 && atoi( ntty ) != 999 ) {
		printf( MSG_NUMTTY, atoi(ntty) );
	}
	exit( 0 );
	return ( 0 );
}

/*-----------------------------------------------------------------
 * sintaxe
 * objetivo:
 *	emitir mensagem de sintaxe do comando e
 *	terminar a execucao.
 */

sintaxe()
{
	fprintf( stderr, MSG_SINTAX );
	exit( 1 );
	return ( 1 );
}

/*-------------------------------------------------------------------
 * msg_maxtry
 * objetivo:
 *	avisar que o maximo de tentativas foi ultrapassado e
 *	terminar a execucao.
 */

msg_maxtry()
{
	printf( MSG_MAXTRY );
	exit( 1 );
	return ( 1 );
}

/*------------------------------------------------------------------
 * opcoes
 * objetivo:
 *	analizar as opcoes do comando e a sintaxe de chamada
 * retorna:
 *	nada
 *	indiretamente, altera argc e argv, retirando os argumentos.
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
#ifndef UNIX
			case 'c':
				troca_nome = 1;
				break;
#endif
			case 's':	/* trabalha em modo silencioso */
				silencio++;
				break;
			case 't':	/* verifica numero de terminais */
				vertty++;
				break;
			case 'd':	/* verifica data	*/
				verdata++;
				break;
			case 'X':
				fprintf(stderr,"Versao: %s\n",_verpers);
				exit(1);
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
	 */
	*argcp = argc;
	*argvp = argv;
	return ( 1 );
}

FILE *localiza( arq, pers )
char	*arq;
T_PERS	pers;
{

	FILE	*fp;

	strcpy( buf, TAG( pers ) );
	size_tag = (t_maq) strlen( buf );
	if( ( fp = fopen( arq, modo_abertura ) ) == NULL ) {
		printf( MSG_OPENERROR, arq );
		return NULL;
	}

	/* Vamos procurar a Marca da estrutura (TAG).
	 * Person nao aceita ajuda para posicionamento inicial
	 * na busca da marca (opcao -p do ischeck). Isso eh
	 * para evitar 'dar bandeira' sobre a posicao da marca de
	 * personalizacao. Por exemplo, seria arriscado colocar em
	 * um script de instalacao o comando "person -p120000 iw.ovl".
	 */
	while( 1 ) {
		while( ( c = getc( fp ) ) != buf[ 0 ] && c != EOF )
			;
		if( c != buf[ 0 ] ) {
			printf( MSG_NOINFOCON, arq );
			return NULL;
		}
		ungetc( c, fp );
		posicao = ftell( fp );
		if( fread( (char *) pers, sizeof (T_PERS), 1, fp ) != 1 ) {
			printf( MSG_READERROR );
			return NULL;
		}

		if( strncmp( buf, TAG( pers ), (t_maq) size_tag ) == 0 ) {
			DPRINTF( (D_PERS, "Marca encontrada = <%s>", buf ) );
			break;
		}

		/* um falso inicio de marca.
		 * Continuemos a busca ...
		 */
		if( fseek( fp, posicao + 1, 0 ) == -1 ) {
			printf( MSG_POSERROR );
			return NULL;
		}
	}
	return fp;
}

void monta_pers( pers, nome, serie, ntty, volume, data )
T_PERS	pers;
char	*nome;
char	*serie;
char	*ntty;
char	*volume;
char	*data;
{
	int	check;

	strncpy( NOME( pers ), nome, MAXNOME );
	strncpy( SERIE( pers ), serie, MAXSERIE );
/*
	strncpy( MAXTTY( pers ), ntty, TAM_TERM );
*/
	strcpy( MAXTTY( pers ), ntty );
	strncpy( VOLUME( pers ), volume, MAXVOL );
	strncpy( DATA( pers ), data, MAXDATA );
#ifdef DIGIX
	strncpy( CPU( pers ), cpu, MAXCPU );
#endif /* DIGIX */
	/* indica que o produto estah personalizado */
       	PERS( pers ) = PE_PERS;

	check = criptografa( NOME( pers ), CHECKSUM( pers ) );
	CHECKSUM( pers )[ 0 ] = ( check & 0xFF00 ) >> 8;
	CHECKSUM( pers )[ 1 ] = ( check & 0x00FF );
}

#ifdef MUITO_LENTO

/* obtem a data e o numero de terminais (ao mesmo tempo) a partir
 * do numero de serie e da ativation key fornecida.
 * os valores sao retornados nas variaveis 'data' e 'ntty'.
 * o numero de combinacoes a tentar eh grande demais !
 */
destrincha( serie, key, data, ntty )
char	*serie;
char	*key;
char	**data;
char	**ntty;
{
	int	n_data;
	int	n_tty;

	static char	tmpdata[TAM_DATA+1] = "910101";
	static char	tmptty[ 4 ];
	char		cripserie[ MAXSERIE + 1 ];
	char		buf[ MAXSERIE ];
	int		max_tty = 200;

	int		max_dias = 3285;	/* de 910101 ateh 991231 */
	void		incrdata();

	DPRINTF( (D_PERS, "checando data + tty" ) );
	/* como o algoritmo de geracao da chave de ativacao nao eh
	 * reversivel, tenho que encontrar a data com pesquisa exaustiva
	 */
  	size_serie = (t_maq) strlen( serie );
	for( n_data = 0; n_data < max_dias; n_data++ ) {	/* numero maximo dias a tentar (ateh 1999) */
		for( n_tty = 0; n_tty <= max_tty; n_tty++ ) {	/* maximo de terminais	*/
			strcpy( cripserie, serie );
			sprintf( cripserie, "%03x", n_data );
			sprintf( buf, "%03x", n_tty );
			memcpy( cripserie + 3, buf, 3 );

			DPRINTF( (D_PERS, "serie       = <%s>", cripserie ) );

			crypt_key( cripserie, cripserie + MAXSERIE );
			if( strcmp( activ_key, cripserie ) == 0 ){
				/* tudo ok */
				incrdata( tmpdata, n_data );
				*data = tmpdata;
				sprintf( tmptty, "%03d", n_tty );
				*ntty = tmptty;
				DPRINTF( (D_PERS, "data    = <%s>", *data ) );
				DPRINTF( (D_PERS, "tty     = <%s>", *ntty ) );
				return( 1 );
			}
		}
	}
	return( 0 );
}
#endif

void pede_nome ( argc, argv )
int argc;
char **argv;
{
	FILE *fp;
	int prog;

	for( prog = 1; prog < argc; prog++ )
	{

		if( (fp = localiza( argv[prog], cl_pers )) == NULL )
			exit( 1 );
		else
			fclose ( fp );

		/* OK. Ja estou com a estrutura cl_pers ...
		 */
		if( PERS( cl_pers ) == PE_PERS ) {
			/* Jah passou o "person" por aqui antes.
			 * decriptografo a estrutura, para nao perder as
			 * demais informacoes nela contidas: data, volume ...
			 */
			decriptografa( NOME( cl_pers ), CHECKSUM( cl_pers ) );
		}

		/* copia na estrutura o nome, a serie e
		 * ajusta o numero maximo de terminais
		 */

#ifndef UNIX
	if (  strncmp (  NOME ( cl_pers ), "nome", 4  ) == 0 || ( troca_nome ) )
	{
		!silencio && isatty( 0 ) && printf( MSG_NAME );
		gets ( nome );
	}
#else
	!silencio && isatty( 0 ) && printf( MSG_NAME );
	gets ( nome );
#endif

	}

	return;
}

