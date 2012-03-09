/*
 *	portable specifications: automatically included via ctstdr.h
 *
 *	c-tree(TM)	Version 4.3
 *			Release B
 *			May 12, 1988 17:30
 *
 */

/*
 * Remove the comments from the following define to permit variable
 * length records larger than 64K bytes.  Only effective on machines
 * with 32 bit unsigned integers (i.e., sizeof(unsigned int) == 32).
 *

#define LONGVARD

 *
 */

#define	FAST	register    	/* register variable	   */
#define  PFAST	register			/* register parameter	*/
#define	EXTERN	extern		/* External variable	   */
#define	GLOBAL	/* */			/* Global variable	   */
#define	LOCAL	static			/* Local to module	   */
#define  VOID	void		      /* if void is not supported, change to int */

typedef short		      COUNT;
typedef unsigned short	UCOUNT;  /* some 16 bit compilers might  */
				                     /* require unsigned int instead */
#ifndef LONGVARD
   typedef unsigned short	VRLEN;
#else
   typedef unsigned int	VRLEN;
#endif

typedef char		TEXT;

#ifdef CT_ANSI
   typedef FILE	       *RNDFILE;
   typedef size_t        SIZE;
#else
   typedef int		RNDFILE;
   typedef unsigned int  SIZE;
#endif

typedef int		WORD;
typedef long	POINTER,LONG;

typedef long		      DATET;
typedef unsigned long	ULONG;	/* if unsigned long not supported, make long */
typedef unsigned char	UTEXT;	/* if unsigned char not supported, make char */
typedef char	         *TEXTPTR;

#define FOREVER 	for (;;)

#ifdef YES
#undef YES
#endif
#ifdef NO
#undef NO
#endif
#define	YES	1			/* TRUE	 */
#define	NO	   0			/* FALSE */

#define EXCLUSIVE 	0x0000		/* file in locked mode	*/
#define SHARED		   0x0001		/* file in shared mode	*/
#define COMPLETE	   EXCLUSIVE
#define PARTIAL		SHARED
#define VIRTUAL		0x0000		/* file is virtually opened  */
#define PERMANENT	   0x0002		/* file is physically opened */
#define FIXED		   0x0000		/* fixed length data	     */
#define VLENGTH		0x0004		/* variable length data	     */
#define READFIL		0x0008		/* read only file lock	     */
#define NONEXCLUSIVE	(READFIL | SHARED)

#define REGADD   	0	/* See ADDKEY: split full nodes in half     */
#define INCADD   	1	/* split nodes to handle increasing values  */
#define DECADD   	2	/* split nodes to handle decreasing values  */

#define FREE		0	/* free all pending data record locks	    */
#define RESET		1	/* same as FREE followed by ENABLE	    */
#define ENABLE		2	/* enable data record locks		    */
#define SUSPEND	3	/* temporarily suspend enable mode	    */
#define RESTORE	4	/* restore enable mode			    */
#define RESTRED	5	/* restore read locks			    */
#define READREC	6	/* change to read only record locks	    */


/* end of ctport.h */
