/*
 *	add key
 *
 */

#include "ctstdr.h"		/* standard i/o header 		*/


LOCAL TEXT   hghbuf[MAXLEN];

/* --------------------------------------------------------------------
   ADDKEY enters the key value pointed to by target into the index file
   specified by keyno and associates the value of recbyt with the key value.
   if successful, ADDKEY returns a value of zero. if the index already
   contains the key value, then a value of 2 (two) is returned and no
   change is made to the index file. values greater than 2 indicate an error
   has occurred.
 */
   
COUNT ADDKEY(keyno,target,recbyt,typadd, tam_chave)

COUNT   keyno;
TEXT   *target;
POINTER recbyt;
COUNT   typadd;
COUNT   tam_chave;
/** ANANA  recbyt vai passar a ser o tamanho da chave, portanto mude todo length */
{
	TEXT 	*idxval;
	FAST 	KEYFILE *knum;
	FAST 	TREEBUFF *buffer;
	COUNT	npoint;
	LOCAL	LONG    node;


	uerr_cod	= 0;	/* set user error cod to zero */

	if ((knum = tstfnm(keyno)) == NULL)	return(uerr_cod);
	if (chkredf(knum))                  return(uerr_cod);

	cpybuf((idxval = ct_dupkey),target, tam_chave);

	ct_lnode = btlev = 0;   /*** ANANIAS para insercao sequencial talves ***/
                           /*** seja melhor rever ***/

#ifndef FPUTFGET
	if ((node = gtroot(knum)) == 0 ) {	/* tree may be empty or gtroot err */
		if (	uerr_cod ||
				tstupd(knum) ||
				adroot(knum,recbyt,DRNZERO,idxval, tam_chave) )
			return(uerr_cod);
		return(NO_ERROR);
	}
#else
	if (!(node = gtroot(knum))) {
		if (uerr_cod || tstupd(knum))
			return(uerr_cod);
		else if ((npoint = adroot(knum,recbyt,DRNZERO,idxval, tam_chave)) == -1)
			node = gtroot(knum);
		else if (npoint )
			return(uerr_cod);
		else
			return(NO_ERROR);
	}
#endif

	while (node) {	/* walk down or across tree until NO FOLHA found */

		ct_lnode = node;

		if ((buffer = getnod(node,knum)) == NULL)  return(uerr_cod);

		if (buffer->leaf == LEAF)                  break;

		if ((npoint = nodser(buffer, idxval, tam_chave, 'L')) != -1) {

			if (npoint == -2) 			terr(218);
			if (++btlev >= MAXLEV)     terr(240);

			ct_npath[btlev] = node;
			node = nodpnt(buffer,npoint);

		}

		else node = buffer->sucesr;
	}

	if (!node)	/* => no leaf node found */
		terr(219);

   /* movrgt caminha atraves dos NOS procurando uma chave NAO MENOR
      que idxval
   */

	if ((buffer = movrgt(idxval, tam_chave, knum,buffer)) == NULL)
		return(uerr_cod);

   /* se movrgt achou chave IGUAL : ct_tky = 0 */

   if (!ct_tky) 
		return(uerr(KDUP_ERR)); /* chave ja' existe */

	if (tstupd(knum) ||
			insert(buffer,knum,idxval,tam_chave,typadd) )
		return(uerr_cod);
	return(NO_ERROR);
}

/* --------------------------------------------------------------------
   insert key value & backtrack as necessary
*/

COUNT insert(buffer,knum,idxval,tam_left,typadd)

PFAST TREEBUFF *buffer;
PFAST KEYFILE  *knum;
TEXT           *idxval;
COUNT           tam_left;
COUNT           typadd;

{
	COUNT    temp,spltel, tam_rgt;
	UCOUNT   tempb;
	LONG     node,oldnode;
	FAST TREEBUFF  *new;
	TEXT	   *tp;
   POINTER pntr = 0;

# ifdef DEBUGEST
extern ULONG Xinsert;
#endif

again:

# ifdef DEBUGEST
Xinsert++;
#endif

	temp = buffer->nkv + 1;

	/* for leaf nodes, if node already full, then must save high key.
	   (note that leaf holds maxkv-1 keys (besides high key))
	*/

	if (buffer->leaf == LEAF)
		cpybuf(hghbuf,hghpnt(buffer), buffer->klen);

	if (!ct_elm || ct_elm > buffer->nkv)  /* inicio ou fim do NO */
		ct_tky = 0;

   /* garante que a nova chave vai caber porque
      se extrapolar maxkb mela a chave MOR mas nao tem problema
      pois esta sera reconstruida
   */


	insexp(buffer, idxval, pntr, tam_left);

	tempb = buffer->nkb;

	if (tempb <= buffer->maxb) { /* caso simples: ainda cabe no NODO */
		node = buffer->nodeid;
      /* ANANIAS putnod pode virar uma macro */
		if ( putnod(buffer,temp) )
			return(uerr_cod);
		return(NO_ERROR);
	}


   /* Vai fazer split do NO */
	if ((new = newnod(knum, &ct_nwnod, NO)) == NULL)
		return(uerr_cod);

	if (buffer->leaf == LEAF) {
      new->maxkv = knum->maxkvl;
      new->maxb  = knum->maxkbl;
      new->confg = LB_EXTRA;

	} else {
		new->maxkv = knum->maxkvn;
		new->maxb  = knum->maxkbn;
      new->confg = REGULAR;
	}

	if 		(typadd == REGADD) 	spltel = temp / 2;
	else if 	(typadd == INCADD) 	spltel = 6 * temp / 7;
	else if 	(typadd > 1000   ) 	spltel = typadd - 1000;
	else                       	spltel = temp / 7 + 1;


   /* spltel e o ponto de split (enesima chave do NO) split element */
   /* valpnt retorna um ponteiro para a chave numero spltel */

	tp = valpnt(buffer,spltel);

	/* begbyt counts to beg of split ct_key: check that split ct_key actually
	   fits - INCADD with ct_key compression & keys out of order can cause
	   a problem.
	*/

	if ((buffer->begbyt + buffer->actlen) > buffer->maxb) {
		spltel	= temp / 2;
		tp	= valpnt(buffer,spltel);
	}

   tam_left = buffer->len_exp;
	cpybuf(spkey,tp, buffer->klen);


   /* acha o recbyt da chave (PROXIMA) onde vai iniciar proximo NO */

	pntr = drnpnt(buffer,spltel + 1);
   /* drnpnt tambem altera keyexp e len_exp */
   tam_rgt = buffer->len_exp;

	ct_tky = ct_tkp = tempb = 0;

	tp = buffer->keyexp;

	if (buffer->confg & REGULAR) /* tem POINTER na chave (NO INTERM)*/
		tempb = sizeof(POINTER);

   insexp(new, tp + tempb, pntr, tam_rgt);

	tempb  = buffer->begbyt + buffer->actlen;

	cpybuf(new->ct_kyval + new->nkb,buffer->ct_kyval + tempb,
						buffer->nkb - tempb);

	new->nkb      += (buffer->nkb - tempb);
	buffer->nkb    = buffer->begbyt;
	buffer->begbyt = buffer->actlen = buffer->lstpos = 0;


	if (buffer->leaf == LEAF) { /* set high keys */
		/* copy over high key for new node */
		cpybuf(hghpnt(new),hghbuf, buffer->klen);

		/* spkey holds the high key for the oldnode */
		cpybuf(hghpnt(buffer),spkey, buffer->klen);
	}

	new->sucesr = buffer->sucesr;
	oldnode = buffer->nodeid;

	if ((new->leaf = buffer->leaf) == LEAF)
		new->predsr = oldnode;

	buffer->sucesr = ct_nwnod;

	if (putnod(new,temp - spltel) || putnod(buffer,spltel))
		return(uerr_cod);

	if (new->sucesr && new->leaf == LEAF) {
		if ((buffer = getnod(new->sucesr,knum)) == NULL)
			return(uerr_cod);
		buffer->predsr = ct_nwnod;
		if (putnod(buffer,buffer->nkv) )
			return(uerr_cod);
	}


	idxval = spkey;
	pntr = oldnode;


	if ( (node = ct_npath[btlev--]) != 0 ) { /* parent node exists, repeat process */
		if ((buffer = movrgt(idxval, tam_left, knum,getnod(node,knum))) == NULL)
			return(uerr_cod);
		if (nodpnt(buffer,ct_elm) != oldnode)
			/*
			 * apparently we have encountered a prematurely
			 * terminated node split. parent does not have
			 * complete set of pointers. move right strategy
			 * avoids crash.
			 */
			pntr = nodpnt(buffer,ct_elm);

		cpybuf(buffer->ct_kyval + buffer->begbyt,&ct_nwnod,sizeof(POINTER));

		/* atualiza a informacao de keyexp decorrente do copy anterior */
		cpybuf(buffer->keyexp,&ct_nwnod,sizeof(POINTER));


#ifdef UNIFRMAT
		revobj(buffer->ct_kyval + buffer->begbyt,sizeof(POINTER));
		revobj(buffer->keyexp,sizeof(POINTER));
#endif


		goto again;


	} else { /* create new non-leaf root */

		if ( adroot(knum,pntr,(POINTER) ct_nwnod,spkey, tam_left))
			return(uerr_cod);
	}

	return(NO_ERROR);
}


/* --------------------------------------------------------------------
   add new root to b-tree pointed to by knum. lpntr & rpntr are the left
   and right pointers associated with the key value pointed to by idxval.
 */

COUNT adroot(knum,lpntr,rpntr,idxval, tam_chave)

PFAST KEYFILE *knum;
POINTER lpntr,rpntr;
TEXT *idxval;
COUNT tam_chave;

{
	FAST TEXT *tp;
	FAST TREEBUFF *new;
	COUNT i;


# ifdef DEBUGEST
extern ULONG Xaddroot;
Xaddroot++;
#endif

	/* if last parameter in newnod == YES, the header lock
	   acquired in newnod is NOT released by newnod if newnod
	   is successful. If two or more processes call newnod for
	   a virgin tree, then only the first will return a non-null
	   value. The other calls to newnod will result in uerr_cod
	   set to -1 as a signal to ADDKEY to skip regular adroot procedure.
	*/

	if ((new = newnod(knum, &ct_nwnod, rpntr ? NO : YES)) == NULL)
		return(uerr_cod);

	if (rpntr) { /* then non-leaf root; add adjacent max high key */
		new->leaf  = NOLEAF;
		new->maxkv = knum->maxkvn;
		new->maxb  = knum->maxkbn;
      new->confg = REGULAR;

	} else { /* leaf root (virgin tree) */

		new->leaf  = LEAF;
		new->maxkv = knum->maxkvl;
		new->maxb  = knum->maxkbl;
		new->confg = LB_EXTRA;
		tp = hghpnt(new);
	}

	ct_tky = ct_tkp = 0;

   insexp(new,idxval,lpntr, tam_chave);

	if (new->leaf == NOLEAF)
		tp = hghbuf;

/*
 * Constroe maior chave
 */

	for ( i = knum->length;  i > 0; i--)
		*tp++ = 0xff;


	if (rpntr) {
		new->nkv = i = 2;
		valpnt(new,1); /* to set last position */
		insexp(new,hghbuf,rpntr, new->klen);
	}

   else i = 1;


	if (putnod(new,i))
		return(uerr_cod);

	knum->root = ct_nwnod;

#ifdef NOTFORCE
	/* no action needed */
#else
	if (wrthdr(knum))
		return(uerr_cod);
#endif

	return(NO_ERROR);
}


/* --------------------------------------------------------------------
   routine to get next available node. 
*/

TREEBUFF *newnod(knum, pnode, virgin)
PFAST KEYFILE *knum;
LONG    *pnode;
COUNT virgin; /* virgin adroot: YES or NO */
{
	FAST TREEBUFF *buf;

#ifdef VARLDATA
	VHDR	  vrhdr;
#endif

# ifdef DEBUGEST
extern ULONG Xnewnod;
Xnewnod++;
#endif


#ifdef FPUTFGET
	if (redhdr(knum - knum->kmem))
		return(NULL);
	if (virgin == YES && knum->root) {
		/* someone has already added a root: send signal to adroot
		   to skip virgin adroot and proceed with regular insert */
		uerr_cod = -1;
		return(NULL);
	}
#endif

#ifndef VARLDATA 
	/* no variable length data routine support */
	if (knum->clstyp == VAT_CLOSE)
		terr(225);
	else if (!(*pnode = extfil(knum,knum->recsiz))) {

		return(NULL);
	}
#else
	/* variable length data routines supported */
	if (knum->clstyp == VAT_CLOSE) {
		if ( (*pnode = extfil(knum, knum->recsiz + SIZVHDR)) != 0) {
			*pnode += SIZVHDR;
			vrhdr.recmrk = VNOD_FLAG;
			vrhdr.trclen = vrhdr.urclen = knum->recsiz;
			if (putvhdr(knum,*pnode,&vrhdr)) {
				return(NULL);
			}
		} else {
			return(NULL);
		}
	} else if ((*pnode = extfil(knum,knum->recsiz)) == 0) {
		return(NULL);
	}
#endif

#ifdef NOTFORCE
	/* no action */
#else
	if (wrthdr(knum))
		return(NULL);
#endif

	if ((buf    = lrubuf(NULL)) == NULL)
		return(NULL);
	buf->nkv    = buf->nkb = buf->begbyt = buf->actlen = buf->lstpos = 0;
   buf->ktipe  = knum->ktype;
	buf->keyid  = knum->filnum;
   buf->klen   = knum->length;
	buf->nodeid = *pnode;
	buf->update = 'y';
	buf->predsr = buf->sucesr = NODEZERO;
	buf->bmem   = knum->kmem;

   virgin = virgin; /* tirar warning */

	return(buf);
}


/* --------------------------------------------------------------------
   insere uma chave no NO
                       total
             //                       \\ 
   | pointer | total | comun |  chave  |
             ^          ^         parte nao comun a chave anterior
             |          parte comun com chave anterior
             tp

*/

VOID insexp(bp, ip, pntr, n)
TREEBUFF   *bp;
TEXT	      *ip;
POINTER	   pntr;
COUNT       n;
{
	COUNT    movcnt, movct2, comun, comun2, total2;
	COUNT	   poff;
	FAST TEXT *tp;

# ifdef DEBUGEST
extern ULONG Xinsexp;
Xinsexp++;
#endif

	if (ct_tky > 0 || ct_tkp < 0)
		terr(235);

	if ((comun = ct_tkp - 1) > 0) {
		if (comun > n) {
			comun = n; /* "ANANIAS acho que isto e um ABSURDO 1" */
      }
		ip += comun;
		n  -= comun;
	} else
		comun = 0;

/* check for addition to end of buffer (& empty buffer ) */

	if (ct_tky == 0)	/* assumes values for lstpos begbyt & actlen */
		tp = bp->ct_kyval + bp->begbyt + bp->actlen;
	else {

/* insertion in middle or beginning of buffer */

		ct_tky = -ct_tky;
		movcnt = n + 2;

/* see if following value is further compressed and shift */

		if (bp->confg & REGULAR)      movcnt += (poff = sizeof(POINTER));
		else                          poff = 0;

		tp = bp->ct_kyval + bp->begbyt;

		total2  = *(tp + poff) & 0x00ff;        /* TOTAL */

		comun2 = *(tp + poff + 1) & 0x00ff;    /* COMUN */

      --ct_tky;

		if (ct_tky <= comun2)
			shfrgt(movcnt,bp,bp->begbyt);
		else {
			if ((movct2 = ct_tky - comun2) > movcnt)
				terr(236);

			bp->actlen -= movct2;

			*(tp + movct2 + poff) = total2 - movct2;

			*(tp + movct2 + poff + 1) = ct_tky;

			if (poff) {
				shflft(movct2,bp,bp->begbyt + poff + movct2);
				bp->nkb -= movct2;
				shfrgt(movcnt,bp,bp->begbyt);
			} else {
				shfrgt(movcnt - movct2,bp,	bp->begbyt + movct2);
				bp->nkb -= movct2;
			}
		}

		tp = bp->ct_kyval + bp->begbyt;
		bp->lstpos++;   /** ANANIAS ???? nao deveria alterar keyexp */
		bp->begbyt += movcnt;
	}

	/* stuff bytes in place */

	if (bp->confg & REGULAR) {
#ifdef UNIFRMAT
		revobj(&pntr,sizeof(POINTER));
#endif
		cpybuf(tp,&pntr,sizeof(POINTER));
		tp      += sizeof(POINTER);
		bp->nkb += sizeof(POINTER);
	}

	bp->nkb += ( 2 + n );

	*tp++ = 2 + n;
	*tp++ = comun;

	cpybuf(tp,ip,n);

}

/* end of ctaddk.c */
