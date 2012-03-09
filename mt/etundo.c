
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  "window.h"
#include  "mensagem.h"
#include  "ethead.h"

#include  PROTO

#ifdef    MSDOS
#include  <io.h>
#include  <dir.h>
#else
#ifdef    VAXVMS
SC        *mktemp( );
#include  <file.h>
#else
SC        *mktemp( );
#include  <fcntl.h>
#endif
#endif

/*** E' necessario nos XENIX e IX386 ***/
#ifndef VAXVMS
SL       tell();
#endif

#ifdef   MSDOS
VOID avalia_unredo( ARG *, SS undo, SC *linha );
SL   prox_redo( ARG *, SC *linha );
#else
VOID avalia_unredo( );
SL   prox_redo( );
#endif

#define   MAX_ALLOC  (LINE_LEN + 30)
#define   MAX_GET    (LINE_LEN + 25)
#define   U_HEADER   14 /*** Caracteres antes da linha undo_linha() ***/
#ifdef    ET_UNDO

/***
Deve ser chamada em load_buffer() .
E' responsavel por criar e inicializar um arquivo temporario onde sera' ...
... colocado os retrospectos de edicao para UNDO e REDO.
Usa malloc() para o nome do arquivo, que devera' ser free() em free_buffer().
***/
VOID start_undo( args )

ARG  *args;
{
   SC     *P, *D, temp[ 250 ];
   FILE   *FA;

      /* No VAX p/ criar temos que ter o ponto no final do nome do arquivo */
   static SC masc[] = "UNXXXXXX";

   undo_on = U_NEXT | U_LINHA;

   args->undo = NULL;
   args->redo = 0;
   args->FU   = NULL;
   args->off  = 0L;
   args->ant  = 0L;
   args->pos  = 0L;
   args->flg  = 0;

   if ( args->doc_mode )   return;

#ifdef   MSDOS
   if ((D = getenv( loc_str( STR_OBRIG, OBRIG_32 ))) == NULL ) D = diretorio;
#else
   if ((D = getenv( loc_str( STR_OBRIG, OBRIG_33 ))) == NULL ) D = ".";
#endif

   tira_separador( D );

   sprintf( temp, "%s%c%s", D, DIRSEP, masc );
   if ((P = malloc( strlen( temp ) + 1 )) != NULL )
   {
      strcpy( P, temp );

      if ( mktemp( P ) != NULL  &&  (FA = fopen( P, MODO_BWR )) != NULL )
      {
         fprintf( FA, "Desfaz Refaz\n" );
         args->off  = ftell( FA );
         args->undo = P;
         fclose(  FA );
      }

      else   free( P );
   }
}

/***
Para ser chamada em init_logic().
se nome de arquivo (args->undo) esta inicializado com sucesso, abre o ...
... arquivo em args->FU.
Em end_logic() este arquivo devera' ser fechado!
***/
VOID restart_undo( args )

ARG  *args;
{
   SC   *P;
   FILE *FA;

   if ( args->doc_mode )   return;

   if ((P = args->undo) == NULL ||
      (FA = args->FU = fopen( P, MODO_BRW )) == NULL )
   {
      end_undo( args, args->jm );
   }

   else   fseek( FA, args->off, 0 );
}

VOID close_undo( args )

ARG  *args;
{
   if ( args->FU != NULL )
   {
      fclose( args->FU );
      args->FU = NULL;
   }
}

VOID end_undo( args, mens )

ARG  *args;
WIN  *mens;
{
   if ( ! args->doc_mode  &&  (args->FU == NULL  ||  ferror( args->FU )) )
   {
      errorexists = YES;
      mens_1( mens, loc_str( STR_EDIT, EDIT_41 ));
   }

   close_undo( args );

   if ( args->undo != NULL )
   {
      unlink( args->undo );
      free( args->undo );
      args->undo = NULL;
   }
}

VOID next_undo( args, clin, col )

ARG  *args;
SS   clin, col;
{
   SS   C;
   SL   ant;
   FILE *FA;

   if ( ! (undo_on & U_NEXT)  ||  (FA = args->FU) == NULL )   return;

   C = args->tecla;

   args->redo = 0;

   if ( args->flg )
   {
      fseek( FA, args->ant, 0 );
      ant = args->pos;
   }
   else
   {
      ant = args->pos = args->ant;
      args->ant = args->off;
   }

   fprintf( FA, "%c %d %d %ld %d\n", U_INIC, clin, col, ant, C );

   /*** ASSUMA que o proximo comando vira logo depois ***/
   args->off = ftell( FA );

   if ( C == DESFAZ + 256 )   args->flg = 1;
   else                       args->flg = 0;
}

VOID C_undo( args )

ARG  *args;
{
   SC   cmd, *linha = NULL;
   SI   clin, col, tecla;
   SL   pos;
   FILE *FA;

   if ((FA = args->FU) == NULL )   goto fim;

#ifdef    ET_UNDO
   if ( args->redo == 0 )   next_undo( args, args->clin, args->col );
#endif

   if ((pos = args->pos) == 0 )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_42 ));
      goto fim;
   }

   if ((linha = malloc( MAX_ALLOC )) == NULL )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_528 ));
      goto fim;
   }

   fseek( FA, pos, 0 );

   if ( fgets( linha, MAX_GET, FA ) == NULL )   goto fim;

   sscanf( linha, "%c %d %d %ld %d", &cmd, &clin, &col, &pos, &tecla );

   args->redo++;
   args->off = args->ant;
   args->ant = args->pos;
   args->pos = pos;

   /*** Desfazer tudo ***/
   avalia_unredo( args, 1, linha );

   args->col = col;
   pos_line( args, clin );

   display = disp_file;

fim:

   if ( linha != NULL )   free( linha );
}

VOID C_redo( args )

ARG  *args;
{
   SC   cmd, *linha = NULL;
   SI   clin, col, tecla;
   SL   ant;
   FILE *FA;

   if ((FA = args->FU) == NULL )   goto fim;

   if ( args->redo == 0 )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_43 ));
      goto fim;
   }

   /*** As sobras sao para o header dor UNDO, veja undo_linha() ***/
   if ((linha = malloc( MAX_ALLOC )) == NULL )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_528 ));
      goto fim;
   }

   /*** Volte o comando e ignore a posicao ***/
   fseek( FA, args->ant, 0 );
   if ( fgets( linha, MAX_GET, FA ) == NULL )   goto fim;

   /*** Refazer tudo ***/
   avalia_unredo( args, 0, linha );

   /*** Posicionar ***/
   fseek( FA, args->off, 0 );
   if ( fgets( linha, MAX_GET, FA ) == NULL )   goto fim;

   sscanf( linha, "%c %d %d %ld %d", &cmd, &clin, &col, &ant, &tecla );

   args->redo--;
   args->pos = args->ant;
   args->ant = args->off;
   args->off = prox_redo( args, linha );

   args->col = col;
   pos_line( args, clin );

   display = disp_file;

fim:
   if ( linha != NULL )   free( linha );
}

VOID avalia_unredo( args, undo, linha )

ARG  *args;
SS   undo;
SC   *linha;
{
   SC   *L1, cmd;
   SI   num, tam;
   SI   S1, S2, S3, S4;
   SS   qlin = 0;
   FILE *FA;

   if ((FA = args->FU) == NULL )   return;

   while ( 1 )
   {
      if ( fgets( linha, MAX_GET, FA ) == NULL )   goto fim;

      sscanf( linha, "%c %d %d", &cmd, &num, &tam );

      /*** Se for UNDO inverta os comandos ***/
      if ( undo )
      {
         switch ( cmd )
         {
         case U_ANTES : cmd = U_DEPOIS;   break;
         case U_DEPOIS: cmd = U_ANTES;    break;
         case U_ENTRA : cmd = U_SAI;      break;
         case U_SAI   : cmd = U_ENTRA;    break;
         }
      }

      switch ( cmd )
      {
      default      : /*** end_undo( args ); ***/
      case U_INIC  : goto fim;

      case U_ANTES : continue;

      case U_DEPOIS: linalloc( args, num, args->len[ num ], tam );
                     if ((L1 = vm_deref( args->ptr[ num ], 0 )) != NULL )
                     {
                        memmove((UC *) L1, linha + U_HEADER, tam );
                     }
                     continue;

      case U_ENTRA :
                     ins_linha( args, num );
                     linalloc( args, num, args->len[ num ], tam );
                     if ((L1 = vm_deref( args->ptr[ num ], 0 )) != NULL )
                     {
                        memmove((UC *) L1, linha + U_HEADER, tam );
                     }
                     if ( args->admlin != NULL )
                     {
                        (*args->admlin)( args, num, LIN_ENTRA );
                     }
                     continue;

      case U_SAI   :
                     num -= qlin++;
                     ret_linha( args, num );
                     if ( args->admlin != NULL )
                     {
                        (*args->admlin)( args, num, LIN_SAI );
                     }
                     continue;

      case U_BLOCK : S1 = S2 = S3 = S4 = 0;

                     if ( fgets( linha, MAX_GET, FA ) == NULL )   goto fim;
                     sscanf( linha, "%c %d %d %d %d", &cmd,&S1,&S2,&S3,&S4 );

                     if ( undo ){ blk_tipo = num; blk_li = S1; blk_ci = S2; }
                     else       { blk_tipo = tam; blk_li = S3; blk_ci = S4; }
                     continue;
      }
   }

fim:

   return;
}

/*** Modificacoes nas Linhas - Comandos para UNDO/REDO  ***/
VOID undo_linha( args, cmd, num, tam, linha )

ARG  *args;
SS   cmd;
SS   num;
SS   tam;
SC   *linha;
{
   FILE *FA;

   if ( ! (undo_on & U_LINHA)  ||  (FA = args->FU) == NULL )   return;

   /*** ATENCAO: Se mudar este fprintf(), ATUALIZE U_HEADER ***/
   /*** U_HEADER = 14 caracteres antes da linha ***/
   fprintf( FA, "%c %5d %5d ", cmd, num, tam );

   if ( tam  &&  linha != NULL )    fwrite( linha, tam, 1, FA );

   fputc( '\n', FA );

   /*** ASSUMA que o proximo comando vira' logo depois ***/
   args->off = ftell( FA );

   if ( ferror( FA ) )    end_undo( args, args->jm );
}

/*** Modificacoes nos Blocos - Comandos para UNDO/REDO  ***/
VOID undo_block( args, cmd, ss1, ss2, ss3, ss4 )

ARG  *args;
SS   cmd, ss1, ss2, ss3, ss4;
{
   FILE *FA;

   if ( ! (undo_on & U_LINHA)  ||  (FA = args->FU) == NULL )   return;

   fprintf( FA, "%c %d %d %d %d", cmd, ss1, ss2, ss3, ss4 );

   fputc( '\n', FA );

   /*** ASSUMA que o proximo comando vira' logo depois ***/
   args->off = ftell( FA );

   if ( ferror( FA ) )    end_undo( args, args->jm );
}

SL   prox_redo( args, linha )

ARG *args;
SC  *linha;
{
   SL   ret;
   FILE *FA;

   if ((FA = args->FU) == NULL )   return( 0 );

   while ( 1 )
   {
      ret = ftell( FA );

      if ( fgets( linha, MAX_GET, FA ) == NULL )   break;

      if ( linha[ 0 ] == U_INIC )                  break;
   }

   fseek( FA, ret, 0 );

   return ( ret );
}

#endif

