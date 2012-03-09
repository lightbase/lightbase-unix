#include  "lb2all.h"

#include  <time.h>
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"


extern COUNT sai_stat;

#define   C_DEMO     "OK !"
#define   C_OK       "Copia invalida !"

/*
#define   DATA_BASE   ( 694242001L )
#define   DATA_FIM    ( DATA_BASE + (8L * 2592000L))
*/

#define   PRIM_JAN   ( 694242001L)
#define   PRIM_AGO   ( 712641617L)
#define   PRIM_SET   ( 715320052L)
#define   PRIM_OUT   ( 717912072L)
#define   PRIM_NOV   ( 720594097L)
#define   PRIM_DEZ   ( 723186003L)

#define   JAN_93     ( 725864610L)
#define   JAN_94     ( 757400423L)

#define   DATA_EXPIRA "01/01/94"

#define   DATA_BASE  JAN_93
#define   DATA_FIM   JAN_94

/* se copia demostracao so funciona dentro do intervalo definido */
#ifdef ENTRA_TESTE_DATA
COUNT data_demo_ok()
{  COUNT ret = 0;
   TEXT *p;
   time_t secsnow;


   p = C_DEMO;

   time(&secsnow);

   if ( ap_aprovado[0] == p[0]  &&  ap_aprovado[1] == p[1]  &&
        ap_aprovado[2] == p[2]  &&  ap_aprovado[3] == p[3]  &&
        ap_aprovado[4] == p[4] ) {
      if ( secsnow > DATA_BASE && secsnow < DATA_FIM ) {
         extern TEXT data_limite[];
         strcpy(data_limite, DATA_EXPIRA);   /* data limite para imprimir quando sair */

         ret = 1;
      }
   }


   return( ret );
}
#endif


LONG cai_fora(nreg)
LONG nreg;
{  COUNT mr=50;
   LONG  max_regs;
#ifdef MSDOS
   COUNT zoneia(COUNT, COUNT);
#else
   COUNT zoneia( );
#endif

#define   MAX_REGS_DEMO  2 /* * 50 */


   /* nao vale nada */
   if ( mr < 30 )
      mr = 18;
   else
      mr = 50;

   max_regs = zoneia(MAX_REGS_DEMO, mr); /* so multiplica um pelo outro */

   if ( nreg > max_regs ) {
      sai_stat = 1003;
      encerra();
      win_quit();
      exit(1003);
   }
   else
      nreg--;

   return(nreg);
}


COUNT zoneia(x, y)
COUNT x,y;
{  COUNT i=234, j, n=4;
   
   j=i;

   while (n-- > 1 ) {
      j -= i;
      i = j-3+i;
   }

   return(x * y);   
}

#ifdef DISCADOR
#ifdef MSDOS
   VOID f_discagem( VOID );
   COUNT lin_disca( TEXT *, COUNT, TEXT *, COUNT);
#else
   VOID f_discagem( );
   COUNT lin_disca( );
#endif

VOID f_discagem( )
{  TEXT buf[200];
   debug ("entrei em f_discagem");
   buf[0]= '\0';
   if ( lin_disca( "TEXTOX", 1, buf, 200) > 0 ) 
      debug(buf);
   else 
      debug("linha 1 invalida");

   if ( lin_disca( "TEXTOX", 2, buf, 200) > 0 ) 
      debug(buf);
   else 
      debug("linha 2 invalida");

   if ( lin_disca( "TEXTOX", 10, buf, 200) > 0 ) 
      debug(buf);
   else 
      debug("linha 10 invalida ");
}


COUNT lin_disca( nome_campo, num_linha, buf, max_buf)
TEXT *nome_campo;
COUNT num_linha;
TEXT *buf;
COUNT max_buf;
{  COUNT ret = -1, ind_campo, num, tl;
   Campo **vtcampos, *pc;
   TEXT linha[300];
   ARG *adoc;

   vtcampos = pos_vetcmp(BASE_EDIT);

   ind_campo = ver_campo(BASE_EDIT, nome_campo, &num);

   if ( ind_campo < 0 ) {
      goto fim;
   }

   pc = vtcampos[ind_campo];

   if ( pc->cp_tipo != 'T' ) {
      goto fim;
   }

   if ( (adoc = pc->cp_adoc) == NULL ) {
      goto fim;
   }

   if ( buf == NULL ) {
      ret = adoc->t_lines;
      goto fim;
   }

   tl = lin_texto( adoc, num_linha, linha, 1);
   
   if ( tl <= 0 ) {
      goto fim;
   }

   strncpy(buf, linha, max_buf);

   buf[max_buf - 1] = '\0';

   ret = strlen(buf);

fim :

   return(ret);
}

#endif
