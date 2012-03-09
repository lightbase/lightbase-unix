
#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>

#include   <vm.h>

#ifdef   MSDOS
#include   <dir.h>
#endif

#define SW_FNAME  40
static  SC sw_file[ SW_FNAME + 1 ];

/***
Inicializa os diversos modos de SWAP -> XMS, DISCO etc.
Retorna ZERO em caso de ERRO e VM_erro fica com:
   VM_ER43 -> Nao consegue nome de arquivo de SWAP
   VM_ER44 -> Nome de arquivo >= SW_FNAME
   VM_ER45 -> Nao consegue abrir arquivo de SWAP
***/

VM_TAM  sw_start( )
{
   SC     file[ 250 ];
   SC     *D;

   VM_erro = 0;

   VM_file = NULL;

   D = VM_dir;

   sprintf( file, "%s%cVMXXXXXX", D, DIRSEP );

   if ( mktemp( file ) == NULL )   RET_ER( VM_ER43 );

   if ( strlen( file )  >= SW_FNAME )   RET_ER( VM_ER44 );

   strcpy( sw_file, file );

   if ((VM_file = fopen( sw_file, MODO_BWR )) == NULL )   RET_ER( VM_ER45 );

   return( 1 );
}

/***
Encerra os diversos modos de SWAP -> XMS, DISCO etc.
Retorna 1.
***/

VM_TAM  sw_end( )
{
   VM_erro = 0;

   fclose( VM_file );

   unlink( sw_file );

   return( 1 );
}

/***
Retira uma pagina da memoria e coloca em XMS, DISCO etc.
Recebe a PAGINA como parametro.
Retorna para aonde foi o SWAP (PG_DSK, PG_XMS)  em sucesso OU ...
... ZERO em caso de ERRO e VM_erro fica com:
    VM_ER46 -> PAGINA nao esta' em MEMORIA.
    VM_ER47 -> ERRO FATAL DE GRAVACAO NO ARQUIVO DE SWAP
***/

VM_TAM  sw_out( slot )

Slot  *slot;
{
   SL      off;
   Page  *page;

   VM_erro = 0;

   /*** Converta para Pagina Interna "-1" ***/
   page = &VM_page[ slot->page - 1 ];

   if ( ! MEMORIA( page->loc ) )   RET_ER( VM_ER46 );

#ifdef   NAODEF
   /*** Veremos se existe vaga em XMS ***/
   if ( XMS ) {     }

   /*** Bem, apenas restou o DISCO ***/
   else
#endif
   {
      /*** Converta a Pagina para representacao Interna "-1" ***/
      off = (SL) (slot->page - 1) * (SL) PG_TAM;

      fseek( VM_file, off, 0 );

      fwrite((SC *) slot->mem, PG_TAM, 1, VM_file );

      if ( ferror( VM_file ) )   RET_ER( VM_ER47 );

      page->loc = PG_DSK;
      page->mem = (SC huge *) off;
   }

   return( PG_LOC( page->loc ) );
}

/***
SWAP_IN - Coloca uma pagina que esta em PG_DSK, PG_XMS em um SLOT.
Recebe a PAGINA e o SLOT como parametro.
Retorna ZERO em caso de ERRO e VM_erro fica com:
    VM_ER48 -> SLOT->MEM == NULL.
    VM_ER49 -> ERRO FATAL DE LEITURA NO ARQUIVO DE SWAP
    VM_ER62 -> Nao sabemos aonde esta' a PAGINA
***/

VM_TAM  sw_in( slot, page )

Slot  *slot;
Page  *page;
{
   SL      off;

   if ( slot->mem == NULL )   RET_ER( VM_ER48 );

#ifdef   NAODEF
   /*** Sera que a PAGINA esta' em XMS ***/
   if ( XMS( page->loc ) ) {  return( 1 );  }
   else
#endif

   if ( DISCO( page->loc ) )
   {
      off = (SL) page->mem;

      fseek( VM_file, off, 0 );

      fread((SC *) slot->mem, PG_TAM, 1, VM_file );

      if ( ferror( VM_file ) )   RET_ER( VM_ER49 );

      page->loc = PG_MEM;
      page->mem = slot->mem;

      return( 1 );
   }

   /*** Nao sabemos aonde esta' a PAGINA ***/
   RET_ER( VM_ER62 );
}


