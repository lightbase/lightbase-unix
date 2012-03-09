#include  <stdio.h>
#include  <stdlib.h>
#include  <ctype.h>

#include  "window.h"
#include  "mensagem.h"

#include  "ethead.h"

#include  MEMORY

/**********************************************************************/
/* process carriage return */
VOID C_cr( args )

ARG *args;
/**********************************************************************/
{
   if ( ins_mode || cr_new_line ) {  split_line( args, 1, tab_align ); }

   else                           {  args->col = 0;   C_down( args );  }
}

/**********************************************************************/
/* process backspace characger */
VOID C_bk_spc( args )

ARG *args;
/**********************************************************************/
{
   if ( args->col == 0 )
   {
      /*** Igual e' suficiente pois o loop principal pega o resto ***/
      if ( args->forma == ET_APPEND  &&  args->clin == args->alin )   return;

      if ( args->clin == 0 )   return;

      if ( ! args->doc_mode  &&  ! join_lines )   return;

      linha_anterior( args );

      args->col = args->len[ args->clin ];

      if ( args->doc_mode  &&  args->col )   args->col--;

      /*** Se for PARA_CHR o del() retira tambem o ET_FORM ***/
      C_del( args );
   }

   else
   {
#ifdef   ET_UNDO
      next_undo( args, args->clin, args->col );
      undo_on = U_LINHA;
#endif

      args->col--;

      if ( args->col < args->len[ args->clin ] )   C_del( args );

#ifdef   ET_UNDO
      undo_on = U_NEXT | U_LINHA;
#endif
   }
}

/******************************************************************************/
/* delete a character*/
VOID C_del( args )

ARG * args;
/******************************************************************************/
{
   SC  *L1;
   SS  i, len, clin, col, tam, C;

   clin = args->clin;
   col  = args->col;
   len  = tam = args->len[ clin ];

   if ( col + 1 > len )
   {
      if ( col == 0 )   delete_line( args );

      else
      {
         if ( col > len )
         {
#ifdef   ET_UNDO
            next_undo( args, clin, col );

            if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   tam = 0;
            undo_linha( args, U_ANTES, clin, tam, L1 );
#endif

            linalloc( args, clin, len, col );

            if ((L1 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
            {
               /* move spaces in unused area */
               for ( i = len;   i < col;   i++ )   L1[ i ] = ' ';
            }

#ifdef   ET_UNDO
            if ( L1 == NULL )   tam = 0;
            else                tam = args->len[ clin ];
            undo_linha( args, U_DEPOIS, clin, tam, L1 );
#endif
         }

         if ( ! args->doc_mode  &&  join_lines )   line_join( args );
      }
   }

   else   /*** ( col + 1 <= len ) ***/
   {
      if ( args->doc_mode )
      {
         /*** Se for atributo refazer toda a tela ***/
         if ( e_et_atr( args, clin, col, &C ))   display = disp_file;

         /*** Se estiver em cima de um ET_FORM ou de PARA_CHR ... ***/
         /*** ... retire ambos da linha. ***/
         if ((UC)C == ET_FORM1 || (UC) C == ET_FORM2 || (UC)C == PARA_CHR )
         {
            retira_form( args, clin );
            return;
         }
      }

      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )  return;

#ifdef   ET_UNDO
      next_undo( args, clin, col );

      if ( L1 == NULL )   tam = 0;
      undo_linha( args, U_ANTES, clin, tam, L1 );
#endif

      for ( i = col;   i < len - 1;   i++ )   L1[ i ] = L1[ i + 1 ];

      linalloc( args, clin, len, len - 1 );

#ifdef   ET_UNDO
      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )  tam = 0;
      else                                  tam = args->len[ clin ];
      undo_linha( args, U_DEPOIS, clin, tam, L1 );
#endif

      if ( display != disp_file )  display = disp_line;
   }
}

#ifdef   VM_ARRAYS

/***
Precisamos de "ptr", "len", e "max_lines" adequados entre si.
No inicio "ptr = len = NULL" e max_lines = 0.
Funciona com MALLOC()
***/

US   ajusta_array( args, novo_max )

ARG * args;
US   novo_max;
{
   PTR  *r_ptr;
   SS   *r_len, m_lines, I;
   VOH  r_VMptr, r_VMlen;

   if ( novo_max >= MAX_LINE )    return( 0 );

   if ((r_VMptr = vm_alloc((novo_max + 1) * sizeof( PTR ))) == NULL )
   {
erro_1:
      debug( loc_str( STR_EDIT, EDIT_503 ));
      return ( 0 );
   }

   if ((r_ptr = (PTR *) vm_deref( r_VMptr, 1 )) == NULL )
   {
      vm_free( r_VMptr );
      goto  erro_1;
   }

   if ((r_VMlen = vm_alloc((novo_max + 1) * sizeof( SS ) )) == NULL )
   {
erro_2:
      vm_free( r_VMptr );
      debug( loc_str( STR_EDIT, EDIT_504 ) );
      return ( 0 );
   }

   if ((r_len = (SS *) vm_deref( r_VMlen, 1 )) == NULL )
   {
      vm_free( r_VMlen );
      goto erro_2;
   }

   m_lines = MIN( args->max_lines, novo_max );

   if ( args->ptr != NULL )
   {
      for ( I = 0;   I < m_lines;   I++ )  r_ptr[ I ] = args->ptr[ I ];
      vm_free( args->VMptr );
   }
   args->ptr   = r_ptr;
   args->VMptr = r_VMptr;

   if ( args->len != NULL )
   {
      for ( I = 0;   I < m_lines;   I++ )  r_len[ I ] = args->len[ I ];
      vm_free( args->VMlen );
   }
   args->len   = r_len;
   args->VMlen = r_VMlen;

   for ( I = args->max_lines;   I < novo_max;   I++ )
   {
      args->ptr[ I ] = NULL;
      args->len[ I ] = 0;
   }

   return ( args->max_lines = novo_max );
}

#else

US   ajusta_array( args, novo_max )

ARG * args;
US   novo_max;
{
   PTR  *r_ptr;
   SS   *r_len, m_lines, I;

   if ( novo_max >= MAX_LINE )    return( 0 );

   r_ptr = (PTR *) malloc( (novo_max + 1) * sizeof( PTR ) );
   if ( r_ptr == NULL )
   {
      debug( loc_str( STR_EDIT, EDIT_503 ));
      return ( 0 );
   }

   r_len =  (SS *)  malloc( (novo_max + 1) * sizeof( SS ) );
   if ( r_len == NULL )
   {
      free( (SC *) r_ptr );
      debug( loc_str( STR_EDIT, EDIT_504 ) );
      return ( 0 );
   }

   m_lines = MIN( args->max_lines, novo_max );

   if ( args->ptr != NULL )
   {
      for ( I = 0;   I < m_lines;   I++ )  r_ptr[ I ] = args->ptr[ I ];
      free( (SC *) args->ptr );
   }
   args->ptr = r_ptr;

   if ( args->len != NULL )
   {
      for ( I = 0;   I < m_lines;   I++ )  r_len[ I ] = args->len[ I ];
      free( (SC *) args->len );
   }
   args->len = r_len;

   for ( I = args->max_lines;   I < novo_max;   I++ )
   {
      args->ptr[ I ] = NULL;
      args->len[ I ] = 0;
   }

   return ( args->max_lines = novo_max );
}

#endif


