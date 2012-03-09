#include "lb2all.h"

#include <stdlib.h>
#include <string.h>

#include "tools.h"


#ifdef MSDOS
#include <io.h>
#endif

TEXT *nomearq="arq.lb$";
TEXT *nomeind="ind.lb$";


#ifdef MSDOS
    POINTER deleta_rec(COUNT, POINTER);
    VOID teste1(COUNT, COUNT);
    VOID teste2(COUNT, COUNT);
    VOID teste3(COUNT, COUNT);
    VOID teste4(COUNT, COUNT);
    VOID teste7(COUNT, COUNT);
#else
    POINTER deleta_rec( );
    VOID teste1( );
    VOID teste2( );
    VOID teste3( );
    VOID teste4( );
    VOID teste7( );
#endif




VOID teste_ctree()
{  TEXT aux[200];
   COUNT t;

   if ( param.totaliza ) {
      param.so_conta=0;
      param.delimita_key = 1;
   }

   while (1) {

      fprintf(stderr, "\n\n\nTeste a ser executado :\n");
      fprintf(stderr, "\t1 - Criacao de arquivo e registros-\n");
      fprintf(stderr, "\t2 - Dinamica de alocacao de espacos\n");
      fprintf(stderr, "\t3 - Add_rec e Find_rec\n");
      fprintf(stderr, "\t4 - adicao de indice\n");
      fprintf(stderr, "\t5 - pesquisa em indice\n");
      fprintf(stderr, "\t6 - Lista indice\n");
      fprintf(stderr, "\t7 - Indice exaustivo (5000)\n");
      fprintf(stderr, "\t. - Encerra\n");

      gets(aux);

      if (aux[0] == '.' )
         break;

      t = atoi(aux);

      switch(t) {
         case 1 : teste1(FILE_DATNO, FILE_KEYNO);
                  break;

         case 2 : teste2(FILE_DATNO, FILE_KEYNO);
                  break;

         case 3 : teste3(FILE_DATNO, FILE_KEYNO);
                  break;

         case 4 : teste4(FILE_DATNO, FILE_KEYNO);
                  break;

         case 5 : pesquisa(nomeind);
                  break;

         case 6 : lista_ind(nomeind, NULL);
                  break;

         case 7 : teste7(FILE_DATNO, FILE_KEYNO);
                  break;

         default: fprintf(stderr, "Opcao invalida\n\n\n");
                  break;
      }
   }


   return;
}



VOID teste1(m_filno, m_keyno)
COUNT m_filno;
COUNT m_keyno;
{  POINTER r1, r2, r3, r4, r5;

	abre_arq(nomearq, nomeind, 1, m_filno, m_keyno);

   r1 = grava_rec(m_filno, "Light Software S/C Ltda");
   r2 = grava_rec(m_filno, "Light Software S/C Ltdaxxxxxx");
   r3 = grava_rec(m_filno, "Light Software S/C Ltdaxxxxxxxxxxxxxxx");
   r4 = grava_rec(m_filno, "Light Software S/C Ltdaxxxxxxxxxxxxxxxxxxxxx");

   deleta_rec(m_filno, r3);
   deleta_rec(m_filno, r2);

   r2 = grava_rec(m_filno, "Light");
   r3 = grava_rec(m_filno, "Sciencia Informatica");
   r5 = grava_rec(m_filno, "Lynx informacoes");

   deleta_rec(m_filno, r5);
   deleta_rec(m_filno, r4);
   deleta_rec(m_filno, r3);
   deleta_rec(m_filno, r2);
   deleta_rec(m_filno, r1);

	fecha_arq(m_filno, m_keyno);

}

VOID teste2(m_filno, m_keyno)
COUNT m_filno;
COUNT m_keyno;
{  TEXT aux[20], *buf, *pr;
   LONG *pl;
   UCOUNT num, i, *nu, tam;


   fprintf(stderr, "Numero de registros a serem criados ? ");
   gets(aux);
   num = atoi(aux);

   fprintf(stderr, "Tamanho maximo dos regsitros ? ");
   gets(aux);
   tam = atoi(aux);

   buf = malloc( tam + 10 );
   if ( buf == NULL) {
      fprintf(stderr, "Sem memoria\n");
      exit(1);
   }
   memset(buf, 'X', tam + 10);
   buf[tam] = '\0';

   pl = (POINTER *)calloc(num, sizeof(POINTER));
   if ( pl == NULL) {
      fprintf(stderr, "Sem memoria\n");
      exit(1);
   }

   nu = (UCOUNT *)calloc(num, sizeof(UCOUNT));
   if ( nu == NULL) {
      fprintf(stderr, "Sem memoria\n");
      exit(1);
   }

   for (i = 0; i < num; i++)
      pl[i] = -1;

   srand( num );

	abre_arq(nomearq, nomeind, 1, m_filno, m_keyno);

   for (i = 0; i < num; i++) {
      nu[i] = lb_random( tam ); /* gera registro de no maximo n bytes */
      if ( nu[i] < 10) nu[i] = 10;

      pr = buf + tam - nu[i] + 1;
      pl[i] = grava_rec(m_filno, pr);
   }

   for (i = 0; i < num; i = i + 2) {
      deleta_rec(m_filno, pl[i]);
   }

   for (i = 0; i < num; i = i + 2) {
      nu[i] = lb_random( tam ); /* gera registro de no maximo n bytes */
      if ( nu[i] < 10) nu[i] = 10;
      pr = buf + tam - nu[i] + 1;
      pl[i] = grava_rec(m_filno, pr);
   }


   for (i = 0; i < num; i++) {
      deleta_rec(m_filno, pl[i]);
   }

	fecha_arq(m_filno, m_keyno);

}

VOID teste3(m_filno, m_keyno)
COUNT m_filno;
COUNT m_keyno;
{  TEXT chave[100];
   TEXT aux[100], buf[100];

	abre_arq(nomearq, nomeind, 1, m_filno, m_keyno);

   strcpy(aux, "Teste de registro");
   strcpy(chave, "ananias");

	if ( ! add_rec(m_filno, m_keyno, US_ID, chave, aux, strlen(aux))) {
      LOG_SL("Erro em add_rec : %d", uerr_cod);
      exit(3);
   }

	if ( load_rec(m_filno, m_keyno, US_ID, chave, buf, sizeof(buf), '=') == NULL  ) {
      LOG_SL("Erro em WRTVREC : %d", uerr_cod);
      exit(3);
   }


	fecha_arq(m_filno, m_keyno);

}

VOID teste4(m_filno, m_keyno)
COUNT m_filno;
COUNT m_keyno;
{
	TEXT linha[200];
   FILE *fp;
   LONG pos = 0;
   COUNT tam, ret;

	abre_arq(nomearq, nomeind, 1, m_filno, m_keyno);

   while (1) {
      pos++;
      printf("Informe palavra a ser indexada (@arquivo ou [.]) : ");
      gets(linha);
      if ( linha[0] == '@') {
      	if ( access(&linha[1], 0) == 0 ) {
            if ( (fp = fopen(&linha[1], "r")) == NULL ) {
               LOG_S("Erro na abertura");
            	fecha_arq(m_filno, m_keyno);
               exit(4);
            }

            while ( fgets(linha, MAX_CHAVE, fp) != NULL ) {
               linha[MAX_CHAVE] = '\0';
               tam = strlen(linha);
            	ret = ADDKEY(m_keyno, linha, pos, REGADD, tam+1);
	            if ( uerr_cod  != NO_ERROR ) {
                  LOG_SL("Erro em ADDKEY : %d", ret);
               	fecha_arq(m_filno, m_keyno);
                  exit(4);
               }
            }
         }
      }
      else {

         if ( linha[0] == '.' )
            break;

         linha[MAX_CHAVE] = '\0';
         tam = strlen(linha);
         ret = ADDKEY(m_keyno, linha, pos, REGADD, tam+1);
	      if ( uerr_cod  != NO_ERROR ) {
            LOG_SL("Erro em ADDKEY : %d", ret);
         	fecha_arq(m_filno, m_keyno);
            exit(4);
         }
      }
   }

   fecha_arq(m_filno, m_keyno);

}

VOID teste7(m_filno, m_keyno)
COUNT m_filno;
COUNT m_keyno;
{
	TEXT linha[200];
	TEXT old[200];
   LONG pos = 0, *pl;
   LONG x;
   COUNT tam, ret, reg, i;
   COUNT ini, fim, a, b;

	abre_arq(nomearq, nomeind, 1, m_filno, m_keyno);
   fprintf(stderr, "\n");

	fprintf(stderr, "Inicial : ");
   gets(linha);
   ini = atoi(linha);

   if ( ini == 0 )
      goto vai;

	fprintf(stderr, "Final : ");
   gets(linha);
   fim = atoi(linha);


   for ( reg = ini; reg < fim; reg++) {

		fprintf(stderr, "\r%d", reg);

      for (i=0; i < 20; i++ ) {
         linha[0] = 1;
         linha[1] = 1;
         linha[2] = i*10;
         linha[3] = 0;
         pl = (LONG *)&linha[4];
         linha[8] = 0;

         *pl = reg * 500;

         tam = 8;
         ret = ADDKEY(m_keyno, linha, pos, REGADD, tam+1);
	      if ( uerr_cod  != NO_ERROR ) {
            LOG_SL("Erro em ADDKEY : %d", ret);
         	fecha_arq(m_filno, m_keyno);
            exit(4);
         }
      }
   }

vai :

   fprintf(stderr, "\n");

   pos = FRSKEY(m_keyno, linha);
   strcpy(old, linha);

   x = 0;
   a=linha[2];
   b=0;

   while ( pos > 0 ) {
      if ( strcmp(old, linha) > 0 ) {
         printf("Maior %s  %s\n", old, linha);
      }
		fprintf(stderr, "\r** %ld", ++x);
		if ( a != linha[2] ) {
		   fprintf(stderr, "   a=%d   b = %d\n", a, b);
		   a = linha[2];
		   b = 1;
		}
		else
		  b++;

      pos = NXTKEY(m_keyno, linha);
	   strcpy(old, linha);

	   if ( linha[1] != 1 || linha[0] != 1 )
	      break;
   }

	fprintf(stderr, "\nAcabou\n");
	gets(linha);

   fecha_arq(m_filno, m_keyno);

}

VOID abre_arq(arq_dat, arq_ind, opcao, m_filno, m_keyno)
TEXT *arq_dat, *arq_ind;
COUNT opcao;
COUNT m_filno, m_keyno;
{  TEXT aux[10], cmd[100];
   UCOUNT id_arq;
   LONG serie;
   UCOUNT slot = 4;
   COUNT m_keylen = MAX_CHAVE,
         m_keytyp = 0,
         m_nomemb = 0;

	if ( opcao && access(arq_dat, 0) == 0 ) {
      fprintf(stderr, "Deseja remover arquivo ? ");
      gets(aux);
      if (aux[0] == 'S' || aux[0] == 's') {
#ifdef MSDOS
         sprintf(cmd, "del %s", arq_dat); system(cmd);
         sprintf(cmd, "del %s", arq_ind); system(cmd);
#else
         sprintf(cmd, "rm %s", arq_dat); system(cmd);
         sprintf(cmd, "rm %s", arq_ind); system(cmd);
#endif
      }
	}


   /* so para ver numero de serie e id_arq */
   serie = 0L;
   id_arq = 999;
   MANUT_OPEN( m_filno, arq_dat, (PERMANENT | SHARED ), &serie, &id_arq);



	if ( OPNFIL(m_filno, arq_dat, (PERMANENT | SHARED ), serie, id_arq) != NO_ERROR ) {

      serie = 0L;
      id_arq = 0;
      ct_sernum   = 0L;

		if ( CREDAT(m_filno, arq_dat, 0, slot, (PERMANENT | EXCLUSIVE | VLENGTH), serie, id_arq)) {

			fprintf(stderr, "Erro #%d na criacao do arquivo : %.14s\n",uerr_cod, arq_dat);
			exit(uerr_cod);
		}
		else {
			fprintf(stderr, "arquivo : %.14s criado OK\n",arq_dat );
		}

	}


   /* so para ver numero de serie e id_arq */
   serie = 0L;
   id_arq = 999;
   MANUT_OPEN( m_keyno, arq_ind, (PERMANENT | SHARED ), &serie, &id_arq);

	if ( OPNFIL(m_keyno, arq_ind, (PERMANENT | EXCLUSIVE ), serie, id_arq ) != NO_ERROR ) {

      serie = 0L;
      id_arq = 0;

		if ( CREIDX(m_keyno, arq_ind, m_keylen, m_keytyp, m_nomemb, slot * 4, (PERMANENT | EXCLUSIVE | VLENGTH), serie, id_arq )) {

			fprintf(stderr, "Erro #%d na criacao do arquivo : %.14s\n",uerr_cod, arq_ind);
			exit(uerr_cod);
		}
		else {
			fprintf(stderr, "arquivo : %.14s criado OK\n",arq_ind );
		}
	}
}

VOID fecha_arq(m_filno, m_keyno)
COUNT m_filno;
COUNT m_keyno;
{
	if ( CLSFIL(m_filno, COMPLETE) ) {
		fprintf(stderr, "Erro em CLSFIL ==> m_filno: %d    uerr_cod: %d\n", m_filno, uerr_cod);
		exit(1);
	}

	if ( CLSFIL(m_keyno, COMPLETE) ) {
		fprintf(stderr, "Erro em CLSFIL ==> m_filno: %d    uerr_cod: %d\n", m_keyno, uerr_cod);
		exit(1);
	}
}






POINTER grava_rec(datno, linha)
COUNT datno;
TEXT *linha;
{  COUNT tam_reg;
	POINTER recd;


	tam_reg = strlen(linha) + 1;

	if ( (recd = NEWVREC(datno, tam_reg)) == 0L ) {
		fprintf(stderr, "Nao alocou espaco para novo registro. Erro # %d\n",uerr_cod);
		exit(uerr_cod);
	}

	if ( WRTVREC(datno, recd, linha, tam_reg) ) {
		fprintf(stderr, "Nao gravou novo registro. Erro # %d\n",uerr_cod);
		exit(uerr_cod);
	}

   LOG_SLL("Gravou registro de %ld em pos = %ld\n", (LONG) tam_reg, (LONG) recd);

   return(recd);
}


POINTER deleta_rec(datno, recd)
COUNT datno;
POINTER recd;
{


   if ( RETVREC(datno, recd) ) {
		fprintf(stderr, "Nao liberou novo registro. Erro # %d\n",uerr_cod);
		return(1);
	}

   LOG_SL("Devolvendo registro de %ld\n", recd);

   return(NO_ERROR);

}



