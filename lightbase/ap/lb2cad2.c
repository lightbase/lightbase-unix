#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2files.h"
#include  "lb2vet.h"
#include  "lb2prot1.h"

#ifdef   VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#define   GRP_OPCLS  18

#define   H_LDVIS    "ERRLDVIS"   /* erro na carga da descricao das vistas */
/* #define   H_NOVIS    "NOVISDEF"*/   /* Nenhuma vista definida. tem que definir */
#define   H_NOACV    "NOACEVIS"   /* Usuario sem acesso definido as vista */
#define   H_LDITV    "ERRLDITV"   /* Erro na carga dos itens da vista */
#define   H_FCPVI    "ERRCPDES"   /* Campo desconhecido na base informada */
#define   H_BRCLS    "ERRBRCLS"   /* Base relacionada ja deveria estar aberta */

/* #define   E_NOVIS    loc_str(GRP_OPCLS,  10) */
#define   E_NOACV    loc_str(GRP_OPCLS,  11)
#define   E_LDVIS    loc_str(GRP_OPCLS,  12)

#define   E_LDITV    loc_str(GRP_OPCLS,  13)

#define   M_SELVI    loc_str(GRP_OPCLS,  14)
#define   M_MESMA    loc_str(GRP_OPCLS,  15)

#define   T_MUDVI    loc_str(GRP_OPCLS,  16)

#define   E_FCPVI   loc_str(GRP_OPCLS,  503)
#define   E_BRCLS   loc_str(GRP_OPCLS,  504)




#define   CARREGA    'C'
#define   FINALIZA   'F'

/*
#define   DEFINE     'D'
#define   SELECAO    'S'
#define   E_OPV_01     loc_str(VET_ERROS, 158)
#define   E_OPV_02     loc_str(VET_ERROS, 159)
#define   E_OPV_03     loc_str(VET_ERROS, 160)
#define   E_OPV_04     loc_str(VET_ERROS, 161)
*/


#ifdef MSDOS
   COUNT monta_vista( VOID );
   VOID  desm_vista(VOID);
   COUNT cp_entsai(TTela *);
   COUNT acessa_vista(Vista **, COUNT );
#else
   COUNT monta_vista( );
   VOID  desm_vista( );
   COUNT cp_entsai( );
   COUNT acessa_vista( );
#endif


F__GLB   COUNT open_vista(sele, vista, flag)
COUNT sele, vista, flag;
{  COUNT ret, n_vista, i, ind;
   Vet_itens *v;

   v = NULL;
   n_vista = -1; /* vista que sera efetitamente aberta */

   if ( (v = load_vistas(sele, flag)) == NULL ) {
      mens_erro(H_LDVIS, E_LDVIS);
      ret = -1;
      goto fim;
   }

   if ( v->vi_vtit[0] == NULL ) {
      mens_erro(H_NOVIS, E_NOVIS);
      ret = -1;
      goto fim;
   }

   if ( vista == '?' ) {
      if ( ap_status == 'A' ||
           (ap_status == 'B' && strcmp(ap_nome, WBS->bs_dono) == 0) ) {
         ind = 0;
         n_vista = ((Vista *)(v->vi_vtit[ind]))->vis_ordem;
      }
      else {
         n_vista = -1;
         for ( i = 0; n_vista == -1 && i < MAX_VISTAS ; i++) {
            ind = acessa_vista((Vista **)v->vi_vtit, ap_acesso_ativo.da_vistas[i]);
            if ( ind >= 0 ) {
               n_vista = ap_acesso_ativo.da_vistas[i];
               break;
            }
         }
      }
   }
   else {
      n_vista = vista;
      ind = acessa_vista((Vista **)v->vi_vtit, vista);
      if ( ind < 0 )
         n_vista = -1;
   }

   /* verifica se usario tem acesso a vista */
   if ( n_vista < 0 ) {
      mens_erro(H_NOACV, E_NOACV);
      ret = -1;
      goto fim;
   }

   memcpy(&ap_vista, v->vi_vtit[ind], sizeof(Vista));

   rlse_vet(v);
   v = NULL;


   if ( (v = load_itvis(&ap_vista, flag)) == NULL ) {
      mens_erro(H_LDITV, E_LDITV);
      ret = -1;
      goto fim;
   }

   ap_vista.vis_vet = v;


   if ( flag == UTILIZACAO ) {
      if ( monta_vista() < 0 ) {
         desm_vista();
         rlse_vet(ap_vista.vis_vet);
         ap_vista.vis_vet = v = NULL;
         memset((TEXT *)&ap_vista, '\0', sizeof(Vista));
         ret = -1;
         goto fim;
      }
   }

   v = NULL;  /* para nao liberar no final */
   ret = 0;

fim :

   if ( v != NULL ) {
      rlse_vet(v);
   }

   return(ret);
}




F__GLB   VOID close_vista( )
{
   if ( ap_vista.vis_vet != NULL ) {
      desm_vista();
      rlse_vet(ap_vista.vis_vet);
      ap_vista.vis_vet = NULL;
      memset((TEXT *)&ap_vista, '\0', sizeof(Vista));
   }
}



F__GLB   COUNT muda_vista(atual, numero)
COUNT atual, numero;
{	COUNT ret, i, volta, tam, lin, inicia_em;
   TEXT ultimo[TD+1];
   Base      *wbs;
   Vista *pv;
   Vet_itens *v;
	Selecao m0;
	TEXT nx[PATH_MAX+10];
   TEXT  *vet_sel[(MAX_VISTAS + 1) * 3];
   TEXT *ajuda_ant = win_ajuda;
   WIN *jm = NULL, *jf = NULL;

   v = NULL;

   up_str(GRP_OPCLS);

   wbs = v_bases[BASE_EDIT];

   win_ajuda    = "SELVISTA";

   info_base( wbs->bs_nome, wbs->bs_path, LB_4_EXT, nx);

   if ( open_lb4(wbs, nx, UTILIZACAO, ap_nserie, wbs->bs_idbase ) != 0 ) {
      ret = -1;
      goto fim;
   }

   inicia_em = 0;

   if ( (v = load_vistas(BASE_EDIT, UTILIZACAO)) == NULL ) {
      mens_erro(H_LDVIS, E_LDVIS);
      ret = -1;
      goto fim;
   }

   /* para mudar para uma vista especificada por diretivas especiais */
   if ( esp_base.presente ) {
      if ( numero != -1 ) {
         ret = numero;
         goto selecionada;
      }
   }

   for ( i = 0; (vet_sel[OP(i)] = v->vi_vtit[i]) != NULL; i++ ) {
      pv = (Vista *)v->vi_vtit[i];
      if (pv->vis_ordem == atual )
         inicia_em = i;

      if ( acessa_vista((Vista **)v->vi_vtit, pv->vis_ordem) >= 0 )
         vet_sel[AT(i)] = "1";
      else
         vet_sel[AT(i)] = "0";

		vet_sel[AJ(i)] = M_SELVI;
   };


   /* Manter sempre a possibilidade de navegar */
   sprintf(ultimo,"%-35.35s", M_MESMA);

	vet_sel[OP(i)] = ultimo;
   vet_sel[AT(i)] = "1";
	vet_sel[AJ(i)] = M_SELVI;

   i++;

	vet_sel[OP(i)] = NULL;

   tam = MIN ((i+1), (size_y - 7));
   lin = ((size_y - 5) - tam) / 2;
   lin += 2;

	def_menu( &m0, 37, lin, TD,  tam,  1, (TEXT)MS_CAMPO, (TEXT)MS_BARRA,
				   (TEXT)1, (TEXT) MS_JANELA, (TEXT)MS_DESTAQUE,
					&vet_sel, T_MUDVI, str_nada);

   jm = abre_mensagem(str_nada);
	jf = abre_funcoes(funcao("1 3 E"));


   m0.escolhido = inicia_em;

	ret = menu( &m0 );

	fecha_funcoes(jf);
	fecha_mensagem(jm);


selecionada:

   if ( ret < 0 ) {
      volta = atual;
      goto fim;
   }
   else {
      pv = (Vista *) v->vi_vtit[ret];
      if ( pv == NULL )
         volta = atual;
      else
         volta = pv->vis_ordem;
   }

   if ( volta != atual ){
      close_vista();
      if ( open_vista(BASE_EDIT, volta , UTILIZACAO) < 0 ) {
         /* nao foi possivel abrir vista selecionada */
         if ( open_vista(BASE_EDIT, atual, UTILIZACAO) < 0 ) {
            /* nao foi possivel re_abrir vista atual */
            volta = -1;
         }
      }
   }

fim :

   close_lb4();

   if ( v != NULL ) rlse_vet(v);

   win_ajuda = ajuda_ant;

   down_str(GRP_OPCLS);

   return (volta);
}



F__LOC   COUNT monta_vista( )
{  COUNT ret, ind;
   TTela *pe;
   Campo *pc;
   VOID *opcoes;

   ret = 0;

   for (ind = 0; (pe = pos_itvis(ind)) != NULL; ind++ ){

      pe->te_win = NULL;
      pe->te_ivetcp = ind;

      switch ( pe->te_tipo ) {

         case 'B' :
         case 'D' :
         case 'H' :
         case 'L' :
                     break;

         case 'E' :
         case 'S' :  if ( cp_entsai( pe ) < 0 ) {
                        ret = -1;
                        goto fim;
                     }

      };

      if ( pe->te_tipo == 'E' || pe->te_tipo == 'S' ) {

         pc = pos_vetcmp(pe->te_icb)[pe->te_ivetcp];

         /* para contornar situacao de mudanca de tipo e nao mexer na vista */
         if ( pc->cp_tipo == 'F' ) {
            pe->te_tipo = 'S';
            continue;
         }

         if ( pc->cp_critica == 'T' ) {
            COUNT n=0;
            n = atoi(pc->cp_param.tab_auto.tabela);

            if ( n > 0 && n < 99 && strlen(pc->cp_param.tab_auto.tabela) < 3 ) {
               /* tabelas numericas nao sao carregadas. Ficam indexadas */
               pc->cp_2futuro = n;
               pc->cp_pttab = NULL;
            }
            else {
               if ( file_tabela( pc->cp_param.tab_auto.tabela,
                           NULL,0,0,0,0,pc->cp_mascara,0,0,0,0,0,NULL,NULL,0,
                           CARREGA, &opcoes) == NULL ) {
                  mens_erro(H_OPTAB, E_OPTAB, pc->cp_param.tab_auto.tabela);
                  pc->cp_pttab = NULL;
                  ret = -1;
               }
               else
                  pc->cp_pttab = opcoes;
            }

         }
      }

   };

fim :

   return(ret);
}



VOID desm_vista( )
{  COUNT ind;
   TTela *pe;
   Campo *pc;


   for (ind = 0; (pe = pos_itvis(ind)) != NULL; ind++ ){

      if ( pe->te_tipo == 'E' || pe->te_tipo == 'S' ) {

         pc = pos_vetcmp(pe->te_icb)[pe->te_ivetcp];

         if ( pc->cp_tipo == CP_TEXTO && pe->te_win != NULL) {
            fwkill(pe->te_win);
            pe->te_win = NULL;
         }

         if ( pc->cp_critica == 'T' && pc->cp_pttab != NULL ) {
            file_tabela( NULL,NULL,0,0,0,0,NULL,0,0,0,0,0,NULL,NULL,0,
                        FINALIZA, &pc->cp_pttab);
            pc->cp_pttab = NULL;
         }
      }
   }

   if ( ap_visdoc !=NULL ) {
      free_buffer( ap_visdoc );
      ap_visdoc = NULL;
   }
}

COUNT cp_entsai(pe)
TTela *pe;
{  COUNT ret, i, ind;
   COUNT base;
   Campo *pc;

   ret  = 0;
   base = -1;

   if ( pe->te_base[0] == '\0' ) { /* base corrente */
      base = 0;
   }
   else {
      for ( i = 0 ; i < MAX_SELE; i++ ) {
         if ( v_bases[i] != NULL ) {
            if ( strcmp(pe->te_base, v_bases[i]->bs_nome) == 0 ) {
               base = i;
               break;
            }
         }
      }

      if ( base == -1 ) {
         /* base relacionada ja deveria estar aberta */
         mens_erro(H_BRCLS, E_BRCLS);
         ret = -1;
         goto fim;
      };
   }

   pe->te_icb = base;


   ind = acha_campo(base, pe->te_num );

   if ( ind < 0 ) {
      mens_erro(H_FCPVI, E_FCPVI);
      ret = -1;
      goto fim;
   }


   pe->te_ivetcp = ind;
   pe->te_win = NULL;

   /* nao permitir campo obrigatorio que seja de preenchimento automatico */
   pc = pos_vetcmp(pe->te_icb)[pe->te_ivetcp];
   if ( pc->cp_tipo == 'F' || pc->cp_rodape[0] == '@' )
      pe->te_obrigatorio = 'N';

fim :

   return(ret);
}


/*******************************************************************/
F__LOC   COUNT acessa_vista(pvet, num_vista)
Vista **pvet;
COUNT num_vista;
{  COUNT ret, ind, i, existe;

   ind = existe = 0;
   ret = -1;


   while( existe == 0 && pvet[ind] != NULL ) {
      if ( pvet[ind]->vis_ordem == num_vista )
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
         for ( i = 0; i < MAX_VISTAS; i++) {
            if ( ap_acesso_ativo.da_vistas[i] == num_vista ) {
               ret = ind;
               break;
            }
         }
      }
   }

   return(ret);
}


