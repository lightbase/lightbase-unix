
#include <stdio.h>
#include <stdlib.h>
#include "etsr.h"

void    test(char *str, char *pat, char *sub);

main() {

        /*      ---- The string to search in for the regular expression.*/
        /*      |        ---- The regular expression pattern.           */
        /*      |        |      ---- What gets substituted.             */
        /*      |        |      |                                       */
        test("aabada", "b.d", "ca");

        /* Note that a \\ needs to be used so that the 'C' compiller    */
        /* will leave a \ after it interprets the string.               */
        Sr_metaflag = 0;
        printf("test sr_metaflag\n");
        test("aabada", "b\\.d", "ca");
        Sr_metaflag = 1;

        Sr_shortmatch = 1;
        printf("test Sr_shortmatch = 1\n");
        test("aabaaada", "ba+", "ca");
        test("aabda", "ba+", "ca");
        test("aabaaada", "ba*", "ca");
        test("aabda", "ba*", "ca");

        Sr_shortmatch = 0;
        printf("test Sr_shortmatch = 0\n");
        test("aabaaada", "ba+", "ca");
        test("aabda", "ba+", "ca");
        test("aabaaada", "ba*", "ca");
        test("aabda", "ba*", "ca");

        test("aabada", "[acb].d", "ca");
        test("aabada", "[^acd].d", "ca");
        test("aabada", "[a-d].d", "ca");
        test("aabada", "[-acb].d", "ca");
        test("aabada", "[^-ac].d", "ca");
        test("aabada", "[acb-].d", "ca");
        test("aabada", "<2-4>", "ca");
        test("aabada", "<2,4>", "ca");
        test("aabada", "<2-~2>", "ca");
        test("aabada", "<2>", "ca");
        test("aabada", "<0>[a-z]{4}", "ca");
        test("aabada", "<0>[a-z]{4}d", "ca");
        test("aabada", "<1>", "ca");
        test("aabada", "<2>", "ca");
        test("aabada", "<0>a", "ca");
        test("aabada", "<1>a", "ca");
        test("aabada", "<2>b", "ca");

        /* Test for Sr_eol                                              */
        printf("Test Sr_eol\n");
        test("aabada", "<~2>", "ca");
        Sr_eol = '\n';
        test("aabada\n", "<~2>", "ca");
        Sr_eol = 0;

        test("aabaaada", "ba+", "ca");
        test("aabda", "ba+", "ca");
        test("aabaaada", "ba*", "ca");
        test("aabda", "ba*", "ca");
        test("aababada", "(ba)*", "ca");
        test("aababada", "aa(ba)*", "ca");
        test("aababada", "(ba)+", "ca");
        test("aabada", "aa(ba)*", "ca");
        test("aabada", "(ba)+", "ca");
        test("aada", "aa(ba)*", "ca");
        test("aada", "(ba)+", "ca");
        test("aabada", "(c|b)", "ca");
        test("aabada", "c|b", "ca");
        test("aabada", "(b|c)", "ca");
        test("aabada", "b|c", "ca");
        test("aabada", "(c|e|b|d|f)", "ca");

        /* Find the shortest match.                             */
        test("aabacabacada", "aa.{0,}c", "xx");
        test("aababababada", "aa(ba){2,4}", "ca");
        test("aababababada", "aa(ba){4,2}", "ca");
        test("aababababada", "aa(ba){4}", "ca");
        test("aababababada", "aa(ba){2}", "ca");
        test("aababababada", "aa(ba){2,}", "ca");
        test("aababababada", "aa(ba){,2}", "ca");
        test("aababababada", "aa(ba){4,}", "ca");
        test("aababababada", "aa(ba){,4}", "ca");
        test("aabada", "(ba)$c", "c<c>a");
        test("word reverse", "[a-z]+$a[^a-z]+$b[a-z]+$c", "<c><b><a>");
        printf("Sr_assign['a' - 'a'] = %s Sr_assign['c' - 'a'] = %s\n\n\n",
                Sr_assign['a' - 'a'], Sr_assign['c' - 'a']);
        test("abbaac", "[a-zA-Z]$a<a>", "<a>");
        test("remove double double words", "[a-z]+$a[^a-z]+<a>", "<a>");

#define LET     "[a-zA-Z]"
#define SPACE   "[^"LET"+"
#define WORD_A  LET"+$a"SPACE
#define DUP_A   WORD_A"(<a>"SPACE")*<a>"
        test("remove double double double words", DUP_A, "<a>");

        test("abcdefghij", "(ef[zg])|(hij)", "yy");
        test("abcdefghij", "(hij)|(e[zf]g)", "yy");
        test("abcdefghij", "hij|e[zf]g", "yy");

        test("abcdefghij abcdefghij", "<0>a", "1");
        test("abcdefghij abcdefghij", "<0>b", "1");
        test("abcdefghij abcdefghij", "<3-~4>", "1");
        test("abcdefghij abcdefghij", "j<~0>", "1");
        test("abcdefghij abcdefghij", "<3-5>j", "1");
        test("abcdefghij abcdefghij", "<3-8>j", "1");
        
        return(1);
        }

void
test(str, pat, sub)
char    *str;
char    *pat;
char    *sub;
{

        int      len;           /* The length of the match.             */
        char    *found;         /* Where in str the match was found.    */
        SR_CPAT *cpat;          /* The compiled pattern.                */
        char     buf[110];      /* Buffer for the changed string.       */
        int      max = 100;    
        
        printf("string:     %s\n", str);
        printf("pattern:    %s\n", pat);
        cpat = sr_compile(pat);
        found = str;
        len  = sr_search(cpat, str, &found);
        sr_free_re(cpat);       /* Free the compiled pattern.           */
        if (len == -1) {
                printf("pattern not found\n\n");
                return;
                }

        printf("found:      %.*s\n", len, found);
        sr_replace(str, found, len, buf, sub, &max );
        printf("substitute: %s\n", sub);
        printf("new str:    %s\n\n", buf);
        return;
        }
