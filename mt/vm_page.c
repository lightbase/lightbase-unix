
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <alloc.h>

#define   VM_VARS
#include  "vm.h"
#undef    VM_VARS

#define   BL_NAOJUNTOU   1
#define   BL_JUNTOU      2

/***
Inicializa as estruturas PAGEs.
Retorna 1 pois nao ha erros.
***/

VM_PGN  pg_start( )

{
   VM_PGN    I;
   Page      *P;

   VM_erro = 0;

   for ( I = 0;   I < PG_QTD;   I++ )
   {
      P = &VM_page[ I ];

      P->maior = (VM_TAM) 0;
      P->livre = (VM_TAM) 0;
      P->loc   = PG_DES;
      P->mem   = NULL;
   }

   return( I );
}

#ifdef   NAODEF
Nem pg_end() nao e necessario, pois ...
... sl_end() liberara' toda memoria.
/***
Finaliza as estruturas PAGEs.
Retorna 1 pois nao ha erros.
***/

VM_PGN  pg_end( )

{
   VM_PGN    I;
   Page      *P;

   VM_erro = 0;

   for ( I = 0;   I < PG_QTD;   I++ )
   {
      P = &VM_page[ I ];

      if ( MEMORIA( P->loc ) && P->mem != NULL )   free( P->mem );
   }

   return( I );
}

#endif /* NAODEF */

/***
Libera uma pagina em memoria. ATENCAO: NAO se preocupa com os BLOCOS.
Necessita da pagina em memoria
Recebe como parametro a pagina.
Retorna ZERO em caso de erro e VM_erro fica com :
    VM_ER2  -> Pagina nao esta em memoria.
***/

VM_PGN  pg_free( page )

Page  *page;
{
   VM_erro = 0;

   if ( ! MEMORIA( page->loc )  ||  page->mem == NULL )   RET_ER( VM_ER2 );

   free( page->mem );

   page->maior = (VM_TAM) 0;
   page->livre = (VM_TAM) 0;
   page->loc   = PG_DES;
   page->mem   = NULL;

   return( 1 );
}

/***
Inicializa uma pagina em memoria.
Necessita da pagina em memoria
Recebe como parametro a pagina, e tamanho.
Retorna ZERO em caso de erro e VM_erro fica com :
    VM_ER1 -> Nao conseguio alocar a Pagina em memoria.
***/

VM_PGN  pg_alloc( page, mem )

Page    *page;
SC huge *mem;
{
   Bloco   *B1;

   VM_erro = 0;

   /*** So alloc memoria se for preciso ***/
   if ( mem == NULL && (mem = malloc( PG_TAM )) == NULL )   RET_ER( VM_ER1 );

   B1 = (Bloco *) mem;

   B1->senha  = BL_SENHA;
   B1->tam    = BL_MAX;
   B1->status = BL_STSTAT( BL_LIVRE ) + 0;

   page->maior = B1->tam;
   page->livre = 0;
   page->loc   = PG_MEM;
   page->mem   = mem;

   /*** Inicialize a Lista de disponiveis com o primeiro Bloco ***/
   if ( pg_ultimo( page, B1 )  ==  0 )   RET_ER( VM_erro );
   
   return( 1 );
}

/***
Atualiza cadeia de espacos LIVRES da PAGINA.
Recebe uma estrutura PAGE e uma estrutura BLOCO.
Retorna o ultimo LIVRE.
***/

VM_TAM pg_ultimo( P, B )

Page  *P;
Bloco *B;
{
   VM_TAM  *ubl;
   SC huge *base;
   
   base = (SC huge *) B + BL_SIZE;
   ubl  = (VM_TAM * ) base;
   *ubl = P->livre;
   
   return( P->livre = base - (SC huge *) P->mem );
}

/***
Retira um BLOCO LIVRE da LISTA.
Retorna ZERO em caso de erro e VM_erro fica com:
   VM_ER69 - Bloco nao Inicializado
   VM_ER70 - Bloco nao Disponivel
   VM_ER71 - Bloco nao Encontrado
***/

VM_TAM  pg_retira( page, retira )

Page    *page;
Bloco   *retira;
{
   SS      achou = 0;
   VM_TAM  *pbr, obr; /*** Offset do Bloco a ser retirado ***/
   VM_TAM  *pbl, obl;
   Bloco   *B;
   SC huge *base;

   VM_erro = 0;

   /*** Pegue o OFFSET do Bloco Fornecido ***/
   base = (SC huge *) retira + BL_SIZE;
   obr  = base - page->mem;
   
   /*** Percorreremos a lista de bloco livres. ***/
   /*** Pegue o primeiro da lista ***/
   pbl = &page->livre;
   obl = *pbl;

   /*** Se a lista encerrou ***/
   while ( obl != 0 )
   {
      /*** Sera' este o Bloco que desejamos retirar ? ***/
      if ( obl == obr )
      {
         /*** Quem apontou para este Bloco na verdade devera' ... ***/
         /*** ... apontar para onde este aponta ! ***/
         base = page->mem + obr;
         pbr  = (VM_TAM *) base;
         *pbl = *pbr;
         achou++;
         break;
      }
      
      base = page->mem + obl - BL_SIZE;
      B    = (Bloco *)   base;
   
      if ( B->senha != BL_SENHA )   RET_ER( VM_ER69 );

      if ( ! DISPONIVEL( B->status ) )   RET_ER( VM_ER70 );
      
      base = (SC huge *) B + BL_SIZE;
      
      pbl  = (VM_TAM  *) base;
      obl  = *pbl;
   }

   if ( ! achou )   RET_ER( VM_ER71 );
   
   return( 1 );
}

/***
Coloca na lista de Blocos Livres ou junta com algum contiguo.
Atualiza "page->maior".
Retorna ZERO em caso de ERRO e VM_erro fica com:
   VM_ER66 - Bloco nao Inicializado
   VM_ER67 - Bloco nao esta LIVRE
***/

VM_TAM  pg_livre( page, livre )

Page    *page;
Bloco   *livre;
{
   VM_TAM  off, *pbl, *pvt, *plv, obl;
   VM_TAM  borda = 0;  /* 1 Posterior, 2 Anterior, 3 Ambas */
   Bloco   *B;
   SC huge *base, *ant, *pos;

   VM_erro = 0;

   /*** Guarde os limites para serem testados ***/
   ant = (SC huge *) livre;
   pos = (SC huge *) livre + livre->tam + BL_SIZE;
   
   /*** Percorreremos a lista de bloco livres. ***/
   /*** Veremos se o BLOCO recebido como parametro e' contiguo ... ***/
   /*** ... de outro BLOCO LIVRE, pois assim nao precisa entrar ... ***/
   /*** ... na LISTA. ***/
   /*** Se for preciso atualizaremos o maior BLOCO LIVRE da PAGINA. ***/

   /*** Pegue o primeiro da lista ***/
   pbl = &page->livre;
   off = *pbl;

   /*** Apenas para evitar Warnings. Pois "plv" apenas sera usado ... ***/
   /*** ... encontrarmos as duas bordas e a Posterior primeiro. ***/
   plv = pbl;
   
   /*** Se a lista encerrou ou se ja encontrou as duas Bordas - ENCERRE ***/
   while ( off != 0  &&  borda != 3 )
   {
      base = page->mem + off - BL_SIZE;
      B    = (Bloco *)   base;
   
      if ( B->senha != BL_SENHA )   RET_ER( VM_ER66 );

      if ( ! DISPONIVEL( B->status ) )   RET_ER( VM_ER67 );
      
      /*** Sera' que este Bloco e' o anterior ? ***/
      if ( base + B->tam + BL_SIZE  ==  ant )
      {
         /*** Vamos anexar "livre" a este Bloco ***/
         B->tam += livre->tam + BL_SIZE;
         
         livre->senha = 0;

         /*** Ja' encontrou o Bloco Posterior ? ***/
         if ( borda != 0 )
         {
            /*** Retire este Bloco da Lista ***/
            base = (SC huge *) B + BL_SIZE;
            pvt  = (VM_TAM  *) base;
            *pbl = *pvt;
         
            /*** Reaponte quem apontava para Livre ***/
            obl  = base - page->mem;
            *plv = obl;

            /*** Este Bloco devera' apontar para onde Livre apontava ***/
            base = (SC huge *) livre + BL_SIZE;
            pbl  = (VM_TAM  *) base;
            *pvt = *pbl;
         }

         /*** Atualize o novo Bloco Livre ***/
         livre = B;
         ant   = (SC huge *) B;
            
         /*** Marque o Bloco Anterior ***/
         borda |= 1;
      }

      /*** Sera' que este Bloco e' o Posterior ? ***/
      else if ( base == pos )
      {
         /*** Este Bloco devera' ser anexado a "livre" ***/
         livre->tam += B->tam + BL_SIZE;

         B->senha = 0;

         /*** Ja' encontrou o Bloco Anterior ? ***/
         if ( borda != 0 )
         {
            /*** Retire este Bloco da Lista ***/
            base = (SC huge *) B + BL_SIZE;
            pvt  = (VM_TAM  *) base;
            *pbl = *pvt;
         }
         
         else
         {
            /*** Devera' existir um ponteiro que aponte para Livre ... ***
            /*** ... que vai ser usado caso o Anterior seja encontrado. ***/
            plv = pbl;

            /*** Guarde para onde este Bloco aponta ***/
            base = (SC huge *) B + BL_SIZE;
            pvt  = (VM_TAM  *) base;
            off  = *pvt;
         
            /*** Temos que atualizar quem apontou para este Bloco ***/
            base = (SC huge *) livre + BL_SIZE;
            obl  = base - page->mem;
            *pbl = obl;
         
            /*** Atualize em Livre para onde este Bloco apontava ***/
            pvt  = (VM_TAM  *) base;
            *pvt = off;
         }
         
         /*** Atualize o Bloco corrente ***/
         B = livre;
         
         /*** Marque o Bloco Posterior ***/
         borda |= 2;
      }

      base = (SC huge *) B + BL_SIZE;
      pbl  = (VM_TAM  *) base;
      off  = *pbl;
   }

   /*** Se nao encotrou nenhuma borda coloque "livre" como o ultimo ***/
   if ( ! borda  &&  pg_ultimo( page, livre ) == 0 )   RET_ER( VM_erro );

   if ( livre->tam > page->maior )   page->maior = livre->tam;

   return( 1 );
}

/***
Procura em uma pagina o maior bloco livre.
Necessita da pagina em memoria
Recebe como parametro a pagina.
Retorna ZERO em caso de erro e VM_erro fica com :
    VM_ER3 -> Bloco nao inicializado.
    VM_ER4 -> Bloco nao disponivel, porem esta na lista dos LIVRES
***/

VM_TAM  pg_maior( page )

Page    *page;
{
   VM_TAM  off, *pbl;
   Bloco   *B;
   SC huge *base;

   VM_erro = 0;

   /*** Procuraremos o maior BLOCO LIVRE da PAGINA ... ***/
   /*** ... dentro da lista de bloco livres. ***/

   /*** Inicializemos o PAGE->MAIOR ***/
   page->maior = 0;

   off = page->livre;

   while( off != 0 )
   {
      base = page->mem + off - BL_SIZE;
      B    = (Bloco *)   base;
   
      if ( B->senha != BL_SENHA )   RET_ER( VM_ER3 );

      if ( ! DISPONIVEL( B->status ) )   RET_ER( VM_ER4 );
      
      if ( page->maior < B->tam )   page->maior = B->tam;

      base = (SC huge *) B + BL_SIZE;
      
      pbl  = (VM_TAM *) base;
      off  = *pbl;
   }

   return( 1 );
}

/***
Realoca um bloco na pagina informada.
Necessita da pagina em memoria
Recebe como parametro a pagina o offset do bloco e NOVO tamanho do bloco.
Retorna o OFFSET do bloco alocado em caso de sucesso OU ...
... ZERO em caso de erro e VM_erro fica com :
    VM_ER17 -> Pagina nao esta' em memoria.
    VM_ER18 -> Bloco fornecido nao inicializado.
    VM_ER19 -> Bloco posterior nao inicializado.
    VM_ER20 -> Bloco esta liberado.
    VM_ER21 -> Bloco lotado, nao consegue realocar.
    VM_ER22 -> Bloco lotado, nao consegue realocar.
***/

VM_TAM  bl_realloc( page, off, tam )

Page    *page;
VM_TAM  off;
VM_TAM  tam;
{
   VM_TAM  res, bl_tam, bl_res, total;
   Bloco   *B1, *B2;
   SC huge *base;

   VM_erro = 0;

   if ( ! MEMORIA( page->loc ) )   RET_ER( VM_ER17 );

   base = page->mem + off - BL_SIZE;
   B1   = (Bloco *)   base;

   if ( B1->senha != BL_SENHA )   RET_ER( VM_ER18 );

   if ( DISPONIVEL( B1->status )) RET_ER( VM_ER20 );

   bl_res = BL_RES( B1->status );
   bl_tam = B1->tam - bl_res;

   if ( bl_tam > tam )
   {
      /*** Nova area e' MENOR, vamos ver se cabe na reserva ***/
      res = (bl_tam - tam) + bl_res;

      if ( res <= RES_MAX )   B1->status = BL_STSTAT( BL_ALLOC ) + res;

      else
      {
         total      = tam + RES_MIN;
         total      = BL_ALIGN( total );
         B1->status = BL_STSTAT( BL_ALLOC ) + (total - tam);

         /*** Salve o tamanho anterior e acerte o novo tamanho ***/
         bl_tam  = B1->tam;
         B1->tam = total;

         /*** Vamos calcular o tamanho do novo bloco ***/
         bl_tam = (bl_tam - B1->tam) - BL_SIZE;

         base = (SC huge *) B1 + B1->tam + BL_SIZE;
         B2   = (Bloco *)   base;

         B2->senha  = BL_SENHA;
         B2->status = BL_STSTAT( BL_LIVRE ) + 0;
         B2->tam    = bl_tam;

         /*** Vamos ver se o proximo bloco esta' livre ***/
         if ( pg_livre( page, B2 )  ==  0 )   RET_ER( VM_erro );
      }
   }

   else if ( bl_tam < tam )
   {
      /*** Nova area e' MAIOR, Vamos ver se temos reserva ***/
      if ( B1->tam >= tam )
      {
         res = B1->tam - tam;
         B1->status = BL_STSTAT( BL_ALLOC ) + res;
      }

      else
      {
         /*** Nao temos reserva veremos se o proximo bloco esta livre ***/
         base = (SC huge *) B1 + B1->tam + BL_SIZE;
         B2   = (Bloco *)   base;

         /*** Se for o ultimo bloco, abandone ***/
         if ( base - page->mem  >=  PG_TAM )
         {
            RET_ER( VM_ER21 );
         }

         if ( B2->senha != BL_SENHA )   RET_ER( VM_ER19 );

         /*** Se proximo bloco nao estiver disponivel  OU  ... ***/
         /*** ... mesmo estando livre nao couber a nova area. Abandone! ***/
         total = B1->tam + B2->tam + BL_SIZE;
         if ( ! DISPONIVEL( B2->status )  ||  total < tam  )
         {
            RET_ER( VM_ER22 );
         }

         /*** Retire o Bloco da LISTA, vai ser usado ***/
         if ( pg_retira( page, B2 )  == 0 )   RET_ER( VM_erro );

         /*** Vamos juntar o proximo bloco ? ***/
         bl_tam = tam + BL_SIZE + BL_MIN + RES_MIN;
         if ( bl_tam > total )
         {
            /*** Area devera' ser incorporada, nao cabe outro bloco. ***/
            B1->tam    = total;
            B1->status = BL_STSTAT( BL_ALLOC ) + B1->tam - tam;
         }

         else
         {
            /*** Aproveitamos o necessario e faremos outro bloco ***/
            bl_tam     = tam + RES_MIN;
            B1->tam    = BL_ALIGN( bl_tam );
            B1->status = BL_STSTAT( BL_ALLOC ) + (B1->tam - tam);

            base = (SC huge *) B1 + B1->tam + BL_SIZE;
            B2   = (Bloco *)   base;

            B2->senha  = BL_SENHA;
            B2->status = BL_STSTAT( BL_LIVRE ) + 0;
            B2->tam    = total - (B1->tam + BL_SIZE);

            /*** Coloque este Bloco na LISTA ***/
            if ( pg_ultimo( page, B2 ) == 0 )    RET_ER( VM_erro );
         }

         /*** Atualize "page->maior" ***/
         if ( pg_maior( page )  ==  0 )   RET_ER( VM_erro );
      }
   }

   return( off );
}

/***
Alloca um bloco em uma determinada Pagina.
Necessita da pagina em memoria.
Aloca com reserva de no maximo 50 bytes.
Retorna o OFFSET + BL_SIZE do bloco alocado em caso de sucesso OU ...
... ZERO em caso de erro e VM_erro fica com :
    VM_ER8  -> Nao pode alocar bloco de tamanho ZERO.
    VM_ER9  -> Pagina nao esta' em memoria.
    VM_ER10 -> Nao tem espaco na pagina.
    VM_ER11 -> Bloco nao inicializado.
    VM_ER12 -> Interno - Bloco com espaco nao foi localizado.
    VM_ER68 -> Bloco nao esta LIVRE
***/

VM_TAM  bl_alloc( page, tam, reserva )

Page    *page;
VM_TAM  tam;
VM_RES  reserva;
{
   VM_TAM  ret, off, *pbl, total, parcial;
   VM_TAM  bl_tam;
   Bloco   *B1, *B2;
   SC huge *base;

   VM_erro = 0;

   if ( reserva > RES_MAX )   reserva = RES_MAX;

   if ( tam == 0 )   RET_ER( VM_ER8 );

   if ( ! MEMORIA( page->loc ) )   RET_ER( VM_ER9 );

   parcial  = tam + reserva;
   total    = BL_ALIGN( parcial );
   reserva += total - parcial;

   if ( page->maior <  total  )   RET_ER( VM_ER10 );

   off = page->livre;

   /*** Achar um bloco disponivel que seja >= "total" ***/
   while( 1 )
   {
      if ( off == 0 )   RET_ER( VM_ER12 );

      base = page->mem + off - BL_SIZE;
      B1   = (Bloco *)   base;

      if ( B1->senha != BL_SENHA )   RET_ER( VM_ER11 );

      if ( ! DISPONIVEL( B1->status ) )   RET_ER( VM_ER68 );

      /*** Se couber "bl_tam" sera' aproveitado mais embaixo ***/
      if ((bl_tam = B1->tam) >= total )   break;

      base = (SC huge *) B1 + BL_SIZE;
      pbl  = (VM_TAM *)  base;
      off  = *pbl;
   }

   /*** Bom, vamos usar este bloco LIVRE, retire-o da LISTA ***/
   if ( pg_retira( page, B1 )  ==  0 )   RET_ER( VM_erro );

   /*** B1->tam e' igual a "bl_tam" e' maior ou igual a "total" ***/
   /*** Vamos ver se cabe outro bloco ***/
   parcial = total + BL_SIZE + BL_MIN + RES_MIN;

   if ( parcial  >  bl_tam )
   {
      /*** Compense todo extra como reserva ***/
      reserva += bl_tam - total;

      /*** Nao cabe outro header com espaco suficiente ***/
      /*** Indique que nao vai ser preciso abrir outro bloco ***/
      total = bl_tam;
   }

   /*** Pelo sim pelo nao a reserva ja esta correta ***/
   B1->status = BL_STSTAT( BL_ALLOC ) + reserva;

   /*** Se for igual nao precisamos modificar o B1->tam ... ***/
   /*** ... nem abrir outro bloco. ***/
   if ( bl_tam != total )
   {
      parcial = bl_tam;
      B1->tam = total;

      base = (SC huge *) B1 + total + BL_SIZE;
      B2   = (Bloco *)   base;

      B2->senha  = BL_SENHA;
      B2->status = BL_STSTAT( BL_LIVRE ) + 0;
      B2->tam    = parcial - (total + BL_SIZE);

      if ( pg_ultimo( page, B2 )  ==  0 )   RET_ER( VM_erro );
   }

   /*** Atualize "page->maior" ***/
   if ( pg_maior( page )  ==  0 )   RET_ER( VM_erro );

   /*** Retorne o OFFSET ***/
   ret = (VM_TAM)((SC huge *) B1 - page->mem );
   
   /*** Retorne o OFFSET + HEADER para evitar o OFFSET ZERO ***/
   return( ret += BL_SIZE );
}

/***
Libera um bloco na pagina informada.
Necessita da pagina em memoria e o OFFSET do bloco na pagina.
Retorna o OFFSET do bloco liberado em caso de sucesso OU ...
... ZERO em caso de erro e VM_erro fica com :
    VM_ER14 -> Pagina nao esta' em memoria.
    VM_ER15 -> Bloco nao inicializado.
    VM_ER16 -> Bloco ja esta liberado.
***/

VM_TAM  bl_free( page, off )

Page    *page;
VM_TAM  off;
{
   Bloco   *B1;
   SC huge *base;

   VM_erro = 0;

   if ( ! MEMORIA( page->loc ) )   RET_ER( VM_ER14 );

   base = page->mem + off - BL_SIZE;
   B1   = (Bloco *)   base;

   if ( B1->senha != BL_SENHA )     RET_ER( VM_ER15 );

   if ( DISPONIVEL( B1->status ))   RET_ER( VM_ER16 );

   B1->status = BL_STSTAT( BL_LIVRE ) + 0;

   /*** Atualize "page->maior" ***/
   /*** Administre Blocos contiguos ***/
   /*** Coloque na lista de Blocos Livres ***/
   if ( pg_livre( page, B1 )  ==  0 )   RET_ER( VM_erro );

   return( off );
}

/***
Acha o tamanho real do bloco.
Retorna ZERO em caso de erro e VM_erro fica com :
    VM_ER56 -> Pagina nao esta' em memoria.
    VM_ER57 -> Bloco nao inicializado.
    VM_ER58 -> Bloco ja esta liberado.
***/

VM_TAM  bl_tamreal( page, off, tam )

Page    *page;
VM_TAM  off;
VM_TAM  *tam;
{
   Bloco   *B1;
   SC huge *base;

   VM_erro = 0;
   
   *tam = 0;

   if ( ! MEMORIA( page->loc ) )   RET_ER( VM_ER56 );

   base = page->mem + off - BL_SIZE;
   B1   = (Bloco *)   base;

   if ( B1->senha != BL_SENHA )     RET_ER( VM_ER57 );

   if ( DISPONIVEL( B1->status ))   RET_ER( VM_ER58 );

   *tam = B1->tam - BL_RES( B1->status );
   
   return( 1 );
}


#ifdef   PAGE_TEST

#include   <ctype.h>
#include   <conio.h>

SS      pega_tecla( void );
SS      pega_mostra( void );
SL      pega_inteiro( SC *S );
VM_TAM  set_reserva( void );
VM_TAM  aloca( Page * );
VM_TAM  realoca( Page * );
VM_TAM  libera( Page * );
VM_TAM  imprime( Page * );
SI      main( void );

SS    reserva;

SS pega_tecla( )
{
   SS   C;

   C = getch( );

   if ( C == 0 )   C = getch( );

   return( C );
}

SS pega_mostra( )
{
   SS   C;

   C = pega_tecla( );

   printf( "%c", C );

   return( C );
}

SL  pega_inteiro( S )

SC *S;
{
   SS   C;
   SL   I = 0;

   printf( "%s", S );

   while( 1 )
   {
      C = pega_tecla( );

      if ( C == '\n' || C == '\r' )   break;     /*** Enter ***/
      if ( C == '\b' )                I /= 10;   /*** Back Space ***/
      if ( C == 27 )                  I  = 0;    /*** Escape ***/
      if ( C < '0' || C > '9' )       continue;

      I *= 10;
      I += C - '0';
   }

   printf( "%d", I );
   return( I );
}

VM_TAM  set_reserva( )
{
   reserva = pega_inteiro( "Reserva: " );

   printf( "\n" );

   return( reserva );
}

VM_TAM  aloca( P )

Page *P;
{
   SL  I;
   SL  O;

   I = pega_inteiro( "Aloca - Tamanho: " );

   O = bl_alloc( P, I, reserva );

   printf( "  Offset = %ld \n", (SL) O );

   return( O );
}

VM_TAM  realoca( P )

Page *P;
{
   SL  I;
   SL  O;

   I = pega_inteiro( "Realoca - Offset : " );
   O = pega_inteiro( "  Tamanho: " );

   O = bl_realloc( P, I, O );

   printf( "  Offset = %ld\n", (SL) O );

   return( O );
}

VM_TAM  libera( P )

Page *P;
{
   SL   I;
   SL   O;

   I = pega_inteiro( "Libera - Offset: " );

   O = bl_free( P, I );

   printf( "  Offset = %ld \n", (SL) O );

   return( O );
}

VM_TAM  imprime( P )

Page *P;
{
   SL      I = 0;
   US      J = 0;
   Bloco   *B1;
   SC huge *base;

   printf( "Imprime:\n" );

   B1 = (Bloco *) P->mem;

   while( 1 )
   {
      J++;
      I += B1->tam + BL_SIZE;

      printf( "%d ", J );
      printf( "Off = %-5d ", (SC huge *) B1 - P->mem + BL_SIZE );
      printf( "Senha = %-4X ", B1->senha );
      printf( "Tam = %-5d ", B1->tam );
      printf( "Res = %-3d ", BL_RES(  B1->status ) );
      
      if ( ! BL_STAT( B1->status ) )
      {
         printf( "Livre = %d", *((VM_TAM *) ((SC huge *) B1 + BL_SIZE)) );
      }

      printf( "\n" );

      base = (SC huge *) B1 + B1->tam + BL_SIZE;
      B1   = (Bloco *)   base;

      if ( base >= P->mem + PG_TAM )   break;
   }

   printf( "Total = %ld\n", I );

   return( I );
}

SI   main( )
{
   SS     encerra = 0;
   Page   P;

   reserva = 7;

   if ( pg_alloc( &P, NULL ) == 0 )    return  VM_erro;

   bl_alloc( &P, 1000, reserva );
   bl_alloc( &P, 1000, reserva );
   bl_alloc( &P, 1000, reserva );
   bl_alloc( &P, 1000, reserva );
   bl_alloc( &P, 1000, reserva );
   bl_alloc( &P, 1000, reserva );
   
   while( ! encerra )
   {
      if ( VM_erro )
      {
         printf( "VM_erro = %d", VM_erro );
         pega_tecla( );
         VM_erro = 0;
      }

      printf( "\n" );
      printf( "Page.tam   = %d \n", PG_TAM );
      printf( "Page.maior = %d \n", P.maior );
      printf( "Page.livre = %d \n", P.livre );

      printf( "\n" );
      printf( "Escolha: \n" );
      printf( "   A - Aloca Bloco \n" );
      printf( "   R - Realoca Bloco \n" );
      printf( "   L - Libera Bloco \n" );
      printf( "   I - Imprime Blocos \n" );
      printf( "   P - Reserva padrao \n" );
      printf( "   F - Finaliza \n" );
      printf( "\n" );
      printf( "Opcao -> " );

pega:
      switch( toupper( pega_tecla( ) ) )
      {
      default :   goto pega;
      case 'A':   aloca(   &P );   break;
      case 'R':   realoca( &P );   break;
      case 'L':   libera(  &P );   break;
      case 'I':   imprime( &P );   break;
      case 'P':   set_reserva( );  break;
      case 'F':   encerra = 1;     break;
      }
   }

   if ( P.mem != NULL )   free( P.mem );

   return VM_erro;
}
#endif
