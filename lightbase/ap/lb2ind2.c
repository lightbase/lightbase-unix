#include  "lb2all.h"

#include <ctype.h>
#include <string.h>
#include  HD_IO

#include  "lb2stru.h"
#include  "lb2vars.h"
#include  "lb2text.h"  /* strings comus a todos os modulos */


#include  "lb2bases.h"
#include  "lb2maces.h"
#include  "lb2prot1.h"
#include  "lb2trans.h"



#define   MAX_SEQ   500
#define   MAX_FRA   1000



#define   TML  255            /*** Tamanho Maximo da LINHA   ***/



#ifdef MSDOS
   SI    bqsp_comp(TEXT **, TEXT **);
#else
   SI    bqsp_comp( );
#endif



/*
**#  ---------------------------------------------------------------------
**#   Funcao...: prep_indice()
**#   Objetivo.: preparar indice
**#
**/
F__GLB   VOID prep_indice( I, registro, campo, paragrafo, frase, sequencia )
S_INDEX  *I;
UL       registro;
US       campo;
US       paragrafo;
US       frase;
US       sequencia;
{
   I->palavra[0]= '\0';
   I->registro  = registro;
   I->campo     = campo;
   I->paragrafo = paragrafo;
   I->frase     = frase;
   I->sequencia = sequencia;
}




/*
**#  ---------------------------------------------------------------------
**#   Funcao...: sep_palavra
**#   Objetivo.: avaliar strings separando palavras
**#
**/
F__GLB   SS  sep_palavra( I, string, D, f_exe )
S_INDEX    *I;
SC         *string;
S_DELIMIT  *D;
#ifdef MSDOS
   COUNT (*f_exe)(S_INDEX *, TEXT *);
#else
   COUNT (*f_exe)( );
#endif
{
   SS    ret = 0, tam_pal, separa;
   UC    *P1, *P2, *P3;

   P3 = (UC*)&I->palavra[ 0 ];

   for ( P1 = (UC*)string;   *P1 != '\0';   P1++ )
   {
      /*** Ache em P1 o primeiro caracter valido ***/

      if ( strchr( D->paragrafo, *P1 ) != NULL )
      {
         I->paragrafo++;   I->frase = I->sequencia = 0;
         continue;
      }

      if ( strchr( D->frase,     *P1 ) != NULL )
      {
         I->frase++;   I->sequencia = 0;
         continue;
      }

      if ( strchr( D->palavra,   *P1 ) != NULL ){  continue;  }

      /*** Ache em P2 o ultimo caracter valido ***/
      separa = 0;
      tam_pal = 1;

      for ( P2 = P1 + 1;  *P2 != '\0';   P2++, tam_pal++ ) {
         switch ( *P2 )  {
            case ':' :    /*** HORAS -> 12:00 ***/
            case ',' :    /*** VALOR -> 12,89 ***/
            case '.' :    /*** VALOR -> 9.600 ***/
                           if ( *(P2 + 1) >= '0'  &&  *(P2 + 1) <= '9' ) {
                              P2++;
                              tam_pal++;
                              continue;
                           }
                           break;
            case '\'' :    /*** D'agua ***/
                           if ( (*(P2 + 1) >= 'A'  &&  *(P2 + 1) <= 'Z') ||
                              (*(P2 + 1) >= 'a'  &&  *(P2 + 1) <= 'z')  )
                           {
                              P2++;
                              tam_pal++;
                              continue;
                           }
                           break;
         }

         if ( strchr(D->paragrafo, *P2) != NULL ) {  separa = 3;  break;  }
         if ( strchr(D->frase,     *P2) != NULL ) {  separa = 2;  break;  }
         if ( strchr(D->palavra,   *P2) != NULL ) {  separa = 1;  break;  }
      }

      if ( tam_pal >= par_cfg.word_len || ( tam_pal == 1 && isdigit(P1[0] ) ) )
      {
         tam_pal = MIN( tam_pal, IND_EFETIVA );

         strncpy( (SC*)P3, (SC*)P1, tam_pal );

         P3[ tam_pal ] = '\0';

         prep_palavra( (SC*)P3 );

         if ( bsearch( (TEXT *)&P3, (TEXT *)D->stop_words, D->q_stop,
                       sizeof( SC * ), (SI (* )()) bqsp_comp ) == NULL )
         {
            /*** P1 esta' sendo passado para ajudar no brilho ***/
            if ( (ret = ( *f_exe )( I, (SC*)P1 )) != 0 )   goto fim;
         }
      }

      /*** separa indica qual delimitador que foi encontrado por P2 ***/
      switch ( separa )
      {
      case  3 :   /*** Paragrafo ***/
                  I->paragrafo++;  I->frase = I->sequencia = 0;  break;

      case  2 :   /*** Frase ***/
                  if ( I->frase++ > MAX_FRA ) {
                     I->paragrafo++;  I->frase = I->sequencia = 0;
                  }
                  else
                     I->sequencia = 0;
                  break;

      case  1 :   /*** Palavra ***/
                  if ( I->sequencia++ > MAX_SEQ ) {
                     I->frase++;  I->sequencia = 0;
                  }
                  break;

      case  0 :   /*** Final -> '\0' ***/
                  if ( I->sequencia++ > MAX_SEQ ) {
                     I->frase++;  I->sequencia = 0;
                  }
                  goto fim;
      }

      /*** P1 ainda vai ser incrementado no para fugir do delimitador ***/
      P1 = P2;
   }

fim :

   return( ret );
}


F__GLB   SS lb_palavra( adoc, lin_inicio, I, D, f_exe )
ARG        *adoc;
US         lin_inicio;
S_INDEX    *I;
S_DELIMIT  *D;
SS         ( *f_exe )( );
{  SS    ret = 0;
   UC    string[ TML + 2 ];

   while (ret == 0 &&
         lin_texto( adoc, lin_inicio++, (TEXT *)string, 0) >= 0) {
      ret = sep_palavra( I, (SC *)string, D, f_exe );
   }

   return(ret);
}




F__GLB   US  to_struct( I, bits )
S_INDEX   *I;
UC        *bits;
{
   US   tipo;
   UL   S0, S1, S2, S3, S4, S5, S6, S7, S8;

   S0 = bits[ 0 ];
   S1 = bits[ 1 ];
   S2 = bits[ 2 ];
   S3 = bits[ 3 ];
   S4 = bits[ 4 ];
   S5 = bits[ 5 ];
   S6 = bits[ 6 ];
   S7 = bits[ 7 ];
   S8 = bits[ 8 ];

   tipo = ((S0 & 0xC0) >> 6);

   switch ( tipo )
   {
   case 0 : /*** MINIMO  ***/
            /*** 2  - Tipo ***/
            /*** 4  - Sequencia ***/
            /*** 1  - Frase ***/
            /*** 1  - Paragrafo ***/
            /*** 4  - Campos ***/
            /*** 12 - registro ***/
            /*** 24 - TOTAL 3 BYTES ***/

            I->sequencia = (S0 & 0x3C) >> 2;
            I->frase     = (S0 & 0x02) >> 1;
            I->paragrafo = (S0 & 0x01);
            I->campo     = (S1 & 0xF0) >> 4;
            I->registro  = ( SL ) ( ( (S1 & 0x0F) << 8) + S2 );
            break;

   case 1 : /*** MEDIO ***/
            /*** 2  - Tipo ***/
            /*** 4  - Sequencia ***/
            /*** 4  - Frase ***/
            /*** 3  - Paragrafo ***/
            /*** 5  - Campos ***/
            /*** 14 - registro ***/
            /*** 32 - TOTAL 4 BYTES ***/

            I->sequencia = ((S0 & 0x3C) >> 2);
            I->frase     = ((S0 & 0x03) << 1) + ((S1 & 0xC0) >> 6);
            I->paragrafo = ((S1 & 0x38) >> 3);
            I->campo     = ((S1 & 0x07) << 2) + ((S2 & 0xC0) >> 6);
            I->registro  = ( SL ) ( ( (S2 & 0x3F) << 8) + S3 );
            break;

   case 2 : /*** MAXIMO ***/
            /*** 2  - Tipo ***/
            /*** 5  - Sequencia ***/
            /*** 6  - Frase ***/
            /*** 6  - Paragrafo ***/
            /*** 5  - Campos ***/
            /*** 16 - registro ***/
            /*** 40 - TOTAL 5 BYTES ***/

            I->sequencia = ((S0 & 0x3E) >> 1);
            I->frase     = ((S0 & 0x01) << 5) + ((S1 & 0xF8) >> 3);
            I->paragrafo = ((S1 & 0x07) << 3) + ((S2 & 0xE0) >> 5);
            I->campo     = ((S2 & 0x1F));
            I->registro  = ( SL ) ( (S3 << 8) + S4 );
            break;
#ifdef ERA_ASSIM
   case 3 : /*** ABSURDO ***/
            /*** 2  - Tipo ***/
            /*** 9  - Sequencia ***/
            /*** 10 - Frase ***/
            /*** 11 - Paragrafo ***/
            /*** 8  - Campos ***/
            /*** 24 - registro ***/
            /*** 64 - TOTAL 8 BYTES ***/

            I->sequencia = ((S0 & 0x3F) << 3) + ((S1 & 0xE0) >> 5);
            I->frase     = ((S1 & 0x1F) << 5) + ((S2 & 0xF8) >> 3);
            I->paragrafo = ((S2 & 0x07) << 8) + S3;
            I->campo     = S4;
            I->registro  = ( SL ) ( (S5 << 16) + (S6 << 8) + S7 );
            break;
   }
#endif
   case 3 : /*** ABSURDO ***/
            /*** 2  - Tipo ***/
            /*** 10  - Sequencia ***/
            /*** 11 - Frase ***/
            /*** 14 - Paragrafo ***/
            /*** 8  - Campos ***/
            /*** 27 - registro ***/
            /*** 72 - TOTAL 9 BYTES ***/

            I->sequencia = ((S0 & 0x3F) << 4) + ((S1 & 0xF0) >> 4);
            I->frase     = ((S1 & 0xF) << 7) + (S2 >> 1);
            I->paragrafo = ((S2 & 0x1) << 13) + (S3 << 5) + (S4 >> 3 );
            I->campo     = ((S4 & 0x7) << 5) + (S5 >> 3);
            I->registro  = ((S5 & 0x7) * 0x1000000L ) + (S6 * 0X10000L) + (S7 * 0x100L) + S8;
            /* I->registro  = ( SL ) ((SL)(S5 && 0x7) << 24) + (SL)(S6 << 16) + (SL)(S7 << 8 ) + S8; */
            break;
   }
   return( tipo );
}


F__LOC   SI  bqsp_comp( S1,  S2 )
SC    **S1, **S2;
{
   return( strcmp( *S1, *S2 ) );
}

