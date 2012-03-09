#define		MAXPROD         99

#define		MAXTAG		40
#define		MAXNOME		80
#define		MAXSERIE	11
#define		MAXCPU		16
#define		MAXVOL		8
#define		MAXDATA		8
#define		MAXMAXTTY	7
#define		MAXCHECK	2 

/* definicoes dos offsets dos membros do buffer de personalizacao */
#define	O_TAG		0
#define	O_NOME		(O_TAG+MAXTAG+1)
#define	O_SERIE		(O_NOME+MAXNOME+1)
#define	O_CPU		(O_SERIE+MAXSERIE+1)
#define	O_VOLUME	(O_CPU+MAXCPU+1)
#define	O_DATA		(O_VOLUME+MAXVOL+1)
#define	O_MAXTTY	(O_DATA+MAXDATA+1)
#define	O_CHECKSUM	(O_MAXTTY+MAXMAXTTY+1)
#define	O_PERS		(O_CHECKSUM+MAXCHECK)	/* nao tem +1 */

#define	TAG(x)		((x) + O_TAG)
#define	NOME(x)		((x) + O_NOME)
#define	SERIE(x)	((x) + O_SERIE)
#define	CPU(x)		((x) + O_CPU)
#define	VOLUME(x)	((x) + O_VOLUME)
#define	DATA(x)		((x) + O_DATA)
#define	MAXTTY(x)	((x) + O_MAXTTY)
#define	CHECKSUM(x)	((x) + O_CHECKSUM)
#define	PERS(x)		(x[O_PERS])

#define MSG_TAG1	"General failure while open files"
#define	MSG_TAG2	"Memory allocation error"

typedef char	T_PERS[ O_PERS + 1 ];
