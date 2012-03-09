#define   TIPOS_LB    /* para evitar ser incluido mais de uma vez */

/* #define   LB_ANSI */

#ifdef LB_ANSI
   typedef FILE	       *RNDFILE;
#else
   typedef int		RNDFILE;
#endif

#define   T_CHAR   char
#define   T_INT    int
#define   T_SHORT  short
#define   T_LONG   long
#define   T_FLOAT   float
#define   T_DOUBLE  double


#define	FAST	register    	/* register variable	   */
#define  PFAST	register			/* register parameter	*/
#define	EXTERN	extern		/* External variable	   */
#define	GLOBAL	/* */			/* Global variable	   */
#define	LOCAL	static			/* Local to module	   */

#ifndef VOID
#define  VOID	void
#endif

#define  VOID_T  void         /* if void is not supported, change to char */

typedef          T_CHAR 	TEXT;
typedef unsigned T_CHAR 	UTEXT;	/* if un char not supported, make char */
typedef          T_CHAR   *TEXTPTR;

/*
** se compilador nao suportar usar T_INT
*/

typedef          T_SHORT	COUNT;   /* CTREE usa assim */

typedef unsigned T_SHORT	UCOUNT;  /* CTREE usa assim */

/*
**
*/

typedef          T_INT		WORD;    /* CTREE usa assim */
typedef unsigned T_SHORT   VRLEN;   /* CTREE usa +- assim */
/*
**
*/
typedef          T_LONG	   POINTER,LONG;  /* CTREE usa assim */
typedef          T_LONG	   DATET;         /* CTREE usa assim */
typedef unsigned T_LONG	   ULONG;         /* CTREE usa assim */

typedef          T_FLOAT   FLOAT;
typedef          T_DOUBLE   DOUBLE;
