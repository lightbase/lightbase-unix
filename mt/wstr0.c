
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  <window.h>
#include  <mensagem.h>

#ifdef   MSDOS

GRP   *acha_grupo( SS numero );
SC    *str_trata( SC *dest, SC *orig );
GRP   *novo_grupo( SS num, SL off );

#else 

GRP   *acha_grupo( );
SC    *str_trata( );
GRP   *novo_grupo( );

#endif

GRP *grp_first = NULL;
SC  str_arquivo[ MAX_ARQ + 1 ];
SC  *str_erro = "[***]";
SC  *str_grupo = STR_GRUPO;
SS  TG;
SC  str_tmp[ MAX_CHAR + 10 ];

SC   *loc_str( grupo, item )

SS   grupo, item;
{
   SC   linha[ MAX_GETS + 10 ], trab[ MAX_GETS + 10 ], *ret;
   SS   I;
   FILE *F;
   GRP  *G;

   F = NULL;
   ret = str_tmp;

   if ((G = acha_grupo( grupo )) == NULL )
   {
fim_erro:
      sprintf( str_tmp, "%s GRP = %d MEN = %d", str_erro, grupo, item );
      goto fim;
   }

   /*** Se grupo ja esta carregado, volte a mensagem ***/
   if ( item <= G->max && G->qup ) { ret = G->ptr[ item ]; goto fim; }

   if ((F = fopen( str_arquivo, MODO_RO )) == NULL )   goto fim_erro;

   fseek( F, G->off, 0 );

   while ( fgets( linha, MAX_GETS, F ) != NULL )
   {
      if ((I = strlen( linha )) == 0 )   continue;

      if ( linha[ I - 1 ]  !=  '\n' )    ate_nl( F );
      else                               linha[ I - 1 ] = '\0';

      /*** Se encontrou outro GRUPO, mensagem nao existe ***/
      if ( strncmp( strupr( strcpy( trab, linha ) ), str_grupo, TG ) == 0 )
      {
         goto fim_erro;
      }

      if ( (I = atoi( linha )) != item )   continue;

      str_trata( trab, linha );

      strcpy( str_tmp, trab );

      /*** Achou a Mensagem, retorne em "str_tmp" ***/
      break;
   }

fim:

   if ( F != NULL )   fclose( F );

   return ( ret );
}

GRP  *acha_grupo( numero )

SS  numero;
{
   GRP  *G = NULL;

   if ( numero <= 0 )   goto fim;

   for ( G = grp_first;   G != NULL  &&  G->num != numero;   G = G->nxt )
      ;

fim:

   return ( G );
}


SS   stat_str( numero )

SS   numero;
{
   SS   ret = 0;
   GRP  *G;

   if ((G = acha_grupo( numero )) != NULL  )   ret = G->qup;

   return   ret;
}


