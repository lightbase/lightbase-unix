#include  "lb2all.h"

#include  <ctype.h>
#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2locs.h"
#include  "lb2prot1.h"

#ifdef    VAXVMS
#include  "menu.h"
#else
#include  "../mt/menu.h"
#endif

#define   DBG_NUMJA "numero de janela invalido"


F__GLB   TEXT *prep_palavra( P1 )
TEXT *P1;
{
   FAST UTEXT  *P2;

   if ( P1 == NULL )  return( NULL );

   for ( P2 = (UTEXT *)P1;   *P2 != '\0';   P2++ )
   {
      if ( *P2 >= 'a'  &&  *P2 <= 'z' ) {  *P2 -= 32;  continue; }

      if ( *P2 > 0  &&  *P2 < 32 ) {  *P2 = '?';  continue; }

      if ( *P2 <= 127 ) {  continue;  }

      switch ( *P2 )
      {
	   case UG_CED_c :
	   case UG_CED_C :  *P2 = 'C';   continue;

   	case UG_GRA_a :
	   case UG_AGU_a :
   	case UG_CIR_a :
	   case UG_TIL_a :
   	case UG_GRA_A :
	   case UG_AGU_A :
   	case UG_CIR_A :
	   case UG_TIL_A :   *P2 = 'A';   continue;

	   case UG_AGU_e :
	   case UG_CIR_e :
	   case UG_AGU_E :
	   case UG_CIR_E :   *P2 = 'E';   continue;

	   case UG_AGU_i :
	   case UG_AGU_I :   *P2 = 'I';   continue;

	   case UG_TRE_u :
	   case UG_TRE_U :   *P2 = UG_TRE_U;   continue;

	   case UG_TIL_n :
	   case UG_TIL_N :   *P2 = UG_TIL_N;   continue;

	   case UG_AGU_o :
	   case UG_CIR_o :
	   case UG_TIL_o :
	   case UG_AGU_O :
	   case UG_CIR_O :
	   case UG_TIL_O :   *P2 = 'O';   continue;

	   case UG_AGU_u :
	   case UG_AGU_U :   *P2 = 'U';   continue;

      default       :   *P2 = '?';   continue;
      }
   }

   return ( P1 );
}


/**
**#  ---------------------------------------------------------------------
**#   Funcao...: COUNT val_indice()
**#   Objetivo.: Analisar se uma palavra pode ser indice (USUARIO/BASE)
**#   Entrada..: chave - indice proposto
**#   Retorna..:  OK   - palavra proposta pode ser indice.
**#              !OK   - Indice invalido
**#
**#   Obs......: - Para ser valido o indice deve :
**#                . conter somente letras, digitos, o caracter '_'
**#                . Iniciar por uma letra
**#                . Nao conter espacos em branco
**#              - Brancos finais sao RETIRADOS da chave
**#
**/
F__GLB   COUNT val_indice(chave_ori)
TEXT *chave_ori;
{  TEXT c = 99;
   TEXT chave[TD+1], *p;

   strcpy(chave, chave_ori);
   prep_palavra(chave);
   p = chave;

	if ( isalpha(*p) ) {

		trim (p);

		while ( (c = *p++) != '\0' ) {
			if ( c == ' ' ) break;
			if ( c == '_' ) continue;
			if ( isdigit(c) ) continue;
			if ( isalpha(c) ) continue;
         break;
		}
	}

	if ( c != '\0' ) c = !OK; /* analise achou caracter invalido */
	else             c = OK;  /* analise avaliou todos os caracteres OK */

	return((COUNT)c);
}


/**
**#  ---------------------------------------------------------------------
**#   Funcao...: TEXT * field_info()
**#   Objetivo.: Obter conteudo de um campo ( registro corrente )
**#   Entradas.: numero do campo
**#              ou indice em vetcmp
**#              ou nome do campo
**#              No nome do campo pode ser especificado um limite de
**#              tamanho usando uma virgula( ex.: "NOME,20" ou ",20"
**#              Limite de tamanho ( tambem pode limitar );
**#   Retorna..: ponteiro para informacao
**#              NULL se erro ou campo TEXTO
**#   CUIDADO..: Area de informacao estatica
**/
F__GLB   TEXT *field_info(sele, cp_ind, cp_num, cp_nome, limite)
COUNT sele, cp_ind, cp_num;
TEXT *cp_nome;
COUNT limite;
{  static TEXT conteudo[ 80 + 1 ];
   TEXT campo[TE + 1], *p1, c;
   COUNT ret;
   Campo **vet_cmp, *pc;

   ret = 0;
   vet_cmp  = pos_vetcmp(sele);

   if ( limite < 1 || limite > 80 )
      limite = 80; /* Tamanho maximo da informacao */

   if ( cp_ind < 0 ) { /* nao sabe o indice de vetcmp */
      if ( cp_num > 0 ) {  /* sabendo o numero acha em vetcmp */
         cp_ind = acha_campo(sele, cp_num);
      }
      else { /* sabendo o nome do campo tenta localizar em vetcmp */
         p1 = campo;
         *p1 = '\0';
	      if ( isalpha(*cp_nome) ) {
		      while ( (c = *p1++ = *cp_nome++) != '\0' ) {
			      if ( c == ' ' ) break;
			      if ( c == '_' || isdigit(c) || isalpha(c) ) continue;
               break;
		      }
            p1--;
            cp_nome--;
            *p1 = '\0';
            if ( strlen(campo) > 0 ) { /* ja tem o nome do campo separado */
               cp_ind = ver_campo(sele, campo, &cp_num);
               if ( *cp_nome == ',' )
                  limite = atoi(cp_nome + 1);
            }
	      }
      }
   }


   if ( cp_ind < 0 || cp_ind >= qtd_vetcmp(sele)) { /* Nao consegui identificar o campo */
      ret = -1;
      goto fim;
   }

   pc = vet_cmp[cp_ind];

   if ( pc->cp_tipo == 'T' ) {
      ret = -1;
      goto fim;
   }

   ret = strlen(pc->cp_newdata);
   if ( ret > limite ) {
      strncpy(conteudo, pc->cp_newdata, limite);
      conteudo[limite] = '\0';
      ret = limite;
   }
   else {
      strcpy(conteudo, pc->cp_newdata);
   }

fim :

   if ( ret == -1 )
      return(NULL);
   else
      return(conteudo);

}


F__GLB   COUNT lin_texto(adoc, lin, buf, flag)
ARG *adoc;
COUNT lin;
TEXT *buf;
COUNT flag; /* se true retira marca de fim de paragrafo */
{  COUNT ret = 0, tam_linha;

   if ( lin >= adoc->t_lines ) {
      ret = -1;  /* indica fim de arquivo e nao um erro */
      goto fim;
   }

   tam_linha = lin_buffer( adoc, lin, buf);

   if ( tam_linha < 0 ) {
      ret = -2;  /* indica erro */
      goto fim;
   }

   /* retira marca de margens caso existam */
   if ( (UC)buf[tam_linha-1] == PARA_CHR ) {
      if (tam_linha >= 5 &&
               (UC)buf[tam_linha-2] == ET_FORM2 &&
               (UC)buf[tam_linha-6] == ET_FORM1) {
         tam_linha -= 5;
         buf[tam_linha - 1] = PARA_CHR;
         buf[tam_linha] = '\0';
      }

      if ( flag ) {
         buf[--tam_linha] = '\0';
      }
   }

   ret = tam_linha;

fim :

   return(ret);
}



F__GLB   VOID en_cripta(buf, tam)
UTEXT *buf;
UCOUNT tam;
{

   if ( WBS->bs_versao <= VERSAO_NOCRIPT ||
        WBS->bs_idbase == 0 )
      return;

   while(tam) {
      buf[tam] ^= ap_cripto[tam & 0x7];
      tam--;
   }


}

F__GLB   VOID de_cripta(buf, tam)
UTEXT *buf;
UCOUNT tam;
{

   if ( WBS->bs_versao <= VERSAO_NOCRIPT ||
        WBS->bs_idbase == 0 )
      return;

   while(tam) {
      buf[tam] ^= ap_cripto[tam & 0x7];
      tam--;
   }

}



F__GLB   WIN *abre_janela(numero)
COUNT numero;
{  WIN *win;
   COUNT lf, cf;

   if ( numero <= 0 || numero >= JANELA_MOR )
      debug(DBG_NUMJA);

   lf = ap_winloc[numero][L_FI];
   cf = ap_winloc[numero][C_FI];


   if ( lf > 200 ) lf = size_y - (lf - 200);
   if ( cf > 200 ) cf = size_x - (cf - 200);

   win = abre_win(ap_winloc[numero][L_IN], ap_winloc[numero][C_IN],
                  lf, cf,
                  ap_winloc[numero][L_VI], ap_winloc[numero][C_VI],
                  ap_winloc[numero][TIPO] & 0x7F);

   win->ident = (UCOUNT) numero;

	return(win);
}




F__GLB   WIN * abre_win(li, ci, lf, cf, l_vis, c_vis, tipo)
COUNT li, ci, lf, cf, l_vis, c_vis, tipo;
/*ANANIAS @  falta parametro ident */
{  WIN *j;

   if (l_vis == 0 ) l_vis = lf-li+1;
   if (c_vis == 0 ) c_vis = cf-ci+1;

   j = win_make(cf-ci+1, lf-li+1, c_vis, l_vis, ci, li);

   if ( j == NULL ) {
      fim_lb(SEM_MEMO);
   }

	j->w_flags = W_VISIB | W_NOSCROL;

   switch (tipo) {

      case LM :   j->w_atrib  = LM_CAMPO;
	               j->w_optatr = LM_BARRA;
	               j->w_defatr = LM_DESTAQUE;
						j->w_boxatr = LM_CAMPO;
						j->t_atr    = LM_CAMPO;
						j->campoatr = LM_CAMPO;
                  break;

      case JP :   j->w_atrib  = JP_JANELA;
                  j->w_optatr = JP_BARRA;
                  j->w_defatr = JP_DESTAQUE;
                  j->w_boxatr = JP_JANELA;
                  j->t_atr    = JP_JANELA;
						j->campoatr = JP_CAMPO;
                  break;


      case JS :   j->w_atrib  = JS_JANELA;
                  j->w_optatr = JS_BARRA;
                  j->w_defatr = JS_DESTAQUE;
                  j->w_boxatr = JS_JANELA;
                  j->t_atr    = JS_JANELA;
						j->campoatr = JS_CAMPO;
                  break;


      case MP :   j->w_atrib  = MP_JANELA;
                  j->w_optatr = MP_BARRA;
                  j->w_defatr = MP_DESTAQUE;
                  j->w_boxatr = MP_JANELA;
                  j->t_atr    = MP_JANELA;
						j->campoatr = MP_CAMPO;
                  break;


      case MS :   j->w_atrib  = MS_JANELA;
                  j->w_optatr = MS_BARRA;
                  j->w_defatr = MS_DESTAQUE;
                  j->w_boxatr = MS_JANELA;
                  j->t_atr    = MS_JANELA;
						j->campoatr = MS_CAMPO;
                  break;


      case JE :   j->w_atrib  = JE_JANELA;
                  j->w_optatr = JE_BARRA;
                  j->w_defatr = JE_DESTAQUE;
                  j->w_boxatr = JE_JANELA;
                  j->t_atr    = JE_JANELA;
						j->campoatr = JE_CAMPO;
                  break;

/*
      case FE :   j->w_atrib  = FE_JANELA;
                  j->w_optatr = FE_BARRA;
                  j->w_defatr = FE_DESTAQUE;
                  j->w_boxatr = FE_JANELA;
                  j->t_atr    = FE_JANELA;
						j->campoatr = FE_CAMPO;
                  break;
*/
   }

   fwcls(j);
   return(j);
}



F__GLB   VOID set_menu(m, n, opcao, status, rodape)
TEXT **m;
COUNT n;
TEXT *opcao;
TEXT *status;
TEXT *rodape;
{
   m[OP(n)]    = opcao;
   m[AJ(n)]    = rodape;
   m[AT(n)]    = status;
}







/* Funcoes para padronizar gravacao/leitura de LONG na memoria */
#ifndef PUT_LONG
F__GLB   VOID PUT_LONG(p, l)
UTEXT *p;
LONG l;
{  UTEXT *pc;
   COUNT i;

   pc = (UTEXT*)&l;

   for ( i = 0; i < sizeof(LONG) - 1; i++) {
      *p++ = *pc++;
   }
   *p = *pc;
}
#endif



#ifndef GET_LONG
F__GLB   LONG GET_LONG(p)
UTEXT *p;
{  LONG l;
   UTEXT *pc;
   COUNT i;

   pc = (UTEXT*)&l;

   for ( i = 0; i < sizeof(LONG) - 1; i++) {
      *pc++ = *p++;
   }
   *pc = *p;

   return(l);
}
#endif


/* Funcoes para padronizar gravacao/leitura de LONG na memoria */
#ifndef PUT_LONG4
F__GLB   VOID PUT_LONG4(p, l)
UTEXT *p;
LONG l;
{  UTEXT *pc;
   COUNT i;

   pc = (UTEXT*)&l;

   for ( i = 0; i < 4; i++) {
      *p++ = *pc++;
   }
}
#endif



#ifndef GET_LONG4
F__GLB   LONG GET_LONG4(p)
UTEXT *p;
{  LONG l;
   UTEXT *pc;
   COUNT i;

   l = 0;
   pc = (UTEXT*)&l;

   for ( i = 0; i < 4; i++) {
      *pc++ = *p++;
   }
   return(l);
}
#endif



F__GLB POINTER get_postexto(fonte, p)
COUNT fonte;
UTEXT *p;
{  POINTER ret_pointer;

	ret_pointer = 0;

	switch (fonte) {
	   case 'N' : /* posicao em newdata long normal */
	   			  ret_pointer = GET_LONG(p);
	   			  break;
	   case 'O' : /* posicao em olddata long truncado (4 bytes iniciais) */
	   			  ret_pointer = GET_LONG4(p);
	   			  break;

	   case 'L' : /* posicao em forma de ponteiro para long */
	   			  ret_pointer = *(POINTER *)p;
                 if ( sizeof(POINTER) > 4 )
                    debug("WWW Atencao tratamento de POINTER com tamanho maior que 4");
	   			  break;

		default  : debug("Forma de Long nao tratada");
		           break;
	}

	if ( ret_pointer < 0 ||  ret_pointer > 0xFFFFFFFF )
	   debug("Posicao de texto provavelmente invalida : %ld", ret_pointer);

	return((LONG)ret_pointer);
}


F__GLB VOID put_postexto(fonte, p, pos)
COUNT fonte;
UTEXT *p;
LONG pos;
{

	switch (fonte) {
	   case 'N' : /* posicao em newdata long normal */
	   			  PUT_LONG(p, pos);
	   			  break;
	   case 'O' : /* posicao em olddata long truncado (4 bytes iniciais) */
	   			  PUT_LONG4(p, pos);
	   			  break;
		default  : debug("Forma de Long nao tratada");
		           break;
	}

}

