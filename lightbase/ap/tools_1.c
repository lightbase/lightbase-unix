#include  "lb2all.h"

#include <string.h>
#include  <time.h>

#include  "lb2stru.h"
#include  "lb2files.h"
#include  "lb2bases.h"

#include "tools.h"

#ifdef MSDOS
#include <io.h>
#endif

#define   OFF_AC (((((POINTER)128 + (POINTER)(sizeof(Reg_controle))) - (POINTER)1) \
             / (POINTER)sizeof(Reg_controle) ) * (POINTER)sizeof(Reg_controle))

#define   NUM_TO_POS(num)  ( (POINTER)OFF_AC + \
          ((POINTER) sizeof(Reg_controle) * (POINTER)((LONG)num - (LONG)1)))

#define   POS_TO_NUM(pos)  (((pos - (POINTER)OFF_AC) / (POINTER) sizeof(Reg_controle)) + (POINTER) 1)




COUNT trata_lb1()
{  COUNT ret=0, edita;
   LONG pos, reg=0, dl;
   Reg_controle controle;
   FILE *fp=NULL, *fp_log=NULL;
   TEXT aux[80], log[80];

   fprintf(CON, "\nNome da base : ");
   gets(aux);
   strcpy(log, aux);

   if ( aux[0] == '\0' || aux[0] == '.' )
      return(0);

   strcat(aux, ".lb1");

   if ( access(aux, 0) != 0 ) {
      fprintf(stderr, "Arquivo de controle da base nao existe.\n");
      ret = 1;
      goto fim;
   }

#ifdef MSDOS
   fp = fopen( aux, "rb+");
#else
   fp = fopen( aux, "r+");
#endif

   if ( fp == NULL ) {
      fprintf(stderr, "Arquivo de controle da base nao pode ser aberto.\n");
      ret = 1;
      goto fim;
   }

   strcat(log, ".log");

   if ( (fp_log = fopen(log, "w")) == NULL )
      fp_log = stderr;

   while (1) {

      edita = 0;
      fprintf(stderr, "\n\nRegistro a ser manipulado : ");
      gets(aux);

      /* para editar o registro atual vamos usar um '*' */
      if ( aux[0] == '*' && reg != 0 )
         edita = 1;
      else
         reg = atol(aux);

      if (reg <= 0 )
         break;

      pos = NUM_TO_POS(reg);

   	fseek( fp, pos, 0 );

	   if ( fread( &controle, sizeof(Reg_controle), 1, fp) != 1 ) {
         fprintf(stderr, "Erro lendo lb1\n");
         ret = 3;
         goto fim;
      }


      switch(controle.rc_status) {
         case NOVO_REGISTRO   : strcpy(aux, "Novo registro  "); break;
         case ATIVO_INDEX     : strcpy(aux, "Ativo Indexado "); break;
         case ATIVO_NAO_INDEX : strcpy(aux, "Ativo nao indexado"); break;
         case LIBERADO        : strcpy(aux, "Liberado       "); break;
         default :
            strcpy(aux, "ATENCAO..... Status de registro desconhecido.");
            break;
      }

      fprintf(stderr, "--------------------------------------------------------------\n");

      if ( controle.rc_incdata == -1L || controle.rc_pos == -1L ) {
         fprintf(stderr, "\nRegistro Inexistente\n");
         continue;
      }


      fprintf(stderr, "\nStatus         : %c - %s", controle.rc_status, aux);
      if ( edita ) {
         fprintf(stderr, "     Novo Status(0/I/N/D) : ");
         gets(aux);
         if (aux[0] != '\0' )
            controle.rc_status = aux[0];
      }

      if ( ! edita ) {
      	dl = controle.rc_incdata;
         if ( dl > 0 )
            fprintf(stderr, "\ndata inclusao  : %s", ctime(&dl));

      	dl = controle.rc_altdata;
         if ( dl > 0 )
            fprintf(stderr, "data alteracao : %s", ctime(&dl));
      }

      fprintf(stderr, "Posicao em lb2 : %ld", controle.rc_pos);
      if ( edita ) {
         fprintf(stderr, "    Nova posicao : ");
         gets(aux);
         if (aux[0] != '\0' )
            controle.rc_pos = atol(aux);
      }


      if ( edita ) {
   	   fseek( fp, pos, 0 );
	      if ( fwrite( &controle, sizeof(Reg_controle), 1, fp ) != 1) {
            fprintf(stderr, "Erro gravando lb1\n");
            ret = 3;
            goto fim;
         }
      }
   }

fim:

   if ( fp != NULL )
      fclose(fp);

   if ( fp_log != NULL && fp_log != stderr ) {
      fseek(fp_log, 0, 2);
      if ( ftell(fp_log) != 0 )
         fprintf(CON, "Atencao. Verifique arquivo de log %s.\n", log);
      fclose(fp_log);
   }

   fp = NULL;

   return(ret);
}

