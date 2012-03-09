#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <ctype.h>
#ifdef MSDOS
#   include  <dos.h>
#endif

#define   SIB            (*((char **) MK_FP(_psp, 0xFC)))
#define   EV_PROGRAM     (SIB + 9)
#define   EV_SERIE       (*((unsigned long *) (SIB + 22)))


#define   VOID  void
#define   COUNT short
#define   UCOUNT unsigned short
#define   UTEXT unsigned char
#define   UCOUNT unsigned short
#define   SI int
#define   SC char
#define   TEXT char
#define   LONG long
#define   ULONG unsigned long

#define   F__GLB     /* Token para funcoes GLOBAIS */
#define   F__LOC     /* Token para funcoes LOCAIS  */

UCOUNT   idbase;   /* Numero de identificacao da base */
COUNT    release;  /* Release da base comercial       */
COUNT    finger;   /* Identifica base comercial       */

TEXT nome_arq[50];

#define   TAM_SERIE    6


typedef struct {
   TEXT *cadeia;
   TEXT  sep;
   UCOUNT  achou;
} PT_TOKEN;



#ifdef MSDOS
   VOID chacoalha(TEXT *, TEXT *, UCOUNT, UCOUNT, UCOUNT);
   VOID roda_viva( TEXT * );
   VOID pega_strid( TEXT *, TEXT *);
   VOID xor_id( TEXT *, TEXT * );
   UCOUNT pega_tok( TEXT *, TEXT *, PT_TOKEN *);
   VOID str_nible( TEXT *, TEXT * );
   SC   get_nible( SC, SI );
   COUNT atualiza_hdr( VOID );
#else
   VOID chacoalha( );
   VOID roda_viva( );
   VOID pega_strid( );
   VOID xor_id( );
   UCOUNT pega_tok( );
   VOID str_nible( );
   SC   get_nible( );
   COUNT atualiza_hdr();
#endif


F__GLB   COUNT main(argc, argv)
int argc;
char **argv;
{  LONG l, num=0;
   COUNT i;
   TEXT aux[200], aux2[20], del = '\0', *p;
   TEXT token[60+1], salva[60];
   TEXT senha[20], chave[20];
   FILE *fp, *fpo;

   printf("\n");
   printf("Copyright 1991 Light Software LTDA\n");
   printf("Copyright 1995 - Light-Infocon\n\n");
   printf("Copyright 1996 - Light-Infocon S/A\n\n");
   printf("Gerador de chave de PERMISSAO para CLIENTES de BASES\n");

#ifdef MSDOS
   if ( !strncmp( SIB, "AZEV", 4 ) ) {
      num = EV_SERIE;
   }
#else
	printf("Numero do fornecedor ? ");
	gets(aux);
	num = atol(aux);
#endif

   if (num == 0 ) {
      putchar(7);
      printf("\n\nUtilizacao nao autorizada\n");
      printf("Uso exclusivo de FORNECEDORES cadastrados\n");
      exit(1);
   }

   printf("Uso restrito do FORNECEDOR. Matricula : %ld", num);


   nome_arq[0] = '\0';

   if ( argc > 1) {
   	for( i = 1;   i < argc;   i++ ) {
         if ( argv[i][0] != '-' ) { /* erro */
            printf("Parametro Invalido\n");
            exit(1);
         }

         if ( toupper(argv[i][1]) ==  'B' ) {
            /* seta nome de arquivo para macro  */
            strcpy(nome_arq, &argv[i][2]);
         }

      }

   }

   senha[0] = token[0] = '\0';


   /****************************** Pega Token ****************************/

   printf("\nIdentificacao do Usuario : ");
   gets(aux);

   if ( aux[0] == '\0' )
      exit(0);

   l = atol(aux);

   if ( l == 0 ) {
      /* nome do usuario pode ser limitado usando [x] no inicio da string */
      if (aux[0] == '[' && aux[2] == ']') {
         del = aux[1];
         strncpy(token, aux+3, 60);
         p = strchr(token, del);
         if ( p != NULL )
            *p = '\0';
      }
      else {
         strncpy(token, aux, 60);
      }
      token[60] = '\0';
   }

   else {
      sprintf(token, "%6.6ld", l);
   }

   /****************************** Pega outros dados *********************/

   idbase = (UCOUNT) num;

le :

   printf("\nIdentificacao da base : ");
   gets(aux);
   finger = atoi(aux);

   if ( finger <= 1000 || finger >= 2000 ) {
      putchar(7);
      printf("\n\tIdentificacoes liberadas no intervalo de 1001 a 1999\n");
      goto le;
   }

le1:

   printf("\nVersao da base : ");
   gets(aux);
   release = atoi(aux);

   if ( release < 1 || release >= 2000 ) {
      putchar(7);
      printf("\n\tVersoes liberadas no intervalo de 1 a 1999\n");
      goto le1;
   }

   strcpy(salva, token);
   chacoalha(aux, token, finger, release, idbase);
   strcpy(chave, aux);

   if ( num != 0 ) { /* so pra garantir integridade da copia */
      if ( nome_arq[0] != '\0' ) {
         strcat(nome_arq, ".lb");

         if ( (fpo = fopen(nome_arq, "w")) == NULL ) {
            fpo = stdout;
         }

         fprintf(fpo, "%s\t\t%s\n", salva, chave);

         if ( fpo != stdout) {
            fclose(fpo);
            printf("\nResultado gerado no arquivo %s\n", nome_arq);
         }



      }
      else {
         fp = fopen("autoriza.txt", "r");
         if ( fp == NULL ) {
            printf("\nChave de liberacao : %s\n", chave);
         }
         else {
            if ( (fpo = fopen("autoriza.doc", "w")) == NULL ) {
               fpo = stdout;
            }

            memset(aux, 0, 150);
            while (fgets(aux, 150, fp)) {
               while( (p = strchr(aux, '@')) != NULL ) {
                  switch(*(p+1)) {
                     case 'N' : /* Num serie / Nome */
                              strncpy(p, salva, strlen(salva));
                              break;
                     case 'A' : /* Matricula */
                              sprintf(aux2, "%5.5u", num);
                              strncpy(p, aux2, strlen(aux2));
                              break;
                     case 'I' : /* Identificacao da Base */
                              sprintf(aux2, "%5.5u", finger);
                              strncpy(p, aux2, strlen(aux2));
                              break;
                     case 'V' : /* Versao */
                              sprintf(aux2, "%5.5u", release);
                              strncpy(p, aux2, strlen(aux2));
                              break;
                     case 'C' : /* chave */
                              strncpy(p, chave, strlen(chave));
                              break;
                     default  : *p = ' ';
                              break;

                  }
               }
               fprintf(fpo, "%s", aux);
               if ( strchr(aux, '\n') == NULL )
                  fprintf(fpo, "\n");

               memset(aux, 0, 150);
            }
            fclose(fp);
            if ( fpo != stdout)
               fclose(fpo);
            printf("\nResultado gerado no arquivo autoriza.doc\n");
         }
      }
   }

   return(0);
}

F__LOC   VOID chacoalha(sai, str, n1, n2, n3)
TEXT *sai, *str;
UCOUNT n1, n2, n3;
{  TEXT strid[20], aux[30];
   LONG l;
   UTEXT c=0;
   COUNT i;

	memset( strid, 0, 20 );

	sprintf( aux, "%6.6u", n1 );
   roda_viva(aux);

   if ( (l = atol(str)) == 0 ) {
   	pega_strid( str, strid );
   }
   else {
      int x1,x2,x3;
      x1 = (int)l/10000;
      x2 = (int)((l%10000) / 100);
      x3 = (int)(l % 100);
      sprintf(str, "%2.2d %2.2d %2.2d", x1, x2, x3);
   	pega_strid( str, strid );
   }
	xor_id( aux, strid );

	sprintf( strid, "%6.6u", n2 );
   roda_viva(aux);
	xor_id( aux, strid );

	sprintf( strid, "%6.6u", n3 );
   roda_viva(aux);
	xor_id( aux, strid );


   for ( i = 0; i < 6 && str[i] != '\0'; i++) {
      c += str[i];
   }

   c = c & 0xf;

   for ( i = 0; sai[i] != '\0'; i++) {
      sai[i] = sai[i] + c;
   }

	str_nible(aux, sai);
}

VOID roda_viva(str)
TEXT *str;
{  COUNT i;

   for ( i = 0; str[i] != '\0'; i++) {
      str[i] += 1;
   }
}


VOID xor_id( serial, xorado )

SC *serial, *xorado;
{
	SI i,j,m;

	for ( i=0; serial[i]; ++i )
	{
	        m = i+1;

	        if( m > strlen(xorado) )
	           m = strlen(xorado);

		for ( j=m; xorado[j]; ++j )
		{
			serial[i] = serial[i] ^ xorado[j];
		}
	}

	return;
}


VOID pega_strid( linha, id )
SC *linha;
SC *id;
{
	PT_TOKEN Tok;
	SC aux[128];
	SI  i = 0, j = 0, k = 0 ;

	while( 1 )
	{
	 k = 0;
	 strcpy( aux, linha );
	 pega_tok( aux, " ", &Tok );
	 while ( *Tok.cadeia )
	 {
	    if ( j < strlen( Tok.cadeia ) )
	    {
		  id[i++] = toupper(Tok.cadeia[j]);
		  if ( i == 8 )
			goto fim;
		  k++;
	    }
	    pega_tok( NULL, " ", &Tok );
	 }
	 j++;
	 if ( !k )
	 {
	    while( i < 8 )
	    {
		  id[i] = 'A' + 8 - i - 1;
		  ++i;
	    }
	    goto fim;
	 }
	}
fim:
	return;
}


UCOUNT pega_tok( cadeia, separador, result )
TEXT *cadeia, *separador;
PT_TOKEN *result;
{
   /*
      Pega tudo de cadeia até encontrar separador. Quando encontrar
      coloca 'zero' na posicao e retorna 1. Se nao encontrar retorna 0.
   */

   static TEXT *pt_inicio, *pt_start;

   if( cadeia )   pt_inicio = cadeia;
   else           pt_inicio = pt_start;

   result->cadeia = pt_inicio;
   result->achou  = 0;

   for( ; *pt_inicio; ++pt_inicio )
   {
      if( strchr( separador, *pt_inicio ) )
      {
         result->achou = 1;
         result->sep   = *strchr( separador, *pt_inicio );
         *pt_inicio = 0;
         pt_start   = ++pt_inicio;
         goto fim;
      }
   }
   pt_start = pt_inicio;

fim:
   return result->achou;
}



VOID str_nible( chave, nibles )
SC *chave;
SC *nibles;
{
	SI i,j;

	i = 0;
	for ( j=0; j<TAM_SERIE; j++)
	{
	 nibles[i++] = get_nible(chave[j],0);
	 nibles[i++] = get_nible(chave[j],1);
	}

	nibles[ 2*TAM_SERIE ] = 0;
}


SC get_nible( c, n )
SC c;
SI n;
{
	SC cc;

	if( n )      cc = c & 0xf;
	else         cc = c >> 4;

	if( cc < 10 ) cc+='0';
	else          cc+='A'-10;

	return cc;
}




COUNT atualiza_hdr()
{

      return(0);
}
