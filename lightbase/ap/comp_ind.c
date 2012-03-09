#include  "lb2all.h"

#include  <string.h>
#include  <stdlib.h>

#include  "../ct/ctdefs.h"
#include  "../ct/cteproto.h"
#include  "../ct/cterrc.h"

VOID abre_arq(TEXT *, TEXT *);
VOID fecha_arq( VOID );
VOID lista( COUNT );
VOID pesq( COUNT );

#define   LB_KEYMAX    30  /* se mudar observar ctoptn.h */
#define   AI_INDICES      6

extern COUNT ct_lenkey;

long tot_ind;

#ifdef DEBUGEST
unsigned long  Xaddkey = 0,
               Xinsert = 0,
               Xaddroot = 0,
               Xnewnod = 0,
               Xinsexp = 0,
               Xnodserv = 0,
               Xgetnod  = 0,
               Xrednod = 0,
               Xgtroot = 0,
               Xvalpnt = 0,
               Xhghpnt = 0,
               Xnodpnt = 0,
               Xdrnpnt = 0,
               Xexpval = 0;
#endif


#define MAX_CHAVE   30
#define DUPL_OK 		1
#define TIPO_CHAVE	12
#define NUM_REGS     1000

#ifdef CTSERVER
#	include "ctcomm.h"
	extern UCOUNT  cts_apxsiz;			/* message size */
#endif

EXTERN COUNT uerr_cod;


UCOUNT slot = 4, nlin;
TEXT linha[300], temp[300], monta[100];
POINTER NEWVREC(), recd;
COUNT m_filno = 7, m_keyno = 0;

/*		m_keylen = MAX_CHAVE, m_keytyp = TIPO_CHAVE, m_keydup = DUPL_OK, m_nomemb = 4; */

int num_ind = 0, tam_ind = 0;
int flag_remove = 1;
COUNT indice;
COUNT teste(TEXT *, COUNT);

main(argc, argv)
int argc;
char *argv[];
{  COUNT num_indices, num;

	if ( argc < 4 ) {
		printf("Uso : comp_ind     arq_ori    arq_dest   num_indices\n");
	}

   num_indices = atoi(argv[3]);

   if ( num_indices != AI_INDICES ) {
      printf("Numero de indices invalidos\n");
      exit(1);
   }


	if ( INTREE(12, 16, 8) ) {
		printf("Erro \n");
		exit(1);
	}

	abre_arq(argv[1], argv[2]);

   for (num = 0; num < AI_INDICES; num++) {
      lista(num);
      printf("  Indices %d  Total de chaves : %ld\n\n", num, tot_ind);
   }

	fecha_arq();


   return(0);
}



VOID lista(ni)
COUNT ni;
{
	UTEXT  chave[50];
   COUNT tam;
	POINTER pos_arq;

   tot_ind = 0;

	pos_arq = FRSKEY(m_keyno + ni, (TEXT *)chave);

	if ( pos_arq == 0 ) {
		if ( uerr_cod ) {
			printf("Erro numero : %d em lista\n", uerr_cod);
			exit(uerr_cod);
		}
		else {
			printf("Indice #%d indice esta vazio !!!\n", ni);
		}
	}

	while ( pos_arq ) {

      tot_ind++;

      if ( tot_ind % 100 == 0 ) {
         printf("\r%ld   %30.30s", tot_ind, chave);
      }

      tam = ct_lenkey;

	   ADDKEY(m_filno + ni, (TEXT *)chave, pos_arq, INCADD, tam);

	   if ( uerr_cod  != NO_ERROR ) {
			printf("Erro [%d] na adicao de chave\n", uerr_cod);
         exit(1);
      }

#ifdef LIXO
...      if ( strlen(chave) == ct_lenkey ) {
...   		printf("%s\n", chave);
...      }
...      else {
...         for ( i = 0; i < ct_lenkey; i++ ) {
...            c = ct_indkey[i];
...            if ( c < 32 || c > 126 ) printf("%2.2x ", c);
...            else                     printf("%c  ", c);
...			}
...			printf ("\n");
...      }
#endif
		pos_arq = NXTKEY(m_keyno + ni, (TEXT *)chave);
	}
}





VOID abre_arq( arq_ind, arq_out)
TEXT *arq_ind;
TEXT *arq_out;
{  LONG num_serie;
   UCOUNT id_arq;
   COUNT num;
   COUNT varl = PERMANENT | EXCLUSIVE | VLENGTH;

   num_serie = 0L;
   id_arq = 999;


   MANUT_OPEN( m_keyno, arq_ind, 'I', &num_serie, &id_arq );

	if ( OPNFIL(m_keyno, arq_ind, (PERMANENT | EXCLUSIVE ),
               num_serie, id_arq ) != 0 ) {
		printf("Erro #%d na abertura do arquivo : %.14s\n",uerr_cod, arq_ind);
		exit(uerr_cod);
	}


   /**
   ***   CRIA ARQUIVO DE INDICE DOS DADOS
   **/
   if ( CREIDX(m_filno, arq_out, LB_KEYMAX, 0, AI_INDICES - 1,
           SLOT, varl, num_serie, id_arq ) != 0 ){
      printf("Erro na criacao do arquivo\n");
      exit(1);
   }
   else {
      /* cria outros indices do arquivo */
      for (num = 1; num < AI_INDICES; num++){
         if ( CREMEM(m_filno, LB_KEYMAX, 0,
                 num, num_serie, id_arq ) != 0 ) {
            printf("Erro na criacao de membro do arquivo\n");
            exit(1);
         }
      }
   }
}

VOID fecha_arq()
{
	if ( CLSFIL(m_keyno, COMPLETE) ) {
		printf("Erro em CLSFIL ==> m_keyno: %d    uerr_cod: %d\n", m_keyno, uerr_cod);
		exit(1);
	}
	if ( CLSFIL(m_filno, COMPLETE) ) {
		printf("Erro em CLSFIL ==> m_filno: %d    uerr_cod: %d\n", m_filno, uerr_cod);
		exit(1);
	}
}




