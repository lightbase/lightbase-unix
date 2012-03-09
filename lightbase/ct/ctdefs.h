/* Define uteis para utilizar CTREE */

#define   COMPACTA      0
#define   TRANSPARENTE  1


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

