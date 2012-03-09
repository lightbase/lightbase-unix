
#include  <stdio.h>
#include  <stdlib.h>

#include  "window.h"
#include  "ethead.h"

#include  MEMORY

#define   W_SAI        0 /* Saia ou incremente a linha no loop principal */
#define   W_INCR       1 /* Apenas incremente a linha no loop principal */
#define   W_FICA       2 /* Fica na mesma linha no loop principal */

#define   W_NOTINS     0 /* NAO forca insercao de linha em to_next() */
#define   W_INSERE     1 /* Forca insercao de linha em to_next() */

static    SS   q_atr;    /* Quant. Atributos na linha - Auxiliar */
static    SS   q_car;    /* Quant. Caracteres na linha - Auxiliar */

#ifdef     MSDOS

ST( SS )   wrap_longas( ARG *, US );
ST( SS )   wrap_curtas( ARG *, US );
ST( VOID ) insere_linha( ARG *, US );
ST( VOID ) retira_linha( ARG *, US );
ST( VOID ) from_next( ARG *, US, SS );
ST( VOID ) to_next( ARG *, US, SS, SS );
ST( VOID ) junta_proxima( ARG *, US );
ST( VOID ) ini_brancos( ARG *, US );
ST( SS )   novo_para( ARG *, US );
ST( VOID ) quebra_longas( ARG *, US );
ST( VOID ) coloca_paragrafo( ARG *, US );
ST( VOID ) marca_video( ARG *, US );

#else

ST( SS )   wrap_longas( );
ST( SS )   wrap_curtas( );
ST( VOID ) insere_linha( );
ST( VOID ) retira_linha( );
ST( VOID ) from_next( );
ST( VOID ) to_next( );
ST( VOID ) junta_proxima( );
ST( VOID ) ini_brancos( );
ST( SS )   novo_para( );
ST( VOID ) quebra_longas( );
ST( VOID ) coloca_paragrafo( );
ST( VOID ) marca_video( );

#endif

US  text_wrap( args, linha )

ARG * args;
US linha;
{
   SC   *L1;
   SS   I, parag, flag, seg = 0;
   US   ini_lin = linha;
   SS   mar_tot;  /* Margem Total */
   SS   mar_esq;  /* Deslocamento da Margem Esquerda do Paragrafo */
   SS   mar_pri;  /* Deslocamento da Margem Esquerda da Primeira Linha */
   SS   mar_dir;  /* Deslocamento da Margem Direita */
   SS   esq;      /* mar_esq OU mar_pri */

   /*** Ajuste de inicio os paramentros default de atualizacao ***/
   l_inicio = l_fim = args->row;

   /*** Se linha for a ultima e nao contiver um paragrafo, coloque ***/
   coloca_paragrafo( args, linha );

   /*** Se a linha anterior pertencer ao paragrafo, comece por ela ***/
   if ( ! prim_linha( args, linha ) )   linha--;

   /*** O wrap tem que reservar espaco para um espaco ou PARA_CHR ***/
   /*** Garantir o wrap minimo e maximo ***/
   I = args->doc_mode + 1;
   mar_tot = COTA( 10, I, MAX_WRAP );

   /*** Ache as margens ***/
   pega_margens( args, linha, &mar_esq, &mar_pri, &mar_dir );

   while ( 1 )
   {
      /*** Quebra linhas muito longas, porem assumimos que ... ***/
      /*** ... nao existem linhas maiores que MAX_WRAP... ***/
      /*** ... no read_file() ja esta OK, porem falta garantir ... ***/
      /*** ... nas copias de BLOCOS !!! ***/
      quebra_longas( args, linha );

      retira_linha( args, linha );

      /*** Se mesmo depois de retira_linha( ), estiver vazia, encerre ***/
      I = args->len[ linha ];
      if ( I == 0  ||  args->ptr[ linha ] == NULL )   break;

      /*** Se existe uma marca de paragrafo na linha, quebre um ... ***/
      /*** ... novo paragrafo. ***/
      parag = novo_para( args, linha );

      /*** Os brancos iniciais vao para cima. ***/
      ini_brancos( args, linha );

      /*** Se a linha estiver vazia, encerre ***/
      I = args->len[ linha ];
      if ( I == 0  ||  args->ptr[ linha ] == NULL )   break;

      q_atr = q_atributos( args, linha, mar_tot, 0 );

      /*** Se for a primeira linha do paragrafo possui margem propria ***/
      if ( prim_linha( args, linha ) )   esq = mar_pri;
      else                               esq = mar_esq;

      q_car = mar_tot - esq - mar_dir + q_atr;

      if ( I < q_car )   flag = wrap_curtas( args, linha );

      else               flag = wrap_longas( args, linha );

      /*** Se esta linha nao deu wrap e a esta linha e' maior ou ... ***/
      /*** ... igual a linha inicial, nao tem por que continuar. ***/
      if ( parag == 0  &&  flag == W_SAI  &&  linha >= ini_lin )   break;

      /*** Se for a ultima linha, encerre ***/
      if ( linha >= (args->t_lines - 1) )   break;

      /*** Se linha estiver vazia, encerre ***/
      I = args->len[ linha ];
      if ( I == 0  ||  args->ptr[ linha ] == NULL )   break;

      /*** Se linha for a ultima linha do paragrafo encerre ***/

      if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   break;
      if ( parag == 0  &&  (UC) L1[ I - 1 ] == PARA_CHR )   break;

      /*** Se ficar 15 vezes na mesma linha entrou em loop, encerre !!! ***/
      if ( seg++ == 15 )   break;

      /*** Incrementa WLINHA somente aqui !!! ***/
      if ( flag != W_FICA ) {  ++linha;  seg = 0;  }
   }

   /*** Retorne a proxima linha em que parou ***/
   if ( linha <= ini_lin )   linha = ini_lin + 1;

   return ( linha );
}


ST( SS ) wrap_curtas( args, linha )

ARG * args;
US  linha;
{
   SC   *L1;
   SS   ret = W_SAI;
   register   SS   I, J, K;
   register   UC   *P;
   register   US   next;

   /*** Se for a ultima linha ou se a linha for paragrafo, encerre ***/
   I = args->len[ linha ];
   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return( ret );
   P = (UC *) &L1[ I - 1 ];
   if ( *P == PARA_CHR )   return( ret );

   if ( *P != ' '  ||  I >= q_car )
   {
      /*** Para o caso de nao haver brancos no final da linha ***/
      junta_proxima( args, linha );
      return( wrap_longas( args, linha ) );
   }

   /*** Ainda cabe alguma coisa nesta linha ? ***/
   if ( I >= q_car - 1 )   return( ret );

   /*** Calcule quanto ainda cabe nesta linha ***/
   J = q_car - I;

   /*** Se a proxima linha NAO estiver alocada, encerre ... ***/
   /*** ... pois devera ser o final. ***/
   next = linha + 1;
   I = args->len[ next ];
   if ( I == 0  ||  args->ptr[ next ] == NULL )   return( ret );

   /*** Do limite, procure o 1o. branco a margem ideal ***/
   if ( J > I )   J = I;

   if ((L1 = vm_deref( args->ptr[ next ], 0 )) == NULL )   return( ret );
   P = (UC *) &L1[J - 1];

   if ( *P == PARA_CHR )   K = I;

   else
   {
/*** jairo ultimo parametro era 1 ***/
      J += q_atributos( args, next, J, 0 );

      if ( J > I )   J = I;

      if ((L1 = vm_deref( args->ptr[ next ], 0 )) == NULL )  return( ret );
      P = (UC *) &L1[J - 1];

      if ( *P == ' ' )
      {
         /*** Procure para FRENTE o 1o. NAO branco ***/
         for ( K = J;   K <= I  &&  *P == ' ';   K++, P++ ) /* Nada */ ;
         K--;  P--;
      }

      else if ( *P == PARA_CHR )   K = I;

      else
      {
         /*** Procure para TRAZ o 1o. branco ***/
         /*** Se a 1a. palavra da posterior nao cabe nesta, encerre ***/
         for ( K  = J;   K > 0  &&  *P != ' ';   K--, P-- ) /* Nada */;
         if (  K == 0 )  return( ret );
      }
   }

   /*** Se aproveitou a proxima linha inteira, precisamos ficar ... ***/
   /*** ... nesta linha para tratar corretamente muitas linhas pequenas. ***/
   if ( K >= I )   ret = W_FICA;
   else            ret = W_INCR;

   from_next( args, linha, K );

   return ( ret );
}


ST( VOID ) from_next( args, linha, quant )

ARG * args;
US  linha;
SS  quant;  /*** Quantos caracteres serao retirados da linha posterior ***/
{
   SC   *L1;
   SS   I, J, K;
   US   next;
   UC   str_res[ (MAX_WRAP * 2) + 10 ];

   if ( display != disp_file )  display = disp_line;

   K    = quant;
   next = linha + 1;

   /*** Sera que precisa ajustar o cursor ***/
   if ( args->clin == next )
   {
      if ( args->col < K )
      {
         args->col += args->len[ linha ];
         linha_anterior( args );
      }

      /*** Fique nesta linha porem volte um pouco ***/
      else {  args->col -= K;  }
   }

   /*** Retire os caracteres da proxima linha ***/
   args->f_mod = YES;
   I = args->len[ next ];
   J = I - K;
   if ((L1 = vm_deref( args->ptr[ next ], 0 )) == NULL )   return;
   memmove( str_res, (UC *) L1, I );

   /*** Realoque a linha ***/
   linalloc( args, next, I, J );
   marca_video( args, next );

   /*** Copie ou retire a linha ***/
   if ( J )
   {
      if ((L1 = vm_deref( args->ptr[ next ], 0 )) == NULL )   return;
      memmove((UC *) L1, str_res + K, J );
   }
   else       retira_linha( args, next );

   /*** E coloque no final desta ***/

   I = args->len[ linha ];
   linalloc( args, linha, I, I + K );
   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return;
   memmove((UC *) &L1[ I ], str_res, K );
   marca_video( args, linha );
}


ST( SS ) wrap_longas( args, linha )

ARG * args;
US   linha;
{
   SC   *L1;
   SS   ret = W_SAI;
   register   SS   I, J;
   register   UC   *P;

   I = args->len[ linha ];
   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return( ret );
   P = (UC *) &L1[ I - 1 ];

   /*** Se a linha for menor que margem ou ja estiver ajustada, OK ! ***/
   if ( I <  q_car ||
       (I == q_car && (*P == ' ' || *P == PARA_CHR)))   return( ret );

   /*** Da margem para tras procure o 1o. branco ***/
   for ( J = q_car, P = (UC *) &L1[ J - 1 ];
         J > 0   &&  *P != ' ';   J--, P-- ) /*** Virgula ***/ ;

   /*** De onde parou, para frente, procure o 1o. NAO branco ***/
   if ( J == 0 ) {  J = q_car;  P = (UC *) &L1[ J - 1 ];  }

   else  {  while ( *P == ' '  &&  J <= I ) {  P++;  J++;  }  }

   /*** Se nao existir um caracter diferente de branco, OK ! ***/
   if ( J  >  I + q_atr )
   {
      /*** Retire brancos excedentes da linha ***/
      if ( I > q_car + 5 )   linalloc( args, linha, I, q_car + 5 );

      return( ret );
   }

   /*** Calcule resto, guarde e reajuste a linha ***/
   J--;

   to_next( args, linha, J, W_NOTINS );

   return ( W_INCR );
}


ST( VOID ) to_next( args, linha, tam, insere )

ARG * args;
US  linha;
SS  tam;     /*** Novo tamanho da linha, passe o excedente para baixo ***/
SS  insere;  /*** Forca insercao de linha ***/
{
   SC   *L1;
   SS   I, J, K;
   US   next;
   UC   *P, str_res[ (MAX_WRAP * 2) + 10 ];

   if ( display != disp_file )  display = disp_line;

   args->f_mod = YES;
   J = tam;
   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return;
   P = (UC *) &L1[ J ];
   I = args->len[ linha ];
   K = I - J;
   memmove( str_res, P, K );
   linalloc( args, linha, I, J );
   marca_video( args, linha );

   /*** Chegou no limite de linhas ? ***/
   if ( args->t_lines  >=  args->max_lines   &&
        ajusta_array( args, INC_LIN + args->t_lines ) == 0 )
   {
      /*** Linha truncada, veja se o cursor nao fica fora da linha ***/
      if ( args->clin == linha  &&  args->col >= J )   args->col = J - 1;
      return;
   }

   /*** Passe o resto para baixo ***/
   /*** Sera' a ultima linha do texto ? ***/
   if ( (next = linha + 1)  >=  args->t_lines )
   {
      args->t_lines++;
      linalloc( args, next, 0, K );
      if ((L1 = vm_deref( args->ptr[ next ], 0 )) == NULL )   return;
      memmove((UC *) L1, str_res, K );
   }

   /*** Sera' que precisamos inserir linha ou ... ***/
   /*** ... que este resto nao e' fim de paragrafo ? ***/
   else if ( insere  ||  str_res[ K - 1 ] == PARA_CHR )
   {
      insere_linha( args, next );
      linalloc( args, next, 0, K );
      if ((L1 = vm_deref( args->ptr[ next ], 0 )) == NULL )   return;
      memmove((UC *) L1, str_res, K );
   }

   /*** Bem, coloque o resto no comeco da proxima linha ***/
   else
   {
      I = args->len[ next ];
      if ( I )
      {
         if ((L1 = vm_deref( args->ptr[ next ], 0 )) == NULL )   return;
         memmove( str_res + K, (UC *) L1, I );
      }
      linalloc( args, next, I, I + K );
      if ((L1 = vm_deref( args->ptr[ next ], 0 )) == NULL )   return;
      memmove((UC *) L1, str_res, I + K );
   }

   /*** Sera que precisa ajustar o cursor ***/
   if ( args->clin == next  )   args->col += K;

   else if ( args->clin == linha  )
   {
      if ( args->col >= J )
      {
         /*** Vai para BAIXO ***/
         args->col -= J;
         if ( scroll_pagina ) {  args->clin++;  linha_visivel( args );  }
         else                    linha_posterior( args );
      }
   }

   marca_video( args, next );
}


ST( VOID ) retira_linha( args, linha )

ARG * args;
US   linha;
{
   register  SS  I;

   /*** Ache a proxima linha valida ***/
   while ( 1 )
   {
      /*** Se for a ultima linha, OK encerre ***/
      if ( linha >= args->t_lines )  return;

      /*** Se a proxima linha NAO estiver alocada ou ... ***/
      /*** ... estiver vazia, retire-a do arranjo. ***/
      I = args->len[ linha ];

      if ( I > 0   &&  args->ptr[ linha ] != NULL )   return;

      else
      {
         /*** Se a linha estiver visivel atualize ate o final do video ***/
         if ( display != disp_file  &&
              args->je != NULL      &&
              linha  <=  args->beg_line + (args->je->w_height - 1)  &&
              linha  >=  args->beg_line )  l_fim = args->je->w_height - 1;

         ret_linha( args, linha );
      }

      /*** Teste proxima Linha ***/
      linha++;
   }
}


ST( VOID ) insere_linha( args, linha )

ARG * args;
US   linha;
{
   /*** Se a linha estiver visivel atualize ate o final do video ***/
   if ( display != disp_file  &&
        args->je != NULL      &&
        linha  <=  args->beg_line + (args->je->w_height - 1)  &&
        linha  >=  args->beg_line )  l_fim = args->je->w_height - 1;

   ins_linha( args, linha );
}

ST( VOID ) junta_proxima( args, linha )

ARG * args;
US linha;
{
   SC  *L1;
   register   SS   I, J, K;
   register   UC   *P;
   register   US   next;

   while ( 1 )
   {
      /*** Se for a ultima linha ou se a linha for paragrafo, encerre ***/
      I = args->len[ linha ];
      if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return;
      P = (UC *) &L1[ I - 1 ];

      /*** Ja chegou ao limite ***/
      if ( I >  q_car  ||  *P == PARA_CHR  ||
         ( I == q_car  &&  *P == ' ' )       )  return;

      /*** Calcule quanto ainda cabe nesta linha, precisa estourar !!! ***/
      J = (q_car - I) + 1;

      /*** Se a proxima linha NAO estiver alocada, encerre ... ***/
      /*** ... pois devera ser o final. ***/
      next = linha + 1;
      I = args->len[ next ];
      if ( I == 0  ||  args->ptr[ next ] == NULL )   return;

      /*** Calcule a melhor quantidade ***/
      if ( J >= I )   K = I;
      else            K = J;

      from_next( args, linha, K );
   }
}


ST( VOID ) ini_brancos( args, linha )

ARG * args;
US  linha;
{
   SC   *L1;
   SS   I, J;
   US   prev;
   UC   *P;

   /*** Se a linha anterior NAO pertencer ao paragrafo, encerre ***/
   if ( linha == 0   ||
      ((I = args->len[ linha - 1 ]) > 0   &&
      (L1 = vm_deref( args->ptr[ linha - 1 ], 0 )) != NULL   &&
      (UC) L1[ I - 1 ] == PARA_CHR ) )   return;

   prev = linha - 1;
   I = args->len[ linha ];
   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return;
   P = (UC *) &L1[ 0 ];

   for ( J = 0;   J < I  &&  *P == ' ';   J++, P++ ) /* Virgula */ ;

   if ( J > 0 )   from_next( args, prev, J );
}


ST( SS ) novo_para( args, linha )

ARG * args;
US  linha;
{
   SC   *L1;
   SS   ret = 0, esq, pri, dir;
   register   SS   I, J;
   register   UC   *P, *Q;

   I = args->len[ linha ];
   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return( 0 );
   P = (UC *) &L1[ 0 ];

   /*** Existe alguma marca de paragrafo nesta linha ***/
   for ( J = 0, Q = P;  J < I && *Q != PARA_CHR;  J++, Q++ ) /* Nada */ ;

   /*** Se existir porem for a ultima posicao, encerre ***/
   if ( Q  >=  (UC *) &L1[ I - 1 ] )   return( 0 );

   /*** Calcule resto, guarde e reajuste a linha ***/
   J = (SS) (Q - P) + 1;

   if ( J < I )
   {
      to_next( args, linha, J, W_NOTINS );
      ret = 1;
   }

   /*** Comente este if( ) caso o LB ainda nao indexe observando ... ***/
   /*** ... o formato do paragrafo. ***/
   /*** Pegue e Set as Margens apenas se ja nao existirem ***/
   if ( I < TMF  ||  *(--Q) != ET_FORM2 )
   {
      pega_margens( args, linha + 1, &esq, &pri, &dir );
      set_margens(  args, linha,      esq,  pri,  dir );
   }

   return ( ret );
}


ST( VOID ) quebra_longas( args, linha )

ARG * args;
US  linha;
{
   SS   I;

   while ( linha < args->t_lines  &&  (I = args->len[ linha ]) > MAX_WRAP )
   {
      /*** O que vamos passar para baixo NAO PODERA' ser maior ... ***/
      /*** ... que MAX_WRAP, porem o que ficar nesta linha ... ***/
      /*** NAO PRECISAR ser menor que MAX_WRAP. ***/
      I -= MAX_WRAP;   if ( I < MAX_WRAP )   I = MAX_WRAP;

      to_next( args, linha, I, W_INSERE );
   }
}


ST( VOID ) coloca_paragrafo( args, linha )

ARG * args;
US linha;
{
   SC  *L1;
   SS  I, J;

   /*** Nao precisa marcar o arquivo como modificado ***/
   J = args->f_mod;

   /*** Se nao for a ultima linha retorne ***/
   if ( linha != args->t_lines - 1 )   return;

   /*** Se for a ultima linha ou se a linha for paragrafo, encerre ***/
   I = args->len[ linha ];

   /*** Desejamos apenas a linha que tenha tamanho 0 ou ... ***/
   /*** ... que nao possua marca de final de paragrafo.     ***/
   if ( I != 0  &&
       (L1 = vm_deref( args->ptr[ linha ], 0 )) != NULL  &&
       (UC) L1[ I - 1 ] == PARA_CHR )   return;

   /*** Bem, entao vamos colocar uma marca de paragrafo nesta linha ***/
   linalloc( args, linha, I, I + 1 );
   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return;
   L1[ I ] = PARA_CHR;

   /*** Nao precisa marcar o arquivo como modificado ***/
   args->f_mod = J;

   if ( display != disp_file )  display = disp_line;
}


ST( VOID ) marca_video( args, linha )

ARG * args;
US  linha;
{
   SS  I;

   if ( display == disp_file  ||
        args->je == NULL      ||
        linha   <  args->beg_line   ||
        linha   >  args->beg_line + (args->je->w_height - 1) )  return;

   I = linha - args->beg_line;

   if ( I  <  l_inicio )   l_inicio = I;
   if ( I  >  l_fim    )   l_fim    = I;
}

/*** Retorna 1 se a linha for a primeira do paragrafo ***/
SS  prim_linha( args, linha )

ARG *args;
SS  linha;
{
   SS   I;
   SC   *L1;

   if ( linha != 0   &&
       (I  = args->len[ linha - 1 ]) > 0   &&
       (L1 = vm_deref( args->ptr[ linha - 1 ], 0 )) != NULL   &&
       (UC) L1[ I - 1 ] != PARA_CHR ) {  return( 0 );  }

   else                               {  return( 1 );  }
}

/***
Pega as margens de um paragrafo
Retorna a linha caso ja exista algum formato definido ou ...
... -1 se nao existir formato definido.
***/
SS    pega_margens( args, linha, mar_esq, mar_pri, mar_dir )

ARG   *args;
SS    linha;
SS    *mar_esq;
SS    *mar_pri;
SS    *mar_dir;
{
   SC  *L1;
   SS  T1, T2, tot, esq, pri, dir;
   register   SS   I;
   register   UC   *P;

   *mar_esq = *mar_pri = *mar_dir = 0;

   while( 1 )
   {
      if ( linha >= args->t_lines      ||
          (I  = args->len[ linha ]) == 0   ||
          (L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return( -1 );

      P = (UC *) &L1[ I - 1 ];

      /*** Ja chegou ao limite ***/
      if ((UC) *P-- == PARA_CHR )   break;

      linha++;
   }

   /*** Achou o Final de Paragrafo, verifique se tem FORMATO ***/
   if ( I <= TMF )   return( -1 );

   if ((UC) *P-- != ET_FORM2 )   return( -1 );

   /*** A constante 'A' e' para fugir de carateres especiais e ... ***/
   /*** ... principalmente do BRANCO ***/
   dir = (*P--) - 'A';
   pri = (*P--) - 'A';
   esq = (*P--) - 'A';

   if ((UC) *P != ET_FORM1 )   return( -1 );

   T1 = args->doc_mode + 1;
   tot = COTA( 10, T1, MAX_WRAP );

   T1 = tot * 3 / 4;
   if ((T2 = tot - T1)  <  10 )
   {
      T2  = 10 - T2;
      T1 -= T2;
      if ( T1 < 0 )   T1 = 0;
   }

   esq = COTA( 0, esq, T1 );
   pri = COTA( 0, pri, T1 );

   T1 = tot * 1 / 4;
   if ((T2 = (tot - esq) - T1)  <  10 )
   {
      if ( T2 < 0 )   T2 = 0;
      T2  = 10 - T2;
      T1 -= T2;
      if ( T1 < 0 )   T1 = 0;
   }

   dir = COTA( 0, dir, T1 );

   *mar_dir = dir;
   *mar_pri = pri;
   *mar_esq = esq;

   return( linha );
}

/*** Retira PARA_CHR e se tiver todo o ET_FORM ***/
VOID retira_form( args, linha )

ARG *args;
SS  linha;
{
   SC   *L1;
   SS   I, len;

   if ((L1 = vm_deref( args->ptr[ linha ], 0 )) == NULL )   return;

   if ((len = args->len[ linha ])  <=  0 )   return;

   if ( (UC) L1[ len - 1 ] != PARA_CHR )   return;

   I = len - 1;

   if ( len >= 2 && (UC) L1[ len - 2 ] == ET_FORM2  )   I -= TMF;

   if ( I < 0 )   I = 0;

   linalloc( args, linha, len, I );

   /*** O cursor fica fora da linha para que se posicione corretamenta ***/
   if ( linha == args->clin  &&  args->col > I )   args->col = I;

   display = disp_file;
}

/***************************************************************************/
VOID total_wrap( args, inicio )

ARG * args;
SS inicio;
/***************************************************************************/
{
   SS   I;

   for ( I = inicio;   I < args->t_lines;   I = text_wrap( args, I ) )
      ;
}


