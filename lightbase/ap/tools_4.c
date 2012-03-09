#include  "lb2all.h"

#include <string.h>


#include "tools.h"


COUNT lista_ind(nomeind, filename)
TEXT *nomeind;
TEXT *filename;
{
	TEXT chave[50], *p, *p1, saida[100], anterior[100], linha[100];
	POINTER pos_arq;
	COUNT ni, key_file, key_index=0, ret = 0;
   LONG nc=0L, ncd=0L, nci=0L, npal=0L;
   LONG tam_indice = 0;
   FILE *fileout=NULL;

   anterior[0]='\0';

   if ( nomeind == NULL || nomeind[0] == '\0' ) {
      fprintf(stderr, "\nNome do arquivo de indices : ");
      gets(linha);
   }
   else
      strcpy(linha, nomeind);


   if ( (key_file = abre_indice(linha, FILE_KEYNO)) < 0 ) {
      ret = -1;
      goto fim;
   }


   if ( filename != NULL )
      fileout = fopen(filename, "w");

   if ( fileout == NULL )
      fileout = stdout;

   ni = 0;

   if ( param.totaliza ) {
      param.so_conta=0;
      param.delimita_key = 1;
   }

	pos_arq = FRSKEY(key_file + key_index + ni, chave);

   /*so guarda chave anterior */
   for (p=chave, p1=anterior; *p != '\0' && *p != ' '; p++, p1++)
      *p1 = *p;
   *p1 = '\0';


	if ( pos_arq == 0 ) {
		if ( uerr_cod ) {
         LOG_SL("Erro em FRSKEY : %d", uerr_cod);
		}
		else {
         LOG_S("Indice esta vazio !!!\n");
		}
	}

	while ( pos_arq ) {

      if ( param.delimita_key ) {
			if ( chave[0] > ' ') {
            for (p=chave, p1=saida; *p != '\0' && *p != ' '; p++, p1++)
               *p1 = *p;
            *p1 = '\0';

            if ( stricmp(saida, anterior) != 0 ) {
   		      if ( param.so_conta != 0 ) {
                  if ( param.so_conta++ % 100 == 0 ) {
                     fprintf(fileout, "\r%ld ", param.so_conta);
                     fprintf(fileout, "  %ld", tam_indice);
                  }
               }
               else {
                  if ( param.totaliza != 0 ) {
                     if ( npal >= param.minimo && npal <= param.maximo ) {
                        fprintf(fileout, "%06.6ld - %s\n", npal, anterior);
                     }
                  }
                  else
                     fprintf(fileout, "%s\n", anterior);
               }
               if ( npal <= 2 )
                  tam_indice += 25;
               else
/*                  tam_indice = tam_indice + 39 + (((npal/5) + 1) * 76); */
                  tam_indice = tam_indice + 25 + 16 + (npal * 10);


               strcpy(anterior, saida);
               npal=1;
               ncd++;
            }
            else
               npal++;
         }
         else {
            nci++;
         }

      }
      else
   		if ( param.so_conta != 0 ) {
            if ( param.so_conta++ % 100 == 0 )
               fprintf(fileout, "\r%ld", param.so_conta);
         }
         else {

            if ( param.lista_pos == 0 )
               fprintf(fileout, "%s\n", chave);
            else {
               LONG l;
               UTEXT *p;

               p = (UTEXT*)strchr(chave, '\0') + 1;
               pega_long((UTEXT *)&l, p);
               fprintf(fileout, "%s %ld\n", chave, l);
            }
         }

      nc++;

		pos_arq = NXTKEY(key_file + key_index + ni, chave);
		if ( uerr_cod ) {
         LOG_SL("Erro em NXTKEY : %d", uerr_cod);
         fecha_indice(key_file);
         ret = -1;
         goto fim;
		}
	}

   if ( param.delimita_key ) {

      if ( param.totaliza != 0 ) {
         if ( npal >= param.minimo && npal <= param.maximo )
            fprintf(fileout, "%06.6ld - %s\n", npal, anterior);
      }
      else
         fprintf(fileout, "%s\n", anterior);

      fprintf(fileout, "\n\nTotal de chaves : %ld   Chaves Distintas : %ld   especiais : %ld\n\n", nc, ncd, nci);
   }
   else
      fprintf(fileout, "\n\nTotal de chaves : %ld\n\n", nc);

   if ( fileout != stdout )
      fclose(fileout);

   fecha_indice(key_file);

fim :

   return(ret);
}



