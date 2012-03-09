/*
 *	index file compare routine
 *
 */

#include "ctstdr.h"		/* standard i/o header 		*/

#define NO_FPLIB		/* do not use floating point lib for compar */

#ifdef REVBIN
#undef REVBIN
#endif
#ifdef LOW_HIGH
#define REVBIN
#endif
#ifdef HIGH_LOW
#ifdef UNIFRMAT
#define REVBIN
#endif
#endif

/* --------------------------------------------------------------------
   routine to compar key values

   compar returns a value < 0  for *val1 < *val2
                          = 0  for *val1 = *val2
                          > 0  for *val1 > *val2
 */

COUNT compar(val1, val2, tamanho)
PFAST UTEXT *val1,*val2;
COUNT tamanho;
{
	FAST COUNT i=0;

   while (i++ < tamanho && *val1++ == *val2++ )
   	;


#ifdef DS
	tamanho = ((COUNT) *--val1 & CMPMSK) - ((COUNT) *--val2 & CMPMSK);
#else
	tamanho = (*--val1 & CMPMSK) - (*--val2 & CMPMSK);
#endif

	if (tamanho > 0)     		return(i);
	else if (tamanho < 0)		return(-i);
	else                 		return(0);

}

/* end of ctcomp.c */
