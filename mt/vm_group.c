
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <alloc.h>

#include  "vm.h"

/***
Inicialiaza os GROUPs.
***/

VM_HND  gr_start( )
{
   US     I;
   Group  *G;

   VM_erro = 0;

   for ( I = 0;   I < GR_QTD;   I++ )
   {
      G = &VM_group[ I ];

      G->livre = 0;   /*** Lista Vazia ***/
      G->qtd   = 0;
      G->mem   = NULL;
   }

   return( 1 );
}

/***
Finaliza liberando incondicionalmente os GROUPs.
***/

VM_HND  gr_end( )
{
   US     I;

   VM_erro = 0;

   for ( I = 0;   I < GR_QTD;   I++ )
   {
      gr_free( &VM_group[ I ] );
   }

   return( 1 );
}

/***
Aloca memoria para um Group.
Retorna ZERO em caso de ERRO e VM_erro fica com:
... VM_ER25 - Erro de memoria Grupo nao consegue ser inicializado.
***/

VM_HND  gr_alloc( G )

Group   *G;
{
   US      J;
   Handle  *H;

   if ((G->mem = malloc(GR_HND * HD_SIZE)) == NULL )  RET_ER( VM_ER25 );

   for ( J = 0, H = (Handle *) G->mem;   J < GR_HND;   J++ )
   {
      /*** Em principio vamos Liberar todos os Handles e ... ***/
      /*** ... Encadear a Lista, some "+1" para fugir do ZERO ***/
      (H + J)->page = 0;
      (H + J)->off  = J + 2;
   }

   /*** Feche o Final da Lista ***/
   (H + J - 1)->off = 0;

   /*** Aponte o Inicio da Lista com a representacao externa "+1" ***/
   G->livre = 1;

   /*** Quantidade Maxima de Handles Livres ***/
   G->qtd = GR_HND;

   return( 1 );
}

/***
Libera um grupo.
***/

VM_HND  gr_free( G )

Group  *G;
{
   VM_erro = 0;

   if ( G->mem != NULL )   free((SC *) G->mem );

   G->livre = 0;
   G->qtd   = 0;
   G->mem   = NULL;

   return( 1 );
}

/***
Aloca um Handle livre.
Retorna o numero do Handle ou ...
... ZERO em caso de ERRO e VM_erro fica com:
... VM_ER26 - Interno - Grupo indica handle livre mas nao tem.
... VM_ER27 - Nao existe mais handle livres
***/

VM_HND  hd_alloc( )
{
   US      I;
   Group   *G;
   Handle  *H;
   VM_HND  ret;

   VM_erro = 0;

   for ( I = 0;   I < GR_QTD;   I++ )
   {
      G = &VM_group[ I ];

      if ( G->mem == NULL  &&  gr_alloc( G ) == 0 )   RET_ER( VM_erro);

      /*** Area ja' esta' alocada e inicializada ***/
      if ((ret = G->livre) != 0 )
      {
         /*** Transforme para representacao interna ***/
         ret--;

         H = (Handle *) G->mem;

         /*** Confirme se o handle esta' Livre ***/
         if ((H + ret)->page )   RET_ER( VM_ER26 );

         /*** Pegue o proximo Livre ***/
         G->livre = (H + ret)->off;

         /*** Decremente o numero de Handles Livres ***/
         G->qtd--;

         /*** Retorne o Handle disponivel - Some +1 ***/
         return((I * GR_HND + ret) + 1 );
      }
   }

   RET_ER( VM_ER27 );
}

/***
Libera um Handle ja ocupado.
Recebe o HANDLE como parametro.
Retorna o numero do Handle ou ...
... ZERO em caso de ERRO e VM_erro fica com:
... VM_ER28 - Handle maior que GR_QTD
... VM_ER31 - GROUP->END nao esta alocado
... VM_ER59 - Handle == ZERO
... VM_ER65 - Handle ja' esta' LIVRE
***/

VM_HND  hd_free( handle )

VM_HND  handle;
{
   US      I, J;
   Group   *G;
   Handle  *H;

   VM_erro = 0;

   /*** Teste e converta o Handle para iniciar de ZERO ***/
   if ( handle-- == 0 )   RET_ER( VM_ER59 );

   I = handle / GR_HND;
   J = handle % GR_HND;

   if ( I >= GR_QTD )   RET_ER( VM_ER28 );

   G = &VM_group[ I ];

   if ((H = (Handle *) G->mem ) == NULL )   RET_ER( VM_ER31 );

   /*** Se ja esta' Livre retorne ERRO ***/
   if ((H + J)->page == 0 )   RET_ER( VM_ER65 );

   /*** Marque como Livre ***/
   (H + J)->page = 0;

   /*** Reaponte a Lista de Handles Livres ***/
   (H + J)->off = G->livre;
   G->livre = J + 1;

   /*** Em PRIMEIRO lugar incremente o numero de Handles Livres ***/
   /*** Se chegar ao limite e o GROUP for superior a VM_mingroup ... ***/
   /*** ... libere o GROUP ***/
   if ( ++G->qtd >= GR_HND  &&  I >= VM_mingroup )   gr_free( G );

   /*** Converta para o modo externo ***/
   return( ++handle );
}

/***
Disponibiliza o HANDLE->PAGE e HANDLE->OFFSET em uma estrutura HANDLE.
Recebe o HANDLE e um ponteiro para estrutura HANDLE como parametro.
Volta o numero do HANDLE fornecido em caso de sucesso OU ...
... ERRO em caso de ERRO e VM_erro fica com:
... VM_ER32 - Handle maior que GR_QTD
... VM_ER34 - GROUP->END nao esta alocado
... VM_ER60 - handle == ZERO
***/

VM_HND  hd_get( handle, page, off )

VM_HND  handle;
VM_PGN  *page;
VM_TAM  *off;
{
   US     I, J;
   Handle *H;

   VM_erro = 0;

   /*** Teste e converta o Handle para iniciar de ZERO ***/
   if ( handle-- == 0 )   RET_ER( VM_ER60 );

   I = handle / GR_HND;
   J = handle % GR_HND;

   if ( I >= GR_QTD )   RET_ER( VM_ER32 );

   if ((H = (Handle *) VM_group[ I ].mem) == NULL )   RET_ER( VM_ER34 );

   *off  = (H + J)->off;
   *page = (H + J)->page;

   /*** Reconverta para Handle externo ***/
   return( ++handle );
}

/***
Armazena em um handle as informacoes passadas com parametro.
Recebe o HANDLE e um ponteiro para estrutura HANDLE como parametro.
Volta o numero do HANDLE fornecido em caso de sucesso OU ...
... ZERO em caso de ERRO e VM_erro fica com:
... VM_ER35 - Handle maior que GR_QTD
... VM_ER37 - GROUP->END nao esta alocado
... VM_ER61 - Handle == ZERO
***/

VM_HND  hd_set( handle, page, off )

VM_HND  handle;
VM_PGN  page;
VM_TAM  off;
{
   US     I, J;
   Handle *H;

   VM_erro = 0;

   /*** Teste e converta o Handle para iniciar de ZERO ***/
   if ( handle-- == 0 )   RET_ER( VM_ER61 );

   I = handle / GR_HND;
   J = handle % GR_HND;

   if ( I >= GR_QTD )   RET_ER( VM_ER35 );

   if ((H = (Handle *) VM_group[ I ].mem) == NULL )   RET_ER( VM_ER37 );

   (H + J)->page = page;
   (H + J)->off  = off;

   /*** Reconverta para Handle externo ***/
   return( ++handle );
}


