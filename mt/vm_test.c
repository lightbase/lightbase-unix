
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>

#include   <vm.h>

#define	MAX_ITEMS 5050
#define	MAX_SIZE	 100

UC      val[ MAX_ITEMS ];
VM_TAM  hnd[ MAX_ITEMS ];
VM_TAM  len[ MAX_ITEMS ];
US      max, mpass;

void nada( SC *str, ... ){ str = str; }

#define  PRINTF1      printf
#define  PRINTF2      nada
#define  PRINTF3      nada

void tsterr( SC *str )

{
   SS   erro = VM_erro;
   fprintf( stderr,"%s - VM_erro %d \n", str, erro );
   vm_end( );
   exit( erro );
}

SS  qpage( void );
SS  qslot( void );

SS  qslot( )
{
   SS   I;

   for( I = 0;   I < SL_QTD;   I++ )
   {
      if ( VM_slot[ I ].lru == 0 )   break;
   }

   return( I );
}

SS  qpage( )
{
   SS   I;

   for( I = 0;   I < PG_QTD;   I++ )
   {
      if ( DESABILIT( VM_page[ I ].loc ) )   break;
   }

   return( I );
}

void confere( void )
{
   SC   *P;
   UC   C;
   US   I, J;

   /*** Confira todos os alloc() ***/
   PRINTF1( "Deref() em %u elementos S/P %u/%u \n", max, qslot(), qpage() );
   for ( I = 0;   I != max;   ++I )
   {
      if ((P = vm_deref( hnd[ I ], 0 )) == NULL )  tsterr("vm_deref == NULL");

      for ( C = val[ I ], J = 0;   J < len[ I ];   J++, P++ )
      {
         if ( *P != C )
         {
            PRINTF1("%u/%u - ", I, J );
            tsterr( "Diferente" );
         }
      }
   }
}

void main( argc, argv )

SI    argc;
SC    *argv[ ];
{
   SL      a = 0L, B, C;
   US      i, j, k, n;
   SC      c, *p, *q;

   if ( argc > 1 )
   {
      max = atoi( argv[ 1 ] );
      max = max < 1 ? 1 : max;
      max = max > MAX_ITEMS ? MAX_ITEMS : max;
   }

   else  max = MAX_ITEMS;

   if ( argc > 2 )   mpass = atoi( argv[ 2 ] );

   else              mpass = max;

   C = farcoreleft( );
   PRINTF1("Core = %ld \n", C );

/***
   VM_maxslot = 10;
***/

   /*** Inicialize o VM ***/
   vm_start( );

   VM_res = MAX_SIZE / 10;
   PRINTF1( "VM_res = %u \n", VM_res );

   /*** Alloc max elementos ***/
   PRINTF1( "Alloc() em %u elementos, Core %ld/%ld \n", max, C, farcoreleft() );
   for ( i = 0;   i != max;   ++i )
   {
      len[ i ] = 1 + rand() % (MAX_SIZE - 1);

      if ((hnd[ i ] = vm_alloc( len[ i ])) == 0 ) tsterr( "vm_alloc() == 0");

      if ((p = vm_deref( hnd[ i ], 0 )) == NULL )  tsterr("vm_deref() == NULL");

      a += len[ i ];
      val[ i ] = c = rand() % 256;

      for ( j = len[ i ];   j > 0;   j--, p++  )
      {
         *p = c;
      }

      B = farcoreleft( );

      PRINTF2("%u/%u Alloc %u Total %ld Core = %ld", i + 1, hnd[ i ], len[ i ], a, B );
      {
         SS Z;
         PRINTF3( " Lru/Page " );
         for ( Z = 0;   Z < VM_maxslot;   Z++ )
         {
            PRINTF3( "%d/%d ", VM_slot[ Z ].lru, VM_slot[ Z ].page );
         }
         PRINTF2( "\n" );
      }
   }

   confere( );

   /*** Free()  e novo alloc() aleatorio mpass vezes ***/
   PRINTF1( "Free() e Alloc() em %u elementos, Core %ld/%ld \n", mpass, C, farcoreleft() );
   for ( n = 0;   n != mpass;   ++n )
   {
      i = rand() % max;

      k = len[ i ];

      if ( vm_free( hnd[ i ] )  ==  0 )   tsterr("vm_free == 0");

      len[ i ] = 1 + rand() % (MAX_SIZE - 1);

      if ((hnd[ i ] = vm_alloc( len[ i ])) == 0 ) tsterr("vm_alloc() == 0");

      if ( (p = vm_deref( hnd[ i ], 0 )) == NULL ) tsterr("vm_deref() == NULL");

      c = val[ i ] = rand() % 256;

      for ( j = len[ i ];   j > 0;   j--, p++  )   *p = c;

      PRINTF2("%u/%u %u/%u Free %u Alloc %u", n + 1, mpass, i + 1, hnd[ i ], k, len[ i ] );
      {
         SS Z;
         PRINTF3( " Lru/Page " );
         for ( Z = 0;   Z < VM_maxslot;   Z++ )
         {
            PRINTF3( "%d/%d ", VM_slot[ Z ].lru, VM_slot[ Z ].page );
         }
         PRINTF2( "\n" );
      }
   }

   confere( );

   /*** Realloc() max elementos ***/
   PRINTF1( "Realloc() e conferencia de  %u elementos, Core %ld/%ld \n", mpass, C, farcoreleft() );
   for ( n = 0;   n != mpass;   n++ )
   {
      i = rand() % max;

      k = len[ i ];

      len[ i ] = 1 + rand() % (MAX_SIZE - 1);

      if ((hnd[ i ] = vm_realloc( hnd[ i ], len[ i ])) == 0 )
         tsterr("realloc() == 0");

      if ((q = p = vm_deref( hnd[ i ], 0 )) == NULL ) tsterr("vm_deref() == NULL");

      j = len[ i ];
      if ( j > k )   j = k;

      for ( c = val[ i ];   j > 0;   j--, q++ )
      {
         if ( *q != c )   tsterr( "Diferente" );
      }

      c = val[ i ] = rand() % 256;

      for ( j = len[ i ];   j > 0;   j--, p++  )   *p = c;

      PRINTF2("%u/%u %u/%u Realloc %u/%u", n + 1, mpass, i + 1, hnd[ i ], k, len[ i ] );
      {
         SS Z;
         PRINTF3( " Lru/Page " );
         for ( Z = 0;   Z < VM_maxslot;   Z++ )
         {
            PRINTF3( "%d/%d ", VM_slot[ Z ].lru, VM_slot[ Z ].page );
         }
         PRINTF2( "\n" );
      }
   }

   confere( );

   /*** Free()  e novo alloc() aleatorio mpass vezes ***/
   PRINTF1( "Free() em %u elementos, Core %ld/%ld \n", max, C, farcoreleft() );
   for ( i = 0;   i != max;   ++i )
   {
      if ( vm_free( hnd[ i ] )  ==  0 )   tsterr("vm_free == 0");
   }

   /*** Alloc max elementos ***/
   PRINTF1( "Alloc() em %u elementos, Core %ld/%ld \n", max, C, farcoreleft() );
   for ( i = 0;   i != max;   ++i )
   {
      len[ i ] = 1 + rand() % (MAX_SIZE - 1);

      if ((hnd[ i ] = vm_alloc( len[ i ])) == 0 ) tsterr( "vm_alloc() == 0");

      if ((p = vm_deref( hnd[ i ], 0 )) == NULL )  tsterr("vm_deref() == NULL");

      a += len[ i ];
      val[ i ] = c = rand() % 256;

      for ( j = len[ i ];   j > 0;   j--, p++  )
      {
         *p = c;
      }

      B = farcoreleft( );

      PRINTF2("%u/%u Alloc %u Total %ld Core = %ld", i + 1, hnd[ i ], len[ i ], a, B );
      {
         SS Z;
         PRINTF3( " Lru/Page " );
         for ( Z = 0;   Z < VM_maxslot;   Z++ )
         {
            PRINTF3( "%d/%d ", VM_slot[ Z ].lru, VM_slot[ Z ].page );
         }
         PRINTF2( "\n" );
      }
   }

   confere( );

   /*** Free()  e novo alloc() aleatorio mpass vezes ***/
   PRINTF1( "Free() e Alloc() em %u elementos, Core %ld/%ld \n", mpass, C, farcoreleft() );
   for ( n = 0;   n != mpass;   ++n )
   {
      i = rand() % max;

      k = len[ i ];

      if ( vm_free( hnd[ i ] )  ==  0 )   tsterr("vm_free == 0");

      len[ i ] = 1 + rand() % (MAX_SIZE - 1);

      if ((hnd[ i ] = vm_alloc( len[ i ])) == 0 ) tsterr("vm_alloc() == 0");

      if ( (p = vm_deref( hnd[ i ], 0 )) == NULL ) tsterr("vm_deref() == NULL");

      c = val[ i ] = rand() % 256;

      for ( j = len[ i ];   j > 0;   j--, p++  )   *p = c;

      PRINTF2("%u/%u %u/%u Free %u Alloc %u", n + 1, mpass, i + 1, hnd[ i ], k, len[ i ] );
      {
         SS Z;
         PRINTF3( " Lru/Page " );
         for ( Z = 0;   Z < VM_maxslot;   Z++ )
         {
            PRINTF3( "%d/%d ", VM_slot[ Z ].lru, VM_slot[ Z ].page );
         }
         PRINTF2( "\n" );
      }
   }

   confere( );

   /*** Realloc() max elementos ***/
   PRINTF1( "Realloc() e conferencia de  %u elementos, Core %ld/%ld \n", mpass, C, farcoreleft() );
   for ( n = 0;   n != mpass;   n++ )
   {
      i = rand() % max;

      k = len[ i ];

      len[ i ] = 1 + rand() % (MAX_SIZE - 1);

      if ((hnd[ i ] = vm_realloc( hnd[ i ], len[ i ])) == 0 )
         tsterr("realloc() == 0");

      if ((q = p = vm_deref( hnd[ i ], 0 )) == NULL ) tsterr("vm_deref() == NULL");

      j = len[ i ];
      if ( j > k )   j = k;

      for ( c = val[ i ];   j > 0;   j--, q++ )
      {
         if ( *q != c )   tsterr( "Diferente" );
      }

      c = val[ i ] = rand() % 256;

      for ( j = len[ i ];   j > 0;   j--, p++  )   *p = c;

      PRINTF2("%u/%u %u/%u Realloc %u/%u", n + 1, mpass, i + 1, hnd[ i ], k, len[ i ] );
      {
         SS Z;
         PRINTF3( " Lru/Page " );
         for ( Z = 0;   Z < VM_maxslot;   Z++ )
         {
            PRINTF3( "%d/%d ", VM_slot[ Z ].lru, VM_slot[ Z ].page );
         }
         PRINTF2( "\n" );
      }
   }

   confere( );

   PRINTF1("VM_end() - Core %ld/%ld\n", C, farcoreleft() );

   vm_end( );

   PRINTF1("Core %ld/%ld\n", C, farcoreleft() );
}

