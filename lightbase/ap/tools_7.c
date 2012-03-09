#include  "lb2all.h"

#include <string.h>
#include  <time.h>
#include  <ctype.h>

#include  "lb2stru.h"
#include  "lb2files.h"
#include  "lb2bases.h"

#include "tools.h"

#ifdef MSDOS
#include <io.h>
#endif

static TEXT *nomearq="tables.dat";
static TEXT *nomeind="tables.idx";
static TEXT *nometab="tables.txt";

static COUNT m_datno, m_keyno;

#ifdef MSDOS
   VOID cria_tab(VOID);
   VOID lista_tab(VOID);
   COUNT mk_tabkey(COUNT, TEXT *, POINTER, TEXT *);
#else
   VOID cria_tab( );
   VOID lista_tab( );
   COUNT mk_tabkey( );
#endif



COUNT manut_tab()
{  TEXT aux[20];
   COUNT t;

   m_datno = FILE_DATNO;
   m_keyno = FILE_KEYNO;

   while (1) {
      fprintf(stderr, "\n\n\n\t\tManutencao de tabelas indexadas. Opcoes : \n");
      fprintf(stderr, "\t\t1 - Criar tabela\n");
      fprintf(stderr, "\t\t2 - Listar tabela\n");
      fprintf(stderr, "\t\t. - Encerra\n");
      fprintf(stderr, "\n\t\tFuncao a ser executada ? ");

      gets(aux);

      if (aux[0] == '.' )
         break;

      t = atoi(aux);

      switch(t) {
         case 1 : cria_tab();
                  break;

         case 2 : lista_tab();
                  break;

         default: fprintf(stderr, "Opcao invalida\n\n\n");
                  break;
      }
   }


   return(0);

}

VOID cria_tab()
{
   TEXT linha[300], chave[100], *p;
   FILE *fp=NULL;
   POINTER r1;
   COUNT num, tam;
   LONG lin = 0;

	abre_arq(nomearq, nomeind, 1, m_datno, m_keyno);

   if ( (fp = fopen(nometab, "r")) == NULL ) {
      LOG_S("Erro na abertura do arquivo : tables.txt");
      goto fim;
   }


   while ( fgets(linha, sizeof(linha) - 1, fp) != NULL ) {

   	if ( ++lin % 100 )
   		fprintf(stderr, "\r%ld", lin);

      strip_cr(linha);

      if ( linha[0] == '#' ) {

         if ( linha[1] == '#' )  /* comentarios */
            continue;

         num = atoi(&linha[1]);
         if ( num < 1 || num > 99 ) {
            LOG_S("Numero da tabela invalido\n");
         	fecha_arq(m_datno, m_keyno);
            goto fim;
         }
         continue;
      }

      if ( (p=strchr(linha, '|')) == NULL )
         continue;

      *p++ = '\0';
      strip_br(linha);

      while ( *p && (*p == ' ' || *p == '\t'))
         p++;

      if ( strlen(p) != 0 ) {
      	calado = 1;
      	r1 = grava_rec(m_datno, p);
      	calado = 0;
		   if ( uerr_cod != NO_ERROR  || r1 == 0L ) {
		      char mens[80];
	   	   sprintf(mens, "Erro %d na gravacao de registro.\n", uerr_cod);
         	LOG_S(mens);
      	}
      }
      else r1 = 0;

      p = linha;

      while ( *p && (*p == ' ' || *p == '\t'))
         p++;

      tam = strlen(p);

      if ( tam > 0 && tam < MAX_CHAVE )
      	tam = mk_tabkey(num, p, r1, chave);

      if ( tam > MAX_CHAVE ) {
         sprintf(linha, "\nATENCAO : Chave invalida [%s] tamanho %d\n", &chave[2], strlen(&chave[2]));
         LOG_S(linha);
         chave[tam] = '\0';
         tam = MAX_CHAVE;
      }

      ADDKEY(m_keyno, chave, 0L, REGADD, tam);

	   if ( uerr_cod  != NO_ERROR ) {
	      char mens[80];

	      if ( uerr_cod == 2 )
	         sprintf(mens, "Chave duplicada na tabela %d : %s\n", num, &chave[ 2 ]);
	      else
	      	sprintf(mens, "Erro %d na inclusao da chave %s\n", uerr_cod, chave);

         LOG_S(mens);
      }
   }

fim :

   if ( fp != NULL )
      fclose(fp);

   fecha_arq(m_datno, m_keyno);

}

COUNT mk_tabkey(num, ident, pos, chave)
COUNT num;
TEXT *ident;
POINTER pos;
TEXT *chave;
{     COUNT tam;

      sprintf(chave, "%02.2d%s", num, ident);
      tam = strlen(chave) + 1;
      sprintf(chave+tam, "%ld", pos);
      tam = tam + strlen(chave+tam) + 1;

      return(tam);
}


VOID lista_tab()
{
	TEXT chave[MAX_CHAVE], *p, aux[MAX_CHAVE+1], area[300];
	POINTER pos_arq, pos;
   COUNT num, num_ant = 0;

	abre_arq(nomearq, nomeind, 0, m_datno, m_keyno);

	pos_arq = FRSKEY(m_keyno, chave);

	if ( pos_arq == 0 ) {
		if ( uerr_cod ) {
         LOG_SL("Erro em FRSKEY : %d", uerr_cod);
      }
		else {
         LOG_S("Indice esta vazio !!!\n");
      }
	}


	while ( pos_arq ) {

      aux[0] = chave[0];
      aux[1] = chave[1];
      aux[2] = '\0';

      num = atoi(aux);

      if ( num != num_ant ) {
         printf("\n\n#%02.2d\n", num);
         num_ant = num;
      }

      p = strchr(chave, '\0');

      pos = atol(p+1);


	   if ( RDVREC(m_datno, pos, area, sizeof(area)) != NO_ERROR ) {
         LOG_SL("Erro : %d", uerr_cod);
         goto fim;
      }

      printf("%s  | %s\n", &chave[2], area);


		pos_arq = NXTKEY(m_keyno, chave);
		if ( uerr_cod ) {
         LOG_SL("Erro em NXTKEY : %d", uerr_cod);
         fecha_arq(m_datno, m_keyno);
         goto fim;
		}
	}

fim :

   fecha_arq(m_datno, m_keyno);
}




