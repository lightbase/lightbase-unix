#include "lb2all.h"

#include <stdlib.h>
#include <string.h>

#include  "lb2maces.h"

#include "tools.h"

#ifdef MSDOS
#include <io.h>
#endif




COUNT abre_indice(arq_ind, m_keyno)
TEXT *arq_ind;
COUNT m_keyno;
{  LONG serie;
   UCOUNT id_arq;


   /* so para ver numero de serie e id_arq */
   serie = 0L;
   id_arq = 999;


   MANUT_OPEN( m_keyno, arq_ind, (PERMANENT | SHARED ), &serie, &id_arq);

	if ( OPNFIL(m_keyno, arq_ind, (PERMANENT | EXCLUSIVE ), serie, id_arq ) != NO_ERROR ) {
		printf("Erro #%d na abertura do arquivo : %.14s\n",uerr_cod, arq_ind);
      return(-1);
	}

   return(m_keyno);
}

VOID fecha_indice(m_keyno)
COUNT m_keyno;
{
	if ( CLSFIL(m_keyno, COMPLETE) ) {
		printf("Erro em CLSFIL ==> m_filno: %d    uerr_cod: %d\n", m_keyno, uerr_cod);
	}
}




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
POINTER find_rec(datno, indno, tipo_reg, ch_pesq, operador)
COUNT datno, indno;
TEXT *tipo_reg, *ch_pesq, operador;
{  COUNT    i, tam;
	TEXT     *po, *pc;
	TEXT     chave[80];
   POINTER  pos;

   datno = datno;

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
      default  :  LOG_S("Operdor invalido");
                  break;
   }

	if ( uerr_cod != NO_ERROR ) {
      LOG_SL("Erro : %d", uerr_cod);
		pos = (POINTER) 0;
      goto fim;
   }

   if ( pos != (POINTER) 0 ) {

      pc = (TEXT *)&pos;
      po = &ct_indkey[ct_lenkey - 4];

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
TEXT *load_rec(datno, indno, tipo_reg, ch_pesq, result, tam_max, operador)
COUNT datno, indno;
TEXT *tipo_reg, *ch_pesq;
VOID_T  *result;
UCOUNT tam_max;
TEXT  operador;
{  POINTER pos = (POINTER) 0;
   UCOUNT tam;
	VOID_T *area;
	extern COUNT uerr_cod;


   pos = find_rec(datno, indno, tipo_reg, ch_pesq, operador);

   LOG_SL("Localizou registro em pos = %ld\n", pos);


   if ( ! pos ) {
      result = NULL;
      goto fim;
   }


   /* existe registro a ser lido (segundo indice) */

   area  = result;

   tam = GTVLEN(datno, pos);

   if ( uerr_cod != 0 ) {
      LOG_SL("Erro : %d", uerr_cod);
		result = NULL;
      goto fim;
   }

	if (tam > tam_max || result == NULL ) {
		area = (VOID_T*)malloc(tam);
      if ( area == NULL ) {
         LOG_S("Sem memoria");
   		result = NULL;
         goto fim;
      }
   }

	if ( RDVREC(datno, pos, (TEXT *)area, tam) != NO_ERROR ) {
      LOG_SL("Erro : %d", uerr_cod);
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
COUNT add_ind(datno, indno, tipo_reg, ch_ind, pos)
COUNT datno, indno;
TEXT *tipo_reg, *ch_ind;
POINTER pos;
{  COUNT ret = 0, tam, i;
   TEXT chave[80];
	TEXT *po, *pc;


   datno = datno;

	/* coloca chave no indice */

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
      LOG_SL("Erro em ADDKEY : %d", ret);
		ret = -1;
		goto fim;
   }

   ret = 0;

fim :

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
POINTER add_rec(datno, indno, tipo_reg, ch_ind, registro, tam_reg)
COUNT datno, indno;
TEXT *tipo_reg, *ch_ind;
VOID_T  *registro;
UCOUNT tam_reg;
{  COUNT ret = 0, salva_seg;
   POINTER pos;

   salva_seg = ct_semseguranca;
   ct_semseguranca = 1;

	if ( (pos = NEWVREC(datno, tam_reg)) == (POINTER) 0 ) {
      LOG_SL("Erro em NEWVREC : %d", ret);
		ret = -1;
		goto fim;
   }

   if ( registro != NULL ) {
	   ret = WRTVREC(datno, pos, registro, tam_reg);

      if ( uerr_cod != NO_ERROR ) {
         LOG_SL("Erro em WRTVREC : %d", ret);
		   ret = -1;
		   goto fim;
      }

      LOG_SL("add_rec em pos = %ld\n", pos);
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

   if ( ret == -1 ) return((POINTER)0);
   else             return(pos);
}

