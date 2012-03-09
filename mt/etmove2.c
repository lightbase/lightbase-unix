
#include  <stdio.h>
#include  <stdlib.h>

#include  "window.h"
#include  "ethead.h"
#include  "mensagem.h"

VOID imprime_arq( args )

ARG *args;
{
   args = args;
}

/***************************************************************************/
/* Procura o caracter correspondente a '{', '(', '[', '}', ')', ']' */
VOID et_matching( args )

ARG * args;
/***************************************************************************/
{
   SC   *L1;
   SS   col, clin, len;

   col  = args->col;
   clin = args->clin;
   len  = args->len[ clin ];

   if ( len == 0    ||
        col >= len  ||
        (L1  = vm_deref( args->ptr[ clin ], 0 ))  == NULL )
   {
      beep( args );
      return;
   }

   switch ( L1[ col ] )
   {
   default  :  beep( args );                 break;

   case '{' :  m_frente( args, '{', '}' );   break;
   case '(' :  m_frente( args, '(', ')' );   break;
   case '[' :  m_frente( args, '[', ']' );   break;

   case '}' :  m_traz(   args, '}', '{' );   break;
   case ')' :  m_traz(   args, ')', '(' );   break;
   case ']' :  m_traz(   args, ']', '[' );   break;
   }
}

VOID  m_traz( args, ori, pro )

ARG   *args;
SS    ori, pro;
{
   SS   I, J, K = 1, clin, col;

   I = clin = args->clin;
   J = col  = args->col;

   while ( ant_car( args ) )
   {
      if (      args->tmp_ss == ori )   K++;

      else if ( args->tmp_ss == pro )   K--;

      if ( ! K ) {  I = args->clin;  J = args->col;  break;  }
   }

   if ( I != args->clin  ||  J != args->col )
   {
      errorexists = YES;
      mens_2( args->jm, loc_str( STR_EDIT, EDIT_524 ) );
   }
#ifdef    ET_UNDO
   else   next_undo( args, clin, col );
#endif

   args->clin = I;
   args->col  = J;

   linha_visivel( args );
}

/***
Posiciona no caracter anterior do arquivo.
Volta 1 se conseguir e 0 se ja estiver na ultima posicao.
Deixa em ARG.TMP_SS o caracter da posicao.
***/

SS   ant_car( args )

ARG  *args;
{
   SC   *L1;

   if ( args->col == 0  ||  args->len[ args->clin ] == 0 )
   {
      while ( 1 )
      {
         if ( args->clin == 0 )   return( 0 );

         args->clin--;
         args->col = args->len[ args->clin ];
         if ( args->col > 0 )   args->col--;

         if ( args->len[ args->clin ] )   break;
      }
   }

   else if ( args->col >= args->len[ args->clin ] )
   {
      args->col = args->len[ args->clin ] - 1;
   }

   else   args->col--;

   if ((L1 = vm_deref( args->ptr[ args->clin ], 0 )) == NULL )  return( 0 );

   args->tmp_ss = L1[ args->col ];

   return( 1 );
}

VOID  m_frente( args, ori, pro )

ARG   *args;
SS    ori, pro;
{
   SS   I, J, K = 1, clin, col;

   I = clin = args->clin;
   J = col  = args->col;

   while ( prox_car( args ) )
   {
      if (      args->tmp_ss == ori )   K++;

      else if ( args->tmp_ss == pro )   K--;

      if ( ! K ) {  I = args->clin;  J = args->col;  break;  }
   }

   if ( I != args->clin  ||  J != args->col )
   {
      errorexists = YES;
      mens_2( args->jm, loc_str( STR_EDIT, EDIT_524 ) );
   }
#ifdef    ET_UNDO
   else   next_undo( args, clin, col );
#endif

   args->clin = I;
   args->col  = J;

   linha_visivel( args );
}


/***
Posiciona no proximo caracter do arquivo.
Volta 1 se conseguir e 0 se ja estiver na ultima posicao.
Deixa em ARG.TMP_SS o caracter da posicao.
***/

SS   prox_car( args )

ARG  *args;
{
   SC   *L1;

   if ( args->col + 1 < args->len[ args->clin ] )   args->col++;

   else  /*** Proxima Linha Valida ***/
   {
      while ( 1 )
      {
         if ( args->clin >= (args->t_lines - 1) )   return( 0 );

         args->col = 0;
         args->clin++;

         if ( args->len[ args->clin ] )   break;
      }
   }

   if ((L1 = vm_deref( args->ptr[ args->clin ], 0 )) == NULL )  return( 0 );

   args->tmp_ss = L1[ args->col ];

   return( 1 );
}

VOID initela( args )

ARG *args;
{
   SS   I, beg, clin, col;

   clin = args->clin;
   col  = args->col;
   beg  = args->beg_line;

#ifdef    ET_UNDO
   if ( beg != clin )   next_undo( args, clin, col );
#endif

   I = cur_aparente( args, clin, col );

   pos_line( args, beg );

   args->col = pos_coluna( args, args->clin, I );
}


VOID fimtela( args )

ARG *args;
{
   SS   I, end, clin, col;

   clin = args->clin;
   col  = args->col;
   end  = args->beg_line + args->je->w_height - 1;

#ifdef    ET_UNDO
   if ( end != clin )   next_undo( args, clin, col );
#endif

   I = cur_aparente( args, clin, col );

   pos_line( args, end );

   args->col = pos_coluna( args, args->clin, I );
}

VOID leftscroll( args )

ARG *args;
{
   SS   col, clin, I, J;

   col  = args->col;
   clin = args->clin;
   
   if ( args->doc_mode )   return;

#ifdef    ET_UNDO
   next_undo( args, clin, col );
#endif

   J = args->je->w_len / 4;

   I = cur_aparente( args, clin, col ) + J;

   args->beg_col += J;
   args->end_col += J;

   args->col = pos_coluna( args, clin, I );

   display = disp_file;
}

VOID rightscroll( args )

ARG *args;
{
   SS   clin, col, I, J;

   col  = args->col;
   clin = args->clin;
   
   if ( args->doc_mode )   return;

#ifdef    ET_UNDO
   if ( args->beg_col )   next_undo( args, clin, col );
#endif

   J = args->je->w_len / 4;

   I = cur_aparente( args, clin, col );

   if ( args->beg_col < J )   J = args->beg_col;

               I -= J;
   args->beg_col -= J;
   args->end_col -= J;

   args->col = pos_coluna( args, clin, I );

   display = disp_file;
}

#define MAR_LEN  2

VOID C_margens( args )

ARG  *args;
{
   SS   tecla, estado;
   SS   mar_esq, mar_pri, mar_dir;
   SC   str_esq[ MAR_LEN + 10 ];
   SC   str_pri[ MAR_LEN + 10 ];
   SC   str_dir[ MAR_LEN + 10 ];
   SC   *S1;

   if ( ! args->doc_mode )   return;

   pega_margens( args, args->clin, &mar_esq, &mar_pri, &mar_dir );
   
   sprintf( str_esq, "%d", mar_esq );
   sprintf( str_pri, "%d", mar_pri );
   sprintf( str_dir, "%d", mar_dir );
   
   /*** Evita muitos refresh na janela ***/
   S1 = loc_str( STR_EDIT, EDIT_62 );
   errorexists = 1;
   mens_1( args->jm, S1 );

   for( estado = 1;   estado < 100;    )
   {
      switch( estado )
      {
      default  :
      case    1:  /*** Pede Margem Esquerda do Paragrafo ***/
                  tecla = pede_cadeia( loc_str( STR_EDIT, EDIT_63 ),
                                       str_esq, MAR_LEN,
                                       S1,
                                       '9', NULL );

                  if (      tecla == K_ESC   )   estado = 100;
                  else if ( tecla == K_ENTER )   estado = 2;
                  else if ( tecla == K_TAB   )   estado = 2;
                  else if ( tecla == K_DN    )   estado = 2;
                  break;

      case    2:  /*** Pede Margem Esquerda da Primeira Linha ***/
                  tecla = pede_cadeia( loc_str( STR_EDIT, EDIT_64 ),
                                       str_pri, MAR_LEN,
                                       S1,
                                       '9', NULL );

                  if (      tecla == K_ESC   )   estado = 100;
                  else if ( tecla == K_ENTER )   estado = 3;
                  else if ( tecla == K_TAB   )   estado = 3;
                  else if ( tecla == K_DN    )   estado = 3;
                  else if ( tecla == K_UP    )   estado = 1;
                  break;

      case    3:  /*** Pede Margem Direita ***/
                  tecla = pede_cadeia( loc_str( STR_EDIT, EDIT_65 ),
                                       str_dir, MAR_LEN,
                                       S1,
                                       '9', NULL );

                  if (      tecla == K_ESC   )   estado = 100;
                  else if ( tecla == K_ENTER )   estado = 200;
                  else if ( tecla == K_UP    )   estado = 2;
                  break;
      }
   }

   if ( estado == 200 )
   {
      mar_esq = atoi( str_esq );
      mar_pri = atoi( str_pri );
      mar_dir = atoi( str_dir );
   
      set_margens( args, args->clin, mar_esq, mar_pri, mar_dir );
      
      total_wrap( args, 0 );
      
      display = disp_file;
   }
}

/*** Pega as margens de um paragrafo ***/
VOID  set_margens( args, linha, mar_esq, mar_pri, mar_dir )

ARG   *args;
SS    linha;
SS    mar_esq;
SS    mar_pri;
SS    mar_dir;
{
   SC  *L1;
   SS  lin, esq, pri, dir;
   register   SS   I;
   register   UC   *P;
   
   if ((lin = pega_margens( args, linha, &esq, &pri, &dir )) < 0 )
   {
      /*** Paragrafo nao possui formato, procure o final ***/
      while( 1 )
      {
         if ( linha >= args->t_lines      ||
             (I  = args->len[ linha ]) == 0   ||
             (L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return;
            
         P = (UC *) &L1[ I - 1 ];

         /*** Ja chegou ao Fim do Paragrafo ***/
         if ( *P == PARA_CHR )   break;
         
         linha++;
      }
      
      lin = linha;
      
      linalloc( args, lin, I, I + TMF );
      
      if ((L1 = vm_deref( args->ptr[ lin ], 0 )) != NULL )
      {
         P = (UC *) &L1[ I + TMF - 1 ];
         
         /*** A constante 'A' e' para fugir de carateres especiais e ... ***/
         /*** ... principalmente do BRANCO ***/
         *P-- = PARA_CHR;
         *P-- = ET_FORM2;
         *P-- = mar_dir + 'A';
         *P-- = mar_pri + 'A';
         *P-- = mar_esq + 'A';
         *P   = ET_FORM1;
      }
   }
   
   /*** Ja possui formato apenas recoloque novos formatos ***/
   else if ((I = args->len[ lin ]) > TMF  &&
           (L1 = vm_deref( args->ptr[ lin ], 0 )) != NULL )
   {
      /*** Ache onde esta o formato ***/
      for ( P = (UC *) &L1[ --I ];   I > 0  &&  *P != ET_FORM2;   I--, P-- )
         ;
      
      if ( I >= TMF - 1  &&  *P-- == ET_FORM2 )
      {
         /*** A constante 'A' e' para fugir de carateres especiais e ... ***/
         /*** ... principalmente do BRANCO ***/
         *P-- = mar_dir + 'A';
         *P-- = mar_pri + 'A';
         *P   = mar_esq + 'A';
      }
   }
}

/***************************************************************************/
/* Informa em decimal e Hexa qual o caracter Corrente */
VOID qual_char( args )

ARG * args;
/***************************************************************************/
{
   SS   C;
   
   C = char_posicao( args, args->clin, args->col );
   
   if ( C != -1 )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_71), C, C );
   }
}


