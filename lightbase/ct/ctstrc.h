/*
 *	data structures
 *
 *	c-tree(TM)	Version 4.3
 *			Release B
 *			May 12, 1988 17:30
 *
 */


			/* Data Structures */

typedef struct trebuf {	   /*********** B-Tree Buffer	***************/
	LONG     nodeid;	         /* node number 				*/
	COUNT    keyid;	         /* key number 				   */
	UCOUNT   nodacs;	         /* access counter for LRU routine	*/
	COUNT    klen;	            /* key length 				   */
	COUNT    maxkv;	         /* maximum key values per node		*/
	UCOUNT   maxb;	            /* maximum bytes per node		*/
	COUNT    ktipe;
	UCOUNT   begbyt;	         /* beginning byte of cur var len ct_key	*/
	COUNT	   actlen;	         /* stored length of cur var len ct_key	*/
	TEXT	   update;	         /* node update indicator 'y/n' 		*/
	TEXT    *ct_kyval;	      /* pointer to key values for node	*/
	TEXT    *nodorg;	         /* pointer to node ct_origin for I/O	*/
	TEXT	   confg;	         /* regular or duplicate ct_key leaf	*/
	COUNT    lstpos;	         /* cur var len ct_key pos#			*/
/*	COUNT	   cursfx; */	      /* cur var len ct_key suffix count		*/
	TEXT	   keyexp[MAXLEN+4]; /* var ct_key - expansion	*/
	TEXT     len_exp;		   	/* Tamanho efetivo (sem compactacao) da chave) */

/* beginning of permanent node */

	LONG     sucesr;           /* right sibling 			*/
	LONG     predsr;           /* left sibling 			*/
	COUNT    nkv;	            /* number of key values 		*/
	UCOUNT   nkb;	            /* number of ct_key bytes			*/
	TEXT	   leaf;	            /* leaf indicator LEAF/NOLEAF		*/
	TEXT	   bmem;	            /* member number			*/
} TREEBUFF;

typedef struct wfils {	/* File Control Structure 		*/
	COUNT	   verson;	/* ATT. 8 BYTES   configuartion options at create	*/
	UCOUNT	extsiz;	/*                extend file (chunk) size		*/
	UCOUNT	reclen;	/*                data record length			*/
	UCOUNT	recsiz;	/*                node record size			*/

	COUNT	   flmode;	/* file mode (virtual, etc)		*/
	COUNT	   clstyp;	/* flag for type of virtual close	*/
	TEXT	   updflg;	/* update (corrupt) flag		*/
	TEXT	   ktype;	/* file type flag			*/

	POINTER	sernum;	/* serial number			*/
	POINTER	delstk;	/* top of delete stack: fixed len data	*/
	POINTER	numrec;	/* last byte offset written		*/
	POINTER	phyrec;	/* last byte offset of file		*/
	LONG     root;	   /* B-Tree root 				*/
	LONG   	nodstk;	/* node delete stack (unused)		*/

   LONG     numserie; /* numero de serie do software */
   UCOUNT   baseid;   /* Identificacao da base */
   COUNT    pega1;    /* xor de numserie e baseid */

	COUNT	   nmem;	   /* number of members			*/
	COUNT	   kmem;	   /* member number			*/
	UCOUNT   finger;	/* Finger do arquivo - Ananias */
	UCOUNT	maxkbl;	/* maximum key bytes leaf-var		*/
	UCOUNT	maxkbn;	/* maximum key bytes non leaf-var 	*/
	COUNT    maxkvl;	/* max key values per leaf node-fixed	*/
	COUNT    maxkvn;	/* max key values per non leaf node-fxd	*/
	COUNT    length;	/* key length 				*/

	UTEXT	   release[2];	/* Uso como COUNT para nao perder compatibilidade */

	/* end of permanent information */

	TEXT	   flname[MAX_NAME];	/* file name		*/
	POINTER	sekpos;	      /* current abs file position		*/
	LONG   	retnod;	      /* current node				*/

	COUNT	   filnum;	/* file number assigned by user		*/
	UCOUNT	usecnt;	/* lru age				*/
	UCOUNT   lokcnt;  /* keep track of pending locks		*/
	COUNT	   retelm;	/* current node element			*/
	TEXT	   chnacs;	/* 'y' 'n' 'v'  ||||||  'w' 'm' 'c'  */
	RNDFILE	fd;	/* file descritor			*/
} CTFILE;

#define	DATFILE		CTFILE
#define	KEYFILE		CTFILE
#define	VATFILE		CTFILE




/*---------------------------------------------------------------------*/
/* Registros variaveis                                                 */
/*---------------------------------------------------------------------*/

#ifndef LONGVARD

typedef struct vrfmt {	/* beginning of var len records */
	UCOUNT	recmrk;	/* record marker		*/
			            /* 0xfafa active 0xfdfd deleted */
			            /* 0xfbfb available space node	*/
	VRLEN	trclen;	   /* total record length		*/
	VRLEN	urclen;	   /* utilized record length	*/
} VHDR;

#else

typedef struct vrfmt {	/* beginning of var len records */
	VRLEN	trclen;	   /* total record length		*/
	VRLEN	urclen;	   /* utilized record length	*/
	UCOUNT	recmrk;	/* record marker		*/
			            /* 0xfafa active 0xfdfd deleted */
			            /* 0xfbfb available space node	*/
} VHDR;

#endif


#define	VDEL_FLAG	0xfdfd
#define  VACT_FLAG	0xfafa
#define	VNOD_FLAG	0xfbfb
#define	SIZVHDR		(sizeof(UCOUNT) + 2 * sizeof(VRLEN))



typedef struct lok {	/* ISAM Lock Table Entry		*/
	COUNT    datfnm;	/* data file number			*/
	COUNT	   ltltyp;	/* lock type				*/
	POINTER  recnum;	/* record number			*/
} LOKS;



typedef struct keyops {
	COUNT	kokeyn;	/* key number				*/
	COUNT	komode;	/* mode: add or delete			*/
	POINTER	kopntr;	/* associated byte offset		*/
	TEXT	kobufr[MAXLEN];	/* key value buffer		*/
} KEYOPS;

typedef struct reclok {
	POINTER	rlbpos;	/* byte position			*/
	struct
	reclok *rllink;	/* forward link				*/
	struct
	reclok *rlrlnk;	/* reverse link				*/
	COUNT	rlusrn;	/* user number of owner			*/
	COUNT	rltype;	/* write lock or read lock		*/
} RECLOK;

typedef struct usrcon {
	POINTER	ucbpos;	/* byte offset of segmented record	*/
	TEXT   *ucmsgp;	/* usr message buffer address		*/
	UCOUNT	ucepos;	/* ucbpos + length of record		*/
	COUNT	ucsgop;	/* segment operation code		*/
	COUNT	ucslep;	/* user is sleeping on usrn		*/
	COUNT	ucfiln;	/* file number				*/
} USRCON;



typedef struct {		/* Multiple-Set Buffer		*/
	COUNT	qlen;		/* significant length		*/
	COUNT	qkey;		/* sequence keyno + 1		*/
	TEXT	qbuf[MAXLEN];	/* ct_key buffer			*/
	} SAVSET;


/* given pointer defined as

	TREEBUFF *buffer;

   and an integer defined as

	COUNT n;

   then the construct to reference the nth key value stored in the b-tree
   node is

		valpnt(buffer,n)

   which evaluates to a TEXT pointer; and the constructs to return the nth
   pointer stored in the b-tree node are

		nodpnt(buffer,n), drnpnt(buffer,n)
  
   which evaluate to typedef's LONG    and POINTER, respectively.

***	end of ctstrc.h		***/
