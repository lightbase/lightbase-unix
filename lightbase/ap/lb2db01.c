#include  "lb2all.h"

#include  <string.h>
#include  <time.h>

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#include  "lb2db.h"


/**
**#  ---------------------------------------------------------------------
**#   Funcao...: lb4_space
**#   Objetivo.: Realocar espaco para informacoes do arquivo de descricao
**#              da base de dados
**#   Entrada..: Pos_ant  ->  se zero nao existia informacao anterior
**#              new_tam  ->  Novo tamnho da infomacao
**#   Retorna..: posicao do novo registro em lb4. (0 se ERRO)
**/
F__GLB   POINTER lb4_space(pos_ant, new_tam)
POINTER pos_ant;
UCOUNT new_tam;
{  VRLEN tam;
   POINTER pos_ret;

   tam = 0;
   pos_ret = (POINTER) 0;


   if ( pos_ant )
      tam = GTVLEN(lb4_dnum, pos_ant);

   if ( uerr_cod )
      goto fim;

   if ( tam != new_tam ) {
	   if ( pos_ant && RETVREC(lb4_dnum, pos_ant) != NO_ERROR ) 
         goto fim;

	   if ( (pos_ret = NEWVREC(lb4_dnum, new_tam)) == (POINTER) 0 ) 
		   goto fim;

      LOKREC(lb4_dnum, FREE, pos_ret); /* vou liberar o LOCK aqui pois */
                             /* so pode existir um usuario atualizando */
   }
   else
      pos_ret = pos_ant;

fim :

   return(pos_ret);
}




/**
**#  ---------------------------------------------------------------------
**#   Funcao...: lb4_upd
**#   Objetivo.: atualizar informacoes do header da base de dados
**#   Entrada..: flag = 0 -> Nao modirfica datas
**#              flag = 1 -> Atualiza data de modificacao
**#              flag = 2 -> Atualiza data de backup
**#   Retorna..: 0 - se OK
**#             -1 - se ERRO
**/
F__GLB   COUNT lb4_upd( flag )
COUNT flag;
{  COUNT 	ret, salva;
	time_t 	tempo;


   time(&tempo);            /* data de atualizacao da base */

	if ( flag == 1 ) WBS->bs_al_data = tempo;
	if ( flag == 2 ) WBS->bs_bk_data = tempo;

   salva = WBS->bs_updated;
   WBS->bs_updated = 0;

	if ( WRTVREC(lb4_dnum, lb4_pos, (TEXT *)WBS, sizeof(Base)) != 0 ) {
      mens_erro(H_LB4BAD, E_LB4BAD, uerr_cod);
      ret = -1;
      goto fim;
   }
   else
      FLUSHFIL(lb4_dnum);

   WBS->bs_updated = salva;

   ret = 0;

fim :

   return(ret);
}


