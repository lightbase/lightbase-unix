#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2bases.h"
#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2vet.h"

#include  "lb2db.h"

#ifdef    VAXVMS
#include  "tabela.h"
#else
#include  "../mt/tabela.h"
#endif

#define   GRP_DFVIS    21

#define   LF123E      "1 2 3 E"
#define   LF1236IDE   "1 2 3 6 IDE"
#define   STR_DUAS    "%s\n%s"


#define   H_NOCMP    "NOCAMPOS"     /* Nao pode definir vista sem campo */
#define   H_NOMEBR   "NOMEVSBR"     /* Nome da vista nao pode estar em branco */
#define   H_CFPROP   "CF_PROP"      /* Confirma proposta padrao */
#define   H_CFF2VS   "CF_F2VIS"     /* Confirma gravacao de vista */
#define   H_SVITV    "ERF2ITV"      /* Erro na gravacao de itens de vista */
#define   H_SVVIS    "ERF2VIS"      /* Erro na gravacao das vistas */
#define   H_VSEXC    "CFEXCVIS"     /* Confirma exclusao de vistas */



#define   T_VSREL       loc_str(GRP_DFVIS, 1)
#define   T_VSROD       loc_str(GRP_DFVIS, 2)

#define   M_VSSEL       loc_str(GRP_DFVIS, 3)
#define   M_VSINS       loc_str(GRP_DFVIS, 4)
#define   M_VSALT       loc_str(GRP_DFVIS, 5)

#define   M_ATUVIS      loc_str(GRP_DFVIS, 6)
#define   M_ATUITV      loc_str(GRP_DFVIS, 7)


#define   TW_VSPROP     loc_str(GRP_DFVIS, 8)

#define   TW_VSEX9      loc_str(GRP_DFVIS, 9)
#define   TW_VSEX1      loc_str(GRP_DFVIS, 10)



#define   E_NOCMP       loc_str(GRP_DFVIS, 11)
#define   E_NOMEBR      loc_str(GRP_DFVIS, 12)
#define   E_SVITV       loc_str(GRP_DFVIS, 13)
#define   E_SVVIS       loc_str(GRP_DFVIS, 14)






#ifdef MSDOS
   COUNT dsp_vista(Tab *);
   COUNT exec_vista(Tab *);
   COUNT opr_vista( COUNT, TEXT *, COUNT);
   COUNT del_vista(COUNT);
   POINTER f2_itvista(COUNT);
   COUNT   f2_vistas(VOID);
   VOID  prop_vista(Vista *);
#else
   COUNT dsp_vista( );
   COUNT exec_vista( );
   COUNT opr_vista( );
   COUNT del_vista( );
   POINTER f2_itvista( );
   COUNT   f2_vistas( );
   VOID  prop_vista( );
#endif



static Vet_itens *vv;
static Vista    **vet_vis;

extern SC *tab_ultimo;



F__GLB   VOID rec_vistas(sele)
COUNT sele;
{	Tab   t;
	UTEXT str_escape[20];
   TEXT *ajuda_ant = win_ajuda;
	TEXT  lin_fun[100], lin_alt[100], tec_fun[15];
   WIN   *jm = NULL, *jf = NULL, *ji = NULL;

   vv = NULL;

   up_str(GRP_DFVIS);

   win_ajuda = "DEF_VIS";

   if ( qtd_vetcmp(sele) <= 0 ) {
      mens_erro(H_NOCMP, E_NOCMP);
      goto fim;
   }

   if ( (vv = load_vistas(BASE_EDIT, 'E')) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto fim;
   }

   vet_vis = (Vista **) vv->vi_vtit;  /* aponta para vetor de vistas */

	strcpy(tec_fun, LF1236IDE);

	def_tabela(&t, 19, 9, 45, 8, vv->vi_nelem, MAX_VISTAS, x40,
				  MS_CAMPO, MS_BARRA, SIMPLES, MS_JANELA, MS_DESTAQUE,
				  ( char *(* )[]) vet_vis, T_VSREL, str_nada);

	pr_escape(tec_fun, str_escape);

	strncpy(lin_fun, funcao(LF123E), 99);
	strncpy(lin_alt, funcao(LF123E), 99);

   t.mens_ins  = M_VSINS;
   t.mens_alt  = M_VSALT;
	t.fun_ins   = lin_fun;
	t.fun_alt   = lin_alt;
   t.escape    = str_escape;
   t.edita     = INSERE | ALTERA | DELETA ;
   t.comando   = dsp_vista;
	t.executa   = exec_vista;

   jm = abre_mensagem( M_VSSEL );
	jf = abre_funcoes(funcao(tec_fun));
   ji = abre_insercol(T_VSROD);

   tabela(&t);

	fecha_insercol(ji);
	fecha_funcoes(jf);
	fecha_mensagem(jm);


fim :

   if ( vv != NULL ) {
      rlse_vet(vv);
   }

   win_ajuda = ajuda_ant;

   down_str(GRP_DFVIS);
}






F__LOC   COUNT dsp_vista(tp)
Tab *tp;
{
   if ( w_kbhit(tp->win) ) goto fim;

fim:

   fwsetcursor( tp->win );
   return ( -2 );
}



F__LOC   COUNT exec_vista(tp)
Tab *tp;
{  COUNT ret;


	if (tp->tecla == K_F6 && tp->escolhido == (tp->n_elem - 1) &&
                  (*tp->opcoes)[tp->escolhido] == tab_ultimo ) {
		if ( tp->e->str[0] != '\0' )    tp->tecla = K_INS;
		else {  /* digitado campo vazio na ultima ocorencia da tabela */
         win_bell(BEL_INVALID);
			ret = tp->escolhido;           /* permanece no ultimo */
         goto  fim;
		}
	}

   switch (tp->tecla ) {
		case K_DEL  :  /* Exclui vista */
							if ( del_vista(tp->escolhido) == 0 )
                        ret = -2;
                     else
                        ret = tp->escolhido;

                     break;

      case K_ENTER:  /* Editar vista */
		case K_F6   :  /* Alterar o nome da vista */
      case K_INS  :  /* Incluir uma nova vista */
							ret = opr_vista(tp->escolhido, tp->e->str, tp->tecla);
							if ( ret == -2 && tp->tecla == K_INS)
                        tp->escolhido++;
                     break;

		case K_F2  :   /* Interceptado  */
                     /*****   seria f2_vistas() */
                     ret = -3; /* finaliza edicao */
						   break;

      case K_ESC :   /* Abandonar edicao */
     					   ret = -3;
						   break;

		default      : numdebug(202);
                     break;
   }

fim :

   fwtop( tp->win );
   fwsetcursor( tp->win );

   return(ret);
};


F__LOC   COUNT opr_vista(ind_vet, nome_vista, tecla)
COUNT ind_vet;
TEXT *nome_vista;
COUNT tecla;
{  COUNT ret, num, foi_modificado;
   POINTER pos;
   Vet_itens *vis_ant;

   foi_modificado = 0;
   ap_vista.vis_vet = NULL;

   if ( tecla != K_ENTER && nome_vista[0] == '\0' ) {
		mens_erro(H_NOMEBR, E_NOMEBR);
		ret = -1;
      goto fim;
   }

   if ( tecla == K_INS )
      memset((TEXT *)&ap_vista, '\0', sizeof(Vista));
	else
      memcpy((TEXT *)&ap_vista, vet_vis[ind_vet], sizeof(Vista));

   if ( tecla == K_F6 && strcmp(ap_vista.vis_ident, nome_vista) != 0 )
      foi_modificado = 1;


   if ( tecla != K_ENTER )
      strcpy(ap_vista.vis_ident, nome_vista);

   ap_vista.vis_vet = load_itvis(&ap_vista, 'E');

   if ( ap_vista.vis_vet == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   if ( qtd_itvis() == 0 ) {
      switch(decida(str_nada, OP_SIM_NAO, H_CFPROP, 1, 34, "%s", TW_VSPROP)){
         case -1 :   /* Esc */
                     ret = ind_vet;
                     goto fim;

         case 0  :   /* Deseja padrao */
                     prop_vista(&ap_vista);
                     foi_modificado = 1;
                     break;

         case 1 :    /* nao quer padrao */
                     break;

      }

   };

   ret = ed_tela( &foi_modificado );

   if ( ret == 1 ) { /* K_ESC com alteracao */

      if ( decida(TW_GGV0, TW_GGV9, H_CFF2VS, 3, 42, STR_DUAS,
                  TW_GGV1, TW_GGV2) == 0 ) {
         ret = K_F2;
      }
   }

   if ( ret == K_F2 && foi_modificado ) {

      num = vv->vi_ident;
      if ( tecla == K_INS )    /* nova vista sendo inserida */
         num++;

      if ( foi_modificado ) {
         ap_vista.vis_vet->vi_status |= VET_ALT;
      }


      if ( (pos = f2_itvista( num )) == (POINTER) 0 ) {
         mens_erro(H_SVITV, E_SVITV);
         ret = -1;
         goto fim;
      }


      if ( tecla == K_INS ) {    /* nova vista sendo inserida */
         vv->vi_ident++;
         ap_vista.vis_ordem = vv->vi_ident;
      }
      else {
         pos_item( vv, ind_vet);
         del_item( vv, 0);
      }

      vis_ant = ap_vista.vis_vet; /* salva anterior para gravar NULL */

      ap_vista.vis_it_ativo   = 0;
      ap_vista.vis_old_ativo  = 0;
      ap_vista.vis_vet        = NULL;
      ap_vista.vis_itpos      = pos;

      if ( add_item(vv, ind_vet, &ap_vista, sizeof(Vista), 0) == NULL ) {
         mens_erro(H_MXITEM, E_MXITEM, ind_vet);
         ret = -1;
         goto fim;
      }

      ap_vista.vis_vet = vis_ant;

      if ( f2_vistas( ) != 0 ) {
         mens_erro(H_SVVIS, E_SVVIS);
         ret = -1;
         goto fim;
      }

      ret = -2;
   }

   else {  /* Nao confirmada gravacao */
      ret = ind_vet;
   }



fim :

   if ( ap_vista.vis_vet != NULL ) {
      rlse_vet(ap_vista.vis_vet);
      if ( ap_visdoc !=NULL ) {
         free_buffer( ap_visdoc );
         ap_visdoc = NULL;
      }
   }

   if ( ret == -1 )
      ret = ind_vet;

   return (ret);
}


F__LOC   COUNT del_vista(ind_vet)
COUNT ind_vet;
{  COUNT ret = 0;

   if ( decida(str_nada, TW_VSEX9, H_VSEXC, 1, 32, "%s", TW_VSEX1) == 0 ) {
      RETVREC(lb4_dnum, vet_vis[ind_vet]->vis_itpos);

      pos_item( vv, ind_vet);
      del_item( vv, 0);

      if (  f2_vistas() != 0 ) {
         mens_erro(H_SVVIS, E_SVVIS);
         ret = -1;
      }
   }
   else
      ret = -1;

   return(ret);
}




/*******************************************************************/
F__LOC   COUNT f2_vistas( )
{  POINTER 	pos, pos_ant;
	COUNT 	ret, tam;
   WIN *jm = NULL;


   jm = abre_mensagem( M_ATUVIS );

   ret = -1;

   tam = vv->vi_mtam + TAMANHO_HDR;
   pos_ant = vv->vi_pos;
   pos = lb4_space(pos_ant, tam);
   if ( pos != (POINTER) 0 ) {
      if ( save_vet(vv, pos) != (POINTER) NULL ) {
         WBS->bs_vspos = pos;
         WBS->bs_n_vis = vv->vi_nelem;
         if ( lb4_upd( 1 ) == 0 )
            ret = 0;
      }
   }

fim :

   fecha_mensagem(jm);

   return(ret);
}

/*******************************************************************/
F__LOC   POINTER f2_itvista(num_vista)
COUNT num_vista;
{  POINTER 	pos, pos_ant;
   LONG tam_texto;
	COUNT 	tam;
   WIN *jm = NULL;


   jm = abre_mensagem( M_ATUITV );

   ap_extravet = 0;

   if ( ap_visdoc != NULL ) {
      tam_texto = tam_buffer(ap_visdoc, ET_HANDLE);
      if ( tam_texto > 4 ) { /* minimo valido senao nao tem comando nenhum */
         /* salva em memoria */
         ap_visdoc->tipo    = ET_MEM;
         ap_visdoc->buffer  = buf_iotxt;
         ap_visdoc->buf_len = tam_texto;
         if ( save_file(ap_visdoc) != 0 ) /* igual a zero indica que nao salvou */
            ap_extravet = tam_texto;
      }
   }

   tam = ap_vista.vis_vet->vi_mtam + TAMANHO_HDR + ap_extravet;

   pos_ant = ap_vista.vis_itpos;
   pos = lb4_space(pos_ant, tam);
   if ( pos != (POINTER) 0 ) {

      ap_vista.vis_vet->vi_tipo  = 'I';
      ap_vista.vis_vet->vi_ident = num_vista;
      if ( save_vet(ap_vista.vis_vet, pos) == (POINTER) NULL ) {
         pos = 0L;
      }
   }

fim :

   ap_extravet = 0;

   fecha_mensagem(jm);

   return(pos);
}




/*******************************************************************/
F__LOC   VOID prop_vista(pv)
Vista *pv;
{  COUNT item = 0, lin1, lin2, tot_campos;
	COUNT campo, pagina, nova_tela, tam, vvert;
	TEXT  *pd;
	TTela *pi;
   Campo **vet_cmp;

   tot_campos = qtd_vetcmp(BASE_EDIT);
   vet_cmp = pos_vetcmp(BASE_EDIT);

   item = 0;
   pagina = 0;
   vvert = 0;
   nova_tela = 1;


	for ( campo = 0; campo < tot_campos; campo++ ) {

nova :

      if ( nova_tela ) {
         nova_tela = 0;
         lin1 = lin2 = vvert = 0;
         pagina++;
      }

      lin1 += (vvert + 1);

      if ( vet_cmp[campo]->cp_tipo == CP_TEXTO ) {
         lin1++;
         lin2  = lin1 + 6;
         vvert = 5;
      }
      else {
         lin2  = lin1;
         vvert = 1;
      }



      if ( lin2 > (size_y - 5) ) {
         nova_tela = 1;
         goto nova;
      }

      /*
      **
      **  MONTA DISPLAY
      */
      if ( vet_cmp[campo]->cp_tipo != CP_TEXTO ) {
         tam = sizeof(TTela) + strlen(vet_cmp[campo]->cp_nome) + 1;

         pi = (TTela *)add_item(pv->vis_vet, item++, NULL, tam, 1);

         if ( pi == NULL )
            numdebug(206);

			pd = (TEXT *) pi + sizeof(TTela); /* inicio da descricao */

         pi->te_tela    = pagina;
         pi->te_tipo    = 'D';
		   pi->te_num     = 0;
         pi->te_base[0] = '\0'; /* nome de base NULL significa a corrente */
         pi->te_ivetcp      = -1;
         pi->te_icb         = BASE_EDIT;  /* ANANIAS @@@ */

         pi->te_linha   = lin1;
         pi->te_coluna  = 1;

         strcpy(pd, vet_cmp[campo]->cp_nome);
      }

      /*
      **
      **  MONTA CAMPO
      */
      if ( vet_cmp[campo]->cp_tipo == CP_TEXTO ) {
         tam = sizeof(TTela) + strlen(vet_cmp[campo]->cp_desc) + 1;
      }
      else
         tam = sizeof(TTela) + 1;

      pi = (TTela *)add_item(pv->vis_vet, item++, NULL, tam, 1);

      if ( pi == NULL )
         numdebug(206);

		pd = (TEXT *) pi + sizeof(TTela); /* inicio da descricao */

      pi->te_tela    = pagina;

      if ( vet_cmp[campo]->cp_tipo == 'F' )
         pi->te_tipo    = 'S';
      else
         pi->te_tipo    = 'E';

		pi->te_num     = vet_cmp[campo]->cp_num;
      pi->te_base[0] = '\0'; /* nome de base NULL significa a corrente */
      pi->te_ivetcp      = -1;
      pi->te_icb         = BASE_EDIT;  /* ANANIAS @@@ */

      pi->te_linha   = lin1;
      pi->te_vver    = vvert;

      /** Valores padroes   **/
      pi->te_autoclear   = 'S';
      pi->te_append      = 'N';

      if ( vet_cmp[campo]->cp_critica     == 'T' ||
           vet_cmp[campo]->cp_chave_unica == 'S' )
         pi->te_obrigatorio = 'S';
      else
         pi->te_obrigatorio = 'N';

      if ( vet_cmp[campo]->cp_tipo == CP_TEXTO ) {
         pi->te_coluna  = 2;
         pi->te_vhor    = MIN(vet_cmp[campo]->cp_mhor, size_x - 6) + 1;

         strcpy(pd, vet_cmp[campo]->cp_desc);
         lin1++;
      }
      else {
         pi->te_vhor    = MIN(vet_cmp[campo]->cp_mhor, size_x - (TE+4));
         pi->te_coluna  = TE + 2;
         *pd = '\0';  /* string vazia para Campo nao texto */
      }
   }
}

