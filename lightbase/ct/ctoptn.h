#define LOW_HIGH  /* a principio considero esta opcao */

#ifdef PA800R
#undef LOW_HIGH
#define HIGH_LOW
#endif

#ifdef  AIX
#undef LOW_HIGH
#define HIGH_LOW
#endif

#ifdef  SUN
#undef LOW_HIGH
#define HIGH_LOW
#endif



#define NOTFORCE
#define NO_FLUSH
#define VARLDATA
#define PERC_D
#define VARLKEYS
#define NO_FLOAT
/*******   #define UNIFRMAT  atualmente nao tem compatibilidade mesmo ! ***/
#define PARMFILE
#define NO_RTREE
#define NO_SERVE


/* Ananias. Vide pg 6.1 */
#define NO_IDXENT



#define MAXLEN		64	   /* maximum key length			*/
#define MAXVFIL	255	/* maximum virtual files open		*/
#define MAX_NAME	64	   /* maximum file name length		*/


/* ***			Variable Length Key & Field Constants	    *** */

#define PADDING		' '	/* trailing ct_key padding			*/
#define FIELD_DELIM	'\0'	/* var length field delimiter		*/

/*
 * The following capacity constants are used only by the ISAM routines and
 * the REBUILD routines.
 */


/* ******************************************************************** */
/* ************************     S T O P    **************************** */
/* ***     D O   N O T   C H A N G E   T H E    F O L L O W I N G   *** */
/* ***   W I T H O U T   C H E C K I N G   W I T H   F A I R C O M  *** */
/* ******************************************************************** */

/* ***********     FPUTFGET CANNOT BE USED WITH THE SERVER    ********* */
#ifdef CTSERVER
#ifdef FPUTFGET
#undef FPUTFGET
#define FPUTONLY
#endif
#endif

/* ***********   MUSTFRCE indicates multi-user requirements  ********* */
#ifdef MUSTFRCE
#undef MUSTFRCE
#endif
#ifdef FPUTFGET
#define MUSTFRCE
#endif
#ifdef CTSERVER
#define MUSTFRCE
#endif

/* ***********  DOSFLUSH APPLIES ONLY TO FPUTONLY & FPUTFGET  ********* */
#ifdef NOTFORCE
#ifdef DOSFLUSH
#undef  DOSFLUSH
#define NO_FLUSH
#endif
#endif
/* ************* ************* ************* ************ ************* */

/* *************     UNIFRMAT APPLIES ONLY TO HIGH_LOW    ************* */
#ifdef LOW_HIGH
#ifdef UNIFRMAT
#undef  UNIFRMAT
#define NO_UNIFM
#endif
#endif
/* ************* ************* ************* ************ ************* */


#define  MAXLEV		10	   /* maximum height of B-Tree + 1		*/
#define	VARI_LEN	   (sizeof(VRLEN) + sizeof(LONG))
#define  STATUS  (2 * sizeof(COUNT) + 2 * sizeof(LONG) + 2 * sizeof(TEXT))
/* node info:	 nkv & nkb	     sucesr & predsr	   leaf & bmem    */

#ifdef DECALFA
#define HDRSIZ   94  /* era 110 mais estava melando o filename */
#else
#define HDRSIZ   64
#endif

/* permanent header information */
#define DHDRSIZ  HDRSIZ


#ifdef  FPUTFGET
#define PRDRPEAT	10	/* # of times to retry predsr node	*/
#else
#define PRDRPEAT	0
#endif

#define	MAXMEMB		31	/* maximum additional index members	*/



/* ******************************************************************** */
/* ***			SYMBOLIC CONSTANTS			   ***  */
/* ******************************************************************** */

#define	DRNZERO	 	(POINTER) 0
#define	NODEZERO	(LONG   ) 0

#define CTREAD	 0
#define CTWRITE	 1
#define CTNONE	 3
#define CTXTND	 4

#define LEAF     '\1'
#define NOLEAF   '\0'

#define ALPHAKEY	   '\0'
#define INTKEY		   '\1'
#define SFLOATKEY	   '\2'
#define DFLOATKEY	   '\3'
#define COL_PREFIX   '\4'	/* leading character compression	*/
#define COL_SUFFIX   '\10'	/*  8 decimal: padding compression	*/
#define COL_BOTH	   '\14'	/* 12 decimal: both of the above	*/
#define ALT_SEQ	   '\20'	/* 16 decimal: alternative col sequence	*/
#define MSK_ALT	   ~ALT_SEQ

#define DUPKEY       '\1'
#define NODUPKEY     '\0'

#define REGULAR      '\1'
#define LB_EXTRA     '\0'

#define DUPLEAF      '\0'
#define DUPNONL      '\3'

#define DAT_CLOSE    '\0'
#define IDX_CLOSE    '\1'
#define VAT_CLOSE    '\2'

#define SECSIZ       128		    /* logical sector size. DO NOT CHANGE.	*/
#define CTBUFSIZ     (4 * SECSIZ) /* ct_buf[] size */
#define UPDATED      C255	   	 /* C255 defined in ctcmpl.h */
#define DELFLG	      C255
#define COMPACT      '\143'  /* file compaction flag: must rebuild indices */
#define CMPMSK	      0x00ff

#define MAXAGE    0xffff	/* max age of lru counter before roll-over */
#define MAXSRLPOS 0xffff	/* no serial segment flag		   */

#define	LH_VER	 0x0001			/* LOW_HIGH option 		*/
#define	HL_VER	 0x0002			/* HIGH_LOW   "			*/
#define  CT_V1_0	 0x0004			/* version 4.1E/E2/F 4.3A*/

/*	end of ctoptn.h		*/
