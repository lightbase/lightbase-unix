#include  <stdio.h>
#include  <stdlib.h>

#define   SC signed char
#define   SL signed long
#define   US unsigned short
#define   UC unsigned char
#define   UL unsigned long


typedef struct 
{  /***** ATENCAO SE ALTERAR MUDAR TAMBEM SDINDEX logo abaixo *****/
   SC   palavra[ 30 ];
   SL   registro;
   US   campo;
   US   paragrafo;  /* So' Campo Texto, outros Campos serao de uma Frase */
   US   frase;      /* So' Campo Texto, outros Campos serao de uma Frase */
   US   sequencia;  /* Sequencia da Palavra na FRASE */
}  S_INDEX;

US to_bits( S_INDEX *, UC *);
US to_struct( S_INDEX *, UC *);

main()
{  int x;
   UC str[10];
   SC aux[100];

   S_INDEX si, so;

   while ( 1 ) {
      printf("Registro  ? "); gets(aux); si.registro  = atol(aux);
       if ( aux[0] == '.' ) return(0);

      printf("campo     ? "); gets(aux); si.campo     = atoi(aux);
      printf("paragrafo ? "); gets(aux); si.paragrafo = atoi(aux);
      printf("frase     ? "); gets(aux); si.frase     = atoi(aux);
      printf("sequencia ? "); gets(aux); si.sequencia = atoi(aux);

      x = to_bits(&si, str);

      printf("%d %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", x,
         str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7], str[8]);

      to_struct( &so, str);
  
      x = 0;

      if (si.registro  != so.registro  ) { printf("si.registro  diferente\n"); x=1; };
      if (si.campo     != so.campo     ) { printf("si.campo     diferente\n"); x=1; };
      if (si.paragrafo != so.paragrafo ) { printf("si.paragrafo diferente\n"); x=1; };
      if (si.frase     != so.frase     ) { printf("si.frase     diferente\n"); x=1; };
      if (si.sequencia != so.sequencia ) { printf("si.sequencia diferente\n"); x=1; };

      if ( x ) {
         printf("Reg = %ld\nCampo = %d\nParagrafo = %d\nFrase = %d\nSequencia = %d\n",
            si.registro, si.campo, si.paragrafo, si.frase, si.sequencia);
      }
   }
}



US to_bits( I, bits )
S_INDEX   *I;
UC        *bits;
{
   US   tipo;

   if (      I->sequencia < 16  &&      /* 4  BITS */
             I->frase     < 2   &&      /* 1  BITS */
             I->paragrafo < 2   &&      /* 1  BITS */
             I->campo     < 16  &&      /* 4  BITS */
             I->registro  < 4096L  )    /* 12 BITS */
   {  /***   MINIMO   ***/
      tipo = 0;
      bits[ 0 ] = ((tipo & 3) << 6) +
                  ((I->sequencia & 0x0F) << 2) +
                  ((I->frase & 1) << 1) +
                  ((I->paragrafo & 1));
      bits[ 1 ] = ((I->campo & 0x0F) << 4) +
                  ((I->registro & 0xF00) >> 8);
      bits[ 2 ] = ((I->registro & 0xFF));

      return ( 3 );
   }

   else if ( I->sequencia < 16  &&      /* 4  BITS */
             I->frase     < 16  &&      /* 4  BITS */
             I->paragrafo < 8   &&      /* 3  BITS */
             I->campo     < 32  &&      /* 5  BITS */
             I->registro  < 16384L )    /* 14 BITS */
   {  /***   MEDIO   ***/
      tipo = 1;
      bits[ 0 ] = ((tipo & 3) << 6) +
                  ((I->sequencia & 0x0F) << 2) +
                  ((I->frase & 0x0C) >> 2);
      bits[ 1 ] = ((I->frase & 0x03) << 6) +
                  ((I->paragrafo & 0x7) << 3) +
                  ((I->campo & 0x1C) >> 2);
      bits[ 2 ] = ((I->campo & 0x03) << 6) +
                  ((I->registro & 0x3F00) >> 8);
      bits[ 3 ] = ((I->registro & 0xFF));

      return ( 4 );
   }

   else if ( I->sequencia < 32  &&      /* 5  BITS */
             I->frase     < 64  &&      /* 6  BITS */
             I->paragrafo < 64  &&      /* 6  BITS */
             I->campo     < 32  &&      /* 5  BITS */
             I->registro  < 65536L )    /* 16 BITS */
   {  /***   MAXIMO   ***/
      tipo = 2;
      bits[ 0 ] = ((tipo & 3) << 6) +
                  ((I->sequencia & 0x1F) << 1) +
                  ((I->frase & 0x20) >> 5);
      bits[ 1 ] = ((I->frase & 0x1F) << 3) +
                  ((I->paragrafo & 0x38) >> 3);
      bits[ 2 ] = ((I->paragrafo & 0x07) << 5) +
                  ((I->campo & 0x1F));
      bits[ 3 ] = ((I->registro & 0xFF00) >> 8);
      bits[ 4 ] = ((I->registro & 0xFF));

      return ( 5 );
   }
#ifdef ERA_ASSIM
...   else if ( I->sequencia < 512  &&      /* 9  BITS */
...             I->frase     < 1024 &&      /* 10 BITS */
...             I->paragrafo < 2048 &&      /* 11 BITS */
...             I->campo     < 256  &&      /* 8  BITS */
...             I->registro  < 16777216L )  /* 24 BITS */
...   {  /***   ABSURDO   ***/
...      tipo = 3;
...      bits[ 0 ] = ((tipo & 3) << 6) +
...                  ((I->sequencia & 0x1F8) >> 3);
...      bits[ 1 ] = ((I->sequencia & 0x07) << 5) +
...                  ((I->frase & 0x3E0) >> 5);
...      bits[ 2 ] = ((I->frase & 0x1F) << 3) +
...                  ((I->paragrafo & 0x700) >> 8);
...      bits[ 3 ] = ((I->paragrafo & 0xFF));
...      bits[ 4 ] = ((I->campo & 0xFF));
...      bits[ 5 ] = ((I->registro & 0xFF0000L) >> 16);
...      bits[ 6 ] = ((I->registro & 0xFF00) >> 8);
...      bits[ 7 ] = ((I->registro & 0xFF));
...
...      return ( 8 );
...   }
#endif

   else if ( I->sequencia < 1024  &&      /* 10  BITS */
             I->frase     < 2048  &&      /* 11 BITS */
             I->paragrafo < 16384 &&      /* 14 BITS */
             I->campo     < 256   &&      /* 8  BITS */
             I->registro  < 134217728L )  /* 27 BITS */
   {  /***   ABSURDO   ***/
      tipo = 3;
      bits[ 0 ] = (tipo << 6) +                   /* tt...... */
                  (I->sequencia >> 4);            /* pegou 1111110000 de seq ttssssss */
      bits[ 1 ] = ((I->sequencia & 0x0f) << 4) +  /* ssss.... */
                  (I->frase >> 7);                /* pegou 111 1000 0000 de frase  ssssffff*/
      bits[ 2 ] = ((I->frase & 0x7F) << 1) +      /* fffffff. */
                  (I->paragrafo >> 13);           /* pegou 10 0000 0000 0000 de paragrafo  fffffffp */
      bits[ 3 ] = (I->paragrafo & 0x1FE0) >> 5;   /* pppppppp */
      bits[ 4 ] = ((I->paragrafo & 0x1F) << 3) +  /* ppppp... */
                  (I->campo >> 5);                /* pegou 0001 1111 de campo pppccccc */
      bits[ 5 ] = ((I->campo & 0x01F) << 3) +     /* ccccc... */
                  (I->registro >> 24);            /* pegou 111000000000000000000000000 registro  cccccrrr */
      bits[ 6 ] = ((I->registro & 0xFF0000L) >> 16);
      bits[ 7 ] = ((I->registro & 0xFF00) >> 8);
      bits[ 8 ] = ((I->registro & 0xFF));

      return ( 9 );
   }

   else {
      printf("WWW %l %d %d %d %d",
         I->registro, I->campo, I->paragrafo, I->frase, I->sequencia);
      return( 0 );
   }
}



US  to_struct( I, bits )
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
