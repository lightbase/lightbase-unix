#include  "lb2all.h"

#include <string.h>


#include "tools.h"


VOID pesquisa(nomeind)
TEXT *nomeind;
{
	TEXT linha[200], chave[50];
	POINTER ret;
	COUNT func = 5, i, key_file, key_index=0;

   if ( nomeind == NULL || nomeind[0] == '\0' ) {
      fprintf(CON, "\nNome do arquivo de indices : ");
      gets(linha);
   }
   else
      strcpy(linha, nomeind);


   if ( (key_file = abre_indice(linha, FILE_KEYNO)) < 0 ) {
      return;
   }

	while ( 1 ) {

		printf("Comando de pesquisa : ");
		gets(linha);

		if (strcmp(linha, "FIM" ) == 0 ) break;

		if (linha[0] == '?' ) {
			printf("\tCOMANNDO chave\n");
			printf("\t=  chave\n");
			printf("\t>  chave\n");
			printf("\t<  chave\n");
			printf("\t>= chave\n");
			printf("\t<= chave\n");
			printf("\t+  chave (proxima)\n");
			printf("\t-  chave (anterior)\n");
			printf("\t&  (primeira)\n");
			printf("\t.  (ultima)\n");
			printf("\tin  usa indice n\n");
			continue;
		}

		if (linha[0] == '\0' ) break;

		switch (linha[0]) {
			case '='  : func = 1;   break;
			case '>'  : if (linha [1] == '=' ) func = 2;
							else 						  func = 7;
							break;

			case '<'  : if (linha [1] == '=' ) func = 9;
							else 						  func = 8;
							break;

			case '+'  : func = 3; 	break;
			case '-'  : func = 4; 	break;
			case '&'  : func = 5; 	break;
			case '.'  : func = 6; 	break;
/*** 			case 'G'  : func = 30; 	break;  GATOKEY ***/
			case 'i'  : key_index = linha[1] - '0'; break;
			default   :             break;
		}

		for (i=0; i < strlen(linha) ; i++ )
			 if ( strchr(" >=<+-&.", linha[i])  ==  NULL )
				 break;
		strcpy(chave, &linha[i]);

      switch ( func ) {
      	case D_EQLKEY :  /* */
								ret = EQLKEY(key_file + key_index, chave, strlen(chave) + 1);
								printf("\tRes = %lu     CT_INDKEY=[%s]  CTLENKEY=%2d\n",
                        ret, ct_indkey, ct_lenkey);
                        break;
	      case D_GTEKEY :
								ret = GTEKEY(key_file + key_index, chave, linha, strlen(chave) + 1);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
                        break;
			case D_NXTKEY :  ret = NXTKEY(key_file + key_index , linha);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
                        break;
			case D_PRVKEY :  ret = PRVKEY(key_file + key_index , linha);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
								break;
			case D_FRSKEY :  ret = FRSKEY(key_file + key_index , linha);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
                        break;
			case D_LSTKEY :  ret = LSTKEY(key_file + key_index , linha);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
                        break;
			case D_GTKEY  : /**/
								ret = GTKEY(key_file + key_index, chave, linha, strlen(chave) + 1);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
                        break;
			case D_LTKEY  : /**/
								ret = LTKEY(key_file + key_index, chave, linha, strlen(chave) + 1);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
                        break;
			case D_LTEKEY :  /* */
								ret = LTEKEY(key_file + key_index, chave, linha, strlen(chave) + 1);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
                        break;
/****
	      case 30 :
								ret = GATOKEY(key_file + key_index, chave, linha, strlen(chave) + 1);
								printf("\tRes = %lu  Achou ==> [%s]      CT_INDKEY=[%s]  CTLENKEY=%2d\n",
								ret, linha, ct_indkey, ct_lenkey);
                        break;
****/
      }
   }

	fecha_indice(key_file);
}

