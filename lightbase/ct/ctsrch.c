/*
 *	index search routines
 *
 */

#include "ctstdr.h"		/* standard i/o header 		*/


/* --------------------------------------------------------------------
   returns the "pointer" (ie, data record number or whatever) associated
   with the key value pointed to by target for the index file specified by
   keyno

   retorna 1 se achou a chave ==> seta vars globais   : ct_ind_key = achada
                                                        ct_len_key = tamanho   
   retorna 0 se nao achou chave ==> seta vars globais : ct_ind_key = ""
                                                        ct_len_key = 0   
 */

POINTER EQLKEY(keyno,target, tamanho)

PFAST COUNT keyno;
TEXT *target;
COUNT tamanho;

{
	FAST KEYFILE *knum;
	LOCAL POINTER temp;

	uerr_cod = 0;
   ct_lenkey = 0;

	if ((knum = tstfnm(keyno)) == NULL)
		return(DRNZERO);

	/* fndkey seta as variaveis citadas no comentario */

   temp = fndkey(knum,target,'E', tamanho);

	knum->retelm = ct_elm;
	knum->retnod = ct_fnode;

	return(temp);
}


/* -------------------------------------------------------------------
   returns the "pointer" associated with the first key value in the index
   file specified by keyno which is equal to or greater than the key value
   pointed to by target. if such an entry exists, then the variable pointed
   to by idxval is set equal to the key value found in the index file
 */

POINTER GTEKEY(keyno, target, idxval, tamanho)

PFAST COUNT keyno;
TEXT *target;
TEXT *idxval;
UCOUNT tamanho;

{
	FAST KEYFILE *knum;
	LOCAL POINTER temp;

	uerr_cod = 0;
   ct_lenkey = 0;

	if ((knum = tstfnm(keyno)) == NULL)
		return(DRNZERO);

	/* fndkey seta as variaveis citadas no comentario */

	if ( (temp = fndkey(knum,target,'G', tamanho)) != (POINTER) 0) {
		knum->retelm = ct_elm;
		knum->retnod = ct_fnode;
		cpybuf(idxval,ct_indkey,knum->length);
	} else {
		knum->retelm = 0;
		knum->retnod = DRNZERO;
		setnul(idxval);
	}
	return(temp);
}


/* --------------------------------------------------------------------
   returns the "pointer" associated with the next key value (in ascending
   key value order) and sets the variable pointed to by idxval to this
   key value. this function is used to access the key values (and their
   associated "pointers" in ascending key value order.
 */

POINTER NXTKEY(keyno,idxval)

COUNT keyno;
TEXT *idxval;

{
	FAST KEYFILE *knum;
	FAST COUNT temp;
	FAST TREEBUFF *ret;


	uerr_cod = 0;
   ct_lenkey = 0;

	if ((knum = tstfnm(keyno)) == NULL)
		return(DRNZERO);

again:
	if (!knum->retnod) {
		setnul(idxval);
		return(DRNZERO);
	}
	if ((ret = getnod(knum->retnod,knum)) == NULL) /* then error */
		return(DRNZERO);
	if ((temp = knum->retelm) < ret->nkv) {
		cpybuf(idxval,valpnt(ret,++temp),knum->length);

      cpybuf(ct_indkey, idxval, knum->length); /* ananias */
      ct_lenkey = ret->len_exp;

      knum->retelm = temp;
		return( (POINTER) 1 ); /* Nao fazia sentido retornar endereco */
	} else {
		knum->retnod = ret->sucesr;
		knum->retelm = 0;
		goto again;
	}
}


/* --------------------------------------------------------------------
   returns the "pointer" associated with the previous key value and sets 
   the variable pointed to by idxval equal to this key value. this function
   is used to access the key values (and their associated "pointers") in
   descending key value order.
 */

POINTER PRVKEY(keyno,idxval)

COUNT keyno;
TEXT *idxval;

{
	FAST TREEBUFF *ret;
	FAST COUNT prdtst,temp;
	FAST KEYFILE *knum;
	LONG    oldnode;


	uerr_cod = 0;
   ct_lenkey = 0;

	if ((knum = tstfnm(keyno)) == NULL)
		return(DRNZERO);

	if(!knum->retnod) {
		setnul(idxval);
		return(DRNZERO);
	}

	prdtst = PRDRPEAT;

split:
	if ((ret = getnod((oldnode = knum->retnod),knum)) == NULL)
		return(DRNZERO); /* then error */
again2:
	if ((temp = knum->retelm) > 1) {
		cpybuf(idxval,valpnt(ret,--temp),knum->length);

      cpybuf(ct_indkey, idxval, knum->length); /* ananias */
      ct_lenkey = ret->len_exp;

      knum->retelm = temp;

		return( (POINTER) 1 ); /* Nao fazia sentido retornar endereco */
      
		/*return(drnpnt(ret,temp)); */
	} else if ( (knum->retnod = ret->predsr) != (LONG) 0) {
		if ((ret = getnod(knum->retnod,knum)) == NULL) /* then error */
			return(DRNZERO);
		if ((temp = ret->nkv) < 0)
			terr(211);
		if (oldnode != ret->sucesr) {
			if (!(prdtst--)) {
				uerr(PRDS_ERR);
				return(DRNZERO);
			}
			knum->retnod = oldnode;
			goto split;	/* node splitting at time of request */
		}
		knum->retelm = temp;
		if (!temp) {
			oldnode = knum->retnod;
			goto again2;
		}
		cpybuf(idxval,valpnt(ret,temp),knum->length);

      cpybuf(ct_indkey, idxval, knum->length); /* ananias */
      ct_lenkey = ret->len_exp; /* ananias */

		return( (POINTER) 1 ); /* Nao fazia sentido retornar endereco */
		/* return(drnpnt(ret,temp)); */
	} else {
		setnul(idxval);
		return(DRNZERO);
	}
}

/* --------------------------------------------------------------------
   find first index entry.
 */

POINTER FRSKEY(keyno,idxval)

PFAST COUNT keyno;
TEXT *idxval;

{
	LOCAL LONG    node;
	FAST TREEBUFF *buffer;
	FAST KEYFILE *knum;

	uerr_cod = 0;
   ct_lenkey = 0;

	if ((knum = tstfnm(keyno)) == NULL)
		return(DRNZERO);

	if ((node = gtroot(knum)) == (LONG) 0 )
		goto empty;

	while (node) {	/* walk down tree until leaf node found */
		if ((buffer = getnod(node,knum)) == NULL)
			return(DRNZERO);
		if (buffer->leaf == LEAF)
			break;
		node = nodpnt(buffer,1);
	}
	if (!node)	/* => no leaf node found */
		terr(212);

	while (!buffer->nkv) /* walk across empty left most leaf nodes */
		if ( (node = buffer->sucesr) == (LONG) 0 )
			goto empty;
		else if ((buffer = getnod(node,knum)) == NULL)
			return(DRNZERO);

	knum->retnod = node;
	knum->retelm = 1;
	cpybuf(idxval,valpnt(buffer,1),knum->length);

   cpybuf(ct_indkey, idxval, knum->length); /* ananias */
   ct_lenkey = buffer->len_exp; /* ananias */

	return( (POINTER) 1 ); /* Nao fazia sentido retornar endereco */

/*	return( drnpnt(buffer,1)); ANANIAS nao faz mais sentido retornar endereco*/

empty:
	setnul(idxval);
	knum->retelm = 0;
	knum->retnod = NODEZERO;
	return(DRNZERO);

}


/* --------------------------------------------------------------------
   find LAST index entry.
 */

POINTER LSTKEY(keyno,idxval)

FAST COUNT keyno;
TEXT *idxval;

{
	LOCAL LONG    node;
	FAST TREEBUFF *buffer;
	FAST KEYFILE *knum;
	FAST COUNT tmpsiz;

	uerr_cod = 0;
   ct_lenkey = 0;

	if ((knum = tstfnm(keyno)) == NULL)
		return(DRNZERO);

	if ((node = gtroot(knum)) == (LONG) 0)
		goto empty2;

	while (node) {	/* walk down or across tree until leaf node found */
		if ((buffer = getnod(node,knum)) == NULL)
			return(DRNZERO);
		if (buffer->leaf == LEAF)
			break;
		if ( (node = buffer->sucesr) == (LONG) 0)
			node = nodpnt(buffer,buffer->nkv);
	}                     	
	if (!node)	/* => no leaf node found */
		terr(213);

	while (!buffer->nkv) /* walk across empty rightmost leaf nodes */
		if( (node = buffer->predsr) == (LONG) 0)
			goto empty2;
		else if ((buffer = getnod(node,knum)) == NULL)
			return(DRNZERO);

	knum->retnod = node;
	knum->retelm = tmpsiz = buffer->nkv;
	cpybuf(idxval,valpnt(buffer,tmpsiz),knum->length);

   cpybuf(ct_indkey, idxval, knum->length); /* ananias */
   ct_lenkey = buffer->len_exp; /* ananias */

	return( (POINTER) 1 ); /* Nao fazia sentido retornar endereco */

	/* return(drnpnt(buffer,tmpsiz)); */

empty2:
	setnul(idxval);
	knum->retelm = 0;
	knum->retnod = NODEZERO;
	return(DRNZERO);
}

/* ---------------------------------------------------------------------
   returns entry after target value
 */

POINTER GTKEY(keyno,target,idxval, tamanho)

PFAST COUNT keyno;
TEXT *target;
TEXT *idxval;
UCOUNT tamanho;


{
	POINTER temp;

	if ((temp = GTEKEY(keyno,target,idxval, tamanho)) == DRNZERO || ct_tky) 
		return(temp);
	else
		return(NXTKEY(keyno,idxval));
}

/* ---------------------------------------------------------------------
   returns entry immediately before target
 */

POINTER LTKEY(keyno,target,idxval, tamanho)

PFAST COUNT keyno;
TEXT *target;
TEXT *idxval;
UCOUNT tamanho;

{
	EQLKEY(keyno, target, tamanho);
	return(PRVKEY(keyno,idxval));
}	


/* ---------------------------------------------------------------------
   returns entry <= target
 */

POINTER LTEKEY(keyno,target,idxval, tamanho)

PFAST COUNT keyno;
TEXT *target;
TEXT *idxval;
UCOUNT tamanho;
{

	if ( EQLKEY(keyno, target, tamanho) != (POINTER) 0) {
		cpybuf(idxval, ct_indkey, (ct_key + keyno)->length);
		return((POINTER) 1);
	} else
		return(PRVKEY(keyno,idxval));
}	


/* --------------------------------------------------------------------
 general purpose search routine. ordinarily not called by application
   program; but called from other searches: rtriev, search, etc.
 */

POINTER fndkey(knum, idxval, stratg, tamanho)

PFAST KEYFILE *knum; 	/* pointer to ct_key structure (used like a ct_key #) */
TEXT *idxval; 		/* pointer to target key value */
TEXT stratg; 		/* search strategy indicator 'E'== equality
			   'G' == greater than or equal to search */
COUNT tamanho;
{
	LOCAL LONG    node;
	LOCAL COUNT npoint;
	FAST TREEBUFF *buffer;


	ct_fnode = ct_lnode = 0;	/* global var tracking last node visited */
	setnul(ct_indkey);		/* set return key value to null */
	if ( (node = gtroot(knum)) == (LONG) 0 ) {	/* tree may be empty or error */
		ct_elm = 0;
		return(DRNZERO);
	}

	while (node) {	/* walk down or across tree until leaf node found */
		ct_lnode = node;
		if ((buffer = getnod(node,knum)) == NULL) /* then error */
		    return(DRNZERO);
		if (buffer->leaf == LEAF) /* then found bottom level of tree */
		    break;
		if ((npoint = nodser(buffer, idxval, tamanho, 'L')) != -1) {
		    if (npoint == -2) /* then corrupt tree */
			terr(214);
		    /* get child node */
		    node = nodpnt(buffer,npoint);
		} else
		    /* move right because of incomplete tree update */
		    node = buffer->sucesr;
	}

	if (!node)	/* then no leaf node found */
		terr(215);

	return(serlef(idxval,knum,buffer,stratg, tamanho)); /* search leaf node */
}


/* --------------------------------------------------------------------
   search leaf node 
 */

POINTER serlef(idxval,knum,buffer,stratg, tamanho)

TEXT *idxval;		/* pointer to target key value */
PFAST KEYFILE *knum; 	/* key number pointer */
PFAST TREEBUFF *buffer; /* pointer to buffer containing leaf node */
TEXT stratg; 		/* 'E/G': see fndkey */
COUNT tamanho;

{
	LOCAL COUNT temp;

/* see if it is necessary to move right along leaf nodes */

   /* para fazer pesquisa string e necessario ligar o flag global */

	while ((temp = nodser(buffer,idxval, tamanho, stratg == 'E' ? 'E' : 'S')) == -1)
		if ((buffer = getnod((ct_lnode = buffer->sucesr),knum)) == NULL){
         /* sempre retorna para default binario */
			return(DRNZERO); /* error in getnod */
      }

   /* sempre retorna para default binario */

	ct_fnode = ct_lnode;
	if (temp != -2) { /* then sucessful leaf search */
		cpybuf(ct_indkey,valpnt(buffer,temp),
			knum->length); /* copy key value found into ct_indkey. */
		ct_lenkey = buffer->len_exp;
		return((POINTER) 1);
	} else		  /* target not found */
		return(DRNZERO);
}

/* end of ctsrch.c */




POINTER VARREKEY(keyno,idxval)

PFAST COUNT keyno;
TEXT *idxval;

{
	LOCAL LONG    node;
	FAST TREEBUFF *buffer;
	FAST KEYFILE *knum;
   TEXT ultima[100];
   UTEXT *p1, *p2;
   FILE *fp;

   fp = fopen("arv.ct", "w");


	uerr_cod = 0;
   ct_lenkey = 0;

	if ((knum = tstfnm(keyno)) == NULL)
		return(DRNZERO);

	if ((node = gtroot(knum)) == (LONG) 0 )
		goto empty;

volta:

	while (node) {	/* walk down tree until leaf node found */
		if ((buffer = getnod(node,knum)) == NULL)
			return(DRNZERO);
		if (buffer->leaf == LEAF)
			break;
		node = nodpnt(buffer,1);
	}

	if (!node)	/* => no leaf node found */
		terr(212);

	while (!buffer->nkv) /* walk across empty left most leaf nodes */
		if ( (node = buffer->sucesr) == (LONG) 0 )
			goto empty;
		else if ((buffer = getnod(node,knum)) == NULL)
			return(DRNZERO);

	knum->retnod = node;
	knum->retelm = 1;
	cpybuf(idxval,valpnt(buffer,1),knum->length);

   cpybuf(ct_indkey, idxval, knum->length); /* ananias */
   ct_lenkey = buffer->len_exp; /* ananias */

	cpybuf(idxval,valpnt(buffer,buffer->nkv),knum->length);
   cpybuf(ultima, idxval, knum->length); /* ananias */

   p1 = (UTEXT *)ct_indkey;
   p2 = (UTEXT *)ultima;

   fprintf(fp, "Node = %ld    nkv = %d    [%3.3d][%3.3d][%3.3d][%3.3d][%3.3d][%3.3d][%3.3d][%3.3d]   [%3.3d][%3.3d][%3.3d][%3.3d][%3.3d][%3.3d][%3.3d][%3.3d]\n",
      node, buffer->nkv,
         p1[0], p1[1], p1[2], p1[3], p1[4], p1[5], p1[6], p1[7],
         p2[0], p2[1], p2[2], p2[3], p2[4], p2[5], p2[6], p2[7]);

	if ( (node = buffer->sucesr) != (LONG) 0 )
      goto volta;

	return( (POINTER) 1 ); /* Nao fazia sentido retornar endereco */

/*	return( drnpnt(buffer,1)); ANANIAS nao faz mais sentido retornar endereco*/

empty:
   fclose(fp);
   
	setnul(idxval);
	knum->retelm = 0;
	knum->retnod = NODEZERO;
	return(DRNZERO);

}






/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
#ifdef PEGA_GATOKEY

#include "stdlib.h"


POINTER gatokey(KEYFILE *, TEXT *, TEXT, COUNT);
VOID printa_chave(TEXT *, TEXT *);

POINTER GATOKEY(keyno, target, idxval, tamanho)

PFAST COUNT keyno;
TEXT *target;
TEXT *idxval;
UCOUNT tamanho;

{
	FAST KEYFILE *knum;
	LOCAL POINTER temp;

	uerr_cod = 0;
   ct_lenkey = 0;

	if ((knum = tstfnm(keyno)) == NULL)
		return(DRNZERO);

	/* fndkey seta as variaveis citadas no comentario */

	if ( (temp = gatokey(knum,target,'G', tamanho)) != (POINTER) 0) {
		knum->retelm = ct_elm;
		knum->retnod = ct_fnode;
		cpybuf(idxval,ct_indkey,knum->length);
	} else {
		knum->retelm = 0;
		knum->retnod = DRNZERO;
		setnul(idxval);
	}
	return(temp);
}


POINTER gatokey(knum, idxval, stratg, tamanho)

PFAST KEYFILE *knum; 	/* pointer to ct_key structure (used like a ct_key #) */
TEXT *idxval; 		/* pointer to target key value */
TEXT stratg; 		/* search strategy indicator 'E'== equality
			   'G' == greater than or equal to search */
COUNT tamanho;
{
	LOCAL LONG    node;
	FAST TREEBUFF *buffer;
   TEXT *p;
   TEXT aux[100];
   COUNT  nc;


	ct_fnode = ct_lnode = 0;	/* global var tracking last node visited */
	setnul(ct_indkey);		/* set return key value to null */
	if ( (node = gtroot(knum)) == (LONG) 0 ) {	/* tree may be empty or error */
		ct_elm = 0;
		return(DRNZERO);
	}

printf("\nRoot = %ld\n", node);

	while (node) {	/* walk down or across tree until leaf node found */

		ct_lnode = node;

		if ((buffer = getnod(node,knum)) == NULL) /* then error */
		    return(DRNZERO);

printf("\n\n\tnode = %ld\n", node);
printf("\ttipo = %d\n", buffer->leaf);
printf("\tsucesr = %ld\n", buffer->sucesr);
printf("\tpredsr = %ld\n", buffer->predsr);
printf("\tnkv    = %d\n",  buffer->nkv);


	   if (buffer->leaf == LEAF ) {
         p = hghpnt(buffer);
         printa_chave(p, "HIGH");
      }

      nc = 1;

      while ( nc > 0 ) {
         printf("\tNumero da chave: ");
         gets(aux);
         if ( aux[0] == ' ' || aux[0] == '.')
            goto buff;

         if ( aux[0] == '+')
            nc++;
         else if ( aux[0] == '-')
            nc--;
         else
            nc = atoi(aux);

         if ( nc > 0 && nc <= buffer->nkv) {
            sprintf(aux, "expval de %d", nc);
            p = expval(buffer, nc);
            printa_chave(p, aux);
         }
      }
buff :
      printf("\tProximo buffer : ");
      gets(aux);

      if ( aux[0] == '.' )
         break;

      if ( aux[0] == '+')
         node = buffer->sucesr;
      else if ( aux[0] == '-')
         node = buffer->predsr;
      else
         node = atol(aux);
   }

	if (!node)	/* then no leaf node found */
		terr(215);

	return(1);
}


VOID printa_chave(pp, str)
TEXT *pp, *str;
{ 
   UTEXT *p;
   LONG *l;
   l = (LONG *)(pp - 4);
   p = (UTEXT *)pp;

   printf("\t%6.6ld   %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x   - %s\n",

      *l, *(p+0), *(p+1), *(p+2), *(p+3), *(p+4), *(p+5), *(p+6), *(p+7), str);

}
#endif
