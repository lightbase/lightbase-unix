#include  "lb2all.h"

#include  <string.h>
#include  HD_DIR

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

static COUNT lb4_aberto;


#define   H_CLLB4    "ERCLSLB4"   /* Erro em close lb4 */


F__GLB   COUNT open_lb4(wbs, n4, modo, serie, idarq)
Base *wbs;
TEXT *n4;
COUNT modo;
LONG serie;
UCOUNT idarq;
{  COUNT  ret, modo_op, status;
   UCOUNT tamanho, livre;
   POINTER proximo;
   Base *area;


   ret = -1;

   /* aloca area leitura do descritor da base */
   if ( ( area = (Base *) calloc(1, sizeof(Base))) == NULL ) {
      mens_erro(H_NOMEMO, SEM_MEMO);
      goto sai;
   }

   switch(modo) {
      case 'I' :  /* importacao */
                  modo_op = PERMANENT | EXCLUSIVE;
                  break;

      default  :
      case 'D' :  /* Definicao */
                  modo_op = PERMANENT | EXCLUSIVE;
                  break;

      case INFORMACAO :  /* INFORMACAO */
      case 'U' :  /* Utilizacao */
                  modo_op = PERMANENT | SHARED | READFIL;
                  break;

   }


	if ( file_open(lb4_dnum, n4, modo_op, serie, idarq) == 0 ) {

      lb4_aberto = 1;

      REG_INFO(lb4_dnum, (POINTER) 0L, &tamanho, &livre, &proximo, &status);


	   if ( RDVREC(lb4_dnum, proximo, (TEXT *)area, sizeof(Base)) == NO_ERROR ) {
         /* alterado na versao 1.8 rel 4.0d para poder mudar path so em cad_acessos */
         strcpy( area->bs_nome, wbs->bs_nome);
         strcpy( area->bs_path, wbs->bs_path);
         strcpy( area->bs_dono, wbs->bs_dono);
         memcpy((TEXT *)wbs, (TEXT *)area, sizeof(Base));

#ifndef ANTES_DE_1_8
         if ( modo == 'I' ) {
            strcpy( area->bs_nome, wbs->bs_nome);
            strcpy( area->bs_path, wbs->bs_path);
            memcpy((TEXT *)wbs, (TEXT *)area, sizeof(Base));
         }
         else {
            memcpy((TEXT *)wbs, (TEXT *)area, sizeof(Base));
         }
#endif
         area->bs_updated = 0;
         ret = 0;
         lb4_pos = (POINTER)proximo;
      }
      else {
         lb4_aberto = 0;
         CLSFIL(lb4_dnum, COMPLETE);
         ret = -1;
      }
   }

   free(area);

sai:

   return(ret);
}

F__GLB   COUNT close_lb4()
{  COUNT  ret;

	if ( lb4_aberto && CLSFIL(lb4_dnum, COMPLETE) != 0 ) {
      mens_erro(H_CLLB4, E_CLS_MA, uerr_cod);
      ret = -1;
   }
   else
      ret = 0;

   lb4_aberto = 0;

   return(ret);
}
