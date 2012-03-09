/*** INFOCON *** debug.h *** VERSAO 1.10 *** 89/10/12 ***/

#ifdef DEBUG


/*
 * niveis de debug
 */
#define	D_PERS	0	/* debug a nivel global  */

/*
 * nomes dos arquivos de saida dos debugs: 
 */
#define	PERS_STDERR	""
#define	PERS_DBGARQ	"debug"

 
#define	DPRINTF(x)	debug x
#define	INI_DEBUG(pargc,pargv,nome_arq)	ini_debug(pargc, pargv,nome_arq)


/*
 * estrutura de controle de nivel de debug
 */
struct debug {
	int	d_ligado;	/* este nivel esta ligado ou nao 	*/
	char	*d_nome;	/* nome do nivel ("PERS" ... ) 		*/
};

/*
 * a tabela de debug
 * existe uma estrutura para cada nivel de debug
 */
extern struct debug tabdebug[];

#else


/* nao quero debug */
#define	DPRINTF(x)
#define	INI_DEBUG(pargc, pargv,nome_arq)

#endif /* DEBUG */

