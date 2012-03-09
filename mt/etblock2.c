
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>

#include  "window.h"
#include  "ethead.h"
#include  "mensagem.h"

#include  MEMORY

/*************************************************************************/
/* copy a column block para o SCRAP corrente */
/* Deleta o bloco */
VOID move_col_block( args, FA, append )

ARG  *args;
FILE *FA;
SS   append;
/*************************************************************************/
{
   copy_col_block( args, FA, append );
   del_col_block(  args );
}

/****************************************************************************/
VOID del_col_block( args )

ARG *args;
/****************************************************************************/
{
   SC  *L1;
   SS  I, del, mov, ci, cf, len, tam;

   for( I = blk_li;   I <= blk_lf;   I++ )
   {
      ci = pos_coluna( args, I, blk_ci );
      cf = pos_coluna( args, I, blk_cf );

      if ((UC) e_et_atr( args, I, cf, NULL ) == ET_FORM1 )   cf += TMF;

      len = args->len[ I ];

      if ( len  >  ci )
      {
         if ( len > cf )   del = cf - ci + 1;

         else              del = len - ci;

         L1 = vm_deref( args->ptr[ I ], 0 );

#ifdef   ET_UNDO
         tam = len;
         if ( L1 == NULL )   tam = 0;
         undo_linha( args, U_ANTES, I, tam, L1 );
#endif

         if ( len  >  cf + 1 )
         {
            mov = len - cf - 1;

            if ( L1 != NULL )   memmove( &L1[ ci ], &L1[ ci + del ], mov );
         }

#ifdef   ET_UNDO
         tam = len - del;
         if ( L1 == NULL )   tam = 0;
         undo_linha( args, U_DEPOIS, I, tam, L1 );
#endif

         linalloc( args, I, len, len - del );
      }
   }
}

/****************************************************************************/
/* Copia bloco coluna para o SCRAP corrente */
VOID copy_col_block( args, FA, append )

ARG  *args;
FILE *FA;
SS   append;
/****************************************************************************/
{
   SC    *L1;
   SS    I, J, K, in, semi, quant, grav, ci, cf;

   if ( ! append ) {  fputc( BLK_COL, FA );   fputc( '\n', FA );  }

   for ( I = blk_li;   I <= blk_lf;   I++ )
   {
      grav = semi = in = 0;

      ci = pos_coluna( args, I, blk_ci );
      cf = pos_coluna( args, I, blk_cf );

      if ((UC) e_et_atr( args, I, cf, NULL ) == ET_FORM1 )   cf += TMF;

      quant = cf - ci + 1;

      K = args->len[ I ] - 1;

      if (      cf <= K )   in   = 1;
      else if ( ci <= K )   semi = 1;

      if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
      {
         if      ( in   )   grav = quant;
         else if ( semi )   grav = args->len[ I ] - ci;
         else               grav = 0;

         if ( grav )   fwrite( &L1[ ci ], grav, 1, FA );
      }

      for ( J = grav;   J < quant;   J++ )   fputc( ' ', FA  );

      fputc( '\n', FA  );
   }
}

/*************************************************************************/
/* Insere bloco coluna do SCRAP corrente */
VOID ins_col_block( args, FA )

ARG   *args;
FILE  *FA;
/*************************************************************************/
{
   SS    I, J, K, tam, max_len, a_ler, clin, len;
   SC    *L1, *line = NULL;

   a_ler   = ( args->doc_mode ) ? MAX_WRAP * 3 / 4 : LINE_LEN;
   max_len = ( args->doc_mode ) ? MAX_WRAP : LINE_LEN;

   if ((line = malloc( max_len + 10 )) == NULL )   return;

   while ( 1 )
   {
      if ( fgets((SC *) line, a_ler, FA )  ==  NULL )   goto fim;

      strip_nl( line );

      tam = strlen( line );

      clin = args->clin;

      I = args->len[ clin ];
      J = args->col;

      if ( J >= I )   K = J + tam;
      else            K = I + tam;

#ifdef   ET_UNDO
      len = I;
      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   len = 0;
      undo_linha( args, U_ANTES, clin, len, L1 );
#endif

      linalloc( args, clin, I, K );

      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
      {
         if ( J >= I )   memset( &L1[ I ], ' ', J - I );

         else            memmove((UC *) &L1[ J + tam ], &L1[ J ], I - J );

         memmove((UC *) &L1[ J ], line, tam );
      }

#ifdef   ET_UNDO
      len = K;
      if ( L1 == NULL )   len = 0;
      undo_linha( args, U_DEPOIS, clin, len, L1 );
#endif

      if ( clin >= args->t_lines - 1 )
      {
         if ( args->t_lines >= args->max_lines   &&
              ajusta_array( args, INC_LIN + args->t_lines ) == 0 )  goto fim;

         ins_linha( args, clin + 1 );

#ifdef   ET_UNDO
         undo_linha( args, U_ENTRA, clin + 1, 0, L1 );
#endif
         if ( args->admlin != NULL )
            (*args->admlin)( args, clin + 1, LIN_ENTRA );
      }

      args->clin++;
   }

fim:

   free( line );
}

/****************************************************************************/
/* Coloca um bloco Coluna em UPPER ou LOWER */
VOID UL_col_block( args, upplwr )

ARG  *args;
SS   upplwr;
/****************************************************************************/
{
   SC    *L1;
   SS    I, K, in, semi, quant, grav, ci, cf;

   for ( I = blk_li;   I <= blk_lf;   I++ )
   {
      grav = semi = in = 0;

      ci = pos_coluna( args, I, blk_ci );
      cf = pos_coluna( args, I, blk_cf );

      quant = cf - ci + 1;

      K = args->len[ I ] - 1;

      if (      cf <= K )   in   = 1;
      else if ( ci <= K )   semi = 1;

      if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
      {
         if (      in   )   grav = quant;
         else if ( semi )   grav = args->len[ I ] - ci;
         else               grav = 0;

         if ( grav )   lin_applwr( args, L1, I, ci, grav, upplwr );
      }
   }
}

/*************************************************************************/
/* copy a CHAR block para o SCRAP corrente */
/* Deleta o bloco */
VOID move_chr_block( args, FA, append )

ARG  *args;
FILE *FA;
SS   append;
/*************************************************************************/
{
   copy_chr_block( args, FA, append );
   del_chr_block(  args );
}

/****************************************************************************/
/* delete um bloco CHAR e NAO coloca no SCRAP */
VOID del_chr_block( args )

ARG *args;
/****************************************************************************/
{
   SC  *L1, *L2;
   SS  I, TI, TF, quant, tam, ci, cf, len, TU;

   ci = pos_coluna( args, blk_li, blk_ci );
   cf = pos_coluna( args, blk_lf, blk_cf );

   if ((UC) e_et_atr( args, blk_lf, cf, NULL ) == ET_FORM1 )   cf += TMF;

   TI = args->len[ blk_li ];
   TF = args->len[ blk_lf ];

   if ( cf >= TF )   cf = TF - 1;

   if ( blk_li == blk_lf )
   {
      if ( ci >= TI )   return;

      L1 = vm_deref( args->ptr[ blk_li ], 0 );

#ifdef   ET_UNDO
      TU = TI;
      if ( L1 == NULL )   TU = 0;
      undo_linha( args, U_ANTES, blk_li, TU, L1 );
#endif

      if ( L1 != NULL )   memmove( &L1[ ci ], &L1[ cf + 1 ], TI - cf - 1 );

      linalloc( args, blk_li, TI, TI - (cf - ci + 1) );

      len = args->len[ blk_li ];

#ifdef   ET_UNDO
      TU = len;
      if ((L1 = vm_deref( args->ptr[ blk_li ], 0 )) == NULL )   TU = 0;
      undo_linha( args, U_DEPOIS, blk_li, TU, L1 );
#endif

/***
      if ( len == 0 )
      {
#ifdef   ET_UNDO
         undo_linha( args, U_SAI, blk_li, 0, L1 );
#endif
         if ( args->admlin != NULL )
            (*args->admlin)( args, blk_li, LIN_SAI );
         ret_linha( args, blk_li );
         if ( args->clin > blk_li )   args->clin--;
      }
***/
   }

   else
   {
      /*** Junte a primeira linha com a ultima !!! ***/

      quant = TF - cf - 1;

      if ( quant < 0 )   quant = 0;

      tam = ci + quant;

      len = TI;

#ifdef   ET_UNDO
      TU = TI;
      if ((L1 = vm_deref( args->ptr[ blk_li ], 0 )) == NULL )   TU = 0;
      undo_linha( args, U_ANTES, blk_li, TU, L1 );
#endif

      linalloc( args, blk_li, len, tam );

      if ( tam > 0  &&  quant > 0 )
      {
         L1 = vm_deref( args->ptr[ blk_li ], 1 );
         L2 = vm_deref( args->ptr[ blk_lf ], 0 );

         if ( L1 != NULL )
         {
            for ( I = len;  I < tam;  I++ )   L1[ I ] = ' ';

            if ( L2 != NULL )   memmove( &L1[ ci ], &L2[ cf + 1 ], quant );
         }

         vm_unlock( args->ptr[ blk_li ] );
      }

#ifdef   ET_UNDO
      TU = tam;
      if ((L1 = vm_deref( args->ptr[ blk_li ], 0 )) == NULL )   TU = 0;
      undo_linha( args, U_DEPOIS, blk_li, TU, L1 );
#endif

      for ( I = blk_li + 1;   I < blk_lf + 1;   I++ )
      {
         len = args->len[ I ];
#ifdef   ET_UNDO
         TU = len;
         if ((L1 = vm_deref( args->ptr[ I ], 0 )) == NULL )   TU = 0;
         undo_linha( args, U_SAI, I, TU, L1 );
#endif
         if ( args->admlin != NULL )
            (*args->admlin)( args, I, LIN_SAI );
         linalloc( args, I, len, 0 );
      }

      memmove((UC *) &args->ptr[ blk_li + 1 ],
              (UC *) &args->ptr[ blk_lf + 1 ],
              (args->t_lines - 1 - blk_lf) * sizeof(PTR) );

      memmove((UC *) &args->len[ blk_li + 1 ],
              (UC *) &args->len[ blk_lf + 1 ],
              (args->t_lines - 1 - blk_lf) * sizeof(SS) );

      args->t_lines = args->t_lines - (blk_lf - blk_li);

      if ( args->t_lines == 0 )
      {
         args->t_lines = 1;
         args->ptr[ 0 ] = NULL;
         args->len[ 0 ] = 0;
      }
   }
}

/****************************************************************************/
/* copia o bloco CHAR para o SCRAP corrente */
VOID copy_chr_block( args, FA, append )

ARG  *args;
FILE *FA;
SS   append;
/****************************************************************************/
{
   SC    *L1;
   SS    I, J, ci, cf;

   if ( ! append ) {  fputc( BLK_CHR, FA );   fputc( '\n', FA );  }

   ci = pos_coluna( args, blk_li, blk_ci );
   cf = pos_coluna( args, blk_lf, blk_cf );

   if ((UC) e_et_atr( args, blk_lf, cf, NULL ) == ET_FORM1 )   cf += TMF;

   J = args->len[ blk_lf ];

   if ( cf >= J )   cf = J - 1;

   if ( blk_li == blk_lf )
   {
      if ( ci < args->len[ blk_li ] )
      {
         if ((L1 = vm_deref( args->ptr[ blk_li ], 0 )) != NULL )
         {
            fwrite( &L1[ ci ], cf - ci + 1, 1, FA );
         }
      }

      fputc( '\n', FA );
   }

   else
   {
      if ((L1 = vm_deref( args->ptr[ blk_li ], 0 )) != NULL )
      {
         if ( ci < args->len[ blk_li ] )
         {
            fwrite( &L1[ ci ], args->len[ blk_li ] - ci, 1, FA );
         }
      }

      fputc( '\n', FA );

      for( I = blk_li + 1;   I <= blk_lf - 1;   I++ )
      {
         if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
         {
            fwrite( L1, args->len[ I ], 1, FA );
         }

         fputc( '\n', FA );

         if ( ferror( FA ) )   break;
      }

      if ((L1 = vm_deref( args->ptr[ blk_lf ], 0 )) != NULL )
      {
         fwrite( L1, cf + 1, 1, FA );
      }

      fputc( '\n', FA );
   }
}

/*************************************************************************/
/* Insere a CHAR block do SCRAP corrente */
VOID ins_chr_block( args, FA )

ARG   *args;
FILE  *FA;
/*************************************************************************/
{
   SS    tam, a_ler, max_len, l_cur, c_cur, len, clin;
   SC    *L1, *line = NULL;

   c_cur = args->col;
   l_cur = args->clin;

   a_ler   = ( args->doc_mode ) ? MAX_WRAP * 3 / 4 : LINE_LEN;
   max_len = ( args->doc_mode ) ? MAX_WRAP : LINE_LEN;

   if ((line = malloc( max_len + 10 )) == NULL )   return;

#ifdef   ET_UNDO
   undo_on = U_LINHA;
#endif

   split_line( args, 0, 0 );

#ifdef   ET_UNDO
   undo_on = U_NEXT | U_LINHA;
   next_undo( args, args->clin, args->col );
#endif

   linha_posterior( args );


   while ( 1 )
   {
      if ( fgets((SC *) line, a_ler, FA )  ==  NULL )   break;

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
      len = tam;
      if ( L1 == NULL )   len = 0;
      undo_linha( args, U_ENTRA, clin, len, L1 );
#endif
      if ( args->admlin != NULL )   (*args->admlin)( args, clin, LIN_ENTRA );

      args->clin++;
   }

fim:

   /*** Junte a ultima linha ***/
   args->col = args->len[ --args->clin ];
   line_join( args );

#ifdef   ET_UNDO
   next_undo( args, args->clin, args->col );
#endif

   /*** Junte a primeira linha ***/
   args->clin = l_cur;
   args->col  = c_cur;
   C_del( args );

   free( line );
}


/****************************************************************************/
/* Coloca o bloco caracter em UPPER ou LOWER */
VOID UL_chr_block( args, upplwr )

ARG  *args;
SS   upplwr;
/****************************************************************************/
{
   SC    *L1;
   SS    I, J, ci, cf;

   ci = pos_coluna( args, blk_li, blk_ci );
   cf = pos_coluna( args, blk_lf, blk_cf );

   J = args->len[ blk_lf ];

   if ( cf >= J )   cf = J - 1;

   if ( blk_li == blk_lf )
   {
      if ( ci < args->len[ blk_li ] )
      {
         if ((L1 = vm_deref( args->ptr[ blk_li ], 0 )) != NULL )
         {
            lin_applwr( args, L1, blk_li, ci, cf - ci + 1, upplwr );
         }
      }
   }

   else
   {
      if ((L1 = vm_deref( args->ptr[ blk_li ], 0 )) != NULL )
      {
         lin_applwr(args, L1, blk_li, ci, args->len[ blk_li ] - ci, upplwr );
      }

      for( I = blk_li + 1;   I <= blk_lf - 1;   I++ )
      {
         if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
         {
            lin_applwr( args, L1, I, 0, args->len[ I ], upplwr );
         }
      }

      if ((L1 = vm_deref( args->ptr[ blk_lf ], 0 )) != NULL )
      {
         lin_applwr( args, L1, blk_lf, 0, cf + 1, upplwr );
      }
   }
}

VOID blk_mudatrans( args )

ARG  *args;
{
   SS   I;

   if ( args->jm == NULL )   return;

   errorexists = YES;

   fwat( 0, 0, args->jm );

   fwprintf( args->jm, loc_str( STR_EDIT, EDIT_31 ), blk_scrap );

   fwcle( args->jm );

   while ((I = fwqgc( args->jm )) != K_ESC )
   {
      if ( I >= '0'  &&  I <= '9' ) {  blk_scrap = I - '0';  break;  }

      win_bell( BEL_INVALID );
   }

   fwpc( blk_scrap + '0', args->jm );

   fwrefresh( args->jm );
}


VOID lin_applwr( args, linha, num, inicio, quant, upplwr )

ARG *args;
SC  *linha;
SS  num;
SS  inicio;
SS  quant;
SS  upplwr;
{
   SS  C, I, J, T;

   T = args->len[ num ];

   if ( T <= 0 )   return;

   if ( inicio >= T )          inicio = T - 1 ;
   if ( inicio + quant > T )   quant  = T - inicio;

#ifdef   ET_UNDO
   undo_linha( args, U_ANTES, num, T, linha );
#endif

   for ( I = 0, J = inicio;   I < quant;   I++, J++ )
   {
      C = linha[ J ];

      if ( upplwr == BLK_UPPER )   linha[ J ] = toupper( C );
      else                         linha[ J ] = tolower( C );
   }
#ifdef   ET_UNDO
   undo_linha( args, U_DEPOIS, num, T, linha );
#endif
}

/*************************************************************************/
/* Insere um caracter em cada linha do bloco na posicao de blk_cf  */
VOID I_right( args, car )

ARG  *args;
SS   car;
/*************************************************************************/
{
   SC    *L1;
   SS    I, J, K, C;
   SS    lt, li = blk_li, lf = blk_lf;

   next_undo( args, args->clin, args->col );

   if ( li > lf ) {  lt = li;  li = lf;  lf = lt;  }

   for( I = li;   I <= lf;   I++ )
   {
      C = pos_coluna( args, I, blk_cf );

      if ((J = args->len[ I ]) == 0  ||  J <= C )   continue;

      linalloc( args, I, J, J + 1 );

      if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
      {
         args->f_mod = 1;

#ifdef   ET_UNDO
         undo_linha( args, U_ANTES, I, J, L1 );
#endif
         for ( K = J;   K > C;   K-- )   L1[ K ] = L1[ K - 1 ];

         L1[ C ] = car;

#ifdef   ET_UNDO
         undo_linha( args, U_DEPOIS, I, J + 1, L1 );
#endif
      }
   }
}

/*************************************************************************/
/* Retira um espaco ou Tab em cada linha do bloco na posicao de blk_cf */
VOID I_left( args )

ARG  *args;
/*************************************************************************/
{
   SC    *L1;
   SS    I, J, K, C;
   SS    lt, li = blk_li, lf = blk_lf;

   next_undo( args, args->clin, args->col );

   if ( li > lf ) {  lt = li;  li = lf;  lf = lt;  }

   for( I = li;   I <= lf;   I++ )
   {
      C = pos_coluna( args, I, blk_cf );

      if ((J = args->len[ I ]) == 0  ||  J <= C )   continue;

      if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
      {
         if ( L1[ C ] == ' '  ||  L1[ C ] == TAB_CHAR )
         {
            args->f_mod = 1;

#ifdef   ET_UNDO
            undo_linha( args, U_ANTES, I, J, L1 );
#endif
            for ( K = C;   K < J - 1;   K++ )   L1[ K ] = L1[ K + 1 ];

            linalloc( args, I, J, J - 1 );

#ifdef   ET_UNDO
            if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
            {
               undo_linha( args, U_DEPOIS, I, J - 1, L1 );
            }
#endif
         }
      }
   }
}

/***************************************************************************/
/*** Indenta um bloco na posicao do cursor ***/
VOID blk_indent( args )

ARG *args;
/***************************************************************************/
{
   SS   T, tecla;

   if ( blk_tipo == BLK_NOT )   C_col_blk( args );

   else
   {
      blk_ci = blk_cf;

      if ( blk_tipo != BLK_COL )
      {
#ifdef   ET_UNDO
         next_block( args, args->clin, args->col, BLK_COL, blk_li, blk_ci );
#endif

         blk_tipo = BLK_COL;
      }
   }

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_59 ));

   while ( 1 )
   {
      display = disp_file;
      move_scr( args );
      fwrefresh( args->je );

      if ((tecla = fwnqgc( args->je )) == K_ESC || tecla == ' ' )  goto sai;

      w_ungc( tecla );

      switch ( pegacomando( args->je ) - 256 )
      {
      default          :  continue;

      case DESMARCABLO :  goto  sai;

      case SOBE        :  C_up( args );                break;
      case DESCE       :  C_down( args );              break;
      case DIREITA     :  I_right( args, ' ' );        break;
      case PROXTAB     :  I_right( args, K_TAB );      break;
      case TABANT      :
      case ESQUERDA    :  I_left(  args );             break;
      case PROPAGINA   :
      case ANTPAGINA   :
                          next_block( args, args->clin, args->col,
                                      BLK_COL, blk_lf, blk_cf );
                          T = blk_li; blk_li = blk_lf; blk_lf = T;
                          pos_line( args, blk_lf );
                          args->col = pos_coluna( args, args->clin, blk_cf );
                          break;
      }
   }

sai:

   C_blk_not( args );
}

