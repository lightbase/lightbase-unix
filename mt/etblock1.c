
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  "window.h"
#include  "ethead.h"
#include  "mensagem.h"

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

#include  MEMORY

#ifdef    PROTO_ANSI
VOID row_blk( ARG *args, SS row );
#else
VOID row_blk( );
#endif

static SC  *scrap_file[ MAX_TEMP ] =
{
   /*** Para o VAX temos que ter . no final do nome do arquivo */
   /*** A primeira posicao indica se o arquivo foi usado. ***/
   /*** 0 - arquivo nao criado. ***/
   /*** 1 - arquivo aberto com sucesso. ***/
   /*** 2 - arquivo com error. ***/
   NULL,  /* "0TAXXXXXX"  Scrap 0 */
   NULL,  /* "0TBXXXXXX"  Scrap 1 */
   NULL,  /* "0TCXXXXXX"  Scrap 2 */
   NULL,  /* "0TDXXXXXX"  Scrap 3 */
   NULL,  /* "0TEXXXXXX"  Scrap 4 */
   NULL,  /* "0TFXXXXXX"  Scrap 5 */
   NULL,  /* "0TGXXXXXX"  Scrap 6 */
   NULL,  /* "0THXXXXXX"  Scrap 7 */
   NULL,  /* "0TIXXXXXX"  Scrap 8 */
   NULL,  /* "0TJXXXXXX"  Scrap 9 */
   NULL,  /* "0TKXXXXXX"  10 - Macro Default */
};


/***
Inicializa as mascaras de arquivos temporarios.
***/
VOID scrap_inic( )
{
   SS   I, J;
   SC   *P, *D, temp[ 250 ];

   for ( I = 0;   I < MAX_TEMP;   I++ )
   {
#ifdef   MSDOS
      if((D = getenv( loc_str(STR_OBRIG, OBRIG_30))) == NULL ) D = diretorio;
#else
      if((D = getenv( loc_str(STR_OBRIG, OBRIG_31))) == NULL ) D = ".";
#endif

      tira_separador( D );

      sprintf( temp, "0%s%c%c%cXXXXXX", D, DIRSEP, 'T', 'A' + I );

      J = strlen( temp ) + 1;

      if ((P = malloc( J )) != NULL )   strcpy( P, temp );

      scrap_file[ I ] = P;
   }
}

/***
Frees todos os nomes de arquivos temporarios.
***/
VOID scrap_free( )
{
   SS   I;
   SC   *P;

   for ( P = scrap_file[ I = 0 ];   I < MAX_TEMP;   P = scrap_file[ ++I ] )
   {
      if ( P != NULL )   free( P );

      scrap_file[ I ] = NULL;
   }
}

/***
Unlink todos os arquivos temporarios.
***/
VOID  scrap_unlink( )
{
   SC   *P;
   SS   I, J;

   for ( P = scrap_file[ I = 0 ];   I < MAX_TEMP;   P = scrap_file[ ++I ] )
   {
      if ( P != NULL )
      {
         if ( *P == '1' )   unlink( P + 1 );

         *P = '0';
         for ( P = strchr( P, 0 ) - 1, J = 0;   J < 6;   J++ )  *P-- = 'X';
      }
   }
}

/***
Abre o scrap corrente para gravacao, retorna "FILE *".
***/
FILE  *scrap_write( args, scrap_no, append )

ARG *args;
SS  scrap_no;
SS  append;
{
   SC    *P, *modo;
   FILE  *FA;

   args = args;

   scrap_no = COTA( 0, scrap_no, MAX_TEMP - 1 );

   if ((P = scrap_file[ scrap_no ]) == NULL )   goto ret_null;

   if ( *P == '0'  &&  mktemp( P + 1 ) == NULL )   *P = '2';

   /*** Macros exigem gravacao e binario porisso MODO_BWR ***/
   if ( scrap_no == MACRO_TMP )   modo = MODO_BWR;
   else if ( append )             modo = MODO_AO;
   else                           modo = MODO_WO;

   if ( *P == '2'  ||  (FA = fopen( P + 1, modo )) == NULL )
   {
      *P = '2';
ret_null:
      errorexists = YES;
      beep( args );
      if ( scrap_no == MACRO_TMP ) /* Se for macro */
         mens_1( args->jm, loc_str( STR_EDIT, EDIT_526 ));
      else
         mens_1( args->jm, loc_str( STR_EDIT, EDIT_525 ));
      return( NULL );
   }

   *P = '1';

   return ( FA );
}

/***
Abre o scrap corrente para leitura, retorna "FILE *".
***/
FILE  *scrap_read( args, scrap_no )

ARG *args;
SS  scrap_no;
{
   SC    *P, *modo;
   FILE  *FA;

   args = args;

   scrap_no = COTA( 0, scrap_no, MAX_TEMP - 1 );

   if ((P = scrap_file[ scrap_no ]) == NULL || *P != '1' )   goto ret_null;

   modo = MODO_RO;

   /*** Macros exige binario porisso MODO_BRO ***/
   if ( scrap_no == MACRO_TMP )   modo = MODO_BRO;

   if ((FA = fopen( P + 1, modo )) == NULL )
   {
      *P = '2';
ret_null:
      errorexists = YES;
      beep( args );
      if ( scrap_no == MACRO_TMP )   /* Se for Macro */
         mens_1( args->jm, loc_str( STR_EDIT, EDIT_527 ));
      else
         mens_1( args->jm, loc_str( STR_EDIT, EDIT_522 ), scrap_no );
      return( NULL );
   }

   return ( FA );
}

#ifdef   ET_UNDO
/****************************************************************************/
/* Marca um ponto de bloco no UNDO/REDO */

VOID next_block( args, lin, col, tipo, li, ci )

ARG  *args;
SS   lin, col, tipo, li, ci;
{
   next_undo(  args, lin, col );
   undo_linha( args, U_BLOCK, blk_tipo, tipo, NULL );
   undo_block( args, U_BLOCK, blk_li, blk_ci, li, ci );
}
#endif

/****************************************************************************/
/* hilight line block */
VOID C_line_blk( args )

ARG *args;
/****************************************************************************/
{
   SS   clin, col, li, ci;

   clin = args->clin;
   col  = args->col;
   li   = blk_li;
   ci   = blk_ci;

   if ( blk_inifim  && blk_tipo == BLK_LIN )
   {
      blk_copy( args, blk_scrap, 0 );
      return;
   }

   if ( blk_tipo == BLK_NOT )
   {
      li = clin;
      ci = cur_aparente( args, clin, col );
   }

#ifdef   ET_UNDO
   next_block( args, clin, col, BLK_LIN, li, ci );
#endif

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_44 ), blk_scrap );

   blk_tipo = BLK_LIN;
   blk_li   = li;
   blk_ci   = ci;
}

/****************************************************************************/
VOID C_col_blk( args )             /* hilight the character block */

ARG *args;
/****************************************************************************/
{
   SS   clin, col, li, ci;

   clin = args->clin;
   col  = args->col;
   li   = blk_li;
   ci   = blk_ci;

   if ( blk_inifim  && blk_tipo == BLK_COL )
   {
      blk_copy( args, blk_scrap, 0 );
      return;
   }

   if ( blk_tipo == BLK_NOT )
   {
      li = clin;
      ci = cur_aparente( args, clin, col );
   }

#ifdef   ET_UNDO
   next_block( args, clin, col, BLK_COL, li, ci );
#endif

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_45 ), blk_scrap );

   blk_tipo = BLK_COL;
   blk_li   = li;
   blk_ci   = ci;
}

/****************************************************************************/
/* hilight character block */
VOID C_chr_blk( args )

ARG *args;
/****************************************************************************/
{
   SS   clin, col, li, ci;

   clin = args->clin;
   col  = args->col;
   li   = blk_li;
   ci   = blk_ci;

   if ( blk_inifim  && blk_tipo == BLK_CHR )
   {
      blk_copy( args, blk_scrap, 0 );
      return;
   }

   if ( blk_tipo == BLK_NOT )
   {
      li = clin;
      ci = cur_aparente( args, clin, col );
   }

#ifdef   ET_UNDO
   next_block( args, clin, col, BLK_CHR, li, ci );
#endif

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_46 ), blk_scrap );

   blk_tipo = BLK_CHR;
   blk_li   = li;
   blk_ci   = ci;
}

/****************************************************************************/
/* normlize beginning and ending of hilighted blocks */
VOID set_block( args )

ARG *args;
/****************************************************************************/
{
   SS tmp;

   args = args;

   if ( blk_tipo == BLK_CHR )
   {
      if ( blk_li > blk_lf )
      {
         tmp = blk_li; blk_li = blk_lf; blk_lf = tmp;
         tmp = blk_ci; blk_ci = blk_cf; blk_cf = tmp;
      }

      else if ( blk_li == blk_lf  &&  blk_ci > blk_cf )
      {
         tmp = blk_ci; blk_ci = blk_cf; blk_cf = tmp;
      }
   }

   else
   {
      if ( blk_li > blk_lf ) { tmp = blk_li; blk_li = blk_lf; blk_lf = tmp; }
      if ( blk_ci > blk_cf ) { tmp = blk_ci; blk_ci = blk_cf; blk_cf = tmp; }
   }
}

/***************************************************************************/
/*** Desmarca BLOCO ***/
VOID C_blk_not( args )

ARG *args;
/***************************************************************************/
{
   args = args;

#ifdef   ET_UNDO
   if ( blk_tipo != BLK_NOT )
   {
      next_block( args, args->clin, args->col, BLK_NOT, blk_li, blk_ci );

      blk_not( args );

      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_47 ));
   }
#endif
}

/***************************************************************************/
/*** Desmarca BLOCO ***/
VOID blk_not( args )

ARG *args;
/***************************************************************************/
{
   args = args;

   blk_tipo = BLK_NOT;

   display = disp_file;
}

/****************************************************************************/
/* copia para scrap e deleta */
VOID blk_move( args, scrap_no, append )

ARG *args;
SS  scrap_no;
SS  append;
/****************************************************************************/
{
   SS    s_col, s_row, s_clin;
   FILE  *FA;

   if ( ! blk_sel( args ) )   return;

   if ( append  &&  *scrap_file[ scrap_no ] != '1' )   append = 0;

   if ((FA = scrap_write( args, scrap_no, append )) == NULL )   return;

   args->f_mod = 1;

#ifdef   ET_UNDO
   next_block( args, args->clin, args->col, BLK_NOT, blk_li, blk_ci );
#endif

   set_block( args );

   if ( args->forma == ET_APPEND  &&  blk_li < args->alin )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_36));
      beep( args );
      return;
   }

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_48 ), scrap_no );

   s_col  = pos_coluna( args, blk_li, blk_ci );
   s_row  = args->row;
   s_clin = args->clin;

   switch ( blk_tipo )
   {
   case BLK_LIN :  move_lin_block( args, FA, append );   break;
   case BLK_COL :  move_col_block( args, FA, append );   break;
   case BLK_CHR :  move_chr_block( args, FA, append );   break;
   }

   blk_not( args );

   fclose( FA );

   args->clin = blk_li;
   args->col  = s_col;

   row_blk( args, s_row + args->clin - s_clin );

   errorexists = YES;
   if ( append )  mens_1( args->jm, loc_str( STR_EDIT, EDIT_55 ), scrap_no );
   else           mens_1( args->jm, loc_str( STR_EDIT, EDIT_38 ), scrap_no );
}

/****************************************************************************/
/* delete a block */
VOID blk_delete( args )

ARG *args;
/****************************************************************************/
{
   SS    s_col, s_row, s_clin;

   if ( ! blk_sel( args ) )   return;

   args->f_mod = 1;

#ifdef   ET_UNDO
   next_block( args, args->clin, args->col, BLK_NOT, blk_li, blk_ci );
#endif

   set_block( args );

   if ( args->forma == ET_APPEND  &&  blk_li < args->alin )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_36));
      beep( args );
      return;
   }

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_49 ));

   s_col  = pos_coluna( args, blk_li, blk_ci );
   s_row  = args->row;
   s_clin = args->clin;

   switch ( blk_tipo )
   {
   case BLK_LIN :  del_lin_block( args );   break;
   case BLK_COL :  del_col_block( args );   break;
   case BLK_CHR :  del_chr_block( args );   break;
   }

   blk_not( args );

   args->clin = blk_li;
   args->col  = s_col;

   row_blk( args, s_row + args->clin - s_clin );

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_40 ));
}

/***************************************************************************/
/*** Coloca o bloco no scrap corrente ***/
VOID blk_copy( args, scrap_no, append )

ARG *args;
SS  scrap_no;
SS  append;
/***************************************************************************/
{
   FILE *FA;

   if ( ! blk_sel( args ) )   return;

   if ( append  &&  *scrap_file[ scrap_no ] != '1' )   append = 0;

   if ((FA = scrap_write( args, scrap_no, append )) == NULL )   return;

#ifdef   ET_UNDO
   next_block( args, args->clin, args->col, BLK_NOT, blk_li, blk_ci );
#endif

   set_block( args );

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_50 ), scrap_no );

   switch ( blk_tipo )
   {
   case BLK_LIN :  copy_lin_block( args, FA, append );   break;
   case BLK_COL :  copy_col_block( args, FA, append );   break;
   case BLK_CHR :  copy_chr_block( args, FA, append );   break;
   }

   blk_not( args );

   fclose( FA );

   errorexists = YES;
   if ( append )  mens_1( args->jm, loc_str( STR_EDIT, EDIT_56 ), scrap_no );
   else           mens_1( args->jm, loc_str( STR_EDIT, EDIT_39 ), scrap_no );
}

/****************************************************************************/
/* insert a block from scrap */
VOID blk_ins( args, scrap_no )

ARG *args;
SS  scrap_no;
/****************************************************************************/
{
   SS    s_row, s_clin;
   SC    linha[ 100 ];
   FILE  *FA;

   linha[ 0 ] = '\0';
   s_row  = args->row;
   s_clin = args->clin;

   if ((FA = scrap_read( args, scrap_no )) == NULL )   return;

   if ( fgets( linha, 80, FA ) == NULL )   return;

   args->f_mod = 1;

   C_blk_not( args );

#ifdef   ET_UNDO
   next_undo(  args, args->clin, args->col );
#endif

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_51 ), scrap_no );

   switch ( linha[ 0 ] )
   {
   case BLK_LIN :   ins_lin_block( args, FA );    break;
   case BLK_COL :   ins_col_block( args, FA );    break;
   case BLK_CHR :   ins_chr_block( args, FA );    break;
   }

   row_blk( args, s_row + args->clin - s_clin );

   fclose( FA );

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_52 ), scrap_no );
}

/***************************************************************************/
/*** Transforma os caracteres em um bloco em MAIUSCULAS ou MINUSCULAS ***/
VOID blk_upplwr( args, upplwr )

ARG *args;
SS  upplwr;  /*** BLK_UPPER ou BLK_LOWER ***/
/***************************************************************************/
{
   if ( ! blk_sel( args ) )   return;

#ifdef   ET_UNDO
   next_block( args, args->clin, args->col, BLK_NOT, blk_li, blk_ci );
#endif

   args->f_mod = 1;

   set_block( args );

   errorexists = YES;
   if ( upplwr == BLK_UPPER ) mens_1( args->jm, loc_str( STR_EDIT, EDIT_57 ));
   else                       mens_1( args->jm, loc_str( STR_EDIT, EDIT_58 ));

   switch ( blk_tipo )
   {
   case BLK_LIN :  UL_lin_block( args, upplwr );   break;
   case BLK_COL :  UL_col_block( args, upplwr );   break;
   case BLK_CHR :  UL_chr_block( args, upplwr );   break;
   }

   blk_not( args );
}

/*************************************************************************/
/* copy a line block para o SCRAP corrente */
/* Deleta o bloco */
VOID move_lin_block( args, FA, append )

ARG  *args;
FILE *FA;
SS   append;
/*************************************************************************/
{
   copy_lin_block( args, FA, append );
   del_lin_block(  args );
}

/****************************************************************************/
/* delete a line block NAO coloca no Scrap */
VOID del_lin_block( args )

ARG *args;
/****************************************************************************/
{
   SC   *L1;
   SS   I, total, len, tam;

   for( I = blk_li;   I <= blk_lf;   I++ )
   {
      len = args->len[ I ];
#ifdef   ET_UNDO
      tam = len;
      if ((L1 = vm_deref( args->ptr[ I ], 0 )) == NULL )   tam = 0;
      undo_linha( args, U_SAI, I, tam, L1 );
#endif
      if ( args->admlin != NULL )   (*args->admlin)( args, I, LIN_SAI );
      linalloc( args, I, len, 0 );
   }

   memmove( (UC *) &args->ptr[ blk_li ],
            (UC *) &args->ptr[ blk_lf + 1 ],
            (args->t_lines - 1 - blk_lf) * sizeof( PTR ) );

   memmove( (UC *) &args->len[ blk_li ],
            (UC *) &args->len[ blk_lf + 1 ],
            (args->t_lines - 1 - blk_lf) * sizeof( SS ) );

   total = args->t_lines;

   /*** args->t_lines -= blk_lf - blk_li + 1;  Sera' que ta' certo ? ***/

   args->t_lines = args->t_lines - (blk_lf - blk_li + 1);

   for( I = args->t_lines;   I < total;   I++ )
   {
       args->ptr[ I ] = NULL;
       args->len[ I ] = 0;
   }

   if ( args->t_lines == 0 )   args->t_lines = 1;
}

/*************************************************************************/
/* copy a line block para o SCRAP corrente */
VOID copy_lin_block( args, FA, append )

ARG  *args;
FILE *FA;
SS   append;
/*************************************************************************/
{
   SC    *L1;
   SS    I;

   if ( ! append ) {  fputc( BLK_LIN, FA );   fputc( '\n', FA );  }

   for( I = blk_li;   I <= blk_lf;   I++ )
   {
      if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
      {
         fwrite( L1, args->len[ I ], 1, FA );
      }

      fputc( '\n', FA );

      if ( ferror( FA ) )   break;
   }
}


/*************************************************************************/
/* Insere a line block do SCRAP corrente */
VOID ins_lin_block( args, FA )

ARG   *args;
FILE  *FA;
/*************************************************************************/
{
   SS    tam, max_len, a_ler, clin;
   SC    *L1, *line = NULL;

   a_ler   = ( args->doc_mode ) ? MAX_WRAP * 3 / 4 : LINE_LEN;
   max_len = ( args->doc_mode ) ? MAX_WRAP : LINE_LEN;

   if ((line = malloc( max_len + 10 )) == NULL )   return;

   while ( 1 )
   {
      if ( fgets((SC *) line, a_ler, FA )  ==  NULL )   goto fim;

      strip_nl( line );

      tam = strlen( line );

      if ( args->t_lines >= args->max_lines   &&
           ajusta_array( args, INC_LIN + args->t_lines ) == 0 )   goto fim;

      clin = args->clin;

      ins_linha( args, clin );

      linalloc( args, clin, args->len[ clin ], tam );

      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
      {
         memmove((UC *) L1, line, tam );
      }

#ifdef   ET_UNDO
      if ( L1 == NULL )   tam = 0;
      undo_linha( args, U_ENTRA, clin, tam, L1 );
#endif
      if ( args->admlin != NULL )   (*args->admlin)( args, clin, LIN_ENTRA );

      args->clin++;
   }

fim:

   free( line );
}


/*************************************************************************/
/* Coloca o BLOCO e MAIUSCULA ou MINUSCULA ***/
VOID UL_lin_block( args, upplwr )

ARG  *args;
SS   upplwr; /* BLK_UPPER ou BLK_LOWER */
/*************************************************************************/
{
   SC    *L1;
   SS    I;

   for( I = blk_li;   I <= blk_lf;   I++ )
   {
      if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
      {
         lin_applwr( args, L1, I, 0, args->len[ I ], upplwr );
      }
   }
}


SS   blk_sel( args )

ARG *args;
{
   if ( blk_tipo == BLK_NOT )
   {
      errorexists = YES;
      beep( args );
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_519 ));
      return( NOT );
   }

   return ( YES );
}

VOID row_blk( args, row )

ARG  *args;
SS   row;
{
   SS   max = args->je->w_height - 1;

   args->row = COTA( 0, row, max );

   row_touch(  args, args->row );
   clin_touch( args, args->clin );
}


