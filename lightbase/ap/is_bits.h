/*** INFOCON *** %M% *** VERSAO %R%.%L% *** %E% ***/

/*
 * macros para manipulacao de bits
 */

#define MASC(x) 	(0x80>>(x))	/* Mascara usada para selecionar o bit
					 * de ordem "x" de um byte ( da esquerda
					 * para direita ).
					 */
#define	BIT(x,n)		((x)[(n)/8] & MASC((n) % 8))
#define	LIGA_BIT(x,n)		(x)[(n)/8] |= MASC((n) % 8)
#define	DESLIGA_BIT(x,n)	(x)[(n)/8] &= ~MASC((n) % 8)
#define INVERTE_BIT(x,n)	(x)[(n)/8] ^= MASC((n) % 8)

/* equivalente a ceiling( n / lim ) */
#define	TETO( n, lim )	(((n) + (lim)-1) / (lim))
