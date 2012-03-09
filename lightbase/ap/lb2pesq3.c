#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

#include  "lb2pesq.h"
#include  "lb2prot1.h"


#define   SEG_MOR 100

extern WIN *jm_wait; /* janela para ficar rodando o ventidador */

#ifdef MSDOS
   SDINDEX *mont_vet(COUNT *, COUNT, SDINDEX *);
   VOID     aval_vet(COUNT , COUNT *, COUNT *, LONG, COUNT, COUNT, COUNT, COUNT, COUNT);
#else
   SDINDEX *mont_vet( );
   VOID     aval_vet( );
#endif



/*--------------------------------------------------------------------*/
/*  prototipos                                                        */
/*--------------------------------------------------------------------*/
#ifdef MSDOS
   SI    psqcmp_testa(SDINDEX *, SDINDEX *);
#else
   SI    psqcmp_testa( );
#endif



/*--------------------------------------------     conector: E            */
F__GLB   LONG e(l0, l1, l_res)
COUNT l0, l1, l_res;
{  LONG res;
   COUNT helice = 0, contador = PSQ_SLOT / 8;
   SDINDEX *p0, *p1;


   p0 = prim_sdi(l0);
   p1 = prim_sdi(l1);

   while ( p0 != NULL && p1 != NULL ) {

      if ( --contador == 0 ) {
         TEXT *p;
         p="|/-\\|/-\\";
         helice = helice++ & 0x7;
         if ( jm_wait != NULL ) {
            fwat(jm_wait->w_cx-1, jm_wait->w_cy, jm_wait);
            fwpc(*(p+helice), jm_wait);
            fwrefresh(jm_wait);
         }
         contador = PSQ_SLOT / 8;
      }

      if ( p0->registro < p1->registro )
         { p0 = prox_sdi(l0); continue;};
      if ( p0->registro > p1->registro )
         { p1 = prox_sdi(l1); continue;};

      add_sdi(l_res, p0) ;

      p0 = prox_sdi(l0);
      p1 = prox_sdi(l1);
   }

   res = vet_lista[l_res].tot_vet;

   return(res);
}

/*--------------------------------------------     conector: OU/XOU       */
F__GLB   LONG ou(l0, l1, l_res, exclusivo)
COUNT l0, l1, l_res, exclusivo;
{  COUNT ret;
   COUNT helice = 0, contador = PSQ_SLOT / 8;
   LONG res, pulareg;
   SDINDEX *p0, *p1;


   pulareg = 0L;

   p0 = prim_sdi(l0);
   p1 = prim_sdi(l1);

   while ( p0 != NULL || p1 != NULL ) {

      if ( --contador == 0 ) {
         TEXT *p;
         p="|/-\\|/-\\";
         helice = helice++ & 0x7;
         if ( jm_wait != NULL ) {
            fwat(jm_wait->w_cx-1, jm_wait->w_cy, jm_wait);
            fwpc(*(p+helice), jm_wait);
            fwrefresh(jm_wait);
         }
         contador = PSQ_SLOT / 8;
      }

      if ( p0 == NULL || p1 == NULL ) {
         if ( p0 == NULL ) ret = 1;
         else              ret = -1;
      }
      else  {
         if ( p0->registro > p1->registro )       ret = 1;
         else if ( p0->registro < p1->registro )  ret = -1;
         else {
            /* para o OU a ordem e importante para o XOU so interessa o reg */
            if ( ! exclusivo )
               ret = psqcmp_testa(p0, p1);
            else
               ret = 0;
         }
      }

      if ( ret > 0 ) {
         if ( ! exclusivo || p1->registro != pulareg )
            add_sdi(l_res, p1) ;
         p1 = prox_sdi(l1);
         continue;
      }
      else if ( ret < 0 ) {
         if ( ! exclusivo || p0->registro != pulareg )
            add_sdi(l_res, p0) ;
         p0 = prox_sdi(l0);
         continue;
      }
      else {
         if ( ! exclusivo )
            add_sdi(l_res, p0) ;
         else
            pulareg = p0->registro;
         p0 = prox_sdi(l0);
         p1 = prox_sdi(l1);
      }
   }

   res = vet_lista[l_res].tot_vet;

   return(res);
}

/*--------------------------------------------     conector: NOT           */
F__GLB   LONG not(l0, l1, l_res)
COUNT l0, l1, l_res;
{  COUNT ret;
   LONG res;
   COUNT helice = 0, contador = PSQ_SLOT / 8;
   SDINDEX *p0, *p1;


   p0 = prim_sdi(l0);
   p1 = prim_sdi(l1);

   while ( p0 != NULL || p1 != NULL ) {

      if ( --contador == 0 ) {
         TEXT *p;
         p="|/-\\|/-\\";
         helice = helice++ & 0x7;
         if ( jm_wait != NULL ) {
            fwat(jm_wait->w_cx-1, jm_wait->w_cy, jm_wait);
            fwpc(*(p+helice), jm_wait);
            fwrefresh(jm_wait);
         }
         contador = PSQ_SLOT / 8;
      }

      if ( p0 == NULL || p1 == NULL ) {
         if ( p0 == NULL ) break; /****************  finaliza aqui */
         else              ret = -1;
      }
      else  {
         if ( p0->registro > p1->registro )       ret = 1;
         else if ( p0->registro < p1->registro )  ret = -1;
         else                                     ret = 0;
      }

      if ( ret > 0 ) {
         p1 = prox_sdi(l1);
         continue;
      }
      else if ( ret < 0 ) {
         add_sdi(l_res, p0) ;
         p0 = prox_sdi(l0);
         continue;
      }
      else {
         p0 = prox_sdi(l0);
         /* p1 = prox_sdi(l1); nao pode avancar l1 */
      }
   }

   res = vet_lista[l_res].tot_vet;

   return(res);
}


/*--------------------------------------------     conector: NOCAMPO     */
F__GLB   LONG nocampo(l0, l1, l_res, dep_campo, dep_paragrafo, dep_frase)
COUNT l0, l1, l_res;
COUNT dep_campo, dep_paragrafo, dep_frase;
{  LONG res;
   SDINDEX *p0, *p1;

   p0 = prim_sdi(l0);
   p1 = prim_sdi(l1);

   while ( p0 != NULL && p1 != NULL ) {

      /* verifica se mesmo registro ******************/
      if ( p0->registro < p1->registro )   { p0 = prox_sdi(l0); continue;};
      if ( p0->registro > p1->registro )   { p1 = prox_sdi(l1); continue;};

      /*** registro sao eguais.    Testa Campo */

      if ( dep_campo ) {
         if ( p0->campo < p1->campo )      { p0 = prox_sdi(l0); continue; }
         else if ( p0->campo > p1->campo ) { p1 = prox_sdi(l1); continue; }

         /*** registro e campo sao iguais.    ************* Testa paragrafo */

         if ( dep_paragrafo ) {
            if ( p0->paragrafo < p1->paragrafo )      { p0 = prox_sdi(l0); continue; }
            else if ( p0->paragrafo > p1->paragrafo ) { p1 = prox_sdi(l1); continue; }

            /*** registro , campo e paragrafo sao iguais. ****  Testa frase */
            if ( dep_frase ) {
               if ( p0->frase < p1->frase )      { p0 = prox_sdi(l0); continue; }
               else if ( p0->frase > p1->frase ) { p1 = prox_sdi(l1); continue; }
            }
         }
      }

      switch( psqcmp_testa(p0, p1) ) {

         case -1 :   add_sdi(l_res, p0);
                     p0 = prox_sdi(l0);
                     break;

         case 0 :    add_sdi(l_res, p0);
                     p0 = prox_sdi(l0);
                     p1 = prox_sdi(l1);
                     break;

         case 1 :    add_sdi(l_res, p1);
                     p1 = prox_sdi(l1);
                     break;
      }
   }

   res = vet_lista[l_res].tot_vet;

   return(res);
}





/*--------------------------------------------     conector: ADJ e PROX  */
F__GLB   LONG adj_prox(l0, l1, l_res, n, ordem)
COUNT l0, l1, l_res;
COUNT n, ordem;
{  LONG res, reg;
   COUNT helice = 0, contador = PSQ_SLOT / 8;
   COUNT campo, paragrafo, frase;
   SDINDEX *p0, *p1;
   COUNT v0[SEG_MOR], v1[SEG_MOR];


   p0 = prim_sdi(l0);
   p1 = prim_sdi(l1);

   while ( p0 != NULL && p1 != NULL ) {

      if ( --contador == 0 ) {
         TEXT *p;
         p="|/-\\|/-\\";
         helice = helice++ & 0x7;
         if ( jm_wait != NULL ) {
            fwat(jm_wait->w_cx-1, jm_wait->w_cy, jm_wait);
            fwpc(*(p+helice), jm_wait);
            fwrefresh(jm_wait);
         }
         contador = PSQ_SLOT / 8;
      }

      /* verifica se mesmo registro ******************/
      if ( p0->registro < p1->registro )   { p0 = prox_sdi(l0); continue;};
      if ( p0->registro > p1->registro )   { p1 = prox_sdi(l1); continue;};

      /*** registro sao eguais. Testa Campo */

      if ( p0->campo < p1->campo )      { p0 = prox_sdi(l0); continue; }
      else if ( p0->campo > p1->campo ) { p1 = prox_sdi(l1); continue; }

      /*** registro e campo sao iguais.  ************* Testa paragrafo */

      if ( p0->paragrafo < p1->paragrafo )      { p0 = prox_sdi(l0); continue; }
      else if ( p0->paragrafo > p1->paragrafo ) { p1 = prox_sdi(l1); continue; }

      /*** registro , campo e paragrafo sao iguais. ****  Testa frase */
      if ( p0->frase < p1->frase )      { p0 = prox_sdi(l0); continue; }
      else if ( p0->frase > p1->frase ) { p1 = prox_sdi(l1); continue; }


      reg       = p1->registro;
      campo     = p1->campo;
      paragrafo = p1->paragrafo;
      frase     = p1->frase;

      p0 = mont_vet(v0, l0, p0);
      p1 = mont_vet(v1, l1, p1);

      aval_vet(l_res, v0, v1, reg, campo, paragrafo, frase, n, ordem);
   }

   res = vet_lista[l_res].tot_vet;

   return(res);
}

F__LOC   SDINDEX *mont_vet(vet, lista, p)
COUNT vet[];
COUNT lista;
SDINDEX *p;
{  LONG reg;
   COUNT campo, parag, frase;
   COUNT ind;

   reg   = p->registro;
   campo = p->campo;
   parag = p->paragrafo;
   frase = p->frase;

   ind = 0;
   vet[ind++] = p->sequencia;

   while ( (p = prox_sdi(lista)) != NULL ) {

      if ( reg   != p->registro ) break;
      if ( campo != p->campo    ) break;
      if ( parag != p->paragrafo) break;
      if ( frase != p->frase    ) break;

      vet[ind++] = p->sequencia;
   }

   vet[ind] = -1;

   return(p);
}



F__LOC   VOID aval_vet(l_res, v0, v1, reg, campo, parag, frase, n, ordem)
COUNT l_res, v0[], v1[];
LONG reg;
COUNT campo, parag, frase, n, ordem;
{  COUNT n0, n1, i0, i1, dif;
   COUNT st0[SEG_MOR], st1[SEG_MOR];  /* flags de status se deve ir para resultado */
   SDINDEX sdi;


   sdi.registro  = reg;
   sdi.campo     = campo;
   sdi.paragrafo = parag;
   sdi.frase     = frase;

   for (n0 = 0; v0[n0] >= 0 ; n0++)  /* verifica quantidade e zera flag */
   {
      st0[n0] = 0;
      if ( n0 >= SEG_MOR )
         debug("SEG_MOR deve ser no minimo %d",n0);
   }

   for (n1 = 0; v1[n1] >= 0 ; n1++)
   {
      st1[n1] = 0;
      if ( n1 >= SEG_MOR )
         debug("SEG_MOR deve ser no minimo %d",n1);
   }


   for (i0 = 0; i0 < n0; i0++) {
      for (i1 = 0; i1 < n1; i1++) {
         dif = v1[i1] - v0[i0];
         if ( ordem == 0 ) dif = abs(dif);
         if ( dif > 0 && dif <= n ) {
            st0[i0] = 1;
            st1[i1] = 1;
         }
      }
   }

   i0=i1=0;

   while ( v0[i0] != 9999 && v1[i1] != 9999 ) {

      if ( v0[i0] < v1[i1] ) {
         if ( st0[i0] == 1 ) {
            sdi.sequencia = v0[i0];
            add_sdi(l_res, &sdi);
         }
         i0++;
      }
      else {
         if ( st1[i1] == 1 ) {
            sdi.sequencia = v1[i1];
            add_sdi(l_res, &sdi);
         }
         i1++;
      }

      if ( v0[i0] == -1 ) v0[i0] = 9999;
      if ( v1[i1] == -1 ) v1[i1] = 9999;

   }
}





