#include	<stdio.h>
#include	<ctype.h>
#include	<is_bits.h>

/*
 * filtro generico mapeador de caracteres de sequencias
 * Sintaxe: filtro comandos < arqin >arqout
 * onde os comandos indicam que tipo de mapeamento fazer.
 * A sintaxe eh um comando de mapeiamento por linha.
 * Linhas vazias e iniciando com # sao ignoradas.
 * A linha eh do tipo:
 *	MAP<TAB>entrada<TAB>saida
 * (os TABS sao importantes. Tem que ter um so. Eh chato
 * mas permite simplificar muito as outras coisas.
 * O que for indicado pela sequencia de entrada eh mapeiado
 * pelo que esta indicado na sequencia de saida.
 * a sequencia de entrada tem o formato:
 *	sequencia_inicial,caractere,sequencia_final
 * Um caractere de uma sequencia eh informado em ascii ou
 * em octal com a sintaxe \nnn. \\ significa \.
 * Uma ou ambas as sequencias podem ser nulas.
 * O caractere \000 pode ser usado nas sequencias.
 * Se o caractere for nulo, significa qualquer caractere.
 * A sequencia de saida tem a mesma sintaxe explicada acima mais
 * as seguintes coisas especiais:
 * Pode conter \% que eh substituido pelo caractere entre virgulas
 * da parte de entrada. A sequencia \& eh igual mas o oitavo
 * bit do caractere eh ligado.
 * Finalmente, a sequencia \tnc indica o mapeamento do caractere
 * c pela tabela de numero n. As tabelas sao declaradas
 * no inicio com a sintaxe TABELA<TAB>arquivo. A primeira tabela
 * eh de numero 0. O arquivop deve conter 256 bytes e eh um mapa
 * de caracteres de 8 bits.
 *
 * No mapeiamento da entrada, o primeiro mapa aplicavel
 * eh usado e so este. Portanto a ordem das diretivas MAP
 * podem ser importantes.
 *
 * Exemplos podem ser vistos no front-end fiw.sh que faz
 * varias conversoes do formato infoword.
 *
 */

#ifdef DEBUG
#define	DPRINTF(x)	debug x
#else
#define	DPRINTF(x)
#endif

#define	MAX_TABELAS		20
#define	MAX_MAPAS		200
#define	MAX_NOME_TABELA		32
#define	MAX_TAM_TABELA		256
#define	MAX_MAP_SEQUENCIA	20
#define	MAX_BUF_LEITURA		1000	/* so posso casar sequencias ate este tamanho devido ao algoritmo casa_mapa() */
#define	QUALQUER_CARACTERE	0

typedef struct tabela {
	unsigned char	t_tabela[ MAX_TAM_TABELA ];
} TABELA;

typedef struct map {
	unsigned char	m_seq_inicial[ MAX_MAP_SEQUENCIA ];
	int		m_tam_inicial;
	unsigned char	m_seq_final[ MAX_MAP_SEQUENCIA ];
	int		m_tam_final;
	unsigned char	m_seq_saida[ MAX_MAP_SEQUENCIA ];
	unsigned char	m_caractere;
} MAP;

TABELA		*tabelas[ MAX_TABELAS + 1 ];
MAP		*mapas[ MAX_MAPAS + 1 ];
unsigned char	tem_mapa[ TETO( MAX_TAM_TABELA, 8 ) ];

main( argc, argv )
int	argc;
char	*argv[];
{

	int	c;

	if( argc != 2 ) {
		erro( "Sintaxe: filtro especificacao\n" );
	}
	if( pega_espec( argv[1] ) < 0 ) {
		exit( 1 );
	}
	while( (c = prox_char()) != EOF ) {
		mapeia( c );
	}
}

erro( formato, p1, p2 )
char	*formato;
char	*p1;
char	*p2;
{

	fprintf( stderr, formato, p1, p2 );
	exit( 1 );
}

pega_espec( espec )
char	*espec;
{

	char	*strchr();
	char	*campo();
	TABELA	*nova_tabela();
	MAP	*novo_mapa();
	char	temp[ 100 ];
	TABELA	*tab;
	MAP	*map;
	FILE	*fpt;
	char	*p;
	char	*np;
	int	i;
	int	tam;

	p = espec;
	while( *p != '\0' ) {
		/* pula brancos iniciais */
		while( strchr( " \t\n", *p ) != NULL ) {
			++p;
		}
		/* pula comentarios */
		if( strchr( "#", *p ) != NULL ) {
			if( (p = strchr( p, '\n' )) == NULL ) {
				/* acabou especificacao */
				break;
			}
			++p;
			continue;
		}
		np = campo( temp, p, sizeof temp, '\t', &tam );
		if( strcmp( temp, "TABELA" ) == 0 ) {
			if( (tab = nova_tabela()) == NULL ) {
				erro( "Tem mais do que %d  tabelas\n", MAX_TABELAS );
			}
			p = np;
			np = campo( temp, p, sizeof( temp ), '\n', &tam );
			if( (fpt = fopen( temp, "r" )) == NULL ) {
				erro( "Nao pode abrir tabela <%s>\n", temp );
			}
			if( fread( tab->t_tabela, sizeof( tab->t_tabela ), 1, fpt ) != 1 ) {
				erro( "Tabela <%s> incompleta\n", temp );
			}
			fclose( fpt );
			p = np;
		} else if( strcmp( temp, "MAP" ) == 0 ) {
			if( (map = novo_mapa()) == NULL ) {
				erro( "Tem mais do que %d mapeamentos\n", MAX_MAPAS );
			}
			p = np;
			np = campo( map->m_seq_inicial, p, sizeof( map->m_seq_inicial ), ',', &map->m_tam_inicial );
			DPRINTF( ("MAP: seq_inicial <%s>\n", map->m_seq_inicial ) );
			if( map->m_tam_inicial > 0 ) {
				LIGA_BIT( tem_mapa, map->m_seq_inicial[ 0 ] );
			}
			p = np;
			np = campo( temp, p, sizeof( temp ), ',', &tam );
			map->m_caractere = temp[0];
			if( map->m_tam_inicial == 0 ) {
				if( map->m_caractere == QUALQUER_CARACTERE ) {
					for( i = 0; i < MAX_TAM_TABELA; ++i ) {
						LIGA_BIT( tem_mapa, i );
					}
				} else {
					LIGA_BIT( tem_mapa, map->m_caractere );
				}
			}
			DPRINTF( ("MAP: caractere <%c> (%d)\n", map->m_caractere, map->m_caractere ) );
			p = np;
			np = campo( map->m_seq_final, p, sizeof( map->m_seq_final ), '\t', &map->m_tam_final );
			DPRINTF( ("MAP: seq_final <%s>\n", map->m_seq_final ));
			if( map->m_tam_final > 0 ) {
				LIGA_BIT( tem_mapa, map->m_seq_final[ 0 ] );
			}
			p = np;
			np = campo( map->m_seq_saida, p, sizeof( map->m_seq_saida ), '\n', &tam );
			DPRINTF( ("MAP: seq_saida <%s>\n", map->m_seq_saida ));
			p = np;
		} else {
			erro( "comando desconhecido <%s>\n", p );
		}
	}
	for( i = 0; i < MAX_MAPAS && mapas[ i ] != NULL; ++i ) {
		map = mapas[ i ];
		DPRINTF( ("mapa %d: ini <%s> carac %d final <%s> saida <%s>\n", i, map->m_seq_inicial, map->m_caractere, map->m_seq_final, map->m_seq_saida ));
	}
	return( 0 );	/* tudo ok */
}

/*
 * funcao que faz o parse parcial de uma linha
 * para localizar um campo e dizer onde esta o proximo campo
 */
char *
campo( dest, fonte, tam, separador, ptam )
char	*dest;
char	*fonte;
int	tam;
int	separador;
int	*ptam;
{

	char	*p;

	p = fonte;
	*ptam = 0;
	/* procura o fim do campo (termina com separador) */
	while( *p != '\0' && *p != separador ) {
		if( p[0] == '\\' && isdigit( p[1] ) && isdigit( p[2] ) && isdigit( p[3] ) ) {
			/* conversao octal */
			if( tam > 1 ) {
				*dest++ = (p[1] - '0') * 64 + (p[2] - '0') * 8 + (p[3] - '0');
				++(*ptam);
				--tam;
			}
			p += 4;
		} else {
			if( tam > 1 ) {
				*dest++ = *p;
				++(*ptam);
				--tam;
			}
			++p;
		}
	}
	/* o campo deve ter terminado como desejado, num separador */
	if( *p != separador ) {
		erro( "Erro de sintaxe caractere %c\n", *p );
		return( NULL );
	}
	*dest = '\0';
	return( p + 1 );
}

TABELA *
nova_tabela()
{

	void	*memoria();
	int	i;

	for( i = 0; i < MAX_TABELAS && tabelas[ i ] != NULL ; ++i ) {
	}
	if( i >= MAX_TABELAS ) {
		return( NULL );
	}
	tabelas[ i ] = (TABELA *)memoria( sizeof( TABELA ) );
	return( tabelas[ i ] );
}

MAP *
novo_mapa()
{

	void	*memoria();
	int	i;

	for( i = 0; i < MAX_MAPAS && mapas[ i ] != NULL ; ++i ) {
	}
	if( i >= MAX_MAPAS ) {
		return( NULL );
	}
	mapas[ i ] = (MAP *)memoria( sizeof( MAP ) );
	return( mapas[ i ] );
}

void *
memoria( n )
int	n;
{

	char	*calloc();
	char	*p;

	if( (p = calloc( n, 1 )) == NULL ) {
		erro( "Falta memoria\n" );
	}
	return( p );
}

mapeia( c )
int	c;
{

	MAP	*map;
	int	i;

	DPRINTF( ("mapeia caractere %c (%d)\n", c, c ));
	if( BIT( tem_mapa, c ) ) {
		/* se quiser melhorar o desempenho, tem que mexer
		 * primeiro aqui: a procura de que mapa aplicar
		 * eh sequencial. Poderia ter uma lista encadeada de mapas
		 * que se aplicam a cada caractere possivel de entrada.
		 */
		for( i = 0; mapas[ i ] != NULL; ++i ) {
			map = mapas[ i ];
			/* tenha certeza que o buffer de leitura tem informacao suficiente para casar as sequencias */
			if( enche_buffer( map->m_tam_inicial + 1 + map->m_tam_final ) < 0 ) {
				/* impossivel casar a sequencia */
				continue;
			}
			DPRINTF( ("tentando mapa %d\n", i ));
			if( casa_mapa( map ) ) {
				DPRINTF( ("mapeiou pelo mapa %d\n", i ));
				poe_seq( map );
				come_seq( map->m_tam_inicial + 1 + map->m_tam_final );
				return;
			}
		}
	}
	DPRINTF( ("nao mapeiou\n" ));
	come_seq( 1 );
	putchar( c );
}

unsigned char	buf_leitura[ MAX_BUF_LEITURA ];	/* entrada eh lida aqui dentro */
int		indice_atual;	/* indice do proximo caractere a ler */
int		indice_livre;	/* indice da primeira posicao livre do buffer */

casa_mapa( map )
MAP	*map;
{

	DPRINTF( ("casa_mapa: tam_inicial %d, tam_final %d\n", map->m_tam_inicial, map->m_tam_final ));
	DPRINTF( ("casa_mapa: compara entrada <%.*s> com <%s>\n", map->m_tam_inicial, &buf_leitura[ indice_atual ], map->m_seq_inicial ));
	DPRINTF( ("casa_mapa: compara caractere entrada %d com %d\n", buf_leitura[ indice_atual + map->m_tam_inicial ], map->m_caractere ));
	DPRINTF( ("casa_mapa: compara entrada <%.*s> com <%s>\n", map->m_tam_final, &buf_leitura[ indice_atual + map->m_tam_inicial + 1 ], map->m_seq_final ));
	return( memcmp( map->m_seq_inicial, &buf_leitura[ indice_atual ], map->m_tam_inicial )  == 0 &&
		(map->m_caractere == QUALQUER_CARACTERE || map->m_caractere == buf_leitura[ indice_atual + map->m_tam_inicial ] ) &&
		memcmp( map->m_seq_final, &buf_leitura[ indice_atual + map->m_tam_inicial + 1 ], map->m_tam_final ) == 0 );
}

prox_char()
{

	enche_buffer( 1 );
	return( indice_atual < indice_livre ? buf_leitura[ indice_atual ] : EOF );
}

enche_buffer( tam )
int	tam;
{

	/* tenha certeza que o buffer contem tam caracteres */
	if( indice_livre - indice_atual >= tam ) {
		return;	/* buffer contem informacao suficiente */
	}
	memcpy( buf_leitura, buf_leitura + indice_atual, indice_livre - indice_atual );
	indice_livre -= indice_atual;
	indice_atual = 0;
	indice_livre += fread( buf_leitura + indice_livre, 1, sizeof( buf_leitura ) - indice_livre, stdin );
}

come_seq( tam )
int	tam;
{

	/* retira informacao lida do buffer */
	if( indice_atual + tam > indice_livre ) {
		/* programacao defensiva */
		erro( "Tentou comer sequencia maior que esperada\n" );
	}
	indice_atual += tam;
}

poe_seq( map )
MAP	*map;
{

	unsigned char	*seq;
	int		c;

	seq = map->m_seq_saida;
	while( *seq ) {
		c = pega_char( &seq, map );
		putchar( c );
	}
}

pega_char( pseq, map )
unsigned char	**pseq;
MAP		*map;
{

	int	c;
	int	n;

	/* interpreta a sequencia
	 * a sintaxe da sequencia de saida eh um string de caracteres
	 * um caractere pode ser:
	 *	1. normal
	 *	2. sequencia octal \ooo (ja tratado na rotina campo())
	 *	3. \% (representando o caractere sendo casado)
	 *	4. \& (como \% mas liga o oitavo bit)
	 *	5. \tnc (onde c eh um caractere. Isto coloca para fora
	 *		tabela[ c ] onde tabela eh a tabela numero n
	 *	6. \\ (representando o caractere \)
	 */
	if( (c = **pseq) != '\0' ) {
		++(*pseq);
	}
	if( c == '\\' ) {
		if( (c = **pseq) != '\0' ) {
			++(*pseq);
		}
		switch( c ) {
		case '\\':
			return( '\\' );
		case '%':
			return( buf_leitura[ indice_atual + map->m_tam_inicial ] );
		case '&':
			return( buf_leitura[ indice_atual + map->m_tam_inicial ] | 0x80 );
		case 't':	/* tabela */
			n = **pseq;
			if( isdigit( n ) ) {
				n -= '0';	/* numero da tabela */
				++(*pseq);
				if( n < MAX_TABELAS && tabelas[ n ] != NULL ) {
					return( tabelas[ n ]->t_tabela[ pega_char( pseq, map ) ] );
				} else {
					erro( "Tabela %d invalida\n", n );
				}
			} else {
				erro( "Tabela %c invalida (nao numerica)\n", n );
			}
		}
	} else {
		/* caractere normal */
		return( c );
	}
}

debug( formato, p1, p2, p3, p4 )
char	*formato;
char	*p1, *p2, *p3, *p4;
{

	fprintf( stderr, formato, p1, p2, p3, p4 );
}
