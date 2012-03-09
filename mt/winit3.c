
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include  <window.h>

#ifdef    MT_VARARGS
#include  <varargs.h>
#endif

#ifdef     MSDOS
UC * get1( UC * );
#else
UC * get1( );
#endif

#ifdef    PROTO_ANSI
UC *parset( UC *p, SS num, UC **p1, ... )
#else
#ifdef    MT_VARARGS
UC *parset( p, num, va_alist )
UC *p;
SS num;
va_dcl
#else
UC *parset( p, num, p1 )
UC *p;
SS num;                         /* number of parameters passed */
UC **p1;                        /* address of some of them */
#endif
#endif
{
   UC **pp;
   UC **qq;
   SS a;
#ifdef    MT_VARARGS
   va_list ap;
   va_start( ap );
#else
   SS diff = 1;
#endif

db2( "parset" );
#ifdef    MT_VARARGS
   pp = va_arg( ap, UC ** );
#else
#ifndef   MSDOS
   if ( inc_int < 0 )   diff = -1;   /*** Stack Crescente ***/
#endif
   pp = (UC **) &p1;
#endif

   for ( a = 0;   a < num;   a++ )
   {
#ifdef    MT_VARARGS
      qq  = pp;
#else
      qq  = (UC **) *pp;
#endif

      *qq = p;

      /*** Nao compreendi isto, mas o "core dumped" no PA800R ... ***/
      /*** ... podera' ser resolvido. ***/
      /*** Caso nao resolva no HP recompile com MT_VARARGS ***/
      if ( ! *p )   *qq = (UC *) "";

      p   = get1( p );

#ifdef    MT_VARARGS
      pp  = va_arg( ap, UC ** );
#else
      pp += diff;
#endif
   }

#ifdef    MT_VARARGS
   va_end( ap );
#endif

   return p;
}


UC * paryset( p,ary,max,fd )    /* parameter array setup */
UC *p;                                /* position in buffer */
UC **ary;                             /* array */
SS max;                                /* # of elements to expect */
FILE *fd;                               /* parameter file */
{
  SS a;
  SS endflag = 0;
  UC buf[ 220 ];
  UC *p1;
  SS more = 1;
  SS size;

db2( "paryset" );
  if( ! p ) endflag = 1;
  for( a = 0;a < max;a++ )
        {
          if( endflag ) { ary[a] = (UC *)""; continue; }
          if( more )
                { more = 0;
                  if( ! fgets( (SC *)p,1000,fd ))
                        { endflag = 1; p = 0; a--; continue; }
                  if( ! strncmp( (SC *)p,"ARYEND",6 ))
                        { endflag = 1; a--; continue; }
                  size = strlen( (SC *)p );
                  p1 = p;
                }
          ary[a] = p;
          /* advance p */
          p = get1( p );
          if( p >= p1 + size )  more = 1;
        }
  while( ! endflag ) { if( ! fgets( (SC *)buf,200,fd ))
                                { p = 0; return p; }
                       if( ! strncmp( (SC *)buf,"ARYEND",6 ))
                                return p;
                     }
  return p;
}



UC * get1( p )          /* get a parameter -
                                   PARAMETER CONVERSION DONE HERE! */
UC *p;
{
  UC *q;
  SS a,b;

db2( "get1" );
  if( ! p ) return p;
  if( ! *p ) return p;
  q = p;
  while( 1 )
        { if( *p == '\n' )
                { *q = 0; p++; return p; } /* end of string */
          if( *p == '|' )
                { *q = 0; p++; return p; } /* end of entry */
          if( *p == '^' && *(p + 1) >= 'A' && *(p + 1) <= 'Z' )
                { p++; *q = *p - 'A' + 1;
                  p++; q++; continue;
                }
          if( *p == '^' && *(p + 1) == '^' )
                { p++; *q = '^';
                  p++; q++; continue;
                }
          if( *p == '\\' )
          {
             p++;
             switch( *p )
             {
             case 'E' :  *q = 033;   q++;  break;
             case 'n' :  *q = '\n';  q++;  break;
             case 'r' :  *q = '\r';  q++;  break;
             case '\0':
             case '\n':  *q = 0;     p--;  break;
             default  :  *q = *p;    q++;  break;
             }
             p++;  continue;
          }
#ifdef NAODEF
                {
                  p++;
                  switch( *p )
                        {
                          case 'E':     /* escape */
                                    *q = 033; q++; break;
                          case '\\':    /* inserted backslash or | ) */
                                     if( *( p + 1 ) == '|' )
                                         { *q = '|'; p++; q++; break; }
                                     *q = '\\'; q++; break;
                          case 'n':     /* newline */
                                     *q = '\n'; q++; break;
                          case 'r':     /* return */
                                     *q = '\r'; q++; break;
                          case '\n': *q = '\\'; p--; break;
                          default:      /* bogus backslash (separater) */
                                     *q = *p; q++; p++; continue;
                        }
                  p++; continue;
                }
#endif
          if( *p == '%' )
                {
                  p++;
                  switch( *p )
                        { case 'd': p++;
                                    for( b = 3,*q = 0;b;b-- )
                                        { if( *p < '0' || *p > '9' )
                                                break;
                                          *q *= 10;
                                          *q += *p - '0';
                                          p++;
                                        }
                                    q++; break;
                          case '0':
                          case 'o': p++;
                                    for( b = 3,*q = 0;b;b-- )
                                        { if( *p < '0' || *p > '7' )
                                                break;
                                          *q *= 8;
                                          *q += *p - '0';
                                          p++;
                                        }
                                    q++; break;
                          case 'x': p++;
                                    for( b = 2,*q = 0;b;b-- )
                                        { a = *p;
                                          if( a >= 'a' && a <= 'f' )
                                                a -= 'a' - 'A';
                                          if(! (( a >= '0' && a <= '9' ) ||
                                                ( a >= 'A' && a <= 'F' )))
                                                      break;
                                          if( a >= 'A' && a <= 'F' )
                                                a = 10 + a - 'A';
                                          else  a = a - '0';
                                          *q *= 16;
                                          *q += a;
                                          p++;
                                        }
                                    q++;
                                    break;
                          case '%': *q = '%'; p++, q++;
                          default : continue;
                        }
                  continue;
                }
          *q = *p;
          p++; q++; continue;
        }

  /** esta aqui apenas para evitar warnings */
  /** return ( p );                         */
}



