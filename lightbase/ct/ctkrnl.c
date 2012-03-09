/*
 *	index file kernel
 *	Version 4.3 Release B
 *	May 12, 1988 17:30
 *
 */


#include "ctstdr.h"		/* standard i/o header 		*/
#include  "ctdefs.h"

#ifdef FPUTFGET
	COUNT redhdr();
#endif

/* --------------------------------------------------------------------
   general purpose node search function. works with leaf and non-leaf nodes.
   returns relative position in node of key value satisfying search request
   as well as setting the global variable ct_elm to this value. also sets
   the global variable ct_tky which indicates the sense of the comparison
   between the target key value (pointed to by idxval) and the key values
   actually in the index structure. if search is not satisfied, nodser
   returns a value of -2. if idxval greater than high key, then nodser
   returns a value of -1.
 */

COUNT nodser(buffer,idxval, tamanho, stratg)
PFAST TREEBUFF *buffer; /*pointer to buffer containing node */
TEXT *idxval; 		      /* pointer to target key value */
COUNT tamanho;
TEXT stratg; 		      /* search strategy: E == eq  L == le  S == le (from
			                * leaf node)
			               */
{
	COUNT compar();

	COUNT begser,endser, poff;
/*	FAST KEYFILE *knum; */

	COUNT	   n, implen, total, comun;
	FAST TEXT *ip,*tp;
	TEXT      *retval;

# ifdef DEBUGEST
extern ULONG Xnodserv;
Xnodserv++;
#endif


	begser  = 1;
	endser  = buffer->nkv;
	ct_tkp  = poff = 0;

/* high key comparison
*/
	if ((buffer->leaf == LEAF &&
		 (compar((UTEXT *)idxval,(UTEXT *)hghpnt(buffer), tamanho) > 0 ||
	    (stratg == 'S' && !endser && buffer->sucesr))) ||

	    (buffer->leaf == NOLEAF && /******* SAI !buffer->ktipe && ******/
		  compar((UTEXT *)idxval,(UTEXT *)expval(buffer,endser), tamanho) > 0) ) { /** ??? **/
							/* era valpnt */

		/* target exceeds high key value for node */
		ct_tky = 1;
		ct_elm = 0;
		return(-1);
	}

/* check for empty (leaf) node
*/
	if (!endser) {
		ct_elm = 0;
		ct_tky = -1;
		return(-2);
	}

	if (buffer->lstpos) {

      if (buffer->confg & REGULAR)
         poff = sizeof(POINTER);

      /* Este if foi colocado para nao perder ocorencias anteriores */
      /* isto acontecia em funcao da navegacao na arvore bater inicalmente */
      /* no ultimo elemento do bloco */
      if ( buffer->lstpos != buffer->nkv ) {
		   ct_tkp = compar((UTEXT *)idxval, (UTEXT *)buffer->keyexp + poff, tamanho);
         if (ct_tkp > 0) {
            begser = buffer->lstpos + 1;
         } else if (ct_tkp == 0) {
            ct_tky       = 0;
            ct_tkp       = -1;
            return(ct_elm = buffer->lstpos);
         } else {
            ct_tkp = 0;
         }
      }
	}

/* perform sequential search over remaining possibilities */

	for (ct_elm = begser; ct_elm <= endser; ct_elm++) {

#ifdef como_era_antes
      /* embuti toda a funcao expval dentro deste for para ter um ganho
         de aproximadamente 3% */
		ct_tky = compar((UTEXT *)idxval, (UTEXT *)expval(buffer, ct_elm),
                      tamanho); /* chamava valpnt */

#else

/**** inicio */
      n        = ct_elm;
	   retval   = buffer->keyexp;


	   if (buffer->confg & REGULAR)
		   retval += sizeof(POINTER);

		if (n == buffer->lstpos) 	goto xxx;
		else                       ip = buffer->ct_kyval;

	   if (n > buffer->lstpos) {
		   ip += (buffer->begbyt + buffer->actlen);
		   n  -= buffer->lstpos;
	   } else
		   buffer->lstpos = buffer->actlen = buffer->begbyt = 0;

	   while (n > 0) {

         tp = buffer->keyexp;
         buffer->lstpos++;
         n--;

         buffer->begbyt += buffer->actlen;

         if (buffer->confg & REGULAR) {
	         cpybuf(tp,ip,buffer->actlen = sizeof(POINTER));
	         tp        += sizeof(POINTER);
	         ip        += sizeof(POINTER);
         } else
	         buffer->actlen = 0;

		   total = *ip++;
		   comun = *ip++;
         implen = total - 2; /* 2 sao os bytes de total e comun */

         buffer->len_exp = implen + comun;

         buffer->actlen += total;
         tp         += comun;

         if ( buffer->len_exp > buffer->klen) {
            printf("\nTotal   = %d", total);
            printf("\nComun   = %d", comun);
            printf("\nLen_exp = %d", buffer->len_exp);
            printf("\nIp = [%s]\n", ip);
            terr(233);
         }

         cpybuf(tp,ip,implen);
         ip += implen;
	   }

xxx:


/**** termino */
		ct_tky = compar((UTEXT *)idxval, (UTEXT *)retval , tamanho); /* chamava valpnt */


#endif

		if (ct_tky > 0) {
			ct_tkp = ct_tky;
		} else if (ct_tky < 0 && stratg == 'E')
			return(-2);
		else
			return(ct_elm);
	}

	if ((stratg == 'S' && buffer->sucesr) || ( /**** SAI buffer->ktipe && ****/
	    buffer->leaf == NOLEAF)) {
		ct_tky = 1;
		ct_elm = 0;
		return(-1);
	} else {
		ct_tky = ct_tkp;
		ct_elm = buffer->nkv + 1;
		return(-2);
	}
}

#ifndef setnul
VOID setnul(s)
TEXT *s;
{
	*s = '\0';
}
#endif

/* --------------------------------------------------------------------
   getnod searches the buffer area for the requested b-tree node (specified
   by node number and key number (pointer)). if found, it returns pointer to
   buffer; else it pushes out the least recently used node and reads the
   requested node into the vacated buffer, passing back a pointer to this
   buffer.
 */

TREEBUFF *getnod(node,knum)
LONG    node; 	/* node number requested */
KEYFILE *knum; /* key number pointer */
{

#ifdef FPUTFGET
	CTFILE	      *ctnum;
#endif
	TREEBUFF      *getflg;
	FAST TREEBUFF *fndbuf;
						/* pointers to buffers */
	FAST COUNT i; /* counter */
	FAST UCOUNT lstusd; /* store for finding node (buffer) with lowest
			 node access number; ie lru buffer */

# ifdef DEBUGEST
extern ULONG Xgetnod;
Xgetnod++;
#endif

	if (node == NODEZERO){
		return(NULL); /* ANANIAS. so deveria voltar NULL se fosse
							  folha unica, mas como nao sei como saber
							  isto agora vai ficar voltando NULL sempre */

		/* terr(237); */
	}
	getflg = fndbuf = ct_btree;
	lstusd = fndbuf->nodacs;

	for (i = 0; i++ < ct_mxbuf; fndbuf++) {
		if (fndbuf->nodeid == node && fndbuf->keyid == knum->filnum) {

#ifdef FPUTFGET
			if (fndbuf->update == 'y')
				terr(208);
			ctnum = knum - knum->kmem;

			if ((ctnum->flmode & SHARED) && rednod(fndbuf,node,knum))
				return(NULL);
#endif

			inracs(fndbuf);   /* node found; update node
 						            access count and return
						            pointer to buffer
						         */
			return(fndbuf);
		}

		if (fndbuf->nodacs < lstusd)  /* set getflg to buffer pointer
						                     with lowest nodacs */
			lstusd = (getflg = fndbuf)->nodacs;
	}


/* node not found in buffers. read node into lru buffer */

	if ((fndbuf = lrubuf(getflg)) == NULL) /* then error in lrubuf */
		return(NULL);

	if (rednod(fndbuf, node, knum))	/* then error in rednod */
		return(NULL);

	return(fndbuf);
}


/* --------------------------------------------------------------------
   return pointer to buffer containing lru buffer. if lru buffer contains
   an updated node, write out the node to appropriate index file
 */

TREEBUFF *lrubuf(getflg)
TREEBUFF *getflg; /* if non-zero, then pointer to buffer
			       containing lru buffer; ie, the lru buffer
			       has already been determined */
{
	COUNT wrtnod();	    	/* write updated node back to disk */
	FAST COUNT i;	       	/* counter */
	FAST TREEBUFF *fndbuf; 	/* pointer to buffer */
	FAST UCOUNT  lstusd;    /* store to determine buffer with lowest
               				   node access number */

	if (!getflg) {		/* lru buffer has not been determined yet */
		getflg = fndbuf = ct_btree;
		lstusd = fndbuf->nodacs;
		for (i = 1, ++fndbuf; i++ < ct_mxbuf; fndbuf++)
			if (fndbuf->nodacs < lstusd)
				lstusd = (getflg = fndbuf)->nodacs;
	}

	if (getflg->update == 'y')  /* lru node has been updated. write it */
		if (wrtnod(getflg)) /* then write error */
			return(NULL);
	return(getflg);		    /* return lru buffer ptr */
}


/* --------------------------------------------------------------------
   increment buffer age & check for roll over
 */

VOID inracs(buffer)

PFAST TREEBUFF *buffer;

{
	FAST TREEBUFF *tmpbuf;
	FAST COUNT i;
	LOCAL UCOUNT minage;

	if (++ct_lacs != 0 )	/* then buffer age did not rollover */
		buffer->nodacs = ct_lacs;
	else {		/* buffer age rollover. must adjust all buffer ages */
		tmpbuf = ct_btree;
		minage = MAXAGE;
		for (i = 0; i++ < ct_mxbuf; tmpbuf++)
			if (tmpbuf->nodacs < minage && tmpbuf->nodacs)
				minage = tmpbuf->nodacs;
		ct_lacs = MAXAGE - (--minage);
		tmpbuf = ct_btree;
		for (i = 0; i++ < ct_mxbuf; tmpbuf++)
			if (tmpbuf->nodacs)
				tmpbuf->nodacs -= minage;
		buffer->nodacs = ++ct_lacs;
	}
}


/* --------------------------------------------------------------------
   read b-tree node from the index file pointed to by knum into the
   b-tree buffer. returns non-zero value if error.
 */

COUNT rednod(buffer,node,knum)
PFAST TREEBUFF *buffer;
LONG            node;
PFAST KEYFILE  *knum;
{
	CTFILE    *ctnum;


# ifdef DEBUGEST
extern ULONG Xrednod;
Xrednod++;
#endif

/* update buffer status information */

	buffer->nodeid = node;
	buffer->keyid  = knum->filnum;
	buffer->update = 'n';
	buffer->klen   = knum->length;
	buffer->ktipe  = knum->ktype;
	buffer->lstpos = buffer->actlen = buffer->begbyt = 0;
	inracs(buffer);

/* read node into buffer */

	if (knum->kmem > 0)
#ifdef CTSERVER
		ctnum = knum->hmem;
#else
		ctnum = knum - knum->kmem;
#endif
	else
		ctnum = knum;



#ifdef CTSERVER
#ifdef EXTRABFR
	if(!redxnod(buffer,node,knum))
#endif
#endif


	if (ctio(CTREAD,ctnum,node,buffer->nodorg,ctnum->recsiz))
		return(uerr_cod);

/* move node status info to TREEBUFF area */

	cpybuf(&buffer->sucesr,buffer->nodorg,STATUS);

#ifdef UNIFRMAT
	revbyt(&buffer->sucesr,6);
	revwrd(&buffer->sucesr,2);
#endif

/* check member number */

	if (buffer->bmem != knum->kmem)
		terr(231);

	if (buffer->leaf == LEAF) {
		buffer->maxkv = knum->maxkvl;
		buffer->maxb  = knum->maxkbl;
		buffer->confg = LB_EXTRA;
	} else {
		buffer->maxkv = knum->maxkvn;
		buffer->maxb  = knum->maxkbn;
		buffer->confg = REGULAR;
	}

	return(NO_ERROR);
}


/* --------------------------------------------------------------------
   return root node #
*/
#ifndef gtroot

LONG    gtroot(knum)
PFAST KEYFILE *knum;
{

# ifdef DEBUGEST
extern ULONG Xgtroot;
#endif


#ifdef FPUTFGET
	CTFILE *ctnum;

	ctnum = knum - knum->kmem;
	if ((ctnum->flmode & SHARED) && redhdr(knum - knum->kmem))
		return(DRNZERO);
#endif


# ifdef DEBUGEST
Xgtroot++;
#endif

	return(knum->root);
}

#endif
TEXT *hghpnt(buf)
PFAST TREEBUFF *buf;
{
	COUNT i;

# ifdef DEBUGEST
extern ULONG Xhghpnt;
Xhghpnt++;
#endif

#ifdef TRATA_NAO_COMPACTADA_OK
   esta com erro
	if ( buf->ktipe == COMPACTA )
   	i = buf->klen + 2;
   else
      printf("Chave nao compactada\n");
#else
   i = buf->klen + 2;
#endif

   /**** DUVIDA SE TEM QUE SOMAR 2 PARA chave NAO COMPACTADA */

	if (buf->confg & REGULAR)
		return( buf->ct_kyval + (buf->maxkv - 1) * (i + sizeof(POINTER))
                            + sizeof(POINTER)); /** ??? ??? */
	else
		return( buf->ct_kyval + (buf->maxkv - 1) * i);
}


/* --------------------------------------------------------------------
   return node pointer value
 */

LONG    nodpnt(buffer,ct_elm)

TREEBUFF *buffer;
PFAST COUNT ct_elm;

{
	POINTER tp;

# ifdef DEBUGEST
extern ULONG Xnodpnt;
Xnodpnt++;
#endif

	if (buffer->confg != REGULAR) /* so deve ser chamado para NO interm */
		terr(209);

	cpybuf(&tp,valpnt(buffer,ct_elm) - sizeof(POINTER),sizeof(POINTER));

#ifdef UNIFRMAT
	revobj(&tp,sizeof(POINTER));
#endif

	return((LONG   ) tp);
}

/* --------------------------------------------------------------------
   return drn pointer value   ( Duplicated Record Number )
 */

#ifdef LOW_HIGH

POINTER drnpnt(buffer,ct_elm)

TREEBUFF *buffer;
COUNT ct_elm;

{
	FAST TEXT *tp,*pp;
	POINTER    pntr;

# ifdef DEBUGEST
extern ULONG Xdrnpnt;
Xdrnpnt++;
#endif

	pp = (TEXT *) &pntr;
	tp = valpnt(buffer,ct_elm);

	if (buffer->confg & REGULAR)
      cpybuf(pp,tp - sizeof(POINTER),sizeof(POINTER));
	else {
      pntr = (POINTER) 0;
	}
	return(pntr);

}

#else

POINTER drnpnt(buffer,ct_elm)

PFAST TREEBUFF *buffer;
PFAST COUNT     ct_elm;

{
	FAST TEXT    *tp;
	POINTER       pntr;

	tp = valpnt(buffer,ct_elm) - sizeof(POINTER);

	if (buffer->confg & REGULAR) {
   	cpybuf(&pntr,tp,sizeof(POINTER));
#ifdef UNIFRMAT
   	revobj(&pntr,sizeof(POINTER));
#endif
   }
	else {
      pntr = (POINTER) 0;
	}

	return(pntr);
}

#endif


/* --------------------------------------------------------------------
   return pointer to key value in designated position of buffer
 */

/**** SAI a proxima funcao toda */
TEXT *valpnt(buffer,ct_elm)

PFAST TREEBUFF *buffer;
PFAST COUNT ct_elm;

{

# ifdef DEBUGEST
extern ULONG Xvalpnt;
Xvalpnt++;
#endif

	return(expval(buffer,ct_elm));
}

/* --------------------------------------------------------------------
 */

TEXT *expval(bp,n)
TREEBUFF    *bp;
COUNT           n;
{
	COUNT	   implen, total, comun;
	FAST TEXT *ip,*tp;
	TEXT      *retval;


# ifdef DEBUGEST
extern ULONG Xexpval;
Xexpval++;
#endif

	if (n < 1 || n > (bp->nkv + 1))
		terr(232);

	retval = bp->keyexp;


	if (bp->confg & REGULAR)
		retval += sizeof(POINTER);

	if (n == bp->lstpos)    return(retval);
	else                    ip = bp->ct_kyval;

	if (n > bp->lstpos) {
		ip += (bp->begbyt + bp->actlen);
		n  -= bp->lstpos;
	} else
		bp->lstpos = bp->actlen = bp->begbyt = 0;

	while (n > 0) {

      tp = bp->keyexp;
      bp->lstpos++;
      n--;

      bp->begbyt += bp->actlen;

      if (bp->confg & REGULAR) {  /* NO intermediario tem POINTER antes p/ */
                                 /* apontar para o proximo NO */
	      cpybuf(tp,ip,bp->actlen = sizeof(POINTER));
	      tp        += sizeof(POINTER);
	      ip        += sizeof(POINTER);
      } else
	      bp->actlen = 0;

		total = *ip++;
		comun = *ip++;
      implen = total - 2; /* 2 sao os bytes de total e comun */

      bp->len_exp = implen + comun;

      bp->actlen += total;
      tp         += comun;

      if ( bp->len_exp > bp->klen) {
         printf("\nTotal   = %d", total);
         printf("\nComun   = %d", comun);
         printf("\nLen_exp = %d", bp->len_exp);
         printf("\nIp = [%s]\n", ip);
         terr(233);
      }

      cpybuf(tp,ip,implen);
      ip += implen;
	}

	return(retval);
}

/* end of ctkrnl.c */


