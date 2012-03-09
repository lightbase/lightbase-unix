#include <winport.h>
#ifdef DEBUG
#define	DPRINTF(x)	debug x
#define	INI_DEBUG(pargc,pargv,nome_arq)	ini_debug(pargc, pargv,nome_arq)
#else
#define	DPRINTF(x)
#define	INI_DEBUG(pargc, pargv,nome_arq)
#endif

#ifdef SOX
typedef long	t_maq;			/* parametros tem tipos diferentes em certas chamadas de funcoes no SOX 	*/
#else
typedef int	t_maq;
#endif


#define	   TAM_TERM      3
#define	   TAM_DATA      6
#define		MAXPROD      99
#define		MAXDATA		  8
#define		PE_INSTALA	  2
#define		NULO		     '\0'
#define		MAXCPU		  16
#define	   D_PERS        0	/* debug a nivel global  */
#define		PE_DATA		  4
#define		PE_OK		     1
#define		PE_PERS		  '1'
#define		MAXTAG		  40
#define		MAXNOME		  80
#define		MAXSERIE	     11
#define		MAXCPU		  16
#define		MAXVOL		  8
#define		MAXMAXTTY	  7
#define		MAXCHECK	     2

#define	   O_TAG		     0
#define	   O_NOME		  (O_TAG + MAXTAG + 1)
#define	   O_SERIE	  	  (O_NOME + MAXNOME + 1)
#define	   O_CPU		     (O_SERIE + MAXSERIE + 1)
#define	   O_VOLUME	     (O_CPU + MAXCPU + 1)
#define	   O_DATA		  (O_VOLUME + MAXVOL + 1)
#define	   O_MAXTTY	     (O_DATA + MAXDATA + 1)
#define	   O_CHECKSUM	  (O_MAXTTY + MAXMAXTTY + 1)
#define	   O_PERS		  (O_CHECKSUM + MAXCHECK)	/* nao tem +1 */

#define	   NOME(x)		  ((x) + O_NOME)
#define	   SERIE(x)	     ((x) + O_SERIE)
#define	   DATA(x)		  ((x) + O_DATA)
#define	   CHECKSUM(x)	  ((x) + O_CHECKSUM)
#define	   VOLUME(x)	  ((x) + O_VOLUME)
#define	   MAXTTY(x)	  ((x) + O_MAXTTY)
#define	   PERS(x)		  (x [ O_PERS ])
#define	   CPU(x)		  ((x) + O_CPU)
#define	   TAG(x)		  ((x) + O_TAG)

#define	   PERS_DBGARQ	  "debug"

typedef char	T_PERS[ O_PERS + 1 ];

/*** INFOCON *** %M% *** VERSAO %I% *** %E% ***/

/*
 * para obtermos as versoes das bibliotecas, preciso
 * fazer com que os simbolos _ver* estejam indefinidos
 * ao chegar aa biblioteca desejada (infoscr, infogab, ...)
 * e antes de chegar a libinfocon (onde eu pegaria a versao
 * default chamada "?")
 *
 * As linhas que seguem servem so para forcar uma indefinicao
 */

/* extern char	*_vercon;
extern char	*_vergab;
extern char	*_verscr;
extern char	*_vermenu;
extern char	*_verhelp; */


/* extern char	*_vervar;
extern char	*_verprn; */

/* char	**_lixo1 = &_vercon;
char	**_lixo2 = &_vergab;
char	**_lixo3 = &_verscr;
char	**_lixo4 = &_vermenu;
char	**_lixo5 = &_verhelp; */


/* char	**_lixo7 = &_vervar;
char	**_lixo8 = &_verprn; */

#ifdef PROTO_ANSI

int pe_map ( void );
char *pe_pers ( void );
char *pe_serie ( void );
char *pe_data ( int *, int *, int * );
int pe_tty ( void );
void pe_x ( void );
int pe_teclou_x ( int, char ** );
int criptografa ( char *, char * );
int decriptografa ( char *, char * );
void crypt_key ( char *, char * );
int valid_date ( char * );
char *ttyconfere ( char *, char *, int );
char *dataconfere (  char *, char *, int );
void  incrdata ( char *, register int );
void istrncpy( register char *, char *, register int );

#else

int pe_map ( );
char *pe_pers ( );
char *pe_serie ( );
char *pe_data ( );
int pe_tty ( );
void pe_x ( );
int pe_teclou_x ( );
int criptografa ( );
int decriptografa ( );
void crypt_key ( );
int valid_date ( );
char *ttyconfere ( );
char *dataconfere (  );
void  incrdata ( );
void istrncpy ( );

#endif

