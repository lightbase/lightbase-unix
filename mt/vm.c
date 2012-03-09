
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <alloc.h>

#include  "vm.h"

#ifdef    MSDOS
#include  <alloc.h>
#endif

UC        VM_maxslot  = SL_QTD;    /* Quant. Maxima de SLOTs ***/
UC        VM_minslot  = 4;         /* Quant. Minima de SLOTs ***/
UC        VM_mingroup = 4;         /* Quant. Minima de HANDLEs ***/
VM_RES    VM_res      = 10;        /* Reserva Default */
SL        VM_coreleft = 120000L;   /* Reserva de memoria para alocar SLOT */
SC       *VM_dir = ".";            /* Diretorio do arquivo do VM */

/***
Inicializa MEMORIA VIRTUAL.
Retorna ZERO em caso de erro e VM_erro fica com :
***/

VM_HND  vm_start( )
{
   SS   I;

   VM_erro = 0;

   if ( VM_minslot  < 3 )            VM_minslot  = 3;
   if ( VM_minslot  > SL_QTD )       VM_minslot  = SL_QTD;

   if ( VM_maxslot  < VM_minslot )   VM_maxslot  = VM_minslot;

   if ( VM_mingroup < 1 )            VM_mingroup = 1;
   if ( VM_mingroup > GR_QTD )       VM_mingroup = GR_QTD;

   /*** Inicialize as Paginas ***/
   if ( pg_start( ) == 0 )   RET_ER( VM_erro );

   /*** Inicialize os Groupos ***/
   if ( gr_start( ) == 0 )   RET_ER( VM_erro );

   /*** Inicialize os Slots ***/
   if ( sl_start( ) == 0 )   RET_ER( VM_erro );

   /*** Inicialize areas de SWAP ***/
   if ( sw_start( ) == 0 )   RET_ER( VM_erro );

   /*** Alloc os primeiros Slots e Paginas ***/
   for ( VM_slqtd = I = 0;   I < VM_minslot;   I++ )
   {
      Page   *P;
      Slot   *S;

      P = &VM_page[ I ];

      /*** Alloc memoria ***/
      if ( pg_alloc( P, NULL ) == 0 )   RET_ER( VM_erro );

      /*** Vincule as Paginas aos SLOTs ***/
      S = &VM_slot[ I ];

      S->page = I + 1; /* As Paginas possuem numero externo */
      S->lru  = I + 1; /* Encadeie LRU/MRU */
      S->mem  = P->mem;

      /*** Sinalize a quantidade de Slots Disponiveis ***/
      VM_slqtd++;
   }

   /*** Alloc os primeiros Groups ***/
   for ( I = 0;   I < VM_mingroup;   I++ )
   {
      if ( gr_alloc( &VM_group[ I ] ) == 0 )   RET_ER( VM_erro);
   }

   return( 1 );
}

/***
Finaliza MEMORIA VIRTUAL.
Retorna ZERO em caso de erro e VM_erro fica com :
***/

VM_HND  vm_end( )
{
   VM_erro = 0;

   /*** Finalize areas de SWAP ***/
   sw_end( );

   /*** Finalize Slots ***/
   sl_end( );

   /*** Finalize Groupos ***/
   gr_end( );

   return( 1 );
}

/***
Procura nas estruturas PAGINA, uma que tenha disponibilidade de area.
Primeiro nas paginas que estao nos SLOTs, depois, em qualquer uma ...
... que possua espaco ou que esteja DESABILITADA.
Retorna o numero da pagina OU ...
... ZERO em caso de erro e VM_erro fica com:
    VM_ER38 38 - Nenhuma pagina com area disponivel
***/

VM_HND  vm_pgdisp( tam )

VM_TAM  tam;
{
   VM_PGN    I;
   Page      *P;
   Slot      *S;

   VM_erro = 0;

   /*** Veja se alguma pagina que esteja nos SLOTs possui area ***/
   for ( I = 0;   I < VM_slqtd;   I++ )
   {
      S = &VM_slot[ I ];

      if ( S->lru != 0  &&  VM_page[ S->page - 1 ].maior >= tam )
      {
         return( S->page );
      }
   }

   /*** Agora iremos procurar uma PAGINA que possua espaco ... ***/
   /*** ... OU que esteja desabilitada ***/
   for ( I = 0;   I < PG_QTD;   I++ )
   {
      P = &VM_page[ I ];

      /*** Retorne a Pagina externa "+1" ***/
      if ( DESABILIT( P->loc )  ||  P->maior >= tam )   return( I + 1 );
   }

   RET_ER( VM_ER38 );
}

/***
Habilita uma PAGINA em um dos SLOTs.
Verifica se ja' esta' habilitado, se nao pega o LRU.
Inicializa, se necessario, o SLOT e a PAGINA.
Executa sw_out() e sw_in() se for preciso.
Retorna o numero do SLOT OU ...
... ZERO em caso de erro e VM_erro fica com:
    VM_ER40 -> Interno, Slot->mem != Page->mem
    VM_ER41 -> Interno, PAGINA esta em memoria mas nao esta nos SLOTS
    VM_ER42 -> Erro alocando memoria para um SLOT
***/

VM_HND  vm_pgslot( page )

VM_PGN  page;
{
   VM_PGN    I, lru, slot;
   Page      *P;
   Slot      *S;

   VM_erro = 0;

   /*** Converta para representacao Interna ***/
   P = &VM_page[ page - 1 ];

   /*** Veja se a PAGINA esta' em memoria e em qual SLOT ***/
   if ( MEMORIA( P->loc ) )
   {
      /*** Esta' em memoria DEVE estar em algum SLOT, se nao: ERRO ***/
      for ( I = 0;   I < VM_slqtd;   I++ )
      {
         S = &VM_slot[ I ];

         if ( S->page == page )
         {
            /*** Encontrou o SLOT verifique integridade ***/
            if ( S->mem != P->mem )   RET_ER( VM_ER40 );

            /*** Atualize o SLOT de acordo com MRU / LRU ***/
            /*** Converta para representacao Externa ***/
            return( sl_mru( I + 1 ) );
         }
      }

      RET_ER( VM_ER41 );
   }

   /*** Procure algum SLOT vazio ou o LRU ***/
   for ( slot = 0, lru = 0, I = 0;   I < VM_maxslot;   I++ )
   {
      SS  memoria = 1;

      S = &VM_slot[ I ];

      /*** Esta' VAZIO ? ***/
      if ( S->lru == 0 )
      {
         /*** Se o SLOT esta vazio nao podemos testar o LRU ***/
         /*** Teste se ja tentamos alocar memoria ***/
#ifdef   MSDOS
         /*** No MSDOS testaremos a espaco em memoria ... ***/
         /*** ... para que nao a esgotemos ***/
         if ( memoria  &&  farcoreleft( ) > VM_coreleft )
#else
         if ( memoria )
#endif
         {
            if ((S->mem = malloc( PG_TAM )) != NULL )
            {
               /*** Mais um Slot foi alocado - Incremente "VM_slqtd" ***/
               VM_slqtd++;
               slot = I;
               break;
            }
         }

         /*** Se chegar ate aqui a memoria esta cheia ***/
         /*** Nao podemos testar o LRU, portanto continue ***/
         memoria = 0;
         continue;
      }

      /*** E' o LRU ? ***/
      if ( S->lru == VM_maxslot ) {  slot = I;  break;  }

      /*** Atualize o LRU ***/
      if ( S->lru > lru ) {  lru = S->lru;   slot = I;  }
   }

   /*** "slot" possui o primeiro vazio ou o LRU ***/
   S = &VM_slot[ slot ];

   /*** Se "slot" estiver com alguma pagina, temos que "swap it out" ***/
   if ( S->lru != 0 ) {  if ( sw_out( S ) == 0 )   RET_ER( VM_erro );  }

   /*** Se a PAGINA nao estiver inicializada, inicialize. ***/
   /*** S->mem ja tem area e depois de pg_alloc() P->mem == S->mem. ***/
   if ( DESABILIT( P->loc ) )
   {
      if ( pg_alloc( P, S->mem ) == 0 )   RET_ER( VM_erro);
   }

   /*** CUIDADO ***/
   /*** O proximo "else" pode ficar errado por causa da macro RET_ER() ***/
   /*** Coloque a PAGINA neste SLOT ***/
   else if ( sw_in( S, P ) == 0 ) {   RET_ER( VM_erro );  }

   /*** Atualize o SLOT de acordo com a PAGINA ***/
   S->page = page;

   /*** Atualize o SLOT de acordo com MRU / LRU ***/
   /*** Representacao externa "+1" ***/
   return( sl_mru( slot + 1 ) );
}

/***
Alloca uma area na Memoria Virtual.
Recebe como parametro a quantidade de bytes requerida.
Retorna o numero do HANDLE em caso de sucesso OU ...
... ZERO em caso de erro e VM_erro fica com :
    VM_ER39 -> Area > BL_MAX
***/

VM_HND  vm_alloc( tam )

VM_TAM  tam;
{
   VM_HND   hand;
   VM_TAM   off, page, total;
   Page     *P;

   VM_erro = 0;

   if ( tam + VM_res  >  BL_MAX )   RET_ER( VM_ER39 );

   total = tam + VM_res;

   /*** Ache um Handle LIVRE ***/
   if ((hand = hd_alloc( ))  ==  0 )   RET_ER( VM_erro );

   /*** Procure uma pagina que possua espaco suficiente ***/
   if ((page = vm_pgdisp( total ))  ==  0 )   RET_ER( VM_erro );

   /*** Habilite a PAGINA com espaco em um dos SLOTs ***/
   if ( vm_pgslot( page )  ==  0 )   RET_ER( VM_erro );

   /*** Converta para PAGINA interna "-1" ***/
   P = &VM_page[ page - 1 ];

   /*** Alloc o BLOCO na PAGINA ***/
   if ((off = bl_alloc( P, tam, VM_res ))  ==  0 )   RET_ER( VM_erro );

   /*** Atualize o HANDLE ***/
   if ( hd_set( hand, page, off )  ==  0 )   RET_ER( VM_erro );

   return( hand );
}

/***
Quando fizer esta funcao, talvez precise dar LOCK dentro de vm_realloc().
Libera um SLOT para paginacao.
***/

VM_HND  vm_unlock( handle )

VM_HND  handle;
{
   return( handle );
}

/***
Dereferencia um HANDLE.
Recebe o HANDLE como parametro.
Retorna um PONTEIRO para MEMORIA REAL em caso de sucesso OU ...
... NULL em caso de erro e VM_erro fica com :
    VM_ER50 -> A PAGINA esta' desabilitada.
    VM_ER51 -> O BLOCO nao confere SENHA.
    VM_ER52 -> O BLOCO esta' LIVRE.
    VM_ER53 -> O HANDLE esta' LIVRE
***/

SC      *vm_deref( handle, lock )

VM_HND  handle;
SS      lock;
{
   VM_PGN  page;
   VM_TAM  off;
   Page    *P;
   Bloco   *B;
   SC huge *base;

   lock = lock;

   VM_erro = 0;

   /*** Ache a PAGINA e OFFSET deste HANDLE ***/
   if ( hd_get( handle, &page, &off )  ==  0 )   return( NULL );

   /*** O handle esta' LIVRE, retorne ***/
   if ( off == (VM_TAM) 0 ) {  VM_erro = VM_ER53;  return( NULL ); }

   /*** Converta para representacao Interna ***/
   P = &VM_page[ page - 1 ];

   /*** Se a PAGINA estiver desabilitada retorne, pois se nao ... ***/
   /*** ... vm_pgslot() iria inicializa-la. ***/
   if ( DESABILIT( P->loc ) ) {  VM_erro = VM_ER50;  return( NULL ); }

   /*** Habilite a PAGINA com espaco em um dos SLOTs ***/
   if ( vm_pgslot( page )  ==  0 )   return( NULL );

   base = (SC huge *) P->mem + off - BL_SIZE;
   B    = (Bloco *)   base;

   /*** Verifique se o bloco existe ***/
   if ( B->senha != BL_SENHA )    {  VM_erro = VM_ER51;  return( NULL ); }

   /*** Verifique se esta ocupado ***/
   if ( DISPONIVEL( B->status ) ) {  VM_erro = VM_ER52;  return( NULL ); }

   /*** Retorne a area de DADOS do BLOCO ***/
   return((SC *)((SC huge *) base + BL_SIZE) );
}

/***
Libera um HANDLE.
Recebe o HANDLE como parametro.
Retorna o HANDLE livre em caso de sucesso OU ...
... ZERO em caso de erro e VM_erro fica com :
    VM_ER63 -> A PAGINA esta' desabilitada.
    VM_ER64 -> O HANDLE esta' livre.
***/

VM_HND  vm_free( handle )

VM_HND  handle;
{
   VM_PGN  page;
   VM_TAM  off;
   Page    *P;

   VM_erro = 0;

   /*** Ache a PAGINA e OFFSET deste HANDLE ***/
   if ( hd_get( handle, &page, &off )  ==  0 )   RET_ER( VM_erro )

   /*** O handle esta' LIVRE, retorne ***/
   if ( off == (VM_TAM) 0 )   RET_ER( VM_ER64 );

   /*** Converta para Pagina Interna "-1" ***/
   P = &VM_page[ page - 1 ];

   /*** Se a PAGINA estiver desabilitada retorne, pois se nao ... ***/
   /*** ... vm_pgslot() iria inicializa-la. ***/
   if ( DESABILIT( P->loc ) )   RET_ER( VM_ER63 );

   /*** Habilite a PAGINA com espaco em um dos SLOTs ***/
   if ( vm_pgslot( page )  ==  0 )   RET_ER( VM_erro );

   /*** Libere o BLOCO ***/
   if ( bl_free( P, off ) == 0 )   RET_ER( VM_erro );

   /*** Libere o HANDLE ***/
   if ( hd_free( handle )  ==  0 )   RET_ER( VM_erro );

   /*** Verifique se a pagina esta completamente vazia ... ***/
   /*** ... se for o caso, LIBERE tanto a PAGINA quanto o SLOT ***/
   if ( P->maior == BL_MAX )
   {
      US     I;
      Slot  *S;

      /*** Libere apenas o que for maior que VM_minslot ***/
      for ( I = VM_minslot;   I < VM_slqtd;   I++ )
      {
         S = &VM_slot[ I ];

         /*** E' este o SLOT que contem esta Pagina ? ***/
         /*** Verifique integridade ***/
         if ( S->page == page  &&  S->mem == P->mem )
         {
            if ( pg_free( P ) == 0 )   RET_ER( VM_erro );

            /*** Retire da lista de MRU ***/
            sl_lru( I + 1 );

            /*** Libere o SLOT ***/
            S->page = 0;
            S->lru  = 0;
            S->mem  = NULL;
         }
      }
   }

   return( handle );
}

/***
Realloca uma area na Memoria Virtual.
Recebe como parametro o HANDLE e a quantidade de bytes requerida.
Caso o HANDLE esteja livre retorna vm_alloc( tam ).
Retorna o numero do HANDLE em caso de sucesso OU ...
... ZERO em caso de erro e VM_erro fica com :
    VM_ER54 -> Area > BL_MAX
    VM_ER55 -> Pagina Desabilitada
***/

VM_HND  vm_realloc( handle, tam )

VM_HND  handle;
VM_TAM  tam;
{
   SC      *SO, *SD;
   Page    *P;
   VM_PGN  page;
   VM_TAM  off, TO;
   VM_HND  hand;

   VM_erro = 0;

   if ( tam  >  BL_MAX )   RET_ER( VM_ER54 );

   /*** Se tamanho for ZERO - Libere o Handle ***/
   if ( tam == 0 )   return( vm_free( handle ) );

   /*** Se Handle ZERO - Retorne vm_alloc() ***/
   if ( handle == 0 )   return( vm_alloc( tam ) );

   /*** Ache a PAGINA e OFFSET deste HANDLE ***/
   if ( hd_get( handle, &page, &off )  ==  0 )   RET_ER( VM_erro );

   /*** O handle esta' LIVRE, retorne vm_alloc() ***/
   if ( off == 0 )   return( vm_alloc( tam ) );

   /*** Converta para Pagina Interna "-1" ***/
   P = &VM_page[ page - 1 ];

   /*** Se a PAGINA estiver desabilitada retorne, pois se nao ... ***/
   /*** ... vm_pgslot() iria inicializa-la. ***/
   if ( DESABILIT( P->loc ) )   RET_ER( VM_ER55 );

   /*** Habilite a PAGINA com espaco em um dos SLOTs ***/
   if ( vm_pgslot( page )  ==  0 )   RET_ER( VM_erro );

   /*** Tente allocar no proprio BLOCO com a sua reserva ... ***/
   /*** ... ou com o proximo BLOCO, se este for LIVRE. ***/
   if ( bl_realloc( P, off, tam )  !=  0 )
   {
      /*** O MELHOR aconteceu, retorne o proprio HANDLE ***/
      return( handle );
   }

   /*** Bem, temos alocar outro handle, para mover. ***/
   /*** Precisamos do tamanho do bloco atual. ***/
   if ( bl_tamreal( P, off, &TO )  ==  0 )   RET_ER( VM_erro );

   /*** Requesite outro HANDLE ***/
   if ((hand = vm_alloc( tam ))  ==  0 )   RET_ER( VM_erro );

   /*** Dereferencie os BLOCOs - O Antigo primeiro ***/
   if ((SO = vm_deref( handle, 0 ))  ==  NULL )   RET_ER( VM_erro );
   if ((SD = vm_deref( hand,   0 ))  ==  NULL )   RET_ER( VM_erro );

   /*** Copie o conteudo do Handle antigo para o novo ***/
   /*** Cuidado para nao invadir area ***/
   if ( TO > tam )  TO = tam;
   memmove( SD, SO, TO );

   /*** Libere o HANDLE antigo ***/
   vm_free( handle );

   return( hand );
}


