#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2vet.h"
#include  "lb2prot1.h"

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

#ifdef MSDOS
   VOID help_auto(Campo *);
#else
   VOID help_auto( );
#endif

/*---------------------------------------------------------------------*/
/*   C A M P O S                                                       */
/*---------------------------------------------------------------------*/

F__GLB   Vet_itens *load_campos(sele, flag)
COUNT sele, flag;
{  COUNT modo, el_max, nelem, i;
   UCOUNT qtd_max;
   Campo **vetcmp, *pc;
   Vet_itens *v;

   if ( flag == 'E' ) {
      modo = 'E';
      el_max = MAX_CAMPOS;
      qtd_max = sizeof(Campo) * el_max + (el_max * TAM_TAM);
   }
   else {
      modo = 'F'; /* fixo */
      el_max = 0 ;  /* a funcao descobre */
      qtd_max = 0;
   }

   v = load_vet('C', 0, modo, v_bases[sele]->bs_cppos, el_max, qtd_max);

   if ( v != NULL ) {
      vetcmp = (Campo **)v->vi_vtit;
      nelem  = v->vi_nelem;
      for ( i = 0; i < nelem; i++) {
         pc = vetcmp[i];
         pc->cp_olddata = NULL;
         pc->cp_newdata = NULL;
         pc->cp_oldtam  = 0;
         pc->cp_newtam  = 0;
         pc->cp_adoc    = NULL;
         pc->cp_pttab   = NULL;
         pc->cp_2futuro = 0L;
         if ( WCB != NULL && c_bases[sele]->cb_tpopen == UTILIZACAO ) {
            /* normaliza nome do campo */
            prep_palavra(pc->cp_nome);
         }

         help_auto(pc);   /* coloca help e autonum no lugar */
      }
   }

   return(v);
}

F__LOC   VOID help_auto(pc)
Campo *pc;
{  COUNT i;
   TEXT *p;
   TEXT u_help[TD+1];

   u_help[0] = '\0';

   /* retira help da descricao e coloca em campo proprio */
   p = pc->cp_desc;
   if ( *p == '[' ) {
      p++;

      for ( i=0; i < TD && *p != '\0' && *p != ']'; i++ )
         u_help[i] = *p++;

      u_help[i] = '\0';
      if ( *p == ']')
         p++;

      strcpy(pc->cp_desc, p);
   }

   if ( pc->cp_help[0] == '\0' ) {
      u_help[8] = '\0';
      strcpy(pc->cp_help, u_help);
   }
   /* retira autonum da descricao e coloca em campo proprio */
   if ( pc->cp_critica == 'A' ) {
      p = strchr(pc->cp_desc, ',');
      if ( p != NULL ) {
         *p = '\0';
         p++;
         strncpy(pc->cp_param.tab_auto.autonum, p, 25);
         pc->cp_param.tab_auto.autonum[24] = '\0';
      }
   }

}

F__GLB   VOID rlse_campos(sele)
COUNT sele;
{
   if ( v_campos[sele] != NULL ) {
      rlse_vet(v_campos[sele]);
      v_campos[sele] = NULL;
   }
}



/*---------------------------------------------------------------------*/
/*   V I S T A S                                                       */
/*---------------------------------------------------------------------*/

F__GLB   Vet_itens *load_vistas(sele, flag)
COUNT sele, flag;
{  COUNT modo, el_max;
   UCOUNT qtd_max;
   Vet_itens *v;

   if ( flag == 'E' ) {
      modo = 'E';
      el_max = MAX_VISTAS;
      qtd_max = sizeof(Vista) * el_max + (el_max * TAM_TAM);
   }
   else {
      modo = 'F'; /* fixo */
      el_max = 0 ;  /* a funcao descobre */
      qtd_max = 0;
   }

   v = load_vet('V', 0, modo, v_bases[sele]->bs_vspos, el_max, qtd_max);

   return(v);
}




F__GLB   Vet_itens *load_itvis(vista, flag)
Vista *vista;
COUNT flag;
{  COUNT modo, el_max;
   UCOUNT qtd_max;
   Vet_itens *v;

   if ( flag == 'E' ) {
      modo = 'E';
      el_max = ap_mxitens;
      qtd_max = ap_sizeio;   /* sizeof(TTela) * el_max; nao pode por ter final variavel */
   }
   else {
      modo = 'F'; /* fixo */
      el_max = 0 ;  /* a funcao descobre */
      qtd_max = 0;
   }

   v = load_vet('I', 0, modo, vista->vis_itpos, el_max, qtd_max);

   if ( ap_extravet > 0 )
      ap_visdoc = load_buffer( 0, ET_MEM, (TEXT *) buf_iotxt, (LONG)ap_extravet);

   ap_extravet = 0;

   return(v);
}


/*---------------------------------------------------------------------*/
/*   R E L A T O R I O S                                               */
/*---------------------------------------------------------------------*/

F__GLB   Vet_itens *load_rels(sele, flag)
COUNT sele, flag;
{  COUNT modo, el_max;
   UCOUNT qtd_max;
   Vet_itens *v;

   if ( flag == 'E' ) {
      modo = 'E';
      el_max = MAX_REL;
      qtd_max = sizeof(Rel) * el_max + (el_max * TAM_TAM);
   }
   else {
      modo = 'F'; /* fixo */
      el_max = 0 ;  /* a funcao descobre */
      qtd_max = 0;
   }

   v = load_vet('R', 0, modo, v_bases[sele]->bs_rlpos, el_max, qtd_max);

   return(v);
}




F__GLB   Vet_itens *load_itrel(rel, flag)
Rel *rel;
COUNT flag;
{  COUNT modo, el_max;
   UCOUNT qtd_max;
   Vet_itens *v;

   if ( flag == 'E' ) {
      modo = 'E';
      el_max = ap_mxitens;
      qtd_max = ap_sizeio;   /* sizeof(RTela) * el_max; nao pode por ter final variavel */
   }
   else {
      modo = 'F'; /* fixo */
      el_max = 0 ;  /* a funcao descobre */
      qtd_max = 0;
   }

   v = load_vet('L', 0, modo, rel->rel_itpos, el_max, qtd_max);

   if ( ap_extravet > 0 )
      ap_reldoc = load_buffer( 0, ET_MEM, (TEXT *) buf_iotxt, (LONG)ap_extravet);

   ap_extravet = 0;

   return(v);
}



