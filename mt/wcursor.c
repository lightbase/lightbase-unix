
#ifdef comout

This module contains the functions which control cursor style.

win_csset( x )      /* turn on cursor style x */
win_cpush( x )     /* turn on cursor style x placing old style on a stack */
win_cpop()		    /* return cursor to style prior to previous win_cpush() */
fwsetcursor( win ) /* set cursor to appropriate location style for window */

#endif

#include <stdio.h>
#include <window.h>


#define CSSTACKSIZE 15

static SS csstak[ CSSTACKSIZE + 1 ];
static SS cssp = 0;
static SS styleis = -1;			/* if -1 then don't know */


SS win_cpush( x )				/* returns -1 if stack full */

SS x;
{ 
   if( cssp >= CSSTACKSIZE )    return( -1 );

   else  {  csstak[ cssp++ ] = styleis;  win_csset( x );  }

   return( 0 );
}


SS win_cpop( )			/* return -1 if stack empty */
{
   if( ! cssp ) return( -1 );

   win_csset( csstak[ --cssp ] );

   return 0;
}


VOID win_csset( x )

UC  x;			/* style to set on */
{
   SC buf[20];

   if ( x >= MAXCURS  ||  ! *curstyle[ x ] )
   {
      if ( ! *curstyle[ 0 ] )   return;

      x = 0;
   }

   if( styleis == x )    return;

   styleis = x;

   if ( strange_term == 1 )
   {
#ifdef   MSDOS
      tipo_cursor_dos( (SC *) curstyle[ x ] );
      return;
#else
      /*** Para o UNIX ***/
      x = x;
#endif
   }

   sprintf( buf,"%s",curstyle[ x ] );
   doputs( (UC *) buf ); 
}

