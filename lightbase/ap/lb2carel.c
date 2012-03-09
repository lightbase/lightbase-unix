#include  "lb2all.h"

#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2entra.h"
#include  "lb2locs.h"
#include  "lb2prot1.h"

#include  "lb2db.h"



#define   GRP_CAREL  23

#define   x3 "xxx"
#define   n3 "999"
#define   nx "99xxxxxxx"
#define   TIPO_REL         ap_rel.rel_tipo

#define   LC1  11
#define   LC2  12
#define   LC3  13

#define   LC1_ETQ  9
#define   LC2_ETQ  10
#define   LC3_ETQ  11



#define   H_CABEC	      "RL_CABEC"      /* deve imprimir cabecalho  */
#define   H_TIT	      "RL_TIT"        /*   */
#define   H_RODA	      "RL_RODA"       /*   */
#define   H_LIN	      "RL_LIN"        /*   */
#define   H_COL	      "RL_COL"        /*   */
#define   H_CARAC	      "RL_CARAC"      /*   */
#define   H_QUAL	      "RL_QUAL"       /*   */
#define   H_FOL	      "RL_FOL"        /*   */
#define   H_EDITI       "RL_EDITI"      /* Edicao de instrucoes especiais */
#define   H_CAMPO	      "RL_CAMPO"      /*   */
#define   H_ORDEM	      "RL_ORDEM"      /*   */
#define   H_LINQ	      "RL_LINQ"       /*   */
#define   H_TOT	      "RL_TOT"        /*   */


#define   H_ETQLI       "RL_ETQLIN"     /*   */
#define   H_ETQCO       "RL_ETQCOL"     /*   */
#define   H_ELETQ	      "RL_ELETQ"      /*   */
#define   H_ECETQ	      "RL_ECETQ"      /*   */
#define   H_NETQ	      "RL_NETQ"       /*   */

#define   H_LINREL      "E_LINREL"      /* numero de linhas de 2 a 99 */
#define   H_LINUT       "E_RELUTE"      /* numero de linhas uteis */
#define   H_NODIM       "E_NODIM"       /* numero de linhas nao pode ser diminuido */
#define   H_TAMREL      "E_TAMREL"      /* Tamanho de relatório esta fora do intervalo 20 a 250 */
#define   H_ORDTXT      "E_ORDTXT"      /* Campo texto nao pode ser usado para ordenar */
#define   H_CMDESP      "CMDS_REL"      /* Comando especiais validos no Relatorio */

#define   C_CAB_DB5        loc_str(GRP_CAREL, 1 )
#define   C_TIT_DB5        loc_str(GRP_CAREL, 2 )
#define   C_ROD_DB5        loc_str(GRP_CAREL, 3 )
#define   C_VERT_DB5       loc_str(GRP_CAREL, 4 )
#define   C_HOR_DB5        loc_str(GRP_CAREL, 5 )
#define   C_CARAC_DB5      loc_str(GRP_CAREL, 6 )
#define   C_QUAL_DB5       loc_str(GRP_CAREL, 7 )
#define   C_FOLHA_DB5      loc_str(GRP_CAREL, 8 )
#define   C_EDIT_DB5       loc_str(GRP_CAREL, 9 )
#define   C_ORD1_DB5       loc_str(GRP_CAREL, 10)
#define   C_ORD2_DB5       loc_str(GRP_CAREL, 11)

#define   C_DB5_LST        loc_str(GRP_CAREL, 12)
#define   R_TIT_DB5        loc_str(GRP_CAREL, 13)
#define   R_RODAPE_DB5     loc_str(GRP_CAREL, 14)
#define   R_LINHAS_DB5     loc_str(GRP_CAREL, 15)
#define   R_CAMPOC_DB5     loc_str(GRP_CAREL, 16)
#define   R_LINHASC_DB5    loc_str(GRP_CAREL, 17)

#define   C_ETQ_TLIN       loc_str(GRP_CAREL, 18)
#define   C_ETQ_TCOL       loc_str(GRP_CAREL, 19)
#define   C_ETQ_XLIN       loc_str(GRP_CAREL, 20)
#define   C_ETQ_XCOL       loc_str(GRP_CAREL, 21)
#define   C_ETQ_NUM        loc_str(GRP_CAREL, 22)
#define   C_ETQ_1ORD       loc_str(GRP_CAREL, 23)
#define   C_ETQ_2ORD       loc_str(GRP_CAREL, 24)

#define   R_ETQ_LIN        loc_str(GRP_CAREL, 25)
#define   R_ETQ_COL        loc_str(GRP_CAREL, 26)
#define   R_ETQ_ELIN       loc_str(GRP_CAREL, 27)
#define   R_ETQ_ECOL       loc_str(GRP_CAREL, 28)
#define   R_ETQ_NUM        loc_str(GRP_CAREL, 29)

#define   C_CARAC_REL      loc_str(GRP_CAREL, 30)


#define   E_LINHAS   loc_str(GRP_CAREL, 31)
#define   E_RELTCR   loc_str(GRP_CAREL, 32)
#define   E_NODIM    loc_str(GRP_CAREL, 33)
#define   E_TAMREL   loc_str(GRP_CAREL, 34)
#define   E_ORDTXT   loc_str(GRP_CAREL, 35)

#define   OP_CARACTER  loc_str(GRP_CAREL, 36)
#define   OP_QUALIDADE loc_str(GRP_CAREL, 37)
#define   OP_ORDEM     loc_str(GRP_CAREL, 38)
#define   OP_TIPOREL   loc_str(GRP_CAREL, 39)

#define   T_EDITINST   loc_str(GRP_CAREL, 40)
#define   M_EDITINST   loc_str(GRP_CAREL, 41)
#define   M_FIMEDIT    loc_str(GRP_CAREL, 42)

#define   C_ETQ_ESP    loc_str(GRP_CAREL, 43)

#define   R_COLUNAS_DB5    loc_str(GRP_CAREL, 44)

#ifdef MSDOS
   COUNT  l_uteis(TEXT *);
   COUNT pr_lc(d_entry *);
   COUNT vl_linhas(d_entry *);
   COUNT vl_colunas(d_entry *);
   COUNT vl_editdoc(d_entry *);
   COUNT pr_ordem(d_entry *);
   COUNT pr_nlinhas(d_entry *);
   COUNT pr_total(d_entry *);
   COUNT pr_campos(d_entry *);
   COUNT vl_cppsq(d_entry *);
   VOID  ld_re(Rel *);
   VOID  seta_re(Rel *);
   VOID  zera_re(VOID);
#else
   COUNT  l_uteis( );
   COUNT pr_lc( );
   COUNT vl_linhas( );
   COUNT vl_colunas( );
   COUNT vl_editdoc( );
   COUNT pr_ordem( );
   COUNT pr_nlinhas( );
   COUNT pr_total( );
   COUNT pr_campos( );
   COUNT vl_cppsq( );
   VOID  ld_re( );
   VOID  seta_re( );
   VOID  zera_re( );
#endif



static Rw    *rw;
static Rel   *rl_cor;



F__GLB   COUNT carac_rel( )
{  COUNT ret, campo;
   TEXT *ajuda_ant = win_ajuda;
   WIN  *j2;
   DTela dr[22];
   win_ajuda = "DEF_RELC";


   up_str(GRP_CAREL);

   rw = (Rw *) calloc(1, sizeof(Rw));

   rl_cor = &ap_rel;

   if ( rl_cor->rel_linhas == 0 )  /* propoe padrao */
      zera_re();
   else
      ld_re(rl_cor);


   if ( TIPO_REL != 'E' ) {

      j2 = abre_janela(J_CARAC1REL);

      mvwprintw(j2 ,  0,  1, C_CAB_DB5  );
      mvwprintw(j2 ,  1,  1, C_TIT_DB5  );
      mvwprintw(j2 ,  2,  1, C_ROD_DB5  );
      mvwprintw(j2 ,  3,  1, C_VERT_DB5 );
      mvwprintw(j2 ,  4,  1, C_HOR_DB5  );
      mvwprintw(j2 ,  5,  1, C_CARAC_DB5);
      mvwprintw(j2 ,  6,  1, C_QUAL_DB5 );
      mvwprintw(j2 ,  7,  1, C_FOLHA_DB5);
      mvwprintw(j2 ,  8,  1, C_EDIT_DB5 );
      mvwprintw(j2 ,  9,  1, C_ORD1_DB5 );
      mvwprintw(j2 , 10,  1, C_ORD2_DB5 );

      seta_dtela(&dr[ 0], rw->r_cabec,     x3, C_DB5_LST,        0,  36, 37, E_LISTA, H_CABEC, op_sn, NULL,       NULL );
      seta_dtela(&dr[ 1], rw->r_titulo,   x50, R_TIT_DB5,        1,  36, 37, E_ALFA,  H_TIT	, NULL,       NULL,       NULL );
      seta_dtela(&dr[ 2], rw->r_rodape,   x50, R_RODAPE_DB5,     2,  36, 37, E_ALFA,  H_RODA	, NULL,       NULL,       NULL );
      seta_dtela(&dr[ 3], rw->r_linhas,    nx, R_LINHAS_DB5,     3,  36,  0, E_ALFA,  H_LIN	, NULL,       pr_lc,      vl_linhas );
      seta_dtela(&dr[ 4], rw->r_colunas,   n3, R_COLUNAS_DB5,    4,  36,  0, E_ALFA,  H_COL	, NULL,       pr_lc,      vl_colunas );
      seta_dtela(&dr[ 5], rw->r_caracter, X14, C_DB5_LST,        5,  36,  0, E_LISTA, H_CARAC, OP_CARACTER, NULL,      NULL );
      seta_dtela(&dr[ 6], rw->r_qualid,   X14, C_DB5_LST,        6,  36,  0, E_LISTA, H_QUAL	, OP_QUALIDADE, NULL,     NULL );
      seta_dtela(&dr[ 7], rw->r_folha,     x3, C_DB5_LST,        7,  36,  0, E_LISTA, H_FOL	, op_sn, NULL,       NULL );
      seta_dtela(&dr[ 8], rw->r_editdoc,   x3, C_DB5_LST,        8,  36,  0, E_LISTA, H_EDITI, op_sn, NULL,  vl_editdoc );
      seta_dtela(&dr[ 9], rw->r_1campo,   X14, R_CAMPOC_DB5,   LC1,   3,  0, E_ALFA,  H_CAMPO, op_sn, pr_campos,  vl_cppsq );
      seta_dtela(&dr[10], rw->r_1ord,     X14, C_DB5_LST,      LC1,  22,  0, E_LISTA, H_ORDEM, OP_ORDEM,   pr_ordem,   NULL );
      seta_dtela(&dr[11], rw->r_1linhas, "99", R_LINHASC_DB5,  LC1,  50,  0, E_VALOR, H_LINQ	, NULL,       pr_nlinhas, NULL );
      seta_dtela(&dr[12], rw->r_1total,    x3, C_DB5_LST,      LC1,  66,  0, E_LISTA, H_TOT	, op_sn, pr_total,   NULL );
      seta_dtela(&dr[13], rw->r_2campo,   X14, R_CAMPOC_DB5,   LC2,   3,  0, E_ALFA,  H_CAMPO, op_sn, pr_campos,  vl_cppsq );
      seta_dtela(&dr[14], rw->r_2ord,     X14, C_DB5_LST,      LC2,  22,  0, E_LISTA, H_ORDEM, OP_ORDEM,   pr_ordem,   NULL );
      seta_dtela(&dr[15], rw->r_2linhas, "99", R_LINHASC_DB5,  LC2,  50,  0, E_VALOR, H_LINQ	, NULL,       pr_nlinhas, NULL );
      seta_dtela(&dr[16], rw->r_2total,    x3, C_DB5_LST,      LC2,  66,  0, E_LISTA, H_TOT	, op_sn, pr_total,   NULL );
      seta_dtela(&dr[17], rw->r_3campo,   X14, R_CAMPOC_DB5,   LC3,   3,  0, E_ALFA,  H_CAMPO, op_sn, pr_campos,  vl_cppsq );
      seta_dtela(&dr[18], rw->r_3ord,     X14, C_DB5_LST,      LC3,  22,  0, E_LISTA, H_ORDEM, OP_ORDEM,   pr_ordem,   NULL );
      seta_dtela(&dr[19], rw->r_3linhas, "99", R_LINHASC_DB5,  LC3,  50,  0, E_VALOR, H_LINQ	, NULL,       pr_nlinhas, NULL );
      seta_dtela(&dr[20], rw->r_3total,    x3, C_DB5_LST,      LC3,  66,  0, E_LISTA, H_TOT	, op_sn, pr_total,   NULL );
      seta_dtela(&dr[21], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
   }
   else {

      j2 = abre_janela(J_CARAC2REL);

      mvwprintw(j2 ,  1,  1, C_ETQ_TLIN );
      mvwprintw(j2 ,  2,  1, C_ETQ_TCOL );
      mvwprintw(j2 ,  3,  1, C_ETQ_XLIN );
      mvwprintw(j2 ,  4,  1, C_ETQ_XCOL );
      mvwprintw(j2 ,  5,  1, C_ETQ_NUM  );
      mvwprintw(j2 ,  6,  1, C_ETQ_ESP  );

      mvwprintw(j2 ,  7,  1, C_ETQ_1ORD );
      mvwprintw(j2 ,  8,  1, C_ETQ_2ORD );

      seta_dtela(&dr[ 0], rw->r_linhas,  "99", R_ETQ_LIN ,       1, 42,  0, E_ALFA,  H_ETQLI, NULL,       pr_lc,      vl_linhas );
      seta_dtela(&dr[ 1], rw->r_colunas, "99", R_ETQ_COL ,       2, 42,  0, E_ALFA,  H_ETQCO, NULL,       pr_lc,      vl_colunas );
      seta_dtela(&dr[ 2], rw->r_elin,    "99", R_ETQ_ELIN,       3, 42,  0, E_ALFA,  H_ELETQ, NULL,      NULL,       NULL );
      seta_dtela(&dr[ 3], rw->r_ecol,    "99", R_ETQ_ECOL,       4, 42,  0, E_ALFA,  H_ECETQ, NULL,      NULL,       NULL );
      seta_dtela(&dr[ 4], rw->r_netq,    "99", R_ETQ_NUM,        5, 42,  0, E_ALFA,  H_NETQ, NULL,       NULL,       NULL );
      seta_dtela(&dr[ 5], rw->r_editdoc,   x3, C_DB5_LST,        6, 42,  0, E_LISTA, H_EDITI, op_sn,     NULL,  vl_editdoc );
      seta_dtela(&dr[ 6], rw->r_1campo,   X14, R_CAMPOC_DB5 , LC1_ETQ,  3,  0, E_ALFA,  H_CAMPO, op_sn, pr_campos,  vl_cppsq );
      seta_dtela(&dr[ 7], rw->r_1ord,     X14, C_DB5_LST , LC1_ETQ, 22,  0, E_LISTA, H_ORDEM, OP_ORDEM,   pr_ordem,   NULL );
      seta_dtela(&dr[ 8], rw->r_2campo,   X14, R_CAMPOC_DB5 , LC2_ETQ,  3,  0, E_ALFA,  H_CAMPO, op_sn, pr_campos,  vl_cppsq );
      seta_dtela(&dr[ 9], rw->r_2ord,     X14, C_DB5_LST , LC2_ETQ, 22,  0, E_LISTA, H_ORDEM, OP_ORDEM,   pr_ordem,   NULL );
      seta_dtela(&dr[10], rw->r_3campo,   X14, R_CAMPOC_DB5 , LC3_ETQ,  3,  0, E_ALFA,  H_CAMPO, op_sn, pr_campos,  vl_cppsq );
      seta_dtela(&dr[11], rw->r_3ord,     X14, C_DB5_LST , LC3_ETQ, 22,  0, E_LISTA, H_ORDEM, OP_ORDEM,   pr_ordem,   NULL );
      seta_dtela(&dr[12], NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
   }



	fw_title(j2, C_CARAC_REL, j2->w_atrib);
	fwbox(j2);
   fwrefresh(j2);

   campo = 1;

le:

   ret = ed_read(j2, &dr[0], campo, MOSTRA_LE, "1 3 0 E");

   campo = w_lastread;

	switch (ret) {
      case K_ESC  : ret = -1;   goto fim;
		case K_UP   :
		case K_PGUP :
      case K_F0   :
		case K_PGDN : goto le;

		case K_DN   :
		case K_ENTER:
      default     : ret = 0;   break;

      case K_F2   : numdebug(250);
                    break;
	}

   seta_re(rl_cor);

fim:

	fwkill(j2);

   if ( rw ) free(rw);

   win_ajuda = ajuda_ant;
   down_str(GRP_CAREL);
   return (ret);
}


F__LOC   COUNT l_uteis(str)
TEXT *str;
{	COUNT uteis, tam, tam_c, tam_r;
   TEXT aux[TE+1];

   if ( TIPO_REL == 'E' ) {
      uteis = atoi(str);
   }
   else {
      piece(str, aux, '-', 1, TE);  tam   = atoi(aux);
      piece(str, aux, '-', 2, TE);  tam_c = atoi(aux);
      piece(str, aux, '-', 3, TE);  tam_r = atoi(aux);
      uteis = tam - tam_c - tam_r;
   }

	return( uteis );
}



F__LOC   COUNT pr_lc(entry)
d_entry *entry;
{	COUNT ret=0;

   entry = entry;  /* so tira warning */

   if ( WCB != NULL  &&  c_bases[BASE_EDIT]->cb_tpopen == UTILIZACAO )
      ret = 1;  /* nao le */

   return( ret );
}



F__LOC	COUNT vl_linhas(entry)
d_entry *entry;
{	COUNT ret, tam, tam_c, tam_r, uteis;
   TEXT aux[TE+1];

   COUNT tecla;

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP || tecla == K_PGUP ) {
      ret = ACEITA;
      goto fim;
   }

   piece(entry->str, aux, '-', 1, TE);  tam   = atoi(aux);
   piece(entry->str, aux, '-', 2, TE);  tam_c = atoi(aux);
   piece(entry->str, aux, '-', 3, TE);  tam_r = atoi(aux);

   uteis = tam - tam_c - tam_r;

   if ( tam < 2 || tam > 99 ) {
      mens_erro(H_LINREL, E_LINHAS);
      ret = REJEITA;
      goto fim;
   }

   if ( uteis < 2 ) {
      mens_erro(H_LINUT, E_LINHAS);
      ret = REJEITA;
      goto fim;
   }
/*****
   if ( rl_cor->rel_ordem != 0  &&  rl_cor->rel_linhas > tam &&
        TIPO_REL == 'F' ) {
      mens_erro(H_NODIM, E_NODIM);
      ret = REJEITA;
      goto fim;
   }
*****/

   ret   = ACEITA;

fim:

	return( ret );
}


F__LOC	COUNT vl_colunas(entry)
d_entry *entry;
{	COUNT ret, tam;

   COUNT tecla;

	tecla = entry->tecla;

	if ( tecla == K_ESC || tecla == K_UP || tecla == K_PGUP ) {
      ret = ACEITA;
      goto fim;
   }

   tam = atoi(entry->str);

   if ( tam < 20 || tam > 250 ) {
      mens_erro(H_TAMREL, E_TAMREL);
      ret = REJEITA;
      goto fim;
   }

/*****
   if ( rl_cor->rel_ordem != 0  &&
        rl_cor->rel_colunas > tam ) {
      mens_erro(H_NODIM, E_NODIM);
      ret = REJEITA;
      goto fim;
   }
*****/

   ret   = ACEITA;

fim:

	return( ret );
}


F__LOC	COUNT vl_editdoc(entry)
d_entry *entry;
{	COUNT ret;
   WIN *win, *jm;
   UTEXT lin_escape[30];
   TEXT ajuda_esp[80];
   WIN *JF;
   JF = abre_funcoes( funcao( "1 2 3 E T") );

   ret = ACEITA;

	if ( entry->tecla != K_ESC && strcmp(entry->str, OP_SIM) == 0 ) {

      jm = abre_mensagem( M_FIMEDIT );
      win = abre_win(8, 14, 17, 64, 0, 0, JP);  /* abre janela tabela */
   	fw_title(win, T_EDITINST, win->w_atrib);
   	fwbox(win);

      if ( ap_reldoc == NULL )
         ap_reldoc = load_buffer( 0, ET_MEM, (TEXT *) buf_iotxt, 0);

      /* informa ao editor os caracteres escapes */
      pr_escape("2TBE", lin_escape);
      ap_reldoc->escape = (TEXT *)lin_escape;
      ap_reldoc->def_mens = (TEXT *)M_FIMEDIT ;

      sprintf(ajuda_esp, "%s%c%s%c%s",
              ap_home_lb, BARRA, loc_str(7,5),  BARRA, H_CMDESP);

      user_help = ajuda_esp;
      ret = edit_buffer(ap_reldoc, ET_EDITA, win, jm, 0 );
      user_help = NULL;

   	fwkill(win);
	   fecha_mensagem(jm);
   }

   piece(op_sn, entry->str,  '@', 2, TE);

   fecha_funcoes(  JF );

	return( ret );
}


F__LOC	COUNT pr_ordem(entry)
d_entry *entry;
{  COUNT ret=0;
   TEXT *campo;
   COUNT lin;

   lin = entry->lin - ((TIPO_REL != 'E') ? LC1 : LC1_ETQ) ;

   switch ( lin ) {
      case 0  :   campo = rw->r_1campo; break;
      case 1  :   campo = rw->r_2campo; break;
      case 2  :   campo = rw->r_3campo; break;
   }

   if ( campo[0] == '\0' ) {
      entry->str[0] = '\0';
      ret = 1;  /* nao le */
   }

   return( ret );
}

F__LOC	COUNT pr_nlinhas(entry)
d_entry *entry;
{  COUNT ret = 0;
   TEXT *campo;
   COUNT lin;

   lin = entry->lin - ((TIPO_REL != 'E') ? LC1 : LC1_ETQ) ;

   switch ( lin ) {
      case 0  :   campo = rw->r_1campo; break;
      case 1  :   campo = rw->r_2campo; break;
      case 2  :   campo = rw->r_3campo; break;
   }

   if ( campo[0] == '\0' ) {
      entry->str[0] = '\0';
      ret = 1;  /* nao le */
   }

   return( ret );
}

F__LOC	COUNT pr_total(entry)
d_entry *entry;
{  COUNT ret=0;
   TEXT *campo;
   COUNT lin;

   if ( TIPO_REL != 'C' ) {
      strcpy(entry->str, OP_NAO);
      ret = 1;  /* nao le */
      goto fim;
   }

   lin = entry->lin - ((TIPO_REL != 'E') ? LC1 : LC1_ETQ) ;

   switch ( lin ) {
      case 0  :   campo = rw->r_1campo; break;
      case 1  :   campo = rw->r_2campo; break;
      case 2  :   campo = rw->r_3campo; break;
   }

   if ( campo[0] == '\0' ) {
      entry->str[0] = '\0';
      ret = 1;  /* nao le */
   }

fim:

   return( ret );
}

F__LOC	COUNT pr_campos(entry)
d_entry *entry;
{  COUNT ret=0, flag;
   COUNT lin;

   lin = entry->lin - ((TIPO_REL != 'E') ? LC1 : LC1_ETQ) ;

   switch ( lin ) {
      case 0  :   flag = 1;                                    break;
      case 1  :   flag = (rw->r_1campo[0] != '\0') ? 1 : 0 ;    break;
      case 2  :   flag = (rw->r_2campo[0] != '\0') ? 1 : 0 ;    break;
   }

   if ( ! flag )
      ret = 1;  /* nao le */

   return( ret );
}


F__LOC	COUNT vl_cppsq(entry)
d_entry *entry;
{	COUNT ret, i;
   COUNT tecla;
   COUNT sele;
   TEXT *pcampo;

   sele = BASE_EDIT;

	tecla = entry->tecla;
   trim(entry->str);

   if (tecla != K_F0 && tecla != K_DN && tecla != K_PGDN &&
       entry->str[0] == '\0' ) {
      ret   = ACEITA;
      goto fim;
   }

	if ( tecla == K_ESC || tecla == K_UP || tecla == K_PGUP ) {
      ret = ACEITA;
      goto fim;
   }

   sel_campo(sele, NULL, 1 ); /* flag 1 so resetar ultimo campo */


   if ( tecla != K_F0 && entry->str[0] != '\0' ) {
      if ( ver_campo(sele, entry->str, &i) >= 0 ) {
         ret   = ACEITA;
         goto fim;
      }
   }

	ret    = ACT_REIMP;

   pcampo = sel_campo(sele, NULL, 0);

   if (pcampo == NULL ) {
      entry->str[0] = '\0';
   }
   else {
      strcpy(entry->str, pcampo);
   }

fim:

	return( ret );
}




F__LOC   VOID ld_re(p)
Rel *p;
{  COUNT ind;
   Campo **vetcp;

   strcpy (rw->r_ident,     p->rel_ident  );
	strcpy (rw->r_titulo,    p->rel_titulo );
	strcpy (rw->r_rodape,    p->rel_rodape );

   strcpy(rw->r_cabec, (p->rel_cabec == 'S') ? OP_SIM : OP_NAO);

   if ( p->rel_tipo != 'E' && (p->rel_l_inter > 0 || p->rel_c_inter > 0)) {
   	sprintf(rw->r_linhas,  "%d-%d-%d",
         p->rel_linhas, p->rel_l_inter, p->rel_c_inter);
   }
   else {
   	sprintf(rw->r_linhas,  "%d", p->rel_linhas);
   }

	sprintf(rw->r_colunas, "%d", p->rel_colunas);

	sprintf(rw->r_elin, "%d", p->rel_l_inter);
	sprintf(rw->r_ecol, "%d", p->rel_c_inter);
	sprintf(rw->r_netq, "%d", p->rel_n_etq  );

   piece(OP_CARACTER,  rw->r_caracter, '@', (p->rel_caracter == 'N') ? 1 : 2, TE);
   piece(OP_QUALIDADE, rw->r_qualid,   '@', (p->rel_qualid   == 'N') ? 1 : 2, TE);
   piece(op_sn,   rw->r_folha,    '@', (p->rel_folha    == 'S') ? 1 : 2, TE);

   piece(op_sn,    rw->r_editdoc,  '@', 2, TE);

   rw->r_1campo[0] = '\0';
   rw->r_2campo[0] = '\0';
   rw->r_3campo[0] = '\0';

   vetcp = pos_vetcmp(BASE_EDIT);

   if ( (ind = acha_campo( BASE_EDIT, p->rel_1numcampo)) >= 0 )
      strcpy(rw->r_1campo, vetcp[ind]->cp_nome);

   if ( (ind = acha_campo( BASE_EDIT, p->rel_2numcampo)) >= 0 )
      strcpy(rw->r_2campo, vetcp[ind]->cp_nome);

   if ( (ind = acha_campo( BASE_EDIT, p->rel_3numcampo)) >= 0 )
      strcpy(rw->r_3campo, vetcp[ind]->cp_nome);

   piece(OP_ORDEM,  rw->r_1ord, '@', (p->rel_1ord == 'A') ? 1 : 2, TE);
   piece(OP_ORDEM,  rw->r_2ord, '@', (p->rel_2ord == 'A') ? 1 : 2, TE);
   piece(OP_ORDEM,  rw->r_3ord, '@', (p->rel_3ord == 'A') ? 1 : 2, TE);

	sprintf(rw->r_1linhas, "%d", p->rel_1linhas);
	sprintf(rw->r_2linhas, "%d", p->rel_2linhas);
	sprintf(rw->r_3linhas, "%d", p->rel_3linhas);

   piece(op_sn, rw->r_1total,    '@', (p->rel_1total  == 'S') ? 1 : 2, TE);
   piece(op_sn, rw->r_2total,    '@', (p->rel_2total  == 'S') ? 1 : 2, TE);
   piece(op_sn, rw->r_3total,    '@', (p->rel_3total  == 'S') ? 1 : 2, TE);
}



F__LOC   VOID seta_re(p)
Rel *p;
{  TEXT aux[TD + 1];
   TEXT aux2[TE+1];

	strcpy(p->rel_titulo, rw->r_titulo   );
	strcpy(p->rel_rodape, rw->r_rodape   );

   piece(op_sn, aux, '@', 1, TD );
   if ( strcmp( rw->r_cabec,    aux) == 0 ) p->rel_cabec  = 'S';
   else                                     p->rel_cabec  = 'N';

   p->rel_colunas = atoi(rw->r_colunas);

   if ( p->rel_tipo != 'E' ) {
      piece(rw->r_linhas, aux2, '-', 1, TE); p->rel_linhas  = atoi(aux2);
      piece(rw->r_linhas, aux2, '-', 2, TE); p->rel_l_inter = atoi(aux2);
      piece(rw->r_linhas, aux2, '-', 3, TE); p->rel_c_inter = atoi(aux2);
      p->rel_n_etq   = l_uteis(rw->r_linhas);
   }
   else {
      p->rel_linhas  = atoi(rw->r_linhas);
      p->rel_l_inter = atoi(rw->r_elin);
      p->rel_c_inter = atoi(rw->r_ecol);
      p->rel_n_etq   = MAX(1, atoi(rw->r_netq));
   }


   piece(OP_CARACTER, aux, '@', 1, TD );
   if ( strcmp( rw->r_caracter, aux) == 0 ) p->rel_caracter  = 'N';
   else                                     p->rel_caracter  = 'C';


   piece(OP_QUALIDADE, aux, '@', 1, TD );
   if ( strcmp( rw->r_qualid,   aux) == 0 ) p->rel_qualid = 'N';
   else                                     p->rel_qualid = 'C';


   piece(op_sn, aux, '@', 1, TD );
   if ( strcmp( rw->r_folha,    aux) == 0 ) p->rel_folha = 'S';
   else                                     p->rel_folha = 'N';



   ver_campo(BASE_EDIT, rw->r_1campo, &p->rel_1numcampo);
   ver_campo(BASE_EDIT, rw->r_2campo, &p->rel_2numcampo);
   ver_campo(BASE_EDIT, rw->r_3campo, &p->rel_3numcampo);


   piece(OP_ORDEM, aux, '@', 1, TD );
   if ( strcmp( rw->r_1ord,    aux) == 0 ) p->rel_1ord  = 'A';
   else                                    p->rel_1ord  = 'D';
   if ( strcmp( rw->r_2ord,    aux) == 0 ) p->rel_2ord  = 'A';
   else                                    p->rel_2ord  = 'D';
   if ( strcmp( rw->r_3ord,    aux) == 0 ) p->rel_3ord  = 'A';
   else                                    p->rel_3ord  = 'D';

   p->rel_1linhas  = atoi(rw->r_1linhas);
   p->rel_2linhas  = atoi(rw->r_2linhas);
   p->rel_3linhas  = atoi(rw->r_3linhas);


   piece(op_sn, aux, '@', 1, TD );
   if ( strcmp( rw->r_1total,    aux) == 0 ) p->rel_1total = 'S';
   else                                      p->rel_1total = 'N';
   if ( strcmp( rw->r_2total,    aux) == 0 ) p->rel_2total = 'S';
   else                                      p->rel_2total = 'N';
   if ( strcmp( rw->r_3total,    aux) == 0 ) p->rel_3total = 'S';
   else                                      p->rel_3total = 'N';
}


F__LOC   VOID zera_re()
{
   piece(op_sn, rw->r_cabec, '@', 1, TE);

   switch ( TIPO_REL ) {
      case 'F' :
      case 'C' :
                  sprintf(rw->r_linhas,  "%d", 66);
	               sprintf(rw->r_colunas, "%d", 78);
                  break;
   }

   piece(OP_CARACTER,   rw->r_caracter, '@', 1, TE);
   piece(OP_QUALIDADE,  rw->r_qualid,   '@', 1, TE);
   piece(op_sn,    rw->r_folha,    '@', 2, TE);
   piece(op_sn,    rw->r_editdoc,  '@', 2, TE);

   piece(op_sn,    rw->r_1total,   '@', 2, TE);
   piece(op_sn,    rw->r_2total,   '@', 2, TE);
   piece(op_sn,    rw->r_3total,   '@', 2, TE);
}
