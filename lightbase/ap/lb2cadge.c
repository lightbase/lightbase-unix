#include  "lb2all.h"


#include  <string.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"


#define   H_ACDAN    "ACESDAN"


#define   STR_DUAS    "%s\n%s"

#define   DBG_OPINV  "Operador nao definido : [%c]"
#define   DBG_ALTINV "Tentativa de usar alt_rec com tamanho > tamanho corrente"

COUNT ap_ld_err;  /* global para indicar erro em load_rec */

extern COUNT ct_semseguranca;


/*#  ---------------------------------------------------------------------
**#   Funcao...: POINTER find_rec()
**#   Objetivo.: Localizar posicao da chave dentro de um arquivo
**#              formatado (Tipo pre-definido)
**#   Entrada..: datno    - Numero associado ao arquivo de dados
**#              indno    - numero associado ao arquivo de indices
**#              tipo_reg - Tipo de registro
**#              ch_pesq  - chave de pesquisa
**#              operador - operador para localizar registro (vide find_rec)
**#
**#   Retorna..: - Posicao do registro dentro do arquivo
**#              - 0 se chave nao foi localizada.
**#
**#   Obs:       - As chaves sao avaliadas como STRING.
**#
**/
F__GLB   POINTER find_rec(datno, indno, tipo_reg, ch_pesq, operador)
COUNT datno, indno;
TEXT *tipo_reg, *ch_pesq, operador;
{  COUNT    i, tam;
	TEXT     *po, *pc;
	TEXT     chave[80];
   POINTER  pos;

   RECALFIL(datno);
   RECALFIL(indno);

	trim(ch_pesq);

   /* verificar se chave de pesquisa JA existe */

	strcpy(chave, tipo_reg);
	strcat(chave, ch_pesq);
   tam = (COUNT) strlen(chave);

   /* procura chave */

   uerr_cod = 0;

   switch ( operador ) {
		case '=' :  pos = EQLKEY(indno, chave, tam + 1);
                  memcpy((TEXT *)ap_chave_ret, chave, tam + 1);
                  break;
		case '>' :  pos = GTKEY(indno, chave, ap_chave_ret, tam + 1);
                  break;
		case '<' :  pos = LTKEY(indno, chave, ap_chave_ret, tam + 1);
                  break;
		case 'F' :  pos = FRSKEY(indno, ap_chave_ret); /* FIRST */
                  break;
		case 'L' :  pos = LSTKEY(indno, ap_chave_ret); /* LAST */
                  break;
		case 'P' :  pos = PRVKEY(indno, ap_chave_ret); /* PREVIOUS */
                  break;
		case 'N' :  pos = NXTKEY(indno, ap_chave_ret); /* NEXT */
                  break;
		case '^' :  pos = GTEKEY(indno, chave, ap_chave_ret, tam + 1); /* >= */
                  break;
      default  :  debug(DBG_OPINV, operador);
                  break;
   }

	if ( uerr_cod != NO_ERROR ) {
		arq_erro(tipo_reg, uerr_cod);
		pos = (POINTER) 0;
      goto fim;
   }

   if ( pos != (POINTER) 0 ) {

      pc = (TEXT *)&pos;
      po = &ct_indkey[ct_lenkey - sizeof(POINTER)];

#ifdef LOW_HIGH
   	pc += sizeof(POINTER);
   	for (i = 0; i < sizeof(POINTER); i++ )
   		*--pc = *po++;
#else
      for (i = 0; i < sizeof(POINTER); i++ )
   	   	*pc++ = *po++;
#endif
   }

fim :

   return(pos);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: TEXT *load_rec()
**#   Objetivo.: Ler registro de arquivos formatados (Tipo pre-definido)
**#   Entrada..: datno    - Numero associado ao arquivo de dados
**#              indno    - numero associado ao arquivo de indices
**#              tipo_reg - Tipo de registro
**#              ch_pesq  - chave de pesquisa
**#              result   - Ponteiro para onde devera ser lido o registro
**#              tam_max  - tamanho maximo de informacao a ser recuperada
**#              operador - operador para localizar registro (vide find_rec)
**#
**#   Retorna..: - Ponteiro para onde o registro foi lido
**#              - NULL se nenhum registro foi lido
**#
**#   Obs:       - Informando result == NULL, esta funcao alocara' uma area
**#                para onde devera ser lido o registro. Cabera `a funcao
**#                chamadora gerenciar a liberacao da area alocada
**#              - Caso tam_max seja menor que o tamanho do registro
**#                localizado, sera lido apenas parte do registro
**#              - As chaves sao avaliadas como STRING.
**#
**/
F__GLB   TEXT *load_rec(datno, indno, tipo_reg, ch_pesq, result, tam_max, operador)
COUNT datno, indno;
TEXT *tipo_reg, *ch_pesq;
VOID_T  *result;
UCOUNT tam_max;
TEXT  operador;
{  POINTER pos = (POINTER) 0;
   UCOUNT tam;
	VOID_T *area;
	extern COUNT uerr_cod;

   ap_ld_err = 0;

   pos = find_rec(datno, indno, tipo_reg, ch_pesq, operador);

   if ( ! pos ) {
      result = NULL;
      goto fim;
   }


   /* existe registro a ser lido (segundo indice) */

   area  = result;

   tam = GTVLEN(datno, pos);

   if ( uerr_cod != 0 ) {
		arq_erro(tipo_reg, uerr_cod);
		result = NULL;
      ap_ld_err = 1; /* erro de acesso */
      goto fim;
   }

	if (tam > tam_max || result == NULL ) {
		area = (VOID_T*)malloc(tam);
      if ( area == NULL ) {
         mens_erro(H_NOMEMO, SEM_MEMO);
   		result = NULL;
         ap_ld_err = 2; /* erro de falta de memoria */
         goto fim;
      }
   }

	if ( RDVREC(datno, pos, (TEXT *)area, tam) != NO_ERROR ) {
		arq_erro(tipo_reg, uerr_cod);
		result = NULL;
      goto fim;
   }

   if ( area != result ) { /* leu para outra area */
      if ( result != NULL ) {
         memcpy((TEXT *)result, area, tam_max);
         free((TEXT *)area);  /* area alocada para ler parte */
      }
      else {
         result = area; /********* retorna area alocada ******/
                        /*  ATENCAO --> que mandar ler tem que liberar */
      }
   }

fim :

   return((TEXT *)result);
}


/*#  ---------------------------------------------------------------------
**#   Funcao...: COUNT add_ind()
**#   Objetivo.: Inclui chave no indice
**#   Entrada..: datno    - Numero associado ao arquivo de dados
**#              indno    - numero associado ao arquivo de indices
**#              tipo_reg - Tipo de registro
**#              ch_ind   - chave a ser incluida
**#              pos      - Posicao do registro associado a chave
**#
**#   Retorna..: - 0 se indice incluido OK
**#              - -1 ERRO na inclusao do indice
**#
**/
F__GLB   COUNT add_ind(datno, indno, tipo_reg, ch_ind, pos)
COUNT datno, indno;
TEXT *tipo_reg, *ch_ind;
POINTER pos;
{  COUNT ret = 0, tam, i;
   TEXT chave[80];
	TEXT *po, *pc;

   RECALFIL(datno);
   RECALFIL(indno);


	/* coloca chave no indice */

	trim(ch_ind);
	strcpy(chave, tipo_reg);
	strcat(chave, ch_ind);


	pc = (TEXT *)&pos;
	po = chave;

   tam = (COUNT) strlen(chave);

	tam = ( tam <= (LB_KEYMAX - sizeof(POINTER) - 1)) ? tam : (LB_KEYMAX - sizeof(POINTER) - 1);

   chave[tam++] = '\0';

	po = &chave[tam];

#ifdef LOW_HIGH
	pc += sizeof(POINTER);
	for (i = 0; i < sizeof(POINTER); i++ )
		 *po++ = *--pc;
#else
	for (i = 0; i < sizeof(POINTER); i++ )
		 *po++ = *pc++;
#endif

	tam += sizeof(POINTER);

	ret = ADDKEY(indno, chave, pos, REGADD, tam);

	if ( uerr_cod  != NO_ERROR ) {
		arq_erro(tipo_reg, uerr_cod);
		ret = -1;
		goto fim;
   }

   ret = 0;

fim :

   FLUSHFIL(datno);
   FLUSHFIL(indno);

   return(ret);
}




/*#  ---------------------------------------------------------------------
**#   Funcao...: POINTER add_rec()
**#   Objetivo.: Incluir registro e respectiva chave no arquivo
**#              formatado (Tipo pre-definido)
**#   Entrada..: datno    - Numero associado ao arquivo de dados
**#              indno    - numero associado ao arquivo de indices
**#              tipo_reg - Tipo de registro
**#              ch_ind   - chave de indexacao
**#              registro - Ponteiro para registro a ser gravado
**#              tam_reg  - Tamanho do registro a ser gravado
**#
**#   Retorna..: - Posicao de gravacao do registro
**#              - 0 se ERRO na inclusao do registro
**#
**/
F__GLB   POINTER add_rec(datno, indno, tipo_reg, ch_ind, registro, tam_reg)
COUNT datno, indno;
TEXT *tipo_reg, *ch_ind;
VOID_T  *registro;
UCOUNT tam_reg;
{  COUNT ret = 0, salva_seg;
   POINTER pos;

   RECALFIL(datno);
   RECALFIL(indno);

   salva_seg = ct_semseguranca;
   ct_semseguranca = 1;

	if ( (pos = NEWVREC(datno, tam_reg)) == (POINTER) 0 ) {
		arq_erro(tipo_reg, uerr_cod);
		ret = -1;
		goto fim;
   }

   if ( registro != NULL ) {
	   ret = WRTVREC(datno, pos, registro, tam_reg);

      if ( uerr_cod != NO_ERROR ) {
		   arq_erro(tipo_reg, uerr_cod);
		   ret = -1;
		   goto fim;
      }
      LOKREC(datno, FREE, pos);
   }

	/* coloca chave no indice */

   if ( add_ind(datno, indno, tipo_reg, ch_ind, pos) != 0 ) {
      ret = -1;
      goto fim;
   }

   ret = 0;


fim :

	ct_semseguranca = salva_seg;

   FLUSHFIL(datno);
   FLUSHFIL(indno);

   if ( ret == -1 ) return((POINTER)0);
   else             return(pos);
}



/*#  ---------------------------------------------------------------------
**#   Funcao...: POINTER alt_rec()
**#   Objetivo.: Atualizar registro no arquivo
**#   Entrada..: datno    - Numero associado ao arquivo de dados
**#              indno    - numero associado ao arquivo de indices
**#              tipo_reg - Tipo de registro
**#              pos      - Posicao do registro no arquivo
**#              registro - Ponteiro para registro a ser gravado
**#              tam_reg  - Tamanho do registro a ser gravado
**#
**#   Retorna..: - Posicao de re_gravacao do registro
**#              - 0 se ERRO na re_gravacao do registro
**#
**/
F__GLB   POINTER alt_rec(datno, indno, tipo_reg, pos, registro, tam_reg)
COUNT datno, indno;
TEXT *tipo_reg;
POINTER pos;
VOID_T  *registro;
UCOUNT tam_reg;
{

   indno = indno; /* so para padronizar chamada */

   RECALFIL(datno);
   RECALFIL(indno);

   if ( tam_reg > GTVLEN(datno, pos) ){
      debug(DBG_ALTINV);
      tam_reg =  GTVLEN(datno, pos);
   }


	WRTVREC(datno, pos, registro, tam_reg);

   if ( uerr_cod != NO_ERROR ) {
		arq_erro(tipo_reg, uerr_cod);
		pos = (POINTER) 0;
		goto fim;
   }


fim :

   FLUSHFIL(datno);
   FLUSHFIL(indno);

   return(pos);
}



/*------------------------------------------------------exc_rec--------*/
/*
**   Exclui registro no arquivo e indice especificado.
**    - ch_ind e a chave para o registro
**    - so_index se == SO_INDEX so exclui do indice, senao exclui do
**      cadastro e do indice.
**
**    Retorna : Endereco (posicao no arquivo) onde estava gravado o registro
**              Ou (POINTER) 0 no caso de ERRO.
*/
F__GLB   POINTER exc_rec(datno, indno, tipo_reg, ch_ind, so_index)
COUNT datno, indno;
TEXT *tipo_reg, *ch_ind;
COUNT so_index;
{  COUNT ret = 0, tam, i;
   TEXT chave[80];
	TEXT *po, *pc;
   extern COUNT uerr_cod;
   POINTER pos;

   RECALFIL(datno);
   RECALFIL(indno);

   /* localiza posicao do registro consultando o indice */

	pos = find_rec(datno, indno, tipo_reg, ch_ind, '=');

   if (pos == (POINTER) 0) { /* indice nao localizado */
      ret = -1;
      goto fim;
   }


   /* so vai liberar area se assim for solicitado */

   if ( so_index != SO_INDEX ) {

	   ret = RETVREC(datno, pos);

      if ( ret != NO_ERROR ) {
         mens_erro(H_ACDAN, E_CGER1, uerr_cod);
         ret = -1;
         goto fim;
      }
   }

	/* exclui chave do indice */

	trim(ch_ind);
	strcpy(chave, tipo_reg);
	strcat(chave, ch_ind);


	pc = (TEXT *)&pos;
	po = chave;

   tam = (COUNT) strlen(chave);

	tam = ( tam <= (LB_KEYMAX - sizeof(POINTER) - 1)) ? tam : (LB_KEYMAX - sizeof(POINTER) - 1);

   chave[tam++] = '\0';

	po = &chave[tam];

#ifdef LOW_HIGH
	pc += sizeof(POINTER);
	for (i = 0; i < sizeof(POINTER); i++ )
		 *po++ = *--pc;
#else
	for (i = 0; i < sizeof(POINTER); i++ )
		 *po++ = *pc++;
#endif

	tam += sizeof(POINTER);

	ret = DELKEY(indno, chave, tam);      /* ANANIAS 0 e REGADD */

	if ( ret != 0 ) {
      mens_erro(H_ACDAN, E_CGER2, uerr_cod);
		ret = -1;
      goto fim;
	}


fim :

   FLUSHFIL(datno);
   FLUSHFIL(indno);

   if ( ret == -1 ) return((POINTER)0);
   else             return(pos);

}







/**
**#  ---------------------------------------------------------------------
**#   Funcao...: file_open
**#   Objetivo.: tentar sucessivas vezes abrir o arquivo especificado
**#              no modo especificado
**#   Entrada..: num  - numero ao qual o arquivo sera associado
**#              nome - nome do arquivo
**#              modo - modo de abertura
**#              serie - numero de serie do produto
**#              magico- numero magico do arquivo
**#
**#   Retorna..:  0  - se abriu OK
**#               -1 - Erro na abertura dos arquivos da base
**#
**/
F__GLB   COUNT file_open(num, nome, modo, serie, magico)
COUNT num;
TEXT *nome;
COUNT modo;
LONG serie;
COUNT magico;
{  COUNT ret;
   extern COUNT ct_updwarning;

	if ( (ret = OPNFIL(num, nome, modo, serie, magico)) != 0 ) {
      switch(ret) {
         case FNOP_ERR  : mens_erro(H_LOCKED, E_LOCKED);              break;
         case FPERM_ERR : mens_erro(H_PERMAC, E_PERMAC, nome);        break;
         case 14        : mens_erro(H_CLSANO, E_CLSANO);              break;
         case 58        :
         case 59        : mens_erro(H_CONFAC, E_CONFAC);              break;
         default        : mens_erro(H_OPESO, E_OPE_SO, ret, nome);    break;
      }

      ret = -1;
   }

   else {
      if ( ct_updwarning ) {
         TEXT str0[80], str1[80], str2[80];
         strcpy(str0, TW_NOUPD0);
         strcpy(str1, TW_NOUPD1);
         strcpy(str2, TW_NOUPD2);

         if ( decida( M_ATENCAO, str0, H_NOUPD, 2, 59, STR_DUAS,
                     str1, str2 ) != 0 )
            ret = -1;
         else
            ct_updwarning = 0;
      }
   }

   return(ret);
}


F__GLB   COUNT arq_erro(tipo_reg, erro)
TEXT * tipo_reg;
COUNT erro;
{  TEXT temp[100];

   sprintf(temp, E_CGER9, erro, tipo_reg);
   debug(temp);
   return(0);
}


