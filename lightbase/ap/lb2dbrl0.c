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
#include  "menu.h"
#include  "tabela.h"
#else
#include  "../mt/menu.h"
#include  "../mt/tabela.h"
#endif


#define   GRP_DFREL    22

#define   LF123E      "1 2 3 E"
#define   LF1236IDE   "1 2 3 6 IDE"
#define   STR_DUAS    "%s\n%s"
#define   TIPO_REL   ap_rel.rel_tipo



#define   H_NOCMP    "NOCAMPOS"     /* Nao pode definir relatorio sem campo */
#define   H_NOMEBR   "NOMERLBR"     /* Nome da relatorio nao pode estar em branco */
#define   H_CFPROP   "CF_PROP"      /* Confirma proposta padrao */
#define   H_CFF2RL   "CF_F2REL"     /* Confirma gravacao de relatorio */
#define   H_SVITR    "ERF2ITR"      /* Erro na gravacao de itens de relatorio */
#define   H_SVREL    "ERF2REL"      /* Erro na gravacao das relats */
#define   H_RLEXC    "CFEXCREL"     /* Confirma exclusao de relats */
#define   H_RLPEQ    "PROPINC"      /* Nao incluiu todos os campos */



#define   T_RLREL       loc_str(GRP_DFREL, 1)
#define   T_RLROD       loc_str(GRP_DFREL, 2)

#define   M_RLSEL       loc_str(GRP_DFREL, 3)
#define   M_RLINS       loc_str(GRP_DFREL, 4)
#define   M_RLALT       loc_str(GRP_DFREL, 5)

#define   M_ATUREL      loc_str(GRP_DFREL, 6)
#define   M_ATUITR      loc_str(GRP_DFREL, 7)


#define   TW_RLPROP     loc_str(GRP_DFREL, 8)

#define   TW_RLEX9      loc_str(GRP_DFREL, 9)
#define   TW_RLEX1      loc_str(GRP_DFREL, 10)



#define   E_NOCMP       loc_str(GRP_DFREL, 11)
#define   E_NOMEBR      loc_str(GRP_DFREL, 12)
#define   E_SVITR       loc_str(GRP_DFREL, 13)
#define   E_SVREL       loc_str(GRP_DFREL, 14)


#define   M_TR00        loc_str(GRP_DFREL, 47)

#define   M_TR01        loc_str(GRP_DFREL, 48)
#define   M_TR02        loc_str(GRP_DFREL, 49)
#define   M_TR03        loc_str(GRP_DFREL, 50)

#define   R_TR01        loc_str(GRP_DFREL, 51)
#define   R_TR02        loc_str(GRP_DFREL, 52)
#define   R_TR03        loc_str(GRP_DFREL, 53)
#define   TW_RLPEQ1     loc_str(GRP_DFREL, 54)
#define   TW_RLPEQ2     loc_str(GRP_DFREL, 55)




#ifdef MSDOS
   COUNT dsp_relatorio(Tab *);
   COUNT exec_relatorio(Tab *);
   COUNT opr_relatorio( COUNT, TEXT *, COUNT);
   COUNT del_relatorio(COUNT);
   POINTER f2_itrelatorio(COUNT);
   COUNT   f2_relats(VOID);
   VOID prop_rel(Rel *, COUNT, COUNT);
   COUNT tipo_rel(VOID);
   COUNT acha_lin( VOID );
#else
   COUNT dsp_relatorio( );
   COUNT exec_relatorio( );
   COUNT opr_relatorio( );
   COUNT del_relatorio( );
   POINTER f2_itrelatorio( );
   COUNT   f2_relats( );
   VOID  prop_rel( );
   COUNT tipo_rel( );
   COUNT acha_lin( );
#endif



static Vet_itens *vr;
static Rel   **vet_rel;

extern SC *tab_ultimo;

F__GLB   VOID rec_relats(sele)
COUNT sele;
{	Tab   t;
	UTEXT str_escape[20];
   TEXT *ajuda_ant = win_ajuda;
	TEXT  lin_fun[100], lin_alt[100], tec_fun[15];
   WIN   *jm = NULL, *jf = NULL, *ji = NULL;

   vr = NULL;

   up_str(GRP_DFREL);

   win_ajuda = "DEF_REL";

   if ( qtd_vetcmp(sele) <= 0 ) {
      mens_erro(H_NOCMP, E_NOCMP);
      goto fim;
   }

   if ( (vr = load_rels(BASE_EDIT, 'E')) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto fim;
   }

   vet_rel = (Rel**) vr->vi_vtit;  /* aponta para vetor de relats */

	strcpy(tec_fun, LF1236IDE);

	def_tabela(&t, 19, 9, 45, 8, vr->vi_nelem, MAX_REL, x40,
				  MS_CAMPO, MS_BARRA, SIMPLES, MS_JANELA, MS_DESTAQUE,
				  ( char *(* )[]) vet_rel, T_RLREL, str_nada);

	pr_escape(tec_fun, str_escape);

	strncpy(lin_fun, funcao(LF123E), 99);
	strncpy(lin_alt, funcao(LF123E), 99);

   t.mens_ins  = M_RLINS;
   t.mens_alt  = M_RLALT;
	t.fun_ins   = lin_fun;
	t.fun_alt   = lin_alt;
   t.escape    = str_escape;
   t.edita     = INSERE | ALTERA | DELETA ;
   t.comando   = dsp_relatorio;
	t.executa   = exec_relatorio;

   jm = abre_mensagem( M_RLSEL );
	jf = abre_funcoes(funcao(tec_fun));
   ji = abre_insercol(T_RLROD);

   tabela(&t);

	fecha_insercol(ji);
	fecha_funcoes(jf);
	fecha_mensagem(jm);


fim :

   if ( vr != NULL ) {
      rlse_vet(vr);
   }

   win_ajuda = ajuda_ant;

   down_str(GRP_DFREL);
}






F__LOC   COUNT dsp_relatorio(tp)
Tab *tp;
{
   fwsetcursor( tp->win );
   return ( -2 );
}



F__LOC   COUNT exec_relatorio(tp)
Tab *tp;
{  COUNT ret;

	if (tp->tecla == K_F6 && tp->escolhido == (tp->n_elem - 1) &&
                  (*tp->opcoes)[tp->escolhido] == tab_ultimo ) {
		if ( tp->e->str[0] != '\0' ) tp->tecla = K_INS;
		else {  /* digitado campo vazio na ultima ocorencia da tabela */
         win_bell(BEL_INVALID);
			ret = tp->escolhido;           /* permanece no ultimo */
         goto  fim;
		}
	}

   switch (tp->tecla ) {
		case K_DEL  :  /* Exclui relatorio */
							if ( del_relatorio(tp->escolhido) == 0 )
                        ret = -2;
                     else
                        ret = tp->escolhido;

                     break;

      case K_ENTER:  /* Editar relatorio */
		case K_F6   :  /* Alterar o nome da relatorio */
      case K_INS  :  /* Incluir uma nova relatorio */
                     fwbot( tp->win );
							ret = opr_relatorio(tp->escolhido, tp->e->str, tp->tecla);
                     fwtop( tp->win );
							if ( ret == -2 && tp->tecla == K_INS)
                        tp->escolhido++;
                     break;

		case K_F2  :   /* Interceptado  */
                     /*****   seria f2_relats() */
                     ret = -3; /* finaliza edicao */
						   break;

      case K_ESC :   /* Abandonar edicao */
     					   ret = -3;
						   break;

		default      : numdebug(202);
                     break;
   }

fim :


   fwsetcursor( tp->win );
   return(ret);
};


F__LOC   COUNT opr_relatorio(ind_vet, nome_relatorio, tecla)
COUNT ind_vet;
TEXT *nome_relatorio;
COUNT tecla;
{  COUNT ret, num, foi_modificado, tam, max_linhas;
   POINTER pos;
   Vet_itens *rel_ant;

   ret = -1;
   foi_modificado = 0;
   ap_rel.rel_vet = NULL;

   if ( tecla != K_ENTER && nome_relatorio[0] == '\0' ) {
		mens_erro(H_NOMEBR, E_NOMEBR);
      goto fim;
   }

   if ( tecla == K_INS ) {
      memset((TEXT *)&ap_rel, '\0', sizeof(Rel));
      if ( tipo_rel() < 0 )
         goto sai;

      if ( carac_rel( ) < 0 )    /* 0 nao seta estrutura de leitura */
         goto fim;
   }
	else {
      memcpy((TEXT *)&ap_rel, vet_rel[ind_vet], sizeof(Rel));
   }

   if ( tecla == K_F6 && strcmp(ap_rel.rel_ident, nome_relatorio) != 0 )
      foi_modificado = 1;


   if ( tecla != K_ENTER )
      strcpy(ap_rel.rel_ident, nome_relatorio);


   ap_rel.rel_vet = load_itrel(&ap_rel, 'E');

   if ( ap_rel.rel_vet == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }

   if ( (max_linhas = acha_lin()) == 0 )
      max_linhas = ap_rel.rel_n_etq; /* linhas uteis no relatorio */

   if ( qtd_itrel() == 0 && TIPO_REL == 'F') {
      tam = strlen(TW_RLPROP) + 2;
      switch(decida(str_nada, OP_SIM_NAO, H_CFPROP, 1, tam, "%s", TW_RLPROP)){
         case -1 :   /* Esc */
                     ret = ind_vet;
                     goto fim;

         case 0  :   /* Deseja padrao */
                     prop_rel(&ap_rel, MAX_ELEM, ap_rel.rel_colunas);
                     foi_modificado = 1;
                     break;

         case 1 :    /* nao quer padrao */
                     break;
      }
   };

   if ( TIPO_REL != 'C') {
      COUNT nu_lin;
      nu_lin = (TIPO_REL == 'F' ) ? max_linhas : ap_rel.rel_linhas;
      ret = ed_relat( &foi_modificado, nu_lin, ap_rel.rel_colunas );
   }
   else
      ret = make_colunado(&foi_modificado, ap_rel.rel_colunas, 'A');

   if ( ret == 1 ) { /* K_ESC com alteracao */

      if ( decida(TW_GGV0, TW_GGV9, H_CFF2RL, 3, 42, STR_DUAS,
                  TW_GGV1, TW_GGV2) == 0 ) {
         ret = K_F2;
      }
   }

   if ( ret == K_F2 && foi_modificado ) {


      if ( TIPO_REL == 'C')
          prep_f2_colunado( );

      num = vr->vi_ident;
      if ( tecla == K_INS )    /* novo relatorio sendo inserido */
         num++;

      ap_rel.rel_vet->vi_status |= VET_ALT;

      if ( (pos = f2_itrelatorio( num )) == (POINTER) 0 ) {
         mens_erro(H_SVITR, E_SVITR);
         ret = -1;
         goto fim;
      }


      if ( tecla == K_INS ) {    /* nova relatorio sendo inserida */
         vr->vi_ident++;
         ap_rel.rel_ordem = vr->vi_ident;
      }
      else {
         pos_item( vr, ind_vet);
         del_item( vr, 0);
      }

      rel_ant = ap_rel.rel_vet; /* salva anterior para gravar NULL */

      ap_rel.rel_it_ativo   = 0;
      ap_rel.rel_old_ativo  = 0;
      ap_rel.rel_vet        = NULL;
      ap_rel.rel_itpos      = pos;

      if ( add_item(vr, ind_vet, &ap_rel, sizeof(Rel), 0) == NULL ) {
         mens_erro(H_MXITEM, E_MXITEM, ind_vet);
         ret = -1;
         goto fim;
      }

      ap_rel.rel_vet = rel_ant;

      if ( f2_relats( ) != 0 ) {
         mens_erro(H_SVREL, E_SVREL);
         ret = -1;
         goto fim;
      }

      ret = -2;
   }

   else {  /* Nao confirmada gravacao */
      ret = ind_vet;
   }

fim :

   if ( TIPO_REL  == 'C' )
      make_colunado(&foi_modificado, 0, 'F'); /* libera area que pode ter sido alocada */

   if ( ap_rel.rel_vet != NULL )  {
      rlse_vet(ap_rel.rel_vet);
      if ( ap_reldoc !=NULL ) {
         free_buffer( ap_reldoc );
         ap_reldoc = NULL;
      }
   }

sai:

   if ( ret == -1 )
      ret = ind_vet;


   return (ret);
}



/*******************************************************************/

F__LOC COUNT tipo_rel()
{  COUNT ret;
   TEXT *ajuda_ant = win_ajuda;
   Selecao m0;
   TEXT *m_def[(4 * 3) + 1];
   WIN *jf = NULL;


   set_menu(m_def, 0, M_TR01, "1", R_TR01);
   set_menu(m_def, 1, M_TR02, "1", R_TR02);
   set_menu(m_def, 2, M_TR03, "1", R_TR03);
   set_menu(m_def, 3, NULL,   "0", NULL);

   win_ajuda    = "MENUTREL";

	def_menu( &m0, 35, 12, 15,  3,  1 , MS_CAMPO, MS_BARRA, 1,
            MS_JANELA, MS_DESTAQUE, &m_def, M_TR00, str_nada);

   win_ajuda = "MENUTREL";

   jf = abre_funcoes(funcao(LF123E));

   ret = menu( &m0 );          /* Abertura do menu de tipo de relatorio */

   fecha_funcoes(jf);

   switch( ret ) {
      case  0  :  TIPO_REL = 'F'; break;
      case  1  :  TIPO_REL = 'C'; break;
      case  2  :  TIPO_REL = 'E'; break;
      default  :  ret = -1;       break;
   }

   win_ajuda = ajuda_ant;

   return(ret);
}








F__LOC   COUNT del_relatorio(ind_vet)
COUNT ind_vet;
{  COUNT ret = 0, tam;

   tam = strlen(TW_RLEX1) + 2;

   if ( decida(str_nada, TW_RLEX9, H_RLEXC, 1, tam, "%s", TW_RLEX1) == 0 ) {
      RETVREC(lb4_dnum, vet_rel[ind_vet]->rel_itpos);

      pos_item( vr, ind_vet);
      del_item( vr, 0);

      if (  f2_relats() != 0 ) {
         mens_erro(H_SVREL, E_SVREL);
         ret = -1;
      }
   }
   else
      ret = -1;


   return(ret);
}




/*******************************************************************/
F__LOC   COUNT f2_relats( )
{  POINTER 	pos, pos_ant;
	COUNT 	ret, tam;
   WIN *jm = NULL;


   jm = abre_mensagem( M_ATUREL );

   ret = -1;

   tam = vr->vi_mtam + TAMANHO_HDR;
   pos_ant = vr->vi_pos;
   pos = lb4_space(pos_ant, tam);
   if ( pos != (POINTER) 0 ) {
      if ( save_vet(vr, pos) != (POINTER) NULL ) {
         WBS->bs_rlpos = pos;
         WBS->bs_n_rel = vr->vi_nelem;
         if ( lb4_upd( 1 ) == 0 )
            ret = 0;
      }
   }

fim :

   fecha_mensagem(jm);

   return(ret);
}

/*******************************************************************/
F__LOC   POINTER f2_itrelatorio(num_relatorio)
COUNT num_relatorio;
{  POINTER 	pos, pos_ant;
   LONG tam_texto;
	COUNT 	tam;
   WIN *jm = NULL;


   jm = abre_mensagem( M_ATUITR );

   ap_extravet = 0;

   if ( ap_reldoc != NULL ) {
      tam_texto = tam_buffer(ap_reldoc, ET_HANDLE);
      if ( tam_texto > 4 ) { /* minimo valido senao nao tem comando nenhum */
         /* salva em memoria */
         ap_reldoc->tipo    = ET_MEM;
         ap_reldoc->buffer  = buf_iotxt;
         ap_reldoc->buf_len = tam_texto;
         if ( save_file(ap_reldoc) != 0 ) /* igual a zero indica que nao salvou */
            ap_extravet = tam_texto;
      }
   }

   tam = ap_rel.rel_vet->vi_mtam + TAMANHO_HDR + ap_extravet;

   pos_ant = ap_rel.rel_itpos;
   pos = lb4_space(pos_ant, tam);
   if ( pos != (POINTER) 0 ) {

      ap_rel.rel_vet->vi_tipo  = 'L';
      ap_rel.rel_vet->vi_ident = num_relatorio;
      if ( save_vet(ap_rel.rel_vet, pos) == (POINTER) NULL ) {
         pos = 0L;
      }
   }

fim :

   ap_extravet = 0;

   fecha_mensagem(jm);

   return(pos);
}




/*******************************************************************/
F__LOC   VOID prop_rel(pr, lin_mor, col_mor)
Rel *pr;
COUNT lin_mor, col_mor;
{  COUNT item = 0, lin1, lin2, tot_campos;
	COUNT campo, tam, vvert;
	TEXT  *pd;
	RTela *pi;
   Campo **vet_cmp;

   tot_campos = qtd_vetcmp(BASE_EDIT);
   vet_cmp = pos_vetcmp(BASE_EDIT);

   item = 0;
   vvert = 0;

   lin1 = lin2 = vvert = 0;

	for ( campo = 0; campo < tot_campos; campo++ ) {

      lin1 += (vvert + 1);
      lin2  = lin1;
      vvert = 1;

      if ( lin2 > (lin_mor - 1) ) {
         decida( M_ATENCAO, str_vazia, H_RLPEQ,
                  2, 42, STR_DUAS, TW_RLPEQ1, TW_RLPEQ2);
         break; /* sai do for */
      }

      /*
      **
      **  MONTA DISPLAY
      */
      if ( vet_cmp[campo]->cp_tipo != CP_TEXTO ) {
         tam = sizeof(RTela) + strlen(vet_cmp[campo]->cp_nome) + 1;

         pi = (RTela *)add_item(pr->rel_vet, item++, NULL, tam, 1);

         if ( pi == NULL ) {
            numdebug(206);
            return;
         }

         pi->re_tela    = 1;
         pi->re_tipo    = 'D';
		   pi->re_num     = 0;
         pi->re_base[0] = '\0'; /* nome de base NULL significa a corrente */
         pi->re_ivetcp  = -1;
         pi->re_icb     = BASE_EDIT;  /* ANANIAS @@@ */
         pi->re_linha   = lin1;
         pi->re_coluna  = 1;

			pd = (TEXT *) pi + sizeof(RTela); /* inicio da descricao */
         strcpy(pd, vet_cmp[campo]->cp_nome);
      }

      /*
      **
      **  MONTA CAMPO
      */
      if ( vet_cmp[campo]->cp_tipo == CP_TEXTO )
         tam = sizeof(RTela) + strlen(vet_cmp[campo]->cp_desc) + 1;
      else
         tam = sizeof(RTela) + 1;

      pi = (RTela *)add_item(pr->rel_vet, item++, NULL, tam, 1);

      if ( pi == NULL ) {
         numdebug(206);
         return;
      }


      pi->re_tela    = 1;
      pi->re_tipo    = 'S';
		pi->re_num     = vet_cmp[campo]->cp_num;
      pi->re_base[0] = '\0'; /* nome de base NULL significa a corrente */
      pi->re_ivetcp  = -1;
      pi->re_icb     = BASE_EDIT;  /* ANANIAS @@@ */
      pi->re_linha   = lin1;
      pi->re_vver    = vvert;

		pd = (TEXT *) pi + sizeof(RTela); /* inicio da descricao */

      if ( vet_cmp[campo]->cp_tipo == CP_TEXTO ) {
         pi->re_coluna  = 2;
         pi->re_vhor    = MIN(vet_cmp[campo]->cp_mhor, col_mor - 6) + 1;
         *pd = '\0';  /* string vazia para Campo nao texto */
      }
      else {
         pi->re_vhor    = MIN(vet_cmp[campo]->cp_mhor, col_mor - (TE+4));
         pi->re_coluna  = TE + 2;
         *pd = '\0';  /* string vazia para Campo nao texto */
      }
   }
}


F__LOC   COUNT acha_lin( )
{  COUNT num_lin, ret=0;
   TEXT linha[250], *p;

   num_lin = 0;

   while ( ap_reldoc != NULL &&
           lin_texto( ap_reldoc, num_lin++, linha, 0) > 0 ) {
      if ( (p=strchr(linha, ' ')) == NULL )
         p = "";
      else {
         *p = '\0';
         p++;
      }

      /* estave assim ate 10/05/93 if ( stricmp(linha, "LINHAS_DEF") == 0 ) { */
      if ( stricmp(linha, "LINES") == 0 ) {
         ret = atoi(p);
         ret = MIN(ret, MAX_ELEM);
         break;
      }
   }

   return(ret);
}

