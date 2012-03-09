
#ifdef comout

win_bell( x )    /***  rings the xth bell,
                       if not found, rings bell 0,
                       if not found, no bell.
                  ***/

#endif

#include <stdio.h>
#include <window.h>


VOID win_bell( x )

SS x;
{
   UC *p;
   SS a;

   if ( x >= MAXBELLS )   x = 0;

   if( ! *bel_ary[ x ] )
   {
      /*** BELL ACCEPT o MENOR ***/
      if ( ! *bel_ary[ (x = 3) ] ) {  doputch( 7 );   goto fim;  }
   }

   for ( p = bel_ary[ x ];   *p;   p++ )
   {
      /*** ^A ***/
      if ( *p == 1 ) {  doflush( );   for( a = BELPAUSE * 2;  a;  a-- )  ; }
      else              doputch( *p );
   }

fim:
   doflush( );
}  


