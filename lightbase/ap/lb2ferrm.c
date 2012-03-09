#include  "lb2all.h"

#include  <string.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */


#include  "lb2files.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"

#include  "lb2fer.h"

#define   GRP_CADUSU    13
#define   E_NOPCA    loc_str(GRP_CADUSU, 502)
#define   E_NOPRD    loc_str(GRP_CADUSU, 503)


#ifdef MSDOS
   COUNT elim_acesso(TEXT *);
#else
   COUNT elim_acesso( );
#endif



F__GLB   COUNT del_base(j0)
WIN *j0;
{  COUNT ret, i;
   COUNT t1, t2, tam, x;
   TEXT *ajuda_ant = win_ajuda;
   WIN   *jm = NULL;
   TEXT nx[PATH_MAX], linha[80], *pnx;

   win_ajuda = "DEL_BASE";

   sprintf(linha, "%s - %s", rec_base->bs_nome, rec_base->bs_desc);

   t1 = strlen(T_DELBAS);
   t2 = strlen(linha);

   tam = MAX(t1, t2);

   x = (size_x - (tam + 6)) / 2;



   fwframe( 1, x, 5, x + tam + 6, SIMPLES, j0);

   x = (size_x - (t1 + 6)) / 2;
   mvwprintw(j0 , 1, x + 3, T_DELBAS);

   x = (size_x - (t2 + 6)) / 2;
   mvwprintw(j0 , 3, x + 3, linha);

   
   if ( decida( TW_EB_T00, TW_EB_T99, H_CFEXC, 4, 43, "%s\n%s\n%s\n%s",
                TW_EB_T01, TW_EB_T02, TW_EB_T03, TW_EB_T04 ) != 1 ) {
      ret = -1;
      goto fim;
   }

   jm = abre_mensagem(M_DLARQB);


   info_base(rec_base->bs_nome, rec_base->bs_path, LB_1_EXT, nx);

   /* faz pnx apontar para numero do arquivo LB? */
   pnx = strchr(nx, '\0');
   pnx--;

   for ( i = 1; i <= 4; i++ ) {
      *pnx = '0' + i;  /* Monta nome do arquivo */

      if ( access(nx, 0) == 0 && DELFILE(nx) != NO_ERROR) {
         mens_erro(H_DLF_SO, E_DLF_SO, rec_base->bs_nome);
         ret = -1;
         goto fim;
      }
   }

   /*
   **  Elimina Acessos que referenciam a base excluida
   */

   mens_1(jm, M_DLACEB);

   ret = elim_acesso(rec_base->bs_nome);

fim :

   win_ajuda = ajuda_ant;

	fecha_mensagem(jm);
   return(ret);
}



F__LOC   COUNT elim_acesso(nome_base)
TEXT *nome_base;

{  COUNT ret= 0, tam;
	TEXT ident[TE+1], chave[TE+TE+2];
   POINTER pos;


   /* primeiro exclui registro do tipo D0_ID */

	pos = exc_rec(ace_dnum, ace_inum, D0_ID, nome_base, !SO_INDEX);

   if ( pos == 0 ) {
      ret = -1;
      goto fim;
   }

   /* uma vez excluido do indice, temos que eliminar as definicoes */
   /* de acesso D1_ID e depois o D2_ID da base excluida */



   while ( 1 ) {

      strcpy(chave, D1_ID);
	   strcat(chave, nome_base);
      tam = strlen(chave);

   	pos = find_rec(ace_dnum, ace_inum, D1_ID, nome_base, '^');

      if (pos == (POINTER) 0 || strncmp(ct_indkey, chave, tam) != 0 ) {
         break;
      }

      if ( ct_indkey[tam] != '!' ) break;

      strncpy(ident, &ct_indkey[tam+1], TE+1);

      sprintf(chave, "%s!%s", nome_base, ident);
      pos = exc_rec(ace_dnum, ace_inum, D1_ID, chave, SO_INDEX);

      if ( pos == 0 )
         numdebug(560);

      sprintf(chave, "%s!%s", ident, nome_base);
      pos = exc_rec(ace_dnum, ace_inum, D2_ID, chave, !SO_INDEX);

      if ( pos == 0 )
         numdebug(561);

   }


   ret = 0;

fim:

   return(ret);
}

