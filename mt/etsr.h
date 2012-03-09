
#include "window.h"

/*                      FILE  sr.h                                        */
/*Copyright 1989, 1991 by English Knowledge Systems, Inc. All Rights Reserved.*/

extern char *Sr_assign[26];     /* assigned variable pointers             */
extern short Sr_metaflag;       /* whether metacharacters are on or off   */
extern char  Sr_eol;            /* additional end-of-line (e.g. newline)  */
extern short Sr_shortmatch;     /* Do a short or a longest match.         */

/* This is the structure of a compiled pattern.                           */
#define SR_CPAT struct pnode
SR_CPAT {
        SR_CPAT *parent;     /* parent pointer                            */
        SR_CPAT *left;       /* left  operand pointer                     */
        SR_CPAT *right;      /* right operand pointer                     */
        char    *asgpos;     /* the assigned postion                      */
        short    alter;      /* alternative count                         */
        short    ileft;      /* left integer                              */
        short    iright;     /* right integer                             */
        char     type;       /* type field                                */
        };

#ifdef PROTO_ANSI

SR_CPAT *sr_compile(char *pat);
int      sr_search(SR_CPAT *re, char *str, char **str_st);
int      sr_replace(char *old_str,
                    char *old_str_st,
                    int   len,
                    char *new_str,
                    char *sub_str, int *max );
void     sr_free_re(SR_CPAT *re);

#else /* PROTO_ANSI */

SR_CPAT *sr_compile( );
int      sr_search( );
int      sr_replace( );
void     sr_free_re( );

#endif

