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

#ifdef   VAXVMS
#include  "tabela.h"
#include  "menu.h"
#else
#include  "../mt/tabela.h"
#include  "../mt/menu.h"
#endif


/*---------------------------------------------------------------------*/
/*  Edicao de PROTECAO                                                 */
/*---------------------------------------------------------------------*/
typedef struct { /* estrutura permanente para edicao de permissoes */
	TEXT  ident[TE+1];
	TEXT  perfil[2];
   TEXT  campo[2];
   TEXT  vista[2];
	TEXT  acesso[2];
	TEXT  relat[2];
	TEXT  stop[2];
	TEXT  inclui[2];
	TEXT  altera[2];
	TEXT  exclui[2];
	TEXT  pesq[2];
	TEXT  emite[2];
	TEXT  exporta[2];
	TEXT  importa[2];
	TEXT  indexa[2];
	TEXT  total[2];
	TEXT  backup[2];
	TEXT  restore[2];
	TEXT  edt_vis[TE+1];
	TEXT  edt_rel[TE+1];
	COUNT vistas[MAX_VISTAS + 1];     /* vetor de acesso a vistas */
	COUNT relats[MAX_REL + 1];        /* vetor de acesso a relatorios */
} Pw;

static COUNT stat_loc;
static Pw *pw;

extern SC *tab_ultimo;

#define   LIN_V   11
#define   LIN_R   12
#define   POS_BORDA    9,  0, 14, 54


#define   H_ACINC    "AC_INC"      /*   */
#define   H_ACALT    "AC_ALT"      /*   */
#define   H_ACEXC    "AC_EXC"      /*   */
#define   H_ACPESQ   "AC_PESQ"     /*   */
#define   H_ACEMIT   "AC_EMIT"     /*   */
#define   H_ACEXPO   "AC_EXPO"     /*   */
#define   H_ACIMPO   "AC_IMPO"     /*   */

#define   H_ACINDP   "AC_INDP"     /*   */
#define   H_ACINDT   "AC_INDT"     /*   */
#define   H_ACBACK   "AC_BACK"     /*   */
#define   H_ACREST   "AC_REST"     /*   */

#define   H_ACVIST   "AC_VIST"     /*   */
#define   H_ACRELS   "AC_RELS"     /*   */




#define   H_CFF2PRT   "CF_F2PRT"  /* adandonar def de protecao */
#define   H_IDINV     "NOMEINV"   /* nome invalido para acessos */
#define   H_DB3DUP    "E_DB3_DUP" /* ja existe identificacao com mesmo nome */
#define   H_DB3NO     "E_DB3_NO"  /* identificacao na existe */
#define   H_ACADM     "E_DB3_ADM" /* usuario 'A' pode tudo */
#define   H_CFEXPR    "CF_EXXPR"  /* confirma excesso de exclusao de acessos */
#define   H_CFEXCPR   "CF_EXCPR"  /* COnfirma exclusao de acessos */
#define   H_DB3EXC    "E_DB3_EXC" /* Erro na exclusao de protecao */
#define   H_DB3INC    "E_DB3_INC" /* Erro na inclusao de protecao */


#define   GRP_DFACE    33



#define   C_DB3_ICOL      loc_str(GRP_DFACE, 1 )

#define   M_AC_INCLUI     loc_str(GRP_DFACE, 2 )
#define   M_AC_ALTERA     loc_str(GRP_DFACE, 3 )
#define   M_AC_EXCLUI     loc_str(GRP_DFACE, 4 )
#define   M_AC_PESQ       loc_str(GRP_DFACE, 5 )
#define   M_AC_EMITIR     loc_str(GRP_DFACE, 6 )
#define   M_AC_EXPORT     loc_str(GRP_DFACE, 7 )
#define   M_AC_IMPORT     loc_str(GRP_DFACE, 8 )

#define   M_AC_PARCIAL    loc_str(GRP_DFACE, 9 )
#define   M_AC_TOTAL      loc_str(GRP_DFACE, 10)
#define   M_AC_BACKUP     loc_str(GRP_DFACE, 11)
#define   M_AC_RESTORE    loc_str(GRP_DFACE, 12)

#define   M_AC_RVISTA     loc_str(GRP_DFACE, 13)
#define   M_AC_RREL       loc_str(GRP_DFACE, 14)

#define   M_INS_PRT       loc_str(GRP_DFACE, 15)
#define   M_EXC_PRT       loc_str(GRP_DFACE, 16)
#define   M_SEL_PRT       loc_str(GRP_DFACE, 17)

#define   M_DB3_INVERTE   loc_str(GRP_DFACE, 18)
#define   M_DB3_WAIT      loc_str(GRP_DFACE, 19)


/* #define   C_DB3_DEF     loc_str(GRP_DFACE, 20) */
#define   C_DB3_UTI     loc_str(GRP_DFACE, 21)
#define   C_DB3_MAN     loc_str(GRP_DFACE, 22)
#define   C_DB3_VR      loc_str(GRP_DFACE, 23)

#define   C_DB3_PINC    loc_str(GRP_DFACE, 24)
#define   C_DB3_PALT    loc_str(GRP_DFACE, 25)
#define   C_DB3_PEXC    loc_str(GRP_DFACE, 26)
#define   C_DB3_PPSQ    loc_str(GRP_DFACE, 27)
#define   C_DB3_PEMI    loc_str(GRP_DFACE, 28)
#define   C_DB3_PEXP    loc_str(GRP_DFACE, 29)
#define   C_DB3_PIMP    loc_str(GRP_DFACE, 30)

#define   C_DB3_PPAR    loc_str(GRP_DFACE, 31)
#define   C_DB3_PTOT    loc_str(GRP_DFACE, 32)
#define   C_DB3_PBAK    loc_str(GRP_DFACE, 33)
#define   C_DB3_PRES    loc_str(GRP_DFACE, 34)

#define   C_DB3_MVIS    loc_str(GRP_DFACE, 35)
#define   C_DB3_MREL    loc_str(GRP_DFACE, 36)
#define   C_DB3_IDENT   loc_str(GRP_DFACE, 37)



#define   E_DB3_CMP  loc_str(GRP_DFACE, 50)
#define   E_DB3_ADM  loc_str(GRP_DFACE, 51)
#define   E_DB3_DUP  loc_str(GRP_DFACE, 52)
#define   E_DB3_NO   loc_str(GRP_DFACE, 53)
#define   E_DB3_INC 	loc_str(GRP_DFACE, 54)
#define   E_DB3_EXC 	loc_str(GRP_DFACE, 55)


#define   E_EXC_EXC 	loc_str(GRP_DFACE, 501)



static WIN   *j2;
static Acesso **vtprot = NULL;
static Acesso *vet_exc[MAX_EXC];

static COUNT ind_exc;  /* indice do vetor de protecoes excluidas */
static COUNT num_prot;
static COUNT f2_ativa;
static COUNT esc_ativa;
static DTela *tep;

extern COUNT ct_semseguranca;

#ifdef MSDOS
   COUNT def_prot(Tab *);
   COUNT dsp_prot(Tab *);
   COUNT inc_prot( COUNT, TEXT *);
   COUNT edt_prot( COUNT );
   COUNT exc_prot( COUNT );

   COUNT tr_visrel(d_entry *);
   COUNT muda_acesso(Selecao *);

   COUNT prt_load(COUNT, TEXT *);
   COUNT prt_save(Acesso **, COUNT *);
   COUNT if_prt_mod( COUNT );

   VOID  load_da(Acesso *);
   VOID  seta_da(Acesso *);
   VOID  zera_pw(VOID);

   COUNT vl_pode(d_entry *);
   COUNT visrel_ok( COUNT, COUNT );
   VOID  monta_tela( VOID );
   VOID seta_tep(VOID);
#else
   COUNT def_prot( );
   COUNT dsp_prot( );
   COUNT inc_prot( );
   COUNT edt_prot( );
   COUNT exc_prot( );

   COUNT tr_visrel( );
   COUNT muda_acesso( );

   COUNT prt_load( );
   COUNT prt_save( );
   COUNT if_prt_mod( );

   VOID  load_da( );
   VOID  seta_da( );
   VOID  zera_pw( );

   COUNT vl_pode( );
   COUNT visrel_ok( );
   VOID  monta_tela( );
   VOID seta_tep( );
#endif





F__GLB   VOID rec_prot(sele)
COUNT sele;
{	COUNT  i;
	Tab   t;
	UTEXT str_escape[20];
   TEXT *ajuda_ant = win_ajuda;
	TEXT  lin_fun[100], lin_alt[100], tec_fun[15];
   WIN   *jm = NULL, *jf = NULL, *ji = NULL;

   sele = sele;

   up_str(GRP_DFACE);

   win_ajuda = "DEF_PROT";
   vtprot = NULL;
   tep = NULL;
   pw = NULL;

	j2 = abre_janela(J_ACESWIN);

   tep    = calloc(15, sizeof(DTela));
   pw     = calloc(1, sizeof(Pw));
	vtprot = (Acesso **)calloc(MAX_PROTECOES + 1, sizeof(Acesso *));

   if ( pw == NULL || tep == NULL || vtprot == NULL) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto fim;
   }
   else
      seta_tep();

	ind_exc = 0;    /* inicializa indice do vetor de excluidos */


	if ( prt_load( MAX_PROTECOES, WBS->bs_nome ) != OK )
		goto fim;

	strcpy(tec_fun, "1 2 3 I D E");

	def_tabela(&t, 3, 8, 25, 13, num_prot, MAX_PROTECOES, X14,
				  MS_CAMPO, MS_BARRA, SIMPLES, MS_JANELA, MS_DESTAQUE,
				  ( char *(* )[]) vtprot, C_DB3_IDENT, str_nada);

	pr_escape(tec_fun, str_escape);

	strncpy(lin_fun, funcao("1 2 3 E"), 99);
	strncpy(lin_alt, funcao("1 2 3 E"), 99);

   t.mens_ins  = M_INS_PRT;
	t.fun_ins   = lin_fun;
	t.fun_alt   = lin_alt;
   t.escape    = str_escape;
	t.comando   = dsp_prot;
	t.executa   = def_prot;
   t.edita     = INSERE | DELETA ;

   monta_tela( );  /* monta tela e seta mensagens de ed_read para tep[] */

   jm = abre_mensagem( M_SEL_PRT );
	jf = abre_funcoes(funcao(tec_fun));
   ji = abre_insercol(C_DB3_ICOL);

   tabela(&t);

	fecha_insercol(ji);
	fecha_funcoes(jf);
	fecha_mensagem(jm);


fim :

   if ( vtprot != NULL ) {
		for (i = 0 ; i < MAX_PROTECOES; i++ )
			if (vtprot[i] != NULL) { free((TEXT *)vtprot[i]); vtprot[i] = NULL; };
      free((TEXT *)vtprot);
      vtprot = NULL;
   }

   if ( pw != NULL ) free(pw);
   if ( tep != NULL ) free(tep);

	if ( j2 != NULL ) fwkill(j2);

   win_ajuda = ajuda_ant;
   down_str(GRP_DFACE);
}





F__LOC   COUNT def_prot(tp)
Tab *tp;
{
	COUNT ret;

   fwtop( j2 );

   f2_ativa = 0;
   esc_ativa = 0;

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
		case K_DEL  :  ret = exc_prot(tp->escolhido);
                     break;

      case K_INS  :  /* Incluir uma nova protecao */
							ret = inc_prot(tp->escolhido, tp->e->str);
							if ( ret == -2 ) tp->escolhido++;
                     break;

		case K_F2  :   /* Gravar registro (atualizar base) */

                     prt_save(vet_exc, &ind_exc);

                     ret = -3; /* finaliza edicao */

						   break;

      case K_ESC :   /* Abandonar edicao */
						   if ( if_prt_mod( ind_exc ) != 0 ) {
                        ret = decida(TW_GGV0, TW_GGV9, H_CFF2PRT,
                              3, 42, "%s\n%s", TW_GGV1, TW_GGV2 );
                        if ( ret == 0 ) {
         	               prt_save(vet_exc, &ind_exc);
                           ret = -3;
                        }
                        else if ( ret == 1 ) ret = -3;
                        else ret = tp->escolhido;
                     }
     					   else ret = -3;

						   break;

      case K_ENTER :
		default      : /* Editar caracteristicas da protecao */
							ret = edt_prot(tp->escolhido);
							if (ret == -2 ) tp->escolhido++; /* ANANIAS @ ??? */
                     break;
   }

   if ( f2_ativa ) {
      prt_save(vet_exc, &ind_exc);
      ret = -3; /* finaliza edicao */
   }

fim :

   fwtop( tp->win );
   fwsetcursor( tp->win );

   return(ret);
}


F__LOC   COUNT dsp_prot(tp)
Tab *tp;
{

   if ( w_kbhit(j2) ) goto fim;

	if ( tp->escolhido != (tp->n_elem - 1)) {

		load_da(vtprot[tp->escolhido]); 	/* atribui valores de pw para area alocada */

		ed_read(j2, &tep[0], 0, SO_MOSTRA, str_nada);
	}
	else {
		memset((TEXT *) pw, '\0', sizeof(Pw));
		ed_read(j2, &tep[0], 0, SO_LIMPA, str_nada);
	}


fim:

   fwsetcursor( tp->win );
   return ( -2 );
}


F__LOC    COUNT inc_prot(ind_vet, identific)
COUNT ind_vet;
TEXT *identific;
{  COUNT i, ret, campo, max;
   Usuario usuario;
   Grupo grupo;
   TEXT *ptr_original;
   TEXT aux1[30], aux2[30];


   stat_loc = 0;
   ptr_original = (TEXT *) vtprot[ind_vet];

	if ( val_indice(identific) != OK) {
		mens_erro(H_IDINV, E_NOMEIN);
		ret = ind_vet;
      goto fim;
   }

   /* verificar se nome proposto ja existe */

   strcpy(aux1, identific);
   prep_palavra(aux1);


   max = (ind_vet < num_prot) ? num_prot + 1 : num_prot;

   for ( i = 0; i < max; i++) {

      if ( i == ind_vet ) continue;

      strcpy(aux2, vtprot[i]->da_ugnome);
      prep_palavra(aux2);

		if ( strcmp(aux1, aux2) == 0 ) {
         /* indica que achou um identificacao com o mesmo nome. Nao pode */
         mens_erro(H_DB3DUP, E_DB3_DUP);
         ret = ind_vet;
         goto fim;
      }
   }

	/* verifica se identificacao e valida */

   if ( load_rec(ace_dnum, ace_inum, US_ID, identific, &usuario, sizeof(Usuario), '=') != NULL) {
      stat_loc = usuario.us_status[0];
   }
   else {
      if ( load_rec(ace_dnum, ace_inum, GR_ID, identific, &grupo, sizeof(Grupo), '=') != NULL ) {
         stat_loc = 'C'; /*** grupo.gr_status[0]; ****/
      }
   }


	if ( stat_loc == 0 ) {
      mens_erro(H_DB3NO, E_DB3_NO);
      ret = ind_vet;
      goto fim;
   }

	if ( stat_loc == 'A' ) {
      mens_erro(H_ACADM, E_DB3_ADM);
      ret = ind_vet;
      goto fim;
   }

	vtprot[ind_vet] = (Acesso *)calloc( 1, sizeof(Acesso));

	if ( vtprot[ind_vet] == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = ind_vet;
      goto fim;
   }

   zera_pw();            /* inicializa valores default do campo */

   campo = 1;

le :

	ret = ed_read(j2, &tep[0], campo, (campo == 99) ? MOSTRA_LE : LIMPA_LE, "1 2 3 E");

   if ( esc_ativa == 1 ) ret = K_ESC;

	switch (ret) {
      case K_ESC  : ret = -1;   goto fim;
		case K_UP   :
		case K_PGUP : campo =  1; goto le;
		case K_DN   :
		case K_PGDN : campo = 99; goto le;
		case K_F2   : f2_ativa = 1;
		case K_ENTER:
      default     : ret = -2;   break;
	}


	seta_da(vtprot[ind_vet]);

	strcpy(vtprot[ind_vet]->da_ugnome, identific);

	vtprot[ind_vet]->da_mod_flag  = 1;  /* incluido */

   num_prot++;

   ret = -2;

fim :

   if ( ret == -1 ) {
      if (vtprot[ind_vet] != NULL ) {
         free((TEXT *)vtprot[ind_vet]);
         vtprot[ind_vet] = NULL;
      };
      vtprot[ind_vet] = (Acesso *) ptr_original;
      ret = ind_vet; /* nao coonfirma */
   }

   return (ret);
}



F__LOC   COUNT edt_prot(ind_vet)
COUNT ind_vet;
{  COUNT ret, campo;
   TEXT identific[TE+1];
   Usuario usuario;
   Grupo grupo;

   strcpy(identific, vtprot[ind_vet]->da_ugnome);

   stat_loc = 0;
	/* verifica se identificacao e valida */

   if ( load_rec(ace_dnum, ace_inum, US_ID, identific, &usuario, sizeof(Usuario), '=') != NULL)
      stat_loc = usuario.us_status[0];
   else
      if ( load_rec(ace_dnum, ace_inum, GR_ID, identific, &grupo, sizeof(Grupo), '=') != NULL )
         stat_loc = 'C'; /*** grupo.gr_status[0]; ****/


	if ( stat_loc == 0 ) {
      mens_erro(H_DB3NO, E_DB3_NO);
      ret = ind_vet;
      goto fim;
   }

	load_da(vtprot[ind_vet]); 	/* atribui valores de pw para area alocada */

   campo = 1;

le:

	ret = ed_read(j2, &tep[0], campo, MOSTRA_LE, "1 2 3 E");

   if ( esc_ativa == 1 ) ret = K_ESC;

	switch (ret) {
      case K_ESC  : ret = -1;   goto fim;
		case K_UP   :
		case K_PGUP : campo =  1; goto le;
		case K_DN   :
		case K_PGDN : campo = 99; goto le;
		case K_F2   : f2_ativa = 1;
		case K_ENTER:
      default     : ret = -2;   break;
	}


	seta_da(vtprot[ind_vet]);


   /* se NAO for campo recem incluido, marca como alterado */
	if ( vtprot[ind_vet]->da_mod_flag != 1 )
      vtprot[ind_vet]->da_mod_flag = 2;

fim :

   if ( ret == -1 ) {
      ret = ind_vet;
   }

   return (ret);
}


F__LOC   COUNT exc_prot(ind_vet)
COUNT ind_vet;
{  COUNT ret, campo;

   if ( ind_exc == MAX_EXC ) { /* nao ha mais espaco na tab_exc */
      men_conf(E_EXC_EXC, "*", H_CFEXPR, NULL);
      ret = ind_vet;
      goto fim;
   };

   load_da(vtprot[ind_vet]);  /* atribui valores de area alocada para pw */

   campo = 1;

le:

	ret = ed_read(j2, &tep[0], campo, SO_MOSTRA, str_nada);


   if ( men_conf(M_EXC_PRT, "SN", H_CFEXCPR, NULL) == 0 ) { /* ANANIAS @ */
		if ( vtprot[ind_vet]->da_mod_flag == 1 ) {
         /* foi incluida nesta mesma edicao */
         free((TEXT *)vtprot[ind_vet]);
         vtprot[ind_vet] = NULL;
         ret = -2;
      }
      else {
			vtprot[ind_vet]->da_mod_flag = 3;  /* campo excluido */
			vet_exc[ind_exc] = vtprot[ind_vet];
         ind_exc++;
         ret = -2;
      }

      num_prot--;
   }

fim :

   return (ret);
}



F__LOC   COUNT tr_visrel(entry)
d_entry *entry;
{  COUNT ret = REJEITA, tam_str, tam_op, i, conta, tipo, num;
   TEXT salva[TD+1];
   COUNT existe;
	Selecao m0;
	WIN *jf, *w;
	TEXT *pode, *p_nome, *p_opcao,  *m_acesso[MAX_VISTAS * 3 + 1];
   Vista    **vet_vis;
   Rel      **vet_rel;
   Vet_itens *vv;

   vv = NULL;

	ret = ACEITA;

   if ( entry->lin == LIN_V ) tipo = 'V';
   else                       tipo = 'R';

   tam_op    = MAX( strlen(OP_SIM) , strlen(OP_NAO) );

	m_acesso[MAX_VISTAS * 3] = NULL;

   for ( i = 0 ; i < MAX_VISTAS; i++ )
      m_acesso[OP(i)] = NULL;

	if ( entry->tecla == K_ESC || entry->tecla == K_DN || entry->tecla == K_UP )
		goto fim;

	if ( strcmp(entry->str, OP_SIM) == 0 ) {

      tam_str = MIN((entry->win->w_len - 4), (TD + TE)); /* vide lb2stru.h */


      if ( tipo == 'V' ) {
         if ( (vv = load_vistas(BASE_EDIT, 'U')) == NULL ) {
            mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
            goto fim;
         }
         vet_vis = (Vista **) vv->vi_vtit;  /* aponta para vetor de vistas */
      }
      else {
         if ( (vv = load_rels(BASE_EDIT, 'U')) == NULL ) {
            mens_erro(H_LBNBAD, E_LBNBAD, uerr_cod);
            goto fim;
         }
         vet_rel = (Rel**) vv->vi_vtit;  /* aponta para vetor de relats */
      }

      existe = vv->vi_nelem;


      /* monta vetor de vistas/relatorios existentes */

      if ( existe <= 0 ) {
         ret = 0;  /* sem vista/relatorio */
         goto fim;
      }

      for ( i = 0 ; i < existe; i++ ) {

         p_nome = vv->vi_vtit[i];
         strcpy(salva, p_nome);

         m_acesso[OP(i)] = p_nome;

         memset((TEXT *) p_nome, ' ', tam_str);

         p_opcao   = p_nome + tam_str - tam_op ;

         num = (tipo == 'V') ? vet_vis[i]->vis_ordem : vet_rel[i]->rel_ordem;

         if ( visrel_ok(num, tipo) )
            pode = OP_SIM;
         else
            pode = OP_NAO;

         strncpy(p_nome , salva, strlen(salva));

         strcpy(p_opcao, pode);

         m_acesso[AT(i)] = "1";
         m_acesso[AJ(i)] = M_DB3_INVERTE;
      };


      def_menu( &m0, j2->w_x + 1 , j2->w_y + 10, 53, 4, 1, /* estave 54 */
               j2->w_boxatr, MS_BARRA, 0, MS_JANELA, MS_DESTAQUE,
					&m_acesso, str_nada, str_nada);

      m0.comando = muda_acesso;

      jf = abre_funcoes(funcao("1 2 3 E"));
		w  = abre_win( j2->w_y + j2->w_aheight - 4, j2->w_x + 1,
                    j2->w_y + j2->w_aheight - 2, j2->w_x + j2->w_len - 2,
                     0, 0 , JS);
/*		w  = abre_win( j2->w_y + j2->w_aheight - 5, j2->w_x + 1,
                    j2->w_y + j2->w_aheight - 2, j2->w_x + j2->w_len - 2,
                     0, 0 , JS);
*/
	   ret = menu( &m0 );

      fwkill(w);
      fecha_funcoes(jf);

      ret = ACEITA;

      if ( esc_ativa == 0 ) {
         for (conta = 0, i = 0; i < MAX_VISTAS; i++) {

            if ( (p_nome = m_acesso[OP(i)]) == NULL )
               break;

            p_opcao   = p_nome + tam_str - tam_op ;

            if ( strncmp(p_opcao, OP_SIM, strlen(OP_SIM) ) == 0 ) {

               num = (tipo == 'V') ? vet_vis[i]->vis_ordem :
                                     vet_rel[i]->rel_ordem;

               if ( tipo == 'V' )
                  pw->vistas[conta++] = num;
               else
                  pw->relats[conta++] = num;
            }
         }

         for ( i = conta; i< MAX_VISTAS; i++)
            if ( tipo == 'V' )  pw->vistas[i] = 0;
            else                pw->relats[i] = 0;
      }
   }


fim :

   rlse_vet(vv);

	return(ret);
}



F__LOC   COUNT muda_acesso(s)
Selecao *s;
{  COUNT ret, tam_op;
   TEXT *ajuda_ant = win_ajuda;
   TEXT *p_nome, *p_opcao;

   win_ajuda = "MUDA_ACE";

   /* se a ultima tecla pressionada foi ESC, retorna -1 para fechar */

   s->tecla = fwnqgc(s->win);

   if ( s->tecla == K_F2 )  f2_ativa = 1;
   if ( s->tecla == K_ESC ) esc_ativa = 1;

   if ( s->tecla == K_ESC || s->tecla == K_F2 || s->tecla == K_ENTER ) {
      ret = -3; /* sai */
      goto fim;
   }

   if ( s->tecla != ' ' ) {
      ret = -2;  /* assume tecla */
      goto fim;
   }

   tam_op    = MAX( strlen(OP_SIM) , strlen(OP_NAO) );

   p_nome   = (*s->opcoes)[OP(s->escolhido)];
   p_opcao  = p_nome + strlen(p_nome) - tam_op ;

   if ( strncmp(p_opcao, OP_SIM, strlen(OP_SIM) ) == 0 ) {
      strncpy(p_opcao, OP_NAO, strlen(OP_NAO));
   }
   else {
      strncpy(p_opcao, OP_SIM, strlen(OP_SIM));
   };

   ret = -2; /* assume tecla */

fim :

   win_ajuda = ajuda_ant;
   fwsetcursor( s->win );

   return(ret);

}


/*******************************************************************/
/*******************************************************************/

F__LOC   COUNT prt_load( num, nome)
COUNT num;
TEXT *nome;
{  COUNT ret = OK, conta = 0, tam, salva_seg;
   TEXT  chave[LB_KEYMAX + 2], operador;
	Acesso *pc;

   RECALFIL(ace_dnum);
   RECALFIL(ace_inum);

   salva_seg = ct_semseguranca;
   ct_semseguranca = 1; /* para fazer com que find_rec consiga achar um proximo */

   /*inicializa vetor de ponteiros para PROTECOES */

	for ( conta=0; conta< num; conta++ ) {
		if ( vtprot[conta] != NULL ) {
         free((TEXT *) vtprot[conta] );
   		vtprot[conta] = NULL;
      }
   }

	conta    = 0;
   num_prot = 0;

   strcpy(chave, D1_ID);
	strcat(chave, nome);
   tam = strlen(chave);

	operador = '^'; /* maior ou igual */

   while ( 1 ) {

		pc = (Acesso *) calloc( 1, sizeof(Acesso));

		if ( pc == NULL ) {
         mens_erro(H_NOMEMO, SEM_MEMO);
			for ( ret = 0 ; ret < conta ; ret++ ) {
				 free((TEXT *) vtprot[ret]);
				 vtprot[ret] = NULL;
         }
			ret = !OK;
			goto fim;
		}

		if ( load_rec(ace_dnum, ace_inum, D1_ID, nome, pc , sizeof(Acesso), operador) == NULL ||
			  strncmp(ct_indkey, chave, tam) != 0 ||
           ct_indkey[tam] != '!' ) {
         free((TEXT *)pc);
   		vtprot[conta] = pc = NULL;
         break;
      }


      /* localizado uma descricao de uma protecao */

      pc->da_mod_flag = 0;

		vtprot[conta] = pc; /* o primeiro e o zero */

      conta++;

      if ( conta > MAX_PROTECOES )
         numdebug(783);

      num_prot++;
      operador = 'N';
   }

	ret = OK;

fim :

   ct_semseguranca = salva_seg ;

	return(ret);
}





/*******************************************************************/
F__LOC    COUNT prt_save(vet_exc, ind_exc )
Acesso **vet_exc;
COUNT *ind_exc;
{  POINTER 	pos;
	COUNT 	nc, ret;
	Acesso  *pc;
   TEXT    nome_base[TB + 1];
	TEXT 	  chave[LB_KEYMAX];
   WIN *jm = NULL;

   strcpy(nome_base, WBS->bs_nome);

   if ( if_prt_mod( *ind_exc ) == 0 ) { /* nenhuma alteracao */
      ret = 0;
      goto fim;
   }

   /*
   **    Processa protecoes excluidas
   */

   jm = abre_mensagem( M_DB3_WAIT );

   for ( nc = 0 ; nc < *ind_exc ; nc++) {

		pc = vet_exc[nc]; /* pc aponta para protecao excluida */

      /* primeiro exclui D2 = (IDENT + BASE ), so indice */
	   sprintf(chave, "%s!%s", pc->da_ugnome, nome_base);
      pos = exc_rec(ace_dnum, ace_inum, D2_ID, chave, SO_INDEX);
	   if (pos == 0) {
         mens_erro(H_DB3EXC, E_DB3_EXC);
		   ret = -1 ;
         goto fim;
      }

      /* exclui D1 do indice e exclui registro associado */
	   sprintf(chave, "%s!%s", nome_base, pc->da_ugnome);
      pos = exc_rec(ace_dnum, ace_inum, D1_ID,  chave, !SO_INDEX);
	   if (pos == 0) {
         mens_erro(H_DB3EXC, E_DB3_EXC);
		   ret = -1 ;
         goto fim;
      }
   }

   /* restaura condicao de nenhuma protecao na lista de excluidas */
   *ind_exc = 0;


   /*
   **    ATUALIZACAO DA DESCRICAO DAS PROTECOES
   */

	/* atualizacao de protecoes efetivas ( nao deletadas ) */


   for ( nc = 0; nc < num_prot && vtprot[nc] != NULL ; nc++) {

		pc = vtprot[nc]; /* pc aponta para protecao */

		switch ( pc->da_mod_flag ) {

         case 2 : /* modificadas caracteristicas da protecao */

                  /* primeiro exclui D2 = (IDENT + BASE ), so indice */
	               sprintf(chave, "%s!%s", pc->da_ugnome, nome_base);
                  pos = exc_rec(ace_dnum, ace_inum, D2_ID,  chave, SO_INDEX);
	               if (pos == 0) {
                     mens_erro(H_DB3EXC, E_DB3_EXC);
		               ret = -1 ;
                     goto fim;
                  }

                  /* exclui D1 do indice e exclui registro associado */
	               sprintf(chave, "%s!%s", nome_base, pc->da_ugnome);
                  pos = exc_rec(ace_dnum, ace_inum, D1_ID,  chave, !SO_INDEX);
	               if (pos == 0) {
                     mens_erro(H_DB3EXC, E_DB3_EXC);
		               ret = -1 ;
                     goto fim;
                  }

                  /* nao tem break, tem que passar no proximo case */

         case 1 : /* incluido     */


                 strcpy(pc->da_tpreg, DA_ID);

                 /* inclui registro e indice D1 */
					  sprintf(chave, "%s!%s", nome_base, pc->da_ugnome);
                 strcpy(pc->da_tpreg, DA_ID);
					  if ( (pos = add_rec(ace_dnum, ace_inum, D1_ID, chave, pc, sizeof(Acesso))) == (POINTER)0 ) {
                    mens_erro(H_DB3INC, E_DB3_INC);
						  ret = -1 ;
						  goto fim;
					  }

                 /* inclui chave no indice */
                 sprintf(chave, "%s!%s", pc->da_ugnome, nome_base);
					  if ( add_ind(ace_dnum, ace_inum, D2_ID, chave, pos) != 0 ) {
                    mens_erro(H_DB3INC, E_DB3_INC);
						  ret = -1 ;
						  goto fim;
					  }
					  break;


         case 0 : /* Nao alterado */
                  break;

         case 3 : numdebug(784);
                  fim_lb("100");
                  break;

         default : numdebug(785); /* Flag de modificacao nao prevista em prt_save */
                   ret = -1;
                   goto fim;
      }
   }

   ret = 0;

fim :

   fecha_mensagem(jm);

   return(ret);
}

/**
**#  ---------------------------------------------------------------------
**#   Funcao...: if_prt_mod
**#   Objetivo.: Verificar se foi efetuada alguma inc/alt/exc nas protecoes
**#              da base.
**#   Entrada..: b     - ponteiro para estrutura Base
**#              ind_exc - indice para vetor de protecoes excluidas
**#
**#   Retorna..: 0 - Nenhuma inc/alt/exc
**#              1 - EXISTE  inc/alt/exc efetuada
**#
**/
F__LOC   COUNT if_prt_mod( ind_exc )
COUNT ind_exc;
{  COUNT nc, ret;
	Acesso *pa;

   ret = 0;  /* nao houve alteracao */

	/* avalia se houve inclusao/alteracao de/nas protecoes */

	for ( nc = 0; nc < num_prot && (pa = vtprot[nc]) != NULL ; nc++) {
		if ( pa->da_mod_flag != 0 ) {
         ret = 1;
         break;
      }
   }


	/* avalia se houve exclusao de protecoes */

   if (ind_exc != 0 ) {
      ret = 1;
   }

   return(ret);
}



F__LOC   VOID load_da(p)
Acesso *p;
{  COUNT i;

   strcpy (pw->ident  , p->da_ugnome    );

	pw->campo[0]    = p->da_dfcampo ;    pw->campo[1]   = '\0';
	pw->vista[0]    = p->da_dfvista ;    pw->vista[1]   = '\0';
	pw->acesso[0]   = p->da_dfacesso;    pw->acesso[1]  = '\0';
	pw->relat[0]    = p->da_dfrel   ;    pw->relat[1]   = '\0';
	pw->stop[0]     = p->da_dfstop  ;    pw->stop[1]    = '\0';

	pw->inclui[0]   = p->da_inclui  ;    pw->inclui[1]  = '\0';
	pw->altera[0]   = p->da_altera  ;    pw->altera[1]  = '\0';
	pw->exclui[0]   = p->da_exclui  ;    pw->exclui[1]  = '\0';
	pw->pesq[0]     = p->da_pesquisa;    pw->pesq[1]    = '\0';
	pw->emite[0]    = p->da_emite   ;    pw->emite[1]   = '\0';
	pw->exporta[0]  = p->da_exporta ;    pw->exporta[1] = '\0';
	pw->importa[0]  = p->da_importa ;    pw->importa[1] = '\0';
	pw->indexa[0]   = p->da_parcial ;    pw->indexa[1]  = '\0';
	pw->total[0]    = p->da_total;       pw->total[1]   = '\0';
	pw->backup[0]   = p->da_backup  ;    pw->backup[1]  = '\0';
	pw->restore[0]  = p->da_restore ;    pw->restore[1] = '\0';

   strcpy(pw->edt_vis, OP_NAO);
   strcpy(pw->edt_rel, OP_NAO);

   for (i = 0; i < MAX_VISTAS; i++) {
       pw->vistas[i] = p->da_vistas[i] ;
       pw->relats[i] = p->da_rel[i] ;
   }
}



F__LOC   VOID seta_da(p)
Acesso *p;
{	COUNT i;
   strcpy (p->da_ugnome   , pw->ident  );

	p->da_dfcampo  = pw->campo[0] ;
	p->da_dfvista  = pw->vista[0] ;
	p->da_dfacesso = pw->acesso[0];
	p->da_dfrel    = pw->relat[0] ;
	p->da_dfstop   = pw->stop[0]  ;

	p->da_inclui   = pw->inclui[0]  ;
	p->da_altera   = pw->altera[0]  ;
	p->da_exclui   = pw->exclui[0]  ;
	p->da_pesquisa = pw->pesq[0]    ;
	p->da_emite    = pw->emite[0]   ;
	p->da_exporta  = pw->exporta[0] ;
	p->da_importa  = pw->importa[0] ;
	p->da_parcial  = pw->indexa[0]  ;
	p->da_total    = pw->total[0];
	p->da_backup   = pw->backup[0]  ;
	p->da_restore  = pw->restore[0] ;

   for (i = 0; i < MAX_VISTAS; i++) {
       p->da_vistas[i] = pw->vistas[i];
       p->da_rel[i] = pw->relats[i];
   }

}


F__LOC   VOID zera_pw()
{  COUNT i;

	memset((TEXT *)pw, '\0', sizeof(pw));

   for (i = 0; i < MAX_VISTAS; i++) {
       pw->vistas[i] = 0;
       pw->relats[i] = 0;
   }
}



/**
**#  ---------------------------------------------------------------------
**#   Funcao...: COUNT vl_pode
**#   Objetivo.: codificar tecla digitada em X
**#   Entrada..: entry - Endereco da estrutura de entrada de dados
**#   Retorna..: ACEITA    - Campo valido
**#              ACT_REIMP  - Campo valido deve ser reimpresso
**#
**/
F__LOC   COUNT vl_pode(entry)
d_entry *entry;
{  TEXT c;
   COUNT ret;

	c = entry->str[0];

   if ( c == 'X' )
      ret = ACEITA;
   else {
      c   = ' ';
      ret = ACT_REIMP;
   }

   entry->str[0] = c;
   entry->str[1] = '\0';

	return( ret );
}

F__LOC   COUNT visrel_ok(num, tipo)
COUNT num, tipo;
{  COUNT ret, i;

   ret = 0;

   if ( stat_loc == 'A' ) {
      ret = 1;
   }
   else {
      for ( i = 0; i < MAX_VISTAS; i++) {
         if ( (tipo == 'V' ? pw->vistas[i] : pw->relats[i] ) == num ) {
            ret = 1;
            break;
         }
      }
   }

   return(ret);

}


F__LOC   VOID seta_tep()
{
   seta_dtela(&tep[0], pw->inclui, "X", M_AC_INCLUI, 1, 4,  0, E_ALFA, H_ACINC,   NULL, NULL, vl_pode);
   seta_dtela(&tep[1], pw->altera, "X", M_AC_ALTERA, 2, 4,  0, E_ALFA, H_ACALT,   NULL, NULL, vl_pode);
   seta_dtela(&tep[2], pw->exclui, "X", M_AC_EXCLUI, 3, 4,  0, E_ALFA, H_ACEXC,   NULL, NULL, vl_pode);
   seta_dtela(&tep[3], pw->pesq,   "X", M_AC_PESQ,   4, 4,  0, E_ALFA, H_ACPESQ,  NULL, NULL, vl_pode);
   seta_dtela(&tep[4], pw->emite,  "X", M_AC_EMITIR, 5, 4,  0, E_ALFA, H_ACEMIT,  NULL, NULL, vl_pode);
   seta_dtela(&tep[5], pw->exporta,"X", M_AC_EXPORT, 6, 4,  0, E_ALFA, H_ACEXPO,  NULL, NULL, vl_pode);
   seta_dtela(&tep[6], pw->importa,"X", M_AC_IMPORT, 7, 4,  0, E_ALFA, H_ACIMPO,  NULL, NULL, vl_pode);

   seta_dtela(&tep[7], pw->indexa, "X", M_AC_PARCIAL, 1, 33,  0, E_ALFA, H_ACINDP, NULL, NULL, vl_pode);
   seta_dtela(&tep[8], pw->total,  "X", M_AC_TOTAL,   3, 33,  0, E_ALFA, H_ACINDT, NULL, NULL, vl_pode);
   seta_dtela(&tep[9], pw->backup, "X", M_AC_BACKUP,  5, 33,  0, E_ALFA, H_ACBACK, NULL, NULL, vl_pode);
   seta_dtela(&tep[10],pw->restore,"X", M_AC_RESTORE, 7, 33,  0, E_ALFA, H_ACREST, NULL, NULL, vl_pode);

   seta_dtela(&tep[11], pw->edt_vis,  "xxx", M_AC_RVISTA, LIN_V, 46,  0, E_LISTA, H_ACVIST, op_sn, NULL, tr_visrel);
   seta_dtela(&tep[12], pw->edt_rel,  "xxx", M_AC_RREL,   LIN_R, 46,  0, E_LISTA, H_ACRELS, op_sn, NULL, tr_visrel);

   tep[13].destino = NULL;
};



F__LOC   VOID monta_tela( )
{
/*
   fwframe( 0,  0,  8, 16, SIMPLES, j2);   mvwprintw(j2 ,  0,  3, C_DB3_DEF);
   fwframe( 0, 18,  8, 35, SIMPLES, j2);   mvwprintw(j2 ,  0, 21, C_DB3_UTI);
   fwframe( 0, 37,  8, 54, SIMPLES, j2);   mvwprintw(j2 ,  0, 40, C_DB3_MAN);
   fwframe( POS_BORDA, SIMPLES, j2);       mvwprintw(j2 ,  9, 13, C_DB3_VR);
*/

   fwframe( 0,  0,  8, 24, SIMPLES, j2);   mvwprintw(j2 ,  0,  6, C_DB3_UTI);
   fwframe( 0, 29,  8, 54, SIMPLES, j2);   mvwprintw(j2 ,  0, 36, C_DB3_MAN);
   fwframe( POS_BORDA, SIMPLES, j2);       mvwprintw(j2 ,  9, 13, C_DB3_VR);


   mvwprintw(j2 ,  1, 3, C_DB3_PINC );
   mvwprintw(j2 ,  2, 3, C_DB3_PALT );
	mvwprintw(j2 ,  3, 3, C_DB3_PEXC );
	mvwprintw(j2 ,  4, 3, C_DB3_PPSQ );
	mvwprintw(j2 ,  5, 3, C_DB3_PEMI );
	mvwprintw(j2 ,  6, 3, C_DB3_PEXP );
	mvwprintw(j2 ,  7, 3, C_DB3_PIMP );

   mvwprintw(j2 ,  1, 32, C_DB3_PPAR );
	mvwprintw(j2 ,  3, 32, C_DB3_PTOT );
	mvwprintw(j2 ,  5, 32, C_DB3_PBAK );
	mvwprintw(j2 ,  7, 32, C_DB3_PRES );

	mvwprintw(j2 , 11,  5, C_DB3_MVIS); /* se mudar ver reimp_tela */
	mvwprintw(j2 , 12,  5, C_DB3_MREL); /* se mudar ver reimp_tela */
}

