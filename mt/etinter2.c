
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

/*** E' necessario no XENIX e 386IX ***/
#ifndef VAXVMS
SL       tell();
#endif

/**************************************************************************/
/* quite file */
SS quit_file( args )

ARG * args;
/**************************************************************************/
{
   SS ans;

   if ( args->f_mod == 0 )   goto END;

get_ans:
    ans = decida( loc_str( STR_EDIT, EDIT_13 ),
                  loc_str( STR_EDIT, EDIT_14 ), "", 3, 36,
                  "%s\n%s\n%s",
                  loc_str( STR_EDIT, EDIT_15 ),
                  loc_str( STR_EDIT, EDIT_16 ),
                  loc_str( STR_EDIT, EDIT_17 ) );

    if ( ans <= 0 )   return 0;

    if ( ans == 1  &&  ! save_file( args ) )   return 0;

END:

    return YES;
}

/***************************************************************************/
/* update file */
SS save_file( args )

ARG * args;
/***************************************************************************/
{
   SC   *L1;
   SS   lin, i, tam, ret = 0, saved = 1;
   SL   buf_p;
   FILE *out_stream;
   SC   out_file[ FILE_LEN + 1  ];
   SC   bu_file[  FILE_LEN + 10 ]; /*** Reserva para ".bak" ***/
   UC   *line;

   /*** NAO ESQUECER DO FREE( ) ***/

   line = (UC *) malloc( LINE_LEN + 20 );
   if ( line == NULL )    return 0;

   if ( args->tipo == ET_FILE )
   {
      out_file[ 0 ] = '\0';
      if ( args->file != NULL )    strcpy( out_file, args->file );

      for ( i = 0;   i < FILE_LEN  &&  out_file[ i ] != '.'   &&
                                       out_file[ i ] != '\0';   i++ )
      {
         bu_file[ i ] = out_file[ i ];
      }

      bu_file[ i ] = 0;

      strcat( bu_file, ".bak" );

      if ( access( out_file, 0 /* exist_mode */ ) == 0 )
      {
         if ( !args->f_saved )
         {
            unlink( bu_file );
            w_rename( out_file, bu_file );
         }
      }

      /*** Veja o if() testando erroexist != YES, mais abaixo ***/
      errorexists = NOT;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_2 ), out_file );

      if (( out_stream = fopen( out_file, MODO_WO ) ) == NULL )
      {
         errorexists = YES;
         mens_2( args->jm, loc_str( STR_EDIT, EDIT_511 ), out_file );
         w_rename( bu_file, out_file );
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

      out_stream = fdopen( handle, MODO_BWO );

      if ( out_stream == NULL )
      {
         debug( loc_str( STR_EDIT, EDIT_507 ) );
         goto fim;
      }

      fseek( out_stream, inicio, 0 );
   }

   else if ( args->tipo == ET_MEM )   args->tmp_ss = args->tmp_ss;

   else if ( args->tipo == ET_MEDIT )   out_stream = (FILE *) args->file;

   else   goto fim;

   for ( buf_p = 0, lin = 0;   lin < args->t_lines;   lin++ )
   {
      if ( args->tipo == ET_FILE || args->tipo == ET_MEDIT )
      {
         tam = args->len[ lin ];

         if ((L1 = vm_deref( args->ptr[ lin ], 0 )) == NULL )   tam = 0;

         memmove( line, (UC *) L1, tam );

         line[ tam ] = '\0';
         if ( !args->doc_mode && no_br_fim ) tam = tira_br_fim((SC *) line );

         line[ tam ] = '\0';
         if ( ! args->doc_mode && br_tab )   tam = br_to_tab( (SC *) line );

         line[ tam ] = '\n';
         line[ tam + 1 ] = '\0';

         fputs((SC *) line, out_stream );

         if ( ferror( out_stream ) )
         {
            switch ( args->tipo )
            {
            case ET_FILE : debug( loc_str( STR_EDIT, EDIT_512 ), out_file );
                           fclose( out_stream );
                           unlink( out_file );
                           w_rename( bu_file, out_file );
                           break;
            case ET_MEDIT: debug( loc_str( STR_EDIT, EDIT_509 ) );
                           break;
            }

            goto fim;
         }
      }

      else if ( args->tipo == ET_HANDLE )
      {
         if ( buf_p >= args->buf_len )   break;

         tam = args->len[ lin ];

         /*** Nao estoure a memoria e reserve espaco para delimitador ***/
         if ( tam + buf_p >= args->buf_len ) tam = args->buf_len - buf_p - 1;

         if ((L1 = vm_deref( args->ptr[ lin ], 0 )) == NULL )   tam = 0;

         memmove( line, L1, tam );
         buf_p += tam;

         line[ tam ] = '\n';
         line[ tam + 1 ] = '\0';
         buf_p++;

         fputs((SC *) line, out_stream );

         if ( ferror( out_stream ) )
         {
            debug( loc_str( STR_EDIT, EDIT_513 ) );
            fclose( out_stream );
            goto fim;
         }
      }

      else if ( args->tipo == ET_MEM )
      {
         if ( buf_p >= args->buf_len )   break;

         tam = args->len[ lin ];

         /*** Nao estoure a memoria e reserve espaco para delimitador ***/
         if ( tam + buf_p >= args->buf_len ) tam = args->buf_len - buf_p - 1;

         if ((L1 = vm_deref( args->ptr[ lin ], 0 )) == NULL )   tam = 0;

         memmove( &args->buffer[ (US) buf_p ], L1, tam );
         buf_p += tam;

         args->buffer[ (US) buf_p ] = delim;
         buf_p++;
      }
   }

   if ( args->tipo == ET_FILE   ||
        args->tipo == ET_HANDLE    )   fclose( out_stream );

   args->f_saved = 1;
   args->f_mod = 0;
   ret = saved;

fim:

   if ( args->tipo == ET_FILE  &&  errorexists != YES )
   {
      errorexists = YES;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_28 ), out_file );
   }

   free( line );
   return ret;
}

/***************************************************************************/
VOID C_import( args )                 /* import from a file */

ARG * args;
/***************************************************************************/
{
   SS    cmd, max_len, para_used = 0, longa_lin = 0, cur_len, a_ler;
   FILE  *inp_stream;
   SS    s_clin, clin, tam;
   SC    *L1, import_file[ FILE_LEN + 1 ];
   UC    *result;
   UC    *line, *tmp_line;

   a_ler   = ( args->doc_mode ) ? MAX_WRAP * 3 / 4 : LINE_LEN;
   max_len = ( args->doc_mode ) ? MAX_WRAP         : LINE_LEN;

   /*** NAO ESQUECER DO FREE( ); ***/

   line = (UC *) malloc( max_len + 50 );
   if ( line == NULL ) {  return;  }

   tmp_line = (UC *) malloc( max_len + 50 );
   if ( tmp_line == NULL ) {  free( line );  return;  }

   import_file[ 0 ] = '\0';

get_file:
   cmd = pede_cadeia( loc_str( STR_EDIT, EDIT_18 ), import_file, FILE_LEN,
                      loc_str( STR_EDIT, EDIT_19 ), 'x', NULL );

   insert_mode( args, insercao );

   if ( cmd == K_ESC )  goto fim;

   if ( import_file[ 0 ] == '\0' )   goto fim;

   if ( access( import_file, 4 /* ro_mode */ ) == -1 )
   {
      errorexists = YES;
      mens_2( args->jm, loc_str( STR_EDIT, EDIT_514 ), import_file );
      fwqgc( args->jm );
      goto get_file;
   }

   inp_stream = fopen( import_file, MODO_RO );

   if ( inp_stream == NULL )
   {
      debug( loc_str( STR_EDIT, EDIT_506 ), import_file );
      goto fim;
   }

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_20 ), import_file );

   s_clin = args->clin;

   C_blk_not( args );

#ifdef   ET_UNDO
   next_undo( args, s_clin, args->col );
#endif

   while ( 1 )
   {
      result = (UC *) fgets((SC *) line, a_ler, inp_stream );

      if ( ferror( inp_stream ) )
      {
erro_ler:
         debug( loc_str( STR_EDIT, EDIT_508 ), import_file );
         break;
      }

      if ( result == NULL )
      {
         if ( feof( inp_stream ) )   break;
         goto  erro_ler;
      }

      cur_len = strlen( (SC *) line );

      if ( args->doc_mode && ! longa_lin && cur_len > args->doc_mode + 10 )
      {
         longa_lin = YES;
      }

      cur_len = longa_para( args, line, cur_len, longa_lin, &para_used );

      cur_len = exp_tabs( args, line, tmp_line, max_len, cur_len, args->jm );

      if ( args->t_lines >= args->max_lines   &&
           ajusta_array( args, INC_LIN + args->t_lines ) == 0 )   goto fim;

      clin = args->clin;

      ins_linha( args, clin );

      linalloc( args, clin, args->len[ clin ], cur_len );

      if ((L1 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
      {
         memmove((UC *) L1, line, cur_len );
      }

#ifdef   ET_UNDO
      tam = cur_len;
      if ( L1 == NULL )   tam = 0;
      undo_linha( args, U_ENTRA, clin, tam, L1 );
#endif
      if ( args->admlin != NULL )  (*args->admlin)( args, clin, LIN_ENTRA );

      args->clin++;
   }

   fclose( inp_stream );

   if ( args->doc_mode )
   {
      if ( ! para_used )   ins_markers( args, s_clin );
      total_wrap( args, s_clin );
   }

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_29 ), import_file );

fim:

   /*** Garanta a reparagrafacao (Wrap) em Doc_Mode ***/
   if ( args->clin > args->t_lines - 1 )   args->clin = args->t_lines - 1;

   display = disp_file;
   linha_visivel( args );

   free( tmp_line );
   free( line );
}

/***************************************************************************/
VOID C_export( args )        /* export the text to a file */

ARG * args;
/***************************************************************************/
{
   SC    *L1;
   SS    cmd, exist_mode = 0, lin, tam, l_ini, l_fim;
   FILE  *out_stream;
   SC    export_file[ FILE_LEN + 1 ], *open_opt;
   UC    *line;

   /*** NAO ESQUECER DO FREE() ***/

   if ((line = (UC *) malloc( LINE_LEN + 20 )) == NULL )    return;

   if ( blk_tipo == BLK_NOT )
   {
      l_ini = 0;
      l_fim = args->t_lines -1;
   }

   else
   {
      l_ini = MIN( blk_li, blk_lf );
      l_fim = MAX( blk_li, blk_lf );
   }

   export_file[ 0 ] = '\0';

getfile:
   cmd = pede_cadeia( loc_str( STR_EDIT, EDIT_18 ), export_file, FILE_LEN,
                      loc_str( STR_EDIT, EDIT_21 ), 'x', NULL );

   insert_mode( args, insercao );

   if ( cmd == K_ESC )  goto fim;

   if ( export_file[ 0 ] == '\0' )   goto fim;

   open_opt = MODO_WO;

   if ( access( export_file, exist_mode ) == 0 )
   {
      cmd = decida( loc_str( STR_EDIT, EDIT_13 ),
                    loc_str( STR_EDIT, EDIT_22 ), "", 1, 40,
                    loc_str( STR_EDIT, EDIT_23 ), export_file );

      if ( cmd < 0   )   goto  getfile;
      if ( cmd == 1  )   open_opt = MODO_AO;
   }

   out_stream = fopen( export_file, open_opt );

   if ( out_stream == NULL )
   {
      errorexists = YES;
      mens_2( args->jm, loc_str( STR_EDIT, EDIT_515 ) );
      goto fim;
   }

   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_24 ), export_file );

   for ( lin = l_ini;   lin <= l_fim;   lin++ )
   {
      tam = args->len[ lin ];

      if ((L1 = vm_deref( args->ptr[ lin ], 0 )) == NULL )   tam = 0;

      memmove( line, (UC *) L1, tam );

      line[ tam ] = '\n';
      line[ tam + 1 ] = '\0';

      fputs( (SC *) line, out_stream );

      if ( ferror( out_stream ) )
      {
         mens_2( args->jm, loc_str( STR_EDIT, EDIT_516 ) );
         fclose( out_stream );
         goto fim;
      }
   }

   fclose( out_stream ); /* close the stream */
   errorexists = YES;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_30 ), export_file );

fim:
   free( line );
}

/***
Troca o primeiro grupo de brancos por TAB's
Retorna o tamanho da string
***/

SS   br_to_tab( buf )

SC   *buf;
{
   SS   I, J, K;
   SC   *P, *Q;

   /*** Cont os brancos iniciais ***/
   for ( I = 0, Q = P = buf;   *P == ' ';   I++, P++ )
      ;

   /*** Se so' tiver brancos, trunque a linha ***/
   if ( *P == '\0' ) {  buf[ 0 ] = '\0';   goto fim;  }

   if ( I < tab_width )   goto fim;

   /*** Ache quanto TABs sao necessarios ***/
   for ( J = 0, K = I / tab_width;   J < K;   J++, Q++ )   *Q = TAB_CHAR;

   /*** Complemente com BRANCOS necessarios ***/
   for ( J = 0, K = I % tab_width;   J < K;   J++, Q++ )   *Q = ' ';

   /*** Copie o restante ***/
   strcpy( Q, P );

fim:
   return ( strlen( buf ) );
}

/***
Tira brancos e TABs do final da linha e retorna o novo tamanho.
***/
SS   tira_br_fim( buf )

SC   *buf;
{
   SC   *P = buf;

   while ( *P != '\0' )  P++;   P--;

   while ( P >= buf  &&  (*P == ' ' || *P == TAB_CHAR))   P--;   P++;

   *P = '\0';

   return ( P - buf );
}

