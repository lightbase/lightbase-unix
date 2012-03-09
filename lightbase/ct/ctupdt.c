/*
 *	update support routines (used in ctaddk & ctdelk)
 *
 */

#include "ctstdr.h"		/* standard i/o header 		*/

/*
 * To disable the index entry count as described in Section 6.2,
 * add the following #define to the CTOPTN.H module:
 *
 *	#define NO_IDXENT
 *
 */



/* --------------------------------------------------------------------
   routine to update node size and note that node has been updated
 */

COUNT putnod(buf,nodsiz)

PFAST TREEBUFF *buf;
PFAST COUNT     nodsiz;

{
	VOID  inracs();
	COUNT wrtnod();

	if (nodsiz < 0)
		terr(216);
	inracs(buf);
	buf->update = 'y';
	buf->nkv = nodsiz;

	return(NO_ERROR);
}


/* ---------------------------------------------------------------------
   setup duplicate ct_key entry for ADDKEY & DELKEY
 */


/* --------------------------------------------------------------------
   move right with left-to-right locking
*/

TREEBUFF *movrgt(idxval, tam_chave, knum, buffer)

TEXT *idxval; /* pointer to target key value */
COUNT tam_chave;
PFAST KEYFILE *knum; /* key number pointer */
PFAST TREEBUFF *buffer; /* pointer to buffer containing leaf node */
{
	LOCAL LONG    node;

	COUNT nodser();
	TREEBUFF *getnod();
						/* NOTE: nodser sets ct_elm */
	while (buffer != NULL && nodser(buffer, idxval, tam_chave, 'L') == -1) {
		buffer = getnod(node,knum);
	}
	return(buffer);
}

/* shift right */

VOID shfrgt(n, bp, strbyt)
PFAST COUNT n;
TREEBUFF     *bp;
UCOUNT           strbyt;
{
	FAST TEXT *dp,*sp;

	sp = bp->ct_kyval + bp->nkb - 1;
	dp = sp + n;
	for (n = bp->nkb - strbyt; n-- > 0; ) /* strbyt org at 0, nkb org 1 */
		*dp-- = *sp--;	
}

/* shift left */

VOID shflft(n,bp,strbyt)
PFAST COUNT n;
TREEBUFF     *bp;
UCOUNT           strbyt;
{
	FAST TEXT *sp;

	sp = bp->ct_kyval + strbyt;
	cpybuf(sp - n,sp,bp->nkb - strbyt);
}

/* end of ctupdt.c */
