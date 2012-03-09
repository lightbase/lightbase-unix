
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <window.h>
#include PROTO
#ifdef MSDOS
#include  <io.h>
#endif

#include "mensagem.h"

#ifdef     PROTO_ANSI

UC * paryset( UC *, UC **, SS, FILE * );
UC * parset( UC *, SS, UC **, ... );
UC * get1( UC * );

ST( VOID ) find_badatr( VOID );
ST( VOID ) loadgraph( UC *[] );
ST( VOID ) loadconv( UC *[] );
ST( VOID ) prepcurs( UC * );
ST( VOID ) linefix( UC *, UC * );

#else

UC * paryset( );
UC * parset( );
UC * get1( );

ST( VOID ) find_badatr( );
ST( VOID ) loadgraph( );
ST( VOID ) loadconv( );
ST( VOID ) prepcurs( );
ST( VOID ) linefix( );

#endif

/**** BEGIN HIDDEN FUNCTIONS ****/

SS find_params()        /* finds the appropriate terminal info, and
                                   loads the variables */
{
   SS a;
   SC buf[120];
   long l,ftell();
   FILE *param;
   UC *p, *px,*py,*lc,*strg;
   UC **aryptr;

db2( "find_params" );

   win_params = NULL;   /* the list of the parameters */

   sprintf( buf, "%s%c%s", diretorio, DIRSEP, terminal );

   if( access( buf, 4 ) )
   {
      /* if here, then it't a no go */
      printf( "\n\n%s\n", loc_str( STR_DISCO, SEM_TERM ) );
      doflush( );
      goto err0;
   }

   /* if here, then buf pts to a readable file */
   if ((param = fopen( buf, MODO_RO )) == NULL )   goto err0;

   fseek( param, 0L, 2 );

   l = ftell( param );

   fseek( param, 0L, 0 );

   if ((win_params = p = malloc( (SI) (l + 100))) == NULL )   goto err1;

   l = MAX( MAXCONV  * 3, MAXATRIB * 5 );
   l = MAX( MAXGRAPH * 4, l );
   if ((aryptr = (UC **) malloc((SI) l * sizeof(UC *))) == NULL )  goto err2;

   /* read in all parameters, marking block headers */

   if( ! fgets( (SC *) p, 1000, param ))    goto err3;

   p = parset( p,8,&px,&py,&lc,&init_seq,&init_proc,&init_stty,
               &term_seq,&term_proc );

   size_x = atoi( (SC *)px ); 
   size_y = atoi( (SC *)py ); 
   
   if ( size_x > MAXLENG   )   size_x = MAXLENG;
   if ( size_y > MAXHEIGHT )   size_y = MAXHEIGHT;
   
   lc_scrolls = atoi( (SC *)lc );

   if( p )   if( ! fgets( (SC *) p, 1000, param ) )   goto err3;

   p = parset( p,2,&px,&strg );
   prepcurs( px ); 

#ifdef  MSDOS   
   strange_term = atoi( (SC *)strg );
#else   
   strange_term = 0;
#endif   

   p = parset( p,1,&px ); linefix( px,go_upx );
   p = parset( p,1,&px ); linefix( px,go_dnx );
   p = parset( p,1,&px ); linefix( px,go_rtx );
   p = parset( p,1,&px ); linefix( px,go_lftx );

   if( p ) fgets( (SC *) p, 1000, param );

   p = parset( p,6,&go_home,&go_up,&go_dn,&go_rt,&go_lft,&go_nl );

   if( strange_term == 0 && ! *mov_cur && ! *go_home )
        goto err3;               /*  must be able to locate cursor */

   if( p ) fgets( (SC *) p, 1000, param );

   p = parset( p,4,&do_eol,&do_eos,&do_cls,&do_bs );

   if( p ) fgets( (SC *) p, 1000, param );

   p = parset( p,6,&do_insline,&do_delline,
               &do_scroll,&do_unscroll,&do_pan,&do_unpan );

   /* load UG ( User graphic display ) */
   p = paryset( p, aryptr, MAXGRAPH * 4, param );  
   loadgraph( aryptr );  

   /* load atrib_ary ( atributes ) */
   if( p )   p = (UC *) fgets( (SC *) p, 1000, param );

   p = paryset( p,aryptr,MAXATRIB * 5,param );

   for( a = 0;a < MAXATRIB;a++ )
   {
      atrib_ary[a].at_on = ( UC * ) aryptr[ a * 5 ];
      atrib_ary[a].at_off = ( UC * ) aryptr[ ( a * 5 ) + 1];
      atrib_ary[a].at_onlen = atoi( (SC *)aryptr[ (a * 5 ) + 2 ] );
      atrib_ary[a].at_offlen = atoi( (SC *)aryptr[ (a * 5 ) + 3 ] );
      atrib_ary[a].at_ideo = atoi( (SC *)aryptr[ (a * 5) + 4 ] );
   }

   find_badatr();                        /* fix up the bad_atribs string */
   p = paryset( p,aryptr,MAXCONV * 3,param );
   loadconv( aryptr );   /* insert convert records into thier stack */

   if( p )   p = (UC *) fgets( (SC *) p, 1000, param );

   p = paryset( p,curstyle,MAXCURS,param );

   if( p )   p = (UC *) fgets( (SC *) p, 1000, param );

   p = parset( p,6,&px,&sp_dest,&py,&lc,&sp_eol,&sp_eop );
   sp_rp = *px;
   sp_atrib = *py;
   sp_graph = *lc;
   p = paryset( p,bel_ary,MAXBELLS,param );

   /*** win_params sera liberado na saida win_quit() ***/

   free( (SC *)  aryptr );

   fclose( param );

   return 0;

err3:
   free( (SC *)  aryptr );
err2:
   free( (SC *)  win_params );   win_params = NULL;
err1:
   fclose( param );
err0:
   return -1;
}

ST( VOID ) find_badatr()                    /* find the bad atributes */
{
  SS a;
  UC mini[2];
  bad_atribs[ 0 ] = '\0';
  mini[1] = 0;
  for( a = 1;a < MAXATRIB;a++ )
        { if( !*atrib_ary[a].at_on ) continue;
          if(( atrib_ary[a].at_onlen || atrib_ary[a].at_offlen ))
                { /* a is a problem atribute */
                  mini[0] = a;
                  strcat((SC *) bad_atribs, (SC *) mini );
                }
        }
}


ST( VOID ) loadgraph( aryptr )

UC *aryptr[ MAXGRAPH * 4 ];
{ 
   SS   a, a4, b;

   db2( "loadgraph" );

   for( a = 0;   a < MAX_SE + 1;   a++ )
   {
      SE[ a ].start = SE[ a ].end = (UC *) "";
   }

   for( a = 0;   a < MAXGRAPH;   a++ )
   {
      UG[ a ].ary  = (UC *) "";
      UG[ a ].type = 0;
   }

   for( a = a4 = 0;   a < MAXGRAPH;   a++, a4 += 4 )
   {
      if ( ! *aryptr[ a4 ] )   continue;

      b = atoi((SC *) aryptr[a4 + 1] );

      if ( b  <  MAX_SE + 1 ) /** Nao e' caracter e' sequencia START/END **/
      {
         if ( atoi((SC *) aryptr[a4 + 2] ) )  SE[ b ].start = aryptr[ a4 ];
         else                                 SE[ b ].end   = aryptr[ a4 ];
         continue;
      }

      if ( (b -= 128)  >=  MAXGRAPH )   continue;

      UG[ b ].ary  = aryptr[ a4 ];
      UG[ b ].type = atoi((SC *) aryptr[a4 + 2]);
   }
}


ST( VOID ) loadconv( aryptr )           /* load(  ); the convert stack etc. */
UC *aryptr[ MAXCONV * 3 ];
{ 
  UC mini[2];
  SS a,a3,b;
  mini[1] = 0;
 
db2( "loadconv" ); 
  for( a = 0,a3 = 0;a < MAXCONV;a++, a3 += 3 )
        { 
          if( ! *aryptr[ a3 ] ) continue;       /* no convert here */
          for( b = kc_sp;b;b-- ) /* simple insertion sort (assending)*/
                { 
                  if( strcmp((SC *) aryptr[ a3 ], (SC *) kc_ary[b - 1].kc_from) < 0 )  
                        { kc_ary[b].kc_from = kc_ary[b - 1].kc_from;
                          kc_ary[b].kc_to = kc_ary[b - 1].kc_to;
                          kc_ary[b].kc_dsc = kc_ary[b - 1].kc_dsc;
                        }
                  else break;
                }
          kc_sp++;
          kc_ary[b].kc_from = ( UC * ) aryptr[a3];  
          kc_ary[b].kc_to   = ( UC * ) aryptr[a3 + 1];
          kc_ary[b].kc_dsc  = ( UC * ) aryptr[a3 + 2];
        }
  /* make up kc_fary */
  *kc_fary = 0;       
  for( a = 0;a < kc_sp;a++ )
        if( ! strchr( ( SC * )kc_fary, *kc_ary[a].kc_from )) 
                { *mini = *kc_ary[a].kc_from;
                  strcat( ( SC * ) kc_fary, (SC *) mini );
                }
}

ST( VOID ) prepcurs( s )        /* prepare more manageable cursor move */
UC *s;
{
  UC *p,*q;
  SS *inc;
 
db2( "prepcurs" ); 
  xplus_const = 0;
  yplus_const = 0;
  inc = &xplus_const; 
  cur_order = 0;
  for( p = s,q = mov_cur;*p;p++ )
        { if( *p != '%' ) { *q = *p; q++; continue; }
          p++;
          switch( *p )
                { case 'd': *q = '%'; q++; *q = 'd'; q++; break;
                  case '2': *q = '%'; q++; *q = '2'; 
                            q++; *q = 'd'; q++; break;
                  case '3': *q = '%'; q++; *q = '3'; 
                            q++; *q = 'd'; q++; break;
                  case '.': *q = '%'; q++; *q = 'c'; q++; break;
                  case '+': p++; *inc += *p;
                            *q = '%'; q++; *q = 'c'; q++; break;
                  case 'i': xplus_const++; yplus_const++; continue;
                  case 'r': cur_order = 1; break;
                  default : *q = '%'; q++; *q = *p; q++; continue;
                }
          /* if here, then rotate inc pointer */
          if( inc == &xplus_const )
                inc = &yplus_const;
          else inc = &xplus_const;
        }
  *q = 0;
}


ST( VOID ) linefix( s,res )     /* fix formats for go_upx, go_dnx etc. */
UC *s,*res;
{
  UC *p,*q;

db2( "linefix" );
  for( p = s,q = res;*p;p++ )
        { if( *p != '%' ) { *q = *p; q++; continue; }
          p++;
          switch( *p )
                { case 'd': *q = '%'; q++; *q = 'd'; q++; continue;
                  case '2': *q = '%'; q++;
                            *q = '2'; q++;
                            *q = 'd'; q++; continue;
                  case '3': *q = '%'; q++;
                            *q = '3'; q++;
                            *q = 'd'; q++; continue;
                  case '.': *q = '%'; q++; *q = 'c'; q++; continue;
                  default: *q = '%'; q++; *q = *p; q++; continue;
                }
        }
  *q = 0;
}


