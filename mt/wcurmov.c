
#include <stdio.h>
#include <window.h>

#ifdef     MSDOS

ST( SS ) mov_check( SS, SS );

#else

ST( SS ) mov_check( );

#endif

static SC buf[ 40 ];

ST( SS ) mov_check( x, y )             /* try for a shorter rout */

SS x, y;
{
db2( "mov_check" );

   if ( tcp_x == x  &&  tcp_y == y )    return 1;

   if ( *go_home  &&  !x  &&  !y ) {  doputs( go_home );  return 1;  }

   if ( tcp_x == -1  ||  tcp_y == -1 )   return 0;

db3( "*" );
   if ( *go_nl && x == 0 && y == tcp_y + 1 ) { doputs( go_nl );  return 1; }

db3( "=" );
   if ( x != tcp_x  &&  y != tcp_y )   return  0;

   if ( x != tcp_x )
   { 
db3( "?" );
      if ( *go_rt   &&  x == tcp_x + 1 ) {  doputs( go_rt );   return 1;  }

      if ( *go_lft  &&  x == tcp_x - 1 ) {  doputs( go_lft );  return 1;  }

      if ( *go_rtx  &&  x > tcp_x )
      {
         sprintf( buf, (SC *) go_rtx, x - tcp_x );
         doputs( (UC *) buf );
         return ( 1 );
      }

      if ( *go_lftx &&  x < tcp_x )
      {
         sprintf( buf, (SC *) go_lftx, tcp_x - x );
         doputs( (UC *) buf );
         return ( 1 );
      }
   }

   if ( y != tcp_y )
   {
db3( "!" );
      if ( *go_dn   &&  y == tcp_y + 1 ) {  doputs( go_dn );   return 1;  }

      if ( *go_up   &&  y == tcp_y - 1 ) {  doputs( go_up );   return 1;  }

      if ( *go_dnx  &&  y > tcp_y )
      {
         sprintf( buf, (SC *) go_dnx, y - tcp_y );
         doputs( (UC *) buf );
         return ( 1 );
      }

      if ( *go_upx  &&  y < tcp_y )
      {
         sprintf( buf, (SC *) go_upx, tcp_y - y );
         doputs( (UC *) buf );
         return ( 1 );
      }
   }

   return ( 0 );
}

/***    ATENCAO    ***/
/*** Esta funcao so' deve ser chamada por W_curmove() ***/
/*** Pois ela nao testa limites. ***/
VOID WW_curmove( x, y )

SS x, y;
{
   SS a = 0;

db1( "WW_curmove" );

   a = mov_check( x, y );

   tcp_x = x;
   tcp_y = y;

   if ( a ) return;

db3( "MOVCUR" );

   if ( *mov_cur )
   {
      if ( xplus_const )  x += xplus_const;
      if ( yplus_const )  y += yplus_const;
      if ( cur_order ) {  a = x;  x = y;  y = a;  }

      sprintf( buf, (SC *) mov_cur, y, x );

      doputs( (UC *) buf );
   }
}

