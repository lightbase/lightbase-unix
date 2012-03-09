/*---------- lb2rel3.c ---------*/
#include  "lb2all.h"

#include  <string.h>

#ifdef    MT_VARARGS
#include  <varargs.h>
#endif

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */

#include  "lb2files.h"
#include  "lb2prot1.h"

#include  "lb2rel.h"


#define   EXT_IMPR     ".lbp"


#define   FIM_ARRAY    "ARYEND"  /* final de array em drivers terminal, impressora */

static TEXT str_branco[] = " ";

/*------------------------------------------------------------------------*/
/* inicio de prototipos                                                   */
/*------------------------------------------------------------------------*/
#ifdef MSDOS
   VOID  i_graph( UC ** );
   static UC   *paryset( UC *, UC **, SS, FILE * );
   static UC   *get1( UC * );
   VOID escrava_off(VOID);
   VOID escrava_on(VOID);
#else
   VOID  i_graph( );
   static UC   *paryset( );
   static UC   *get1( );
   VOID escrava_off();
   VOID escrava_on();
#endif

#ifdef PROTO_ANSI
   static UC   *parset( UC *p, SS num, UC **p1, ... );
#else
   static UC   *parset( );
#endif





/*---------- mont_vt_impr() ----------*/
F__LOC   VOID mont_vt_impr(buf)
TEXT *buf;
{  COUNT i;
   TEXT aux[TE+1];

   buf[0] = '\0';

   for ( i = 0; i < ap_maxprinter; i++) {
      piece(ap_vetprinter[i], aux, '@', 1, TE);
      strcat(buf, aux);
      if ( (i + 1) < ap_maxprinter )
         strcat(buf, "@");
   }
}


F__LOC   SS i_find_params( flag )
COUNT flag;
{
   SS    a;
   UC    *p;
   static UC *buffer = NULL, **aryptr = NULL;
   long  l,ftell();
   SC aux[ PATH_MAX + 1 ];
   FILE  *param;

   if ( flag == 'F' ) {
      if ( buffer != NULL ) {
         free( buffer );
         buffer = NULL;
      }
      if ( aryptr != NULL ) {
         free( (SC *)  aryptr );
         aryptr = NULL;
      }
      return(0);
   }

   l = MAX( MAXGRAPH * 4, I_MAXATRIB * 2 );

   aryptr = (UC **) malloc( (SI) l * sizeof( UC * ) );

   if ( aryptr == NULL )   goto err0;

   driver = c_impress.driver;

   info_base( driver, ap_home_lb, EXT_IMPR, aux );

   param = fopen( aux,  "r" );

   if( ! param )   goto err1;

   fseek( param, 0L, 2 );

   l = ftell( param );

   fseek( param, 0L, 0 );

   buffer = p = malloc( (SI) (l + 100) );

   if( ! p ) goto err2;

   /* read in all parameters, marking block headers */

   if( ! fgets( (SC *) p, 1000, param ))    goto err2;

   p = parset( p, 4, &i_inic, &i_final, &i_fdl, &i_fdp );

   if( p )   if( ! fgets( (SC *) p, 1000, param ) )   goto err2;

   /* le parametros de caracteres graficos */
   p = paryset( p, aryptr, MAXGRAPH * 4, param );
   i_graph( aryptr );

   /* ler atributos */
   if( p )   p = (UC *) fgets( (SC *) p, 1000, param );

   p = paryset( p,aryptr,I_MAXATRIB * 2,param );

   for( a = 0;a < I_MAXATRIB;a++ )
   {
      i_atrib[a].at_on   =  (UC *) aryptr[ a * 2 + 0 ];
      i_atrib[a].at_off  =  (UC *) aryptr[ a * 2 + 1 ];
   }

   fclose( param );


   return 0;

err2:
   fclose( param );
err1:
   free( (SC *)  aryptr );
   aryptr = NULL;
err0:
   return -1;
}

F__LOC   VOID i_graph( aryptr )
UC *aryptr[ MAXGRAPH * 4 ];
{
   SS   a, a4, b;

   for( a = 0;   a < MAX_SE + 1;   a++ )
   {
      I_SE[ a ].start = I_SE[ a ].end = (UC *) str_nada;
   }

   for( a = 0;   a < MAXGRAPH;   a++ )
   {
      IMP[ a ].ary  = (UC *) str_branco;  /* era str_nada. mudou em 2.1 1.5 */
      IMP[ a ].type = 0;
   }

   for( a = a4 = 0;   a < MAXGRAPH;   a++, a4 += 4 )
   {
      if ( ! *aryptr[ a4 ] )   continue;

      b = atoi((SC *) aryptr[a4 + 1] );

      if ( b  <  MAX_SE + 1 ) /** Nao e' caracter e' sequencia START/END **/
      {
         if ( atoi((SC *) aryptr[a4 + 2] ) )  I_SE[ b ].start = aryptr[ a4 ];
         else                                 I_SE[ b ].end   = aryptr[ a4 ];
         continue;
      }

      if ( (b -= 128)  >=  MAXGRAPH )   continue;

      IMP[ b ].ary  = aryptr[ a4 ];
      IMP[ b ].type = atoi((SC *) aryptr[a4 + 2]);
   }
}

#ifdef    PROTO_ANSI
static UC *parset( UC *p, SS num, UC **p1, ... )
#else
#ifdef    MT_VARARGS
static UC *parset( p, num, va_alist )
UC *p;
SS num;
va_dcl
#else
static UC *parset( p, num, p1 )
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

      if ( ! *p )   *qq = (UC *) "";

      p = get1( p );

#ifdef    MT_VARARGS
      pp = va_arg( ap, UC ** );
#else
      pp += diff;
#endif
   }

#ifdef    MT_VARARGS
   va_end( ap );
#endif

   return p;
}


static   UC * paryset( p,ary,max,fd )    /* parameter array setup */
UC *p;                                /* position in buffer */
UC **ary;                             /* array */
SS max;                                /* # of elements to expect */
FILE *fd;                               /* parameter file */
{
   SS a;
   SS endflag = 0;
   UC buf[200];
   UC *p1;
   SS more = 1;
   SS size;

   if( ! p ) endflag = 1;

   for( a = 0;  a < max;  a++ )  {

      if( endflag )  {
         ary[ a ] = (UC *)str_branco;  /* era str_nada. mudou em 2.1 1.5 */
         continue;
      }

      if( more )   {
         more = 0;

         if( ! fgets( (SC *)p, 1000, fd ))   {
            endflag = 1;
            p = 0;
            a--;
            continue;
         }

         if( ! strncmp( (SC *)p, FIM_ARRAY,6 ))   {
            endflag = 1;
            a--;
            continue;
         }

         size = strlen( (SC *)p );
         p1 = p;
      }

      ary[a] = p;
      /* advance p */
      p = get1( p );

      if( p >= p1 + size )
         more = 1;
   }

   while( ! endflag )   {

      if( ! fgets( (SC *)buf,200,fd ))  {
         p = 0;
         return p;
      }

      if( ! strncmp( (SC *)buf, FIM_ARRAY,6 ))
         return p;
   }

   return p;
}



static   UC * get1( p )  /* get a parameter - PARAMETER CONVERSION DONE HERE! */
UC *p;
{
   UC *q;
   SS a,b;

   if( ! p ) return p;

   if( ! *p ) return p;

   q = p;

   while( 1 )   {

      if( *p == '\n' )   {
         *q = 0;
         p++;
         return p;
      } /* end of string */

      if( *p == '|' )   {
         *q = 0;
         p++;
         return p;
      } /* end of entry */

      if( *p == '^' && *(p + 1) >= 'A' && *(p + 1) <= 'Z' )  {
         p++;
         *q = *p - 'A' + 1;
         p++;
         q++;
         continue;
      }

      if( *p == '^' && *(p + 1) == '^' )   {
         p++;
         *q = '^';
         p++;
         q++;
         continue;
      }

      if( *p == '\\' )  {
         p++;

         switch( *p )   {
            case 'E':
               /* escape */
               *q = 033;
               q++;
               break;
            case '\\':
               /* inserted backslash or | ) */
               if( *( p + 1 ) == '|' )  {
                  *q = '|';
                  p++;
                  q++;
                  break;
               }
               *q = '\\';
               q++;
               break;
            case 'n':
               /* newline */
               *q = '\n';
               q++;
               break;
            case 'r':
               /* return */
               *q = '\r';
               q++;
               break;
            case '\n':
               *q = '\\';
               p--;
               break;
            default:
               /* bogus backslash (separater) */
               *q = *p;
               q++;
               p++;
               continue;
         }
         p++;
         continue;
      }

      if( *p == '%' )   {

         p++;
         switch( *p )   {
            case 'd':
               p++;
               for( b = 3,*q = 0;b;b-- )  {

                  if( *p < '0' || *p > '9' )
                     break;

                  *q *= 10;
                  *q += *p - '0';
                  p++;
               }

/*** jairo ***/
               if ( *q == 0 )   *q = 255;

               q++;
               break;

            case '0':
            case 'o':
               p++;

               for( b = 3,*q = 0;b;b-- ) {

                  if( *p < '0' || *p > '7' )
                     break;

                  *q *= 8;
                  *q += *p - '0';
                  p++;
               }
               q++;
               break;
            case 'x':
               p++;

               for( b = 2,*q = 0;b;b-- )   {
                  a = *p;

                  if( a >= 'a' && a <= 'f' )
                     a -= 'a' - 'A';

                  if(! (( a >= '0' && a <= '9' )
                        || ( a >= 'A' && a <= 'F' )))
                     break;

                  if( a >= 'A' && a <= 'F' )
                     a = 10 + a - 'A';
                  else
                     a = a - '0';

                  *q *= 16;
                  *q += a;
                  p++;
               }
               q++;
               break;
            case '%':
               *q = '%';
               p++, q++;

            default :
               continue;
         }
         continue;
      }
      *q = *p;
      p++;
      q++;
      continue;
   }
   /** esta aqui apenas para evitar warnings */
   /** return ( p );                         */
}


F__LOC   VOID inic_printer( )
{

   if ( device == NULL )   numdebug( 2001 );  /*  1- device invalido  */

   escrava_on();

   imp_puts( device, i_inic );

   if ( prel->rel_caracter == 'C' ) {
      imp_puts( device, i_atrib[ COMPRIMIDO ].at_on);
   }

   if ( prel->rel_qualid == 'C')
      imp_puts(device, i_atrib[ QUAL_CARTA ].at_on );

   escrava_off();

}


F__LOC   VOID term_printer( )
{
   if( device == NULL ) numdebug( 2002 );   /* 2 - device invalido */

   escrava_on();

   imp_puts( device, i_final );

   if ( prel->rel_caracter == 'C' ) {
      imp_puts( device, i_atrib[ COMPRIMIDO ].at_off);
   }

   if ( prel->rel_qualid == 'C')
      imp_puts( device, i_atrib[ QUAL_CARTA ].at_off );

   escrava_off();

}





