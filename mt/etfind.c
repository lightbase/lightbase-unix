
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>

#include  "window.h"
#include  "mensagem.h"
#include  "entrada.h"
#include  "ethead.h"
#include  "etsr.h"

#include  MEMORY

/***
G - Global
K - Bloco
I - Ignorar Maiusculas / Minusculas
N - Substitui Sem Perguntar, porem apenas uma vez
C - Change All without questions
B - Para traz
R - Expressao Regular
S - Menor String para os metacaracteres * e +
***/

#define   OPT_LEN      12
#define   FIND_LEN     80

static  SR_CPAT  *cpat;                            /* The compiled pattern */
static  SC       busca_troca = 0;                  /* 1 = BUSCA, 2 = TROCA */
static  SC       *str_pos;                        /* Posicao se achou a RE */
static  SS       str_col;                         /* Posicao se achou a RE */
static  SC       searchre[] = "searchre";
static  SC       confrmre[] = "confrmre";
static  SC       flag_men;
static  SC       str_opt[ OPT_LEN + 1 ] = "";
static  SC       str_find[ FIND_LEN + 1 ] = "";
static  SC       str_subs[ FIND_LEN + 1 ] = "";
static  US       SuperBack;
static  UC       Icas, Glob, Chge, Nask, Back, blcK, Rexp, Shrt;
static  SS       from_col, from_line, to_line;   /* ATENCAO sempre SIGNED */

/**************************************************************************/
VOID avalia_opt( str )

SC *str;
{
   for ( Back=Icas=Glob=Chge=Nask=blcK=Rexp=Shrt=0;   *str != 0;   str++ )
   {
      switch ( toupper( *str ) )
      {
         case 'I':   Icas = 1;   break;
         case 'G':   Glob = 1;   break;
         case 'N':   Nask = 1;   break;
         case 'C':   Chge = 1;   break;
         case 'B':   Back = 1;   break;
         case 'K':   blcK = 1;   break;
         case 'R':   Rexp = 1;   break;
         case 'S':   Shrt = 1;   break;
      }
   }

   if ( (SuperBack & 0xFF00L) != 0 )   Back = SuperBack & 0x00FFL;
}

/**************************************************************************/
SS   pega_strings( args, bt, frente )

ARG  *args;
SS   bt, frente;
{
   SS   tecla, estado;
   SC   bak_opt[  OPT_LEN  + 1 ];
   SC   bak_find[ FIND_LEN + 1 ];
   SC   bak_subs[ FIND_LEN + 1 ];
   SC   *S1;

   strcpy( bak_opt,  str_opt );
   strcpy( bak_find, str_find );
   strcpy( bak_subs, str_subs );

   if ((S1 = strchr( str_opt, 'K' )) != NULL )   *S1 = ' ';

   S1 = strchr( str_opt, 'B' );

   if ( frente ) {  if ( S1 != NULL )   *S1 = ' ';  }

   else if ( S1 == NULL )
   {
      if ( strlen( str_opt ) < OPT_LEN )   strcat( str_opt, "B" );
   }

   if ( bt == BUSCA )   S1 = loc_str( STR_EDIT, EDIT_4 );
   else    /* TROCA */  S1 = loc_str( STR_EDIT, EDIT_3 );

   /*** Evita muitos refresh na janela ***/
   mens_1( args->jm, S1 );

   for( estado = 1;   estado < 100;    )
   {
      switch( estado )
      {
      default  :
      case    1:  /*** Pede sequencia a ser procurada ***/
                  tecla = pede_cadeia( loc_str( STR_EDIT, EDIT_6 ),
                                       str_find, FIND_LEN,
                                       S1,
                                       'x', NULL );

                  if (      tecla == K_ESC   )   estado = 100;
                  else if ( tecla == K_ENTER )   estado = 2;
                  else if ( tecla == K_TAB   )   estado = 3;
                  else if ( tecla == K_DN    )   estado = 2;
                  break;

      case    2:  /*** Pede sequencia para substituir ***/
                  if ( bt == BUSCA )
                  {
                     if (      tecla == K_ENTER )   estado = 200;
                     else if ( tecla == K_DN    )   estado = 3;
                     else if ( tecla == K_UP    )   estado = 1;
                     break;
                  }

                  tecla = pede_cadeia( loc_str( STR_EDIT, EDIT_7 ),
                                       str_subs, FIND_LEN,
                                       S1,
                                       'x', NULL );

                  if (      tecla == K_ESC   )   estado = 0;
                  else if ( tecla == K_ENTER )   estado = 200;
                  else if ( tecla == K_TAB   )   estado = 3;
                  else if ( tecla == K_DN    )   estado = 3;
                  else if ( tecla == K_UP    )   estado = 1;
                  break;

      case    3:  /*** Pede OPCOES ***/
                  tecla = pede_cadeia( loc_str( STR_EDIT, EDIT_8 ),
                                       str_opt, OPT_LEN,
                                       S1,
                                       'X', NULL );

                  if (      tecla == K_ESC   )   estado = 0;
                  else if ( tecla == K_ENTER )   estado = 200;
                  else if ( tecla == K_UP    )   estado = 2;
                  break;
      }
   }

   if ( estado == 100 )
   {
      strcpy( str_opt,  bak_opt );
      strcpy( str_find, bak_find );
      strcpy( str_subs, bak_subs );
      return( 0 );
   }

   insert_mode( args, insercao );

   return( 1 );
}

/**************************************************************************/
VOID set_limites( args, repete )

ARG *args;
SS repete;
{
   SS   clin, col, fim;

   clin = args->clin;
   col  = args->col;
   fim  = args->t_lines - 1;

   if ( repete  &&  ! new_sr  &&  ! SuperBack )
   {
      if ( Back ) {  from_col = col - 1;  to_line   = clin;  }
      else        {  from_col = col + 1;  from_line = clin;  }
   }

   else
   {
      if ( blcK )
      {
         if ( blk_tipo != BLK_NOT )
         {
            Back = Glob = 0;
            set_block( args );
            from_col = 0;  from_line = blk_li;  to_line = blk_lf;
            C_blk_not( args );
         }

         /*** Caso seja um (^L) nao faca nada ***/
      }

      else if ( Glob )
      {
         if ( Back )     from_col = args->len[ fim ];
         else            from_col = 0;

         from_line = 0;   to_line = fim;
      }

      else if ( Back ) { from_col = col; from_line = 0;    to_line = clin; }
      else             { from_col = col; from_line = clin; to_line = fim;  }
   }

   new_sr = 0;

   if ( SuperBack ) {  SuperBack = 0;  set_limites( args, repete );  }
}


/**************************************************************************/
VOID sr_manager( args, sr, repete, frente )

ARG  *args;
SS   sr;      /* BUSCA ou TROCA */
SS   repete;  /* Se repete -> NAO pega_strings() ***/
SS   frente;  /* Quando pega_strings() -> Propoe ou nao Opcao "B" */
/**************************************************************************/
{
   SC   *L1, *linha = NULL, *lin_rep = NULL;
   SS   tecla, TR, TS = 0, TU, TL;
   SI   TF; /* Tam maximo na entrada e tam real na saida sr_replace() */
   SS   clin, col, qtr = 0, mostra = 0, uma_vez = 0, achou = 0;
   SC   *ajuda_ant = win_ajuda;
   SC   s_find[ FIND_LEN + 1 ];

   cpat = NULL;

   win_ajuda = searchre;

   if ( sr != BUSCA  &&  sr != TROCA )  goto sai;

   if ( ! repete  &&  ! pega_strings( args, sr, frente ) )   goto sai;

   flag_men = 0;
   mens_1( args->jm, loc_str( STR_EDIT, EDIT_61 ) );

   avalia_opt( str_opt );

   if ( (linha   = malloc( LINE_LEN + 10 )) == NULL )   goto sai;
   if ( (lin_rep = malloc( LINE_LEN + 10 )) == NULL )   goto sai;

   busca_troca = sr;

   set_limites( args, repete );

   Sr_metaflag   = ! Rexp;
   Sr_shortmatch = Shrt;

   strcpy( s_find, str_find );

   if ( Icas )   strlwr( s_find );

   if ( Chge )
   {
      Nask = 1;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_5 ) );
      fwsetcursor( args->jm );
   }

   else if ( Nask )   uma_vez = 1;

   if ((cpat = sr_compile( s_find )) == NULL )
   {
      flag_men = 1;
      mens_1( args->jm, loc_str( STR_EDIT, EDIT_53 ) );
      goto sai;
   }

   col  = args->col;
   clin = args->clin;

   while ((TS = s_text( args, linha ))  >= 0 )
   {
      achou++;

#ifdef    ET_UNDO
      if ( clin != args->clin  ||  col != args->col )
      {
         next_undo( args, clin, col );
      }
#endif

      col  = args->col;
      clin = args->clin;

      if ( Back )
      {
         /*** Para Traz esta' correto sempre !!! ***/
         from_col = col - 1;
      }

      else
      {
         /*** Para Frente se nao houver troca esta' OK !***/
         /*** Porem se houver troca temos que incrementar ... ***/
         /*** ... o tamanho da substituicao para evitar procura ... ***/
         /*** ... no texto acabou de ser trocado. ***/
         from_col = col + 1;
      }

      if ( sr == BUSCA )   break;

      if ( ! Nask )
      {
         tecla = men_conf( loc_str( STR_EDIT, EDIT_10 ),
                           loc_str( STR_EDIT, EDIT_11 ),
                           confrmre, args->je );

         if ( tecla == K_ESC )   break;
         else if ( tecla == 2 )
         {
            Nask = mostra = 1;
            mens_1( args->jm, loc_str( STR_EDIT, EDIT_5 ) );
            fwsetcursor( args->jm );
         }
         else if ( tecla != 0 )   continue;
      }

      lin_rep[ 0 ] = '\0';

      TF = LINE_LEN;

      /*** TF indicara o limite maximo na entrada e retornara o ... ***/
      /*** tamanho final na saida. ***/
      /*** TR tera' o tamanho da substituicao efetuada. ***/

      if ((TR = sr_replace(linha,str_pos,TS,lin_rep,str_subs,&TF)) >= 0 )
      {
         qtr++;

         TL = args->len[ clin ];

#ifdef   ET_UNDO
         next_undo( args, clin, col );
         TU = TL;
         if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   TU = 0;
         undo_linha( args, U_ANTES, clin, TU, L1 );
#endif

         linalloc( args, clin, TL, TF );

         if ((L1 = vm_deref( args->ptr[ clin ], 0 )) != NULL )
         {
            memmove( L1, lin_rep, TF );
         }

         args->f_mod = 1;

         if ( TS == 0  &&  TR == 0 )   TR = 1;

         /*** Apenas se for para frente ***/
         if ( ! Back )   from_col = col + TR;

#ifdef   ET_UNDO
         TU = TF;
         if ((L1 = vm_deref( args->ptr[ clin ], 0 )) == NULL )   TU = 0;
         undo_linha( args, U_DEPOIS, clin, TU, L1 );
#endif

         if ( uma_vez )   goto  sai;

         if ( ! Nask  ||  mostra )
         {
            mostra = 0;
            display = disp_file;
            move_scr( args );
            fwrefresh( args->je );
         }
      }
   }

sai:

   win_ajuda = ajuda_ant;

   if ( cpat    != NULL )   sr_free_re( cpat );
   if ( lin_rep != NULL )   free( lin_rep );
   if ( linha   != NULL )   free( linha );

   errorexists = 1;

   if ( ! flag_men )
   {
      if ( TS  <  0  &&  ! qtr )
      {
         if ( Back )   mens_1( args->jm, loc_str( STR_EDIT, EDIT_60 ) );
         else          mens_1( args->jm, loc_str( STR_EDIT, EDIT_9 ) );
      }
      else if ( achou )
      {
         if ( sr == TROCA )
         {
            mens_1( args->jm, loc_str( STR_EDIT, EDIT_12 ), qtr );
         }
         else
         {
            mens_1( args->jm, loc_str( STR_EDIT, EDIT_54 ) );
         }
      }
      else   mens_1( args->jm, args->def_mens );
   }

   linha_visivel( args );
   display = disp_file;
}

/**************************************************************************/
SS s_text( args, linha )

ARG * args;
SC  *linha;
/**************************************************************************/
{
   SS  TS, I;

   if ( from_line <  0 )  from_line = 0;
   if ( to_line   <  0 )  to_line   = 0;

   if ( from_line >= args->t_lines )  from_line = args->t_lines - 1;
   if ( to_line   >= args->t_lines )  to_line   = args->t_lines - 1;

   if ( Back )  /*** Para Traz !!! ***/
   {
      for ( I = to_line;   to_line >= from_line;   to_line-- )
      {
         if ( I != to_line )   from_col = args->len[ to_line ] - 1;

         if ( from_col < 0 )   continue;

         if ( s_line( args, linha, to_line, 0, 1 ) >= 0  &&
                                           str_col <= from_col )
         {
            for (   ;   from_col >= 0;   from_col-- )
            {
               if ( s_line( args, linha, to_line, from_col, 1 ) >= 0 )
               {
                  if ( str_col > from_col )   continue;

                  return( s_line( args, linha, to_line, from_col, Nask ) );
               }
            }
         }
      }
   }

   else  /*** Para Frente !!! ***/
   {
      for ( I = from_line;   from_line <= to_line;   from_line++ )
      {
         if ( I != from_line )   from_col = 0;

         if ((TS = s_line( args, linha, from_line, from_col, Nask )) >= 0 )
            return TS;
      }
   }

   return( -1 );
}

/**************************************************************************/
SS s_line( args, linha, lin_num, col, no_ask )

ARG * args;
SC  *linha;
SS  lin_num;
SS  col;
SS  no_ask;
/**************************************************************************/
{
   SC  *L1;
   SS  I, TS, disp_len, clin, C;

   while ( 1 )
   {
      I = args->len[ lin_num ];

      if ( I == 0  ||  I <= col  ||  col < 0 )   return( -1 );

      if ((L1 = vm_deref( args->ptr[ lin_num ], 0 )) == NULL )  return( -1 );

      memmove( linha, L1, I );

      linha[ I ] = '\0';

      if ( Icas )   strlwr( linha );

      /*** Na entrada str_pos indicara a partir de onde havera' procura ***/
      str_pos = linha + col;

      if ((TS = sr_search( cpat, linha, &str_pos )) >= 0 && str_pos != NULL )
      {
         /*** Sinalize a posicao em caracteres ***/
         str_col = str_pos - linha;

         args->clin = clin = lin_num;

         if ( ! no_ask )
         {
            C = cur_aparente( args, clin, str_col );
            disp_len = cur_aparente( args, clin, str_col + TS ) - C;
            if ( disp_len <= 0 )   disp_len = 1;

            args->col = pos_coluna( args, clin, C );
            linha_visivel( args );
            display = disp_file;
            move_scr( args );

            fwsetatr( ATR_REVERSO, disp_len, args->je );
            fwrefresh( args->je );
         }

         args->col = str_col;

         if ( Icas )
         {
            if ((L1 = vm_deref( args->ptr[lin_num], 0 )) == NULL ) return -1;
            memmove( linha, L1, I );
            linha[ I ] = '\0';
         }

         return TS;
      }

      return( -1 );
   }
}

VOID  repita_busca( args )

ARG *args;
{
   repita_ft( args, ! Back );
}

VOID  repita_ft( args, frente )

ARG *args;
SS  frente;
{
   if ( frente )   frente = 0;
   else            frente = 1;

   if ( new_sr )   blcK = 0;

   SuperBack = 0xFF00L  |  frente;

   /*** O parametro "frente" nao vai ser usada, pois nao ... ***/
   /*** ... executara pega_strings() ***/
   sr_manager( args, busca_troca, 1, frente );
}






