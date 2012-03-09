#include  <stdio.h>
#include  <stdlib.h>

#include  "window.h"
#include  "ethead.h"

#include  MEMORY

/***************************************************************************/
/* position at the line number */
VOID pos_line( args, goto_line )

ARG * args;
SS goto_line;
/***************************************************************************/
{
   if ( goto_line < 0 )                   goto_line = 0;
   if ( goto_line > args->t_lines - 1 )   goto_line = args->t_lines - 1;

   args->clin = goto_line;

   linha_visivel( args );
}


/****************************************************************************/
/* process up arrow */
VOID C_up( args )

ARG * args;
/****************************************************************************/
{
   SS   I, clin, col;

   clin = args->clin;
   col  = args->col;

   if ( clin <= 0 )   return;

#ifdef    ET_UNDO
   next_undo( args, clin, col );
#endif

   I = cur_aparente( args, clin, col );

   linha_anterior( args );

   args->col = pos_coluna( args, args->clin, I );
}

/****************************************************************************/
/* process the down arrow */
VOID C_down( args )

ARG * args;
/****************************************************************************/
{
   SS   I, clin, col;

   clin = args->clin;
   col  = args->col;

   if ( clin + 1  >=  args->t_lines )   return;

#ifdef    ET_UNDO
   next_undo( args, clin, col );
#endif

   I = cur_aparente( args, clin, col );

   linha_posterior( args );

   args->col = pos_coluna( args, args->clin, I );
}


/****************************************************************************/
/* process the right arrow */
VOID C_right( args )

ARG * args;
/****************************************************************************/
{
   SS   clin, col;

   clin = args->clin;
   col  = args->col;

   if ( col >= LINE_LEN - 1 )   return;

#ifdef    ET_UNDO
   next_undo( args, clin, col );
   undo_on = U_DESLIGA;
#endif

   args->col++;

   if ( args->doc_mode )
   {
      if ( args->col >= args->len[ clin ] )
      {
         if ( clin < args->t_lines - 1 ) { C_down( args ); C_home( args ); }
         else                              C_end(  args );
      }
   }

#ifdef    ET_UNDO
   undo_on = U_NEXT | U_LINHA;
#endif
}

/****************************************************************************/
/* process the left arrow */
VOID C_left( args )

ARG * args;
/****************************************************************************/
{
   if ( args->col > 0 )
   {
#ifdef    ET_UNDO
      next_undo( args, args->clin, args->col );
#endif

      args->col--;
   }

   else
   {
      if ( args->doc_mode  &&  args->clin != 0 )
      {
#ifdef    ET_UNDO
         next_undo( args, args->clin, args->col );
         undo_on = U_DESLIGA;
#endif

         linha_anterior( args );
         C_end( args );

#ifdef    ET_UNDO
         undo_on = U_NEXT | U_LINHA;
#endif
      }
   }
}

/****************************************************************************/
/* process the home key */
VOID C_home( args )

ARG * args;
/****************************************************************************/
{
#ifdef    ET_UNDO
   if ( args->col != 0 )   next_undo( args, args->clin, args->col );
#endif

   args->col = 0;
}

/****************************************************************************/
/* process the end key */
VOID C_end( args )

ARG * args;
/****************************************************************************/
{
   SS   I, clin, col;

   clin = args->clin;
   col  = args->col;

   I = cur_aparente( args, clin, args->len[ clin ] );

#ifdef    ET_UNDO
   if ( I != cur_aparente( args, clin, col ) )  next_undo( args, clin, col );
#endif

   args->col = pos_coluna( args, args->clin, I );
}

/****************************************************************************/
/* process the pgup key */
VOID C_pgup( args )

ARG * args;
/****************************************************************************/
{
   SS   I, clin, col;

   clin = args->clin;
   col  = args->col;

#ifdef    ET_UNDO
   if ( clin )   next_undo( args, clin, col );
#endif

   I = cur_aparente( args, clin, col );

   clin_touch( args, clin - args->je->w_height + margin );

   args->col = pos_coluna( args, args->clin, I );
}

/****************************************************************************/
/* process the pgdn key */
VOID C_pgdn( args )

ARG * args;
/****************************************************************************/
{
   SS   I, clin, col;

   clin = args->clin;
   col  = args->col;

#ifdef    ET_UNDO
   if ( clin < args->t_lines - 1 )   next_undo( args, clin, col );
#endif

   I = cur_aparente( args, clin, col );

   clin_touch( args, clin + args->je->w_height - margin );

   args->col = pos_coluna( args, args->clin, I );
}

/****************************************************************************/
/* position at the next word */
VOID C_next_word( args )

ARG * args;
/****************************************************************************/
{
   SC  *L1;
   SS  I, found_space = NOT, clin, col;

   clin = args->clin;
   col  = args->col;

#ifdef    ET_UNDO
   if ( col  <  args->len[ clin ]  ||
        clin != args->t_lines - 1 )   next_undo( args, clin, col );
   undo_on = U_DESLIGA;
#endif

LINE_LOOP:

   for ( L1 = vm_deref( args->ptr[ clin ], 0 ), I = col;
         L1 != NULL  &&  I < args->len[ clin ];   I++ )
   {
      if ( found_space && L1[ I ] != ' ' && L1[ I ] != TAB_CHAR )
      {
         col = args->col = I;   goto disp_screen;
      }

      if ( L1[ I ] == ' ' || L1[ I ] == TAB_CHAR )   found_space = YES;
   }

   if ( clin + 1 < args->t_lines )
   {
      col  = args->col = 0;
      clin = ++args->clin;
      args->row++;
      found_space = YES;
      goto LINE_LOOP;
   }

   C_end( args );

disp_screen:
   pos_line( args, clin );

#ifdef    ET_UNDO
   undo_on = U_NEXT | U_LINHA;
#endif
}

/****************************************************************************/
/* position at the previous word */
VOID C_prev_word( args )

ARG * args;
/****************************************************************************/
{
   SC  *L1;
   SS  I, found_chars = NOT, start_col, clin, col;

   clin = args->clin;
   col  = args->col;

#ifdef    ET_UNDO
   if ( col  ||  clin )   next_undo( args, clin, col );
#endif

   if ( col >= args->len[ clin ] )   start_col = args->len[ clin ] - 2;
   else                              start_col = col - 1;

LINE_LOOP:

   for ( L1 = vm_deref( args->ptr[ clin ], 0 ), I = start_col;
         L1 != NULL  &&  I >= 0;   I-- )
   {
      if ( found_chars  &&  (L1[ I ] == ' ' || L1[ I ] == TAB_CHAR) )
      {
         args->col = I + 1;
         goto disp_screen;
      }

      if ( L1[ I ] != ' ' && L1[ I ] != TAB_CHAR &&
           ! e_et_atr( args, clin, I, NULL ) )   found_chars = YES;
   }

   if ( clin > 0  &&  ! found_chars )
   {
      clin = --args->clin;
      args->row--;
      start_col = args->len[ clin ] - 2;
      goto LINE_LOOP;
   }

   args->col = 0;

disp_screen:
   pos_line( args, clin );
}

/***************************************************************************/
VOID linha_anterior( args )  /* decrementa linha corrente */

ARG * args;
/***************************************************************************/
{
   if ( args->clin == 0 )   return;

   args->clin--;

   if ( args->row > 0 )   args->row--;
   else             display = disp_file;

   if ( args->clin < args->row )   args->row = args->clin;

   args->beg_line = args->clin - args->row;
}

/***************************************************************************/
VOID linha_posterior( args )  /* incrementa linha corrente */

ARG * args;
/***************************************************************************/
{
   if ( args->clin + 1  >=  args->t_lines )   return;

   args->clin++;

   if ( args->row < args->je->w_height - 1 )   args->row++;
   else                          display = disp_file;

   args->beg_line = args->clin - args->row;
}

/***************************************************************************/
VOID linha_visivel( args )  /* coloca linha corrente visivel (no meio da tela) */

ARG * args;
/***************************************************************************/
{
   if ( args->je != NULL  &&
       (args->clin - args->beg_line) >= args->je->w_height  ||
       (args->clin - args->beg_line) < 0 )
   {
      args->beg_line = args->clin - args->je->w_height / 2;
      if ( args->beg_line < 0 )   args->beg_line = 0;
      display = disp_file;
   }

   args->row = args->clin - args->beg_line;
}


/***************************************************************************/
VOID clin_touch( args, linha )

ARG *args;
SS  linha;
/***************************************************************************/
{
   if ( linha > args->t_lines - 1 )   linha = args->t_lines - 1;

   else if ( linha < 0 )              linha = 0;

   args->clin = linha;

   if ( args->clin < args->row )   args->row = args->clin;

   args->beg_line = args->clin - args->row;

   display = disp_file;
}

/***************************************************************************/
SS row_touch( args, linha )

ARG *args;
SS  linha;
/***************************************************************************/
{
   if ( linha < 0  ||
        linha > args->je->w_height - 1 )   return( 0 );

   args->row = linha;

   if ( args->clin < args->row )   args->row = args->clin;

   args->beg_line = args->clin - args->row;

   display = disp_file;

   return ( 1 );
}

/***************************************************************************/
/* insert paragraph markers in a word wrap file */
VOID ins_markers( args, inicio )

ARG * args;
SS inicio;
/***************************************************************************/
{
   SC   *L1;
   SS   I, J;

   for ( I = inicio;   I < args->t_lines;   I++  )
   {
      if (( J = args->len[ I ]) == 0 )
      {
         /* Inserir uma marca de paragrafo */
         linalloc( args, I, J, J + 1 );
         if ((L1 = vm_deref( args->ptr[ I ], 0 )) != NULL )
            L1[ J ] = PARA_CHR;
      }
   }
}


