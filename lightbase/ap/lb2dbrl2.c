#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2entra.h"
#include  "lb2files.h"
#include  "lb2locs.h"
#include  "lb2prot1.h"
#include  "lb2vet.h"

#include  "lb2db.h"
#include  "lb2vista.h"

#ifdef    VAXVMS
#include  "tabela.h"
#include  "menu.h"
#else
#include  "../mt/tabela.h"
#include  "../mt/menu.h"
#endif

#define   NUM_ITENS  40
#define   GRP_COLUNA    24


static WIN *j2;
static COUNT f2_ativa;

extern SC *tab_ultimo;


#ifdef MSDOS
   COUNT    dsp_coluna(Tab *);
   COUNT    exec_coluna(Tab *);
   COUNT    tab_coluna( COUNT, COUNT, TEXT *);
   VOID     seta_rd(COUNT, COUNT);
   COUNT    careg_itens(VOID);
   COUNT    vl_visib(d_entry *);
   VOID     just_itens(TEXT *);
   COUNT    pr_numero(d_entry *);
#else
   COUNT    dsp_coluna( );
   COUNT    exec_coluna( );
   COUNT    tab_coluna( );
   VOID     seta_rd( );
   COUNT    careg_itens( );
   COUNT    vl_visib( );
   VOID     just_itens( );
   COUNT    pr_numero( );
#endif


#define   H_TIT_COL     "TIT_COL"
#define   H_VIS_COL     "VIS_COL"
#define   H_TOT_COL     "TOT_COL"
#define   H_QTD_COL     "QTD_COL"
#define   H_MED_COL     "MED_COL"
#define   H_DES_COL     "DES_COL"
#define   H_CFEXCOL     "CF_EXCOL"
#define   H_NMCPIN      "CP_NMINV"      /* Nome de campo invalido */
#define   H_COLNE       "E_COL_NE"
#define   H_COLMAX      "E_COL_MA"
#define   H_COLVIS      "E_COL_VI"
#define   H_COLVTX      "E_COL_VT"

#define   C_COL_CAB        loc_str(GRP_COLUNA, 1 )
#define   C_COL_ICOL       loc_str(GRP_COLUNA, 2 )

#define   M_COL_INS        loc_str(GRP_COLUNA, 3 )
#define   M_COL_ALT        loc_str(GRP_COLUNA, 4 )
#define   M_COL_SEL        loc_str(GRP_COLUNA, 5 )

#define   C_COL_1T         loc_str(GRP_COLUNA, 6 )
#define   C_COL_2T         loc_str(GRP_COLUNA, 7 )
#define   C_COL_3T         loc_str(GRP_COLUNA, 8 )
#define   C_COL_4T         loc_str(GRP_COLUNA, 9 )
#define   C_COL_5T         loc_str(GRP_COLUNA, 10)
#define   C_COL_6T         loc_str(GRP_COLUNA, 11)

#define   C_COL_1R         loc_str(GRP_COLUNA, 12)
#define   C_COL_2R         loc_str(GRP_COLUNA, 13)
#define   C_COL_3R         loc_str(GRP_COLUNA, 14)
#define   C_COL_4R         loc_str(GRP_COLUNA, 15)
#define   C_COL_5R         loc_str(GRP_COLUNA, 16)
#define   C_COL_6R         loc_str(GRP_COLUNA, 17)

#define   TW_COL1          loc_str(GRP_COLUNA, 18)

#define   E_COL_NE         loc_str(GRP_COLUNA, 19)
#define   E_COL_MAX        loc_str(GRP_COLUNA, 20)
#define   E_COL_VIS        loc_str(GRP_COLUNA, 21)
#define   E_COL_VTX        loc_str(GRP_COLUNA, 22)


typedef struct {
   TEXT campo[TE+1];
   TEXT titulo[TD+1];
   TEXT visibilidade[4];
   TEXT total[5];
   TEXT quant[5];
   TEXT media[5];
   TEXT desvio[5];
   COUNT ocupa;
   COUNT num_campo;
   COUNT pos;
} Com_item;

static Com_item **vtitens = NULL;
static Com_item *base_vet = NULL;
static COUNT mudou_colunado, n_itens, max_colunas;
static Campo *pc_db5 = NULL;
static DTela *p_dcol;


F__GLB   COUNT make_colunado(foi_modificado, colunas, flag)
COUNT *foi_modificado;
COUNT colunas;
COUNT flag;
{	COUNT ret;
   Tab   t;
	UTEXT str_escape[20];
   TEXT *ajuda_ant = win_ajuda;
	TEXT  lin_fun[100], lin_alt[100], tec_fun[15];
   WIN   *jm = NULL, *jf = NULL, *ji = NULL;
   DTela dcol[7];


   up_str(GRP_COLUNA);

   win_ajuda = "DEF_COL";

   j2             = NULL;
   ret            = 0;
   mudou_colunado = 0;
   max_colunas    = colunas;
   p_dcol         = &dcol[0];

   seta_dtela(&dcol[0], NULL, x40,   C_COL_1R,  1, 15, 37, E_ALFA,  H_TIT_COL, NULL, NULL, NULL );
   seta_dtela(&dcol[1], NULL, "999", C_COL_2R,  3, 15,  0, E_VALOR, H_VIS_COL, NULL, NULL, vl_visib);
   seta_dtela(&dcol[2], NULL, "xxx", C_COL_3R,  5, 15,  0, E_LISTA, H_TOT_COL, op_sn, pr_numero, NULL );
   seta_dtela(&dcol[3], NULL, "xxx", C_COL_4R,  7, 15,  0, E_LISTA, H_QTD_COL, op_sn, pr_numero, NULL );
   seta_dtela(&dcol[4], NULL, "xxx", C_COL_5R,  9, 15,  0, E_LISTA, H_MED_COL, op_sn, pr_numero, NULL );
   seta_dtela(&dcol[5], NULL, "xxx", C_COL_6R, 11, 15,  0, E_LISTA, H_DES_COL, op_sn, pr_numero, NULL );
   seta_dtela(&dcol[6], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);


   if ( flag == 'F' )  /* so libera area */
      goto fim;


   vtitens = NULL;
   base_vet = NULL;


   if ( (vtitens = calloc(NUM_ITENS + 2, sizeof(Com_item *))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto fim;
   }

   if ( ( base_vet = calloc(NUM_ITENS + 2, sizeof(Com_item))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto fim;
   }


   j2 = abre_janela(J_COLUNADO);
	fw_title(j2, C_COL_CAB, j2->w_atrib);
	fwbox(j2);

	strcpy(tec_fun, "1 2 3 6 0 IDE");

   n_itens = careg_itens();

   sel_campo(BASE_EDIT, NULL, 1 ); /* 1 so resetar ultimo campo */

	def_tabela(&t, 3, 8, 25, 13, n_itens, NUM_ITENS, X14,
				   MS_CAMPO, MS_BARRA, SIMPLES, MS_JANELA, MS_DESTAQUE,
				   ( char *(* )[]) vtitens, str_vazia ,  str_vazia );

	pr_escape(tec_fun, str_escape);

	strncpy(lin_fun, funcao("1 2 3 0 E"), 99);
	strncpy(lin_alt, funcao("1 2 3 0 E"), 99);

   t.mens_ins  = M_COL_INS;
   t.mens_alt  = M_COL_ALT;
	t.fun_ins   = lin_fun;
	t.fun_alt   = lin_alt;
   t.escape    = str_escape;
   t.comando   = dsp_coluna;
	t.executa   = exec_coluna;
   t.edita     = INSERE | ALTERA | DELETA ;


	mvwprintw(j2 ,  1,  1, C_COL_1T);
	mvwprintw(j2 ,  3,  1, C_COL_2T);
   mvwprintw(j2 ,  5,  1, C_COL_3T);
	mvwprintw(j2 ,  7,  1, C_COL_4T);
   mvwprintw(j2 ,  9,  1, C_COL_5T);
   mvwprintw(j2 , 11,  1, C_COL_6T);


   jm = abre_mensagem( M_COL_SEL );
	jf = abre_funcoes(funcao(tec_fun));
   ji = abre_insercol( C_COL_ICOL);

   tabela(&t);

   ret = t.tecla;

   fecha_insercol(ji);
	fecha_funcoes(jf);
	fecha_mensagem(jm);

fim :

   if ( mudou_colunado )
      *foi_modificado = 1;

	if ( j2 != NULL ) fwkill(j2);

   if ( flag == 'F' ) {
      if ( vtitens != NULL ) {
         if ( base_vet != NULL ) free(base_vet);
         free(vtitens);
      }
      vtitens = NULL;
      base_vet = NULL;
   }

   win_ajuda = ajuda_ant;
   down_str(GRP_COLUNA);

   if ( ret == K_ESC && *foi_modificado && flag != 'F' )
      ret = 1;  /* ESC com alteracao */

   return(ret);
}


F__LOC   COUNT dsp_coluna(tp)
Tab *tp;
{  COUNT ret;

   ret = -2;

   if ( w_kbhit(j2) ) goto fim;

	if ( tp->escolhido != (tp->n_elem - 1)) {
      seta_rd(tp->escolhido, 0);
		ed_read(j2, p_dcol, 0, SO_MOSTRA, str_vazia);
	}
	else {
      seta_rd(tp->escolhido, 'Z');
		ed_read(j2, p_dcol, 0, SO_LIMPA, str_vazia);
	}

fim:

   fwsetcursor( tp->win );
   return ( ret );
}




F__LOC   COUNT exec_coluna(tp)
Tab *tp;
{  COUNT ret;
   TEXT *nome_ret;

   fwtop( j2 );

   f2_ativa = 0;

   if ( tp->tecla == K_F0 ) {
      if ( (nome_ret = sel_campo(BASE_EDIT, NULL, 0) ) != NULL ) {
         w_ungs(nome_ret);
         w_ungc(0X19); /* CTRL Y */
      	if ( tp->escolhido == (tp->n_elem - 1) &&
              (*tp->opcoes)[tp->escolhido] == tab_ultimo )
            w_ungc(K_HOME);
         else
            w_ungc(K_F6);
      }
      ret = tp->escolhido;
      goto fim;
   }

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
		case K_DEL  :  /* Apagar */
                     ret = tab_coluna('D', tp->escolhido, str_vazia);
                     break;

		case K_F6   :  /* Alterar o nome do campo */
							ret = tab_coluna('A', tp->escolhido, tp->e->str);
							if ( ret == -2 ) tp->escolhido++;
                     break;

      case K_INS  :  /* Incluir um novo campo */
							ret = tab_coluna('I', tp->escolhido, tp->e->str);
							if ( ret == -2 ) tp->escolhido++;
                     break;

		case K_F2  :   /* Gravar registro (atualizar base) */
      case K_ESC :   /* Abandonar edicao */
                     ret = -3; /* finaliza edicao */
						   break;

      case K_ENTER :
		default      :  /* Editar caracteristicas do campo */
							ret = tab_coluna('A', tp->escolhido, NULL);
							if (ret == -2 ) tp->escolhido++; /* ANANIAS @ ??? */
                     break;
   }

   if ( f2_ativa ) {
		tp->tecla = K_F2;
      ret = -3; /* finaliza edicao */
   }


fim :

   fwtop( tp->win );

   fwsetcursor( tp->win );
   return(ret);
};




F__LOC   COUNT tab_coluna(flag, ind_vet, nome_campo)
COUNT flag;
COUNT ind_vet;
TEXT *nome_campo;
{  COUNT ret, campo, ind, j, i, ocupado;

   ret = ind_vet;

   if ( flag == 'D' ) {
      ret = decida(str_vazia, OP_SIM_NAO, H_CFEXCOL, 1, 34, "%s", TW_COL1 );

      if ( ret == 0 ) ret = -2;
      else            ret = 0;

      goto fim;
   }

   if ( nome_campo != NULL ) {
	   if ( val_indice(nome_campo) != OK) {
		   mens_erro(H_NMCPIN, E_NOMEIN);
		   ret = ind_vet;
         goto fim;
      }

      ind = ver_campo(BASE_EDIT, nome_campo, &j);

      if ( ind == -1 ) { /* campo nao existe */
         mens_erro(H_COLNE, E_COL_NE);
         ret = ind_vet;
         goto fim;
      }
      else
         pc_db5 = pos_vetcmp(BASE_EDIT)[ind];
   }
   else {
      ind = acha_campo(BASE_EDIT, vtitens[ind_vet]->num_campo );
      pc_db5 = pos_vetcmp(BASE_EDIT)[ind];
   }


   if ( flag == 'I' ) {
      for ( i = NUM_ITENS ; i > ind_vet; i--) {
         base_vet[i] = base_vet[i-1];
         vtitens[i] = &base_vet[i];
      }
      n_itens++;

   	vtitens[ind_vet] = &base_vet[ind_vet];
   }

   seta_rd(ind_vet, flag == 'I' ? 'Z' : 0);

   if ( vtitens[ind_vet]->visibilidade[0] == '\0' ) {
      sprintf(vtitens[ind_vet]->visibilidade, "%d",  pc_db5->cp_mhor);
   }

   campo = 1;

le:

	ret = ed_read(j2, p_dcol, campo, MOSTRA_LE, "1 2 3 0 E");

   campo = w_lastread;

	switch (ret) {
      case K_ESC  :  ret = -1;   goto fim;
      case K_F0   :  ret = carac_rel( );
                     if ( ret >= 0 ) {
                        max_colunas = ap_rel.rel_colunas;
                        mudou_colunado = 1;
                     }
                     goto le;
		case K_UP   :
		case K_PGUP :  campo =  1; goto le;
      case K_F2   :  f2_ativa = 1;
		case K_ENTER:
      default     : ret = -2;   break;
	}

   i = strlen(vtitens[ind_vet]->titulo);
   j = atoi(vtitens[ind_vet]->visibilidade);
   vtitens[ind_vet]->ocupa = MAX(i, j ) + 1;

   for (i=0, ocupado = 0; i < n_itens; i++)
      ocupado += vtitens[i]->ocupa;


   if ( (ocupado - 1 ) > max_colunas ) {
      ocupado -= vtitens[ind_vet]->ocupa;
      j = max_colunas - ocupado;
		mens_erro(H_COLMAX, E_COL_MAX, abs(j));
		ret = -1;
      goto fim;
   }

   vtitens[ind_vet]->num_campo = pc_db5->cp_num;

   if ( nome_campo != NULL )
      strcpy((TEXT *)vtitens[ind_vet], nome_campo);

fim :

   pc_db5 = NULL; /* para pr_numero nao tratar */

   if ( (ret == -1 && flag == 'I') || (flag == 'D' && ret == -2) ) {
      for ( i = ind_vet; i <= NUM_ITENS; i++) {
         base_vet[i] = base_vet[i+1];
         vtitens[i+1] = &base_vet[i];
      }
      seta_rd(n_itens, 'Z');
      strcpy((TEXT *)vtitens[n_itens], str_vazia);
      n_itens--;
   }

   if ( ret == -1 )
      ret = ind_vet;

   if ( ret == -2 )
      mudou_colunado = 1;

   return (ret);
}


F__LOC   VOID seta_rd(ind, flag)
COUNT ind, flag;
{  Com_item *p;


   /****  p = (Com_item *)( (TEXT *)base_vet + (ind * sizeof(Com_item))); */
   p = &base_vet[ind];

   p_dcol[0].destino = p->titulo;
   p_dcol[1].destino = p->visibilidade;
   p_dcol[2].destino = p->total;
   p_dcol[3].destino = p->quant;
   p_dcol[4].destino = p->media;
   p_dcol[5].destino = p->desvio;
   p_dcol[6].destino = NULL;

   if ( flag == 'Z' ) {
      p_dcol[0].destino[0] = '\0';
      p_dcol[1].destino[0] = '\0';
      p_dcol[2].destino[0] = '\0';
      p_dcol[3].destino[0] = '\0';
      p_dcol[4].destino[0] = '\0';
      p_dcol[5].destino[0] = '\0';
   }
}



F__LOC   COUNT careg_itens()
{  COUNT itens, i, ib, ind, x, y;
   TEXT *pd;
   TEXT *aux;
   TEXT inds[120]; /* indicadores de pieces validos */
   Campo **vetcp;
	RTela *pi;


   if ( (aux = malloc(300)) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      itens = 0;
      goto fim;
   }

   vetcp    = pos_vetcmp(BASE_EDIT);

   itens = 0;
   ib = 0;
   pd = str_vazia;

   for ( i = 0;   (pi = pos_itrel(i)) != NULL;   i++ ) {

      if ( pi->re_tipo != 'D' ) {

         ind = acha_campo( pi->re_icb, pi->re_num );

         if ( ind < 0 )
            continue;

         inds[itens] = i;
         itens++;

         strcpy(base_vet[ib].campo, vetcp[ ind ]->cp_nome );

         sprintf(base_vet[ib].visibilidade, "%d",  pi->re_vhor);

         base_vet[ib].num_campo = pi->re_num;

         /* base_vet[ib].ocupa vai ser calculado no desmonte do display */

         strcpy(base_vet[ib].total , (pi->re_tot    == 'S') ? OP_SIM : OP_NAO);
         strcpy(base_vet[ib].quant , (pi->re_quant  == 'S') ? OP_SIM : OP_NAO);
         strcpy(base_vet[ib].media , (pi->re_media  == 'S') ? OP_SIM : OP_NAO);
         strcpy(base_vet[ib].desvio, (pi->re_desvio == 'S') ? OP_SIM : OP_NAO);

         ib++;
      }
      else {
      	pd = (TEXT *) pi + sizeof(RTela);
      }
   }

	for ( i=0; i < itens; i++ ) {
      piece(pd, aux, UG_SP, inds[i], 299);
      trim(aux);
      ltrim(aux);
      x = strlen(aux);
      y = atoi(base_vet[i].visibilidade);
      strcpy(base_vet[i].titulo, aux);
      base_vet[i].ocupa = MAX(x, y ) + 1;
   	vtitens[i] = (Com_item *)&base_vet[i];
      vtitens[i+1] = (Com_item *)NULL;
   }

   free(aux);

fim :

   return(itens);
}



F__GLB   COUNT prep_f2_colunado()
{  COUNT ret, i, tam;
   COUNT item=0;
   TEXT *pd;
	RTela *pi;
   Vet_itens *v;

   v = ap_rel.rel_vet;

   /**/
   /** libera itens antigos */
   /**/

   while (v->vi_nelem > 0 ) {
      pos_item(v, v->vi_nelem - 1);
      del_item(v, 1);
   };

   if ( n_itens == 0 ) {
      ret = 0;
      goto fim;
   }

   if ( (pi = (RTela *)calloc(1, sizeof(RTela) + max_colunas + 3)) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      ret = -1;
      goto fim;
   }


   /**/
   /** monta linha de cabecalho */
   /**/

   pd = (TEXT *) pi + sizeof(RTela);       /* inicio da descricao */

   just_itens(pd);

   tam = sizeof(RTela) + strlen(pd) + 1;

   pi->re_tela    = 1;
   pi->re_tipo    = 'D';
	pi->re_num     = 0;
	pi->re_ivetcp  = -1;
   pi->re_base[0] = '\0'; /* nome de base NULL significa a corrente */
   pi->re_linha   = 0;
   pi->re_coluna  = 0;

   if ( add_item(v, item++, pi, tam, 1) == NULL )  {
      mens_erro(H_MXITEM, E_MXITEM, item - 1);
      ret = -1;
      goto fim;
   }


   /**/
   /** monta resto dos campos */
   /**/

   tam = sizeof(RTela) + 1;
   *pd = '\0';  /* string vazia */

   for (i=0; i < n_itens; i++) {

      pi->re_tela    = 1;
      pi->re_tipo    = 'S';
		pi->re_num     = base_vet[i].num_campo;
   	pi->re_ivetcp  = -1;
      pi->re_base[0] = '\0'; /* nome de base NULL significa a corrente */
      pi->re_linha   = 0;
      pi->re_coluna  = base_vet[i].pos;
      pi->re_vver    = 1;
      pi->re_vhor    = atoi(base_vet[i].visibilidade);
      pi->re_tot     = ( strcmp( base_vet[i].total , OP_SIM) == 0 ) ? 'S' : 'N';
      pi->re_quant   = ( strcmp( base_vet[i].quant , OP_SIM) == 0 ) ? 'S' : 'N';
      pi->re_media   = ( strcmp( base_vet[i].media , OP_SIM) == 0 ) ? 'S' : 'N';
      pi->re_desvio  = ( strcmp( base_vet[i].desvio, OP_SIM) == 0 ) ? 'S' : 'N';

      if ( add_item(v, item++, pi, tam, 1) == NULL ) {
         mens_erro(H_MXITEM, E_MXITEM, item - 1);
         ret = -1;
         goto fim;
      }

   }

   ret = 0;

fim :

   return(ret);
}



F__LOC   COUNT vl_visib(entry)
d_entry *entry;
{  COUNT ret;
	COUNT tecla;

	tecla = entry->tecla;
	ret = ACEITA;

	if ( tecla == K_ESC || tecla == K_UP ) {
      ret = ACEITA;
	}
   else if ( atoi(entry->str) > pc_db5->cp_mhor || atoi(entry->str) < 1 ) {
      mens_erro(H_COLVIS, E_COL_VIS, pc_db5->cp_mhor);
      ret = REJEITA;
   }

   else if ( pc_db5->cp_tipo == 'T' &&
             (atoi(entry->str) < MIN(15, pc_db5->cp_mhor)) ) {
      mens_erro(H_COLVTX, E_COL_VTX);
      ret = REJEITA;
   }


   return(ret);
}

F__LOC   COUNT pr_numero(entry)
d_entry *entry;
{  COUNT ret;

   ret = 0;

   if ( strcmp(entry->str, OP_SIM) != 0 ) {
      if ( pc_db5 == NULL ||
         (pc_db5->cp_tipo != 'N' &&  pc_db5->cp_tipo != 'F')) {
         strcpy(entry->str, OP_NAO);
         ret = 1;
      }
      else if ( entry->str[0] == '\0') {
         strcpy(entry->str, OP_NAO);
      }
   }

   return(ret);
}



F__LOC   VOID just_itens(str)
TEXT *str;
{  COUNT i, ocupado, pos, espaco, tam;

   for (i=0, ocupado = 0; i < n_itens; i++)
      ocupado += base_vet[i].ocupa;

   ocupado--;

   espaco = ( max_colunas - ocupado ) / (n_itens + 1);

   memset(str, ' ', max_colunas);

   pos = 0;

   for (i=0; i < n_itens; i++) {
      pos += espaco;
      base_vet[i].pos = pos;
      tam = strlen(base_vet[i].titulo);
      strncpy(&str[pos], base_vet[i].titulo, tam);
      /* str[pos + tam] = UG_SP;    V  */
      pos += base_vet[i].ocupa;
      str[pos-1] = UG_SP;  /******  ^  */
   }

   str[pos]='\0';
}
