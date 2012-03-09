#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2vet.h"
#include  "lb2prot1.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#define   GRP_OPCLS  18


#define   H_OPR01   "E_OPR_01"    /* campo nao existe mais */
#define   H_LDREL   "E_LDRELS"    /* erro na carga dos realtorios. Reprocesse */
#define   H_LDITR   "E_LDITR"     /* erro na carga dos itens do realtorios. Reprocesse */
#define   H_NOREL   "NO_RELS"     /* sem relatorios definidos. defina */


#define   E_OPR01    loc_str(GRP_OPCLS,  512)
#define   E_LDREL    loc_str(GRP_OPCLS,  513)
#define   E_LDITR    loc_str(GRP_OPCLS,  514)


#define   T_MUDRE    loc_str(GRP_OPCLS,  17)
#define   M_CANRE    loc_str(GRP_OPCLS,  18)
#define   M_SELRE    loc_str(GRP_OPCLS,  19)
#define   E_NOREL    loc_str(GRP_OPCLS,  20)






#ifdef MSDOS
   COUNT monta_rel( COUNT );
   VOID  desm_rel( COUNT );
   COUNT acessa_rel(Rel **, COUNT );
   COUNT sele_rel(Vet_itens *);
   COUNT cp_r_entsai(RTela *);
#else
   COUNT monta_rel( );
   VOID  desm_rel( );
   COUNT acessa_rel( );
   COUNT sele_rel( );
   COUNT cp_r_entsai( );
#endif





F__GLB   COUNT open_rel(sele, rel, flag)
COUNT sele, rel, flag;
{  COUNT n_rel, ind_rel;
   Base      *wbs;
	TEXT nx[PATH_MAX+10];
   Vet_itens *r;

   rel = rel; /* so warnings */

   up_str(GRP_OPCLS);

   wbs = v_bases[sele];

   n_rel = -1; /* relatorio que sera efetivamente aberto */

   info_base( wbs->bs_nome, wbs->bs_path, LB_4_EXT, nx);

   if ( open_lb4(wbs, nx, UTILIZACAO, ap_nserie, wbs->bs_idbase ) != 0 ) {
      goto fim;
   }


   if ( (r = load_rels(sele, flag)) == NULL ) {
      mens_erro(H_LDREL, E_LDREL);
      goto fim;
   }

   if ( r->vi_nelem == 0 ) {
      mens_erro(H_NOREL, E_NOREL);
      goto fim;
   }

   if ( (ind_rel = sele_rel(r)) < 0 ){
      goto fim;
   }


   memcpy(&ap_rel, r->vi_vtit[ind_rel], sizeof(Rel));

   rlse_vet(r);
   r = NULL;


   if ( (r = load_itrel(&ap_rel, flag)) == NULL ) {
      mens_erro(H_LDITR, E_LDITR);
      goto fim;
   }

   ap_rel.rel_vet = r;
   ap_rel.rel_n_elem = r->vi_nelem;

   if ( flag == UTILIZACAO ) {
      if ( monta_rel(sele) < 0 ) {
         desm_rel(sele);
         rlse_vet(ap_rel.rel_vet);
         ap_rel.rel_vet = r = NULL;
         goto fim;
      }
   }

   r = NULL;  /* para nao liberar no final */
   n_rel = ap_rel.rel_ordem;


fim :

   close_lb4();

   if ( r != NULL ) rlse_vet(r);

   down_str(GRP_OPCLS);

   return(n_rel);
}




F__GLB   VOID close_rel( )
{
   if ( ap_rel.rel_vet != NULL ) {
      desm_rel(BASE_EDIT);
      rlse_vet(ap_rel.rel_vet);
      memset((TEXT *)&ap_rel, '\0', sizeof(Rel));
   }
}



F__LOC   COUNT monta_rel( sele )
COUNT sele;
{  COUNT ret, ind;
   RTela *pr;

   if ( sele != 0 ) {
      numdebug(8);
      ret = -1;
      goto fim;
   }

   ret = 0;

   ap_rel.rel_1ind = acha_campo(BASE_EDIT, ap_rel.rel_1numcampo);
   ap_rel.rel_2ind = acha_campo(BASE_EDIT, ap_rel.rel_2numcampo);
   ap_rel.rel_3ind = acha_campo(BASE_EDIT, ap_rel.rel_3numcampo);


   for (ind = 0; (pr = pos_itrel(ind)) != NULL; ind++ ){

      pr->re_win = NULL;

      switch ( pr->re_tipo ) {

         case 'B' :
         case 'D' :
         case 'H' :
         case 'L' :
                     break;

         case 'E' :
         case 'S' :  if ( cp_r_entsai( pr ) < 0 ) {
                        ret = -1;
                        goto fim;
                     }

      };
   };

fim :

   return(ret);
}




F__LOC   VOID desm_rel( sele )
COUNT sele;
{
   /* libera bases que porventura tenham sido abertas */
   sele = sele;

   if ( ap_reldoc !=NULL ) {
      free_buffer( ap_reldoc );
      ap_reldoc = NULL;
   }
}




F__LOC   COUNT acessa_rel(pvet, num_rel)
Rel **pvet;
COUNT num_rel;
{  COUNT ret, ind, i, existe;

   ind = existe = 0;
   ret = -1;

   while( existe == 0 && pvet[ind] != NULL ) {
      if ( pvet[ind]->rel_ordem == num_rel )
         existe = 1;
      else {
         ind++;
      }
   }

   if (  existe ) {
      if ( ap_status == 'A' ||
         (ap_status == 'B' && strcmp(ap_nome, WBS->bs_dono) == 0) ) {
         ret = ind;
      }
      else {
         for ( i = 0; i < MAX_REL; i++) {
            if ( ap_acesso_ativo.da_rel[i] == num_rel ) {
               ret = ind;
               break;
            }
         }
      }
   }

   return(ret);
}




F__LOC   COUNT sele_rel(v)
Vet_itens *v;
{	COUNT ret, i, volta, tam, lin;
   TEXT ultimo[TD+1];
   Rel *pr;
	Selecao m0;
   TEXT  *vet_sel[(MAX_REL + 1) * 3];
   TEXT *ajuda_ant = win_ajuda;
   WIN *jm = NULL, *jf = NULL;


   win_ajuda    = "SELRELAT";

   for ( i = 0; (vet_sel[OP(i)] = v->vi_vtit[i]) != NULL; i++ ) {

      pr = (Rel *)v->vi_vtit[i];

      if ( acessa_rel((Rel **)v->vi_vtit, pr->rel_ordem) >= 0 )
         vet_sel[AT(i)] = "1";
      else
         vet_sel[AT(i)] = "0";

		vet_sel[AJ(i)] = M_SELRE;
   };


   /* Manter sempre a possibilidade de navegar */
   sprintf(ultimo, "%-35.35s", M_CANRE);

	vet_sel[OP(i)] = ultimo;
   vet_sel[AT(i)] = "1";
	vet_sel[AJ(i)] = M_SELRE;

   i++;

	vet_sel[OP(i)] = NULL;


   tam = MIN((i+1), (size_y - 7));
   lin = ((size_y - 5) - tam) / 2;
   lin += 2;

	def_menu( &m0, 37, lin, TD,  tam,  1, (TEXT)MS_CAMPO, (TEXT)MS_BARRA,
				   (TEXT)1, (TEXT) MS_JANELA, (TEXT)MS_DESTAQUE,
					&vet_sel, T_MUDRE, str_nada);

   jm = abre_mensagem(str_vazia);
	jf = abre_funcoes(funcao("1 3 E"));


	ret = menu( &m0 );

	fecha_funcoes(jf);
	fecha_mensagem(jm);

   if ( ret < 0 ) {
      volta = -1;
   }
   else {
      pr = (Rel *) v->vi_vtit[ret];
      if ( pr == NULL )
         volta = -1;
      else
         volta = ret;
   }

fim :

   win_ajuda = ajuda_ant;
   return (volta);
}





F__LOC   COUNT cp_r_entsai(pr)
RTela *pr;
{  COUNT ret, ind;
   COUNT base;

   ret  = 0;
   base = -1;

   if ( pr->re_base[0] == '\0' ) { /* base corrente */
      base = 0;
   }
   else {
      /* relacionamento de bases */
      base = 0;  /* sai */
   }

   pr->re_icb = base;

   if ( (ind = acha_campo(base, pr->re_num )) < 0 ) {
      mens_erro(H_OPR01, E_OPR01);
      ret = -1;
      goto fim;
   }

   pr->re_ivetcp = ind;
   pr->re_win = NULL;

fim :

   return(ret);
}

