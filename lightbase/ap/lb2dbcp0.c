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

#define   GRP_DFCMP    20


#define   LF13E       "1 3 E"
#define   LF123E      "1 2 3 E"
#define   LF1230E      "1 2 3 0 E"
#define   LF1236IDE   "1 2 3 6 IDE"

#define   STR_DUAS    "%s\n%s"

#define   IND_DX      7           /* numero de reads em dx */
#define   IND_DI      3           /* numero de reads em di */


#define   H_NMCPIN   "CP_NMINV"      /* Nome de campo invalido */
#define   H_CPDESC   "CP_DESC"      /* Descricao do campo*/
#define   H_CPRODA   "CP_RODA"      /* Rodape do campo */
#define   H_CPTIPO   "CP_TIPO"      /* Tipo de campo*/
#define   H_CPTAM    "CP_TAM"       /* tamanho do campo*/
#define   H_CPDEC    "CP_DEC"       /* Numero de decimais*/
#define   H_CPMASC   "CP_MASC"      /* Mascara do campo */
#define   H_CPCRIT   "CP_CRIT"      /* Critica do campo*/

#define   H_DUPCP    "DUPNMCP"      /* Nome de campo ja existe */
#define   H_GRVCP    "ABMODCP"      /* Abandona modificacoes em campos */

#define   H_CPEXC    "CFEXCCP"      /* Confirme exclusao do campo */
#define   H_CRNMTAB  "DBNOMETA"     /* Nome de tabela na critica */

#define   H_CRITMIN  "DBMINIMO"     /* Parametro minimo na critica */
#define   H_CRITMAX  "DBMAXIMO"     /* Parametro maximo na critica */

#define   H_CPINDE   "CP_INDE"      /* campo devera ser indexado */
#define   H_CPUNIC   "CP_UNIC"      /* campo devera ser Chave Unica */
#define   H_CPBRAN   "CP_BRAN"      /* campo podera ser pesquisado em branco */
#define   H_CPFONET  "CP_FONET"     /* campo permitira pesquisa fonetica */








Cw    *cw;
static WIN *j2;
static COUNT f2_ativa;
static DTela *end_dx;
static Campo    **vet_cmp;
static COUNT ultseq_loc;
static COUNT excluiu_velho;

extern SC *tab_ultimo;
Campo  *pc_glbcrit;


#ifdef MSDOS
   COUNT exec_campo(Tab *);
   COUNT dsp_campo(Tab *);
   COUNT opr_cmp( COUNT, TEXT *, COUNT);
   COUNT f2_campos( VOID );
   VOID  load_cp(Campo *);
   VOID  seta_cp(Campo *, TEXT *);
   VOID  zera_cw(VOID);
   COUNT carac_index(VOID);

   COUNT cr_campo( VOID );
   COUNT vl_rodape(d_entry *);
   COUNT pr_tipo(d_entry *);
   COUNT vl_tipo(d_entry *);
   COUNT pr_tam(d_entry *);
   COUNT vl_tam(d_entry *);
   COUNT pr_dec(d_entry *);
   COUNT vl_dec(d_entry *);
   COUNT pr_mascara(d_entry *);
   COUNT vl_mascara(d_entry *);
   COUNT pr_critica(d_entry *);
   COUNT vl_critica(d_entry *);
   COUNT vl_ntab(d_entry *);
   COUNT vl_minimo(d_entry *);
   COUNT vl_maximo(d_entry *);
   COUNT cr_unica(d_entry *);
#else
   COUNT exec_campo( );
   COUNT dsp_campo( );
   COUNT opr_cmp( );
   COUNT f2_campos( );
   VOID  load_cp( );
   VOID  seta_cp( );
   VOID  zera_cw( );
   COUNT carac_index( );

   COUNT cr_campo( );
   COUNT vl_rodape( );
   COUNT pr_tipo( );
   COUNT vl_tipo( );
   COUNT pr_tam( );
   COUNT vl_tam( );
   COUNT pr_dec( );
   COUNT vl_dec( );
   COUNT pr_mascara( );
   COUNT vl_mascara( );
   COUNT pr_critica( );
   COUNT vl_critica( );
   COUNT vl_ntab( );
   COUNT vl_minimo( );
   COUNT vl_maximo( );
   COUNT cr_unica( );
#endif



#define   T_CPCAR       loc_str(GRP_DFCMP, 21)
#define   T_CPREL       loc_str(GRP_DFCMP, 22)
#define   T_CPROD       loc_str(GRP_DFCMP, 23)

#define   D_CPDF1       loc_str(GRP_DFCMP, 24)
#define   D_CPDF2       loc_str(GRP_DFCMP, 25)
#define   D_CPDF3       loc_str(GRP_DFCMP, 26)
#define   D_CPDF4       loc_str(GRP_DFCMP, 27)
#define   D_CPDF5       loc_str(GRP_DFCMP, 28)
#define   D_CPDF6       loc_str(GRP_DFCMP, 29)
#define   D_CPDF7       loc_str(GRP_DFCMP, 30)

#define   R_CPDF1       loc_str(GRP_DFCMP, 31)
#define   R_CPDF2       loc_str(GRP_DFCMP, 32)
#define   R_CPDF3       loc_str(GRP_DFCMP, 33)
#define   R_CPDF4       loc_str(GRP_DFCMP, 34)
#define   R_CPDF5       loc_str(GRP_DFCMP, 35)
#define   R_CPDF6       loc_str(GRP_DFCMP, 36)

#define   M_CPSEL       loc_str(GRP_DFCMP, 37)
#define   M_CPINS       loc_str(GRP_DFCMP, 38)
#define   M_CPALT       loc_str(GRP_DFCMP, 39)

#define   M_CPATU       loc_str(GRP_DFCMP, 40)

#define   TW_CPEX9      loc_str(GRP_DFCMP, 41)
#define   TW_CPEX1      loc_str(GRP_DFCMP, 42)

#define   R_CPCR1R      loc_str(GRP_DFCMP, 43)
#define   R_CPCR2R      loc_str(GRP_DFCMP, 44)
#define   R_CPCR3R      loc_str(GRP_DFCMP, 45)

#define   T_CPCRT       loc_str(GRP_DFCMP, 46)
#define   T_CPCRI       loc_str(GRP_DFCMP, 47)
#define   D_CPCRT1      loc_str(GRP_DFCMP, 48)
#define   D_CPCRI1      loc_str(GRP_DFCMP, 49)
#define   D_CPCRI2      loc_str(GRP_DFCMP, 50)

#define   T_CPIDX       loc_str(GRP_DFCMP, 51)
#define   D_CPIDX1      loc_str(GRP_DFCMP, 52)
#define   D_CPIDX2      loc_str(GRP_DFCMP, 53)
#define   D_CPIDX3      loc_str(GRP_DFCMP, 54)
#define   D_CPIDX4      loc_str(GRP_DFCMP, 59)

#define   E_MAXCMP      loc_str(GRP_DFCMP, 55)
#define   E_TFUNICA     loc_str(GRP_DFCMP, 56)
#define   E_23UNICA     loc_str(GRP_DFCMP, 57)
#define   E_DUPCP       loc_str(GRP_DFCMP, 58)

#define   D_CPIDX5      loc_str(GRP_DFCMP, 60)
#define   D_CPIDX6      loc_str(GRP_DFCMP, 61)

#define   M_HELPCP      loc_str(GRP_DFCMP, 62)
#define   M_AUTONUM     loc_str(GRP_DFCMP, 63)

#define   H_HELPCP      "dfhelpcp"
#define   H_AUTONUM     "dfmscaut"


F__GLB   VOID rec_campos(sele)
COUNT sele;
{	Tab   t;
	UTEXT str_escape[20];
   TEXT *ajuda_ant = win_ajuda;
	TEXT  lin_fun[100], lin_alt[100], tec_fun[15];
   WIN   *jm = NULL, *jf = NULL, *ji = NULL;
   DTela dx[IND_DX + 1];

   up_str(GRP_DFCMP);

   win_ajuda = "DEF_CMP";

   cw = (Cw *) calloc(1, sizeof(Cw));
   end_dx = &dx[0];
   vet_cmp = pos_vetcmp(sele);

   seta_dtela(&dx[0], cw->desc,     x40,   R_CPDF1,  1, 15, 37, E_ALFA,  H_CPDESC, NULL, NULL, NULL);
   seta_dtela(&dx[1], cw->rodape,   x50,   R_CPDF2,  3, 15, 37, E_ALFA,  H_CPRODA, NULL, NULL, vl_rodape);
   seta_dtela(&dx[2], cw->tipo,     x30,   R_CPDF3,  5, 15,  0, E_LISTA, H_CPTIPO, OP_CAMPOS, pr_tipo, vl_tipo);
   seta_dtela(&dx[3], cw->tamanho,  "999", R_CPDF4,  7, 15,  0, E_VALOR, H_CPTAM,  NULL, pr_tam, vl_tam);
   seta_dtela(&dx[4], cw->decimais, "99",  R_CPDF5,  7, 31,  0, E_VALOR, H_CPDEC,  NULL, pr_dec, vl_dec);
   seta_dtela(&dx[5], cw->mascara,  xTM,   R_CPDF6,  9, 15, 37, E_ALFA,  H_CPMASC, NULL, pr_mascara, vl_mascara);
   seta_dtela(&dx[6], cw->critica,  x30,   R_CPDF3, 11, 15,  0, E_LISTA, H_CPCRIT, OP_CRITICAS, pr_critica, vl_critica);
   seta_dtela(&dx[7], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);


   j2 = abre_janela(J_DEFCAMPOS);
	fw_title(j2, T_CPCAR, j2->w_atrib);
	fwbox(j2);

	strcpy(tec_fun, LF1236IDE);

	def_tabela(&t, 3, 8, 25, 13, qtd_vetcmp(sele), MAX_CAMPOS, X14,
				   MS_CAMPO, MS_BARRA, SIMPLES, MS_JANELA, MS_DESTAQUE,
				   ( char *(* )[]) vet_cmp, T_CPREL ,  str_nada );

	pr_escape(tec_fun, str_escape);

	strncpy(lin_fun, funcao(LF123E), 99);
	strncpy(lin_alt, funcao(LF123E), 99);

   t.mens_ins  = M_CPINS;
   t.mens_alt  = M_CPALT;
	t.fun_ins   = lin_fun;
	t.fun_alt   = lin_alt;
   t.escape    = str_escape;
   t.edita     = INSERE | ALTERA | DELETA ;
   t.comando   = dsp_campo;
	t.executa   = exec_campo;


	mvwprintw(j2 ,  1,  1, D_CPDF1);
	mvwprintw(j2 ,  3,  1, D_CPDF2);
   mvwprintw(j2 ,  5,  1, D_CPDF3);
	mvwprintw(j2 ,  7,  1, D_CPDF4);
   mvwprintw(j2 ,  7, 20, D_CPDF5);
   mvwprintw(j2 ,  9,  1, D_CPDF6);
   mvwprintw(j2 , 11,  1, D_CPDF7);

   jm = abre_mensagem( M_CPSEL );
	jf = abre_funcoes(funcao(tec_fun));
   ji = abre_insercol(T_CPROD);

   ultseq_loc = WBS->bs_u_seq;

   tabela(&t);

	fecha_insercol(ji);
	fecha_funcoes(jf);
	fecha_mensagem(jm);

   /* se modificacoes efetuadas nao foram gravadas. Reload em campos */
	if ( (v_campos[BASE_EDIT]->vi_status & (~VET_EDIT)) != 0  ) {
      rlse_campos(BASE_EDIT);
      if ( (v_campos[BASE_EDIT] = load_campos(BASE_EDIT, 'E')) == NULL ) {
         mens_erro(H_NOMEMO, SEM_MEMO);
         goto fim;
      }
   }


fim :

	fwkill(j2);

   if ( cw ) free(cw);

   win_ajuda = ajuda_ant;

   down_str(GRP_DFCMP);
}






F__LOC   COUNT dsp_campo(tp)
Tab *tp;
{  COUNT salva;

   salva = w_refresh;  /* otimizar refresh */
   w_refresh = 0;


   if ( w_kbhit(j2) ) goto fim;

	if ( tp->escolhido != (tp->n_elem - 1)) {

		load_cp(vet_cmp[tp->escolhido])	; 	/* atribui valores de cw para area alocada */

		ed_read(j2, end_dx, 0, SO_MOSTRA, str_nada);
	}
	else {
		memset((TEXT *) cw, '\0', sizeof(cw));
		ed_read(j2, end_dx, 0, SO_LIMPA, str_nada);  /* so limpa os campos */
	}

fim:

   w_refresh = salva;
   fwrefresh(j2);

   fwsetcursor( tp->win );

   return ( -2 );
}



COUNT exec_campo(tp)
Tab *tp;
{
	COUNT ret;

   fwtop( j2 );

   f2_ativa = 0;

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
		case K_DEL  :  /* Exclui campo */
      case K_ENTER:  /* Editar caracteristicas do campo */
		case K_F6   :  /* Alterar o nome do campo */
      case K_INS  :  /* Incluir um novo campo */
							ret = opr_cmp(tp->escolhido, tp->e->str, tp->tecla);
							if ( ret == -2 && tp->tecla != K_DEL )
                        tp->escolhido++;
                     break;

		case K_F2  :   /* Gravar registro (atualizar base) */
                     f2_campos( );
                     ret = -3; /* finaliza edicao */
						   break;

      case K_ESC :   /* Abandonar edicao */
						   if ( (v_campos[BASE_EDIT]->vi_status & (~VET_EDIT)) != 0  ) {
                        ret =  decida(TW_GGV0, TW_GGV9, H_GRVCP,
                                      3, 42, STR_DUAS, TW_GGV1, TW_GGV2);
                        if ( ret == 0 ) {
                           f2_campos();
                           ret = -3;
                        }
                        else if ( ret == K_ESC ) ret = tp->escolhido;
      		 			   else ret = -3;
                     }
     					   else ret = -3;

						   break;

		default      : numdebug(202);
                     break;
   }

   if ( f2_ativa ) {
      f2_campos( );
      ret = -3; /* finaliza edicao */
   }

fim :

   fwtop( tp->win );
   fwsetcursor( tp->win );

   return(ret);
};


COUNT opr_cmp(ind_vet, nome_campo, tecla)
COUNT ind_vet;
TEXT *nome_campo;
COUNT tecla;
{  COUNT ret, campo, num_cp;
   TEXT rodape_cp[6];
   Campo *pc;

   if ( tecla == K_INS || tecla == K_F6 ) {
	   if ( val_indice(nome_campo) != OK) {
		   mens_erro(H_NMCPIN, E_NOMEIN);
		   ret = ind_vet;
         goto fim;
      }

      /* verificar se nome proposto ja existe */

               /* mascarar nome em teste */
               pc = vet_cmp[ind_vet];
               vet_cmp[ind_vet] = (Campo *) " - - - ";
               v_campos[BASE_EDIT]->vi_nelem++;

      ret = ver_campo(BASE_EDIT, nome_campo, &campo);

               /* restaurar nome em teste */
               vet_cmp[ind_vet] = pc;
               v_campos[BASE_EDIT]->vi_nelem--;

      if ( ret >= 0 ) {
         mens_erro(H_DUPCP, E_DUPCP);
         ret = ind_vet;
         goto fim;
      }
   }

   if ( tecla == K_INS ) {
      zera_cw();
      num_cp = ultseq_loc + 1;
   }
	else {
      load_cp(vet_cmp[ind_vet]); 	/* atribui valores de cw para area alocada */
      num_cp = vet_cmp[ind_vet]->cp_num;
   }

   if ( num_cp > MAX_CAMPOS ) {
      debug(E_MAXCMP);
      ret = -1;
      goto fim;
   }

   campo = 1;

le :

   sprintf( rodape_cp, "%03.3d", num_cp );
	fw_bottom(j2, rodape_cp);

   pc_glbcrit = vet_cmp[ind_vet];

	ret = ed_read(j2, end_dx, campo,
                 (tecla == K_DEL) ? SO_MOSTRA : MOSTRA_LE, LF1230E);

	fw_bottom(j2, "");
/*   fwrefresh(j2); */

   campo = w_lastread;

pos_le:

	switch (ret) {
      case K_ESC  :  ret = -1;
                     goto fim;
      case K_F0   :  ret = carac_index();
                     if ( ret != 0 ) {
                        goto pos_le;
                     }
                     goto le;
		case K_UP   :
		case K_PGUP :  campo =  1; goto le;
		case K_DN   :
		case K_PGDN :  campo = (campo >= IND_DX ) ? 1 : IND_DX;
		               goto le;
      case K_F2   :  campo = cr_campo();
                     if ( campo != 0 ) goto le;
                     f2_ativa = 1;
                     ret = -2;
                     break;
		case K_ENTER:
      default     : ret = -2;   break;
	}


   switch(tecla) {
      case K_F6   :  /*  Modifica inclusive o nome do campo */
                     pc = vet_cmp[ind_vet];
                     break;

      case K_INS  :  /*  Insere  campo novo */
                     pc = (Campo *) add_item(v_campos[BASE_EDIT], ind_vet,
                                             NULL, sizeof(Campo), 0);
                     ultseq_loc++;
                     if ( pc != NULL ) {
                        pc->cp_num = ultseq_loc;
	                     pc->cp_mod_flag  = NOVO;
	                     pc->cp_reb_flag  = NOVO;
                     }
                     break;

      case K_DEL  :  /*  Exclui campo */
                     pc = vet_cmp[ind_vet];
                     if ( decida(str_nada, TW_CPEX9, H_CPEXC,
                          1, 32, "%s", TW_CPEX1) == 1 ) {
	                     if ( pc->cp_mod_flag != NOVO )
                           excluiu_velho = 1;
                        pos_item( v_campos[BASE_EDIT], ind_vet);
                        del_item( v_campos[BASE_EDIT], 0);
                     }
                     else
                        ret = -1;

                     break;

      default     :  /*   Modifica   */
                     pc = vet_cmp[ind_vet];
                     nome_campo = NULL;
                     break;

   }

   if ( tecla != K_DEL ) {
      if ( nome_campo != NULL )
   	   seta_cp(pc, nome_campo);
      else
         seta_cp(pc, pc->cp_nome);
   }


fim :

   if ( ret == -1 )
      ret = ind_vet;

   return (ret);
}


/*******************************************************************/
COUNT f2_campos( )
{  POINTER 	pos, pos_ant;
	COUNT 	nc, ret, tam;
   Campo   *pc;
   WIN *jm = NULL;

	if ( (v_campos[BASE_EDIT]->vi_status & (~VET_EDIT)) == 0  ) {
      ret = 0;
      goto fim;
   }

   jm = abre_mensagem( M_CPATU );

   /*
   **    ATUALIZACAO DA DESCRICAO DOS CAMPOS
   */

   for ( nc = 0 ; nc < qtd_vetcmp(BASE_EDIT); nc++) {

      pc = vet_cmp[nc];

      if ( pc->cp_mod_flag == NOVO ) { /* campo incluido */
         pc->cp_a_tipo = pc->cp_tipo; /* cria anterior */
         if ( pc->cp_tipo == 'N' || pc->cp_tipo == 'F')
            pc->cp_a_tam  = pc->cp_mhor + (pc->cp_decimais << 5 ); /* cria anterior */
         else
            pc->cp_a_tam  = pc->cp_mhor; /* cria anterior */

         pc->cp_reb_flag = NAO_MODIFICADO;
      }

#ifdef CP_FLAG
      if ( par_cfg.flags_especiais & 0X04 ) {
         if ( pc->cp_mhor == 1 )
            pc->cp_1futuro = 'F';
      }
#endif



      if ( pc->cp_tipo == 'N' || pc->cp_tipo == 'F') {
         if ( pc->cp_a_tipo != pc->cp_tipo )
            pc->cp_reb_flag = REB_OBRIGATORIO;

         if ( pc->cp_a_tam != ( pc->cp_mhor + (pc->cp_decimais << 5 ) ))
            pc->cp_reb_flag = REB_OBRIGATORIO;
      }
      else {
         if ( pc->cp_a_tipo != pc->cp_tipo || pc->cp_a_tam  > pc->cp_mhor )
            pc->cp_reb_flag = REB_OBRIGATORIO;
      }

      pc->cp_mod_flag = NAO_MODIFICADO;
      pc->cp_ordem = (TEXT) (nc + 1);
      pc->cp_pttab = NULL;
   }


   /* salva vetor de campos */

   ret = -1;

   tam = v_campos[BASE_EDIT]->vi_mtam + TAMANHO_HDR;
   pos_ant = WBS->bs_cppos;
   pos = lb4_space(pos_ant, tam);
   if ( pos != (POINTER) 0 ) {
      if ( save_vet(v_campos[BASE_EDIT], pos) != (POINTER) NULL ) {
         WBS->bs_cppos = pos;
         WBS->bs_u_seq = ultseq_loc;
         WBS->bs_updated = 1;
         if ( excluiu_velho )
            WBS->bs_status = 1;

         if ( lb4_upd( 1 ) == 0 )
            ret = 0;
      }
   }

fim :

   fecha_mensagem(jm);

   return(ret);
}



COUNT pega_param(critica)
TEXT critica;
{  COUNT ret, param;
   WIN *j1;
	DTela vt[3];




   switch( critica ) {

      case 'T' :  /*------------------------------------------ tabela ---*/

         seta_dtela(&vt[0], cw->param.tab_auto.tabela, X8, R_CPCR1R,
                  0, 9, 0, E_ALFA, H_CRNMTAB, NULL, NULL, vl_ntab);
         seta_dtela(&vt[1], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

         j1 = abre_win(13, 45, 13, 70, 0, 0, JS);  /* abre janela tabela */
         fw_title(j1, T_CPCRT, j1->w_atrib);
	      fwbox(j1);

	      mvwprintw(j1, 0, 2, D_CPCRT1);
         break;


      case 'I' : /*----------------------------------------- intervalo ---*/
         if ( cw->tipo_x == 'N' )  param = E_VALOR;
         else                      param = E_ALFA;

         seta_dtela(&vt[0],cw->param.inter.minimo, cw->mascara, R_CPCR2R,
                  0, 11, 0, param, H_CRITMIN, NULL, NULL, vl_minimo);
         seta_dtela(&vt[1],cw->param.inter.maximo, cw->mascara, R_CPCR3R,
                  1, 11, 0, param, H_CRITMAX, NULL, NULL, vl_maximo);
         seta_dtela(&vt[2], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

         j1 = abre_win(13, 45, 14, 75, 0, 0, JS);  /* abre janela valores */
         fw_title(j1, T_CPCRI, j1->w_atrib);
	      fwbox(j1);

	      mvwprintw(j1, 0, 2, D_CPCRI1);
	      mvwprintw(j1, 1, 2, D_CPCRI2);

         break;

      default : goto fim; /* ANANIAS @ colocar outras criticas */
	}


le :

 	ret = ed_read(j1, vt, 1, MOSTRA_LE, LF13E);

	switch (ret) {
      case -1    :
      case K_ESC : ret = -1; goto fim;

		case K_UP  : goto le;

      default    :
                   break;
	}


fim :

	fwkill( j1 );
   return(ret);
}



VOID load_cp(p)
Campo *p;
{  COUNT tipo, tam;

   strcpy(cw->nome,    p->cp_nome    );
	strcpy(cw->desc,    p->cp_desc    );
	strcpy(cw->mascara, p->cp_mascara );
	strcpy(cw->rodape,  p->cp_rodape  );
	strcpy(cw->help,    p->cp_help    );

	if ( p->cp_tipo == 'N' || p->cp_tipo == 'F' ) {
      if (p->cp_decimais > 0 )
         tam = p->cp_mhor - p->cp_decimais - 1;
      else
         tam = p->cp_mhor;

      tam = tam - (tam / 4);
      sprintf(cw->tamanho, "%d", tam);
      sprintf(cw->decimais, "%d", p->cp_decimais);
   }
   else {
      sprintf(cw->tamanho, "%d", p->cp_mhor);
      cw->decimais[0] = '\0';
   }

   switch ( p->cp_tipo ) {
      case 'x' : tipo = 1; break;
      case 'T' : tipo = 2; break;
      case 'N' : tipo = 3; break;
      case 'F' : tipo = 4; break;
      case 'D' : tipo = 5; break;
      case 'H' : tipo = 6; break;
      case 'd' : tipo = 7; break;
      case 'h' : tipo = 8; break;

      default  : tipo = 1;  break;
   }

   piece(OP_CAMPOS, cw->tipo, '@', tipo, TE);
   cw->tipo_x = p->cp_tipo;

   switch ( p->cp_critica ) {
      case 'N' : tipo = 1;  break;
      case 'I' : tipo = 2;  break;
      case 'T' : tipo = 3;  break;
      case 'M' : tipo = 4;  break;
      case 'C' : tipo = 5;  break;
      case 'G' : tipo = 6;  break;
#ifdef AUTO_INCREMENTO
      case 'A' : tipo = 7;  break;
#endif
      default  : tipo = 1;  break;
   }

   piece(OP_CRITICAS, cw->critica, '@', tipo, TE);
   cw->crit_x = p->cp_critica;

   strcpy( cw->unica,    (p->cp_chave_unica == 'S') ? OP_SIM : OP_NAO);
   strcpy( cw->index,    (p->cp_indexavel   == 'S') ? OP_SIM : OP_NAO);
   strcpy( cw->branco,   (p->cp_branco      == 'S') ? OP_SIM : OP_NAO);
   strcpy( cw->fonetica, (p->cp_fonetica    == 'S') ? OP_SIM : OP_NAO);

	memcpy((TEXT *)&cw->param, (TEXT *) &p->cp_param, sizeof(cw->param));

}



VOID seta_cp(p, nome)
Campo *p;
TEXT *nome;
{  COUNT avisa, modificado, tam, dec;
   TEXT unica, indexa, branco, fonetica;

   unica = indexa = branco = fonetica = 'N';

   if ( strcmp( cw->unica,    OP_SIM) == 0 ) unica    = 'S';
   if ( strcmp( cw->index,    OP_SIM) == 0 ) indexa   = 'S';
   if ( strcmp( cw->branco,   OP_SIM) == 0 ) branco   = 'S';
   if ( strcmp( cw->fonetica, OP_SIM) == 0 ) fonetica = 'S';

   modificado = NAO_MODIFICADO;
   avisa = 0;

   tam = atoi(cw->tamanho);
   dec = atoi(cw->decimais);

   if ( p->cp_mod_flag != NOVO ) {
      if ( strcmp(p->cp_nome, nome ) != 0 ) avisa = 1;

      if ( p->cp_mhor != tam )
         modificado = MODIFICADO;

      if ( p->cp_mhor != tam && cw->tipo_x == 'F') {
         modificado = MODIFICADO;
         if ( p->cp_mhor >  tam ) avisa = 1;
         if ( p->cp_decimais !=  dec ) avisa = 1;
         if ( strncmp(p->cp_mascara, cw->mascara, p->cp_mhor) != 0 ) avisa = 1;
      }
      else {
         if ( strcmp(p->cp_mascara, cw->mascara) != 0 ) avisa = 1;
      }


      if ( p->cp_tipo        != cw->tipo_x ) avisa = 1;
      if ( p->cp_critica     != cw->crit_x ) avisa = 1;
      if ( p->cp_chave_unica != unica   ) avisa = 1;
      if ( p->cp_indexavel   != indexa  ) avisa = 1;
      if ( p->cp_branco      != branco  ) avisa = 1;
      if ( p->cp_fonetica    != fonetica) avisa = 1;
   }

   if ( avisa ) modificado = 1;

   if ( strcmp(p->cp_desc,    cw->desc   ) != 0 ) modificado = MODIFICADO;
   if ( strcmp(p->cp_rodape,  cw->rodape ) != 0 ) modificado = MODIFICADO;
   if ( strcmp(p->cp_help,    cw->help   ) != 0 ) modificado = MODIFICADO;
	if ( memcmp((TEXT *)&p->cp_param ,
               (TEXT *) &cw->param , sizeof(p->cp_param)) != 0 )
                                                 modificado = MODIFICADO;

	if ( cw->tipo_x == 'N' || cw->tipo_x == 'F' ) {
	   p->cp_decimais = dec;
	   p->cp_mhor = tam + ( (tam-1) / 3 );
      if ( dec > 0 ) {
         p->cp_mhor += (dec + 1);
      }
   }
   else {
   	p->cp_decimais = 0;
   	p->cp_mhor = tam;

   }

	strcpy(p->cp_nome, nome);
	strcpy(p->cp_desc, cw->desc   );
	strcpy(p->cp_mascara   , cw->mascara);
	strcpy(p->cp_rodape    , cw->rodape );
	strcpy(p->cp_help      , cw->help   );

   p->cp_tipo        = cw->tipo_x;
	p->cp_critica     = cw->crit_x;

   p->cp_chave_unica = unica;
   p->cp_indexavel   = indexa;
   p->cp_branco      = branco;
   p->cp_fonetica    = fonetica;

	memcpy((TEXT *)&p->cp_param , (TEXT *) &cw->param, sizeof(p->cp_param));

   if ( p->cp_mod_flag == NAO_MODIFICADO ) {
      p->cp_mod_flag = modificado;
      if ( avisa != 0 && p->cp_reb_flag == NAO_MODIFICADO )
         p->cp_reb_flag = REB_WARNING;
   }

   if ( modificado )
      v_campos[BASE_EDIT]->vi_status |=  VET_ALT;

}


VOID zera_cw()
{
   memset(cw, '\0', sizeof(Cw));
   cw->tipo_x = 'x';
   cw->crit_x = 'N';
   strcpy(cw->unica,    OP_NAO);
   strcpy(cw->index,    OP_SIM);
   strcpy(cw->branco,   OP_NAO);
   strcpy(cw->fonetica, OP_NAO);
}



COUNT carac_index()
{  COUNT ret, campo;
   WIN *ji;
   DTela di[7];


   seta_dtela(&di[0], cw->index,   "xxx", M_INVERTE, 1, 45,  0, E_LISTA, H_CPINDE,  op_sn, NULL, NULL);
   seta_dtela(&di[1], cw->unica,   "xxx", M_INVERTE, 2, 45,  0, E_LISTA, H_CPUNIC,  op_sn, NULL, cr_unica);
   seta_dtela(&di[2], cw->branco,  "xxx", M_INVERTE, 3, 45,  0, E_LISTA, H_CPBRAN,  op_sn, NULL, NULL);
   seta_dtela(&di[3], cw->fonetica,"xxx", M_INVERTE, 4, 45,  0, E_LISTA, H_CPFONET, op_sn, NULL, NULL);
   seta_dtela(&di[4], cw->help,    "xxxxxxxx", M_HELPCP, 5, 30,  0, E_ALFA, H_HELPCP, NULL, NULL, NULL);
   seta_dtela(&di[5], cw->param.tab_auto.autonum, "xxxxxxxxxxxxxxxxxxxx", M_AUTONUM, 6, 30,  0, E_ALFA, H_AUTONUM, NULL, NULL, NULL);
   seta_dtela(&di[6], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);

   ji = abre_janela(J_DEFCARCP);

	fw_title(ji, T_CPIDX, ji->w_atrib);
	fwbox(ji);

	mvwprintw(ji ,  1,  1, D_CPIDX1);
   mvwprintw(ji ,  2,  1, D_CPIDX2);
   mvwprintw(ji ,  3,  1, D_CPIDX3);
   mvwprintw(ji ,  4,  1, D_CPIDX4);
   mvwprintw(ji ,  5,  1, D_CPIDX5);
   mvwprintw(ji ,  6,  1, D_CPIDX6);

	ed_read(ji, &di[0], 0, SO_MOSTRA, str_nada);

   campo = 1;

le:

	ret = ed_read(ji, &di[0], campo, MOSTRA_LE, LF123E);

	switch (ret) {
      case K_ESC  : ret = 0; goto fim;
      case K_F2   : goto fim;
		case K_UP   :
		case K_PGUP : campo =  1; goto le;
		case K_DN   :
		case K_PGDN : campo = (campo >= IND_DI) ? 1 : IND_DI;
                    goto le;
		case K_ENTER:
      default     : ret = 0;   break;
	}

fim :

   fwkill(ji);
   return(ret);

}




