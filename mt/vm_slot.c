
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <alloc.h>

#include  "vm.h"

/***
Inicializa os SLOTs.
Retorna sempre 1;
***/

VM_PGN   sl_start( )
{
   SS    I;
   Slot  *S;

   VM_erro = 0;

   for ( I = 0;   I < SL_QTD;   I++ )
   {
      S = &VM_slot[ I ];

      S->page = 0;
      S->lru  = 0;
      S->mem  = NULL;
   }

   VM_slqtd = 0;

   return( 1 );
}

/***
Finaliza as estruturas os SLOTs.
Retorna 1 pois nao ha erros.
***/

VM_PGN   sl_end( )

{
   SS    I;
   Slot  *S;

   VM_erro = 0;

   for ( I = 0;   I < SL_QTD;   I++ )
   {
      S = &VM_slot[ I ];

      if ( S->lru != 0 && S->mem != NULL )   free((SC *) S->mem );
   }

   return( 0 );
}

/***
Atualiza o SLOT fornecido para ser o MRU.
Retorna o SLOT.
***/

VM_PGN  sl_mru( slot )

VM_PGN  slot;
{
   VM_PGN  I, mru;
   Slot    *S1, *S2;

   VM_erro = 0;

   /*** Converta para Representacao Interna "-1" ***/
   S1 = &VM_slot[ slot - 1 ];

   /*** Se ja' for o MRU nao precisa mudar nada ***/
   if ((mru = S1->lru) == 1 )   return( slot );

   if ( mru == 0 )   mru = VM_maxslot;

   for ( I = 0;   I < VM_maxslot;   I++ )
   {
      S2 = &VM_slot[ I ];

      if ( S2->lru == 0  ||  S2->lru >= mru )   continue;

      S2->lru++;
   }

   S1->lru = 1;

   return( slot );
}

/***
Atualiza o SLOT fornecido para ser o LRU.
Retorna o SLOT.
***/

VM_PGN  sl_lru( slot )

VM_PGN  slot;
{
   VM_PGN  I, lru, max;
   Slot    *S1, *S2;

   VM_erro = 0;

   /*** Converta para Representacao Interna "-1" ***/
   S1 = &VM_slot[ slot - 1 ];

   /*** Se ja' for o LRU nao precisa mudar nada ***/
   if ((lru = S1->lru) >= VM_maxslot  ||  lru == 0 )   return( slot );

   for ( I = 0, max = 0;   I < VM_maxslot;   I++ )
   {
      S2 = &VM_slot[ I ];

      /*** Guarde o maximo ***/
      if ( max < S2->lru )   max = S2->lru;

      if ( S2->lru == 0  ||  S2->lru <= lru )   continue;
      
      S2->lru--;
   }

   S1->lru = max;

   return( slot );
}

