
#include  <stdio.h>
#include  <stdlib.h>
#include  <ctype.h>

#include  "window.h"
#include  "mensagem.h"

#include  "ethead.h"

#include  MEMORY

/******************************************************************************/
/* delete a word */
VOID C_d_word( args )

ARG *args;
/******************************************************************************/
{
   SC   *L1;
   SS   C, A, flag, clin, len, col;

   clin = args->clin;
   len  = args->len[ clin ];
   col  = args->col;

   if ( col + 1 > len ) {  C_del( args );   return;  }

   if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   return;

#ifdef   ET_UNDO
   next_undo( args, clin, col );
   undo_linha( args, U_ANTES, clin, len, L1 );
   undo_on = U_DESLIGA;
#endif

   C = L1[ col ];

   if (      isalpha( C ) )   flag = 1;
   else if ( isdigit( C ) )   flag = 2;
   else if ( ispunct( C ) )   flag = 3;
   else if ( isspace( C ) )   flag = 4;
   else                       flag = 0;

   C_del( args );

   while ( 1 )
   {
      len = args->len[ clin ];

      if ( col + 1 > len || len == 0 || args->ptr[ clin ] == NULL )   break;

      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   goto fim;

      C = L1[ col ];

      if ( (flag && (C >= 128  ||
                    (e_et_atr( args, clin, col, &A )  &&
                                  (UC) A != ET_FORM1 ) ) ) ||
           (flag == 1  &&  isalpha( C )) ||
           (flag == 2  &&  isdigit( C )) ||
           (flag == 3  &&  ispunct( C )) ||
           (flag == 4  &&  isspace( C ))    )   C_del( args );

      else
      {
         /*** Retire o branco subsequente ***/
         if ( isspace( C ) )   C_del( args );
         break;
      }
   }

fim:

#ifdef   ET_UNDO
   undo_on = U_NEXT | U_LINHA;
   len = args->len[ clin ];
   if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   len = 0;
   undo_linha( args, U_DEPOIS, clin, len, L1 );
#endif
}


/**********************************************************************/
/* process the tab character */
VOID C_tab( args )

ARG * args;
/**********************************************************************/
{
   SC  *L1;
   SS  C, I, add_col, len, clin, col;

   clin = args->clin;
   len  = args->len[ clin ];
   col  = args->col;

   if( tab_tab && ins_mode && !args->doc_mode ) to_text( args, TAB_CHAR, 0 );

   else
   {
      C = cur_aparente( args, clin, col );

      I = ((C + tab_width) / tab_width) * tab_width;

      if ( I >= LINE_LEN )  I = LINE_LEN - 1;

      add_col = I - C;

      if ( add_col < 0 )   add_col = 0;

      L1 = vm_deref( args->ptr[ clin ], 0 );

#ifdef   ET_UNDO
      next_undo( args, clin, col );
#endif

      if ((L1 != NULL && (UC) L1[ col ] == PARA_CHR ) ||
         (ins_mode  &&  col < len ) )
      {
#ifdef   ET_UNDO
         undo_linha( args, U_ANTES, clin, len, L1 );
         undo_on = U_DESLIGA;
#endif

         for ( I = 0;   I < add_col;   I++ )   to_text( args, ' ', 1 );

#ifdef   ET_UNDO
         undo_on = U_NEXT | U_LINHA;
         if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )  len = 0;
         else                                   len = args->len[ clin ];
         undo_linha( args, U_DEPOIS, clin, len, L1 );
#endif
      }

      else  args->col = pos_coluna( args, args->clin, C + add_col );
   }
}

/****************************************************************************/
/* insere uma linha apos a linha corrente */
VOID insert_line( args )

ARG * args;
/****************************************************************************/
{
   args->f_mod = 1;

   if ( args->t_lines >= args->max_lines   &&
        ajusta_array( args, INC_LIN + args->t_lines ) == 0 )   return;

   ins_linha( args, args->clin + 1 );

   display = disp_file;
}

/****************************************************************************/
/* insere uma linha apos a linha corrente com UNDO */
VOID ins_l_after( args )

ARG * args;
/****************************************************************************/
{
   if ( args->doc_mode )   return;

   C_end( args );

#ifdef   ET_UNDO
   undo_on = U_LINHA;
#endif

   split_line( args, 1, 1 );

#ifdef   ET_UNDO
   undo_on = U_NEXT | U_LINHA;
#endif
}

/****************************************************************************/
/* insert a line before the current line */
VOID ins_l_bef( args )

ARG * args;
/****************************************************************************/
{
   SS   clin;

   if ( args->doc_mode )   return;

   if ((clin = args->clin) > 0 )
   {
      SS row = args->row;

#ifdef    ET_UNDO
      next_undo( args, clin, args->col );
      undo_on = U_LINHA;
#endif

      args->col = args->len[ --args->clin ];

      split_line( args, 1, 1 );

#ifdef   ET_UNDO
      undo_on = U_NEXT | U_LINHA;
#endif

      args->row = row;

      clin_touch( args, args->clin );
   }

   else if ( clin == 0 )
   {
      if ( args->t_lines >= args->max_lines   &&
           ajusta_array( args, INC_LIN + args->t_lines ) == 0 )   return;

      args->f_mod = 1;

#ifdef    ET_UNDO
      next_undo( args, clin, args->col );
      undo_linha( args, U_ENTRA, clin, 0, str_vazia );
#endif
      if ( args->admlin != NULL )  (*args->admlin)( args, clin, LIN_ENTRA );

      ins_linha( args, clin );

      args->col = 0;

      linha_visivel( args );

      display = disp_file;
   }
}


/****************************************************************************/
/* delete a line */
VOID delete_line( args )

ARG * args;
/****************************************************************************/
{
   SC   *L1;
   SS   I, len, tam, clin, col;

   display = disp_file;
   args->f_mod = YES;

   clin = args->clin;
   len  = tam = args->len[ clin ];
   col  = args->col;

   I = cur_aparente( args, clin, col );

#ifdef    ET_UNDO
   if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   tam = 0;
#endif

   if ( args->t_lines <= 1 )
   {
#ifdef    ET_UNDO
      if ( len || col )
      {
         next_undo(  args, clin, col );
         undo_linha( args, U_ANTES,  clin, tam, L1 );
         undo_linha( args, U_DEPOIS, clin, 0, L1 );
      }
#endif

      linalloc( args, clin, len, 0 );
      args->col = 0;
      return;
   }

#ifdef    ET_UNDO
   next_undo(  args, clin, col );
   undo_linha( args, U_SAI, clin, tam, L1 );
#endif
   if ( args->admlin != NULL )  (*args->admlin)( args, clin, LIN_SAI );

   ret_linha( args, clin );

   if ( clin == args->t_lines ) {  args->col = 0;  args->t_lines++;  }

   else    args->col = pos_coluna( args, args->clin, I );
}

/*=========================================================================*/
/* Join next line at the end of the current line */
VOID line_join( args )

ARG * args;
/*=========================================================================*/
{
   SC  *L1, *L2;
   SS   i, len1, len2, clin, tam;

   if ( args->doc_mode )   return;

   clin = args->clin;
   len1 = args->len[ clin ];
   len2 = args->len[ clin + 1 ];

   if ( args->t_lines == (clin + 1) )   return;

   if ( len1 + len2  >  LINE_LEN )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_502 ));
      return;
   }

   args->f_mod = 1;

#ifdef   ET_UNDO
   next_undo( args, clin, args->col );
#endif

   if ( vm_deref( args->ptr[ clin + 1 ], 0 ) != NULL )
   {
      /* A proxima linha existe ! */
      /* reserve more space in the current line */

#ifdef   ET_UNDO
      tam = len1;
      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   tam = 0;
      undo_linha( args, U_ANTES, clin, tam, L1 );
#endif

      tam = len1 + len2;

      linalloc( args, clin, len1, tam );

      if ((L1 = vm_deref( args->ptr[ clin ],     1 )) == NULL )  return;
      if ((L2 = vm_deref( args->ptr[ clin + 1 ], 0 )) != NULL )
      {
         for ( i = 0;   i < len2;   i++ )   L1[ i + len1 ] = L2[ i ];
      }

#ifdef   ET_UNDO
      undo_linha( args, U_DEPOIS, clin, tam, L1 );
#endif

      vm_unlock( args->ptr[ clin ] );
   }

#ifdef   ET_UNDO
   if ((L2 = vm_deref( args->ptr[ clin + 1 ], 0 )) == NULL )   len2 = 0;
   undo_linha( args, U_SAI, clin + 1, len2, L2 );
#endif
   if ( args->admlin != NULL )  (*args->admlin)( args, clin + 1, LIN_SAI );

   if ( clin + 1  <  args->t_lines )   ret_linha( args, clin + 1 );

   display  = disp_file;
}


/***************************************************************************/
VOID C_dup_line( args )                /* duplicate the previous line */

ARG * args;
/***************************************************************************/
{
   SC  *L1, *L2;
   SS  clin, len;

   clin = args->clin;
   len  = args->len[ clin ];

   display = disp_file;

   insert_line( args );

   linalloc( args, clin + 1, args->len[ clin + 1 ], len );

   if ((L1 = vm_deref( args->ptr[ clin + 1 ], 1 )) == NULL )  return;
   if ((L2 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
   {
      memmove( (UC *) L1, (UC *) L2, len );
   }

#ifdef    ET_UNDO
   next_undo( args, clin, args->col );
   undo_linha( args, U_ENTRA, clin, len, L1 );
#endif
   if ( args->admlin != NULL )  (*args->admlin)( args, clin, LIN_ENTRA );

   vm_unlock( args->ptr[ clin + 1 ] );

   linha_posterior( args );
}

/***************************************************************************/
VOID del_end_line( args ) /* delete from cursor position to the end of the line */

ARG * args;
/***************************************************************************/
{
   SS  tam, flag = 0, clin, len;
   SC  *L1;
   SS   mar_esq, mar_pri, mar_dir;

   clin = args->clin;
   len  = args->len[ clin ];

   if ( (tam = args->col)  >=  len )   return;

   if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   return;

#ifdef   ET_UNDO
   next_undo( args, clin, tam );
   undo_linha( args, U_ANTES, clin, len, L1 );
#endif

   if ( args->doc_mode )
   {
      pega_margens( args, clin, &mar_esq, &mar_pri, &mar_dir );

      if ( (UC) L1[ len - 1 ] == PARA_CHR ) {  tam++;  flag++;  }
   }

   linalloc( args, clin, len, tam );

   if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   return;

   /*** Em DocMode Nao destrua a marca de final de paragrafo ***/
   if ( args->doc_mode )
   {
      if ( flag )    L1[ tam - 1 ] = PARA_CHR;

      set_margens( args, clin, mar_esq, mar_pri, mar_dir );
   }

#ifdef   ET_UNDO
   if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   tam = 0;
   undo_linha( args, U_DEPOIS, clin, tam, L1 );
#endif

   if ( display != disp_file )   display = disp_line;
}


SS edit_arquivo( arquivo, JE, doc, mens, salvjmp )

SC  *arquivo;
WIN *JE;
SS  doc;
SC  *mens;
SS  salvjmp;
{
   SS  ret = -1;
   ARG *args;

   if ( (args = load_buffer( doc, ET_FILE, arquivo, 0L )) != NULL )
   {
      fw_title( JE, arquivo, JE->t_atr );

      args->def_mens = mens;

#ifdef   MEDIT

      while( 1 )
      {
         SC    escape[ 6 ];

         sprintf( escape, "%c", K_ESC );
         args->escape = escape;

         if ( salvjmp )   s_edit_buffer( args, ET_EDITA, JE, NULL, 0 );
         else               edit_buffer( args, ET_EDITA, JE, NULL, 0 );

         switch( ret = args->tecla )
         {
         case SALVARQ:
            save_file( args );
            continue;

         case TERMINA:
         case SAISALVA:
            save_file( args );
         case ABANDONA:
         case K_ESC:
            C_zoom( args, 1 );
            goto sai;

         case ZOOM:
            C_zoom( args, 0 );
            continue;

         default:
            win_bell( BEL_INVALID );
            continue;
         }
      }

sai:

#else

      if ( salvjmp )   ret = s_edit_buffer( args, ET_EDITA, JE, NULL, 0 );
      else             ret =   edit_buffer( args, ET_EDITA, JE, NULL, 0 );

#endif

      free_buffer( args );
   }

   return ( ret );
}


SS s_edit_buffer( args, forma, JE, JM, linha )

ARG *args;
SS   forma;
WIN *JE;
WIN *JM;
SS   linha;
{
   SS      ret;
/*   jmp_buf s_end_rtn; */

/*   memmove((UC *) &s_end_rtn, (UC *) &end_rtn, sizeof (jmp_buf) ); */

   ret = edit_buffer( args, forma, JE, JM, linha );

/*   memmove((UC *) &end_rtn, (UC *) &s_end_rtn, sizeof (jmp_buf) ); */

   return ( ret );
}


/*=========================================================================*/
/* split the current line*/
VOID split_line( args, pos_col, alinha )

ARG *args;
SS pos_col;
SS alinha;
/*=========================================================================*/
{
   SC   *L1, *L2, *L3;
   SS   I, J, linha = -1, new_col = 0, tam, col, ntl, len, clin, tlc;

   /*******************************************************
    *                                                     *
    * Esta linha foi comentada, pois na inclusao de bloco *
    * caracter no modo documento nao estava fazendo       *
    * o split.                                            *
    *                                                     *
    *******************************************************/

/****   if ( args->doc_mode )   return; ****/

   if ( args->t_lines >= args->max_lines   &&
        ajusta_array( args, INC_LIN + args->t_lines ) == 0 )   return;

   insert_line( args );

   ntl = col = args->col;
   clin = args->clin;
   len = args->len[ clin ];

#ifdef    ET_UNDO
   next_undo( args, clin, col );
#endif

   if ( alinha )
   {
      for ( I = clin, linha = -1;   I >= 0;   J = 0, I--, linha = -1 )
      {
         /*** Se a linha e' ZERO nao serve ***/
         if ((tlc = args->len[ I ]) == 0 )   continue;

         linha = I;

         /*** A linha nao e' ZERO se for NULL esta' ERRADO ***/
         if ((L3 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   goto sai;

         for( J = 0; J < tlc && (L3[J] == ' ' || L3[J] == TAB_CHAR); J++ )
            ;

         if ( J < tlc  &&  (I != clin || col >= J ))   break;
      }

      new_col = J;

      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
      {
         for(   ;   col < len;   col++ )
         {
            if ( L1[ col ] != ' '  &&  L1[ col ] != TAB_CHAR )   break;
         }
      }
   }

   if ( col > len )   col = len;
   if ( ntl > len )   ntl = len;

   if ( new_col + len - col > LINE_LEN )
   {
      new_col = LINE_LEN - len + col;
      if ( new_col < 0 )   new_col = 0;
   }

   tam = new_col + len - col;

   linalloc( args, clin + 1, args->len[ clin + 1 ], tam );

   if ((L1 = vm_deref( args->ptr[ clin + 1 ], 1 )) == NULL )  goto sai;

   for( I = 0;   I < tam;   I++ )   L1[ I ] = ' ';

   if ( linha != -1 && (L3 = vm_deref(args->ptr[ linha ], 0)) != NULL )
   {
      for( I = 0;   I < new_col;   I++ )   L1[ I ] = L3[ I ];
   }

   if ((L2 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
   {
      for( I = col;  I < len;  I++ )  L1[ new_col + I - col ] = L2[ I ];

      if ( len != ntl )
      {
#ifdef    ET_UNDO
         undo_linha( args, U_ANTES,  clin, len, L2 );
         undo_linha( args, U_DEPOIS, clin, ntl, L2 );
#endif
         linalloc( args, clin, len, ntl );
      }
   }

   vm_unlock( args->ptr[ clin + 1 ] );

sai:

#ifdef    ET_UNDO
   if ((L1 = vm_deref( args->ptr[ clin + 1 ], 0 )) == NULL )   tam = 0;
   undo_linha( args, U_ENTRA,  clin + 1, tam, L1 );
#endif
   if ( args->admlin != NULL )  (*args->admlin)( args, clin + 1, LIN_ENTRA );

   if ( pos_col )
   {
      I = 0;

      if ( linha != -1 )   I = cur_aparente( args, linha, new_col );

      linha_posterior( args );

      args->col = pos_coluna( args, args->clin, I );
   }
}

/****************************************************************************/
/***
Insere Caracteres Control
***/
VOID insere_ctrl( args )

ARG *args;
/****************************************************************************/
{
   SS   C;

   if ( args->doc_mode )   return;

   errorexists = YES;

   fwat( 0, 0, args->jm );

   fwprintf( args->jm, loc_str( STR_EDIT, EDIT_66) );

   fwcle( args->jm );

   while ((C = fwqgc( args->jm )) != K_ESC )
   {
      if ( C >= 'a'  &&  C <= 'z' )   C -= 32;

      if ( C >= 'A'  &&  C <= '_' )
      {
         fwpc( C, args->jm );
         fwrefresh( args->jm );
         to_text( args, C - CTRL_VIS,  1 );
         break;
      }

      win_bell( BEL_INVALID );
   }
}

/****************************************************************************/
/***
Insere Caracteres Atraves de Expressao
***/
VOID exprechar( args )

ARG *args;
/****************************************************************************/
{
   DB   D;
   SC   exp[ 80 ];

   exp[ 0 ] = '\0';

   if ( pede_cadeia( loc_str( STR_EDIT, EDIT_69 ), exp, 60,
                     loc_str( STR_EDIT, EDIT_70 ),
                                      'x', NULL ) == K_ESC ) return;

   if ( calc_avalia( exp, &D, args->jm, NULL, '?' ) >= 0 )
   {
      to_text( args, (SS) D,  1 );
   }
}

