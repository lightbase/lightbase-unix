
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  <window.h>
#include  <mensagem.h>

#ifdef   MSDOS

GRP   *acha_grupo( SS numero );
SC    *str_trata( SC *dest, SC *orig );
GRP   *novo_grupo( SS num, SL off );
VOID  ate_nl( FILE *F );

#else

GRP   *acha_grupo( );
SC    *str_trata( );
GRP   *novo_grupo( );
VOID  ate_nl( );

#endif

extern GRP *grp_first;
extern SC  str_arquivo[ MAX_ARQ + 1 ];
extern SC  *str_erro;
extern SC  *str_grupo;
extern SS  TG;
extern SC  str_tmp[ MAX_CHAR + 10 ];

SS   down_str( numero )

SS   numero;
{
   SS  ret = 0;
   GRP *G;

   if ((G = acha_grupo( numero )) == NULL || G->qup == 0 )   goto fim;

   if ( --G->qup == 0 )
   {
      if ( G->ptr != NULL )   free((SC *) G->ptr );
      if ( G->str != NULL )   free((SC *) G->str );

      G->ptr = NULL;
      G->str = NULL;
   }

   ret = 1;

fim:

   return ( ret );
}

SS   up_str( numero )

SS   numero;
{
   SC   *P, linha[ MAX_GETS + 10 ], trab[ MAX_GETS + 10 ];
   SS   I, ret = 0;
   FILE *F;
   GRP  *G;

   G = NULL;
   F = NULL;

   if ((G = acha_grupo( numero )) == NULL )   goto sai;

   if ( G->qup++ ) {  ret = 1;  goto sai;  }

   if ((F = fopen( str_arquivo, MODO_RO )) == NULL )   goto sai;

   fseek( F, G->off, 0 );

   /*** Na realidade precisaria apenas de G->max + 1 no G->ptr ***/
   if ((G->str = (SC *) calloc(G->mem + 8, sizeof(SC  ))) == NULL ||
       (G->ptr = (SC **)calloc(G->max + 8, sizeof(SC *))) == NULL ) goto fim;

   for ( I = 0;   I <= G->max;   I++ )   G->ptr[ I ] = str_erro;

   for ( P = G->str;   fgets( linha, MAX_GETS, F ) != NULL;    )
   {
      if ((I = strlen( linha )) == 0 )   continue;

      if ( linha[ I - 1 ]  !=  '\n' )    ate_nl( F );
      else                               linha[ I - 1 ] = '\0';

      if ( strncmp( strupr( strcpy( trab, linha ) ), str_grupo, TG ) == 0 )
      {
         break;
      }

      if ( (I = atoi( linha )) <= 0  || I > G->max )   continue;

      str_trata( trab, linha );

      G->ptr[ I ] = P;
      strcpy( P, trab );
      P += strlen( trab ) + 1;
   }

   ret = 1;

fim:

   if ( F != NULL )   fclose( F );

   if ( ret == 0 )   down_str( numero );

sai:

   return ( ret );
}


SS   start_str( arquivo )

SC   *arquivo;
{
   SC   linha[ MAX_GETS + 10 ], trab[ MAX_GETS + 10 ];
   SS   I, N, ret = 0;
   GRP  *G;
   FILE *F;

   if ( grp_first != NULL )   end_str( );

   F = NULL;
   G = NULL;

   strupr( str_grupo );
   TG = strlen( str_grupo );

   if ( strlen( arquivo ) > MAX_ARQ ||
       (F = fopen( arquivo, MODO_RO )) == NULL )   goto fim;

   while ( fgets( linha, MAX_GETS, F ) != NULL )
   {
      if ((I = strlen( linha )) == 0 )   continue;

      if ( linha[ I - 1 ]  !=  '\n' )    ate_nl( F );
      else                               linha[ I - 1 ] = '\0';

      if ( strncmp( strupr( strcpy( trab, linha ) ), str_grupo, TG ) == 0 )
      {
         G = NULL;

         if ((N = atoi( linha + TG )) > 0  &&
             (G = novo_grupo( N, ftell( F ))) == NULL )   goto fim;

         continue;
      }

      if ( G == NULL )   continue;

      if ((I = atoi( linha )) <= 0  ||  I > MAX_STR )   continue;

      str_trata( trab, linha );

      if ( I > G->max )   G->max = I;

      G->mem += strlen( trab ) + 1;
   }

   if ( ferror( F ) )  goto fim;

   strcpy( str_arquivo, arquivo );

   if ( ! up_str( STR_OBRIG )  ||  ! up_str( STR_EDIT ) )   goto fim;

   ret = 1;

fim:

   if ( F != NULL )   fclose( F );

   if ( ret == 0 )    end_str( );

   return ( ret );
}

SC  *str_trata( dest, orig )

SC   *dest;
SC   *orig;
{
   SC   *P, *Q;
   SS   I;

   /*** DEST = MAX_CHAR, ORIG = MAX_GETS, MAX_GETS > MAX_CHAR ***/

   dest[ 0 ] = '\0';

   for ( P = orig;   *P != ABRE_STR  &&  *P != '\0';   P++ )
      ;

   if ( *P++ == '\0'  ||  *P == '\0' )   goto fim;

   I = strlen( orig );

   for ( Q = orig + I;   *Q != FECHA_STR  &&  Q > P;   Q-- )
      ;

   if ( *Q == FECHA_STR )   *Q = '\0';
   else                     trim( P );

   /*** Garanta que nao vai estourar ***/
   if ( strlen( P ) > MAX_CHAR )   P[ MAX_CHAR ] = '\0';

   strcpy( dest, P );

fim:

   /* Ananias (durante viagem do jairo */
   /* str nula deve ser permitida portanto comentei a linha abaixo. */
   /* if ( dest[ 0 ] == '\0' )   strcpy( dest, str_erro ); */

   return ( dest );
}


GRP *novo_grupo( num, off )

SS   num;
SL   off;
{
   GRP *G;

   if ( num <= 0 || (G = (GRP *) malloc(sizeof(GRP))) == NULL )  return NULL;

   G->max = 0;
   G->mem = 0;
   G->qup = 0;
   G->str = NULL;
   G->ptr = NULL;

   G->num = num;
   G->off = off;

   G->nxt = grp_first;
   grp_first = G;

   return ( G );
}


VOID  end_str( )
{
   GRP  *G;

   for ( G = grp_first;   G != NULL;   G = grp_first )
   {
      grp_first = G->nxt;

      if ( G->ptr != NULL )   free((SC *) G->ptr );
      if ( G->str != NULL )   free((SC *) G->str );

      free((SC *) G );
   }

   str_arquivo[ 0 ] = '\0';
}


VOID  ate_nl( F )

FILE  *F;
{
   SS   C;

   while ((C = fgetc( F )) != EOF  &&  C != '\n' )
      ;
}


