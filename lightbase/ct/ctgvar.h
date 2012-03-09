/*
 *	global variables
 *
 *	c-tree(TM)	Version 4.3
 *			Release B
 *			May 12, 1988 17:30
 *
 */

			/* Global Variables */

EXTERN UCOUNT    ct_finger;  	/* identificacao para base comercial */
EXTERN UCOUNT    ct_release;  /* identificacao para base comercial */
EXTERN ULONG     ct_sernum;   /* guarda meio da chave infocon */

EXTERN VRLEN    ct_tamlast;	/* Tamanho do ultimi registro variavel lido */
EXTERN COUNT    ct_maxuser;	/* Numero maximo de usuarios permitidos */
EXTERN COUNT    ct_ignoraupd;	/* Ignora flag de arquivo corrompido */
EXTERN COUNT    ct_updwarning;/* Avisa que arquivo pode estar bichado */
EXTERN COUNT    ct_semseguranca; /* sem gravacoes de FLUSF */
EXTERN COUNT    ct_noerror;	/* Retorno dummy ok para funcoes dummys */

EXTERN TREEBUFF *ct_btree;	   /* ptr to beg of b-tree buffers		*/
EXTERN KEYFILE  *ct_key;	   /* ptr to beg of ct_key file structures	*/
EXTERN DATFILE  *ct_dat;	   /* ptr to beg of data file structures	*/
EXTERN VATFILE  *ct_vat;	   /* ptr to beg of var data file struc	*/
EXTERN TEXT	    *ct_origin;   /* ptr to main index buffer area	*/

EXTERN LONG    ct_npath[MAXLEV]; /* array to trace path down B-tree. Assumes*/
				                     /* maximum of MAXLEV - 1 levels in B-tree. */

EXTERN TEXT ct_indkey[MAXLEN+1]; /* storage for key values found during	*/
				                     /* indexing operations 			*/
EXTERN COUNT ct_lenkey;          /* ANANIAS tamanho real da chave */

EXTERN TEXT ct_dupkey[MAXLEN+1]; /* for searches of duplicate keys */
EXTERN TEXT spkey[MAXLEN+1];     /* temporary storage for key values during*/
											/* node updates 			   */


EXTERN COUNT ct_elm;		   /* position within B-Tree node	*/
EXTERN COUNT ct_tky;		   /* result of comparison between target	*/
				               /* value and index entry.		*/
				               /* ct_tky < 0 => target < index entry	*/
				               /*	  = 0 => target = index entry	*/
				               /*	  > 0 => target > index entry 	*/
EXTERN COUNT ct_tkp;		   /* previous value of ct_tky		*/

EXTERN COUNT ct_mxfil;	   /* maximum files specified in intree. 	*/
EXTERN COUNT ct_mxbuf;	   /* maximum buffers specified in intree 	*/
EXTERN COUNT ct_ndsec;	   /* # of sectors per node specified in intree */
EXTERN COUNT ct_numvfil;   /* number of virtual files open		*/

EXTERN LONG    ct_lnode;   /* last node found during walk down tree */
EXTERN LONG    ct_fnode;   /* node found during search/retrieval    */
EXTERN UCOUNT  ct_lacs;	   /* counter to determine relative age of  */
				               /* buffer activity. each time buffer is used */
				               /* nodacs for buffer is set to ct_lacs++  */
EXTERN UCOUNT  ct_lfil;	   /* counter to track lru file for virtual */
				               /* opens and closes			 */

EXTERN LONG    ct_nwnod;   /* pointer to new node */
EXTERN COUNT   btlev;	   /* b-tree level counter. used as index of */
				               /* ct_npath				  */
EXTERN COUNT   uerr_cod;   /* user error cod */

EXTERN TEXT    ct_buf[CTBUFSIZ];/* temporary io buffer */
EXTERN COUNT   ct_ver;		/* defines configuration options */
EXTERN LONG    ct_gsrl;		/* next serial number for data file */

/* end of ctgvar.h  */
