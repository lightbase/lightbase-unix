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

/*---------------------------------------------------------------------*/
/*   C A M P O S                                                       */
/*---------------------------------------------------------------------*/


F__GLB   Campo **pos_vetcmp(sele)
COUNT sele;
{

   if ( v_campos[sele] != NULL )
      return((Campo **)v_campos[sele]->vi_vtit);
   else
      return((Campo **)NULL);
}


F__GLB   COUNT qtd_vetcmp(sele)
COUNT sele;
{
   if ( v_campos[sele] != NULL )
      return(v_campos[sele]->vi_nelem);
   else
      return(0);
}


F__GLB   COUNT acha_campo( sele, num )
COUNT sele, num;
{  COUNT  ret, i, qtd;
   Campo **campos;

   campos = pos_vetcmp(sele);
   qtd = qtd_vetcmp(sele);

   ret = -1;

   for( i = 0;  i  <  qtd;  i++ ) {
      if ( campos[i]->cp_num  ==  num ) {
         ret = i;
         break;
      }
   }

   return ( ret );
}


F__GLB   COUNT ver_campo(sele, nome_campo, num)
COUNT sele;
TEXT *nome_campo;
COUNT *num;
{     COUNT ret, i, numcps;
      Campo **vtcampos;
      TEXT *p, nome_vet[30], aux[30];

      vtcampos = pos_vetcmp(sele);
      numcps   = qtd_vetcmp(sele);

      *num = 0;
      ret = -1;

      if ( nome_campo != NULL && val_indice(nome_campo) == OK) {

         strcpy(aux, nome_campo);
         prep_palavra(aux);

         /* localiza nome fornecido */
         for ( i = 0; i < numcps; i++) {
            if ( WCB != NULL && c_bases[sele]->cb_tpopen == UTILIZACAO ) {
               p = vtcampos[i]->cp_nome;
            }
            else {
		         strcpy(nome_vet, vtcampos[i]->cp_nome);
               prep_palavra(nome_vet);
               p = nome_vet;
            }

		      if ( strcmp(aux, p) == 0 ) {
               *num = vtcampos[i]->cp_num;
               ret = i;
               break;
            }
	      }
      }

      return(ret);
}

F__GLB   TEXT *sel_campo(sele, ptr_cmp, flag)
COUNT sele;
Campo **ptr_cmp;
COUNT flag;  /* se flag == 1 so reseta o static ult_selec */
{  COUNT num_campos, n_campo;
   static TEXT campo[TE+1];
   static COUNT ult_selec;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *p_ret;
   Tab   t;
   WIN  *jm = NULL;
   win_ajuda = "SELCAMPO";

   if ( flag == 1 ) {
      ult_selec = 0;
      p_ret = NULL;
      goto fim;
   }

   num_campos = qtd_vetcmp(sele);

   def_tabela( &t, 44, 7, TE + 1, 10,
		         num_campos, num_campos, X14,
               MS_CAMPO, MS_BARRA, 1, MS_JANELA, MS_DESTAQUE,
               ( TEXT *(* )[ ] ) pos_vetcmp(sele), T_MN_CAMPO, str_nada );

   t.escolhido = ult_selec;

   jm = abre_mensagem(R_MN_CAMPO);

   n_campo = tabela( &t );
	fecha_mensagem(jm);

   ult_selec = t.escolhido + 1;

   if ( n_campo  ==  -1 ){
      if ( ptr_cmp != NULL ) *ptr_cmp = NULL;
      p_ret = NULL;
   }
   else {
      if ( ptr_cmp != NULL ) *ptr_cmp = pos_vetcmp(sele)[n_campo];
      strcpy(campo, (TEXT *)pos_vetcmp(sele)[n_campo]);
      prep_palavra(campo);
      p_ret = &campo[0];
   }

fim :

   win_ajuda = ajuda_ant;
   return(p_ret);
}



/*---------------------------------------------------------------------*/
/*   V I S T A S                                                       */
/*---------------------------------------------------------------------*/



F__GLB   TTela *pos_itvis(num)
COUNT num;
{

   if ( ap_vista.vis_vet->vi_vtit  != NULL )
      return((TTela *)ap_vista.vis_vet->vi_vtit[num]);
   else
      return((TTela*)NULL);
}

F__GLB   COUNT qtd_itvis()
{
   if ( ap_vista.vis_vet->vi_vtit  != NULL )
      return(ap_vista.vis_vet->vi_nelem);
   else
      return(0);
}




/*---------------------------------------------------------------------*/
/*   R E L A T O R I O S                                               */
/*---------------------------------------------------------------------*/


F__GLB   RTela *pos_itrel(num)
COUNT num;
{

   if ( ap_rel.rel_vet->vi_vtit  != NULL )
      return((RTela *)ap_rel.rel_vet->vi_vtit[num]);
   else
      return((RTela*)NULL);
}

F__GLB   COUNT qtd_itrel()
{
   if ( ap_rel.rel_vet->vi_vtit  != NULL )
      return(ap_rel.rel_vet->vi_nelem);
   else
      return(0);
}
