/*
 *	user error codes
 *
 *	c-tree(TM)	Version 4.3
 *			Release B
 *			May 12, 1988 17:30
 *
 */


		/* USER ERROR CODES */

#define NO_ERROR	0

#define	KDUP_ERR	2	/* key value already exists */

#define	KMAT_ERR	3	/* could nor delete since pntr's don't match */
#define	KDEL_ERR	4	/* could not find key to delete */
#define	KBLD_ERR	5	/* cannot call delete w/o verification with  */
				/* duplicate keys */

/* *** 8 is reserved for NO_RQST defined in CTCOMM.H *** */

#define SPAC_ERR	10	/* INTREE parameters require too much space */
#define SPRM_ERR	11	/* bad INTREE parameters */
#define FNOP_ERR	12	/* could not open index file */
#define FUNK_ERR	13	/* unknown file type */
#define FCRP_ERR	14	/* file corrupt at open */
#define FCMP_ERR	15	/* file has been compacted */
#define KCRAT_ERR	16	/* could not create index file */
#define DCRAT_ERR	17	/* could not create data file */
#define KOPN_ERR	18	/* tried to create existing index file */
#define DOPN_ERR	19	/* tried to create existing data file */
#define KMIN_ERR	20	/* key length too large for node size */
#define DREC_ERR	21	/* record length too small */
#define FNUM_ERR	22	/* file number out of range */
#define KMEM_ERR	23	/* illegal index member info */
#define FCLS_ERR	24	/* could not close file */
#define FACS_ERR	26	/* file number not active */
#define LBOF_ERR	27	/* drn before beginning of data records */
#define ZDRN_ERR	28	/* zero drn in ADDKEY */
#define ZREC_ERR	29	/* zero drn in data file routine */
#define LEOF_ERR	30	/* drn exceeds logical end of file */
#define DELFLG_ERR	31	/* flag not set on record in delete chain */
#define DDRN_ERR	32	/* attempt to delete record twice in a row */
#define DNUL_ERR	33	/* attempt to use NULL ptr in read/write */
#define PRDS_ERR	34	/* predecessor repeat attempts exhausted */
#define SEEK_ERR	35	/* seek error  */
#define READ_ERR	36	/* read error  */
#define WRITE_ERR	37	/* write error */
#define VRTO_ERR	38	/* could not convert virtual open to actual */
#define FULL_ERR	39	/* no more records availble */
#define KSIZ_ERR	40	/* key recsiz in OPNIDX too large */
#define UDLK_ERR	41	/* could not unlock data record */
#define DLOK_ERR	42	/* could not obtain data record lock */
#define FVER_ERR	43	/* index file  -  version incompatibility */
#define OSRL_ERR	44	/* data file serial number overflow */
#define KLEN_ERR	45	/* key length exceeds MAXLEN parameter */
#define	FUSE_ERR	46	/* file number already in use.		  */
#define FINT_ERR	47	/* c-tree has not been initialized	  */
#define FMOD_ERR	48	/* file mode incompatibility		  */
#define	FSAV_ERR	49	/* could not save file			  */
#define LNOD_ERR	50	/* could not lock node */
#define UNOD_ERR	51	/* could not unlock node */
#define KTYP_ERR	52	/* variable length keys disabled in  CTOPTN.H */
#define FTYP_ERR	53	/* variable length records disabled in   "    */
#define REDF_ERR	54	/* attempt to write a read only file	      */
#define DLTF_ERR	55	/* file deletion failed			      */
#define DLTP_ERR	56	/* file must be opened exclusive for delete   */
#define DADV_ERR	57	/* attempt to write or delete w/o proper lock */

#define SERIE_ERR  58 /* numero de serie incompativel */
#define IDBASE_ERR 59 /* identificacao da base incompativel */
#define FPERM_ERR  60 /* Sem permissao (sist operacional) p/ acessar file */

            /* or with a conflicting lock (server only)   */
				/* see CHECK_LOCK/MUST_LOCK in CTSRVR.C	      */


#define RRLN_ERR	120	/* data record length exceeds rebuild max  */
#define RSPC_ERR	121	/* not enough space for sort area.	   */
#define RMOD_ERR	122	/* attempt to change fixed vs variable len */
#define	RVHD_ERR	123	/* var length header has bad record mark   */
#define INIX_ERR	124	/* # of indices does not match (OPNIFIL)   */
#define IINT_ERR	125	/* c-tree already initialized		   */

#define VMAX_ERR	140	/* variable record length too long */
#define	VDLK_ERR	146	/* could not update free space info */
#define VDLFLG_ERR	147	/* space to be reused is not marked deleted */
#define	VLEN_ERR	148	/* WRTVREC cannot fit record at recbyt */
#define	VRLN_ERR	149	/* varlen less than minimum in ADDVREC */
#define	VBSZ_ERR	153	/* buffer too small in REDVREC */
#define	VRCL_ERR	154	/* zero length record in REDVREC */
#define	VFLG_ERR	158	/* REDVREC record not marked active */
#define	VPNT_ERR	159	/* zero recbyt value */


/* end of cterrc.h */
