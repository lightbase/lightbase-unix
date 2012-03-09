
#include  <stdio.h>
#include  <stdlib.h>

#include  "window.h"
#include  "ethead.h"

#include  MEMORY

/***************************************************************************/
/* ensure allocation */
VOID linalloc( args, line, old_size, new_size )

ARG * args;
US line, old_size, new_size;
/***************************************************************************/
{
   args->f_mod = YES;

   if ( old_size == 0  &&  new_size == 0 )
   {
      args->ptr[ line ] = NULL;
      args->len[ line ] = 0;
   }

   else if ( old_size == new_size )
   {
      old_size = new_size;  /*** Apenas para nao corromper os "else if" ***/
   }

   else if ( new_size == 0 )
   {
      vm_free( args->ptr[ line ] );
      args->ptr[ line ] = NULL;
      args->len[ line ] = 0;
   }

   else if ( old_size != 0 )
   {
      if ((args->ptr[ line ] = (PTR) vm_realloc( args->ptr[ line ],
                                                new_size )) == NULL )
      {
         end_logic( args, -1 );
      }
      args->len[ line ] = new_size;
   }

   else if ( old_size == 0 )
   {
      if ((args->ptr[ line ] = (PTR) vm_alloc( new_size )) == NULL )
      {
         end_logic( args, -2 );
      }
      args->len[ line ] = new_size;
   }
}


VOID ins_linha( args, linha )

ARG * args;
US   linha;
{
   args->t_lines++;

   memmove((UC *) &args->ptr[ linha + 1 ],
           (UC *) &args->ptr[ linha ],
           (args->t_lines - linha) * (sizeof (PTR) ) );

   memmove((UC *) &args->len[ linha + 1 ],
           (UC *) &args->len[ linha ],
           (args->t_lines - linha) * (sizeof (SS) ) );

   args->ptr[ linha ] = NULL;
   args->len[ linha ] = 0;
}


VOID ret_linha( args, linha )

ARG * args;
US   linha;
{
   args->t_lines--;

   linalloc( args, linha, args->len[ linha ], 0 );

   memmove((UC *) &args->ptr[ linha ],
           (UC *) &args->ptr[ linha + 1 ],
           (args->t_lines - linha) * (sizeof (PTR)) );

   memmove((UC *) &args->len[ linha ],
           (UC *) &args->len[ linha + 1 ],
           (args->t_lines - linha) * (sizeof (SS)) );

   args->ptr[ args->t_lines ] = NULL;
   args->len[ args->t_lines ] = 0;
}


