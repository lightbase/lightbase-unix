
/*****
   Editor de Texto do Light Base
*****/

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  "window.h"
#include  "mensagem.h"

#ifdef    MSDOS
#include  <io.h>
#else
#ifdef    VAXVMS
#include  <file.h>
#else
#include  <fcntl.h>
#endif
#endif

#include  MEMORY

#define   MTE_VAR
#include  "ethead.h"
#undef    MTE_VAR

SS edit_buffer( args, forma, JE, JM, linha )

ARG *args;
SS   forma;
WIN *JE;
WIN *JM;
SS   linha;        /*  Linha inicial de edicao em arquivo TEXTO
                    *  Paragrafo inicial quando em "doc_mode".
                    */
{
   US   C;
   SC   *ant_ajuda;
   SS   ret_jmp;
#ifdef        MSDOS
   UI   di;  /* longjmp nao restaura DI no TURBO_C ++ */
#endif

   if ( args == NULL || args->estado < 1  ||
        JE == NULL   || win_stak[ JE->w_num ] != JE )   return( -1 );

   ant_ajuda = win_ajuda;
   win_ajuda = et_ajuda;

#ifdef        MSDOS
   di = _DI;  /* longjmp nao restaura DI no TURBO_C ++ */
#endif

	ret_jmp = setjmp ( end_rtn [ num_texto++ ] );

#ifdef        MSDOS
   _DI = di;  /* longjmp nao restaura DI no TURBO_C ++ */
#endif

   if ( ret_jmp != 0 )
   {
      win_ajuda = ant_ajuda;
      return( args->tecla );
   }

   if ( args->jzoom == NULL )
   {
      args->je = JE;
      args->je->w_flags |= W_VISIB;
   }

   else
   {
      args->jzoom = JE;
      args->jzoom->w_flags |= W_VISIB;
   }

   args->forma  = forma;
   args->jm     = JM;

   init_logic( args, linha );

   if ( args->forma == ET_MOSTRA )
   {
      /*** Este fresh_buffer() e' muito lento talvez nao precise. ***/
      fresh_buffer( args );
      end_logic( args, 0  );
   }

   while( 1 )
   {
      C = next_letter( args );

#ifdef    UNDO_DEBUG
      if ( C == '/' )
      {
         SC   linha[ 120 ];
         SL   L;
         SS   I, J;

         J = 45;

         W_curmove( 0, 0 );
         prclear( );
         doflush( );

         L = ftell( args->FU );

         fseek( args->FU, 0L, 0 );

         while ( fgets( linha, 100, args->FU ) != NULL )
            printf( linha );

         printf( "\nRedo = %d   Pos = %ld   Ant = %ld   Off = %ld",
                 args->redo, args->pos, args->ant, args->off );

         fseek( args->FU, L, 0 );

         getch( );
         win_redo();
         continue;
      }
#endif

      if ( C < 256 )
      {
         /*** E_PRINT( ) -> coloque no texto ***/
         if ( args->forma == ET_EDITA   ||
             (args->forma == ET_APPEND  &&  args->clin >= args->alin) )
         {
            to_text( args, C, 0 );
         }
         else
         {
            errorexists = YES;
            mens_1( args->jm, loc_str( STR_EDIT, EDIT_36));
            beep( args );
         }
         continue;
      }

      switch ( C - 256 )
      {
      default          :   if ( args->forma == ET_EDITA   ||
                               (args->forma == ET_APPEND  &&
                                args->clin  >= args->alin)  )   break;
                           errorexists = YES;
                           mens_1( args->jm, loc_str( STR_EDIT, EDIT_36));
                           beep( args );                    continue;
      case LINHACIMA   :   row_touch( args, 0 );            continue;
      case LINHABAIXO  :   row_touch( args, args->je->w_height - 1 ); continue;
      case LINHAMEIO   :   row_touch( args, args->je->w_height / 2 ); continue;
      case DESFAZ      :   C_undo( args );                  continue;
      case REFAZ       :   C_redo( args );                  continue;
      case PLINTELA    :   initela( args );                 continue;
      case ULINTELA    :   fimtela( args );                 continue;
      case ROLAESQ     :   leftscroll( args );              continue;
      case ROLADIR     :   rightscroll( args );             continue;
      case SOBE        :   C_up( args );                    continue;
      case DESCE       :   C_down( args );                  continue;
      case DIREITA     :   C_right( args );                 continue;
      case ESQUERDA    :   C_left( args );                  continue;
      case INILINHA    :   C_home( args );                  continue;
      case FIMLINHA    :   C_end( args );                   continue;
      case ROLACIMA    :   C_up_scroll( args );             continue;
      case ROLABAIXO   :   C_down_scroll( args );           continue;
      case PROPAGINA   :   C_pgdn( args );                  continue;
      case ANTPAGINA   :   C_pgup( args );                  continue;
      case INIARQUIVO  :   C_beg_file( args );              continue;
      case FIMARQUIVO  :   C_end_file( args );              continue;
      case TABANT      :   C_shift_tab( args );             continue;
      case ANTPALAVRA  :   C_prev_word( args );             continue;
      case PROPALAVRA  :   C_next_word( args );             continue;
      case VAIPRALIN   :   line_go( args );                 continue;
      case PROCTEXTO   :   sr_manager( args, BUSCA, 0, 1 ); continue;
      case PROCTRAS    :   sr_manager( args, BUSCA, 0, 0 ); continue;
      case TAMTEXTO    :   mem_stat( args );                continue;
      case INSEREOUNAO :   C_ins( args );                   continue;
      case ZOOM        :   C_zoom( args, 0 );               continue;
      case PROCPAR     :   et_matching( args );             continue;
      case INIBLCOL    :   C_col_blk( args );               continue;
      case INIBLLIN    :   C_line_blk( args );              continue;
      case INIBLCHAR   :   C_chr_blk( args );               continue;
      case BLOCOCOPIA  :   blk_copy( args, blk_scrap, 0 );  continue;
      case APPBLCOPIA  :   blk_copy( args, blk_scrap, 1 );  continue;
      case DESMARCABLO :   C_blk_not( args );               continue;
      case MUDATRANS   :   blk_mudatrans( args );           continue;
      case GRAVAMACRO  :   et_grava_macro( args );          continue;
      case ARQMACRO    :   arq_macro( args );               continue;
      case REPITABUSCA :   repita_busca( args );            continue;
      case REPITAFRENTE:   repita_ft( args, 1 );            continue;
      case REPITATRAS  :   repita_ft( args, 0 );            continue;
      case INSCTRLE    :   diretivas( args );               continue;
      case EXPORTE     :   C_export( args );                continue;
      case IMPRIMEARQ  :   imprime_arq( args );             continue;
      case CODCHAR     :   qual_char( args );               continue;
      case ABANDONA    :   if ((args->forma != ET_EDITA  &&
                                args->forma != ET_APPEND ) ||
                                quit_file( args ) )
                           {
                              args->tecla = K_ESC;
                              end_logic( args, 0 );
                           }
                           continue;
      }

      switch ( C - 256 )
      {
      default          :   errorexists = YES;
                           mens_1( args->jm, loc_str( STR_EDIT, EDIT_37));
                           beep( args );                    continue;
      case RETORNO     :   C_cr( args );                    continue;
      case VOLTAESPACO :   C_bk_spc( args );                continue;
      case PROXTAB     :   C_tab( args );                   continue;
      case APAGACAR    :   C_del( args );                   continue;
      case APAGAPAL    :   C_d_word( args );                continue;
      case SALVARQ     :   save_file( args );               continue;
      case APAGALINHA  :   delete_line( args );             continue;
      case DUPLINHA    :   C_dup_line( args );              continue;
      case APAGAFIMLIN :   del_end_line( args );            continue;
      case APAGABLOCO  :   blk_delete( args );              continue;
      case BLOCOINSERE :   blk_ins( args, blk_scrap );      continue;
      case BLOCOMOVE   :   blk_move( args, blk_scrap, 0 );  continue;
      case APPBLMOVE   :   blk_move( args, blk_scrap, 1 );  continue;
      case BLOCOUPPER  :   blk_upplwr( args, BLK_UPPER );   continue;
      case BLOCOLOWER  :   blk_upplwr( args, BLK_LOWER );   continue;
      case INDENTBLOCO :   blk_indent( args );              continue;
      case IMPORTE     :   C_import( args );                continue;
      case TROCATEXTO  :   sr_manager( args, TROCA, 0, 1 ); continue;
      case TROCATRAS   :   sr_manager( args, TROCA, 0, 0 ); continue;
      case EXEMACRO    :   et_exec_macro( args );           continue;
      case MARGEMPARA  :   C_margens( args );               continue;
      case INSLIDEPO   :   ins_l_after( args );             continue;
      case INSLIANTES  :   ins_l_bef( args );               continue;
      case QUEBRALINHA :   split_line( args, 0, 0 );        continue;
      case JUNTAPROXL  :   line_join( args );               continue;
      case EXPRECHAR   :   exprechar( args );               continue;
      }
   }
}

/***
Retorno:
         0 .. 255 -> e_print(), pode imprimir !!!
         > 255    -> e' comando interno do editor, execute !
         > 512    -> e' macro localize-a e execute !
         < 0      -> comandos externos ao editor, ex: muda janela MEDIT.
***/
SS pegacomando( janela )

WIN *janela;
{
   static hot_act = -1;

   SS  c, c1 = -1;
   SS  I, FUN;

   while( 1 )
   {
start:
      c = fwnqgc( janela );

      if( hot_act >0 )
      {
	 --hot_act;
         if ( e_print( c ) ) goto volta;
      }

      /**********************/
      /* procurar o comando */

      for( I = 0;   vet_com[ I ].num_comando;   I++ )
      {
         if ( c == vet_com[ I ].prim_tecla )   break;
      }

      if ( !vet_com[ I ].num_comando )
      {
         if ( e_print( c ) ) goto volta;
      }

      else
      {
         if ( ! vet_com[ I ].seg_tecla )
         {
            if ( vet_com[ I ].num_comando >= 512 )
            {
               if( hot_act >= 0 )
               {
                  if( hot_act == 0 ) hot_act = -1;
                  continue;
               }
               hot_act = strlen( Hot_Keys[ vet_com[ I ].num_comando - 512 ] );
               w_ungs( Hot_Keys[ vet_com[ I ].num_comando - 512 ] );
               goto start;
            }

            else goto  e_escape;
         }

         for ( FUN = 0;   ! w_kbhit( janela )    &&
                          FUN < MAX_MULTKEY         &&
                          key_func[ FUN ].tecla  != c   &&
                          key_func[ FUN ].funcao != NULL;   FUN++ )
            ;

         if ( ! w_kbhit( janela )  &&
              FUN < MAX_MULTKEY    &&  f_key_func != NULL )
         {
            (*f_key_func)( key_func[ FUN ].funcao );
            fwsetcursor( janela );
            FUN = 1;
         }

         else  FUN = 0;

         c1 = fwnqgc( janela );
         if( hot_act >0 ) --hot_act;

         if ( FUN == 1  &&  f_key_func != NULL )
         {
            (*f_key_func)( def_key_func );
            fwsetcursor( janela );
         }

         for (   ;   vet_com[ I ].prim_tecla == c;   I++ )
         {
            if ( c1 == vet_com[ I ].seg_tecla )
            {
               if ( vet_com[ I ].num_comando >= 512 )
               {
                  if( hot_act >0 )
                  {
                     --hot_act;
                     if( hot_act == 0 ) hot_act = -1;
                     goto start;
                  }
                  hot_act = strlen( Hot_Keys[ vet_com[ I ].num_comando - 512 ] );
                  w_ungs( Hot_Keys[ vet_com[ I ].num_comando - 512 ] );
                  goto start;
               }
e_escape:
               ult_comando = I;
               switch ( vet_com[ I ].escape )
               {
               case   0:   /*** Comando Interno - MAIS 256 ***/
                           c = ((SS) vet_com[ I ].num_comando) + 256;
                           goto volta;

               case   1:   /*** Comando Escape - Negativo ***/
                           c = - ((SS) vet_com[ I ].num_comando);
                           goto volta;

               case   2:   /*** Comando MACRO - MAIS 512 ***/
                           c = ((SS) vet_com[ I ].num_comando) + 512;
                           goto volta;
               }
            }
         }
         if ( e_print( c ) )
         {
            w_ungc( c1 );
            goto volta;
         }
      }

      if ( c1 != K_ESC && c1 != ' ' && c1 != CTRLU ) win_bell( BEL_INVALID );
      if( hot_act == 0 ) hot_act = -1;
   }
volta:
   if( hot_act == 0 ) hot_act = -1;
   return  c;

}

/**************************************************************************/
/* accpet next letter and do init logic */
SS next_letter( args )

ARG * args;
/**************************************************************************/
{
   SS  tecla;
   WIN *W;

   W = args->je;

   salta_atr( args, args->tecla );

   if ( ! w_kbhit( W ) )
   {
atualiza:
      if ( args->doc_mode )   text_wrap( args, args->clin );

      move_scr( args );
   }

   /*** Devemos acompanhar o bloco final ***/
   else if ( blk_tipo != BLK_NOT )
   {
      blk_lf = args->clin;
      blk_cf = cur_aparente( args, blk_lf, args->col );
   }

novamente:

   args->tecla = tecla = fwnqgc( W );

   /*** Tecla de final de macro ***/
   if ( tecla == -1 ) {  display = disp_file;  goto  atualiza;  }

   if ( strchr( args->escape, args->tecla ) != NULL )   end_logic( args, 0 );

   w_ungc( tecla );

   args->tecla = tecla = pegacomando( W );

   if ( tecla >= 512 )  /*** Comando de MACRO */
   {
      /*** Comandos de macros comecam de 1 e vamos aproveitar indice 0 ***/
      exec_macroarq( args, mac_files[ tecla - 512 - 1 ] );
      goto novamente;
   }

   /*** Comando de Escape do MEDIT */
   if ( tecla < 0 ) {  args->tecla = -tecla;  end_logic( args, 0 );  }

   if ( errorexists ) {  clrerror( args );  }

   /*** when in document mode, convert CR to the paragraph character ***/
   if ( args->doc_mode  &&  tecla == RETORNO + 256 )
   {
      args->tecla = tecla = PARA_CHR;
   }

   return  tecla;
}

/**********************************************************************/
/* process asc characters */
VOID  to_text( args, caracter, insere )

ARG * args;
SS   caracter, insere;
{
   SC   *L1;
   SS   i, save_len, clin, col, tam;

   col  = args->col;
   clin = args->clin;

   save_len = args->len[ clin ];  /* store line length */

   L1 = vm_deref( args->ptr[ clin ], 0 );

#ifdef    ET_UNDO
   next_undo( args, clin, col );

   if ( L1 == NULL )   tam = 0;
   else                tam = save_len;
   undo_linha( args, U_ANTES, clin, tam, L1 );
#endif

   /*** Se insert_mode e coluna for menor que o tamanho da linha OU ... ***/
   /*** ... se doc_mode e o caracter for PARA_CHR OU se doc_mode e ... ***/
   /*** ... o caracter a inserir for PARA_CHR, OU ... ***/
   /*** ... o caracter a inserir for ET_FORM, INSIRA o caracter. ***/

   if ( col < save_len  &&
       (ins_mode ||
        insere   ||
       (args->doc_mode  &&
        L1 != NULL      &&
       (caracter == PARA_CHR  ||
        (UC) L1[ col ] == PARA_CHR  ||  (UC) L1[ col ] == ET_FORM1) ) ) )
   {
      if ( (i = save_len) >= LINE_LEN )   goto fim;

      linalloc( args, clin, i, i + 1);

      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
      {
         for (   ;   i > col;   i-- )   L1[ i ] = L1[ i - 1 ];
      }

      if ( display != disp_file )   display = disp_line;
   }

   else if ((col + 1) > save_len )
   {
      linalloc( args, clin, save_len, col + 1 );
   }

   if ((L1 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
   {
      /* move spaces in unused area */
      for ( i = save_len;   i < col;   i++ )   L1[ i ] = ' ';

      L1[ col ] = caracter;
   }

   if ( col  <  LINE_LEN - 1 )   args->col++;

   args->f_mod = YES;

fim:

#ifdef    ET_UNDO
   if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   tam = 0;
   else                                   tam = args->len[ clin ];

   undo_linha( args, U_DEPOIS, clin, tam, L1 );
#endif
}

/****************************************************************************/
VOID disp_stat( args, cur_row, cur_col, cur_line )

ARG * args;
US cur_row;
US cur_col;
US cur_line;

/****************************************************************************/
{
   SC   string[ 50 ];
   SS   A;
   WIN  *W;

   W = args->je;

   if ( W->w_len <= 15  ||  args->forma == ET_MOSTRA )
   {
      args->bot[ 0 ] = '\0';
   }
   else
   {
      cur_row  += 1;
      cur_col  += 1;
      cur_line += 1;
      if ( cur_col  > 32000 )  cur_col  = 32000;
      if ( cur_line > 32000 )  cur_line = 32000;
      sprintf( string, loc_str( STR_EDIT,  EDIT_25 ),  cur_line, cur_col );
      /*** Cuidado para nao estourar */
      A = strlen( string );
      if ( A >= TAM_BOT )   string[ A ] = '\0';
      strcpy( args->bot, string );
      W->bottom = args->bot;
      bot_remap( W );
   }
}

/****************************************************************************/
/* move screen memory to ROM memory                    */
VOID move_scr( args )

ARG * args;
/****************************************************************************/
{
   SS   col;
   WIN  *W;

   W = args->je;

   col = cur_aparente( args, args->clin, args->col );

   if ( blk_tipo != BLK_NOT )
   {
      display = disp_file;
      blk_lf  = args->clin;
      blk_cf  = col;
   }

   if ( args->forma == ET_MENU )   display = disp_file;

   scr_number( args, col );

   if ( display == disp_file )   set_scr( args, 0, W->w_height - 1 );

   else
   {
      if ( args->doc_mode )      set_scr( args, l_inicio, l_fim );
      else                       set_scr( args, args->row, args->row );
   }

   display = 0;

   if ( !w_kbhit( W ) )   (*lin_col)( args, args->row, col, args->clin );

   /*** Garanta tipo do cursor quando voltar dos Utilitarios ***/
   W->w_curstyle = (ins_mode) ? CS_INTER : CS_NORMAL;

   fwat( W->w_cx, W->w_cy, W );
   fwrefresh ( W );
   fwsetcursor ( W );

   if ( !w_kbhit( W )  &&
        args->funcao != NULL && (*args->funcao)( args ) == -1 )
   {
      end_logic( args, 0 );
   }
}


/***************************************************************************/
VOID set_scr( args, frst_row, last_row )

ARG * args;
SS frst_row, last_row;
/***************************************************************************/
{
   SC   *L1, just[ MAX_WRAP + 20 ];
   SS   X, p_ver, p_hor, line, c_off, tam, bcol, B, mar_esq;
   UC   *C;
   US   *T, normal, atr, atr_rev, V_char, V_atr, C_true;
   WIN  *W;

   /*** Garanta a margem esquerda para ! Doc_Mode ***/
   mar_esq = 0;

   W    = args->je;
   bcol = args->beg_col;
   X    = args->col - bcol;

   if ( frst_row > last_row )   goto fim;

   if ((last_row - frst_row) >= ((W->w_height * 3) / 4))  W->w_anymod = 2;

   /* inicialize linha absoluta */
   line = args->beg_line + frst_row;

   atr = normal = W->w_boxatr << 8;
   atr_rev      = W->w_optatr << 8;

   C_true = ( ! args->doc_mode && mostra_oito == 1 )  ?  1 << 15  :  0;

   for ( p_ver = frst_row;   p_ver <= last_row;   p_ver++, line++ )
   {
      if ( W->w_anymod  <  2 )
      {
         W->w_cy = p_ver;
         W->w_cx = 0;
         wu_lmark( W, W->w_len );
      }

/***
if ( args->len[ line ] > 500 ){
debug( "linha = %d  Tamanho = %d", line, args->len[ line ] );
continue;
}
***/
      /*** Posicao na janela ***/
      T = &W->w_ary[ p_ver * W->w_len ];

      /*** Complete com brancos se linha nao for valida ***/
      if ( line >= args->t_lines  ||
           (tam  = args->len[ line ])  == 0  ||
           (L1   = vm_deref( args->ptr[ line ], 0 )) == NULL )
      {
         tam = c_off = bcol;
         C = (UC *) just;
      }

      else
      {
         /*** Posicao na linha ***/
         if ( ! args->doc_mode )
         {
            tam = tab_linha( args, just, L1, line, &X );
            c_off = 0;
            C =  (UC *) &just[ c_off ];
         }

         else
         {
            /*** Ache o atributo corrente ***/
            /*** Nao pode ficar antes do for() ... ***/
            /*** ... por causa do rolamento horizontal. ***/
            B   = pos_coluna( args, line, bcol );
            atr = atr_corrente( args, line, B - 1 );

            tam = margem_linha( args, just, L1, line, &c_off, &X, &mar_esq );
            C =  (UC *) &just[ c_off ];
         }
      }

      for ( p_hor = 0;   p_hor < W->w_len;   C++, c_off++ )
      {
         if ( c_off >= tam ) {  V_char = ' ';   V_atr = normal;  }

         else
         {
            if ( args->doc_mode )
            {
               switch ( *C )
               {
               /* Salte o formato do paragrafo e continue */
               case ET_FORM1 :  for( C++, c_off++;
                                     c_off < tam  &&  *C != ET_FORM2;
                                     C++, c_off++ )      /* Nada */;
                                continue;
               /* Se cair nestes cases set o novo atributo e continue */
               case ET_NORMAL:  atr = W->w_boxatr << 8;  continue;
               case ET_SUBLIN:  atr = atr_rev;           continue;
               case ET_ENFAT :  atr = W->w_defatr << 8;  continue;
               case ET_ITAL  :  atr = ATR_BLINK   << 8;  continue;
               }
            }

            V_char = *C;

            /*** Em ! Doc_Mode a mar_esq = 0 ***/
            V_atr = ( c_off < mar_esq )  ?  normal  :  atr;
         }

         if ( blk_tipo != BLK_NOT )
         {
            V_atr = blk_atr( args, V_atr, line, bcol + p_hor );
         }

         if ( args->forma == ET_MENU && line == args->clin ) V_atr = atr_rev;

         *T++ = V_char | V_atr | C_true;

         p_hor++;
      }
   }

fim:

   if ( X < 0 )   X = 0;

   W->w_cx = X;
   W->w_cy = args->row;
}

/***************************************************************************/
/* Retorna o atributo corrente, com relacao a BLOCOS marcados */
US blk_atr( args, atr, lin, col )

ARG *args;
US  atr;
SS  lin, col;
{
   SS  tmp, li, lf, ci, cf;
   US  atr_rev;

   if ( blk_tipo == BLK_NOT )   return( atr );

   li = blk_li;  lf = blk_lf;
   ci = blk_ci;  cf = blk_cf;

   if ( blk_tipo == BLK_CHR )
   {
      if ( li > lf )
      {
         tmp = li; li = lf; lf = tmp;
         tmp = ci; ci = cf; cf = tmp;
      }

      else if ( li == lf && ci > cf ) { tmp = ci; ci = cf; cf = tmp; }
   }

   else
   {
      if ( li > lf ) { tmp = li; li = lf; lf = tmp; }
      if ( ci > cf ) { tmp = ci; ci = cf; cf = tmp; }
   }

   atr_rev = args->je->w_optatr << 8;

   if ( blk_tipo == BLK_LIN )
   {
      if ( lin >= li  &&  lin <= lf  )   return( atr_rev );
   }

   else if ( blk_tipo == BLK_COL )
   {
      if ( lin >= li  &&  lin <= lf  &&
           col >= ci  &&  col <= cf  )   return( atr_rev );
   }

   else if ( blk_tipo == BLK_CHR )
   {
      if ( li == lf )
      {
         if ( lin == li  &&  col >= ci  &&  col <= cf  )   return( atr_rev );
      }

      else if ( lin >  li  &&  lin <  lf  ||
                lin == li  &&  col >= ci  ||
                lin == lf  &&  col <= cf  )   return( atr_rev );
   }

   return( atr );
}

/***************************************************************************/
/* find width screen number and beg and end col */
SS scr_number( args, col )

ARG * args;
SS  col;      /*** Coluna Aparente ***/
/***************************************************************************/
{
   SS   I, J;

   if ( col >= args->beg_col  &&
        col <= args->end_col  &&  args->end_col != -1  )   return  0;

   I = args->je->w_len;

   if ( I == 1 )   args->beg_col = args->end_col = col;

   else
   {
      if ( col < args->beg_col  ||  col > args->beg_col + I - 1 )
      {
         J = I * 3 / 4;
         args->beg_col = col - J;
         if ( args->beg_col < 0 )   args->beg_col = 0;
      }

      args->end_col = args->beg_col + I - 1;
   }

   display = disp_file;

   return  1;
}

/**************************************************************************/
/* clear error line */
VOID clrerror( args )

ARG * args;
/**************************************************************************/
{
    mens_1( args->jm, args->def_mens );
    errorexists = 0;
}

/***************************************************************************/
/***
wu_remap( win->w_x - 1, win->w_y - 1,win->w_alen + 2, win->w_aheight + 2 );
wu_remap( x, y, l, h )    rebuild win_map, and inform main_win
***/
/*** Remapeia em main_win o rodape ***/
VOID bot_remap( W )

WIN *W;
/***************************************************************************/
{
   UC     *P;
   SS     A, B;

   if ( ! (W->w_flags & W_BOXED) )   return;

   A = (W->w_y - 1) + (W->w_aheight + 2) - 1;

   if ( strange_term == 1 )
   {
      if ( w_linemod[ 0 ][ A ]  <  (W->w_x - 1) + (W->w_alen + 2) )
           w_linemod[ 0 ][ A ]  =  (W->w_x - 1) + (W->w_alen + 2);

      if ( w_linemod[ 1 ][ A ]  >  (W->w_x - 1) )
           w_linemod[ 1 ][ A ]  =  (W->w_x - 1);
   }

   P = main_win->w_mod + (main_win->w_modblock * A) + ((W->w_x - 1) / 8);

   for( B = (W->w_alen + 2) + 7;   B > 0;   B -= 8 )   *P++ = 255;

   if ( ! main_win->w_anymod )    main_win->w_anymod = 1;
}

VOID beep( args )

ARG * args;
{
   args = args;

   win_bell( BEL_GEN );
}


VOID curslin( args )

ARG * args;
{
   WIN  *W;

   if ((W = args->je) != NULL )   W->w_curstyle = CS_NORMAL;
}


VOID cursblk( args )

ARG * args;
{
   WIN  *W;

   if ((W = args->je) != NULL )   W->w_curstyle = CS_INTER;
}


/***************************************************************************/
/*
Em MODO DOCOMENTO:

Volta a posicao do Cursor MAIS a quantidade de falsos brancos ...
... MENOS a quantidade de atributos em uma linha ate a coluna indicada.

Em MODO TEXTO

Volta a posicao do Cursor MAIS a quantidade de falsos brancos ...
... expandidos pelos TABs MAIS os caracteres CTRL ...
... em uma linha ate a coluna indicada.

*/
SS   cur_aparente( args, linha, posicao )

ARG  *args;
SS   linha, posicao;
/***************************************************************************/
{
   SC   *L1, *C, just[ MAX_WRAP + 20 ];
   SS  I, J, K, pcj = 0, mar, atr, len, qap, com, tbl, ret;
   SS  mar_esq, mar_pri, mar_dir;

   ret = posicao;
   len = args->len[ linha ];

   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return  ret;

   if ( args->doc_mode )
   {
      mar = mar_atr( args, L1, linha, len, posicao, &atr, &qap,
                     &mar_esq, &mar_pri, &mar_dir );

      p_just_line( just, L1, mar, len, (SS) UG_SP, posicao, &pcj );

      ret += pcj - qap + mar_esq;
/***
debug( "Q_Just = %d", ret );
***/
   }

   else
   {
      com = tbl = COTA( 1, tab_width, TAB_MAX );

      for ( I = J = 0, C = L1;   I < len  &&  I < posicao;   I++, C++ )
      {
         if ( *C == TAB_CHAR )
         {
            for ( K = 0;   K < com;   K++ )   J++;
            J--;
            com = tbl;
         }

         else
         {
            if ( E_CTRL( *C ) ) {  J++;  if ( --com == 0 )   com = tbl;  }

            else if ( E_8BITS( *C ) )
            {
               K = ( mostra_oito == 2 ) ? 3 : 2;
               for (   ;   K;   K-- )
               {
                  J++;
                  if ( --com == 0 )   com = tbl;
               }
            }

            if ( --com == 0 )   com = tbl;
         }
      }

      ret += J;
   }

   if ( ret < 0 )   ret = 0;

   return( ret );
}

/***************************************************************************/
/***
Volta a coluna correta levando em conta ...
... os atributos e justificacao no MODO DOCUMENTO ...
... e expancao de TABs e CTRL no MODO TEXTO.
***/
SS   pos_coluna( args, linha, posicao )

ARG  *args;
SS   linha, posicao;
/***************************************************************************/
{
   SC  *L1, *C, just[ MAX_WRAP + 20 ];
   US  car;
   SS  ret, I, J, K, A, form, mar, atr, qap, len, des, com, tbl, pos;
   SS  mar_esq, mar_pri, mar_dir;

   ret = posicao;
   len = args->len[ linha ];

   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return  ret;

   if ( args->doc_mode )
   {
      mar = mar_atr( args, L1, linha, len, posicao, &atr, &qap,
                     &mar_esq, &mar_pri, &mar_dir );

      mar = p_just_line( just, L1, mar, len, (SS) UG_SP, posicao, &des );

      for ( A = mar_esq, form = I = pos = 0, C = just;  A < posicao;  I++, C++ )
      {
         if ( I < mar )   car = (UC) *C;
         else             car = ' ';

         if ((UC) car !=  UG_SP )   pos++;

         if ( form  ||  strchr( et_atr, car ) != NULL )
         {
            if ( form )                       form--;
            else if ((UC) car == ET_FORM1 )   form = TMF - 1;
         }

         else   A++;
      }

      if ( pos >= len ) {  pos = len;   if ( pos > 0 )   pos--;  }

      else if ( pos < 0 )  pos = 0;

      ret = pos;
/***
debug( "Pos_Just = %d", ret );
***/
   }

   else
   {
      com = tbl = COTA( 1, tab_width, TAB_MAX );

      for ( I = J = 0, C = L1;   J < posicao;   I++, C++ )
      {
         if ( I < len  &&  *C == TAB_CHAR )
         {
            for ( K = 0;   K < com  &&  J < posicao;   K++ )   J++;
            com = tbl;
         }

         else
         {
            if ( I < len  &&  E_CTRL( *C ) )
            {
               if ( --com == 0   )   com = tbl;
               if ( ++J >= posicao )   continue;
            }

            else if ( I < len  &&  E_8BITS( *C ) )
            {
               K = ( mostra_oito == 2 ) ? 3 : 2;
               for (   ;   K;   K-- )
               {
                  if ( --com == 0 )   com = tbl;
                  if ( ++J >= posicao )   continue;
               }
            }

            J++;
            if ( --com == 0 )   com = tbl;
         }
      }

      if ( I >= LINE_LEN )   I = LINE_LEN - 1;

      ret = I;
   }

   return  ret;
}

/***************************************************************************/
/***
Volta quantidade de atributo na linha.
Se pos_real estiver TRUE a contagem ira' ate' esta posicao na linha ...
... porem se pos_real for ZERO a contagem ira' ate' a posicao VISIVEL.
***/
SS   q_atributos( args, linha, posicao, pos_real )

ARG  *args;
SS   linha, posicao, pos_real;
/***************************************************************************/
{
   SC  *L1;
   UC  *C;
   SS  I, A, T, F, form;

   if ( ! args->doc_mode )    return( 0 );

   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )  return( 0 );

   F = args->len[ linha ];

   if ( pos_real )     F = MIN( F, posicao );

   for ( form=T=A=I = 0, C = (UC *) L1;  I < F  &&  T < posicao;  I++, C++ )
   {
      if ( form  ||  strchr( et_atr, *C ) != NULL )
      {
         if ( form )                      form--;
         else if ((UC) *C == ET_FORM1 )   form = TMF - 1;

         A++;
      }

      else   T++;
   }

   return ( A );
}

/***************************************************************************/
/*** Devolve o atributo corrente ***/
US   atr_corrente( args, linha, coluna )

ARG *args;
SS  linha, coluna;
{
   SC   *L1;
   UC   *P;
   SS   limite;
   US   atr = args->je->w_boxatr << 8;

   if ( ! args->doc_mode  ||
        linha >= args->t_lines  ||
        args->ptr[ linha ] == NULL )   return( atr );

   if ( coluna >= args->len[ linha ] )   coluna = args->len[ linha ] - 1;

   if ( coluna < 0 )   coluna = 0;

   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )  return( atr );

   P = (UC *) &L1[ coluna ];

   limite =  linha - 100;
   if ( limite < 0 )   limite = 0;

   while ( 1 )
   {
      switch ( *P )
      {
      default       :  break;

      case ET_NORMAL:  atr = args->je->w_boxatr << 8;  return( atr );
      case ET_SUBLIN:  atr = args->je->w_optatr << 8;  return( atr );
      case ET_ENFAT :  atr = args->je->w_defatr << 8;  return( atr );
      case ET_ITAL  :  atr = ATR_BLINK   << 8;         return( atr );
      }

      if ( coluna == 0 )
      {
         linha--;
         if ( linha < 0  ||  linha < limite )   return( atr );

         coluna = args->len[ linha ] - 1;
         if ( coluna < 0 || args->ptr[ linha ] == NULL )   return( atr );
         if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )  return( atr );

         P = (UC *) &L1[ coluna ];
         if ( *P == PARA_CHR )   return( atr );
      }

      else {  coluna--;   P--;  }
   }
}


/***************************************************************************/
SS   e_et_atr( args, linha, coluna, car )

ARG *args;
SS  linha, coluna, *car;
{
   SC  *L1;
   SS  C;

   if ( car != NULL )   *car = -1;

   if ( args->doc_mode  &&
        linha  < args->t_lines  &&
        coluna < args->len[ linha ]  &&
        (L1 = vm_deref( args->ptr[ linha ], 0 )) != NULL )
   {
      C = L1[ coluna ];

      if ( car != NULL )   *car = C;

      if ( strchr( et_atr, C ) != NULL )   return( C );
   }

   return( 0 );
}

VOID free_buffer( args )

ARG *args;
{
   SS i;

   if ( args == NULL )   return;

   /*** Titulo e rodape nao podem ficar com lixo ***/
   if ( args->je != NULL   &&
        win_stak[ args->je->w_num ] == args->je  &&
        args->je->bottom == args->bot )
   {
      args->je->bottom = str_vazia;
   }

   /*** Libere toda a area alocada */
/***
   for ( i = 0;   i < args->t_lines;   i++  )
   {
      if ( args->len[ i ] != 0 && args->ptr[ i ] != NULL )
      {
         vm_free( args->ptr[ i ] );
      }
   }
***/
   if ( args->t_lines > 0 )
   {
      for ( i = args->t_lines - 1;   i >= 0;   i--  )
      {
         if ( args->len[ i ] != 0  &&  args->ptr[ i ] != NULL )
         {
            vm_free( args->ptr[ i ] );
         }
      }
   }

#ifdef   VM_ARRAYS
   if ( args->ptr != NULL )   vm_free( args->VMptr );
   if ( args->len != NULL )   vm_free( args->VMlen );
#else
   if ( args->ptr != NULL )   free((SC *) args->ptr );
   if ( args->len != NULL )   free((SC *) args->len );
#endif

#ifdef    ET_UNDO
   /*** O NULL e' para nao mandar mensagem de FIM DE UNDO ***/
   /* para evitar chamar funcao em overlay - DOS */
   if ( ! args->doc_mode )
      end_undo( args, NULL );
#endif

   free((SC *) args );

   if ( --et_quant < 0 )   et_quant = 0;
}

SL   tam_buffer( args, tipo )

ARG *args;
SS  tipo;
{
   SS lin, incr;
   SL len;

   incr = 1;

   if ( tipo == ET_FILE )   incr = INCR_TEXTO;

   for( len = 0, lin = 0;   lin < args->t_lines;   lin++ )
   {
      /*** "incr" = 2 para o '\n' com o \r ***/
      /*** ou incr = 1 para o '\n' pois campo texto e' binario ***/
      len += args->len[ lin ] + incr;
   }

   return ( len );
}

/***
ATENCAO "orig" e' uma linha valida com informacao. !!!
Esta funcao especifica, justifica uma linha.
Retorna o tamanho da linha justificada.
Atualiza o offset da linha com relacao a BEG_COL e atributos.
Se a linha for CLIN ela calcula a posicao do cursor.
***/

SS   margem_linha( args, just, orig, linha, off, col, esq )

ARG   *args;
SC    *just, *orig;
SS    linha, *off, *col, *esq;
{
   SC  *C;
   SS  form, I, atr, qap, mar, A, len, lim, pos, des = 0, cmv = UG_SP;
   SS  mar_esq, mar_pri, mar_dir;

   len = args->len[ linha ];

   if ( ! tab_ug_sp )   cmv = ' ';

   mar = mar_atr( args, orig, linha, len, 0, &atr, &qap,
                  &mar_esq, &mar_pri, &mar_dir );

   /*** Faremos a margem esquerda ***/
   for( I = 0, C = just;   I < mar_esq;   I++, C++ )   *C = ' ';

   /*** Justifique a linha devolva a verdadeira margem direita ***/
   /*** Devolva em "des" o deslocamento da posicao do cursor ***/
   mar = p_just_line( C, orig, mar, len, cmv, args->col, &des );

   mar += mar_esq;

   /*** Calcule o offset de BEG_COL ***/
   lim = args->beg_col;
   for ( form = A = I = 0, C = just;   I < mar && A < lim;   I++, C++ )
   {
      if ( form  ||  strchr( et_atr, *C ) != NULL )
      {
         if ( form )                      form--;
         else if ((UC) *C == ET_FORM1 )   form = TMF - 1;
      }

      else   A++;
   }

   *off = pos = I;  /*** Atualize o Offset de BEG_COL ***/

   if ( args->clin == linha )
   {
      /*** Aproveite "I" em BEG_COL e veja quantos atributos ate CURSOR ***/
      lim = args->col + des + mar_esq;
      if ( lim > mar )   lim = mar;
      for ( form = atr = 0;   I < lim;   I++, C++ )
      {
         if ( form  ||  strchr( et_atr, *C ) != NULL )
         {
            if ( form )                      form--;
            else if ((UC) *C == ET_FORM1 )   form = TMF - 1;

            atr++;
         }
      }

      /*** Atualize a posicao do cursor ***/
      *col = args->col - pos - atr + des + mar_esq;
   }

   *esq = mar_esq;

   return ( mar );
}

/***
ATENCAO "orig" e' uma linha valida com informacao. !!!
Esta funcao especifica, prepara uma linha com expancao dos TABs
Retorna o tamanho da nova linha apenas em sua parte visivel
O offset da linha com relacao a BEG_COL e TABs sera' SEMPRE 0.
Se a linha for CLIN ela calcula a posicao do cursor.
***/

SS   tab_linha( args, dest, orig, linha, col )

ARG   *args;
SC    *dest, *orig;
SS    linha, *col;
{
   SC  *C, *D;
   SS  I, J, K, L, beg, len, end, tbl, com, vis, cmv = UG_SP;

   len = args->len[ linha ];
   beg = args->beg_col;
   end = args->je->w_len;
   com = tbl = COTA( 1, tab_width, TAB_MAX );
   if ( ! tab_ug_sp )   cmv = ' ';

   for ( I = J = 0, C = orig;   J < beg;   I++, C++ )
   {
      if ( I < len  &&  *C == TAB_CHAR )
      {
         for ( K = 0;   K < com;   K++ )   J++;
         com = tbl;
      }

      else
      {
         if ( I < len  &&  E_CTRL( *C ) )
         {
            J++;
            if ( --com == 0  )   com = tbl;
         }

         else if ( I < len  &&  E_8BITS( *C ) )
         {
            K = ( mostra_oito == 2 ) ? 3 : 2;
            for (   ;   K;   K-- )
            {
               J++;
               if ( --com == 0 )   com = tbl;
            }
         }

         J++;
         if ( --com == 0 )   com = tbl;
      }
   }

   vis = args->col - I;

   for ( L = 0, D = dest;   J > beg;   J--, vis++ )
   {
      L++;
      *D++ = cmv;
   }

   for (   ;   I < len && L < end;   I++, C++ )
   {
      if ( *C == TAB_CHAR )
      {
         if ( args->col > I )   vis += com - 1;
         for ( K = 0;   K < com;   K++ ) {  L++;  *D++ = cmv;  }
         com = tbl;
      }

      else if ( E_CTRL( *C ) )
      {
         *D++ = CTRL_CHAR;
         *D++ = *C + CTRL_VIS;

         for ( K = 0;   K < 2;   K++  ) {  if ( --com == 0 )   com = tbl;  }
         L  += K;
         if ( args->col > I )   vis += K - 1;
      }

      else if ( E_8BITS( *C ) )
      {
         SC  S[ 10 ];
         SS  T;
         if ( mostra_oito == 2 )
         {
            T = 4;
            sprintf( S, "%1.1s%3.3d", loc_str( STR_EDIT, EDIT_67), (UC) *C );
         }
         else
         {
            T = 3;
            sprintf( S, "%1.1s%2.2X", loc_str( STR_EDIT, EDIT_68), (UC) *C );
         }
         for ( K = 0;   K < T;   K++ )
         {
            *D++ = S[ K ];
            if ( --com == 0 )   com = tbl;
         }
         L  += K;
         if ( args->col > I )   vis += K - 1;
      }

      else
      {
         L++;
         *D++ = *C;
         if ( --com == 0 )   com = tbl;
      }
   }

   if ( args->clin == linha )   *col = vis;

   return ( L );
}

SS  mar_atr( args, L1, linha, len, posicao, qatr, qapos, esq, pri, dir )

ARG *args;
SC  *L1;
SS  linha, len, posicao;
SS  *qatr, *qapos, *esq, *pri, *dir;
{
   SC  *C;
   SS  I, form, qap, atr, mar_esq, mar_pri, mar_dir, mar;

   /*** Quantos atributos possui a linha original ***/
   for ( form = qap = atr = I = 0, C = L1;   I < len;   I++, C++ )
   {
      if ( form  ||  strchr( et_atr, *C ) != NULL )
      {
         if ( form )                      form--;
         else if ((UC) *C == ET_FORM1 )   form = TMF - 1;

         atr++;
         if ( I < posicao )   qap++;
      }
   }

   pega_margens( args, linha, &mar_esq, &mar_pri, &mar_dir );

   if ( prim_linha( args, linha ) )   mar_esq = mar_pri;

   mar = args->doc_mode + atr - mar_esq - mar_dir;

   mar = COTA( 10, mar, MAX_WRAP );

   if ( ! len  ||  (UC) L1[ len - 1 ] == PARA_CHR )   mar = len;

   else if ( ! et_just  &&  len > 1 )
   {
      for( I = 0, mar = len - 1;  mar  &&  L1[ mar ] == ' ';  mar-- )
         ;
      mar++;
   }

   *qatr  = atr;
   *qapos = qap;
   *esq   = mar_esq;
   *pri   = mar_pri;
   *dir   = mar_dir;

   return( mar );
}

/*** Devemos Pular os Atributos e Formatos ***/
/*** Direcao deve ser Comando + 256 ***/

VOID salta_atr( args, dir )

ARG  *args;
SS   dir;
{
   SS  A, C, I, J, K, D0, D1, D2, sai = 0;

   D1 = ESQUERDA + 256;
   D2 = TABANT   + 256;

   /*** Com isto evitamos que o cursor fique em ... ***/
   /*** cima de um Atributo ou Formato ***/

   while ( args->doc_mode )
   {
      I = args->clin;
      J = args->col;
      K = 0;

      /*** Se o caracter corrente for Atributo ou Formato ***/
      if ((A = e_et_atr( args, I, J, &C ))  !=  0 )
      {
         if ((UC) A == ET_FORM1  ||  (UC) A == ET_FORM2 )
         {
            /*** E' formato se for o final volte TMF caracteres. ***/
            if ((UC) A == ET_FORM2 ) {  K = TMF;   D0 = D1;  }

            /*** De qualquer maneira devemos sair do while() ***/
            sai++;
         }

         /*** Se for um Atributo comum mova apenas uma posicao ... ***/
         /*** ... na direcao de edicao. ***/
         else {  K = 1;   D0 = dir;  }
      }

      /*** O caracter corrente nao e' Atributo ou Formato ... ***/
      /*** ... vamos ver se o anterior e' Formato ***/
      else if ((UC)(A = e_et_atr( args, I, J - 1, NULL )) == ET_FORM1 ||
                                                    (UC)A == ET_FORM2   )
      {
         /*** De qualquer maneira salte o Formato, vamos ver a direcao ***/
         K = TMF;  D0 = 0;   sai++;

         /*** Se o corrente for Paragrafo Volte ***/
         if ((UC) C == PARA_CHR )   D0 = D1;
      }

      if ( K )
      {
         if ( D0 == D1 || D0 == D2 ) {  while( K-- )  C_left(  args );  }
         else                        {  while( K-- )  C_right( args );  }
      }

      if ( sai  ||  (I == args->clin  &&  J == args->col) )   break;
   }
}


/***
Devolve o Caracter da posicao indicada ou -1 caso nao exita caracter
***/
SS  char_posicao( args, linha, coluna )

ARG   *args;
SS    linha, coluna;
{
   SC   *L1;
   SS   C = -1;

   if ( linha  < args->t_lines  &&
        coluna < args->len[ linha ]  &&
        (L1 = vm_deref( args->ptr[ linha ], 0 )) != NULL )
   {
      C = (UC) L1[ coluna ];
   }

   return( C );
}


