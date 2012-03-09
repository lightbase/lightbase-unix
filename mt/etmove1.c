
#include  <stdio.h>
#include  <stdlib.h>

#include  "window.h"
#include  "mensagem.h"
#include  "ethead.h"

#include  MEMORY

/****************************************************************************/
/* CTRL Z */
VOID C_up_scroll( args )

ARG * args;
/****************************************************************************/
{
   SS   I, clin, col;

   clin = args->clin;
   col  = args->col;

   I = cur_aparente( args, clin, col );

   if ( ! row_touch( args, args->row - 1 ))
   {
#ifdef    ET_UNDO
      next_undo( args, clin, col );
#endif
      clin_touch(args, clin + 1);
   }

   args->col = pos_coluna( args, args->clin, I );
}

/****************************************************************************/
/* CTRL W */
VOID C_down_scroll( args )

ARG * args;
/****************************************************************************/
{
   SS   I, clin, col;

   clin = args->clin;
   col  = args->col;

   I = cur_aparente( args, clin, col );

   if ( ! row_touch( args, args->row + 1 ))
   {
#ifdef    ET_UNDO
      next_undo( args, clin, col );
#endif
      clin_touch(args, clin - 1);
   }

   args->col = pos_coluna( args, args->clin, I );
}

/****************************************************************************/
/* position at the beginning of the file */
VOID C_beg_file( args )

ARG * args;
/****************************************************************************/
{
   SS   clin, col;

   clin = args->clin;
   col  = args->col;

#ifdef    ET_UNDO
   if ( col != 0  ||  clin != 0 )   next_undo( args, clin, col );
#endif

   args->col = 0;
   pos_line( args, 0 );
}

/****************************************************************************/
/* position at the end of the file */
VOID C_end_file( args )

ARG * args;
/****************************************************************************/
{
   SS   clin, col;

   clin = args->clin;
   col  = args->col;

#ifdef    ET_UNDO
   if ( col  != args->len[ clin ]  ||
        clin != args->t_lines - 1 )   next_undo( args, clin, col );
   undo_on = U_DESLIGA;
#endif

   pos_line( args, args->t_lines - 1 );

   C_end( args );

#ifdef    ET_UNDO
   undo_on = U_NEXT | U_LINHA;
#endif
}

/****************************************************************************/
/* process shift-tab key */
VOID C_shift_tab( args )

ARG * args;
/****************************************************************************/
{
   SS  clin, col, C, I = 0;

   col  = args->col;
   clin = args->clin;

   C = cur_aparente( args, clin, col );

   if ( C > 0 )
   {
#ifdef    ET_UNDO
      next_undo( args, clin, col );
#endif
      I = ((C - 1) / tab_width) * tab_width;
   }

   args->col = pos_coluna( args, clin, I );
}

/***************************************************************************/
/*** Goto LINE ***/
VOID line_go( args )

ARG * args;

/***************************************************************************/
{
   SS  tecla, I, clin, col;
   SC  linha[ 15 ];

   clin = args->clin;
   col  = args->col;

   linha[ 0 ] = '\0';

   tecla = pede_cadeia( loc_str( STR_EDIT, EDIT_26 ), linha, 6,
                        loc_str( STR_EDIT, EDIT_27 ),
                        '9', NULL );

   if ( tecla != K_ESC )
   {
      I = atoi( linha ) - 1;

      if ( I < 0 )                   I = 0;
      if ( I > args->t_lines - 1 )   I = args->t_lines - 1;

#ifdef    ET_UNDO
   if ( col  ||  clin != I )   next_undo( args, clin, col );
#endif

      args->col = 0;
      pos_line( args, I );
   }

   insert_mode( args, insercao );
}

/**************************************************************************/
/* Reimprime o buffer na janela corrente */
/* Recalcula beg_col e beg_line */
VOID fresh_buffer( args )

ARG *args;
/**************************************************************************/
{
   insert_mode( args, ins_mode );
   linha_visivel( args );
   args->end_col = -1;  /*** Forca calculo em src_number() ***/
   fwtop( args->je );   /*** Garanta visibilidade ***/
   display = disp_file;
   if ( args->doc_mode )   text_wrap( args, args->clin );
   move_scr( args );
}

/**************************************************************************/
/* Faz e desfaz o ZOOM */
VOID C_zoom( args, inicial )

ARG *args;
SS  inicial;
/**************************************************************************/
{
   if ( args->jzoom != NULL )
   {
      fwkill( args->je );
      args->je = args->jzoom;
      args->jzoom = NULL;
      fresh_buffer( args );
   }

   else
   {
      if ( ! inicial )
      {
         WIN *JE;
         SS  X = 1, Y = 2, C, L;

         L = args->doc_mode + 1;

         if ( L  >  1 )   C = MIN( L, size_x - 2 );
         else             C = size_x - 2;

         L = size_y - 5;

#ifdef   MEDIT
         Y = 1;
         L = size_y - 4;
#endif

         if ( args->je->ident == 255 )   X = args->je->w_x;

         JE = win_make( C, L, C, L, X, Y );

         fwbot( args->je );
         args->tmp_scp = (SC *) args->je;

         if ( jan_buffer( args, JE ) )   args->jzoom = (WIN *) args->tmp_scp;

         else
         {
            fwtop( args->je );
            errorexists = YES;
            mens_2( args->jm, loc_str( STR_EDIT, EDIT_520 ) );
         }
      }
   }
}


/**************************************************************************/
/***
Passos para mudanca de janela corrente
   jan_buffer( ARG *args, WIN *Nova_jan );
***/
SS jan_buffer( args, JE )

ARG *args;
WIN *JE;
/**************************************************************************/
{
   if ( JE == NULL  ||  win_stak[ JE->w_num ] != JE )   return( 0 );

   C_zoom( args, 1 );   /*** Volta ao original ***/

   JE->w_intr     = args->je->w_intr;
   JE->w_atrib    = args->je->w_atrib;
   JE->w_defatr   = args->je->w_defatr;
   JE->campoatr   = args->je->campoatr;
   JE->w_optatr   = args->je->w_optatr;
   JE->w_boxatr   = args->je->w_boxatr;
   JE->w_curstyle = args->je->w_curstyle;
   JE->title      = args->je->title;
   JE->bottom     = args->je->bottom;
   JE->t_atr      = args->je->t_atr;
   JE->type       = args->je->type;
   JE->w_flags   |= W_VISIB;

#ifndef MEDIT
   fwbox( JE );
#endif

   args->je = JE;
   fresh_buffer( args );

   return ( 1 );
}

/**************************************************************************/
VOID init_logic( args, linha )    /* logic initialization */

ARG *args;
SS  linha;
/**************************************************************************/
{
   /*** Sinalizar para o SR estamos editado um novo Buffer ***/
   new_sr = 1;

   /*** Inicializar sempre que mudar BUFFER ***/
   /*** Blocos ***/
   blk_tipo = BLK_NOT;

   if ( args->jm == NULL  &&  args->forma != ET_MOSTRA )
   {
      args->jm = abre_mensagem( args->def_mens );
      args->abriu_jm = 1;
   }
   else    args->abriu_jm = 0;

   errorexists = YES;

   /*** Set Modo Insercao da aplicacao, variavel "insercao" ***/
   insert_mode( args, insercao );

   /*** Force o calculo do rolamento horizontal ***/
   args->end_col = -1;

   if ( args->forma == ET_APPEND  &&  args->alin < 0 )
   {
      SC   *L1;
      SS   Y, Z;

      Y = args->t_lines;

      /*** Se a ultima linha NAO apenas um paragrafo ***/
      /*** Crie uma linha adicional ***/
      if ( args->doc_mode  &&  args->len[ Y - 1 ] > 1 )
      {
         if ( Y < args->max_lines || ajusta_array( args, INC_LIN + Y) != 0 )
         {
            Z = args->len[ Y ];
            ins_linha( args, Y );
            linalloc( args, Y, Z, Z + 1 );
            if ((L1 = vm_deref( args->ptr[ Y ], 0 )) != NULL )
               L1[ Z ] = PARA_CHR;
         }
      }

      args->alin = args->t_lines - 1;

      args->col = 0;
      pos_line( args, 32000 );
   }

   else if ( args->estado < 2 )
   {
      /*** Em TEXT_MODE e' a propria linha, porem ***/
      /*** Em DOC_MODE transforma paragrafo em linha ***/
      linha = acha_paragrafo( args, linha );
      pos_line( args, linha );
   }

   else   linha_visivel( args );   /*** Garanta a visibilidade vertical ***/

   args->estado = 2;

   if ( args->tipo == ET_MEDIT || args->tipo == ET_FILE )  fwtop( args->je );

   if ( w_refresh <= 10 )   w_refresh = 0;

   display = disp_file;

   if ( args->doc_mode )   text_wrap( args, args->clin );
   move_scr( args );

#ifdef    ET_UNDO
   restart_undo( args );
#endif
}


/**************************************************************************/
VOID end_logic( args, code )    /* logic finalization */

ARG * args;
SS   code;
/**************************************************************************/
{
   if ( code < 0 )
   {
      beep( args );
      mens_2( args->jm, loc_str( STR_EDIT, EDIT_521 ), code );
      beep( args );
      fwnqgc( args->je );
      args->tecla = code;
   }

#ifndef   MEDIT
   C_zoom( args, 1 );
#endif

   if ( args->abriu_jm )   fecha_mensagem( args->jm );

   /*** O rodape deve estar vazio se o buffer nao estiver ativo ***/
   args->je->bottom = str_vazia;

#ifdef    ET_UNDO
   /* para evitar chamar funcao em overlay - DOS */
   if ( ! args->doc_mode )
      close_undo( args );
#endif

	longjmp ( end_rtn [ --num_texto ], -1 );
}


/***************************************************************************/
/* memory status display */
VOID mem_stat( args )

ARG * args;
/***************************************************************************/
{
   SS mem_text;

   mem_text = (SS) (tam_buffer( args, args->tipo ) / 1024);

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_501 ), mem_text + 1 );
}

/******************************************************************************/
/* insert key toggle routine*/
VOID C_ins( args )

ARG * args;
/******************************************************************************/
{
   insert_mode( args, ! ins_mode );
}


/******************************************************************************/
/* insert key toggle routine*/
VOID insert_mode( args, tipo )

ARG * args;
SS  tipo;
/******************************************************************************/
{
   /*** A variavel "insercao" e' da aplicacao ***/
   insercao = ins_mode = tipo;

   if ( ins_mode )   cursblk( args );
   else              curslin( args );
}


/****************************************************************************/
/***
Insere Atributos em Doc_mode
N - Normal     E - Enfatizado     S - Sublinhado      I - Italico
               B - Bold           U - Underline
***/
VOID  diretivas( args )

ARG *args;
/****************************************************************************/
{
   SS   C, D;

/***
   C = args->tecla;
   case INSCTRLE    :   if ( args->doc_mode ) ins_atrib( args );
                        continue;
***/
   C = 'P';
   if ( C < 32 )  C += 64;

   errorexists = YES;
   mens_1( args->jm, "Ctrl-%c", C );

   D = fwnqgc( args->je );

   if  ( D < 32 )  D += 64;      /* O segundo char nao pode ser CTRL */
   else if ( D >= 'a' && D <= 'z' )  D -= 32;       /* Nem minusculo */

   mens_1( args->jm, "Ctrl-%c-%c", C, D );

   if ( args->doc_mode &&
       (args->forma == ET_EDITA || args->forma == ET_APPEND) )
   {
      switch ( D )
      {
      default  :  break;
      case 'B' :
      case 'E' :  to_text( args, ET_ENFAT,  1 );  break;
      case 'N' :  to_text( args, ET_NORMAL, 1 );  break;
      case 'S' :
      case 'U' :  to_text( args, ET_SUBLIN, 1 );  break;
      case 'I' :  to_text( args, ET_ITAL,   1 );  break;
      }
   }

   switch ( D )
   {
   default  :  break;
   case 'C' :  if ( ! args->doc_mode &&
                   (args->forma == ET_EDITA || args->forma == ET_APPEND) )
               {
                  insere_ctrl( args );
               }
               break;
   case 'H' :  if ( ++mostra_oito > 3 )   mostra_oito = 0;   break;
   case 'A' :  no_br_fim    = ! no_br_fim;         break;
   case 'F' :  tab_ug_sp    = ! tab_ug_sp;         break;
   case 'J' :  et_just      = ! et_just;           break;
   case 'T' :  tab_tab      = ! tab_tab;           break;
   case 'L' :  cr_new_line  = ! cr_new_line;       break;
   }

   display = disp_file;
}


