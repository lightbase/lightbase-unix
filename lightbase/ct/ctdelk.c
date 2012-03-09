/*
 *	delete key
 *
 *
 */

#include "ctstdr.h"		/* standard i/o header 		*/

COUNT uerr(),putnod();


/* --------------------------------------------------------------------
   utility routine to delete key value from leaf node. Note that to
   support concurrent node updates, underflowed nodes (i.e., nodes
   less than half-full are not removed from the tree.  A node may even
   become empty.  Such under utilized nodes will be reused when key
   values from the same range of values are added back into the index.

   If key values are systematically removed from one end of the index,
   and replacements are added to the other end of the index; it will be
   necessary to rebuild the index periodically to avoid wasted disk
   space.
 */

COUNT DELKEY(keyno, target, tam_chave)
COUNT keyno;
TEXT *target;
COUNT tam_chave;
{
	FAST KEYFILE *knum;
	LOCAL COUNT npoint;
	FAST TREEBUFF *buffer;
	LOCAL LONG    node;
	TEXT *idxval;

	LONG    nodpnt();
	POINTER drnpnt();
	COUNT   nodser(), tstupd();
	TREEBUFF *getnod();
	TREEBUFF *movrgt();
	VOID	delexp();
	KEYFILE *tstfnm();

	uerr_cod = 0;

	/* veio de DELCKH */
   if ((knum = tstfnm(keyno)) == NULL)
		return(uerr_cod);

	cpybuf((idxval = ct_dupkey),target,knum->length);
   /* ate aqui */


	if (chkredf(knum))
		return(uerr_cod);

	if ( (node = gtroot(knum)) == (LONG) 0 ) /* tree may be empty or gtroot err */
		if (uerr_cod)
			return(uerr_cod);
		else
			return(uerr(KDEL_ERR));

	while (node) {	/* walk down or across tree until leaf node found */
		ct_lnode = node;
		if ((buffer = getnod(node,knum)) == NULL)
			return(uerr_cod);
		if (buffer->leaf == LEAF)
			break;

		if ((npoint = nodser(buffer, idxval, tam_chave, 'L')) != -1) {
			if (npoint == -2)
				terr(220);
			node = nodpnt(buffer,npoint);
		} else
			node = buffer->sucesr;
	}

	if (!node)	/* then no leaf node found */
		terr(221);

	if ((buffer = movrgt(idxval, tam_chave, knum, buffer)) == NULL)
		return(uerr_cod);

	if (ct_tky) 
		return(uerr(KDEL_ERR)); /* ct_key does not exist */

	if (tstupd(knum))
		return(uerr_cod);

	delexp(buffer);

	node = buffer->nodeid; /* save for unlock */
	if (putnod(buffer,buffer->nkv) )
		return(uerr_cod);
	return(NO_ERROR);
}


VOID delexp(bp)
PFAST TREEBUFF *bp;
{
	COUNT      expcnt, sactlen,sbegbyt;
	COUNT      poff;
	COUNT      tot2, comun, comun2;

	FAST TEXT *tp;

	TEXT      *valpnt();

	if (bp->lstpos == bp->nkv) {	/* deleting last entry */
		bp->nkv--;
		bp->nkb   -= bp->actlen;
		bp->lstpos = bp->actlen = bp->begbyt = 0;
      bp->len_exp = 0; /* lb */
		return;
	}

	tp     = bp->ct_kyval + bp->begbyt;


	if (bp->confg & REGULAR) poff = sizeof(POINTER);
	else                     poff = 0;


   comun = *(tp + poff + 1) & 0x00ff;

   tot2   = *(tp + poff + bp->actlen) & 0x00ff;
	comun2 = *(tp + poff + bp->actlen + 1) & 0x00ff;

	sactlen = bp->actlen; /* os bytes de ponteiro qdo REGULAR esta incluido */
	sbegbyt = bp->begbyt;


	valpnt(bp, bp->lstpos + 1);

	if ( comun2 <= comun ) /* applies to no prefix case as well */

		shflft(expcnt = sactlen,  bp,   sbegbyt+sactlen );

	else { /* must expand some or all of pfxct2 bytes */

		expcnt = sactlen - (comun2 - comun);

		if (poff) {
			cpybuf(tp, tp + sactlen, poff); /* move o endereco do prox. NO' */
      }

      *(tp + poff) = tot2 + (comun2 - comun);

		if (expcnt > 0)
			shflft(expcnt, bp, sbegbyt + sactlen + poff + 2);
		else { /* right shift required!!!! */
			terr(238);
		}

		bp->actlen += (comun2 - comun);
	}

	bp->nkb   -= expcnt;
	bp->begbyt = sbegbyt;
	bp->nkv--;
	bp->lstpos--;
}

/* end of ctdelk.c */
