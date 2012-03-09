
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#include  "window.h"
#include  "mensagem.h"
#include  "ethead.h"

#ifdef    VAXVMS
#include  <file.h>
#else
#include  <fcntl.h>
#endif

#include  PROTO

#ifdef    MSDOS
#include  <io.h>
#endif

#ifndef VAXVMS
/*** E' necessario nos XENIX e IX386 ***/
SL       tell();
#endif

/***
ARG   *load_buffer( args );
SS    edit_buffer( ARG * ); - Edita, Navega ou Mostra um Buffer.
SS    save_file( ARG * )  - Salva um buffer.
VOID  free_buffer( ARG * ); - Libera todas as areas do Buffer.
***/

ARG *load_buffer( doc_margem, tipo, origem, tamanho )

SS   doc_margem;
SS   tipo;
SC   *origem;
SL   tamanho;
{
   US  q_linhas;
   ARG *args;

   if ((args = (ARG *) malloc( sizeof( ARG ))) == NULL )   return( NULL );

   /*** Opcoes de carga ***/
   args->admlin = NULL;
   args->funcao = NULL;
   args->def_mens = args->escape = "";
   args->doc_mode = doc_margem;

   args->tipo = tipo;

   switch ( tipo )
   {
   case ET_MEDIT  : args->file   = (SC *) origem;   break;
   case ET_FILE   : args->file   =        origem;   break;
   case ET_MEM    : args->buffer =        origem;   break;
   case ET_HANDLE : args->handle = (SS)   origem;   break;
   }

   args->buf_len = tamanho;

   /*** Variaveis VINCULADAS ao BUFFER ***/

   /*** Forca calculo em src_number() ***/
   args->end_col = -1;
   args->beg_col = 0;

   args->beg_line = 0;
   args->row = args->col = args->clin = 0;
   args->alin = -1;  /* Force posicionamento na primeira chamada ET_APPEND */
   args->t_lines = 1;
   args->f_mod = 0;
   args->f_saved = 0;
   args->tecla = 0;
   args->je = NULL;
   args->jm = NULL;
   args->jzoom  = NULL;

   args->max_lines = 0;
   args->ptr = NULL;
   args->len = NULL;
#ifdef   VM_ARRAYS
   args->VMptr = 0;
   args->VMlen = 0;
#endif
#ifdef    ET_UNDO
   args->undo = NULL;
   args->FU   = NULL;
#endif

   switch ( tipo )
   {
   case ET_FILE   : q_linhas = LIN_INIC;            break;
   case ET_MEM    :
   case ET_MEDIT  :
   case ET_HANDLE : q_linhas = args->buf_len / 50;  break;
   }

   if ( q_linhas < LIN_INIC )   q_linhas = LIN_INIC;
   if ( q_linhas > MAX_LINE )   q_linhas = MAX_LINE;

   if ( ajusta_array( args, q_linhas ) == 0 )
   {
      free( (SC *) args );
      return( (ARG *) NULL );
   }

   /*** Comandos que devem ficar antes do read_file() ***/
   args->estado = 1;

   /*** Comandos que devem ficar antes do free_buffer ***/
   et_quant++;
#ifdef    ET_UNDO
   /* para evitar chamar funcao em overlay - DOS */
   if ( args->doc_mode ) {
      undo_on = 0;
      args->undo = NULL;
      args->redo = 0;
      args->FU   = NULL;
      args->off  = 0L;
      args->ant  = 0L;
      args->pos  = 0L;
      args->flg  = 0;
   }
   else
      start_undo( args );
#endif

   if ( read_file( args ) == NOT )
   {
      free_buffer( args );
      return ( NULL );
   }

   return ( args );
}

SS  lin_buffer( args, linha, dest )

ARG *args;
SS  linha;
SC  *dest;
{
   SS   I;
   UC  *P;

   if ( args == NULL  ||  linha >= args->t_lines )    return( -1 );

   I = args->len[ linha ];
   P = (UC *) vm_deref( args->ptr[ linha ], 0 );

   if ( P == NULL )   I = 0;

   if ( I != 0 )   memmove( dest, P, I );

   dest[ I ] = '\0';

   return ( I );
}


SS  add_linbuffer( args, linha )

ARG *args;
SC  *linha;
{
   UC   tmp_line[ MAX_WRAP + 10 ];
   SS   max_len, cur_len, ret, para_used = YES, longa_lin = 0;

   max_len = MAX_WRAP;

   cur_len = strlen( (SC *) linha );

   if ( cur_len > MAX_WRAP )   cur_len = MAX_WRAP;

   ret = lin_no_final( args, (UC *)linha, tmp_line,
                       max_len, cur_len, longa_lin, &para_used, NULL );

   return( ret );
}


/***************************************************************************/
/* read the input file into memory */
SS read_file( args )

ARG * args;
/***************************************************************************/
{
   SS   i, ret, max_len, para_used = 0, longa_lin = 0, cur_len, a_ler;
   SL   buf_p;
   FILE *inp_stream;
   UC   *result;
   UC   *line, *tmp_line;
   WIN  *JM = NULL;   /* Para ET_FILE, janela do buffer ainda nao existe */

   ret = NOT; /* Not_Opened */

   a_ler   = ( args->doc_mode ) ? MAX_WRAP * 3 / 4 : LINE_LEN;
   max_len = ( args->doc_mode ) ? MAX_WRAP : LINE_LEN;

   /*** NAO ESQUECER DO FREE( ); ***/

   line = (UC *) malloc( max_len + 50 );
   if ( line == NULL )   return ret;

   tmp_line = (UC *) malloc( max_len + 50 );
   if ( tmp_line == NULL ) {  free( line );  return ret;  }

   if ( args->tipo == ET_FILE )
   {
      JM = abre_mensagem( "" );
   }

   if ( args->tipo == ET_FILE )
   {
      if ( args->file == NULL  ||  *args->file == '\0' )  goto fim;

      if ( access( args->file, 4 /* ro_mode */ ) == -1 )
      {
         errorexists = YES;
         mens_1( JM, loc_str( STR_EDIT, EDIT_505 ), args->file );
         ret = YES; /*** Arquivo nao existe ***/
         goto fim;
      }

      inp_stream = fopen( args->file, MODO_RO );

      if ( inp_stream == NULL )
      {
         debug( loc_str( STR_EDIT, EDIT_506 ), args->file);
         goto fim;
      }
   }

   else if ( args->tipo == ET_HANDLE )
   {
      SS   handle;
      SL   inicio;

      if ( args->handle < 0  ||
          (handle = dup( args->handle )) < 0  ||
          (inicio = tell( handle )) < 0 )  goto  fim;

      inp_stream = fdopen( handle, MODO_BRO );

      if ( inp_stream == NULL )
      {
         debug( loc_str( STR_EDIT, EDIT_507 ) );
         goto fim;
      }

      fseek( inp_stream, inicio, 0 );
   }

   else if ( args->tipo == ET_MEM )     args->tmp_ss = args->tmp_ss;

   else if ( args->tipo == ET_MEDIT )   inp_stream = (FILE *) args->file;

   else   goto fim;

   /*** Se e' a primeira vez que carrega decremente "t_lines" ... ***/
   /*** ... lin_no_final() incrementa o "t_lines". ***/
   if ( args->estado == 1 )   args->t_lines = 0;
   buf_p = 0;

   ret = YES;

   while ( 1 )
   {
      if ( args->tipo == ET_FILE   ||
           args->tipo == ET_HANDLE ||
           args->tipo == ET_MEDIT   )
      {
         /*** No tipo ET_HANDLE, vamos incrementar "buf_p", mais embaixo ***/
         if ( args->tipo == ET_HANDLE && buf_p >= args->buf_len )  break;

         result = (UC *) fgets((SC *) line, a_ler, inp_stream );

         if ( ferror( inp_stream ) )
         {
            switch ( args->tipo )
            {
            case  ET_FILE   : debug( loc_str( STR_EDIT, EDIT_508 ) ); break;
            case  ET_HANDLE : debug( loc_str( STR_EDIT, EDIT_510 ) ); break;
            case  ET_MEDIT  : debug( loc_str( STR_EDIT, EDIT_523 ) ); break;
            }

            break;
         }

         if ( result == NULL )   break;
      }

      else  /*** args->tipo == ET_MEM ***/
      {
         if ( buf_p >= args->buf_len )   break;

         for ( i = 0;   buf_p < args->buf_len  &&
                        args->buffer[ (US) buf_p ] != delim  &&
                        i < max_len;   i++, buf_p++ )
         {
            line[ i ] = args->buffer[ (US) buf_p ];
         }

         /* to simulate a read from disk, add a new line character */
         line[ i ] = 10;   i++;   buf_p++;   line[ i ] = 0;
      }

      cur_len = strlen((SC *) line );

      if ( args->tipo == ET_FILE  &&  args->doc_mode &&
           ! longa_lin  &&  cur_len > args->doc_mode + 10 )
      {
         longa_lin = YES;
      }

      if ( args->tipo == ET_HANDLE )
      {
         buf_p += cur_len;

         if ( buf_p > args->buf_len )
         {
            cur_len -= (buf_p - args->buf_len);
            if ( cur_len <= 0 )   break;
         }
      }

      if ( lin_no_final( args, line, tmp_line,  max_len, cur_len,
                         longa_lin, &para_used, JM ) != YES )
      {
         ret = NOT;
         break;
      }

      if ( args->tipo == ET_FILE  &&  ! (args->t_lines % 50) )
      {
         errorexists = YES;
         mens_1( JM, loc_str( STR_EDIT,  EDIT_1 ),
                 args->file, args->t_lines );
         fwsetcursor( JM );
      }
   }

   if ( args->tipo == ET_FILE   ||
        args->tipo == ET_HANDLE   )   fclose( inp_stream );

   if ( args->t_lines == 0 )  args->t_lines = 1;

   if ( ret == YES  &&  args->doc_mode )
   {
      if ( !para_used )   ins_markers( args, 0 );
      total_wrap( args, 0 );
   }

   args->f_mod = 0;
   args->clin = args->row = args->col = 0;

fim:
   free( tmp_line );
   free( line );
   if ( JM != NULL )   fecha_mensagem( JM );
   return ret;
}


/***************************************************************************/
/***
Coloca linha no final do buffer, primitiva usada em read_file() e C_import().
***/

SS  lin_no_final( args, line, tmp_line,
                  max_len, cur_len, longa_lin, para_used, JM )

ARG * args;
UC *line, *tmp_line;
SS max_len, cur_len, longa_lin, *para_used;
WIN *JM;
/***************************************************************************/
{
   SC  *L1;
   SS   i;

   cur_len = longa_para( args, line, cur_len, longa_lin, para_used );

   cur_len = exp_tabs( args, line, tmp_line, max_len, cur_len, JM );

   args->t_lines++;

   if ( args->t_lines >= args->max_lines   &&
        ajusta_array( args, INC_LIN + args->t_lines ) == 0 )
   {
erro:
      args->t_lines--;
      errorexists = YES;
      if ( JM != NULL )
      {
         mens_1( JM, loc_str( STR_EDIT, EDIT_518 ) );
         win_bell( BEL_GEN );
         fwqgc( JM );
      }
      return( NOT );
   }

   args->clin = args->t_lines - 1;

   /*** Simule um linalloc(), pois end_logic() nao pode ser chamada ***/
   i = args->clin;
   args->f_mod = YES;
   args->ptr[ i ] = NULL;
   args->len[ i ] = 0;
   if ( cur_len > 0 )
   {
      if ((args->ptr[ i ] = (PTR) vm_alloc( cur_len )) == NULL )   goto erro;

      if ((L1 = vm_deref( args->ptr[ i ], 0 )) == NULL )  goto erro;

      args->len[ i ] = cur_len;

      memmove((UC *) L1, line, cur_len );
   }

   return ( YES );
}

/***************************************************************************/
/***
Retorna o numero da primeira linha do paragrafo indicado, porem em ...
modo TEXTO retorna "paragrafo".
O primeiro PARAGRAFO e a primeira LINHA retornada e' ZERO.
***/
SS acha_paragrafo( args, paragrafo )

ARG *args;
SS paragrafo;
/***************************************************************************/
{
   SC  *L1;
   SS  I, para, linha;

   if ( !args->doc_mode )   return( paragrafo );

   for ( para = linha = 0;   para  < paragrafo  &&
                             linha < args->t_lines;   linha++ )
   {
      I = args->len[ linha ];
      if ( I == 0  ||  args->ptr[ linha ] == NULL )    break;

      if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   break;

      if ( (UC) L1[ I - 1 ] == PARA_CHR )   para++;
   }

   return ( linha );
}

VOID ajuda_editor( )
{
   et_ajuda = "editor";
}

VOID et_primeiro( )
{
   SS   I;

   lin_col = disp_stat;  /* Display Linha Coluna padrao */

   blk_inifim = 0; /* Segundo INIBLOCO = COPIABLOCO. Norton editor */

   ajuda_editor();

   /*** Controle de quantos editores abertos existem ***/
   et_quant = 0;

   /* Atributos de impressao */
   et_atr[ 0 ] = ET_NORMAL;
   et_atr[ 1 ] = ET_SUBLIN;
   et_atr[ 2 ] = ET_ENFAT;
   et_atr[ 3 ] = ET_ITAL;
   et_atr[ 4 ] = ET_FORM1;
   et_atr[ 5 ] = ET_FORM2;
   et_atr[ 6 ] = '\0';   /* Cuidado Pode Ir Ate 9 */

   /*** Inicializa funcoes de comandos multiplos ***/
   f_key_func = NULL; /* Nao existe funcao que escreve na linha de funcao */
   def_key_func = str_vazia;
   for ( I = 0;   I < MAX_MULTKEY;   I++ )
   {
      key_func[ I ].tecla  = 0;
      key_func[ I ].funcao = NULL;
   }

   /*** Inicializa macros ***/
   for ( I = 0;   I < MAX_MACROS;   I++ )   mac_files[ I ] = NULL;

   /*** Inicializa arquivos temporarios ***/
   scrap_inic( );

   /*** Inicializa comandos do teclado, DEFAULT ***/
   carga_coms( NULL, NULL, NULL, 0, NULL, 0 );

   /*** Configuracao basica ***/
   scroll_pagina = 1;
   margin = 1;
   tab_width = 8;
   tab_ug_sp = 0; /* Mostra os TABs e Justificacao como UG_SP / Branco */
   tab_tab = 1;   /* Tab e' TAB ou trocado por brancos */
   cr_new_line = 1;
   tab_align   = 1;
   join_lines  = 1;
   et_just = 1;
   no_br_fim = 1;  /* Quando salva tira Brancos e TABs do Final das linhas */
   mostra_oito = 0; /* Os Caracteres >= 128 serao mostrados como caracter */
   br_tab = 0;     /* Ao salvar os primeiros brancos serao trocados por TABs */
   flag_medit = 3; /* Nivel de confirmacao maximo para o MEDIT */
}

VOID et_ultimo( )
{
   SS  I;

   /*** Unlink os arquivos temporarios ***/
   scrap_unlink( );

   /*** Libere area de nomes de arquivos temporarios ***/
   scrap_free( );

   /*** Libere Macros ***/
   for ( I = 0;   mac_files[ I ] != NULL;   I++ )   free( mac_files[ I ] );

   /*** Libere Funcoes de comandos multiplos ***/
   for ( I = 0;   I < MAX_MULTKEY;   I++ )
   {
      if ( key_func[ I ].funcao != NULL )   free( key_func[ I ].funcao );
   }
}

/*** Expande TABs em uma linha, Retorna o tamanho atual da linha ***/

SS  exp_tabs( args, line, tmp_line, max_len, cur_len, JM )

ARG *args;
UC  *line, *tmp_line;
SS   max_len, cur_len;
WIN *JM;
{
   SS   i, j, tmp_len, tab_len;

   if ( !args->doc_mode || strchr((SC *)line,TAB_CHAR) == NULL )
      return( cur_len );

   tmp_len = 0;

   for ( i = 0;   i < cur_len;   i++ )
   {
      if ( line[ i ] == TAB_CHAR )
      {
         tab_len = tmp_len / tab_width;
         tab_len = (tab_len + 1) * tab_width - tmp_len;

         for ( j = 0;   j < tab_len;   j++ )   tmp_line[ tmp_len++ ] = ' ';
      }

      else   tmp_line[ tmp_len++ ] = line[ i ];

      if ( tmp_len >= (max_len - 1) )
      {
         errorexists = YES;
         mens_1( JM, loc_str( STR_EDIT, EDIT_517 ) );
         break;
      }
   }

   tmp_line[ tmp_len ] = 0;

   strcpy((SC *) line, (SC *) tmp_line );

   return ( tmp_len );
}

/*** Poe paragrafo em linhas longas ***/

SS   longa_para( args, line, cur_len, longa_lin, para_used )

ARG *args;
UC  *line;
SS   cur_len, longa_lin, *para_used;
{
   args = args;

   if ( ! *para_used  &&  strchr((SC *) line, PARA_CHR ) != NULL )
   {
      *para_used = 2;
   }

   if ( cur_len  &&  line[ cur_len - 1 ] == '\n' )
   {
      if ( args->doc_mode  &&  ! *para_used  &&  longa_lin )
      {
         line[ cur_len - 1 ] = PARA_CHR;
      }

      else   cur_len--;
   }

   return ( cur_len );
}


