/*
 *	Low Level Library Interface #2
 *
 */

#include "ctstdr.h"		/* standard i/o header 		*/
#include  <stdlib.h>

extern COUNT uerr_cod;

#ifdef CTDEBUG

#ifdef RTREE
#define ALIMIT 1000
#else
#define ALIMIT 50
#endif

LOCAL struct {
	TEXT	   *aadr;
	UCOUNT	 aamt;
} a[ALIMIT];
LOCAL COUNT	acnt,anul;
LOCAL LONG	atot,amax,alev;
#endif



#ifdef NO_STD
	void *calloc();
   void *free();
   void exit();
#endif



TEXT *mballc(numobj, sizobj)
unsigned int numobj, sizobj;
{

	if (sizobj == 0 || numobj == 0) {
		numobj = 1;
		sizobj = 1; /* to avoid false NULL return */
	}

#ifdef CTDEBUG
	if (acnt >= ALIMIT) {
		printf("\nError ALIMIT. atot=%ld\n",atot);
		exit(2);
	}

	atot += a[acnt].aamt = numobj * sizobj;
	alev += a[acnt].aamt;
	if (alev > amax)
		amax = alev;
#ifdef CT_ANSI
	return(a[acnt++].aadr = (TEXT *) calloc(numobj,sizobj));
#else
	return(a[acnt++].aadr = calloc(numobj,sizobj));
#endif
#else
#ifdef CT_ANSI
	return((TEXT *) calloc(numobj,sizobj));
#else
	return( calloc(numobj,sizobj) );
#endif
#endif
}

/* ------------------------------------------------------------ */

VOID mbfree(objptr)
VOID       *objptr;
{
#ifdef CTDEBUG
	FAST COUNT i;

	if (objptr != NULL) {
		free(objptr);
		for (i = acnt - 1; i >= 0; i--)
			if (objptr == a[i].aadr) {
				a[i].aadr = NULL;
				alev -= a[i].aamt;
				return;
			}
		printf("\nError free. atot=%ld acnt=%d objptr=%x\n",
			atot,acnt,objptr);
	} else
		anul++;
#else
	if (objptr != NULL)
		free(objptr);
#endif
}

#ifdef CTDEBUG
VOID arep(mode)
COUNT	  mode;
{
	FAST COUNT i;

	printf("\nacnt = %d  anul = %d\n",acnt,anul);
	printf("\natot = %ld amax=%ld alev=%ld",atot,amax,alev);
	if (mode > 0) {
		for (i = 0; i < acnt; i++)
			if (a[i].aadr != NULL)
				printf("adr = %4x\tamt = %u\n",
					a[i].aadr,a[i].aamt);
	}
}
#endif

/* --------------------------------------------------------------------
   routine to copy tree buffer contents. it does not terminate on null
   byte.
 */

#ifndef cpybuf
VOID cpybuf(dp, sp, n)
PFAST TEXT *dp,*sp;
PFAST UCOUNT n;

{
	while (n-- != 0)
		*dp++ = *sp++;
}
#endif

COUNT uerr(err_no)
COUNT err_no;
{
	return(uerr_cod = err_no);
}

VOID terr(err_no)
COUNT err_no;
{
	printf("\nErro interno numero : %d.",err_no);
	exit(0);
}

#ifdef UNIFRMAT

VOID revbyt(tp, wrdcnt)
PFAST TEXT *tp;
PFAST COUNT    wrdcnt;
{
	TEXT ch;

	while (wrdcnt-- > 0) {
		ch	  = *tp++;
		*(tp - 1) = *tp;
		*tp++	  = ch;
	}
}

VOID revobj(tp,len)
PFAST TEXT *tp;
COUNT	       len;
{
	FAST TEXT *hp;
	TEXT	   ch;

	hp = tp + len - 1;
	while (tp < hp) {
		ch    = *tp;
		*tp++ = *hp;
		*hp-- = ch;
	}
}

VOID revwrd(tp,wrdcnt)
PFAST TEXT *tp;
PFAST COUNT    wrdcnt;
{	/* assumes that revbyt alraedy called */
	TEXT ch1,ch2;

	while (wrdcnt-- > 0) {
		ch1	  = *tp++;
		ch2	  = *tp++;
		*(tp - 2) = *tp;
		*tp++	  = ch1;
		*(tp - 2) = *tp;
		*tp++	  = ch2;
	}
}

#endif


/* end of ctclb2.c */
